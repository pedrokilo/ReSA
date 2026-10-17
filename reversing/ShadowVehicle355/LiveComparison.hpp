#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Shadows.h"
#include "../ProbeRuntime.hpp"
namespace ShadowVehicle355Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline int scenario{}; alignas(16) inline std::array<uint8,sizeof(CBike)> vehicleBytes{};
inline CMatrixLink matrix{},cameraMatrix{}; inline CColModel collision{};
inline uint32 Bits(float f){return std::bit_cast<uint32>(f);}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto*b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline uint32 Rnd(uint32 i,uint32 s){uint32 x=i*2654435761u+s*40503u+0x9E3779B9u;x^=x<<13;x^=x>>17;x^=x<<5;x*=2246822519u;x^=x>>15;return x;}
inline float Rf(uint32 i,uint32 s,float lo,float hi){return lo+(hi-lo)*(float(Rnd(i,s)>>8)*(1.0f/16777216.0f));}
inline float Special(uint32 i,uint32 s,float normal){switch(Rnd(i,s+7777u)%24u){case 0:return std::bit_cast<float>(0x7FC00123u);case 1:return std::bit_cast<float>(0x7F800000u);case 2:return -0.0f;case 3:return 1.0e-40f;case 4:return 1.0e30f;case 5:return -1.0e30f;default:return normal;}}
inline void StressFixture(int i,CBike* v){const uint32 n=uint32(i);const int type=(i/22)%8;const float d=type==5?288.0f:(type>=2&&type<=4?144.0f:18.0f);
matrix.GetPosition()=CVector{Rf(n,1,-30.f,30.f),Rf(n,2,-30.f,30.f),Rf(n,3,-2.f,6.f)};matrix.GetRight()=CVector{Rf(n,4,-1.f,1.f),Rf(n,5,-1.f,1.f),Rf(n,6,-1.f,1.f)};
matrix.GetForward()=CVector{Rf(n,7,-1.f,1.f),Rf(n,8,-1.f,1.f),Special(n,9,Rf(n,10,-1.f,1.f))};matrix.GetUp()=CVector{Rf(n,11,-1.f,1.f),Rf(n,12,-1.f,1.f),Rf(n,13,-1.f,1.f)};
v->m_vecMoveSpeed=CVector{Special(n,14,Rf(n,15,-.3f,.3f)),Rf(n,16,-.3f,.3f),Rf(n,17,-.3f,.3f)};*reinterpret_cast<float*>(vehicleBytes.data()+0x648)=Special(n,18,Rf(n,19,-2.f,2.f));
cameraMatrix.GetPosition()=CVector{matrix.GetPosition().x-Rf(n,20,.6f*d,1.05f*d),matrix.GetPosition().y,0};TheCamera.m_fLODDistMultiplier=Rf(n,21,.2f,3.f);CTimer::ms_fTimeStep=Rf(n,22,.05f,3.f);
collision.m_boundBox.m_vecMin=CVector{-Rf(n,23,.3f,3.f),-Rf(n,24,.5f,5.f),-1};collision.m_boundBox.m_vecMax=CVector{Rf(n,25,.3f,3.f),Rf(n,26,.5f,5.f),2};}
inline bool __cdecl Quality(){trace.push_back({"quality"});return scenario%17==0;}
inline CColModel* __fastcall Col(CEntity*,void*){trace.push_back({"col"});return &collision;}
inline CVehicle* __cdecl Player(int32 p,bool remote){trace.push_back({"player",p,remote});return scenario%3==0?reinterpret_cast<CVehicle*>(vehicleBytes.data()):nullptr;}
inline void Capture(const char* name,RwTexture*t,const CVector&p,float fx,float fy,float sx,float sy,int16 v,uint8 r,uint8 g,uint8 b,float z,float scale,bool building){trace.push_back({name,uintptr_t(t),Bits(p.x),Bits(p.y),Bits(p.z),Bits(fx),Bits(fy),Bits(sx),Bits(sy),v,r,g,b,Bits(z),Bits(scale),building});}
inline bool __cdecl Static(uint32 id,eShadowType type,RwTexture*t,const CVector&p,float fx,float fy,float sx,float sy,int16 v,uint8 r,uint8 g,uint8 b,float z,float scale,float draw,bool temporary,float update){Capture("static",t,p,fx,fy,sx,sy,v,r,g,b,z,scale,false);trace.push_back({id,int(type),Bits(draw),temporary,Bits(update)});return scenario&1;}
inline void __cdecl Dynamic(uint8 type,RwTexture*t,const CVector&p,float fx,float fy,float sx,float sy,int16 v,uint8 r,uint8 g,uint8 b,float z,bool water,float scale,CRealTimeShadow*rt,bool buildings){Capture("dynamic",t,p,fx,fy,sx,sy,v,r,g,b,z,scale,buildings);trace.push_back({int(type),water,uintptr_t(rt)});}
inline void Write(const json&j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char*cls;const char*name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CShadows","StoreShadowForVehicle",0x70BDA0,0,-1,6144}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base*h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&c){if(c.Name()=="CShadows")for(auto&x:c.Items())if(x->Name()=="StoreShadowForVehicle")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"shadow-vehicle355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"shadow-vehicle355-runtime.jsonl");ReversingProbe::InstallFailureCapture("shadow-vehicle355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::array<uint8,sizeof(CCamera)> savedCamera{};memcpy(savedCamera.data(),&TheCamera,sizeof(TheCamera));const auto savedColours=CTimeCycle::m_CurrentColours;const auto savedStep=CTimer::ms_fTimeStep;auto& cutscene=*reinterpret_cast<uint8*>(0xB5F851);const auto savedCutscene=cutscene;
int pairs=0,failures=0,positive=0,inactive=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch q(0x70F9B0,Quality),qd(bit_cast<uintptr_t>(&GraphicsHighQuality),Quality),c(0x535300,Col),cd(bit_cast<uintptr_t>(&CEntity::GetColModel),Col),p(0x56E0D0,Player),pd(bit_cast<uintptr_t>(&FindPlayerVehicle),Player),s(0x70BA00,Static),sd(bit_cast<uintptr_t>(&CShadows::StoreStaticShadow),Static),d(0x707390,Dynamic),dd(bit_cast<uintptr_t>(static_cast<void(__cdecl*)(uint8,RwTexture*,const CVector&,float,float,float,float,int16,uint8,uint8,uint8,float,bool,float,CRealTimeShadow*,bool)>(&CShadows::StoreShadowToBeRendered)),Dynamic);
auto*h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
constexpr int models[]{400,417,425,447,469,487,497,465,501,463,468,523,448,461,462,481,509,510,521,522,539,-2};
for(int i=0;i<6144;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();memset(vehicleBytes.data(),0xCD,vehicleBytes.size());auto*v=reinterpret_cast<CBike*>(vehicleBytes.data());v->m_matrix=&matrix;v->m_nModelIndex=int16(models[i%std::size(models)]);v->SetStatus(i&1?STATUS_ABANDONED:STATUS_PHYSICS);vehicleBytes[0x42C]=uint8(i&8?0x80:0);*reinterpret_cast<float*>(vehicleBytes.data()+0x648)=float(i%19-9)*.125f;
matrix.GetPosition()=CVector{float(i%11-5),float(i%7-3),float(i%5)};matrix.GetRight()=CVector{1,.125f,float(i%9-4)*.25f};matrix.GetForward()=CVector{float(i%7-3)*.25f,float(i%11-5)*.25f,float(i%9-4)*.25f};matrix.GetUp()=CVector{.5f,-.75f,float(i%5-2)};v->m_vecMoveSpeed=CVector{float(i%5)*.01f,float(i%3)*.02f,float(i%7)*.01f};
cameraMatrix.GetPosition()=CVector{matrix.GetPosition().x-float((i/22)%17)*12,matrix.GetPosition().y,0};TheCamera.m_matrix=&cameraMatrix;TheCamera.m_fLODDistMultiplier=float(i%4+1)*.5f;CTimer::ms_fTimeStep=float(i%5+1)*.25f;cutscene=uint8((i>>3)&1);CTimeCycle::m_CurrentColours.m_nShadowStrength=i%29?int16(i*17):0;collision.m_boundBox.m_vecMin=CVector{-1,-2,-1};collision.m_boundBox.m_vecMax=CVector{float(i%5+1),float(i%7+1),2};
if(i>=4096)StressFixture(i,v);
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x70BDA0)==candidate;reinterpret_cast<void(__cdecl*)(CVehicle*,VEH_SHD_TYPE)>(0x70BDA0)(v,static_cast<VEH_SHD_TYPE>((i/22)%8));return json{{"trace",trace},{"vehicle",Hash(vehicleBytes.data(),vehicleBytes.size())},{"matrix",Hash(&matrix,sizeof(matrix))},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=a["trace"].size()>2;inactive+=a["trace"].size()<=2;Write({{"event","comparison"},{"function","CShadows::StoreShadowForVehicle"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
memcpy(&TheCamera,savedCamera.data(),sizeof(TheCamera));CTimeCycle::m_CurrentColours=savedColours;CTimer::ms_fTimeStep=savedStep;cutscene=savedCutscene;pass&=positive>0&&inactive>0;Write({{"event","function_summary"},{"function","CShadows::StoreShadowForVehicle"},{"cases",pairs},{"failures",failures},{"positive",positive},{"inactive",inactive}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
