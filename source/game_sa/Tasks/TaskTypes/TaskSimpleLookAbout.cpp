/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleLookAbout
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
#include "TaskSimpleLookAbout.h"

namespace {
// 0x48EA90 (lote 151A, banco Fn151A; el listado decia CTaskSimpleHandsUp): cuerpo del destructor de CTaskSimpleLookAbout del EXE (thiscall, sin argumentos de pila; es `jmp 0x61A6F0`, el cuerpo de ~CTaskSimpleAnim, sin hook;
// no escribe ninguna vtable). Como el original, sigue con el cuerpo de ~CTaskSimpleAnim. Lo llama el destructor con borrado (0x48EA70).
void __fastcall LookAboutDestructorBody(CTaskSimpleLookAbout* self, void* /*edx*/) {
    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);
}

// 0x48EA70 (lote 151A, banco Fn151A; el listado decia CTaskSimpleHandsUp): destructor con borrado de CTaskSimpleLookAbout del EXE (ranura 0 de su vtable 0x85A164; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48EA90; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x48EA90, con hook),
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskSimpleLookAbout* __fastcall LookAboutScalarDeletingDestructor(CTaskSimpleLookAbout* self, void* /*edx*/, uint8 flags) {
    LookAboutDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleLookAbout::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleLookAbout, 0x0085A164, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E0A0);
    RH_ScopedNamedGlobalInstall(LookAboutDestructorBody, "Destructor", 0x48EA90); // lote 151A (banco Fn151A)
    RH_ScopedNamedGlobalInstall(LookAboutScalarDeletingDestructor, "ScalarDeletingDestructor", 0x48EA70); // lote 151A (banco Fn151A)

    RH_ScopedVMTInstall(Clone, 0x48E0F0);
    RH_ScopedVMTInstall(IsInterruptable, 0x48E160);
}

// 0x48E0A0
CTaskSimpleLookAbout::CTaskSimpleLookAbout(uint32 durationMs) :
    CTaskSimpleRunTimedAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_IDLE_HBHB_1,
        4.0f,
        -4.0f,
        durationMs,
        Type,
        "LookAbout",
        false
    }
{
}

CTaskSimpleLookAbout::CTaskSimpleLookAbout(const CTaskSimpleLookAbout& o) :
    CTaskSimpleLookAbout{o.m_durationMs}
{
}
