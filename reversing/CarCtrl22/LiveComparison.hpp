#pragma once
// Banco dentro del motor: CCarCtrl (lista de 22). Original (hooks de CCarCtrl desactivados) frente a candidato con
// fixtures deterministas. No se crea ni se borra ningun vehiculo ni peaton real: los vehiculos son FALSOS (bytes con
// desplazamientos crudos) dentro de un pool de vehiculos FALSO (se cambia el puntero 0xB74494 y se restaura); las
// entidades objetivo y las matrices tambien son falsas. Se simulan y trazan: rand / CGeneral::GetRandomNumber,
// operator new de vehiculos, GetNewVehicleDependingOnCarModel (solo en CreateCarForScript), FindGroundZForCoord,
// GetDistanceFromCentreOfMassToBaseOfModel, PlaceOnRoadProperly, ClearSpaceForMissionEntity, CWorld::Add/Remove,
// CMissionCleanup::AddEntityToList, JoinCarWithRoadSystem(GotoCoors), FindPlayerCoors/CentreOfWorld/Wanted,
// CWanted::Are*Required, GetDefaultCopCarModel, PickGangCar, PossiblyRemoveVehicle, IsLawEnforcementVehicle,
// GenerateRoadBlockPedsForCar, CanBeDeleted, IsThisCarBeingTargettedByAnyCrane, el destructor virtual (vtable falsa),
// semaforos/puentes, CarHasReasonToStop, FindMaximumSpeedForThisCarInTraffic, ModifySpeed, AddProjectile,
// GenerateOneRandomCar, trenes, aviones, servicios de emergencia, DontCreateCivilians y LaRiotsActiveHere.
// El arreglo reversible CCarCtrl_RemoveDistantCars_UseAfterFree se desactiva durante el banco (comportamiento del EXE)
// y se restaura al final. Se guardan y restauran por ejecucion y al final: globales de CCarCtrl, grupos de coches
// cargados de CPopulation, grupos de coches, estados de carga de streaming, contadores de model infos, reloj,
// area, cinematica, jugador en foco, contador de generadores y los punteros de pool y de model infos tocados.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblebugfixes/Bugs.hpp>
#include "CarCtrl.h"
#include "GangWars.h"
#include "GameLogic.h"
#include "TheScripts.h"
#include "TrafficLights.h"
#include "TheCarGenerators.h"
#include "../ProbeRuntime.hpp"

namespace CarCtrl22Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- direcciones del EXE ---
constexpr uintptr_t VEH_POOL=0xB74494, MI_PTRS=0xA9B0C8, FRAME=0xB7CB4C, TIME_NOW=0xB7CB84, TIME_PREV=0xB7CB78, STEP=0xB7CB5C,
                    AREA=0xB72914, CUT_RUNNING=0xB5F851, FOCUS=0xB7CD74, GEN_COUNTER=0xC279D0, CLEANUP=0xA90850;
constexpr uintptr_t G1=0x969070, G2=0x8A5B20, POP=0xC0E998, CARGRP=0xC0EC78, STREAM=0x8E4CC0;
constexpr size_t G1_SIZE=0x60, G2_SIZE=0x10, POP_SIZE=0xC0EC22-0xC0E998, CARGRP_SIZE=0xC0ED38-0xC0EC78+34*46, STREAM_SIZE=20*700;
constexpr uintptr_t LOADSTATE=0x8E4CD0; // + 20 * modelo
constexpr size_t VEH_STRIDE=0xA18, NVEH=12, ENT_SIZE=0x200, NENT=4, MAT_SIZE=0x54, NMAT=16;
// desplazamientos (del EXE)
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_FLAGS=0x1C, E_SEED=0x20, E_MODEL=0x22, E_STATUS=0x36, E_SPEED=0x44,
                 AP_LANE=0x3B7, AP_STYLE=0x3B9, AP_MISSION=0x3BA, AP_TEMP=0x3BB, AP_SPEED=0x3C8, AP_MAXSPEED=0x3CC, AP_CRUISE=0x3D0,
                 AP_MOVE=0x3DC, AP_DEST=0x3EC, AP_TARGET=0x41C, V_FLAGS=0x428, V_FLAGS2=0x42A, V_RBPOS=0x4AD, V_TYPE=0x590, V_SUBTYPE=0x594, V_TRAIN=0x5B9;

// --- memoria falsa (direcciones fijas: iguales en las dos ejecuciones) ---
alignas(16) inline std::array<uint8,NVEH*VEH_STRIDE> vehStore{};
alignas(16) inline std::array<uint8,NVEH> vehStates{};
alignas(16) inline std::array<uint8,0x14> vehHdr{};
alignas(16) inline std::array<std::array<uint8,ENT_SIZE>,NENT> ents{};
alignas(16) inline std::array<std::array<uint8,MAT_SIZE>,NMAT> mats{};
alignas(16) inline std::array<std::array<uint8,0x80>,2> miBuf{};
alignas(16) inline std::array<std::array<uint8,0x30>,2> colBuf{};
alignas(16) inline std::array<uint8,0x40> wantedBuf{};
inline std::array<void*,80> fakeVt{};
inline uint8* Veh(size_t k){ return vehStore.data()+k*VEH_STRIDE; }

inline std::string Name(const void* p) {
    const auto u=uintptr_t(p); if(!u)return "null";
    struct R { uintptr_t base; size_t size, stride; const char* tag; };
    const R regions[]{
        {uintptr_t(vehStore.data()),vehStore.size(),VEH_STRIDE,"veh"},{uintptr_t(ents.data()),sizeof(ents),ENT_SIZE,"ent"},
        {uintptr_t(mats.data()),sizeof(mats),MAT_SIZE,"mat"},{uintptr_t(wantedBuf.data()),wantedBuf.size(),wantedBuf.size(),"wanted"},
        {CLEANUP,0x10,0x10,"cleanup"},
    };
    for(auto& r:regions)if(u>=r.base&&u<r.base+r.size)return std::string(r.tag)+std::to_string((u-r.base)/r.stride)+"+"+std::to_string((u-r.base)%r.stride);
    return "other";
}
inline json V3(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }

// --- RNG ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- mascaras de respuestas de los stubs ---
inline uint32 maskA{}, callsA{}, maskB{}, callsB{}, maskC{}, callsC{};
inline bool BitA(){ return (maskA>>(callsA++%32))&1; } inline bool BitB(){ return (maskB>>(callsB++%32))&1; } inline bool BitC(){ return (maskC>>(callsC++%32))&1; }
inline float floatRet{}; inline CVector playerPos{}, centrePos{}; inline void* vehResult{};

// operator new de vehiculos: siempre nullptr (no se construye ningun vehiculo)
inline void* __cdecl FakeVehNew(unsigned size){ trace.push_back({"veh_new",size}); return nullptr; }
inline CVehicle* __cdecl FakeGetNewVehicle(int32 model,uint32 createdBy){ trace.push_back({"get_new_vehicle",model,createdBy&0xFF}); return static_cast<CVehicle*>(vehResult); }
inline float __cdecl FakeGroundZ(float x,float y){ trace.push_back({"ground_z",F(x),F(y)}); return floatRet; }
inline float __fastcall FakeDistBase(void* self,void*){ trace.push_back({"dist_base",Name(self)}); return floatRet*0.25f; }
inline void __fastcall FakePlaceAuto(void* self,void*){ trace.push_back({"place_auto",Name(self)}); }
inline void __fastcall FakePlaceBike(void* self,void*){ trace.push_back({"place_bike",Name(self)}); }
inline void __cdecl FakeClearSpace(const CVector* p,CEntity* e){ trace.push_back({"clear_space",V3(*p),Name(e)}); }
inline void __cdecl FakeJoin(CVehicle* v){ trace.push_back({"join",Name(v)}); }
inline bool __cdecl FakeJoinGoto(CVehicle* v,const CVector* p,uint32 a,uint32 b){ trace.push_back({"join_goto",Name(v),Name(p),V3(*p),a&0xFF,b&0xFF}); return BitA(); }
inline void __cdecl FakeWorldAdd(CEntity* e){ trace.push_back({"world_add",Name(e)}); }
inline void __cdecl FakeWorldRemove(CEntity* e){ trace.push_back({"world_remove",Name(e)}); }
inline void __fastcall FakeAddCleanup(void* self,void*,int32 h,int32 t){ trace.push_back({"cleanup_add",Name(self),h,t&0xFF}); }
inline CVector __cdecl FakeCoors(int32 id){ trace.push_back({"player_coors",id}); return playerPos; }
inline const CVector& __cdecl FakeCentre(int32 id){ trace.push_back({"player_centre",id}); return centrePos; }
inline CWanted* __cdecl FakeWanted(int32 id){ trace.push_back({"player_wanted",id}); return reinterpret_cast<CWanted*>(wantedBuf.data()); }
inline bool __fastcall FakeSwat(void* self,void*){ trace.push_back({"swat",Name(self)}); return BitA(); }
inline bool __fastcall FakeFbi(void* self,void*){ trace.push_back({"fbi",Name(self)}); return BitB(); }
inline bool __fastcall FakeArmy(void* self,void*){ trace.push_back({"army",Name(self)}); return BitC(); }
inline int32 __cdecl FakeCopCar(int32 a){ trace.push_back({"default_cop_car",a&0xFF}); return 596+(a&1); }
inline int32 __cdecl FakePickGang(int32 g){ trace.push_back({"pick_gang_car",g}); return 400+g*3; }
inline void __cdecl FakePossiblyRemove(CVehicle* v){ trace.push_back({"possibly_remove",Name(v)}); }
inline bool __fastcall FakeLawEnf(void* self,void*){ trace.push_back({"law_enforcer",Name(self)}); return BitB(); }
inline void __cdecl FakeRoadBlock(CVehicle* v,int32 o,int32 t){ trace.push_back({"road_block",Name(v),o,t}); }
inline bool __fastcall FakeCanBeDeleted(void* self,void*){ trace.push_back({"can_be_deleted",Name(self)}); return BitA(); }
inline bool __cdecl FakeCrane(CVehicle* v){ trace.push_back({"crane",Name(v)}); return BitB(); }
inline void __fastcall FakeDtor(void* self,void*,int32 flag){ trace.push_back({"deleting_dtor",Name(self),flag}); }
inline void __fastcall FakeTrap(void* self,void*){ trace.push_back({"vtable_trap",Name(self)}); }
inline bool __cdecl FakeStopLight(CVehicle* v,uint32 b){ trace.push_back({"stop_light",Name(v),b&0xFF}); return BitA(); }
inline bool __cdecl FakeStopBridge(CVehicle* v){ trace.push_back({"stop_bridge",Name(v)}); return BitB(); }
inline void __cdecl FakeReasonToStop(CVehicle* v){ trace.push_back({"reason_to_stop",Name(v)}); }
inline float __cdecl FakeMaxTraffic(CVehicle* v){ trace.push_back({"max_traffic_speed",Name(v)}); return floatRet; }
inline void __fastcall FakeModifySpeed(void* self,void*,float t){ trace.push_back({"modify_speed",Name(self),F(t)}); }
inline bool __cdecl FakeAddProjectile(CEntity* c,int32 type,CVector pos,float force,const CVector* dir,CEntity* victim){
    trace.push_back({"add_projectile",Name(c),type,V3(pos),F(force),Name(dir),Name(victim)}); return true; }
inline void __cdecl FakeOneRandomCar(){ trace.push_back({"one_random_car"}); }
inline void __cdecl FakeTrains(){ trace.push_back({"trains"}); }
inline void __cdecl FakePlanes(){ trace.push_back({"planes"}); }
inline void __cdecl FakeEmergency(){ trace.push_back({"emergency"}); }
inline bool __cdecl FakeNoCivilians(){ trace.push_back({"no_civilians"}); return BitA(); }
inline bool __cdecl FakeRiots(){ trace.push_back({"riots"}); return BitB(); }
inline CAutomobile* __cdecl FakeOneEmergency(uint32 m,CVector p){ trace.push_back({"one_emergency",m,V3(p)}); return static_cast<CAutomobile*>(vehResult); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=22;
inline const std::array<Fn,NFN> fns{{
    {"CCarCtrl","Init",0x4212E0,0,-1,64},{"CCarCtrl","ReInit",0x4213B0,0,-1,64},{"CCarCtrl","InitSequence",0x421740,0,-1,256},
    {"CCarCtrl","ChooseBoatModel",0x421970,0,-1,256},{"CCarCtrl","ChooseCarModelToLoad",0x421900,0,-1,512},{"CCarCtrl","ChoosePoliceCarModel",0x421980,0,-1,512},
    {"CCarCtrl","ChooseGangCarModel",0x421A40,0,-1,64},{"CCarCtrl","GetNewVehicleDependingOnCarModel",0x421440,0,-1,128},{"CCarCtrl","CreateCarForScript",0x431F80,0,-1,512},
    {"CCarCtrl","IsAnyoneParking",0x42C250,0,-1,256},{"CCarCtrl","IsThisVehicleInteresting",0x423EA0,0,-1,256},{"CCarCtrl","RemoveFromInterestingVehicleList",0x423ED0,0,-1,256},
    {"CCarCtrl","JoinCarWithRoadAccordingToMission",0x432CB0,0,-1,512},{"CCarCtrl","PossiblyFireHSMissile",0x429600,0,-1,1024},{"CCarCtrl","PruneVehiclesOfInterest",0x423F10,0,-1,512},
    {"CCarCtrl","RemoveCarsIfThePoolGetsFull",0x4322B0,0,-1,512},{"CCarCtrl","RemoveDistantCars",0x42CD10,0,-1,512},{"CCarCtrl","SlowCarDownForObject",0x426220,0,-1,1024},
    {"CCarCtrl","SlowCarOnRailsDownForTrafficAndLights",0x434790,0,-1,1024},{"CCarCtrl","FindMaxSteerAngle",0x427FE0,0,-1,1024},{"CCarCtrl","GenerateRandomCars",0x4341C0,0,-1,512},
    {"CCarCtrl","ScriptGenerateOneEmergencyServicesCar",0x42FBC0,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrl22-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrl22-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrl22");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"carctrl22-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // modelos de vehiculo validos (no barco) para CreateCarForScript
    std::vector<int32> vehModels, nonBoat;
    for(int32 m=400;m<612;++m)if(CModelInfo::GetModelInfo(m)){ vehModels.push_back(m); if(!CModelInfo::IsBoatModel(m))nonBoat.push_back(m); }
    if(vehModels.empty()||nonBoat.empty()){Write({{"event","error"},{"reason","no vehicle model infos"}});return;}

    // estado global guardado
    auto Save=[](uintptr_t a,size_t n){ auto* p=reinterpret_cast<uint8*>(a); return std::vector<uint8>(p,p+n); };
    const auto sG1=Save(G1,G1_SIZE), sG2=Save(G2,G2_SIZE), sPop=Save(POP,POP_SIZE), sCarGrp=Save(CARGRP,CARGRP_SIZE), sStream=Save(STREAM,STREAM_SIZE);
    auto& frame=*reinterpret_cast<uint32*>(FRAME); auto& timeNow=*reinterpret_cast<uint32*>(TIME_NOW); auto& timePrev=*reinterpret_cast<uint32*>(TIME_PREV);
    auto& step=*reinterpret_cast<float*>(STEP); auto& area=*reinterpret_cast<int32*>(AREA); auto& cut=*reinterpret_cast<uint8*>(CUT_RUNNING);
    auto& focus=*reinterpret_cast<uint8*>(FOCUS); auto& genCounter=*reinterpret_cast<uint8*>(GEN_COUNTER); auto& vehPool=*reinterpret_cast<void**>(VEH_POOL);
    auto& mi1=reinterpret_cast<void**>(MI_PTRS)[1]; auto& mi2=reinterpret_cast<void**>(MI_PTRS)[2];
    const uint32 sFrame=frame, sTime=timeNow, sPrev=timePrev; const float sStep=step; const int32 sArea=area; const uint8 sCut=cut, sFocus=focus, sGen=genCounter;
    void* const sVehPool=vehPool; void* const sMi1=mi1; void* const sMi2=mi2;
    std::vector<std::pair<uint16,uint8>> sUsage; for(int32 m:vehModels){ auto* vmi=CModelInfo::GetVehicleModelInfo(m); sUsage.emplace_back(vmi->m_nRefCount,vmi->m_nTimesUsed); }
    auto& fixFlag=const_cast<ReversibleBugFix&>(notsa::bugfixes::CCarCtrl_RemoveDistantCars_UseAfterFree); const bool sFix=fixFlag.Enabled;
    auto RestoreAll=[&]{
        std::memcpy(reinterpret_cast<void*>(G1),sG1.data(),G1_SIZE); std::memcpy(reinterpret_cast<void*>(G2),sG2.data(),G2_SIZE);
        std::memcpy(reinterpret_cast<void*>(POP),sPop.data(),POP_SIZE); std::memcpy(reinterpret_cast<void*>(CARGRP),sCarGrp.data(),CARGRP_SIZE);
        std::memcpy(reinterpret_cast<void*>(STREAM),sStream.data(),STREAM_SIZE);
        frame=sFrame; timeNow=sTime; timePrev=sPrev; step=sStep; area=sArea; cut=sCut; focus=sFocus; genCounter=sGen; vehPool=sVehPool; mi1=sMi1; mi2=sMi2;
        for(size_t k=0;k<vehModels.size();++k){ auto* vmi=CModelInfo::GetVehicleModelInfo(vehModels[k]); vmi->m_nRefCount=sUsage[k].first; vmi->m_nTimesUsed=sUsage[k].second; }
    };
    auto Restored=[&]{
        bool ok=std::memcmp(reinterpret_cast<void*>(G1),sG1.data(),G1_SIZE)==0&&std::memcmp(reinterpret_cast<void*>(G2),sG2.data(),G2_SIZE)==0
              &&std::memcmp(reinterpret_cast<void*>(POP),sPop.data(),POP_SIZE)==0&&std::memcmp(reinterpret_cast<void*>(CARGRP),sCarGrp.data(),CARGRP_SIZE)==0
              &&std::memcmp(reinterpret_cast<void*>(STREAM),sStream.data(),STREAM_SIZE)==0
              &&frame==sFrame&&timeNow==sTime&&timePrev==sPrev&&F(step)==F(sStep)&&area==sArea&&cut==sCut&&focus==sFocus&&genCounter==sGen
              &&vehPool==sVehPool&&mi1==sMi1&&mi2==sMi2&&fixFlag.Enabled==sFix;
        for(size_t k=0;k<vehModels.size();++k){ auto* vmi=CModelInfo::GetVehicleModelInfo(vehModels[k]); ok&=vmi->m_nRefCount==sUsage[k].first&&vmi->m_nTimesUsed==sUsage[k].second; }
        return ok;
    };

    for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrap); fakeVt[0]=reinterpret_cast<void*>(&FakeDtor);
    fixFlag.Enabled=false; // Como el original: PossiblyRemoveVehicle intercalado (arreglo reversible desactivado)

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(&CCarCtrl::JoinCarWithRoadSystem),FakeJoin), s1e(0x42F5A0,FakeJoin);
        EntryPatch s2(bit_cast<uintptr_t>(&CCarCtrl::JoinCarWithRoadSystemGotoCoors),FakeJoinGoto), s2e(0x42F870,FakeJoinGoto);
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeCoors), s3e(0x56E010,FakeCoors);
        EntryPatch s4(bit_cast<uintptr_t>(static_cast<const CVector&(*)(int32)>(&FindPlayerCentreOfWorld)),FakeCentre), s4e(0x56E250,FakeCentre);
        EntryPatch s5(bit_cast<uintptr_t>(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),FakeWanted), s5e(0x56E230,FakeWanted);
        EntryPatch s6(bit_cast<uintptr_t>(&CWanted::AreSwatRequired),FakeSwat), s6e(0x561F40,FakeSwat);
        EntryPatch s7(bit_cast<uintptr_t>(&CWanted::AreFbiRequired),FakeFbi), s7e(0x561F60,FakeFbi);
        EntryPatch s8(bit_cast<uintptr_t>(&CWanted::AreArmyRequired),FakeArmy), s8e(0x561F80,FakeArmy);
        EntryPatch s9(bit_cast<uintptr_t>(&CStreaming::GetDefaultCopCarModel),FakeCopCar), s9e(0x407C50,FakeCopCar);
        EntryPatch s10(bit_cast<uintptr_t>(&CPopulation::PickGangCar),FakePickGang), s10e(0x614490,FakePickGang);
        EntryPatch s11(bit_cast<uintptr_t>(&CCarCtrl::PossiblyRemoveVehicle),FakePossiblyRemove), s11e(0x424F80,FakePossiblyRemove);
        EntryPatch s12(bit_cast<uintptr_t>(&CVehicle::IsLawEnforcementVehicle),FakeLawEnf), s12e(0x6D2370,FakeLawEnf);
        EntryPatch s13(bit_cast<uintptr_t>(&CRoadBlocks::GenerateRoadBlockPedsForCar),FakeRoadBlock), s13e(0x461170,FakeRoadBlock);
        EntryPatch s14(bit_cast<uintptr_t>(&CVehicle::CanBeDeleted),FakeCanBeDeleted), s14e(0x6D1180,FakeCanBeDeleted);
        EntryPatch s15(bit_cast<uintptr_t>(&CCranes::IsThisCarBeingTargettedByAnyCrane),FakeCrane), s15e(0x6F4000,FakeCrane);
        EntryPatch s16(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove), s16e(0x563280,FakeWorldRemove);
        EntryPatch s17(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), s17e(0x563220,FakeWorldAdd);
        EntryPatch s18(bit_cast<uintptr_t>(&CTrafficLights::ShouldCarStopForLight),FakeStopLight), s18e(0x49D610,FakeStopLight);
        EntryPatch s19(bit_cast<uintptr_t>(&CTrafficLights::ShouldCarStopForBridge),FakeStopBridge), s19e(0x49D420,FakeStopBridge);
        EntryPatch s20(bit_cast<uintptr_t>(&CCarAI::CarHasReasonToStop),FakeReasonToStop), s20e(0x41C050,FakeReasonToStop);
        EntryPatch s21(bit_cast<uintptr_t>(&CCarCtrl::FindMaximumSpeedForThisCarInTraffic),FakeMaxTraffic), s21e(0x434400,FakeMaxTraffic);
        EntryPatch s22(bit_cast<uintptr_t>(&CAutoPilot::ModifySpeed),FakeModifySpeed), s22e(0x41B980,FakeModifySpeed);
        EntryPatch s23(bit_cast<uintptr_t>(&CProjectileInfo::AddProjectile),FakeAddProjectile), s23e(0x737C80,FakeAddProjectile);
        EntryPatch s24(bit_cast<uintptr_t>(&CCarCtrl::GenerateOneRandomCar),FakeOneRandomCar), s24e(0x430050,FakeOneRandomCar);
        EntryPatch s25(bit_cast<uintptr_t>(&CTrain::DoTrainGenerationAndRemoval),FakeTrains), s25e(0x6F7900,FakeTrains);
        EntryPatch s26(bit_cast<uintptr_t>(&CPlane::DoPlaneGenerationAndRemoval),FakePlanes), s26e(0x6CD2F0,FakePlanes);
        EntryPatch s27(bit_cast<uintptr_t>(&CCarCtrl::GenerateEmergencyServicesCar),FakeEmergency), s27e(0x42F9C0,FakeEmergency);
        EntryPatch s28(bit_cast<uintptr_t>(&CGangWars::DontCreateCivilians),FakeNoCivilians), s28e(0x4439C0,FakeNoCivilians);
        EntryPatch s29(bit_cast<uintptr_t>(&CGameLogic::LaRiotsActiveHere),FakeRiots), s29e(0x441C10,FakeRiots);
        EntryPatch s30(bit_cast<uintptr_t>(&CCarCtrl::GenerateOneEmergencyServicesCar),FakeOneEmergency), s30e(0x42B7D0,FakeOneEmergency);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    RestoreAll();
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xCA22u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    callsA=callsB=callsC=0; maskA=next(); maskB=next(); maskC=next(); floatRet=0.f; vehResult=nullptr; playerPos=CVector{0.f,0.f,0.f}; centrePos=playerPos;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // ---------- piezas comunes ----------
                    vehStore.fill(0x11); vehStates.fill(0x80); for(auto& e:ents)e.fill(0x22); for(auto& m:mats)m.fill(0);
                    vehHdr.fill(0); At<void*>(vehHdr.data(),0)=vehStore.data(); At<void*>(vehHdr.data(),4)=vehStates.data(); At<int32>(vehHdr.data(),8)=int32(NVEH); At<int32>(vehHdr.data(),0xC)=-1;
                    vehPool=vehHdr.data();
                    size_t matUsed=0;
                    auto MakeMat=[&](const CVector& pos)->uint8*{ uint8* m=mats[matUsed++%NMAT].data(); std::memset(m,0,MAT_SIZE); auto* v=reinterpret_cast<float*>(m);
                        const float h=uni(-3.2f,3.2f), p=next()%3 ? 0.f : uni(-0.6f,0.6f), c=std::cos(h), s=std::sin(h), cp=std::cos(p), sp=std::sin(p);
                        v[0]=c; v[1]=s; v[2]=0.f; v[4]=-s*cp; v[5]=c*cp; v[6]=sp; v[8]=s*sp; v[9]=-c*sp; v[10]=cp; v[12]=pos.x; v[13]=pos.y; v[14]=pos.z;
                        if(next()%5==0)for(int k:{0,1,2,4,5,6,8,9,10})v[k]+=uni(-0.2f,0.2f); return m; };
                    auto InitEnt=[&](uint8* e,const CVector& pos,bool withMat){ At<CVector>(e,E_POS)=pos; At<void*>(e,E_MATRIX)=withMat ? MakeMat(pos) : nullptr; At<void*>(e,0)=fakeVt.data(); };
                    auto PosOf=[&](uint8* e)->CVector{ auto* m=At<uint8*>(e,E_MATRIX); return m ? At<CVector>(m,0x30) : At<CVector>(e,E_POS); };
                    auto UseVeh=[&](size_t k){ vehStates[k]=uint8(next()&0x7F); return Veh(k); };
                    auto VehBytes=[&](size_t k){ return Fnv(Veh(k),VEH_STRIDE); };
                    const CVector cam=TheCamera.GetPosition();

                    if(nm=="Init"||nm=="ReInit") {
                        for(size_t k=0;k<G1_SIZE;++k)At<uint8>(reinterpret_cast<void*>(G1),k)=uint8(next());
                        for(size_t k=0;k<G2_SIZE;++k)At<uint8>(reinterpret_cast<void*>(G2),k)=uint8(next());
                        for(size_t k=0;k<POP_SIZE;++k)At<uint8>(reinterpret_cast<void*>(POP),k)=uint8(next());
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["g1"]=ReversingProbe::Bytes(reinterpret_cast<void*>(G1),G1_SIZE); out["g2"]=ReversingProbe::Bytes(reinterpret_cast<void*>(G2),G2_SIZE);
                        out["pop"]=Fnv(reinterpret_cast<void*>(POP),POP_SIZE); out["pop_tail"]=*reinterpret_cast<uint8*>(0xC0EC22);
                    }
                    else if(nm=="InitSequence") {
                        int32 n=next()%3 ? int32(1+next()%64) : int32(next()); if(n==0)n=7;
                        reinterpret_cast<void(__cdecl*)(int32)>(a)(n); out["g1"]=ReversingProbe::Bytes(reinterpret_cast<void*>(G1),0xC);
                    }
                    else if(nm=="ChooseBoatModel") {
                        auto* boats=reinterpret_cast<int16*>(POP); const uint32 cnt=next()%24;
                        for(uint32 k=0;k<23;++k)boats[k]=k<cnt ? int16(vehModels[next()%vehModels.size()]) : int16(-1);
                        for(int32 m:vehModels){ auto* vmi=CModelInfo::GetVehicleModelInfo(m); vmi->m_nRefCount=uint16(next()%3); vmi->m_nTimesUsed=uint8(next()%4); }
                        ret=reinterpret_cast<int32(__cdecl*)()>(a)();
                    }
                    else if(nm=="ChooseCarModelToLoad") {
                        const int32 g=int32(next()%34); auto* counts=reinterpret_cast<uint16*>(CARGRP); auto* groups=reinterpret_cast<uint16*>(0xC0ED38);
                        counts[g]=uint16(next()%5==0 ? 0 : 1+next()%23); for(int k=0;k<23;++k)groups[g*23+k]=uint16(vehModels[next()%vehModels.size()]);
                        const uint32 bias=next()%4; for(int32 m=400;m<612;++m)*reinterpret_cast<uint8*>(LOADSTATE+20*m)=uint8(next()%4<=bias ? 1 : next()%4);
                        ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(g);
                    }
                    else if(nm=="ChoosePoliceCarModel") {
                        for(int32 m:{427,285,490,286,432,433,287})*reinterpret_cast<uint8*>(LOADSTATE+20*m)=uint8(next()%4 ? 1 : next()%3);
                        ret=reinterpret_cast<int32(__cdecl*)(uint32)>(a)(next()%2);
                    }
                    else if(nm=="ChooseGangCarModel") ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(int32(next()%10));
                    else if(nm=="GetNewVehicleDependingOnCarModel") {
                        miBuf[0].fill(0); static const int32 types[]{0,1,2,3,4,5,6,7,8,11,12,-1,100,0x10000};
                        At<int32>(miBuf[0].data(),0x3C)=types[next()%std::size(types)]; mi1=miBuf[0].data();
                        CVehicle* r;
                        { EntryPatch n1(0x6E2D50,FakeVehNew), n2(bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CVehicle::operator new)),FakeVehNew);
                          r=reinterpret_cast<CVehicle*(__cdecl*)(int32,uint32)>(a)(1,1+next()%4); }
                        ret=Name(r);
                    }
                    else if(nm=="CreateCarForScript") {
                        const size_t k=next()%NVEH; uint8* v=UseVeh(k); for(size_t o=0;o<NVEH;++o)if(o!=k&&next()%2)vehStates[o]=uint8(next()&0x7F);
                        static const int32 types[]{0,0,9,9,6,3,4,1,2,10,11};
                        At<int32>(v,V_TYPE)=types[next()%std::size(types)]; At<int32>(v,V_SUBTYPE)=int32(next()%12);
                        At<uint32>(v,E_FLAGS)=next(); At<uint8>(v,E_STATUS)=uint8(next()); At<uint8>(v,AP_MOVE)=uint8(next()); At<uint8>(v,V_FLAGS)=uint8(next());
                        At<uint8>(v,V_FLAGS2)=uint8(next()); At<uint8>(v,V_TRAIN)=uint8(next()); At<void*>(v,E_MATRIX)=next()%2 ? MakeMat(CVector{0.f,0.f,0.f}) : nullptr;
                        vehResult=v; floatRet=uni(-5.f,40.f);
                        CVector p{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-50.f,200.f)}; if(next()%4==0)p.z=next()%3 ? -100.f : uni(-300.f,-100.f);
                        const int32 model=nonBoat[next()%nonBoat.size()]; const bool cleanupFlag=next()%2;
                        CVehicle* r;
                        { EntryPatch c1(0x421440,FakeGetNewVehicle), c2(bit_cast<uintptr_t>(&CCarCtrl::GetNewVehicleDependingOnCarModel),FakeGetNewVehicle);
                          EntryPatch c3(0x569660,FakeGroundZ), c4(bit_cast<uintptr_t>(&CWorld::FindGroundZForCoord),FakeGroundZ);
                          EntryPatch c5(0x536BE0,FakeDistBase), c6(bit_cast<uintptr_t>(&CEntity::GetDistanceFromCentreOfMassToBaseOfModel),FakeDistBase);
                          EntryPatch c7(0x6AF420,FakePlaceAuto), c8(bit_cast<uintptr_t>(&CAutomobile::PlaceOnRoadProperly),FakePlaceAuto);
                          EntryPatch c9(0x6BEEB0,FakePlaceBike), c10(bit_cast<uintptr_t>(&CBike::PlaceOnRoadProperly),FakePlaceBike);
                          EntryPatch c11(0x486B00,FakeClearSpace), c12(bit_cast<uintptr_t>(&CTheScripts::ClearSpaceForMissionEntity),FakeClearSpace);
                          EntryPatch c13(0x4637E0,FakeAddCleanup), c14(bit_cast<uintptr_t>(static_cast<void(CMissionCleanup::*)(int32,MissionCleanUpEntityType)>(&CMissionCleanup::AddEntityToList)),FakeAddCleanup);
                          r=reinterpret_cast<CVehicle*(__cdecl*)(int32,CVector,bool)>(a)(model,p,cleanupFlag); }
                        ret=Name(r); out["veh"]=VehBytes(k); out["mat"]=Fnv(mats.data(),sizeof(mats));
                    }
                    else if(nm=="IsAnyoneParking") {
                        At<int32>(vehHdr.data(),8)=int32(next()%(NVEH+1)); static const int8 ms[]{45,46,49,50,0,1,8,47,48,51,44,-3};
                        for(size_t k=0;k<NVEH;++k){ if(next()%2)vehStates[k]=uint8(next()&0x7F); At<int8>(Veh(k),AP_MISSION)=next()%4 ? ms[4+next()%8] : ms[next()%std::size(ms)]; }
                        ret=reinterpret_cast<bool(__cdecl*)()>(a)();
                    }
                    else if(nm=="IsThisVehicleInteresting"||nm=="RemoveFromInterestingVehicleList") {
                        void* pick[]{Veh(0),Veh(1),nullptr,Veh(0)}; auto** keep=reinterpret_cast<void**>(0x969084);
                        keep[0]=pick[next()%4]; keep[1]=pick[next()%4]; void* arg=next()%5==0 ? nullptr : Veh(next()%3);
                        if(nm=="IsThisVehicleInteresting")ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(arg); else reinterpret_cast<void(__cdecl*)(void*)>(a)(arg);
                        out["keep"]=json::array({Name(keep[0]),Name(keep[1])});
                    }
                    else if(nm=="JoinCarWithRoadAccordingToMission") {
                        uint8* v=UseVeh(0); InitEnt(v,CVector{uni(-500.f,500.f),uni(-500.f,500.f),uni(0.f,50.f)},next()%2);
                        At<int8>(v,AP_MISSION)=next()%6==0 ? int8(next()) : int8(next()%69); At<int32>(v,V_SUBTYPE)=next()%3==0 ? 5 : int32(next()%12);
                        At<CVector>(v,AP_DEST)=CVector{uni(-900.f,900.f),uni(-900.f,900.f),uni(-10.f,90.f)};
                        uint8* t=next()%2 ? ents[0].data() : UseVeh(1); InitEnt(t,CVector{uni(-500.f,500.f),uni(-500.f,500.f),uni(0.f,50.f)},next()%2);
                        At<void*>(v,AP_TARGET)=next()%4==0 ? nullptr : t; playerPos=CVector{uni(-900.f,900.f),uni(-900.f,900.f),uni(0.f,40.f)};
                        reinterpret_cast<void(__cdecl*)(void*)>(a)(v);
                    }
                    else if(nm=="PossiblyFireHSMissile") {
                        uint8* v=UseVeh(0); const CVector lp{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(0.f,300.f)}; InitEnt(v,lp,true);
                        auto* m=At<uint8*>(v,E_MATRIX); const CVector fwd=At<CVector>(m,0x10);
                        CVector d; switch(next()%3){ case 0: d=fwd; break; case 1: d=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; break; default: d=fwd+CVector{uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-0.5f,0.5f)}; }
                        float dist=uni(0.f,200.f); if(next()%10==0)dist=next()%2 ? 30.f : 160.f;
                        const float len=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z); const CVector tp=len>0.001f ? lp+d*(dist/len) : lp;
                        uint8* t=next()%2 ? ents[0].data() : UseVeh(1); InitEnt(t,tp,next()%2);
                        timeNow=next()%100000000; timePrev=timeNow-next()%3000; if(next()%4==0)timePrev=timeNow;
                        reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(v,next()%8==0 ? nullptr : t);
                    }
                    else if(nm=="PruneVehiclesOfInterest") {
                        frame=next()%3 ? 19+64*(next()%100000) : next(); playerPos=CVector{0.f,0.f,next()%5==0 ? 950.f : uni(900.f,1000.f)};
                        timeNow=200000+next()%100000000; auto** keep=reinterpret_cast<void**>(0x969084); auto* times=reinterpret_cast<uint32*>(0x96907C);
                        for(int k=0;k<2;++k){ keep[k]=next()%4 ? Veh(k) : nullptr; times[k]=next()%6==0 ? timeNow-180000 : timeNow-180000+int32(next()%20000)-10000; }
                        reinterpret_cast<void(__cdecl*)()>(a)(); out["keep"]=json::array({Name(keep[0]),Name(keep[1]),times[0],times[1]});
                    }
                    else if(nm=="RemoveCarsIfThePoolGetsFull") {
                        frame=next()%4 ? 3+8*(next()%100000) : next(); At<int32>(vehHdr.data(),8)=int32(1+next()%NVEH);
                        std::vector<CVector> used;
                        for(size_t k=0;k<NVEH;++k){ if(next()%6)vehStates[k]=uint8(next()&0x7F); uint8* v=Veh(k);
                            CVector p=cam+CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-50.f,50.f)}; if(next()%10==0)p=cam+CVector{uni(1e7f,2e7f),0.f,0.f};
                            if(!used.empty()&&next()%4==0)p=used[next()%used.size()]; used.push_back(p);
                            InitEnt(v,p,next()%2); At<uint8>(v,V_FLAGS)=uint8(next()%4 ? next()&0xF7 : next()); }
                        auto** keep=reinterpret_cast<void**>(0x969084); keep[0]=next()%3 ? nullptr : Veh(next()%NVEH); keep[1]=next()%3 ? nullptr : Veh(next()%NVEH);
                        if(next()%2)maskA|=next(); // mas vehiculos borrables
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="RemoveDistantCars") {
                        At<int32>(vehHdr.data(),8)=int32(1+next()%NVEH); focus=uint8(next()%2);
                        centrePos=CVector{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(0.f,100.f)};
                        for(size_t k=0;k<NVEH;++k){ if(next()%4)vehStates[k]=uint8(next()&0x7F); uint8* v=Veh(k);
                            const float r=next()%10==0 ? 54.5f : uni(0.f,80.f), h=uni(-3.2f,3.2f);
                            InitEnt(v,centrePos+CVector{r*std::cos(h),r*std::sin(h),uni(-200.f,200.f)},next()%2);
                            At<uint8>(v,V_FLAGS2)=uint8(next()); At<uint8>(v,V_RBPOS)=uint8(next()%3); }
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        json fl=json::array(); for(size_t k=0;k<NVEH;++k)fl.push_back(At<uint8>(Veh(k),V_FLAGS2)); out["flags"]=fl;
                    }
                    else if(nm=="SlowCarDownForObject") {
                        for(int k=0;k<2;++k){ miBuf[k].fill(0); colBuf[k].fill(0); At<void*>(miBuf[k].data(),0x14)=colBuf[k].data();
                            At<float>(colBuf[k].data(),0x0C)=uni(0.2f,4.f); At<float>(colBuf[k].data(),0x24)=uni(0.2f,6.f); }
                        mi1=miBuf[0].data(); mi2=miBuf[1].data();
                        uint8* v=UseVeh(0); const CVector vp{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(0.f,100.f)}; InitEnt(v,vp,true); At<int16>(v,E_MODEL)=int16(1+next()%2);
                        auto* m=At<uint8*>(v,E_MATRIX); const CVector R=At<CVector>(m,0), Fw=At<CVector>(m,0x10), U=At<CVector>(m,0x20);
                        float h=uni(-5.f,25.f); if(next()%8==0){ static const float hs[]{0.f,7.f,20.f}; h=hs[next()%3]; }
                        const CVector ep=vp+Fw*h+R*uni(-10.f,10.f)+U*uni(-3.f,3.f);
                        uint8* e=next()%2 ? ents[0].data() : UseVeh(1); InitEnt(e,ep,next()%2); At<int16>(e,E_MODEL)=int16(1+next()%2);
                        float s=next()%6==0 ? 0.f : uni(0.f,1.5f); const float k4=next()%6==0 ? 1.f : uni(0.f,2.f);
                        reinterpret_cast<void(__cdecl*)(void*,void*,float*,float)>(a)(e,v,&s,k4); out["speed"]=F(s);
                    }
                    else if(nm=="SlowCarOnRailsDownForTrafficAndLights") {
                        uint8* v=UseVeh(0); At<uint8>(v,E_SEED)=uint8(next()); frame=next(); step=next()%5==0 ? 1.f : uni(0.f,3.f);
                        static const float sp[]{0.f,0.1f,0.05f,1.f}; float speed=next()%4==0 ? sp[next()%4] : uni(0.f,2.f);
                        float mx=next()%5==0 ? speed : uni(0.f,2.f); if(next()%8==0)mx=0.f;
                        At<float>(v,AP_SPEED)=speed; At<float>(v,AP_MAXSPEED)=mx; floatRet=next()%4==0 ? speed : uni(0.f,2.f);
                        reinterpret_cast<void(__cdecl*)(void*)>(a)(v); out["ap"]=ReversingProbe::Bytes(v+0x390,0x70);
                    }
                    else if(nm=="FindMaxSteerAngle") {
                        uint8* v=UseVeh(0); CVector s{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.3f,0.3f)};
                        switch(next()%6){ case 0: s=CVector{0.7f,0.f,0.f}; break; case 1: s=s*0.3f; break; case 2: s=CVector{uni(0.68f,0.72f),0.f,0.f}; break; case 3: s=CVector{0.f,0.f,0.f}; break; default: break; }
                        At<CVector>(v,E_SPEED)=s; ret=F(reinterpret_cast<float(__cdecl*)(void*)>(a)(v));
                    }
                    else if(nm=="GenerateRandomCars") {
                        cut=next()%5==0 ? 1 : 0; area=next()%4 ? 0 : int32(1+next()%18);
                        auto& tm=*reinterpret_cast<float*>(0x9690BC); tm=next()%6==0 ? 480.f : uni(0.f,1300.f);
                        *reinterpret_cast<int32*>(0x969094)=int32(30+next()%20); if(next()%6==0)*reinterpret_cast<int32*>(0x969094)=45;
                        *reinterpret_cast<int8*>(0x9690C0)=int8(next()%4); step=uni(0.f,3.f); genCounter=uint8(next());
                        timeNow=next(); timePrev=next()%2 ? timeNow-next()%100 : timeNow-next()%10000;
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["g1"]=ReversingProbe::Bytes(reinterpret_cast<void*>(G1),G1_SIZE); out["gen"]=genCounter;
                    }
                    else if(nm=="ScriptGenerateOneEmergencyServicesCar") {
                        uint8* v=UseVeh(0); vehResult=next()%4 ? v : nullptr; const int32 model=vehModels[next()%vehModels.size()];
                        *reinterpret_cast<uint8*>(LOADSTATE+20*model)=uint8(next()%4 ? 1 : next()%4);
                        CVector p{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-10.f,100.f)};
                        ret=reinterpret_cast<bool(__cdecl*)(uint32,CVector)>(a)(uint32(model),p); out["veh"]=VehBytes(0);
                    }
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreAll();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll(); fixFlag.Enabled=sFix;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Restored()}});
}
}
