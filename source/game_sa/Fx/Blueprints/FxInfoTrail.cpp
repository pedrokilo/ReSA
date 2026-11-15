#include "StdInc.h"

#include "FxInfoTrail.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoTrail_c::InjectHooks() {
    RH_ScopedClass(FxInfoTrail_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A6220);
    RH_ScopedInstall(Destructor, 0x004A77A0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A7780);
}

// 0x4A6220
FxInfoTrail_c::FxInfoTrail_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_TRAIL_DATA;
    m_InterpInfo.Allocate(2);
}

// 0x4A6220 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoTrail_c* FxInfoTrail_c::Constructor() {
    this->FxInfoTrail_c::FxInfoTrail_c();
    return this;
}

// 0x4A77A0 (lote 218A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A7780.
__declspec(noinline) void FxInfoTrail_c::Destructor() {
    this->FxInfoTrail_c::~FxInfoTrail_c();
}

// 0x4A7780 (lote 218A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A77A0; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoTrail_c* FxInfoTrail_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A6290
void FxInfoTrail_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A62B0
void FxInfoTrail_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / length;
    }

    float values[2];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_fTrailTime = values[0];
    render.m_nTrailScreenMode = 1;
    if (values[1] > 0.1f) {
        render.m_nTrailScreenMode = 2;
    }
}
