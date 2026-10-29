#pragma once
// Grupo G5 del banco AutomobileFinal4 (ProcessControl 0x6B1880 y ProcessAI 0x6B4800): estado del escenario, buferes propios
// (peatones falsos, datos del jugador) y dobles trazados de TODAS las subfunciones. Los dobles se parchean en la direccion del
// EXE y en el simbolo/cuerpo de la DLL (AddHookStub/AddStub) y las ranuras virtuales usadas se sustituyen en la vtable falsa
// (SetVt). Asi solo se compara el codigo propio de ProcessControl/ProcessAI (flujo, ramas, banderas, formulas) mas la traza
// completa de las llamadas salientes y sus argumentos.
#include "Ctx.hpp"
#include "Pad.h"
#include "Stats.h"
#include "Garages.h"
#include "CarAI.h"
#include "PlayerInfo.h"
#include "Camera.h"
#include "MouseControllerState.h"
#include "SurfaceInfos_c.h"
#include "Collision/ColModel.h"
#include "Collision/CollisionData.h"

namespace AutomobileFinal4Live {

// ---------------------------------------------------------------------------------------------------------------------
// Offsets crudos del EXE (leidos del desensamblado de 6B1880 / 6B4800); LayoutG5 comprueba los que tienen miembro en la DLL
// ---------------------------------------------------------------------------------------------------------------------
namespace G5o {
constexpr size_t Flags=0x1C, Seed=0x20, Model=0x22, Status=0x36, Phys=0x40, MoveSpd=0x44, TurnSpd=0x50, FricMove=0x5C, FricTurn=0x68,
    Force=0x74, Torque=0x80, Mass=0x8C, TurnMass=0x90, VelFreq=0x94, CoM=0xA4, Fake=0xB8, MovingSpeed=0xD4, DmgInt=0xD8, DmgEnt=0xDC,
    Impact=0xE0, Audio=0x138, Handling=0x384, HFlags=0x38C, Mission=0x3BA, ApSpeed=0x3C8, ApCtrl=0x3DB, RecId=0x424, VFlags=0x428,
    Alarm=0x45C, Driver=0x460, Pass=0x464, NumPass=0x484, MaxPass=0x488, Nitro=0x48A, SpecCol=0x48B, Steer=0x494, Steer2=0x498, Gas=0x49C,
    Brake=0x4A0, CreatedBy=0x4A4, Bomb=0x4A8, Gear=0x4B4, GearCount=0x4B8, SpinAudio=0x4BC, Health=0x4C0, Towing=0x4C4, BeingTowed=0x4C8,
    BombOwner=0x4CC, BombTimer=0x4DE, Detonator=0x4E0, Horn=0x514, SubType=0x594, DmgMgr=0x5A0, WheelCol=0x724, Comp=0x7D4, CompPrev=0x7E4,
    Counts=0x7F4, Inertia1=0x808, Inertia2=0x80C, SkidType=0x810, Muddy=0x824, WheelRot=0x828, WheelSpeed=0x848, AutoFlags=0x868, Burnout=0x86A,
    MiscAngle=0x86C, MiscAnglePrev=0x86E, BusEnd=0x870, SpringLen=0x878, LineLen=0x888, FrontH=0x898, RearH=0x89C, CarTraction=0x8A0,
    TireTemp=0x8A4, WheelEnt=0x8E8, WheelEntPos=0x8F8, Doom=0x94C, Contact=0x960, DriveWheels=0x961, DriveWheelsLast=0x962, WheelState=0x968,
    NitroFx=0x978, FireCounter=0x981, PlaneBreak=0x994, HeliBreak=0x998;
}

// ---------------------------------------------------------------------------------------------------------------------
// Estado del escenario: lo que devuelven/hacen los dobles (identico en ambos lados; se rellena antes de cada llamada)
// ---------------------------------------------------------------------------------------------------------------------
struct G5State {
    uint32 aiOr, aiRet;                         // doble de ProcessAI (solo en el banco de ProcessControl)
    uint32 upsideDown, canPedEnter, usesSiren, umc, rcHit, gunDown, handbrake, accel, plosRet;
    int32  steerLR, steerUD, gunLR, gunUD;      // direccion / canon (int16 extendidos)
    void*  pVeh; void* pPed; void* plosHit;     // FindPlayerVehicle / FindPlayerPed / entidad que devuelve ProcessLineOfSight
    float  pCoors[3], pSpeed[3];                // FindPlayerCoors / FindPlayerSpeed
    int32  remoteSlot; uint32 taskMask; float fat;
    float  susp[4]; uint32 suspDo;              // ProcessSuspension (virtual 70): compresiones que deja el doble
    uint32 buoy;                                // ProcessBuoyancy / ProcessBoatControl: 0 nada, 1 sumergido, 2 no sumergido
    uint32 blowWrecks, takeChanges; void* takeTarget;   // BlowUpCar -> WRECKED, TakeRemoteControlledCarFromPlayer -> PHYSICS (en takeTarget)
    uint32 soilOr;                              // DoSoftGroundResistance: OR de las banderas extra
};
inline G5State g5;

constexpr size_t G5_PED_SIZE=0x800;
alignas(16) inline uint8 g5ped[4][G5_PED_SIZE];     // 0 conductor, 1..2 pasajeros, 3 jugador (FindPlayerPed)
alignas(16) inline uint8 g5intel[4][0x400];         // CPedIntelligence falsos (m_TaskMgr en +4)
alignas(16) inline uint8 g5aux[0x400];              // +0 CPlayerPedData falso (+0 m_pWanted), +0x100 CWanted falso (+0x2C nivel), +0x200 tarea falsa
constexpr uintptr_t G5_PADS=0xB73458;               // CPad::Pads (el EXE lo lee en linea; la DLL tambien)

inline uint64 G5Fnv(){ uint64 h=Fnv64(g5ped,sizeof(g5ped)); h^=Fnv64(g5intel,sizeof(g5intel)); h*=1099511628211ull; h^=Fnv64(g5aux,sizeof(g5aux)); return h; }

// ---------------------------------------------------------------------------------------------------------------------
// Dobles. Todos: "if(!Mine()) return neutro" (hilos ajenos); thiscall = __fastcall(self, edx_no_usado, args); el 'ret N' coincide con el del EXE.
// ---------------------------------------------------------------------------------------------------------------------
// --- CWorld ---
inline void* __cdecl G5S_FindPlayerVehicle(int32 pid,bool remote){ if(!Mine())return nullptr; Push(TG("FindPlayerVehicle"),pid,remote); return g5.pVeh; }
inline void* __cdecl G5S_FindPlayerPed(int32 pid){ if(!Mine())return nullptr; Push(TG("FindPlayerPed"),pid); return g5.pPed; }
inline void* __cdecl G5S_FindPlayerCoors(float* out,int32 pid){ if(!Mine())return out; Push(TG("FindPlayerCoors"),pid); out[0]=g5.pCoors[0]; out[1]=g5.pCoors[1]; out[2]=g5.pCoors[2]; return out; }
inline float* __cdecl G5S_FindPlayerSpeed(int32 pid){ if(!Mine())return g5.pSpeed; Push(TG("FindPlayerSpeed"),pid); return g5.pSpeed; }
inline uint32 __cdecl G5S_ProcessLineOfSight(const float* o,const float* t,void* cp,void** hit,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7,bool b8){
    if(!Mine())return 0;
    Push(TG("ProcessLineOfSight"),o[0],o[1],o[2],t[0],t[1],t[2]);
    Push(TG("ProcessLineOfSightFlags"),cp,hit,b1,b2,b3,b4);
    Push(TG("ProcessLineOfSightFlags2"),b5,b6,b7,b8);
    *hit=g5.plosHit; return g5.plosRet; }
inline int32 __cdecl G5S_FindPlayerSlot(void* veh){ if(!Mine())return -1; Push(TG("FindPlayerSlotWithRemoteVehiclePointer"),veh); return g5.remoteSlot; }

// --- CPad (this = pad; devuelven int16: el EXE comprueba ax) ---
inline int16 __fastcall G5S_CarGunJustDown(void* pad,void*){ if(!Mine())return 0; Push(TG("CarGunJustDown"),pad); return int16(g5.gunDown ? 1 : 0); }
inline int16 __fastcall G5S_SteerLR(void* pad,void*){ if(!Mine())return 0; Push(TG("GetSteeringLeftRight"),pad); return int16(g5.steerLR); }
inline int16 __fastcall G5S_SteerUD(void* pad,void*){ if(!Mine())return 0; Push(TG("GetSteeringUpDown"),pad); return int16(g5.steerUD); }
inline int16 __fastcall G5S_HandBrake(void* pad,void*){ if(!Mine())return 0; Push(TG("GetHandBrake"),pad); return int16(g5.handbrake ? 1 : 0); }
inline int16 __fastcall G5S_Accelerate(void* pad,void*){ if(!Mine())return 0; Push(TG("GetAccelerate"),pad); return int16(g5.accel ? 1 : 0); }
inline int16 __fastcall G5S_GunLR(void* pad,void*){ if(!Mine())return 0; Push(TG("GetCarGunLeftRight"),pad); return int16(g5.gunLR); }
inline int16 __fastcall G5S_GunUD(void* pad,void*){ if(!Mine())return 0; Push(TG("GetCarGunUpDown"),pad); return int16(g5.gunUD); }
inline void __fastcall G5S_StartShake(void* pad,void*,int32 time,uint32 freq,uint32 arg2){
    if(!Mine())return; Push(TG("StartShake"),pad,uint32(uint16(time)),uint32(uint8(freq)),arg2); }   // el EXE pasa 'freq' con basura en los 3 bytes altos
inline void* __fastcall G5S_GetPadFromPlayer(void* ped,void*){ if(!Mine())return reinterpret_cast<void*>(G5_PADS); Push(TG("GetPadFromPlayer"),ped); return reinterpret_cast<void*>(G5_PADS); }
inline void __fastcall G5S_ResetPlayerBreath(void* ped,void*){ if(!Mine())return; Push(TG("ResetPlayerBreath"),ped); }
inline void* __fastcall G5S_FindActiveTask(void* mgr,void*,int32 type){
    if(!Mine())return nullptr; Push(TG("FindActiveTaskByType"),mgr,type);
    const int bit = type==823 ? 0 : type==825 ? 1 : type==809 ? 2 : -1;
    return (bit>=0 && ((g5.taskMask>>bit)&1u)) ? static_cast<void*>(g5aux+0x200) : nullptr; }

// --- CCarAI / CCarCtrl / CRemote (solo EXE: la DLL llama por plugin::Call) ---
inline void __cdecl G5S_UpdateCarAI(void* veh){ if(!Mine())return; Push(TG("UpdateCarAI"),veh); }
inline float __cdecl G5S_GetCarToPark(void* veh,const float* p){ if(!Mine())return 0.f; Push(TG("GetCarToParkAtCoors"),veh,p[0],p[1],p[2]); return 0.f; }
inline void __cdecl G5S_MakeWay(void* veh){ if(!Mine())return; Push(TG("MakeWayForCarWithSiren"),veh); }
inline void __cdecl G5S_SteerAI(void* veh){ if(!Mine())return; Push(TG("SteerAICarWithPhysics"),veh); }
inline void __cdecl G5S_Reconsider(void* veh){ if(!Mine())return; Push(TG("ReconsiderRoute"),veh); }
inline void __cdecl G5S_OnRails(void* veh){ if(!Mine())return; Push(TG("UpdateCarOnRails"),veh); }
inline void __cdecl G5S_PedDanger(void* veh){ if(!Mine())return; Push(TG("ScanForPedDanger"),veh); }
inline void __cdecl G5S_TakeRemote(bool explode){ if(!Mine())return; Push(TG("TakeRemoteControlledCarFromPlayer"),explode);
    if(g5.takeChanges && g5.takeTarget){ auto* st=&At<uint8>(g5.takeTarget,G5o::Status); *st=uint8((*st&7)|(3<<3)); } }
inline void __fastcall G5S_DriveBy(void* self,void*){ if(!Mine())return; Push(TG("DoDriveByShootings"),self); }
inline float __cdecl G5S_FatMuscle(int32 stat){ if(!Mine())return 1.f; Push(TG("GetFatAndMuscleModifier"),stat); return g5.fat; }

// --- CVehicle (this = coche) ---
inline void __fastcall G5S_CarAlarm(void* self,void*){ if(!Mine())return; Push(TG("ProcessCarAlarm"),self); }
inline void __fastcall G5S_ClumpAlpha(void* self,void*){ if(!Mine())return; Push(TG("UpdateClumpAlpha"),self); }
inline void __fastcall G5S_PassengerList(void* self,void*){ if(!Mine())return; Push(TG("UpdatePassengerList"),self); }
inline uint32 __fastcall G5S_IsUpsideDown(void* self,void*){ if(!Mine())return 0; Push(TG("IsUpsideDown"),self); return g5.upsideDown ? 1u : 0u; }
inline uint32 __fastcall G5S_CanPedEnter(void* self,void*){ if(!Mine())return 0; Push(TG("CanPedEnterCar"),self); return g5.canPedEnter ? 1u : 0u; }
inline uint32 __fastcall G5S_UsesSiren(void* self,void*){ if(!Mine())return 0; Push(TG("UsesSiren"),self); return g5.usesSiren ? 1u : 0u; }
inline void __fastcall G5S_BoatControl(void* self,void*,void* boat,float* rot,bool hit,bool post){
    if(!Mine())return; Push(TG("ProcessBoatControl"),self,boat,rot,hit,post);
    if(g5.buoy==1) At<uint32>(self,G5o::Phys)|=0x100u; else if(g5.buoy==2) At<uint32>(self,G5o::Phys)&=~0x100u; }
inline void __fastcall G5S_SirenHorn(void* self,void*,bool a){ if(!Mine())return; Push(TG("ProcessSirenAndHorn"),self,a); }
inline void __fastcall G5S_TrailerLink(void* self,void*,bool a,bool b){ if(!Mine())return; Push(TG("UpdateTrailerLink"),self,a,b); }
inline void __fastcall G5S_TractorLink(void* self,void*,bool a,bool b){ if(!Mine())return; Push(TG("UpdateTractorLink"),self,a,b); }
inline void __fastcall G5S_Prune(void* self,void*){ if(!Mine())return; Push(TG("PruneReferences"),self); }
inline void __fastcall G5S_RegRef(void* self,void*,void** ref){ if(!Mine())return; Push(TG("RegisterReference"),self,ref); }
inline void __fastcall G5S_CleanRef(void* self,void*,void** ref){ if(!Mine())return; Push(TG("CleanUpOldReference"),self,ref); }
inline void __cdecl G5S_Garage(const char* tag,int32 a,uint32 t,int32 b){ if(!Mine())return; Push(TG("CGarages::TriggerMessage"),S(tag),uint32(uint16(a)),uint32(uint16(t)),uint32(uint16(b))); }
inline void __fastcall G5S_AudioService(void* audio,void*){ if(!Mine())return; Push(TG("CAEVehicleAudioEntity::Service"),audio); }
inline void __fastcall G5S_FxKill(void* fx,void*){ if(!Mine())return; Push(TG("FxSystem_c::Kill"),fx); }

// --- CPhysical (this = coche u otra entidad) ---
inline void __fastcall G5S_SkipPhysics(void* self,void*){ if(!Mine())return; Push(TG("SkipPhysics"),self); }
inline void __fastcall G5S_AddMoving(void* self,void*){ if(!Mine())return; Push(TG("AddToMovingList"),self); }
inline void __fastcall G5S_RemoveMoving(void* self,void*){ if(!Mine())return; Push(TG("RemoveFromMovingList"),self); }
inline void __fastcall G5S_PhysicalControl(void* self,void*){ if(!Mine())return; Push(TG("CPhysical::ProcessControl"),self); }
inline void __fastcall G5S_TurnForce(void* self,void*,float fx,float fy,float fz,float px,float py,float pz){
    if(!Mine())return; Push(TG("ApplyTurnForce"),self,fx,fy,fz); Push(TG("ApplyTurnForcePoint"),px,py,pz); }
inline void __fastcall G5S_MoveForce(void* self,void*,float fx,float fy,float fz){ if(!Mine())return; Push(TG("ApplyMoveForce"),self,fx,fy,fz); }
inline void* __fastcall G5S_GetSpeed(void* self,void*,float* out,float x,float y,float z){
    if(!Mine()){ out[0]=out[1]=out[2]=0.f; return out; }
    Push(TG("GetSpeed"),self,x,y,z);
    const float k=float(Id(self)&0xFFu)*1.0e-5f;
    out[0]=z*0.25f+0.001f+k; out[1]=x*0.5f-k; out[2]=y*0.75f-0.002f; return out; }

// --- CAutomobile (funciones de la clase que ProcessControl/ProcessAI llaman directamente) ---
inline void __fastcall G5S_BusDoors(void* self,void*){ if(!Mine())return; Push(TG("ProcessAutoBusDoors"),self); }
inline void __fastcall G5S_Hydraulic(void* self,void*){ if(!Mine())return; Push(TG("HydraulicControl"),self); }
inline uint32 __fastcall G5S_UpdateMovingCollision(void* self,void*,float angle){ if(!Mine())return 0; Push(TG("UpdateMovingCollision"),self,angle); return g5.umc ? 1u : 0u; }
inline void __fastcall G5S_Tank(void* self,void*){ if(!Mine())return; Push(TG("TankControl"),self); }
inline void __fastcall G5S_TowTruck(void* self,void*){ if(!Mine())return; Push(TG("TowTruckControl"),self); }
inline void __fastcall G5S_FireTruck(void* self,void*,void* fire){ if(!Mine())return; Push(TG("FireTruckControl"),self,fire); }
inline void __fastcall G5S_BoostJump(void* self,void*){ if(!Mine())return; Push(TG("BoostJumpControl"),self); }
inline void __fastcall G5S_Nitrous(void* self,void*,int32 boost){ if(!Mine())return; Push(TG("NitrousControl"),self,int32(int8(boost))); }
inline void __fastcall G5S_WheelPair(void* self,void*,int32 l,int32 r,float steer,const float* speeds,const float* points,float traction,float accel,float brake,bool front){
    if(!Mine())return;
    Push(TG("ProcessCarWheelPair"),self,l,r,steer,traction,accel);
    Push(TG("ProcessCarWheelPair2"),brake,front,Fnv32(speeds,48),Fnv32(points,48)); }
inline void __fastcall G5S_Buoyancy(void* self,void*){
    if(!Mine())return; Push(TG("ProcessBuoyancy"),self);
    if(g5.buoy==1) At<uint32>(self,G5o::Phys)|=0x100u; else if(g5.buoy==2) At<uint32>(self,G5o::Phys)&=~0x100u; }
inline void __fastcall G5S_OnFire(void* self,void*,bool now){ if(!Mine())return; Push(TG("ProcessCarOnFireAndExplode"),self,now); }
inline void __fastcall G5S_Harvester(void* self,void*){ if(!Mine())return; Push(TG("ProcessHarvester"),self); }
inline void __fastcall G5S_ScanCrimes(void* self,void*){ if(!Mine())return; Push(TG("ScanForCrimes"),self); }
inline void __fastcall G5S_PlayHorn(void* self,void*){ if(!Mine())return; Push(TG("PlayHornIfNecessary"),self); }
inline void __fastcall G5S_SoftGround(void* self,void*,uint32* flags){ if(!Mine())return; Push(TG("DoSoftGroundResistance"),self,*flags); *flags|=g5.soilOr; }
inline uint32 __fastcall G5S_RcHitWheels(void* self,void*){ if(!Mine())return 0; Push(TG("RcbanditCheckHitWheels"),self); return g5.rcHit ? 1u : 0u; }

// --- ranuras virtuales del coche (vtable falsa vtFake) ---
inline void __fastcall G5S_V_CollisionCheck(void* self,void*,bool applySpeed){ if(!Mine())return; Push(TG("ProcessControlCollisionCheck"),self,applySpeed); }
inline void __fastcall G5S_V_ControlInputs(void* self,void*,uint32 playerNum){ if(!Mine())return; Push(TG("ProcessControlInputs"),self,uint32(uint8(playerNum))); }
inline void __fastcall G5S_V_BlowUp(void* self,void*,void* dmgr,bool hide){
    if(!Mine())return; Push(TG("BlowUpCar"),self,dmgr,hide);
    if(g5.blowWrecks){ auto* st=&At<uint8>(self,G5o::Status); *st=uint8((*st&7)|(5<<3)); } }
inline void __fastcall G5S_V_VehicleDamage(void* self,void*,float dmg,int32 comp,void* damager,float* coors,float* dir,int32 weapon){
    if(!Mine())return; Push(TG("VehicleDamage"),self,dmg,comp,damager,coors,dir); Push(TG("VehicleDamageWeapon"),weapon); }
inline uint32 __fastcall G5S_V_BreakTow(void* self,void*){ if(!Mine())return 0; Push(TG("BreakTowLink"),self); return 0; }
inline uint32 __fastcall G5S_V_ProcessAI(void* self,void*,uint32* flags){
    if(!Mine())return 0; Push(TG("ProcessAI"),self,*flags); *flags|=g5.aiOr; return g5.aiRet ? 1u : 0u; }
inline void __fastcall G5S_V_FlyingCar(void* self,void*){ if(!Mine())return; Push(TG("ProcessFlyingCarStuff"),self); }
inline void __fastcall G5S_V_Suspension(void* self,void*){
    if(!Mine())return; Push(TG("ProcessSuspension"),self);
    if(g5.suspDo) for(int i=0;i<4;++i) At<float>(self,G5o::Comp+4u*unsigned(i))=g5.susp[i]; }

// Instala los dobles virtuales en la vtable falsa de ESTE lado (la vtable se resetea sola en cada ejecucion)
inline void G5SetVirtuals(){
    SetVt(23,FnPtr(&G5S_V_CollisionCheck)); SetVt(24,FnPtr(&G5S_V_ControlInputs)); SetVt(41,FnPtr(&G5S_V_BlowUp)); SetVt(56,FnPtr(&G5S_V_VehicleDamage));
    SetVt(62,FnPtr(&G5S_V_BreakTow)); SetVt(66,FnPtr(&G5S_V_ProcessAI)); SetVt(68,FnPtr(&G5S_V_FlyingCar)); SetVt(70,FnPtr(&G5S_V_Suspension));
}

// ---------------------------------------------------------------------------------------------------------------------
// Registro de dobles: una sola mascara propia (bit 0) para todo el grupo
// ---------------------------------------------------------------------------------------------------------------------
constexpr uint64 G5_ALL=1ull;
inline std::string StubsG5(){
    std::string miss;
    auto H=[&](const char* cls,const char* name,uintptr_t exe,void* fn){ if(!AddHookStub(5,G5_ALL,exe,cls,name,fn)) miss+=std::string(cls)+"::"+name+" "; };
    auto E=[&](uintptr_t exe,void* fn){ AddStub(5,G5_ALL,exe,0,fn); };    // solo EXE: la DLL llama a esa direccion por plugin::Call
    // CWorld
    H("CWorld","FindPlayerVehicle",0x56E0D0,FnPtr(&G5S_FindPlayerVehicle)); H("CWorld","FindPlayerPed",0x56E210,FnPtr(&G5S_FindPlayerPed));
    H("CWorld","FindPlayerCoors",0x56E010,FnPtr(&G5S_FindPlayerCoors));     H("CWorld","FindPlayerSpeed",0x56E090,FnPtr(&G5S_FindPlayerSpeed));
    H("CWorld","ProcessLineOfSight",0x56BA00,FnPtr(&G5S_ProcessLineOfSight)); H("CWorld","FindPlayerSlotWithRemoteVehiclePointer",0x563FD0,FnPtr(&G5S_FindPlayerSlot));
    // CPad / CPlayerPed / CTaskManager
    H("CPad","CarGunJustDown",0x53FFE0,FnPtr(&G5S_CarGunJustDown));   H("CPad","GetSteeringLeftRight",0x53FB80,FnPtr(&G5S_SteerLR));
    H("CPad","GetSteeringUpDown",0x53FBD0,FnPtr(&G5S_SteerUD));       H("CPad","GetHandBrake",0x540040,FnPtr(&G5S_HandBrake));
    H("CPad","GetAccelerate",0x5403F0,FnPtr(&G5S_Accelerate));        H("CPad","GetCarGunLeftRight",0x53FC50,FnPtr(&G5S_GunLR));
    H("CPad","GetCarGunUpDown",0x53FC10,FnPtr(&G5S_GunUD));           H("CPad","StartShake",0x53F920,FnPtr(&G5S_StartShake));
    H("CPlayerPed","GetPadFromPlayer",0x609560,FnPtr(&G5S_GetPadFromPlayer)); H("CPlayerPed","ResetPlayerBreath",0x60A8A0,FnPtr(&G5S_ResetPlayerBreath));
    H("CTaskManager","FindActiveTaskByType",0x681740,FnPtr(&G5S_FindActiveTask));
    // CCarAI / CCarCtrl / CRemote / CStats
    H("CCarAI","UpdateCarAI",0x41DA30,FnPtr(&G5S_UpdateCarAI)); H("CCarAI","GetCarToParkAtCoors",0x41D350,FnPtr(&G5S_GetCarToPark));
    H("CCarAI","MakeWayForCarWithSiren",0x41D660,FnPtr(&G5S_MakeWay));
    E(0x437C20,FnPtr(&G5S_SteerAI)); /* CCarCtrl y CRemote ya son C++ real en la DLL (Fusion CarCtrl, lotes 63/75): hay que parchear tambien su simbolo, no solo la direccion del EXE */ H("CCarCtrl","ReconsiderRoute",0x42FC40,FnPtr(&G5S_Reconsider)); H("CCarCtrl","UpdateCarOnRails",0x436540,FnPtr(&G5S_OnRails)); H("CCarCtrl","ScanForPedDanger",0x42CE40,FnPtr(&G5S_PedDanger));
    H("CRemote","TakeRemoteControlledCarFromPlayer",0x45AE80,FnPtr(&G5S_TakeRemote)); AddStub(5,G5_ALL,0x559AF0,bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier),FnPtr(&G5S_FatMuscle)); /* la DLL ya no es naked: Automobile.cpp la llama directo */ H("CVehicle","DoDriveByShootings",0x741FD0,FnPtr(&G5S_DriveBy)); /* lote 80A: ahora es C++ real y esta enganchada; el falso va tambien en el cuerpo de la DLL */
    // CVehicle
    H("CVehicle","ProcessCarAlarm",0x6D21F0,FnPtr(&G5S_CarAlarm));   H("CVehicle","UpdateClumpAlpha",0x6D2980,FnPtr(&G5S_ClumpAlpha));
    H("CVehicle","UpdatePassengerList",0x6D29E0,FnPtr(&G5S_PassengerList)); H("CVehicle","IsUpsideDown",0x6D1D90,FnPtr(&G5S_IsUpsideDown));
    H("CVehicle","CanPedEnterCar",0x6D1E80,FnPtr(&G5S_CanPedEnter));  H("CVehicle","UsesSiren",0x6D8470,FnPtr(&G5S_UsesSiren));
    H("CVehicle","ProcessBoatControl",0x6DBCE0,FnPtr(&G5S_BoatControl));
    H("CVehicle","ProcessSirenAndHorn",0x6E0950,FnPtr(&G5S_SirenHorn)); /* ya es C++ real en la DLL */ H("CVehicle","UpdateTrailerLink",0x6DFC50,FnPtr(&G5S_TrailerLink)); /* ahora C++ real en la DLL (lote 81A): parchear tambien su cuerpo */ E(0x6E0050,FnPtr(&G5S_TractorLink));
    // CEntity / CPhysical / varios
    H("CEntity","PruneReferences",0x571A90,FnPtr(&G5S_Prune)); H("CEntity","RegisterReference-",0x571B70,FnPtr(&G5S_RegRef));
    H("CEntity","CleanUpOldReference-",0x571A00,FnPtr(&G5S_CleanRef));
    H("CPhysical","SkipPhysics",0x5433B0,FnPtr(&G5S_SkipPhysics)); H("CPhysical","AddToMovingList",0x542800,FnPtr(&G5S_AddMoving));
    H("CPhysical","RemoveFromMovingList",0x542860,FnPtr(&G5S_RemoveMoving)); H("CPhysical","ProcessControl",0x5485E0,FnPtr(&G5S_PhysicalControl));
    H("CPhysical","ApplyTurnForce",0x542A50,FnPtr(&G5S_TurnForce)); H("CPhysical","ApplyMoveForce-vec",0x5429F0,FnPtr(&G5S_MoveForce));
    H("CPhysical","GetSpeed",0x542CE0,FnPtr(&G5S_GetSpeed));
    H("CGarages","TriggerMessage",0x447B80,FnPtr(&G5S_Garage)); H("CAEVehicleAudioEntity","Service",0x502280,FnPtr(&G5S_AudioService));
    H("FxSystem_c","Kill",0x4AA3F0,FnPtr(&G5S_FxKill));
    // CAutomobile (las demas funciones de la clase que se llaman desde ProcessControl / ProcessAI)
    H("CAutomobile","ProcessAutoBusDoors",0x6A38A0,FnPtr(&G5S_BusDoors)); H("CAutomobile","HydraulicControl",0x6A07A0,FnPtr(&G5S_Hydraulic));
    H("CAutomobile","UpdateMovingCollision",0x6A1460,FnPtr(&G5S_UpdateMovingCollision)); H("CAutomobile","TankControl",0x6AE850,FnPtr(&G5S_Tank));
    H("CAutomobile","TowTruckControl",0x6A40F0,FnPtr(&G5S_TowTruck)); H("CAutomobile","FireTruckControl",0x729B60,FnPtr(&G5S_FireTruck));
    H("CAutomobile","BoostJumpControl",0x6A3A60,FnPtr(&G5S_BoostJump)); H("CAutomobile","NitrousControl",0x6A3EA0,FnPtr(&G5S_Nitrous));
    H("CAutomobile","ProcessCarWheelPair",0x6A4EC0,FnPtr(&G5S_WheelPair)); H("CAutomobile","ProcessBuoyancy",0x6A8C00,FnPtr(&G5S_Buoyancy));
    H("CAutomobile","ProcessCarOnFireAndExplode",0x6A7090,FnPtr(&G5S_OnFire)); H("CAutomobile","ProcessHarvester",0x6A9680,FnPtr(&G5S_Harvester));
    H("CAutomobile","ScanForCrimes",0x6ADFF0,FnPtr(&G5S_ScanCrimes)); H("CAutomobile","PlayHornIfNecessary",0x6A3820,FnPtr(&G5S_PlayHorn));
    H("CAutomobile","DoSoftGroundResistance",0x6A4AF0,FnPtr(&G5S_SoftGround)); H("CAutomobile","RcbanditCheckHitWheels",0x6B45E0,FnPtr(&G5S_RcHitWheels));
    return miss;
}
}
