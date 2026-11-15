#include "StdInc.h"

#include "FxInfoHeatHaze.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoHeatHaze_c::InjectHooks() {
    RH_ScopedClass(FxInfoHeatHaze_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A61A0);
    RH_ScopedInstall(Destructor, 0x004A7730);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A7710);
}

// 0x4A61A0
FxInfoHeatHaze_c::FxInfoHeatHaze_c() : FxInfo_c() {
    m_nType = FX_INFO_HEATHAZE_DATA;
    // Como el original: solo pone a 0 el byte bajo del contador (+0xE); no pide memoria al pool
    *reinterpret_cast<int8*>(&m_InterpInfo.m_nCount) = 0;
}

// 0x4A61A0 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoHeatHaze_c* FxInfoHeatHaze_c::Constructor() {
    this->FxInfoHeatHaze_c::FxInfoHeatHaze_c();
    return this;
}

// 0x4A7730 (lote 218A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A7710.
__declspec(noinline) void FxInfoHeatHaze_c::Destructor() {
    this->FxInfoHeatHaze_c::~FxInfoHeatHaze_c();
}

// 0x4A7710 (lote 218A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A7730; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoHeatHaze_c* FxInfoHeatHaze_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A61F0
void FxInfoHeatHaze_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A6210
void FxInfoHeatHaze_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    static_cast<RenderInfo_t*>(info)->m_bHeatHaze = true;
}
