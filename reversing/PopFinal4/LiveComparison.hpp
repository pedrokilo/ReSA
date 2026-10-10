#pragma once
// Banco PopFinal4 (flujo viejo, estilo PopFinal3/WorldFinal1): las 9 funciones de CPopulation que quedaron fuera de PopFinal3:
// AddPed, AddDeadPedInFrontOfCar, AddPedInCar, AddPedAtAttractor, PlaceCouple, CreateWaitingCoppers, GeneratePedsAtAttractors,
// PopulateInterior y TestSafeForRealObject.
// Original (hook desactivado: EXE) frente a candidato (hook activo: DLL) sobre memoria falsa: CPed::operator new (0x5E4720),
// CVehicle::operator new (0x6E2D50) y CTask::operator new (0x61A5A0) devuelven huecos de buferes propios (relleno 0xCD); los
// constructores de CCivilianPed/CCopPed/CEmergencyPed/CAutomobile se doblan (EXE + simbolo exportado de la DLL) y fijan solo los campos
// que el codigo posterior lee; las tareas y los eventos de pila son REALES (constructores/destructores del EXE y de la DLL) y se
// describen con los campos que escribe el constructor cuando CTaskManager::SetTask (0x681AF0) y CEventGroup::Add (0x4AB420) los reciben.
// Los colaboradores con efectos externos (mundo, camara, suelo, colisiones, rutas, streaming, armas, RNG...) se doblan con trazas
// y argumentos; las funciones de CPopulation que no se prueban en cada caso (AddPed, ChooseCivilian*, ...) tambien. Todos los Body()
// se calculan antes de crear ningun EntryPatch. Se guarda y restaura todo el estado global tocado (state_restored).
#include "Pop4Cases.hpp"
#include "Glass.h"
#include "PedPlacement.h"
#include "Attractors/PedAttractorPedPlacer.h"
#include "TaskTypes/TaskComplexWander.h"
#include "Collision/ColStore.h"
#include "GameLogic.h"
#include "Events/EventSexyPed.h"
#include "Events/EventAttractor.h"
#include "PathFind.h"

namespace PopFinal4Live {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn, 9> fns{{
    {"CPopulation","AddDeadPedInFrontOfCar",0x612CD0,0,-1,256},
    {"CPopulation","AddPed",0x612710,0,-1,1024},
    {"CPopulation","AddPedAtAttractor",0x614210,0,-1,256},
    {"CPopulation","AddPedInCar",0x613A00,0,-1,512},
    {"CPopulation","CreateWaitingCoppers",0x6133F0,0,-1,512},
    {"CPopulation","GeneratePedsAtAttractors",0x615970,0,-1,512},
    {"CPopulation","PlaceCouple",0x613D60,0,-1,512},
    {"CPopulation","PopulateInterior",0x616470,0,-1,256},
    {"CPopulation","TestSafeForRealObject",0x6123A0,0,-1,256},
}};

inline uintptr_t Exported(const char* mangled) { if (!modBase) return 0; const auto p = uintptr_t(GetProcAddress(reinterpret_cast<HMODULE>(modBase), mangled)); return p ? FollowIn(p) : 0; }

enum J { J_RAND, J_WADD, J_WREM, J_FVEH, J_FPED, J_CAB, J_CHOOSECIV, J_GANGMODEL, J_COPCAR, J_PEDNEW, J_VEHNEW, J_CIV, J_COP, J_EMG, J_AUTO, J_TASKNEW, J_TASKDEL,
         J_SETTASK, J_WANDER, J_EVADD, J_REG, J_CLEAN, J_SETORIENT, J_SETPOSN, J_GIVEDEL, J_CURSLOT, J_CURTYPE, J_GIVEW, J_CREATEDBY, J_SETDM, J_EVHANDLER,
         J_INTELPROC, J_WALK, J_PLACEPED, J_ATTRMGR, J_HASQUEUE, J_SETINCAR, J_DOOR, J_GROUNDZ, J_CLEARPOS, J_SPHERE, J_LOS, J_FINDRANGE, J_PROCCOL, J_SETALPHA,
         J_NORM, J_PLACEROAD, J_SIREN, J_UPDRW, J_SIMPLEUPD, J_JOIN, J_FINDNTH, J_GENINT, J_FINDNEW, J_LOADCOLL, J_LOADALL, J_RIOTS, J_MODA, J_MODB, J_MODC,
         J_ADDPED, J_FORVEH, J_ADDATTR, J_GENATTR, J_REMOVEALL, J_BOUNDREF, J_TOUCH, J_ALLOCMAT, J_UPDMAT, J_GETCOL, J_RAND2D, J_COORS, J_N };
struct Col { const char* n; uintptr_t exe; uintptr_t dll; void* se; void* sd; };
inline std::array<Col, J_N> cols{};
template<class F> inline void* P_(F f) { return reinterpret_cast<void*>(f); }
struct Run { int f; json (*run)(Rng&); std::vector<int> uses; };
inline std::vector<Run> runs;
inline void* __cdecl S_Coors(CVector* out, int32) { if (out) *out = CVector{}; return out; }

inline void BuildCollabs() {
    auto S = [&](int k, const char* n, uintptr_t exe, uintptr_t dll, void* se, void* sd = nullptr) { cols[size_t(k)] = { n, exe, dll, se, sd ? sd : se }; };
    S(J_RAND, "rand", 0x821B1E, Body(&CGeneral::GetRandomNumber), P_(&S_RandExe), P_(&S_RandDll));
    S(J_WADD, "world_add", 0x563220, Body(&CWorld::Add), P_(&S_WorldAdd));
    S(J_WREM, "world_remove", 0x563280, Body(&CWorld::Remove), P_(&S_WorldRemove));
    S(J_FVEH, "find_vehicle", 0x56E0D0, Body(&FindPlayerVehicle), P_(&S_FindVeh));
    S(J_FPED, "find_ped", 0x56E210, Body(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)), P_(&S_FindPed));
    S(J_CAB, "cab_model", 0x407D50, Body(&CStreaming::GetDefaultCabDriverModel), P_(&S_Cab));
    S(J_CHOOSECIV, "choose_civilian", 0x612F90, Body(&CPopulation::ChooseCivilianOccupation), P_(&S_ChooseCiv));
    S(J_GANGMODEL, "gang_model", 0x5DE5A0, Body(&CGangs::ChooseGangPedModel), P_(&S_GangModel));
    S(J_COPCAR, "default_cop_car", 0x407C50, Body(&CStreaming::GetDefaultCopCarModel), P_(&S_CopCar));
    S(J_PEDNEW, "ped_new", 0x5E4720, Body(static_cast<void*(*)(unsigned)>(&CPed::operator new)), P_(&S_PedNew));
    S(J_VEHNEW, "veh_new", 0x6E2D50, Body(static_cast<void*(*)(unsigned)>(&CVehicle::operator new)), P_(&S_VehNew));
    S(J_CIV, "civ_ctor", 0x5DDB70, Exported("??0CCivilianPed@@QAE@W4ePedType@@I@Z"), P_(&S_CivCtor));
    S(J_COP, "cop_ctor", 0x5DDC60, Exported("??0CCopPed@@QAE@I@Z"), P_(&S_CopCtor));
    S(J_EMG, "emergency_ctor", 0x5DE340, Exported("??0CEmergencyPed@@QAE@W4ePedType@@I@Z"), P_(&S_EmgCtor));
    S(J_AUTO, "automobile_ctor", 0x6B0A90, Exported("??0CAutomobile@@QAE@HW4eVehicleCreatedBy@@_N@Z"), P_(&S_AutoCtor));
    S(J_TASKNEW, "task_new", 0x61A5A0, Body(static_cast<void*(*)(size_t)>(&CTask::operator new)), P_(&S_TaskNew));
    S(J_TASKDEL, "task_delete", 0x61A5B0, Body(static_cast<void(*)(void*)>(&CTask::operator delete)), P_(&S_TaskDelete));
    S(J_SETTASK, "set_task", 0x681AF0, Body(&CTaskManager::SetTask), P_(&S_SetTask));
    S(J_WANDER, "wander_task", 0x673D00, Body(&CTaskComplexWander::GetWanderTaskByPedType), P_(&S_Wander));
    S(J_EVADD, "event_add", 0x4AB420, Body(static_cast<CEvent*(CEventGroup::*)(CEvent*, bool)>(&CEventGroup::Add)), P_(&S_EvAdd));
    S(J_REG, "register_reference", 0x571B70, Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)), P_(&S_RegRef));
    S(J_CLEAN, "cleanup_reference", 0x571A00, Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)), P_(&S_CleanRef));
    S(J_SETORIENT, "set_orientation", 0x439A80, Body(static_cast<void(CPlaceable::*)(float, float, float)>(&CPlaceable::SetOrientation)), P_(&S_SetOrient));
    S(J_SETPOSN, "set_posn", 0x420B80, Body(static_cast<void(CPlaceable::*)(float, float, float)>(&CPlaceable::SetPosn)), P_(&S_SetPosn));
    S(J_GIVEDEL, "give_delayed", 0x5E89B0, Body(&CPed::GiveDelayedWeapon), P_(&S_GiveDelayed));
    S(J_CURSLOT, "set_current_slot", 0x5E61F0, Body(static_cast<void(CPed::*)(int32)>(&CPed::SetCurrentWeapon)), P_(&S_SetCurSlot));
    S(J_CURTYPE, "set_current_type", 0x5E6280, Body(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)), P_(&S_SetCurType));
    S(J_GIVEW, "give_weapon", 0x5E6080, Body(static_cast<eWeaponSlot(CPed::*)(eWeaponType, uint32, bool)>(&CPed::GiveWeapon)), P_(&S_GiveWeapon));
    S(J_CREATEDBY, "set_created_by", 0x5E47E0, Body(&CPed::SetCharCreatedBy), P_(&S_SetCreatedBy));
    S(J_SETDM, "set_decision_maker", 0x600B50, Body(&CPedIntelligence::SetPedDecisionMakerType), P_(&S_SetDM));
    S(J_EVHANDLER, "process_event_handler", 0x601BB0, Body(&CPedIntelligence::ProcessEventHandler), P_(&S_EvHandler));
    S(J_INTELPROC, "intelligence_process", 0x608260, Body(&CPedIntelligence::Process), P_(&S_IntelProcess));
    S(J_WALK, "walk_speed", 0x5E04B0, Body(&CPed::GetWalkAnimSpeed), P_(&S_WalkSpeed));
    S(J_PLACEPED, "place_ped_at_effect", 0x5EA390, Body(&CPedAttractorPedPlacer::PlacePedAtEffect), P_(&S_PlacePed));
    S(J_ATTRMGR, "attractor_manager", 0x5EE190, Body(&GetPedAttractorManager), P_(&S_AttrMgr));
    S(J_HASQUEUE, "has_queue_tail", 0x5EBBA0, Body(&CPedAttractorManager::HasQueueTailArrivedAtSlot), P_(&S_HasQueue));
    S(J_SETINCAR, "set_ped_in_car", 0x650280, Body(&CCarEnterExit::SetPedInCarDirect), P_(&S_SetInCar));
    S(J_DOOR, "compute_door", 0x64F190, Body(&CCarEnterExit::ComputeTargetDoorToEnterAsPassenger), P_(&S_Door));
    S(J_GROUNDZ, "ground_z", 0x5696C0, Body(&CWorld::FindGroundZFor3DCoord), P_(&S_GroundZ));
    S(J_CLEARPOS, "position_clear", 0x616860, Body(static_cast<bool(*)(const CVector&, float, int32, CEntity**, bool, bool, bool)>(&CPedPlacement::IsPositionClearForPed)), P_(&S_ClearPos));
    S(J_SPHERE, "sphere_visible", 0x420C40, Body(static_cast<bool(CCamera::*)(const CVector&, float, RwMatrix*)>(&CCamera::IsSphereVisible)), P_(&S_Sphere));
    S(J_LOS, "line_of_sight", 0x56A490, Body(&CWorld::GetIsLineOfSightClear), P_(&S_Los));
    S(J_FINDRANGE, "find_in_range", 0x564A20, Body(&CWorld::FindObjectsInRange), P_(&S_FindInRange));
    S(J_PROCCOL, "process_col", 0x4185C0, Body(&CCollision::ProcessColModels), P_(&S_ProcessCol));
    S(J_SETALPHA, "set_alpha", 0x732B00, Body(&CVisibilityPlugins::SetClumpAlpha), P_(&S_SetAlpha));
    S(J_NORM, "normalise", 0x59C910, Body(&CVector::Normalise), P_(&S_Normalise));
    S(J_PLACEROAD, "place_on_road", 0x6AF420, Body(&CAutomobile::PlaceOnRoadProperly), P_(&S_PlaceRoad));
    S(J_SIREN, "uses_siren", 0x6D8470, Body(&CVehicle::UsesSiren), P_(&S_Siren));
    S(J_UPDRW, "matrix_update_rw", 0x59AD70, Body(&CMatrix::UpdateRwMatrix), P_(&S_UpdRw));
    S(J_SIMPLEUPD, "transform_update_rw", 0x54EF40, Body(&CSimpleTransform::UpdateRwMatrix), P_(&S_SimpleUpdRw));
    S(J_JOIN, "join_car", 0x42F5A0, 0, P_(&S_Join));   // la DLL llama al EXE directamente (plugin::Call)
    S(J_FINDNTH, "find_nth_node", 0x44F8C0, Body(&CPathFind::FindNthNodeClosestToCoors), P_(&S_FindNth));
    S(J_GENINT, "gen_interior", 0x44ECA0, Body(&CPathFind::GeneratePedCreationCoors_Interior), P_(&S_GenInterior));
    S(J_FINDNEW, "find_new_ped_type", 0x60FBD0, Body(&CPopCycle::FindNewPedType), P_(&S_FindNewPed));
    S(J_LOADCOLL, "load_collision", 0x410860, Body(&CColStore::LoadCollision), P_(&S_LoadColl));
    S(J_LOADALL, "load_all_requested", 0x40EA10, Body(&CStreaming::LoadAllRequestedModels), P_(&S_LoadAll));
    S(J_RIOTS, "la_riots", 0x441C10, Body(&CGameLogic::LaRiotsActiveHere), P_(&S_Riots));
    S(J_MODA, "default_medic", 0x407D20, Body(&CStreaming::GetDefaultMedicModel), P_(&S_ModelA));
    S(J_MODB, "default_fireman", 0x407D40, Body(&CStreaming::GetDefaultFiremanModel), P_(&S_ModelB));
    S(J_MODC, "default_cop", 0x407C00, Body(&CStreaming::GetDefaultCopModel), P_(&S_ModelC));
    S(J_ADDPED, "add_ped", 0x612710, Body(&CPopulation::AddPed), P_(&S_AddPed));
    S(J_FORVEH, "choose_for_vehicle", 0x613260, Body(&CPopulation::ChooseCivilianOccupationForVehicle), P_(&S_ChooseForVeh));
    S(J_ADDATTR, "add_ped_at_attractor", 0x614210, Body(&CPopulation::AddPedAtAttractor), P_(&S_AddAtAttr));
    S(J_GENATTR, "gen_attractors", 0x615970, Body(&CPopulation::GeneratePedsAtAttractors), P_(&S_GenAttr4));
    S(J_REMOVEALL, "remove_all", 0x6122C0, Body(&CPopulation::RemoveAllRandomPeds), P_(&S_RemoveAll));
    S(J_BOUNDREF, "bound_ref", 0x534290, Body(static_cast<void(CEntity::*)(CVector&) const>(&CEntity::GetBoundCentre)), P_(&S_BoundRef));
    S(J_TOUCH, "is_touching", 0x5344B0, Body(static_cast<bool(CEntity::*)(const CVector&, float) const>(&CEntity::GetIsTouching)), P_(&S_Touching));
    S(J_ALLOCMAT, "allocate_matrix", 0x54F560, Body(&CPlaceable::AllocateMatrix), P_(&S_AllocMat));
    S(J_UPDMAT, "update_matrix", 0x54F1B0, Body(&CSimpleTransform::UpdateMatrix), P_(&S_UpdMat));
    S(J_GETCOL, "get_col_model", 0x535300, Body(&CEntity::GetColModel), P_(&S_GetCol));
    S(J_RAND2D, "random_2d_effect", 0x533410, Body(&CEntity::GetRandom2dEffect), P_(&S_Random2d));
    S(J_COORS, "find_coors", 0x56E010, 0, P_(&S_Coors));   // solo lo llama el EXE (resultado sin uso)
}

inline void BuildRuns() {
    auto A = [&](const char* name, json (*fn)(Rng&), std::vector<int> uses) {
        for (size_t i = 0; i < fns.size(); ++i) if (!std::strcmp(fns[i].name, name)) { runs.push_back({ int(i), fn, std::move(uses) }); return; }
    };
    const std::vector<int> ped{ J_PEDNEW, J_CIV, J_COP, J_EMG, J_WADD, J_SETORIENT, J_GIVEDEL, J_CURSLOT, J_CURTYPE, J_SETTASK, J_WANDER, J_EVADD, J_TASKNEW, J_TASKDEL, J_REG, J_CLEAN, J_FPED, J_RAND };
    A("AddPed", CaseAddPed, ped);
    A("AddDeadPedInFrontOfCar", CaseAddDead, { J_SPHERE, J_FPED, J_FVEH, J_GROUNDZ, J_ADDPED, J_EVADD, J_REG, J_CLEAN, J_TASKNEW, J_TASKDEL, J_CLEARPOS, J_PROCCOL, J_SETALPHA, J_WREM, J_GETCOL });
    A("AddPedInCar", CaseAddInCar, { J_COORS, J_CAB, J_RAND, J_ADDPED, J_MODA, J_MODB, J_CHOOSECIV, J_FORVEH, J_GANGMODEL, J_DOOR, J_SETINCAR });
    A("PlaceCouple", CaseCouple2, { J_RIOTS, J_SPHERE, J_FPED, J_FVEH, J_CLEARPOS, J_GROUNDZ, J_ADDPED, J_SETALPHA, J_WALK, J_SETTASK, J_TASKNEW, J_TASKDEL, J_REG, J_CLEAN, J_SETPOSN, J_WREM });
    A("AddPedAtAttractor", CaseAddAtAttr, { J_HASQUEUE, J_ATTRMGR, J_ADDPED, J_CREATEDBY, J_SETDM, J_SETTASK, J_WANDER, J_PLACEPED, J_EVADD, J_EVHANDLER, J_INTELPROC, J_REG, J_CLEAN, J_TASKNEW, J_TASKDEL });
    A("CreateWaitingCoppers", CaseCoppers, { J_FPED, J_COPCAR, J_FINDNTH, J_PEDNEW, J_VEHNEW, J_COP, J_AUTO, J_NORM, J_PLACEROAD, J_UPDRW, J_SIMPLEUPD, J_JOIN, J_SIREN, J_WADD, J_RAND, J_LOS, J_FINDRANGE, J_GROUNDZ, J_GIVEW, J_CURTYPE, J_EVADD, J_INTELPROC, J_REG, J_CLEAN, J_TASKNEW, J_TASKDEL });
    A("GeneratePedsAtAttractors", CaseGenAttr, { J_FINDRANGE, J_RAND2D, J_ALLOCMAT, J_UPDMAT, J_SPHERE, J_RAND, J_MODC, J_CHOOSECIV, J_ADDATTR });
    A("PopulateInterior", CasePopInterior, { J_REMOVEALL, J_LOADCOLL, J_LOADALL, J_GENATTR, J_GENINT, J_FINDNEW, J_ADDPED, J_SETDM });
    A("TestSafeForRealObject", CaseTestSafe, { J_BOUNDREF, J_TOUCH, J_ALLOCMAT, J_UPDMAT, J_GETCOL, J_PROCCOL });
}

inline void Update() {
    if (PopFinal3Live::done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "popfinal4-runtime.start")) { PopFinal3Live::done = true; return; }
    if (!PopFinal3Live::log.is_open()) { PopFinal3Live::log.open(dir / "popfinal4-runtime.jsonl"); Write({ { "event", "waiting_for_player" } }); ReversingProbe::InstallFailureCapture("popfinal4"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; PopFinal3Live::done = true; owner = GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir / "popfinal4-only.txt"); if (f) std::getline(f, only); }

    std::array<ReversibleHooks::ReversibleHook::Base*, fns.size()> hooks{};
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ { "event", "error" }, { "reason", "hook not found" }, { "function", std::string(fns[f].cls) + "::" + fns[f].name } }); return; }

    BuildCollabs();
    { std::set<uintptr_t> all; bool bad = false; json miss = json::array();
      for (auto& c : cols) { if (!c.n) continue; if (!c.dll) { if (c.exe == 0x42F5A0 || c.exe == 0x56E010) continue; bad = true; miss.push_back(c.n); } else if (!all.insert(c.dll).second) { bad = true; miss.push_back(std::string(c.n) + "(repetido)"); } }
      if (bad) { Write({ { "event", "error" }, { "reason", "DLL body not found or overlapping" }, { "which", miss } }); return; } }
    BuildRuns();

    std::array<bool, fns.size()> enabled{}, locked{};
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    for (auto& v : entVt) v = P_(&V_Trap); entVt[0] = P_(&V_Dtor); entVt[4] = P_(&V_SetStatic);
    for (auto& v : miVt) v = P_(&V_Ret2); miVt[1] = P_(&V_AsAtomic);   // GetRwModelType() (hueco desconocido) devuelve rpCLUMP = 2
    for (auto& v : taskVt) v = P_(&V_Trap); taskVt[0] = P_(&V_Dtor); taskVt[4] = P_(&V_TaskType);
    for (auto& c : clumpBuf) { std::memset(c.data(), 0, c.size()); c[0] = 2; }
    if (named.empty()) {
        Nm(pedStore.data(), pedStore.size(), "ped"); Nm(objStore.data(), objStore.size(), "obj"); Nm(dumStore.data(), dumStore.size(), "dummy");
        Nm(vehRaw.data(), vehRaw.size(), "veh"); Nm(clumpBuf.data(), sizeof(clumpBuf), "clump"); Nm(taskArena.data(), sizeof(taskArena), "task");
        for (int i = 0; i < kNewPeds; ++i) { static const char* np[8]{ "newped0","newped1","newped2","newped3","newped4","newped5","newped6","newped7" }; static const char* ni[8]{ "intel0","intel1","intel2","intel3","intel4","intel5","intel6","intel7" }; Nm(newPed[size_t(i)].data(), newPed[size_t(i)].size(), np[i]); Nm(newIntel[size_t(i)].data(), 0x400, ni[i]); }
        for (int i = 0; i < kNewVehs; ++i) { static const char* nv[4]{ "newveh0","newveh1","newveh2","newveh3" }; Nm(newVeh[size_t(i)].data(), newVeh[size_t(i)].size(), nv[i]); }
        Nm(playerPed.data(), playerPed.size(), "player"); Nm(playerIntel.data(), playerIntel.size(), "playerintel"); Nm(fakeTask.data(), fakeTask.size(), "faketask");
        Nm(fxBuf.data(), sizeof(fxBuf), "effect"); Nm(entMat.data(), sizeof(entMat), "entmat"); Nm(pathNodes.data(), sizeof(pathNodes), "pathnodes");
    }

    bool layoutOk = true;
    { json lay = json::object();
      alignas(16) static std::array<uint8, 0x900> cb{};
      auto Chk = [&](const char* n, uint32 got, uint32 want) { lay[n] = got; if (got != want) { layoutOk = false; lay[std::string(n) + "_expected"] = want; } };
      auto Zero = [&] { std::memset(cb.data(), 0, cb.size()); };
      auto OffOf = [&](const void* m) { return uint32(static_cast<const uint8*>(m) - cb.data()); };
      auto* P = reinterpret_cast<CPed*>(cb.data());
      Zero(); P->bUseAttractorInstantly = true; Chk("CPed.bUseAttractorInstantly", At<uint32>(cb.data(), 0x474), 0x200000);
      Chk("CPed.m_nAnimGroup", OffOf(&P->m_nAnimGroup), 0x4D4); Chk("CPed.m_nMoneyCount", OffOf(&P->m_nMoneyCount), 0x756);
      /* el EXE escribe el mismo valor en 0x558 y 0x55C (0x6138B9/0x6138BF); la DLL los nombra al reves que este banco */ Chk("CPed.m_fCurrentRotation", OffOf(&P->m_fCurrentRotation), 0x558); Chk("CPed.m_fAimingRotation", OffOf(&P->m_fAimingRotation), 0x55C);
      Chk("CPed.m_nPedType", OffOf(&P->m_nPedType), 0x598); Chk("CPed.m_VehDeadInFrontOf", OffOf(&P->m_VehDeadInFrontOf), 0x590);
      Zero(); Chk("CEventSexyPed.m_SexyPed", OffOf(&reinterpret_cast<CEventSexyPed*>(cb.data())->m_SexyPed), 0x14); Chk("CEventAttractor.m_entity", OffOf(&reinterpret_cast<CEventAttractor*>(cb.data())->m_entity), 0x18);
      Zero(); P->bDeadPedInFrontOfCar = true; Chk("CPed.bDeadPedInFrontOfCar", At<uint32>(cb.data(), 0x470), 0x400000);
      Zero(); reinterpret_cast<CVehicle*>(cb.data())->vehicleFlags.bSirenOrAlarm = true; Chk("CVehicle.bSirenOrAlarm", At<uint8>(cb.data(), 0x42D), 0x80);
      Chk("CEntity.scancode", [&] { Zero(); At<uint16>(cb.data(), 0x2C) = 0x1234; return uint32(reinterpret_cast<CEntity*>(cb.data())->GetScanCode()); }(), 0x1234);
      Zero(); { auto* fx = reinterpret_cast<C2dEffectPedAttractor*>(cb.data());
        Chk("Attractor.m_Pos", OffOf(&fx->m_Pos), 0); Chk("Attractor.m_Type", OffOf(&fx->m_Type), 0xC); Chk("EffectAttractor", uint32(EFFECT_ATTRACTOR), 3); Chk("Attractor.type", OffOf(&fx->m_nAttractorType), 0x34); Chk("Attractor.flags", OffOf(&fx->m_nFlags), 0x37); Chk("Attractor.name", OffOf(fx->m_szScriptName), 0x38); }
      Zero(); At<int32>(cb.data(), 0x2C) = 3; At<void*>(playerData.data(), 0) = cb.data();
      Chk("CWanted.level", uint32(reinterpret_cast<CWanted*>(cb.data())->GetWantedLevel()), 3);
      Chk("CPlayerPedData.m_pWanted", uint32(reinterpret_cast<CPlayerPedData*>(playerData.data())->m_pWanted == reinterpret_cast<CWanted*>(cb.data())), 1);
      Chk("CPathNode.sizeof", uint32(sizeof(CPathNode)), 0x1C);
      Chk("CObject.objectFlags", [&] { Zero(); reinterpret_cast<CObject*>(cb.data())->objectFlags.bEnableDisabledAttractors = true; return At<uint32>(cb.data(), 0x140); }(), 0x1000000);
      Chk("TASK_COMPLEX_USE_ATTRACTOR(DLL)", uint32(TASK_COMPLEX_USE_ATTRACTOR), 235);
      Write({ { "event", "layout" }, { "ok", layoutOk }, { "fields", lay } }); }

    if (regions.empty()) {
        Snap(0xA9B0C8, 320 * 4); Snap(MiPtrAddr(kSlotVeh), 8);
        Snap(0xC0EC20, 0x60); Snap(0x8D2530, 0x10); Snap(0xC0FCB2, 0x0C); Snap(0xC0BC40, 4); Snap(0xB72914, 4); Snap(0xB70153, 1); Snap(0xB7CB84, 4); Snap(0xB7CB4C, 4);
        Snap(0xB6F028, 0xA00); Snap(0xB74490, 0x14); Snap(0xB7CD74, 1); Snap(0xB7CD78, 2); Snap(0xC81314, 4); Snap(0xC81324, 4); Snap(0x8E4C00, 32); Snap(0x8E4CC0, 320 * 0x14);
        Snap(0x8CD600, 0x120); Snap(0xC0E998, 0x2A0); Snap(0x969130, 0x60); Snap(0xC091F0, 0xA0); Snap(0x96F854, 4 * 32); Snap(0xB992B8, 256 * 12); Snap(0xC10280, 4);
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
                const bool switches = ReversingProbe::JumpsToDll(fn.address) == candidate;
                json out = { { "switches", switches } };
                {
                    Patches P;
                    for (int k : rn.uses) { auto& c = cols[size_t(k)]; P.Add(c.exe, c.se); P.Add(c.dll, c.sd); }
                    _controlfp(_PC_24, _MCW_PC);
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
            else {
                json diff = json::array(); for (auto& [k, v] : original.items()) if (candidate[k] != v) diff.push_back(k);
                json first = nullptr; const auto& ot = original["trace"]; const auto& ct = candidate["trace"];
                for (size_t k = 0; k < std::max(ot.size(), ct.size()); ++k) { const json a = k < ot.size() ? ot[k] : json(nullptr), b = k < ct.size() ? ct[k] : json(nullptr); if (a != b) { first = json::array({ int(k), a, b }); break; } }
                Write({ { "event", "comparison" }, { "pair", pair++ }, { "function", full }, { "case", i }, { "equal", false }, { "accepted", false }, { "differs", diff }, { "first_trace_diff", first },
                        { "original", failures <= 2 ? original : json(nullptr) }, { "candidate", failures <= 2 ? candidate : json(nullptr) } });
            }
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
