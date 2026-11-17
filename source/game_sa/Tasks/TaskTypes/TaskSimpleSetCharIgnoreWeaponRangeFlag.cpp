/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSetCharIgnoreWeaponRangeFlag
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleSetCharIgnoreWeaponRangeFlag.h"

namespace {
// 0x4748D0 (lote 223A, sin validar; el listado decia CTheScripts): destructor con borrado de CTaskSimpleSetCharIgnoreWeaponRangeFlag del EXE (ranura 0 de su vtable 0x859EBC; thiscall, un argumento de pila, `ret 4`,
// 30 bytes: `push esi; mov esi,ecx; call 0x4748F0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x4748F0, que es `jmp 0x61A3A0` = CTask::Destructor, con hook: deja la vtable de CTask), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y
// devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleSetCharIgnoreWeaponRangeFlag* __fastcall SetCharIgnoreWeaponRangeFlagScalarDeletingDestructor(CTaskSimpleSetCharIgnoreWeaponRangeFlag* self, void* /*edx*/, uint8 flags) {
    self->CTask::Destructor();
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleSetCharIgnoreWeaponRangeFlag::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetCharIgnoreWeaponRangeFlag, 0x859ebc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x474620);
    RH_ScopedInstall(Destructor, 0x4748F0);
    RH_ScopedNamedGlobalInstall(SetCharIgnoreWeaponRangeFlagScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4748D0); // lote 223A (sin validar)

    RH_ScopedVMTInstall(Clone, 0x474640);
    RH_ScopedVMTInstall(GetTaskType, 0x4746A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x4746B0);
    RH_ScopedVMTInstall(ProcessPed, 0x622B50);
}

// 0x474620
CTaskSimpleSetCharIgnoreWeaponRangeFlag::CTaskSimpleSetCharIgnoreWeaponRangeFlag(bool bIgnoreWeaponRange) :
    m_bIgnoreWeaponRange{ bIgnoreWeaponRange } {
}

// 0x474640
CTaskSimpleSetCharIgnoreWeaponRangeFlag::CTaskSimpleSetCharIgnoreWeaponRangeFlag(const CTaskSimpleSetCharIgnoreWeaponRangeFlag& o) :
    CTaskSimpleSetCharIgnoreWeaponRangeFlag{ o.m_bIgnoreWeaponRange } {
}
