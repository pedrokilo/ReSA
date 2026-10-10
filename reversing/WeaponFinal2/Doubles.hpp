#pragma once
// Fixtures y dobles comunes del banco WeaponFinal2 (nueve funciones de CWeapon que PickWeap1 dejo fuera).
// Todo objeto del juego es FALSO (memoria del banco con desplazamientos del EXE) o un global guardado/restaurado:
//   - entidades/peds/vehiculos/objetos falsos con vtable falsa (ranura 0x2C del vehiculo = BurstTyre, ranura 4 del objeto = SetIsStatic,
//     ranura 4 de la tarea = GetTaskType), inteligencia falsa por ped y datos de jugador falsos;
//   - lista de referencias (CReferences::pEmptyList) intercambiada por una lista falsa para que los constructores/destructores REALES
//     de los eventos (CEventGunShot, CEventDamage, ...) registren referencias sobre entidades falsas;
//   - cada colaborador externo se dobla con traza en la direccion del EXE y en el cuerpo de la DLL (Patches::Add).
// Los tres dobles que distinguen ABI (valor de retorno CVector, tColLighting) tienen version EXE y version DLL distintas.
#include "Common.hpp"

namespace WeaponFinal2Live {

constexpr uintptr_t kTimeMs=0xB7CB84, kPrevTimeMs=0xB7CB78, kPlayers=0xB7CD98, kPlayerInFocus=0xB7CD74;
constexpr uintptr_t kActiveCamIdx=0xB6F081, kCamMode=0xB6F1A8, kCamFront=0xB6F32C, kCamSource=0xB6F338, kCamStride=0x238;
constexpr uintptr_t kIgnoreEntity=0xB7CD68, kIncludeBikers=0xB7CD6F, kIncludeCarTyres=0xB7CD70, kIncludeDeadPeds=0xB7CD71, kSpreadRate=0xB7CD64;
constexpr uintptr_t kFlags73A=0xC8A7C0, kTheCamera=0xB6F028, kCamMatrixPtr=0xB6F03C, kCamPlacePos=0xB6F02C, kCamMatFwd=0xB6F9AC;
constexpr uintptr_t kRsMaxW=0xC17044, kRsMaxH=0xC17048, kProcLevel=0xB7298C, kPedPoolPtr=0xB74490, kObjPoolPtr=0xB7449C, kMoonSize=0x8D4B60, kCrossHair=0xC8A838;
inline uintptr_t CamF(uintptr_t first){ return first+uintptr_t(AtA<uint8>(kActiveCamIdx))*kCamStride; }
inline uint32 U(const void* p){ return uint32(reinterpret_cast<uintptr_t>(p)); }

// ---------------- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch) ----------------
struct Bodies {
    uintptr_t los{}, handleGun{}, oil{}, reportCrime{}, incStat{}, getStat{}, addLight{}, addTrace1{}, addTrace2{}, localDir{}, addAnim{}, blendAnim{}, firstAssoc{}, getAssoc{}, setTime{};
    uintptr_t genDamage{}, inflict{}, bulletHit{}, waterLevel{}, glass{}, addToMoving{}, applyForce{}, objDamage{}, sphere2{}, impactFx{}, bloodFx{}, tyreFx{}, lighting{}, updHit{}, sameGroup{}, skillCur{}, shake{};
    uintptr_t calcResp{}, affects{}, computeDeath{}, computeDamage{}, locBlood{}, locKick{}, simplest{}, active{}, groupAdd{}, globalGroup{}, interesting{}, addBullet{}, camShake{}, doBulletImpact{};
    uintptr_t isLocked{}, pickTarget{}, losClear{}, testSphere{}, addProj{}, removeNotAdd{}, createFx{}, playKill{}, pictureTaken{}, bonePos{}, calcScreen{};
    uintptr_t createMat{}, matCreate{}, matDestroy{}, fxCopy{}, fxPlay{}, fxMust{}, fxConst{}, fxSetMat{}, fxKill{};
};
inline Bodies D;

// ---------------- memoria falsa ----------------
constexpr size_t kEnt=12, kEntSize=0xA20;
alignas(16) inline std::array<std::array<uint8,kEntSize>,kEnt> fE{};          // entidades (ped 0x7C4, vehiculo 0xA18, objeto 0x19C: sobra tamano)
alignas(16) inline std::array<std::array<uint8,0x60>,kEnt> fEM{};            // matriz de cada entidad
alignas(16) inline std::array<std::array<uint8,0x400>,kEnt> fIntel{};        // CPedIntelligence falso de cada ped
alignas(16) inline std::array<std::array<uint8,0x200>,kEnt> fPData{};        // CPlayerPedData falso de cada ped
alignas(16) inline std::array<std::array<uint8,0x40>,16> fTask{};            // tareas falsas (vtable con GetTaskType en la ranura 4)
alignas(16) inline std::array<uint8,0x40> fClump{};                          // RpClump falso: el byte 0 = 2 (rpCLUMP) porque la DLL lo comprueba en GetRpClump
alignas(16) inline std::array<std::array<uint8,0x40>,6> fAssoc{};            // CAnimBlendAssociation falsas
alignas(16) inline std::array<uint8,0x80> fObjInfo{};                        // CObjectInfo falso
alignas(16) inline std::array<uint8,0x80> fFx{};                             // FxSystem_c falso
alignas(16) inline std::array<uint8,0x40> fGroup{};                          // grupo de eventos GLOBAL falso
alignas(16) inline std::array<std::array<uint8,0x40>,8> fMat{};              // RwMatrix falsas
alignas(16) inline std::array<uint8,0x40> fCpOut{};                          // CColPoint de salida (0x2C)
alignas(16) inline std::array<uint8,0x40> fVecs{};                           // vectores auxiliares
alignas(16) inline std::array<CReference,64> fRefs{};                        // pool de referencias falso
inline std::array<void*,96> trapVt{}, vehVt{}, objVt{}, taskVt{};

inline Rng* nullRng=nullptr;
inline bool Chance(uint32 p){ return gRng ? gRng->pct(p) : false; }

// ---------------- referencias ----------------
inline void ResetRefs(){
    for(size_t i=0;i<fRefs.size();++i){ fRefs[i].m_ppEntity=nullptr; fRefs[i].m_pNext= i+1<fRefs.size() ? &fRefs[i+1] : nullptr; }
    CReferences::pEmptyList=fRefs.data(); }
inline json RefState(){
    json o=json::object(); json order=json::array(); int n=0;
    for(auto* q=CReferences::pEmptyList;q&&n<200;q=q->m_pNext,++n)order.push_back(int(q-fRefs.data()));
    o["free"]=order; json ents=json::array();
    for(size_t k=0;k<kEnt;++k){ int c=0; for(auto* q=reinterpret_cast<CEntity*>(fE[k].data())->m_pReferences;q&&c<100;q=q->m_pNext)++c; ents.push_back(c); }
    o["live"]=ents; return o; }

// ---------------- objetos falsos ----------------
inline void* Ent(size_t k){ return fE[k].data(); }
inline uint8* EntB(size_t k){ return fE[k].data(); }
inline CVector RV(Rng& r,float a=80.0f){ return CVector{r.unit(-a,a),r.unit(-a,a),r.unit(-a,a)}; }
// etype: 1 edificio 2 vehiculo 3 ped 4 objeto 5 dummy. status en los 5 bits altos del byte 0x36.
inline void* MakeEnt(Rng& r,size_t k,int etype,int status,bool matrix=true){
    uint8* e=fE[k].data(); std::memset(e,0,kEntSize);
    At<void*>(e,0)= etype==2 ? static_cast<void*>(vehVt.data()) : etype==4 ? static_cast<void*>(objVt.data()) : static_cast<void*>(trapVt.data());
    At<uint8>(e,0x36)=uint8((status<<3)|etype);
    auto& m=fEM[k]; std::memset(m.data(),0,m.size());
    for(size_t o:{size_t(0),size_t(4),size_t(8),size_t(0x10),size_t(0x14),size_t(0x18),size_t(0x20),size_t(0x24),size_t(0x28)})At<float>(m.data(),o)=r.unit(-1,1);
    for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(m.data(),o)=r.unit(-60,60);
    At<void*>(e,0x14)= matrix ? static_cast<void*>(m.data()) : nullptr;
    for(size_t o:{size_t(4),size_t(8),size_t(0xC)})At<float>(e,o)=r.unit(-60,60);
    At<float>(e,0x10)=r.unit(-3.1f,3.1f);
    return e; }
inline CVector EntPos(size_t k){ uint8* e=EntB(k); void* m=At<void*>(e,0x14); const float* p= m ? reinterpret_cast<const float*>(static_cast<uint8*>(m)+0x30) : reinterpret_cast<const float*>(e+4); return CVector{p[0],p[1],p[2]}; }
// ped: puntero de inteligencia y de datos de jugador propios, clump falso, estado y salud
inline uint8* MakePed(Rng& r,size_t k,int pedType,bool matrix=true){
    uint8* p=static_cast<uint8*>(MakeEnt(r,k,3,0,matrix));
    std::memset(fIntel[k].data(),0,fIntel[k].size()); std::memset(fPData[k].data(),0,fPData[k].size());
    At<void*>(p,0x47C)=fIntel[k].data(); At<void*>(p,0x480)=fPData[k].data(); At<int32>(p,0x598)=pedType; At<void*>(p,0x18)=fClump.data();
    At<uint32>(p,0x46C)=0; At<uint32>(p,0x470)=0; At<float>(p,0x540)=r.pickF({100.0f,50.0f,1.0f,0.5f}); At<int32>(p,0x530)=0;
    At<float>(p,0x12C)=r.unit(0.0f,1.0f);
    return p; }
inline uint8* MakeVeh(Rng& r,size_t k,bool matrix=true){
    uint8* v=static_cast<uint8*>(MakeEnt(r,k,2,0,matrix)); At<float>(v,0x4C0)=r.pickF({1000.0f,500.0f,0.0f,-5.0f,0.5f}); At<float>(v,0x8C)=r.pickF({1500.0f,100.0f,0.0f,10.0f,3000.0f,r.unit(0,5000)});
    return v; }
inline void SetPlayerSwap(void* ped){ AtA<void*>(kPlayers)=ped; AtA<uint8>(kPlayerInFocus)=0; }
inline void SetTimes(Rng& r){ const uint32 t=r.next()>>r.below(4); AtA<uint32>(kTimeMs)=t; AtA<uint32>(kPrevTimeMs)=t-r.pickI({0,1,16,50,1000}); }

// ---------------- arma bajo prueba (buffer de 0x20 bytes) ----------------
alignas(16) inline std::array<uint8,0x20> wbuf{};
inline CWeapon* W(){ return reinterpret_cast<CWeapon*>(wbuf.data()); }
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
    At<void*>(wbuf.data(),0x18)= r.pct(50) ? static_cast<void*>(fFx.data()) : nullptr; }
// Tipos validos para CWeaponInfo::GetWeaponInfo(tipo, STD) en la DLL: 0..46 y 49..58 (RAMMEDBYCAR..FLARE); 47/48 hacen assert en la DLL.
inline int32 ValidType(Rng& r){ const int32 v=int32(r.below(57)); return v<=46 ? v : v+2; }
inline int32 SkillType(Rng& r){ return int32(22+r.below(11)); }

// ---------------- vtables falsas ----------------
inline bool gBurstRet{}; inline uint32 gBurstCalls{};
inline bool __fastcall S_BurstTyre(void* self,void*,uint32 comp,uint32 phys){      // ranura 0x2C del vehiculo (call [edx+0B0h]) ret 8
    if(!Mine())return false; const bool v=Chance(60); T("burst_tyre",self,comp&0xFF,phys&0xFF,uint8(v)); return v; }
inline void __fastcall S_SetIsStatic(void* self,void*,uint32 on){                   // ranura 4 del objeto (call [edx+10h]) ret 4
    if(!Mine())return; T("set_is_static",self,on&0xFF);
    auto* e=reinterpret_cast<CEntity*>(self); e->m_bIsStatic=(on&1u)!=0; }
inline uint32 __fastcall S_TaskType(void* self,void*){ return At<uint32>(self,8); } // ranura 4 de la tarea (call [edx+10h]) ret 0
inline void __fastcall S_Trap(void* self,void*){ if(Mine())T("unexpected_virtual",self); }
inline void InitVtables(){
    trapVt.fill(reinterpret_cast<void*>(&S_Trap)); vehVt.fill(reinterpret_cast<void*>(&S_Trap)); objVt.fill(reinterpret_cast<void*>(&S_Trap)); taskVt.fill(reinterpret_cast<void*>(&S_Trap));
    vehVt[44]=reinterpret_cast<void*>(&S_BurstTyre); objVt[4]=reinterpret_cast<void*>(&S_SetIsStatic); taskVt[4]=reinterpret_cast<void*>(&S_TaskType);
    std::memset(fClump.data(),0,fClump.size()); fClump[0]=2;
    for(auto& t:fTask){ std::memset(t.data(),0,t.size()); At<void*>(t.data(),0)=taskVt.data(); } }
inline void* MakeTask(size_t i,uint32 type){ uint8* t=fTask[i].data(); At<uint32>(t,8)=type; return t; }

// ---------------- resumen de eventos (campos reales; el vptr, el relleno y los bits 5..7 del byte 0x25 de CEventDamage, que el constructor no escribe, no se comparan) ----------------
inline json EvSummary(void* evp){
    if(!evp)return nullptr;
    auto* ev=static_cast<CEvent*>(evp); uint8* b=static_cast<uint8*>(evp);
    json o=json::object(); const int t=int(ev->GetEventType()); o["type"]=t;
    o["base"]=json::array({At<int32>(b,4),At<uint8>(b,8),At<uint8>(b,0xC),At<int16>(b,0xE),At<int16>(b,0x10)});
    if(t==int(EVENT_SHOT_FIRED)||t==int(EVENT_SHOT_FIRED_WHIZZED_BY)){
        o["gun"]=json::array({Name(U(At<void*>(b,0x14))),Bits(At<float>(b,0x18)),Bits(At<float>(b,0x1C)),Bits(At<float>(b,0x20)),Bits(At<float>(b,0x24)),Bits(At<float>(b,0x28)),Bits(At<float>(b,0x2C)),At<uint8>(b,0x30)}); }
    else if(t==int(EVENT_DAMAGE)){
        o["dmg"]=json::array({Name(U(At<void*>(b,0x14))),At<uint32>(b,0x18),At<uint32>(b,0x1C),At<uint32>(b,0x20),At<uint8>(b,0x24),uint8(At<uint8>(b,0x25)&0x1F),At<int32>(b,0x28),At<int32>(b,0x2C),Bits(At<float>(b,0x30)),Bits(At<float>(b,0x34)),
            Bits(At<float>(b,0x38)),Bits(At<float>(b,0x3C)),At<uint8>(b,0x40),At<uint8>(b,0x41),At<uint8>(b,0x42),At<uint8>(b,0x43)}); }
    else if(t==int(EVENT_VEHICLE_DAMAGE_WEAPON)){
        o["veh"]=json::array({Name(U(At<void*>(b,0x14))),Name(U(At<void*>(b,0x18))),At<uint32>(b,0x1C)}); }
    return o; }

// ---------------- dobles comunes ----------------
// ProcessLineOfSight del mundo (0x56BA00): cdecl, 12 argumentos. El punto de impacto sale de la plantilla gCpTpl.
inline uint32 gLosCalls{}; inline uint32 gLosPct{50}; inline void* gLosEnt[4]{}; inline std::array<uint8,0x2C> gCpTpl{};
inline bool __cdecl S_Los(const CVector* s,const CVector* e,void* cp,void** hit,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e2,uint32 f,uint32 g,uint32 h){
    if(!Mine()){ if(hit)*hit=nullptr; return false; }
    const uint32 k=gLosCalls++; const bool v=Chance(gLosPct);
    T("los",s->x,s->y,s->z,e->x,e->y,e->z,a&0xFF,b&0xFF,c&0xFF,d&0xFF,e2&0xFF,f&0xFF,g&0xFF,h&0xFF,Name(AtA<uint32>(kIgnoreEntity)),AtA<uint8>(kIncludeBikers),uint8(v));
    if(hit)*hit= v ? gLosEnt[k&3u] : nullptr;
    if(v&&cp)std::memcpy(cp,gCpTpl.data(),gCpTpl.size());
    return v; }
inline void __cdecl S_HandleGunShot(const CVector* a,const CVector* b){ if(Mine())T("birds_gunshot",a->x,a->y,a->z,b->x,b->y,b->z); }
inline void __cdecl S_OilOnFire(const CVector* a,const CVector* b){ if(Mine())T("oil_on_fire",a->x,a->y,a->z,b->x,b->y,b->z); }
inline void __cdecl S_ReportCrime(uint32 type,void* ent,void* ped){ if(Mine())T("report_crime",type,ent,ped); }
inline void __cdecl S_IncStat(uint32 stat,float v){ if(Mine())T("inc_stat",stat,v); }
inline std::array<float,512> gStats{};
inline float __cdecl S_GetStat(uint32 stat){ if(!Mine())return 0.0f; T("get_stat",stat); return gStats[stat&511u]; }
inline void __cdecl S_AddLight(uint32 type,float px,float py,float pz,float dx,float dy,float dz,float rad,float r,float g,float b,uint32 fog,uint32 shadows,void* ent){
    if(Mine())T("add_light",type&0xFF,px,py,pz,dx,dy,dz,rad,r,g,b,fog&0xFF,shadows&0xFF,ent); }
inline void __cdecl S_AddTrace1(const CVector* from,const CVector* to,float radius,uint32 time,uint32 alpha){ if(Mine())T("add_trace_radius",from->x,from->y,from->z,to->x,to->y,to->z,radius,time,alpha&0xFF); }
inline void __cdecl S_AddTrace2(const CVector* from,const CVector* to,uint32 type,void* ent){ if(Mine())T("add_trace_weapon",from->x,from->y,from->z,to->x,to->y,to->z,type,ent); }

// ---- anim ----
inline uint32 gDirSalt{}; inline bool gAssocNull{}; inline uint32 gAssocCalls{};
inline int32 __fastcall S_LocalDir(void* self,void*,const CVector2D* p){       // 0x5DEF60 (const, ret 4)
    if(!Mine())return 0; const int32 v=int32((Bits(p->x)*31u+Bits(p->y)*17u+gDirSalt)&3u); T("local_direction",self,p->x,p->y,v); return v; }
inline void* __cdecl S_AddAnim(void* clump,uint32 group,uint32 id){            // 0x4D3AA0 cdecl (clump, grupo, id)
    if(!Mine())return fAssoc[0].data(); void* res=fAssoc[(gAssocCalls++)%fAssoc.size()].data(); T("add_animation",clump,group,id,res); return res; }
inline void* __cdecl S_BlendAnim(void* clump,uint32 group,uint32 id,float blend){   // 0x4D4610 cdecl (clump, grupo, id, mezcla)
    if(!Mine())return fAssoc[0].data(); void* res= (gAssocNull&&(id==36||id==39)) ? nullptr : static_cast<void*>(fAssoc[(gAssocCalls++)%fAssoc.size()].data());
    T("blend_animation",clump,group,id,blend,res); return res; }
inline void* __cdecl S_FirstAssoc(void* clump,uint32 flags){                     // 0x4D6A70 cdecl (clump, flags)
    if(!Mine())return nullptr; const bool v=Chance(50); void* res= v ? static_cast<void*>(fAssoc[5].data()) : nullptr; T("first_association",clump,flags,res); return res; }
inline void* __cdecl S_GetAssoc(void* clump,uint32 id){                          // 0x4D68B0 cdecl (clump, id)
    if(!Mine())return nullptr; const bool v=Chance(50); void* res= v ? static_cast<void*>(fAssoc[4].data()) : nullptr; T("get_association",clump,id,res); return res; }
inline void __fastcall S_SetTime(void* self,void*,float t){ if(Mine())T("anim_set_current_time",self,t); }   // 0x4CEA80 ret 4
inline void ResetAnim(){ for(auto& a:fAssoc)std::memset(a.data(),0xCD,a.size()); gAssocCalls=0; }

// ---- audio, fx de impacto ----
inline void __fastcall S_BulletHit(void* self,void*,void* ent,uint32 surf,const CVector* pos,float angle){   // 0x506EC0 ret 0x10
    if(Mine())T("report_bullet_hit",ent,surf&0xFF,pos->x,pos->y,pos->z,angle); (void)self; }
inline bool __cdecl S_Water(float x,float y,float z,float* out,uint32 touch,void* normals){                // 0x6EB690 cdecl (x,y,z,&out,tocando,normales)
    if(!Mine())return false; const bool v=Chance(50); T("water_level",x,y,z,touch&0xFF,normals,uint8(v)); if(v&&out)*out=z-10.0f+0.375f; return v; }
inline void __cdecl S_Glass(void* ent,float x,float y,float z){ if(Mine())T("glass_hit_by_bullet",ent,x,y,z); }  // 0x71C0D0 cdecl (entidad, CVector por valor)
inline void __fastcall S_ImpactFx(void* self,void*,const CVector* p,const CVector* d,int32 fxType,int32 amount,float light){   // 0x49F3D0 ret 0x14
    if(Mine())T("bullet_impact_fx",p->x,p->y,p->z,d->x,d->y,d->z,fxType,amount,light); (void)self; }
inline void __fastcall S_BloodFx(void* self,void*,const CVector* o,const CVector* d,int32 amount,float brightness){        // 0x49EB00 ret 0x10
    if(Mine())T("blood_fx",o->x,o->y,o->z,d->x,d->y,d->z,amount,brightness); (void)self; }
inline void __fastcall S_TyreFx(void* self,void*,const CVector* p,const CVector* v){                                       // 0x49F300 ret 8
    if(Mine())T("tyre_burst_fx",p->x,p->y,p->z,v->x,v->y,v->z); (void)self; }
inline float __cdecl S_LightExe(uint32 v,float scale){ if(!Mine())return 0.0f; T("lighting",v&0xFF,scale); return float(v&0xFFu)*0.25f+scale; }                 // 0x59F0C0 cdecl (valor, escala)
inline float __fastcall S_LightDll(void* self,void*,float scale){ if(!Mine())return 0.0f; const uint32 v=*static_cast<uint8*>(self); T("lighting",v&0xFF,scale); return float(v&0xFFu)*0.25f+scale; }   // miembro const: this = el byte
inline bool __fastcall S_Sphere2(void* self,void*,const CVector* o,float rad){ if(!Mine())return false; const bool v=Chance(60); T("camera_sphere_visible",o->x,o->y,o->z,rad,uint8(v)); (void)self; return v; }   // 0x420D40 ret 8

// ---- fisica / estadisticas / jugador ----
inline void __fastcall S_Inflict(void* self,void*,void* damager,uint32 weapon,float intensity,float x,float y,float z){   // 0x6D7C90 ret 0x18
    if(Mine())T("inflict_damage",self,damager,weapon,intensity,x,y,z); }
inline void __fastcall S_ApplyForce(void* self,void*,float fx,float fy,float fz,float ox,float oy,float oz,uint32 flag){    // 0x542B50 ret 0x1C
    if(Mine())T("apply_force",self,fx,fy,fz,ox,oy,oz,flag&0xFF); }
inline void __fastcall S_ObjDamage(void* self,void*,float dmg,const CVector* fxo,const CVector* fxd,void* damager,uint32 type){   // 0x5A0D90 ret 0x14
    if(Mine())T("object_damage",self,dmg,fxo->x,fxo->y,fxo->z,fxd->x,fxd->y,fxd->z,damager,type); }
inline void __fastcall S_ToMoving(void* self,void*){ if(Mine())T("add_to_moving_list",self); }                             // 0x542800
inline void __cdecl S_UpdHit(uint32 type){ if(Mine())T("update_stats_weapon_hit",type); }                                   // 0x55CEB0
inline bool __cdecl S_SameGroup(const void* a,const void* b){ if(!Mine())return false; const bool v=Chance(35); T("same_group",a,b,uint8(v)); return v; }   // 0x5F7F40
inline bool gSkillAllowed{};
inline uint8 __fastcall S_SkillCur(void* self,void*){ if(!Mine())return 1; const uint8 v= gSkillAllowed ? uint8(gRng ? gRng->below(4) : 1u) : uint8(1); T("weapon_skill_current",self,v); return v; }   // 0x5E6580
inline void __fastcall S_Shake(void* self,void*,uint32 time,uint32 freq,float x,float y,float z){ if(Mine())T("start_shake_distance",self,time&0xFFFF,freq&0xFF,x,y,z); }   // 0x53F9A0 ret 0x14
inline void __cdecl S_CamShake(void* cam,float strength){ if(Mine())T("cam_shake_no_pos",cam,strength); }                    // 0x50A970 cdecl

// ---- eventos ----
inline void* __cdecl S_GlobalGroup(){ if(!Mine())return fGroup.data(); T("global_group"); return fGroup.data(); }          // 0x4ABA50
inline void* __fastcall S_GroupAdd(void* self,void*,void* ev,uint32 valid){ if(!Mine())return nullptr; trace.push_back(json::array({"group_add",Name(U(self)),EvSummary(ev),valid&0xFF})); return nullptr; }   // 0x4AB420 ret 8
inline void __fastcall S_Interesting(void* self,void*,uint32 type,void* ent){ if(Mine())T("interesting_event",type,ent); (void)self; }   // 0x602590 ret 8
inline bool __cdecl S_AddBullet(void* creator,uint32 type,float px,float py,float pz,float vx,float vy,float vz){ if(!Mine())return false; T("add_bullet",creator,type,px,py,pz,vx,vy,vz); return true; }   // 0x736010

// ---- tareas por ped ----
inline std::array<void*,kEnt> gTmSimplest{}, gTmActive{};
inline int TmIndex(void* tm){ for(size_t k=0;k<kEnt;++k)if(tm==fIntel[k].data()+4)return int(k); return -1; }
inline void* __fastcall S_Simplest(void* tm,void*){ if(!Mine())return nullptr; const int k=TmIndex(tm); T("get_simplest_task",tm); return k>=0 ? gTmSimplest[size_t(k)] : nullptr; }   // 0x6819D0
inline void* __fastcall S_Active(void* tm,void*){ if(!Mine())return nullptr; const int k=TmIndex(tm); T("get_active_task",tm); return k>=0 ? gTmActive[size_t(k)] : nullptr; }       // 0x681720
inline void ResetTasks(){ gTmSimplest.fill(nullptr); gTmActive.fill(nullptr); }

// ---- registro de nombres de los buffers ----
inline void NameBuffers(){
    static const char* en[]{"e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","e10","e11"};
    static const char* itl[]{"i0","i1","i2","i3","i4","i5","i6","i7","i8","i9","i10","i11"};
    static const char* pd[]{"pd0","pd1","pd2","pd3","pd4","pd5","pd6","pd7","pd8","pd9","pd10","pd11"};
    static const char* tn[]{"t0","t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11","t12","t13","t14","t15"};
    static const char* an[]{"a0","a1","a2","a3","a4","a5"}; static const char* mn[]{"m0","m1","m2","m3","m4","m5","m6","m7"};
    for(size_t k=0;k<kEnt;++k){ Nm(fE[k].data(),kEntSize,en[k]); Nm(fIntel[k].data(),fIntel[k].size(),itl[k]); Nm(fPData[k].data(),fPData[k].size(),pd[k]); }
    for(size_t k=0;k<fTask.size();++k)Nm(fTask[k].data(),fTask[k].size(),tn[k]);
    for(size_t k=0;k<fAssoc.size();++k)Nm(fAssoc[k].data(),fAssoc[k].size(),an[k]);
    for(size_t k=0;k<fMat.size();++k)Nm(fMat[k].data(),fMat[k].size(),mn[k]);
    Nm(wbuf.data(),wbuf.size(),"weapon"); Nm(fFx.data(),fFx.size(),"fx"); Nm(fGroup.data(),fGroup.size(),"ggroup"); Nm(fClump.data(),fClump.size(),"clump");
    Nm(fObjInfo.data(),fObjInfo.size(),"objinfo"); Nm(fCpOut.data(),fCpOut.size(),"cp"); Nm(fVecs.data(),fVecs.size(),"vecs");
    Nm(&CPad::GetPad(0)[0],sizeof(CPad)*2,"pads"); }
}
