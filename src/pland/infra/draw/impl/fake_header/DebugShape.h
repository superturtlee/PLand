#pragma once
#include <mc/_HeaderOutputPredefine.h>
#include <mc/deps/core/math/Color.h>
#include <mc/world/phys/AABB.h>


class Player;

namespace debug_shape {


class IDrawer {
public:
    virtual ~IDrawer() = default;

    virtual void draw() const                        = 0; // sendToClients (all players)
    virtual void draw(Player& player) const          = 0; // sendToPlayer (only specific player)
    virtual void draw(DimensionType dimension) const = 0; // sendToDimension (only specific dimension)

    virtual void remove() const                        = 0;
    virtual void remove(Player& player) const          = 0;
    virtual void remove(DimensionType dimension) const = 0;

    virtual void update() const                        = 0;
    virtual void update(Player& player) const          = 0;
    virtual void update(DimensionType dimension) const = 0;
};


} // namespace debug_shape

namespace debug_shape::extension {

class IBoundsBox : public IDrawer {
public:
    // DSNDAPI static std::unique_ptr<IBoundsBox>
    // create(AABB const& bounds, mce::Color const& color = mce::Color::WHITE());

    virtual void setBounds(AABB const& bounds) = 0;

    virtual std::optional<Vec3> getRotation() const = 0; // 旋转

    virtual void setRotation(std::optional<Vec3> rot) = 0;

    virtual std::optional<mce::Color> getColor() const = 0; // 颜色

    virtual void setColor(std::optional<mce::Color> c) = 0;

    virtual bool hasDuration() const = 0; // 是否有持续时间

    virtual std::optional<float> getTotalTimeLeft() const = 0; // 剩余时间(s)

    virtual void setTotalTimeLeft(std::optional<float> t) = 0;

    // v1.21.120
    virtual DimensionType getDimensionId() const = 0;

    virtual void setDimensionId(DimensionType d) = 0;
};

} // namespace debug_shape::extension