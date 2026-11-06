#include "StdInc.h"
#include "Rubbish.h"

void CRubbish::InjectHooks() {
    RH_ScopedClass(CRubbish);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SetVisibility, 0x7204C0);
}

// 0x7204C0
void CRubbish::SetVisibility(bool unk) {
    // NOP
}
