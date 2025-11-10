#pragma once
#include <cstdint>

namespace land {


enum class DrawerType : uint8_t {
    DefaultParticle = 0, // 默认粒子系统
    BSCI            = 1, // BedrockServerClientInterface  模组
    DebugShape      = 2  // DebugShape 调试形状
};


}