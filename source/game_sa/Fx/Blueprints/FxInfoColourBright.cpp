#include "StdInc.h"

#include "FxInfoColourBright.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoColourBright_c::InjectHooks() {
    RH_ScopedClass(FxInfoColourBright_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A67D0);
}

// 0x4A67D0
FxInfoColourBright_c::FxInfoColourBright_c() : FxInfo_c() {
    m_nType = FX_INFO_COLOURBRIGHT_DATA;
    m_InterpInfo.Allocate(5);
}

// 0x4A67D0 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoColourBright_c* FxInfoColourBright_c::Constructor() {
    this->FxInfoColourBright_c::FxInfoColourBright_c();
    return this;
}

// 0x4A6840
void FxInfoColourBright_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A6860
void FxInfoColourBright_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[5];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.SetColor(values[0], values[1], values[2], values[3]);
    render.m_Color2.alpha = (uint8)values[4];
    render.m_nColorType = ERenderColorType::BRIGHT;
}
