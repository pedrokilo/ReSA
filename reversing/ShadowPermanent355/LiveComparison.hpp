#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Shadows.h"
#include "FireManager.h"
#include "../ProbeRuntime.hpp"
namespace ShadowPermanent355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline json Vec(const CVector& v){return json::array({Bits(v.x),Bits(v.y),Bits(v.z)});}
inline bool __cdecl Store(uint32 id,eShadowType type,RwTexture* tex,const CVector& pos,float fx,float fy,float sx,float sy,int16 intensity,uint8 r,uint8 g,uint8 b,float zd,float scale,float draw,bool temporary,float up){trace.push_back({"store",(id-0xC4AC30)/sizeof(CPermanentShadow),int(type),uintptr_t(tex),Vec(pos),Bits(fx),Bits(fy),Bits(sx),Bits(sy),int(intensity),int(r),int(g),int(b),Bits(zd),Bits(scale),Bits(draw),temporary,Bits(up)});return scenario%5!=0;}
inline CFire* __fastcall Fire(CFireManager*,void*,CVector pos,float size,uint8 unused,CEntity* creator,uint32 time,int8 generations,uint8 unused2){trace.push_back({"fire",Vec(pos),Bits(size),int(unused),uintptr_t(creator),time,int(generations),int(unused2)});return nullptr;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CShadows","AddPermanentShadow",0x706F60,0,-1,2048},
{"CShadows","UpdatePermanentShadows",0x70C950,0,-1,4096}};
inline auto* FindHook(const char* name){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CShadows")for(auto& x:c.Items())if(x->Name()==name)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"shadow-permanent355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"shadow-permanent355-runtime.jsonl");ReversingProbe::InstallFailureCapture("shadow-permanent355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::array<uint8,sizeof(CShadows::aPermanentShadows)> saved{};std::memcpy(saved.data(),&CShadows::aPermanentShadows,saved.size());auto timeSaved=CTimer::m_snTimeInMilliseconds,frameSaved=CTimer::m_FrameCounter;bool pass=true;int pairs=0;
{
using ReversingProbe::EntryPatch;using std::bit_cast;using FireFn=CFire*(CFireManager::*)(CVector,float,uint8,CEntity*,uint32,int8,uint8);
EntryPatch se(0x70BA00,Store),sd(bit_cast<uintptr_t>(&CShadows::StoreStaticShadow),Store),fe(0x539F00,Fire),fd(bit_cast<uintptr_t>(static_cast<FireFn>(&CFireManager::StartFire)),Fire);
for(auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);int failures=0,stores=0,fires=0,changed=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();std::memset(&CShadows::aPermanentShadows,0xCD,saved.size());CTimer::m_snTimeInMilliseconds=i%11==0?0xFFFFFFF0u:5000u;CTimer::m_FrameCounter=uint32(i%8);
for(size_t k=0;k<CShadows::aPermanentShadows.size();++k){auto& s=CShadows::aPermanentShadows[k];s.m_nType=static_cast<eShadowType>((i+int(k))%9);s.m_vecPosn=CVector{float(int(k)%7-3)*.75f,float(int(k)/7-3)*.5f,float(k%3)*.25f};s.m_fFrontX=i%3?.25f:.5f;s.m_fFrontY=0.f;s.m_fSideX=0.f;s.m_fSideY=i%4?.25f:.5f;s.m_fZDistance=4.f;s.m_fScale=1.25f;s.m_pTexture=reinterpret_cast<RwTexture*>(0x12340000u+uint32(k)*4);s.m_nIntensity=uint16(i*31+int(k)*101);s.m_nRed=uint8(i+k);s.m_nGreen=uint8(255-k);s.m_nBlue=uint8(i*3+k);constexpr uint32 durations[]{0,1,2,3,4,7,100,2000,0xFFFFFFFEu};s.m_nTimeDuration=durations[(i/8+int(k))%9];constexpr uint32 elapsed[]{0,1,2,3,4,75,99,100,1999,2000,0xFFFFFFFEu};s.m_nTimeCreated=CTimer::GetTimeInMS()-elapsed[(i/16+int(k))%11];}
if(fn.address==0x706F60){for(auto& s:CShadows::aPermanentShadows)s.m_nType=static_cast<eShadowType>(i%4==0?0:1);if(i%4==1)CShadows::aPermanentShadows[i%48].m_nType=SHADOW_NONE;}
const auto before=Hash(&CShadows::aPermanentShadows,saved.size());h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
if(fn.address==0x706F60){CVector pos{float(i%11-5),float(i%7-3),float(i%3)};reinterpret_cast<void(__cdecl*)(uint8,RwTexture*,CVector*,float,float,float,float,int16,uint8,uint8,uint8,float,uint32,float)>(fn.address)(uint8(i%9),reinterpret_cast<RwTexture*>(0x45678000),&pos,.125f,.5f,-.25f,.75f,int16(i*47),uint8(i),uint8(i*2),uint8(i*3),10.f,uint32(i%2001),.25f);}
else reinterpret_cast<void(__cdecl*)()>(fn.address)();
return json{{"state",Hash(&CShadows::aPermanentShadows,saved.size())},{"before",before},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;changed+=a["state"]!=a["before"];for(auto& e:a["trace"]){stores+=e[0]=="store";fires+=e[0]=="fire";}Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);pass&=changed>0&&(fn.address!=0x70C950||(stores>0&&fires>0));Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"stores",stores},{"fires",fires},{"changed",changed}});}}
std::memcpy(&CShadows::aPermanentShadows,saved.data(),saved.size());CTimer::m_snTimeInMilliseconds=timeSaved;CTimer::m_FrameCounter=frameSaved;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
