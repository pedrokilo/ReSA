/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeaderQuitEnteringCarAsDriver
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventLeaderQuitEnteringCarAsDriver.h"


void CEventLeaderQuitEnteringCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderQuitEnteringCarAsDriver, 0x86E6B0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x63A110);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B1010);
}

// 0x63A110
CEventLeaderQuitEnteringCarAsDriver* CEventLeaderQuitEnteringCarAsDriver::Constructor()
{
    this->CEventLeaderQuitEnteringCarAsDriver::CEventLeaderQuitEnteringCarAsDriver();
    return this;
}

// 0x4B1010
bool CEventLeaderQuitEnteringCarAsDriver::AffectsPedGroup(CPedGroup* pedGroup) {
    const auto oe = pedGroup->GetIntelligence().GetCurrentEvent();
    return oe && oe->GetEvent().GetEventType() == EVENT_LEADER_ENTERED_CAR_AS_DRIVER;
}