#pragma once
#include "DefaultSelector.h"
#include "mc/world/actor/player/Player.h"
#include "pland/land/Land.h"
#include "pland/selector/DefaultSelector.h"
#include "pland/selector/ISelector.h"


namespace land {


DefaultSelector::DefaultSelector(Player& player, bool is3D) : ISelector(player, player.getDimensionId(), is3D) {}

SharedLand DefaultSelector::newLand() const {
    if (!isPointABSet()) {
        return nullptr;
    }

    auto player = getPlayer();
    if (!player) {
        return nullptr;
    }

    auto land = Land::make(*newLandAABB(), getDimensionId(), is3D(), player->getUuid());
    land->markDirty();
    return land;
}


} // namespace land