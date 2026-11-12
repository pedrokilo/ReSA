/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitForPizza
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

#include "TaskSimpleWaitForPizza.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
constexpr uint32 kWaitForPizzaVTable = 0x86E1DC;

// 0x632B30 (lote 171A, banco Fn171A; thiscall, sin argumentos de pila, `ret`, 32 bytes): cuerpo del constructor de CTaskSimpleWaitForPizza del EXE. Base CTaskSimple (0x61A390, con hook), vtable de la clase del
// EXE (0x86E1DC), temporizador a cero (inicio +8 e intervalo +0Ch como dwords, iniciado +10h y detenido +11h como bytes) y devuelve `this`. Funcion propia sin expansion en linea: la llama tambien Clone.
__declspec(noinline) CTaskSimpleWaitForPizza* __fastcall WaitForPizzaConstructorBody(CTaskSimpleWaitForPizza* self, void* /*edx*/) {
    CTaskSimpleBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = kWaitForPizzaVTable;
    *reinterpret_cast<uint32*>(bytes + 8) = 0;
    *reinterpret_cast<uint32*>(bytes + 0xC) = 0;
    bytes[0x10] = 0;
    bytes[0x11] = 0;
    return self;
}

// 0x632B70 (lote 171A, banco Fn171A; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x86E1DC; jmp 0x61A3A0`): cuerpo no virtual de ~CTaskSimpleWaitForPizza del EXE. Deja
// la vtable de la clase del EXE (0x86E1DC) y salta a CTask::Destructor (0x61A3A0, con hook). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado.
__declspec(noinline) void __fastcall WaitForPizzaDestructorBody(CTaskSimpleWaitForPizza* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = kWaitForPizzaVTable;
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
    RH_ScopedVirtualClass(CTaskSimpleWaitForPizza, kWaitForPizzaVTable, 9); // vtable 0x86E1DC (verificado en gta_sa.exe)
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(WaitForPizzaScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6389D0); // lote 149A (banco Fn149A)
    RH_ScopedNamedGlobalInstall(WaitForPizzaConstructorBody, "Constructor", 0x632B30);                          // lote 171A (banco Fn171A)
    RH_ScopedNamedGlobalInstall(WaitForPizzaDestructorBody, "DestructorBody", 0x632B70);                        // lote 171A

    RH_ScopedVMTInstall(Clone, 0x636B40);
    RH_ScopedVMTInstall(GetTaskType, 0x632B50);
    RH_ScopedVMTInstall(MakeAbortable, 0x632B60);
    RH_ScopedVMTInstall(ProcessPed, 0x632B80);
}

// 0x632B50 (lote 171A, banco Fn171A; thiscall, `ret`, 5 bytes: `mov eax,0EFh`): TASK_SIMPLE_WAIT_FOR_PIZZA (239); no lee el objeto (ranura 4 de la vtable 0x86E1DC).
eTaskType CTaskSimpleWaitForPizza::GetTaskType() const {
    static_assert(Type == 0xEF);
    return Type;
}

// 0x636B40 (lote 171A; 113 bytes con marco SEH que solo desenrolla): CTask::operator new(0x14) (0x61A5A0); si no es nulo, el cuerpo del constructor (el EXE lo trae en linea: base 0x61A390, vtable del EXE y
// temporizador a cero); devuelve la memoria o nulo si no hubo memoria.
CTask* CTaskSimpleWaitForPizza::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleWaitForPizza));
    if (!memory) {
        return nullptr;
    }
    return WaitForPizzaConstructorBody(static_cast<CTaskSimpleWaitForPizza*>(memory), nullptr);
}

// 0x632B60 (lote 171A, banco Fn171A; thiscall, tres dwords de pila, `ret 0Ch`, 5 bytes: `mov al,1`): siempre se puede abortar; solo AL.
bool CTaskSimpleWaitForPizza::MakeAbortable(CPed* /*ped*/, eAbortPriority /*priority*/, const CEvent* /*event*/) {
    return true;
}

// 0x632B80 (lote 171A, banco Fn171A; thiscall, un puntero a ped en la pila, `ret 4`, 72 bytes): si el temporizador no esta iniciado lo inicia con el reloj del juego (0xB7CB84) e intervalo 2000 ms; si esta
// detenido reinicia su inicio con el reloj y lo marca como no detenido; devuelve true cuando el reloj alcanza inicio + intervalo (comparacion sin signo, aritmetica de 32 bits).
bool CTaskSimpleWaitForPizza::ProcessPed(CPed* /*ped*/) {
    if (!m_timer.m_bStarted) {
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval = 2000;
        m_timer.m_bStarted = true;
    }
    if (m_timer.m_bStopped) {
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_bStopped = false;
    }
    return CTimer::GetTimeInMS() >= m_timer.m_nStartTime + static_cast<uint32>(m_timer.m_nInterval);
}
