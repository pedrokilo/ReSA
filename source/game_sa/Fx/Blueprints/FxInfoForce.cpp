#include "StdInc.h"

#include "FxInfoForce.h"

// 0x4A5450
FxInfoForce_c::FxInfoForce_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_FORCE_DATA;
    m_InterpInfo.Allocate(3);
}

// 0x4A54C0
void FxInfoForce_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A54E0
void FxInfoForce_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[3];
    m_InterpInfo.GetVal(values, mult);

    auto& movement = *static_cast<MovementInfo_t*>(info);
    movement.m_Vel += CVector(values[0], values[1], values[2]) * totalTime;
}

// Lote 217A (sin validar): ver reversing/PendienteValidar/217A.md
void FxInfoForce_c::InjectHooks() {
    RH_ScopedClass(FxInfoForce_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A6ED0);
    RH_ScopedInstall(DestructorBody, 0x4A6EF0);
}

// 0x4A6EF0 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoForce_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfo32_c, 0x4A89B0: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoForce_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A6ED0 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoForce_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A6EF0; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoForce_c* FxInfoForce_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
