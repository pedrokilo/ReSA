/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleUseAtm
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
#include "TaskSimpleUseAtm.h"


namespace {
// 0x48E960 (lote 147A, banco Fn147A): cuerpo del destructor de CTaskSimpleUseAtm del EXE (con hook propio (Destructor); vtable 0x85A134). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor
// con borrado como el `call 0x48E960` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleUseAtmDestructorBody(CTaskSimpleUseAtm* self, void* /*edx*/) { self->CTaskSimpleUseAtm::~CTaskSimpleUseAtm(); }

// 0x48E940 (lote 147A, banco Fn147A): destructor con borrado de CTaskSimpleUseAtm del EXE (ranura 0 de su vtable 0x85A134; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48E960; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleUseAtm* __fastcall CTaskSimpleUseAtmScalarDeletingDestructor(CTaskSimpleUseAtm* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleUseAtmDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote147ABody_CTaskSimpleUseAtm() { return reinterpret_cast<uintptr_t>(&CTaskSimpleUseAtmDestructorBody); } // para el banco Fn147A

void CTaskSimpleUseAtm::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleUseAtm, 0x85a134, 10);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleUseAtmScalarDeletingDestructor, "ScalarDeletingDestructor", 0x48E940); // lote 147A (banco Fn147A)

    RH_ScopedInstall(Constructor, 0x48DFE0);
    RH_ScopedInstall(Destructor, 0x48E960);

    RH_ScopedVMTInstall(Clone, 0x48E010);
    RH_ScopedVMTInstall(IsInterruptable, 0x48E080);
}

// 0x48DFE0
CTaskSimpleUseAtm::CTaskSimpleUseAtm() :
    CTaskSimpleRunAnim { ANIM_GROUP_DEFAULT, ANIM_ID_ATM, 4.f, Type, "UseAtm", false }
{
}

// NOTSA
CTaskSimpleUseAtm::CTaskSimpleUseAtm(const CTaskSimpleUseAtm&) :
    CTaskSimpleUseAtm{}
{
}
