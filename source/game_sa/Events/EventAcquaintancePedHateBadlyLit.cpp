/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventAcquaintancePedHateBadlyLit
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
#include "EventAcquaintancePedHateBadlyLit.h"


void CEventAcquaintancePedHateBadlyLit::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAcquaintancePedHateBadlyLit, 0x86CB10, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5FF250);
    RH_ScopedVMTInstall(AffectsPed, 0x4AFA90);
}

// 0x5FF250
CEventAcquaintancePedHateBadlyLit::CEventAcquaintancePedHateBadlyLit(CPed* ped, int32 startTimeInMs, const CVector& point) : CEventAcquaintancePed(ped)
{
    m_startTimeInMs = startTimeInMs;
    m_point = point;
    if (startTimeInMs == -1) {
        m_startTimeInMs = CTimer::GetTimeInMS();
        m_point = ped->GetPosition();
    }
}

// 0x5FF250
CEventAcquaintancePedHateBadlyLit* CEventAcquaintancePedHateBadlyLit::Constructor(CPed* ped, int32 startTimeInMs, const CVector& point)
{
    this->CEventAcquaintancePedHateBadlyLit::CEventAcquaintancePedHateBadlyLit(ped, startTimeInMs, point);
    return this;
}

// 0x4AFA90
bool CEventAcquaintancePedHateBadlyLit::AffectsPed(CPed* ped)
{
    if (CEventAcquaintancePed::AffectsPed(ped)) {
        CEvent* currentEvent = ped->GetEventHandlerHistory().GetCurrentEvent();
        if (!currentEvent || currentEvent->GetEventType() != EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT
            || currentEvent->GetSourceEntity() != GetSourceEntity())
        {
            return true;
        }
        auto theCurrentEvent = static_cast<CEventAcquaintancePedHateBadlyLit*>(currentEvent);
        if (m_startTimeInMs - theCurrentEvent->m_startTimeInMs >= 2000) {
            CVector distance = m_point - theCurrentEvent->m_point;
            return distance.SquaredMagnitude() >= 1.0f;
        }
    }
    return false;
}

