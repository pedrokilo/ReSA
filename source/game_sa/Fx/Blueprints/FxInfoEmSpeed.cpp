/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoEmSpeed_c
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

#include "FxInfoEmSpeed.h"
#include "FxManager.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoEmSpeed_c::InjectHooks() {
    RH_ScopedClass(FxInfoEmSpeed_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Destructor, 0x004A6B70);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A6B50);
    RH_ScopedInstall(Constructor, 0x4A4D00); // lote 215A (sin validar)
}

// 0x4A4D00
FxInfoEmSpeed_c::FxInfoEmSpeed_c() : m_InterpInfo() { // la base FxInfo_c por defecto (sin `()`): el EXE no escribe +6 ni +7
    m_nType = FX_INFO_EMSPEED_DATA;
    m_InterpInfo.Allocate(2);
}

// 0x4A6B70 (lote 216A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A6B50.
__declspec(noinline) void FxInfoEmSpeed_c::Destructor() {
    // Como el EXE (0x4A89B0 / 0x4A8460 / 0x4A87F0 y despues el [this]): el destructor del interp deja su vtable base (0x85A968) y despues se deja la de FxInfo_c (0x85A7DC). El ~T() generado deja
    // vtables de la DLL y omite la del interp (destructor trivial).
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A6B50 (lote 216A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A6B70; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoEmSpeed_c* FxInfoEmSpeed_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A4D70
void FxInfoEmSpeed_c::Load(FILESTREAM file, int32 version) {
    // nick7:
    // well, this is interesting: this is not a pointer, so there's 2 ways:
    //  1) WIN32 version of SA calls __vfptr->Load
    //  2) Android version of SA calls FxInterpInfo32_c::Load
    // anyway, this is NOT a bug here, but this is not good
    m_InterpInfo.Load(file);
}

// 0x4A4D90
void FxInfoEmSpeed_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[2];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);
    emission.m_Speed     = values[0];
    emission.m_fSpeedBias = values[1];
}
