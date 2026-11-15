#include "StdInc.h"

#include "FxInfoEmSize.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoEmSize_c::InjectHooks() {
    RH_ScopedClass(FxInfoEmSize_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Destructor, 0x004A6B00);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A6AE0);
}

// 0x4A4C20
FxInfoEmSize_c::FxInfoEmSize_c() : FxInfo_c() {
    m_nType = FX_INFO_EMSIZE_DATA;
    m_InterpInfo.Allocate(7);
}

// 0x4A6B00 (lote 216A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A6AE0.
__declspec(noinline) void FxInfoEmSize_c::Destructor() {
    this->FxInfoEmSize_c::~FxInfoEmSize_c();
}

// 0x4A6AE0 (lote 216A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A6B00; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoEmSize_c* FxInfoEmSize_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A4C90
void FxInfoEmSize_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4CB0
void FxInfoEmSize_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[7];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);
    emission.m_fRadius = values[0];

    emission.m_SizeMin.x = values[1];
    emission.m_SizeMin.y = values[3];
    emission.m_SizeMin.z = values[5];

    emission.m_SizeMax.x = values[2];
    emission.m_SizeMax.y = values[4];
    emission.m_SizeMax.z = values[6];
}
