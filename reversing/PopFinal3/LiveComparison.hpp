#pragma once
// Banco dentro del motor (flujo viejo, estilo Small2/PadMouse1/WorldFinal1): CPopulation (55 de las 64 funciones pendientes).
// Original (hook de la funcion desactivado: codigo del EXE) frente a candidato (hook activo) sobre objetos FALSOS: pools de
// peatones/objetos/dummies falsos (se intercambia el puntero global solo durante cada ejecucion y se restaura), model infos falsos
// para los IDs 0..319 y dos reservados (19990 vehiculo, 19991 objeto), peatones/vehiculos/objetos/dummies en buferes propios con
// vtable falsa (hueco 0 = destructor trazado, hueco 4 = SetIsStatic). Los colaboradores con efectos reales (CWorld::Add/Remove,
// rand 0x821B1E + CGeneral::GetRandomNumber, FindPlayer*, streaming, camara, pools de modelos...) se doblan con EntryPatch en la
// direccion del EXE y en el cuerpo de la DLL (todos los Body() se calculan ANTES de crear ningun parche) y registran trazas con
// argumentos solo en el hilo del banco. Todo el estado global tocado se guarda y se restaura (state_restored).
//
// FUERA del banco (9): AddPed, AddDeadPedInFrontOfCar, AddPedInCar, AddPedAtAttractor, PlaceCouple, CreateWaitingCoppers,
// GeneratePedsAtAttractors, PopulateInterior, TestSafeForRealObject (ver README.md: construyen peatones/vehiculos/eventos/tareas
// reales con constructores del EXE y de la DLL, matrices y listas de sectores reales).
#include "Glass.h"
#include "PopCases2.hpp"

namespace PopFinal3Live {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn, 55> fns{{
    {"CPopulation","AddExistingPedInCar",0x611560,0,-1,64},
    {"CPopulation","ArePedStatsCompatible",0x610F50,0,-1,1024},
    {"CPopulation","CanJeerAtStripper",0x6117B0,0,-1,128},
    {"CPopulation","CanSolicitPlayerInCar",0x611790,0,-1,128},
    {"CPopulation","CanSolicitPlayerOnFoot",0x611780,0,-1,64},
    {"CPopulation","ChooseCivilianCoupleOccupations",0x613180,0,-1,512},
    {"CPopulation","ChooseCivilianOccupation",0x612F90,0,-1,1024},
    {"CPopulation","ChooseCivilianOccupationForVehicle",0x613260,0,-1,512},
    {"CPopulation","ChooseGangOccupation",0x611550,0,-1,64},
    {"CPopulation","ChoosePolicePedOccupation",0x610F40,0,-1,64},
    {"CPopulation","ConvertAllObjectsToDummyObjects",0x616420,0,-1,128},
    {"CPopulation","ConvertToDummyObject",0x614670,0,-1,256},
    {"CPopulation","ConvertToRealObject",0x614580,0,-1,256},
    {"CPopulation","DealWithZoneChange",0x6116B0,0,-1,64},
    {"CPopulation","DoesCarGroupHaveModelId",0x406F50,0,-1,256},
    {"CPopulation","FindCarMultiplierMotorway",0x611B60,0,-1,64},
    {"CPopulation","FindDistanceToNearestPedOfType",0x6143E0,0,-1,256},
    {"CPopulation","FindDummyDistForModel",0x610ED0,0,-1,256},
    {"CPopulation","FindNumberOfPedsWeCanPlaceOnBenches",0x612240,0,-1,512},
    {"CPopulation","FindPedDensityMultiplierCullZone",0x610F00,0,-1,64},
    {"CPopulation","FindPedMultiplierMotorway",0x611B80,0,-1,64},
    {"CPopulation","FindPedRaceFromName",0x5B6D40,0,-1,256},
    {"CPopulation","FindSpecificDriverModelForCar_ToUse",0x611900,0,-1,256},
    {"CPopulation","GeneratePedsAtStartOfGame",0x615C90,0,-1,128},
    {"CPopulation","Initialise",0x610E10,0,-1,128},
    {"CPopulation","IsCorrectTimeOfDayForEffect",0x611B20,0,-1,256},
    {"CPopulation","IsFemale",0x611490,0,-1,128},
    {"CPopulation","IsMale",0x611470,0,-1,128},
    {"CPopulation","IsSecurityGuard",0x6114B0,0,-1,64},
    {"CPopulation","IsSkateable",0x6114C0,0,-1,256},
    {"CPopulation","IsSunbather-ModelID",0x611760,0,-1,128},
    {"CPopulation","LoadCarGroups",0x5BD1A0,0,-1,64},
    {"CPopulation","LoadPedGroups",0x5BCFE0,0,-1,64},
    {"CPopulation","LoadSpecificDriverModelsForCar",0x6117F0,0,-1,128},
    {"CPopulation","ManageAllPopulation",0x6160A0,0,-1,256},
    {"CPopulation","ManageDummy",0x616000,0,-1,256},
    {"CPopulation","ManagePed",0x611FC0,0,-1,1024},
    {"CPopulation","ManagePopulation",0x616190,0,-1,256},
    {"CPopulation","MoveCarsAndPedsOutOfAbandonedZones",0x6116A0,0,-1,64},
    {"CPopulation","PedCreationDistMultiplier",0x6116C0,0,-1,128},
    {"CPopulation","PedMICanBeCreatedAtAttractor",0x6110C0,0,-1,128},
    {"CPopulation","PedMICanBeCreatedAtThisAttractor",0x6110E0,0,-1,1024},
    {"CPopulation","PedMICanBeCreatedInInterior",0x611450,0,-1,128},
    {"CPopulation","PickGangCar",0x614490,0,-1,128},
    {"CPopulation","PickRiotRoadBlockCar",0x6144B0,0,-1,256},
    {"CPopulation","PlaceGangMembers",0x6117D0,0,-1,128},
    {"CPopulation","PlaceMallPedsAsStationaryGroup",0x613CD0,0,-1,128},
    {"CPopulation","RemoveAllRandomPeds",0x6122C0,0,-1,128},
    {"CPopulation","RemovePed",0x610F20,0,-1,128},
    {"CPopulation","RemovePedsIfThePoolGetsFull",0x616300,0,-1,256},
    {"CPopulation","RemoveSpecificDriverModelsForCar",0x6119D0,0,-1,128},
    {"CPopulation","Shutdown",0x610EC0,0,-1,64},
    {"CPopulation","TestRoomForDummyObject",0x612320,0,-1,256},
    {"CPopulation","Update",0x616650,0,-1,512},
    {"CPopulation","UpdatePedCount",0x611570,0,-1,256},
}};

// ---------- colaboradores ----------
enum K { K_RAND, K_WADD, K_WREM, K_FEWER, K_FVEH, K_CENTRE, K_FPED, K_ISPLAYER, K_CANDEL, K_ONSCR, K_FRENZY, K_GANGWAR, K_CLUMP, K_REQ, K_CAB, K_SETDEL, K_SETTXD,
         K_GANGMODEL, K_PLACE, K_PVL, K_SKATE, K_ACCEPT, K_MOBJ, K_MDUM, K_MPED, K_CREAL, K_CDUM, K_TROOM, K_TSAFE, K_PICK, K_FOK, K_BOUND, K_GETHOLD,
         K_ADDTOPOP, K_GENATTR, K_LOADPED, K_LOADCAR, K_MANAGEPOP, K_REMOVEFULL, K_GENSTART, K_UPDOBJ, K_CREATEOBJ, K_RESOLVE, K_SETREL, K_ISGLASS,
         K_ADDMOVING, K_CHOOSECIV, K_COPCAR, K_N };
struct Col { const char* n; uintptr_t exe; uintptr_t dll; void* se; void* sd; };
inline std::array<Col, K_N> cols{};
template<class F> inline void* P_(F f) { return reinterpret_cast<void*>(f); }

struct Run { int f; json (*run)(Rng&); std::vector<int> uses; };
inline std::vector<Run> runs;

inline void BuildCollabs() {
    auto S = [&](int k, const char* n, uintptr_t exe, uintptr_t dll, void* se, void* sd = nullptr) { cols[size_t(k)] = { n, exe, dll, se, sd ? sd : se }; };
    S(K_RAND, "rand", 0x821B1E, Body(&CGeneral::GetRandomNumber), P_(&S_RandExe), P_(&S_RandDll));
    S(K_WADD, "world_add", 0x563220, Body(&CWorld::Add), P_(&S_WorldAdd));
    S(K_WREM, "world_remove", 0x563280, Body(&CWorld::Remove), P_(&S_WorldRemove));
    S(K_FEWER, "fewer_peds", 0x72DD90, Body(&CCullZones::FewerPeds), P_(&S_Fewer));
    S(K_FVEH, "find_vehicle", 0x56E0D0, Body(&FindPlayerVehicle), P_(&S_FindVeh));
    S(K_CENTRE, "find_centre", 0x56E250, Body(&FindPlayerCentreOfWorld), P_(&S_Centre));
    S(K_FPED, "find_ped", 0x56E210, Body(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)), P_(&S_FindPed));
    S(K_ISPLAYER, "is_player", 0x5DF8F0, Body(&CPed::IsPlayer), P_(&S_IsPlayer));
    S(K_CANDEL, "can_be_deleted", 0x5DF100, Body(&CPed::CanBeDeleted), P_(&S_CanDelete));
    S(K_ONSCR, "on_screen", 0x534540, Body(&CEntity::GetIsOnScreen), P_(&S_OnScreen));
    S(K_FRENZY, "frenzy", 0x43D1F0, Body(&CDarkel::FrenzyOnGoing), P_(&S_Frenzy));
    S(K_GANGWAR, "gangwar", 0x443AC0, Body(&CGangWars::GangWarFightingGoingOn), P_(&S_GangWar));
    S(K_CLUMP, "clump_alpha", 0x732B20, Body(&CVisibilityPlugins::GetClumpAlpha), P_(&S_ClumpAlpha));
    S(K_REQ, "request_model", 0x4087E0, Body(&CStreaming::RequestModel), P_(&S_RequestModel));
    S(K_CAB, "cab_model", 0x407D50, Body(&CStreaming::GetDefaultCabDriverModel), P_(&S_Cab));
    S(K_SETDEL, "set_deletable", 0x409C10, Body(&CStreaming::SetModelIsDeletable), P_(&S_SetDelExe), P_(&S_SetDelDll));
    S(K_SETTXD, "set_txd_deletable", 0x409C70, Body(&CStreaming::SetModelTxdIsDeletable), P_(&S_SetTxd));
    S(K_GANGMODEL, "gang_model", 0x5DE5A0, Body(&CGangs::ChooseGangPedModel), P_(&S_GangModel));
    S(K_PLACE, "place_group", 0x5FD810, Body(&CPedGroupPlacer::PlaceGroup), P_(&S_PlaceGroup));
    S(K_PVL, "pvl", 0x5674E0, Body(static_cast<bool(*)(const CVector&, float, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*)>(&CWorld::ProcessVerticalLine)), P_(&S_Pvl));
    S(K_SKATE, "skateable", 0x55E7D0, Body(&SurfaceInfos_c::IsSkateable), P_(&S_Skate));
    S(K_ACCEPT, "acceptable", 0x610720, Body(&CPopCycle::PedIsAcceptableInCurrentZone), P_(&S_Acceptable));
    S(K_MOBJ, "manage_object", 0x615DC0, Body(&CPopulation::ManageObject), P_(&S_ManageObject));
    S(K_MDUM, "manage_dummy", 0x616000, Body(&CPopulation::ManageDummy), P_(&S_ManageDummy));
    S(K_MPED, "manage_ped", 0x611FC0, Body(&CPopulation::ManagePed), P_(&S_ManagePed));
    S(K_CREAL, "convert_real", 0x614580, Body(&CPopulation::ConvertToRealObject), P_(&S_ConvReal));
    S(K_CDUM, "convert_dummy", 0x614670, Body(&CPopulation::ConvertToDummyObject), P_(&S_ConvDummy));
    S(K_TROOM, "test_room", 0x612320, Body(&CPopulation::TestRoomForDummyObject), P_(&S_TestRoom));
    S(K_TSAFE, "test_safe", 0x6123A0, Body(&CPopulation::TestSafeForRealObject), P_(&S_TestSafe));
    S(K_PICK, "pick_random_car", 0x611C50, Body(&CLoadedCarGroup::PickRandomCar), P_(&S_PickCar));
    S(K_FOK, "find_kinda", 0x568B80, Body(&CWorld::FindObjectsKindaColliding), P_(&S_FindObjKinda));
    S(K_BOUND, "bound_centre", 0x534250, Body(static_cast<CVector(CEntity::*)() const>(&CEntity::GetBoundCentre)), P_(&S_BoundCentre));
    S(K_GETHOLD, "get_task_hold", 0x600FF0, Body(&CPedIntelligence::GetTaskHold), P_(&S_TaskHold));
    S(K_ADDTOPOP, "add_to_population", 0x614720, Body(&CPopulation::AddToPopulation), P_(&S_AddToPop));
    S(K_GENATTR, "gen_attractors", 0x615970, Body(&CPopulation::GeneratePedsAtAttractors), P_(&S_GenAttr));
    S(K_LOADPED, "load_ped_groups", 0x5BCFE0, Body(&CPopulation::LoadPedGroups), P_(&S_LoadPed));
    S(K_LOADCAR, "load_car_groups", 0x5BD1A0, Body(&CPopulation::LoadCarGroups), P_(&S_LoadCar));
    S(K_MANAGEPOP, "manage_population", 0x616190, Body(&CPopulation::ManagePopulation), P_(&S_ManagePop));
    S(K_REMOVEFULL, "remove_full", 0x616300, Body(&CPopulation::RemovePedsIfThePoolGetsFull), P_(&S_RemoveFull));
    S(K_GENSTART, "gen_start", 0x615C90, Body(&CPopulation::GeneratePedsAtStartOfGame), P_(&S_GenStart));
    S(K_UPDOBJ, "update_from_object", 0x59EB70, Body(&CDummyObject::UpdateFromObject), P_(&S_UpdFromObj));
    S(K_CREATEOBJ, "create_object", 0x59EAC0, Body(&CDummyObject::CreateObject), P_(&S_CreateObj));
    S(K_RESOLVE, "resolve_references", 0x571A40, Body(&CEntity::ResolveReferences), P_(&S_Resolve));
    S(K_SETREL, "set_related_dummy", 0x59F160, Body(&CObject::SetRelatedDummy), P_(&S_SetRelated));
    S(K_ISGLASS, "is_object_glass", 0x46A760, Body(&CGlass::IsObjectGlass), P_(&S_IsGlass));
    S(K_ADDMOVING, "add_to_moving_list", 0x542800, Body(&CPhysical::AddToMovingList), P_(&S_AddMoving));
    S(K_CHOOSECIV, "choose_civilian", 0x612F90, Body(&CPopulation::ChooseCivilianOccupation), P_(&S_ChooseCiv));
    S(K_COPCAR, "default_cop_car", 0x407C50, Body(&CStreaming::GetDefaultCopCarModel), P_(&S_CopCar));
}

inline void BuildRuns() {
    auto A = [&](const char* name, json (*fn)(Rng&), std::vector<int> uses) {
        for (size_t i = 0; i < fns.size(); ++i) if (!std::strcmp(fns[i].name, name)) { runs.push_back({ int(i), fn, std::move(uses) }); return; }
    };
    A("AddExistingPedInCar", [](Rng& r) { return CaseTrivial(r, 0x611560, 5); }, {});
    A("ArePedStatsCompatible", CaseArePedStats, {});
    A("CanJeerAtStripper", [](Rng& r) { return CaseModelBool(r, 0x6117B0); }, {});
    A("CanSolicitPlayerInCar", [](Rng& r) { return CaseModelBool(r, 0x611790); }, {});
    A("CanSolicitPlayerOnFoot", [](Rng& r) { return CaseTrivial(r, 0x611780, 3); }, {});
    A("ChooseCivilianCoupleOccupations", CaseCouple, { K_ACCEPT, K_RAND });
    A("ChooseCivilianOccupation", CaseChooseCiv, { K_ACCEPT });
    A("ChooseCivilianOccupationForVehicle", CaseChooseForVeh, { K_ACCEPT });
    A("ChooseGangOccupation", CaseGangOcc, { K_GANGMODEL });
    A("ChoosePolicePedOccupation", [](Rng& r) { return CaseTrivial(r, 0x610F40, 0); }, {});
    A("ConvertAllObjectsToDummyObjects", CaseConvertAll, { K_CDUM });
    A("ConvertToDummyObject", CaseConvDummy, { K_TROOM, K_UPDOBJ, K_WADD, K_WREM });
    A("ConvertToRealObject", CaseConvReal, { K_TSAFE, K_CREATEOBJ, K_WADD, K_WREM, K_RESOLVE, K_SETREL, K_ISGLASS, K_ADDMOVING });
    A("DealWithZoneChange", [](Rng& r) { return CaseTrivial(r, 0x6116B0, 2); }, {});
    A("DoesCarGroupHaveModelId", CaseCarGroup, {});
    A("FindCarMultiplierMotorway", [](Rng& r) { return CaseTrivial(r, 0x611B60, 4); }, {});
    A("FindDistanceToNearestPedOfType", CaseDistNearestType, {});
    A("FindDummyDistForModel", CaseDummyDist, {});
    A("FindNumberOfPedsWeCanPlaceOnBenches", CaseBenches, { K_FEWER });
    A("FindPedDensityMultiplierCullZone", CaseCullZone, { K_FEWER });
    A("FindPedMultiplierMotorway", [](Rng& r) { return CaseTrivial(r, 0x611B80, 4); }, {});
    A("FindPedRaceFromName", CaseRace, {});
    A("FindSpecificDriverModelForCar_ToUse", [](Rng& r) { return CaseDriverModels(r, 0x611900); }, { K_CAB, K_RAND });
    A("GeneratePedsAtStartOfGame", CaseGenStart, { K_ADDTOPOP, K_GENATTR, K_FVEH, K_CENTRE });
    A("Initialise", CaseInitialise, { K_LOADPED, K_LOADCAR });
    A("IsCorrectTimeOfDayForEffect", CaseTimeOfDay, {});
    A("IsFemale", [](Rng& r) { return CaseModelBool(r, 0x611490); }, {});
    A("IsMale", [](Rng& r) { return CaseModelBool(r, 0x611470); }, {});
    A("IsSecurityGuard", CaseSecurityGuard, {});
    A("IsSkateable", CaseSkate, { K_PVL, K_SKATE });
    A("IsSunbather-ModelID", [](Rng& r) { return CaseModelBool(r, 0x611760); }, {});
    A("LoadCarGroups", [](Rng& r) { return CaseLoadGroups(r, false); }, {});
    A("LoadPedGroups", [](Rng& r) { return CaseLoadGroups(r, true); }, {});
    A("LoadSpecificDriverModelsForCar", [](Rng& r) { return CaseDriverModels(r, 0x6117F0); }, { K_REQ, K_CAB });
    A("ManageAllPopulation", CaseManageAll, { K_MOBJ, K_MDUM, K_CENTRE, K_FPED, K_GETHOLD });
    A("ManageDummy", CaseManageDummy, { K_CREAL });
    A("ManagePed", CaseManagePed, { K_ISPLAYER, K_CANDEL, K_FRENZY, K_GANGWAR, K_CLUMP, K_FVEH, K_ONSCR, K_WREM });
    A("ManagePopulation", CaseManagePopulation, { K_MOBJ, K_MDUM, K_MPED, K_CENTRE });
    A("MoveCarsAndPedsOutOfAbandonedZones", [](Rng& r) { return CaseTrivial(r, 0x6116A0, 1); }, {});
    A("PedCreationDistMultiplier", CaseDistMult, { K_FVEH });
    A("PedMICanBeCreatedAtAttractor", [](Rng& r) { return CaseModelBool(r, 0x6110C0); }, {});
    A("PedMICanBeCreatedAtThisAttractor", CaseThisAttractor, {});
    A("PedMICanBeCreatedInInterior", [](Rng& r) { return CaseModelBool(r, 0x611450); }, {});
    A("PickGangCar", CasePickGang, { K_PICK });
    A("PickRiotRoadBlockCar", CasePickRiot, { K_PICK, K_RAND, K_COPCAR });
    A("PlaceGangMembers", CasePlaceGang, { K_PLACE });
    A("PlaceMallPedsAsStationaryGroup", CaseMall, { K_CHOOSECIV, K_PLACE, K_RAND });
    A("RemoveAllRandomPeds", CaseRemoveAllRandom, { K_CANDEL, K_WREM });
    A("RemovePed", CaseRemovePed, { K_WREM });
    A("RemovePedsIfThePoolGetsFull", CaseRemoveIfFull, { K_CANDEL, K_WREM });
    A("RemoveSpecificDriverModelsForCar", [](Rng& r) { return CaseDriverModels(r, 0x6119D0); }, { K_SETDEL, K_SETTXD, K_CAB });
    A("Shutdown", [](Rng& r) { return CaseTrivial(r, 0x610EC0, 1); }, {});
    A("TestRoomForDummyObject", CaseTestRoom, { K_FOK, K_BOUND });
    A("Update", CaseUpdate, { K_MANAGEPOP, K_REMOVEFULL, K_GENSTART, K_ADDTOPOP, K_GENATTR, K_FVEH, K_CENTRE });
    A("UpdatePedCount", CaseUpdatePedCount, {});
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "popfinal3-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "popfinal3-runtime.jsonl"); Write({ { "event", "waiting_for_player" } }); ReversingProbe::InstallFailureCapture("popfinal3"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir / "popfinal3-only.txt"); if (f) std::getline(f, only); }

    std::array<ReversibleHooks::ReversibleHook::Base*, fns.size()> hooks{};
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ { "event", "error" }, { "reason", "hook not found" }, { "function", std::string(fns[f].cls) + "::" + fns[f].name } }); return; }

    // cuerpos de la DLL de los colaboradores (TODOS antes de crear ningun EntryPatch)
    BuildCollabs();
    { std::set<uintptr_t> all; bool bad = false; json miss = json::array();
      for (auto& c : cols) { if (!c.dll) { bad = true; miss.push_back(c.n); } else if (!all.insert(c.dll).second) { bad = true; miss.push_back(std::string(c.n) + "(repetido)"); } }
      if (bad) { Write({ { "event", "error" }, { "reason", "DLL body not found or overlapping" }, { "which", miss } }); return; } }
    BuildRuns();

    std::array<bool, fns.size()> enabled{}, locked{};
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    // vtables falsas y nombres de buferes
    for (auto& v : entVt) v = P_(&V_Trap); entVt[0] = P_(&V_Dtor); entVt[4] = P_(&V_SetStatic);
    for (auto& v : miVt) v = P_(&V_Trap); miVt[1] = P_(&V_AsAtomic);
    for (auto& v : taskVt) v = P_(&V_Trap); taskVt[0] = P_(&V_Dtor); taskVt[4] = P_(&V_TaskType);
    for (auto& c : clumpBuf) { std::memset(c.data(), 0, c.size()); c[0] = 2; }          // tipo rpCLUMP
    if (named.empty()) {
        Nm(pedStore.data(), pedStore.size(), "ped"); Nm(objStore.data(), objStore.size(), "obj"); Nm(dumStore.data(), dumStore.size(), "dummy");
        Nm(vehRaw.data(), vehRaw.size(), "veh"); Nm(taskBuf.data(), taskBuf.size(), "task"); Nm(clumpBuf.data(), sizeof(clumpBuf), "clump");
        Nm(reinterpret_cast<void*>(0xC0EA28), 0x1CC, "gangcars"); Nm(reinterpret_cast<void*>(0xC0E9C8), 0x60, "carsgrp");
        Nm(reinterpret_cast<void*>(0xB6F028), 0x100, "camera");
    }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk = true;
    { json lay = json::object();
      alignas(16) static std::array<uint8, 0x900> cb{};
      auto Chk = [&](const char* n, uint32 got, uint32 want) { lay[n] = got; if (got != want) { layoutOk = false; lay[std::string(n) + "_expected"] = want; } };
      auto Zero = [&] { std::memset(cb.data(), 0, cb.size()); };
      auto OffOf = [&](const void* m) { return uint32(static_cast<const uint8*>(m) - cb.data()); };
      auto* P = reinterpret_cast<CPed*>(cb.data());
      Zero(); P->bInVehicle = true; Chk("CPed.bInVehicle", At<uint32>(cb.data(), 0x46C), 0x100);
      Zero(); P->bFadeOut = true; Chk("CPed.bFadeOut", At<uint32>(cb.data(), 0x470), 8);
      Zero(); P->bCullExtraFarAway = true; Chk("CPed.bCullExtraFarAway", At<uint32>(cb.data(), 0x470), 0x40);
      Zero(); P->bDeadPedInFrontOfCar = true; Chk("CPed.bDeadPedInFrontOfCar", At<uint32>(cb.data(), 0x470), 0x400000);
      Zero(); P->bHasBeenAddedToPopulation = true; Chk("CPed.bHasBeenAddedToPopulation", At<uint32>(cb.data(), 0x474), 4);
      Chk("CPed.m_nPedState", OffOf(&P->m_nPedState), 0x530);
      Chk("CPed.m_nTimeTillWeNeedThisPed", OffOf(&P->m_nTimeTillWeNeedThisPed), 0x54C); Chk("CPed.m_VehDeadInFrontOf", OffOf(&P->m_VehDeadInFrontOf), 0x590);
      Chk("CPed.m_nPedType", OffOf(&P->m_nPedType), 0x598); Chk("CPed.m_nDeathTimeMS", OffOf(&P->m_nDeathTimeMS), 0x750);
      Chk("CPed.m_fRemovalDistMultiplier", OffOf(&P->m_fRemovalDistMultiplier), 0x790); Chk("CPed.m_pAttachedTo", OffOf(&P->m_pAttachedTo), 0xFC);
      Chk("CPed.m_matrix", OffOf(&P->m_matrix), 0x14); Chk("CPed.m_nModelIndex", OffOf(&P->m_nModelIndex), 0x22);
      Chk("CObject.m_pDummyObject", OffOf(&reinterpret_cast<CObject*>(cb.data())->m_pDummyObject), 0x170); Chk("CObject.m_nObjectType", OffOf(&reinterpret_cast<CObject*>(cb.data())->m_nObjectType), 0x13C);
      Chk("CVehicle.m_pDriver", OffOf(&reinterpret_cast<CVehicle*>(cb.data())->m_pDriver), 0x460); Chk("CVehicle.m_apPassengers", OffOf(reinterpret_cast<CVehicle*>(cb.data())->m_apPassengers.data()), 0x464);
      Chk("CPhysical.m_vecMoveSpeed", OffOf(&reinterpret_cast<CPhysical*>(cb.data())->m_vecMoveSpeed), 0x44);
      Chk("CCam.m_nMode", OffOf(&reinterpret_cast<CCam*>(cb.data())->m_nMode), 0xC); Chk("sizeof(eCamMode)", uint32(sizeof(eCamMode)), 2);
      Chk("CCam.m_bLookingBehind", OffOf(&reinterpret_cast<CCam*>(cb.data())->m_bLookingBehind), 7);
      Chk("CColPoint.m_nSurfaceTypeB", OffOf(&reinterpret_cast<CColPoint*>(cb.data())->m_nSurfaceTypeB), 0x23);
      Chk("sizeof(CLoadedCarGroup)", uint32(sizeof(CLoadedCarGroup)), 0x2E);
      Zero(); At<int32>(cb.data(), 0x28) = 7; At<int32>(cb.data(), 0x2C) = 9; At<uint8>(cb.data(), 0x4D) = 5;
      Chk("CPedModelInfo.PedType", uint32(reinterpret_cast<CPedModelInfo*>(cb.data())->GetPedType()), 7);
      Chk("CPedModelInfo.StatType", uint32(reinterpret_cast<CPedModelInfo*>(cb.data())->GetPedStatType()), 9);
      Chk("CVehicleModelInfo.m_nVehicleClass", uint32(reinterpret_cast<CVehicleModelInfo*>(cb.data())->m_nVehicleClass), 5);
      Chk("TASK_COMPLEX_GANG_HASSLE_PED", uint32(TASK_COMPLEX_GANG_HASSLE_PED), 1212);
      Write({ { "event", "layout" }, { "ok", layoutOk }, { "fields", lay } }); }

    // estado global guardado
    if (regions.empty()) {
        Snap(0xA9B0C8, 320 * 4); Snap(MiPtrAddr(kSlotVeh), 8);
        Snap(0xC0EC20, 0x60); Snap(0x8D2530, 0x10); Snap(0xC0FCB2, 0x0C); Snap(0xC0BC40, 4); Snap(0xB72914, 4); Snap(0xB70153, 1); Snap(0xB7CB84, 4); Snap(0xB7CB4C, 4);
        Snap(0xB6F028, 0xA00); Snap(0xB74490, 0x14); Snap(0xB7CD74, 1); Snap(0xC81314, 4); Snap(0xC81324, 4); Snap(0x8E4C00, 32); Snap(0x8E4CC0, 320 * 0x14);
        Snap(0x8CD600, 0x120); Snap(0xC0E998, 0x2A0); Snap(0x969130, 0x60); Snap(0xA43088, 1); Snap(0xB5F852, 1); Snap(0xC0FCBC, 4); Snap(0xC10280, 4);
        Snap(0xC0EC78, sizeof(CPopulation::m_nNumCarsInGroup)); Snap(0xC0ED38, sizeof(CPopulation::m_CarGroups));
        Snap(0xC0ECC0, sizeof(CPopulation::m_nNumPedsInGroup)); Snap(0xC0F358, sizeof(CPopulation::m_PedGroups));
    }
    const unsigned savedFP = _controlfp(0, 0);

    bool passed = layoutOk; int pair = 0;
    for (auto& rn : runs) {
        const size_t f = size_t(rn.f); const auto& fn = fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
        if (!only.empty() && only != full) continue;
        for (int i = 0; i < fn.cases; ++i) {
            auto run = [&](bool candidate) -> json {
                uint32 stt = uint32(i) * 2654435761u ^ uint32(f * 0x9E3779B9u) ^ 0x7F1Au; if (!stt) stt = 1; Rng r{ stt };
                trace = json::array(); atomicNull = false; heldTask = nullptr; vehPtr = nullptr; playerPtr = nullptr;
                FillScripts(r);
                hooks[f]->State(candidate);
                const bool switches = ReversingProbe::JumpsToDll(fn.address) == candidate;   // despues de State(), no antes
                json out = { { "switches", switches } };
                {
                    Patches P;
                    for (int k : rn.uses) { P.Add(cols[size_t(k)].exe, cols[size_t(k)].se); P.Add(cols[size_t(k)].dll, cols[size_t(k)].sd); }
                    _controlfp(_PC_24, _MCW_PC);                 // x87 a 24 bits como el juego
                    out["state"] = rn.run(r);
                    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC); out["fp"] = fp;
                }
                out["trace"] = trace;
                RestoreRegions();
                return out;
            };
            const auto original = run(false), candidate = run(true);
            const bool ok = original == candidate && original["switches"] == true && candidate["switches"] == true;
            passed &= ok; failures += !ok;
            auto Summary = [](const json& r) -> json { const auto s = r.dump(); return std::format("{:016x}", Fnv(s.data(), s.size())); };
            if (ok) Write({ { "event", "comparison" }, { "pair", pair++ }, { "function", full }, { "case", i }, { "equal", true }, { "accepted", true }, { "original", Summary(original) }, { "candidate", Summary(candidate) }, { "events", original["trace"].size() } });
            else Write({ { "event", "comparison" }, { "pair", pair++ }, { "function", full }, { "case", i }, { "equal", false }, { "accepted", false },
                         { "original", failures <= 2 ? original : json(nullptr) }, { "candidate", failures <= 2 ? candidate : json(nullptr) } });
        }
        Write({ { "event", "function_summary" }, { "function", full }, { "failures", failures }, { "cases", fn.cases } });
    }
    for (size_t f = 0; f < fns.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreRegions();
    bool restored = RegionsRestored() && _controlfp(0, 0) == savedFP;
    for (size_t f = 0; f < fns.size(); ++f) restored &= hooks[f]->Hooked() == enabled[f] && hooks[f]->Locked() == locked[f];
    Write({ { "event", passed ? "complete" : "error" }, { "pairs", pair }, { "layout_ok", layoutOk }, { "state_restored", restored } });
}
}
