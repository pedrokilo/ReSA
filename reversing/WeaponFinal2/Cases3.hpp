#pragma once
// Casos 3: CWeapon::FireInstantHitFromCar2 (0x73CBA0), FireSniper (0x73AAC0) y FireProjectile (0x741360).
// Los eventos (CEventGunShot, CEventGunShotWhizzedBy) se construyen y destruyen de verdad sobre entidades falsas con la lista de
// referencias falsa; CEventGroup::Add (0x4AB420) y GetEventGlobalGroup (0x4ABA50) se doblan.
#include "Cases2.hpp"

namespace WeaponFinal2Live {

inline void __fastcall S_DoBulletImpact(void* self,void*,void* firedBy,void* victim,const CVector* s,const CVector* e,void* cp,uint32 inc){   // 0x73B550 ret 0x18
    if(Mine())T("do_bullet_impact",self,firedBy,victim,s->x,s->y,s->z,e->x,e->y,e->z,victim ? Fnv(cp,0x2C) : uint64(0),inc); }   // colPoint solo definido si hubo impacto
inline void AddEventStubs(Patches& P){
    P.Add(0x532010,S_ReportCrime); P.Add(D.reportCrime,S_ReportCrime); P.Add(0x4ABA50,S_GlobalGroup); P.Add(D.globalGroup,S_GlobalGroup);
    P.Add(0x4AB420,S_GroupAdd); P.Add(D.groupAdd,S_GroupAdd); P.Add(0x602590,S_Interesting); P.Add(D.interesting,S_Interesting); }

// =============== FireInstantHitFromCar2 ===============
inline json CaseFireHit2(Rng& r){
    ResetTasks(); SetTimes(r); gLosCalls=0; gLosPct=uint32(r.pickI({0,50,100}));
    const int32 type= r.pct(35) ? int32(r.pickI({23,17,23})) : ValidType(r); FillWeapon(r,type);
    void* veh= r.pct(90) ? static_cast<void*>(MakeVeh(r,1,true)) : nullptr;
    void* owner=nullptr; const int ok=int(r.pickI({0,1,1,2}));
    if(ok==1)owner=MakePed(r,0,int(r.pickI({2,4,6})),true); else if(ok==2)owner=MakePed(r,0,int(r.pickI({0,1})),true);
    MakePed(r,2,0,true); SetPlayerSwap( r.pct(70) ? Ent(2) : nullptr );
    MakePed(r,3,4,true); MakeVeh(r,4,true); MakeEnt(r,5,4,0); MakeEnt(r,6,1,0);
    gLosEnt[0]=Ent(size_t(3+r.below(4))); gLosEnt[1]=gLosEnt[2]=gLosEnt[3]=gLosEnt[0];
    CVector s=RV(r,40), e=RV(r,40); FillCpTpl(r,s);
    Patches P; AddEventStubs(P);
    P.Add(0x7000E0,S_AddLight); P.Add(D.addLight,S_AddLight); P.Add(0x712E40,S_HandleGunShot); P.Add(D.handleGun,S_HandleGunShot); P.Add(0x707550,S_OilOnFire); P.Add(D.oil,S_OilOnFire);
    P.Add(0x56BA00,S_Los); P.Add(D.los,S_Los); P.Add(0x73B550,S_DoBulletImpact); P.Add(D.doBulletImpact,S_DoBulletImpact);
    reinterpret_cast<void(__thiscall*)(CWeapon*,float,float,float,float,float,float,void*,void*)>(0x73CBA0)(W(),s.x,s.y,s.z,e.x,e.y,e.z,veh,owner);
    json o=WeaponOut(); o["refs"]=RefState();
    o["glob"]=json::array({Name(AtA<uint32>(kIgnoreEntity)),AtA<uint8>(kIncludeBikers),AtA<uint8>(kIncludeCarTyres),AtA<uint8>(kIncludeDeadPeds),Bits(AtA<float>(kSpreadRate))}); return o; }

// =============== FireSniper ===============
inline json CaseFireSniper(Rng& r){
    ResetTasks(); SetTimes(r);
    const int32 type= r.pct(40) ? int32(r.pickI({23,17,34})) : ValidType(r); FillWeapon(r,type);
    const int sk=int(r.pickI({0,0,0,1,1,2}));                                       // 0 jugador 1 ped 2 vehiculo
    uint8* sh=nullptr;
    if(sk==2){ sh=MakeVeh(r,0,true); At<int32>(sh,0x598)=7; MakePed(r,1,2,true); At<void*>(sh,0x460)= r.pct(70) ? Ent(1) : nullptr; }
    else sh=MakePed(r,0,sk==0 ? int(r.pickI({0,1})) : int(r.pickI({2,4,6})),r.pct(85));
    MakePed(r,2,0,true); SetPlayerSwap( sk==0 ? (r.pct(85) ? static_cast<void*>(sh) : Ent(2)) : (r.pct(50) ? Ent(2) : nullptr) );
    AtA<uint8>(kActiveCamIdx)=uint8(r.below(3));
    AtA<uint16>(CamF(kCamMode))=uint16(r.pickI({34,7,46,8,51,42,39,40,52,0,45,5}));
    CVector f= r.pct(35) ? CVector{r.unit(-0.01f,0.01f),-0.9894f+r.unit(-0.004f,0.004f),0.145f+r.unit(-0.004f,0.004f)} : RV(r,1.0f); if(r.pct(3))f=CVector{0,0,0};
    AtA<float>(CamF(kCamFront))=f.x; AtA<float>(CamF(kCamFront)+4)=f.y; AtA<float>(CamF(kCamFront)+8)=f.z;
    const CVector src=RV(r,60); AtA<float>(CamF(kCamSource))=src.x; AtA<float>(CamF(kCamSource)+4)=src.y; AtA<float>(CamF(kCamSource)+8)=src.z;
    AtA<uint32>(kMoonSize)=uint32(r.pickI({0,3,7,8,100,-1}));
    CVector tgt=RV(r,30); void* victim= r.pct(50) ? Ent(3) : nullptr; MakeEnt(r,3,3,0);
    Patches P; AddEventStubs(P);
    P.Add(0x736010,S_AddBullet); P.Add(D.addBullet,S_AddBullet); P.Add(0x53F9A0,S_Shake); P.Add(D.shake,S_Shake); P.Add(0x50A970,S_CamShake); P.Add(D.camShake,S_CamShake);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*,void*,void*,CVector*)>(0x73AAC0)(W(),sh,victim,&tgt)&0xFF;
    json o=WeaponOut(); o["al"]=al; o["moon"]=AtA<uint32>(kMoonSize); o["refs"]=RefState(); o["tgt"]=V3(tgt); return o; }

// =============== FireProjectile ===============
inline void* gPickRet{}; inline uint32 gClearPct{60}; inline void* gSphereRet{};
inline void* __cdecl S_Pick(float ox,float oy,float oz,float dx,float dy,float dz,float tol,void* ignore,uint32 planes,void* pref){      // 0x73F910 cdecl (2 CVector por valor, ...)
    if(!Mine())return nullptr; T("pick_target",ox,oy,oz,dx,dy,dz,tol,ignore,planes&0xFF,pref); return gPickRet; }
inline bool __cdecl S_Locked(uint32 id){ if(!Mine())return false; const bool v=Chance(65); T("is_locked_on",id,uint8(v)); return v; }       // 0x742BD0
inline bool __cdecl S_LosClear(const CVector* o,const CVector* t,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e,uint32 f,uint32 g){          // 0x56A490 cdecl (9 argumentos)
    if(!Mine())return false; const bool v=Chance(gClearPct);
    T("is_line_of_sight_clear",o->x,o->y,o->z,t->x,t->y,t->z,a&0xFF,b&0xFF,c&0xFF,d&0xFF,e&0xFF,f&0xFF,g&0xFF,Name(AtA<uint32>(kIgnoreEntity)),uint8(v)); return v; }
inline void* __cdecl S_TestSphere(float cx,float cy,float cz,float rad,void* ignore,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e,uint32 f){   // 0x569E20 cdecl
    if(!Mine())return nullptr; const bool v=Chance(50); T("test_sphere_against_world",cx,cy,cz,rad,ignore,a&0xFF,b&0xFF,c&0xFF,d&0xFF,e&0xFF,f&0xFF,uint8(v)); return v ? gSphereRet : nullptr; }
inline bool __cdecl S_AddProj(void* creator,uint32 type,float px,float py,float pz,float force,const CVector* dir,void* victim){             // 0x737C80 cdecl
    if(!Mine())return false; trace.push_back(json::array({"add_projectile",Name(U(creator)),type,Bits(px),Bits(py),Bits(pz),Bits(force),dir ? V3(*dir) : json(nullptr),Name(U(victim))})); return true; }
inline void __cdecl S_RemoveNot(void* creator,uint32 type,float px,float py,float pz){ if(Mine())T("remove_not_add",creator,type,px,py,pz); }   // 0x737C00 cdecl
inline json CaseFireProj(Rng& r){
    ResetTasks(); SetTimes(r); gLosCalls=0; gClearPct=uint32(r.pickI({0,30,60,100}));
    const uint32 tp=r.below(100);
    const int32 type= tp<30 ? 35 : tp<55 ? 36 : tp<70 ? int32(r.pickI({16,39})) : tp<80 ? int32(r.pickI({17,18})) : ValidType(r);
    FillWeapon(r,type);
    const int sk=int(r.pickI({0,0,0,0,1,1,1,2}));                                   // 0 jugador 1 ped 2 vehiculo (siempre con matriz: sin ella el EXE reserva una matriz REAL)
    uint8* fb=nullptr;
    if(sk==2){ fb=MakeVeh(r,0,true); At<int32>(fb,0x598)=7; MakePed(r,1,2,true); At<void*>(fb,0x460)= r.pct(70) ? Ent(1) : nullptr; }
    else fb=MakePed(r,0,sk==0 ? int(r.pickI({0,1})) : int(r.pickI({2,4,6})),true);
    const uint32 now=AtA<uint32>(kTimeMs);
    MakePed(r,3,4,true); MakePed(r,4,6,true); MakePed(r,5,2,r.pct(70)); MakeVeh(r,6,r.pct(70)); MakeEnt(r,7,4,0,r.pct(70)); MakeEnt(r,8,1,0); MakeEnt(r,9,3,0);
    if(sk==0){ uint8* pd=fPData[0].data();
        At<uint32>(pd,0x94)=uint32(r.pickI({0,0,int64_t(now-1501u),int64_t(now-1500u),int64_t(now-1499u),int64_t(now-100u),int64_t(now-5000u),int64_t(now+10u)}));
        At<void*>(pd,0x98)= r.pct(60) ? Ent(3) : nullptr; }
    const uint32 pk=r.below(4); gPickRet= pk==0 ? nullptr : pk==1 ? (sk==0 ? At<void*>(fPData[0].data(),0x98) : Ent(3)) : pk==2 ? Ent(4) : Ent(3);
    gSphereRet= r.pct(50) ? Ent(5) : nullptr;
    AtA<uint8>(kActiveCamIdx)=uint8(r.below(3)); AtA<uint16>(CamF(kCamMode))=uint16(r.pickI({34,7,8,51,42,39,40,52,0,46,45}));
    const CVector f=RV(r,1.0f), src=RV(r,60);
    AtA<float>(CamF(kCamFront))=f.x; AtA<float>(CamF(kCamFront)+4)=f.y; AtA<float>(CamF(kCamFront)+8)=f.z;
    AtA<float>(CamF(kCamSource))=src.x; AtA<float>(CamF(kCamSource)+4)=src.y; AtA<float>(CamF(kCamSource)+8)=src.z;
    // cruceta 0 (gCrossHair): bytes 0xC8A84C..4E (RGB), 0xC8A85C (rotacion), 0xC8A83C (tiempo)
    FillBytes(reinterpret_cast<void*>(kCrossHair),0x2C,r);
    AtA<uint32>(kIgnoreEntity)=U(Ent(9));
    CVector origin=RV(r,40), tp2=RV(r,40);
    void* targetEnt=nullptr; const uint32 te=r.below(5); if(te==1)targetEnt=Ent(5); else if(te==2)targetEnt=Ent(6); else if(te==3)targetEnt=Ent(7);
    const CVector* targetPos= r.pct(40) ? &tp2 : nullptr;
    const float force=r.pickF({0.0f,1.0f,2.5f,10.0f,r.unit(0,20)});
    Patches P; AddEventStubs(P);
    P.Add(0x742BD0,S_Locked); P.Add(D.isLocked,S_Locked); P.Add(0x73F910,S_Pick); P.Add(D.pickTarget,S_Pick); P.Add(0x56A490,S_LosClear); P.Add(D.losClear,S_LosClear);
    P.Add(0x569E20,S_TestSphere); P.Add(D.testSphere,S_TestSphere); P.Add(0x737C80,S_AddProj); P.Add(D.addProj,S_AddProj); P.Add(0x737C00,S_RemoveNot); P.Add(D.removeNotAdd,S_RemoveNot);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*,void*,CVector*,void*,const CVector*,float)>(0x741360)(W(),fb,&origin,targetEnt,targetPos,force)&0xFF;
    json o=WeaponOut(); o["al"]=al; o["origin"]=V3(origin); o["cross"]=Fnv(reinterpret_cast<void*>(kCrossHair),0x2C); o["ignore"]=Name(AtA<uint32>(kIgnoreEntity)); o["refs"]=RefState(); return o; }
}
