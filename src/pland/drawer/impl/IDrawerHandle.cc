#include "IDrawerHandle.h"
#include "mc/world/actor/player/Player.h"

namespace land::drawer {

void IDrawerHandle::setTargetPlayer(Player& player) { mTargetPlayer = player.getWeakEntity(); }

Player* IDrawerHandle::getTargetPlayer() const { return mTargetPlayer.tryUnwrap<Player>().as_ptr(); }

} // namespace land::drawer