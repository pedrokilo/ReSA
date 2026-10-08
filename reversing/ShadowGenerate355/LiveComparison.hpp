#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Shadows.h"
#include "World.h"
#include "../ProbeRuntime.hpp"
namespace ShadowGenerate355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},calls{};inline std::array<uint32,48> tokens{};
struct Access:CWorld{using CWorld::ClearScanCodes;};
inline uint32 Bits(float f){return std::bit_cast<uint32>(f);}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
using List=CPtrListSingleLink<CPhysical*>;
inline void __cdecl Sector(List& list,float x1,float y1,float x2,float y2,CVector* position,float fx,float fy,float sx,float sy,int16 intensity,uint8 red,uint8 green,uint8 blue,float z,float scale,CPolyBunch** out,uint8* dayNight,int32 type){
trace.push_back({"sector",(uintptr_t(&list)-uintptr_t(&CWorld::ms_aSectors))/8,Bits(x1),Bits(y1),Bits(x2),Bits(y2),Bits(position->x),Bits(position->y),Bits(position->z),Bits(fx),Bits(fy),Bits(sx),Bits(sy),intensity,red,green,blue,Bits(z),Bits(scale),type});++calls;*out=reinterpret_cast<CPolyBunch*>(&tokens[scenario%48]);*dayNight=uint8(calls);}
inline void __cdecl Clear(){trace.push_back({"clear"});}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CShadows","GeneratePolysForStaticShadow",0x70B730,0,-1,2048}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CShadows")for(auto& x:c.Items())if(x->Name()=="GeneratePolysForStaticShadow")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"shadow-generate355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"shadow-generate355-runtime.jsonl");ReversingProbe::InstallFailureCapture("shadow-generate355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;auto saved=CShadows::aStaticShadows;auto scan=CWorld::ms_nCurrentScanCode;int pairs=0,failures=0,positive=0,clears=0,inactive=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch s(0x70A470,Sector),sd(bit_cast<uintptr_t>(&CShadows::CastPlayerShadowSectorList<List>),Sector),c(0x563470,Clear),cd(bit_cast<uintptr_t>(&Access::ClearScanCodes),Clear);
auto* h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
for(int i=0;i<fns[0].cases;++i){auto run=[&](bool candidate){scenario=i;calls=0;trace=json::array();memset(CShadows::aStaticShadows.data(),0xCD,sizeof(CShadows::aStaticShadows));auto& shadow=CShadows::aStaticShadows[i%48];constexpr float positions[]{-4000,-3000,-2999.99f,-2950,-.001f,0,.001f,2949.999f,2950,2999.99f,3000,4000};shadow.m_vecPosn=CVector{positions[i%12],positions[(i/12)%12],float(i%17)};shadow.m_fFrontX=float((i/144)%7-3)*.5f;shadow.m_fFrontY=float((i/1008)%7-3)*.5f;shadow.m_fSideX=float(i%9-4)*.125f;shadow.m_fSideY=float(i%13-6)*.125f;shadow.m_fZDistance=4.5f;shadow.m_fScale=float(i%7)*.125f;shadow.m_pPolyBunch=nullptr;CWorld::ms_nCurrentScanCode=i%5?uint16(i):uint16(65535);
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x70B730)==candidate;reinterpret_cast<void(__cdecl*)(int16)>(0x70B730)(int16(i%48));return json{{"trace",trace},{"state",Hash(CShadows::aStaticShadows.data(),sizeof(CShadows::aStaticShadows))},{"scan",CWorld::ms_nCurrentScanCode},{"calls",calls},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=a["calls"].get<int>()>0;inactive+=a["calls"].get<int>()==0;for(const auto& t:a["trace"])clears+=t[0]=="clear";Write({{"event","comparison"},{"function","CShadows::GeneratePolysForStaticShadow"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
CShadows::aStaticShadows=saved;CWorld::ms_nCurrentScanCode=scan;pass&=positive>0&&inactive>0&&clears>0;Write({{"event","function_summary"},{"function","CShadows::GeneratePolysForStaticShadow"},{"cases",pairs},{"failures",failures},{"positive",positive},{"inactive",inactive},{"scan_wrap",clears}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
