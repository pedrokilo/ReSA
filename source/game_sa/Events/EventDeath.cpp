#include "StdInc.h"

#include "EventDeath.h"

void CEventDeath::InjectHooks() {
    RH_ScopedClass(CEventDeath);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4ADDF0);
}

// 0x4ADE50, `time` should be int32
CEventDeath::CEventDeath(bool bDrowning, uint32 deathTimeInMs) : CEvent() {
    m_bDrowning = bDrowning;
    m_deathTimeInMs = deathTimeInMs;
}

// 0x4ADDF0
CEventDeath::CEventDeath(bool bDrowning) : CEvent() {
    m_bDrowning = bDrowning;
    m_deathTimeInMs = CTimer::GetTimeInMS();
}
