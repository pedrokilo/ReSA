/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarDrive
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"
#include "TaskSimpleCarDrive.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskComplexSequence.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexDriveWander.h"
#include "CarEnterExit.h"
#include "Door.h"
#include "Cheat.h"
#include "Camera.h"
#include "Weather.h"
#include "GameLogic.h"
#include "PedGroups.h"
#include "RwHelper.h"
#include "VehicleAnimGroupData.h"
#include "Audio/AudioEngine.h"
#include "Scripts/TheScripts.h"
#include "Events/EventCarUpsideDown.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventGroup.h"
#include "Events/EventGlobalGroup.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "ModelIndices.h"

void CTaskSimpleCarDrive::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarDrive, 0x86E904, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63C340);
    RH_ScopedInstall(Destructor, 0x63C460);

    RH_ScopedInstall(TriggerIK, 0x63C500);
    RH_ScopedInstall(UpdateBopping, 0x63C900);
    RH_ScopedInstall(StartBopping, 0x642760);
    RH_ScopedInstall(ProcessHeadBopping, 0x6428C0);
    RH_ScopedInstall(ProcessArmBopping, 0x642AE0);
    RH_ScopedInstall(ProcessBopping, 0x642E70);
    RH_ScopedVMTInstall(Clone, 0x63DC20);
    RH_ScopedVMTInstall(GetTaskType, 0x63C450);
    RH_ScopedVMTInstall(MakeAbortable, 0x63C670);
    RH_ScopedVMTInstall(ProcessPed, 0x644470);
    RH_ScopedVMTInstall(SetPedPosition, 0x63C770);
}

// 0x63C340
CTaskSimpleCarDrive::CTaskSimpleCarDrive(CVehicle* vehicle, CTaskUtilityLineUpPedWithCar* utilityTask, bool updateCurrentVehicle) : CTaskSimple() {
    m_pVehicle = vehicle;
    m_pAnimCloseDoorRolling = nullptr;
    m_pTaskUtilityLineUpPedWithCar = nullptr;
    // Como el original: el temporizador de espera (0x14..0x1D) queda a cero con los valores por defecto de CTaskTimer.
    m_nTimePassedSinceCarUpSideDown = 0;

    // Como el original (0x63C38B..0x63C41B): el byte de banderas deja a cero las banderas 01, 02, 10 y 20.
    m_b01 = false;
    m_b02 = false;
    m_bUpdateCurrentVehicle = updateCurrentVehicle;
    m_b08 = true;
    m_b10 = false;
    m_b20 = false;

    CEntity::SafeRegisterRef(m_pVehicle);

    if (utilityTask) {
        m_pTaskUtilityLineUpPedWithCar = new CTaskUtilityLineUpPedWithCar(CVector{}, 0, utilityTask->m_nDoorOpenPosType, utilityTask->m_nDoorIdx);
    }

    m_fHeadBoppingFactor = 0.0f;
    m_fHeadBoppingOrientation = 0.0f;
    m_fRandomHeadBoppingMultiplier = 0.0f;
    m_nBoppingStartTime = -1;
}

// 0x63C460
CTaskSimpleCarDrive::~CTaskSimpleCarDrive() {
    CEntity::SafeCleanUpRef(m_pVehicle);

    if (m_pTaskUtilityLineUpPedWithCar) {
        delete m_pTaskUtilityLineUpPedWithCar;
        m_pTaskUtilityLineUpPedWithCar = nullptr;
    }

    if (m_b20) {
        // Como el original (0x63C4BB): sin guarda de nulo sobre la animación (m_pAnimCloseDoorRolling).
        m_pAnimCloseDoorRolling->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        if (m_pVehicle) {
            m_pVehicle->ClearGettingOutFlags(1);
        }
    }
}

// 0x63C500
void CTaskSimpleCarDrive::TriggerIK(CPed* ped) const {
    if (!m_pVehicle) {
        return;
    }

    switch (m_pVehicle->m_autoPilot.m_nCarMission) {
    case MISSION_RAMPLAYER_FARAWAY:
    case MISSION_RAMPLAYER_CLOSE:
    case MISSION_BLOCKPLAYER_FARAWAY:
    case MISSION_BLOCKPLAYER_CLOSE:
    case MISSION_BLOCKPLAYER_HANDBRAKESTOP: { // Make ped look at player ped
        if (g_ikChainMan.IsLooking(ped)) {
            return;
        }
        // Como el original: rand() del EXE directo, (rand & 0xFFFF) * (1/32768) * 100 truncado (no la plantilla entera)
        if ((int32)((float)(int32)CGeneral::GetRandomNumber() * 3.0517578125e-05f * 100.0f) >= 5) {
            return;
        }
        g_ikChainMan.LookAt("DriveCar", ped, FindPlayerPed(0), 3000, BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
        break;
    }
    case MISSION_RAMCAR_FARAWAY:
    case MISSION_RAMCAR_CLOSE: {
        if (g_ikChainMan.IsLooking(ped)) {
            return;
        }
        if (CGeneral::GetRandomIntInRange(0, 100) >= 5) { // 0x407180
            return;
        }
        if (const auto vehTargetCar = m_pVehicle->m_autoPilot.m_TargetEntity) {
            if (vehTargetCar->GetIsTypeVehicle()) {
                if (const auto driver = vehTargetCar->m_pDriver) { // Make ped look at target car or it's driver (if any)
                    g_ikChainMan.LookAt("DriveCar", ped, driver, 3000, BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
                } else {
                    g_ikChainMan.LookAt("DriveCar", ped, vehTargetCar, 3000, BONE_UNKNOWN, nullptr, false, 0.25f, 500, 3, false);
                }
            }
        }
        break;
    }
    }
}

// 0x63C900
void CTaskSimpleCarDrive::UpdateBopping() {
    const auto timeDelta = (int32)CTimer::GetTimeInMS() - m_nBoppingStartTime;
    // Como el original: fidiv (el divisor entero entra exacto en el x87, un solo redondeo a float)
    m_fBoppingProgress = (float)((double)(timeDelta % m_nBoppingEndTime) / (double)m_nBoppingEndTime);
    m_nBoppingCompletedTimes = timeDelta / m_nBoppingEndTime % 2;
}

// Como el original: fsin del x87 con precisión interna completa sobre un float ya redondeado.
static float X87Sin(float angle) {
    return TrigSimpleSinCpp(angle); // X87Math.cpp
}

// Como el original: ejes pasados a RtQuatRotate (0x86EBD4 = X, 0x86EBC8/0x86EBE0 = Z).
static const RwV3d s_AxisX{ 1.0f, 0.0f, 0.0f };
static const RwV3d s_AxisZ{ 0.0f, 0.0f, 1.0f };

// 0x642700 (sin catalogar): empieza la animación de cerrar la puerta rodando
void CTaskSimpleCarDrive::StartRollingDoor(CPed* ped) {
    const auto group = CVehicleAnimGroupData::GetVehicleAnimGroup(ped->m_pVehicle->m_pHandlingData->m_nAnimGroup).GetGroup(ANIM_ID_CAR_ROLLDOOR);
    m_pAnimCloseDoorRolling = CAnimManager::AddAnimation(ped->GetRpClump(), group, ANIM_ID_CAR_ROLLDOOR);
    // Como el original: el callback de fin es la función del EXE 0x63C840 (sin catalogar)
    m_pAnimCloseDoorRolling->SetFinishCallback(reinterpret_cast<void(*)(CAnimBlendAssociation*, void*)>(0x63C840), this);
}

// 0x642760
void CTaskSimpleCarDrive::StartBopping(CPed* ped) {
    int32 start = -1;
    int32 end   = 0; // Como el original: solo se lee si se ha encontrado una tarea de conducción con inicio

    if (const auto driver = ped->m_pVehicle->m_pDriver; driver && driver != ped) {
        if (const auto task = driver->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_SIMPLE_CAR_DRIVE)) {
            const auto drive = static_cast<CTaskSimpleCarDrive*>(task);
            start = drive->m_nBoppingStartTime;
            end   = drive->m_nBoppingEndTime;
        }
    }

    if (start == -1) {
        // Como el original: se prueban los tres primeros pasajeros (offsets 0x464, 0x468 y 0x46C del vehículo)
        for (int32 i = 0; i < 3; ++i) {
            const auto passenger = ped->m_pVehicle->m_apPassengers.data()[i];
            if (!passenger || passenger == ped) {
                continue;
            }
            if (const auto task = passenger->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_SIMPLE_CAR_DRIVE)) {
                const auto drive = static_cast<CTaskSimpleCarDrive*>(task);
                end   = drive->m_nBoppingEndTime;
                start = drive->m_nBoppingStartTime;
            }
            if (start != -1) {
                break;
            }
        }
    }

    if (start == -1) {
        // Ninguno conduce: periodo aleatorio (CRT rand del EXE)
        m_nBoppingStartTime = CTimer::GetTimeInMS();
        const float r0 = (float)(int32)CGeneral::GetRandomNumber() * 3.0517578125e-05f;
        const int32 a  = (int32)(r0 * -60.0f);
        const float g  = (float)(60 - a) * 0.016666667f;
        m_nBoppingEndTime = (int32)((1.0f / g) * 1000.0f);
        m_fBoppingProgress = 0.0f;
        return;
    }

    m_nBoppingStartTime = start;
    m_nBoppingEndTime   = end;
    const int32 delta = (int32)CTimer::GetTimeInMS() - start;
    const int32 quot  = delta / end;
    const int32 rem   = delta % end;
    m_fBoppingProgress = (float)((double)rem / (double)end);
    m_nBoppingCompletedTimes = quot % 2;
}

// 0x6428C0
void CTaskSimpleCarDrive::ProcessHeadBopping(CPed* ped, bool a3, float a4) {
    float sinValue = 0.0f;

    if (a3) {
        if (!m_b01) {
            if (!(m_fHeadBoppingFactor == 0.0f) || CGeneral::GetRandomIntInRange(0, 1000) <= 995) {
                a3 = false;
            } else {
                if (m_nBoppingStartTime == -1) {
                    StartBopping(ped);
                    m_nHeadBoppingStartTime = m_nBoppingStartTime;
                } else {
                    m_nHeadBoppingStartTime = (int32)CTimer::GetTimeInMS();
                }
                m_nHeadBoppingDirection = CGeneral::GetRandomIntInRange(1, 3);
                m_fRandomHeadBoppingMultiplier = CGeneral::GetRandomNumberInRange(3.0f, 8.0f);
                m_fHeadBoppingFactor = 0.0f;
                m_fHeadBoppingOrientation = 0.0f;
                sinValue = 0.0f;
            }
        } else {
            sinValue = X87Sin(m_fBoppingProgress * 3.1415927f);
            if (CTimer::GetTimeInMS() - (uint32)m_nHeadBoppingStartTime > 5000u && CGeneral::GetRandomIntInRange(0, 1000) > 995) {
                a3 = false;
            }
        }
    }

    m_b01 = a3;

    float delta;
    if (m_b01) {
        if (m_fHeadBoppingFactor < 1.0f) {
            m_fHeadBoppingFactor = m_fHeadBoppingFactor + 0.05f;
        }
        if (m_fHeadBoppingFactor > 1.0f) {
            m_fHeadBoppingFactor = 1.0f;
        }
        delta = (sinValue - m_fHeadBoppingOrientation) * m_fHeadBoppingFactor;
    } else {
        if (m_fHeadBoppingFactor > 0.0f) {
            m_fHeadBoppingFactor = m_fHeadBoppingFactor - 0.05f;
        }
        if (m_fHeadBoppingFactor < 0.0f) {
            m_fHeadBoppingFactor = 0.0f;
        }
        delta = (sinValue - m_fHeadBoppingOrientation) * (1.0f - m_fHeadBoppingFactor);
    }
    m_fHeadBoppingOrientation = delta + m_fHeadBoppingOrientation;

    if (a4 < 64.0f && m_fHeadBoppingOrientation > 0.0f) {
        const float angle = m_fRandomHeadBoppingMultiplier * m_fHeadBoppingOrientation;
        const auto quat = &ped->m_apBones[PED_NODE_HEAD]->KeyFrame->q;
        if (m_nHeadBoppingDirection >= 1) {
            RtQuatRotate(quat, &s_AxisX, m_nBoppingCompletedTimes == 0 ? angle : -angle, rwCOMBINEPRECONCAT);
        }
        RtQuatRotate(quat, &s_AxisZ, m_nHeadBoppingDirection == 2 ? angle : -angle, rwCOMBINEPRECONCAT);
        ped->bUpdateMatricesRequired = true;
    }
}

// 0x642AE0
void CTaskSimpleCarDrive::ProcessArmBopping(CPed* ped, bool a3, float a4) {
    if (ped->IsPlayer()) {
        return;
    }

    // Como el original: con lluvia se cierran las ventanillas; si no, se abre la del asiento del peatón.
    if (CWeather::Rain > 0.0f) {
        ped->m_pVehicle->ClearWindowOpenFlag(10);
        ped->m_pVehicle->ClearWindowOpenFlag(8);
        ped->m_pVehicle->ClearWindowOpenFlag(11);
        ped->m_pVehicle->ClearWindowOpenFlag(9);
        a3 = false;
    } else {
        const auto veh = ped->m_pVehicle;
        if (veh->m_pDriver == ped) {
            veh->SetWindowOpenFlag(10);
        } else if (veh->m_apPassengers.data()[0] == ped) {
            veh->SetWindowOpenFlag(8);
        } else if (veh->m_apPassengers.data()[1] == ped) {
            veh->SetWindowOpenFlag(11);
        } else if (veh->m_apPassengers.data()[2] == ped) {
            veh->SetWindowOpenFlag(9);
        }
    }

    AnimationId animId = ANIM_ID_TAP_HAND;
    {
        const auto veh = ped->m_pVehicle;
        if (veh->m_apPassengers.data()[0] == ped || veh->m_apPassengers.data()[2] == ped) {
            animId = ANIM_ID_TAP_HANDP;
        }
    }
    const auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), (uint32)animId);

    if (!a3) {
        if (assoc) {
            assoc->m_BlendDelta = -4.0f;
        }
        m_b02 = false;
        return;
    }

    if (!m_b02) {
        if (CGeneral::GetRandomIntInRange(0, 1000) <= 995) {
            return;
        }

        bool canStart = true;
        if (m_pVehicle->m_nVehicleSubType == VEHICLE_TYPE_AUTOMOBILE) {
            const int32 door = CCarEnterExit::ComputeTargetDoorToExit(m_pVehicle, ped);
            // Como el original: el estado de la puerta se lee como dword con signo (fuera de 0..5 devuelve 4)
            const uint32 doorStatus = door >= 0 && door < 6
                ? (uint32)m_pVehicle->AsAutomobile()->m_damageManager.GetDoorStatus((eDoors)door)
                : 4u;
            if (doorStatus > 0
                || CCarEnterExit::CarHasDoorToClose(m_pVehicle, door)
                || !m_pVehicle->CanPedLeanOut(ped)
            ) {
                canStart = false;
            }

            const auto vehStruct = CModelInfo::ms_modelInfoPtrs[(int16)m_pVehicle->m_nModelIndex]->AsVehicleModelInfoPtr()->m_pVehicleStruct;
            const CVector handRest = vehStruct->m_avDummyPos[DUMMY_HAND_REST];
            if (handRest.x == 0.0f && handRest.y == 0.0f && handRest.z == 0.0f) {
                return;
            }
            const CVector seatFront = vehStruct->m_avDummyPos[DUMMY_SEAT_FRONT];
            const float dz = handRest.z - seatFront.z;
            if (m_pVehicle->vehicleFlags.bLowVehicle) {
                return;
            }
            const auto& animGroup = CVehicleAnimGroupData::GetVehicleAnimGroup(m_pVehicle->m_pHandlingData->m_nAnimGroup);
            if (animGroup.m_specialFlags.bUseTruckDriveAnims) {
                if (dz < 0.4f) {
                    return;
                }
                if (dz > 0.44f) {
                    return;
                }
            } else {
                if (dz < 0.39f) {
                    return;
                }
                if (dz > 0.46f) {
                    return;
                }
            }
            if (!canStart) {
                return;
            }
        }

        CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, animId, 4.0f);
        if (m_nBoppingStartTime == -1) {
            StartBopping(ped);
            m_nArmBoppingStartTime = m_nBoppingStartTime;
        } else {
            m_nArmBoppingStartTime = (int32)CTimer::GetTimeInMS();
        }
        m_b02 = true;
        return;
    }

    if (!assoc) {
        m_b02 = false;
        return;
    }

    if (a4 < 64.0f) {
        float angle = 0.0f;
        if (m_fBoppingProgress >= 0.75f) {
            angle = (m_fBoppingProgress - 0.75f) * 45.0f;
        }
        const auto bone = animId == ANIM_ID_TAP_HAND ? ped->m_apBones[PED_NODE_LEFT_HAND] : ped->m_apBones[PED_NODE_RIGHT_HAND];
        RtQuatRotate(&bone->KeyFrame->q, &s_AxisZ, -angle, rwCOMBINEPRECONCAT);
        ped->bUpdateMatricesRequired = true;
    }

    if (CTimer::GetTimeInMS() - (uint32)m_nArmBoppingStartTime > 5000u && CGeneral::GetRandomIntInRange(0, 1000) > 995) {
        assoc->m_BlendDelta = -4.0f;
        m_b02 = false;
    }
}

// 0x642E70
void CTaskSimpleCarDrive::ProcessBopping(CPed* ped, bool a3) {
    if (ped->m_pVehicle->m_pDriver == FindPlayerPed(0)
        || ped->m_nPedType == PED_TYPE_COP
        || ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP)
    ) {
        return;
    }

    auto* vehicle = ped->m_pVehicle;
    if (vehicle->IsAutomobile() && !vehicle->IsSubQuad() && !ped->IsCreatedByMission()) {
        if (m_nBoppingStartTime != -1) { // IsBopping
            UpdateBopping();
        }

        const auto dist = DistanceBetweenPointsSquared(TheCamera.GetPosition(), ped->GetPosition());
        ProcessHeadBopping(ped, a3, dist);
        ProcessArmBopping(ped, a3, dist);
        if (m_nBoppingStartTime != -1 && !m_b01 && !m_b02) {
            m_nBoppingStartTime = -1;
        }
    }
}

// 0x63DC20


CTask* CTaskSimpleCarDrive::Clone() const {
    auto task = new CTaskSimpleCarDrive(m_pVehicle);
    task->m_bUpdateCurrentVehicle = m_bUpdateCurrentVehicle;
    return task;
}

// 0x63C670


bool CTaskSimpleCarDrive::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        // Como el original: si el peatón va en un vehículo, lo saca con una tarea de salida creada en la pila.
        if (ped->bInVehicle && ped->m_pVehicle) {
            CTaskSimpleCarSetPedOut task{
                m_pVehicle,
                static_cast<eTargetDoor>(CCarEnterExit::ComputeTargetDoorToExit(ped->m_pVehicle, ped)),
                false
            };
            if (ped->m_pVehicle->m_nVehicleType == VEHICLE_TYPE_BIKE && event) {
                task.m_bFallingOutOfCar = true;
            }
            task.ProcessPed(ped);
        }
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped, 250);
        }
        return true;
    }
    m_b08 = true;
    return !m_b20;
}

// 0x644470


bool CTaskSimpleCarDrive::ProcessPed(CPed* ped) {
    const bool retuneStarted = AudioEngine.HasRadioRetuneJustStarted();

    // Como el original: con m_bUpdateCurrentVehicle se adopta el vehículo actual del peatón.
    if (m_bUpdateCurrentVehicle && ped->bInVehicle && ped->m_pVehicle && ped->m_pVehicle != m_pVehicle) {
        CEntity::SafeCleanUpRef(m_pVehicle);
        m_pVehicle = ped->m_pVehicle;
        CEntity::SafeRegisterRef(m_pVehicle);
    }

    if (!m_pVehicle || !ped->bInVehicle) {
        ped->bInVehicle = false;
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped, 250);
        }
        return true;
    }

    const auto group = CPedGroups::GetPedsGroup(ped);

    if (CTheScripts::UpsideDownCars.IsCarUpsideDown(m_pVehicle)) {
        // Como el original: se llama a FindPlayerPed(-1) hasta tres veces según las condiciones.
        if (ped != FindPlayerPed(-1) && FindPlayerPed(-1)->bInVehicle) {
            if (FindPlayerPed(-1)->m_pVehicle == m_pVehicle) {
                ped->Say(static_cast<eGlobalSpeechContext>(0x22), 0, 1.0f, false, false, false);
            }
        }
        if (!group || group->GetMembership().IsLeader(ped)) {
            // Como el original: CTimer::ms_fTimeStep * 0.02 * 1000 truncado (no el GetTimeStepInMS inline)
            m_nTimePassedSinceCarUpSideDown += (int32)(CTimer::ms_fTimeStep * 0.02f * 1000.0f);
            if ((uint32)m_nTimePassedSinceCarUpSideDown > 2000u) {
                CEventCarUpsideDown event{ m_pVehicle };
                ped->GetEventGroup().Add(&event, false);
            }
        }
    } else {
        m_nTimePassedSinceCarUpSideDown = 0;
    }

    if (ped->IsPlayer() && m_pVehicle->IsLawEnforcementVehicle()) {
        if (!m_copCarStolenTimer.m_bStarted || m_copCarStolenTimer.IsOutOfTime()) {
            m_copCarStolenTimer.Start(2000);
            CEventCopCarBeingStolen event{ ped, m_pVehicle };
            GetEventGlobalGroup()->Add(&event, false);
        }
    }

    if (m_pVehicle->m_nVehicleType == VEHICLE_TYPE_BIKE || m_pVehicle->m_nVehicleSubType == VEHICLE_TYPE_QUAD || m_pVehicle->m_nModelIndex == 0x213) {
        ped->bTestForShotInVehicle = true;
    }

    if (ped->IsPlayer() && m_pVehicle->m_pDriver == ped) {
        CPad* pad = ped->AsPlayer()->GetPadFromPlayer();
        // Como el original: el pad se lee en el orden acelerar, girar, frenar y se corta en el primero distinto de cero.
        const auto PadIdle = [&] { return pad->GetAccelerate() == 0 && pad->GetSteeringLeftRight() == 0 && pad->GetBrake() == 0; };

        // Como el original: con el truco de apuntar conduciendo y un arma de la ranura 4 con munición, el jugador hace un drive-by.
        if (CCheat::IsActive(CHEAT_WEAPON_AIMING_WHILE_DRIVING)
            && ped->m_aWeapons[4].GetType() != WEAPON_UNARMED
            && (int32)ped->m_aWeapons[4].GetTotalAmmo() > 0
            && m_pVehicle->CanPedLeanOut(ped)
        ) {
            const auto driveBy = new CTaskSimpleGangDriveBy(nullptr, nullptr, 100.0f, 100, static_cast<eDrivebyStyle>(8), false);
            CEventScriptCommand event{ 3, driveBy, false };
            ped->GetEventGroup().Add(&event, false);
        }

        // Como el original: la puerta del tren se maneja mirando la puerta en +0x608 (CTrain::m_aDoors[0]).
        if (ped->m_pVehicle->m_nModelIndex != 0x23A
            && ped->m_pVehicle->m_nVehicleType == VEHICLE_TYPE_TRAIN
            && !reinterpret_cast<CDoor*>(reinterpret_cast<uint8*>(ped->m_pVehicle) + 0x608)->IsClosed()
        ) {
            if (!pad || PadIdle()) {
                ped->m_pVehicle->ProcessOpenDoor(
                    ped,
                    10,
                    CVehicleAnimGroupData::GetGroupForAnim(static_cast<AssocGroupId>(m_pVehicle->m_pHandlingData->m_nAnimGroup), ANIM_ID_CAR_ROLLDOOR),
                    ANIM_ID_CAR_ROLLDOOR,
                    1.0f
                );
            }
        } else if (ped->m_pVehicle->m_nVehicleType != VEHICLE_TYPE_AUTOMOBILE
            || ped->m_pVehicle->AsAutomobile()->m_damageManager.GetDoorStatus(DOOR_LEFT_FRONT) != DAMSTATE_OPENED
        ) {
            m_pVehicle->ProcessDrivingAnims(ped, retuneStarted);
            ProcessBopping(ped, true);
        } else if (!ped->m_pVehicle->IsDoorMissing(DOOR_LEFT_FRONT)) {
            const auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), (uint32)ANIM_ID_CAR_ROLLDOOR);

            // Como el original: la puerta sigue la animación de rodar que ya estaba activa.
            const auto FollowRollAnim = [&] {
                const float time = assoc->m_CurrentTime;
                ped->m_pVehicle->ProcessOpenDoor(
                    ped,
                    10,
                    CVehicleAnimGroupData::GetGroupForAnim(static_cast<AssocGroupId>(m_pVehicle->m_pHandlingData->m_nAnimGroup), static_cast<AnimationId>(assoc->m_AnimId)),
                    ANIM_ID_CAR_ROLLDOOR,
                    time
                );
            };

            if ((ped->m_pVehicle->m_nGettingOutFlags & 1) || (ped->m_pVehicle->m_nGettingInFlags & 1)) {
                if (assoc) {
                    FollowRollAnim();
                }
                ProcessBopping(ped, false);
            } else if (assoc) {
                FollowRollAnim();
                ProcessBopping(ped, false);
            } else if (pad && !PadIdle()) {
                ProcessBopping(ped, false);
            } else {
                // Sin animación ni mando activo: empieza a cerrar la puerta rodando y la tarea devuelve false.
                ped->m_pVehicle->SetGettingOutFlags(1);
                m_b20 = true;
                StartRollingDoor(ped);
                return false;
            }
        }
    } else if (m_pVehicle->m_pDriver == ped) {
        m_pVehicle->ProcessDrivingAnims(ped, retuneStarted);
        ProcessBopping(ped, true);
    } else if (m_pVehicle->IsPassenger(ped)) {
        if (!(m_b08 || m_b10)) {
            if (m_pVehicle->GetRideAnimData()) {
                CAnimManager::BlendAnimation(ped->GetRpClump(), m_pVehicle->GetRideAnimData()->AnimGroup, ANIM_ID_BIKE_PASSENGER, 8.0f);
            }
            m_b10 = true;
        }
        ProcessBopping(ped, true);

        if (ped->IsPlayer()
            && CGameLogic::IsCoopGameGoingOn()
            && ped->m_aWeapons[4].GetType() != WEAPON_UNARMED
            && (int32)ped->m_aWeapons[4].GetTotalAmmo() > 0
            && m_pVehicle->CanPedLeanOut(ped)
            && TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING
        ) {
            const bool seatRHS = m_pVehicle->m_apPassengers.data()[1] != ped;
            const auto driveBy = new CTaskSimpleGangDriveBy(nullptr, nullptr, 100.0f, 100, static_cast<eDrivebyStyle>(8), seatRHS);
            CEventScriptCommand event{ 3, driveBy, false };
            ped->GetEventGroup().Add(&event, false);
        }
    }

    // Como el original: el cigarrillo (modelo de humo de pandilla) que el peatón sostiene cambia de animación según el asiento.
    if (const auto held = ped->GetEntityThatThisPedIsHolding()) {
        if ((int32)(int16)held->m_nModelIndex == (int32)(uint16)eModelID(ModelIndices::MI_GANG_SMOKE)) {
            if (!g_ikChainMan.IsLooking(ped)) {
                if (CGeneral::GetRandomIntInRange(0, 60) == 15) {
                    if (const auto task = ped->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_SIMPLE_HOLD_ENTITY)) {
                        static_cast<CTaskSimpleHoldEntity*>(task)->PlayAnim(
                            m_pVehicle->IsPassenger(ped) ? ANIM_ID_PASS_SMOKE_IN_CAR : ANIM_ID_SMOKE_IN_CAR,
                            ANIM_GROUP_DEFAULT
                        );
                    }
                }
            }
        }
    }

    if (RpAnimBlendClumpGetAssociation(ped->GetRpClump(), (uint32)ANIM_ID_SMOKE_IN_CAR)) {
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped, 250);
        }
        if (m_nBoppingStartTime != -1) {
            m_nBoppingStartTime = -1;
        }
    } else {
        TriggerIK(ped);
    }

    // Como el original: un peatón generado por el juego, sin grupo, sentado de pasajero delantero en un coche sin conductor
    // acaba bajándose y deambulando en coche tras esperar 4 segundos.
    if (ped->GetCreatedBy() == PED_GAME && !CPedGroups::GetPedsGroup(ped)) {
        if (!m_pVehicle->m_pDriver && m_pVehicle->m_apPassengers.data()[0] == ped) {
            if (!m_waitTimer.m_bStarted) {
                m_waitTimer.m_nStartTime = CTimer::GetTimeInMS();
                m_waitTimer.m_nInterval  = 4000;
                m_waitTimer.m_bStarted   = true;
            }
            if (m_waitTimer.IsOutOfTime()) {
                const auto sequence = new CTaskComplexSequence;
                sequence->AddTask(new CTaskComplexLeaveCar(m_pVehicle, 0, 0, true, false));
                sequence->AddTask(new CTaskComplexCarDriveWander(m_pVehicle, static_cast<eCarDrivingStyle>(0), 10.0f));
                CEventScriptCommand event{ 3, sequence, false };
                ped->GetEventGroup().Add(&event, false);
            }
        } else {
            m_waitTimer.m_bStarted = false;
        }
    }

    // Como el original: de vez en cuando (según el contador de fotogramas y la semilla del peatón) el pasajero comenta la conducción del jugador.
    if (((CTimer::m_FrameCounter + (uint32)ped->m_nRandomSeed) & 0x3FFFu) == 0) {
        const auto driver = m_pVehicle->m_pDriver;
        if (driver && driver != ped && driver->IsPlayer()) {
            const float sx = m_pVehicle->m_vecMoveSpeed.x;
            const float sy = m_pVehicle->m_vecMoveSpeed.y;
            if (std::sqrt(sx * sx + sy * sy) > 0.7f) {
                ped->Say(static_cast<eGlobalSpeechContext>(0x20), 0, 1.0f, false, false, false);
            }
            const float tx = m_pVehicle->m_vecMoveSpeed.x;
            const float ty = m_pVehicle->m_vecMoveSpeed.y;
            if (std::sqrt(tx * tx + ty * ty) < 0.1f) {
                ped->Say(static_cast<eGlobalSpeechContext>(0x27), 0, 1.0f, false, false, false);
            }
            ped->Say(static_cast<eGlobalSpeechContext>(0x29), 0, 1.0f, false, false, false);
        }
    }
    return false;
}

// 0x63C770

// 0x0
bool CTaskSimpleCarDrive::SetPedPosition(CPed* ped) {
    if (m_b08) {
        // Como el original: se hereda la utilidad de alineación de otra tarea de conducción del peatón.
        const auto task = ped->GetIntelligence()->m_TaskMgr.FindTaskByType(TASK_PRIMARY_DEFAULT, TASK_SIMPLE_CAR_DRIVE);
        if (task && task != this) {
            const auto other = static_cast<CTaskSimpleCarDrive*>(task);
            if (other->m_pTaskUtilityLineUpPedWithCar) {
                m_pTaskUtilityLineUpPedWithCar = other->m_pTaskUtilityLineUpPedWithCar;
                other->m_pTaskUtilityLineUpPedWithCar = nullptr;
            }
        }
        m_b08 = false;
    }

    if (m_pTaskUtilityLineUpPedWithCar) {
        // Como el original: tabla de 12 animaciones (0x8D2E9C); se prueban en orden hasta encontrar una activa.
        static constexpr AnimationId anims[12]{
            ANIM_ID_CAR_GETIN_LHS_0, ANIM_ID_CAR_GETIN_RHS_0, ANIM_ID_CAR_GETIN_LHS_1, ANIM_ID_CAR_GETIN_RHS_1,
            ANIM_ID_CAR_GETIN_BIKE_FRONT, ANIM_ID_CAR_CLOSEDOOR_LHS_0, ANIM_ID_CAR_CLOSEDOOR_RHS_0, ANIM_ID_CAR_CLOSEDOOR_LHS_1,
            ANIM_ID_CAR_CLOSEDOOR_RHS_1, ANIM_ID_CAR_SHUFFLE_RHS_0, ANIM_ID_CAR_SHUFFLE_RHS_1, ANIM_ID_CAR_ROLLDOOR
        };
        CAnimBlendAssociation* assoc;
        int32 i = 0;
        do {
            assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), (uint32)anims[i]);
            if (assoc) {
                break;
            }
            ++i;
        } while (i < 12);

        m_pTaskUtilityLineUpPedWithCar->ProcessPed(ped, m_pVehicle, assoc);
        if (m_pTaskUtilityLineUpPedWithCar) {
            delete m_pTaskUtilityLineUpPedWithCar;
        }
        m_pTaskUtilityLineUpPedWithCar = nullptr;
        return true;
    }

    if (m_pVehicle && ped->m_pVehicle == m_pVehicle && ped->bInVehicle) {
        ped->SetPedPositionInCar();
    }
    return true;
}
