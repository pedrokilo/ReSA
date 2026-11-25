// SilentPatch (MIT, Silent): banco aislado de los arreglos de peds/jugador/generadores; ver SilentPatchPedsSelfTest.h.
#include "StdInc.h"
#include "SilentPatchPedsSelfTest.h"

#include <cstdarg>
#include <cstdio>
#include <set>
#include <vector>

#include "extensions/SilentPatchConfig.h"
#include "TheCarGenerators.h"
#include "CarGenerator.h"
#include "CarEnterExit.h"
#include "RoadBlocks.h"
#include "GangWars.h"
#include "PedGroups.h"
#include "Pickups.h"
#include "PopCycle.h"
#include "PathFind.h"
#include "SilentPatchNewGame.h"
#include "GameLogic.h"
#include "TagManager.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/CivilianPed.h"
#include "Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h"
#include "VehicleAnimGroupData.h"
#include "Tasks/TaskTypes/TaskSimpleJetPack.h"
#include "Entity/Ped/PlayerPed.h"

namespace SilentPatchPedsSelfTest {
namespace {
FILE* g_log = nullptr;

void Log(const char* fmt, ...) {
    if (!g_log) {
        g_log = fopen("logs/silentpatch_peds_selftest.log", "a");
    }
    if (!g_log) {
        return;
    }
    va_list a;
    va_start(a, fmt);
    vfprintf(g_log, fmt, a);
    va_end(a);
    fputc('\n', g_log);
    fflush(g_log);
}

bool Enabled() {
    static const bool v = [] {
        char b[8]{};
        return GetEnvironmentVariableA("GTASA_SILENTPATCH_SELFTEST_PEDS", b, sizeof(b)) != 0 && b[0] == '1';
    }();
    return v;
}

void Remove(CEntity* e) {
    CWorld::Remove(e);
    delete e;
}

size_t UsedPickups() {
    size_t n = 0;
    for (auto& p : CPickups::aPickUps) {
        if (p.m_nPickupType != PICKUP_NONE) {
            n++;
        }
    }
    return n;
}

size_t UsedCarGens() {
    size_t n = 0;
    for (auto& g : CTheCarGenerators::CarGeneratorArray) {
        if (g.m_bIsUsed) {
            n++;
        }
    }
    return n;
}

void Opciones() {
    Log("opciones: general=%d CarGenCounterFix=%d CarGenInteriors=%d RoadblockNoCJClones=%d VanRearDoorFix=%d BikeFrontEnterZ=%d GangWarKeepGroupBlips=%d SecondPlayerBlipFix=%d ConsoleRandomness=%d ResetVariablesOnNewGame=%d ReinitIplObjectsOnNewGame=%d",
        (int)SilentPatchEnabled(), (int)SilentPatchOption("CarGenCounterFix", true), (int)SilentPatchOption("CarGenInteriors", true), (int)SilentPatchOption("RoadblockNoCJClones", true),
        (int)SilentPatchOption("VanRearDoorFix", true), (int)SilentPatchOption("BikeFrontEnterZ", true), (int)SilentPatchOption("GangWarKeepGroupBlips", true),
        (int)SilentPatchOption("SecondPlayerBlipFix", true), (int)SilentPatchOption("ConsoleRandomness", true), (int)SilentPatchOption("ResetVariablesOnNewGame", true),
        (int)SilentPatchOption("ReinitIplObjectsOnNewGame", true));
}

CAutomobile* g_van = nullptr;

// L60 (contador de generadores con n fijo) y L74 (generadores en interiores: area 13 si z >= 950)
void TestCarGenerators(const CVector& at) {
    CStreaming::RequestModel(MODEL_BMX, STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    if (!CStreaming::IsModelLoaded(MODEL_BMX)) {
        Log("L60/L74: BMX no cargado; se omite");
        return;
    }
    const auto savedEven = CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter;
    CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter = 1;
    const CVector pos{ at.x + 1.5f, at.y + 1.5f, at.z };
    const auto idx = CTheCarGenerators::CreateCarGenerator(pos, 0.0f, MODEL_BMX, -1, -1, 1, 0, 0, 0, 0, 0, true);
    Log("L60/L74: generador creado idx=%d en (%.1f %.1f %.1f)", idx, pos.x, pos.y, pos.z);
    if (idx < 0) {
        CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter = savedEven;
        return;
    }
    auto* gen = CTheCarGenerators::Get((uint16)idx);
    gen->SwitchOnWithCount(2);
    int spawned = 0;
    for (int i = 1; i <= 4; i++) {
        gen->m_nNextGenTime = 0;
        gen->bWaitUntilFarFromPlayer = false;
        Log("L60/L74: antes de la pasada %d: handle=%d espera_lejos=%d rango=%d bloqueo=%d contador=%u", i, (int)gen->m_nVehicleHandle, (int)gen->bWaitUntilFarFromPlayer, (int)gen->CheckIfWithinRangeOfAnyPlayers(), (int)gen->CheckForBlockage(MODEL_BMX), (unsigned)gen->m_nGenerateCount);
        gen->m_nNextGenTime = 0;
        gen->bWaitUntilFarFromPlayer = false;
        gen->Process();
        const bool made = gen->m_nVehicleHandle != -1;
        CVehicle* v = made ? GetVehiclePool()->GetAtRef(gen->m_nVehicleHandle) : nullptr;
        if (v) {
            spawned++;
            Log("L60/L74: pasada %d: vehiculo creado modelo=%d area=%d z=%.2f; contador=%u", i, (int)v->GetModelIndex(), (int)v->GetAreaCode(), v->GetPosition().z, (unsigned)gen->m_nGenerateCount);
            gen->m_nVehicleHandle = -1;
            Remove(v); // se elimina del todo: si no, bloquea al generador en la pasada siguiente (CheckForBlockage)
        } else {
            Log("L60/L74: pasada %d: NO se creo vehiculo; contador=%u", i, (unsigned)gen->m_nGenerateCount);
        }
    }
    Log("L60/L74: resumen: %d vehiculos creados en 4 pasadas con contador inicial 2; contador final=%u (con el arreglo: 2 creados y contador 0; sin el: 4 creados y contador 2)", spawned, (unsigned)gen->m_nGenerateCount);
    gen->m_bIsUsed = false;
    CTheCarGenerators::NumOfCarGenerators--;
    CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter = savedEven;
}

// L120: bloqueo de carretera de pandilla en una zona sin pandillas: sin el arreglo se crean clones de CJ (modelo 0)
void TestRoadblock() {
    if (!g_van) {
        Log("L120: sin vehiculo de prueba; se omite");
        return;
    }
    CStreaming::RequestModel(CStreaming::GetDefaultCopCarModel(false), STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    static CZoneInfo empty{};
    auto* const saved = CPopCycle::m_pCurrZoneInfo;
    CPopCycle::m_pCurrZoneInfo = &empty;
    g_van->m_nNumPedsForRoadBlock = 2;
    std::set<CPed*> before;
    for (auto& p : GetPedPool()->GetAllValid()) {
        before.insert(&p);
    }
    CRoadBlocks::GenerateRoadBlockPedsForCar(g_van, 0, PED_TYPE_GANG1);
    int clones = 0;
    std::vector<CPed*> created;
    for (auto& p : GetPedPool()->GetAllValid()) {
        if (!before.count(&p)) {
            created.push_back(&p);
            if ((int)p.GetModelIndex() == 0) {
                clones++;
            }
        }
    }
    Log("L120: bloqueo de pandilla en zona sin pandillas: peds nuevos=%zu, de ellos clones de CJ (modelo 0)=%d (con el arreglo 0; sin el, 2)", created.size(), clones);
    for (auto* p : created) {
        Remove(p);
    }
    CPopCycle::m_pCurrZoneInfo = saved;
}

// L117 (moto de frente: la Z se resta) y L118 (furgoneta: puerta trasera sin desplazamiento de asiento)
void TestDoors() {
    if (!g_van) {
        Log("L117/L118: sin Burrito; se omite");
        return;
    }
    auto* van = g_van;
    Log("L118: Burrito: m_nAnimGroup=%d bIsVan=%d seatOffsetDistance=%.4f", (int)van->m_pHandlingData->m_nAnimGroup, (int)van->vehicleFlags.bIsVan, van->m_pHandlingData->m_fSeatOffsetDistance);
    for (int door : { 9, 11 }) {
        const CVector p = CCarEnterExit::GetPositionToOpenCarDoor(van, door);
        Log("L118: GetPositionToOpenCarDoor(Burrito, puerta %d) = (%.4f %.4f %.4f)", door, p.x, p.y, p.z);
    }
    {
        // Las animaciones de subir a una moto no estan cargadas aqui (ComputeAnimDoorOffsets leeria una asociacion nula): se pre-rellena el desplazamiento memorizado del grupo del Burrito para la puerta 18 (BIKE_FRONT)
        auto& group = CVehicleAnimGroupData::GetVehicleAnimGroup(van->m_pHandlingData->m_nAnimGroup);
        const CVector savedOffset = group.m_aVecDoorOffsets[ENTER_BIKE_FRONT];
        group.m_aVecDoorOffsets[ENTER_BIKE_FRONT] = CVector{ 0.5f, 1.0f, 0.25f };
        CTaskUtilityLineUpPedWithCar task{ CVector{}, 0, 0, 18 };
        const CVector r = task.GetLocalPositionToOpenCarDoor(van, 0.0f, nullptr);
        auto* mi = static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(van->m_nModelIndex));
        const CVector seat = mi->GetFrontSeatPosn();
        Log("L117: GetLocalPositionToOpenCarDoor(puerta 18) en el Burrito: local=(%.4f %.4f %.4f); asiento delantero=(%.4f %.4f %.4f) con desplazamiento sintetico z=0.25 (con el arreglo local.z = asiento.z - 0.25; sin el, asiento.z + 0.25)", r.x, r.y, r.z, seat.x, seat.y, seat.z);
        group.m_aVecDoorOffsets[ENTER_BIKE_FRONT] = savedOffset;
    }
}

// L124: al terminar la guerra no se quitan los blips de los miembros del grupo del jugador
void TestGangWarBlips() {
    auto* ped = new CCivilianPed(PED_TYPE_CIVMALE, MODEL_MALE01);
    ped->bClearRadarBlipOnDeath = true;
    CPedGroups::ms_groups[0].GetMembership().AddFollower(ped);
    const bool inGroup = CPedGroups::IsInPlayersGroup(ped);
    CGangWars::ReleasePedsInAttackWave(true, false);
    Log("L124: ped en el grupo del jugador=%d; tras ReleasePedsInAttackWave(fin de guerra) bClearRadarBlipOnDeath=%d (con el arreglo 1 = conserva el blip; sin el 0)", (int)inGroup, (int)ped->bClearRadarBlipOnDeath);
    CPedGroups::ms_groups[0].GetMembership().RemoveMember(ped);
    delete ped;
}

// L123: la posicion del segundo jugador usa su vehiculo, no el del jugador en foco
void TestSecondPlayer() {
    auto& info1 = CWorld::Players[1];
    if (info1.m_pPed || !g_van) {
        Log("L123: jugador 2 presente o sin vehiculo; se omite");
        return;
    }
    auto* ped2 = new CCivilianPed(PED_TYPE_CIVMALE, MODEL_MALE01);
    ped2->SetPosn(CVector{ 100.0f, 200.0f, 300.0f });
    g_van->SetPosn(CVector{ 700.0f, 800.0f, 900.0f });
    ped2->bInVehicle = true;
    ped2->m_pVehicle = g_van;
    info1.m_pPed = reinterpret_cast<CPlayerPed*>(ped2);
    const auto saveBar = CCarCtrl::bCarsGeneratedAroundCamera;
    CCarCtrl::bCarsGeneratedAroundCamera = false;
    const CVector c = FindPlayerCentreOfWorld(1);
    Log("L123: FindPlayerCentreOfWorld(1) = (%.1f %.1f %.1f); ped2=(100 200 300), vehiculo del ped2=(700 800 900) (con el arreglo, el vehiculo; sin el, el ped)", c.x, c.y, c.z);
    CCarCtrl::bCarsGeneratedAroundCamera = saveBar;
    info1.m_pPed = nullptr;
    ped2->bInVehicle = false;
    ped2->m_pVehicle = nullptr;
    delete ped2;
}

// L61: nodos de peatones distintos que genera GeneratePedCreationCoors (con rand de 15 bits solo salian los 512 primeros de la region)
void TestPedNodes(const CVector& at) {
    std::set<uint32> nodes;
    int ok = 0, maxNode = 0;
    for (int i = 0; i < 1500; i++) {
        CVector out;
        CNodeAddress a1, a2;
        float orient = 0.f;
        if (ThePaths.GeneratePedCreationCoors(at.x, at.y, 0.0f, 400.0f, 0.0f, 400.0f, &out, &a1, &a2, &orient, true, nullptr)) {
            ok++;
            nodes.insert((uint32(a1.m_wAreaId) << 16) | a1.m_wNodeId);
            maxNode = std::max<int>(maxNode, a1.m_wNodeId);
        }
    }
    Log("L61: 1500 llamadas a GeneratePedCreationCoors: aciertos=%d, nodos distintos=%zu, indice de nodo maximo=%d (con rand de 31 bits salen muchos mas nodos y el indice crece)", ok, nodes.size(), maxNode);
}

// L104: flotar con el jetpack con teclado y raton (Q + E = arma siguiente + anterior = GetLookBehindForCar)
void TestJetpackHover(CPlayerPed* player) {
    auto* pad = CPad::GetPad(0);
    const auto savedNew      = pad->NewState;
    const auto savedMouse    = CCamera::m_bUseMouse3rdPerson;
    const bool savedStanding = player->bIsStanding;
    const float savedAim     = player->m_fAimingRotation;
    const float savedBlend   = player->GetPlayerData()->m_fMoveBlendRatio;
    pad->NewState.LeftShoulder2  = 255;
    pad->NewState.RightShoulder2 = 255;
    CCamera::m_bUseMouse3rdPerson = true;
    player->bIsStanding           = false;
    CTaskSimpleJetPack task{ nullptr, 10.0f, 0, nullptr };
    task.ProcessControlInput(player);
    const auto* b = reinterpret_cast<const uint8*>(&task);
    Log("L104: Q+E con raton en 3a persona y jetpack en el aire: m_ThrustStop(+0xD)=%d m_ThrustFwd(+0xE)=%d (con el arreglo 1/0 = flota; sin el 0 = no flota)", (int)b[0xD], (int)b[0xE]);
    pad->NewState                 = savedNew;
    CCamera::m_bUseMouse3rdPerson = savedMouse;
    player->bIsStanding           = savedStanding;
    player->m_fAimingRotation     = savedAim;
    player->GetPlayerData()->m_fMoveBlendRatio = savedBlend;
}

// L121: contador de graffitis en memoria tras cargar la partida (si ya vale lo mismo que el del guardado, el arreglo no cambia nada con guardados sin mods)
void TestTags() {
    int nonZero = 0, total = 0;
    for (const auto& t : CTagManager::ms_tagDesc) {
        total++;
        if (t.Alpha != 0) {
            nonZero++;
        }
    }
    Log("L121: ms_numTags=%d (tras cargar la partida con el arreglo %s); etiquetas con alfa != 0: %d de %d", (int)CTagManager::ms_numTags, SilentPatchOption("TagManagerLoadFix", true) ? "ENCENDIDO" : "apagado", nonZero, total);
}

// L83 y L93: reinicio en Nueva Partida
void TestNewGame() {
    const size_t pk0 = UsedPickups(), cg0 = UsedCarGens();
    CGameLogic::bPenaltyForDeathApplies  = false;
    CGameLogic::bPenaltyForArrestApplies = false;
    SilentPatchNewGame::BeforeReInit();
    Log("L83: tras BeforeReInit: bPenaltyForDeathApplies=%d bPenaltyForArrestApplies=%d (con el arreglo 1/1; sin el 0/0)", (int)CGameLogic::bPenaltyForDeathApplies, (int)CGameLogic::bPenaltyForArrestApplies);
    SilentPatchNewGame::AfterReInit();
    Log("L93: pickups usados %zu -> %zu, generadores usados %zu -> %zu tras AfterReInit (con el arreglo se vuelven a cargar las definiciones de los IPL de texto: aumentan; sin el, igual)", pk0, UsedPickups(), cg0, UsedCarGens());
}
} // namespace

void OnFrame() {
    if (!Enabled()) {
        return;
    }
    static int    stage      = 0;
    static uint32 startFrame = 0;
    auto* player = FindPlayerPed();
    if (!player || !player->GetRwObject() || CTimer::GetFrameCounter() < 200) {
        return;
    }
    if (stage == 0) {
        startFrame = CTimer::GetFrameCounter();
        stage      = 1;
        return;
    }
    if (CTimer::GetFrameCounter() - startFrame < 600) { // unos segundos para que el mundo y la colision de la zona esten cargados
        return;
    }
    if (stage == 1) {
        stage = 2;
        const CVector at = player->GetPosition();
        Log("=== SILENTPATCH PEDS SELFTEST inicio: jugador en (%.1f %.1f %.1f) area=%d", at.x, at.y, at.z, (int)player->GetAreaCode());
        Opciones();
        TestCarGenerators(at);
        CStreaming::RequestModel(MODEL_BURRITO, STREAMING_GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::IsModelLoaded(MODEL_BURRITO)) {
            g_van = new CAutomobile(MODEL_BURRITO, RANDOM_VEHICLE, true);
            g_van->SetPosn(CVector{ 0.0f, 0.0f, 0.0f });
        }
        TestRoadblock();
        TestDoors();
        TestGangWarBlips();
        TestSecondPlayer();
        TestPedNodes(at);
        TestJetpackHover(player);
        TestTags();
        TestNewGame();
        if (g_van) {
            delete g_van; // nunca estuvo en el mundo
            g_van = nullptr;
        }
        Log("=== SILENTPATCH PEDS SELFTEST FIN");
    }
}
} // namespace SilentPatchPedsSelfTest
