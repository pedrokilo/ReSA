#include "StdInc.h"

#include "FxInfoUnderwater.h"
#include "MovementInfo.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoUnderwater_c::InjectHooks() {
    RH_ScopedClass(FxInfoUnderwater_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A5E20);
}

// 0x4A5E20
FxInfoUnderwater_c::FxInfoUnderwater_c() : FxInfo_c() {
    m_nType = FX_INFO_UNDERWATER_DATA;
    // Como el original: solo pone a 0 el byte bajo del contador (+0xE); no pide memoria al pool
    *reinterpret_cast<int8*>(&m_InterpInfo.m_nCount) = 0;
}

// 0x4A5E20 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoUnderwater_c* FxInfoUnderwater_c::Constructor() {
    this->FxInfoUnderwater_c::FxInfoUnderwater_c();
    return this;
}

void FxInfoUnderwater_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5E90
void FxInfoUnderwater_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) {
    static_cast<MovementInfo_t*>(info)->m_bHasUnderwaterInfo = true;
}
