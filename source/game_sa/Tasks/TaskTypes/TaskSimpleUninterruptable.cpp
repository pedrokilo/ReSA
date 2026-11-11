/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleUninterruptable
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

#include "TaskSimpleUninterruptable.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// 0x43E3D0 (lote 158A, banco Fn158A): cuerpo no virtual de ~CTaskSimpleUninterruptable del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],859580h ; jmp 61A3A0`).
// Deja la vtable de CTaskSimpleUninterruptable del EXE (0x859580) y salta a CTask::Destructor (0x61A3A0, con hook), que deja la vtable de CTask.
__declspec(noinline) void __fastcall UninterruptableDestructorBody(CTaskSimpleUninterruptable* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x859580;
    self->CTask::Destructor();
}

// 0x43E3B0 (lote 158A, banco Fn158A): destructor con borrado de CTaskSimpleUninterruptable del EXE (ranura 0 de su vtable 0x859580; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x43E3D0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x43E3D0, aqui UninterruptableDestructorBody), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleUninterruptable* __fastcall UninterruptableScalarDeletingDestructor(CTaskSimpleUninterruptable* self, void* /*edx*/, uint8 flags) {
    UninterruptableDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleUninterruptable::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskSimpleUninterruptable, 0x859580, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x43E2E0);
    RH_ScopedNamedGlobalInstall(UninterruptableDestructorBody, "DestructorBody", 0x43E3D0);                        // lote 158A (banco Fn158A)
    RH_ScopedNamedGlobalInstall(UninterruptableScalarDeletingDestructor, "ScalarDeletingDestructor", 0x43E3B0);   // lote 158A

    RH_ScopedVMTInstall(Clone, 0x43E330);          // lote 158A: ranura 1 de la vtable 0x859580
    RH_ScopedVMTInstall(GetTaskType, 0x43E380);    // lote 158A: ranura 4
    RH_ScopedVMTInstall(MakeAbortable, 0x43E390);  // lote 158A: ranura 6
    RH_ScopedVMTInstall(ProcessPed, 0x43E3A0);     // lote 158A: ranura 7
}

// 0x43E380 (lote 158A, banco Fn158A; thiscall, `ret`, 6 bytes: `mov eax,0C9h`): TASK_SIMPLE_UNINTERRUPTABLE (201); no lee el objeto (ranura 4 de la vtable 0x859580).
eTaskType CTaskSimpleUninterruptable::GetTaskType() const {
    return Type;
}

// 0x43E330 (lote 158A, banco Fn158A; thiscall, sin argumentos, `ret`, 80 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); no lee `this`; ranura 1).
// Como el original: CTask::operator new (0x61A5A0, con hook) con 8 bytes; sin memoria devuelve nulo; si no, CTaskSimple::CTaskSimple (0x61A390, con hook; ECX = memoria), deja la vtable de la clase del
// EXE (0x859580; la de la DLL no vale para los llamadores del EXE) y devuelve la memoria.
CTask* CTaskSimpleUninterruptable::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleUninterruptable));
    if (!memory) {
        return nullptr;
    }
    CTaskSimpleBody::Constructor(memory, nullptr);
    *reinterpret_cast<uint32*>(memory) = 0x859580;
    return static_cast<CTaskSimpleUninterruptable*>(memory);
}

// 0x43E390 (lote 158A, banco Fn158A; thiscall, tres argumentos de pila, `ret 0Ch`, 11 bytes: `cmp dword [esp+8],2; sete al`): verdadero solo con la prioridad IMMEDIATE (2, dword completo); no lee ni el objeto, ni el ped ni el evento.
bool CTaskSimpleUninterruptable::MakeAbortable(CPed* /*ped*/, eAbortPriority priority, const CEvent* /*event*/) {
    return priority == ABORT_PRIORITY_IMMEDIATE;
}

// 0x43E3A0 (lote 158A, banco Fn158A; thiscall, un argumento de pila, `ret 4`, 5 bytes: `xor al,al`): siempre falso; no lee ni el objeto ni el ped.
bool CTaskSimpleUninterruptable::ProcessPed(CPed* /*ped*/) {
    return false;
}

CTaskSimpleUninterruptable* CTaskSimpleUninterruptable::Constructor()
{
    this->CTaskSimpleUninterruptable::CTaskSimpleUninterruptable();
    return this;
}

