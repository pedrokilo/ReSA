/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFallAndGetUp
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleFall.h"
#include "TaskSimpleGetUp.h"

namespace {
// 0x678790 (lote 154A, banco Fn154A): cuerpo del destructor de CTaskComplexFallAndGetUp del EXE (enganchado desde el lote 191A: `mov [ecx],0x870454; jmp 0x61A3D0`, 11 bytes; vtable 0x870454). Funcion propia de la DLL, sin expansion en linea, para que el banco pueda
// sustituirla; la llama el destructor con borrado como el `call 0x678790` del EXE.
__declspec(noinline) void __fastcall CTaskComplexFallAndGetUpDestructorBody(CTaskComplexFallAndGetUp* self, void* /*edx*/) { self->CTaskComplexFallAndGetUp::~CTaskComplexFallAndGetUp(); }

// 0x67CB50 (lote 154A, banco Fn154A): destructor con borrado de CTaskComplexFallAndGetUp del EXE (ranura 0 de su vtable 0x870454; thiscall, un argumento de pila, `ret 4`, 30 bytes, cuerpo 0x678790). Libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexFallAndGetUp* __fastcall CTaskComplexFallAndGetUpScalarDeletingDestructor(CTaskComplexFallAndGetUp* self, void* /*edx*/, uint8 flags) {
    CTaskComplexFallAndGetUpDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote154ABody_CTaskComplexFallAndGetUp() { return reinterpret_cast<uintptr_t>(&CTaskComplexFallAndGetUpDestructorBody); } // para el banco Fn154A

static_assert(CTaskComplexFallAndGetUp::Type == 0xD0); // 0x6786F0: `mov eax,0D0h`

// 0x6786F0 (lote 191A, sin validar; `mov eax,0D0h; ret`, 6 bytes; ranura 4 de la vtable 0x870454): no lee el objeto.
eTaskType CTaskComplexFallAndGetUp::GetTaskType() const {
    return Type;
}

void CTaskComplexFallAndGetUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFallAndGetUp, 0x870454, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexFallAndGetUpDestructorBody, "DestructorBody", 0x678790); // lote 191A (sin validar): el cuerpo ya existia sin enganchar (lote 154A)
    RH_ScopedVMTInstall(GetTaskType, 0x6786F0); // lote 191A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x67C200);       // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(CTaskComplexFallAndGetUpScalarDeletingDestructor, "ScalarDeletingDestructor", 0x67CB50); // lote 154A (banco Fn154A)
    RH_ScopedInstall(Constructor, 0x6786C0);
    RH_ScopedInstall(Constructor2, 0x678700);
    RH_ScopedInstall(CreateSubTask, 0x678900);
    RH_ScopedInstall(IsFalling, 0x6787D0);
    RH_ScopedInstall(SetDownTime, 0x6787A0);
    // VTABLE
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x678870);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67CB70);
    RH_ScopedVMTInstall(ControlSubTask, 0x6788F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6787F0);
}

CTaskComplexFallAndGetUp* CTaskComplexFallAndGetUp::Constructor(AnimationId nFallAnimId, AssocGroupId nFallAnimGroup, int32 nFallDownTime) {
    this->CTaskComplexFallAndGetUp::CTaskComplexFallAndGetUp(nFallAnimId, nFallAnimGroup, nFallDownTime);
    return this;
}

CTaskComplexFallAndGetUp* CTaskComplexFallAndGetUp::Constructor2(int32 nDir, int32 nFallDownTime) {
    this->CTaskComplexFallAndGetUp::CTaskComplexFallAndGetUp(nDir, nFallDownTime);
    return this;
}

// 0x67C200 (lote 200A, sin validar; thiscall, sin argumentos, `ret`, 106 bytes con marco SEH que solo desenrolla): reserva 0x18 bytes con CTask::operator new (0x61A5A0; sin memoria devuelve nulo) y construye una copia: lee
// ANTES de construir el tiempo (+0x14), el grupo (+0x10) y la animacion (+0xC) y, tras la base CTaskComplex (0x61A3B0), escribe +0x10 = grupo, +0x14 = tiempo, vtable 0x870454 y +0xC = animacion (el constructor 0x6786C0).
CTask* CTaskComplexFallAndGetUp::Clone() const {
    const AnimationId  animId    = m_nFallAnimId;
    const AssocGroupId animGroup = m_nFallAnimGroup;
    const int32        downTime  = m_nFallDownTime;
    void* const memory = CTask::operator new(sizeof(CTaskComplexFallAndGetUp));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskComplexFallAndGetUp(animId, animGroup, downTime);
}

// 0x6786C0
CTaskComplexFallAndGetUp::CTaskComplexFallAndGetUp(AnimationId nFallAnimId, AssocGroupId nFallAnimGroup, int32 nFallDownTime) {
    m_nFallAnimId    = nFallAnimId;
    m_nFallAnimGroup = nFallAnimGroup;
    m_nFallDownTime  = nFallDownTime;
}

// 0x678700
CTaskComplexFallAndGetUp::CTaskComplexFallAndGetUp(int32 nDir, int32 nFallDownTime) {
    m_nFallDownTime = nFallDownTime;
    m_nFallAnimGroup = ANIM_GROUP_DEFAULT;

    switch (nDir) {
    case 0:
        m_nFallAnimId = ANIM_ID_KO_SKID_FRONT;
        break;
    case 1:
        m_nFallAnimId = ANIM_ID_KO_SPIN_R;
        break;
    case 2:
        m_nFallAnimId = ANIM_ID_KO_SKID_BACK;
        break;
    case 3:
        m_nFallAnimId = ANIM_ID_KO_SPIN_L;
        break;
    default:
        break;
    }
}

// 0x678870
CTask* CTaskComplexFallAndGetUp::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_FALL);
}

// 0x67CB70
CTask* CTaskComplexFallAndGetUp::CreateNextSubTask(CPed* ped) {
    auto subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_SIMPLE_FALL)
        return CreateSubTask(TASK_SIMPLE_GET_UP);

    if (subTaskType == TASK_SIMPLE_GET_UP)
        return CreateSubTask(TASK_FINISHED);

    return nullptr;
}

// 0x6788F0
CTask* CTaskComplexFallAndGetUp::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}

// 0x6787F0
bool CTaskComplexFallAndGetUp::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE)
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);

    if (ped->m_nPedState == PEDSTATE_ARRESTED || ped->bIsBeingArrested)
        return false;

    if (priority == ABORT_PRIORITY_URGENT)
        return m_pSubTask->MakeAbortable(ped, priority, event);

    // Como el original: cualquier otra prioridad mira el tipo de la subtarea sin comprobar si es nula,
    // pone el tiempo de caída a 0, aborta la subtarea y devuelve false sin mirar su resultado
    if (m_pSubTask->GetTaskType() != TASK_SIMPLE_FALL) {
        m_nFallDownTime = 0;
        m_pSubTask->MakeAbortable(ped, priority, event);
    }
    return false;
}

// 0x678900
CTask* CTaskComplexFallAndGetUp::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_FALL:
        return new CTaskSimpleFall(m_nFallAnimId, m_nFallAnimGroup, m_nFallDownTime);
    case TASK_SIMPLE_GET_UP:
        return new CTaskSimpleGetUp();
    case TASK_FINISHED:
    default:
        return nullptr;
    }
}

// 0x6787D0
bool CTaskComplexFallAndGetUp::IsFalling() {
    // Como el original: sin subtarea no está cayendo
    return m_pSubTask && m_pSubTask->GetTaskType() == TASK_SIMPLE_FALL;
}

// 0x6787A0
void CTaskComplexFallAndGetUp::SetDownTime(int32 nTime) {
    m_nFallDownTime = nTime;
    // Como el original: consulta el tipo de la subtarea sin comprobar si es nula
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_FALL) {
        auto pFallTask = reinterpret_cast<CTaskSimpleFall*>(m_pSubTask);
        pFallTask->m_nTotalDownTime = nTime;
        pFallTask->m_nCurrentDownTime = nTime;
    }
}
