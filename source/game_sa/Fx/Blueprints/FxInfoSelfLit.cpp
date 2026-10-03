#include "StdInc.h"

#include "FxInfoSelfLit.h"
#include "RenderInfo.h"

// 0x4A6760
FxInfoSelfLit_c::FxInfoSelfLit_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_SELFLIT_DATA;
    // Como el original: solo pone a 0 el byte bajo del contador (+0xE); no pide memoria al pool
    *reinterpret_cast<int8*>(&m_InterpInfo.m_nCount) = 0;
}

// 0x4A67B0
void FxInfoSelfLit_c::Load(FILESTREAM file, int32 version) {
    // NOP
}

// 0x4A67C0
void FxInfoSelfLit_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    static_cast<RenderInfo_t*>(info)->m_bSelfLit = true;
}
