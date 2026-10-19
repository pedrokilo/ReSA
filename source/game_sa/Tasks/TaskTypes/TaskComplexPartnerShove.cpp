/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPartnerShove
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

#include "TaskComplexPartnerShove.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleRunAnim.h"

void CTaskComplexPartnerShove::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexPartnerShove, 0x870800, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x6846F0);
    RH_ScopedVMTInstall(GetPartnerSequence, 0x683120);
}

CTaskComplexPartnerShove::CTaskComplexPartnerShove(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, false, updateDirectionCount, point)
{
    m_updateDirectionCount = updateDirectionCount;
    m_taskId = TASK_COMPLEX_PARTNER_SHOVE;
}

CTaskComplexPartnerShove* CTaskComplexPartnerShove::Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, CVector point)
{
    this->CTaskComplexPartnerShove::CTaskComplexPartnerShove(commandName, partner, leadSpeaker, distanceMultiplier, updateDirectionCount, point);
    return this;
}

// 0x683120
CTaskComplexSequence* CTaskComplexPartnerShove::GetPartnerSequence()
{
    auto* seq = new CTaskComplexSequence; // Como el original: sin inicialización por valor
    seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f, 0.001f)); // 0x86FC7C y 0x3A83126F
    seq->AddTask(new CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, m_leadSpeaker ? ANIM_ID_SHOVE_PARTIAL : ANIM_ID_HANDSUP, 4.0f, false)); // 0xA4 si m_leadSpeaker != 0, si no 0x8E; mezcla 4.0f (0x40800000)
    return seq;
}
