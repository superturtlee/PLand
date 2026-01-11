#include "LandCreateValidator.h"
#include "pland/PLand.h"
#include "pland/aabb/LandAABB.h"
#include "pland/infra/Config.h"
#include "pland/land/LandRegistry.h"
#include "pland/utils/McUtils.h"

#include "ll/api/service/Bedrock.h"

#include "mc/world/level/Level.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/dimension/DimensionHeightRange.h"

#include "nonstd/expected.hpp"

namespace land {


ll::Expected<> LandCreateValidator::validateCreateOrdinaryLand(Player& player, SharedLand land) {
    if (auto res = isPlayerLandCountLimitExceeded(player.getUuid()); !res) {
        return res;
    }
    if (auto res = isLandRangeLegal(land->getAABB(), land->getDimensionId(), land->is3D()); !res) {
        return res;
    }
    if (auto res = isLandInForbiddenRange(land->getAABB(), land->getDimensionId()); !res) {
        return res;
    }
    if (auto res = isLandRangeConflict(land); !res) {
        return res;
    }
    return {};
}

ll::Expected<> LandCreateValidator::validateChangeLandRange(SharedLand land, LandAABB newRange) {
    if (auto res = isLandRangeLegal(newRange, land->getDimensionId(), land->is3D()); !res) {
        return res;
    }
    if (auto res = isLandInForbiddenRange(newRange, land->getDimensionId()); !res) {
        return res;
    }
    if (auto res = isLandRangeConflict(land, newRange); !res) {
        return res;
    }
    return {};
}

ll::Expected<> LandCreateValidator::validateCreateSubLand(Player& player, SharedLand land, LandAABB const& subRange) {
    if (auto res = isPlayerLandCountLimitExceeded(player.getUuid()); !res) {
        return res;
    }
    if (auto res = isLandRangeLegal(subRange, land->getDimensionId(), true); !res) {
        return res;
    }
    if (auto res = isSubLandPositionLegal(land, subRange); !res) {
        return res;
    }
    return {};
}


ll::Expected<> LandCreateValidator::isPlayerLandCountLimitExceeded(mce::UUID const& uuids) {
    auto& registry = PLand::getInstance().getLandRegistry();
    auto  count    = static_cast<int>(registry.getLands(uuids).size());
    auto& maxCount = Config::cfg.land.maxLand;

    // 非管理员 && 领地数量超过限制
    if (!registry.isOperator(uuids) && count >= Config::cfg.land.maxLand) {
        return makeError<LandCountExceeded>(count, maxCount);
    }
    return {};
}

ll::Expected<> LandCreateValidator::isLandInForbiddenRange(LandAABB const& range, LandDimid dimid) {
    for (auto const& forbiddenRange : Config::cfg.land.bought.forbiddenRanges) {
        if (forbiddenRange.dimensionId == dimid && LandAABB::isCollision(forbiddenRange.aabb, range)) {
            return makeError<LandInForbiddenRange>(range, forbiddenRange.aabb);
        }
    }
    return {};
}

ll::Expected<> LandCreateValidator::isLandRangeLegal(LandAABB const& range, LandDimid dimid, bool is3D) {
    auto const& squareRange = Config::cfg.land.bought.squareRange;

    auto const length = range.getBlockCountX();
    auto const width  = range.getBlockCountZ();
    auto const height = range.getBlockCountY();

    auto dimension = ll::service::getLevel()->getDimension(dimid).lock();
    if (!dimension) {
        return makeError<ValidateError>(ErrorCode::Undefined);
    }

    // 范围长度和宽度
    if (length < squareRange.min || width < squareRange.min) {
        return makeError<LandRangeError>(ErrorCode::LandRangeTooSmall, squareRange.min);
    }
    if (length > squareRange.max || width > squareRange.max) {
        return makeError<LandRangeError>(ErrorCode::LandRangeTooLarge, squareRange.max);
    }

    if (is3D) {
        // 校验维度范围
        auto& dimHeightRange = dimension->mHeightRange;
        if (range.min.y < dimHeightRange->mMin) {
            return makeError<LandHeightError>(
                ErrorCode::LandOutOfDimensionHeightRange,
                range.min.y,
                dimHeightRange->mMin
            );
        }
        if (range.max.y > dimHeightRange->mMax) {
            return makeError<LandHeightError>(
                ErrorCode::LandOutOfDimensionHeightRange,
                range.max.y,
                dimHeightRange->mMax
            );
        }
        // 校验业务允许范围
        if (height < squareRange.minHeight) {
            return makeError<LandHeightError>(ErrorCode::LandHeightTooSmall, height, squareRange.minHeight);
        }
    }

    return {};
}

ll::Expected<> LandCreateValidator::isLandRangeConflict(SharedLand const& land, std::optional<LandAABB> newRange) {
    return isLandRangeConflict(PLand::getInstance().getLandRegistry(), land, newRange);
}

ll::Expected<> LandCreateValidator::isLandRangeConflict(
    LandRegistry&           registry,
    SharedLand const&       land,
    std::optional<LandAABB> newRange
) {
    auto&       aabb       = newRange ? *newRange : land->getAABB();
    auto const& minSpacing = Config::cfg.land.minSpacing;
    auto        expanded   = aabb.expanded(minSpacing, Config::cfg.land.minSpacingIncludeY);
    auto        lands      = registry.getLandAt(expanded.min.as(), expanded.max.as(), land->getDimensionId());
    if (lands.empty()) {
        return {};
    }

    for (auto& ld : lands) {
        if (newRange && ld == land) {
            continue; // 仅在更改范围时排除自己
        }

        if (LandAABB::isCollision(ld->getAABB(), aabb)) {
            // 领地范围与其他领地冲突
            return makeError<LandRangeConflict>(aabb, ld);
        }
        if (!LandAABB::isComplisWithMinSpacing(ld->getAABB(), aabb, minSpacing)) {
            // 领地范围与其他领地间距过小
            return makeError<LandSpacingError>(LandAABB::getMinSpacing(ld->getAABB(), aabb), minSpacing, ld);
        }
    }
    return {};
}

ll::Expected<> LandCreateValidator::isSubLandPositionLegal(SharedLand const& land, LandAABB const& subRange) {
    // 子领地必须位于父领地内
    if (!LandAABB::isContain(land->getAABB(), subRange)) {
        return makeError<SubLandNotInParent>(land, subRange);
    }

    auto const& minSpacing = Config::cfg.land.subLand.minSpacing;
    bool const  includeY   = Config::cfg.land.subLand.minSpacingIncludeY;
    auto        expanded   = subRange.expanded(minSpacing, includeY);

    auto family  = land->getFamilyTree();       // 整个领地家族
    auto parents = land->getSelfAndAncestors(); // 相对于 land 的所有父领地

    // 子领地不能与家族内其他领地冲突
    for (auto& member : family) {
        if (member == land) {
            continue; // 排除自身(因为 sub 是 land 的子领地，所以 land 必然与 sub 冲突)
        }
        if (parents.contains(member)) {
            continue; // 排除父领地(因为 sub 是 land 的子领地，那么必然与整个家族内的父领地冲突)
        }

        auto& memberAABB = member->getAABB();

        if (LandAABB::isCollision(memberAABB, subRange)) {
            // 子领地与家族内其他领地冲突
            return makeError<LandRangeConflict>(subRange, member);
        }
        if (!LandAABB::isComplisWithMinSpacing(memberAABB, expanded, minSpacing, includeY)) {
            // 子领地与家族内其他领地间距过小
            return makeError<LandSpacingError>(LandAABB::getMinSpacing(memberAABB, expanded), minSpacing, member);
        }
    }
    return {};
}


LandCreateValidator::ValidateError::ValidateError(ErrorCode code) : code(code) {}
std::string LandCreateValidator::ValidateError::message() const noexcept {
    return "Land Create Validator Error: {}"_tr(magic_enum::enum_name(code));
}
void LandCreateValidator::ValidateError::sendTo(Player& player) const {
    mc_utils::sendText<mc_utils::LogLevel::Error>(player, translateError(player.getLocaleCode()));
}
using ll::operator""_trl;
std::string LandCreateValidator::ValidateError::translateError(std::string const& localeCode) const {
    return "验证失败，未知异常"_trl(localeCode);
}


LandCreateValidator::LandCountExceeded::LandCountExceeded(int count, int maxCount)
: ValidateError(ErrorCode::LandCountLimitExceeded),
  count(count),
  maxCount(maxCount) {}
std::string LandCreateValidator::LandCountExceeded::translateError(std::string const& localeCode) const {
    return "领地数量超过上限, 当前领地数量: {0}, 最大领地数量: {1}"_trl(localeCode, count, maxCount);
}


LandCreateValidator::LandInForbiddenRange::LandInForbiddenRange(LandAABB const& range, LandAABB const& forbiddenRange)
: ValidateError(ErrorCode::LandInForbiddenRange),
  range(range),
  forbiddenRange(forbiddenRange) {}
std::string LandCreateValidator::LandInForbiddenRange::translateError(std::string const& localeCode) const {
    return "领地范围在禁止区域内，当前范围: {0}, 禁止区域: {1}"_trl(
        localeCode,
        range.toString(),
        forbiddenRange.toString()
    );
}


LandCreateValidator::LandRangeError::LandRangeError(ErrorCode code, int limit)
: ValidateError(code),
  limitSize(limit) {}
std::string LandCreateValidator::LandRangeError::translateError(std::string const& localeCode) const {
    if (code == ErrorCode::LandRangeTooSmall) {
        return "领地范围过小，最小范围: {0}"_trl(localeCode, this->limitSize);
    } else if (code == ErrorCode::LandRangeTooLarge) {
        return "领地范围过大，最大范围: {0}"_trl(localeCode, this->limitSize);
    } else {
        return ValidateError::translateError(localeCode);
    }
}


LandCreateValidator::LandHeightError::LandHeightError(ErrorCode code, int actual, int limit)
: ValidateError(code),
  actualHeight(actual),
  limitHeight(limit) {}
std::string LandCreateValidator::LandHeightError::translateError(std::string const& localeCode) const {
    if (code == ErrorCode::LandOutOfDimensionHeightRange) {
        return "领地过高(维度高度)，当前高度: {0}, 最大高度: {1}(min/max)"_trl(localeCode, actualHeight, limitHeight);
    } else if (code == ErrorCode::LandHeightTooSmall) {
        return "领地高度过低 {0}<{1}"_trl(localeCode, this->actualHeight, this->limitHeight);
    } else {
        return ValidateError::translateError(localeCode);
    }
}


LandCreateValidator::LandRangeConflict::LandRangeConflict(LandAABB const& range, std::shared_ptr<Land> conflictLand)
: ValidateError(ErrorCode::LandRangeConflict),
  range(range),
  conflictLand(conflictLand) {}
std::string LandCreateValidator::LandRangeConflict::translateError(std::string const& localeCode) const {
    return "当前领地范围与领地 {0}({1}) 重叠，请调整领地范围!"_trl(
        localeCode,
        conflictLand->getName(),
        conflictLand->getId()
    );
}


LandCreateValidator::LandSpacingError::LandSpacingError(int spacing, int minSpacing, std::shared_ptr<Land> conflictLand)
: ValidateError(ErrorCode::LandSpacingTooSmall),
  spacing(spacing),
  minSpacing(minSpacing),
  conflictLand(conflictLand) {}
std::string LandCreateValidator::LandSpacingError::translateError(std::string const& localeCode) const {
    return "当前领地范围与领地 {0}({1}) 间距过小，请调整领地范围\n当前间距: {2}, 最小间距: {3}"_trl(
        localeCode,
        conflictLand->getName(),
        conflictLand->getId(),
        spacing,
        minSpacing
    );
}


LandCreateValidator::SubLandNotInParent::SubLandNotInParent(std::shared_ptr<Land> parent, LandAABB const& subRange)
: ValidateError(ErrorCode::SubLandOutOfParentLandRange),
  parent(parent),
  subRange(subRange) {}
std::string LandCreateValidator::SubLandNotInParent::translateError(std::string const& localeCode) const {
    return "子领地范围不在父领地范围内，当前范围: {0}, 父领地范围: {1}"_trl(
        localeCode,
        subRange.toString(),
        parent->getAABB().toString()
    );
}


} // namespace land
