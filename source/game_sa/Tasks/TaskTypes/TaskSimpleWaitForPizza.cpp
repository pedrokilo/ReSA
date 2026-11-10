#include "StdInc.h"

#include "TaskSimpleWaitForPizza.h"

namespace {
// 0x632B70 (sin hook propio; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x86E1DC; jmp 0x61A3A0`): cuerpo no virtual de ~CTaskSimpleWaitForPizza del EXE (la clase aun no esta reconstruida). Deja
// la vtable de la clase del EXE (0x86E1DC) y salta a CTask::Destructor (0x61A3A0, con hook). Funcion propia sin expansion en linea para que el banco Fn149A pueda sustituirla; la llama el destructor con borrado.
__declspec(noinline) void __fastcall WaitForPizzaDestructorBody(CTaskSimpleWaitForPizza* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86E1DC;
    self->CTask::Destructor();
}

// 0x6389D0 (lote 149A, banco Fn149A): destructor con borrado de CTaskSimpleWaitForPizza del EXE (ranura 0 de su vtable 0x86E1DC; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x632B70; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleWaitForPizza* __fastcall WaitForPizzaScalarDeletingDestructor(CTaskSimpleWaitForPizza* self, void* /*edx*/, uint8 flags) {
    WaitForPizzaDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote149ABody_CTaskSimpleWaitForPizza() { return reinterpret_cast<uintptr_t>(&WaitForPizzaDestructorBody); } // para el banco Fn149A

void CTaskSimpleWaitForPizza::InjectHooks() {
    RH_ScopedClass(CTaskSimpleWaitForPizza); // vtable 0x86E1DC (verificado en gta_sa.exe; GetTaskType en [4]). Clase abstracta aun sin constructor: su vtable no se exporta, asi que no se usa RH_ScopedVirtualClass (solo hay hooks S)
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(WaitForPizzaScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6389D0); // lote 149A (banco Fn149A)

    // RH_ScopedVMTInstall(GetTaskType, 0x632B50);
    // TODO: instalar las funciones conforme se reconstruyan.
}
