#pragma once
// Banco dentro del motor: tipos de peatón, temporizador y remolque (lista de 13). 39 funciones de CPedType, CTimer y
// CTrailer. Original (hooks del lote desactivados) frente a candidato con fixtures deterministas:
// - CPedType trabaja sobre una tabla falsa de 34 entradas (las entradas 32 y 33 recogen las escrituras fuera de la
//   tabla del EXE); el puntero global 0xC0BBE8 se cambia solo durante la llamada. Se simulan CFileMgr::OpenFile,
//   ReadLine y CloseFile (líneas de PED.DAT generadas), la carga/guardado del búfer de partida y operator new[]/
//   delete[] (solo durante la llamada, registro sin reservas). LoadLine es el real (con hook en los dos lados).
// - CTimer: se guardan y restauran todas las globales del reloj (0xB7CB28..0xB7CB8B) y bSnapShotActive en cada
//   ejecución y al final; QueryPerformanceCounter/Frequency y RsTimer devuelven valores fijos del caso (sin traza en
//   otros hilos) solo durante la llamada. El puntero de la función de reloj se compara por nombre (0x5617A0 del EXE
//   y GetOSWPerformanceTime de la DLL = "perf"; 0x5617C0 y GetMillisecondTime = "ms"). Ninguna función de CTimer
//   tiene HookInstall permanente; los hooks se reponen al terminar.
// - CTrailer: remolque, tractores y entidades falsos (nunca del pool) con vtables de trampas (huecos 59/60/61
//   simulados), model infos falsos (los punteros de ms_modelInfoPtrs de los modelos usados se cambian solo durante
//   la llamada), colmodel, líneas, marcos RW y handling falsos. Las funciones base de CAutomobile se simulan en la
//   dirección del EXE y en el cuerpo de la DLL (tomado del hook virtual); el constructor de CAutomobile de la DLL se
//   localiza como la primera llamada de CTrailer::CTrailer y se cruza con la del envoltorio CAutomobile::Constructor.
//   Se simulan y trazan las dependencias físicas (muelles, velocidades, amortiguación, pares de ruedas, registros de
//   colisión, referencias, listas de movimiento, audio, colocación, búsqueda de objetos, reproducción, rueda del
//   model info, CMemoryMgr::Malloc). CMatrix (Attach/SetTranslate/UpdateRW/SetRotateZOnly), RwMatrixTransform,
//   GetColModel y CDamageManager se ejecutan de verdad sobre los objetos falsos.
#include <fstream>
#include <set>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "PedType.h"
#include "Timer.h"
#include "oswrapper.h"
#include "platform.h"
#include "SpecialFX.h"
#include "GenericGameStorage.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "VehicleRecording.h"
#include "World.h"
#include "Collision/Collision.h"
#include "MemoryMgr.h"
#include "Entity/Vehicle/Trailer.h"
#include "../ProbeRuntime.hpp"

namespace Sys13Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; } };
inline const float QNAN=std::numeric_limits<float>::quiet_NaN();
inline DWORD mainThread{};
inline bool Main(){ return GetCurrentThreadId()==mainThread; }

// --- registro sin reservas (para stubs activos mientras operator new[] está sustituido) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,2048> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(void* p,size_t n){ regions.push_back({p,n,ReversingProbe::Bytes(p,n)}); }
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

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// cuerpo de la DLL de una función con hook (virtual o simple)
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }

// ======================= CPedType =======================
constexpr size_t PT_N=36;
alignas(16) inline std::array<uint8,4+0x14*PT_N> ptBuf{};
alignas(16) inline std::array<uint8,0x300> ptArena{};
inline void* ptNewRet{};
inline void* __cdecl PtNew(size_t n){ Push(1,uint32(n)); return ptNewRet; }
inline uint32 PtOff(const void* p){ const auto u=uintptr_t(p); if(!u)return 0xFFFFFFFFu;
    if(u>=uintptr_t(ptBuf.data())&&u<uintptr_t(ptBuf.data())+ptBuf.size())return 0x10000u+uint32(u-uintptr_t(ptBuf.data()));
    if(u>=uintptr_t(ptArena.data())&&u<uintptr_t(ptArena.data())+ptArena.size())return 0x20000u+uint32(u-uintptr_t(ptArena.data()));
    return 0xEEEEEEEEu; }
inline void __cdecl PtDelete(void* p){ Push(2,PtOff(p)); }
// fichero falso
inline FILESTREAM const FAKE_FILE=reinterpret_cast<FILESTREAM>(0x7E300000);
inline std::vector<std::string> lines; inline size_t lineIdx{};
inline FILESTREAM __cdecl FakeOpenFile(const char* path,const char* mode){ Push(11,uint32(Fnv(path,strlen(path))),uint32(Fnv(mode,strlen(mode)))); return FAKE_FILE; }
inline bool __cdecl FakeReadLine(FILESTREAM f,char* str,int32 num){ Push(10,uint32(lineIdx),f==FAKE_FILE);
    if(lineIdx>=lines.size())return false; const auto& l=lines[lineIdx++]; const size_t n=std::min<size_t>(l.size(),size_t(num-1)); std::memcpy(str,l.data(),n); str[n]=0; return true; }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ Push(12,f==FAKE_FILE); return 0; }
// búfer de partida
inline std::array<uint32,256> loadVals{}; inline size_t loadIdx{};
inline bool __cdecl FakeSaveWB(void* data,int32 size){ Push(20,size==4 ? *static_cast<uint32*>(data) : 0xDEAD0000u,uint32(size)); return true; }
inline bool __cdecl FakeLoadWB(void* data,int32 size){ Push(21,uint32(size)); if(size==4)*static_cast<uint32*>(data)=loadVals[loadIdx++%loadVals.size()]; return true; }

inline const char* const PED_NAMES[32]{"PLAYER1","PLAYER2","PLAYER_NETWORK","PLAYER_UNUSED","CIVMALE","CIVFEMALE","COP","GANG1","GANG2","GANG3","GANG4","GANG5","GANG6","GANG7","GANG8","GANG9",
    "GANG10","DEALER","MEDIC","FIREMAN","CRIMINAL","BUM","PROSTITUTE","SPECIAL","MISSION1","MISSION2","MISSION3","MISSION4","MISSION5","MISSION6","MISSION7","MISSION8"};
inline std::string RandomName(Rng& r){
    switch(r.next()%10){
    case 0: { std::string s=PED_NAMES[r.next()%32]; for(auto& c:s)if(isalpha(uint8(c))&&r.next()%2)c=char(c^0x20); return s; }
    case 1: { std::string s=PED_NAMES[r.next()%32]; s.pop_back(); return s; }
    case 2: { std::string s=PED_NAMES[r.next()%32]; s+=char('0'+r.next()%10); return s; }
    case 3: { static const char* odd[]{"GANG","GANG11","GANG0","PLAYER","MISSION","MISSION9","CIV","NONE","X","Hate","Like","Respect","Dislike","PLAYER_NETWORKX","PLAYER_UNUSE"}; return odd[r.next()%std::size(odd)]; }
    default: return PED_NAMES[r.next()%32];
    } }
inline std::string Sep(Rng& r){ static const char* s[]{" ",",","\t"," , ","  ",",,","\t "}; return s[r.next()%std::size(s)]; }
inline void MakeLines(Rng& r){
    lines.clear(); lineIdx=0; const int n=int(r.next()%14);
    for(int k=0;k<n;++k){ std::string l; const uint32 kind=r.next()%10;
        if(r.next()%6==0)l+=r.next()%2 ? " " : "\t";
        if(kind==0)l+="# comentario "+RandomName(r);
        else if(kind==1)l+="";
        else if(kind<=4)l+=RandomName(r);
        else { static const char* rel[]{"Hate","Dislike","Like","Respect","Hatex","Lik","Respects","hate"}; l+=rel[r.next()%(r.next()%5==0 ? 8 : 4)];
            const int t=int(r.next()%7); for(int j=0;j<t;++j){ l+=Sep(r); l+=RandomName(r); } if(r.next()%4==0)l+=Sep(r); }
        lines.push_back(l); } }

inline json RunPedType(const std::string& nm,uintptr_t a,Rng& r,json& out){
    auto& g=CPedType::ms_apPedTypes; auto* const savedG=g; auto* const savedOld=CPedType::ms_apPedTypesOld;
    for(size_t k=0;k<ptBuf.size();k+=4)At(ptBuf.data(),k)=r.next()%3 ? r.next() : 0u;
    auto* const tbl=reinterpret_cast<CAcquaintance*>(ptBuf.data()+4);
    g=tbl; json ret=nullptr; nrec=0;
    // ficheros y búfer de partida: solo durante la llamada
    Patches io;
    io.Add(0x538900,FakeOpenFile); io.Add(Addr(&CFileMgr::OpenFile),FakeOpenFile);
    io.Add(0x5389B0,FakeReadLine); io.Add(Addr(&CFileMgr::ReadLine),FakeReadLine);
    io.Add(0x5389D0,FakeCloseFile); io.Add(Addr(&CFileMgr::CloseFile),FakeCloseFile);
    io.Add(0x5D1270,FakeSaveWB); io.Add(Addr(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWB);
    io.Add(0x5D1300,FakeLoadWB); io.Add(Addr(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FakeLoadWB);
    auto type=[&]{ return int32(r.next()%8==0 ? int32(r.next()%(PT_N-1)) : int32(r.next()%32)); };
    auto id=[&]{ return int32(r.next()%5); };
    auto mask=[&]{ switch(r.next()%4){ case 0: return 1u<<(r.next()%32); case 1: return 0u; default: return r.next(); } };
    if(nm=="FindPedType"){ std::string s=RandomName(r); if(r.next()%16==0)s=""; ret=int32(reinterpret_cast<int32(__cdecl*)(const char*)>(a)(s.c_str())); out["name"]=s; }
    else if(nm=="GetPedFlag"){ const int32 t=r.next()%4==0 ? int32(r.next()) : int32(r.next()%80)-20; ret=reinterpret_cast<uint32(__cdecl*)(int32)>(a)(t); }
    else if(nm=="GetPedTypeAcquaintances-CAcquaintance"){ ret=PtOff(reinterpret_cast<void*(__cdecl*)(int32)>(a)(type())); }
    else if(nm=="GetPedTypeAcquaintances-uint32"){ const int32 i=id(), t=type(); ret=reinterpret_cast<uint32(__cdecl*)(int32,int32)>(a)(i,t); }
    else if(nm=="SetPedTypeAsAcquaintance"||nm=="ClearPedTypeAsAcquaintance"){ const int32 i=id(), t=type(); const uint32 m=mask(); reinterpret_cast<void(__cdecl*)(int32,int32,uint32)>(a)(i,t,m); }
    else if(nm=="ClearPedTypeAcquaintances"){ const int32 i=id(), t=type(); reinterpret_cast<void(__cdecl*)(int32,int32)>(a)(i,t); }
    else if(nm=="PoliceDontCareAboutCrimesAgainstPedType"){ const int32 t=int32(r.next()%46)-5; ret=reinterpret_cast<uint8(__cdecl*)(int32)>(a)(t)!=0; }
    else if(nm=="Save"){ ret=reinterpret_cast<uint8(__cdecl*)()>(a)(); }
    else if(nm=="Load"){ for(auto& v:loadVals)v=r.next(); loadIdx=0; ret=reinterpret_cast<uint8(__cdecl*)()>(a)(); }
    else if(nm=="LoadPedData"){ MakeLines(r); reinterpret_cast<void(__cdecl*)()>(a)(); out["lines"]=lines; }
    else if(nm=="Initialise"){ MakeLines(r); for(auto& b:ptArena)b=uint8(r.next()); ptNewRet=ptArena.data()+(r.next()%4)*4;
        { ReversingProbe::EntryPatch n2(0x821195,PtNew),d2(0x8213AE,PtDelete); reinterpret_cast<void(__cdecl*)()>(a)(); }
        out["arena"]=ReversingProbe::Bytes(ptArena.data(),ptArena.size()); out["lines"]=lines; }
    else if(nm=="Shutdown"){ const uint32 mode=r.next()%4; At(ptBuf.data(),0)=mode==0 ? 32u : r.next()%40;
        g=mode==3 ? nullptr : tbl;
        { ReversingProbe::EntryPatch d2(0x8213AE,PtDelete); reinterpret_cast<void(__cdecl*)()>(a)(); } }
    out["g"]=PtOff(g); out["recs"]=Recs(); out["table"]=ReversingProbe::Bytes(ptBuf.data(),ptBuf.size());
    g=savedG; CPedType::ms_apPedTypesOld=savedOld;
    return ret;
}

// ======================= CTimer =======================
constexpr uintptr_t TM_BEGIN=0xB7CB28; constexpr size_t TM_SIZE=0x64;
inline volatile int64 qpcValue{}, qpfValue{}; inline volatile BOOL qpfRet{}; inline volatile uint32 rsValue{};
inline BOOL WINAPI FakeQPC(LARGE_INTEGER* p){ p->QuadPart=qpcValue; if(Main())trace.push_back({"qpc"}); return TRUE; }
inline BOOL WINAPI FakeQPF(LARGE_INTEGER* p){ p->QuadPart=qpfValue; if(Main())trace.push_back({"qpf"}); return qpfRet; }
inline uint32 __cdecl FakeRsTimer(){ if(Main())trace.push_back({"RsTimer"}); return rsValue; }
inline json TimerFn(uintptr_t v){
    if(!v)return nullptr;
    if(v==0x5617A0||v==Addr(&GetOSWPerformanceTime)||FollowIn(v)==FollowIn(Addr(&GetOSWPerformanceTime)))return "perf";
    if(v==0x5617C0||v==Addr(&GetMillisecondTime)||FollowIn(v)==FollowIn(Addr(&GetMillisecondTime)))return "ms";
    return v; }
inline json TimerState(){ json r=json::array(); auto* p=reinterpret_cast<uint8*>(TM_BEGIN); r.push_back(TimerFn(At<uintptr_t>(p,0)));
    for(size_t k=4;k<TM_SIZE;k+=4)r.push_back(At(p,k)); r.push_back(*reinterpret_cast<uint8*>(0xC7C714)); return r; }

inline json RunTimer(const std::string& nm,uintptr_t a,Rng& r,json& out){
    auto* p=reinterpret_cast<uint8*>(TM_BEGIN);
    // estado del reloj del caso
    static const uintptr_t fnsExe[]{0x5617A0,0x5617C0};
    const uintptr_t fnChoice[]{0x5617A0,0x5617C0,Addr(&GetOSWPerformanceTime),Addr(&GetMillisecondTime)};
    At<uintptr_t>(p,0)=r.next()%3 ? fnsExe[r.next()%2] : fnChoice[r.next()%4];
    if(nm=="Update"&&r.next()%16==0)At<uintptr_t>(p,0)=0;
    static const uint32 divs[]{1,10,1000,3579,10000,14318,2600000,16777217,0x80000001u,0xFFFFFFFFu,0,3};
    At(p,0x2C-0x28)=r.next()%3 ? divs[r.next()%std::size(divs)] : r.next()%(r.next()%2 ? 100000u : 0xFFFFFFFFu);
    At(p,0x30-0x28)=r.next(); At(p,0x34-0x28)=r.next()%4 ? 0u : r.next();          // render pause time
    At(p,0x38-0x28)=r.next(); At(p,0x3C-0x28)=r.next()%4 ? 0u : r.next()%8;        // render start time
    p[0x40-0x28]=uint8(r.next()%3!=0);                                               // m_sbEnableTimeDebug
    { static const uint32 pc[]{0,1,2,3,0xFFFFFFFFu}; At(p,0x44-0x28)=pc[r.next()%5]; }
    p[0x48-0x28]=uint8(r.next()%4==0); p[0x49-0x28]=uint8(r.next()%4==0);            // code / user pause
    At(p,0x4C-0x28)=r.next();
    At<float>(p,0x50-0x28)=r.uni(0.f,100.f); At<float>(p,0x54-0x28)=r.uni(0.f,3.f);
    At<float>(p,0x58-0x28)=r.pick({0.f,0.01f,1.f,3.f,r.uni(0.f,4.f)}); At<float>(p,0x5C-0x28)=r.pick({1.f,0.5f,r.uni(0.f,3.f)});
    At<float>(p,0x60-0x28)=r.pick({-1.f,1.f,0.5f});
    At<float>(p,0x64-0x28)=r.pick({1.f,0.5f,0.f,std::nextafter(1.f,0.f),std::nextafter(1.f,2.f),2.f,-1.f,QNAN,r.uni(0.f,2.f)});
    auto ms=[&]{ switch(r.next()%5){ case 0: return r.next(); case 1: return r.next()%100000u; case 2: return 16777216u+r.next()%1000u; case 3: return 0x80000000u+r.next()%100000u; default: return r.next()%0x40000000u; } };
    for(size_t off:{size_t(0x68),size_t(0x6C),size_t(0x70),size_t(0x74),size_t(0x78),size_t(0x7C),size_t(0x80),size_t(0x84)})At(p,off-0x28)=ms();
    if(r.next()%2)At(p,0x68-0x28)=At(p,0x80-0x28)-r.next()%60;                        // fotograma anterior cercano
    p[0x88-0x28]=uint8(r.next()%4==0); p[0x89-0x28]=uint8(r.next()%4==0);
    *reinterpret_cast<uint8*>(0xC7C714)=uint8(r.next()%6==0);
    // fuentes de tiempo
    { const uint64 start=uint64(At(p,0x38-0x28))|(uint64(At(p,0x3C-0x28))<<32);
      switch(r.next()%5){ case 0: qpcValue=int64(r.next())|(int64(r.next())<<32); break; case 1: qpcValue=int64(start); break;
        case 2: qpcValue=int64(start-r.next()%1000); break; default: qpcValue=int64(start+(r.next()%3 ? r.next()%2000000u : uint64(r.next())<<(r.next()%20))); } }
    qpfValue=r.next()%3 ? int64(r.next()%50000000u) : int64(r.next())|(int64(r.next()%4)<<32); qpfRet=r.next()%5!=0; rsValue=r.next();
    json ret=nullptr;
    {
        Patches t; t.Add(Addr(&QueryPerformanceCounter),FakeQPC); t.Add(Addr(&QueryPerformanceFrequency),FakeQPF);
        t.Add(0x619410,FakeRsTimer); t.Add(Addr(&RsTimer),FakeRsTimer);
        if(nm=="GetCyclesPerMillisecond"||nm=="GetCyclesPerFrame"||nm=="GetCurrentTimeInCycles")ret=reinterpret_cast<uint32(__cdecl*)()>(a)();
        else if(nm=="GetIsSlowMotionActive")ret=reinterpret_cast<uint8(__cdecl*)()>(a)()!=0;
        else if(nm=="UpdateVariables"){ const float te=r.next()%10==0 ? r.pick({0.f,-0.f,-1.f,-500.f,QNAN,1e30f,1e-30f}) : r.uni(0.f,r.pick({100.f,40000.f,1e7f,1e10f}));
            reinterpret_cast<void(__cdecl*)(float)>(a)(te); out["arg"]=F(te); }
        else reinterpret_cast<void(__cdecl*)()>(a)();
    }
    out["state"]=TimerState();
    return ret;
}

// ======================= CTrailer =======================
constexpr size_t VEH=sizeof(CTrailer), MATB=0x60, NSLOT=71;
alignas(16) inline std::array<uint8,VEH> trl{}, veh0{}, veh1{}, ent{}, tmpl{};
alignas(16) inline std::array<uint8,MATB> trlMat{}, vehMat0{}, vehMat1{}, entMat{};
alignas(16) inline std::array<uint8,sizeof(CVehicleModelInfo)> miT{}, miE{};
alignas(16) inline std::array<uint8,0x400> vstruct{};
alignas(16) inline std::array<uint8,0x30> colT{}, colE{}, cdT{}, cdE{};
alignas(16) inline std::array<uint8,0xC0> lines6{}, lines6b{};
alignas(16) inline std::array<uint8,0x100> handling{};
alignas(16) inline std::array<std::array<uint8,0x100>,4> frames{};
alignas(16) inline std::array<uint8,0x2C*32> cps{};
inline std::array<void*,NSLOT> trlVt{}, vehVt{};
constexpr uintptr_t EXE_TRAILER_VT=0x871C28;
inline const int16 TRAILER_MODELS[]{435,450,584,591,606,607,608,610};
inline const int16 OTHER_MODELS[]{403,514,515,525,1337};
inline std::vector<std::pair<void**,void*>> savedMi;

struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v==EXE_TRAILER_VT)return "exe_trailer_vtbl";
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    return v; }
inline json Name(const void* p){ return Name(uintptr_t(p)); }
inline json Vec(const void* p){ const auto* f=static_cast<const float*>(p); return json::array({F(f[0]),F(f[1]),F(f[2])}); }
inline json Words(const void* p,size_t from,size_t to){ json r=json::array(); for(size_t k=from;k+4<=to;k+=4)r.push_back(Name(*reinterpret_cast<const uint32*>(static_cast<const uint8*>(p)+k))); return r; }
inline void T(json j){ if(Main())trace.push_back(std::move(j)); }

// trampas de vtable
template<int N> void* __fastcall VTrap(void* v,void*) { T({"unexpected_slot",N,Name(v)}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,NSLOT>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
// huecos simulados
inline CVector hitchPos{}; inline bool hitchRet{}; inline std::array<CVector,2> barPos{}; inline std::array<bool,2> barRet{};
inline bool __fastcall HitchV(void* self,void*,CVector* out,uint32 check,void* veh){ T({"GetTowHitchPos",Name(self),Name(out),check&0xFF,Name(veh)}); *out=hitchPos; return hitchRet; }
inline bool __fastcall BarV(void* self,void*,CVector* out,uint32 check,void* veh){ const size_t k=self==veh1.data(); T({"GetTowBarPos",Name(self),Name(out),check&0xFF,Name(veh),Vec(out)}); *out=barPos[k]; return barRet[k]; }
inline bool __fastcall SetTowLinkV(void* self,void*,void* veh,uint32 b){ T({"SetTowLink",Name(self),Name(veh),b&0xFF}); return true; }

// dependencias simuladas
inline bool baseAiRet{}; inline int32 baseColRet{}; inline bool playbackRet{};
inline std::array<CVector,4> wheelPosn{};
inline void __fastcall BaseVoid0(void* self,void*){ T({"CAutomobile::ProcessControl",Name(self)}); }
inline void __fastcall BaseVoid1(void* self,void*){ T({"CAutomobile::ResetSuspension",Name(self)}); }
inline void __fastcall BaseVoid2(void* self,void*){ T({"CAutomobile::ProcessSuspension",Name(self)}); }
inline void __fastcall BaseVoid3(void* self,void*){ T({"CAutomobile::SetupSuspensionLines",Name(self)}); }
inline void __fastcall BaseVoid4(void* self,void*){ T({"CAutomobile::PreRender",Name(self)}); }
inline bool __fastcall BaseAI(void* self,void*,uint32* flags){ T({"CAutomobile::ProcessAI",Name(self),Name(flags),*flags}); *flags^=0x5A5Au; return baseAiRet; }
inline int32 __fastcall BaseCol(void* self,void*,void* e,void* cp){ T({"CAutomobile::ProcessEntityCollision",Name(self),Name(e),Name(cp)}); return baseColRet; }
inline void* __fastcall FakeAutoCtor(void* self,void*,int32 model,int32 createdBy,uint32 b){ T({"CAutomobile::CAutomobile",Name(self),model,createdBy,b&0xFF}); std::memcpy(self,tmpl.data(),VEH); At<int16>(self,0x22)=int16(model); return self; }
inline bool __fastcall FakeSpring(void* self,void*,float force,CVector* dir,CVector* point,float ratio,float bias,CVector* normal,float* outDamp){
    T({"ApplySpringCollisionAlt",Name(self),F(force),Vec(dir),Vec(point),F(ratio),F(bias),Vec(normal),Name(outDamp)}); *outDamp=ratio*7.f+force-point->x; return true; }
inline CVector* __fastcall FakeGetSpeed(void* self,void*,CVector* out,float x,float y,float z){ T({"GetSpeed",Name(self),F(x),F(y),F(z)}); *out=CVector{x*2.f+1.f,y-3.f,z*0.5f}; return out; }
inline bool __fastcall FakeDamp(void* self,void*,float damping,float force,CVector* dir,CVector* point,CVector* speed){ T({"ApplySpringDampening",Name(self),F(damping),F(force),Vec(dir),Vec(point),Vec(speed)}); return true; }
inline void __fastcall FakeWheelPair(void* self,void*,int32 l,int32 rr,float steer,CVector* speeds,CVector* points,float traction,float accel,float brake,uint32 front){
    T({"ProcessCarWheelPair",Name(self),l,rr,F(steer),Vec(&speeds[0]),Vec(&speeds[1]),Vec(&points[0]),Vec(&points[1]),F(traction),F(accel),F(brake),front&0xFF,
       At(self,0x5A4)}); }
inline void __fastcall FakeSetCompRot(void* self,void*,void* frame,int32 axis,float angle,uint32 reset){ T({"SetComponentRotation",Name(self),Name(frame),axis,F(angle),reset&0xFF}); }
inline void __fastcall FakeAudio(void* self,void*,int32 ev,float p){ T({"AddAudioEvent",Name(self),ev,F(p)}); }
inline void __fastcall FakeRegRef(void* self,void*,void* field){ T({"RegisterReference",Name(self),Name(field)}); }
inline void __fastcall FakeCleanRef(void* self,void*,void* field){ T({"CleanUpOldReference",Name(self),Name(field)}); }
inline void __fastcall FakeColRecord(void* self,void*,void* e){ T({"AddCollisionRecord",Name(self),Name(e)}); }
inline void __fastcall FakeRemoveMoving(void* self,void*){ T({"RemoveFromMovingList",Name(self)}); }
inline void __fastcall FakeAddMoving(void* self,void*){ T({"AddToMovingList",Name(self)}); }
inline void __fastcall FakeTractorLink(void* self,void*,uint32 a0,uint32 a1){ T({"UpdateTractorLink",Name(self),a0&0xFF,a1&0xFF}); }
inline void __fastcall FakeSetPosn(void* self,void*,const CVector* pos){ T({"SetPosn",Name(self),Vec(pos)}); }
inline void __fastcall FakePlaceOnRoad(void* self,void*){ T({"PlaceOnRoadProperly",Name(self)}); }
inline void __fastcall FakeWheelPosn(void* self,void*,int32 wheel,CVector* out,uint32 local){ T({"GetWheelPosn",Name(self),wheel,Name(out),local&0xFF}); *out=wheelPosn[uint32(wheel)&3]; }
inline void* __cdecl FakeMalloc(uint32 size){ T({"Malloc",size}); return lines6b.data(); }
// parches de Malloc: dirección del EXE + entradas de la DLL (naked desde d26e3a5d; CTrailer::SetupSuspensionLines llama CMemoryMgr::Malloc directo)
struct MallocGuard { ReversingProbe::EntryPatch e{0x72F420,FakeMalloc}, d2{Addr(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc}, d1{Addr(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc)),FakeMalloc}; };
inline bool __cdecl FakePlayback(void* v){ T({"IsPlaybackGoingOnForCar",Name(v)}); return playbackRet; }
inline std::vector<void*> foundObjs;
inline void __cdecl FakeFindObjects(const CVector* pt,float radius,uint32 b2d,int16* count,int16 maxCount,CEntity** outEnt,uint32 b,uint32 v,uint32 pd,uint32 o,uint32 d){
    T({"FindObjectsInRange",Vec(pt),F(radius),b2d&0xFF,Name(count),maxCount,Name(outEnt),b&0xFF,v&0xFF,pd&0xFF,o&0xFF,d&0xFF});
    const int16 n=int16(std::min<size_t>(foundObjs.size(),size_t(std::max<int16>(maxCount,0)))); for(int16 k=0;k<n;++k)outEnt[k]=static_cast<CEntity*>(foundObjs[size_t(k)]); *count=n; }
inline void __cdecl FakeScan(void* self){ T({"ScanForTowLink",Name(self)}); }
inline void __fastcall FakeScanThis(void* self,void*){ T({"ScanForTowLink",Name(self)}); }
// ProcessColModels
inline int32 colRet{}; inline std::array<std::array<uint8,0x2C>,6> lineCPs{}; inline std::array<float,6> lineDists{}; inline std::array<bool,6> lineHit{};
inline int32 __cdecl FakeProcessColModels(const CMatrix* ma,void* ca,const CMatrix* mb,void* cb,void* sphereCPs,void* lineCp,float* dists,uint32 b){
    json d=json::array(); for(int k=0;k<6;++k)d.push_back(F(dists[k]));
    T({"ProcessColModels",Name(ma),Name(ca),Name(mb),Name(cb),Name(sphereCPs),Name(lineCp),Name(dists),b&0xFF,d,
       At<uint16>(cdT.data(),4),At<uint8>(cdT.data(),6),At<uint16>(cdE.data(),4)});
    for(int k=0;k<6;++k){ std::memcpy(static_cast<uint8*>(lineCp)+0x2C*k,lineCPs[size_t(k)].data(),0x2C); if(lineHit[size_t(k)])dists[k]=lineDists[size_t(k)]; }
    for(int k=0;k<std::clamp(colRet,0,32);++k)std::memcpy(static_cast<uint8*>(sphereCPs)+0x2C*k,cps.data()+0x2C*k,0x2C);
    return colRet; }

inline void RandMatrix(Rng& r,std::array<uint8,MATB>& buf,float spread){
    std::memset(buf.data(),0,buf.size()); CMatrix m{}; m.SetRotate(r.uni(-0.6f,0.6f),r.uni(-0.6f,0.6f),r.uni(-3.1f,3.1f));
    m.GetPosition()=CVector{r.uni(-spread,spread),r.uni(-spread,spread),r.uni(-5.f,5.f)}; std::memcpy(buf.data(),&m,0x40); }
inline void RandColPoint(Rng& r,uint8* cp){
    for(size_t k=0;k<0x20;k+=4)At<float>(cp,k)=r.uni(-10.f,10.f);
    At<float>(cp,0x18)=r.pick({1.f,0.35f,std::nextafter(0.35f,1.f),0.2f,r.uni(-1.f,1.f)});
    for(size_t k=0x20;k<0x28;++k)cp[k]=uint8(r.next()); At<float>(cp,0x28)=r.uni(0.f,1.f); }
inline void MakeVehicle(Rng& r,std::array<uint8,VEH>& b,std::array<uint8,MATB>& m,bool needMatrix){
    std::memset(b.data(),0xCD,b.size()); At<void*>(b.data(),0)=vehVt.data();
    At<float>(b.data(),4)=r.uni(-50.f,50.f); At<float>(b.data(),8)=r.uni(-50.f,50.f); At<float>(b.data(),0xC)=r.uni(-5.f,5.f); At<float>(b.data(),0x10)=r.uni(-3.2f,3.2f);
    RandMatrix(r,m,50.f); At<void*>(b.data(),0x14)=needMatrix||r.next()%6 ? static_cast<void*>(m.data()) : nullptr;
    At<void*>(b.data(),0x18)=nullptr; At<uint32>(b.data(),0x1C)=r.next(); At<int16>(b.data(),0x22)=OTHER_MODELS[r.next()%4]; At<void*>(b.data(),0x24)=nullptr;
    b[0x36]=uint8(2|((r.next()%13)<<3)); At<uint32>(b.data(),0x40)=r.next();
    for(size_t k=0;k<6;++k)At<float>(b.data(),0x44+4*k)=r.uni(-1.f,1.f);
    b[0x48B]=0xFF; At<float>(b.data(),0x4A0)=r.uni(0.f,1.f); At<void*>(b.data(),0x4C4)=nullptr; At<void*>(b.data(),0x4C8)=nullptr;
    for(size_t k=0;k<4;++k)b[0x574+k]=uint8(r.next()); }

inline void SetupTrailerWorld(const std::string& nm,Rng& r){
    // model infos
    std::memset(miT.data(),0,miT.size()); std::memset(miE.data(),0,miE.size()); std::memset(vstruct.data(),0,vstruct.size());
    At<void*>(miT.data(),0x14)=colT.data(); At<void*>(miE.data(),0x14)=colE.data(); At<void*>(miT.data(),0x5C)=vstruct.data(); At<void*>(miE.data(),0x5C)=vstruct.data();
    At<float>(miT.data(),0x40)=r.pick({0.8f,1.f,1.2f,r.uni(0.3f,2.f)}); At<float>(miE.data(),0x40)=1.f;
    for(size_t k=0;k<vstruct.size();k+=4)At<float>(vstruct.data(),k)=r.uni(-3.f,3.f);
    switch(r.next()%5){ case 0: case 1: At<float>(vstruct.data(),0x6C)=0.f; At<float>(vstruct.data(),0x70)=0.f; At<float>(vstruct.data(),0x74)=0.f; break;
        case 2: At<float>(vstruct.data(),0x6C+4*(r.next()%3))=0.f; if(r.next()%2)At<float>(vstruct.data(),0x6C)=-0.f; break; default: break; }
    auto colModel=[&](std::array<uint8,0x30>& c,std::array<uint8,0x30>& d){
        std::memset(c.data(),0,c.size()); std::memset(d.data(),0,d.size());
        At<float>(c.data(),0)=r.uni(-2.f,-0.5f); At<float>(c.data(),4)=r.uni(-8.f,-1.f); At<float>(c.data(),8)=r.uni(-2.f,0.f);
        At<float>(c.data(),0xC)=r.uni(0.5f,2.f); At<float>(c.data(),0x10)=r.uni(1.f,8.f); At<float>(c.data(),0x14)=r.uni(0.5f,4.f);
        if(r.next()%10==0)At<float>(c.data(),0)=-At<float>(c.data(),0xC);
        for(size_t k=0x18;k<0x24;k+=4)At<float>(c.data(),k)=r.uni(-1.f,1.f); At<float>(c.data(),0x24)=r.pick({0.f,5.f,r.uni(0.f,12.f)});
        At<void*>(c.data(),0x2C)=d.data();
        At<uint16>(d.data(),4)=uint16(r.next()%6==0 ? 0x8000u+r.next()%0x8000u : r.next()%300u);
        static const uint8 nl[]{0,4,5,6,6,6,8,0x85}; d[6]=nl[r.next()%std::size(nl)];
        At<void*>(d.data(),0x10)=lines6.data(); };
    colModel(colT,cdT); colModel(colE,cdE);
    for(size_t k=0;k<lines6.size();k+=4)At<float>(lines6.data(),k)=r.uni(-4.f,4.f);
    for(size_t k=0;k<lines6b.size();k+=4)At<uint32>(lines6b.data(),k)=0xCDCDCDCDu;
    if(nm=="PreRender"){ At<float>(lines6.data(),0x84)=r.pick({r.uni(1.f,6.f),r.uni(-6.f,-1.f),0.f}); }
    if((nm=="SetupSuspensionLines"||nm=="Constructor")&&r.next()%3==0)At<void*>(cdT.data(),0x10)=nullptr;
    // handling
    for(size_t k=0;k<handling.size();k+=4)At<float>(handling.data(),k)=r.uni(0.f,2.f);
    At<float>(handling.data(),0x28)=r.uni(0.5f,2.f); At<float>(handling.data(),0xAC)=r.uni(0.5f,3.f);
    At<float>(handling.data(),0xB8)=r.uni(0.f,0.4f); At<float>(handling.data(),0xBC)=r.uni(-0.4f,0.f);
    // marcos RW
    for(auto& f:frames){ std::memset(f.data(),0,f.size()); for(size_t k=0x10;k<0x50;k+=4)At<float>(f.data(),k)=r.uni(-1.f,1.f); At<uint32>(f.data(),0x1C)=0; At<float>(f.data(),0x44)=r.uni(-3.f,3.f); }
    { const uint32 chain=r.next()%5; void* const f0=frames[0].data(); void* const f1=frames[1].data(); void* const f2=frames[2].data(); void* const f3=frames[3].data();
      At<void*>(f0,4)=chain==0 ? nullptr : f2; At<void*>(f2,4)=chain>=2 ? f3 : nullptr; At<void*>(f3,4)=chain==4 ? f0 : (chain==3 ? f1 : nullptr); At<void*>(f1,4)=nullptr; }
    // vehículos y entidad
    MakeVehicle(r,veh0,vehMat0,nm=="ProcessEntityCollision"); MakeVehicle(r,veh1,vehMat1,nm=="ProcessEntityCollision");
    if(r.next()%4==0)At<int16>(veh0.data(),0x22)=525;
    if(r.next()%2)At<void*>(veh0.data(),0x4C8)=r.next()%3 ? static_cast<void*>(trl.data()) : static_cast<void*>(veh1.data());
    std::memset(ent.data(),0xCD,ent.size()); At<void*>(ent.data(),0)=vehVt.data(); RandMatrix(r,entMat,20.f); At<void*>(ent.data(),0x14)=entMat.data();
    At<void*>(ent.data(),0x18)=nullptr; At<uint32>(ent.data(),0x1C)=r.next(); At<void*>(ent.data(),0x24)=nullptr; At<uint32>(ent.data(),0x40)=r.next();
    { static const uint8 types[]{1,1,2,3,4,4}; const uint8 t=types[r.next()%6]; ent[0x36]=uint8(t|((r.next()%13)<<3)); At<int16>(ent.data(),0x22)=t==2 ? int16(403) : int16(1337); ent[0x48B]=0xFF; }
    // remolque
    std::memset(trl.data(),0xCD,trl.size()); void* const t=trl.data(); At<void*>(t,0)=trlVt.data();
    At<float>(t,4)=r.uni(-50.f,50.f); At<float>(t,8)=r.uni(-50.f,50.f); At<float>(t,0xC)=r.uni(-5.f,5.f); At<float>(t,0x10)=r.uni(-3.2f,3.2f);
    RandMatrix(r,trlMat,50.f);
    const bool needMatrix=nm=="GetTowBarPos"||nm=="GetTowHitchPos"||nm=="ProcessSuspension"||nm=="PreRender"||nm=="ProcessEntityCollision"||nm=="ScanForTowLink";
    At<void*>(t,0x14)=needMatrix||r.next()%5 ? static_cast<void*>(trlMat.data()) : nullptr;
    At<void*>(t,0x18)=nullptr; At<uint32>(t,0x1C)=r.next(); At<int16>(t,0x22)=TRAILER_MODELS[r.next()%8]; At<void*>(t,0x24)=nullptr;
    { static const uint8 st[]{3,4,4,10,10,11,2,8,0,12}; trl[0x36]=uint8(2|(st[r.next()%std::size(st)]<<3)); }
    At<uint32>(t,0x40)=r.next();
    const bool still=r.next()%3==0; for(size_t k=0;k<6;++k)At<float>(t,0x44+4*k)=still&&r.next()%4 ? 0.f : r.uni(-0.3f,0.3f);
    trl[0xB8]=uint8(r.next()%2); trl[0xBA]=uint8(r.next());
    At<void*>(t,0x384)=handling.data(); trl[0x42B]=uint8(r.next()); trl[0x48B]=0xFF; At<void*>(t,0x48C)=nullptr;
    At<float>(t,0x494)=r.uni(-1.f,1.f); At<float>(t,0x4A0)=r.uni(0.f,1.f);
    At<void*>(t,0x4C4)=r.next()%3 ? static_cast<void*>(veh0.data()) : nullptr; At<void*>(t,0x4C8)=r.next()%4==0 ? static_cast<void*>(veh1.data()) : nullptr;
    for(size_t k=0;k<4;++k)trl[0x574+k]=uint8(r.next());
    At<uint32>(t,0x594)=11; At<float>(t,0x5A0)=r.uni(0.f,1.f); trl[0x5A4]=uint8(r.next()%250); for(size_t k=0;k<4;++k)trl[0x5A5+k]=uint8(r.next()%3);
    for(size_t k=0;k<23;++k)At<void*>(t,0x648+4*k)=nullptr;
    At<void*>(t,0x698)=r.next()%5 ? static_cast<void*>(frames[0].data()) : nullptr; At<void*>(t,0x69C)=frames[1].data();
    for(size_t k=0;k<4;++k)RandColPoint(r,trl.data()+0x724+0x2C*k);
    for(size_t k=0;k<4;++k)At<float>(t,0x7D4+4*k)=r.pick({1.f,0.5f,0.f,r.uni(0.f,1.2f)});
    for(size_t k=0x7E4;k<0x8E8;k+=4)At<float>(t,k)=r.uni(-2.f,2.f);
    At<float>(t,0x898)=r.uni(-1.f,1.f); At<float>(t,0x89C)=r.uni(-1.f,1.f);
    for(size_t k=0;k<4;++k)At<void*>(t,0x8E8+4*k)=r.next()%3==0 ? static_cast<void*>(veh1.data()) : nullptr;
    for(size_t k=0x8F8;k<0x928;k+=4)At<float>(t,k)=r.uni(-2.f,2.f);
    At<float>(t,0x958)=r.pick({std::bit_cast<float>(0x40C90FDBu),std::nextafter(std::bit_cast<float>(0x40C90FDBu),10.f),r.uni(-7.f,14.f),r.uni(0.f,7.f)});
    for(size_t k=0;k<2;++k)RandColPoint(r,trl.data()+0x988+0x2C*k);
    auto ratio=[&]{ return r.pick({-1000.f,0.f,-0.f,0.1f,std::nextafter(0.1f,1.f),0.5f,std::nextafter(0.5f,1.f),0.9f,1.f,std::nextafter(1.f,0.f),1.2f,-1.f,r.uni(0.f,1.f),r.uni(0.f,1.f)}); };
    At<float>(t,0x9E0)=ratio(); At<float>(t,0x9E4)=ratio(); At<float>(t,0x9E8)=r.uni(-2.f,2.f);
    At<float>(t,0x9EC)=r.next()%5==0 ? -1000.f : ratio(); At<float>(t,0x9F0)=ratio();
    if(nm=="ProcessSuspension"||nm=="ProcessEntityCollision"){ if(r.next()%4)At<float>(t,0x9EC)=r.uni(0.f,1.f); }
    // simulaciones
    hitchPos=CVector{r.uni(-50.f,50.f),r.uni(-50.f,50.f),r.uni(-3.f,3.f)}; hitchRet=r.next()%5!=0;
    for(size_t k=0;k<2;++k){ const auto& h=hitchPos; switch(r.next()%4){ case 0: barPos[k]=CVector{h.x+r.uni(-0.3f,0.3f),h.y+r.uni(-0.25f,0.25f),h.z+r.uni(-0.9f,0.9f)}; break;
        case 1: barPos[k]=CVector{h.x+r.pick({0.4f,0.39f,0.41f}),h.y,h.z+r.pick({1.f,0.99f,-1.f})}; break; default: barPos[k]=CVector{h.x+r.uni(-3.f,3.f),h.y+r.uni(-3.f,3.f),h.z+r.uni(-2.f,2.f)}; }
        barRet[k]=r.next()%5!=0; }
    baseAiRet=r.next()%2; baseColRet=int32(r.next()%5)-1; playbackRet=r.next()%6==0;
    for(auto& w:wheelPosn)w=CVector{r.uni(-1.5f,1.5f),r.uni(-4.f,4.f),r.uni(-0.5f,0.5f)};
    foundObjs.clear(); { const int n=int(r.next()%5); for(int k=0;k<n;++k){ const uint32 c=r.next()%4; foundObjs.push_back(c==0 ? static_cast<void*>(trl.data()) : c==1 ? static_cast<void*>(veh1.data()) : static_cast<void*>(veh0.data())); } }
    colRet=r.next()%4==0 ? 0 : int32(r.next()%6); for(auto& c:lineCPs)RandColPoint(r,c.data());
    for(size_t k=0;k<6;++k){ lineHit[k]=r.next()%3!=0; lineDists[k]=r.pick({0.f,0.5f,1.f,std::nextafter(1.f,0.f),1.5f,r.uni(0.f,1.f)}); }
    for(size_t k=0;k<cps.size();k+=4)At<float>(cps.data(),k)=r.uni(-5.f,5.f);
    CTimer::ms_fTimeStep=r.pick({1.f,0.5f,2.f,3.f,0.00001f,r.uni(0.f,3.f)});
    // plantilla del constructor de CAutomobile
    std::memcpy(tmpl.data(),trl.data(),VEH);
    // model infos falsos
    savedMi.clear();
    for(const int16 m:TRAILER_MODELS){ auto& slot=reinterpret_cast<void*&>(CModelInfo::ms_modelInfoPtrs[m]); savedMi.push_back({&slot,slot}); slot=miT.data(); }
    for(const int16 m:OTHER_MODELS){ auto& slot=reinterpret_cast<void*&>(CModelInfo::ms_modelInfoPtrs[m]); savedMi.push_back({&slot,slot}); slot=miE.data(); }
}
inline void RestoreModelInfos(){ for(auto it=savedMi.rbegin();it!=savedMi.rend();++it)*it->first=it->second; savedMi.clear(); }

inline uintptr_t dllAutoCtor{};
inline json RunTrailer(const std::string& nm,const auto& fn,bool candidate,Rng& r,json& out){
    SetupTrailerWorld(nm,r);
    void* const t=trl.data(); json ret=nullptr;
    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
    out["switches"]=fn.slot>=0 ? ((target<0x400000||target>=0x1000000)==candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
    if(nm=="BreakTowLink")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(target)(t)!=0;
    else if(nm=="GetTowBarPos"||nm=="GetTowHitchPos"){ CVector o{r.uni(-9.f,9.f),r.uni(-9.f,9.f),r.uni(-9.f,9.f)}; const uint32 check=r.next()%2;
        void* const v=r.next()%4==0 ? nullptr : r.next()%3 ? static_cast<void*>(veh0.data()) : t;
        ret=reinterpret_cast<uint8(__thiscall*)(void*,CVector*,uint32,void*)>(target)(t,&o,check,v)!=0; out["pos"]=Vec(&o); }
    else if(nm=="SetupSuspensionLines"){ MallocGuard m; reinterpret_cast<void(__thiscall*)(void*)>(target)(t); }
    else if(nm=="PreRender"||nm=="ProcessControl"||nm=="ProcessSuspension"||nm=="ResetSuspension")reinterpret_cast<void(__thiscall*)(void*)>(target)(t);
    else if(nm=="ProcessAI"){ uint32 flags=r.next();
        { Patches s; s.Add(0x6CF030,FakeScanThis); s.Add(FollowIn(Addr(&CTrailer::ScanForTowLink)),FakeScanThis); s.Add(Addr(&CTrailer::ScanForTowLink),FakeScanThis);
          ret=reinterpret_cast<uint8(__thiscall*)(void*,uint32*)>(target)(t,&flags)!=0; }
        out["flags"]=flags; }
    else if(nm=="ProcessEntityCollision"){ void* e=ent.data();
        switch(r.next()%5){ case 0: if(At<void*>(t,0x4C4))e=At<void*>(t,0x4C4); break; case 1: if(At<void*>(t,0x4C8))e=At<void*>(t,0x4C8); break; default: break; }
        ret=reinterpret_cast<int32(__thiscall*)(void*,void*,void*)>(target)(t,e,cps.data()); }
    else if(nm=="SetTowLink"){ void* const v=r.next()%8==0 ? nullptr : r.next()%4==0 ? static_cast<void*>(veh1.data()) : static_cast<void*>(veh0.data());
        ret=reinterpret_cast<uint8(__thiscall*)(void*,void*,uint32)>(target)(t,v,r.next()%2)!=0; }
    else if(nm=="ScanForTowLink")reinterpret_cast<void(__thiscall*)(void*)>(target)(t);
    else if(nm=="Constructor"){ std::memset(t,0xCD,VEH); const int32 model=TRAILER_MODELS[r.next()%8]; const int32 by=int32(r.next()%5);
        MallocGuard m; ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,int32)>(target)(t,model,by)); }
    RestoreModelInfos();
    // vptr: el EXE escribe 0x871C28 y la DLL su propia vtable de CTrailer
    const uintptr_t vp=At<uintptr_t>(t,0); json vpj=vp==EXE_TRAILER_VT||(nm=="Constructor"&&InDll(vp)) ? json("CTrailer") : Name(vp);
    out["vptr"]=vpj; out["trailer"]=Words(t,4,VEH);
    out["mats"]={Words(trlMat.data(),0,0x40),Words(vehMat0.data(),0,0x40),Words(vehMat1.data(),0,0x40),Words(entMat.data(),0,0x40)};
    out["others"]={Fnv(veh0.data(),VEH),Fnv(veh1.data(),VEH),Fnv(ent.data(),VEH)};
    out["col"]={Words(colT.data(),0,0x30),Words(cdT.data(),0,0x30),Words(colE.data(),0,0x30),Words(cdE.data(),0,0x30)};
    out["lines"]={Words(lines6.data(),0,0xC0),Words(lines6b.data(),0,0xC0)};
    { json fr=json::array(); for(auto& f:frames)fr.push_back(Words(f.data(),0,0x50)); out["frames"]=fr; }
    out["mi"]={Fnv(miT.data(),miT.size()),Fnv(vstruct.data(),vstruct.size()),Fnv(handling.data(),handling.size()),Fnv(cps.data(),cps.size())};
    return ret;
}

// ======================= tabla =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=39;
inline const std::array<Fn,NFN> fns{{
    {"CPedType","ClearPedTypeAcquaintances",0x608A20,0,-1,256},{"CPedType","ClearPedTypeAsAcquaintance",0x6089F0,0,-1,512},
    {"CPedType","FindPedType",0x608790,0,-1,512},{"CPedType","GetPedFlag",0x608830,0,-1,256},
    {"CPedType","GetPedTypeAcquaintances-CAcquaintance",0x6089B0,0,-1,128},{"CPedType","GetPedTypeAcquaintances-uint32",0x6089D0,0,-1,256},
    {"CPedType","Initialise",0x608E40,0,-1,128},{"CPedType","Load",0x5D3D10,0,-1,64},{"CPedType","LoadPedData",0x608B30,0,-1,512},
    {"CPedType","PoliceDontCareAboutCrimesAgainstPedType",0x608A40,0,-1,128},{"CPedType","Save",0x5D3CD0,0,-1,64},
    {"CPedType","SetPedTypeAsAcquaintance",0x608E20,0,-1,512},{"CPedType","Shutdown",0x608B00,0,-1,64},
    {"CTimer","EndUserPause",0x561B00,0,-1,64},{"CTimer","GetCurrentTimeInCycles",0x561A80,0,-1,256},{"CTimer","GetCyclesPerFrame",0x561A50,0,-1,512},
    {"CTimer","GetCyclesPerMillisecond",0x561A40,0,-1,64},{"CTimer","GetIsSlowMotionActive",0x561AD0,0,-1,256},{"CTimer","Initialise",0x5617E0,0,-1,128},
    {"CTimer","Resume",0x561A00,0,-1,256},{"CTimer","Shutdown",0x5618C0,0,-1,64},{"CTimer","StartUserPause",0x561AF0,0,-1,64},
    {"CTimer","Stop",0x561AA0,0,-1,64},{"CTimer","Suspend",0x5619D0,0,-1,256},{"CTimer","Update",0x561B10,0,-1,1024},
    {"CTimer","UpdateVariables",0x5618D0,0,-1,1024},
    {"CTrailer","BreakTowLink",0,0x871C28,62,256},{"CTrailer","Constructor",0x6D03A0,0,-1,256},{"CTrailer","GetTowBarPos",0,0x871C28,60,512},
    {"CTrailer","GetTowHitchPos",0,0x871C28,59,512},{"CTrailer","PreRender",0,0x871C28,17,512},{"CTrailer","ProcessAI",0,0x871C28,66,1024},
    {"CTrailer","ProcessControl",0,0x871C28,10,1024},{"CTrailer","ProcessEntityCollision",0,0x871C28,22,1024},{"CTrailer","ProcessSuspension",0,0x871C28,70,1024},
    {"CTrailer","ResetSuspension",0,0x871C28,67,256},{"CTrailer","ScanForTowLink",0x6CF030,0,-1,512},{"CTrailer","SetTowLink",0,0x871C28,61,1024},
    {"CTrailer","SetupSuspensionLines",0,0x871C28,48,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"sys13-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"sys13-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("sys13");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"sys13-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // cuerpos de la DLL de las funciones base de CAutomobile
    struct Base { const char* name; uintptr_t exe; void* stub; uintptr_t dll; };
    std::vector<Base> bases{
        {"ProcessControl",0x6B1880,reinterpret_cast<void*>(&BaseVoid0),0},{"ResetSuspension",0x6A2AE0,reinterpret_cast<void*>(&BaseVoid1),0},
        {"ProcessSuspension",0x6AFB10,reinterpret_cast<void*>(&BaseVoid2),0},{"SetupSuspensionLines",0x6A65D0,reinterpret_cast<void*>(&BaseVoid3),0},
        {"ProcessAI",0x6B4800,reinterpret_cast<void*>(&BaseAI),0},{"ProcessEntityCollision",0x6ACE70,reinterpret_cast<void*>(&BaseCol),0}};
    for(auto& b:bases)if(!(b.dll=DllBody("CAutomobile",b.name))){Write({{"event","error"},{"reason","CAutomobile body not found"},{"function",b.name}});return;}
    { const uintptr_t trailerCtor=FirstDllCall(DllBody("CTrailer","Constructor")); const uintptr_t a=trailerCtor ? FirstDllCall(trailerCtor) : 0;
      const uintptr_t b=FirstDllCall(DllBody("CAutomobile","Constructor"));
      // si no se localiza, solo se descarta el constructor de CTrailer (cuenta como fallo)
      if(!a||a!=b)Write({{"event","warning"},{"reason","CAutomobile constructor not found"},{"from_trailer",a},{"from_wrapper",b}});
      else dllAutoCtor=a; }
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    regions.clear();
    Snap(reinterpret_cast<void*>(TM_BEGIN),TM_SIZE); Snap(reinterpret_cast<void*>(0xC7C714),1);
    Snap(&CPedType::ms_apPedTypes,4); Snap(&CPedType::ms_apPedTypesOld,4);
    for(const int16 m:TRAILER_MODELS)Snap(&CModelInfo::ms_modelInfoPtrs[m],4);
    for(const int16 m:OTHER_MODELS)Snap(&CModelInfo::ms_modelInfoPtrs[m],4);

    FillTraps(trlVt,std::make_integer_sequence<int,int(NSLOT)>{}); FillTraps(vehVt,std::make_integer_sequence<int,int(NSLOT)>{});
    trlVt[59]=reinterpret_cast<void*>(&HitchV); trlVt[61]=reinterpret_cast<void*>(&SetTowLinkV); vehVt[60]=reinterpret_cast<void*>(&BarV);
    named={{trl.data(),VEH,"trailer"},{veh0.data(),VEH,"veh0"},{veh1.data(),VEH,"veh1"},{ent.data(),VEH,"ent"},{trlMat.data(),MATB,"trailer_mat"},
           {vehMat0.data(),MATB,"veh0_mat"},{vehMat1.data(),MATB,"veh1_mat"},{entMat.data(),MATB,"ent_mat"},{miT.data(),miT.size(),"miT"},{miE.data(),miE.size(),"miE"},
           {vstruct.data(),vstruct.size(),"vstruct"},{colT.data(),0x30,"colT"},{colE.data(),0x30,"colE"},{cdT.data(),0x30,"cdT"},{cdE.data(),0x30,"cdE"},
           {lines6.data(),0xC0,"lines"},{lines6b.data(),0xC0,"lines_malloc"},{handling.data(),handling.size(),"handling"},
           {frames[0].data(),0x100,"frame0"},{frames[1].data(),0x100,"frame1"},{frames[2].data(),0x100,"frame2"},{frames[3].data(),0x100,"frame3"},
           {cps.data(),cps.size(),"colpoints"},{trlVt.data(),sizeof(trlVt),"trailer_vt"},{vehVt.data(),sizeof(vehVt),"veh_vt"}};

    bool passed=true; int pair=0;
    {
        // dependencias de CTrailer (ninguna se usa en CPedType ni CTimer; las llama solo el hilo principal)
        Patches patches;
        for(auto& b:bases){ patches.Add(b.exe,b.stub); patches.Add(b.dll,b.stub); }
        patches.Add(0x6AAB50,BaseVoid4);
        patches.Add(0x6B0A90,FakeAutoCtor); patches.Add(dllAutoCtor,FakeAutoCtor);
        patches.Add(0x543D60,FakeSpring); patches.Add(Addr(&CPhysical::ApplySpringCollisionAlt),FakeSpring);
        patches.Add(0x542CE0,FakeGetSpeed); patches.Add(Addr(&CPhysical::GetSpeed),FakeGetSpeed);
        patches.Add(0x543E90,FakeDamp); patches.Add(Addr(&CPhysical::ApplySpringDampening),FakeDamp);
        patches.Add(0x6A4EC0,FakeWheelPair); patches.Add(Addr(&CAutomobile::ProcessCarWheelPair),FakeWheelPair);
        patches.Add(0x6DBA30,FakeSetCompRot); patches.Add(Addr(&CVehicle::SetComponentRotation),FakeSetCompRot);
        patches.Add(0x4F6420,FakeAudio); patches.Add(Addr(static_cast<void(CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent)),FakeAudio);
        patches.Add(0x571B70,FakeRegRef); patches.Add(Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegRef);
        patches.Add(0x571A00,FakeCleanRef); patches.Add(Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanRef);
        patches.Add(0x543490,FakeColRecord); patches.Add(Addr(&CPhysical::AddCollisionRecord),FakeColRecord);
        patches.Add(0x542860,FakeRemoveMoving); patches.Add(Addr(&CPhysical::RemoveFromMovingList),FakeRemoveMoving);
        patches.Add(0x542800,FakeAddMoving); patches.Add(Addr(&CPhysical::AddToMovingList),FakeAddMoving);
        patches.Add(0x6E0050,FakeTractorLink); patches.Add(Addr(&CVehicle::UpdateTractorLink),FakeTractorLink);
        patches.Add(0x4241C0,FakeSetPosn); patches.Add(Addr(static_cast<void(CPlaceable::*)(const CVector&)>(&CPlaceable::SetPosn)),FakeSetPosn);
        patches.Add(0x6AF420,FakePlaceOnRoad); patches.Add(Addr(&CAutomobile::PlaceOnRoadProperly),FakePlaceOnRoad);
        patches.Add(0x4C7D20,FakeWheelPosn); patches.Add(Addr(static_cast<void(CVehicleModelInfo::*)(int32,CVector&,bool) const>(&CVehicleModelInfo::GetWheelPosn)),FakeWheelPosn);
        patches.Add(0x4594C0,FakePlayback); patches.Add(Addr(&CVehicleRecording::IsPlaybackGoingOnForCar),FakePlayback);
        patches.Add(0x564A20,FakeFindObjects); patches.Add(Addr(&CWorld::FindObjectsInRange),FakeFindObjects);
        patches.Add(0x4185C0,FakeProcessColModels); patches.Add(Addr(&CCollision::ProcessColModels),FakeProcessColModels);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            if(nm=="Constructor"&&cls=="CTrailer"&&!dllAutoCtor){ passed=false; Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","CAutomobile constructor not found"}}); continue; }
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A13u}; if(!r.s)r.s=1;
                    trace=json::array(); nrec=0; RestoreRegions();
                    for(size_t k=0;k<hooks.size();++k)hooks[k]->State(candidate);
                    json out; json ret=nullptr;
                    if(cls=="CTrailer") ret=RunTrailer(nm,fn,candidate,r,out);
                    else {
                        out["switches"]=ReversingProbe::JumpsToDll(fn.address)==candidate;
                        ret=cls=="CPedType" ? RunPedType(nm,fn.address,r,out) : RunTimer(nm,fn.address,r,out);
                    }
                    RestoreRegions();
                    out["ret"]=ret; out["trace"]=trace; return out;
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
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()}});
}
}
