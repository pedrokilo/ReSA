#pragma once
// Casos de CWeapon, segunda parte: Fire (despacho), FireAreaEffect, FireM16_1stPerson y Update.
#include "WeaponCases.hpp"

namespace PickWeap1Live {

inline uint32 gBoolBits{}, gBoolCalls{};
inline bool NextBool(){ return (gBoolBits>>(gBoolCalls++&31u))&1u; }
inline json JV3(const CVector* v){ return v ? V3(*v) : json(nullptr); }

// ---------------- dobles comunes de disparo ----------------
inline bool __fastcall S_FireInstantHit(void*,void*,void* firing,CVector* origin,CVector* muzzle,void* tgtEnt,CVector* tgt,CVector* alt,uint32 a6,uint32 muz){
    if(!Mine())return false; const bool v=NextBool(); trace.push_back(json::array({"fire_instant_hit",Name(U(firing)),JV3(origin),JV3(muzzle),Name(U(tgtEnt)),JV3(tgt),JV3(alt),a6&0xFF,muz&0xFF,v})); return v; }
inline bool __fastcall S_FireProjectile(void*,void*,void* firedBy,CVector* origin,void* tgtEnt,CVector* tgt,float force){
    if(!Mine())return false; const bool v=NextBool(); trace.push_back(json::array({"fire_projectile",Name(U(firedBy)),JV3(origin),Name(U(tgtEnt)),JV3(tgt),Bits(force),v})); return v; }
inline bool __fastcall S_FireM16(void*,void*,void* owner){ if(!Mine())return false; const bool v=NextBool(); T("fire_m16",owner,uint8(v)); return v; }
inline bool __fastcall S_FireSniper(void*,void*,void* shooter,void* victim,CVector* target){ if(!Mine())return false; const bool v=NextBool(); trace.push_back(json::array({"fire_sniper",Name(U(shooter)),Name(U(victim)),JV3(target),v})); return v; }
inline bool __fastcall S_FireArea(void*,void*,void* firing,CVector* origin,void* tgtEnt,CVector* tgt){
    if(!Mine())return false; const bool v=NextBool(); trace.push_back(json::array({"fire_area",Name(U(firing)),JV3(origin),Name(U(tgtEnt)),JV3(tgt),v})); return v; }
inline bool __fastcall S_Photo(void*,void*,void* owner,CVector* point){ if(!Mine())return false; const bool v=NextBool(); trace.push_back(json::array({"take_photograph",Name(U(owner)),JV3(point),v})); return v; }
inline void __cdecl S_UseDetonator(void* ped){ if(Mine())T("use_detonator",ped); }
inline int16 __fastcall S_Say(void* self,void*,uint32 ctx,uint32 delay,float prob,uint32 a,uint32 b,uint32 c){ if(!Mine())return 0; T("say",self,ctx,delay,prob,a&0xFF,b&0xFF,c&0xFF); return 1; }
inline uint32 __fastcall S_GiveWeapon(void* self,void*,uint32 type,uint32 ammo,uint32 flag){ if(!Mine())return 0; T("give_weapon",self,type,ammo,flag&0xFF); return type%13u; }
inline uint32 __fastcall S_GetWeaponSlot(void*,void*,uint32 type){ if(!Mine())return 0; return (type*5u+3u)%13u; }
inline void __fastcall S_SetCurrentWeapon(void* self,void*,uint32 type){ if(Mine())T("set_current_weapon",self,type); }
inline void __fastcall S_PadClear(void* self,void*,uint32 a,uint32 b){ if(Mine())T("pad_clear",U(self),a&0xFF,b&0xFF); }
inline void __fastcall S_PadShake(void* self,void*,uint32 time,uint32 freq,uint32 unk){ if(Mine())T("pad_shake",U(self),time&0xFFFF,freq&0xFF,unk&0xFF); }
inline void* gActiveTask{};
inline void* __fastcall S_GetActiveTask(void* self,void*){ if(!Mine())return nullptr; T("get_active_task",self); return gActiveTask; }
inline void __cdecl S_AddShot(void* creator,uint32 type,float ox,float oy,float oz,float tx,float ty,float tz){ if(Mine())T("add_shot",Name(U(creator)),type,ox,oy,oz,tx,ty,tz); }
inline void __fastcall S_DoWeaponEffect(void*,void*,float ox,float oy,float oz,float dx,float dy,float dz){ if(Mine())T("do_weapon_effect",ox,oy,oz,dx,dy,dz); }
inline bool __cdecl S_StartFire(float x,float y,float z,uint32 a,uint32 b,uint32 c,float d){ if(!Mine())return false; const bool v=NextBool(); T("start_fire",x,y,z,a&0xFF,b&0xFF,c&0xFF,d,uint8(v)); return v; }
inline void __cdecl S_ReportCrime(uint32 type,void* ent,void* ped){ if(Mine())T("report_crime",type,Name(U(ent)),Name(U(ped))); }
inline void __fastcall S_Find3rd(void*,void*,float range,float ox,float oy,float oz,CVector* camPos,CVector* camTarget){
    if(!Mine())return; T("find_3rd_person",range,ox,oy,oz); *camPos=CVector{ox+1.5f,oy-2.0f,oz+0.25f}; *camTarget=CVector{ox+range*0.5f,oy+range*0.25f,oz-range*0.125f}; }
inline uint32 gMouseCam{};
inline bool __fastcall S_MouseCam(void*,void*){ if(!Mine())return false; T("using_3rd_person_mouse_cam"); return gMouseCam&1u; }
inline CVector* __fastcall S_BonePos(void* self,void*,CVector* out,uint32 bone,uint32 upd){ if(!Mine())return out; T("bone_position",self,bone,upd&0xFF); *out=CVector{At<float>(self,4)+0.25f,At<float>(self,8)-0.5f,At<float>(self,0xC)+1.25f}; return out; }
inline void __cdecl S_HandleGunShot(const CVector* a,const CVector* b){ if(Mine())T("birds_gunshot",a->x,a->y,a->z,b->x,b->y,b->z); }
inline void __cdecl S_OilOnFire(const CVector* a,const CVector* b){ if(Mine())T("oil_on_fire",a->x,a->y,a->z,b->x,b->y,b->z); }
inline void* gShotVictim{}; inline uint32 gCheckOccupantChange{};
inline void __cdecl S_CheckOccupant(void** pEnt,void* cp,uint32 type,const CVector* a,const CVector* b){
    if(!Mine())return; T("check_shooting_occupant",Name(U(*pEnt)),type,a->x,a->y,a->z,b->x,b->y,b->z); (void)cp; if(gCheckOccupantChange&1u)*pEnt=nullptr; }
inline float gRangeMult{};
inline float __cdecl S_RangeMult(void* target,void* owner){ if(!Mine())return 1.0f; T("range_multiplier",Name(U(target)),Name(U(owner))); return gRangeMult; }
inline void __fastcall S_DoBulletImpact(void*,void*,void* firedBy,void* victim,const CVector* s,const CVector* e,void* cp,uint32 inc){
    if(Mine())T("do_bullet_impact",Name(U(firedBy)),Name(U(victim)),s->x,s->y,s->z,e->x,e->y,e->z,victim ? Fnv(cp,0x1C) : uint64(0),inc); }   // colPoint: solo pos/normal si hay victima (el resto es basura de pila)

// ---------------- peds falsos con campos de arma ----------------
alignas(16) inline std::array<uint8,0x400> wIntel{};
alignas(16) inline std::array<uint8,0x200> wPData{};
inline void SetupWeaponPed(Rng& r,size_t k,int pedType,bool player){
    uint8* p=static_cast<uint8*>(MakeEnt(r,k,3,0));
    std::memset(wIntel.data(),0,wIntel.size()); std::memset(wPData.data(),0,wPData.size());
    At<void*>(p,0x47C)=wIntel.data(); At<void*>(p,0x480)=wPData.data(); At<int32>(p,0x598)=pedType;
    At<uint32>(p,0x46C)=r.next()&~0x4000100u&~0x10000u; if(r.pct(25))At<uint32>(p,0x46C)|=0x100u; if(r.pct(25))At<uint32>(p,0x46C)|=0x4000000u;
    At<float>(wPData.data(),0x2C)=r.unit(0,40.0f); At<float>(wPData.data(),0x54)=r.pickF({0.0f,0.3f,-0.3f,1.2f,r.unit(-1.4f,1.4f)});
    for(int s=0;s<13;++s){ uint8* w=p+0x5A0+size_t(s)*0x1C; At<int32>(w,0)=r.pickI({0,22,39,40}); At<uint32>(w,4)=r.below(5); At<uint32>(w,0xC)=uint32(r.pickI({0,1,2,50}));  }
    (void)player; }
inline void SetPlayerSwap(void* ped){ AtA<void*>(kPlayers)=ped; AtA<uint8>(kPlayerInFocus)=0; }

// ---------------- Fire ----------------
inline json CaseFire(Rng& r){
    SetTimes(r); gSkillSalt=r.next(); gBoolBits=r.next(); gBoolCalls=0; gStats.fill(0.0f);
    const int32 type=r.pickI({0,5,15,16,17,18,19,20,21,22,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,int64_t(r.below(47))});
    FillWeapon(r,type); At<uint32>(wbuf.data(),4)=uint32(r.pickI({0,0,0,1,1,2,3,4}));
    At<uint32>(wbuf.data(),8)=uint32(r.pickI({0,0,1,2,17,100})); At<uint32>(wbuf.data(),0xC)=uint32(r.pickI({0,1,2,30,24999,25000,100000,int64_t(int32(r.next()))}));
    const bool vehicleShooter= (type!=39&&type!=40&&type!=43) && r.pct(8);
    const int pedType=int(r.pickI({0,0,1,2,4,5,7}));
    SetupWeaponPed(r,0,pedType,true);
    /* el vehiculo va en su propio hueco: en el 0 borraba el ped del jugador (m_pPlayerData=0) y el EXE leia [0x480+0x2C] en 0x742725 */
    void* firedBy= vehicleShooter ? MakeEnt(r,4,2,0) : Ent(0);
    SetPlayerSwap( r.pct(55) ? Ent(0) : Ent(1) );
    MakeEnt(r,2,r.pickI({3,2}),0); MakeEnt(r,3,3,0);
    AtA<uint8>(kPlayers+0x14D)=uint8(r.pct(40)); AtA<uint8>(kInfiniteAmmo)=uint8(r.pct(15)); gPct=r.pickF({0.0f,60.0f,99.9f,100.0f});
    gActiveTask= r.pct(70) ? static_cast<void*>(wTask[0].data()) : nullptr; At<void*>(wTask[0].data(),0)=taskVt.data(); At<uint32>(wTask[0].data(),8)=uint32(r.pickI({1022,1022,1023,3,200}));
    AtA<uint16>(kPlayerWeaponMode)=uint16(r.pickI({0,7,0x22,0x2D,5,46})); AtA<uint8>(kActiveCamIdx)=uint8(r.below(3));
    AtA<uint16>(kCamMode+uintptr_t(AtA<uint8>(kActiveCamIdx))*kCamStride)=uint16(r.pickI({46,0,7,49}));
    CVector sp=RV(r,40), bp=RV(r,40), tp=RV(r,40), ap=RV(r,40);
    CVector* startPosn= r.pct(70) ? &sp : nullptr; CVector* barrel= r.pct(80) ? &bp : nullptr; if(startPosn&&!barrel)barrel=&bp;
    void* targetEnt= r.pct(30) ? Ent(r.pct(50) ? 2 : 3) : nullptr; CVector* targetPosn= r.pct(30) ? &tp : nullptr; CVector* alt= r.pct(40) ? &ap : nullptr;
    if(!firedBy)firedBy=Ent(0);
    Patches P;
    P.Add(0x5E3B60,S_WeaponSkill); P.Add(D.getWeaponSkill,S_WeaponSkill);
    P.Add(0x73FB10,S_FireInstantHit); P.Add(D.fireInstantHit,S_FireInstantHit); P.Add(0x741360,S_FireProjectile); P.Add(D.fireProjectile,S_FireProjectile);
    P.Add(0x741C00,S_FireM16); P.Add(D.fireM16,S_FireM16); P.Add(0x73AAC0,S_FireSniper); P.Add(D.fireSniper,S_FireSniper);
    P.Add(0x73E800,S_FireArea); P.Add(D.fireArea,S_FireArea); P.Add(0x73C1F0,S_Photo); P.Add(D.takePhoto,S_Photo);
    P.Add(0x5660B0,S_UseDetonator); P.Add(D.useDetonator,S_UseDetonator); P.Add(0x4E69F0,S_AddAudioEvent); P.Add(D.addAudioEvent,S_AddAudioEvent);
    P.Add(0x5EFFE0,S_Say); P.Add(D.say,S_Say); P.Add(0x5E6080,S_GiveWeapon); P.Add(D.giveWeapon,S_GiveWeapon); P.Add(0x5DF200,S_GetWeaponSlot); P.Add(D.getWeaponSlot,S_GetWeaponSlot);
    P.Add(0x5E6280,S_SetCurrentWeapon); P.Add(D.setCurrentWeapon,S_SetCurrentWeapon); P.Add(0x541A70,S_PadClear); P.Add(D.padClear,S_PadClear);
    P.Add(0x681720,S_GetActiveTask); P.Add(D.getActiveTask,S_GetActiveTask); P.Add(0x5591E0,S_Pct); P.Add(D.pctProgress,S_Pct); P.Add(0x55C180,S_IncStat); P.Add(D.incStat,S_IncStat);
    const uint32 sp0=Bits(sp.x);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*,void*,CVector*,CVector*,void*,CVector*,CVector*)>(0x742300)(W(),firedBy,startPosn,barrel,targetEnt,targetPosn,alt)&0xFF;
    json o=WeaponOut(); o["al"]=uint32(al!=0); /* el EXE devuelve bl crudo (0x7429FB mov al,bl; en el default 0x742794 bl = byte bajo de targetPosn) */ o["type"]=type; o["sp"]=V3(sp); o["bp"]=V3(bp); o["sp0"]=sp0;
    o["ped"]=Fnv(Ent(0),0x600); o["veh"]=Fnv(firedBy,0x60); o["weapons"]=Fnv(static_cast<uint8*>(Ent(0))+0x5A0,13*0x1C); return o; }

// ---------------- FireAreaEffect ----------------
inline json CaseFireArea(Rng& r){
    gBoolBits=r.next(); gBoolCalls=0; gMouseCam=r.next(); gStats.fill(0.0f);
    FillWeapon(r,r.pickI({37,37,41,42,22,0})); SetTimes(r);
    const bool pedShooter=r.pct(75); SetupWeaponPed(r,0,int(r.pickI({0,1,2})),true);
    void* firing= pedShooter ? Ent(0) : MakeEnt(r,0,2,0,r.pct(70));
    if(pedShooter&&r.pct(30)){ wEnt[0][0x14]=0; wEnt[0][0x15]=0; wEnt[0][0x16]=0; wEnt[0][0x17]=0; }          // sin matriz: heading del placeable
    if(pedShooter&&r.pct(15))At<void*>(firing,0x480)=nullptr;
    SetPlayerSwap( r.pct(60) ? firing : Ent(1) ); MakeEnt(r,1,3,0); MakeEnt(r,2,r.pickI({3,2}),0); MakeEnt(r,3,3,0);
    CVector origin=RV(r,40), tp=RV(r,40);
    void* targetEnt= r.pct(30) ? Ent(r.pct(50) ? 2 : 3) : nullptr; CVector* target= r.pct(25) ? &tp : nullptr;
    Patches P; AddRandStubs(P);
    P.Add(0x739C30,S_AddShot); P.Add(D.addShot,S_AddShot); P.Add(0x73E690,S_DoWeaponEffect); P.Add(D.doWeaponEffect,S_DoWeaponEffect);
    P.Add(0x53A450,S_StartFire); P.Add(D.startFire,S_StartFire); P.Add(0x55C180,S_IncStat); P.Add(D.incStat,S_IncStat); P.Add(0x532010,S_ReportCrime); P.Add(D.reportCrime,S_ReportCrime);
    P.Add(0x514970,S_Find3rd); P.Add(D.find3rd,S_Find3rd); P.Add(0x50A850,S_MouseCam); P.Add(D.mouseCam,S_MouseCam); P.Add(0x5E4280,S_BonePos); P.Add(D.bonePos,S_BonePos);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*,void*,const CVector*,void*,CVector*)>(0x73E800)(W(),firing,&origin,targetEnt,target)&0xFF;
    json o=WeaponOut(); o["al"]=al; o["origin"]=V3(origin); return o; }

// ---------------- FireM16_1stPerson ----------------
alignas(16) inline std::array<uint8,0x60> m16ColPoint{};
inline bool __cdecl S_LosM16(const CVector* s,const CVector* e,void* cp,void** hit,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e2,uint32 f,uint32 g,uint32 h){
    if(!Mine())return false; const bool v=NextBool();
    T("los",s->x,s->y,s->z,e->x,e->y,e->z,a&0xFF,b&0xFF,c&0xFF,d&0xFF,e2&0xFF,f&0xFF,g&0xFF,h&0xFF,uint8(v));
    if(v){ auto* p=static_cast<uint8*>(cp); At<float>(p,0)=(s->x+e->x)*0.5f; At<float>(p,4)=(s->y+e->y)*0.5f; At<float>(p,8)=(s->z+e->z)*0.5f; At<uint32>(p,0xC)=0x11111111u; At<float>(p,0x10)=0.0f; At<float>(p,0x14)=1.0f; At<float>(p,0x18)=0.0f; *hit=gShotVictim; }
    else *hit=nullptr; return v; }
inline json CaseFireM16(Rng& r){
    SetTimes(r); gBoolBits=r.next(); gBoolCalls=0; gCheckOccupantChange=r.next(); gRangeMult=r.pickF({0.5f,1.0f,2.0f,3.0f,r.unit(0.1f,4.0f)});
    FillWeapon(r,r.pickI({22,24,28,29,30,31,34,35,38,int64_t(r.below(47))}));
    SetupCamera3D(r);
    AtA<uint16>(kCamMode+uintptr_t(AtA<uint8>(kActiveCamIdx))*kCamStride)=uint16(r.pickI({0x22,7,46,8,0x33,0x2A,0x27,0x28,0x34,0x2D,0,49,5}));
    AtA<float>(CamF(kCamVert))=r.unit(-1,1); AtA<float>(CamF(kCamHoriz))=r.unit(-3,3);
    SetupWeaponPed(r,0,int(r.pickI({0,1,2,4})),true); SetPlayerSwap( Ent(0) ); MakeEnt(r,2,r.pickI({3,2,4}),0);
    At<void*>(Ent(0),0xFC)= r.pct(25) ? Ent(3) : nullptr;
    gShotVictim= r.pct(70) ? Ent(2) : nullptr;
    Patches P; AddRandStubs(P);
    P.Add(0x712E40,S_HandleGunShot); P.Add(D.handleGunShot,S_HandleGunShot); P.Add(0x707550,S_OilOnFire); P.Add(D.oilOnFire,S_OilOnFire);
    P.Add(0x56BA00,S_LosM16); P.Add(D.losWorld,S_LosM16); P.Add(0x73F480,S_CheckOccupant); P.Add(D.checkOccupant,S_CheckOccupant);
    P.Add(0x73B380,S_RangeMult); P.Add(D.rangeMult,S_RangeMult); P.Add(0x73B550,S_DoBulletImpact); P.Add(D.doBulletImpact,S_DoBulletImpact);
    P.Add(0x53F920,S_PadShake); P.Add(D.padShake,S_PadShake);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*,void*)>(0x741C00)(W(),Ent(0))&0xFF;
    json o=WeaponOut(); o["al"]=al; o["angles"]=json::array({Bits(AtA<float>(CamF(kCamVert))),Bits(AtA<float>(CamF(kCamHoriz)))});
    o["world"]=json::array({AtA<uint8>(kIncludeBikers),AtA<uint8>(kIncludeCarTyres),AtA<uint8>(kIncludeDeadPeds),AtA<uint32>(kIgnoreEntity)}); return o; }

// ---------------- Update ----------------
alignas(16) inline std::array<uint8,0x40> wAssoc{};
alignas(16) inline std::array<uint8,0x40> wHier{};
inline uint32 gAssocMode{}, gAssocCalls{};
inline void* __cdecl S_GetAssoc(void* clump,int32 id){
    if(!Mine())return nullptr; void* res= ((gAssocMode>>(gAssocCalls&15u))&1u) ? static_cast<void*>(wAssoc.data()) : nullptr; ++gAssocCalls;
    T("get_association",clump,id,Name(U(res))); return res; }
inline void* gUseGunTask{};
inline void* __fastcall S_GetTaskUseGun(void* self,void*){ if(!Mine())return nullptr; T("get_task_use_gun",self); return gUseGunTask; }
alignas(16) inline std::array<uint8,0x40> wClump{};
inline json CaseUpdate(Rng& r){
    SetTimes(r); gSkillSalt=r.next();
    const int32 type=RandType(r); FillWeapon(r,type); At<uint32>(wbuf.data(),4)=uint32(r.pickI({0,1,1,2,2,3,4}));
    At<uint32>(wbuf.data(),0x10)=AtA<uint32>(kTimeMs)+uint32(r.pickI({-3000,-1000,-100,-1,0,1,50,500,1000,3000}));
    const bool hasOwner=r.pct(85);
    SetupWeaponPed(r,0,int(r.pickI({0,1,2,4})),true); SetPlayerSwap( r.pct(60) ? Ent(0) : Ent(1) );
    AtA<uint8>(kPlayers+0x14D)=uint8(r.pct(40));
    std::memset(wClump.data(),0,wClump.size()); wClump[0]=2; /* rpCLUMP: la DLL lo comprueba en GetRpClump */ At<void*>(Ent(0),0x18)=wClump.data();      // RpClump* falso (solo se pasa como identificador)
    std::memset(wAssoc.data(),0,wAssoc.size()); std::memset(wHier.data(),0,wHier.size());
    At<void*>(wAssoc.data(),0x14)=wHier.data(); At<float>(wAssoc.data(),0x20)=r.unit(0.0f,3.0f); At<float>(wAssoc.data(),0x28)=r.unit(0.0f,0.2f); At<float>(wHier.data(),0x10)=r.pickF({0.0f,1.0f,2.5f,r.unit(0.5f,4.0f)});
    gAssocMode=r.next(); gAssocCalls=0; gUseGunTask= r.pct(40) ? static_cast<void*>(wTask[1].data()) : nullptr;
    Patches P;
    P.Add(0x5E3B60,S_WeaponSkill); P.Add(D.getWeaponSkill,S_WeaponSkill); P.Add(0x4E69F0,S_AddAudioEvent); P.Add(D.addAudioEvent,S_AddAudioEvent);
    P.Add(0x4D68B0,S_GetAssoc); P.Add(D.getAssoc,S_GetAssoc); P.Add(0x600F70,S_GetTaskUseGun); P.Add(D.getTaskUseGun,S_GetTaskUseGun);
    P.Add(0x4AA3F0,S_FxKill); P.Add(D.fxKill,S_FxKill);
    reinterpret_cast<void(__thiscall*)(CWeapon*,void*)>(0x73DB40)(W(),hasOwner ? Ent(0) : nullptr);
    json o=WeaponOut(); o["assoc"]=Fnv(wAssoc.data(),0x40); return o; }
}
