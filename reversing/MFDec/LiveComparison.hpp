#pragma once
// Banco dentro del motor: CAEMFDecoder con QuickTime como el EXE (10 funciones: constructor, destructor, las 7
// virtuales de la vtable 0x85F370 salvo el destructor con borrado, e InitLibrary). Original (hooks de la clase
// desactivados) frente a candidato con fixtures deterministas. Nunca se llama a QuickTime real:
// - Todas las funciones de pegamento QuickTime del EXE que tocan estas funciones (0x818430, 0x818450, 0x818490,
//   0x818510, 0x818570, 0x818D70, 0x818DB0, 0x818F90, 0x8191F0, 0x819210, 0x819270, 0x8195C0, 0x819820, 0x819B20,
//   0x81A180) y los ayudantes del EXE 0x4E7990/0x4E7A80 se sustituyen en su dirección del EXE por falsos trazados
//   que devuelven valores del fixture (los OSErr llevan basura en los 16 bits altos). La DLL los llama en esas
//   mismas direcciones, así que el EXE y la DLL pasan por los mismos falsos.
// - Los falsos no reservan memoria: registran en un array fijo (con un hash del objeto en cada llamada para
//   comprobar el orden de escritura) que se convierte a json después de la llamada.
// - El decoder es un objeto falso de 0xC4 bytes con una vtable falsa (GetStreamLengthMs trazado y simulado, para
//   la comprobación de 7 s de Initialise); las virtuales se llaman por el hueco de la vtable del EXE 0x85F370.
// - operator new/delete del EXE (0x82119A/0x821195/0x8214BD/0x8213AE) solo se sustituyen durante el destructor.
// - El flag 0xB61D70 se fija por caso y se restaura tras cada pasada y al final.
// - El hilo de streaming de audio (AEAudioHardware.m_pStreamThread) se aparca durante todo el banco dentro de
//   IsForegroundApp (0x746060 + cuerpo de la DLL), como en FixAudio/Audio13: decodifica user tracks con este decoder.
// - Fuera: el callback 0x4E7B80 (código del EXE solo referenciado por puntero; el falso de 0x818570 no lo llama) y
//   Shutdown (0x4E7850, sin hook: solo llama a 0x819840 y 0x819770).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Loaders/AEMFDecoder.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace MFDecLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& G(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
// Sigue los saltos de la ILT de la DLL, pero nunca entra en la imagen del EXE.
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// --- objetos falsos ---
constexpr size_t OBJ=0xC4;
alignas(16) inline std::array<uint8,OBJ> obj{};
alignas(16) inline std::array<uint8,0x40> nameBuf{};
alignas(16) inline std::array<uint8,0x40> atomBuf{};
alignas(16) inline std::array<uint8,0x10> handleBuf{};
alignas(16) inline std::array<uint8,0x40> srcBuf{};
alignas(16) inline std::array<uint8,0x40> destBuf{};
alignas(16) inline std::array<uintptr_t,8> fakeVt{};
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    auto in=[&](const auto& a){ return u>=uintptr_t(a.data())&&u<uintptr_t(a.data())+a.size(); };
    if(in(obj))return 0x1000u+uint32(u-uintptr_t(obj.data()));
    if(in(nameBuf))return 0x2000u+uint32(u-uintptr_t(nameBuf.data()));
    if(in(atomBuf))return 0x3000u+uint32(u-uintptr_t(atomBuf.data()));
    if(in(handleBuf))return 0x4000u+uint32(u-uintptr_t(handleBuf.data()));
    if(in(srcBuf))return 0x5000u+uint32(u-uintptr_t(srcBuf.data()));
    if(in(destBuf))return 0x6000u+uint32(u-uintptr_t(destBuf.data()));
    return 0xEEEEEEEEu; } // local de la pila (dirección distinta en el EXE y en la DLL)
// Hash del objeto sin el vptr (el destructor lo cambia a la vtable propia del EXE o de la DLL antes de los falsos).
inline uint32 ObjHash(){ return Fnv32(obj.data()+4,OBJ-4); }

// --- registro sin reservas ---
struct Rec { uint32 t, a, b, c, d; };
inline std::array<Rec,64> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec]={t,a,b,c,d}; ++nrec; }
inline json Recs(){
    const size_t n=std::min(nrec,recs.size()); json r=json::array();
    for(size_t i=0;i<n;++i){ const auto& x=recs[i]; r.push_back(json::array({x.t,x.a,x.b,x.c,x.d})); }
    if(nrec>n)r.push_back(json::array({0xFFFFFFFFu,uint32(nrec)}));
    return r; }

// --- fixture de los falsos (se fija antes de cada pasada con la misma semilla) ---
struct Fixture {
    uint32 openMode, openRet; void* movie; void* media;
    uint32 fmtMode, fmtRet; void* atom; std::array<uint8,0x44> info;
    uint32 scOpenMode, scOpenRet; void* sc;
    std::array<uint32,2> setInfoRet; uint32 setInfoIdx;
    uint32 beginRet; int32 duration; void* handle; uint32 memErrRet;
    int32 lenRet; uint32 lenMode;
    uint32 fillMask, fillBytes, fillFrames, fillFlags, fillRet;
    uint32 qtmlRet, enterRet;
};
inline Fixture fx{};
inline uint32 Err(uint32 hi,int16 e){ return (hi<<16)|uint16(e); }

// QuickTime falso (cdecl, como las funciones de pegamento del EXE)
inline uint32 __cdecl FakeOpen(char* name,void** movie,void** media){
    if(!Main())return 0xFFFF;
    Push(0x10,Id(name),Id(movie),Id(media),ObjHash());
    if(fx.openMode!=1){ *movie=fx.movie; *media=fx.media; }
    return fx.openRet; }
inline uint32 __cdecl FakeFormat(void* media,void** atom,uint8* info){
    if(!Main())return 0xFFFF;
    Push(0x11,uint32(uintptr_t(media)),Id(atom),Id(info),ObjHash());
    Push(0x12,uint32(uintptr_t(atom)!=uintptr_t(info)),uint32(uintptr_t(atom) ? uintptr_t(*atom) : 0xBAD),0,0); // el átomo llega a 0
    if(fx.fmtMode!=2){ std::memcpy(info,fx.info.data(),fx.info.size()); if(fx.fmtMode==0)*atom=fx.atom; }
    return fx.fmtRet; }
inline uint32 __cdecl FakeScOpen(void* in,void* out,void** sc){
    if(!Main())return 0xFFFF;
    Push(0x13,Id(in),Id(out),Id(sc),ObjHash());
    if(fx.scOpenMode!=1)*sc=fx.sc;
    return fx.scOpenRet; }
inline uint32 __cdecl FakeSetInfo(void* sc,uint32 sel,void* p){
    if(!Main())return 0xFFFF;
    Push(0x14,uint32(uintptr_t(sc)),sel,uint32(uintptr_t(p)),ObjHash());
    return fx.setInfoRet[fx.setInfoIdx++%2]; }
inline void __cdecl FakeDisposePtr(void* p){ if(!Main())return; Push(0x15,Id(p),ObjHash()); }
inline uint32 __cdecl FakeBegin(void* sc){ if(!Main())return 0xFFFF; Push(0x16,uint32(uintptr_t(sc)),ObjHash()); return fx.beginRet; }
inline int32 __cdecl FakeDuration(void* media){ if(!Main())return 0; Push(0x17,uint32(uintptr_t(media)),ObjHash()); return fx.duration; }
inline void* __cdecl FakeNewHandle(int32 n){ if(!Main())return nullptr; Push(0x18,uint32(n),ObjHash()); return fx.handle; }
inline uint32 __cdecl FakeMemError(){ if(!Main())return 0; Push(0x19,ObjHash()); return fx.memErrRet; }
inline void __cdecl FakeMoveHHi(void* h){ if(!Main())return; Push(0x1A,Id(h),ObjHash()); }
inline void __cdecl FakeHLock(void* h){ if(!Main())return; Push(0x1B,Id(h),ObjHash()); }
inline uint32 __cdecl FakeFill(void* sc,void* proc,void* refCon,void* dest,uint32 size,uint32* bytes,uint32* frames,uint32* flags){
    if(!Main())return 0xFFFF;
    Push(0x1C,uint32(uintptr_t(sc)),uint32(uintptr_t(proc)),Id(refCon),ObjHash());
    Push(0x1D,Id(dest),size,Id(bytes)|Id(frames)|Id(flags),uint32(bytes!=frames&&frames!=flags&&bytes!=flags));
    Push(0x1E,*bytes,*frames,*flags); // puestos a 0 antes de la llamada
    if(fx.fillMask&1)*bytes=fx.fillBytes; if(fx.fillMask&2)*frames=fx.fillFrames; if(fx.fillMask&4)*flags=fx.fillFlags;
    return fx.fillRet; }
inline uint32 __cdecl FakeQtml(int32 f){ if(!Main())return 0xFFFF; Push(0x20,uint32(f),G<uint8>(0xB61D70)); return fx.qtmlRet; }
inline uint32 __cdecl FakeEnterMovies(){ if(!Main())return 0xFFFF; Push(0x21,G<uint8>(0xB61D70)); return fx.enterRet; }
inline void __cdecl FakeDisposeMovie(void* m){ if(!Main())return; Push(0x22,uint32(uintptr_t(m)),ObjHash()); }
inline void __cdecl FakeDisposeHandle(void* h){ if(!Main())return; Push(0x23,uint32(uintptr_t(h)),ObjHash()); }
inline uint32 __cdecl FakeScClose(void* sc){ if(!Main())return 0; Push(0x24,uint32(uintptr_t(sc)),ObjHash()); return 0x7777; }
inline void* __cdecl FakeNew(size_t n){ Push(0x30,uint32(n)); return nullptr; }
inline void __cdecl FakeDelete(void* p){ Push(0x31,Id(p)); }
// vtable falsa del objeto
inline int32 __fastcall FakeLen(uint8* s,void*){
    Push(0x40,Id(s),s[0xC],At<uint32>(s,0xB8),ObjHash());
    return fx.lenMode==0 ? At<int32>(s,0xB8) : fx.lenRet; }
inline uint32 __fastcall FakeBadSlot(uint8* s,void*){ Push(0x41,Id(s)); return 0; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=10;
inline const std::array<Fn,NFN> fns{{
    {"CAEMFDecoder","Initialise",0,0x85F370,0,1024},
    {"CAEMFDecoder","FillBuffer",0,0x85F370,1,512},
    {"CAEMFDecoder","InitLibrary",0x4E7C70,0,-1,256},
    {"CAEMFDecoder","Constructor",0x4E7770,0,-1,128},
    {"CAEMFDecoder","Destructor",0x4E77C0,0,-1,512},
    {"CAEMFDecoder","GetStreamLengthMs",0,0x85F370,2,256},
    {"CAEMFDecoder","GetStreamPlayTimeMs",0,0x85F370,3,512},
    {"CAEMFDecoder","SetCursor",0,0x85F370,4,512},
    {"CAEMFDecoder","GetSampleRate",0,0x85F370,5,256},
    {"CAEMFDecoder","GetStreamID",0,0x85F370,7,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"mfdec-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"mfdec-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("mfdec");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"mfdec-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // vtables de la DLL: se leen construyendo y destruyendo un CAEMFDecoder de la DLL (sin nombre, sin inicializar)
    uint32 dllMfVt{}, dllBaseVt{};
    { alignas(16) std::array<uint8,sizeof(CAEMFDecoder)> tmp{}; auto* d=new(tmp.data()) CAEMFDecoder(nullptr,0); dllMfVt=At<uint32>(tmp.data(),0); d->~CAEMFDecoder(); dllBaseVt=At<uint32>(tmp.data(),0); }
    auto vtName=[&](uint32 v)->json{ if(v==uintptr_t(fakeVt.data()))return "fake"; if(v==0x85F370||v==dllMfVt)return "CAEMFDecoder"; if(v==0x85F418||v==dllBaseVt)return "CAEStreamingDecoder"; return v; };
    for(auto& s:fakeVt)s=reinterpret_cast<uintptr_t>(&FakeBadSlot);
    fakeVt[2]=reinterpret_cast<uintptr_t>(&FakeLen);

    // estado global vigilado (cuerpos de la DLL resueltos antes de poner ningún parche)
    const uintptr_t dllForeground=Body(std::bit_cast<uintptr_t>(&IsForegroundApp));
    const uint8 savedFlag=G<uint8>(0xB61D70);
    const auto savedVt=ReversingProbe::Bytes(reinterpret_cast<void*>(0x85F370),8*4);
    static constexpr std::array<uintptr_t,17> qtAddrs{0x4E7990,0x4E7A80,0x818430,0x818450,0x818490,0x818510,0x818570,0x818D70,0x818DB0,
                                                      0x818F90,0x8191F0,0x819210,0x819270,0x8195C0,0x819820,0x819B20,0x81A180};
    std::vector<std::vector<uint8>> savedQt; for(auto a:qtAddrs)savedQt.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),5));

    // aparcar el hilo de streaming de audio
    parkedEvt=CreateEventA(nullptr,FALSE,FALSE,nullptr); resumeEvt=CreateEventA(nullptr,TRUE,FALSE,nullptr);
    const HANDLE streamThread=AEAudioHardware.m_pStreamThread.m_nHandle;
    const bool streamRunning=streamThread&&WaitForSingleObject(streamThread,0)==WAIT_TIMEOUT;
    streamTid=streamRunning ? GetThreadId(streamThread) : 0;
    bool parkedOk=!streamRunning;
    std::array<bool,NFN> enabled{},locked{};
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch fg1(0x746060,FakeForeground), fg2(dllForeground,FakeForeground);
        if(streamRunning)parkedOk=WaitForSingleObject(parkedEvt,3000)==WAIT_OBJECT_0;
        if(!parkedOk){ SetEvent(resumeEvt); Write({{"event","error"},{"reason","audio stream thread not parked"},{"state_restored",true}}); return; }
        Write({{"event","stream_thread"},{"running",streamRunning},{"parked",streamRunning}});
        for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

        // QuickTime falso en las direcciones del EXE (la DLL llama a esas mismas direcciones)
        EntryPatch q1(0x4E7990,FakeOpen), q2(0x4E7A80,FakeFormat), q3(0x818430,FakeScOpen), q4(0x818450,FakeScClose);
        EntryPatch q5(0x818490,FakeBegin), q6(0x818510,FakeSetInfo), q7(0x818570,FakeFill), q8(0x818D70,FakeMemError);
        EntryPatch q9(0x818DB0,FakeNewHandle), q10(0x818F90,FakeHLock), q11(0x8191F0,FakeMoveHHi), q12(0x819210,FakeDisposePtr);
        EntryPatch q13(0x819270,FakeDisposeHandle), q14(0x8195C0,FakeQtml), q15(0x819820,FakeEnterMovies), q16(0x819B20,FakeDisposeMovie);
        EntryPatch q17(0x81A180,FakeDuration);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3Du; if(!stt)stt=1; nrec=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // objeto y buffers
                    for(auto& b:obj)b=uint8(next());
                    for(auto& b:nameBuf)b=uint8('a'+next()%26); nameBuf.back()=0;
                    for(auto& b:atomBuf)b=uint8(next()); for(auto& b:srcBuf)b=uint8(next()); destBuf.fill(0x5A);
                    handleBuf.fill(0); At<void*>(handleBuf.data(),0)=srcBuf.data()+4*(next()%8);
                    At<uintptr_t>(obj.data(),0)=uintptr_t(fakeVt.data()); At<uint32>(obj.data(),4)=0;
                    At<void*>(obj.data(),8)=next()%8 ? static_cast<void*>(nameBuf.data()+4*(next()%4)) : nullptr;
                    obj[0xC]=uint8(next()%4 ? 1 : 0);
                    { static const uint32 divs[]{0x42306666u,0x3F800000u,0x3D34A234u,0x447A0000u,0x3E2AAAABu,0u,0xBF800000u};
                      At<uint32>(obj.data(),0xC0)=next()%3 ? divs[next()%2] : divs[next()%7]; }
                    { auto& pt=At<int32>(obj.data(),0xA0); switch(next()%4){ case 0: pt=int32(next()); break; case 1: pt=-int32(next()%100000); break; default: pt=int32(next()%30000000); } }

                    // fixture de QuickTime
                    auto hi=[&]{ return next()%3 ? 0u : next()&0xFFFFu; };
                    auto err=[&](bool ok)->uint32{ if(ok)return Err(hi(),0); static const int16 errs[]{-2009,-2008,-50,-108,-231,1,-1,int16(0x8000)}; return Err(hi(),errs[next()%8]); };
                    fx={};
                    fx.openMode=next()%3; fx.openRet=err(next()%8!=0); if(fx.openMode==1&&next()%2==0)fx.openRet=err(false);
                    fx.movie=reinterpret_cast<void*>(uintptr_t(0x6D000000u|(next()&0xFFF0u))); fx.media=reinterpret_cast<void*>(uintptr_t(0x6E000000u|(next()&0xFFF0u)));
                    fx.fmtMode=next()%4==0 ? 1 : next()%10==0 ? 2 : 0; fx.fmtRet=err(fx.fmtMode!=2&&next()%10!=0);
                    fx.atom=next()%5 ? static_cast<void*>(atomBuf.data()+4*(next()%4)) : nullptr;
                    for(auto& b:fx.info)b=uint8(next());
                    { static const uint16 ch[]{1,2,6,0xFFFF}; At<uint32>(fx.info.data(),4)=uint32(int32(int16(next()%4 ? ch[next()%2] : ch[next()%4])));
                      static const uint32 rates[]{0xAC440000u,0xBB800000u,0x56220000u,0x5DC00000u}; if(next()%4)At<uint32>(fx.info.data(),8)=rates[next()%4];
                      static const uint32 fmts[]{0x2E6D7033u,0x6D703461u,0x736F7774u,0x74776F73u}; if(next()%4)At<uint32>(fx.info.data(),0x28)=fmts[next()%4];
                      static const uint16 cids[]{0xFFFE,0,0xFFFF,0xFFFE,0x0102,0xFEFF}; At<uint16>(fx.info.data(),0x38)=cids[next()%6];
                      if(next()%4)At<uint16>(fx.info.data(),0x3E)=uint16(next()%2 ? 16 : 8); }
                    fx.scOpenMode=next()%6==0 ? 1 : 0; fx.scOpenRet=err(next()%8!=0); fx.sc=reinterpret_cast<void*>(uintptr_t(0x5C000000u|(next()&0xFFF0u)));
                    { const uint32 r=next()%8; fx.setInfoRet[0]=err(next()%3!=0); fx.setInfoRet[1]=r<4 ? err(true) : r<6 ? Err(hi(),-231) : r==6 ? Err(hi(),-232) : err(false); }
                    fx.beginRet=err(next()%8!=0);
                    { switch(next()%6){ case 0: fx.duration=int32(next()%400000); break; case 1: fx.duration=int32(308700+next()%64)-32; break; /* ~7000 ms */
                        case 2: fx.duration=-int32(next()%100000); break; case 3: fx.duration=int32(next()); break; default: fx.duration=int32(441000+next()%20000000); } }
                    fx.memErrRet=err(next()%8!=0);
                    fx.handle=handleBuf.data(); if(uint16(fx.memErrRet)!=0&&next()%2)fx.handle=nullptr;
                    fx.lenMode=next()%3; { static const int32 lens[]{7000,6999,-1,0,7001,int32(0x80000000u)}; fx.lenRet=next()%2 ? lens[next()%6] : int32(next()); }
                    fx.fillMask=next()%4 ? 7 : next()%8; fx.fillBytes=next()%3 ? next()%0x10000 : next(); fx.fillFrames=next()%0x4000; fx.fillFlags=next()%4; fx.fillRet=err(next()%4!=0);
                    fx.qtmlRet=err(next()%3!=0); fx.enterRet=err(next()%3!=0);
                    const uint8 flag0=uint8(next()%3==0 ? 1 : 0); G<uint8>(0xB61D70)=flag0;

                    if(nm=="Initialise")ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(obj.data());
                    else if(nm=="FillBuffer"){ const uint32 size=next()%4 ? 0x10000u>>(next()%6) : next();
                        ret=reinterpret_cast<uint32(__thiscall*)(void*,void*,uint32)>(a)(obj.data(),destBuf.data()+4*(next()%4),size); }
                    else if(nm=="InitLibrary")ret=reinterpret_cast<bool(__cdecl*)()>(a)();
                    else if(nm=="Constructor"){ obj.fill(0xCD); char* name=next()%4 ? reinterpret_cast<char*>(nameBuf.data()+8*(next()%4)) : nullptr; const int32 track=int32(next());
                        ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*,char*,int32)>(a)(obj.data(),name,track))-intptr_t(obj.data()); }
                    else if(nm=="Destructor"){ EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); }
                    else if(nm=="SetCursor"){ const uint32 pos=next()%3 ? next()%600000 : next()%4 ? next() : 0x80000000u|next();
                        reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(obj.data(),pos); }
                    else ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(obj.data());

                    out["flag"]=G<uint8>(0xB61D70); G<uint8>(0xB61D70)=savedFlag;
                    out["vptr"]=vtName(At<uint32>(obj.data(),0)); At<uint32>(obj.data(),0)=0;
                    out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    out["dest"]=Fnv32(destBuf.data(),destBuf.size());
                    out["ret"]=ret; out["recs"]=Recs(); return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",std::min(nrec,recs.size())}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
        SetEvent(resumeEvt); // el hilo de streaming sigue con todos los hooks como estaban
    }
    G<uint8>(0xB61D70)=savedFlag;
    bool restored=G<uint8>(0xB61D70)==savedFlag&&ReversingProbe::Bytes(reinterpret_cast<void*>(0x85F370),8*4)==savedVt;
    for(size_t k=0;k<qtAddrs.size();++k)restored&=ReversingProbe::Bytes(reinterpret_cast<void*>(qtAddrs[k]),5)==savedQt[k];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
