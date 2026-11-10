/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToPointShooting
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

#include "TaskComplexGoToPointShooting.h"

namespace {
// 0x668870 (cuerpo de ~CTaskComplexGoToPointAiming, con hook propio): llamada no virtual al destructor de la base, en una funcion propia sin expansion en linea para que el banco Fn147A pueda sustituirla.
__declspec(noinline) void __fastcall GoToPointAimingDestructorCall(CTaskComplexGoToPointShooting* self, void* /*edx*/) { self->CTaskComplexGoToPointAiming::~CTaskComplexGoToPointAiming(); }

// 0x66DFA0 (lote 147A, banco Fn147A; thiscall, sin argumentos de pila, 5 bytes: `jmp 0x668870`): cuerpo no virtual de ~CTaskComplexGoToPointShooting del EXE. La clase no tiene destructor propio: el del EXE
// es un salto directo al de su base CTaskComplexGoToPointAiming (0x668870, que deja su propia vtable 0x86FE00 y sigue con ~CTaskComplex); no escribe la vtable de CTaskComplexGoToPointShooting.
__declspec(noinline) void __fastcall GoToPointShootingDestructorBody(CTaskComplexGoToPointShooting* self, void* /*edx*/) { GoToPointAimingDestructorCall(self, nullptr); }

// 0x66DF80 (lote 147A, banco Fn147A): destructor con borrado de CTaskComplexGoToPointShooting del EXE (ranura 0 de su vtable 0x86FE2C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x66DFA0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x66DFA0),
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexGoToPointShooting* __fastcall GoToPointShootingScalarDeletingDestructor(CTaskComplexGoToPointShooting* self, void* /*edx*/, uint8 flags) {
    GoToPointShootingDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote147AAimingDestructorCall() { return reinterpret_cast<uintptr_t>(&GoToPointAimingDestructorCall); } // para el banco Fn147A

void CTaskComplexGoToPointShooting::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointShooting, 0x86fe2c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(GoToPointShootingDestructorBody, "Destructor", 0x66DFA0);                         // lote 147A (banco Fn147A)
    RH_ScopedNamedGlobalInstall(GoToPointShootingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x66DF80); // lote 147A (banco Fn147A)

    RH_ScopedInstall(Constructor, 0x668C70);

    RH_ScopedVMTInstall(Clone, 0x66CE10);
    RH_ScopedVMTInstall(GetTaskType, 0x668CC0);
}

// 0x668C70
CTaskComplexGoToPointShooting::CTaskComplexGoToPointShooting(
    eMoveState moveState,
    const CVector& movePos,
    CEntity* aimAtEntity,
    CVector aimPos,
    const float targetRadius,
    const float slowDownDist
) :
    CTaskComplexGoToPointAiming{
        moveState,
        movePos,
        aimAtEntity,
        aimPos,
        targetRadius,
        slowDownDist
    }
{
    // NOP
}

CTaskComplexGoToPointShooting::CTaskComplexGoToPointShooting(const CTaskComplexGoToPointShooting& o) :
    CTaskComplexGoToPointAiming{o}
{
}

// 0x66CE10: the original calls the base constructor, then installs Shooting's vtable.
CTask* CTaskComplexGoToPointShooting::Clone() const {
    return new CTaskComplexGoToPointShooting{*this};
}

// 0x668CC0
eTaskType CTaskComplexGoToPointShooting::GetTaskType() const {
    return Type;
}
