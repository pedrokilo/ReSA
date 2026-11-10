/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeaderExitedCarAsDriver
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventLeaderExitedCarAsDriver.h"


void CEventLeaderExitedCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderExitedCarAsDriver, 0x85B898, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B8300);
    RH_ScopedVMTInstall(CloneEditable, 0x4B8340); // lote 156A (banco Fn156A)
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B0F80);
}

// 0x4B8340 (lote 156A, banco Fn156A; thiscall, sin argumentos, `ret`, 80 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); no lee `this`; ranura 16 de la vtable 0x85B898).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x14 bytes; sin memoria devuelve nulo; si no, CEventEditableResponse::CEventEditableResponse (0x4AC450, con hook; ECX = memoria), deja la vtable
// de la clase del EXE (0x85B898; la de la DLL no vale para los llamadores del EXE) y devuelve la memoria.
CEventLeaderExitedCarAsDriver* CEventLeaderExitedCarAsDriver::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventLeaderExitedCarAsDriver));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventEditableResponse*>(memory)->Constructor();
    *reinterpret_cast<uint32*>(memory) = 0x85B898;
    return static_cast<CEventLeaderExitedCarAsDriver*>(memory);
}

// 0x4B8300
CEventLeaderExitedCarAsDriver* CEventLeaderExitedCarAsDriver::Constructor()
{
    this->CEventLeaderExitedCarAsDriver::CEventLeaderExitedCarAsDriver();
    return this;
}

// 0x4B0F80
bool CEventLeaderExitedCarAsDriver::AffectsPedGroup(CPedGroup* pg) {
    const auto leader = pg->GetMembership().GetLeader();
    for (auto* const m : pg->GetMembership().GetFollowers()) {
        if (m->m_pVehicle && m->bInVehicle && m->m_pVehicle == leader->m_pVehicle) {
            return true;
        }
        if (m->GetTaskManager().HasAnyOf<TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_WAIT>(false)) {
            return true;
        }
    }
    return false;
}
