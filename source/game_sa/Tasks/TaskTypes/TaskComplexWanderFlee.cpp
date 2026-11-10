/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderFlee
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
#include "TaskComplexWanderFlee.h"

namespace {
// 0x65B370 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexWanderFlee del EXE (con hook propio (Destructor); vtable 0x86F690). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x65B370` del EXE.
__declspec(noinline) void __fastcall CTaskComplexWanderFleeDestructorBody(CTaskComplexWanderFlee* self, void* /*edx*/) { self->CTaskComplexWanderFlee::~CTaskComplexWanderFlee(); }

// 0x65D0C0 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexWanderFlee del EXE (ranura 0 de su vtable 0x86F690; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x65B370; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexWanderFlee* __fastcall CTaskComplexWanderFleeScalarDeletingDestructor(CTaskComplexWanderFlee* self, void* /*edx*/, uint8 flags) {
    CTaskComplexWanderFleeDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexWanderFlee() { return reinterpret_cast<uintptr_t>(&CTaskComplexWanderFleeDestructorBody); } // para el banco Fn145A

void CTaskComplexWanderFlee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderFlee, 0x86f690, 15);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexWanderFleeScalarDeletingDestructor, "ScalarDeletingDestructor", 0x65D0C0); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x65B320);
    RH_ScopedInstall(Destructor, 0x65B370);

    RH_ScopedVMTInstall(Clone, 0x65CD70);
    RH_ScopedVMTInstall(ControlSubTask, 0x65B380);
    RH_ScopedVMTInstall(GetWanderType, 0x65B350);
    RH_ScopedVMTInstall(ScanForStuff, 0x65B360);
}

// 0x65B320
CTaskComplexWanderFlee::CTaskComplexWanderFlee(eMoveState moveState, uint8 dir) :
    CTaskComplexWander{moveState, dir, false}
{
}

CTaskComplexWanderFlee::CTaskComplexWanderFlee(const CTaskComplexWanderFlee& o) :
    CTaskComplexWanderFlee{o.m_nMoveState, o.m_nDir}
{
}
