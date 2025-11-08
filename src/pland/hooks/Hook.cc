#include "pland/Global.h"
#include "pland/PLand.h"
#include "pland/hooks/EventListener.h"
#include "pland/hooks/listeners/ListenerHelper.h"
#include "pland/infra/Config.h"
#include "pland/land/LandRegistry.h"


#include "ll/api/event/EventBus.h"
#include "ll/api/event/entity/ActorHurtEvent.h"
#include "ll/api/memory/Hook.h"


#include "mc/server/ServerPlayer.h"
#include "mc/world/actor/ActorDamageSource.h"
#include "mc/world/actor/ActorType.h"
#include "mc/world/actor/FishingHook.h"
#include "mc/world/actor/Mob.h"
#include "mc/world/actor/ai/goal/LayEggGoal.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/block/FireBlock.h"
#include "mc/world/level/block/actor/ChestBlockActor.h"
namespace land {


// Fix [#140](https://github.com/engsr6982/PLand/issues/140)
LL_TYPE_INSTANCE_HOOK(
    MobHurtHook,
    HookPriority::Normal,
    Mob,
    &Mob::$_hurt,
    bool,
    ::ActorDamageSource const& source,
    float                      damage,
    bool                       knock,
    bool                       ignite
) {
    ll::event::ActorHurtEvent ev{*this, source, damage, knock, ignite};
    ll::event::EventBus::getInstance().publish(ev);
    if (ev.isCancelled()) {
        return false;
    }
    return origin(source, damage, knock, ignite);
}

// Fix [#56](https://github.com/engsr6982/PLand/issues/56)
LL_TYPE_INSTANCE_HOOK(
    FishingHookHitHook,
    ll::memory::HookPriority::Normal,
    ::FishingHook,
    &::FishingHook::_pullCloser,
    void,
    Actor& inEntity,
    float  inSpeed
) {
    // 获取钓鱼钩的位置和维度
    auto& hookActor = *this;
    auto& pos       = hookActor.getPosition();
    auto  dimId     = hookActor.getDimensionId();

    // 获取领地注册表实例
    auto* db   = PLand::getInstance().getLandRegistry();
    auto  land = db->getLandAt(pos, dimId);

    auto* player = hookActor.getPlayerOwner();
    if (!player) {
        origin(inEntity, inSpeed);
        return;
    }

    // 如果在领地内
    if (land) {
        // 检查玩家是否有权限
        if (!PreCheckLandExistsAndPermission(land, player->getUuid())) {
            // 领地不存在或玩家没有权限，则拦截
            return;
        }
        // 检查钓鱼竿权限
        if (!land->getPermTable().allowFishingRodAndHook) {
            // 如果不允许使用钓鱼竿，则拦截
            return;
        }
    }
    origin(inEntity, inSpeed);
}

// Fix [#69](https://github.com/engsr6982/PLand/issues/69)
LL_TYPE_INSTANCE_HOOK(
    LayEggGoalHook,
    ll::memory::HookPriority::Normal,
    ::LayEggGoal,
    &::LayEggGoal::$isValidTarget,
    bool,
    ::BlockSource&    region,
    ::BlockPos const& pos
) {
    // 获取领地注册表实例
    auto* db   = PLand::getInstance().getLandRegistry();
    auto  land = db->getLandAt(pos, region.getDimensionId());

    // 如果在领地内且不允许实体破坏，则阻止产蛋
    if (land && !land->getPermTable().allowActorDestroy) {
        return false;
    }
    return origin(region, pos);
}

// Fix [#136](https://github.com/engsr6982/PLand/issues/136)
LL_TYPE_INSTANCE_HOOK(
    FireBlockBurnHook,
    ll::memory::HookPriority::Normal,
    FireBlock,
    &FireBlock::checkBurn,
    void,
    ::BlockSource&    region,
    ::BlockPos const& pos,
    int               chance,
    ::Randomize&      randomize,
    int               age,
    ::BlockPos const& firePos
) {
    // 获取领地注册表实例
    auto* db   = PLand::getInstance().getLandRegistry();
    auto  land = db->getLandAt(pos, region.getDimensionId());

    // 如果在领地内且不允许火焰蔓延，则拦截
    if (land && !land->getPermTable().allowFireSpread) {
        return;
    }
    origin(region, pos, chance, randomize, age, firePos);
}



// Fix [#158](https://github.com/engsr6982/PLand/issues/158)
LL_TYPE_INSTANCE_HOOK(
    ChestBlockActorOpenHook,
    ll::memory::HookPriority::Normal,
    ChestBlockActor,
    &ChestBlockActor::$startOpen,
    void,
    ::Actor& actor
) {
    if (actor.isPlayer()) {
        origin(actor);
        return;
    }
    // 获取领地注册表实例
    auto* db   = PLand::getInstance().getLandRegistry();
    auto  land = db->getLandAt(this->mPosition, actor.getDimensionId());


    if (land && !land->getPermTable().allowOpenChest) {
        return;
    }
    origin(actor);
}

// impl EventListener
void EventListener::registerHooks() {
    RegisterHookIf<MobHurtHook>(Config::cfg.hooks.registerMobHurtHook);
    RegisterHookIf<FishingHookHitHook>(Config::cfg.hooks.registerFishingHookHitHook);
    RegisterHookIf<LayEggGoalHook>(Config::cfg.hooks.registerLayEggGoalHook);
    RegisterHookIf<FireBlockBurnHook>(Config::cfg.hooks.registerFireBlockBurnHook);
    RegisterHookIf<ChestBlockActorOpenHook>(Config::cfg.hooks.registerChestBlockActorOpenHook);
}


} // namespace land
