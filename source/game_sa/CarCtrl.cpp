/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCarCtrl
 * Reconstrucción: COMPLETA — 93/93 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 93/93 entradas.
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

// Este archivo se compila con /Zi (sin /ZI ni comprobaciones en tiempo de ejecución), sin PCH ni unity (ver
// source/CMakeLists.txt): el relleno de /ZI cambiaría las banderas de Native_00424100, que se optimiza por pragma.
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"

#include "CarCtrl.h"
#include "TrafficLights.h"
#include "TheScripts.h"
#include "GangWars.h"
#include "Game.h"
#include "General.h"
#include "X87Math.h"
#include "VehicleRecording.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"
#include "GameLogic.h"
#include "CutsceneMgr.h"
#include "TheCarGenerators.h"
#include "eAreaCodes.h"
#include "TaskComplexWander.h"
#include "PedIntelligence.h"
#include "Events/EventPotentialGetRunOver.h"
#include "Curves.h"
#include "X87Math.h"
#include "Collision/ColStore.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Bike.h"
#include "DamageManager.h"

#include <reversiblebugfixes/Bugs.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// CPathNode::GetPosition (0x420A10) se llama fuera de linea, una vez por lectura, como hace el EXE (puntero volatil: impide inlinearlo).
static CVector (CPathNode::* volatile const cc63GetNodePosition)() const = &CPathNode::GetPosition;

// Ayudantes x87 de la tanda 3 (definidos al final de X87Math.cpp; declarados aqui para no recompilar todo el proyecto al tocar X87Math.h).
float __cdecl Cc63cLinkDistanceCpp(int node2X, int node2Y, int node1X, int node1Y);                               // JoinCarWithRoadSystem: distancia entre dos nodos (x, y enteros de 16 bits / 8)
bool __cdecl Cc63cDotNegativeCpp(int node1X, int node1Y, int node3X, int node3Y, float forwardX, float forwardY);  // JoinCarWithRoadSystem: producto escalar con el vector adelante < 0
// Ayudantes x87 de la tanda 5 (ChooseModel), definidos al final de X87Math.cpp.
int __cdecl Cc63eTotalCpp(float a, float b, float c, float d);                                // ftol2(((a + b) + c) + d)
bool __cdecl Cc63eRollFailsCpp(int randValue);                                                // ftol2(fild(r & 0xFFFF) * (1/65536) * 100.0f) != 0
float __cdecl Cc63eRatioCpp(int randValue);                                                   // fild(r) * 0x38000100 guardado como float
bool __cdecl Cc63eTierExceedsCpp(float a, float b, float c, int count, float total, float roll);  // (a [+ b [+ c]]) / total > roll, ordenada
int __cdecl Cc63ePickRollCpp(int randValue, int total);                                       // ftol2((fild(r & 0xFFFF) * (1/65536)) * total)
int __cdecl Cc63eGroupRollCpp(int randValue);                                                 // ftol2(fild(r & 0xFFFF) * (1/65536) * 23.0f)
// Ayudantes x87 de la tanda 6 (IsThisAnAppropriateNode), definidos al final de X87Math.cpp.
bool __cdecl Cc63fCrossZNegativeCpp(const float* node3, const float* point, const float* node2);   // ((nodo3 - nodo2) x (nodo3 - punto)).z < 0 (CrossProduct 0x59C730 en el orden del EXE)
bool __cdecl Cc63fRightDotNegativeCpp(const float* right, const float* node3, const float* point);  // DotProduct(derecha, nodo3 - punto) < 0 (0x40FDB0: suma en el orden z, y, x)

auto& apCarsToKeep = StaticRef<CVehicle*[2]>(0x969084);
auto& aCarsToKeepTime = StaticRef<std::array<uint32, 2>>(0x96907C);

// Lote 63 (aire): colaboradores del EXE sin reconstruir que se llaman por direccion (los bancos los sustituyen por falsos identicos).
// CVector::Normalise (0x59C910, thiscall, in situ; deja la x87 vacia) y rand() del CRT del EXE (0x821B1E: avanza la semilla del juego, la rand() de la DLL no).
static void CarCtrlAirExeNormalise(CVector* v) { reinterpret_cast<void(__thiscall*)(CVector*)>(0x59C910)(v); }
static int CarCtrlAirExeRand() { return reinterpret_cast<int(__cdecl*)()>(0x821B1E)(); }
// Funcion del EXE sin reconstruir que llama SteerAIBoatWithPhysicsAttackingPlayer con el modelo 430 (0x73DF00, thiscall sobre el vehiculo)
static void CarCtrlAirExeBoatSpecial(CVehicle* boat) { reinterpret_cast<void(__thiscall*)(CVehicle*)>(0x73DF00)(boat); }

// 0x424100: XOR AL,AL; RET. Conserva EAX alto, ECX/EDX y los registros no volátiles. Función suelta (alias de dirección):
// CCarCtrl es sólo el dueño de la instrumentación. Optimizada por pragma para que el cuerpo mida 3 bytes como el del EXE.
#pragma optimize("gty", on)
bool Native_00424100(float, float) { return false; }
#pragma optimize("", on)

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
    // Lote 63 (parte C)
    RH_ScopedInstall(ThisVehicleShouldTryNotToTurn, 0x421FE0);
    RH_ScopedInstall(FindPathDirection, 0x422090);
    RH_ScopedInstall(SwitchBetweenPhysicsAndGhost, 0x4222A0);
    RH_ScopedNamedGlobalInstall(Cc63SpeedMultiplierCpp, "FindSpeedMultiplier", 0x4224E0);
    RH_ScopedInstall(StopCarIfNodesAreInvalid, 0x422590);
    RH_ScopedInstall(FindPercDependingOnDistToLink, 0x422620);
    RH_ScopedNamedGlobalInstall(Cc63Intersection2LinesCpp, "FindIntersection2Lines", 0x4226F0);
    // Lote 63 (parte C, tanda 2)
    RH_ScopedInstall(SetUpDriverAndPassengersForVehicle, 0x4217C0);
    RH_ScopedInstall(ScanForPedDanger, 0x42CE40);
    RH_ScopedInstall(SetCoordsOfScriptCar, 0x4342A0);
    RH_ScopedInstall(FindMaximumSpeedForThisCarInTraffic, 0x434400);
    // Lote 63 (parte C, tanda 3)
    RH_ScopedInstall(PickNextNodeAccordingStrategy, 0x432B10);
    RH_ScopedInstall(JoinCarWithRoadSystem, 0x42F5A0);
    RH_ScopedInstall(JoinCarWithRoadSystemGotoCoors, 0x42F870);
    // Lote 63 (parte C, tanda 4)
    RH_ScopedInstall(ReconsiderRoute, 0x42FC40);
    RH_ScopedInstall(FindLinksToGoWithTheseNodes, 0x42B470);
    // Lote 63 (parte C, tanda 5)
    RH_ScopedInstall(ChooseModel, 0x424CE0);
    // Lote 63 (parte C, tanda 6)
    RH_ScopedInstall(IsThisAnAppropriateNode, 0x42DAB0);
    RH_ScopedInstall(JoinCarWithRoadAccordingToMission, 0x432CB0);
    RH_ScopedInstall(PossiblyFireHSMissile, 0x429600);
    // Lote 63 (aire): helicopteros, aviones y combate aereo
    RH_ScopedInstall(GetAIHeliToFlyInDirection, 0x42A730);
    RH_ScopedInstall(GetAIPlaneToDoDogFightAgainstPlayer, 0x42F370);
    RH_ScopedInstall(SteerAIHeliToCrashAndBurn, 0x4238E0);
    RH_ScopedInstall(SteerAIPlaneToCrashAndBurn, 0x423880);
    RH_ScopedInstall(SteerAIPlaneTowardsTargetCoors, 0x423790);
    RH_ScopedInstall(SteerAIPlaneToFollowEntity, 0x4237F0);
    RH_ScopedInstall(SteerAIHeliTowardsTargetCoors, 0x42A630);
    RH_ScopedInstall(SteerAIHeliFlyingAwayFromPlayer, 0x42ACB0);
    RH_ScopedInstall(SteerAIHeliToLand, 0x42AD30);
    RH_ScopedInstall(SteerAIHeliAsPoliceHeli, 0x42AAD0);
    RH_ScopedInstall(TestWhetherToFirePlaneGuns, 0x429520);
    RH_ScopedInstall(TriggerDogFightMoves, 0x429300);
    RH_ScopedInstall(GetAIPlaneToAttackPlayer, 0x429780);
    RH_ScopedInstall(GetAIPlaneToDoDogFight, 0x429890);
    RH_ScopedInstall(SteerAIHeliToFollowEntity, 0x42A750);
    RH_ScopedInstall(SteerAIHeliToKeepEntityInView, 0x42AEB0);
    RH_ScopedInstall(GetAIHeliToAttackPlayer, 0x42F3C0);
    RH_ScopedInstall(FireHeliRocketsAtTarget, 0x42B270);
    RH_ScopedInstall(FlyAIHeliToTarget_FixedOrientation, 0x423940);
    RH_ScopedInstall(FlyAIPlaneInCertainDirection, 0x423000);
    RH_ScopedInstall(FlyAIHeliInCertainDirection, 0x429A70);
    RH_ScopedInstall(SteerAIBoatWithPhysicsHeadingForTarget, 0x428BE0);
    RH_ScopedInstall(SteerAIBoatWithPhysicsAttackingPlayer, 0x428DE0);
    RH_ScopedInstall(SteerAIBoatWithPhysicsCirclingPlayer, 0x429090);
    RH_ScopedInstall(PruneVehiclesOfInterest, 0x423F10);
    RH_ScopedInstall(RemoveCarsIfThePoolGetsFull, 0x4322B0);
    RH_ScopedInstall(RemoveDistantCars, 0x42CD10);
    RH_ScopedInstall(RemoveFromInterestingVehicleList, 0x423ED0);
    RH_ScopedInstall(ScriptGenerateOneEmergencyServicesCar, 0x42FBC0);
    RH_ScopedInstall(SlowCarDownForObject, 0x426220);
    RH_ScopedInstall(SlowCarOnRailsDownForTrafficAndLights, 0x434790);
    RH_ScopedInstall(FindMaxSteerAngle, 0x427FE0);
    RH_ScopedInstall(SteerAICarWithPhysicsTryingToBlockTarget_Stop, 0x428990);
    RH_ScopedInstall(SteerAICarWithPhysicsHeadingForTarget, 0x433280);
    RH_ScopedInstall(SteerAICarWithPhysicsTryingToBlockTarget, 0x4335E0);
    RH_ScopedInstall(SteerAICarWithPhysicsFollowPreRecordedPath, 0x432DD0);
    RH_ScopedInstall(SteerAICarTowardsPointInEscort, 0x4336D0);
    RH_ScopedInstall(SteerAICarBlockingPlayerForwardAndBack, 0x422B20);
    RH_ScopedInstall(SteerAICarParkParallel, 0x433BA0);
    RH_ScopedInstall(SteerAICarParkPerpendicular, 0x433EA0);
    RH_ScopedInstall(DealWithBend_Racing, 0x428040);
    RH_ScopedInstall(GenerateRandomCars, 0x4341C0);
    RH_ScopedInstall(ClearInterestingVehicleList, 0x423F00);
    RH_ScopedInstall(RegisterVehicleOfInterest, 0x423DE0);
    RH_ScopedInstall(FindSequenceElement, 0x421770);
    RH_ScopedInstall(SwitchVehicleToRealPhysics, 0x423FC0);
    RH_ScopedInstall(UpdateCarCount, 0x424000);
    RH_ScopedInstall(FindSpeedMultiplierWithSpeedFromNodes, 0x424130);
    // Lote 63, parte B2
    RH_ScopedInstall(TestCollisionBetween2MovingRects, 0x425B30);
    RH_ScopedInstall(TestCollisionBetween2MovingRects_OnlyFrontBumper, 0x425F70);
    RH_ScopedInstall(SlowCarDownForOtherCar, 0x42D0E0);
    RH_ScopedInstall(WeaveForOtherCar, 0x426350);
    RH_ScopedInstall(WeaveForObject, 0x426BC0);
    RH_ScopedInstall(SlowCarDownForCarsSectorList<CPtrListDoubleLink<CVehicle*>>, 0x432420);
    RH_ScopedInstall(SlowCarDownForObjectsSectorList<CPtrListDoubleLink<CObject*>>, 0x42D4F0);
    RH_ScopedInstall(SlowCarDownForPedsSectorList<CPtrListDoubleLink<CPed*>>, 0x425440);
    RH_ScopedInstall(WeaveThroughCarsSectorList<CPtrListDoubleLink<CVehicle*>>, 0x42D680);
    RH_ScopedInstall(WeaveThroughPedsSectorList<CPtrListDoubleLink<CPed*>>, 0x42D7E0);
    RH_ScopedInstall(WeaveThroughObjectsSectorList<CPtrListDoubleLink<CObject*>>, 0x42D950);
    RH_ScopedInstall(UpdateCarOnRails, 0x436540);
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
    // Como el original (0x424CE0..0x424F7F). rand es el del EXE (0x821B1E). Suma de las cuatro densidades (CPopCycle 0xC0BC30..3C, float, x87 24 bits) truncada con _ftol2; <= 0 -> -1.
    // Con el byte 0x96917A: un rand & 0xFFFF * 1/65536 * 100.0f truncado distinto de 0 -> -1. Se sortea f = rand (sin enmascarar) * 0x38000100 como float y t = (float)total y la
    // categoria es la primera cuyo acumulado / t supera a f: dealers (0x19: el modelo es [0xC0F240] si esta cargado, si no -1), pandillas (con el byte 0x96915B tambien),
    // policia (13: ni guerra de pandillas ni coches de policia prohibidos -> ChoosePoliceCarModel(0)) u otros (0: [0xA4448C] o PickRandomCar(true, false)).
    const auto exeRand = reinterpret_cast<int(__cdecl*)()>(0x821B1E);
    const int32 total = Cc63eTotalCpp(CPopCycle::m_NumOther_Cars, CPopCycle::m_NumCops_Cars, CPopCycle::m_NumGangs_Cars, CPopCycle::m_NumDealers_Cars);
    if (total <= 0) {
        return -1;
    }
    if (StaticRef<uint8>(0x96917A)) {
        if (Cc63eRollFailsCpp(exeRand())) {
            return -1;
        }
    }
    volatile float roll = Cc63eRatioCpp(exeRand());
    const uint8 flag = StaticRef<uint8>(0x96915B);
    volatile float totalFloat = static_cast<float>(total);
    const auto loaded = [](int32 model) { return StaticRef<uint8>(0x8E4CD0 + static_cast<uintptr_t>(model) * 20) == 1; };
    if (Cc63eTierExceedsCpp(CPopCycle::m_NumDealers_Cars, 0.0f, 0.0f, 1, totalFloat, roll) && !flag) {
        *arg1 = 0x19;
        const int32 model = StaticRef<uint16>(0xC0F240);
        return loaded(model) ? model : -1;
    }
    if (Cc63eTierExceedsCpp(CPopCycle::m_NumGangs_Cars, CPopCycle::m_NumDealers_Cars, 0.0f, 2, totalFloat, roll) || flag) {
        if (CPopulation::m_bDontCreateRandomGangMembers) {
            return -1;
        }
        const uint8* strength = CPopCycle::m_pCurrZoneInfo->GangStrength;
        const int32 sum = strength[8] + strength[9] + strength[7] + strength[6] + strength[5] + strength[4] + strength[3] + strength[2] + strength[1] + strength[0];
        int32 gang = 0;
        if (sum <= 0) {
            return -1;
        }
        int32 pick = Cc63ePickRollCpp(exeRand(), sum);
        strength = CPopCycle::m_pCurrZoneInfo->GangStrength;
        if (strength[0] < pick) {
            int32 current = strength[0];
            do {
                pick -= current;
                current = strength[gang + 1];
                ++gang;
            } while (current < pick);
        }
        if (StaticRef<uint8>(0x96915B)) {
            gang = reinterpret_cast<int32(__cdecl*)(int32, int32)>(0x407180)(0, 9);
        }
        *arg1 = gang + 14;
        if (CPopulation::PickGangCar(static_cast<eGangID>(gang)) < 0) {
            return -1;
        }
        const uintptr_t row = static_cast<uintptr_t>(gang) * 23;
        for (int32 tries = 0; tries < 10; tries++) {
            int32 index;
            do {
                index = static_cast<int32>(row) + Cc63eGroupRollCpp(exeRand());
            } while (StaticRef<uint16>(0xC0F074 + static_cast<uintptr_t>(index) * 2) == 2000);
            const int32 model = StaticRef<uint16>(0xC0F074 + static_cast<uintptr_t>(index) * 2);
            if (loaded(model)) {
                return model;
            }
        }
        return -1;
    }
    if (!Cc63eTierExceedsCpp(CPopCycle::m_NumCops_Cars, CPopCycle::m_NumGangs_Cars, CPopCycle::m_NumDealers_Cars, 3, totalFloat, roll)) {
        *arg1 = 0;
        const int32 model = StaticRef<int32>(0xA4448C);
        if (model != -1) {
            return model;
        }
        return CPopulation::m_AppropriateLoadedCars.PickRandomCar(true, false);
    }
    if (CGangWars::GangWarFightingGoingOn() || CPopulation::m_bDontCreateRandomCops) {
        return -1;
    }
    *arg1 = 13;
    return ChoosePoliceCarModel(0);
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
    // Como el original: las direcciones de tramo se pasan en una palabra de pila (solo importan sus 16 bits bajos) y los tramos (CCarPathLink, 0xE bytes) se leen en crudo:
    // posicion int16 en +0/+2 (x 0.125), direccion int8 en +8/+9 (x 0.01f, no /100), banderas de carriles en +0xB.
    constexpr float kPi = std::bit_cast<float>(0x40490FDBu);    // 0x858CB8
    constexpr float kTwoPi = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
    const auto raw = [](const CCarPathLinkAddress& address) { return *reinterpret_cast<const uint16*>(&address); };
    const auto i16 = [](const uint8* link, uint32 offset) { return static_cast<float>(*reinterpret_cast<const int16*>(link + offset)); };
    const auto i8 = [](const uint8* link, uint32 offset) { return static_cast<float>(*reinterpret_cast<const int8*>(link + offset)); };
    const auto position = [&](const uint8* link, uint32 offset) { return i16(link, offset) * 0.125f; }; // 0x858C48
    const auto linkOf = [](uint16 address) { return reinterpret_cast<const uint8*>(ThePaths.m_pNaviNodes[address >> 10]) + (address & 0x3FFu) * 0xEu; };

    if (raw(LinkAddress1) == 0xFFFF) {
        return false;
    }
    if (raw(LinkAddress2) == 0xFFFF) {
        return false;
    }
    // Moto (subtipo +0x594 == 10): umbrales de angulo y distancia distintos
    const bool bike = *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(vehicle) + 0x594) == 10;
    const float angleMin = bike ? std::bit_cast<float>(0x3EB2B8C3u) : std::bit_cast<float>(0x3F060A92u);
    const float angleWide = bike ? std::bit_cast<float>(0x3F32B8C3u) : std::bit_cast<float>(0x3F860A92u);
    const float limit = bike ? 50.0f : 40.0f;
    *arg12 = 1.0f;
    if (!ThePaths.m_pPathNodes[raw(LinkAddress1) >> 10]) {
        return false;
    }
    if (!ThePaths.m_pPathNodes[raw(LinkAddress2) >> 10]) {
        return false;
    }
    const uint8* const link1 = linkOf(raw(LinkAddress1));
    const uint8* link2 = linkOf(raw(LinkAddress2));

    // Distancia del vehiculo al primer tramo (> 60: no hace nada)
    const float link1X = position(link1, 0);
    const float link1Y = position(link1, 2);
    const CVector& vehiclePos = vehicle->GetPosition();
    const float toLinkY = link1Y - vehiclePos.y;
    const float toLinkX = link1X - vehiclePos.x;
    *arg14 = std::sqrt(toLinkY * toLinkY + toLinkX * toLinkX);
    if (*arg14 > 60.0f) { // 0x858B34
        return false;
    }

    // Cadena de tramos: sigue con el 3.o y el 4.o mientras cada uno este a menos de 10 m del anterior; el multiplicador de direccion es el argumento 7, 8 o 9 segun el ultimo
    const auto closeTo = [&](const uint8* a, const uint8* b) {
        const float dy = position(a, 2) - position(b, 2);
        const float dx = position(a, 0) - position(b, 0);
        return dx * dx + dy * dy < 100.0f; // 0x858628
    };
    int8 select = static_cast<int8>(arg7);
    if (raw(LinkAddress3) != 0xFFFF) {
        const uint8* const link3 = linkOf(raw(LinkAddress3));
        if (closeTo(link2, link3)) {
            select = static_cast<int8>(arg8);
            link2 = link3;
        }
        if (raw(LinkAddress4) != 0xFFFF) {
            const uint8* const link4 = linkOf(raw(LinkAddress4));
            if (closeTo(link2, link4)) {
                select = static_cast<int8>(arg9);
                link2 = link4;
            }
        }
    }

    const float endX = position(link2, 0);
    const float endY = position(link2, 2);
    const float multiplier1 = static_cast<float>(static_cast<int8>(arg6));
    const float multiplier2 = static_cast<float>(select);
    const float v1x = i8(link1, 8) * 0.01f * multiplier1; // 0x858C58
    const float v1y = i8(link1, 9) * 0.01f * multiplier1;
    const float v2x = i8(link2, 8) * 0.01f * multiplier2;
    const float v2y = i8(link2, 9) * 0.01f * multiplier2;

    // Diferencia de rumbo entre el avance del vehiculo y la direccion del ultimo tramo (el ultimo fpatan se resta SIN redondear, X87Math.cpp)
    const CVector& top = vehicle->m_matrix->GetForward();
    const float vehicleHeading = CGeneral::GetATanOfXY(top.x, top.y);
    float diff = CarCtrlSteer63AtanRsubCpp(v2x, v2y, vehicleHeading);
    while (diff > kPi) {
        diff -= kTwoPi;
    }
    while (diff < -kPi) {
        diff += kTwoPi;
    }
    const float absDiff = diff < 0.0f ? -diff : diff;
    if (absDiff < angleMin) {
        return false;
    }
    const auto lanes = [](const uint8* link) {
        const uint8 flags = link[0xB];
        return static_cast<int32>((flags >> 3) & 7) + static_cast<int32>(flags & 7);
    };
    if (lanes(link2) >= 4 && lanes(link1) >= 4) {
        if (absDiff < angleWide) {
            return false;
        }
    }
    *arg13 = diff;

    // Puntos de los carriles del primer tramo (Q: carriles opuestos, R: carriles del mismo sentido; 5.4 = 0x858C50) y el mas cercano al final del ultimo tramo
    struct LanePoints {
        float opposite0, opposite1, same0, same1;
    };
    const auto lanePoints = [](const uint8* link, float dirX, float dirY) -> LanePoints {
        const float offset = reinterpret_cast<const CCarPathLink*>(link)->OneWayLaneOffset(); // 0x44DB00
        const float opposite = static_cast<float>(link[0xB] & 7);
        const float same = static_cast<float>((link[0xB] >> 3) & 7);
        return {
            ((offset + (opposite - 1.0f)) * dirY) * 5.4f,
            ((offset + (opposite - 1.0f)) * dirX) * 5.4f,
            ((offset + (same - 1.0f)) * dirY) * 5.4f,
            ((offset + (same - 1.0f)) * dirX) * 5.4f,
        };
    };
    const LanePoints first = lanePoints(link1, v1x, v1y);
    const float q1x = first.opposite0 + link1X;
    const float q1y = link1Y - first.opposite1;
    const float r1x = link1X - first.same0;
    const float r1y = first.same1 + link1Y;
    const float a1 = q1y - endY;
    const float b1 = q1x - endX;
    const float c1 = r1y - endY;
    const float d1 = r1x - endX;
    float ax;
    float ay;
    float bx;
    float by;
    if (d1 * d1 + c1 * c1 > b1 * b1 + a1 * a1) {
        ax = q1x;
        ay = q1y;
        bx = r1x;
        by = r1y;
    } else {
        ax = r1x;
        ay = r1y;
        bx = q1x;
        by = q1y;
    }
    // El segundo punto se acerca como mucho a 11 m del primero
    {
        const float dx = bx - ax;
        const float dy = by - ay;
        const float length = std::sqrt(dy * dy + dx * dx);
        if (length > 11.0f) { // 0x858FBC
            const float scale = 11.0f / length;
            bx = scale * dx + ax;
            by = scale * dy + ay;
        }
    }

    // Puntos de los carriles del ultimo tramo y el mas cercano al primer tramo
    const LanePoints last = lanePoints(link2, v2x, v2y);
    const float q2x = last.opposite0 + endX;
    const float q2y = endY - last.opposite1;
    const float r2x = endX - last.same0;
    const float r2y = last.same1 + endY;
    const float a2 = q2y - link1Y;
    const float b2 = q2x - link1X;
    const float c2 = r2y - link1Y;
    const float d2 = r2x - link1X;
    float cx;
    float cy;
    if (d2 * d2 + c2 * c2 > b2 * b2 + a2 * a2) {
        cx = q2x;
        cy = q2y;
    } else {
        cx = r2x;
        cy = r2y;
    }

    // Direcciones casi paralelas (|dx| < 0.1 y |dy| != 0): el punto es A; si no, interseccion de las dos rectas (0x4226F0)
    const float directionDx = v1x - v2x;
    const float directionDy = v1y - v2y;
    const float absDx = directionDx < 0.0f ? -directionDx : directionDx;
    const float absDy = directionDy < 0.0f ? -directionDy : directionDy;
    if (absDx < 0.1f && !(absDy == 0.0f)) { // 0x858B1C, 0x858B50
        pos->x = ax;
        pos->y = ay;
    } else {
        FindIntersection2Lines(ax, ay, v1x, v1y, cx, cy, v2x, v2y, &pos->x, &pos->y);
    }

    // Factor de velocidad (arg12) segun la distancia y el giro; |giro x 0.6366| acotado a 1
    const auto turn = [&] {
        const float scaled = diff * 0.63661975f; // 0x858FB8
        return 1.0f < (scaled < 0.0f ? -scaled : scaled) ? 1.0f : (scaled < 0.0f ? -scaled : scaled);
    };
    if (*arg14 < limit) {
        *arg12 = 1.0f - ((limit - *arg14) / limit * turn()) * 0.6f; // 0x858CC8
    }
    float p = arg10 * 0.033333335f; // 0x858F10
    if (1.0f < p) {
        p = 1.0f;
    } else if (0.2f > p) { // 0x858CC4
        p = 0.2f;
    }
    const float threshold = p * turn() * 20.0f; // 0x858BA4
    const CVector& pos2 = vehicle->GetPosition();
    if (*arg14 < threshold) {
        *arg11 = CGeneral::GetATanOfXY(pos->x - pos2.x, pos->y - pos2.y);
    } else {
        *arg11 = CGeneral::GetATanOfXY(bx - pos2.x, by - pos2.y);
    }
    return true;
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
    // Aritmetica x87 de 24 bits en X87Math (Cc63Intersection2LinesCpp), que es tambien el destino del hook (conserva EDX como el EXE).
    Cc63Intersection2LinesCpp(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

// 0x42B470
void CCarCtrl::FindLinksToGoWithTheseNodes(CVehicle* vehicle) {
    // Como el original (0x42B470..0x42B7CD). Si la palabra +0x45E del vehiculo no es 0: srand del EXE (0x821B11). cur = +0x390, start = +0x394 (direcciones de nodo); NINGUN area se comprueba salvo
    // la del enlace candidato. base = int16 +0x10 del nodo actual. Se busca (hasta 12 enlaces, contador de 16 bits con signo) el indice i de enlaces[base + i] igual a start; +0x3A6 (siguiente enlace de
    // carretera) = m_pNaviLinks[area][base + i]. +0x3B6 = -1 si cur < start (area y luego nodo, sin signo; iguales -> 1). Con un solo enlace el indice es 0; con ninguno 0; si no se elige el enlace (no igual a
    // start y con el area cargada) mas cercano al vehiculo segun CCollision::DistToLine(posicion del nodo actual, posicion del enlace, posicion del vehiculo) (posiciones = enteros de 16 bits / 8 exactos;
    // `<` ordenado contra 1000000.0f / el mejor) y, sin ninguno, 0. +0x3A4 (enlace actual) = m_pNaviLinks[area][base + indice] y +0x3B5 = -1 si el enlace elegido < cur (iguales -> 1).
    auto* const raw = reinterpret_cast<uint8*>(vehicle);
    auto& autoPilot = vehicle->m_autoPilot;
    {
        const uint16 seed = *reinterpret_cast<const uint16*>(raw + 0x45E);
        if (seed) {
            reinterpret_cast<void(__cdecl*)(uint32)>(0x821B11)(seed);
        }
    }
    const auto nodeAt = [](CNodeAddress address) { return reinterpret_cast<const uint8*>(ThePaths.m_pPathNodes[address.m_wAreaId]) + static_cast<size_t>(address.m_wNodeId) * 0x1C; };
    const auto baseOf = [&](CNodeAddress address) { return static_cast<int32>(*reinterpret_cast<const int16*>(nodeAt(address) + 0x10)); };
    const auto numLinksOf = [&](CNodeAddress address) { return static_cast<int32>(nodeAt(address)[0x18] & 0x0F); };
    const CNodeAddress cur = autoPilot.m_currentAddress;
    const CNodeAddress* const links = ThePaths.m_pNodeLinks[cur.m_wAreaId];
    int16 index = 0;
    do {
        const CNodeAddress& link = links[baseOf(cur) + index];
        if (link.m_wAreaId == autoPilot.m_startingRouteNode.m_wAreaId && link.m_wNodeId == autoPilot.m_startingRouteNode.m_wNodeId) {
            break;
        }
        ++index;
    } while (index < 12);
    autoPilot.m_nNextPathNodeInfo = ThePaths.m_pNaviLinks[cur.m_wAreaId][baseOf(cur) + index];
    {
        const CNodeAddress current = autoPilot.m_currentAddress;
        const CNodeAddress start = autoPilot.m_startingRouteNode;
        const bool below = current.m_wAreaId < start.m_wAreaId || (current.m_wAreaId == start.m_wAreaId && current.m_wNodeId < start.m_wNodeId);
        autoPilot._smthNext = below ? -1 : 1;
    }
    int32 chosen;
    if (numLinksOf(cur) == 1) {
        chosen = 0;
    } else {
        chosen = -1;
        float best = std::bit_cast<float>(0x497423FEu);
        if (numLinksOf(cur) != 0) {
            for (int32 k = 0; ; ) {
                const CNodeAddress here = autoPilot.m_currentAddress;
                const CNodeAddress link = links[baseOf(here) + k];
                bool skip = link.m_wAreaId == autoPilot.m_startingRouteNode.m_wAreaId && link.m_wNodeId == autoPilot.m_startingRouteNode.m_wNodeId;
                if (!skip && ThePaths.m_pPathNodes[link.m_wAreaId]) {
                    const uint8* const from = nodeAt(here);
                    const uint8* const to = nodeAt(link);
                    const CVector fromPos{ static_cast<float>(*reinterpret_cast<const int16*>(from + 8)) * 0.125f, static_cast<float>(*reinterpret_cast<const int16*>(from + 0xA)) * 0.125f,
                                           static_cast<float>(*reinterpret_cast<const int16*>(from + 0xC)) * 0.125f };
                    const CVector toPos{ static_cast<float>(*reinterpret_cast<const int16*>(to + 8)) * 0.125f, static_cast<float>(*reinterpret_cast<const int16*>(to + 0xA)) * 0.125f,
                                         static_cast<float>(*reinterpret_cast<const int16*>(to + 0xC)) * 0.125f };
                    const float distance = CCollision::DistToLine(fromPos, toPos, vehicle->GetPosition());
                    if (distance < best) {
                        best = distance;
                        chosen = k;
                    }
                }
                ++k;
                if (!(k < numLinksOf(autoPilot.m_currentAddress))) {
                    break;
                }
            }
        }
        if (chosen < 0) {
            chosen = 0;
        }
    }
    autoPilot.m_nCurrentPathNodeInfo = ThePaths.m_pNaviLinks[autoPilot.m_currentAddress.m_wAreaId][baseOf(autoPilot.m_currentAddress) + chosen];
    {
        const CNodeAddress* const chosenLink = &ThePaths.m_pNodeLinks[autoPilot.m_currentAddress.m_wAreaId][baseOf(autoPilot.m_currentAddress) + chosen];
        const CNodeAddress current = autoPilot.m_currentAddress;
        const bool below = chosenLink->m_wAreaId < current.m_wAreaId || (chosenLink->m_wAreaId == current.m_wAreaId && chosenLink->m_wNodeId < current.m_wNodeId);
        autoPilot._smthCurr = below ? -1 : 1;
    }
}

// 0x434400
float CCarCtrl::FindMaximumSpeedForThisCarInTraffic(CVehicle* vehicle) {
    // Como el original (0x434400..0x434783). +0x3B9 es el estilo de conduccion (byte), +0x3D0 la velocidad de crucero (byte) y +0x3D4 su multiplicador (float).
    // Estilos 2, 3 y 5: devuelve crucero * multiplicador sin mas. Si no, caja de +-14.0f (0x859030) alrededor del vehiculo en sectores del mundo (ftol2 de floor(v * 0.02f + 60.0f);
    // el minimo se acota por abajo a 0 y el maximo por arriba a 0x77, cada uno sin acotar el otro lado), nuevo codigo de escaneo, y para cada sector repetido (y exterior, x interior;
    // indice ((y & 15) << 4) + (x & 15)) llama, por este orden, a SlowCarDownForCarsSectorList (salvo estilo 6), ...PedsSectorList y ...ObjectsSectorList con (x1, y1, x2, y2,
    // &maxSpeed, crucero * multiplicador recalculado en cada llamada). Despues marca bWarnedPeds y devuelve maxSpeed (estilos 0 y 4) o (crucero * multiplicador + maxSpeed) * 0.5f.
    auto& autoPilot = vehicle->m_autoPilot;
    const auto cruiseSpeed = [&] { return Cc63bCruiseSpeedCpp(autoPilot.m_nCruiseSpeed, autoPilot.m_SpeedMult); };
    {
        const uint8 style = static_cast<uint8>(autoPilot.m_nCarDrivingStyle);
        if (style == 2 || style == 3 || style == 5) {
            return cruiseSpeed();
        }
    }
    constexpr float range = 14.0f; // 0x859030
    const CVector& position = vehicle->GetPosition();
    const float x1 = position.x - range;
    const float x2 = position.x + range;
    const float y1 = position.y - range;
    const float y2 = position.y + range;
    const int32 xMinRaw = Cc63bSectorCpp(x1);
    const int32 xMin = xMinRaw > 0 ? xMinRaw : 0;
    const int32 yMinRaw = Cc63bSectorCpp(y1);
    const int32 yMin = yMinRaw > 0 ? yMinRaw : 0;
    const int32 xMaxRaw = Cc63bSectorCpp(x2);
    const int32 xMax = xMaxRaw < 0x77 ? xMaxRaw : 0x77;
    const int32 yMaxRaw = Cc63bSectorCpp(y2);
    const int32 yMax = yMaxRaw < 0x77 ? yMaxRaw : 0x77;
    CWorld::AdvanceCurrentScanCode();
    float maxSpeed = cruiseSpeed();
    for (int32 y = yMin; y <= yMax; y++) {
        for (int32 x = xMin; x <= xMax; x++) {
            CRepeatSector& sector = CWorld::GetRepeatSector(x, y);
            if (static_cast<uint8>(autoPilot.m_nCarDrivingStyle) != 6) {
                const float arg8 = cruiseSpeed();
                SlowCarDownForCarsSectorList(sector.Vehicles, vehicle, x1, y1, x2, y2, &maxSpeed, arg8);
            }
            {
                const float arg8 = cruiseSpeed();
                SlowCarDownForPedsSectorList(sector.Peds, vehicle, x1, y1, x2, y2, &maxSpeed, arg8);
            }
            {
                const float arg8 = cruiseSpeed();
                SlowCarDownForObjectsSectorList(sector.Objects, vehicle, x1, y1, x2, y2, &maxSpeed, arg8);
            }
        }
    }
    vehicle->vehicleFlags.bWarnedPeds = true;
    const uint8 style = static_cast<uint8>(autoPilot.m_nCarDrivingStyle);
    if (style == 0 || style == 4) {
        return maxSpeed;
    }
    return Cc63bTrafficSpeedCpp(autoPilot.m_nCruiseSpeed, autoPilot.m_SpeedMult, maxSpeed);
}

// 0x42BD20
void CCarCtrl::FindNodesThisCarIsNearestTo(CVehicle* vehicle, CNodeAddress& nodeAddress1, CNodeAddress& nodeAddress2) {
    plugin::Call<0x42BD20, CVehicle*, CNodeAddress&, CNodeAddress&>(vehicle, nodeAddress1, nodeAddress2);
}

// 0x422090
int8 CCarCtrl::FindPathDirection(CNodeAddress nodeAddress1, CNodeAddress nodeAddress2, CNodeAddress nodeAddress3, bool* arg4) {
    // Como el original (0x422090..0x422292): *arg4 = false antes de nada; cualquier area 0xFFFF o sin cargar (en el orden 1, 2, 3) -> 0. CPathNode::GetPosition
    // se llama fuera de linea ocho veces en este orden (nodo 2, 1, 2, 1, 3, 2, 3, 2) y cada vez solo se lee una coordenada; la aritmetica x87 esta en X87Math.
    *arg4 = false;
    if (nodeAddress1.m_wAreaId == 0xFFFF || nodeAddress2.m_wAreaId == 0xFFFF || nodeAddress3.m_wAreaId == 0xFFFF) {
        return 0;
    }
    CPathNode* const area1 = ThePaths.m_pPathNodes[nodeAddress1.m_wAreaId];
    if (!area1) {
        return 0;
    }
    CPathNode* const area2 = ThePaths.m_pPathNodes[nodeAddress2.m_wAreaId];
    if (!area2) {
        return 0;
    }
    CPathNode* const area3 = ThePaths.m_pPathNodes[nodeAddress3.m_wAreaId];
    if (!area3) {
        return 0;
    }
    CPathNode* const node1 = area1 + nodeAddress1.m_wNodeId;
    CPathNode* const node2 = area2 + nodeAddress2.m_wNodeId;
    CPathNode* const node3 = area3 + nodeAddress3.m_wNodeId;
    const float p2xA = (node2->*cc63GetNodePosition)().x;
    const float p1xB = (node1->*cc63GetNodePosition)().x;
    const float p2yC = (node2->*cc63GetNodePosition)().y;
    const float p1yD = (node1->*cc63GetNodePosition)().y;
    const float p3xE = (node3->*cc63GetNodePosition)().x;
    const float p2xF = (node2->*cc63GetNodePosition)().x;
    const float p3yG = (node3->*cc63GetNodePosition)().y;
    const float p2yH = (node2->*cc63GetNodePosition)().y;
    return static_cast<int8>(Cc63PathDirectionCpp(p2xA, p1xB, p2yC, p1yD, p3xE, p2xF, p3yG, p2yH, arg4));
}

// 0x422620
float CCarCtrl::FindPercDependingOnDistToLink(CVehicle* vehicle, CCarPathLinkAddress linkAddress) {
    // Como el original (0x422620..0x4226EF): sin comprobar que el area este cargada; el enlace (14 bytes) guarda x, y como int16 en +0 y +2 (escala 0.125).
    const auto* const link = reinterpret_cast<const int16*>(&ThePaths.m_pNaviNodes[linkAddress.m_wAreaId][linkAddress.m_wCarPathLinkId]);
    const CVector& pos = vehicle->GetPosition();
    return Cc63PercDistCpp(pos.x, pos.y, link[0], link[1]);
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
    // Aritmetica x87 de 24 bits en X87Math (Cc63SpeedMultiplierCpp), que es tambien el destino del hook.
    return Cc63SpeedMultiplierCpp(arg1, arg2, arg3, arg4);
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

// 0x422370: altura del camino de un coche fantasma, interpolada entre el nodo actual y el siguiente con la distancia 2D del coche a
// cada uno: (distA·zB + distB·zA) / (distB + distA). CPathNode::GetPosition se llama cuatro veces, en el mismo orden que el original
// (A, B, A, B), por la instanciacion fuera de linea de la DLL.
float CCarCtrl::FindGhostRoadHeight(CVehicle* vehicle) {
    static CVector (CPathNode::* volatile const getPosition)() const = &CPathNode::GetPosition;
    constexpr float noHeight = 0.0f; // 0x858B50
    const CNodeAddress current = vehicle->m_autoPilot.m_currentAddress;
    const CNodeAddress next    = vehicle->m_autoPilot.m_startingRouteNode;
    if (current.m_wAreaId == 0xFFFF || next.m_wAreaId == 0xFFFF) {
        return noHeight;
    }
    CPathNode** const areas = ThePaths.m_pPathNodes;
    if (!areas[current.m_wAreaId]) {
        return noHeight;
    }
    if (!areas[next.m_wAreaId]) {
        return noHeight;
    }
    CPathNode* const nodeA = areas[current.m_wAreaId] + current.m_wNodeId;
    CPathNode* const nodeB = areas[next.m_wAreaId] + next.m_wNodeId;

    const float zA = (nodeA->*getPosition)().z;
    const float zB = (nodeB->*getPosition)().z;
    const CVector& carPos = vehicle->GetPosition();
    const CVector  posA   = (nodeA->*getPosition)();
    const float    dxA    = posA.x - carPos.x;
    const float    dyA    = posA.y - carPos.y;
    const float    distA  = std::sqrt(dyA * dyA + dxA * dxA);
    const CVector  posB   = (nodeB->*getPosition)();
    const float    dxB    = posB.x - carPos.x;
    const float    dyB    = posB.y - carPos.y;
    const float    distB  = std::sqrt(dyB * dyB + dxB * dxB);
    return (distA * zB + distB * zA) / (distB + distA);
}

// 0x42B270
void CCarCtrl::FireHeliRocketsAtTarget(CAutomobile* entityLauncher, CEntity* entity) {
    // Como el original: solo con arma 4 o 0 y cuando cambia el cubo de 250 ms del reloj ((t * 0x10624DD3) >> 32) >> 4 entre el fotograma anterior y el actual; el objetivo debe estar a menos
    // de 80.0 y a menos de 7.0 de la recta lanzador -> lanzador + eje adelante (CCollision::DistToMathematicalLine); entonces lanza un cohete (WEAPON_ROCKET) desde la posicion del lanzador
    // + adelante * 4 + derecha * 1.5 (signo segun la paridad del cubo), hacia el eje adelante (m_matrix del lanzador, sin comprobar nulo)
    const uint8 weapon = static_cast<uint8>(entityLauncher->m_nVehicleWeaponInUse);
    if (weapon != 4 && weapon != 0) {
        return;
    }
    const auto bucket = [](uint32 time) { return static_cast<uint32>((static_cast<uint64>(time) * 0x10624DD3ull) >> 32) >> 4; };
    const uint32 previousBucket = bucket(CTimer::m_snPreviousTimeInMilliseconds);
    if (bucket(CTimer::GetTimeInMS()) == previousBucket) {
        return;
    }
    if (!CcAirRocketsInRange(&entityLauncher->GetPosition().x, &entity->GetPosition().x)) {
        return;
    }
    CMatrix& matrix = *entityLauncher->m_matrix;
    CVector point;
    CcAirRocketsPoint(&entityLauncher->GetPosition().x, &matrix.GetForward().x, &point.x);
    if (!(CCollision::DistToMathematicalLine(&entityLauncher->GetPosition(), &point, &entity->GetPosition()) < 7.0f)) {
        return;
    }
    const bool odd = (bucket(CTimer::GetTimeInMS()) & 1) != 0;
    CVector position;
    CcAirRocketsMuzzle(&entityLauncher->GetPosition().x, &matrix.GetForward().x, &matrix.GetRight().x, odd, &position.x);
    CProjectileInfo::AddProjectile(entityLauncher, WEAPON_ROCKET, position, 1.0f, &matrix.GetForward(), nullptr);
}

// 0x429A70
void CCarCtrl::FlyAIHeliInCertainDirection(CHeli* heli, float arg2, float arg3, bool arg4) {
    // Como el original (m_matrix del helicoptero se usa sin comprobar nulo): en mision HELI_NEWS_BEHAVIOUR (0x29), con el byte +0x3DF > arg3 y velocidad horizontal < 0.01, el rumbo gira pi/2 y
    // se anota "news"; una vez por cada 500 ms de reloj (con la semilla del helicoptero) lanza rayos: uno hacia delante en (cos, sin, -1), otro de longitud velocidad * 100 + (5 o 30) y, si choca,
    // mira 10 unidades a cada lado para elegir el giro (+0x994) y marca +0x9B8. Despues calcula freno (+0x998), deslizamiento (+0x98C) y el mando longitudinal (+0x990)
    auto& autoPilot = heli->m_autoPilot;
    const auto byteAt = [heli](size_t offset) { return *(reinterpret_cast<const uint8*>(heli) + offset); };
    char& byte9B8 = reinterpret_cast<char&>(heli->field_9B8);
    bool news = false;
    float heading = arg2;
    if (autoPilot.m_nCarMission == MISSION_HELI_NEWS_BEHAVIOUR) {
        if (CcAirHeliNews(byteAt(0x3DF), arg3, &heli->m_vecMoveSpeed.x)) {
            news = true;
            heading = CcAirAddF(heading, std::bit_cast<float>(0x3FC90FDBu)); // + pi/2 (0x858FE4)
        }
    }
    const uint32 seed = heli->m_nRandomSeed;
    const uint32 previous = (CTimer::m_snPreviousTimeInMilliseconds + seed) % 500u;
    const uint32 current = (CTimer::GetTimeInMS() + seed) % 500u;
    if (current < previous) {
        heli->field_9AC = heli->m_fMaxAltitude; // 0x9A8 -> 0x9AC
        CVector start;
        CcAirFixedStart(&heli->m_vecMoveSpeed.x, &heli->GetPosition().x, &start.x);
        CVector direction;
        CcAirSinCos(heading, &direction.x, &direction.y);
        direction.z = -1.0f;
        const float cosRaw = direction.x, sinRaw = direction.y;
        CarCtrlAirExeNormalise(&direction);
        CVector end;
        CcAirFixedEnd(&direction.x, &start.x, &end.x);
        CColPoint colPoint;
        CEntity* entity;
        if (CWorld::ProcessLineOfSight(start, end, colPoint, entity, true, false, false, false, false, false, false, true)) {
            heli->field_9AC = CcAirHeliRaise1(news, heli->m_fMinAltitude, colPoint.m_vecPoint.z, heli->field_9AC);
        }
        const float lookahead = CcAirHeliLookahead(&heli->m_vecMoveSpeed.x, (byteAt(0x42C) & 0x80) != 0);
        byte9B8 = 1;
        CVector ahead;
        CcAirHeliAhead(cosRaw, sinRaw, lookahead, &start.x, &ahead.x);
        if (!CWorld::ProcessLineOfSight(start, ahead, colPoint, entity, true, false, false, false, false, false, false, true)) {
            byte9B8 = 0;
            heli->m_fSteeringLeftRight = 0.0f; // 0x994
        } else {
            const CMatrix& matrix = *heli->m_matrix;
            CVector right = matrix.GetRight();
            right.z = 0.0f;
            CarCtrlAirExeNormalise(&right);
            CVector sideStart;
            CcAirHeliSide(&right.x, &heli->GetPosition().x, 0, &sideStart.x);
            CVector sideEnd;
            CcAirHeliAhead(cosRaw, sinRaw, lookahead, &sideStart.x, &sideEnd.x);
            float rightFree = 1000.0f;
            if (CWorld::ProcessLineOfSight(heli->GetPosition(), sideStart, colPoint, entity, true, false, false, false, false, false, false, true)) {
                rightFree = 0.0f;
            } else if (CWorld::ProcessLineOfSight(sideStart, sideEnd, colPoint, entity, true, false, false, false, false, false, false, true)) {
                rightFree = static_cast<float>(CcAirHeliHitDist(&colPoint.m_vecPoint.x, &sideStart.x));
            }
            CcAirHeliSide(&right.x, &heli->GetPosition().x, 1, &sideStart.x);
            CcAirHeliAhead(cosRaw, sinRaw, lookahead, &sideStart.x, &sideEnd.x);
            double leftFree = 1000.0f;
            if (CWorld::ProcessLineOfSight(heli->GetPosition(), sideStart, colPoint, entity, true, false, false, false, false, false, false, true)) {
                leftFree = 0.0f;
            } else if (CWorld::ProcessLineOfSight(sideStart, sideEnd, colPoint, entity, true, false, false, false, false, false, false, true)) {
                leftFree = CcAirHeliHitDist(&colPoint.m_vecPoint.x, &sideStart.x);
            }
            heli->m_fSteeringLeftRight = (leftFree > rightFree) ? 0.5f : -0.5f; // 0x994
        }
    }
    if (autoPilot.m_nCarMission == MISSION_HELI_LAND_TOUCHING_DOWN) {
        byte9B8 = 0;
        heli->m_fSteeringLeftRight = 0.0f;
    }
    const CMatrix& matrix = *heli->m_matrix;
    const float ownHeading = CGeneral::GetATanOfXY(matrix.GetForward().x, matrix.GetForward().y);
    heli->m_fAccelerationBreakStatus = 0.0f; // 0x998
    heli->m_fAccelerationBreakStatus = CcAirFixedBrake(heli->m_vecMoveSpeed.z, heli->GetPosition().z, heli->field_9AC);
    const int random = CarCtrlAirExeRand() & 0xF;
    float m;
    heli->m_fAccelerationBreakStatus = CcAirHeliBrake2(random, heli->m_fAccelerationBreakStatus, &m);
    heli->m_fLeftRightSkid = CcAirHeliSkid(heading, ownHeading); // 0x98C
    if (arg3 > 60.0f || !arg4) {
        heli->m_fSteeringUpDown = std::bit_cast<float>(0xBF4CCCCDu); // 0x990: -0.8f
    } else {
        float approach;
        if (CcAirHeliApproach(&heli->m_vecMoveSpeed.x, &heli->GetPosition().x, &autoPilot.m_vecDestinationCoors.x, byteAt(0x3DF), &approach)) {
            heli->m_fSteeringUpDown = approach;
        }
    }
    if (heli->m_fSteeringUpDown < 0.0f) {
        float limited;
        CcAirHeliTurnLimit(ownHeading, heading, heli->m_fSteeringUpDown, &heli->m_vecMoveSpeed.x, &matrix.GetForward().x, &limited);
        heli->m_fSteeringUpDown = limited;
    }
    if (m > 0.0f) {
        heli->m_fSteeringUpDown = CcAirHeliScale(m, heli->m_fSteeringUpDown);
    }
    if (heli->m_fSteeringUpDown < 0.0f) {
        float stored;
        const double value = CcAirHeliSpeedCtl(byteAt(0x3D0), &heli->m_vecMoveSpeed.x, &matrix.GetForward().x, heli->m_fSteeringUpDown, &stored);
        heli->m_fSteeringUpDown = stored;
        if (byteAt(0x3D9) != 0 && value < std::bit_cast<float>(0xBE4CCCCDu) && byte9B8 == 0) { // -0.2f (0x858FDC)
            CcAirHeliDecay(static_cast<int8>(byteAt(0x3D9)), CTimer::ms_fTimeStep, &matrix.GetForward().x, &heli->m_vecMoveSpeed.x);
        }
    }
    if (byte9B8 != 0) {
        CVector forward2D = matrix.GetForward();
        forward2D.z = 0.0f;
        CarCtrlAirExeNormalise(&forward2D);
        heli->m_fSteeringUpDown = CcAirHeliForwardDot(&forward2D.x, &heli->m_vecMoveSpeed.x);
    }
    heli->m_fSteeringUpDown = CcAirHeliClamp990(heli->m_fSteeringUpDown);
}

// 0x423940
void CCarCtrl::FlyAIHeliToTarget_FixedOrientation(CHeli* heli, float Orientation, CVector posn) {
    // Como el original (el EXE recibe ademas un cuarto argumento, true, que no lee): una vez por cada 500 ms de reloj (con la semilla del helicoptero) copia la altitud maxima (+0x9A8 -> +0x9AC),
    // lanza un rayo desde la posicion + velocidad x 50 en la direccion (cos, sin, -1) normalizada x 60 y, si choca, +0x9AC = max(+0x9AC, z del choque + altitud minima +0x9B0).
    // Despues calcula el freno (+0x998), el deslizamiento (+0x98C) y los mandos lateral (+0x994) y longitudinal (+0x990) hacia el destino (m_matrix sin comprobar nulo)
    const uint32 seed = heli->m_nRandomSeed;
    const uint32 previous = (CTimer::m_snPreviousTimeInMilliseconds + seed) % 500u;
    const uint32 current = (CTimer::GetTimeInMS() + seed) % 500u;
    if (current < previous) {
        heli->field_9AC = heli->m_fMaxAltitude;
        CVector start;
        CcAirFixedStart(&heli->m_vecMoveSpeed.x, &heli->GetPosition().x, &start.x);
        CVector direction;
        CcAirSinCos(Orientation, &direction.x, &direction.y);
        direction.z = -1.0f;
        CarCtrlAirExeNormalise(&direction);
        CVector end;
        CcAirFixedEnd(&direction.x, &start.x, &end.x);
        CColPoint colPoint;
        CEntity* entity;
        if (CWorld::ProcessLineOfSight(start, end, colPoint, entity, true, false, false, false, false, false, false, true)) {
            heli->field_9AC = CcAirFixedMaxAlt(colPoint.m_vecPoint.z, heli->m_fMinAltitude, heli->field_9AC);
        }
    }
    const CMatrix& matrix = *heli->m_matrix;
    const float ownHeading = CGeneral::GetATanOfXY(matrix.GetForward().x, matrix.GetForward().y);
    heli->m_fAccelerationBreakStatus = 0.0f; // 0x998
    heli->m_fAccelerationBreakStatus = CcAirFixedBrake(heli->m_vecMoveSpeed.z, heli->GetPosition().z, heli->field_9AC);
    const int random = CarCtrlAirExeRand() & 0xF;
    float out[7];
    CcAirFixedControls(Orientation, ownHeading, heli->m_fAccelerationBreakStatus, random, &heli->m_vecMoveSpeed.x, &matrix.GetRight().x, &matrix.GetForward().x, &heli->GetPosition().x, &posn.x, out);
    heli->m_fAccelerationBreakStatus = out[0];
    heli->m_fLeftRightSkid = out[1]; // 0x98C (sin limitar y limitado)
    heli->m_fLeftRightSkid = out[2];
    heli->m_fSteeringLeftRight = out[3]; // 0x994
    heli->m_fSteeringLeftRight = out[4];
    heli->m_fSteeringUpDown = out[5]; // 0x990
    heli->m_fSteeringUpDown = out[6];
}

// 0x423000
void CCarCtrl::FlyAIPlaneInCertainDirection(CPlane* pPlane) {
    // Como el original. Una vez por segundo de reloj (con la semilla del avion) sondea rumbos alternativos con la funcion 0x422F80 del EXE (dos pasadas sobre 19 candidatos a +-15 grados) y fija
    // +0x9B8 (rumbo) y +0x9AC (altitud); despues calcula, segun el avion lleve mas o menos de 4 s sin tocar el suelo, los mandos de cabeceo (+0x98C), alabeo (+0x988), timon (+0x990) y
    // aceleracion (+0x994). El local del limite de rumbo del bloque periodico comparte la posicion de pila ([esp+0x10]) con el rumbo propio que se guarda despues, y el segundo es el que
    // leen los mandos (rumbo deseado +0x9B8 menos rumbo propio). m_matrix se usa sin comprobar nulo
    CPlane* const plane = pPlane;
    const uint32 seed = plane->m_nRandomSeed;
    const uint32 previous = (CTimer::m_snPreviousTimeInMilliseconds + seed) % 1000u;
    const uint32 current = (CTimer::GetTimeInMS() + seed) % 1000u;
    if (current < previous) {
        float target;
        const CMatrix& matrix = *plane->m_matrix;
        const float ownHeading = CGeneral::GetATanOfXY(matrix.GetForward().x, matrix.GetForward().y);
        target = plane->m_planeHeading;
        switch (plane->m_autoPilot.m_nTempAction) {
        case TEMPACT_PLANE_FLY_STRAIGHT:
            target = ownHeading;
            break;
        case TEMPACT_PLANE_SHARP_LEFT:
            target = CcAirSubF(ownHeading, 2.0f);
            break;
        case TEMPACT_PLANE_SHARP_RIGHT:
            target = CcAirAddF(2.0f, ownHeading);
            break;
        default:
            break;
        }
        plane->m_altitude = std::bit_cast<float>(0x43FA0000u); // 0x9AC: 500.0f
        plane->m_planeHeadingPrev = CcAirAddF(plane->m_planeHeading, std::bit_cast<float>(0x40490FDBu)); // 0x9B8
        float limit;
        CcAirPlaneLimitCalc(ownHeading, target, &limit);
        for (int pass = 0; pass < 2; ++pass) {
            for (int n = 1; n < 20; ++n) {
                float s, t;
                int inRange;
                CcAirPlaneCand(n, target, limit, &s, &t, &inRange);
                if (pass == 0 ? !inRange : inRange != 0) {
                    continue;
                }
                const float clearance = reinterpret_cast<float(__cdecl*)(CPlane*, float)>(0x422F80)(plane, t);
                if (!(clearance < 150.0f)) {
                    continue;
                }
                plane->m_planeHeadingPrev = CcAirPlaneAccept(s, plane->m_planeHeading); // 0x9B8
                plane->m_altitude = CcAirPlaneRaise(clearance, plane->m_minAltitude, plane->m_maxAltitude); // 0x9AC
                break;
            }
        }
    }
    const uint8 flags42C = *(reinterpret_cast<const uint8*>(plane) + 0x42C);
    const float v28 = (flags42C & 0x80) ? *reinterpret_cast<const float*>(0x8A5B54) : 32.0f;
    const CMatrix& matrix = *plane->m_matrix;
    const float ownHeading2 = CGeneral::GetATanOfXY(matrix.GetForward().x, matrix.GetForward().y);
    const float speed60 = CcAirPlaneSpeed60(&matrix.GetForward().x, &plane->m_vecMoveSpeed.x);
    const auto& compression = plane->m_fWheelsSuspensionCompression;
    if (compression[0] < 1.0f || compression[1] < 1.0f || compression[2] < 1.0f || compression[3] < 1.0f) {
        plane->m_nStartedFlyingTime = CTimer::GetTimeInMS(); // 0x9C0
    }
    const bool ground = (CTimer::GetTimeInMS() - plane->m_nStartedFlyingTime) <= 4000u;
    if (!ground) {
        if (std::bit_cast<uint32>(plane->m_fLandingGearStatus) != 0x3F800000u) {
            plane->SetGearUp();
        }
        const float pz = CcAirPlanePz(plane->m_vecMoveSpeed.z, plane->GetPosition().z);
        float new9BC, out98C;
        CcAirPlaneAir(matrix.GetForward().z, plane->m_forwardZ, CTimer::ms_fTimeStep, *reinterpret_cast<const float*>(0x8A5B50), plane->m_altitude, pz, speed60, v28, &new9BC, &out98C);
        plane->m_forwardZ = new9BC; // 0x9BC
        plane->m_fSteeringUpDown = out98C; // 0x98C
    } else {
        plane->m_fSteeringUpDown = (speed60 < v28) ? 0.0f : std::bit_cast<float>(0x3ECCCCCDu);
    }
    float out[4];
    CcAirPlaneFinish(ground, plane->m_planeHeadingPrev, ownHeading2, plane->m_planeCreationHeading, plane->m_fSteeringUpDown, plane->m_fSteeringFactor, (flags42C & 0x80) != 0, v28, speed60,
                     CTimer::ms_fTimeStep, *reinterpret_cast<const float*>(0x8A5B4C), *reinterpret_cast<const float*>(0x8A5B48), *reinterpret_cast<const float*>(0x8A5B44), &matrix.GetRight().x, out);
    plane->m_fLeftRightSkid = out[0]; // 0x988
    if (!ground) {
        plane->m_fSteeringFactor = out[2]; // 0x99C: balanceo en bruto
    }
    plane->m_fSteeringLeftRight = out[1]; // 0x990
    plane->m_fSteeringUpDown = CcAirPlaneClamp98C(out[3]); // 0x98C, limitado a [-1, 1]
    plane->m_fAccelerationBreakStatus = plane->m_fAccelerationBreakStatusPrev; // 0x994 = 0x998 (copia de 32 bits)
    if (plane->m_autoPilot.m_nTempAction == TEMPACT_PLANE_FLY_UP) {
        plane->m_fSteeringUpDown = 1.0f;
        if (speed60 < 20.0) {
            plane->m_autoPilot.m_nTempAction = TEMPACT_NONE;
        }
    }
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
    // Como el original: ocho llamadas a FindPlayerCoors(-1) y cada una aporta un dato distinto (x del rumbo, y del rumbo, x e y de la distancia, z a +0x9A8, destino +0x3EC);
    // en mision HELI_ATTACK_PLAYER (23) a menos de 15.0 pasa a la 56 y suma 50.0 a la distancia; en la 56 a mas de 18.0 vuelve a la 23 y suma pi al rumbo;
    // despues FlyAIHeliInCertainDirection(rumbo, distancia, false), TestWhetherToFirePlaneGuns y FireHeliRocketsAtTarget contra FindPlayerEntity(-1)
    auto* const heli = static_cast<CHeli*>(automobile);
    auto& autoPilot = heli->m_autoPilot;
    const CVector c1 = FindPlayerCoors(-1);
    const CVector c2 = FindPlayerCoors(-1);
    const float hy = CcAirSubF(c2.y, heli->GetPosition().y);
    const float hx = CcAirSubF(c1.x, heli->GetPosition().x);
    float heading = CGeneral::GetATanOfXY(hx, hy);
    const CVector c3 = FindPlayerCoors(-1);
    const CVector c4 = FindPlayerCoors(-1);
    const CVector c5 = FindPlayerCoors(-1);
    const CVector c6 = FindPlayerCoors(-1);
    float distance = CcAirHeliAttackDist(c3.x, c4.x, c5.y, c6.y, &heli->GetPosition().x);
    const CVector c7 = FindPlayerCoors(-1);
    heli->m_fMaxAltitude = c7.z; // 0x9A8
    const CVector c8 = FindPlayerCoors(-1);
    autoPilot.m_vecDestinationCoors = c8; // 0x3EC
    const auto mission = autoPilot.m_nCarMission;
    if (mission == MISSION_HELI_ATTACK_PLAYER) {
        if (distance < 15.0f) {
            autoPilot.m_nCarMission = MISSION_HELI_ATTACK_PLAYER_FLY_AWAY;
        }
        distance = CcAirAddF(distance, 50.0f);
    } else if (mission == MISSION_HELI_ATTACK_PLAYER_FLY_AWAY) {
        if (distance > 18.0f) {
            autoPilot.m_nCarMission = MISSION_HELI_ATTACK_PLAYER;
        }
        heading = CcAirAddF(heading, std::bit_cast<float>(0x40490FDBu));
    }
    FlyAIHeliInCertainDirection(heli, heading, distance, false);
    TestWhetherToFirePlaneGuns(heli, FindPlayerEntity(-1));
    FireHeliRocketsAtTarget(heli, FindPlayerEntity(-1));
}

// 0x42A730
void CCarCtrl::GetAIHeliToFlyInDirection(CAutomobile* automobile) {
    // Como el original: pasa +0x9B4 (CHeli::field_9B4, copia de 32 bits), 1000.0f y false
    auto* const heli = static_cast<CHeli*>(automobile);
    FlyAIHeliInCertainDirection(heli, heli->field_9B4, 1000.0f, false);
}

// 0x429780
void CCarCtrl::GetAIPlaneToAttackPlayer(CAutomobile* automobile) {
    // Como el original: apunta al jugador adelantado por su velocidad x 50.0 (FindPlayerSpeed y despues FindPlayerCoors); rumbo = GetATanOfXY a +0x9B4, altitud maxima = z en bruto
    // a +0x9A8; despues FlyAIPlaneInCertainDirection y, con el vehiculo del jugador (FindPlayerVehicle se llama en cada uso), TriggerDogFightMoves solo si es un avion,
    // TestWhetherToFirePlaneGuns y PossiblyFireHSMissile
    auto* const plane = static_cast<CPlane*>(automobile);
    float scaled[3];
    CcAirScale3(&FindPlayerSpeed(-1).x, std::bit_cast<float>(0x42480000u), scaled); // 50.0f (0x858B40)
    const CVector playerCoors = FindPlayerCoors(-1);
    float aim[3];
    CcAirAttackAim(scaled, &playerCoors.x, &plane->GetPosition().x, aim);
    plane->m_planeHeading = CGeneral::GetATanOfXY(aim[0], aim[1]); // 0x9B4
    plane->m_maxAltitude = aim[2]; // 0x9A8
    FlyAIPlaneInCertainDirection(plane);
    if (FindPlayerVehicle(-1, false)) {
        if (FindPlayerVehicle(-1, false)->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE) {
            TriggerDogFightMoves(plane, FindPlayerVehicle(-1, false));
        }
    }
    TestWhetherToFirePlaneGuns(plane, FindPlayerVehicle(-1, false));
    PossiblyFireHSMissile(plane, FindPlayerVehicle(-1, false));
}

// 0x429890
void CCarCtrl::GetAIPlaneToDoDogFight(CAutomobile* automobile) {
    // Como el original: el objetivo (+0x41C) se lee una vez al principio y se usa tambien al final. Punto de mira = posicion del objetivo + su velocidad x 50.0.
    // Con +0x425 (int8) distinto de 1 apunta al punto de mira y, con rand() & 0x3FF == 500, pasa a +0x425 = 1 y fija un destino aleatorio (x, y = mira + rand * 600 / 32767 - 300; z = mira.z + 50);
    // con +0x425 == 1 apunta al destino (+0x3EC) y lo suelta (+0x425 = 0) a menos de 50.0
    auto* const plane = static_cast<CPlane*>(automobile);
    auto& autoPilot = plane->m_autoPilot;
    CVehicle* const target = autoPilot.m_TargetEntity;
    float aim[3];
    CcAirDogAim(&target->m_vecMoveSpeed.x, &target->GetPosition().x, aim);
    if (reinterpret_cast<const int8&>(autoPilot.m_bPlaneDogfightSomething) != 1) {
        const float dy = CcAirSubF(aim[1], plane->GetPosition().y);
        const float dx = CcAirSubF(aim[0], plane->GetPosition().x);
        plane->m_planeHeading = CGeneral::GetATanOfXY(dx, dy); // 0x9B4
        plane->m_maxAltitude = aim[2]; // 0x9A8
        if ((CarCtrlAirExeRand() & 0x3FF) == 500) {
            autoPilot.m_bPlaneDogfightSomething = true; // 0x425
            const int randX = CarCtrlAirExeRand();
            autoPilot.m_vecDestinationCoors.x = CcAirDogRandOffset(randX, aim[0]);
            const int randY = CarCtrlAirExeRand();
            autoPilot.m_vecDestinationCoors.y = CcAirDogRandOffset(randY, aim[1]);
            autoPilot.m_vecDestinationCoors.z = CcAirAddF(aim[2], std::bit_cast<float>(0x42480000u)); // + 50.0f
        }
    } else {
        const CVector& dest = autoPilot.m_vecDestinationCoors;
        const float dy = CcAirSubF(dest.y, plane->GetPosition().y);
        const float dx = CcAirSubF(dest.x, plane->GetPosition().x);
        plane->m_planeHeading = CGeneral::GetATanOfXY(dx, dy); // 0x9B4
        plane->m_maxAltitude = dest.z; // 0x9A8
        if (CcAirHorizDistD(&dest.x, &plane->GetPosition().x) < 50.0f) {
            autoPilot.m_bPlaneDogfightSomething = false;
        }
    }
    FlyAIPlaneInCertainDirection(plane);
    TestWhetherToFirePlaneGuns(plane, target);
    PossiblyFireHSMissile(plane, target);
}

// 0x42F370
void CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer(CAutomobile* automobile) {
    // Como el original: FindPlayerVehicle se llama dos veces (la primera solo comprueba); sin vehiculo el objetivo es el jugador (FindPlayerPed).
    // El final es una llamada en cola a GetAIPlaneToDoDogFight (0x429890).
    if (FindPlayerVehicle(-1, false)) {
        automobile->m_autoPilot.m_TargetEntity = FindPlayerVehicle(-1, false);
    } else {
        automobile->m_autoPilot.m_TargetEntity = reinterpret_cast<CVehicle*>(FindPlayerPed(-1));
    }
    GetAIPlaneToDoDogFight(automobile);
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
    // Como el original (0x42DAB0..0x42DE53). nodo3 sin cargar o igual al nodo1 -> false; el nodo2 se lee SIN comprobar su area. Los bytes +0x18 de los nodos 2 y 3 (bit 7 = agua; 0x10 y 0x20)
    // y el byte +0x1A del 3 (>> 4 = tipo 1..10; el resto, tipo por defecto): si difiere el bit de agua y el modelo no es 539 (0x21B) -> false. Tipo 1: no de mision (+0x4A4 == 2), conductor
    // (+0x460) sin byte +0x484 == 2, informacion del modelo (+0x4D) != 5 y, si !arg5, el producto vectorial z de ((nodo3 - nodo2) x (nodo3 - posicion)) no negativo; tipo 2: lo mismo pero sin la
    // comprobacion del modelo al principio y con DotProduct(derecha del vehiculo, nodo3 - posicion) no negativo; los dos continuan con IsLawEnforcementVehicle, modelo != 5, conductor con +0x598 != 0x14,
    // IsAnyoneParking y FindObjectsKindaColliding (radio 5.0f, solo vehiculos, maximo 2) sin objetos. Tipos 5 y 10: arg5 -> false y col.max.y (modelo +0x14, +0x14) < 2.0f -> false; tipos 8 y 9:
    // arg5 -> false, +0x14 > 1.5f, +0x0C > 2.0f o +0x10 > 4.0f -> false. Los tipos 5, 8, 9 y 10 que sobreviven, y los demas, siguen con las banderas de nodo y devuelven !arg5.
    auto* const raw = reinterpret_cast<uint8*>(vehicle);
    CPathNode* const area3 = ThePaths.m_pPathNodes[nodeAddress3.m_wAreaId];
    if (!area3) {
        return false;
    }
    if (nodeAddress1.m_wAreaId == nodeAddress3.m_wAreaId && nodeAddress1.m_wNodeId == nodeAddress3.m_wNodeId) {
        return false;
    }
    const uint8* const node2 = reinterpret_cast<const uint8*>(ThePaths.m_pPathNodes[nodeAddress2.m_wAreaId]) + static_cast<size_t>(nodeAddress2.m_wNodeId) * 0x1C;
    const uint8* const node3 = reinterpret_cast<const uint8*>(area3) + static_cast<size_t>(nodeAddress3.m_wNodeId) * 0x1C;
    const uint8 flags2 = node2[0x18];
    const uint8 flags3 = node3[0x18];
    if (((flags2 ^ flags3) & 0x80) && static_cast<int16>(vehicle->m_nModelIndex) != 0x21B) {
        return false;
    }
    const auto modelInfo = [&] { return reinterpret_cast<const uint8*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(vehicle->m_nModelIndex)]); };
    const auto driver = [&] { return *reinterpret_cast<const uint8* const*>(raw + 0x460); };
    const auto nodePosition = [](const uint8* node) {
        return CVector{ static_cast<float>(*reinterpret_cast<const int16*>(node + 8)) * 0.125f, static_cast<float>(*reinterpret_cast<const int16*>(node + 0xA)) * 0.125f,
                        static_cast<float>(*reinterpret_cast<const int16*>(node + 0xC)) * 0.125f };
    };
    const auto freeOfObjects = [&] {
        const CVector position = nodePosition(node3);
        int16 count;
        CWorld::FindObjectsKindaColliding(position, 5.0f, true, &count, 2, nullptr, false, true, false, false, false);
        return count == 0;
    };
    const auto commonTail = [&]() -> bool {
        if (vehicle->IsLawEnforcementVehicle()) {
            return false;
        }
        if (modelInfo()[0x4D] == 5) {
            return false;
        }
        if (const uint8* const d = driver(); d && *reinterpret_cast<const int32*>(d + 0x598) == 0x14) {
            return false;
        }
        if (IsAnyoneParking()) {
            return false;
        }
        return freeOfObjects();
    };
    const uint32 kind = static_cast<uint32>(node3[0x1A] >> 4) - 1u;
    bool useDefault = false;
    if (kind == 0 || kind == 1) {
        if (raw[0x4A4] == 2) {
            return false;
        }
        if (const uint8* const d = driver(); d && d[0x484] == 2) {
            return false;
        }
        if (kind == 0 && modelInfo()[0x4D] == 5) {
            return false;
        }
        if (!arg5) {
            const CVector position3 = nodePosition(node3);
            const CVector position2 = nodePosition(node2);
            const CVector& point = vehicle->GetPosition();
            if (kind == 0) {
                if (Cc63fCrossZNegativeCpp(&position3.x, &point.x, &position2.x)) {
                    return false;
                }
            } else {
                CVector right;
                reinterpret_cast<CVector*(__thiscall*)(CVehicle*, CVector*)>(0x41CC70)(vehicle, &right);
                if (Cc63fRightDotNegativeCpp(&right.x, &position3.x, &point.x)) {
                    return false;
                }
            }
        }
        return commonTail();
    } else if (kind == 4 || kind == 9) {
        if (arg5) {
            return false;
        }
        const float* const col = *reinterpret_cast<const float* const*>(modelInfo() + 0x14);
        if (col[5] < 2.0f) {
            return false;
        }
        useDefault = true;
    } else if (kind == 7 || kind == 8) {
        if (arg5) {
            return false;
        }
        const float* const col = *reinterpret_cast<const float* const*>(modelInfo() + 0x14);
        if (col[5] > 1.5f) {
            return false;
        }
        if (col[3] > 2.0f) {
            return false;
        }
        if (col[4] > 4.0f) {
            return false;
        }
        useDefault = true;
    } else {
        useDefault = true;
    }
    (void)useDefault;
    if (raw[0x4A4] == 2 && raw[0x3BA] == 1) {
        if ((node3[0x19] & 4) && !(node2[0x19] & 4)) {
            return false;
        }
    }
    if ((flags3 & 0x10) && !(flags2 & 0x10)) {
        return false;
    }
    if ((flags3 & 0x20) && !(flags2 & 0x20)) {
        return false;
    }
    return !arg5;
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
    // Como el original (0x42F5A0..0x42F86B). Se borran las direcciones de nodo/enlace del piloto automatico (areas = 0xFFFF; contador de ruta = 0) y se busca el nodo mas cercano
    // favoreciendo la direccion del vehiculo (FindNodeClosestToCoorsFavourDirection, matriz SIN comprobar nulo: lee +0x10/+0x14). Entre sus enlaces cargados se elige el mas cercano al propio
    // nodo (distancia x87 de 24 bits en X87Math, comparacion ordenada `<` contra 1000000.0f, 0x497423FE) y, segun el signo del producto escalar (nodo - enlace) . adelante (adelante = 1.0f en x
    // si ambos son 0), el piloto pasa a ir (nodo -> enlace) o (enlace -> nodo): +0x390 = actual, +0x394 = siguiente. Despues FindLinksToGoWithTheseNodes y se ponen a 0 los carriles.
    auto& autoPilot = vehicle->m_autoPilot;
    autoPilot.m_endingRouteNode.m_wAreaId = 0xFFFF;
    autoPilot.m_currentAddress.m_wAreaId = 0xFFFF;
    autoPilot.m_startingRouteNode.m_wAreaId = 0xFFFF;
    *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(vehicle) + 0x3A4) = 0xFFFF; // m_nCurrentPathNodeInfo
    *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(vehicle) + 0x3A8) = 0xFFFF; // m_nPreviousPathNodeInfo
    *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(vehicle) + 0x3A6) = 0xFFFF; // m_nNextPathNodeInfo
    autoPilot.m_nPathFindNodesCount = 0;
    const CMatrix* const matrix = vehicle->m_matrix;
    const float forwardX = matrix->GetForward().x;
    const float forwardY = matrix->GetForward().y;
    const CVector position = vehicle->GetPosition();
    const CNodeAddress start = ThePaths.FindNodeClosestToCoorsFavourDirection(position, static_cast<ePathType>(0), CVector2D{ forwardX, forwardY });
    if (start.m_wAreaId == 0xFFFF) {
        return;
    }
    CPathNode* const startArea = ThePaths.m_pPathNodes[start.m_wAreaId];
    if (!startArea) {
        return;
    }
    const uint8* const startNode = reinterpret_cast<const uint8*>(startArea + start.m_wNodeId);
    const int16 numLinks = startNode[0x18] & 0x0F;
    float best = std::bit_cast<float>(0x497423FEu);
    uint16 bestArea = 0xFFFF; // el dword completo solo es valido si el area no es 0xFFFF
    uint32 bestLink = 0;
    if (numLinks <= 0) {
        return;
    }
    const uint32* links = reinterpret_cast<const uint32*>(ThePaths.m_pNodeLinks[start.m_wAreaId]) + *reinterpret_cast<const int16*>(startNode + 0x10);
    const int startX = *reinterpret_cast<const int16*>(startNode + 8);
    const int startY = *reinterpret_cast<const int16*>(startNode + 0xA);
    for (int16 remaining = numLinks; remaining != 0; --remaining, ++links) {
        const uint32 link = *links;
        CPathNode* const linkArea = ThePaths.m_pPathNodes[link & 0xFFFF];
        if (!linkArea) {
            continue;
        }
        const uint8* const linkNode = reinterpret_cast<const uint8*>(linkArea) + (link >> 16) * 0x1C;
        const float distance = Cc63cLinkDistanceCpp(*reinterpret_cast<const int16*>(linkNode + 8), *reinterpret_cast<const int16*>(linkNode + 0xA), startX, startY);
        if (distance < best) {
            best = distance;
            bestLink = link;
            bestArea = static_cast<uint16>(link);
        }
    }
    if (bestArea == 0xFFFF) {
        return;
    }
    const CMatrix* const matrix2 = vehicle->m_matrix;
    const uint8* const bestNode = reinterpret_cast<const uint8*>(ThePaths.m_pPathNodes[bestArea]) + (bestLink >> 16) * 0x1C;
    const bool negative = Cc63cDotNegativeCpp(startX, startY, *reinterpret_cast<const int16*>(bestNode + 8), *reinterpret_cast<const int16*>(bestNode + 0xA),
                                              matrix2->GetForward().x, matrix2->GetForward().y);
    uint32 current, next;
    if (negative) {
        current = std::bit_cast<uint32>(start);
        next = bestLink;
    } else {
        current = bestLink;
        next = std::bit_cast<uint32>(start);
    }
    autoPilot.m_endingRouteNode.m_wAreaId = 0xFFFF;
    *reinterpret_cast<uint32*>(&autoPilot.m_currentAddress) = current;
    *reinterpret_cast<uint32*>(&autoPilot.m_startingRouteNode) = next;
    FindLinksToGoWithTheseNodes(vehicle);
    autoPilot.m_nCurrentLane = 0;
    autoPilot.m_nNextLane = 0;
}

// 0x42F870
bool CCarCtrl::JoinCarWithRoadSystemGotoCoors(CVehicle* vehicle, const CVector& posn, bool unused, bool bIsBoat) {
    // Como el original (0x42F870..0x42F9B6). El destino se copia tal cual (bits) a +0x3EC; DoPathSearch(tipo 0, posicion del vehiculo, [0x8A5F44] (direccion invalida), destino, ruta (+0x3F8), contador (+0x418),
    // 8, sin distancia, 1000000.0f (0x497423FE), sin nodo de destino, 1000000.0f, bit 6 de +0x3DB, [0x8A5F44] (prohibido), modelo == 539 (0x21B), bIsBoat); RemoveBadStartNode (0x44E4F0, sin reconstruir:
    // por direccion) con la posicion releida; si el contador (releido, con signo) es < 2: JoinCarWithRoadSystem y contador = 0 -> true; si no: actual = ruta[0] (+0x390), +0x398 (area) = 0xFFFF,
    // RemoveOnePathNode, siguiente = ruta[0] (+0x394), RemoveOnePathNode, FindLinksToGoWithTheseNodes, carriles (+0x3B7, +0x3B8) = 0 -> false.
    auto& autoPilot = vehicle->m_autoPilot;
    std::memcpy(&autoPilot.m_vecDestinationCoors, &posn, sizeof(CVector));
    const bool isVortex = static_cast<int16>(vehicle->m_nModelIndex) == 0x21B;
    CNodeAddress* const nodes = autoPilot.m_aPathFindNodesInfo.data();
    int16& count = reinterpret_cast<int16&>(autoPilot.m_nPathFindNodesCount);
    const CNodeAddress invalidAddress = StaticRef<CNodeAddress>(0x8A5F44);
    ThePaths.DoPathSearch(static_cast<ePathType>(0), vehicle->GetPosition(), invalidAddress, posn, nodes, count, 8, nullptr, std::bit_cast<float>(0x497423FEu), nullptr,
                          std::bit_cast<float>(0x497423FEu), autoPilot.carCtrlFlags.bCantGoAgainstTraffic, StaticRef<CNodeAddress>(0x8A5F44), isVortex, bIsBoat);
    plugin::CallMethod<0x44E4F0, CPathFind*, CVector, CNodeAddress*, int16*>(&ThePaths, vehicle->GetPosition(), nodes, &count);
    if (count < 2) {
        JoinCarWithRoadSystem(vehicle);
        count = 0;
        return true;
    }
    autoPilot.m_currentAddress = nodes[0];
    autoPilot.m_endingRouteNode.m_wAreaId = 0xFFFF;
    autoPilot.RemoveOnePathNode();
    autoPilot.m_startingRouteNode = nodes[0];
    autoPilot.RemoveOnePathNode();
    FindLinksToGoWithTheseNodes(vehicle);
    autoPilot.m_nCurrentLane = 0;
    autoPilot.m_nNextLane = 0;
    return false;
}

// 0x432B10
bool CCarCtrl::PickNextNodeAccordingStrategy(CVehicle* vehicle) {
    // Como el original (0x432B10..0x432C50). De +0x394 (siguiente nodo de la ruta) se leen, SIN comprobar que el area este cargada, los bytes +0x19 y +0x1A del nodo:
    // +0x3D1 = (byte19 >> 4) & 3 y, si byte1A >> 4 esta en 8..9, +0x3D1 = 0xFF. Segun la mision (int8 en +0x3BA; mision - 2 como sin signo <= 0x42, tabla de bytes en 0x432C64):
    // misiones 2, 4 y 43 persiguen al jugador: FindPlayerCoors(-1), FindPlayerVehicle(-1, 0), FindPlayerCoors(-1), FindPlayerCoors(-1) y PickNextNodeToChaseCar(veh, x de la 1a, y de la 3a, z de la 2a)
    // (el EXE empuja ademas el vehiculo del jugador como dword sobrante); 15, 17, 52, 54, 61 y 65..68 persiguen a +0x41C (objetivo, sin comprobar nulo) con su posicion;
    // 8 y 12 siguen la ruta (PickNextNodeToFollowPath, su resultado); el resto: PickNextNodeRandomly y false.
    auto& autoPilot = vehicle->m_autoPilot;
    const CNodeAddress next = autoPilot.m_startingRouteNode;
    const uint8* const nodeBase = reinterpret_cast<const uint8*>(ThePaths.m_pPathNodes[next.m_wAreaId] + next.m_wNodeId);
    autoPilot.field_41 = static_cast<char>((nodeBase[0x19] >> 4) & 3);
    {
        const uint8* const nodeAgain = reinterpret_cast<const uint8*>(ThePaths.m_pPathNodes[next.m_wAreaId] + next.m_wNodeId);
        const uint8 kind = nodeAgain[0x1A] >> 4;
        if (kind >= 8 && kind <= 9) {
            autoPilot.field_41 = static_cast<char>(0xFF);
        }
    }
    const uint32 index = static_cast<uint32>(static_cast<int32>(static_cast<int8>(autoPilot.m_nCarMission)) - 2);
    if (index <= 0x42) {
        switch (index + 2) {
        case 2: case 4: case 43: {
            const CVector first = FindPlayerCoors(-1);
            (void)FindPlayerVehicle(-1, false);
            const CVector second = FindPlayerCoors(-1);
            const CVector third = FindPlayerCoors(-1);
            return PickNextNodeToChaseCar(vehicle, first.x, third.y, second.z);
        }
        case 15: case 17: case 52: case 54: case 61: case 65: case 66: case 67: case 68: {
            CEntity* const target = autoPilot.m_TargetEntity;
            const CVector position = target->GetPosition();
            return PickNextNodeToChaseCar(vehicle, position.x, position.y, position.z);
        }
        case 8: case 12:
            return PickNextNodeToFollowPath(vehicle);
        default:
            break;
        }
    }
    PickNextNodeRandomly(vehicle);
    return false;
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
    // Como el original (0x42FC40..0x42FF96). Solo actua si (semilla + tiempo anterior) / 2000 != (semilla + tiempo actual) / 2000 (division por constante magica 0x10624DD3, sin signo) y la mision
    // (int8 +0x3BA; tabla de bytes 0x42FFA0) es una de 2, 4, 8, 15, 17, 43, 52, 54, 61; si no, solo se pone a 0 el contador (+0x3C5). Pide los dos nodos mas cercanos (FindNodesThisCarIsNearestTo, EXE) y no
    // hace nada si no hay primero; sale (contador a 0) si la ruta ya los incluye (6 condiciones sobre actual/siguiente/final); incrementa el contador (byte; <= 4 sale sin ponerlo a 0); busca el
    // destino (2, 4, 43: FindPlayerCoors(-1); 8: +0x3EC; 15, 17, 52, 54, 61: posicion del objetivo +0x41C) y, si DoPathSearch (solo distancia, nodo de origen = segundo) da < 90000.0f y mas de 1 nodo,
    // actual = primero, siguiente = segundo, FindLinksToGoWithTheseNodes, un DoPathSearch completo (+0x3F8, +0x418) y RemoveOnePathNode.
    auto& autoPilot = vehicle->m_autoPilot;
    {
        const uint32 seed = vehicle->m_nRandomSeed;
        const uint32 previous = seed + CTimer::m_snPreviousTimeInMilliseconds;
        const uint32 current = seed + CTimer::m_snTimeInMilliseconds;
        const auto slot = [](uint32 value) { return static_cast<uint32>((static_cast<uint64>(0x10624DD3u) * value) >> 32) >> 7; };
        if (slot(previous) == slot(current)) {
            return;
        }
    }
    const int32 mission = static_cast<int8>(autoPilot.m_nCarMission);
    int32 kind = -1; // 0: jugador, 1: destino, 2: objetivo
    switch (mission) {
    case 2: case 4: case 43: kind = 0; break;
    case 8: kind = 1; break;
    case 15: case 17: case 52: case 54: case 61: kind = 2; break;
    default: break;
    }
    if (kind < 0) {
        autoPilot.m_ucCarMissionModeCounter = 0;
        return;
    }
    CNodeAddress first, second;
    first.m_wAreaId = 0xFFFF;
    second.m_wAreaId = 0xFFFF;
    FindNodesThisCarIsNearestTo(vehicle, first, second);
    if (first.m_wAreaId == 0xFFFF) {
        return;
    }
    const auto clear = [&] { autoPilot.m_ucCarMissionModeCounter = 0; };
    const CNodeAddress current = autoPilot.m_currentAddress;
    if (current == first && autoPilot.m_startingRouteNode == second) { return clear(); }
    if (current == second && autoPilot.m_startingRouteNode == first) { return clear(); }
    if (autoPilot.m_endingRouteNode == first && current == second) { return clear(); }
    if (autoPilot.m_endingRouteNode == second && current == first) { return clear(); }
    if (current == second) { return clear(); }
    if (autoPilot.m_endingRouteNode == second) { return clear(); }
    const uint8 counter = ++autoPilot.m_ucCarMissionModeCounter;
    if (counter <= 4) {
        return;
    }
    CVector target;
    switch (kind) {
    case 0: { const CVector player = FindPlayerCoors(-1); std::memcpy(&target, &player, sizeof(CVector)); break; }
    case 1: std::memcpy(&target, &autoPilot.m_vecDestinationCoors, sizeof(CVector)); break;
    default: {
        const CEntity* const entity = autoPilot.m_TargetEntity;
        const CVector& position = entity->GetPosition();
        std::memcpy(&target, &position, sizeof(CVector));
        break;
    }
    }
    const bool isVortex = static_cast<int16>(vehicle->m_nModelIndex) == 0x21B;
    int16 count;
    float distance;
    ThePaths.DoPathSearch(static_cast<ePathType>(0), vehicle->GetPosition(), second, target, nullptr, count, 0, &distance, std::bit_cast<float>(0x497423FEu), nullptr,
                          std::bit_cast<float>(0x497423FEu), autoPilot.carCtrlFlags.bCantGoAgainstTraffic, autoPilot.m_currentAddress, isVortex, false);
    if (!(distance < 90000.0f) || count < 2) {
        return clear();
    }
    autoPilot.m_currentAddress = first;
    autoPilot.m_startingRouteNode = second;
    FindLinksToGoWithTheseNodes(vehicle);
    ThePaths.DoPathSearch(static_cast<ePathType>(0), vehicle->GetPosition(), second, target, autoPilot.m_aPathFindNodesInfo.data(), reinterpret_cast<int16&>(autoPilot.m_nPathFindNodesCount), 8, nullptr,
                          std::bit_cast<float>(0x497423FEu), nullptr, std::bit_cast<float>(0x497423FEu), autoPilot.carCtrlFlags.bCantGoAgainstTraffic, autoPilot.m_currentAddress, isVortex, false);
    autoPilot.RemoveOnePathNode();
    clear();
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
    // Como el original (0x42CE40..0x42D0CD): radio 44.0f (0x858FF8) si es el vehiculo del jugador (FindPlayerVehicle(-1, 0)) y 11.0f (0x858FBC) si no; se guarda el bit
    // bHonkAtPed (+0x3DB bit 1) y se restaura al final. Misma caja, mismos sectores y mismo orden que FindMaximumSpeedForThisCarInTraffic, pero solo con la lista de peatones
    // (repeatSector + 4) y velocidad inicial = crucero (byte +0x3D0) como float.
    auto& autoPilot = vehicle->m_autoPilot;
    const float range = vehicle == FindPlayerVehicle(-1, false) ? 44.0f : 11.0f;
    const bool honk = autoPilot.carCtrlFlags.bHonkAtPed;
    const CVector& position = vehicle->GetPosition();
    const float x1 = position.x - range;
    const float x2 = range + position.x;
    const float y1 = position.y - range;
    const float y2 = range + position.y;
    const int32 xMinRaw = Cc63bSectorCpp(x1);
    const int32 xMin = xMinRaw > 0 ? xMinRaw : 0;
    const int32 yMinRaw = Cc63bSectorCpp(y1);
    const int32 yMin = yMinRaw > 0 ? yMinRaw : 0;
    const int32 xMaxRaw = Cc63bSectorCpp(x2);
    const int32 xMax = xMaxRaw < 0x77 ? xMaxRaw : 0x77;
    const int32 yMaxRaw = Cc63bSectorCpp(y2);
    const int32 yMax = yMaxRaw < 0x77 ? yMaxRaw : 0x77;
    CWorld::AdvanceCurrentScanCode();
    float speed = static_cast<float>(autoPilot.m_nCruiseSpeed);
    for (int32 y = yMin; y <= yMax; y++) {
        for (int32 x = xMin; x <= xMax; x++) {
            CRepeatSector& sector = CWorld::GetRepeatSector(x, y);
            const float arg8 = static_cast<float>(autoPilot.m_nCruiseSpeed);
            SlowCarDownForPedsSectorList(sector.Peds, vehicle, x1, y1, x2, y2, &speed, arg8);
        }
    }
    vehicle->vehicleFlags.bWarnedPeds = true;
    autoPilot.carCtrlFlags.bHonkAtPed = honk;
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
    // Como el original (0x4342A0..0x4343FC). El manejador del vehiculo (CPools::GetVehicleRef inlinado en el EXE) se calcula sin comprobar que el puntero pertenezca al pool:
    // indice = (vehiculo - almacenamiento) / 0xA18 con signo y manejador = (indice << 8) + byte de estado. z <= -100.0f (0x859014; con NaN no) se sustituye por la altura del suelo en
    // (x, y); arg6 != 0 suma la distancia del centro de masas a la base del modelo. Teleport virtual (ranura 14) con arg5 como `clearOrientation`; el barco (+0x590 == 5) solo despeja
    // el espacio y pone la accion temporal (+0x3BB) a 0; los demas colocan sobre la carretera (automovil y remolque: 0 y 11; moto: 9), despejan, se unen a la carretera segun la mision.
    const uint8* const pool = reinterpret_cast<const uint8*>(GetVehiclePool());
    const intptr_t storage = *reinterpret_cast<const intptr_t*>(pool);
    const uint8* const slotStates = *reinterpret_cast<const uint8* const*>(pool + 4);
    const int32 index = static_cast<int32>(reinterpret_cast<intptr_t>(vehicle) - storage) / 0xA18;
    const int32 handle = slotStates[index] + (index << 8);
    if (z <= -100.0f) {
        z = CWorld::FindGroundZForCoord(x, y);
    }
    if (arg6) {
        z = Cc63bAddFloatCpp(vehicle->GetDistanceFromCentreOfMassToBaseOfModel(), z);
    }
    vehicle->SetIsStatic(false);
    CTheScripts::StuckCars.ClearStuckFlagForCar(handle);
    CVector position{ x, y, z };
    vehicle->Teleport(position, std::bit_cast<bool>(arg5)); // el EXE empuja el byte tal cual (no lo normaliza a 0/1)
    if (vehicle->m_nVehicleType == VEHICLE_TYPE_BOAT) {
        CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
        vehicle->m_autoPilot.m_nTempAction = TEMPACT_NONE;
        return;
    }
    const int32 type = vehicle->m_nVehicleType;
    if (type == VEHICLE_TYPE_AUTOMOBILE || type == VEHICLE_TYPE_TRAILER) {
        static_cast<CAutomobile*>(vehicle)->PlaceOnRoadProperly();
    } else if (type == VEHICLE_TYPE_BIKE) {
        static_cast<CBike*>(vehicle)->PlaceOnRoadProperly();
    }
    CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
    JoinCarWithRoadAccordingToMission(vehicle);
    vehicle->m_autoPilot.m_nTempAction = TEMPACT_NONE;
}

// 0x4217C0
void CCarCtrl::SetUpDriverAndPassengersForVehicle(CVehicle* vehicle, int32 arg2, int32 arg3, bool arg4, bool arg5, int32 passengersNum) {
    // Como el original (0x4217C0..0x4218F2). arg2 = tipo de peaton (14..23 = bandas), arg3 = minimo de pasajeros. rand es el del EXE (0x821B1E, no la de la DLL).
    // El conductor (+0x460, sin comprobar nulo) y cada pasajero de banda sostienen un objeto (word 0x8CD72C) si rand() < 0x3FFF. Cada pasajero extra hasta el maximo
    // (byte +0x488) se anade con probabilidad 1/8 (rand * 0x858C7C < 0.125f); en los coches con la animacion de furgoneta ("van", 0x858EE0) como mucho 1.
    const auto exeRand = reinterpret_cast<int(__cdecl*)()>(0x821B1E);
    const uint16 heldModel = StaticRef<uint16>(0x8CD72C);
    vehicle->SetUpDriver(arg2, arg4, arg5);
    if (arg2 >= 14 && arg2 <= 23 && exeRand() < 0x3FFF) {
        vehicle->m_pDriver->GiveObjectToPedToHold(heldModel, 1);
    }
    int32 limit = passengersNum;
    if (static_cast<int32>(vehicle->m_nMaxPassengers) < limit) {
        limit = vehicle->m_nMaxPassengers;
    }
    int32 count = arg3;
    if (count < limit) {
        for (int32 remaining = limit - count; remaining != 0; --remaining) {
            count += Cc63bRandBelowEighthCpp(exeRand()) ? 1 : 0;
        }
        if (count >= limit) {
            count = limit;
        }
    } else {
        count = limit;
    }
    const int32 modelIndex = static_cast<int16>(vehicle->m_nModelIndex);
    if (CModelInfo::IsCarModel(modelIndex)) {
        const int32 vanBlockIndex = CAnimManager::GetAnimationBlockIndex("van");
        if (CModelInfo::ms_modelInfoPtrs[modelIndex]->GetAnimFileIndex() == vanBlockIndex && count >= 1) {
            count = 1;
        }
    }
    for (int32 seat = 0; seat < count; seat++) {
        CPed* const passenger = vehicle->SetupPassenger(seat, arg2, arg4, arg5);
        if (!passenger) {
            continue;
        }
        passenger->UpdateStatEnteringVehicle();
        if (arg2 >= 14 && arg2 <= 23 && exeRand() < 0x3FFF) {
            passenger->GiveObjectToPedToHold(heldModel, 1);
        }
    }
}

namespace {
// Ayudantes de Weave*, SlowCarDown* y TestCollisionBetween2MovingRects* (lote 63, parte B2)
constexpr float kWvPi             = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
constexpr float kWvTwoPi          = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
constexpr float kWvMargin         = std::bit_cast<float>(0x3E4CCCCDu); // 0x858CC4 (0.2f)
constexpr float kWvStep           = std::bit_cast<float>(0x3DD67750u); // 0x858FAC (6 grados)
constexpr float kWvSlowFactor     = std::bit_cast<float>(0x3F44EC4Fu); // 0x858FFC
constexpr float kWvCarWidthScale  = std::bit_cast<float>(0x4019999Au); // 0x858FB0 (2.4f)
constexpr float kWvCarWidthBase   = std::bit_cast<float>(0x3E99999Au); // 0x858C24 (0.3f)

// El EXE lee estos campos de entidades de cualquier tipo sin comprobar el tipo (p. ej. +0x3BA, +0x41C, +0x42B, +0x46C, +0x58C)
template<typename T>
T& WvRaw(const void* base, uint32 offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset);
}

// Colaboradores matematicos del EXE sin reconstruir: CMatrix * CVector (0x59C890) y Multiply3x3 (0x59C790)
CVector WvMatrixTimesPoint(const CMatrix* matrix, const CVector& point) {
    CVector result;
    plugin::CallAndReturn<CVector*, 0x59C890, CVector*, const CMatrix*, const CVector*>(&result, matrix, &point);
    return result;
}

CVector WvMatrixTimesVector(const CMatrix* matrix, const CVector& vector) {
    CVector result;
    plugin::CallAndReturn<CVector*, 0x59C790, CVector*, const CMatrix*, const CVector*>(&result, matrix, &vector);
    return result;
}
} // namespace

// 0x432420
template<typename PtrListType>
void CCarCtrl::SlowCarDownForCarsSectorList(PtrListType& ptrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8) {
    // Como el original: aritmetica de float (x87 a 24 bits), nodos leidos antes de cada llamada, matriz del vehiculo sin comprobar nulo.
    // arg3/arg5 = x minimo/maximo y arg4/arg6 = y minimo/maximo del rectangulo.
    for (auto* node = ptrList.GetNode(); node;) {
        CEntity* const entity = static_cast<CEntity*>(node->Item);
        node = node->Next;

        if (entity == vehicle || entity->IsScanCodeCurrent() || !entity->GetUsesCollision()) {
            continue;
        }
        entity->SetScanCode(CWorld::ms_nCurrentScanCode);

        const CVector centre = entity->GetBoundCentre(); // 0x534250
        if (!(centre.x > arg3) || !(centre.x < arg5) || !(centre.y > arg4) || !(centre.y < arg6)) {
            continue;
        }
        const CVector& vehiclePosition = vehicle->GetPosition();
        if (!(std::fabs(centre.z - vehiclePosition.z) < 10.0f)) { // 0x85862C
            continue;
        }
        const CMatrix& matrix = *vehicle->m_matrix;
        const float distance = CCollision::DistAlongLine2D(vehiclePosition.x, vehiclePosition.y, matrix.GetForward().x, matrix.GetForward().y, centre.x, centre.y); // 0x412A80
        if (!(std::fabs(centre.z - (distance * matrix.GetForward().z + vehicle->GetPosition().z)) < 3.0f)) { // 0x41CCB0: forward de la matriz
            continue;
        }
        SlowCarDownForOtherCar(entity, vehicle, arg7, arg8); // 0x42D0E0
    }
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
    // Como el original: igual que la de coches, pero solo para tres modelos de barrera y el codigo de exploracion se marca antes de mirar el modelo
    for (auto* node = ptrList.GetNode(); node;) {
        CEntity* const entity = static_cast<CEntity*>(node->Item);
        node = node->Next;

        if (entity->IsScanCodeCurrent()) {
            continue;
        }
        entity->SetScanCode(CWorld::ms_nCurrentScanCode);

        const int32 modelIndex = static_cast<int16>(entity->m_nModelIndex);
        if (modelIndex != int32(uint16(eModelID(ModelIndices::MI_ROADWORKBARRIER1)))      // 0x8CD57C
            && modelIndex != int32(uint16(eModelID(ModelIndices::MI_ROADBLOCKFUCKEDCAR1))) // 0x8CD580
            && modelIndex != int32(uint16(eModelID(ModelIndices::MI_ROADBLOCKFUCKEDCAR2)))) { // 0x8CD584
            continue;
        }

        const CVector centre = entity->GetBoundCentre(); // 0x534250
        if (!(centre.x > arg3) || !(centre.x < arg5) || !(centre.y > arg4) || !(centre.y < arg6)) {
            continue;
        }
        const CVector& vehiclePosition = vehicle->GetPosition();
        if (!(std::fabs(centre.z - vehiclePosition.z) < 10.0f)) { // 0x85862C
            continue;
        }
        const CMatrix& matrix = *vehicle->m_matrix;
        const float distance = CCollision::DistAlongLine2D(vehiclePosition.x, vehiclePosition.y, matrix.GetForward().x, matrix.GetForward().y, centre.x, centre.y); // 0x412A80
        if (!(std::fabs(centre.z - (distance * matrix.GetForward().z + vehicle->GetPosition().z)) < 3.0f)) { // 0x41CCB0: forward de la matriz
            continue;
        }
        SlowCarDownForObject(entity, vehicle, arg7, arg8); // 0x426220
    }
}

// 0x42D0E0
void CCarCtrl::SlowCarDownForOtherCar(CEntity* car1, CVehicle* car2, float* arg3, float arg4) {
    // Como el original: aritmetica de float (x87 a 24 bits); car2->m_matrix se usa sin comprobar nulo, car1 se trata como fisico/vehiculo sin mirar su tipo
    // salvo donde el EXE lo comprueba.
    const CMatrix& matrix2 = *car2->m_matrix;
    float dirX2;
    float dirY2 = matrix2.GetForward().y;
    {
        const float forwardX = matrix2.GetForward().x;
        const float length   = std::sqrt(dirY2 * dirY2 + forwardX * forwardX);
        if (length == 0.0f) {
            dirX2 = 1.0f;
        } else {
            const float inverse = 1.0f / length;
            dirX2 = inverse * forwardX;
            dirY2 = inverse * dirY2;
        }
    }

    const CVector& position1 = car1->GetPosition();
    const CVector& position2 = car2->GetPosition();
    const float side = (position1.y - position2.y) * dirY2 + (position1.x - position2.x) * dirX2;
    if (side < 0.0f) {
        return;
    }

    const CPhysical* const physical1 = car1->AsPhysical();
    const float speedX    = physical1->m_vecMoveSpeed.x * 60.0f;
    const float speedY    = physical1->m_vecMoveSpeed.y * 60.0f;
    const float scaledDirX = dirX2 * arg4;
    const float scaledDirY = dirY2 * arg4;

    // Como el original: la matriz de car1 se comprueba (y se crea) dos veces seguidas
    if (!car1->m_matrix) {
        car1->AllocateMatrix();                         // 0x54F560
        car1->m_placement.UpdateMatrix(car1->m_matrix); // 0x54F1B0
    }
    if (!car1->m_matrix) {
        car1->AllocateMatrix();
        car1->m_placement.UpdateMatrix(car1->m_matrix);
    }
    float dirY1 = car1->m_matrix->GetForward().y;
    float dirX1;
    {
        const float forwardX = car1->m_matrix->GetForward().x;
        const float length   = std::sqrt(dirY1 * dirY1 + forwardX * forwardX);
        if (length == 0.0f) {
            dirX1 = 1.0f;
            dirY1 = car1->m_matrix->GetForward().y;
        } else {
            const float inverse = 1.0f / length;
            dirX1 = inverse * forwardX;
            dirY1 = inverse * dirY1;
        }
    }
    CVector direction2{ dirX2, dirY2, 0.0f }; // A
    CVector direction1{ dirX1, dirY1, 0.0f }; // B

    const float relativeX = speedX - scaledDirX;
    const float relativeY = speedY - scaledDirY;
    const float forward   = TestCollisionBetween2MovingRects_OnlyFrontBumper(car1->AsVehicle(), car2, relativeX, relativeY, &direction2, &direction1);
    const float backward  = TestCollisionBetween2MovingRects(car2, car1->AsVehicle(), -relativeX, -relativeY, &direction1, &direction2);
    float time = forward < backward ? forward : backward;
    if (!(time >= 0.0f)) {
        return;
    }

    auto& autoPilot2 = car2->m_autoPilot;
    if (time < 1.5f) {
        autoPilot2.m_nCarCtrlFlags |= 1;
        autoPilot2.m_ObstructingEntity = car1;
        car1->RegisterReference(&autoPilot2.m_ObstructingEntity); // 0x571B70
        const float reciprocal = 1.0f / arg4;
        if (time < reciprocal) {
            *arg3 = 0.0f;
        } else if (time < 3.0f * reciprocal) {
            *arg3 = *arg3 < 1.0f ? *arg3 : 1.0f;
        } else {
            const float scaled = (time - kWvMargin) * kWvSlowFactor; // 0x858CC4, 0x858FFC
            time = scaled < 0.0f ? 0.0f : scaled;
            const float limit = time * arg4;
            if (!(limit > *arg3)) {
                *arg3 = limit;
            }
        }
    }

    if (!(time >= 0.0f)) {
        return;
    }
    if (!(time < 0.5f)) {
        return;
    }
    if (car1->GetType() != ENTITY_TYPE_VEHICLE) {
        return;
    }
    const uint32 now = CTimer::GetTimeInMS();
    if (now - autoPilot2.m_nTimeSwitchedToRealPhysics <= 15000u) {
        return;
    }
    if (now - car1->AsVehicle()->m_autoPilot.m_nTimeSwitchedToRealPhysics <= 15000u) {
        return;
    }

    // Como el original: GetMatrix (0x411990) de car1 se llama dos veces y la matriz de car2 se lee dos veces sin comprobar nulo
    const CMatrix* const matrix2First = car2->m_matrix;
    const CMatrix&       matrix1First = car1->GetMatrix();
    const CMatrix* const matrix2Second = car2->m_matrix;
    const CMatrix&       matrix1Second = car1->GetMatrix();
    const float dot = matrix1Second.GetForward().y * matrix2Second->GetForward().y + matrix1First.GetForward().x * matrix2First->GetForward().x;
    if (car1 == FindPlayerVehicle(-1, false)) { // 0x56E0D0
        return;
    }
    if (!(dot < -0.5f)) {
        return;
    }
    if (!(reinterpret_cast<uintptr_t>(car2) < reinterpret_cast<uintptr_t>(car1))) {
        return;
    }
    const float reduced = arg4 * kWvMargin;
    if (!(reduced < *arg3)) {
        *arg3 = reduced;
    }
    if (car2->GetStatus() == STATUS_SIMPLE) {
        SwitchVehicleToRealPhysics(car2);
    }
    autoPilot2.m_nCarDrivingStyle = DRIVING_STYLE_AVOID_CARS;
    autoPilot2.m_nTempActionTime  = CTimer::GetTimeInMS() + 1000;
}

// 0x425440
template<typename PtrListType>
void CCarCtrl::SlowCarDownForPedsSectorList(PtrListType& ptrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8) {
    // Como el original: aritmetica de float (x87 a 24 bits), nodos leidos antes de cada llamada, matriz del vehiculo sin comprobar nulo. arg3/arg5 = x minimo/maximo,
    // arg4/arg6 = y minimo/maximo. El ancho (halfWidth) es una variable local que persiste (y, para tipo 9, se multiplica por 1.6 en cada peaton que llega hasta ahi).
    float halfWidth = CModelInfo::GetModelInfo(static_cast<int16>(vehicle->m_nModelIndex))->GetColModel()->GetBoundingBox().m_vecMax.x;
    const CMatrix& matrix = *vehicle->m_matrix;
    const CVector& moveSpeed = vehicle->m_vecMoveSpeed;
    const float forwardSpeed = (matrix.GetForward().z * moveSpeed.z + matrix.GetForward().y * moveSpeed.y) + matrix.GetForward().x * moveSpeed.x;
    const float lookAhead = forwardSpeed * 200.0f;                 // 0x858A48
    const float lookSide  = std::fabs(forwardSpeed) * 50.0f;       // 0x858B40
    bool special;
    if (vehicle == FindPlayerVehicle(-1, false)) { // 0x56E0D0
        special = true;
    } else {
        const int32 vehicleType = static_cast<int32>(vehicle->m_nVehicleType);
        const uint8 style = static_cast<uint8>(vehicle->m_autoPilot.m_nCarDrivingStyle);
        if (vehicleType == 6 || vehicleType == 4 || vehicleType == 3) {
            special = true;
        } else if (style != 0 && style != 6) {
            special = true;
        } else {
            special = (WvRaw<uint8>(vehicle, 0x36) & 0xF8) == 0x18;
        }
    }
    const float halfLength = CModelInfo::GetModelInfo(static_cast<int16>(vehicle->m_nModelIndex))->GetColModel()->GetBoundingBox().m_vecMax.y;

    for (auto* node = ptrList.GetNode(); node;) {
        CEntity* const entity = static_cast<CEntity*>(node->Item);
        node = node->Next;

        if (entity->IsScanCodeCurrent() || !entity->GetUsesCollision()) {
            continue;
        }
        entity->SetScanCode(CWorld::ms_nCurrentScanCode);

        const CVector position = entity->GetPosition();
        if (!(position.x > arg3) || !(position.x < arg5) || !(position.y > arg4) || !(position.y < arg6)) {
            continue;
        }
        if (!(std::fabs(position.z - vehicle->GetPosition().z) < 6.0f)) { // 0x858B44
            continue;
        }
        const float distance = CCollision::DistAlongLine2D(vehicle->GetPosition().x, vehicle->GetPosition().y, matrix.GetForward().x, matrix.GetForward().y, position.x, position.y); // 0x412A80
        if (!(std::fabs(position.z - (distance * matrix.GetForward().z + vehicle->GetPosition().z)) < 3.0f)) { // 0x41CCB0: forward de la matriz
            continue;
        }
        const CVector& vehiclePosition = vehicle->GetPosition();
        const float relativeZ = position.z - vehiclePosition.z;
        const float relativeY = position.y - vehiclePosition.y;
        const float relativeX = position.x - vehiclePosition.x;
        const float projectedForward = (relativeY * matrix.GetForward().y + relativeZ * matrix.GetForward().z) + relativeX * matrix.GetForward().x;

        const uint8 drivingStyle = static_cast<uint8>(vehicle->m_autoPilot.m_nCarDrivingStyle);
        if (drivingStyle == 0 || drivingStyle == 4 || drivingStyle == 1 || drivingStyle == 6) {
            do {
                if (entity == FindPlayerPed(-1)) { // 0x56E210
                    if (CWorld::Players[CWorld::PlayerInFocus].m_pLastTargetVehicle == vehicle) {
                        break;
                    }
                }
                if (!(projectedForward > halfLength)) {
                    break;
                }
                const float ahead = projectedForward - halfLength;
                if (!(ahead < lookAhead)) {
                    break;
                }
                const float lateral = std::fabs((relativeY * matrix.GetRight().y + relativeZ * matrix.GetRight().z) + relativeX * matrix.GetRight().x);
                if (static_cast<int32>(vehicle->m_nVehicleType) == 9) {
                    halfWidth = halfWidth * std::bit_cast<float>(0x3FCCCCCDu); // 0x858FA8 (1.6f)
                }
                if (!(lateral <= halfWidth + 0.5f)) {
                    break;
                }
                if (!(ahead < 13.0f)) { // 0x858CE4
                    break;
                }
                const float shifted = ahead - 1.0f;
                const float clamped = (0.0f > shifted) ? 0.0f : shifted;
                const float scaled  = (std::bit_cast<float>(0x3D9D89D9u) * clamped) * arg8; // 0x858FA4
                const float wanted  = (1.0f > scaled) ? 1.0f : scaled;
                if (!(wanted > *arg7)) {
                    *arg7 = wanted;
                }
                vehicle->m_autoPilot.m_nCarCtrlFlags |= 2;
                if (ahead < 4.0f) { // 0x858B90
                    vehicle->m_autoPilot.m_nTempAction     = TEMPACT_WAIT;
                    vehicle->m_autoPilot.m_nTempActionTime = CTimer::GetTimeInMS() + 4000;
                }
                if (ahead < 2.5f) { // 0x858FA0
                    vehicle->m_autoPilot.m_nTempAction     = TEMPACT_BRAKE;
                    vehicle->m_autoPilot.m_nTempActionTime = CTimer::GetTimeInMS() + 4000;
                }
            } while (false);
        }

        // 0x425926
        if (entity->GetType() != ENTITY_TYPE_PED) {
            continue;
        }
        CPed* const ped = static_cast<CPed*>(entity);
        if (vehicle == FindPlayerVehicle(-1, false)) {
            if (WvRaw<uint32>(vehicle, 0x514) != 0) {
                const CVector difference = ped->GetPosition() - vehicle->GetPosition(); // 0x40FE60
                if (difference.SquaredMagnitude() < 49.0f) { // 0x406DA0, 0x858D04
                    CEventPotentialGetRunOver event(vehicle); // 0x4AE1A0
                    ped->GetEventGroup().Add(&event, false);  // 0x4AB420; ~CEventPotentialGetRunOver = 0x4AE270
                }
            }
        }
        if (!special) {
            continue;
        }
        if (forwardSpeed == 0.0f) {
            continue;
        }
        if (((projectedForward < 0.0f) ? -1 : 1) != ((forwardSpeed < 0.0f) ? -1 : 1)) {
            continue;
        }
        const float absProjected = std::fabs(projectedForward);
        if (!(absProjected > halfLength)) {
            continue;
        }
        if (!(std::fabs(forwardSpeed) > std::bit_cast<float>(0x3D4CCCCDu))) { // 0x406D10, 0x86C8CC (0.05f)
            continue;
        }
        if (!((absProjected - halfLength) < lookSide)) {
            continue;
        }
        const float lateral2 = std::fabs((relativeY * matrix.GetRight().y + relativeZ * matrix.GetRight().z) + relativeX * matrix.GetRight().x);
        if (!(lateral2 <= halfWidth + std::bit_cast<float>(0x3EB33333u))) { // 0x858F9C (0.35f)
            continue;
        }
        CEventPotentialGetRunOver event(vehicle);
        ped->GetEventGroup().Add(&event, false);
        if (vehicle->m_pDriver && vehicle->m_pDriver->IsPlayer()) { // 0x5DF8F0
            ped->GetIntelligence()->IncrementAngerAtPlayer(2);       // 0x421050
        }
    }
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
    // Como el original: apunta por delante del jugador (su posicion + velocidad x factor de distancia x 60.0; FindPlayerCoors y FindPlayerSpeed se llaman en el orden del EXE);
    // *arg2 = diferencia de rumbo normalizada, *arg3 = acelerador/freno, *arg4 = 0, *arg5 = false; con el modelo 430, a menos de 40.0 del jugador y diferencia < 0.15 llama a 0x73DF00
    const CVector& position = vehicle->GetPosition();
    const CVector coors1 = FindPlayerCoors(-1);
    float distance;
    const float lead = CcAirBoatAttackLead(&coors1.x, &position.x, &distance);
    float forward[2];
    CcAirBoatForward(&vehicle->m_matrix->GetForward().x, forward);
    const CVector& speed1 = FindPlayerSpeed(-1);
    const CVector coors2 = FindPlayerCoors(-1);
    const float leadX = CcAirBoatLeadX(lead, &speed1.x, &coors2.x);
    const CVector& speed2 = FindPlayerSpeed(-1);
    const float leadYPart = CcAirBoatLeadYPart(lead, &speed2.x);
    const CVector coors3 = FindPlayerCoors(-1);
    const float dy = CcAirBoatLeadDy(leadYPart, &coors3.x, &vehicle->GetPosition().x);
    const float dx = CcAirSubF(leadX, vehicle->GetPosition().x);
    const float headingToTarget = CGeneral::GetATanOfXY(dx, dy);
    const float ownHeading = CGeneral::GetATanOfXY(forward[0], forward[1]);
    const double diff = CcAirBoatSteerThrottle(headingToTarget, ownHeading, &vehicle->m_vecMoveSpeed.x, vehicle->m_autoPilot.m_nCruiseSpeed, 1, arg2, arg3);
    *arg4 = 0.0f;
    *arg5 = false;
    if (vehicle->m_nModelIndex == 430) {
        if (distance < 40.0f) {
            if (diff < std::bit_cast<float>(0x3E19999Au)) { // 0.15f (0x858FCC)
                CarCtrlAirExeBoatSpecial(vehicle);
            }
        }
    }
}

// 0x429090
void CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    // Como el original: vector del jugador a la lancha (z = 0) normalizado por 0x59C910 y girado 90 grados con modulo 26.0 (bit 0 de m_nRandomSeed a 0) o -12.0 (a 1), sumado a la posicion del jugador
    // (segunda llamada a FindPlayerCoors); mismo calculo de rumbo y de acelerador que AttackingPlayer; *arg4 = 0, *arg5 = false
    const CVector& position = vehicle->GetPosition();
    const CVector coors1 = FindPlayerCoors(-1);
    CVector toBoat;
    toBoat.y = CcAirSubF(coors1.y, position.y);
    toBoat.x = CcAirSubF(coors1.x, position.x);
    toBoat.z = 0.0f;
    CarCtrlAirExeNormalise(&toBoat);
    float rotated[2];
    CcAirCirclingVec(&toBoat.x, (vehicle->m_nRandomSeed & 1) != 0, rotated);
    const CVector coors2 = FindPlayerCoors(-1);
    float forward[2];
    CcAirBoatForward(&vehicle->m_matrix->GetForward().x, forward);
    float aim[2];
    CcAirCirclingAim(rotated, &coors2.x, &vehicle->GetPosition().x, aim);
    const float headingToTarget = CGeneral::GetATanOfXY(aim[0], aim[1]);
    const float ownHeading = CGeneral::GetATanOfXY(forward[0], forward[1]);
    CcAirBoatSteerThrottle(headingToTarget, ownHeading, &vehicle->m_vecMoveSpeed.x, vehicle->m_autoPilot.m_nCruiseSpeed, 1, arg2, arg3);
    *arg4 = 0.0f;
    *arg5 = false;
}

// 0x428BE0
void CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget(CVehicle* vehicle, float arg2, float arg3, float* arg4, float* arg5, float* arg6) {
    // Como el original: el eje adelante se toma de m_matrix (sin comprobar nulo) y se normaliza en 2D; rumbo hacia (arg2, arg3) menos el rumbo de la lancha, normalizado a [-pi, pi] y limitado
    // a [-0.5, 0.5] en *arg4 (cambiado de signo al frenar); *arg5 = acelerador/freno segun la velocidad frente a +0x3D0 (crucero); *arg6 = 0
    float forward[2];
    CcAirBoatForward(&vehicle->m_matrix->GetForward().x, forward);
    const float dy = CcAirSubF(arg3, vehicle->GetPosition().y);
    const float dx = CcAirSubF(arg2, vehicle->GetPosition().x);
    const float headingToTarget = CGeneral::GetATanOfXY(dx, dy);
    const float ownHeading = CGeneral::GetATanOfXY(forward[0], forward[1]);
    CcAirBoatSteerThrottle(headingToTarget, ownHeading, &vehicle->m_vecMoveSpeed.x, vehicle->m_autoPilot.m_nCruiseSpeed, 0, arg4, arg5);
    *arg6 = 0.0f;
}

// 0x422B20
void CCarCtrl::SteerAICarBlockingPlayerForwardAndBack(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    // arg2 = giro (siempre 0), arg3 = acelerador, arg4 = freno, arg5 = freno de mano. El vehiculo se coloca delante o detras del jugador segun su velocidad lateral.
    *arg2 = 0.0f;
    *arg5 = false;

    // Velocidad del jugador (copia) mas 0.1 x el avance de su entidad (sin comprobar nulo la entidad; la matriz se crea si falta)
    const CVector playerSpeed = FindPlayerSpeed(-1);
    CEntity* const playerEntity = FindPlayerEntity(-1);
    if (!playerEntity->m_matrix) {
        playerEntity->AllocateMatrix();                                     // 0x54F560
        playerEntity->m_placement.UpdateMatrix(playerEntity->m_matrix);     // 0x54F1B0
    }
    const CVector& playerForward = playerEntity->m_matrix->GetForward();
    const float playerForwardX = playerForward.x * 0.1f; // 0x858B1C
    const float playerForwardY = playerForward.y * 0.1f;
    const float playerVelocityX = playerForwardX + playerSpeed.x;
    const float playerVelocityY = playerSpeed.y + playerForwardY;

    // Ejes del vehiculo en el plano (derecha y avance), normalizados con z = 0
    const CMatrix& matrix = *vehicle->m_matrix;
    CVector right{ matrix.GetRight().x, matrix.GetRight().y, 0.0f };
    right.Normalise(); // 0x59C910
    CVector forward{ matrix.GetForward().x, matrix.GetForward().y, 0.0f };
    forward.Normalise();

    // Desplazamiento del jugador respecto al vehiculo, proyectado sobre el eje derecho (A) y velocidad lateral del jugador (C, 0.01 si es 0)
    const CVector& pos = vehicle->GetPosition();
    const CVector playerPos = FindPlayerCoors(-1); // 0x56E010
    const float dx = playerPos.x - pos.x;
    const float dy = playerPos.y - pos.y;
    const float dz = playerPos.z - pos.z;
    const float lateralOffset = (right.y * dy + dx * right.x) + dz * right.z;
    float lateralSpeed = (right.y * playerVelocityY + right.x * playerVelocityX) + right.z * 0.0f;
    if (lateralSpeed == 0.0f) {
        lateralSpeed = 0.01f; // 0x858C58
    }
    const float t = -(lateralOffset / lateralSpeed);
    if (t < 0.0f) {
        *arg3 = 0.0f;
        *arg4 = 0.0f;
        return;
    }

    // Posicion del jugador (segunda llamada) frente al avance del vehiculo
    const CVector& pos2 = vehicle->GetPosition();
    const CVector playerPos2 = FindPlayerCoors(-1);
    const float dx2 = playerPos2.x - pos2.x;
    const float dy2 = playerPos2.y - pos2.y;
    const float dz2 = playerPos2.z - pos2.z;
    const CVector& moveSpeed = vehicle->m_vecMoveSpeed;
    const float vehicleForwardSpeed = (forward.y * moveSpeed.y + forward.z * moveSpeed.z) + forward.x * moveSpeed.x;
    const float playerForwardSpeed = (forward.y * playerVelocityY + forward.x * playerVelocityX) + forward.z * 0.0f;
    const float ahead = (forward.y * dy2 + dx2 * forward.x) + dz2 * forward.z;
    const float gap = (playerForwardSpeed * t + ahead) - vehicleForwardSpeed * t;
    if (gap > 0.0f) {
        const float scaled = gap * 0.1f;
        *arg3 = 1.0f < scaled ? 1.0f : scaled;
        *arg4 = 0.0f;
    } else if (vehicleForwardSpeed > 0.0f) {
        const float scaled = gap * -0.1f; // 0x858EF4
        *arg3 = 0.0f;
        const float brake = 1.0f < scaled ? 1.0f : scaled;
        *arg4 = brake;
        if (brake > 0.95f) { // 0x858EF0
            *arg5 = true;
        }
    } else {
        const float scaled = gap * 0.1f;
        *arg3 = -1.0f > scaled ? -1.0f : scaled; // 0x858C1C
        *arg4 = 0.0f;
    }
}

namespace {
// Como el original (0x433DF5..0x433E49 y 0x4340C7..0x434173): al aparcar, el conductor y cada pasajero reciben una tarea CTaskComplexLeaveAnyCar(0, true, false) en la ranura primaria
// 3 (SetTask 0x681AF0). Se reserva con CTask::operator new (0x61A5A0) y, si el pool esta agotado (nulo), no se construye y se asigna una tarea nula.
CTask* ParkNewLeaveAnyCarTask() {
    void* const mem = CTask::operator new(sizeof(CTaskComplexLeaveAnyCar));
    return mem ? ::new (mem) CTaskComplexLeaveAnyCar(0, true, false) : nullptr;
}

// CTaskManager::SetTask (0x681AF0) esta definido en linea en la cabecera: se llama por la direccion del EXE, como el original
void ParkSetPrimaryTask(CPed* ped, CTask* task) {
    reinterpret_cast<void(__thiscall*)(CTaskManager*, CTask*, ePrimaryTasks, bool)>(0x681AF0)(&ped->GetTaskManager(), task, TASK_PRIMARY_PRIMARY, false);
}

void ParkOccupantsLeave(CVehicle* vehicle) {
    if (vehicle->m_pDriver) {
        CTask* const task = ParkNewLeaveAnyCarTask();
        ParkSetPrimaryTask(vehicle->m_pDriver, task);
    }
    for (auto i = 0; i < 8; i++) { // m_apPassengers (+0x464), leidos con .data(): el EXE recorre las 8 ranuras
        if (CPed* const passenger = vehicle->m_apPassengers.data()[i]) {
            CTask* const task = ParkNewLeaveAnyCarTask();
            ParkSetPrimaryTask(passenger, task);
        }
    }
}

// Nodo de la direccion (sin comprobar el indice; el area ya se comprobo)
CPathNode* ParkNode(CNodeAddress address) {
    return ThePaths.m_pPathNodes[address.m_wAreaId] + address.m_wNodeId;
}
} // namespace

// 0x433BA0
void CCarCtrl::SteerAICarParkParallel(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    // arg2 = giro, arg3 = acelerador, arg4 = freno, arg5 = freno de mano (los rellena HeadingForTarget). m_currentAddress (+0x390) y m_startingRouteNode (+0x394)
    auto& autoPilot = vehicle->m_autoPilot;
    const CNodeAddress startAddress = autoPilot.m_startingRouteNode;
    if (!ThePaths.m_pPathNodes[startAddress.m_wAreaId]) {
        autoPilot.m_nCarMission = static_cast<eCarMission>(0xB);
        *arg4 = 0.0f;
        *arg3 = 0.0f;
        *arg2 = 0.0f;
        return;
    }
    const CNodeAddress currentAddress = autoPilot.m_currentAddress;
    if (!ThePaths.m_pPathNodes[currentAddress.m_wAreaId]) {
        autoPilot.m_nCarMission = static_cast<eCarMission>(0xB);
        *arg4 = 0.0f;
        *arg3 = 0.0f;
        *arg2 = 0.0f;
        return;
    }
    CPathNode* const startNode = ParkNode(startAddress);
    CPathNode* const currentNode = ParkNode(currentAddress);

    // Mision 0x2E: el punto de aparcado esta 1 m por delante del nodo inicial, en la direccion del nodo actual al inicial; si no, es el propio nodo inicial
    CVector target;
    if (autoPilot.m_nCarMission == static_cast<eCarMission>(0x2E)) {
        const CVector currentPos = currentNode->GetPosition();
        const CVector startPos = startNode->GetPosition();
        CVector direction{ startPos.x - currentPos.x, startPos.y - currentPos.y, startPos.z - currentPos.z };
        direction.Normalise(); // 0x59C910
        const CVector startPos2 = startNode->GetPosition();
        target = CVector{ direction.x + startPos2.x, direction.y + startPos2.y, direction.z + startPos2.z };
    } else {
        target = startNode->GetPosition();
    }
    SteerAICarWithPhysicsHeadingForTarget(vehicle, nullptr, target.x, target.y, arg2, arg3, arg4, arg5);
    autoPilot.m_nCruiseSpeed = std::min<uint8>(autoPilot.m_nCruiseSpeed, 8);

    const CVector& pos = vehicle->GetPosition();
    const float dx = target.x - pos.x;
    const float dy = target.y - pos.y;
    const float distance = std::sqrt(dx * dx + dy * dy);
    if (autoPilot.m_nCarMission == static_cast<eCarMission>(0x2E)) {
        if (distance < 4.0f) { // 0x858B90
            autoPilot.m_nCarMission = static_cast<eCarMission>(0x32);
        }
        return;
    }
    if (!(distance < 2.0f)) { // 0x858CA0
        return;
    }
    autoPilot.m_nCarMission = static_cast<eCarMission>(0xB);
    vehicle->vehicleFlags.bEngineOn = false;  // flags (+0x428) &= 0xAF: motor (0x10) y luces (0x40)
    vehicle->vehicleFlags.bLightsOn = false;
    ParkOccupantsLeave(vehicle);
}

// 0x433EA0
void CCarCtrl::SteerAICarParkPerpendicular(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    // arg2 = giro, arg3 = acelerador, arg4 = freno, arg5 = freno de mano (los rellena HeadingForTarget). m_currentAddress (+0x390) y m_startingRouteNode (+0x394)
    auto& autoPilot = vehicle->m_autoPilot;
    const CNodeAddress startAddress = autoPilot.m_startingRouteNode;
    if (!ThePaths.m_pPathNodes[startAddress.m_wAreaId]) {
        autoPilot.m_nCarMission = static_cast<eCarMission>(0xB);
        *arg4 = 0.0f;
        *arg3 = 0.0f;
        *arg2 = 0.0f;
        return;
    }
    const CNodeAddress currentAddress = autoPilot.m_currentAddress;
    if (!ThePaths.m_pPathNodes[currentAddress.m_wAreaId]) {
        autoPilot.m_nCarMission = static_cast<eCarMission>(0xB);
        *arg4 = 0.0f;
        *arg3 = 0.0f;
        *arg2 = 0.0f;
        return;
    }
    CPathNode* const startNode = ParkNode(startAddress);
    CPathNode* const currentNode = ParkNode(currentAddress);

    // Mision 0x2D: si el vehiculo esta a menos de 6 m de la recta nodo inicial - nodo actual pasa a la 0x31; mientras siga en la 0x2D apunta al nodo actual, si no al inicial
    if (autoPilot.m_nCarMission == static_cast<eCarMission>(0x2D)) {
        const CVector startPos = startNode->GetPosition();
        const CVector currentPos = currentNode->GetPosition();
        const CVector lineStart{ startPos.x, startPos.y, 0.0f };
        const CVector lineEnd{ currentPos.x, currentPos.y, 0.0f };
        const CVector& pos = vehicle->GetPosition();
        const CVector point{ pos.x, pos.y, 0.0f };
        if (CCollision::DistToMathematicalLine(&lineStart, &lineEnd, &point) < 6.0f) { // 0x412970, 0x858B44
            autoPilot.m_nCarMission = static_cast<eCarMission>(0x31);
        }
    }
    const CVector target = (autoPilot.m_nCarMission == static_cast<eCarMission>(0x2D)) ? currentNode->GetPosition() : startNode->GetPosition();
    SteerAICarWithPhysicsHeadingForTarget(vehicle, nullptr, target.x, target.y, arg2, arg3, arg4, arg5);
    autoPilot.m_nCruiseSpeed = std::min<uint8>(autoPilot.m_nCruiseSpeed, 8);

    // Distancia al nodo inicial (no al punto de mira): a menos de 2 m aparca
    const CVector& pos = vehicle->GetPosition();
    const CVector startPos = startNode->GetPosition();
    const float dx = startPos.x - pos.x;
    const float dy = startPos.y - pos.y;
    if (!(std::sqrt(dx * dx + dy * dy) < 2.0f)) { // 0x858CA0
        return;
    }
    autoPilot.m_nMovementFlags |= 2;
    autoPilot.m_nCarMission = static_cast<eCarMission>(0xB);
    vehicle->vehicleFlags.bEngineOn = false;  // flags (+0x428) &= 0xAF: motor (0x10) y luces (0x40)
    vehicle->vehicleFlags.bLightsOn = false;
    ParkOccupantsLeave(vehicle);
}

// 0x4336D0
void CCarCtrl::SteerAICarTowardsPointInEscort(CVehicle* vehicle1, CVehicle* vehicle2, float arg3, float arg4, float* arg5, float* arg6, float* arg7, bool* arg8) {
    // vehicle1 conduce hacia un punto (arg3, arg4) en el sistema del vehiculo escoltado (vehicle2). arg5 = giro, arg6 = acelerador, arg7 = freno, arg8 = freno de mano.
    constexpr float kPi = std::bit_cast<float>(0x40490FDBu);    // 0x858CB8
    constexpr float kTwoPi = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC

    // Punto del escoltado en el mundo (0x59C890, matriz x vector con z = 0) mas su velocidad: matriz sin comprobar nulo, como el EXE
    const float local[3]{ arg3, arg4, 0.0f };
    float world[3];
    AutoPreMultiplyMatrixWithVectorCpp(&vehicle2->m_matrix->GetRight().x, local, world);
    const CVector& escortSpeed = vehicle2->m_vecMoveSpeed;
    const float targetX = escortSpeed.x + world[0];
    const float targetY = escortSpeed.y + world[1];
    *arg8 = false;

    // Direccion de avance del vehiculo 1 normalizada (longitud 0: (1, y))
    const CVector& forward = vehicle1->m_matrix->GetForward();
    float forwardX = forward.x;
    float forwardY = forward.y;
    const float forwardLength = std::sqrt(forwardY * forwardY + forwardX * forwardX);
    if (forwardLength == 0.0f) {
        forwardX = 1.0f;
    } else {
        const float inverse = 1.0f / forwardLength;
        forwardX = inverse * forwardX;
        forwardY = inverse * forwardY;
    }

    // Punto de mira 3 m por delante en la direccion "top" del escoltado (0x858B3C = 3.0)
    const CVector& escortForward = vehicle2->m_matrix->GetForward();
    const float aimX = escortForward.x * 3.0f + targetX;
    const float aimY = escortForward.y * 3.0f + targetY;
    const CVector& pos = vehicle1->GetPosition();
    const float headingToAim = CGeneral::GetATanOfXY(aimX - pos.x, aimY - pos.y);
    const float vehicleHeading = CGeneral::GetATanOfXY(forwardX, forwardY);
    float diff = FindAngleToWeaveThroughTraffic(vehicle1, nullptr, headingToAim, vehicleHeading, 1.0f) - vehicleHeading;
    while (diff < -kPi) {
        diff += kTwoPi;
    }
    while (diff > kPi) {
        diff -= kTwoPi;
    }
    const float maxSteer = FindMaxSteerAngle(vehicle1);
    if (diff < -maxSteer) {
        diff = -maxSteer;
    }
    if (diff > maxSteer) {
        diff = maxSteer;
    }

    // El EXE calcula tambien el rumbo al punto sin proyeccion y descarta el resultado (la llamada se conserva)
    const CVector& pos2 = vehicle1->GetPosition();
    CGeneral::GetATanOfXY(targetX - pos2.x, targetY - pos2.y);

    // Proyeccion del vector al punto sobre el avance y distancia
    const CVector& pos3 = vehicle1->GetPosition();
    const float dot = (targetY - pos3.y) * forwardY + (targetX - pos3.x) * forwardX;
    const float distance = std::sqrt((targetY - pos3.y) * (targetY - pos3.y) + (targetX - pos3.x) * (targetX - pos3.x));

    // Velocidad deseada
    float desired;
    if (dot > 0.5f) { // 0x858B8C
        const float escortSpeed60 = std::sqrt((escortSpeed.x * escortSpeed.x + escortSpeed.y * escortSpeed.y) + escortSpeed.z * escortSpeed.z) * GAME_SPEED_TO_CAR_AI_SPEED;
        if (distance < 15.0f) { // 0x858B48
            const float ahead = (dot - 0.5f) - 0.1f; // 0x858B1C
            const float margin = 4.0f < ahead ? 4.0f : ahead; // 0x858B90
            const float scaledDistance = distance * 3.5f; // 0x859028
            const float sum = escortSpeed60 + margin;
            desired = sum > scaledDistance ? sum : scaledDistance;
        } else {
            desired = 300.0f; // 0x858FD8
        }
        const float limit = escortSpeed60 + 10.0f; // 0x85862C
        if (!(desired < limit)) {
            desired = limit;
        }
    } else {
        if (distance < 15.0f) {
            // Detras y cerca: sin giro ni acelerador; freno fuerte si esta muy detras (< -3, 0x859024)
            *arg5 = 0.0f;
            *arg6 = 0.0f;
            *arg7 = dot < -3.0f ? 1.0f : 0.1f;
            return;
        }
        desired = 8.0f; // 0x859000
    }

    // Aceleracion / freno: (deseada - |v| * 60)
    const CVector& moveSpeed = vehicle1->m_vecMoveSpeed;
    *arg7 = 0.0f;
    const float currentSpeed = std::sqrt((moveSpeed.z * moveSpeed.z + moveSpeed.y * moveSpeed.y) + moveSpeed.x * moveSpeed.x) * GAME_SPEED_TO_CAR_AI_SPEED;
    const float error = desired - currentSpeed;
    if (error > 0.0f) {
        if (currentSpeed < 25.0f) { // 0x858FE8
            const float scaled = error * 0.1f;
            *arg6 = 1.0f < scaled ? 1.0f : scaled;
        } else {
            *arg6 = 1.0f;
        }
    } else {
        const float scaled = error * -0.05f; // 0x85901C
        *arg6 = 0.0f;
        *arg7 = 0.5f < scaled ? 0.5f : scaled;
    }
    *arg5 = diff;
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
    // arg2 = giro, arg3 = acelerador, arg4 = freno, arg5 = freno de mano. Constantes del EXE: pi (0x858CB8), 2*pi (0x858CBC)
    constexpr float kPi = std::bit_cast<float>(0x40490FDBu);
    constexpr float kTwoPi = std::bit_cast<float>(0x40C90FDBu);
    constexpr float kPackedVelocity = std::bit_cast<float>(0x38800100u); // 0x858EAC: 1/16383.5
    // Mision 0xB: reproduccion terminada (no hay una constante en eCarMission usada por el original en este punto)
    const auto stop = [&] {
        *arg4 = 0.0f;
        *arg3 = 0.0f;
        *arg2 = 0.0f;
        *arg5 = false;
        vehicle->m_autoPilot.m_nCarMission = static_cast<eCarMission>(0xB);
    };

    const int32 id = vehicle->m_autoPilot.m_vehicleRecordingId; // int8: negativo = sin reproduccion
    if (id < 0) {
        stop();
        return;
    }
    // Los arrays del EXE (16 entradas) se leen sin comprobar el indice, como el original
    const auto at = [id](auto& arr) -> auto& { return arr.data()[id]; };
    uint8* const base = reinterpret_cast<uint8*>(at(CVehicleRecording::pPlaybackBuffer));
    if (!base) {
        stop();
        return;
    }
    if (at(CVehicleRecording::bPlaybackPaused)) {
        *arg3 = 0.0f;
        *arg2 = 0.0f;
        *arg4 = 0.5f;
        *arg5 = false;
        return;
    }

    // El registro actual (rec) se fija ahora y NO se actualiza al avanzar: el giro y la velocidad usan siempre ese primer registro (el original conserva el puntero en EBX)
    const int32 startOffset = at(CVehicleRecording::PlaybackIndex);
    const uint8* const rec = base + startOffset;
    const auto distanceTo = [&](const uint8* frame, uint32 at) {
        const CVector& pos = vehicle->GetPosition();
        const float x = *reinterpret_cast<const float*>(frame + at);
        const float y = *reinterpret_cast<const float*>(frame + at + 4);
        const float z = *reinterpret_cast<const float*>(frame + at + 8);
        const float dx = pos.x - x;
        const float dy = pos.y - y;
        const float dz = pos.z - z;
        return std::sqrt((dz * dz + dy * dy) + dx * dx);
    };
    float nearDistance = distanceTo(rec, 0x14);
    float nextDistance = distanceTo(rec, 0x34);
    // Avanza mientras el vehiculo este a menos de 10 m del registro o mas cerca del siguiente
    while (nearDistance < 10.0f || nextDistance < nearDistance) { // 0x85862C
        int32& offset = at(CVehicleRecording::PlaybackIndex);
        const uint32 next = static_cast<uint32>(offset) + 0x20u;
        const uint32 limit = static_cast<uint32>(at(CVehicleRecording::PlaybackBufferSize)) - 0x20u;
        offset = static_cast<int32>(next);
        if (next >= limit) {
            CVehicleRecording::StopPlaybackWithIndex(id);
            vehicle->m_autoPilot.m_vehicleRecordingId = -1;
            stop();
            return;
        }
        nearDistance = distanceTo(base + next, 0x14);
        nextDistance = distanceTo(base + next, 0x34);
    }

    // Giro: rumbo del vehiculo (top de la matriz, sin comprobar nulo) y rumbo al registro, corregido con 0x4325C0
    const CVector& forward = vehicle->m_matrix->GetForward();
    const float vehicleHeading = CGeneral::GetATanOfXY(forward.x, forward.y);
    const CVector& pos = vehicle->GetPosition();
    const float recX = *reinterpret_cast<const float*>(rec + 0x14);
    const float recY = *reinterpret_cast<const float*>(rec + 0x18);
    const float headingToRecord = CGeneral::GetATanOfXY(recX - pos.x, recY - pos.y);
    float diff = FindAngleToWeaveThroughTraffic(vehicle, nullptr, headingToRecord, vehicleHeading, 2.0f) - vehicleHeading;
    while (diff < -kPi) {
        diff += kTwoPi;
    }
    while (diff > kPi) {
        diff -= kTwoPi;
    }
    const float maxSteer = FindMaxSteerAngle(vehicle);
    if (diff < -maxSteer) {
        diff = -maxSteer;
    }
    if (diff > maxSteer) {
        diff = maxSteer;
    }

    // Velocidad objetivo: |velocidad del registro| * multiplicador de reproduccion * 60, con suelo de 5 en los primeros 0x320 bytes
    const float recordVelocityX = static_cast<float>(*reinterpret_cast<const int16*>(rec + 6)) * kPackedVelocity;
    const float recordVelocityY = static_cast<float>(*reinterpret_cast<const int16*>(rec + 4)) * kPackedVelocity;
    float target = std::sqrt(recordVelocityY * recordVelocityY + recordVelocityX * recordVelocityX) * at(CVehicleRecording::PlaybackSpeed) * GAME_SPEED_TO_CAR_AI_SPEED;
    if (!(static_cast<uint32>(at(CVehicleRecording::PlaybackIndex)) > 0x320u)) {
        if (!(target > 5.0f)) { // 0x858C80
            target = 5.0f;
        }
    }
    *arg4 = 0.0f;
    const CVector& moveSpeed = vehicle->m_vecMoveSpeed;
    const float currentSpeed = std::sqrt(moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y) * GAME_SPEED_TO_CAR_AI_SPEED;
    const float error = target - currentSpeed;
    if (error > 0.0f) {
        const float scaled = error * (currentSpeed < 2.0f ? 0.25f : 0.125f); // 0x858CA0, 0x858C84, 0x858C48
        *arg3 = 1.0f < scaled ? 1.0f : scaled;
    } else {
        const float scaled = error * -0.05f; // 0x85901C
        *arg3 = 0.0f;
        *arg4 = 0.5f < scaled ? 0.5f : scaled; // 0x858B8C
    }
    *arg2 = diff;
    *arg5 = false;
}

// 0x433280
void CCarCtrl::SteerAICarWithPhysicsHeadingForTarget(CVehicle* vehicle, CPhysical* target, float x, float y, float* steer, float* accel, float* brake, bool* handbrake) {
    // Como el original: tres constantes del EXE (-pi, pi y 2*pi: 0x858CC0, 0x858CB8, 0x858CBC)
    constexpr float kPi = std::bit_cast<float>(0x40490FDBu);
    constexpr float kTwoPi = std::bit_cast<float>(0x40C90FDBu);

    *handbrake = false;

    // Direccion de avance (columna "top" de la matriz, sin comprobar nulo como el EXE), normalizada: con longitud 0 queda (1, y)
    const CVector& forward = vehicle->m_matrix->GetForward();
    float forwardX = forward.x;
    float forwardY = forward.y;
    const float forwardLength = std::sqrt(forwardY * forwardY + forwardX * forwardX);
    if (forwardLength == 0.0f) {
        forwardX = 1.0f;
    } else {
        const float inverse = 1.0f / forwardLength;
        forwardX = inverse * forwardX;
        forwardY = inverse * forwardY;
    }

    // Rumbo hacia el objetivo y rumbo del vehiculo; dos llamadas a 0x53CC70 en este orden
    const CVector& pos = vehicle->GetPosition();
    const float headingToTarget = CGeneral::GetATanOfXY(x - pos.x, y - pos.y);
    const float vehicleHeading = CGeneral::GetATanOfXY(forwardX, forwardY);

    // Con los estilos 2, 5 y 6 el rumbo se corrige con FindAngleToWeaveThroughTraffic (0x4325C0)
    const auto style = vehicle->m_autoPilot.m_nCarDrivingStyle;
    const bool weave = style == 2 || style == 5 || style == 6;
    float diff = (weave ? FindAngleToWeaveThroughTraffic(vehicle, target, headingToTarget, vehicleHeading, 1.0f) : headingToTarget) - vehicleHeading;

    // Normaliza a [-pi, pi]
    while (diff < -kPi) {
        diff += kTwoPi;
    }
    while (diff > kPi) {
        diff -= kTwoPi;
    }

    // Giro brusco a velocidad: freno de mano
    const CVector& moveSpeed = vehicle->m_vecMoveSpeed;
    const float speed = std::sqrt((moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y) + moveSpeed.z * moveSpeed.z);
    if (speed > 0.3f) { // 0x858C24
        if ((diff < 0.0f ? -diff : diff) > 0.7f) { // 0x858CB0
            *handbrake = true;
        }
    }

    // Limita el giro a +-FindMaxSteerAngle (0x427FE0)
    const float maxSteer = FindMaxSteerAngle(vehicle);
    if (diff < -maxSteer) {
        diff = -maxSteer;
    }
    if (diff > maxSteer) {
        diff = maxSteer;
    }

    // Segunda llamada a 0x53CC70 con la resta hecha por el x87 sin redondear antes (X87Math.cpp), y 0x4224E0 con (0.4, 1.2, 0.4)
    const CVector& pos2 = vehicle->GetPosition();
    const float angleError = CarCtrlSteer63AtanMinusCpp(x - pos2.x, y - pos2.y, vehicleHeading);
    const float multiplier = FindSpeedMultiplier(angleError, 0.4f, 1.2f, 0.4f);

    // Aceleracion/freno: (multiplicador * velocidad de crucero) - (|v| * 60), con |v| calculada en el orden z, y, x
    const float speedForAi = std::sqrt((moveSpeed.z * moveSpeed.z + moveSpeed.y * moveSpeed.y) + moveSpeed.x * moveSpeed.x) * GAME_SPEED_TO_CAR_AI_SPEED;
    *brake = 0.0f;
    const float error = multiplier * static_cast<float>(vehicle->m_autoPilot.m_nCruiseSpeed) - speedForAi;
    if (error > 0.0f) {
        if (speedForAi < 25.0f) { // 0x858FE8
            const float scaled = 0.1f * error; // 0x858B1C
            *accel = 1.0f < scaled ? 1.0f : scaled;
        } else {
            *accel = 1.0f;
        }
        // Subtipo 10 (0x594) con error > 3 y temporizador (0x818) agotado: lo rearma a 10
        if (*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(vehicle) + 0x594) == 10 && error > 3.0f) {
            float& timer = *reinterpret_cast<float*>(reinterpret_cast<uint8*>(vehicle) + 0x818);
            if (timer <= 0.0f) {
                timer = 10.0f;
            }
        }
    } else {
        const float scaled = error * -0.05f; // 0x85901C
        *accel = 0.0f;
        *brake = 0.5f < scaled ? 0.5f : scaled; // 0x858B8C
    }
    *steer = diff;
}

// 0x4335E0
void CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget(CVehicle* vehicle, CEntity* Unusued, float arg3, float arg4, float arg5, float arg6, float* arg7, float* arg8, float* arg9, bool* arg10) {
    // (arg5, arg6) es la velocidad del objetivo: se limita a longitud 0.13 (0x859020) y se proyecta 60 fotogramas desde (arg3, arg4)
    float directionX = arg5;
    float directionY = arg6;
    const float length = std::sqrt(directionX * directionX + directionY * directionY);
    if (length > 0.13f) { // con NaN (comparacion desordenada) no escala, como el EXE
        const float scale = 0.13f / length;
        directionX = directionX * scale;
        directionY = directionY * scale;
    }
    const float targetX = directionX * GAME_SPEED_TO_CAR_AI_SPEED + arg3;
    const float targetY = directionY * GAME_SPEED_TO_CAR_AI_SPEED + arg4;
    vehicle->m_autoPilot.m_nCarDrivingStyle = static_cast<eCarDrivingStyle>(2);
    SteerAICarWithPhysicsHeadingForTarget(vehicle, nullptr, targetX, targetY, arg7, arg8, arg9, arg10);

    // Cerca del punto (a menos de 5 m): alterna la mision 0x12 -> 0x13; cualquier otra -> 6
    const CVector& pos = vehicle->GetPosition();
    const float distanceSq = (targetY - pos.y) * (targetY - pos.y) + (targetX - pos.x) * (targetX - pos.x);
    if (distanceSq < 25.0f) { // 0x858FE8
        vehicle->m_autoPilot.m_nCarMission = static_cast<eCarMission>(vehicle->m_autoPilot.m_nCarMission == static_cast<eCarMission>(0x12) ? 0x13 : 6);
    }
}

// 0x428990
void CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop(CVehicle* vehicle, float x, float y, float arg4, float arg5, float* arg6, float* arg7, float* arg8, bool* arg9) {
    // Mision 0x13 (BLOCKCAR_HANDBRAKESTOP) / 0x12 (BLOCKCAR_CLOSE) / 5 y 6 (BLOCKPLAYER_*): se comparan por numero
    *arg6 = 0.0f;
    *arg7 = 0.0f;
    *arg8 = 1.0f;
    *arg9 = true;

    auto& autoPilot = vehicle->m_autoPilot;
    const CVector& pos = vehicle->GetPosition();
    const float distanceSq = (pos.y - y) * (pos.y - y) + (pos.x - x) * (pos.x - x);

    // Lejos (> 10 m): la mision pasa a su version "close" y no hace nada mas
    if (distanceSq > 100.0f) { // 0x858628
        autoPilot.m_nCarMission = static_cast<eCarMission>(autoPilot.m_nCarMission == static_cast<eCarMission>(0x13) ? 0x12 : 5);
        return;
    }

    const auto tellOccupantsToLeave = [&] {
        CCarAI::TellOccupantsToLeaveCar(vehicle); // 0x41C760
        autoPilot.m_nCruiseSpeed = 0;
        autoPilot.m_nCarMission = MISSION_NONE;
    };

    // El EXE vuelve a pedir FindPlayerVehicle(-1, false) cada vez que lo usa
    const auto playerSpeedBelowLimit = [] {
        const CVector& playerSpeed = FindPlayerVehicle(-1, false)->m_vecMoveSpeed;
        const float magnitude = std::sqrt((playerSpeed.x * playerSpeed.x + playerSpeed.y * playerSpeed.y) + playerSpeed.z * playerSpeed.z);
        return magnitude < 0.05f; // 0x858C28
    };

    if (autoPilot.m_nCarMission == static_cast<eCarMission>(0x13)) {
        if (CPed* const driver = vehicle->m_pDriver) {
            if (CTask* const task = driver->GetTaskManager().GetActiveTask()) { // 0x681720
                if (task->GetTaskType() == TASK_COMPLEX_KILL_CRIMINAL) { // 0x451
                    return;
                }
            }
        }
        const CVector& moveSpeed = vehicle->m_vecMoveSpeed;
        if (!(moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y < 0.0001f)) { // 0x858FC4
            return;
        }
        if (!(arg4 * arg4 + arg5 * arg5 < 0.0004f)) { // 0x858FC0
            return;
        }
        if (!vehicle->vehicleFlags.bIsLawEnforcer) {
            return;
        }
        tellOccupantsToLeave();
        return;
    }

    if (FindPlayerVehicle(-1, false) && playerSpeedBelowLimit()) {
        // temporizador (uint16) = trunc(paso de tiempo * 16.666... + temporizador)
        const float next = CTimer::ms_fTimeStep * 16.666666f + static_cast<float>(static_cast<uint16>(vehicle->m_nCopsInCarTimer)); // 0x858CF0
        vehicle->m_nCopsInCarTimer = static_cast<int16>(static_cast<int64>(next));
    } else {
        vehicle->m_nCopsInCarTimer = 0;
    }

    if (FindPlayerVehicle(-1, false)) {
        if (!FindPlayerVehicle(-1, false)->IsUpsideDown()) { // 0x6D1D90
            if (!playerSpeedBelowLimit()) {
                return;
            }
            if (static_cast<uint16>(vehicle->m_nCopsInCarTimer) <= 0x9C4u) {
                return;
            }
        }
    }
    if (!vehicle->vehicleFlags.bIsLawEnforcer) {
        return;
    }
    if (!(distanceSq < 100.0f)) {
        return;
    }
    tellOccupantsToLeave();
}

// 0x436A90
void CCarCtrl::SteerAICarWithPhysics_OnlyMission(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x436A90, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x42AAD0
void CCarCtrl::SteerAIHeliAsPoliceHeli(CAutomobile* automobile) {
    // Como el original: el objetivo (+0x41C) se lee sin comprobar nulo y se relee tras GetATanOfXY; altitud maxima = max(z del objetivo, 6.0) (NaN -> 6.0) y,
    // si la distancia horizontal es > 50.0, max(altitud, 25.0); destino = posicion del objetivo (copia en bruto); si la salud (+0x4C0) < 230.0, mision HELI_FLY_AWAY_FROM_PLAYER (0x2A)
    auto* const heli = static_cast<CHeli*>(automobile);
    auto& autoPilot = heli->m_autoPilot;
    CVehicle* target = autoPilot.m_TargetEntity;
    const CVector& targetPos = target->GetPosition();
    const CVector& pos = heli->GetPosition();
    const float dy = CcAirSubF(targetPos.y, pos.y);
    const float dx = CcAirSubF(targetPos.x, pos.x);
    const float heading = CGeneral::GetATanOfXY(dx, dy);
    target = autoPilot.m_TargetEntity;
    const float dist = CcAirHorizDist(&target->GetPosition().x, &heli->GetPosition().x);
    float altitude = target->GetPosition().z;
    if (!(altitude > 6.0f)) {
        altitude = 6.0f;
    }
    heli->m_fMaxAltitude = altitude; // 0x9A8
    if (dist > 50.0f) {
        if (!(altitude > 25.0f)) {
            altitude = 25.0f;
        }
        heli->m_fMaxAltitude = altitude;
    }
    autoPilot.m_vecDestinationCoors = target->GetPosition(); // 0x3EC
    FlyAIHeliInCertainDirection(heli, heading, dist, true);
    if (heli->m_fHealth < 230.0f) {
        autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
    }
}

// 0x42ACB0
void CCarCtrl::SteerAIHeliFlyingAwayFromPlayer(CAutomobile* automobile) {
    // Como el original: dos llamadas a FindPlayerCoors(-1); x sale de la primera e y de la segunda. El rumbo es GetATanOfXY + pi (0x858CB8) con un solo guardado a float
    auto* const heli = static_cast<CHeli*>(automobile);
    const CVector& pos = heli->GetPosition();
    const CVector playerPos1 = FindPlayerCoors(-1);
    const CVector playerPos2 = FindPlayerCoors(-1);
    const float dy = CcAirSubF(playerPos2.y, pos.y);
    const float dx = CcAirSubF(playerPos1.x, pos.x);
    const float heading = CcAirAddF(CGeneral::GetATanOfXY(dx, dy), std::bit_cast<float>(0x40490FDBu));
    FlyAIHeliInCertainDirection(heli, heading, 1000.0f, false);
}

// 0x4238E0
void CCarCtrl::SteerAIHeliToCrashAndBurn(CAutomobile* automobile) {
    // Como el original: el bit 0 de m_nRandomSeed (word en +0x20) decide el signo; con bit 0 copia field_A14 en bruto (mov) y sin el bit lo niega con la x87 (fchs)
    auto* const heli = static_cast<CHeli*>(automobile);
    const bool odd = (heli->m_nRandomSeed & 1) != 0;
    heli->m_fSteeringUpDown = std::bit_cast<float>(0xBE99999Au); // 0x990: -0.3f
    heli->m_fLeftRightSkid = odd ? heli->field_A14 : CcAirNegF(heli->field_A14); // 0x98C
    heli->m_fAccelerationBreakStatus = -0.5f; // 0x998
    heli->m_fSteeringLeftRight = odd ? 1.0f : -1.0f; // 0x994
}

// 0x42A750
void CCarCtrl::SteerAIHeliToFollowEntity(CAutomobile* automobile) {
    // Como el original: destino (+0x3EC) = posicion del objetivo (+0x41C, sin comprobar nulo), desplazada por su eje adelante x el multiplicador con signo +0x3DA si es distinto de 0
    // (crea la matriz del objetivo si falta); rumbo hacia el destino y distancia horizontal al objetivo; altitud maxima = max(z del objetivo, 6.0) y, a mas de 50.0, max(altitud, 25.0);
    // con +0x954 >= 0 (ordenado) FlyAIHeliToTarget_FixedOrientation(posicion del objetivo) o si no FlyAIHeliInCertainDirection(rumbo, distancia, true); con bDoTargetCatchupCheck y a menos de 25.0 el
    // objetivo pasa a conducir en crucero (vehiculo en estado 2 o 3) o a esprintar (ped con tarea Wander); con bHeliFollowTarget y 50 s desde la creacion pasa a HELI_FLY_AWAY_FROM_PLAYER
    auto* const heli = static_cast<CHeli*>(automobile);
    auto& autoPilot = heli->m_autoPilot;
    CVehicle* target = autoPilot.m_TargetEntity;
    autoPilot.m_vecDestinationCoors = target->GetPosition();
    if (autoPilot.field_4A != 0) {
        if (!target->m_matrix) {
            target->AllocateMatrix();
            target->m_placement.UpdateMatrix(target->m_matrix);
        }
        CcAirFollowOffset(&target->m_matrix->GetForward().x, static_cast<int8>(autoPilot.field_4A), &autoPilot.m_vecDestinationCoors.x);
    }
    const CVector& dest = autoPilot.m_vecDestinationCoors;
    const float dy = CcAirSubF(dest.y, heli->GetPosition().y);
    const float dx = CcAirSubF(dest.x, heli->GetPosition().x);
    const float heading = CGeneral::GetATanOfXY(dx, dy);
    target = autoPilot.m_TargetEntity;
    const float dist = CcAirHorizDist(&target->GetPosition().x, &heli->GetPosition().x);
    float altitude = target->GetPosition().z;
    if (!(altitude > 6.0f)) {
        altitude = 6.0f;
    }
    heli->m_fMaxAltitude = altitude; // 0x9A8
    if (dist > 50.0f) {
        if (!(altitude > 25.0f)) {
            altitude = 25.0f;
        }
        heli->m_fMaxAltitude = altitude;
    }
    if (heli->m_fForcedOrientation >= 0.0f) {
        FlyAIHeliToTarget_FixedOrientation(heli, heli->m_fForcedOrientation, target->GetPosition());
    } else {
        FlyAIHeliInCertainDirection(heli, heading, dist, true);
    }
    if (autoPilot.carCtrlFlags.bDoTargetCatchupCheck) {
        CVehicle* const entity = autoPilot.m_TargetEntity;
        if (CcAirHorizDistD(&heli->GetPosition().x, &entity->GetPosition().x) < 25.0f) {
            if (entity->GetType() == ENTITY_TYPE_VEHICLE) {
                const auto status = entity->GetStatus();
                if (status == STATUS_SIMPLE || status == STATUS_PHYSICS) {
                    entity->SetStatus(STATUS_PHYSICS);
                    entity->m_autoPilot.m_nCarMission = MISSION_CRUISE;                // 0x3BA
                    entity->m_autoPilot.m_nCruiseSpeed = 100;                          // 0x3D0
                    entity->m_autoPilot.m_nCarDrivingStyle = DRIVING_STYLE_AVOID_CARS; // 0x3B9
                }
            } else if (entity->GetType() == ENTITY_TYPE_PED) {
                // Como el original: el "vehiculo" objetivo es un ped; su CTaskManager esta en m_pIntelligence + 4
                auto* const ped = reinterpret_cast<CPed*>(entity);
                if (CTask* const task = ped->GetTaskManager().GetActiveTask()) {
                    if (task->GetTaskType() == TASK_COMPLEX_WANDER) {
                        static_cast<CTaskComplexWander*>(task)->m_nMoveState = PEDMOVE_SPRINT;
                    }
                }
            }
            autoPilot.carCtrlFlags.bDoTargetCatchupCheck = false;
        }
    }
    if (autoPilot.carCtrlFlags.bHeliFollowTarget) {
        if (CTimer::GetTimeInMS() > heli->m_nCreationTime + 50000u) {
            autoPilot.carCtrlFlags.bHeliFollowTarget = false;
            autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
        }
    }
    if (autoPilot.m_nCarMission == MISSION_HELI_NEWS_BEHAVIOUR) {
        if (heli->m_fHealth < 300.0f) {
            autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
        }
    }
}

// 0x42AEB0
void CCarCtrl::SteerAIHeliToKeepEntityInView(CAutomobile* automobile) {
    // Como el original: rumbo hacia el objetivo (+0x41C) y distancia horizontal; a mas del doble de +0x3D8 (byte) sigue al objetivo con SteerAIHeliToFollowEntity; si no, mantiene el
    // objetivo de lado: deslizamiento (+0x98C) con la diferencia entre rumbo + pi/2 y el rumbo del helicoptero normalizada y limitada a [-1, 1]; altitud maxima (+0x9A8, +0x9AC) = z del
    // objetivo + 15.0 y destino = su posicion; freno/aceleracion (+0x998) con ganancia 0.1/0.2 + 0.3, mas una rand() & 0xF del EXE, limitado a [0, 1]; mandos +0x994 y +0x990 segun la
    // distancia (m_matrix del helicoptero se usa sin comprobar nulo)
    auto* const heli = static_cast<CHeli*>(automobile);
    auto& autoPilot = heli->m_autoPilot;
    CVehicle* target = autoPilot.m_TargetEntity;
    const float dy = CcAirSubF(target->GetPosition().y, heli->GetPosition().y);
    const float dx = CcAirSubF(target->GetPosition().x, heli->GetPosition().x);
    const float heading = CGeneral::GetATanOfXY(dx, dy);
    target = autoPilot.m_TargetEntity;
    float distance;
    if (CcAirViewFar(&target->GetPosition().x, &heli->GetPosition().x, autoPilot.m_ucHeliTargetDist, &distance)) {
        SteerAIHeliToFollowEntity(heli);
        return;
    }
    const CMatrix& matrix = *heli->m_matrix;
    const float rotated = CcAirAddF(heading, std::bit_cast<float>(0x3FC90FDBu)); // + pi/2 (0x858FE4)
    const float ownHeading = CGeneral::GetATanOfXY(matrix.GetForward().x, matrix.GetForward().y);
    float skidRaw, skid;
    CcAirViewSkid(rotated, ownHeading, &skidRaw, &skid);
    heli->m_fLeftRightSkid = skidRaw; // 0x98C: el EXE guarda dos veces (sin limitar y limitado)
    heli->m_fLeftRightSkid = skid;
    target = autoPilot.m_TargetEntity;
    heli->m_fMaxAltitude = CcAirAddF(target->GetPosition().z, 15.0f); // 0x9A8
    autoPilot.m_vecDestinationCoors = target->GetPosition();
    heli->field_9AC = heli->m_fMaxAltitude; // 0x9AC
    heli->m_fAccelerationBreakStatus = std::bit_cast<float>(0x3E99999Au); // 0x998
    heli->m_fAccelerationBreakStatus = CcAirViewBrake1(heli->m_fMaxAltitude, heli->m_vecMoveSpeed.z, heli->GetPosition().z);
    const int random = CarCtrlAirExeRand() & 0xF;
    heli->m_fAccelerationBreakStatus = CcAirViewBrake2(random, heli->m_fAccelerationBreakStatus);
    CcAirViewLeftRight(distance, autoPilot.m_ucHeliTargetDist, &heli->m_vecMoveSpeed.x, &heli->m_matrix->GetRight().x, &heli->m_fSteeringLeftRight);
    heli->m_fSteeringUpDown = 0.0f; // 0x990
    float upDown;
    if (CcAirViewUpDown(distance, autoPilot.m_ucHeliTargetDist, &heli->m_vecMoveSpeed.x, &heli->m_matrix->GetForward().x, &upDown)) {
        heli->m_fSteeringUpDown = upDown;
    }
}

// 0x42AD30
void CCarCtrl::SteerAIHeliToLand(CAutomobile* automobile) {
    // Como el original: vuela hacia el destino (+0x3EC) con FlyAIHeliInCertainDirection(heli, rumbo, distancia, true); si la distancia es < 10.0 y la velocidad horizontal < 0.05
    // (comparaciones ordenadas) pone las altitudes (+0x9B0, +0x9A8) a 0 y, si alguna compresion de rueda (+0x7D4..) es < 1.0, los cuatro mandos (+0x998, +0x98C, +0x990, +0x994) a 0
    auto* const heli = static_cast<CHeli*>(automobile);
    const CVector& dest = heli->m_autoPilot.m_vecDestinationCoors;
    const CVector& pos = heli->GetPosition();
    const float dy = CcAirSubF(dest.y, pos.y);
    const float dx = CcAirSubF(dest.x, pos.x);
    const float heading = CGeneral::GetATanOfXY(dx, dy);
    const float dist = CcAirHorizDist(&dest.x, &heli->GetPosition().x);
    FlyAIHeliInCertainDirection(heli, heading, dist, true);
    if (!(dist < 10.0f)) {
        return;
    }
    if (!(CcAirSpeed2D(&heli->m_vecMoveSpeed.x) < std::bit_cast<float>(0x3D4CCCCDu))) { // 0.05f (0x858C28)
        return;
    }
    heli->m_fMinAltitude = 0.0f; // 0x9B0
    heli->m_fMaxAltitude = 0.0f; // 0x9A8
    const auto& compression = heli->m_fWheelsSuspensionCompression; // 0x7D4
    if (compression[0] < 1.0f || compression[1] < 1.0f || compression[2] < 1.0f || compression[3] < 1.0f) {
        heli->m_fAccelerationBreakStatus = 0.0f; // 0x998
        heli->m_fLeftRightSkid = 0.0f;           // 0x98C
        heli->m_fSteeringUpDown = 0.0f;          // 0x990
        heli->m_fSteeringLeftRight = 0.0f;       // 0x994
    }
}

// 0x42A630
void CCarCtrl::SteerAIHeliTowardsTargetCoors(CAutomobile* automobile) {
    // Como el original: con orientacion forzada (+0x954 >= 0, comparacion ordenada: NaN no entra) vuela con FlyAIHeliToTarget_FixedOrientation pasando el destino por valor;
    // si no, rumbo con GetATanOfXY (y, x restados con la x87) y distancia horizontal en float a FlyAIHeliInCertainDirection(heli, rumbo, distancia, true)
    auto* const heli = static_cast<CHeli*>(automobile);
    const CVector& dest = heli->m_autoPilot.m_vecDestinationCoors; // 0x3EC
    if (heli->m_fForcedOrientation >= 0.0f) {
        FlyAIHeliToTarget_FixedOrientation(heli, heli->m_fForcedOrientation, dest);
        return;
    }
    const CVector& pos = heli->GetPosition();
    const float dy = CcAirSubF(dest.y, pos.y);
    const float dx = CcAirSubF(dest.x, pos.x);
    const float heading = CGeneral::GetATanOfXY(dx, dy);
    const float dist = CcAirHorizDist(&dest.x, &heli->GetPosition().x);
    FlyAIHeliInCertainDirection(heli, heading, dist, true);
}

// 0x423880
void CCarCtrl::SteerAIPlaneToCrashAndBurn(CAutomobile* automobile) {
    // Como el original: el bit 0 de m_nRandomSeed (word en +0x20) decide el signo de +0x988 y +0x990; +0x98C = -0.3f y +0x994 = 0
    auto* const plane = static_cast<CPlane*>(automobile);
    const bool odd = (plane->m_nRandomSeed & 1) != 0;
    plane->m_fSteeringUpDown = std::bit_cast<float>(0xBE99999Au); // 0x98C: -0.3f
    plane->m_fLeftRightSkid = odd ? 1.0f : -1.0f;                  // 0x988
    plane->m_fAccelerationBreakStatus = 0.0f;                      // 0x994
    plane->m_fSteeringLeftRight = odd ? 1.0f : -1.0f;              // 0x990
}

// 0x4237F0
void CCarCtrl::SteerAIPlaneToFollowEntity(CAutomobile* automobile) {
    // Como el original: el objetivo (+0x41C) se lee sin comprobar nulo y se vuelve a leer tras GetATanOfXY; la altitud maxima (+0x9A8) es la z del objetivo copiada en bruto
    auto* const plane = static_cast<CPlane*>(automobile);
    const CVector& targetPos = plane->m_autoPilot.m_TargetEntity->GetPosition();
    const CVector& pos = plane->GetPosition();
    const float dy = CcAirSubF(targetPos.y, pos.y);
    const float dx = CcAirSubF(targetPos.x, pos.x);
    const float heading = CGeneral::GetATanOfXY(dx, dy);
    CVehicle* const target = plane->m_autoPilot.m_TargetEntity;
    plane->m_planeHeading = heading; // 0x9B4
    plane->m_maxAltitude = target->GetPosition().z; // 0x9A8
    FlyAIPlaneInCertainDirection(plane);
}

// 0x423790
void CCarCtrl::SteerAIPlaneTowardsTargetCoors(CAutomobile* automobile) {
    // Como el original: y y x (en ese orden) se restan con la x87 y se guardan a float; el rumbo va a +0x9B4
    auto* const plane = static_cast<CPlane*>(automobile);
    const CVector& dest = plane->m_autoPilot.m_vecDestinationCoors; // 0x3EC
    const CVector& pos = plane->GetPosition();
    const float dy = CcAirSubF(dest.y, pos.y);
    const float dx = CcAirSubF(dest.x, pos.x);
    plane->m_planeHeading = CGeneral::GetATanOfXY(dx, dy); // 0x9B4
    FlyAIPlaneInCertainDirection(plane);
}

// 0x422590
bool CCarCtrl::StopCarIfNodesAreInvalid(CVehicle* vehicle) {
    // Como el original (0x422590..0x422618): las direcciones de enlace (+0x3A4, +0x3A6) buscan el area con `>> 10`, las de nodo (+0x390, +0x394) con
    // la palabra entera (sin acotar el indice); 0xFFFF o un area sin cargar en cualquiera de las cuatro marca el coche como parado (bit 0 de +0x3DC).
    const auto* const areas = reinterpret_cast<CPathNode* const*>(&ThePaths.m_pPathNodes);
    const auto* const raw = reinterpret_cast<const uint8*>(vehicle);
    const auto word = [&](size_t offset) { return *reinterpret_cast<const uint16*>(raw + offset); };
    const bool valid = word(0x3A4) != 0xFFFF && areas[word(0x3A4) >> 10]
        && word(0x3A6) != 0xFFFF && areas[word(0x3A6) >> 10]
        && word(0x390) != 0xFFFF && areas[word(0x390)]
        && word(0x394) != 0xFFFF && areas[word(0x394)];
    if (valid) {
        return false;
    }
    vehicle->m_autoPilot.m_nMovementFlags |= 1; // +0x3DC: bIsStopped
    return true;
}

// 0x4222A0
void CCarCtrl::SwitchBetweenPhysicsAndGhost(CVehicle* vehicle) {
    // Como el original (0x4222A0..0x42236C): solo coches de mision (+0x4A4 == 2) con el indicador 0x4000 de +0x40, y que no sean helicoptero,
    // avion ni barco (+0x594 == 3, 4, 5). El estado son los 5 bits altos del byte +0x36: 3 (fisica) -> 0xC (fantasma) si el colisionado NO esta cargado
    // en la posicion (CColStore::HasCollisionLoaded en +0x14 ? matriz + 0x30 : +4) y, en un automovil (+0x594 == 0), las cuatro ruedas a estado 0;
    // 0xC -> 3 si esta cargado, y entonces PlaceOnRoadProperly del automovil (+0x590 == 0) o de la moto (+0x590 == 9).
    const auto* const raw = reinterpret_cast<const uint8*>(vehicle);
    if (!(*reinterpret_cast<const uint32*>(raw + 0x40) & 0x4000u) || raw[0x4A4] != 2) {
        return;
    }
    const int32 subType = *reinterpret_cast<const int32*>(raw + 0x594);
    if (subType == 5 || subType == 4 || subType == 3) {
        return;
    }
    uint8& statusByte = *const_cast<uint8*>(raw + 0x36);
    const uint32 status = static_cast<uint8>(statusByte >> 3);
    if (status == 3) {
        if (CColStore::HasCollisionLoaded(vehicle->GetPosition(), AREA_CODE_NORMAL_WORLD)) {
            return;
        }
        statusByte = (statusByte & 7) | 0x60;
        if (*reinterpret_cast<const int32*>(raw + 0x594) == 0) {
            auto& damageManager = *reinterpret_cast<CDamageManager*>(const_cast<uint8*>(raw) + 0x5A0);
            for (auto wheel = 0; wheel <= 3; wheel++) {
                damageManager.SetWheelStatus(static_cast<eCarWheel>(wheel), static_cast<eCarWheelStatus>(0));
            }
        }
    } else if (status == 0xC) {
        if (!CColStore::HasCollisionLoaded(vehicle->GetPosition(), AREA_CODE_NORMAL_WORLD)) {
            return;
        }
        statusByte = (statusByte & 7) | 0x18;
        const int32 type = *reinterpret_cast<const int32*>(raw + 0x590);
        if (type == 0) {
            reinterpret_cast<CAutomobile*>(vehicle)->PlaceOnRoadProperly();
        } else if (type == 9) {
            reinterpret_cast<CBike*>(vehicle)->PlaceOnRoadProperly();
        }
    }
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
    // Como el original: cada operacion x87 va a 24 bits (palabra de control del juego), es decir, aritmetica de float. Los vehiculos, sus col models y
    // las dos direcciones se usan sin comprobar nulos. El tiempo devuelto es el menor de los dos ejes (empieza en 1.0f).
    const CVector& position1 = vehicle1->GetPosition();
    const CVector& position2 = vehicle2->GetPosition();
    const float deltaX = position1.x - position2.x;
    const float deltaY = position1.y - position2.y;
    const CBoundingBox& box2 = CModelInfo::GetModelInfo(static_cast<int16>(vehicle2->m_nModelIndex))->GetColModel()->GetBoundingBox();
    const CBoundingBox& box1 = CModelInfo::GetModelInfo(static_cast<int16>(vehicle1->m_nModelIndex))->GetColModel()->GetBoundingBox();
    const float halfX2     = box2.m_vecMax.x; // 0x0C
    const float halfY2     = box2.m_vecMax.y; // 0x1C
    const float negMinY2   = -box2.m_vecMin.y; // 0x20
    const float halfY1     = box1.m_vecMax.y; // 0x24
    const float halfX1     = box1.m_vecMax.x; // 0x28
    const float moveAcross = arg3 * pos1->y - arg4 * pos1->x; // 0x34
    const float moveAlong  = arg3 * pos1->x + arg4 * pos1->y; // 0x38

    float best = 1.0f;
    for (int32 pass = 0; pass < 2; pass++) {
        float cornerA; // 0x3C
        float cornerB; // 0x40
        if (pass == 0) {
            cornerA = (halfY1 * pos2->x + halfX1 * pos2->y) + deltaX;
            cornerB = (halfY1 * pos2->y + deltaY) - halfX1 * pos2->x;
        } else {
            cornerA = (halfY1 * pos2->x + deltaX) - halfX1 * pos2->y;
            cornerB = (halfX1 * pos2->x + halfY1 * pos2->y) + deltaY;
        }

        // Primer eje
        float enter1 = 0.0f; // 0x10
        float exit1;         // ST0 al terminar el eje
        const float rel1 = cornerA * pos1->y - cornerB * pos1->x; // 0x50
        if (rel1 > halfX2) {
            if (moveAcross < 0.0f) {
                const float inverse = 1.0f / moveAcross;
                const float t = -((rel1 - halfX2) * inverse);
                if (t < 1.0f) {
                    enter1 = t;
                    const float u = t - inverse * (halfX2 + halfX2);
                    exit1 = u < 1.0f ? u : 1.0f;
                } else {
                    enter1 = 1.0f;
                    exit1  = 1.0f;
                }
            } else {
                enter1 = 1.0f;
                exit1  = 1.0f;
            }
        } else if (-halfX2 > rel1) {
            if (moveAcross > 0.0f) {
                const float inverse = 1.0f / moveAcross;
                const float t = -((rel1 + halfX2) * inverse);
                if (t < 1.0f) {
                    enter1 = t;
                    const float u = inverse * (halfX2 + halfX2) + t;
                    exit1 = u < 1.0f ? u : 1.0f;
                } else {
                    enter1 = 1.0f;
                    exit1  = 1.0f;
                }
            } else {
                enter1 = 1.0f;
                exit1  = 1.0f;
            }
        } else {
            if (moveAcross > 0.0f) {
                exit1 = (halfX2 - rel1) / moveAcross;
            } else if (moveAcross < 0.0f) {
                exit1 = -((rel1 + halfX2) / moveAcross);
            } else {
                exit1 = 1.0f;
            }
        }

        // Segundo eje: enter2 (pila) y exit2 (0x14)
        float enter2;
        float exit2 = 1.0f; // 0x14
        const float rel2 = cornerB * pos1->y + cornerA * pos1->x; // 0x50
        if (rel2 > halfY2) {
            if (moveAlong < 0.0f) {
                const float inverse = 1.0f / moveAlong;
                const float v = -((rel2 - halfY2) * inverse);
                if (v < 1.0f) {
                    const float w = v - (negMinY2 + halfY2) * inverse;
                    enter2 = v;
                    exit2  = w < 1.0f ? w : 1.0f;
                } else {
                    enter2 = 1.0f;
                }
            } else {
                enter2 = 1.0f;
            }
        } else if (-negMinY2 > rel2) {
            if (moveAlong > 0.0f) {
                const float inverse = 1.0f / moveAlong;
                const float v = -((rel2 + negMinY2) * inverse);
                if (v < 1.0f) {
                    const float w = (negMinY2 + halfY2) * inverse + v;
                    enter2 = v;
                    exit2  = w < 1.0f ? w : 1.0f;
                } else {
                    enter2 = 1.0f;
                }
            } else {
                enter2 = 1.0f;
            }
        } else {
            enter2 = 0.0f;
            if (moveAlong > 0.0f) {
                exit2 = (halfY2 - rel2) / moveAlong;
            } else if (moveAlong < 0.0f) {
                exit2 = -((rel2 + negMinY2) / moveAlong);
            }
        }

        const float enter = enter1 > enter2 ? enter1 : enter2; // 0x50
        if (enter < exit1 && enter < exit2 && !(best < enter)) {
            best = enter;
        }
    }
    return best;
}

// 0x425F70
float CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper(CVehicle* vehicle1, CVehicle* vehicle2, float arg3, float arg4, CVector* pos1, CVector* pos2) {
    // Como el original: aritmetica de float (x87 a 24 bits), nulos sin comprobar, ocho esquinas del vehiculo 1 desplazadas arg3/arg4 contra el rectangulo del 2.
    const CVector& position1 = vehicle1->GetPosition();
    const float x1 = position1.x;
    const float y1 = position1.y;
    const CBoundingBox& box2 = CModelInfo::GetModelInfo(static_cast<int16>(vehicle2->m_nModelIndex))->GetColModel()->GetBoundingBox();
    const float halfX2 = box2.m_vecMax.x;
    const float halfY2 = box2.m_vecMax.y;
    const CBoundingBox& box1 = CModelInfo::GetModelInfo(static_cast<int16>(vehicle1->m_nModelIndex))->GetColModel()->GetBoundingBox();
    const float halfY1   = box1.m_vecMax.y;
    const float halfX1   = box1.m_vecMax.x;
    const float negMinY1 = -box1.m_vecMin.y;
    const CVector& position2 = vehicle2->GetPosition();
    const float x2 = position2.x;
    const float y2 = position2.y;

    const float acrossFront = halfY2 * pos1->x;   // st(0) duplicado
    const float halfX2Y     = halfX2 * pos1->y;
    const float cornerAX    = (acrossFront + halfX2Y) + x2; // 0x20
    const float halfY2Y     = halfY2 * pos1->y;
    const float halfX2X     = halfX2 * pos1->x;
    const float cornerAY    = (halfY2Y + y2) - halfX2X;     // 0x24
    const float cornerBX    = (acrossFront + x2) - halfX2Y; // 0x38
    const float cornerBY    = y2 + (halfX2X + halfY2Y);     // registro

    float best = 1.0f; // 0x14
    for (int32 corner = 0; corner < 4; corner++) {
        float u;
        float v;
        switch (corner) {
        case 0:
            u = (halfX1 * pos2->y + halfY1 * pos2->x) + x1;
            v = (halfY1 * pos2->y + y1) - halfX1 * pos2->x;
            break;
        case 1:
            u = (halfY1 * pos2->x + x1) - halfX1 * pos2->y;
            v = (halfY1 * pos2->y + halfX1 * pos2->x) + y1;
            break;
        case 2:
            u = (x1 - negMinY1 * pos2->x) + halfX1 * pos2->y;
            v = (y1 - negMinY1 * pos2->y) - halfX1 * pos2->x;
            break;
        default:
            u = (x1 - negMinY1 * pos2->x) - halfX1 * pos2->y;
            v = (y1 - negMinY1 * pos2->y) + halfX1 * pos2->x;
            break;
        }

        const float movedU = u + arg3;
        const float movedV = v + arg4;
        const float before = (v - cornerAY) * pos1->y + (u - cornerAX) * pos1->x;           // 0x10
        const float after  = (movedV - cornerAY) * pos1->y + (movedU - cornerAX) * pos1->x; // 0x1C
        if (!(before > 0.0f)) {
            continue;
        }
        if (!(after < 0.0f)) {
            continue;
        }
        const float sideB = (cornerBX - u) * arg4 - (cornerBY - v) * arg3;
        const float sideA = (cornerAX - u) * arg4 - (cornerAY - v) * arg3;
        if (!(sideB * sideA < 0.0f)) {
            continue;
        }
        const float time = before / (before - after);
        if (!(best < time)) {
            best = time;
        }
    }
    return best;
}

// 0x429520
void CCarCtrl::TestWhetherToFirePlaneGuns(CVehicle* vehicle, CEntity* target) {
    // Como el original: apaga bFireGun y solo sigue con arma 0 o 1 y objetivo no nulo; distancia < 150.0 (dz, dy, dx = objetivo - vehiculo con el x87), direccion normalizada por 0x59C910
    // y bFireGun si dir . adelante > 0.8 (m_matrix del vehiculo se usa sin comprobar nulo)
    vehicle->vehicleFlags.bFireGun = false;
    const uint8 weapon = static_cast<uint8>(vehicle->m_nVehicleWeaponInUse);
    if (weapon != 0 && weapon != 1) {
        return;
    }
    if (!target) {
        return;
    }
    const CVector& vehiclePos = vehicle->GetPosition();
    const CVector& targetPos = target->GetPosition();
    CVector dir;
    if (!CcAirDiffDistLess(&targetPos.x, &vehiclePos.x, 150.0f, &dir.x)) {
        return;
    }
    CarCtrlAirExeNormalise(&dir);
    if (CcAirDirDotGreater(&dir.x, &vehicle->m_matrix->GetForward().x, std::bit_cast<float>(0x3F4CCCCDu))) { // 0.8f (0x858C98)
        vehicle->vehicleFlags.bFireGun = true;
    }
}

// 0x421FE0
bool CCarCtrl::ThisVehicleShouldTryNotToTurn(CVehicle* vehicle) {
    // Como el original (0x421FE0): tabla de 122 bytes (0x422010) indexada por modelo - 403 (movsx de 16 bits, comparacion sin signo contra 0x79) y
    // salto a "true" o "false"; los unicos modelos con 0 son estos nueve: Linerunner, Dumper, Bus, Coach, Packer, Flatbed, Tanker, Roadtrain y Cement Truck.
    switch (static_cast<int32>(static_cast<int16>(vehicle->m_nModelIndex))) {
    case 403: case 406: case 431: case 437: case 443: case 455: case 514: case 515: case 524:
        return true;
    default:
        return false;
    }
}

// 0x429300
void CCarCtrl::TriggerDogFightMoves(CVehicle* vehicle1, CVehicle* vehicle2) {
    // Como el original: solo sin accion temporal; distancia < 70.0 (dz, dy, dx = vehiculo1 - vehiculo2), direccion normalizada por 0x59C910, producto con el eje adelante de vehiculo2
    // (m_matrix sin comprobar nulo) guardado a float, |dz| < 15.0 y rand() & 0xFF (del EXE): 12..15 eligen la accion (dot > 0, 0, 0 y 0.7) y su duracion con una segunda rand()
    auto& autoPilot = vehicle1->m_autoPilot;
    if (autoPilot.m_nTempAction != TEMPACT_NONE) {
        return;
    }
    CVector dir;
    if (!CcAirDiffDistLess(&vehicle1->GetPosition().x, &vehicle2->GetPosition().x, std::bit_cast<float>(0x428C0000u), &dir.x)) { // 70.0f (0x858CE0)
        return;
    }
    CarCtrlAirExeNormalise(&dir);
    const float dot = CcAirDirDotF(&dir.x, &vehicle2->m_matrix->GetForward().x);
    if (!CcAirAbsDiffLess(vehicle1->GetPosition().z, vehicle2->GetPosition().z, 15.0f)) {
        return;
    }
    switch (CarCtrlAirExeRand() & 0xFF) {
    case 12:
        if (!(dot > 0.0f)) {
            return;
        }
        autoPilot.m_nTempAction = TEMPACT_PLANE_FLY_STRAIGHT;
        autoPilot.m_nTempActionTime = static_cast<uint32>(CarCtrlAirExeRand() & 0x3FF) + CTimer::GetTimeInMS() + 0x5DC;
        return;
    case 13:
        if (!(dot > 0.0f)) {
            return;
        }
        autoPilot.m_nTempAction = TEMPACT_PLANE_SHARP_LEFT;
        break;
    case 14:
        if (!(dot > 0.0f)) {
            return;
        }
        autoPilot.m_nTempAction = TEMPACT_PLANE_SHARP_RIGHT;
        break;
    case 15:
        if (!(dot > std::bit_cast<float>(0x3F333333u))) { // 0.7f (0x858CB0)
            return;
        }
        autoPilot.m_nTempAction = TEMPACT_PLANE_FLY_UP;
        autoPilot.m_nTempActionTime = static_cast<uint32>(CarCtrlAirExeRand() & 0x7FF) + CTimer::GetTimeInMS() + 0xBB8;
        return;
    default:
        return;
    }
    autoPilot.m_nTempActionTime = static_cast<uint32>(CarCtrlAirExeRand() & 0x1FF) + CTimer::GetTimeInMS() + 0x2BC;
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
    // Como el original: aritmetica de float (x87 a 24 bits). Los enteros entran por fild (exactos) y se calculan en double antes de redondear una vez a float.
    // Colaboradores sin reconstruir (0x422590, 0x432B10, 0x42EC90, 0x59C910) por su direccion.
    plugin::Call<0x422590, CVehicle*>(vehicle);
    auto& autoPilot = vehicle->m_autoPilot;
    if (WvRaw<uint8>(vehicle, 0x3DC) & 1) { // m_nMovementFlags.bIsStopped
        return;
    }

    const int32 tempAction = static_cast<int8>(autoPilot.m_nTempAction);
    if (tempAction == 1 || tempAction == 24) { // TEMPACT_WAIT, TEMPACT_BRAKE
        vehicle->m_vecMoveSpeed = CVector{ 0.0f, 0.0f, 0.0f };
        autoPilot.ModifySpeed(0.0f); // 0x41B980
        if (CTimer::GetTimeInMS() > autoPilot.m_nTempActionTime) {
            autoPilot.m_nTempAction = TEMPACT_NONE;
            autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS(); // dos lecturas separadas del reloj, como el EXE
            autoPilot.m_nTimeSwitchedToRealPhysics = CTimer::GetTimeInMS();
        }
        return;
    }
    if (tempAction == 12) { // TEMPACT_STUCKINTRAFFIC
        vehicle->m_vecMoveSpeed = CVector{ 0.0f, 0.0f, 0.0f };
        autoPilot.ModifySpeed(0.0f);
        return;
    }

    SlowCarOnRailsDownForTrafficAndLights(vehicle); // 0x434790
    uint32 now = CTimer::GetTimeInMS();
    const int32 limit = static_cast<int32>(autoPilot.field_C) + static_cast<int32>(autoPilot.m_nSpeedScaleFactor);
    if (limit < 0 || now >= static_cast<uint32>(limit)) {
        plugin::Call<0x432B10, CVehicle*>(vehicle);
        now = CTimer::GetTimeInMS();
    }
    if ((WvRaw<uint8>(vehicle, 0x36) & 0xF8) == 0x18) { // estado 3
        return;
    }

    // Tiempo normalizado del tramo: (now - field_C) como entero con signo, +2^32 si es negativo (float), / m_nSpeedScaleFactor (fidiv)
    const int32  elapsed  = static_cast<int32>(now - static_cast<uint32>(autoPilot.field_C));
    const double elapsedD = elapsed < 0 ? static_cast<double>(static_cast<float>(static_cast<double>(elapsed) + 4294967296.0)) : static_cast<double>(elapsed);
    const float  progress = static_cast<float>(elapsedD / static_cast<double>(static_cast<int32>(autoPilot.m_nSpeedScaleFactor)));

    const uint16 currentAddress = *reinterpret_cast<const uint16*>(&autoPilot.m_nCurrentPathNodeInfo);
    const uint16 nextAddress    = *reinterpret_cast<const uint16*>(&autoPilot.m_nNextPathNodeInfo);
    CCarPathLink* const linkA = &(&ThePaths.m_pNaviNodes[0])[currentAddress >> 10][currentAddress & 0x3FF];
    CCarPathLink* const linkB = &(&ThePaths.m_pNaviNodes[0])[nextAddress >> 10][nextAddress & 0x3FF];
    const uint8* const a = reinterpret_cast<const uint8*>(linkA);
    const uint8* const b = reinterpret_cast<const uint8*>(linkB);

    const float laneA = static_cast<float>(autoPilot._smthCurr); // +0x3B5
    const float laneB = static_cast<float>(autoPilot._smthNext); // +0x3B6
    const float aDirX = (static_cast<float>(static_cast<int8>(a[8])) * std::bit_cast<float>(0x3C23D70Au)) * laneA; // 0x858C58 (0.01f)
    const float aDirY = (static_cast<float>(static_cast<int8>(a[9])) * std::bit_cast<float>(0x3C23D70Au)) * laneA;
    const float bDirX = (static_cast<float>(static_cast<int8>(b[8])) * std::bit_cast<float>(0x3C23D70Au)) * laneB;
    const float bDirY = (static_cast<float>(static_cast<int8>(b[9])) * std::bit_cast<float>(0x3C23D70Au)) * laneB;

    float widthA = (linkA->OneWayLaneOffset() + static_cast<float>(autoPilot.m_nCurrentLane)) * std::bit_cast<float>(0x40ACCCCDu); // 0x44DB00, 0x858C50 (5.4f)
    float widthB = (linkB->OneWayLaneOffset() + static_cast<float>(autoPilot.m_nNextLane)) * std::bit_cast<float>(0x40ACCCCDu);
    if (static_cast<int32>(vehicle->m_nVehicleSubType) == 10) {
        widthA = widthA + std::bit_cast<float>(0x3FBA9FBFu); // 0x859010 (1.458f)
        widthB = widthB + std::bit_cast<float>(0x3FBA9FBFu);
    }

    // Variacion pequena segun la semilla del vehiculo y la direccion del tramo (enteros de 3 bits - 3)
    const uint32 seed = WvRaw<uint16>(vehicle, 0x20);
    constexpr float kJitter = std::bit_cast<float>(0x3C1374BCu); // 0x859048 (0.009f)
    const float jitterA1 = static_cast<float>(static_cast<int32>((seed + currentAddress) & 7) - 3) * kJitter;
    const float jitterA2 = static_cast<float>(static_cast<int32>(((seed + currentAddress) >> 3) & 7) - 3) * kJitter;
    const float jitterB1 = static_cast<float>(static_cast<int32>((seed + nextAddress) & 7) - 3) * kJitter;
    const float jitterB2 = static_cast<float>(static_cast<int32>(((seed + nextAddress) >> 3) & 7) - 3) * kJitter;

    CVector directionA{ jitterA1 + aDirX, jitterA2 + aDirY, 0.0f };
    CVector directionB{ jitterB1 + bDirX, jitterB2 + bDirY, 0.0f };
    plugin::CallMethod<0x59C910, CVector*>(&directionA); // CVector::Normalise del EXE
    plugin::CallMethod<0x59C910, CVector*>(&directionB);

    constexpr float kEighth = 0.125f; // 0x858C48
    const CVector start{ static_cast<float>(reinterpret_cast<const int16*>(a)[0]) * kEighth + widthA * aDirY, static_cast<float>(reinterpret_cast<const int16*>(a)[1]) * kEighth - widthA * aDirX, 0.0f };
    const CVector end{ static_cast<float>(reinterpret_cast<const int16*>(b)[0]) * kEighth + widthB * bDirY, static_cast<float>(reinterpret_cast<const int16*>(b)[1]) * kEighth - widthB * bDirX, 0.0f };

    CVector position;
    CVector speed;
    CCurves::CalcCurvePoint(start, end, directionA, directionB, progress, static_cast<int32>(autoPilot.m_nSpeedScaleFactor), position, speed); // 0x43C900
    position.z = 15.0f;
    plugin::Call<0x42EC90, CVehicle*, CVector*>(vehicle, &position);

    constexpr float kPerSecond = std::bit_cast<float>(0x3C888889u); // 0x859044 (1/60)
    speed.x = speed.x * kPerSecond;
    speed.y = speed.y * kPerSecond;
    speed.z = speed.z * kPerSecond;
    if (*reinterpret_cast<const uint16*>(&autoPilot.m_nCurrentPathNodeInfo) != *reinterpret_cast<const uint16*>(&autoPilot.m_nNextPathNodeInfo)) {
        vehicle->m_vecMoveSpeed = speed;
    }
}

// 0x426BC0
void CCarCtrl::WeaveForObject(CEntity* entity, CVehicle* vehicle, float* arg3, float* arg4) {
    // Como el original: aritmetica de float (x87 a 24 bits). Desplazamientos laterales y hacia delante (en el espacio del objeto) segun el modelo.
    float sideOffset;    // 0x...-24
    float forwardOffset; // 0x...-20
    const int32 modelIndex = static_cast<int16>(entity->m_nModelIndex);
    if (modelIndex == int32(uint16(eModelID(ModelIndices::MI_TRAFFICLIGHTS)))) { // 0x8CD4F4
        sideOffset    = std::bit_cast<float>(0x403D3F7Du);
        forwardOffset = std::bit_cast<float>(0x3E16872Bu);
    } else if (modelIndex == int32(uint16(eModelID(ModelIndices::MI_SINGLESTREETLIGHTS1)))) { // 0x8CD518
        sideOffset    = std::bit_cast<float>(0x3F3E76C9u);
        forwardOffset = 0.0f;
    } else if (modelIndex == int32(uint16(eModelID(ModelIndices::MI_SINGLESTREETLIGHTS2)))) { // 0x8CD51C
        sideOffset    = std::bit_cast<float>(0x3D3020C5u);
        forwardOffset = 0.0f;
    } else if (modelIndex == int32(uint16(eModelID(ModelIndices::MI_SINGLESTREETLIGHTS3)))) { // 0x8CD520
        sideOffset    = std::bit_cast<float>(0x3F924DD3u);
        forwardOffset = std::bit_cast<float>(0x3E147AE1u);
    } else if (modelIndex == int32(uint16(eModelID(ModelIndices::MI_DOUBLESTREETLIGHTS)))) { // 0x8CD524
        sideOffset    = 0.0f;
        forwardOffset = std::bit_cast<float>(0xBD449BA6u);
    } else {
        if (!CModelInfo::GetModelInfo(modelIndex)->SwaysInWind()) { // 0x4212C0
            return;
        }
        sideOffset    = 0.0f;
        forwardOffset = 0.0f;
    }

    // Como el original: la posicion se copia antes de crear la matriz y la matriz se comprueba (y se crea) en cuatro puntos
    const CVector entityPosition = entity->GetPosition();
    if (!entity->m_matrix) {
        entity->AllocateMatrix();                         // 0x54F560
        entity->m_placement.UpdateMatrix(entity->m_matrix); // 0x54F1B0
    }
    if (!entity->m_matrix) {
        entity->AllocateMatrix();
        entity->m_placement.UpdateMatrix(entity->m_matrix);
    }
    const float objectX = (forwardOffset * entity->m_matrix->GetForward().x + sideOffset * entity->m_matrix->GetRight().x) + entityPosition.x;
    if (!entity->m_matrix) {
        entity->AllocateMatrix();
        entity->m_placement.UpdateMatrix(entity->m_matrix);
    }
    if (!entity->m_matrix) {
        entity->AllocateMatrix();
        entity->m_placement.UpdateMatrix(entity->m_matrix);
    }
    const float objectY = (sideOffset * entity->m_matrix->GetRight().y + forwardOffset * entity->m_matrix->GetForward().y) + entityPosition.y;

    const CVector& vehiclePosition = vehicle->GetPosition();
    const float deltaX = objectX - vehiclePosition.x;
    const float deltaY = objectY - vehiclePosition.y;
    const float angle  = CGeneral::GetATanOfXY(deltaX, deltaY); // 0x53CC70

    const float halfWidth = CModelInfo::GetModelInfo(static_cast<int16>(vehicle->m_nModelIndex))->GetColModel()->GetBoundingBox().m_vecMax.x * kWvCarWidthScale + kWvCarWidthBase;
    const float distance  = std::sqrt(deltaY * deltaY + deltaX * deltaX);
    const float ratio     = halfWidth / distance;
    const float margin    = ratio * 0.5f;

    auto difference = [angle](float bound) {
        float diff = angle - bound;
        if (diff < -kWvPi) {
            do {
                diff += kWvTwoPi;
            } while (diff < -kWvPi);
        }
        if (diff > kWvPi) {
            do {
                diff -= kWvTwoPi;
            } while (diff > kWvPi);
        }
        if (diff < 0.0f) {
            diff = -diff;
        }
        return diff;
    };

    if (difference(*arg3) < margin) {
        float bound = angle - margin;
        *arg3 = bound;
        if (bound < -kWvPi) {
            do {
                bound += kWvTwoPi;
            } while (bound < -kWvPi);
            *arg3 = bound;
        }
    }
    if (difference(*arg4) < margin) {
        float bound = angle + margin;
        *arg4 = bound;
        if (bound > kWvPi) {
            do {
                bound -= kWvTwoPi;
            } while (bound > kWvPi);
            *arg4 = bound;
        }
    }
}

// 0x426350
void CCarCtrl::WeaveForOtherCar(CEntity* entity, CVehicle* vehicle, float* arg3, float* arg4) {
    // Como el original: aritmetica de float (x87 a 24 bits); los campos de "entity" se leen por desplazamiento como si fuera un vehiculo, salvo donde el EXE
    // comprueba el tipo, y la matriz del vehiculo y la de entity se usan sin comprobar nulo.
    auto& autoPilot = vehicle->m_autoPilot;
    CEntity* const target = autoPilot.m_TargetEntity;
    if (autoPilot.m_nCarMission == MISSION_RAMPLAYER_CLOSE) {
        if (entity == FindPlayerVehicle(-1, false)) { // 0x56E0D0
            return;
        }
    }
    const eCarMission mission = autoPilot.m_nCarMission;
    if (mission == MISSION_RAMCAR_CLOSE) {
        if (entity == target) {
            return;
        }
    }
    if (mission == MISSION_FOLLOWCAR_CLOSE) {
        if (entity == target) {
            return;
        }
        if (entity->GetType() == ENTITY_TYPE_VEHICLE && (WvRaw<uint8>(entity, 0x42B) & 8)) {
            return;
        }
    }
    if (mission == MISSION_KILLPED_CLOSE) {
        if (entity->GetType() == ENTITY_TYPE_PED && (WvRaw<uint32>(entity, 0x46C) & 0x100) && WvRaw<CEntity*>(entity, 0x58C) == target) {
            return;
        }
    }
    const int8 entityMission = WvRaw<int8>(entity, 0x3BA);
    if (entityMission == MISSION_PROTECTION_REAR || entityMission == MISSION_PROTECTION_FRONT || entityMission == MISSION_ESCORT_FRONT
        || entityMission == MISSION_ESCORT_REAR || entityMission == MISSION_ESCORT_LEFT || entityMission == MISSION_ESCORT_RIGHT) {
        if (WvRaw<CEntity*>(entity, 0x41C) == vehicle) {
            return;
        }
    }

    const CVector& entityPosition  = entity->GetPosition();
    const CVector& vehiclePosition = vehicle->GetPosition();
    const CMatrix& vehicleMatrix   = *vehicle->m_matrix;
    const float deltaX = entityPosition.x - vehiclePosition.x; // 0x14 (en el EXE)
    const float deltaY = entityPosition.y - vehiclePosition.y; // 0x3C
    const float alongForward = deltaY * vehicleMatrix.GetForward().y + deltaX * vehicleMatrix.GetForward().x;
    if (alongForward < 0.0f) {
        return;
    }
    const float alongRight = deltaY * vehicleMatrix.GetRight().y + deltaX * vehicleMatrix.GetRight().x;

    const CBoundingBox& vehicleBox = CModelInfo::GetModelInfo(static_cast<int16>(vehicle->m_nModelIndex))->GetColModel()->GetBoundingBox();

    // Cuatro esquinas del rectangulo del vehiculo (con margen) y direcciones de sus lados
    const float aheadY = alongForward > 0.0f ? vehicleBox.m_vecMax.y + kWvMargin : vehicleBox.m_vecMin.y - kWvMargin;
    const CVector frontCorner{ vehicleBox.m_vecMin.x - kWvMargin, aheadY, 0.0f };
    const CVector vehicleCorner1 = WvMatrixTimesPoint(vehicle->m_matrix, frontCorner); // 0x59C890
    const float vehicleWidth = ((vehicleBox.m_vecMax.x + kWvMargin) - vehicleBox.m_vecMin.x) - kWvMargin;
    const CVector vehicleSide1 = WvMatrixTimesVector(vehicle->m_matrix, CVector{ vehicleWidth, 0.0f, 0.0f }); // 0x59C790
    const float sideX = alongRight > 0.0f ? vehicleBox.m_vecMax.x + kWvMargin : vehicleBox.m_vecMin.x - kWvMargin;
    const CVector rearCorner{ sideX, vehicleBox.m_vecMin.y - kWvMargin, 0.0f };
    const CVector vehicleCorner2 = WvMatrixTimesPoint(vehicle->m_matrix, rearCorner);
    const float vehicleLength = ((vehicleBox.m_vecMax.y + kWvMargin) - vehicleBox.m_vecMin.y) - kWvMargin;
    const CVector vehicleSide2 = WvMatrixTimesVector(vehicle->m_matrix, CVector{ 0.0f, vehicleLength, 0.0f });

    // Como el original: la matriz de entity se lee sin comprobar nulo; los productos escalares usan su derecha y su adelante
    const CMatrix& entityMatrix = *entity->m_matrix;
    const float entityAlongRight   = deltaY * entityMatrix.GetRight().y + deltaX * entityMatrix.GetRight().x;
    const float entityAlongForward = deltaY * entityMatrix.GetForward().y + deltaX * entityMatrix.GetForward().x;
    const CBoundingBox& entityBox = CModelInfo::GetModelInfo(static_cast<int16>(entity->m_nModelIndex))->GetColModel()->GetBoundingBox();
    const float entityAheadY = entityAlongForward < 0.0f ? entityBox.m_vecMax.y + kWvMargin : entityBox.m_vecMin.y - kWvMargin;
    const CVector entityCorner1 = WvMatrixTimesPoint(entity->m_matrix, CVector{ entityBox.m_vecMin.x - kWvMargin, entityAheadY, 0.0f });
    const float entityWidth = ((entityBox.m_vecMax.x + kWvMargin) - entityBox.m_vecMin.x) - kWvMargin;
    const CVector entitySide1 = WvMatrixTimesVector(entity->m_matrix, CVector{ entityWidth, 0.0f, 0.0f });
    const float entitySideX = entityAlongRight < 0.0f ? entityBox.m_vecMax.x + kWvMargin : entityBox.m_vecMin.x - kWvMargin;
    const CVector entityCorner2 = WvMatrixTimesPoint(entity->m_matrix, CVector{ entitySideX, entityBox.m_vecMin.y - kWvMargin, 0.0f });
    const float entityLength = ((entityBox.m_vecMax.y + kWvMargin) - entityBox.m_vecMin.y) - kWvMargin;
    const CVector entitySide2 = WvMatrixTimesVector(entity->m_matrix, CVector{ 0.0f, entityLength, 0.0f });

    const bool entityIsWider = vehicleBox.m_vecMax.x < entityBox.m_vecMax.x;
    const float speed = std::sqrt(vehicle->m_vecMoveSpeed.x * vehicle->m_vecMoveSpeed.x + vehicle->m_vecMoveSpeed.y * vehicle->m_vecMoveSpeed.y);
    // Como el original: el argumento 13 es un dword de pila cuyo byte bajo es la bandera y cuyos tres altos son restos del producto escalar entityAlongRight (0x426641)
    const uint32 flagArgument = (std::bit_cast<uint32>(entityAlongRight) & 0xFFFFFF00u) | (entityIsWider ? 1u : 0u);
    const float entitySpeedX = entity->AsPhysical()->m_vecMoveSpeed.x;
    const float entitySpeedY = entity->AsPhysical()->m_vecMoveSpeed.y;

    using SweepTest = bool(__cdecl*)(float, const CVector*, const CVector*, const CVector*, const CVector*, const CVector*, const CVector*, const CVector*, const CVector*, float, float, float, uint32);
    const auto sweepTest = reinterpret_cast<SweepTest>(0x421A50);
    for (int32 step = 0; step < 8;) {
        if (!sweepTest(*arg3, &vehicleCorner1, &vehicleSide1, &vehicleCorner2, &vehicleSide2, &entityCorner1, &entitySide1, &entityCorner2, &entitySide2, entitySpeedX, entitySpeedY, speed, flagArgument)) {
            break;
        }
        step++;
        float value = *arg3 - kWvStep;
        *arg3 = value;
        if (value < 0.0f) {
            value += kWvTwoPi;
            *arg3 = value;
        }
    }
    for (int32 step = 0; step < 8;) {
        if (!sweepTest(*arg4, &vehicleCorner1, &vehicleSide1, &vehicleCorner2, &vehicleSide2, &entityCorner1, &entitySide1, &entityCorner2, &entitySide2, entitySpeedX, entitySpeedY, speed, flagArgument)) {
            break;
        }
        step++;
        float value = *arg4 + kWvStep;
        *arg4 = value;
        if (value > kWvTwoPi) {
            value -= kWvTwoPi;
            *arg4 = value;
        }
    }
}

// 0x42D680
template<typename PtrListType>
void CCarCtrl::WeaveThroughCarsSectorList(PtrListType& ptrList, CVehicle* vehicle, CPhysical* physical, float arg4, float arg5, float arg6, float arg7, float* arg8, float* arg9) {
    // Como el original: arg4/arg6 = x minimo/maximo y arg5/arg7 = y minimo/maximo del rectangulo; matrices sin comprobar nulo (la del vehiculo)
    for (auto* node = ptrList.GetNode(); node;) {
        CEntity* const entity = static_cast<CEntity*>(node->Item);
        node = node->Next;

        if (entity->IsScanCodeCurrent() || !entity->GetUsesCollision() || entity == physical) {
            continue;
        }
        entity->SetScanCode(CWorld::ms_nCurrentScanCode);

        const CVector centre = entity->GetBoundCentre(); // 0x534250
        if (!(centre.x > arg4) || !(centre.x < arg6) || !(centre.y > arg5) || !(centre.y < arg7)) {
            continue;
        }
        if (!(std::fabs(entity->GetPosition().z - vehicle->GetPosition().z) < 8.0f)) { // 0x859000
            continue;
        }
        if (entity == vehicle) {
            continue;
        }
        if ((WvRaw<uint8>(vehicle, 0x42B) & 8) && (WvRaw<uint8>(entity, 0x42B) & 8)) {
            continue;
        }
        WeaveForOtherCar(entity, vehicle, arg8, arg9); // 0x426350
    }
}

// 0x42D950
template<typename PtrListType>
void CCarCtrl::WeaveThroughObjectsSectorList(PtrListType& ptrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float* arg8) {
    // Como el original: se usa la posicion del objeto y solo pasan los que estan casi verticales (up.z > 0.9)
    for (auto* node = ptrList.GetNode(); node;) {
        CEntity* const entity = static_cast<CEntity*>(node->Item);
        node = node->Next;

        if (entity->IsScanCodeCurrent() || !entity->GetUsesCollision()) {
            continue;
        }
        entity->SetScanCode(CWorld::ms_nCurrentScanCode);

        const CVector& position = entity->GetPosition();
        if (!(position.x > arg3) || !(position.x < arg5) || !(position.y > arg4) || !(position.y < arg6)) {
            continue;
        }
        if (!(std::fabs(position.z - vehicle->GetPosition().z) < 8.0f)) { // 0x859000
            continue;
        }
        if (!(entity->GetMatrix().GetUp().z > std::bit_cast<float>(0x3F666666u))) { // 0x411990, 0x858C20 (0.9f)
            continue;
        }
        WeaveForObject(entity, vehicle, arg7, arg8); // 0x426BC0
    }
}

// 0x42D7E0
template<typename PtrListType>
void CCarCtrl::WeaveThroughPedsSectorList(PtrListType& ptrList, CVehicle* vehicle, CPhysical* physical, float arg4, float arg5, float arg6, float arg7, float* arg8, float* arg9) {
    // Como el original: se usa la posicion del peaton (no su centro) y 0x426970 (WeaveForPed, sin catalogar) se llama por su direccion
    for (auto* node = ptrList.GetNode(); node;) {
        CEntity* const entity = static_cast<CEntity*>(node->Item);
        node = node->Next;

        if (entity->IsScanCodeCurrent() || !entity->GetUsesCollision() || entity == physical) {
            continue;
        }
        entity->SetScanCode(CWorld::ms_nCurrentScanCode);

        const CVector& position = entity->GetPosition();
        if (!(position.x > arg4) || !(position.x < arg6) || !(position.y > arg5) || !(position.y < arg7)) {
            continue;
        }
        if (!(std::fabs(position.z - vehicle->GetPosition().z) < 4.0f)) { // 0x858B90
            continue;
        }
        if (WvRaw<CEntity*>(entity, 0x584) == vehicle || WvRaw<CEntity*>(entity, 0xFC) == vehicle) {
            continue;
        }
        plugin::Call<0x426970, CEntity*, CVehicle*, float*, float*>(entity, vehicle, arg8, arg9);
    }
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
