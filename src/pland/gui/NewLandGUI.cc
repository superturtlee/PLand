#include "NewLandGUI.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/ModalForm.h"

#include "mc/world/actor/player/Player.h"

#include "pland/Global.h"
#include "pland/PLand.h"
#include "pland/aabb/LandAABB.h"
#include "pland/infra/Config.h"
#include "pland/land/Land.h"
#include "pland/land/LandEvent.h"
#include "pland/land/LandRegistry.h"
#include "pland/selector/DefaultSelector.h"
#include "pland/selector/SelectorManager.h"
#include "pland/selector/SubLandSelector.h"
#include "pland/utils/McUtils.h"

#include <string>


using namespace ll::form;


namespace land {


void NewLandGUI::sendChooseLandDim(Player& player) {
    if (!std::ranges::contains(Config::cfg.land.bought.allowDimensions, player.getDimensionId().id)) {
        mc_utils::sendText(player, "你所在的维度无法购买领地"_trf(player));
        return;
    }

    PlayerAskCreateLandBeforeEvent ev(player);
    ll::event::EventBus::getInstance().publish(ev);
    if (ev.isCancelled()) {
        return;
    }

    ModalForm(
        PLUGIN_NAME + ("| 选择领地维度"_trf(player)),
        "请选择领地维度\n\n2D: 领地拥有整个Y轴\n3D: 自行选择Y轴范围"_trf(player),
        "2D", // true
        "3D"  // false
    )
        .sendTo(player, [](Player& pl, ModalFormResult res, FormCancelReason) {
            if (!res.has_value()) {
                return;
            }

            bool land3D = !((bool)res.value());
            if (land3D && !Config::cfg.land.bought.threeDimensionl.enabled) {
                mc_utils::sendText(pl, "3D领地功能未启用，请联系管理员"_trf(pl));
                return;
            }
            if (!land3D && !Config::cfg.land.bought.twoDimensionl.enabled) {
                mc_utils::sendText(pl, "2D领地功能未启用，请联系管理员"_trf(pl));
                return;
            }

            PlayerAskCreateLandAfterEvent ev(pl, land3D);
            ll::event::EventBus::getInstance().publish(ev);

            auto selector = std::make_unique<DefaultSelector>(pl, land3D);
            if (land::PLand::getInstance().getSelectorManager()->startSelection(std::move(selector))) {
                mc_utils::sendText(
                    pl,
                    "选区功能已开启，使用命令 /pland set 或使用 {} 来选择ab点"_trf(pl, Config::cfg.selector.tool)
                );
            } else {
                mc_utils::sendText(pl, "选区开启失败，当前存在未完成的选区任务"_trf(pl));
            }
        });
}


void NewLandGUI::sendConfirmPrecinctsYRange(Player& player, std::string const& exception) {
    auto selector = land::PLand::getInstance().getSelectorManager()->getSelector(player);
    if (!selector) {
        return;
    }
    CustomForm fm(PLUGIN_NAME + ("| 确认Y轴范围"_trf(player)));

    fm.appendLabel("确认选区的Y轴范围\n您可以在此调节Y轴范围，如果不需要修改，请直接点击提交"_trf(player));

    SubLandSelector* subSelector = nullptr;
    SharedLand       parentLand  = nullptr;
    if (subSelector = selector->as<SubLandSelector>(); subSelector) {
        if (parentLand = subSelector->getParentLand(); parentLand) {
            auto& aabb = parentLand->getAABB();
            fm.appendLabel(
                "当前为子领地模式，子领地的Y轴范围不能超过父领地。\n父领地Y轴范围: {} ~ {}"_trf(
                    player,
                    aabb.min.y,
                    aabb.max.y
                )
            );
        }
    }

    fm.appendInput("start", "开始Y轴"_trf(player), "int", std::to_string(selector->getPointA()->y));
    fm.appendInput("end", "结束Y轴"_trf(player), "int", std::to_string(selector->getPointB()->y));

    fm.appendLabel(exception);

    fm.sendTo(player, [selector, subSelector, parentLand](Player& pl, CustomFormResult res, FormCancelReason) {
        if (!res.has_value()) {
            return;
        }

        std::string start = std::get<std::string>(res->at("start"));
        std::string end   = std::get<std::string>(res->at("end"));

        try {
            int64_t startY = std::stoll(start);
            int64_t endY   = std::stoll(end);
            if (startY >= INT32_MAX || startY <= INT32_MIN || endY >= INT32_MAX || endY <= INT32_MIN) {
                sendConfirmPrecinctsYRange(pl, "数值过大，请输入正确的Y轴范围"_trf(pl));
                return;
            }

            if (startY >= endY) {
                sendConfirmPrecinctsYRange(pl, "请输入正确的Y轴范围, 开始Y轴必须小于结束Y轴"_trf(pl));
                return;
            }

            if (subSelector) {
                if (!subSelector || !parentLand) {
                    throw std::runtime_error("subSelector or parentLand is nullptr");
                }

                auto& aabb = parentLand->getAABB();
                if (startY < aabb.min.y || endY > aabb.max.y) {
                    sendConfirmPrecinctsYRange(pl, "请输入正确的Y轴范围, 子领地的Y轴范围不能超过父领地"_trf(pl));
                    return;
                }
            }

            selector->setYRange(startY, endY);
            selector->onPointConfirmed();
        } catch (...) {
            sendConfirmPrecinctsYRange(pl, "处理失败,请输入正确的Y轴范围"_trf(pl));
        }
    });
}


} // namespace land
