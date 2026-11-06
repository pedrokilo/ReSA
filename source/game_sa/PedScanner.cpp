#include "StdInc.h"
#include "PedScanner.h"

// Lote 109A (banco Fn109A): GetClosestPedInRange (0x5FFF20; thiscall, devuelve el dword +0x4C del escaner en EAX).
void CPedScannerInjectHooksLote109A() {
    RH_ScopedClass(CPedScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetClosestPedInRange, 0x5FFF20);
}

// 0x5FFEF0
void CPedScanner::ScanForPedsInRange(const CPed& ped) {
    if (ped.IsAlive())
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_PEDS, ped);
}

// 0x5FFF20
CPed* CPedScanner::GetClosestPedInRange() const {
    return static_cast<CPed*>(m_pClosestEntityInRange);
}
