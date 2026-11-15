#include "StdInc.h"

#include "FxInfoSize.h"
#include "RenderInfo.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoSize_c::InjectHooks() {
    RH_ScopedClass(FxInfoSize_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A5FA0);
}

// 0x4A5FA0
FxInfoSize_c::FxInfoSize_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_SIZE_DATA;
    m_InterpInfo.Allocate(4);
}

// 0x4A5FA0 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoSize_c* FxInfoSize_c::Constructor() {
    this->FxInfoSize_c::FxInfoSize_c();
    return this;
}

// 0x4A6010
void FxInfoSize_c::Load(FILESTREAM file, int32 version) {
    if (version < 105) {
        m_InterpInfo.m_nCount = 2;
    }
    m_InterpInfo.Load(file);
}

// 0x4A6030
void FxInfoSize_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[4];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_fSizeX = values[0];
    render.m_fSizeY = values[1];

    if (m_InterpInfo.m_nCount == 4) {
        render.m_fSizeXBias = values[2];
        render.m_fSizeYBias = values[3];
    } else {
        render.m_fSizeXBias = 0.0f;
        render.m_fSizeYBias = 0.0f;
    }
}

// Lote 217A (sin validar): ver reversing/PendienteValidar/217A.md
void FxInfoSize_c::InjectHooks() {
    RH_ScopedClass(FxInfoSize_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A7630);
    RH_ScopedInstall(DestructorBody, 0x4A7650);
}

// 0x4A7650 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoSize_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfo32_c, 0x4A89B0: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoSize_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A7630 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoSize_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A7650; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoSize_c* FxInfoSize_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
