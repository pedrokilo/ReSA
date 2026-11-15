#include "StdInc.h"

#include "FxInfoEmLife.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoEmLife_c::InjectHooks() {
    RH_ScopedClass(FxInfoEmLife_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Destructor, 0x004A6CC0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A6CA0);
}

// 0x4A4F50
FxInfoEmLife_c::FxInfoEmLife_c() : FxInfo_c() {
    m_nType = FX_INFO_EMLIFE_DATA;
    m_InterpInfo.Allocate(2);
}

// 0x4A6CC0 (lote 216A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A6CA0.
__declspec(noinline) void FxInfoEmLife_c::Destructor() {
    this->FxInfoEmLife_c::~FxInfoEmLife_c();
}

// 0x4A6CA0 (lote 216A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A6CC0; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoEmLife_c* FxInfoEmLife_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A4FC0
void FxInfoEmLife_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4FE0
void FxInfoEmLife_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[2];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);
    emission.m_fLife     = values[0];
    emission.m_fLifeBias = values[1];
}
