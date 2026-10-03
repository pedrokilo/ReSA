#include "StdInc.h"

#include "FxInfoUnderwater.h"
#include "MovementInfo.h"

// 0x4A5E20
FxInfoUnderwater_c::FxInfoUnderwater_c() : FxInfo_c() {
    m_nType = FX_INFO_UNDERWATER_DATA;
    // Como el original: solo pone a 0 el byte bajo del contador (+0xE); no pide memoria al pool
    *reinterpret_cast<int8*>(&m_InterpInfo.m_nCount) = 0;
}

void FxInfoUnderwater_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5E90
void FxInfoUnderwater_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) {
    static_cast<MovementInfo_t*>(info)->m_bHasUnderwaterInfo = true;
}
