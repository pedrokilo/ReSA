#pragma once
// Banco dentro del motor: CVehicleRecording (lista de 28, 29 funciones). Original (hooks de CVehicleRecording
// desactivados, código del EXE) frente a candidato con fixtures deterministas. Nunca se cargan ficheros carrec reales
// ni se tocan el streaming ni vehículos reales: las grabaciones son buffers falsos de CVehicleStateEachFrame (con
// holgura legible después del final), los vehículos son objetos FALSOS con desplazamientos crudos del EXE (matriz
// propia, vtable con trampas y ProcessControlCollisionCheck en el hueco 23). Se simulan y trazan (dirección del EXE +
// símbolo/cuerpo de la DLL): CStreaming::RemoveModel/RequestModel, CEntity::RegisterReference, CPhysical::RemoveAndAdd,
// CEntity::UpdateRwMatrix/UpdateRwFrame, CTrain::FindPositionOnTrackFromCoors,
// MarkSurroundingEntitiesForCollisionWithTrain y las primitivas RW (RwStreamRead/Close, RwIm3DTransform,
// RwIm3DRenderIndexedPrimitive, RwIm3DEnd). CMemoryMgr::Malloc/Free y printf (0x821982) se sustituyen solo durante la
// llamada. Los stubs no reservan memoria: guardan valores crudos en un array fijo y el json se construye después de la
// llamada. Todo el estado global de CVehicleRecording (0x97D670..0x97F634: índices de streaming, banderas, velocidades,
// tiempos, tamaños, índices, buffers, vehículos, StreamingArray y NumPlayBackFiles), el modo de CReplay, los relojes
// y los índices temporales de RW se guardan y restauran en cada ejecución y al final.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "VehicleRecording.h"
#include "Streaming.h"
#include "MemoryMgr.h"
#include "Replay.h"
#include "Entity/Vehicle/Train.h"
#include "../ProbeRuntime.hpp"

namespace VehRec29Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int32> l){ return l.begin()[next()%l.size()]; } };
inline DWORD mainThread{};
inline bool Main(){ return GetCurrentThreadId()==mainThread; }

// --- regiones globales guardadas ---
constexpr uintptr_t G_BEGIN=0x97D670, G_END=0x97F634; // PlayBackStreamingIndex .. NumPlayBackFiles
constexpr uintptr_t SA_BEGIN=0x97D880; constexpr size_t SA_N=475;
constexpr uintptr_t TEMPIDX=0xC4B958, TEMPVERTS=0xC4D958;
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- módulo de la DLL ---
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

// ======================= objetos falsos =======================
constexpr size_t NVEH=4, VSTRIDE=0x620, MATB=0x48, NREC=8, MAXF=24, FRB=0x20, RECB=MAXF*FRB+0x40, NENT=10, NSLOT=80, ARENA=0x1000, BLOB=0x800;
alignas(16) inline std::array<uint8,VSTRIDE*NVEH> vehStore{};
alignas(16) inline std::array<std::array<uint8,MATB>,NVEH+1> mats{}; // NVEH = matriz suelta para RestoreInfoForMatrix
alignas(16) inline std::array<uint8,RECB*NREC> recStore{};
alignas(16) inline std::array<uint8,ARENA> arena{};
alignas(16) inline std::array<uint8,BLOB> blob{};
alignas(16) inline std::array<uint8,0x20> streamObj{};
inline std::array<uint32,NREC> recFrames{};
inline std::array<void*,NSLOT> vehVt{};
inline uint8* Veh(size_t k){ return vehStore.data()+k*VSTRIDE; }
inline uint8* Rec(size_t k){ return recStore.data()+k*RECB; }
inline uint8* SA(size_t k){ return reinterpret_cast<uint8*>(SA_BEGIN+k*16); }

// --- nombres de punteros (solo fuera de las llamadas probadas) ---
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v==uintptr_t(vehVt.data()))return "veh_vt";
    struct R { const void* p; size_t n, stride; const char* tag; };
    const R rs[]{{vehStore.data(),vehStore.size(),VSTRIDE,"veh"},{mats.data(),sizeof(mats),MATB,"mat"},{recStore.data(),recStore.size(),RECB,"rec"},
                 {arena.data(),arena.size(),arena.size(),"arena"},{blob.data(),blob.size(),blob.size(),"blob"},{streamObj.data(),streamObj.size(),streamObj.size(),"stream"}};
    for(auto& r:rs)if(v>=uintptr_t(r.p)&&v<uintptr_t(r.p)+r.n)return std::string(r.tag)+std::to_string((v-uintptr_t(r.p))/r.stride)+"+"+std::to_string((v-uintptr_t(r.p))%r.stride);
    if(v>=G_BEGIN&&v<G_END)return "g+"+std::to_string(v-G_BEGIN);
    if(v==TEMPIDX)return "tempidx"; if(v==TEMPVERTS)return "tempverts";
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    return v; }
inline json Words(const void* p,size_t from,size_t to){ json r=json::array(); for(size_t k=from;k+4<=to;k+=4)r.push_back(Name(At<uint32>(p,k))); return r; }

// --- registro de stubs sin reservas: tipo + valores crudos en un array fijo ---
struct I { int32 v; }; struct U { uint32 v; }; struct Fl { float v; }; struct Pt { const void* p; }; struct Bo { bool v; }; struct V3 { const void* p; };
struct Arg { uint8 kind; uint32 v; };
struct Rec_ { const char* tag; uint32 n; std::array<Arg,8> a; };
inline std::array<Rec_,1024> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Put(Rec_& r,uint8 k,uint32 v){ if(r.n<r.a.size())r.a[r.n++]={k,v}; }
inline void Add(Rec_& r,I x){ Put(r,0,uint32(x.v)); }
inline void Add(Rec_& r,U x){ Put(r,1,x.v); }
inline void Add(Rec_& r,Fl x){ Put(r,1,std::bit_cast<uint32>(x.v)); }
inline void Add(Rec_& r,Pt x){ Put(r,3,uint32(uintptr_t(x.p))); }
inline void Add(Rec_& r,Bo x){ Put(r,4,x.v ? 1u : 0u); }
inline void Add(Rec_& r,V3 x){ const auto* f=static_cast<const uint32*>(x.p); for(int k=0;k<3;++k)Put(r,1,f[k]); }
template<class... A> void T(const char* tag,A... a){
    if(!Main())return; if(nrec>=recs.size()){ recOverflow=true; return; }
    Rec_& r=recs[nrec++]; r.tag=tag; r.n=0; (Add(r,a),...); }
inline json RecsToJson(){
    json out=json::array();
    for(size_t i=0;i<nrec;++i){ const Rec_& r=recs[i]; json j=json::array(); j.push_back(r.tag);
        for(uint32 k=0;k<r.n;++k){ const Arg& a=r.a[k];
            switch(a.kind){ case 0: j.push_back(int32(a.v)); break; case 3: j.push_back(Name(uintptr_t(a.v))); break; case 4: j.push_back(a.v!=0); break; default: j.push_back(a.v); } }
        out.push_back(std::move(j)); }
    if(recOverflow)out.push_back("overflow");
    return out; }

// trampas de vtable
template<int N> void* __fastcall VTrap(void* v,void*) { T("unexpected_slot",I{N},Pt{v}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,NSLOT>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
inline void __fastcall VCollisionCheck(void* self,void*,uint32 applySpeed){ T("ProcessControlCollisionCheck",Pt{self},U{applySpeed&0xFF}); }

// ======================= dependencias simuladas =======================
inline size_t arenaTop{};
inline void* __cdecl FakeMalloc(uint32 n,uint32 h){
    if(!Main())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);
    void* p=n<=arena.size()-arenaTop ? arena.data()+arenaTop : nullptr; if(p)arenaTop+=(size_t(n)+15)&~size_t(15);
    T("Malloc",U{n},U{h},Pt{p}); return p; }
inline void __cdecl FakeFree(void* p){
    if(!Main()){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    T("Free",Pt{p}); }
inline int __cdecl FakePrintf(const char* fmt,...){ T("printf",U{uint32(Fnv(fmt,std::strlen(fmt)))}); return 0; }
inline void __cdecl FakeRemoveModel(int32 id){ T("RemoveModel",I{id}); }
inline void __cdecl FakeRequestModel(int32 id,int32 flags){ T("RequestModel",I{id},I{flags}); }
inline void __fastcall FakeRegRef(void* self,void*,void* field){ T("RegisterReference",Pt{self},Pt{field}); }
inline void __fastcall FakeRemoveAndAdd(void* self,void*){ T("RemoveAndAdd",Pt{self}); }
inline void __fastcall FakeUpdateRwMatrix(void* self,void*){ T("UpdateRwMatrix",Pt{self}); }
inline void __fastcall FakeUpdateRwFrame(void* self,void*){ T("UpdateRwFrame",Pt{self}); }
inline void __fastcall FakeFindOnTrack(void* self,void*){ T("FindPositionOnTrackFromCoors",Pt{self}); }
inline void __cdecl FakeMark(CVector pos,float radius,CEntity* e,bool only){ T("MarkSurroundingEntitiesForCollisionWithTrain",V3{&pos},Fl{radius},Pt{e},Bo{only}); }
inline uint32 readSize{}; inline bool transformOk{};
inline uint32 __cdecl FakeStreamRead(void* s,void* buf,uint32 len){
    const uint32 n=std::min(readSize,len); if(buf&&n)std::memcpy(buf,blob.data(),n);
    T("RwStreamRead",Pt{s},Pt{buf},U{len},U{n}); return n; }
inline int32 __cdecl FakeStreamClose(void* s,void* data){ T("RwStreamClose",Pt{s},Pt{data}); return 1; }
inline void* __cdecl FakeIm3DTransform(void* v,uint32 n,void* ltm,uint32 flags){ T("RwIm3DTransform",Pt{v},U{n},Pt{ltm},U{flags}); return transformOk ? v : nullptr; }
inline int32 __cdecl FakeIm3DRenderIndexed(int32 type,void* idx,int32 n){
    T("RwIm3DRenderIndexedPrimitive",I{type},Pt{idx},I{n},U{uint32(Fnv(idx,size_t(std::clamp(n,0,0x400))*2))}); return 1; }
inline int32 __cdecl FakeIm3DEnd(){ T("RwIm3DEnd"); return 1; }

// ======================= fixtures =======================
// fotogramas: tiempos crecientes (a veces iguales, desordenados o enormes), valores empaquetados con extremos
inline int16 RandI16(Rng& r){ return int16(r.next()%5 ? int32(r.next()%4001)-2000 : r.pickI({-32768,32767,-1,0,1,16383,-16384})); }
inline int8 RandI8(Rng& r){ return int8(r.next()%4 ? int32(r.next()%255)-127 : r.pickI({-128,127,0,-1,1,100,-100})); }
inline void MakeFrame(Rng& r,uint8* f,uint32 time,const CVector& c,float spread){
    At<uint32>(f,0)=time;
    for(int k=0;k<3;++k)At<int16>(f,4+2*k)=RandI16(r);
    for(int k=0;k<6;++k)At<int8>(f,0xA+k)=RandI8(r);
    At<int8>(f,0x10)=RandI8(r); At<int8>(f,0x11)=RandI8(r); At<int8>(f,0x12)=RandI8(r);
    f[0x13]=uint8(r.next()%3 ? r.next()%2 : r.next());
    At<CVector>(f,0x14)=CVector{c.x+r.uni(-spread,spread),c.y+r.uni(-spread,spread),c.z+r.uni(-spread*0.2f,spread*0.2f)}; }
inline uint32 TimeStep(Rng& r){ return uint32(r.pickI({0,1,33,50,100,250,int32(r.next()%400)})); }
inline void MakeRec(size_t k,Rng& r,uint32 n,bool hugeTimes,const CVector& c){
    uint8* b=Rec(k); for(size_t q=0;q<RECB;q+=4)At<uint32>(b,q)=r.next()&0x3FFFFFFFu; // holgura legible y determinista (sin NaN)
    uint32 t=hugeTimes ? r.next() : (r.next()%4 ? 0u : r.next()%5000);
    for(uint32 i=0;i<n;++i){ MakeFrame(r,b+i*FRB,t,c,r.pick({5.f,30.f,200.f})); t+=hugeTimes ? r.next() : TimeStep(r); if(r.next()%12==0)t-=std::min<uint32>(t,r.next()%300); }
    recFrames[k]=n; }

inline uint8* MakeVeh(size_t k,Rng& r,const CVector& pos,bool withMat){
    uint8* v=Veh(k); for(size_t q=0;q<VSTRIDE;q+=4)At<uint32>(v,q)=r.next()&0x3F7FFFFFu;
    At<void*>(v,0)=vehVt.data(); At<CVector>(v,4)=pos;
    auto* m=mats[k].data(); std::memset(m,0,MATB);
    if(withMat){ CMatrix mm{}; mm.SetRotate(r.uni(-0.5f,0.5f),r.uni(-0.5f,0.5f),r.uni(-3.14f,3.14f)); mm.GetPosition()=pos; std::memcpy(m,&mm,0x40);
        for(size_t q:{0xCu,0x1Cu,0x2Cu,0x3Cu})At<uint32>(m,q)=r.next(); At<uint32>(m,0x40)=0; At<uint32>(m,0x44)=0;
        At<void*>(v,0x14)=m; }
    else At<void*>(v,0x14)=nullptr;
    At<uint32>(v,0x40)=r.next()&~0x20000000u; if(r.next()%10==0)At<uint32>(v,0x40)|=0x20000000u;
    At<CVector>(v,0x44)=CVector{r.uni(-1.f,1.f),r.uni(-1.f,1.f),r.uni(-0.2f,0.2f)}; At<CVector>(v,0x50)=CVector{r.uni(-0.1f,0.1f),r.uni(-0.1f,0.1f),r.uni(-0.1f,0.1f)};
    At<int8>(v,0x424)=int8(r.next()%3 ? -1 : int32(r.next()%16));
    At<float>(v,0x494)=r.uni(-0.6f,0.6f); At<float>(v,0x498)=r.uni(-1.f,1.f); At<float>(v,0x49C)=r.uni(0.f,1.f); At<float>(v,0x4A0)=r.uni(0.f,1.f);
    At<uint32>(v,0x590)=uint32(r.pickI({0,0,9,9,5,6,3})); At<uint32>(v,0x594)=uint32(r.pickI({0,0,6,6,9,5}));
    v[0x614]=uint8(r.next());
    return v; }

// estado de reproducción coherente: buffer/tamaño copiados de la entrada; buffer nulo => tamaño 0
inline void SetEntry(size_t e,int32 number,int32 rec,uint32 sizeExtra,int8 refs){
    uint8* s=SA(e); At<int32>(s,0)=number;
    if(rec>=0){ At<void*>(s,4)=Rec(size_t(rec)); At<int32>(s,8)=int32(recFrames[size_t(rec)]*FRB+sizeExtra); }
    else { At<void*>(s,4)=nullptr; At<int32>(s,8)=0; }
    At<int8>(s,0xC)=refs; }
inline void SetSlot(Rng& r,int32 i,void* veh,size_t entry,bool aiAllowed=true){
    auto& V=CVehicleRecording::pVehicleForPlayback; auto& B=CVehicleRecording::pPlaybackBuffer; auto& S=CVehicleRecording::PlaybackBufferSize;
    CVehicleRecording::bPlaybackGoingOn[i]=true; V[i]=static_cast<CVehicle*>(veh);
    CVehicleRecording::PlayBackStreamingIndex[i]=uint32(entry);
    B[i]=At<CVehicleStateEachFrame*>(SA(entry),4); S[i]=At<int32>(SA(entry),8);
    const int32 n=S[i]/int32(FRB); CVehicleRecording::PlaybackIndex[i]=n>0 ? int32(r.next()%uint32(n))*int32(FRB) : 0;
    CVehicleRecording::bUseCarAI[i]=aiAllowed&&r.next()%4==0; CVehicleRecording::bPlaybackLooped[i]=r.next()%2; CVehicleRecording::bPlaybackPaused[i]=r.next()%5==0;
    CVehicleRecording::PlaybackSpeed[i]=r.pick({1.f,1.f,0.5f,2.f,0.f,-1.f,r.uni(0.f,3.f)});
    const uint32 t=n>0 ? At<uint32>(B[i],uint32(CVehicleRecording::PlaybackIndex[i])) : 0u;
    CVehicleRecording::PlaybackRunningTime[i]=r.next()%3 ? float(t)+r.uni(-80.f,400.f) : r.uni(0.f,6000.f); }

// mundo común: estado global a cero + grabaciones/entradas/ranuras/vehículos falsos
inline void MakeWorld(Rng& r,bool hugeTimes=false){
    std::memset(reinterpret_cast<void*>(G_BEGIN),0,G_END-G_BEGIN);
    arena.fill(0xCD); arenaTop=0; readSize=0; transformOk=true;
    for(size_t q=0;q<blob.size();q+=4)At<uint32>(blob.data(),q)=r.next()&0x3FFFFFFFu;
    const CVector c{r.uni(-2000.f,2000.f),r.uni(-2000.f,2000.f),r.uni(0.f,50.f)};
    for(size_t k=0;k<NREC;++k)MakeRec(k,r,1+r.next()%MAXF,hugeTimes,c);
    for(size_t k=0;k<NVEH;++k)MakeVeh(k,r,CVector{c.x+r.uni(-100.f,100.f),c.y+r.uni(-100.f,100.f),c.z},true);
    for(auto& m:mats[NVEH])m=0;
    static const int32 numbers[]{100,101,102,103,104,105,106,107,101,850};
    for(size_t e=0;e<NENT;++e)SetEntry(e,numbers[e],r.next()%5 ? int32(e%NREC) : -1,0,int8(r.next()%4 ? r.next()%3 : r.next()));
    if(r.next()%3==0)SetEntry(300,int32(r.next()%2 ? 102 : 300),int32(r.next()%NREC),0,int8(r.next()%2));
    if(r.next()%3==0)SetEntry(474,int32(r.next()%2 ? 104 : 474),int32(r.next()%NREC),0,0);
    CVehicleRecording::NumPlayBackFiles=r.next()%4 ? int32(NENT) : int32(r.next()%(NENT+1));
    CTimer::m_snTimeInMilliseconds=100000+r.next()%1000000;
    CTimer::m_snPPPPreviousTimeInMilliseconds=CTimer::m_snTimeInMilliseconds-(r.next()%8 ? r.next()%120 : r.next()%5000);
    CReplay::Mode=eReplayMode(0);
}
// ranuras activas con vehículos falsos (cada vehículo en una ranura, a veces repetido o nulo)
inline void FillSlots(Rng& r,bool needBuffers,bool aiAllowed=true){
    for(int32 i=0;i<TOTAL_VEHICLE_RECORDS;++i){
        if(r.next()%2)continue;
        size_t e=r.next()%NENT; if(needBuffers)for(size_t k=0;k<NENT&&!At<void*>(SA(e),4);++k)e=(e+1)%NENT;
        void* veh=r.next()%8==0 ? nullptr : Veh(r.next()%NVEH);
        SetSlot(r,i,veh,e,aiAllowed); }
}
inline int32 SlotOf(void* veh){ for(int32 i=0;i<TOTAL_VEHICLE_RECORDS;++i)if(CVehicleRecording::bPlaybackGoingOn[i]&&CVehicleRecording::pVehicleForPlayback[i]==veh)return i; return 16; }
// fuerza que veh esté en una ranura activa con buffer válido
inline int32 EnsureSlot(Rng& r,void* veh,bool aiAllowed=true){
    int32 i=SlotOf(veh); if(i<16&&CVehicleRecording::pPlaybackBuffer[i]&&CVehicleRecording::PlaybackBufferSize[i]>=int32(FRB))return i;
    if(i>=16)i=int32(r.next()%16);
    size_t e=r.next()%NENT; for(size_t k=0;k<NENT&&!At<void*>(SA(e),4);++k)e=(e+1)%NENT;
    if(!At<void*>(SA(e),4))SetEntry(e,At<int32>(SA(e),0),int32(e%NREC),0,1);
    for(int32 j=0;j<i;++j)if(CVehicleRecording::bPlaybackGoingOn[j]&&CVehicleRecording::pVehicleForPlayback[j]==veh)CVehicleRecording::pVehicleForPlayback[j]=nullptr;
    SetSlot(r,i,veh,e,aiAllowed); return i; }

struct V3f { float x,y,z; };
inline json Run(const std::string& nm,uintptr_t a,Rng& r,const std::function<void(const std::function<void()>&)>& call){
    json ret=nullptr; using VP=void*;
    if(nm=="Init"||nm=="InitAtStartOfGame"||nm=="ShutDown"||nm=="RemoveAllRecordingsThatArentUsed"||nm=="Render"){
        MakeWorld(r); FillSlots(r,false);
        call([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); }); }
    else if(nm=="ChangeCarPlaybackToUseAI"){
        MakeWorld(r); FillSlots(r,false); uint8* v=Veh(r.next()%NVEH);
        if(r.next()%4==0){ At<void*>(v,0x14)=nullptr; }
        if(r.next()%3)EnsureSlot(r,v);
        // índice 16: el EXE lee como buffer pVehicleForPlayback[0] y como tamaño PlaybackIndex[0] (y escribe pPlaybackBuffer[0]);
        // se acota para que solo lea dentro del vehículo falso 0..3 (o nada)
        if(SlotOf(v)>=16){ CVehicleRecording::PlaybackIndex[0]=CVehicleRecording::pVehicleForPlayback[0] ? r.pickI({0,0x20,0x100,0x600,-0x20}) : r.pickI({0,-1}); }
        call([&]{ reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }); }
    else if(nm=="FindIndexWithFileNameNumber"||nm=="HasRecordingFileBeenLoaded"||nm=="RemoveRecordingFile"||nm=="RequestRecordingFile"){
        MakeWorld(r); if(r.next()%6==0)CVehicleRecording::NumPlayBackFiles=r.pickI({-1,-5,0});
        if(nm=="RemoveRecordingFile"&&r.next()%2)SetEntry(5,101,int32(r.next()%NREC),0,0);
        const int32 n=r.pickI({100,101,102,103,104,105,106,107,850,300,474,0,-1,999});
        if(nm=="FindIndexWithFileNameNumber"){ uint32 res{}; call([&]{ res=reinterpret_cast<uint32(__cdecl*)(int32)>(a)(n); }); ret=res; }
        else if(nm=="HasRecordingFileBeenLoaded"){ uint8 res{}; call([&]{ res=reinterpret_cast<uint8(__cdecl*)(int32)>(a)(n); }); ret=res; }
        else call([&]{ reinterpret_cast<void(__cdecl*)(int32)>(a)(n); }); }
    else if(nm=="InterpolateInfoForCar"){
        MakeWorld(r); uint8* v=Veh(r.next()%NVEH); const uint8* f=Rec(r.next()%NREC)+FRB*(r.next()%recFrames[0]);
        const float t=r.pick({0.f,1.f,0.5f,0.25f,r.uni(0.f,1.f),r.uni(0.f,1.f),r.uni(-0.5f,1.5f),-0.f});
        call([&]{ reinterpret_cast<void(__cdecl*)(VP,const void*,float)>(a)(v,f,t); }); }
    else if(nm=="Load"){
        MakeWorld(r,r.next()%3==0);
        // fotogramas del fichero: a veces con un tiempo 0 a partir del segundo (recorte)
        const uint32 nf=1+r.next()%40; for(uint32 i=0;i<nf;++i){ uint8* f=blob.data()+i*FRB; MakeFrame(r,f,r.next()%3 ? 100+i*50 : r.next(),CVector{0.f,0.f,0.f},50.f); }
        if(r.next()%3){ const uint32 z=r.next()%nf; At<uint32>(blob.data(),z*FRB)=0; }
        if(r.next()%4==0)At<uint32>(blob.data(),0)=0;
        readSize=std::max<uint32>(FRB,nf*FRB+(r.next()%4==0 ? uint32(r.next()%FRB) : 0u)); // >= 0x20: con menos, el EXE recorre memoria fuera del buffer
        const int32 total=int32(std::min<uint32>(uint32(arena.size()),readSize+uint32(r.next()%3 ? 0 : r.next()%256)));
        readSize=std::min<uint32>(readSize,uint32(total));
        const int32 id=r.pickI({0,1,2,5,9,300,474,int32(r.next()%NENT)});
        call([&]{ reinterpret_cast<void(__cdecl*)(VP,int32,int32)>(a)(streamObj.data(),id,total); }); }
    else if(nm=="SmoothRecording"){
        MakeWorld(r,r.next()%3==0); const size_t e=r.next()%NENT; const uint32 k=uint32(r.next()%NREC);
        const uint32 extra=r.next()%4==0 ? uint32(r.next()%FRB) : 0u; SetEntry(e,At<int32>(SA(e),0),int32(k),extra,0);
        if(r.next()%6==0){ At<int32>(SA(e),8)=int32(FRB+r.next()%(FRB+1)); } // tamaños 0x20..0x40: no suaviza
        call([&]{ reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(e)); }); }
    else if(nm=="RegisterRecordingFile"){
        MakeWorld(r); static const char* const names[]{"carrec123","CARREC77","carrec","CARREC","abc","","Carrec5","carrec-4","CARREC 9","carrec0012x","carrec850","CARREC2147483647","x"};
        const char* name=names[r.next()%std::size(names)]; CVehicleRecording::NumPlayBackFiles=r.next()%2 ? int32(r.next()%NENT) : int32(r.next()%470);
        int32 res{}; call([&]{ res=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(name); }); ret=res; }
    else if(nm=="StopPlaybackWithIndex"){
        MakeWorld(r); FillSlots(r,false); const int32 i=int32(r.next()%16);
        if(r.next()%3)SetSlot(r,i,r.next()%6 ? static_cast<void*>(Veh(r.next()%NVEH)) : nullptr,r.next()%NENT);
        if(r.next()%3==0)At<int8>(SA(CVehicleRecording::PlayBackStreamingIndex[i]),0xC)=1;
        call([&]{ reinterpret_cast<void(__cdecl*)(int32)>(a)(i); }); }
    else if(nm=="StartPlaybackRecordedCar"){
        MakeWorld(r); FillSlots(r,false); uint8* v=Veh(r.next()%NVEH); if(r.next()%5==0)At<void*>(v,0x14)=nullptr;
        const int32 n=r.pickI({100,101,102,103,104,105,106,107,850,999});
        const bool ai=r.next()%2, looped=r.next()%2;
        if(r.next()%8==0){ // las 16 ranuras ocupadas: el EXE usa el índice 16 (solapa con otros arrays, todo dentro de la región guardada)
            for(int32 i=0;i<16;++i){ CVehicleRecording::bPlaybackGoingOn[i]=true; CVehicleRecording::PlaybackRunningTime[i]=0.f; }
            const auto idx=CVehicleRecording::FindIndexWithFileNameNumber(n); if(!At<void*>(SA(idx),4))SetEntry(idx,At<int32>(SA(idx),0),int32(idx%NREC),0,0); }
        call([&]{ reinterpret_cast<void(__cdecl*)(VP,int32,bool,bool)>(a)(v,n,ai,looped); }); }
    else if(nm=="StopPlaybackRecordedCar"||nm=="PausePlaybackRecordedCar"||nm=="UnpausePlaybackRecordedCar"||nm=="IsPlaybackGoingOnForCar"||nm=="IsPlaybackPausedForCar"||nm=="SetPlaybackSpeed"){
        MakeWorld(r); FillSlots(r,false); void* v=r.next()%10==0 ? nullptr : Veh(r.next()%NVEH);
        if(v&&r.next()%3)EnsureSlot(r,v);
        if(nm=="IsPlaybackGoingOnForCar"||nm=="IsPlaybackPausedForCar"){ uint8 res{}; call([&]{ res=reinterpret_cast<uint8(__cdecl*)(VP)>(a)(v); }); ret=res; }
        else if(nm=="SetPlaybackSpeed"){ const float s=r.pick({0.f,1.f,2.f,-1.f,r.uni(0.f,4.f)}); call([&]{ reinterpret_cast<void(__cdecl*)(VP,float)>(a)(v,s); }); }
        else call([&]{ reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }); }
    else if(nm=="RenderLineSegment"){
        MakeWorld(r); int32 n=r.pickI({-3,-1,0,1,2,3,int32(r.next()%64),int32(r.next()%64)}); transformOk=r.next()%3!=0;
        for(size_t q=0;q<0x100;q+=4)At<uint32>(reinterpret_cast<void*>(TEMPIDX),q)=r.next();
        call([&]{ reinterpret_cast<void(__cdecl*)(int32*)>(a)(&n); }); ret=n; }
    else if(nm=="RestoreInfoForCar"){
        MakeWorld(r); uint8* v=Veh(r.next()%NVEH); const uint8* f=Rec(r.next()%NREC)+FRB*(r.next()%recFrames[1]);
        const bool pause=r.next()%3==0;
        call([&]{ reinterpret_cast<void(__cdecl*)(VP,const void*,bool)>(a)(v,f,pause); }); }
    else if(nm=="RestoreInfoForMatrix"){
        MakeWorld(r); uint8* m=mats[NVEH].data(); for(size_t q=0;q<0x40;q+=4)At<uint32>(m,q)=r.next()&0x3F7FFFFFu;
        const uint8* f=Rec(r.next()%NREC)+FRB*(r.next()%recFrames[2]);
        call([&]{ reinterpret_cast<void(__cdecl*)(VP,const void*)>(a)(m,f); }); }
    else if(nm=="SaveOrRetrieveDataForThisFrame"){
        MakeWorld(r,r.next()%8==0); FillSlots(r,true); // a veces tiempos >= 2^24 y >= 2^31 (conversiones del fild)
        // ranuras activas sin IA: buffer válido (el EXE lee el fotograma actual y el siguiente sin comprobar)
        for(int32 i=0;i<16;++i)if(CVehicleRecording::bPlaybackGoingOn[i]&&CVehicleRecording::PlaybackBufferSize[i]<int32(FRB))CVehicleRecording::bUseCarAI[i]=true;
        if(r.next()%10==0)CReplay::Mode=eReplayMode(1); else if(r.next()%10==0)CReplay::Mode=eReplayMode(2);
        call([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); }); }
    else if(nm=="SetRecordingToPointClosestToCoors"){
        MakeWorld(r); FillSlots(r,false); const int32 i=int32(r.next()%16);
        if(r.next()%4)SetSlot(r,i,Veh(0),r.next()%NENT);
        V3f p{}; const auto* b=CVehicleRecording::pPlaybackBuffer[i]; const int32 n=CVehicleRecording::PlaybackBufferSize[i]/int32(FRB);
        if(b&&n>0&&r.next()%2){ const auto q=At<CVector>(b,size_t(r.next()%uint32(n))*FRB+0x14); p={q.x+r.uni(-3.f,3.f),q.y+r.uni(-3.f,3.f),q.z}; }
        else p={r.uni(-2500.f,2500.f),r.uni(-2500.f,2500.f),r.uni(-10.f,60.f)};
        if(b&&n>2&&r.next()%4==0){ At<CVector>(const_cast<CVehicleStateEachFrame*>(b),FRB+0x14)=At<CVector>(b,0x14); } // empate de distancia: gana el primero
        call([&]{ reinterpret_cast<void(__cdecl*)(int32,V3f)>(a)(i,p); }); }
    else if(nm=="SkipForwardInRecording"){
        MakeWorld(r); FillSlots(r,false); uint8* v=Veh(r.next()%NVEH); const int32 i=EnsureSlot(r,v);
        const float d=r.pick({0.f,-0.f,1.f,-1.f,10.f,-10.f,50.f,-50.f,500.f,-500.f,1e6f,-1e6f,r.uni(-300.f,300.f)});
        if(r.next()%5==0){ auto* b=reinterpret_cast<uint8*>(CVehicleRecording::pPlaybackBuffer[i]); for(int32 k=1;k*int32(FRB)<CVehicleRecording::PlaybackBufferSize[i];k+=2)At<CVector>(b,size_t(k)*FRB+0x14)=At<CVector>(b,size_t(k-1)*FRB+0x14); }
        call([&]{ reinterpret_cast<void(__cdecl*)(VP,float)>(a)(v,d); }); }
    else if(nm=="SkipToEndAndStopPlaybackRecordedCar"){
        MakeWorld(r); FillSlots(r,true); uint8* v=Veh(r.next()%NVEH);
        if(r.next()%4)EnsureSlot(r,v);
        { const int32 i=SlotOf(v); if(i<16&&CVehicleRecording::PlaybackBufferSize[i]<int32(FRB))EnsureSlot(r,v); }
        if(r.next()%3==0){ const int32 i=SlotOf(v); if(i<16)At<int8>(SA(CVehicleRecording::PlayBackStreamingIndex[i]),0xC)=1; }
        call([&]{ reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }); }
    return ret;
}

// ======================= tabla =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=29;
inline const std::array<Fn,NFN> fns{{
    {"CVehicleRecording","ChangeCarPlaybackToUseAI",0x45A360,0,-1,512},{"CVehicleRecording","FindIndexWithFileNameNumber",0x459FF0,0,-1,256},
    {"CVehicleRecording","HasRecordingFileBeenLoaded",0x45A060,0,-1,256},{"CVehicleRecording","Init",0x459390,0,-1,64},
    {"CVehicleRecording","InitAtStartOfGame",0x45A1B0,0,-1,64},{"CVehicleRecording","InterpolateInfoForCar",0x459B30,0,-1,1024},
    {"CVehicleRecording","IsPlaybackGoingOnForCar",0x4594C0,0,-1,256},{"CVehicleRecording","IsPlaybackPausedForCar",0x4595A0,0,-1,64},
    {"CVehicleRecording","Load",0x45A8F0,0,-1,512},{"CVehicleRecording","PausePlaybackRecordedCar",0x459740,0,-1,256},
    {"CVehicleRecording","RegisterRecordingFile",0x459F80,0,-1,256},{"CVehicleRecording","RemoveAllRecordingsThatArentUsed",0x45A160,0,-1,256},
    {"CVehicleRecording","RemoveRecordingFile",0x45A0A0,0,-1,256},{"CVehicleRecording","Render",0x459F70,0,-1,64},
    {"CVehicleRecording","RenderLineSegment",0x459F00,0,-1,256},{"CVehicleRecording","RequestRecordingFile",0x45A020,0,-1,256},
    {"CVehicleRecording","RestoreInfoForCar",0x459A30,0,-1,1024},{"CVehicleRecording","RestoreInfoForMatrix",0x459960,0,-1,512},
    {"CVehicleRecording","SaveOrRetrieveDataForThisFrame",0x45A610,0,-1,1024},{"CVehicleRecording","SetPlaybackSpeed",0x459660,0,-1,256},
    {"CVehicleRecording","SetRecordingToPointClosestToCoors",0x45A1E0,0,-1,512},{"CVehicleRecording","ShutDown",0x459400,0,-1,128},
    {"CVehicleRecording","SkipForwardInRecording",0x459D10,0,-1,512},{"CVehicleRecording","SkipToEndAndStopPlaybackRecordedCar",0x45A4A0,0,-1,256},
    {"CVehicleRecording","SmoothRecording",0x45A0F0,0,-1,512},{"CVehicleRecording","StartPlaybackRecordedCar",0x45A980,0,-1,512},
    {"CVehicleRecording","StopPlaybackRecordedCar",0x45A280,0,-1,512},{"CVehicleRecording","StopPlaybackWithIndex",0x459440,0,-1,512},
    {"CVehicleRecording","UnpausePlaybackRecordedCar",0x459850,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"vehrec29-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"vehrec29-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("vehrec29");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"vehrec29-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    regions.clear();
    Snap(G_BEGIN,G_END-G_BEGIN); Snap(0xA43088,1); Snap(0xB7CB84,4); Snap(0xB7CB6C,4); Snap(TEMPIDX,0x2000);

    FillTraps(vehVt,std::make_integer_sequence<int,int(NSLOT)>{});
    vehVt[23]=reinterpret_cast<void*>(&VCollisionCheck);

    bool passed=true; int pair=0;
    {
        // Destinos resueltos ANTES de escribir ningún parche (FollowIn no debe seguir un salto recién escrito hasta un
        // stub); nunca se parchea la dirección de un stub.
        struct Want { uintptr_t exe, dll; void* stub; bool scoped; };
        std::vector<Want> wants{
            {0x4089A0,Addr(&CStreaming::RemoveModel),reinterpret_cast<void*>(&FakeRemoveModel),false},
            {0x4087E0,Addr(&CStreaming::RequestModel),reinterpret_cast<void*>(&FakeRequestModel),false},
            {0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),reinterpret_cast<void*>(&FakeRegRef),false},
            {0x542560,Addr(&CPhysical::RemoveAndAdd),reinterpret_cast<void*>(&FakeRemoveAndAdd),false},
            {0x446F90,Addr(&CEntity::UpdateRwMatrix),reinterpret_cast<void*>(&FakeUpdateRwMatrix),false},
            {0x532B00,Addr(&CEntity::UpdateRwFrame),reinterpret_cast<void*>(&FakeUpdateRwFrame),false},
            {0x6F6CC0,Addr(&CTrain::FindPositionOnTrackFromCoors),reinterpret_cast<void*>(&FakeFindOnTrack),false},
            {0x6F6640,Addr(&MarkSurroundingEntitiesForCollisionWithTrain),reinterpret_cast<void*>(&FakeMark),false},
            {0x7EC9D0,0,reinterpret_cast<void*>(&FakeStreamRead),false},{0x7ECE20,0,reinterpret_cast<void*>(&FakeStreamClose),false},
            {0x7EF450,0,reinterpret_cast<void*>(&FakeIm3DTransform),false},{0x7EF550,0,reinterpret_cast<void*>(&FakeIm3DRenderIndexed),false},
            {0x7EF520,0,reinterpret_cast<void*>(&FakeIm3DEnd),false},
            {0x72F420,Addr(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),reinterpret_cast<void*>(&FakeMalloc),true},
            {0x72F430,Addr(&CMemoryMgr::Free),reinterpret_cast<void*>(&FakeFree),true},
            {0x821982,0,reinterpret_cast<void*>(&FakePrintf),true},
        };
        std::set<uintptr_t> stubSet; for(auto& w:wants){ stubSet.insert(uintptr_t(w.stub)); stubSet.insert(FollowIn(uintptr_t(w.stub))); }
        struct Target { uintptr_t at; void* stub; bool scoped; };
        std::vector<Target> targets; std::set<uintptr_t> seen;
        for(auto& w:wants){ for(uintptr_t at:{w.exe,w.dll,w.dll ? FollowIn(w.dll) : uintptr_t(0)})if(at&&!stubSet.count(at)&&seen.insert(at).second)targets.push_back({at,w.stub,w.scoped}); }
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> global, scoped;
        for(auto& t:targets){ auto e=std::make_unique<ReversingProbe::EntryPatch>(t.at,t.stub); if(t.scoped){ e->Enable(false); scoped.push_back(std::move(e)); } else global.push_back(std::move(e)); }
        // dependencias sustituidas solo durante la llamada (sin json ni reservas dentro)
        auto call=[&](const std::function<void()>& body){ for(auto& e:scoped)e->Enable(true); body(); for(auto& e:scoped)e->Enable(false); };

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7EC29u}; if(!r.s)r.s=1;
                    nrec=0; recOverflow=false; RestoreRegions();
                    for(auto* h:hooks)h->State(candidate);
                    for(auto& e:global)e->Enable(true); // los parches se reafirman después de fijar los hooks
                    json out; out["switches"]=ReversingProbe::JumpsToDll(fn.address)==candidate; // comprobado DESPUÉS de State
                    json ret=Run(nm,fn.address,r,call);
                    out["ret"]=ret; out["trace"]=RecsToJson();
                    out["g"]=Words(reinterpret_cast<void*>(G_BEGIN),0,SA_BEGIN-G_BEGIN);
                    { json e=json::array(); for(size_t k:{0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,300u,474u})e.push_back(Words(SA(k),0,16)); out["sa"]=e; }
                    out["sa_hash"]=Fnv(reinterpret_cast<void*>(SA_BEGIN),SA_N*16); out["num_files"]=CVehicleRecording::NumPlayBackFiles;
                    { json h=json::array(); for(size_t k=0;k<NVEH;++k){ uint8* v=Veh(k);
                        json w=json::array(); for(size_t q:{0x4u,0x8u,0xCu,0x14u,0x34u,0x40u,0x44u,0x48u,0x4Cu,0x50u,0x54u,0x58u,0x3B8u,0x424u,0x428u,0x42Cu,0x494u,0x498u,0x49Cu,0x4A0u,0x614u})w.push_back(Name(At<uint32>(v,q)));
                        w.push_back(Fnv(v,VSTRIDE)); h.push_back(std::move(w)); } out["vehs"]=h; }
                    // relleno de las matrices enmascarado: InterpolateInfoForCar copia el de una temporal sin inicializar
                    { json h=json::array(); for(auto& m:mats){ json w=json::array(); for(size_t q=0;q<MATB;q+=4)w.push_back((q&0xF)==0xC&&q<0x40 ? json("pad") : json(At<uint32>(m.data(),q))); h.push_back(std::move(w)); } out["mats"]=h; }
                    { json h=json::array(); for(size_t k=0;k<NREC;++k)h.push_back(Fnv(Rec(k),RECB)); out["recs"]=h; }
                    if(nm=="SmoothRecording"||nm=="Load"){ json t=json::array(); for(size_t q=0;q<0x400;q+=FRB)t.push_back(At<uint32>(arena.data(),q)); out["arena_times"]=t;
                        json u=json::array(); for(size_t k=0;k<NREC;++k)for(size_t q=0;q<MAXF*FRB;q+=FRB)u.push_back(At<uint32>(Rec(k),q)); out["rec_times"]=u; }
                    out["arena"]=Fnv(arena.data(),arena.size()); out["tempidx"]=Fnv(reinterpret_cast<void*>(TEMPIDX),0x200);
                    out["replay"]=int32(*reinterpret_cast<int8*>(0xA43088));
                    RestoreRegions();
                    return out;
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
        for(auto& e:scoped)e->Enable(false);
        while(!scoped.empty())scoped.pop_back();
        while(!global.empty())global.pop_back();
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()}});
}
}
