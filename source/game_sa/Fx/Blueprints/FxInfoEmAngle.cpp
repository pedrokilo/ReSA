#include "StdInc.h"

#include "FxInfoEmAngle.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoEmAngle_c::InjectHooks() {
    RH_ScopedClass(FxInfoEmAngle_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Destructor, 0x004A6C50);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A6C30);
    RH_ScopedInstall(Constructor, 0x4A4E90); // lote 215A (sin validar)
}

// 0x4A4E90
FxInfoEmAngle_c::FxInfoEmAngle_c() : FxInfo_c() {
    m_nType = FX_INFO_EMANGLE_DATA;
    m_InterpInfo.Allocate(2);
}

// 0x4A6C50 (lote 216A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A6C30.
__declspec(noinline) void FxInfoEmAngle_c::Destructor() {
    this->FxInfoEmAngle_c::~FxInfoEmAngle_c();
}

// 0x4A6C30 (lote 216A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A6C50; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoEmAngle_c* FxInfoEmAngle_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A4F00
void FxInfoEmAngle_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4F20
void FxInfoEmAngle_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[2];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);
    emission.m_fAngleMin = values[0];
    emission.m_fAngleMax = values[1];
}
