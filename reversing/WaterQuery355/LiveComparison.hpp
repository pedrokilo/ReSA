#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "WaterLevel.h"
#include "../ProbeRuntime.hpp"
namespace WaterQuery355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline uint32 F(float x){return std::bit_cast<uint32>(x);}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline bool Query(bool tri,const void* shape,float x,float y,float z,float* height,float* big,float* smallAmplitude){const auto base=tri?uintptr_t(CWaterLevel::WaterTriangles.data()):uintptr_t(CWaterLevel::WaterQuads.data());const auto size=tri?sizeof(CWaterTriangle):sizeof(CWaterQuad);const auto index=(uintptr_t(shape)-base)/size;const bool yes=(scenario+int(index)+(tri?1:0))%3==0;trace.push_back({tri?"triangle":"quad",index,F(x),F(y),F(z),big!=nullptr,smallAmplitude!=nullptr,yes});*height=float(index)+2.5f;if(big)*big=.25f;if(smallAmplitude)*smallAmplitude=.125f;return yes;}
inline bool __cdecl Quad(const CWaterQuad* q,float x,float y,float z,float* h,float* b,float* s){return Query(false,q,x,y,z,h,b,s);}
inline bool __cdecl Tri(const CWaterTriangle* q,float x,float y,float z,float* h,float* b,float* s){return Query(true,q,x,y,z,h,b,s);}
inline void Wave(int x,int y,float big,float smallAmplitude,float* h){float v=float(x)*.13f-float(y)*.37f+big*.25f+smallAmplitude*.5f;if(scenario%8==6)v=0.f;else if(scenario%8==7)v=(scenario&8)?std::numeric_limits<float>::infinity():std::numeric_limits<float>::quiet_NaN();trace.push_back({"wave",x,y,F(big),F(smallAmplitude),F(*h),F(v)});*h+=v;}
inline void __cdecl WaveExe(int x,int y,float b,float s,float* h){Wave(x,y,b,s,h);}
inline void __cdecl WaveDll(int x,int y,float* h,float b,float s){Wave(x,y,b,s,h);}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CWaterLevel","GetWaterLevelNoWaves",0x6E8580,0,-1,4096},
{"CWaterLevel","AddWaveToResult",0x6E81E0,0,-1,4096}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CWaterLevel")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"water-query355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"water-query355-runtime.jsonl");ReversingProbe::InstallFailureCapture("water-query355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::array<uint8,sizeof(CWaterLevel::m_BlockPolyInfo)> blocks{};std::array<uint8,sizeof(CWaterLevel::m_PolyCombos)> combos{};std::memcpy(blocks.data(),&CWaterLevel::m_BlockPolyInfo,blocks.size());std::memcpy(combos.data(),&CWaterLevel::m_PolyCombos,combos.size());int pairs=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch qe(0x6E5BB0,Quad),te(0x6E5E90,Tri),we(0x6E7210,WaveExe),wd(bit_cast<uintptr_t>(&CWaterLevel::CalculateWavesOnlyForCoordinate2),WaveDll),qd(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWavesForQuad),Quad),td(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWavesForTriangle),Tri);
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,positive=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();auto* b=reinterpret_cast<uint16*>(&CWaterLevel::m_BlockPolyInfo);const uint16 code=uint16(((i%4)<<14)|uint16(i%4==3?0:i%6));for(int k=0;k<144;++k)b[k]=code;auto* c=reinterpret_cast<uint16*>(&CWaterLevel::m_PolyCombos);std::fill(c,c+700,0);c[0]=0x4001;c[1]=0x8002;c[2]=0xC003;c[3]=0x4004;
constexpr float coords[]{-4000.f,-3000.f,-2999.875f,-2500.f,-.125f,0.f,.125f,499.875f,500.f,2999.875f,3000.f,4000.f};CVector pos{coords[i%12],coords[(i/12)%12],float(i%11-5)};float level=std::bit_cast<float>(uint32(0x3f800000+i%256)),big=17.f,smallAmplitude=19.f;CVector normal{7.f,8.f,9.f};bool ret=false;
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;if(fn.address==0x6E8580)ret=reinterpret_cast<bool(__cdecl*)(CVector,float*,float*,float*)>(fn.address)(pos,&level,(i&4)?&big:nullptr,(i&8)?&smallAmplitude:nullptr);else {float x=float(i%65-32)*.125f,y=float((i/65)%49-24)*.125f;if(i%32==31){x=1.f;y=1.f-5.9604645e-8f;}reinterpret_cast<void(__cdecl*)(float,float,float*,float,float,CVector*)>(fn.address)(x,y,&level,float(i%9)*.17f,float(i%7)*.23f,(i&4)?&normal:nullptr);}
return json{{"returned",ret},{"level",F(level)},{"big",F(big)},{"smallAmplitude",F(smallAmplitude)},{"normal",json::array({F(normal.x),F(normal.y),F(normal.z)})},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=!a["trace"].empty();Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=positive>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"positive",positive}});}}
std::memcpy(&CWaterLevel::m_BlockPolyInfo,blocks.data(),blocks.size());std::memcpy(&CWaterLevel::m_PolyCombos,combos.data(),combos.size());Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
