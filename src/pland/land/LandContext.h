#pragma once
#include "pland/Global.h"
#include "pland/aabb/LandAABB.h"
#include <vector>


namespace land {


struct LandPermTable {
    // === 基础权限 ===
    bool allowDestroy{false};  // 破坏方块
    bool allowPlace{false};    // 放置方块
    bool allowInteract{false}; // 基础交互(开箱子、使用按钮等)

    // === 战斗权限 ===
    bool allowPlayerDamage{false};        // 玩家PVP
    bool allowMonsterDamage{true};        // 攻击敌对生物
    bool allowPassiveDamage{false};       // 攻击友好生物
    bool allowSpecialDamage{false};       // 攻击特殊实体(船、矿车、画等)
    bool allowCustomSpecialDamage{false}; // 攻击自定义特殊实体

    // === 物品权限 ===
    bool allowPickupItem{false};       // 拾取物品
    bool allowDropItem{true};          // 丢弃物品
    bool allowProjectileCreate{false}; // 创建弹射物

    // === 实体权限 ===
    bool allowRideEntity{false};     // 骑乘实体
    bool allowInteractEntity{false}; // 实体交互
    bool allowActorDestroy{false};   // 实体破坏

    // === 工具权限 ===
    bool useTools{false};               // 使用工具(斧头去皮、锄头、铲子、骨粉等)
    bool useBucket{false};              // 使用桶
    bool allowFishingRodAndHook{false}; // 使用钓鱼竿

    // === 功能方块权限 ===
    bool useFurnaces{false};       // 使用熔炉类(熔炉、高炉、烟熏炉)
    bool useCraftingBlocks{false}; // 使用工作台类(工作台、制图台、锻造台、砂轮、附魔台等)
    bool useStorageBlocks{false};  // 使用存储类(箱子、木桶、漏斗、潜影盒等)
    bool useRedstoneBlocks{false}; // 使用红石类(比较器、中继器、发射器、投掷器等)
    bool useUtilityBlocks{false};  // 使用功能类(信标、讲台、炼药锅、重生锚等)

    // === 装饰交互权限 ===
    bool useDecorative{false}; // 使用装饰类(音符盒、唱片机、物品展示框、盔甲架、花盆等)
    bool useMovement{false};   // 使用移动类(门、活板门、栅栏门、压力板等)
    bool editSigns{false};     // 编辑告示牌
    bool editFlowerPot{false}; // 编辑花盆

    // === 特殊权限 ===
    bool useBed{false};               // 使用床
    bool useCake{false};              // 吃蛋糕
    bool placeVehicles{false};        // 放置载具(船、矿车)
    bool allowAttackDragonEgg{false}; // 点击龙蛋

    // === 环境权限 ===
    bool allowFireSpread{true};           // 火焰蔓延
    bool allowFarmDecay{true};            // 耕地退化
    bool allowPistonPushOnBoundary{true}; // 活塞推动
    bool allowRedstoneUpdate{true};       // 红石更新
    bool allowExplode{false};             // 爆炸
    bool allowBlockFall{false};           // 方块掉落
    bool allowWitherDestroy{false};       // 凋零破坏
    bool allowLiquidFlow{true};           // 液体流动
    bool allowSculkBlockGrowth{true};     // 幽匿尖啸体生长
    bool allowMonsterSpawn{true};         // 怪物生成
    bool allowAnimalSpawn{true};          // 动物生成
    bool allowEndermanLeaveBlock{false};  // 末影人放下方块

    // === 兼容性映射(废弃，保留用于数据迁移) ===
    bool allowOpenChest{false};       // -> useStorageBlocks
    bool allowAxePeeled{false};       // -> useTools
    bool allowRideTrans{false};       // -> allowRideEntity
    bool useAnvil{false};             // -> useCraftingBlocks
    bool useBarrel{false};            // -> useStorageBlocks
    bool useBeacon{false};            // -> useUtilityBlocks
    bool useBell{false};              // -> useDecorative
    bool useBlastFurnace{false};      // -> useFurnaces
    bool useBrewingStand{false};      // -> useCraftingBlocks
    bool useCampfire{false};          // -> useUtilityBlocks
    bool useFlintAndSteel{false};     // -> useTools
    bool useCartographyTable{false};  // -> useCraftingBlocks
    bool useComposter{false};         // -> useUtilityBlocks
    bool useCraftingTable{false};     // -> useCraftingBlocks
    bool useDaylightDetector{false};  // -> useRedstoneBlocks
    bool useDispenser{false};         // -> useRedstoneBlocks
    bool useDropper{false};           // -> useRedstoneBlocks
    bool useEnchantingTable{false};   // -> useCraftingBlocks
    bool useDoor{false};              // -> useMovement
    bool useFenceGate{false};         // -> useMovement
    bool useFurnace{false};           // -> useFurnaces
    bool useGrindstone{false};        // -> useCraftingBlocks
    bool useHopper{false};            // -> useStorageBlocks
    bool useJukebox{false};           // -> useDecorative
    bool useLoom{false};              // -> useCraftingBlocks
    bool useStonecutter{false};       // -> useCraftingBlocks
    bool useNoteBlock{false};         // -> useDecorative
    bool useCrafter{false};           // -> useCraftingBlocks
    bool useChiseledBookshelf{false}; // -> useStorageBlocks
    bool useComparator{false};        // -> useRedstoneBlocks
    bool useRepeater{false};          // -> useRedstoneBlocks
    bool useShulkerBox{false};        // -> useStorageBlocks
    bool useSmithingTable{false};     // -> useCraftingBlocks
    bool useSmoker{false};            // -> useFurnaces
    bool useTrapdoor{false};          // -> useMovement
    bool useLectern{false};           // -> useUtilityBlocks
    bool useCauldron{false};          // -> useUtilityBlocks
    bool useLever{false};             // -> useRedstoneBlocks
    bool useButton{false};            // -> useRedstoneBlocks
    bool useRespawnAnchor{false};     // -> useUtilityBlocks
    bool useItemFrame{false};         // -> useDecorative
    bool usePressurePlate{false};     // -> useMovement
    bool useArmorStand{false};        // -> useDecorative
    bool useBoneMeal{false};          // -> useTools
    bool useHoe{false};               // -> useTools
    bool useShovel{false};            // -> useTools
    bool useVault{false};             // -> useStorageBlocks
    bool useBeeNest{false};           // -> useStorageBlocks
    bool placeBoat{false};            // -> placeVehicles
    bool placeMinecart{false};        // -> placeVehicles
    bool editSign{false};             // -> editSigns
};


// ! 注意：如果 LandContext 有更改，则必须递增 LandContextVersion，否则导致加载异常
constexpr int LandContextVersion = 26;
struct LandContext {
    int                      version{LandContextVersion}; // 版本号
    LandAABB                 mPos{};                      // 领地对角坐标
    LandPos                  mTeleportPos{};              // 领地传送坐标
    LandID                   mLandID{LandID(-1)};         // 领地唯一ID  (由 LandRegistry::addLand() 时分配)
    LandDimid                mLandDimid{};                // 领地所在维度
    bool                     mIs3DLand{};                 // 是否为3D领地
    LandPermTable            mLandPermTable{};            // 领地权限
    std::string              mLandOwner{};                // 领地主人(默认UUID,其余情况看mOwnerDataIsXUID)
    std::vector<std::string> mLandMembers{};              // 领地成员
    std::string              mLandName{"Unnamed territories"_tr()}; // 领地名称
    std::string              mLandDescribe{"No description"_tr()};  // 领地描述
    int                      mOriginalBuyPrice{0};                  // 原始购买价格
    bool                     mIsConvertedLand{false}; // 是否为转换后的领地(其它插件创建的领地)
    bool mOwnerDataIsXUID{false}; // 领地主人数据是否为XUID (如果为true，则主人上线自动转换为UUID)
    LandID              mParentLandID{LandID(-1)}; // 父领地ID
    std::vector<LandID> mSubLandIDs{};             // 子领地ID
};

STATIC_ASSERT_AGGREGATE(LandPermTable);
STATIC_ASSERT_AGGREGATE(LandContext);
template <typename T, typename I>
concept AssertPosField = requires(T const& t, I const& i) {
    { t.min } -> std::convertible_to<I>;
    { t.max } -> std::convertible_to<I>;
    { i.x } -> std::convertible_to<int>;
    { i.y } -> std::convertible_to<int>;
    { i.z } -> std::convertible_to<int>;
};
static_assert(
    AssertPosField<LandAABB, LandPos>,
    "If the field is changed, please update the data transformation rules accordingly."
);

} // namespace land