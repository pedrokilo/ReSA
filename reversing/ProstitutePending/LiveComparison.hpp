#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexProstituteSolicit.h"
#include "PlayerPedData.h"
#include "PlayerInfo.h"
#include "CarEnterExit.h"
#include "Ragdoll/IKChainManager.h"
#include "../ProbeRuntime.hpp"
namespace ProstitutePendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskComplexProstituteSolicit","CreateSubTask",0x666360,0,-1,512},
{"CTaskComplexProstituteSolicit","CreateFirstSubTask",0x6666A0,0x86FB88,9,512},
{"CTaskComplexProstituteSolicit","CreateNextSubTask",0x666780,0x86FB88,8,1024}};
alignas(16)inline std::array<uint8,sizeof(CPed)> ped,client;
alignas(16)inline std::array<uint8,sizeof(CPlayerPedData)> data;
alignas(16)inline std::array<uint8,sizeof(CPlayerInfo)> info;
alignas(16)inline std::array<uint8,sizeof(CAutomobile)> vehicle;
alignas(16)inline std::array<uint8,sizeof(CTaskComplexProstituteSolicit)> task;
alignas(16)inline std::array<uint8,128> subtask;
inline CMatrix matrix;
alignas(16)inline std::array<std::array<uint8,256>,8> arena;
inline std::array<size_t,8> sizes{};inline int allocCount{},caseId{},subType{};inline bool valid{};
inline std::array<void*,11> subVT{};inline std::array<GxtChar,8> message{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;auto in=[&](const void* b,size_t n){return p>=uintptr_t(b)&&p<uintptr_t(b)+n;};if(in(ped.data(),ped.size()))return json::array({"ped",p-uintptr_t(ped.data())});if(in(client.data(),client.size()))return json::array({"client",p-uintptr_t(client.data())});if(in(data.data(),data.size()))return json::array({"player_data",p-uintptr_t(data.data())});if(in(task.data(),task.size()))return json::array({"task",p-uintptr_t(task.data())});if(in(subtask.data(),subtask.size()))return json::array({"subtask",p-uintptr_t(subtask.data())});if(in(vehicle.data(),vehicle.size()))return json::array({"vehicle",p-uintptr_t(vehicle.data())});if(in(&matrix,sizeof(matrix)))return json::array({"matrix",p-uintptr_t(&matrix)});for(int k=0;k<8;++k)if(in(arena[k].data(),256))return json::array({"allocation",k,p-uintptr_t(arena[k].data())});return p;}
inline json Words(const void* p,size_t n,bool vptr=false){json j=json::array();auto* w=static_cast<const uint32*>(p);for(size_t i=vptr?1:0;i<n/4;++i)j.push_back(Ptr(w[i]));return j;}
inline void* __cdecl New(size_t n){trace.push_back({"allocate",n});if(allocCount>=8||n>256)return nullptr;sizes[allocCount]=n;return arena[allocCount++].data();}
inline int __fastcall Type(void*,void*){trace.push_back({"subtask_type",subType});return subType;}
inline void __fastcall Ref(CEntity* e,void*,CEntity** slot){trace.push_back({"register_reference",Ptr(uintptr_t(e)),Ptr(uintptr_t(slot))});}
inline void __fastcall Unref(CEntity* e,void*,CEntity** slot){trace.push_back({"cleanup_reference",Ptr(uintptr_t(e)),Ptr(uintptr_t(slot))});}
inline bool __cdecl Valid(CPed* p,CPed* c){trace.push_back({"valid",Ptr(uintptr_t(p)),Ptr(uintptr_t(c)),valid});return valid;}
inline CPlayerPed* __cdecl Player(int id){trace.push_back({"find_player",id});return reinterpret_cast<CPlayerPed*>(client.data());}
inline CPlayerInfo* __fastcall Info(CPlayerPed*,void*){trace.push_back({"player_info"});return reinterpret_cast<CPlayerInfo*>(info.data());}
inline CVector __cdecl Door(const CVehicle* v,int door){trace.push_back({"door",Ptr(uintptr_t(v)),door});return door==10?CVector{-3.f,2.f,1.f}:CVector{3.f,2.f,1.f};}
inline void __fastcall Look(IKChainManager_c*,void*,const char* label,CPed* p,CEntity* e,int time,eBoneTag32 bone,CVector* offset,bool torso,float speed,int blend,int priority,bool force){trace.push_back({"look",std::string(label),Ptr(uintptr_t(p)),Ptr(uintptr_t(e)),time,int(bone),offset==nullptr,torso,std::bit_cast<uint32>(speed),blend,priority,force});}
inline int16 __fastcall Say(CPed* p,void*,int ctx,uint32 delay,float prob,bool a,bool b,bool c){trace.push_back({"say",Ptr(uintptr_t(p)),ctx,delay,std::bit_cast<uint32>(prob),a,b,c});return 0;}
inline const GxtChar* __fastcall Text(CText*,void*,const char* key){trace.push_back({"text",std::string(key)});return message.data();}
inline void __cdecl AddMessage(const GxtChar*,uint32 time,uint16 flag,bool brief){trace.push_back({"message",time,flag,brief});}
inline void __cdecl Clear(bool flag){trace.push_back({"clear_messages",flag});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"prostitute-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"prostitute-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("prostitute-pending");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 std::string only;{std::ifstream f(dir/"prostitute-pending-only.txt");if(f)std::getline(f,only);}
 subVT[4]=reinterpret_cast<void*>(&Type);const auto savedCheat=*reinterpret_cast<uint8*>(0x96918A);bool pass=true,selected=false;int total=0;
 for(const auto& fn:fns){const auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto* h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}const bool hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();
  {
   using ReversingProbe::EntryPatch;using std::bit_cast;
   EntryPatch ne(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New);
   EntryPatch refe(0x571B70,Ref),refd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref),ure(0x571A00,Unref),urd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Unref);
   EntryPatch ve(0x661BB0,Valid),vd(bit_cast<uintptr_t>(&CTaskComplexProstituteSolicit::IsTaskValid),Valid);
   EntryPatch pe(0x56E210,Player),pd(bit_cast<uintptr_t>(&FindPlayerPed),Player),ie(0x609FF0,Info),id(bit_cast<uintptr_t>(&CPlayerPed::GetPlayerInfoForThisPlayerPed),Info);
   EntryPatch de(0x64E740,Door),dd(bit_cast<uintptr_t>(&CCarEnterExit::GetPositionToOpenCarDoor),Door);
   EntryPatch le(0x618970,Look),ld(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),Look);
   EntryPatch spe(0x5EFFE0,Say),spd(bit_cast<uintptr_t>(&CPed::Say),Say);
   EntryPatch te(0x6A0050,Text),td(bit_cast<uintptr_t>(&CText::Get),Text),ame(0x69F0B0,AddMessage),amd(bit_cast<uintptr_t>(&CMessages::AddMessageQ),AddMessage),ce(0x69DCD0,Clear),cd(bit_cast<uintptr_t>(&CMessages::ClearMessages),Clear);
   for(int i=0;i<fn.cases;++i){caseId=i;const int scenario=i%16;json result[2];
    for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+fn.slot*4);const bool switches=fn.slot<0?ReversingProbe::JumpsToDll(address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);pass&=switches;
     ped.fill(0);client.fill(0);data.fill(0);info.fill(0);vehicle.fill(0);task.fill(0xCD);subtask.fill(0xCD);for(auto& a:arena)a.fill(0xCD);sizes.fill(0);allocCount=0;trace=json::array();
     auto* t=reinterpret_cast<CTaskComplexProstituteSolicit*>(task.data());Put<uintptr_t>(task.data(),4,0);Put(task.data(),8,reinterpret_cast<CTask*>(subtask.data()));Put(subtask.data(),0,subVT.data());t->m_pClient=fn.slot==8&&scenario==0?nullptr:reinterpret_cast<CPed*>(client.data());
     Put(client.data(),0x480,data.data());Put(client.data(),0x58C,vehicle.data());Put<uint8>(client.data(),0x36,3);Put<uint8>(ped.data(),0x36,3);Put<uint8>(vehicle.data(),0x36,2);
     matrix.SetScale(1.f);matrix.GetPosition()=CVector{float(i%7),float(i%13),float(i%3)};Put(vehicle.data(),0x14,&matrix);
     Put(ped.data(),4,CVector{float(scenario%3-1)*5.f,float(i%11),1.f});Put(data.data(),0xA4,scenario%3==0?reinterpret_cast<CPed*>(ped.data()):nullptr);Put(data.data(),0xA8,scenario%3==1?reinterpret_cast<CPed*>(client.data()):scenario%3==2?reinterpret_cast<CPed*>(ped.data()):nullptr);
     valid=scenario!=1;t->bPlayerHasAcceptedSexProposition=((i/16)&1)!=0;*reinterpret_cast<uint8*>(0x96918A)=uint8(((i/16)&2)!=0);Put<int32>(info.data(),0xB8,((i/16)&4)?20:19);
     constexpr int types[]{TASK_FINISHED,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_CAR_DRIVE,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_TURN_TO_FACE_ENTITY,12345};subType=types[i%8];
     CTask* ret;if(fn.slot<0)ret=reinterpret_cast<CTask*(__thiscall*)(void*,int,CPed*)>(address)(t,subType,reinterpret_cast<CPed*>(ped.data()));else ret=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,reinterpret_cast<CPed*>(ped.data()));
     if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};hit(("task_"+std::to_string(subType)).c_str());if(fn.slot==9)hit(valid?"valid_client":"invalid_client");if(fn.slot==8){if(!t->m_pClient)hit("null_client");if(t->bTaskCanBeFinished)hit("invalid_or_finished");for(const auto& e:trace){if(e[0]=="look")hit("look_at");if(e[0]=="message")hit("message");if(e[0]=="say")hit("speech");}}}
     json allocations=json::array();for(int k=0;k<allocCount;++k)allocations.push_back({{"size",sizes[k]},{"bytes",Words(arena[k].data(),sizes[k],true)}});
     result[candidate]={{"return",Ptr(uintptr_t(ret))},{"task",Words(task.data(),task.size(),true)},{"player_data",Words(data.data(),data.size())},{"allocations",allocations},{"trace",trace},{"switch",switches}};
    }
    const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
   }
  }
  *reinterpret_cast<uint8*>(0x96918A)=savedCheat;h->State(hs);h->LockState(hl);
  auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};
  if(fn.slot==9){require("valid_client");require("invalid_client");}else if(fn.slot==8){for(const auto* n:{"null_client","look_at","message","speech"})require(n);}else{for(const int k:{TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_CAR_DRIVE,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_TURN_TO_FACE_ENTITY})require(("task_"+std::to_string(k)).c_str());}
  Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
 }
 Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",*reinterpret_cast<uint8*>(0x96918A)==savedCheat}});
}
}
