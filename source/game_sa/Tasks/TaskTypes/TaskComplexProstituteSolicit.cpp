/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexProstituteSolicit
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

#include "TaskComplexProstituteSolicit.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexCarDrive.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "SeekEntity/TaskComplexSeekEntityXYOffset.h"
#include "CarEnterExit.h"

void CTaskComplexProstituteSolicit::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexProstituteSolicit, 0x86FB88, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x661A60);
    RH_ScopedInstall(Destructor, 0x661AF0);

    RH_ScopedInstall(CreateSubTask, 0x666360);
    RH_ScopedInstall(GetRidOfPlayerProstitute, 0x661D30);
    RH_ScopedInstall(IsTaskValid, 0x661BB0);

    RH_ScopedVMTInstall(Clone, 0x6622F0);
    RH_ScopedVMTInstall(GetTaskType, 0x661AE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x661B80);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6666A0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x666780);
    RH_ScopedVMTInstall(ControlSubTask, 0x6669D0);
}

// 0x661A60
CTaskComplexProstituteSolicit::CTaskComplexProstituteSolicit(CPed* client) : CTaskComplex() {
    m_nLastSavedTime = 0;
    m_nNextTimeToCheckForSecludedPlace = 0;
    m_nLastPaymentTime = 0;
    m_nCurrentTimer = 0;
    m_pClient = client;
    m_nVehicleMovementTimer = 850;
    // 0x661A60: flags = (flags & 0xE2C0) | 0x02C0; preserve the unnamed upper bits.
    bSearchingForSecludedPlace = false;
    bSexProcessStarted = false;
    bTaskCanBeFinished = false;
    bPedsCanPotentiallySeeThis = false;
    bPedsCanSeeThis = false;
    bCopsCanSeeThis = false;
    bMoveCameraDown = false;
    bPlayerHasAcceptedSexProposition = false;
    bVehicleShifted = false;
    bSecludedPlaceMessageShown = false;
    b07 = true;
    b08 = true;
    b10 = true;
    m_pClient->RegisterReference(m_pClient);
}

// 0x661AF0
CTaskComplexProstituteSolicit::~CTaskComplexProstituteSolicit() {
    auto player = FindPlayerPed();
    if (!player)
        return;

    CEntity::ClearReference(player->GetPlayerData()->m_pCurrentProstitutePed);
    if (bMoveCameraDown) {
        bMoveCameraDown = false;
    }
}

// 0x661B80
bool CTaskComplexProstituteSolicit::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    bool aborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (aborted) {
        bMoveCameraDown = false;
    }
    return aborted;
}

// 0x661D30
void CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute() {
    auto* prostitute = FindPlayerPed()->GetPlayerData()->m_pCurrentProstitutePed;
    if (!prostitute)
        return;

    const auto intel = prostitute->GetIntelligence();
    auto* task = intel->FindTaskByType(TASK_COMPLEX_PROSTITUTE_SOLICIT);
    if (!task)
        return;

    auto* t = static_cast<CTaskComplexProstituteSolicit*>(task);
    t->bTaskCanBeFinished = true;
    t->m_nCurrentTimer = 0;
}

// 0x666360
CTask* CTaskComplexProstituteSolicit::CreateSubTask(eTaskType taskType, CPed* prostitute) {
    switch (taskType) {
    case TASK_COMPLEX_CAR_DRIVE:
        bSearchingForSecludedPlace = true;
        bSexProcessStarted = false;
        return new CTaskComplexCarDrive{m_pClient->m_pVehicle};
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{5000, false, false, 8.f};
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        return new CTaskComplexEnterCarAsPassenger{m_pClient->m_pVehicle, 8, false};
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{m_pClient->m_pVehicle, 0, 0, true, false};
    case TASK_COMPLEX_SEEK_ENTITY: {
        auto* vehicle = m_pClient->m_pVehicle;
        CMatrix inverse;
        Invert(*vehicle->m_matrix, inverse);
        const auto left = CCarEnterExit::GetPositionToOpenCarDoor(vehicle, 10);
        const auto right = CCarEnterExit::GetPositionToOpenCarDoor(vehicle, 8);
        const auto& position = prostitute->GetPosition();
        const auto leftDistance = (position - left).SquaredMagnitude();
        const auto rightDistance = (position - right).SquaredMagnitude();
        // Como el original: el empate y unordered escogen la puerta derecha.
        const auto offset = inverse.TransformPoint(rightDistance > leftDistance ? left : right);
        auto* seek = new CTaskComplexSeekEntityXYOffset{vehicle, 50000, 1000, 1.f, 2.f, 2.f, false, false};
        seek->GetSeekPosCalculator().SetOffset(offset);
        return seek;
    }
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
        return new CTaskComplexTurnToFaceEntityOrCoord{m_pClient, 0.5f, 0.2f};
    default:
        return nullptr;
    }
}

// 0x661BB0
bool CTaskComplexProstituteSolicit::IsTaskValid(CPed* prostitute, CPed* ped) {
    if (FindPlayerPed() != ped)
        return false;

    if (!ped)
        return false;

    if (!ped->IsInVehicle())
        return false;

    if (ped->bIsBeingArrested)
        return false;

    if (ped->GetPlayerData()->m_pCurrentProstitutePed && ped->GetPlayerData()->m_pCurrentProstitutePed != prostitute)
        return false;

    if (ped->m_pVehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_AUTOMOBILE)
        return false;

    if (ped->m_pVehicle->IsUpsideDown())
        return false;

    if (ped->m_pVehicle->IsOnItsSide())
        return false;

    auto task = ped->GetTaskManager().GetSimplestActiveTask();
    if (task->GetTaskType() != TASK_SIMPLE_CAR_DRIVE)
        return false;

    if (ped->m_pVehicle->m_pDriver != ped)
        return false;

    if (prostitute->m_pVehicle) {
        if (prostitute->m_pVehicle != ped->m_pVehicle || prostitute->m_pVehicle->m_nNumPassengers != 1)
            return false;
    } else if (ped->m_pVehicle->m_nNumPassengers) {
        return false;
    }

    if (!ped->m_pVehicle->m_nMaxPassengers || ped->m_pVehicle->m_pHandlingData->m_bTandemSeats)
        return false;

    CVector out = ped->GetPosition() - prostitute->GetPosition();
    if (out.SquaredMagnitude() > 100.0f || CTheScripts::IsPlayerOnAMission() || CGameLogic::IsCoopGameGoingOn()) {
        return false;
    }
    return true;
}

// 0x6666A0
CTask* CTaskComplexProstituteSolicit::CreateFirstSubTask(CPed* ped) {
    if (!IsTaskValid(ped, m_pClient)) {
        bTaskCanBeFinished = true;
        return nullptr;
    }

    m_vecVehiclePosn = m_pClient->m_pVehicle->GetPosition();

    auto player = FindPlayerPed();
    m_pClient->GetPlayerData()->m_pCurrentProstitutePed = ped;
    player->GetPlayerData()->m_pCurrentProstitutePed->RegisterReference(player->GetPlayerData()->m_pCurrentProstitutePed);

    if (m_pClient->GetPlayerData()->m_pLastProstituteShagged != ped) {
        CEntity::SafeCleanUpRef(m_pClient->GetPlayerData()->m_pLastProstituteShagged);
        m_pClient->GetPlayerData()->m_pLastProstituteShagged = ped;
        m_pClient->GetPlayerData()->m_pLastProstituteShagged->RegisterReference(m_pClient->GetPlayerData()->m_pLastProstituteShagged);
    }

    return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x666780
CTask* CTaskComplexProstituteSolicit::CreateNextSubTask(CPed* ped) {
    if (!m_pClient) {
        return nullptr;
    }
    if (!IsTaskValid(ped, m_pClient)) {
        bTaskCanBeFinished = true;
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
        ped->Say(CTX_GLOBAL_SOLICIT);
        CMessages::AddMessageQ(TheText.Get("PROS_04"), 5000, 1, true);
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    case TASK_COMPLEX_SEEK_ENTITY:
        g_ikChainMan.LookAt("TaskProzzy", ped, m_pClient, 5000, BONE_UNKNOWN, nullptr, false, 0.25f, 500, 3, false);
        return CreateSubTask(TASK_COMPLEX_TURN_TO_FACE_ENTITY, ped);
    case TASK_COMPLEX_CAR_DRIVE:
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    case TASK_SIMPLE_STAND_STILL:
        if (!bPlayerHasAcceptedSexProposition) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        if (!CCheat::IsActive(CHEAT_PROSTITUTES_PAY_YOU) && FindPlayerPed()->GetPlayerInfoForThisPlayerPed()->m_nMoney < 20) {
            CMessages::ClearMessages(false);
            CMessages::AddMessageQ(TheText.Get("PROS_06"), 2000, 1, true);
            CMessages::AddMessageQ(TheText.Get("PROS_09"), 3000, 1, true);
            return CreateSubTask(TASK_FINISHED, ped);
        }
        return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, ped);
    case TASK_COMPLEX_LEAVE_CAR:
        g_ikChainMan.LookAt("TaskProzzy", ped, m_pClient, 2500, BONE_UNKNOWN, nullptr, false, 0.25f, 500, 3, false);
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        ped->Say(CTX_GLOBAL_SOLICIT_THANKS);
        m_vecVehiclePosn = m_pClient->m_pVehicle->GetPosition();
        return CreateSubTask(TASK_COMPLEX_CAR_DRIVE, ped);
    default:
        return nullptr;
    }
}

// 0x6669D0
CTask* CTaskComplexProstituteSolicit::ControlSubTask(CPed* ped) {
    bMoveCameraDown = bSexProcessStarted;
    if (!IsTaskValid(ped, m_pClient)) {
        bMoveCameraDown = false;
        bTaskCanBeFinished = true;
    }
    const auto step = int32(CTimer::GetTimeStep() * 0.02f * 1000.0f);
    if (bTaskCanBeFinished) {
        if (m_nCurrentTimer) {
            m_nCurrentTimer = int16(m_nCurrentTimer - step);
            if (m_nCurrentTimer <= 0) {
                CAEPedSpeechAudioEntity::SetCJMood(eCJMood(9), 120000, -1, -1, -1);
                m_pClient->Say(eGlobalSpeechContext(11));
                m_nCurrentTimer = 0;
            }
        } else if (m_pSubTask->GetTaskType() != TASK_COMPLEX_LEAVE_CAR && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }
        return m_pSubTask;
    }
    auto* pad = CPad::GetPad(0);
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        if (pad->ConversationYesJustDown()) {
            bPlayerHasAcceptedSexProposition = true;
            if (m_pClient) m_pClient->Say(eGlobalSpeechContext(196));
            return CreateNextSubTask(ped);
        }
        if (pad->ConversationNoJustDown()) {
            if (m_pClient) m_pClient->Say(eGlobalSpeechContext(195));
            bTaskCanBeFinished = true;
        }
        return m_pSubTask;
    }
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_SEEK_ENTITY || m_pSubTask->GetTaskType() == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER) {
        const auto d = m_pClient->m_pVehicle->GetPosition() - m_vecVehiclePosn;
        // The EXE sums x*x + z*z + y*y here.
        if (d.x * d.x + d.z * d.z + d.y * d.y > 16.0f) bTaskCanBeFinished = true;
        return m_pSubTask;
    }
    if (m_pSubTask->GetTaskType() != TASK_COMPLEX_CAR_DRIVE) return m_pSubTask;
    const auto now = CTimer::GetTimeInMS();
    if (StaticRef<int32>(0xB6F0D8) == 1) {
        m_nLastSavedTime = int32(now);
        m_nCurrentTimer = 8000;
        bMoveCameraDown = false;
        b08 = true;
        bVehicleShifted = true;
        return m_pSubTask;
    }
    const auto speed = m_pClient->m_pVehicle->m_vecMoveSpeed * 50.0f;
    const bool stationary = speed.SquaredMagnitude() < 0.5625f;
    if (!stationary || b08) {
        b08 = false;
        m_nLastSavedTime = int32(now);
    }
    auto* wanted = FindPlayerWanted();
    if (now > uint32(m_nNextTimeToCheckForSecludedPlace)) {
        bPedsCanPotentiallySeeThis = bPedsCanSeeThis = bCopsCanSeeThis = false;
        m_nNextTimeToCheckForSecludedPlace = int32(now + 1000);
        auto** entities = ped->GetIntelligence()->GetPedEntities();
        for (int i = 0; i < 16; ++i) {
            auto* observer = static_cast<CPed*>(entities[i]);
            if (!observer || observer == m_pClient || observer->m_nPedType == PED_TYPE_PROSTITUTE) continue;
            const auto distance = (observer->GetPosition() - ped->GetPosition()).SquaredMagnitude();
            if (distance < 56.25f) bPedsCanSeeThis = true;
            if (distance < 400.0f) bPedsCanPotentiallySeeThis = true;
            if (observer->m_nPedType == PED_TYPE_COP && bSexProcessStarted && wanted && int(wanted->m_WantedLevel) < 1) {
                auto* veh = m_pClient->m_pVehicle;
                const auto collision = veh->GetUsesCollision();
                veh->SetUsesCollision(false);
                const auto clear = CWorld::GetIsLineOfSightClear(observer->GetPosition(), m_pClient->GetPosition(), true, true, false, true, false, true, false);
                m_pClient->m_pVehicle->SetUsesCollision(collision);
                if (clear) bCopsCanSeeThis = true;
            }
        }
    }
    const auto message = [](const char* key, uint32 duration) { CMessages::AddMessageQ(TheText.Get(key), duration, 1, true); };
    if (bSearchingForSecludedPlace) {
        if (stationary && now - uint32(m_nLastSavedTime) > 4000) {
            if (!bPedsCanPotentiallySeeThis) {
                bSearchingForSecludedPlace = false;
                bSexProcessStarted = true;
                m_nLastPaymentTime = int32(now);
                message("PROS_02", 2000);
            } else if (!bSecludedPlaceMessageShown) {
                message("PROS_01", 3000);
                bSecludedPlaceMessageShown = true;
            }
        }
        return m_pSubTask;
    }
    if (!bSexProcessStarted) return m_pSubTask;
    if (b07) {
        b07 = false;
        m_nCurrentTimer = 15000;
        CStats::IncrementStat(eStats(190), 1.0f);
    }
    const bool movingControls = pad->GetAccelerate() || pad->GetBrake();
    const auto random = plugin::CallAndReturn<int32, 0x821B1E>();
    bool discovered = false;
    if (bCopsCanSeeThis && wanted && int(wanted->m_WantedLevel) < 1) {
        FindPlayerWanted()->SetWantedLevel(eWantedLevel(1));
        discovered = true;
    }
    if (movingControls || bPedsCanSeeThis || discovered) {
        m_nLastSavedTime = int32(now);
        bSexProcessStarted = false;
        bSearchingForSecludedPlace = true;
        bVehicleShifted = true;
        if (movingControls) {
            if (uint32(random) < 0x1FFFFFFF || m_nCurrentTimer < 3000) {
                bTaskCanBeFinished = true;
                m_nCurrentTimer = 0;
                message("PROS_09", 3000);
                return m_pSubTask;
            }
        } else if (m_nCurrentTimer < 3000) {
            bTaskCanBeFinished = true;
            m_nCurrentTimer = 0;
            return m_pSubTask;
        } else {
            message("PROS_01", 3000);
        }
        m_nCurrentTimer = 8000;
        return m_pSubTask;
    }
    m_nVehicleMovementTimer = int16(m_nVehicleMovementTimer - int16(step));
    if (m_nVehicleMovementTimer <= 0) {
        auto factor = CGeneral::GetRandomNumberInRange(-0.5f, -0.9f);
        if (m_nCurrentTimer > 10000) m_nVehicleMovementTimer = 850;
        else if (m_nCurrentTimer > 5000) m_nVehicleMovementTimer = 450;
        else if (m_nCurrentTimer > 1000) m_nVehicleMovementTimer = 120;
        else {
            factor *= 0.5f;
            m_nVehicleMovementTimer = 850;
            CPad::GetPad(0)->StartShake(1000, 120, 0);
        }
        auto* veh = m_pClient->m_pVehicle;
        const auto carPos = veh->GetPosition();
        const auto clientPos = m_pClient->GetPosition();
        const auto position = (random & 1) ? clientPos : ped->GetPosition();
        const auto scaledMass = veh->m_fMass * (1.f / 15.f);
        const auto minMass = 150.f < scaledMass ? 150.f : scaledMass;
        const auto mass = minMass > m_pClient->m_fMass ? minMass : m_pClient->m_fMass;
        // Original force helper (0x65DA70), separate from this task's catalog.
        plugin::CallMethod<0x65DA70>(veh, 0.f, 0.f, mass * factor, position.x - carPos.x, position.y - carPos.y, 0.f);
        veh->m_vehicleAudio.AddAudioEvent(eAudioEvents(108), 0.f);
        if (b10 && uint32(random) > 0xFFFFFFF) {
            ped->Say(eGlobalSpeechContext((random & 0xFFFF) < 0xFF ? 112 : 116), 0, 0.5f);
        }
    }
    m_nCurrentTimer = int16(m_nCurrentTimer - int16(step));
    if (m_nCurrentTimer <= 0) {
        m_nCurrentTimer = 3000;
        bSexProcessStarted = false;
        bTaskCanBeFinished = true;
    }
    if (now - uint32(m_nLastPaymentTime) <= 1000) return m_pSubTask;
    m_nLastPaymentTime = int32(now);
    auto* info = m_pClient->AsPlayer()->GetPlayerInfoForThisPlayerPed();
    if (CCheat::IsActive(CHEAT_PROSTITUTES_PAY_YOU)) {
        info->m_nMoney += 2;
    } else if (info->m_nMoney >= 2) {
        info->m_nMoney -= 2;
        CStats::IncrementStat(eStats(33), 2.f);
        ++*reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(ped) + 0x756);
    } else {
        info->m_nMoney = 0;
        m_nCurrentTimer = 0;
        bSexProcessStarted = false;
        bTaskCanBeFinished = true;
        CMessages::ClearMessages(false);
        message("PROS_06", 2000);
        message("PROS_09", 3000);
    }
    if (!bVehicleShifted) info->AddHealth(2);
    return m_pSubTask;
}
