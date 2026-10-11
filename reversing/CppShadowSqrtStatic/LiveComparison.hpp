#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Shadows.h"
#include "../ProbeRuntime.hpp"
namespace CppShadowSqrtStaticLive {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};inline std::array<uint32,64> tokens{};inline CMatrixLink cameraMatrix{};
inline uint32 Bits(float f){return std::bit_cast<uint32>(f);}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline void __cdecl Generate(int16 index){auto& s=CShadows::aStaticShadows[index];trace.push_back({"generate",index,Bits(s.m_vecPosn.x),Bits(s.m_fScale)});s.m_nDayNightIntensity=uint8(scenario);s.m_pPolyBunch=scenario%5?reinterpret_cast<CPolyBunch*>(&tokens[index]):nullptr;}
inline void __fastcall Free(CStaticShadow* s,void*){trace.push_back({"free",int(s-CShadows::aStaticShadows.data()),s->m_nId});s->m_pPolyBunch=nullptr;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CShadows","StoreStaticShadow",0x70BA00,0,-1,4096}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CShadows")for(auto& x:c.Items())if(x->Name()=="StoreStaticShadow")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"cpp-shadow-sqrt-static-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"cpp-shadow-sqrt-static-runtime.jsonl");ReversingProbe::InstallFailureCapture("cpp-shadow-sqrt-static");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto savedShadows=CShadows::aStaticShadows;std::array<uint8,sizeof(CCamera)> savedCamera{};memcpy(savedCamera.data(),&TheCamera,sizeof(TheCamera));auto savedTime=CTimer::GetTimeInMS();int pairs=0,failures=0,positive=0,inactive=0,reused=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch g(0x70B730,Generate),gd(bit_cast<uintptr_t>(&CShadows::GeneratePolysForStaticShadow),Generate),f(0x707670,Free),fd(bit_cast<uintptr_t>(&CStaticShadow::Free),Free);
auto* h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
for(int i=0;i<fns[0].cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();memset(CShadows::aStaticShadows.data(),0xCD,sizeof(CShadows::aStaticShadows));for(int k=0;k<48;++k){auto& s=CShadows::aStaticShadows[k];s.m_nId=100+uint32(k);s.m_pPolyBunch=reinterpret_cast<CPolyBunch*>(&tokens[k]);s.m_vecPosn=CVector{float(k),1,2};s.m_fFrontX=1;s.m_fFrontY=0;s.m_fSideX=0;s.m_fSideY=1;}
const int selected=i%48;auto& s=CShadows::aStaticShadows[selected];CVector pos{float(selected),1,2};const int mode=(i/48)%6;const uint32 id=mode<3?s.m_nId:700;if(mode==0)pos.x+=.025f;if(mode==1)pos.z+=1.75f;if(mode==2)pos.x+=3;if(mode==3)s.m_pPolyBunch=nullptr;if(mode==5)for(auto& row:CShadows::aStaticShadows)row.m_pPolyBunch=nullptr;
cameraMatrix.GetPosition()=CVector{pos.x-float((i/288)%13)*2,1,2};TheCamera.m_matrix=&cameraMatrix;CTimer::m_snTimeInMilliseconds=uint32(i)*1234567u;const float distance=(i&1)?20.0f:0.0f,up=(i&2)?.1f:0.0f,front=(i&4)?1.0001f:1.0f;
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x70BA00)==candidate;const bool result=reinterpret_cast<bool(__cdecl*)(uint32,eShadowType,RwTexture*,const CVector&,float,float,float,float,int16,uint8,uint8,uint8,float,float,float,bool,float)>(0x70BA00)(id,static_cast<eShadowType>(i%9),reinterpret_cast<RwTexture*>(uintptr_t(0x123400)),pos,front,0,0,1,int16(i*31),uint8(i),uint8(i*3),uint8(i*7),4.5f,float(i%17)*.125f,distance,(i&8)!=0,up);return json{{"result",result},{"state",Hash(CShadows::aStaticShadows.data(),sizeof(CShadows::aStaticShadows))},{"trace",trace},{"time",CTimer::GetTimeInMS()},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=!a["trace"].empty();inactive+=a["trace"].empty();reused+=a["trace"].empty()&&i%2==0;Write({{"event","comparison"},{"function","CShadows::StoreStaticShadow"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
CShadows::aStaticShadows=savedShadows;memcpy(&TheCamera,savedCamera.data(),sizeof(TheCamera));CTimer::m_snTimeInMilliseconds=savedTime;pass&=positive>0&&inactive>0&&reused>0;Write({{"event","function_summary"},{"function","CShadows::StoreStaticShadow"},{"cases",pairs},{"failures",failures},{"positive",positive},{"inactive",inactive},{"reused",reused}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
