/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleChat
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
#include "TaskSimpleChat.h"


void CTaskSimpleChat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleChat, 0x86C78C, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Destructor, 0x5F7FD0);
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

// 0x5F65E0
CTaskSimpleChat::CTaskSimpleChat(const CTaskSimpleChat& o) :
    CTaskSimpleChat{ o.m_durationMs }
{
}

// 0x5F7FD0
CTaskSimpleChat::~CTaskSimpleChat() {
    // Como el original: 0x5F7FD0 es solo `jmp 0x61A6F0` (destructor de la clase base)
}
