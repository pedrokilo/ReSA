#include "StdInc.h"

#include "FxInfoJitter.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoJitter_c::InjectHooks() {
    RH_ScopedClass(FxInfoJitter_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A5B20);
}

// 0x4A5B20
FxInfoJitter_c::FxInfoJitter_c() : FxInfo_c() {
    m_nType = FX_INFO_JITTER_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A5B20 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoJitter_c* FxInfoJitter_c::Constructor() {
    this->FxInfoJitter_c::FxInfoJitter_c();
    return this;
}

// 0x4A5B90
void FxInfoJitter_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5BB0
void FxInfoJitter_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float value;
    m_InterpInfo.GetVal(&value, mult);

    CVector in((float)(CGeneral::GetRandomNumber() % 10'000) / 5000.0f - 1.0f);
    in.Normalise();
    in *= value * totalTime;

    auto& movement = *static_cast<MovementInfo_t*>(info);
    movement.m_Pos += in;
}
