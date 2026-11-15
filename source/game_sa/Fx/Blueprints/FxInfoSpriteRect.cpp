#include "StdInc.h"

#include "FxInfoSpriteRect.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoSpriteRect_c::InjectHooks() {
    RH_ScopedClass(FxInfoSpriteRect_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A60B0);
}

// 0x4A60B0
FxInfoSpriteRect_c::FxInfoSpriteRect_c() : FxInfo_c() {
    m_nType = FX_INFO_SPRITERECT_DATA;
    m_InterpInfo.Allocate(4);
}

// 0x4A60B0 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoSpriteRect_c* FxInfoSpriteRect_c::Constructor() {
    this->FxInfoSpriteRect_c::FxInfoSpriteRect_c();
    return this;
}

// 0x4A6120
void FxInfoSpriteRect_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A6140
void FxInfoSpriteRect_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[4];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_fSpriteTop    = values[0];
    render.m_fSpriteBottom = values[1];
    render.m_fSpriteLeft   = values[2];
    render.m_fSpriteRight  = values[3];
}
