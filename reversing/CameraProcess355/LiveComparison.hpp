#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "PedIntelligence.h"
#include "World.h"
#include "Collision/Collision.h"
#include "CullZones.h"
#include "WaterLevel.h"
#include "Draw.h"
#include "Renderer.h"
#include "Weather.h"
#include "Scene.h"
#include "../ProbeRuntime.hpp"
namespace CameraProcess355Live {
inline json trace;inline uint32 scenario{};inline CPlayerPed* player{};inline CVehicle* vehicle{};
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline json V(const CVector& v){return json::array({Bits(v.x),Bits(v.y),Bits(v.z)});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
inline void __cdecl Invisible(){trace.push_back({"invisible"});}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline CVehicle* __cdecl Vehicle(int32 id,bool remote){trace.push_back({"vehicle",id,remote});return scenario%3==0?nullptr:vehicle;}
inline void __fastcall Ref(CEntity*,void*,CEntity**){trace.push_back({"reference"});}
inline void __fastcall Target(CCamera*,void*){trace.push_back({"target"});}
inline void __fastcall Control(CCamera*,void*){trace.push_back({"control"});}
inline void __fastcall Move(CCamera*,void*){trace.push_back({"move"});}
inline void __fastcall Track(CCamera*,void*){trace.push_back({"track"});}
inline void __fastcall FovLerp(CCamera*,void*){trace.push_back({"fov_lerp"});}
inline void __fastcall Fade(CCamera*,void*){trace.push_back({"fade"});}
inline void __fastcall Music(CCamera*,void*){trace.push_back({"music"});}
inline void __fastcall Wide(CCamera*,void*){trace.push_back({"wide"});}
inline RwCamera* __cdecl Near(RwCamera* camera,float value){trace.push_back({"near",Bits(value)});return camera;}
inline RwCamera* __cdecl Far(RwCamera* camera,float value){trace.push_back({"far",Bits(value)});return camera;}
inline void __fastcall Cam(CCam* camera,void*){trace.push_back({"cam",V(camera->m_vecFront),V(camera->m_vecSource)});camera->m_vecFront.x+=scenario%3==0?.5f:.05f;camera->m_vecSource.y+=.125f;}
inline CVector* __fastcall Shake(CCamera* camera,void*,float ratio){trace.push_back({"shake",Bits(ratio)});return &camera->m_vecGameCamPos;}
inline void __fastcall Store(CCamera* camera,void*,CVector* source,CVector* target,CVector* up,float* fov){trace.push_back({"store",V(*source),V(*target),V(*up),Bits(*fov)});camera->m_vecSourceDuringInter=*source;camera->m_vecTargetDuringInter=*target;camera->m_vecUpDuringInter=*up;camera->m_fFOVDuringInter=*fov+.25f;}
inline void __fastcall Speed(CCam*,void*,const CVector& source,const CVector& target,const CVector& up,const float& alpha,const float& beta,const float& fov){trace.push_back({"speed",V(source),V(target),V(up),Bits(alpha),Bits(beta),Bits(fov)});}
inline bool __cdecl Stairs(){trace.push_back({"stairs"});return scenario%5==0;}
inline bool __cdecl Line(const CVector& from,const CVector& to,CColPoint& cp,CEntity*& entity,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){trace.push_back({"line",V(from),V(to),b0,b1,b2,b3,b4,b5,b6,b7});std::memset(&cp,0,sizeof(cp));cp.m_vecPoint=CVector{2,3,4};entity=nullptr;return scenario%3!=0;}
inline int __cdecl Random(){const int value=int((scenario*7919+33)%32768);trace.push_back({"random",value});return value;}
inline void __cdecl Fov(float value){trace.push_back({"fov",Bits(value)});CDraw::ms_fFOV=value;}
inline void __fastcall Derived(CCamera*,void*,bool mirror,bool oriented){trace.push_back({"derived",mirror,oriented});}
inline void __fastcall Copy(CCamera*,void*,bool update){trace.push_back({"copy",update});}
inline void __fastcall Sound(CCamera*,void*){trace.push_back({"sound"});}
inline bool __cdecl Water(float x,float y,float z,float& level,uint8 touching,CVector* normals){trace.push_back({"water",Bits(x),Bits(y),Bits(z),touching,normals==nullptr});level=z+float(int32(scenario%7)-3)*.25f;return scenario%4!=0;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCamera","Process",0x52B730,0,-1,16384}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-process355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-process355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-process355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch invisible(0x415540,Invisible),invisibled(bit_cast<uintptr_t>(&ResetMadeInvisibleObjects),Invisible);
 EntryPatch playerPatch(0x56E210,Player),playerd(bit_cast<uintptr_t>(&FindPlayerPed),Player),vehiclePatch(0x56E0D0,Vehicle),vehicled(bit_cast<uintptr_t>(&FindPlayerVehicle),Vehicle);
 EntryPatch reference(0x571B70,Ref),referenced(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
 EntryPatch target(0x50C360,Target),targetd(bit_cast<uintptr_t>(&CCamera::UpdateTargetEntity),Target),control(0x527FA0,Control),controld(bit_cast<uintptr_t>(&CCamera::CamControl),Control);
 EntryPatch move(0x5164A0,Move),moved(bit_cast<uintptr_t>(static_cast<void(CCamera::*)()>(&CCamera::ProcessVectorMoveLinear)),Move),track(0x516440,Track),trackd(bit_cast<uintptr_t>(static_cast<void(CCamera::*)()>(&CCamera::ProcessVectorTrackLinear)),Track),fovlerp(0x516500,FovLerp),fovlerpd(bit_cast<uintptr_t>(static_cast<void(CCamera::*)()>(&CCamera::ProcessFOVLerp)),FovLerp);
 EntryPatch fade(0x50B5D0,Fade),faded(bit_cast<uintptr_t>(&CCamera::ProcessFade),Fade),music(0x50B6D0,Music),musicd(bit_cast<uintptr_t>(&CCamera::ProcessMusicFade),Music),wide(0x50B890,Wide),wided(bit_cast<uintptr_t>(&CCamera::ProcessWideScreenOn),Wide);
 EntryPatch nearPatch(0x7EE1D0,Near),neard(bit_cast<uintptr_t>(&RwCameraSetNearClipPlane),Near),farPatch(0x7EE2A0,Far),fard(bit_cast<uintptr_t>(&RwCameraSetFarClipPlane),Far);
 EntryPatch cam(0x526FC0,Cam),camd(bit_cast<uintptr_t>(&CCam::Process),Cam),shake(0x516560,Shake),shaked(bit_cast<uintptr_t>(static_cast<CVector*(CCamera::*)(float)>(&CCamera::ProcessShake)),Shake);
 EntryPatch store(0x50C290,Store),stored(bit_cast<uintptr_t>(&CCamera::StoreValuesDuringInterPol),Store),speed(0x509DF0,Speed),speedd(bit_cast<uintptr_t>(&CCam::KeepTrackOfTheSpeed),Speed);
 EntryPatch stairs(0x72DD30,Stairs),stairsd(bit_cast<uintptr_t>(&CCullZones::CamStairsForPlayer),Stairs),line(0x56BA00,Line),lined(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line),random(0x821B1E,Random),randomd(bit_cast<uintptr_t>(&std::rand),Random);
 EntryPatch fov(0x6FF410,Fov),fovd(bit_cast<uintptr_t>(&CDraw::SetFOV),Fov),derived(0x5150E0,Derived),derivedd(bit_cast<uintptr_t>(&CCamera::CalculateDerivedValues),Derived),copy(0x50AFA0,Copy),copyd(bit_cast<uintptr_t>(&CCamera::CopyCameraMatrixToRWCam),Copy),sound(0x515BD0,Sound),soundd(bit_cast<uintptr_t>(&CCamera::UpdateSoundDistances),Sound);
 EntryPatch water(0x6EB690,Water),waterd(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),Water);
 const auto savedMS=CTimer::GetTimeInMS();const auto savedScene=Scene.m_pRwCamera;const auto savedFov=CDraw::ms_fFOV,savedLod=CRenderer::ms_lodDistScale,savedDrunk=StaticRef<float>(0xC73C58),savedAngle=StaticRef<float>(0xB6EC30),savedUnder=CWeather::UnderWaterness,savedDepth=CWeather::WaterDepth;std::array<uint8,4> savedFlags;std::memcpy(savedFlags.data(),reinterpret_cast<void*>(0xB70140),4);const auto savedFarA=StaticRef<float>(0xC3EFA0),savedFarB=StaticRef<float>(0xC3EF9C);const uint8 savedViewA=StaticRef<uint8>(0xB5F851),savedViewB=StaticRef<uint8>(0xB5F853);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> selfBytes{};auto* self=reinterpret_cast<CCamera*>(selfBytes.data());alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> initialMatrix{},entityMatrix[3]{};alignas(16) std::array<uint8,4096> entity[3]{};alignas(RwCamera) std::array<uint8,512> rw{};
    player=reinterpret_cast<CPlayerPed*>(entity[0].data());vehicle=reinterpret_cast<CVehicle*>(entity[2].data());entity[0][0x36]=uint8(i%3==0?2:i%3==1?3:4);entity[1][0x36]=3;entity[2][0x36]=2;
    for(size_t n=0;n<3;++n){At<uintptr_t>(entity[n].data(),0x14)=uintptr_t(entityMatrix[n].data());At<CVector>(entityMatrix[n].data(),0)=CVector{1,0,0};At<CVector>(entityMatrix[n].data(),0x10)=CVector{0,1,0};At<CVector>(entityMatrix[n].data(),0x20)=CVector{0,0,n==2&&i%2?.1f:1.f};At<CVector>(entityMatrix[n].data(),0x30)=CVector{1,2,3};}
    At<CVector>(initialMatrix.data(),0)=CVector{1,0,0};At<CVector>(initialMatrix.data(),0x10)=CVector{0,1,.25f};At<CVector>(initialMatrix.data(),0x20)=CVector{0,0,1};At<CVector>(initialMatrix.data(),0x30)=CVector{4,5,6};self->m_placement.m_vPosn=CVector{4,5,6};self->m_placement.m_fHeading=float(i%7)*.25f;self->m_matrix=i%5==0?nullptr:i%5==1?reinterpret_cast<CMatrixLink*>(&self->m_mCameraMatrix):reinterpret_cast<CMatrixLink*>(initialMatrix.data());
    self->m_nActiveCam=uint8(i%2);self->m_pTargetEntity=i%5==0?nullptr:reinterpret_cast<CEntity*>(entity[0].data());self->m_pRwCamera=reinterpret_cast<RwCamera*>(rw.data());Scene.m_pRwCamera=self->m_pRwCamera;At<float>(rw.data(),0x80)=.3f;At<float>(rw.data(),0x84)=50.1234f;
    for(size_t n=0;n<2;++n){auto& cam=self->m_aCams[n];cam.m_pCamTargetEntity=i%4==0?nullptr:reinterpret_cast<CEntity*>(entity[n].data());constexpr std::array<int,4> modes{1,37,16,4};cam.m_nMode=static_cast<eCamMode>(modes[size_t((i/2)%4)]);cam.m_nDirectionWasLooking=uint32(i%3==0?3:1);cam.m_vecFront=CVector{.25f,1,.125f};cam.m_vecSource=CVector{4,5,6};cam.m_vecSourceBeforeLookBehind=CVector{7,8,9};cam.m_vecUp=CVector{0,0,1};cam.m_vecTargetCoorsForFudgeInter=CVector{1,2,3};cam.m_fFOV=70.f+float(i%5);}
    self->m_bLookingAtPlayer=i%3!=0;self->m_bLookingAtVector=i%7==0;self->m_bTargetJustBeenOnTrain=i%11==0;self->m_nWhoIsInControlOfTheCamera=uint32(i%4);self->m_bFading=i%2!=0;self->m_bMusicFading=i%3!=0;self->m_bWideScreenOn=i%4!=0;self->m_bUseNearClipScript=i%5!=0;self->m_fNearClipScript=.2f;self->m_bMoveCamToAvoidGeom=i%3==0;self->m_vecClearGeometryVec=CVector{.125f,.25f,.375f};self->m_bTransitionState=i%2!=0;self->m_bDoingSpecialInterp=true;self->m_bWaitForInterpolToFinish=true;self->m_bPlayerIsInGarage=i%7==0;self->m_bJustInitialized=i%11==0;self->m_bCameraJustRestored=true;
    self->m_nTimeTransitionStart=100;self->m_nTransitionDuration=uint32(i%5==0?500:i%5==1?1000:1800);self->m_nTransitionDurationTargetCoors=1200;self->m_fFractionInterToStopMoving=float((i/3)%3)*.25f;self->m_fFractionInterToStopCatchUp=.75f;self->m_fFractionInterToStopMovingTarget=float((i/5)%3)*.25f;self->m_fFractionInterToStopCatchUpTarget=.75f;
    self->m_vecStartingSourceForInterPol=i%13==0?CVector{1.1f,2.1f,3}:CVector{4,5,6};self->m_vecStartingTargetForInterPol=CVector{1,2,3};self->m_vecStartingUpForInterPol=CVector{0,0,1};self->m_vecSourceSpeedAtStartInter=CVector{.25f,.5f,.75f};self->m_vecTargetSpeedAtStartInter=CVector{.125f,.25f,.375f};self->m_vecUpSpeedAtStartInter=CVector{.125f,.25f,.375f};self->m_vecSourceWhenInterPol=i%13==0?CVector{1.1f,2.1f,3}:CVector{4,5,6};self->m_vecTargetWhenInterPol=CVector{1,2,3};self->m_vecUpWhenInterPol=CVector{0,0,1};self->m_fStartingFOVForInterPol=50.f;self->m_fFOVDuringInter=60.f;self->m_fFOVSpeedAtStartInter=2.f;
    self->m_fStartShakeTime=100.f;self->m_fEndShakeTime=i%3==0?900.f:2000.f;self->m_nCamShakeStart=uint32(i%3==0?0:900);self->m_fCamShakeForce=float(i%6)*.5f;self->m_nBlurType=static_cast<eMotionBlurType>(i%3);self->m_nWorkOutSpeedThisNumFrames=4;self->m_nNumFramesSoFar=uint32(i%4);self->m_fCameraSpeedSoFar=2.f;self->m_fOrientation=.5f;self->m_vecPreviousCameraPosition=CVector{4,5,6};
    self->m_mCameraMatrix.GetRight()=CVector{1,0,0};self->m_mCameraMatrix.GetForward()=CVector{0,1,0};self->m_mCameraMatrix.GetUp()=CVector{0,0,1};self->m_mCameraMatrix.GetPosition()=CVector{4,5,6};StaticRef<uint32>(0xB7CB84)=1000;CDraw::ms_fFOV=70.f;CRenderer::ms_lodDistScale=1.2f;StaticRef<float>(0xC73C58)=i%3==0?0.f:float(i%5)*.125f;StaticRef<float>(0xB6EC30)=float(i%4)*45.f;CWeather::UnderWaterness=.75f;CWeather::WaterDepth=4.5f;std::memset(reinterpret_cast<void*>(0xB70140),0,4);StaticRef<bool>(0xB70142)=i%3!=0;StaticRef<bool>(0xB70143)=true;StaticRef<uint8>(0xB5F851)=uint8(i%2);StaticRef<uint8>(0xB5F853)=uint8(i%3==0);
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCamera*)>(fn.address)(self);
    return json{{"vectors",json::array({V(self->m_mCameraMatrix.GetRight()),V(self->m_mCameraMatrix.GetForward()),V(self->m_mCameraMatrix.GetUp()),V(self->m_mCameraMatrix.GetPosition()),V(self->m_vecSourceWhenInterPol),V(self->m_vecTargetWhenInterPol),V(self->m_vecUpWhenInterPol)})},{"scalars",json::array({F(self->m_fFOVDuringInter),F(self->m_fOrientation),F(self->m_fCameraSpeedSoFar),F(self->m_fCameraAverageSpeed),self->m_nMotionBlurAddAlpha})},{"camera",Digest(selfBytes.data(),selfBytes.size())},{"initial_matrix",Digest(initialMatrix.data(),initialMatrix.size())},{"entities",Digest(entity,sizeof(entity))},{"rw",Digest(rw.data(),rw.size())},{"fov",F(CDraw::ms_fFOV)},{"drunk_angle",F(StaticRef<float>(0xB6EC30))},{"under",F(CWeather::UnderWaterness)},{"depth",F(CWeather::WaterDepth)},{"flags",Digest(reinterpret_cast<void*>(0xB70140),4)},{"far_a",F(StaticRef<float>(0xC3EFA0))},{"far_b",F(StaticRef<float>(0xC3EF9C))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uint32>(0xB7CB84)=savedMS;Scene.m_pRwCamera=savedScene;CDraw::ms_fFOV=savedFov;CRenderer::ms_lodDistScale=savedLod;StaticRef<float>(0xC73C58)=savedDrunk;StaticRef<float>(0xB6EC30)=savedAngle;CWeather::UnderWaterness=savedUnder;CWeather::WaterDepth=savedDepth;std::memcpy(reinterpret_cast<void*>(0xB70140),savedFlags.data(),4);StaticRef<float>(0xC3EFA0)=savedFarA;StaticRef<float>(0xC3EF9C)=savedFarB;StaticRef<uint8>(0xB5F851)=savedViewA;StaticRef<uint8>(0xB5F853)=savedViewB;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
