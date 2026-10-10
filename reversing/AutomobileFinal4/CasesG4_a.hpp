#pragma once
// G4 (parte a): dobles trazados, estado de guion de los dobles, buferes propios, tablas de superficies y comprobaciones de disposicion.
// Funciones del grupo: ProcessSuspension, DoBurstAndSoftGroundRatios, DoHoverSuspensionRatios, DoSoftGroundResistance,
// ProcessCarWheelPair, ProcessBuoyancy, ProcessEntityCollision, ProcessControlCollisionCheck, VehicleDamage.
// Todos los nombres de este grupo llevan el prefijo G4_ / g4 para no chocar con los de los demas grupos (mismo espacio de nombres).
#include "Ctx.hpp"
#include "Cheat.h"
#include "Weather.h"
#include "Replay.h"
#include "Stats.h"
#include "Pad.h"
#include "SurfaceInfos_c.h"
#include "DamageManager.h"
#include "PedIntelligence.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Physical.h"
#include "Entity/Entity.h"
#include "Collision/ColModel.h"
#include "Collision/CollisionData.h"
#include "Collision/ColPoint.h"
#include "Collision/ColLine.h"
#include "Models/VehicleModelInfo.h"

namespace AutomobileFinal4Live {

// ---------------------------------------------------------------------------------------------------------------------
// Mascaras propias (propietario 4) de dobles
// ---------------------------------------------------------------------------------------------------------------------
enum : uint64 {
    G4_B_FORCE =1ull<<0,   // CPhysical::ApplyMoveForce(vec) / ApplyTurnForce
    G4_B_SPRING=1ull<<1,   // CPhysical::ApplySpringCollision / ApplySpringCollisionAlt / ApplySpringDampening
    G4_B_WATER =1ull<<2,   // CWaterLevel::GetWaterLevel
    G4_B_WHEEL =1ull<<3,   // CVehicle::ProcessWheel
    G4_B_BUOY  =1ull<<4,   // cBuoyancy::ProcessBuoyancy
    G4_B_COLL  =1ull<<5,   // CCollision::ProcessColModels / CPhysical::AddCollisionRecord
    G4_B_CHECK =1ull<<6,   // CPhysical::CheckCollision / SkipPhysics / ApplyMoveSpeed / ApplyTurnSpeed
    G4_B_REF   =1ull<<7,   // CEntity::RegisterReference
    G4_B_DMG   =1ull<<8,   // colaboradores escalares de VehicleDamage
    G4_B_PED   =1ull<<9    // peatones y eventos (ProcessBuoyancy y VehicleDamage); opcionales (simbolos exportados)
};

// ---------------------------------------------------------------------------------------------------------------------
// Direcciones del EXE
// ---------------------------------------------------------------------------------------------------------------------
constexpr uintptr_t G4_X_TIMESTEP=0xB7CB5C, G4_X_TIMEMS=0xB7CB84, G4_X_WETROADS=0xC81308, G4_X_REPLAY=0xA43088, G4_X_CHEATS=0x969130,
                    G4_X_AUTOCOLPTS=0xC1BFF8, G4_X_PLAYERS_VTC=0xB7CE68, G4_X_CURRAREA=0xB72914, G4_X_FOCUS=0xB7CD74;

// ---------------------------------------------------------------------------------------------------------------------
// Estado de guion de los dobles (se reinicia al empezar cada ejecucion de un lado)
// ---------------------------------------------------------------------------------------------------------------------
struct G4_Script {
    // GetWaterLevel
    uint8 wRet[4]{}; float wLevel[4]{}; int wN{};
    // muelles
    uint32 sprSeed{1};
    // ProcessWheel
    int32 wheelOut[4]{};
    // cBuoyancy::ProcessBuoyancy
    uint8 buoyRet{}; float buoyForce[3]{}, buoyTurn[3]{};
    // ProcessColModels
    int32 pcN{}; uint32 pcSeed{1}; float pcDist[12]{}; uint8 pcForklift{};
    // CheckCollision
    uint32 checkBits{}; int checkN{};
    // VehicleDamage y colaboradores
    uint8 canDamage{1}, knockRet{}, canSee{1}, affects{1}; float progress{};
    int16 vehAudioType{}; void* pickPed{}; uint32 lightSmash{}; uint8 applyRet{1};
};
inline G4_Script g4S;

// Registro de objetos de pila (eventos / calculadoras) para identificarlos en las trazas
inline std::array<void*,12> g4Objs{}; inline int g4nObj{};
inline int G4_ObjNew(void* p){ const int id= g4nObj<12 ? g4nObj++ : 11; g4Objs[size_t(id)]=p; return id; }
inline int G4_ObjId(void* p){ for(int i=g4nObj-1;i>=0;--i)if(g4Objs[size_t(i)]==p)return i; return -1; }

struct G4_Rng { uint32 s; uint32 n(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; } float u(float lo,float hi){ return lo+(hi-lo)*float(n()&0xFFFFFF)/float(0xFFFFFF); } };
inline uint32 G4_H12(const void* p){ return p ? Fnv32(p,12) : 0u; }
inline float& G4_F(void* b,size_t o){ return At<float>(b,o); }

// Estado de los "resultados" de varios dobles que el banco inspecciona: bufer de salida de ProcessColModels
alignas(16) inline std::array<uint8,32*0x2C+64> g4OutBuf{};
alignas(16) inline std::array<uint8,0x200> g4Dummy{};

// ---------------------------------------------------------------------------------------------------------------------
// Dobles (convencion y 'ret N' del original; los hilos ajenos no se trazan)
// ---------------------------------------------------------------------------------------------------------------------
// CPhysical::ApplyMoveForce(CVector): thiscall, ret 0Ch (0x5429F0). Efecto simplificado sobre la velocidad del propio coche falso
inline void __fastcall G4_MoveForce(void* self,void*,float x,float y,float z){
    if(!Mine())return; Push(TG("ApplyMoveForce"),self,x,y,z);
    if(Known(self)){ G4_F(self,0x44)+=x*0.002f; G4_F(self,0x48)+=y*0.002f; G4_F(self,0x4C)+=z*0.002f; } }
// CPhysical::ApplyTurnForce(CVector force, CVector point): thiscall, ret 18h (0x542A50)
inline void __fastcall G4_TurnForce(void* self,void*,float fx,float fy,float fz,float px,float py,float pz){
    if(!Mine())return; const float p[3]{px,py,pz}; Push(TG("ApplyTurnForce"),self,fx,fy,fz,Fnv32(p,12));
    if(Known(self)){ G4_F(self,0x50)+=(py*fz-pz*fy)*0.001f; G4_F(self,0x54)+=(pz*fx-px*fz)*0.001f; G4_F(self,0x58)+=(px*fy-py*fx)*0.001f; } }
// CPhysical::ApplySpringCollision(float, CVector&, CVector&, float, float, float&): thiscall, ret 18h, bool en AL (0x543C90)
inline uint8 __fastcall G4_Spring(void* self,void*,float force,const float* dir,const float* pos,float len,float bias,float* limit){
    if(!Mine())return 0; Push(TG("ApplySpringCollision"),self,force,G4_H12(dir),G4_H12(pos),len,bias);
    *limit=force*0.5f+len*0.25f+bias; if(Known(self))G4_F(self,0x4C)+=(*limit)*0.001f;
    g4S.sprSeed=g4S.sprSeed*1664525u+1013904223u; return uint8((g4S.sprSeed>>29)&1u); }
// CPhysical::ApplySpringCollisionAlt(float, CVector&, CVector&, float, float, CVector&, float&): thiscall, ret 1Ch (0x543D60)
inline uint8 __fastcall G4_SpringAlt(void* self,void*,float force,const float* dir,const float* pos,float len,float bias,const float* normal,float* limit){
    if(!Mine())return 0; Push(TG("ApplySpringCollisionAlt"),self,force,G4_H12(dir),G4_H12(pos),len,bias); Push(TG("ApplySpringCollisionAlt_n"),G4_H12(normal));
    *limit=force*0.5f+len*0.25f+bias+normal[2]; if(Known(self))G4_F(self,0x4C)+=(*limit)*0.001f;
    g4S.sprSeed=g4S.sprSeed*1664525u+1013904223u; return uint8((g4S.sprSeed>>29)&1u); }
// CPhysical::ApplySpringDampening(float, float, CVector&, CVector&, CVector&): thiscall, ret 14h (0x543E90)
inline uint8 __fastcall G4_Damp(void* self,void*,float damping,float limit,const float* dir,const float* pos,const float* speed){
    if(!Mine())return 0; Push(TG("ApplySpringDampening"),self,damping,limit,G4_H12(dir),G4_H12(pos),G4_H12(speed));
    if(Known(self))G4_F(self,0x48)-=damping*limit*0.001f; return 1; }
// CWaterLevel::GetWaterLevel(float x,y,z, float* level, bool, void*): cdecl (0x6EB690)
inline uint8 __cdecl G4_Water(float x,float y,float z,float* level,uint32 flag,void* p){
    if(!Mine())return 0; Push(TG("GetWaterLevel"),x,y,z,flag&0xFFu,p); const int n=g4S.wN<4 ? g4S.wN++ : 3;
    *level=g4S.wLevel[n]; return g4S.wRet[n]; }
// CVehicle::ProcessWheel(CVector&x4, int, float x4, uint8, float*, tWheelState*, eWheelStatus): thiscall, ret 30h (0x6D6C00)
inline void __fastcall G4_Wheel(void* self,void*,const float* fwd,const float* right,const float* speed,const float* point,int32 onGround,
                                float thrust,float brake,float adhesion,uint32 wheelId,float* burnout,int32* state,int32 status){
    if(!Mine())return; float v[12]; std::memcpy(v,fwd,12); std::memcpy(v+3,right,12); std::memcpy(v+6,speed,12); std::memcpy(v+9,point,12);
    const uint32 packed=(uint32(onGround)&0xFFu)|((wheelId&0xFFu)<<8)|((uint32(status)&0xFFu)<<16)|((uint32(*state)&0xFFu)<<24);
    Push(TG("ProcessWheel"),self,Fnv32(v,48),packed,thrust,brake,adhesion);
    *state=g4S.wheelOut[wheelId&3u]; (void)burnout; }
// cBuoyancy::ProcessBuoyancy(CPhysical*, float, CVector*, CVector*): thiscall, ret 10h, bool (0x6C3EF0)
inline uint8 __fastcall G4_Buoy(void* self,void*,void* entity,float constant,float* turnPt,float* force){
    if(!Mine())return 0; (void)self; Push(TG("cBuoyancy_ProcessBuoyancy"),entity,constant);
    if(g4S.buoyRet){ for(int i=0;i<3;++i){ turnPt[i]=g4S.buoyTurn[i]; force[i]=g4S.buoyForce[i]; } }
    return g4S.buoyRet; }
// CVehicle::BlowUpCar(CEntity*, bool) (ranura 41 de la vtable falsa): thiscall, ret 8
inline void __fastcall G4_BlowUp(void* self,void*,void* damager,uint32 hide){ if(!Mine())return; Push(TG("BlowUpCar"),self,damager,hide&0xFFu); }

// CCollision::ProcessColModels(const CMatrix&, CColModel&, const CMatrix&, CColModel&, std::array<CColPoint,32>&, CColPoint*, float*, bool): cdecl (0x4185C0)
inline void G4_FillColPoint(uint8* cp,G4_Rng& r){
    std::memset(cp,0,0x2C);
    At<float>(cp,0)=r.u(-50.f,50.f); At<float>(cp,4)=r.u(-50.f,50.f); At<float>(cp,8)=r.u(0.f,30.f);
    float nx=r.u(-1.f,1.f), ny=r.u(-1.f,1.f), nz=r.u(-1.f,1.f); double l=std::sqrt(double(nx)*nx+double(ny)*ny+double(nz)*nz);
    if(l<1e-3){ nx=0.f; ny=0.f; nz=1.f; l=1.0; }
    At<float>(cp,0x10)=float(nx/l); At<float>(cp,0x14)=float(ny/l); At<float>(cp,0x18)=float(nz/l);
    At<uint8>(cp,0x20)= (r.n()%8==0) ? uint8(65) : uint8(r.n()%100);          // surfaceTypeA (a veces SURFACE_CAR_MOVINGCOMPONENT)
    At<uint8>(cp,0x21)=uint8(r.n()%20); At<uint8>(cp,0x22)=uint8(r.n());
    At<uint8>(cp,0x23)=uint8(r.n()%179);                                       // surfaceTypeB (siempre valido en la tabla)
    At<uint8>(cp,0x24)=uint8(r.n()%20); At<uint8>(cp,0x25)=uint8(r.n());
    At<float>(cp,0x28)=r.u(0.f,1.f); }
inline int32 __cdecl G4_ProcCol(const void* mA,void* cmA,const void* mB,void* cmB,void* out,void* wheelCP,float* dists,uint32 flag){
    if(!Mine())return 0;
    uint8* cdA=At<uint8*>(cmA,0x2C); uint8* cdB=At<uint8*>(cmB,0x2C);
    const uint32 st=uint32(At<uint8>(cdA,6))|(uint32(At<uint16>(cdA,4))<<8)|(uint32(At<uint16>(cdB,4))<<24);
    Push(TG("ProcessColModels"),mA,cmA,mB,cmB,flag&0xFFu,st);
    Push(TG("ProcessColModels_out"),out,wheelCP,dists);
    G4_Rng r{g4S.pcSeed};
    for(int32 i=0;i<g4S.pcN && i<32;++i) G4_FillColPoint(static_cast<uint8*>(out)+0x2C*size_t(i),r);
    if(g4S.pcForklift){ for(int32 i=0;i<g4S.pcN && i<32;++i) At<uint8>(static_cast<uint8*>(out),0x2C*size_t(i)+0x20)=(r.n()%3==0) ? uint8(65) : uint8(r.n()%60); }
    for(int i=0;i<12;++i){ G4_FillColPoint(static_cast<uint8*>(wheelCP)+0x2C*size_t(i),r); dists[i]=g4S.pcDist[i]; }
    return g4S.pcN; }
// CPhysical::AddCollisionRecord(CEntity*): thiscall, ret 4 (0x543490); CEntity::RegisterReference(CEntity**): thiscall, ret 4 (0x571B70)
inline void __fastcall G4_AddRec(void* self,void*,void* other){ if(!Mine())return; Push(TG("AddCollisionRecord"),self,other); }
// CEntity::CleanUpOldReference(CEntity**): thiscall, ret 4 (0x571A00)
inline void __fastcall G4_CleanUp(void* self,void*,void** ref){ if(!Mine())return; Push(TG("CleanUpOldReference"),self,ref); }
inline void __fastcall G4_RegRef(void* self,void*,void** ref){ if(!Mine())return; Push(TG("RegisterReference"),self,ref); }

// CPhysical::CheckCollision(): thiscall sin argumentos, bool en AL (0x54D920)
inline uint8 __fastcall G4_Check(void* self,void*){ if(!Mine())return 0; Push(TG("CheckCollision"),self); const uint32 k=uint32(g4S.checkN++); return uint8((g4S.checkBits>>(k&31u))&1u); }
inline void __fastcall G4_Skip(void* self,void*){ if(!Mine())return; Push(TG("SkipPhysics"),self); }
// CPhysical::ApplyMoveSpeed / ApplyTurnSpeed: thiscall sin argumentos (0x542DD0 / 0x542E20). Efecto simplificado sobre la posicion / el giro
inline void __fastcall G4_MoveSpeed(void* self,void*){ if(!Mine())return; Push(TG("ApplyMoveSpeed"),self);
    if(Known(self)){ auto* m=At<uint8*>(self,0x14); if(m&&Known(m)){ G4_F(m,0x30)+=G4_F(self,0x44)*0.5f; G4_F(m,0x34)+=G4_F(self,0x48)*0.5f; G4_F(m,0x38)+=G4_F(self,0x4C)*0.5f; } } }
inline void __fastcall G4_TurnSpeed(void* self,void*){ if(!Mine())return; Push(TG("ApplyTurnSpeed"),self);
    if(Known(self)){ auto* m=At<uint8*>(self,0x14); if(m&&Known(m)){ G4_F(m,0x30)+=G4_F(self,0x50)*0.25f; } } }

// ---- VehicleDamage ----
// CVehicle::CanVehicleBeDamaged(CEntity*, eWeaponType, bool&): thiscall, ret 0Ch (0x6D1280)
inline uint8 __fastcall G4_CanDmg(void* self,void*,void* damager,int32 weapon,uint8* unused){ if(!Mine())return 1; (void)unused; Push(TG("CanVehicleBeDamaged"),self,damager,weapon); return g4S.canDamage; }
// CBike::DamageKnockOffRider(CVehicle*, float, uint16, CEntity*, CVector&, CVector&): cdecl (0x6B5A10)
inline uint8 __cdecl G4_Knock(void* veh,float intensity,uint32 piece,void* entity,const float* pos,const float* vel){
    if(!Mine())return 0; Push(TG("DamageKnockOffRider"),veh,intensity,piece&0xFFFFu,entity,G4_H12(pos)); Push(TG("DamageKnockOffRider_v"),G4_H12(vel)); return g4S.knockRet; }
// CStats::GetPercentageProgress(): cdecl, float en ST0 (0x5591E0). (CGame::CanSeeOutSideFromCurrArea no se dobla: el EXE lee currArea directamente)
inline float __cdecl G4_Progress(){ if(!Mine())return 0.f; Push(TG("GetPercentageProgress")); return g4S.progress; }
// CPlayerPed::SetWantedLevelNoDrop(eWantedLevel): thiscall, ret 4 (0x609F30)
inline void __fastcall G4_Wanted(void* self,void*,int32 level){ if(!Mine())return; Push(TG("SetWantedLevelNoDrop"),self,level); }
// CPad::StartShake(int16, uint8, uint32): thiscall, ret 0Ch (0x53F920). El EXE apila el byte de frecuencia con basura en los 24 bits altos
inline void __fastcall G4_Shake(void* self,void*,int32 time,uint32 freq,uint32 arg){ if(!Mine())return; Push(TG("StartShake"),self,time,freq&0xFFu,arg); }
// CInterestingEvents::Add(type, CEntity*): thiscall, ret 8 (0x602590)
inline void __fastcall G4_Interesting(void* self,void*,int32 type,void* entity){ if(!Mine())return; Push(TG("InterestingEvents_Add"),self,type,entity); }
// CAEVehicleAudioEntity::AddAudioEvent(int, float): thiscall, ret 8 (0x4F6420); GetVehicleTypeForAudio(): thiscall, int16 en AX (0x4F4F00)
inline void __fastcall G4_AudioEvent(void* self,void*,int32 ev,float vol){ if(!Mine())return; Push(TG("AddAudioEvent"),self,ev,vol); }
inline int16 __fastcall G4_VehTypeAudio(void* self,void*){ if(!Mine())return 0; Push(TG("GetVehicleTypeForAudio"),self); return g4S.vehAudioType; }
// CVehicle::PickRandomPassenger(): thiscall, CPed* (0x6D2A10)
inline void* __fastcall G4_PickPassenger(void* self,void*){ if(!Mine())return nullptr; Push(TG("PickRandomPassenger"),self); return g4S.pickPed; }
// CPed::Say(eGlobalSpeechContext, uint32, float, bool, bool, bool): thiscall, ret 18h, int16 (0x5EFFE0)
inline int16 __fastcall G4_Say(void* self,void*,int32 ctx,uint32 delay,float prob,uint32 a,uint32 b,uint32 cc){
    if(!Mine())return 0; Push(TG("Say"),self,ctx,delay,prob,(a&0xFFu)|((b&0xFFu)<<8)|((cc&0xFFu)<<16)); return 1; }
// CDamageManager::ApplyDamage(CAutomobile*, tComponent, float, float): thiscall, ret 10h, bool (0x6C24B0). Puede romper una luz del coche
inline uint8 __fastcall G4_ApplyDamage(void* self,void*,void* veh,uint32 comp,float intensity,float mult){
    if(!Mine())return 0; Push(TG("ApplyDamage"),self,veh,comp&0xFFu,intensity,mult);
    if(g4S.lightSmash && Known(self)){ At<uint32>(self,0x10)|= (g4S.lightSmash & 3u) << (2u*((comp+g4S.lightSmash)&3u)); }
    return g4S.applyRet; }
// CAutomobile::dmgDrawCarCollidingParticles(const CVector&, float, eWeaponType): thiscall, ret 0Ch (0x6A6DC0)
inline void __fastcall G4_DmgDraw(void* self,void*,const float* pos,float force,int32 weapon){ if(!Mine())return; Push(TG("dmgDrawCarCollidingParticles"),self,G4_H12(pos),force,weapon); }
// CVehicle::ReactToVehicleDamage(CPed*): thiscall, ret 4 (0x6D5490)
inline void __fastcall G4_React(void* self,void*,void* ped){ if(!Mine())return; Push(TG("ReactToVehicleDamage"),self,ped); }
// CEventVehicleDamageCollision::CEventVehicleDamageCollision(CVehicle*, CEntity*, eWeaponType): thiscall, ret 0Ch (0x6A0620); ~CEventVehicleDamage (0x4B1990)
inline void* __fastcall G4_EvVdCtor(void* self,void*,void* veh,void* attacker,int32 weapon){ if(!Mine())return self; const int id=G4_ObjNew(self); Push(TG("EventVehicleDamageCollision_ctor"),id,veh,attacker,weapon); return self; }
inline void __fastcall G4_EvVdDtor(void* self,void*){ if(!Mine())return; Push(TG("EventVehicleDamage_dtor"),G4_ObjId(self)); }
// CEventGroup::Add(CEvent*, bool): thiscall, ret 8 (0x4AB420)
inline void* __fastcall G4_EvAdd(void* self,void*,void* ev,uint32 flag){ if(!Mine())return ev; Push(TG("EventGroup_Add"),self,G4_ObjId(ev),flag&0xFFu); return ev; }
// CPlayerPed::HandlePlayerBreath(bool, float): thiscall, ret 8 (0x60A8D0)
inline void __fastcall G4_Breath(void* self,void*,uint32 flag,float f){ if(!Mine())return; Push(TG("HandlePlayerBreath"),self,flag&0xFFu,f); }
// CPedDamageResponseCalculator::CPedDamageResponseCalculator(CEntity*, float, eWeaponType, ePedPieceTypes, bool): thiscall, ret 14h (0x4AD3F0)
inline void* __fastcall G4_PdrCtor(void* self,void*,void* entity,float dmg,int32 weapon,int32 piece,uint32 speak){
    if(!Mine())return self; const int id=G4_ObjNew(self); Push(TG("PedDamageResponseCalculator_ctor"),id,entity,dmg,weapon,(uint32(piece)&0xFFu)|((speak&0xFFu)<<8)); return self; }
// CPedDamageResponseCalculator::ComputeDamageResponse(CPed*, CPedDamageResponse&, bool): thiscall, ret 0Ch (0x4B5AC0)
inline void __fastcall G4_PdrCompute(void* self,void*,void* ped,void* resp,uint32 flag){ if(!Mine())return; Push(TG("ComputeDamageResponse"),G4_ObjId(self),ped,G4_ObjId(resp),flag&0xFFu); }
// CEventDamage::CEventDamage(CEntity*, uint32, eWeaponType, ePedPieceTypes, uint8, bool, bool): thiscall, ret 1Ch (0x4AD830)
inline void* __fastcall G4_EvDmgCtor(void* self,void*,void* src,uint32 start,int32 weapon,int32 piece,uint32 dir,uint32 a7,uint32 inVeh){
    if(!Mine())return self; const int id=G4_ObjNew(self); Push(TG("EventDamage_ctor"),id,src,start,weapon,(uint32(piece)&0xFFu)|((dir&0xFFu)<<8)|((a7&0xFFu)<<16)|((inVeh&0xFFu)<<24)); return self; }
inline uint8 __fastcall G4_EvDmgAffects(void* self,void*,void* ped){ if(!Mine())return 0; Push(TG("EventDamage_AffectsPed"),G4_ObjId(self),ped); return g4S.affects; }
inline void __fastcall G4_EvDmgDtor(void* self,void*){ if(!Mine())return; Push(TG("EventDamage_dtor"),G4_ObjId(self)); }
// CEventKnockOffBike::CEventKnockOffBike(CVehicle*, const CVector&, const CVector&, float, float, uint8, uint8, int32, CPed*, bool, bool): thiscall, ret 2Ch (0x4AFCF0)
inline void* __fastcall G4_EvKnockCtor(void* self,void*,void* veh,const float* moveSpeed,const float* impactVel,float dmg,float force,uint32 type,uint32 dir,int32 time,void* ped,uint32 victim,uint32 forceOff){
    if(!Mine())return self; const int id=G4_ObjNew(self);
    Push(TG("EventKnockOffBike_ctor"),id,veh,G4_H12(moveSpeed),G4_H12(impactVel),dmg,force);
    Push(TG("EventKnockOffBike_ctor2"),type&0xFFu,dir&0xFFu,time,ped,(victim&0xFFu)|((forceOff&0xFFu)<<8)); return self; }
inline void __fastcall G4_EvKnockDtor(void* self,void*){ if(!Mine())return; Push(TG("EventKnockOffBike_dtor"),G4_ObjId(self)); }

}
