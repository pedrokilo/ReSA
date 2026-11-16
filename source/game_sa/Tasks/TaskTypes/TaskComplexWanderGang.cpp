/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderGang
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWanderGang.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
// 0x66F670 (lote 205A, sin validar; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov dword ptr [ecx], 0x86FE84; jmp 0x61A3D0`): cuerpo no virtual de ~CTaskComplexWanderGang del EXE. Como el original: deja la vtable de CTaskComplexWander del
// EXE (0x86FE84; NO escribe la de su propia clase, el compilador elimino esa escritura) y sigue con el cuerpo de CTaskComplex (0x61A3D0, con hook; salto final). Sin marco SEH (no hay miembros que destruir). Lo llama el destructor con borrado (0x66F650).
__declspec(noinline) void __fastcall WanderGangDestructorBodyExe(CTaskComplexWanderGang* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86FE84;
    CTaskComplexBody::Destructor(self, nullptr);
}
} // namespace

void CTaskComplexWanderGang::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderGang, 0x8700D4, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66F5C0);
    RH_ScopedNamedGlobalInstall(WanderGangDestructorBodyExe, "DestructorBody", 0x66F670); // lote 205A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x66F650);                                  // lote 205A (sin validar)
    RH_ScopedInstall(CanJoinGang, 0x669FB0); // lote 194A (sin validar)

    RH_ScopedVMTInstall(Clone, 0x671470);
    RH_ScopedVMTInstall(GetWanderType, 0x66F630);
    RH_ScopedVMTInstall(ScanForStuff, 0x66F640);
}

// 0x66F5C0
CTaskComplexWanderGang::CTaskComplexWanderGang(eMoveState moveState, uint8 dir, uint32 scanTime, bool bWanderSensibly, float fTargetRadius)
    : CTaskComplexWander(moveState, dir, bWanderSensibly, fTargetRadius)
{
    m_NextScanTime = scanTime;
    m_TaskTimer.m_nInterval = scanTime;
    m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
    m_TaskTimer.m_bStarted = true;
}

// 0x66F640
void CTaskComplexWanderGang::ScanForStuff(CPed* ped) {
    // NOP
}

// 0x66F650 (lote 205A, sin validar; destructor con borrado de CTaskComplexWanderGang del EXE, ranura 0 de su vtable 0x8700D4; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x66F670; test byte [esp+8],1; je ..;
// push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x66F670), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE
// de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexWanderGang* CTaskComplexWanderGang::ScalarDeletingDestructor(uint8 flags) {
    WanderGangDestructorBodyExe(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x669FB0 (lote 194A, sin validar; thiscall, sin argumentos, `ret`, 54 bytes; el listado decia CTaskComplexWander: la clase real es CTaskComplexWanderGang, con el temporizador en +0x28; lo llama el
// escaneo de pandilleros del EXE en 0x65E4C8 tras comprobar GetWanderType == WANDER_TYPE_GANG). Es CTaskTimer::IsOutOfTime (0x420E30) expandido EN LINEA, sin llamar a esa funcion: temporizador sin arrancar
// (+0x30) -> false; si esta parado (+0x31) lo rearranca (+0x28 = hora actual, +0x31 = false); devuelve hora actual >= inicio + intervalo (+0x2C), comparacion SIN signo (cmp + sbb + inc).
bool CTaskComplexWanderGang::CanJoinGang() {
    if (!m_TaskTimer.m_bStarted) {
        return false;
    }
    if (m_TaskTimer.m_bStopped) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_bStopped = false;
    }
    const uint32 end = uint32(m_TaskTimer.m_nStartTime) + uint32(m_TaskTimer.m_nInterval);
    return CTimer::GetTimeInMS() >= end;
}
