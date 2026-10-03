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
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexPartnerGreet.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleDoHandSignal.h"
#include "TaskSimpleChat.h"

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
    strcpy(m_animBlockName, "gangs"); // Como el original: copia solo los 6 bytes de "gangs" (strcpy_s rellena con 0xFE en debug)
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

// 0x6825B0
void CTaskComplexPartnerGreet::StreamRequiredAnims() {
    // Como el original: sin referencia, se pide/referencia el bloque si hay que cargar las animaciones de bandas;
    // con referencia, se suelta si ya no hay que cargarlas (el bloque cargado se compara con == 1)
    if (!m_requiredAnimsStreamedIn) {
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

// 0x682630
CTaskComplexSequence* CTaskComplexPartnerGreet::GetPartnerSequence() {
    auto* seq = new CTaskComplexSequence; // Como el original: sin inicialización por valor
    seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f, 0.001f));

    // Como el original: animación de saludo según el tipo (2: según quien habla primero); otros tipos no añaden nada
    const auto AddHandShake = [&](int32 animId) {
        seq->AddTask(new CTaskSimpleRunAnim(ANIM_GROUP_GANGS, (AnimationId)animId, 4.f, false));
    };
    switch (m_handShakeType) {
    case 0: AddHandShake(ANIM_ID_HNDSHKAA); break;
    case 1: AddHandShake(ANIM_ID_HNDSHKBA); break;
    case 2: AddHandShake(m_leadSpeaker ? ANIM_ID_HNDSHKCA : ANIM_ID_HNDSHKCB); break;
    case 3: AddHandShake(ANIM_ID_HNDSHKDA); break;
    case 4: AddHandShake(ANIM_ID_HNDSHKEA); break;
    case 5: AddHandShake(ANIM_ID_HNDSHKFA); break;
    }

    // Como el original: 3000 - (int)(rand * (1/32768) * -4000), cuatro veces seguidas, y después rand() < 0x3FFF
    const auto RandomDuration = [] {
        return 3000 - (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * 3.0517578125e-05f * -4000.f);
    };
    const int32 t1 = RandomDuration();
    const int32 t2 = RandomDuration();
    const int32 t3 = RandomDuration();
    const int32 t4 = RandomDuration();
    const bool  chatAgain = (int32)CGeneral::GetRandomNumber() < 0x3FFF;

    if (m_leadSpeaker) {
        seq->AddTask(new CTaskSimpleDoHandSignal);
        seq->AddTask(new CTaskSimpleStandStill(1000, false, false, 8.f));
        seq->AddTask(new CTaskSimpleStandStill(t1, false, false, 8.f));
        seq->AddTask(new CTaskSimpleChat(t2));
    } else {
        seq->AddTask(new CTaskSimpleStandStill(1000, false, false, 8.f));
        seq->AddTask(new CTaskSimpleDoHandSignal);
        seq->AddTask(new CTaskSimpleChat(t1));
        seq->AddTask(new CTaskSimpleStandStill(t2, false, false, 8.f));
    }
    if (chatAgain) {
        seq->AddTask(new CTaskSimpleStandStill(t3, false, false, 8.f));
        seq->AddTask(new CTaskSimpleChat(t4));
    }
    return seq;
}
