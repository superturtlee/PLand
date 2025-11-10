#pragma once
#include "impl/IDrawerHandle.h"
#include "pland/Global.h"

#include <complex.h>
#include <memory>
#include <unordered_map>

class Player;

namespace land {

class DrawHandleManager final {
    std::unordered_map<mce::UUID, std::unique_ptr<drawer::IDrawerHandle>> mDrawHandles;

    std::unique_ptr<drawer::IDrawerHandle> createHandle() const;

public:
    LD_DISABLE_COPY_AND_MOVE(DrawHandleManager);
    explicit DrawHandleManager();
    ~DrawHandleManager();

public:
    LDNDAPI drawer::IDrawerHandle* getOrCreateHandle(Player& player);

    LDAPI void removeHandle(Player& player);

    LDAPI void removeAllHandle();
};


} // namespace land