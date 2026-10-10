/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGangFollower
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

#include "TaskComplexGangFollower.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskComplexSignalAtPed.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimplePause.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexWanderGang.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskComplexPassObject.h"
#include "EventPassObject.h"
#include "SeekEntity/TaskComplexSeekEntity.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexGangFollower::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangFollower, 0x86F938, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65EAA0);
    RH_ScopedInstall(Destructor, 0x65EBB0);
    //RH_ScopedInstall(CalculateOffsetPosition, 0x65ED40, { .reversed = false }); // not hooked because i want to keep CVector return, but original function took a CVector&
    RH_ScopedInstall(SetCanEnterLeadersCar, 0x65EE90);
    RH_ScopedVMTInstall(Clone, 0x65ECB0);
    RH_ScopedVMTInstall(MakeAbortable, 0x65EC30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x665E00);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x666160);
    RH_ScopedVMTInstall(ControlSubTask, 0x662A10);
}

// 0x65EAA0
CTaskComplexGangFollower::CTaskComplexGangFollower(CPedGroup* pedGroup, CPed* ped, uint8 a4, CVector pos, float a6) : CTaskComplex() {
    dword40 = dword44 = 0;
    byte48 = byte49 = 0;
    m_PedGroup = pedGroup;
    byte3C = a4;
    dword2C = dword20 = pos;
    dword38 = a6;
    m_Leader = ped;
    m_Flags |= 0x14;
    if (m_Leader) { CEntity::SafeRegisterRef(m_Leader); m_PedPosn = m_Leader->GetPosition(); }
    m_Flags &= 0xFC;
    m_Flags = m_Leader == FindPlayerPed(0) ? m_Flags | 8 : m_Flags & 0xF7;
}


// 0x65EBB0
CTaskComplexGangFollower::~CTaskComplexGangFollower() {
    CEntity::SafeCleanUpRef(m_Leader);
    if (m_Flags & 1) { CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs")); m_Flags &= 0xFE; }
}

// 0x65ED40
CVector CTaskComplexGangFollower::CalculateOffsetPosition() {
    const auto state = m_Leader->m_nMoveState;
    if (state == PEDMOVE_WALK || state == PEDMOVE_RUN || state == PEDMOVE_SPRINT) {
        const auto offset = CTaskComplexFollowLeaderInFormation::ms_offsets.MovingOffsets[byte3C];
        dword20.FromMultiply3x3(*m_Leader->m_matrix, CVector{offset.x, offset.y, 0.f});
        m_Flags &= 0xEF;
    } else {
        const auto delta = m_Leader->GetPosition() - m_PedPosn;
        if (!(delta.SquaredMagnitude() <= 9.f && (m_Flags & 0x10))) {
            m_PedPosn = m_Leader->GetPosition();
            const auto offset = CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[byte3C];
            dword20 = CVector{offset.x, offset.y, 0.f};
            m_Flags |= 0x10;
        }
    }
    return dword20;
}

// 0x65ECB0
CTask* CTaskComplexGangFollower::Clone() const {
    auto* task = new CTaskComplexGangFollower(m_PedGroup, m_Leader, byte3C, dword20, dword38);
    task->m_Flags = (task->m_Flags & ~4u) | (m_Flags & 4u);
    return task;
}

// 0x65EC30
bool CTaskComplexGangFollower::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_pSubTask && !m_pSubTask->MakeAbortable(ped, priority, event)) return false;
    *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x478) &= 0xFFFEFFFF;
    *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x474) &= 0xFFBFFFFF;
    return true;
}

// 0x665E00
CTask* CTaskComplexGangFollower::CreateNextSubTask(CPed* ped) {
    const auto clear = [ped] { *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x478) &= 0xFFFEFFFF; };
    if (!m_Leader) { clear(); return nullptr; }
    const auto counter = ped->GetIntelligence()->m_AnotherStaticCounter;
    if ((m_Flags & 2) && m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x4BA)) {
        m_PedGroup->GetMembership().RemoveMember(ped);
        auto* storage = CTask::operator new(sizeof(CTaskComplexWanderGang));
        auto* task = storage ? ::new (storage) CTaskComplexWanderGang(PEDMOVE_WALK, uint8(CGeneral::GetRandomIntInRange(0, 8)), 30000, true, .5f) : nullptr;
        ped->GetTaskManager().SetTask(task, TASK_PRIMARY_DEFAULT, false);
        clear();
        return nullptr;
    }
    if (counter < 31 && (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0x38B) || m_Leader->m_nMoveState > PEDMOVE_TURN_R)) {
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0xCB) || m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x1AA)) {
            if (m_Flags & 2) return new CTaskComplexSignalAtPed(m_Leader, -1, false);
            if (CGeneral::GetRandomIntInRange(0, 30) == 20) {
                auto& members = m_PedGroup->GetMembership();
                auto* other = members.GetMember(CGeneral::GetRandomIntInRange(0, members.CountMembers()));
                if (other == ped) other = members.GetLeader();
                if (other) return new CTaskComplexTurnToFaceEntityOrCoord(other, .5f, .2f);
            }
            return new CTaskSimplePause(50);
        }
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x2C5)) return CreateFirstSubTask(ped);
        if (m_Flags & 4) {
            auto* task = new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>(m_Leader, 50000, 1000, .5f, 5.f, 2.f, false, false);
            task->GetSeekPosCalculator().SetOffset(dword20);
            task->SetIsTrackingEntity(true);
            task->SetMoveState(PEDMOVE_SPRINT);
            return task;
        }
    } else if (m_Leader->IsPlayer()) {
        ped->Say(static_cast<eGlobalSpeechContext>(0x5A), 0, .3f, false, false, false);
    }
    return new CTaskSimpleStandStill(500, false, false, 8.f);
}

// 0x666160
CTask* CTaskComplexGangFollower::CreateFirstSubTask(CPed* ped) {
    if (!m_Leader) { *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x478) &= 0xFFFEFFFF; return nullptr; }
    if (ped->bInVehicle && ped->m_pVehicle) {
        if (m_Leader->m_pVehicle == ped->m_pVehicle) return new CTaskSimpleCarDrive(ped->m_pVehicle, nullptr, false);
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    }
    if ((m_Flags & 4) && ped->GetIntelligence()->m_AnotherStaticCounter < 31) {
        auto* task = new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>(m_Leader, 50000, 1000, .5f, 5.f, 2.f, false, true);
        task->GetSeekPosCalculator().SetOffset(dword20);
        task->SetIsTrackingEntity(true);
        task->SetMoveState(PEDMOVE_SPRINT);
        return task;
    }
    return new CTaskSimpleStandStill(500, false, false, 8.f);
}

// 0x662A10
CTask* CTaskComplexGangFollower::ControlSubTask(CPed* ped) {
    auto& pedFlags = *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x474);
    auto& followFlags = *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x478);
    pedFlags &= 0xFFBFFFFF;
    if (!m_Leader) {
        if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { followFlags &= 0xFFFEFFFF; return nullptr; }
        return m_pSubTask;
    }
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x2C5)) return m_pSubTask;
    auto* idle = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(0));
    const auto leaderMove = m_Leader->GetIntelligence()->GetMoveStateFromGoToTask();
    const bool leaderMoving = leaderMove == PEDMOVE_WALK || leaderMove == PEDMOVE_RUN || leaderMove == PEDMOVE_SPRINT;
    CalculateOffsetPosition();
    const auto leaderDelta = m_Leader->GetPosition() - ped->GetPosition();
    bool constrainSpeed = false;
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x38B) && (m_Flags & 4)) {
        auto* seek = static_cast<CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>*>(m_pSubTask);
        seek->GetSeekPosCalculator().SetOffset(dword20);
        *reinterpret_cast<float*>(reinterpret_cast<uint8*>(seek) + 0x20) = 2.f;
        const bool stalled = ped->GetIntelligence()->m_AnotherStaticCounter > 8;
        auto* gotoPoint = ped->GetTaskManager().FindActiveTaskByType(static_cast<eTaskType>(900));
        auto* follow = ped->GetTaskManager().FindActiveTaskByType(static_cast<eTaskType>(0x38A));
        if (gotoPoint && !follow) {
            CVector destination = m_Leader->GetPosition() + dword20;
            CVector delta = destination - ped->GetPosition();
            delta.z = 0.f;
            if (leaderMoving) {
                const auto& forward = m_Leader->GetForward();
                const float plane = -((destination.x*forward.x + destination.z*forward.z) + destination.y*forward.y);
                const auto& pos = ped->GetPosition();
                const float dot = DotProduct(pos, forward);
                if (dot + plane < 0.f) {
                    const float radius = *reinterpret_cast<float*>(reinterpret_cast<uint8*>(gotoPoint) + 0x18);
                    if (delta.SquaredMagnitude() > (radius + 1.f)*(radius + 1.f)) {
                        if (idle) {
                            const float previous = idle->m_Speed;
                            ped->SetMoveAnimSpeed(idle);
                            const float current = idle->m_Speed;
                            if (!approxEqual(previous, current, std::bit_cast<float>(uint32{0x3C54FDF4}))) idle->m_Speed = current < previous ? previous - .0125f : previous + .0125f;
                            else idle->m_Speed = previous;
                            constrainSpeed = true;
                        }
                    } else {
                        destination += forward*2.f;
                        if (idle) { const float speed = idle->m_Speed - .0125f; idle->m_Speed = .85f > speed ? .85f : speed; constrainSpeed = true; }
                    }
                }
            }
            static_cast<CTaskSimpleGoToPoint*>(gotoPoint)->UpdatePoint(destination, .5f, false);
        }
        if (stalled && leaderDelta.x*leaderDelta.x + leaderDelta.y*leaderDelta.y < 64.f && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            dword20 = CVector{leaderDelta.x, leaderDelta.y, 0.f};
            return new CTaskSimpleStandStill(500, false, false, 8.f);
        }
    }
    followFlags = (followFlags & ~0x10000u) | (constrainSpeed ? 0x10000u : 0u);
    if (!(m_Flags & 1)) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto index = CAnimManager::GetAnimationBlockIndex("gangs");
            if (CAnimManager::GetAnimBlocks()[index].IsLoaded == 1) { CAnimManager::AddAnimBlockRef(index); m_Flags |= 1; }
            else CStreaming::RequestModel(IFPToModelId(index), STREAMING_KEEP_IN_MEMORY);
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_Flags &= 0xFE;
    }
    if (!m_PedGroup->m_bIsMissionGroup && !(m_Flags & 8) && m_PedGroup->GetMembership().CountMembers() > 3 && CGeneral::GetRandomIntInRange(0, 2000) == 500) m_Flags |= 2;
    auto& timer = *reinterpret_cast<CTaskTimer*>(&dword40);
    if (byte48 && timer.IsOutOfTime() && ped->GetRwObject()) {
        auto* frame = RpClumpGetFrame(ped->GetRpClump());
        auto* fx = g_fxMan.CreateFxSystem("exhale", CVector{0.f,.1f,0.f}, RwFrameGetMatrix(frame), false);
        if (fx) { fx->AttachToBone(ped, static_cast<eBoneTag>(5)); fx->PlayAndKill(); }
        byte48 = 0;
    }
    if (!ped->IsVisible()) return m_pSubTask;
    if (!g_ikChainMan.IsLooking(ped) && CGeneral::GetRandomIntInRange(0, 100) > 95) {
        const int time = CGeneral::GetRandomIntInRange(3000, 5000);
        auto* other = m_PedGroup->GetMembership().GetMember(CGeneral::GetRandomIntInRange(0, 8));
        if (other == ped) other = m_Leader;
        if (other) g_ikChainMan.LookAt("TaskGangFollower", ped, other, time, static_cast<eBoneTag>(5), nullptr, true, .15f, 500, 3, false);
    }
    if (!(m_Flags & 1) || ped->m_nMoveState > PEDMOVE_JOG) return m_pSubTask;
    auto* held = ped->GetEntityThatThisPedIsHolding();
    auto speak = [&]() {
        switch (CGeneral::GetRandomIntInRange(0, 10)) {
        case 0: case 1: case 2: ped->Say(static_cast<eGlobalSpeechContext>(0x2D),0,1.f,false,false,false); break;
        case 3: case 4: case 5: case 6: case 7: ped->Say(static_cast<eGlobalSpeechContext>(0xA6),0,1.f,false,false,false); break;
        case 8: ped->Say(static_cast<eGlobalSpeechContext>(0x18),0,1.f,false,false,false); break;
        case 9: ped->Say(static_cast<eGlobalSpeechContext>(0xC9),0,1.f,false,false,false); break;
        }
    };
    if (!held) {
        if (ped->IsPlayingHandSignal() || ped->GetTaskManager().GetTaskSecondary(static_cast<eSecondaryTask>(4))) return m_pSubTask;
        const auto random = CGeneral::GetRandomIntInRange(0, 500);
        if (random > 50 && random < 56) {
            auto* storage = CTask::operator new(sizeof(CTaskSimpleRunAnim));
            CTask* animation = nullptr;
            if (storage) {
                const auto value = CGeneral::GetRandomNumber() & 0xFFFF;
                const int anim = 0x117 + int(double(value)*(1.0/32768.0)*8.0);
                animation = ::new (storage) CTaskSimpleRunAnim(static_cast<AssocGroupId>(0x34),static_cast<AnimationId>(anim),4.f,false);
            }
            ped->GetTaskManager().SetTaskSecondary(animation, static_cast<eSecondaryTask>(4));
            if (ped->m_nMoveState == PEDMOVE_STILL) speak();
        } else if (random == 100 && ped->m_nMoveState == PEDMOVE_STILL) {
            ped->GetTaskManager().SetTaskSecondary(new CTaskComplexPlayHandSignalAnim(static_cast<AnimationId>(-1),4.f), static_cast<eSecondaryTask>(4));
            speak();
        }
        return m_pSubTask;
    }
    auto* drinkStart = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(299));
    auto* drinkIdle = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(300));
    auto* smokeStart = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(301));
    auto* smokeIdle = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(302));
    if (drinkStart || drinkIdle || smokeStart || smokeIdle) {
        if (g_ikChainMan.IsLooking(ped)) g_ikChainMan.AbortLookAt(ped, 250);
        pedFlags |= 0x400000;
        if (((drinkIdle && drinkIdle->m_CurrentTime < .5f) || (smokeIdle && smokeIdle->m_CurrentTime < .5f)) && !byte48) { dword40 = CTimer::GetTimeInMS(); dword44 = 2700; byte48 = 1; }
    }
    if (ped->GetTaskManager().FindActiveTaskByType(static_cast<eTaskType>(0x4BB))) return m_pSubTask;
    if (CGeneral::GetRandomIntInRange(0, 500) != 200) {
        if (CGeneral::GetRandomIntInRange(0, 100) == 50) {
            if (auto* task = ped->GetTaskManager().FindActiveTaskByType(static_cast<eTaskType>(0x133))) {
                static_cast<CTaskSimpleHoldEntity*>(task)->PlayAnim(CTaskComplexGangLeader::GetRandomGangAmbientAnim(ped, held), static_cast<AssocGroupId>(0x34));
            }
        }
        return m_pSubTask;
    }
    if (drinkStart || drinkIdle || smokeStart || smokeIdle) return m_pSubTask;
    auto* receiver = CTaskComplexGangLeader::TryToPassObject(ped, m_PedGroup);
    if (!receiver || receiver->GetEntityThatThisPedIsHolding() || receiver->GetActiveWeapon().m_Type != static_cast<eWeaponType>(0)) return m_pSubTask;
    if (int32(int16(held->m_nModelIndex)) == int32(uint16(eModelID(ModelIndices::MI_GANG_DRINK)))) {
        if (CGeneral::GetRandomIntInRange(0, 500) < 250) receiver->Say(static_cast<eGlobalSpeechContext>(0x18),0,1.f,false,false,false);
        else ped->Say(static_cast<eGlobalSpeechContext>(0x17),1500,1.f,false,false,false);
    } else if (int32(int16(held->m_nModelIndex)) == int32(uint16(eModelID(ModelIndices::MI_GANG_SMOKE)))) {
        if (CGeneral::GetRandomIntInRange(0, 500) < 250) receiver->Say(static_cast<eGlobalSpeechContext>(0xC9),0,1.f,false,false,false);
        else ped->Say(static_cast<eGlobalSpeechContext>(0xC8),1500,1.f,false,false,false);
    }
    CEventPassObject event(ped, false);
    receiver->GetEventGroup().Add(&event, false);
    return new CTaskComplexPassObject(receiver, true);
}
