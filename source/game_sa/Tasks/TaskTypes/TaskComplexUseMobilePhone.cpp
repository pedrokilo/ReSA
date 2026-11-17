/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseMobilePhone
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexUseMobilePhone.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimplePause.h"
#include "Enums/eEventType.h"
#include "Events/Event.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx);  // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// Lote 206A (sin validar). La clase del EXE (vtable 0x86E454, TASK_COMPLEX_USE_MOBILE_PHONE = 0x640, 0x20 bytes) sigue sin reconstruir como clase concreta (el constructor del .h no esta definido y faltan Clone, CreateFirstSubTask y
// ControlSubTask): se enganchan sus funciones sueltas como S (la entrada sirve a la vtable y a las llamadas directas) con funciones libres que reciben el objeto. El listado de entrada las atribuia a CTaskComplexUseClosestFreeScriptedAttractor
// (vtable 0x86E428); la clase real sale de la vtable 0x86E454 (la que escribe 0x6348A0) y de que 0x634A40 (Quit) es de esta misma clase.
static_assert(TASK_COMPLEX_USE_MOBILE_PHONE == 0x640); // 0x6348D0: `mov eax, 640h; ret`
static_assert(TASK_SIMPLE_PHONE_TALK == 0x641 && TASK_SIMPLE_PHONE_IN == 0x642 && TASK_SIMPLE_PHONE_OUT == 0x643 && TASK_SIMPLE_PAUSE == 0xCA);
static_assert(EVENT_ON_FIRE == 0x3A);
static_assert(MODEL_CELLPHONE == 0x14A);
static_assert(WEAPON_UNIDENTIFIED == 0x37);
static_assert(offsetof(CPed, m_nSavedWeapon) == 0x70C);
static_assert(offsetof(CPlayerPedData, m_bDontAllowWeaponChange) == 0x85);

// Los dos bits de peaton que el EXE baja al terminar la llamada (el EXE lee y escribe las palabras de banderas enteras): bit 1 de la palabra +0x478 (bUsingMobilePhone, cuarta palabra de banderas) y bit 22 de la +0x474
// (bDontAcceptIKLookAts, tercera palabra).
void ClearPhoneFlags(CPed* ped) {
    auto* const raw = reinterpret_cast<uint8*>(ped);
    *reinterpret_cast<uint32*>(raw + 0x478) &= 0xFFFFFFFDu;
    *reinterpret_cast<uint32*>(raw + 0x474) &= 0xFFBFFFFFu;
}

// 0x6348A0 (lote 206A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor; thiscall, un argumento de pila (la duracion), `ret 4`, 47 bytes; devuelve `this`): constructor de la clase del EXE. Base CTaskComplex
// (0x61A3B0, con hook), +0xC = duracion, la vtable 0x86E454 (la del EXE) y a cero: el temporizador (+0x10, +0x14, y los BYTE +0x18 y +0x19) y los BYTE +0x1C y +0x1D.
CTaskComplexUseMobilePhone* __fastcall UseMobilePhoneConstructor(CTaskComplexUseMobilePhone* self, void* /*edx*/, int32 duration) {
    CTaskComplexBody::Constructor(self, nullptr);
    self->m_nDuration = duration;
    *reinterpret_cast<uint32*>(self) = 0x86E454;
    self->m_timer.m_nStartTime = 0;
    self->m_timer.m_nInterval  = 0;
    self->m_timer.m_bStarted   = false;
    self->m_timer.m_bStopped   = false;
    self->m_bIsAborting        = false;
    self->m_bQuit              = false;
    return self;
}

// 0x6348D0 (lote 206A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor; ranura 4 (GetTaskType) de la vtable 0x86E454; thiscall, `ret`, 6 bytes: `mov eax, 640h; ret`): no lee el objeto.
eTaskType __fastcall UseMobilePhoneGetTaskType(void* /*self*/, void* /*edx*/) {
    return TASK_COMPLEX_USE_MOBILE_PHONE;
}

// 0x6348E0 (lote 206A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor; thiscall, sin argumentos de pila, 11 bytes: `mov dword [ecx], 86E454h; jmp 0x61A3D0`): cuerpo del destructor de la clase del EXE: deja la vtable
// 0x86E454 y sigue (salto final) con el cuerpo de ~CTaskComplex (0x61A3D0, con hook), que borra la subtarea.
void __fastcall UseMobilePhoneDestructorBody(CTaskComplexUseMobilePhone* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86E454;
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x6348F0 (lote 206A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor; thiscall con un argumento de pila (el peaton), `ret 4`, 62 bytes; no usa `this`): guarda el movil: CPed::RemoveWeaponModel (0x5E3990) con el
// modelo 0x14A (MODEL_CELLPHONE), CPed::SetCurrentWeapon (0x5E6280) con el arma guardada (+0x70C), pone el arma guardada a 0x37 (WEAPON_UNIDENTIFIED) y, si el peaton tiene datos de jugador (+0x480), baja su BYTE +0x85
// (m_bDontAllowWeaponChange). La llaman MakeAbortable (0x634930, desde 0x634987) y otra funcion de la clase del EXE (0x634E1C, sin reconstruir).
void __fastcall UseMobilePhonePutAway(CTaskComplexUseMobilePhone* /*self*/, void* /*edx*/, CPed* ped) {
    ped->RemoveWeaponModel(MODEL_CELLPHONE);
    ped->SetCurrentWeapon(ped->m_nSavedWeapon);
    ped->m_nSavedWeapon = WEAPON_UNIDENTIFIED;
    if (CPlayerPedData* const playerData = ped->GetPlayerData()) {
        playerData->m_bDontAllowWeaponChange = false;
    }
}

// 0x634930 (lote 206A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor; ranura 6 (MakeAbortable) de la vtable 0x86E454; thiscall, tres argumentos de pila (peaton, prioridad, evento), `ret 0Ch`, 262 bytes): como el original:
//  - prioridad 2 (IMMEDIATE): aborta la subtarea (+8) por la vtable (ranura 6) con (peaton, 2, evento); si lo consigue, baja los bits del movil del peaton y, si su arma guardada (+0x70C) no es 0x37, llama a 0x6348F0; devuelve lo que devolvio la subtarea.
//  - otra prioridad: si la subtarea es de tipo 0xCA (TASK_SIMPLE_PAUSE) baja los bits y devuelve verdadero. Si no: con evento no nulo, si es de tipo 0x3A (EVENT_ON_FIRE) y el peaton es jugador (CPed::IsPlayer 0x5DF8F0) devuelve verdadero
//    sin mas; con evento no nulo y otro caso, marca el BYTE +0x1C (m_bIsAborting); con evento nulo no lo marca. Despues reinicia el temporizador SIN la comprobacion de duracion no negativa (+0x10 = CTimer::m_snTimeInMilliseconds,
//    +0x14 = +0xC, BYTE +0x18 = 1) y, si la subtarea es de tipo 0x643 (TASK_SIMPLE_PHONE_OUT), la aborta por la vtable con (peaton, prioridad, evento) (el resultado se ignora). Devuelve falso.
bool __fastcall UseMobilePhoneMakeAbortable(CTaskComplexUseMobilePhone* self, void* /*edx*/, CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        const bool aborted = self->GetSubTaskRaw()->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
        if (aborted) {
            ClearPhoneFlags(ped);
            if (ped->m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
                UseMobilePhonePutAway(self, nullptr, ped);
            }
        }
        return aborted;
    }
    if (self->GetSubTaskRaw()->GetTaskType() == TASK_SIMPLE_PAUSE) {
        ClearPhoneFlags(ped);
        return true;
    }
    if (event) {
        if (event->GetEventType() == EVENT_ON_FIRE && ped->IsPlayer()) {
            return true;
        }
        self->m_bIsAborting = true;
    }
    self->m_timer.m_nStartTime = CTimer::m_snTimeInMilliseconds;
    self->m_timer.m_nInterval  = self->m_nDuration;
    self->m_timer.m_bStarted   = true;
    if (self->GetSubTaskRaw()->GetTaskType() == TASK_SIMPLE_PHONE_OUT) {
        self->GetSubTaskRaw()->MakeAbortable(ped, priority, event);
    }
    return false;
}

// Crea una subtarea de animacion del telefono como el EXE (ver CTaskComplexUseMobilePhone::NewPhoneAnimTask: grupo 0, animacion, 4.0f, tipo, nombre (se ignora), sin mantener el ultimo fotograma).
CTask* NewPhoneAnimTaskImpl(int32 animId, int32 taskType, uintptr_t exeNameAddress) {
    return CTaskComplexUseMobilePhone::NewPhoneAnimTask(animId, taskType, exeNameAddress);
}

// 0x634A60 (lote 206A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor; ranura 8 (CreateNextSubTask) de la vtable 0x86E454; thiscall, un argumento de pila (el peaton), `ret 4`, 425 bytes con marco SEH que solo desenrolla): segun
// el tipo de la subtarea actual (+8, GetTaskType por la vtable):
//  - 0x641 (TASK_SIMPLE_PHONE_TALK): CTaskSimpleRunAnim (grupo 0, animacion 0x92, 4.0f, tipo 0x643 (TASK_SIMPLE_PHONE_OUT), nombre 0x86E48C "PhoneOut").
//  - 0x642 (TASK_SIMPLE_PHONE_IN): si la duracion (+0xC) no es negativa reinicia el temporizador (+0x10 = tiempo del juego, +0x14 = duracion, BYTE +0x18 = 1) y crea CTaskSimpleRunAnim (grupo 0, animacion 0x93, 4.0f, tipo 0x641, nombre 0x86E480 "PhoneChat").
//  - 0x643 (TASK_SIMPLE_PHONE_OUT): con m_bQuit (+0x1D) a cero y m_bIsAborting (+0x1C) distinto de cero crea CTaskSimplePause (0x48E750) con 1000 ms; si no, baja los bits del movil del peaton y devuelve nulo.
//  - 0xCA (TASK_SIMPLE_PAUSE): pone a cero los BYTE +0x1C y +0x18 (temporizador parado); con m_bQuit puesto baja los bits del movil del peaton y devuelve nulo; si no, devuelve CreateFirstSubTask del peaton por la vtable (ranura 9).
//  - cualquier otro tipo: nulo.
// La reserva con CTask::operator new se comprueba como el EXE (pool agotado -> nulo, sin construir).
CTask* __fastcall UseMobilePhoneCreateNextSubTask(CTaskComplexUseMobilePhone* self, void* /*edx*/, CPed* ped) {
    switch (self->GetSubTaskRaw()->GetTaskType()) {
    case TASK_SIMPLE_PHONE_TALK:
        return NewPhoneAnimTaskImpl(0x92, TASK_SIMPLE_PHONE_OUT, 0x86E48C);
    case TASK_SIMPLE_PHONE_IN:
        if (self->m_nDuration >= 0) {
            self->m_timer.m_nStartTime = CTimer::m_snTimeInMilliseconds;
            self->m_timer.m_nInterval  = self->m_nDuration;
            self->m_timer.m_bStarted   = true;
        }
        return NewPhoneAnimTaskImpl(0x93, TASK_SIMPLE_PHONE_TALK, 0x86E480);
    case TASK_SIMPLE_PHONE_OUT:
        if (!self->m_bQuit && self->m_bIsAborting) {
            return CTaskComplexUseMobilePhone::NewPhonePauseTask(1000);
        }
        ClearPhoneFlags(ped);
        return nullptr;
    case TASK_SIMPLE_PAUSE:
        self->m_bIsAborting      = false;
        self->m_timer.m_bStarted = false;
        if (self->m_bQuit) {
            ClearPhoneFlags(ped);
            return nullptr;
        }
        return self->CreateFirstSubTask(ped);
    default:
        return nullptr;
    }
}
} // namespace

// Lote 206A (sin validar): CTask::operator new (0x61A5A0) y, solo si hay memoria, el cuerpo ENGANCHADO del constructor (CTaskSimpleRunAnim::Constructor2 0x61A900, CTaskSimplePause::Constructor 0x48E750; la DLL no construye con su
// constructor de C++): su EAX (this) es el resultado, como en el EXE. Pool agotado -> nulo sin construir.
CTask* CTaskComplexUseMobilePhone::NewPhoneAnimTask(int32 animId, int32 taskType, uintptr_t exeNameAddress) {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleRunAnim));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskSimpleRunAnim*>(memory)->Constructor2(ANIM_GROUP_DEFAULT, static_cast<AnimationId>(animId), 4.0f, taskType, reinterpret_cast<const char*>(exeNameAddress), false);
}
CTask* CTaskComplexUseMobilePhone::NewPhonePauseTask(int32 timeMs) {
    void* const memory = CTask::operator new(sizeof(CTaskSimplePause));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskSimplePause*>(memory)->Constructor(timeMs);
}

#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimplePlayerOnFoot.h"
#include "Camera.h"

namespace CTaskComplexBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// Desplazamientos de CAnimBlendAssociation que lee el EXE (+0x20 y +0x28: con la vtable en +0 son m_CurrentTime y m_TimeStep)
static_assert(offsetof(CAnimBlendAssociation, m_CurrentTime) == 0x20);
static_assert(offsetof(CAnimBlendAssociation, m_TimeStep) == 0x28);
// Tipos de tarea que el EXE pasa al constructor de CTaskSimpleRunAnim (0x642 y 0x643)
static_assert(TASK_SIMPLE_PHONE_IN == 0x642 && TASK_SIMPLE_PHONE_OUT == 0x643);

// 0x639660 (lote 207A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor): destructor con borrado de CTaskComplexUseMobilePhone del EXE (ranura 0 de su vtable 0x86E454; thiscall, un argumento de
// pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6348E0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). El cuerpo 0x6348E0 (vtable 0x86E454 y salto a ~CTaskComplex
// 0x61A3D0) no esta reconstruido: se llama por direccion fija. Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexUseMobilePhone* __fastcall UseMobilePhoneScalarDeletingDestructor(CTaskComplexUseMobilePhone* self, void* /*edx*/, uint8 flags) {
    reinterpret_cast<void(__fastcall*)(CTaskComplexUseMobilePhone*, void*)>(0x6348E0)(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexUseMobilePhone::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseMobilePhone, 0x86E454, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(UseMobilePhoneConstructor, "Constructor", 0x6348A0);         // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(UseMobilePhoneGetTaskType, "GetTaskType", 0x6348D0);         // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(UseMobilePhoneDestructorBody, "DestructorBody", 0x6348E0);   // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(UseMobilePhonePutAway, "PutAway", 0x6348F0);                 // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(UseMobilePhoneMakeAbortable, "MakeAbortable", 0x634930);     // lote 206A (sin validar)
    RH_ScopedInstall(Quit, 0x634A40);
    RH_ScopedNamedGlobalInstall(UseMobilePhoneCreateNextSubTask, "CreateNextSubTask", 0x634A60); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(UseMobilePhoneScalarDeletingDestructor, "ScalarDeletingDestructor", 0x639660); // lote 207A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x636FD0);                // lote 207A (sin validar)
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x634C10);   // lote 207A (sin validar)
    RH_ScopedVMTInstall(ControlSubTask, 0x634D60);       // lote 207A (sin validar)
}

// 0x634A40
void CTaskComplexUseMobilePhone::Quit(CPed* ped) {
    // Como el original: el byte +0x1D (m_bQuit) se prueba entero y se marca antes de abortar; la ranura 6 de la vtable es MakeAbortable (prioridad 0 = leisure, sin evento)
    if (!m_bQuit) {
        m_bQuit = true;
        MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }
}

// 0x636FD0 (lote 207A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor): copia de la tarea (ranura 1 de la vtable 0x86E454; thiscall, sin argumentos, `ret`, 130 bytes con marco SEH que solo desenrolla).
// Reserva 0x20 bytes con CTask::operator new (0x61A5A0; nulo -> devuelve nulo sin construir) y construye en linea sin pasar por el constructor del EXE: cuerpo de CTaskComplex (0x61A3B0, con hook), duracion (+0xC) copiada de
// esta, vtable 0x86E454 (+0) y a cero el temporizador (+0x10 inicio, +0x14 intervalo, +0x18 y +0x19 bytes) y los dos bytes +0x1C (m_bIsAborting) y +0x1D (m_bQuit): la copia no hereda el temporizador ni las banderas.
// La clase es abstracta en la DLL (faltan tres ranuras), asi que no se puede usar `new`; se escribe sobre la memoria reservada como hace el EXE.
CTask* CTaskComplexUseMobilePhone::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskComplexUseMobilePhone));
    if (!memory) {
        return nullptr;
    }
    const int32 duration = m_nDuration;
    CTaskComplexBody::Constructor(memory, nullptr);
    auto* const copy = static_cast<CTaskComplexUseMobilePhone*>(memory);
    copy->m_nDuration = duration;
    *reinterpret_cast<uint32*>(copy) = 0x86E454;
    copy->m_timer.m_nStartTime = 0;
    copy->m_timer.m_nInterval  = 0;
    copy->m_timer.m_bStarted   = false;
    copy->m_timer.m_bStopped   = false;
    copy->m_bIsAborting        = false;
    copy->m_bQuit              = false;
    return copy;
}

// 0x634C10 (lote 207A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor): primera subtarea (ranura 9; thiscall, un argumento de pila, `ret 4`, 330 bytes con marco SEH que solo desenrolla).
// Como el original: 1) marca en el peaton las dos banderas de telefono con lecturas y escrituras de dword (dword +0x474 |= 0x400000, bDontAcceptIKLookAts; dword +0x478 |= 2, bUsingMobilePhone; ambos valores se leen
// antes de escribir); 2) si es el jugador (CPed::IsPlayer 0x5DF8F0), CPlayerPed::ClearWeaponTarget (0x609C80); 3) si el arma guardada (+0x70C) es 55 (WEAPON_UNIDENTIFIED, "ninguna"), la guarda tomando el tipo del
// arma de la ranura activa (byte con signo +0x718; sin acotar) y llama a CPed::SetCurrentWeapon(tipo 0) (0x5E6280); 4) con datos de jugador (+0x480) pone el byte +0x85 (m_bDontAllowWeaponChange); 5) si es el
// jugador, CCamera::ClearPlayerWeaponMode (0x50AB10, sobre TheCamera 0xB6F028); 6) construye una CTaskSimpleStandStill local (tiempo 0, sin repeticion, sin postura de reposo, mezcla 8.0f), llama a su ProcessPed
// (0x62F370; el resultado no se usa) y la destruye; 7) devuelve una CTaskSimpleRunAnim nueva (grupo 0, animacion 0x91 PHONE_IN, mezcla 4.0f, tipo 0x642, "PhoneIn", sin mantener el ultimo fotograma) o nulo si
// CTask::operator new (0x61A5A0) devuelve nulo.
CTask* CTaskComplexUseMobilePhone::CreateFirstSubTask(CPed* ped) {
    uint8* const rawPed    = reinterpret_cast<uint8*>(ped);
    uint32&      flagsB    = *reinterpret_cast<uint32*>(rawPed + 0x478);
    uint32&      flagsA    = *reinterpret_cast<uint32*>(rawPed + 0x474);
    const uint32 newFlagsA = flagsA | 0x400000u;
    const uint32 newFlagsB = flagsB | 2u;
    flagsB = newFlagsB;
    flagsA = newFlagsA;

    if (ped->IsPlayer()) {
        ped->AsPlayer()->ClearWeaponTarget();
    }

    if (ped->m_nSavedWeapon == static_cast<eWeaponType>(0x37)) {
        const int32 slot = static_cast<int8>(ped->m_nActiveWeaponSlot);
        ped->m_nSavedWeapon = ped->m_aWeapons.data()[slot].m_Type;
        ped->SetCurrentWeapon(WEAPON_UNARMED);
    }

    if (ped->GetPlayerData()) {
        ped->GetPlayerData()->m_bDontAllowWeaponChange = true;
    }

    if (ped->IsPlayer()) {
        TheCamera.ClearPlayerWeaponMode();
    }

    {
        CTaskSimpleStandStill standStill{ 0, false, false, 8.0f };
        standStill.ProcessPed(ped);
    }

    void* const memory = CTask::operator new(sizeof(CTaskSimpleRunAnim));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskSimpleRunAnim*>(memory)->Constructor2(ANIM_GROUP_DEFAULT, ANIM_ID_PHONE_IN, 4.0f, TASK_SIMPLE_PHONE_IN, "PhoneIn", false); // 0x61A900 (con hook)
}

// 0x634D60 (lote 207A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor): control de la subtarea (ranura 10; thiscall, un argumento de pila, `ret 4`, 387 bytes con marco SEH que solo desenrolla).
// Como el original: resultado = subtarea actual; con datos de jugador (+0x480, comprobados) pone el byte +0x85; si el tipo de la subtarea (SIN comprobar nulo) NO es 0xCA: pide las asociaciones de animacion 0x91 (PHONE_IN)
// y 0x92 (PHONE_OUT) del clump del peaton (en ese orden); con PHONE_IN, si su tiempo actual (+0x20) NO es menor (ordenado) que 0.85f (0x862E1C), anade el modelo 0x14A al peaton (CPed::AddWeaponModel 0x5E5ED0); sin PHONE_IN
// pero con PHONE_OUT, si su tiempo actual es >= 1.566 (doble 0x86E4A0) y tiempo actual - paso de tiempo (+0x28) < 1.566 (comparaciones ordenadas), llama a la funcion 0x6348F0 del EXE (quitar el modelo, restaurar el arma
// guardada y bajar el byte +0x85; sin reconstruir: se llama por direccion fija con esta tarea y el peaton); sin ninguna de las dos, si el temporizador (+0x10) esta agotado (CTaskTimer::IsOutOfTime 0x420E30) crea una
// CTaskSimpleRunAnim nueva (grupo 0, animacion 0x92, mezcla 4.0f, tipo 0x643, "PhoneOut") como resultado (nulo si CTask::operator new devuelve nulo). Al final, con datos de jugador (releidos): byte +0x84 (m_bPlayerSprintDisabled)
// a 1, el tiempo de carrera (+0x18) a 0.0f si es menor que cero, PlayerControlZelda (0x6883D0, pad 1) de la tarea de a pie del peaton (CPedIntelligence::FindTaskByType 0x600EE0 con tipo 0) si existe y el byte +0x84 a 0
// (releyendo los datos de jugador). Devuelve el resultado.
CTask* CTaskComplexUseMobilePhone::ControlSubTask(CPed* ped) {
    CTask* result = m_pSubTask;
    if (ped->GetPlayerData()) {
        ped->GetPlayerData()->m_bDontAllowWeaponChange = true;
    }

    if (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCA)) {
        CAnimBlendAssociation* const phoneIn  = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_PHONE_IN);
        CAnimBlendAssociation* const phoneOut = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_PHONE_OUT);
        if (phoneIn) {
            if (phoneIn->m_CurrentTime >= 0.85f) { // 0x862E1C; `fcomp; test ah,1; jne` salta con menor o no ordenado (aqui: solo mayor o igual ordenado)
                ped->AddWeaponModel(0x14A);
            }
        } else if (phoneOut) {
            if (static_cast<double>(phoneOut->m_CurrentTime) >= 1.566) { // doble 0x86E4A0; igual que arriba, un NaN salta
                const float elapsed = phoneOut->m_CurrentTime - phoneOut->m_TimeStep;
                if (static_cast<double>(elapsed) < 1.566) {
                    reinterpret_cast<void(__thiscall*)(CTaskComplexUseMobilePhone*, CPed*)>(0x6348F0)(this, ped);
                }
            }
        } else if (m_timer.IsOutOfTime()) {
            void* const memory = CTask::operator new(sizeof(CTaskSimpleRunAnim));
            result = memory ? static_cast<CTaskSimpleRunAnim*>(memory)->Constructor2(ANIM_GROUP_DEFAULT, ANIM_ID_PHONE_OUT, 4.0f, TASK_SIMPLE_PHONE_OUT, "PhoneOut", false) : nullptr; // 0x61A900 (con hook)
        }
    }

    if (CPlayerPedData* const playerData = ped->GetPlayerData()) {
        playerData->m_bPlayerSprintDisabled = true;
        if (playerData->m_fTimeCanRun < 0.0f) {
            playerData->m_fTimeCanRun = 0.0f;
        }
        if (auto* const onFoot = ped->GetIntelligence()->FindTaskByType(TASK_SIMPLE_PLAYER_ON_FOOT)) {
            static_cast<CTaskSimplePlayerOnFoot*>(onFoot)->PlayerControlZelda(ped->AsPlayer(), true);
        }
        ped->GetPlayerData()->m_bPlayerSprintDisabled = false;
    }
    return result;
}
