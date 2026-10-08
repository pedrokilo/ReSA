/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskGangHassleVehicle
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 6.
 * Validación registrada: 6/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   CalcTargetOffset | 0x006641A0 | S
 *   Clone | 0x00000000 | V
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 *   GetTargetHeading | 0x0065FDD0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskGangHassleVehicle.h"
#include "TaskComplexTrackEntity.h"
#include "TaskComplexSmartFleeEntity.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskGangHasslePed.h"
#include "TaskSimpleShakeFist.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleFight.h"
#include "Ragdoll/IKChainManager.h"

void CTaskGangHassleVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGangHassleVehicle, 0x86F9D4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65FAC0);
    RH_ScopedInstall(Destructor, 0x65FB60);
    RH_ScopedInstall(GetTargetHeading, 0x65FDD0);
    RH_ScopedInstall(CalcTargetOffset, 0x6641A0);
    RH_ScopedVMTInstall(Clone, 0x65FC00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65FC80);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x664BA0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6637C0);
}

// 0x65FAC0
CTaskGangHassleVehicle::CTaskGangHassleVehicle(CVehicle* vehicle, int32 a3, uint8 a4, float a5, float a6) : CTaskComplex() {
    m_nTime = 0;
    dword3C = 0;
    byte40 = 0;
    byte41 = 0;
    byte18 = a4;
    dword1C = a5;
    m_Vehicle = vehicle;
    m_nHasslePosId = -1;
    m_fOffsetX = a6;
    m_bRemoveAnim = 0;
    m_pEntity = nullptr;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x65FB60
CTaskGangHassleVehicle::~CTaskGangHassleVehicle() {
    if (m_Vehicle) {
        if (m_nHasslePosId > -1) {
            m_Vehicle->SetHasslePosId(m_nHasslePosId, false);
        }
        CEntity::SafeCleanUpRef(m_Vehicle);
    }

    CEntity::SafeCleanUpRef(m_pEntity);

    if (m_bRemoveAnim) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_bRemoveAnim = false;
    }
}

// 0x65FDD0
float CTaskGangHassleVehicle::GetTargetHeading(CPed* ped) {
    auto dir = m_Vehicle->GetRight();
    switch (m_nHasslePosId) {
    case 1: case 3: dir *= -1.f; break;
    case 4: dir = m_Vehicle->GetForward(); break;
    case 5: dir = -m_Vehicle->GetForward(); break;
    }
    return CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f));
}

// 0x6641A0
void CTaskGangHassleVehicle::CalcTargetOffset() {
    m_vecPosn = CVector{0.f, 0.f, 0.f};
    const auto& b = CModelInfo::GetModelInfo(m_Vehicle->m_nModelIndex)->GetColModel()->m_boundBox;
    switch (m_nHasslePosId) {
    case 0: m_vecPosn.x = b.m_vecMin.x - m_fOffsetX; m_vecPosn.y = b.m_vecMax.y*.5f; break;
    case 1: m_vecPosn.x = b.m_vecMax.x + m_fOffsetX; m_vecPosn.y = b.m_vecMax.y*.5f; break;
    case 2: m_vecPosn.x = b.m_vecMin.x - m_fOffsetX; m_vecPosn.y = b.m_vecMin.y*.5f; break;
    case 3: m_vecPosn.x = b.m_vecMax.x + m_fOffsetX; m_vecPosn.y = b.m_vecMin.y*.5f; break;
    case 4: m_vecPosn.y = b.m_vecMin.y - m_fOffsetX; break;
    case 5: m_vecPosn.y = b.m_vecMax.y + m_fOffsetX; break;
    }
}

// 0x65FC80
CTask* CTaskGangHassleVehicle::CreateNextSubTask(CPed* ped) {
    if (!m_Vehicle) return nullptr;
    if (m_pSubTask && m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x38F)) return nullptr;
    if (m_Vehicle->m_fHealth < 250.f) return new CTaskComplexSmartFleeEntity(m_Vehicle, false, 30.f, 1000000, 1000, *reinterpret_cast<const float*>(0xC18CF0));
    if (m_pSubTask && m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x4BC)) return nullptr;
    if (m_pSubTask && m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x39D)) return nullptr;
    auto* storage = CTask::operator new(0x58);
    return storage ? ::new (storage) CTaskComplexTrackEntity(m_Vehicle, m_vecPosn, 1, -1, 10.f, 40.f, 1) : nullptr;
}

// 0x664BA0
CTask* CTaskGangHassleVehicle::CreateFirstSubTask(CPed* ped) {
    if (!m_Vehicle) return nullptr;
    m_pEntity = m_Vehicle->m_pDriver;
    CEntity::SafeRegisterRef(m_pEntity);
    const auto& box = CModelInfo::GetModelInfo(m_Vehicle->m_nModelIndex)->GetColModel()->m_boundBox;
    if (box.m_vecMax.x - box.m_vecMin.x > 4.f || box.m_vecMax.y - box.m_vecMin.y > 8.f) return nullptr;
    m_nHasslePosId = m_Vehicle->GetSpareHasslePosId();
    if (m_nHasslePosId == -1) return nullptr;
    m_Vehicle->SetHasslePosId(m_nHasslePosId, true);
    CalcTargetOffset();
    m_b31 = 0;
    ped->SetPedStats(static_cast<ePedStats>(1));
    m_nTime = CTimer::GetTimeInMS();
    dword3C = CGeneral::GetRandomIntInRange(150000, 250000);
    byte40 = true;
    if (!ped->bInVehicle || !ped->m_pVehicle) return CreateNextSubTask(ped);
    return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
}

// 0x6637C0
CTask* CTaskGangHassleVehicle::ControlSubTask(CPed* ped) {
    if (m_pSubTask && m_pSubTask->GetTaskType() != static_cast<eTaskType>(1000) && !m_Vehicle) {
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr) ? nullptr : m_pSubTask;
    }
    if (!m_bRemoveAnim) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto index = CAnimManager::GetAnimationBlockIndex("gangs");
            if (CAnimManager::GetAnimBlocks()[index].IsLoaded == 1) { CAnimManager::AddAnimBlockRef(index); m_bRemoveAnim = true; }
            else CStreaming::RequestModel(IFPToModelId(index), STREAMING_KEEP_IN_MEMORY);
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_bRemoveAnim = false;
    }
    if (byte40) {
        if (byte41) { m_nTime = CTimer::GetTimeInMS(); byte41 = 0; }
        if (uint32(m_nTime + dword3C) <= CTimer::GetTimeInMS() && byte18) {
            if (m_pSubTask->GetTaskType() != static_cast<eTaskType>(1000)) return new CTaskComplexKillPedOnFoot(static_cast<CPed*>(m_pEntity), -1, 0, 0, 0, 1);
            return m_pSubTask;
        }
    }
    if (m_Vehicle && !m_Vehicle->m_pDriver && m_pEntity && m_pSubTask->GetTaskType() != static_cast<eTaskType>(0x4BC) && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        m_b31 = 3;
        return new CTaskGangHasslePed(static_cast<CPed*>(m_pEntity), byte18 ? 2 : 1, 12000, 20000);
    }
    float distance = 100.f;
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x39D)) distance = *reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(m_pSubTask) + 0x50);
    if (ped->IsVisible() && distance < 4.f && !g_ikChainMan.IsLooking(ped) && CGeneral::GetRandomIntInRange(0, 100) > 60) {
        const auto lookTime = CGeneral::GetRandomIntInRange(1000, 3000);
        if (m_Vehicle->m_pDriver) g_ikChainMan.LookAt("TaskHassleVehicle", ped, m_Vehicle->m_pDriver, lookTime, static_cast<eBoneTag>(5), nullptr, true, .15f, 500, 3, false);
    }
    if (!m_pSubTask || (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0x39D) && m_pSubTask->GetTaskType() != static_cast<eTaskType>(0x38A))) return m_pSubTask;
    if (m_Vehicle->m_fHealth < 250.f && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) return CreateNextSubTask(ped);
    auto& manager = ped->GetTaskManager();
    const float threshold = dword1C*dword1C;
    if (m_b31 == 0) {
        if (!manager.GetTaskSecondary(static_cast<eSecondaryTask>(4)) && CGeneral::GetRandomIntInRange(0, 100) > 60) manager.SetTaskSecondary(new CTaskSimpleShakeFist, static_cast<eSecondaryTask>(4));
        if (distance <= threshold) m_b31 = 1;
        ped->Say(static_cast<eGlobalSpeechContext>(0x101), 0, 1.f, false, false, false);
        return m_pSubTask;
    }
    if (m_b31 == 1) {
        const float heading = GetTargetHeading(ped);
        ped->m_fAimingRotation = heading;
        const float actual = ped->GetHeading();
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x39D)) {
            if (distance <= threshold) { if (std::abs(actual - heading) < .05f) m_b31 = 2; }
            else m_b31 = 0;
        }
        return m_pSubTask;
    }
    if (m_b31 != 2) return m_pSubTask;
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x39D) && threshold + .05f < distance) m_b31 = 0;
    const float heading = GetTargetHeading(ped);
    if (std::abs(double(ped->GetHeading()) - double(heading)) >= double(.1f)) m_b31 = 1;
    const auto speech = CGeneral::GetRandomIntInRange(0, 3);
    if (speech >= 0 && speech <= 2) ped->Say(static_cast<eGlobalSpeechContext>(0xFE + speech), 0, 1.f, false, false, false);
    if (!m_bRemoveAnim || ped->m_nMoveState > static_cast<eMoveState>(5)) return m_pSubTask;
    if (!manager.GetTaskSecondary(static_cast<eSecondaryTask>(4))) {
        const float random = float(CGeneral::GetRandomIntInRange(0, 200));
        CTask* secondary = nullptr;
        if (random > 166.f) secondary = new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0x34), static_cast<AnimationId>(0x128), 4.f, false);
        else if (random > 133.f) secondary = new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0x34), static_cast<AnimationId>(0x129), 4.f, false);
        else if (random > 100.f) secondary = new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0x34), static_cast<AnimationId>(0x12A), 4.f, false);
        else if (random > 70.f) {
            auto* storage = CTask::operator new(sizeof(CTaskSimpleRunAnim));
            if (storage) {
                const int anim = 0x117 + int(double(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0 / 32768.0) * 8.0);
                secondary = ::new (storage) CTaskSimpleRunAnim(static_cast<AssocGroupId>(0x34), static_cast<AnimationId>(anim), 4.f, false);
            }
        } else if (random > 60.f) {
            if (ped->IsPlayingHandSignal()) return m_pSubTask;
            secondary = new CTaskComplexPlayHandSignalAnim(static_cast<AnimationId>(-1), 4.f);
        } else if (random > 40.f) { m_b31 = 0; return m_pSubTask; }
        else return m_pSubTask;
        manager.SetTaskSecondary(secondary, static_cast<eSecondaryTask>(4));
        return m_pSubTask;
    }
    if (threshold < distance) m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
    auto* assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(0x128));
    float hitTime = .5f;
    if (!assoc) {
        assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(0x129));
        hitTime = .7f;
        if (!assoc) { assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<AnimationId>(0x12A)); hitTime = .5f; if (!assoc) return m_pSubTask; }
    }
    if (assoc->m_CurrentTime <= hitTime || assoc->m_CurrentTime - assoc->m_TimeStep >= hitTime) return m_pSubTask;
    const float impulse = m_Vehicle->m_fMass*.02f;
    CVector force{0.f, 0.f, impulse};
    switch (m_nHasslePosId) {
    case 0: case 2: force = -m_Vehicle->GetRight(); break;
    case 1: case 3: force = m_Vehicle->GetRight(); break;
    case 4: force = -m_Vehicle->GetForward(); break;
    case 5: force = m_Vehicle->GetForward(); break;
    }
    m_Vehicle->ApplyTurnForce(CVector{0.f, 0.f, impulse}, force);
    const float health = m_Vehicle->m_fHealth;
    CTaskSimpleFight fight(m_Vehicle, 11, 20000);
    CMatrix matrix(*ped->m_matrix);
    matrix.GetPosition() += ped->GetForward();
    plugin::CallMethod<0x61D5F0, CTaskSimpleFight*, float>(&fight, .5f);
    reinterpret_cast<uint8*>(&fight)[0x20] = 4;
    reinterpret_cast<uint8*>(&fight)[0x21] = 1;
    reinterpret_cast<uint8*>(&fight)[0x23] = 11;
    if (CCollision::ProcessColModels(matrix, *reinterpret_cast<CColModel*>(0xC17824), *m_Vehicle->m_matrix, *m_Vehicle->GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) > 0) {
        plugin::CallMethod<0x61D0B0, CTaskSimpleFight*, CPed*, CVehicle*, CVector*, CVector*, int16, int8>(&fight, ped, m_Vehicle, reinterpret_cast<CVector*>(0xB9ACD0), reinterpret_cast<CVector*>(0xB9ACE0), int16(*reinterpret_cast<uint8*>(0xB9ACF4)), int8(*reinterpret_cast<uint8*>(0xB9ACF3)));
    }
    m_Vehicle->m_fHealth = health;
    m_Vehicle->m_vehicleAudio.AddAudioEvent(static_cast<eAudioEvents>(0x6C), 0.f);
    return m_pSubTask;
}
