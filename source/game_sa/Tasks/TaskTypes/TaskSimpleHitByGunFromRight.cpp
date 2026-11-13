/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHitByGunFromRight
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleHitByGunFromRight.h"

namespace {
// 0x637450 (lote 183A, sin validar; el listado decia CTaskSimpleHitFromFront): destructor con borrado de CTaskSimpleHitByGunFromRight del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x637470;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). 0x637470 es solo `jmp 0x61A6F0` (cuerpo de ~CTaskSimpleAnim, con hook, que deja la vtable 0x86D504 y termina en CTask::Destructor):
// la clase no escribe vtable propia. Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S.
CTaskSimpleHitByGunFromRight* __fastcall HitByGunFromRightScalarDeletingDestructor(CTaskSimpleHitByGunFromRight* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleHitByGunFromRight::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitByGunFromRight, 0x86dcc4, 10);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(HitByGunFromRightScalarDeletingDestructor, "ScalarDeletingDestructor", 0x637450); // lote 183A (sin validar)

    RH_ScopedInstall(Constructor, 0x62F080);

    RH_ScopedVMTInstall(Clone, 0x62F0B0);
    RH_ScopedVMTInstall(IsInterruptable, 0x62F120);
}

// 0x62F080
CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight() :
    CTaskSimpleRunAnim::CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_SHOT_RIGHTP,
        4.0f,
        Type,
        "HitByGunFromRight",
        false
    }
{
}

CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight(const CTaskSimpleHitByGunFromRight&) :
    CTaskSimpleHitByGunFromRight{}
{
}
