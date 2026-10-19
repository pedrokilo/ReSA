#pragma once
// Casos de CPickups. Cada caso rellena un fixture determinista sobre el array REAL de pickups (0x9788C0, 620 x 0x20 bytes) y sus
// globales contiguos (0x978610..0x97D690), que se guardan y restauran tras cada ejecucion (los pickups no se tocan fuera del caso:
// el banco corre dentro de un unico fotograma). Los colaboradores externos se doblan con traza en la direccion del EXE y en el cuerpo
// de la DLL; los objetos de los pickups son objetos FALSOS con una vtable falsa cuya ranura 0 (destructor con borrado) se registra.
#include "Common.hpp"

namespace PickWeap1Live {

constexpr uintptr_t kPickups=0x9788C0, kPickRegion=0x978610, kPickRegionSize=0x5080;   // hasta 0x97D690
constexpr uintptr_t kNumMessages=0x978678, kMessages=0x978680, kCollectedIndex=0x978624, kCollected=0x978628, kDisplayHelp=0x8A5F48;
constexpr uintptr_t kPlayerOnWeaponPickup=0x97D640, kCollectBuffer=0x97D644;
constexpr uintptr_t kMoney0=0xB7CE50, kPlayers=0xB7CD98, kPlayerInFocus=0xB7CD74, kCamMatrixPtr=0xB6F03C, kMirrorActive=0xB6F998, kTimeMs=0xB7CB84, kPrevTimeMs=0xB7CB78;
constexpr uintptr_t kModelInfoPtrs=0xA9B0C8;

// ---------------- DLL: cuerpos de los colaboradores (se rellenan una vez, antes de crear ningun EntryPatch) ----------------
struct DllBodies {
    uintptr_t clearBlip{}, worldRemove{}, processGunShot{}, getTaskJetPack{}, saveData{}, loadData{}, shadow{}, corona{}, getRandom{}, groundZ{};
    uintptr_t audio{}, incStat{}, getStat{}, trigMsg{}, pctProgress{}, fov{}, sphereVis{}, updHealth{}, resetSprint{}, getWanted{}, setWanted{};
    uintptr_t fSetProp{}, fSetBack{}, fSetScale{}, fSetOrient{}, fSetCentre{}, fSetColor{}, fSetStyle{}, fPrint{}, insertNum{};
    uintptr_t pickOyster{}, pickHorse{}, generateNewOne{};   // generateNewOne = cuerpo de CPickups::GenerateNewOne_ExeAbi (C++ directo desde 18/10/2026)
    // CWeapon (WeaponCases.hpp)
    uintptr_t addRef{}, removeRef{}, fxKill{}, shotInfoInit{}, shotInfoShutdown{}, shotInfoUpdate{}, explInit{}, explShutdown{}, explUpdate{};
    uintptr_t projInit{}, projShutdown{}, projUpdate{}, bulletInit{}, bulletShutdown{}, bulletUpdate{}, weaponInfoInit{}, weaponInfoShutdown{}, removeColVolumes{};
    uintptr_t getWeaponSkill{}, evaluateTarget{}, distToLine{}, getIsOnScreen{}, addParticle{}, findObjectsInRange{}, canSeeEntity{}, losWorld{};
    uintptr_t addAudioEvent{}, getTaskUseGun{}, getAssoc{}, getActiveTask{}, isPlayer{}, fireInstantHitFromCar{}, calcScreen{}, allocateData{};
    uintptr_t fireInstantHit{}, fireProjectile{}, fireM16{}, fireSniper{}, fireArea{}, takePhoto{}, useDetonator{}, say{}, giveWeapon{}, getWeaponSlot{}, setCurrentWeapon{};
    uintptr_t padClear{}, padShake{}, addShot{}, doWeaponEffect{}, startFire{}, reportCrime{}, find3rd{}, mouseCam{}, bonePos{}, handleGunShot{}, oilOnFire{};
    uintptr_t checkOccupant{}, rangeMult{}, doBulletImpact{};
};
inline DllBodies D;

// ---------------- objetos falsos de pickup ----------------
constexpr size_t kFakeObjs=64, kFakeObjSize=0x1A0;
alignas(16) inline std::array<std::array<uint8,kFakeObjSize>,kFakeObjs> fakeObj{};
inline std::array<void*,16> objVt{};
inline uint32 U(const void* p){ return uint32(reinterpret_cast<uintptr_t>(p)); }

// ---------------- dobles ----------------
inline void* __fastcall S_ObjDtor(void* self,void*,uint32 flag){ if(Mine())T("object_delete",self,uint8(flag)); return self; }     // vtable[0]: "push 1; call [vt]" -> ret 4
inline void __cdecl S_ClearBlip(int32 type,int32 ref){ if(Mine())T("clear_blip",type,uint32(ref)); }
inline void __cdecl S_WorldRemove(void* e){ if(Mine())T("world_remove",e); }
inline void __fastcall S_ProcessGunShot(void* self,void*,const CVector* o,const CVector* t){
    if(Mine())T("process_gunshot",int32((uintptr_t(self)-kPickups)/0x20),o->x,o->y,o->z,t->x,t->y,t->z); }
inline void* gJetpackTask{};
inline void* __fastcall S_GetTaskJetPack(void* self,void*){ if(!Mine())return nullptr; T("get_task_jetpack",self); return gJetpackTask; }
inline bool __cdecl S_SaveData(void* src,int32 n){ if(Mine())T("save_data",n,n>0 ? Fnv(src,size_t(n)) : uint64(0)); return true; }
inline bool __cdecl S_LoadData(void* dst,int32 n){
    if(!Mine()||n<=0)return false;
    auto* b=static_cast<uint8*>(dst); for(int32 k=0;k<n;++k)b[k]=gRng ? uint8(gRng->next()>>9) : uint8(0);
    T("load_data",n,Fnv(dst,size_t(n))); return true; }
inline bool __cdecl S_Shadow(uint32 id,uint32 type,void* tex,const CVector* pos,float a,float b,float c,float d,uint32 intensity,uint32 rr,uint32 gg,uint32 bb,float z,float scale,float draw,uint32 temp,float up){
    if(Mine())T("shadow",id,type&0xFF,U(tex),pos->x,pos->y,pos->z,a,b,c,d,intensity&0xFFFF,rr&0xFF,gg&0xFF,bb&0xFF,z,scale,draw,temp&0xFF,up); return true; }
inline void __cdecl S_Corona(uint32 id,void* attach,uint32 r,uint32 g,uint32 b,uint32 a,const CVector* pos,float size,float range,uint32 type,uint32 flare,uint32 refl,uint32 los,uint32 trail,float angle,uint32 neon,float pull,uint32 fullBright,float fade,uint32 below,uint32 white){
    if(Mine())T("corona",id,U(attach),r&0xFF,g&0xFF,b&0xFF,a&0xFF,pos->x,pos->y,pos->z,size,range,type&0xFF,flare&0xFF,refl&0xFF,los&0xFF,trail&0xFF,angle,neon&0xFF,pull,fullBright&0xFF,fade,below&0xFF,white&0xFF); }
inline int __cdecl S_RandExe(){ if(!Mine())return 0; const uint32 v=NextRand15(); T("rand",v); return int(v); }
inline uint16 __cdecl S_RandDll(){ if(!Mine())return 0; const uint32 v=NextRand15(); T("rand",v); return uint16(v); }
inline float __cdecl S_GroundZ(float x,float y,float z,bool* ok,void** ent){
    if(!Mine()){ if(ok)*ok=false; return 0.0f; }
    const uint32 h=Bits(x)*31u+Bits(y)*17u+Bits(z)*7u; const bool good=(h%4u)!=0u; const float zz=z+0.375f*float(int32(h%5u)-2);
    T("ground_z",x,y,z,uint8(good)); if(ok)*ok=good; if(ent)*ent=nullptr; return zz; }
inline int32 gGenerateCounter{};
inline int32 __cdecl S_GenerateNewOne(float x,float y,float z,uint32 model,uint32 type,uint32 ammo,uint32 money,uint32 empty,char* msg){
    if(!Mine())return -1; T("generate_new",x,y,z,model,type&0xFF,ammo,money,empty&0xFF,U(msg)); return 0x00010000+(++gGenerateCounter); }
inline void __fastcall S_Audio(void* self,void*,uint32 ev,float a,float b){ if(Mine())T("audio",ev,a,b); }
inline void __cdecl S_IncStat(uint32 stat,float v){ if(Mine())T("inc_stat",stat,v); }
inline std::array<float,512> gStats{};
inline float __cdecl S_GetStat(uint32 stat){ if(!Mine())return 0.0f; T("get_stat",stat); return gStats[stat&511u]; }
inline void __cdecl S_Trigger(const char* msg,int32 a,uint32 b,int32 c){
    if(!Mine())return; std::string s; for(int k=0;k<12&&msg[k];++k)s.push_back(msg[k]); trace.push_back(json::array({"trigger",s,int16(a),uint16(b),int16(c)})); }
inline float gPct{};
inline float __cdecl S_Pct(){ if(!Mine())return 0.0f; T("pct"); return gPct; }
inline float gFov{};
inline float __fastcall S_Fov(void*,void*){ if(!Mine())return 0.0f; T("fov"); return gFov; }
inline uint32 gSphereBits{}, gSphereCalls{};
inline bool __fastcall S_Sphere(void*,void*,const CVector* o,float rad,void* mtx){
    if(!Mine())return false; const bool v=(gSphereBits>>(gSphereCalls++&31u))&1u; T("sphere",o->x,o->y,o->z,rad,U(mtx),uint8(v)); return v; }
inline void __cdecl S_UpdHealth(uint32 v){ if(Mine())T("add_to_health",v); }
inline void __fastcall S_ResetSprint(void* self,void*){ if(Mine())T("reset_sprint",self); }
inline int32 gWanted{};
inline int32 __fastcall S_GetWanted(void* self,void*){ if(!Mine())return 0; T("get_wanted",self); return gWanted; }
inline void __fastcall S_SetWanted(void* self,void*,int32 lvl){ if(Mine())T("set_wanted",self,lvl); }
inline void __cdecl S_PickOyster(){ if(Mine())T("picked_oyster"); }
inline void __cdecl S_PickHorse(){ if(Mine())T("picked_horse"); }

// ---------------- fixture de pickups ----------------
inline void FillPickups(Rng& r,uint32 activePct,bool objects){
    auto* base=reinterpret_cast<uint8*>(kPickups);
    for(int k=0;k<620;++k){
        uint8* p=base+k*0x20; FillBytes(p,0x20,r);
        At<float>(p,0)=r.unit(0.0f,64.0f);
        const uint8 type= r.pct(activePct) ? uint8(r.pickI({1,2,3,4,4,5,6,7,8,8,9,10,11,12,12,13,14,15,16,17,18,19,20,20,21,22,22})) : uint8(0);
        At<uint8>(p,0x1C)=type;
        At<uint8>(p,0x1D)=uint8(r.next());
        At<uint8>(p,0x1D)=uint8(At<uint8>(p,0x1D)&(r.pct(60) ? 0xF7u : 0xFFu));   // bit 3 (bVisible) mas bien apagado
        At<int16>(p,0x1A)=int16(r.pickI({1,1,2,3,7,40,0x7FFD,0x7FFE,-2,-1,0,int64_t(r.below(60))}));
        for(int c=0;c<3;++c)At<int16>(p,0x10+2*size_t(c))=int16(int32(r.below(1601))-800);
        At<int16>(p,0x18)=int16(r.pickI({-1,0,1,331,346,349,350,355,358,362,370,371}));
        At<uint32>(p,8)=r.pickI({0,1,5,30,100,500,int64_t(r.below(100000))});
        At<uint32>(p,0xC)=r.pickI({0,1,10,1000,5000,0x7FFFFFFF,int64_t(r.below(60000))});
        At<void*>(p,4)= objects&&r.pct(45) ? static_cast<void*>(fakeObj[r.below(uint32(kFakeObjs))].data()) : nullptr;
    }
}
inline void FillPickGlobals(Rng& r){
    AtA<uint16>(kNumMessages)=0; AtA<uint16>(kCollectedIndex)=uint16(r.below(20));
    for(int k=0;k<20;++k)AtA<uint32>(kCollected+4u*uint32(k))=r.pct(40) ? r.next() : 0u;
    AtA<uint8>(kDisplayHelp)=uint8(r.next());
    AtA<int32>(kPlayerOnWeaponPickup)=r.pickI({0,0,1,5,-3,100}); AtA<int32>(kCollectBuffer)=r.pickI({0,0,1,6,-2,3});
}
inline json PickState(const std::vector<uint8>& before){
    json o=json::object();
    o["pickups"]=Fnv(reinterpret_cast<void*>(kPickups),620*0x20);
    o["globals"]=Fnv(reinterpret_cast<void*>(kPickRegion),kPickups-kPickRegion);
    o["tail"]=Fnv(reinterpret_cast<void*>(kPickups+620*0x20),kPickRegion+kPickRegionSize-(kPickups+620*0x20));
    o["help"]=AtA<uint8>(kDisplayHelp);
    json ch=json::array(); const auto after=ReversingProbe::Bytes(reinterpret_cast<void*>(kPickups),620*0x20);
    for(int k=0;k<620&&ch.size()<10;++k)if(std::memcmp(after.data()+k*0x20,before.data()+k*0x20,0x20)!=0){
        char hex[0x41]{}; for(int b=0;b<0x20;++b)std::snprintf(hex+2*b,3,"%02x",after[size_t(k*0x20+b)]); ch.push_back(json::array({k,std::string(hex)})); }
    o["changed_first10"]=ch;
    return o;
}
inline std::vector<uint8> PickBefore(){ return ReversingProbe::Bytes(reinterpret_cast<void*>(kPickups),620*0x20); }
inline void AddRemoveStubs(Patches& P){
    P.Add(0x587C60,S_ClearBlip); P.Add(D.clearBlip,S_ClearBlip); P.Add(0x563280,S_WorldRemove); P.Add(D.worldRemove,S_WorldRemove); }
// GenerateNewOne es C++ directo: el envoltorio CPickups::GenerateNewOne llama a GenerateNewOne_ExeAbi (cuerpo enganchado en 0x456F20), asi que el falso va en la direccion del EXE Y en ese cuerpo.
inline void AddGenerateStub(Patches& P){ P.Add(0x456F20,S_GenerateNewOne); P.Add(D.generateNewOne,S_GenerateNewOne); }
inline void AddRandStubs(Patches& P){ P.Add(0x821B1E,S_RandExe); P.Add(D.getRandom,S_RandDll); }
inline void AddStatStubs(Patches& P){
    P.Add(0x55C180,S_IncStat); P.Add(D.incStat,S_IncStat); P.Add(0x558E40,S_GetStat); P.Add(D.getStat,S_GetStat);
    P.Add(0x447B80,S_Trigger); P.Add(D.trigMsg,S_Trigger); P.Add(0x506EA0,S_Audio); P.Add(D.audio,S_Audio); }

// ---------------- Init / ReInit ----------------
inline json CaseInit(Rng& r){
    FillPickups(r,60,true); FillPickGlobals(r); AtA<uint16>(kNumMessages)=uint16(r.next());
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)()>(0x454A70)();
    return PickState(before); }
inline json CaseReInit(Rng& r){
    FillPickups(r,r.pickI({10,50,90}),true); FillPickGlobals(r); AtA<uint16>(kNumMessages)=uint16(r.next());
    Patches P; AddRemoveStubs(P);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)()>(0x456E60)();
    return PickState(before); }

// ---------------- indices y referencias ----------------
inline int32 RandRef(Rng& r,bool matching,int32 idx){
    int16 ref= matching ? AtA<int16>(kPickups+uint32(idx)*0x20+0x1A) : int16(r.pickI({1,2,3,40,-1,0,0x7FFE}));
    return int32((uint32(uint16(ref))<<16)|uint32(uint16(idx))); }
inline json CaseAddCollected(Rng& r){
    FillPickups(r,50,false); FillPickGlobals(r);
    const int32 idx=int32(r.below(620)); const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)(int32)>(0x455240)(idx);
    json o=PickState(before); o["idx"]=idx; return o; }
inline json CaseActualIndex(Rng& r,int which){   // 0 GetActualPickupIndex, 1 GetUniquePickupIndex, 2 GetNewUniquePickupIndex
    FillPickups(r,50,false); FillPickGlobals(r);
    const int32 idx=int32(r.below(620));
    if(which==2){ const int16 refs[]{1,2,3,0x7FFD,0x7FFE,0x7FFF,-3,-2,-1,0,40}; AtA<int16>(kPickups+uint32(idx)*0x20+0x1A)=refs[r.below(11)]; }
    const auto before=PickBefore(); int32 res;
    if(which==0){ const int32 ref= r.pct(10) ? -1 : RandRef(r,r.pct(60),idx); res=reinterpret_cast<int32(__cdecl*)(int32)>(0x4552A0)(ref); }
    else res=reinterpret_cast<int32(__cdecl*)(int32)>(which==1 ? 0x455280 : 0x456A30)(idx);
    json o=PickState(before); o["res"]=res; return o; }
inline json CaseIsPickedUp(Rng& r){
    FillPickups(r,50,false); FillPickGlobals(r);
    const int32 want= r.pct(60) ? int32(AtA<uint32>(kCollected+4u*r.below(20))) : int32(r.next());
    const auto before=PickBefore();
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(int32)>(0x454B40)(want)&0xFF;
    json o=PickState(before); o["al"]=al; return o; }
inline json CasePassTime(Rng& r){
    FillPickups(r,70,false); FillPickGlobals(r);
    const uint32 t=r.pickI({0,1,50,1000,5000,0x7FFFFFFF,int64_t(int32(0xFFFFFFFF)),int64_t(r.below(70000))});
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)(uint32)>(0x455200)(t);
    return PickState(before); }
inline json CaseMoneyPerDay(Rng& r){
    FillPickups(r,50,false); FillPickGlobals(r);
    const int32 idx=int32(r.below(620)); const int32 ref= r.pct(10) ? -1 : RandRef(r,r.pct(70),idx); const uint32 money=r.below(70000)&0xFFFF;
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)(int32,uint32)>(0x455680)(ref,money);
    return PickState(before); }
inline json CaseRemovePickUp(Rng& r){
    FillPickups(r,60,true); FillPickGlobals(r);
    const int32 idx=int32(r.below(620)); if(r.pct(70))At<uint8>(reinterpret_cast<void*>(kPickups+uint32(idx)*0x20),0x1C)=uint8(1+r.below(22));
    const int32 ref= r.pct(10) ? -1 : RandRef(r,r.pct(70),idx);
    Patches P; AddRemoveStubs(P);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)(int32)>(0x4573D0)(ref);
    return PickState(before); }
inline json CaseRemoveMission(Rng& r){
    FillPickups(r,60,true); FillPickGlobals(r);
    Patches P; AddRemoveStubs(P);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)()>(0x456DE0)();
    return PickState(before); }
inline json CaseRemoveObjects(Rng& r){
    FillPickups(r,60,true); FillPickGlobals(r);
    Patches P; AddRemoveStubs(P);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)()>(0x455470)();
    return PickState(before); }
inline json CaseRemoveInArea(Rng& r){
    FillPickups(r,60,true); FillPickGlobals(r);
    float x0=r.unit(-110,110), x1=r.unit(-110,110), y0=r.unit(-110,110), y1=r.unit(-110,110), z0=r.unit(-110,110), z1=r.unit(-110,110);
    if(r.pct(80)){ if(x0>x1)std::swap(x0,x1); if(y0>y1)std::swap(y0,y1); if(z0>z1)std::swap(z0,z1); }
    Patches P; AddRemoveStubs(P);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)(float,float,float,float,float,float)>(0x456D30)(x0,x1,y0,y1,z0,z1);
    return PickState(before); }
inline json CaseRemoveUnnecessary(Rng& r){
    FillPickups(r,60,true); FillPickGlobals(r);
    CVector c{r.unit(-100,100),r.unit(-100,100),r.unit(-100,100)}; const float radius=r.pickF({0.0f,5.0f,20.0f,50.0f,150.0f,r.unit(0,200)});
    Patches P; AddRemoveStubs(P);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)(const CVector*,float)>(0x4563A0)(&c,radius);
    return PickState(before); }
inline json CaseBubble(Rng& r){
    FillPickups(r,r.pickI({0,20,70}),false); FillPickGlobals(r);
    CVector c{r.unit(-100,100),r.unit(-100,100),r.unit(-100,100)}; const float radius=r.pickF({0.0f,1.3f,10.0f,50.0f,150.0f,r.unit(0,200)});
    const auto before=PickBefore();
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(CVector,float)>(0x456450)(c,radius)&0xFF;
    json o=PickState(before); o["al"]=al; return o; }
inline json CaseMerge(Rng& r){
    FillPickups(r,60,false); FillPickGlobals(r);
    const int32 wt=r.pickI({0,1,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,int64_t(r.below(47))});
    const auto* info=CWeaponInfo::GetWeaponInfo(eWeaponType(wt),eWeaponSkill::STD);
    const int32 model= info ? info->m_nModelId1 : -1;
    const int32 ptype=r.pickI({1,2,3,4,8,22});
    // varios pickups con el modelo y tipo buscados, cerca del punto
    CVector c{r.unit(-100,100),r.unit(-100,100),r.unit(-100,100)};
    for(int n=0;n<6;++n){ uint8* p=reinterpret_cast<uint8*>(kPickups)+r.below(620)*0x20;
        At<int16>(p,0x18)=int16(model); At<uint8>(p,0x1C)=uint8(ptype);
        for(int k=0;k<3;++k){ const float v= (k==0?c.x:k==1?c.y:c.z)+r.unit(-7.0f,7.0f); At<int16>(p,0x10+2*size_t(k))=int16(int32(v*8.0f)); } }
    const uint32 ammo=r.pickI({0,1,25,100,int64_t(r.below(5000))});
    const auto before=PickBefore();
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(CVector,int32,int32,uint32,uint32)>(0x4555A0)(c,wt,ptype,ammo,r.below(2))&0xFF;
    json o=PickState(before); o["al"]=al; return o; }
inline json CaseFindForObject(Rng& r){
    FillPickups(r,60,true); FillPickGlobals(r);
    void* obj= r.pct(15) ? static_cast<void*>(nullptr) : static_cast<void*>(fakeObj[r.below(uint32(kFakeObjs))].data());
    const auto before=PickBefore();
    const uint32 res=reinterpret_cast<uint32(__cdecl*)(void*)>(0x4551C0)(obj);
    json o=PickState(before); o["res_idx"]=int32((res-uint32(kPickups))/0x20); o["res_rem"]=(res-uint32(kPickups))%0x20; return o; }
inline json CaseDetonate(Rng& r){
    FillPickups(r,60,false); FillPickGlobals(r);
    CVector o0{r.unit(-100,100),r.unit(-100,100),r.unit(-100,100)}, t0{r.unit(-100,100),r.unit(-100,100),r.unit(-100,100)};
    Patches P; P.Add(0x4588B0,S_ProcessGunShot); P.Add(D.processGunShot,S_ProcessGunShot);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)(const CVector*,const CVector*)>(0x4590C0)(&o0,&t0);
    return PickState(before); }

// ---------------- armas y modelos ----------------
constexpr int32 kMiBase=19980, kMiCount=16;
alignas(16) inline std::array<std::array<uint8,0x80>,kMiCount> fakeMi{};
inline std::array<void*,64> miVt{};
inline uint8 __fastcall S_ModelType(void* self,void*){ return At<uint8>(self,0x70); }       // vtable[4] (+0x10): GetModelType
inline json CaseModelForWeapon(Rng& r){
    const int32 wt=r.pickI({0,1,10,22,28,30,34,35,38,40,41,45,46,int64_t(r.below(47))});
    const auto before=PickBefore();
    const int32 res=reinterpret_cast<int32(__cdecl*)(int32)>(0x454AC0)(wt);
    json o=PickState(before); o["res"]=res; return o; }
inline json CaseWeaponForModel(Rng& r){
    // identificadores especiales: los MI_PICKUP_* son variables globales; se fijan a valores conocidos (se restauran)
    AtA<int16>(0x8CD5BC)=int16(kMiBase+0); AtA<int16>(0x8CD5C0)=int16(kMiBase+1); AtA<int16>(0x8CD5C8)=int16(kMiBase+2);
    for(int j=0;j<kMiCount;++j){ auto& m=fakeMi[size_t(j)]; FillBytes(m.data(),m.size(),r); At<void*>(m.data(),0)=miVt.data();
        At<uint8>(m.data(),0x70)=uint8(r.pickI({4,4,1,2,3,0,5,6})); At<uint32>(m.data(),0x24)=uint32(r.pickI({0,22,28,34,45,46,47,48,int64_t(r.below(60))}));
        AtA<void*>(kModelInfoPtrs+4u*uint32(kMiBase+j))=m.data(); }
    int32 model= r.pickI({-1,0x172,kMiBase,kMiBase+1,kMiBase+2,kMiBase+3,kMiBase+4,kMiBase+5,kMiBase+9,kMiBase+15,int64_t(kMiBase+int32(r.below(16)))});
    const auto before=PickBefore();
    const int32 res=reinterpret_cast<int32(__cdecl*)(int32)>(0x454AE0)(model);
    json o=PickState(before); o["res"]=res; o["model"]=model; return o; }

// ---------------- jugador falso ----------------
constexpr size_t kPedSize=0x7C4;
alignas(16) inline std::array<uint8,kPedSize> fakePed{};
alignas(16) inline std::array<uint8,0x300> fakeIntel{};
alignas(16) inline std::array<uint8,0x200> fakePlayerData{};
inline void SetupFakePlayer(Rng& r){
    std::memset(fakePed.data(),0,fakePed.size()); std::memset(fakeIntel.data(),0,fakeIntel.size()); std::memset(fakePlayerData.data(),0,fakePlayerData.size());
    At<void*>(fakePed.data(),0x47C)=fakeIntel.data(); At<void*>(fakePed.data(),0x480)=fakePlayerData.data();
    At<float>(fakePed.data(),0x540)=r.pickF({0.0f,10.0f,50.0f,100.0f,145.0f,-5.0f,r.unit(0,150)}); At<float>(fakePed.data(),0x548)=r.pickF({0.0f,20.0f,100.0f,r.unit(0,100)});
    At<uint32>(fakePlayerData.data(),0x34)=r.next(); At<uint32>(fakePlayerData.data(),0x3C)=r.next();
    AtA<uint8>(kPlayerInFocus)=0;
    AtA<void*>(kPlayers)=fakePed.data();
    AtA<void*>(kPlayers+0x190)=nullptr;
}
inline json CasePlayerCanPickUp(Rng& r){
    SetupFakePlayer(r);
    const int32 wt=r.pickI({0,1,10,22,28,30,34,35,38,40,41,43,45,46,int64_t(r.below(47))});
    gJetpackTask= r.pct(50) ? static_cast<void*>(fakeIntel.data()+0x100) : nullptr;
    Patches P; P.Add(0x601110,S_GetTaskJetPack); P.Add(D.getTaskJetPack,S_GetTaskJetPack);
    const auto before=PickBefore();
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(int32)>(0x4554C0)(wt)&0xFF;
    json o=PickState(before); o["al"]=al; o["wt"]=wt; return o; }
inline json CaseModifyLabel(Rng& r){
    char buf[24]; std::memset(buf,0x7E,sizeof(buf));
    const char* samples[]{"","A","AB","A_","_","AB_C","PU_B","TXT_X_","X__","HELP_Z","Q_"};
    const char* s=samples[r.below(11)]; const size_t n=std::strlen(s); std::memcpy(buf,s,n+1);
    const int16 mode=int16(r.pickI({0,1,2,3,4,5,-1,100}));
    AtA<int16>(0xB73458+0x10A)=mode;
    reinterpret_cast<void(__cdecl*)(char*)>(0x454B70)(buf);
    json o=json::object(); o["buf"]=Fnv(buf,sizeof(buf)); o["text"]=std::string(buf,buf+std::min<size_t>(n,23)); o["mode"]=mode; return o; }
inline json CaseSaveLoad(Rng& r,bool load){
    FillPickups(r,60,true); FillPickGlobals(r);
    Patches P; P.Add(0x5D1270,S_SaveData); P.Add(D.saveData,S_SaveData); P.Add(0x5D1300,S_LoadData); P.Add(D.loadData,S_LoadData);
    const auto before=PickBefore();
    const uint32 al=reinterpret_cast<uint32(__cdecl*)()>(load ? 0x5D35A0 : 0x5D3540)()&0xFF;
    json o=PickState(before); o["al"]=al; return o; }

// ---------------- efectos (entidad falsa + camara falsa) ----------------
constexpr size_t kFakeEnt=64, kFakeEntStride=0x108;
alignas(16) inline std::array<uint8,kFakeEnt*kFakeEntStride+64> fakeEnts{};
alignas(16) inline std::array<std::array<uint8,0x60>,kFakeEnt> fakeEntMat{};
alignas(16) inline std::array<uint8,0x60> fakeCamMat{};
inline void* FakeEntity(Rng& r,int k,bool withMatrix){
    uint8* e=fakeEnts.data()+size_t(k)*kFakeEntStride+(r.below(16)&~3u);
    std::memset(e,0,0x40); auto& m=fakeEntMat[size_t(k)]; FillBytes(m.data(),m.size(),r);
    for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(m.data(),o)=r.unit(-60,60);
    for(size_t o:{size_t(0),size_t(4),size_t(8),size_t(0x10),size_t(0x14),size_t(0x18),size_t(0x20),size_t(0x24),size_t(0x28)})At<float>(m.data(),o)=r.unit(-1,1);
    At<void*>(e,0x14)= withMatrix ? static_cast<void*>(m.data()) : nullptr;
    for(size_t o:{size_t(4),size_t(8),size_t(0xC),size_t(0x10)})At<float>(e,o)=r.unit(-60,60);
    return e; }
inline void SetupFakeCamera(Rng& r){
    std::memset(fakeCamMat.data(),0,fakeCamMat.size());
    for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(fakeCamMat.data(),o)=r.unit(-60,60);
    AtA<void*>(kCamMatrixPtr)=fakeCamMat.data(); }
inline json CaseEffect(Rng& r,int which){   // 0 Money, 1 Collectable, 2 Mine
    SetupFakeCamera(r); AtA<uint32>(kTimeMs)=r.next()>>(r.below(3));
    const int k=int(r.below(uint32(kFakeEnt))); void* e=FakeEntity(r,k,true);   // siempre con matriz: sin ella GetMatrix() reservaria una matriz REAL del pool
    std::vector<uint8> mat0(fakeEntMat[size_t(k)].begin(),fakeEntMat[size_t(k)].end());
    Patches P;
    P.Add(0x70BA00,S_Shadow); P.Add(D.shadow,S_Shadow); P.Add(0x6FC580,S_Corona); P.Add(D.corona,S_Corona);
    reinterpret_cast<void(__cdecl*)(void*)>(which==0 ? 0x454E80 : which==1 ? 0x455E20 : 0x4560E0)(e);
    json o=json::object(); o["mat"]=Fnv(fakeEntMat[size_t(k)].data(),0x60); o["ent"]=Fnv(e,0x40);
    return o; }

// ---------------- CreateSomeMoney / GenerateNewOne_WeaponType ----------------
inline json CaseCreateSomeMoney(Rng& r){
    FillPickups(r,40,false); FillPickGlobals(r); gGenerateCounter=0;
    CVector c{r.unit(-200,200),r.unit(-200,200),r.unit(-20,100)}; const int32 amount=r.pickI({1,19,20,39,40,99,100,139,140,500,5000,100000,int64_t(r.below(3000))});
    AtA<int16>(0x8CD59C)=int16(r.pickI({0,100,1212,19999}));
    Patches P; AddRandStubs(P); P.Add(0x5696C0,S_GroundZ); P.Add(D.groundZ,S_GroundZ); AddGenerateStub(P);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)(CVector,int32)>(0x458970)(c,amount);
    return PickState(before); }
inline json CaseGenerateWeapon(Rng& r){
    FillPickups(r,40,false); FillPickGlobals(r); gGenerateCounter=0;
    CVector c{r.unit(-200,200),r.unit(-200,200),r.unit(-20,100)};
    const int32 wt=r.pickI({0,22,24,28,30,34,35,38,41,43,45,46,int64_t(r.below(47))}); char msgBuf[8]{};
    Patches P; AddGenerateStub(P);
    const auto before=PickBefore();
    const int32 res=reinterpret_cast<int32(__cdecl*)(CVector,int32,int32,uint32,uint32,char*)>(0x457380)(c,wt,int32(1+r.below(22)),r.below(500),r.below(2),r.pct(50) ? msgBuf : nullptr);
    json o=PickState(before); o["res"]=res; return o; }

// ---------------- coleccionables y fotografia ----------------
inline void SetupStats(Rng& r,uint32 a,uint32 b){
    gStats.fill(0.0f); const float v=float(r.below(60)); gStats[a]=v; gStats[b]= r.pct(40) ? v : float(r.below(60)); gStats[0xA6]=0; }
inline json CaseCollectable(Rng& r,int which){   // 0 Oyster, 1 HorseShoe
    FillPickups(r,30,false); FillPickGlobals(r);
    AtA<int32>(kMoney0)=int32(r.next()>>8);
    if(which==0)SetupStats(r,0xF3,0xF4); else { SetupStats(r,0xF1,0xF2); gStats[0xF2]=r.pickF({1.0f,5.0f,float(1+r.below(50)),gStats[0xF1]<1.0f ? 1.0f : gStats[0xF1]}); }
    Patches P; AddStatStubs(P);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)()>(which==0 ? 0x4552D0 : 0x455390)();
    json o=PickState(before); o["money"]=AtA<int32>(kMoney0); return o; }
inline json CasePictureTaken(Rng& r){
    FillPickups(r,50,true); FillPickGlobals(r); SetupFakeCamera(r); SetupFakePlayer(r);
    AtA<int32>(kMoney0)=int32(r.next()>>8); AtA<int32>(kMoney0+0x190)=int32(r.next()>>8);
    AtA<uint8>(kMirrorActive)=uint8(r.below(2)); gFov=r.pickF({20.0f,45.0f,70.0f,0.0f,r.unit(5,120)}); gSphereBits=r.next(); gSphereCalls=0;
    for(int k=0;k<12;++k){ uint8* p=reinterpret_cast<uint8*>(kPickups)+r.below(620)*0x20; At<uint8>(p,0x1C)=20; for(int c=0;c<3;++c)At<int16>(p,0x10+2*size_t(c))=int16(int32(r.below(801))-400); }
    SetupStats(r,0xE7,0xE8);
    Patches P; AddStatStubs(P); AddRemoveStubs(P);
    P.Add(0x50AD20,S_Fov); P.Add(D.fov,S_Fov); P.Add(0x420C40,S_Sphere); P.Add(D.sphereVis,S_Sphere);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)()>(0x456A70)();
    json o=PickState(before); o["money0"]=AtA<int32>(kMoney0); o["money1"]=AtA<int32>(kMoney0+0x190); return o; }

// ---------------- GivePlayerGoodiesWithPickUpMI ----------------
inline json CaseGoodies(Rng& r){
    FillPickups(r,10,false); FillPickGlobals(r); SetupFakePlayer(r);
    AtA<int16>(0x8CD5BC)=int16(r.pickI({700,701})); AtA<int16>(0x8CD5C0)=int16(r.pickI({702,703})); AtA<int16>(0x8CD5C4)=704; AtA<int16>(0x8CD5C8)=705;
    AtA<int16>(0x8CD5CC)=706; AtA<int16>(0x8CD5D0)=707; AtA<int16>(0x8CD5D4)=708; AtA<int16>(0x8CD750)=709; AtA<int16>(0x8CD754)=710;
    const int32 pid=r.pickI({0,0,0,1});
    AtA<uint8>(kPlayers+0x190*uint32(pid)+0x14F)=uint8(r.pickI({0,50,100,150,255}));     // m_nMaxHealth (0xB7CEE7)
    AtA<uint8>(kPlayers+0x190*uint32(pid)+0x150)=uint8(r.pickI({0,50,100,150,255}));     // m_nMaxArmour (0xB7CEE8)
    if(pid==1)AtA<void*>(kPlayers+0x190)=fakePed.data();
    AtA<uint32>(kTimeMs)=r.next()>>2; gWanted=r.pickI({0,1,2,3,6}); gStats.fill(0.0f);
    const uint32 models[]{700,701,702,703,704,705,706,707,708,709,710,0,1,372,0xFFFF};
    const uint32 model=models[r.below(uint32(sizeof(models)/sizeof(models[0])))];
    Patches P; AddStatStubs(P);
    P.Add(0x55D030,S_UpdHealth); P.Add(D.updHealth,S_UpdHealth); P.Add(0x60A530,S_ResetSprint); P.Add(D.resetSprint,S_ResetSprint);
    P.Add(0x41BE60,S_GetWanted); P.Add(D.getWanted,S_GetWanted); P.Add(0x609F10,S_SetWanted); P.Add(D.setWanted,S_SetWanted);
    P.Add(0x4552D0,S_PickOyster); P.Add(D.pickOyster,S_PickOyster); P.Add(0x455390,S_PickHorse); P.Add(D.pickHorse,S_PickHorse);
    const auto before=PickBefore();
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(uint32,int32)>(0x4564F0)(model,pid)&0xFF;
    json o=PickState(before); o["al"]=al; o["model"]=model; o["pid"]=pid;
    o["ped"]=Fnv(fakePed.data(),0x600); o["pdata"]=Fnv(fakePlayerData.data(),0x100); return o; }

// ---------------- RenderPickUpText ----------------
constexpr size_t kFakeTexts=16;
alignas(16) inline std::array<std::array<uint8,64>,kFakeTexts> fakeText{};
inline uint64 TextHash(const void* p){ if(!p)return 0; const auto* b=static_cast<const uint8*>(p); size_t n=0; while(n<63&&b[n])++n; return Fnv(b,n)^n; }
inline void __cdecl S_FProp(uint32 on){ if(Mine())T("font_proportional",on&0xFF); }
inline void __cdecl S_FBack(uint32 a,uint32 b){ if(Mine())T("font_background",a&0xFF,b&0xFF); }
inline void __cdecl S_FScale(float x,float y){ if(Mine())T("font_scale",x,y); }
inline void __cdecl S_FOrient(uint32 a){ if(Mine())T("font_orientation",a&0xFF); }
inline void __cdecl S_FCentre(float v){ if(Mine())T("font_centre",v); }
inline void __cdecl S_FColor(uint32 rgba){ if(Mine())T("font_color",rgba); }
inline void __cdecl S_FStyle(uint32 s){ if(Mine())T("font_style",s&0xFF); }
inline void __cdecl S_FPrint(float x,float y,const void* text){ if(Mine())T("font_print",x,y,U(text),TextHash(text)); }
inline void __cdecl S_Insert(const void* src,int32 a,int32 b,int32 c,int32 d,int32 e,int32 f,void* dst){
    if(!Mine())return; T("insert_number",U(src),TextHash(src),a,b,c,d,e,f,U(dst));
    if(dst){ auto* o=static_cast<uint8*>(dst); const uint64 h=TextHash(src); for(int k=0;k<6;++k)o[k]=uint8(0x41+((h>>(k*5))&15)); o[6]=0; } }
inline json CaseRenderText(Rng& r){
    FillPickups(r,20,false); FillPickGlobals(r);
    for(size_t k=0;k<kFakeTexts;++k){ auto& t=fakeText[k]; std::memset(t.data(),0,t.size()); const uint32 n=r.below(12); for(uint32 c=0;c<n;++c)t[c]=uint8(0x41+r.below(26)); }
    const uint32 count=r.pickI({0,1,2,5,16,int64_t(r.below(17))});
    AtA<uint16>(kNumMessages)=uint16(count);
    for(uint32 k=0;k<16;++k){ uint8* m=reinterpret_cast<uint8*>(kMessages)+k*0x24; FillBytes(m,0x24,r);
        At<float>(m,0)=r.unit(0,640); At<float>(m,4)=r.unit(0,448); At<float>(m,8)=r.unit(0,50);
        At<float>(m,0xC)=r.pickF({0.0f,5.0f,30.0f,60.0f,200.0f,r.unit(0,300)}); At<float>(m,0x10)=r.pickF({0.0f,5.0f,30.0f,60.0f,200.0f,r.unit(0,300)});
        At<uint8>(m,0x18)=uint8(r.below(4)); At<uint32>(m,0x1C)= r.pct(40) ? r.below(400000) : 0u;
        At<void*>(m,0x20)= r.pct(25) ? nullptr : static_cast<void*>(fakeText[r.below(uint32(kFakeTexts))].data()); }
    AtA<int32>(0xC17044)=r.pickI({640,800,1024,1280,1366,1600,1920,2560,3840,1000,int64_t(320+int32(r.below(2000)))});
    std::memset(reinterpret_cast<void*>(0xB71670),0x55,64); std::memset(reinterpret_cast<void*>(0xC1B100),0x66,64);
    Patches P;
    P.Add(0x7195B0,S_FProp); P.Add(D.fSetProp,S_FProp); P.Add(0x7195C0,S_FBack); P.Add(D.fSetBack,S_FBack); P.Add(0x719380,S_FScale); P.Add(D.fSetScale,S_FScale);
    P.Add(0x719610,S_FOrient); P.Add(D.fSetOrient,S_FOrient); P.Add(0x7194E0,S_FCentre); P.Add(D.fSetCentre,S_FCentre); P.Add(0x719430,S_FColor); P.Add(D.fSetColor,S_FColor);
    P.Add(0x719490,S_FStyle); P.Add(D.fSetStyle,S_FStyle); P.Add(0x71A700,S_FPrint); P.Add(D.fPrint,S_FPrint); P.Add(0x69DE90,S_Insert); P.Add(D.insertNum,S_Insert);
    const auto before=PickBefore();
    reinterpret_cast<void(__cdecl*)()>(0x455000)();
    json o=PickState(before); o["gstring"]=Fnv(reinterpret_cast<void*>(0xB71670),64); o["ggxt"]=Fnv(reinterpret_cast<void*>(0xC1B100),64); return o; }

// regiones globales propias de CPickups
inline void SnapPickupRegions(){
    Snap(kPickRegion,kPickRegionSize); Snap(kDisplayHelp,1); Snap(kMoney0,4); Snap(kMoney0+0x190,4);
    Snap(kPlayers,0x190*2); Snap(kPlayerInFocus,1); Snap(kCamMatrixPtr,4); Snap(kMirrorActive,1); Snap(kTimeMs,4); Snap(kPrevTimeMs,4);
    Snap(0x8CD59C,2); Snap(0x8CD5BC,0x40); Snap(0x8CD750,8); Snap(0xB73458+0x10A,2);
    Snap(kModelInfoPtrs+4u*uint32(kMiBase),4u*uint32(kMiCount)); Snap(0xC17044,8); Snap(0xB71670,64); Snap(0xC1B100,64); }
}
