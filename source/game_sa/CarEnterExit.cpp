/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCarEnterExit
 * Reconstrucción: COMPLETA — 32/32 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 5.
 * Validación registrada: 5/32 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   GetNearestCarPassengerDoor | 0x00650BB0 | S
 *   GetPositionToOpenCarDoor | 0x0064E740 | S
 *   IsPlayerToQuitCarEnter | 0x0064F240 | S
 *   IsRoomForPedToLeaveCar | 0x006504C0 | S
 *   MakeUndraggedPassengerPedsLeaveCar | 0x0064F540 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Events/EventPedEnteredMyVehicle.h"

#include "PedStats.h"
#include "CarEnterExit.h"
#include "VehicleAnimGroupData.h"
#include "VehicleModelInfo.h"
#include "Bike.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskComplexDriveWander.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"

void CCarEnterExit::InjectHooks() {
    RH_ScopedClass(CCarEnterExit);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddInCarAnim, 0x64F720);
    RH_ScopedInstall(CarHasDoorToClose, 0x64EE10);
    RH_ScopedInstall(CarHasDoorToOpen, 0x64EDD0);
    RH_ScopedInstall(CarHasOpenableDoor, 0x64EE50);
    RH_ScopedInstall(CarHasPartiallyOpenDoor, 0x64EE70);
    RH_ScopedInstall(ComputeDoorFlag, 0x64E550);
    RH_ScopedInstall(ComputeOppositeDoorFlag, 0x64E610);
    RH_ScopedInstall(ComputePassengerIndexFromCarDoor, 0x64F1E0);
    RH_ScopedInstall(ComputeSlowJackedPed, 0x64F070);
    RH_ScopedInstall(ComputeTargetDoorToEnterAsPassenger, 0x64F190);
    RH_ScopedInstall(ComputeTargetDoorToExit, 0x64F110);
    RH_ScopedInstall(GetNearestCarDoor, 0x6528F0);
    RH_ScopedInstall(GetNearestCarPassengerDoor, 0x650BB0);
    RH_ScopedInstall(GetPositionToOpenCarDoor, 0x64E740);
    RH_ScopedInstall(IsCarDoorInUse, 0x64EC90);
    RH_ScopedInstall(IsCarDoorReady, 0x64ED90);
    RH_ScopedInstall(IsCarQuickJackPossible, 0x64EF00);
    RH_ScopedInstall(IsCarSlowJackRequired, 0x64EF70);
    RH_ScopedInstall(IsClearToDriveAway, 0x6509B0);
    RH_ScopedInstall(IsPathToDoorBlockedByVehicleCollisionModel, 0x651210);
    RH_ScopedInstall(IsPedHealthy, 0x64EEE0);
    RH_ScopedInstall(IsPlayerToQuitCarEnter, 0x64F240);
    RH_ScopedInstall(IsRoomForPedToLeaveCar, 0x6504C0);
    RH_ScopedInstall(IsVehicleHealthy, 0x64EEC0);
    RH_ScopedInstall(IsVehicleStealable, 0x6510D0);
    RH_ScopedInstall(MakeUndraggedDriverPedLeaveCar, 0x64F600);
    RH_ScopedInstall(MakeUndraggedPassengerPedsLeaveCar, 0x64F540);
    RH_ScopedInstall(QuitEnteringCar, 0x650130);
    RH_ScopedInstall(RemoveCarSitAnim, 0x64F680);
    RH_ScopedInstall(RemoveGetInAnims, 0x64F6E0);
    RH_ScopedInstall(SetAnimOffsetForEnterOrExitVehicle, 0x64F860);
    RH_ScopedInstall(SetPedInCarDirect, 0x650280);
}

// 0x64F720
void CCarEnterExit::AddInCarAnim(const CVehicle* vehicle, CPed* ped, bool bAsDriver) {
    const auto [grpId, animId] = [&]() -> std::pair<AssocGroupId, AnimationId> {
        if (bAsDriver) { // Inverted
            if (const auto data = const_cast<CVehicle*>(vehicle)->GetRideAnimData()) {
                return { data->AnimGroup, ANIM_ID_BIKE_RIDE };
            } else if (vehicle->IsBoat()) {
                if (vehicle->m_pHandlingData->m_bSitInBoat) {
                    return { ANIM_GROUP_DEFAULT, ANIM_ID_DRIVE_BOAT };
                }
            } else if (vehicle->vehicleFlags.bLowVehicle) {
                return { ANIM_GROUP_DEFAULT, ANIM_ID_CAR_LSIT };
            }

            return { ANIM_GROUP_DEFAULT, ANIM_ID_CAR_SIT };
        } else {
            if (const auto data = const_cast<CVehicle*>(vehicle)->GetRideAnimData()) {
                return { data->AnimGroup, ANIM_ID_BIKE_RIDE };
            } else if (vehicle->vehicleFlags.bLowVehicle) {
                return { ANIM_GROUP_DEFAULT, ANIM_ID_CAR_SITPLO };
            }

            return { ANIM_GROUP_DEFAULT, ANIM_ID_CAR_SITP };
        }
    }();
    CAnimManager::BlendAnimation(ped->GetRpClump(), grpId, animId, 1000.f);
    ped->StopNonPartialAnims();
}

// 0x64EE10
bool CCarEnterExit::CarHasDoorToClose(const CVehicle* vehicle, int32 doorId) {
    auto& veh = const_cast<CVehicle&>(*vehicle);
    return !veh.IsDoorMissingU32(doorId) && !veh.IsDoorClosedU32(doorId);
}

// 0x64EDD0
bool CCarEnterExit::CarHasDoorToOpen(const CVehicle* vehicle, int32 doorId) {
    auto& veh = const_cast<CVehicle&>(*vehicle);
    return !veh.IsDoorMissingU32((uint32)doorId) && !veh.IsDoorFullyOpenU32((uint32)doorId);
}

// 0x64EE50
bool CCarEnterExit::CarHasOpenableDoor(const CVehicle* vehicle, int32 doorId_UnusedArg, const CPed* ped) {
    return vehicle->CanPedOpenLocks(ped);
}

// 0x64EE70
bool CCarEnterExit::CarHasPartiallyOpenDoor(const CVehicle* vehicle, int32 doorId) {
    auto& veh = const_cast<CVehicle&>(*vehicle); // TODO: Fix
    return !veh.IsDoorMissingU32((uint32)doorId)
        && !veh.IsDoorFullyOpenU32((uint32)doorId)
        && !veh.IsDoorClosedU32((uint32)doorId);
}

// 0x64E550
int32 CCarEnterExit::ComputeDoorFlag(const CVehicle* vehicle, int32 doorId, bool bSettingFlags) {
    if (bSettingFlags && (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats)) {
        switch (doorId) {
        case 8:
        case 10:
        case 18: return 5;
        case 9:
        case 11: return 10;
        default: NOTSA_UNREACHABLE(); // Originally `return 0`
        }
    } else {
        switch (doorId) {
        case 8:  return 4;
        case 9:  return 8;
        case 10:
        case 18: return 1;
        case 11: return 2;
        default: NOTSA_UNREACHABLE(); // Originally `return 0`
        }
    }
}

// 0x64E610
int32 CCarEnterExit::ComputeOppositeDoorFlag(const CVehicle* vehicle, int32 doorId, bool bCheckVehicleType) {
    if (bCheckVehicleType && (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats)) {
        switch (doorId) {
        case 8:
        case 10:
        case 18: return 5;
        case 9:
        case 11: return 10;
        default: NOTSA_UNREACHABLE(); // Originally `return 0`
        }
    } else {
        switch (doorId) {
        case 8: return 1;
        case 9: return 2;
        case 10:
        case 18: return 4;
        case 11: return 8;
        default: NOTSA_UNREACHABLE(); // Originally `return 0`
        }
    }
}

// 0x64F1E0
int32 CCarEnterExit::ComputePassengerIndexFromCarDoor(const CVehicle* vehicle, int32 doorId) {
    if (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats) {
        switch (doorId) {
        case 9:
        case 11:
            return 0;
        default:
            return -1;
        }
    }

    switch (doorId) {
    case 8:
        return 0;
    case 9:
        return 2;
    case 11:
        return 1;
    default:
        return -1;
    }
}

// 0x64F070
CPed* CCarEnterExit::ComputeSlowJackedPed(const CVehicle* vehicle, int32 doorId) {
    if (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats) {
        switch (doorId) {
        case 8:
        case 10:
        case 18:
            return vehicle->m_pDriver;
        case 9:
        case 11:
            return vehicle->m_apPassengers[0];
        default:
            return nullptr;
        }
    }

    switch (doorId) {
    case 8:
        return vehicle->m_apPassengers[0];
    case 9:
        return vehicle->m_apPassengers[2];
    case 10:
        return vehicle->m_pDriver;
    case 11:
        return vehicle->m_apPassengers[1];
    default:
        return nullptr;
    }
}

// 0x64F190
int32 CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(const CVehicle* vehicle, int32 psgrIdx) {
    if (vehicle->vehicleFlags.bIsBus) {
        return 8;
    }

    switch (psgrIdx) {
    case 0:
        return (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats) ? 11 : 8; // Inverted condition
    case 1:
        return 11;
    case 2:
        return 9;
    default:
        return -1;
    }
}

// 0x64F110
int32 CCarEnterExit::ComputeTargetDoorToExit(const CVehicle* vehicle, const CPed* ped) {
    // Como el original (0x64F110): conductor -> 10; autobús/autocar (grupo de animación) -> 8 sin mirar al peatón;
    // después solo se comparan los tres primeros pasajeros; si no es ninguno, -1.
    if (vehicle->m_pDriver == ped) {
        return 10;
    }

    const auto group = vehicle->GetAnimGroupId();
    if (group == ANIM_GROUP_COACHCARANIMS || group == ANIM_GROUP_BUSCARANIMS) {
        return 8;
    }

    if (vehicle->m_apPassengers[0] == ped) {
        if (vehicle->m_nVehicleType == VEHICLE_TYPE_BIKE || vehicle->m_pHandlingData->m_bTandemSeats) {
            return 11;
        }
        return 8;
    }
    if (vehicle->m_apPassengers[1] == ped) {
        return 11;
    }
    if (vehicle->m_apPassengers[2] == ped) {
        return 9;
    }
    return -1;
}

// 0x6528F0
bool CCarEnterExit::GetNearestCarDoor(const CPed* ped, const CVehicle* vehicle, CVector& outPos, int32& doorId) {
    auto driverDraggedOutOffset = vehicle->m_pDriver ? &ms_vecPedQuickDraggedOutCarAnimOffset : nullptr;
    auto psgrDraggedOutOffset   = vehicle->HasPassengerAtSeat(0) ? &ms_vecPedQuickDraggedOutCarAnimOffset : nullptr;

    if ((vehicle->IsBike() && !vehicle->IsSubBMX()) || vehicle->IsSubQuad()) {
        driverDraggedOutOffset = nullptr;
        psgrDraggedOutOffset = nullptr;

        if (ped->GetTaskManager().GetActiveTask()->GetTaskType() != TASK_COMPLEX_ENTER_CAR_AS_PASSENGER) {
            if (std::abs(vehicle->GetRight().z) < 0.1f) { // Isn't on it's side
                // Check if ped is 30 degrees to the left of the vehicle
                // Original code used atan and whatnot, but this achieves the same result
                if (DotProduct2D(vehicle->GetRight(), ped->GetForward()) > 0 // On the left
                 && DotProduct2D(vehicle->GetForward(), ped->GetForward()) > std::cos(PI / 6.f)
                ) {
                    if ((ped->IsPlayer() && ped->GetPlayerData()->m_fMoveBlendRatio > 1.5f && doorId == 0) 
                    || (!ped->IsPlayer() && ped->m_nPedType != PED_TYPE_COP && ped->m_nMoveState == PEDMOVE_RUN && ped->m_pStats->m_nTemper > 65 && doorId == 0)
                    ) {
                        // 18 here is probably either from eBikeNodes or eQuadNodes, not sure?
                        if (IsRoomForPedToLeaveCar(vehicle, 18)) {
                            doorId = 18;
                            outPos = GetPositionToOpenCarDoor(vehicle, 18);
                            return true;
                        }
                    }
                }
            }
        }
    } else if (vehicle->vehicleFlags.bIsBus || vehicle->vehicleFlags.bLowVehicle) {
        driverDraggedOutOffset = nullptr;
        psgrDraggedOutOffset = nullptr;
    }


    const auto posDoorFLeft = GetPositionToOpenCarDoor(vehicle, CAR_DOOR_LF);
    const auto posDoorFRight = GetPositionToOpenCarDoor(vehicle, CAR_DOOR_RF);

    CVector2D pedPos2D = ped->GetPosition();
    CVector2D dir2DToDoorFLeft = posDoorFLeft - pedPos2D, dir2DToDoorFRight = posDoorFRight - pedPos2D;

    if (vehicle->m_pVehicleBeingTowed) {
        if (dir2DToDoorFLeft.SquaredMagnitude() < dir2DToDoorFRight.SquaredMagnitude()) {
            if (IsPathToDoorBlockedByVehicleCollisionModel(ped, vehicle, posDoorFRight)) {
                dir2DToDoorFRight = { 999.90002f, 999.90002f };
            } else if (IsPathToDoorBlockedByVehicleCollisionModel(ped, vehicle, posDoorFLeft)) {
                dir2DToDoorFLeft = { 999.90002f, 999.90002f };
            }
        }
    }

    if (vehicle->m_pHandlingData->m_bForceDoorCheck && vehicle->IsAutomobile()) {
        const auto aut = static_cast<const CAutomobile*>(vehicle);

        if (aut->m_aCarNodes[CAR_DOOR_RF]) { // Inverted
            if (!aut->m_aCarNodes[CAR_DOOR_LF]) {
                if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_RF, driverDraggedOutOffset)) {
                    doorId = CAR_DOOR_RF;
                    outPos = posDoorFRight;
                    return true;
                }
            }
        } else {
            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)) {
                doorId = CAR_DOOR_LF;
                outPos = posDoorFLeft;
                return true;
            }
        }

        return false;
    }

    if (doorId != CAR_NODE_NONE && IsRoomForPedToLeaveCar(vehicle, doorId, driverDraggedOutOffset)) {
        switch (doorId) {
        case CAR_DOOR_LF: {
            doorId = CAR_DOOR_LF;
            outPos = posDoorFLeft;
            return true;
        }
        case CAR_DOOR_RF: {
            doorId = CAR_DOOR_RF;
            outPos = posDoorFRight;
            return true;
        }
        default:
            return false;
        }
    }

    if (!vehicle->m_pDriver
    || (!CPedGroups::AreInSameGroup(ped, vehicle->m_pDriver) && !vehicle->m_pDriver->bDontDragMeOutCar)
    ) {
        if (vehicle->vehicleFlags.bIsBus
         || dir2DToDoorFRight.SquaredMagnitude() > dir2DToDoorFLeft.SquaredMagnitude()
        ) {
            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)) {
                doorId = CAR_DOOR_LF;
                outPos = posDoorFLeft;
                return true;
            }
            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_RF, driverDraggedOutOffset)) {
                doorId = CAR_DOOR_RF;
                outPos = posDoorFRight;
                return true;
            }
        } else {
            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_RF, driverDraggedOutOffset)) {
                if ((
                        vehicle->HasPassengerAtSeat(0)
                        && !vehicle->IsBike()
                        && !vehicle->m_pHandlingData->m_bTandemSeats
                        && (CPedGroups::AreInSameGroup(vehicle->m_apPassengers[0], ped) || vehicle->m_apPassengers[0]->bDontDragMeOutCar || vehicle->IsMissionVehicle())
                        && IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)
                    ) || (
                        vehicle->m_nGettingInFlags & 4
                        && IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)
                    )
                ) {
                    doorId = CAR_DOOR_LF;
                    outPos = posDoorFLeft;
                    return true;
                } else {
                    doorId = CAR_DOOR_RF;
                    outPos = posDoorFRight;
                    return true;
                }
            }

            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)) {
                doorId = CAR_DOOR_LF;
                outPos = posDoorFLeft;
                return true;
            }
        }
    }
    return false;
}

// 0x650BB0
bool CCarEnterExit::GetNearestCarPassengerDoor(const CPed* ped, const CVehicle* vehicle, CVector* outVec, int32* doorId, bool CheckIfOccupiedTandemSeat, bool CheckIfDoorIsEnterable, bool CheckIfRoomToGetIn) {
    const auto* h=vehicle->m_pHandlingData;const uint8 flags=*reinterpret_cast<const uint8*>(reinterpret_cast<const uint8*>(vehicle)+0x486);
    const auto canUse=[&](int32 door,uint8 mask){return (!CheckIfDoorIsEnterable||!(flags&mask))&&(!CheckIfRoomToGetIn||IsRoomForPedToLeaveCar(vehicle,door,nullptr));};
    if(h->m_nAnimGroup==15||h->m_nAnimGroup==16){if(!canUse(8,4))return false;*outVec=GetPositionToOpenCarDoor(vehicle,8);*doorId=8;return true;}
    CVector front{},leftRear{},rightRear{};CVector2D frontDelta{999,999},leftDelta{999,999},rightDelta{999,999};bool found=false;
    const auto set=[&](int32 door,CVector& point,CVector2D& delta){point=GetPositionToOpenCarDoor(vehicle,door);delta=CVector2D{point.x-ped->GetPosition().x,point.y-ped->GetPosition().y};found=true;};
    if(vehicle->m_nVehicleType==VEHICLE_TYPE_BIKE||h->m_bTandemSeats){
        if(!CheckIfOccupiedTandemSeat||!vehicle->m_apPassengers[0]){
            if(canUse(11,2))set(11,rightRear,rightDelta);
            if(!CheckIfOccupiedTandemSeat&&canUse(9,8))set(9,leftRear,leftDelta);
            else if(CheckIfOccupiedTandemSeat&&!vehicle->m_apPassengers[0]&&canUse(9,8))set(9,leftRear,leftDelta);
        }
    }else if((!CheckIfOccupiedTandemSeat||!vehicle->m_apPassengers[0])&&canUse(8,4))set(8,front,frontDelta);
    const auto* model=static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(int16(vehicle->GetModelIndex())));
    if(*reinterpret_cast<const int8*>(reinterpret_cast<const uint8*>(model)+0x4C)>2){
        const bool rearSeatModel=(*reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(h)+0xCC)&0x8000u)==0;
        const bool rightDoorPresent=vehicle->m_nVehicleType==VEHICLE_TYPE_AUTOMOBILE&&*reinterpret_cast<void*const*>(reinterpret_cast<const uint8*>(vehicle)+0x674);
        const bool leftDoorPresent=vehicle->m_nVehicleType==VEHICLE_TYPE_AUTOMOBILE&&*reinterpret_cast<void*const*>(reinterpret_cast<const uint8*>(vehicle)+0x66C);
        if((rearSeatModel||rightDoorPresent)&&(!CheckIfOccupiedTandemSeat||!vehicle->m_apPassengers[1])&&canUse(11,2))set(11,rightRear,rightDelta);
        if((rearSeatModel||leftDoorPresent)&&(!CheckIfOccupiedTandemSeat||!vehicle->m_apPassengers[2])&&canUse(9,8))set(9,leftRear,leftDelta);
    }
    *outVec=front;*doorId=8;
    float best=frontDelta.x*frontDelta.x+frontDelta.y*frontDelta.y;
    const float rightDistance=rightDelta.x*rightDelta.x+rightDelta.y*rightDelta.y;
    if(rightDistance<best){*outVec=rightRear;*doorId=11;best=rightDistance;}
    if(leftDelta.x*leftDelta.x+leftDelta.y*leftDelta.y<best){*outVec=leftRear;*doorId=9;}
    return found;
}

// 0x64E740
// Originally RVO'd
CVector CCarEnterExit::GetPositionToOpenCarDoor(const CVehicle* vehicle, int32 doorId) {
    auto* model=static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(int16(vehicle->GetModelIndex())));
    const auto* handling=vehicle->m_pHandlingData;
    auto& anim=CVehicleAnimGroupData::GetVehicleAnimGroup(handling->m_nAnimGroup);
    const bool bike=vehicle->m_nVehicleType==VEHICLE_TYPE_BIKE;
    const auto seat=[&] { return doorId==9||doorId==11?model->GetBackSeatPosn():model->GetFrontSeatPosn(); };
    if(bike||handling->m_bTandemSeats){
        if(doorId==18){const auto offset=anim.ComputeAnimDoorOffsets(ENTER_BIKE_FRONT);const auto point=seat();return const_cast<CVehicle*>(vehicle)->GetMatrix().TransformPoint(CVector{point.x-offset.x,offset.y+point.y,point.z-offset.z});}
        auto offset=anim.ComputeAnimDoorOffsets(ENTER_FRONT);auto point=seat();
        if(bike){if(doorId==9||doorId==8)offset.x*=-1.0f;CVector result;const_cast<CBike*>(static_cast<const CBike*>(vehicle))->GetCorrectedWorldDoorPosition(result,offset,point);return result;}
        if(doorId==9||doorId==8){offset.x*=-1.0f;point.x+=handling->m_fSeatOffsetDistance;}else point.x-=handling->m_fSeatOffsetDistance;
        return const_cast<CVehicle*>(vehicle)->GetMatrix().TransformPoint(point-offset);
    }
    float seatOffset=handling->m_nAnimGroup==101&&(doorId==11||doorId==9)?0.0f:handling->m_fSeatOffsetDistance;
    int32 kind=std::bit_cast<int32>(seatOffset);
    if(doorId==10||doorId==8)kind=0;else if(doorId==11||doorId==9)kind=1;else if(doorId==18)kind=2;
    auto offset=anim.ComputeAnimDoorOffsets(static_cast<eVehAnimDoorOffset>(kind));auto point=seat();
    switch(doorId){
    case 8:case 9:point.x+=seatOffset;offset.x=-offset.x;break;
    case 10:case 11:point.x=-(point.x+seatOffset);break;
    default:offset=CVector{0,0,0};break;
    }
    auto local=point-offset;
    if(vehicle->m_nVehicleSubType==VEHICLE_TYPE_MTRUCK||(handling->m_bIsBig&&vehicle->m_nVehicleSubType!=VEHICLE_TYPE_PLANE))local.z=0.95f-const_cast<CVehicle*>(vehicle)->GetHeightAboveRoad();
    auto rotated=const_cast<CVehicle*>(vehicle)->GetMatrix().TransformVector(local);
    const auto result=vehicle->GetPosition()+rotated;
    return result;
}

// 0x64EC90
bool CCarEnterExit::IsCarDoorInUse(const CVehicle* vehicle, int32 firstDoorId, int32 secondDoorId) {
    const auto CheckIsDoorInUse = [vehicle](int32 door) {
        const auto CheckInOutFlags = [vehicle](uint32 n) {
            const auto flag = 1 << n;
            return (flag & vehicle->m_nGettingInFlags) || (flag & vehicle->m_nGettingOutFlags);
        };
        switch (door) {
        case 8: return CheckInOutFlags(2);
        case 9: return CheckInOutFlags(3);
        case 10:
        case 18: return CheckInOutFlags(0);
        case 11: return CheckInOutFlags(1);
        default: return false;
        }
    };
    return CheckIsDoorInUse(firstDoorId) || CheckIsDoorInUse(secondDoorId);
}

// 0x64ED90
bool CCarEnterExit::IsCarDoorReady(const CVehicle* vehicle, int32 doorId) {
    // TODO: Make IsDoorReadyU32 a const member function to avoid const_cast
    auto& veh = const_cast<CVehicle&>(*vehicle); // TODO: Fix
    return veh.IsDoorReadyU32((uint32)doorId)
        || veh.IsDoorFullyOpenU32((uint32)doorId);
}

// 0x64EF00
bool CCarEnterExit::IsCarQuickJackPossible(CVehicle* vehicle, int32 doorId, const CPed* ped) {
    // I think doorId 10 is the driver's door
    //if (doorId == 10 && vehicle->IsAutomobile() && !vehicle->IsDoorMissingU32(doorId) && vehicle->IsDoorClosedU32(doorId)) {
    //    // This does *nothing* - I tried `return vehicle->CanPedOpenLocks(ped);` but that just breaks everything.
    //    // Basically, returning anything but `false` from here breaks the code (in `CTaskComplexEnterCar`)
    //    vehicle->CanPedOpenLocks(ped); 
    //}
    return false;
}

// 0x64EF70
bool CCarEnterExit::IsCarSlowJackRequired(const CVehicle* vehicle, int32 doorId) {
    if (vehicle->IsBike() || (vehicle->m_pHandlingData->m_bTandemSeats)) {
        switch (doorId) {
        case 8:
        case 10:
        case 18:
            return vehicle->HasDriver();
        case 9:
        case 11:
            return vehicle->HasPassengerAtSeat(0);
        default:
            return false;
        }
    }

    int group = vehicle->GetAnimGroupId();
    if (group == ANIM_GROUP_COACHCARANIMS || group == ANIM_GROUP_BUSCARANIMS) {
        switch (doorId) {
        case 8:
            return false;
        case 10:
            return vehicle->HasDriver();
        }
    } else {
        switch (doorId) {
        case 8:
            return vehicle->HasPassengerAtSeat(0);
        case 9:
            return vehicle->HasPassengerAtSeat(2);
        case 10:
            return vehicle->HasDriver();
        case 11:
            return vehicle->HasPassengerAtSeat(1);
        default:
            return false;
        }
    }
    return false;
}

// 0x6509B0
bool CCarEnterExit::IsClearToDriveAway(const CVehicle* vehicle) {
    const auto& vehPos = vehicle->GetPosition();
    const auto  bbSizeY = vehicle->GetColModel()->GetBoundingBox().GetSize().y;
    CEntity* hitEntity{};
    CColPoint hitCP{};
    return !CWorld::ProcessLineOfSight(vehPos + vehicle->GetForward() * bbSizeY, vehPos, hitCP, hitEntity, true, true, false, false, false, true, true, false) || hitEntity == vehicle;
}

// 0x651210
bool CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel(const CPed* ped, const CVehicle* vehicle, const CVector& pos) {
    if (vehicle->GetModelIndex() == eModelID::MODEL_AT400) {
        return false;
    }

    const auto vehMatInv = Invert(*vehicle->m_matrix);
    const CColLine line{
        vehMatInv.TransformPoint(ped->GetPosition()),
        vehMatInv.TransformPoint(pos)
    };

    for (const auto& sp : vehicle->GetColModel()->GetData()->GetSpheres()) {
        if (CCollision::TestLineSphere(line, sp)) {
            return false;
        }
    }

    return true;
}

// 0x64EEE0
bool CCarEnterExit::IsPedHealthy(CPed* ped) {
    return ped->m_fHealth > 0.f;
}

// 0x64F240
bool CCarEnterExit::IsPlayerToQuitCarEnter(const CPed* ped, const CVehicle* vehicle, int32 startTime, CTask* task) {
    auto* pad=static_cast<const CPlayerPed*>(ped)->GetPadFromPlayer();
    float heading=ped->m_fCurrentRotation;bool special=false,orient=false;
    if(task){const int32 type=int32(task->GetTaskType());switch(type){
    case 0x2C0:case 0x322:case 0x323:case 0x324:case 0x331:case 0x334:special=true;orient=true;break;
    case 0x321:case 0xCB:case 0xD0:case 0x325:case 0x327:case 0x328:case 0x32C:case 0x330:case 0x33D:orient=true;break;
    default:break;
    }}
    const float pi=std::bit_cast<float>(0x40490FDBu),twoPi=std::bit_cast<float>(0x40C90FDBu),halfPi=std::bit_cast<float>(0x3FC90FDBu);
    if(orient){
        const auto* matrix=vehicle->m_matrix;const auto delta=ped->GetPosition()-vehicle->GetPosition();const auto& right=matrix->GetRight();const float projection=(delta.x*right.x+delta.y*right.y)+delta.z*right.z;const auto& forward=matrix->GetForward();const float negativeX=-forward.x,forwardY=forward.y,signedHalfPi=projection>0.0f?halfPi:-halfPi;
        __asm { fld negativeX
            fld forwardY
            fpatan
            fadd signedHalfPi
            fstp heading
        }
        if(matrix->GetUp().z<0){heading+=pi;if(heading>pi)heading-=twoPi;}
        if(heading>pi)heading-=twoPi;else if(heading<-pi)heading+=twoPi;
    }
    if(*reinterpret_cast<CEntity*const*>(reinterpret_cast<const uint8*>(vehicle)+0x490))return true;
    if(pad->DisablePlayerControls)return false;
    if(special){if(pad->MeleeAttackJustDown(false))return true;}
    else {const auto elapsed=CTimer::GetTimeInMS()-uint32(startTime);const double time=int32(elapsed)<0?double(float(int32(elapsed))+4294967296.0f):double(int32(elapsed));if(time<=double(StaticRef<float>(0x8D2ED8)))return false;}
    const float upDown=float(pad->GetPedWalkUpDown()),leftRight=float(pad->GetPedWalkLeftRight());
    const float negativeLeft=-leftRight,zero=0,cameraOrientation=StaticRef<float>(0xB6F178);
    const float magnitude=std::sqrt(leftRight*leftRight+upDown*upDown)*0.0078125f;
    const float upper=heading+pi,lower=heading-pi,threshold=std::bit_cast<float>(0x3F490FDBu);bool quit=false;
    const auto angleFunction=static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints);
    __asm { push upDown
        push negativeLeft
        push zero
        push zero
        call angleFunction
        add esp,16
        fsub cameraOrientation
        fcom upper
        fnstsw ax
        test ah,41h
        jnz CarQuitBelowUpper
        fsub twoPi
        jmp CarQuitAngleReady
    CarQuitBelowUpper:
        fcom lower
        fnstsw ax
        test ah,5
        jp CarQuitAngleReady
        fadd twoPi
    CarQuitAngleReady:
        fsub heading
        fabs
        fcomp threshold
        fnstsw ax
        test ah,41h
        jnz CarQuitAngleDone
        mov quit,1
    CarQuitAngleDone:
    }
    return magnitude>0.75f&&quit;
}

// 0x6504C0
bool CCarEnterExit::IsRoomForPedToLeaveCar(const CVehicle* vehicle, int32 doorId, const CVector* pos) {
    auto* model=static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(int16(vehicle->GetModelIndex())));
    if(vehicle->m_nVehicleType!=VEHICLE_TYPE_BIKE&&!vehicle->m_pHandlingData->m_bTandemSeats&&(doorId<8||doorId>11))return false;
    auto seat=doorId==9||doorId==11?model->GetBackSeatPosn():model->GetFrontSeatPosn();if(doorId==10||doorId==11)seat.x=-seat.x;
    auto& matrix=const_cast<CVehicle*>(vehicle)->GetMatrix();auto start=matrix.TransformPoint(seat);auto end=GetPositionToOpenCarDoor(vehicle,doorId);
    if(pos){auto offset=*pos;if(doorId==8||doorId==9)offset.x=-offset.x;end+=matrix.TransformVector(offset);}
    if(matrix.GetUp().z<0){start.z+=0.5f;end.z+=0.5f;}
    CVector testPoint;
    if(vehicle->m_nVehicleType==VEHICLE_TYPE_BIKE){testPoint=CVector{end.x,end.y,end.z+0.2f};end.z=testPoint.z+0.35f;}
    else {auto delta=end-start;delta.z=0;const float distance=std::sqrt(delta.y*delta.y+delta.x*delta.x);const float scale=(distance+0.35f)/distance;const auto extended=delta*scale+start;testPoint=CVector{extended.x,extended.y,end.z};}
    if(!CWorld::GetIsLineOfSightClear(start,testPoint,true,false,false,true,false,false,false))return false;
    auto* collision=CWorld::TestSphereAgainstWorld(end,0.35f,const_cast<CVehicle*>(vehicle),vehicle->m_nVehicleType!=VEHICLE_TYPE_TRAIN,true,false,true,false,false);
    if(collision&&!((collision->GetModelIndex()==608&&vehicle->GetModelIndex()==577)||collision==vehicle->m_pAttachedTo))return false;
    CColPoint point{};CEntity* hit{};
    const bool ceiling=CWorld::ProcessVerticalLine(end,1000,point,hit,true,false,false,true,false,false,nullptr);const float ceilingZ=point.m_vecPoint.z;
    if(ceiling&&end.z<ceilingZ&&end.z+0.6f>ceilingZ)return false;
    const int16 modelId=int16(vehicle->GetModelIndex());
    if(vehicle->m_nVehicleType==VEHICLE_TYPE_BOAT||modelId==460||modelId==539||modelId==447||modelId==417)point.m_vecPoint.z-=1;
    else if(!CWorld::ProcessVerticalLine(end,-1000,point,hit,true,false,false,true,false,false,nullptr))return false;
    return ceilingZ==0.0f||!(ceilingZ<point.m_vecPoint.z);
}

// 0x64EEC0
bool CCarEnterExit::IsVehicleHealthy(const CVehicle* vehicle) {
    return vehicle->GetStatus() != STATUS_WRECKED;
}

// 0x6510D0
bool CCarEnterExit::IsVehicleStealable(const CVehicle* vehicle, const CPed* ped) {
    switch (vehicle->m_nVehicleSubType) {
    case VEHICLE_TYPE_PLANE:
    case VEHICLE_TYPE_HELI:
        return false;
    }

    switch (vehicle->m_nVehicleType) {
    case VEHICLE_TYPE_AUTOMOBILE:
    case VEHICLE_TYPE_BIKE:
        break;
    default:
        return false;
    }

    if (ped->m_pVehicle != vehicle) {
        switch (vehicle->GetCreatedBy()) {
        case RANDOM_VEHICLE:
        case PARKED_VEHICLE:
            break;
        default:
            return false;
        }
    }

    if (CUpsideDownCarCheck{}.IsCarUpsideDown(vehicle)) {
        return false;
    }

    if (!vehicle->CanBeDriven()) {
        return false;
    }

    if (vehicle->IsLawEnforcementVehicle()) {
        return false;
    }

    if (const auto drvr = vehicle->m_pDriver) {
        if (   drvr->IsCreatedByMission()
            || drvr->IsPlayer()
            || ped->GetIntelligence()->IsFriendlyWith(*drvr)
            || CPedGroups::AreInSameGroup(ped, drvr)
        ) {
            return false;
        }
    }

    if (const auto grp = ped->GetGroup()) {
        if (grp->IsAnyoneUsingCar(vehicle)) {
            return false;
        }
    }

    if (vehicle->m_pFire) {
        return false;
    }

    if (vehicle->m_fHealth <= 600.f) {
        return false;
    }

    if (vehicle->IsUpsideDown() || vehicle->IsOnItsSide()) {
        return false;
    }

    if (!IsClearToDriveAway(vehicle)) {
        return false;
    }

    return true;
}

// 0x64F600
void CCarEnterExit::MakeUndraggedDriverPedLeaveCar(const CVehicle* vehicle, const CPed* pedGettingIn) {
    auto& veh = const_cast<CVehicle&>(*vehicle); // TODO: Fix
    auto& ped = const_cast<CPed&>(*pedGettingIn); // TODO: Fix
    veh.m_pDriver->GetEventGroup().Add(CEventDraggedOutCar{ &veh, &ped, true });
}

// 0x64F540
void CCarEnterExit::MakeUndraggedPassengerPedsLeaveCar(const CVehicle* targetVehicle, const CPed* draggedPed, const CPed* ped) {
    for (uint32 i = 0; i < targetVehicle->m_nMaxPassengers; ++i) {
        auto* passenger = targetVehicle->m_apPassengers[i];
        if (passenger && passenger != draggedPed && !passenger->bStayInCarOnJack) {
            CEventPedEnteredMyVehicle event{
                const_cast<CPed*>(ped),
                const_cast<CVehicle*>(targetVehicle),
                eTargetDoor(ComputeTargetDoorToExit(targetVehicle, passenger))
            };
            passenger->GetEventGroup().Add(&event, false);
        }
    }
}

// unused
// 0x650130
void CCarEnterExit::QuitEnteringCar(CPed* ped, CVehicle* vehicle, int32 doorId, bool bCarWasBeingJacked) {
    RemoveGetInAnims(ped);
    ped->RestartNonPartialAnims();
    if (!RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_IDLE)) {
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
    }

    if (bCarWasBeingJacked) {
        vehicle->vehicleFlags.bIsBeingCarJacked = true;
    }
    vehicle->m_nNumGettingIn--;

    if (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats) {
        switch (doorId) {
        case 8:
        case 10:
            vehicle->SetGettingInFlags(5);
            break;
        case 9:
        case 11:
            vehicle->SetGettingInFlags(10);
            break;
        }
        vehicle->vehicleFlags.bIsBig = false;
    } else {
        switch (doorId) {
        case 8:
            vehicle->SetGettingInFlags(4);
            break;
        case 9:
            vehicle->SetGettingInFlags(8);
            break;
        case 10:
            vehicle->SetGettingInFlags(vehicle->m_nMaxPassengers ? 1 : 3);
            break;
        case 11:
            vehicle->SetGettingInFlags(vehicle->m_nMaxPassengers ? 2 : 3);
            break;
        }
    }
    ped->SetUsesCollision(false);
}

// 0x64F680
void CCarEnterExit::RemoveCarSitAnim(const CPed* ped) {
    for (auto anim = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_SECONDARY_TASK_ANIM); anim; anim = RpAnimBlendGetNextAssociation(anim, ANIMATION_SECONDARY_TASK_ANIM)) {
        anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
        anim->m_BlendDelta = -1000.f;
    }
    CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0);
}

// 0x64F6E0
void CCarEnterExit::RemoveGetInAnims(const CPed* ped) {
    for (auto anim = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_IS_PARTIAL); anim; anim = RpAnimBlendGetNextAssociation(anim, ANIMATION_IS_PARTIAL)) {
        anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
        anim->m_BlendDelta = -1000.f;
    }
}

// 0x64F860
void CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle() {
    if (ms_bPedOffsetsCalculated) {
        return;
    }

    const auto animBlockIdxs = {
        CAnimManager::GetAnimationBlockIndex("int_house"),
        CAnimManager::GetAnimationBlockIndex("int_office")
    };

    for (const auto idx : animBlockIdxs) {
        CStreaming::RequestModel(IFPToModelId(idx), STREAMING_KEEP_IN_MEMORY);
    }
    CStreaming::LoadAllRequestedModels(false);

    for (const auto idx : animBlockIdxs) {
        CAnimManager::AddAnimBlockRef(idx);
    }

    {
        const auto anim = CAnimManager::GetAnimAssociation(ANIM_GROUP_DEFAULT, ANIM_ID_GETUP_0);
        CAnimManager::UncompressAnimation(anim->m_BlendHier);
        const auto& seq = anim->m_BlendHier->GetSequences()[0];
        ms_vecPedGetUpAnimOffset = seq.m_FramesNum ? seq.GetUKeyFrame(0)->Trans : CVector{};
    }

    ms_vecPedQuickDraggedOutCarAnimOffset = CVector{ -1.841797f, -0.3261719f, -0.01269531f };

    const std::tuple<AssocGroupId, AnimationId, CVector*> toProcess[]{
        {ANIM_GROUP_INT_HOUSE,  ANIM_ID_BED_IN_L,   &ms_vecPedBedLAnimOffset  },
        {ANIM_GROUP_INT_HOUSE,  ANIM_ID_BED_IN_R,   &ms_vecPedBedRAnimOffset  },
        {ANIM_GROUP_INT_OFFICE, ANIM_ID_OFF_SIT_IN, &ms_vecPedDeskAnimOffset  },
        {ANIM_GROUP_INT_HOUSE,  ANIM_ID_LOU_IN,     &ms_vecPedChairAnimOffset },
    };
    for (const auto [grpId, animId, out] : toProcess) {
        // Calculate translation delta between first and last sequence frames
        *out = [grpId, animId] {
            const auto anim = CAnimManager::GetAnimAssociation(grpId, animId);
            CAnimManager::UncompressAnimation(anim->m_BlendHier);
            const auto& seq = anim->m_BlendHier->GetSequences()[0];
            if (seq.m_FramesNum > 0) {
                return seq.GetUKeyFrame(seq.m_FramesNum - 1)->Trans - seq.GetUKeyFrame(0)->Trans;
            }
            return CVector{};
        }();
    }

    for (const auto idx : animBlockIdxs) {
        CAnimManager::RemoveAnimBlockRef(idx);
    }

    ms_bPedOffsetsCalculated = true;
}

// 0x650280
bool CCarEnterExit::SetPedInCarDirect(CPed* ped, CVehicle* vehicle, int32 doorId, bool bAsDriver) {
    if (bAsDriver) {
        // Warp ped into vehicle
        CTaskSimpleCarSetPedInAsDriver task{ vehicle };
        task.m_bWarpingInToCar = true;
        task.ProcessPed(ped);

        // And make them drive
        ped->GetTaskManager().SetTask(new CTaskComplexCarDriveWander{ vehicle, vehicle->m_autoPilot.m_nCarDrivingStyle, (float)vehicle->m_autoPilot.m_nCruiseSpeed }, TASK_PRIMARY_PRIMARY);

        return true;
    }

    // Warp ped into vehicle
    {
        CTaskSimpleCarSetPedInAsPassenger task{ vehicle, (eTargetDoor)doorId };
        task.m_bWarpingInToCar = true;
        task.ProcessPed(ped);
    }

    if (vehicle->IsBike()) {
        ped->GetTaskManager().SetTask(new CTaskComplexCarDrive{ vehicle, false }, TASK_PRIMARY_PRIMARY);
    }

    // Set mutal acquaintance respect between the ped and all other occupants up to the ped's seat
    // I assume the function is only ever called with `bAsDriver` if there are no passengers
    // So that's why this code-path is only reachable if `bAsDriver` is false

    const auto SetMutalAcquaintanceWith = [ped](CPed* other) {
        if (other) {
            const auto SetWith = [](CPed* of, CPed* with) {
                if (!of->IsCreatedByMission()) {
                    of->GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_RESPECT, CPedType::GetPedFlag(with->m_nPedType));
                }
            };
            SetWith(ped, other);
            SetWith(other, ped);
        }
    };

    SetMutalAcquaintanceWith(vehicle->m_pDriver);

    const auto psgrIdx = ComputePassengerIndexFromCarDoor(vehicle, doorId);
    assert(psgrIdx != -1); // I really doubt this can happen, if it does, an `if` has to be added
    rng::for_each(vehicle->GetPassengers() | rng::views::take((size_t)psgrIdx), SetMutalAcquaintanceWith); // Set with all other passengers up to the ped's seat

    return true;
}
