/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDuckToggle
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

#include "TaskSimpleDuckToggle.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleUseGun.h"

namespace {
// 0x62F6D0: cuerpo no virtual de ~CTaskSimpleDuckToggle del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx], 0x86DD98 ; jmp 0x61A3A0`). Deja la vtable de la clase del EXE (0x86DD98) y salta a CTask::Destructor (0x61A3A0),
// que deja la vtable de CTask. El hook `Destructor` de esta clase (0x62F6D0) usa el destructor del compilador, que escribiria la vtable de la DLL: aqui se reproduce el cuerpo del EXE.
void __fastcall DuckToggleDestructorBodyExe(CTaskSimpleDuckToggle* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DD98;
    self->CTask::Destructor();
}

// 0x6374E0 (lote 183A, sin validar; el listado decia CTaskSimpleHitFromFront): destructor con borrado de CTaskSimpleDuckToggle del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x62F6D0;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). El cuerpo es el de la clase (0x62F6D0, con hook). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskSimpleDuckToggle* __fastcall DuckToggleScalarDeletingDestructor(CTaskSimpleDuckToggle* self, void* /*edx*/, uint8 flags) {
    DuckToggleDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleDuckToggle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDuckToggle, 0x86dd98, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(DuckToggleScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6374E0); // lote 183A (sin validar)

    RH_ScopedInstall(Constructor, 0x62F690);
    RH_ScopedInstall(Destructor, 0x62F6D0);

    RH_ScopedVMTInstall(Clone, 0x635C90);
    RH_ScopedVMTInstall(GetTaskType, 0x62F6B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62F6C0);
    RH_ScopedVMTInstall(ProcessPed, 0x62F6E0);
}

// 0x62F690
CTaskSimpleDuckToggle::CTaskSimpleDuckToggle(eMode mode) :
    m_ToggleMode{ mode }
{
}

// 0x62F6E0
bool CTaskSimpleDuckToggle::ProcessPed(CPed* ped) {
    auto* const intel = ped->GetIntelligence();

    // Try to toggle off
    if (auto* const tDuck = intel->GetTaskDuck(false); tDuck && notsa::contains({ eMode::OFF, eMode::AUTO }, m_ToggleMode)) {
        tDuck->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
        if (auto* const tUseGun = intel->GetTaskUseGun()) {
            tUseGun->ClearAnim(ped);
        }
        return true;
    }

    // Try to toggle on
    if (notsa::contains({ eMode::ON, eMode::AUTO }, m_ToggleMode)) {
        auto* const tSimplestDuck  = notsa::dyn_cast_if_present<CTaskSimpleDuck>(intel->GetTaskManager().GetSimplestActiveTask()); // Como el original: la tarea mas simple puede no existir
        auto* const tSecondaryDuck = intel->GetTaskDuck(true);

        const auto SetScriptControlledDuckTask = [intel] {
            intel->GetTaskManager().SetTaskSecondary(new CTaskSimpleDuck{ DUCK_SCRIPT_CONTROLLED, 0 }, TASK_SECONDARY_DUCK);
        };

        if (!tSecondaryDuck) { // 0x62F791
            if (tSimplestDuck) { // 0x62F79C
                tSimplestDuck->MakeAbortable(ped, ABORT_PRIORITY_URGENT);
            }
            SetScriptControlledDuckTask();
            if (auto* const tUseGun = intel->GetTaskUseGun()) {
                tUseGun->ClearAnim(ped);
            }
        } else if (tSecondaryDuck->GetControlType() != DUCK_SCRIPT_CONTROLLED) { // 0x62F821
            if (tSimplestDuck) { // 0x62F82F
                tSimplestDuck->AbortBecauseOfOtherDuck(ped);
            }
            if (!tSecondaryDuck->SetControlType(DUCK_SCRIPT_CONTROLLED)) { // 0x62F84A
                tSecondaryDuck->AbortBecauseOfOtherDuck(ped);
                SetScriptControlledDuckTask();
            }
        }
    }

    return true;
}
