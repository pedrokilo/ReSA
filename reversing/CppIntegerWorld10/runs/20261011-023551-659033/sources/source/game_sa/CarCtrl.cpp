/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCarCtrl
 * Reconstrucción: COMPLETA — 22/22 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 22/22 entradas.
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

#include "CarCtrl.h"
#include "TrafficLights.h"
#include "TheScripts.h"
#include "GangWars.h"
#include "Game.h"
#include "General.h"
#include "GameLogic.h"
#include "CutsceneMgr.h"
#include "TheCarGenerators.h"
#include "eAreaCodes.h"

#include <reversiblebugfixes/Bugs.hpp>

auto& apCarsToKeep = StaticRef<CVehicle*[2]>(0x969084);
auto& aCarsToKeepTime = StaticRef<std::array<uint32, 2>>(0x96907C);

void CCarCtrl::InjectHooks()
{
    RH_ScopedClass(CCarCtrl);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(FindGhostRoadHeight, 0x422370);
    RH_ScopedGlobalInstall(Native_00424100, 0x424100);

    using namespace ReversibleHooks;
    RH_ScopedInstall(Init, 0x4212E0);
    RH_ScopedInstall(ReInit, 0x4213B0);
    RH_ScopedInstall(InitSequence, 0x421740);
    RH_ScopedInstall(ChooseGangCarModel, 0x421A40, { .jmpCodeSize = 7 });
    RH_ScopedInstall(ChoosePoliceCarModel, 0x421980, { .jmpCodeSize = 7 });
    RH_ScopedInstall(CreateCarForScript, 0x431F80);
    RH_ScopedInstall(ChooseBoatModel, 0x421970);
    RH_ScopedInstall(ChooseCarModelToLoad, 0x421900);
    RH_ScopedInstall(GetNewVehicleDependingOnCarModel, 0x421440);
    RH_ScopedInstall(IsAnyoneParking, 0x42C250);
    RH_ScopedInstall(IsThisVehicleInteresting, 0x423EA0);
    RH_ScopedInstall(JoinCarWithRoadAccordingToMission, 0x432CB0);
    RH_ScopedInstall(PossiblyFireHSMissile, 0x429600);
    RH_ScopedInstall(PruneVehiclesOfInterest, 0x423F10);
    RH_ScopedInstall(RemoveCarsIfThePoolGetsFull, 0x4322B0);
    RH_ScopedInstall(RemoveDistantCars, 0x42CD10);
    RH_ScopedInstall(RemoveFromInterestingVehicleList, 0x423ED0);
    RH_ScopedInstall(ScriptGenerateOneEmergencyServicesCar, 0x42FBC0);
    RH_ScopedInstall(SlowCarDownForObject, 0x426220);
    RH_ScopedInstall(SlowCarOnRailsDownForTrafficAndLights, 0x434790);
    RH_ScopedInstall(FindMaxSteerAngle, 0x427FE0);
    RH_ScopedInstall(GenerateRandomCars, 0x4341C0);
    RH_ScopedInstall(ClearInterestingVehicleList, 0x423F00);
    RH_ScopedInstall(RegisterVehicleOfInterest, 0x423DE0);
    RH_ScopedInstall(FindSequenceElement, 0x421770);
    RH_ScopedInstall(SwitchVehicleToRealPhysics, 0x423FC0);
    RH_ScopedInstall(UpdateCarCount, 0x424000);
    RH_ScopedInstall(FindSpeedMultiplierWithSpeedFromNodes, 0x424130);
}

// 0x4212E0
void CCarCtrl::Init() {
    ZoneScoped;

    CarDensityMultiplier = 1.0f;
    NumRandomCars = 0;
    NumLawEnforcerCars = 0;
    NumMissionCars = 0;
    NumParkedCars = 0;
    NumPermanentVehicles = 0;
    NumAmbulancesOnDuty = 0;
    NumFireTrucksOnDuty = 0;

    LastTimeAmbulanceCreated = 0;
    LastTimeFireTruckCreated = 0;
    bAllowEmergencyServicesToBeCreated = true;
    bCarsGeneratedAroundCamera = false;
    CountDownToCarsAtStart = 2;

    TimeNextMadDriverChaseCreated = CGeneral::GetRandomNumberInRange(600.0f, 1200.0f);

    std::ranges::fill(apCarsToKeep, nullptr);
    // Como el original: limpia 11 grupos (0xC0EA28 hasta 0xC0EC22, paso 0x2E), uno mas que m_LoadedGangCars
    for (auto i = 0; i < 11; i++) {
        CPopulation::m_LoadedGangCars.data()[i].Clear();
    }
    CPopulation::m_AppropriateLoadedCars.Clear();
    CPopulation::m_InAppropriateLoadedCars.Clear();
    CPopulation::m_LoadedBoats.Clear();
}

// 0x4213B0
void CCarCtrl::ReInit() {
    CarDensityMultiplier = 1.0f;
    NumRandomCars = 0;
    NumLawEnforcerCars = 0;
    NumMissionCars = 0;
    NumParkedCars = 0;
    NumPermanentVehicles = 0;
    NumAmbulancesOnDuty = 0;
    NumFireTrucksOnDuty = 0;

    LastTimeLawEnforcerCreated = 0;

    bAllowEmergencyServicesToBeCreated = true;
    CountDownToCarsAtStart = 2;

    std::ranges::fill(apCarsToKeep, nullptr);
    // Como el original: limpia 11 grupos (0xC0EA28 hasta 0xC0EC22, paso 0x2E), uno mas que m_LoadedGangCars
    for (auto i = 0; i < 11; i++) {
        CPopulation::m_LoadedGangCars.data()[i].Clear();
    }
    CPopulation::m_AppropriateLoadedCars.Clear();
    CPopulation::m_InAppropriateLoadedCars.Clear();
    CPopulation::m_LoadedBoats.Clear();
}

// 0x421970
int32 CCarCtrl::ChooseBoatModel() {
    return CPopulation::m_LoadedBoats.PickLeastUsedModel(1);
}

// 0x421900
int32 CCarCtrl::ChooseCarModelToLoad(int32 groupID) {
    // Como el original: el numero de coches se lee con signo (movsx) y no se comprueba que sea > 0;
    // indice = _ftol2(n * (rand * 1/32768)) y el modelo se lee sin signo (movzx).
    const auto numCarsInGroup = static_cast<int32>(static_cast<int16>(CPopulation::m_nNumCarsInGroup.data()[groupID]));
    const auto* groups = reinterpret_cast<const uint16*>(&CPopulation::m_CarGroups);
    for (auto i = 0; i < 16; i++) { // 16 tries
        const float r = static_cast<float>(CGeneral::GetRandomNumber() & 0xFFFF) * std::bit_cast<float>(0x38000000u); // 0x858B14
        const auto idx = static_cast<int32>(static_cast<float>(numCarsInGroup) * r);
        const auto model = static_cast<int32>(groups[groupID * 23 + idx]);
        if (!CStreaming::IsModelLoaded(model)) {
            return model;
        }
    }
    return -1;
}

eModelID CCarCtrl::ChooseGangCarModel(eGangID loadedCarGroupId) {
    return CPopulation::PickGangCar(loadedCarGroupId);
}

// 0x424CE0
int32 CCarCtrl::ChooseModel(int32* arg1) {
    return plugin::CallAndReturn<int32, 0x424CE0, int32*>(arg1);
}

int32 CCarCtrl::ChoosePoliceCarModel(uint32 ignoreLvpd1Model) {
    // Como el original: FindPlayerWanted(-1) se llama antes de cada consulta
    if (FindPlayerWanted(-1)->AreSwatRequired()
        && CStreaming::IsModelLoaded(MODEL_ENFORCER)
        && CStreaming::IsModelLoaded(MODEL_SWAT)
    ) {
        if (CGeneral::GetRandomIntInRange(0, 3) == 2) // Como el original: 0x407180
            return MODEL_ENFORCER;
    }
    else
    {
        if (FindPlayerWanted(-1)->AreFbiRequired()
            && CStreaming::IsModelLoaded(MODEL_FBIRANCH)
            && CStreaming::IsModelLoaded(MODEL_FBI))
            return MODEL_FBIRANCH;

        if (FindPlayerWanted(-1)->AreArmyRequired()
            && CStreaming::IsModelLoaded(MODEL_RHINO)
            && CStreaming::IsModelLoaded(MODEL_BARRACKS)
            && CStreaming::IsModelLoaded(MODEL_ARMY))
            return (CGeneral::GetRandomNumber() < 0x3FFF) + MODEL_RHINO;
    }
    return CStreaming::GetDefaultCopCarModel(ignoreLvpd1Model);
}

// 0x423F00
#ifdef _DEBUG
#pragma optimize("", on)
#endif
void CCarCtrl::ClearInterestingVehicleList() {
    *reinterpret_cast<CVehicle* volatile*>(0x969084) = nullptr;
    *reinterpret_cast<CVehicle* volatile*>(0x969088) = nullptr;
}
#ifdef _DEBUG
#pragma optimize("", off)
#endif

// 0x422760
void CCarCtrl::ClitargetOrientationToLink(CVehicle* vehicle, CCarPathLinkAddress arg2, int8 arg3, float* arg4, float arg5, float arg6) {
    plugin::Call<0x422760, CVehicle*, CCarPathLinkAddress, int8, float*, float, float>(vehicle, arg2, arg3, arg4, arg5, arg6);
}

// 0x431F80
CVehicle* CCarCtrl::CreateCarForScript(int32 modelid, CVector posn, bool doMissionCleanup) {
    if (CModelInfo::IsBoatModel(modelid))
    {
        auto* boat = new CBoat(modelid, eVehicleCreatedBy::MISSION_VEHICLE);
        if (posn.z <= MAP_Z_LOW_LIMIT)
            posn.z = CWorld::FindGroundZForCoord(posn.x, posn.y);

        posn.z += boat->GetDistanceFromCentreOfMassToBaseOfModel();
        boat->SetPosn(posn);

        CTheScripts::ClearSpaceForMissionEntity(posn, boat);
        boat->vehicleFlags.bEngineOn = false;
        boat->vehicleFlags.bIsLocked = true;
        boat->SetStatus(STATUS_ABANDONED);
        JoinCarWithRoadSystem(boat);

        boat->m_autoPilot.SetCarMission(eCarMission::MISSION_NONE);
        boat->m_autoPilot.m_nTempAction = TEMPACT_NONE;
        boat->m_autoPilot.m_speed = 20.0F;
        boat->m_autoPilot.SetCruiseSpeed(20);

        if (doMissionCleanup)
            boat->m_bIsStaticWaitingForCollision = true;

        boat->m_autoPilot.movementFlags.bIsStopped = true;
        CWorld::Add(boat);

        if (doMissionCleanup)
            CTheScripts::MissionCleanUp.AddEntityToList(GetVehiclePool()->GetRef(boat), MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);

        return boat;
    }

    auto* vehicle = GetNewVehicleDependingOnCarModel(modelid, eVehicleCreatedBy::MISSION_VEHICLE);
    if (posn.z <= MAP_Z_LOW_LIMIT)
        posn.z = CWorld::FindGroundZForCoord(posn.x, posn.y);

    posn.z += vehicle->GetDistanceFromCentreOfMassToBaseOfModel();
    vehicle->SetPosn(posn);

    if (!doMissionCleanup)
    {
        if (vehicle->IsAutomobile())
            vehicle->AsAutomobile()->PlaceOnRoadProperly();
        else if (vehicle->IsBike())
            vehicle->AsBike()->PlaceOnRoadProperly();
    }

    if (vehicle->IsTrain())
        vehicle->AsTrain()->trainFlags.bNotOnARailRoad = true;

    CTheScripts::ClearSpaceForMissionEntity(posn, vehicle);
    vehicle->vehicleFlags.bIsLocked = true;
    vehicle->SetStatus(STATUS_ABANDONED);
    JoinCarWithRoadSystem(vehicle);
    vehicle->vehicleFlags.bEngineOn = false;
    vehicle->vehicleFlags.bHasBeenOwnedByPlayer = true;

    vehicle->m_autoPilot.SetCarMission(eCarMission::MISSION_NONE);
    vehicle->m_autoPilot.m_nTempAction = TEMPACT_NONE;
    vehicle->m_autoPilot.m_nCarDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS;
    vehicle->m_autoPilot.m_speed = 13.0F;
    vehicle->m_autoPilot.SetCruiseSpeed(13);
    vehicle->m_autoPilot.m_nCurrentLane = 0;
    vehicle->m_autoPilot.m_nNextLane = 0;

    if (doMissionCleanup)
        vehicle->m_bIsStaticWaitingForCollision = true;

    CWorld::Add(vehicle);
    if (doMissionCleanup)
        CTheScripts::MissionCleanUp.AddEntityToList(GetVehiclePool()->GetRef(vehicle), MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);

    if (vehicle->IsSubRoadVehicle())
        vehicle->m_autoPilot.movementFlags.bIsStopped = true;

    return vehicle;
}

// 0x42C740
bool CCarCtrl::CreateConvoy(CVehicle* vehicle, int32 arg2) {
    return plugin::CallAndReturn<bool, 0x42C740, CVehicle*, int32>(vehicle, arg2);
}

// 0x42C2B0
bool CCarCtrl::CreatePoliceChase(CVehicle* vehicle, int32 arg2, CNodeAddress NodeAddress) {
    return plugin::CallAndReturn<bool, 0x42C2B0, CVehicle*, int32, CNodeAddress>(vehicle, arg2, NodeAddress);
}

// 0x428040
bool CCarCtrl::DealWithBend_Racing(CVehicle* vehicle, CCarPathLinkAddress LinkAddress1, CCarPathLinkAddress LinkAddress2, CCarPathLinkAddress LinkAddress3, CCarPathLinkAddress LinkAddress4, char arg6, char arg7, char arg8, char arg9, float arg10, float* arg11, float* arg12, float* arg13, float* arg14, CVector* pos) {
    return plugin::CallAndReturn<bool, 0x428040, CVehicle*, CCarPathLinkAddress, CCarPathLinkAddress, CCarPathLinkAddress, CCarPathLinkAddress, int8, int8, int8, int8, float, float*, float*, float*, float*, CVector*>(vehicle, LinkAddress1, LinkAddress2, LinkAddress3, LinkAddress4, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, pos);
}

// 0x42EC90
void CCarCtrl::DragCarToPoint(CVehicle* vehicle, CVector* pos) {
    plugin::Call<0x42EC90, CVehicle*, CVector*>(vehicle, pos);
}

// 0x4325C0
float CCarCtrl::FindAngleToWeaveThroughTraffic(CVehicle* vehicle, CPhysical* physical, float arg3, float arg4, float arg5) {
    return plugin::CallAndReturn<float, 0x4325C0, CVehicle*, CPhysical*, float, float, float>(vehicle, physical, arg3, arg4, arg5);
}

// 0x4226F0
void CCarCtrl::FindIntersection2Lines(float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float* arg9, float* arg10) {
    plugin::Call<0x4226F0, float, float, float, float, float, float, float, float, float*, float*>(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

// 0x42B470
void CCarCtrl::FindLinksToGoWithTheseNodes(CVehicle* vehicle) {
    plugin::Call<0x42B470, CVehicle*>(vehicle);
}

// 0x434400
float CCarCtrl::FindMaximumSpeedForThisCarInTraffic(CVehicle* vehicle) {
    return plugin::CallAndReturn<float, 0x434400, CVehicle*>(vehicle);
}

// 0x42BD20
void CCarCtrl::FindNodesThisCarIsNearestTo(CVehicle* vehicle, CNodeAddress& nodeAddress1, CNodeAddress& nodeAddress2) {
    plugin::Call<0x42BD20, CVehicle*, CNodeAddress&, CNodeAddress&>(vehicle, nodeAddress1, nodeAddress2);
}

// 0x422090
int8 CCarCtrl::FindPathDirection(CNodeAddress nodeAddress1, CNodeAddress nodeAddress2, CNodeAddress nodeAddress3, bool* arg4) {
    return plugin::CallAndReturn<int8, 0x422090, CNodeAddress, CNodeAddress, CNodeAddress, bool*>(nodeAddress1, nodeAddress2, nodeAddress3, arg4);
}

// 0x422620
float CCarCtrl::FindPercDependingOnDistToLink(CVehicle* vehicle, CCarPathLinkAddress linkAddress) {
    return plugin::CallAndReturn<float, 0x422620, CVehicle*, CCarPathLinkAddress>(vehicle, linkAddress);
}

// 0x421770
int32 CCarCtrl::FindSequenceElement(int32 arg1) {
    // 0x421770..0x4217A8: read the flag's byte representation, wrap
    // additions/subtractions to 32 bits, then take the signed remainder.
    const auto otherWay = *reinterpret_cast<const uint8*>(&bSequenceOtherWay);
    const auto length = SequenceElements;
    const auto offset = static_cast<uint32>(SequenceRandomOffset);
    const auto argument = static_cast<uint32>(arg1);
    const uint32 numerator = otherWay
        ? argument + offset
        : static_cast<uint32>(length) - argument + offset;
    return std::bit_cast<int32>(numerator) % length;
}

// 0x4224E0
float CCarCtrl::FindSpeedMultiplier(float arg1, float arg2, float arg3, float arg4) {
    return plugin::CallAndReturn<float, 0x4224E0, float, float, float, float>(arg1, arg2, arg3, arg4);
}

// 0x424130
float CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes(int8 arg1) {
    switch (static_cast<uint8>(arg1)) {
    case 0xFF: return 0.5f;
    case 0: return std::bit_cast<float>(0x3F266666u);
    case 2: return std::bit_cast<float>(0x40133333u);
    default: return 1.0f;
    }
}

static uintptr_t GhostPathNodePositionBody=std::bit_cast<uintptr_t>(&CPathNode::GetPosition);
__declspec(naked) float CCarCtrl::FindGhostRoadHeight(CVehicle*) {
    __asm {
    sub esp,18h
    push edi
    mov edi,dword ptr [esp+20h]
    mov eax,0FFFFh
    cmp word ptr [edi+00000390h],ax
    je L0042238F
    cmp word ptr [edi+00000394h],ax
    jne L0042239A
    L0042238F: fld dword ptr ds:[00858B50h]
    pop edi
    add esp,18h
    ret
    L0042239A: push ebp
    mov ebp,dword ptr [edi+00000390h]
    movzx eax,bp
    mov ecx,dword ptr [eax*4+0096F854h]
    test ecx,ecx
    push esi
    jne L004223BD
    L004223B0: fld dword ptr ds:[00858B50h]
    pop esi
    pop ebp
    pop edi
    add esp,18h
    ret
    L004223BD: mov esi,dword ptr [edi+00000394h]
    movzx ecx,si
    mov eax,dword ptr [ecx*4+0096F854h]
    test eax,eax
    mov dword ptr [esp+0Ch],esi
    je L004223B0
    mov ecx,ebp
    shr ecx,10h
    imul ecx,ecx,1Ch
    movzx eax,bp
    push ebx
    mov ebx,dword ptr [eax*4+0096F854h]
    lea edx,[esp+1Ch]
    push edx
    add ecx,ebx
    mov dword ptr [esp+30h],ebp
    call dword ptr [GhostPathNodePositionBody]
    mov ecx,dword ptr [eax+8]
    mov dword ptr [esp+18h],ecx
    mov ecx,esi
    shr ecx,10h
    imul ecx,ecx,1Ch
    mov eax,esi
    mov dword ptr [esp+2Ch],eax
    movzx eax,ax
    lea edx,[esp+1Ch]
    push edx
    add ecx,dword ptr [eax*4+0096F854h]
    call dword ptr [GhostPathNodePositionBody]
    mov esi,dword ptr [edi+14h]
    test esi,esi
    mov ecx,dword ptr [eax+8]
    mov dword ptr [esp+14h],ecx
    lea ebx,[esi+30h]
    jne L00422435
    lea ebx,[edi+4]
    L00422435: mov ecx,ebp
    shr ecx,10h
    imul ecx,ecx,1Ch
    movzx eax,bp
    lea edx,[esp+1Ch]
    push edx
    add ecx,dword ptr [eax*4+0096F854h]
    mov dword ptr [esp+30h],ebp
    call dword ptr [GhostPathNodePositionBody]
    test esi,esi
    fld dword ptr [eax]
    fsub dword ptr [ebx]
    fld dword ptr [eax+4]
    fsub dword ptr [ebx+4]
    pop ebx
    fld st(0)
    fmul st,st(1)
    fld st(2)
    fmul st,st(3)
    faddp st(1),st
    fsqrt
    fstp st(2)
    fstp st(0)
    fstp dword ptr [esp+28h]
    je L0042247D
    add esi,30h
    jmp L00422480
    L0042247D: lea esi,[edi+4]
    L00422480: mov eax,dword ptr [esp+0Ch]
    lea ecx,[esp+18h]
    push ecx
    mov ecx,eax
    shr ecx,10h
    imul ecx,ecx,1Ch
    movzx edx,ax
    mov dword ptr [esp+10h],eax
    add ecx,dword ptr [edx*4+0096F854h]
    call dword ptr [GhostPathNodePositionBody]
    fld dword ptr [eax]
    fsub dword ptr [esi]
    fld dword ptr [eax+4]
    fsub dword ptr [esi+4]
    pop esi
    fld st(0)
    pop ebp
    fmul st,st(1)
    pop edi
    fld st(2)
    fmul st,st(3)
    faddp st(1),st
    fsqrt
    fstp st(2)
    fstp st(0)
    fld dword ptr [esp+1Ch]
    fmul dword ptr [esp+4]
    fld st(1)
    fmul dword ptr [esp+8]
    faddp st(1),st
    fxch st(1)
    fadd dword ptr [esp+1Ch]
    fdivp st(1),st
    add esp,18h
    ret
    }
}

// 0x42B270
void CCarCtrl::FireHeliRocketsAtTarget(CAutomobile* entityLauncher, CEntity* entity) {
    plugin::Call<0x42B270, CAutomobile*, CEntity*>(entityLauncher, entity);
}

// 0x429A70
void CCarCtrl::FlyAIHeliInCertainDirection(CHeli* heli, float arg2, float arg3, bool arg4) {
    plugin::Call<0x429A70, CHeli*, float, float, bool>(heli, arg2, arg3, arg4);
}

// 0x423940
void CCarCtrl::FlyAIHeliToTarget_FixedOrientation(CHeli* heli, float Orientation, CVector posn) {
    plugin::Call<0x423940, CHeli*, float, CVector>(heli, Orientation, posn);
}

// 0x423000
void CCarCtrl::FlyAIPlaneInCertainDirection(CPlane* pPlane) {
    plugin::Call<0x423000, CPlane*>(pPlane);
}

// 0x424210
bool CCarCtrl::GenerateCarCreationCoors2(CVector posn, float radius, float arg3, float arg4, bool arg5, float arg6, float arg7, CVector* pOrigin, CNodeAddress* pNodeAddress1, CNodeAddress* pNodeAddress12, float* arg11, bool arg12, bool arg13) {
    return plugin::CallAndReturn<bool, 0x424210, CVector, float, float, float, bool, float, float, CVector*, CNodeAddress*, CNodeAddress*, float*, bool, bool>(posn, radius, arg3, arg4, arg5, arg6, arg7, pOrigin, pNodeAddress1, pNodeAddress12, arg11, arg12, arg13);
}

// 0x42F9C0
void CCarCtrl::GenerateEmergencyServicesCar() {
    plugin::Call<0x42F9C0>();
}

// 0x42B7D0
CAutomobile* CCarCtrl::GenerateOneEmergencyServicesCar(uint32 modelId, CVector posn) {
    return plugin::CallAndReturn<CAutomobile*, 0x42B7D0, uint32, CVector>(modelId, posn);
}

// 0x430050
void CCarCtrl::GenerateOneRandomCar() {
    plugin::Call<0x430050>();
}

// 0x4341C0
void CCarCtrl::GenerateRandomCars() {
    if (CCutsceneMgr::ms_running) {
        CountDownToCarsAtStart = 2;
        return;
    }
    // Como el original: solo en el mundo exterior (currArea == 0)
    if (CGangWars::DontCreateCivilians() || CGame::currArea != AREA_CODE_NORMAL_WORLD) {
        return;
    }

    if (CGameLogic::LaRiotsActiveHere() && TimeNextMadDriverChaseCreated > 480.0f) {
        TimeNextMadDriverChaseCreated = CGeneral::GetRandomNumberInRange(240.0f, 480.0f);
    }
    TimeNextMadDriverChaseCreated -= (CTimer::GetTimeStep() * 0.02f);

    if (NumRandomCars < 45) {
        if (CountDownToCarsAtStart) {
            // Como el original: los 100 coches solo se generan cuando la cuenta atras llega a 0
            if (--CountDownToCarsAtStart == 0) {
                for (auto i = 100; i --> 0;) {
                    GenerateOneRandomCar();
                }
                CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter = 20;
            }
        } else {
            GenerateOneRandomCar();
            GenerateOneRandomCar();
        }
    }

    // Como el original: trenes, aviones y servicios de emergencia (cada 4096 ms)
    CTrain::DoTrainGenerationAndRemoval();
    CPlane::DoPlaneGenerationAndRemoval();
    if ((CTimer::GetTimeInMS() ^ CTimer::GetPreviousTimeInMS()) & 0xFFFFF000) {
        GenerateEmergencyServicesCar();
    }

}

// 0x42F3C0
void CCarCtrl::GetAIHeliToAttackPlayer(CAutomobile* automobile) {
    plugin::Call<0x42F3C0, CAutomobile*>(automobile);
}

// 0x42A730
void CCarCtrl::GetAIHeliToFlyInDirection(CAutomobile* automobile) {
    plugin::Call<0x42A730, CAutomobile*>(automobile);
}

// 0x429780
void CCarCtrl::GetAIPlaneToAttackPlayer(CAutomobile* automobile) {
    plugin::Call<0x429780, CAutomobile*>(automobile);
}

// 0x429890
void CCarCtrl::GetAIPlaneToDoDogFight(CAutomobile* automobile) {
    plugin::Call<0x429890, CAutomobile*>(automobile);
}

// 0x42F370
void CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer(CAutomobile* automobile) {
    plugin::Call<0x42F370, CAutomobile*>(automobile);
}

// 0x421440
CVehicle* CCarCtrl::GetNewVehicleDependingOnCarModel(int32 modelId, uint8 createdBy) {
    const auto create = [=]<typename T, typename... Args>(Args... args) -> T* {
        auto* memory = CVehicle::operator new(sizeof(T));
        return memory ? ::new (memory) T(modelId, eVehicleCreatedBy(createdBy), args...) : nullptr;
    };
    switch (CModelInfo::GetModelInfo(modelId)->AsVehicleModelInfoPtr()->m_nVehicleType) {
    case VEHICLE_TYPE_MTRUCK:  return create.operator()<CMonsterTruck>();
    case VEHICLE_TYPE_QUAD:    return create.operator()<CQuadBike>();
    case VEHICLE_TYPE_HELI:    return create.operator()<CHeli>();
    case VEHICLE_TYPE_PLANE:   return create.operator()<CPlane>();
    case VEHICLE_TYPE_BOAT:    return create.operator()<CBoat>();
    case VEHICLE_TYPE_TRAIN:   return create.operator()<CTrain>();
    case VEHICLE_TYPE_TRAILER: return create.operator()<CTrailer>();
    case VEHICLE_TYPE_BIKE: {
        auto* bike = create.operator()<CBike>();
        bike->bikeFlags.bOnSideStand = true;
        return bike;
    }
    case VEHICLE_TYPE_BMX: {
        auto* bike = create.operator()<CBmx>();
        bike->bikeFlags.bOnSideStand = true;
        return bike;
    }
    default: return create.operator()<CAutomobile>(true);
    }
}

// 0x42C250
bool CCarCtrl::IsAnyoneParking() {
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        switch (veh.m_autoPilot.m_nCarMission) {
        case eCarMission::MISSION_PARK_PARALLEL:
        case eCarMission::MISSION_PARK_PARALLEL_2:
        case eCarMission::MISSION_PARK_PERPENDICULAR:
        case eCarMission::MISSION_PARK_PERPENDICULAR_2:
            return true;
        }
    }
    return false;
}

// 0x42DAB0
bool CCarCtrl::IsThisAnAppropriateNode(CVehicle* vehicle, CNodeAddress nodeAddress1, CNodeAddress nodeAddress2, CNodeAddress nodeAddress3, bool arg5) {
    return plugin::CallAndReturn<bool, 0x42DAB0, CVehicle*, CNodeAddress, CNodeAddress, CNodeAddress, bool>(vehicle, nodeAddress1, nodeAddress2, nodeAddress3, arg5);
}

// 0x423EA0
bool CCarCtrl::IsThisVehicleInteresting(CVehicle* vehicle) {
    for (auto& car : apCarsToKeep) {
        if (car == vehicle) {
            return true;
        }
    }
    return false;
}

// 0x432CB0
void CCarCtrl::JoinCarWithRoadAccordingToMission(CVehicle* vehicle) {
    switch (vehicle->m_autoPilot.m_nCarMission) {
    case MISSION_NONE:
    case MISSION_CRUISE:
    case MISSION_WAITFORDELETION:
    case MISSION_EMERGENCYVEHICLE_STOP:
    case MISSION_STOP_FOREVER:
    case MISSION_FOLLOW_RECORDED_PATH:
    case MISSION_PARK_PERPENDICULAR:
    case MISSION_PARK_PARALLEL:
    case MISSION_PARK_PERPENDICULAR_2:
    case MISSION_PARK_PARALLEL_2:
        return JoinCarWithRoadSystem(vehicle);
    case MISSION_RAMPLAYER_FARAWAY:
    case MISSION_RAMPLAYER_CLOSE:
    case MISSION_BLOCKPLAYER_FARAWAY:
    case MISSION_BLOCKPLAYER_CLOSE:
    case MISSION_BLOCKPLAYER_HANDBRAKESTOP:
    case MISSION_BOAT_ATTACKPLAYER:
    case MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1:
    case MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2:
    case MISSION_BLOCKPLAYER_FORWARDANDBACK:
    case MISSION_APPROACHPLAYER_FARAWAY:
    case MISSION_APPROACHPLAYER_CLOSE:
    case MISSION_BOAT_CIRCLEPLAYER: {
        JoinCarWithRoadSystemGotoCoors(vehicle, FindPlayerCoors(-1), true, vehicle->IsSubBoat());
        break;
    }
    case MISSION_GOTOCOORDINATES:
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE:
    case MISSION_GOTOCOORDINATES_ACCURATE:
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE:
    case MISSION_GOTOCOORDINATES_ASTHECROWSWIMS:
    case MISSION_GOTOCOORDINATES_RACING: {
        JoinCarWithRoadSystemGotoCoors(vehicle, vehicle->m_autoPilot.m_vecDestinationCoors, true, vehicle->IsSubBoat());
        break;
    }
    case MISSION_RAMCAR_FARAWAY:
    case MISSION_RAMCAR_CLOSE:
    case MISSION_BLOCKCAR_FARAWAY:
    case MISSION_BLOCKCAR_CLOSE:
    case MISSION_BLOCKCAR_HANDBRAKESTOP:
    case MISSION_PROTECTION_REAR:
    case MISSION_PROTECTION_FRONT:
    case MISSION_ESCORT_LEFT:
    case MISSION_ESCORT_RIGHT:
    case MISSION_ESCORT_REAR:
    case MISSION_ESCORT_FRONT:
    case MISSION_FOLLOWCAR_FARAWAY:
    case MISSION_FOLLOWCAR_CLOSE:
    case MISSION_KILLPED_FARAWAY:
    case MISSION_KILLPED_CLOSE:
    case MISSION_DO_DRIVEBY_CLOSE:
    case MISSION_DO_DRIVEBY_FARAWAY:
    case MISSION_ESCORT_LEFT_FARAWAY:
    case MISSION_ESCORT_RIGHT_FARAWAY:
    case MISSION_ESCORT_REAR_FARAWAY:
    case MISSION_ESCORT_FRONT_FARAWAY: {
        // Como el original: sin entidad objetivo no hace nada
        if (vehicle->m_autoPilot.m_TargetEntity) {
            JoinCarWithRoadSystemGotoCoors(vehicle, vehicle->m_autoPilot.m_TargetEntity->GetPosition(), true, vehicle->IsSubBoat());
        }
        break;
    }
    }
}

// 0x42F5A0
void CCarCtrl::JoinCarWithRoadSystem(CVehicle* vehicle) {
    plugin::Call<0x42F5A0, CVehicle*>(vehicle);
}

// 0x42F870
bool CCarCtrl::JoinCarWithRoadSystemGotoCoors(CVehicle* vehicle, const CVector& posn, bool unused, bool bIsBoat) {
    return plugin::CallAndReturn<bool, 0x42F870, CVehicle*, const CVector&, bool, bool>(vehicle, posn, unused, bIsBoat);
}

// 0x432B10
bool CCarCtrl::PickNextNodeAccordingStrategy(CVehicle* vehicle) {
    return plugin::CallAndReturn<bool, 0x432B10, CVehicle*>(vehicle);
}

// 0x421740
void CCarCtrl::InitSequence(int32 numSequenceElements) {
    SequenceElements = numSequenceElements;
    SequenceRandomOffset = CGeneral::GetRandomNumber() % numSequenceElements;
    bSequenceOtherWay = (CGeneral::GetRandomNumber() >> 4) & 1; // Como el original: shr al,4 / and al,1 (bit 4)
}

// 0x42DE80
void CCarCtrl::PickNextNodeRandomly(CVehicle* vehicle) {
    plugin::Call<0x42DE80, CVehicle*>(vehicle);
}

// 0x426EF0
bool CCarCtrl::PickNextNodeToChaseCar(CVehicle* vehicle, float destX, float destY, float destZ) {
    return plugin::CallAndReturn<bool, 0x426EF0, CVehicle*, float, float, float>(vehicle, destX, destY, destZ);
}

// 0x427740
bool CCarCtrl::PickNextNodeToFollowPath(CVehicle* vehicle) {
    return plugin::CallAndReturn<bool, 0x427740, CVehicle*>(vehicle);
}

// 0x429600
void CCarCtrl::PossiblyFireHSMissile(CVehicle* entityLauncher, CEntity* targetEntity) {
    if (!targetEntity)
        return;

    if (CTimer::GetTimeInMS() / 2000u == CTimer::GetPreviousTimeInMS() / 2000u)
        return;

    const CVector& launcherPos = entityLauncher->GetPosition();
    const CVector& targetPos = targetEntity->GetPosition();
    CVector dir;
    dir.z = targetPos.z - launcherPos.z;
    dir.y = targetPos.y - launcherPos.y;
    dir.x = targetPos.x - launcherPos.x;
    const float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    if (dist < 160.0f && dist > 30.0f) {
        // Como el original: usa m_matrix directamente, normaliza dir (0x59C910) y el producto escalar es (z + y) + x
        const CMatrix& launcherMat = *entityLauncher->m_matrix;
        dir.Normalise();
        const float dot = dir.z * launcherMat.GetForward().z + dir.y * launcherMat.GetForward().y + dir.x * launcherMat.GetForward().x;
        if (dot > 0.8f) {
            CProjectileInfo::AddProjectile(
                entityLauncher,
                eWeaponType::WEAPON_ROCKET_HS,
                entityLauncher->GetPosition() + launcherMat.GetForward() * 4.0f - launcherMat.GetUp() * 3.0f,
                1.0f,
                &launcherMat.GetForward(),
                targetEntity
            );
        }
    }
}

// 0x424F80
void CCarCtrl::PossiblyRemoveVehicle(CVehicle* vehicle) {
    plugin::Call<0x424F80, CVehicle*>(vehicle);
}

// 0x423F10
void CCarCtrl::PruneVehiclesOfInterest() {
    ZoneScoped;

    if ((CTimer::GetFrameCounter() % 64) == 19 && FindPlayerCoors(-1).z < 950.0f) {
        for (size_t i = 0; i < std::size(apCarsToKeep); i++) {
            if (apCarsToKeep[i]) {
                if (CTimer::GetTimeInMS() > aCarsToKeepTime[i] + 180000) {
                    apCarsToKeep[i] = nullptr;
                }
            }
        }
    }
}

// 0x42FC40
void CCarCtrl::ReconsiderRoute(CVehicle* vehicle) {
    plugin::Call<0x42FC40, CVehicle*>(vehicle);
}

// 0x423DE0
void CCarCtrl::RegisterVehicleOfInterest(CVehicle* vehicle) {
    // 0x423DF0..0x423E98: renueva duplicados; después primer hueco o timestamp mínimo sin signo.
    for (size_t i = 0; i < 2; ++i) {
        if (apCarsToKeep[i] == vehicle) {
            aCarsToKeepTime[i] = CTimer::GetTimeInMS();
            return;
        }
    }
    for (size_t i = 0; i < 2; ++i) {
        if (!apCarsToKeep[i]) {
            apCarsToKeep[i] = vehicle;
            aCarsToKeepTime[i] = CTimer::GetTimeInMS();
            return;
        }
    }
    size_t oldest = 0;
    uint32 oldestTime = UINT32_MAX;
    for (size_t i = 0; i < 2; ++i) {
        if (apCarsToKeep[i] && aCarsToKeepTime[i] < oldestTime) {
            oldest = i;
            oldestTime = aCarsToKeepTime[i];
        }
    }
    apCarsToKeep[oldest] = vehicle;
    aCarsToKeepTime[oldest] = CTimer::GetTimeInMS();
}

// 0x4322B0
void CCarCtrl::RemoveCarsIfThePoolGetsFull() {
    ZoneScoped;

    if (CTimer::GetFrameCounter() % 8 != 3)
        return;

    if (GetVehiclePool()->GetNoOfFreeSpaces() >= 8)
        return;

    // Find closest deletable vehicle
    // Como el original: recorre el pool de atras hacia delante, empieza con 10000000.0 y la distancia es (dz^2 + dx^2) + dy^2
    const CVector& camPos = TheCamera.GetPosition();
    float fClosestDist = 10000000.0f;
    CVehicle* closestVeh = nullptr;
    auto* const pool = GetVehiclePool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const pveh = pool->GetAt(i);
        if (!pveh) {
            continue;
        }
        auto& veh = *pveh;
        if (IsThisVehicleInteresting(&veh)) {
            continue;
        }
        if (veh.vehicleFlags.bIsLocked) {
            continue;
        }
        if (!veh.CanBeDeleted()) {
            continue;
        }
        if (CCranes::IsThisCarBeingTargettedByAnyCrane(&veh)) {
            continue;
        }

        const CVector& vehPos = veh.GetPosition();
        const float dx = camPos.x - vehPos.x;
        const float dy = camPos.y - vehPos.y;
        const float dz = camPos.z - vehPos.z;
        const float fCamVehDist = std::sqrt(dz * dz + dx * dx + dy * dy);
        if (fCamVehDist < fClosestDist) {
            fClosestDist = fCamVehDist;
            closestVeh   = &veh;
        }
    }
    if (closestVeh) {
        CWorld::Remove(closestVeh);
        delete closestVeh;
    }
}

// 0x42CD10
void CCarCtrl::RemoveDistantCars() {
    ZoneScoped;

    // Como el original: el pool se recorre de atras hacia delante
    auto* const pool = GetVehiclePool();

    // FIXBUGS: First remove vehicles that can be removed
    if (notsa::bugfixes::CCarCtrl_RemoveDistantCars_UseAfterFree) {
        for (auto i = pool->GetSize(); i-- > 0;) {
            if (auto* const veh = pool->GetAt(i)) {
                PossiblyRemoveVehicle(veh);
            }
        }
    }

    //... only then process them, this way we don't do use-after-free
    // only other solution would be `PossiblyRemoveVehicle` returning a `bool`
    // to indicate whenever the vehicle was deleted or not.
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const pveh = pool->GetAt(i);
        if (!pveh) {
            continue;
        }
        auto& veh = *pveh;
        if (!notsa::bugfixes::CCarCtrl_RemoveDistantCars_UseAfterFree) {
            PossiblyRemoveVehicle(&veh); // This may or may not invalidate `veh`
        }
        if (!veh.vehicleFlags.bCreateRoadBlockPeds) {
            continue;
        }
        // Como el original: centro del jugador con CWorld::PlayerInFocus y distancia 2D sqrt(dy^2 + dx^2)
        const CVector centre = FindPlayerCentreOfWorld(static_cast<int32>(CWorld::PlayerInFocus));
        const CVector& vehPos = veh.GetPosition();
        const float dy = vehPos.y - centre.y;
        const float dx = vehPos.x - centre.x;
        if (!(std::sqrt(dy * dy + dx * dx) < 54.5f)) {
            continue;
        }
        CRoadBlocks::GenerateRoadBlockPedsForCar(
            &veh,
            veh.m_nPedsPositionForRoadBlock,
            veh.IsLawEnforcementVehicle() ? PED_TYPE_COP : PED_TYPE_GANG1
        );
        veh.vehicleFlags.bCreateRoadBlockPeds = false;
    }
}

// 0x423ED0
void CCarCtrl::RemoveFromInterestingVehicleList(CVehicle* vehicle) {
    for (auto& car : apCarsToKeep) {
        if (car == vehicle) {
            car = nullptr; // Como el original: sin break, limpia todas las coincidencias
        }
    }
}

// 0x42CE40
void CCarCtrl::ScanForPedDanger(CVehicle* vehicle) {
    plugin::Call<0x42CE40, CVehicle*>(vehicle);
}

// 0x42FBC0
bool CCarCtrl::ScriptGenerateOneEmergencyServicesCar(uint32 modelId, CVector posn) {
    if (CStreaming::IsModelLoaded(modelId)) {
        if (auto pAuto = GenerateOneEmergencyServicesCar(modelId, posn)) {
            pAuto->m_autoPilot.m_vecDestinationCoors = posn;
            pAuto->m_autoPilot.SetCarMission(JoinCarWithRoadSystemGotoCoors(pAuto, posn, false, false) ? MISSION_GOTOCOORDINATES_STRAIGHTLINE : MISSION_GOTOCOORDINATES);
            return true;
        }
    }
    return false;
}

// 0x4342A0
void CCarCtrl::SetCoordsOfScriptCar(CVehicle* vehicle, float x, float y, float z, uint8 arg5, uint8 arg6) {
    plugin::Call<0x4342A0, CVehicle*, float, float, float, uint8, uint8>(vehicle, x, y, z, arg5, arg6);
}

// 0x4217C0
void CCarCtrl::SetUpDriverAndPassengersForVehicle(CVehicle* vehicle, int32 arg2, int32 arg3, bool arg4, bool arg5, int32 passengersNum) {
    plugin::Call<0x4217C0, CVehicle*, int32, int32, bool, bool, int32>(vehicle, arg2, arg3, arg4, arg5, passengersNum);
}

// 0x432420
template<typename PtrListType>
void CCarCtrl::SlowCarDownForCarsSectorList(PtrListType& ptrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8) {
    plugin::Call<0x432420, PtrListType&, CVehicle*, float, float, float, float, float*, float>(ptrList, vehicle, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x426220
void CCarCtrl::SlowCarDownForObject(CEntity* entity, CVehicle* vehicle, float* arg3, float arg4) {
    // Como el original: m_matrix del vehiculo sin comprobar, productos escalares (z + y) + x, col models del model info
    // (sin col model especial) y *arg3 = min(*arg3, (1 - (h - 7) * 0.07692308) * arg4).
    const CVector& entityPos = entity->GetPosition();
    const CVector& vehiclePos = vehicle->GetPosition();
    const float dx = entityPos.x - vehiclePos.x;
    const float dy = entityPos.y - vehiclePos.y;
    const float dz = entityPos.z - vehiclePos.z;
    const CMatrix& mat = *vehicle->m_matrix;
    const float entityHeading = dz * mat.GetForward().z + dy * mat.GetForward().y + dx * mat.GetForward().x;
    if (!(entityHeading > 0.0f) || !(entityHeading < 20.0f)) {
        return;
    }
    const float side = std::fabs(dz * mat.GetRight().z + dy * mat.GetRight().y + dx * mat.GetRight().x);
    const float radius = CModelInfo::GetModelInfo(static_cast<int16>(entity->m_nModelIndex))->GetColModel()->GetBoundRadius()
                       + CModelInfo::GetModelInfo(static_cast<int16>(vehicle->m_nModelIndex))->GetColModel()->GetBoundingBox().m_vecMax.x;
    if (!(radius > side)) {
        return;
    }
    if (entityHeading < 7.0f) {
        *arg3 = 0.0f;
        return;
    }
    const float t = (1.0f - (entityHeading - 7.0f) * std::bit_cast<float>(0x3D9D89D9u)) * arg4; // 0x858FA4
    *arg3 = *arg3 < t ? *arg3 : t;
}

// 0x42D4F0
template<typename PtrListType>
void CCarCtrl::SlowCarDownForObjectsSectorList(PtrListType& ptrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8) {
    plugin::Call<0x42D4F0, PtrListType&, CVehicle*, float, float, float, float, float*, float>(ptrList, vehicle, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x42D0E0
void CCarCtrl::SlowCarDownForOtherCar(CEntity* car1, CVehicle* car2, float* arg3, float arg4) {
    plugin::Call<0x42D0E0, CEntity*, CVehicle*, float*, float>(car1, car2, arg3, arg4);
}

// 0x425440
template<typename PtrListType>
void CCarCtrl::SlowCarDownForPedsSectorList(PtrListType& PtrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8) {
    plugin::Call<0x425440, PtrListType&, CVehicle*, float, float, float, float, float*, float>(PtrList, vehicle, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x434790
void CCarCtrl::SlowCarOnRailsDownForTrafficAndLights(CVehicle* vehicle) {
    auto& autoPilot = vehicle->m_autoPilot;

    if ((((int8)CTimer::GetFrameCounter() + (int8)(vehicle->m_nRandomSeed)) & 3) == 0) {
        if (CTrafficLights::ShouldCarStopForLight(vehicle, false) || CTrafficLights::ShouldCarStopForBridge(vehicle)) {
            CCarAI::CarHasReasonToStop(vehicle);
            autoPilot.m_fMaxTrafficSpeed = 0.0f;
        } else {
            autoPilot.m_fMaxTrafficSpeed = FindMaximumSpeedForThisCarInTraffic(vehicle);
        }
    }

    // Como el original: con velocidad maxima == velocidad no se llama a ModifySpeed; comparaciones NaN como el x87
    const float maxSpeed = autoPilot.m_fMaxTrafficSpeed;
    if (!(maxSpeed < autoPilot.m_speed)) {
        if (maxSpeed > autoPilot.m_speed) {
            const float newSpeed = CTimer::GetTimeStep() * 0.05f + autoPilot.m_speed;
            autoPilot.ModifySpeed(newSpeed < maxSpeed ? newSpeed : maxSpeed);
        }
        return;
    }
    if (autoPilot.m_speed == 0.0f) {
        return;
    }
    if (autoPilot.m_speed < 0.1f) {
        autoPilot.ModifySpeed(0.0f);
        return;
    }
    const float newSpeed = autoPilot.m_speed - CTimer::GetTimeStep() * 0.7f;
    autoPilot.ModifySpeed(newSpeed > maxSpeed ? newSpeed : maxSpeed);
}

// 0x428DE0
void CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x428DE0, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x429090
void CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x429090, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x428BE0
void CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget(CVehicle* vehicle, float arg2, float arg3, float* arg4, float* arg5, float* arg6) {
    plugin::Call<0x428BE0, CVehicle*, float, float, float*, float*, float*>(vehicle, arg2, arg3, arg4, arg5, arg6);
}

// 0x422B20
void CCarCtrl::SteerAICarBlockingPlayerForwardAndBack(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x422B20, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x433BA0
void CCarCtrl::SteerAICarParkParallel(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x433BA0, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x433EA0
void CCarCtrl::SteerAICarParkPerpendicular(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x433EA0, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x4336D0
void CCarCtrl::SteerAICarTowardsPointInEscort(CVehicle* vehicle1, CVehicle* vehicle2, float arg3, float arg4, float* arg5, float* arg6, float* arg7, bool* arg8) {
    plugin::Call<0x4336D0, CVehicle*, CVehicle*, float, float, float*, float*, float*, bool*>(vehicle1, vehicle2, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x437C20
void CCarCtrl::SteerAICarWithPhysics(CVehicle* vehicle) {
    plugin::Call<0x437C20, CVehicle*>(vehicle);
}

// 0x434900
void CCarCtrl::SteerAICarWithPhysicsFollowPath(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x434900, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x435830
void CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x435830, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x432DD0
void CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x432DD0, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x433280
void CCarCtrl::SteerAICarWithPhysicsHeadingForTarget(CVehicle* vehicle, CPhysical* target, float arg3, float arg4, float* arg5, float* arg6, float* arg7, bool* arg8) {
    plugin::Call<0x433280, CVehicle*, CPhysical*, float, float, float*, float*, float*, bool*>(vehicle, target, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x4335E0
void CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget(CVehicle* vehicle, CEntity* Unusued, float arg3, float arg4, float arg5, float arg6, float* arg7, float* arg8, float* arg9, bool* arg10) {
    plugin::Call<0x4335E0, CVehicle*, CEntity*, float, float, float, float, float*, float*, float*, bool*>(vehicle, Unusued, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

// 0x428990
void CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop(CVehicle* vehicle, float x, float y, float arg4, float arg5, float* arg6, float* arg7, float* arg8, bool* arg9) {
    plugin::Call<0x428990, CVehicle*, float, float, float, float, float*, float*, float*, bool*>(vehicle, x, y, arg4, arg5, arg6, arg7, arg8, arg9);
}

// 0x436A90
void CCarCtrl::SteerAICarWithPhysics_OnlyMission(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x436A90, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x42AAD0
void CCarCtrl::SteerAIHeliAsPoliceHeli(CAutomobile* automobile) {
    plugin::Call<0x42AAD0, CAutomobile*>(automobile);
}

// 0x42ACB0
void CCarCtrl::SteerAIHeliFlyingAwayFromPlayer(CAutomobile* automobile) {
    plugin::Call<0x42ACB0, CAutomobile*>(automobile);
}

// 0x4238E0
void CCarCtrl::SteerAIHeliToCrashAndBurn(CAutomobile* automobile) {
    plugin::Call<0x4238E0, CAutomobile*>(automobile);
}

// 0x42A750
void CCarCtrl::SteerAIHeliToFollowEntity(CAutomobile* automobile) {
    plugin::Call<0x42A750, CAutomobile*>(automobile);
}

// 0x42AEB0
void CCarCtrl::SteerAIHeliToKeepEntityInView(CAutomobile* automobile) {
    plugin::Call<0x42AEB0, CAutomobile*>(automobile);
}

// 0x42AD30
void CCarCtrl::SteerAIHeliToLand(CAutomobile* automobile) {
    plugin::Call<0x42AD30, CAutomobile*>(automobile);
}

// 0x42A630
void CCarCtrl::SteerAIHeliTowardsTargetCoors(CAutomobile* automobile) {
    plugin::Call<0x42A630, CAutomobile*>(automobile);
}

// 0x423880
void CCarCtrl::SteerAIPlaneToCrashAndBurn(CAutomobile* automobile) {
    plugin::Call<0x423880, CAutomobile*>(automobile);
}

// 0x4237F0
void CCarCtrl::SteerAIPlaneToFollowEntity(CAutomobile* automobile) {
    plugin::Call<0x4237F0, CAutomobile*>(automobile);
}

// 0x423790
void CCarCtrl::SteerAIPlaneTowardsTargetCoors(CAutomobile* automobile) {
    plugin::Call<0x423790, CAutomobile*>(automobile);
}

// 0x422590
bool CCarCtrl::StopCarIfNodesAreInvalid(CVehicle* vehicle) {
    return plugin::CallAndReturn<bool, 0x422590, CVehicle*>(vehicle);
}

// 0x4222A0
void CCarCtrl::SwitchBetweenPhysicsAndGhost(CVehicle* vehicle) {
    plugin::Call<0x4222A0, CVehicle*>(vehicle);
}

// 0x423FC0
void CCarCtrl::SwitchVehicleToRealPhysics(CVehicle* vehicle) {
    vehicle->SetStatus(STATUS_PHYSICS);
    vehicle->m_autoPilot.m_nTempAction = TEMPACT_NONE;
    // Two separate original timer reads; the first deadline wraps modulo 2^32.
    const auto* timer = reinterpret_cast<const volatile uint32*>(0xB7CB84);
    vehicle->m_autoPilot.m_nTimeToStartMission = *timer + 2000u;
    vehicle->m_autoPilot.m_nTimeSwitchedToRealPhysics = *timer;
    vehicle->m_nFakePhysics = 0;
}

// 0x425B30
float CCarCtrl::TestCollisionBetween2MovingRects(CVehicle* vehicle1, CVehicle* vehicle2, float arg3, float arg4, CVector* pos1, CVector* pos2) {
    return plugin::CallAndReturn<float, 0x425B30, CVehicle*, CVehicle*, float, float, CVector*, CVector*>(vehicle1, vehicle2, arg3, arg4, pos1, pos2);
}

// 0x425F70
float CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper(CVehicle* vehicle1, CVehicle* vehicle2, float arg3, float arg4, CVector* pos1, CVector* pos2) {
    return plugin::CallAndReturn<float, 0x425F70, CVehicle*, CVehicle*, float, float, CVector*, CVector*>(vehicle1, vehicle2, arg3, arg4, pos1, pos2);
}

// 0x429520
void CCarCtrl::TestWhetherToFirePlaneGuns(CVehicle* vehicle, CEntity* target) {
    plugin::Call<0x429520, CVehicle*, CEntity*>(vehicle, target);
}

// 0x421FE0
bool CCarCtrl::ThisVehicleShouldTryNotToTurn(CVehicle* vehicle) {
    return plugin::CallAndReturn<bool, 0x421FE0, CVehicle*>(vehicle);
}

// 0x429300
void CCarCtrl::TriggerDogFightMoves(CVehicle* vehicle1, CVehicle* vehicle2) {
    plugin::Call<0x429300, CVehicle*, CVehicle*>(vehicle1, vehicle2);
}

// 0x424000
void CCarCtrl::UpdateCarCount(CVehicle* vehicle, uint8 bDecrease) {
    // DEC saturates by the sign of its wrapped result, not the input sign.
    const auto increment = [](auto& counter) {
        uint32 bits;
        std::memcpy(&bits, &counter, sizeof(bits));
        ++bits;
        std::memcpy(&counter, &bits, sizeof(bits));
    };
    const auto decrement = [](auto& counter, bool saturate) {
        uint32 bits;
        std::memcpy(&bits, &counter, sizeof(bits));
        --bits;
        if (saturate && (bits & 0x80000000u)) bits = 0;
        std::memcpy(&counter, &bits, sizeof(bits));
    };
    if (!bDecrease) {
        switch (vehicle->m_nCreatedBy) {
        case RANDOM_VEHICLE:
            if (vehicle->vehicleFlags.bIsLawEnforcer) increment(NumLawEnforcerCars);
            increment(NumRandomCars);
            break;
        case MISSION_VEHICLE:
            if (vehicle->vehicleFlags.bIsLawEnforcer) {
                vehicle->vehicleFlags.bIsLawEnforcer = false;
                decrement(NumLawEnforcerCars, false);
            }
            increment(NumMissionCars);
            break;
        case PARKED_VEHICLE: increment(NumParkedCars); break;
        case PERMANENT_VEHICLE: increment(NumPermanentVehicles); break;
        default: break;
        }
    } else {
        switch (vehicle->m_nCreatedBy) {
        case RANDOM_VEHICLE:
            if (vehicle->vehicleFlags.bIsLawEnforcer) decrement(NumLawEnforcerCars, true);
            decrement(NumRandomCars, true);
            break;
        case MISSION_VEHICLE: decrement(NumMissionCars, true); break;
        case PARKED_VEHICLE: decrement(NumParkedCars, true); break;
        case PERMANENT_VEHICLE: decrement(NumPermanentVehicles, true); break;
        default: break;
        }
    }
}

// 0x436540
void CCarCtrl::UpdateCarOnRails(CVehicle* vehicle) {
    plugin::Call<0x436540, CVehicle*>(vehicle);
}

// 0x426BC0
void CCarCtrl::WeaveForObject(CEntity* entity, CVehicle* vehicle, float* arg3, float* arg4) {
    plugin::Call<0x426BC0, CEntity*, CVehicle*, float*, float*>(entity, vehicle, arg3, arg4);
}

// 0x426350
void CCarCtrl::WeaveForOtherCar(CEntity* entity, CVehicle* vehicle, float* arg3, float* arg4) {
    plugin::Call<0x426350, CEntity*, CVehicle*, float*, float*>(entity, vehicle, arg3, arg4);
}

// 0x42D680
template<typename PtrListType>
void CCarCtrl::WeaveThroughCarsSectorList(PtrListType& ptrList, CVehicle* vehicle, CPhysical* physical, float arg4, float arg5, float arg6, float arg7, float* arg8, float* arg9) {
    plugin::Call<0x42D680, PtrListType&, CVehicle*, CPhysical*, float, float, float, float, float*, float*>(ptrList, vehicle, physical, arg4, arg5, arg6, arg7, arg8, arg9);
}

// 0x42D950
template<typename PtrListType>
void CCarCtrl::WeaveThroughObjectsSectorList(PtrListType& ptrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float* arg8) {
    plugin::Call<0x42D950, PtrListType&, CVehicle*, float, float, float, float, float*, float*>(ptrList, vehicle, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x42D7E0
template<typename PtrListType>
void CCarCtrl::WeaveThroughPedsSectorList(PtrListType& ptrList, CVehicle* vehicle, CPhysical* physical, float arg4, float arg5, float arg6, float arg7, float* arg8, float* arg9) {
    plugin::Call<0x42D7E0, PtrListType&, CVehicle*, CPhysical*, float, float, float, float, float*, float*>(ptrList, vehicle, physical, arg4, arg5, arg6, arg7, arg8, arg9);
}

// 0x427FE0
float CCarCtrl::FindMaxSteerAngle(CVehicle* veh) {
    // Como el original: si |v| > 0.7 devuelve 0.2; si no, min(0.9 - |v|, 0.7) (sin limite inferior)
    const CVector& v = veh->GetMoveSpeed();
    const float speed = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (speed > 0.7f) {
        return 0.2f;
    }
    const float angle = 0.9f - speed;
    return 0.7f < angle ? 0.7f : angle;
}

// 0x424100: native no-effect helper. Upper24bits of EAX must survive.
