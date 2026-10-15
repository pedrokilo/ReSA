/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCarAI
 * Reconstrucción: COMPLETA — 25/25 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 25/25 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "TrafficLights.h"
#include "VehicleRecording.h"
#include "CarAI.h"
#include "CarCtrl.h"
#include "CullZones.h"
#include "TaskComplexCopInCar.h"
#include "TaskComplexDriveFireTruck.h"
#include "TaskComplexMedicTreatInjuredPed.h"
#include "TaskComplexKillPedFromBoat.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskSimpleCarSetPedOut.h"
#include "Timer.h"
#include "eWeaponType.h"
#include "X87Math.h"

void CCarAI::InjectHooks() {
    RH_ScopedClass(CCarAI);
    RH_ScopedCategory("AI");

    RH_ScopedInstall(AddAmbulanceOccupants, 0x41C4A0);
    RH_ScopedInstall(AddFiretruckOccupants, 0x41C600);
    RH_ScopedInstall(AddPoliceCarOccupants, 0x41C070);
    RH_ScopedInstall(BackToCruisingIfNoWantedLevel, 0x41BFA0);
    RH_ScopedInstall(CarHasReasonToStop, 0x41C050);
    RH_ScopedInstall(EntitiesGoHeadOn, 0x41CD00);
    RH_ScopedInstall(FindPoliceBikeMissionForWantedLevel, 0x41CA40);
    RH_ScopedInstall(FindPoliceBoatMissionForWantedLevel, 0x41CA50);
    RH_ScopedInstall(FindPoliceCarMissionForWantedLevel, 0x41C9D0);
    RH_ScopedInstall(FindPoliceCarSpeedForWantedLevel, 0x41CAA0);
    RH_ScopedInstall(FindSwitchDistanceClose, 0x41BF50);
    RH_ScopedInstall(FindSwitchDistanceFar, 0x41BF70);
    RH_ScopedInstall(GetCarToGoToCoors, 0x41CE30);
    RH_ScopedInstall(GetCarToGoToCoorsAccurate, 0x41D0E0);
    RH_ScopedInstall(GetCarToGoToCoorsRacing, 0x41D210);
    RH_ScopedInstall(GetCarToGoToCoorsStraightLine, 0x41CFB0);
    RH_ScopedInstall(GetCarToParkAtCoors, 0x41D350);
    RH_ScopedInstall(MakeWayForCarWithSiren, 0x41D660);
    RH_ScopedInstall(MellowOutChaseSpeed, 0x41D3D0);
    RH_ScopedInstall(MellowOutChaseSpeedBoat, 0x41CB70);
    RH_ScopedInstall(TellCarToBlockOtherCar, 0x41C900);
    RH_ScopedInstall(TellCarToFollowOtherCar, 0x41C960);
    RH_ScopedInstall(TellCarToRamOtherCar, 0x41C8A0);
    RH_ScopedInstall(TellOccupantsToLeaveCar, 0x41C760);
    RH_ScopedInstall(UpdateCarAI, 0x41DA30);
}

namespace {
// Niveles de búsqueda como enteros: el EXE compara m_nWantedLevel (0x2C) con signo
constexpr int32 WANTED_CLEAN = 0, WANTED_LEVEL_1 = 1, WANTED_LEVEL_2 = 2, WANTED_LEVEL_3 = 3, WANTED_LEVEL_4 = 4, WANTED_LEVEL_5 = 5, WANTED_LEVEL_6 = 6;

// Como el original: _ftol2 (truncamiento a 64 bits; el llamador usa los bits bajos)
int32 Ftol(float f) {
    return static_cast<int32>(static_cast<int64>(f));
}

// Como el original: sqrt(dy * dy + dx * dx) en 2D
float Dist2D(const CVector& a, const CVector& b) {
    const float ddx = a.x - b.x;
    const float ddy = a.y - b.y;
    return std::sqrt(ddy * ddy + ddx * ddx);
}

// Como el original (CVector::Magnitude 0x4082C0): sqrt((x * x + y * y) + z * z)
float Mag3(const CVector& v) {
    return std::sqrt((v.x * v.x + v.y * v.y) + v.z * v.z);
}

// Como el original (DotProduct 0x40FDB0): (a.z * b.z + a.y * b.y) + a.x * b.x
float Dot3(const CVector& a, const CVector& b) {
    return (a.z * b.z + a.y * b.y) + a.x * b.x;
}

// Como el original (0x41CCB0): forward de la matriz o (-sin h, cos h, 0) con fsin/fcos del x87, guardado en float
CVector ExeForward(const CPlaceable& e) {
    if (e.m_matrix) {
        return e.m_matrix->GetForward();
    }
    const float h = e.m_placement.m_fHeading;
    return CVector{ -TrigSimpleSinCpp(h), TrigSimpleCosCpp(h), 0.f };
}

// Como el original (0x41CC70): right de la matriz o (cos h, sin h, 0) con fsin/fcos del x87, guardado en float
CVector ExeRight(const CPlaceable& e) {
    if (e.m_matrix) {
        return e.m_matrix->GetRight();
    }
    const float h = e.m_placement.m_fHeading;
    return CVector{ TrigSimpleCosCpp(h), TrigSimpleSinCpp(h), 0.f };
}

// Como el original (EntitiesGoHeadOn): forward . d en orden (z + y) + x; sin matriz el forward (-sin h, cos h, 0)
// se queda en los registros del x87 sin redondear (ayudante de X87Math)
float HeadOnDot(const CPlaceable& e, const CVector& d) {
    if (e.m_matrix) {
        const auto& f = e.m_matrix->GetForward();
        return (d.z * f.z + f.y * d.y) + f.x * d.x;
    }
    return CarAIHeadOnDotNoMatrixCpp(e.m_placement.m_fHeading, d.x, d.y, d.z);
}

// Como el original: |coors - destino| en x y luego en y contra el umbral (NaN cuenta como cerca)
bool IsFarFromDestination(const CAutoPilot& ap, const CVector& coors, float threshold) {
    return std::abs(coors.x - ap.m_vecDestinationCoors.x) > threshold
        || std::abs(coors.y - ap.m_vecDestinationCoors.y) > threshold;
}

// Como el original: preparación común de GetCarToGoToCoors* (estilo, sin acción temporal, velocidad, hora y estado)
void PrepareGoToCoors(CVehicle* veh, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    auto& ap = veh->m_autoPilot;
    ap.m_nCarDrivingStyle = drivingStyle;
    ap.m_nTempAction      = TEMPACT_NONE;
    if (setCruiseSpeed) {
        ap.m_nCruiseSpeed = 20;
    }
    ap.m_nTimeToStartMission = CTimer::GetTimeInMS();
    if (veh->GetStatus() != STATUS_GHOST) {
        veh->SetStatus(STATUS_PHYSICS);
    }
}

void SetTempActionFor(CAutoPilot& ap, eAutoPilotTempAction action, uint32 durMs) {
    ap.m_nTempAction     = action;
    ap.m_nTempActionTime = CTimer::GetTimeInMS() + durMs;
}

int32 PlayerWantedLevel() {
    return static_cast<int32>(FindPlayerPed()->GetWantedLevel());
}
} // namespace

// 0x41C4A0
void CCarAI::AddAmbulanceOccupants(CVehicle* vehicle) {
    const auto driver    = vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
    const auto passenger = vehicle->SetupPassenger(1, PED_TYPE_NONE, false, false);

    const auto SetupPed = [vehicle](CPed* ped, CPed* partner, bool isDriver) {
        ped->GetTaskManager().SetTask(new CTaskSimpleCarDrive{ vehicle }, TASK_PRIMARY_DEFAULT);
        ped->GetTaskManager().SetTask(new CTaskComplexMedicTreatInjuredPed{ vehicle, partner, isDriver }, TASK_PRIMARY_PRIMARY);
    };
    SetupPed(driver, passenger, true);
    SetupPed(passenger, driver, false);
}

// 0x41C600
void CCarAI::AddFiretruckOccupants(CVehicle* vehicle) {
    const auto driver    = vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
    const auto passenger = vehicle->SetupPassenger(0, PED_TYPE_NONE, false, false);

    const auto SetupPed  = [vehicle](CPed* ped, CPed* partner, bool isDriver) {
        ped->GetTaskManager().SetTask(new CTaskSimpleCarDrive{ vehicle }, TASK_PRIMARY_DEFAULT);
        ped->GetTaskManager().SetTask(new CTaskComplexDriveFireTruck{ vehicle, partner, isDriver }, TASK_PRIMARY_PRIMARY);
    };
    SetupPed(driver, passenger, true);
    SetupPed(passenger, driver, false);
}

// 0x41C070
void CCarAI::AddPoliceCarOccupants(CVehicle* vehicle, bool bAlwaysCreatePassenger) {
    if (vehicle->vehicleFlags.bOccupantsHaveBeenGenerated) {
        return;
    }
    vehicle->vehicleFlags.bOccupantsHaveBeenGenerated = true;

    switch (static_cast<int16>(vehicle->m_nModelIndex)) { // Como el original: movsx
    case MODEL_ENFORCER:
    case MODEL_FBIRANCH: {
        vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
        for (int32 i = 0; i < 3; i++) {
            vehicle->SetupPassenger(i, PED_TYPE_NONE, false, false);
        }
        break;
    }
    case MODEL_PREDATOR: {
        if (PlayerWantedLevel() > WANTED_LEVEL_1) {
            const auto drvr = vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
            CTaskSimpleCarSetPedOut setPedOut{ vehicle, TARGET_DOOR_DRIVER, true };
            setPedOut.ProcessPed(drvr);
            // Como el original: desplazamiento (0, 3, 2) y 2*pi exacto del EXE (0x40C90FDB)
            drvr->AttachPedToEntity(vehicle, CVector{ 0.f, 3.f, 2.f }, 0, std::bit_cast<float>(0x40C90FDBu), WEAPON_PISTOL);
            drvr->bStayInSamePlace = true;
            drvr->GetTaskManager().SetTask(new CTaskComplexKillPedFromBoat{ FindPlayerPed() }, TASK_PRIMARY_PRIMARY);
        }
        vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
        break;
    }
    case MODEL_RHINO:
    case MODEL_COPBIKE: {
        vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
        break;
    }
    case MODEL_BARRACKS:
    case MODEL_COPCARLA:
    case MODEL_COPCARSF:
    case MODEL_COPCARVG:
    case MODEL_COPCARRU: {
        const auto drvr = vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
        if (PlayerWantedLevel() > WANTED_LEVEL_1) {
            const auto psgr = vehicle->SetupPassenger(0, PED_TYPE_NONE, false, false);

            // Como el original: GetRandomNumberInRange(0, 1) < 0.25 (una segunda tirada para el pasajero)
            if (PlayerWantedLevel() > WANTED_LEVEL_2) {
                if (CGeneral::GetRandomNumberInRange(0.f, 1.f) < 0.25f) {
                    drvr->GiveDelayedWeapon(WEAPON_SHOTGUN, 1000);
                } else if (CGeneral::GetRandomNumberInRange(0.f, 1.f) < 0.25f) {
                    psgr->GiveDelayedWeapon(WEAPON_SHOTGUN, 1000);
                }
            }

            const auto SetupPed = [vehicle](CPed* ped, CPed* partner, bool isDriver) {
                ped->GetIntelligence()->ClearTasks(true, true);
                ped->GetTaskManager().SetTask(
                    new CTaskComplexCopInCar{ vehicle, partner, FindPlayerPed(), isDriver },
                    TASK_PRIMARY_PRIMARY,
                    true
                );
            };
            SetupPed(drvr, psgr, true);
            SetupPed(psgr, drvr, false);
        } else if (bAlwaysCreatePassenger || CGeneral::GetRandomIntInRange(0, 100) < 50) { // Como el original: 0x407180
            vehicle->SetupPassenger(0, PED_TYPE_NONE, false, false);
        }
        break;
    }
    }
}

// 0x41BFA0
void CCarAI::BackToCruisingIfNoWantedLevel(CVehicle* vehicle) {
    if (!vehicle->vehicleFlags.bIsLawEnforcer) {
        return;
    }

    // Como el original: m_bEverybodyBackOff primero; después se vuelve a mirar bIsLawEnforcer y el nivel del jugador
    if (!FindPlayerWanted()->m_bEverybodyBackOff) {
        if (!vehicle->vehicleFlags.bIsLawEnforcer) {
            return;
        }
        if (PlayerWantedLevel() != WANTED_CLEAN && !FindPlayerWanted()->PoliceBackOff() && !CCullZones::NoPolice()) {
            return;
        }
    }

    CCarCtrl::JoinCarWithRoadSystem(vehicle);

    vehicle->vehicleFlags.bSirenOrAlarm   = false; // Como el original: apaga la sirena
    vehicle->m_autoPilot.m_nCarMission      = MISSION_CRUISE;
    vehicle->m_autoPilot.m_nCarDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS;
    if (CCullZones::NoPolice()) {
        vehicle->m_autoPilot.m_nCarMission = MISSION_NONE;
    }
}

// 0x41C050
void CCarAI::CarHasReasonToStop(CVehicle* vehicle) {
    vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
}

// 0x41CD00
bool CCarAI::EntitiesGoHeadOn(CEntity* eA, CEntity* eB) {
    CVector d = eA->GetPosition() - eB->GetPosition();
    d.Normalise();
    // Como el original: NaN en el primer producto sigue adelante
    if (HeadOnDot(*eA, d) > -0.8f) {
        return false;
    }
    return HeadOnDot(*eB, d) >= 0.8f;
}

// 0x41CA40
eCarMission CCarAI::FindPoliceBikeMissionForWantedLevel() {
    return MISSION_APPROACHPLAYER_FARAWAY;
}

// 0x41CA50
eCarMission CCarAI::FindPoliceBoatMissionForWantedLevel() {
    const auto level = static_cast<int32>(FindPlayerWanted()->GetWantedLevel());
    if (level >= WANTED_LEVEL_2 && level <= WANTED_LEVEL_6) {
        return FindPlayerVehicle() ? MISSION_BOAT_ATTACKPLAYER : MISSION_BOAT_CIRCLEPLAYER;
    }
    return FindPlayerVehicle() ? MISSION_BLOCKPLAYER_FARAWAY : MISSION_BOAT_CIRCLEPLAYER;
}

// 0x41C9D0
eCarMission CCarAI::FindPoliceCarMissionForWantedLevel() {
    // Como el original: rand() & 3 directo; bloquear de lejos o embestir de lejos
    int32 below;
    switch (static_cast<uint32>(FindPlayerWanted()->GetWantedLevel())) {
    case WANTED_LEVEL_2: below = 3; break;
    case WANTED_LEVEL_3: below = 2; break;
    case WANTED_LEVEL_4:
    case WANTED_LEVEL_5:
    case WANTED_LEVEL_6: below = 1; break;
    default:             return MISSION_BLOCKPLAYER_FARAWAY;
    }
    return static_cast<int32>(CGeneral::GetRandomNumber() & 3) < below
        ? MISSION_BLOCKPLAYER_FARAWAY
        : MISSION_RAMPLAYER_FARAWAY;
}

// 0x41CAA0
int32 CCarAI::FindPoliceCarSpeedForWantedLevel(CVehicle* vehicle) {
    // Como el original: velocidad máxima por 54/72/75/78 (constantes del EXE) y _ftol2
    const auto MaxVel = [vehicle] { return vehicle->m_pHandlingData->m_transmissionData.m_MaxFlatVelocity; };
    switch (static_cast<uint32>(FindPlayerWanted()->GetWantedLevel())) {
    case WANTED_CLEAN:   return Ftol(CGeneral::GetRandomNumberInRange(12.f, 16.f));
    case WANTED_LEVEL_1: return 25;
    case WANTED_LEVEL_2: return 34;
    case WANTED_LEVEL_3: return Ftol(MaxVel() * 54.f);
    case WANTED_LEVEL_4: return Ftol(MaxVel() * 72.f);
    case WANTED_LEVEL_5: return Ftol(MaxVel() * 75.f);
    case WANTED_LEVEL_6: return Ftol(MaxVel() * 78.f);
    default:             return 0;
    }
}

// 0x41BF50
float CCarAI::FindSwitchDistanceClose(CVehicle* vehicle) {
    return (float)vehicle->m_autoPilot.m_nStraightLineDistance;
}

// 0x41BF70
float CCarAI::FindSwitchDistanceFar(CVehicle* vehicle) {
    return vehicle->vehicleFlags.bIsLawEnforcer
        ? 50.f
        : FindSwitchDistanceClose(vehicle) + 5.0f;
}

// 0x41CE30
float CCarAI::GetCarToGoToCoors(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    auto& ap = veh->m_autoPilot;
    switch (ap.m_nCarMission) {
    case MISSION_PARK_PERPENDICULAR:
    case MISSION_PARK_PARALLEL:
    case MISSION_PARK_PERPENDICULAR_2:
    case MISSION_PARK_PARALLEL_2:
        break;
    case MISSION_GOTOCOORDINATES:
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE: {
        // Como el original: se compara con el destino actual (no con la posición del coche)
        if (!IsFarFromDestination(ap, coors, 5.f)) {
            break;
        }
        [[fallthrough]];
    }
    default: {
        PrepareGoToCoors(veh, drivingStyle, setCruiseSpeed);
        ap.m_nCarMission = CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, coors, false, false)
            ? MISSION_GOTOCOORDINATES_STRAIGHTLINE
            : MISSION_GOTOCOORDINATES;
        break;
    }
    }
    return Dist2D(veh->GetPosition(), coors);
}

// 0x41D0E0
float CCarAI::GetCarToGoToCoorsAccurate(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    auto& ap = veh->m_autoPilot;
    switch (ap.m_nCarMission) {
    case MISSION_GOTOCOORDINATES_ACCURATE:
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE: {
        if (IsFarFromDestination(ap, coors, 2.f)) {
            ap.m_vecDestinationCoors = coors;
        }
        break;
    }
    default: {
        PrepareGoToCoors(veh, drivingStyle, setCruiseSpeed);
        ap.m_nCarMission = CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, coors, false, false)
            ? MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE
            : MISSION_GOTOCOORDINATES_ACCURATE;
        break;
    }
    }
    return Dist2D(veh->GetPosition(), coors);
}

// 0x41D210
float CCarAI::GetCarToGoToCoorsRacing(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    auto& ap = veh->m_autoPilot;
    switch (ap.m_nCarMission) {
    case MISSION_GOTOCOORDINATES_RACING:
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE: {
        if (IsFarFromDestination(ap, coors, 2.f)) {
            ap.m_vecDestinationCoors = coors;
        }
        break;
    }
    default: {
        PrepareGoToCoors(veh, drivingStyle, setCruiseSpeed);
        ap.m_vecDestinationCoors = coors;
        ap.m_nCarMission         = MISSION_GOTOCOORDINATES_RACING;
        CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, coors, false, false);
        break;
    }
    }
    return Dist2D(veh->GetPosition(), coors);
}

// 0x41CFB0
float CCarAI::GetCarToGoToCoorsStraightLine(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    auto& ap = veh->m_autoPilot;
    if (ap.m_nCarMission == MISSION_GOTOCOORDINATES_STRAIGHTLINE) {
        if (IsFarFromDestination(ap, coors, 2.f)) {
            ap.m_vecDestinationCoors = coors;
        }
    } else {
        PrepareGoToCoors(veh, drivingStyle, setCruiseSpeed);
        ap.m_vecDestinationCoors = coors;
        ap.m_nCarMission         = MISSION_GOTOCOORDINATES_STRAIGHTLINE; // Como el original: sin JoinCarWithRoadSystemGotoCoors
    }
    return Dist2D(veh->GetPosition(), coors);
}

// 0x41D350
float CCarAI::GetCarToParkAtCoors(CVehicle* veh, const CVector& coors) {
    // Como el original: primero GetCarToGoToCoors (estilo 0, con velocidad) y luego bParking (no bCanPark)
    GetCarToGoToCoors(veh, coors, DRIVING_STYLE_STOP_FOR_CARS, true);
    veh->vehicleFlags.bParking       = true;
    veh->m_autoPilot.m_nCruiseSpeed = 10;
    return Dist2D(veh->GetPosition(), coors);
}

// 0x41D660
void CCarAI::MakeWayForCarWithSiren(CVehicle* carWithSiren) {
    const auto& sirenMoveSpeed = carWithSiren->m_vecMoveSpeed;
    auto* const pool = GetVehiclePool();
    const float sirenSpeed2D = std::sqrt(sirenMoveSpeed.x * sirenMoveSpeed.x + sirenMoveSpeed.y * sirenMoveSpeed.y);
    if (sirenSpeed2D < 0.1f) { // Como el original: sale si va despacio (NaN sigue)
        return;
    }
    const float invSpeed = 1.f / sirenSpeed2D;
    const float dirX     = invSpeed * sirenMoveSpeed.x;
    const float dirY     = invSpeed * sirenMoveSpeed.y;

    for (int32 i = pool->GetSize() - 1; i >= 0; i--) { // Como el original: de atrás hacia delante
        auto* const v = pool->GetAt(i);
        if (!v) {
            continue;
        }
        if (!v->IsAutomobile() && !v->IsBike()) {
            continue;
        }
        const auto status = v->GetStatus();
        if (status != STATUS_SIMPLE && status != STATUS_PHYSICS) {
            continue;
        }
        if (!v->IsCreatedBy(RANDOM_VEHICLE)) {
            continue;
        }
        if (v->vehicleFlags.bIsLawEnforcer) {
            continue;
        }
        if (v->m_pDriver && !v->m_pDriver->IsCreatedBy(PED_GAME)) {
            continue;
        }
        if (v == carWithSiren) {
            continue;
        }
        if (v->vehicleFlags.bIsAmbulanceOnDuty || v->vehicleFlags.bIsFireTruckOnDuty) {
            continue;
        }
        if (v->vehicleFlags.bMadDriver) {
            continue;
        }
        if (static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(v->m_nModelIndex)])->m_nVehicleClass == VEHICLE_CLASS_BIG) {
            continue;
        }
        const auto& sirenPos = carWithSiren->GetPosition();
        const auto& vehPos   = v->GetPosition();
        if (!(std::abs(sirenPos.z - vehPos.z) < 5.f)) { // Como el original: NaN se salta
            continue;
        }
        const float ddx  = vehPos.x - sirenPos.x;
        const float ddy  = vehPos.y - sirenPos.y;
        const float dist = std::sqrt(ddy * ddy + ddx * ddx);
        if (!(sirenSpeed2D * 45.f + 40.f > dist)) {
            continue;
        }
        const auto& vms = v->m_vecMoveSpeed;
        if (!(std::sqrt(vms.y * vms.y + vms.x * vms.x) > 0.05f)) { // Como el original: módulo (no cuadrado) contra 0,05
            continue;
        }
        auto& ap = v->m_autoPilot;
        if ((ddy * dirY + ddx * dirX) / dist > 0.8f) { // 0x41D8A2
            const auto& fwd = v->m_matrix->GetForward(); // Como el original: sin comprobar la matriz
            const float sameDirDot = dirY * fwd.y + dirX * fwd.x;
            if (sameDirDot > 0.7f || sameDirDot < -0.9f) { // 0x41D942
                if (ap.m_nTempAction != TEMPACT_SWIRVELEFT_STOP && ap.m_nTempAction != TEMPACT_SWIRVERIGHT_STOP) {
                    // Como el original: lado según el producto cruzado (no el escalar)
                    ap.m_nTempAction = ddx * dirY - ddy * dirX > 0.f ? TEMPACT_SWIRVELEFT_STOP : TEMPACT_SWIRVERIGHT_STOP;
                    if (sameDirDot < 0.f) {
                        ap.m_nTempAction = ap.m_nTempAction == TEMPACT_SWIRVELEFT_STOP ? TEMPACT_SWIRVERIGHT_STOP : TEMPACT_SWIRVELEFT_STOP;
                    }
                    if (ap.m_nTempAction == TEMPACT_SWIRVERIGHT_STOP) {
                        ap.m_nTempAction = TEMPACT_WAIT;
                    }
                    ap.m_nTempActionTime = CTimer::GetTimeInMS() + 2'500;
                }
                if (status != STATUS_GHOST) {
                    v->SetStatus(STATUS_PHYSICS);
                }
            } else if (ddx * vms.x + ddy * vms.y < 0.f) {
                if (ap.m_nTempAction != TEMPACT_WAIT && ap.m_nTempAction != TEMPACT_BRAKE) { // Como el original: WAIT/BRAKE
                    SetTempActionFor(ap, TEMPACT_WAIT, 4'000);
                }
            }
        } else if (ddx * vms.x + ddy * vms.y < 0.f) { // 0x41D9D5
            if (ap.m_nTempAction != TEMPACT_WAIT && ap.m_nTempAction != TEMPACT_BRAKE) {
                SetTempActionFor(ap, TEMPACT_WAIT, 2'000);
            }
        }
    }
}

// 0x41D3D0
void CCarAI::MellowOutChaseSpeed(CVehicle* vehicle) {
    auto& ap = vehicle->m_autoPilot;
    // Como el original: distancia 3D en orden (dz^2 + dy^2) + dx^2
    const auto DistToPlayer = [vehicle] {
        const auto  player = FindPlayerCoors();
        const auto& pos    = vehicle->GetPosition();
        const float ddx = pos.x - player.x, ddy = pos.y - player.y, ddz = pos.z - player.z;
        return std::sqrt((ddz * ddz + ddy * ddy) + ddx * ddx);
    };

    if (static_cast<int32>(FindPlayerWanted()->GetWantedLevel()) == WANTED_LEVEL_1) {
        const float dist = DistToPlayer();
        if (FindPlayerVehicle()) {
            ap.m_nCruiseSpeed = static_cast<uint8>(dist < 10.f ? 15 : dist < 20.f ? 22 : 25);
        } else {
            ap.m_nCruiseSpeed = static_cast<uint8>(dist < 20.f ? 5 : dist < 40.f ? 13 : 25);
        }
    } else if (static_cast<int32>(FindPlayerWanted()->GetWantedLevel()) == WANTED_LEVEL_2) {
        const float dist = DistToPlayer();
        if (FindPlayerVehicle()) {
            ap.m_nCruiseSpeed = static_cast<uint8>(dist < 10.f ? 27 : dist < 20.f ? 30 : 34);
        } else {
            ap.m_nCruiseSpeed = static_cast<uint8>(dist < 20.f ? 5 : dist < 40.f ? 18 : 34);
        }
    }

    // Como el original: jugador a pie casi parado (módulo < 0,07) a menos de 30 -> como mucho 10
    if (!FindPlayerVehicle() && Mag3(FindPlayerPed()->m_vecMoveSpeed) < 0.07f) {
        if (DistToPlayer() < 30.f) {
            ap.m_nCruiseSpeed = std::min<uint8>(ap.m_nCruiseSpeed, 10);
        }
    }
}

// 0x41CB70
void CCarAI::MellowOutChaseSpeedBoat(CVehicle* vehicle) {
    auto& cruise = vehicle->m_autoPilot.m_nCruiseSpeed;
    switch (static_cast<uint32>(FindPlayerWanted()->GetWantedLevel())) {
    case WANTED_CLEAN:   cruise = 8;  break;
    case WANTED_LEVEL_1: cruise = 10; break;
    case WANTED_LEVEL_2: cruise = 15; break;
    case WANTED_LEVEL_3: cruise = 20; break;
    case WANTED_LEVEL_4: cruise = 25; break;
    case WANTED_LEVEL_5: cruise = 30; break;
    case WANTED_LEVEL_6: cruise = 40; break;
    default:             break; // Como el original: no cambia
    }
}

// 0x41C900
void CCarAI::TellCarToBlockOtherCar(CVehicle* vehicle1, CVehicle* vehicle2) {
    auto& ap = vehicle1->m_autoPilot;
    ap.m_TargetEntity = vehicle2;
    vehicle2->RegisterReference(reinterpret_cast<CEntity**>(&ap.m_TargetEntity)); // Como el original: sin comprobar nulo
    CCarCtrl::JoinCarWithRoadSystem(vehicle1);
    ap.m_nCarMission                   = MISSION_BLOCKCAR_FARAWAY;
    vehicle1->vehicleFlags.bEngineOn = !vehicle1->vehicleFlags.bEngineBroken;
    ap.m_nCruiseSpeed                  = std::max<uint8>(ap.m_nCruiseSpeed, 6);
}

// 0x41C960
void CCarAI::TellCarToFollowOtherCar(CVehicle* follower, CVehicle* toFollow, float radius) {
    auto& ap = follower->m_autoPilot;
    ap.m_TargetEntity = toFollow;
    toFollow->RegisterReference(reinterpret_cast<CEntity**>(&ap.m_TargetEntity)); // Como el original: sin comprobar nulo
    CCarCtrl::JoinCarWithRoadSystem(follower);
    ap.m_nCarMission                   = MISSION_FOLLOWCAR_FARAWAY;
    follower->vehicleFlags.bEngineOn = !follower->vehicleFlags.bEngineBroken;
    ap.m_nCruiseSpeed                  = std::max<uint8>(ap.m_nCruiseSpeed, 6);
    ap.m_ucCarFollowDist               = static_cast<uint8>(Ftol(radius));
}

// 0x41C8A0
void CCarAI::TellCarToRamOtherCar(CVehicle* vehicle1, CVehicle* vehicle2) {
    auto& ap = vehicle1->m_autoPilot;
    ap.m_TargetEntity = vehicle2;
    vehicle2->RegisterReference(reinterpret_cast<CEntity**>(&ap.m_TargetEntity)); // Como el original: sin comprobar nulo
    CCarCtrl::JoinCarWithRoadSystem(vehicle1);
    ap.m_nCarMission                   = MISSION_RAMCAR_FARAWAY;
    vehicle1->vehicleFlags.bEngineOn = !vehicle1->vehicleFlags.bEngineBroken;
    ap.m_nCruiseSpeed                  = std::max<uint8>(ap.m_nCruiseSpeed, 6);
}

// 0x41C760
void CCarAI::TellOccupantsToLeaveCar(CVehicle* vehicle) {
    // Como el original: CPedIntelligence::FindTaskByType (0x600EE0); devuelve si se creó la tarea nueva
    const auto TellToLeaveCar = [vehicle](CPed* ped) {
        if (auto* const task = static_cast<CTaskComplexCopInCar*>(ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_COP_IN_CAR))) {
            task->m_flag0x2 = true;
            return false;
        }
        ped->GetTaskManager().SetTask(new CTaskComplexLeaveCar{ vehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false }, TASK_PRIMARY_PRIMARY, false);
        return true;
    };

    if (vehicle->m_pDriver) {
        TellToLeaveCar(vehicle->m_pDriver);
    }
    // Como el original: todos los asientos hasta m_nMaxPassengers (0x488), saltando los vacíos, y un rand descartado
    // tras cada tarea nueva
    for (uint32 i = 0; i < vehicle->m_nMaxPassengers; i++) {
        if (CPed* const passenger = vehicle->m_apPassengers.data()[i]) {
            if (TellToLeaveCar(passenger)) {
                CGeneral::GetRandomNumber();
            }
        }
    }
}

// 0x41DA30
void CCarAI::UpdateCarAI(CVehicle* veh) {
    auto& ap = veh->m_autoPilot;

    if (ap.m_vehicleRecordingId >= 0 && !CVehicleRecording::bUseCarAI.data()[ap.m_vehicleRecordingId]) {
        return;
    }
    if (veh->IsSubHeli()) { // Como el original: los helicópteros salen siempre
        return;
    }

    if (ap.m_nCarMission == MISSION_PLANE_ATTACK_PLAYER_POLICE) { // 0x41DAA0
        if (PlayerWantedLevel() < WANTED_LEVEL_3 || !FindPlayerVehicle()) {
            ap.m_vecDestinationCoors = CVector{ -6'000.f, 10'000.f, 500.f }; // Como el original: y positiva
            ap.m_nCarMission         = MISSION_PLANE_FLYTOCOORS;
        }
    }

    if (veh->vehicleFlags.bIsLawEnforcer) { // 0x41DAF8
        switch (ap.m_nCarMission) {
        case MISSION_BLOCKPLAYER_FARAWAY:
        case MISSION_RAMPLAYER_FARAWAY:
        case MISSION_BLOCKPLAYER_CLOSE:
        case MISSION_RAMPLAYER_CLOSE:
        case MISSION_APPROACHPLAYER_FARAWAY:
        case MISSION_APPROACHPLAYER_CLOSE:
        case MISSION_KILLPED_FARAWAY:
        case MISSION_KILLPED_CLOSE:
            ap.m_nCruiseSpeed = static_cast<uint8>(FindPoliceCarSpeedForWantedLevel(veh));
            break;
        }
    }

    const auto law = [veh] { return veh->vehicleFlags.bIsLawEnforcer != 0; };
    const auto TargetDist = [veh, &ap] { return Dist2D(ap.m_TargetEntity->GetPosition(), veh->GetPosition()); };
    const auto PlayerDist = [veh] { return Dist2D(FindPlayerCoors(), veh->GetPosition()); };
    const auto DestDist   = [veh, &ap] { return Dist2D(ap.m_vecDestinationCoors, veh->GetPosition()); };
    const auto TellAndStop = [veh, &ap] {
        TellOccupantsToLeaveCar(veh);
        ap.m_nCruiseSpeed = 0;
        ap.m_nCarMission  = MISSION_NONE;
        if (PlayerWantedLevel() <= WANTED_LEVEL_1) {
            veh->vehicleFlags.bSirenOrAlarm = false;
        }
    };

    switch (veh->GetStatus()) {
    case STATUS_SIMPLE:
    case STATUS_PHYSICS:
    case STATUS_GHOST: {
        switch (ap.m_nCarMission) {
        case MISSION_RAMPLAYER_FARAWAY:    // 0x41DB72
        case MISSION_BLOCKPLAYER_FARAWAY: { // 0x41DF54
            const bool  ram  = ap.m_nCarMission == MISSION_RAMPLAYER_FARAWAY;
            const float dist = PlayerDist();
            if (dist < FindSwitchDistanceClose(veh) || EntitiesGoHeadOn(FindPlayerEntity(), veh) && dist < 40.f) {
                ap.m_nCarMission = ram ? MISSION_RAMPLAYER_CLOSE : MISSION_BLOCKPLAYER_CLOSE;
                if (veh->UsesSiren()) {
                    veh->vehicleFlags.bSirenOrAlarm = true;
                }
            }
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_RAMPLAYER_CLOSE:    // 0x41DC92
        case MISSION_BLOCKPLAYER_CLOSE: { // 0x41E07D
            const bool  ram      = ap.m_nCarMission == MISSION_RAMPLAYER_CLOSE;
            const float maxSpeed = ram ? 0.05f : 0.04f; // Como el original: módulo (no cuadrado)
            const float dist     = PlayerDist();
            if (FindSwitchDistanceFar(veh) < dist) {
                if (!CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, FindPlayerCoors(), true, false)) {
                    ap.m_nCarMission                = ram ? MISSION_RAMPLAYER_FARAWAY : MISSION_BLOCKPLAYER_FARAWAY;
                    veh->m_HornCounter              = 0;
                    veh->vehicleFlags.bSirenOrAlarm = false;
                }
            } else {
                if (ram) {
                    if (FindPlayerVehicle() && veh->GetHasCollidedWith(FindPlayerVehicle())) {
                        if (ap.m_nTempAction != TEMPACT_TURNLEFT && ap.m_nTempAction != TEMPACT_TURNRIGHT) {
                            ap.m_nTempAction     = TEMPACT_REVERSE;
                            ap.m_nTempActionTime = CTimer::GetTimeInMS() + (Mag3(FindPlayerVehicle()->m_vecMoveSpeed) < 0.05f ? 800 : 50);
                        }
                    }
                }
                auto& timer = reinterpret_cast<uint16&>(veh->m_nCopsInCarTimer);
                if (FindPlayerVehicle() && Mag3(FindPlayerVehicle()->m_vecMoveSpeed) < maxSpeed) {
                    // Como el original: paso * 16,666666 (constante del EXE) + temporizador sin signo, _ftol2
                    timer = static_cast<uint16>(Ftol(CTimer::GetTimeStep() * 16.666666f + static_cast<float>(timer)));
                } else {
                    timer = 0;
                }
                if (!FindPlayerVehicle() || FindPlayerVehicle()->IsUpsideDown() || Mag3(FindPlayerVehicle()->m_vecMoveSpeed) < maxSpeed && timer > 2'500) {
                    if (!law()) {
                        break;
                    }
                    if ((veh->m_nModelIndex != MODEL_RHINO || veh->m_nRandomSeed > 10'000) && dist < 10.f) {
                        TellAndStop();
                    }
                }
            }
            if (law()) {
                MellowOutChaseSpeed(veh);
                BackToCruisingIfNoWantedLevel(veh);
            }
            break;
        }
        case MISSION_GOTOCOORDINATES: // 0x41E2BF
            if (DestDist() < FindSwitchDistanceClose(veh)) {
                ap.m_nCarMission = MISSION_GOTOCOORDINATES_STRAIGHTLINE;
            }
            break;
        case MISSION_GOTOCOORDINATES_ACCURATE: // 0x41E338
            if (DestDist() < FindSwitchDistanceClose(veh)) {
                ap.m_nCarMission = MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE;
            }
            break;
        case MISSION_GOTOCOORDINATES_STRAIGHTLINE:            // 0x41E3B1
        case MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE: { // 0x41E473
            const bool  accurate = ap.m_nCarMission == MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE;
            const float dist     = DestDist();
            if (dist < (accurate ? 1.f : 3.f)) {
                ap.m_nCarMission = MISSION_NONE;
                ap.m_nTempAction = TEMPACT_NONE;
                if (accurate && veh->vehicleFlags.bParking) {
                    TellOccupantsToLeaveCar(veh);
                    veh->vehicleFlags.bParking = false;
                }
            } else if (dist > FindSwitchDistanceClose(veh) + 5.f && (CTimer::GetFrameCounter() & 7) == 0) {
                ap.m_nTempAction = TEMPACT_NONE;
                const bool joined = CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, ap.m_vecDestinationCoors, true, false);
                ap.m_nCarMission = accurate
                    ? (joined ? MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE : MISSION_GOTOCOORDINATES_ACCURATE)
                    : (joined ? MISSION_GOTOCOORDINATES_STRAIGHTLINE : MISSION_GOTOCOORDINATES);
            }
            break;
        }
        case MISSION_EMERGENCYVEHICLE_STOP:
        case MISSION_PROTECTION_REAR:
        case MISSION_PROTECTION_FRONT:
            break;
        case MISSION_RAMCAR_FARAWAY:    // 0x41E568
        case MISSION_BLOCKCAR_FARAWAY:  // 0x41E79D
        case MISSION_FOLLOWCAR_FARAWAY: // 0x41E989
        case MISSION_KILLPED_FARAWAY:   // 0x41EE76
        case MISSION_DO_DRIVEBY_FARAWAY: { // 0x41F4D7
            if (!ap.m_TargetEntity) {
                ap.m_nCarMission = MISSION_NONE;
                break;
            }
            if (TargetDist() < FindSwitchDistanceClose(veh)) {
                switch (ap.m_nCarMission) {
                case MISSION_RAMCAR_FARAWAY:    ap.m_nCarMission = MISSION_RAMCAR_CLOSE;    break;
                case MISSION_FOLLOWCAR_FARAWAY: ap.m_nCarMission = MISSION_FOLLOWCAR_CLOSE; break;
                case MISSION_KILLPED_FARAWAY:   ap.m_nCarMission = MISSION_KILLPED_CLOSE;   break;
                case MISSION_DO_DRIVEBY_FARAWAY: ap.m_nCarMission = MISSION_DO_DRIVEBY_CLOSE; break;
                default:
                    ap.m_nCarMission = MISSION_BLOCKCAR_CLOSE;
                    if (veh->UsesSiren()) {
                        veh->vehicleFlags.bSirenOrAlarm = true;
                    }
                    break;
                }
            }
            break;
        }
        case MISSION_RAMCAR_CLOSE: { // 0x41E642
            if (!ap.m_TargetEntity) {
                ap.m_nCarMission = MISSION_CRUISE;
                break;
            }
            if (ap.m_TargetEntity == FindPlayerVehicle()) {
                BackToCruisingIfNoWantedLevel(veh);
            }
            if (FindSwitchDistanceFar(veh) < TargetDist()) {
                ap.m_nCarMission = MISSION_RAMCAR_FARAWAY;
                CCarCtrl::JoinCarWithRoadSystem(veh);
            } else if (veh->GetHasCollidedWith(ap.m_TargetEntity) && Mag3(veh->m_vecMoveSpeed) < 0.04f) {
                SetTempActionFor(ap, TEMPACT_REVERSE, 800);
            }
            break;
        }
        case MISSION_BLOCKCAR_CLOSE: { // 0x41E88D
            if (!ap.m_TargetEntity) {
                ap.m_nCarMission = MISSION_NONE;
                break;
            }
            if (FindSwitchDistanceFar(veh) < TargetDist()) {
                veh->vehicleFlags.bSirenOrAlarm = false;
                ap.m_nCarMission                = MISSION_BLOCKCAR_FARAWAY;
                veh->m_HornCounter              = 0;
                CCarCtrl::JoinCarWithRoadSystem(veh);
            }
            break;
        }
        case MISSION_FOLLOWCAR_CLOSE:   // 0x41EA63
        case MISSION_KILLPED_CLOSE:     // 0x41EF50
        case MISSION_DO_DRIVEBY_CLOSE: { // 0x41F5B1
            if (!ap.m_TargetEntity) {
                ap.m_nCarMission = MISSION_NONE;
                break;
            }
            if (FindSwitchDistanceFar(veh) < TargetDist()) {
                ap.m_nCarMission = ap.m_nCarMission == MISSION_FOLLOWCAR_CLOSE ? MISSION_FOLLOWCAR_FARAWAY
                                 : ap.m_nCarMission == MISSION_KILLPED_CLOSE   ? MISSION_KILLPED_FARAWAY
                                                                               : MISSION_DO_DRIVEBY_FARAWAY;
                CCarCtrl::JoinCarWithRoadSystem(veh);
            }
            break;
        }
        case MISSION_BOAT_ATTACKPLAYER: { // 0x41EE05
            if (!law()) {
                break;
            }
            if (!FindPlayerVehicle()) {
                ap.m_nCarMission = MISSION_BOAT_CIRCLEPLAYER;
            }
            MellowOutChaseSpeedBoat(veh);
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_BOAT_CIRCLEPLAYER: { // 0x41EE3D
            if (!law()) {
                break;
            }
            if (FindPlayerVehicle()) {
                ap.m_nCarMission = MISSION_BOAT_ATTACKPLAYER;
            }
            ap.m_nCruiseSpeed = 10;
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1: { // 0x41EB46
            if (DestDist() < 1.5f) {
                ap.m_nCarMission = MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2;
            }
            BackToCruisingIfNoWantedLevel(veh); // Como el original
            break;
        }
        case MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2: { // 0x41EBB9
            const auto StopForever = [veh, &ap] {
                TellOccupantsToLeaveCar(veh);
                ap.m_nCruiseSpeed = 0;
                ap.m_nCarMission  = MISSION_STOP_FOREVER;
            };
            const float dist = PlayerDist();
            if (dist < 13.f) {
                StopForever();
            }
            // Como el original: se vuelve a comprobar todo aunque ya se haya parado (puede llamar dos veces)
            bool stop = dist > 70.f || FindPlayerWanted()->m_bEverybodyBackOff;
            if (!stop) {
                if (!law()) {
                    break;
                }
                stop = PlayerWantedLevel() == WANTED_CLEAN || FindPlayerWanted()->PoliceBackOff() || CCullZones::NoPolice();
            }
            if (stop) {
                StopForever();
            }
            break;
        }
        case MISSION_BLOCKPLAYER_FORWARDANDBACK: { // 0x41ED1C
            // Como el original: dirección jugador - coche y longitud mínima 0,001
            const auto& pos = veh->GetPosition();
            const float ddx = FindPlayerCoors().x - pos.x;
            const float ddy = FindPlayerCoors().y - pos.y;
            float len = std::sqrt(ddy * ddy + ddx * ddx);
            if (0.001f > len) {
                len = 0.001f;
            }
            if (!FindPlayerVehicle()) {
                ap.m_nCarMission = MISSION_BLOCKPLAYER_CLOSE;
            } else {
                const float inv = 1.f / len;
                const auto& speedX = FindPlayerSpeed();
                const auto& speedY = FindPlayerSpeed();
                if (inv * ddy * speedY.y + inv * ddx * speedX.x > 0.05f) {
                    ap.m_nCarMission = MISSION_BLOCKPLAYER_CLOSE;
                }
            }
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_ESCORT_LEFT:
        case MISSION_ESCORT_RIGHT:
        case MISSION_ESCORT_REAR:
        case MISSION_ESCORT_FRONT: { // 0x41F771
            if (ap.m_nStraightLineDistance >= 240) {
                break;
            }
            if (FindSwitchDistanceFar(veh) < PlayerDist()) {
                if (!CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, FindPlayerCoors(), true, false)) {
                    ap.m_nCarMission = static_cast<eCarMission>(ap.m_nCarMission + (MISSION_ESCORT_LEFT_FARAWAY - MISSION_ESCORT_LEFT));
                }
            }
            break;
        }
        case MISSION_ESCORT_LEFT_FARAWAY:
        case MISSION_ESCORT_RIGHT_FARAWAY:
        case MISSION_ESCORT_REAR_FARAWAY:
        case MISSION_ESCORT_FRONT_FARAWAY: { // 0x41F694
            if (PlayerDist() < FindSwitchDistanceClose(veh)) {
                ap.m_nCarMission = static_cast<eCarMission>(ap.m_nCarMission - (MISSION_ESCORT_LEFT_FARAWAY - MISSION_ESCORT_LEFT));
            }
            break;
        }
        case MISSION_APPROACHPLAYER_FARAWAY: { // 0x41F033
            if (PlayerDist() < FindSwitchDistanceClose(veh)) {
                ap.m_nCarMission = MISSION_APPROACHPLAYER_CLOSE;
                if (veh->UsesSiren()) {
                    veh->vehicleFlags.bSirenOrAlarm = true;
                }
            }
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_APPROACHPLAYER_CLOSE: { // 0x41F12D
            const float dist = PlayerDist();
            if (FindSwitchDistanceFar(veh) < dist) {
                if (!CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, FindPlayerCoors(), true, false)) {
                    veh->vehicleFlags.bSirenOrAlarm = false;
                    ap.m_nCarMission                = MISSION_APPROACHPLAYER_FARAWAY;
                    veh->m_HornCounter              = 0;
                }
            } else {
                auto& timer = reinterpret_cast<uint16&>(veh->m_nCopsInCarTimer);
                if (FindPlayerVehicle() && Mag3(FindPlayerVehicle()->m_vecMoveSpeed) < 0.05f) {
                    timer = static_cast<uint16>(timer + Ftol(CTimer::GetTimeStep() * 0.02f * 1000.f)); // Como el original: (paso * 0,02) * 1000
                } else {
                    timer = 0;
                }
                // Como el original: 20000 para la moto de policía y 2500 para el resto
                const float limit = veh->m_nModelIndex == MODEL_COPBIKE ? 20'000.f : 2'500.f;
                bool handled = false;
                if (!FindPlayerVehicle() || FindPlayerVehicle()->IsUpsideDown() || static_cast<float>(timer) > limit) {
                    if (law() && dist < 10.f) {
                        TellAndStop();
                        handled = true;
                    }
                }
                if (!handled && veh->m_nModelIndex == MODEL_COPBIKE && veh->m_pDriver) {
                    auto* const primary = veh->m_pDriver->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY);
                    const auto* const seq = primary && primary->GetTaskType() == TASK_COMPLEX_SEQUENCE ? static_cast<CTaskComplexSequence*>(primary) : nullptr;
                    if (!seq || !seq->Contains(TASK_COMPLEX_ENTER_CAR_AS_DRIVER) && !seq->Contains(TASK_SIMPLE_GANG_DRIVEBY)) {
                        // Como el original: secuencia creada primero y tareas añadidas una a una (orden del pool)
                        auto* const newSeq = new CTaskComplexSequence; // sin {}: el EXE no escribe el relleno
                        newSeq->AddTask(new CTaskComplexEnterCarAsDriver{ veh });
                        newSeq->AddTask(new CTaskSimpleGangDriveBy{ FindPlayerPed(), nullptr, 50.f, 50, eDrivebyStyle::AI_ALL_DIRN, false });
                        CEventScriptCommand event{ TASK_PRIMARY_PRIMARY, newSeq, false };
                        veh->m_pDriver->GetEventGroup().Add(static_cast<CEvent*>(&event), false);
                        veh->m_pDriver->SetCurrentWeapon(WEAPON_PISTOL); // Como el original: 22
                    }
                }
            }
            if (law()) {
                MellowOutChaseSpeed(veh);
            }
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        default: { // 0x41F880
            if (!law()) {
                break;
            }
            if (PlayerWantedLevel() <= WANTED_CLEAN || CCullZones::NoPolice()) {
                break;
            }
            const auto& pos = veh->GetPosition();
            if (std::abs(FindPlayerCoors().x - pos.x) > 10.f || std::abs(FindPlayerCoors().y - pos.y) > 10.f) { // 0x41FA11
                ap.m_nCruiseSpeed = static_cast<uint8>(FindPoliceCarSpeedForWantedLevel(veh));
                if (veh->GetStatus() != STATUS_GHOST) {
                    veh->SetStatus(STATUS_PHYSICS);
                }
                if (veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT) {
                    ap.m_nCarMission = FindPoliceBoatMissionForWantedLevel();
                } else if (veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) {
                    ap.m_nCarMission = MISSION_APPROACHPLAYER_FARAWAY; // Como el original: en línea
                } else {
                    ap.m_nCarMission = FindPoliceCarMissionForWantedLevel();
                }
                ap.m_nTempAction      = TEMPACT_NONE;
                ap.m_nCarDrivingStyle = DRIVING_STYLE_AVOID_CARS;
            } else if (ap.m_nCarMission == MISSION_CRUISE) {
                veh->SetStatus(STATUS_PHYSICS); // Como el original: sin mirar STATUS_GHOST
                TellAndStop();
            }
            break;
        }
        }
        break;
    }
    case STATUS_ABANDONED:
    case STATUS_WRECKED: {
        ap.m_nCruiseSpeed = 0;
        ap.m_nCarMission  = MISSION_NONE;
        break;
    }
    }

    // 0x41FA92 - Como el original: CCullZones::PoliceAbandonCars
    if (law() && PlayerWantedLevel() >= WANTED_LEVEL_1 && CCullZones::PoliceAbandonCars()) {
        TellOccupantsToLeaveCar(veh);
        ap.m_nCruiseSpeed = 0;
        ap.m_nCarMission  = MISSION_NONE;
    }

    // 0x41FADB
    const auto& ms             = veh->m_vecMoveSpeed;
    const float vehMvSpeed2DSq = ms.x * ms.x + ms.y * ms.y;
    if (vehMvSpeed2DSq > std::bit_cast<float>(0x3B23D70Bu)) { // Como el original: 0,0025 del EXE (no el más cercano), estricto
        ap.m_nTimeSwitchedToRealPhysics = CTimer::GetTimeInMS();
        ap.m_nTimeToStartMission        = CTimer::GetTimeInMS();
    }

    // 0x41FB1B - Como el original: solo sin acción temporal, con misión que avanza y velocidad de crucero
    if (ap.m_nTempAction == TEMPACT_NONE
        && ap.m_nCarMission != MISSION_NONE
        && ap.m_nCarMission != MISSION_STOP_FOREVER
        && ap.m_nCarMission != MISSION_BLOCKPLAYER_HANDBRAKESTOP
        && ap.m_nCruiseSpeed != 0
        && !notsa::contains({ MISSION_PROTECTION_REAR, MISSION_PROTECTION_FRONT, MISSION_ESCORT_LEFT, MISSION_ESCORT_RIGHT, MISSION_ESCORT_REAR, MISSION_ESCORT_FRONT, MISSION_FOLLOWCAR_CLOSE }, ap.m_nCarMission)
    ) {
        if (CTimer::GetTimeInMS() - veh->m_nLastCollisionTime > 500) {
            ap.m_nTimeToStartMission = CTimer::GetTimeInMS();
        }
        if (vehMvSpeed2DSq < 0.000144f) { // 0x41FBB6
            const auto style = ap.m_nCarDrivingStyle;
            // Como el original: ((semilla & 15) + 40) * 500 y resta con signo
            const int32 updateInterval = style == DRIVING_STYLE_STOP_FOR_CARS || style == DRIVING_STYLE_STOP_FOR_CARS_IGNORE_LIGHTS
                ? ((veh->m_nRandomSeed & 0xF) + 40) * 500
                : 1000;
            if (static_cast<int32>(CTimer::GetTimeInMS() - ap.m_nTimeToStartMission) > updateInterval) {
                ap.m_ucTempActionMode = static_cast<uint8>(CTimer::GetTimeInMS() >= ap.m_LastUpdateTimeMs + 10'000 // 0x41FC02
                    ? 0
                    : (ap.m_ucTempActionMode + 1) % 4);
                ap.m_LastUpdateTimeMs = CTimer::GetTimeInMS();

                // Como el original: bMadDriver (no bUsedForReplay) y 700 ms
                if (ap.m_nCarMission == MISSION_CRUISE && !veh->IsCreatedBy(MISSION_VEHICLE) && !veh->vehicleFlags.bMadDriver) {
                    SetTempActionFor(ap, TEMPACT_REVERSE, 700);
                } else {
                    switch (ap.m_ucTempActionMode) {
                    case 0: SetTempActionFor(ap, TEMPACT_REVERSE, 1'500);       break;
                    case 1: SetTempActionFor(ap, TEMPACT_REVERSE, 4'000);       break;
                    case 2: SetTempActionFor(ap, TEMPACT_REVERSE_LEFT, 2'500);  break;
                    case 3: SetTempActionFor(ap, TEMPACT_REVERSE_RIGHT, 2'500); break;
                    }
                }

                ap.m_nTimeToStartMission = CTimer::GetTimeInMS();

                if (veh->IsCreatedBy(RANDOM_VEHICLE)) { // 0x41FCF2
                    switch (style) {
                    case DRIVING_STYLE_STOP_FOR_CARS:
                    case DRIVING_STYLE_SLOW_DOWN_FOR_CARS:
                    case DRIVING_STYLE_STOP_FOR_CARS_IGNORE_LIGHTS:
                    case DRIVING_STYLE_DRIVINGMODE_AVOIDCARS_STOPFORPEDS_OBEYLIGHTS:
                        ap.m_nCarDrivingStyle = DRIVING_STYLE_AVOID_CARS;
                        break;
                    }
                }

                if (veh->GetStatus() == STATUS_SIMPLE) { // 0x41FD18
                    veh->SetStatus(STATUS_PHYSICS);
                }

                veh->PlayCarHorn();
            }
        }
    }

    if ((veh->m_nRandomSeed & 7) == 0) { // 0x41FD3C
        if (   CTimer::GetTimeInMS() - ap.m_nTimeSwitchedToRealPhysics > 30'000
            && CTimer::GetPreviousTimeInMS() - ap.m_nTimeSwitchedToRealPhysics <= 30'000 // Como el original: <=
            && ap.m_nCarMission == MISSION_CRUISE
            && !CTrafficLights::ShouldCarStopForBridge(veh)
        ) {
            CCarCtrl::SwitchVehicleToRealPhysics(veh);
            ap.m_nCarDrivingStyle = DRIVING_STYLE_AVOID_CARS;
            SetTempActionFor(ap, TEMPACT_REVERSE, 400);
        }
    }

    if (law()) { // 0x41FDA1
        if (ap.m_nCarMission == MISSION_RAMPLAYER_FARAWAY || ap.m_nCarMission == MISSION_RAMPLAYER_CLOSE) {
            if (FindPlayerVehicle() && FindPlayerVehicle()->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) {
                ap.m_nCarMission = MISSION_BLOCKPLAYER_FARAWAY;
            }
        }
    }

    if (veh->m_matrix->GetUp().z < -0.7f) { // 0x41FDEA - Como el original: espera (no marcha atrás), sin comprobar la matriz
        SetTempActionFor(ap, TEMPACT_WAIT, 1'000);
    }

    if (   (ap.m_nTempAction == TEMPACT_NONE || ap.m_nTempAction == TEMPACT_BOOST_USE_STEERING_ANGLE)
        && notsa::contains({ MISSION_RAMPLAYER_FARAWAY, MISSION_RAMPLAYER_CLOSE, MISSION_BLOCKPLAYER_FARAWAY, MISSION_BLOCKPLAYER_CLOSE, MISSION_APPROACHPLAYER_FARAWAY, MISSION_APPROACHPLAYER_CLOSE }, ap.m_nCarMission)
        && FindPlayerVehicle()
    ) { // 0x41FE46
        CVector plyrVehFwd = FindPlayerVehicle()->m_matrix->GetForward();
        plyrVehFwd.z = 0.f;
        plyrVehFwd.Normalise();
        CVector thisVehFwd = ExeForward(*veh);
        thisVehFwd.z = 0.f;
        thisVehFwd.Normalise();
        // Como el original: producto (y + x) + z de los forwards normalizados
        const float fwdDot = (thisVehFwd.y * plyrVehFwd.y + thisVehFwd.x * plyrVehFwd.x) + thisVehFwd.z * plyrVehFwd.z;

        const float plyrSpeed = Mag3(FindPlayerSpeed());
        if (Mag3(veh->m_vecMoveSpeed) < plyrSpeed && Mag3(FindPlayerSpeed()) > 0.1f) { // 0x41FEE8
            const auto& plyrMat = *FindPlayerVehicle()->m_matrix;
            const auto& vehMat  = *veh->m_matrix;
            if (plyrMat.GetForward().y * vehMat.GetForward().y + vehMat.GetForward().x * plyrMat.GetForward().x > 0.f) { // 0x41FF25
                const auto& pos = veh->GetPosition();
                const float ax  = pos.x - FindPlayerCoors().x;
                const float ay  = pos.y - FindPlayerCoors().y;
                const float sx  = FindPlayerSpeed().x;
                const float sy  = FindPlayerSpeed().y;
                const float dotS   = ay * sy + sx * ax;
                const float speed2 = std::sqrt(sy * sy + sx * sx);
                const float dist2  = std::sqrt(ay * ay + ax * ax);
                if (speed2 * dist2 * 0.5f < dotS) { // 0x41FF9F
                    if (Mag3(FindPlayerCoors() - veh->GetPosition()) > 12.f) { // 0x42002A
                        SetTempActionFor(ap, TEMPACT_WAIT, 250);
                    }
                    if (   Mag3(FindPlayerCoors() - veh->GetPosition()) < 20.f
                        && fwdDot > 0.8f
                        && Mag3(FindPlayerSpeed()) > 0.6f
                        && Mag3(FindPlayerSpeed()) - 0.2f > Mag3(veh->m_vecMoveSpeed)
                    ) { // 0x420087
                        SetTempActionFor(ap, TEMPACT_BOOST_USE_STEERING_ANGLE, 250);
                        const CVector target = FindPlayerCoors() + FindPlayerSpeed() * 120.f;
                        const auto&   vpos   = veh->GetPosition();
                        const float   toTgt  = CGeneral::GetATanOfXY(target.x - vpos.x, target.y - vpos.y);
                        const float   fwdAng = CGeneral::GetATanOfXY(veh->m_matrix->GetForward().x, veh->m_matrix->GetForward().y);
                        // Como el original: ajuste a [-pi, pi] en línea con las constantes del EXE (sin LimitRadianAngle)
                        const float pi = std::bit_cast<float>(0x40490FDBu), minusPi = std::bit_cast<float>(0xC0490FDBu), twoPi = std::bit_cast<float>(0x40C90FDBu);
                        float steer = toTgt - fwdAng;
                        if (steer < minusPi) {
                            do {
                                steer += twoPi;
                            } while (steer < minusPi);
                        }
                        if (steer > pi) {
                            do {
                                steer -= twoPi;
                            } while (steer > pi);
                        }
                        const float maxSteer = CCarCtrl::FindMaxSteerAngle(veh);
                        const float minSteer = -maxSteer;
                        if (steer < minSteer) {
                            steer = minSteer;
                        }
                        if (steer > maxSteer) {
                            steer = maxSteer;
                        }
                        veh->m_fSteerAngle = steer; // 0x42025C
                    }
                }
            }
        }

        if (   fwdDot < -0.8f // 0x420262
            && Mag3(FindPlayerSpeed()) > 0.3f
            && Mag3(FindPlayerCoors() - veh->GetPosition()) < 45.f
        ) {
            const CVector right = ExeRight(*veh);
            const float   rdot  = Dot3(FindPlayerCoors() - veh->GetPosition(), right);
            if (Mag3(veh->m_vecMoveSpeed) > 0.5f) { // 0x420344
                if (std::abs(rdot) < 15.f) {
                    ap.m_nTempAction = rdot < 0.f ? TEMPACT_HANDBRAKETURNLEFT : TEMPACT_HANDBRAKETURNRIGHT;
                }
                ap.m_nTempActionTime = CTimer::GetTimeInMS() + 2'000;
            }
        }
    }

    //> 0x42039C - Como el original: rand del EXE (CGeneral::GetRandomNumber)
    if (veh->vehicleFlags.bSirenOrAlarm && static_cast<uint8>(CGeneral::GetRandomNumber() ^ veh->m_nRandomSeed) == 0xAD) {
        veh->m_HornCounter = 45;
    }

    //> 0x4203C1 - Como el original: si la diferencia es menor que el paso se fija el objetivo
    {
        const float targetSpeedMult = ap.m_nCarMission == MISSION_CRUISE
            ? CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes(ap.field_41)
            : 1.f;
        const float step = CTimer::GetTimeStep() * 0.01f;
        if (std::abs(ap.m_SpeedMult - targetSpeedMult) < step) {
            ap.m_SpeedMult = targetSpeedMult;
        } else if (targetSpeedMult < ap.m_SpeedMult) {
            ap.m_SpeedMult = ap.m_SpeedMult - step;
        } else {
            ap.m_SpeedMult = step + ap.m_SpeedMult;
        }
    }

    //> 0x420439 - Handle player changing water/land vehicles while being chased
    if (law() && PlayerWantedLevel() > WANTED_CLEAN) {
        if (FindPlayerVehicle()) {
            const bool plyrOnLand = FindPlayerVehicle()->GetVehicleAppearance() == VEHICLE_APPEARANCE_AUTOMOBILE
                                 || FindPlayerVehicle()->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE;
            const bool plyrInBoat = FindPlayerVehicle()->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT;
            if (plyrOnLand) {
                if (veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT) {
                    SetTempActionFor(ap, TEMPACT_WAIT, 1'000);
                }
            } else if (plyrInBoat) {
                if (veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_AUTOMOBILE || veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) {
                    SetTempActionFor(ap, TEMPACT_WAIT, 1'000);
                }
            }
        } else if (FindPlayerPed()->GetIntelligence()->GetTaskSwim()) {
            if (veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_AUTOMOBILE || veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) {
                SetTempActionFor(ap, TEMPACT_WAIT, 1'000);
            }
        } else if (veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT) {
            SetTempActionFor(ap, TEMPACT_WAIT, 1'000);
        }
    }

    //> 0x420553 - Como el original: cuenta todos los peatones de banda/camello (tipos 7 a 17) a menos de 10
    if ((CTimer::GetFrameCounter() & 0xF) == 14 && law()) {
        const auto status = veh->GetStatus();
        if ((status == STATUS_SIMPLE || status == STATUS_PHYSICS) && ap.m_nCarMission == MISSION_CRUISE && veh->IsSubAutomobile() && CPopCycle::m_bCurrentZoneIsGangArea) {
            int32 numGangPeds = 0;
            auto* const pool = GetPedPool();
            for (int32 i = pool->GetSize() - 1; i >= 0; i--) {
                auto* const ped = pool->GetAt(i);
                if (!ped) {
                    continue;
                }
                const auto pedType = static_cast<int32>(ped->m_nPedType);
                if (pedType < static_cast<int32>(PED_TYPE_GANG1) || pedType > static_cast<int32>(PED_TYPE_DEALER)) {
                    continue;
                }
                if (Mag3(veh->GetPosition() - ped->GetPosition()) < 10.f) {
                    numGangPeds++;
                }
            }
            ap.m_nCruiseSpeed = static_cast<uint8>(std::max(10 - numGangPeds, 1));
        }
    }
}
