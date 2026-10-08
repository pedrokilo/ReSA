#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "WaterLevel.h"
#include "PostEffects.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace WaterFog355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline json trace;inline int scenario{};
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline void __cdecl Save(){trace.push_back({"save"});}inline void __cdecl Set(){trace.push_back({"set"});}inline void __cdecl Restore(){trace.push_back({"restore"});}
inline void __cdecl Quad(float x,float y,float w,float h,uint8 r,uint8 g,uint8 b,uint8 a,RwRaster* texture){trace.push_back({"quad",Bits(x),Bits(y),Bits(w),Bits(h),int(r),int(g),int(b),int(a),uintptr_t(texture)});}
inline RwBool __cdecl State(RwRenderState s,void* value){trace.push_back({"state",int(s),uintptr_t(value)});return TRUE;}
inline uint64 Hash(const void*,size_t);
inline void* __cdecl Transform(RwIm3DVertex* verts,RwUInt32 count,RwMatrix* matrix,RwUInt32 flags){const bool success=scenario%7!=0;trace.push_back({"transform",count,uintptr_t(matrix),flags,Hash(verts,count*sizeof(RwIm3DVertex)),success});return success?verts:nullptr;}
inline RwBool __cdecl Render(RwPrimitiveType type){trace.push_back({"render",int(type)});return TRUE;}
inline RwBool __cdecl End(){trace.push_back({"end"});return TRUE;}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline json Ptr(const void* p){if(!p)return nullptr;auto a=uintptr_t(p);if(a>=uintptr_t(targetTokens.data())&&a<uintptr_t(targetTokens.data())+sizeof(targetTokens))return json::array({"target",(a-uintptr_t(targetTokens.data()))/4});return a;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CWaterLevel","RenderWaterFog",0x6E7760,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CWaterLevel")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"water-fog355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"water-fog355-runtime.jsonl");ReversingProbe::InstallFailureCapture("water-fog355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::array<uint8,0x20> fogState{},fogConfig{};std::array<uint8,sizeof(CWaterLevel::ms_WaterFog)> fogSaved{};std::array<uint8,sizeof(TempBufferVertices)> bufferSaved{};
std::memcpy(fogState.data(),reinterpret_cast<void*>(0xC228E4),fogState.size());std::memcpy(fogConfig.data(),reinterpret_cast<void*>(0x8D37D4),fogConfig.size());std::memcpy(fogSaved.data(),&CWaterLevel::ms_WaterFog,fogSaved.size());std::memcpy(bufferSaved.data(),&TempBufferVertices,bufferSaved.size());auto waterSaved=CWeather::UnderWaterness,fxSaved=CWeather::WaterFogFXControl,timeSaved=CTimer::ms_fTimeStep;
auto& stateCallback=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet;auto oldCallback=stateCallback;stateCallback=State;
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch s1(0x700CC0,Save),s2(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesStore),Save),s3(0x700D70,Set),s4(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesSet),Set),s5(0x700E00,Restore),s6(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesReStore),Restore),q1(0x700EC0,Quad),q2(bit_cast<uintptr_t>(&CPostEffects::DrawQuad),Quad),t1(0x7EF450,Transform),r1(0x7EF6B0,Render),e1(0x7EF520,End);
int pairs=0;bool pass=true;
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();std::memset(&TempBufferVertices,0xCD,bufferSaved.size());std::memset(reinterpret_cast<void*>(0xC228E4),0xCD,fogState.size());CWaterLevel::m_bWaterFog=(i&1)!=0;CWaterLevel::m_bWaterFogScript=(i&2)!=0;constexpr float under[]{0.f,.534f,.535f,.536f};CWeather::UnderWaterness=under[(i/4)%4];constexpr float controls[]{0.f,.5f,1.f,2.f};CWeather::WaterFogFXControl=controls[(i/16)%4];constexpr int densities[]{0,1,2,6};CWaterLevel::m_WaterFogDensity=densities[(i/64)%4];CWaterLevel::m_fWaterFogHeight=3.f;StaticRef<float>(0x8D37F0)=.05f;StaticRef<CRGBA>(0x8D37E8)=CRGBA{11,22,33,44};StaticRef<CRGBA>(0x8D37EC)=CRGBA{55,66,77,188};constexpr uint32 counts[]{0,1,2,70};CWaterLevel::gWaterFogIndex=counts[(i/256)%4];CWaterLevel::gbPlayerIsInsideWaterFog=(i&32)!=0;constexpr float fades[]{0.f,.25f,.999f,1.f};StaticRef<float>(0xC228EC)=fades[(i/8)%4];StaticRef<float>(0xC228F0)=i%3?1.f:0.f;CTimer::ms_fTimeStep=(i&16)?1.f:2.f;
for(int k=0;k<70;++k){CWaterLevel::ms_WaterFog.minX[k]=int16(k-30);CWaterLevel::ms_WaterFog.maxX[k]=int16(k-20);CWaterLevel::ms_WaterFog.minY[k]=int16(k*2);CWaterLevel::ms_WaterFog.maxY[k]=int16(k*2+10);CWaterLevel::ms_WaterFog.z[k]=float(k)*.25f;}
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__cdecl*)()>(fn.address)();return json{{"state",Hash(reinterpret_cast<void*>(0xC228E4),fogState.size())},{"buffer",Hash(&TempBufferVertices,bufferSaved.size())},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=!a["trace"].empty();yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
stateCallback=oldCallback;std::memcpy(reinterpret_cast<void*>(0xC228E4),fogState.data(),fogState.size());std::memcpy(reinterpret_cast<void*>(0x8D37D4),fogConfig.data(),fogConfig.size());std::memcpy(&CWaterLevel::ms_WaterFog,fogSaved.data(),fogSaved.size());std::memcpy(&TempBufferVertices,bufferSaved.data(),bufferSaved.size());CWeather::UnderWaterness=waterSaved;CWeather::WaterFogFXControl=fxSaved;CTimer::ms_fTimeStep=timeSaved;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
