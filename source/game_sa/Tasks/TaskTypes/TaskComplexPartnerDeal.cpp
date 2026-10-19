/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPartnerDeal
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexPartnerDeal.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleRunAnim.h"

void CTaskComplexPartnerDeal::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexPartnerDeal, 0x870754, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x684190);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6823B0);
    RH_ScopedVMTInstall(StreamRequiredAnims, 0x6823C0);
    RH_ScopedVMTInstall(GetPartnerSequence, 0x682440);
}

CTaskComplexPartnerDeal::CTaskComplexPartnerDeal(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, true, 1, point)
{
    m_taskId = TASK_COMPLEX_PARTNER_DEAL;
    // Como el original: copia los 6 bytes de "gangs" (dword + word desde 0x86F92C) sin tocar el resto del búfer
    strcpy(m_animBlockName, "gangs");
}

CTaskComplexPartnerDeal* CTaskComplexPartnerDeal::Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, CVector point)
{
    this->CTaskComplexPartnerDeal::CTaskComplexPartnerDeal(commandName, partner, leadSpeaker, distanceMultiplier, point);
    return this;
}

// 0x6823B0
CTask* CTaskComplexPartnerDeal::CreateFirstSubTask(CPed* ped)
{
    return CTaskComplexPartner::CreateFirstSubTask(ped); // Como el original: salto directo a 0x681F20
}

// 0x6823C0
void CTaskComplexPartnerDeal::StreamRequiredAnims()
{
    // Como el original: sin referencia, se pide/referencia el bloque si hay que cargar las animaciones de bandas;
    // con referencia, se suelta si ya no hay que cargarlas (el bloque cargado se compara con == 1)
    if (reinterpret_cast<const uint8&>(m_requiredAnimsStreamedIn) == 0) { // Como el original: se prueba el byte entero
        if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            return;
        }
        const auto blkIdx = CAnimManager::GetAnimationBlockIndex(m_animBlockName);
        if (reinterpret_cast<const uint8&>(CAnimManager::GetAnimBlocks()[blkIdx].IsLoaded) != 1) {
            CStreaming::RequestModel(IFPToModelId(blkIdx), STREAMING_KEEP_IN_MEMORY);
            return;
        }
        CAnimManager::AddAnimBlockRef(blkIdx);
        m_requiredAnimsStreamedIn = true;
    } else {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            return;
        }
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_animBlockName));
        m_requiredAnimsStreamedIn = false;
    }
}

// 0x682440
CTaskComplexSequence* CTaskComplexPartnerDeal::GetPartnerSequence()
{
    auto* seq = new CTaskComplexSequence; // Como el original: sin inicialización por valor
    seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f, 0.02f)); // 0x86FC7C y 0x3CA3D70A
    // Como el original: el byte m_leadSpeaker se prueba entero (cualquier valor distinto de 0 es verdadero)
    const bool lead = reinterpret_cast<const uint8&>(m_leadSpeaker) != 0;
    seq->AddTask(new CTaskSimpleRunAnim(ANIM_GROUP_GANGS, lead ? ANIM_ID_DEALER_DEAL : ANIM_ID_DRUGS_BUY, 4.0f, false));
    return seq;
}
