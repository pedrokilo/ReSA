#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Plane.h"
#include "Collision.h"
#include "ColSphere.h"
#include "../ProbeRuntime.hpp"
namespace PlaneCreation355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
struct WorldAccess:CWorld{using CWorld::ClearScanCodes;};
inline int draw{},playerCalls{},losCalls{};
inline uint32 F(float value){return std::bit_cast<uint32>(value);}
inline int32 __cdecl Random(){const auto r=(scenario*43+draw++*7919)&0x7FFF;trace.push_back({"random",r});return r;}
inline CVector* __cdecl Player(CVector* out,int32 id){*out=CVector{float(scenario%97-48+playerCalls),float(scenario%71-35-playerCalls),float(scenario%31-15)};++playerCalls;trace.push_back({"player",id,F(out->x),F(out->y),F(out->z)});return out;}
inline float __cdecl Heading(float x,float y){const auto result=float(scenario%13-6)*.125f;trace.push_back({"heading",F(x),F(y),F(result)});return result;}
inline void __cdecl Clear(){trace.push_back({"clear",StaticRef<uint16>(0xB7CD78)});StaticRef<CVector>(0xB6F9AC).z+=.25f;}
inline void __fastcall Sphere(CColSphere* sphere,void*,float radius,const CVector& center,eSurfaceType surface,uint8 piece,tColLighting lighting){trace.push_back({"sphere",F(radius),F(center.x),F(center.y),F(center.z),int(surface),piece,lighting.value});memset(sphere,0xCD,sizeof(*sphere));sphere->m_fRadius=radius;sphere->m_vecCenter=center;sphere->m_Surface.m_nMaterial=surface;sphere->m_Surface.m_nPiece=piece;sphere->m_Surface.m_nLighting=lighting;StaticRef<CVector>(0xB6F9AC).x+=.125f;}
inline bool __cdecl Sight(const CVector& start,const CVector& end,bool buildings,bool vehicles,bool peds,bool objects,bool dummies,bool seeThrough,bool ignore){const auto r=(scenario+losCalls)%3!=0;++losCalls;trace.push_back({"sight",F(start.x),F(start.y),F(start.z),F(end.x),F(end.y),F(end.z),buildings,vehicles,peds,objects,dummies,seeThrough,ignore,r});const_cast<CVector&>(start).z+=.5f;return r;}
inline bool __cdecl Buildings(int32 x,int32 y,const CColBox&,const CColSphere& sum,const CColSphere& a,const CColSphere& b){const auto r=losCalls<=scenario%51;trace.push_back({"buildings",x,y,&sum==&a&&&a==&b,F(sum.m_fRadius),F(sum.m_vecCenter.x),F(sum.m_vecCenter.y),F(sum.m_vecCenter.z),r});return r;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CPlane","FindPlaneCreationCoors",0x6CD090,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"plane-creation355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"plane-creation355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plane-creation355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"plane-creation355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto savedForward=StaticRef<CVector>(0xB6F9AC);const auto savedScan=StaticRef<uint16>(0xB7CD78);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x821B1E,Random),ad(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch b(0x56E010,Player),bd(bit_cast<uintptr_t>(&FindPlayerCoors),Player);
 EntryPatch c(0x53CC70,Heading),cd(bit_cast<uintptr_t>(&CGeneral::GetATanOfXY),Heading);
 EntryPatch d(0x563470,Clear),dd(bit_cast<uintptr_t>(&WorldAccess::ClearScanCodes),Clear);
 EntryPatch e(0x40FD10,Sphere),ed(bit_cast<uintptr_t>(&CColSphere::Set),Sphere);
 EntryPatch f(0x56A490,Sight),fd(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Sight);
 EntryPatch g(0x41A820,Buildings),gd(bit_cast<uintptr_t>(&CCollision::CheckCameraCollisionBuildings),Buildings);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){std::array<CVector,2> output;std::array<float,2> numbers;
   auto run=[&](bool candidate){scenario=i;draw=playerCalls=losCalls=0;trace=json::array();output={CVector{-1.f,-2.f,-3.f},CVector{-4.f,-5.f,-6.f}};numbers={-123.f,-124.f};StaticRef<CVector>(0xB6F9AC)=CVector{float(i%7-3)*.25f,float(i%9-4)*.125f,float(i%5-2)*.1f};StaticRef<uint16>(0xB7CD78)=std::array<uint16,4>{0,65534,65535,100}[(i/2)%4];
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(CVector*,CVector*,float*,float*,bool)>(fn.address)(&output[0],&output[i%17==0?0:1],&numbers[0],&numbers[i%19==0?0:1],i%2!=0);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"output",Hash(output.data(),sizeof(output))},{"numbers",Hash(numbers.data(),sizeof(numbers))},{"forward",Hash(&StaticRef<CVector>(0xB6F9AC),sizeof(CVector))},{"scan",StaticRef<uint16>(0xB7CD78)},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 StaticRef<CVector>(0xB6F9AC)=savedForward;StaticRef<uint16>(0xB7CD78)=savedScan;bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

