#include "StdInc.h"

#include "FxInfoUnderwater.h"
#include "MovementInfo.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoUnderwater_c::InjectHooks() {
    RH_ScopedClass(FxInfoUnderwater_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A5E20);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A7550);
    RH_ScopedInstall(DestructorBody, 0x4A7570);
}

// 0x4A5E20
FxInfoUnderwater_c::FxInfoUnderwater_c() : FxInfo_c() {
    m_nType = FX_INFO_UNDERWATER_DATA;
    // Como el original: solo pone a 0 el byte bajo del contador (+0xE); no pide memoria al pool
    *reinterpret_cast<int8*>(&m_InterpInfo.m_nCount) = 0;
}

// 0x4A5E20 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoUnderwater_c* FxInfoUnderwater_c::Constructor() {
    this->FxInfoUnderwater_c::FxInfoUnderwater_c();
    return this;
}

void FxInfoUnderwater_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5E90
void FxInfoUnderwater_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) {
    static_cast<MovementInfo_t*>(info)->m_bHasUnderwaterInfo = true;
}


// 0x4A7570 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoUnderwater_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfoU255_c, 0x4A87F0: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoUnderwater_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A7550 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoUnderwater_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A7570; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoUnderwater_c* FxInfoUnderwater_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
