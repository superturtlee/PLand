#include "LandTemplatePermTable.h"

namespace land {


LandTemplatePermTable::LandTemplatePermTable(LandPermTable permTable) : mTemplatePermTable(permTable) {}

LandPermTable const& LandTemplatePermTable::get() const { return mTemplatePermTable; }

void LandTemplatePermTable::set(LandPermTable const& permTable) {
    mTemplatePermTable = permTable;
    markDirty();
}
bool LandTemplatePermTable::isDirty() const { return mDirty; }
void LandTemplatePermTable::markDirty() { mDirty = true; }
void LandTemplatePermTable::resetDirty() { mDirty = false; }


} // namespace land