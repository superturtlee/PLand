#pragma once
#include <memory>

#include "Global.h"
#include "ll/api/mod/NativeMod.h"

namespace ll {
namespace thread {
class ThreadPoolExecutor;
}
namespace data {
struct Version;
}
} // namespace ll

#ifdef LD_DEVTOOL
namespace devtool {
class DevToolApp;
}
#endif

namespace land {

class PLand {
    PLand();

public: /* private */
    [[nodiscard]] ll::mod::NativeMod& getSelf() const;

    bool load();
    bool enable();
    bool disable();
    bool unload();

public: /* public */
    LDAPI static PLand& getInstance();

    LDNDAPI class SafeTeleport*      getSafeTeleport() const;
    LDNDAPI class LandScheduler*     getLandScheduler() const;
    LDNDAPI class SelectorManager*   getSelectorManager() const;
    LDNDAPI class LandRegistry&      getLandRegistry() const;
    LDNDAPI class DrawHandleManager* getDrawHandleManager() const;

    LDNDAPI ll::thread::ThreadPoolExecutor* getThreadPool() const;

#ifdef LD_DEVTOOL
    [[nodiscard]] devtool::DevToolApp* getDevToolApp() const;
#endif

    LDNDAPI static ll::data::Version const& getVersion();

private:
    static void printLogo(ll::io::Logger& logger);
    void        checkVersion(ll::io::Logger& logger);

    struct Impl;
    std::unique_ptr<Impl> mImpl{nullptr};
};

} // namespace land
