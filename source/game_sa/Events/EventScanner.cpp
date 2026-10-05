#include "StdInc.h"

#include "EventScanner.h"

// 0x605300
CEventScanner::CEventScanner() {
    // Como el original: los temporizadores quedan a cero (inicializadores de miembro); el escáner de atractores
    // empieza con field_0 = 1, efecto y field_14 nulos y se limpia; el de conocidos permite peatones de script
    m_attractorScanner.field_0       = 1;
    m_attractorScanner.m_pEffectInUse = nullptr;
    m_attractorScanner.field_14      = 0;
    m_attractorScanner.Clear();
    m_pedAcquaintanceScanner.m_bScanAllowedScriptPed    = true;
    m_pedAcquaintanceScanner.m_bScanAllowedInVehicle    = false;
    m_pedAcquaintanceScanner.m_bScanAllowedScriptedTask = false;
    // Como el original: tiempo - ftol((rand() & 0xFFFF) * (1/32768) * -3000), con productos redondeados a float
    const uint32 time = CTimer::GetTimeInMS();
    m_nNextScanTime = time - (uint32)(int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * 3.0517578125e-05f * -3000.0f);
}

void CEventScanner::Clear() {
    m_attractorScanner.Clear();
}

// 0x607E30
void CEventScanner::ScanForEvents(CPed& ped) {
    plugin::CallMethod<0x607E30, CEventScanner*, CPed&>(this, ped);
}

// 0x6053D0
void CEventScanner::ScanForEventsNow(const CPed& ped, bool bDontScan) {
    if (bDontScan)
        return;

    auto scanner = &m_vehiclePotentialCollisionScanner;
    if (scanner->m_timer.m_bStarted) { // todo: inlined?
        scanner->m_timer.m_nStartTime = CTimer::GetTimeInMS();
        scanner->m_timer.m_nInterval = -1;
        scanner->m_timer.m_bStarted = true;
    }
    scanner->ScanForVehiclePotentialCollisionEvents(ped, ped.GetIntelligence()->GetVehicleEntities(), 16);
}

// 0x603720
void CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents(const CPed& ped, CEntity** entities, int32 count) {
    plugin::CallMethod<0x603720, CVehiclePotentialCollisionScanner*, const CPed&, CEntity**, int32>(this, ped, entities, count);
}
