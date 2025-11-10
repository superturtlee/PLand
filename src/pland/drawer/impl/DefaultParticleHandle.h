#pragma once
#include "IDrawerHandle.h"

#include <memory>


namespace land::drawer::detail {


class DefaultParticleHandle final : public IDrawerHandle {
    class Impl;
    std::unique_ptr<Impl> impl;

public:
    explicit DefaultParticleHandle();
    ~DefaultParticleHandle() override;

    GeoId draw(LandAABB const& aabb, DimensionType dimId, mce::Color const& color) override;

    void draw(std::shared_ptr<Land> const& land, mce::Color const& color) override;

    void remove(GeoId id) override;

    void remove(LandID landId) override;

    void remove(std::shared_ptr<Land> land) override;

    void clear() override;

    void clearLand() override;
};


} // namespace land::drawer::detail