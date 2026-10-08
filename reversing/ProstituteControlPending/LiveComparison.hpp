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
namespace ProstituteControlPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskComplexProstituteSolicit","ControlSubTask",0x6669D0,0x86FB88,10,1024}};
alignas(16)inline std::array<uint8,sizeof(CPed)> ped,client;
alignas(16)inline std::array<uint8,sizeof(CPlayerPedData)> data;
alignas(16)inline std::array<uint8,sizeof(CPlayerInfo)> info;
alignas(16)inline std::array<uint8,sizeof(CAutomobile)> vehicle;
alignas(16)inline std::array<uint8,sizeof(CTaskComplexProstituteSolicit)> task;
alignas(16)inline std::array<uint8,128> subtask;
inline CMatrix matrix;
alignas(16)inline std::array<uint8,sizeof(CPedIntelligence)> intel;
alignas(16)inline std::array<uint8,sizeof(CWanted)> wanted;
alignas(16)inline std::array<uint8,sizeof(CPed)> observer;
inline int scenario{},rngCalls{};inline bool aborted{};
alignas(16)inline std::array<std::array<uint8,256>,8> arena;
inline std::array<size_t,8> sizes{};inline int allocCount{},caseId{},subType{};inline bool valid{};
inline std::array<void*,11> subVT{},taskVT{};inline std::array<GxtChar,8> message{};
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
inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){trace.push_back({"abort",priority,aborted});return aborted;}
inline CTask* __fastcall Next(CTaskComplexProstituteSolicit*,void*,CPed*){trace.push_back({"next"});return reinterpret_cast<CTask*>(arena[0].data());}
inline CWanted* __cdecl Wanted(int id){trace.push_back({"wanted",id});return reinterpret_cast<CWanted*>(wanted.data());}
inline CEntity** __fastcall Entities(CPedIntelligence*,void*){return reinterpret_cast<CPedIntelligence*>(intel.data())->GetPedScanner().m_apEntities.data();}
inline void __fastcall WantedLevel(CWanted*,void*,eWantedLevel n){trace.push_back({"wanted_level",int(n)});}
inline void __cdecl Mood(eCJMood n,uint32 duration,int16 a,int16 b,int16 c){trace.push_back({"mood",int(n),duration,a,b,c});}
inline void __cdecl Stat(eStats n,float v){trace.push_back({"stat",int(n),std::bit_cast<uint32>(v)});}
inline int16 __fastcall Accelerate(const CPad*,void*){trace.push_back({"accelerate",scenario==29});return scenario==29;}
inline int16 __fastcall Brake(const CPad*,void*){trace.push_back({"brake",scenario==30});return scenario==30;}
inline void __fastcall Shake(CPad*,void*,int16 a,uint8 b,uint32 c){trace.push_back({"shake",a,b,c});}
inline int __cdecl Rand(){const int r=int((caseId*15731u+(rngCalls++)*14543u)&32767);trace.push_back({"rand",r});return r;}
inline uint16 Random(){return uint16(Rand());}
inline void __fastcall Force(CVehicle*,void*,float x,float y,float z,float px,float py,float pz){trace.push_back({"force",std::bit_cast<uint32>(x),std::bit_cast<uint32>(y),std::bit_cast<uint32>(z),std::bit_cast<uint32>(px),std::bit_cast<uint32>(py),std::bit_cast<uint32>(pz)});}
inline void __fastcall Audio(CAEVehicleAudioEntity*,void*,eAudioEvents n,float v){trace.push_back({"audio",int(n),std::bit_cast<uint32>(v)});}
inline void __fastcall Health(const CPlayerInfo*,void*,int n){trace.push_back({"health",n});}
inline bool __cdecl LOS(const CVector& a,const CVector& b,bool c,bool d,bool e,bool f,bool g,bool h,bool j){const bool clear=scenario!=27;trace.push_back({"los",std::bit_cast<uint32>(a.x),std::bit_cast<uint32>(a.y),std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z),c,d,e,f,g,h,j,clear});return clear;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"prostitute-control-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"prostitute-control-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("prostitute-control-pending");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 std::string only;{std::ifstream f(dir/"prostitute-control-pending-only.txt");if(f)std::getline(f,only);}
 subVT[4]=reinterpret_cast<void*>(&Type);subVT[6]=reinterpret_cast<void*>(&Abort);const auto savedTime=CTimer::GetTimeInMS();const auto savedStep=CTimer::GetTimeStep();const auto savedCamera=StaticRef<int32>(0xB6F0D8);std::array<uint8,sizeof(CPad)> savedPad;std::memcpy(savedPad.data(),CPad::GetPad(0),savedPad.size());const auto savedCheat=*reinterpret_cast<uint8*>(0x96918A);bool pass=true,selected=false;int total=0;
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
   EntryPatch nx(0x666780,Next),nxd(bit_cast<uintptr_t>(&CTaskComplexProstituteSolicit::CreateNextSubTask),Next),nxv(reinterpret_cast<uintptr_t*>(0x86FB88)[8],Next);
   EntryPatch wa(0x56E230,Wanted),wad(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted);
   EntryPatch en(0x4893E0,Entities);
   EntryPatch wl(0x562470,WantedLevel),wld(bit_cast<uintptr_t>(&CWanted::SetWantedLevel),WantedLevel);
   EntryPatch mo(0x4E3ED0,Mood),mod(bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::SetCJMood),Mood);
   EntryPatch st(0x55C180,Stat),stdp(bit_cast<uintptr_t>(&CStats::IncrementStat),Stat);
   EntryPatch ac(0x5403F0,Accelerate),acd(bit_cast<uintptr_t>(&CPad::GetAccelerate),Accelerate);
   EntryPatch br(0x540080,Brake),brd(bit_cast<uintptr_t>(&CPad::GetBrake),Brake);
   EntryPatch sh(0x53F920,Shake),shd(bit_cast<uintptr_t>(&CPad::StartShake),Shake);
   EntryPatch rn(0x821B1E,Rand),rnd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
   EntryPatch fo(0x65DA70,Force);
   EntryPatch au(0x4F6420,Audio),aud(bit_cast<uintptr_t>(static_cast<void(CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent)),Audio);
   EntryPatch he(0x56EAB0,Health),hed(bit_cast<uintptr_t>(&CPlayerInfo::AddHealth),Health);
   EntryPatch los(0x56A490,LOS),losd(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),LOS);
   for(int i=0;i<fn.cases;++i){caseId=i;scenario=i%32;json result[2];
    for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+fn.slot*4);const bool switches=fn.slot<0?ReversingProbe::JumpsToDll(address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);pass&=switches;
     ped.fill(0);client.fill(0);data.fill(0);info.fill(0);vehicle.fill(0);task.fill(0xCD);subtask.fill(0xCD);for(auto& a:arena)a.fill(0xCD);sizes.fill(0);allocCount=0;trace=json::array();rngCalls=0;intel.fill(0);wanted.fill(0);observer.fill(0);std::memset(CPad::GetPad(0),0,sizeof(CPad));
     auto* t=reinterpret_cast<CTaskComplexProstituteSolicit*>(task.data());std::memcpy(taskVT.data(),reinterpret_cast<void*>(0x86FB88),11*4);taskVT[8]=reinterpret_cast<void*>(&Next);Put(task.data(),0,taskVT.data());Put<uintptr_t>(task.data(),4,0);Put(task.data(),8,reinterpret_cast<CTask*>(subtask.data()));Put(subtask.data(),0,subVT.data());t->m_pClient=fn.slot==8&&scenario==0?nullptr:reinterpret_cast<CPed*>(client.data());
     Put(client.data(),0x480,data.data());Put(client.data(),0x58C,vehicle.data());Put<uint8>(client.data(),0x36,3);Put<uint8>(ped.data(),0x36,3);Put<uint8>(vehicle.data(),0x36,2);
     matrix.SetScale(1.f);matrix.GetPosition()=CVector{float(i%7),float(i%13),float(i%3)};Put(vehicle.data(),0x14,&matrix);
     Put(ped.data(),4,CVector{float(scenario%3-1)*5.f,float(i%11),1.f});Put(data.data(),0xA4,scenario%3==0?reinterpret_cast<CPed*>(ped.data()):nullptr);Put(data.data(),0xA8,scenario%3==1?reinterpret_cast<CPed*>(client.data()):scenario%3==2?reinterpret_cast<CPed*>(ped.data()):nullptr);
     valid=scenario!=1;aborted=(scenario&1)!=0;Put(ped.data(),0x47C,intel.data());Put(data.data(),0,wanted.data());auto* pad=CPad::GetPad(0);pad->NewState.DPadRight=scenario==2;pad->NewState.DPadLeft=scenario==3;
     *reinterpret_cast<uint16*>(task.data()+0x2C)=0;t->bSearchingForSecludedPlace=scenario>=8&&scenario<=15;t->bSexProcessStarted=scenario>=16;t->bTaskCanBeFinished=scenario>=4&&scenario<=7;t->bMoveCameraDown=(scenario&1)!=0;t->b08=scenario==8;t->b07=scenario==16;t->b10=scenario==20;t->bVehicleShifted=(scenario&2)!=0;t->bSecludedPlaceMessageShown=scenario==15;
     t->m_nCurrentTimer=int16(scenario==4?10:scenario==5?100:scenario==6||scenario==7?0:scenario==20?500:15000);t->m_nVehicleMovementTimer=int16(scenario%3==0||scenario==20?0:200);t->m_nLastSavedTime=1000;t->m_nNextTimeToCheckForSecludedPlace=scenario==18?13000:0;t->m_nLastPaymentTime=scenario==19?12000:0;t->m_vecVehiclePosn=scenario<4?CVector{}:matrix.GetPosition();
     *reinterpret_cast<uint8*>(0x96918A)=uint8((i/32)&1);Put<int32>(info.data(),0xB8,scenario==21?1:20);*reinterpret_cast<uint32*>(0xB7CB84)=12000;CTimer::SetTimeStep(scenario==5?.25f:1.f);StaticRef<int32>(0xB6F0D8)=scenario==9?1:0;
     auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());veh->m_vecMoveSpeed=CVector{scenario==10?.02f:0.f,0.f,0.f};veh->m_fMass=1500.f;reinterpret_cast<CPed*>(client.data())->m_fMass=80.f;
     auto* o=reinterpret_cast<CPed*>(observer.data());o->m_nPedType=scenario==26||scenario==27?PED_TYPE_COP:PED_TYPE_CIVMALE;o->GetPosition()=CVector{scenario==13||scenario==28?100.f:scenario==14?10.f:1.f,0.f,0.f};auto& scanned=reinterpret_cast<CPedIntelligence*>(intel.data())->GetPedScanner().m_apEntities;if(scenario==13||scenario==14||scenario==15||scenario>=26)scanned[0]=o;
     reinterpret_cast<CWanted*>(wanted.data())->m_WantedLevel=eWantedLevel(0);
     constexpr int types[]{TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_CAR_DRIVE};subType=types[(i/32)%4];
     CTask* ret;if(fn.slot<0)ret=reinterpret_cast<CTask*(__thiscall*)(void*,int,CPed*)>(address)(t,subType,reinterpret_cast<CPed*>(ped.data()));else ret=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,reinterpret_cast<CPed*>(ped.data()));
     if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};hit(("task_"+std::to_string(subType)).c_str());hit(t->bTaskCanBeFinished?"finish_flag":"active");if(t->bSexProcessStarted)hit("sex");for(const auto& e:trace)if(e[0].is_string())hit(e[0].get<std::string>().c_str());}
     json allocations=json::array();for(int k=0;k<allocCount;++k)allocations.push_back({{"size",sizes[k]},{"bytes",Words(arena[k].data(),sizes[k],true)}});
     result[candidate]={{"return",Ptr(uintptr_t(ret))},{"task",Words(task.data(),task.size(),true)},{"player_data",Words(data.data(),data.size())},{"info",Words(info.data(),info.size())},{"ped",Words(ped.data(),ped.size(),true)},{"vehicle",Words(vehicle.data(),vehicle.size(),true)},{"allocations",allocations},{"trace",trace},{"switch",switches}};
    }
    const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
   }
  }
  *reinterpret_cast<uint8*>(0x96918A)=savedCheat;*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;CTimer::SetTimeStep(savedStep);StaticRef<int32>(0xB6F0D8)=savedCamera;std::memcpy(CPad::GetPad(0),savedPad.data(),savedPad.size());h->State(hs);h->LockState(hl);
  auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};
  for(const auto* n:{"finish_flag","active","sex","abort","mood","next","force","audio","shake","stat","health","message","los","wanted_level"})require(n);
  Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
 }
 Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",*reinterpret_cast<uint8*>(0x96918A)==savedCheat&&CTimer::GetTimeInMS()==savedTime&&CTimer::GetTimeStep()==savedStep&&StaticRef<int32>(0xB6F0D8)==savedCamera&&std::memcmp(CPad::GetPad(0),savedPad.data(),savedPad.size())==0}});
}
}
