#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RealTimeShadowManager.h"
#include "RealTimeShadow.h"
#include "../ProbeRuntime.hpp"
namespace ShadowManagerPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
 {"CRealTimeShadowManager","ReInit",0x706870,0,-1,512},
 {"CRealTimeShadowManager","GetRealTimeShadow",0x706970,0,-1,2048},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}


alignas(16) inline std::array<std::array<uint8,sizeof(CRealTimeShadow)>,16> shadows{};
inline std::array<RwCamera,34> cameras{};inline std::array<RwRaster,34> oldRasters{},newRasters{};inline std::array<RwTexture,34> textures{};
inline int rasterIndex{};
inline int ShadowID(CRealTimeShadow* s){return s?int((uintptr_t(s)-uintptr_t(shadows.data()))/sizeof(shadows[0])):-1;}
inline RwBool __cdecl Destroy(RwRaster* r){trace.push_back({"destroy",int(r-oldRasters.data())});return true;}
inline RwRaster* __cdecl Create(int w,int h,int d,int flags){const int k=rasterIndex++;trace.push_back({"create",k,w,h,d,flags,cameras[k].frameBuffer==nullptr});return &newRasters[k];}
inline RwTexture* __cdecl SetRaster(RwTexture* t,RwRaster* r){trace.push_back({"set",int(t-textures.data()),int(r-newRasters.data())});t->raster=r;return t;}
inline RwRaster* __fastcall Gradient(CShadowCamera* self,void*){trace.push_back({"gradient",self->m_pRwCamera==&cameras[33]});return nullptr;}
inline bool __fastcall Setup(CRealTimeShadow* self,void*,CPhysical* owner){trace.push_back({"setup",ShadowID(self)});self->m_pOwner=owner;return false;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"shadow-manager-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"shadow-manager-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("shadow-manager-pending");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 auto& focus=*reinterpret_cast<uint8*>(0xB7CD74);const auto savedFocus=focus;auto& player=*reinterpret_cast<CPed**>(0xB7CD98);auto* savedPlayer=player;
 bool passed=true;uint32 pairs=0;int selectedCount=0,noneCount=0;
 {
 using ReversingProbe::EntryPatch;
 EntryPatch e1(0x7FB020,Destroy),d1(std::bit_cast<uintptr_t>(&RwRasterDestroy),Destroy),e2(0x7FB230,Create),d2(std::bit_cast<uintptr_t>(&RwRasterCreate),Create),e3(0x7F35D0,SetRaster),d3(std::bit_cast<uintptr_t>(&RwTextureSetRaster),SetRaster),e4(0x705D20,Gradient),d4(std::bit_cast<uintptr_t>(&CShadowCamera::MakeGradientRaster),Gradient),e5(0x706520,Setup),d5(std::bit_cast<uintptr_t>(&CRealTimeShadow::SetupForThisEntity),Setup);
 for(const auto& fn:fns){auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;const std::string full=std::string(fn.cls)+"::"+fn.name;
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);trace=json::array();rasterIndex=0;
 alignas(16) std::array<uint8,sizeof(CRealTimeShadowManager)> manager{};alignas(16) std::array<uint8,sizeof(CPed)> ped{},playerPed{};alignas(16) std::array<uint8,sizeof(CAutomobile)> vehicle{};
 manager.fill(0xcd);ped.fill(0xcd);playerPed.fill(0);vehicle.fill(0);auto* self=reinterpret_cast<CRealTimeShadowManager*>(manager.data());auto* entity=reinterpret_cast<CPhysical*>(ped.data());auto* p=reinterpret_cast<CPed*>(playerPed.data());auto* v=reinterpret_cast<CVehicle*>(vehicle.data());focus=0;player=p;
 p->bInVehicle=i%4!=0;p->m_pVehicle=i%7==0?nullptr:v;v->GetMoveSpeed()=CVector{i%5==0?0.3f:float(i%11)*0.03125f,float(i%9)*0.015625f,float(i%3)*0.0625f};
 At<uint8>(ped.data(),0x36)=i%3==0?3:2;At<int32>(ped.data(),0x598)=i%4==0?0:4;self->m_bInitialised=i%9!=0;
 for(int k=0;k<16;++k){shadows[k].fill(0xcd);auto* sh=reinterpret_cast<CRealTimeShadow*>(shadows[k].data());self->m_apShadows[k]=sh;sh->m_pOwner=(i%5==0 || ((uint32(i)*0x12345u)>>k)&1)?entity:nullptr;sh->m_camera.m_pRwCamera=&cameras[k*2];sh->m_camera.m_pRwRenderTexture=&textures[k*2];sh->m_blurCamera.m_pRwCamera=&cameras[k*2+1];sh->m_blurCamera.m_pRwRenderTexture=&textures[k*2+1];}
 self->m_BlurCamera.m_pRwCamera=&cameras[32];self->m_BlurCamera.m_pRwRenderTexture=&textures[32];self->m_GradientCamera.m_pRwCamera=&cameras[33];self->m_GradientCamera.m_pRwRenderTexture=&textures[33];
 for(int k=0;k<34;++k){cameras[k]={};oldRasters[k]={};newRasters[k]={};textures[k]={};oldRasters[k].width=16*((i+k)%8+1);oldRasters[k].height=oldRasters[k].width+7;cameras[k].frameBuffer=&oldRasters[k];textures[k].raster=&oldRasters[k];}
 const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;CRealTimeShadow* result{};
 if(fn.address==0x706870)reinterpret_cast<void(__thiscall*)(CRealTimeShadowManager*)>(fn.address)(self);else result=reinterpret_cast<CRealTimeShadow*(__thiscall*)(CRealTimeShadowManager*,CPhysical*)>(fn.address)(self,entity);
 json out{{"manager",ReversingProbe::Bytes(manager.data(),manager.size())},{"ped",ReversingProbe::Bytes(ped.data(),ped.size())},{"shadows",ReversingProbe::Bytes(shadows.data(),sizeof(shadows))},{"result",ShadowID(result)},{"trace",trace},{"switches",switched}};
 out["rasters"]=json::array();for(int k=0;k<34;++k)out["rasters"].push_back({cameras[k].frameBuffer==&newRasters[k],textures[k].raster==&newRasters[k]});return out;
 };
 const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;if(fn.address==0x706970){selectedCount+=original["result"]!=-1;noneCount+=original["result"]==-1;}
 Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 }
 focus=savedFocus;player=savedPlayer;const bool coverage=selectedCount&&noneCount;Write({{"event","coverage"},{"selected",selectedCount},{"none",noneCount},{"accepted",coverage}});Write({{"event",passed&&coverage?"complete":"error"},{"pairs",pairs},{"state_restored",focus==savedFocus&&player==savedPlayer}});
}
}
