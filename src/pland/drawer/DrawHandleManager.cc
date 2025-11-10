#include "DrawHandleManager.h"
#include "DrawerType.h"
#include "impl/BSCIHandle.h"
#include "impl/DebugShapeHandle.h"
#include "impl/DefaultParticleHandle.h"
#include "mc/world/actor/player/Player.h"
#include "pland/PLand.h"
#include "pland/infra/Config.h"
#include <memory>


namespace land {


DrawHandleManager::DrawHandleManager() {
    auto& logger = PLand::getInstance().getSelf().getLogger();
    switch (Config::cfg.land.drawHandleBackend) {
    case DrawerType::BSCI: {
        if (!drawer::detail::BSCIHandle::isBsciModuleLoaded()) {
            logger.warn(
                "[DrawHandleManager] The BSCI module is not loaded, and the plugin uses "
                "the built-in particle system!"
            );
            logger.warn("[DrawHandleManager] BSCI 模块未加载，插件将使用内置粒子系统!");
            Config::cfg.land.drawHandleBackend = DrawerType::DefaultParticle;
            Config::trySave();
        }
        break;
    }
    case DrawerType::DebugShape: {
        if (!drawer::detail::DebugShapeHandle::isDebugShapeLoaded()) {
            logger.warn(
                "[DrawHandleManager] The DebugShape module is not loaded, and the plugin uses the built-in particle "
                "system!"
            );
            logger.warn("[DrawHandleManager] DebugShape 模块未加载，插件将使用内置粒子系统!");
            Config::cfg.land.drawHandleBackend = DrawerType::DefaultParticle;
            Config::trySave();
        }
        break;
    }
    default:
        break;
    }
}

DrawHandleManager::~DrawHandleManager() = default;

std::unique_ptr<drawer::IDrawerHandle> DrawHandleManager::createHandle() const {
    switch (Config::cfg.land.drawHandleBackend) {
    case DrawerType::DefaultParticle:
        return std::make_unique<drawer::detail::DefaultParticleHandle>();
    case DrawerType::BSCI:
        return std::make_unique<drawer::detail::BSCIHandle>();
    case DrawerType::DebugShape:
        return std::make_unique<drawer::detail::DebugShapeHandle>();
    }
    throw std::runtime_error("Unknown drawer type");
}

drawer::IDrawerHandle* DrawHandleManager::getOrCreateHandle(Player& player) {
    auto iter = mDrawHandles.find(player.getUuid());
    if (iter == mDrawHandles.end()) {
        auto handle = createHandle();
        iter        = mDrawHandles.emplace(player.getUuid(), std::move(handle)).first;
    }
    return iter->second.get();
}

void DrawHandleManager::removeHandle(Player& player) { mDrawHandles.erase(player.getUuid()); }

void DrawHandleManager::removeAllHandle() { mDrawHandles.clear(); }


} // namespace land
