#pragma once
// Banco dentro del motor (flujo viejo, estilo Small2/PadMouse1/VehWave2/WorldFinal1/TaskFinal2): las 4 funciones que faltaban para dejar
// COMPLETA la clase CBike:
//   CBike::Constructor (EXE 0x6BF430), CBike::Destructor (0x6B57A0), CBike::DamageKnockOffRider (0x6B5A10, estatica) y la virtual
//   CBike::ProcessEntityCollision (vtable 0x871360, hueco 22 -> 0x6BDEA0).
// Original (hook desactivado: se ejecuta el codigo del EXE) frente a candidato (hook activo) sobre objetos falsos (motos/entidades de
// 0xA18 bytes con vtable falsa y guardas; peatones y inteligencias del tamano de CPed/CPedIntelligence con desplazamientos crudos del
// EXE; tareas falsas con vtable trazada; informacion de modelo falsa con vtable trazada), con fixtures deterministas por caso y la
// traza de todas las llamadas salientes con sus argumentos. Los colaboradores se sustituyen en la direccion del EXE y en el cuerpo
// de la DLL (EntryPatch) y solo trazan en el hilo del banco.
//
// Constructor: CVehicle::CVehicle (0x6D5F10), GetAnimFileIndex (vtable de la informacion de modelo falsa, hueco 14), CVehicle::SetModelIndex
//   (0x6D6A40), CClumpModelInfo::FillFrameArray (0x4C5440), ChooseVehicleColour (0x4C8500), CMemoryMgr::Malloc (0x72F420; solo la primera
//   llamada del hilo del banco, el resto se reenvia al malloc del EXE 0x824257), CBike::SetupSuspensionLines (0x6B89B0) y
//   CAEVehicleAudioEntity::Initialise (0x4F7670). Se ejecutan de verdad: cHandlingDataMgr::Get*Pointer, CAnimManager (bloques de animacion
//   reales, solo lectura), CMatrix::operator= y CTimer. El model info de 6 ranuras (521, 522, 581, 461, 463, 468) se sustituye y se restaura.
// Destructor: CAEVehicleAudioEntity::Terminate (0x4FB8C0), RwMatrixDestroy (0x7F2A20, via CMatrix::~CMatrix de la matriz de inclinacion) y
//   CVehicle::~CVehicle (0x6E2B40).
// DamageKnockOffRider: CPed::GetBikeRidingSkill (0x5DF510), CPed::GetLocalDirection (0x5DEF60), CCullZones::CamStairsForPlayer (0x72DD30),
//   FindZoneWithStairsAttributeForPlayer (0x72DAD0), CEventKnockOffBike::CEventKnockOffBike (0x4AFCF0), CEventGroup::Add (0x4AB420) y
//   ~CEventKnockOffBike (0x4AFE70). Se ejecutan de verdad: CTaskManager::GetActiveTask (0x681720), CPed::IsPlayer (0x5DF8F0) y las llamadas
//   virtuales GetTaskType de las tareas falsas (vtable trazada).
// ProcessEntityCollision: CEntity::GetColModel (0x535300, sin traza: es pura y el EXE y la DLL la llaman un numero distinto de veces),
//   CCollision::ProcessColModels (0x4185C0), CPhysical::AddCollisionRecord (0x543490) y CEntity::RegisterReference (0x571B70). Se ejecutan de
//   verdad: CMatrix::CMatrix(const CMatrix&) / ~CMatrix, Multiply3x3 (0x59C790, la DLL lo transcribe en linea) y el array estatico del EXE
//   (0xC1C280, guardado y restaurado).
//
// El pool de vehiculos, los pools de peatones/tareas/eventos y los archivos reales NUNCA se tocan. Todo el estado global tocado se guarda
// y se restaura (state_restored).
#include <fstream>
#include <set>
#include <cmath>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Bike.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Events/EventKnockOffBike.h"
#include "Events/EventGroup.h"
#include "Collision/Collision.h"
#include "CullZones.h"
#include "Audio/Entities/AEVehicleAudioEntity.h"
#include "Models/ClumpModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Timer.h"
#include "World.h"
#include "../ProbeRuntime.hpp"

namespace BikeWave4Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int32> l){ return l.begin()[next()%l.size()]; } };
constexpr uint32 kQNaN=0x7FC00000u;

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
inline bool IsOther(const json& j){ return j.is_string() && j.get<std::string>()=="other"; }
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

// --- modulo de la DLL y busqueda de cuerpos (como VehWave2 / WorldFinal1) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
// simbolo exportado (las clases con NOTSA_EXPORT_VTABLE exportan constructores, destructores y miembros), seguido hasta el cuerpo
inline uintptr_t Exported(const char* mangled){ if(!modBase)return 0; const auto p=uintptr_t(GetProcAddress(reinterpret_cast<HMODULE>(modBase),mangled)); return p ? FollowIn(p) : 0; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(std::bit_cast<uintptr_t>(f)); }

// ======================= objetos falsos =======================
constexpr size_t kGuard=32, kVehSize=0xA18, kBikeSize=0x814;
struct alignas(16) Veh { uint8 b[kVehSize]; };
alignas(16) inline std::array<uint8,kGuard> guardLo{};
inline std::array<Veh,8> vehs{};                                       // moto (0), entidad (1), conductor de la colision (2), danador y otros
alignas(16) inline std::array<uint8,kGuard> guardHi{};
alignas(16) inline std::array<std::array<uint8,0x60>,3> mats{};        // CMatrix falsos (derecha 0, adelante 0x10, arriba 0x20, posicion 0x30, RwMatrix* 0x40, posee 0x44)
alignas(16) inline std::array<std::array<uint8,0x40>,4> ltms{};        // RwMatrix falsos (solo identidad)
alignas(16) inline std::array<uint8,0x80> frames{};                    // "RwFrame*" falsos (solo identidad)
constexpr size_t kMiSize=0x308;
constexpr std::array<int32,6> kModels{521,522,581,461,463,468};
alignas(16) inline std::array<std::array<uint8,kMiSize>,6> mis{};      // CVehicleModelInfo falsos (vtable falsa)
alignas(16) inline std::array<std::array<uint8,0x40>,3> colms{};       // CColModel falsos: m_pColData en +0x2C (0 = moto/constructor, 1 = entidad, 2 = conductor)
alignas(16) inline std::array<std::array<uint8,0x40>,3> cdatas{};      // CCollisionData falsos: m_nNumLines en +6, m_pLines en +0x10
alignas(16) inline std::array<std::array<uint8,0x80>,2> lineBufs{};    // 0 = lineas previas del model info, 1 = bloque que "reserva" Malloc
alignas(16) inline std::array<uint8,0x340> vstruct{};                  // CVehicleStructure falsa (m_avDummyPos al principio)
alignas(16) inline std::array<float,4> velBuf{}, posBuf{};             // colisionImpactVelocity / colisionPos
alignas(16) inline std::array<uint8,32*0x2C> outBuf{};                 // salida de ProcessEntityCollision (32 CColPoint)
alignas(16) inline std::array<uint8,0x40> zoneDummy{};
constexpr size_t kPedSize=sizeof(CPed), kIntelSize=sizeof(CPedIntelligence);
alignas(16) inline std::array<std::array<uint8,kPedSize>,2> peds{};    // 0 = conductor, 1 = pasajero
alignas(16) inline std::array<std::array<uint8,kIntelSize>,2> intels{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> tasks{};       // tareas falsas: vtable en 0, tipo en +0x1C
inline std::array<void*,96> vt{};                                      // vtable falsa de vehiculos
inline std::array<void*,24> miVt{};                                    // vtable falsa del model info (hueco 14 = GetAnimFileIndex)
inline std::array<void*,16> taskVt{};                                  // vtable falsa de tareas (hueco 4 = GetTaskType)
inline uint8* VP(int i){ return vehs[size_t(i)].b; }
inline void* Mat(int i){ return mats[size_t(i)].data(); }

// estado del caso actual compartido con los dobles
inline uint32 baseSeed{}, goN{}, pcN1{}, pcN2{}, pcSeed{};
inline int32 animIdx{};
inline float skillV[2]{}; inline int32 dirRet{}; inline bool camRet{}; inline void* zoneRet{};
inline std::array<float,4> newRatios{};
inline bool mallocArmed{};
inline int vptrKind{}, vptrKinds[2]{};       // 1 = vtable del EXE (0x871360), 2 = vtable de la DLL; se comprueba aparte para que el json de ambos lados sea comparable
inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
inline void FillBase(void* self,size_t n){ Rng r{baseSeed}; size_t k=4; for(;k+4<=n;k+=4)At<uint32>(self,k)=r.next(); for(;k<n;++k)At<uint8>(self,k)=uint8(r.next()); At<uint32>(self,0)=0x0BADF00Du; }
inline float FiniteF(Rng& r,float a,float b){ return r.pct(8) ? 0.f : r.uni(a,b); }

// registro de modelos de colision para el doble silencioso de CEntity::GetColModel
struct ColReg { const void* obj; void* colm; };
inline std::array<ColReg,4> colReg{}; inline size_t colRegN{};
inline void* colmDefault{};

// ======================= dobles (se llaman en la direccion del EXE y en el cuerpo de la DLL) =======================
inline void __fastcall V_Dtor(void* self,void*,uint32 flags){ if(Mine())T("dtor",self,flags); }
inline void __fastcall V_Trap(void* self,void*){ if(Mine())T("virtual_inesperada",self); }
inline void* __fastcall S_MiTrap(void* self,void*){ if(Mine())T("virtual_mi_inesperada",self); return nullptr; }
inline void* __fastcall S_TaskTrap(void* self,void*){ if(Mine())T("virtual_tarea_inesperada",self); return nullptr; }
inline int32 __fastcall S_AnimIdx(void* self,void*){ if(Mine())T("GetAnimFileIndex",self); return animIdx; }                          // hueco 14 del model info
inline int32 __fastcall S_TaskType(void* self,void*){ if(Mine())T("GetTaskType",self); return At<int32>(self,0x1C); }              // hueco 4 de la tarea

// ---- constructor ----
// CVehicle::CVehicle(eVehicleCreatedBy) : thiscall ret 4 (rellena la base de forma determinista; la matriz es la falsa mats[0])
inline void* __fastcall S_VehCtor(void* self,void*,uint32 createdBy){
    if(Mine())T("CVehicle_ctor",self,createdBy);
    FillBase(self,0x5A0); At<void*>(self,0x14)=Mat(0);
    return self; }
// CVehicle::SetModelIndex(uint32) : thiscall ret 4
inline void __fastcall S_VehSetModel(void* self,void*,uint32 idx){ if(Mine())T("CVehicle_SetModelIndex",self,idx); }
// CClumpModelInfo::FillFrameArray(RpClump*, RwFrame**) : cdecl
inline void __cdecl S_FillFrames(void* clump,void** nodes){ if(Mine()){ T("FillFrameArray",uint32(uintptr_t(clump)),nodes); for(int i=0;i<10;++i)nodes[i]=(i*7+goN)%5 ? &frames[size_t(i)*4] : nullptr; } }
// CVehicleModelInfo::ChooseVehicleColour(uint8&, uint8&, uint8&, uint8&, int32) : thiscall ret 0x14
inline void __fastcall S_Choose(void* self,void*,uint8* a,uint8* b,uint8* c,uint8* d,int32 shift){
    if(Mine())T("ChooseVehicleColour",self,a,b,c,d,shift);
    Rng q{baseSeed^0xC010u}; *a=uint8(q.next()); *b=uint8(q.next()); *c=uint8(q.next()); *d=uint8(q.next()); }
// CMemoryMgr::Malloc(size, hint): cdecl; la DLL solo apila el tamano. Solo se intercepta la primera llamada armada del hilo del banco
inline void* __cdecl S_Malloc(uint32 size){
    if(Mine()&&mallocArmed){ mallocArmed=false; T("Malloc",size); return lineBufs[1].data(); }
    if(Mine())T("malloc_inesperado",size);
    return reinterpret_cast<void*(__cdecl*)(uint32)>(0x824257)(size); }
inline void __fastcall S_SetupSusp(void* self,void*){ if(Mine())T("SetupSuspensionLines",self); }
inline void __fastcall S_AudioInit(void* self,void*,void* entity){ if(Mine())T("AudioInitialise",self,entity); }

// ---- destructor ----
inline void __fastcall S_AudioTerm(void* self,void*){ if(Mine())T("AudioTerminate",self); }
inline void __fastcall S_VehDtorBase(void* self,void*){ if(Mine())T("CVehicle_dtor",self); }
// RwMatrixDestroy(RwMatrix*) : cdecl. Otro hilo: se replica el cuerpo del EXE (liberar en la lista libre de matrices de RenderWare)
inline RwBool __cdecl S_RwMatDestroy(void* m){
    if(!Mine()){ const uintptr_t eng=AtA<uintptr_t>(0xC97B24), off=AtA<uintptr_t>(0xC979BC);
        reinterpret_cast<void(__cdecl*)(void*,void*)>(AtA<uintptr_t>(eng+0x148))(AtA<void*>(eng+off),m); return 1; }
    T("RwMatrixDestroy",m); return 1; }

// ---- DamageKnockOffRider ----
inline float __fastcall S_Skill(void* self,void*){ if(Mine())T("GetBikeRidingSkill",self); return self==peds[0].data() ? skillV[0] : skillV[1]; }       // float en ST0
inline int32 __fastcall S_LocalDir(void* self,void*,const float* pt){ if(Mine())T("GetLocalDirection",self,pt[0],pt[1]); return dirRet; }
inline uint8 __cdecl S_CamStairs(){ if(Mine())T("CamStairsForPlayer"); return camRet ? 1 : 0; }
inline void* __cdecl S_StairsZone(){ if(Mine())T("FindZoneWithStairsAttributeForPlayer"); return zoneRet; }
inline std::array<void*,6> evs{}; inline int nEv{};
/* Se busca del ultimo al primero: la DLL puede reutilizar la misma ranura de pila para los dos eventos (ambos con la misma direccion); el ultimo constructor con esa direccion es el vivo. Ruido de disposicion de pila, no de comportamiento. */
inline int EvId(void* p){ for(int i=nEv-1;i>=0;--i)if(evs[size_t(i)]==p)return i; return -1; }
// CEventKnockOffBike::CEventKnockOffBike(CVehicle*, const CVector&, const CVector&, float, float, uint8, uint8, int32, CPed*, bool, bool) : thiscall ret 0x2C
inline void* __fastcall S_EvCtor(void* self,void*,void* veh,const float* moveSpeed,const float* impactVel,float dmg,float force,uint32 type,uint32 dir,int32 time,void* ped,uint32 victim,uint32 forceOff){
    if(Mine()){ const int id= nEv<6 ? nEv++ : 5; evs[size_t(id)]=self;
        T("EventKnockOffBike_ctor",id,veh,moveSpeed,impactVel,dmg,force,type,dir,time,ped,uint8(victim),uint8(forceOff)); }
    return self; }
// CEventGroup::Add(CEvent*, bool) : thiscall ret 8
inline void* __fastcall S_EvAdd(void* self,void*,void* ev,uint32 valid){ if(Mine())T("EventGroup_Add",self,EvId(ev),uint8(valid)); return ev; }
inline void __fastcall S_EvDtor(void* self,void*){ if(Mine())T("EventKnockOffBike_dtor",EvId(self)); }

// ---- ProcessEntityCollision ----
// CEntity::GetColModel : pura, sin traza (el EXE y la DLL no la llaman el mismo numero de veces)
inline void* __fastcall S_ColQuiet(void* self,void*){ for(size_t i=0;i<colRegN;++i)if(colReg[i].obj==self)return colReg[i].colm; return colmDefault; }
// identifica una matriz: nombre si es un bufer del banco; si no (copia temporal del conductor) hash de sus 4 vectores, el puntero RW y la bandera
inline json MatId(const void* m){ json n=Name(uintptr_t(m)); if(!IsOther(n))return n;
    const auto* b=static_cast<const uint8*>(m); uint64 h=1469598103934665603ull;
    for(size_t off:{size_t(0),size_t(0x10),size_t(0x20),size_t(0x30)})h=Fnv(b+off,12,h);
    h=Fnv(b+0x40,4,h); h=Fnv(b+0x44,1,h);
    return std::format("tmp:{:016x}",h); }
// CCollision::ProcessColModels(const CMatrix&, CColModel&, const CMatrix&, CColModel&, std::array<CColPoint,32>&, CColPoint*, float*, bool) : cdecl
inline int32 __cdecl S_ProcCol(const void* mA,void* cmA,const void* mB,void* cmB,void* out,void* wheelCP,float* ratios,uint32 flag){
    if(!Mine())return 0;
    T("ProcessColModels",MatId(mA),cmA,MatId(mB),cmB,out,wheelCP,ratios,uint8(flag));
    const bool first= wheelCP!=nullptr;
    const int32 n= int32(first ? pcN1 : pcN2);
    Rng q{pcSeed+(first?1u:2u)};
    for(int32 i=0;i<n;++i){ auto* cp=static_cast<uint8*>(out)+0x2C*size_t(i); FillBytes(cp,0x2C,q); if(!first)cp[0x21]= q.pct(30) ? uint8(0) : uint8(1+q.below(9)); }
    if(first){ for(size_t i=0;i<4;++i){ FillBytes(static_cast<uint8*>(wheelCP)+0x2C*i,0x2C,q); ratios[i]=newRatios[i]; } }
    return n; }
inline void __fastcall S_AddColRec(void* self,void*,void* other){ if(Mine())T("AddCollisionRecord",self,other); }
inline void __fastcall S_RegRef(void* self,void*,void** ref){ if(Mine())T("RegisterReference",self,ref); }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,4> fns{{
    {"CBike","Constructor",0x6BF430,0,-1,512},
    {"CBike","Destructor",0x6B57A0,0,-1,256},
    {"CBike","DamageKnockOffRider",0x6B5A10,0,-1,1024},
    {"CBike","ProcessEntityCollision",0,0x871360,22,1024},
}};
inline constexpr size_t kRealFns=4;

// ======================= estado comun =======================
inline json VehJson(int n){ json o=json::object(); for(int i=0;i<n;++i)o["veh"+std::to_string(i)]=Fnv(VP(i),kVehSize);
    o["guard"]=Fnv(guardLo.data(),kGuard,Fnv(guardHi.data(),kGuard)); return o; }
inline json Blocks(const uint8* p,size_t from,size_t to){ json a=json::array(); for(size_t o=from;o<to;o+=0x40)a.push_back(Fnv(p+o,std::min<size_t>(0x40,to-o))); return a; }
inline void InitVeh(int i,Rng& r){ FillBytes(VP(i),kVehSize,r); At<void*>(VP(i),0)=vt.data(); At<void*>(VP(i),0x14)=nullptr; }
inline void FillMat(int i,Rng& r){ auto& m=mats[size_t(i)]; FillBytes(m.data(),m.size(),r);
    for(int row=0;row<4;++row){ for(int c=0;c<3;++c)At<float>(m.data(),0x10*size_t(row)+4*size_t(c))=r.pct(10)?(row==c?1.f:0.f):r.uni(-2.f,2.f); At<uint32>(m.data(),0x10*size_t(row)+12)=0; }
    for(int c=0;c<3;++c)At<float>(m.data(),0x30+4*size_t(c))=r.uni(-200.f,200.f);
    At<uint32>(m.data(),0x40)=0; At<uint32>(m.data(),0x44)=0; }
inline void SetKnownModels(){ for(size_t k=0;k<kModels.size();++k)AtA<void*>(0xA9B0C8+4*uint32(kModels[k]))=mis[k].data(); }

// handling id razonable (los datos de handling reales de 0..209; el flotante de arrastre y la masa son finitos)
inline int32 HandlingId(Rng& r){ return r.pct(70) ? int32(r.below(210)) : r.pickI({0,1,162,163,185,186,187,208,209}); }
// configura la informacion de modelo falsa k (forCtor: tipo de vehiculo casi siempre moto)
inline void BuildMi(size_t k,Rng& r,bool forCtor){
    auto* m=mis[k].data(); FillBytes(m,kMiSize,r);
    At<void*>(m,0)=miVt.data(); At<void*>(m,0x14)=colms[0].data();
    At<uint32>(m,0x3C)= (forCtor ? r.pct(88) : r.pct(92)) ? 9u : (r.pct(60) ? 5u : uint32(r.pickI({0,2,3,7})));
    At<uint16>(m,0x4A)=uint16(HandlingId(r));
    At<float>(m,0x58)= r.pct(60) ? r.pick({0.f,20.f,25.f,30.f,35.5f,45.f,60.f,75.f,85.f,89.99f,-10.f,10.5f,1.f}) : r.uni(-80.f,80.f);
    At<void*>(m,0x5C)=vstruct.data(); }

// ======================= CBike::Constructor =======================
inline std::vector<int32> animIn, animLow, animHigh;
inline json CaseCtor(Rng& r,uintptr_t target){
    std::memset(VP(0),0xCD,kVehSize); baseSeed=r.next()|1u; goN=uint32(r.below(5));
    FillBytes(Mat(0),0x60,r); At<uint32>(Mat(0),0x40)=0; At<uint32>(Mat(0),0x44)=0;   // matriz del vehiculo (con relleno aleatorio: se copia entera, 0x40 bytes)
    const size_t k=r.below(uint32(kModels.size())); const int32 model=kModels[k];
    BuildMi(k,r,true);
    FillBytes(colms[0].data(),0x40,r); At<void*>(colms[0].data(),0x2C)=cdatas[0].data();
    FillBytes(cdatas[0].data(),0x40,r); At<uint8>(cdatas[0].data(),6)=uint8(r.pickI({0,1,2,3,4,5,8}));
    const bool hasLines=r.pct(50); At<void*>(cdatas[0].data(),0x10)= hasLines ? lineBufs[0].data() : nullptr;
    FillBytes(lineBufs[0].data(),0x80,r); std::memset(lineBufs[1].data(),0xCD,0x80);
    { const uint32 c=r.below(100); const std::vector<int32>& pool= (c<45&&!animIn.empty()) ? animIn : (c<70&&!animLow.empty()) ? animLow : (c<90&&!animHigh.empty()) ? animHigh : animIn;
      animIdx= pool.empty() ? 0 : pool[r.below(uint32(pool.size()))]; }
    mallocArmed=true;
    AtA<uint32>(0xB7CB84)=r.pct(10) ? uint32(r.below(100)) : r.next();                   // CTimer::m_snTimeInMilliseconds
    const uint32 createdBy=r.below(5);
    void* ret=reinterpret_cast<void*(__thiscall*)(void*,int32,uint32)>(target)(VP(0),model,createdBy);
    json o=json::object(); o["ret_is_this"]=(ret==VP(0));
    const uint32 vp=At<uint32>(VP(0),0); vptrKind=(vp==0x871360u) ? 1 : (InDll(vp) ? 2 : 0); o["vptr_ok"]=(vptrKind!=0);   // EXE / vtable de la DLL
    At<uint32>(VP(0),0)=0;
    o["blocks"]=Blocks(VP(0),0,kBikeSize); o["tail"]=Fnv(VP(0)+kBikeSize,kVehSize-kBikeSize);
    o["guard"]=Fnv(guardLo.data(),kGuard,Fnv(guardHi.data(),kGuard));
    o["mi"]=Fnv(mis[k].data(),kMiSize); o["colm"]=Fnv(colms[0].data(),0x40); o["cdata"]=Fnv(cdatas[0].data(),0x40);
    o["lines0"]=Fnv(lineBufs[0].data(),0x80); o["lines1"]=Fnv(lineBufs[1].data(),0x80); o["mat"]=Fnv(Mat(0),0x48);
    o["malloc_used"]=!mallocArmed;
    mallocArmed=false;
    return o; }

// ======================= CBike::Destructor =======================
inline json CaseDtor(Rng& r,uintptr_t target){
    InitVeh(0,r);
    At<uint32>(VP(0),0x60C)= r.pct(60) ? uint32(uintptr_t(ltms[r.below(4)].data())) : 0u;          // m_pAttachMatrix de la matriz de inclinacion
    At<uint32>(VP(0),0x610)= r.pct(50) ? 1u : 0u;                                                    // m_bOwnsAttachedMatrix (dword, relleno a cero como lo deja el constructor)
    reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0));
    json o=json::object();
    const uint32 vp=At<uint32>(VP(0),0); vptrKind=(vp==0x871360u) ? 1 : (InDll(vp) ? 2 : 0); o["vptr_ok"]=(vptrKind!=0);
    At<uint32>(VP(0),0)=0;
    /* el CMatrix::~CMatrix de la DLL (Detach) anula m_pAttachMatrix; el destructor inlinado del EXE (0x59ACD0) no: se enmascara ese puntero */
    At<uint32>(VP(0),0x60C)=0;
    o["blocks"]=Blocks(VP(0),0,kBikeSize); o["tail"]=Fnv(VP(0)+kBikeSize,kVehSize-kBikeSize);
    o["guard"]=Fnv(guardLo.data(),kGuard,Fnv(guardHi.data(),kGuard));
    return o; }

// ======================= CBike::DamageKnockOffRider =======================
inline void BuildDkoMatrix(Rng& r){
    float m[3][3]; const uint32 mode=r.below(100);
    auto N=[&](){ return r.pct(40) ? r.uni(-0.1f,0.1f) : 0.f; };
    if(mode<30){ for(int i=0;i<3;++i)for(int c=0;c<3;++c)m[i][c]=(i==c ? 1.f : 0.f)+N(); }
    else if(mode<50){ for(int i=0;i<3;++i)for(int c=0;c<3;++c)m[i][c]=(i==c ? 1.f : 0.f); }
    else if(mode<62){ for(int i=0;i<3;++i)for(int c=0;c<3;++c)m[i][c]=(i==c ? 1.f : 0.f); m[2][2]=-1.f; m[0][0]=r.pct(50)?1.f:-1.f; m[1][1]=-m[0][0]; }
    else { for(int i=0;i<3;++i)for(int c=0;c<3;++c)m[i][c]=r.uni(-1.f,1.f); }
    FillBytes(Mat(0),0x60,r);
    for(int row=0;row<3;++row){ for(int c=0;c<3;++c)At<float>(Mat(0),0x10*size_t(row)+4*size_t(c))=m[row][c]; }
    for(int c=0;c<3;++c)At<float>(Mat(0),0x30+4*size_t(c))=r.uni(-200.f,200.f);
    At<uint32>(Mat(0),0x40)=0; At<uint32>(Mat(0),0x44)=0; }
inline void BuildDkoPed(int k,Rng& r,bool isDriver){
    auto* p=peds[size_t(k)].data(); auto* in=intels[size_t(k)].data(); auto* tk=tasks[size_t(k)].data();
    FillBytes(p,kPedSize,r); FillBytes(in,kIntelSize,r); FillBytes(tk,0x40,r);
    const uint32 cant= r.pct(40) ? 0u : uint32(r.below(4));
    At<uint32>(p,0x474)=(At<uint32>(p,0x474)&~0x18000000u)|(cant<<27);
    At<void*>(p,0x47C)=in;
    At<uint32>(p,0x530)= r.pct(88) ? 50u : uint32(r.pickI({49,0,20,51,2}));
    At<uint32>(p,0x598)= uint32(r.pickI({0,1,4,5,6,6,7,2}));
    for(int s=0;s<5;++s)At<void*>(in,4+4*size_t(s))=nullptr;
    if(r.pct(isDriver ? 70 : 30)){ At<void*>(in,4+4*size_t(r.below(5)))=tk;
        const uint32 c=r.below(100); At<int32>(tk,0x1C)= c<40 ? 1022 : c<62 ? 1008 : int32(r.pickI({0,1000,1009,1021,1023,1100,-1}));   // GANG_DRIVEBY (3FEh), BE_HIT (3F0h), otros
    }
    At<void*>(tk,0)=taskVt.data();
    skillV[k]= r.pct(5) ? std::bit_cast<float>(kQNaN) : r.pick({0.f,0.25f,0.5f,0.75f,1.f,0.1f,0.9f,0.f}); }
inline json CaseDko(Rng& r,uintptr_t target){
    for(int s=0;s<4;++s)InitVeh(s,r);
    nEv=0; evs.fill(nullptr);
    BuildDkoMatrix(r);
    auto* v=VP(0); At<void*>(v,0x14)=Mat(0);
    At<uint8>(v,0x36)=uint8((r.pct(40) ? 0u : uint32(r.pickI({1,2,3,4,5})))<<3 | 2u);                // estado (bits 3..7) y tipo vehiculo
    At<float>(v,0x8C)= r.pct(3) ? 0.f : r.pick({1000.f,300.f,1.f,5000.f,150.f,r.uni(100.f,2000.f)});    // masa
    for(int c=0;c<3;++c)At<float>(v,0x44+4*size_t(c))=r.uni(-2.f,2.f);                                 // velocidad de movimiento (se pasa por referencia al evento)
    At<int16>(v,0x22)= r.pct(25) ? int16(468) : int16(r.pickI({521,522,0,448,461}));                    // MODEL_SANCHEZ
    At<int32>(v,0x594)= r.pct(20) ? 2 : r.pickI({0,9,1,5});                                            // subtipo (2 = quad)
    const bool hasDriver=r.pct(88), hasPass=r.pct(45);
    BuildDkoPed(0,r,true); BuildDkoPed(1,r,false);
    At<void*>(v,0x460)= hasDriver ? static_cast<void*>(peds[0].data()) : nullptr;
    At<void*>(v,0x464)= hasPass ? static_cast<void*>(peds[1].data()) : nullptr;
    /* velocidad de impacto: vectores "bonitos" en los umbrales (0.85, 0.0) o aleatorios finitos */
    { const uint32 c=r.below(100);
      if(c<30){ static const float V[][3]={{0,0.85f,0},{0,0.8500001f,0},{0,-0.85f,0},{0.5f,0.5f,0.7f},{0,0,1},{0,0,0.85f},{0,0,0.9f},{1,0,0},{0,1,0},{0,-1,0},{0.3f,0.4f,0.5f},{0,0,0},{-0.2f,0.9f,0.86f},{0.6f,0.6f,0.6f},{0,0.84999996f,0}};
          const auto& s=V[r.below(uint32(sizeof(V)/sizeof(V[0])))]; for(int i=0;i<3;++i)velBuf[size_t(i)]=s[i]; }
      else { const float sc=r.pick({0.1f,1.f,3.f,10.f,0.5f}); for(int i=0;i<3;++i)velBuf[size_t(i)]=r.uni(-1.f,1.f)*sc; }
      velBuf[3]=0.f; for(int i=0;i<3;++i)posBuf[size_t(i)]=r.uni(-100.f,100.f); posBuf[3]=0.f; }
    /* intensidad del dano: fuerza base ~ umbrales 10 / 20 / 75 */
    const float mass=At<float>(v,0x8C);
    const float fb= r.pct(35) ? r.pick({0.f,5.f,10.f,10.0001f,20.f,75.f,80.f,150.f,300.f,15.f,60.f}) : r.uni(0.f,420.f);
    float dmg= fb*mass/800.f; if(r.pct(2))dmg=-dmg;
    dirRet=int32(r.below(4)); camRet=r.pct(30); zoneRet= r.pct(60) ? static_cast<void*>(zoneDummy.data()) : nullptr;
    const uint16 piece=uint16(r.next());
    const uint8 ret=reinterpret_cast<uint8(__cdecl*)(void*,float,uint16,void*,const float*,const float*)>(target)(VP(0),dmg,piece,VP(3),posBuf.data(),velBuf.data());
    json o=VehJson(4); o["ret"]=ret;
    for(size_t k=0;k<2;++k){ o["ped"+std::to_string(k)]=Fnv(peds[k].data(),kPedSize); o["intel"+std::to_string(k)]=Fnv(intels[k].data(),kIntelSize); o["task"+std::to_string(k)]=Fnv(tasks[k].data(),0x40); }
    o["vel"]=Fnv(velBuf.data(),16); o["pos"]=Fnv(posBuf.data(),16); o["events"]=nEv;
    return o; }

// ======================= CBike::ProcessEntityCollision =======================
inline json CasePec(Rng& r,uintptr_t target){
    for(int s=0;s<3;++s)InitVeh(s,r);
    nEv=0;
    FillMat(0,r); FillMat(1,r);
    const size_t k=r.below(uint32(kModels.size()));
    BuildMi(k,r,false);
    FillBytes(vstruct.data(),vstruct.size(),r); for(int i=0;i<45;++i)At<float>(vstruct.data(),4*size_t(i))=r.uni(-3.f,3.f);
    for(size_t c=0;c<3;++c){ FillBytes(colms[c].data(),0x40,r); At<void*>(colms[c].data(),0x2C)=cdatas[c].data(); FillBytes(cdatas[c].data(),0x40,r); }
    At<uint8>(cdatas[0].data(),6)=uint8(r.pickI({0,0,4,4,4,2,1}));
    At<uint8>(cdatas[2].data(),6)=uint8(r.pct(70) ? 0 : r.pickI({1,3,4}));
    colRegN=0; colReg[colRegN++]={VP(0),colms[0].data()}; colReg[colRegN++]={VP(1),colms[1].data()}; colReg[colRegN++]={VP(2),colms[2].data()};
    auto* b=VP(0); auto* e=VP(1);
    At<void*>(b,0x14)=Mat(0); At<void*>(e,0x14)=Mat(1);
    At<int16>(b,0x22)=int16(kModels[k]);
    At<uint8>(b,0x36)=uint8((uint32(r.pickI({0,1,2,2,3,4,5}))<<3) | 2u);
    { uint32 pf=r.next(); pf&=~0x9000u; if(r.pct(15))pf|=0x1000u; if(r.pct(15))pf|=0x8000u; At<uint32>(b,0x40)=pf; }
    { uint32 f=r.next(); if(r.pct(50))f&=~0x1000u; At<uint32>(b,0x1C)=f; }
    At<void*>(b,0x460)= r.pct(60) ? static_cast<void*>(VP(2)) : nullptr;
    At<uint8>(b,0x7B6)=uint8(r.pickI({0,1,1,2,5,255}));
    for(size_t i=0;i<4;++i){ float v; const uint32 c=r.below(100);
        v= c<18 ? 1.f : c<28 ? 0.f : c<36 ? 1.5f : c<42 ? std::bit_cast<float>(kQNaN) : c<55 ? 0.5f : r.uni(0.f,1.2f);
        At<float>(b,0x710+4*i)=v;
        float nv; const uint32 d=r.below(100); const float old=v;
        nv= d<15 ? old : d<25 ? 1.f : d<33 ? std::bit_cast<float>(kQNaN) : d<45 ? 1.5f : d<55 ? 0.f : r.uni(0.f,1.2f);
        newRatios[i]=nv; }
    for(size_t i=0;i<4;++i)At<void*>(b,0x7C0+4*i)= r.pct(40) ? nullptr : static_cast<void*>(VP(3));
    { const uint32 t=r.below(100); const uint32 ty= t<22 ? 1u : t<45 ? 2u : t<65 ? 3u : t<90 ? 4u : uint32(r.pickI({0,5,6,7}));
      At<uint8>(e,0x36)=uint8((r.below(6)<<3) | ty); }
    { uint32 f=r.next(); At<uint32>(e,0x1C)=f; uint32 pf=r.next(); if(r.pct(40))pf|=4u; else pf&=~4u; At<uint32>(e,0x40)=pf; }
    for(int c=0;c<3;++c)At<float>(e,4+4*size_t(c))=r.uni(-300.f,300.f);
    std::memset(outBuf.data(),0xCD,outBuf.size());
    pcN1= r.pct(20) ? 0u : r.pct(25) ? uint32(r.pickI({29,30,31,32})) : r.below(33);
    pcN2= r.pct(15) ? 0u : r.pct(30) ? uint32(r.pickI({1,2,3,30,31,32})) : r.below(33);
    pcSeed=r.next();
    const int32 ret=reinterpret_cast<int32(__thiscall*)(void*,void*,void*)>(target)(b,e,outBuf.data());
    json o=VehJson(3); o["ret"]=ret; o["out"]=Fnv(outBuf.data(),outBuf.size()); o["bike_blk"]=Blocks(b,0x400,kBikeSize);
    o["cdata0"]=Fnv(cdatas[0].data(),0x40); o["cdata1"]=Fnv(cdatas[1].data(),0x40); o["cdata2"]=Fnv(cdatas[2].data(),0x40);
    o["mat0"]=Fnv(Mat(0),0x48); o["mat1"]=Fnv(Mat(1),0x48);
    return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"bikewave4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"bikewave4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bikewave4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"bikewave4-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,kRealFns> hooks{};
    for(size_t f=0;f<kRealFns;++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (TODOS antes de crear ningun EntryPatch: leccion 11) ---
    const uintptr_t dllVehCtor=Exported("??0CVehicle@@QAE@W4eVehicleCreatedBy@@@Z");
    const uintptr_t dllVehDtor=Exported("??1CVehicle@@UAE@XZ");
    const uintptr_t dllEvCtor=Exported("??0CEventKnockOffBike@@QAE@PAVCVehicle@@ABVCVector@@1MMEEHPAVCPed@@_N3@Z");
    const uintptr_t dllEvDtor=Exported("??1CEventKnockOffBike@@UAE@XZ");
    const uintptr_t dllVehSetModel=DllBody("CVehicle","SetModelIndex");
    const uintptr_t dllSetupSusp=DllBody("CBike","SetupSuspensionLines");
    const uintptr_t dllFill=Body(&CClumpModelInfo::FillFrameArray);
    const uintptr_t dllChoose=Body(&CVehicleModelInfo::ChooseVehicleColour);
    // CMemoryMgr::Malloc de la DLL: Bike.cpp llama a la sobrecarga de 1 argumento, que reenvia a la de 2 (cuerpo propio de la DLL, no pasa por 0x72F420); hay que interceptar las dos entradas
    const uintptr_t dllMalloc1=Body(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc)), dllMalloc2=Body(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc));
    const uintptr_t dllAudioInit=Body(&CAEVehicleAudioEntity::Initialise), dllAudioTerm=Body(&CAEVehicleAudioEntity::Terminate);
    const uintptr_t dllRwMatDestroy=Body(&RwMatrixDestroy);
    const uintptr_t dllSkill=Body(&CPed::GetBikeRidingSkill), dllLocalDir=Body(&CPed::GetLocalDirection);
    const uintptr_t dllCamStairs=Body(&CCullZones::CamStairsForPlayer), dllStairsZone=Body(&CCullZones::FindZoneWithStairsAttributeForPlayer);
    const uintptr_t dllEvAdd=Body(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add));
    const uintptr_t dllColModel=Body(&CEntity::GetColModel), dllProcCol=Body(&CCollision::ProcessColModels), dllAddColRec=Body(&CPhysical::AddCollisionRecord);
    const uintptr_t dllRegRef=Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    if(!dllVehCtor||!dllVehDtor||!dllEvCtor||!dllEvDtor||!dllVehSetModel||!dllSetupSusp){
        Write({{"event","error"},{"reason","DLL body of a collaborator not found"},{"vehCtor",dllVehCtor},{"vehDtor",dllVehDtor},{"evCtor",dllEvCtor},{"evDtor",dllEvDtor},{"setModel",dllVehSetModel},{"setupSusp",dllSetupSusp}}); return; }

    std::array<bool,kRealFns> enabled{},locked{};
    for(size_t f=0;f<kRealFns;++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtables falsas y nombres de buferes
    for(auto& v:vt)v=reinterpret_cast<void*>(&V_Trap); vt[0]=reinterpret_cast<void*>(&V_Dtor);
    for(auto& v:miVt)v=reinterpret_cast<void*>(&S_MiTrap); miVt[14]=reinterpret_cast<void*>(&S_AnimIdx);
    for(auto& v:taskVt)v=reinterpret_cast<void*>(&S_TaskTrap); taskVt[4]=reinterpret_cast<void*>(&S_TaskType);
    colmDefault=colms[2].data();
    if(named.empty()){
        static const char* vn[8]{"veh0","veh1","veh2","veh3","veh4","veh5","veh6","veh7"};
        for(size_t i=0;i<vehs.size();++i)Nm(vehs[i].b,kVehSize,vn[i]);
        static const char* mn[3]{"mat0","mat1","mat2"}; for(size_t i=0;i<mats.size();++i)Nm(mats[i].data(),0x60,mn[i]);
        static const char* ln[4]{"ltm0","ltm1","ltm2","ltm3"}; for(size_t i=0;i<ltms.size();++i)Nm(ltms[i].data(),0x40,ln[i]);
        static const char* in[6]{"mi0","mi1","mi2","mi3","mi4","mi5"}; for(size_t i=0;i<mis.size();++i)Nm(mis[i].data(),kMiSize,in[i]);
        static const char* cn[3]{"colm0","colm1","colm2"}; for(size_t i=0;i<colms.size();++i)Nm(colms[i].data(),0x40,cn[i]);
        static const char* dn[3]{"cdata0","cdata1","cdata2"}; for(size_t i=0;i<cdatas.size();++i)Nm(cdatas[i].data(),0x40,dn[i]);
        static const char* bn[2]{"lines0","lines1"}; for(size_t i=0;i<lineBufs.size();++i)Nm(lineBufs[i].data(),0x80,bn[i]);
        static const char* pn[2]{"pedD","pedP"}; for(size_t i=0;i<peds.size();++i)Nm(peds[i].data(),kPedSize,pn[i]);
        static const char* sn[2]{"intelD","intelP"}; for(size_t i=0;i<intels.size();++i)Nm(intels[i].data(),kIntelSize,sn[i]);
        static const char* tn[2]{"taskD","taskP"}; for(size_t i=0;i<tasks.size();++i)Nm(tasks[i].data(),0x40,tn[i]);
        Nm(frames.data(),frames.size(),"frames"); Nm(vstruct.data(),vstruct.size(),"vstruct"); Nm(velBuf.data(),16,"vel"); Nm(posBuf.data(),16,"pos");
        Nm(outBuf.data(),outBuf.size(),"out"); Nm(zoneDummy.data(),zoneDummy.size(),"zone"); }

    // bloques de animacion reales (solo lectura): grupos dentro de [2, 6] (ANIM_GROUP_BIKES..WAYFARER), por debajo y por encima
    { const uint32 n=AtA<uint32>(0xB4EA30);
      for(uint32 i=0;i<n&&i<180;++i){ const int32 g=AtA<int32>(0xB5D4A0+32*i+0x1C); (g>=2&&g<=6 ? animIn : g<2 ? animLow : animHigh).push_back(int32(i)); } }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { auto Chk=[&](json& lay,const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      auto Sem=[&](json& lay,const char* n,bool got){ Chk(lay,n,got ? 1u : 0u,1u); };
      json lay=json::object();
      auto* Bk=reinterpret_cast<CBike*>(VP(0));
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      Chk(lay,"CBike.m_aBikeNodes",Off(Bk,Bk->m_aBikeNodes.data()),0x5A0); Chk(lay,"CBike.m_bLeanMatrixCalculated",Off(Bk,&Bk->m_bLeanMatrixCalculated),0x5C8);
      Chk(lay,"CBike.m_mLeanMatrix",Off(Bk,&Bk->m_mLeanMatrix),0x5CC); Chk(lay,"CBike.nBikeFlags",Off(Bk,&Bk->nBikeFlags),0x614);
      Chk(lay,"CBike.m_vecOldSpeedForPlayback",Off(Bk,&Bk->m_vecOldSpeedForPlayback),0x630); Chk(lay,"CBike.m_BikeHandling",Off(Bk,&Bk->m_BikeHandling),0x63C);
      Chk(lay,"CBike.m_RideAnimData",Off(Bk,&Bk->m_RideAnimData),0x640); Chk(lay,"CBike.m_nWheelStatus",Off(Bk,Bk->m_nWheelStatus.data()),0x65C);
      Chk(lay,"CBike.m_aWheelColPoints",Off(Bk,Bk->m_aWheelColPoints.data()),0x660); Chk(lay,"CBike.m_aWheelRatios",Off(Bk,Bk->m_aWheelRatios.data()),0x710);
      Chk(lay,"CBike.m_aRatioHistory",Off(Bk,Bk->m_aRatioHistory.data()),0x720); Chk(lay,"CBike.m_WheelCounts",Off(Bk,Bk->m_WheelCounts.data()),0x730);
      Chk(lay,"CBike.m_fBrakeCount",Off(Bk,&Bk->m_fBrakeCount),0x740); Chk(lay,"CBike.m_aWheelSkidmarkType",Off(Bk,Bk->m_aWheelSkidmarkType.data()),0x744);
      Chk(lay,"CBike.m_bWheelBloody",Off(Bk,Bk->m_bWheelBloody.data()),0x74C); Chk(lay,"CBike.m_bMoreSkidMarks",Off(Bk,Bk->m_bMoreSkidMarks.data()),0x74E);
      Chk(lay,"CBike.m_aWheelPitchAngles",Off(Bk,Bk->m_aWheelPitchAngles.data()),0x750); Chk(lay,"CBike.m_aWheelAngularVelocity",Off(Bk,Bk->m_aWheelAngularVelocity.data()),0x758);
      Chk(lay,"CBike.m_aWheelSuspensionHeights",Off(Bk,Bk->m_aWheelSuspensionHeights.data()),0x760); Chk(lay,"CBike.m_aWheelOrigHeights",Off(Bk,Bk->m_aWheelOrigHeights.data()),0x768);
      Chk(lay,"CBike.m_fSuspensionLength",Off(Bk,Bk->m_fSuspensionLength.data()),0x770); Chk(lay,"CBike.m_fLineLength",Off(Bk,Bk->m_fLineLength.data()),0x780);
      Chk(lay,"CBike.m_fHeightAboveRoad",Off(Bk,&Bk->m_fHeightAboveRoad),0x790); Chk(lay,"CBike.m_fExtraTractionMult",Off(Bk,&Bk->m_fExtraTractionMult),0x794);
      Chk(lay,"CBike.m_fSwingArmLength",Off(Bk,&Bk->m_fSwingArmLength),0x798); Chk(lay,"CBike.m_fForkYOffset",Off(Bk,&Bk->m_fForkYOffset),0x79C);
      Chk(lay,"CBike.m_fForkZOffset",Off(Bk,&Bk->m_fForkZOffset),0x7A0); Chk(lay,"CBike.m_fSteerAngleTan",Off(Bk,&Bk->m_fSteerAngleTan),0x7A4);
      Chk(lay,"CBike.m_nBrakesOn",Off(Bk,&Bk->m_nBrakesOn),0x7A8); Chk(lay,"CBike.m_fTyreTemp",Off(Bk,&Bk->m_fTyreTemp),0x7AC);
      Chk(lay,"CBike.m_fBrakingSlide",Off(Bk,&Bk->m_fBrakingSlide),0x7B0); Chk(lay,"CBike.m_nFixLeftHand",Off(Bk,&Bk->m_nFixLeftHand),0x7B4);
      Chk(lay,"CBike.m_nFixRightHand",Off(Bk,&Bk->m_nFixRightHand),0x7B5); Chk(lay,"CBike.m_nTestPedCollision",Off(Bk,&Bk->m_nTestPedCollision),0x7B6);
      Chk(lay,"CBike.m_PrevSpeed",Off(Bk,&Bk->m_PrevSpeed),0x7B8); Chk(lay,"CBike.m_BlowUpTimer",Off(Bk,&Bk->m_BlowUpTimer),0x7BC);
      Chk(lay,"CBike.m_aGroundPhysicalPtrs",Off(Bk,Bk->m_aGroundPhysicalPtrs.data()),0x7C0); Chk(lay,"CBike.m_aGroundOffsets",Off(Bk,Bk->m_aGroundOffsets.data()),0x7D0);
      Chk(lay,"CBike.m_Damager",Off(Bk,&Bk->m_Damager),0x800); Chk(lay,"CBike.m_nNoOfContactWheels",Off(Bk,&Bk->m_nNoOfContactWheels),0x804);
      Chk(lay,"CBike.m_NumDriveWheelsOnGround",Off(Bk,&Bk->m_NumDriveWheelsOnGround),0x805); Chk(lay,"CBike.m_NumDriveWheelsOnGroundLastFrame",Off(Bk,&Bk->m_NumDriveWheelsOnGroundLastFrame),0x806);
      Chk(lay,"CBike.m_GasPedalAudioRevs",Off(Bk,&Bk->m_GasPedalAudioRevs),0x808); Chk(lay,"CBike.m_WheelStates",Off(Bk,Bk->m_WheelStates.data()),0x80C);
      Chk(lay,"sizeof(CBike)",uint32(sizeof(CBike)),0x814); Chk(lay,"sizeof(CMatrix)",uint32(sizeof(CMatrix)),0x48);
      Chk(lay,"CVehicle.m_vehicleAudio",Off(Bk,&Bk->m_vehicleAudio),0x138); Chk(lay,"CVehicle.m_pHandlingData",Off(Bk,&Bk->m_pHandlingData),0x384);
      Chk(lay,"CVehicle.m_pFlyingHandlingData",Off(Bk,&Bk->m_pFlyingHandlingData),0x388); Chk(lay,"CVehicle.m_nHandlingFlagsIntValue",Off(Bk,&Bk->m_nHandlingFlagsIntValue),0x38C);
      Chk(lay,"CVehicle.m_autoPilot.m_nTimeToStartMission",Off(Bk,&Bk->m_autoPilot.m_nTimeToStartMission),0x3AC); Chk(lay,"CVehicle.m_autoPilot.m_nCarMission",Off(Bk,&Bk->m_autoPilot.m_nCarMission),0x3BA);
      Chk(lay,"CVehicle.m_autoPilot.m_nTempAction",Off(Bk,&Bk->m_autoPilot.m_nTempAction),0x3BB); Chk(lay,"CVehicle.m_nPrimaryColor",Off(Bk,&Bk->m_nPrimaryColor),0x434);
      Chk(lay,"CVehicle.m_nQuaternaryColor",Off(Bk,&Bk->m_nQuaternaryColor),0x437); Chk(lay,"CVehicle.m_pDriver",Off(Bk,&Bk->m_pDriver),0x460);
      Chk(lay,"CVehicle.m_apPassengers",Off(Bk,Bk->m_apPassengers.data()),0x464); Chk(lay,"CVehicle.m_nNumPassengers",Off(Bk,&Bk->m_nNumPassengers),0x484);
      Chk(lay,"CVehicle.m_pEntityWeAreOn",Off(Bk,&Bk->m_pEntityWeAreOn),0x48C); Chk(lay,"CVehicle.m_fSteerAngle",Off(Bk,&Bk->m_fSteerAngle),0x494);
      Chk(lay,"CVehicle.m_GasPedal",Off(Bk,&Bk->m_GasPedal),0x49C); Chk(lay,"CVehicle.m_BrakePedal",Off(Bk,&Bk->m_BrakePedal),0x4A0);
      Chk(lay,"CVehicle.m_pWhoInstalledBombOnMe",Off(Bk,&Bk->m_pWhoInstalledBombOnMe),0x4CC); Chk(lay,"CVehicle.m_anCollisionLighting",Off(Bk,&Bk->m_anCollisionLighting[0]),0x574);
      Chk(lay,"CVehicle.m_nVehicleType",Off(Bk,&Bk->m_nVehicleType),0x590); Chk(lay,"CVehicle.m_nVehicleSubType",Off(Bk,&Bk->m_nVehicleSubType),0x594);
      Chk(lay,"CPhysical.m_nPhysicalFlags",Off(Bk,&Bk->m_nPhysicalFlags),0x40); Chk(lay,"CPhysical.m_vecMoveSpeed",Off(Bk,&Bk->m_vecMoveSpeed),0x44);
      Chk(lay,"CPhysical.m_fMass",Off(Bk,&Bk->m_fMass),0x8C); Chk(lay,"CPhysical.m_fTurnMass",Off(Bk,&Bk->m_fTurnMass),0x90);
      Chk(lay,"CPhysical.m_fAirResistance",Off(Bk,&Bk->m_fAirResistance),0x98); Chk(lay,"CPhysical.m_fElasticity",Off(Bk,&Bk->m_fElasticity),0x9C);
      Chk(lay,"CPhysical.m_fBuoyancyConstant",Off(Bk,&Bk->m_fBuoyancyConstant),0xA0); Chk(lay,"CPhysical.m_vecCentreOfMass",Off(Bk,&Bk->m_vecCentreOfMass),0xA4);
      Chk(lay,"CPhysical.m_nContactSurface",Off(Bk,&Bk->m_nContactSurface),0xBA);
      Chk(lay,"CEntity.m_matrix",Off(Bk,&Bk->m_matrix),0x14); Chk(lay,"CEntity.m_nModelIndex",Off(Bk,&Bk->m_nModelIndex),0x22);
      auto* Mi=reinterpret_cast<CVehicleModelInfo*>(mis[0].data());
      Chk(lay,"CVehicleModelInfo.m_pColModel",Off(Mi,&Mi->m_pColModel),0x14); Chk(lay,"CVehicleModelInfo.m_nVehicleType",Off(Mi,&Mi->m_nVehicleType),0x3C);
      Chk(lay,"CVehicleModelInfo.m_nHandlingId",Off(Mi,&Mi->m_nHandlingId),0x4A); Chk(lay,"CVehicleModelInfo.m_fBikeSteerAngle",Off(Mi,&Mi->m_fBikeSteerAngle),0x58);
      Chk(lay,"CVehicleModelInfo.m_pVehicleStruct",Off(Mi,&Mi->m_pVehicleStruct),0x5C); Chk(lay,"sizeof(CVehicleModelInfo)",uint32(sizeof(CVehicleModelInfo)),0x308);
      { CColModel* cm=reinterpret_cast<CColModel*>(colms[0].data()); Chk(lay,"CColModel.m_pColData",Off(cm,&cm->m_pColData),0x2C);
        CCollisionData* cd=reinterpret_cast<CCollisionData*>(cdatas[0].data()); Chk(lay,"CCollisionData.m_nNumLines",Off(cd,&cd->m_nNumLines),6); Chk(lay,"CCollisionData.m_pLines",Off(cd,&cd->m_pLines),0x10);
        CColLine* ln=reinterpret_cast<CColLine*>(lineBufs[0].data()); Chk(lay,"CColLine.size",Off(&ln[0],&ln[1]),0x20); Chk(lay,"CColLine.m_vecStart.z",Off(&ln[0],&ln[0].m_vecStart.z),8);
        CColPoint* cp=reinterpret_cast<CColPoint*>(outBuf.data()); Chk(lay,"CColPoint.size",Off(&cp[0],&cp[1]),0x2C); Chk(lay,"CColPoint.m_nSurfaceTypeB",Off(cp,&cp->m_nSurfaceTypeB),0x23);
        Chk(lay,"CColPoint.m_nPieceTypeA",Off(cp,&cp->m_nPieceTypeA),0x21); Chk(lay,"CColPoint.m_nLightingB",Off(cp,&cp->m_nLightingB),0x25); Chk(lay,"CColPoint.m_fDepth",Off(cp,&cp->m_fDepth),0x28);
        CMatrix* mt=reinterpret_cast<CMatrix*>(Mat(0)); Chk(lay,"CMatrix.m_pAttachMatrix",Off(mt,&mt->m_pAttachMatrix),0x40); Chk(lay,"CMatrix.m_bOwnsAttachedMatrix",Off(mt,&mt->m_bOwnsAttachedMatrix),0x44); }
      // semantica de campos con bits (se escribe el valor crudo del EXE y se lee con el miembro de la DLL)
      std::memset(VP(0),0,kVehSize);
      At<uint8>(VP(0),0x36)=uint8((2<<3)|2); Sem(lay,"CEntity.status_simple_bits3_7",Bk->GetStatus()==STATUS_SIMPLE); Sem(lay,"CEntity.type_vehicle_bits0_2",Bk->GetType()==ENTITY_TYPE_VEHICLE);
      At<uint32>(VP(0),0x1C)=0x40000000u; Sem(lay,"CEntity.m_bTunnel_bit30",Bk->m_bTunnel&&!Bk->m_bTunnelTransition);
      At<uint32>(VP(0),0x1C)=0x80000000u; Sem(lay,"CEntity.m_bTunnelTransition_bit31",Bk->m_bTunnelTransition&&!Bk->m_bTunnel);
      At<uint32>(VP(0),0x1C)=0x1000u; Sem(lay,"CEntity.m_bHasHitWall_bit12",Bk->GetHasHitWall());
      At<uint32>(VP(0),0x1C)=0;
      At<uint32>(VP(0),0x40)=0x1000u; Sem(lay,"CPhysical.bSkipLineCol_bit12",Bk->physicalFlags.bSkipLineCol&&!Bk->physicalFlags.bProcessingShift);
      At<uint32>(VP(0),0x40)=0x8000u; Sem(lay,"CPhysical.bProcessingShift_bit15",Bk->physicalFlags.bProcessingShift&&!Bk->physicalFlags.bSkipLineCol);
      At<uint32>(VP(0),0x40)=0x4u; Sem(lay,"CPhysical.bDisableCollisionForce_bit2",Bk->physicalFlags.bDisableCollisionForce);
      At<uint32>(VP(0),0x40)=0;
      At<uint8>(VP(0),0x42B)=1; Sem(lay,"CVehicle.bVehicleColProcessed_byte42B_bit0",Bk->vehicleFlags.bVehicleColProcessed);
      At<uint8>(VP(0),0x42B)=0; At<uint8>(VP(0),0x429)=0x0F; Sem(lay,"CVehicle.byte429_low_nibble",Bk->vehicleFlags.bIsVan&&Bk->vehicleFlags.bIsBus&&Bk->vehicleFlags.bIsBig&&Bk->vehicleFlags.bLowVehicle&&!Bk->vehicleFlags.bComedyControls);
      At<uint8>(VP(0),0x429)=0;
      At<uint8>(VP(0),0x3DB)=0x04; Sem(lay,"CAutoPilot.bAvoidLevelTransitions_byte3DB_bit2",Bk->m_autoPilot.carCtrlFlags.bAvoidLevelTransitions);
      At<uint8>(VP(0),0x3DB)=0;
      { auto* pd=peds[0].data(); std::memset(pd,0,kPedSize); auto* Pd=reinterpret_cast<CPed*>(pd);
        At<uint32>(pd,0x530)=50; Sem(lay,"CPed.IsStateDriving_0x530",Pd->IsStateDriving());
        At<uint32>(pd,0x598)=6; Sem(lay,"CPed.IsCop_0x598",Pd->IsCop());
        At<uint32>(pd,0x598)=1; Sem(lay,"CPed.IsPlayer_0x598",Pd->IsPlayer());
        At<uint32>(pd,0x474)=0x08000000u; Chk(lay,"CPed.CantBeKnockedOffBike_1",uint32(Pd->CantBeKnockedOffBike),1);
        At<uint32>(pd,0x474)=0x10000000u; Chk(lay,"CPed.CantBeKnockedOffBike_2",uint32(Pd->CantBeKnockedOffBike),2);
        At<uint32>(pd,0x474)=0x18000000u; Chk(lay,"CPed.CantBeKnockedOffBike_3",uint32(Pd->CantBeKnockedOffBike),3);
        At<void*>(pd,0x47C)=intels[0].data(); Sem(lay,"CPed.GetIntelligence_0x47C",reinterpret_cast<void*>(Pd->GetIntelligence())==intels[0].data());
        Chk(lay,"CPedIntelligence.m_TaskMgr",Off(intels[0].data(),&Pd->GetTaskManager()),4); Chk(lay,"CPedIntelligence.m_eventGroup",Off(intels[0].data(),&Pd->GetEventGroup()),0x68);
        std::memset(pd,0,kPedSize); }
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        Snap(0xB7CB84,4);        // CTimer::m_snTimeInMilliseconds
        Snap(0xC1C280,0x584);    // array estatico de 32 CColPoint de CBike::ProcessEntityCollision en el EXE (+ su bandera de inicializacion 0xC1C800)
        for(int32 m:kModels)Snap(0xA9B0C8+4*uint32(m),4);
    }
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<kRealFns;++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7F1Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); goN=0; nEv=0; mallocArmed=false; colRegN=0; vptrKind=0;
                hooks[f]->State(candidate);
                const bool switches= fn.address ? ReversingProbe::JumpsToDll(fn.address)==candidate : InDll(AtA<uintptr_t>(fn.vt+4*uint32(fn.slot)))==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                const uintptr_t target= fn.address ? fn.address : AtA<uintptr_t>(fn.vt+4*uint32(fn.slot));
                std::memset(guardLo.data(),0xCD,kGuard); std::memset(guardHi.data(),0xCD,kGuard);
                { Rng rs{stt^0xC0FFEEu}; for(int s=0;s<8;++s)InitVeh(s,rs); }
                SetKnownModels();
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                Patches P;
                if(nm=="Constructor"){
                    P.Add(0x6D5F10,S_VehCtor); P.Add(dllVehCtor,S_VehCtor);
                    P.Add(0x6D6A40,S_VehSetModel); P.Add(dllVehSetModel,S_VehSetModel);
                    P.Add(0x4C5440,S_FillFrames); P.Add(dllFill,S_FillFrames);
                    P.Add(0x4C8500,S_Choose); P.Add(dllChoose,S_Choose);
                    P.Add(0x72F420,S_Malloc); P.Add(dllMalloc1,S_Malloc); P.Add(dllMalloc2,S_Malloc);
                    P.Add(0x6B89B0,S_SetupSusp); P.Add(dllSetupSusp,S_SetupSusp);
                    P.Add(0x4F7670,S_AudioInit); P.Add(dllAudioInit,S_AudioInit);
                    out["state"]=CaseCtor(r,target); }
                else if(nm=="Destructor"){
                    P.Add(0x4FB8C0,S_AudioTerm); P.Add(dllAudioTerm,S_AudioTerm);
                    P.Add(0x6E2B40,S_VehDtorBase); P.Add(dllVehDtor,S_VehDtorBase);
                    P.Add(0x7F2A20,S_RwMatDestroy); P.Add(dllRwMatDestroy,S_RwMatDestroy);
                    out["state"]=CaseDtor(r,target); }
                else if(nm=="DamageKnockOffRider"){
                    P.Add(0x5DF510,S_Skill); P.Add(dllSkill,S_Skill);
                    P.Add(0x5DEF60,S_LocalDir); P.Add(dllLocalDir,S_LocalDir);
                    P.Add(0x72DD30,S_CamStairs); P.Add(dllCamStairs,S_CamStairs);
                    P.Add(0x72DAD0,S_StairsZone); P.Add(dllStairsZone,S_StairsZone);
                    P.Add(0x4AFCF0,S_EvCtor); P.Add(dllEvCtor,S_EvCtor);
                    P.Add(0x4AB420,S_EvAdd); P.Add(dllEvAdd,S_EvAdd);
                    P.Add(0x4AFE70,S_EvDtor); P.Add(dllEvDtor,S_EvDtor);
                    out["state"]=CaseDko(r,target); }
                else {
                    P.Add(0x535300,S_ColQuiet); P.Add(dllColModel,S_ColQuiet);
                    P.Add(0x4185C0,S_ProcCol); P.Add(dllProcCol,S_ProcCol);
                    P.Add(0x543490,S_AddColRec); P.Add(dllAddColRec,S_AddColRec);
                    P.Add(0x571B70,S_RegRef); P.Add(dllRegRef,S_RegRef);
                    out["state"]=CasePec(r,target); }
                const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                out["fp"]=fp; out["trace"]=trace; vptrKinds[candidate ? 1 : 0]=vptrKind;
                RestoreRegions();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            if(nm=="Constructor"||nm=="Destructor")ok=ok&&vptrKinds[0]==1&&vptrKinds[1]==2;   // original = vtable del EXE; candidato = vtable de la DLL
            if(nm=="Constructor")ok=ok&&original["state"]["ret_is_this"]==true&&candidate["state"]["ret_is_this"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                json sd=json::array(); if(original.contains("state")&&candidate.contains("state"))for(auto& [k,v]:original["state"].items())if(candidate["state"][k]!=v)sd.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_differs",sd},
                       {"original",failures<=8 ? original:json(nullptr)},{"candidate",failures<=8 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<kRealFns;++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP;
    for(size_t f=0;f<kRealFns;++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
