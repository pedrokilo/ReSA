/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGotoDoorAndOpen
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
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
#include "X87Math.h"

static_assert(CTaskComplexGotoDoorAndOpen::Type == 0x3A4); // 0x66BB90: `mov eax,3A4h`
static_assert(ABORT_PRIORITY_IMMEDIATE == 2);              // 0x66BC80: `cmp dword [esp+8],2`

namespace {
// 0x66C280 (lote 191A, sin validar; el listado lo atribuia a CTaskComplexGotoDoorAndOpen): funcion del EXE SIN llamadores ni punteros (ni llamada directa, ni salto, ni dword en ninguna seccion; codigo muerto).
// thiscall, un argumento de pila que no se lee (`ret 4`), 151 bytes con marco SEH que solo desenrolla. NO es de CTaskComplexGotoDoorAndOpen: `this + 0xC` es un CEntryExit* (rectangulo de entrada en +8..+0x14 y
// Z en +0x18), asi que es de una tarea sin identificar con ese puntero. Como el original: calcula el centro de la entrada (X87 de CEntryExit::GetPosition en X87Math.cpp), reserva 0x30 bytes con
// CTask::operator new (0x61A5A0; sin memoria devuelve nulo) y construye CTaskSimpleGoToPoint (0x667CD0, con hook) con (PEDMOVE_RUN = 6, centro, 0.5f, false, false).
CTask* __fastcall GoToEntryExitCenterTask(void* self, void* /*edx*/, void* /*unused*/) {
    CVector center;
    EntryExitGetPositionCpp(*reinterpret_cast<void* const*>(static_cast<const uint8*>(self) + 0xC), &center);
    void* const memory = CTask::operator new(sizeof(CTaskSimpleGoToPoint));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskSimpleGoToPoint(PEDMOVE_RUN, center, 0.5f, false, false);
}
} // namespace

namespace {
// 0x6711F0 (lote 210A, sin validar; el listado lo atribuia a otra clase): destructor con borrado de CTaskComplexGotoDoorAndOpen del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x66BC00;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del
// BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexGotoDoorAndOpen* __fastcall GotoDoorAndOpenScalarDeletingDestructor(CTaskComplexGotoDoorAndOpen* self, void* /*edx*/, uint8 flags) {
    self->Destructor(); // cuerpo enganchado (con hook)
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

// 0x66BB90 (lote 191A, sin validar; `mov eax,3A4h; ret`, 6 bytes; ranura 4 de la vtable 0x86FFC0): no lee el objeto.
eTaskType CTaskComplexGotoDoorAndOpen::GetTaskType() const {
    return Type;
}

void CTaskComplexGotoDoorAndOpen::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGotoDoorAndOpen, 0x86FFC0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "0", 0x66BB20, CTaskComplexGotoDoorAndOpen*(CTaskComplexGotoDoorAndOpen::*)(CObject *));
    RH_ScopedOverloadedInstall(Constructor, "1", 0x66BBA0, CTaskComplexGotoDoorAndOpen*(CTaskComplexGotoDoorAndOpen::*)(CVector const&, CVector const&));
    RH_ScopedInstall(Destructor, 0x66BC00);
    RH_ScopedNamedGlobalInstall(GotoDoorAndOpenScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6711F0); // lote 210A (sin validar)
    RH_ScopedNamedGlobalInstall(GoToEntryExitCenterTask, "GoToEntryExitCenterTask", 0x66C280); // lote 191A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x66BB90);   // lote 191A (sin validar)
    RH_ScopedVMTInstall(MakeAbortable, 0x66BC80); // lote 191A (sin validar)
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

// 0x66BC80 (lote 191A, sin validar; thiscall, ped/prioridad/evento en la pila, `ret 0Ch`, 28 bytes; ranura 6): solo con prioridad 2 (inmediata) salta SIN guarda de nulo a MakeAbortable de la subtarea (ranura 6, mismos
// argumentos con la prioridad 2) y devuelve su resultado; con otra prioridad devuelve false sin tocar la subtarea.
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
