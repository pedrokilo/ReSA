/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSetStayInSamePlace
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

#include "TaskSimpleSetStayInSamePlace.h"

namespace {
// 0x62F5D0 (lote 122A, banco Fn122A): cuerpo no virtual de ~CTaskSimpleSetStayInSamePlace del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx], 0x86DD50 ; jmp 0x61A3A0`).
// Deja la vtable de CTaskSimpleSetStayInSamePlace del EXE (0x86DD50) y salta a CTask::Destructor (0x61A3A0), que deja la vtable de CTask (0x86D48C). Lo llaman el funclet de limpieza (SEH) y el destructor con borrado.
void __fastcall SetStayInSamePlaceDestructorBody(CTaskSimpleSetStayInSamePlace* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DD50;
    self->CTask::Destructor();
}

// 0x6374A0 (lote 183A, sin validar; el listado decia CTaskSimpleHitFromFront): destructor con borrado de CTaskSimpleSetStayInSamePlace del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x62F5D0;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). El cuerpo es el de ~CTaskSimpleSetStayInSamePlace (0x62F5D0, con hook: vtable 0x86DD50 y CTask::Destructor). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskSimpleSetStayInSamePlace* __fastcall SetStayInSamePlaceScalarDeletingDestructor(CTaskSimpleSetStayInSamePlace* self, void* /*edx*/, uint8 flags) {
    SetStayInSamePlaceDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleSetStayInSamePlace::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetStayInSamePlace, 0x86DD50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(SetStayInSamePlaceScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6374A0); // lote 183A (sin validar)

    RH_ScopedNamedGlobalInstall(SetStayInSamePlaceDestructorBody, "Destructor", 0x62F5D0);
    RH_ScopedInstall(Constructor, 0x62F590);

    RH_ScopedVMTInstall(Clone, 0x635BD0);
    RH_ScopedVMTInstall(GetTaskType, 0x62F5B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62F5C0);
    RH_ScopedVMTInstall(ProcessPed, 0x62F5E0);
}

// 0x62F590
CTaskSimpleSetStayInSamePlace::CTaskSimpleSetStayInSamePlace(bool bStayInSamePlace) : CTaskSimple() {
    m_bStayInSamePlace = bStayInSamePlace;
}

// 0x62F5E0
bool CTaskSimpleSetStayInSamePlace::ProcessPed(CPed* ped) {
    ped->bStayInSamePlace = m_bStayInSamePlace;
    return true;
}
