/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPartner
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexPartner.h"
#include "TaskComplexPartnerChat.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexWanderStandard.h"
#include "TaskSimpleStandStill.h"
#include "General.h"
#include "Ragdoll/IKChainManager.h"
#include "X87Math.h"

void CTaskComplexPartner::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartner, 0x870664, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x681E70);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x683AD0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x681F20);
    RH_ScopedVMTInstall(ControlSubTask, 0x6840D0);
    RH_ScopedVMTInstall(StreamRequiredAnims, 0x682310);
    RH_ScopedVMTInstall(RemoveStreamedAnims, 0x682370);
}

// 0x681E70
CTaskComplexPartner::CTaskComplexPartner(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, bool makePedAlwaysFacePartner, int8 updateDirectionCount, CVector point)
    : CTaskComplex()
{
    m_leadSpeaker              = leadSpeaker;
    m_makePedAlwaysFacePartner = makePedAlwaysFacePartner;
    m_distanceMultiplier       = distanceMultiplier;
    m_updateDirectionCount     = updateDirectionCount;
    m_point                    = point;
    m_partner                  = partner;
    m_partnerState             = PARTNER_STATE_UNK_1;
    m_taskCompleted            = false;
    m_firstToTargetFlag        = -1;
    m_requiredAnimsStreamedIn  = false;
    m_animBlockName[0]         = '\0';
    // Como el original: se registra la referencia del miembro (lea eax,[esi+38h]), no la del parámetro en la pila
    CEntity::SafeRegisterRef(m_partner);
}

/*!
 * @addr 0x683A40
 */
CTaskComplexPartner::~CTaskComplexPartner() {
    CEntity::SafeCleanUpRef(m_partner);

    if (m_requiredAnimsStreamedIn) {
        if (strcmp(m_animBlockName, "") != 0) {
            CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_animBlockName));
        }
        m_requiredAnimsStreamedIn = false;
    }
}

CTaskComplexPartner* CTaskComplexPartner::Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, bool makePedAlwaysFacePartner, int8 updateDirectionCount, CVector point) {
    this->CTaskComplexPartner::CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, makePedAlwaysFacePartner, updateDirectionCount, point);
    return this;
}

namespace {
// Como el original: los bytes bool (+0x58, +0x5C, +0x5D, +0x6E) se prueban enteros (`test al,al`), también los no canónicos
uint8 PartnerByte(const bool& flag) {
    return reinterpret_cast<const uint8&>(flag);
}

// 0x6822B0 (sin símbolo en el catálogo): estado (con signo, movsx) de la tarea equivalente del compañero, 0 si no la tiene
int32 PartnerStateOfPartner(const CTaskComplexPartner& task) {
    auto* const other = static_cast<CTaskComplexPartner*>(task.m_partner->GetIntelligence()->GetTaskManager().FindActiveTaskByType((eTaskType)task.m_taskId));
    return other ? (int32)(int8)other->m_partnerState : 0;
}

// Tarea equivalente del compañero (CTaskManager::FindActiveTaskByType 0x681740 con el mismo tipo)
CTaskComplexPartner* PartnerTaskOfPartner(const CTaskComplexPartner& task) {
    return static_cast<CTaskComplexPartner*>(task.m_partner->GetIntelligence()->GetTaskManager().FindActiveTaskByType((eTaskType)task.m_taskId));
}

// 0x41CCB0 (sin hook): forward de la matriz o (-sin h, cos h, 0) con fsin/fcos del x87, guardado en float
CVector PartnerForwardOf(const CPed& ped) {
    if (ped.m_matrix) {
        return ped.m_matrix->GetForward();
    }
    const float h = ped.m_placement.m_fHeading;
    return CVector{ -TrigSimpleSinCpp(h), TrigSimpleCosCpp(h), 0.f };
}

// 0x44E480 (CVector2D::Normalise del EXE): con x*x + y*y <= 0 solo pone x = 1; si no (también NaN) multiplica por 1 / sqrt(suma)
void PartnerNormalise2D(float& x, float& y) {
    const float sum = x * x + y * y;
    if (sum <= 0.0f) {
        x = 1.0f;
        return;
    }
    const float inv = 1.0f / std::sqrt(sum);
    x = x * inv;
    y = y * inv;
}

// Como el original: reserva con CTask::operator new (0x61A5A0) y, si el pool está agotado (nulo), devuelve nulo sin construir
template <class T, class... Args>
T* PartnerNewTask(Args... args) {
    void* const mem = CTask::operator new(sizeof(T));
    if (!mem) {
        return nullptr;
    }
    return ::new (mem) T(args...);
}

CTask* PartnerStandStill() {
    return PartnerNewTask<CTaskSimpleStandStill>(50, false, false, 8.0f);
}

// Como el original: los métodos de la base usan el word en +0x70, que pertenece a las clases derivadas (todas miden >= 0x74)
int16& PartnerCounter70(CTaskComplexPartner& task) {
    return *reinterpret_cast<int16*>(reinterpret_cast<uint8*>(&task) + 0x70);
}
} // namespace

// 0x683AD0
CTask* CTaskComplexPartner::CreateNextSubTask(CPed* ped) {
    if (!m_partner) {
        return nullptr;
    }
    auto* const partnerTask = PartnerTaskOfPartner(*this);
    if (!partnerTask || partnerTask->m_partner != ped) {
        return nullptr;
    }

    switch ((int8)m_partnerState) {
    case 6: {
        if (!PartnerByte(m_requiredAnimsStreamedIn) || !m_updateDirectionCount) {
            return nullptr;
        }
        m_updateDirectionCount--;
        return GetPartnerSequence();
    }
    case 5: {
        if (!PartnerByte(m_makePedAlwaysFacePartner)) {
            if (PartnerStateOfPartner(*this) >= 5) {
                CTask* const defTask = ped->GetIntelligence()->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_DEFAULT];
                if (defTask && defTask->GetTaskType() == TASK_COMPLEX_WANDER && static_cast<CTaskComplexWander*>(defTask)->GetWanderType() == WANDER_TYPE_STANDARD) {
                    // Como el original: rumbo opuesto al forward (GetNodeHeadingFromVector(-x, -y)) y 100000 ms hasta el siguiente escaneo
                    const CVector fwd     = PartnerForwardOf(*ped);
                    const auto    heading = (uint16)CGeneral::GetNodeHeadingFromVector(-fwd.x, -fwd.y);
                    // Como el original: con el pool agotado el EXE escribiría en nulo+0x34 (se detiene); aquí simplemente no se crea la tarea
                    if (auto* const wander = PartnerNewTask<CTaskComplexWanderStandard>(PEDMOVE_WALK, (uint8)heading, true)) {
                        wander->m_nMinNextScanTime = CTimer::GetTimeInMS() + 100000; // 0x86FCDC
                        ped->GetIntelligence()->GetTaskManager().SetTask(wander, TASK_PRIMARY_DEFAULT, false);
                    }
                }
                m_partnerState = (ePartnerState)6;
            }
            return PartnerStandStill();
        }

        if (m_firstToTargetFlag == 1) {
            if (PartnerStateOfPartner(*this) == 6) {
                m_partnerState = (ePartnerState)6;
            }
            return PartnerStandStill();
        }

        // Como el original: dirección 2D del compañero al peatón; si el peatón está entre 0.99 y 1.01 m (0x862CD0 y 0x870750) se da por llegado
        const CVector& pedPos     = ped->GetPosition();
        const CVector& partnerPos = m_partner->GetPosition();
        const float    pedX = pedPos.x, pedY = pedPos.y, partnerX = partnerPos.x, partnerY = partnerPos.y;
        float          vx   = pedX - partnerX;
        float          vy   = pedY - partnerY;
        const float    dist = std::sqrt(vx * vx + vy * vy);
        if (dist > 0.99f && dist < 1.01f) {
            m_partnerState = (ePartnerState)6;
            return PartnerStandStill();
        }

        PartnerNormalise2D(vx, vy);
        float       dx  = (vx + partnerX) - pedX;
        float       dy  = (vy + partnerY) - pedY;
        const float len = std::sqrt(dx * dx + dy * dy);
        if (len > 0.02f) { // 0x858B38; con NaN no se normaliza
            PartnerNormalise2D(dx, dy);
            dx = dx * 0.02f;
            dy = dy * 0.02f;
        }

        // Como el original: sin comprobar que el peatón tenga matriz; ped+0x4D8/0x4DC = (right . d, forward . d)
        const CMatrix& mat    = *ped->m_matrix;
        const float    right0 = mat.GetRight().x, right1 = mat.GetRight().y;
        const float    fwd0 = mat.GetForward().x, fwd1 = mat.GetForward().y;
        ped->m_vecAnimMovingShiftLocal.x = right0 * dx + right1 * dy;
        ped->m_vecAnimMovingShiftLocal.y = fwd0 * dx + fwd1 * dy;
        return PartnerStandStill();
    }
    case 4: {
        const int32 otherState = PartnerStateOfPartner(*this);
        if (otherState == 4 || otherState == 5) {
            if (m_firstToTargetFlag == -1) {
                m_firstToTargetFlag = 0;
            }
            auto* const t = PartnerTaskOfPartner(*this); // Como el original: sin comprobar nulo
            if (t->m_firstToTargetFlag == -1) {
                t->m_firstToTargetFlag = 1;
            }
            m_partnerState = (ePartnerState)5;
            return PartnerStandStill();
        }
        if (m_firstToTargetFlag == -1) {
            m_firstToTargetFlag = 1;
        }
        auto* const t = PartnerTaskOfPartner(*this); // Como el original: sin comprobar nulo
        if (t->m_firstToTargetFlag == -1) {
            t->m_firstToTargetFlag = 0;
        }
        t->m_point = m_targetPoint;
        return PartnerStandStill();
    }
    case 3: {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
            return PartnerNewTask<CTaskComplexTurnToFaceEntityOrCoord>(static_cast<CEntity*>(m_partner), 0.5f, 0.2f); // 0x86FC7C y 0x86FC80
        }
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_TURN_TO_FACE_ENTITY) {
            m_partnerState = (ePartnerState)4;
            return PartnerStandStill();
        }
        return nullptr;
    }
    case 2: {
        if (m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
            return nullptr;
        }
        if (m_point.x == 0.f && m_point.y == 0.f) { // 0x858B50; con NaN no entra
            return PartnerStandStill();
        }
        m_partnerState          = (ePartnerState)3;
        PartnerCounter70(*this) = 0;
        void* const mem         = CTask::operator new(sizeof(CTaskComplexGoToPointAndStandStill));
        if (!mem) {
            return nullptr;
        }
        return ::new (mem) CTaskComplexGoToPointAndStandStill(PEDMOVE_WALK, m_point, 0.1f, 0.f, false, true);
    }
    case 1: {
        if (m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
            return nullptr;
        }
        if (PartnerByte(m_leadSpeaker)) {
            // 0x681FE0 (sin símbolo en el catálogo, sin reconstruir): calcula el punto de encuentro a partir de m_point y m_targetPoint
            plugin::CallMethod<0x681FE0, CTaskComplexPartner*, CPed*, CVector*, CVector*>(this, ped, &m_point, &m_targetPoint);
            auto* const t = PartnerTaskOfPartner(*this); // Como el original: sin comprobar nulo
            t->m_point    = m_targetPoint;
        }
        m_partnerState = (ePartnerState)2;
        return PartnerStandStill();
    }
    default:
        return nullptr;
    }
}

// 0x681F20
CTask* CTaskComplexPartner::CreateFirstSubTask(CPed* ped) {
    // Como el original: los bytes +0x58 (m_leadSpeaker) y +0x74/+0x75 (campos de CTaskComplexPartnerChat) se prueban enteros; el +0x74 se lee sin
    // comprobar el tipo real del objeto (sólo m_taskId == TASK_COMPLEX_PARTNER_CHAT)
    if (m_leadSpeaker && m_taskId == TASK_COMPLEX_PARTNER_CHAT) {
        auto* const chat = static_cast<CTaskComplexPartnerChat*>(this);
        if (chat->m_conversationEnabled) {
            if (CAEPedSpeechAudioEntity::RequestPedConversation(ped, m_partner)) {
                chat->m_pedConversationLoaded = true;
            } else if (chat->field_75) {
                chat->m_conversationEnabled = false;
            } else {
                return nullptr; // Como el original: no se crea subtarea ni se llama a StopPlayingHandSignal
            }
        }
    }
    ped->StopPlayingHandSignal();
    // Como el original: si el pool de tareas está agotado (operator new devuelve nulo) devuelve nulo sin construir
    void* const mem = CTask::operator new(sizeof(CTaskSimpleStandStill));
    if (!mem) {
        return nullptr;
    }
    return ::new (mem) CTaskSimpleStandStill(50, false, false, 8.0f);
}

// 0x6840D0
CTask* CTaskComplexPartner::ControlSubTask(CPed* ped) {
    // Como el original: sin abortar mientras el compañero siga con la misma tarea apuntando a este peatón (o, en los estados <= 1 con signo, mientras haya compañero)
    bool checkAbort = true;
    if ((int8)m_partnerState <= 1 && m_partner && !PartnerByte(m_taskCompleted)) {
        checkAbort = false;
    } else if (m_partner) {
        auto* const other = PartnerTaskOfPartner(*this);
        if (other && other->m_partner == ped && !PartnerByte(m_taskCompleted)) {
            checkAbort = false;
        }
    }
    if (checkAbort && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return nullptr;
    }

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
        auto& counter = PartnerCounter70(*this); // word con signo (cmp ...,96h / jle)
        counter++;
        if (counter > 150 && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return nullptr;
        }
    }

    ped->DropEntityThatThisPedIsHolding(true);
    StreamRequiredAnims();
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 500);
    }
    return m_pSubTask;
}

// 0x682310
void CTaskComplexPartner::StreamRequiredAnims() {
    // Como el original: el byte entero m_requiredAnimsStreamedIn (+0x6E); sin referencia, se pide/referencia el bloque (si el nombre no está vacío)
    // y se marca como cargado; el bloque cargado se compara con == 1 sobre el byte
    if (PartnerByte(m_requiredAnimsStreamedIn)) {
        return;
    }
    if (m_animBlockName[0] != '\0') {
        const auto blkIdx = CAnimManager::GetAnimationBlockIndex(m_animBlockName);
        if (reinterpret_cast<const uint8&>(CAnimManager::GetAnimBlocks()[blkIdx].IsLoaded) != 1) {
            CStreaming::RequestModel(IFPToModelId(blkIdx), STREAMING_KEEP_IN_MEMORY);
            return;
        }
        CAnimManager::AddAnimBlockRef(blkIdx);
    }
    m_requiredAnimsStreamedIn = true;
}

// 0x682370
void CTaskComplexPartner::RemoveStreamedAnims() {
    if (!PartnerByte(m_requiredAnimsStreamedIn)) {
        return;
    }
    if (m_animBlockName[0] != '\0') {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_animBlockName));
    }
    m_requiredAnimsStreamedIn = false;
}
