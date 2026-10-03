#pragma once
// Live, in-engine comparison for CEntryExit::TransitionFinished (0x4404A0).
// Injected into CGame::Process() exactly like
// reversing/CTaskSimpleUseGun/LiveComparison.hpp. Alternates the installed
// hook so `entry()` executes either the real original machine code (hook
// disabled) or the reconstructed C++ (hook enabled) at the very same
// address, and compares both runs with independent, synthetic fixtures.
//
// SCOPE: only the branches of TransitionFinished that return `false` without
// reaching the world-completion block (streaming a scene, teleporting the
// ped/vehicle, shop/population/script processing, CGame::TidyUpMemory, ...)
// are exercised here. That block only runs once EXIT_ENTER_STATE_2
// transitions to EXIT_ENTER_STATE_3 (TheCamera.GetFading() == false); this
// probe forces GetFading() to always observe `true`, so that transition -
// and everything after it - is intentionally never reached. See README.md
// for the exact list of covered/uncovered branches.
//
// CEntryExitManager::SetAreaCodeForVisibleObjects (called from the
// else-branch's state 0) is a *real*, uninstrumented dependency: it touches
// whatever real entities are currently in the camera frustum and share the
// controlled `CGame::currArea` value, flipping them to AREA_CODE_13. Both
// the original and the candidate call reset `ms_numVisibleEntities` to a
// known baseline first and the probe restores every touched entity's area
// code immediately after each call (the match condition itself proves the
// pre-call value), so no real entity keeps a modified area code once a
// pair finishes.
//
// PHASE 2 (appended below, after phase 1 restores every global it touched):
// by explicit request, exercises the EXIT_ENTER_STATE_3 completion block
// phase 1 deliberately never reaches (scene streaming, ped Teleport,
// CPopulation, CTheScripts::Process, CGame::TidyUpMemory, ...). It requires
// `ped->AsPlayer()`/`m_pEnex` to be valid, so it uses the real player
// instead of a synthetic ped. To keep the real displacement as close to
// zero as possible, the synthetic CEntryExit it builds always uses
// AREA_CODE_NORMAL_WORLD and the player's own current position/heading as
// the "exit" target: Teleport()/SetHeading() land the player back where it
// already was, SetAreaCode(AREA_CODE_NORMAL_WORLD) is a no-op if the player
// was already outside, and `m_pEnex` ends up `nullptr` either way. Runs
// only when the player starts on foot, in the normal world, holding
// nothing and not already inside an interior; otherwise it is skipped and
// reported as such rather than guessed at. Subsystems reached inside the
// block that are not individually restored (CPopulation, CTheScripts,
// CGarages, CWaterLevel, CAudioZones, g_interiorMan, the streaming
// systems, the camera's fade-out animation beyond cancelling it) are left
// to run for real and are documented as real dependencies, not diffed
// bit-exact - see README.md.
#include <fstream>
#include <filesystem>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "EntryExit.h"
#include "EntryExitManager.h"
#include "Entity/Ped/Ped.h"
#include "Camera.h"
#include "Renderer.h"
#include "Pad.h"
#include "Probe.hpp"

namespace EnexLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } } log << r.dump() << '\n'; log.flush(); }
inline int Report(int type, char* msg, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event","assertion"},{"message",msg ? msg : ""}}); ExitProcess(91);
}
inline std::vector<uint8> Bytes(const void* ptr, size_t size) {
    const auto* p = static_cast<const uint8*>(ptr); return {p,p+size};
}

template<class T> struct Storage {
    alignas(T) std::array<uint8,sizeof(T)> bytes{};
    T* get() { return reinterpret_cast<T*>(bytes.data()); }
};

// `CPed::m_pIntelligence` is `protected`; a derived class with no extra data
// members has the exact same layout and may re-expose it, same trick as
// used for other probes that need to wire up a synthetic ped.
struct PedAccess : CPed { using CPed::m_pIntelligence; };

struct Fixture {
    Storage<CEntryExit> enexStorage;
    Storage<CPed> pedStorage;
    Storage<CPedIntelligence> intelStorage;

    Fixture(const CVector& pos, int area, int scenario, int subFlag) {
        auto* enex = enexStorage.get();
        enex->m_vecExitPos = pos;
        enex->m_nArea = eAreaCodes(area);
        enex->m_szName[0] = 0; // HasNameSet() == false: GetEntryExitToDisplayNameOf() always returns null here.
        enex->m_pLink = nullptr;

        const bool foodBranch = scenario < 2; // 0,1 = (bFoodDateFlag||bUnknownPairing||ped->bInVehicle); 2,3,4 = else
        enex->bFoodDateFlag    = foodBranch && subFlag == 0;
        enex->bUnknownPairing  = foodBranch && subFlag == 1;

        auto* access = reinterpret_cast<PedAccess*>(pedStorage.get());
        access->m_pIntelligence = intelStorage.get(); // zeroed: m_aPrimaryTasks all null, read-only in this scope.
        auto* ped = pedStorage.get();
        ped->bInVehicle = foodBranch && subFlag == 2;
    }

    CEntryExit* Enex() { return enexStorage.get(); }
    CPed* Ped() { return pedStorage.get(); }

    auto ReadOnlyStorage() {
        auto bytes = Bytes(enexStorage.get(), sizeof(CEntryExit));
        const auto p = Bytes(pedStorage.get(), sizeof(CPed));
        bytes.insert(bytes.end(), p.begin(), p.end());
        return bytes;
    }
};

inline bool Equal(const json& a, const json& b) {
    if (a.is_object()) { if (!b.is_object() || a.size()!=b.size()) return false; for (auto it=a.begin(); it!=a.end(); ++it) if (!b.contains(it.key()) || !Equal(it.value(), b[it.key()])) return false; return true; }
    if (a.is_array()) { if (!b.is_array() || a.size()!=b.size()) return false; for (size_t i=0;i<a.size();++i) if (!Equal(a[i],b[i])) return false; return true; }
    return a == b;
}

inline int StateOf(int scenario) {
    switch (scenario) {
    case 0: return EXIT_ENTER_STATE_0;
    case 1: return EXIT_ENTER_STATE_2;
    case 2: return EXIT_ENTER_STATE_0;
    case 3: return EXIT_ENTER_STATE_1;
    default: return EXIT_ENTER_STATE_2;
    }
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir/"enextransition-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir/"enextransition-runtime.jsonl"); Write({{"event","waiting_for_player"}}); }
    auto* player = FindPlayerPed();
    if (!player || !player->GetPlayerData() || !player->m_matrix || CCutsceneMgr::IsRunning()) return;
    done = true;
    auto report = _CrtSetReportHook(Report);

    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){ if (c.Name()=="CEntryExit") for (auto& h : c.Items()) if (h->Name()=="TransitionFinished") hook = h.get(); });
    if (!hook) { Write({{"event","error"},{"reason","missing hook"}}); _CrtSetReportHook(report); return; }

    const bool enabled = hook->Hooked(), locked = hook->Locked();
    hook->LockState(false);

    // Real globals this probe temporarily overwrites; restored verbatim at the end.
    const auto realState           = CEntryExitManager::ms_exitEnterState;
    const eAreaCodes realArea      = CGame::currArea;
    const auto realSpawn           = CEntryExit::ms_spawnPoint;
    const auto realWarping         = CEntryExit::ms_bWarping;
    const auto realLoadingPriority = CRenderer::m_loadingPriority;
    const auto realFading          = TheCamera.m_bFading;
    const auto realNumVisible      = CEntryExitManager::ms_numVisibleEntities;
    const eAreaCodes realOldArea   = (eAreaCodes)CEntryExitManager::ms_oldAreaCode;
    std::array<CEntity*,32> realVisibleList{};
    for (int k=0;k<32;++k) realVisibleList[k] = CEntryExitManager::ms_visibleEntityList[k];

    Write({{"event","player_ready"},{"independent_inputs",true}});

    bool passed = true; int pairs = 0;

    constexpr int ScenarioCount=5, SubFlagCount=3, WarpCount=2, AreaCount=8, PosCount=4;
    constexpr int Total = ScenarioCount*SubFlagCount*WarpCount*AreaCount*PosCount;
    const CVector positions[PosCount] = { {0.f,0.f,20.f}, {200.f,-100.f,10.f}, {-500.f,300.f,5.f}, {1000.f,1000.f,50.f} };

    auto entry = reinterpret_cast<bool(__thiscall*)(CEntryExit*,CPed*)>(0x4404A0);

    for (int i=0; i<Total && passed; ++i) {
        int idx=i;
        const int posIdx    = idx % PosCount;    idx/=PosCount;
        const int areaIdx    = idx % AreaCount;   idx/=AreaCount;
        const int warpIdx    = idx % WarpCount;   idx/=WarpCount;
        const int subFlag    = idx % SubFlagCount;idx/=SubFlagCount;
        const int scenario   = idx % ScenarioCount;

        const bool warping = warpIdx != 0;
        const int area = areaIdx + 1; // never AREA_CODE_NORMAL_WORLD/NONE
        const auto& pos = positions[posIdx];
        const int state = StateOf(scenario);

        Fixture a(pos, area, scenario, subFlag), b(pos, area, scenario, subFlag);
        const bool independent = a.Enex()!=b.Enex() && a.Ped()!=b.Ped();
        const auto aReadOnly = a.ReadOnlyStorage(), bReadOnly = b.ReadOnlyStorage();

        const auto RunOne = [&](Fixture& f) {
            CEntryExitManager::ms_exitEnterState = state;
            CGame::currArea = AREA_CODE_NORMAL_WORLD;
            CEntryExit::ms_spawnPoint = f.Enex();
            CEntryExit::ms_bWarping = warping;
            CRenderer::m_loadingPriority = true;
            TheCamera.m_bFading = true;
            CEntryExitManager::ms_numVisibleEntities = 0; // Known baseline before the (possibly unreached) real call.
            const auto start = EnexProbe::calls;
            const bool ret = entry(f.Enex(), f.Ped());
            const auto calls = EnexProbe::calls - start;
            const int32 numVisible = CEntryExitManager::ms_numVisibleEntities;
            // Immediately undo the only real-world mutation this scope can reach: every
            // entity SetAreaCodeForVisibleObjects touched had, by its own match condition,
            // AreaCode == AREA_CODE_NORMAL_WORLD right before the call.
            for (int32 k=0;k<numVisible;++k) {
                CEntryExitManager::ms_visibleEntityList[k]->SetAreaCode(AREA_CODE_NORMAL_WORLD);
            }
            CEntryExitManager::ms_numVisibleEntities = 0;
            return json{
                {"return", ret},
                {"state_after", CEntryExitManager::ms_exitEnterState},
                {"area_after", (int32)CGame::currArea},
                {"loading_priority_after", CRenderer::m_loadingPriority},
                {"num_visible", numVisible},
                {"calls", calls},
            };
        };

        hook->State(false);
        const auto originalResult = RunOne(a);
        const auto originalCalls = originalResult["calls"].get<unsigned>();

        hook->State(true);
        const auto candidateResult = RunOne(b);
        const auto candidateCalls = candidateResult["calls"].get<unsigned>();

        const bool aUnchanged = a.ReadOnlyStorage()==aReadOnly;
        const bool bUnchanged = b.ReadOnlyStorage()==bReadOnly;
        // "calls" is checked separately below (0 for the original machine code, 1 for the
        // candidate C++); it is never expected to match and must not enter the equality check.
        auto originalComparable = originalResult, candidateComparable = candidateResult;
        originalComparable.erase("calls"); candidateComparable.erase("calls");
        const bool equal = Equal(originalComparable, candidateComparable);

        passed = equal && originalCalls==0 && candidateCalls==1 && independent && aUnchanged && bUnchanged;
        Write({{"event","comparison"},{"pair",i},{"scenario",scenario},{"state",state},{"sub_flag",subFlag},{"warping",warping},{"area",area},
               {"original",originalResult},{"candidate",candidateResult},{"equal",equal},
               {"independent_inputs",independent},{"inputs_unchanged",aUnchanged&&bUnchanged},
               {"original_candidate_calls",originalCalls},{"candidate_calls",candidateCalls}});
        ++pairs;
    }

    CEntryExitManager::ms_exitEnterState = realState;
    CGame::currArea = realArea;
    CEntryExit::ms_spawnPoint = realSpawn;
    CEntryExit::ms_bWarping = realWarping;
    CRenderer::m_loadingPriority = realLoadingPriority;
    TheCamera.m_bFading = realFading;
    CEntryExitManager::ms_numVisibleEntities = realNumVisible;
    CEntryExitManager::ms_oldAreaCode = realOldArea;
    for (int k=0;k<32;++k) CEntryExitManager::ms_visibleEntityList[k] = realVisibleList[k];

    bool listRestored = true;
    for (int k=0;k<32;++k) listRestored = listRestored && CEntryExitManager::ms_visibleEntityList[k]==realVisibleList[k];
    const bool phase1Restored = CEntryExitManager::ms_exitEnterState==realState && (eAreaCodes)CGame::currArea==realArea
        && CEntryExit::ms_spawnPoint==realSpawn && CEntryExit::ms_bWarping==realWarping
        && CRenderer::m_loadingPriority==realLoadingPriority && TheCamera.m_bFading==realFading
        && CEntryExitManager::ms_numVisibleEntities==realNumVisible && (eAreaCodes)CEntryExitManager::ms_oldAreaCode==realOldArea
        && listRestored;
    Write({{"event","phase1_complete"},{"pairs",pairs},{"state_restored",phase1Restored}});

    // ---- Phase 2: real execution of the EXIT_ENTER_STATE_3 completion block ----
    bool heavyAttempted = false, heavyPassed = true; int heavyPairs = 0;
    if (passed && phase1Restored) {
        const auto playerPos          = player->GetPosition();
        const auto playerHeadingDeg   = RadiansToDegrees(player->m_fCurrentRotation);
        const auto realAiming         = player->m_fAimingRotation;
        const auto realAreaCode       = player->GetAreaCode();
        const auto realEnex           = player->m_pEnex;
        const auto realCanExitCar     = player->bCanExitCar;
        const auto realPadTransition  = CPad::GetPad()->bPlayerOnInteriorTransition;
        const auto realStackPosn      = CEntryExitManager::ms_entryExitStackPosn;
        std::array<CEntryExit*,4> realStack{};
        for (int k=0;k<4;++k) realStack[k] = CEntryExitManager::ms_entryExitStack[k];

        // Only run against a clean, on-foot, already-outside, nothing-held player;
        // otherwise this phase is skipped and reported, never guessed at.
        const bool safeToRun = realAreaCode==AREA_CODE_NORMAL_WORLD && realEnex==nullptr
            && !player->bInVehicle && !player->GetEntityThatThisPedIsHolding();
        Write({{"event","heavy_precondition"},{"safe_to_run",safeToRun},{"area_code",(int32)realAreaCode},
               {"has_enex",realEnex!=nullptr},{"in_vehicle",(bool)player->bInVehicle}});

        if (safeToRun) {
            heavyAttempted = true;
            const auto RunHeavy = [&](bool foodDateFlag) {
                Storage<CEntryExit> spawnStorage;
                auto* spawn = spawnStorage.get();
                spawn->m_vecExitPos = playerPos;
                spawn->m_fExitAngle = playerHeadingDeg;
                spawn->m_nArea = AREA_CODE_NORMAL_WORLD;
                spawn->m_szName[0] = 0;
                spawn->m_pLink = nullptr;
                spawn->m_nSkyColor = 0;
                spawn->bUnknownInterior = false;
                spawn->bUsedRewardEntrance = false;
                spawn->bRewardInterior = false;
                spawn->bAcceptNpcGroup = false;
                spawn->bDeleteEnex = false;
                spawn->bFoodDateFlag = foodDateFlag;
                spawn->bUnknownPairing = false;

                CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_3;
                CGame::currArea = AREA_CODE_NORMAL_WORLD;
                CEntryExit::ms_spawnPoint = spawn;
                CEntryExit::ms_bWarping = false;
                CEntryExitManager::ms_entryExitStackPosn = 0;
                TheCamera.m_bFading = true; // Irrelevant here: state is already 3, GetFading() is not read on this path.
                const auto start = EnexProbe::calls;
                const bool ret = entry(spawn, player);
                const auto calls = EnexProbe::calls - start;

                const auto stackPosnAfter = CEntryExitManager::ms_entryExitStackPosn;
                const auto areaAfter = player->GetAreaCode();
                const auto enexAfter = player->m_pEnex;
                const auto canExitCarAfter = player->bCanExitCar;
                const auto padTransitionAfter = CPad::GetPad()->bPlayerOnInteriorTransition;
                const auto posAfter = player->GetPosition();

                // Undo everything this probe can reach deterministically before the next
                // call (or before handing control back to the real game loop). Position,
                // heading and area are expected to already equal the pre-call values by
                // construction (see the comment block above); Teleport()/SetHeading() are
                // still used - never a raw memory write - to go through the engine's own
                // repositioning path.
                player->Teleport(playerPos, false);
                player->m_fCurrentRotation = DegreesToRadians(playerHeadingDeg);
                player->m_fAimingRotation = realAiming;
                player->SetHeading(player->m_fCurrentRotation);
                player->SetAreaCode(realAreaCode);
                player->m_pEnex = realEnex;
                player->bCanExitCar = realCanExitCar;
                CPad::GetPad()->bPlayerOnInteriorTransition = realPadTransition;
                CEntryExitManager::ms_entryExitStackPosn = realStackPosn;
                for (int k=0;k<4;++k) CEntryExitManager::ms_entryExitStack[k] = realStack[k];
                TheCamera.m_bFading = false; // Cancel the real Fade(1.f, FADE_OUT) this call just started.

                return json{
                    {"return", ret},
                    {"area_after", (int32)areaAfter},
                    {"had_enex_after", enexAfter!=nullptr},
                    {"can_exit_car_after", canExitCarAfter},
                    {"pad_transition_after", padTransitionAfter},
                    {"stack_posn_after", stackPosnAfter},
                    {"position_delta", (posAfter-playerPos).Magnitude()},
                    {"calls", calls},
                };
            };

            // Sub-case 1 (light): bFoodDateFlag==true reaches the shared state-3 prefix
            // (SetAreaCode, m_pEnex, AddEntryExitToStack, bCanExitCar, CPad flag,
            // CClothes::RebuildPlayerIfNeeded) and returns early, before any streaming,
            // teleport, population or script processing. Repeated: it is cheap and safe.
            constexpr int LightPairs = 8;
            for (int i=0; i<LightPairs && heavyPassed; ++i) {
                hook->State(false); const auto orig = RunHeavy(true);
                hook->State(true);  const auto cand = RunHeavy(true);
                auto oc=orig, cc=cand; const auto origCalls=oc["calls"].get<unsigned>(), candCalls=cc["calls"].get<unsigned>();
                oc.erase("calls"); cc.erase("calls");
                const bool eq = Equal(oc,cc);
                heavyPassed = eq && origCalls==0 && candCalls==1;
                Write({{"event","heavy_comparison"},{"sub_case","light_early_return"},{"pair",i},
                       {"original",orig},{"candidate",cand},{"equal",eq},
                       {"original_calls",origCalls},{"candidate_calls",candCalls}});
                ++heavyPairs;
            }

            // Sub-case 2 (full): bFoodDateFlag==false runs the entire completion block -
            // scene streaming, CPopulation::RemoveAllRandomPeds/GenerateAmbientPeds/
            // ManageAllPopulation, CTheScripts::ClearSpaceForMissionEntity/Process,
            // CGarages, CWaterLevel, CAudioZones, g_interiorMan, CGame::TidyUpMemory -
            // for real, exactly once per side, given how invasive repeating it would be.
            if (heavyPassed) {
                hook->State(false); const auto orig = RunHeavy(false);
                hook->State(true);  const auto cand = RunHeavy(false);
                auto oc=orig, cc=cand; const auto origCalls=oc["calls"].get<unsigned>(), candCalls=cc["calls"].get<unsigned>();
                oc.erase("calls"); cc.erase("calls");
                const bool eq = Equal(oc,cc);
                heavyPassed = eq && origCalls==0 && candCalls==1;
                Write({{"event","heavy_comparison"},{"sub_case","full_completion"},{"pair",0},
                       {"original",orig},{"candidate",cand},{"equal",eq},
                       {"original_calls",origCalls},{"candidate_calls",candCalls}});
                ++heavyPairs;
            }
        }

        const bool playerRestored = player->GetPosition()==playerPos && player->GetAreaCode()==realAreaCode
            && player->m_pEnex==realEnex && (bool)player->bInVehicle==false
            && CEntryExitManager::ms_entryExitStackPosn==realStackPosn;
        heavyPassed = heavyPassed && (!heavyAttempted || playerRestored);
        Write({{"event","heavy_complete"},{"attempted",heavyAttempted},{"pairs",heavyPairs},{"player_restored",playerRestored}});
    }

    hook->State(enabled); hook->LockState(locked);
    const bool restored = phase1Restored && hook->Hooked()==enabled && hook->Locked()==locked;
    const bool allPassed = passed && restored && heavyPassed;
    Write({{"event", allPassed ? "complete":"error"},{"pairs",pairs},{"heavy_pairs",heavyPairs},
           {"heavy_attempted",heavyAttempted},{"state_restored",restored}});
    _CrtSetReportHook(report);
}
}
