#pragma once
#include "LandContext.h"


namespace land {


class LandTemplatePermTable {
public:
    LDAPI explicit LandTemplatePermTable(LandPermTable permTable);

    LandPermTable const& get() const;

    void set(LandPermTable const& permTable);

    bool isDirty() const;
    void markDirty();
    void resetDirty();

private:
    std::atomic_bool mDirty{false};
    LandPermTable    mTemplatePermTable;
};


} // namespace land