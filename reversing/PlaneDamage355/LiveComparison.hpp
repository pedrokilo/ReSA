#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Plane.h"
#include "SurfaceInfos_c.h"
#include "AEVehicleAudioEntity.h"
#include "CarCtrl.h"
#include "World.h"
#include "Camera.h"
#include "Explosion.h"
#include "Darkel.h"
#include "VisibilityPlugins.h"
#include "Stats.h"
#include "FireManager.h"
#include "DamageManager.h"
#include "FxSystem.h"
#include "FxManager.h"
#include "BouncingPanel.h"
#include "../ProbeRuntime.hpp"
namespace PlaneDamage355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}

inline CPlane* current{};inline int scenario{},statusCalls{};inline CEntity* offender{};inline CPed* player{};inline std::array<RwFrame,25> frames{};inline std::array<std::array<uint8,sizeof(FxSystem_c)>,2> effects{};
inline json V(const CVector& v){return {std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)};}
inline CPlayerPed* __cdecl Player(int32 n){trace.push_back({"player",n});if(scenario%11==0)current->m_pDamageEntity=current;return reinterpret_cast<CPlayerPed*>(player);}
inline CVehicle* __cdecl PlayerVehicle(int32 n,bool remote){trace.push_back({"vehicle",n,remote});if(scenario%13==0)current->m_fHealth+=.125f;return scenario%3==0?current:nullptr;}
inline int32 __cdecl Random(){int32 n=(scenario*7919+int(trace.size()))&32767;trace.push_back({"random",n});return n;}
inline bool __fastcall CanDamage(void*,void*,CEntity* d,eWeaponType w,bool& fire){trace.push_back({"can_damage",d==offender,int(w)});fire=true;return scenario%5!=0;}
inline void __fastcall BaseDamage(void*,void*,float intensity,eVehicleCollisionComponent c,CEntity* d,CVector* pos,CVector* dir,eWeaponType w){trace.push_back({"base",std::bit_cast<uint32>(intensity),int(c),d==offender,V(*pos),V(*dir),int(w)});current->m_fHealth-=.25f;}
inline void __fastcall Blow(void*,void*,CEntity* d,bool hide){trace.push_back({"blow",d==current,hide});current->m_fHealth=-1.f;}
inline bool __fastcall Progress(void*,void*,uint8 node){trace.push_back({"progress",node});reinterpret_cast<uint8*>(&current->m_damageManager)[0]^=node;return scenario%4!=0;}
inline uint32 __fastcall Status(void*,void*,uint8 node){const uint32 result=(scenario+statusCalls++)%3;trace.push_back({"status",node,result});return result;}
inline void __fastcall Panel(CBouncingPanel* p,void*,int16 node,int16 axis,float angle){trace.push_back({"panel",(reinterpret_cast<uint8*>(p)-reinterpret_cast<uint8*>(current)-0x6AC)/0x20,node,axis,std::bit_cast<uint32>(angle)});p->m_nFrameId=uint16(node);p->m_nAxis=uint16(axis);p->m_fAngleLimit=angle;}
inline void __fastcall Visible(void*,void*,RwFrame* frame,uint32 flags){trace.push_back({"visible",frame-frames.data(),flags});frame->modelling.pos.x+=.25f;}
inline void __fastcall Particles(void*,void*,const CVector& position,float force,eWeaponType weapon){trace.push_back({"particles",V(position),std::bit_cast<uint32>(force),int(weapon)});for(auto& f:frames)f.modelling.pos.z+=.25f;}
inline void __fastcall Kill(FxSystem_c* fx,void*){trace.push_back({"kill",fx==reinterpret_cast<FxSystem_c*>(effects[0].data())});current->m_pSmokeParticle=reinterpret_cast<FxSystem_c*>(effects[1].data());effects[0][3]^=1;}
inline RwMatrix* __fastcall Matrix(void*,void*){trace.push_back({"matrix"});return scenario%7==0?nullptr:&frames[0].modelling;}
inline FxSystem_c* __fastcall Create(void*,void*,const char* name,const CVector& position,RwMatrix* matrix,bool ignore){trace.push_back({"create",std::string(name),V(position),matrix==&frames[0].modelling,ignore});return scenario%9==0?nullptr:reinterpret_cast<FxSystem_c*>(effects[0].data());}
inline void __fastcall Play(FxSystem_c* fx,void*){trace.push_back({"play",fx==reinterpret_cast<FxSystem_c*>(effects[0].data())});current->m_vecMoveSpeed.x+=.25f;if(scenario%2)current->m_pSmokeParticle=reinterpret_cast<FxSystem_c*>(effects[1].data());}
inline void __fastcall Velocity(FxSystem_c* fx,void*,const CVector& velocity){trace.push_back({"velocity",fx==reinterpret_cast<FxSystem_c*>(effects[1].data()),V(velocity)});}
inline void __fastcall Local(FxSystem_c* fx,void*,bool local){trace.push_back({"local",fx==reinterpret_cast<FxSystem_c*>(effects[1].data()),local});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CPlane","VehicleDamage",0,0x871948,56,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"plane-damage355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"plane-damage355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plane-damage355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 auto* hook=FindHook("CPlane","VehicleDamage");auto* base=FindHook("CAutomobile","VehicleDamage");auto* blow=FindHook("CPlane","BlowUpCar");if(!hook||!base||!blow){Write({{"event","error"},{"reason","missing hook"}});return;}
 const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);uint32 failures=0,positiveCases=0,pairs=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch pp(0x56E210,Player),ppd(bit_cast<uintptr_t>(&FindPlayerPed),Player),pv(0x56E0D0,PlayerVehicle),pvd(bit_cast<uintptr_t>(&FindPlayerVehicle),PlayerVehicle);
 EntryPatch rn(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch cd(0x6D1280,CanDamage),cdd(bit_cast<uintptr_t>(&CVehicle::CanVehicleBeDamaged),CanDamage);
 EntryPatch bd(0x6A7650,BaseDamage),bdd(reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(base)->GetHookOurAddress()),BaseDamage);
 EntryPatch bl(0x6CCCF0,Blow),bld(reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(blow)->GetHookOurAddress()),Blow);
 EntryPatch pr(0x6C2460,Progress),prd(bit_cast<uintptr_t>(&CDamageManager::ProgressAeroplaneDamage),Progress),st(0x6C2300,Status),stdp(bit_cast<uintptr_t>(&CDamageManager::GetAeroplaneCompStatus),Status);
 EntryPatch pa(0x6F4920,Panel),pad(bit_cast<uintptr_t>(&CBouncingPanel::SetPanel),Panel),vi(0x6D2700,Visible),vid(bit_cast<uintptr_t>(&CVehicle::SetComponentVisibility),Visible);
 EntryPatch pt(0x6A6DC0,Particles),ptd(bit_cast<uintptr_t>(&CAutomobile::dmgDrawCarCollidingParticles),Particles);
 EntryPatch ki(0x4AA3F0,Kill),kid(bit_cast<uintptr_t>(&FxSystem_c::Kill),Kill),ma(0x46A2D0,Matrix),mad(bit_cast<uintptr_t>(&CEntity::GetModellingMatrix),Matrix);
 EntryPatch cr(0x4A9BE0,Create),crd(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),Create);
 EntryPatch pl(0x4AA2F0,Play),pld(bit_cast<uintptr_t>(&FxSystem_c::Play),Play),ve(0x4AA730,Velocity),ved(bit_cast<uintptr_t>(&FxSystem_c::SetVelAdd),Velocity),lo(0x4AA910,Local),lod(bit_cast<uintptr_t>(&FxSystem_c::SetLocalParticles),Local);
 for(int i=0;i<1024;++i){alignas(16) std::array<uint8,sizeof(CPlane)> object{};alignas(16) std::array<uint8,0x80> matrix{},handling{};alignas(16) std::array<uint8,sizeof(CPed)> ped{};alignas(16) std::array<uint8,sizeof(CEntity)> entity{};CVector position{0,0,0},direction{1,0,0};
 auto run=[&](bool candidate){scenario=i;statusCalls=0;trace=json::array();memset(object.data(),0,object.size());memset(matrix.data(),0,matrix.size());memset(handling.data(),0,handling.size());memset(entity.data(),0,entity.size());memset(ped.data(),0,ped.size());memset(frames.data(),0,sizeof(frames));memset(effects.data(),0,sizeof(effects));current=reinterpret_cast<CPlane*>(object.data());offender=reinterpret_cast<CEntity*>(entity.data());player=reinterpret_cast<CPed*>(ped.data());position=CVector{0,0,0};direction=CVector{1,0,0};*reinterpret_cast<uintptr_t*>(current)=0x871948;current->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());current->m_matrix->GetRight()=CVector{1,0,0};current->m_matrix->GetForward()=CVector{0,1,0};current->m_matrix->GetUp()=CVector{0,0,1};current->GetPosition()=CVector{0,0,0};current->m_vecMoveSpeed=CVector{.25f,-.5f,.125f};current->m_nModelIndex=i%23==0?539:476;current->SetStatus(static_cast<eEntityStatus>((i/5)%6));object[0x42A]=i%19==0?0:0x20;object[0x429]=i%2?0x80:0;current->m_nPhysicalFlags=i%31==0?0x100000:i%17==0?0x400000:0;current->m_pDamageEntity=i%3==0?nullptr:offender;entity[0x36]=uint8(i%4);current->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());current->m_pHandlingData->m_fCollisionDamageMultiplier=.5f;current->m_fMass=i%7==0?800.f:i%11==0?10000.f:2000.f;current->m_fDamageIntensity=i%13==0?0.f:i%9==0?700.f:2.f+float(i%5);current->m_fHealth=i%8==0?.5f:100.f;current->m_vecLastCollisionPosn=CVector{0,0,0};std::ranges::fill(current->m_aCarNodes,nullptr);for(int j=12;j<25;++j){frames[j].modelling.pos=i%4==0?RwV3d{float(j-16)*.231789f,float((j*7+i)%11-5)*.173919f,float((j*13+i)%9-4)*.387127f}:RwV3d{float(j-16)*.25f,0,0};if(i%5!=0||j==16)current->m_aCarNodes[j]=&frames[j];}if(i%29==0)std::ranges::fill(current->m_aCarNodes,nullptr);auto* panels=reinterpret_cast<CBouncingPanel*>(object.data()+0x6AC);for(int j=0;j<4;++j)panels[j].m_nFrameId=uint16((i+j)%5==0?-1:20+j);current->m_pSmokeParticle=i%2?reinterpret_cast<FxSystem_c*>(effects[0].data()):nullptr;current->m_nSmokeTimer=77;
 // 64 targeted nearest-node cases: identical off-axis vectors have an upward-rounded length.
 // EXE keeps squared distance extended; rounding both squares to float would wrongly retain node12.
 if(i%16==4){current->m_nModelIndex=476;current->SetStatus(STATUS_PLAYER);object[0x42A]=0x20;current->m_nPhysicalFlags=0;current->m_fMass=2000.f;current->m_fDamageIntensity=600.f;current->m_fHealth=1000.f;for(int j=12;j<25;++j){frames[j].modelling.pos={0.0031638420186936855f,0.4666784703731537f,0.702342689037323f};current->m_aCarNodes[j]=&frames[j];}}
 // Accepted explosive cases always have previous health .5 and reach BlowUpCar before smoke.
 const bool explosive=i%8==0;const float input=explosive?3.f:0.f;const auto weapon=static_cast<eWeaponType>(explosive?((i/8)%4==0?19:(i/8)%4==1?20:(i/8)%4==2?21:51):0);
 hook->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(0x871948)[56];if((address!=0x6CC4B0)!=candidate)return json{{"switch",false}};
 reinterpret_cast<void(__thiscall*)(CPlane*,float,eVehicleCollisionComponent,CEntity*,CVector*,CVector*,eWeaponType)>(address)(current,input,static_cast<eVehicleCollisionComponent>(0),offender,&position,&direction,weapon);
 const int smoke=current->m_pSmokeParticle==reinterpret_cast<FxSystem_c*>(effects[0].data())?1:current->m_pSmokeParticle==reinterpret_cast<FxSystem_c*>(effects[1].data())?2:0;current->m_pSmokeParticle=nullptr;current->m_matrix=nullptr;current->m_pHandlingData=nullptr;current->m_pDamageEntity=nullptr;std::ranges::fill(current->m_aCarNodes,nullptr);if(!candidate&&!trace.empty())++positiveCases;
 json objectWords=json::array();for(size_t offset=4;offset+4<=object.size();offset+=4)objectWords.push_back(*reinterpret_cast<const uint32*>(object.data()+offset));
 return json{{"switch",true},{"object_words",objectWords},{"health",std::bit_cast<uint32>(current->m_fHealth)},{"mass",std::bit_cast<uint32>(current->m_fMass)},{"intensity",std::bit_cast<uint32>(current->m_fDamageIntensity)},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"frames",Hash(frames.data(),sizeof(frames))},{"effects",Hash(effects.data(),sizeof(effects))},{"smoke",smoke},{"position",V(position)},{"direction",V(direction)},{"trace",trace}};
 };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function","CPlane::VehicleDamage"},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
 }
 }
 if(!positiveCases)++failures;hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function","CPlane::VehicleDamage"},{"cases",1024},{"failures",failures},{"positive_cases",positiveCases}});Write({{"event",failures?"error":"complete"},{"pairs",pairs},{"failures",failures},{"state_restored",hook->Hooked()==enabled&&hook->Locked()==locked}});log.close();
}
}
