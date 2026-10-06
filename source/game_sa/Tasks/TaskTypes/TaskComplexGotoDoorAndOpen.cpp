/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGotoDoorAndOpen
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

#include "TaskComplexGotoDoorAndOpen.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleRunAnim.h"
#include "EventAreaCodes.h"
#include "EventGroup.h"
#include "Object.h"
#include "Pad.h"
#include "AnimManager.h"
#include "RwHelper.h"

void CTaskComplexGotoDoorAndOpen::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGotoDoorAndOpen, 0x86FFC0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "0", 0x66BB20, CTaskComplexGotoDoorAndOpen*(CTaskComplexGotoDoorAndOpen::*)(CObject *));
    RH_ScopedOverloadedInstall(Constructor, "1", 0x66BBA0, CTaskComplexGotoDoorAndOpen*(CTaskComplexGotoDoorAndOpen::*)(CVector const&, CVector const&));
    RH_ScopedInstall(Destructor, 0x66BC00);
    RH_ScopedVMTInstall(Clone, 0x66BCA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66C0D0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66BD40);
    RH_ScopedVMTInstall(ControlSubTask, 0x66C1F0);
}

// 0x66BB20
CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(CObject* door) : CTaskComplex() {
    m_Object = door;
    m_nStartTime = 0;
    m_nOffsetTime = 0;
    byte30 = 0;
    m_bRefreshTime = false;
    m_nFlags = m_nFlags & 0xF0 | 1; // todo: flags
    CEntity::SafeRegisterRef(m_Object);
}

// 0x66BBA0
CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end) : CTaskComplex() {
    m_Object = nullptr;
    m_Start = start;
    m_End = end;
    m_nStartTime = 0;
    m_nOffsetTime = 0;
    byte30 = 0;
    m_bRefreshTime = false;
    m_nFlags &= 244u; // todo: flags
}

// 0x66BC00
CTaskComplexGotoDoorAndOpen::~CTaskComplexGotoDoorAndOpen() {
    CEntity::SafeCleanUpRef(m_Object);
    if ((m_nFlags & 8) != 0) { // transition finished
        CPad::GetPad()->bPlayerOnInteriorTransition = false;
    }
}

// 0x66BCA0
CTask* CTaskComplexGotoDoorAndOpen::Clone() const {
    if ((m_nFlags & 1) != 0) { // todo: flags
        return new CTaskComplexGotoDoorAndOpen(m_Object);
    } else {
        return new CTaskComplexGotoDoorAndOpen(m_Start, m_End);
    }
}

// 0x66BC80
bool CTaskComplexGotoDoorAndOpen::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
}

// 0x66C0D0
CTask* CTaskComplexGotoDoorAndOpen::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() != TASK_SIMPLE_GO_TO_POINT) {
        return nullptr;
    }
    // Como el original: con el segundo desplazamiento ya hecho solo se avisa al pad del jugador
    if (m_nFlags & 2) { // todo: flags
        if (ped->IsPlayer()) {
            CPad::GetPad(0)->bPlayerOnInteriorTransition = true;
        }
        return nullptr;
    }
    // Como el original: si no hay tarea secundaria en la ranura 4 se crea una animación parcial (grupo 0, animación 0x99)
    auto& taskMgr = ped->GetTaskManager();
    if (!taskMgr.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
        taskMgr.SetTaskSecondary(new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0), static_cast<AnimationId>(0x99), 8.0f, false), TASK_SECONDARY_PARTIAL_ANIM);
    }
    CTask* const task = new CTaskSimpleGoToPoint(PEDMOVE_WALK, m_End, 0.2f, false, false);
    m_nFlags |= 2; // todo: flags
    return task;
}

// 0x66BD40
CTask* CTaskComplexGotoDoorAndOpen::CreateFirstSubTask(CPed* ped) {
    // Como el original: el jugador marca la transición de interior en su pad y en esta tarea
    if (ped->IsPlayer()) {
        CPad::GetPad(0)->bPlayerOnInteriorTransition = true;
        m_nFlags |= 8; // todo: flags
    }

    // Como el original: evento de códigos de área en el grupo global (no se clona: se añade la dirección de la pila)
    CEventAreaCodes event{ ped };
    GetEventGlobalGroup()->Add(static_cast<CEvent*>(&event), false);

    m_nStartTime  = CTimer::m_snTimeInMilliseconds;
    m_nOffsetTime = 1000;
    byte30        = 1;

    CObject* const door = m_Object;
    if (!door) {
        if (m_nFlags & 1) { // todo: flags
            return nullptr;
        }
        m_nFlags |= 2; // todo: flags
        return new CTaskSimpleGoToPoint(PEDMOVE_WALK, m_End, 0.5f, false, false);
    }

    // Como el original: la puerta debe tener matriz (se lee sin guarda de nulo)
    const CVector doorPos = door->GetPosition();
    const CMatrix& doorMat = *door->m_matrix;
    const CVector right = doorMat.GetRight();
    const CVector fwd = doorMat.GetForward();
    const CVector& pedPos = ped->GetPosition();

    // Como el original: producto escalar (fy·dy + fz·dz) + fx·dx entre la dirección de la puerta y el vector al peatón
    const float dx = doorPos.x - pedPos.x;
    const float dy = doorPos.y - pedPos.y;
    const float dz = doorPos.z - pedPos.z;
    float dot = fwd.y * dy + fwd.z * dz;
    dot += fwd.x * dx;
    const bool front = dot > 0.0f; // con NaN es falso

    // Como el original: se desplaza 0,75 hacia la derecha y luego -0,5 (inicio) y 2,0 (fin) sobre la dirección, con signo según el lado
    const float px = right.x * 0.75f + doorPos.x;
    const float py = doorPos.y + right.y * 0.75f;
    const float pz = right.z * 0.75f + doorPos.z;
    if (front) {
        m_Start.x = fwd.x * -0.5f + px;
        m_Start.y = py + fwd.y * -0.5f;
        m_Start.z = pz + fwd.z * -0.5f;
        m_End.x   = fwd.x * 2.0f + px;
        m_End.y   = py + fwd.y * 2.0f;
        m_End.z   = pz + fwd.z * 2.0f;
    } else {
        m_Start.x = px - fwd.x * -0.5f;
        m_Start.y = py - fwd.y * -0.5f;
        m_Start.z = pz - fwd.z * -0.5f;
        m_End.x   = px - fwd.x * 2.0f;
        m_End.y   = py - fwd.y * 2.0f;
        m_End.z   = pz - fwd.z * 2.0f;
    }

    // Como el original: bit 3 de las banderas físicas de la puerta (se apaga junto con el bit 2)
    if (door->m_nPhysicalFlags & 8) {
        m_nFlags |= 4; // todo: flags
        door->m_nPhysicalFlags &= ~8u;
        m_Object->m_nPhysicalFlags &= ~4u;
    }

    // Como el original: si el peatón reproduce la animación 2 se vuelve a la de andar (id 0) con mezcla de 1000
    if (RpAnimBlendClumpGetAssociation(ped->GetRpClump(), 2u)) {
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_WALK, 1000.0f);
    }

    return new CTaskSimpleGoToPoint(PEDMOVE_WALK, m_Start, 0.35f, false, false);
}

// 0x66C1F0
CTask* CTaskComplexGotoDoorAndOpen::ControlSubTask(CPed* ped) {
    bool abort{};
    if (byte30) {
        if (m_bRefreshTime) {
            m_nStartTime   = CTimer::m_snTimeInMilliseconds;
            m_bRefreshTime = false;
        }
        // Como el original: comparación sin signo del tiempo transcurrido
        abort = !(CTimer::m_snTimeInMilliseconds < m_nOffsetTime + m_nStartTime);
    }
    if (!abort) {
        if (m_Object || !(m_nFlags & 1)) { // todo: flags
            return m_pSubTask;
        }
    }
    if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        if (ped->IsPlayer()) {
            CPad::GetPad(0)->bPlayerOnInteriorTransition = true;
        }
        return nullptr;
    }
    return m_pSubTask;
}
