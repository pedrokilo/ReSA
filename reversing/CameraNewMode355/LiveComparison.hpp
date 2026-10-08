#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "Cam.h"
#include "HandShaker.h"
#include "ColModel.h"
#include "BaseModelInfo.h"
#include "WaterLevel.h"
#include "Entity/Ped/PlayerPed.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CameraNewMode355Live {
inline json trace;inline uint32 scenario{};inline CVehicle* vehicle{};inline CPlayerPed* ped{};inline CEntity* entity{};inline std::array<CVehicle*,4> poolVehicles{};inline CVector playerPosition{},playerSpeed{};
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline CVehicle* __cdecl Vehicle(int32 id,bool remote){trace.push_back({"vehicle",id,remote});return scenario%5==0?nullptr:vehicle;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"ped",id});return ped;}
inline CEntity* __cdecl Entity(int32 id){trace.push_back({"entity",id});return entity;}
inline CVector* __cdecl Coors(CVector* out,int32 id){trace.push_back({"coors",id});*out=playerPosition;return out;}
inline CVector& __cdecl Speed(int32 id){trace.push_back({"speed",id});return playerSpeed;}
inline eWantedLevel __fastcall Wanted(const CPlayerPed*,void*){trace.push_back({"wanted"});return static_cast<eWantedLevel>(scenario%3);}
inline CVehicle* __fastcall Pool(void*,void*,int32 index){trace.push_back({"pool",index});return (scenario+uint32(index))%5==0?nullptr:poolVehicles[size_t(index)];}
inline void __fastcall Fixed(CCamera* camera,void*,const CVector& position,const CVector& up){trace.push_back({"fixed",F(position.x),F(position.y),F(position.z),F(up.x),F(up.y),F(up.z)});camera->m_vecFixedModeSource=position;camera->m_vecFixedModeUpOffSet=up;}
inline void __fastcall Control(CCamera* camera,void*,CEntity* target,eCamMode mode,eSwitchType change,int32 who){trace.push_back({"control",uintptr_t(target),int(mode),int(change),who});camera->m_pTargetEntity=target;}
inline bool __cdecl Reject(){trace.push_back({"reject"});return scenario%4==0;}
inline bool __cdecl Clear(const CVector& a,const CVector& b,bool bu,bool ve,bool pe,bool ob,bool du,bool se,bool ca){trace.push_back({"clear",F(a.x),F(a.y),F(a.z),F(b.x),F(b.y),F(b.z),bu,ve,pe,ob,du,se,ca});return scenario%6!=0;}
inline float __cdecl Ground(CVector position,bool* found,CEntity** hit){trace.push_back({"ground",F(position.x),F(position.y),F(position.z),uintptr_t(hit)});*found=scenario%3!=0;return scenario%7==0?1.f:float(int32(scenario%11)-5);}
inline float __cdecl Roof(float x,float y,float z,bool* found){trace.push_back({"roof",F(x),F(y),F(z)});*found=scenario%3==0;return float(int32(scenario%9)-4);}
inline bool __cdecl Water(CVector position,float* out,float* a,float* b){trace.push_back({"water",F(position.x),F(position.y),F(position.z),uintptr_t(a),uintptr_t(b)});*out=float(scenario%5);return scenario%4!=0;}
inline RwCamera* __cdecl Near(RwCamera* camera,float value){trace.push_back({"near",F(value)});camera->nearPlane=value;return camera;}
inline bool Ciney(int32 kind,bool start){trace.push_back({"ciney",kind,start});return scenario%3!=0;}
inline bool __fastcall Heli(CCam*,void*,bool start){return Ciney(20,start);}
inline bool __fastcall Man(CCam*,void*,bool start){return Ciney(21,start);}
inline bool __fastcall Bird(CCam*,void*,bool start){return Ciney(22,start);}
inline bool __fastcall Spot(CCam*,void*,bool start){return Ciney(23,start);}
inline bool __fastcall Plane1(CCam*,void*,bool start){return Ciney(26,start);}
inline bool __fastcall Plane2(CCam*,void*,bool start){return Ciney(27,start);}
inline bool __fastcall Plane3(CCam*,void*,bool start){return Ciney(28,start);}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }

struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCamera","TryToStartNewCamMode",0x51E560,0,-1,16384}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-new-mode355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-new-mode355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-new-mode355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedPool=StaticRef<uintptr_t>(0xB74494),savedRw=StaticRef<uintptr_t>(0xC1703C);const auto savedActive=StaticRef<uint8>(0xB6F059);const auto savedFloor=StaticRef<float>(0x8CC8C0),savedBoatFloor=StaticRef<float>(0x8CC8C4);
 EntryPatch ve(0x56E0D0,Vehicle),ved(bit_cast<uintptr_t>(&FindPlayerVehicle),Vehicle);
 EntryPatch pl(0x56E210,Player),pld(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch en(0x56E120,Entity),end(bit_cast<uintptr_t>(&FindPlayerEntity),Entity);
 EntryPatch co(0x56E010,Coors),cod(bit_cast<uintptr_t>(&FindPlayerCoors),Coors);
 EntryPatch sp(0x56E090,Speed),spd(bit_cast<uintptr_t>(&FindPlayerSpeed),Speed);
 EntryPatch wa(0x41BE60,Wanted),wad(bit_cast<uintptr_t>(&CPlayerPed::GetWantedLevel),Wanted);
 EntryPatch po(0x41CC10,Pool);
 EntryPatch fi(0x50BEC0,Fixed),fid(bit_cast<uintptr_t>(&CCamera::SetCamPositionForFixedMode),Fixed);
 EntryPatch ct(0x50C7C0,Control),ctd(bit_cast<uintptr_t>(&CCamera::TakeControl),Control);
 EntryPatch re(0x50B830,Reject);
 EntryPatch cl(0x56A490,Clear),cld(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Clear);
 EntryPatch gr(0x5696C0,Ground),grd(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),Ground);
 EntryPatch ro(0x569750,Roof),rod(bit_cast<uintptr_t>(&CWorld::FindRoofZFor3DCoord),Roof);
 EntryPatch wt(0x6E8580,Water),wtd(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWaves),Water);
 EntryPatch ne(0x7EE1D0,Near),ned(bit_cast<uintptr_t>(&RwCameraSetNearClipPlane),Near);
 EntryPatch h(0x51A740,Heli),hd(bit_cast<uintptr_t>(&CCam::Process_DW_HeliChaseCam),Heli);
 EntryPatch m(0x51B120,Man),md(bit_cast<uintptr_t>(&CCam::Process_DW_CamManCam),Man);
 EntryPatch b(0x51B850,Bird),bd(bit_cast<uintptr_t>(&CCam::Process_DW_BirdyCam),Bird);
 EntryPatch s(0x51C250,Spot),sd(bit_cast<uintptr_t>(&CCam::Process_DW_PlaneSpotterCam),Spot);
 EntryPatch p1(0x51C760,Plane1),p1d(bit_cast<uintptr_t>(&CCam::Process_DW_PlaneCam1),Plane1);
 EntryPatch p2(0x51CC30,Plane2),p2d(bit_cast<uintptr_t>(&CCam::Process_DW_PlaneCam2),Plane2);
 EntryPatch p3(0x51D100,Plane3),p3d(bit_cast<uintptr_t>(&CCam::Process_DW_PlaneCam3),Plane3);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i%512);trace=json::array();
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(raw.data());alignas(16) std::array<uint8,4096> targets[6]{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrices[5]{};alignas(4) std::array<uint8,24> pool{};RwCamera rw{};
    for(int k=0;k<5;++k){At<uintptr_t>(targets[k].data(),0x14)=uintptr_t(matrices[k].data());At<CVector>(matrices[k].data(),0)=CVector{1,0,0};At<CVector>(matrices[k].data(),0x10)=CVector{0,1,0};At<CVector>(matrices[k].data(),0x20)=CVector{0,0,1};At<CVector>(matrices[k].data(),0x30)=CVector{float(k*2),float(k*3)-8,1};At<int32>(targets[k].data(),0x590)=k==0&&scenario%3==0?5:0;At<int16>(targets[k].data(),0x22)=int16(scenario%3==0?460:scenario%3==1?432:411);targets[k][0x36]=uint8((k+scenario)%3==0?0x1A:2);targets[k][0x428]=uint8((k+scenario)%3!=1);}
    vehicle=reinterpret_cast<CVehicle*>(targets[0].data());entity=reinterpret_cast<CEntity*>(vehicle);ped=reinterpret_cast<CPlayerPed*>(targets[5].data());for(int k=0;k<4;++k)poolVehicles[k]=reinterpret_cast<CVehicle*>(targets[k+1].data());At<int32>(pool.data(),8)=4;
    self->m_pTargetEntity=entity;self->m_nActiveCam=uint8(scenario%3);playerPosition=CVector{float(int32(scenario%11)-5),float(int32(scenario%7)-3),float(scenario%5)};playerSpeed=CVector{float(int32(scenario%9)-4)*.5f,float(int32(scenario%13)-6)*.25f,float(scenario%3)*.125f};
    StaticRef<uintptr_t>(0xB74494)=uintptr_t(pool.data());StaticRef<uintptr_t>(0xC1703C)=uintptr_t(&rw);StaticRef<uint8>(0xB6F059)=uint8(scenario%2);StaticRef<float>(0x8CC8C0)=.5f;StaticRef<float>(0x8CC8C4)=1.5f;rw.nearPlane=.3f;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const int32 sequence=int32(i/512)-1;const bool result=reinterpret_cast<bool(__thiscall*)(CCamera*,int32)>(fn.address)(self,sequence);
    return json{{"camera",Digest(raw.data(),raw.size())},{"entities",Digest(targets,sizeof(targets))},{"matrices",Digest(matrices,sizeof(matrices))},{"rw",Digest(&rw,sizeof(rw))},{"active",StaticRef<uint8>(0xB6F059)},{"sequence",sequence},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xB74494)=savedPool;StaticRef<uintptr_t>(0xC1703C)=savedRw;StaticRef<uint8>(0xB6F059)=savedActive;StaticRef<float>(0x8CC8C0)=savedFloor;StaticRef<float>(0x8CC8C4)=savedBoatFloor;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
