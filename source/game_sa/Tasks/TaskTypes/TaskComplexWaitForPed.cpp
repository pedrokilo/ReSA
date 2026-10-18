/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWaitForPed
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWaitForPed.h"
#include "X87Math.h"

void CTaskComplexWaitForPed::InjectHooks() {
    RH_ScopedClass(CTaskComplexWaitForPed);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x683340);
}

namespace {
// Marco de excepciones del original (EXCEPTION_REGISTRATION_RECORD + this): su manejador (0x846798, tabla FuncInfo 0x89F1DC, estado 0)
// solo desenrolla, y la accion del estado 0 (0x846790) es el destructor de CTaskComplex (0x61A3D0) sobre this. Aqui el manejador hace lo mismo
// y no atiende la excepcion. Solo puede dispararse si CEntity::RegisterReference falla (ped nulo en el EXE tambien cae ahi).
struct WaitForPedFrame {
    WaitForPedFrame* next;
    void*            handler;
    void*            self;
};

EXCEPTION_DISPOSITION __cdecl WaitForPedUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) {
    if (record->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)) {
        reinterpret_cast<void(__thiscall*)(void*)>(0x61A3D0)(static_cast<WaitForPedFrame*>(frame)->self);
    }
    return ExceptionContinueSearch;
}
} // namespace

// 0x683340: como el original (ret 10h). Registra el marco de excepciones en fs:[0], deja la vtable del EXE (0x8706F8) y el resto de campos
// en el mismo valor que el EXE (padding 0x19..0x1B y 0x28 intactos; 0x61A3B0 se integra: padre y subtarea a 0) y registra la referencia
// del ped (0x571B70) como ultima operacion, sin guardia de nulo. Al salir el EXE deja EAX = this, ECX = cabeza anterior de la cadena de
// excepciones y EDX = el residuo de 0x571B70 (el banco los compara); CEntity::RegisterReferenceWithResidue reproduce el EDX con que sale 0x571B70 (todas sus
// salidas, incluida la poda de las tres pools) y ResidueRegsCpp fija ECX, EDX y EAX. Sin comprobaciones de ejecucion para no
// alterar ECX/EDX tras la ultima llamada y sin __try/__finally del compilador (su epilogo cambia ECX).
#pragma warning(push)
#pragma warning(disable: 4733) // el marco se registra a mano en fs:[0], como el original (manejador 0x846798)
#pragma runtime_checks("", off)
CTaskComplexWaitForPed* CTaskComplexWaitForPed::Constructor(CPed* ped, float radius, uint32 time, bool rotate) {
    const uint32 previousHead = __readfsdword(0);
    WaitForPedFrame frame{ reinterpret_cast<WaitForPedFrame*>(static_cast<uintptr_t>(previousHead)), reinterpret_cast<void*>(&WaitForPedUnwind), this };
    __writefsdword(0, static_cast<uint32>(reinterpret_cast<uintptr_t>(&frame)));

    m_Parent = nullptr;
    m_pSubTask = nullptr;
    *reinterpret_cast<uint32*>(this) = 0x8706F8;
    m_timer.m_nStartTime = 0;
    m_timer.m_nInterval = 0;
    m_timer.m_bStarted = false;
    m_timer.m_bStopped = false;
    m_radius = radius;
    m_timeInMs = time;
    m_bRotateOtherPedsToWaitingPed = rotate;
    m_ped = ped;

    // EDX al entrar en 0x571B70: el EXE deja en EDX el tiempo con su byte bajo sustituido por el booleano; CEntity::RegisterReferenceWithResidue
    // devuelve el que deja 0x571B70 (ver Entity.cpp).
    const uint32 entryEdx = (time & 0xFFFFFF00u) | static_cast<uint8>(rotate);
    const uint32 residueEdx = ped->RegisterReferenceWithResidue(reinterpret_cast<CEntity**>(&m_ped), entryEdx);

    __writefsdword(0, previousHead);
    return reinterpret_cast<CTaskComplexWaitForPed*>(static_cast<uintptr_t>(
        ResidueRegsCpp(previousHead, residueEdx, static_cast<uint32>(reinterpret_cast<uintptr_t>(this)))));
}
#pragma runtime_checks("", restore)
#pragma warning(pop)
