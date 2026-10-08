#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Train.h"
#include "AEVehicleAudioEntity.h"
#include "ClumpModelInfo.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
namespace TrainCtor355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int scenario{},draw{};alignas(16) inline std::array<uint8,0x80> clump{};inline void* activeSelf{};
inline uintptr_t moduleBase{},moduleEnd{};
inline void InitModule(){HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&module);moduleBase=uintptr_t(module);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(moduleBase+reinterpret_cast<IMAGE_DOS_HEADER*>(moduleBase)->e_lfanew);moduleEnd=moduleBase+nt->OptionalHeader.SizeOfImage;}
inline bool InDll(uintptr_t address){return address>=moduleBase&&address<moduleEnd;}
inline uintptr_t Follow(uintptr_t address){for(int i=0;i<8&&InDll(address)&&*reinterpret_cast<uint8*>(address)==0xE9;++i){const auto target=address+5+*reinterpret_cast<int32*>(address+1);if(!InDll(target))break;address=target;}return address;}
inline uintptr_t FirstCall(uintptr_t address){address=Follow(address);if(!InDll(address))return 0;for(uintptr_t p=address;p<address+0x300;++p){if(*reinterpret_cast<uint8*>(p)!=0xE8)continue;const auto target=Follow(p+5+*reinterpret_cast<int32*>(p+1));if(!InDll(target))continue;auto* bytes=reinterpret_cast<uint8*>(target);if(bytes[0]==0x8B&&bytes[1]==0xFF)bytes+=2;if(bytes[0]==0x55&&bytes[1]==0x8B&&bytes[2]==0xEC)return target;}return 0;}
inline void* __fastcall Base(void* self,void*,eVehicleCreatedBy by){trace.push_back({"base",self==activeSelf,int(by)});memset(self,0xCD,0x5A0);*reinterpret_cast<uint32*>(static_cast<uint8*>(self)+0x1C)=uint32(scenario)*0x01020304u;*reinterpret_cast<uint32*>(static_cast<uint8*>(self)+0x40)=uint32(scenario)*0x12345u;*reinterpret_cast<uintptr_t*>(static_cast<uint8*>(self)+0x18)=uintptr_t(clump.data());return self;}
inline void __fastcall Model(CVehicle* self,void*,uint32 id){trace.push_back({"model",id,Hash(reinterpret_cast<uint8*>(self)+0x5D8,0x90),int(self->m_nVehicleType),int(self->m_nVehicleSubType)});self->m_nModelIndex=int16(id);}
inline void __cdecl Frames(RpClump* actual,RwFrame** frames){trace.push_back({"frames",actual==reinterpret_cast<RpClump*>(clump.data()),Hash(frames,17*sizeof(RwFrame*))});for(int i=0;i<17;++i)frames[i]=reinterpret_cast<RwFrame*>(uintptr_t(0x1000+i*16));}
inline int32 __cdecl Random(){const auto r=scenario*43+draw++*17;trace.push_back({"random",r});return r;}
inline void __fastcall Colour(void*,void*,uint8& a,uint8& b,uint8& c,uint8& d,int32 shift){trace.push_back({"colour",a,b,c,d,shift});a=1;b=2;c=3;d=4;}
inline void __fastcall Audio(void*,void*,CEntity* entity){trace.push_back({"audio",entity==activeSelf,Hash(reinterpret_cast<uint8*>(entity)+4,sizeof(CTrain)-4)});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","Constructor",0x6F6030,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-ctor355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-ctor355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-ctor355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-ctor355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 auto& modelSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;auto& streakSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+570*4);const auto oldStreak=streakSlot;std::array<uint8,0x380> savedHandling;memcpy(savedHandling.data(),reinterpret_cast<void*>(0xC2B9DC),savedHandling.size());const auto oldTime=StaticRef<uint32>(0xB7CB84);uint32 pairs=0,totalFailures=0;
 auto* baseHook=FindHook("CVehicle","Constructor");if(!baseHook){Write({{"event","error"},{"reason","missing CVehicle constructor"}});return;}
 const auto baseEnabled=baseHook->Hooked(),baseLocked=baseHook->Locked();baseHook->LockState(false);baseHook->State(true);hooks[0]->State(true);InitModule();
 const auto baseWrapper=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(baseHook)->GetHookOurAddress());
 const auto trainWrapper=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(hooks[0])->GetHookOurAddress());
 const auto trainBody=FirstCall(trainWrapper),baseBody=FirstCall(baseWrapper);const auto baseFromTrain=FirstCall(trainBody);
 if(!baseBody||baseBody!=baseFromTrain){baseHook->State(baseEnabled);baseHook->LockState(baseLocked);hooks[0]->State(enabled[0]);hooks[0]->LockState(locked[0]);Write({{"event","error"},{"reason","DLL base constructor resolution mismatch"}});return;}
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x6D5F10,Base),ad(baseBody,Base);
 const auto modelBody=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindHook("CVehicle","SetModelIndex"))->GetHookOurAddress());
 EntryPatch b(0x6D6A40,Model),bd(modelBody,Model);
 EntryPatch c(0x4C5440,Frames),cd(bit_cast<uintptr_t>(&CClumpModelInfo::FillFrameArray),Frames);
 EntryPatch d(0x821B1E,Random),dd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch e(0x4C8500,Colour),ed(bit_cast<uintptr_t>(&CVehicleModelInfo::ChooseVehicleColour),Colour);
 EntryPatch f(0x4F7670,Audio),fd(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::Initialise),Audio);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CTrain)> object{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> mi{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(mi.data(),0,mi.size());memset(clump.data(),0,clump.size());clump[0]=2;activeSelf=object.data();
    auto* model=reinterpret_cast<CVehicleModelInfo*>(mi.data());modelSlot=model;streakSlot=model;model->m_nHandlingId=uint16(((i/4)%4)*0x100+i%4);
    memcpy(reinterpret_cast<void*>(0xC2B9DC),savedHandling.data(),savedHandling.size());auto* handling=reinterpret_cast<tHandlingData*>(0xC2B9DC+i%4*0xE0);handling->m_fMass=float(i%11+1)*100.f;handling->m_fTurnMass=float(i%17+1)*90.f;handling->m_vecCentreOfMass=CVector{.1f,.2f,.3f};handling->m_fBuoyancyConstant=11.f;handling->m_nHandlingFlags=static_cast<eVehicleHandlingFlags>(uint32(i)*0x10001u);handling->m_fDragMult=std::array<float,5>{-.1f,.01f,.02f,10.f,std::bit_cast<float>(0x7FC00000u)}[(i/16)%5];StaticRef<uint32>(0xB7CB84)=uint32(i)*0x1020304u;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto returned=reinterpret_cast<void*(__thiscall*)(void*,int32,eVehicleCreatedBy)>(fn.address)(object.data(),i%2?570:400,static_cast<eVehicleCreatedBy>((i/2)%4));
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return_self",returned==object.data()},{"object",Hash(object.data()+4,object.size()-4)},{"handling",Hash(reinterpret_cast<void*>(0xC2B9DC),savedHandling.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 modelSlot=oldModel;streakSlot=oldStreak;memcpy(reinterpret_cast<void*>(0xC2B9DC),savedHandling.data(),savedHandling.size());StaticRef<uint32>(0xB7CB84)=oldTime;
 }
 baseHook->State(baseEnabled);baseHook->LockState(baseLocked);bool restored=modelSlot==oldModel&&streakSlot==oldStreak;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




