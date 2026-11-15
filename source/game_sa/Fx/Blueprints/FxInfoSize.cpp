#include "StdInc.h"

#include "FxInfoSize.h"
#include "RenderInfo.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoSize_c::InjectHooks() {
    RH_ScopedClass(FxInfoSize_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A5FA0);
}

// 0x4A5FA0
FxInfoSize_c::FxInfoSize_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_SIZE_DATA;
    m_InterpInfo.Allocate(4);
}

// 0x4A5FA0 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoSize_c* FxInfoSize_c::Constructor() {
    this->FxInfoSize_c::FxInfoSize_c();
    return this;
}

// 0x4A6010
void FxInfoSize_c::Load(FILESTREAM file, int32 version) {
    if (version < 105) {
        m_InterpInfo.m_nCount = 2;
    }
    m_InterpInfo.Load(file);
}

// 0x4A6030
void FxInfoSize_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[4];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_fSizeX = values[0];
    render.m_fSizeY = values[1];

    if (m_InterpInfo.m_nCount == 4) {
        render.m_fSizeXBias = values[2];
        render.m_fSizeYBias = values[3];
    } else {
        render.m_fSizeXBias = 0.0f;
        render.m_fSizeYBias = 0.0f;
    }
}
