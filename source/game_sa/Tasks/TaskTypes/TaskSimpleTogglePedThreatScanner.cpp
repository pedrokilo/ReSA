/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleTogglePedThreatScanner
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleTogglePedThreatScanner.h"

namespace {
// 0x492D40 (lote 142A, banco Fn142A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CTaskSimpleTogglePedThreatScanner del EXE (`mov [ecx],0x85A334; jmp 0x61A3A0`). Deja la vtable
// de la clase del EXE (0x85A334) y sigue con el cuerpo de ~CTask (0x61A3A0, con hook). Sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall TogglePedThreatScannerDestructorBody(CTaskSimpleTogglePedThreatScanner* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85A334;
    self->CTask::Destructor();
}
} // namespace

void CTaskSimpleTogglePedThreatScanner::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTogglePedThreatScanner, 0x85A334, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(TogglePedThreatScannerDestructorBody, "Destructor", 0x492D40);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x492DF0);

    RH_ScopedInstall(Constructor, 0x492D10);

    RH_ScopedVMTInstall(Clone, 0x492D50);
    RH_ScopedVMTInstall(GetTaskType, 0x492DD0);
    RH_ScopedVMTInstall(MakeAbortable, 0x492DE0);
    RH_ScopedVMTInstall(ProcessPed, 0x6337A0);
}

// 0x492D10
CTaskSimpleTogglePedThreatScanner::CTaskSimpleTogglePedThreatScanner(bool bScanAllowedScriptPed, bool bScanAllowedInVehicle, bool bScanAllowedScriptedTask)
    : CTaskSimple()
{
    m_bScanAllowedScriptPed = bScanAllowedScriptPed;
    m_bScanAllowedInVehicle = bScanAllowedInVehicle;
    m_bScanAllowedScriptedTask = bScanAllowedScriptedTask;
}

// 0x492DF0 (lote 142A, banco Fn142A) : destructor con borrado de CTaskSimpleTogglePedThreatScanner del EXE (ranura 0 de su vtable 0x85A334; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x492D40; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x492D40), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleTogglePedThreatScanner* CTaskSimpleTogglePedThreatScanner::ScalarDeletingDestructor(uint8 flags) {
    TogglePedThreatScannerDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x6337A0
bool CTaskSimpleTogglePedThreatScanner::ProcessPed(CPed* ped) {
    auto& scanner = ped->GetIntelligence()->GetEventScanner().m_pedAcquaintanceScanner;
    scanner.m_bScanAllowedInVehicle    = m_bScanAllowedInVehicle;
    scanner.m_bScanAllowedScriptPed    = m_bScanAllowedScriptPed;
    scanner.m_bScanAllowedScriptedTask = m_bScanAllowedScriptedTask;
    return true;
}
