/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventDeath
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventDeath.h"

void CEventDeath::InjectHooks() {
    RH_ScopedVirtualClass(CEventDeath, 0x85ADC0, 16); // lote 202A (sin validar): antes RH_ScopedClass; hace falta la vtable para enganchar Clone
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4ADDF0);
    RH_ScopedVMTInstall(Clone, 0x4B6E30); // lote 202A (sin validar): ranura 4
}

// 0x4B6E30 (lote 202A, sin validar; el listado decia CEventVehicleCollision; ranura 4 (Clone) de la vtable 0x85ADC0; thiscall, sin argumentos, `ret`, 53 bytes SIN marco SEH ni llamada al constructor: lo expande en linea).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x14 bytes; sin memoria devuelve nulo sin leer campos; si no, escribe en este orden +4 (m_nTimeActive) = 0, +8 (m_bValid) = 0, la vtable del EXE (0x85ADC0), el byte +0xC
// (m_bDrowning, copiado como byte) y el dword +0x10 (m_deathTimeInMs), y devuelve el evento nuevo. No llama a 0x4ADE50 ni a 0x4ABFC0. Deja la vtable del EXE, no la de la DLL.
CEvent* CEventDeath::Clone() const noexcept {
    auto* const object = static_cast<CEventDeath*>(CEvent::operator new(sizeof(CEventDeath)));
    if (!object) {
        return nullptr;
    }
    const uint32 deathTimeInMs = m_deathTimeInMs;
    const uint8  drowning      = *reinterpret_cast<const uint8*>(&m_bDrowning); // byte tal cual (sin normalizar a 0 o 1)
    object->m_nTimeActive = 0;
    object->m_bValid      = false;
    *reinterpret_cast<uint32*>(object) = 0x85ADC0;
    *reinterpret_cast<uint8*>(&object->m_bDrowning) = drowning;
    object->m_deathTimeInMs  = deathTimeInMs;
    return object;
}

// 0x4ADE50, `time` should be int32
CEventDeath::CEventDeath(bool bDrowning, uint32 deathTimeInMs) : CEvent() {
    m_bDrowning = bDrowning;
    m_deathTimeInMs = deathTimeInMs;
}

// 0x4ADDF0
CEventDeath::CEventDeath(bool bDrowning) : CEvent() {
    m_bDrowning = bDrowning;
    m_deathTimeInMs = CTimer::GetTimeInMS();
}
