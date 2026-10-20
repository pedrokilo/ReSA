/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAutomobile
 * Reconstrucción: COMPLETA — 103/103 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 103/103 entradas.
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

#include <bit> // popcnt

#include "Automobile.h"
#include "X87Math.h"
#include "ModelIndices.h"
#include "WaterCannons.h"
#include "Buoyancy.h"
#include "Skidmarks.h"
#include "Shadows.h"
#include "CarCtrl.h"
#include "Glass.h"
#include "ModelIndices.h"
#include "InterestingEvents.h"
#include "VehicleRecording.h"
#include "EventDanger.h"

#include <Tasks/TaskTypes/TaskSimpleGangDriveBy.h>

#include <FireManager.h>

constexpr size_t NUM_AUTOMOBILE_SUSP_LINES  = (size_t)MAX_CARWHEELS;
constexpr size_t NUM_RHINO_EXTRA_SUSP_LINES = (size_t)MAX_CARWHEELS * 2u; // 8
constexpr size_t NUM_RHINO_SUSP_LINES       = NUM_AUTOMOBILE_SUSP_LINES + NUM_RHINO_EXTRA_SUSP_LINES; // 8
constexpr size_t MAX_NUM_SUSP_LINES         = std::max(NUM_AUTOMOBILE_SUSP_LINES, NUM_RHINO_SUSP_LINES); // 12
auto& aAutomobileColPoints = StaticRef<std::array<CColPoint, MAX_NUM_SUSP_LINES>>(0xC1BFF8);

static constexpr CVector PACKER_COL_PIVOT = CVector(0.0f, 0.0f, 2.0f);

// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE (x: up, forward, right; y y z: up, right, forward)
static CVector G1Multiply3x3(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y,
    };
}

// Como el original: MultiplyMatrixWithVector (0x59C890), igual que el anterior y sumando al final la posicion
static CVector G1MultiplyMatrixWithVector(const CMatrix& m, const CVector& v) {
    return CVector{
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}
static constexpr float CAR_BALANCE_MULT = 0.08f;

static constexpr CVector TANK_SHOT_DOOM_POS(0.0f, -1.394f, 2.296f);
static constexpr CVector TANK_SHOT_DOOM_DEFAULT_TARGET(0.0f, 2.95f, 2.97f);
static constexpr CVector TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET = TANK_SHOT_DOOM_DEFAULT_TARGET - TANK_SHOT_DOOM_POS;

static constexpr uint32 TIGER_GUNFIRE_RATE = 60;
static constexpr CVector TIGER_GUN_POS(0.0f, 0.5f, 0.2f); // 0xC1C208

// Como el original: Multiply3x3 (0x59C790) con el orden de sumas del EXE (x: up, forward, right; y y z: up, right, forward)
static CVector G2Multiply3x3(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y,
    };
}

// Como el original: CMatrix::SetRotate (0x59B120) deja la posicion a cero y el codigo de CAutomobile SUMA la posicion guardada
// (fld pos; fadd guardada; fstp), que no es una copia exacta cuando la posicion es -0.0f. Usado en 0x6AF910, 0x6AFA20, 0x6A6AE0,
// 0x6B1600 y 0x6A9D70 (el equivalente inlinado de CDoor::UpdateFrameMatrix).
static void G2SetRotateAddPos(CMatrix& mat, const CVector& rot) {
    const CVector saved = mat.GetPosition();
    mat.SetRotate(rot.x, rot.y, rot.z);
    CVector& pos = mat.GetPosition();
    pos.x = pos.x + saved.x;
    pos.y = pos.y + saved.y;
    pos.z = pos.z + saved.z;
}

// Como el original: orien[door.m_axis] = door.m_angle; SetRotate(orien) y suma de la posicion (el eje es un indice de 0 a 2)
static void G2UpdateDoorFrameMatrix(const CDoor& door, CMatrix& mat) {
    CVector orien{ 0.f, 0.f, 0.f };
    orien[door.m_axis] = door.m_angle;
    G2SetRotateAddPos(mat, orien);
    mat.UpdateRW();
}

// Como el original (0x6A884D..0x6A886E y 0x6A887C..0x6A889C): fild de CTimer::m_snTimeInMilliseconds (entero con signo), + 2^32 (0x858C54)
// si es negativo, + constante flotante en el x87 (24 bits por operacion) y _ftol2 (truncacion a 64 bits; se guarda el dword bajo)
static uint32 G2TimeMsPlusFloat(float addend) {
    return AutomobileTimeMsPlusFloatCpp(CTimer::GetTimeInMS(), addend);
}

void CAutomobile::InjectHooks()
{
    RH_ScopedVirtualClass(CAutomobile, 0x871120, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6B0A90);
    RH_ScopedInstall(Destructor, 0x6A61E0);

    RH_ScopedInstall(SetupModelNodes, 0x6A0770);
    RH_ScopedInstall(HydraulicControl, 0x6A07A0);
    RH_ScopedInstall(UpdateMovingCollision, 0x6A1460);
    RH_ScopedInstall(ProcessBuoyancy, 0x6A8C00);
    RH_ScopedInstall(ProcessHarvester, 0x6A9680);
    RH_ScopedInstall(TankControl, 0x6AE850);
    RH_ScopedInstall(DoSoftGroundResistance, 0x6A4AF0);
    RH_ScopedInstall(ProcessCarWheelPair, 0x6A4EC0);
    RH_ScopedInstall(PlaceOnRoadProperly, 0x6AF420);
    RH_ScopedInstall(RcbanditCheck1CarWheels, 0x6B3F70);
    RH_ScopedInstall(RcbanditCheckHitWheels, 0x6B45E0);
    RH_ScopedInstall(FireTruckControl, 0x729B60);
    RH_ScopedInstall(SetHeliOrientation, 0x6A2450);
    RH_ScopedInstall(ClearHeliOrientation, 0x6A2460);
    RH_ScopedInstall(ReduceHornCounter, 0x6A29A0);
    RH_ScopedInstall(FixTyre, 0x6A3580);
    RH_ScopedInstall(SetTaxiLight, 0x6A3740);
    RH_ScopedInstall(SetAllTaxiLights, 0x6A3760);
    RH_ScopedInstall(GetMovingCollisionOffset, 0x6A2150);
    RH_ScopedInstall(TellHeliToGoToCoors, 0x6A2390);
    RH_ScopedInstall(TellPlaneToGoToCoors, 0x6A2470);
    RH_ScopedInstall(SetTotalDamage, 0x6A27F0);
    RH_ScopedInstall(CustomCarPlate_BeforeRenderingStart, 0x6A2F00);
    RH_ScopedInstall(CustomCarPlate_AfterRenderingStop, 0x6A2F30);
    RH_ScopedInstall(GetAllWheelsOffGround, 0x6A2F70);
    RH_ScopedInstall(FixDoor, 0x6A35A0);
    RH_ScopedInstall(FixPanel, 0x6A3670);
    RH_ScopedInstall(PlayHornIfNecessary, 0x6A3820);
    RH_ScopedInstall(SetBusDoorTimer, 0x6A3860);
    RH_ScopedInstall(ProcessAutoBusDoors, 0x6A38A0);
    RH_ScopedInstall(BoostJumpControl, 0x6A3A60);
    RH_ScopedInstall(StopNitroEffect, 0x6A3E60);
    RH_ScopedInstall(NitrousControl, 0x6A3EA0);
    RH_ScopedInstall(KnockPedOutCar, 0x6A44C0);
    RH_ScopedInstall(PopBootUsingPhysics, 0x6A44D0);
    RH_ScopedInstall(CloseAllDoors, 0x6A4520);
    RH_ScopedInstall(GetCarRoll, 0x6A6010);
    RH_ScopedInstall(GetCarPitch, 0x6A6050);
    RH_ScopedInstall(IsInAir, 0x6A6140);
    RH_ScopedInstall(dmgDrawCarCollidingParticles, 0x6A6DC0);
    RH_ScopedInstall(SpawnFlyingComponent, 0x6A8580);
    RH_ScopedInstall(ProcessSwingingDoor, 0x6A9D70);
    RH_ScopedInstall(RemoveBonnetInPedCollision, 0x6AA200);
    RH_ScopedInstall(PopDoor, 0x6ADEF0);
    RH_ScopedInstall(PopPanel, 0x6ADF80);
    RH_ScopedInstall(ScanForCrimes, 0x6ADFF0);
    RH_ScopedInstall(BlowUpCarsInPath, 0x6AF110);
    RH_ScopedInstall(PopBoot, 0x6AF910);
    RH_ScopedInstall(CloseBoot, 0x6AFA20);
    RH_ScopedInstall(SetBumperDamage, 0x6B1350);
    RH_ScopedInstall(SetPanelDamage, 0x6B1480);
    RH_ScopedInstall(SetDoorDamage, 0x6B1600);
    RH_ScopedInstall(TowTruckControl, 0x6A40F0);
    RH_ScopedInstall(ProcessCarOnFireAndExplode, 0x6A7090);
    RH_ScopedInstall(HasCarStoppedBecauseOfLight, 0x44D520);
    RH_ScopedInstall(UpdateWheelMatrix, 0x6AA290);
    RH_ScopedVMTInstall(Render, 0x6A2B10);

    RH_ScopedVMTInstall(PreRender, 0x6AAB50);
    RH_ScopedVMTInstall(Fix, 0x6A3440);
    RH_ScopedVMTInstall(DoBurstAndSoftGroundRatios, 0x6A47F0);
    RH_ScopedVMTInstall(PlayCarHorn, 0x6A3770);
    RH_ScopedVMTInstall(VehicleDamage, 0x6A7650);
    RH_ScopedVMTInstall(GetComponentWorldPosition, 0x6A2210);
    RH_ScopedVMTInstall(IsComponentPresent, 0x6A2250);
    RH_ScopedVMTInstall(GetDooorAngleOpenRatio, 0x6A2270);
    RH_ScopedVMTInstall(GetDooorAngleOpenRatioU32, 0x6A62C0);
    RH_ScopedVMTInstall(IsDoorReady, 0x6A2290);
    RH_ScopedVMTInstall(IsDoorReadyU32, 0x6A6350);
    RH_ScopedVMTInstall(IsDoorFullyOpen, 0x6A22D0);
    RH_ScopedVMTInstall(IsDoorFullyOpenU32, 0x6A63E0);
    RH_ScopedVMTInstall(IsDoorClosed, 0x6A2310);
    RH_ScopedVMTInstall(IsDoorClosedU32, 0x6A6470);
    RH_ScopedVMTInstall(IsDoorMissing, 0x6A2330);
    RH_ScopedVMTInstall(IsDoorMissingU32, 0x6A6500);
    RH_ScopedVMTInstall(IsOpenTopCar, 0x6A2350);
    RH_ScopedVMTInstall(IsRoomForPedToLeaveCar, 0x6A3850);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6B3E90);
    RH_ScopedVMTInstall(GetHeightAboveRoad, 0x6A62B0);
    RH_ScopedVMTInstall(GetNumContactWheels, 0x6A62A0);
    RH_ScopedVMTInstall(Teleport, 0x6A9CA0);
    RH_ScopedVMTInstall(Save, 0x5D47E0, {.enabled = false });
    RH_ScopedVMTInstall(Load, 0x5D2980, {.enabled = false });
    RH_ScopedVMTInstall(ProcessControl, 0x6B1880);
    RH_ScopedVMTInstall(AddMovingCollisionSpeed, 0x6A1ED0);
    RH_ScopedVMTInstall(ProcessAI, 0x6B4800);
    RH_ScopedVMTInstall(ResetSuspension, 0x6A2AE0);
    RH_ScopedVMTInstall(ProcessFlyingCarStuff, 0x6A8500);
    RH_ScopedVMTInstall(DoHoverSuspensionRatios, 0x6A45C0);
    RH_ScopedVMTInstall(ProcessSuspension, 0x6AFB10);
    // todo: fix/review Tow funcs
    RH_ScopedVMTInstall(GetTowHitchPos, 0x6AF1D0);
    RH_ScopedVMTInstall(GetTowBarPos, 0x6AF250);
    RH_ScopedVMTInstall(SetTowLink, 0x6B4410);
    RH_ScopedVMTInstall(BreakTowLink, 0x6A4400);
    RH_ScopedVMTInstall(FindWheelWidth, 0x6A6090);

    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6A65D0);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6ACE70);
    RH_ScopedVMTInstall(ProcessControlCollisionCheck, 0x6A29C0);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6AD690);
    RH_ScopedVMTInstall(OpenDoor, 0x6A6AE0);
    RH_ScopedVMTInstall(BlowUpCar, 0x6B3780);
    RH_ScopedVMTInstall(BlowUpCarCutSceneNoExtras, 0x6B3BB0);
}

// 0x6B0A90
CAutomobile::CAutomobile(int32 modelIndex, eVehicleCreatedBy createdBy, bool setupSuspensionLines) :
    CVehicle(createdBy),
    m_damageManager{ 0.5f }
{
    m_nVehicleType    = VEHICLE_TYPE_AUTOMOBILE;
    m_nVehicleSubType = VEHICLE_TYPE_AUTOMOBILE;

    m_fBurnTimer    = 0.0f;
    // Como el original (0x6B0B43): mov word ptr [esi+86Ah], 0 -> tambien se pone a cero el byte de relleno 0x86B
    std::memset(&m_bDoingBurnout, 0, 2);

    CVehicle::SetModelIndex(modelIndex);
    SetupModelNodes();

    auto& mi = *GetVehicleModelInfo(); // has to be called after `SetModelIndex` has been called.

    m_pHandlingData = &mi.GetHandlingData(); // For now this has to be here, because the model index is not set in the member init list
    m_pFlyingHandlingData = &mi.GetFlyingHandlingData(); // Moved this over here
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    if (handlingFlags.bHydraulicGeom && CGeneral::GetRandomNumber() % 4 == 0) { // 0x6B0BD7
        AddVehicleUpgrade(ModelIndices::MI_HYDRAULICS);
    }

    m_fBrakeCount = 20.0f;
    m_GasPedalAudioRevs = 0.0f;
    m_fIntertiaValue1 = 0.0f;
    m_fIntertiaValue2 = 0.0f;

    // 0x6B0C2E
    mi.ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);

    // 0x6B0C33
    // Como el original: and al,0F0h sobre el byte 0x429 (se borran los 4 bits bajos antes de ponerlos segun el handling)
    vehicleFlags.bIsVan      = false;
    vehicleFlags.bIsBus      = false;
    vehicleFlags.bIsBig      = false;
    vehicleFlags.bLowVehicle = false;

    // 0x6B0C49
    if (m_pHandlingData->m_bIsVan) {
        vehicleFlags.bIsVan = true;
    }
    if (m_pHandlingData->m_bIsBig) {
        vehicleFlags.bIsBig = true;
    }
    if (m_pHandlingData->m_bIsBus) {
        vehicleFlags.bIsBus = true;
    }
    if (m_pHandlingData->m_bIsLow) {
        vehicleFlags.bLowVehicle = true;
    }

    // Como el original: angulos de las puertas leidos del EXE (el PI/HALF_PI de common.h son imprecisos: 3.14159f)
    const float kCtorPi     = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
    const float kCtorHalfPi = std::bit_cast<float>(0x3FC90FDBu);
    const float kCtorPi04   = std::bit_cast<float>(0x3FA0D97Cu); // 0.4 * PI
    const float kCtorPi03   = std::bit_cast<float>(0x3F71463Bu); // 0.3 * PI
    const float kCtorPi01   = std::bit_cast<float>(0x3EA0D97Cu); // 0.1 * PI

    // 0x6B0CA8
    // Deal with front doors
    if (vehicleFlags.bIsBus) {
        m_doors[DOOR_LEFT_FRONT].Init(-kCtorHalfPi, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
        m_doors[DOOR_RIGHT_FRONT].Init(kCtorHalfPi, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
    } else {
        m_doors[DOOR_LEFT_FRONT].Init(-kCtorPi04, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
        m_doors[DOOR_RIGHT_FRONT].Init(kCtorPi04, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
    }

    // 0x6B0CF0
    // Deal with rear doors
    if (modelIndex == MODEL_RHINO) { // For Rhino just hide it
        for (auto door : { DOOR_LEFT_REAR, DOOR_RIGHT_REAR }) {
            // NOTE(pirulax): Also search for `Rhino` inside `CAutomobile::ProcessEntityCollision`
            m_doors[door].SetExtraWheelPositions(1.f, 1.f, 1.f, 1.f);
        }

        // Hide both
        for (auto comp : { CAR_WHEEL_LM, CAR_WHEEL_RM }) {
            rwObjectSetFlags(GetFirstObject(m_aCarNodes[comp]), eAtomicComponentFlag::ATOMIC_NONE);
        }
    } else if (vehicleFlags.bIsVan) { // 0x6B0D57
        m_doors[DOOR_LEFT_REAR].Init(-kCtorHalfPi, 0.0f, DOOR_AXIS_X, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
        m_doors[DOOR_RIGHT_REAR].Init(kCtorHalfPi, 0.0f, DOOR_AXIS_NEG_X, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
    } else {
        m_doors[DOOR_LEFT_REAR].Init(-kCtorPi04, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
        m_doors[DOOR_RIGHT_REAR].Init(kCtorPi04, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
    }

    // 0x6B0DBE
    // Bonnet
    if (m_pHandlingData->m_bReverseBonnet) {
        m_doors[DOOR_BONNET].Init(-kCtorPi03, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_X, DOOR_EXTRA_LOW_GRAVITY);
    } else {
        m_doors[DOOR_BONNET].Init(kCtorPi03, 0.0f, DOOR_AXIS_Y, DOOR_AXIS_X, DOOR_EXTRA_LOW_GRAVITY);
    }

    // 0x6B0DF4
    // Boot(y)
    if (m_pHandlingData->m_bHangingBoot) {
        m_doors[DOOR_BOOT].Init(-kCtorPi04, 0.0f, DOOR_AXIS_NEG_Z, DOOR_AXIS_X, DOOR_EXTRA_BASED);
    } else if (m_pHandlingData->m_bTailgateBoot) {
        m_doors[DOOR_BOOT].Init(kCtorHalfPi, 0.0f, DOOR_AXIS_Z, DOOR_AXIS_X, DOOR_EXTRA_BASED);
    } else {
        m_doors[DOOR_BOOT].Init(-kCtorPi03, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_X, DOOR_EXTRA_BASED);
    }

    // 0x6B0E4B
    // If model has no doors, hide them all
    if (m_pHandlingData->m_bNoDoors) {
        m_damageManager.SetDoorStatus({ DOOR_LEFT_FRONT, DOOR_RIGHT_FRONT, DOOR_LEFT_REAR, DOOR_RIGHT_REAR }, DAMSTATE_NOTPRESENT);
    }

    // Init this field which is never used
    for (auto& doorRot : DoorRotation) {
        doorRot = CGeneral::GetRandomNumberInRange(-0.15f, 0.15f);
    }

    // 0x6B0EE6
    // Do something with misc components? Not sure..
    if (m_nModelIndex == MODEL_TOWTRUCK && m_aCarNodes[CAR_MISC_B]) {
        if (auto& panelFL = m_panels[FRONT_LEFT_PANEL]; panelFL.m_nFrameId == (uint16)-1) {
            panelFL.SetPanel(CAR_MISC_B, 2, 1.f);
        }
    } else if (m_nModelIndex == MODEL_TRACTOR && m_aCarNodes[CAR_BOOT]) {
        if (auto& panelFL = m_panels[FRONT_LEFT_PANEL]; panelFL.m_nFrameId == (uint16)-1) {
            panelFL.SetPanel(CAR_BOOT, 1, 1.f);
        }
    }

    // 0x6B0F3B
    // Deal with swinging chassis
    if (m_pHandlingData->m_bSwingingChassis) {
        const auto GetAngleMult = [modelIndex] {
            switch (modelIndex) {
            case MODEL_COPCARVG:
            case MODEL_ESPERANT:
                return 0.03f;
            case MODEL_STRETCH:
                return 0.01f;
            }
            return 0.02f;
        };
        m_swingingChassis.Init(GetAngleMult() * kCtorPi, -GetAngleMult() * kCtorPi, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_CHASSIS | DOOR_EXTRA_FIXEDSTATE);
        m_swingingChassis.m_doorState = eDoorState::DOOR_HIT_MAX_END;
    } else if (modelIndex == MODEL_FIRELA) {
        m_swingingChassis.Init(kCtorPi01, -kCtorPi01, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_FIXEDSTATE | DOOR_EXTRA_FIRETRUCK);
        m_swingingChassis.m_doorState = eDoorState::DOOR_HIT_MAX_END;
    }

    m_moveForce = CVector();
    m_turnForce = CVector();

    // 0x6B1048
    m_fMass = m_pHandlingData->m_fMass;
    m_fTurnMass = m_pHandlingData->m_fTurnMass;
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    m_fElasticity = 0.05f;
    m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;
    m_fAirResistance = GetDefaultAirResistance();

    // 0x6B10B5
    m_fRotationBalance        = 0.0f;
    m_fAircraftGoToHeading    = 0.0f;
    m_nBusDoorTimerEnd        = 0;
    m_nBusDoorTimerStart      = 0;
    m_fSteerAngle             = 0.0f;
    m_GasPedal               = 0.0f;
    m_BrakePedal             = 0.0f;
    m_pExplosionVictim        = nullptr;
    m_GasPedalAudioRevs          = 0.0f;
    m_PrevSpeed          = 0.0f;
    m_wMiscComponentAngle     = 0;
    m_wMiscComponentAnglePrev = 0;

    // 0x6B1111
    rng::fill(m_WheelCounts, 0.0f);
    rng::fill(m_wheelRotation, 0.0f);
    rng::fill(m_wheelSpeed, 0.0f);
    rng::fill(m_fWheelBurnoutSpeed, 0.0f);
    rng::fill(m_wheelSkidmarkType, eSkidmarkType::DEFAULT);
    rng::fill(m_wheelSkidmarkBloodState, false);
    // Como el original (0x6B1155): solo se ponen a cero los 4 bytes de m_wheelSkidmarkBloodState; m_wheelSkidmarkMuddy no se inicializa
    rng::fill(m_WheelStates, WHEEL_STATE_NORMAL);
    rng::fill(m_fWheelsSuspensionCompression, 1.0f);
    rng::fill(m_fWheelsSuspensionCompressionPrev, 1.0f);

    m_nNumContactWheels     = 0;
    m_NumDriveWheelsOnGround       = 0;
    m_NumDriveWheelsOnGroundLastFrame     = 0;
    m_fFrontHeightAboveRoad = 0.0f;
    m_fRearHeightAboveRoad  = 0.0f;
    m_fCarTraction          = 1.0f;
    m_fTireTemperature      = 1.0f;

    // 0x6B119A
    if (setupSuspensionLines) {
        CAutomobile::SetupSuspensionLines();
    }

    SetStatus(STATUS_SIMPLE);
    m_nNumPassengers = 0;

    if (m_nDoorLock == CARLOCK_UNLOCKED) {
        if (IsLawEnforcementVehicle()) {
            m_nDoorLock = CARLOCK_COP_CAR;
        }
    }

    m_fDoomVerticalRotation = 0.0f;
    m_fDoomHorizontalRotation = 0.05f;
    m_fPropRotate = 0.0f;
    m_fForcedOrientation = -1.0f;
    m_harvesterParticleCounter = 0;
    RightDoorOpenForDriveBys = 0.f;
    LeftDoorOpenForDriveBys = 0.f;

    switch (m_nModelIndex) {
    case -2: { // 0x6B120F
        // I don't think this is reachable, as it would crash somewhere above.

        rwObjectSetFlags(GetFirstObject(m_aCarNodes[CAR_WHEEL_LF]), eAtomicComponentFlag::ATOMIC_NONE);

        // Probably an inlined function here, as there was a redundant check here (model == -2)
        CMatrix wheelRFMat{ RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RF]) };
        CMatrix wheelLFMat{ RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LF]) };
        wheelRFMat.SetTranslate({ wheelLFMat.GetPosition().x + 0.1f, 0.f, wheelLFMat.GetPosition().z});
        wheelRFMat.UpdateRW();
        break;
    }
    case MODEL_RHINO: { // 0x6B12D6
        physicalFlags.bExplosionProof = true;
        physicalFlags.bBulletProof = true;
        break;
    }
    }

    std::ranges::fill(m_exhaustNitroFxSystem, nullptr);

    m_vehicleAudio.Initialise(this);
    m_heliDustFxTimeConst = 0.0f;
    m_fireParticleCounter = 0;
}

// 0x6A61E0
CAutomobile::~CAutomobile() {
    FxSystem_c::SafeKillAndClear(m_pFireParticle);
    StopNitroEffect();
    m_vehicleAudio.Terminate();
}

// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE (x: up, forward, right; y y z: up, right, forward)
static CVector G5Multiply3x3(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y,
    };
}

// Como el original: 6B2DCD..6B2DEE: m_fWheelSpinForAudio = r * audio + (1 - r) * rate con r = pow(base, ts) del CRT del EXE (0x822130)
// sin redondear a float (el resultado de la potencia se usa con la precision completa de ST0)
static float G5WheelSpinForAudio(float base, float timeStep, float audio, float rate) {
    return AutomobileWheelSpinAudioCpp(base, timeStep, audio, rate);
}

// 0x6B1880
void CAutomobile::ProcessControl()
{
    uint32 extraHandlingFlags = 0;
    if (vehicleFlags.bUseCarCheats) {
        extraHandlingFlags |= EXTRA_HANDLING_PERFECT;
    }
    CCollisionData* colData = GetColModel()->m_pColData;
    // Como el original: 6B18DE mov word ptr [esi+86Ah],di (escribe m_bDoingBurnout y el byte de relleno 0x86B)
    *reinterpret_cast<uint16*>(&m_bDoingBurnout) = 0;
    autoFlags.bIsBoggedDownInSand = false;
    vehicleFlags.bWarnedPeds = false;
    vehicleFlags.bRestingOnPhysical = false;
    m_vehicleAudio.Service();
    bool bExplodeImmediately = false;
    if (IsSubPlane() || IsSubHeli()) {
        eCarMission carMission = m_autoPilot.m_nCarMission;
        if ((carMission == MISSION_PLANE_CRASH_AND_BURN
            || carMission == MISSION_HELI_CRASH_AND_BURN
            || GetStatus() == STATUS_PLAYER && m_fHealth < 250.0 && m_fireParticleCounter == 2)
            && GetStatus() != STATUS_WRECKED
            && (m_fDamageIntensity > 0.0f && m_vecLastCollisionImpactVelocity.z > 0.0f || !IsInAir())
            && (m_vecMoveSpeed.z >= 0.0f || physicalFlags.bSubmergedInWater))
        {
            bExplodeImmediately = true;
        }
    }

    if (CCheat::IsActive(CHEAT_ALL_TAXIS_NITRO)) {
        if (GetStatus() == STATUS_PLAYER && IsTransportVehicle())
            extraHandlingFlags |= EXTRA_HANDLING_TAXI_BOOST;
    }

    bool bExtraHandlingTaxiBoost = !!(extraHandlingFlags & EXTRA_HANDLING_TAXI_BOOST);
    if (CCheat::IsActive(CHEAT_ALL_CARS_HAVE_NITRO) || bExtraHandlingTaxiBoost) {
        handlingFlags.bNosInst = true;
        m_nNitroBoosts = 101;
    }
    if (vehicleFlags.bIsBus)
        ProcessAutoBusDoors();
    ProcessCarAlarm();
    UpdateClumpAlpha();
    UpdatePassengerList();
    if (m_pDriver) {
        if (!vehicleFlags.bDriverLastFrame && m_nBombOnBoard == BOMB_IGNITION_ACTIVATED) {
            m_wBombTimer = 1000;
            m_pWhoDetonatedMe = m_pWhoInstalledBombOnMe;
            CEntity::SafeRegisterRef(m_pWhoDetonatedMe);
        }
        vehicleFlags.bDriverLastFrame = true;
    }
    else {
        vehicleFlags.bDriverLastFrame = false;
    }

    if (m_nNumPassengers && IsUpsideDown())
        CanPedEnterCar();

    if (ProcessAI(extraHandlingFlags))
        return;

    bool extraPerfectHandling = !!(extraHandlingFlags & EXTRA_HANDLING_PERFECT);
    bool extraHandlingNitros = !!(extraHandlingFlags & EXTRA_HANDLING_NITROS);
    bool extraHandlingWheelsTouchingSand = !!(extraHandlingFlags & EXTRA_HANDLING_WHEELS_TOUCHING_SAND);
    bool extraHandlingTaxiBoost = !!(extraHandlingFlags & EXTRA_HANDLING_TAXI_BOOST);

    bool skipPhysics = false;
    if (GetStatus() == STATUS_ABANDONED || GetStatus() == STATUS_WRECKED) {
        bool isVehicleIdle = false;
        if (!vehicleFlags.bVehicleColProcessed
            && m_vecMoveSpeed == 0.0f
            && m_fWheelsSuspensionCompressionPrev[CAR_WHEEL_REAR_RIGHT] != 1.0f)
        {
            isVehicleIdle = true;
        }

        float forceLimitRadius = 0.003f;
        float torqueLimitRadius = std::bit_cast<float>(0x3A6BEDFBu); // Como el original: 6B1B76 mov dword [esp+24h],3A6BEDFBh (el float siguiente a 0.0009f)
        float movingSpeedLimit = 0.005f;
        if (IsSubPlane()) {
            if (m_fDamageIntensity > 0.0f && m_pDamageEntity && m_pDamageEntity->GetIsTypeVehicle())
                forceLimitRadius = 0.003f;
            else
                forceLimitRadius = 0.009f;
            torqueLimitRadius = 0.003f;
            movingSpeedLimit = 0.1f;
        }
        else if (GetStatus() == STATUS_WRECKED) {
            forceLimitRadius = 0.006f;
            torqueLimitRadius = 0.0015f;
            movingSpeedLimit = 0.015f;
        }

        m_vecForce  = (m_vecForce + m_vecMoveSpeed)  / 2.0f;
        m_vecTorque = (m_vecTorque + m_vecTurnSpeed) / 2.0f;

        bool resetSpeed = true;

        forceLimitRadius *= CTimer::GetTimeStep();
        torqueLimitRadius *= CTimer::GetTimeStep();
        // Como el original: 6B1C4C/6B1C8D fcompp + test ah,1 + jne (C0 tambien se activa con NaN: salta si limite^2 < suma o hay NaN)
        if (!(forceLimitRadius * forceLimitRadius >= m_vecForce.SquaredMagnitude())
            || !(sq(torqueLimitRadius) >= m_vecTorque.SquaredMagnitude())
            || !(m_fMovingSpeed < movingSpeedLimit) // Como el original: 6B1CA6 fcomp + test ah,5 + jp (con NaN tambien salta)
            || m_fDamageIntensity > 0.0f && m_pDamageEntity && m_pDamageEntity->GetIsTypePed())
        {
            resetSpeed = false;
        }
        else if (physicalFlags.bSubmergedInWater) {
            if (DistanceBetweenPointsSquared(TheCamera.GetPosition(), GetPosition()) < sq(50.0f)) {
                resetSpeed = false;
            }
        }
        if (!resetSpeed && !isVehicleIdle) {
            m_nFakePhysics = 0;
        }
        else {
            m_nFakePhysics++;
            // Como el original: 6B1D60..6B1D6F llama a 0x424100 (xor al,al; ret, siempre devuelve false) con la posicion; no tiene efecto
            if (m_nFakePhysics > 10 || isVehicleIdle) {
                if (!isVehicleIdle || m_nFakePhysics > 10)
                    m_nFakePhysics = 10;
                ResetMoveSpeed();
                ResetTurnSpeed();
                skipPhysics = true;
            }
        }

        if (IsRealHeli()
            && m_wheelSpeed[CAR_WHEEL_REAR_LEFT] > 0.0f || IsAmphibiousHeli()
            && !m_nNumContactWheels
            && m_fDamageIntensity <= 0.0f
            || m_nModelIndex == MODEL_VORTEX
            || CCheat::IsActive(CHEAT_CARS_ON_WATER)
            && IsAnyWheelTouchingShallowWaterGround())
        {
            skipPhysics = false;
            m_nFakePhysics = 0;
        }
    }

    for (auto& collisionEntity : m_apWheelCollisionEntity) {
        if (collisionEntity) {
            vehicleFlags.bRestingOnPhysical = true;
            if (!CWorld::bForceProcessControl && collisionEntity->GetIsInSafePosition()) {
                SetWasPostponed(true);
                return;
            }
        }
    }

    if (vehicleFlags.bRestingOnPhysical) {
        skipPhysics = false;
        m_nFakePhysics = 0;
    }

    VehicleDamage(0.0f, eVehicleCollisionComponent::DEFAULT, nullptr, nullptr, nullptr, WEAPON_RAMMEDBYCAR);
    if (GetStatus() == STATUS_PLAYER && ModelIndices::HasWaterCannon(m_nModelIndex)) {
        FireTruckControl(nullptr);
    }
    else {
        switch (m_nModelIndex)
        {
        case MODEL_RHINO:
        case MODEL_RCTIGER:
            TankControl();
            break;
        case MODEL_PACKER:
        case MODEL_DOZER:
        case MODEL_DUMPER:
        case MODEL_CEMENT:
        case MODEL_ANDROM:
        case MODEL_FORKLIFT:
            if (UpdateMovingCollision(-1.0f))
                skipPhysics = false;
            break;
        case MODEL_TOWTRUCK:
        case MODEL_TRACTOR:
            TowTruckControl();
            break;
        default:
            if (handlingFlags.bHydraulicInst)
                HydraulicControl();
            else if ((CCheat::IsActive(CHEAT_PERFECT_HANDLING) || (extraHandlingFlags & EXTRA_HANDLING_TAXI_BOOST))
                && GetStatus() == STATUS_PLAYER
                && m_vecMoveSpeed.SquaredMagnitude() > std::bit_cast<float>(0x3D23D70Bu)) // Como el original: 6B1FF0 fcomp [0x863244] = 0x3D23D70B (el float siguiente a 0.04f)
            {
                BoostJumpControl();
            }
            break;
        }
    }
    if (handlingFlags.bNosInst) {
        NitrousControl(0);
    } else {
        StopNitroEffect();
    }

    if (FindPlayerVehicle() == this && CPad::GetPad()->CarGunJustDown())
        ActivateBomb();

    if (skipPhysics) {
        SkipPhysics();
        vehicleFlags.bAudioChangingGear = false;
        vehicleFlags.bVehicleColProcessed = false;
        m_fTireTemperature = 1.0f;
    }
    else {
        if (!vehicleFlags.bVehicleColProcessed) {
            ProcessControlCollisionCheck(true);
        }
        // Como el original: 6B2131..6B214B leen el flag "ha golpeado una pared" (bit 12 de 0x1C) y bSubmergedInWater ANTES de CPhysical::ProcessControl
        const bool hasHitWall = GetHasHitWall();
        bool oldSubmergedInWater = physicalFlags.bSubmergedInWater;
        bool postCollision = false;
        if (m_fDamageIntensity > 0.0f && m_vecLastCollisionImpactVelocity.z > 0.1f)
            postCollision = true;
        CPhysical::ProcessControl();

        if (m_nModelIndex == MODEL_SKIMMER) {
            auto skimmerModelInfo = CModelInfo::GetModelInfo(MODEL_SKIMMER)->AsVehicleModelInfoPtr();
            auto skimmerHandling = gHandlingDataMgr.GetBoatPointer(static_cast<uint8>(skimmerModelInfo->m_nHandlingId));
            ProcessBoatControl(skimmerHandling, &m_fDoomVerticalRotation, hasHitWall, postCollision);
            physicalFlags.bTouchingWater = physicalFlags.bSubmergedInWater;
        }
        else {
            ProcessBuoyancy();
        }

        if (!oldSubmergedInWater && physicalFlags.bSubmergedInWater) {
            if (m_pDriver && m_pDriver->IsPlayer()) {
                m_pDriver->AsPlayer()->ResetPlayerBreath();
            }
            else {
                for (const auto passenger : GetPassengers()) {
                    if (passenger && passenger->IsPlayer()) {
                        passenger->AsPlayer()->ResetPlayerBreath();
                    }
                }
            }
        }
        if (m_nModelIndex == MODEL_RCBARON)
            ProcessFlyingCarStuff();
        if (!autoFlags.bIsMonsterTruck) {
            for (int32 i = 0; i < 4; i++) {
                float wheelRadius = 1.0f - m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i];
                m_fWheelsSuspensionCompression[i] = (m_fWheelsSuspensionCompression[i] - wheelRadius) / (1.0f - wheelRadius);
            }
        }

        ProcessSuspension();

        CVector contactPoints[4]; // Como el original: sin inicializar (todos se escriben antes de leerse)
        CVector contactSpeeds[4];

        for (int32 i = 0; i < 4; i++) {
            if (!(m_fWheelsSuspensionCompression[i] < 1.0f)) { // Como el original: 6B22C7 fcomp + test ah,5 + jp (con NaN tambien va por la rama de contacto)
                CColLine& colLine = colData->m_pLines[i];
                CColDisk& colDisk = colData->m_pDisks[i];
                if (colData->bUsesDisks) {
                    CVector& point = contactPoints[i];
                    point = colDisk.m_vecCenter;
                    point.z -= colDisk.m_fRadius;
                    point = G5Multiply3x3(GetMatrix(), contactPoints[i]); // Como el original: 6B2382 call 59C790
                }
                else {
                    contactPoints[i] = G5Multiply3x3(GetMatrix(), colLine.m_vecEnd); // Como el original: 6B239F call 59C790
                }
            }
            else {
                m_wheelSkidmarkMuddy[i] = false;
                m_wheelSkidmarkType[i] = static_cast<eSkidmarkType>(g_surfaceInfos.GetSkidmarkType(m_wheelColPoint[i].m_nSurfaceTypeB));
                if (m_wheelSkidmarkType[i] == eSkidmarkType::MUDDY)
                    m_wheelSkidmarkMuddy[i] = true;
                contactPoints[i] = m_wheelColPoint[i].m_vecPoint - GetPosition();
            }
        }

        for (int32 i = 0; i < 4; i++) {
            CPhysical* collisionEntity = m_apWheelCollisionEntity[i];
            contactSpeeds[i] = GetSpeed(contactPoints[i]);
            if (collisionEntity) {
                contactSpeeds[i] -= collisionEntity->GetSpeed(m_vWheelCollisionPos[i]);
                m_apWheelCollisionEntity[i] = nullptr;
            }
        }

        float speedForward = DotProduct(m_vecMoveSpeed, GetForward());

        if (vehicleFlags.bAudioChangingGear
            && m_GasPedal > 0.4f
            && m_BrakePedal < 0.1f
            && speedForward  > 0.15f
            && this == FindPlayerVehicle()
            && CCamera::GetActiveCamera().m_nMode != MODE_1STPERSON)
        {
            if (GetStatus() == STATUS_PLAYER && !m_pHandlingData->m_bIsBus)
            {
                if (m_nBusDoorTimerEnd) {
                    // Como el original: 6B256D..6B2579 fld ts; fmul 0.02f; fmul 1000.0f; call _ftol (no la division de GetTimeStepInMS)
                    uint32 timeStep = (uint32)(CTimer::GetTimeStep() * 0.02f * 1000.0f);
                    if (m_nBusDoorTimerEnd <= timeStep)
                        m_nBusDoorTimerEnd = 0;
                    else
                        m_nBusDoorTimerEnd -= timeStep;
                }
                else {
                    m_nBusDoorTimerEnd = 1000;
                }
            }

            if (IsAnyFrontAndRearWheelTouchingGround()) {
                CVector point = GetForward() * -1.0f;
                // Como el original: 6B2626..6B2635 fcom 2500 + test ah,5 + jnp (con NaN se usa 2500)
                const float turnMassLimited = (m_fTurnMass < 2500.0f) ? m_fTurnMass : 2500.0f;
                CVector force = turnMassLimited * -0.008f * GetUp();
                ApplyTurnForce(force, point);
            }
        }

        float brake = m_pHandlingData->m_fBrakeDeceleration * m_BrakePedal * CTimer::GetTimeStep();

        m_NumDriveWheelsOnGroundLastFrame = m_NumDriveWheelsOnGround;
        m_nNumContactWheels = 0;
        m_NumDriveWheelsOnGround = 0;

        for (int32 i = 0; i < 4; i++) {
            if (!(m_fWheelsSuspensionCompression[i] < 1.0f)) { // Como el original: 6B26D6 fcomp + test ah,5 + jp (con NaN va por la rama de rueda en el aire)
                // Como el original: 6B26EB..6B2715 fld cnt; fsub ts; fcom 0; (> 0 ? v : 0); fst; fcomp 0 + test ah,41h + jne (NaN y <= 0 saltan)
                const float counts = m_WheelCounts[i] - CTimer::GetTimeStep();
                m_WheelCounts[i] = (counts > 0.0f) ? counts : 0.0f;
                if (!(m_WheelCounts[i] > 0.0f))
                    continue;
            }
            else {
                m_WheelCounts[i] = 4.0f;
            }

            m_nNumContactWheels++;

            switch (m_pHandlingData->GetTransmission().m_nDriveType)
            {
            case '4':
                m_NumDriveWheelsOnGround++;
                break;
            case 'F':
                if (i == CAR_WHEEL_FRONT_LEFT || i == CAR_WHEEL_FRONT_RIGHT) {
                    m_NumDriveWheelsOnGround++;
                    break;
                }
                break;
            case 'R':
                if (i == CAR_WHEEL_REAR_LEFT || i == CAR_WHEEL_REAR_RIGHT) {
                    m_NumDriveWheelsOnGround++;
                    break;
                }
            }
        }

        uint8 cheatType = CHEAT_HANDLING_NONE;
        if (handlingFlags.bNosInst && m_fTireTemperature < 0.0f)
            cheatType = CHEAT_HANDLING_NITROS;
        else if (extraPerfectHandling || CCheat::IsActive(CHEAT_PERFECT_HANDLING))
            cheatType = CHEAT_HANDLING_PERFECT;

        float acceleration = 0.0f;
        if (vehicleFlags.bEngineOn && !m_pHandlingData->m_bIsPlane && !m_pHandlingData->m_bIsHeli) {
            acceleration = m_pHandlingData->GetTransmission().CalculateDriveAcceleration(
                m_GasPedal,
                m_nCurrentGear,
                m_fGearChangeCount,
                speedForward,
                &m_fIntertiaValue1,
                &m_fIntertiaValue2,
                m_NumDriveWheelsOnGround,
                cheatType
            );
            acceleration /= m_fVelocityFrequency;
        }

        // Como el original: 6B2803..6B2845: ((carTraction * mult) * G) o (G * mult) con G = [0xC1BFEC] (= 0.0004f * 10, se calcula en 0x853500);
        // despues * 0.25f y / velocidad de frecuencia (no / 250 ni 0.25f / frecuencia)
        float traction;
        if (GetStatus() == STATUS_PHYSICS)
            traction = m_fCarTraction * m_pHandlingData->m_fTractionMultiplier * StaticRef<float>(0xC1BFEC);
        else
            traction = StaticRef<float>(0xC1BFEC) * m_pHandlingData->m_fTractionMultiplier;
        traction *= 0.25f;
        traction /= m_fVelocityFrequency;
        CPlane* vortex = AsPlane();
        if (CCheat::IsActive(CHEAT_PERFECT_HANDLING) || m_nModelIndex == MODEL_VORTEX && vortex->m_fAccelerationBreakStatus == 0.0f)
            traction *= 4.0f;

        if (this != FindPlayerVehicle() && (extraPerfectHandling || CCheat::IsActive(CHEAT_PERFECT_HANDLING))) {
            traction *= 1.2f;
            acceleration *= 1.4f;
            if (extraHandlingNitros || CCheat::IsActive(CHEAT_PERFECT_HANDLING)) {
                traction *= 1.3f;
                acceleration *= 1.4f;
            }
        }

        // Como el original: 6B28D6 fld [0x8D3214] (= 4.0f, global en .data); fcomp 0; jne -> si no es > 0 se salta todo el bloque (incluido el producto por m_fSteerAngle)
        if (StaticRef<float>(0x8D3214) > 0.0f) {
            float steerAngle = 1.0f; // todo: Repeated branch in conditional chain
            bool steerAngleApplied = false;
            if (!(speedForward > 0.01f) || !(m_WheelCounts[CAR_WHEEL_FRONT_LEFT] > 0.0f) && !(m_WheelCounts[CAR_WHEEL_REAR_LEFT] > 0.0f))
                steerAngle = 1.0f;
            else if (GetStatus() != STATUS_PLAYER)
                steerAngle = 1.0f;
            else {
                CColPoint colPoint; // Como el original: 6B2955/6B295F solo se escriben los dos tipos de superficie (sin inicializar el resto)
                colPoint.m_nSurfaceTypeA = SURFACE_WHEELBASE;
                colPoint.m_nSurfaceTypeB = SURFACE_TARMAC;
                float speedRight = DotProduct(m_vecMoveSpeed, GetRight());
                float adhesive = g_surfaceInfos.GetAdhesiveLimit(&colPoint);
                // Como el original: 6B2970..6B2988 ((adhesive * traction) * [0x8D3214]) * 4.0f / (speedForward^2)
                steerAngle = adhesive * traction * StaticRef<float>(0x8D3214) * 4.0f / (speedForward * speedForward);
                steerAngle = (1.0f < steerAngle) ? 1.0f : steerAngle;
                // Como el original: 6B2989..6B2A34. El cociente asin(x) / (steeringLock * 0.017453292f) (asin del CRT del EXE, 0x821E70) se queda sin
                // redondear en ST0 hasta el producto por m_fSteerAngle (fmul [esi+494h]; fstp): todo va en un ayudante x87 (un solo redondeo a float).
                // Se pone a 1.0 si m_fSteerAngle y speedRight van en sentidos opuestos, con el freno de mano o si el cociente supera 1.0.
                const bool steerOpposed = m_fSteerAngle < 0.0f && speedRight > 0.05f
                    || m_fSteerAngle > 0.0f && speedRight < -0.05f
                    || vehicleFlags.bIsHandbrakeOn;
                m_fSteerAngle = AutomobileSteerFactorCpp(steerAngle, m_pHandlingData->m_fSteeringLock, StaticRef<float>(0x8595EC), m_fSteerAngle, steerOpposed);
                steerAngleApplied = true;
            }
            if (!steerAngleApplied)
                m_fSteerAngle *= steerAngle;
        }

        if (!handlingFlags.bProcRearwheelFirst) {
            if (handlingFlags.bSteerRearwheels)
                CAutomobile::ProcessCarWheelPair(
                    CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_FRONT_RIGHT,
                    -999.0f,
                    contactSpeeds,
                    contactPoints,
                    traction,
                    acceleration,
                    brake,
                    true
                );
            else
                CAutomobile::ProcessCarWheelPair(
                    CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_FRONT_RIGHT,
                    m_fSteerAngle,
                    contactSpeeds,
                    contactPoints,
                    traction,
                    acceleration,
                    brake,
                    true
                );
        }

        float rearWheelsSteerAngle = -999.0f;
        if (handlingFlags.bSteerRearwheels)
            rearWheelsSteerAngle = -m_fSteerAngle;
        else if (handlingFlags.bHbRearwheelSteer)
            rearWheelsSteerAngle = m_f2ndSteerAngle;

        CAutomobile::ProcessCarWheelPair(CAR_WHEEL_REAR_LEFT, CAR_WHEEL_REAR_RIGHT,
            rearWheelsSteerAngle,
            contactSpeeds,
            contactPoints,
            traction,
            acceleration,
            brake,
            false
        );

        if (handlingFlags.bProcRearwheelFirst) {
            if (handlingFlags.bSteerRearwheels)
                CAutomobile::ProcessCarWheelPair(
                    CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_FRONT_RIGHT,
                    -999.0f,
                    contactSpeeds,
                    contactPoints,
                    traction,
                    acceleration,
                    brake,
                    true
                );
            else
                CAutomobile::ProcessCarWheelPair(
                    CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_FRONT_RIGHT,
                    m_fSteerAngle,
                    contactSpeeds,
                    contactPoints,
                    traction,
                    acceleration,
                    brake,
                    true
                );
        }

        if (GetStatus() != STATUS_PLAYER) {
            if (CanUpdateHornCounter())
                ReduceHornCounter();
        }
        else if (handlingFlags.bHydraulicInst || CCheat::IsActive(CHEAT_PERFECT_HANDLING) || extraHandlingTaxiBoost)
            ProcessSirenAndHorn(false);
        else
            ProcessSirenAndHorn(true);

        if (m_nModelIndex != MODEL_RCBARON)
            ProcessFlyingCarStuff();
    }
    ProcessCarOnFireAndExplode(bExplodeImmediately);
    if (vehicleFlags.bSirenOrAlarm
        && (CTimer::GetFrameCounter() & 7) == 5
        && UsesSiren()
        && m_nModelIndex != MODEL_MRWHOOP
        && FindPlayerVehicle() == this)
    {
        CCarAI::MakeWayForCarWithSiren(this);
    }

    float suspensionShake = 0.0f;
    float roughnessShake = 0.0f;
    float wheelSpinRate = 0.0f;
    for (int32 i = 0; i < 4; i++) {
        float suspensionChange = m_fWheelsSuspensionCompressionPrev[i] - m_fWheelsSuspensionCompression[i];
        if (suspensionChange > 0.3f && !extraHandlingWheelsTouchingSand && m_vecMoveSpeed.SquaredMagnitude() > std::bit_cast<float>(0x3D23D70Bu)) { // Como el original: 6B2C61 fcomp [0x863244]
            (void)m_damageManager.GetWheelStatus(static_cast<eCarWheel>(i)); // Como el original: 6B2C6E call 6C21B0 (el valor devuelto se ignora)
            if (suspensionChange > suspensionShake)
                suspensionShake = suspensionChange;
        }

        if (m_fWheelsSuspensionCompression[i] < 1.0f && GetStatus() == STATUS_PLAYER) {
            float roughness = (float)g_surfaceInfos.GetRoughness(m_wheelColPoint[i].m_nSurfaceTypeB) * 0.1f;
            roughnessShake = std::max(roughnessShake, roughness);
        }
        uint32 handlingId = m_pHandlingData->m_nVehicleId;
        cTransmission& transmission = gHandlingDataMgr.GetVehiclePointer(handlingId)->GetTransmission();
        if (m_WheelCounts[i] <= 0.0f
            && (m_GasPedal > 0.5f || m_GasPedal < -0.5f)
            && ((i == CAR_WHEEL_FRONT_LEFT || i == CAR_WHEEL_FRONT_RIGHT) && transmission.m_nDriveType != 'R'
                || (i == CAR_WHEEL_REAR_LEFT || i == CAR_WHEEL_REAR_RIGHT) && transmission.m_nDriveType != 'F'))
        {
            wheelSpinRate += CVehicle::WHEELSPIN_INAIR_TARGET_RATE;
        }
        else if (m_WheelStates[i] == WHEEL_STATE_SPINNING) {
            wheelSpinRate += CVehicle::WHEELSPIN_TARGET_RATE;
        }

        m_fWheelsSuspensionCompressionPrev[i] = m_fWheelsSuspensionCompression[i];
        m_fWheelsSuspensionCompression[i] = 1.0f;
    }

    if (m_pHandlingData->GetTransmission().m_nDriveType == '4')
        wheelSpinRate *= 0.25f;
    else
        wheelSpinRate *= 0.5f;

    float riseFallWheelSpinRate = CVehicle::WHEELSPIN_FALL_RATE;
    if (!(wheelSpinRate < m_fWheelSpinForAudio)) // Como el original: 6B2DB2 fcomp + test ah,5 + jp (con NaN tambien sube)
        riseFallWheelSpinRate = CVehicle::WHEELSPIN_RISE_RATE;

    // Como el original: 6B2DCD..6B2DEE (pow del CRT del EXE sin redondear a float, un solo redondeo por operacion)
    m_fWheelSpinForAudio = G5WheelSpinForAudio(riseFallWheelSpinRate, CTimer::GetTimeStep(), m_fWheelSpinForAudio, wheelSpinRate);

    if ((CTimer::GetTimeInMS() & 2047) > 800) { // % 2046
        if (roughnessShake >= 0.29f)
            suspensionShake = 0.0f;
        roughnessShake = 0.0f;
    }

    if (!extraHandlingWheelsTouchingSand
        && (suspensionShake > 0.0f || roughnessShake > 0.0f)
        && GetStatus() == STATUS_PLAYER)
    {
        float speed = m_vecMoveSpeed.SquaredMagnitude();
        if (speed > std::bit_cast<float>(0x3C23D70Bu)) { // Como el original: 6B2E83 fcom [0x86CD7C] (= 0x3C23D70B, el float siguiente a 0.01f)
            speed = sqrt(speed);
            // Como el original: 6B2E94..6B2F5B: (velocidad / masa) * sacudida * 400000 + base, la frecuencia se trunca a entero
            // (_ftol) y el tiempo se divide por esa frecuencia entera (fidiv)
            const float speedPerMass = speed / m_fMass;
            if (suspensionShake > 0.0f) {
                float frequency = speedPerMass * suspensionShake * 400000.0f + 100.0f;
                frequency = std::min(frequency, 250.0f);
                const auto freq = static_cast<uint8>(static_cast<int32>(frequency));
                auto time = static_cast<int16>(static_cast<int32>(CTimer::GetTimeStep() * 20000.0f / static_cast<float>(freq)));
                CPad::GetPad()->StartShake(time, freq, 0);
            }
            else {
                float frequency = speedPerMass * roughnessShake * 400000.0f + 40.0f;
                frequency = std::min(frequency, 150.0f);
                const auto freq = static_cast<uint8>(static_cast<int32>(frequency));
                auto time = static_cast<int16>(static_cast<int32>(CTimer::GetTimeStep() * 5000.0f / static_cast<float>(freq)));
                CPad::GetPad()->StartShake(time, freq, 0);
            }
        }
    }

    vehicleFlags.bAudioChangingGear = false;
    vehicleFlags.bVehicleColProcessed = false;

    // 0x6B2F94
    if (!vehicleFlags.bWarnedPeds)
        CCarCtrl::ScanForPedDanger(this);

    if (handlingFlags.bHydraulicInst && m_vecMoveSpeed.Magnitude() < 0.2f) {
        if ([this] {
            switch (GetStatus()) {
            case STATUS_PHYSICS: {
                const auto CheckWheelIsUsingHydraulics = [this](eCarWheel wheel) {
                    // Como el original: 6B2FEC movsx eax,byte [esi+48Bh]; lea ecx,[eax+eax*4]; shl ecx,3 (sin comprobar -1; .data()[] por si el indice esta fuera de rango)
                    return m_aSpecialHydraulicData.data()[m_vehicleSpecialColIndex].m_aWheelSuspension[wheel] > 0.5f;
                };
                return (CheckWheelIsUsingHydraulics(CAR_WHEEL_FRONT_LEFT) && CheckWheelIsUsingHydraulics(CAR_WHEEL_REAR_LEFT))
                    || (CheckWheelIsUsingHydraulics(CAR_WHEEL_FRONT_RIGHT) && CheckWheelIsUsingHydraulics(CAR_WHEEL_REAR_RIGHT));
            }
            case STATUS_PLAYER: {
                if (!IsDriverAPlayer())
                    return false;
                // Como el original: 6B306C call 609560 una sola vez (edi = pad), luego CarGunLeftRight y CarGunUpDown
                CPad* const driverPad = m_pDriver->AsPlayer()->GetPadFromPlayer();
                return std::fabs((float)driverPad->GetCarGunLeftRight()) > 50.0f
                    && std::fabs((float)driverPad->GetCarGunUpDown()) < 50.0f;
            }
            }
            return false;
        }()) {
            float       turnSpeedForward = DotProduct(m_vecTurnSpeed, GetForward());
            // Como el original: 6B30E0..6B317E. speedTimeStep = pow(0.985f, ts) / (tsf^2 * 5 + 1); speed = pow(speedTimeStep, ts) * tsf - tsf (pow del CRT del
            // EXE, 0x822130) queda sin redondear en ST0 y alimenta los tres productos (speed * (up * -1.0f)) * m_fTurnMass: todo en un ayudante x87.
            CVector force;
            AutomobileHydraulicForceCpp(turnSpeedForward, CTimer::GetTimeStep(), StaticRef<float>(0x8D3210), StaticRef<float>(0x8D320C), &GetUp().x, m_fTurnMass, &force.x);
            CVector point = GetRight() + G5Multiply3x3(GetMatrix(), m_vecCentreOfMass); // Como el original: 6B318E call 59C790
            ApplyTurnForce(force, point);
        }
    }

    if (m_pTowingVehicle) {
        if (GetStatus() == STATUS_IS_TOWED) {
            bool updateTractorLink = false; // TODO: rename this variable later
            // Como el original: 6B324D fcomp [0x86CD7C] (= 0x3C23D70B, el float siguiente a 0.01f)
            if (m_pTowingVehicle->m_vecMoveSpeed != 0.0f || m_vecMoveSpeed.SquaredMagnitude() > std::bit_cast<float>(0x3C23D70Bu)) {
                updateTractorLink = true;
                m_pTowingVehicle->UpdateTractorLink(false, false);
            }
            CVehicle::UpdateTrailerLink(false, false);
            if (m_pTowingVehicle && GetStatus() == STATUS_IS_TOWED) {
                if (updateTractorLink)
                    m_pTowingVehicle->UpdateTractorLink(false, true);
                UpdateTrailerLink(false, true);
            }
        }
        else {
            BreakTowLink();
        }
    }
    else if (m_pVehicleBeingTowed) {
        if (m_pVehicleBeingTowed->GetStatus() == STATUS_IS_TOWED) {
            if (m_pVehicleBeingTowed->m_pTowingVehicle == this) {
                RemoveFromMovingList();
                AddToMovingList();
            }
        }
        else {
            CEntity::ClearReference(m_pVehicleBeingTowed);
        }
    }

    // 0x6B32EF
    if (physicalFlags.bDisableCollisionForce && physicalFlags.bCollidable) {
        ResetMoveSpeed();
        ResetTurnSpeed();
        ResetFrictionMoveSpeed();
        ResetFrictionTurnSpeed();
    }
    else if (!skipPhysics
        && (m_GasPedal == 0.0f || GetStatus() == STATUS_WRECKED)
        && std::fabs(m_vecMoveSpeed.x) < 0.005f // Como el original: 6B33F6/6B340C/6B3422 fcomp [0x858B4C] = 0.005f (no 0.0045f)
        && std::fabs(m_vecMoveSpeed.y) < 0.005f
        && std::fabs(m_vecMoveSpeed.z) < 0.005f)
    {
        if (!(m_fDamageIntensity > 0.0f) || m_pDamageEntity != FindPlayerPed()) { // Como el original: 6B3439 fcomp + test ah,41h + jne (con NaN tambien salta)
            if ((!IsSubPlane() || AsPlane()->m_fAccelerationBreakStatus == 0.0f) &&
                (!IsSubHeli() || AsHeli()->m_fAccelerationBreakStatus == 0.0f))
            {
                if (((float)m_wMiscComponentAngle == 0.0f || m_wMiscComponentAngle == m_wMiscComponentAnglePrev)
                    && !physicalFlags.bSubmergedInWater)
                {
                    if ((!m_pTowingVehicle || m_pTowingVehicle->m_vecMoveSpeed == 0.0f) && DidAnyWheelTouchGroundPrev()) {
                        if (m_nModelIndex == MODEL_VORTEX || CCheat::IsActive(CHEAT_CARS_ON_WATER)
                            && DidAnyWheelTouchShallowWaterGroundPrev())
                        {
                            m_nFakePhysics = 0;
                        }
                        else {
                            ResetMoveSpeed();
                            m_vecTurnSpeed.z = 0.0f;
                        }
                    }
                }
            }
        }
    }

    if (CWeather::Earthquake > 0.0f) {
        // Como el original: 6B362F..6B36B2: z = rand * (1 / 32767) * (Earthquake * 0.5f) + z (se suma a la z actual; escribe x, y y z de vuelta)
        CVector& position = GetPosition();
        const float earthquakeHalf = CWeather::Earthquake * 0.5f;
        const float savedX = position.x, savedY = position.y;
        const float randomZ = static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL * earthquakeHalf + position.z;
        position.x = savedX;
        position.y = savedY;
        position.z = randomZ;
    }

    if (m_nModelIndex == MODEL_COMBINE)
        ProcessHarvester();

    if (m_nModelIndex == MODEL_RHINO && (m_vecMoveSpeed != 0.0f || m_vecTurnSpeed != 0.0f)) {
        m_doors[DOOR_LEFT_REAR].SetExtraWheelPositions(1.0f, 1.0f, 1.0f, 1.0f);
        m_doors[DOOR_RIGHT_REAR].SetExtraWheelPositions(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

// 0x6A1ED0
CVector CAutomobile::AddMovingCollisionSpeed(CVector& point) {
    if (GetStatus() != STATUS_PLAYER && GetStatus() != STATUS_FORCED_STOP) {
        if (GetCreatedBy() != MISSION_VEHICLE || !m_wMiscComponentAngle && !m_wMiscComponentAnglePrev)
            return {};
    }

    const auto angleDiff = (int32)m_wMiscComponentAngle - (int32)m_wMiscComponentAnglePrev;
    if (angleDiff > 100 || angleDiff < -100)
        return {};

    float colAngleMult = 0.0f;
    CVector colPivot{};
    RwFrame* carNodeMisc = nullptr;
    if (ModelIndices::IsDumper(m_nModelIndex))
    {
        carNodeMisc = m_aCarNodes[CAR_MISC_C];
        // Como el original (0x6A1F74..0x6A1F7C): el multiplicador solo se carga si existe el nodo (test eax,eax / je)
        if (carNodeMisc) {
            colAngleMult = CMonsterTruck::DUMPER_COL_ANGLEMULT;
        }
    }
    else if (ModelIndices::IsPacker(m_nModelIndex)) {
        colAngleMult = PACKER_COL_ANGLE_MULT;
        colPivot = PACKER_COL_PIVOT;
    }
    else if (ModelIndices::IsDozer(m_nModelIndex)) {
        carNodeMisc = m_aCarNodes[CAR_MISC_A];
        if (carNodeMisc) {
            colAngleMult = DOZER_COL_ANGLE_MULT;
        }
    }
    else if (ModelIndices::IsAndromada(m_nModelIndex)) {
        carNodeMisc = m_aCarNodes[CAR_MISC_E];
        if (carNodeMisc) {
            colAngleMult = CPlane::ANDROM_COL_ANGLE_MULT;
        }
    }
    else if (ModelIndices::IsForklift(m_nModelIndex)) {
        float rot = float(angleDiff) / CTimer::GetTimeStep() * FORKLIFT_COL_ANGLE_MULT * 2.0f;
        return rot * GetUp();
    }

    if (colAngleMult == 0.0f)
        return {};

    if (carNodeMisc) {
        colPivot = RwFrameGetMatrix(carNodeMisc)->pos;
    }
    CVector rotation(float(angleDiff) / CTimer::GetTimeStep() * colAngleMult, 0.0f, 0.0f);
    // Como el original (0x6A2089 / 0x6A20B0): Multiply3x3 (0x59C790) con su orden de sumas
    CVector pos = G1Multiply3x3(GetMatrix(), rotation);
    CVector pivotPos = G1Multiply3x3(GetMatrix(), colPivot);
    CVector distance = point - pivotPos;
    return CrossProduct(pos, distance);
}

// 0x6B4800
bool CAutomobile::ProcessAI(uint32& extraHandlingFlags) {
    CColModel* colModel = GetColModel();
    CCollisionData* colData = colModel->m_pColData;
    int8 recordingId = this->m_autoPilot.m_vehicleRecordingId;
    m_autoPilot.carCtrlFlags.bHonkAtCar = false;
    m_autoPilot.carCtrlFlags.bHonkAtPed = false;
    if (recordingId >= 0 && !CVehicleRecording::bUseCarAI[recordingId])
        return false;

    eCarMission carMission = m_autoPilot.m_nCarMission;
    // Como el original: 6B4841 y 6B4855 llaman a FindPlayerVehicle dos veces; el nivel de busqueda se lee por FindPlayerPed(-1)->GetWanted() (6B4869..6B4883, no por FindPlayerWanted)
    if (FindPlayerVehicle() && FindPlayerVehicle() != this && FindPlayerPed(-1)->GetWanted()->GetWantedLevel() > eWantedLevel::WANTED_LEVEL_3
        && (carMission == MISSION_RAMPLAYER_FARAWAY
            || carMission == MISSION_BLOCKPLAYER_FARAWAY
            || carMission == MISSION_RAMPLAYER_CLOSE
            || carMission == MISSION_BLOCKPLAYER_CLOSE)
        && FindPlayerSpeed().Magnitude() > 0.3f
    ) {
        extraHandlingFlags |= EXTRA_HANDLING_PERFECT;
        if (FindPlayerSpeed().Magnitude() > 0.4f && m_vecMoveSpeed.Magnitude() < 0.3f) {
            extraHandlingFlags |= EXTRA_HANDLING_NITROS;
        } else {
            if (DistanceBetweenPoints(FindPlayerCoors(), GetPosition()) > 50.0f)
                extraHandlingFlags |= EXTRA_HANDLING_NITROS;
        }
    } else if (m_nModelIndex == MODEL_RCBANDIT && GetStatus() != STATUS_REMOTE_CONTROLLED) {
        extraHandlingFlags |= EXTRA_HANDLING_PERFECT;
    }

    bool extraPerfectHandling = !!(extraHandlingFlags & EXTRA_HANDLING_PERFECT);
    if (extraPerfectHandling || CCheat::IsActive(CHEAT_PERFECT_HANDLING)) {
        m_vecCentreOfMass.z = m_aSuspensionSpringLength[CAR_WHEEL_FRONT_LEFT] * 0.3f - m_fFrontHeightAboveRoad;
    }
    else if (GetStatus() == STATUS_PHYSICS) {
        if (handlingFlags.bHydraulicGeom) {
            if (m_autoPilot.m_nCarMission != MISSION_NONE && colData && static_cast<int8>(colData->m_nNumLines) > 0) { // Como el original: 6B49CC test dl,dl; jle (con signo)
                m_vecCentreOfMass.y = (colData->m_pLines[0].m_vecStart.y + colData->m_pLines[1].m_vecStart.y)  * 0.5f;
                handlingFlags.bNpcNeutralHandl = true;
            }
            else {
                m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
                handlingFlags.bNpcNeutralHandl = false;
            }
        }

        if (handlingFlags.bNpcAntiRoll) {
            // Como el original: 6B4A1B..6B4A65 el resultado va a m_vecCentreOfMass.z del vehiculo (fstp [esi+0ACh]); no se modifica el handling compartido
            m_vecCentreOfMass.z = (colModel->m_boundBox.m_vecMin.z - m_pHandlingData->m_vecCentreOfMass.z) * 0.4f + m_pHandlingData->m_vecCentreOfMass.z;
        }
    }
    else {
        m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    }

    if (GetStatus() != STATUS_ABANDONED && GetStatus() != STATUS_WRECKED
        && GetStatus() != STATUS_PLAYER && GetStatus() != STATUS_REMOTE_CONTROLLED
        && GetStatus() != STATUS_FORCED_STOP && vehicleFlags.bIsLawEnforcer)
    {
        ScanForCrimes();
    }

    if (vehicleFlags.bCanPark
        && !vehicleFlags.bParking
        && GetCreatedBy() != MISSION_VEHICLE
        && m_autoPilot.m_nCarMission == MISSION_CRUISE
        && ((CTimer::GetFrameCounter() + static_cast<uint8>(m_nRandomSeed)) & 0xF) == 0)
    {
        if (m_nModelIndex != MODEL_TAXI && m_nModelIndex != MODEL_CABBIE) {
            // Como el original: 6B4B00..6B4B3C target = (posicion + adelante * 10) + derecha * 3 (orden de suma del EXE)
            CVector target = GetPosition() + GetForward() * 10.0f;
            target = target + GetRight() * 3.0f;
            CColPoint cp; // Como el original: sin inicializar
            CEntity* hit = nullptr; // Como el original: 6B4B64 mov dword ptr [esp+4Ch],0
            if (!CWorld::ProcessLineOfSight(GetPosition(), target, cp, hit, true, true, true, false, false, false, false, false) || hit == this) {
                CCarAI::GetCarToParkAtCoors(this, target);
            }
        }
    }

    bool isRemotelyControlledByPlayer = false;
    // Como el original: 6B4B95 lee el estado una sola vez (jump table 6B5718); BlowUpCar / TakeRemoteControlledCarFromPlayer pueden
    // cambiarlo y el EXE sigue en 6B4C8D sin volver a mirarlo
    const eEntityStatus switchStatus = GetStatus();
    switch (switchStatus) {
    case STATUS_SIMPLE:
    {
        CCarAI::UpdateCarAI(this);
        CPhysical::ProcessControl();
        CCarCtrl::UpdateCarOnRails(this);
        m_nNumContactWheels = 4;
        m_NumDriveWheelsOnGroundLastFrame = m_NumDriveWheelsOnGround;
        m_NumDriveWheelsOnGround = 4;
        float speed = m_autoPilot.m_speed * 0.02f; // Como el original: 6B51EE fmul [0x858B38] (0.02f), no una division entre 50
        m_pHandlingData->GetTransmission().CalculateGearForSimpleCar(speed, m_nCurrentGear);
        float wheelRot = CVehicle::ProcessWheelRotation(WHEEL_STATE_NORMAL, GetForward(), m_vecMoveSpeed, 0.35f);
        for (float& rotation : m_wheelRotation) {
            rotation += wheelRot;
        }
        PlayHornIfNecessary();
        ReduceHornCounter();
        vehicleFlags.bAudioChangingGear = false;
        vehicleFlags.bVehicleColProcessed = false;
        return true;
    }
    case STATUS_PHYSICS:
    case STATUS_GHOST:
    {
        CCarAI::UpdateCarAI(this);
        CCarCtrl::SteerAICarWithPhysics(this);
        CCarCtrl::ReconsiderRoute(this);
        PlayHornIfNecessary();
        if (vehicleFlags.bIsBeingCarJacked) {
            m_GasPedal = 0.0f;
            m_BrakePedal = 1.0f;
            vehicleFlags.bIsHandbrakeOn = true;
        }
        if (!IsAnyWheelTouchingSand())
            return false;
        if (m_nModelIndex == MODEL_RCBANDIT || m_nModelIndex == MODEL_SANDKING || m_nModelIndex == MODEL_BFINJECT)
            return false;
        autoFlags.bIsBoggedDownInSand = true;
        // Como el original: 6B53DF..6B5425 con lluvia ((1 - WetRoads) * masa * paso) * 0.005f; sin ella (paso * masa) * 0.005f
        float force;
        if (CWeather::WetRoads > 0.0f)
            force = (1.0f - CWeather::WetRoads) * m_fMass * CTimer::GetTimeStep() * 0.005f;
        else
            force = CTimer::GetTimeStep() * m_fMass * 0.005f;
        ApplyMoveForce(-force * m_vecMoveSpeed);
        return false;
    }
    case STATUS_ABANDONED:
        for (auto& collisionEntity : m_apWheelCollisionEntity) {
            if (collisionEntity) {
                vehicleFlags.bRestingOnPhysical = true;
                break;
            }
        }

        if (IsSubHeli() || IsSubPlane() && m_vecMoveSpeed.SquaredMagnitude() < 0.1f)
            m_BrakePedal = 1.0f;

        if (vehicleFlags.bRestingOnPhysical)
            m_BrakePedal = 0.5f;
        else if (m_vecMoveSpeed.SquaredMagnitude() < 0.01f)
            m_BrakePedal = 0.2f;
        else
            m_BrakePedal = 0.0f;

        vehicleFlags.bIsHandbrakeOn = false;
        m_fSteerAngle = 0.0f;
        m_GasPedal = 0.0f;

        if (CanUpdateHornCounter())
            m_HornCounter = 0;

        if (!vehicleFlags.bIsBeingCarJacked)
            return false;

        vehicleFlags.bIsHandbrakeOn = true;
        m_GasPedal = 0.0f;
        m_BrakePedal = 1.0f;
        return false;
    case STATUS_WRECKED:
        m_BrakePedal = 0.05f;
        vehicleFlags.bIsHandbrakeOn = true;
        m_fSteerAngle = 0.0f;
        m_GasPedal = 0.0f;
        if (!CanUpdateHornCounter())
            return false;
        m_HornCounter = 0;
        return false;
    case STATUS_REMOTE_CONTROLLED:
        if (CPad::GetPad()->CarGunJustDown() && !CVehicle::bDisableRemoteDetonation) {
            BlowUpCar(FindPlayerPed(), false);
            CRemote::TakeRemoteControlledCarFromPlayer(true);
        }

        if (m_nModelIndex == MODEL_RCBANDIT && !CVehicle::bDisableRemoteDetonationOnContact) {
            (void)FindPlayerCoors(); // Como el original: 6B4C08 call 56E010 (el valor devuelto se ignora)
            if (RcbanditCheckHitWheels()|| physicalFlags.bSubmergedInWater) {
                CRemote::TakeRemoteControlledCarFromPlayer(true);
                BlowUpCar(FindPlayerPed(), false);
            }
        }
        if (FindPlayerInfo().m_pRemoteVehicle == this)
            isRemotelyControlledByPlayer = true;
        break;
    case STATUS_FORCED_STOP:
        if (m_vecMoveSpeed.SquaredMagnitude() < 0.01f
            || m_pDriver
            && m_pDriver->IsPlayer()
            && (m_pDriver->m_nPedState == PEDSTATE_ARRESTED || m_pDriver->GetTaskManager().HasAnyOf<TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, TASK_COMPLEX_CAR_QUICK_BE_DRAGGED_OUT, TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN>()))
        {
            m_BrakePedal = 1.0f;
            m_GasPedal = 0.0f;
            vehicleFlags.bIsHandbrakeOn = true;
        }
        else
        {
            m_BrakePedal = 0.0f;
            vehicleFlags.bIsHandbrakeOn = false;
        }

        m_fSteerAngle = 0.0f;
        m_GasPedal = 0.0f;
        if (!CanUpdateHornCounter())
            return false;
        m_HornCounter = 0;
        return false;
    case STATUS_IS_TOWED:
        vehicleFlags.bIsHandbrakeOn = false;
        m_BrakePedal = 0.0f;
        m_fSteerAngle = 0.0f;
        m_GasPedal = 0.0f;
        if (!m_pTowingVehicle)
            BreakTowLink();
        return false;
    }

    if (switchStatus != STATUS_PLAYER && switchStatus != STATUS_REMOTE_CONTROLLED)
        return false;

    bool processControlInput = isRemotelyControlledByPlayer;
    if (!processControlInput && m_pDriver)
    {
        ePedState pedState = m_pDriver->m_nPedState;
        if (pedState != PEDSTATE_EXIT_CAR && pedState != PEDSTATE_DRAGGED_FROM_CAR && pedState != PEDSTATE_ARRESTED)
            processControlInput = true;
    }
    if (!processControlInput)
        return false;

    CPad* pad = nullptr;
    if (m_pDriver) {
        pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    }

    PruneReferences();
    if (isRemotelyControlledByPlayer) {
        int32 playerSlot = CWorld::FindPlayerSlotWithRemoteVehiclePointer(this);
        if (playerSlot >= 0)
            ProcessControlInputs(playerSlot);
    }
    else if (m_pDriver->IsPlayer()) { // 0x6B4CD4
        ProcessControlInputs(static_cast<uint8>(m_pDriver->m_nPedType));
    }

    if (GetStatus() == STATUS_PLAYER && !IsSubHeli()) {
        if (m_nModelIndex == MODEL_VORTEX || !IsSubPlane()) {
            if (m_nModelIndex != MODEL_SWATVAN && m_nModelIndex != MODEL_RHINO) {
                DoDriveByShootings();
            }
        }
    }

    auto carLess3WheelCounter = FindPlayerInfo().m_nCarLess3WheelCounter;
    m_vecCentreOfMass.z = m_pHandlingData->m_vecCentreOfMass.z;
    if (carLess3WheelCounter > 500 && !IsSubHeli() && !IsSubPlane()) {
        carLess3WheelCounter = std::min(carLess3WheelCounter - 500u, 1000u);
        float fCarLess3WheelCounter = static_cast<float>(carLess3WheelCounter) * 0.002f;
        const float boundMaxZ = colModel->m_boundBox.m_vecMax.z;
        if (GetUp().z > -0.4f && pad) {
            if (!(GetRight().z > 0.0f)) // Como el original: 6B4D8C fcomp 0; test ah,41h; je (con NaN tambien se invierte)
                fCarLess3WheelCounter *= -1.0f;

            // Como el original: 6B4DAB..6B4DD9 ((direccion / 128) * maxZ * CAR_BALANCE_MULT) * contador + z del handling
            float steerLeftRight = (float)pad->GetSteeringLeftRight() * 0.0078125f;
            m_vecCentreOfMass.z += steerLeftRight * boundMaxZ * CAR_BALANCE_MULT * fCarLess3WheelCounter;
        }
        else {
            m_vecCentreOfMass.z -= CAR_BALANCE_MULT * boundMaxZ * 0.5f;
        }
    }

    if (!pad || m_nNumContactWheels != 0 || IsSubHeli() || IsSubPlane()) {
        DoSoftGroundResistance(extraHandlingFlags);
        return false;
    }

    float carWeightMult = 0.0007f;
    if (IsSubMonsterTruck())
        carWeightMult = 0.0025f;
    else
        carWeightMult *= CStats::GetFatAndMuscleModifier(STAT_MOD_DRIVING_SKILL);
    // Como el original: 6B4E6E..6B4E97 (min(1, 3000 / masa angular) * factor) * masa angular
    // Como el original: 0x404330 es min(a, b) = (a < b) ? a : b (con NaN devuelve b), no std::min
    const float turnMassRatio = 3000.0f / m_fTurnMass;
    carWeightMult = (1.0f < turnMassRatio ? 1.0f : turnMassRatio) * carWeightMult * m_fTurnMass;

    float steerLeftRight = (float)pad->GetSteeringLeftRight() * 0.0078125f; // Como el original: fmul [0x858B88] (1 / 128)
    float steerUpDown    = (float)pad->GetSteeringUpDown() * 0.0078125f;
    if (CCamera::m_bUseMouse3rdPerson && std::fabs(steerLeftRight) < 0.05f && std::fabs(steerUpDown) < 0.05f) {
        // Como el original: 6B4F04..6B4F5B x * [0x871040] (0.02f), no x / 50
        steerLeftRight = std::clamp<float>(CPad::NewMouseControllerState.m_AmountMoved.x * 0.02f, -1.5f, 1.5f);
        steerUpDown    = std::clamp<float>(CPad::NewMouseControllerState.m_AmountMoved.y * 0.02f, -1.5f, 1.5f);
    }

    // 0x6B4F69
    if (pad->GetHandBrake()) {
        float dotTurnUp = DotProduct(m_vecTurnSpeed, GetUp());
        if (dotTurnUp < 0.02f && steerLeftRight < 0.0f || dotTurnUp > -0.02f && steerLeftRight > 0.0f) {
            CVector point = m_vecCentreOfMass + GetForward();
            CVector force = GetRight() * (CTimer::GetTimeStep() * steerLeftRight * carWeightMult); // Como el original: 6B4FED..6B5006 (paso * direccion) * factor, luego derecha * ese valor
            ApplyTurnForce(force, point);
        }
    }
    else if (!pad->GetAccelerate()) {
        float dotTurnForward = DotProduct(m_vecTurnSpeed, GetForward());
        if (dotTurnForward < 0.02f && steerLeftRight < 0.0f
            || dotTurnForward > -0.02f && steerLeftRight > 0.0f)
        {
            CVector point = m_vecCentreOfMass + GetUp();
            CVector force = GetRight() * (CTimer::GetTimeStep() * steerLeftRight * carWeightMult); // Como el original: 6B508D..6B50A9
            ApplyTurnForce(force, point);
        }
    }

    if (!pad->GetAccelerate()) {
        float dotTurnRight = DotProduct(m_vecTurnSpeed, GetRight());
        if (dotTurnRight < 0.02f && steerUpDown < 0.0f
            || dotTurnRight > -0.02f && steerUpDown > 0.0f)
        {
            CVector point = m_vecCentreOfMass + GetForward();
            CVector force = GetUp() * (CTimer::GetTimeStep() * steerUpDown * carWeightMult); // Como el original: 6B5156..6B517B
            ApplyTurnForce(force, point);
        }
    }
    DoSoftGroundResistance(extraHandlingFlags);
    return false;
}

// 0x6A2AE0
void CAutomobile::ResetSuspension()
{
    for (int32 i = 0; i < 4; i++) {
        m_fWheelsSuspensionCompression[i] = 1.0f;
        m_WheelCounts[i] = 0.0f;
        m_wheelRotation[i] = 0.0f;
        m_WheelStates[i] = WHEEL_STATE_NORMAL;
    }
}

// 0x6A8500
void CAutomobile::ProcessFlyingCarStuff()
{
    if (GetStatus() == STATUS_PLAYER || GetStatus() == STATUS_REMOTE_CONTROLLED || GetStatus() == STATUS_PHYSICS) {
        if (CCheat::IsActive(CHEAT_CARS_FLY)
            && m_vecMoveSpeed.Magnitude() > 0.0f
            && CTimer::GetTimeStep() > 0.0f
        ) {
            FlyingControl(FLIGHT_MODEL_PLANE, -9999.9902F, -9999.9902F, -9999.9902F, -9999.9902F);
        }
    }
}

namespace {
// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE (G4: helper propio del grupo de suspension/colision)
CVector G4Multiply3x3(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y,
    };
}

// Como el original: MultiplyMatrixWithVector (0x59C890), con el orden de sumas del EXE
CVector G4MultiplyPoint(const CMatrix& m, const CVector& v) {
    return CVector{
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// Como el original: CColPoint::operator= (0x40FC80) copia punto, normal, superficies/piezas/iluminacion y profundidad, pero NO
// el relleno (+0x0C, +0x1C, +0x26)
void G4CopyColPoint(CColPoint& dst, const CColPoint& src) {
    auto* d = reinterpret_cast<uint8*>(&dst);
    const auto* s = reinterpret_cast<const uint8*>(&src);
    std::memcpy(d, s, 0x0C);
    std::memcpy(d + 0x10, s + 0x10, 0x0C);
    std::memcpy(d + 0x20, s + 0x20, 0x06);
    std::memcpy(d + 0x28, s + 0x28, 0x04);
}

// Como el original: fsin / fcos del x87 (0x41BCE0 / 0x41BCF0), resultado redondeado al guardar como float
float G4Fsin(float v) {
    return TrigSimpleSinCpp(v);
}

float G4Fcos(float v) {
    return TrigSimpleCosCpp(v);
}

// Como el original (0x6A5CE7..0x6A5D0F): pow(0.995, paso) con el _CIpow del EXE (0x822130, via 0x470000), usado con
// precision completa en (restante * pow) + 1.0
float G4TireTemperatureDecay(float temperatureAboveOne, float timeStep) {
    return AutomobileTireDecayCpp(std::bit_cast<float>(0x3F7EB852u), timeStep, temperatureAboveOne); // 0.995f
}

// Como el original: pow(base, exponente) con el _CIpow del EXE (0x822130), guardado como float (fstp dword)
float G4PowToFloat(float base, float exponent) {
    return CnvDecayPowCpp(base, exponent);
}
} // namespace

// 0x6A45C0
void CAutomobile::DoHoverSuspensionRatios()
{
    // Como el original (0x6A45C6): fcomp up.z con 0.3 y test ah,5 / jnp: se continua salvo up.z < 0.3 (con NaN continua)
    if (!(GetUp().z < 0.3f) && !vehicleFlags.bIsDrowning) {
        auto colData = GetColModel()->m_pColData;
        for (int32 i = 0; i < 4; i++)  {
            CColLine& line = colData->m_pLines[i];
            // Como el original: MultiplyMatrixWithVector (0x59C890) con su orden de sumas
            CVector start = G4MultiplyPoint(*m_matrix, line.m_vecStart);
            CVector end = G4MultiplyPoint(*m_matrix, line.m_vecEnd);
            float colPointZ = MAP_Z_LOW_LIMIT;
            if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                colPointZ = m_wheelColPoint[i].m_vecPoint.z;
            }
            float waterLevel = 0.0f;
            if (CWaterLevel::GetWaterLevel(end.x, end.y, end.z, waterLevel, false, nullptr)
                && waterLevel > colPointZ && end.z - 1.0f < waterLevel)
            {
                if (waterLevel <= end.z)
                    m_fWheelsSuspensionCompression[i] = std::bit_cast<float>(0x3F7FFF58u); // Como el original (0x6A46EB): mov [edi],3F7FFF58h (no 1.0f)
                else if (waterLevel <= start.z)
                    m_fWheelsSuspensionCompression[i] = (start.z - waterLevel) / (start.z - end.z);
                else
                    m_fWheelsSuspensionCompression[i] = 0.0f;

                CVector point = (end - start) * m_fWheelsSuspensionCompression[i];
                point += start;
                point.z = waterLevel;
                m_wheelColPoint[i].m_vecPoint = point;
                m_wheelColPoint[i].m_vecNormal = CVector(0.0f, 0.0f, 1.0f);
                m_wheelColPoint[i].m_nSurfaceTypeB = SURFACE_WATER_SHALLOW; // Como el original (0x6A4754): mov byte ptr [ebx+23h],27h
            }
        }
    }
}

// 0x6AFB10
void CAutomobile::ProcessSuspension() {
    // Como el original: springLength y los limites de amortiguacion viven en la pila; los limites se ponen a cero al entrar
    float springLength[4];
    float wheelSpringForceDampingLimits[4]{};
    for (int32 i = 0; i < 4; i++) {
        springLength[i] = m_fWheelsSuspensionCompression[i];
    }

    // Como el original (0x6AFB88..): |dot| con el orden z, y, x del EXE (DotProduct)
    const float forwardSpeed = std::fabs(DotProduct(m_vecMoveSpeed, GetForward()));

    // Como el original: contador de vueltas (1, o 3 para el Rhino fuera de replay) con do { } while (contador > 0)
    int32 loopsLeft = 1;
    if (ModelIndices::IsRhino(m_nModelIndex) && CReplay::Mode != MODE_PLAYBACK) {
        loopsLeft = 3;
    }

    CVector contactPoints[4];
    CVector directions[4];
    do {
        for (int32 i = 0; i < 4; i++) {
            directions[i] = GetUp() * -1.0f;
            if (springLength[i] < 1.0f) {
                contactPoints[i] = m_wheelColPoint[i].m_vecPoint - GetPosition();
            } else {
                contactPoints[i].Set(0.f, 0.f, 0.f);
            }
        }

        for (int32 i = 0; i < 4; i++) {
            if (!(springLength[i] < 1.0f))
                continue;

            float suspensionBias = m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear;
            if (i == CAR_WHEEL_REAR_LEFT || i == CAR_WHEEL_REAR_RIGHT)
                suspensionBias = 1.0f - suspensionBias;

            float fSuspensionForceLevel = m_pHandlingData->m_fSuspensionForceLevel;
            // Como el original (0x6AFF85..0x6AFFA3): el sesgo 0.5 exige geometria + instalacion hidraulica + NPC neutral + STATUS_PHYSICS
            if (handlingFlags.bHydraulicGeom && handlingFlags.bHydraulicInst
                && handlingFlags.bNpcNeutralHandl && GetStatus() == STATUS_PHYSICS) {
                suspensionBias = 0.5f;
            }
            // Como el original (0x6AFFAB..0x6B0006): con instalacion hidraulica y velocidad < 0.15 multiplica por 1.5 (con
            // geometria) o por 1.0 (sin ella); solo si no entra aqui comprueba el Vortex
            if (handlingFlags.bHydraulicInst && std::fabs(forwardSpeed) < 0.15f) {
                fSuspensionForceLevel = (handlingFlags.bHydraulicGeom ? 1.5f : 1.0f) * fSuspensionForceLevel;
            } else if (ModelIndices::IsVortex(m_nModelIndex)) {
                fSuspensionForceLevel = (std::fabs(AsPlane()->m_fAccelerationBreakStatus) * 0.25f + 1.0f) * fSuspensionForceLevel;
            }

            if (CCheat::IsActive(CHEAT_CARS_ON_WATER) || ModelIndices::IsVortex(m_nModelIndex))
                ApplySpringCollision(
                    fSuspensionForceLevel,
                    directions[i],
                    contactPoints[i],
                    springLength[i],
                    suspensionBias,
                    wheelSpringForceDampingLimits[i]
                );
            else
                ApplySpringCollisionAlt(
                    fSuspensionForceLevel,
                    directions[i],
                    contactPoints[i],
                    springLength[i],
                    suspensionBias,
                    m_wheelColPoint[i].m_vecNormal,
                    wheelSpringForceDampingLimits[i]
                );
        }

        CVector contactSpeeds[4];
        for (int32 i = 0; i < 4; ++i) {
            contactSpeeds[i] = CPhysical::GetSpeed(contactPoints[i]);
            if (m_apWheelCollisionEntity[i])
                contactSpeeds[i] -= m_apWheelCollisionEntity[i]->GetSpeed(m_vWheelCollisionPos[i]);

            if (m_fWheelsSuspensionCompression[i] < 1.0f && m_wheelColPoint[i].m_vecNormal.z > 0.35f) {
                directions[i] = -m_wheelColPoint[i].m_vecNormal;
            }
        }

        for (int32 i = 0; i < 4; ++i) {
            float dampingForce = m_pHandlingData->m_fSuspensionDampingLevel;
            // Como el original (0x6B01F6..0x6B0239): 0.1 < amortiguacion y velocidad < 0.15 con instalacion hidraulica
            if (handlingFlags.bHydraulicInst && 0.1f < dampingForce && std::fabs(forwardSpeed) < 0.15f)
                dampingForce = 0.1f;

            // Como el original (0x6B023D): el limite de compresion es 0.99999 (0x871258 = 3F7FFF58h), no 1.0
            if (springLength[i] < std::bit_cast<float>(0x3F7FFF58u) && !autoFlags.bIsMonsterTruck)
                ApplySpringDampening(
                    dampingForce,
                    wheelSpringForceDampingLimits[i],
                    directions[i],
                    contactPoints[i],
                    contactSpeeds[i]
                );
        }

        // Como el original (0x6B0293..): se descuenta la vuelta y solo el Rhino prepara la siguiente
        --loopsLeft;
        if (!ModelIndices::IsRhino(m_nModelIndex))
            continue;

        if (loopsLeft <= 0)
            break;

        const auto colData = GetColModel()->m_pColData;
        int32 wheelLineIndices[4]{ 4, 7, 8, 11 };
        float fDummy1, fDummy2;
        if (loopsLeft == 2) {
            // Como el original (0x6B02EF): m_doors[4].open, m_doors[4].prevAngle, m_doors[5].open, m_doors[5].prevAngle
            m_doors[DOOR_LEFT_REAR].GetExtraWheelPositions(springLength[CAR_WHEEL_FRONT_LEFT], fDummy1, fDummy2, springLength[CAR_WHEEL_REAR_LEFT]);
            m_doors[DOOR_RIGHT_REAR].GetExtraWheelPositions(springLength[CAR_WHEEL_FRONT_RIGHT], fDummy1, fDummy2, springLength[CAR_WHEEL_REAR_RIGHT]);
        } else if (loopsLeft == 1) {
            // Como el original (0x6B0322): m_doors[4].closed, m_doors[4].angle, m_doors[5].closed, m_doors[5].angle
            m_doors[DOOR_LEFT_REAR].GetExtraWheelPositions(fDummy1, springLength[CAR_WHEEL_FRONT_LEFT], springLength[CAR_WHEEL_REAR_LEFT], fDummy2);
            m_doors[DOOR_RIGHT_REAR].GetExtraWheelPositions(fDummy1, springLength[CAR_WHEEL_FRONT_RIGHT], springLength[CAR_WHEEL_REAR_RIGHT], fDummy2);
            wheelLineIndices[CAR_WHEEL_FRONT_LEFT]  = 5;
            wheelLineIndices[CAR_WHEEL_REAR_LEFT]   = 6;
            wheelLineIndices[CAR_WHEEL_FRONT_RIGHT] = 9;
            wheelLineIndices[CAR_WHEEL_REAR_RIGHT]  = 10;
        } else {
            continue;
        }

        for (int32 i = 0; i < 4; i++) {
            CColLine& colLine = colData->m_pLines[wheelLineIndices[i]];
            // Como el original (0x6B0382..): z = (-(linea * springLength)) + start.z; luego Multiply3x3 (0x59C790)
            const float zOffset = -(m_aSuspensionLineLength[i] * springLength[i]);
            const CVector local{ colLine.m_vecStart.x, colLine.m_vecStart.y, zOffset + colLine.m_vecStart.z };
            contactPoints[i] = G4Multiply3x3(GetMatrix(), local);

            const float springTension = 1.f - (m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i]);
            springLength[i] = (springLength[i] - springTension) / (1.f - springTension);
        }
    } while (loopsLeft > 0);

    // Como el original (0x6B042B..): umbral 0.02 y fuerza 0.0025 (x2 si no es jugador ni mando a distancia)
    float speedThreshold = 0.02f;
    float rollOnToWheelsForce = ROLL_ONTO_WHEELS_FORCE;
    if (GetStatus() != STATUS_PLAYER && GetStatus() != STATUS_REMOTE_CONTROLLED) {
        speedThreshold = speedThreshold + speedThreshold;
        rollOnToWheelsForce = rollOnToWheelsForce + rollOnToWheelsForce;
    }

    if (forwardSpeed < speedThreshold) {
        // Como el original (0x6B0470..0x6B050C): la igualdad con 1.0 se hace sobre los bits (cmp eax,3F800000h)
        const auto IsOneBits = [](float v) { return std::bit_cast<uint32>(v) == 0x3F800000u; };
        float fDir = 0.0f;
        if (IsOneBits(m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_LEFT])
            && IsOneBits(m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_LEFT])
            && (m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_RIGHT] < 1.0f
                || m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_RIGHT] < 1.0f))
        {
            fDir = 1.0f;
        }
        else if (forwardSpeed < speedThreshold
            && IsOneBits(m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_RIGHT])
            && IsOneBits(m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_RIGHT])
            && (m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_LEFT] < 1.0f
                || m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_LEFT] < 1.0f))
        {
            fDir = -1.0f;
        }

        if (fDir != 0.0f) {
            const CVector worldUp{ 0.0f, 0.0f, 1.0f };
            const CVector right = CrossProduct(GetForward(), worldUp);
            if (std::fabs(DotProduct(right, GetRight())) < 0.6f) {
                auto colModel = GetVehicleModelInfo()->GetColModel();
                // Como el original (0x6B058B..): escalar (fuerza * masaGiro) * fDir y despues por cada componente de up
                const float turnScale = (rollOnToWheelsForce * m_fTurnMass) * fDir;
                const CVector force{ turnScale * GetUp().x, turnScale * GetUp().y, turnScale * GetUp().z };
                const float maxX = colModel->GetBoundingBox().m_vecMax.x;
                const CVector point{ maxX * GetRight().x, maxX * GetRight().y, maxX * GetRight().z };
                ApplyTurnForce(force, point);
                // Como el original (0x6B0608..): cross(up, forward) * ((fuerza * masa) * fDir)
                const CVector rightNeg = CrossProduct(worldUp, GetForward());
                const float moveScale = (rollOnToWheelsForce * m_fMass) * fDir;
                ApplyMoveForce(CVector{ moveScale * rightNeg.x, moveScale * rightNeg.y, moveScale * rightNeg.z });
            }
        }
    }
}

// 0x6ACE70
int32 CAutomobile::ProcessEntityCollision(CEntity* entity, CColPoint* outColPoints) {
    if (GetStatus() != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true;
    }

    const auto tcd = GetColData();

#ifdef FIX_BUGS
    // FIX_BUGS@CAutomobile::ProcessEntityCollision:1
    // The original code handled this properly, because `ProcessColModels` returned `0`
    // if either colmodel's data was missing
    // but there's a lot of no-op stuff done below that we can just avoid altogether
    // Though, in the original code there was an edge case if `m_pTractor == entity || m_pTrailer == entity` => crash
    // but since I moved the assignment to the outside of the function now it always crashes xD
    if (!tcd || !entity->GetColData()) {
        return 0;
    }
#endif

    // The Rhinosaurus Rex has extra wheels (8 of them)
    std::array<float, MAX_NUM_SUSP_LINES> wheelColPtsTouchDists{};
    rng::copy(m_fWheelsSuspensionCompression, wheelColPtsTouchDists.begin());

    const auto originalWheelSuspCompr{m_fWheelsSuspensionCompression};

    // ...and their suspension compression values are stored inside doors (fml pls)
    const auto GetRhinosaurusRexExtraWheelSuspCompression = [this](size_t extraWheelIdx) -> float& { //
        auto& door = m_doors[4 + extraWheelIdx / 4];
        switch (extraWheelIdx % 4) {
        case 0: return door.m_openAngle;
        case 1: return door.m_closedAngle;
        case 2: return door.m_angle;
        case 3: return door.m_prevAngle;
        default: NOTSA_UNREACHABLE();
        }
    };

    // 0x6ACEF7
    if (m_nModelIndex == eModelID::MODEL_RHINO) {
        // Store shit for the rhinosaurus rex
        for (auto i = 0; i < NUM_RHINO_EXTRA_SUSP_LINES; i++) {
            wheelColPtsTouchDists[NUM_AUTOMOBILE_SUSP_LINES + i] = GetRhinosaurusRexExtraWheelSuspCompression(i);
        }
    }

    // Como el original (0x6ACE96): movsx del byte m_nNumLines
    const int32 tNumLines = int8(tcd->m_nNumLines);
    if (   physicalFlags.bSkipLineCol
        || physicalFlags.bProcessingShift
        || entity->GetIsTypePed()
        || m_nModelIndex == (uint16)eModelID::UNLOAD_MODEL && entity->GetIsTypeVehicle()
    ) {
        tcd->m_nNumLines = 0; // Later reset back to original value
    }

    // Como el original (0x6ACFBC..0x6AD007): oculta los triangulos de ambas colisiones guardando el valor con signo
    // (movsx word) y solo los restaura si lo guardado es >= 0
    int32 tNumTri = -1, oNumTri = -1;
    if (m_pTowingVehicle == entity || m_pVehicleBeingTowed == entity) {
        tNumTri = int16(tcd->m_nNumTriangles);
        tcd->m_nNumTriangles = 0;
        oNumTri = int16(entity->GetColModel()->m_pColData->m_nNumTriangles);
        entity->GetColModel()->m_pColData->m_nNumTriangles = 0;
    }

    // For ghosts we dont do shit (In case this garbage is a forklift this value is modified below)
    auto numColPts = GetStatus() == STATUS_GHOST
        ? 0
        : CCollision::ProcessColModels(
            GetMatrix(), *GetColModel(),
            entity->GetMatrix(), *entity->GetColModel(),
            *(std::array<CColPoint, 32>*)(outColPoints),
            aAutomobileColPoints.data(),
            wheelColPtsTouchDists.data(),
            false
        );

    // Restore hidden triangles
    if (oNumTri >= 0) {
        entity->GetColModel()->m_pColData->m_nNumTriangles = uint16(oNumTri);
    }
    if (tNumTri >= 0) {
        tcd->m_nNumTriangles = uint16(tNumTri);
    }

    int32 numProcessedWheels{};
    if (tcd->m_nNumLines) {
        // Process the real wheels
        for (auto i = 0; i < MAX_CARWHEELS; i++) {
            // 0x6AD0D4
            const auto& cp = aAutomobileColPoints[i];

            // Como el original (0x6AD0D4..0x6AD0FE): dist < 1.0 && dist < compresion, ambos fcomp + test ah,5 / jp
            const auto wheelColPtsTouchDist = wheelColPtsTouchDists[i];
            if (!(wheelColPtsTouchDist < 1.f) || !(wheelColPtsTouchDist < m_fWheelsSuspensionCompression[i])) {
                continue;
            }

            m_fWheelsSuspensionCompression[i] = wheelColPtsTouchDist;
            G4CopyColPoint(m_wheelColPoint[i], cp); // Como el original: 0x40FC80 (sin relleno)

            m_anCollisionLighting[i] = m_wheelColPoint[i].m_nLightingB;
            numProcessedWheels++;

            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT: {
                // Como el original (0x6AD147..0x6AD150): guarda el puntero y registra la referencia SIN limpiar la anterior
                CEntity::SetEntityReference(m_apWheelCollisionEntity[i], entity->AsPhysical());

                m_vWheelCollisionPos[i] = m_wheelColPoint[i].m_vecPoint - entity->GetPosition();
                if (entity->GetIsTypeVehicle()) {
                    m_anCollisionLighting[i] = entity->AsVehicle()->m_anCollisionLighting[i];
                }
                break;
            }
            default:
                break;
            }

            m_nContactSurface = m_wheelColPoint[i].m_nSurfaceTypeB;

            if (entity->GetType() == ENTITY_TYPE_BUILDING) {
                m_pEntityWeAreOn = entity;
                m_bTunnel = entity->m_bTunnel;
                m_bTunnelTransition = entity->m_bTunnelTransition;
            }
        }

        // Deal with extra wheels for the Rhinosaurus Rex
        if (m_nModelIndex == eModelID::MODEL_RHINO) {
            // Como el original (0x6AD24A..0x6AD372): (dist < original) ? dist : original, y las 8 se vuelcan al final
            float extraCompr[NUM_RHINO_EXTRA_SUSP_LINES];
            for (auto i = 0; i < NUM_RHINO_EXTRA_SUSP_LINES; i++) {
                const float dist = wheelColPtsTouchDists[NUM_AUTOMOBILE_SUSP_LINES + i];
                const float orig = GetRhinosaurusRexExtraWheelSuspCompression(i);
                extraCompr[i] = (dist < orig) ? dist : orig;
            }
            for (auto i = 0; i < NUM_RHINO_EXTRA_SUSP_LINES; i++) {
                GetRhinosaurusRexExtraWheelSuspCompression(i) = extraCompr[i];
            }
        }
    } else {
        tcd->m_nNumLines = uint8(tNumLines);
    }

    // 0x6AD38A - Remove a few specific cp's for forklifts
    if (m_nModelIndex == eModelID::MODEL_FORKLIFT && numColPts > 0 && entity->GetIsTypeObject() && !m_wMiscComponentAngle && !m_wMiscComponentAnglePrev) {
        // Como el original (0x6AD3BB..0x6AD42D): se borra en el array de SALIDA (outColPoints, no el de ruedas) desplazando un
        // elemento cada vez (CColPoint::operator= 0x40FC80) y repitiendo el mismo indice tras cada borrado
        int32 last = numColPts - 1;
        for (int32 k = 0; k < numColPts; k++) {
            if (outColPoints[k].m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT) {
                if (k < last) {
                    for (int32 j = k; j < last; j++) {
                        G4CopyColPoint(outColPoints[j], outColPoints[j + 1]);
                    }
                    k--;
                }
                numColPts--;
                last--;
            }
        }
    }

    if (numColPts <= 0 && numProcessedWheels <= 0) {
        goto rtn;
    }

    AddCollisionRecord(entity);
    if (!entity->GetIsTypeBuilding()) {
        entity->AsPhysical()->AddCollisionRecord(this);
    }

    if (numColPts > 0) {
        if (   entity->GetIsTypeBuilding()
            || (entity->GetIsTypeObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)
        ) {
            SetHasHitWall(true);
        }
    }

    // 0x6AD482 - Add additional output colpoints for wheels
    if (numProcessedWheels > 0 && entity->GetIsTypeBuilding() && m_pHandlingData->m_fSuspensionHighSpdComDamp > 0.f) {
        for (auto i = 0; i < MAX_CARWHEELS; i++) {
            // Como el original (0x6AD4D0..0x6AD50A): lerp(anterior, 1, t) = (1 - t) * anterior + t, y minimo con la original
            const float springRatio = 1.0f - m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i];
            float compressionNow = (1.0f - springRatio) * m_fWheelsSuspensionCompressionPrev[i] + springRatio;
            if (originalWheelSuspCompr[i] < compressionNow) {
                compressionNow = originalWheelSuspCompr[i];
            }
            const auto suspComprDelta = (compressionNow - m_fWheelsSuspensionCompression[i]) * m_aSuspensionLineLength[i]; // Suspension compression delta (In meters)
            if (!(suspComprDelta > 0.1f /* 0x8D3200 */)) {
                continue;
            }
            if (m_damageManager.GetWheelStatus((eCarWheel)i) != eCarWheelStatus::WHEEL_STATUS_OK) { // Move to top
                continue;
            }
            const auto& wheelcp = m_wheelColPoint[i];
            if (g_surfaceInfos.GetAdhesionGroup(wheelcp.m_nSurfaceTypeB) == ADHESION_GROUP_SAND || wheelcp.m_nSurfaceTypeB == SURFACE_RAILTRACK) {
                continue;
            }
            auto& outcp = outColPoints[numColPts];
            G4CopyColPoint(outcp, wheelcp); // Como el original: 0x40FC80 (sin relleno)

            // Como el original (0x6AD589..0x6AD630): (delta - [0xC1C214]) acotado a 0.2, |velocidad . forward| acotado a 0.3
            // y |normal . up| con el orden (z, x) + y; todo multiplicado por 3.3333333 (0x859F60) y por HighSpdComDamp
            float comprDelta = (compressionNow - m_fWheelsSuspensionCompression[i]) * m_aSuspensionLineLength[i];
            comprDelta = comprDelta - StaticRef<float>(0xC1C214);
            const float limitedDelta = (0.2f < comprDelta) ? 0.2f : comprDelta; // 0x8D31FC
            const float forwardSpeedAbs = std::fabs(DotProduct(GetMoveSpeed(), GetForward()));
            const float limitedSpeed = (0.3f < forwardSpeedAbs) ? 0.3f : forwardSpeedAbs;
            const float normalDotUp = std::fabs(
                (outcp.m_vecNormal.z * GetUp().z + outcp.m_vecNormal.x * GetUp().x) + GetUp().y * outcp.m_vecNormal.y
            );
            outcp.m_fDepth = (((normalDotUp * limitedSpeed) * limitedDelta) * std::bit_cast<float>(0x40555555u)) * m_pHandlingData->m_fSuspensionHighSpdComDamp;
            outcp.m_nPieceTypeA = CarWheelToCarPiece((eCarWheel)i);
            outcp.m_nSurfaceTypeA = outcp.m_nSurfaceTypeB = SURFACE_WHEELBASE;
            // Como el original (0x6AD657..0x6AD66F): sin FIX_BUGS ni break, cuenta el colpoint y sigue con las demas ruedas
            if (numColPts < 31) { // TODO/NOTE: `32` is ProcessColModels output array size
                numColPts++;
            }
        }
    }

rtn:
    return numColPts;
}

// 0x6A29C0
void CAutomobile::ProcessControlCollisionCheck(bool applySpeed) {
    const CMatrix ogmat = GetMatrix(); // Save original matrix (We need it later)

    SetIsStuck(false);

    SkipPhysics();

    physicalFlags.bSkipLineCol = physicalFlags.bProcessingShift = false;

    m_fMovingSpeed = 0.f;
    rng::fill(m_fWheelsSuspensionCompression, 1.f);
    
    if (applySpeed) {
        ApplyMoveSpeed();
        ApplyTurnSpeed();
        for (auto tr = 0; CheckCollision() && tr < 5; tr++) { // tr = tries
            GetMatrix() = ogmat; // reset matrix
            ApplyMoveSpeed();
            ApplyTurnSpeed();
        }
    } else {
        const auto ogUsesCollision = GetUsesCollision();
        SetUsesCollision(false);
        (void)CheckCollision();
        SetUsesCollision(ogUsesCollision); // restore
    }

    SetIsStuck(false);
    SetIsInSafePosition(true);
}

// Como el original: 0x6AD8D1..0x6AD8E8 fld 0.975f; fld timestep; call 0x822130 (pow del CRT del EXE, precision completa en ST0); fmul raw (se redondea al guardar)
static float G3MouseSteerDecay(float raw, float timeStep) {
    const float base = StaticRef<float>(0x871064);
    float result = raw;
    PowScaleFloatCpp(&result, base, timeStep); // result = float(pow(base, ts) * raw)
    return result;
}

// 0x6AD690
void CAutomobile::ProcessControlInputs(uint8 playerNum) {
    // Como el original: reconstruido desde 0x6AD690..0x6ADEE1 (las constantes y el orden de las operaciones en coma flotante son los del EXE)
    constexpr float kInv255 = std::bit_cast<float>(0x3B808081u); // [0x859A3C]
    constexpr float kInv128 = 0.0078125f;                         // [0x858B88]
    static auto& s_signedSquareScratch = StaticRef<float>(0xC1C218); // 0x6ADD00 fstp [0xC1C218]

    const auto plyrpad = CPad::GetPad((int32)playerNum);
    const auto plyrdriver = m_pDriver; // I guess in case of the RC Bandit for example the player isn't sitting in the plyrveh, but can control it, so this might be null!

    //> 0x6AD69C - velocidad hacia delante: (z*fz + y*fy) + x*fx
    const auto& fwd = m_matrix->GetForward();
    const float fwdvel = (m_vecMoveSpeed.z * fwd.z + m_vecMoveSpeed.y * fwd.y) + m_vecMoveSpeed.x * fwd.x;

    //> 0x6AD6CB - Update handbrake state
    bool automaticallyHandBrake = false;
    if (plyrdriver) {
        auto& tm = plyrdriver->GetTaskManager();
        if (!tm.FindActiveTaskByType(TASK_SIMPLE_CAR_JUMP_OUT)) {
            if (   plyrdriver->m_nPedState == PEDSTATE_ARRESTED
                || tm.FindActiveTaskByType(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT)
                || tm.FindActiveTaskByType(TASK_COMPLEX_CAR_QUICK_BE_DRAGGED_OUT)
                || tm.FindActiveTaskByType(TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN)
                || CPad::GetPad((int32)playerNum)->GetExitVehicle()
            ) {
                automaticallyHandBrake = true;
            }
        }
    }
    if (automaticallyHandBrake) {
        vehicleFlags.bIsHandbrakeOn = true;
    } else {
        vehicleFlags.bIsHandbrakeOn = CPad::GetPad((int32)playerNum)->GetHandBrake() != 0;
    }

    //> 0x6AD79F - Update steer angle (And  possible control input too)
    const float timeStep = CTimer::GetTimeStep();
    // 0x6AD8F0 (teclado/pad sin raton): raw += ((x - raw) * 0.2f) * timeStep ; 0x6AD831 (dentro del modo raton): raw += ((x - raw) * timeStep) * 0.2f
    const auto SteerTarget = [&] { return (float)(-(int32)CPad::GetPad((int32)playerNum)->GetSteeringLeftRight()) * kInv128; };
    if (CCamera::m_bUseMouse3rdPerson && m_bEnableMouseSteering) {
        const float mouseX = CPad::NewMouseControllerState.m_AmountMoved.x;
        enum class Act { MOUSE_BRANCH, KEYBOARD_UPDATE, NO_CHANGE } act;
        if (mouseX != 0.f) { // 0x6AD7D1 fcomp; test ah,44h; jp 0x6AD873
            act = Act::MOUSE_BRANCH;
        } else if (std::fabs(m_fRawSteerAngle) > 0.f && m_nLastControlInput == eControllerType::MOUSE && CPad::GetPad((int32)playerNum)->GetSteeringLeftRight() == 0) {
            act = Act::MOUSE_BRANCH; // 0x6AD7DC..0x6AD80D
        } else if (CPad::GetPad((int32)playerNum)->GetSteeringLeftRight() == 0 && m_nLastControlInput == eControllerType::MOUSE) {
            act = Act::NO_CHANGE; // 0x6AD80F..0x6AD82B (je 0x6AD941)
        } else {
            act = Act::KEYBOARD_UPDATE; // 0x6AD831
        }

        if (act == Act::MOUSE_BRANCH) { // 0x6AD873
            const auto pad = CPad::GetPad((int32)playerNum);
            if (pad->NewState.m_bVehicleMouseLook == 0) { // cmp word [pad+0x2C],0
                m_fRawSteerAngle = mouseX * std::bit_cast<float>(0xBB656042u) + m_fRawSteerAngle; // [0x87105C] (-0.0035f)
            }
            m_nLastControlInput = eControllerType::MOUSE;
            if (std::fabs(m_fRawSteerAngle) < 0.7f || CPad::GetPad((int32)playerNum)->NewState.m_bVehicleMouseLook != 0) { // 0x6AD8AD..0x6AD8CF
                // Como el original: 0x6AD8D1..0x6AD8EE fld 0.975f; fld timestep; call pow; fmul raw
                m_fRawSteerAngle = G3MouseSteerDecay(m_fRawSteerAngle, timeStep);
            }
        } else if (act == Act::KEYBOARD_UPDATE) {
            m_fRawSteerAngle = ((SteerTarget() - m_fRawSteerAngle) * timeStep * 0.2f) + m_fRawSteerAngle; // 0x6AD85E..0x6AD92F
            m_nLastControlInput = eControllerType::KEYBOARD;
        }
    } else {
        m_fRawSteerAngle = (((SteerTarget() - m_fRawSteerAngle) * 0.2f) * timeStep) + m_fRawSteerAngle; // 0x6AD91F..0x6AD93B
        m_nLastControlInput = eControllerType::KEYBOARD;
    }
    // 0x6AD941 - clamp a [-1, 1]
    if (1.0f < m_fRawSteerAngle) {
        m_fRawSteerAngle = 1.0f;
    } else if (-1.0f > m_fRawSteerAngle) { // 0x6AD96E fcomp / test ah,41h / jne: con NaN se conserva el valor
        m_fRawSteerAngle = -1.0f;
    }

    //> 0x6AD992 - valor del pedal (acelerador - freno) escalado por 1/255
    const int32 accelerate = (int32)CPad::GetPad((int32)playerNum)->GetAccelerate();
    const int32 brakeInput = (int32)CPad::GetPad((int32)playerNum)->GetBrake();
    float gasPedalInput = (float)(accelerate - brakeInput) * kInv255;
    if (gasPedalInput == 0.f && plyrdriver) {
        // 0x6AD9CE..0x6ADA42: gatillos derecho/izquierdo cuando se apunta desde el coche con una tarea de tiroteo desde coche
        const auto simplest = plyrdriver->GetTaskManager().GetSimplestActiveTask();
        if (simplest && simplest->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY && TheCamera.m_aCams[0].m_nMode == MODE_AIMWEAPON_FROMCAR) {
            const auto pad = CPad::GetPad((int32)playerNum);
            const int32 rs2 = (int32)pad->NewState.RightShoulder2;
            const int32 ls2 = (int32)CPad::GetPad((int32)playerNum)->NewState.LeftShoulder2;
            gasPedalInput = (float)(rs2 - ls2) * kInv255;
        }
    }
    // 0x6ADA46 cmp word [esi+0x22],0FFFEh / fcomp 0; jp -> *0.3 ([0x858C24])
    if (m_nModelIndex == (uint16)0xFFFE && gasPedalInput < 0.f) {
        gasPedalInput = gasPedalInput * std::bit_cast<float>(0x3E99999Au);
    }
    // 0x6ADA6C test byte [esi+0x428],10h (bEngineOn) / test byte [esi+0x42B],40h (bIsDrowning): sin motor y ahogandose, 0
    if (!vehicleFlags.bEngineOn && vehicleFlags.bIsDrowning) {
        gasPedalInput = 0.f;
    }

    if (automaticallyHandBrake) { // 0x6ADA8D
        m_BrakePedal = 1.f;
        m_GasPedal = 0.f;
    } else if (std::fabs(fwdvel) < 0.01f) { // 0x6ADAA4..0x6ADB7C
        if ((float)CPad::GetPad((int32)playerNum)->GetAccelerate() > 150.f && (float)CPad::GetPad((int32)playerNum)->GetBrake() > 150.f) {
            m_GasPedal = (float)(int32)CPad::GetPad((int32)playerNum)->GetAccelerate() * kInv255;
            *reinterpret_cast<uint16*>(&m_bDoingBurnout) = 1; // Como el original: 0x6ADB5E mov word [esi+0x86A],1 (tambien pone a cero el byte de relleno 0x86B)
            m_BrakePedal = (float)(int32)CPad::GetPad((int32)playerNum)->GetBrake() * kInv255;
        } else {
            m_GasPedal = gasPedalInput;
            m_BrakePedal = 0.f;
        }
    } else if (!(fwdvel < 0.f)) { // 0x6ADB8B: se mueve hacia delante
        if (gasPedalInput < 0.f) {
            m_GasPedal = 0.f;
            m_BrakePedal = -gasPedalInput;
        } else {
            m_GasPedal = gasPedalInput;
            m_BrakePedal = 0.f;
        }
    } else if (gasPedalInput < 0.f) { // 0x6ADBC1: marcha atras
        m_GasPedal = gasPedalInput;
        m_BrakePedal = 0.f;
    } else if (m_GasPedal > 0.5f && fwdvel > std::bit_cast<float>(0xBE19999Au)) { // [0x86C330] (-0.15f)
        m_GasPedal = gasPedalInput;
        m_BrakePedal = 0.f;
    } else {
        m_GasPedal = 0.f;
        m_BrakePedal = gasPedalInput;
    }

    //> 0x6ADC18 and 0x6ADC64 - Check if we can go towards that direction
    if (m_GasPedal > 0.f && plyrdriver) {
        const auto& f = m_matrix->GetForward();
        if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(plyrdriver, CVector{ f.x, f.y, f.z }, 0.f)) {
            m_GasPedal = 0.f;
        }
    }
    if (m_GasPedal < 0.f && plyrdriver) {
        const auto& f = m_matrix->GetForward();
        if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(plyrdriver, CVector{ f.x * -1.0f, f.y * -1.0f, f.z * -1.0f }, 0.f)) {
            m_GasPedal = 0.f;
        }
    }

    //> 0x6ADCDB - angulo de giro: (steeringLock * grados->radianes) * (raw^2 con el signo de raw); deja el cuadrado en el global [0xC1C218]
    {
        float signedSquare = m_fRawSteerAngle * m_fRawSteerAngle;
        if (m_fRawSteerAngle < 0.f) {
            signedSquare = -signedSquare;
        }
        s_signedSquareScratch = signedSquare;
        m_fSteerAngle = (m_pHandlingData->m_fSteeringLock * std::bit_cast<float>(0x3C8EFA35u)) * signedSquare; // [0x8595EC]
    }

    //> 0x6ADD28 - Rear wheel steer
    if (m_pHandlingData->m_bHbRearwheelSteer) {
        if (vehicleFlags.bIsHandbrakeOn) {
            // Como el original: 0x6ADD39..0x6ADD57  0.9f * 2nd - (1.0f - 0.9f) * steer  (antes: lerp(2nd, -steer, 0.9f) = 0.1 * 2nd - 0.9 * steer)
            const float k = std::bit_cast<float>(0x3F666666u); // [0x8D3208]
            m_f2ndSteerAngle = k * m_f2ndSteerAngle - (1.0f - k) * m_fSteerAngle;
        } else {
            m_f2ndSteerAngle = std::bit_cast<float>(0x3F666666u) * m_f2ndSteerAngle; // [0x8D3204]
        }
    }

    //> 0x6ADD6B - Comedy controls stuff
    if (vehicleFlags.bComedyControls) {
        // Como el original: 0x6ADD74 rand() % 10 con signo (antes GetRandomNumberInRange(0, 10), que usa 1/32767 y float)
        const int32 rndnum = (int32)CGeneral::GetRandomNumber() % 10;

        using enum eComedyControlState;
        if (m_comedyControlState == INACTIVE) {
            if (rndnum < 2) {
                m_comedyControlState = STEER_RIGHT;
            } else if (rndnum < 4) {
                m_comedyControlState = STEER_LEFT;
            }
        } else if (m_comedyControlState == STEER_RIGHT) {
            m_fSteerAngle = m_fSteerAngle + 0.05f;
            if (rndnum < 2) {
                m_comedyControlState = INACTIVE;
            }
        } else if (m_comedyControlState == STEER_LEFT) {
            m_fSteerAngle = m_fSteerAngle - 0.05f;
            if (rndnum < 2) {
                m_comedyControlState = INACTIVE;
            }
        }
    } else {
        // Como el original: 0x6ADD6B test byte [esi+0x429],10h / je 0x6ADDBC: mov byte [esi+0x51A],0 (sin controles comicos el estado se pone a INACTIVE)
        m_comedyControlState = eComedyControlState::INACTIVE;
    }

    //> 0x6ADDC3 - Slow player down in some cases
    if ((CPad::GetPad(0)->DisablePlayerControls != 0 && CGameLogic::SkipState != 2) || CPad::GetPad(0)->bApplyBrakes) {
        vehicleFlags.bIsHandbrakeOn = true;

        m_BrakePedal = 1.f;
        m_GasPedal = 0.f;
        FindPlayerPed()->KeepAreaAroundPlayerClear();
        // Como el original: 0x6ADE25..0x6ADE87 magnitud con raiz ((x*x + y*y) + z*z) > 0.28f ([0x871254]); se escala con 0.28f / magnitud
        const float mag = GetMoveSpeed().Magnitude();
        if (mag > std::bit_cast<float>(0x3E8F5C29u)) {
            const float scale = std::bit_cast<float>(0x3E8F5C29u) / mag;
            m_vecMoveSpeed.x = scale * m_vecMoveSpeed.x;
            m_vecMoveSpeed.y = scale * m_vecMoveSpeed.y;
            m_vecMoveSpeed.z = scale * m_vecMoveSpeed.z;
        }
    }

    //> 0x6ADEB3
    if (vehicleFlags.bEngineBroken) {
        vehicleFlags.bIsHandbrakeOn = false;

        m_BrakePedal = 0.05f;
        m_GasPedal = 0.f;
    }
}

// 0x6A2210
void CAutomobile::GetComponentWorldPosition(int32 componentId, CVector& outPos)
{
    if (m_aCarNodes[componentId]) {
        outPos = RwFrameGetLTM(m_aCarNodes[componentId])->pos;
    }
}

// 0x6A2250
bool CAutomobile::IsComponentPresent(int32 componentId) const
{
    return m_aCarNodes[componentId] != 0;
}

// 0x6A6AE0
void CAutomobile::OpenDoor(CPed* ped, int32 nodeIdx, eDoors doorIdx, float doorOpenRatio, bool playSound) {
    const auto frame = m_aCarNodes[nodeIdx];
    if (!frame) {
        return;
    }

    CMatrix frameMat{ RwFrameGetMatrix(frame) }; // Is it necessary to do this here?
    auto& door = m_doors[doorIdx];

    const auto wasClosed = door.IsClosed();
    if (wasClosed) {
        RwFrameForAllObjects( // TODO: Make function (See refs to `SetAtomicFlagCB` and `ClearAtomicFlagCB` and combine them somehow)
            frame,
            CVehicleModelInfo::ClearAtomicFlagCB,
            (void*)(eAtomicComponentFlag::ATOMIC_DONT_CULL)
        );
    }

    door.Open(doorOpenRatio);

    const auto ProcessSound = [&](eAudioEvents baseAE, float eventQuietVolume) {
        if (!playSound) {
            return;
        }
        m_vehicleAudio.AddAudioEvent((eAudioEvents)((size_t)baseAE + (size_t)doorIdx), 0.f);
        if (!ped) {
            return;
        }
        GetEventGlobalGroup()->Add(CEventSoundQuiet{ ped, eventQuietVolume, (uint32)-1, {} });
    };

    if (wasClosed && !door.IsClosed()) {
        RwFrameForAllObjects( // TODO: Make function (See refs to `SetAtomicFlagCB` below)
            frame,
            CVehicleModelInfo::SetAtomicFlagCB,
            (void*)(eAtomicComponentFlag::ATOMIC_DONT_CULL)
        );
        ProcessSound(AE_CAR_BONNET_OPEN, 60.f);
    } else if (!wasClosed && doorOpenRatio == 0.f) {
        m_damageManager.SetDoorClosed(doorIdx);
        ProcessSound(AE_CAR_BONNET_CLOSE, 120.f);
    }

    // Como el original (0x6A6D2C..0x6A6D85): orien[eje] = angulo; SetRotate y suma de la posicion guardada
    G2UpdateDoorFrameMatrix(door, frameMat);
}

// 0x6A2270
float CAutomobile::GetDooorAngleOpenRatio(eDoors door)
{
    const auto doorId = (int32)door;
    return m_doors[doorId].GetAngleOpenRatio();
}

// 0x6A62C0
float CAutomobile::GetDooorAngleOpenRatioU32(uint32 door)
{
    switch (door) {
    case 8:
        return GetDooorAngleOpenRatio(DOOR_RIGHT_FRONT);
    case 9:
        return GetDooorAngleOpenRatio(DOOR_RIGHT_REAR);
    case 10:
        return GetDooorAngleOpenRatio(DOOR_LEFT_FRONT);
    case 11:
        return GetDooorAngleOpenRatio(DOOR_LEFT_REAR);
    case 18:
        return 0.0f;
    default:
        assert(0); // Prevent usage mistakes - There's is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return 0.0f;
    }
}

// 0x6A2290
bool CAutomobile::IsDoorReady(eDoors door)
{
    if (m_doors[door].IsClosed())
        return true;

    if (IsDoorMissing(door))
        return true;

    return false;
}

// 0x6A6350
bool CAutomobile::IsDoorReadyU32(uint32 door)
{
    switch (door) {
    case 8:
        return IsDoorReady(DOOR_RIGHT_FRONT);
    case 9:
        return IsDoorReady(DOOR_RIGHT_REAR);
    case 10:
        return IsDoorReady(DOOR_LEFT_FRONT);
    case 11:
        return IsDoorReady(DOOR_LEFT_REAR);
    case 18:
        return true;
    default:
        assert(0); // Prevent usage mistakes - There's is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return false;
    }
}

// 0x6A22D0
bool CAutomobile::IsDoorFullyOpen(eDoors door)
{
    if (m_doors[door].IsFullyOpen())
        return true;

    if (IsDoorMissing(door))
        return true;

    return false;
}

// 0x6A63E0
bool CAutomobile::IsDoorFullyOpenU32(uint32 door) {
    switch (door) {
    case 8:  return IsDoorFullyOpen(DOOR_RIGHT_FRONT);
    case 9:  return IsDoorFullyOpen(DOOR_RIGHT_REAR);
    case 10: return IsDoorFullyOpen(DOOR_LEFT_FRONT);
    case 11: return IsDoorFullyOpen(DOOR_LEFT_REAR);
    case 18: return false;
    default:
        assert(0); // Prevent usage mistakes - There's is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return false;
    }
}

// 0x6A2310
bool CAutomobile::IsDoorClosed(eDoors door) {
    return m_doors[door].IsClosed();
}

// 0x6A6470
bool CAutomobile::IsDoorClosedU32(uint32 door) {
    switch (door) {
    case 8:  return IsDoorClosed(DOOR_RIGHT_FRONT);
    case 9:  return IsDoorClosed(DOOR_RIGHT_REAR);
    case 10: return IsDoorClosed(DOOR_LEFT_FRONT);
    case 11: return IsDoorClosed(DOOR_LEFT_REAR);
    case 18: return false;
    default:
        assert(0); // Prevent usage mistakes - There's is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return false;
    }
}

// 0x6A2330
bool CAutomobile::IsDoorMissing(eDoors door) {
    return m_damageManager.GetDoorStatus(door) == DAMSTATE_NOTPRESENT;
}

// 0x6A6500
bool CAutomobile::IsDoorMissingU32(uint32 door) {
    switch (door) {
    case 8:  return IsDoorMissing(DOOR_RIGHT_FRONT);
    case 9:  return IsDoorMissing(DOOR_RIGHT_REAR);
    case 10: return IsDoorMissing(DOOR_LEFT_FRONT);
    case 11: return IsDoorMissing(DOOR_LEFT_REAR);
    case 18: return true;
    default:
        assert(0); // Prevent usage mistakes - There is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return false;
    }
}

// 0x6A2350
bool CAutomobile::IsOpenTopCar() {
    return m_nModelIndex == MODEL_COMET    && m_anExtras[0] && m_anExtras[1]
        || m_nModelIndex == MODEL_STALLION && m_anExtras[0] && m_anExtras[1];
}

// 0x6A3030
void CAutomobile::RemoveRefsToVehicle(CEntity* entity) {
    for (auto& ref : m_apWheelCollisionEntity) {
        if (ref == entity)
            ref = nullptr;
    }
}

// NOTSA - Combined implementation of `BlowUpCar` and `BlowUpCarCutSceneNoExtras`
void CAutomobile::BlowUpCar_Impl(CEntity* dmgr, bool bDontShakeCam, bool bDontSpawnStuff, bool bNoExplosion, bool bHideExplosionFx, bool bIsForCutScene, bool bMakeSound) {
    if (!vehicleFlags.bCanBeDamaged) {
        return;
    }

    if (bIsForCutScene) {
        if (IsSubPlane() && GetStatus() != STATUS_PLAYER) {
            switch (m_autoPilot.m_nCarMission) {
            case MISSION_PLANE_FLYTOCOORS:
            case MISSION_PLANE_ATTACK_PLAYER:
            case MISSION_PLANE_FLYINDIRECTION:
            case MISSION_PLANE_FOLLOW_ENTITY:
            case MISSION_PLANE_ATTACK_PLAYER_POLICE:
            case MISSION_PLANE_DOG_FIGHT_ENTITY:
            case MISSION_PLANE_DOG_FIGHT_PLAYER:
                // Como el original: 0x6B3BED mov byte [esi+0x3BA],0x39 (solo se escribe la mision; SetCarMission tambien toca m_nTimeToStartMission)
                m_autoPilot.m_nCarMission = MISSION_PLANE_CRASH_AND_BURN;
                return;
            }
        }
    } else {
        if (dmgr == FindPlayerPed() || dmgr == FindPlayerVehicle()) {
            auto& plyrinfo = FindPlayerInfo();
            plyrinfo.m_nHavocCaused += 20;
            plyrinfo.m_fCurrentChaseValue += 10.f;
            // Como el original: 0x6B37DF..0x6B37FB (rand() % 6000 + 4000 como entero -> fild -> float)
            CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, (float)(int32)(CGeneral::GetRandomNumber() % 6000 + 4000));
        }
    }

    if (m_nModelIndex == eModelID::MODEL_VCNMAV) {
        CWanted::UseNewsHeliInAdditionToPolice = false;
    }

    if (!bNoExplosion) {
        GetMoveSpeed().z += 0.13f;
    }

    SetStatus(STATUS_WRECKED);

    physicalFlags.bRenderScorched = true;
    m_nTimeWhenBlowedUp      = CTimer::GetTimeInMS();

    CVisibilityPlugins::SetClumpForAllAtomicsFlag(GetRpClump(), eAtomicComponentFlag::ATOMIC_PIPE_NO_EXTRA_PASSES);
    // Como el original: 0x6B3C4E..0x6B3C5C push bNoExplosion (en BlowUpCar es siempre 0)
    m_damageManager.FuckCarCompletely(bNoExplosion);

    // Como el original: BlowUpCar (0x6B3861) compara con RCTIGER (0x234) y RCBANDIT (0x1B9); la version de cutscene (0x6B3C61) solo con RCBANDIT
    const auto isRcShit = !bIsForCutScene
        ? notsa::contains({ MODEL_RCTIGER, MODEL_RCBANDIT }, GetModelId())
        : GetModelId() == MODEL_RCBANDIT;
    if (!isRcShit) { // 0x6B3C61
        for (auto bumper : { FRONT_BUMPER, REAR_BUMPER }) {
            SetBumperDamage(bumper, bDontSpawnStuff);
        }
        for (auto door : { DOOR_BONNET, DOOR_BOOT, DOOR_LEFT_FRONT, DOOR_RIGHT_FRONT, DOOR_LEFT_REAR, DOOR_RIGHT_REAR }) {
            SetDoorDamage(door, bDontSpawnStuff);
        }
        if (!bDontSpawnStuff) {
            SpawnFlyingComponent(CAR_WHEEL_LF, 1);
        }
        if (!bNoExplosion) {
            if (const auto obj = GetCurrentAtomicObject(m_aCarNodes[CAR_WHEEL_LF])) {
                RpAtomicSetFlags(obj, 0); // TODO: Use appropriate enum (if any?)
            }
        }
    }

    m_nBombOnBoard = 0;
    m_fHealth      = 0.f;
    m_wBombTimer   = 0;

    if (!bDontShakeCam) {
        TheCamera.CamShake(0.4f, GetPosition());
    }

    KillPedsInVehicle();
    if (!bIsForCutScene) { // Como el original: 0x6B39E6 solo en BlowUpCar (no en BlowUpCarCutSceneNoExtras)
        KillPedsGettingInVehicle();
    }

    vehicleFlags.bLightsOn     = false;
    vehicleFlags.bEngineOn     = false;
    vehicleFlags.bSirenOrAlarm = false;
    m_nOverrideLights          = NO_CAR_LIGHT_OVERRIDE;
    autoFlags.bTaxiLight     = false;

    if (vehicleFlags.bIsAmbulanceOnDuty) { //> 0x6B3DAE
        assert(!vehicleFlags.bIsFireTruckOnDuty);
        vehicleFlags.bIsAmbulanceOnDuty = false;
        CCarCtrl::NumAmbulancesOnDuty--;
    }

    if (vehicleFlags.bIsFireTruckOnDuty) {
        assert(!vehicleFlags.bIsAmbulanceOnDuty);
        vehicleFlags.bIsFireTruckOnDuty = false;
        CCarCtrl::NumFireTrucksOnDuty--;
    }

    ChangeLawEnforcerState(false);

    if (!bNoExplosion) {
        // Como el original: BlowUpCar (0x6B39D1..0x6B39E6) llama primero a StartFire y despues a Darkel; la version de cutscene (0x6B3DE7..0x6B3E08) al reves
        if (bIsForCutScene) {
            CDarkel::RegisterCarBlownUpByPlayer(*this, 0); // Um.... Okay
            gFireManager.StartFire(this, dmgr);
        } else {
            gFireManager.StartFire(this, dmgr);
            CDarkel::RegisterCarBlownUpByPlayer(*this, 0);
        }

        if (bIsForCutScene) { //> 0x6B3E0D
            CExplosion::AddExplosion(this, dmgr, EXPLOSION_MOLOTOV, GetPosition(), 0, bMakeSound, -1.f, bHideExplosionFx);
        } else { //> 0x6B3A17 - Add explosion fx (cancer inducing code)
            const auto maxOffset = IsSubAutomobile() || IsSubQuad()
                ? 0.75f
                : 0.1f;
            // Como el original: 0x6B3A2E..0x6B3B47. Cada componente usa rand() * [0x858C7C] (1/32767); x e y: (max - (-max)) * t + (-max);
            // la tercera llamada a rand() NO se acota por maxOffset: z = (t + 0.5f) * boundBox.max.z (antes: (maxOffset + 0.5f) * max.z sin azar)
            const float minOffset = -maxOffset;
            const auto RandomT = []() { return (float)(int32)CGeneral::GetRandomNumber() * std::bit_cast<float>(0x38000100u); };
            const float tx = RandomT();
            const float ox = ((maxOffset - minOffset) * tx + minOffset) * GetColModel()->GetBoundingBox().m_vecMax.x;
            const float ty = RandomT();
            const float oy = ((maxOffset - minOffset) * ty + minOffset) * GetColModel()->GetBoundingBox().m_vecMax.y;
            const float tz = RandomT();
            const float oz = (tz + 0.5f) * GetColModel()->GetBoundingBox().m_vecMax.z;
            const CVector& right   = GetRight();
            const CVector& forward = GetForward();
            const CVector& pos     = GetPosition();
            const CVector explPos{
                (pos.x + ox * right.x) + oy * forward.x,
                (pos.y + ox * right.y) + oy * forward.y,
                ((pos.z + ox * right.z) + oy * forward.z) - oz
            };
            CExplosion::AddExplosion(
                this,
                dmgr,
                isRcShit ? EXPLOSION_QUICK_CAR : EXPLOSION_CAR,
                explPos,
                0,
                bMakeSound,
                -1.f,
                bHideExplosionFx
            );
        }
    }
}

// 0x6B3780
void CAutomobile::BlowUpCar(CEntity* dmgr, bool bHideExplosionFx) {
    BlowUpCar_Impl(dmgr, false, false, false, bHideExplosionFx, false, true);
}

// 0x6B3BB0
void CAutomobile::BlowUpCarCutSceneNoExtras(bool bDontShakeCam, bool bDontSpawnStuff, bool bNoExplosion, bool bMakeSound) {
    BlowUpCar_Impl(nullptr, bDontShakeCam, bDontSpawnStuff, bNoExplosion, false, true, bMakeSound);
}

// 0x6A3060
bool CAutomobile::SetUpWheelColModel(CColModel* wheelCol) {
    const auto mi = GetVehicleModelInfo();
    const auto wcm = GetColModel();
    const auto wcd = wheelCol->m_pColData;

    if (m_nModelIndex == MODEL_TRACTOR || m_nModelIndex == MODEL_COMBINE || m_nModelIndex == MODEL_KART)
        return false;

    wheelCol->m_boundBox = wcm->m_boundBox;
    wheelCol->m_boundSphere = wcm->m_boundSphere;

    CMatrix mat;

    mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LF]), false);
    wcd->m_pSpheres[0].Set(mi->m_fWheelSizeFront / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0xD);

    mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LB]), false);
    wcd->m_pSpheres[1].Set(mi->m_fWheelSizeRear / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0xF);

    mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RF]), false);
    wcd->m_pSpheres[2].Set(mi->m_fWheelSizeFront / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0xE);

    mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RB]), false);
    wcd->m_pSpheres[3].Set(mi->m_fWheelSizeRear / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0x10);

    if (m_aCarNodes[CAR_WHEEL_LM] && m_aCarNodes[CAR_WHEEL_RM]) {
        mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LM]), false);
        wcd->m_pSpheres[4].Set(mi->m_fWheelSizeRear / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0xF);

        mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RM]), false);
        wcd->m_pSpheres[5].Set(mi->m_fWheelSizeRear / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0x10);

        wcd->m_nNumSpheres = 6;
    } else {
        wcd->m_nNumSpheres = 4;
    }
    return true;
}

// 0x6A32B0
bool CAutomobile::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    if (m_nModelIndex == MODEL_RHINO)
        return false;

    if (vehicleFlags.bTyresDontBurst)
        return false;

    if (physicalFlags.bRenderScorched)
        return false;

    const auto GetCarWheel = [&]() -> eCarWheel { // TODO: `CarPieceToCarWheel` (See `CarWheelToCarPiece`)
        switch (tyreComponentId) {
        case CAR_PIECE_WHEEL_LF: return CAR_WHEEL_FRONT_LEFT;
        case CAR_PIECE_WHEEL_RF: return CAR_WHEEL_FRONT_RIGHT;
        case CAR_PIECE_WHEEL_RL: return CAR_WHEEL_REAR_LEFT;
        case CAR_PIECE_WHEEL_RR: return CAR_WHEEL_REAR_RIGHT;
        default:
            return (eCarWheel)tyreComponentId; // TODO: NOTSA_UNREACHABLE()
        }
    };

    auto wheel = GetCarWheel();
    if (m_damageManager.GetWheelStatus(wheel))
        return false;

    m_damageManager.SetWheelStatus(wheel, WHEEL_STATUS_BURST);
    CStats::IncrementStat(STAT_TIRES_POPPED_WITH_GUNFIRE, 1.0f);
    m_vehicleAudio.AddAudioEvent(AE_TYRE_BURST, 0.0f);
    if (GetStatus() == STATUS_SIMPLE) {
        CCarCtrl::SwitchVehicleToRealPhysics(this);
    }

    constexpr auto force = 0.03f;
    if (bPhysicalEffect) {
        ApplyMoveForce(m_matrix->GetRight() * CGeneral::GetRandomNumberInRange(-force, force) * m_fMass);
        ApplyTurnForce(
            m_matrix->GetRight() * CGeneral::GetRandomNumberInRange(-force, force) * m_fTurnMass,
            m_matrix->GetForward()
        );

        m_nFakePhysics = 0;
    }
    return true;
}

// 0x6A3850
bool CAutomobile::IsRoomForPedToLeaveCar(uint32 arg0, CVector* arg1) {
    return true;
}

// 0x6A65D0
void CAutomobile::SetupSuspensionLines() {
    const auto& mi = *GetVehicleModelInfo();
    // Como el original (0x6A65EA): CEntity::GetColModel (0x535300) del propio vehiculo, no el del model info
    auto& cm = *GetColModel();
    auto& cd = *cm.m_pColData;

    // 0x6A65FB
    // Como el original (0x6A65F6..0x6A6629): se reserva si el puntero m_pLines es nulo (no si m_nNumLines es cero)
    bool hadToAllocateLines = false;
    if (!cd.m_pLines) {
        hadToAllocateLines = true;
        cd.AllocateLines(ModelIndices::IsRhino(m_nModelIndex) ? NUM_RHINO_SUSP_LINES : NUM_AUTOMOBILE_SUSP_LINES);
    }

    // 0x6A6754
    // Calculate col line positions for wheels, set wheel explPos, spring and line lengths
    const auto& handling = *m_pHandlingData;
    for (auto i = 0; i < NUM_AUTOMOBILE_SUSP_LINES; i++) {
        auto& colLine = cd.m_pLines[i]; // Collision line for this wheel

        CVector wheelPos;
        mi.GetWheelPosn(i, wheelPos, false);

        // Como el original (0x6A665C..0x6A6681): fcomp con 0 + test ah,41h; si x > 0 se suma 0.15f, si no (incluido NaN) se resta
        if (IsSubQuad()) {
            if (wheelPos.x > 0.f)
                wheelPos.x += 0.15f;
            else
                wheelPos.x -= 0.15f;
        }

        m_wheelPosition[i] = wheelPos.z; // Como el original (0x6A668D): antes de sumar los limites de suspension

        // Como el original (0x6A6696..0x6A670B): inicio z = z + limite superior; fin z = ((inferior - superior) - rueda * 0.5f) + inicio z
        const float startZ = wheelPos.z + handling.m_fSuspensionUpperLimit;
        colLine.m_vecStart = CVector{ wheelPos.x, wheelPos.y, startZ };
        const float limitsDiff = handling.m_fSuspensionLowerLimit - handling.m_fSuspensionUpperLimit;
        const float endZ = (limitsDiff - mi.GetSizeOfWheel((eCarWheel)i) * 0.5f) + startZ;
        colLine.m_vecEnd   = CVector{ wheelPos.x, wheelPos.y, endZ };

        m_aSuspensionSpringLength[i] = handling.m_fSuspensionUpperLimit - handling.m_fSuspensionLowerLimit;
        m_aSuspensionLineLength[i]   = colLine.m_vecStart.z - colLine.m_vecEnd.z;
    }

    /* NOP code - 0x6A6764
    if (GetVehicleAppearance() != eVehicleAppearance::VEHICLE_APPEARANCE_PLANE) {
        GetVehicleAppearance();
    }*/

    // Como el original (0x6A676D..0x6A67E3): k*spring + (rueda*0.5f - inicio.z), con k = 1 - 1/(fuerza*4); la delantera usa la rueda 0 y la
    // trasera la rueda 3 (muelle [3] y linea 3, 0x884 y 0x60+8) con el tamano trasero (mi+0x44)
    const float forceTerm = 1.f - 1.f / (handling.m_fSuspensionForceLevel * 4.f);
    m_fFrontHeightAboveRoad = forceTerm * m_aSuspensionSpringLength[(size_t)eCarWheel::CAR_WHEEL_FRONT_LEFT]
        + (mi.GetSizeOfWheel(eCarWheel::CAR_WHEEL_FRONT_LEFT) * 0.5f - cd.m_pLines[(size_t)eCarWheel::CAR_WHEEL_FRONT_LEFT].m_vecStart.z);
    m_fRearHeightAboveRoad = forceTerm * m_aSuspensionSpringLength[(size_t)eCarWheel::CAR_WHEEL_REAR_RIGHT]
        + (mi.GetSizeOfWheel(eCarWheel::CAR_WHEEL_REAR_RIGHT) * 0.5f - cd.m_pLines[(size_t)eCarWheel::CAR_WHEEL_REAR_RIGHT].m_vecStart.z);

    // 0x6A681A
    // Adjust wheel's position based on height above road
    for (auto i = 0u; i < std::size(m_wheelPosition); i++) {
        m_wheelPosition[i] = mi.GetSizeOfWheel((eCarWheel)i) * 0.5f - m_fFrontHeightAboveRoad; // Not sure why it uses front height for all wheels?
    }

    // 0x6A681C
    // Como el original (0x6A681F..0x6A68B4): la comparacion solo condiciona la asignacion de min.z; el radio se recalcula siempre
    if (cd.m_pLines[0].m_vecEnd.z < cm.m_boundBox.m_vecMin.z) {
        cm.m_boundBox.m_vecMin.z = cd.m_pLines[0].m_vecEnd.z; // Adjust bounding box (mov [ecx+8], edx)
    }
    {
        // Radio de la esfera: modulo de min y de max (raiz de (x*x + y*y) + z*z) y el mayor de ambos (min solo si min > max) frente al radio
        const auto& bbMin = cm.m_boundBox.m_vecMin;
        const auto& bbMax = cm.m_boundBox.m_vecMax;
        const float magMin = std::sqrt((bbMin.x * bbMin.x + bbMin.y * bbMin.y) + bbMin.z * bbMin.z);
        const float magMax = std::sqrt((bbMax.x * bbMax.x + bbMax.y * bbMax.y) + bbMax.z * bbMax.z);
        const float candidate = (magMin > magMax) ? magMin : magMax;
        if (candidate > cm.m_boundSphere.m_fRadius) {
            cm.m_boundSphere.m_fRadius = candidate;
        }
    }

    // 0x6A68C1
    // Adjust sphere sizes for rcbandit
    if (ModelIndices::IsRCBandit(m_nModelIndex)) {
        cm.m_boundSphere.m_fRadius = 2.f;

        for (auto&& sph : cd.GetSpheres()) {
            sph.m_fRadius = 0.3f;
        }
    }

    // 0x6A68FD
    if (handling.m_bForceGroundClearance && hadToAllocateLines) {
        const auto sphBottomMinZ = (ModelIndices::IsKart(m_nModelIndex) ? 0.12f : 0.25f) - m_fFrontHeightAboveRoad;
        for (auto&& sph : cd.GetSpheres()) {
            const auto sphBottomZ = sph.m_vecCenter.z - sph.m_fRadius;
            if (sphBottomZ < sphBottomMinZ) {
                if (sph.m_fRadius > 0.4f) {
                    // Como el original (0x6A6958..0x6A6987): usa (centro.z - minimo) y no (centro.z - radio): radio = max(0.4f, c.z - minimo)
                    // con fcomp (0.4f > valor -> 0.4f) y centro.z = minimo + radio
                    const float candidateRadius = sph.m_vecCenter.z - sphBottomMinZ;
                    sph.m_fRadius = (0.4f > candidateRadius) ? 0.4f : candidateRadius;
                }
                // Como el original (0x6A6956 jne 0x6A6979): la posicion z se recalcula SIEMPRE que bottom < minimo, tambien si radio <= 0.4f
                sph.m_vecCenter.z = sphBottomMinZ + sph.m_fRadius;
            }
        }
    }

    // 0x6A6995
    // Rhino has 12 suspension lines, which are calculated based on the 4 main wheel's line's position
    if (ModelIndices::IsRhino(m_nModelIndex)) {
        size_t lineIndex{ NUM_AUTOMOBILE_SUSP_LINES }; // Skip first 4 lines as those are already in use for the real wheels

        for (auto i = 0u; i < NUM_AUTOMOBILE_SUSP_LINES; i += NUM_AUTOMOBILE_SUSP_LINES / 2) { // Do left, and right side - Wheels 0, 1 are on the left side, while 2, 3 are on the right
            for (auto j = 0u; j < NUM_AUTOMOBILE_SUSP_LINES; j++, lineIndex++) { // Add 4 extra lines on each side
                // Como el original (0x6A69C3..0x6A69CD): (j + 1) * 0.2f (0x858CC4), no (j + 1) / 5; y u = 1 - t
                const float wheelRelativePos = (float)(j + 1u) * 0.2f;
                const float inverseRelativePos = 1.f - wheelRelativePos;

                // Calculate positions of this wheel's line by lerping between the 2 wheel's lines on this side
                // Como el original (0x6A69D7..0x6A6AB3): componente a componente u*desde + t*hasta
                auto& line      = cd.m_pLines[lineIndex];
                const auto& from = cd.m_pLines[i];
                const auto& to   = cd.m_pLines[i + 1];
                line.m_vecStart = CVector{
                    inverseRelativePos * from.m_vecStart.x + wheelRelativePos * to.m_vecStart.x,
                    inverseRelativePos * from.m_vecStart.y + wheelRelativePos * to.m_vecStart.y,
                    inverseRelativePos * from.m_vecStart.z + wheelRelativePos * to.m_vecStart.z
                };
                line.m_vecEnd = CVector{
                    inverseRelativePos * from.m_vecEnd.x + wheelRelativePos * to.m_vecEnd.x,
                    inverseRelativePos * from.m_vecEnd.y + wheelRelativePos * to.m_vecEnd.y,
                    inverseRelativePos * from.m_vecEnd.z + wheelRelativePos * to.m_vecEnd.z
                };
            }
        }
    }
}

// 0x6A3440
void CAutomobile::Fix() {
    m_damageManager.ResetDamageStatus();

    // Reset actual doorIdx's status
    // Como el original (0x6A3471): test ch,1 sobre el dword 0xCC del handling = m_bNoDoors (bit 8 de las banderas de modelo)
    if (m_pHandlingData->m_bNoDoors) {
        m_damageManager.SetDoorStatus({ DOOR_LEFT_FRONT, DOOR_RIGHT_FRONT, DOOR_LEFT_REAR, DOOR_RIGHT_REAR }, eDoorStatus::DAMSTATE_NOTPRESENT);
    }

    vehicleFlags.bIsDamaged = false;

    // Hide all DAM state atomics
    RpClumpForAllAtomics(GetRpClump(), CVehicleModelInfo::HideAllComponentsAtomicCB, (void*)eAtomicComponentFlag::ATOMIC_DAMAGED);

    // Reset rotation of some nodes
    for (auto i = (size_t)CAR_DOOR_RF; i < (size_t)CAR_NUM_NODES; i++) {
        if (auto frame = m_aCarNodes[i]) {
            CMatrix mat{ RwFrameGetMatrix(frame), false };
            mat.SetTranslate(mat.GetPosition());
            mat.UpdateRW();
        }
    }

    // Set all wheels
    m_damageManager.SetAllWheelsState(eCarWheelStatus::WHEEL_STATUS_OK);

    // Reset all bouncing panels
    // Como el original (0x6A3555..0x6A356D): 4 entradas de 0x20 bytes desde 0x6AC; la cuarta es el principio de m_swingingChassis
    // (los 2 bytes bajos de m_openAngle se ponen a 0xFFFF)
    {
        auto* const bouncing = reinterpret_cast<uint8*>(m_panels.data());
        for (size_t i = 0; i < 4; i++) {
            *reinterpret_cast<uint16*>(bouncing + i * sizeof(CBouncingPanel)) = (uint16)-1;
        }
    }
}

// 0x6B3E90
void CAutomobile::SetupDamageAfterLoad() {
    // TODO: Moderinfy....

    // Como el original (0x6B3E93..0x6B3EE5): el orden de las llamadas es parachoques delantero, capo, parachoques trasero, maletero
    if (m_aCarNodes[CAR_BUMP_FRONT]) SetBumperDamage(FRONT_BUMPER, false);
    if (m_aCarNodes[CAR_BONNET])  SetDoorDamage(DOOR_BONNET, false);
    if (m_aCarNodes[CAR_BUMP_REAR])  SetBumperDamage(REAR_BUMPER, false);
    if (m_aCarNodes[CAR_BOOT])    SetDoorDamage(DOOR_BOOT, false);
    if (m_aCarNodes[CAR_DOOR_LF]) SetDoorDamage(DOOR_LEFT_FRONT, false);
    if (m_aCarNodes[CAR_DOOR_RF]) SetDoorDamage(DOOR_RIGHT_FRONT, false);
    if (m_aCarNodes[CAR_DOOR_LR]) SetDoorDamage(DOOR_LEFT_REAR, false);
    if (m_aCarNodes[CAR_DOOR_RR]) SetDoorDamage(DOOR_RIGHT_REAR, false);

    if (m_aCarNodes[CAR_WING_LF]) SetPanelDamage(FRONT_LEFT_PANEL, false);
    if (m_aCarNodes[CAR_WING_RF]) SetPanelDamage(FRONT_RIGHT_PANEL, false);
}

// 0x6A47F0
void CAutomobile::DoBurstAndSoftGroundRatios()
{
    const auto& mi = *GetVehicleModelInfo();
    const auto speedToFwdRatio = std::fabs(DotProduct(m_vecMoveSpeed, m_matrix->GetForward())); // Remember: Dot product is 0 when the two vectors are perpendicular - TODO: Rename var to fwdvel (forward velocity)

    for (auto i = 0u; i < 4; i++) {
        const auto& wheelCP = m_wheelColPoint[i];
        const auto& springLen = m_aSuspensionSpringLength[i];
        const auto& lineLen = m_aSuspensionLineLength[i];
        auto& compression = m_fWheelsSuspensionCompression[i];
        auto& rotation = m_wheelRotation[i];

        const auto GetRemainingSuspensionCompression = [&] {
            return (lineLen - springLen) / lineLen;
        };

        switch (m_damageManager.GetWheelStatus((eCarWheel)i)) {
        case eCarWheelStatus::WHEEL_STATUS_MISSING:
            compression = 1.0f;
            break;
        case eCarWheelStatus::WHEEL_STATUS_BURST: {
            // The more opposite the speedsq is to the forward vector the bigger chance
            // The highest chance is when the speedsq is opposite to forward (ie.: It's backwards)
            // Como el original (0x6A4870..0x6A48AD): rand() * (1/32768) * (n + 98) en x87 con n = (uint16)_ftol(ratio * 40)
            // (movzx eax,ax tras _ftol2), es decir GetRandomIntInRange(0, n + 98) y no la plantilla entera de 1/32767
            const auto val = CGeneral::GetRandomIntInRange(0, int32(uint16(int32(speedToFwdRatio * 40.0f))) + 98);
            if (val < 100) {
                compression += GetRemainingSuspensionCompression() / 4.f;
                compression = std::min(1.0f, compression);
            }
            break;
        }
        default: {
            // Como el original (0x6A4870): fcomp 1.0 + test ah,5 / jp: con compresion NaN va a la rama de la via
            if (   !(compression < 1.0f)
                || g_surfaceInfos.GetAdhesionGroup(wheelCP.m_nSurfaceTypeB) != ADHESION_GROUP_SAND
                || ModelIndices::IsRhino(m_nModelIndex)
            ) {
                if (compression < 1.0f && wheelCP.m_nSurfaceTypeB == SURFACE_RAILTRACK) {
                    auto wheelSizeFactor = 1.5f / (mi.GetSizeOfWheel((eCarWheel)i) / 2.0f);
                    if (speedToFwdRatio > 0.3f) {
                        wheelSizeFactor *= speedToFwdRatio / 0.3f;
                    }

                    auto wheelSizeInv = 1.0f / wheelSizeFactor;
                    auto wheelRotFactor = wheelSizeInv * rotation;
                    auto wheelRotFactorFract = wheelRotFactor - std::floor(wheelRotFactor);

                    auto timeSpeedRotFactor = (CTimer::GetTimeStep() * m_wheelSpeed[i] + rotation) * wheelSizeInv;
                    auto timeSpeedRotFactorFract = timeSpeedRotFactor - std::floor(timeSpeedRotFactor);

                    if (   m_wheelSpeed[i] > 0.0f && timeSpeedRotFactorFract < wheelRotFactorFract
                        || m_wheelSpeed[i] < 0.0f && timeSpeedRotFactorFract > wheelRotFactorFract
                    ) {
                        compression = std::max(0.2f, compression - GetRemainingSuspensionCompression() * 0.3f);
                    }
                }
            } else {
                float offroadFactor = 0.3f;
                if (handlingFlags.bOffroadAbility2) {
                    offroadFactor = 0.15f;
                } else if (handlingFlags.bOffroadAbility) {
                    offroadFactor = 0.2f;
                }

                auto adhesionFactor = 1.0f - speedToFwdRatio / 0.3f * 0.7f - CWeather::WetRoads * 0.7f;
                // Como el original (0x6A496F..0x6A4977): fcomp + test ah,41h / jne: solo sustituye si 0.4 > adhesion
                if (0.4f > adhesionFactor) {
                    adhesionFactor = 0.4f;
                }

                // Como el original (0x6A4986..0x6A4995): offroad * (restante * adhesion), no (offroad * restante) * adhesion
                compression += offroadFactor * (GetRemainingSuspensionCompression() * adhesionFactor);
                compression = std::min(compression, 1.0f);
            }
            break;
        }
        }
    }
}

// 0x6A3770
void CAutomobile::PlayCarHorn()
{
    // Como el original: 0x6A377F cmp ax,0FFFFh (16 bits sin signo; antes `uint16 != -1` siempre era cierto); 0x6A3792 mov eax,[esi+0x514]; test eax,eax
    if (m_nAlarmState && m_nAlarmState != (uint16)-1 && GetStatus() != STATUS_WRECKED || m_HornCounter) {
        return;
    }

    if (m_nCarHornTimer) {
        m_nCarHornTimer -= 1;
        return;
    }

    m_nCarHornTimer = CGeneral::GetRandomNumber() % 128 - 106; // TODO: GetRandomNumberInRange
    // Como el original: 0x6A37BF and eax,7 sobre el byte nuevo (sin signo; antes % 8 con signo daba restos negativos)
    if (const auto r = (uint32)(uint8)m_nCarHornTimer & 7u; r < 4) {
        if (r >= 2) {
            if (m_pDriver && m_autoPilot.carCtrlFlags.bHonkAtCar) {
                m_pDriver->Say(CTX_GLOBAL_BLOCKED);
            }
        }
        m_HornCounter = 45;
    } else {
        if (m_pDriver) {
            m_pDriver->Say(CTX_GLOBAL_BLOCKED);
        }
    }
}

// 0x6A62B0
float CAutomobile::GetHeightAboveRoad() {
    return m_fFrontHeightAboveRoad;
}

// 0x6A62A0
int32 CAutomobile::GetNumContactWheels() {
    return m_nNumContactWheels;
}

// 0x6A7650
void CAutomobile::VehicleDamage(float damageIntensity, eVehicleCollisionComponent collisionComponent, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) {
    assert(this);

    if (!vehicleFlags.bCanBeDamaged) {
        return;
    }

    assert(m_matrix);

    // Como el original (0x6A7672 / 0x6A767A): minimo 25.0 y multiplicador 0.333 (0x864E30 = 3EAA7EFAh, no 1/3)
    float minDmgIntensity{ 25.f };
    float collForceMult{ std::bit_cast<float>(0x3EAA7EFAu) };
    float calcDmgIntensity{ damageIntensity };

    if (damageIntensity == 0.f) {
        // Man this is so stupid..
        vecCollisionDirection = &m_vecLastCollisionImpactVelocity;
        vecCollisionCoors     = &m_vecLastCollisionPosn;
        damager               = m_pDamageEntity;
        collisionComponent    = (eVehicleCollisionComponent)m_nPieceType;

        collForceMult = 1.f;
        // Como el original (0x6A76A0..0x6A76D8): (masa * 0.00066666667 (0x87123C)) * 25.0, no masa / 1500 * 25
        minDmgIntensity  = (m_fMass * std::bit_cast<float>(0x3A2EC33Eu)) * 25.f;
        calcDmgIntensity = m_fDamageIntensity;

        // 0x6A7705
        if (m_matrix->GetUp().z < 0.f/*is flipped*/ && this != FindPlayerVehicle()) {
            if (autoFlags.bDoesNotGetDamagedUpsideDown) {
                return;
            }
            // Como el original (0x6A7724..0x6A776B): el mando a distancia y el estar sumergido SIEMPRE salen; la vida baja salvo
            // en STATUS_WRECKED, con minimo 0
            if (GetStatus() == eEntityStatus::STATUS_REMOTE_CONTROLLED) {
                return;
            }
            if (physicalFlags.bSubmergedInWater) {
                return;
            }
            if (GetStatus() != eEntityStatus::STATUS_WRECKED) {
                const float newHealth = m_fHealth - CTimer::GetTimeStep() * 4.f;
                m_fHealth = (0.f > newHealth) ? 0.f : newHealth;
            }
        }

        // 0x6A7780
        if (m_fDamageIntensity == 0.f || physicalFlags.bCollisionProof) {
            return;
        }

        if (IsSubQuad()) {
            if (CBike::DamageKnockOffRider(this, m_fDamageIntensity, m_nPieceType, m_pDamageEntity, m_vecLastCollisionPosn, m_vecLastCollisionImpactVelocity)) {
                return;
            }
        }

        // 0x6A77F0
        const auto lastImpactVel_Dot_Fwd = DotProduct(m_matrix->GetForward(), m_vecLastCollisionImpactVelocity);

        // 0x6A77D6
        if (GetStatus() == STATUS_PHYSICS
            && !IsMissionVehicle()
            && lastImpactVel_Dot_Fwd < -0.4f // Impact was from behind - (-66, 66) deg
            && m_fDamageIntensity / m_fMass > 0.1f
        ) {
            m_autoPilot.SetTempAction(TEMPACT_HEADON_COLLISION, 4'000);
        }

        // Como el original (0x6A7840..0x6A7877): fuera de la zona exterior y con |dot| > 0.4 (float), un vehiculo agresor
        // multiplica por 0.333 (0x864E30) en lugar de dividir entre 3
        if (!CGame::CanSeeOutSideFromCurrArea()
            && std::fabs(lastImpactVel_Dot_Fwd) > 0.4f // Sideways impact - (142, 66) deg
            && m_pDamageEntity
            && m_pDamageEntity->GetIsTypeVehicle()
        ) {
            calcDmgIntensity = calcDmgIntensity * std::bit_cast<float>(0x3EAA7EFAu);
        }

        if (m_pDamageEntity) {
            if (m_pDamageEntity->GetType() == eEntityType::ENTITY_TYPE_PED) {
                // 0x6A7883
                if (const auto& ped = *m_pDamageEntity->AsPed(); ped.bIsStanding) {
                    const auto impactDir = DotProduct(ped.m_matrix->GetForward() * ped.m_vecAnimMovingShiftLocal.y, m_vecLastCollisionImpactVelocity);
                    if (impactDir < 0.f) { // From behind - (-90, 90) deg
                        const float pushed = impactDir * ped.m_fMass + calcDmgIntensity;
                        calcDmgIntensity = (0.f > pushed) ? 0.f : pushed;
                    }
                }
            }
        }
    } else {
        // 0x6A7BE5
        if (bool unused{}; !CanVehicleBeDamaged(damager, weapon, unused)) {
            return;
        }
    }

    // 0x6A78F0
    if (   damager && damager->GetIsTypeBuilding()
        && DotProduct(*vecCollisionDirection, m_matrix->GetUp()) > 0.6f // In front - (-66, 66) deg
    ) {
        return;
    }

    // 0x6A792B
    if (GetStatus() == STATUS_PLAYER) {
        if (CStats::GetPercentageProgress() >= 100.f) {
            calcDmgIntensity = calcDmgIntensity * 0.5f;
        }
    } else {
        if (   physicalFlags.bInvulnerable
            && damager
            && damager != FindPlayerPed()
            && damager != FindPlayerVehicle()
        ) {
            return;
        }
    }

    // 0x6A7984
    if (damager && (damager == m_pTowingVehicle || damager == m_pVehicleBeingTowed)) {
        return;
    }

    // 0x6A79AB (Condition inverted)
    if (calcDmgIntensity > minDmgIntensity && GetStatus() != STATUS_WRECKED) {
        // 0x6A79C7
        // If we're a law enforcer, and the dmgr is the
        // player's plyrveh increase their wanted level.
        // Como el original (0x6A79C0..0x6A7A54): el EXE vuelve a pedir FindPlayerVehicle en cada comparacion (4 llamadas) y usa
        // la magnitud (no la cuadrada) de ambos vehiculos
        if (vehicleFlags.bIsLawEnforcer) {
            if (FindPlayerVehicle() && damager == FindPlayerVehicle() && GetStatus() != STATUS_ABANDONED) {
                const auto vehicle = FindPlayerVehicle();
                const float thisSpeedMag = m_vecMoveSpeed.Magnitude();
                if (!(vehicle->m_vecMoveSpeed.Magnitude() < thisSpeedMag)
                    && FindPlayerVehicle()->m_vecMoveSpeed.Magnitude() > 0.1f
                ) {
                    FindPlayerPed()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_1);
                }
            }
        }

        // 0x6A7A59
        // Some shaky-shaky
        if (GetStatus() == STATUS_PLAYER && calcDmgIntensity > 50.f) {
            float intensityF = calcDmgIntensity / m_fMass * 800.f + 100.f;
            if (250.f < intensityF) {
                intensityF = 250.f;
            }
            const auto intensity = (uint8)(int32)intensityF; // _ftol2 y guarda solo el byte bajo
            CPad::GetPad()->StartShake(40000 / intensity, intensity, 2000u);
        }

        // 0x6A7ACF
        if (damager && damager->GetIsTypeVehicle()) {
            m_nLastWeaponDamageType = WEAPON_RAMMEDBYCAR;
            m_pLastDamageEntity = damager;
            damager->RegisterReference(&m_pLastDamageEntity);
        }

        // Store light states, so later we can determine if any was blown
        // and play the fx sound
        int32 prevLightStates[4];
        for (int32 i = 0; i < 4; i++) {
            prevLightStates[i] = (int32)m_damageManager.GetLightStatus((eLights)i);
        }

        if (GetStatus() == STATUS_PLAYER) {
            FindPlayerInfo().m_nVehicleTimeCounter = CTimer::GetTimeInMS();
        }

        // 0x6A7B63
        const auto moveSpeedMagSq = m_vecMoveSpeed.SquaredMagnitude();
        if (moveSpeedMagSq > std::bit_cast<float>(0x39D1B717u)) { // 0x858FC0 = 0.0004
            dmgDrawCarCollidingParticles(*vecCollisionCoors, collForceMult * calcDmgIntensity, weapon);
        }

        if (m_matrix->GetUp().z > 0.f /*Not flipped on roof*/ || moveSpeedMagSq > 0.3f) {
            const auto ApplyDamageToComponent = [this, &calcDmgIntensity](tComponent component) {
                m_damageManager.ApplyDamage(this, component, calcDmgIntensity * 4.0f, m_pHandlingData->m_fCollisionDamageMultiplier);
            };

            // Como el original (0x6A7DA8..0x6A803C): los casos compuestos acumulan una mascara y la aplican al final en el orden
            // de la cadena del EXE (la parte de mascara siempre se evalua ANTES de aplicar los componentes de la mascara)
            switch ((uint16)collisionComponent) {
            case (uint16)eVehicleCollisionComponent::BONNET:
                ApplyDamageToComponent(tComponent::COMPONENT_BONNET);
                break;
            case (uint16)eVehicleCollisionComponent::BOOT:
                ApplyDamageToComponent(tComponent::COMPONENT_BOOT);
                break;
            case (uint16)eVehicleCollisionComponent::BUMP_FRONT: {
                ApplyDamageToComponent(tComponent::COMPONENT_BUMP_FRONT);

                const auto colDirDotRight = DotProduct(*vecCollisionDirection, m_matrix->GetRight());
                // Como el original (0x6A7C3B..0x6A7C93): dot(coors - pos, right) / bbox.max.x (GetColModel() se pide despues del dot)
                const auto collDirDotRight = DotProduct(*vecCollisionCoors - GetPosition(), m_matrix->GetRight());
                const auto localDir = collDirDotRight / GetColModel()->m_boundBox.m_vecMax.x;

                uint32 mask = 0u;
                // Possibly apply right/left wing damage based on direction
                if (   localDir > 0.7f // [0, 45] deg
                    || localDir > 0.5f && colDirDotRight < -0.5f && 0.35f * m_fMass < calcDmgIntensity
                ) {
                    mask = 8u; // WING_RF
                } else if (
                       localDir < -0.7f // [90, 135] deg
                    || localDir < -0.5f && colDirDotRight > 0.5f && 0.35f * m_fMass < calcDmgIntensity
                ) {
                    mask = 4u; // WING_LF
                }

                // Possibly apply bonnet damage if front bumper is not `ok`
                // Como el original (0x6A7D42..0x6A7D7D): el azar (rand() & 3) y el arma solo se miran con el parachoques danado
                if (m_damageManager.GetPanelStatus(ePanels::FRONT_BUMPER) >= 1) {
                    if (0.3f * m_fMass < calcDmgIntensity) {
                        mask |= 1u;
                    } else if (weapon < WEAPON_LAST_WEAPON && ((CGeneral::GetRandomNumber() & 3) == 0)) {
                        mask |= 1u;
                    }
                }

                // Possibly apply windscreen damage if front bumper is `damaged`
                if (m_damageManager.GetPanelStatus(ePanels::FRONT_BUMPER) >= DAMSTATE_DAMAGED && 0.2f * m_fMass < calcDmgIntensity) {
                    mask |= 2u;
                }

                if (mask & 1u) ApplyDamageToComponent(tComponent::COMPONENT_BONNET);
                if (mask & 4u) ApplyDamageToComponent(tComponent::COMPONENT_WING_LF);
                if (mask & 8u) ApplyDamageToComponent(tComponent::COMPONENT_WING_RF);
                if (mask & 2u) ApplyDamageToComponent(tComponent::COMPONENT_WINDSCREEN);
                break;
            }
            case (uint16)eVehicleCollisionComponent::BUMP_REAR: {
                ApplyDamageToComponent(tComponent::COMPONENT_BUMP_REAR);

                const auto collDirDotRight = DotProduct(*vecCollisionCoors - GetPosition(), m_matrix->GetRight());
                const auto localDir = collDirDotRight / GetColModel()->m_boundBox.m_vecMax.x;

                uint32 mask = 0u;
                // If this is a van possibly apply damage to the rear doors
                if (vehicleFlags.bIsVan) {
                    if (localDir > 0.1f && 0.35f * m_fMass < calcDmgIntensity) { // [-90, -85] deg
                        mask = 0x40u; // DOOR_RR
                    } else if (localDir < -0.1f && 0.35f * m_fMass < calcDmgIntensity) { // [-95, -90]
                        mask = 0x20u; // DOOR_LR
                    }
                }

                // Possibly apply damage to boot if not already damaged
                if (m_damageManager.GetPanelStatus(ePanels::REAR_BUMPER) < DAMSTATE_DAMAGED) {
                    mask |= 0x10u;
                }

                if (mask & 0x10u) ApplyDamageToComponent(tComponent::COMPONENT_BOOT);
                if (mask & 0x20u) ApplyDamageToComponent(tComponent::COMPONENT_DOOR_LR);
                if (mask & 0x40u) ApplyDamageToComponent(tComponent::COMPONENT_DOOR_RR);
                break;
            }
            case (uint16)eVehicleCollisionComponent::DOOR_LF:
                ApplyDamageToComponent(tComponent::COMPONENT_DOOR_LF);
                break;
            case (uint16)eVehicleCollisionComponent::DOOR_RF:
                ApplyDamageToComponent(tComponent::COMPONENT_DOOR_RF);
                break;
            case (uint16)eVehicleCollisionComponent::DOOR_LR:
                ApplyDamageToComponent(tComponent::COMPONENT_DOOR_LR);
                break;
            case (uint16)eVehicleCollisionComponent::DOOR_RR:
                ApplyDamageToComponent(tComponent::COMPONENT_DOOR_RR);
                break;
            case (uint16)eVehicleCollisionComponent::WING_LF:
                ApplyDamageToComponent(tComponent::COMPONENT_WING_LF);
                break;
            case (uint16)eVehicleCollisionComponent::WING_RF:
                ApplyDamageToComponent(tComponent::COMPONENT_WING_RF);
                break;
            case (uint16)eVehicleCollisionComponent::WINDSCREEN:
                ApplyDamageToComponent(tComponent::COMPONENT_WINDSCREEN);
                break;
            default:
                break; // DEFAULT, WING_LR, WING_RR y el resto: la tabla del EXE salta al final
            }
        }

        // 0x6A8052
        auto calcCollHealthLoss = (calcDmgIntensity - minDmgIntensity) * m_pHandlingData->m_fCollisionDamageMultiplier * 0.6f * collForceMult;

        switch (m_nModelIndex) {
        case eModelID::MODEL_SECURICA: {
            if (m_pDamageEntity && m_pDamageEntity->GetStatus() == STATUS_PLAYER) {
                calcCollHealthLoss *= 7.f;
            }
            break;
        }
        case eModelID::MODEL_RCRAIDER:
        case eModelID::MODEL_RCGOBLIN:
        case eModelID::MODEL_RCTIGER: {
            calcCollHealthLoss *= 30.f;
            break;
        }
        }

        if (m_pDamageEntity && ModelIndices::IsRhino(m_pDamageEntity->m_nModelIndex)) {
            calcCollHealthLoss *= 15.f;
        }

        // 0x6A80E1
        if (calcCollHealthLoss > 0.f) {
            if (calcCollHealthLoss > 5.f) {
                if (m_pDamageEntity && m_pDamageEntity->GetIsTypeVehicle() && m_pDamageEntity->AsVehicle()->m_pDriver && m_pDriver) {
                    const auto& damagedVeh = *m_pDamageEntity->AsVehicle();

                    // Add collision event
                    {
                        const auto dot = DotProduct(
                            m_pDamageEntity->AsPhysical()->m_vecMoveSpeed - m_vecMoveSpeed,
                            m_vecLastCollisionImpactVelocity
                        );
                        if (dot < 0.f) { // Forces are opposite
                            CEventVehicleDamageCollision event{ this, m_pDamageEntity->AsVehicle(), WEAPON_RAMMEDBYCAR };
                            m_pDriver->GetEventGroup().Add(&event);
                            // Como el original (0x6A81A5..0x6A81AE): luego llama a ReactToVehicleDamage con el conductor del vehiculo evento
                            ReactToVehicleDamage(m_pDriver);
                        } else {
                            CEventVehicleDamageCollision event{ m_pDamageEntity->AsVehicle(), this, WEAPON_RAMMEDBYCAR };
                            m_pDamageEntity->AsVehicle()->m_pDriver->GetEventGroup().Add(&event);
                            ReactToVehicleDamage(damagedVeh.m_pDriver);
                        }
                    }
                }

                // Make our driver say something reacting to the collision..
                // Como el original (0x6A81CC..0x6A8254): la condicion compara FindPlayerVehicle() con ESTE vehiculo y la probabilidad
                // de la frase es 0.66 (0x3F28F5C3)
                if (m_pDriver && m_pDamageEntity && m_pDamageEntity->GetIsTypeVehicle()) {
                    const auto& damagedVeh = *m_pDamageEntity->AsVehicle();
                    if (!(FindPlayerVehicle() == this && damagedVeh.IsMissionVehicle()) && damagedVeh.m_pDriver) {
                        const auto sayProbability = std::bit_cast<float>(0x3F28F5C3u);
                        switch (m_vehicleAudio.GetVehicleTypeForAudio()) {
                        case eAEVehicleAudioType::CAR:      m_pDriver->Say(CTX_GLOBAL_CRASH_CAR, 0, sayProbability);     break;
                        case eAEVehicleAudioType::BIKE:     m_pDriver->Say(CTX_GLOBAL_CRASH_BIKE, 0, sayProbability);    break;
                        default:                            m_pDriver->Say(CTX_GLOBAL_CRASH_GENERIC, 0, sayProbability); break;
                        }
                    }
                }

                // Como el original (0x6A8259..0x6A8298): se pide FindPlayerVehicle() aunque no haya entidad que dano
                if (FindPlayerVehicle() == this && m_pDamageEntity) {
                    if (const auto p = PickRandomPassenger()) {
                        p->Say(m_pDamageEntity->GetIsTypePed() ? CTX_GLOBAL_CAR_HIT_PED : CTX_GLOBAL_CAR_CRASH);
                    }
                }
                g_InterestingEvents.Add(CInterestingEvents::EType::VEHICLE_DAMAGE, this);
            }

            // Decrease m_fHealth
            // Como el original (0x6A82AA..0x6A8325): la vida previa se guarda truncada (_ftol2) y las divisiones son
            // multiplicaciones por constantes del EXE (1/6, 1/2, 1/12, 1/1.5, 1/4)
            const auto prevHealth = (int32)m_fHealth;

            // 0x6A82CB
            if (this == FindPlayerVehicle()) {
                m_fHealth = m_fHealth - calcCollHealthLoss * (vehicleFlags.bTakeLessDamage ? std::bit_cast<float>(0x3E2AAAABu) : 0.5f);
            } else if (vehicleFlags.bTakeLessDamage) {
                m_fHealth = m_fHealth - calcCollHealthLoss * std::bit_cast<float>(0x3DAAAAABu);
            } else {
                if (m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle()) {
                    m_fHealth = m_fHealth - calcCollHealthLoss * std::bit_cast<float>(0x3F2AAAABu);
                }
                else {
                    m_fHealth = m_fHealth - calcCollHealthLoss * 0.25f;
                }
            }
            // 0x6A8338
            if (CCheat::IsActive(CHEAT_SMASH_N_BOOM) && m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle()) {
                BlowUpCar(m_pDamageEntity, false);
            }
            else if (m_fHealth <= 0.f && (int16)prevHealth > 0) { // 0x6A8354: test bx,bx / jle (16 bits con signo)
                m_fHealth = 1.f; // 0x6A8374
                if (m_pDamageEntity && ModelIndices::IsRhino(m_pDamageEntity->m_nModelIndex) && FindPlayerVehicle() != this) {
                    BlowUpCar(m_pDamageEntity, false);
                }
            }
        }

        // Como el original (0x6A83AB..0x6A83D0): suena si alguna luz que no estaba rota (estado 1) pasa a estar rota
        for (int32 i = 0; i < 4; i++) {
            if (prevLightStates[i] == 1)
                continue;
            if ((int32)m_damageManager.GetLightStatus((eLights)i) == 1) {
                m_vehicleAudio.AddAudioEvent(AE_LIGHT_SMASH, 0.f); // Yeah, play sound
                break;
            }
        }
    }

    // 0x6A83DF
    if (m_fHealth >= 250.f) {
        if (ModelIndices::IsBFInjection(m_nModelIndex)) {
            if (m_fHealth >= 400.f) {
                if (m_fHealth < 600.f) {
                    m_damageManager.SetEngineStatus(100u);
                }
            } else {
                m_damageManager.SetEngineStatus(200u);
            }
        }
    } else if (m_damageManager.GetEngineStatus() < 225u) {
        m_damageManager.SetEngineStatus(225u);

        m_fBurnTimer = 0.f;

        // Como el original (0x6A8416..0x6A8438): escribe m_pExplosionVictim (0x928), no m_pLastDamageEntity, y registra su referencia
        m_pExplosionVictim = reinterpret_cast<CPed*>(m_pDamageEntity);
        if (m_pDamageEntity) {
            m_pDamageEntity->RegisterReference(reinterpret_cast<CEntity**>(&m_pExplosionVictim));
        }

        if (const auto passenger = PickRandomPassenger()) {
            passenger->Say(CTX_GLOBAL_CAR_FIRE, 1500);
        }
    }
}

//0x6AF1D0
bool CAutomobile::GetTowHitchPos(CVector& outPos, bool bCheckModelInfo, CVehicle* attachTo) {
    if (!bCheckModelInfo) {
        return false;
    }
    outPos.x = 0.0f;
    outPos.y = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->m_boundBox.m_vecMax.y - 0.5f;
    outPos.z = 0.5f - m_fFrontHeightAboveRoad;
    // Como el original (0x6AF219): MultiplyMatrixWithVector (0x59C890) con su orden de sumas
    outPos = G1MultiplyMatrixWithVector(*m_matrix, outPos);
    return true;
}

// 0x6AF250
bool CAutomobile::GetTowBarPos(CVector& outPos, bool ignoreModelType, CVehicle* attachTo) {
    switch (m_nModelIndex) {
    case eModelID::MODEL_TOWTRUCK:
    case eModelID::MODEL_TRACTOR: {
        float baseY = -1.05f;
        if (m_nModelIndex == MODEL_TRACTOR) {
            if (attachTo && attachTo->IsSubTrailer() && attachTo->m_nModelIndex != MODEL_FARMTR1) {
                return false;
            }
            baseY = -0.6f;
        } else if (attachTo && attachTo->IsSubTrailer()) {
            return false;
        }

        outPos.x = 0.0f;
        outPos.y = baseY + CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->m_boundBox.m_vecMin.y;
        outPos.z = ((1.0f - (float)m_wMiscComponentAngle / (float)TOWTRUCK_HOIST_DOWN_LIMIT) / 2.0f + 0.5f) - m_fFrontHeightAboveRoad;
        // Como el original (0x6AF3D1): MultiplyMatrixWithVector (0x59C890) con su orden de sumas
        outPos = G1MultiplyMatrixWithVector(*m_matrix, outPos);
        return true;
    }
    default: { // TODO: Move out to after switch (deindentate)
        if (!m_aCarNodes[CAR_MISC_A]) {
            break;
        }

        switch (m_nModelIndex) {
        case eModelID::MODEL_PETRO:
        case eModelID::MODEL_RDTRAIN:
        case eModelID::MODEL_LINERUN:
        case eModelID::MODEL_ARTICT3: {
            outPos = *RwMatrixGetPos(RwFrameGetLTM(m_aCarNodes[CAR_MISC_A]));
            return true;
        }
        case eModelID::MODEL_UTILITY: {
            if (attachTo && attachTo->m_nModelIndex == MODEL_UTILTR1) {
                outPos = *RwMatrixGetPos(RwFrameGetLTM(m_aCarNodes[CAR_MISC_A]));
                return true;
            }
            break;
        }
        case eModelID::MODEL_BAGGAGE:
        case eModelID::MODEL_TUG:
        case eModelID::MODEL_BAGBOXA:
        case eModelID::MODEL_BAGBOXB: {
            if (attachTo) {
                switch (attachTo->m_nModelIndex) {
                case eModelID::MODEL_BAGBOXA:
                case eModelID::MODEL_BAGBOXB:
                case eModelID::MODEL_TUGSTAIR:
                    outPos = *RwMatrixGetPos(RwFrameGetLTM(m_aCarNodes[CAR_MISC_A]));
                    return true;
                }
            }
            break;
        }
        }
        break;
    }
    }

    // Como el original (0x6AF300..0x6AF3EF): NO llama a GetTowHitchPos; con ignoreModelType a 0 devuelve false y, si no, usa
    // y = -0.5 + min.y de la caja de colision ([87109C] + [colmodel+4]) y z = 0.5 - altura delantera
    if (!ignoreModelType) {
        return false;
    }
    outPos.x = 0.0f;
    outPos.y = -0.5f + CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->m_boundBox.m_vecMin.y;
    outPos.z = 0.5f - m_fFrontHeightAboveRoad;
    outPos = G1MultiplyMatrixWithVector(*m_matrix, outPos);
    return true;
}

// 0x6B4410
bool CAutomobile::SetTowLink(CVehicle* tractor, bool placeMeOnRoadProperly) {
    if (!tractor) {
        return false;
    }

    if (m_pTowingVehicle) {
        return false;
    }

    // Como el original (0x6B4430..0x6B445E): PHYSICS (3), IS_TOWED (10) y ABANDONED (4) siguen sin mas; SIMPLE (2) pasa antes a
    // fisica real (SwitchVehicleToRealPhysics); cualquier otro estado devuelve false
    switch (GetStatus()) {
    case eEntityStatus::STATUS_PHYSICS:
    case eEntityStatus::STATUS_IS_TOWED:
    case eEntityStatus::STATUS_ABANDONED:
        break;
    case eEntityStatus::STATUS_SIMPLE: {
        CCarCtrl::SwitchVehicleToRealPhysics(this);
        break;
    }
    default:
        return false;
    }

    SetStatus(STATUS_IS_TOWED);

    m_pTowingVehicle = tractor;
    tractor->RegisterReference(m_pTowingVehicle);

    m_pTowingVehicle->m_pVehicleBeingTowed = this;
    RegisterReference(m_pTowingVehicle->m_pVehicleBeingTowed);

    // Como el original (0x6B4482..0x6B4499): primero se quitan de la lista de movimiento los dos y despues se anaden los dos
    AsVehicle()->RemoveFromMovingList();
    m_pTowingVehicle->RemoveFromMovingList();
    AsVehicle()->AddToMovingList();
    m_pTowingVehicle->AddToMovingList();

    if (placeMeOnRoadProperly) {
        switch (tractor->m_nModelIndex) {
        case eModelID::MODEL_TOWTRUCK:
        case eModelID::MODEL_TRACTOR: {
            tractor->AsAutomobile()->m_wMiscComponentAngle = TOWTRUCK_HOIST_UP_LIMIT;
            break;
        }
        }

        SetHeading(tractor->GetHeading());

        CVector towHitchPos{}, towBarPos{};
        if (GetTowHitchPos(towHitchPos, true, this) && tractor->GetTowBarPos(towBarPos, true, this)) {
            SetPosn(towBarPos - (towHitchPos - GetPosition()));
            PlaceOnRoadProperly();
            return true;
        }
    } else {
        UpdateTrailerLink(true, false);
        return true;
    }
    return false;
}

// 0x6A4400
bool CAutomobile::BreakTowLink() {
    if (m_pTowingVehicle) {
        CEntity::ClearReference(m_pTowingVehicle->m_pVehicleBeingTowed);
        CEntity::ClearReference(m_pTowingVehicle);
    }

    switch (GetStatus()) {
    case STATUS_IS_TOWED:
    case STATUS_IS_SIMPLE_TOWED: {
        if (m_pDriver) { // TODO: Use ternary
            if (m_pDriver->IsPlayer()) {
                SetStatus(STATUS_PLAYER);
            } else {
                SetStatus(STATUS_PHYSICS);
            }
        } else {
            // Como el original (0x6A4485..0x6A449B): fcomp con 1.0 + test ah,5 / jp: solo "menor que 1" da WRECKED (NaN da ABANDONED)
            if (!(m_fHealth < 1.f)) {
                SetStatus(STATUS_ABANDONED);
            } else {
                SetStatus(STATUS_WRECKED);
            }
        }
        return true;
    }
    }
    return false;
}

// 0x6A6090
float CAutomobile::FindWheelWidth(bool bRear) {
    // TODO: Maybe replace this with `bool flagValaue` and use bitfields directly (instead of bitmasks?)
    static constexpr struct { eVehicleHandlingFlags flag; float mult; } mapping[2][4]{
        { // Rear wheel
            { VEHICLE_HANDLING_WHEEL_R_NARROW2, 0.65f },
            { VEHICLE_HANDLING_WHEEL_R_NARROW,  0.8f  },
            { VEHICLE_HANDLING_WHEEL_R_WIDE,    1.1f  },
            { VEHICLE_HANDLING_WHEEL_R_WIDE2,   1.25f }
        },
        { // Front wheel
            { VEHICLE_HANDLING_WHEEL_F_NARROW2, 0.65f },
            { VEHICLE_HANDLING_WHEEL_F_NARROW,  0.8f  },
            { VEHICLE_HANDLING_WHEEL_F_WIDE,    1.1f  },
            { VEHICLE_HANDLING_WHEEL_F_WIDE2,   1.25f }
        }
    };

    const auto& mi = *GetVehicleModelInfo();

    // Como el original (0x6A60A7..0x6A60B1): tamano trasero ([mi+44h]) o delantero ([mi+40h]) multiplicado por 0x3EB6DB6E (1/2.8 como float), no dividido
    auto wheelWidth = (bRear ? mi.m_fWheelSizeRear : mi.m_fWheelSizeFront) * std::bit_cast<float>(0x3EB6DB6Eu);

    // Como el original (0x6A60BD..0x6A6128): rueda trasera con los bits 0xF000, delantera con 0x0F00; se aplica el primer indicador
    // encontrado y el coche kart (0x23B) multiplica por 1.5 AL FINAL (0x6A6128..0x6A6130), despues del factor del indicador
    for (auto&& [flag, mult] : mapping[bRear ? 0 : 1]) {
        if (m_nHandlingFlagsIntValue & flag) {
            wheelWidth *= mult;
            break;
        }
    }

    if (m_nModelIndex == eModelID::MODEL_KART) {
        wheelWidth *= 1.5f;
    }

    return wheelWidth;
}

// 0x5D47E0
bool CAutomobile::Save() {
    CVehicle::Save();
    CGenericGameStorage::SaveDataToWorkBuffer(&m_damageManager, sizeof(m_damageManager));
    return true;
}

// 0x5D2980
bool CAutomobile::Load() {
    CVehicle::Load();
    CGenericGameStorage::LoadDataFromWorkBuffer(&m_damageManager, sizeof(m_damageManager));
    return true;
}

// 0x6A0770
void CAutomobile::SetupModelNodes() {
    std::ranges::fill(m_aCarNodes, nullptr);
    CClumpModelInfo::FillFrameArray(GetRpClump(), m_aCarNodes.data());
}

// Como el original: 0x6A0E9D..0x6A0EBF. angulo = fpatan(lr, ud) + (-pi/4 = [0x8D31BC]) (la suma se redondea a float);
// cosOut = fcos(angulo) * jump y sinOut = fsin(angulo) * jump con fpatan/fcos/fsin en precision extendida y un unico redondeo en cada producto
static void G6HydraulicStickAngleChange(float stickLR, float stickUD, float jump, float& cosOut, float& sinOut) {
    AutomobileHydraulicStickCpp(stickLR, stickUD, jump, std::bit_cast<float>(0xBF490FDBu), &cosOut, &sinOut);
}

// 0x6A07A0
void CAutomobile::HydraulicControl() {
    if (GetStatus() == STATUS_PHYSICS) { // TODO: switch()
        if (!IsCreatedBy(MISSION_VEHICLE))
            return;

        if (m_vehicleSpecialColIndex < 0)
            return;
    } else if (GetStatus() != STATUS_PLAYER) {
        return;
    }

    if (handlingFlags.bHydraulicNone) {
        return;
    }

    CPlayerPed* driver = nullptr;
    if (GetStatus() == STATUS_PLAYER) {
        if (m_pDriver && m_pDriver->IsPlayer())
            driver = m_pDriver->AsPlayer();
        // Como el original: 0x6A0820 test byte [0x96A8B0]; jne -> retorno (GameState es eGameLogicState, 0 = GAMELOGIC_STATE_PLAYING)
        if (CGameLogic::GameState != GAMELOGIC_STATE_PLAYING)
            return;
    }
    if (!CVehicle::GetSpecialColModel()) {
        return;
    }

    auto modelInfo = GetVehicleModelInfo();
    float wheelRadius = modelInfo->m_fWheelSizeFront / 2.0f;
    CCollisionData* colData = CVehicle::m_aSpecialColModel[m_vehicleSpecialColIndex].m_pColData;
    auto& hydraulicData = CVehicle::m_aSpecialHydraulicData[m_vehicleSpecialColIndex];
    float normalUpperLimit   = hydraulicData.m_fSuspensionNormalUpperLimit;
    float normalLowerLimit   = hydraulicData.m_fSuspensionNormalLowerLimit;
    float extendedUpperLimit = hydraulicData.m_fSuspensionExtendedUpperLimit;
    float extendedLowerLimit = hydraulicData.m_fSuspensionExtendedLowerLimit;
    float normalSpringLength = normalUpperLimit - normalLowerLimit;
    float extendedSpringLength = extendedUpperLimit - extendedLowerLimit;
    if (hydraulicData.m_fSuspensionExtendedUpperLimit >= 100.0f) {
        float minz = 0.0f;
        // Como el original: 0x6A086D movsx eax,word ptr [esi]; jle (contador con signo de 16 bits)
        for (int32 i = 0; i < (int32)(int16)colData->m_nNumSpheres; i++) {
            float z = colData->m_pSpheres[i].m_vecCenter.z - colData->m_pSpheres[i].m_fRadius;
            if (z < minz)
                minz = z;
        }

        CVector pos;
        modelInfo->GetWheelPosn(CAR_WHEEL_FRONT_LEFT, pos, false);
        float limitExtend = minz - pos.z;
        normalLowerLimit = m_pHandlingData->m_fSuspensionLowerLimit;
        normalUpperLimit = m_pHandlingData->m_fSuspensionUpperLimit;
        normalSpringLength = normalUpperLimit - normalLowerLimit;
        if (handlingFlags.bHydraulicGeom)
            normalSpringLength *= 1.5f;
        normalUpperLimit = normalSpringLength + normalLowerLimit;
        hydraulicData.m_fSuspensionNormalUpperLimit = normalUpperLimit;
        hydraulicData.m_fSuspensionNormalLowerLimit = normalLowerLimit;
        if (handlingFlags.bHydraulicGeom) {
            extendedUpperLimit = normalUpperLimit - 0.4f;
            extendedLowerLimit = normalLowerLimit - 0.4f;
        } else {
            extendedUpperLimit = normalUpperLimit - 0.2f;
            extendedLowerLimit = normalLowerLimit - 0.2f;
        }
        // Como el original: 0x6A096D fld [0x870FEC] (0.0f); fadd limitExtend; solo se sustituye si extendedUpperLimit < (limitExtend + 0.0f)
        extendedUpperLimit = std::max(extendedUpperLimit, limitExtend + 0.0f);
        extendedSpringLength = extendedUpperLimit - extendedLowerLimit;
        hydraulicData.m_fSuspensionExtendedUpperLimit = extendedUpperLimit;
        hydraulicData.m_fSuspensionExtendedLowerLimit = extendedLowerLimit;

        // or 0.15f * normalSpringLength
        // Como el original: 0x6A09B4..0x6A09CE fld 1.0; fsub [0x870FE4]; fmul normalSpringLength; fmul 0.5  -> ((1 - 0.7) * longitud) * 0.5
        float normalSpringLength_15_Percent = ((1.0f - 0.7f) * normalSpringLength) * 0.5f;
        hydraulicData.m_fSuspensionNormalIdleUpperLimit = normalUpperLimit - normalSpringLength_15_Percent;
        hydraulicData.m_fSuspensionNormalIdleLowerLimit = normalLowerLimit + normalSpringLength_15_Percent;
        float wheelHeight = limitExtend + wheelRadius - 0.1f;
        if (wheelHeight < hydraulicData.m_fSuspensionNormalIdleLowerLimit) {
            float height = hydraulicData.m_fSuspensionNormalIdleLowerLimit - wheelHeight;
            hydraulicData.m_fSuspensionNormalIdleUpperLimit -= height;
            hydraulicData.m_fSuspensionNormalIdleLowerLimit -= height;
        }
    }

    bool setPrevRatio = false;
    // Como el original: 0x6A0A66..0x6A0A71 fcomp [0x870FDC]; test ah,41h; jne -> salta con NaN, por eso !(x > 0.02f) y no x <= 0.02f
    if (m_wMiscComponentAngle >= 20 || !(m_PrevSpeed > 0.02f) || m_GasPedal == 0.0f) {
        if (m_wMiscComponentAngle && m_wMiscComponentAngle < 61 && m_PrevSpeed < 0.01f) {
            m_wMiscComponentAngle--;
            // Como el original: 0x6A0B1E lea eax,[ecx-1]; test ax,ax; jne -> el evento suena cuando el nuevo valor es 0 (antes: == 1)
            if (m_wMiscComponentAngle == 0)
                m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_OFF, 0.0f);
        }
    } else if (m_wMiscComponentAngle) {
        m_wMiscComponentAngle++;
    } else {
        m_wMiscComponentAngle = 20;
        m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_ON, 0.0f);
        setPrevRatio = true;
    }

    bool suspensionTriggered = false;
    bool normalHornHandled = false; // Como el original: la rama normal de la bocina (0x6A0CAE..0x6A0DBA) salta a 0x6A13B8 y no ejecuta el resto
    float wheelPositions[4];
    float suspensionChange[4];

    CVector pos;
    if (driver && driver->GetPadFromPlayer()->HornJustDown()) {
        float lineLength = extendedSpringLength + wheelRadius;
        m_wMiscComponentAngle = m_wMiscComponentAngle < 500 ? 500 : 60;
        if (m_wMiscComponentAngle >= 500) {
            for (int32 i = 0; i < 4; i++) {
                CColLine& line = colData->m_pLines[i];
                wheelPositions[i] = line.m_vecStart.z - m_aSuspensionLineLength[i] * m_fWheelsSuspensionCompression[i];
                modelInfo->GetWheelPosn(i, pos, false);
                pos.z += extendedUpperLimit;
                line.m_vecStart = pos;
                pos.z -= lineLength;
                line.m_vecEnd = pos;
                m_aSuspensionSpringLength[i] = extendedSpringLength;
                m_aSuspensionLineLength[i] = lineLength;
                if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                    float compressed = (line.m_vecStart.z - wheelPositions[i]) / lineLength;
                    m_fWheelsSuspensionCompression[i] = std::min(compressed, 1.0f);
                }
            }
            m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_TRIGGER, 0.0f);
            suspensionTriggered = true;
        } else {
            lineLength = normalSpringLength + wheelRadius;
            for (int32 i = 0; i < 4; i++) {
                CColLine& line = colData->m_pLines[i];
                wheelPositions[i] = line.m_vecStart.z - m_fWheelsSuspensionCompression[i] * m_aSuspensionLineLength[i];
                modelInfo->GetWheelPosn(i, pos, false);
                pos.z += normalUpperLimit;
                line.m_vecStart = pos;
                pos.z -= lineLength;
                line.m_vecEnd = pos;
                m_aSuspensionSpringLength[i] = normalSpringLength;
                m_aSuspensionLineLength[i] = lineLength;
                if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                    float compressed = (line.m_vecStart.z - wheelPositions[i]) / lineLength;
                    m_fWheelsSuspensionCompression[i] = std::min(compressed, 1.0f);
                }
            }
            m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_HYDRAULIC, 0.0f);
            normalHornHandled = true;
        }
    }

    if (!suspensionTriggered && !normalHornHandled) {
        std::ranges::fill(suspensionChange, 1.0f);

        if (GetStatus() == STATUS_PHYSICS) {
            for (int32 i = 0; i < 4; i++) {
                suspensionChange[i] = hydraulicData.m_aWheelSuspension[i];
            }
        } else if (driver) {
            if (!driver->GetPadFromPlayer()->GetHydraulicJump()) { // 0x6A0E3D
                float gunLeftRight = driver->GetPadFromPlayer()->GetCarGunLeftRight();
                float gunUpDown = driver->GetPadFromPlayer()->GetCarGunUpDown();
                float jumpFactor = (std::sqrt(gunUpDown * gunUpDown + gunLeftRight * gunLeftRight) * 1.5f) / 128.0f;
                // Como el original: 0x6A0E9D..0x6A0F08. fpatan/fcos/fsin en precision extendida; el angulo se redondea al sumar [0x8D31BC] (-pi/4);
                // los productos con jumpFactor se redondean una sola vez; 0x420800 es max(a, b) = (a <= b) ? b : a (distingue -0.0)
                float change_rear_right, change_front_right;
                G6HydraulicStickAngleChange(gunLeftRight, gunUpDown, jumpFactor, change_rear_right, change_front_right);
                // Como el original: 0x420800 devuelve b si !(a > b) (con NaN devuelve b)
                const auto ExeMax = [](float a, float b) { return (a > b) ? a : b; };
                suspensionChange[CAR_WHEEL_FRONT_LEFT]  = ExeMax(0.0f, -change_rear_right);
                suspensionChange[CAR_WHEEL_REAR_LEFT]   = ExeMax(0.0f, -change_front_right);
                suspensionChange[CAR_WHEEL_FRONT_RIGHT] = ExeMax(0.0f, +change_front_right);
                suspensionChange[CAR_WHEEL_REAR_RIGHT]  = ExeMax(0.0f, +change_rear_right);
            }
        }

        float maxDelta = 0.0f;
        if (m_wMiscComponentAngle < 500u) {
            if (suspensionChange[CAR_WHEEL_FRONT_LEFT] == 0.0f &&
                suspensionChange[CAR_WHEEL_REAR_LEFT] == 0.0f &&
                suspensionChange[CAR_WHEEL_FRONT_RIGHT] == 0.0f &&
                suspensionChange[CAR_WHEEL_REAR_RIGHT] == 0.0f
            ) {
                if (m_wMiscComponentAngle == 0) {
                    normalUpperLimit = hydraulicData.m_fSuspensionNormalIdleUpperLimit;
                    normalLowerLimit = hydraulicData.m_fSuspensionNormalIdleLowerLimit;
                    normalSpringLength = normalUpperLimit - normalLowerLimit;
                }
            } else {
                if (m_wMiscComponentAngle == 0) {
                    std::ranges::fill(suspensionChange, 0.0f);
                }
                m_wMiscComponentAngle = 60;
            }

            for (int32 i = 0; i < 4; i++) {
                CColLine& line = colData->m_pLines[i];
                suspensionChange[i] = std::min(suspensionChange[i], 1.0f);
                float springLength = (extendedSpringLength - normalSpringLength) * suspensionChange[i] + normalSpringLength;
                float lineLength = springLength + wheelRadius;
                wheelPositions[i] = line.m_vecStart.z - m_aSuspensionLineLength[i] * m_fWheelsSuspensionCompression[i];
                modelInfo->GetWheelPosn(i, pos, false);
                pos.z += (extendedUpperLimit - normalUpperLimit) * suspensionChange[i] + normalUpperLimit;
                line.m_vecStart = pos;
                pos.z -= lineLength;
                // Como el original: 0x6A1005..0x6A101C fcompp |maxDelta| con |delta|; solo si |maxDelta| < |delta| se guarda el delta CON signo
                if (const float deltaZ = pos.z - line.m_vecEnd.z; std::fabs(maxDelta) < std::fabs(deltaZ))
                    maxDelta = deltaZ;
                line.m_vecEnd = pos;
                m_aSuspensionLineLength[i] = lineLength;
                m_aSuspensionSpringLength[i] = springLength;
                if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                    float compressed = (line.m_vecStart.z - wheelPositions[i]) / lineLength;
                    m_fWheelsSuspensionCompression[i] = std::min(compressed, 1.0f);
                }
            }
        } else {
            if (m_wMiscComponentAngle < 504u) {
                m_wMiscComponentAngle++;
            }

            for (int32 i = 0; i < 4; i++) {
                CColLine& line = colData->m_pLines[i];
                suspensionChange[i] = std::min(suspensionChange[i], 1.0f);
                float springLength = (normalSpringLength - extendedSpringLength) * suspensionChange[i] + extendedSpringLength;
                wheelPositions[i] = line.m_vecStart.z - m_aSuspensionLineLength[i] * m_fWheelsSuspensionCompression[i];
                modelInfo->GetWheelPosn(i, pos, false);
                pos.z += (normalUpperLimit - extendedUpperLimit) * suspensionChange[i] + extendedUpperLimit;
                float lineLength = springLength + wheelRadius;
                line.m_vecStart = pos;
                pos.z -= lineLength;
                // Como el original: 0x6A1225..0x6A123C (igual que la rama anterior)
                if (const float deltaZ = pos.z - line.m_vecEnd.z; std::fabs(maxDelta) < std::fabs(deltaZ))
                    maxDelta = deltaZ;
                line.m_vecEnd = pos;
                m_aSuspensionLineLength[i] = lineLength;
                m_aSuspensionSpringLength[i] = springLength;
                if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                    float compressed = (line.m_vecStart.z - wheelPositions[i]) / lineLength;
                    m_fWheelsSuspensionCompression[i] = std::min(compressed, 1.0f);
                }
            }
        }
        float limitDiff = extendedLowerLimit - normalLowerLimit;
        // Como el original: 0x6A12E5 fcomp qword [0x85A308] (double 0.01, no el float 0.01f); test ah,41h; jne
        if (limitDiff != 0.0f && (double)std::fabs(maxDelta / limitDiff) > 0.01) {
            float f = (limitDiff + maxDelta) * 0.5f / limitDiff;
            f = std::clamp(f, 0.0f, 1.0f);
            if (f < 0.4f || f > 0.6f)
                setPrevRatio = true;
            if (f < 0.5f - CTimer::GetTimeStep() * 0.05f)
                m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_HYDRAULIC, 0.0f);
            else if (f > CTimer::GetTimeStep() * 0.05f + 0.5f)
                m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_TRIGGER, 0.0f);
            else if (f > CTimer::GetTimeStep() * 0.025f + 0.5f)
                m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_HYDRAULIC, 0.0f);
        }
    }

    if (setPrevRatio || suspensionTriggered) {
        for (int32 i = 0; i < MAX_CARWHEELS; i++) {
            const float t = 1.f - m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i];
            // same as `(m_fWheelsSuspensionCompression[i] - 1.f + k) / k` where `k = m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i]`
            m_fWheelsSuspensionCompressionPrev[i] = (m_fWheelsSuspensionCompression[i] - t) / (1.0f - t); 
        }
    }
    std::ranges::fill(hydraulicData.m_aWheelSuspension, 0.0f);
}

// Como el original: 0x59C890 (matriz * punto). Orden de suma del EXE: x = ((up.x*z + fwd.x*y) + right.x*x) + pos.x;
// y = ((up.y*z + right.y*x) + fwd.y*y) + pos.y; z = ((up.z*z + right.z*x) + fwd.z*y) + pos.z (CMatrix::TransformPoint suma en otro orden)
static CVector G6TransformPointExe(const CMatrix& m, const CVector& v) {
    const CVector r = G2Multiply3x3(m, v); // mismo orden de sumas que 0x59C790 / 0x59C890
    return CVector{ r.x + m.GetPosition().x, r.y + m.GetPosition().y, r.z + m.GetPosition().z };
}

static constexpr uint16 DEFAULT_COLLISION_EXTENDLIMIT = 2500; // 0x8D314C

// 0x6A1460
bool CAutomobile::UpdateMovingCollision(float angle) {
    if (ModelIndices::HasMiscComponent(m_nModelIndex)) {
        m_wMiscComponentAnglePrev = m_wMiscComponentAngle;
    }

    CPad* pad = nullptr;
    if (GetStatus() == STATUS_PLAYER) {
        // Como el original: 0x6A1523 comprueba m_pDriver != nullptr antes de IsPlayer (el codigo previo hacia AsPlayer() sobre un puntero nulo)
        if (!m_pDriver || !m_pDriver->IsPlayer() || CGameLogic::GameState)
            return false;

        pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    } else {
        if (   !IsCreatedBy(MISSION_VEHICLE)
            || angle < 0.0f
            || !ModelIndices::HasMiscComponent(m_nModelIndex) && !ModelIndices::IsFireTruckLadder(m_nModelIndex)
        ) {
            return false;
        }
    }

    if (!ModelIndices::IsCementTruck(m_nModelIndex) && !ModelIndices::IsFireTruckLadder(m_nModelIndex)) { // Inverted(!)
        CColModel* cm = GetVehicleModelInfo()->GetColModel();
        CCollisionData* cd = cm->m_pColData;
        if (!GetSpecialColModel()) {
            return false;
        }

        CColModel& specialColModel = m_aSpecialColModel[m_vehicleSpecialColIndex];
        CCollisionData* specialColData = specialColModel.m_pColData;

        m_wMiscComponentAnglePrev = m_wMiscComponentAngle;

        // Como el original: 0x6A15BE..0x6A15C5 fcomp 0.0; test ah,1; je -> la rama "negativa" tambien se toma con NaN, por eso !(angle >= 0)
        if (!(angle >= 0.0f)) {
            if (!pad || std::abs(pad->GetCarGunUpDown()) <= 10) {
                return false;
            }
        }

        if (angle >= 0.0f) {
            m_wMiscComponentAngle = static_cast<uint16>(angle * DEFAULT_COLLISION_EXTENDLIMIT);
        } else {
            // Como el original: 0x6A1648..0x6A1724. Multiplicador [0x871008] (10.0f); montacargas: [0x87101C] (-20.0f) si el stick >= 0, si no [0x871018] (-10.0f).
            float colAngleMult = 10.0f;
            if (ModelIndices::IsForklift(m_nModelIndex) ) {
                if ((float)pad->GetCarGunUpDown() >= 0.0f)
                    colAngleMult = -20.0f;
                else
                    colAngleMult = -10.0f;
            }

            // fild stick; fmul timestep; fmul mult; fmul 1/128; _ftol2; movsx -> se suma como entero al angulo actual; un resultado negativo se deja en 0
            // y luego se compara (sin signo, 16 bits) con DEFAULT_COLLISION_EXTENDLIMIT (antes: suma modular de uint16 y clamp)
            const int32 deltaAngle = (int32)(int16)(int32)((float)pad->GetCarGunUpDown() * CTimer::GetTimeStep() * colAngleMult * 0.0078125f);
            int32 newAngle = deltaAngle + (int32)m_wMiscComponentAngle;
            if (newAngle < 0)
                newAngle = 0;
            m_wMiscComponentAngle = (uint16)newAngle;
            if ((int32)(uint16)newAngle > (int32)DEFAULT_COLLISION_EXTENDLIMIT)
                m_wMiscComponentAngle = DEFAULT_COLLISION_EXTENDLIMIT;
        }

        CMatrix rotMatrix;

        // 0x6A174A
        RwFrame* carNodeMisc = nullptr;
        if (ModelIndices::IsDozer(m_nModelIndex)) {
            carNodeMisc = m_aCarNodes[CAR_MISC_A];
            if (carNodeMisc)
                rotMatrix.SetRotateX((float)m_wMiscComponentAngle * CAutomobile::DOZER_COL_ANGLE_MULT);
        }
        else if (ModelIndices::IsDumper(m_nModelIndex) && IsSubMonsterTruck()) {
            carNodeMisc = m_aCarNodes[CAR_MISC_C];
            if (carNodeMisc)
                rotMatrix.SetRotateX((float)m_wMiscComponentAngle * CMonsterTruck::DUMPER_COL_ANGLEMULT);
        }
        else if (ModelIndices::IsAndromada(m_nModelIndex)) {
            carNodeMisc = m_aCarNodes[CAR_MISC_E];
            if (carNodeMisc)
                rotMatrix.SetRotateX((float)m_wMiscComponentAngle * CPlane::ANDROM_COL_ANGLE_MULT);
        }
        else if (ModelIndices::IsForklift(m_nModelIndex)) {
            carNodeMisc = m_aCarNodes[CAR_MISC_A];
            if (carNodeMisc)
                rotMatrix.SetTranslate(CVector(0.0f, 0.0f, (float)m_wMiscComponentAngle * CAutomobile::FORKLIFT_COL_ANGLE_MULT));
        }
        else if (ModelIndices::IsPacker(m_nModelIndex)) {
            carNodeMisc = m_aCarNodes[CAR_MISC_A];
            if (carNodeMisc)
                rotMatrix.SetRotateX((float)m_wMiscComponentAngle * CAutomobile::PACKER_COL_ANGLE_MULT);
        }

        // Como el original: 0x6A18A2..0x6A18CE la posicion del componente del montacargas es (0, 0, 0)
        CVector componentPos{ 0.f, 0.f, 0.f };
        if (carNodeMisc && !ModelIndices::IsForklift(m_nModelIndex))
            componentPos = RwFrameGetMatrix(carNodeMisc)->pos;

        float minZ = 1000.0f;
        float maxZ = -1000.0f;
        // Como el original: los contadores se leen con movsx word (0x6A18D4 cmp word [esi+4]; jle ... 0x6A1BC0 movsx edx,word ptr [esi+4]) -> con signo
        for (int32 triIndx = 0; triIndx < (int32)(int16)specialColData->m_nNumTriangles; triIndx++) {
            CColTriangle& specialColTriangle = specialColData->m_pTriangles[triIndx];
            if (specialColTriangle.m_nMaterial == SURFACE_CAR_MOVINGCOMPONENT) {
                const CColTriangle& colTriangle = cd->m_pTriangles[triIndx];
                for (int32 i = 0; i < 3; i++) {
                    CVector vertexPos = cd->m_pVertices[colTriangle.m_vertIndices[i]];
                    CVector distance  = vertexPos - componentPos;
                    vertexPos = G6TransformPointExe(rotMatrix, distance) + componentPos;
                    specialColData->m_pVertices[specialColTriangle.m_vertIndices[i]] = vertexPos;
                    if (maxZ < vertexPos.z)
                        maxZ = vertexPos.z;
                    else if (minZ > vertexPos.z)
                        minZ = vertexPos.z;
                }
            }
        }

        if (specialColData->m_pTrianglePlanes) {
            for (int32 i = 0; i < (int32)(int16)specialColData->m_nNumTriangles; i++) {
                CColTrianglePlane& trianglePlane = specialColData->m_pTrianglePlanes[i];
                trianglePlane.Set(specialColData->m_pVertices, specialColData->m_pTriangles[i]);
            }
        }

        for (int32 i = 0; i < (int32)(int16)specialColData->m_nNumSpheres; i++) {
            CColSphere& specialColSphere = specialColData->m_pSpheres[i];
            if (specialColSphere.m_Surface.m_nMaterial == SURFACE_CAR_MOVINGCOMPONENT) {
                CColSphere& colSphere = cd->m_pSpheres[i];
                CVector distance = colSphere.m_vecCenter - componentPos;
                specialColSphere.m_vecCenter = G6TransformPointExe(rotMatrix, distance) + componentPos;
                const float newMaxZ = specialColSphere.m_fRadius + specialColSphere.m_vecCenter.z;
                const float newMinZ = specialColSphere.m_vecCenter.z - specialColSphere.m_fRadius;
                if (maxZ < newMaxZ)
                    maxZ = newMaxZ;
                else if (minZ > newMinZ)
                    minZ = newMinZ;
            }
        }
        if (cm->GetBoundingBox().m_vecMax.z < maxZ)
            specialColModel.GetBoundingBox().m_vecMax.z = maxZ;
        if (cm->GetBoundingBox().m_vecMin.z > minZ)
            specialColModel.GetBoundingBox().m_vecMin.z = minZ;
        return true;
    }

    m_wMiscComponentAnglePrev = m_wMiscComponentAngle;
    if (angle >= 0.0f) { // Como el original: 0x6A1D83 fcomp 0.0; test ah,1; jne -> la rama negativa tambien se toma con NaN
        m_wMiscComponentAngle = static_cast<uint16>((int32)(angle * DEFAULT_COLLISION_EXTENDLIMIT)); // 0x6A1DA6 (_ftol2, palabra baja)
        return false;
    }

    if (!pad)
        return false;

    // Como el original: 0x6A1DBC..0x6A1E9F. Los factores se multiplican en este orden (stick * paso * 10.0f * 1/128) y la resta se hace en enteros;
    // el resultado pasa por min/max en float (0x404330 / 0x420800) y _ftol2 (palabra baja)
    const float timeStep = CTimer::GetTimeStep();
    if ((float)pad->GetCarGunUpDown() < -10.0f) { // [0x859004]
        const int32 delta = (int32)(int16)(int32)((float)(pad->GetCarGunUpDown() * 2) * timeStep * 10.0f * 0.0078125f);
        const int32 newAngle = (int32)m_wMiscComponentAngle - delta;
        const float a = (float)DEFAULT_COLLISION_EXTENDLIMIT;
        const float b = (float)newAngle;
        m_wMiscComponentAngle = static_cast<uint16>((int32)(a < b ? a : b));
        return false;
    }

    if (!m_wMiscComponentAngle)
        return false;

    const int32 delta = (int32)(int16)(int32)((float)(pad->GetCarGunUpDown() + 100) * timeStep * 10.0f * 0.0078125f);
    const int32 newAngle = (int32)m_wMiscComponentAngle - delta;
    const float b = (float)newAngle;
    m_wMiscComponentAngle = static_cast<uint16>((int32)(0.0f <= b ? b : 0.0f));

    return false;
}

// 0x6A2150
float CAutomobile::GetMovingCollisionOffset() {
    if (m_wMiscComponentAngle) { // TODO: Invert
        switch (m_nModelIndex) {
        case eModelID::MODEL_DUMPER:
            return m_aCarNodes[CAR_MISC_C] ? (float)m_wMiscComponentAngle * CMonsterTruck::DUMPER_COL_ANGLEMULT : 0.0f;

        // Como el original (0x6A2190..0x6A219C y 0x6A21B4..0x6A21C0): fild + fmul por [87100C] (-0.0001f) y [871010] (0.0002f), no divisiones
        case eModelID::MODEL_PACKER:
            return (float)m_wMiscComponentAngle * PACKER_COL_ANGLE_MULT;

        case eModelID::MODEL_DOZER:
            return m_aCarNodes[CAR_MISC_A] ? (float)m_wMiscComponentAngle * DOZER_COL_ANGLE_MULT : 0.0f;

        case eModelID::MODEL_ANDROM:
            return m_aCarNodes[CAR_MISC_E] ? (float)m_wMiscComponentAngle * CPlane::ANDROM_COL_ANGLE_MULT : 0.0f;

        case eModelID::MODEL_FORKLIFT:
            return (float)m_wMiscComponentAngle * 0.0006f; // / 1666.66?
        }
    }
    return 0.f;
}

// 0x6A2390
void CAutomobile::TellHeliToGoToCoors(float x, float y, float z, float altitudeMin, float altitudeMax) {
    m_autoPilot.m_vecDestinationCoors = CVector{ x, y, z };
    m_autoPilot.SetCarMission(MISSION_HELI_FLYTOCOORS);
    m_autoPilot.SetCruiseSpeed(100);

    AsHeli()->m_fMinAltitude = altitudeMin;
    AsHeli()->m_fMaxAltitude = altitudeMax;

    SetStatus(STATUS_PHYSICS);

    if (m_fAircraftGoToHeading == 0.f) {
        // Como el original (0x6A23FD..0x6A2443): PI = 0x40490FDB (fadd [858CB8]) y 2*PI = 0x40C90FDB ([858CBC]), no los
        // valores imprecisos de common.h; tras el bucle NO se resta otra vez 2*PI (el EXE salta a 0x6A2449 y retorna).
        const auto heliPi    = std::bit_cast<float>(0x40490FDBu);
        const auto heliTwoPi = std::bit_cast<float>(0x40C90FDBu);
        m_fAircraftGoToHeading = CGeneral::GetATanOfXY(m_matrix->GetForward().x, m_matrix->GetForward().y) + heliPi;
        if (m_fAircraftGoToHeading > heliTwoPi) {
            auto heading = m_fAircraftGoToHeading;
            do
                heading -= heliTwoPi;
            while (heading > heliTwoPi);
            m_fAircraftGoToHeading = heading;
        }
    }
}

// 0x6A2450
void CAutomobile::SetHeliOrientation(float angle) {
    m_fForcedOrientation = angle;
}

// 0x6A2460
void CAutomobile::ClearHeliOrientation() {
    m_fForcedOrientation = -1.0f;
}

// 0x6A2470
void CAutomobile::TellPlaneToGoToCoors(float x, float y, float z, float altitudeMin, float altitudeMax) {
    m_autoPilot.m_vecDestinationCoors = CVector{ x, y, z };
    m_autoPilot.SetCarMission(MISSION_PLANE_FLYTOCOORS);
    m_autoPilot.SetCruiseSpeed(0);

    // Como el original (0x6A2474..0x6A24C5): minimo = altitudeMin tal cual ([9B0h]); maximo = (altitudeMax > z) ? altitudeMax : z ([9A8h])
    AsPlane()->m_minAltitude = altitudeMin;
    AsPlane()->m_maxAltitude = (altitudeMax > z) ? altitudeMax : z;

    SetStatus(STATUS_PHYSICS);

    if (m_fAircraftGoToHeading == 0.f) {
        m_fAircraftGoToHeading = CGeneral::GetATanOfXY(m_matrix->GetForward().x, m_matrix->GetForward().y);
    }

    vehicleFlags.bEngineOn = true;
}

// unused
// 0x6A2510
void CAutomobile::HideAllComps() {
    // NOP
}

// unused
// 0x6A2520
void CAutomobile::ShowAllComps() {
    // NOP
}

// 0x6A2530
void CAutomobile::SetRandomDamage(bool bRemoveStuff) {
    //> Bugfix shit
    const auto FixedRandom = [&](int32 x, int32 y) {
        // I'm very sure somebody fell into the trap of the shit naming of `GetRandomNumberInRange`
        // (The int version of it is exclusive, while the float one is inclusive)
        // Reason being that at `0x6A260B` the possible interval is [0, 1), that is, in integer terms a fat `0` :D
        // Obviously, they intended it to be `[0, 1]`
#ifdef FIX_BUGS
        return (size_t)CGeneral::GetRandomNumberInRange(x, y + 1);
#else
        return (size_t)CGeneral::GetRandomNumberInRange(x, y);
#endif
    };

    //> Processes a group of components (panels/doors)
    const auto Process = [
        this,
        IsCompDamageable = [vs = GetVehicleModelInfo()->GetVehicleStruct()](auto nodeIdx) {
            return vs->IsComponentDamageable(nodeIdx);
        }
    ](
        size_t pickedbs, 
        size_t numToDmg,
        size_t numTotal,
        auto   GetNodeIdx,
        auto   IsDamStateOk,
        auto   SetAsDamaged
    ) {
        /**
        * Get the `n`-th component that wasn't yet picked
        * @param n 0-based number
        * @param pickedbs Bitset of components that were picked previously
        */
        const auto GetNthUnpicked = [&](size_t n) {
            if (pickedbs == 0) {
                return n; // No pickedbs components so we can directly return
            }
            for (size_t i = 0; i < numTotal; i++) {
                if ((pickedbs & (1 << i)) == 0) {
                    if (n-- == 0) {
                        return i;
                    }
                }
            }
            NOTSA_UNREACHABLE(); // This is reached if the first `ncomps` bits are all set - Should never happen
        };

        for (size_t i = 0u; i < numToDmg; i++) {
            const auto pick = GetNthUnpicked((size_t)CGeneral::GetRandomNumberInRange(0, (int32)(numToDmg - i)));
            pickedbs |= 1 << pick; // We mark here for simplicity
            if (!IsDamStateOk(pick)) {
                continue;
            }
            const auto nodeIdx = GetNodeIdx(pick);
            if (!IsCompDamageable(nodeIdx)) {
                continue;
            }
            SetAsDamaged(pick);
            SetComponentVisibility(m_aCarNodes[nodeIdx], eAtomicComponentFlag::ATOMIC_DAMAGED);
        }
    };

    //> Doors
    const auto numDoorsToDmg = FixedRandom(0, bRemoveStuff ? MAX_DOORS : 1);
    Process(
        0,
        numDoorsToDmg,
        MAX_DOORS,
        [](auto i)     { return CDamageManager::GetCarNodeIndexFromDoor((eDoors)i); },
        [this](auto i) { return m_damageManager.GetDoorStatus((eDoors)i) == DAMSTATE_OK; },
        [this](auto i) { return m_damageManager.SetDoorStatus((eDoors)i, DAMSTATE_DAMAGED); }
    );

    //> Panels (With a few exceptions)
    const auto numPanelsToDmg = numDoorsToDmg == 0
        ? bRemoveStuff ? FixedRandom(1, 4) : 1 // The 4 here and below comes from MAX_PANELS - 3 (As 3 panels are never damaged, see below)
        : FixedRandom(0, bRemoveStuff ? 4 : 1);
    Process(
        1 << REAR_LEFT_PANEL | 1 << REAR_RIGHT_PANEL | 1 << WINDSCREEN_PANEL, // These are never damaged
        numPanelsToDmg,
        MAX_PANELS,
        [](auto i)     { return CDamageManager::GetCarNodeIndexFromPanel((ePanels)i); },
        [this](auto i) { return m_damageManager.GetPanelStatus((ePanels)i) == DAMSTATE_OK; },
        [this](auto i) { return m_damageManager.SetPanelStatus((ePanels)i, DAMSTATE_DAMAGED); }
    );
}

// 0x6A27F0
void CAutomobile::SetTotalDamage(bool randomness) {
    const auto IsComponentDamageable = [vehStruct = GetVehicleModelInfo()->m_pVehicleStruct](int32 component) {
        return vehStruct->IsComponentDamageable(component);
    };

    // Doors
    for (auto i = 0u; i < MAX_DOORS; i++) {
        const auto nodeIdx{ CDamageManager::GetCarNodeIndexFromDoor((eDoors)i) };
        if (const auto doorFrame{ m_aCarNodes[nodeIdx] }) {
            // Como el original (0x6A2822): (rand() & 0xFFFF) * (1/32768) * 3.0 truncado = GetRandomIntInRange(0, 3), no la interpolacion de 1/32767
            if (!randomness || CGeneral::GetRandomIntInRange(0, 3)) {
                if (m_damageManager.GetDoorStatus((eDoors)i) == DAMSTATE_OK && IsComponentDamageable(nodeIdx)) {
                    m_damageManager.SetDoorStatus((eDoors)i, DAMSTATE_DAMAGED);
                    SetComponentVisibility(doorFrame, eAtomicComponentFlag::ATOMIC_DAMAGED);
                }
            } else {
                m_damageManager.SetDoorStatus((eDoors)i, DAMSTATE_NOTPRESENT);
                SetComponentVisibility(doorFrame, eAtomicComponentFlag::ATOMIC_NONE);
            }
        }
    }

    // Panels
    for (auto i = 0u; i < MAX_PANELS; i++) { // TODO: Maybe use array of panel enums instead?
        switch ((ePanels)i) {
        case ePanels::REAR_LEFT_PANEL:
        case ePanels::REAR_RIGHT_PANEL:
            break;

        default: {
            const auto nodeIdx{ CDamageManager::GetCarNodeIndexFromPanel((ePanels)i) };
            if (const auto panelFrame{ m_aCarNodes[nodeIdx] }) {
                if (!randomness || CGeneral::GetRandomIntInRange(0, 3)) {
                    if (m_damageManager.GetPanelStatus((ePanels)i) == DAMSTATE_OK && IsComponentDamageable(nodeIdx)) {
                        m_damageManager.SetPanelStatus((ePanels)i, DAMSTATE_OPENED);
                        SetComponentVisibility(panelFrame, eAtomicComponentFlag::ATOMIC_DAMAGED);
                    }
                } else {
                    m_damageManager.SetPanelStatus((ePanels)i, DAMSTATE_OPENED_DAMAGED);
                    SetComponentVisibility(panelFrame, eAtomicComponentFlag::ATOMIC_NONE);
                }
            }
            break;
        }
        }
    }
}

// 0x6A2980
RpMaterial* DisableMatFx(RpMaterial* material, void* data) {
    RpMatFXMaterialSetEffects(material, rpMATFXEFFECTNULL);
    return material;
}

// 0x6A29A0
void CAutomobile::ReduceHornCounter() {
    if (m_HornCounter) {
        m_HornCounter--;
    }
}

static RwTexture*& renderLicensePlateTexture{ *(RwTexture**)0xC1BFD8 };

// 0x6A2F00
void CAutomobile::CustomCarPlate_BeforeRenderingStart(const CVehicleModelInfo& mi)
{
    if (mi.m_pPlateMaterial) {
        renderLicensePlateTexture = RpMaterialGetTexture(mi.m_pPlateMaterial);
        RwTextureAddRef(renderLicensePlateTexture);
        RpMaterialSetTexture(mi.m_pPlateMaterial, m_pCustomCarPlate);
    }
}

// 0x6A2F30
void CAutomobile::CustomCarPlate_AfterRenderingStop(CVehicleModelInfo* model) {
    if (model->m_pPlateMaterial) {
        RpMaterialSetTexture(model->m_pPlateMaterial, renderLicensePlateTexture);
        RwTextureDestroy(renderLicensePlateTexture);
        renderLicensePlateTexture = nullptr;
    }
}

/*
 * Check if plyrveh is in air
 * @addr 0x6A2F70
 */
bool CAutomobile::GetAllWheelsOffGround() const {
    return m_NumDriveWheelsOnGround == 0;
}

// 0x6A2F80
void CAutomobile::DebugCode() {
    auto* mi = GetVehicleModelInfo();
    auto* vehicle = FindPlayerVehicle();
    auto* remoteVehicle = FindPlayerInfo().m_pRemoteVehicle;
    if (vehicle == this && !remoteVehicle || remoteVehicle == this) {
        auto* handling      = gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId);
        m_fMass             = handling->m_fMass;
        m_fTurnMass         = handling->m_fTurnMass;
        m_fBuoyancyConstant = handling->m_fBuoyancyConstant;
        m_fAirResistance    = GetDefaultAirResistance(); // todo: check handling. does handling in f() equal handling above
    }
}

// 0x6A3580
void CAutomobile::FixTyre(eWheels wheel) {
    m_damageManager.SetWheelStatus(wheel, WHEEL_STATUS_OK);
}

// 0x6A35A0
void CAutomobile::FixDoor(int32 nodeIndex, eDoors door) {
    if (!m_pHandlingData->m_bNoDoors) { // TODO: Invert
        m_doors[door].Open(0.f);
        m_damageManager.SetDoorStatus(door, DAMSTATE_OK);
        if (const auto frame = m_aCarNodes[nodeIndex]) {
            SetComponentVisibility(frame, eAtomicComponentFlag::ATOMIC_OK);

            // Reset it's matrix
            // Como el original (0x6A3614): la matriz de modelado del marco (frame+0x10, RwFrameGetMatrix), no la LTM; y se queda con
            // la posicion que ya tiene la propia matriz (lee x,y,z de la matriz), no con la del vehiculo
            CMatrix mat{ RwFrameGetMatrix(frame), false };
            const CVector matPos = mat.GetPosition();
            mat.SetTranslate(matPos);
            mat.UpdateRW();
        }
    }
}

// 0x6A3670
void CAutomobile::FixPanel(eCarNodes nodeIndex, ePanels panel) {
    m_damageManager.SetPanelStatus(panel, DAMSTATE_OK);

    // Remove any bouncing panels belonging to this frame
    // Como el original (0x6A36A2): el bucle recorre 4 entradas de 0x20 bytes desde 0x6AC (los 3 paneles y los primeros 2 bytes
    // de m_swingingChassis); compara movsx word con nodeIndex y pone 0xFFFF (mov word ptr [eax],0FFFFh)
    {
        auto* const bouncing = reinterpret_cast<uint8*>(m_panels.data());
        for (size_t i = 0; i < 4; i++) {
            auto& frameId = *reinterpret_cast<int16*>(bouncing + i * sizeof(CBouncingPanel));
            if ((int32)frameId == (int32)nodeIndex) {
                frameId = (int16)-1;
            }
        }
    }

    if (const auto frame = m_aCarNodes[nodeIndex]) { // Same code as in FixDoor.. Maybe this was a standalone function, like "FixNode"?
        SetComponentVisibility(frame, eAtomicComponentFlag::ATOMIC_OK);

        // Como el original (0x6A36E0): matriz de modelado (frame+0x10) y posicion de la propia matriz
        CMatrix mat{ RwFrameGetMatrix(frame), false };
        const CVector matPos = mat.GetPosition();
        mat.SetTranslate(matPos);
        mat.UpdateRW();
    }
}

// 0x6A3740
void CAutomobile::SetTaxiLight(bool enable) {
    autoFlags.bTaxiLight = enable;
}

// 0x6A3760
void CAutomobile::SetAllTaxiLights(bool enable) {
    m_sAllTaxiLights = enable;
}

// 0x6A3820
void CAutomobile::PlayHornIfNecessary() {
    if ((m_autoPilot.carCtrlFlags.bHonkAtPed || m_autoPilot.carCtrlFlags.bHonkAtCar) && !HasCarStoppedBecauseOfLight()) {
        PlayCarHorn();
    }
}

// 0x6A3860
void CAutomobile::SetBusDoorTimer(uint32 timerEndDelta, bool setAsStartedInPast) {
    const auto time = setAsStartedInPast ? CTimer::GetTimeInMS() - 500u : CTimer::GetTimeInMS();
    m_nBusDoorTimerStart = time;
    m_nBusDoorTimerEnd   = time + std::max(1000u, timerEndDelta);
}

// 0x6A38A0
void CAutomobile::ProcessAutoBusDoors() {
    // Como el original: reconstruido desde 0x6A38A0..0x6A3A58 (la version anterior exigia el bit de m_nGettingInFlags activo y no cerraba bien las puertas)
    if (m_nBusDoorTimerEnd <= CTimer::GetTimeInMS()) { // 0x6A38B2 jbe 0x6A39E4
        if (m_nBusDoorTimerStart) { // 0x6A39E4
            // 0x6A39F0..0x6A3A3F: cada puerta se abre del todo (ratio 0) si no falta y su bit de m_nGettingInFlags NO esta activo
            if (!IsDoorMissing(eDoors::DOOR_LEFT_FRONT) && !(m_nGettingInFlags & 1)) {
                OpenDoor(nullptr, tComponent::COMPONENT_DOOR_RR, eDoors::DOOR_LEFT_FRONT, 0.f, true);
            }
            if (!IsDoorMissing(eDoors::DOOR_RIGHT_FRONT) && !(m_nGettingInFlags & 4)) {
                OpenDoor(nullptr, tComponent::COMPONENT_DOOR_RF, eDoors::DOOR_RIGHT_FRONT, 0.f, true);
            }

            m_nBusDoorTimerStart = 0;
            m_nBusDoorTimerEnd = 0;
        }
    } else if (m_nBusDoorTimerEnd && CTimer::GetTimeInMS() > m_nBusDoorTimerEnd - 500) { // 0x6A38B8..0x6A38C7
        // 0x6A38CD..0x6A394F (puerta delantera izquierda)
        if (!IsDoorMissing(eDoors::DOOR_LEFT_FRONT) && !(m_nGettingInFlags & 1)) {
            float ratio;
            if (IsDoorClosed(eDoors::DOOR_LEFT_FRONT)) {
                m_nBusDoorTimerEnd = CTimer::GetTimeInMS(); // 0x6A392A
                ratio = 0.f;
            } else {
                // 0x6A38F6..0x6A3924: fild (+2^32 si es negativo); fmul [0x858F44] (0.002f); fsubr 1.0
                ratio = 1.0f - (float)(uint32)(CTimer::GetTimeInMS() - m_nBusDoorTimerEnd + 500) * 0.002f;
            }
            OpenDoor(nullptr, tComponent::COMPONENT_DOOR_RR, eDoors::DOOR_LEFT_FRONT, ratio, true);
        }

        // 0x6A3952..0x6A39DE (puerta delantera derecha); lee de nuevo m_nBusDoorTimerEnd (puede haberlo cambiado la anterior)
        if (!IsDoorMissing(eDoors::DOOR_RIGHT_FRONT) && !(m_nGettingInFlags & 4)) {
            float ratio;
            if (IsDoorClosed(eDoors::DOOR_RIGHT_FRONT)) {
                m_nBusDoorTimerEnd = CTimer::GetTimeInMS(); // 0x6A39B9
                ratio = 0.f;
            } else {
                ratio = 1.0f - (float)(uint32)(CTimer::GetTimeInMS() - m_nBusDoorTimerEnd + 500) * 0.002f;
            }
            OpenDoor(nullptr, tComponent::COMPONENT_DOOR_RF, eDoors::DOOR_RIGHT_FRONT, ratio, true);
        }
    }
}

// 0x6A3A60
void CAutomobile::BoostJumpControl() {
    if (m_pDriver && m_pDriver->IsPlayer()) { // TODO: Invert
        if (const auto playerPad = m_pDriver->AsPlayer()->GetPadFromPlayer()) {
            if (playerPad->HornJustDown() && m_fWheelsSuspensionCompression[0] < 1.f) {
                ApplyMoveForce(CVector{ 0.f, 0.f, 1.f } * (m_fMass * 0.15f));
                // Como el original: 0x6A3B72 fld [esi+0x90]; fmul [0x858C58] (=0.01f): multiplica, no divide
                ApplyTurnForce(m_matrix->GetUp() * (m_fTurnMass * 0.01f), m_matrix->GetForward());
            }
        }
    }
}

// 0x6A3BD0
void CAutomobile::DoNitroEffect(float power) {
    // TODO: Refactor

    const auto& mi = *GetVehicleModelInfo();
    CVector exhaustPosition = mi.GetModelDummyPosition(DUMMY_EXHAUST);

    bool firstExhaustSubmergedInWater = false;
    bool secondExhaustSubmergedInWater = false;
    float level = 0.0f;
    if (physicalFlags.bTouchingWater) {
        CVector point = m_matrix->TransformPoint(exhaustPosition);
        if (CWaterLevel::GetWaterLevel(point.x, point.y, point.z, level, true, nullptr)) {
            if (level >= point.z)
                firstExhaustSubmergedInWater = true;
        }
    }
    CVector secondExhaustPosition;
    if (m_pHandlingData->m_bDoubleExhaust) {
        secondExhaustPosition = exhaustPosition;
        secondExhaustPosition.x *= -1.0f;
        if (!physicalFlags.bTouchingWater) {
            CVector point = m_matrix->TransformPoint(secondExhaustPosition);
            if (CWaterLevel::GetWaterLevel(point, level, true)) {
                if (level >= point.z)
                    secondExhaustSubmergedInWater = true;
            }
        }
    }

    RwMatrix* rwMatrix = GetModellingMatrix();

    FxSystem_c* firstExhaustFxSystem = m_exhaustNitroFxSystem[0];
    if (firstExhaustFxSystem) {
        firstExhaustFxSystem->SetConstTime(1, std::fabs(power));
        if (firstExhaustFxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_PLAYING && firstExhaustSubmergedInWater)
            firstExhaustFxSystem->Stop();
        else if (firstExhaustFxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED && !firstExhaustSubmergedInWater)
            firstExhaustFxSystem->Play();
    } else if (!firstExhaustSubmergedInWater && rwMatrix) {
        firstExhaustFxSystem = g_fxMan.CreateFxSystem("nitro", exhaustPosition, rwMatrix, true);
        m_exhaustNitroFxSystem[0] = firstExhaustFxSystem;
        if (firstExhaustFxSystem) {
            firstExhaustFxSystem->SetLocalParticles(true);
            firstExhaustFxSystem->Play();
        }
    }

    if (m_pHandlingData->m_bDoubleExhaust) {
        FxSystem_c* secondExhaustFxSystem = m_exhaustNitroFxSystem[1];
        if (secondExhaustFxSystem) {
            secondExhaustFxSystem->SetConstTime(1, std::fabs(power));
            if (secondExhaustFxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_PLAYING && secondExhaustSubmergedInWater)
                secondExhaustFxSystem->Stop();
            else if (secondExhaustFxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED && !secondExhaustSubmergedInWater)
                secondExhaustFxSystem->Play();
        } else if (!firstExhaustSubmergedInWater && rwMatrix) {
            secondExhaustFxSystem = g_fxMan.CreateFxSystem("nitro", secondExhaustPosition, rwMatrix, true);
            m_exhaustNitroFxSystem[1] = secondExhaustFxSystem;
            if (secondExhaustFxSystem) {
                secondExhaustFxSystem->SetLocalParticles(1);
                secondExhaustFxSystem->Play();
            }
        }
    }
}

// 0x6A3E60
void CAutomobile::StopNitroEffect() {
    for (auto&& fx : m_exhaustNitroFxSystem) {
        FxSystem_c::SafeKillAndClear(fx);
    }
}

// 0x6A3EA0
void CAutomobile::NitrousControl(int8 boost) {
    CPad* pad = nullptr;
    if (GetStatus() == STATUS_PLAYER && m_pDriver->IsPlayer()) {
        pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    }

    if (boost > 0) {
        handlingFlags.bNosInst = true;
        m_fTireTemperature = 1.0f;
        m_nNitroBoosts = boost;
        StopNitroEffect();
        return;
    }

    if (boost < 0) {
        handlingFlags.bNosInst = false;
        m_fTireTemperature = 1.0f; // todo: FIX_BUGS: 0.0f?
        m_nNitroBoosts = 0;
        StopNitroEffect();
        return;
    }

    // Como el original: 0x6A3F4D/0x6A3F53 (cmp [0x8A4],1.0 bitwise; mov al,[0x48A]; test al,al; jle): m_nNitroBoosts se trata como int8 con signo
    if (m_fTireTemperature == 1.0f && static_cast<int8>(m_nNitroBoosts) > 0) {
        if (GetStatus() != STATUS_PHYSICS) {
            if (   !pad
                || !pad->GetCarGunFired()
                || pad->GetLookLeft()
                || pad->GetLookRight()
                || pad->GetLookBehindForCar()
            ) {
                StopNitroEffect();
                return;
            }
        }

        m_fTireTemperature = -0.000001f; // Just set some small negative value

        if (static_cast<int8>(m_nNitroBoosts) >= 101) { // todo: magic number (0x6A3FA7 cmp al,65h / jge: con signo)
            StopNitroEffect();
            return;
        }

        m_nNitroBoosts -= 1;
        StopNitroEffect();
        return;
    }

    // Como el original: 0x6A3FC6 fcomp qword [0x859EF8] (0.0) / test ah,5 / jp 0x6A4075: la rama >= 0 tambien se toma con NaN (!(temp < 0))
    if (!(m_fTireTemperature < 0.f)) {
        // Como el original: 0x6A4081..0x6A4090 fcomp 0.25 / test ah,41h / jne: conserva 1-gas salvo que 0.25 > 1-gas (con NaN conserva 1-gas)
        const float oneMinusGas = 1.f - m_GasPedal;
        const float a = (0.25f > oneMinusGas) ? 0.25f : oneMinusGas;
        // Como el original: 0x6A406E.. fld timestep; fmul [0x871088] (=0.001f); fmulp a; fadd temp (multiplica, no divide; 0.001 no 0.01)
        const float b = a * (CTimer::GetTimeStep() * 0.001f) + m_fTireTemperature;
        // Como el original: 0x6A40B2 fcomp 1.0 / test ah,41h / jne: solo se fija 1.0 si b > 1.0 (con NaN conserva b)
        m_fTireTemperature = (b > 1.0f) ? 1.0f : b;
        const auto power = (1.0f - m_fTireTemperature) / 2.f;
        DoNitroEffect(power);
        return;
    }

    // Como el original: 0x6A3FD7 fld timestep; fmul [0x871084] (=0.001f); fsubr temp (multiplica, no divide)
    m_fTireTemperature -= CTimer ::GetTimeStep() * 0.001f;
    if (m_fTireTemperature < -1.f) {
        m_fTireTemperature = 0.000001f; // Just set some small positive value.
        if (!m_nNitroBoosts) {
            handlingFlags.bNosInst = false;
            RemoveUpgrade(eVehicleUpgradePosn::UPGRADE_NITRO);
            m_fTireTemperature = 1.0f;
        }
    }
    // Como el original: 0x6A4033 fcomp [0x858B50] (0.0) / test ah,41h / jne 0x6A4064: con gas <= 0 o NaN el parametro es 0.5f
    DoNitroEffect(
        !(m_GasPedal > 0.0f)
            ? 0.5f
            : std::abs(m_GasPedal) * 0.5f + 0.5f
    );
}

template<typename T>
auto Get(CEntity** entities, size_t numEntities)
    requires std::is_base_of_v<CEntity, T>
{
    return (
        std::span{ entities, numEntities }
      | std::views::transform([](auto&& entity) { return reinterpret_cast<T*>(entity); })
    );
};

// 0x6A40F0
void CAutomobile::TowTruckControl() {
    const auto UP_SPEED   = 6.0f;
    const auto DOWN_SPEED = 2.0f;

    if (GetStatus() != STATUS_PLAYER) {
        return;
    }

    if (!m_pDriver || !m_pDriver->IsPlayer()) {
        return;
    }

    const auto driversPad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    if (!driversPad) {
        return;
    }

    // Update misc door. angle
    const auto carUpDown = driversPad->GetCarGunUpDown();
    if (std::fabs(carUpDown) > 10.f) {
        const auto speed = carUpDown > 0 ? DOWN_SPEED : UP_SPEED;
        // Como el original: 0x6A41B6 call _ftol2; movsx ecx,ax -> el paso es un int16 con signo
        const auto step = (int16)(int32)(float(carUpDown) * speed * CTimer::GetTimeStep());

        while (true) {
            // up
            if (carUpDown > 0) {
                m_wMiscComponentAngle = std::max(
                    m_pVehicleBeingTowed ? TOWTRUCK_HOIST_UP_LIMIT : 0,
                    m_wMiscComponentAngle - step
                );
            } else if (m_wMiscComponentAngle < TOWTRUCK_HOIST_DOWN_LIMIT) { // down
                // Como el original: 0x6A425A cmp ebx,ecx / jge: minimo con signo de 32 bits, guardado como word
                m_wMiscComponentAngle = std::min<int32>(
                    TOWTRUCK_HOIST_DOWN_LIMIT,
                    m_wMiscComponentAngle - step
                );
            }
            break;
        }
    }

    // Attach a suitable plyrveh in range if we don't already have a trailer
    if (m_wMiscComponentAngle != TOWTRUCK_HOIST_DOWN_LIMIT || m_pVehicleBeingTowed)
        return;

    CVector towBarPos{};
    if (!GetTowBarPos(towBarPos, false, this))
        return;

    CEntity* entitiesInRange[16]{};
    int16 numEntitiesInRange{};
    CVector hitchPos{};
    CWorld::FindObjectsInRange(towBarPos, 10.f, true, &numEntitiesInRange, (int16)std::size(entitiesInRange), entitiesInRange, false, true, false, false, false);

    for (auto* vehicle : Get<CVehicle>(entitiesInRange, numEntitiesInRange)) {
        if (vehicle == this)
            continue;

        if (!vehicle->GetTowHitchPos(hitchPos, true, this))
            continue;

        if (vehicle->vehicleFlags.bIsLocked != 0u)
            continue;

        // Como el original: 0x6A436E..0x6A43BF calcula dx,dy,dz = hitch - bar; fsqrt(dx*dx+dy*dy) < 0.5f ([0x871094]) y despues fabs(dz) < 1.0f ([0x871098])
        // (distancia 2D con raiz, NO la distancia al cuadrado frente a 0.25f)
        const float dx = hitchPos.x - towBarPos.x;
        const float dy = hitchPos.y - towBarPos.y;
        const float dz = hitchPos.z - towBarPos.z;
        const float dist2D = std::sqrt(dy * dy + dx * dx);
        if (dist2D < 0.5f && std::fabs(dz) < 1.0f) {
            vehicle->SetTowLink(this, false);
            m_wMiscComponentAngle -= 100; // "hide" hoist
            break;
        }
    }
}

// 0x6A44C0
CPed* CAutomobile::KnockPedOutCar(eWeaponType type, uint16 a2, CPed* ped) {
    return ped;
}

// 0x6A44D0
// UNUSED - Likely inlined?
void CAutomobile::PopBootUsingPhysics() {
    // Como el original (0x6A44D8..0x6A4510): estado 0 -> SetDoorStatus(1, 1) y luego (1, 3); estado 2 -> SetDoorStatus(1, 3);
    // el resto (1, 3, 4) no cambia nada; en todos los casos m_angVel = -2.0f (0xC0000000)
    switch (m_damageManager.GetDoorStatus(eDoors::DOOR_BOOT)) {
    case DAMSTATE_OK:
        m_damageManager.SetDoorStatus(eDoors::DOOR_BOOT, DAMSTATE_OPENED);
        [[fallthrough]];
    case DAMSTATE_DAMAGED:
        m_damageManager.SetDoorStatus(eDoors::DOOR_BOOT, DAMSTATE_OPENED_DAMAGED);
        break;
    default:
        break;
    }
    m_doors[eDoors::DOOR_BOOT].m_angVel = -2.f;
}

// 0x6A4520
void CAutomobile::CloseAllDoors() {
    const auto& mi = *GetVehicleModelInfo();

    const auto CloseDoor = [this](tComponent comp, eDoors door) {
        if (!IsDoorMissing(door)) {
            OpenDoor(nullptr, comp, door, 0.f, true);
        }
    };

    assert(mi.m_nNumDoors > 0);
    CloseDoor(tComponent::COMPONENT_DOOR_RR, eDoors::DOOR_LEFT_FRONT); // TODO: Again, enums dont seem to match up..
    if (mi.m_nNumDoors > 1) {
        CloseDoor(tComponent::COMPONENT_DOOR_RF, eDoors::DOOR_RIGHT_FRONT);
        if (mi.m_nNumDoors > 2) {
            assert(mi.m_nNumDoors == 4); // Pirulax: May assert, I'm not sure. Let me know if it did.

            CloseDoor(tComponent::COMPONENT_WING_LF, eDoors::DOOR_LEFT_REAR); // TODO: Enums dont match at all..
            CloseDoor(tComponent::COMPONENT_DOOR_LR, eDoors::DOOR_RIGHT_REAR);
        }
    }
}

void CAutomobile::DoSoftGroundResistance(uint32& extraHandlingFlags) {
    if (IsAnyWheelTouchingSand() && m_nModelIndex != MODEL_RCBANDIT && m_nModelIndex != MODEL_RHINO) {
        // Como el original (0x6A4BC2..0x6A4C2B): velocidad menos su componente sobre up, y modulo cuadrado en el orden z, y, x
        const float dot = DotProduct(m_vecMoveSpeed, GetUp());
        const float dotUpX = dot * GetUp().x;
        const float dotUpY = dot * GetUp().y;
        const float dotUpZ = dot * GetUp().z;
        CVector speedUp{ m_vecMoveSpeed.x - dotUpX, m_vecMoveSpeed.y - dotUpY, m_vecMoveSpeed.z - dotUpZ };
        const float squaredMag = (speedUp.z * speedUp.z + speedUp.y * speedUp.y) + speedUp.x * speedUp.x;
        float offroadAbility;
        // Como el original (0x6A4C2B..0x6A4C40): fcompp + test ah,41h / jne: <= (con NaN tambien entra)
        if (!(squaredMag > 0.3f * 0.3f)) {
            autoFlags.bIsBoggedDownInSand = true;
            offroadAbility = 0.005f;
        } else {
            // Como el original (0x6A4C46..0x6A4CCF): fsqrt, cada componente * 0.3 y despues * (1 / magnitud) (no al reves)
            const float magnitude = std::sqrt(squaredMag);
            const float scaledX = speedUp.x * 0.3f;
            const float scaledY = speedUp.y * 0.3f;
            const float scaledZ = speedUp.z * 0.3f;
            const float recip = 1.0f / magnitude;
            speedUp.x = scaledX * recip;
            speedUp.y = scaledY * recip;
            speedUp.z = scaledZ * recip;
            float scale = 1.0f - (magnitude + magnitude);
            if (0.2f > scale) {
                scale = 0.2f;
            }
            offroadAbility = scale * 0.005f;
        }

        if (handlingFlags.bOffroadAbility2)
            offroadAbility *= 0.3f;
        else if (handlingFlags.bOffroadAbility)
            offroadAbility *= 0.6f;

        if (CWeather::WetRoads > 0.2f)
            offroadAbility *= (1.2f - CWeather::WetRoads);

        float direction = -(offroadAbility * (CTimer::GetTimeStep() * m_fMass));
        ApplyMoveForce(speedUp * direction);
        extraHandlingFlags |= EXTRA_HANDLING_WHEELS_TOUCHING_SAND;
    } else if (IsAnyWheelTouchingRailTrack()) {
        if (m_nModelIndex != MODEL_RCBANDIT && m_nModelIndex != MODEL_RHINO) {
            CVector speedUp = m_vecMoveSpeed - DotProduct(m_vecMoveSpeed, GetUp()) * GetUp();
            float direction = -(CTimer::GetTimeStep() * m_fMass * CVehicle::ms_fRailTrackResistance);
            ApplyMoveForce(speedUp * direction);
        }
    }
}

// 0x6A4EC0
void CAutomobile::ProcessCarWheelPair(eCarWheel leftWheel, eCarWheel rightWheel, float steerAngle, CVector* contactSpeeds, CVector* contactPoints, float traction, float acceleration, float brake, bool bFront)
{
    bool driveWheels = false; // TODO: Refactor please
    if (bFront && gHandlingDataMgr.HasFrontWheelDrive(m_pHandlingData->m_nVehicleId))
        driveWheels = true;
    else if (!bFront && gHandlingDataMgr.HasRearWheelDrive(m_pHandlingData->m_nVehicleId))
        driveWheels = true;

    float suspensionBias = 2.0f * m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear; // TODO: Use lambda
    if (!bFront) {
        suspensionBias = 2.0f - suspensionBias;

        if (!vehicleFlags.bIsHandbrakeOn || handlingFlags.bHbRearwheelSteer) {
            // Como el original (0x6A4F99): el EXE lee una palabra (2 bytes) en 0x86A, no solo el byte de m_bDoingBurnout
            if (driveWheels && *reinterpret_cast<const uint16*>(&m_bDoingBurnout) != 0) {
                brake = 0.0f;
                traction = 0.0f;
                // Como el original (0x6A4FC8..0x6A5009): min(1, 3000 / masaGiro) * (-0.002 * masaGiro), luego * giro del volante
                const float turnMassScale = std::bit_cast<float>(0xBB03126Fu) * m_fTurnMass;
                const float burnoutScale = std::min(1.0f, 3000.0f / m_fTurnMass) * turnMassScale;
                CVector point = (burnoutScale * m_fSteerAngle) * GetRight();
                ApplyTurnForce(contactPoints[leftWheel], point);
            } else if (!handlingFlags.bNosInst && gHandlingDataMgr.HasRearWheelDrive(m_pHandlingData->m_nVehicleId)) {
                traction *= m_fTireTemperature;
            }
        } else {
            brake = 20'000.0f;
        }
    }

    if (m_WheelCounts[leftWheel] > 0.0f || m_WheelCounts[rightWheel] > 0.0f) {
        // TOOD: Refactor
        float sinSteerAngle = 0.0f;
        float cosSteerAngle = 0.0f;
        bool canSteer = steerAngle > -100.0f;
        if (canSteer) {
            // Como el original (0x6A5134..0x6A514B): fsin y fcos del x87 (0x41BCE0 / 0x41BCF0) con precision completa
            sinSteerAngle = G4Fsin(steerAngle);
            cosSteerAngle = G4Fcos(steerAngle);
        }

        const auto neutralHandling = GetStatus() != STATUS_PLAYER && GetStatus() != STATUS_REMOTE_CONTROLLED && handlingFlags.bNpcNeutralHandl;

        // TOOD: Refactor
        float brakeBias = 0.0f;
        float tractionBias = 0.0f;

        if (bFront) {
            brakeBias = neutralHandling ? 1.0f : 2.0f * m_pHandlingData->m_fBrakeBias;
            tractionBias = neutralHandling ? 1.0f : 2.0f * m_pHandlingData->m_fTractionBias;
        } else {
            brakeBias = neutralHandling ? 1.0f : 2.0f - (2.0f * m_pHandlingData->m_fBrakeBias);
            tractionBias = neutralHandling ? 1.0f : 2.0f - (2.0f * m_pHandlingData->m_fTractionBias);
        }

        // Como el original: los dos bloques (rueda izquierda y derecha) son identicos salvo la comprobacion del Hydra
        // en la izquierda (0x6A53B5..0x6A53BF), que la derecha no tiene
        const auto ProcessOneWheel = [&](eCarWheel wheel, bool checkHydra) {
            float thrust = driveWheels ? acceleration : 0.0f;

            CVector wheelFwd = GetForward();
            const float fwdDot = DotProduct(wheelFwd, m_wheelColPoint[wheel].m_vecNormal);
            wheelFwd -= fwdDot * m_wheelColPoint[wheel].m_vecNormal;
            wheelFwd.Normalise();
            CVector wheelRight = CrossProduct(wheelFwd, m_wheelColPoint[wheel].m_vecNormal);
            wheelRight.Normalise();
            if (canSteer && (!checkHydra || m_nModelIndex != MODEL_HYDRA))
            {
                CVector tmp = cosSteerAngle * wheelFwd - sinSteerAngle * wheelRight;
                wheelRight = sinSteerAngle * wheelFwd + cosSteerAngle * wheelRight;
                wheelFwd = tmp;
            }
            m_wheelColPoint[wheel].m_nSurfaceTypeA = SURFACE_WHEELBASE;
            float adhesion = g_surfaceInfos.GetAdhesiveLimit(&m_wheelColPoint[wheel]) * traction;
            if (TreatAsPlayerForCollisions())
            {
                adhesion *= g_surfaceInfos.GetWetMultiplier(m_wheelColPoint[wheel].m_nSurfaceTypeB);
                // Como el original (0x6A550D..0x6A5525): (((1 - compresion) * 4) * fuerza) * sesgo, y luego min con 2
                const float suspensionAdhesion = (((1.0f - m_fWheelsSuspensionCompression[wheel]) * 4.0f) * m_pHandlingData->m_fSuspensionForceLevel) * suspensionBias;
                adhesion = (suspensionAdhesion > 2.0f ? 2.0f : suspensionAdhesion) * adhesion;
                if (handlingFlags.bOffroadAbility2 && g_surfaceInfos.GetAdhesionGroup(m_wheelColPoint[wheel].m_nSurfaceTypeB) > ADHESION_GROUP_ROAD)
                {
                    adhesion *= 1.4f;
                }
                else if (handlingFlags.bOffroadAbility)
                {
                    if (g_surfaceInfos.GetAdhesionGroup(m_wheelColPoint[wheel].m_nSurfaceTypeB) > ADHESION_GROUP_ROAD)
                        adhesion *= 1.15f;
                }
            }
            tWheelState wheelState = m_WheelStates[wheel];
            if (m_damageManager.GetWheelStatus(wheel) == WHEEL_STATUS_BURST) {
                CVehicle::ProcessWheel(wheelFwd, wheelRight, contactSpeeds[wheel], contactPoints[wheel], m_nNumContactWheels,
                    thrust,
                    brake * brakeBias,
                    adhesion * m_damageManager.m_fWheelDamageEffect * tractionBias,
                    wheel, &m_fWheelBurnoutSpeed[wheel], &wheelState, WHEEL_STATUS_BURST);
            }
            else {
                CVehicle::ProcessWheel(wheelFwd, wheelRight, contactSpeeds[wheel], contactPoints[wheel], m_nNumContactWheels,
                    thrust,
                    brake * brakeBias,
                    adhesion * tractionBias,
                    wheel, &m_fWheelBurnoutSpeed[wheel], &wheelState, WHEEL_STATUS_OK);
            }
            if (driveWheels && m_GasPedal < 0.0f && wheelState == WHEEL_STATE_SPINNING)
                m_WheelStates[wheel] = WHEEL_STATE_NORMAL;
            else
                m_WheelStates[wheel] = wheelState;
        };

        if (m_WheelCounts[leftWheel] > 0.0f) {
            ProcessOneWheel(leftWheel, true);
        }

        if (m_WheelCounts[rightWheel] > 0.0f) {
            ProcessOneWheel(rightWheel, false);
        }
    }

    if (!bFront && !handlingFlags.bNosInst) {
        if (*reinterpret_cast<const uint16*>(&m_bDoingBurnout) != 0 && driveWheels &&
            (m_WheelStates[CAR_WHEEL_REAR_LEFT] == WHEEL_STATE_SPINNING ||
            m_WheelStates[CAR_WHEEL_REAR_RIGHT] == WHEEL_STATE_SPINNING))
        {
            // Como el original (0x6A5C68..0x6A5CB4): temperatura += paso * 0.001 (0x858CDC), no paso / 1000
            m_fTireTemperature = CTimer::GetTimeStep() * std::bit_cast<float>(0x3A83126Fu) + m_fTireTemperature;
            if (m_fTireTemperature > 3.0f) {
                m_fTireTemperature = 3.0f;
            }
        }
        else if (m_fTireTemperature > 1.0f) {
            // Como el original (0x6A5CCF..0x6A5D0F): pow(0.995, paso) del EXE (0x470000 -> 0x822130) con precision completa
            m_fTireTemperature = G4TireTemperatureDecay(m_fTireTemperature - 1.0f, CTimer::GetTimeStep());
        }
    }

    if (IsRealHeli()) {
        return;
    }

    // Como el original (0x6A5D34..0x6A6003): mismo bloque para cada rueda sin contacto
    const auto SpinFreeWheel = [&](eCarWheel wheel) {
        if (m_WheelCounts[wheel] <= 0.0f) {
            if (driveWheels && acceleration != 0.0f) {
                if (acceleration > 0.0f) {
                    if (m_wheelSpeed[wheel] < 1.0f) {
                        m_wheelSpeed[wheel] -= 0.1f;
                    }
                } else {
                    if (m_wheelSpeed[wheel] > -1.0f) {
                        m_wheelSpeed[wheel] += 0.05f;
                    }
                }
            } else {
                m_wheelSpeed[wheel] *= 0.95f;
            }
            m_wheelRotation[wheel] += CTimer::GetTimeStep() * m_wheelSpeed[wheel];
        }
    };
    SpinFreeWheel(leftWheel);
    SpinFreeWheel(rightWheel);
}

// TODO: Could add these to CMatrix as well, might be useful.. Like: CMatrix::GetRoll()

// Ayudantes de X87Math.cpp (AutomobileCarRollDblCpp / PlaceableRollDblCpp) que devuelven un double: el fpatan (y su producto) queda sin redondear en ST0.
// Aqui se declaran como float (alias del enlazador, como en Placeable.cpp) para que el metodo devuelva ST0 tal cual.
extern "C" float __cdecl AutomobileCarRollFltCpp(const float* right, float upZ);
extern "C" float __cdecl AutomobileCarPitchFltCpp(const float* forward, float upZ);
#pragma comment(linker, "/alternatename:_AutomobileCarRollFltCpp=_AutomobileCarRollDblCpp")
#pragma comment(linker, "/alternatename:_AutomobileCarPitchFltCpp=_PlaceableRollDblCpp")

/*!
* @addr 0x6A6010
* @brief Returns `roll` in DEGREES
*/
float CAutomobile::GetCarRoll() {
    // Como el original (0x6A6010, igual que CPlaceable::GetRoll 0x420B30): sqrt(x*x + y*y) con fsqrt, signo segun up.z (fcomp + test ah,5 / jp: solo negativo si up.z < 0),
    // fpatan a precision completa y multiplicacion por 57.2957763671875 (0x859878) sin redondear antes a float: el ayudante x87 devuelve double (ST0)
    // y esta funcion lo declara como float con otro nombre (/alternatename) para que el resultado salga tal cual en ST0, como el EXE.
    const auto& right = m_matrix->GetRight();
    return AutomobileCarRollFltCpp(&right.x, m_matrix->GetUp().z);
}

/*!
* @addr 0x6A6050
* @brief Returns `pitch` in RADIANS
*/
float CAutomobile::GetCarPitch() {
    // Como el original (0x6A6050): igual que GetCarRoll pero con el vector forward y sin escalar el resultado de fpatan; es el mismo calculo que
    // CPlaceable::GetRoll sobre el vector forward (PlaceableRollDblCpp), con el fpatan sin redondear en ST0.
    const auto& fwd = m_matrix->GetForward();
    return AutomobileCarPitchFltCpp(&fwd.x, m_matrix->GetUp().z);
}

// 0x6A6140
bool CAutomobile::IsInAir() {
    if (physicalFlags.bDontApplySpeed) {
        return true;
    }
    if (physicalFlags.bSubmergedInWater) {
        return false;
    }
    // Como el original (0x6A6151..0x6A61D0): cada compresion se compara con fcomp + test ah,5 / jnp: solo "menor que 1" (no NaN) corta
    for (const float compression : m_fWheelsSuspensionCompression) {
        if (compression < 1.0f) {
            return false;
        }
    }
    // Como el original (0x6A619D..0x6A61CB): fcomp con 0 y test ah,44h: cualquier componente distinta de cero (o NaN) es "en el aire"
    return m_vecMoveSpeed.x != 0.0f || m_vecMoveSpeed.y != 0.0f || m_vecMoveSpeed.z != 0.0f;
}

// 0x6A6DC0
void CAutomobile::dmgDrawCarCollidingParticles(const CVector& position, float force, eWeaponType weapon) {
    if (!GetIsOnScreen()) {
        return;
    }

    switch (weapon) {
    case eWeaponType::WEAPON_UNARMED:
    case eWeaponType::WEAPON_FLOWERS:
        break;
    default: {
        auto fxDirection = m_vecMoveSpeed;
        const auto speedMag = fxDirection.NormaliseAndMag();

        // Como el original (0x6A6E1C..0x6A6E8E): la fuerza se trunca a int32 (_ftol2), se divide entre 10 con signo, se suma 4 y se
        // enmascara con 0x3F (and edx,3Fh); la posicion se pasa como copia local; el tipo de chispa es 1 (SPARK_PARTICLE_SPARK)
        // y los dos ultimos parametros son 0.3f y 1.0f
        CVector sparkPos = position;
        g_fx.AddSparks(
            sparkPos,
            fxDirection,
            speedMag * -10.f,
            (((int32)force / 10) + 4) & 0x3F,
            m_vecMoveSpeed,
            eSparkType::SPARK_PARTICLE_SPARK,
            0.3f,
            1.f
        );
        break;
    }
    }


    // Como el original (0x6A6EE4..0x6A6F3C): (posicion - propia) * 0.7f + propia, componente a componente (no la lerp generica)
    const CVector& ownPos = GetPosition();
    CVector fxPos{ (position.x - ownPos.x) * 0.7f + ownPos.x, (position.y - ownPos.y) * 0.7f + ownPos.y, (position.z - ownPos.z) * 0.7f + ownPos.z }; // Actually const, just that fx functions dont take consts :D

    // Add smoke
    {
        // The higher our speedsq the more particles we create
        // Como el original (0x6A6F40..0x6A6F8A): (ts*vx, ts*vy, ts*vz) -> raiz de la suma (z, y, x) -> *4.0f -> _ftol2 (int32 con signo) y minimo 1
        const float tsx = CTimer::GetTimeStep() * m_vecMoveSpeed.x;
        const float tsy = CTimer::GetTimeStep() * m_vecMoveSpeed.y;
        const float tsz = CTimer::GetTimeStep() * m_vecMoveSpeed.z;
        const float tsMag = std::sqrt((tsz * tsz + tsy * tsy) + tsx * tsx); // la pila x87 deja z arriba: (z*z + y*y) + x*x
        const auto numSmokes = std::max(1, (int32)(tsMag * 4.f));
        for (auto i = 0; i < numSmokes; i++) {
            // Como el original (0x6A6EB6): el ultimo parametro de FxPrtMult_c es 0.1f (0x3DCCCCCD)
            g_fx.m_SmokeHuge->AddParticle(fxPos, {}, 0.f, FxPrtMult_c{ 0.4f, 0.4f, 0.4f, 0.6f, 0.4f, 1.f, 0.1f });
        }
    }

    // Add debris
    // Como el original (0x6A6FD9..0x6A6FEA): fcomp + test ah,41h + jne -> solo si SquaredMagnitude > 0.0625f (estricto; NaN sale)
    const float speedSq = (m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.z * m_vecMoveSpeed.z;
    if (speedSq > sq(0.25f)) {
        // Como el original (0x6A6FF0..0x6A7054): R, G y B se multiplican por el brillo de contacto con _ftol2 y se guarda el byte bajo;
        // el alfa de la tabla no se toca
        auto color = CVehicleModelInfo::ms_vehicleColourTable[m_nPrimaryColor];
        color.ScaleRGB(m_fContactSurfaceBrightness);
        auto rwcolor = color.ToRwRGBA();
        g_fx.AddDebris(fxPos, rwcolor, 0.06f, (int32)force / 100 + 1);
    }
}

// 0x6A7090
void CAutomobile::ProcessCarOnFireAndExplode(bool bExplodeImmediately) {
    // Como el original: reconstruido desde 0x6A7090..0x6A7643 (la version anterior no creaba las particulas de fuego de los coches,
    // aplicaba mal el factor del contador de fuego, usaba la posicion de mundo del dummy y consumia numeros aleatorios de mas)
    const uint8 engineStatus = static_cast<uint8>(m_damageManager.GetEngineStatus()); // 0x6A70B3 call 0x6C22C0 (se guarda en [esp+0xF])

    // 0x6A75FF..0x6A762D
    const auto FinishHealthAndProcess = [&] {
        if (engineStatus > 225 && m_fHealth > 250.f) { // cmp byte [esp+0xF],0E1h / fcomp [0x859F80]; test ah,41h
            m_fHealth = m_fHealth - 2.f;                // fsub [0x858CA0]
        }
        ProcessDelayedExplosion();
    };

    // 0x6A758D..0x6A75E0
    const auto ApplyVelocityAndFinish = [&] {
        if (m_pFireParticle) {
            CVector bullshit{ m_vecMoveSpeed.x * 50.f, m_vecMoveSpeed.y * 50.f, m_vecMoveSpeed.z * 50.f }; // fmul [0x858B40]
            m_pFireParticle->SetVelAdd(bullshit);
        }
        FinishHealthAndProcess();
    };

    // 0x6A70BE..0x6A70ED: sale al "reset" si !(health < 250), esta destrozado o se esta ahogando
    if (!(m_fHealth < 250.f) || GetStatus() == STATUS_WRECKED || vehicleFlags.bIsDrowning) {
        // 0x6A75E2
        m_fBurnTimer = 0.f;
        if (m_pFireParticle) {
            m_pFireParticle->Kill();
            m_pFireParticle = nullptr;
        }
        FinishHealthAndProcess();
        return;
    }

    const bool isSubPlaneOrHeli = m_nVehicleSubType == VEHICLE_TYPE_PLANE || m_nVehicleSubType == VEHICLE_TYPE_HELI;
    int32 partFxToUse = (!m_pFireParticle && !isSubPlaneOrHeli) ? 1 : 0; // 0x6A70ED..0x6A710E (edi)

    // Como el original: 0x6A7138 call _ftol2 -> fild (+2^32 si es negativo) = tiempo del paso en ms como entero sin signo, en float
    const auto TimeStepMS = []() -> float {
        return (float)(uint32)(int32)(CTimer::GetTimeStep() * 0.02f * 1000.f);
    };

    if (!isSubPlaneOrHeli) {
        // 0x6A7126
        m_fBurnTimer = TimeStepMS() + m_fBurnTimer;
        if (m_fBurnTimer > 5000.f) {
            BlowUpCar(m_pExplosionVictim, false); // 0x6A71F3 call [vtable+0xA4]
        }
    } else {
        // 0x6A71CD
        if (!m_fireParticleCounter) {
            m_fireParticleCounter = IsInAir() ? 2 : 1;
        }

        if (bExplodeImmediately) {
            BlowUpCar(m_pExplosionVictim, false); // 0x6A71F3
        } else {
            // 0x6A720B
            const bool isRcShit = notsa::contains({
                MODEL_RCBARON,
                MODEL_RCRAIDER,
                MODEL_RCGOBLIN,
                MODEL_RCBANDIT,
                MODEL_RCTIGER
            }, GetModelId());

            if (m_fireParticleCounter == 1 || isRcShit) { // cmp byte [esi+0x981],1 / je (antes: contador != 0)
                m_fBurnTimer = TimeStepMS() + m_fBurnTimer;
            } else {
                m_fBurnTimer = TimeStepMS() * 0.2f + m_fBurnTimer; // fmul [0x858CC4] (0.2f); antes: / 5.f
            }

            if (m_fBurnTimer > 5000.f) { // 0x6A72A4 / je 0x6A743A
                BlowUpCar(m_pExplosionVictim, false);
            } else { // 0x6A72BB - humo
                FxPrtMult_c fxPrtMult{ 0.f, 0.f, 0.f, 0.4f, 1.f, 1.f, 0.3f };
                if (isRcShit) {
                    if (GetStatus() == STATUS_REMOTE_CONTROLLED) {
                        fxPrtMult.m_Color.alpha = 0.5f; // [esp+0x4C]
                        fxPrtMult.m_fSize       = 0.1f; // [esp+0x50]
                        fxPrtMult.m_fLife       = 0.1f; // [esp+0x58]
                    } else {
                        fxPrtMult.m_Color.alpha = std::bit_cast<float>(0x3E19999Au); // 0.15f
                        fxPrtMult.m_fSize       = 0.3f;
                        fxPrtMult.m_fLife       = 0.3f;
                    }
                }

                if (CTimer::GetFrameCounter() % 2 == 0) { // 0x6A731F test byte [0xB7CB4C],1
                    CVector pos = GetPosition();
                    CVector vel;
                    // Como el original: 0x6A7353..0x6A7433. Orden de las llamadas a 0x41BD90: vx, vy, vz, dx(pos.x), dy(pos.y) (pos.z no consume azar)
                    if (isRcShit) {
                        vel.x = CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);
                        vel.y = CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);
                        vel.z = CGeneral::GetRandomNumberInRange(0.f, 0.4f);
                        pos.x = CGeneral::GetRandomNumberInRange(-0.7f, 0.7f) + pos.x;
                        pos.y = CGeneral::GetRandomNumberInRange(-0.7f, 0.7f) + pos.y;
                    } else {
                        vel.x = CGeneral::GetRandomNumberInRange(-1.5f, 1.5f);
                        vel.y = CGeneral::GetRandomNumberInRange(-1.5f, 1.5f);
                        vel.z = CGeneral::GetRandomNumberInRange(0.f, 1.f);
                        pos.x = CGeneral::GetRandomNumberInRange(-2.f, 2.f) + pos.x;
                        pos.y = CGeneral::GetRandomNumberInRange(-2.f, 2.f) + pos.y;
                    }
                    g_fx.m_SmokeHuge->AddParticle(pos, vel, 0.f, fxPrtMult);
                }
            }

            // 0x6A744D
            const auto status = GetStatus();
            if (status == STATUS_PLAYER || status == STATUS_REMOTE_CONTROLLED) { // 0x6A74F7
                if (!m_pFireParticle && m_fBurnTimer > 2500.f) { // [0x8598B0]
                    partFxToUse = 2;
                }
            } else if (!isRcShit) {
                // 0x6A7499: GetRandomNumberInRange(0, 250) < 3 (0x407180) -> explosion de cohete
                if (CGeneral::GetRandomIntInRange(0, 250) < 3) {
                    CExplosion::AddExplosion(this, m_pExplosionVictim, EXPLOSION_ROCKET, GetPosition(), 0, true, -1.f, false);
                }
            }
        }
    }

    // 0x6A716C / 0x6A7174: crea el sistema de particulas (fire_car / fire_large)
    if (partFxToUse != 0) {
        if (const auto mat = GetModellingMatrix()) {
            // Como el original: 0x6A7179..0x6A7195 lee la posicion del dummy del motor en espacio del objeto (+0x54 de la estructura del modelo)
            const CVector pos = GetVehicleModelInfo()->GetModelDummyPosition(DUMMY_ENGINE);
            if (partFxToUse == 1) {
                m_pFireParticle = g_fxMan.CreateFxSystem("fire_car", pos, mat, false);
            } else if (partFxToUse == 2) {
                m_pFireParticle = g_fxMan.CreateFxSystem("fire_large", pos, mat, false);
            }
        }
        if (m_pFireParticle) { // 0x6A7548
            m_pFireParticle->Play();
            // Como el original: 0x6A7557..0x6A7580 un unico evento (ctor 0x4B10C0, CEventGroup::Add(&evento, false) 0x4AB420, dtor 0x4B1170)
            CEventVehicleOnFire event{ this };
            GetEventGlobalGroup()->Add(&event, false);
        }
    }

    ApplyVelocityAndFinish();
}

// todo: nodeIndex one of { eBikeNodes eBmxNodes eBoatNodes eHeliNodes eMonsterTruckNodes ePlaneNodes eQuadBikeNodes eTrailerNodes eTrainNodes eCarNodes }
// 0x6A8580
CObject* CAutomobile::SpawnFlyingComponent(eCarNodes nodeIndex, uint32 collisionType)
{
    if (CObject::nNoTempObjects >= 150) {
        return nullptr;
    }

    auto compFrame = m_aCarNodes[(size_t)nodeIndex];
    if (!compFrame) {
        return nullptr;
    }

    // Grab frame's atomic (so we can clone it later)
    RpAtomic* compAtomic{};
    RwFrameForAllObjects(compFrame, GetCurrentAtomicObjectCB, &compAtomic);
    if (!compAtomic) {
        return nullptr; // No atomic means there's nothing to render :D
    }

    GetObjectPool()->SetDealWithNoMemory(true);
    auto obj = new CObject;
    GetObjectPool()->SetDealWithNoMemory(false);

    if (!obj) {
        return nullptr;
    }

    const auto& frameLTM = RwFrameGetLTM(compFrame);

    if (nodeIndex == eCarNodes::CAR_WINDSCREEN) {
        obj->SetModelIndexNoCreate(MODEL_TEMPCOL_BONNET1);
    } else {
        switch (collisionType) {
        case 0u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_BUMPER1);

            CMatrix frameMat{ frameLTM, false };
            // Como el original (0x6A86F6..0x6A872B): diferencia (posicion del marco - propia) y producto escalar con right en el
            // orden z, y, x (40FDB0); se niega con fchs
            const CVector diffToFrame = frameMat.GetPosition() - GetPosition();
            const float dotZ = diffToFrame.z * m_matrix->GetRight().z;
            const float dotY = diffToFrame.y * m_matrix->GetRight().y;
            const float dotZY = dotZ + dotY;
            const float dotX = diffToFrame.x * m_matrix->GetRight().x;
            obj->m_vecCentreOfMass = CVector{
                -(dotZY + dotX), // Set it's X centre on mass depending on where it is on our local X axis
                0.f,
                0.f
            };
            break;
        }
        case 1u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_WHEEL1);
            break;
        }
        case 2u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_DOOR1);
            obj->m_vecCentreOfMass = CVector{ 0.f, -0.5f, 0.f };
            obj->m_bDrawLast = true;
            break;
        }
        case 3u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_BONNET1);
            obj->m_vecCentreOfMass = CVector{ 0.f, 0.4f, 0.f };
            break;
        }
        case 4u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_BOOT1);
            obj->m_vecCentreOfMass = CVector{ 0.f, -0.3f, 0.f };
            break;
        }
        default: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_PANEL1);
            break;
        }
        }
    }

    obj->RefModelInfo(m_nModelIndex);

    // Create a new frame using the flying object's atomic
    auto flyingObjFrame = RwFrameCreate();
    auto clonedFlyingObjAtomic = RpAtomicClone(compAtomic);  // Clone this component's atomic
    RpAtomicSetFrame(clonedFlyingObjAtomic, flyingObjFrame); // Associate the cloned atomic with a the new frame
    *RwFrameGetMatrix(flyingObjFrame) = *frameLTM; // Set this frame's matrix to be the same as the component's - TODO: This most likely isn't the correct way to do this..
    CVisibilityPlugins::SetAtomicRenderCallback(clonedFlyingObjAtomic, nullptr);
    obj->AttachToRwObject((RwObject*)clonedFlyingObjAtomic, true);
    obj->m_bDontStream = true;
    obj->m_fMass = 10.f;
    obj->m_fTurnMass = 25.f;
    obj->m_fAirResistance = 0.97f;
    obj->m_fElasticity = 0.1f;
    obj->m_fBuoyancyConstant = std::bit_cast<float>(0x3DDA740Fu); // Como el original (0x6A87E3): inmediato 3DDA740Fh (no es 0.106666f ni 8/75)
    obj->m_nObjectType = OBJECT_TEMPORARY;

    obj->SetIsStatic(false);

    // Como el original (0x6A87F7..0x6A8834): bIsPickup = false y bChangesVehColor = true (and ~1, or 0x80 sobre el dword 0x140),
    // se copian los dos primeros colores del coche (0x14C, 0x14D) y SetRemapTexture(m_pRemapTexture, movsx word [0x598])
    obj->m_nObjectFlags = (obj->m_nObjectFlags & ~1u) | 0x80u;
    obj->m_nCarColor[0] = m_nPrimaryColor;
    obj->m_nCarColor[1] = m_nSecondaryColor;
    obj->SetRemapTexture(m_pRemapTexture, m_nPreviousRemapTxd);

    CObject::nNoTempObjects++;
    // Como el original (0x6A8839..0x6A88B6): hasta 10 objetos suma 20000 como entero; de 11 a 20 y a partir de 21 el tiempo se carga
    // con fild (con signo, + 2^32 si es negativo), se le suma 10000.0f / 4000.0f en el x87 y se trunca con _ftol2
    if (CObject::nNoTempObjects <= 20u) {
        if (CObject::nNoTempObjects <= 10u) {
            obj->m_nRemovalTime = CTimer::GetTimeInMS() + 20'000;
        } else {
            obj->m_nRemovalTime = G2TimeMsPlusFloat(10000.0f); // 0x859AA4
        }
    } else {
        obj->m_nRemovalTime = G2TimeMsPlusFloat(4000.0f); // 0x871240
    }

    const auto GetMoveSpeedZ = [&, this] {
        // Como el original (0x6A88D5..0x6A8931): z > 0 -> z * 1.5f; si no, con up.z > 0.0 (se compara con un double) y tipo 3 o 4 o
        // nodo 18 (WINDSCREEN, el primer argumento) -> 0.04f - z * 1.5f; en el resto z * 0.25f
        if (m_vecMoveSpeed.z > 0.f) {
            return m_vecMoveSpeed.z * 1.5f;
        }
        if (m_matrix->GetUp().z > 0.0 && (collisionType == 3 || collisionType == 4 || nodeIndex == eCarNodes::CAR_WINDSCREEN)) {
            return 0.04f - m_vecMoveSpeed.z * 1.5f;
        }
        return m_vecMoveSpeed.z * 0.25f;
    };
    obj->m_vecMoveSpeed = CVector{ m_vecMoveSpeed.x * 0.75f, m_vecMoveSpeed.y * 0.75f, GetMoveSpeedZ() };
    obj->m_vecTurnSpeed = m_vecTurnSpeed * 2.f;

    auto objDir = Normalized(obj->GetPosition() - GetPosition());
    // Como el original (0x6A89BF..0x6A8A3B): el tipo 3 o 4 o el nodo 18 (WINDSCREEN, primer argumento; no el tipo 18)
    if (collisionType == 3 || collisionType == 4 || nodeIndex == eCarNodes::CAR_WINDSCREEN) {
        objDir += m_matrix->GetUp();
        if (m_matrix->GetUp().z > 0.f/*not flipped*/) {
            obj->m_matrix->GetPosition() += m_matrix->GetUp() * m_vecMoveSpeed.Magnitude2D();
        }
    }

    obj->ApplyMoveForce(objDir);

    if (collisionType == 1) {
        obj->m_fTurnMass = 5.f;
        obj->m_vecTurnSpeed.x = 0.5f;
        obj->m_fAirResistance = 0.99f;
    }

    if (GetStatus() == STATUS_WRECKED && IsVisible()) {
        const auto camDirUnnorm = TheCamera.GetPosition() - GetPosition();
        // Como el original (0x6A8AC3..0x6A8ADE): producto escalar en el orden z, y, x frente a -0.5f
        const float camDotZ = objDir.z * camDirUnnorm.z;
        const float camDotY = objDir.y * camDirUnnorm.y;
        const float camDotZY = camDotZ + camDotY;
        const float camDotX = objDir.x * camDirUnnorm.x;
        if ((camDotZY + camDotX) > -0.5f) { // Object's direction is opposite to that of the camera's
            auto camDir = Normalized(camDirUnnorm);
            camDir.z += 0.3f;
            obj->ApplyMoveForce(camDir * 5.f); // Apply some move force toward the camera's direction
        }
    }

    if (CCollision::ProcessColModels(*obj->m_matrix, *obj->GetColModel(), *m_matrix, *GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) > 0)
        obj->m_pEntityIgnoredCollision = this;

    if (physicalFlags.bRenderScorched)
        obj->physicalFlags.bRenderScorched = true;

    CWorld::Add(obj);

    return obj;
}

// 0x6A8C00
void CAutomobile::ProcessBuoyancy()
{
    // Como el original (0x6A8C21..0x6A8C36): el EXE llama a los tres CTimeCycle::GetAmbient*_Obj (resultado descartado) y a
    // rand() (0x821B1E), que SI avanza la semilla del generador
    (void)CTimeCycle::GetAmbientRed_Obj();
    (void)CTimeCycle::GetAmbientGreen_Obj();
    (void)CTimeCycle::GetAmbientBlue_Obj();
    (void)CGeneral::GetRandomNumber();

    CVector vecBuoyancyTurnPoint;
    CVector vecBuoyancyForce;
    if (!mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &vecBuoyancyTurnPoint, &vecBuoyancyForce)) {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
        physicalFlags.bTouchingWater = false;

        m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;
        for (int32 i = 0; i < 4; ++i) {
            auto& colPoint = m_wheelColPoint[i];
            if (m_fWheelsSuspensionCompression[i] < 1.0F && g_surfaceInfos.IsWater(colPoint.m_nSurfaceTypeB)) {
                auto vecWaterImpactVelocity = (colPoint.m_vecPoint + GetUp() * 0.3F) - GetPosition();
                CVector vecSpeed = GetSpeed(vecWaterImpactVelocity);
            }
        }
        return; // The loop above is pretty much an overcomplicated NOP.
    }

    if (IsSubPlane() && m_vecMoveSpeed.z < -1.0F) {
        BlowUpCar(this, false);
    }

    physicalFlags.bTouchingWater = true;
    // Como el original (0x6A8C9B..0x6A8CC8): max(0.01, paso), y masa usada = masa * 0.008 (0x863984), no masa / 125
    const float fTimeStep = (0.01F > CTimer::GetTimeStep()) ? 0.01F : CTimer::GetTimeStep();
    const float fUsedMass = m_fMass * std::bit_cast<float>(0x3C03126Fu);
    float fBuoyancyForceZ = vecBuoyancyForce.z / ((fTimeStep * m_fMass) * std::bit_cast<float>(0x3C03126Fu));

    // Como el original (0x6A8CCC..0x6A8CF5): ((masaUsada / constante) * fuerzaZ) * 1.05
    if (fUsedMass > m_fBuoyancyConstant)
        fBuoyancyForceZ = ((fUsedMass / m_fBuoyancyConstant) * fBuoyancyForceZ) * std::bit_cast<float>(0x3F866666u);

    if (physicalFlags.bMakeMassTwiceAsBig)
        fBuoyancyForceZ *= 1.5F;

    // Como el original (0x6A8D10..0x6A8D3B): 1 - fuerzaZ * 0.05 (0x858C28) y max con 0.5
    float fBuoyancyForceMult = 1.0F - fBuoyancyForceZ * std::bit_cast<float>(0x3D4CCCCDu);
    if (0.5F > fBuoyancyForceMult)
        fBuoyancyForceMult = 0.5F;
    // Como el original (0x6A8D3F..0x6A8D54): pow(multiplicador, paso) con el _CIpow del EXE (0x822130), guardado como float
    const float fSpeedMult = G4PowToFloat(fBuoyancyForceMult, CTimer::GetTimeStep());

    if (m_nModelIndex != MODEL_VORTEX
        || !(GetUp().z > 0.3F)
        || vehicleFlags.bIsDrowning) {

        m_vecMoveSpeed *= fSpeedMult;
        m_vecTurnSpeed *= fSpeedMult;
    }

    bool bHeliRotorKilled = false;
    if (m_pHandlingData->m_bIsHeli && m_fHeliRotorSpeed > 0.15F) {
        bool bForceKillRotor = false;
        if (m_nModelIndex != MODEL_SEASPAR && m_nModelIndex != MODEL_LEVIATHN) {
            // Como el original (0x6A8DFA..0x6A8E75): max(1, fuerzaZ * 8); la division por escalar es x * (1 / d) (0x4119D0)
            float fUsedForce = fBuoyancyForceZ * 8.0F;
            if (1.0F > fUsedForce)
                fUsedForce = 1.0F;
            const CVector vecScaled{ vecBuoyancyForce.x * -2.0F, vecBuoyancyForce.y * -2.0F, vecBuoyancyForce.z * -2.0F };
            const float fRecip = 1.0F / fUsedForce;
            const CVector vecMoveForceHeli{ vecScaled.x * fRecip, vecScaled.y * fRecip, vecScaled.z * fRecip };
            ApplyMoveForce(vecMoveForceHeli);
            m_vecTurnSpeed *= fSpeedMult;
            if (fBuoyancyForceZ <= 0.9F)
                return;

            bForceKillRotor = true;
        }

        if (fBuoyancyForceZ > 3.0F || bForceKillRotor) {
            m_fHeliRotorSpeed = 0.0F;
            bHeliRotorKilled = true;
        }
    }

    physicalFlags.bTouchingWater = true;
    ApplyMoveForce(vecBuoyancyForce);
    ApplyTurnForce(vecBuoyancyForce, vecBuoyancyTurnPoint);

    if (ModelIndices::IsAmphibiousHeli(m_nModelIndex)
        && fBuoyancyForceZ < 3.0F
        && (GetUp().z > -0.5F || fBuoyancyForceZ < 0.6F)) {

        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
        return;
    }

    // 0x6A8F67
    if ((CCheat::IsActive(CHEAT_CARS_ON_WATER) || m_nModelIndex == MODEL_VORTEX)
        && GetStatus() == STATUS_PLAYER
        && GetUp().z > 0.3F
    ) {
        if (!vehicleFlags.bIsDrowning) {
            vehicleFlags.bIsDrowning = false; // todo: FIX_BUGS?
            physicalFlags.bSubmergedInWater = false;
            return;
        }
    }

    if (bHeliRotorKilled || fBuoyancyForceZ >= 1.0F
        || (fBuoyancyForceZ > 0.6F && IsAnyWheelNotMakingContactWithGround())) {

        vehicleFlags.bIsDrowning = true;
        physicalFlags.bSubmergedInWater = true;

        // Como el original (0x6A9051..0x6A9072): solo si z < -0.1
        if (m_vecMoveSpeed.z < -0.1F)
            m_vecMoveSpeed.z = -0.1F;

        // Como el original (0x6A9079..0x6A90AA): masa * 0.0064 (0x871248) y masa * 8.0e-6 (0x871244), leidos del EXE
        if (m_fMass * std::bit_cast<float>(0x3BD1B718u) < m_fBuoyancyConstant)
            m_fBuoyancyConstant = m_fBuoyancyConstant - m_fMass * std::bit_cast<float>(0x370637BEu);

        if (m_fBuoyancyConstant < m_fMass * std::bit_cast<float>(0x3C03126Fu))
            vehicleFlags.bEngineOn = false;

        auto driver = m_pDriver->AsPed();
        ProcessPedInVehicleBuoyancy(driver, true);

        for (const auto passenger : GetPassengers()) {
            ProcessPedInVehicleBuoyancy(passenger, false);
        }
    }
    else {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
    }
}

inline void CAutomobile::ProcessPedInVehicleBuoyancy(CPed* ped, bool bIsDriver) {
    if (!ped)
        return;

    ped->physicalFlags.bTouchingWater = true;
    if (!ped->IsPlayer() && autoFlags.bWaterTight)
        return;

    if (!IsSubQuad() || IsAnyWheelMakingContactWithGround()) {
        if (ped->IsPlayer())
            ped->AsPlayer()->HandlePlayerBreath(true, 1.0F);
        else {
            auto pedDamageResponseCalc = CPedDamageResponseCalculator(this, CTimer::GetTimeStep(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, false);
            auto damageEvent = CEventDamage(this, CTimer::GetTimeInMS(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, true);
            if (damageEvent.AffectsPed(ped))
                pedDamageResponseCalc.ComputeDamageResponse(ped, damageEvent.m_damageResponse, true);
            else
                damageEvent.m_damageResponse.m_bDamageCalculated = true;

            ped->GetEventGroup().Add(&damageEvent, false);
        }
    } else {
        auto fDamageIntensity = m_vecMoveSpeed.Magnitude() * m_fMass; // TODO: It's "force"

        auto knockOffBikeEvent = CEventKnockOffBike(this, m_vecMoveSpeed, (-m_vecMoveSpeed).Normalized(), fDamageIntensity,
            0.0F, KNOCK_OFF_TYPE_FALL, 0, 0, nullptr, false, false);

        ped->GetEventGroup().Add(&knockOffBikeEvent, false);

        if (ped->IsPlayer())
            ped->AsPlayer()->HandlePlayerBreath(true, 1.0F);

        if (bIsDriver)
            vehicleFlags.bEngineOn = false;
    }
}

// 0x6A9680
void CAutomobile::ProcessHarvester()
{
    if (GetStatus() != STATUS_PLAYER)
        return;

    CStreaming::m_bStreamHarvesterModelsThisFrame = true;
    if (m_vecMoveSpeed.Magnitude2D() > 0.01f) {
        if ((CTimer::GetFrameCounter() & 1) != 0)
        {
            for (int32 i = GetPedPool()->GetSize() - 1; i >= 0; i--) {
                CPed* ped = GetPedPool()->GetAt(i);
                if (!ped)
                    continue;

                if (ped->IsPlayer())
                    continue;

                // Como el original: 0x6A9752..0x6A9815. Los productos escalares se suman como (y*y' + z*z') + x*x' (no x, y, z)
                CVector distance = ped->GetPosition() - GetPosition();
                const auto DotExe = [&](const CVector& v) { return (distance.y * v.y + distance.z * v.z) + distance.x * v.x; };
                float distanceForward = DotExe(GetForward());
                if (distanceForward > 4.0f && distanceForward < 5.0f) {
                    float distanceRight = std::abs(DotExe(GetRight()));
                    if (distanceRight < 4.0f) {
                        float distanceUp = std::abs(DotExe(GetUp()));
                        if (distanceUp < 4.0f) {
                            ped->FlagToDestroyWhenNextProcessed();
                            m_harvesterParticleCounter = 20;
                            m_vehicleAudio.AddAudioEvent(AE_BODY_HARVEST, 0.0f);
                        }
                    }
                }
            }
        }
        else
        {
            for (int32 i = GetObjectPool()->GetSize() - 1; i >= 0; i--) {
                CObject* object = GetObjectPool()->GetAt(i);
                if (!object)
                    continue;

                // Como el original: 0x6A98A0..0x6A98CC compara el modelo del OBJETO (antes el del cosechador) y exige m_bIsVisible y !m_bRenderDamaged
                if (object->m_nModelIndex != ModelIndices::MI_GRASSHOUSE && object->m_nModelIndex != ModelIndices::MI_GRASSPLANT)
                    continue;
                if (!object->m_bIsVisible || object->m_bRenderDamaged)
                    continue;

                CVector distance1 = object->GetPosition() - GetPosition();
                const auto DotExe1 = [&](const CVector& v) { return (distance1.y * v.y + distance1.z * v.z) + distance1.x * v.x; };
                float distanceForward1 = DotExe1(GetForward());
                if (distanceForward1 > 4.0f && distanceForward1 < 5.0f) {
                    float distanceRight1 = std::abs(DotExe1(GetRight()));
                    if (distanceRight1 < 4.0f) {
                        float distanceUp1 = std::abs(DotExe1(GetUp()));
                        if (distanceUp1 < 4.0f)
                            object->ObjectDamage(99999.0f, nullptr, nullptr, this, WEAPON_RUNOVERBYCAR);
                    }
                }
            }
        }
    }

    if (!m_harvesterParticleCounter)
        return;

    // Como el original: 0x6A99FD call 0x59C890 (matriz * punto) con su orden de sumas ((up*z + fwd*y) + right*x) + pos; CMatrix::TransformPoint suma distinto
    CVector pos = G6TransformPointExe(*m_matrix, CVector(-1.2f, -3.8f, 1.5f));
    CVector velocity = GetForward() * -0.1f;
    // Como el original: 0x6A9A2C..0x6A9A81: ((rand * (1/32767) * 0.1f) + vel) - 0.05f, sin GetRandomNumberInRange
    const auto RandT = []() { return (float)(int32)CGeneral::GetRandomNumber() * std::bit_cast<float>(0x38000100u); }; // [0x858C7C]
    velocity.x = (RandT() * 0.1f + velocity.x) - 0.05f;
    velocity.y = (RandT() * 0.1f + velocity.y) - 0.05f;
    int32 bodyPartModelId = -1;
    switch (m_harvesterParticleCounter - 1) { // todo: remove -1 and refactor?
    case 0:
    case 6:
        bodyPartModelId = ModelIndices::MI_HARVESTERBODYPART1;
        break;
    case 1:
    case 3:
        bodyPartModelId = ModelIndices::MI_HARVESTERBODYPART2;
        break;
    case 2:
        bodyPartModelId = ModelIndices::MI_HARVESTERBODYPART3;
        break;
    case 5:
        bodyPartModelId = ModelIndices::MI_HARVESTERBODYPART4;
        break;
    }

    if (CLocalisation::ShootLimbs() && bodyPartModelId >= 0) {
        auto* limb = new CObject(bodyPartModelId, true);
        limb->CPlaceable::SetMatrix(*m_matrix); // Como el original: 0x6A9AF2..0x6A9AFD (ecx = el miembro, no el cosechador)
        limb->SetPosn(pos);
        limb->m_vecMoveSpeed = velocity;
        // Como el original: 0x6A9B25..0x6A9B9A. Cada componente = rand * (1/32767) * 0.08f - 0.04f; el orden de las llamadas a rand es z, y, x
        // (el primer numero aleatorio va a z, el tercero a x: SetTurnSpeed(x = 3.o, y = 2.o, z = 1.o))
        const float turnZ = RandT() * 0.08f - 0.04f;
        const float turnY = RandT() * 0.08f - 0.04f;
        const float turnX = RandT() * 0.08f - 0.04f;
        limb->m_vecTurnSpeed = CVector{ turnX, turnY, turnZ };
        limb->m_nObjectType = OBJECT_TEMPORARY;
        limb->UpdateRwMatrix();
        limb->UpdateRwFrame();
        limb->SetIsStatic(false);
        CObject::nNoTempObjects++;
        CWorld::Add(limb);
    }
    m_harvesterParticleCounter--;
    if (CLocalisation::Blood() && m_harvesterParticleCounter % 3 == 0) {
        g_fx.m_SmokeII3expand->AddParticle(pos, velocity, 0.0f, { 0.15f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f });
    }
}

// 0x6A9D70
void CAutomobile::ProcessSwingingDoor(eCarNodes nodeIdx, eDoors doorIdx)
{
    // Como el original (0x6A9D70): el estado de la puerta se lee una sola vez en una variable local (bl, un byte) que se modifica
    // con los incrementos y decrementos de abajo; los 2 comprobaciones previas usan 1 y 3 = abierta (suelta) y > 3 = ausente
    auto doorState = (uint8)m_damageManager.GetDoorStatus(doorIdx);

    if (!m_aCarNodes[(size_t)nodeIdx]) {
        return;
    }

    if (doorState != DAMSTATE_OPENED && doorState != DAMSTATE_OPENED_DAMAGED) {
        if (doorState > DAMSTATE_OPENED_DAMAGED) { // jae tras cmp bl,3: ausente (4 o mas)
            return;
        }

        // Estados 0 y 2 (cerrada): solo si el dano por impacto supera 100 (fcomp + test ah,41h: NaN tambien sale), no es el capo y
        // el estado del vehiculo (byte 0x36 >> 3) es 0 (STATUS_PLAYER) o 3 (STATUS_PHYSICS)
        if (!(m_fDamageIntensity > 100.f)) {
            return;
        }
        if ((int32)doorIdx < 1) {
            return;
        }
        if (GetStatus() != STATUS_PLAYER && GetStatus() != STATUS_PHYSICS) {
            return;
        }
    }

    CMatrix frameMatrix{ RwFrameGetMatrix(m_aCarNodes[(size_t)nodeIdx]) };
    const CVector framePos = frameMatrix.GetPosition();

    auto& door = m_doors[doorIdx];

    if (doorState == DAMSTATE_OK || doorState == DAMSTATE_DAMAGED) {
        if (CanDoorsBeDamaged()) {
            // Como el original (0x6A9E52): Multiply3x3 (0x59C790) con el orden de sumas del EXE
            const CVector rotated = G2Multiply3x3(*m_matrix, framePos);
            if (door.ProcessImpact(this, m_moveForce, m_turnForce, rotated)) {
                doorState++;
                m_damageManager.SetDoorStatus(doorIdx, (eDoorStatus)doorState);
            }
        }
    }

    // Try opening the doorIdx (If it's not open already)
    if (doorState != DAMSTATE_OPENED && doorState != DAMSTATE_OPENED_DAMAGED) {
        return;
    }

    // Como el original (0x6A9EAB): segunda CMatrix sobre el marco (se vuelve a leer m_aCarNodes[nodeIdx]) y su posicion guardada
    CMatrix frameMatrix2{ RwFrameGetMatrix(m_aCarNodes[(size_t)nodeIdx]) };
    const CVector framePos2 = frameMatrix2.GetPosition();

    // 0x6A9F02
    // If it's the bonnet, we possibly apply some angle velocity based on our current speedsq
    if (doorIdx == eDoors::DOOR_BONNET) {
        auto& bonnet = m_doors[eDoors::DOOR_BONNET];
        if ((bonnet.m_dirn & DOOR_EXTRA_DIRN_MASK) == DOOR_AXIS_Y) {
            // Como el original (0x6A9F12..0x6A9F51): s = (fsin(angulo) + 0.1f [0x858B1C]) * 0.05f [0x8D31CC] y los productos s*forward, despues por la velocidad
            // (y, z, x), la suma y el fadd con m_angVel quedan en la pila x87 sin pasar por float hasta el fstp final: todo en un ayudante x87.
            bonnet.m_angVel = AutomobileBonnetAngVelCpp(bonnet.m_angle, 0.1f, 0.05f, &m_matrix->GetForward().x, &m_vecMoveSpeed.x, bonnet.m_angVel);
        }
    }

    // 0x6A9F91
    // Now process it, if successful close it and do AudioFX
    {
        // Como el original (0x6A9F57): Multiply3x3 (0x59C790) de la posicion de la segunda matriz
        const CVector rotated = G2Multiply3x3(*m_matrix, framePos2);
        if (door.Process(this, m_moveForce, m_turnForce, rotated)) {
            doorState--;
            m_damageManager.SetDoorStatus(doorIdx, (eDoorStatus)doorState);
            m_vehicleAudio.AddAudioEvent((eAudioEvents)((int32)AE_CAR_BONNET_CLOSE + (int32)doorIdx), 0.f);
        }
    }

    // 0x6A9FCE
    // Como el original: orien[eje] = angulo; SetRotate(orien) y suma de la posicion guardada
    G2UpdateDoorFrameMatrix(door, frameMatrix2);

    // Possibly detach bonnet and let it fly
    if (doorIdx == eDoors::DOOR_BONNET) {
        // Como el original (0x6A9F..0x6AA047): producto escalar z, y, x (primero vz*fz, luego vy*fy, luego vx*fx) frente a 0.4f
        const CMatrix& vehMat = *m_matrix;
        const float dotZ = m_vecMoveSpeed.z * vehMat.GetForward().z;
        const float dotY = m_vecMoveSpeed.y * vehMat.GetForward().y;
        const float dotZY = dotZ + dotY;
        const float dotX = m_vecMoveSpeed.x * vehMat.GetForward().x;
        const float forwardSpeed = dotZY + dotX;
        if (   door.m_doorState == DOOR_HIT_MAX_END                     // Still open (couldn't close it)
            && forwardSpeed > 0.4f // Speed's direction is kinda forwards
        ) {
            auto* flyingObj = SpawnFlyingComponent(CAR_BONNET, 2);

            m_vehicleAudio.AddAudioEvent(AE_BONNET_FLUBBER_FLUBBER, flyingObj);
            SetComponentVisibility(m_aCarNodes[CAR_BONNET], eAtomicComponentFlag::ATOMIC_NONE);
            m_damageManager.SetDoorStatus(DOOR_BONNET, DAMSTATE_NOTPRESENT);

            // Apply some additional forces to the flying component
            if (flyingObj) {
                // Como el original (0x6AA096..0x6AA1AD): rand() & 1 -> (v*0.4 + right*0.1) + up*0.5; si no (v*0.4 - right*0.1) + up*0.5
                const bool plus = (CGeneral::GetRandomNumber() & 1) != 0;
                const CVector up05    = m_matrix->GetUp() * 0.5f;
                const CVector right01 = m_matrix->GetRight() * 0.1f;
                const CVector speed04 = m_vecMoveSpeed * 0.4f;
                const CVector side    = plus ? (speed04 + right01) : (speed04 - right01);
                flyingObj->m_vecMoveSpeed = side + up05;
                flyingObj->ApplyTurnForce(m_matrix->GetUp() * 10.f, m_matrix->GetForward());
            }
        }
    }
}

/*!
* @addr 0x6AA200
* @brief Remove bonnet if it's present and open
* @returns Flying component object (May be null even in case the bonnet was removed - See `SpawnFlyingComponent` as to when this happens)
*/
CObject* CAutomobile::RemoveBonnetInPedCollision() {
    if (!m_aCarNodes[eCarNodes::CAR_BONNET]) {
        return nullptr; // Not present
    }

    if (!m_damageManager.IsDoorPresent(eDoors::DOOR_BONNET) || !m_damageManager.IsDoorOpen(eDoors::DOOR_BONNET)) {
        return nullptr; // Not open/missing
    }

    // Como el original (0x6AA230..0x6AA249): fcompp + test ah,5 + jp -> solo continua si abierta*0.4 < angulo (estricto, y NaN sale)
    if (const auto& bonnet = m_doors[eDoors::DOOR_BONNET]; !(bonnet.GetAngleWhenOpen() * 0.4f < bonnet.m_angle)) {
        return nullptr; // Not open enough
    }

    auto* const flyingComp = SpawnFlyingComponent(eCarNodes::CAR_BONNET, 2u);
    m_vehicleAudio.AddAudioEvent(eAudioEvents::AE_BONNET_FLUBBER_FLUBBER, flyingComp);
    SetComponentVisibility(m_aCarNodes[eCarNodes::CAR_BONNET], eAtomicComponentFlag::ATOMIC_NONE);
    m_damageManager.SetDoorStatus(eDoors::DOOR_BONNET, eDoorStatus::DAMSTATE_NOTPRESENT);
    return flyingComp;
}

// 0x6AA290
// Como el original (0x6AA290..0x6AAB2F): coloca el nodo de rueda `nodeIndex` (2..7) sobre una CMatrix local unida a frame->modelling: escala de la rueda,
// direccion (m_fSteerAngle / m_f2ndSteerAngle), giro (m_wheelRotation) y, segun el eje, inclinacion (asin / atan2) o efectos de agua y arena.
// `flags`: bit 0 = sin inclinacion, bit 1 = rueda reflejada (sin direccion), bit 2 = sin giro de -pi/2 sobre el agua, bit 3 = sin particulas.
// Cada operacion float del EXE se redondea a 24 bits (x87 con control de precision sencillo): aqui son operaciones float una a una; fsin, fpatan y
// _CIasin (0x821E70) se hacen con los ayudantes x87 de X87Math. Los valores que el EXE deja en registro conservan el exponente extendido: no se
// reproduce (solo importaria con desbordamientos intermedios).
void CAutomobile::UpdateWheelMatrix(int32 nodeIndex, int32 flags) {
    constexpr float kPi = std::bit_cast<float>(0x40490FDBu); // 0x858CB8

    RwFrame* const frame = m_aCarNodes.data()[nodeIndex]; // 6AA2B6: indice sin comprobar (esi + ebp*4 + 0x648)
    if (!frame) {
        return;
    }
    // 6AA2DE: tabla de modelos con el word +0x22 con signo; se lee antes de mirar el nodo
    auto* const mi = static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(m_nModelIndex)]);
    if (static_cast<uint32>(nodeIndex) - 2u > 5u) { // 6AA2EC: tabla de saltos 6AAB34 (solo 2..7)
        return;
    }

    // ---- 6AA307..6AA47E: rueda, signo y angulo de direccion del nodo (CAR_WHEEL_RF=2, RM=3, RB=4, LF=5, LM=6, LB=7)
    bool  front = false;      // [esp+14h]
    bool  rear = false;       // bl
    float sign;               // [esp+18h]
    int32 wheel;              // edi
    float steer;              // [esp+10h]
    bool  steerFixed = false; // 6AA474 / 6AA47E: el angulo ya es pi o 0 y no pasa por 6AA3CB
    switch (nodeIndex) {
    case CAR_WHEEL_LF: // 6AA307
        wheel = 0;
        sign  = -1.0f;
        front = true;
        if (handlingFlags.bSteerRearwheels) {
            steer      = kPi;
            steerFixed = true;
        } else {
            steer = m_fSteerAngle;
        }
        break;
    case CAR_WHEEL_RF: // 6AA330
        wheel = 2;
        sign  = 1.0f;
        front = true;
        if (handlingFlags.bSteerRearwheels) {
            steer      = 0.0f;
            steerFixed = true;
        } else {
            steer = m_fSteerAngle;
        }
        break;
    case CAR_WHEEL_LM: // 6AA357
    case CAR_WHEEL_LB:
        wheel = 1;
        sign  = -1.0f;
        rear  = true;
        if (handlingFlags.bSteerRearwheels) {
            steer = -m_fSteerAngle;
        } else if (handlingFlags.bHbRearwheelSteer) {
            steer = m_f2ndSteerAngle;
        } else {
            steer      = kPi;
            steerFixed = true;
        }
        break;
    default: // CAR_WHEEL_RM, CAR_WHEEL_RB: 6AA392
        wheel = 3;
        sign  = 1.0f;
        rear  = true;
        if (handlingFlags.bSteerRearwheels) {
            steer = -m_fSteerAngle;
        } else if (handlingFlags.bHbRearwheelSteer) {
            steer = m_f2ndSteerAngle;
        } else {
            steer      = 0.0f;
            steerFixed = true;
        }
        break;
    }
    // ---- 6AA3CB..6AA47E: reduce la direccion del lado contrario (x0,6) y desplaza pi en las ruedas izquierdas.
    // `test ah,44h; jnp` solo salta con steer == 0; las comparaciones ordenadas son falsas con NaN (el EXE tampoco multiplica con NaN).
    if (!steerFixed) {
        if (!(steer == 0.0f) && !(flags & 2)) {
            const bool negativeSide = sign < 0.0f;
            const bool scaleIt      = (negativeSide == rear) ? (steer > 0.0f) : (steer < 0.0f);
            if (scaleIt) {
                steer = 0.6f * steer; // 0x871048
            }
            if (negativeSide) {
                steer = steer + kPi;
            }
        } else {
            steer = sign < 0.0f ? kPi : 0.0f;
        }
    }

    // ---- 6AA486: CMatrix local unida al modelling del nodo
    CMatrix matrix;
    matrix.Attach(&frame->modelling, false);

    // ---- 6AA495: [esp+24h..2Ch] = (pos.x, pos.y, m_wheelPosition[rueda])
    CVector offset{matrix.GetPosition().x, matrix.GetPosition().y, m_wheelPosition.data()[wheel]};

    // ---- 6AA4C6..6AA515: escala de la rueda ([esp+1Ch]) y factor x (st0)
    float wheelSize = m_fWheelScale; // [esi+458h]
    float xScale;
    if (mi->m_nWheelModelIndex == -1) {
        if (rear) {
            wheelSize = (mi->m_fWheelSizeRear / mi->m_fWheelSizeFront) * wheelSize;
        }
        xScale = mi->m_fWheelSizeFront / (m_fWheelScale * 0.7f); // 0x858CB0
    } else {
        xScale    = 1.0f;
        wheelSize = rear ? mi->m_fWheelSizeRear : mi->m_fWheelSizeFront;
    }

    // ---- 6AA515..6AA5A8: cosechadora (532, nodos 6 y 3) y modelo 513 (nodo 5)
    const int16 model = static_cast<int16>(m_nModelIndex);
    if (model == MODEL_COMBINE && (nodeIndex == 6 || nodeIndex == 3)) {
        wheelSize = 1.7f / mi->m_fWheelSizeFront;                      // 0x8D31D4
        offset.z  = (1.7f - mi->m_fWheelSizeRear) * 0.45f + offset.z; // 0x8D31D0
        steer     = (sign < 0.0f ? steer - kPi : steer) * 0.5f;
        if (sign < 0.0f) {
            steer = steer + kPi;
        }
    } else if (model == 0x201 && nodeIndex == 5) {
        steer = 0.0f;
    }

    // ---- 6AA5A8..6AA65E: anchura de las ruedas segun handling.cfg
    const uint32 hf = m_nHandlingFlagsIntValue; // [esi+38Ch]
    if (static_cast<int32>(m_nVehicleSubType) == VEHICLE_TYPE_MTRUCK || static_cast<int32>(m_nVehicleSubType) == VEHICLE_TYPE_QUAD) {
        xScale = 1.0f;
    } else if (rear && (hf & 0xF000)) {
        if (hf & 0x1000) {
            xScale *= 0.65f; // 0x858F50
        } else if (hf & 0x2000) {
            xScale *= 0.8f; // 0x858C98
        } else if (hf & 0x4000) {
            xScale *= 1.1f; // 0x858F14
        } else {
            xScale *= 1.25f; // 0x8595F0
        }
    } else if (front && (hf & 0x0F00)) {
        if (hf & 0x100) {
            xScale *= 0.65f;
        } else if (hf & 0x200) {
            xScale *= 0.8f;
        } else if (hf & 0x400) {
            xScale *= 1.1f;
        } else {
            xScale *= 1.25f;
        }
    }
    if (model == MODEL_KART) { // 6AA651
        xScale *= 3.0f;        // 0x87104C
    }

    // ---- 6AA65E..6AA6D1: escala y giro
    matrix.SetScale(xScale * wheelSize, wheelSize, wheelSize);
    if (static_cast<int32>(m_nVehicleSubType) == VEHICLE_TYPE_HELI) {
        matrix.Rotate(CVector{0.0f, 0.0f, steer});
    } else {
        const auto  status = m_damageManager.GetWheelStatus(static_cast<eCarWheel>(wheel)); // 0x6C21B0
        const float angle  = sign * m_wheelRotation.data()[wheel];
        // rueda pinchada (estado 1): oscilacion sin(angulo) * 0,3 sobre el eje z (fsin sin redondear antes de multiplicar)
        const float z = static_cast<int32>(status) == 1 ? TrigCompoundSinMulAdd(angle, 0.3f, steer) : steer; // 0x858C24
        matrix.Rotate(CVector{angle, 0.0f, z});
    }

    // ---- 6AA6D6..6AAAC7: inclinacion del eje
    const bool monsterOrHydraulic = autoFlags.bIsMonsterTruck || handlingFlags.bHydraulicInst; // +0x868 bit 7 y +0x38C & 0x20000
    uint32     modelFlags         = 0;
    bool       solidAxle          = monsterOrHydraulic;
    if (!solidAxle) {
        modelFlags = m_pHandlingData->m_nModelFlags; // [[esi+384h]+0CCh]
        solidAxle  = (front && (modelFlags & 0x20000)) || (rear && (modelFlags & 0x200000)); // AXLE_F_SOLID / AXLE_R_SOLID
    }
    if (solidAxle) { // 6AAA8F: atan2 de la diferencia de alturas con la rueda del mismo eje
        const int32 other   = wheel > 1 ? wheel - 2 : wheel + 2;
        const float twoAbsY = std::fabs(offset.y) * 2.0f;
        const float diff    = ((m_wheelPosition.data()[wheel] - m_wheelPosition.data()[other]) * sign) * -1.0f; // 0x858C1C
        matrix.RotateY(TrigAtan2Cpp(diff, twoAbsY));
    } else if ((CCheat::IsActive(CHEAT_CARS_ON_WATER) || model == MODEL_VORTEX) && m_fWheelsSuspensionCompressionPrev.data()[wheel] < 1.0f && !vehicleFlags.bIsDrowning) {
        // 6AA751..6AA904: sobre el agua
        if (!(flags & 4)) {
            matrix.RotateY(sign * std::bit_cast<float>(0xBFC90FDBu)); // 0x859998 (-pi/2)
        }
        if (!(flags & 8)) {
            const FxPrtMult_c waterMults{1.0f, 1.0f, 1.0f, 0.2f, 0.5f, 0.0f, 0.05f}; // 0x4AB290
            const CColPoint&  contact  = m_wheelColPoint.data()[wheel];
            const CVector     position = contact.m_vecPoint;
            const CVector     right{sign * GetRight().x, sign * GetRight().y, sign * GetRight().z};                  // 0x40FE90
            const CVector     half{0.5f * m_vecMoveSpeed.x, 0.5f * m_vecMoveSpeed.y, 0.5f * m_vecMoveSpeed.z};      // 0x40FE90
            CVector           velocity{half.x + right.x, half.y + right.y, half.z + right.z};                        // 0x40FE30
            if (g_surfaceInfos.IsWater(contact.m_nSurfaceTypeB)) { // 0x55E710
                if (((CTimer::GetFrameCounter() - 1u) & 1u) != 0u) {
                    velocity.z = velocity.z + 2.0f; // 0x858CA0
                    g_fx.m_WaterSplash->AddParticle(position, velocity, 0.0f, waterMults, -1.0f, 1.2f, 0.6f, false); // 0x4AA440
                } else {
                    g_fx.m_SmokeHuge->AddParticle(position, velocity, 0.0f, waterMults, -1.0f, 1.2f, 0.6f, false);
                }
            } else if (g_surfaceInfos.IsSand(contact.m_nSurfaceTypeB)) { // 0x55E6F0
                const FxPrtMult_c sandMults{0.81f, 0.67f, 0.57f, 0.35f, 0.5f, 0.0f, 0.05f};
                g_fx.m_Sand->AddParticle(position, velocity, 0.0f, sandMults, -1.0f, 1.2f, 0.6f, false);
            }
        }
    } else if (!(flags & 1) && !(front && (modelFlags & 0x10000)) && !(rear && (modelFlags & 0x100000))) { // AXLE_F_NOTILT / AXLE_R_NOTILT
        // 6AA936..6AAAC7: inclinacion = asin(-(z + alturaDelantera - tamano/2) * signo), limitada a [-1, 1]
        const float raised   = offset.z + m_fFrontHeightAboveRoad;      // [esi+898h]
        const float height   = raised - mi->GetWheelSize(front) * 0.5f; // 0x6A06F0
        const bool  reversed = (front && (modelFlags & 0x80000)) || (rear && (modelFlags & 0x800000)); // AXLE_F_REVERSE / AXLE_R_REVERSE
        const float arg      = (height * -1.0f) * sign;
        const float clamped  = arg > 1.0f ? 1.0f : (arg < -1.0f ? -1.0f : arg);
        const float angle    = CnvAsinFloatCpp(clamped); // _CIasin (0x821E70)
        matrix.RotateY(reversed ? -angle : angle);
    }

    // ---- 6AAAD0: posicion final = posicion de la matriz + (x, y, z)
    matrix.GetPosition().x = matrix.GetPosition().x + offset.x;
    matrix.GetPosition().y = matrix.GetPosition().y + offset.y;
    matrix.GetPosition().z = matrix.GetPosition().z + offset.z;
    matrix.UpdateRW(); // 0x59BBB0
}

// 0x6ADEF0
void CAutomobile::PopDoor(eCarNodes nodeIdx, eDoors doorIdx, bool showVisualEffect) {
    if (!m_damageManager.IsDoorPresent(doorIdx)) {
        return;
    }

    if (showVisualEffect) {
        if (nodeIdx != eCarNodes::CAR_NODE_NONE) {
            SpawnFlyingComponent(nodeIdx, nodeIdx == eCarNodes::CAR_CHASSIS ? 4u : 2u);
        } else {
            const auto obj = SpawnFlyingComponent(CAR_NODE_NONE, 3u)->AsVehicle();
            m_vehicleAudio.AddAudioEvent(eAudioEvents::AE_BONNET_FLUBBER_FLUBBER, obj);
        }
    }

    m_damageManager.SetDoorStatus(doorIdx, DAMSTATE_NOTPRESENT);
    SetComponentVisibility(m_aCarNodes[nodeIdx], eAtomicComponentFlag::ATOMIC_NONE);
}

// 0x6ADF80
void CAutomobile::PopPanel(eCarNodes nodeIdx, ePanels panel, bool showVisualEffect) {
    if (m_damageManager.GetPanelStatus(panel) == ePanelDamageState::DAMSTATE_OPENED_DAMAGED) {
        return;
    }

    if (showVisualEffect) {
        switch (nodeIdx) {
        case eCarNodes::CAR_WHEEL_LF:
        case eCarNodes::CAR_WHEEL_LM:
            SpawnFlyingComponent(nodeIdx, 0u);
            break;
        default:
            SpawnFlyingComponent(nodeIdx, 5u);
            break;
        }
    }

    m_damageManager.SetPanelStatus(panel, ePanelDamageState::DAMSTATE_OPENED_DAMAGED);
    SetComponentVisibility(m_aCarNodes[nodeIdx], eAtomicComponentFlag::ATOMIC_NONE);
}

/*!
* @addr 0x6ADFF0
*/
void CAutomobile::ScanForCrimes() {
    const auto plyrveh = FindPlayerVehicle();
    // Como el original: 0x6AE02A cmp cx,0FFFFh (comparacion de 16 bits sin signo: antes `uint16 != -1` siempre era cierto)
    if (   plyrveh
        && plyrveh->IsAutomobile()
        && plyrveh->m_nAlarmState
        && plyrveh->m_nAlarmState != (uint16)-1
        && plyrveh->GetStatus() != STATUS_WRECKED
    ) {
        // Como el original: 0x6AE07B..0x6AE09B suma (dz*dz + dx*dx) + dy*dy y compara con [0x85A700] (400.0f)
        const CVector& otherPos = plyrveh->GetPosition();
        const CVector& selfPos  = GetPosition();
        const float dx = otherPos.x - selfPos.x;
        const float dy = otherPos.y - selfPos.y;
        const float dz = otherPos.z - selfPos.z;
        if ((dz * dz + dx * dx) + dy * dy < 400.f) {
            // Como el original: 0x6AE0AE..0x6AE0C3 lee CWorld::Players[PlayerInFocus].m_pPed y llama a CPlayerPed::SetWantedLevelNoDrop(1)
            FindPlayerPed()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_1);
        }
    }
}

// Como el original: Multiply3x3 vector * matriz (0x59C810), con el orden de sumas del EXE
// (x: (ry*vy + rz*vz) + vx*rx ; y: (fy*vy + fx*vx) + fz*vz ; z: (uy*vy + ux*vx) + uz*vz). Usado por TankControl y FireTruckControl.
static CVector TankFireMultiplyVectorMatrix59C810(const CVector& v, const CMatrix& m) {
    return CVector{
        (m.GetRight().y * v.y + m.GetRight().z * v.z) + v.x * m.GetRight().x,
        (m.GetForward().y * v.y + m.GetForward().x * v.x) + m.GetForward().z * v.z,
        (m.GetUp().y * v.y + m.GetUp().x * v.x) + m.GetUp().z * v.z,
    };
}

// 0x6AE850
void CAutomobile::TankControl()
{
    // Como el original: constantes leidas del EXE (el PI/TWO_PI de common.h son imprecisos: 3.14159f)
    constexpr float kPi       = std::bit_cast<float>(0x40490FDBu); // [0x858CB8]
    constexpr float kTwoPi    = std::bit_cast<float>(0x40C90FDBu); // [0x858CBC]
    constexpr float kHalfPi   = std::bit_cast<float>(0x3FC90FDBu); // [0x858FE4]
    constexpr float kDeg15    = std::bit_cast<float>(0x3E860A92u); // [0x871068]
    constexpr float kHorMax   = std::bit_cast<float>(0x3F1C61AAu); // [0x871074] (35 grados)
    constexpr float kHorMin   = std::bit_cast<float>(0xBDFA35DDu); // [0x871078] (-7 grados)
    constexpr float kHorMinM3 = std::bit_cast<float>(0xBD567750u); // [0x87107C] (-3 grados)
    constexpr float kVerStep  = std::bit_cast<float>(0x3C75C28Fu); // [0x87106C] (0.015)
    constexpr float kHorStep  = std::bit_cast<float>(0x3BA3D70Au); // [0x871070] (0.005)

    if (m_nModelIndex == MODEL_RCTIGER && GetStatus() == STATUS_REMOTE_CONTROLLED) {
        if (CPad::GetPad()->CarGunJustDown()) {
            if (CTimer::GetTimeInMS() > m_nGunFiringTime + TIGER_GUNFIRE_RATE) {
                CWeapon minigun(WEAPON_MINIGUN, 5000);
                // Como el original: 0x6AE8BC..0x6AE90E transforma TIGER_GUN_POS con la matriz y DESPUES suma timestep * velocidad (en mundo)
                const float stepX = CTimer::GetTimeStep() * m_vecMoveSpeed.x;
                const float stepY = CTimer::GetTimeStep() * m_vecMoveSpeed.y;
                const float stepZ = CTimer::GetTimeStep() * m_vecMoveSpeed.z;
                const CVector transformed = G1MultiplyMatrixWithVector(*m_matrix, TIGER_GUN_POS); // Como el original: 0x6AE90E call 0x59C890 (orden de sumas del EXE)
                CVector point{ stepX + transformed.x, stepY + transformed.y, stepZ + transformed.z };
                minigun.FireInstantHit(this, &point, &point, nullptr, nullptr, nullptr, false, true);
                CVector2D direction(0.0f, 0.1f);
                minigun.AddGunshell(this, point, direction, 0.025f);
                AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE, WEAPON_MINIGUN, this);
                m_nGunFiringTime = CTimer::GetTimeInMS();
            }
        }
        return;
    }

    if (GetStatus() != STATUS_PLAYER || m_nModelIndex != MODEL_RHINO)
        return;

    // Como el original: 0x6AE9D3 cmp byte [0x96A8B0],0 / jne -> sale si CGameLogic::GameState != 0 (GAMELOGIC_STATE_PLAYING)
    if (CGameLogic::GameState != GAMELOGIC_STATE_PLAYING || !m_pDriver || !m_pDriver->IsPlayer())
        return;

    CPad* pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    if (!pad)
        return;

    CCam& activeCam = CCamera::GetActiveCamera();
    if (activeCam.m_nMode != MODE_CAM_ON_A_STRING) {
        // Como el original: 0x6AEB8D..0x6AEBE5 (fild; fmul ts; fmul 0.015; fmul 1/128; fsubr/fadd)
        const float lr = (float)pad->GetCarGunLeftRight();
        m_fDoomVerticalRotation = m_fDoomVerticalRotation - lr * CTimer::GetTimeStep() * kVerStep * 0.0078125f;
        const float ud = (float)pad->GetCarGunUpDown();
        m_fDoomHorizontalRotation = ud * CTimer::GetTimeStep() * kHorStep * 0.0078125f + m_fDoomHorizontalRotation;
    } else {
        // Como el original: 0x6AEA53 call 0x59C810 (Multiply3x3 vector * matriz, con su orden de sumas)
        CVector frontDot = TankFireMultiplyVectorMatrix59C810(activeCam.m_vecFront, GetMatrix());
        // Como el original: fpatan en precision extendida (0x6AEA6F/0x6AEA97); la suma de 15 grados se redondea una sola vez a float
        double doomVerticalRotation  = std::atan2((double)-frontDot.x, (double)frontDot.y);
        const float mag2D            = std::sqrt(frontDot.x * frontDot.x + frontDot.y * frontDot.y);
        const float doomHorizontalRotation = (float)(std::atan2((double)frontDot.z, (double)mag2D) + (double)kDeg15);

        // Como el original: 0x6AEAAF..0x6AEADF compara con (rot + PI) y (rot - PI) calculados como float; el ajuste se redondea a float
        if (doomVerticalRotation > (double)(m_fDoomVerticalRotation + kPi))
            doomVerticalRotation = (double)(float)(doomVerticalRotation - (double)kTwoPi);
        else if (doomVerticalRotation < (double)(m_fDoomVerticalRotation - kPi))
            doomVerticalRotation = (double)(float)(doomVerticalRotation + (double)kTwoPi);

        const float doomVerticalRotDiff = (float)(doomVerticalRotation - (double)m_fDoomVerticalRotation);
        float timeStep = CTimer::GetTimeStep() * kVerStep;
        if (doomVerticalRotDiff > timeStep)
            m_fDoomVerticalRotation = timeStep + m_fDoomVerticalRotation;
        else if (doomVerticalRotDiff < -timeStep)
            m_fDoomVerticalRotation = m_fDoomVerticalRotation - timeStep;
        else
            m_fDoomVerticalRotation = (float)doomVerticalRotation; // Como el original: 0x6AEB1A..0x6AEB2F (jp -> fstp [esi+0x94C])

        const float doomHorizontalRotDiff = doomHorizontalRotation - m_fDoomHorizontalRotation;
        timeStep = CTimer::GetTimeStep() * kHorStep;
        if (doomHorizontalRotDiff > timeStep)
            m_fDoomHorizontalRotation = timeStep + m_fDoomHorizontalRotation;
        else if (doomHorizontalRotDiff < -timeStep) // Como el original: 0x6AEB6E test ah,5 / jp -> con NaN (desordenado) se asigna el objetivo
            m_fDoomHorizontalRotation = m_fDoomHorizontalRotation - timeStep;
        else
            m_fDoomHorizontalRotation = doomHorizontalRotation;
    }

    // Como el original: 0x6AEBEB..0x6AEC25 (rot < -PI -> += 2PI; rot > PI -> -= 2PI)
    if (m_fDoomVerticalRotation < -kPi) {
        m_fDoomVerticalRotation = m_fDoomVerticalRotation + kTwoPi;
    } else if (m_fDoomVerticalRotation > kPi) {
        m_fDoomVerticalRotation = m_fDoomVerticalRotation - kTwoPi;
    }

    if (m_fDoomHorizontalRotation > kHorMax) {
        m_fDoomHorizontalRotation = kHorMax;
    }
    else if (m_fDoomVerticalRotation > kHalfPi || m_fDoomVerticalRotation < -kHalfPi) {
        // Como el original: 0x6AEC95..0x6AECDD: x = max(-1.0f, (float)(fcos(rot) * 1.3f)); horRot = -7deg - (-3deg - -7deg) * x
        const float cosScaled = (float)(std::cos((double)m_fDoomVerticalRotation) * (double)1.3f);
        const float x = (-1.0f > cosScaled) ? -1.0f : cosScaled; // 0x420800 (fcomp; test ah,41h; jne): devuelve a solo si a > b; con NaN devuelve b
        const float doomHorizontalRot = kHorMin - (kHorMinM3 - kHorMin) * x;
        if (m_fDoomHorizontalRotation < doomHorizontalRot)
            m_fDoomHorizontalRotation = doomHorizontalRot;
    }
    else if (m_fDoomHorizontalRotation < kHorMin) {
        m_fDoomHorizontalRotation = kHorMin;
    }

    if (pad->CarGunJustDown()) { // TODO: Denest
        CPlayerInfo& playerInfo = FindPlayerInfo();
        if (CTimer::GetTimeInMS() > playerInfo.m_nLastTimeBigGunFired + 800) {
            playerInfo.m_nLastTimeBigGunFired = CTimer::GetTimeInMS();
            CVector point;
            point.x = (float)std::sin((double)-m_fDoomVerticalRotation);
            point.y = (float)std::cos((double)m_fDoomVerticalRotation);
            point.z = (float)std::sin((double)m_fDoomHorizontalRotation);
            point = G1Multiply3x3(GetMatrix(), point); // Como el original: 0x6AED52 call 0x59C790 (orden de sumas del EXE)

            CVector newTurretPosition;
            if (m_aCarNodes[CAR_MISC_C]) { // 0x6AED5D
                RwMatrix* carNodeMiscMatrix = RwFrameGetLTM(m_aCarNodes[CAR_MISC_C]);
                newTurretPosition = *RwMatrixGetPos(carNodeMiscMatrix);
                newTurretPosition += GetSpeed(newTurretPosition - GetPosition()) * CTimer::GetTimeStep();
            }
            else {
                // Como el original: 0x6AEE2B..0x6AEEA5. sin/cos en precision extendida; los productos se redondean a float;
                // x = (Kx*cos - sin*Ky) + Px ; y = Py + (cos*Ky + sin*Kx) ; z = Pz + (Kz - 1.0f)
                const double sinD = std::sin((double)m_fDoomVerticalRotation);
                const double cosD = std::cos((double)m_fDoomVerticalRotation);
                const float kx = TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET.x;
                const float ky = TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET.y;
                const float kz = TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET.z;
                const float kxCos = (float)((double)kx * cosD);
                const float sinKy = (float)(sinD * (double)ky);
                const float cosKy = (float)(cosD * (double)ky);
                const float sinKx = (float)(sinD * (double)kx);
                const float xPrime = kxCos - sinKy;
                const float yPrime = cosKy + sinKx;
                const float zPrime = kz - 1.0f;

                CVector doomOffset;
                doomOffset.x = xPrime + TANK_SHOT_DOOM_POS.x;
                doomOffset.y = TANK_SHOT_DOOM_POS.y + yPrime;
                doomOffset.z = TANK_SHOT_DOOM_POS.z + zPrime;

                // NOTSA: el EXE (0x6AEEA5 call 0x59C890) escribe el resultado en un temporal descartado y deja newTurretPosition SIN
                // inicializar (basura de pila). Se conserva la correccion previa y el banco no recorre esta rama.
                newTurretPosition = m_matrix->TransformPoint(doomOffset);
            }

            CVector distance = newTurretPosition - GetPosition();
            // Como el original: 0x6AEEC7..0x6AEF37: escalar = (-0.1f * masa) y despues point * escalar
            const float forceScale = std::bit_cast<float>(0xBDCCCCCDu) * m_fMass;
            CVector force = point * forceScale;
            ApplyForce(force, distance, true);
            CVector endPoint = newTurretPosition + point * 60.0f;
            CWeapon::DoTankDoomAiming(this, m_pDriver, &newTurretPosition, &endPoint);

            CColPoint colPoint{};
            CEntity* entity = nullptr;
            CWorld::pIgnoreEntity = this;
            CWorld::ProcessLineOfSight(
                newTurretPosition,
                endPoint,
                colPoint,
                entity,
                true,
                true,
                true,
                true,
                true,
                true,
                false,
                false
            );
            CWorld::pIgnoreEntity = nullptr;
            CVector explosionPos = endPoint;
            if (entity)
                explosionPos = colPoint.m_vecPoint - (colPoint.m_vecPoint - newTurretPosition) * 0.04f;
            CExplosion::AddExplosion(nullptr, FindPlayerPed(), EXPLOSION_TANK_FIRE, explosionPos, 0, true, -1.0f, false);
            CVector target = explosionPos - newTurretPosition;
            g_fx.TriggerTankFire(newTurretPosition, target);
        }
    }
}

// 0x6AF110
void CAutomobile::BlowUpCarsInPath() {
    // Como el original: 0x6AF12C fsqrt; 0x6AF134 fcomp [0x858B1C] (0.1f); test ah,41h; jne -> sale si !(Magnitude() > 0.1f)
    if (!(m_vecMoveSpeed.Magnitude() > 0.1f)) {
        return;
    }

    // Como el original: 0x6AF145 test byte [edi+0x868],0x20; je -> sale si el bit bTankExplodesCars NO esta activo
    if (!autoFlags.bTankExplodesCars) {
        return;
    }

    for (auto&& entity : GetCollidingEntities()) {
        if (!entity)
            continue; // I don't think should ever happen? But original code checks...
        if (!entity->GetIsTypeVehicle())
            continue;

        auto& veh = *entity->AsVehicle();
        if (ModelIndices::IsRhino(veh.m_nModelIndex))
            continue;
        if (veh.physicalFlags.bRenderScorched)
            continue;

        if (this == FindPlayerVehicle()) {
            CCrime::ReportCrime(eCrimeType::CRIME_EXPLOSION, &veh, FindPlayerPed());
        }
        veh.BlowUpCar(this, false);
    }
}

// 0x6AF420
void CAutomobile::PlaceOnRoadProperly()
{
    auto* cm = GetColModel();
    const auto fStartY =  cm->m_pColData->m_pLines[0].m_vecStart.y;
    const auto fEndY   = -cm->m_pColData->m_pLines[3].m_vecStart.y;

    const auto& vecPos = GetPosition();

    auto vecRearCheck = vecPos - GetForward() * fEndY;
    vecRearCheck.z = vecPos.z;

    auto vecFrontCheck = vecPos + GetForward() * fStartY;
    vecFrontCheck.z = vecPos.z;

    // Como el original: 0x6AF4DE..0x6AF734 (delantero) y 0x6AF607..0x6AF734 (trasero): igual tratamiento para ambos extremos.
    // Despues de la consulta hacia abajo SIEMPRE se aplica (iluminacion + z) si hubo algun impacto, tambien cuando se conserva el primero.
    const auto ProcessEnd = [this](CVector& check, CStoredCollPoly& poly) {
        bool bColFound = false;
        CColPoint colPoint{};
        CEntity* colEntity{};
        float fColZ{};
        const auto Take = [&] {
            m_bTunnel = colEntity->m_bTunnel;
            m_bTunnelTransition = colEntity->m_bTunnelTransition;

            fColZ = colPoint.m_vecPoint.z;
            m_pEntityWeAreOn = colEntity;
        };
        const auto Apply = [&] {
            // Como el original: 0x6AF5E5 movzx ecx,byte [colPoint.lightingB]; mov [esi+0x544/0x570],ecx (escritura de 32 bits: pone a cero el relleno)
            *reinterpret_cast<uint32*>(&poly.ligthing) = (uint32)colPoint.m_nLightingB.value;
            check.z = fColZ;
        };

        if (CWorld::ProcessVerticalLine(check, check.z + 5.0F, colPoint, colEntity, true)) {
            Take();
            bColFound = true;
        }
        if (CWorld::ProcessVerticalLine(check, check.z - 5.0F, colPoint, colEntity, true)) {
            if (!bColFound || std::fabs(check.z - colPoint.m_vecPoint.z) < std::fabs(check.z - fColZ)) {
                Take();
            }
            Apply();
        } else if (bColFound) {
            Apply();
        }
    };
    ProcessEnd(vecFrontCheck, m_FrontCollPoly);
    ProcessEnd(vecRearCheck, m_RearCollPoly);

    auto fHeightAboveRoad = GetHeightAboveRoad();
    vecFrontCheck.z += fHeightAboveRoad;
    vecRearCheck.z += m_fRearHeightAboveRoad;

    auto fLength = fEndY + fStartY;
    GetRight().Set((vecFrontCheck.y - vecRearCheck.y) / fLength, -((vecFrontCheck.x - vecRearCheck.x) / fLength), 0.0F);

    auto vecDiff = vecFrontCheck - vecRearCheck;
    vecDiff.Normalise();
    GetForward() = vecDiff;

    auto vecCross = CrossProduct(GetRight(), GetForward());
    GetUp() = vecCross;

    // Como el original: 0x6AF88C..0x6AF8BA: se calcula 1.0f / fLength y cada componente se MULTIPLICA por el inverso (no se divide)
    const CVector vecSum = vecFrontCheck * fEndY + vecRearCheck * fStartY;
    const float fInvLength = 1.0f / fLength;
    CVector vecNewPos{ vecSum.x * fInvLength, vecSum.y * fInvLength, vecSum.z * fInvLength };
    SetPosn(vecNewPos);

    if (IsSubPlane())
        AsPlane()->m_planeCreationHeading = CGeneral::GetATanOfXY(GetForward().x, GetForward().y);
}

// 0x6AF910
void CAutomobile::PopBoot() {
    // Como el original (0x6AF933): GetDoorStatus(1) y comparacion con 0 o 2 (sin las aserciones de IsDoorClosed con la puerta ausente)
    if (const auto bootStatus = m_damageManager.GetDoorStatus(eDoors::DOOR_BOOT); bootStatus == DAMSTATE_OK || bootStatus == DAMSTATE_DAMAGED) {
        auto& door = m_doors[eDoors::DOOR_BOOT];

        door.OpenFully();

        CMatrix frameMat{ RwFrameGetMatrix(m_aCarNodes[eCarNodes::CAR_BOOT]) };
        G2UpdateDoorFrameMatrix(door, frameMat); // Como el original (0x6AF984..0x6AF9F0): SetRotate y suma de la posicion guardada
    }
}

// 0x6AFA20
void CAutomobile::CloseBoot() {
    auto& door = m_doors[eDoors::DOOR_BOOT];

    door.CloseFully();

    // Code copy pasted from `PopBoot`:
    CMatrix frameMat{ RwFrameGetMatrix(m_aCarNodes[eCarNodes::CAR_BOOT]) };
    G2UpdateDoorFrameMatrix(door, frameMat); // Como el original (0x6AFA5C..0x6AFAE5): SetRotate y suma de la posicion guardada
}

// 0x6B0690
void CAutomobile::DoHeliDustEffect(float timeConstMult, float fxMaxZMult) {
    const auto KillDustFx = [this] {
        if (m_pDustParticle) {
            m_pDustParticle->Kill();
            m_pDustParticle = nullptr;
            m_heliDustFxTimeConst = 0.f;
        }
    };

    const auto& myPos = GetPosition();

    // 0x6B07E9
    // Moved early out here instead
    if (DistanceBetweenPointsSquared(TheCamera.GetPosition(), myPos) >= sq(50.f)) {
        KillDustFx();
        return;
    }

    switch (m_nVehicleSubType) {
    case eVehicleType::VEHICLE_TYPE_HELI: {
        if (m_fHeliRotorSpeed < 0.1125f) {
            KillDustFx();
            return;
        }
        break;
    }
    case eVehicleType::VEHICLE_TYPE_PLANE: {
        break;
    }
    default: {
        KillDustFx();
        return;
    }
    }

    // 0x6B0705

    // Figure out ground position, and if it's underwater

    CColPoint groundCP{};
    CEntity* hitEntity{}; // Unused
    CWorld::ProcessVerticalLine(myPos, -1000.f, groundCP, hitEntity, true); // TODO: Check if it returned true :D
    const bool isGroundSand = g_surfaceInfos.IsSand(groundCP.m_nSurfaceTypeB);

    auto waterLevel{-1000.f};
    const bool isThereWaterUnderUs = CWaterLevel::GetWaterLevel(myPos, waterLevel, false);
    const auto isHitPosUnderWater = isThereWaterUnderUs && waterLevel > groundCP.m_vecPoint.z;

    const auto groundZ = isHitPosUnderWater ? waterLevel : groundCP.m_vecPoint.z;
    const auto distToGroundZ = myPos.z - groundZ;

    // Calculate max height for fx to be played
    const auto GetBaseMaxZForFx = [this]{
        switch (m_nModelIndex) {
        case eModelID::MODEL_RCGOBLIN:
        case eModelID::MODEL_RCRAIDER:
            return 3.f;
        }
        return 30.f;
    };
    const auto maxZForFx = GetBaseMaxZForFx() * fxMaxZMult;

    // Check if we're flying low enough for the fx to be played. If not, kill it, and return.
    if (distToGroundZ >= maxZForFx) {
        KillDustFx();
        return;
    }

    // Early out moved to top

    // Add danger event if we're flying too low to the ground
    if (CTimer::GetFrameCounter() % 20 == 0) {
        const auto dangerRadius = m_nModelIndex == MODEL_HYDRA ? 30.f : 20.f;
        if (distToGroundZ < dangerRadius) {
            CEventDanger danger{this, dangerRadius};
            GetEventGlobalGroup()->Add(&danger);
        }
    }

    // 0x6B08B1
    // Create and play fx if it doesn't exist.
    if (!m_pDustParticle) {
        CVector fxPos{0.f, 0.f, 0.f};
        m_pDustParticle = g_fxMan.CreateFxSystem("heli_dust", CVector{0.f, 0.f, 0.f}, nullptr, true);
        if (!m_pDustParticle) { // Failed, return
            return;
        }
        m_pDustParticle->SetLocalParticles(true);
        m_pDustParticle->Play();
    }

    // Update time constant of fx
    {
        const auto heightProgress = (maxZForFx - distToGroundZ) / maxZForFx * timeConstMult;
        const auto heightProgressSq = heightProgress * heightProgress;

        // TODO: Figure out a better way to do this... Very ugly.
        // Basically it's stepping `m_heliDustFxTimeConst` by 0.04 towards `heightProgressSq`.
        if (m_heliDustFxTimeConst < heightProgressSq) {
            m_heliDustFxTimeConst = std::min(m_heliDustFxTimeConst + 0.04f, heightProgressSq);
        } else if (m_heliDustFxTimeConst > heightProgressSq) { // Must check if less, because they might be equal (tbh, the chance for that to happen are... small)
            m_heliDustFxTimeConst = std::max(m_heliDustFxTimeConst - 0.04f, heightProgressSq);
        }

        m_pDustParticle->SetConstTime(true, m_heliDustFxTimeConst);
    }

    // Update max of fx
    m_pDustParticle->SetOffsetPos({ myPos.x, myPos.y, groundZ });

    // Finally enable/disable prims (whatever that is)
    m_pDustParticle->EnablePrim(0, !isHitPosUnderWater && !isGroundSand); // Dust or similar particles?
    m_pDustParticle->EnablePrim(1, !isHitPosUnderWater && isGroundSand);  // Sand particles?
    m_pDustParticle->EnablePrim(2, isHitPosUnderWater);                   // Water particles?

    // Above code originally (kinda):
    //if (isHitPosUnderWater) {
    //    m_pDustParticle->EnablePrim(0, false);
    //    m_pDustParticle->EnablePrim(1, false);
    //    m_pDustParticle->EnablePrim(2, true);
    //} else {
    //    m_pDustParticle->EnablePrim(0, !isGroundSand);
    //    m_pDustParticle->EnablePrim(1, isGroundSand);
    //    m_pDustParticle->EnablePrim(2, false);
    //}
}

// 0x6B1350
void CAutomobile::SetBumperDamage(ePanels panelIdx, bool withoutVisualEffect) {
    auto nodeIdx = CDamageManager::GetCarNodeIndexFromPanel(panelIdx);
    auto frame = m_aCarNodes[nodeIdx];
    if (!frame) {
        NOTSA_LOG_WARN("Trying to damage component {} of {}", (int32)nodeIdx, CModelInfo::GetModelInfo(m_nModelIndex)->GetModelNameAsString()); // R* log
        return;
    }

    if (!GetVehicleModelInfo()->m_pVehicleStruct->IsComponentDamageable(nodeIdx)) {
        return;
    }

    switch (m_damageManager.GetPanelStatus(panelIdx)) {
    case ePanelDamageState::DAMSTATE_DAMAGED: {
        if (!m_pHandlingData->m_bBouncePanels) { // TODO: Weird... The flag name might be incorrect, because here we actually set the bouncing panelIdx.
            if (auto* panelFrame = CheckIfExistsGetFree(nodeIdx)) {
                // Como el original (0x6B1540..0x6B1563): GetRandomNumberInRange(-0.2f, -0.5f) (extremos en ese orden, 0x41BD90)
                panelFrame->SetPanel(nodeIdx, 0, CGeneral::GetRandomNumberInRange(-0.2f, -0.5f));
            }
        }
        break;
    }
    case ePanelDamageState::DAMSTATE_OPENED: {
        SetComponentVisibility(frame, eAtomicComponentFlag::ATOMIC_DAMAGED);
        break;
    }
    case ePanelDamageState::DAMSTATE_OPENED_DAMAGED: {
        if (!withoutVisualEffect) {
            SpawnFlyingComponent(nodeIdx, 0u);
        }
        // Como el original (0x6B145D): el marco se vuelve a leer de m_aCarNodes tras SpawnFlyingComponent
        SetComponentVisibility(m_aCarNodes[nodeIdx], eAtomicComponentFlag::ATOMIC_NONE);
        break;
    }
    }
}

// 0x6B1480
void CAutomobile::SetPanelDamage(ePanels panel, bool createWindowGlass)
{
    auto nodeIdx = CDamageManager::GetCarNodeIndexFromPanel(panel);
    auto frame = m_aCarNodes[nodeIdx];
    if (!frame) {
        return;
    }

    if (!GetVehicleModelInfo()->m_pVehicleStruct->IsComponentDamageable(nodeIdx)) {
        return;
    }

    switch (m_damageManager.GetPanelStatus(panel)) {
    case ePanelDamageState::DAMSTATE_DAMAGED: {
        if (m_pHandlingData->m_bBouncePanels) { // TODO: Weird... The flag name might be incorrect, because here we actually set the bouncing panelIdx.
            return;
        }
        if (auto* panel_ = CheckIfExistsGetFree(nodeIdx)) {
            switch (nodeIdx) {
            case eCarNodes::CAR_WINDSCREEN:
            case eCarNodes::CAR_WING_LF:
            case eCarNodes::CAR_WING_RF:
                break;
            default: {
                panel_->SetPanel(nodeIdx, 1, CGeneral::GetRandomNumberInRange(-0.2f, -0.5f));
                break;
            }
            }
        }
        SetComponentVisibility(frame, eAtomicComponentFlag::ATOMIC_DAMAGED);
        break;
    }
    case ePanelDamageState::DAMSTATE_OPENED: {
        if (panel == ePanels::WINDSCREEN_PANEL) {
            m_vehicleAudio.AddAudioEvent(eAudioEvents::AE_WINDSCREEN_SHATTER, 0.f);
        }
        SetComponentVisibility(frame, eAtomicComponentFlag::ATOMIC_DAMAGED);
        break;
    }
    case ePanelDamageState::DAMSTATE_OPENED_DAMAGED: {
        if (createWindowGlass) {
            if (panel == ePanels::WINDSCREEN_PANEL) {
                CGlass::CarWindscreenShatters(this);
            }
        } else {
            SpawnFlyingComponent(nodeIdx, 5u);
        }
        // Como el original (0x6B15DA): el marco se vuelve a leer de m_aCarNodes tras SpawnFlyingComponent / CarWindscreenShatters
        SetComponentVisibility(m_aCarNodes[nodeIdx], eAtomicComponentFlag::ATOMIC_NONE);
        break;
    }
    }
}

// 0x6B1600
void CAutomobile::SetDoorDamage(eDoors doorIdx, bool withoutVisualEffect)
{
    auto nodeIdx = CDamageManager::GetCarNodeIndexFromDoor(doorIdx);
    auto frame = m_aCarNodes[nodeIdx];
    if (!frame) {
        NOTSA_LOG_WARN("Trying to damage component {} of {}", (int32)nodeIdx, CModelInfo::GetModelInfo(m_nModelIndex)->GetModelNameAsString()); // R* log
        return;
    }

    const auto isDamageable = GetVehicleModelInfo()->m_pVehicleStruct->IsComponentDamageable(nodeIdx);

    auto& door{ m_doors[doorIdx] };

    // Leaving this here for further reference...
    // The `if` @ `0x6B1650` is inverted here a little, because
    // `0x6B169C` is only reachable if `eDoors::DOOR_BOOT` in which all other if's are ignored (that is the one at `0x6B1650` and `0x6B1673`)
    // If the doorIdx isn't BOOT, but is BONNET just ignore it, because of `0x6B1660`
    // Now, if it's neither, then we go on and check the logical invert of the 2 other conditions at `0x6B1650`
    // If those are all true we will rtn up @ `0x6B1673`

    switch (doorIdx) {
    case eDoors::DOOR_BOOT: {
        // 0x6B168E
        // Como el original: test al,al / jns sobre el byte 0xCC del handling = bit 7 de las banderas de modelo (m_bNoswingBoot);
        // el estado se calcula en una variable local (bl) y SetDoorStatus(BOOT, bl) se llama siempre, tambien con el mismo estado
        if (m_pHandlingData->m_bNoswingBoot) {
            auto bootStatus = m_damageManager.GetDoorStatus(eDoors::DOOR_BOOT);
            switch (bootStatus) {
            case eDoorStatus::DAMSTATE_OPENED: {
                bootStatus = isDamageable ? eDoorStatus::DAMSTATE_DAMAGED : eDoorStatus::DAMSTATE_NOTPRESENT;
                break;
            }
            case eDoorStatus::DAMSTATE_OPENED_DAMAGED: {
                bootStatus = eDoorStatus::DAMSTATE_NOTPRESENT;
                break;
            }
            default:
                break;
            }
            m_damageManager.SetDoorStatus(eDoors::DOOR_BOOT, bootStatus);
        }
        break;
    }
    case eDoors::DOOR_BONNET: // Check from 0x6B1660
        break;

    default: {
        // Inverted `if` from `0x6B1650`, this way we get to the `else` branch at `0x6B1667`
        if (!CanDoorsBeDamaged()) {
            switch (m_damageManager.GetDoorStatus(doorIdx)) { // 0x6B1659
            case eDoorStatus::DAMSTATE_OPENED_DAMAGED:
            case eDoorStatus::DAMSTATE_NOTPRESENT: {
                // 0x6B1667
                door.Open(0.f);
                m_damageManager.SetDoorStatus(doorIdx, eDoorStatus::DAMSTATE_DAMAGED);
                return;
            }
            }
        }
        break;
    }
    }

    // 0x6B16E5
    switch (m_damageManager.GetDoorStatus(doorIdx)) {
    case DAMSTATE_DAMAGED: { // 0x6B16E5
        if (isDamageable) {
            SetComponentVisibility(frame, eAtomicComponentFlag::ATOMIC_DAMAGED);
        }

        if (door.m_prevAngle != 0.f) {
            door.m_angle = 0.f;
            door.m_prevAngle = 0.f;
            door.m_angVel = 0.f;

            // Reset component rotation
            {
                // Como el original (0x6B1759..0x6B17B9): SetRotate(0, 0, 0) y suma de la posicion guardada
                CMatrix frameMatrix{RwFrameGetMatrix(frame)};
                G2SetRotateAddPos(frameMatrix, CVector{ 0.f, 0.f, 0.f });
                frameMatrix.UpdateRW();
            }

            m_vehicleAudio.AddAudioEvent((eAudioEvents)((int32)AE_CAR_BONNET_CLOSE + (int32)doorIdx), 0.f);
        }
        break;
    }
    case DAMSTATE_NOTPRESENT: { // 0x6B17F0
        if (!withoutVisualEffect) {
            if (doorIdx == eDoors::DOOR_BONNET) { // Inverted
                const auto obj = SpawnFlyingComponent(nodeIdx, 3u)->AsVehicle();
                m_vehicleAudio.AddAudioEvent(AE_BONNET_FLUBBER_FLUBBER, obj);
            } else {
                SpawnFlyingComponent(nodeIdx, doorIdx == eDoors::DOOR_BOOT ? 4u : 2u);
            }
        }
        // Como el original (0x6B1821): el marco se vuelve a leer de m_aCarNodes tras SpawnFlyingComponent
        SetComponentVisibility(m_aCarNodes[nodeIdx], eAtomicComponentFlag::ATOMIC_NONE);
        break;
    }
    case DAMSTATE_OPENED: // 0x6B1834
    case DAMSTATE_OPENED_DAMAGED: {
        RwFrameForAllObjects(frame, CVehicleModelInfo::SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_DONT_CULL);
        if (doorIdx == eDoors::DOOR_BONNET) {
            door.m_angVel = 0.2f;
        }
        break;
    }
    }
}

// 0x6B3F70
bool CAutomobile::RcbanditCheck1CarWheels(CPtrListDoubleLink<CVehicle*>& ptrList)
{
    CColModel* colModel = GetVehicleModelInfo()->GetColModel();

    for (auto* const vehicle : ptrList) {
        if (vehicle == this || !vehicle->IsAutomobile())
            continue;

        // Como el original: 0x6B4003 cmp bp,1B9h / je (los RC Bandit se saltan) y 0x6B400E cmp [ebx+0x2C],ax / je (ya visitado);
        // 0x6B4018 mov [ebx+0x2C],ax marca el vehiculo como visitado
        if (ModelIndices::IsRCBandit(vehicle->m_nModelIndex) || vehicle->IsScanCodeCurrent())
            continue;
        vehicle->SetCurrentScanCode();

        // Como el original: 0x6B403B..0x6B40F7 |dx| < 10.0f y |dy| < 10.0f (antes sin valor absoluto)
        CVector distance = GetPosition() - vehicle->GetPosition();
        if (std::fabs(distance.x) < 10.0f && std::fabs(distance.y) < 10.0f) {
            auto modelInfo = vehicle->GetVehicleModelInfo();
            for (int32 i = 0; i < 4; i++) {
                // Como el original: 0x6B4110 lee la compresion previa del OTRO vehiculo ([ebx+0x7E4]) y su estado (cmp al,10h)
                if (!(vehicle->AsAutomobile()->m_fWheelsSuspensionCompressionPrev[i] < 1.0f) && vehicle->GetStatus() != STATUS_SIMPLE)
                    continue;

                CVector wheelPos;
                modelInfo->GetWheelPosn(i, wheelPos, false);

                // Como el original: 0x6B4149 Invert(src, estatica 0xC1C220) (0x59B920) y dos 0x59C890. Se reproduce el orden exacto de sumas del EXE
                // (el TransformPoint de cabecera suma x,y,z y la inversa de CMatrix usa otro orden: diferencias de varios ulp).
                const float* s = reinterpret_cast<const float*>(m_matrix);
                float d[16] = {};
                d[0] = s[0]; d[1] = s[4]; d[2] = s[8];
                d[4] = s[1]; d[5] = s[5]; d[6] = s[9];
                d[8] = s[2]; d[9] = s[6]; d[10] = s[10];
                {
                    float tx = s[12] * d[0], ty = d[1] * s[12], tz = d[2] * s[12];
                    tx = (s[13] * d[4]) + tx; ty = (d[5] * s[13]) + ty; tz = (d[6] * s[13]) + tz;
                    tx = (d[8] * s[14]) + tx; ty = (s[14] * d[9]) + ty; tz = (d[10] * s[14]) + tz;
                    d[12] = tx * -1.0f; d[13] = ty * -1.0f; d[14] = tz * -1.0f;
                }
                const auto ExeTransform = [](const float* m, float x, float y, float z) {
                    return CVector{
                        ((m[8] * z + m[4] * y) + m[0] * x) + m[12],
                        ((m[9] * z + m[1] * x) + m[5] * y) + m[13],
                        ((m[10] * z + m[2] * x) + m[6] * y) + m[14],
                    };
                };
                const CVector worldWheel = ExeTransform(reinterpret_cast<const float*>(vehicle->m_matrix), wheelPos.x, wheelPos.y, wheelPos.z);

                CColSphere sphere;
                sphere.m_vecCenter = ExeTransform(d, worldWheel.x, worldWheel.y, worldWheel.z);
                sphere.m_fRadius = modelInfo->m_fWheelSizeFront * 0.25f;
                if (CCollision::TestSphereBox(sphere, colModel->m_boundBox))
                    return true;
            }
        }
    }
    return false;
}

// 0x6B45E0
bool CAutomobile::RcbanditCheckHitWheels() {
    const auto& point = GetPosition();
    float fRadius = 2.0f; // [0x858CA0]
    const float minX = point.x - fRadius;
    const float maxX = point.x + fRadius;
    const float minY = point.y - fRadius;
    const float maxY = point.y + fRadius;
    // Como el original: 0x6B464F..0x6B4766: sector = (int)floor(v * 0.02f + 60.0f) ([0x858B38], [0x858B34]); (antes se dividia entre 50)
    const auto SectorOf = [](float v) { return static_cast<int32>(std::floor(v * 0.02f + 60.0f)); };
    int32 startSectorX = std::max(SectorOf(minX), 0);
    int32 startSectorY = std::max(SectorOf(minY), 0);
    int32 endSectorX = std::min(SectorOf(maxX), MAX_SECTORS_X - 1);
    int32 endSectorY = std::min(SectorOf(maxY), MAX_SECTORS_Y - 1);
    CWorld::AdvanceCurrentScanCode();
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            auto& repeatSector = CWorld::GetRepeatSector(sectorX, sectorY);
            // Como el original: 0x6B47CF test ax,ax / jne 0x6B47F1 -> devuelve true al primer acierto; si no, 0x6B47E6 devuelve false
            if (RcbanditCheck1CarWheels(repeatSector.Vehicles))
                return true;
        }
    }
    return false;
}

// 0x729B60
void CAutomobile::FireTruckControl(CFire* fire) {
    // Como el original: constantes leidas del EXE (el PI/TWO_PI/DegreesToRadians de common.h son imprecisos)
    constexpr float kPi      = std::bit_cast<float>(0x40490FDBu); // [0x858CB8]
    constexpr float kTwoPi   = std::bit_cast<float>(0x40C90FDBu); // [0x858CBC]
    constexpr float kDeg15   = std::bit_cast<float>(0x3E860A92u); // [0x872A38]
    constexpr float kDeg22   = std::bit_cast<float>(0x3EC49809u); // [0x872A44]
    constexpr float kStepV   = std::bit_cast<float>(0x3D4CCCCDu); // [0x872A48] (0.05)
    constexpr float kStepH   = std::bit_cast<float>(0x3CA3D70Au); // [0x872A4C] (0.02)
    constexpr float kStepAI  = std::bit_cast<float>(0x3C23D70Au); // [0x858C58] (0.01)
    constexpr float kHorHiNormal = std::bit_cast<float>(0x3EB2B8C3u); // [0x872A30]
    constexpr float kHorLoNormal = std::bit_cast<float>(0xBEB2B8C3u); // [0x872A34]
    constexpr float kHorHiSwat   = std::bit_cast<float>(0x3F1C61AAu); // [0x872A3C]
    constexpr float kHorLoSwat   = std::bit_cast<float>(0xBE32B8C3u); // [0x872A40]
    const bool isSwat = m_nModelIndex == MODEL_SWATVAN; // cmp word [esi+0x22], 0x259

    if (this != FindPlayerVehicle()) {
        if (GetStatus() != STATUS_PHYSICS || !fire)
            return;

        // Como el original: 0x729E6B..0x729F5E. Las distancias se restan como float; fpatan en precision extendida
        const CVector& selfPos = GetPosition();
        const float dx = fire->GetPosition().x - selfPos.x;
        const float dy = fire->GetPosition().y - selfPos.y;
        double shootDir = std::atan2((double)-dx, (double)dy);
        double heading = m_matrix ? std::atan2((double)-m_matrix->GetForward().x, (double)m_matrix->GetForward().y) : (double)m_placement.m_fHeading;
        if (shootDir > (double)(float)(heading + (double)kPi))
            shootDir = (double)(float)(shootDir - (double)kTwoPi);
        else if (shootDir < (double)(float)(heading - (double)kPi))
            shootDir = (double)(float)(shootDir + (double)kTwoPi);

        float doomVerticalRotation = (float)(shootDir - heading);
        if (doomVerticalRotation > m_fDoomVerticalRotation + kPi)
            doomVerticalRotation = doomVerticalRotation - kTwoPi;
        else if (doomVerticalRotation < m_fDoomVerticalRotation - kPi)
            doomVerticalRotation = doomVerticalRotation + kTwoPi;

        const float doomVerticalRotDiff = doomVerticalRotation - m_fDoomVerticalRotation;
        const float timeStep = CTimer::GetTimeStep() * kStepAI;
        // Como el original: 0x729F28..0x729F5E: si |diff| < paso, se asigna directamente doomVerticalRotation
        if (std::fabs(doomVerticalRotDiff) < timeStep) {
            m_fDoomVerticalRotation = doomVerticalRotation;
        } else if (!(doomVerticalRotDiff > 0.0f)) { // Como el original: 0x729F43 test ah,41h / jne -> con diff <= 0 o NaN se resta
            m_fDoomVerticalRotation = m_fDoomVerticalRotation - timeStep;
        } else {
            m_fDoomVerticalRotation = timeStep + m_fDoomVerticalRotation;
        }
        // Como el original: 0x729F64..0x729F90: sin((time & 0xFFF) * 0.0015332032f) * 0.15f
        const float angle = (float)(int32)(CTimer::GetTimeInMS() & 0xFFFu) * std::bit_cast<float>(0x3AC8F5C3u);
        m_fDoomHorizontalRotation = (float)(std::sin((double)angle) * (double)0.15000000596046448f);
    }
    else {
        CCam& activeCam = CCamera::GetActiveCamera();
        if (activeCam.m_nMode != MODE_CAM_ON_A_STRING)
        {
            CPad* pad = CPad::GetPad();
            // Como el original: 0x729D1B..0x729D7F (fild; fmul ts; fmul 0.05f; fmul 1/128; fsubr/fadd)
            m_fDoomVerticalRotation = m_fDoomVerticalRotation - (float)pad->GetCarGunLeftRight() * CTimer::GetTimeStep() * kStepV * 0.0078125f;
            m_fDoomHorizontalRotation = (float)pad->GetCarGunUpDown() * CTimer::GetTimeStep() * kStepH * 0.0078125f + m_fDoomHorizontalRotation;
        }
        else {
            // Como el original: 0x729BC4 call 0x59C810 (Multiply3x3 vector * matriz, con su orden de sumas)
            CVector frontDot = TankFireMultiplyVectorMatrix59C810(activeCam.m_vecFront, GetMatrix());
            double doomVerticalRotation = std::atan2((double)-frontDot.x, (double)frontDot.y);
            const float mag2D = std::sqrt(frontDot.x * frontDot.x + frontDot.y * frontDot.y);
            const float doomHorizontalRotation = (float)(std::atan2((double)frontDot.z, (double)mag2D) + (double)(isSwat ? kDeg22 : kDeg15));

            if (doomVerticalRotation > (double)(m_fDoomVerticalRotation + kPi))
                doomVerticalRotation = (double)(float)(doomVerticalRotation - (double)kTwoPi);
            else if (doomVerticalRotation < (double)(m_fDoomVerticalRotation - kPi))
                doomVerticalRotation = (double)(float)(doomVerticalRotation + (double)kTwoPi);

            const float doomVerticalRotDiff = (float)(doomVerticalRotation - (double)m_fDoomVerticalRotation);
            float timeStep = CTimer::GetTimeStep() * kStepV;
            // Como el original: 0x729C7A..0x729CB2: si |diff| <= paso se asigna el objetivo
            if (doomVerticalRotDiff > timeStep)
                m_fDoomVerticalRotation = timeStep + m_fDoomVerticalRotation;
            else if (doomVerticalRotDiff < -timeStep)
                m_fDoomVerticalRotation = m_fDoomVerticalRotation - timeStep;
            else
                m_fDoomVerticalRotation = (float)doomVerticalRotation;

            const float doomHorizontalRotDiff = doomHorizontalRotation - m_fDoomHorizontalRotation;
            timeStep = CTimer::GetTimeStep() * kStepH;
            if (doomHorizontalRotDiff > timeStep)
                m_fDoomHorizontalRotation = timeStep + m_fDoomHorizontalRotation;
            else if (doomHorizontalRotDiff < -timeStep)
                m_fDoomHorizontalRotation = m_fDoomHorizontalRotation - timeStep;
            else
                m_fDoomHorizontalRotation = doomHorizontalRotation;
        }

        // Como el original: 0x729D85..0x729DBF (rot < -PI -> += 2PI; rot > PI -> -= 2PI)
        if (m_fDoomVerticalRotation < -kPi)
            m_fDoomVerticalRotation = m_fDoomVerticalRotation + kTwoPi;
        else if (m_fDoomVerticalRotation > kPi)
            m_fDoomVerticalRotation = m_fDoomVerticalRotation - kTwoPi;

        // Como el original: 0x729DC5..0x729E1F: limite inferior primero, luego superior
        const float horLo = isSwat ? kHorLoSwat : kHorLoNormal;
        const float horHi = isSwat ? kHorHiSwat : kHorHiNormal;
        if (horLo > m_fDoomHorizontalRotation)
            m_fDoomHorizontalRotation = horLo;
        else if (m_fDoomHorizontalRotation > horHi)
            m_fDoomHorizontalRotation = horHi;

        if (!CPad::GetPad()->GetCarGunFired())
            return;
    }

    CVector newTurretPosition;
    if (m_aCarNodes[CAR_MISC_A]) {
        RwMatrix* carNodeMiscMatrix = RwFrameGetLTM(m_aCarNodes[CAR_MISC_A]);
        newTurretPosition = *RwMatrixGetPos(carNodeMiscMatrix);
    } else {
        newTurretPosition = G1MultiplyMatrixWithVector(*m_matrix, CVector(0.0f, 1.5f, 1.8f)); // Como el original: 0x729FEC call 0x59C890
    }

    // Como el original: 0x729FF4..0x72A06F: sin/cos en precision extendida; el producto se redondea una vez a float
    const double cosH = std::cos((double)m_fDoomHorizontalRotation);
    CVector point{
        -(float)(std::sin((double)m_fDoomVerticalRotation) * cosH),
        (float)(std::cos((double)m_fDoomVerticalRotation) * cosH),
        (float)std::sin((double)m_fDoomHorizontalRotation),
    };
    point = G1Multiply3x3(GetMatrix(), point); // Como el original: 0x72A062 call 0x59C790 (orden de sumas del EXE)

    if (m_aCarNodes[CAR_MISC_A]) {
        if (isSwat)
            newTurretPosition += point * 2.0f;
        else
            newTurretPosition += point * std::bit_cast<float>(0x3F99999Au); // [0x872A50] (1.2)

        newTurretPosition += GetSpeed(newTurretPosition - GetPosition()) * CTimer::GetTimeStep();
    }

    // Como el original: 0x72A1C4..0x72A1DE: rand() & 0xF, fild, fmul [0x858CDC] (0.001f), fadd point.z
    point.z += float(CGeneral::GetRandomNumber() % 16) * std::bit_cast<float>(0x3A83126Fu);
    CVector endPoint = m_vecMoveSpeed * CVector(1.0f, 1.0f, 0.3f);
    if (isSwat)
        endPoint += point * std::bit_cast<float>(0x3ECCCCCDu); // [0x872A5C] (0.4)
    else
        endPoint += point * 0.5f;
    CWaterCannons::UpdateOne((uint32)this, &newTurretPosition, &endPoint);
}

// 0x44D520
bool CAutomobile::HasCarStoppedBecauseOfLight() {
    // Como el original (0x44D520..0x44D77C): solo aplica con estado SIMPLE (2) o PHYSICS (3) (byte +0x36 >> 3); recorre los enlaces del nodo
    // actual buscando el que lleva al nodo inicial de la ruta y despues los del nodo final buscando el que lleva al actual; devuelve true si el enlace
    // de carretera correspondiente tiene el semaforo en estado distinto de 0 (byte +0xC & 3). Sin comprobaciones de rango (como el EXE): el area se usa
    // tal cual como indice. Solo AL lo lee el llamador (0x6A382C: `test al, al`).
    const auto status = static_cast<uint8>(GetStatus());
    if (status != STATUS_SIMPLE && status != STATUS_PHYSICS) {
        return false;
    }

    auto&            paths = ThePaths;
    const CNodeAddress cur   = m_autoPilot.m_currentAddress;     // +0x390
    const CNodeAddress start = m_autoPilot.m_startingRouteNode;  // +0x394
    const CNodeAddress end   = m_autoPilot.m_endingRouteNode;    // +0x398

    // Recorre los enlaces de `from` buscando uno igual a `target` cuyo enlace de carretera (area<<10 | indice) tenga el semaforo activo
    const auto ScanLinks = [&paths](const CNodeAddress& from, const CNodeAddress& target) -> bool {
        const CPathNode* const node = &paths.m_pPathNodes[from.m_wAreaId][from.m_wNodeId];
        for (int32 i = 0; i < static_cast<int32>(node->m_nNumLinks); i++) {
            const int32 idx = static_cast<int32>(node->m_wBaseLinkId) + i;
            const CNodeAddress& link = paths.m_pNodeLinks[from.m_wAreaId][idx];
            if (link.m_wAreaId != target.m_wAreaId || link.m_wNodeId != target.m_wNodeId) {
                continue;
            }
            const uint16 naviRaw  = *reinterpret_cast<const uint16*>(&paths.m_pNaviLinks[from.m_wAreaId][idx]);
            const uint32 naviArea = naviRaw >> 10u;
            if (paths.m_pPathNodes[naviArea] == nullptr) {
                continue;
            }
            const auto* const naviNode = reinterpret_cast<const uint8*>(paths.m_pNaviNodes[naviArea]) + (naviRaw & 0x3FFu) * sizeof(CCarPathLink);
            if (naviNode[0xC] & 3) {
                return true;
            }
        }
        return false;
    };

    if (paths.m_pPathNodes[cur.m_wAreaId] == nullptr) {
        return false;
    }
    if (paths.m_pPathNodes[start.m_wAreaId] == nullptr) {
        return false;
    }
    if (ScanLinks(cur, start)) {
        return true;
    }

    if (end.m_wAreaId == static_cast<uint16>(-1)) {
        return false;
    }
    if (paths.m_pPathNodes[end.m_wAreaId] == nullptr) {
        return false;
    }
    return ScanLinks(end, cur);
}

/*!
* @notsa
*/
CBouncingPanel* CAutomobile::CheckIfExistsGetFree(eCarNodes nodeIdx) {
    // NOTE/TODO/BUG:
    // This is the original code.. I'm not sure how it works..
    // It checks panel's frame == `nodeIdx`, but only up until the first one whose frame is == -1... Weird, maybe a bug?
    // Como el original (0x6B13C6..0x6B13EB y 0x6B14F6..0x6B1517): el bucle recorre 4 entradas de 0x20 bytes desde 0x6AC (la cuarta
    // son los primeros bytes de m_swingingChassis); 0xFFFF = libre, y movsx word == nodeIdx = ya existe
    auto* const base = reinterpret_cast<uint8*>(m_panels.data());
    for (size_t i = 0; i < 4; i++) {
        auto* const panel = reinterpret_cast<CBouncingPanel*>(base + i * sizeof(CBouncingPanel));
        const uint16 frameId = *reinterpret_cast<const uint16*>(panel);
        if (frameId == (uint16)-1) {
            return panel;
        } else if ((int32)(int16)frameId == (int32)nodeIdx) {
            return nullptr;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// CAutomobile::PreRender (0x6AAB50..0x6ACCBA). Subfunciones privadas fieles al flujo del EXE (direcciones en los comentarios); los tramos x87 con
// registros sin guardar van a los ayudantes AutoPre* de X87Math. Los modelos se comparan como en el EXE (cmp ax, imm16 sobre m_nModelIndex).
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace {

// 0x59C790 y 0x59C890 con la pila x87 (ayudantes AutoPre de X87Math)
CVector AutoPreRenderMultiply3x3(const CMatrix& matrix, const CVector& v) {
    CVector out;
    AutoPreMultiply3x3Cpp(&matrix.GetRight().x, &v.x, &out.x);
    return out;
}
CVector AutoPreRenderMultiplyMatrixWithVector(const CMatrix& matrix, const CVector& v) {
    CVector out;
    AutoPreMultiplyMatrixWithVectorCpp(&matrix.GetRight().x, &v.x, &out.x);
    return out;
}

// 0x41CCB0 (CPlaceable::GetForward del EXE): forward de la matriz o (-sin h, cos h, 0) con fsin/fcos del x87 guardados en float
CVector AutoPreRenderPlaceableForward(const CPlaceable& placeable) {
    if (placeable.m_matrix) {
        return placeable.m_matrix->GetForward();
    }
    const float heading = placeable.m_placement.m_fHeading;
    return CVector{ -TrigSimpleSinCpp(heading), TrigSimpleCosCpp(heading), 0.f };
}

// 0x6AABA4..0x6AADCF: suspension de las cuatro ruedas (DoBurstAndSoftGroundRatios = virtual +0xD0, lineas de la ColData en 0x10 + 0x20 * rueda)
void AutoPreRenderSuspension(CAutomobile& car, const CCollisionData* colData) {
    car.DoBurstAndSoftGroundRatios();
    for (size_t wheel = 0; wheel < 4; wheel++) {
        car.m_wheelPosition[wheel] = AutoPreSuspensionHeightCpp(
            car.m_aSuspensionSpringLength[wheel],
            car.m_aSuspensionLineLength[wheel],
            car.m_fWheelsSuspensionCompression[wheel],
            colData->m_pLines[wheel].m_vecStart.z,
            car.m_wheelPosition[wheel]
        );
    }
}

// 0x6AAE48..0x6AB06E (estado SIMPLE): posicion de contacto de cada rueda = matriz del coche * (x, y del marco de la rueda, posicion de la rueda * 1.5f).
// Orden del EXE: nodo 4 (rueda 3), nodo 7 (rueda 1), nodo 2 (rueda 2), nodo 5 (rueda 0).
void AutoPreRenderSimpleWheelPoints(CAutomobile& car) {
    CMatrix frameMatrix; // CMatrix local sin matriz asociada
    struct { eCarNodes node; size_t wheel; } static constexpr order[] = {
        { CAR_WHEEL_RB, 3 }, { CAR_WHEEL_LB, 1 }, { CAR_WHEEL_RF, 2 }, { CAR_WHEEL_LF, 0 },
    };
    for (const auto& [node, wheel] : order) {
        frameMatrix.Attach(&car.m_aCarNodes[node]->modelling, false);
        const CVector local{ frameMatrix.GetPosition().x, frameMatrix.GetPosition().y, AutoPreMulCpp(car.m_wheelPosition[wheel], 1.5f) }; // fld; fmul [1.5f]; fstp: x87 (un subnormal no se vacia a 0 como con SSE FTZ)
        car.m_wheelColPoint[wheel].m_vecPoint = AutoPreRenderMultiplyMatrixWithVector(*car.m_matrix, local);
        car.m_wheelColPoint[wheel].m_nSurfaceTypeB = SURFACE_DEFAULT;
    }
}

// 0x6AB088..0x6AB14E (estado 0..3): particulas de las cuatro ruedas
void AutoPreRenderWheelParticles(CAutomobile& car, float speedMagnitude) {
    const bool rearSkidding = car.m_WheelStates[1] == static_cast<tWheelState>(2) || car.m_WheelStates[3] == static_cast<tWheelState>(2);
    for (size_t wheel = 0; wheel < 4; wheel++) {
        uint32 flags = 0;
        if ((wheel == 0 || wheel == 2) && !rearSkidding) {
            flags = 4;
        }
        if (car.m_wheelSkidmarkBloodState[wheel]) {
            flags += 1;
        }
        if (car.m_wheelSkidmarkMuddy[wheel]) {
            flags += 2;
        }
        const float direction = wheel < 2 ? -1.f : 1.f;
        const uint32 wheelStatus = static_cast<uint32>(car.m_damageManager.GetWheelStatus(static_cast<eCarWheel>(wheel))); // 0x6C21B0
        car.AddSingleWheelParticles( // 0x6DE880
            car.m_WheelStates[wheel],
            wheelStatus,
            car.m_fWheelsSuspensionCompressionPrev[wheel],
            speedMagnitude,
            &car.m_wheelColPoint[wheel],
            &car.m_wheelColPoint[wheel].m_vecPoint,
            direction,
            static_cast<int32>(wheel),
            static_cast<uint32>(car.m_wheelSkidmarkType[wheel]),
            &car.m_wheelSkidmarkBloodState[wheel],
            flags
        );
    }
}

// 0x6AB154..0x6AB2BC: particulas de las ruedas intermedias (nodo 3 presente): posicion de contacto de la rueda trasera + 2 * forward
void AutoPreRenderMiddleWheelParticles(CAutomobile& car, float speedMagnitude) {
    if (!car.m_aCarNodes[CAR_WHEEL_RM]) {
        return;
    }
    struct { size_t wheel; int32 slot; } static constexpr order[] = { { 1, 5 }, { 3, 6 } };
    for (const auto& [wheel, slot] : order) {
        uint32 flags = 0;
        if (car.m_wheelSkidmarkBloodState[wheel]) {
            flags = 1;
        }
        if (car.m_wheelSkidmarkMuddy[wheel]) {
            flags += 2;
        }
        CVector position;
        AutoPreDoublePlusCpp(&car.m_matrix->GetForward().x, &car.m_wheelColPoint[wheel].m_vecPoint.x, &position.x);
        const uint32 wheelStatus = static_cast<uint32>(car.m_damageManager.GetWheelStatus(static_cast<eCarWheel>(wheel)));
        car.AddSingleWheelParticles(
            car.m_WheelStates[wheel],
            wheelStatus,
            car.m_fWheelsSuspensionCompressionPrev[wheel],
            speedMagnitude,
            &car.m_wheelColPoint[wheel],
            &position,
            1.0f,
            slot,
            static_cast<uint32>(car.m_wheelSkidmarkType[wheel]),
            &car.m_wheelSkidmarkBloodState[wheel],
            flags
        );
    }
}

// 0x6AB2BC..0x6AB349: salpicaduras de lluvia y escape
void AutoPreRenderRainAndExhaust(CAutomobile& car, float speedRatio) {
    if (!CCullZones::CamNoRain() && !CCullZones::PlayerNoRain()
        && fabsf(speedRatio) < 20.f
        && CWeather::Rain > 0.02f
        && StaticRef<int32>(0xB72914) == 0 // CGame::currArea
    ) {
        car.AddWaterSplashParticles(); // 0x6DDF60
    }
    if (car.vehicleFlags.bEngineOn
        && !car.m_pHandlingData->m_bNoExhaust
        && speedRatio < 130.f
        && !car.vehicleFlags.bIsDrowning
        && !car.vehicleFlags.bDisableParticles
    ) {
        car.AddExhaustParticles(); // 0x6DE240
    }
}

// 0x6ABA84..0x6ABBAE (taxi 420 y cabbie 438): corona + luz puntual del cartel
void AutoPreRenderTaxiLight(CAutomobile& car, const CVector& local) {
    if (!car.autoFlags.bTaxiLight) {
        return;
    }
    const CVector world = AutoPreRenderMultiplyMatrixWithVector(*car.m_matrix, local); // 0x59C890
    const uint8 intensity = static_cast<uint8>(AutoPreCoronaByteCpp(StaticRef<float>(0xB7C4E4)));
    CCoronas::RegisterCorona( // 0x6FC580
        static_cast<uint32>(reinterpret_cast<uintptr_t>(&car) + 0x11), &car,
        intensity, intensity, 0, 255, local, 0.8f, AutoPreMulCpp(StaticRef<float>(0xB6F118), 150.f),
        static_cast<eCoronaType>(1), static_cast<eCoronaFlareType>(0), static_cast<eCoronaReflType>(1), static_cast<eCoronaLOSCheck>(0), static_cast<eCoronaTrail>(0),
        0.f, false, 1.5f, false, 15.f, false, true
    );
    CPointLights::AddLight(0, world, CVector{ 0.f, 0.f, 0.f }, 10.f, 0.1f, 0.1f, 0.05f, 0, true, nullptr); // 0x7000E0
}

// 0x6ABBB3..0x6ABC6E (FBI Rancher 490): corona azul solo si el forward del coche apunta en contra del de la camara
void AutoPreRenderRancherLight(CAutomobile& car) {
    if (!car.vehicleFlags.bSirenOrAlarm) {
        return;
    }
    const CVector local{ 0.f, 1.2f, 0.5f };
    if (!(CTimer::GetTimeInMS() & 0x100)) {
        return;
    }
    const CVector cameraForward = AutoPreRenderPlaceableForward(TheCamera);
    if (!AutoPreDotNegativeCpp(&car.m_matrix->GetForward().x, &cameraForward.x)) {
        return;
    }
    CCoronas::RegisterCorona(
        static_cast<uint32>(reinterpret_cast<uintptr_t>(&car) + 0x15), &car,
        0, 0, 255, 255, local, 0.4f, AutoPreMulCpp(StaticRef<float>(0xB6F118), 150.f),
        static_cast<eCoronaType>(0), static_cast<eCoronaFlareType>(0), static_cast<eCoronaReflType>(0), static_cast<eCoronaLOSCheck>(0), static_cast<eCoronaTrail>(0),
        0.f, false, 1.5f, false, 15.f, false, true
    );
}

// 0x6AB374..0x6ABA7F (modelos 407, 416, 427, 596..599): luz puntual de la sirena y cuatro coronas por turno
void AutoPreRenderEmergencyLights(CAutomobile& car) {
    if (!car.vehicleFlags.bSirenOrAlarm) {
        return;
    }

    // Puntos izquierdo (a) y derecho (b) de la barra de luces y cuatro bytes de color (al, dl, cl y el byte [esp+40h]); los casos 0..4 de 0x6ACD94
    CVector a{}, b{};
    uint8 al = 0, dl = 0, cl = 0, extra = 0;
    switch (static_cast<int16>(car.m_nModelIndex)) {
    case 407: a = CVector{ 0.9f, 3.2f, 1.3f }; b = CVector{ -0.9f, 3.2f, 1.3f }; al = 255; dl = 255; cl = 0;   extra = 255; break;
    case 416: a = CVector{ 0.6f, 0.9f, 1.2f }; b = CVector{ -0.6f, 0.9f, 1.2f }; al = 255; dl = 255; cl = 255; extra = 255; break;
    case 427: a = CVector{ 0.55f, 1.1f, 1.4f }; b = CVector{ -0.55f, 1.1f, 1.4f }; al = 255; dl = 0; cl = 255; extra = 0;   break;
    case 596: case 597: case 598:
              a = CVector{ 0.7f, -0.4f, 1.0f }; b = CVector{ -0.7f, -0.4f, 1.0f }; al = 255; dl = 0; cl = 255; extra = 0;   break;
    case 599: a = CVector{ 0.7f, -0.1f, 1.2f }; b = CVector{ -0.7f, -0.1f, 1.2f }; al = 255; dl = 0; cl = 255; extra = 0;   break;
    default: break; // caso 5 de la tabla (0x6AB5C6): sin colores
    }

    // 0x6AB5C6: color de la luz puntual segun la fase de 1024 ms
    const uint32 timer = CTimer::GetTimeInMS();
    uint32 phase = timer & 0x3FF;
    uint32 c0, c1, c2;
    if (phase < 0x200) {
        c0 = al / 6;
        c1 = 0;
        c2 = 0;
    } else {
        c0 = dl / 6;
        c1 = extra / 6;
        c2 = cl / 6;
    }
    c0 &= 0xFF; // bl, [esp+13h], [esp+1Fh]: solo cuentan los bytes bajos
    phase &= 0x1FF;
    if (phase < 100) {
        const float fade = static_cast<float>(static_cast<int32>(phase));
        c0 = AutoPreFadeByteCpp(c0, fade);
        c1 = AutoPreFadeByteCpp(c1, fade);
        c2 = AutoPreFadeByteCpp(c2, fade);
    } else if (phase > 0x19C) {
        const float fade = static_cast<float>(static_cast<int32>(0x200 - phase));
        c0 = AutoPreFadeByteCpp(c0, fade);
        c1 = AutoPreFadeByteCpp(c1, fade);
        c2 = AutoPreFadeByteCpp(c2, fade);
    }

    // 0x6AB715..0x6AB814: AddLight(0, posicion + 2 * up, 0, 10, c * 0.005f, ..., 0, true, nullptr)
    const CVector& position = car.m_matrix ? car.m_matrix->GetPosition() : car.m_placement.m_vPosn;
    CVector lightPosition;
    AutoPreDoublePlusCpp(&car.m_matrix->GetUp().x, &position.x, &lightPosition.x);
    CPointLights::AddLight(0, lightPosition, CVector{ 0.f, 0.f, 0.f }, 10.f,
        static_cast<float>(c0) * 0.005f, static_cast<float>(c1) * 0.005f, static_cast<float>(c2) * 0.005f, 0, true, nullptr);

    // 0x6AB823..0x6AB8B7: colores de las coronas = byte bajo de _ftol2((color * [0xB7C4E4]) * 0.1f); el segundo producto (M * 0.0f) vale siempre 0
    const float coronaScale = StaticRef<float>(0xB7C4E4);
    const uint8 coronaA = static_cast<uint8>(AutoPreScaleByteCpp(al, coronaScale));
    const uint8 coronaB = static_cast<uint8>(AutoPreScaleByteCpp(dl, coronaScale));
    const uint8 coronaD = static_cast<uint8>(AutoPreScaleByteCpp(extra, coronaScale));
    const uint8 coronaC = static_cast<uint8>(AutoPreScaleByteCpp(cl, coronaScale));

    // 0x6AB8E0..0x6ABA7F: cuatro luces repartidas entre b (i = 0) y a (i = 3)
    for (uint32 i = 0; i < 4; i++) {
        const float index = static_cast<float>(static_cast<int32>(i));
        const float rest = 3.0f - index;
        const uint32 selector = ((i * 0x40 + CTimer::GetTimeInMS()) >> 8) & 3;
        const float bz = b.z * rest;
        const float by = b.y * rest;
        const float az = a.z * index;
        const float ay = a.y * index;
        const float ax = a.x * index;
        const float qz = az + bz;
        const float qy = ay + by;
        const float qx = ax + rest * b.x;
        const CVector local{ qx * 0.33333334f, qy * 0.33333334f, qz * 0.33333334f };
        uint8 r, g, bl;
        if (selector == 0) {
            r = coronaA; g = 0; bl = 0;
        } else if (selector == 2) {
            r = coronaB; g = coronaD; bl = coronaC;
        } else {
            continue;
        }
        CCoronas::RegisterCorona(
            static_cast<uint32>(reinterpret_cast<uintptr_t>(&car) + 0x15 + i), &car,
            r, g, bl, 255, local, 0.4f, AutoPreMulCpp(StaticRef<float>(0xB6F118), 150.f),
            static_cast<eCoronaType>(0), static_cast<eCoronaFlareType>(0), static_cast<eCoronaReflType>(0), static_cast<eCoronaLOSCheck>(0), static_cast<eCoronaTrail>(0),
            0.f, false, 1.5f, false, 15.f, false, true
        );
    }
}

// 0x6AB350..0x6ABC71: luces por modelo (tabla de saltos 0x6ACCBC / bytes 0x6ACCD0)
void AutoPreRenderModelLights(CAutomobile& car) {
    switch (static_cast<int16>(car.m_nModelIndex)) {
    case 407: case 416: case 427: case 596: case 597: case 598: case 599:
        AutoPreRenderEmergencyLights(car);
        break;
    case 420: // taxi
        AutoPreRenderTaxiLight(car, CVector{ 0.f, -0.4f, 0.95f });
        break;
    case 438: // cabbie
        AutoPreRenderTaxiLight(car, CVector{ 0.f, 0.f, 0.85f });
        break;
    case 490: // FBI Rancher
        AutoPreRenderRancherLight(car);
        break;
    default:
        break;
    }
}

// 0x6ABD4D..0x6ABE83: direcciones de las ruedas delanteras (front) y traseras (rear) segun la direccion y las banderas de manejo
void AutoPreRenderWheelDirections(CAutomobile& car, CVector& front, CVector& rear) {
    const float steer = car.m_fSteerAngle;
    front = AutoPreRenderMultiply3x3(*car.m_matrix, CVector{ -TrigSimpleSinCpp(steer), TrigSimpleCosCpp(steer), 0.f }); // 0x59C790
    rear = car.m_matrix->GetForward();
    if (car.m_nHandlingFlagsIntValue & 0x20) { // bSteerRearwheels
        front = car.m_matrix->GetForward();
        rear = AutoPreRenderMultiply3x3(*car.m_matrix, CVector{ TrigSimpleSinCpp(steer), TrigSimpleCosCpp(steer), 0.f });
    } else if ((car.m_nHandlingFlagsIntValue & 0x40) && car.m_f2ndSteerAngle != 0.f) { // bHbRearwheelSteer
        const float steer2 = car.m_f2ndSteerAngle;
        rear = AutoPreRenderMultiply3x3(*car.m_matrix, CVector{ -TrigSimpleSinCpp(steer2), TrigSimpleCosCpp(steer2), 0.f });
    }
}

// 0x6ABE85..0x6ABFC0: rotacion de las cuatro ruedas
void AutoPreRenderWheelRotation(CAutomobile& car, const CVehicleModelInfo& modelInfo, const CVector& front, const CVector& rear) {
    const bool isHeli = car.m_pHandlingData->m_bIsHeli; // bit 25 de las banderas de modelo
    for (size_t wheel = 0; wheel < 4; wheel++) {
        if (!(car.m_WheelCounts[wheel] > 0.f)) {
            continue;
        }
        if (isHeli && wheel != 0 && wheel != 2) {
            continue;
        }
        const CVector& position = car.m_matrix ? car.m_matrix->GetPosition() : car.m_placement.m_vPosn;
        const CVector contact = car.m_wheelColPoint[wheel].m_vecPoint;
        const CVector relative{ AutoPreSubCpp(contact.x, position.x), AutoPreSubCpp(contact.y, position.y), AutoPreSubCpp(contact.z, position.z) };
        const CVector speed = car.GetSpeed(relative); // 0x542CE0
        const bool isFront = wheel == 0 || wheel == 2;
        const float radius = AutoPreMulCpp(isFront ? modelInfo.m_fWheelSizeFront : modelInfo.m_fWheelSizeRear, 0.5f); // fld; fmul [0.5f]; fstp: x87 (un tamano subnormal no se vacia a 0 como con SSE FTZ)
        car.m_wheelSpeed[wheel] = car.ProcessWheelRotation(car.m_WheelStates[wheel], isFront ? front : rear, speed, radius); // 0x6D1230
        car.m_wheelRotation[wheel] = AutoPreWheelRotationCpp(CTimer::GetTimeStep(), car.m_wheelSpeed[wheel], car.m_wheelRotation[wheel]);
    }
}

// 0x6ABFC6..0x6AC22D: matrices de las ruedas, puertas, paneles y cadera oscilante
void AutoPreRenderBodyParts(CAutomobile& car, CMatrix& frameMatrix) {
    const int32 wheelFlags = car.m_nModelIndex == 432 ? 7 : 0;
    car.UpdateWheelMatrix(2, wheelFlags);
    car.UpdateWheelMatrix(5, wheelFlags);
    car.UpdateWheelMatrix(4, wheelFlags);
    car.UpdateWheelMatrix(7, wheelFlags);
    if (car.m_aCarNodes[CAR_WHEEL_RM]) {
        car.UpdateWheelMatrix(3, wheelFlags);
    }
    if (car.m_aCarNodes[CAR_WHEEL_LM]) {
        car.UpdateWheelMatrix(6, wheelFlags);
    }
    if (car.m_nModelIndex == 432) { // el Rhino salta puertas, paneles y cadera hasta 0x6AC232
        return;
    }

    car.ProcessSwingingDoor(CAR_DOOR_LF, DOOR_LEFT_FRONT);      // 0x6A9D70 (10, 2)
    car.ProcessSwingingDoor(CAR_DOOR_RF, DOOR_RIGHT_FRONT);     // (8, 3)
    car.ProcessSwingingDoor(CAR_DOOR_LR, DOOR_LEFT_REAR);       // (11, 4)
    car.ProcessSwingingDoor(CAR_DOOR_RR, DOOR_RIGHT_REAR);      // (9, 5)
    car.ProcessSwingingDoor(CAR_BONNET, DOOR_BONNET);           // (16, 0)
    car.ProcessSwingingDoor(CAR_BOOT, DOOR_BOOT);               // (17, 1)

    // 0x6AC073..0x6AC0E0: tres paneles que rebotan (el indice de marco es un int16 con signo; < 0 = sin usar)
    for (size_t panel = 0; panel < 3; panel++) {
        auto& bouncing = car.m_panels[panel];
        if (static_cast<int16>(bouncing.m_nFrameId) > -1) {
            RwFrame* const frame = reinterpret_cast<RwFrame* const*>(car.m_aCarNodes.data())[static_cast<int16>(bouncing.m_nFrameId)];
            bouncing.ProcessPanel(&car, frame, car.m_moveForce, car.m_turnForce, -1.f, -1.f); // 0x6F49A0
        }
    }

    // 0x6AC0E2..0x6AC22D: cadera oscilante (nodo 1; nodo 21 en el modelo 544)
    const eCarNodes chassisNode = car.m_nModelIndex == 544 ? CAR_MISC_B : CAR_CHASSIS;
    if (car.m_swingingChassis.m_doorState == DOOR_HIT_MAX_END) {
        if (RwFrame* const frame = car.m_aCarNodes[chassisNode]) {
            frameMatrix.Attach(&frame->modelling, false);
            const CVector savedPosition = frameMatrix.GetPosition();
            const CVector rotated = AutoPreRenderMultiply3x3(*car.m_matrix, savedPosition); // 0x59C790
            car.m_swingingChassis.Process(&car, car.m_moveForce, car.m_turnForce, rotated); // 0x6F4040
            CVector orientation{ 0.f, 0.f, 0.f };
            orientation[car.m_swingingChassis.m_axis] = car.m_swingingChassis.m_angle;
            frameMatrix.SetRotate(orientation.x, orientation.y, orientation.z); // 0x59B120
            if (car.m_nModelIndex == 544) {
                frameMatrix.RotateX(static_cast<float>(car.m_wMiscComponentAngle) * -0.0001f); // 0x59B1E0
            }
            // SetRotate deja la posicion a cero y el EXE SUMA la guardada (fld; fadd; fstp por componente)
            frameMatrix.GetPosition().x = AutoPreAddCpp(savedPosition.x, frameMatrix.GetPosition().x);
            frameMatrix.GetPosition().y = AutoPreAddCpp(savedPosition.y, frameMatrix.GetPosition().y);
            frameMatrix.GetPosition().z = AutoPreAddCpp(frameMatrix.GetPosition().z, savedPosition.z);
            frameMatrix.UpdateRW(); // 0x59BBB0
        }
    }
}

// 0x6AC232..0x6AC297: fuerzas = velocidad + velocidad de friccion (movimiento y giro)
void AutoPreRenderForces(CAutomobile& car) {
    car.m_moveForce = CVector{
        AutoPreAddCpp(car.m_vecMoveSpeed.x, car.m_vecFrictionMoveSpeed.x),
        AutoPreAddCpp(car.m_vecMoveSpeed.y, car.m_vecFrictionMoveSpeed.y),
        AutoPreAddCpp(car.m_vecMoveSpeed.z, car.m_vecFrictionMoveSpeed.z),
    };
    car.m_turnForce = CVector{
        AutoPreAddCpp(car.m_vecTurnSpeed.x, car.m_vecFrictionTurnSpeed.x),
        AutoPreAddCpp(car.m_vecTurnSpeed.y, car.m_vecFrictionTurnSpeed.y),
        AutoPreAddCpp(car.m_vecTurnSpeed.z, car.m_vecFrictionTurnSpeed.z),
    };
}

// 0x6AC2B0..0x6AC40C y 0x6ACADE..0x6ACBC0: ejes inclinables (BF Injection 424: nodos 21, 0, 22; Bandito 568 / Hotknife 434: nodos 20, 22, 21, 23)
void AutoPreRenderTiltAxles(CAutomobile& car, CMatrix& frameMatrix, const CCollisionData* colData, const int32* nodes, size_t count, bool useY) {
    for (size_t i = 0; i < count; i++) {
        const int32 nodeId = nodes[i];
        if (useY && nodeId == 0) {
            continue;
        }
        RwFrame* const frame = car.m_aCarNodes.data()[nodeId];
        if (!frame) {
            continue;
        }
        frameMatrix.Attach(&frame->modelling, false);
        const CColLine& line = colData->m_pLines[i];
        const float upper = car.m_pHandlingData->m_fSuspensionUpperLimit; // offset 0xB8
        if (useY) {
            frameMatrix.GetForward().z = AutoPreAxleTiltCpp(car.m_wheelPosition[i], line.m_vecStart.z, upper, line.m_vecStart.y, frameMatrix.GetPosition().y);
        } else {
            frameMatrix.GetRight().z = AutoPreAxleTiltCpp(car.m_wheelPosition[i], line.m_vecStart.z, upper, line.m_vecStart.x, frameMatrix.GetPosition().x);
        }
        frameMatrix.UpdateRW();
    }
}

// 0x6AC2B0..0x6AC40C (modelo 424)
void AutoPreRenderBfInjection(CAutomobile& car, CMatrix& frameMatrix, const CCollisionData* colData, CVehicleModelInfo& modelInfo) {
    const float angle = AutoPreMulCpp(CTimer::GetTimeStep(), fabsf(car.m_GasPedal) > 0.f ? 0.5f : 0.3f);
    car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_A], AXIS_Y, angle, false); // 0x6DBA30
    car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_E], AXIS_Y, AutoPreMulCpp(angle, 1.5f), false);
    (void)car.GetColModel(); // 0x535300 (el EXE no usa el resultado)
    static constexpr int32 nodes[] = { 21, 0, 22 }; // 0x8D31F0
    AutoPreRenderTiltAxles(car, frameMatrix, colData, nodes, 3, true);
    CVector wheelPosition;
    modelInfo.GetWheelPosn(1, wheelPosition, false); // 0x4C7D20
    car.SetTransmissionRotation(car.m_aCarNodes[CAR_MISC_D], car.m_wheelPosition[1], car.m_wheelPosition[3], wheelPosition, false); // 0x6DBBB0
}

// 0x6AC507..0x6AC6D4 (modelo 525): brazo de la grua
void AutoPreRenderTowTruck(CAutomobile& car, CMatrix& frameMatrix) {
    const float towAngle = AutoPreTowAngleCpp(car.m_wMiscComponentAngle, StaticRef<float>(0x8D3144), StaticRef<uint16>(0x8D313C));
    car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_A], AXIS_X, towAngle, true);
    if (car.m_pVehicleBeingTowed) {
        const float negAngle = towAngle * -1.0f; // fmul por -1.0f (0x858C1C)
        CVector hitch, bar;
        if (!car.m_pVehicleBeingTowed->GetTowHitchPos(hitch, true, &car)) { // virtual +0xEC
            return;
        }
        if (!car.GetTowBarPos(bar, true, car.m_pVehicleBeingTowed)) {       // virtual +0xF0
            return;
        }
        float angleA, angleB;
        AutoPreTowAnglesCpp(&hitch.x, &bar.x, &car.m_matrix->GetForward().x, &car.m_matrix->GetRight().x, StaticRef<float>(0x8D3148), negAngle, &angleA, &angleB);
        car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_B], AXIS_X, angleA, true);
        car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_B], AXIS_Y, angleB, false);
    } else {
        car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_B], AXIS_X, -towAngle, true);
        if (car.m_panels[0].m_nFrameId == 21) {
            frameMatrix.Attach(&car.m_aCarNodes[CAR_MISC_B]->modelling, false);
            frameMatrix.GetUp().y = AutoPreAddCpp(frameMatrix.GetUp().y, car.m_panels[0].m_vecRotation.x);
            frameMatrix.GetUp().x = AutoPreAddCpp(frameMatrix.GetUp().x, car.m_panels[0].m_vecRotation.y);
            frameMatrix.UpdateRW();
        }
    }
}

// 0x6AC71C..0x6AC7A6 (modelo 530): horquilla del montacargas
void AutoPreRenderForklift(CAutomobile& car, CMatrix& frameMatrix) {
    const auto* const forkliftInfo = reinterpret_cast<CBaseModelInfo* const*>(0xA9B0C8)[530];
    RwFrame* const rest = CClumpModelInfo::GetFrameFromId(forkliftInfo->GetRpClump(), 0x14); // 0x4C53C0 sobre [ms_modelInfoPtrs[530] + 0x1C]
    const float restX = rest->modelling.pos.x;
    const float restY = rest->modelling.pos.y;
    const float restZ = AutoPreForkliftZCpp(car.m_wMiscComponentAngle, 0.0006f, rest->modelling.pos.z);
    frameMatrix.Attach(&car.m_aCarNodes[CAR_MISC_A]->modelling, false);
    frameMatrix.SetTranslateOnly(CVector{ restX, restY, restZ }); // 0x59AF80
    frameMatrix.UpdateRW();
}

// 0x6AC7AB..0x6ACA32 (modelo 532): cosechadora
void AutoPreRenderHarvester(CAutomobile& car, CMatrix& frameMatrix, CVehicleModelInfo& modelInfo) {
    car.m_fPropRotate = AutoPreHarvesterPropCpp(&car.m_vecMoveSpeed.x, &car.m_matrix->GetForward().x, car.m_fPropRotate, CTimer::GetTimeStep());
    car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_B], AXIS_X, car.m_fPropRotate, true);

    CVector wheelPosition;
    modelInfo.GetWheelPosn(0, wheelPosition, false); // 0x4C7D20
    frameMatrix.Attach(&car.m_aCarNodes[CAR_MISC_A]->modelling, false);
    float angleX, angleY;
    VehicleTransmissionAnglesCpp(car.m_wheelPosition[0], car.m_wheelPosition[2], frameMatrix.GetPosition().y, &wheelPosition.x, true, &angleX, &angleY);
    car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_A], AXIS_X, angleX, true);
    car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_A], AXIS_Y, angleY, false);

    float tilt = 0.f;
    if (car.m_matrix->GetUp().z > 0.f && car.m_nGettingInFlags == 0 && car.m_nGettingOutFlags == 0) {
        tilt = AutoPreClampedAsinCpp(car.m_matrix->GetRight().z, StaticRef<float>(0x8D31EC));
    }
    car.m_fDoomVerticalRotation = AutomobileWheelSpinAudioCpp(StaticRef<float>(0x8D31E8), CTimer::GetTimeStep(), car.m_fDoomVerticalRotation, tilt);
    car.SetComponentRotation(car.m_aCarNodes[CAR_CHASSIS], AXIS_Y, car.m_fDoomVerticalRotation, true);
}

// 0x6AC297..0x6ACBC7: movimiento de los componentes propios de cada modelo
void AutoPreRenderModelComponents(CAutomobile& car, CMatrix& frameMatrix, const CCollisionData* colData, CVehicleModelInfo& modelInfo) {
    const int16 model = static_cast<int16>(car.m_nModelIndex);
    const float misc = static_cast<float>(car.m_wMiscComponentAngle);
    auto& nodes = car.m_aCarNodes;

    if (model == 424) {
        AutoPreRenderBfInjection(car, frameMatrix, colData, modelInfo);
    } else if (model == 486) { // 0x1E6
        car.SetComponentRotation(nodes[CAR_MISC_A], AXIS_X, misc * 0.0002f, true);
    } else if (model == 524) { // 0x20C
        if (car.GetStatus() != STATUS_WRECKED) {
            car.SetComponentRotation(nodes[CAR_MISC_A], AXIS_X, StaticRef<float>(0x871050), true);
            car.m_fPropRotate = AutoPreMixerPropCpp(car.m_wMiscComponentAngle, StaticRef<int16>(0x8D314C), CTimer::GetTimeStep(), car.m_fPropRotate);
            car.SetComponentRotation(nodes[CAR_MISC_B], AXIS_Y, car.m_fPropRotate, true);
            car.SetComponentRotation(nodes[CAR_MISC_B], AXIS_X, StaticRef<float>(0x871050), false);
        }
    } else if (model == 443) { // 0x1BB
        car.SetComponentRotation(nodes[CAR_MISC_A], AXIS_X, misc * -0.0001f, true);
    } else if (model == 525) { // 0x20D
        AutoPreRenderTowTruck(car, frameMatrix);
    } else if (model == 531) { // 0x213
        car.SetComponentRotation(nodes[CAR_MISC_A], AXIS_X, AutoPreTowAngleCpp(car.m_wMiscComponentAngle, StaticRef<float>(0x8D3144), StaticRef<uint16>(0x8D313C)), true);
    } else if (model == 530 && nodes[CAR_MISC_A]) { // 0x212
        AutoPreRenderForklift(car, frameMatrix);
    } else if (model == 532) { // 0x214
        AutoPreRenderHarvester(car, frameMatrix, modelInfo);
    } else if (model == 568 || model == 434) { // 0x238, 0x1B2
        (void)car.GetColModel();
        static constexpr int32 axleNodes[] = { 20, 22, 21, 23 }; // 0x8D31D8
        AutoPreRenderTiltAxles(car, frameMatrix, colData, axleNodes, 4, false);
        car.SetComponentRotation(nodes[CAR_MISC_E], AXIS_Y, AutoPreMulCpp(CTimer::GetTimeStep(), fabsf(car.m_GasPedal) > 0.f ? 0.5f : 0.3f), false);
    } else if (model == 432 || model == 601) { // 0x1B0, 0x259
        car.SetComponentRotation(nodes[CAR_MISC_A], AXIS_Z, car.m_fDoomVerticalRotation, true);
        car.SetComponentRotation(nodes[CAR_MISC_B], AXIS_X, car.m_fDoomHorizontalRotation, true);
    } else if (model == 407) { // 0x197
        car.SetComponentRotation(nodes[CAR_MISC_A], AXIS_X, car.m_fDoomHorizontalRotation, true);
        car.SetComponentRotation(nodes[CAR_MISC_A], AXIS_Z, car.m_fDoomVerticalRotation, false);
    } else if (model == 477) { // 0x1DD
        car.SetComponentRotation(nodes[CAR_MISC_A], AXIS_X, car.m_fPropRotate, true);
    }
}

// 0x6ACBC7..0x6ACC8B: visual de la hidraulica
void AutoPreRenderHydraulics(CAutomobile& car, CVehicleModelInfo& modelInfo) {
    if (car.m_nModelIndex != 495
        && !((car.m_nHandlingFlagsIntValue & 0x10000) && (car.m_nHandlingFlagsIntValue & 0x20000) && car.m_nVehicleSubType == 0)) {
        return;
    }
    CVector wheelPosition;
    modelInfo.GetWheelPosn(0, wheelPosition, false); // 0x4C7D20
    car.SetTransmissionRotation(car.m_aCarNodes[CAR_MISC_A], car.m_wheelPosition[0], car.m_wheelPosition[2], wheelPosition, true); // 0x6DBBB0
    modelInfo.GetWheelPosn(1, wheelPosition, false);
    car.SetTransmissionRotation(car.m_aCarNodes[CAR_MISC_B], car.m_wheelPosition[1], car.m_wheelPosition[3], wheelPosition, false);
}

} // namespace

// 0x6AAB50
void CAutomobile::PreRender() {
    CVehicle::PreRender(); // 0x6D6480
    if (StaticRef<bool>(0x969152)) { // CCheat::m_aCheatsActive[CHEAT_CARS_ON_WATER]
        DoHoverSuspensionRatios();   // virtual +0x114
    }
    const CCollisionData* const colData = GetColModel()->m_pColData; // 0x535300; ColData en +0x2C (se guarda en [esp+70h])

    if (vehicleFlags.bVehicleColProcessed && m_nVehicleSubType != VEHICLE_TYPE_MTRUCK) { // byte 0x42B bit 0 y [esi+594h] != 1
        AutoPreRenderSuspension(*this, colData);
    }

    // 0x6AADCF..0x6AAE1B
    const float speedRatio = AutoPreForwardSpeedCpp(&m_vecMoveSpeed.x, &m_matrix->GetForward().x, StaticRef<float>(0xC1BFF0));
    const float speedMagnitude = AutoPreSpeedMagnitudeCpp(&m_vecMoveSpeed.x);

    // 0x6AAE1F..0x6AB154: salta con el modelo 0xFFFE (-2), 539 o 441
    if (m_nModelIndex != 0xFFFE && m_nModelIndex != 539 && m_nModelIndex != 441) {
        if (static_cast<uint32>(GetStatus()) == STATUS_SIMPLE) { // byte 0x36 & 0xF8 == 0x10
            AutoPreRenderSimpleWheelPoints(*this);
        }
        if (static_cast<uint32>(GetStatus()) <= STATUS_PHYSICS) { // byte 0x36 >> 3 en 0..3
            AutoPreRenderWheelParticles(*this, speedMagnitude);
        }
    }
    AutoPreRenderMiddleWheelParticles(*this, speedMagnitude);
    AutoPreRenderRainAndExhaust(*this, speedRatio);
    AddDamagedVehicleParticles(); // 0x6D2A80
    AutoPreRenderModelLights(*this);

    // 0x6ABC71..0x6ABCBE: luces del vehiculo (no en el Bandit RC 441, el modelo 0xFFFE, el Rhino 432, vehiculos RC ni helicopteros)
    if (m_nModelIndex != 441 && m_nModelIndex != 0xFFFE && m_nModelIndex != 432 && !vehicleFlags.bIsRCVehicle && GetVehicleAppearance() != VEHICLE_APPEARANCE_HELI) {
        DoVehicleLights(*m_matrix, (m_nModelIndex == 532 || m_nModelIndex == 471) ? static_cast<eVehicleLightsFlags>(0) : static_cast<eVehicleLightsFlags>(3)); // 0x6E1A60
    }

    // 0x6ABCBE..0x6ABCFD: sombra del vehiculo (no la del jugador en primera persona mirando adelante)
    if (this != FindPlayerVehicle(-1, false) || !TheCamera.GetLookingForwardFirstPerson()) { // 0x56E0D0, 0x50AED0
        CShadows::StoreShadowForVehicle(this, (vehicleFlags.bIsRCVehicle && m_nVehicleSubType != VEHICLE_TYPE_AUTOMOBILE) ? VEH_SHD_RC : VEH_SHD_CAR); // 0x70BDA0
    }

    // 0x6ABCFD..0x6ABD63
    CMatrix frameMatrix; // segunda CMatrix local del EXE
    auto& modelInfo = *reinterpret_cast<CVehicleModelInfo* const*>(0xA9B0C8)[static_cast<int16>(m_nModelIndex)]; // movsx; CModelInfo::ms_modelInfoPtrs
    if (m_nModelIndex == 432) {
        SetComponentRotation(m_aCarNodes[CAR_BONNET], AXIS_Z, m_fDoomVerticalRotation, true); // 0x6DBA30
    }

    CVector frontDirection, rearDirection;
    AutoPreRenderWheelDirections(*this, frontDirection, rearDirection);
    AutoPreRenderWheelRotation(*this, modelInfo, frontDirection, rearDirection);
    AutoPreRenderBodyParts(*this, frameMatrix);
    AutoPreRenderForces(*this);
    AutoPreRenderModelComponents(*this, frameMatrix, colData, modelInfo);
    AutoPreRenderHydraulics(*this, modelInfo);
}

// 0x6A2B10
// Como el original (0x6A2B10..0x6A2EFC): guarda el estado de render 30 (ALPHATESTFUNCTIONREF) y lo pone a 1; con el truco del coche invisible solo se
// dibujan las ruedas (nodos 4, 7, 2, 5 y, si existen, 3 y 6), con GetFirstObject llamado dos veces por nodo y el renderCallBack de la primera;
// si no, CVehicle::Render. El Rhino (432) desplaza los nodos 7 y 4 para las orugas de las 8 lineas de colision 4..11. Despues restaura el estado
// y dibuja los haces de los faros delanteros.
void CAutomobile::Render() {
    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3000u; // 6A2B20: [B7CB84] + 0xBB8 -> [esi+4D0]

    int32 savedAlphaRef = 1; // 6A2B34: la variable vale 1 antes de la llamada a RenderStateGet
    RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, &savedAlphaRef);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(1));

    // GetFirstObject se llama dos veces por nodo (la primera da el atomico, la segunda su argumento)
    const auto RenderFirstAtomic = [](RwFrame* frame) {
        auto* const atomic = reinterpret_cast<RpAtomic*>(GetFirstObject(frame));
        atomic->renderCallBack(reinterpret_cast<RpAtomic*>(GetFirstObject(frame)));
    };

    if (CCheat::IsActive(CHEAT_INVISIBLE_CAR)) {
        // 6A2B5B..6A2C20
        RenderFirstAtomic(m_aCarNodes[CAR_WHEEL_RB]); // [esi+658h] = nodo 4
        RenderFirstAtomic(m_aCarNodes[CAR_WHEEL_LB]); // [esi+664h] = nodo 7
        RenderFirstAtomic(m_aCarNodes[CAR_WHEEL_RF]); // [esi+650h] = nodo 2
        RenderFirstAtomic(m_aCarNodes[CAR_WHEEL_LF]); // [esi+65Ch] = nodo 5
        if (m_aCarNodes[CAR_WHEEL_RM]) {              // [esi+654h] = nodo 3
            RenderFirstAtomic(m_aCarNodes[CAR_WHEEL_RM]);
        }
        if (m_aCarNodes[CAR_WHEEL_LM]) {              // [esi+660h] = nodo 6
            RenderFirstAtomic(m_aCarNodes[CAR_WHEEL_LM]);
        }
    } else {
        CVehicle::Render(); // 6A2C24: 0x6D0E60
    }

    if (static_cast<int16>(m_nModelIndex) == MODEL_RHINO) {
        // 6A2C35..6A2E86: orugas del Rhino. Las 8 lineas de colision 4..11 (suspension) mueven el nodo 7 (las 4 primeras) o el nodo 4
        const auto* const colData = GetColModel()->m_pColData; // 0x535300 y +0x2C
        const auto RawFloat = [this](size_t offset) { return *reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(this) + offset); };
        const float tracks[8]{RawFloat(0x618), RawFloat(0x61C), RawFloat(0x624), RawFloat(0x628), RawFloat(0x630), RawFloat(0x634), RawFloat(0x63C), RawFloat(0x640)};
        for (int32 i = 0; i < 8; i++) {
            const float ratio = (m_aSuspensionLineLength[0] - m_aSuspensionSpringLength[0]) / m_aSuspensionLineLength[0]; // 888h, 878h
            float       v     = tracks[i] - ratio;
            if (0.0f > v) { // fcomp 0.0 + test ah,41h: solo sustituye si v < 0 (con NaN se queda)
                v = 0.0f;
            }
            const float  u    = 1.0f - v;
            const auto*  line = reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(colData->m_pLines) + 0x80 + i * 0x20); // inicio en +0, fin en +0x10
            const float  sx   = u * line[0];
            const float  sy   = u * line[1];
            const float  ez   = v * line[6];
            const float  x    = v * line[4] + sx;
            const float  y    = v * line[5] + sy;
            const float  z    = ez + u * line[2];
            RwFrame* const node = i < 4 ? m_aCarNodes[CAR_WHEEL_LB] : m_aCarNodes[CAR_WHEEL_RB]; // [esi+664h] / [esi+658h]
            RwV3d move{x - node->modelling.pos.x, y - node->modelling.pos.y, z - node->modelling.pos.z};
            RwMatrixTranslate(&node->modelling, &move, rwCOMBINEPOSTCONCAT);
            RwFrameUpdateObjects(node);
            RenderFirstAtomic(i < 4 ? m_aCarNodes[CAR_WHEEL_LB] : m_aCarNodes[CAR_WHEEL_RB]);
            move.x = move.x * -1.0f; // 0x858C1C
            move.y = move.y * -1.0f;
            move.z = move.z * -1.0f;
            RwMatrixTranslate(&(i < 4 ? m_aCarNodes[CAR_WHEEL_LB] : m_aCarNodes[CAR_WHEEL_RB])->modelling, &move, rwCOMBINEPOSTCONCAT);
        }
    }

    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(savedAlphaRef));

    const auto model = static_cast<int16>(m_nModelIndex);
    if (model != MODEL_RCBANDIT && model != -2 && model != MODEL_RHINO && !vehicleFlags.bIsRCVehicle && GetVehicleAppearance() != VEHICLE_APPEARANCE_HELI) {
        if (m_renderLights.m_bRightFront) { // [esi+584h] bit 0
            DoHeadLightBeam(DUMMY_LIGHT_FRONT_MAIN, *m_matrix, true); // 0x6E0E20
        }
        if (m_renderLights.m_bLeftFront) {  // [esi+584h] bit 1
            DoHeadLightBeam(DUMMY_LIGHT_FRONT_MAIN, *m_matrix, false);
        }
    }
}

// 0x6A9CA0
void CAutomobile::Teleport(CVector destination, bool resetRotation) {
    CWorld::Remove(this);
    GetPosition() = destination;

    if (resetRotation)
        SetOrientation(0.0f, 0.0f, 0.0f);

    ResetMoveSpeed();
    ResetTurnSpeed();
    ResetSuspension();
    CWorld::Add(this);
}

// 0x6A0750
RwObject* GetCurrentAtomicObjectCB(RwObject* object, void* data) {
    if (RpAtomicGetFlags(object) & rpATOMICRENDER)
        *reinterpret_cast<RpAtomic**>(data) = reinterpret_cast<RpAtomic*>(object);

    return object;
}

// NOTSA
RwObject* GetCurrentAtomicObject(RwFrame* frame) {
    RwObject* out = nullptr;
    RwFrameForAllObjects(frame, GetCurrentAtomicObjectCB, &out);
    return out;
}
