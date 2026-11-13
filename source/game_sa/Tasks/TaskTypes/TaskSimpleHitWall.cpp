/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHitWall
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
#include "TaskSimpleHitWall.h"

namespace {
// 0x655DE0 (lote 180A, sin validar; el listado decia CTaskComplexWalkRoundFire): destructor con borrado de CTaskSimpleHitWall del EXE (ranura 0 de su vtable 0x86F1A0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x655E00; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). 0x655E00 es solo `jmp 0x61A6F0` (cuerpo de ~CTaskSimpleAnim, con hook, que deja la vtable 0x86D504 y
// termina en CTask::Destructor): la clase no escribe vtable propia. Como el original, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleHitWall* __fastcall HitWallScalarDeletingDestructor(CTaskSimpleHitWall* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleHitWall::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitWall, 0x86F1A0, 10);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(HitWallScalarDeletingDestructor, "ScalarDeletingDestructor", 0x655DE0); // lote 180A (sin validar)

    RH_ScopedInstall(Constructor, 0x652F90);

    RH_ScopedVMTInstall(Clone, 0x652FC0);
    RH_ScopedVMTInstall(IsInterruptable, 0x653030);
}

// 0x652F90
CTaskSimpleHitWall::CTaskSimpleHitWall() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_WALL,
        4.0f,
        Type,
        "HitWall",
        false
    }
{
}

CTaskSimpleHitWall::CTaskSimpleHitWall(const CTaskSimpleHitWall&) :
    CTaskSimpleHitWall{}
{
}
