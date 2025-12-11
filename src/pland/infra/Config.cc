#include "pland/infra/Config.h"
#include "ll/api/Config.h"
#include "pland/PLand.h"
#include <filesystem>
#include <string>
#include <unordered_map>


namespace land {

namespace fs = std::filesystem;
#define CONFIG_FILE "Config.json"

bool Config::tryLoad() {
    auto dir = land::PLand::getInstance().getSelf().getConfigDir() / CONFIG_FILE;

    if (!std::filesystem::exists(dir)) {
        trySave();
    }

    bool status = ll::config::loadConfig(Config::cfg, dir);

    return status ? status : trySave();
}

bool Config::trySave() {
    auto dir = land::PLand::getInstance().getSelf().getConfigDir() / CONFIG_FILE;

    bool status = ll::config::saveConfig(cfg, dir);

    return status;
}


Config Config::cfg = [] {
    Config c;
    c.protection.permissionMaps.itemSpecific = {
        // 基础权限 - 放置物品
        {          "minecraft:skull", "allowPlace"},
        {         "minecraft:banner", "allowPlace"},
        {   "minecraft:glow_ink_sac", "allowPlace"},
        {    "minecraft:end_crystal", "allowPlace"},
        {      "minecraft:ender_eye", "allowPlace"},
        {    "minecraft:armor_stand", "allowPlace"},
        // 工具权限
        {"minecraft:flint_and_steel",   "useTools"},
        {      "minecraft:bone_meal",   "useTools"}
    };
    c.protection.permissionMaps.blockSpecific = {
        // 特殊权限
        {                "minecraft:dragon_egg", "allowAttackDragonEgg"},
        {                       "minecraft:bed",               "useBed"},
        {                      "minecraft:cake",              "useCake"},
        // 存储类方块
        {                     "minecraft:chest",     "useStorageBlocks"},
        {             "minecraft:trapped_chest",     "useStorageBlocks"},
        // 功能类方块
        {                  "minecraft:campfire",     "useUtilityBlocks"},
        {             "minecraft:soul_campfire",     "useUtilityBlocks"},
        {                 "minecraft:composter",     "useUtilityBlocks"},
        {                   "minecraft:lectern",     "useUtilityBlocks"},
        {                  "minecraft:cauldron",     "useUtilityBlocks"},
        {            "minecraft:respawn_anchor",     "useUtilityBlocks"},
        // 装饰类方块
        {                 "minecraft:noteblock",        "useDecorative"},
        {                   "minecraft:jukebox",        "useDecorative"},
        {                      "minecraft:bell",        "useDecorative"},
        // 红石类方块
        {"minecraft:daylight_detector_inverted",    "useRedstoneBlocks"},
        {         "minecraft:daylight_detector",    "useRedstoneBlocks"},
        // 编辑类
        {                "minecraft:flower_pot",        "editFlowerPot"},
        // 基础破坏权限
        {          "minecraft:sweet_berry_bush",         "allowDestroy"}
    };
    c.protection.permissionMaps.blockFunctional = {
        // 工作台类方块
        {   "minecraft:cartography_table", "useCraftingBlocks"},
        {      "minecraft:smithing_table", "useCraftingBlocks"},
        {       "minecraft:brewing_stand", "useCraftingBlocks"},
        {               "minecraft:anvil", "useCraftingBlocks"},
        {          "minecraft:grindstone", "useCraftingBlocks"},
        {    "minecraft:enchanting_table", "useCraftingBlocks"},
        {                "minecraft:loom", "useCraftingBlocks"},
        {   "minecraft:stonecutter_block", "useCraftingBlocks"},
        {             "minecraft:crafter", "useCraftingBlocks"},
        // 存储类方块
        {              "minecraft:barrel",  "useStorageBlocks"},
        {              "minecraft:hopper",  "useStorageBlocks"},
        {  "minecraft:chiseled_bookshelf",  "useStorageBlocks"},
        {            "minecraft:bee_nest",  "useStorageBlocks"},
        {             "minecraft:beehive",  "useStorageBlocks"},
        {               "minecraft:vault",  "useStorageBlocks"},
        // 红石类方块
        {             "minecraft:dropper", "useRedstoneBlocks"},
        {           "minecraft:dispenser", "useRedstoneBlocks"},
        {"minecraft:unpowered_comparator", "useRedstoneBlocks"},
        {  "minecraft:powered_comparator", "useRedstoneBlocks"},
        {  "minecraft:unpowered_repeater", "useRedstoneBlocks"},
        {    "minecraft:powered_repeater", "useRedstoneBlocks"},
        // 功能类方块
        {              "minecraft:beacon",  "useUtilityBlocks"}
    };
    return c;
}();

} // namespace land
