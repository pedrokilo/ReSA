#pragma once
// Casos 2: CWeapon::DoBulletImpact (0x73B550) y TakePhotograph (0x73C1F0).
#include "Cases1.hpp"

namespace WeaponFinal2Live {

// =============== DoBulletImpact ===============
alignas(16) inline std::array<uint8,0x40> fCpIn{};                              // CColPoint de entrada (0x2C)
inline void RandVictimFields(Rng& r,size_t k,int vk){
    // vk: 1 ped 2 vehiculo 3 objeto 4 edificio 5 dummy (ya creados con MakePed/MakeVeh/MakeEnt)
    uint8* e=EntB(k);
    if(vk==1){ At<float>(e,0x540)=r.pickF({100.0f,10.0f,1.0f,0.0f,-1.0f,-50.0f,QNaN()}); At<uint32>(e,0x470)=r.pct(30) ? 0x1000u : 0u; At<int32>(e,0x530)=r.pickI({0,1,2,54,55}); }
    else if(vk==2){
        At<uint8>(e,0x36)=uint8(((r.pct(15) ? 5 : 0)<<3)|2);
        reinterpret_cast<CVehicle*>(e)->vehicleFlags.bCanBeDamaged= r.pct(80);
        At<uint32>(e,0x40)= r.pct(25) ? 0x40000u : 0u; }
    else if(vk==3){
        At<float>(e,0x154)=r.pickF({100.0f,50.0f,1.0f,0.0f,-1.0f}); At<uint8>(e,0x144)=uint8(r.pickI({0,1,50,199,200,201,255}));
        At<void*>(e,0x160)=fObjInfo.data(); std::memset(fObjInfo.data(),0,fObjInfo.size());
        At<float>(fObjInfo.data(),0x14)=r.pickF({-1.0f,0.0f,0.0f,0.5f,10.0f}); At<float>(fObjInfo.data(),0x18)=r.pickF({0.0f,0.3f,0.5f,0.6f,1.0f,99.0f,99.8f,99.9f,100.0f,1000.0f});
        At<float>(fObjInfo.data(),0x34)=r.pickF({0.0f,0.5f,1.0f,2.5f}); At<int32>(fObjInfo.data(),0x48)=r.pickI({0,1,2,3});
        At<uint32>(e,0x40)=(r.pct(30) ? 4u : 0u)|(r.pct(30) ? 0x20u : 0u)|(r.pct(30) ? 0x80u : 0u);
        reinterpret_cast<CEntity*>(e)->m_bIsStatic= r.pct(50); }
    At<uint16>(e,0x22)=uint16(r.pickI({0,1,400,0x7FFF,0x8000,0xFFFF,int64_t(r.below(20000))})); }
inline json CaseDoBulletImpact(Rng& r){
    ResetAnim(); ResetTasks(); SetTimes(r); gStats.fill(0.0f); gDirSalt=r.next(); gLosCalls=0;
    const int32 type= r.pct(55) ? SkillType(r) : ValidType(r);
    gSkillAllowed=(type>=22&&type<=32); FillWeapon(r,type);
    const int fk=int(r.pickI({0,0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,3,4,4}));        // 0 jugador 1 ped 2 vehiculo 3 nulo 4 se dispara a si mismo
    int vk=int(r.pickI({0,1,1,1,1,1,1,2,2,2,3,3,4,4,4,5}));                      // 0 nulo 1 ped 2 vehiculo 3 objeto 4 edificio 5 dummy
    void* firedBy=nullptr; uint8* fb=nullptr;
    if(fk==0||fk==4)fb=MakePed(r,0,int(r.pickI({0,1})),r.pct(80)); else if(fk==1)fb=MakePed(r,0,int(r.pickI({2,3,4,5,6,7,8,9,17,18})),r.pct(80)); else if(fk==2)fb=MakeVeh(r,0,r.pct(80));
    if(fb){ firedBy=fb; const int st=int(r.pickI({0,0,0,2,4})); At<uint8>(fb,0x36)=uint8((st<<3)|(fk==2 ? 2 : 3)); }
    if(fk==3)vk=0; if(fk==4)vk=1;
    if(fb&&fk!=2){ if(r.pct(15))At<void*>(fb,0x480)=nullptr; }
    void* victim=nullptr;
    if(vk==1){ if(fk==4)victim=fb; else { victim=MakePed(r,1,r.pct(40)&&fb&&fk!=2 ? At<int32>(fb,0x598) : int(r.pickI({2,3,4,5,6,7,8,9,17,18})),r.pct(80)); } }
    else if(vk==2)victim=MakeVeh(r,1,r.pct(80)); else if(vk==3)victim=MakeEnt(r,1,4,0,r.pct(70)); else if(vk==4)victim=MakeEnt(r,1,1,0); else if(vk==5)victim=MakeEnt(r,1,5,0);
    if(victim&&victim!=fb)RandVictimFields(r,1,vk);
    if(fb&&fk!=2){
        // objetivo apuntado del tirador (campo 0x71C): nulo, o una entidad de cualquier tipo
        const int tk=int(r.pickI({0,0,0,1,2,3,4})); void* tgt=nullptr;
        if(tk==1)tgt=MakePed(r,2,int(r.pickI({2,4,6})),true); else if(tk==2)tgt=MakeVeh(r,2,true); else if(tk==3)tgt=MakeEnt(r,2,4,0,true); else if(tk==4)tgt=MakeEnt(r,2,1,0);
        if(tgt)RandVictimFields(r,2,tk==1 ? 1 : tk==2 ? 2 : tk==3 ? 3 : 4);
        At<void*>(fb,0x71C)=tgt; }
    // ocupantes del vehiculo victima
    if(vk==2&&victim){
        uint8* v=static_cast<uint8*>(victim); At<void*>(v,0x460)= r.pct(80) ? static_cast<void*>(MakePed(r,3,int(r.pickI({0,2,4})),true)) : nullptr;
        const uint32 n=r.below(4); At<uint8>(v,0x488)=uint8(n);
        for(uint32 i=0;i<4;++i)At<void*>(v,0x464+4*size_t(i))= r.pct(80) ? static_cast<void*>(MakePed(r,4+i,int(r.pickI({2,4,5})),true)) : nullptr; }
    MakePed(r,11,0,r.pct(80)); SetPlayerSwap(Ent(11));
    AtA<uint8>(kActiveCamIdx)=uint8(r.below(3)); AtA<uint16>(kCamMode+uintptr_t(AtA<uint8>(kActiveCamIdx))*kCamStride)=uint16(r.pickI({49,49,0,34,46,7}));
    AtA<uint32>(kProcLevel)=uint32(r.pickI({0,1,2}));
    gStats[126]=r.pickF({0.0f,1.0f,5.0f,5.0f,10.0f,-1.0f,QNaN()}); gStats[128]=r.pickF({0.0f,1.0f,5.0f,5.0f,10.0f,-1.0f,QNaN()});
    CVector s=RV(r,40), e=RV(r,40); if(r.pct(3))e=s;
    if(vk==1&&victim&&victim!=fb&&r.pct(35)){ const CVector vp=EntPos(1); s=CVector{vp.x+r.unit(-0.5f,0.5f),vp.y+r.unit(-0.5f,0.5f),vp.z+r.unit(-0.5f,0.5f)}; }   // disparo a quemarropa: golpe critico (150)
    FillCpTpl(r,s); if(vk==2&&r.pct(50))gCpTpl[0x24]=uint8(13+r.below(4));
    std::memcpy(fCpIn.data(),gCpTpl.data(),0x2C);
    const int32 incr=r.pickI({0,0,0,0,1,2,5,-1,-2,-3,-7});
    Patches P;
    P.Add(0x5E6580,S_SkillCur); P.Add(D.skillCur,S_SkillCur); P.Add(0x532010,S_ReportCrime); P.Add(D.reportCrime,S_ReportCrime); P.Add(0x726AF0,S_AddTrace2); P.Add(D.addTrace2,S_AddTrace2);
    P.Add(0x55CEB0,S_UpdHit); P.Add(D.updHit,S_UpdHit); P.Add(0x558E40,S_GetStat); P.Add(D.getStat,S_GetStat); P.Add(0x55C180,S_IncStat); P.Add(D.incStat,S_IncStat);
    P.Add(0x5F7F40,S_SameGroup); P.Add(D.sameGroup,S_SameGroup); P.Add(0x5DEF60,S_LocalDir); P.Add(D.localDir,S_LocalDir); P.Add(0x73A530,S_GenDamageStub); P.Add(D.genDamage,S_GenDamageStub);
    P.Add(0x506EC0,S_BulletHit); P.Add(D.bulletHit,S_BulletHit); P.Add(0x71C0D0,S_Glass); P.Add(D.glass,S_Glass); P.Add(0x420D40,S_Sphere2); P.Add(D.sphere2,S_Sphere2);
    P.Add(0x49F3D0,S_ImpactFx); P.Add(D.impactFx,S_ImpactFx); P.Add(0x59F0C0,S_LightExe); P.Add(D.lighting,S_LightDll); P.Add(0x49EB00,S_BloodFx); P.Add(D.bloodFx,S_BloodFx);
    P.Add(0x49F300,S_TyreFx); P.Add(D.tyreFx,S_TyreFx); P.Add(0x6D7C90,S_Inflict); P.Add(D.inflict,S_Inflict); P.Add(0x542B50,S_ApplyForce); P.Add(D.applyForce,S_ApplyForce);
    P.Add(0x5A0D90,S_ObjDamage); P.Add(D.objDamage,S_ObjDamage); P.Add(0x542800,S_ToMoving); P.Add(D.addToMoving,S_ToMoving); P.Add(0x4AB420,S_GroupAdd); P.Add(D.groupAdd,S_GroupAdd);
    P.Add(0x53F9A0,S_Shake); P.Add(D.shake,S_Shake); P.Add(0x56D230,S_LocBlood); P.Add(D.locBlood,S_LocBlood);
    reinterpret_cast<void(__thiscall*)(CWeapon*,void*,void*,const CVector*,const CVector*,void*,int32)>(0x73B550)(W(),firedBy,victim,&s,&e,fCpIn.data(),incr);
    json o=WeaponOut(); o["fb"]=Fnv(Ent(0),0x800); o["victim"]=Fnv(Ent(1),0x800); o["target"]=Fnv(Ent(2),0x800); o["pd"]=Fnv(fPData[0].data(),0x200); o["refs"]=RefState();
    o["cp"]=Fnv(fCpIn.data(),0x2C); o["s"]=V3(s); o["e"]=V3(e); return o; }

// =============== TakePhotograph ===============
constexpr size_t kPoolN=6, kPedStride=0x7C4, kObjStride=0x19C;
alignas(16) inline std::array<uint8,kPoolN*kPedStride> fPedStore{};
alignas(16) inline std::array<uint8,kPoolN*kObjStride> fObjStore{};
alignas(16) inline std::array<std::array<uint8,0x60>,kPoolN> fPedMat{}, fObjMat{};
alignas(16) inline std::array<uint8,kPoolN+2> fPedFlags{}, fObjFlags{};
alignas(16) inline std::array<uint8,0x14> fPedHdr{}, fObjHdr{};
inline void* gCurEnt{};
inline CVector RawPos(const void* e){ const uint8* b=static_cast<const uint8*>(e); const uint8* m=At<uint8*>(const_cast<void*>(e),0x14); const float* p= m ? reinterpret_cast<const float*>(m+0x30) : reinterpret_cast<const float*>(b+4); return CVector{p[0],p[1],p[2]}; }
inline CVector* __fastcall S_BonePosW(void* self,void*,CVector* out,uint32 bone,uint32 upd){      // 0x5E4280 (puntero de salida) y miembro de la DLL que devuelve CVector (puntero oculto): misma pila, ret 0xC
    if(!Mine()){ *out=CVector{}; return out; } gCurEnt=self; T("bone_position",self,bone,upd&0xFF);
    const CVector p=RawPos(self); *out=CVector{p.x+0.25f,p.y-0.5f,p.z+1.25f}; return out; }
inline bool __cdecl S_CalcScreenW(const CVector* w,CVector* out,float* wd,float* ht,uint32 a,uint32 b){   // 0x70CE30 cdecl (6 argumentos)
    if(!Mine())return false; T("calc_screen",w->x,w->y,w->z,a&0xFF,b&0xFF);
    for(size_t i=0;i<kPoolN;++i){ if(!(fObjFlags[i]&0x80)){ const CVector p=RawPos(fObjStore.data()+i*kObjStride); if(Bits(p.x)==Bits(w->x)&&Bits(p.y)==Bits(w->y)&&Bits(p.z)==Bits(w->z))gCurEnt=fObjStore.data()+i*kObjStride; } }
    if(!Chance(85))return false;
    const float Wd=float(AtA<int32>(kRsMaxW)), Ht=float(AtA<int32>(kRsMaxH));
    if(Chance(70)){ out->x=gRng->unit(0.1f*Wd,0.9f*Wd); out->y=gRng->unit(0.1f*Ht,0.9f*Ht); } else { out->x=gRng->unit(-0.2f*Wd,1.2f*Wd); out->y=gRng->unit(-0.2f*Ht,1.2f*Ht); }
    out->z=gRng->unit(0,100); *wd=gRng->unit(1,50); *ht=gRng->unit(1,50); return true; }
inline bool __cdecl S_LosPhoto(const CVector* s,const CVector* e,void* cp,void** hit,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e2,uint32 f,uint32 g,uint32 h){
    if(!Mine()){ if(hit)*hit=nullptr; return false; } const bool v=Chance(55);
    T("los",s->x,s->y,s->z,e->x,e->y,e->z,a&0xFF,b&0xFF,c&0xFF,d&0xFF,e2&0xFF,f&0xFF,g&0xFF,h&0xFF,uint8(v)); (void)cp;
    if(hit)*hit= v ? (Chance(50) ? gCurEnt : Ent(0)) : nullptr; return v; }
inline void __cdecl S_PicTaken(){ if(Mine())T("picture_taken"); }                                  // 0x456A70
inline void SetupPool(Rng& r,uint8* store,size_t stride,std::array<std::array<uint8,0x60>,kPoolN>& mats,std::array<uint8,kPoolN+2>& flags,std::array<uint8,0x14>& hdr,const CVector& cam,int etype){
    std::memset(store,0,kPoolN*stride);
    for(size_t i=0;i<kPoolN;++i){
        flags[i]= r.pct(30) ? uint8(0x80|r.below(128)) : uint8(r.below(128));
        uint8* e=store+i*stride; At<uint8>(e,0x36)=uint8(etype); auto& m=mats[i]; std::memset(m.data(),0,m.size());
        const float range= r.pct(60) ? 100.0f : 200.0f; const CVector p{cam.x+r.unit(-range,range),cam.y+r.unit(-range,range),cam.z+r.unit(-range*0.3f,range*0.3f)};
        if(r.pct(60)){ At<void*>(e,0x14)=m.data(); At<float>(m.data(),0x30)=p.x; At<float>(m.data(),0x34)=p.y; At<float>(m.data(),0x38)=p.z; } else { At<float>(e,4)=p.x; At<float>(e,8)=p.y; At<float>(e,0xC)=p.z; }
        At<uint32>(e,etype==3 ? 0x470 : 0x140)= r.pct(30) ? (etype==3 ? 0x40000u : 0x1000u) : 0u; }
    for(size_t i=kPoolN;i<flags.size();++i)flags[i]=0x80;
    std::memset(hdr.data(),0,hdr.size()); At<void*>(hdr.data(),0)=store; At<void*>(hdr.data(),4)=flags.data(); At<int32>(hdr.data(),8)=int32(kPoolN); At<int32>(hdr.data(),0xC)=-1; }
inline json CaseTakePhoto(Rng& r){
    gLosCalls=0; gCurEnt=nullptr; gFxRet= r.pct(60) ? static_cast<void*>(fFx.data()) : nullptr; gStats.fill(0.0f);
    MakeEnt(r,0,3,0);
    // camara: matriz propia (posicion en +0x30) o posicion del placeable (0xB6F02C), frente de m_mCameraMatrix
    const CVector cam=RV(r,50); std::memset(fMat[0].data(),0,fMat[0].size());
    if(r.pct(60)){ At<float>(fMat[0].data(),0x30)=cam.x; At<float>(fMat[0].data(),0x34)=cam.y; At<float>(fMat[0].data(),0x38)=cam.z; AtA<void*>(kCamMatrixPtr)=fMat[0].data(); }
    else { AtA<void*>(kCamMatrixPtr)=nullptr; AtA<float>(kCamPlacePos)=cam.x; AtA<float>(kCamPlacePos+4)=cam.y; AtA<float>(kCamPlacePos+8)=cam.z; }
    const CVector fw=RV(r,1.0f); AtA<float>(kCamMatFwd)=fw.x; AtA<float>(kCamMatFwd+4)=fw.y; AtA<float>(kCamMatFwd+8)=fw.z;
    AtA<uint8>(kActiveCamIdx)=uint8(r.below(3)); AtA<uint16>(kCamMode+uintptr_t(AtA<uint8>(kActiveCamIdx))*kCamStride)=uint16(r.pct(80) ? 46 : r.pickI({0,7,45,47}));
    AtA<int32>(kRsMaxW)=r.pickI({640,800,1024,1280,1920,4096}); AtA<int32>(kRsMaxH)=r.pickI({480,600,768,720,1080,2160});
    SetupPool(r,fPedStore.data(),kPedStride,fPedMat,fPedFlags,fPedHdr,cam,3); SetupPool(r,fObjStore.data(),kObjStride,fObjMat,fObjFlags,fObjHdr,cam,4);
    AtA<void*>(kPedPoolPtr)=fPedHdr.data(); AtA<void*>(kObjPoolPtr)=fObjHdr.data();
    AtA<uint8>(kFlags73A)=uint8(r.below(2)); AtA<uint8>(kFlags73A+1)=uint8(r.below(2));
    CVector pt=RV(r,40); const CVector* point= r.pct(75) ? &pt : nullptr;
    void* owner= r.pct(70) ? Ent(0) : nullptr;
    Patches P; AddFxStubs(P);
    P.Add(0x456A70,S_PicTaken); P.Add(D.pictureTaken,S_PicTaken); P.Add(0x55C180,S_IncStat); P.Add(D.incStat,S_IncStat); P.Add(0x5E4280,S_BonePosW);
    P.Add(D.bonePos,S_BonePosW); P.Add(0x70CE30,S_CalcScreenW); P.Add(D.calcScreen,S_CalcScreenW); P.Add(0x56BA00,S_LosPhoto); P.Add(D.los,S_LosPhoto);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CWeapon*,void*,const CVector*)>(0x73C1F0)(W(),owner,point)&0xFF;
    json o=json::object(); o["al"]=al; o["peds"]=Fnv(fPedStore.data(),fPedStore.size()); o["objs"]=Fnv(fObjStore.data(),fObjStore.size());
    o["flags"]=json::array({AtA<uint8>(kFlags73A),AtA<uint8>(kFlags73A+1)}); return o; }
}
