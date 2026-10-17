#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "WaterLevel.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace WaterNearest355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline CMatrixLink matrix{};
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline json Ptr(const void* p){if(!p)return nullptr;auto a=uintptr_t(p);if(a>=uintptr_t(targetTokens.data())&&a<uintptr_t(targetTokens.data())+sizeof(targetTokens))return json::array({"target",(a-uintptr_t(targetTokens.data()))/4});return a;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CWaterLevel","FindNearestWaterAndItsFlow",0x6E9D70,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CWaterLevel")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"water-nearest355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"water-nearest355-runtime.jsonl");ReversingProbe::InstallFailureCapture("water-nearest355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::array<uint8,sizeof(TheCamera)> cameraSaved{};std::memcpy(cameraSaved.data(),&TheCamera,cameraSaved.size());std::array<uint8,sizeof(CWaterLevel::WaterQuads)> quadsSaved{};std::memcpy(quadsSaved.data(),&CWaterLevel::WaterQuads,quadsSaved.size());std::array<uint8,sizeof(CWaterLevel::m_aVertices)> verticesSaved{};std::memcpy(verticesSaved.data(),&CWaterLevel::m_aVertices,verticesSaved.size());auto countSaved=CWaterLevel::NumWaterQuads;auto flowSaved=CWaterLevel::m_CurrentDesiredFlow;int pairs=0;bool pass=true;
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){std::memcpy(&TheCamera,cameraSaved.data(),cameraSaved.size());TheCamera.m_matrix=i%2?&matrix:nullptr;matrix.SetUnity();constexpr float coord[]{-3000.f,-2999.99f,-2.f,-1.f,0.f,1.f,2.f,2999.99f,3000.f,0.1f,-7.3f,std::numeric_limits<float>::quiet_NaN()};CVector position{coord[i%12],coord[(i/12)%12],10.f};matrix.GetPosition()=position;TheCamera.m_placement.m_vPosn=position;CWaterLevel::NumWaterQuads=uint32((i/144)%9);CWaterLevel::m_CurrentDesiredFlow.x=17.f;CWaterLevel::m_CurrentDesiredFlow.y=-19.f;
for(int k=0;k<8;++k){auto& q=CWaterLevel::WaterQuads[k];for(int v=0;v<4;++v)q.verts[v]=uint16(k*4+v);for(int v=0;v<4;++v){auto& vertex=CWaterLevel::m_aVertices[k*4+v];vertex.x=int16(k*3-10+(v==1||v==3?2:0));vertex.y=int16(k*2-7+(v>=2?2:0));vertex.rp.z=float(k)+.5f;vertex.rp.bigWaves=(i+k+v)%3?0.f:.125f;vertex.rp.smallWaves=(i+k+v)%5?0.f:.25f;vertex.rp.flowX=int8(i+k*7+v*19);vertex.rp.flowY=int8(i-k*11-v*3);}}
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__cdecl*)()>(fn.address)();return json{{"distance",std::bit_cast<uint32>(TheCamera.m_fDistanceToWater)},{"height",std::bit_cast<uint32>(TheCamera.m_fHeightOfNearestWater)},{"camera",Hash(&TheCamera,sizeof(TheCamera))},{"flow",Hash(&CWaterLevel::m_CurrentDesiredFlow,sizeof(CWaterLevel::m_CurrentDesiredFlow))},{"quads",Hash(&CWaterLevel::WaterQuads,sizeof(CWaterLevel::WaterQuads))},{"vertices",Hash(&CWaterLevel::m_aVertices,sizeof(CWaterLevel::m_aVertices))},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=(i/144)%9!=0;yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
std::memcpy(&TheCamera,cameraSaved.data(),cameraSaved.size());std::memcpy(&CWaterLevel::WaterQuads,quadsSaved.data(),quadsSaved.size());std::memcpy(&CWaterLevel::m_aVertices,verticesSaved.data(),verticesSaved.size());CWaterLevel::NumWaterQuads=countSaved;CWaterLevel::m_CurrentDesiredFlow=flowSaved;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
