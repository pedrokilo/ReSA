#include "StdInc.h"

#include "VehicleScanner.h"

// Lote 109A (banco Fn109A): GetClosestVehicleInRange (0x5FFEE0; thiscall, devuelve el dword +0x4C del escaner en EAX).
void CVehicleScannerInjectHooksLote109A() {
    RH_ScopedClass(CVehicleScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetClosestVehicleInRange, 0x5FFEE0);
}

// 0x5FFE60
void CVehicleScanner::ScanForVehiclesInRange(const CPed& ped) {
    if (!ped.IsAlive())
        return;

    if (ped.m_pVehicle && ped.bInVehicle) {
        if (ped.IsCreatedByMission())
            CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_VEHICLES, ped);

        CEntity::SafeCleanUpRef(m_pClosestEntityInRange);
        m_pClosestEntityInRange = ped.m_pVehicle;
        CEntity::SafeRegisterRef(m_pClosestEntityInRange);
    } else {
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_VEHICLES, ped);
    }
}

CVehicle* CVehicleScanner::GetClosestVehicleInRange() const {
    return static_cast<CVehicle*>(m_pClosestEntityInRange);
}
