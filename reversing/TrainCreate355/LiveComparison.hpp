#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "Population.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
namespace TrainCreate355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},used{};inline uint8* storage{};inline uint8* matrices{};inline uintptr_t* table{};inline CPed* driver{};
inline int Index(const void* ptr){return ptr?int((static_cast<const uint8*>(ptr)-storage)/0xA18):-1;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline uintptr_t moduleBase{},moduleEnd{};
inline void InitModule(){HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&module);moduleBase=uintptr_t(module);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(moduleBase+reinterpret_cast<IMAGE_DOS_HEADER*>(moduleBase)->e_lfanew);moduleEnd=moduleBase+nt->OptionalHeader.SizeOfImage;}
inline bool InDll(uintptr_t address){return address>=moduleBase&&address<moduleEnd;}
inline uintptr_t Follow(uintptr_t address){for(int i=0;i<8&&InDll(address)&&*reinterpret_cast<uint8*>(address)==0xE9;++i){const auto target=address+5+*reinterpret_cast<int32*>(address+1);if(!InDll(target))break;address=target;}return address;}
inline uintptr_t FirstCall(uintptr_t address){address=Follow(address);if(!InDll(address))return 0;for(uintptr_t p=address;p<address+0x300;++p){if(*reinterpret_cast<uint8*>(p)!=0xE8)continue;const auto target=Follow(p+5+*reinterpret_cast<int32*>(p+1));if(!InDll(target))continue;auto* bytes=reinterpret_cast<uint8*>(target);if(bytes[0]==0x8B&&bytes[1]==0xFF)bytes+=2;if(bytes[0]==0x55&&bytes[1]==0x8B&&bytes[2]==0xEC)return target;}return 0;}
inline void* __cdecl Allocate(unsigned size){trace.push_back({"allocate",size,used});return storage+(used++)*0xA18;}
inline void* __fastcall Constructor(void* raw,void*,int32 model,eVehicleCreatedBy by){const auto index=Index(raw);trace.push_back({"construct",index,model,int(by)});memset(raw,0xCD,0x6AC);auto* train=static_cast<CTrain*>(raw);*reinterpret_cast<uintptr_t*>(raw)=uintptr_t(table);train->m_nModelIndex=int16(model);train->m_matrix=reinterpret_cast<CMatrixLink*>(matrices+index*0x80);train->m_pDriver=nullptr;return raw;}
inline void __fastcall Translate(void* raw,void*,CVector pos){trace.push_back({"translate",int((static_cast<uint8*>(raw)-matrices)/0x80),std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z)});memcpy(static_cast<uint8*>(raw)+0x30,&pos,12);}
inline int32 __cdecl Closest(CVector pos,int32* track){*track=(scenario/7)%4;const auto node=(scenario/11)%8;trace.push_back({"closest",std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),*track,node});return node;}
inline void __fastcall Position(void* raw,void*){auto* train=static_cast<CTrain*>(raw);const auto& pos=train->GetPosition();trace.push_back({"position",Index(raw),std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z)});train->m_fCurrentRailDistance=77.25f;}
inline void __fastcall Register(void* raw,void*,CEntity** ref){trace.push_back({"register",Index(raw),Index(*ref),uint32(reinterpret_cast<uint8*>(ref)-storage)%0xA18});}
inline void __fastcall Control(void* raw,void*){auto* train=static_cast<CTrain*>(raw);trace.push_back({"control",Index(raw),std::bit_cast<uint32>(train->m_fCurrentRailDistance),std::bit_cast<uint32>(train->m_fLength),train->m_nTrainFlags,Index(train->m_pPrevCarriage),Index(train->m_pNextCarriage)});train->m_fCurrentRailDistance+=.25f;train->m_fTrainSpeed=.125f;}
inline void __cdecl Remove(CEntity* raw){trace.push_back({"remove",Index(raw)});}
inline void __cdecl Add(CEntity* raw){trace.push_back({"add",Index(raw)});}
inline CPed* __cdecl Spawn(CVehicle* raw,bool asDriver,int32 rating,int32 seat,bool male,bool criminal){trace.push_back({"spawn",Index(raw),asDriver,rating,seat,male,criminal});raw->m_pDriver=scenario%17==0?nullptr:driver;return raw->m_pDriver;}
inline void __fastcall Decision(void* raw,void*,int32 type){trace.push_back({"decision",type});static_cast<CPedIntelligence*>(raw)->m_nDecisionMakerType=type;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","CreateMissionTrain",0x6F7550,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-create355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-create355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-create355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-create355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 InitModule();auto* constructorHook=FindHook("CTrain","Constructor");const bool ctorEnabled=constructorHook&&constructorHook->Hooked(),ctorLocked=constructorHook&&constructorHook->Locked();if(constructorHook){constructorHook->LockState(false);constructorHook->State(true);}const auto constructorBody=constructorHook?FirstCall(uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(constructorHook)->GetHookOurAddress())):0;if(constructorHook){constructorHook->State(ctorEnabled);constructorHook->LockState(ctorLocked);}if(!constructorBody){hooks[0]->State(enabled[0]);hooks[0]->LockState(locked[0]);Write({{"event","error"},{"reason","native train constructor resolution failed"}});return;}
 const auto oldCounts=StaticRef<std::array<int32,4>>(0xC38014);const auto oldNodes=StaticRef<std::array<CTrainNode*,4>>(0xC38024);static constexpr std::array<int32,5> ids{449,537,538,569,570};std::array<void*,5> oldModels{};for(size_t k=0;k<5;++k)oldModels[k]=StaticRef<void*>(0xA9B0C8+ids[k]*4);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x6E2D50,Allocate),ad(bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CVehicle::operator new)),Allocate);
 EntryPatch c(0x6F6030,Constructor),cd(constructorBody,Constructor);
 const auto controlBody=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindHook("CTrain","ProcessControl"))->GetHookOurAddress());EntryPatch pc(0x6F86A0,Control),pcd(controlBody,Control);
 EntryPatch m(0x59AF40,Translate),md(bit_cast<uintptr_t>(&CMatrix::SetTranslate),Translate);
 EntryPatch n(0x6F6BD0,Closest),nd(bit_cast<uintptr_t>(&CTrain::FindClosestTrackNode),Closest);
 EntryPatch p(0x6F6CC0,Position),pd(bit_cast<uintptr_t>(&CTrain::FindPositionOnTrackFromCoors),Position);
 EntryPatch r(0x571B70,Register),rd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Register);
 EntryPatch w(0x563280,Remove),wd(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
 EntryPatch wa(0x563220,Add),wad(bit_cast<uintptr_t>(&CWorld::Add),Add);
 EntryPatch s(0x613A00,Spawn),sd(bit_cast<uintptr_t>(&CPopulation::AddPedInCar),Spawn);
 EntryPatch d(0x600B50,Decision),dd(bit_cast<uintptr_t>(&CPedIntelligence::SetPedDecisionMakerType),Decision);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,8*0xA18> objects{};alignas(16) std::array<uint8,8*0x80> transforms{};alignas(16) std::array<uint8,sizeof(CPed)> ped{};alignas(16) std::array<uint8,sizeof(CPedIntelligence)> intellect{};std::array<CTrainNode,8> nodes;std::array<std::array<uintptr_t,16>,5> models{};alignas(16) std::array<std::array<uint8,sizeof(CColModel)>,5> cols{};std::array<uintptr_t,66> vtable{};vtable[10]=bit_cast<uintptr_t>(&Control);CTrain* first{};CTrain* last{};
   auto run=[&](bool candidate){scenario=i;used=0;trace=json::array();objects.fill(0xCD);transforms.fill(0);ped.fill(0);intellect.fill(0);memset(nodes.data(),0,sizeof(nodes));memset(models.data(),0,sizeof(models));memset(cols.data(),0,sizeof(cols));storage=objects.data();matrices=transforms.data();table=vtable.data();driver=reinterpret_cast<CPed*>(ped.data());*reinterpret_cast<CPedIntelligence**>(ped.data()+0x47C)=reinterpret_cast<CPedIntelligence*>(intellect.data());first=last=nullptr;
    StaticRef<std::array<int32,4>>(0xC38014).fill(8);StaticRef<std::array<CTrainNode*,4>>(0xC38024).fill(nodes.data());for(size_t k=0;k<8;++k)nodes[k].m_nDistanceFromStart=uint16(k*30);
    for(size_t k=0;k<5;++k){models[k][5]=bit_cast<uintptr_t>(cols[k].data());StaticRef<void*>(0xA9B0C8+ids[k]*4)=models[k].data();auto* col=reinterpret_cast<CColModel*>(cols[k].data());col->m_boundBox.m_vecMin=CVector{-1.f,-float(k+1),-2.f};col->m_boundBox.m_vecMax=CVector{1.f,float(k+4),2.f};}
    const auto node=i%5==0?-1:(i/3)%8;const auto track=(i/5)%4;const auto type=uint32((i/7)%10);auto* lastOut=i%3==0?nullptr:i%7==0?&first:&last;const auto pos=CVector{float(i%7),float(i%11),float(i%3)};
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(CVector,bool,uint32,CTrain**,CTrain**,int32,int32,bool)>(fn.address)(pos,i%2!=0,type,&first,lastOut,node,track,(i/13)%2!=0);
    if(!candidate&&used>0)++positiveCases;
    return json{{"switch",true},{"first",Index(first)},{"last",Index(last)},{"objects",Hash(objects.data(),objects.size())},{"transforms",Hash(transforms.data(),transforms.size())},{"ped",Hash(ped.data(),ped.size())},{"intellect",Hash(intellect.data(),intellect.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 StaticRef<std::array<int32,4>>(0xC38014)=oldCounts;StaticRef<std::array<CTrainNode*,4>>(0xC38024)=oldNodes;for(size_t k=0;k<5;++k)StaticRef<void*>(0xA9B0C8+ids[k]*4)=oldModels[k];bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


