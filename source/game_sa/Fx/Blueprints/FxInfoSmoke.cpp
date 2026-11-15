#include "StdInc.h"

#include "FxInfoSmoke.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoSmoke_c::InjectHooks() {
    RH_ScopedClass(FxInfoSmoke_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A68E0);
}

// 0x4A68E0
FxInfoSmoke_c::FxInfoSmoke_c() : FxInfo_c() {
    m_nType = FX_INFO_SMOKE_DATA;
    m_InterpInfo.Allocate(8);
}

// 0x4A68E0 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoSmoke_c* FxInfoSmoke_c::Constructor() {
    this->FxInfoSmoke_c::FxInfoSmoke_c();
    return this;
}

// 0x4A6950
void FxInfoSmoke_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A6970
void FxInfoSmoke_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[8];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);

    render.m_SmokeType = static_cast<int8>(values[0]);
    render.m_SmokeColor.red = values[1];
    render.m_SmokeColor.green = values[2];
    render.m_SmokeColor.blue = values[3];
    render.m_SmokeColor.alpha = values[4];
    render.m_SmokeBrightness = values[5];
    render.m_SmokeSize = values[6];
    render.m_SmokeLife = values[7];
}
