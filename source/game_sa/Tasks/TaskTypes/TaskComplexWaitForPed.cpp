/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWaitForPed
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWaitForPed.h"
#include "TaskSimpleStandStill.h"
#include "EventDeadPed.h"
#include "X87Math.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
// 0x6833D0 (lote 151A, banco Fn151A; con hook desde el lote 193A, sin validar): cuerpo del destructor de CTaskComplexWaitForPed del EXE (la clase aun no declara su destructor; thiscall, sin argumentos de pila, `ret`, con marco SEH que solo desenrolla y aqui
// se omite). Como el original: vtable de la clase del EXE (0x8706F8); si el ped (+0xC) no es nulo, CEntity::CleanUpOldReference (0x571A00) con la direccion de +0xC (sin poner el puntero a cero) y ~CTaskComplex (0x61A3D0, con hook).
// Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x6833D0` del EXE.
__declspec(noinline) void __fastcall WaitForPedDestructorBody(CTaskComplexWaitForPed* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x8706F8;
    CEntity::SafeCleanUpRef(self->m_ped);
    CTaskComplexBody::Destructor(self, nullptr);
}

static_assert(TASK_COMPLEX_WAIT_FOR_PED == 0x4BD); // 0x6833C0: `mov eax, 0x4BD; ret`

// 0x6833C0 (lote 193A, sin validar; ranura 4 (GetTaskType) de la vtable 0x8706F8; thiscall, sin argumentos de pila, `ret`, 6 bytes: `mov eax,0x4BD; ret`): TASK_COMPLEX_WAIT_FOR_PED; no lee el objeto. La clase aun no declara sus
// virtuales: se engancha como S (sin vtable exportada para RH_ScopedVMTInstall).
eTaskType __fastcall WaitForPedGetTaskType(CTaskComplexWaitForPed* /*self*/, void* /*edx*/) {
    return TASK_COMPLEX_WAIT_FOR_PED;
}

// 0x683430 (lote 193A, sin validar; ranura 6 (MakeAbortable) de la vtable 0x8706F8; thiscall, tres argumentos de pila (ped, prioridad, evento), `ret 0xC`, 5 bytes: `mov al,1; ret 0xC`): siempre devuelve true; no lee el
// objeto ni los argumentos (el EXE solo escribe AL: los bytes altos de EAX quedan como estaban). Se engancha como S (clase sin virtuales declaradas).
bool __fastcall WaitForPedMakeAbortable(CTaskComplexWaitForPed* /*self*/, void* /*edx*/, CPed* /*ped*/, eAbortPriority /*priority*/, const CEvent* /*event*/) {
    return true;
}

// 0x684780 (lote 151A, banco Fn151A; el listado decia CTaskComplexPartnerShove): destructor con borrado de CTaskComplexWaitForPed del EXE (ranura 0 de su vtable 0x8706F8; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6833D0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexWaitForPed* __fastcall WaitForPedScalarDeletingDestructor(CTaskComplexWaitForPed* self, void* /*edx*/, uint8 flags) {
    WaitForPedDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote151ABody_CTaskComplexWaitForPed() { return reinterpret_cast<uintptr_t>(&WaitForPedDestructorBody); } // para el banco Fn151A

void CTaskComplexWaitForPed::InjectHooks() {
    RH_ScopedClass(CTaskComplexWaitForPed);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x683340);
    RH_ScopedNamedGlobalInstall(WaitForPedGetTaskType, "GetTaskType", 0x6833C0); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(WaitForPedDestructorBody, "DestructorBody", 0x6833D0); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(WaitForPedMakeAbortable, "MakeAbortable", 0x683430); // lote 193A (sin validar)
    RH_ScopedInstall(CloneBody, 0x683950); // lote 132A (banco Fn132A)
    RH_ScopedNamedGlobalInstall(WaitForPedScalarDeletingDestructor, "ScalarDeletingDestructor", 0x684780); // lote 151A (banco Fn151A)
    RH_ScopedNamedInstall(CreateNextSubTaskBody, "CreateNextSubTask", 0x683440);  // lote 194A (sin validar)
    RH_ScopedNamedInstall(CreateFirstSubTaskBody, "CreateFirstSubTask", 0x683450); // lote 194A (sin validar)
    RH_ScopedNamedInstall(ControlSubTaskBody, "ControlSubTask", 0x6834E0);        // lote 194A (sin validar)
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
// 0x683340
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

// 0x683950 (lote 132A, banco Fn132A): cuerpo del Clone del EXE (thiscall, sin argumentos, `ret`, marco SEH que solo libera la memoria si el constructor lanza: se omite). Reserva 0x2C bytes con
// CTask::operator new (0x61A5A0); si devuelve nulo, devuelve nulo. Si no, llama al constructor (0x683340) sobre la memoria con (ped +0xC, radio +0x10, tiempo +0x14, byte +0x18 sin signo) y devuelve lo
// que el constructor deja en EAX (el EXE no lo sustituye por la direccion reservada).
CTask* CTaskComplexWaitForPed::CloneBody() const {
    auto* const memory = static_cast<CTaskComplexWaitForPed*>(CTask::operator new(sizeof(CTaskComplexWaitForPed)));
    if (!memory) {
        return nullptr;
    }
    return memory->Constructor(m_ped, m_radius, m_timeInMs, m_bRotateOtherPedsToWaitingPed);
}

// 0x683440 (lote 194A, sin validar; `xor eax,eax; ret 4`, 5 bytes; ranura 8 de la vtable 0x8706F8): no lee el ped ni el objeto; siempre devuelve nulo.
CTask* CTaskComplexWaitForPed::CreateNextSubTaskBody(CPed* /*ped*/) {
    return nullptr;
}

// 0x683450 (lote 194A, sin validar; thiscall, un argumento de pila, `ret 4`, 138 bytes con marco SEH que solo libera la memoria si el constructor lanzara: se omite; ranura 9). Sin ped (+0xC) devuelve nulo sin
// tocar nada. Si no: arranca el temporizador (+0x1C = hora actual de CTimer, +0x20 = tiempo, +0x24 = true; +0x25 no se toca) y pone a cero el contador de fotogramas (+0x28). Despues reserva 0x20 bytes con
// CTask::operator new (0x61A5A0; sin memoria devuelve nulo) y construye CTaskSimpleStandStill con el constructor enganchado 0x62F310 (999999 = 0xF423F, false, false, 8.0f) y devuelve lo que este deja en EAX.
CTask* CTaskComplexWaitForPed::CreateFirstSubTaskBody(CPed* /*ped*/) {
    if (!m_ped) {
        return nullptr;
    }
    m_timer.m_nStartTime = CTimer::GetTimeInMS();
    m_timer.m_nInterval = m_timeInMs;
    m_timer.m_bStarted = true;
    m_framesToWaitForSettingRotation = 0;

    void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskSimpleStandStill*>(memory)->Constructor(999999, false, false, 8.0f);
}

// 0x6834E0 (lote 194A, sin validar; thiscall, ped en la pila, `ret 4`, 528 bytes con marco SEH que solo desenrolla el evento local; ranura 10). Como el original:
//  1. sin ped esperado (+0xC) devuelve nulo; con el ped esperado muerto (CPed::IsAlive 0x5E0170) construye un CEventDeadPed local (0x4ADEA0: ped esperado, false, hora actual), pone su tarea de respuesta (+0xE) a
//     TASK_COMPLEX_INVESTIGATE_DEAD_PED (0x258) y lo anade al grupo de eventos del ped recibido (CEventGroup::Add 0x4AB420, false); devuelve nulo (el evento se destruye con 0x4ADF70).
//  2. con el temporizador arrancado (+0x24) y vencido (CTaskTimer::IsOutOfTime 0x420E30) devuelve nulo.
//  3. busca en el ped esperado la tarea TASK_COMPLEX_WALK_ALONGSIDE_PED (0x4B8 = 1208; CPedIntelligence::FindTaskByType 0x600EE0). Con tarea: copia el CVector de la tarea (+0x14, tres dwords), lo pasa por la matriz
//     del ped recibido (0x59C890, sin comprobar nulo) y la diferencia es (resultado - posicion del ped esperado). Sin tarea: (posicion del ped recibido - posicion del ped esperado). Orden x, y, z.
//  4. distancia al cuadrado = (dz*dz + dy*dy) + dx*dx (x87 a 24 bits = floats); si es menor que radio*radio (+0x10) devuelve nulo (NaN o mayor/igual sigue).
//  5. con rotar (+0x18) y contador (+0x28) a cero: CVector (posicion del ped esperado - posicion del ped recibido) (0x40FE60) y m_fAimingRotation (+0x55C; no m_fCurrentRotation, que es +0x558) del ped recibido = LimitRadianAngle(
//     GetRadianAngleBetweenPoints(x, y, 0, 0)) (0x53CB50 / 0x53CBE0); el resultado de ambas pasa por un float en la pila.
//  6. contador = contador + 1, a cero si supera 10 (con signo); devuelve la subtarea (+0x8).
CTask* CTaskComplexWaitForPed::ControlSubTaskBody(CPed* ped) {
    static_assert(offsetof(CPed, m_fAimingRotation) == 0x55C);
    if (!m_ped) {
        return nullptr;
    }
    if (!m_ped->IsAlive()) {
        CEventDeadPed event(m_ped, false, CTimer::GetTimeInMS());
        event.m_TaskId = TASK_COMPLEX_INVESTIGATE_DEAD_PED;
        ped->GetIntelligence()->GetEventGroup().Add(&event, false);
        return nullptr;
    }

    if (m_timer.m_bStarted && m_timer.IsOutOfTime()) {
        return nullptr;
    }

    const CVector& waitedPos = m_ped->GetPosition();
    CVector        world;
    const float*   fromPos;
    if (auto* const alongside = m_ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_WALK_ALONGSIDE_PED)) {
        const CVector local = *reinterpret_cast<const CVector*>(reinterpret_cast<const uint8*>(alongside) + 0x14);
        plugin::CallAndReturn<CVector*, 0x59C890, CVector*, const CMatrix*, const CVector*>(&world, ped->m_matrix, &local);
        fromPos = &world.x;
    } else {
        fromPos = &ped->GetPosition().x;
    }
    // (dz*dz + dy*dy) + dx*dx < radio*radio en la x87 (24 bits, exponente ampliado, sin guardar parciales): X87Math.cpp (el EXE guarda las restas en registros, no como float)
    if (Wfp194NearCpp(fromPos, &waitedPos.x, m_radius)) {
        return nullptr;
    }

    if (m_bRotateOtherPedsToWaitingPed && m_framesToWaitForSettingRotation == 0) {
        float difference[3];
        Veh81BSub3Cpp(&m_ped->GetPosition().x, &ped->GetPosition().x, difference); // 0x40FE60: fld/fsub/fstp por componente (los desnormales no se aplanan como con SSE)
        const float heading = CGeneral::GetRadianAngleBetweenPoints(difference[0], difference[1], 0.0f, 0.0f);
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(heading);
    }

    m_framesToWaitForSettingRotation = m_framesToWaitForSettingRotation + 1;
    if (m_framesToWaitForSettingRotation > 10) {
        m_framesToWaitForSettingRotation = 0;
    }
    return m_pSubTask;
}
