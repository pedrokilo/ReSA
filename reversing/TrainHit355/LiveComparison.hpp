#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "TheZones.h"
#include "Streaming.h"
#include "GameLogic.h"
#include "../ProbeRuntime.hpp"
namespace TrainHit355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
struct Node{CPhysical* item;Node* next;};inline std::array<Node,8> nodes;
alignas(16) inline std::array<std::array<uint8,0x200>,8> objects;inline std::array<uintptr_t,5> vtable;
inline int Index(const void* item){for(int i=0;i<8;++i)if(item==objects[i].data())return i;return -1;}
inline void __fastcall Static(CEntity* entity,void*,bool value){const auto index=Index(entity);trace.push_back({"static",index,value,*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity)+0x1C)});entity->m_bIsStatic=value;if(scenario%3==0&&index>=0)nodes[index].next=nullptr;}
inline void __fastcall Moving(CPhysical* entity,void*){trace.push_back({"moving",Index(entity),bool(entity->m_bIsStatic)});entity->physicalFlags.bDisableSimpleCollision=true;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","TrainHitStuff<CPtrListSingleLink<CPhysical*>>",0x6F5CF0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-hit355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-hit355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-hit355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-hit355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<uint8,12> saved;memcpy(saved.data(),reinterpret_cast<void*>(0x8CD6C8),saved.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 vtable[4]=bit_cast<uintptr_t>(&Static);
 EntryPatch a(0x542800,Moving),ad(bit_cast<uintptr_t>(&CPhysical::AddToMovingList),Moving);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(objects.data(),0xCD,sizeof(objects));memset(reinterpret_cast<void*>(0x8CD6C8),0xCD,saved.size());const auto count=i%9;
    StaticRef<uint16>(0x8CD6C8)=0;StaticRef<uint16>(0x8CD6CC)=500;StaticRef<uint16>(0x8CD6D0)=65535;
    for(int j=0;j<8;++j){auto* physical=reinterpret_cast<CPhysical*>(objects[j].data());*reinterpret_cast<uintptr_t*>(physical)=uintptr_t(vtable.data());*reinterpret_cast<uint32*>(objects[j].data()+0x1C)=std::array<uint32,4>{0,4,0x40000,0x40004}[(i/9+j)%4];objects[j][0x36]=uint8((i/36+j)%3==0?4:(i/36+j)%3);physical->m_nModelIndex=std::array<int16,4>{-1,0,500,501}[(i/108+j)%4];nodes[j].item=physical;nodes[j].next=j+1<count?&nodes[j+1]:nullptr;}
    Node* list=count?nodes.data():nullptr;CEntity* excluded=(i/7)%3==0?reinterpret_cast<CEntity*>(objects[(i/3)%8].data()):nullptr;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(Node**,CEntity*)>(fn.address)(&list,excluded);
    if(!candidate&&!trace.empty())++positiveCases;for(auto& object:objects)*reinterpret_cast<uintptr_t*>(object.data())=0;
    std::array<int,8> next;for(int j=0;j<8;++j)next[j]=nodes[j].next?int(nodes[j].next-nodes.data()):-1;
    return json{{"switch",true},{"objects",Hash(objects.data(),sizeof(objects))},{"next",next},{"models",Hash(reinterpret_cast<void*>(0x8CD6C8),saved.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 memcpy(reinterpret_cast<void*>(0x8CD6C8),saved.data(),saved.size());bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

