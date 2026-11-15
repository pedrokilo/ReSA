#include "StdInc.h"

#include "FxInfoEmPos.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoEmPos_c::InjectHooks() {
    RH_ScopedClass(FxInfoEmPos_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Destructor, 0x004A6D30);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A6D10);
}

// 0x4A5010
FxInfoEmPos_c::FxInfoEmPos_c() : FxInfo_c() {
    m_nType = FX_INFO_EMPOS_DATA;
    m_InterpInfo.Allocate(3);
}

// 0x4A6D30 (lote 216A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A6D10.
__declspec(noinline) void FxInfoEmPos_c::Destructor() {
    this->FxInfoEmPos_c::~FxInfoEmPos_c();
}

// 0x4A6D10 (lote 216A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A6D30; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoEmPos_c* FxInfoEmPos_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A5080
void FxInfoEmPos_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A50A0
void FxInfoEmPos_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[3];
    m_InterpInfo.GetVal(values, currentTime);

    static_cast<EmissionInfo_t*>(info)->m_Pos = *(CVector*)&values[0];
}

// Lote 217A (sin validar): ver reversing/PendienteValidar/217A.md
void FxInfoEmPos_c::InjectHooks() {
    RH_ScopedClass(FxInfoEmPos_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(DestructorBody, 0x4A6D30);
}

// 0x4A6D30 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoEmPos_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfoFloat_c, 0x4A8460: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoEmPos_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}
