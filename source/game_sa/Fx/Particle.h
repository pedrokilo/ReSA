#pragma once

#include "ListItem_c.h"
#include "Vector.h"

class FxSystem_c;
class FxEmitterPrt_c;

struct Particle_c : public ListItem_c<Particle_c> {
    float       m_fTotalLife;
    float       m_fCurrentLife;
    CVector     m_Pos;
    CVector     m_Velocity;
    FxSystem_c* m_System;

    // NOTSA
    FxEmitterPrt_c* AsFxEmitterPrt() { return reinterpret_cast<FxEmitterPrt_c*>(this); }

    static void InjectHooks();
    // 0x4A9110 (lote 144A, banco Fn144A): constructor del EXE (`push esi; mov esi,ecx; call 0x4A8DB0 (ListItem_c: m_pPrev = m_pNext = 0); mov eax,esi; pop esi; ret`); sin llamadores directos
    Particle_c* Constructor() { m_pPrev = nullptr; m_pNext = nullptr; return this; }
};
VALIDATE_SIZE(Particle_c, 0x2C);
