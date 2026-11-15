#include "StdInc.h"

#include "FxInfoAnimTexture.h"
#include "RenderInfo.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoAnimTexture_c::InjectHooks() {
    RH_ScopedClass(FxInfoAnimTexture_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A6550);
}

// 0x4A6550
FxInfoAnimTexture_c::FxInfoAnimTexture_c() : FxInfo_c() {
    m_nType = FX_INFO_ANIMTEX_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A6550 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoAnimTexture_c* FxInfoAnimTexture_c::Constructor() {
    this->FxInfoAnimTexture_c::FxInfoAnimTexture_c();
    return this;
}

// 0x4A65C0
void FxInfoAnimTexture_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A65E0
void FxInfoAnimTexture_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[16];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_bHasAnimTextures = true;
    render.m_nCurrentTexId = static_cast<int8>(values[0]);
}
