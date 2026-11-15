#include "StdInc.h"

#include "FxInfoColourRange.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoColourRange_c::InjectHooks() {
    RH_ScopedClass(FxInfoColourRange_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A6630);
    RH_ScopedInstall(Destructor, 0x004A7960);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A7940);
}

// 0x4A6630
FxInfoColourRange_c::FxInfoColourRange_c() : FxInfo_c() {
    m_nType = FX_INFO_COLOURRANGE_DATA;
    m_InterpInfo.Allocate(7);
}

// 0x4A6630 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoColourRange_c* FxInfoColourRange_c::Constructor() {
    this->FxInfoColourRange_c::FxInfoColourRange_c();
    return this;
}

// 0x4A7960 (lote 218A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A7940.
__declspec(noinline) void FxInfoColourRange_c::Destructor() {
    this->FxInfoColourRange_c::~FxInfoColourRange_c();
}

// 0x4A7940 (lote 218A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A7960; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoColourRange_c* FxInfoColourRange_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A66A0
void FxInfoColourRange_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A66C0
void FxInfoColourRange_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[7];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.SetColor(values[0], values[2], values[4], values[6]);
    render.SetRangeColor(values[1], values[3], values[5]);
    render.m_nColorType = ERenderColorType::RANGE;
}
