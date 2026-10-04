#pragma once
// Banco dentro del motor: CBoat (23 funciones, lista 22). Original (hooks de CBoat desactivados) frente a candidato
// con fixtures deterministas sobre barcos falsos (búfer 0xCD de sizeof(CBoat) con offsets crudos del EXE: matriz,
// clump/marcos RW, handling, puerta, nodos, wake), nunca vehículos reales ni el pool real (FillBoatList cambia el
// puntero del pool de vehículos 0xB74494 por un pool falso solo durante la llamada). Vtable de trampas en el barco
// (huecos 24 ProcessControlInputs y 41 BlowUpCar trazados); la función probada se llama por la dirección del EXE o
// por el hueco de la vtable del EXE 0x8721A0. Se simulan y trazan (dirección del EXE + símbolo/cuerpo de la DLL):
// base de CVehicle (constructor y destructor localizados en la DLL, SetModelIndex, PreRender, Render), CPhysical::
// ProcessControl, FillFrameArray, ChooseVehicleColour, audio del vehículo, FX (gestor y sistemas), RW (LTM, objetos
// de marco, Im3D, render states, marcos/átomos, MatrixUpdate), mundo, IA, explosiones, cámara, Darkel, eventos,
// referencias, pads (freno/acelerador/dirección), jugador, rand, CDoor::Process, ProcessBoatControl/FlyingControl
// (cBuoyancy no se ejecuta: la valida otro banco), CObject (operator new, constructor localizado, RefModelInfo,
// AttachToRwObject). CMatrix (SetRotate, RotateZ, UpdateRW, Attach), Normalise, GetHeading y _CIpow se ejecutan de
// verdad. Las globales tocadas (reloj, trucos, ratón, pads, KeepWaterOut, lista de barcos, contadores del wake,
// model infos usados, contador de objetos temporales, puntero del pool) se guardan y restauran en cada ejecución.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Boat.h"
#include "World.h"
#include "FxManager.h"
#include "EventGroup.h"
#include "Darkel.h"
#include "Explosion.h"
#include "Stats.h"
#include "CarAI.h"
#include "CarCtrl.h"
#include "VisibilityPlugins.h"
#include "../ProbeRuntime.hpp"

namespace Boat23Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; } };
inline DWORD mainThread{};
inline bool Main(){ return GetCurrentThreadId()==mainThread; }

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

// --- módulo de la DLL y búsqueda de cuerpos ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline bool Prologue(uintptr_t f){ const auto* b=reinterpret_cast<const uint8*>(f); if(b[0]==0x8B&&b[1]==0xFF)b+=2; return b[0]==0x55&&b[1]==0x8B&&b[2]==0xEC; }
// primera llamada (E8) a una función de la DLL con prólogo push ebp / mov ebp,esp
inline uintptr_t FirstDllCall(uintptr_t fn){ fn=FollowIn(fn); if(!InDll(fn))return 0;
    for(uintptr_t p=fn;p<fn+0x300;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(InDll(t)&&Prologue(FollowIn(t)))return FollowIn(t); }
    return 0; }
// primera llamada a la DLL que sigue a la del operator new indicado (constructor de la DLL, adaptado de Misc8);
// se salta el operator new de colocación (no se expande en Debug)
inline uintptr_t FindCtorCall(uintptr_t fn,uintptr_t newFn,uintptr_t skipFn){
    fn=FollowIn(fn); const auto nf=FollowIn(newFn); const auto sk=FollowIn(skipFn); bool seen=false;
    for(uintptr_t p=fn;p<fn+0x1200;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(!seen){ if(InDll(t)&&FollowIn(t)==nf){seen=true;p+=4;} continue; }
        if(!InDll(t)||FollowIn(t)==sk)continue; if(*reinterpret_cast<const uint8*>(t)!=0xE9&&(t&15))continue;
        // el constructor es __thiscall: la llamada va precedida de mov ecx,[ebp+d8] / mov ecx,[ebp+d32] / mov ecx,eax
        const auto* q=reinterpret_cast<const uint8*>(p);
        const bool movEcx=(q[-3]==0x8B&&q[-2]==0x4D)||(q[-6]==0x8B&&q[-5]==0x8D)||(q[-2]==0x8B&&q[-1]==0xC8);
        if(!movEcx)continue;
        return FollowIn(t); }
    return 0; }
// Constructor de CObject de la DLL: se toma de una función propia que solo construye el objeto (placement new +
// llamada __thiscall al constructor), más fiable que buscar dentro de BlowUpCar. Nunca se ejecuta.
inline void* __cdecl MakeObjectProbe(void* p){ return ::new (p) CObject(); }
inline uintptr_t FindThisCall(uintptr_t fn,uintptr_t skipFn){
    fn=FollowIn(fn); const auto sk=FollowIn(skipFn); if(!InDll(fn))return 0;
    for(uintptr_t p=fn+6;p<fn+0x200;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(!InDll(t)||FollowIn(t)==sk)continue;
        const auto* q=reinterpret_cast<const uint8*>(p);
        if((q[-3]==0x8B&&q[-2]==0x4D)||(q[-6]==0x8B&&q[-5]==0x8D)||(q[-2]==0x8B&&q[-1]==0xC8))return FollowIn(t); }
    return 0; }
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

// ======================= objetos falsos =======================
constexpr size_t BOAT=sizeof(CBoat), MATB=0x60, FRAME=0xA8, NFRAMES=14, NATOM=8, NFX=4, POOLN=8, VSTRIDE=0xA18, NSLOT=66, OBJ=0x200;
constexpr uintptr_t EXE_BOAT_VT=0x8721A0;
alignas(16) inline std::array<uint8,BOAT> boat{}, tmpl{};
alignas(16) inline std::array<uint8,MATB> boatMat{}, objMat{};
alignas(16) inline std::array<std::array<uint8,FRAME>,NFRAMES> frames{};
alignas(16) inline std::array<std::array<uint8,0x70>,NATOM> atoms{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<uint8,0x100> handling{}, boatHandling{};
alignas(16) inline std::array<uint8,0x308> mi{};
alignas(16) inline std::array<std::array<uint8,0x40>,NFX> fxs{};
alignas(16) inline std::array<uint8,0x800> ped{}, drv{}, otherVeh{};
alignas(16) inline std::array<uint8,0x40> playerData{}, wanted{};
alignas(16) inline std::array<uint8,VSTRIDE*POOLN> poolObjs{};
alignas(16) inline std::array<std::array<uint8,MATB>,POOLN> poolMats{};
alignas(16) inline std::array<uint8,POOLN> poolFlags{};
alignas(16) inline std::array<uint8,0x20> pool{};
alignas(16) inline std::array<uint8,OBJ> obj{};
alignas(16) inline std::array<uint8,FRAME> newFrame{};
alignas(16) inline std::array<uint8,0x70> cloneAtom{};
inline std::array<void*,NSLOT> boatVt{}, objVt{};
inline std::array<int,NFRAMES> frameAtomFirst{}, frameAtomCount{};
inline std::array<uint32,NFX> fxStatus{};
inline const int16 BOAT_MODELS[]{430,446,452,453,454,460,472,473,484,493,595};
inline std::vector<std::pair<void**,void*>> savedMi;

struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v==EXE_BOAT_VT)return "exe_boat_vt";
    if(v==0x6A0750||FollowIn(v)==FollowIn(Addr(&GetCurrentAtomicObjectCB)))return "GetCurrentAtomicObjectCB";
    if(v==0x6F00D0||FollowIn(v)==FollowIn(Addr(&GetBoatAtomicObjectCB)))return "GetBoatAtomicObjectCB";
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    return v; }
inline json Name(const void* p){ return Name(uintptr_t(p)); }
inline json RwMat(const void* p){ const auto* f=static_cast<const float*>(p); json r=json::array(); for(int k:{0,1,2,4,5,6,8,9,10,12,13,14})r.push_back(F(f[k])); return r; }
inline json Vec(const void* p){ const auto* f=static_cast<const float*>(p); return json::array({F(f[0]),F(f[1]),F(f[2])}); }
inline json Words(const void* p,size_t from,size_t to){ json r=json::array(); for(size_t k=from;k+4<=to;k+=4)r.push_back(Name(*reinterpret_cast<const uint32*>(static_cast<const uint8*>(p)+k))); return r; }
// liveTrace: solo marca fases fuera de las llamadas probadas (nunca se escribe desde los stubs)
inline bool liveTrace{};

// --- registro de stubs sin reservas de memoria ni E/S: tipo + valores crudos en un array fijo; el json (con los
//     nombres de punteros) se construye después de que la función probada vuelve ---
struct I { int32 v; }; struct U { uint32 v; }; struct Fl { float v; }; struct Pt { const void* p; }; struct Bo { bool v; };
struct V3 { const void* p; }; struct M12 { const void* p; }; struct H { uint64 v; }; struct S { const char* p; };
struct Arg { uint8 kind; uint32 v; };
struct Rec { const char* tag; uint32 n; std::array<Arg,24> a; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Put(Rec& r,uint8 k,uint32 v){ if(r.n<r.a.size())r.a[r.n++]={k,v}; }
inline void Add(Rec& r,I x){ Put(r,0,uint32(x.v)); }
inline void Add(Rec& r,U x){ Put(r,1,x.v); }
inline void Add(Rec& r,Fl x){ Put(r,2,std::bit_cast<uint32>(x.v)); }
inline void Add(Rec& r,Pt x){ Put(r,3,uint32(uintptr_t(x.p))); }
inline void Add(Rec& r,Bo x){ Put(r,4,x.v ? 1u : 0u); }
inline void Add(Rec& r,V3 x){ const auto* f=static_cast<const uint32*>(x.p); for(int k=0;k<3;++k)Put(r,2,f[k]); }
inline void Add(Rec& r,M12 x){ const auto* f=static_cast<const uint32*>(x.p); for(int k:{0,1,2,4,5,6,8,9,10,12,13,14})Put(r,2,f[k]); }
inline void Add(Rec& r,H x){ Put(r,6,uint32(x.v)); Put(r,7,uint32(x.v>>32)); }
inline void Add(Rec& r,S x){ Add(r,H{Fnv(x.p,strlen(x.p))}); }
// migas de pan a prueba de cuelgues: 32 bytes por llamada (contador, valor, 24 caracteres de etiqueta) con un solo
// WriteFile sobre un fichero abierto una vez con FILE_FLAG_WRITE_THROUGH; sin reservas ni json. Solo en los primeros
// casos de cada función (crumbOn). Fichero: <carpeta del juego>/boat23-breadcrumb.bin
inline HANDLE crumb=INVALID_HANDLE_VALUE; inline uint32 crumbN{}; inline bool crumbOn{};
inline void Crumb(const char* tag,uint32 val=0){
    if(!crumbOn||crumb==INVALID_HANDLE_VALUE)return;
    char buf[32]{}; const uint32 n=++crumbN; std::memcpy(buf,&n,4); std::memcpy(buf+4,&val,4);
    for(int k=0;k<24&&tag[k];++k)buf[8+k]=tag[k];
    DWORD w=0; WriteFile(crumb,buf,32,&w,nullptr); }
template<class... A> void T(const char* tag,A... a){
    Crumb(tag,GetCurrentThreadId());
    if(!Main())return; if(nrec>=recs.size()){ recOverflow=true; return; }
    Rec& r=recs[nrec++]; r.tag=tag; r.n=0; (Add(r,a),...); }
inline json RecsToJson(){
    json out=json::array();
    for(size_t i=0;i<nrec;++i){ const Rec& r=recs[i]; json j=json::array(); j.push_back(r.tag);
        for(uint32 k=0;k<r.n;++k){ const Arg& a=r.a[k];
            switch(a.kind){ case 0: j.push_back(int32(a.v)); break; case 1: case 2: j.push_back(a.v); break; case 3: j.push_back(Name(uintptr_t(a.v))); break;
                case 4: j.push_back(a.v!=0); break; case 6: { const uint32 hi=k+1<r.n ? r.a[k+1].v : 0; j.push_back(uint64(a.v)|(uint64(hi)<<32)); ++k; break; } default: j.push_back(a.v); } }
        out.push_back(std::move(j)); }
    if(recOverflow)out.push_back("overflow");
    return out; }

// trampas de vtable
template<int N> void* __fastcall VTrap(void* v,void*) { T("unexpected_slot",I{N},Pt{v}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,NSLOT>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
inline void __fastcall VInputs(void* self,void*,uint32 pad){ T("vt.ProcessControlInputs",Pt{self},U{pad&0xFF}); }
inline void __fastcall VBlowUp(void* self,void*,void* culprit,uint32 cut){ T("vt.BlowUpCar",Pt{self},Pt{culprit},U{cut&0xFF}); }
inline void __fastcall VObjSetModel(void* self,void*,uint32 m){ T("obj.SetModelIndexNoCreate",Pt{self},U{m}); }
inline void __fastcall VObjSetStatic(void* self,void*,uint32 s){ T("obj.SetIsStatic",Pt{self},U{s&0xFF}); }

// --- dependencias simuladas ---
inline void* fakePed{}; inline void* fakeVeh{}; inline CVector centre{}; inline float progress{}; inline uint32 appearance{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ T("FindPlayerPed",I{id}); return static_cast<CPlayerPed*>(fakePed); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote){ T("FindPlayerVehicle",I{id},Bo{remote}); return static_cast<CVehicle*>(fakeVeh); }
inline const CVector* __cdecl FakeCentre(int32 id){ T("FindPlayerCentreOfWorld_NoSniperShift",I{id}); return &centre; }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand(){ T("rand"); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline float __cdecl FakeProgress(){ T("GetPercentageProgress"); return progress; }
inline uint32 __fastcall FakeAppearance(void* self,void*){ Crumb("~GetVehicleAppearance"); return appearance; }
inline const char* const VOID_NAMES[]{"CVehicle::PreRender","CVehicle::Render","CPhysical::ProcessControl","Audio::Service","ProcessDelayedExplosion",
    "UpdateClumpAlpha","ProcessCarAlarm","DoFixedMachineGuns","DoDriveByShootings","KillPedsInVehicle","Audio::Terminate","CVehicle::~CVehicle","Fx::Play","Fx::Stop","Fx::Kill","Fx::CopyParentMatrix"};
template<int N> void __fastcall Void0(void* self,void*){ T(VOID_NAMES[N],Pt{self}); }
inline void __fastcall FakeLawEnforcer(void* self,void*,uint32 b){ T("ChangeLawEnforcerState",Pt{self},U{b&0xFF}); }
inline void __fastcall FakeAudioInit(void* self,void*,void* e){ T("Audio::Initialise",Pt{self},Pt{e}); }
inline void* __fastcall FakeVehCtor(void* self,void*,uint32 by){ T("CVehicle::CVehicle",Pt{self},U{by&0xFF}); std::memcpy(self,tmpl.data(),0x5A0); At<void*>(self,0)=nullptr; return self; }
inline void __fastcall FakeVehSetModel(void* self,void*,uint32 m){ T("CVehicle::SetModelIndex",Pt{self},U{m}); At<int16>(self,0x22)=int16(m); At<void*>(self,0x18)=clump.data(); }
inline std::array<int8,12> fillPattern{};
inline void __cdecl FakeFillFrameArray(void* c,RwFrame** out){ T("FillFrameArray",Pt{c},Pt{out}); for(size_t k=0;k<12;++k)if(fillPattern[k]>=0)out[k]=reinterpret_cast<RwFrame*>(frames[size_t(fillPattern[k])].data()); }
inline uint8 colours[4]{};
inline void __fastcall FakeColour(void* self,void*,uint8* a,uint8* b,uint8* c,uint8* d,int32 v){ T("ChooseVehicleColour",Pt{self},Pt{a},Pt{b},Pt{c},Pt{d},I{v}); *a=colours[0]; *b=colours[1]; *c=colours[2]; *d=colours[3]; }
inline void __fastcall FakeSetCompRot(void* self,void*,void* frame,int32 axis,float angle,uint32 reset){ T("SetComponentRotation",Pt{self},Pt{frame},I{axis},Fl{angle},U{reset&0xFF}); }
inline void __cdecl FakeCompAlpha(void* atomic,int32 alpha){ T("SetComponentAtomicAlpha",Pt{atomic},I{alpha}); }
inline RwMatrix* __cdecl FakeLTM(RwFrame* f){ Crumb("~RwFrameGetLTM"); return reinterpret_cast<RwMatrix*>(reinterpret_cast<uint8*>(f)+0x50); }
inline RwFrame* __cdecl FakeForAllObjects(RwFrame* f,RwObjectCallBack cb,void* data){
    T("RwFrameForAllObjects",Pt{f},Pt{reinterpret_cast<const void*>(cb)},Pt{data});
    for(size_t k=0;k<NFRAMES;++k)if(frames[k].data()==reinterpret_cast<uint8*>(f))for(int j=0;j<frameAtomCount[k];++j)cb(reinterpret_cast<RwObject*>(atoms[size_t((frameAtomFirst[k]+j)%int(NATOM))].data()),data);
    return f; }
inline RwObject* __cdecl FakeFirstObject(RwFrame* f){ Crumb("~GetFirstObject"); for(size_t k=0;k<NFRAMES;++k)if(frames[k].data()==reinterpret_cast<uint8*>(f))return frameAtomCount[k] ? reinterpret_cast<RwObject*>(atoms[size_t(frameAtomFirst[k]%int(NATOM))].data()) : nullptr; return nullptr; }
inline RpAtomic* __cdecl FakeAtomicRender(RpAtomic* a){ T("AtomicRender",Pt{a}); return a; }
inline RwMatrix* __cdecl FakeMatrixUpdate(RwMatrix* m){ T("RwMatrixUpdate",Pt{m}); return m; }
inline float doorAngle{}; inline bool doorRet{};
inline bool __fastcall FakeDoorProcess(void* door,void*,void* veh,const CVector* om,const CVector* ot,const CVector* off){ T("CDoor::Process",Pt{door},Pt{veh},Pt{om},Pt{ot},V3{off}); At<float>(door,0xC)=doorAngle; return doorRet; }
inline void __fastcall FakeSplashes(void* self,void*,float d){ T("DoBoatSplashes",Pt{self},Fl{d}); }
// FX
inline size_t fxNext{}; inline uint32 fxCreateMask{};
inline void* __fastcall FakeCreateFx(void* mgr,void*,const char* name,const CVector* p,RwMatrix* m,uint32 b){
    const size_t k=fxNext++; const bool ok=(fxCreateMask>>(k%32))&1; T("CreateFxSystem",Pt{mgr},S{name},V3{p},Pt{m},U{b&0xFF},Bo{ok});
    return ok ? fxs[k%NFX].data() : nullptr; }
inline uint32 __fastcall FakeFxStatus(void* fx,void*){ Crumb("~Fx::GetPlayStatus"); for(size_t k=0;k<NFX;++k)if(fxs[k].data()==fx)return fxStatus[k]; return 7; }
inline void __fastcall FakeFxLocal(void* fx,void*,uint32 b){ T("Fx::SetLocalParticles",Pt{fx},U{b&0xFF}); }
inline void __fastcall FakeFxSetMatrix(void* fx,void*,void* m){ T("Fx::SetMatrix",Pt{fx},Pt{m}); }
inline void __fastcall FakeFxConstTime(void* fx,void*,uint32 on,float t){ T("Fx::SetConstTime",Pt{fx},U{on&0xFF},Fl{t}); }
// IA / física
inline void __cdecl FakeCarAI(void* v){ T("UpdateCarAI",Pt{v}); }
inline void __cdecl FakeSteerAI(void* v){ T("SteerAICarWithPhysics",Pt{v}); }
inline float boatVol{};
inline void __fastcall FakeBoatControl(void* self,void*,void* h,float* vol,uint32 hit,uint32 post){ T("ProcessBoatControl",Pt{self},Pt{h},Pt{vol},Fl{*vol},U{hit&0xFF},U{post&0xFF}); *vol=boatVol; }
inline void __fastcall FakeFlying(void* self,void*,int32 model,float a,float b,float c,float d){ T("FlyingControl",Pt{self},I{model},Fl{a},Fl{b},Fl{c},Fl{d}); }
inline int16 __fastcall FakeSay(void* p,void*,int32 ctx,uint32 delay,float prob,uint32 a,uint32 b,uint32 c){ T("Say",Pt{p},I{ctx},U{delay},Fl{prob},U{a&0xFF},U{b&0xFF},U{c&0xFF}); return 0; }
inline void __fastcall FakeRegRef(void* self,void*,void* field){ T("RegisterReference",Pt{self},Pt{field}); }
inline void __fastcall FakeCleanRef(void* self,void*,void* field){ T("CleanUpOldReference",Pt{self},Pt{field}); }
inline void* __fastcall FakeEventAdd(void* grp,void*,CEvent* ev,uint32 valid){ T("CEventGroup::Add",Pt{grp},I{int32(ev->GetEventType())},U{valid&0xFF}); return nullptr; }
// pads
inline int16 brakeVal{}, accelVal{}, steerVal{};
inline int16 __fastcall FakeBrake(void*,void*){ Crumb("~GetBrake"); return brakeVal; }
inline int16 __fastcall FakeAccel(void*,void*){ Crumb("~GetAccelerate"); return accelVal; }
inline int16 __fastcall FakeSteer(void*,void*){ Crumb("~GetSteeringLeftRight"); return steerVal; }
// mundo / render
inline void __cdecl FakeWorldAdd(void* e){ T("CWorld::Add",Pt{e}); }
inline void __cdecl FakeWorldRemove(void* e){ T("CWorld::Remove",Pt{e}); }
inline RwBool __cdecl FakeRenderState(RwRenderState s,void* v){ T("RenderStateSet",I{int32(s)},Pt{v}); return TRUE; }
inline void* im3dRet{};
inline void* __cdecl FakeIm3DTransform(RwIm3DVertex* v,RwUInt32 n,RwMatrix* m,RwUInt32 flags){ T("Im3DTransform",Pt{v},U{n},M12{m},U{flags},H{Fnv(v,sizeof(RwIm3DVertex)*n)}); return im3dRet; }
inline RwBool __cdecl FakeIm3DRender(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ T("Im3DRenderIndexedPrimitive",I{int32(t)},Pt{idx},I{n}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ T("Im3DEnd"); return TRUE; }
// BlowUpCar
inline void __cdecl FakeClumpFlag(void* c,int32 f){ T("SetClumpForAllAtomicsFlag",Pt{c},I{f}); }
inline void __fastcall FakeCamShake(void* cam,void*,float s,float x,float y,float z){ T("CamShake",Pt{cam},Fl{s},Fl{x},Fl{y},Fl{z}); }
inline void __cdecl FakeExplosion(void* v,void* c,int32 type,float x,float y,float z,uint32 life,uint32 snd,float shake,uint32 vis){ T("AddExplosion",Pt{v},Pt{c},I{type},Fl{x},Fl{y},Fl{z},U{life},U{snd&0xFF},Fl{shake},U{vis&0xFF}); }
inline void __cdecl FakeDarkel(void* v,int32 id){ T("RegisterCarBlownUpByPlayer",Pt{v},I{id}); }
inline bool objNewOk{};
inline void* __cdecl FakeObjNew(size_t n){ T("CObject::new",U{uint32(n)}); return objNewOk ? obj.data() : nullptr; }
inline CVector objPos{};
inline void* __fastcall FakeObjCtor(void* self,void*){ T("CObject::CObject",Pt{self}); std::memset(self,0xCD,OBJ); At<void*>(self,0)=objVt.data(); At<void*>(self,0x14)=objMat.data();
    std::memset(objMat.data(),0,MATB); At<float>(objMat.data(),0)=1.f; At<float>(objMat.data(),0x14)=1.f; At<float>(objMat.data(),0x28)=1.f; *reinterpret_cast<CVector*>(objMat.data()+0x30)=objPos; At<uint32>(self,0x1C)=0; At<uint32>(self,0x140)=0xFFFFFFFFu; return self; }
inline void __fastcall FakeRefModel(void* self,void*,int32 m){ T("RefModelInfo",Pt{self},I{m}); }
inline RwFrame* __cdecl FakeFrameCreate(){ T("RwFrameCreate"); return reinterpret_cast<RwFrame*>(newFrame.data()); }
inline RpAtomic* __cdecl FakeAtomicClone(RpAtomic* a){ T("RpAtomicClone",Pt{a}); return reinterpret_cast<RpAtomic*>(cloneAtom.data()); }
inline RpAtomic* __cdecl FakeAtomicSetFrame(RpAtomic* a,RwFrame* f){ T("RpAtomicSetFrame",Pt{a},Pt{f}); return a; }
inline void __cdecl FakeRenderCB(void* a,void* cb){ T("SetAtomicRenderCallback",Pt{a},Pt{cb}); }
inline void __fastcall FakeAttach(void* self,void*,void* o,uint32 upd){ T("AttachToRwObject",Pt{self},Pt{o},U{upd&0xFF}); }

// diagnóstico: primera aparición de cada excepción (no AV) en el hilo principal; no se maneja ninguna
inline std::set<DWORD> seenCodes;
inline LONG CALLBACK LogException(PEXCEPTION_POINTERS p){
    const DWORD code=p->ExceptionRecord->ExceptionCode;
    // durante los casos con traza en vivo también se registran los accesos inválidos (cualquier hilo)
    static int avLogged=0; const bool av=code==EXCEPTION_ACCESS_VIOLATION&&liveTrace&&avLogged++<20;
    if(code!=0x406D1388&&log.is_open()&&(av||(GetCurrentThreadId()==mainThread&&code!=EXCEPTION_ACCESS_VIOLATION&&seenCodes.insert(code).second))){
        log<<json{{"event","step"},{"exception",code},{"eip",p->ContextRecord->Eip},{"eip_dll",InDll(p->ContextRecord->Eip) ? json(p->ContextRecord->Eip-modBase) : json(nullptr)},
            {"access",p->ExceptionRecord->NumberParameters>1 ? json(p->ExceptionRecord->ExceptionInformation[1]) : json(nullptr)}}.dump()<<char(10); log.flush(); }
    return EXCEPTION_CONTINUE_SEARCH; }

// ======================= fixtures =======================
inline void RandMatrix(Rng& r,std::array<uint8,MATB>& buf,const CVector& around,float spread){
    std::memset(buf.data(),0,buf.size()); CMatrix m{}; m.SetRotate(r.uni(-0.6f,0.6f),r.uni(-0.6f,0.6f),r.uni(-3.1f,3.1f));
    if(r.next()%6==0)m.SetRotate(r.uni(2.6f,3.6f),r.uni(-0.4f,0.4f),r.uni(-3.1f,3.1f)); // boca abajo
    m.GetPosition()=CVector{around.x+r.uni(-spread,spread),around.y+r.uni(-spread,spread),around.z+r.uni(-3.f,3.f)}; std::memcpy(buf.data(),&m,0x40); }

inline void MakeWorld(const std::string& nm,Rng& r){
    const CVector cam=TheCamera.GetPosition();
    // marcos y átomos
    for(auto& f:frames){ std::memset(f.data(),0,FRAME); for(size_t k=0x10;k<0x90;k+=4)At<float>(f.data(),k)=r.uni(-2.f,2.f); At<uint32>(f.data(),0x1C)=0; At<uint32>(f.data(),0x5C)=0; }
    for(auto& a:atoms){ std::memset(a.data(),0,a.size()); a[0]=1; a[2]=uint8(r.next()%3==0 ? 0 : 4|(r.next()&1)); At<void*>(a.data(),0x48)=reinterpret_cast<void*>(&FakeAtomicRender); }
    for(size_t k=0;k<NFRAMES;++k){ frameAtomFirst[k]=int(r.next()%NATOM); frameAtomCount[k]=int(r.next()%4); }
    if(nm!="BlowUpCar"&&!frameAtomCount[1])frameAtomCount[1]=1; // el EXE no comprueba GetFirstObject (truco invisible)
    std::memset(clump.data(),0,clump.size()); clump[0]=2; At<void*>(clump.data(),4)=frames[13].data();
    // listas RW bien formadas (vacías y circulares) por si algún recorrido real llega a ellas:
    // atomicList del clump (+8) y objectList de cada marco (+0x90)
    At<void*>(clump.data(),8)=clump.data()+8; At<void*>(clump.data(),0xC)=clump.data()+8;
    for(auto& f:frames){ At<void*>(f.data(),0x90)=f.data()+0x90; At<void*>(f.data(),0x94)=f.data()+0x90; }
    for(auto& fx:fxs)std::memset(fx.data(),0xCD,fx.size());
    for(auto& s:fxStatus)s=r.next()%3==0 ? 1u : 0u;
    fxNext=0; fxCreateMask=r.next()%4 ? r.next()|1u : 0u;
    // handling
    for(size_t k=0;k<handling.size();k+=4)At<float>(handling.data(),k)=r.uni(0.f,2.f);
    At<float>(handling.data(),4)=r.uni(500.f,8000.f); At<float>(handling.data(),0x7C)=r.uni(0.1f,2.f); At<float>(handling.data(),0xA0)=r.uni(10.f,45.f);
    At<float>(handling.data(),0xC8)=r.pick({0.5f,1.f,2.f,r.uni(0.f,4.f)});
    for(size_t k=0;k<boatHandling.size();k+=4)At<float>(boatHandling.data(),k)=r.uni(-2.f,2.f);
    // barco
    std::memset(boat.data(),0xCD,BOAT); void* const b=boat.data();
    At<void*>(b,0)=boatVt.data();
    const bool near_=r.next()%4!=0; const CVector around=near_ ? cam : CVector{cam.x+r.pick({-300.f,300.f}),cam.y+r.uni(-300.f,300.f),cam.z};
    RandMatrix(r,boatMat,around,near_ ? r.pick({20.f,190.f,250.f}) : 50.f);
    At<float>(b,4)=r.uni(-50.f,50.f); At<float>(b,8)=r.uni(-50.f,50.f); At<float>(b,0xC)=r.uni(-5.f,5.f); At<float>(b,0x10)=r.uni(-3.2f,3.2f);
    At<void*>(b,0x14)=boatMat.data(); At<void*>(b,0x18)=nm!="ProcessControl"||r.next()%6 ? static_cast<void*>(clump.data()) : nullptr; At<uint32>(b,0x1C)=r.next(); At<int16>(b,0x22)=BOAT_MODELS[r.next()%std::size(BOAT_MODELS)];
    At<void*>(b,0x24)=nullptr;
    { static const uint8 st[]{0,0,1,2,3,3,4,5,8,10}; boat[0x36]=uint8(2|(st[r.next()%std::size(st)]<<3)); }
    At<uint32>(b,0x40)=r.next();
    for(size_t k=0;k<12;++k)At<float>(b,0x44+4*k)=r.next()%5==0 ? 0.f : r.uni(-0.6f,0.6f);
    At<float>(b,0x8C)=r.uni(300.f,9000.f); At<float>(b,0x90)=r.uni(300.f,20000.f); At<float>(b,0xA0)=r.uni(0.f,80.f);
    At<float>(b,0xD8)=r.pick({0.f,r.uni(0.f,30.f),r.uni(0.f,400.f)}); At<void*>(b,0xDC)=r.next()%2 ? static_cast<void*>(otherVeh.data()) : nullptr;
    for(size_t k=0;k<3;++k)At<float>(b,0xE0+4*k)=r.uni(-1.f,1.f);
    At<void*>(b,0x384)=handling.data(); boat[0x3BA]=uint8(r.next()%4 ? r.next()%24 : r.next());
    for(size_t k=0;k<8;++k)boat[0x428+k]=uint8(r.next());
    At<void*>(b,0x460)=r.next()%3 ? static_cast<void*>(drv.data()) : nullptr;
    At<float>(b,0x494)=r.uni(-0.8f,0.8f); At<float>(b,0x49C)=r.pick({0.f,-0.f,1.f,-1.f,r.uni(-1.f,1.f)}); At<float>(b,0x4A0)=r.uni(0.f,1.f); boat[0x4A4]=uint8(r.next()%5);
    At<float>(b,0x4C0)=r.pick({1000.f,460.f,std::nextafter(460.f,0.f),250.f,std::nextafter(250.f,0.f),30.f,0.f,-5.f,r.uni(0.f,1000.f),r.uni(200.f,300.f)});
    At<int16>(b,0x4DE)=int16(r.next());
    At<void*>(b,0x57C)=r.next()%3==0 ? static_cast<void*>(fxs[3].data()) : nullptr;
    At<float>(b,0x58C)=r.pick({0.f,-0.f,1.f,-1.f,0.4f,-0.6f,r.uni(-1.2f,1.2f)});
    At<uint32>(b,0x590)=5; At<uint32>(b,0x594)=5;
    At<float>(b,0x5A0)=r.uni(-10.f,10.f); At<float>(b,0x5A4)=r.pick({0.f,-0.1f,0.05f,std::nextafter(0.05f,1.f),0.18f,0.34f,r.uni(-0.3f,1.5f)}); At<float>(b,0x5A8)=r.pick({0.f,6.2831855f,std::nextafter(6.2831855f,9.f),r.uni(-1.f,30.f)});
    boat[0x5AC]=uint8(r.next());
    for(size_t k=0;k<12;++k)At<void*>(b,0x5B0+4*k)=r.next()%5 ? static_cast<void*>(frames[k].data()) : nullptr;
    for(size_t k=0;k<2;++k)At<float>(b,0x5E0+4*k)=r.uni(-1.f,1.f); At<uint16>(b,0x5E8)=uint16(r.next()%5); boat[0x5EA]=uint8(r.next()%3); boat[0x5EB]=uint8(r.next()%4);
    for(size_t k=0;k<3;++k)At<float>(b,0x5EC+4*k)=r.uni(-1.f,1.f);
    At<void*>(b,0x5F8)=boatHandling.data();
    At<float>(b,0x5FC)=r.next()%3==0 ? std::bit_cast<float>(0xC61C3FF6u) : r.uni(-3.2f,3.2f);
    At<uint32>(b,0x600)=r.next()%2 ? r.next()%200000u : r.next();
    At<float>(b,0x604)=r.uni(0.f,100.f); At<float>(b,0x608)=r.pick({0.f,4990.f,r.uni(0.f,6000.f)}); At<void*>(b,0x60C)=r.next()%2 ? static_cast<void*>(otherVeh.data()) : nullptr;
    for(size_t k=0;k<6;++k)At<float>(b,0x610+4*k)=r.uni(-0.5f,0.5f);
    At<void*>(b,0x628)=r.next()%2 ? static_cast<void*>(fxs[0].data()) : nullptr; At<void*>(b,0x62C)=r.next()%2 ? static_cast<void*>(fxs[1].data()) : nullptr;
    // el EXE no comprueba la hélice cuando el sistema de partículas ya existe
    if(At<void*>(b,0x628))At<void*>(b,0x5B0+4*8)=frames[8].data(); if(At<void*>(b,0x62C))At<void*>(b,0x5B0+4*10)=frames[10].data();
    for(size_t k=0;k<3;++k)At<float>(b,0x630+4*k)=r.uni(-3.f,3.f);
    boat[0x63C]=uint8(r.next()%6); boat[0x63D]=uint8(r.next()%4);
    At<float>(b,0x640)=r.uni(0.f,10.f);
    { const uint32 n=r.next()%5==0 ? 32u : r.next()%33; At<uint16>(b,0x644)=uint16(n);
      const CVector bp=*reinterpret_cast<CVector*>(boatMat.data()+0x30);
      float px=bp.x, py=bp.y;
      for(size_t k=0;k<32;++k){ At<float>(b,0x648+8*k)=px; At<float>(b,0x64C+8*k)=py; px+=r.uni(-4.f,4.f); py+=r.uni(-4.f,4.f);
          At<float>(b,0x748+4*k)=k<n ? r.pick({150.f,r.uni(0.f,150.f),r.uni(0.f,3.f)}) : (r.next()%4 ? 0.f : r.uni(-5.f,150.f)); boat[0x7C8+k]=uint8(r.next()); }
      if(r.next()%4==0)At<float>(b,0x748)=0.f;
      if(r.next()%6==0){ At<float>(b,0x648)=bp.x+r.uni(-2.f,2.f); At<float>(b,0x64C)=bp.y+r.uni(-2.f,2.f); } }
    // peds y vehículo ajeno
    std::memset(ped.data(),0xCD,ped.size()); std::memset(drv.data(),0xCD,drv.size()); std::memset(otherVeh.data(),0xCD,otherVeh.size());
    At<void*>(ped.data(),0)=boatVt.data(); At<void*>(drv.data(),0)=boatVt.data(); At<void*>(otherVeh.data(),0)=boatVt.data();
    At<void*>(ped.data(),0x480)=playerData.data(); std::memset(playerData.data(),0,playerData.size()); At<void*>(playerData.data(),0)=wanted.data();
    std::memset(wanted.data(),0,wanted.size()); At<int32>(wanted.data(),0x2C)=r.next()%3 ? int32(r.next()%7) : int32(r.next());
    At<uint32>(drv.data(),0x598)=r.next();
    fakePed=ped.data(); { const uint32 c=r.next()%4; fakeVeh=c==0 ? nullptr : c==1 ? static_cast<void*>(otherVeh.data()) : static_cast<void*>(boat.data()); }
    appearance=r.next()%3 ? 4u : r.next()%6; progress=r.pick({0.f,50.f,100.f,std::nextafter(100.f,0.f)});
    centre=*reinterpret_cast<CVector*>(boatMat.data()+0x30)+CVector{r.uni(-160.f,160.f),r.uni(-160.f,160.f),r.uni(-10.f,10.f)};
    // otros
    doorAngle=r.uni(-0.6f,0.6f); doorRet=r.next()%2; boatVol=r.uni(0.f,10.f); im3dRet=r.next()%4 ? reinterpret_cast<void*>(1) : nullptr;
    brakeVal=int16(r.next()%3==0 ? 0 : int32(r.next()%256)); accelVal=int16(r.next()%256); steerVal=int16(r.next()%3==0 ? 0 : int32(r.next()%257)-128);
    for(auto& c:colours)c=uint8(r.next());
    for(auto& p:fillPattern)p=r.next()%4 ? int8(r.next()%12) : int8(-1);
    objNewOk=r.next()%3!=0; objPos=*reinterpret_cast<CVector*>(boatMat.data()+0x30)+CVector{r.uni(-3.f,3.f),r.uni(-3.f,3.f),r.uni(-1.f,2.f)};
    std::memset(obj.data(),0xAB,OBJ); std::memset(newFrame.data(),0,FRAME); std::memset(cloneAtom.data(),0,cloneAtom.size());
    // model info falso para los modelos de barco
    std::memset(mi.data(),0,mi.size()); At<uint16>(mi.data(),0x4A)=uint16(r.next()%4 ? 175+r.next()%12 : r.next()%300);
    savedMi.clear();
    for(const int16 m:BOAT_MODELS){ auto& slot=reinterpret_cast<void*&>(CModelInfo::ms_modelInfoPtrs[m]); savedMi.push_back({&slot,slot}); slot=mi.data(); }
    // globales del caso
    CTimer::ms_fTimeStep=r.pick({1.f,0.5f,2.f,3.f,0.00001f,r.uni(0.f,3.f)});
    CTimer::m_snTimeInMilliseconds=r.next()%3 ? r.next()%300000u : r.next();
    *reinterpret_cast<uint8*>(0x96914B)=uint8(r.next()%4==0); *reinterpret_cast<uint8*>(0x969153)=uint8(r.next()%4==0);
    std::memcpy(tmpl.data(),boat.data(),BOAT);
}
inline void RestoreModelInfos(){ for(auto it=savedMi.rbegin();it!=savedMi.rend();++it)*it->first=it->second; savedMi.clear(); }

// pool falso de vehículos (FillBoatList)
inline void MakePool(Rng& r){
    const CVector cam=TheCamera.GetPosition(); const uint32 n=1+r.next()%POOLN;
    std::memset(pool.data(),0,pool.size()); At<void*>(pool.data(),0)=poolObjs.data(); At<void*>(pool.data(),4)=poolFlags.data(); At<int32>(pool.data(),8)=int32(n); At<int32>(pool.data(),0xC)=-1;
    for(size_t k=0;k<POOLN;++k){ uint8* const o=poolObjs.data()+VSTRIDE*k; std::memset(o,0xCD,VSTRIDE); poolFlags[k]=uint8(r.next()%5==0 ? 0x80|(r.next()&0x7F) : r.next()&0x7F);
        At<void*>(o,0)=boatVt.data(); RandMatrix(r,poolMats[k],cam,r.pick({30.f,90.f,150.f}));
        const bool m=r.next()%5!=0; At<void*>(o,0x14)=m ? static_cast<void*>(poolMats[k].data()) : nullptr;
        *reinterpret_cast<CVector*>(o+4)=*reinterpret_cast<CVector*>(poolMats[k].data()+0x30);
        At<uint32>(o,0x590)=r.next()%5 ? 5u : r.next()%11; At<uint16>(o,0x644)=uint16(r.next()%4 ? 1+r.next()%32 : 0); o[0x36]=uint8(2|((r.next()%3==0 ? 0u : 3u)<<3)); }
}

inline json Globals(){ return {Words(reinterpret_cast<void*>(0xC278F8),0,0x9C),Words(reinterpret_cast<void*>(0xC27994),0,0x10),Words(reinterpret_cast<void*>(0xC279A4),0,0x10),
    *reinterpret_cast<uint32*>(0xC1CC04),*reinterpret_cast<uint16*>(0xBB4A70)}; }

inline uintptr_t dllVehCtor{}, dllVehDtor{}, dllObjCtor{};
inline void Phase(const char* what){ if(liveTrace)Write({{"event","step"},{"phase",what}}); }
inline json Run(const std::string& nm,uintptr_t target,Rng& r,json& out){
    Phase("make_world"); MakeWorld(nm,r); Phase("world_ready"); void* const b=boat.data(); json ret=nullptr;
    auto thiscall0=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(b); };
    if(nm=="Constructor"){ std::memset(b,0xCD,BOAT); const int32 model=BOAT_MODELS[r.next()%std::size(BOAT_MODELS)]; const int32 by=int32(r.next()%5);
        ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,int32)>(target)(b,model,by)); }
    else if(nm=="Destructor"){ reinterpret_cast<void(__thiscall*)(void*)>(target)(b); }
    else if(nm=="DebugCode"){ for(int k=0;k<2;++k){ auto* pd=CPad::GetPad(k); pd->NewState.Start=int16(r.next()%4==0); pd->NewState.DPadLeft=int16(r.next()%3!=0 ? 255 : 0); pd->OldState.DPadLeft=int16(r.next()%3==0 ? 255 : 0); }
        boat[0x63D]=uint8(r.next()%2); thiscall0(); }
    else if(nm=="SetupModelNodes"||nm=="DisplayHandlingData"||nm=="PruneWakeTrail"||nm=="ProcessControl"||nm=="PreRender"||nm=="Render")thiscall0();
    else if(nm=="SetModelIndex")reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(b,uint32(BOAT_MODELS[r.next()%std::size(BOAT_MODELS)]));
    else if(nm=="GetComponentWorldPosition"){ CVector o{1.f,2.f,3.f}; const int32 c=int32(r.next()%12); if(!At<void*>(b,0x5B0+4*size_t(c)))At<void*>(b,0x5B0+4*size_t(c))=frames[size_t(c)].data(); // el EXE no comprueba el nodo
        reinterpret_cast<void(__thiscall*)(void*,int32,CVector*)>(target)(b,c,&o); out["pos"]=Vec(&o); }
    else if(nm=="IsComponentPresent")ret=reinterpret_cast<uint8(__thiscall*)(void*,int32)>(target)(b,int32(r.next()))!=0;
    else if(nm=="ProcessOpenDoor")reinterpret_cast<void(__thiscall*)(void*,void*,uint32,uint32,uint32,float)>(target)(b,ped.data(),r.next(),r.next(),r.next(),r.uni(0.f,1.f));
    else if(nm=="GetBoatAtomicObjectCB"){ void* data=reinterpret_cast<void*>(0x1234); auto* a=atoms[r.next()%NATOM].data();
        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(target)(a,&data)); out["data"]=Name(data); }
    else if(nm=="ModifyHandlingValue"){ const bool plus=r.next()%2; reinterpret_cast<void(__thiscall*)(void*,const bool*)>(target)(b,&plus); }
    else if(nm=="AddWakePoint"){ const CVector p{r.uni(-500.f,500.f),r.uni(-500.f,500.f),r.uni(-5.f,5.f)}; reinterpret_cast<void(__thiscall*)(void*,float,float,float)>(target)(b,p.x,p.y,p.z); }
    else if(nm=="IsSectorAffectedByWake"||nm=="IsVertexAffectedByWake"||nm=="CheckForSkippingCalculations"){
        auto* list=reinterpret_cast<void**>(0xC27994); MakePool(r);
        for(size_t k=0;k<4;++k){ const uint32 c=r.next()%4; list[k]=c==0 ? nullptr : c==1 ? b : static_cast<void*>(poolObjs.data()+VSTRIDE*(r.next()%POOLN)); }
        if(r.next()%3)list[0]=b;
        for(size_t k=0;k<POOLN;++k){ uint8* const o=poolObjs.data()+VSTRIDE*k; std::memcpy(o+0x644,boat.data()+0x644,0x7E8-0x644); At<uint16>(o,0x644)=uint16(r.next()%33);
            for(size_t j=0;j<32;++j){ At<float>(o,0x648+8*j)+=r.uni(-20.f,20.f); At<float>(o,0x64C+8*j)+=r.uni(-20.f,20.f); } }
        auto* sc=reinterpret_cast<int16*>(0xC279A4); for(size_t k=0;k<8;++k)sc[k]=int16(r.next()%3==0 ? int32(r.next()%12)-2 : int32(r.next()%9));
        const CVector bp=*reinterpret_cast<CVector*>(boatMat.data()+0x30);
        if(nm=="IsSectorAffectedByWake"){ CBoat* found[4]{reinterpret_cast<CBoat*>(0x11),reinterpret_cast<CBoat*>(0x22),reinterpret_cast<CBoat*>(0x33),reinterpret_cast<CBoat*>(0x44)};
            const float cx=bp.x+r.uni(-40.f,40.f), cy=bp.y+r.uni(-40.f,40.f), semi=r.pick({0.f,1.f,5.f,r.uni(0.f,20.f)});
            ret=reinterpret_cast<uint8(__cdecl*)(float,float,float,CBoat**)>(target)(cx,cy,semi,found)!=0; out["found"]={Name(found[0]),Name(found[1]),Name(found[2]),Name(found[3])}; }
        else if(nm=="IsVertexAffectedByWake"){ void* const which=r.next()%4 ? b : static_cast<void*>(poolObjs.data()); const CVector c{bp.x+r.uni(-30.f,30.f),bp.y+r.uni(-30.f,30.f),r.uni(-2.f,2.f)};
            const int16 q=int16(r.next()%4); const bool force=r.next()%2;
            ret=F(reinterpret_cast<float(__cdecl*)(float,float,float,void*,int32,uint32)>(target)(c.x,c.y,c.z,which,q,force)); }
        else reinterpret_cast<void(__cdecl*)()>(target)();
        out["pool"]=Fnv(poolObjs.data(),poolObjs.size()); }
    else if(nm=="FillBoatList"){ MakePool(r); auto* list=reinterpret_cast<void**>(0xC27994); for(size_t k=0;k<4;++k)list[k]=reinterpret_cast<void*>(0x5000+k);
        auto& vp=*reinterpret_cast<void**>(0xB74494); void* const saved=vp; vp=pool.data(); reinterpret_cast<void(__cdecl*)()>(target)(); vp=saved; }
    else if(nm=="Teleport"){ const CVector p{r.uni(-3000.f,3000.f),r.uni(-3000.f,3000.f),r.uni(-10.f,100.f)}; const uint32 clear=r.next()%2;
        if(!clear&&r.next()%3==0)At<void*>(b,0x14)=nullptr; // sin matriz solo cuando no se reinicia la orientación
        reinterpret_cast<void(__thiscall*)(void*,float,float,float,uint32)>(target)(b,p.x,p.y,p.z,clear); }
    else if(nm=="ProcessControlInputs"){
        *reinterpret_cast<uint8*>(0xB6EC2E)=uint8(r.next()%3!=0); *reinterpret_cast<uint8*>(0xC1CC02)=uint8(r.next()%3!=0); *reinterpret_cast<uint32*>(0xC1CC04)=r.next()%3==0 ? 0u : r.next()%2 ? 2u : 3u;
        *reinterpret_cast<float*>(0xB73418+0xC)=r.next()%2 ? 0.f : r.pick({-0.f,r.uni(-40.f,40.f),r.uni(-2.f,2.f)});
        for(int k=0;k<2;++k)CPad::GetPad(k)->NewState.m_bVehicleMouseLook=int16(r.next()%3==0);
        const uint32 pad=r.next()%6==0 ? 2u+r.next()%5 : r.next()%2;
        reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(b,pad); }
    else if(nm=="BlowUpCar"){ if(r.next()%3)At<void*>(b,0x5B4)=frames[1].data(); if(r.next()%2){ frameAtomCount[1]=1+int(r.next()%3); for(int j=0;j<frameAtomCount[1];++j)atoms[size_t((frameAtomFirst[1]+j)%int(NATOM))][2]|=4; }
        void* const culprit=r.next()%2 ? static_cast<void*>(otherVeh.data()) : nullptr; const uint32 cut=r.next()%2;
        if(liveTrace)Write({{"event","step"},{"phase","call"},{"target",target},{"flags42A",boat[0x42A]},{"node1",Name(At<void*>(b,0x5B4))},{"obj_new",objNewOk},{"atoms1",frameAtomCount[1]},
            {"entry_7322F0",ReversingProbe::Bytes(reinterpret_cast<void*>(0x7322F0),5)},{"stub",Addr(&FakeClumpFlag)},{"stub_body",FollowIn(Addr(&FakeClumpFlag))}});
        Crumb("<call BlowUpCar>"); reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(target)(b,culprit,cut); Crumb("<BlowUpCar returned>");
        Phase("returned");
        out["obj"]=Words(obj.data(),0,0x160); out["objmat"]=Words(objMat.data(),0,0x40); out["newframe"]=Words(newFrame.data(),0,FRAME); }
    RestoreModelInfos(); Phase("collect");
    const uintptr_t vp=At<uintptr_t>(b,0);
    out["vptr"]=vp==EXE_BOAT_VT||((nm=="Constructor"||nm=="Destructor")&&InDll(vp)) ? json("CBoat") : Name(vp);
    out["boat"]=Words(b,4,BOAT); out["mat"]=Words(boatMat.data(),0,0x40);
    { json fr=json::array(); for(auto& f:frames)fr.push_back(Fnv(f.data(),FRAME)); out["frames"]=fr; json at=json::array(); for(auto& a:atoms)at.push_back(Fnv(a.data(),a.size())); out["atoms"]=at; }
    out["handling"]={Fnv(handling.data(),handling.size()),Fnv(boatHandling.data(),boatHandling.size()),Fnv(mi.data(),mi.size())};
    out["peds"]={Fnv(ped.data(),ped.size()),Fnv(drv.data(),drv.size()),Fnv(otherVeh.data(),otherVeh.size()),Fnv(wanted.data(),wanted.size())};
    out["globals"]=Globals();
    return ret;
}

// ======================= tabla =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=23;
inline const std::array<Fn,NFN> fns{{
    {"CBoat","AddWakePoint",0x6F2550,0,-1,1024},{"CBoat","BlowUpCar",0,0x8721A0,41,512},{"CBoat","CheckForSkippingCalculations",0x6F10C0,0,-1,512},
    {"CBoat","Constructor",0x6F2940,0,-1,256},{"CBoat","DebugCode",0x6F0D00,0,-1,512},{"CBoat","Destructor",0x6F00F0,0,-1,256},
    {"CBoat","DisplayHandlingData",0x6F0D90,0,-1,128},{"CBoat","FillBoatList",0x6F2710,0,-1,1024},{"CBoat","GetBoatAtomicObjectCB",0x6F00D0,0,-1,256},
    {"CBoat","GetComponentWorldPosition",0,0x8721A0,25,256},{"CBoat","IsComponentPresent",0,0x8721A0,26,64},{"CBoat","IsSectorAffectedByWake",0x6F0E80,0,-1,1024},
    {"CBoat","IsVertexAffectedByWake",0x6F0F50,0,-1,1024},{"CBoat","ModifyHandlingValue",0x6F0DE0,0,-1,256},{"CBoat","PreRender",0,0x8721A0,17,1024},
    {"CBoat","ProcessControl",0,0x8721A0,10,1024},{"CBoat","ProcessControlInputs",0,0x8721A0,24,1024},{"CBoat","ProcessOpenDoor",0,0x8721A0,28,64},
    {"CBoat","PruneWakeTrail",0x6F0E20,0,-1,1024},{"CBoat","Render",0,0x8721A0,18,512},{"CBoat","SetModelIndex",0,0x8721A0,5,128},
    {"CBoat","SetupModelNodes",0x6F01A0,0,-1,128},{"CBoat","Teleport",0,0x8721A0,14,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"boat23-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"boat23-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("boat23");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    crumb=CreateFileA((dir/"boat23-breadcrumb.bin").string().c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,nullptr);
    std::string only; { std::ifstream f(dir/"boat23-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // cuerpos de la DLL de la base y constructores/destructores localizados
    struct Base { const char* cls; const char* name; uintptr_t exe; void* stub; uintptr_t dll; };
    std::vector<Base> bases{
        {"CVehicle","PreRender",0x6D6480,reinterpret_cast<void*>(&Void0<0>),0},{"CVehicle","Render",0x6D0E60,reinterpret_cast<void*>(&Void0<1>),0},
        {"CPhysical","ProcessControl",0x5485E0,reinterpret_cast<void*>(&Void0<2>),0},{"CVehicle","SetModelIndex",0x6D6A40,reinterpret_cast<void*>(&FakeVehSetModel),0}};
    for(auto& bs:bases)if(!(bs.dll=DllBody(bs.cls,bs.name))){Write({{"event","error"},{"reason","base body not found"},{"function",std::string(bs.cls)+"::"+bs.name}});return;}
    { const uintptr_t boatCtor=FirstDllCall(DllBody("CBoat","Constructor")); const uintptr_t a=boatCtor ? FirstDllCall(boatCtor) : 0; const uintptr_t c=FirstDllCall(DllBody("CVehicle","Constructor"));
      if(!a||a!=c)Write({{"event","warning"},{"reason","CVehicle constructor not found"},{"from_boat",a},{"from_wrapper",c}}); else dllVehCtor=a;
      dllVehDtor=FirstDllCall(DllBody("CVehicle","Destructor")); if(!dllVehDtor)Write({{"event","warning"},{"reason","CVehicle destructor not found"}});
      { const uintptr_t placement=Addr(static_cast<void*(__cdecl*)(size_t,void*) noexcept>(&::operator new));
        const uintptr_t fromProbe=FindThisCall(Addr(&MakeObjectProbe),placement);
        const uintptr_t fromBoat=FindCtorCall(DllBody("CBoat","BlowUpCar"),Addr(static_cast<void*(*)(size_t)>(&CObject::operator new)),placement);
        // las dos búsquedas deben coincidir; nunca se parchea una función propia del banco
        const uintptr_t own[]{FollowIn(Addr(&RecsToJson)),FollowIn(Addr(&Write)),FollowIn(Addr(static_cast<json(*)(uintptr_t)>(&Name))),FollowIn(Addr(&Words)),
                              FollowIn(Addr(&Phase)),FollowIn(Addr(&MakeObjectProbe)),FollowIn(Addr(&FakeClumpFlag)),FollowIn(Addr(&FakeObjCtor))};
        const bool isOwn=std::find(std::begin(own),std::end(own),fromProbe)!=std::end(own);
        if(fromProbe&&!isOwn)dllObjCtor=fromProbe;
        if(fromProbe!=fromBoat||isOwn)Write({{"event","warning"},{"reason","CObject constructor search mismatch (se usa la de MakeObjectProbe)"},{"from_probe",fromProbe ? fromProbe-modBase : 0},{"from_boat",fromBoat ? fromBoat-modBase : 0},{"own",isOwn}}); }
      if(!dllObjCtor)Write({{"event","warning"},{"reason","CObject constructor not found"}});
      else Write({{"event","info"},{"dll_obj_ctor",dllObjCtor-modBase},{"dll_veh_ctor",dllVehCtor ? dllVehCtor-modBase : 0},{"dll_veh_dtor",dllVehDtor ? dllVehDtor-modBase : 0},
                  {"obj_ctor_bytes",ReversingProbe::Bytes(reinterpret_cast<void*>(dllObjCtor),16)},
                  {"FakeClumpFlag",FollowIn(Addr(&FakeClumpFlag))-modBase}}); }
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    regions.clear();
    Snap(0xB7CB84,4); Snap(0xB7CB5C,4); Snap(0x96914B,1); Snap(0x969153,1); Snap(0xC278F8,0x9C); Snap(0xC27994,0x10); Snap(0xC279A4,0x10); Snap(0xC279CC,4); Snap(0x8D394C,4);
    Snap(0xB74494,4); Snap(0xB6EC2E,1); Snap(0xC1CC02,1); Snap(0xC1CC04,4); Snap(0xB73418,0x14); Snap(0xB73458,sizeof(CPad)*2); Snap(0xBB4A70,2);
    for(const int16 m:BOAT_MODELS)Snap(uintptr_t(&CModelInfo::ms_modelInfoPtrs[m]),4);
    void** const rsSlot=reinterpret_cast<void**>(*reinterpret_cast<uintptr_t*>(0xC97B24)+0x20); Snap(uintptr_t(rsSlot),4);

    FillTraps(boatVt,std::make_integer_sequence<int,int(NSLOT)>{}); FillTraps(objVt,std::make_integer_sequence<int,int(NSLOT)>{});
    boatVt[24]=reinterpret_cast<void*>(&VInputs); boatVt[41]=reinterpret_cast<void*>(&VBlowUp); objVt[6]=reinterpret_cast<void*>(&VObjSetModel); objVt[4]=reinterpret_cast<void*>(&VObjSetStatic);
    named={{boat.data(),BOAT,"boat"},{boatMat.data(),MATB,"boat_mat"},{clump.data(),clump.size(),"clump"},{handling.data(),handling.size(),"handling"},{boatHandling.data(),boatHandling.size(),"boat_handling"},
           {mi.data(),mi.size(),"mi"},{ped.data(),ped.size(),"player"},{drv.data(),drv.size(),"driver"},{otherVeh.data(),otherVeh.size(),"other_veh"},{playerData.data(),playerData.size(),"player_data"},
           {wanted.data(),wanted.size(),"wanted"},{poolObjs.data(),poolObjs.size(),"pool_objs"},{pool.data(),pool.size(),"pool"},{obj.data(),OBJ,"obj"},{objMat.data(),MATB,"obj_mat"},
           {newFrame.data(),FRAME,"new_frame"},{cloneAtom.data(),cloneAtom.size(),"clone_atom"},{boatVt.data(),sizeof(boatVt),"boat_vt"},{objVt.data(),sizeof(objVt),"obj_vt"},
           {reinterpret_cast<void*>(0xC278F8),0x90,"keep_water_verts"},{reinterpret_cast<void*>(0xC27988),0xC,"keep_water_idx"},{reinterpret_cast<void*>(0xB6F028),sizeof(CCamera),"camera"},
           {reinterpret_cast<void*>(0xA9AE80),0x40,"fx_man"},{reinterpret_cast<void*>(0xC228A8),4,"waterclear_ptr"}};
    for(size_t k=0;k<NFRAMES;++k)named.push_back({frames[k].data(),FRAME,"frame"});
    for(size_t k=0;k<NATOM;++k)named.push_back({atoms[k].data(),0x70,"atom"});
    for(size_t k=0;k<NFX;++k)named.push_back({fxs[k].data(),0x40,"fx"});
    { static std::array<std::string,NFRAMES+NATOM+NFX> labels; size_t base=named.size()-(NFRAMES+NATOM+NFX);
      for(size_t k=0;k<NFRAMES+NATOM+NFX;++k){ labels[k]=std::string(named[base+k].name)+std::to_string(k<NFRAMES ? k : k<NFRAMES+NATOM ? k-NFRAMES : k-NFRAMES-NATOM); named[base+k].name=labels[k].c_str(); } }

    bool passed=true; int pair=0;
    void* const veh=nullptr; // LogException queda sin registrar: escribir json desde un manejador puede bloquear
    {
        using std::bit_cast;
        Patches p;
        // Cuerpo de la DLL resuelto ANTES de escribir parches: si se resolvía después, FollowIn seguía el salto recién
        // escrito hasta el propio stub y lo parcheaba con un salto a sí mismo (bucle infinito en la primera llamada).
        // Nunca se parchea un stub ya registrado.
        std::set<uintptr_t> stubSet;
        auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
            const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
            stubSet.insert(st); stubSet.insert(stb);
            auto ok=[&](uintptr_t a){ return a&&!stubSet.count(a); };
            if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
        for(auto& bs:bases){ stubSet.insert(uintptr_t(bs.stub)); stubSet.insert(FollowIn(uintptr_t(bs.stub))); if(!stubSet.count(bs.exe))p.Add(bs.exe,bs.stub); if(!stubSet.count(bs.dll))p.Add(bs.dll,bs.stub); }
        P(0x6D5F10,dllVehCtor,FakeVehCtor); P(0x6E2B40,dllVehDtor,&Void0<11>);
        P(0x56E210,Addr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed);
        P(0x56E0D0,Addr(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle);
        P(0x56E320,Addr(&FindPlayerCentreOfWorld_NoSniperShift),FakeCentre);
        P(0x821B1E,0,FakeRand); P(0,Addr(&CGeneral::GetRandomNumber),FakeRandomNumber);
        P(0x5591E0,Addr(&CStats::GetPercentageProgress),FakeProgress);
        P(0x6D1080,Addr(&CVehicle::GetVehicleAppearance),FakeAppearance); P(0,Addr(&CVehicle::GetVehicleAppearance_Reversed),FakeAppearance);
        P(0x502280,Addr(&CAEVehicleAudioEntity::Service),&Void0<3>); P(0x4FB8C0,Addr(&CAEVehicleAudioEntity::Terminate),&Void0<10>); P(0x4F7670,Addr(&CAEVehicleAudioEntity::Initialise),FakeAudioInit);
        P(0x6D1340,Addr(&CVehicle::ProcessDelayedExplosion),&Void0<4>); P(0x6D2980,Addr(&CVehicle::UpdateClumpAlpha),&Void0<5>); P(0x6D21F0,Addr(&CVehicle::ProcessCarAlarm),&Void0<6>);
        P(0x73F400,Addr(&CVehicle::DoFixedMachineGuns),&Void0<7>); P(0x741FD0,Addr(&CVehicle::DoDriveByShootings),&Void0<8>); P(0x6D1C80,Addr(&CVehicle::KillPedsInVehicle),&Void0<9>);
        P(0x6D2330,Addr(&CVehicle::ChangeLawEnforcerState),FakeLawEnforcer);
        P(0x4C5440,Addr(&CClumpModelInfo::FillFrameArray),FakeFillFrameArray);
        P(0x4C8500,Addr(&CVehicleModelInfo::ChooseVehicleColour),FakeColour);
        P(0x6DBA30,Addr(&CVehicle::SetComponentRotation),FakeSetCompRot); P(0x6D2960,Addr(&CVehicle::SetComponentAtomicAlpha),FakeCompAlpha);
        P(0x7F0990,Addr(&RwFrameGetLTM),FakeLTM); P(0x7F1200,Addr(&RwFrameForAllObjects),FakeForAllObjects); P(0x7348C0,Addr(&GetFirstObject),FakeFirstObject);
        P(0x7F18A0,Addr(&RwMatrixUpdate),FakeMatrixUpdate);
        P(0x6F4040,Addr(&CDoor::Process),FakeDoorProcess); P(0x6DD130,Addr(&CVehicle::DoBoatSplashes),FakeSplashes);
        P(0x4A9BE0,Addr(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),FakeCreateFx);
        P(0x4AA2F0,Addr(&FxSystem_c::Play),&Void0<12>); P(0x4AA390,Addr(&FxSystem_c::Stop),&Void0<13>); P(0x4AA3F0,Addr(&FxSystem_c::Kill),&Void0<14>);
        P(0x4AA890,Addr(&FxSystem_c::CopyParentMatrix),&Void0<15>); P(0x4AA900,Addr(&FxSystem_c::GetPlayStatus),FakeFxStatus); P(0x4AA910,Addr(&FxSystem_c::SetLocalParticles),FakeFxLocal);
        P(0x4AA630,Addr(&FxSystem_c::SetMatrix),FakeFxSetMatrix); P(0x4AA6C0,Addr(&FxSystem_c::SetConstTime),FakeFxConstTime);
        P(0x41DA30,Addr(&CCarAI::UpdateCarAI),FakeCarAI); P(0x437C20,Addr(&CCarCtrl::SteerAICarWithPhysics),FakeSteerAI);
        P(0x6DBCE0,Addr(&CVehicle::ProcessBoatControl),FakeBoatControl); P(0x6D85F0,Addr(&CVehicle::FlyingControl),FakeFlying);
        P(0x5EFFE0,Addr(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),FakeSay);
        P(0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegRef);
        P(0x571A00,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanRef);
        P(0x4AB420,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeEventAdd);
        P(0x540080,Addr(&CPad::GetBrake),FakeBrake); P(0x5403F0,Addr(&CPad::GetAccelerate),FakeAccel); P(0x53FB80,Addr(&CPad::GetSteeringLeftRight),FakeSteer);
        P(0x563220,Addr(&CWorld::Add),FakeWorldAdd); P(0x563280,Addr(&CWorld::Remove),FakeWorldRemove);
        P(0x7EF450,Addr(&RwIm3DTransform),FakeIm3DTransform); P(0x7EF550,Addr(&RwIm3DRenderIndexedPrimitive),FakeIm3DRender); P(0x7EF520,Addr(&RwIm3DEnd),FakeIm3DEnd);
        P(0x7322F0,Addr(&CVisibilityPlugins::SetClumpForAllAtomicsFlag),FakeClumpFlag); P(0x50A9F0,Addr(&CCamera::CamShake),FakeCamShake);
        P(0x736A50,Addr(&CExplosion::AddExplosion),FakeExplosion); P(0x43DF20,Addr(&CDarkel::RegisterCarBlownUpByPlayer),FakeDarkel);
        P(0x5A1EE0,Addr(static_cast<void*(*)(size_t)>(&CObject::operator new)),FakeObjNew); P(0x5A1D10,dllObjCtor,FakeObjCtor);
        P(0x59F330,Addr(&CObject::RefModelInfo),FakeRefModel); P(0x7F0410,Addr(&RwFrameCreate),FakeFrameCreate); P(0x749E60,Addr(&RpAtomicClone),FakeAtomicClone);
        P(0x74BF20,Addr(&RpAtomicSetFrame),FakeAtomicSetFrame); P(0x7328A0,Addr(&CVisibilityPlugins::SetAtomicRenderCallback),FakeRenderCB);
        P(0x533ED0,Addr(&CEntity::AttachToRwObject),FakeAttach);
        void* const savedRs=*rsSlot; *rsSlot=reinterpret_cast<void*>(&FakeRenderState);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            if((nm=="Constructor"&&!dllVehCtor)||(nm=="Destructor"&&!dllVehDtor)||(nm=="BlowUpCar"&&!dllObjCtor)){ passed=false; Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","DLL constructor/destructor not found"}}); continue; }
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    crumbOn=i<4; liveTrace=nm=="BlowUpCar"&&i<8; if(liveTrace)Write({{"event","step"},{"function",full},{"case",i},{"candidate",candidate}});
                    Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xB0A7u}; if(!r.s)r.s=1;
                    nrec=0; recOverflow=false; seed=uint32(i); rngCalls=0; Phase("restore"); RestoreRegions(); *rsSlot=reinterpret_cast<void*>(&FakeRenderState);
                    Phase("hooks"); for(auto* h:hooks)h->State(candidate);
                    for(auto& e:p.v)e->Enable(true); // los parches de entrada se reafirman después de fijar los hooks
                    Phase("hooks_set");
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    json out; out["switches"]=fn.slot>=0 ? ((target<0x400000||target>=0x1000000)==candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    Crumb(candidate ? "<run candidate>" : "<run original>",uint32(f*100000+i));
                    json ret=Run(nm,target,r,out);
                    Crumb("<returned>",uint32(f*100000+i));
                    trace=RecsToJson(); // el registro de los stubs se convierte a json fuera de la llamada
                    RestoreRegions(); *rsSlot=reinterpret_cast<void*>(&FakeRenderState);
                    out["ret"]=ret; out["trace"]=trace; liveTrace=false; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        *rsSlot=savedRs;
    }
    if(veh)RemoveVectoredExceptionHandler(veh);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    crumbOn=false; if(crumb!=INVALID_HANDLE_VALUE){ CloseHandle(crumb); crumb=INVALID_HANDLE_VALUE; }
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()}});
}
}
