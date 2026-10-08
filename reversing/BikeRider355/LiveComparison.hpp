#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendHierarchy.h"
#include "Animation/AnimBlendFrameData.h"
#include "Events/EventKnockOffBike.h"
#include "Events/EventGroup.h"
#include "General.h"
#include "../ProbeRuntime.hpp"
namespace BikeRider355Live {
template<class T> T& At(void* p,size_t n){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+n);}
inline json Bytes(const void* p,size_t n){json result=json::array();auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)result.push_back(b[i]);return result;}
inline uint32 F(float x){return std::bit_cast<uint32>(x);}
inline uint32 scenario,randomCalls;inline json trace;inline std::array<CAnimBlendAssociation*,10> animations;inline CEventGroup* driverGroup;inline CEventGroup* passengerGroup;
inline bool __fastcall Player(const CPed*,void*){trace.push_back({"player"});return (scenario&1)!=0;}
inline float __fastcall Skill(const CPed*,void*){trace.push_back({"skill"});return float(scenario%5)*.25f;}
inline CAnimBlendAssociation* __cdecl Association(RpClump*,uint32 id){trace.push_back({"association",int(id)});const int n=int(id)-195;if(n<0 || n>=10)return nullptr;return (scenario%7==0 || (scenario>>(n%9))&1)?animations[n]:nullptr;}
inline CAnimBlendAssociation* __cdecl Add(RpClump*,AssocGroupId group,AnimationId id){trace.push_back({"add",int(group),int(id)});auto* result=animations[int(id)-195];result->m_BlendAmount=0.f;result->m_BlendDelta=0.f;return result;}
inline CAnimBlendAssociation* __cdecl Blend(RpClump*,AssocGroupId group,AnimationId id,float delta){trace.push_back({"blend",int(group),int(id),F(delta)});auto* result=animations[int(id)-195];result->m_BlendAmount=.125f;result->m_BlendDelta=delta;return result;}
inline void __fastcall Time(CAnimBlendAssociation* animation,void*,float time){trace.push_back({"time",int(animation->m_AnimId),F(time)});animation->m_CurrentTime=time;}
inline void __fastcall Reference(CEntity*,void*,CEntity**){trace.push_back({"reference"});}
inline void __fastcall Cleanup(CEntity*,void*,CEntity**){trace.push_back({"cleanup"});}
inline CEvent* __fastcall Event(CEventGroup* group,void*,CEvent* event,bool valid){auto* knock=static_cast<CEventKnockOffBike*>(event);trace.push_back({"event",group==driverGroup?0:group==passengerGroup?1:2,valid,Bytes(&knock->m_moveSpeed,sizeof(CVector)),Bytes(&knock->m_collisionImpactVelocity,sizeof(CVector)),F(knock->m_damageIntensity),F(knock->field_28),knock->m_time,knock->m_ped==nullptr,int(knock->m_knockOffDirection),bool(knock->m_isVictimDriver),bool(knock->m_forceKnockOff),int(knock->m_knockOffType),int(knock->m_exitDoor)});return nullptr;}
inline int __cdecl Random(){const int result=int((scenario*131+randomCalls++*719)%32768);trace.push_back({"random",result});return result;}
inline RwMatrix* __cdecl Rotate(RwMatrix* matrix,const RwV3d* axis,RwReal angle,RwOpCombineType combine){trace.push_back({"rotate",Bytes(axis,sizeof(*axis)),F(angle),int(combine)});matrix->pos.x+=angle*.001f;return matrix;}
inline bool done;inline std::ofstream log;
inline void Write(const json& value){ReversingProbe::PumpMessages();log<<value.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CBike","ProcessRiderAnims",0x6B7280,0,-1,1024}}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CBike")for(auto& h:c.Items())if(h->Name()=="ProcessRiderAnims")result=h.get();});return result;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto directory=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(directory/"bike-rider355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(directory/"bike-rider355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-rider355");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p0(0x5DF8F0,Player),p1(bit_cast<uintptr_t>(&CPed::IsPlayer),Player),sk0(0x5DF510,Skill),sk1(bit_cast<uintptr_t>(&CPed::GetBikeRidingSkill),Skill);
 EntryPatch as0(0x4D68B0,Association),as1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),Association);
 EntryPatch ad0(0x4D3AA0,Add),ad1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation)),Add),bl0(0x4D4610,Blend),bl1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend);
 EntryPatch t0(0x4CEA80,Time),t1(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime),Time),r0(0x571B70,Reference),r1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Reference),cl0(0x571A00,Cleanup),cl1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Cleanup);
 EntryPatch ev0(0x4AB420,Event),ev1(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),Event),rnd0(0x821B1E,Random),rnd1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random),rot0(0x7EB7C0,Rotate),rot1(bit_cast<uintptr_t>(&RwMatrixRotate),Rotate);
 struct Region{uintptr_t address;size_t count;std::vector<uint8> saved;};std::array<Region,2> globals{{{0xB7CB58,8},{0xB7CB84,4}}};for(auto& r:globals){r.saved.resize(r.count);std::memcpy(r.saved.data(),reinterpret_cast<void*>(r.address),r.count);}
 auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;
 for(int test=0;test<1024;++test){auto run=[&](bool candidate){
 hook->State(candidate);scenario=test;randomCalls=0;trace=json::array();for(auto& r:globals)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.count);
 alignas(CAutomobile) std::array<uint8,sizeof(CAutomobile)> vehicleBytes{};auto* vehicle=reinterpret_cast<CVehicle*>(vehicleBytes.data());At<int32>(vehicle,0x590)=(test/16)%3==0?9:(test/16)%3==1?0:5;At<int32>(vehicle,0x594)=test%5==0?10:test%5==1?2:9;At<uint8>(vehicle,0x804)=At<uint8>(vehicle,0x960)=uint8(test%4);At<uint8>(vehicle,0x614)=0xAF;At<uint32>(vehicle,0x3C)=1000;
 CMatrix matrix{};std::memset(&matrix,0,sizeof(matrix));matrix.GetRight()=CVector{1,0,0};matrix.GetForward()=CVector{0,1,float(int32((test/8)%5)-2)*.25f};matrix.GetUp()=CVector{0,0,float(int32(test%5)-2)*.375f};At<CMatrix*>(vehicle,0x14)=&matrix;
 vehicle->m_vecMoveSpeed=CVector{0,float(int32(test%13)-6)*.125f,0};vehicle->m_vecTurnSpeed=CVector{float(test%5)*.125f,float((test/5)%5)*.125f,0};At<float>(vehicle,0x49C)=float(int32(test%7)-3)*.25f;At<float>(vehicle,0x4A0)=float(test%5)*.25f;
 alignas(float) std::array<uint8,256> handlingBytes{};At<tHandlingData*>(vehicle,0x384)=reinterpret_cast<tHandlingData*>(handlingBytes.data());At<float>(handlingBytes.data(),0x88)=.75f;At<float>(handlingBytes.data(),0x8C)=-.125f;
 for(size_t n=0;n<4;++n){At<float>(vehicle,0x730+n*4)=float(int32((test>>n)%3)-1)*.25f;At<float>(vehicle,0x7F4+n*4)=float(int32((test>>(n+2))%3)-1)*.25f;}
 alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,2> pedBytes{};auto* rider=reinterpret_cast<CPed*>(pedBytes[0].data());auto* passenger=reinterpret_cast<CPed*>(pedBytes[1].data());
 alignas(CPedIntelligence) std::array<std::array<uint8,sizeof(CPedIntelligence)>,2> intelBytes{};for(size_t n=0;n<2;++n)At<CPedIntelligence*>(pedBytes[n].data(),0x47C)=reinterpret_cast<CPedIntelligence*>(intelBytes[n].data());driverGroup=&rider->GetEventGroup();passengerGroup=&passenger->GetEventGroup();vehicle->m_apPassengers[0]=(test&16)?passenger:(test&32)?rider:nullptr;
 RpClump clump{};clump.object.type=2;At<RpClump*>(rider,0x18)=&clump;AnimBlendFrameData frame{};RwMatrix bone{};bone.right={1,0,0};bone.up={0,1,0};bone.at={0,0,1};frame.KeyFrame=reinterpret_cast<RpHAnimBlendInterpFrame*>(&bone);At<AnimBlendFrameData*>(rider,0x490)=&frame;
 CRideAnimData ride{};ride.AnimGroup=ANIM_GROUP_BIKES;ride.LeanAngle=float(int32((test/4)%9)-4)*.125f;ride.LeanFwd=float(int32((test/9)%9)-4)*.125f;ride.AnimLeanLeft=float(int32(test%9)-4)*.25f;ride.AnimLeanFwd=float(int32((test/9)%9)-4)*.25f;
 tBikeHandlingData handling{};handling.m_fFullAnimLean=.5f;handling.m_fWheelieAng=.5f;handling.m_fStoppieAng=-.5f;
 alignas(CAnimBlendAssociation) std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,10> animationBytes{};std::array<CAnimBlendHierarchy,10> hierarchies{};std::memset(hierarchies.data(),0,sizeof(hierarchies));
 for(size_t n=0;n<10;++n){animations[n]=reinterpret_cast<CAnimBlendAssociation*>(animationBytes[n].data());animations[n]->m_AnimId=static_cast<AnimationId>(195+n);animations[n]->m_BlendHier=&hierarchies[n];animations[n]->m_BlendAmount=float((test+n)%5)*.25f;animations[n]->m_BlendDelta=float(int32((test+n)%5)-2)*.5f;animations[n]->m_Flags=0xF0F1;hierarchies[n].m_fTotalTime=1.f+float(n)*.25f;}
 StaticRef<float>(0xB7CB58)=float(10+test%6)*.5f;StaticRef<float>(0xB7CB5C)=float(1+test%5)*.25f;StaticRef<uint32>(0xB7CB84)=1000+test%200;
 const bool switched=ReversingProbe::JumpsToDll(0x6B7280)==candidate;reinterpret_cast<void(__cdecl*)(CPed*,CVehicle*,CRideAnimData*,tBikeHandlingData*,int16)>(0x6B7280)(rider,vehicle,&ride,&handling,int16(test%3));
 json globalState=json::array();for(auto& r:globals)globalState.push_back(Bytes(reinterpret_cast<void*>(r.address),r.count));return json{{"vehicle",Bytes(vehicleBytes.data(),vehicleBytes.size())},{"peds",Bytes(pedBytes.data(),sizeof(pedBytes))},{"intelligence",Bytes(intelBytes.data(),sizeof(intelBytes))},{"ride",Bytes(&ride,sizeof(ride))},{"handling",Bytes(&handling,sizeof(handling))},{"vehicleHandling",Bytes(handlingBytes.data(),handlingBytes.size())},{"animations",Bytes(animationBytes.data(),sizeof(animationBytes))},{"hierarchies",Bytes(hierarchies.data(),sizeof(hierarchies))},{"matrix",Bytes(&matrix,sizeof(matrix))},{"bone",Bytes(&bone,sizeof(bone))},{"frame",Bytes(&frame,sizeof(frame))},{"globals",globalState},{"trace",trace},{"switches",switched}};
 };auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original["switches"]==true;failures+=!equal;Write({{"event","comparison"},{"pair",test},{"function","CBike::ProcessRiderAnims"},{"case",test},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});}
 hook->State(enabled);hook->LockState(locked);for(auto& r:globals)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.count);Write({{"event","function_summary"},{"function","CBike::ProcessRiderAnims"},{"cases",1024},{"failures",failures}});Write({{"event",failures?"error":"complete"},{"pairs",1024},{"state_restored",true}});
}
}

