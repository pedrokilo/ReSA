#pragma once
// Banco dentro del motor (flujo viejo, estilo Small2/PadMouse1/WorldFinal1): CPathFind (54 funciones pendientes).
// Original (hook de la funcion desactivado, se ejecuta el codigo del EXE) frente a candidato (hook activo) sobre un ThePaths con
// contenido de prueba: se hace una copia de ThePaths (0x96F050, 0x3C80 bytes) y de las globales vecinas, se sobrescriben con datos
// deterministas por caso y se restauran tras cada ejecucion (state_restored). Las areas de nodos del banco viven en un arena
// propio; los nodos, enlaces y ficheros reales del juego nunca se tocan. La carga de nodes%d.dat se prueba con RwStream* simulado
// y buferes en memoria (CMemoryMgr::Malloc/Free simulados). El pool de vehiculos real (0xB74494) se sustituye por uno falso.
//
// Colaboradores simulados y trazados (parche en la direccion del EXE y en el cuerpo de la DLL, solo durante cada ejecucion):
// CMemoryMgr::Malloc/Free (0x72F420/0x72F430), RwStreamOpen/Read/Close (0x7ECEF0/0x7EC9D0/0x7ECE20), CTimer::Suspend/Resume (0x5619D0/
// 0x561A00), CPathFind::FindNodeClosestToCoors (0x44F460), CStreaming::RequestModel/RemoveModel (0x4087E0/0x4089A0), FindPlayerPed/
// FindPlayerCoors (0x56E210/0x56E010), CGenericGameStorage::SaveDataToWorkBuffer/LoadDataFromWorkBuffer (0x5D1270/0x5D1300).
#include "PathCases.hpp"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Heli.h"
#include <reversiblebugfixes/Bugs.hpp>

namespace PathFinal3Live {
// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,54> fns{{
    {"CPathFind","AddDynamicLinkBetween2Nodes_For1Node",0x44E000,0,-1,1024},
    {"CPathFind","AddInteriorLink",0x44DED0,0,-1,512},
    {"CPathFind","AddInteriorLinkToExternalNode",0x44DF30,0,-1,256},
    {"CPathFind","AddNodeToList",0x44D1E0,0,-1,512},
    {"CPathFind","AddNodeToNewInterior",0x450E90,0,-1,512},
    {"CPathFind","AreNodesLoadedForArea",0x44DD10,0,-1,1024},
    {"CPathFind","CountNeighboursToBeSwitchedOff",0x4504F0,0,-1,512},
    {"CPathFind","FindLinkBetweenNodes",0x451350,0,-1,512},
    {"CPathFind","FindNearestExteriorNodeToInteriorNode",0x450F30,0,-1,256},
    {"CPathFind","FindNodeClosestToCoorsFavourDirection",0x44FCE0,0,-1,1024},
    {"CPathFind","FindNodeCoorsForScript-LinkedNode",0x4505E0,0,-1,1024},
    {"CPathFind","FindNodeCoorsForScript-TwoNodes",0x450780,0,-1,1024},
    {"CPathFind","FindRegionForCoors",0x44D830,0,-1,1024},
    {"CPathFind","FindStartPointOfRegion-",0x44D930,0,-1,256},
    {"CPathFind","FindXCoorsForRegion",0x44D8F0,0,-1,128},
    {"CPathFind","FindXRegionForCoors",0x44D890,0,-1,1024},
    {"CPathFind","FindYCoorsForRegion",0x44D910,0,-1,128},
    {"CPathFind","FindYRegionForCoors",0x44D8C0,0,-1,1024},
    {"CPathFind","GetPathNode",0x420AC0,0,-1,256},
    {"CPathFind","HaveRequestedNodesBeenLoaded",0x450DB0,0,-1,256},
    {"CPathFind","Init",0x44D080,0,-1,128},
    {"CPathFind","IsAreaNodesAvailable",0x420AA0,0,-1,256},
    {"CPathFind","IsWaterNodeNearby",0x450DE0,0,-1,768},
    {"CPathFind","Load",0x5D3500,0,-1,256},
    {"CPathFind","LoadPathFindData-Area",0x452F40,0,-1,512},
    {"CPathFind","LoadPathFindData-FromStream",0x4529F0,0,-1,512},
    {"CPathFind","LoadSceneForPathNodes",0x44DE00,0,-1,256},
    {"CPathFind","MakeRequestForNodesToBeLoaded",0x450D70,0,-1,512},
    {"CPathFind","MarkRegionsForCoors",0x44DB60,0,-1,1024},
    {"CPathFind","MarkRoadNodeAsDontWander",0x450560,0,-1,256},
    {"CPathFind","ReInit",0x44E4E0,0,-1,64},
    {"CPathFind","ReleaseRequestedNodes",0x44DD00,0,-1,64},
    {"CPathFind","RemoveInterior",0x44E1A0,0,-1,768},
    {"CPathFind","RemoveNodeFromList",0x44D1B0,0,-1,256},
    {"CPathFind","ReturnInteriorNodeIndex",0x451300,0,-1,256},
    {"CPathFind","Save",0x5D34C0,0,-1,256},
    {"CPathFind","SetLinksBridgeLights",0x44D960,0,-1,512},
    {"CPathFind","SetPathsNeededAtPosition",0x44DCD0,0,-1,128},
    {"CPathFind","Shutdown",0x450950,0,-1,128},
    {"CPathFind","StartNewInterior",0x44DE80,0,-1,256},
    {"CPathFind","SwitchOffNodeAndNeighbours",0x452160,0,-1,1024},
    {"CPathFind","SwitchPedRoadsOffInArea",0x452F00,0,-1,512},
    {"CPathFind","SwitchRoadsOffInArea",0x452C80,0,-1,768},
    {"CPathFind","SwitchRoadsOffInAreaForOneRegion",0x452820,0,-1,768},
    {"CPathFind","TakeWidthIntoAccountForWandering",0x4509A0,0,-1,512},
    {"CPathFind","TestCrossesRoad",0x44D790,0,-1,512},
    {"CPathFind","TestForPedTrafficLight",0x44D480,0,-1,512},
    {"CPathFind","These2NodesAreAdjacent",0x44D230,0,-1,512},
    {"CPathFind","ThisNodeHasToBeSwitchedOff",0x44D3E0,0,-1,128},
    {"CPathFind","ThisNodeWillLeadIntoADeadEnd",0x44D310,0,-1,512},
    {"CPathFind","TidyUpNodeSwitchesAfterMission",0x44D3B0,0,-1,128},
    {"CPathFind","UnLoadPathFindData",0x44D0F0,0,-1,256},
    {"CPathFind","UnMarkAllRoadNodesAsDontWander",0x44D400,0,-1,256},
    {"CPathFind","UpdateStreaming",0x450A60,0,-1,1024},
}};

// colaboradores que necesita cada funcion (mascara)
enum : uint32 { N_MEM=1, N_STREAM=2, N_TIMER=4, N_CLOSEST=8, N_STREAMING=16, N_PLAYER=32, N_STORAGE=64 };
inline uint32 Needs(const std::string& nm){
    if(nm=="UnLoadPathFindData"||nm=="RemoveInterior") return N_MEM;
    if(nm=="LoadPathFindData-FromStream") return N_MEM|N_STREAM;
    if(nm=="LoadPathFindData-Area") return N_MEM|N_STREAM|N_TIMER;
    if(nm=="MarkRoadNodeAsDontWander"||nm=="FindNearestExteriorNodeToInteriorNode") return N_CLOSEST;
    if(nm=="LoadSceneForPathNodes"||nm=="Shutdown") return N_STREAMING;
    if(nm=="UpdateStreaming"||nm=="MakeRequestForNodesToBeLoaded") return N_STREAMING|N_PLAYER;
    if(nm=="Save"||nm=="Load") return N_STORAGE;
    return 0; }

// cuerpos de la DLL de los colaboradores (se calculan antes de crear ningun EntryPatch y con los hooks en su estado inicial)
struct Bodies { uintptr_t timerSuspend{}, timerResume{}, request{}, remove{}, ped{}, coors{}, save{}, load{}, memAlloc{}; };

inline json RunCase(const std::string& nm,Rng& r){
    if(nm=="AddDynamicLinkBetween2Nodes_For1Node") return C_AddDynamicLink(r);
    if(nm=="AddInteriorLink") return C_AddInteriorLink(r);
    if(nm=="AddInteriorLinkToExternalNode") return C_AddInteriorLinkExt(r);
    if(nm=="AddNodeToList") return C_AddNodeToList(r);
    if(nm=="AddNodeToNewInterior") return C_AddNodeToNewInterior(r);
    if(nm=="AreNodesLoadedForArea") return C_AreNodesLoaded(r);
    if(nm=="CountNeighboursToBeSwitchedOff") return C_CountNeighbours(r);
    if(nm=="FindLinkBetweenNodes") return C_FindLinkBetween(r);
    if(nm=="FindNearestExteriorNodeToInteriorNode") return C_NearestExterior(r);
    if(nm=="FindNodeClosestToCoorsFavourDirection") return C_FavourDirection(r);
    if(nm=="FindNodeCoorsForScript-LinkedNode") return C_NodeCoorsLinked(r);
    if(nm=="FindNodeCoorsForScript-TwoNodes") return C_NodeCoorsTwo(r);
    if(nm=="FindRegionForCoors") return C_FindRegion(r);
    if(nm=="FindStartPointOfRegion-") return C_FindStartPoint(r);
    if(nm=="FindXCoorsForRegion") return C_FindXCoors(r);
    if(nm=="FindXRegionForCoors") return C_FindXRegion(r);
    if(nm=="FindYCoorsForRegion") return C_FindYCoors(r);
    if(nm=="FindYRegionForCoors") return C_FindYRegion(r);
    if(nm=="GetPathNode") return C_GetPathNode(r);
    if(nm=="HaveRequestedNodesBeenLoaded") return C_HaveRequested(r);
    if(nm=="Init") return C_Init(r);
    if(nm=="IsAreaNodesAvailable") return C_IsAreaNodesAvailable(r);
    if(nm=="IsWaterNodeNearby") return C_IsWaterNearby(r);
    if(nm=="Load") return C_Load(r);
    if(nm=="LoadPathFindData-Area") return C_LoadArea(r);
    if(nm=="LoadPathFindData-FromStream") return C_LoadFromStream(r);
    if(nm=="LoadSceneForPathNodes") return C_LoadScene(r);
    if(nm=="MakeRequestForNodesToBeLoaded") return C_MakeRequest(r);
    if(nm=="MarkRegionsForCoors") return C_MarkRegions(r);
    if(nm=="MarkRoadNodeAsDontWander") return C_MarkDontWander(r);
    if(nm=="ReInit") return C_ReInit(r);
    if(nm=="ReleaseRequestedNodes") return C_ReleaseRequested(r);
    if(nm=="RemoveInterior") return C_RemoveInterior(r);
    if(nm=="RemoveNodeFromList") return C_RemoveNodeFromList(r);
    if(nm=="ReturnInteriorNodeIndex") return C_ReturnInterior(r);
    if(nm=="Save") return C_Save(r);
    if(nm=="SetLinksBridgeLights") return C_BridgeLights(r);
    if(nm=="SetPathsNeededAtPosition") return C_SetPathsNeeded(r);
    if(nm=="Shutdown") return C_Shutdown(r);
    if(nm=="StartNewInterior") return C_StartNewInterior(r);
    if(nm=="SwitchOffNodeAndNeighbours") return C_SwitchOffNeighbours(r);
    if(nm=="SwitchPedRoadsOffInArea") return C_SwitchRoads(r,true);
    if(nm=="SwitchRoadsOffInArea") return C_SwitchRoads(r,false);
    if(nm=="SwitchRoadsOffInAreaForOneRegion") return C_SwitchOneRegion(r);
    if(nm=="TakeWidthIntoAccountForWandering") return C_TakeWidth(r);
    if(nm=="TestCrossesRoad") return C_TestIntersection(r,0x44D790);
    if(nm=="TestForPedTrafficLight") return C_TestIntersection(r,0x44D480);
    if(nm=="These2NodesAreAdjacent") return C_These2Adjacent(r);
    if(nm=="ThisNodeHasToBeSwitchedOff") return C_ThisNodeHasToBeSwitchedOff(r);
    if(nm=="ThisNodeWillLeadIntoADeadEnd") return C_DeadEnd(r);
    if(nm=="TidyUpNodeSwitchesAfterMission") return C_Tidy(r);
    if(nm=="UnLoadPathFindData") return C_UnLoad(r);
    if(nm=="UnMarkAllRoadNodesAsDontWander") return C_UnMarkAll(r);
    if(nm=="UpdateStreaming") return C_UpdateStreaming(r);
    return {{"error","funcion sin caso"}}; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pathfinal3-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pathfinal3-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pathfinal3");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"pathfinal3-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,54> hooks{};
    size_t idxCount=0;
    for(size_t f=0;f<fns.size();++f){ if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
        if(std::string(fns[f].name)=="CountNeighboursToBeSwitchedOff") idxCount=f; }

    // --- cuerpos de la DLL de los colaboradores (hooks en su estado inicial, antes de cualquier EntryPatch) ---
    Bodies B;
    B.timerSuspend=Body(&CTimer::Suspend); B.timerResume=Body(&CTimer::Resume);
    B.request=Body(static_cast<void(*)(int32,int32)>(&CStreaming::RequestModel)); B.remove=Body(static_cast<void(*)(int32)>(&CStreaming::RemoveModel));
    B.ped=Body(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)); B.coors=Body(static_cast<CVector(*)(int32)>(&FindPlayerCoors));
    B.save=Body(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)); B.load=Body(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer));
    // CMemoryMgr::Malloc (0x72F420) esta reconstruida y enganchada: su cuerpo en C++ llama al malloc del CRT (0x824257) sin pasar por 0x72F420
    B.memAlloc=Body(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc));
    if(!B.timerSuspend||!B.timerResume||!B.request||!B.remove||!B.ped||!B.coors||!B.save||!B.load||!B.memAlloc){
        Write({{"event","error"},{"reason","DLL body not found"},{"suspend",B.timerSuspend},{"resume",B.timerResume},{"request",B.request},{"remove",B.remove},{"ped",B.ped},{"coors",B.coors},{"save",B.save},{"load",B.load},{"memAlloc",B.memAlloc}}); return; }
    {   std::set<uintptr_t> all{B.timerSuspend,B.timerResume,B.request,B.remove,B.ped,B.coors,B.save,B.load,B.memAlloc};
        if(all.size()!=9){ Write({{"event","error"},{"reason","DLL bodies overlap"}}); return; } }

    std::array<bool,54> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // nombres de buferes
    if(named.empty()){
        Nm(PathsP(),kPathsSize,"paths"); Nm(arena.data(),arena.size(),"arena"); Nm(mallocArena.data(),mallocArena.size(),"malloc"); Nm(fakeStream.data(),fakeStream.size(),"stream");
        Nm(fakePlayer.data(),fakePlayer.size(),"player"); Nm(fakePool.data(),fakePool.size(),"fake_pool"); Nm(fakeVehStorage.data(),fakeVehStorage.size(),"fake_vehicles"); }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    {   json lay=json::object();
        auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
        auto Off=[&](const void* m){ return uint32(static_cast<const uint8*>(m)-reinterpret_cast<const uint8*>(Paths())); };
        auto* P=Paths();
        Chk("m_pathFindHashTable",Off(P->m_pathFindHashTable),4); Chk("m_pPathNodes",Off(P->m_pPathNodes),0x804); Chk("m_pNaviNodes",Off(P->m_pNaviNodes),0x924);
        Chk("m_pNodeLinks",Off(P->m_pNodeLinks),0xA44); Chk("m_pLinkLengths",Off(P->m_pLinkLengths),0xB64); Chk("m_pPathIntersections",Off(P->m_pPathIntersections),0xC84);
        Chk("m_pNaviLinks",Off(P->m_pNaviLinks),0xDA4); Chk("m_aTempNodes",Off(P->m_aTempNodes),0xEA4); Chk("m_anNumNodes",Off(P->m_anNumNodes),0xFA4);
        Chk("m_anNumVehicleNodes",Off(P->m_anNumVehicleNodes),0x10C4); Chk("m_anNumPedNodes",Off(P->m_anNumPedNodes),0x11E4); Chk("m_anNumCarPathLinks",Off(P->m_anNumCarPathLinks),0x1304);
        Chk("m_anNumAddresses",Off(P->m_anNumAddresses),0x1424); Chk("m_aDynamicLinksBaseIds",Off(P->m_aDynamicLinksBaseIds),0x1544); Chk("m_aDynamicLinksIds",Off(P->m_aDynamicLinksIds),0x2544);
        Chk("m_totalNumNodesInPathFindHashTable",Off(&P->m_totalNumNodesInPathFindHashTable),0x3544); Chk("m_interiorIDs",Off(P->m_interiorIDs),0x3548);
        Chk("m_nNumNodeSwitches",Off(&P->m_nNumNodeSwitches),0x3568); Chk("m_aNodeSwitches",Off(P->m_aNodeSwitches),0x356C); Chk("m_loadAreaRequestPending",Off(&P->m_loadAreaRequestPending),0x3C6C);
        Chk("m_loadAreaRequestMinX",Off(&P->m_loadAreaRequestMinX),0x3C70); Chk("m_loadAreaRequestMaxX",Off(&P->m_loadAreaRequestMaxX),0x3C74);
        Chk("m_loadAreaRequestMinY",Off(&P->m_loadAreaRequestMinY),0x3C78); Chk("m_loadAreaRequestMaxY",Off(&P->m_loadAreaRequestMaxY),0x3C7C);
        Chk("sizeof(CPathFind)",uint32(sizeof(CPathFind)),0x3C80); Chk("sizeof(CPathNode)",uint32(sizeof(CPathNode)),0x1C); Chk("sizeof(CNodesSwitchedOnOrOff)",uint32(sizeof(CNodesSwitchedOnOrOff)),0x1C);
        Chk("sizeof(CCarPathLink)",uint32(sizeof(CCarPathLink)),0xE); Chk("sizeof(CNodeAddress)",uint32(sizeof(CNodeAddress)),4); Chk("sizeof(CCarPathLinkAddress)",uint32(sizeof(CCarPathLinkAddress)),2);
        { alignas(8) uint8 raw[0x1C]{}; auto* n=reinterpret_cast<CPathNode*>(raw);
          Chk("CPathNode.m_next",Off2(&n->m_next,n),0); Chk("CPathNode.m_prev",Off2(&n->m_prev,n),4); Chk("CPathNode.m_vPos",Off2(&n->m_vPos,n),8); Chk("CPathNode.m_totalDistFromOrigin",Off2(&n->m_totalDistFromOrigin,n),0xE);
          Chk("CPathNode.m_wBaseLinkId",Off2(&n->m_wBaseLinkId,n),0x10); Chk("CPathNode.m_wAreaId",Off2(&n->m_wAreaId,n),0x12); Chk("CPathNode.m_wNodeId",Off2(&n->m_wNodeId,n),0x14);
          Chk("CPathNode.m_nPathWidth",Off2(&n->m_nPathWidth,n),0x16); Chk("CPathNode.m_nFloodFill",Off2(&n->m_nFloodFill,n),0x17);
          raw[0x18]=0xA3; raw[0x19]=0x15; raw[0x1A]=0x5A;
          const uint32 bits = uint32(n->m_nNumLinks) | (uint32(n->m_onDeadEnd)<<4) | (uint32(n->m_isSwitchedOff)<<5) | (uint32(n->m_bRoadBlocks)<<6) | (uint32(n->m_bWaterNode)<<7)
              | (uint32(n->m_isSwitchedOffOriginal)<<8) | (uint32(n->unk1)<<9) | (uint32(n->m_bDontWander)<<10) | (uint32(n->unk2)<<11) | (uint32(n->m_bNotHighway)<<12) | (uint32(n->m_bHighway)<<13)
              | (uint32(n->m_nSpawnProbability)<<16) | (uint32(n->m_nBehaviourType)<<20);
          Chk("CPathNode.bitfields",bits,0x5A15A3u); }
        { alignas(8) uint8 raw[0xE]{}; auto* l=reinterpret_cast<CCarPathLink*>(raw); l->m_bridgeLights=1; Chk("CCarPathLink.bridgeLights_byte_0xC",raw[0xC],4); }
        { CNodeAddress na{3,5}; Chk("CNodeAddress.raw",std::bit_cast<uint32>(na),0x00050003u); CCarPathLinkAddress cl(3,5); Chk("CCarPathLinkAddress.raw",uint32(*reinterpret_cast<uint16*>(&cl)),uint32((3u<<10)|5u)); }
        { auto* v=reinterpret_cast<CVehicle*>(fakeVehStorage.data()); Chk("CVehicle.m_nCreatedBy",Off2(&v->m_nCreatedBy,v),0x4A4); Chk("CVehicle.m_nVehicleSubType",Off2(&v->m_nVehicleSubType,v),0x594);
          Chk("sizeof(eVehicleCreatedBy)",uint32(sizeof(eVehicleCreatedBy)),1); Chk("sizeof(eVehicleType)",uint32(sizeof(eVehicleType)),4); Chk("sizeof(CHeli)",uint32(sizeof(CHeli)),0xA18); }
        Chk("StrayAreasBugfixDisabled",notsa::bugfixes::CPathFind_SwitchRoadsOffInArea_StrayAreas.Enabled ? 0u : 1u,1);
        Chk("kVehSize",uint32(kVehSize),0xA18);
        Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        Snap(kPaths,kPathsSize); Snap(kGlobBegin,kGlobEnd-kGlobBegin); Snap(gPathsNeededPos,12); Snap(gExtLinkedTo,32); Snap(gString,352);
        Snap(gTimeMs,4); Snap(gTimePrevMs,4); Snap(gVehPool,4); }
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        const uint32 need=Needs(nm);
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate,Blob& blob)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A17u; if(!stt)stt=1; Rng r{stt};
                trace=json::array();
                hooks[f]->State(candidate);
                if(nm=="SwitchOffNodeAndNeighbours") hooks[idxCount]->State(false);   // el CountNeighbours del EXE conserva ECX; el de la DLL no (el llamador del EXE lo sigue usando)
                const bool switches= ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                { Patches P;
                  if(need&N_MEM){ P.Add(0x72F420,S_Malloc); P.Add(B.memAlloc,S_Malloc); P.Add(0x72F430,S_Free); }
                  if(need&N_STREAM){ P.Add(0x7ECEF0,S_RwOpen); P.Add(0x7EC9D0,S_RwRead); P.Add(0x7ECE20,S_RwClose); }
                  if(need&N_TIMER){ P.Add(0x5619D0,S_TimerSuspend); P.Add(B.timerSuspend,S_TimerSuspend); P.Add(0x561A00,S_TimerResume); P.Add(B.timerResume,S_TimerResume); }
                  if(need&N_CLOSEST) P.Add(0x44F460,S_Closest);
                  if(need&N_STREAMING){ P.Add(0x4087E0,S_Request); P.Add(B.request,S_Request); P.Add(0x4089A0,S_Remove); P.Add(B.remove,S_Remove); }
                  if(need&N_PLAYER){ P.Add(0x56E210,S_FindPed); P.Add(B.ped,S_FindPed); P.Add(0x56E010,S_FindCoors); P.Add(B.coors,S_FindCoors); }
                  if(need&N_STORAGE){ P.Add(0x5D1270,S_Save); P.Add(B.save,S_Save); P.Add(0x5D1300,S_Load); P.Add(B.load,S_Load); }
                  ResetState(r);
                  _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                  out["state"]=RunCase(nm,r);
                  const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                  out["fp"]=fp;
                  out["trace"]=trace;
                  Capture(blob,out);
                }
                RestoreRegions();
                return out;
            };
            Blob blobO, blobC;
            const auto original=run(false,blobO);
            const auto candidate=run(true,blobC);
            bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                json sd=json::array(); if(original.contains("state")&&candidate.contains("state"))for(auto& [k,v]:original["state"].items())if(candidate["state"][k]!=v)sd.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_differs",sd},{"blob_diff",BlobDiff(blobO,blobC)},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        hooks[f]->State(enabled[f]); hooks[idxCount]->State(enabled[idxCount]);
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP;
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
