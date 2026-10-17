#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleArrestPed.h"
#include "TaskSimpleJetPack.h"
#include "Garages.h"
#include "DummyObject.h"
#include "GameLogic.h"
#include "CullZones.h"
#include "AudioEngine.h"
#include "../ProbeRuntime.hpp"
namespace CamControl355Live {
inline json trace;inline uint32 scenario{};
inline CPlayerPed* player{};inline CVehicle* vehicle{};inline CPad* pad{};
inline CAttributeZone* zone{};inline CObject* firstDoor{};inline CObject* secondDoor{};
inline CTaskSimpleHoldEntity* hold{};inline CTaskSimpleArrestPed* arrest{};inline CTaskSimpleJetPack* jet{};
inline bool garageCase{},stairsCase{};
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline json V(const CVector& v){return json::array({Bits(v.x),Bits(v.y),Bits(v.z)});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
template<class T=uint32>T& At(void* data,size_t offset){return *reinterpret_cast<T*>(static_cast<uint8*>(data)+offset);}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline CVehicle* __cdecl Vehicle(int32 id,bool remote){trace.push_back({"vehicle",id,remote});return scenario%7==0?nullptr:vehicle;}
inline CPad* __cdecl Pad(int32 id){trace.push_back({"pad",id});return pad;}
inline void __fastcall Reference(CEntity*,void*,CEntity**){trace.push_back({"reference"});}
inline void __fastcall Cleanup(CEntity*,void*,CEntity**){trace.push_back({"cleanup"});}
inline bool __cdecl Close(){trace.push_back({"close"});return scenario%3==0;}
inline bool __cdecl Stairs(){trace.push_back({"stairs"});return stairsCase;}
inline CAttributeZone* __cdecl Zone(){trace.push_back({"zone"});return zone;}
inline bool __cdecl Coop(){trace.push_back({"coop"});return scenario%13==0;}
inline bool __fastcall Cycle(CPad*,void*){trace.push_back({"cycle_up"});return scenario%5==0;}
inline bool __fastcall CycleDown(const CPad*,void*){trace.push_back({"cycle_down"});return scenario%7==0;}
inline bool __fastcall SoundButton(const CPad*,void*){trace.push_back({"sound_button"});return scenario%2==0;}
inline bool __fastcall Target(const CPad*,void*){trace.push_back({"target_button"});return scenario%3!=0;}
inline bool __fastcall Enter(const CPad*,void*){trace.push_back({"enter_target"});return scenario%4==0;}
inline int16 __fastcall WalkX(const CPad*,void*){trace.push_back({"walk_x"});return scenario%11==0?int16{25}:int16{0};}
inline int16 __fastcall WalkY(const CPad*,void*){trace.push_back({"walk_y"});return scenario%17==0?int16{-25}:int16{0};}
inline int16 __fastcall StickX(const CPad*,void*){trace.push_back({"stick_x"});return scenario%19==0?int16{50}:int16{0};}
inline int16 __fastcall StickY(const CPad*,void*){trace.push_back({"stick_y"});return 0;}
inline bool __fastcall Mouse(const CCam*,void*){trace.push_back({"mouse"});return scenario%7==0;}
inline bool __fastcall First(CCam*,void*){trace.push_back({"first_person"});return scenario%5==0;}
inline bool __fastcall InControl(const CPed*,void*){trace.push_back({"ped_control"});return scenario%23!=0;}
inline bool __fastcall RunFire(CPed*,void*){trace.push_back({"run_fire"});return scenario%3!=0;}
inline CTask* __fastcall Secondary(CTaskManager*,void*,eSecondaryTask type){trace.push_back({"secondary",uint32(type)});return scenario%11==0?reinterpret_cast<CTask*>(hold):nullptr;}
inline CTaskSimpleHoldEntity* __fastcall Hold(CPedIntelligence*,void*,bool ignore){trace.push_back({"hold",ignore});return scenario%3==0?hold:nullptr;}
inline CTaskSimpleJetPack* __fastcall Jet(CPedIntelligence*,void*){trace.push_back({"jet"});return scenario%5==0?jet:nullptr;}
inline CTask* __fastcall Arrest(CTaskManager*,void*,eTaskType type){trace.push_back({"arrest",int32(type)});return scenario%3==0?arrest:nullptr;}
inline void __fastcall Restore(CCamera*,void*){trace.push_back({"restore"});}
inline void __fastcall RestoreJump(CCamera*,void*){trace.push_back({"restore_jump"});}
inline void __fastcall Behind(CCamera*,void*){trace.push_back({"behind"});}
inline void __fastcall ClearWeapon(CCamera* self,void*){trace.push_back({"clear_weapon"});self->m_PlayerWeaponMode.m_nMode=0;}
inline void __fastcall Transition(CCamera* self,void*,eCamMode mode){trace.push_back({"transition",uint16(mode)});self->m_aCams[self->m_nActiveCam].m_nMode=mode;self->m_bTransitionState=true;}
inline void __fastcall Fixed(CCamera* self,void*,const CVector& source,const CVector& up){trace.push_back({"fixed",V(source),V(up)});self->m_vecFixedModeSource=source;self->m_vecFixedModeUpOffSet=up;}
inline bool __cdecl Garage(CVector position){trace.push_back({"garage",V(position)});return garageCase;}
inline void __fastcall Doors(CGarage*,void*,CObject** first,CObject** second){trace.push_back({"doors",firstDoor!=nullptr,secondDoor!=nullptr});*first=firstDoor;*second=secondDoor;}
inline CVector* __fastcall ZonePosition(CAttributeZone*,void*,CVector* out){trace.push_back({"zone_position"});*out=CVector{2,3,4};return out;}
inline uintptr_t Body(uintptr_t at){for(int i=0;i<16&&*reinterpret_cast<const uint8*>(at)==0xE9;++i)at+=5+*reinterpret_cast<const int32*>(at+1);return at;}
inline float __cdecl Ground(CVector position,bool* found,CEntity** entity){trace.push_back({"ground",V(position),found!=nullptr,entity==nullptr});*found=scenario%4!=0;return 2.25f;}
inline bool __cdecl Clear(const CVector& from,const CVector& to,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6){trace.push_back({"clear",V(from),V(to),b0,b1,b2,b3,b4,b5,b6});return scenario%3!=0;}
inline bool __cdecl Line(const CVector& from,const CVector& to,CColPoint& cp,CEntity*& entity,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){trace.push_back({"line",V(from),V(to),b0,b1,b2,b3,b4,b5,b6,b7});std::memset(&cp,0,sizeof(cp));cp.m_vecPoint=CVector{2,3,4};entity=nullptr;return scenario%3!=0;}
inline void __cdecl Buffer(float target,float& value,float& speed,float maximum,float step,bool angle){trace.push_back({"buffer",Bits(target),Bits(value),Bits(speed),Bits(maximum),Bits(step),angle});value=target+value*.5f;speed=.25f;}
inline eVehicleAppearance __fastcall Appearance(const CVehicle*,void*){trace.push_back({"appearance"});return static_cast<eVehicleAppearance>(scenario%5);}
inline bool __fastcall ArrayPosition(CCamera*,void*,eVehicleType type,int32& index){trace.push_back({"array_position",int32(type)});index=int32(scenario%5);return true;}
inline float __fastcall Heading(CPlaceable*,void*){trace.push_back({"heading"});return float(scenario%7)*.25f;}
inline float __cdecl Azimuth(float x,float y){trace.push_back({"azimuth",Bits(x),Bits(y)});return float(scenario%5)*.375f;}
inline void __fastcall Audio(CAudioEngine*,void*,eAudioEvents event,float volume,float speed){trace.push_back({"audio",int32(event),Bits(volume),Bits(speed)});}
inline void Cinema(uint32 kind){trace.push_back({"cinema",kind});StaticRef<uint16>(0xB70140)=uint16(18+kind);StaticRef<uint8>(0xB6EC34)=1;}
inline void __fastcall CinemaCar(CCamera*,void*){Cinema(0);}
inline void __fastcall CinemaTrain(CCamera*,void*){Cinema(1);}
inline void __fastcall CinemaHeli(CCamera*,void*){Cinema(2);}
inline void __fastcall CinemaPlane(CCamera*,void*){Cinema(3);}
inline void __fastcall CinemaBoat(CCamera*,void*){Cinema(4);}
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCamera","CamControl",0x527FA0,0,-1,24576}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();});return found;}

inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-control355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-control355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-control355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch playerPatch(0x56E210,Player),playerDll(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch vehiclePatch(0x56E0D0,Vehicle),vehicleDll(bit_cast<uintptr_t>(&FindPlayerVehicle),Vehicle);
 EntryPatch padPatch(0x53FB70,Pad),padDll(bit_cast<uintptr_t>(&CPad::GetPad),Pad);
 EntryPatch referencePatch(0x571B70,Reference),referenceDll(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Reference);
 EntryPatch cleanupPatch(0x571A00,Cleanup),cleanupDll(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Cleanup);
 EntryPatch closePatch(0x72DD20,Close),closeDll(bit_cast<uintptr_t>(&CCullZones::CamCloseInForPlayer),Close);
 EntryPatch stairsPatch(0x72DD30,Stairs),stairsDll(bit_cast<uintptr_t>(&CCullZones::CamStairsForPlayer),Stairs);
 EntryPatch zonePatch(0x72DAD0,Zone),zoneDll(bit_cast<uintptr_t>(&CCullZones::FindZoneWithStairsAttributeForPlayer),Zone);
 EntryPatch coopPatch(0x441390,Coop),coopDll(bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn),Coop);
 EntryPatch soundButtonPatch(0x540530,SoundButton),soundButtonDll(bit_cast<uintptr_t>(&CPad::sub_540530),SoundButton);
 EntryPatch targetButtonPatch(0x540670,Target),targetButtonDll(bit_cast<uintptr_t>(&CPad::GetTarget),Target);
 EntryPatch enterTargetPatch(0x5406B0,Enter),enterTargetDll(bit_cast<uintptr_t>(&CPad::GetEnterTargeting),Enter);
 EntryPatch walkXPatch(0x53FC90,WalkX),walkXDll(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkLeftRight)),WalkX);
 EntryPatch walkYPatch(0x53FD30,WalkY),walkYDll(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkUpDown)),WalkY);
 EntryPatch mousePatch(0x50A850,Mouse),mouseDll(bit_cast<uintptr_t>(&CCam::Using3rdPersonMouseCam),Mouse);
 EntryPatch firstPatch(0x509DC0,First),firstDll(bit_cast<uintptr_t>(&CCam::GetWeaponFirstPersonOn),First);
 EntryPatch inControlPatch(0x5E3960,InControl),inControlDll(bit_cast<uintptr_t>(&CPed::IsPedInControl),InControl);
 EntryPatch runFirePatch(0x5E88E0,RunFire),runFireDll(bit_cast<uintptr_t>(&CPed::CanWeRunAndFireWithWeapon),RunFire);
 EntryPatch secondaryPatch(0x681810,Secondary),secondaryDll(bit_cast<uintptr_t>(&CTaskManager::GetTaskSecondary),Secondary);
 EntryPatch holdPatch(0x600FF0,Hold),holdDll(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskHold),Hold);
 EntryPatch jetPatch(0x601110,Jet),jetDll(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskJetPack),Jet);
 EntryPatch arrestPatch(0x681740,Arrest),arrestDll(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),Arrest);
 EntryPatch restorePatch(0x50B930,Restore),restoreDll(bit_cast<uintptr_t>(&CCamera::Restore),Restore);
 EntryPatch restoreJumpPatch(0x50BAB0,RestoreJump),restoreJumpDll(bit_cast<uintptr_t>(&CCamera::RestoreWithJumpCut),RestoreJump);
 EntryPatch behindPatch(0x50BD40,Behind),behindDll(bit_cast<uintptr_t>(&CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString),Behind);
 EntryPatch clearWeaponPatch(0x50AB10,ClearWeapon),clearWeaponDll(bit_cast<uintptr_t>(&CCamera::ClearPlayerWeaponMode),ClearWeapon);
 EntryPatch transitionPatch(0x515200,Transition),transitionDll(bit_cast<uintptr_t>(&CCamera::StartTransition),Transition);
 EntryPatch fixedPatch(0x50BEC0,Fixed),fixedDll(bit_cast<uintptr_t>(&CCamera::SetCamPositionForFixedMode),Fixed);
 EntryPatch garagePatch(0x449BA0,Garage),garageDll(bit_cast<uintptr_t>(&CGarages::IsPointInAGarageCameraZone),Garage);
 EntryPatch groundPatch(0x5696C0,Ground),groundDll(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),Ground);
 EntryPatch clearPatch(0x56A490,Clear),clearDll(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Clear);
 EntryPatch linePatch(0x56BA00,Line),lineDll(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line);
 EntryPatch appearancePatch(0x6D1080,Appearance),appearanceDll(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),Appearance);
 EntryPatch arrayPositionPatch(0x50AF00,ArrayPosition),arrayPositionDll(bit_cast<uintptr_t>(&CCamera::GetArrPosForVehicleType),ArrayPosition);
 EntryPatch headingPatch(0x441DB0,Heading),headingDll(bit_cast<uintptr_t>(&CPlaceable::GetHeading),Heading);
 EntryPatch azimuthPatch(0x53CC70,Azimuth),azimuthDll(bit_cast<uintptr_t>(&CGeneral::GetATanOfXY),Azimuth);
 EntryPatch audioPatch(0x506EA0,Audio),audioDll(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),Audio);
 EntryPatch cyclePatch(0x5404A0,Cycle),cycleDll(bit_cast<uintptr_t>(&CPad::GetCameraCycleUp),Cycle);
 EntryPatch stickXPatch(0x540E80,StickX),stickXDll(bit_cast<uintptr_t>(&CPad::GetMouseCameraStickHorizontal),StickX);
 EntryPatch stickYPatch(0x540F80,StickY),stickYDll(bit_cast<uintptr_t>(&CPad::GetMouseCameraStickVertical),StickY);
 EntryPatch doorsPatch(0x449FF0,Doors);
 // CCamera::CamControl (DLL) llama a CZoneDef::GetCenter de la DLL directamente (CameraControlZoneCameraPosition): el mismo falso en la entrada de la DLL (Body calculado ANTES de crear parches, para no seguir un salto propio).
 const uintptr_t zoneCenterThunk=bit_cast<uintptr_t>(&CZoneDef::GetCenter),zoneCenterBody=Body(zoneCenterThunk);
 EntryPatch zonePositionPatch(0x72D6D0,ZonePosition),zonePositionDll(zoneCenterThunk,ZonePosition);
 std::unique_ptr<EntryPatch> zonePositionBody;if(zoneCenterBody!=zoneCenterThunk)zonePositionBody=std::make_unique<EntryPatch>(zoneCenterBody,ZonePosition);
 EntryPatch bufferPatch(0x509AE0,Buffer);
 EntryPatch cinemaCarPatch(0x5267C0,CinemaCar);
 EntryPatch cinemaTrainPatch(0x526950,CinemaTrain);
 EntryPatch cinemaHeliPatch(0x526AE0,CinemaHeli);
 EntryPatch cinemaPlanePatch(0x526C80,CinemaPlane);
 EntryPatch cinemaBoatPatch(0x526E20,CinemaBoat);

 const auto savedPlayer0=StaticRef<CPed*>(0xB7CD98),savedPlayer1=StaticRef<CPed*>(0xB7CF28);const auto savedActive=StaticRef<uint8>(0xB7CD74);
 const auto savedMS=CTimer::GetTimeInMS();const auto savedStep=CTimer::GetTimeStep();const auto savedPause0=StaticRef<uint8>(0xB7CB48),savedPause1=StaticRef<uint8>(0xB7CB49),savedReplay=StaticRef<uint8>(0xA43088),savedModel=StaticRef<uint8>(0xBA6728),savedVisible=StaticRef<uint8>(0x8CC380),savedDraw=StaticRef<uint8>(0x8D5109);
 const auto savedZoom=StaticRef<int32>(0x8CC824);const auto savedGarage=CGarages::bCamShouldBeOutside;
 std::array<uint8,16> savedControl;std::memcpy(savedControl.data(),reinterpret_cast<void*>(0xB70134),savedControl.size());
 std::array<uint8,8> savedCinematic;std::memcpy(savedCinematic.data(),reinterpret_cast<void*>(0xB6EC2D),savedCinematic.size());
 for(const auto& fn:fns){auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
 const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;const std::string full=std::string(fn.cls)+"::"+fn.name;
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){
 hook->State(candidate);scenario=uint32(i);trace=json::array();
 alignas(CCamera) std::array<uint8,sizeof(CCamera)> cameraBytes{};auto* self=reinterpret_cast<CCamera*>(cameraBytes.data());
 alignas(16) std::array<uint8,4096> objects[7]{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrices[7]{};
 alignas(CPedIntelligence) std::array<uint8,sizeof(CPedIntelligence)> intelligence[3]{};
 alignas(CPlayerPedData) std::array<uint8,sizeof(CPlayerPedData)> playerData[2]{};
 alignas(CPad) std::array<uint8,sizeof(CPad)> padBytes{};
 alignas(CGarage) std::array<uint8,sizeof(CGarage)> garageBytes{};
 alignas(CAttributeZone) std::array<uint8,sizeof(CAttributeZone)> zoneBytes{};
 alignas(CTaskSimpleHoldEntity) std::array<uint8,sizeof(CTaskSimpleHoldEntity)> holdBytes{};
 alignas(CTaskSimpleArrestPed) std::array<uint8,sizeof(CTaskSimpleArrestPed)> arrestBytes{};
 alignas(CTaskSimpleJetPack) std::array<uint8,sizeof(CTaskSimpleJetPack)> jetBytes{};
 player=reinterpret_cast<CPlayerPed*>(objects[0].data());vehicle=reinterpret_cast<CVehicle*>(objects[2].data());pad=reinterpret_cast<CPad*>(padBytes.data());zone=reinterpret_cast<CAttributeZone*>(zoneBytes.data());hold=reinterpret_cast<CTaskSimpleHoldEntity*>(holdBytes.data());arrest=reinterpret_cast<CTaskSimpleArrestPed*>(arrestBytes.data());jet=reinterpret_cast<CTaskSimpleJetPack*>(jetBytes.data());
 for(size_t n=0;n<7;++n){At<uintptr_t>(objects[n].data(),0x14)=uintptr_t(matrices[n].data());At<CVector>(matrices[n].data(),0)=CVector{1,0,0};At<CVector>(matrices[n].data(),0x10)=CVector{0,1,0};At<CVector>(matrices[n].data(),0x20)=CVector{0,0,1};At<CVector>(matrices[n].data(),0x30)=CVector{float(n)+1,float(n)+2,3};objects[n][0x36]=uint8(n==2?2:n<3?3:4);At<uint32>(objects[n].data(),0x1C)=0x80;}
 for(size_t n=0;n<2;++n){At<uintptr_t>(objects[n].data(),0x47C)=uintptr_t(intelligence[n].data());At<uintptr_t>(objects[n].data(),0x480)=uintptr_t(playerData[n].data());At<uint32>(objects[n].data(),0x46C)=uint32(i%3==0?0x100:1);At<uintptr_t>(objects[n].data(),0x58C)=uintptr_t(vehicle);At<float>(playerData[n].data(),0x14)=i%19==0?.25f:0.f;At<uint32>(playerData[n].data(),0x34)=uint32(i%7==0?8:0);}
 At<uintptr_t>(objects[1].data(),0x47C)=uintptr_t(intelligence[1].data());At<uint32>(objects[1].data(),0x530)=uint32(i%2==0?55:54);
 At<uintptr_t>(objects[0].data(),0x71C)=i%4==0?uintptr_t(objects[1].data()):0;
 At<uintptr_t>(objects[0].data(),0xFC)=i%17==0?uintptr_t(vehicle):0;
 constexpr std::array<int32,5> states{0,24,54,55,63};At<int32>(objects[0].data(),0x530)=states[size_t((i/7)%5)];
 At<uintptr_t>(objects[0].data(),0x47C)=uintptr_t(intelligence[0].data());At<uintptr_t>(intelligence[0].data(),0x130)=uintptr_t(objects[1].data());At<uintptr_t>(intelligence[0].data(),0x134)=i%7==0?uintptr_t(objects[0].data()):0;
 hold->m_pEntityToHold=reinterpret_cast<CEntity*>(objects[3].data());arrest->m_Ped=player;
 constexpr std::array<int32,6> types{0,9,5,6,3,4};At<int32>(objects[2].data(),0x590)=types[size_t((i/3)%6)];At<int32>(objects[2].data(),0x594)=types[size_t((i/5)%6)];At<int16>(objects[2].data(),0x22)=int16(i%11==0?460:i%13==0?423:2000);
 for(size_t n=4;n<6;++n)At<uintptr_t>(objects[n].data(),0x170)=uintptr_t(objects[6].data());
 firstDoor=i%3==0?nullptr:reinterpret_cast<CObject*>(objects[4].data());secondDoor=i%3==2?reinterpret_cast<CObject*>(objects[5].data()):nullptr;
 auto* garage=reinterpret_cast<CGarage*>(garageBytes.data());garage->m_fLeftCoord=0;garage->m_fRightCoord=8;garage->m_fFrontCoord=0;garage->m_fBackCoord=10;At<uint8>(garageBytes.data(),0x4C)=uint8(2+i%4);
 zone->zoneDef.m_vec1X=3;zone->zoneDef.m_vec1Y=-4;zone->zoneDef.m_vec2X=-5;zone->zoneDef.m_vec2Y=6;
 garageCase=i%3!=0;stairsCase=i%5==0;
 self->m_nActiveCam=uint8(i%2);const auto targetKind=(i/11)%3;self->m_pTargetEntity=reinterpret_cast<CEntity*>(targetKind==0?objects[0].data():targetKind==1?objects[2].data():objects[3].data());
 // Vehicle stairs needs a garage to avoid the original uninitialized door locals.
 self->m_pToGarageWeAreIn=garageCase||(stairsCase&&targetKind==1)?garage:nullptr;
 constexpr std::array<uint16,23> modes{1,4,7,8,15,16,18,22,29,32,34,35,37,38,45,46,48,49,51,53,55,65,0};
 for(size_t n=0;n<2;++n){auto& cam=self->m_aCams[n];cam.m_nMode=static_cast<eCamMode>(modes[size_t((i/2)%22)]);cam.m_pCamTargetEntity=i%17==0?nullptr:self->m_pTargetEntity;cam.m_vecFront=CVector{.25f,1,.125f};cam.m_vecSource=CVector{8,9,6};cam.m_vecLastAboveWaterCamPosition=CVector{float(n),2,3};cam.m_fHorizontalAngle=.375f;cam.m_fTargetZoomGroundOne=1.25f;cam.m_fTargetZoomGroundTwo=2.5f;cam.m_fTargetZoomGroundThree=i%7==0?0.f:3.75f;cam.m_fCloseInPedHeightOffset=.125f;cam.m_fCloseInCarHeightOffset=.25f;}
 self->m_bLookingAtPlayer=i%4!=0;self->m_bLookingAtVector=i%7==0;self->m_bEnable1rstPersonCamCntrlsScript=i%11==0;self->m_bAllow1rstPersonWeaponsCamera=i%13==0;self->m_bFirstPersonBeingUsed=i%7==0;self->m_bWideScreenOn=i%19==0;self->m_bTargetJustBeenOnTrain=i%17==0;self->m_bCooperativeCamMode=i%23==0;self->m_bAllowShootingWith2PlayersInCar=i%3==0;self->m_bCinemaCamera=i%29==0;
 self->m_nModeForTwoPlayersSeparateCars=static_cast<eCamMode>(62);self->m_nModeForTwoPlayersSameCarShootingAllowed=static_cast<eCamMode>(63);self->m_nModeForTwoPlayersSameCarShootingNotAllowed=static_cast<eCamMode>(18);self->m_nModeForTwoPlayersNotBothInCar=static_cast<eCamMode>(61);
 self->m_nCheckCullZoneThisNumFrames=6;self->m_nZoneCullFrameNumWereAt=uint32(i%8);self->m_bFailedCullZoneTestPreviously=i%3==0;self->m_nWhoIsInControlOfTheCamera=uint32(i%3);self->m_nCarZoom=uint32((i/7)%6);self->m_nPedZoom=uint32(1+(i/9)%5);self->m_bDisableFirstPersonInCar=i%2==0;self->m_bUseScriptZoomValueCar=i%13==0;self->m_bUseScriptZoomValuePed=i%17==0;
 self->m_fCarZoomBase=1;self->m_fCarZoomTotal=i%3==0?0.f:2.f;self->m_fCarZoomSmoothed=float(i%6)*.5f;self->m_fCarZoomValueScript=1.5f;self->m_fPedZoomSmoothed=float(i%7)*.25f;self->m_fPedZoomValueScript=2.25f;
 self->m_PlayerMode.m_nMode=i%13==0?modes[size_t((i/17)%23)]:0;self->m_PlayerWeaponMode.m_nMode=i%3==0?0:modes[size_t((i/5)%23)];self->m_vecAimingTargetCoors=CVector{1.5f,2.5f,3};self->m_vecFixedModeVector=CVector{4,5,6};self->m_vecFixedModeSource=CVector{10,11,12};self->m_vecFixedModeUpOffSet=CVector{.125f,.25f,.375f};self->m_mCameraMatrix.GetPosition()=CVector{1,2,3};
 self->m_bRestoreByJumpCut=i%31==0;self->m_nModeToGoTo=static_cast<eCamMode>(modes[size_t((i/3)%22)]);self->m_bTransitionState=i%2==0;self->m_bDoingSpecialInterp=true;self->m_bWaitForInterpolToFinish=i%7==0;self->m_bStartInterScript=i%11==0;self->m_nTypeOfSwitch=static_cast<eSwitchType>(i%3);self->m_bGarageFixedCamPositionSet=i%13==0;self->m_bPlayerIsInGarage=i%17==0;self->m_bIdleOn=i%37==0;self->m_nFirstPersonCamLastInputTime=uint32(i%3==0?0:900);
 StaticRef<CPed*>(0xB7CD98)=player;StaticRef<CPed*>(0xB7CF28)=reinterpret_cast<CPed*>(objects[1].data());StaticRef<uint8>(0xB7CD74)=0;StaticRef<uint32>(0xB7CB84)=1000;StaticRef<float>(0xB7CB5C)=i%5==0?.5f:1.f;StaticRef<uint8>(0xB7CB48)=uint8(i%41==0);StaticRef<uint8>(0xB7CB49)=uint8(i%43==0);StaticRef<uint8>(0xA43088)=uint8(i%47==0);StaticRef<uint8>(0xBA6728)=uint8(i%53==0);StaticRef<uint8>(0x8CC380)=uint8(i%2);
 StaticRef<int32>(0x8CC824)=i%59==0?16:-1;CGarages::bCamShouldBeOutside=i%3!=0;std::memset(reinterpret_cast<void*>(0xB70134),0,16);StaticRef<uint16>(0xB70140)=modes[size_t((i/19)%22)];StaticRef<uint32>(0xB70134)=32;StaticRef<int32>(0xB70138)=i%3==0?63:0;StaticRef<uint8>(0xB7013C)=uint8(i%5==0);StaticRef<uint8>(0xB7013D)=uint8(i%7==0);std::memset(reinterpret_cast<void*>(0xB6EC2D),0,8);StaticRef<uint8>(0xB6EC2E)=uint8(i%11==0);StaticRef<uint8>(0x8D5109)=1;
 const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCamera*)>(fn.address)(self);
 return json{{"camera",Digest(cameraBytes.data(),cameraBytes.size())},{"objects",Digest(objects,sizeof(objects))},{"matrices",Digest(matrices,sizeof(matrices))},{"pad",Digest(padBytes.data(),padBytes.size())},{"intelligence",Digest(intelligence,sizeof(intelligence))},{"player_data",Digest(playerData,sizeof(playerData))},{"control",Digest(reinterpret_cast<void*>(0xB70134),16)},{"cinematic",Digest(reinterpret_cast<void*>(0xB6EC2D),8)},{"draw",StaticRef<uint8>(0x8D5109)},{"zoom_override",StaticRef<int32>(0x8CC824)},{"mode",uint16(self->m_aCams[self->m_nActiveCam].m_nMode)},{"fixed",V(self->m_vecFixedModeSource)},{"trace",trace},{"switches",switched}};
 };
 const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<CPed*>(0xB7CD98)=savedPlayer0;StaticRef<CPed*>(0xB7CF28)=savedPlayer1;StaticRef<uint8>(0xB7CD74)=savedActive;StaticRef<uint32>(0xB7CB84)=savedMS;StaticRef<float>(0xB7CB5C)=savedStep;StaticRef<uint8>(0xB7CB48)=savedPause0;StaticRef<uint8>(0xB7CB49)=savedPause1;StaticRef<uint8>(0xA43088)=savedReplay;StaticRef<uint8>(0xBA6728)=savedModel;StaticRef<uint8>(0x8CC380)=savedVisible;StaticRef<uint8>(0x8D5109)=savedDraw;StaticRef<int32>(0x8CC824)=savedZoom;CGarages::bCamShouldBeOutside=savedGarage;std::memcpy(reinterpret_cast<void*>(0xB70134),savedControl.data(),savedControl.size());std::memcpy(reinterpret_cast<void*>(0xB6EC2D),savedCinematic.data(),savedCinematic.size());
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
