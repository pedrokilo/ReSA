#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "../ProbeRuntime.hpp"
namespace TrainPosition355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int lightCalls{};
inline uint8 __fastcall Lighting(void* node,void*){const auto result=uint8(0x2F+lightCalls++*17);trace.push_back({"lighting",result});StaticRef<float>(0x8D12C0)+=.125f;return result;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","FindPositionOnTrackFromCoors",0x6F6CC0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-position355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-position355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-position355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-position355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<uint8,0x60> globals{};memcpy(globals.data(),reinterpret_cast<void*>(0xC37FEC),globals.size());
 const auto oldDay=StaticRef<float>(0x8D12C0);auto& modelSlot=StaticRef<void*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch l(0x6F5F80,Lighting);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){std::array<CTrainNode,8> nodes;alignas(16) std::array<uint8,sizeof(CTrain)> object{};std::array<uintptr_t,16> model{};CColModel col{};CMatrixLink matrix;
   auto run=[&](bool candidate){trace=json::array();lightCalls=0;memset(object.data(),0xCD,object.size());memset(nodes.data(),0xCD,sizeof(nodes));auto* self=reinterpret_cast<CTrain*>(object.data());
    const auto count=i%9;const auto track=(i/9)%4;StaticRef<std::array<int32,4>>(0xC38014).fill(count);StaticRef<std::array<CTrainNode*,4>>(0xC38024).fill(nodes.data());StaticRef<std::array<float,4>>(0xC37FEC).fill(100.f);StaticRef<float>(0x8D12C0)=float((i/36)%5)*.25f;
    for(int j=0;j<8;++j){nodes[j].x=int16(j*80);nodes[j].y=int16((j%2)*16);nodes[j].z=int16(j*8);nodes[j].m_nDistanceFromStart=uint16(j*30);}
    model.fill(0);model[5]=bit_cast<uintptr_t>(&col);modelSlot=model.data();col.m_boundBox.m_vecMin=CVector{-1.f,-float(i%7),-2.f};col.m_boundBox.m_vecMax=CVector{1.f,float(i%11),2.f};self->m_nModelIndex=400;self->m_nTrackId=int8(track);
    const auto position=CVector{float((i/4)%71)-.01f,float((i/13)%9-4),0.f};self->m_matrix=i%2?nullptr:&matrix;if(self->m_matrix)matrix.GetPosition()=position;else self->m_placement.m_vPosn=position;
    self->m_vecMoveSpeed=CVector{float(i%5-2)*.125f,float((i/5)%5-2)*.25f,i%17==0?std::bit_cast<float>(0x7FC00000u):.125f};self->trainFlags.bClockwiseDirection=i%2;self->m_fCurrentRailDistance=123.f;self->m_fTrainSpeed=-234.f;self->m_fContactSurfaceBrightness=.75f;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);
    json objectWords=json::array();for(size_t offset=0;offset+4<=object.size();offset+=4)objectWords.push_back(*reinterpret_cast<const uint32*>(object.data()+offset));
    return json{{"switch",true},{"object_words",objectWords},{"rail_distance",std::bit_cast<uint32>(self->m_fCurrentRailDistance)},{"train_speed",std::bit_cast<uint32>(self->m_fTrainSpeed)},{"brightness",std::bit_cast<uint32>(self->m_fContactSurfaceBrightness)},{"position",json::array({std::bit_cast<uint32>(self->GetPosition().x),std::bit_cast<uint32>(self->GetPosition().y),std::bit_cast<uint32>(self->GetPosition().z)})},{"object",Hash(object.data(),object.size())},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"day",std::bit_cast<uint32>(StaticRef<float>(0x8D12C0))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xC37FEC),globals.data(),globals.size());StaticRef<float>(0x8D12C0)=oldDay;modelSlot=oldModel;bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


