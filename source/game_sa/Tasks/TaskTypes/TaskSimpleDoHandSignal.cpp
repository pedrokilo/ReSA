/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDoHandSignal
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
#include "TaskSimpleDoHandSignal.h"
#include "TaskComplexPlayHandSignalAnim.h"

namespace {
// 0x6608B0 (lote 153A, banco Fn153A): cuerpo del destructor de CTaskSimpleDoHandSignal del EXE (con hook (Destructor); vtable 0x86FA68). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor
// con borrado como el `call 0x6608B0` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleDoHandSignalDestructorBody(CTaskSimpleDoHandSignal* self, void* /*edx*/) { self->CTaskSimpleDoHandSignal::~CTaskSimpleDoHandSignal(); }

// 0x6644A0 (lote 153A, banco Fn153A): destructor con borrado de CTaskSimpleDoHandSignal del EXE (ranura 0 de su vtable 0x86FA68; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6608B0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleDoHandSignal* __fastcall CTaskSimpleDoHandSignalScalarDeletingDestructor(CTaskSimpleDoHandSignal* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleDoHandSignalDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote153ABody_CTaskSimpleDoHandSignal() { return reinterpret_cast<uintptr_t>(&CTaskSimpleDoHandSignalDestructorBody); } // para el banco Fn153A

void CTaskSimpleDoHandSignal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDoHandSignal, 0x86fa68, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleDoHandSignalScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6644A0); // lote 153A (banco Fn153A)

    RH_ScopedInstall(Constructor, 0x660880);
    RH_ScopedInstall(Destructor, 0x6608B0);

    RH_ScopedVMTInstall(Clone, 0x6608C0);
    RH_ScopedVMTInstall(GetTaskType, 0x6608A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x660930);
    RH_ScopedVMTInstall(ProcessPed, 0x660940);
}

CTaskSimpleDoHandSignal::CTaskSimpleDoHandSignal(const CTaskSimpleDoHandSignal& o) :
    m_Initialized{ o.m_Initialized }
{
}

// 0x660940
bool CTaskSimpleDoHandSignal::ProcessPed(CPed* ped) {
    if (!ped->GetIsOnScreen()) {
        return true;
    }

    const auto animTask = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);

    if (m_Initialized) {
        return !animTask || !notsa::isa<CTaskComplexPlayHandSignalAnim>(animTask);
    }

    if (animTask) {
        if (notsa::isa<CTaskComplexPlayHandSignalAnim>(animTask)) {
            return true;
        }
        animTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        return false;
    }

    ped->GetTaskManager().SetTaskSecondary(
        new CTaskComplexPlayHandSignalAnim{},
        TASK_SECONDARY_PARTIAL_ANIM
    );
    m_Initialized = true;
    return false;
}
