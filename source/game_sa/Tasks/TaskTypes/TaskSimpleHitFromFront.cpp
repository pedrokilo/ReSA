/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHitFromFront
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleHitFromFront.h"

namespace {
// 0x637300 (lote 183A, sin validar; el listado decia CTaskSimpleHitFromFront): destructor con borrado de CTaskSimpleHitFromFront del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x637320;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). 0x637320 es solo `jmp 0x61A6F0` (cuerpo de ~CTaskSimpleAnim, con hook, que deja la vtable 0x86D504 y termina en CTask::Destructor):
// la clase no escribe vtable propia. Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S.
CTaskSimpleHitFromFront* __fastcall HitFromFrontScalarDeletingDestructor(CTaskSimpleHitFromFront* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleHitFromFront::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromFront, 0x86e5ec, 10);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(HitFromFrontScalarDeletingDestructor, "ScalarDeletingDestructor", 0x637300); // lote 183A (sin validar)

    RH_ScopedInstall(Constructor, 0x637250);

    RH_ScopedVMTInstall(Clone, 0x637290);
    RH_ScopedVMTInstall(IsInterruptable, 0x637280);
}

// 0x637250
CTaskSimpleHitFromFront::CTaskSimpleHitFromFront() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_FRONT,
        4.0f,
        Type,
        "HitFromFront",
        false
    }
{
}

CTaskSimpleHitFromFront::CTaskSimpleHitFromFront(const CTaskSimpleHitFromFront& o) :
    CTaskSimpleHitFromFront{}
{
}
