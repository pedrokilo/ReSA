/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFallAndStayDown
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

#include "TaskComplexFallAndStayDown.h"
#include "TaskSimpleFall.h"

static_assert(CTaskComplexFallAndStayDown::Type == 0xD1);  // 0x678A00: `mov eax,0D1h`
static_assert(TASK_SIMPLE_FALL == 0xCF);                   // 0x678AA6.., 0x678BF0: `cmp eax,0CFh`
static_assert(TASK_FINISHED == 0x516);                     // 0x678BF8 y 0x67CBF5: 0x516
static_assert(ANIM_ID_KO_SKID_FRONT == 0x18 && ANIM_ID_KO_SPIN_R == 0x19 && ANIM_ID_KO_SKID_BACK == 0x1A && ANIM_ID_KO_SPIN_L == 0x1B); // tabla de saltos 0x678A80
static_assert(ABORT_PRIORITY_URGENT == 1 && ABORT_PRIORITY_IMMEDIATE == 2);

void CTaskComplexFallAndStayDown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFallAndStayDown, 0x870480, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6789D0);    // lote 191A (sin validar)
    RH_ScopedInstall(Constructor2, 0x678A10);   // lote 191A (sin validar)
    RH_ScopedInstall(Destructor, 0x678A90);     // lote 191A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x67CBC0); // lote 191A (sin validar)
    RH_ScopedInstall(CreateSubTask, 0x678BD0);  // lote 191A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x678A00);          // lote 191A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x67C270);                // CTaskComplex vecino de la ranura 1; necesario para completar la clase (no estaba en el lote), sin validar
    RH_ScopedVMTInstall(MakeAbortable, 0x678AA0);        // lote 191A (sin validar)
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67CBE0);    // idem Clone (extra, no estaba en el lote), sin validar
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x678B20);   // lote 191A (sin validar)
    RH_ScopedVMTInstall(ControlSubTask, 0x678BC0);       // lote 191A (sin validar)
}

// 0x6789D0 (lote 191A, sin validar; thiscall, dos argumentos de pila, `ret 8`, 34 bytes): base CTaskComplex (0x61A3B0), +0xC = animacion, vtable de la clase del EXE (0x870480), +0x10 = grupo; devuelve this.
CTaskComplexFallAndStayDown::CTaskComplexFallAndStayDown(AnimationId fallAnimId, AssocGroupId fallAnimGroup) :
    CTaskComplex(),
    m_nFallAnimId{fallAnimId},
    m_nFallAnimGroup{fallAnimGroup}
{
}

// 0x678A10 (lote 191A, sin validar; thiscall, un argumento de pila, `ret 4`, 110 bytes): base CTaskComplex (0x61A3B0), vtable 0x870480 y tabla de saltos 0x678A80 por direccion (con signo sin comprobar: sin signo y > 3
// salta al final): 0 -> KO_SKID_FRONT (0x18), 1 -> KO_SPIN_R (0x19), 2 -> KO_SKID_BACK (0x1A), 3 -> KO_SPIN_L (0x1B); cualquier otro valor NO escribe +0xC (queda sin inicializar). +0x10 = 0 (grupo por defecto) siempre.
CTaskComplexFallAndStayDown::CTaskComplexFallAndStayDown(int32 direction) :
    CTaskComplex()
{
    switch (uint32(direction)) {
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
    m_nFallAnimGroup = ANIM_GROUP_DEFAULT;
}

// 0x678A00 (lote 191A, sin validar; `mov eax,0D1h; ret`, 6 bytes; ranura 4 de la vtable 0x870480): no lee el objeto.
eTaskType CTaskComplexFallAndStayDown::GetTaskType() const {
    return Type;
}

// 0x67C270 (extra, sin validar; thiscall, sin argumentos, `ret`, marco SEH que solo desenrolla): reserva 0x14 bytes con CTask::operator new (0x61A5A0; sin memoria devuelve nulo) y construye una copia con base CTaskComplex
// (0x61A3B0), +0x10 = grupo, vtable 0x870480 y +0xC = animacion (el EXE guarda los dos campos en registros antes de construir).
CTask* CTaskComplexFallAndStayDown::Clone() const {
    const AnimationId  animId    = m_nFallAnimId;
    const AssocGroupId animGroup = m_nFallAnimGroup;
    void* const memory = CTask::operator new(sizeof(CTaskComplexFallAndStayDown));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskComplexFallAndStayDown(animId, animGroup);
}

// 0x678AA0 (lote 191A, sin validar; thiscall, ped/prioridad/evento en la pila, `ret 0Ch`, 118 bytes; ranura 6). Como el original, SIN guarda de nulo sobre la subtarea:
//  - prioridad 2 (inmediata): salta a MakeAbortable de la subtarea (ranura 6) con (ped, 2, evento) y devuelve su resultado;
//  - prioridad 1 (urgente): pide el tipo de la subtarea (ranura 4); si es TASK_SIMPLE_FALL (0xCF) devuelve false sin llamar a MakeAbortable; si no, devuelve el resultado de MakeAbortable(ped, 1, evento);
//  - cualquier otra: pide el tipo; si no es TASK_SIMPLE_FALL llama a MakeAbortable(ped, prioridad, evento) IGNORANDO el resultado; devuelve siempre false.
bool CTaskComplexFallAndStayDown::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
    }
    if (priority == ABORT_PRIORITY_URGENT) {
        bool aborted = false;
        if (m_pSubTask->GetTaskType() != TASK_SIMPLE_FALL) {
            aborted = m_pSubTask->MakeAbortable(ped, priority, event);
        }
        return aborted;
    }
    if (m_pSubTask->GetTaskType() != TASK_SIMPLE_FALL) {
        m_pSubTask->MakeAbortable(ped, priority, event);
    }
    return false;
}

// 0x67CBE0 (extra, sin validar; thiscall, un argumento de pila, `ret 4`, 46 bytes; ranura 8): pide el tipo de la subtarea (sin guarda de nulo); si es TASK_SIMPLE_FALL (0xCF) devuelve CreateSubTask(TASK_FINISHED = 0x516)
// (0x678BD0, que da nulo); con cualquier otro tipo devuelve nulo.
CTask* CTaskComplexFallAndStayDown::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_FALL) {
        return CreateSubTask(TASK_FINISHED);
    }
    return nullptr;
}

// 0x678B20 (lote 191A, sin validar; thiscall, un argumento de pila, `ret 4`, 145 bytes con marco SEH que solo desenrolla; ranura 9). Como el original: reserva 0x20 bytes con CTask::operator new (0x61A5A0; sin memoria
// devuelve nulo) y construye CTaskSimpleFall (base CTaskSimple 0x61A390, grupo +0x10, animacion +0xC, tiempos +0x18 y +0x1C = 99999999 (0x5F5E0FF), vtable 0x870430, +8 = 0 y +0x14 = 0) con los campos de esta tarea.
CTask* CTaskComplexFallAndStayDown::CreateFirstSubTask(CPed* ped) {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleFall));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskSimpleFall(m_nFallAnimId, m_nFallAnimGroup, 99999999);
}

// 0x678BC0 (lote 191A, sin validar; `mov eax,[ecx+8]; ret 4`, 6 bytes; ranura 10): devuelve la subtarea sin tocarla.
CTask* CTaskComplexFallAndStayDown::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}

// 0x678BD0 (lote 191A, sin validar; thiscall, tipo de tarea en la pila, `ret 4`, 167 bytes con marco SEH que solo desenrolla; llamada desde CreateNextSubTask 0x67CBFC): solo TASK_SIMPLE_FALL (0xCF) crea algo: igual que
// CreateFirstSubTask (0x20 bytes con CTask::operator new, nulo si no hay memoria, CTaskSimpleFall con los campos de esta tarea y 99999999). TASK_FINISHED (0x516) y cualquier otro tipo devuelven nulo.
CTask* CTaskComplexFallAndStayDown::CreateSubTask(eTaskType taskType) {
    if (taskType != TASK_SIMPLE_FALL) {
        return nullptr;
    }
    void* const memory = CTask::operator new(sizeof(CTaskSimpleFall));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskSimpleFall(m_nFallAnimId, m_nFallAnimGroup, 99999999);
}

// 0x67CBC0 (lote 191A, sin validar): destructor con borrado de CTaskComplexFallAndStayDown del EXE (ranura 0 de su vtable 0x870480; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x678A90; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x678A90; aqui su
// envoltura Destructor()), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexFallAndStayDown* CTaskComplexFallAndStayDown::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
