/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleChat
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
#include "TaskSimpleChat.h"


void CTaskSimpleChat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleChat, 0x86C78C, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedInstall(ConstructorExe, "Constructor", 0x5F65A0); // lote 151A (banco Fn151A)
    RH_ScopedInstall(Destructor, 0x5F7FD0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x5F7FB0); // lote 148A (banco Fn148A)
    RH_ScopedVMTInstall(Clone, 0x5F65E0);
    RH_ScopedVMTInstall(IsInterruptable, 0x5F6650);
}

// 0x5F65A0
CTaskSimpleChat::CTaskSimpleChat(uint32 duration) :
    CTaskSimpleRunTimedAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_IDLE_CHAT,
        4.f,
        -4.f,
        duration,
        TASK_SIMPLE_CHAT,
        "Chat",
        0
    }
{
}

// 0x5F65A0 (lote 151A, banco Fn151A; el listado decia CPedGroupDefaultTaskAllocatorSitInLeaderCar): constructor de CTaskSimpleChat del EXE (thiscall, un dword de pila, `ret 4`, 51 bytes: llama al constructor de
// CTaskSimpleRunTimedAnim 0x61ABE0 (con hook; ECX = this) con (grupo 0, anim 0xC, mezcla 4.0 = 0x40800000, -4.0 = 0xC0800000, duracion, tipo 0x1A0, nombre 0x86C7B4 ("Chat"), 0), deja la vtable de la clase del EXE
// (0x86C78C; la de la DLL no vale para los llamadores del EXE: 6 constructores/clones) y devuelve this). Los seis llamadores no leen ECX ni EDX tras la llamada, solo EAX = this. El constructor C++ de arriba
// sigue siendo el de los objetos creados desde la DLL.
static_assert(TASK_SIMPLE_CHAT == 0x1A0);
static_assert(ANIM_GROUP_DEFAULT == 0);
static_assert(ANIM_ID_IDLE_CHAT == 0xC);
CTaskSimpleChat* __fastcall CTaskSimpleChat::ConstructorExe(CTaskSimpleChat* self, void* /*edx*/, uint32 duration) {
    static_cast<CTaskSimpleRunTimedAnim*>(self)->Constructor(ANIM_GROUP_DEFAULT, ANIM_ID_IDLE_CHAT, 4.f, -4.f, duration, TASK_SIMPLE_CHAT, "Chat", false);
    *reinterpret_cast<uint32*>(self) = 0x86C78C;
    return self;
}

// 0x5F65E0
CTaskSimpleChat::CTaskSimpleChat(const CTaskSimpleChat& o) :
    CTaskSimpleChat{ o.m_durationMs }
{
}

// 0x5F7FB0 (lote 148A, banco Fn148A) : destructor con borrado de CTaskSimpleChat del EXE (ranura 0 de su vtable 0x86C78C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x5F7FD0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`; el listado lo atribuia a CPedGroups). Como el original: destruye con el cuerpo
// de la clase (0x5F7FD0; aqui su envoltura Destructor(), con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleChat* CTaskSimpleChat::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x5F7FD0
CTaskSimpleChat::~CTaskSimpleChat() {
    // Como el original: 0x5F7FD0 es solo `jmp 0x61A6F0` (destructor de la clase base)
}
