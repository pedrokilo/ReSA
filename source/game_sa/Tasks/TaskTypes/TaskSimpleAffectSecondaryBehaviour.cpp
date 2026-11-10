/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleAffectSecondaryBehaviour
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
#include "TaskSimpleAffectSecondaryBehaviour.h"

void CTaskSimpleAffectSecondaryBehaviour::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAffectSecondaryBehaviour, 0x870b08, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x691270);
    RH_ScopedInstall(Destructor, 0x6912C0); // lote 130A (banco Fn130A)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x692FB0); // lote 151A (banco Fn151A)

    RH_ScopedVMTInstall(Clone, 0x692910);
    RH_ScopedVMTInstall(GetTaskType, 0x6912A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6912B0);
    RH_ScopedVMTInstall(ProcessPed, 0x691320);
}

// 0x691270
CTaskSimpleAffectSecondaryBehaviour::CTaskSimpleAffectSecondaryBehaviour(bool add, eSecondaryTask secondaryTaskType, CTask* task) :
    m_bAdd{add},
    m_secTaskType{secondaryTaskType},
    m_task{task}
{
}

// NOTSA
CTaskSimpleAffectSecondaryBehaviour::CTaskSimpleAffectSecondaryBehaviour(const CTaskSimpleAffectSecondaryBehaviour& o) :
    CTaskSimpleAffectSecondaryBehaviour{o.m_bAdd, o.m_secTaskType, o.m_task ? o.m_task->Clone() : nullptr}
{
}

// 0x6912C0
CTaskSimpleAffectSecondaryBehaviour::~CTaskSimpleAffectSecondaryBehaviour() {
    delete m_task; // Como el original: la tarea guardada se borra con su destructor virtual
}

// 0x6912C0 (lote 130A, banco Fn130A): cuerpo del destructor del EXE, thiscall sin argumentos, `ret`, con marco SEH (solo desenrolla; aqui se omite). Como el original: deja la vtable de la clase del EXE
// (0x870B08), llama a la ranura 0 de la vtable de la tarea guardada (+0x10; destructor con borrado, `push 1`) si no es nula (sin ponerla a cero) y termina con CTask::Destructor (0x61A3A0).
CTaskSimpleAffectSecondaryBehaviour* CTaskSimpleAffectSecondaryBehaviour::Destructor() {
    *reinterpret_cast<uint32*>(this) = 0x870B08;
    if (m_task) {
        delete m_task;
    }
    CTask::Destructor();
    return this;
}

// 0x692FB0 (lote 151A, banco Fn151A; el listado decia CTaskSimpleFacial): destructor con borrado de CTaskSimpleAffectSecondaryBehaviour del EXE (ranura 0 de su vtable 0x870B08; thiscall, un argumento de pila,
// `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6912C0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x6912C0, con hook propio: Destructor), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleAffectSecondaryBehaviour* CTaskSimpleAffectSecondaryBehaviour::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x692910
CTask* CTaskSimpleAffectSecondaryBehaviour::Clone() const {
    // Como el original: la tarea guardada se clona antes de reservar la nueva tarea en el pool
    const auto task = m_task ? m_task->Clone() : nullptr;
    return new CTaskSimpleAffectSecondaryBehaviour(m_bAdd, m_secTaskType, task);
}

// 0x691320
bool CTaskSimpleAffectSecondaryBehaviour::ProcessPed(CPed* ped) {
    const auto currSecTask = ped->GetTaskManager().GetTaskSecondary(m_secTaskType);

    if (m_bAdd) {
        if (!currSecTask || currSecTask->MakeAbortable(ped)) {
            ped->GetTaskManager().SetTaskSecondary(m_task->Clone(), m_secTaskType);
            return true;
        }
        return false;
    }

    // Como el original: sin tarea secundaria termina; si la hay se pide abortar (LEISURE) y se sigue esperando
    if (currSecTask) {
        currSecTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        return false;
    }
    return true;
}
