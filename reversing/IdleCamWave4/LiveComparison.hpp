#pragma once
// Banco dentro del motor (flujo viejo, estilo Small2/PadMouse1/WorldWave2): las 5 funciones pendientes de CIdleCam.
// Original (hook de la funcion desactivado, se ejecuta el codigo del EXE) frente a candidato (hook activo) sobre un CIdleCam falso
// (0x9C bytes con guardas) y la traza de todas las llamadas salientes (con sus argumentos).
//
// CIdleCam::FinaliseIdleCamera (0x50E760), Process (0x522C80), ProcessSlerp (0x5179E0), ProcessTargetSelection (0x517870), Run (0x51D3E0).
//
// Durante TODA la corrida los demas hooks de CIdleCam se dejan desactivados (se restauran al final): asi los colaboradores de la misma
// clase (IsTargetValid, SetTarget, SetTargetPlayer, GetLookAtPositionOnTarget, ProcessIdleCamTicker, Reset...) ejecutan el codigo del EXE en los
// dos lados y la comparacion mide solo el cuerpo de la funcion bajo prueba. Solo el hook de la funcion probada cambia entre original y candidato.
//
// Colaboradores externos doblados (EXE y cuerpo de la DLL, mismo comportamiento y misma traza):
//   - CHandShaker::Process (0x50D930): traza (this, grado) y escribe valores deterministas en gHandShaker[0] (angulo z y matriz de resultado).
//   - CInterestingEvents::InvalidateNonVisibleEvents/GetInterestingEvent/InvalidateEvent (0x6029C0/0x6028A0/0x602960) sobre g_InterestingEvents (0xC0B058).
//   - CEntity::RegisterReference/CleanUpOldReference (0x571B70/0x571A00), FindPlayerPed (0x56E210), CWorld::GetIsLineOfSightClear (0x56A490).
//   - Para Run se doblan sus cuatro llamadas (ProcessTargetSelection, ProcessSlerp, ProcessFOVZoom, FinaliseIdleCamera) y para Process se dobla Run
//     (mutando m_LastFrameProcessed para comprobar que gbCineyCamProcessedOnFrame se lee del campo y no del contador).
// No se doblan: CGeneral::GetATanOfXY (0x53CC70, pura, ST0 con precision completa: la usan igual EXE y DLL), CVector::Normalise/CrossProduct
// (0x59C910/0x59C730, puras), Multiply3x3 (0x59C810, en el EXE sin hook; la DLL lo transcribe) ni CCam::GetVectorsReadyForRW (0x509CE0, el EXE la tiene en linea).
//
// Globales tocados y restaurados: CPad 0, CTimer (0xB7CB4C/5C/84), gIdleCam.ticker (0xB6FE34), byte de InterestingEvents (0xC0B184), camara activa
// y las tres CCam de TheCamera (0xB6F081, 0xB6F19C..0xB6F844), gbCineyCamProcessedOnFrame (0xB6EC40), gHandShaker[0] (0xB6ECA0), CWorld::pIgnoreEntity (0xB7CD68).
// No se asigna memoria ni se toca ningun pool real; todo el estado global tocado se guarda y se restaura (state_restored).
//
// Dominio de los fixtures: nunca NaN/Inf, enteros pequenos como flotantes ni denormales (el SSE de la DLL los trata como 0 con DAZ/FTZ y el x87 del EXE no).
#include <fstream>
#include <set>
#include <cmath>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "IdleCam.h"
#include "InterestingEvents.h"
#include "HandShaker.h"
#include "../ProbeRuntime.hpp"

namespace IdleCamWave4Live {
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
    int32 pickI(std::initializer_list<int64_t> l){ return int32(l.begin()[next()%l.size()]); } };

// Hilo del banco: otros hilos del juego (audio, etc.) pueden llamar a funciones parcheadas; sus llamadas no se trazan.
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
inline void SnapP(const void* p,size_t n){ Snap(reinterpret_cast<uintptr_t>(p),n); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- modulo de la DLL y busqueda de cuerpos ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(Addr(f)); }

// ======================= objetos falsos =======================
constexpr size_t kEntSize=0xA40, kIdleSize=0x9C, kGuard=32, kNumEnts=4;
struct alignas(16) Ent { uint8 b[kEntSize]; };
inline std::array<Ent,kNumEnts> ents{};                         // 0 = jugador falso, 1..3 = objetivos / entidades de eventos
inline std::array<void*,80> entVt{};                            // vtable falsa de las entidades (todo trampa)
alignas(16) inline std::array<std::array<uint8,0x60>,kNumEnts> fmats{};   // matrices falsas (posicion en +0x30)
inline void* FMat(int i){ return fmats[size_t(i)].data(); }
alignas(16) inline std::array<uint8,kIdleSize+2*kGuard> idleRaw{};
alignas(16) inline std::array<uint8,0x300> camRaw{};            // CCam falsa (solo se usan los vectores 0x190..0x1BC)
alignas(16) inline std::array<uint32,6> fakeEvents{};           // dos TInterestingEvent de 0xC bytes (entidad en +8)
inline uint8* IdleP(){ return idleRaw.data()+kGuard; }

// estado del caso actual compartido con los dobles
inline std::array<bool,8> script{}; inline uint32 scN{};        // resultados programados de GetIsLineOfSightClear
inline void* curPlayer{}; inline void* curEvent{}; inline void* realPlayer{};   // realPlayer: lo que devuelve FindPlayerPed a otros hilos
inline float slerpOutX{}, slerpOutZ{}, slerpRet{};
inline float hsAngAfter{};
alignas(16) inline std::array<uint8,0x40> hsMatAfter{};
inline int EntIdx(const void* p){ for(size_t i=0;i<ents.size();++i)if(p==&ents[i])return int(i); return -1; }

// ======================= dobles (se llaman en la direccion del EXE y en el cuerpo de la DLL) =======================
inline void __fastcall V_Trap(void* self,void*){ if(Mine())T("virtual_inesperada",self); }
inline void __fastcall S_Reg(void* self,void*,void** ref){ if(Mine())T("register_reference",self,ref); }
inline void __fastcall S_Clean(void* self,void*,void** ref){ if(Mine())T("clean_up_old_reference",self,ref); }
inline void* __cdecl S_Player(int32){ return Mine() ? curPlayer : realPlayer; }
inline bool __cdecl S_LosClear(const CVector* a,const CVector* b,uint32 f0,uint32 f1,uint32 f2,uint32 f3,uint32 f4,uint32 f5,uint32 f6){
    if(!Mine())return true;
    const bool res=script[scN++&7];
    T("is_line_of_sight_clear",a->x,a->y,a->z,b->x,b->y,b->z,f0&0xFF,f1&0xFF,f2&0xFF,f3&0xFF,f4&0xFF,f5&0xFF,f6&0xFF,Name(AtA<uint32>(0xB7CD68)),res);
    return res; }
// InterestingEvents (this = g_InterestingEvents)
inline void __fastcall S_IeInvNonVisible(void* self,void*){ if(Mine())T("invalidate_non_visible_events",self); }
inline void* __fastcall S_IeGet(void* self,void*){ if(!Mine())return nullptr; T("get_interesting_event",self,curEvent); return curEvent; }
inline void __fastcall S_IeInvalidate(void* self,void*,void* ev){ if(Mine())T("invalidate_event",self,ev); }
// CHandShaker::Process: traza y deja valores deterministas (angulo z y matriz de resultado)
inline void __fastcall S_HsProcess(void* self,void*,float degree){
    if(!Mine())return;
    T("hs_process",self,degree); At<float>(self,8)=hsAngAfter; std::memcpy(static_cast<uint8*>(self)+0x3C,hsMatAfter.data(),0x40); }
// llamadas de Run
inline void __fastcall S_Pts(void* self,void*){ if(Mine())T("process_target_selection",self); }
inline float __fastcall S_Slerp(void* self,void*,float* ox,float* oz){
    if(!Mine())return 0.f;
    *ox=slerpOutX; *oz=slerpOutZ; T("process_slerp",self,ox!=oz); return slerpRet; }
inline void __fastcall S_Fov(void* self,void*,float t){ if(Mine())T("process_fov_zoom",self,t,At<float>(self,0x24)); }
inline void __fastcall S_Finalise(void* self,void*,float x,float y,float s){ if(Mine())T("finalise",self,x,y,s); }
// llamada de Process
inline void __fastcall S_Run(void* self,void*){ if(!Mine())return; T("run",self); At<uint32>(self,0x30)+=7; }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,5> fns{{
    {"CIdleCam","FinaliseIdleCamera",0x50E760,0,-1,640},
    {"CIdleCam","Process",0x522C80,0,-1,512},
    {"CIdleCam","ProcessSlerp",0x5179E0,0,-1,1024},
    {"CIdleCam","ProcessTargetSelection",0x517870,0,-1,1024},
    {"CIdleCam","Run",0x51D3E0,0,-1,384},
}};
// todos los hooks de CIdleCam (los que no son la funcion probada se desactivan durante la corrida)
inline const std::array<const char*,14> kAllIdle{"GetLookAtPositionOnTarget","Init","Reset","ProcessIdleCamTicker","SetTarget","FinaliseIdleCamera","SetTargetPlayer",
    "IsTargetValid","ProcessTargetSelection","ProcessSlerp","ProcessFOVZoom","Run","Process","IdleCamGeneralProcess"};

// ======================= fixtures =======================
inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
// flotante "sano": 0, valores notables o uniforme; nunca NaN/Inf/denormal
inline float Sane(Rng& r){ const uint32 k=r.below(100); if(k<8)return 0.f; if(k<16)return r.pick({1.f,-1.f,0.5f,2.f,90.f,-90.f,1000.f,-0.25f}); return r.uni(-300.f,300.f); }
inline void FillSane(void* p,size_t from,size_t to,Rng& r){ for(size_t o=from;o+4<=to;o+=4)At<float>(p,o)=Sane(r); }
inline CVector SaneVec(Rng& r){ return CVector{Sane(r),Sane(r),Sane(r)}; }
inline void PutVec(void* base,size_t off,const CVector& v){ At<float>(base,off)=v.x; At<float>(base,off+4)=v.y; At<float>(base,off+8)=v.z; }
inline CVector GetVec(void* base,size_t off){ return CVector{At<float>(base,off),At<float>(base,off+4),At<float>(base,off+8)}; }
inline float F(uint32 t){ return float(t); }
inline uint32 PickNow(Rng& r){ switch(r.below(8)){ case 0:return 0; case 1:return 1000; case 2:return 5000; case 3:return 123456; case 4:return 0x7FFFFFFFu; case 5:return 0x80000000u; case 6:return 0xFFFFFFF0u; default:return r.next(); } }
inline void SetClock(uint32 now,uint32 frame,float step){ AtA<uint32>(0xB7CB84)=now; AtA<uint32>(0xB7CB4C)=frame; AtA<float>(0xB7CB5C)=step; }

// CIdleCam falso: todo flotante sano, enteros/booleanos que cada caso fija; guardas 0xCD a los dos lados; m_Cam = camara falsa
inline void MakeIdle(Rng& r){
    FillBytes(idleRaw.data(),idleRaw.size(),r); std::memset(idleRaw.data(),0xCD,kGuard); std::memset(idleRaw.data()+kGuard+kIdleSize,0xCD,kGuard);
    FillSane(IdleP(),4,0x98,r);
    At<uint32>(IdleP(),0x30)=r.next();                 // m_LastFrameProcessed
    At<int32>(IdleP(),0x50)=14; At<int32>(IdleP(),0x54)=0; At<int32>(IdleP(),0x58)=3;
    At<uint8>(IdleP(),0x78)=uint8(r.below(2)); At<uint8>(IdleP(),0x79)=uint8(r.below(2));
    At<int32>(IdleP(),0x90)=int32(r.below(60)); At<int32>(IdleP(),0x94)=int32(r.below(100));
    At<void*>(IdleP(),0)=nullptr;
    At<void*>(IdleP(),0x98)=camRaw.data();
    FillBytes(camRaw.data(),camRaw.size(),r); FillSane(camRaw.data(),0,camRaw.size(),r);
    curPlayer=&ents[0]; curEvent=nullptr; slerpOutX=slerpOutZ=slerpRet=0.f; hsAngAfter=0.f; hsMatAfter.fill(0);
    scN=0; for(auto& b:script)b=r.pct(50);
    fakeEvents.fill(0); }
// entidad falsa: bytes aleatorios, vtable de trampas, matriz propia o posicion en la ficha, tipo y tipo de ped
inline void ResetEntity(int i,Rng& r,uint8 type){
    FillBytes(&ents[size_t(i)],sizeof(Ent),r);
    auto* e=ents[size_t(i)].b;
    At<void*>(e,0)=entVt.data(); At<void*>(e,0x14)=r.pct(50) ? FMat(i) : nullptr;
    At<uint8>(e,0x36)=uint8((At<uint8>(e,0x36)&~7u)|(type&7u));
    At<int32>(e,0x598)=r.pickI({5,22,0,1,7,31,4,6,21,23,24,3});
    FillSane(FMat(i),0,0x60,r); FillSane(e,4,0x14,r); }
inline void SetEntPos(int i,const CVector& p){
    auto* e=ents[size_t(i)].b; void* m=At<void*>(e,0x14);
    PutVec(m ? static_cast<void*>(static_cast<uint8*>(m)+0x30) : static_cast<void*>(e+4),0,p); }
// entidades 0..n-1 alrededor de la fuente de la camara a distancias notables (limites 4 y 80 de IsTargetValid)
inline void PlaceEntities(Rng& r,const CVector& src){
    for(int i=0;i<int(kNumEnts);++i){
        ResetEntity(i,r,uint8(r.pct(70) ? 3 : r.below(8)));
        const float d=r.pick({0.f,2.f,3.99f,4.f,10.f,50.f,79.f,80.f,81.f,200.f});
        CVector dir= r.pct(35) ? CVector{1.f,0.f,0.f} : CVector{r.uni(-1.f,1.f),r.uni(-1.f,1.f),r.uni(-0.3f,0.3f)};
        SetEntPos(i,CVector{src.x+dir.x*d,src.y+dir.y*d,src.z+dir.z*d}); } }
inline void* PickEnt(Rng& r){ return &ents[r.below(uint32(kNumEnts))]; }

inline uint64 H(const void* p,size_t n){ return Fnv(p,n); }
constexpr uintptr_t kHs0=0xB6ECA0, kCams=0xB6F19C; constexpr size_t kHsSize=0x94, kCamsSize=0x238*3;
inline json IdleCommon(){ json o=json::object();
    o["idle"]=H(idleRaw.data(),idleRaw.size()); o["cam"]=H(camRaw.data(),camRaw.size());
    for(size_t i=0;i<kNumEnts;++i)o["ent"+std::to_string(i)]=H(&ents[i],sizeof(Ent));
    for(size_t i=0;i<kNumEnts;++i)o["mat"+std::to_string(i)]=H(fmats[i].data(),0x60);
    o["events"]=H(fakeEvents.data(),sizeof(fakeEvents));
    o["pad"]=H(CPad::GetPad(0),sizeof(CPad));
    o["gidle_ticker"]=AtA<uint32>(0xB6FE34); o["ie_byte"]=AtA<uint8>(0xC0B184); o["ignore"]=Name(AtA<uint32>(0xB7CD68));
    o["hs0"]=H(reinterpret_cast<void*>(kHs0),kHsSize); o["cams"]=H(reinterpret_cast<void*>(kCams),kCamsSize);
    o["gb_ciney"]=AtA<uint32>(0xB6EC40); o["active_cam"]=AtA<uint8>(0xB6F081);
    o["clock"]=json::array({AtA<uint32>(0xB7CB84),AtA<uint32>(0xB7CB4C),AtA<uint32>(0xB7CB5C)});
    return o; }
inline void SentinelFloats(float* a,int n){ for(int i=0;i<n;++i)a[i]=std::bit_cast<float>(0xDEADBEEFu); }

// ---- CIdleCam::ProcessTargetSelection ----
inline json CasePts(Rng& r){
    MakeIdle(r);
    const uint32 now=PickNow(r); const float nowF=F(now); SetClock(now,r.next(),Sane(r));
    CVector src=SaneVec(r); PutVec(camRaw.data(),0x19C,src); PutVec(camRaw.data(),0x190,SaneVec(r));
    PlaceEntities(r,src);
    At<void*>(IdleP(),0)= r.pct(20) ? nullptr : static_cast<void*>(r.pct(35) ? &ents[0] : PickEnt(r));
    At<float>(IdleP(),0x34)=nowF-r.pick({0.f,3000.f,4999.f,5000.f,5001.f,6000.f,11000.f,20000.f,100000.f,-2000.f,r.uni(0.f,30000.f)});
    At<float>(IdleP(),0x38)=r.pick({5000.f,5000.f,0.f,1000.f,2500.f,10000.f});
    At<float>(IdleP(),0x84)=r.pct(3) ? 0.f : r.pick({2.f,2.f,1.f,0.5f,4.f,3.f});
    At<int32>(IdleP(),0x58)=r.pickI({0,1,2,3,3,3});
    At<int32>(IdleP(),0x54)=r.pickI({0,0,0,1,5,13,14,15,20,-1});
    At<int32>(IdleP(),0x50)=r.pickI({14,14,14,0,3,100,13});
    At<float>(IdleP(),0x24)=r.pick({1.f,1.f,0.5f,0.f,0.9f,2.f});                 // m_SlerpTime (IsTargetValid)
    At<float>(IdleP(),0x44)=r.pick({4.f,4.f,0.f,15.f}); At<float>(IdleP(),0x4C)=r.pick({80.f,80.f,10.f,500.f});
    curPlayer= r.pct(95) ? static_cast<void*>(&ents[0]) : nullptr;
    if(r.pct(65)){                                                                  // hay evento interesante
        fakeEvents[0]=r.below(30); fakeEvents[1]=r.next();
        void* en= r.pct(5) ? nullptr : (r.pct(35) ? At<void*>(IdleP(),0) : PickEnt(r));
        At<void*>(fakeEvents.data(),8)=en; curEvent=fakeEvents.data(); }
    AtA<uint32>(0xB7CD68)=reinterpret_cast<uint32>(&ents[3]);                        // pIgnoreEntity previo
    reinterpret_cast<void(__thiscall*)(void*)>(0x517870)(IdleP());
    return IdleCommon(); }

// ---- CIdleCam::ProcessSlerp ----
inline json CaseSlerp(Rng& r){
    MakeIdle(r);
    const uint32 now=PickNow(r); const float nowF=F(now); SetClock(now,r.next(),Sane(r));
    CVector src=SaneVec(r); PutVec(camRaw.data(),0x19C,src); PutVec(camRaw.data(),0x190,SaneVec(r));
    PlaceEntities(r,src);
    if(r.pct(10))SetEntPos(1,src);                                                   // objetivo justo en la camara (vectores nulos)
    At<void*>(IdleP(),0)=&ents[1+r.below(uint32(kNumEnts-1))];
    CVector from= r.pct(25) ? src : SaneVec(r); PutVec(IdleP(),4,from);               // m_PositionToSlerpFrom
    PutVec(IdleP(),0x18,r.pct(15) ? src : SaneVec(r));                                // m_LastIdlePos
    At<float>(IdleP(),0x34)=nowF-r.pick({0.f,500.f,1000.f,1999.f,2000.f,2001.f,5000.f,-100.f,r.uni(0.f,4000.f)});
    At<float>(IdleP(),0x14)=r.pct(3) ? 0.f : r.pick({2000.f,2000.f,1000.f,500.f,100.f,100000.f});
    At<int32>(IdleP(),0x50)=r.pickI({14,14,14,0,3,13});
    At<int32>(IdleP(),0x54)=r.pickI({0,0,5,13,14,15,20});
    float out[3]; SentinelFloats(out,3);
    const float ret=reinterpret_cast<float(__thiscall*)(void*,float*,float*)>(0x5179E0)(IdleP(),&out[0],&out[1]);
    json o=IdleCommon(); o["ret"]=std::bit_cast<uint32>(ret); o["x"]=std::bit_cast<uint32>(out[0]); o["z"]=std::bit_cast<uint32>(out[1]); o["sent"]=std::bit_cast<uint32>(out[2]); return o; }

// ---- CIdleCam::FinaliseIdleCamera ----
inline json CaseFinalise(Rng& r){
    MakeIdle(r);
    SetClock(PickNow(r),r.next(),Sane(r));
    FillSane(reinterpret_cast<void*>(kHs0),0,kHsSize,r);                              // gHandShaker[0] previo (la matriz la reescribe el doble)
    At<float>(IdleP(),0x88)=r.pick({1.f,1.f,0.f,0.5f,2.f,-1.f,r.uni(0.f,3.f)});
    hsAngAfter=r.pick({0.f,0.05f,-0.05f,r.uni(-0.3f,0.3f),r.uni(-3.f,3.f)});
    FillBytes(hsMatAfter.data(),hsMatAfter.size(),r);
    for(size_t k=0;k<12;++k)At<float>(hsMatAfter.data(),(k/3)*0x10+(k%3)*4)=r.pick({1.f,0.f,-1.f,0.5f,r.uni(-1.5f,1.5f),r.uni(-1.5f,1.5f)});
    for(size_t k=0;k<4;++k)At<uint32>(hsMatAfter.data(),k*0x10+0xC)=0;
    const uint32 deg=r.below(100);
    if(deg<15){ for(size_t c=0;c<6;++c)At<float>(hsMatAfter.data(),(c/3)*0x10+(c%3)*4)=0.f; }          // right = forward = 0 -> front.x = front.y = 0 (rama 1e-4)
    else if(deg<20){ std::memset(hsMatAfter.data(),0,0x40); }                                         // matriz nula: front = (0,0,0)
    PutVec(camRaw.data(),0x19C,SaneVec(r)); PutVec(camRaw.data(),0x190,SaneVec(r)); PutVec(camRaw.data(),0x1B4,SaneVec(r));
    const float ax= r.pct(15) ? r.pick({0.f,1.5707964f,-1.5707964f,3.1415927f}) : r.uni(-7.f,7.f);
    const float ay= r.pct(15) ? r.pick({0.f,1.5707964f,-1.5707964f,3.1415927f}) : r.uni(-7.f,7.f);
    const float sh= r.pick({0.f,1.f,0.3f,0.999f,r.uni(0.f,1.f),r.uni(0.f,1.f)});
    reinterpret_cast<void(__thiscall*)(void*,float,float,float)>(0x50E760)(IdleP(),ax,ay,sh);
    return IdleCommon(); }

// ---- CIdleCam::Run ----
inline json CaseRun(Rng& r){
    MakeIdle(r);
    const uint32 now=PickNow(r); const float nowF=F(now); SetClock(now,r.next(),Sane(r));
    At<float>(IdleP(),0x2C)=nowF-r.pick({0.f,500.f,2999.f,3000.f,3001.f,10000.f,-100.f,r.uni(0.f,6000.f)});   // m_TimeIdleCamStarted
    At<float>(IdleP(),0x8C)=r.pick({3000.f,3000.f,0.f,1000.f,-5.f,1.0e9f,r.uni(1.f,9000.f)});                // m_ShakeBuildUpTime
    slerpOutX=Sane(r); slerpOutZ=Sane(r); slerpRet=r.pick({0.f,1.f,0.5f,0.25f,r.uni(0.f,1.f)});
    reinterpret_cast<void(__thiscall*)(void*)>(0x51D3E0)(IdleP());
    return IdleCommon(); }

// ---- CIdleCam::Process ----
inline json CaseProcess(Rng& r){
    MakeIdle(r);
    const uint32 now=PickNow(r);
    const uint32 frame=uint32(r.pickI({0,1,2,5,100,0x7FFFFFFF,-1,int64_t(r.below(100000))}));
    SetClock(now,frame,r.pct(10) ? 0.f : r.uni(0.f,12.f));
    auto* pad=CPad::GetPad(0); pad->LastTimeTouched=r.pct(50) ? r.below(500000) : r.next();
    At<int32>(IdleP(),0x90)= r.pct(60) ? int32(pad->LastTimeTouched) : int32(r.below(60));     // m_LastTimePadTouched
    At<int32>(IdleP(),0x94)=r.pickI({0,5,89999,90000,90001,100000,300000,-10});                // ticker
    At<float>(IdleP(),0x28)=r.pick({90000.f,90000.f,90000.f,0.f,1000.f,50.f});                 // m_TimeControlsIdleForIdleToKickIn
    switch(r.below(6)){ case 0:At<uint32>(IdleP(),0x30)=frame-1; break; case 1:At<uint32>(IdleP(),0x30)=frame; break; case 2:At<uint32>(IdleP(),0x30)=frame-2; break;
        case 3:At<uint32>(IdleP(),0x30)=0xFFFFFFFFu; break; case 4:At<uint32>(IdleP(),0x30)=frame+5; break; default:break; }
    const uint8 act=uint8(r.below(3)); AtA<uint8>(0xB6F081)=act;
    for(uint32 c=0;c<3;++c){ auto* cam=reinterpret_cast<void*>(kCams+0x238*c); PutVec(cam,0x19C,SaneVec(r)); PutVec(cam,0x190,SaneVec(r)); PutVec(cam,0x1B4,SaneVec(r)); }
    for(size_t i=0;i<kNumEnts;++i)ResetEntity(int(i),r,uint8(r.below(8)));
    At<void*>(IdleP(),0)= r.pct(30) ? nullptr : static_cast<void*>(PickEnt(r));
    At<float>(IdleP(),0x2C)=Sane(r);
    curPlayer= r.pct(92) ? static_cast<void*>(&ents[0]) : nullptr;
    AtA<uint8>(0xC0B184)=uint8(r.next()); AtA<uint32>(0xB6EC40)=r.next();
    const bool ret=reinterpret_cast<bool(__thiscall*)(void*)>(0x522C80)(IdleP());
    json o=IdleCommon(); o["ret"]=ret; return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"idlecamwave4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"idlecamwave4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("idlecamwave4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId(); realPlayer=player;
    InitModule();
    std::string only; { std::ifstream f(dir/"idlecamwave4-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,5> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,14> idleHooks{};
    for(size_t f=0;f<kAllIdle.size();++f)if(!(idleHooks[f]=FindHook("CIdleCam",kAllIdle[f]))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string("CIdleCam::")+kAllIdle[f]}});return;}

    // --- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch) ---
    const uintptr_t dllReg=Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t dllClean=Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const uintptr_t dllPlayer=Body(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed));
    const uintptr_t dllLosClear=Body(&CWorld::GetIsLineOfSightClear);
    const uintptr_t dllIeInvNon=Body(&CInterestingEvents::InvalidateNonVisibleEvents);
    const uintptr_t dllIeGet=Body(&CInterestingEvents::GetInterestingEvent);
    const uintptr_t dllIeInv=Body(&CInterestingEvents::InvalidateEvent);
    const uintptr_t dllHs=Body(&CHandShaker::Process);
    if(!dllReg||!dllClean||!dllPlayer||!dllLosClear||!dllIeInvNon||!dllIeGet||!dllIeInv||!dllHs){
        Write({{"event","error"},{"reason","DLL body not found"},{"reg",dllReg},{"clean",dllClean},{"player",dllPlayer},{"los_clear",dllLosClear},
               {"ie_inv_non",dllIeInvNon},{"ie_get",dllIeGet},{"ie_inv",dllIeInv},{"hs",dllHs}}); return; }
    {   std::set<uintptr_t> all{dllReg,dllClean,dllPlayer,dllLosClear,dllIeInvNon,dllIeGet,dllIeInv,dllHs};
        if(all.size()!=8){ Write({{"event","error"},{"reason","DLL bodies overlap"}}); return; } }

    std::array<bool,14> enabled{},locked{};
    for(size_t f=0;f<idleHooks.size();++f){enabled[f]=idleHooks[f]->Hooked();locked[f]=idleHooks[f]->Locked();idleHooks[f]->LockState(false);}

    // vtable falsa de entidad y nombres de buferes
    for(auto& v:entVt)v=reinterpret_cast<void*>(&V_Trap);
    if(named.empty()){
        Nm(IdleP(),kIdleSize,"idle"); Nm(camRaw.data(),camRaw.size(),"cam"); Nm(fakeEvents.data(),sizeof(fakeEvents),"evt");
        Nm(reinterpret_cast<void*>(0xC0B058),0x200,"ie"); Nm(reinterpret_cast<void*>(kHs0),kHsSize*6,"hs"); Nm(reinterpret_cast<void*>(kCams),kCamsSize,"cams");
        for(size_t i=0;i<ents.size();++i){ static const char* nn[4]{"ent0","ent1","ent2","ent3"}; Nm(&ents[i],sizeof(Ent),nn[i]); }
        for(size_t i=0;i<fmats.size();++i){ static const char* nn[4]{"mat0","mat1","mat2","mat3"}; Nm(fmats[i].data(),0x60,nn[i]); } }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      auto* I=reinterpret_cast<CIdleCam*>(IdleP());
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      Chk("sizeof(CIdleCam)",uint32(sizeof(CIdleCam)),uint32(kIdleSize));
      Chk("CIdleCam.m_Target",Off(I,&I->m_Target),0x0);
      Chk("CIdleCam.m_PositionToSlerpFrom",Off(I,&I->m_PositionToSlerpFrom),0x4); Chk("CIdleCam.m_TimeSlerpStarted",Off(I,&I->m_TimeSlerpStarted),0x10);
      Chk("CIdleCam.m_SlerpDuration",Off(I,&I->m_SlerpDuration),0x14);
      Chk("CIdleCam.m_LastIdlePos",Off(I,&I->m_LastIdlePos),0x18); Chk("CIdleCam.m_SlerpTime",Off(I,&I->m_SlerpTime),0x24); Chk("CIdleCam.m_TimeControlsIdleForIdleToKickIn",Off(I,&I->m_TimeControlsIdleForIdleToKickIn),0x28);
      Chk("CIdleCam.m_TimeIdleCamStarted",Off(I,&I->m_TimeIdleCamStarted),0x2C); Chk("CIdleCam.m_LastFrameProcessed",Off(I,&I->m_LastFrameProcessed),0x30);
      Chk("CIdleCam.m_TimeLastTargetSelected",Off(I,&I->m_TimeLastTargetSelected),0x34); Chk("CIdleCam.m_TimeMinimumToLookAtSomething",Off(I,&I->m_TimeMinimumToLookAtSomething),0x38);
      Chk("CIdleCam.m_DistTooClose",Off(I,&I->m_DistTooClose),0x44); Chk("CIdleCam.m_DistTooFar",Off(I,&I->m_DistTooFar),0x4C); Chk("CIdleCam.m_TargetLOSFramestoReject",Off(I,&I->m_TargetLOSFramestoReject),0x50);
      Chk("CIdleCam.m_TargetLOSCounter",Off(I,&I->m_TargetLOSCounter),0x54); Chk("CIdleCam.m_ZoomState",Off(I,&I->m_ZoomState),0x58); Chk("CIdleCam.m_CurFOV",Off(I,&I->m_CurFOV),0x70);
      Chk("CIdleCam.m_nForceAZoomOut",Off(I,&I->m_nForceAZoomOut),0x78); Chk("CIdleCam.m_bHasZoomedIn",Off(I,&I->m_bHasZoomedIn),0x79);
      Chk("CIdleCam.m_IncreaseMinimumTimeFactorForZoomedIn",Off(I,&I->m_IncreaseMinimumTimeFactorForZoomedIn),0x84); Chk("CIdleCam.m_DegreeShakeIdleCam",Off(I,&I->m_DegreeShakeIdleCam),0x88);
      Chk("CIdleCam.m_ShakeBuildUpTime",Off(I,&I->m_ShakeBuildUpTime),0x8C); Chk("CIdleCam.m_LastTimePadTouched",Off(I,&I->m_LastTimePadTouched),0x90);
      Chk("CIdleCam.m_IdleTickerFrames",Off(I,&I->m_IdleTickerFrames),0x94); Chk("CIdleCam.m_Cam",Off(I,&I->m_Cam),0x98);
      Chk("gIdleCam addr",uint32(reinterpret_cast<uintptr_t>(&gIdleCam)),0xB6FDA0);
      auto* C=reinterpret_cast<CCam*>(camRaw.data());
      Chk("sizeof(CCam)",uint32(sizeof(CCam)),0x238);
      Chk("CCam.m_vecFront",Off(C,&C->m_vecFront),0x190); Chk("CCam.m_vecSource",Off(C,&C->m_vecSource),0x19C); Chk("CCam.m_vecUp",Off(C,&C->m_vecUp),0x1B4);
      Chk("TheCamera.m_aCams",Off(&TheCamera,&TheCamera.m_aCams[0]),0x174); Chk("TheCamera.m_nActiveCam",Off(&TheCamera,&TheCamera.m_nActiveCam),0x59);
      Chk("TheCamera addr",uint32(reinterpret_cast<uintptr_t>(&TheCamera)),0xB6F028);
      auto* S=reinterpret_cast<CHandShaker*>(kHs0);
      Chk("sizeof(CHandShaker)",uint32(sizeof(CHandShaker)),kHsSize); Chk("CHandShaker.m_ang",Off(S,&S->m_ang),0); Chk("CHandShaker.m_resultMat",Off(S,&S->m_resultMat),0x3C);
      Chk("gHandShaker addr",uint32(reinterpret_cast<uintptr_t>(&gHandShaker)),0xB6ECA0);
      CMatrix* M=reinterpret_cast<CMatrix*>(fmats[0].data());
      Chk("CMatrix.right",Off(M,&M->GetRight()),0); Chk("CMatrix.forward",Off(M,&M->GetForward()),0x10); Chk("CMatrix.up",Off(M,&M->GetUp()),0x20); Chk("CMatrix.pos",Off(M,&M->GetPosition()),0x30);
      TInterestingEvent* E=reinterpret_cast<TInterestingEvent*>(fakeEvents.data());
      Chk("sizeof(TInterestingEvent)",uint32(sizeof(TInterestingEvent)),0xC); Chk("TInterestingEvent.entity",Off(E,&E->entity),8);
      Chk("g_InterestingEvents addr",uint32(reinterpret_cast<uintptr_t>(&g_InterestingEvents)),0xC0B058);
      Chk("gbCineyCamProcessedOnFrame addr",uint32(reinterpret_cast<uintptr_t>(&gbCineyCamProcessedOnFrame)),0xB6EC40);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        SnapP(CPad::GetPad(0),sizeof(CPad));
        Snap(0xB7CB4C,4); Snap(0xB7CB5C,4); Snap(0xB7CB84,4);   // CTimer: contador de fotogramas, paso, tiempo en ms
        Snap(0xB6FE34,4); Snap(0xC0B184,1); Snap(0xB6F081,1); Snap(kCams,kCamsSize); Snap(0xB7CD68,4);
        Snap(0xB6EC40,4); Snap(kHs0,kHsSize); }
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4C1D3u; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); scN=0;
                // los demas hooks de CIdleCam desactivados; solo la funcion probada cambia entre original y candidato
                for(size_t k=0;k<idleHooks.size();++k)if(std::string_view(kAllIdle[k])!=nm)idleHooks[k]->State(false);
                hooks[f]->State(candidate);
                const bool switches= ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                Patches P;
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                if(nm=="ProcessTargetSelection"){
                    P.Add(0x571A00,S_Clean); P.Add(dllClean,S_Clean); P.Add(0x571B70,S_Reg); P.Add(dllReg,S_Reg); P.Add(0x56E210,S_Player); P.Add(dllPlayer,S_Player);
                    P.Add(0x56A490,S_LosClear); P.Add(dllLosClear,S_LosClear);
                    P.Add(0x6029C0,S_IeInvNonVisible); P.Add(dllIeInvNon,S_IeInvNonVisible); P.Add(0x6028A0,S_IeGet); P.Add(dllIeGet,S_IeGet);
                    P.Add(0x602960,S_IeInvalidate); P.Add(dllIeInv,S_IeInvalidate);
                    out["state"]=CasePts(r); }
                else if(nm=="ProcessSlerp"){ out["state"]=CaseSlerp(r); }
                else if(nm=="FinaliseIdleCamera"){ P.Add(0x50D930,S_HsProcess); P.Add(dllHs,S_HsProcess); out["state"]=CaseFinalise(r); }
                else if(nm=="Run"){
                    P.Add(0x517870,S_Pts); P.Add(0x5179E0,S_Slerp); P.Add(0x517BF0,S_Fov); P.Add(0x50E760,S_Finalise);
                    out["state"]=CaseRun(r); }
                else if(nm=="Process"){
                    P.Add(0x51D3E0,S_Run); P.Add(0x571A00,S_Clean); P.Add(dllClean,S_Clean); P.Add(0x571B70,S_Reg); P.Add(dllReg,S_Reg); P.Add(0x56E210,S_Player); P.Add(dllPlayer,S_Player);
                    out["state"]=CaseProcess(r); }
                const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                out["fp"]=fp; out["trace"]=trace;
                RestoreRegions();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
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
    for(size_t f=0;f<idleHooks.size();++f){idleHooks[f]->State(enabled[f]);idleHooks[f]->LockState(locked[f]);}
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP;
    for(size_t f=0;f<idleHooks.size();++f)restored&=idleHooks[f]->Hooked()==enabled[f]&&idleHooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
