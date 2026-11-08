/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexStuckInAir
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

#include "TaskComplexFleePoint.h"
#include "TaskSimpleFall.h"
#include "TaskComplexJump.h"
#include "TaskSimpleInAir.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexFallAndGetUp.h"

#include "PedStuckChecker.h"
#include "TaskComplexStuckInAir.h"

// 0x67BE50
CTask* CTaskComplexStuckInAir::ControlSubTask(CPed* ped) {
    const auto& sc = ped->GetIntelligence()->GetStuckChecker();
    const bool  isStuck = sc.GetState() != PED_STUCK_STATE_NONE;

    CTask* newTask = m_pSubTask;
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_FALL_AND_GET_UP: { // 0x67BE86
        if (const auto fall = notsa::dyn_cast_if_present<CTaskSimpleFall>(m_pSubTask->GetSubTask())) {
            if (fall->IsFinished() && isStuck) {
                ped->bIsStanding = true;
            }
        }
        break;
    }
    case TASK_SIMPLE_STAND_STILL: { // 0x67BEE3
        if (!isStuck) {
            newTask = nullptr;
            break;
        }
        if (ped->GetPlayerData()) { // 0x67BEFA: player check through the player data, not the ped type
            auto* const pad = ped->AsPlayer()->GetPadFromPlayer();
            const float lr  = (float)pad->GetPedWalkLeftRight();
            const float ud  = (float)pad->GetPedWalkUpDown();
            if (std::abs(lr) > 0.f || std::abs(ud) > 0.f) {
                ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(0.f, 0.f, -lr, ud) - TheCamera.m_fOrientation);
            }
            if (pad->JumpJustDown()) {
                newTask = CreateSubTask(TASK_COMPLEX_JUMP, ped);
            }
        }
        ped->bIsInTheAir = false;
        ped->bIsStanding = true;
        for (int32 i = ANIM_ID_JUMP_LAUNCH; i <= ANIM_ID_FALL_GLIDE; i++) {
            if (auto* const a = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(i))) {
                if (a->GetBlendAmount() > 0.f && a->GetBlendDelta() >= 0.f) {
                    a->SetBlendDelta(-8.f);
                }
            }
        }
        break;
    }
    case TASK_COMPLEX_JUMP: { // 0x67C016
        if (isStuck) {
            if (auto* const subSubTask = m_pSubTask->GetSubTask()) {
                if (notsa::isa<CTaskSimpleInAir>(subSubTask) || notsa::isa_and_present<CTaskSimpleInAir>(subSubTask->GetSubTask())) {
                    newTask = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
                }
            }
        }
        break;
    }
    case TASK_COMPLEX_FLEE_POINT: { // 0x67C07A
        if (isStuck) {
            newTask = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        }
        break;
    }
    }

    // 0x67C0A0: abort the old sub-task only after the replacement was created
    if (newTask != m_pSubTask) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT);
    }
    return newTask;
}

// 0x67BE20
CTask* CTaskComplexStuckInAir::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(
        ped->GetIntelligence()->GetStuckChecker().GetState() == PED_STUCK_STATE_WAS_STUCK
            ? TASK_COMPLEX_FALL_AND_GET_UP
            : TASK_SIMPLE_STAND_STILL,
        ped
    );        
}

// 0x67BD10
CTask* CTaskComplexStuckInAir::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_FALL_AND_GET_UP: // 0x67BD39
    case TASK_COMPLEX_FLEE_POINT: { // 0x67BDFA
        return CreateSubTask(
            ped->GetIntelligence()->GetStuckChecker().GetState() == PED_STUCK_STATE_NONE
                ? TASK_FINISHED
                : TASK_SIMPLE_STAND_STILL,
            ped
        );
    }
    case TASK_SIMPLE_STAND_STILL: { // 0x67BD7C
        return CreateSubTask(
            ped->GetIntelligence()->GetStuckChecker().GetState() == PED_STUCK_STATE_NONE
                ? TASK_FINISHED
                : ped->GetPlayerData() // 0x67BD92: player check through the player data
                    ? TASK_SIMPLE_STAND_STILL
                    : TASK_COMPLEX_JUMP,
            ped
        );
    }
    case TASK_COMPLEX_JUMP: { // 0x67BDC1
        return CreateSubTask(
            ped->GetIntelligence()->GetStuckChecker().GetState() != PED_STUCK_STATE_NONE
                ? TASK_SIMPLE_STAND_STILL
                : ped->GetPlayerData() // 0x67BDC4
                    ? TASK_FINISHED
                    : TASK_COMPLEX_FLEE_POINT,
            ped
        );
    }
    }
    return nullptr; // 0x67BE0B
}

// 0x67BA80
CTask* CTaskComplexStuckInAir::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_FLEE_POINT: { // 0x67BBDD
        ped->bIsStanding = true;
        const CVector point = ped->GetPosition()
            - ped->GetForward() * 0.5f
            + ped->GetRight() * ((CGeneral::GetRandomNumber() & 1) ? 0.5f : -0.5f); // 0x67BC5D: low bit, not `DoCoinFlip`
        return new CTaskComplexFleePoint{ point, false, 5.f, 10'000 };
    }
    case TASK_FINISHED: { // 0x67BBE5
        return nullptr;
    }
    case TASK_COMPLEX_JUMP: { // 0x67BB74
        ped->bIsStanding = true;

        auto* const sc = &ped->GetIntelligence()->GetStuckChecker();
        sc->m_radius   = 0;
        sc->m_state    = PED_STUCK_STATE_NONE;

        return new CTaskComplexJump{ CTaskComplexJump::eForceClimb::DISABLE };
    }
    case TASK_SIMPLE_STAND_STILL: { // 0x67BB2C
        ped->bIsStanding = true;

        return new CTaskSimpleStandStill{ 5'000 };
    }
    case TASK_COMPLEX_FALL_AND_GET_UP: { // 0x67BAC9
        auto* const sc = &ped->GetIntelligence()->GetStuckChecker();
        sc->m_radius   = 0;
        sc->m_state    = PED_STUCK_STATE_NONE;

        return new CTaskComplexFallAndGetUp{ ANIM_ID_KO_SKID_BACK, ANIM_GROUP_DEFAULT, 1'000 };
    }
    }
    return nullptr; // 0x67BBE9
}

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x67BA40 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskComplexStuckInAir del EXE (thiscall, sin argumentos, `ret`, devuelve this). Base CTaskComplex (0x61A3B0) y vtable 0x870608.
CTaskComplexStuckInAir* __fastcall StuckInAirConstructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) {
    CTaskComplexBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x870608;
    return self;
}
} // namespace

void CTaskComplexStuckInAir::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexStuckInAir, 0x870608, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(ScalarDeletingDestructor, 0x67FA60);
    RH_ScopedNamedGlobalInstall(StuckInAirConstructorBody, "Constructor", 0x67BA40);

    RH_ScopedGlobalInstall(CreateSubTask, 0x67BA80);
    RH_ScopedVMTInstall(Clone, 0x67C700);
    RH_ScopedVMTInstall(GetTaskType, 0x67BA60);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x67BE20);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67BD10);
    RH_ScopedVMTInstall(ControlSubTask, 0x67BE50);
}

namespace {
// 0x67BA70 (lote 128A, banco Fn128A): cuerpo del destructor de CTaskComplexStuckInAir del EXE, sin hook propio (`mov dword [ecx],0x870608; jmp CTaskComplex::Destructor (0x61A3D0)`, thiscall sin argumentos). Funcion propia sin expansion
// en linea para que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x67BA70` del EXE.
__declspec(noinline) void __fastcall ComplexStuckInAirDestructorBody(CTaskComplexStuckInAir* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870608;
    CTaskComplexBody::Destructor(self, nullptr);
}
} // namespace

uintptr_t Lote128AComplexStuckInAirDestructorBody() { return reinterpret_cast<uintptr_t>(&ComplexStuckInAirDestructorBody); } // para el banco Fn128A

// 0x67FA60 (lote 128A, banco Fn128A) : destructor con borrado de CTaskComplexStuckInAir del EXE (ranura 0 de su vtable 0x870608; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x67BA70; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x67BA70), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexStuckInAir* CTaskComplexStuckInAir::ScalarDeletingDestructor(uint8 flags) {
    ComplexStuckInAirDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
