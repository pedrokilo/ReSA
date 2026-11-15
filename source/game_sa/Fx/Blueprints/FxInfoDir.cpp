#include "StdInc.h"

#include "FxInfoDir.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoDir_c::InjectHooks() {
    RH_ScopedClass(FxInfoDir_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A6440);
}

// 0x4A6440
FxInfoDir_c::FxInfoDir_c() : FxInfo_c() {
    m_nType = FX_INFO_DIR_DATA;
    m_InterpInfo.Allocate(3);
}

// 0x4A6440 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoDir_c* FxInfoDir_c::Constructor() {
    this->FxInfoDir_c::FxInfoDir_c();
    return this;
}

// 0x4A64B0
void FxInfoDir_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A64D0
void FxInfoDir_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[3];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_bHasDir = true;
    render.m_Direction = *(CVector*)&values[0];
    if (render.m_Direction.Magnitude() < 0.001f) {
        render.m_bUseVel = true;
    }
}
