#include "StdInc.h"
#include "PlaneBanners.h"

void CPlaneBanners::InjectHooks() {
    RH_ScopedClass(CPlaneBanners);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x717820);
}

// 0x717820
void CPlaneBanners::Init() {
    // NOP
}

// 0x717850
void CPlaneBanners::RegisterPoint(CVector point, uint32 bannerIndex) {
    // NOP
}

// 0x717830
void CPlaneBanners::Render() {
    // NOP
}

// 0x717840
void CPlaneBanners::Update() {
    // NOP
}
