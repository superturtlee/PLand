#pragma once
#include "pland/Global.h"

#include <ll/api/event/player/PlayerEvent.h>

namespace land::event {


class PlayerRequestCreateLandEvent final : public ll::event::PlayerEvent {
public:
    enum class Type { Ordinary, Sub };

    explicit PlayerRequestCreateLandEvent(Player& player, Type type);

    LDAPI Type type() const;

private:
    Type mType;
};


} // namespace land::event