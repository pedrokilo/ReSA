#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Plane.h"
#include "SurfaceInfos_c.h"
#include "AEVehicleAudioEntity.h"
#include "CarCtrl.h"
#include "World.h"
#include "Camera.h"
#include "Explosion.h"
#include "Darkel.h"
#include "VisibilityPlugins.h"
#include "Stats.h"
#include "FireManager.h"
#include "DamageManager.h"
#include "Streaming.h"
#include "TheScripts.h"
#include "GameLogic.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "PedIntelligence.h"
#include "EventDanger.h"
#include "EventGroup.h"
#include "PlayerPed.h"
#include "../ProbeRuntime.hpp"
namespace PlaneGeneration355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}

inline int scenario{};alignas(16) inline std::array<uint8,0xA18> created{};alignas(16) inline std::array<uint8,0x80> matrix{};alignas(16) inline std::array<uint8,sizeof(CPlayerPed)> player{};alignas(16) inline std::array<uint8,sizeof(CPed)> victim{};alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelligence{};alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};alignas(16) inline std::array<uint8,sizeof(CVehicle)> targetCar{};inline std::array<uint8,0x20> pool{};inline CVehicle* vehicle{};
inline json V(const CVector& value){return {std::bit_cast<uint32>(value.x),std::bit_cast<uint32>(value.y),std::bit_cast<uint32>(value.z)};}
inline uint32 __cdecl Count(){const uint32 result=scenario%8==0?6u:scenario%8==1?1u:0u;trace.push_back({"count",result});return result;}
inline bool __cdecl NoPlane(){trace.push_back({"no_plane"});return scenario%17==0;}
inline bool __cdecl NoBig(){trace.push_back({"no_big"});return scenario%19==0;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return reinterpret_cast<CPlayerPed*>(player.data());}
inline int32 __fastcall Wanted(void*,void*){int32 value=(scenario/3)%6;trace.push_back({"wanted",value});return value;}
inline CVehicle* __cdecl PlayerVehicle(int32 id,bool remote){trace.push_back({"player_vehicle",id,remote});return scenario%3==0?nullptr:reinterpret_cast<CVehicle*>(targetCar.data());}
inline CVehicle* __fastcall PoolGet(void*,void*,int32 index){trace.push_back({"pool_get",index});return scenario%5==0?nullptr:reinterpret_cast<CVehicle*>(targetCar.data());}
inline int32 __fastcall Appearance(void*,void*){const int32 value=scenario%4==0?3:scenario%4==1?5:0;trace.push_back({"appearance",value});return value;}
inline int32 __cdecl Random(){const int32 value=(scenario*7919+scenario/8)&32767;trace.push_back({"random",value});return value;}
inline bool __cdecl Mission(){trace.push_back({"mission"});return scenario%3==0;}
inline bool __cdecl Riots(){trace.push_back({"riots"});return scenario%11==0;}
inline void __cdecl Request(int32 model,int32 flags){trace.push_back({"request",model,flags});}
inline void __cdecl Creation(CVector* pos,CVector* target,float* angle,float* height,bool big){trace.push_back({"creation",big});*pos=CVector{1.25f,-2.5f,80.f};*target=CVector{4.f,5.f,6.f};*angle=float(scenario%7)*.25f;*height=125.f+float(scenario%5);if(scenario%13==0)CPlane::GenPlane_ModelIndex=520;CTimer::SetTimeInMS(CTimer::GetTimeInMS()+31);}
inline void* __cdecl Allocate(size_t size){trace.push_back({"allocate",size});return created.data();}
inline void* InitCreated(void* self,int32 model,int32 kind,bool heli){trace.push_back({"ctor",heli,model,kind});vehicle=reinterpret_cast<CVehicle*>(self);vehicle->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());vehicle->m_nModelIndex=int16(model);reinterpret_cast<uint8*>(self)[0x42D]=scenario%5==0?2:0;return self;}
inline void* __fastcall HeliCtor(void* self,void*,int32 model,int32 kind){return InitCreated(self,model,kind,true);}
inline void* __fastcall PlaneCtor(void* self,void*,int32 model,int32 kind){return InitCreated(self,model,kind,false);}
inline void __cdecl Join(CVehicle* self){trace.push_back({"join",self==vehicle});}
inline void __cdecl Add(CEntity* self){trace.push_back({"add",self==vehicle});CTimer::SetTimeInMS(CTimer::GetTimeInMS()+17);if(scenario%7==0)CPlane::GenPlane_ModelIndex=592;}
inline void __fastcall Velocity(void*,void*,CVector value){trace.push_back({"velocity",V(value)});vehicle->m_vecMoveSpeed=value;}
inline void __cdecl GearBody(){trace.push_back({"gear"});reinterpret_cast<CPlane*>(vehicle)->m_fLandingGearStatus=1.f;}
__declspec(naked) inline void Gear(){__asm { pushad } __asm { call GearBody } __asm { popad } __asm { ret }}
inline void __cdecl Deletable(int32 model){trace.push_back({"deletable",model});if(scenario%11==0)CPlane::GenPlane_ModelIndex=577;CTimer::SetTimeInMS(CTimer::GetTimeInMS()+9);}
inline void __cdecl Txd(int32 model){trace.push_back({"txd",model});CTimer::SetTimeInMS(CTimer::GetTimeInMS()+7);}
inline CVector* __cdecl Coordinates(CVector* out,int32 id){trace.push_back({"coordinates",id});if(vehicle)vehicle->GetPosition().x+=.25f;*out=CVector{2.f,3.f,4.f};return out;}
inline CPed* __cdecl TargetPed(CVector point,CVector playerPoint){trace.push_back({"target_ped",V(point),V(playerPoint)});return scenario%5==0?nullptr:reinterpret_cast<CPed*>(victim.data());}
inline CVehicle* __cdecl TargetCar(CVector point,CVector playerPoint){trace.push_back({"target_car",V(point),V(playerPoint)});return scenario%5==0?nullptr:reinterpret_cast<CVehicle*>(targetCar.data());}
inline void __fastcall Register(CEntity* self,void*,CEntity** slot){const auto offset=reinterpret_cast<uintptr_t>(slot)-reinterpret_cast<uintptr_t>(created.data());trace.push_back({"register",self==vehicle,offset<created.size()?int(offset):-1});}
inline void __fastcall Cleanup(CEntity* self,void*,CEntity**){trace.push_back({"cleanup",self==vehicle});}
inline CPedGroup* __cdecl Group(const CPed*){trace.push_back({"group"});return scenario%3==0?nullptr:reinterpret_cast<CPedGroup*>(group.data());}
inline bool __fastcall IsPlayer(void*,void*){trace.push_back({"is_player"});return scenario%7==0;}
inline json EventPayload(CEvent* base){auto* e=static_cast<CEventDanger*>(base);return json{{"time",e->m_nTimeActive},{"valid",e->m_bValid},{"add",e->m_bAddToEventGroup},{"task",int16(static_cast<eTaskType>(e->m_TaskId))},{"face",int16(static_cast<eFacialExpression>(e->m_FacialExpressionType))},{"source",e->m_dangerFrom==vehicle},{"radius",std::bit_cast<uint32>(e->m_dangerRadius)}};}
inline bool __fastcall GroupEvent(void*,void*,CEvent* event){trace.push_back({"group_event",EventPayload(event)});return true;}
inline CEvent* __fastcall PedEvent(void*,void*,CEvent* event,bool valid){trace.push_back({"ped_event",valid,EventPayload(event)});return event;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CPlane","DoPlaneGenerationAndRemoval",0x6CD2F0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"plane-generation355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"plane-generation355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plane-generation355");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 auto* hook=FindHook("CPlane","DoPlaneGenerationAndRemoval");if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
 std::array<uint8,12> savedGen;std::array<uint8,64> savedTimer;std::array<uint8,16> savedWeather;memcpy(savedGen.data(),reinterpret_cast<void*>(0xC1CAD8),12);memcpy(savedTimer.data(),reinterpret_cast<void*>(0xB7CB48),64);memcpy(savedWeather.data(),reinterpret_cast<void*>(0xC81314),16);const auto savedPool=StaticRef<void*>(0xB74494);const auto savedArea=StaticRef<int32>(0xB72914);const auto savedCut=StaticRef<uint8>(0xB5F852);const auto savedGang=StaticRef<uint8>(0xC0BC5C);const auto savedActive=CPlane::GenPlane_Active;
 static constexpr int32 models[]{497,476,512,511,520,577,592};std::array<uint8,7> savedLoad;for(size_t j=0;j<7;++j)savedLoad[j]=StaticRef<uint8>(0x8E4CD0+uintptr_t(models[j])*0x14);uint32 pairs=0,failures=0,positiveCases=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch ct(0x6CCA50,Count),ctd(bit_cast<uintptr_t>(&CPlane::CountPlanesAndHelis),Count),np(0x6CCAA0,NoPlane),npd(bit_cast<uintptr_t>(&CPlane::AreWeInNoPlaneZone),NoPlane),nb(0x6CCBB0,NoBig),nbd(bit_cast<uintptr_t>(&CPlane::AreWeInNoBigPlaneZone),NoBig);
 EntryPatch pp(0x56E210,Player),ppd(bit_cast<uintptr_t>(&FindPlayerPed),Player),wa(0x41BE60,Wanted),wad(bit_cast<uintptr_t>(&CPlayerPed::GetWantedLevel),Wanted),pv(0x56E0D0,PlayerVehicle),pvd(bit_cast<uintptr_t>(&FindPlayerVehicle),PlayerVehicle),pg(0x41CC10,PoolGet);
 EntryPatch ap(0x6D1080,Appearance),apd(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),Appearance),ar(0x821B1E,Random),ard(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random),mi(0x464D50,Mission),mid(bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission),Mission),ri(0x441C10,Riots),rid(bit_cast<uintptr_t>(&CGameLogic::LaRiotsActiveHere),Riots);
 EntryPatch rq(0x4087E0,Request),rqd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request),cr(0x6CD090,Creation),crd(bit_cast<uintptr_t>(&CPlane::FindPlaneCreationCoors),Creation),al(0x6E2D50,Allocate),hc(0x6C4190,HeliCtor),pc(0x6C8E20,PlaneCtor);
 EntryPatch jo(0x42F5A0,Join),jod(bit_cast<uintptr_t>(&CCarCtrl::JoinCarWithRoadSystem),Join),ad(0x563220,Add),addp(bit_cast<uintptr_t>(&CWorld::Add),Add),ve(0x441130,Velocity),ved(bit_cast<uintptr_t>(&CPhysical::SetVelocity),Velocity),ge(0x6CAC20,Gear),ged(bit_cast<uintptr_t>(&CPlane::SetGearUp),Gear);
 EntryPatch de(0x409C10,Deletable),ded(bit_cast<uintptr_t>(&CStreaming::SetModelIsDeletable),Deletable),tx(0x409C70,Txd),txd(bit_cast<uintptr_t>(&CStreaming::SetModelTxdIsDeletable),Txd),co(0x56E010,Coordinates),cod(bit_cast<uintptr_t>(&FindPlayerCoors),Coordinates);
 EntryPatch tp(0x566DA0,TargetPed),tpd(bit_cast<uintptr_t>(&CWorld::FindUnsuspectingTargetPed),TargetPed),tc(0x566C90,TargetCar),tcd(bit_cast<uintptr_t>(&CWorld::FindUnsuspectingTargetCar),TargetCar),re(0x571B70,Register),red(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Register),cl(0x571A00,Cleanup),cld(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Cleanup);
 EntryPatch gr(0x5F7E80,Group),grd(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),Group),ip(0x5DF8F0,IsPlayer),ipd(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer),ev(0x5F7470,GroupEvent),evd(bit_cast<uintptr_t>(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent)),GroupEvent),pe(0x4AB420,PedEvent),pedp(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),PedEvent);
 for(int i=0;i<1024;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();vehicle=nullptr;memset(created.data(),0xCD,created.size());memset(matrix.data(),0,matrix.size());memset(player.data(),0,player.size());memset(victim.data(),0,victim.size());memset(intelligence.data(),0,intelligence.size());memset(group.data(),0,group.size());memset(targetCar.data(),0,targetCar.size());memset(pool.data(),0,pool.size());*reinterpret_cast<int32*>(pool.data()+8)=(i/7)%4;StaticRef<void*>(0xB74494)=pool.data();*reinterpret_cast<CPedIntelligence**>(victim.data()+0x47C)=reinterpret_cast<CPedIntelligence*>(intelligence.data());victim[0x484]=i%13==0?2:0;*reinterpret_cast<int32*>(victim.data()+0x598)=i%23==0?6:0;group[0x2D0]=i%5==0?1:0;auto* car=reinterpret_cast<CVehicle*>(targetCar.data());car->m_nModelIndex=int16((i/5)%4==0?577:(i/5)%4==1?592:476);*reinterpret_cast<int32*>(targetCar.data()+0x594)=i%3==0?9:i%3==1?3:4;targetCar[0x3BA]=i%7==0?59:0;
 CPlane::GenPlane_Active=i%29!=0;CPlane::GenPlane_Status=uint32(i%4);CPlane::GenPlane_ModelIndex=models[(i/4)%7];CPlane::GenPlane_LastTimeGenerated=i%5==0?0xFFFFFF00u:100u;CTimer::SetFrameCounter(i%11==0?0u:30u);CTimer::SetTimeInMS(uint32(1000+i*1000));StaticRef<int32>(0xB72914)=i%31==0?1:0;StaticRef<uint8>(0xB5F852)=i%37==0?1:0;StaticRef<uint8>(0xC0BC5C)=uint8((i/7)%2);memset(reinterpret_cast<void*>(0xC81314),0,16);StaticRef<int16>(0xC81314)=int16((i/5)%5);StaticRef<int16>(0xC8131C)=i%41==0?19:0;StaticRef<int16>(0xC81320)=i%43==0?19:0;for(auto model:models)StaticRef<uint8>(0x8E4CD0+uintptr_t(model)*0x14)=i%9==0?0:1;
 hook->State(candidate);if((*reinterpret_cast<uint8*>(0x6CD2F0)==0xE9)!=candidate)return json{{"switch",false}};reinterpret_cast<void(__cdecl*)()>(0x6CD2F0)();if(!candidate&&!trace.empty())++positiveCases;return json{{"switch",true},{"created",Hash(created.data()+4,created.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"gen",Hash(reinterpret_cast<void*>(0xC1CAD8),12)},{"timer",Hash(reinterpret_cast<void*>(0xB7CB48),64)},{"trace",trace}};};const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function","CPlane::DoPlaneGenerationAndRemoval"},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});}
 }
 memcpy(reinterpret_cast<void*>(0xC1CAD8),savedGen.data(),12);memcpy(reinterpret_cast<void*>(0xB7CB48),savedTimer.data(),64);memcpy(reinterpret_cast<void*>(0xC81314),savedWeather.data(),16);StaticRef<void*>(0xB74494)=savedPool;StaticRef<int32>(0xB72914)=savedArea;StaticRef<uint8>(0xB5F852)=savedCut;StaticRef<uint8>(0xC0BC5C)=savedGang;CPlane::GenPlane_Active=savedActive;for(size_t j=0;j<7;++j)StaticRef<uint8>(0x8E4CD0+uintptr_t(models[j])*0x14)=savedLoad[j];if(!positiveCases)++failures;hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function","CPlane::DoPlaneGenerationAndRemoval"},{"cases",1024},{"failures",failures},{"positive_cases",positiveCases}});Write({{"event",failures?"error":"complete"},{"pairs",pairs},{"failures",failures},{"state_restored",hook->Hooked()==enabled&&hook->Locked()==locked}});log.close();
}
}
