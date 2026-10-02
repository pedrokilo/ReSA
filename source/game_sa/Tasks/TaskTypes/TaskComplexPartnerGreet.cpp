/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPartnerGreet
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexPartnerGreet.h"
#include "TaskComplexGangLeader.h"

void CTaskComplexPartnerGreet::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartnerGreet, 0x87078c, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x684210);
    RH_ScopedInstall(Destructor, 0x684280);

    RH_ScopedVMTInstall(Clone, 0x684DA0);
    RH_ScopedVMTInstall(GetTaskType, 0x681E00);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6825A0);
    RH_ScopedVMTInstall(StreamRequiredAnims, 0x6825B0);
    RH_ScopedVMTInstall(GetPartnerSequence, 0x682630);
}

CTaskComplexPartnerGreet::CTaskComplexPartnerGreet(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int32 handShakeType, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, true, 1, point)
{
    m_handShakeType = handShakeType;
    m_taskId = TASK_COMPLEX_PARTNER_GREET;
    strcpy_s(m_animBlockName, "gangs");
}

CTaskComplexPartnerGreet::CTaskComplexPartnerGreet(const CTaskComplexPartnerGreet& o) :
    CTaskComplexPartnerGreet{
        o.m_commandName,
        o.m_partner,
        o.m_leadSpeaker,
        o.m_distanceMultiplier,
        o.m_handShakeType,
        o.m_point
    }
{
}

void CTaskComplexPartnerGreet::StreamRequiredAnims() {
    CAnimManager::StreamAnimBlock(m_animBlockName, CTaskComplexGangLeader::ShouldLoadGangAnims(), m_requiredAnimsStreamedIn);
}

CTaskComplexSequence* CTaskComplexPartnerGreet::GetPartnerSequence() {
    NOTSA_UNREACHABLE(); // In theory this is unused
}
