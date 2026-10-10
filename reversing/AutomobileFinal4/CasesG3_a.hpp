#pragma once
// Grupo G3 (parte a): mundo propio (peatones/tareas/fx/colision falsos), entorno global (pads, temporizador, camara, jugador...),
// dobles trazados de todo lo que tocan los controles de vehiculos especiales y el registro/guardado de globales.
// Convenciones: miembro thiscall del EXE = __fastcall(self, edx_no_usado, args...) con el mismo 'ret N'; los argumentos de 12 bytes
// por valor (CVector) se declaran como la estructura V3 (mismo diseño en pila); bool/uint8 se leen del byte bajo del dword apilado.
#include "Ctx.hpp"
#include "Pad.h"
#include "Camera.h"
#include "Fx/Fx.h"
#include "Entity/Ped/PlayerPed.h"

namespace AutomobileFinal4Live {
namespace g3 {

struct V3 { float x,y,z; };

// ---------------------------------------------------------------------------------------------------------------------
// Bits de dobles (propietario 3)
// ---------------------------------------------------------------------------------------------------------------------
constexpr uint64 B(int n){ return 1ull<<n; }
constexpr uint64 B_AMF=B(0), B_ATF=B(1), B_AF=B(2), B_FOIR=B(3), B_PLOS=B(4), B_PVL=B(5), B_WADD=B(6), B_CLRSCAN=B(7), B_CAMSHAKE=B(8),
    B_KILLP=B(9), B_KILLPG=B(10), B_LAW=B(11), B_DELEX=B(12), B_SPECIAL=B(13), B_REMUPG=B(14), B_DARKEL=B(15), B_STARTFIRE=B(16),
    B_ADDEXPL=B(17), B_INCSTAT=B(18), B_SETCLUMP=B(19), B_CRIME=B(20), B_SETWANT=B(21), B_KEEPCLR=B(22), B_SAY=B(23), B_ALLOWED=B(24),
    B_FINDTASK=B(25), B_FIREHIT=B(26), B_GUNSHELL=B(27), B_TANKAIM=B(28), B_WEAPEV=B(29), B_TANKFIRE=B(30), B_WCANNON=B(31),
    B_AUDEV=B(32), B_WHEELPOSN=B(33), B_SPHBOX=B(34), B_FXPLAY=B(35), B_FXKILL=B(36), B_FXVEL=B(37), B_FXADD=B(38), B_FXCREATE=B(39),
    B_REGREF=B(40), B_CLEANREF=B(41), B_GROUPADD=B(42), B_GETGROUP=B(43), B_OBJDMG=B(44), B_LTM=B(45), B_FORALL=B(46), B_BUMPER=B(47),
    B_DOOR=B(48), B_SPAWN=B(49), B_NITRO=B(50), B_FPV_T=B(51), B_FPV_Q=B(52), B_FPP_T=B(53), B_FPP_Q=B(54);

// ---------------------------------------------------------------------------------------------------------------------
// Mundo propio. Se registra como UNA region de Id() y se hashea entero en c.out["g3"].
// ---------------------------------------------------------------------------------------------------------------------
constexpr size_t NPED=4, PED_SIZE=0x800, INTEL_SIZE=0x500;
constexpr size_t NPOOLPED=3, NPOOLOBJ=3, OBJ_SIZE=0x19C, NNODE=16;
struct G3World {
    alignas(16) uint8 ped[NPED][PED_SIZE];              // peatones falsos (0 conductor, 1 jugador, 2/3 libres)
    alignas(16) uint8 intel[NPED][INTEL_SIZE];          // CPedIntelligence falsos (el gestor de tareas empieza en +4)
    alignas(16) uint8 task[4][0x40];                    // tareas falsas
    alignas(16) void* pedVt[96];                        // vtable falsa de peatones
    alignas(16) void* taskVt[32];                       // vtable falsa de tareas
    alignas(16) uint8 fx[4][0x80];                      // FxSystem_c falsos (solo se usan sus direcciones)
    alignas(16) uint8 fire[0x40];                       // CFire falso (posicion en +4)
    alignas(16) uint8 group[0x40];                      // CEventGroup global falso
    alignas(16) uint8 poolPed[NPOOLPED][0x7C4];         // almacenamiento del pool de peatones falso
    alignas(16) uint8 poolPedState[8];
    alignas(16) uint8 poolObj[NPOOLOBJ][OBJ_SIZE];      // almacenamiento del pool de objetos falso
    alignas(16) uint8 poolObjState[8];
    alignas(16) void* objVt[96];                        // vtable falsa de objetos
    alignas(16) uint8 pmat[NPOOLPED+NPOOLOBJ+2][0x60];  // matrices para peatones/objetos del pool
    alignas(16) uint8 poolHdr[8][0x20];                 // cabeceras de pool falsas (nodos, vehiculos, peatones, objetos)
    alignas(16) uint8 nodeStore[NNODE*12];              // pool de nodos de lista doble falso
    alignas(16) uint8 nodeState[NNODE];
    alignas(16) uint8 list[0x20];                       // CPtrListDoubleLink falsa
    alignas(16) uint8 sphereBuf[0x20];
    // datos de colision falsos (modelo normal, especial)
    alignas(16) uint8 cmDataA[0x40], cmDataB[0x40];
    alignas(16) uint8 spA[8][0x14], spB[8][0x14];
    alignas(16) uint8 trA[16][8], trB[16][8];
    alignas(16) uint8 vtA[32][6], vtB[32][6];
    alignas(16) uint8 plB[16][0x10];
    alignas(16) uint8 lnA[4][0x20], lnB[4][0x20];
    alignas(16) uint8 colModelB[0x30];                  // CColModel especial falso
    alignas(16) uint8 misc[0x200];
};
inline G3World gw;

// Orculo determinista de los dobles (los dos lados reciben exactamente la misma secuencia)
inline std::array<uint32,128> orc{}; inline size_t orcN{};
inline uint32 Orc(){ return orc[(orcN++)%orc.size()]; }
inline void OrcFill(Ctx& c){ for(auto& x:orc)x=c.next(); orcN=0; }

// Estado del script de los dobles (se reinicia en G3Begin)
inline void* player_vehicle{}; inline void* player_ped{};
inline V3 lastBar{};
inline void* fxCreateResult[2]{};

// ---------------------------------------------------------------------------------------------------------------------
// Direcciones de globales del EXE
// ---------------------------------------------------------------------------------------------------------------------
constexpr uintptr_t A_TIMESTEP=0xB7CB5C, A_TIMEMS=0xB7CB84, A_FRAMECTR=0xB7CB4C, A_SCANCODE=0xB7CD78, A_IGNOREENT=0xB7CD68,
    A_PLAYERFOCUS=0xB7CD74, A_PLAYERS=0xB7CD98, PLAYER_SIZE=0x190, A_GAMESTATE=0x96A8B0, A_SKIPSTATE=0x96A8A0, A_NEWSHELI=0xB7CB8C,
    A_AMBULANCES=0x9690A8, A_TEMPOBJS=0xBB4A70, A_SCRATCH=0xC1C218, A_STATICMAT=0xC1C220, A_HYDRAULIC=0xC1CB60, A_SPECIALCOL=0xC1CC78,
    A_CTRLFLAGS=0xC1CC00, A_REPEAT=0xB992B8, A_POOLS=0xB74484, A_NASTY=0xB9B7EC, A_MODELIDX=0x8CD6E4, A_HARVSTREAM=0x8E4B9D,
    A_USEMOUSE3RD=0xB6EC2E, A_CAMBASE=0xB6F028, A_NEWMOUSE=0xB73418;

constexpr size_t PAD_NEW=0, PAD_OLD=0x30;

// ---------------------------------------------------------------------------------------------------------------------
// Dobles. Todos: if(!Mine()) return <neutro>; traza con Push(TG(...), ...). NUNCA se parchean las funciones probadas.
// ---------------------------------------------------------------------------------------------------------------------
// --- CPhysical ---
inline void __fastcall S3_ApplyMoveForce(void* self,void*,V3 f){ if(!Mine())return; Push(TG("AMF"),self,f.x,f.y,f.z); }
inline void __fastcall S3_ApplyTurnForce(void* self,void*,V3 f,V3 p){ if(!Mine())return; Push(TG("ATF"),self,f.x,f.y,f.z); Push(TG("ATF2"),p.x,p.y,p.z); }
inline void __fastcall S3_ApplyForce(void* self,void*,V3 f,V3 p,bool upd){ if(!Mine())return; Push(TG("AF"),self,f.x,f.y,f.z,upd); Push(TG("AF2"),p.x,p.y,p.z); }

// --- CWorld ---
inline void __cdecl S3_FindObjectsInRange(const V3* pt,float r,bool b2d,int16* cnt,int16 maxc,void** out,bool bB,bool bV,bool bP,bool bO,bool bD){
    if(!Mine())return;
    Push(TG("FOIR"),pt->x,pt->y,pt->z,r,b2d); Push(TG("FOIR2"),maxc,bB,bV,bP,bO,bD);
    const uint32 n=Orc()%5; int16 k=0;
    for(uint32 i=0;i<n&&k<maxc;++i)out[k++]=Cr(int(Orc()%4));
    *cnt=k; Push(TG("FOIR3"),k);
}
inline bool __cdecl S3_ProcessLineOfSight(const V3* a,const V3* b,uint8* cp,void** ent,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7,bool b8){
    if(!Mine())return false;
    Push(TG("PLOS"),a->x,a->y,a->z,b->x,b->y,b->z); Push(TG("PLOS2"),b1,b2,b3,b4,b5,b6); Push(TG("PLOS3"),b7,b8,Val(AtA<uint32>(A_IGNOREENT)),*ent);
    if(Orc()%3==0)return false;
    At<float>(cp,0)=b->x+(float(Orc()%2000)-1000.f)*0.01f; At<float>(cp,4)=b->y+(float(Orc()%2000)-1000.f)*0.01f; At<float>(cp,8)=b->z+(float(Orc()%2000)-1000.f)*0.01f;
    *ent= (Orc()%2) ? Cr(1) : nullptr;
    return true;
}
inline bool __cdecl S3_ProcessVerticalLine(const V3* o,float dist,uint8* cp,void** ent,bool bB,bool bV,bool bP,bool bO,bool bD,bool bS,void* poly){
    if(!Mine())return false;
    Push(TG("PVL"),o->x,o->y,o->z,dist,bB); Push(TG("PVL2"),bV,bP,bO,bD,bS,poly);
    const uint32 r=Orc(); const bool up=dist>o->z;
    if(r%10 >= (up ? 4u : 7u)) return false;
    *ent=Cr(1+int(Orc()%3));   // la entidad se escribe ANTES que el colPoint (el EXE la lee sin inicializar si hay impacto)
    At<float>(cp,0)=o->x; At<float>(cp,4)=o->y; At<float>(cp,8)=o->z+(float(Orc()%6001)-3000.f)*0.001f;
    At<uint8>(cp,0x25)=uint8(Orc());
    return true;
}
inline void __cdecl S3_WorldAdd(void* e){ if(!Mine())return; Push(TG("WAdd"),e); }
inline void __cdecl S3_ClearScanCodes(){ if(!Mine())return; Push(TG("ClrScan")); }

// --- CCamera / CVehicle ---
inline void __fastcall S3_CamShake(void* self,void*,float s,V3 from){ if(!Mine())return; Push(TG("CamShake"),self,s,from.x,from.y,from.z); }
inline void __fastcall S3_KillPedsInVehicle(void* self,void*){ if(!Mine())return; Push(TG("KillPeds"),self); }
inline void __fastcall S3_KillPedsGettingIn(void* self,void*){ if(!Mine())return; Push(TG("KillPedsGI"),self); }
inline void __fastcall S3_ChangeLaw(void* self,void*,bool b){ if(!Mine())return; Push(TG("Law"),self,b); }
inline void __fastcall S3_DelayedExpl(void* self,void*){ if(!Mine())return; Push(TG("DelayedExpl"),self); }
inline bool __fastcall S3_GetSpecialColModel(void* self,void*){ if(!Mine())return false; const bool r=(Orc()%8)!=0; Push(TG("SpecialCol"),self,r); return r; }
inline void __fastcall S3_RemoveUpgrade(void* self,void*,int32 up){ if(!Mine())return; Push(TG("RemUpg"),self,up); }

// --- varios estaticos ---
inline void __cdecl S3_Darkel(void* veh,int32 pl){ if(!Mine())return; Push(TG("Darkel"),veh,pl); }
inline void* __fastcall S3_StartFire(void* self,void*,void* target,void* creator,float size,uint8 b,uint32 time,int8 gens){ if(!Mine())return nullptr; Push(TG("StartFire"),self,target,creator,size,b,time); Push(TG("StartFire2"),gens); return nullptr; }
inline bool __cdecl S3_AddExplosion(void* victim,void* creator,int32 type,V3 pos,uint32 time,bool snd,float shake,bool invis){
    if(!Mine())return false;
    Push(TG("Expl"),victim,creator,type,pos.x,pos.y,pos.z); Push(TG("Expl2"),time,snd,shake,invis); return true; }
inline void __cdecl S3_IncrementStat(int32 st,float v){ if(!Mine())return; Push(TG("Stat"),st,v); }
inline void __cdecl S3_SetClumpFlag(void* clump,int32 flag){ if(!Mine())return; Push(TG("ClumpFlag"),clump,flag); }
inline void __cdecl S3_ReportCrime(int32 type,void* victim,void* ped){ if(!Mine())return; Push(TG("Crime"),type,victim,ped); }
inline void __fastcall S3_SetWantedNoDrop(void* self,void*,int32 lvl){ if(!Mine())return; Push(TG("WantedNoDrop"),self,lvl); }
inline void __fastcall S3_KeepAreaClear(void* self,void*){ if(!Mine())return; Push(TG("KeepClear"),self); }
inline int16 __fastcall S3_Say(void* self,void*,int32 ctx,uint32 delay,float prob,bool b1,bool b2,bool b3){ if(!Mine())return 0; Push(TG("Say"),self,ctx,delay,prob,b1,b2); Push(TG("Say2"),b3); return 1; }
inline bool __cdecl S3_Allowed(void* ped,V3 dir,float d){ if(!Mine())return true; const bool r=(Orc()%3)!=0; Push(TG("Allowed"),ped,dir.x,dir.y,dir.z,d); Push(TG("Allowed2"),r); return r; }
inline void* __fastcall S3_FindTask(void* self,void*,int32 type){ if(!Mine())return nullptr; const bool r=(Orc()%5)==0; Push(TG("FindTask"),self,type,r); return r ? gw.task[0] : nullptr; }

// --- armas / audio / fx ---
inline void* __fastcall S3_FireInstantHit(void* self,void*,void* owner,V3* origin,V3* dest,void* a4,void* a5,void* a6,bool b7,bool b8){
    if(!Mine())return nullptr;
    Push(TG("FIH"),At<uint32>(self,0),At<uint32>(self,4),At<uint32>(self,8),At<uint32>(self,0xC),owner,origin->x); Push(TG("FIH2"),origin->y,origin->z,dest->x,dest->y,dest->z); Push(TG("FIH3"),a4,a5,a6,b7,b8);
    return nullptr; }
inline void __fastcall S3_AddGunshell(void* self,void*,void* owner,const V3* pos,const float* dir,float sz){ if(!Mine())return; Push(TG("Gunshell"),owner,pos->x,pos->y,pos->z,dir[0],dir[1]); Push(TG("Gunshell2"),sz,At<uint32>(self,0)); }
inline void __cdecl S3_TankAim(void* veh,void* driver,V3* start,V3* end){ if(!Mine())return; Push(TG("TankAim"),veh,driver,start->x,start->y,start->z); Push(TG("TankAim2"),end->x,end->y,end->z); end->x+=0.25f; end->z-=0.125f; }
inline void __fastcall S3_ReportWeaponEvent(void* self,void*,int32 ev,int32 wt,void* phys){ if(!Mine())return; Push(TG("WeapEv"),self,ev,wt,phys); }
inline void __fastcall S3_TriggerTankFire(void* self,void*,const V3* o,const V3* t){ if(!Mine())return; Push(TG("TankFire"),self,o->x,o->y,o->z); Push(TG("TankFire2"),t->x,t->y,t->z); }
inline void __cdecl S3_WaterCannon(uint32 veh,V3* s,V3* e){ if(!Mine())return; Push(TG("WCannon"),veh,s->x,s->y,s->z); Push(TG("WCannon2"),e->x,e->y,e->z); }
inline void __fastcall S3_AudioEvent(void* self,void*,int32 ev,float p){ if(!Mine())return; Push(TG("AudEv"),self,ev,p); }
inline void __fastcall S3_GetWheelPosn(void* self,void*,int32 wheel,V3* out,bool local){
    if(!Mine())return;
    out->x=((wheel&2)?-0.9f:0.9f)+float(Orc()%100)*0.001f; out->y=((wheel&1)?-1.4f:1.3f)+float(Orc()%100)*0.001f; out->z=-0.35f+float(Orc()%200)*0.001f;
    Push(TG("WheelPosn"),self,wheel,local,out->x,out->y,out->z); }
inline bool __cdecl S3_TestSphereBox(const float* sph,const float* box){
    if(!Mine())return false; const bool r=(Orc()%4)==0;
    Push(TG("SphBox"),sph[0],sph[1],sph[2],sph[3],r); Push(TG("SphBox2"),box[0],box[1],box[2],box[3],box[4],box[5]); return r; }
inline void __fastcall S3_FxPlay(void* self,void*){ if(!Mine())return; Push(TG("FxPlay"),self); }
inline void __fastcall S3_FxKill(void* self,void*){ if(!Mine())return; Push(TG("FxKill"),self); }
inline void __fastcall S3_FxSetVelAdd(void* self,void*,const V3* v){ if(!Mine())return; Push(TG("FxVel"),self,v->x,v->y,v->z); }
inline void __fastcall S3_FxAddParticle(void* self,void*,const V3* pos,const V3* vel,float t,const float* mult,float rot,float lm,float lml,bool loc){
    if(!Mine())return;
    Push(TG("FxAdd"),self,pos->x,pos->y,pos->z,vel->x,vel->y); Push(TG("FxAdd2"),vel->z,t,rot,lm,lml,loc);
    Push(TG("FxMult"),mult[0],mult[1],mult[2],mult[3],mult[4],mult[5]); Push(TG("FxMult2"),mult[6]); }
inline void* __fastcall S3_FxCreate(void* self,void*,const char* name,const V3* pos,void* mat,bool b){
    if(!Mine())return nullptr;
    const uint32 r=Orc(); void* res= (r%5==0) ? nullptr : gw.fx[(r>>4)%4];
    Push(TG("FxCreate"),self,name?uint32(uint8(name[5])):0u,pos->x,pos->y,pos->z); Push(TG("FxCreate2"),mat,b,res); return res; }
inline void __fastcall S3_RegisterReference(void* self,void*,void** ref){ if(!Mine())return; Push(TG("RegRef"),self,ref,*ref); }
inline void __fastcall S3_CleanUpOldReference(void* self,void*,void** ref){ if(!Mine())return; Push(TG("CleanRef"),self,ref,*ref); }
inline void* __fastcall S3_GroupAdd(void* self,void*,void* ev,bool valid){ if(!Mine())return nullptr; Push(TG("GroupAdd"),self,At<uint32>(ev,0)==0x85B440u,Val(At<uint32>(ev,0x14)),valid); return nullptr; }
inline void* __cdecl S3_GetGlobalGroup(){ if(!Mine())return nullptr; Push(TG("GetGroup")); return gw.group; }
inline void __fastcall S3_ObjectDamage(void* self,void*,float dmg,const V3* o,const V3* d,void* damager,int32 wt){ if(!Mine())return; Push(TG("ObjDmg"),self,dmg,o,d,damager,wt); }

// --- RW / dobles del propio CAutomobile ---
inline void* __cdecl S3_FrameGetLTM(uint8* f){ if(!Mine())return nullptr; Push(TG("LTM"),f); return f ? f+0x50 : nullptr; }
inline void* __cdecl S3_FrameForAllObjects(void* frame,void* cb,void* data){
    if(!Mine())return frame;
    Push(TG("ForAll"),frame,cb?1u:0u,data);
    if(frame&&cb)reinterpret_cast<void*(__cdecl*)(void*,void*)>(cb)(w.at[0],data);
    return frame; }
inline void __fastcall S3_SetBumperDamage(void* self,void*,int32 b,bool x){ if(!Mine())return; Push(TG("Bumper"),self,b,x); }
inline void __fastcall S3_SetDoorDamage(void* self,void*,int32 d,bool x){ if(!Mine())return; Push(TG("DoorDmg"),self,d,x); }
inline void* __fastcall S3_SpawnFlying(void* self,void*,int32 node,uint32 col){ if(!Mine())return nullptr; Push(TG("Spawn"),self,node,col); return nullptr; }
inline void __fastcall S3_DoNitro(void* self,void*,float power){ if(!Mine())return; Push(TG("Nitro"),self,power); }

// --- FindPlayer* (traza o silencio) ---
inline void* __cdecl S3_FPV_T(int32 pid,bool remote){ if(!Mine())return nullptr; Push(TG("FPV"),pid,remote); return player_vehicle; }
inline void* __cdecl S3_FPV_Q(int32,bool){ if(!Mine())return nullptr; return player_vehicle; }
inline void* __cdecl S3_FPP_T(int32 pid){ if(!Mine())return nullptr; Push(TG("FPP"),pid); return player_ped; }
inline void* __cdecl S3_FPP_Q(int32){ if(!Mine())return nullptr; return player_ped; }

// --- dobles que van en la vtable falsa de coches / peatones / objetos / tareas ---
inline void __fastcall S3_VBlowUpCar(void* self,void*,void* dmgr,bool hide){ if(!Mine())return; Push(TG("VBlowUp"),self,dmgr,hide); }
inline bool __fastcall S3_VIsDoorMissing(void* self,void*,int32 d){ if(!Mine())return false; const bool r=(Orc()%5)==0; Push(TG("DoorMissing"),self,d,r); return r; }
inline bool __fastcall S3_VIsDoorClosed(void* self,void*,int32 d){ if(!Mine())return false; const bool r=(Orc()%3)==0; Push(TG("DoorClosed"),self,d,r); return r; }
inline void __fastcall S3_VOpenDoor(void* self,void*,void* ped,int32 comp,int32 door,float ratio,bool quick){ if(!Mine())return; Push(TG("OpenDoor"),self,ped,comp,door,ratio,quick); }
inline float __fastcall S3_VGetHeightAboveRoad(void* self,void*){ if(!Mine())return 0.f; const float h=0.2f+float(Orc()%1000)*0.001f; Push(TG("HeightRoad"),self,h); return h; }
inline bool __fastcall S3_VGetTowBarPos(void* self,void*,V3* out,bool b,void* veh){
    if(!Mine())return false; const bool r=(Orc()%5)!=0;
    if(r){ const float* p=reinterpret_cast<const float*>(reinterpret_cast<uint8*>(w.mat[0])+0x30); out->x=p[0]+(float(Orc()%400)-200.f)*0.01f; out->y=p[1]+(float(Orc()%400)-200.f)*0.01f; out->z=p[2]+(float(Orc()%400)-200.f)*0.01f; lastBar=*out; }
    Push(TG("TowBar"),self,b,veh,r); if(r)Push(TG("TowBar2"),out->x,out->y,out->z); return r; }
inline bool __fastcall S3_VGetTowHitchPos(void* self,void*,V3* out,bool b,void* tractor){
    if(!Mine())return false; const bool r=(Orc()%5)!=0;
    if(r){ const uint32 k=Orc()%8; const float dx=(k<5)?(float(Orc()%160)-80.f)*0.005f:(float(Orc()%400)-200.f)*0.01f, dy=(float(Orc()%160)-80.f)*0.005f, dz=(k<5)?(float(Orc()%240)-120.f)*0.01f:(float(Orc()%100)-50.f)*0.04f;
        out->x=lastBar.x+dx; out->y=lastBar.y+dy; out->z=lastBar.z+dz; }
    Push(TG("TowHitch"),self,b,tractor,r); if(r)Push(TG("TowHitch2"),out->x,out->y,out->z); return r; }
inline bool __fastcall S3_VSetTowLink(void* self,void*,void* tractor,bool b){ if(!Mine())return false; Push(TG("SetTowLink"),self,tractor,b); return true; }
inline void __fastcall S3_PedFlagDestroy(void* self,void*){ if(!Mine())return; Push(TG("PedDestroy"),self); }
inline void* __fastcall S3_TaskGetSubTask(void* self,void*){ if(!Mine())return nullptr; return nullptr; }
inline bool __fastcall S3_TaskIsSimple(void* self,void*){ return true; }
inline int32 __fastcall S3_TaskGetType(void* self,void*){ return int32(At<uint32>(self,4)); }
inline void __fastcall S3_Trap(void* self,void*){ if(Mine())Push(TG("TRAP"),self); }

// ---------------------------------------------------------------------------------------------------------------------
// Registro de dobles (se llama desde StubsG3)
// ---------------------------------------------------------------------------------------------------------------------
inline std::string Register(){
    std::string miss;
    auto H=[&](uint64 bit,uintptr_t exe,const char* cls,const char* name,void* fn){ if(!AddHookStub(3,bit,exe,cls,name,fn))miss+=std::string(cls)+"::"+name+" "; };
    auto E=[&](uint64 bit,uintptr_t exe,void* fn){ AddStub(3,bit,exe,0,fn); };
    H(B_AMF,0x5429F0,"CPhysical","ApplyMoveForce-vec",FnPtr(&S3_ApplyMoveForce));
    H(B_ATF,0x542A50,"CPhysical","ApplyTurnForce",FnPtr(&S3_ApplyTurnForce));
    H(B_AF,0x542B50,"CPhysical","ApplyForce",FnPtr(&S3_ApplyForce));
    H(B_FOIR,0x564A20,"CWorld","FindObjectsInRange",FnPtr(&S3_FindObjectsInRange));
    H(B_PLOS,0x56BA00,"CWorld","ProcessLineOfSight",FnPtr(&S3_ProcessLineOfSight));
    H(B_PVL,0x5674E0,"CWorld","ProcessVerticalLine",FnPtr(&S3_ProcessVerticalLine));
    H(B_WADD,0x563220,"CWorld","Add",FnPtr(&S3_WorldAdd));
    H(B_CLRSCAN,0x563470,"CWorld","ClearScanCodes",FnPtr(&S3_ClearScanCodes));
    H(B_CAMSHAKE,0x50A9F0,"CCamera","CamShake",FnPtr(&S3_CamShake));
    H(B_KILLP,0x6D1C80,"CVehicle","KillPedsInVehicle",FnPtr(&S3_KillPedsInVehicle));
    H(B_KILLPG,0x6D82F0,"CVehicle","KillPedsGettingInVehicle",FnPtr(&S3_KillPedsGettingIn));
    H(B_LAW,0x6D2330,"CVehicle","ChangeLawEnforcerState",FnPtr(&S3_ChangeLaw));
    H(B_DELEX,0x6D1340,"CVehicle","ProcessDelayedExplosion",FnPtr(&S3_DelayedExpl));
    H(B_SPECIAL,0x6DF3D0,"CVehicle","GetSpecialColModel",FnPtr(&S3_GetSpecialColModel));
    H(B_REMUPG,0x6D3630,"CVehicle","RemoveUpgrade",FnPtr(&S3_RemoveUpgrade));
    H(B_DARKEL,0x43DF20,"CDarkel","RegisterCarBlownUpByPlayer",FnPtr(&S3_Darkel));
    H(B_STARTFIRE,0x53A050,"CFireManager","StartFire-",FnPtr(&S3_StartFire));
    H(B_ADDEXPL,0x736A50,"CExplosion","AddExplosion",FnPtr(&S3_AddExplosion));
    H(B_INCSTAT,0x55C180,"CStats","IncrementStat",FnPtr(&S3_IncrementStat));
    H(B_SETCLUMP,0x7322F0,"CVisibilityPlugins","SetClumpForAllAtomicsFlag",FnPtr(&S3_SetClumpFlag));
    H(B_CRIME,0x532010,"CCrime","ReportCrime",FnPtr(&S3_ReportCrime));
    H(B_SETWANT,0x609F30,"CPlayerPed","SetWantedLevelNoDrop",FnPtr(&S3_SetWantedNoDrop));
    AddStub(3,B_KEEPCLR,0x60C1E0,bit_cast<uintptr_t>(static_cast<void(CPlayerPed::*)()>(&CPlayerPed::KeepAreaAroundPlayerClear)),FnPtr(&S3_KeepAreaClear));
    H(B_SAY,0x5EFFE0,"CPed","Say",FnPtr(&S3_Say));
    H(B_ALLOWED,0x441E10,"CGameLogic","IsPlayerAllowedToGoInThisDirection",FnPtr(&S3_Allowed));
    H(B_FINDTASK,0x681740,"CTaskManager","FindActiveTaskByType",FnPtr(&S3_FindTask));
    H(B_FIREHIT,0x73FB10,"CWeapon","FireInstantHit",FnPtr(&S3_FireInstantHit));
    H(B_GUNSHELL,0x73A3E0,"CWeapon","AddGunshell",FnPtr(&S3_AddGunshell));
    H(B_TANKAIM,0x73D1E0,"CWeapon","DoTankDoomAiming",FnPtr(&S3_TankAim));
    H(B_WEAPEV,0x506F40,"CAudioEngine","ReportWeaponEvent",FnPtr(&S3_ReportWeaponEvent));
    AddStub(3,B_TANKFIRE,0x4A0FA0,bit_cast<uintptr_t>(static_cast<void(Fx_c::*)(const CVector&,const CVector&)>(&Fx_c::TriggerTankFire)),FnPtr(&S3_TriggerTankFire));
    H(B_WCANNON,0x728CB0,"CWaterCannons","UpdateOne",FnPtr(&S3_WaterCannon));
    H(B_AUDEV,0x4F6420,"CAEVehicleAudioEntity","AddAudioEvent-0",FnPtr(&S3_AudioEvent));
    H(B_WHEELPOSN,0x4C7D20,"CVehicleModelInfo","GetWheelPosn",FnPtr(&S3_GetWheelPosn));
    H(B_SPHBOX,0x4120C0,"CCollision","TestSphereBox",FnPtr(&S3_TestSphereBox));
    H(B_FXPLAY,0x4AA2F0,"FxSystem_c","Play",FnPtr(&S3_FxPlay));
    H(B_FXKILL,0x4AA3F0,"FxSystem_c","Kill",FnPtr(&S3_FxKill));
    H(B_FXVEL,0x4AA730,"FxSystem_c","SetVelAdd",FnPtr(&S3_FxSetVelAdd));
    H(B_FXADD,0x4AA440,"FxSystem_c","AddParticle-v3d",FnPtr(&S3_FxAddParticle));
    H(B_FXCREATE,0x4A9BE0,"FxManager_c","CreateFxSystem-4",FnPtr(&S3_FxCreate));
    H(B_REGREF,0x571B70,"CEntity","RegisterReference-",FnPtr(&S3_RegisterReference));
    H(B_CLEANREF,0x571A00,"CEntity","CleanUpOldReference-",FnPtr(&S3_CleanUpOldReference));
    H(B_GROUPADD,0x4AB420,"CEventGroup","Add-",FnPtr(&S3_GroupAdd));
    H(B_GETGROUP,0x4ABA50,"RwHelper","GetEventGlobalGroup",FnPtr(&S3_GetGlobalGroup));
    H(B_OBJDMG,0x5A0D90,"CObject","ObjectDamage",FnPtr(&S3_ObjectDamage));
    E(B_LTM,0x7F0990,FnPtr(&S3_FrameGetLTM));
    E(B_FORALL,0x7F1200,FnPtr(&S3_FrameForAllObjects));
    H(B_BUMPER,0x6B1350,"CAutomobile","SetBumperDamage",FnPtr(&S3_SetBumperDamage));
    H(B_DOOR,0x6B1600,"CAutomobile","SetDoorDamage",FnPtr(&S3_SetDoorDamage));
    H(B_SPAWN,0x6A8580,"CAutomobile","SpawnFlyingComponent",FnPtr(&S3_SpawnFlying));
    AddStub(3,B_NITRO,0x6A3BD0,bit_cast<uintptr_t>(static_cast<void(CAutomobile::*)(float)>(&CAutomobile::DoNitroEffect)),FnPtr(&S3_DoNitro));
    H(B_FPV_T,0x56E0D0,"CWorld","FindPlayerVehicle",FnPtr(&S3_FPV_T));
    H(B_FPV_Q,0x56E0D0,"CWorld","FindPlayerVehicle",FnPtr(&S3_FPV_Q));
    H(B_FPP_T,0x56E210,"CWorld","FindPlayerPed",FnPtr(&S3_FPP_T));
    H(B_FPP_Q,0x56E210,"CWorld","FindPlayerPed",FnPtr(&S3_FPP_Q));
    return miss;
}

} // namespace g3
} // namespace AutomobileFinal4Live
