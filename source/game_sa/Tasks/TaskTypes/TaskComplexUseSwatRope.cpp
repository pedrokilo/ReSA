/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseSwatRope
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexUseSwatRope.h"

#include "TaskSimpleAbseil.h"
#include "TaskSimplePause.h"
#include "TaskSimpleNone.h"
#include "PedPlacement.h"
#include "Rope.h"
#include "Ropes.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x6594D0 (lote 153A, banco Fn153A): cuerpo del destructor de CTaskComplexUseSwatRope del EXE (thiscall, sin argumentos de pila, `ret`, 96 bytes con marco SEH que solo desenrolla): deja la vtable de la clase (0x86F59C);
// si el bit 0 del byte +0x14 (m_bIsOnHeli) esta puesto y el helicoptero (+0x18) no es nulo, libera la referencia con CEntity::CleanUpOldReference (0x571A00, con hook); y llama al cuerpo de ~CTaskComplex
// (0x61A3D0, con hook). El destructor generado por el compilador dejaria la vtable de la DLL. Lo llama el destructor con borrado 0x65A3C0. El marco SEH se omite.
void __fastcall CTaskComplexUseSwatRopeDestructorBodyExe(CTaskComplexUseSwatRope* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F59C;
    if (self->m_bIsOnHeli) {
        CEntity::SafeCleanUpRef(self->m_pHeli);
    }
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x65A3C0 (lote 170A, banco Fn170A): destructor con borrado de CTaskComplexUseSwatRope del EXE (ranura 0 de su vtable 0x86F59C; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6594D0;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo del EXE (0x6594D0, con hook), libera `this` con CTask::operator delete (0x61A5B0,
// con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexUseSwatRope* __fastcall CTaskComplexUseSwatRopeScalarDeletingDestructor(CTaskComplexUseSwatRope* self, void* /*edx*/, uint8 flags) {
    CTaskComplexUseSwatRopeDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x659410 (lote 170A, banco Fn170A; el listado lo atribuia a CTaskComplexDriveFireTruck): constructor de CTaskComplexUseSwatRope sin helicoptero del EXE (thiscall, un argumento de pila, `ret 4`, 44 bytes). Como el original:
// base CTaskComplex (0x61A3B0, con hook), lee el byte +0x14 (m_bIsOnHeli es su bit 0) tras ella, +0xC = identificador de la cuerda, +0x10 (distancia recorrida) = +0x18 (helicoptero) = 0, vtable de la clase del EXE
// (0x86F59C) y el byte +0x14 con el bit 0 a cero (el resto no se toca); devuelve `this`. El constructor de la clase (explicit CTaskComplexUseSwatRope(uint32)) deja la vtable de la DLL.
__declspec(noinline) CTaskComplexUseSwatRope* __fastcall CTaskComplexUseSwatRopeConstructorNoHeliExe(CTaskComplexUseSwatRope* self, void* /*edx*/, uint32 ropeId) {
    CTaskComplexBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    const uint8 flags = bytes[0x14];
    *reinterpret_cast<uint32*>(bytes + 0xC) = ropeId;
    *reinterpret_cast<uint32*>(bytes + 0x10) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x18) = 0;
    *reinterpret_cast<uint32*>(bytes) = 0x86F59C;
    bytes[0x14] = static_cast<uint8>(flags & 0xFE);
    return self;
}
} // namespace

void CTaskComplexUseSwatRope::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseSwatRope, 0x86F59C, 13);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexUseSwatRopeDestructorBodyExe, "DestructorBody", 0x6594D0); // lote 153A (banco Fn153A)
    RH_ScopedNamedGlobalInstall(CTaskComplexUseSwatRopeScalarDeletingDestructor, "ScalarDeletingDestructor", 0x65A3C0); // lote 170A (banco Fn170A)
    RH_ScopedNamedGlobalInstall(CTaskComplexUseSwatRopeConstructorNoHeliExe, "ConstructorNoHeli", 0x659410); // lote 170A (banco Fn170A)
    RH_ScopedVMTInstall(GetTaskType, 0x659440); // lote 170A (banco Fn170A): retorno de constante
    RH_ScopedVMTInstall(IsInterruptable, 0x659450); // lote 170A (banco Fn170A): ranura 11
    RH_ScopedVMTInstall(AbortUrgently, 0x659460); // lote 170A (banco Fn170A): ranura 12
    RH_ScopedInstall(Constructor, 0x659470);
    RH_ScopedInstall(CreateSubTask, 0x659620);
    RH_ScopedVMTInstall(Clone, 0x659C30);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65A440);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65A3E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x65A460);
    RH_ScopedVMTInstall(MakeAbortable, 0x659530);
}

CTaskComplexUseSwatRope* CTaskComplexUseSwatRope::Constructor(uint32 ropeId, CHeli* heli) {
    this->CTaskComplexUseSwatRope::CTaskComplexUseSwatRope(ropeId, heli);
    return this;
}

// 0x659470
CTaskComplexUseSwatRope::CTaskComplexUseSwatRope(uint32 ropeId, CHeli* heli) : CTaskComplex() {
    m_nRopeId        = ropeId;
    m_pHeli          = heli;
    m_fCoorAlongRope = 0.0F;
    m_bIsOnHeli      = true;

    // todo: CEntity::SafeRegisterRef
    m_pHeli->RegisterReference(reinterpret_cast<CEntity**>(&m_pHeli));
}

CTaskComplexUseSwatRope::CTaskComplexUseSwatRope(uint32 ropeId) : CTaskComplex() {
    m_nRopeId        = ropeId;
    m_pHeli          = nullptr;
    m_fCoorAlongRope = 0.0F;
    m_bIsOnHeli      = false;
}

// El destructor generado de la clase (el destructor con borrado del EXE 0x65A3C0 esta arriba, con hook).
CTaskComplexUseSwatRope::~CTaskComplexUseSwatRope() {
    if (m_bIsOnHeli) {
        CEntity::SafeCleanUpRef(m_pHeli);
    }
}

// 0x659440 (lote 170A, banco Fn170A; thiscall, `ret`, 6 bytes: `mov eax,6Ch`): TASK_COMPLEX_USE_SWAT_ROPE (108); no lee el objeto (ranura 4 de la vtable 0x86F59C).
eTaskType CTaskComplexUseSwatRope::GetTaskType() const {
    return Type;
}

// 0x659450 (lote 170A, banco Fn170A; thiscall, un argumento de pila que no se lee, `ret 4`, 5 bytes: `xor al,al`): ranura 11 de la vtable 0x86F59C; devuelve falso.
bool CTaskComplexUseSwatRope::IsInterruptable(CPed* /*ped*/) {
    return false;
}

// 0x659460 (lote 170A, banco Fn170A): como IsInterruptable (ranura 12).
bool CTaskComplexUseSwatRope::AbortUrgently(CPed* /*ped*/) {
    return false;
}

// 0x659C30
CTask* CTaskComplexUseSwatRope::Clone() const {
    return m_bIsOnHeli
        ? new CTaskComplexUseSwatRope(m_nRopeId, m_pHeli)
        : new CTaskComplexUseSwatRope(m_nRopeId);
}

// 0x659530
bool CTaskComplexUseSwatRope::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // Como el original: con prioridad IMMEDIATE no se toca el evento (puede ser nulo);
    // si no, se pide el tipo en cada comparación y la salud debe ser estrictamente
    // menor que 20 (fcomp + test ah,5 + jp: igual o NaN no aborta)
    if ((priority == ABORT_PRIORITY_IMMEDIATE
        || event->GetEventType() == EVENT_DEATH
        || event->GetEventType() == EVENT_HEALTH_REALLY_LOW
        || event->GetEventType() == EVENT_ON_FIRE
        || event->GetEventType() == EVENT_IN_WATER
        || event->GetEventType() == EVENT_WATER_CANNON
        || event->GetEventType() == EVENT_VEHICLE_DAMAGE_COLLISION
        || event->GetEventType() == EVENT_VEHICLE_DAMAGE_WEAPON
        || ped->m_fHealth < 20.0F
        )
        && m_pSubTask->MakeAbortable(ped, priority, event)
    ) {
        ped->bIsStanding = false;
        ped->SetUsesCollision(true);
        ped->ResetMoveSpeed();
        return true;
    }
    else
        return false;
}

// 0x65A3E0
CTask* CTaskComplexUseSwatRope::CreateNextSubTask(CPed* ped) {
    eTaskType subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_NONE || subTaskType == TASK_SIMPLE_PAUSE)
        return CreateSubTask(TASK_FINISHED, ped);

    if (subTaskType == TASK_SIMPLE_ABSEIL)
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);

    return nullptr;
}

// 0x65A440
CTask* CTaskComplexUseSwatRope::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ABSEIL, ped);
}

// 0x65A460
CTask* CTaskComplexUseSwatRope::ControlSubTask(CPed* ped) {
    if (   m_bIsOnHeli
        && (
               !m_pHeli
               || m_pHeli->m_autoPilot.m_nCarMission == MISSION_HELI_CRASH_AND_BURN
               || m_pHeli->m_fHealth <= 0.0F
           )
        && m_pSubTask->MakeAbortable(ped)
    ) {
        ped->bIsStanding = false;
        ped->SetUsesCollision(true);
        ped->ResetMoveSpeed();
        return nullptr;
    }

    eTaskType subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_SIMPLE_PAUSE || subTaskType == TASK_SIMPLE_ABSEIL) {
        CVector groundCoord = ped->GetPosition();
        groundCoord = CPedPlacement::FindZCoorForPed(groundCoord).first; // Como el original: actualiza la posición en el sitio
        // Como el original: si ya está cerca del suelo, o si no se encuentra el punto
        // de la cuerda, se intenta abortar la subtarea; si no se puede, se devuelve
        // la subtarea sin avanzar por la cuerda (NaN en la comparación sigue la cuerda)
        if (!(ped->GetPosition().z - 2.0F < groundCoord.z)) {
            m_fCoorAlongRope += CTimer::GetTimeStep() * 0.003F;
            CVector posn;
            if (CRopes::FindCoorsAlongRope(m_nRopeId, m_fCoorAlongRope, &posn, nullptr)) {
                ped->SetPosn(posn);
                ped->m_fAimingRotation = ped->m_fCurrentRotation + CTimer::GetTimeStep() * -0.05F;
                ped->m_vecMoveSpeed.z = -0.03f;
                ped->Say(CTX_GLOBAL_ROPE);
                return m_pSubTask;
            }
        }
        if (m_pSubTask->MakeAbortable(ped)) {
            return CreateSubTask(TASK_NONE, ped);
        }
    }

    return m_pSubTask;
}

// 0x659620
CTask* CTaskComplexUseSwatRope::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_ABSEIL:
        return new CTaskSimpleAbseil();
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause(100'000);
    case TASK_NONE:
        return new CTaskSimpleNone();
    case TASK_FINISHED:
        ped->SetUsesCollision(true);
        ped->ResetMoveSpeed();
        return nullptr;
    default:
        return nullptr;
    }
}
