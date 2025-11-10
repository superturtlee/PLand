#pragma once
#include "IDrawerHandle.h"

namespace land {
class LandPos;
}
class AABB;

namespace land ::drawer::detail {

class BSCIHandle final : public IDrawerHandle {
    class Impl;
    std::unique_ptr<Impl> impl;

public:
    explicit BSCIHandle();
    ~BSCIHandle() override;

    GeoId draw(LandAABB const& aabb, DimensionType dimId, mce::Color const& color) override;

    void draw(std::shared_ptr<Land> const& land, mce::Color const& color) override;

    void remove(GeoId id) override;

    void remove(LandID landId) override;

    void remove(std::shared_ptr<Land> land) override;

    void clear() override;

    void clearLand() override;

    AABB fixAABB(LandPos const& min, LandPos const& max);

    AABB fixAABB(LandAABB const& aabb);

    static bool isBsciModuleLoaded();
};


} // namespace land::drawer::detail