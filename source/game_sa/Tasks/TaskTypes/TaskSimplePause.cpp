/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePause
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

#include "TaskSimplePause.h"

namespace {
// 0x48E8A0 (lote 149A, banco Fn149A; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x85A278; jmp 0x61A3A0`): cuerpo no virtual de ~CTaskSimplePause del EXE. Deja la vtable de la clase del EXE
// (0x85A278) y salta a CTask::Destructor (0x61A3A0, con hook), que deja la de CTask. Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x48E8A0` del EXE.
__declspec(noinline) void __fastcall PauseDestructorBody(CTaskSimplePause* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85A278;
    self->CTask::Destructor();
}

// 0x48E880 (lote 149A, banco Fn149A): destructor con borrado de CTaskSimplePause del EXE (ranura 0 de su vtable 0x85A278; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48E8A0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimplePause* __fastcall PauseScalarDeletingDestructor(CTaskSimplePause* self, void* /*edx*/, uint8 flags) {
    PauseDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimplePause::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePause, 0x85A278, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(PauseDestructorBody, "Destructor", 0x48E8A0);                         // lote 149A (banco Fn149A)
    RH_ScopedNamedGlobalInstall(PauseScalarDeletingDestructor, "ScalarDeletingDestructor", 0x48E880); // lote 149A (banco Fn149A)
    RH_ScopedVMTInstall(GetTaskType, 0x48E800);                                                       // lote 149A (banco Fn149A): retorno de constante (TASK_SIMPLE_PAUSE = 202)

    RH_ScopedInstall(Constructor, 0x48E750);
    RH_ScopedVMTInstall(ProcessPed, 0x48E830);
    RH_ScopedVMTInstall(Clone, 0x48E780);
    RH_ScopedVMTInstall(MakeAbortable, 0x48E810);
}

// 0x48E750
CTaskSimplePause::CTaskSimplePause(int32 pauseIntervalMs)  :
    m_PauseIntervalMs{pauseIntervalMs}
{
}

// For 0x48E780
CTaskSimplePause::CTaskSimplePause(const CTaskSimplePause& o) :
    CTaskSimplePause{o.m_PauseIntervalMs}
{
}

CTaskSimplePause* CTaskSimplePause::Constructor(int32 time) {
    this->CTaskSimplePause::CTaskSimplePause(time);
    return this;
}

// 0x48E800 (lote 149A, banco Fn149A; thiscall, `ret`, 6 bytes: `mov eax,0CAh`): TASK_SIMPLE_PAUSE (202); no lee el objeto.
eTaskType CTaskSimplePause::GetTaskType() const {
    return TASK_SIMPLE_PAUSE;
}

// 0x48E810
bool CTaskSimplePause::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    m_Timer.SetAsOutOfTime();
    return true;
}

// 0x48E830
bool CTaskSimplePause::ProcessPed(CPed* ped) {
    if (!m_Timer.m_bStarted) {
        // Como el original: arranca el temporizador sin comprobar que el intervalo sea >= 0
        m_Timer.m_nStartTime = CTimer::GetTimeInMS();
        m_Timer.m_nInterval  = m_PauseIntervalMs;
        m_Timer.m_bStarted   = true;
        if (!m_Timer.m_bStarted) {
            return false;
        }
    }
    m_Timer.Reset();
    return m_Timer.IsOutOfTime();
}
