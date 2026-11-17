/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoSelfLit_c
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxInfoSelfLit.h"
#include "RenderInfo.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoSelfLit_c::InjectHooks() {
    RH_ScopedClass(FxInfoSelfLit_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A6760);
    RH_ScopedInstall(Destructor, 0x004A79D0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A79B0);
}

// 0x4A6760
FxInfoSelfLit_c::FxInfoSelfLit_c() : m_InterpInfo() {
    m_nType = FX_INFO_SELFLIT_DATA;
    // Como el original: solo pone a 0 el byte bajo del contador (+0xE); no pide memoria al pool
    *reinterpret_cast<int8*>(&m_InterpInfo.m_nCount) = 0;
}

// 0x4A6760 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoSelfLit_c* FxInfoSelfLit_c::Constructor() {
    this->FxInfoSelfLit_c::FxInfoSelfLit_c();
    return this;
}

// 0x4A79D0 (lote 218A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A79B0.
__declspec(noinline) void FxInfoSelfLit_c::Destructor() {
    this->FxInfoSelfLit_c::~FxInfoSelfLit_c();
}

// 0x4A79B0 (lote 218A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A79D0; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoSelfLit_c* FxInfoSelfLit_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A67B0
void FxInfoSelfLit_c::Load(FILESTREAM file, int32 version) {
    // NOP
}

// 0x4A67C0
void FxInfoSelfLit_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    static_cast<RenderInfo_t*>(info)->m_bSelfLit = true;
}
