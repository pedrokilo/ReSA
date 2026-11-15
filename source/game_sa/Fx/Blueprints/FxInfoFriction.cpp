#include "StdInc.h"

#include "FxInfoFriction.h"

void FxInfoFriction_c::InjectHooks() {
    RH_ScopedClass(FxInfoFriction_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A5550); // lote 215A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A6F40);
    RH_ScopedInstall(DestructorBody, 0x4A6F60);
}

// 0x4A5550
FxInfoFriction_c::FxInfoFriction_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_FRICTION_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A55C0
void FxInfoFriction_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A55E0
void FxInfoFriction_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[3];
    m_InterpInfo.GetVal(values, mult);

    auto& movement = *static_cast<MovementInfo_t*>(info);
    movement.m_Vel *= std::pow(values[0], totalTime * 50.0f);
}


// 0x4A6F60 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoFriction_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfo32_c, 0x4A89B0: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoFriction_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A6F40 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoFriction_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A6F60; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoFriction_c* FxInfoFriction_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
