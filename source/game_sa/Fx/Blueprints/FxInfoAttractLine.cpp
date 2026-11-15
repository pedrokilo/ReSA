#include "StdInc.h"

#include "FxInfoAttractLine.h"

void FxInfoAttractLine_c::InjectHooks() {
    RH_ScopedClass(FxInfoAttractLine_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A57C0); // lote 215A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A7020);
    RH_ScopedInstall(DestructorBody, 0x4A7040);
}

// 0x4A57C0
FxInfoAttractLine_c::FxInfoAttractLine_c() : FxInfo_c() {
    m_nType = FX_INFO_ATTRACTLINE_DATA;
    m_InterpInfo.Allocate(7);
}

// 0x4A5830
void FxInfoAttractLine_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4880
void FindClosestPtOnLine(CVector& out, CVector& lineA, CVector& lineB, CVector& point) {
    return plugin::Call<0x4A4880, CVector&, CVector&, CVector&, CVector&>(out, lineA, lineB, point);

    const auto a = DistanceBetweenPoints(point, lineA);
    const auto b = DistanceBetweenPoints(lineB, lineA);
    const auto a3m2 = lineB - lineA;
    const auto a4m2 = point - lineA;
    float v9 = DotProduct(a4m2, a3m2) / a3m2.SquaredMagnitude();
    out = a3m2 * std::clamp(v9, 0.0f, 1.0f) + lineA;
}

// 0x4A5850
void FxInfoAttractLine_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) {
    float values[7];
    if (m_bTimeModeParticle) {
        m_InterpInfo.GetVal(values, mult);
    } else {
        m_InterpInfo.GetVal(values, currentTime / length);
    }

    CVector out;
    auto& lineA = *(CVector*)&values[0];
    auto& lineB = *(CVector*)&values[3];

    auto& movement = *static_cast<MovementInfo_t*>(info);
    FindClosestPtOnLine(out, lineA, lineB, movement.m_Pos);
    CVector in = out - movement.m_Pos;
    auto dist = in.Magnitude();

    if (dist > 0.0f) {
        in.Normalise();
    }

    if (dist > 0.1f) {
        movement.m_Vel += in * values[6] * totalTime;
    }
}


// 0x4A7040 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoAttractLine_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfo32_c, 0x4A89B0: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoAttractLine_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A7020 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoAttractLine_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A7040; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoAttractLine_c* FxInfoAttractLine_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
