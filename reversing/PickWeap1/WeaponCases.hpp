#pragma once
// Casos de CWeapon. El arma bajo prueba es un buffer de 0x1C bytes del banco (relleno con 0xCD: el relleno del 0x16..0x17 debe quedar
// intacto en ambos lados). Entidades, peds, vehiculos, camaras y pools son FALSOS o regiones globales guardadas/restauradas; los
// colaboradores externos (mundo, particulas, audio, estadisticas, eventos que registran referencias) se doblan con traza en la
// direccion del EXE y en el cuerpo de la DLL. Las funciones que construyen CEvent* sobre entidades (que llaman a RegisterReference
// y reservan en el pool REAL de referencias) NO se prueban (ver README).
#include "PickupCases.hpp"

namespace PickWeap1Live {

constexpr uintptr_t kActiveCamIdx=0xB6F081, kCamMode=0xB6F1A8, kCamFront=0xB6F32C, kCamSource=0xB6F338, kCamVert=0xB6F248, kCamHoriz=0xB6F258, kCamStride=0x238;
constexpr uintptr_t kPlayerWeaponMode=0xB6F858, kTimeStep=0xB7CB5C, kInfiniteAmmo=0x969178, kIgnoreEntity=0xB7CD68, kIncludeBikers=0xB7CD6F, kIncludeCarTyres=0xB7CD70, kIncludeDeadPeds=0xB7CD71;
constexpr uintptr_t kSpreadRate=0xB7CD64, kPelletCol=0xC8A7DC, kGunShellPtr=0xA9AE14, kVehiclePoolPtr=0xB74494, kFlags73A=0xC8A7C0, kCamJoyPtr=0xB6F19C;
inline uintptr_t CamF(uintptr_t first){ return first+uintptr_t(AtA<uint8>(kActiveCamIdx))*kCamStride; }

// ---------------- el arma ----------------
alignas(16) inline std::array<uint8,0x20> wbuf{};
inline CWeapon* W(){ return reinterpret_cast<CWeapon*>(wbuf.data()); }
alignas(16) inline std::array<uint8,0x40> fakeFx{};
inline json WeaponOut(){
    json o=json::object(); o["bytes"]=Fnv(wbuf.data(),0x1C); o["pad"]=Fnv(wbuf.data()+0x1C,4);
    o["f"]=json::array({At<int32>(wbuf.data(),0),At<uint32>(wbuf.data(),4),At<uint32>(wbuf.data(),8),At<uint32>(wbuf.data(),0xC),At<uint32>(wbuf.data(),0x10),At<uint8>(wbuf.data(),0x14),At<uint8>(wbuf.data(),0x15),Name(At<uint32>(wbuf.data(),0x18))});
    o["pad16"]=json::array({wbuf[0x16],wbuf[0x17]}); return o; }
inline void FillWeapon(Rng& r,int32 type){
    std::memset(wbuf.data(),0xCD,wbuf.size());
    At<int32>(wbuf.data(),0)=type; At<uint32>(wbuf.data(),4)=uint32(r.pickI({0,1,2,3,4}));
    At<uint32>(wbuf.data(),8)=uint32(r.pickI({0,0,1,7,17,100,int64_t(r.below(500))}));
    At<uint32>(wbuf.data(),0xC)=uint32(r.pickI({0,0,1,10,30,500,24999,25000,99999,int64_t(r.below(120000))}));
    At<uint32>(wbuf.data(),0x10)=r.next()>>r.below(5);
    At<uint8>(wbuf.data(),0x14)=uint8(r.below(2)); At<uint8>(wbuf.data(),0x15)=uint8(r.below(2));
    At<void*>(wbuf.data(),0x18)= r.pct(50) ? static_cast<void*>(fakeFx.data()) : nullptr; }
inline int32 RandType(Rng& r){ return r.pickI({0,1,2,5,8,9,10,14,15,16,17,18,19,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,int64_t(r.below(47))}); }
inline void SetTimes(Rng& r){ const uint32 t=r.next()>>r.below(4); AtA<uint32>(kTimeMs)=t; AtA<uint32>(kPrevTimeMs)=t-r.pickI({0,1,16,50,1000}); AtA<float>(kTimeStep)=r.unit(0.2f,3.0f); }

// ---------------- entidades falsas para armas ----------------
constexpr size_t kWEnt=8, kWEntSize=0xA20;
alignas(16) inline std::array<std::array<uint8,kWEntSize>,kWEnt> wEnt{};
alignas(16) inline std::array<std::array<uint8,0x60>,kWEnt> wEntMat{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> wTask{};
inline std::array<void*,16> taskVt{};
inline uint32 __fastcall S_TaskType(void* self,void*){ return At<uint32>(self,8); }           // vtable[4] (+0x10): GetTaskType
inline void* MakeEnt(Rng& r,size_t k,int etype,int status,bool matrix=true){
    uint8* e=wEnt[k].data(); std::memset(e,0,kWEntSize);
    At<uint8>(e,0x36)=uint8((status<<3)|etype);
    auto& m=wEntMat[k]; std::memset(m.data(),0,m.size());
    for(size_t o:{size_t(0),size_t(4),size_t(8),size_t(0x10),size_t(0x14),size_t(0x18),size_t(0x20),size_t(0x24),size_t(0x28)})At<float>(m.data(),o)=r.unit(-1,1);
    for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(m.data(),o)=r.unit(-80,80);
    At<void*>(e,0x14)= matrix ? static_cast<void*>(m.data()) : nullptr;
    for(size_t o:{size_t(4),size_t(8),size_t(0xC)})At<float>(e,o)=r.unit(-80,80);
    At<float>(e,0x10)=r.unit(-3.1f,3.1f);
    return e; }
inline void* Ent(size_t k){ return wEnt[k].data(); }
inline void NameWeaponBuffers(){
    Nm(wbuf.data(),wbuf.size(),"weapon"); Nm(fakeFx.data(),fakeFx.size(),"fx"); Nm(wEnt[0].data(),kWEnt*kWEntSize,"wents");
    Nm(wTask[0].data(),sizeof(wTask),"wtask"); }
inline CVector RV(Rng& r,float a=80.0f){ return CVector{r.unit(-a,a),r.unit(-a,a),r.unit(-a,a)}; }

// ---------------- dobles de CWeapon ----------------
inline uint32 gSkillSalt{};
inline uint8 __fastcall S_WeaponSkill(void* self,void*,uint32 type){ if(!Mine())return 1; T("weapon_skill",self,type); return uint8((type*7u+gSkillSalt)&3u); }
inline void __fastcall S_AddRef(void* self,void*){ if(Mine())T("add_ref",U(self)); }
inline void __fastcall S_RemoveRef(void* self,void*){ if(Mine())T("remove_ref",U(self)); }
inline void __fastcall S_FxKill(void* self,void*){ if(Mine())T("fx_kill",self); }
inline void __cdecl S_InitW(){ if(Mine())T("weapon_info_init"); }   inline void __cdecl S_ShutW(){ if(Mine())T("weapon_info_shutdown"); }
inline void __cdecl S_InitShot(){ if(Mine())T("shot_init"); }       inline void __cdecl S_ShutShot(){ if(Mine())T("shot_shutdown"); }      inline void __cdecl S_UpdShot(){ if(Mine())T("shot_update"); }
inline void __cdecl S_InitExpl(){ if(Mine())T("explosion_init"); }  inline void __cdecl S_ShutExpl(){ if(Mine())T("explosion_shutdown"); } inline void __cdecl S_UpdExpl(){ if(Mine())T("explosion_update"); }
inline void __cdecl S_InitProj(){ if(Mine())T("projectile_init"); } inline void __cdecl S_ShutProj(){ if(Mine())T("projectile_shutdown"); } inline void __cdecl S_UpdProj(){ if(Mine())T("projectile_update"); }
inline void __cdecl S_InitBul(){ if(Mine())T("bullet_init"); }      inline void __cdecl S_ShutBul(){ if(Mine())T("bullet_shutdown"); }     inline void __cdecl S_UpdBul(){ if(Mine())T("bullet_update"); }
inline void __fastcall S_RemoveColVol(void* self,void*){ if(Mine())T("remove_col_volumes",U(self)); }
inline std::array<float,16> gEvalRatings{}; inline uint32 gEvalCalls{};
inline float __cdecl S_EvalTarget(void* target,const CVector* origin,const CVector* dir,float tol,uint32 planes,void* preferred){
    if(!Mine())return -1.0f; const float v=gEvalRatings[(gEvalCalls++)&15u];
    T("evaluate_target",target,origin->x,origin->y,origin->z,dir->x,dir->y,dir->z,tol,planes&0xFF,preferred,v); return v; }
inline bool __fastcall S_IsOnScreen(void* self,void*){ if(!Mine())return false; T("is_on_screen",self); return (gSphereBits>>(gSphereCalls++&31u))&1u; }
inline void __fastcall S_AddParticle(void* self,void*,const CVector* pos,const CVector* vel,float timeSince,const void* mults,float rotZ,float lightMult,float lightLimit,uint32 local){
    if(Mine())T("add_particle",self,pos->x,pos->y,pos->z,vel->x,vel->y,vel->z,timeSince,Fnv(mults,0x1C),rotZ,lightMult,lightLimit,local&0xFF); }
inline void __fastcall S_AddAudioEvent(void* self,void*,uint32 ev){ if(Mine())T("weapon_audio_event",ev); (void)self; }
inline uint32 gFireIHRet{};
inline bool __fastcall S_FireInstantHitFromCar(void* self,void*,void* veh,uint32 left,uint32 right){ if(!Mine())return false; T("fire_instant_hit_from_car",veh,left&0xFF,right&0xFF); (void)self; return gFireIHRet&1u; }

// ---------------- consultas simples de tipo ----------------
inline json CaseWeaponQuery(Rng& r,int which){   // 0 IsTypeMelee 1 IsType2Handed 2 IsTypeProjectile 3 HasWeaponAmmoToBeUsed 4 CanBeUsedFor2Player (metodo)
    const int32 type= which==3 ? r.pickI({0,1,5,6,7,8,9,10,14,15,16,22,30,45,46,47,48,60,100}) : RandType(r);
    FillWeapon(r,type);
    static const uintptr_t addr[]{0x73B1C0,0x73B1E0,0x73B210,0x73B2A0,0x73DEF0};
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*)>(addr[which])(W())&0xFF;
    json o=WeaponOut(); o["al"]=al; return o; }
inline json CaseCanBeUsedStatic(Rng& r){
    const int32 type=r.pickI({-1,0,8,9,10,33,34,35,36,45,46,47,100,int64_t(int32(r.below(80))-5),int64_t(int32(r.next()))});
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(int32)>(0x73B240)(type)&0xFF;
    json o=json::object(); o["al"]=al; o["type"]=type; return o; }
inline json CaseConstructor(Rng& r){
    FillWeapon(r,0); const int32 type=r.pickI({0,1,22,24,28,30,31,34,35,38,40,46,int64_t(r.below(47))});
    const int32 ammo=r.pickI({0,1,2,17,100,500,99999,100000,100001,0x7FFFFFFF,-1,-100,int64_t(int32(r.next()))});
    void* ret=reinterpret_cast<void*(__thiscall*)(CWeapon*,int32,int32)>(0x73B430)(W(),type,ammo);
    json o=WeaponOut(); o["ret_is_this"]=(ret==wbuf.data()); o["type"]=type; o["ammo"]=ammo; return o; }
inline void ModelRefStubs(Patches& P){ P.Add(0x4C4BA0,S_AddRef); P.Add(D.addRef,S_AddRef); P.Add(0x4C4BB0,S_RemoveRef); P.Add(D.removeRef,S_RemoveRef); }
inline json CaseInitialise(Rng& r){
    FillWeapon(r,RandType(r)); gSkillSalt=r.next();
    const int32 type=r.pickI({0,1,22,24,28,30,31,34,35,38,40,46,int64_t(r.below(47))});
    const int32 ammo=r.pickI({0,1,17,100,500,99999,100000,0x7FFFFFFF,-1,-5,int64_t(int32(r.next()))});
    void* owner= r.pct(55) ? static_cast<void*>(wEnt[1].data()) : nullptr;
    Patches P; ModelRefStubs(P); P.Add(0x5E3B60,S_WeaponSkill); P.Add(D.getWeaponSkill,S_WeaponSkill);
    reinterpret_cast<void(__thiscall*)(CWeapon*,int32,int32,void*)>(0x73B4A0)(W(),type,ammo,owner);
    json o=WeaponOut(); o["type"]=type; o["ammo"]=ammo; return o; }
inline json CaseReload(Rng& r){
    FillWeapon(r,RandType(r)); gSkillSalt=r.next();
    void* owner= r.pct(55) ? static_cast<void*>(wEnt[1].data()) : nullptr;
    Patches P; P.Add(0x5E3B60,S_WeaponSkill); P.Add(D.getWeaponSkill,S_WeaponSkill);
    reinterpret_cast<void(__thiscall*)(CWeapon*,void*)>(0x73AEB0)(W(),owner);
    return WeaponOut(); }
inline json CaseShutdown(Rng& r){
    FillWeapon(r,RandType(r));
    Patches P; ModelRefStubs(P);
    reinterpret_cast<void(__thiscall*)(CWeapon*)>(0x73A380)(W());
    return WeaponOut(); }
inline json CaseStopEffect(Rng& r){
    FillWeapon(r,r.pickI({0,18,18,22,37,41,42,int64_t(r.below(47))}));
    Patches P; P.Add(0x4AA3F0,S_FxKill); P.Add(D.fxKill,S_FxKill);
    reinterpret_cast<void(__thiscall*)(CWeapon*)>(0x73B360)(W());
    return WeaponOut(); }
inline json CaseRangeMultiplier(Rng& r){
    SetTimes(r);
    const int tt=int(r.pickI({0,1,2,3,3,2,4})); const int ot=int(r.pickI({3,3,3,2,0,1}));
    uint8* target=static_cast<uint8*>(MakeEnt(r,0,tt,0)); uint8* owner=static_cast<uint8*>(MakeEnt(r,1,ot,0)); uint8* veh=static_cast<uint8*>(MakeEnt(r,2,2,0)); uint8* att=static_cast<uint8*>(MakeEnt(r,3,2,0));
    At<int32>(target,0x590)=r.pickI({0,1,9,9,10}); At<void*>(target,0x58C)= r.pct(50) ? static_cast<void*>(veh) : nullptr; At<void*>(target,0xFC)= r.pct(50) ? static_cast<void*>(att) : nullptr;
    At<int32>(veh,0x590)=r.pickI({0,9,9,5}); At<int32>(att,0x590)=r.pickI({0,9,5}); At<int32>(owner,0x598)=r.pickI({0,1,2,3,4,5});
    AtA<uint8>(kActiveCamIdx)=uint8(r.below(3)); AtA<uint16>(kCamMode+uintptr_t(AtA<uint8>(kActiveCamIdx))*kCamStride)=uint16(r.pickI({45,49,0,7,46}));
    void* t= r.pct(8) ? nullptr : target; void* o= r.pct(8) ? nullptr : owner;
    const float res=reinterpret_cast<float(__cdecl*)(void*,void*)>(0x73B380)(t,o);
    json j=json::object(); j["res"]=Bits(res); return j; }
inline json CaseWeaponsInit(Rng& r,int which){   // 0 InitialiseWeapons 1 ShutdownWeapons 2 UpdateWeapons
    (void)r; AtA<uint8>(kFlags73A)=uint8(r.below(2)); AtA<uint8>(kFlags73A+1)=uint8(r.below(2));
    Patches P;
    if(which==0){ P.Add(0x5BF750,S_InitW); P.Add(D.weaponInfoInit,S_InitW); P.Add(0x739B60,S_InitShot); P.Add(D.shotInfoInit,S_InitShot); P.Add(0x736A40,S_InitExpl); P.Add(D.explInit,S_InitExpl);
        P.Add(0x737B40,S_InitProj); P.Add(D.projInit,S_InitProj); P.Add(0x735FD0,S_InitBul); P.Add(D.bulletInit,S_InitBul); }
    else if(which==1){ P.Add(0x743C50,S_ShutW); P.Add(D.weaponInfoShutdown,S_ShutW); P.Add(0x739C20,S_ShutShot); P.Add(D.shotInfoShutdown,S_ShutShot); P.Add(0x7368F0,S_ShutExpl); P.Add(D.explShutdown,S_ShutExpl);
        P.Add(0x737BC0,S_ShutProj); P.Add(D.projShutdown,S_ShutProj); P.Add(0x736000,S_ShutBul); P.Add(D.bulletShutdown,S_ShutBul); P.Add(0x40F9E0,S_RemoveColVol); P.Add(D.removeColVolumes,S_RemoveColVol); }
    else { P.Add(0x739E60,S_UpdShot); P.Add(D.shotInfoUpdate,S_UpdShot); P.Add(0x737620,S_UpdExpl); P.Add(D.explUpdate,S_UpdExpl); P.Add(0x738B20,S_UpdProj); P.Add(D.projUpdate,S_UpdProj);
        P.Add(0x7360D0,S_UpdBul); P.Add(D.bulletUpdate,S_UpdBul); }
    reinterpret_cast<void(__cdecl*)()>(which==0 ? 0x73A300 : which==1 ? 0x73A330 : 0x73A360)();
    json o=json::object(); o["flags"]=json::array({AtA<uint8>(kFlags73A),AtA<uint8>(kFlags73A+1)}); return o; }

// ---------------- misiles buscadores ----------------
inline json CaseEvaluate(Rng& r){
    const int tt=int(r.pickI({2,2,2,3,1})); uint8* target=static_cast<uint8*>(MakeEnt(r,0,tt,0,r.pct(85)));
    At<int32>(target,0x594)=r.pickI({0,3,4,5,9});
    uint8* pref=static_cast<uint8*>(MakeEnt(r,1,2,0));
    CVector origin=RV(r,60), dir=RV(r,1.0f); const float tol=r.pickF({0.5f,1.0f,1.2f,2.0f,r.unit(0.1f,4.0f)});
    const uint32 planes=r.below(2); void* preferred= r.pct(35) ? static_cast<void*>(target) : r.pct(40) ? static_cast<void*>(pref) : nullptr;
    const float res=reinterpret_cast<float(__cdecl*)(void*,const CVector*,const CVector*,float,uint32,void*)>(0x73E560)(target,&origin,&dir,tol,planes,preferred);
    json o=json::object(); o["res"]=Bits(res); return o; }
constexpr size_t kPoolSlots=8, kVehStride=0xA18;
alignas(16) inline std::array<uint8,kPoolSlots*kVehStride> fakeVehStore{};
alignas(16) inline std::array<uint8,kPoolSlots+8> fakeVehFlags{};
alignas(16) inline std::array<uint8,0x14> fakeVehHdr{};
inline json CasePickTarget(Rng& r){
    FillBytes(fakeVehStore.data(),fakeVehStore.size(),r);
    for(size_t s=0;s<kPoolSlots;++s){ uint8* v=fakeVehStore.data()+s*kVehStride;
        At<uint8>(v,0x36)=2; At<uint8>(v,0x42D)=uint8(r.pct(70) ? 0x40 : 0); At<float>(v,0x4C0)=r.pickF({-5.0f,0.0f,100.0f,1000.0f,r.unit(0,1000)}); At<void*>(v,0x14)=nullptr;
        fakeVehFlags[s]= r.pct(25) ? uint8(0x80|r.below(128)) : uint8(r.below(128)); }
    for(size_t s=kPoolSlots;s<fakeVehFlags.size();++s)fakeVehFlags[s]=0x80;
    std::memset(fakeVehHdr.data(),0,fakeVehHdr.size());
    At<void*>(fakeVehHdr.data(),0)=fakeVehStore.data(); At<void*>(fakeVehHdr.data(),4)=fakeVehFlags.data(); At<int32>(fakeVehHdr.data(),8)=int32(kPoolSlots); At<int32>(fakeVehHdr.data(),0xC)=-1;
    AtA<void*>(kVehiclePoolPtr)=fakeVehHdr.data();
    gEvalCalls=0; for(auto& v:gEvalRatings)v=r.pickF({-1.0f,0.0f,0.5f,2.0f,2.0f,5.0f,100000.0f,float(r.below(5))});
    CVector origin=RV(r,60), dir=RV(r,1.0f); void* ignore= r.pct(30) ? static_cast<void*>(fakeVehStore.data()+r.below(uint32(kPoolSlots))*kVehStride) : nullptr;
    void* pref= r.pct(30) ? static_cast<void*>(fakeVehStore.data()) : nullptr;
    Patches P; P.Add(0x73E560,S_EvalTarget); P.Add(D.evaluateTarget,S_EvalTarget);
    void* res=reinterpret_cast<void*(__cdecl*)(CVector,CVector,float,void*,uint32,void*)>(0x73F910)(origin,dir,r.unit(0.5f,2.0f),ignore,r.below(2),pref);
    json o=json::object(); o["res"]=Name(U(res)); o["res_idx"]= res ? json(int32((uintptr_t(res)-uintptr_t(fakeVehStore.data()))/kVehStride)) : json(-1); return o; }

// ---------------- SetUpPelletCol ----------------
alignas(16) inline std::array<uint8,0x40> fakeColData{};
alignas(16) inline std::array<uint8,16*0x20+0x40> fakeLines{};
alignas(16) inline std::array<uint8,0x60> pelletMat{};
alignas(16) inline std::array<uint8,0x40> pelletColPoint{};
inline json CasePelletCol(Rng& r){
    std::memset(fakeColData.data(),0,fakeColData.size()); std::memset(fakeLines.data(),0xCD,fakeLines.size()); std::memset(pelletMat.data(),0xCD,pelletMat.size());
    FillBytes(pelletColPoint.data(),pelletColPoint.size(),r);
    At<void*>(fakeColData.data(),0x10)=fakeLines.data();
    AtA<void*>(kPelletCol+0x2C)=fakeColData.data();     // 0xC8A808 (m_pColData)
    AtA<float>(kSpreadRate)=r.pickF({0.0f,0.5f,1.0f,2.0f,r.unit(0.0f,3.0f)});
    const int32 n=r.pickI({1,2,3,7,10,15,int64_t(1+r.below(15))});
    const int vt=int(r.pickI({1,2,3,4,3,2})); const int ot=int(r.pickI({3,2,3,1}));
    void* victim=MakeEnt(r,0,vt,0); void* owner=MakeEnt(r,1,ot,0);
    CVector point=RV(r,40);
    CVector hit=point; if(r.pct(10)){ hit=point; } else { hit.x+=r.unit(-30,30); hit.y+=r.unit(-30,30); hit.z+=r.pct(40) ? r.unit(-60,60) : r.unit(-3,3); }
    At<float>(pelletColPoint.data(),0)=hit.x; At<float>(pelletColPoint.data(),4)=hit.y; At<float>(pelletColPoint.data(),8)=hit.z;
    CVector nrm=RV(r,1.0f); if(r.pct(25)){ nrm=CVector{r.pickF({0.0f,0.95f,-0.95f,1.0f}),r.unit(-0.2f,0.2f),r.unit(-0.2f,0.2f)}; }
    At<float>(pelletColPoint.data(),0x10)=nrm.x; At<float>(pelletColPoint.data(),0x14)=nrm.y; At<float>(pelletColPoint.data(),0x18)=nrm.z;
    FillWeapon(r,22);
    Patches P; AddRandStubs(P);
    const auto cm0=ReversingProbe::Bytes(reinterpret_cast<void*>(kPelletCol),0x30);
    reinterpret_cast<void(__thiscall*)(CWeapon*,int32,void*,void*,CVector*,void*,void*)>(0x73C710)(W(),n,owner,victim,&point,pelletColPoint.data(),pelletMat.data());
    json o=json::object(); o["mat"]=Fnv(pelletMat.data(),0x60); o["mat_first48"]=Fnv(pelletMat.data(),0x48); o["lines"]=Fnv(fakeLines.data(),fakeLines.size()); o["coldata"]=Fnv(fakeColData.data(),fakeColData.size());
    o["colmodel"]=Fnv(reinterpret_cast<void*>(kPelletCol),0x30); o["colpoint"]=Fnv(pelletColPoint.data(),pelletColPoint.size()); o["point"]=V3(point); (void)cm0; return o; }

// ---------------- AddGunshell ----------------
inline json CaseGunshell(Rng& r){
    SetupFakeCamera(r); gSphereBits=r.next(); gSphereCalls=0;
    AtA<void*>(kGunShellPtr)=fakeFx.data();
    FillWeapon(r,r.pickI({22,25,27,27,25,30,31,int64_t(r.below(47))}));
    void* creator= r.pct(10) ? nullptr : MakeEnt(r,0,3,0);
    CVector pos=RV(r,5), dir0{r.unit(-1,1),r.unit(-1,1),0}; const float size=r.unit(0.05f,3.0f);
    if(creator&&r.pct(60)){ for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(wEntMat[0].data(),o)=At<float>(fakeCamMat.data(),o)+r.unit(-8.0f,8.0f); }
    Patches P; AddRandStubs(P); P.Add(0x534540,S_IsOnScreen); P.Add(D.getIsOnScreen,S_IsOnScreen); P.Add(0x4AA440,S_AddParticle); P.Add(D.addParticle,S_AddParticle);
    reinterpret_cast<void(__thiscall*)(CWeapon*,void*,CVector*,const CVector*,float)>(0x73A3E0)(W(),creator,&pos,&dir0,size);
    json o=WeaponOut(); o["pos"]=V3(pos); return o; }

// ---------------- FireFromCar ----------------
inline json CaseFireFromCar(Rng& r){
    SetTimes(r); FillWeapon(r,RandType(r));
    At<uint32>(wbuf.data(),4)=uint32(r.pickI({0,0,1,1,2,3,4,0}));
    void* veh=MakeEnt(r,0,2,r.pickI({0,0,2,5})); void* drv= r.pct(70) ? MakeEnt(r,1,3,0) : nullptr; At<void*>(veh,0x460)=drv;
    gFireIHRet=r.next(); AtA<uint8>(kInfiniteAmmo)=uint8(r.pct(15)); gPct=r.pickF({0.0f,50.0f,99.9f,100.0f,100.5f}); gStats.fill(0.0f);
    Patches P; P.Add(0x73EC40,S_FireInstantHitFromCar); P.Add(D.fireInstantHitFromCar,S_FireInstantHitFromCar);
    P.Add(0x4E69F0,S_AddAudioEvent); P.Add(D.addAudioEvent,S_AddAudioEvent); P.Add(0x5591E0,S_Pct); P.Add(D.pctProgress,S_Pct);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*,void*,uint32,uint32)>(0x73FA20)(W(),veh,r.below(2),r.below(2))&0xFF;
    json o=WeaponOut(); o["al"]=al; return o; }

// ---------------- DoDoomAiming ----------------
inline int32 gFoundCount{}; inline std::array<void*,16> gFound{}; inline uint32 gSeeBits{}, gSeeCalls{}, gLosBits{}, gLosCalls{};
inline void __cdecl S_FindObjects(const CVector* pt,float radius,uint32 b2d,int16* outCount,int16 maxCount,void** out,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e){
    if(!Mine()){ if(outCount)*outCount=0; return; }
    T("find_objects",pt->x,pt->y,pt->z,radius,b2d&0xFF,int32(maxCount),a&0xFF,b&0xFF,c&0xFF,d&0xFF,e&0xFF);
    int16 n=int16(std::min<int32>(gFoundCount,maxCount)); for(int16 k=0;k<n;++k)out[k]=gFound[size_t(k)]; *outCount=n; }
inline bool __fastcall S_CanSee(void* self,void*,void* ent,float angle){ if(!Mine())return false; const bool v=(gSeeBits>>(gSeeCalls++&31u))&1u; T("can_see",self,ent,angle,uint8(v)); return v; }
inline bool __cdecl S_Los(const CVector* s,const CVector* e,void* cp,void** hit,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e2,uint32 f,uint32 g,uint32 h){
    if(!Mine())return false; const bool v=(gLosBits>>(gLosCalls++&31u))&1u;
    T("los",s->x,s->y,s->z,e->x,e->y,e->z,a&0xFF,b&0xFF,c&0xFF,d&0xFF,e2&0xFF,f&0xFF,g&0xFF,h&0xFF,uint8(v)); (void)cp; if(hit)*hit=nullptr; return v; }
inline json CaseDoomAiming(Rng& r){
    uint8* owner=static_cast<uint8*>(MakeEnt(r,0,3,0)); At<uint32>(owner,0x46C)=r.next();
    gFoundCount=r.pickI({0,1,2,5,15,16,-1}); for(size_t k=0;k<16;++k){ if(k<7){ MakeEnt(r,1+k,r.pickI({2,3,4}),r.pickI({0,0,5,6,7,2})); gFound[k]=Ent(1+k); } else gFound[k]=Ent(1+(k%7)); }
    if(r.pct(25))gFound[r.below(7)]=owner;
    gSeeBits=r.next(); gSeeCalls=0; gLosBits=r.next(); gLosCalls=0;
    CVector start=RV(r,40), end=RV(r,60); if(r.pct(20)) end.z=start.z+r.unit(-3,3);
    Patches P; P.Add(0x564A20,S_FindObjects); P.Add(D.findObjectsInRange,S_FindObjects); P.Add(0x5E0730,S_CanSee); P.Add(D.canSeeEntity,S_CanSee); P.Add(0x56BA00,S_Los); P.Add(D.losWorld,S_Los);
    reinterpret_cast<void(__cdecl*)(void*,CVector*,CVector*)>(0x73CDC0)(owner,&start,&end);
    json o=json::object(); o["start"]=V3(start); o["end"]=V3(end); return o; }

// ---------------- LaserScopeDot ----------------
inline int32 gScreenOk{};
inline bool __cdecl S_CalcScreen(const CVector* w,CVector* out,float* wd,float* ht,uint32 a,uint32 b){
    if(!Mine())return false; T("calc_screen",w->x,w->y,w->z,a&0xFF,b&0xFF);
    if(!(gScreenOk&1))return false; *out=CVector{w->x*3.0f+1.0f,w->y*2.0f-1.0f,w->z+0.5f}; *wd=std::fabs(w->x)+10.0f; *ht=std::fabs(w->y)+4.0f; return true; }
inline bool __cdecl S_LosFill(const CVector* s,const CVector* e,void* cp,void** hit,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e2,uint32 f,uint32 g,uint32 h){
    if(!Mine())return false; const bool v=(gLosBits>>(gLosCalls++&31u))&1u;
    T("los",s->x,s->y,s->z,e->x,e->y,e->z,a&0xFF,b&0xFF,c&0xFF,d&0xFF,e2&0xFF,f&0xFF,g&0xFF,h&0xFF,uint8(v));
    if(v){ auto* p=static_cast<uint8*>(cp); At<float>(p,0)=(s->x+e->x)*0.5f; At<float>(p,4)=(s->y+e->y)*0.5f; At<float>(p,8)=(s->z+e->z)*0.5f; } if(hit)*hit=Ent(2); return v; }
inline void SetupCamera3D(Rng& r){
    AtA<uint8>(kActiveCamIdx)=uint8(r.below(3));
    const CVector f=RV(r,1.0f), s=RV(r,50);
    AtA<float>(CamF(kCamFront))=f.x; AtA<float>(CamF(kCamFront)+4)=f.y; AtA<float>(CamF(kCamFront)+8)=f.z;
    AtA<float>(CamF(kCamSource))=s.x; AtA<float>(CamF(kCamSource)+4)=s.y; AtA<float>(CamF(kCamSource)+8)=s.z; }
inline json CaseLaserDot(Rng& r){
    SetupCamera3D(r); FillWeapon(r,r.pickI({22,24,25,28,34,35,38,int64_t(r.below(47))}));
    MakeEnt(r,2,3,0); gLosBits=r.next(); gLosCalls=0; gScreenOk=int32(r.next());
    CVector out{7.0f,7.0f,7.0f}; float size=-5.0f;
    Patches P; P.Add(0x56BA00,S_LosFill); P.Add(D.losWorld,S_LosFill); P.Add(0x70CE30,S_CalcScreen); P.Add(D.calcScreen,S_CalcScreen); P.Add(0x6FC580,S_Corona); P.Add(D.corona,S_Corona);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*,CVector*,float*)>(0x73A8D0)(W(),&out,&size)&0xFF;
    json o=WeaponOut(); o["al"]=al; o["out"]=V3(out); o["size"]=Bits(size); return o; }

// regiones globales propias de CWeapon
inline void SnapWeaponRegions(){
    Snap(0xB6F190,0x700); Snap(kActiveCamIdx,1); Snap(kPlayerWeaponMode,4); Snap(kTimeStep,4); Snap(kInfiniteAmmo,1); Snap(kIgnoreEntity,4); Snap(kIncludeBikers,3);
    Snap(kSpreadRate,4); Snap(kPelletCol,0x30); Snap(kGunShellPtr,4); Snap(kVehiclePoolPtr,4); Snap(kFlags73A,2); }
}
