#include "StdInc.h"

#include "TaskSimple.h"

// Funciones de CTaskSimple del EXE (vtable 0x86D4A8, 9 ranuras: [2] 0x43E300, [3] 0x43E310, [8] 0x43E320). Las comparten, por la misma direccion, las vtables de todas las tareas simples que no las redefinen
// (p. ej. CTaskSimpleUninterruptable 0x859580, CTaskSimpleGoToPointFine 0x86F964, CTaskSimpleCarForcePedOut 0x86EE94).
void CTaskSimple::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimple, 0x86D4A8, 9);
    RH_ScopedCategory("Tasks");

    RH_ScopedVMTInstall(GetSubTask, 0x43E300);     // lote 158A (banco Fn158A): ranura 2, retorno de constante
    RH_ScopedVMTInstall(IsSimple, 0x43E310);       // lote 158A: ranura 3
    RH_ScopedVMTInstall(SetPedPosition, 0x43E320); // lote 158A: ranura 8
}

// 0x43E300 (lote 158A, banco Fn158A; thiscall, `ret`, 3 bytes: `xor eax,eax`): sin subtarea (nulo); no lee el objeto.
CTask* CTaskSimple::GetSubTask() const {
    return nullptr;
}

// 0x43E310 (lote 158A, banco Fn158A; thiscall, `ret`, 3 bytes: `mov al,1`): siempre verdadero; no lee el objeto (solo escribe AL).
bool CTaskSimple::IsSimple() const {
    return true;
}

// 0x43E320 (lote 158A, banco Fn158A; thiscall, un argumento de pila, `ret 4`, 5 bytes: `xor al,al`): siempre falso; no lee ni el objeto ni el ped.
bool CTaskSimple::SetPedPosition(CPed* /*ped*/) {
    return false;
}
