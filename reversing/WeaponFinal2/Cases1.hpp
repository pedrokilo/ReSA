#pragma once
// Casos 1: CWeapon::ProcessLineOfSight (0x73B300), DoWeaponEffect (0x73E690), FireOneInstantHitRound (0x73AF00) y GenerateDamageEvent (0x73A530).
#include "Doubles.hpp"

namespace WeaponFinal2Live {

inline float QNaN(){ return std::bit_cast<float>(0x7FC00000u); }
// CColPoint de la plantilla gCpTpl (0x2C bytes): punto cerca de 'around', normal casi unitaria, superficie B (0x23), pieza B (0x24), iluminacion B (0x25)
inline void FillCpTpl(Rng& r,const CVector& around,uint32 surfMax=60){
    uint8* c=gCpTpl.data(); FillBytes(c,gCpTpl.size(),r);
    At<float>(c,0)=around.x+r.unit(-3,3); At<float>(c,4)=around.y+r.unit(-3,3); At<float>(c,8)=around.z+r.unit(-3,3);
    At<float>(c,0xC)=r.unit(-1,1);
    CVector n=RV(r,1.0f); float m=std::sqrt((n.x*n.x+n.y*n.y)+n.z*n.z); if(!(m>0.05f)){ n=CVector{0,0,1}; m=1.0f; }
    At<float>(c,0x10)=n.x/m; At<float>(c,0x14)=n.y/m; At<float>(c,0x18)=n.z/m; At<float>(c,0x1C)=r.unit(-1,1);
    At<uint8>(c,0x23)=uint8(r.below(surfMax)); At<uint8>(c,0x24)=uint8(r.pickI({0,1,5,9,9,13,14,15,16,int64_t(r.below(20))}));
    At<float>(c,0x28)=r.unit(0,2); }

// =============== ProcessLineOfSight ===============
inline json CaseProcLos(Rng& r){
    gLosCalls=0; gLosPct=uint32(r.pickI({0,30,60,100}));
    MakeEnt(r,0,3,0); MakeEnt(r,1,2,0); MakeEnt(r,2,4,0); gLosEnt[0]=Ent(0); gLosEnt[1]=Ent(1); gLosEnt[2]=Ent(2); gLosEnt[3]=Ent(0);
    std::memset(fCpOut.data(),0xCD,fCpOut.size()); CVector s=RV(r,100), e=RV(r,100); FillCpTpl(r,s);
    void* hitOut=reinterpret_cast<void*>(uintptr_t(0xCDCDCDCDu));
    const uint32 wt=uint32(r.pickI({0,22,28,34,38,46,47,int64_t(r.next())}));
    void* arg5= r.pct(50) ? Ent(1) : nullptr;
    uint32 f[7]; for(auto& x:f)x=r.below(2);
    Patches P; P.Add(0x712E40,S_HandleGunShot); P.Add(D.handleGun,S_HandleGunShot); P.Add(0x707550,S_OilOnFire); P.Add(D.oil,S_OilOnFire); P.Add(0x56BA00,S_Los); P.Add(D.los,S_Los);
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(const CVector*,const CVector*,void*,void**,uint32,void*,uint32,uint32,uint32,uint32,uint32,uint32,uint32)>(0x73B300)(&s,&e,fCpOut.data(),&hitOut,wt,arg5,f[0],f[1],f[2],f[3],f[4],f[5],f[6])&0xFF;
    json o=json::object(); o["al"]=al; o["hit"]=Name(U(hitOut)); o["cp"]=Fnv(fCpOut.data(),0x2C); o["s"]=V3(s); o["e"]=V3(e); return o; }

// =============== DoWeaponEffect ===============
inline void* gFxRet{}; inline uint32 gMatCalls{};
inline void __fastcall S_FxMat(void* self,void*,void* out,const CVector* o,const CVector* d){ if(Mine())T("create_mat_from_vec",out,o->x,o->y,o->z,d->x,d->y,d->z); (void)self; }   // 0x49E950 ret 0xC
inline void* __cdecl S_RwMatCreate(){ if(!Mine())return fMat[0].data(); void* m=fMat[(gMatCalls++)%fMat.size()].data(); T("rw_matrix_create",m); return m; }                 // 0x7F2A50
inline uint32 __cdecl S_RwMatDestroy(void* m){ if(Mine())T("rw_matrix_destroy",m); return 1; }                                                                                   // 0x7F2A20 cdecl, devuelve RwBool
inline void* __fastcall S_CreateFx(void* self,void*,const char* name,const CVector* pt,void* mat,uint32 flag){                                                                  // 0x4A9BE0 ret 0x10
    if(!Mine())return nullptr; std::string n; for(int k=0;k<24&&name[k];++k)n.push_back(name[k]);
    trace.push_back(json::array({"create_fx",n,pt ? V3(*pt) : json(nullptr),Name(U(mat)),flag&0xFF})); (void)self; return gFxRet; }
inline void __fastcall S_FxKill(void* self,void*){ if(Mine())T("fx_kill",self); }
inline void __fastcall S_FxCopy(void* self,void*){ if(Mine())T("fx_copy_parent_matrix",self); }
inline void __fastcall S_FxPlay(void* self,void*){ if(Mine())T("fx_play",self); }
inline void __fastcall S_FxPlayKill(void* self,void*){ if(Mine())T("fx_play_and_kill",self); }
inline void __fastcall S_FxMust(void* self,void*,uint32 on){ if(Mine())T("fx_must_create_prts",self,on&0xFF); }
inline void __fastcall S_FxConst(void* self,void*,uint32 on,float t){ if(Mine())T("fx_set_const_time",self,on&0xFF,t); }
inline void __fastcall S_FxSetMat(void* self,void*,void* mat){ if(Mine())T("fx_set_matrix",self,mat); }
inline void AddFxStubs(Patches& P){
    P.Add(0x7F2A50,S_RwMatCreate); P.Add(D.matCreate,S_RwMatCreate); P.Add(0x7F2A20,S_RwMatDestroy); P.Add(D.matDestroy,S_RwMatDestroy);
    P.Add(0x49E950,S_FxMat); P.Add(D.createMat,S_FxMat); P.Add(0x4A9BE0,S_CreateFx); P.Add(D.createFx,S_CreateFx);
    P.Add(0x4AA3F0,S_FxKill); P.Add(D.fxKill,S_FxKill); P.Add(0x4AA890,S_FxCopy); P.Add(D.fxCopy,S_FxCopy); P.Add(0x4AA2F0,S_FxPlay); P.Add(D.fxPlay,S_FxPlay);
    P.Add(0x4AAC70,S_FxMust); P.Add(D.fxMust,S_FxMust); P.Add(0x4AA6C0,S_FxConst); P.Add(D.fxConst,S_FxConst); P.Add(0x4AA630,S_FxSetMat); P.Add(D.fxSetMat,S_FxSetMat);
    P.Add(0x4AA3D0,S_FxPlayKill); P.Add(D.playKill,S_FxPlayKill); }
inline json CaseDoWeaponEffect(Rng& r){
    gMatCalls=0;
    const int32 type=r.pickI({37,37,41,41,42,42,18,18,0,22,24,38,40,int64_t(ValidType(r))});
    FillWeapon(r,type); gFxRet= r.pct(60) ? static_cast<void*>(fFx.data()) : nullptr;
    const CVector o=RV(r,50), d=RV(r,1.0f);
    Patches P; AddFxStubs(P);
    reinterpret_cast<void(__thiscall*)(CWeapon*,float,float,float,float,float,float)>(0x73E690)(W(),o.x,o.y,o.z,d.x,d.y,d.z);
    return WeaponOut(); }

// =============== FireOneInstantHitRound ===============
inline bool __cdecl S_GenDamageStub(void* victim,void* creator,uint32 type,int32 dmg,uint32 piece,uint32 dir){      // 0x73A530 cdecl (6 argumentos)
    if(!Mine())return false; const bool v=Chance(60); T("generate_damage_event",victim,creator,type,dmg,piece,dir&0xFF,uint8(v)); return v; }
inline json CaseFireOne(Rng& r){
    ResetAnim(); gAssocNull=false; gLosCalls=0; gLosPct=uint32(r.pickI({0,40,80,100})); gDirSalt=r.next(); gStats.fill(0.0f);
    uint8* ped=MakePed(r,1,int(r.pickI({2,4,5,6})),r.pct(75)); At<int32>(ped,0x530)=r.pickI({0,0,0,1,2,54,55,17});
    MakeVeh(r,2,r.pct(80)); MakeEnt(r,3,1,0); MakeEnt(r,4,4,0,r.pct(70)); MakeEnt(r,5,5,0);
    const int kind=int(r.pickI({1,1,1,2,2,3,4,5})); gLosEnt[0]=gLosEnt[1]=gLosEnt[2]=gLosEnt[3]=Ent(size_t(kind==1 ? 1 : kind==2 ? 2 : kind==3 ? 3 : kind==4 ? 4 : 5));
    CVector s=RV(r,40), e=RV(r,40); if(r.pct(4))e=s;
    FillCpTpl(r,e);
    const int32 inten=r.pickI({0,1,-1,5,25,100,1000,-1000,int64_t(int32(r.next())),int64_t(r.below(200))});
    Patches P;
    P.Add(0x7000E0,S_AddLight); P.Add(D.addLight,S_AddLight); P.Add(0x56BA00,S_Los); P.Add(D.los,S_Los); P.Add(0x723750,S_AddTrace1); P.Add(D.addTrace1,S_AddTrace1);
    P.Add(0x5DEF60,S_LocalDir); P.Add(D.localDir,S_LocalDir); P.Add(0x4D3AA0,S_AddAnim); P.Add(D.addAnim,S_AddAnim);
    P.Add(0x73A530,S_GenDamageStub); P.Add(D.genDamage,S_GenDamageStub); P.Add(0x6D7C90,S_Inflict); P.Add(D.inflict,S_Inflict);
    P.Add(0x506EC0,S_BulletHit); P.Add(D.bulletHit,S_BulletHit); P.Add(0x6EB690,S_Water); P.Add(D.waterLevel,S_Water);
    reinterpret_cast<void(__cdecl*)(const CVector*,const CVector*,int32)>(0x73AF00)(&s,&e,inten);
    json o=json::object(); o["assoc"]=Fnv(fAssoc[0].data(),fAssoc.size()*0x40); o["s"]=V3(s); o["e"]=V3(e); return o; }

// =============== GenerateDamageEvent ===============
// Dobles de la funcion: calculadora de respuesta, AffectsPed (ranura 5 de la vtable de CEventDamage), calculo de animacion, localizacion,
// tareas del gestor, animaciones. Los constructores/destructores de CPedDamageResponseCalculator y CEventDamage se ejecutan DE VERDAD.
inline void __fastcall S_CalcResp(void* self,void*,void* ped,void* resp,uint32 speak){                                    // 0x4B5AC0 ret 0xC
    if(!Mine())return; auto* c=static_cast<CPedDamageResponseCalculator*>(self);
    T("compute_damage_response",c->m_pDamager,Bits(c->m_fDamageFactor),uint32(c->m_bodyPart),uint32(c->m_weaponType),uint8(c->m_bSpeak),ped,speak&0xFF);
    uint8* b=static_cast<uint8*>(resp); At<float>(b,0)=gRng ? gRng->unit(0,100) : 0.0f; At<float>(b,4)=gRng ? gRng->unit(0,50) : 0.0f;
    b[8]=uint8(Chance(30)); b[9]=uint8(Chance(20)); b[10]=uint8(Chance(60)); b[11]=uint8(Chance(10)); }
inline bool __fastcall S_Affects(void* self,void*,void* ped){ if(!Mine())return false; const bool v=Chance(75); trace.push_back(json::array({"affects_ped",EvSummary(self),Name(U(ped)),uint8(v)})); return v; }   // 0x4B35A0 ret 4
inline void SetEvAnim(void* self){
    uint8* b=static_cast<uint8*>(self);
    At<int32>(b,0x28)=gRng ? gRng->pickI({0,1,3,0}) : 0; At<int32>(b,0x2C)=gRng ? gRng->pickI({28,29,30,31,191,192,5,100,28,31,191,30}) : 0;
    At<float>(b,0x30)=gRng ? gRng->pickF({8.0f,4.0f,1.0f,gRng->unit(0.5f,9.0f)}) : 0.0f; At<float>(b,0x34)=gRng ? gRng->pickF({1.0f,0.5f,2.0f,gRng->unit(0.1f,3.0f)}) : 0.0f; }
inline void __fastcall S_CompDeath(void* self,void*,void* ped,uint32 ab){ if(!Mine())return; SetEvAnim(self); trace.push_back(json::array({"compute_death_anim",Name(U(ped)),ab&0xFF,EvSummary(self)})); }   // 0x4B3A60 ret 8
inline void __fastcall S_CompDamage(void* self,void*,void* ped,uint32 ab){ if(!Mine())return; SetEvAnim(self); trace.push_back(json::array({"compute_damage_anim",Name(U(ped)),ab&0xFF,EvSummary(self)})); }   // 0x4B3FC0 ret 8
inline bool __cdecl S_LocBlood(){ if(!Mine())return false; const bool v=Chance(85); T("localisation_blood",uint8(v)); return v; }       // 0x56D230
inline bool __cdecl S_LocKick(){ if(!Mine())return false; const bool v=Chance(85); T("localisation_kicking_when_down",uint8(v)); return v; }   // 0x56D270
inline void AddGenDamageStubs(Patches& P){
    P.Add(0x532010,S_ReportCrime); P.Add(D.reportCrime,S_ReportCrime); P.Add(0x4B5AC0,S_CalcResp); P.Add(D.calcResp,S_CalcResp); P.Add(0x4B35A0,S_Affects); P.Add(D.affects,S_Affects);
    P.Add(0x4B3A60,S_CompDeath); P.Add(D.computeDeath,S_CompDeath); P.Add(0x4B3FC0,S_CompDamage); P.Add(D.computeDamage,S_CompDamage);
    P.Add(0x56D230,S_LocBlood); P.Add(D.locBlood,S_LocBlood); P.Add(0x56D270,S_LocKick); P.Add(D.locKick,S_LocKick);
    P.Add(0x6819D0,S_Simplest); P.Add(D.simplest,S_Simplest); P.Add(0x681720,S_Active); P.Add(D.active,S_Active);
    P.Add(0x4D6A70,S_FirstAssoc); P.Add(D.firstAssoc,S_FirstAssoc); P.Add(0x4D4610,S_BlendAnim); P.Add(D.blendAnim,S_BlendAnim); P.Add(0x4D3AA0,S_AddAnim); P.Add(D.addAnim,S_AddAnim);
    P.Add(0x4D68B0,S_GetAssoc); P.Add(D.getAssoc,S_GetAssoc); P.Add(0x4CEA80,S_SetTime); P.Add(D.setTime,S_SetTime); P.Add(0x4AB420,S_GroupAdd); P.Add(D.groupAdd,S_GroupAdd); }
inline float HealthPick(Rng& r){ return r.pickF({100.0f,10.0f,1.0f,0.5f,0.0f,0.0f,-1.0f,-100.0f,-0.0f,QNaN()}); }
inline json CaseGenDamage(Rng& r){
    ResetAnim(); ResetTasks(); gAssocNull=r.pct(25); gDirSalt=r.next(); gStats.fill(0.0f);
    uint8* v=MakePed(r,0,int(r.pickI({2,4,5,0,1})),r.pct(85));
    At<int32>(v,0x530)=r.pickI({0,0,0,0,1,2,17,54,55}); At<float>(v,0x540)=HealthPick(r);
    At<uint32>(v,0x46C)=r.next(); if(r.pct(25))At<uint32>(v,0x46C)|=0x100u; else At<uint32>(v,0x46C)&=~0x100u;
    // creador: 0 nulo, 1 ped no jugador, 2 ped jugador, 3 vehiculo, 4 objeto
    const int ck=int(r.pickI({0,1,1,2,2,3,4,4})); void* creator=nullptr;
    if(ck==1||ck==2)creator=MakePed(r,1,ck==2 ? 0 : int(r.pickI({2,4,6})),r.pct(80)); else if(ck==3)creator=MakeVeh(r,1,true); else if(ck==4)creator=MakeEnt(r,1,4,0,r.pct(70));
    MakePed(r,2,0,true);                                   // otro jugador
    SetPlayerSwap( ck==2 && r.pct(80) ? creator : r.pct(75) ? Ent(2) : nullptr );
    // tareas (victima: tarea mas simple y tarea activa; creador: tarea activa)
    gTmSimplest[0]= r.pct(10) ? nullptr : MakeTask(0,r.pickI({0xDA,0xDA,0xDA,5,100}));
    gTmActive[0]= r.pct(30) ? nullptr : MakeTask(1,r.pickI({0x3FE,0x3FE,5,0xDA,300}));
    gTmActive[1]= r.pct(25) ? nullptr : MakeTask(2,r.pickI({0x403,0x403,5,7}));
    int32 type;
    const uint32 tp=r.below(100); if(tp<35)type=int32(r.below(16)); else if(tp<45)type=54; else if(tp<50)type=23; else type=ValidType(r);
    const int32 dmg=r.pickI({0,1,10,25,100,-1,-50,int64_t(int32(r.next())),int64_t(r.below(1000))});
    const uint32 piece=uint32(r.pickI({0,1,2,3,4,5,6,7,8,9,10,int64_t(r.below(256))}));
    const uint32 dir=uint32(r.pickI({0,1,2,3,3,2,int64_t(r.below(256))}));
    Patches P; AddGenDamageStubs(P);
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(void*,void*,uint32,int32,uint32,uint32)>(0x73A530)(v,creator,uint32(type),dmg,piece,dir)&0xFF;
    json o=json::object(); o["al"]=al; o["assoc"]=Fnv(fAssoc[0].data(),fAssoc.size()*0x40); o["refs"]=RefState(); o["victim"]=Fnv(v,0x600); return o; }
}
