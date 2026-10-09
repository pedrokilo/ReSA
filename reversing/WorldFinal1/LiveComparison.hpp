#pragma once
// Banco dentro del motor (flujo viejo, estilo Small2/PadMouse1/VehCtor62): CRope (5 funciones pendientes) y CMonsterTruck (5 pendientes).
// Original (hook de la funcion desactivado, se ejecuta el codigo del EXE) frente a candidato (hook activo) sobre objetos falsos con
// guardas, con fixtures deterministas por caso y la traza de todas las llamadas salientes (con sus argumentos).
//
// CRope (EXE 0x556030/0x556070/0x5569C0/0x556780/0x556800): cuerda falsa de 0x328 bytes y entidades falsas (buferes de 0xA40 bytes con
// vtable falsa: hueco 0 = destructor con borrado, hueco 4 = SetIsStatic) y matrices falsas. Colaboradores simulados y trazados:
// CEntity::RegisterReference (0x571B70), CPhysical::AddToMovingList (0x542800), CWorld::Add/Remove (0x563220/0x563280), operator new de
// CObject (0x5A1EE0), constructor de CObject (0x5A1D70 y el constructor de la DLL, localizado por su simbolo exportado), DefinedState
// (0x734650), CCamera::IsSphereVisible de 3 argumentos (0x420C40), RwRenderStateSet (puntero de funcion del motor RW), RwIm3DTransform /
// RenderIndexedPrimitive / End (0x7EF450/0x7EF550/0x7EF520). La tabla de model infos se sustituye (ranuras 19990..19992 y 406) por
// modelos falsos y se restaura.
//
// CMonsterTruck (vtable 0x8717D8, huecos 17/22/43/44; constructor EXE 0x6C8D60): camion falso de 0x99C bytes. Simulados: CAutomobile::
// CAutomobile (0x6B0A90 y su constructor en la DLL; rellena la base con bytes deterministas), SetupSuspensionLines (0x6C7FB0 y cuerpo de la
// DLL), CAutomobile::PreRender (0x6AAB50), CVehicleModelInfo::GetWheelPosn (0x4C7D20), CVehicle::SetTransmissionRotation (0x6DBBB0) y
// SetComponentRotation (0x6DBA30), CCollision::ProcessColModels (0x4185C0) y CPhysical::AddCollisionRecord (0x543490).
// No se asigna memoria ni se toca ningun pool; todo el estado global tocado se guarda y se restaura (state_restored).
#include <fstream>
#include <set>
#include <cmath>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Rope.h"
#include "Entity/Vehicle/MonsterTruck.h"
#include "Entity/Object/Object.h"
#include "../ProbeRuntime.hpp"

void DefinedState();

namespace WorldFinal1Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int32> l){ return l.begin()[next()%l.size()]; } };

// Hilo del banco: otros hilos del juego pueden llamar a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

// --- buferes con nombre para identificar punteros en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    return "other";
}
template<class V> inline json Jv(V v){
    if constexpr(std::is_floating_point_v<V>) return std::bit_cast<uint32>(float(v));
    else if constexpr(std::is_pointer_v<V>) return Name(reinterpret_cast<uintptr_t>(v));
    else return v;
}
template<class... A> inline void T(const char* tag,A... a){ json e=json::array(); e.push_back(tag); (e.push_back(Jv(a)),...); trace.push_back(std::move(e)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- modulo de la DLL y busqueda de cuerpos (como VehCtor62 / Sys13 / Boat23) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline bool Prologue(uintptr_t f){ const auto* b=reinterpret_cast<const uint8*>(f); if(b[0]==0x8B&&b[1]==0xFF)b+=2; return b[0]==0x55&&b[1]==0x8B&&b[2]==0xEC; }
// primera llamada (E8) a una funcion de la DLL con prologo push ebp / mov ebp,esp
inline uintptr_t FirstDllCall(uintptr_t fn){ fn=FollowIn(fn); if(!InDll(fn))return 0;
    for(uintptr_t p=fn;p<fn+0x300;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(InDll(t)&&Prologue(FollowIn(t)))return FollowIn(t); }
    return 0; }
// simbolo exportado (las clases con NOTSA_EXPORT_VTABLE exportan constructores y miembros), seguido hasta el cuerpo
inline uintptr_t Exported(const char* mangled){ if(!modBase)return 0; const auto p=uintptr_t(GetProcAddress(reinterpret_cast<HMODULE>(modBase),mangled)); return p ? FollowIn(p) : 0; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// cuerpo de la DLL de una funcion enganchada
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }
// cuerpo de la DLL de un puntero a funcion/miembro
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(Addr(f)); }

// ======================= objetos falsos =======================
constexpr size_t kGuard=32, kRopeSize=0x328, kTruckSize=0x99C, kEntSize=0xA40;
alignas(16) inline std::array<uint8,kRopeSize+2*kGuard> ropeRaw{};
alignas(16) inline std::array<uint8,kTruckSize+2*kGuard> truckRaw{};
inline uint8* RopeP(){ return ropeRaw.data()+kGuard; }
inline uint8* TruckP(){ return truckRaw.data()+kGuard; }
struct alignas(16) Ent { uint8 b[kEntSize]; };
inline std::array<Ent,5> ents{};                     // 0 entidad enganchada, 1 objeto recogido, 2 objeto previo, 3 objeto nuevo, 4 entidad de colision
alignas(16) inline std::array<std::array<uint8,0x60>,6> mats{};   // matrices falsas (derecha 0, arriba 0x10, frente 0x20, posicion 0x30)
alignas(16) inline std::array<std::array<uint8,0x40>,3> mis{};    // model infos falsos (+0x14 = CColModel*)
alignas(16) inline std::array<std::array<uint8,0x40>,3> cols{};   // CColModel falsos (+0x2C = CCollisionData*)
alignas(16) inline std::array<std::array<uint8,0x40>,3> datas{};  // CCollisionData falsos (+6 = m_nNumLines)
alignas(16) inline std::array<uint8,32*0x2C> sphereCP{};          // CColPoint[32] del argumento
alignas(16) inline std::array<uint8,0x80> frames{};               // "RwFrame*" falsos (solo se usan como identidad)
inline std::array<void*,80> entVt{};                              // vtable falsa de las entidades
inline void* Mat(int i){ return mats[size_t(i)].data(); }

constexpr int32 kSlotObj=19990, kSlotTruck=19991, kSlotEnt=19992, kSlotDumper=406;

// estado del caso actual compartido con los dobles
inline uint32 ctxA{}, ctxB{};            // guiones numericos de los dobles
inline std::array<bool,4> script{};      // resultados programados (visibilidad / transformaciones)
inline uint32 visN{}, trN{};
inline CVector camPos{};
inline std::array<CVector,4> wheelV{};   // posiciones de rueda del double de GetWheelPosn
inline int32 pcmRet{}; inline std::array<float,4> pcmTouch{}; inline uint32 pcmSeed{};
inline uint32 baseSeed{};
inline uint8* curObj{}; inline size_t curSize{};
inline uint64 ObjHash(){ return Fnv(curObj+4,curSize-4); }   // sin la vptr (distinta en el EXE y la DLL)
inline void FillBase(void* self,size_t n){ Rng r{baseSeed}; size_t k=4; for(;k+4<=n;k+=4)At<uint32>(self,k)=r.next(); for(;k<n;++k)At<uint8>(self,k)=uint8(r.next()); }

// ======================= dobles (se llaman en la direccion del EXE y en el cuerpo de la DLL) =======================
// vtable falsa de entidad: hueco 0 = destructor con borrado (ret 4), hueco 4 = SetIsStatic (ret 4)
inline void __fastcall V_Dtor(void* self,void*,uint32 flags){ if(Mine())T("dtor",self,flags); }
inline void __fastcall V_SetStatic(void* self,void*,uint32 v){ if(Mine())T("set_is_static",self,uint8(v)); }
inline void __fastcall V_Trap(void* self,void*){ if(Mine())T("virtual_inesperada",self); }
// CEntity::RegisterReference(CEntity**) : thiscall ret 4
inline void __fastcall S_Reg(void* self,void*,void** ref){ if(Mine())T("register_reference",self,ref); }
// CPhysical::AddToMovingList() : thiscall
inline void __fastcall S_AddMoving(void* self,void*){ if(Mine())T("add_to_moving_list",self); }
inline void __cdecl S_WorldAdd(void* e){ if(Mine())T("world_add",e); }
inline void __cdecl S_WorldRemove(void* e){ if(Mine())T("world_remove",e); }
// CObject::operator new (0x5A1EE0, cdecl) y constructor (thiscall, ret 8)
inline void* __cdecl S_New(size_t n){ if(Mine())T("object_new",uint32(n)); return &ents[3]; }
inline void* __fastcall S_ObjCtor(void* self,void*,int32 model,uint32 create){
    T("object_ctor",self,model,uint8(create));
    // lo unico que fija el doble: vptr falsa y puntero a matriz (nulo o falso, segun el caso); el resto queda como esta (0xCD)
    At<void*>(self,0)=entVt.data(); At<void*>(self,0x14)=ctxA ? Mat(3) : nullptr;
    return self; }
inline void __cdecl S_Defined(){ if(Mine())T("defined_state"); }
inline bool __fastcall S_Vis(void*,void*,const CVector* o,float r,void* m){
    if(!Mine())return false;
    const bool res=script[visN++&1];
    T("is_sphere_visible",o->x,o->y,o->z,r,uint32(uintptr_t(m)),res);
    return res; }
inline RwBool __cdecl S_RenderState(int state,void* value){ if(Mine())T("render_state",state,uint32(uintptr_t(value))); return TRUE; }
inline void* __cdecl S_Transform(void* verts,uint32 n,void* ltm,uint32 flags){
    if(!Mine())return nullptr;
    const bool res=script[2+(trN++&1)];
    T("im3d_transform",uint32(uintptr_t(verts)),n,uint32(uintptr_t(ltm)),flags,Fnv(verts,size_t(n)*36),res);
    return res ? verts : nullptr; }
inline RwBool __cdecl S_RenderIdx(int prim,void* idx,int n){
    if(!Mine())return TRUE;
    T("im3d_render_indexed",prim,uint32(uintptr_t(idx)),n,Fnv(idx,size_t(n>0?n:0)*2)); return TRUE; }
inline RwBool __cdecl S_Im3dEnd(){ if(Mine())T("im3d_end"); return TRUE; }

// --- CMonsterTruck ---
// CAutomobile::CAutomobile(int32, eVehicleCreatedBy, bool) : thiscall ret 0xC
inline void* __fastcall S_AutoCtor(void* self,void*,int32 model,uint32 createdBy,uint32 flag){
    T("CAutomobile_ctor",self,model,createdBy,uint8(flag));
    FillBase(self,0x988);
    return self; }
inline void __fastcall S_Setup(void* self,void*){ T("SetupSuspensionLines",self,ObjHash()); }
inline void __fastcall S_AutoPreRender(void* self,void*){ if(Mine())T("CAutomobile_PreRender",self); }
inline void __fastcall S_Wheel(void* self,void*,int32 w,CVector& out,bool local){ out=wheelV[size_t(w)&3]; if(Mine())T("GetWheelPosn",self,w,local); }
inline void __fastcall S_Trans(void* self,void*,void* node,float l,float r,CVector pos,bool front){ if(Mine())T("SetTransmissionRotation",self,node,l,r,pos.x,pos.y,pos.z,front); }
inline void __fastcall S_Comp(void* self,void*,void* node,int32 axis,float angle,bool reset){ if(Mine())T("SetComponentRotation",self,node,axis,angle,reset); }
// CCollision::ProcessColModels(8 args, cdecl): escribe distancias y puntos de rueda programados y devuelve el numero programado
inline int32 __cdecl S_Pcm(const void* ma,void* cma,const void* mb,void* cmb,void* sph,void* lineCP,float* touch,bool flag){
    if(!Mine())return 0;
    T("ProcessColModels",ma,cma,mb,cmb,sph,lineCP,touch,flag,Fnv(cma,0x40));
    Rng r{pcmSeed};
    for(int i=0;i<4;++i){ touch[i]=pcmTouch[size_t(i)]; auto* cp=static_cast<uint8*>(lineCP)+0x2C*i; for(int k=0;k<0x2C;++k)cp[k]=uint8(r.next());
        At<float>(cp,0)=r.uni(-50.f,50.f); At<float>(cp,4)=r.uni(-50.f,50.f); At<float>(cp,8)=r.uni(-50.f,50.f); }
    return pcmRet; }
inline void __fastcall S_AddRec(void* self,void*,void* e){ if(Mine())T("AddCollisionRecord",self,e); }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,10> fns{{
    {"CRope","CreateHookObjectForRope",0x556070,0,-1,768},
    {"CRope","PickUpObject",0x5569C0,0,-1,1536},
    {"CRope","ReleasePickedUpObject",0x556030,0,-1,384},
    {"CRope","Remove",0x556780,0,-1,640},
    {"CRope","Render",0x556800,0,-1,1536},
    {"CMonsterTruck","Constructor",0x6C8D60,0,-1,512},
    {"CMonsterTruck","BurstTyre",0,0x8717D8,44,256},
    {"CMonsterTruck","PreRender",0,0x8717D8,17,1536},
    {"CMonsterTruck","ProcessEntityCollision",0,0x8717D8,22,3072},
    {"CMonsterTruck","SetUpWheelColModel",0,0x8717D8,43,128},
}};

// ======================= fixtures =======================
inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
inline float RandF(Rng& r){ const uint32 k=r.below(100); if(k<6)return 0.f; if(k<10)return r.pick({1.f,-1.f,0.5f,120.f,119.99f,1e-30f,3.4e38f,-3.4e38f}); return r.uni(-300.f,300.f); }
inline void FillMat(int i,Rng& r){ auto& m=mats[size_t(i)]; FillBytes(m.data(),m.size(),r);
    for(int row=0;row<4;++row)for(int c=0;c<3;++c)At<float>(m.data(),0x10*row+4*c)=r.pct(10) ? (row==c?1.f:0.f) : r.uni(-2.f,2.f);
    for(int c=0;c<3;++c)At<float>(m.data(),0x30+4*c)=r.uni(-200.f,200.f); }
inline void ResetEntity(int i,Rng& r,uint8 typeBits,uint8 statusBits){
    FillBytes(&ents[size_t(i)],sizeof(Ent),r);
    auto* e=ents[size_t(i)].b;
    At<void*>(e,0)=entVt.data(); At<void*>(e,0x14)=Mat(i);
    At<uint8>(e,0x36)=uint8((statusBits<<3)|(typeBits&7)); At<int16>(e,0x22)=int16(i==1?kSlotObj:i==4?kSlotEnt:0);
    At<uint8>(e,0x48B)=0xFF; }
inline void SetupModel(std::array<uint8,0x40>& mi,std::array<uint8,0x40>& col,std::array<uint8,0x40>& data,float height,uint8 lines){
    std::memset(mi.data(),0xAB,mi.size()); std::memset(col.data(),0xBC,col.size()); std::memset(data.data(),0xCD,data.size());
    At<void*>(mi.data(),0x14)=col.data(); At<void*>(col.data(),0x2C)=data.data(); At<float>(col.data(),0x14)=height; At<uint8>(data.data(),6)=lines; }

inline json Common(){ json o=json::object(); o["rope"]=ReversingProbe::Bytes(RopeP(),kRopeSize); o["rope_guard"]=Fnv(ropeRaw.data(),kGuard,Fnv(ropeRaw.data()+kGuard+kRopeSize,kGuard));
    for(int i=0;i<5;++i)o["ent"+std::to_string(i)]=Fnv(&ents[size_t(i)],sizeof(Ent));
    for(int i=0;i<6;++i)o["mat"+std::to_string(i)]=Fnv(mats[size_t(i)].data(),0x60);
    return o; }

// ---- CRope::ReleasePickedUpObject / Remove / PickUpObject / CreateHookObjectForRope ----
inline void FillRope(Rng& r,bool attachedNonNull,uint8 type){
    FillBytes(RopeP(),kRopeSize,r); std::memset(ropeRaw.data(),0xCD,kGuard); std::memset(ropeRaw.data()+kGuard+kRopeSize,0xCD,kGuard);
    At<uint8>(RopeP(),0x325)=type; At<void*>(RopeP(),0x314)=attachedNonNull ? &ents[0] : nullptr; At<void*>(RopeP(),0x318)=nullptr; }
inline json CaseRelease(Rng& r){
    FillRope(r,true,uint8(r.below(9)));
    ResetEntity(0,r,uint8(r.below(8)),uint8(r.below(32))); ResetEntity(1,r,uint8(r.below(8)),uint8(r.below(32)));
    if(r.pct(70)) At<void*>(RopeP(),0x318)=&ents[1];
    reinterpret_cast<void(__thiscall*)(void*)>(0x556030)(RopeP());
    return Common(); }
inline json CaseRemove(Rng& r,Patches& P,bool dllLeg){
    (void)dllLeg;
    const bool hasObj=r.pct(60); const bool hasAtt=hasObj||r.pct(80);
    FillRope(r,hasAtt,uint8(r.below(9)));
    ResetEntity(0,r,uint8(r.below(8)),uint8(r.below(32))); ResetEntity(1,r,uint8(r.below(8)),uint8(r.below(32)));
    if(hasObj) At<void*>(RopeP(),0x318)=&ents[1];
    (void)P;
    reinterpret_cast<void(__thiscall*)(void*)>(0x556780)(RopeP());
    return Common(); }
inline json CasePick(Rng& r){
    FillRope(r,true,uint8(r.below(9)));
    const uint32 kind=r.below(10);           // 0..3 vehiculo, 4..7 objeto, 8 peaton, 9 edificio/otros
    const uint8 type=kind<4 ? 2 : kind<8 ? 4 : kind==8 ? 3 : uint8(r.pickI({1,5,6,7,0}));
    const uint8 status=r.pct(30) ? 2 : r.pct(30) ? 3 : uint8(r.below(32));
    ResetEntity(0,r,uint8(r.below(8)),uint8(r.below(32))); ResetEntity(1,r,type,status); ResetEntity(2,r,uint8(r.below(8)),uint8(r.below(32)));
    FillMat(0,r); FillMat(1,r); FillMat(2,r);
    if(r.pct(35))At<void*>(&ents[0],0x14)=nullptr;        // la entidad enganchada guarda la posicion en el placement
    uint32 f1c=At<uint32>(&ents[1],0x1C); f1c=r.pct(50) ? (f1c&~0x40004u) : (f1c|(r.pct(50)?4u:0u)|(r.pct(50)?0x40000u:0u)); At<uint32>(&ents[1],0x1C)=f1c;
    const uint32 prior=r.below(10);                        // 0..5 nulo, 6..8 objeto previo, 9 el mismo
    At<void*>(RopeP(),0x318)= prior<6 ? nullptr : prior<9 ? static_cast<void*>(&ents[2]) : static_cast<void*>(&ents[1]);
    SetupModel(mis[0],cols[0],datas[0],RandF(r),0);
    reinterpret_cast<void(__thiscall*)(void*,void*)>(0x5569C0)(RopeP(),&ents[1]);
    json o=Common(); o["kind"]=kind; return o; }
inline json CaseCreate(Rng& r){
    const uint8 type=uint8(r.pickI({0,1,2,3,4,5,6,7,8,9,255,1,2,3,4,5,6,7}));
    FillRope(r,false,type);
    if(r.pct(12))At<void*>(RopeP(),0x314)=&ents[4];
    for(int i=0;i<5;++i)ResetEntity(i,r,uint8(r.below(8)),uint8(r.below(32)));
    FillMat(3,r); std::memset(&ents[3],0xCD,sizeof(Ent));
    for(int c=0;c<3;++c)At<float>(RopeP(),0x174+4*c)=RandF(r);        // segmento 31: flotantes finitos
    ctxA=r.below(2);                                       // 0: el objeto nuevo guarda la posicion en el placement; 1: en una matriz
    reinterpret_cast<void(__thiscall*)(void*)>(0x556070)(RopeP());
    json o=Common(); o["ctx"]=ctxA; return o; }

// ---- CRope::Render ----
inline json CaseRender(Rng& r){
    FillRope(r,false,uint8(r.pickI({6,6,5,1,0,7,8,4})));
    for(int i=0;i<32;++i)for(int c=0;c<3;++c)At<float>(RopeP(),12*i+4*c)=r.uni(-300.f,300.f);
    const uint32 dk=r.below(8);
    const float dx[8]{0.f,10.f,100.f,119.5f,121.f,500.f,-300.f,0.f};
    At<float>(RopeP(),0)=camPos.x+dx[dk]; At<float>(RopeP(),4)=camPos.y+(dk==7?50.f:0.f); At<float>(RopeP(),8)=camPos.z+r.uni(-10.f,10.f);
    if(dk==7&&r.pct(60))At<uint32>(RopeP(),0)=0x7FC00000u;     // NaN silencioso en la distancia (el original sale por el fcomp con NaN)
    const uint32 vv=r.below(4);                                  // visibilidad: 0 la primera, 1 solo espejo, 2 ninguna, 3 la primera (sin espejo)
    script={vv==0||vv==3, vv==1, r.pct(80), r.pct(80)};
    // m_bMirrorActive (0xB6F998): con 1 el original llama de nuevo con la matriz del espejo
    AtA<uint8>(0xB6F998)=uint8(vv==1||vv==2);
    FillBytes(reinterpret_cast<void*>(0xC4D958),0x480,r);
    reinterpret_cast<void(__thiscall*)(void*)>(0x556800)(RopeP());
    json o=Common(); o["verts"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC4D958),0x480); o["vv"]=vv; return o; }

// ---- CMonsterTruck ----
inline void FillTruck(Rng& r){
    FillBytes(TruckP(),kTruckSize,r); std::memset(truckRaw.data(),0xCD,kGuard); std::memset(truckRaw.data()+kGuard+kTruckSize,0xCD,kGuard);
    At<void*>(TruckP(),0)=entVt.data(); At<void*>(TruckP(),0x14)=Mat(0); At<uint8>(TruckP(),0x48B)=0xFF; At<uint8>(TruckP(),0x36)=uint8((r.pct(40)?2u<<3:uint32(r.below(32))<<3)|2u);
    FillMat(0,r);
    // campos que el codigo lee como flotantes: nunca sNaN (el x87 los silenciaria al copiarlos)
    auto F=[&](size_t off){ At<float>(TruckP(),off)= r.pct(6) ? std::bit_cast<float>(0x7FC00000u) : r.pct(3) ? std::bit_cast<float>(0x7F800000u) : RandF(r); };
    for(size_t i=0;i<4;++i){ F(0x838+4*i); F(0x878+4*i); F(0x888+4*i); At<float>(TruckP(),0x7D4+4*i)=RandF(r); } }
inline json TruckCommon(){ json o=json::object(); o["truck"]=ReversingProbe::Bytes(TruckP(),kTruckSize);
    o["guard"]=Fnv(truckRaw.data(),kGuard,Fnv(truckRaw.data()+kGuard+kTruckSize,kGuard));
    for(int i=0;i<5;++i)o["ent"+std::to_string(i)]=Fnv(&ents[size_t(i)],sizeof(Ent));
    for(int i=0;i<6;++i)o["mat"+std::to_string(i)]=Fnv(mats[size_t(i)].data(),0x60);
    for(int i=0;i<3;++i){ o["data"+std::to_string(i)]=Fnv(datas[size_t(i)].data(),0x40); o["col"+std::to_string(i)]=Fnv(cols[size_t(i)].data(),0x40); }
    return o; }

inline json CaseBurst(Rng& r,uintptr_t fnAddr){
    FillTruck(r); const uint32 comp=r.next(), phys=r.next();
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(void*,uint32,uint32)>(fnAddr)(TruckP(),comp,phys)&0xFF;
    json o=TruckCommon(); o["al"]=al; return o; }
inline json CaseSetUpWheel(Rng& r,uintptr_t fnAddr){
    FillTruck(r); void* col=reinterpret_cast<void*>(uintptr_t(r.next()));
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(void*,void*)>(fnAddr)(TruckP(),col)&0xFF;
    json o=TruckCommon(); o["al"]=al; return o; }
inline json CasePreRender(Rng& r,uintptr_t fnAddr){
    FillTruck(r);
    const bool dumper=r.pct(35);
    At<int16>(TruckP(),0x22)=int16(dumper ? kSlotDumper : kSlotTruck);
    for(int i=0;i<25;++i)At<void*>(TruckP(),0x648+4*i)=r.pct(75) ? static_cast<void*>(frames.data()+(r.below(16)*4)) : nullptr;   // m_aCarNodes
    At<uint16>(TruckP(),0x86C)=uint16(r.pct(30) ? r.below(4) : r.next());
    for(auto& v:wheelV)v=CVector{RandF(r),RandF(r),RandF(r)};
    SetupModel(mis[1],cols[1],datas[1],0.f,0);
    reinterpret_cast<void(__thiscall*)(void*)>(fnAddr)(TruckP());
    json o=TruckCommon(); o["dumper"]=dumper; return o; }
inline json CaseCollide(Rng& r,uintptr_t fnAddr){
    FillTruck(r);
    const uint32 etype=r.pickI({2,2,4,4,1,3,5,6});
    ResetEntity(4,r,uint8(etype),uint8(r.below(32)));
    FillMat(4,r);
    At<int16>(TruckP(),0x22)=int16(kSlotTruck); At<int16>(&ents[4],0x22)=int16(kSlotEnt);
    At<uint8>(&ents[4],0x48B)=0xFF;
    uint32 pf=At<uint32>(TruckP(),0x40); pf&=~0x9000u; if(r.pct(20))pf|=0x1000u; if(r.pct(20))pf|=0x8000u; At<uint32>(TruckP(),0x40)=pf;
    uint32 ef=At<uint32>(&ents[4],0x40); ef=r.pct(40) ? (ef|4u) : (ef&~4u); At<uint32>(&ents[4],0x40)=ef;
    uint32 f1c=At<uint32>(TruckP(),0x1C); f1c=r.pct(60) ? (f1c|1u) : (f1c&~1u); At<uint32>(TruckP(),0x1C)=f1c;
    SetupModel(mis[1],cols[1],datas[1],0.f,uint8(r.pickI({0,4,4,2,255,1})));
    SetupModel(mis[2],cols[2],datas[2],0.f,uint8(r.below(8)));
    pcmRet=r.pickI({-2,-1,0,0,1,2,5,32}); pcmSeed=r.next()|1u;
    for(int i=0;i<4;++i){ const float w=At<float>(TruckP(),0x838+4*i); const uint32 k=r.below(10);
        pcmTouch[size_t(i)]= k<2 ? w : k<4 ? w+1.f : k<6 ? w-1.f : k==6 ? std::bit_cast<float>(0x7FC00000u) : RandF(r); }
    std::memset(sphereCP.data(),0xEE,sphereCP.size());
    const int32 ret=reinterpret_cast<int32(__thiscall*)(void*,void*,void*)>(fnAddr)(TruckP(),&ents[4],sphereCP.data());
    json o=TruckCommon(); o["ret"]=ret; o["sphere"]=Fnv(sphereCP.data(),sphereCP.size()); return o; }
inline json CaseCtor(Rng& r){
    std::memset(truckRaw.data(),0xCD,truckRaw.size()); curObj=TruckP(); curSize=kTruckSize; baseSeed=r.next()|1u;
    const int32 model=r.pickI({444,406,0,1,19991,-1,0x7FFF}); const uint32 createdBy=r.below(5);
    void* ret=reinterpret_cast<void*(__thiscall*)(void*,int32,uint32)>(0x6C8D60)(TruckP(),model,createdBy);
    json o=json::object(); o["ret_is_this"]=(ret==TruckP());
    const uint32 vp=At<uint32>(TruckP(),0); o["vptr_ok"]=(vp==0x8717D8u)||InDll(vp);
    At<uint32>(TruckP(),0)=0;                                // vptr normalizada
    o["truck"]=ReversingProbe::Bytes(TruckP(),kTruckSize);
    o["guard"]=Fnv(truckRaw.data(),kGuard,Fnv(truckRaw.data()+kGuard+kTruckSize,kGuard));
    return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"worldfinal1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"worldfinal1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("worldfinal1");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"worldfinal1-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,10> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores ---
    const uintptr_t dllNew=Body(static_cast<void*(*)(size_t)>(&CObject::operator new));
    uintptr_t dllObjCtor=Exported("??0CObject@@QAE@H_N@Z");
    uintptr_t dllAutoCtor{};
    { const uintptr_t truckCtor=FirstDllCall(DllBody("CMonsterTruck","Constructor")); const uintptr_t a=truckCtor ? FirstDllCall(truckCtor) : 0;
      uintptr_t b=FirstDllCall(DllBody("CAutomobile","Constructor")); if(!b)b=Exported("??0CAutomobile@@QAE@HW4eVehicleCreatedBy@@_N@Z");
      if(!a||a!=b){ Write({{"event","error"},{"reason","CAutomobile constructor not found"},{"from_truck",a},{"from_other",b}}); return; } dllAutoCtor=a; }
    if(!dllObjCtor){ Write({{"event","error"},{"reason","CObject constructor (int32,bool) not found in the DLL exports"}}); return; }
    const uintptr_t dllSetup=DllBody("CMonsterTruck","SetupSuspensionLines");
    const uintptr_t dllReg=Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t dllMoving=Body(&CPhysical::AddToMovingList);
    const uintptr_t dllWorldAdd=Body(&CWorld::Add), dllWorldRemove=Body(&CWorld::Remove);
    const uintptr_t dllDefined=Body(&DefinedState);
    const uintptr_t dllWheel=Body(&CVehicleModelInfo::GetWheelPosn);
    const uintptr_t dllTrans=Body(&CVehicle::SetTransmissionRotation), dllComp=Body(&CVehicle::SetComponentRotation);
    const uintptr_t dllPcm=Body(&CCollision::ProcessColModels), dllAddRec=Body(&CPhysical::AddCollisionRecord);
    if(!dllNew||!dllSetup||!dllReg||!dllMoving||!dllWorldAdd||!dllWorldRemove||!dllDefined||!dllWheel||!dllTrans||!dllComp||!dllPcm||!dllAddRec){
        Write({{"event","error"},{"reason","DLL body not found"},{"new",dllNew},{"setup",dllSetup},{"reg",dllReg},{"moving",dllMoving},{"add",dllWorldAdd},{"remove",dllWorldRemove},
               {"defined",dllDefined},{"wheel",dllWheel},{"trans",dllTrans},{"comp",dllComp},{"pcm",dllPcm},{"addrec",dllAddRec}}); return; }
    {   std::set<uintptr_t> all{dllNew,dllObjCtor,dllAutoCtor,dllSetup,dllReg,dllMoving,dllWorldAdd,dllWorldRemove,dllDefined,dllWheel,dllTrans,dllComp,dllPcm,dllAddRec};
        if(all.size()!=14){ Write({{"event","error"},{"reason","DLL bodies overlap"}}); return; } }

    std::array<bool,10> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtable falsa de entidad y nombres de buferes
    for(auto& v:entVt)v=reinterpret_cast<void*>(&V_Trap); entVt[0]=reinterpret_cast<void*>(&V_Dtor); entVt[4]=reinterpret_cast<void*>(&V_SetStatic);
    if(named.empty()){
        Nm(RopeP(),kRopeSize,"rope"); Nm(TruckP(),kTruckSize,"truck"); Nm(frames.data(),frames.size(),"frames"); Nm(sphereCP.data(),sphereCP.size(),"sphere_cp");
        for(size_t i=0;i<ents.size();++i){ static const char* nn[5]{"ent0","ent1","ent2","ent3","ent4"}; Nm(&ents[i],sizeof(Ent),nn[i]); }
        for(size_t i=0;i<mats.size();++i){ static const char* nn[6]{"mat0","mat1","mat2","mat3","mat4","mat5"}; Nm(mats[i].data(),0x60,nn[i]); }
        for(size_t i=0;i<mis.size();++i){ static const char* nm[3]{"mi0","mi1","mi2"}; static const char* nc[3]{"col0","col1","col2"}; static const char* nd[3]{"data0","data1","data2"};
            Nm(mis[i].data(),0x40,nm[i]); Nm(cols[i].data(),0x40,nc[i]); Nm(datas[i].data(),0x40,nd[i]); } }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { auto* R=reinterpret_cast<CRope*>(RopeP()); auto* M=reinterpret_cast<CMonsterTruck*>(TruckP());
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      Chk("CRope.m_pRopeHolder",Off(R,&R->m_pRopeHolder),0x310); Chk("CRope.m_pAttachedEntity",Off(R,&R->m_pAttachedEntity),0x314); Chk("CRope.m_pRopeAttachObject",Off(R,&R->m_pRopeAttachObject),0x318);
      Chk("CRope.m_nTime",Off(R,&R->m_nTime),0x320); Chk("CRope.m_nSegments",Off(R,&R->m_nSegments),0x324); Chk("CRope.m_nType",Off(R,&R->m_nType),0x325); Chk("CRope.m_nFlags1",Off(R,&R->m_nFlags1),0x326);
      Chk("sizeof(CRope)",uint32(sizeof(CRope)),uint32(kRopeSize)); Chk("sizeof(CMonsterTruck)",uint32(sizeof(CMonsterTruck)),uint32(kTruckSize));
      Chk("CMonsterTruck.m_aCarNodes",Off(M,M->m_aCarNodes.data()),0x648); Chk("CMonsterTruck.m_wheelColPoint",Off(M,M->m_wheelColPoint.data()),0x724);
      Chk("CMonsterTruck.m_fWheelsSuspensionCompression",Off(M,M->m_fWheelsSuspensionCompression.data()),0x7D4); Chk("CMonsterTruck.m_wheelPosition",Off(M,M->m_wheelPosition.data()),0x838);
      Chk("CMonsterTruck.m_wMiscComponentAngle",Off(M,&M->m_wMiscComponentAngle),0x86C); Chk("CMonsterTruck.m_aSuspensionSpringLength",Off(M,M->m_aSuspensionSpringLength.data()),0x878);
      Chk("CMonsterTruck.m_aSuspensionLineLength",Off(M,M->m_aSuspensionLineLength.data()),0x888); Chk("CMonsterTruck.field_988",Off(M,M->field_988.data()),0x988);
      Chk("CMonsterTruck.m_fSuspensionRadius",Off(M,&M->m_fSuspensionRadius),0x998); Chk("CMonsterTruck.m_apWheelCollisionEntity",Off(M,M->m_apWheelCollisionEntity.data()),0x8E8);
      Chk("CMonsterTruck.m_vWheelCollisionPos",Off(M,M->m_vWheelCollisionPos.data()),0x8F8);
      { auto* Ph=reinterpret_cast<CPhysical*>(&ents[1]); Chk("CPhysical.m_nFakePhysics",Off(Ph,&Ph->m_nFakePhysics),0xB8); }
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        for(int32 s:{kSlotObj,kSlotTruck,kSlotEnt,kSlotDumper})Snap(0xA9B0C8+4*uint32(s),4);
        Snap(0xB6F998,1);        // TheCamera.m_bMirrorActive
        Snap(0xC4D958,0x480);    // TempBufferVertices (32 vertices)
        Snap(0xC97B24,4);        // puntero a RwGlobals
    }
    const uintptr_t renderFpAddr=AtA<uintptr_t>(0xC97B24)+0x20; const uintptr_t renderFpSaved=AtA<uintptr_t>(renderFpAddr);
    camPos=TheCamera.GetPosition();
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7F1Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); visN=0; trN=0;
                hooks[f]->State(candidate);
                const bool switches= fn.address ? ReversingProbe::JumpsToDll(fn.address)==candidate : InDll(AtA<uintptr_t>(fn.vt+4*uint32(fn.slot)))==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                const uintptr_t target= fn.address ? fn.address : AtA<uintptr_t>(fn.vt+4*uint32(fn.slot));
                Patches P;
                // modelos falsos en las ranuras 19990..19992 y 406 (se restauran con RestoreRegions)
                AtA<void*>(0xA9B0C8+4*uint32(kSlotObj))=mis[0].data(); AtA<void*>(0xA9B0C8+4*uint32(kSlotTruck))=mis[1].data();
                AtA<void*>(0xA9B0C8+4*uint32(kSlotEnt))=mis[2].data(); AtA<void*>(0xA9B0C8+4*uint32(kSlotDumper))=mis[1].data();
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                if(nm=="ReleasePickedUpObject"){ out["state"]=CaseRelease(r); }
                else if(nm=="Remove"){
                    P.Add(0x563280,S_WorldRemove); P.Add(dllWorldRemove,S_WorldRemove);
                    out["state"]=CaseRemove(r,P,candidate); }
                else if(nm=="PickUpObject"){
                    P.Add(0x571B70,S_Reg); P.Add(dllReg,S_Reg); P.Add(0x542800,S_AddMoving); P.Add(dllMoving,S_AddMoving);
                    out["state"]=CasePick(r); }
                else if(nm=="CreateHookObjectForRope"){
                    P.Add(0x5A1EE0,S_New); P.Add(dllNew,S_New); P.Add(0x5A1D70,S_ObjCtor); P.Add(dllObjCtor,S_ObjCtor);
                    P.Add(0x571B70,S_Reg); P.Add(dllReg,S_Reg); P.Add(0x563220,S_WorldAdd); P.Add(dllWorldAdd,S_WorldAdd);
                    out["state"]=CaseCreate(r); }
                else if(nm=="Render"){
                    P.Add(0x420C40,S_Vis); P.Add(0x734650,S_Defined); P.Add(dllDefined,S_Defined);
                    P.Add(0x7EF450,S_Transform); P.Add(0x7EF550,S_RenderIdx); P.Add(0x7EF520,S_Im3dEnd);
                    AtA<uintptr_t>(renderFpAddr)=reinterpret_cast<uintptr_t>(&S_RenderState);
                    out["state"]=CaseRender(r);
                    AtA<uintptr_t>(renderFpAddr)=renderFpSaved; }
                else if(nm=="Constructor"){
                    P.Add(0x6B0A90,S_AutoCtor); P.Add(dllAutoCtor,S_AutoCtor); P.Add(0x6C7FB0,S_Setup); P.Add(dllSetup,S_Setup);
                    out["state"]=CaseCtor(r); }
                else if(nm=="BurstTyre"){ out["state"]=CaseBurst(r,target); }
                else if(nm=="SetUpWheelColModel"){ out["state"]=CaseSetUpWheel(r,target); }
                else if(nm=="PreRender"){
                    P.Add(0x6AAB50,S_AutoPreRender); P.Add(0x4C7D20,S_Wheel); P.Add(dllWheel,S_Wheel);
                    P.Add(0x6DBBB0,S_Trans); P.Add(dllTrans,S_Trans); P.Add(0x6DBA30,S_Comp); P.Add(dllComp,S_Comp);
                    out["state"]=CasePreRender(r,target); }
                else if(nm=="ProcessEntityCollision"){
                    P.Add(0x4185C0,S_Pcm); P.Add(dllPcm,S_Pcm); P.Add(0x543490,S_AddRec); P.Add(dllAddRec,S_AddRec); P.Add(0x571B70,S_Reg); P.Add(dllReg,S_Reg);
                    out["state"]=CaseCollide(r,target); }
                const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                out["fp"]=fp; out["trace"]=trace;
                RestoreRegions();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            if(nm=="Constructor")ok=ok&&original["state"]["vptr_ok"]==true&&original["state"]["ret_is_this"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                json sd=json::array(); if(original.contains("state")&&candidate.contains("state"))for(auto& [k,v]:original["state"].items())if(candidate["state"][k]!=v)sd.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_differs",sd},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    AtA<uintptr_t>(renderFpAddr)=renderFpSaved;
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP&&AtA<uintptr_t>(renderFpAddr)==renderFpSaved;
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
