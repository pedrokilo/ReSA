#include "StdInc.h"
#include "MentalHealth.h"

// Como el original: operacion local; 0x421050 recibe CPedIntelligence*, no este this.
void CMentalState::IncrementAnger(int32 anger) {
    if (!m_AngerTimer.m_bStarted) return;
    const uint32 now = CTimer::GetTimeInMS();
    if (m_AngerTimer.m_bStopped) {
        m_AngerTimer.m_nStartTime = now;
        m_AngerTimer.m_bStopped = false;
    }
    const uint32 expires = m_AngerTimer.m_nStartTime + static_cast<uint32>(m_AngerTimer.m_nInterval);
    if (now < expires) return;
    m_AngerTimer.m_nStartTime = now;
    m_AngerTimer.m_nInterval = 3000;
    m_AngerTimer.m_bStarted = true;
    m_AngerAtPlayer = static_cast<uint8>(m_AngerAtPlayer + static_cast<uint8>(anger));
}

// 0x6008A0
void CMentalState::Process() {
    plugin::CallMethod<0x6008A0, CMentalState*>(this);
}
