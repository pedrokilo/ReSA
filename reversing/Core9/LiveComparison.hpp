#pragma once
// Banco dentro del motor: decodificador WAV, reloj, CdStream, CGeneral y comunes (lista de 9). 45 funciones de
// CAEWaveDecoder, CClock, CdStream, CGeneral y Common. Original (hooks del lote desactivados) frente a candidato
// con fixtures deterministas. Nada toca ficheros, streams, hilos ni semáforos reales: CAEDataStream
// (FillBuffer/GetCurrentPosition/Seek/Initialise) se simula sobre streams falsos; la API Win32 de CdStream
// (CreateFileA, ReadFile, SetFilePointer, GetOverlappedResult, WaitForSingleObject[Ex], ReleaseSemaphore,
// CreateSemaphoreA, CreateThread, prioridades, LocalAlloc/LocalFree, CloseHandle, GetDiskFreeSpaceA) se
// sustituye por IAT solo en el hilo principal y con handles falsos; los arrays de streams y la cola son buffers
// propios y la región global de CdStream (0x8E3FE0..0x8E489C) se guarda y se restaura tras cada ejecución. Antes de
// tocarla se espera a que el hilo real de CdStream esté parado en su semáforo (cola vacía, sin lecturas en curso).
// CdStreamThread corre una sola vuelta: la segunda espera lanza una excepción que recoge el banco. También se
// simulan rand/CGeneral::GetRandomNumber, CStats::IncrementStat, CMemoryMgr::MallocAlign/FreeAlign y las primitivas
// RwImage/RtPNG. Los valores flotantes devueltos en ST(0) se comparan con sus 80 bits.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Loaders/AEWaveDecoder.h"
#include "Audio/Loaders/AEDataStream.h"
#include "Clock.h"
#include "CdStreamInfo.h"
#include "General.h"
#include "Stats.h"
#include "MemoryMgr.h"
#include "Draw.h"
#include "Camera.h"
#include "GxtChar.h"
#include "../ProbeRuntime.hpp"

namespace Core9Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 seed{}, rngCalls{};
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }
// Otros hilos (audio) no entran en la traza: reciben un valor pseudoaleatorio propio
inline int __cdecl FakeRand() {
    if(!Main()){ static std::atomic<uint32> x{12345u}; const uint32 v=x.load()*214013u+2531011u; x.store(v); return int(v>>16)&0x7FFF; }
    trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- Win32 por IAT (solo el hilo principal; los demas hilos van a la API real) ---
inline HMODULE SelfModule(){ HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&SelfModule),&m); return m; }
inline uintptr_t RealApi(const char* fn){ if(auto m=GetModuleHandleA("kernel32.dll"))if(auto p=GetProcAddress(m,fn))return uintptr_t(p); return 0; }
struct IatPatch {
    std::vector<std::pair<uintptr_t*,uintptr_t>> slots;
    static void Set(uintptr_t* t,uintptr_t v){ DWORD o{}; VirtualProtect(t,4,PAGE_READWRITE,&o); *t=v; VirtualProtect(t,4,o,&o); }
    template<class Fp> IatPatch(const char* fn,Fp fakeFn) {
        const auto fake=reinterpret_cast<uintptr_t>(fakeFn);
        std::array<uintptr_t,2> reals{RealApi(fn),0};
        if(auto kb=GetModuleHandleA("kernelbase.dll"))reals[1]=uintptr_t(GetProcAddress(kb,fn));
        for(HMODULE m:{GetModuleHandleA(nullptr),SelfModule()}) {
            auto* base=reinterpret_cast<uint8*>(m); auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
            const auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if(!dir.VirtualAddress)continue;
            for(auto* d=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);d->Name;++d)
                for(auto* t=reinterpret_cast<uintptr_t*>(base+d->FirstThunk);*t;++t)
                    if(*t==reals[0]||(reals[1]&&*t==reals[1])){ slots.push_back({t,*t}); Set(t,fake); }
        }
    }
    ~IatPatch(){ for(auto& [t,v]:slots)Set(t,v); }
};
template<class T> inline T Real(const char* fn){ return reinterpret_cast<T>(RealApi(fn)); }

// --- buffers de CdStream ---
alignas(16) inline std::array<uint8,0x30*8> streams{};
alignas(16) inline std::array<int32,16> qbuf{};
alignas(16) inline std::array<uint8,0x400> localArena{}; inline size_t localTop{};
alignas(16) inline std::array<uint8,0x1000> alignArena{};
alignas(16) inline std::array<uint8,0x1000> readBuf{};
inline json P(const void* p){
    if(!p)return nullptr; const auto u=uintptr_t(p);
    auto in=[&](const auto& a){ return u>=uintptr_t(a.data())&&u<uintptr_t(a.data())+sizeof(a); };
    if(in(streams))return json::array({"streams",u-uintptr_t(streams.data())});
    if(in(qbuf))return json::array({"queue",u-uintptr_t(qbuf.data())});
    if(in(localArena))return json::array({"local",u-uintptr_t(localArena.data())});
    if(in(alignArena))return json::array({"align",u-uintptr_t(alignArena.data())});
    if(in(readBuf))return json::array({"read",u-uintptr_t(readBuf.data())});
    if(u==0x8E4000)return "thread_id";
    if(u<0x10000)return u;
    return "other"; }
inline uint32 apiMask{}, apiCalls{}, errMask{}, waitCalls{}, waitThrowAt{}, createCount{}, semCount{}, diskBps{};
inline uintptr_t dllThread{};
inline bool Ok(){ return (apiMask>>(apiCalls++%32))&1; }
struct ThreadStop {};
inline json Str(const char* s){ return s ? json(std::string(s,strnlen(s,96))) : json(nullptr); }
inline HANDLE WINAPI FakeCreateFileA(LPCSTR n,DWORD acc,DWORD share,LPSECURITY_ATTRIBUTES sa,DWORD disp,DWORD fl,HANDLE tpl){
    if(!Main())return Real<decltype(&CreateFileA)>("CreateFileA")(n,acc,share,sa,disp,fl,tpl);
    trace.push_back({"CreateFileA",Str(n),acc,share,P(sa),disp,fl,P(tpl)}); return Ok() ? reinterpret_cast<HANDLE>(uintptr_t(0x6100+createCount++)) : INVALID_HANDLE_VALUE; }
inline BOOL WINAPI FakeCloseHandle(HANDLE h){
    if(!Main())return Real<decltype(&CloseHandle)>("CloseHandle")(h);
    trace.push_back({"CloseHandle",P(h)}); return TRUE; }
inline HLOCAL WINAPI FakeLocalAlloc(UINT fl,SIZE_T n){
    if(!Main())return Real<decltype(&LocalAlloc)>("LocalAlloc")(fl,n);
    trace.push_back({"LocalAlloc",fl,n}); const size_t off=localTop; if(off+n>localArena.size())return nullptr; localTop=(off+n+15)&~size_t(15);
    std::memset(localArena.data()+off,(fl&LMEM_ZEROINIT) ? 0 : 0xAB,n); return localArena.data()+off; }
inline HLOCAL WINAPI FakeLocalFree(HLOCAL h){
    if(!Main())return Real<decltype(&LocalFree)>("LocalFree")(h);
    trace.push_back({"LocalFree",P(h)}); return nullptr; }
inline DWORD WINAPI FakeWait(HANDLE h,DWORD ms){
    if(!Main())return Real<decltype(&WaitForSingleObject)>("WaitForSingleObject")(h,ms);
    trace.push_back({"Wait",P(h),ms}); if(++waitCalls==waitThrowAt)throw ThreadStop{}; return WAIT_OBJECT_0; }
inline DWORD WINAPI FakeWaitEx(HANDLE h,DWORD ms,BOOL al){
    if(!Main())return Real<decltype(&WaitForSingleObjectEx)>("WaitForSingleObjectEx")(h,ms,al);
    trace.push_back({"WaitEx",P(h),ms,al}); return Ok() ? WAIT_OBJECT_0 : WAIT_TIMEOUT; }
inline BOOL WINAPI FakeOverlapped(HANDLE h,LPOVERLAPPED ov,LPDWORD n,BOOL w){
    if(!Main())return Real<decltype(&GetOverlappedResult)>("GetOverlappedResult")(h,ov,n,w);
    trace.push_back({"GetOverlappedResult",P(h),P(ov),n!=nullptr,w}); if(n)*n=0x1234; return Ok(); }
inline BOOL WINAPI FakeRelease(HANDLE h,LONG c,LPLONG prev){
    if(!Main())return Real<decltype(&ReleaseSemaphore)>("ReleaseSemaphore")(h,c,prev);
    trace.push_back({"ReleaseSemaphore",P(h),c,P(prev)}); return Ok(); }
inline DWORD WINAPI FakeSetFilePointer(HANDLE h,LONG d,PLONG hi,DWORD m){
    if(!Main())return Real<decltype(&SetFilePointer)>("SetFilePointer")(h,d,hi,m);
    trace.push_back({"SetFilePointer",P(h),d,P(hi),m}); return DWORD(d); }
inline BOOL WINAPI FakeReadFile(HANDLE h,LPVOID buf,DWORD n,LPDWORD rd,LPOVERLAPPED ov){
    if(!Main())return Real<decltype(&ReadFile)>("ReadFile")(h,buf,n,rd,ov);
    trace.push_back({"ReadFile",P(h),P(buf),n,rd!=nullptr,P(ov),ov ? ov->Offset : 0u,ov ? ov->OffsetHigh : 0u});
    const bool ok=Ok(); const bool pending=(errMask>>(apiCalls%32))&1;
    if(ok){ if(rd)*rd=n; auto* b=static_cast<uint8*>(buf); if(P(buf).is_array())for(DWORD i=0;i<std::min<DWORD>(n,64);++i)b[i]=uint8(i*13+n); }
    else SetLastError(pending ? ERROR_IO_PENDING : ERROR_READ_FAULT);
    return ok; }
inline HANDLE WINAPI FakeCreateSemaphoreA(LPSECURITY_ATTRIBUTES sa,LONG init,LONG mx,LPCSTR name){
    if(!Main())return Real<decltype(&CreateSemaphoreA)>("CreateSemaphoreA")(sa,init,mx,name);
    trace.push_back({"CreateSemaphoreA",P(sa),init,mx,Str(name)}); return Ok() ? reinterpret_cast<HANDLE>(uintptr_t(0x6200+semCount++)) : nullptr; }
inline HANDLE WINAPI FakeCreateThread(LPSECURITY_ATTRIBUTES sa,SIZE_T st,LPTHREAD_START_ROUTINE start,LPVOID prm,DWORD fl,LPDWORD id){
    if(!Main())return Real<decltype(&CreateThread)>("CreateThread")(sa,st,start,prm,fl,id);
    const auto s=uintptr_t(start); const json who=(s==0x406560||s==dllThread) ? json("CdStreamThread") : json(s<0x1000000 ? s : 0xD11u);
    trace.push_back({"CreateThread",P(sa),st,who,P(prm),fl,P(id)}); if(id)*id=0x4321; return Ok() ? reinterpret_cast<HANDLE>(uintptr_t(0x6300)) : nullptr; }
inline HANDLE WINAPI FakeGetCurrentThread(){
    if(!Main())return Real<decltype(&GetCurrentThread)>("GetCurrentThread")();
    trace.push_back({"GetCurrentThread"}); return reinterpret_cast<HANDLE>(uintptr_t(0x6400)); }
inline int WINAPI FakeGetThreadPriority(HANDLE h){
    if(!Main())return Real<decltype(&GetThreadPriority)>("GetThreadPriority")(h);
    trace.push_back({"GetThreadPriority",P(h)}); return 2; }
inline BOOL WINAPI FakeSetThreadPriority(HANDLE h,int p){
    if(!Main())return Real<decltype(&SetThreadPriority)>("SetThreadPriority")(h,p);
    trace.push_back({"SetThreadPriority",P(h),p}); return TRUE; }
inline DWORD WINAPI FakeResumeThread(HANDLE h){
    if(!Main())return Real<decltype(&ResumeThread)>("ResumeThread")(h);
    trace.push_back({"ResumeThread",P(h)}); return 1; }
inline BOOL WINAPI FakeDiskFree(LPCSTR root,LPDWORD spc,LPDWORD bps,LPDWORD fr,LPDWORD tot){
    if(!Main())return Real<decltype(&GetDiskFreeSpaceA)>("GetDiskFreeSpaceA")(root,spc,bps,fr,tot);
    trace.push_back({"GetDiskFreeSpaceA",Str(root)}); if(spc)*spc=8; if(bps)*bps=diskBps; if(fr)*fr=100; if(tot)*tot=200; return TRUE; }
struct CdFakes {
    IatPatch i1{"CreateFileA",FakeCreateFileA}, i2{"CloseHandle",FakeCloseHandle}, i3{"LocalAlloc",FakeLocalAlloc}, i4{"LocalFree",FakeLocalFree};
    IatPatch i5{"WaitForSingleObject",FakeWait}, i6{"WaitForSingleObjectEx",FakeWaitEx}, i7{"GetOverlappedResult",FakeOverlapped}, i8{"ReleaseSemaphore",FakeRelease};
    IatPatch i9{"SetFilePointer",FakeSetFilePointer}, i10{"ReadFile",FakeReadFile}, i11{"CreateSemaphoreA",FakeCreateSemaphoreA}, i12{"CreateThread",FakeCreateThread};
    IatPatch i13{"GetCurrentThread",FakeGetCurrentThread}, i14{"GetThreadPriority",FakeGetThreadPriority}, i15{"SetThreadPriority",FakeSetThreadPriority};
    IatPatch i16{"ResumeThread",FakeResumeThread}, i17{"GetDiskFreeSpaceA",FakeDiskFree};
};
__declspec(noinline) inline bool RunThreadOnce(uintptr_t fn){
    try { reinterpret_cast<void(__stdcall*)(void*)>(fn)(nullptr); } catch(const ThreadStop&) { return true; }
    return false; }
inline void* __cdecl FakeMallocAlign(uint32 n,uint32 al,uint32 hint){ trace.push_back({"MallocAlign",n,al,hint}); return alignArena.data(); }
inline void __cdecl FakeFreeAlign(void* p){ trace.push_back({"FreeAlign",P(p)}); }

// --- CAEDataStream falso ---
alignas(16) inline std::array<std::array<uint8,0x28>,2> fakeStreams{};
alignas(16) inline std::array<uint8,0x80> wavSrc{}; inline uint32 fillShort{}, streamPos{};
inline uint8* objBase{}; inline size_t objSize{};
inline json S(const void* p){ for(size_t k=0;k<fakeStreams.size();++k)if(p==fakeStreams[k].data())return json::array({"stream",k}); return p ? json("other") : json(nullptr); }
// Destino real (cuerpo de la DLL tras el salto incremental) para las llamadas de otros hilos (hilo de streaming de audio)
inline uintptr_t fwdFill{}, fwdPos{}, fwdSeek{}, fwdInit{};
inline uintptr_t ThunkTarget(const ReversingProbe::EntryPatch& p){ return p.saved[0]==0xE9 ? p.address+5+*reinterpret_cast<const int32*>(p.saved.data()+1) : 0; }
inline json D(const void* p){ const auto u=uintptr_t(p); if(objBase&&u>=uintptr_t(objBase)&&u<uintptr_t(objBase)+objSize)return json::array({"obj",u-uintptr_t(objBase)}); if(u>=uintptr_t(readBuf.data())&&u<uintptr_t(readBuf.data())+readBuf.size())return json::array({"read",u-uintptr_t(readBuf.data())}); return "other"; }
inline size_t __fastcall FakeFill(CAEDataStream* s,void*,void* dst,size_t n){
    if(!Main())return fwdFill ? reinterpret_cast<size_t(__thiscall*)(CAEDataStream*,void*,size_t)>(fwdFill)(s,dst,n) : 0;
    trace.push_back({"fill",S(s),D(dst),n}); const size_t got=fillShort&&n>fillShort ? n-fillShort : n;
    auto* d=static_cast<uint8*>(dst); for(size_t i=0;i<std::min<size_t>(got,0x200);++i)d[i]=i<wavSrc.size() ? wavSrc[i] : uint8(i*7+n); return got; }
inline uint32 __fastcall FakeGetPos(CAEDataStream* s,void*){ if(!Main())return fwdPos ? reinterpret_cast<uint32(__thiscall*)(CAEDataStream*)>(fwdPos)(s) : 0; trace.push_back({"get_pos",S(s)}); return streamPos; }
inline uint32 __fastcall FakeSeek(CAEDataStream* s,void*,long off,int32 wh){ if(!Main())return fwdSeek ? reinterpret_cast<uint32(__thiscall*)(CAEDataStream*,long,int32)>(fwdSeek)(s,off,wh) : 0; trace.push_back({"seek",S(s),off,wh}); return uint32(off); }
inline bool __fastcall FakeStreamInit(CAEDataStream* s,void*){ if(!Main())return fwdInit ? reinterpret_cast<bool(__thiscall*)(CAEDataStream*)>(fwdInit)(s) : false; trace.push_back({"stream_init",S(s)}); return true; }

// --- otros stubs ---
inline void __cdecl FakeIncrementStat(uint32 stat,float v){ if(!Main())return; trace.push_back({"increment_stat",stat&0xFFFF,F(v)}); }
alignas(16) inline std::array<uint8,0x40> fakeImage{}; inline uint32 imageMask{};
inline json I(const void* p){ return p==fakeImage.data() ? json("image") : p ? json("other") : json(nullptr); }
inline void* __cdecl FakeImageCreate(int32 w,int32 h,int32 d){ trace.push_back({"image_create",w,h,d}); return imageMask&1 ? fakeImage.data() : nullptr; }
inline void* __cdecl FakeImageAlloc(void* i){ trace.push_back({"image_alloc",I(i)}); return i; }
inline void* __cdecl FakeImageSetFromRaster(void* i,void* r){ trace.push_back({"image_set_from_raster",I(i),r!=nullptr}); return i; }
inline void* __cdecl FakePngWrite(void* i,const char* n){ trace.push_back({"png_write",I(i),Str(n)}); return imageMask&2 ? i : nullptr; }
inline int32 __cdecl FakeImageDestroy(void* i){ trace.push_back({"image_destroy",I(i)}); return 1; }

// Llamada cdecl genérica: devuelve EAX, los 80 bits de ST(0) (si la función devuelve float) y el TOP de la pila x87.
struct CallOut { uint32 eax{}; std::array<uint8,10> st{}; uint32 top{}; };
inline CallOut CallRaw(uintptr_t fn,const uint32* args,int n,bool fpu){
    uint32 r=0; uint16 swv=0; uint8 x87[10]{};
    __asm {
        mov ecx, n
        mov edx, args
    L_push:
        test ecx, ecx
        jz L_call
        dec ecx
        push dword ptr [edx+ecx*4]
        jmp L_push
    L_call:
        call dword ptr fn
        mov r, eax
        mov ecx, n
        lea esp, [esp+ecx*4]
    }
    if(fpu){ __asm { fstp tbyte ptr x87 } }
    __asm { fnstsw word ptr swv }
    CallOut o; o.eax=r; std::memcpy(o.st.data(),x87,10); o.top=(swv>>11)&7; return o; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,45> fns{{
    {"CAEWaveDecoder","Constructor",0x503250,0,-1,128},{"CAEWaveDecoder","Destructor",0x503290,0,-1,64},
    {"CAEWaveDecoder","Initialise",0,0x862DE8,0,512},{"CAEWaveDecoder","FillBuffer",0,0x862DE8,1,256},
    {"CAEWaveDecoder","GetStreamLengthMs",0,0x862DE8,2,1024},{"CAEWaveDecoder","GetStreamPlayTimeMs",0,0x862DE8,3,1024},
    {"CAEWaveDecoder","SetCursor",0,0x862DE8,4,1024},{"CAEWaveDecoder","GetSampleRate",0,0x862DE8,5,128},
    {"CAEWaveDecoder","GetStreamID",0,0x862DE8,7,64},
    {"CClock","GetGameClockMinutesUntil",0x52CEB0,0,-1,512},{"CClock","GetIsTimeInRange",0x52CEE0,0,-1,512},
    {"CClock","Initialise",0x52CD90,0,-1,64},{"CClock","NormaliseGameClock",0x52CDE0,0,-1,512},
    {"CClock","OffsetClockByADay",0x52D0B0,0,-1,512},{"CClock","RestoreClock",0x52D070,0,-1,64},
    {"CClock","SetGameClock",0x52D150,0,-1,512},{"CClock","StoreClock",0x52D020,0,-1,64},{"CClock","Update",0x52CF10,0,-1,1024},
    {"CdStream","CdStreamGetStatus",0x4063E0,0,-1,256},{"CdStream","CdStreamInit",0x406B70,0,-1,128},
    {"CdStream","CdStreamInitThread",0x4068F0,0,-1,256},{"CdStream","CdStreamOpen",0x4067B0,0,-1,256},
    {"CdStream","CdStreamRead",0x406A20,0,-1,512},{"CdStream","CdStreamRemoveImages",0x406690,0,-1,256},
    {"CdStream","CdStreamShutdown",0x406370,0,-1,128},{"CdStream","CdStreamSync",0x406460,0,-1,256},
    {"CdStream","CdStreamThread",0x406560,0,-1,512},
    {"CGeneral","GetATanOfXY",0x53CC70,0,-1,1024},{"CGeneral","GetAngleBetweenPoints",0x53CEA0,0,-1,512},
    {"CGeneral","GetNodeHeadingFromVector",0x53CDC0,0,-1,512},{"CGeneral","GetRadianAngleBetweenPoints-",0x53CBE0,0,-1,1024},
    {"CGeneral","GetRandomNumberInRange<float>-",0x41BD90,0,-1,512},{"CGeneral","GetRandomNumberInRange_int32",0x407180,0,-1,512},
    {"CGeneral","LimitAngle",0x53CB00,0,-1,256},{"CGeneral","LimitRadianAngle",0x53CB50,0,-1,512},{"CGeneral","SolveQuadratic",0x53CE30,0,-1,1024},
    {"Common","AsciiToGxtChar",0x718600,0,-1,256},{"Common","CalcScreenCoors-VV",0x71DAB0,0,-1,512},
    {"Common","CalcScreenCoors-VVff",0x71DA00,0,-1,512},{"Common","DoesInfiniteLineTouchScreen",0x71DB80,0,-1,1024},
    {"Common","IsPointInsideLine",0x71E050,0,-1,512},{"Common","LittleTest",0x541330,0,-1,64},
    {"Common","MakeUpperCase-",0x7186E0,0,-1,256},{"Common","MakeUpperCase-in",0x718710,0,-1,256},{"Common","WriteRaster",0x5A4150,0,-1,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"core9-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"core9-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("core9");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"core9-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,45> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,45> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    dllThread=std::bit_cast<uintptr_t>(&CdStreamThread);
    const uintptr_t waveVt=uintptr_t(GetProcAddress(SelfModule(),"??_7CAEWaveDecoder@@6B@")), baseVt=uintptr_t(GetProcAddress(SelfModule(),"??_7CAEStreamingDecoder@@6B@"));
    auto vtName=[&](uint32 v)->json{ if(v==0x862DE8||(waveVt&&v==waveVt))return "CAEWaveDecoder"; if(v==0x85F418||(baseVt&&v==baseVt)||(!baseVt&&v>=0x1000000))return "CAEStreamingDecoder"; return v; };

    // estado global guardado
    auto* clockBlock=reinterpret_cast<uint8*>(0xB70144); std::array<uint8,0x1C> savedClock{}; std::memcpy(savedClock.data(),clockBlock,0x1C);
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto* cheats=reinterpret_cast<uint8*>(0x969130); std::array<uint8,0x60> savedCheats{}; std::memcpy(savedCheats.data(),cheats,0x60);
    const bool savedFreeze=CClock::gbFreezeTime;
    auto* cdRegion=reinterpret_cast<uint8*>(0x8E3FE0); constexpr size_t CD_SIZE=0x8BC; std::vector<uint8> savedCd; bool cdTouched=false; // se guarda solo con el hilo de CdStream parado
    auto* viewM=reinterpret_cast<uint8*>(&TheCamera.GetViewMatrix()); std::array<uint8,0x40> savedView{}; std::memcpy(savedView.data(),viewM,0x40);
    const int32 savedW=RsGlobal.maximumWidth, savedH=RsGlobal.maximumHeight; const float savedFov=CDraw::ms_fFOV;
    auto& im3d=*reinterpret_cast<int32*>(0xB73708); const int32 savedIm3d=im3d;
    auto restoreAll=[&]{ std::memcpy(clockBlock,savedClock.data(),0x1C); timeMs=savedTime; std::memcpy(cheats,savedCheats.data(),0x60); CClock::gbFreezeTime=savedFreeze;
        if(cdTouched)std::memcpy(cdRegion,savedCd.data(),CD_SIZE); std::memcpy(viewM,savedView.data(),0x40); RsGlobal.maximumWidth=savedW; RsGlobal.maximumHeight=savedH; CDraw::ms_fFOV=savedFov; im3d=savedIm3d; };
    const bool viewOk=uintptr_t(viewM)==0xB6FA2C;

    // El hilo real de CdStream debe estar parado en su semáforo antes de tocar la región global
    auto cdIdle=[&]{
        for(int t=0;t<300;++t){ auto* rs=*reinterpret_cast<uint8**>(0x8E3FFC); const int32 n=*reinterpret_cast<int32*>(0x8E4090); auto* q=reinterpret_cast<int32*>(0x8E3FEC);
            bool idle=q[1]==q[2]; for(int32 k=0;idle&&rs&&k<n&&k<64;++k)if(*reinterpret_cast<uint32*>(rs+0x30*k+4)||rs[0x30*k+0xE])idle=false;
            if(idle){ Sleep(50); return true; } Sleep(10); }
        return false; };
    bool cdReady=false, cdChecked=false;

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(&CAEDataStream::FillBuffer),FakeFill), s1e(0x4DC1C0,FakeFill);
        EntryPatch s2(bit_cast<uintptr_t>(&CAEDataStream::GetCurrentPosition),FakeGetPos), s2e(0x4DC230,FakeGetPos);
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<uint32(CAEDataStream::*)(long,int32)>(&CAEDataStream::Seek)),FakeSeek), s3e(0x4DC250,FakeSeek);
        EntryPatch s4(bit_cast<uintptr_t>(&CAEDataStream::Initialise),FakeStreamInit), s4e(0x4DC2B0,FakeStreamInit);
        fwdFill=ThunkTarget(s1); fwdPos=ThunkTarget(s2); fwdSeek=ThunkTarget(s3); fwdInit=ThunkTarget(s4);
        EntryPatch st1(bit_cast<uintptr_t>(static_cast<void(*)(eStats,float)>(&CStats::IncrementStat)),FakeIncrementStat), st1e(0x55C180,FakeIncrementStat);
        EntryPatch w1(0x8026E0,FakeImageCreate), w2(0x8027A0,FakeImageAlloc), w3(0x804250,FakeImageSetFromRaster), w4(0x7CF600,FakePngWrite), w5(0x802740,FakeImageDestroy);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            if(cls=="CdStream"&&!cdChecked){ cdChecked=true; cdReady=cdIdle(); if(!cdReady)Write({{"event","error"},{"reason","CdStream thread not idle"}});
                else { savedCd.assign(cdRegion,cdRegion+CD_SIZE); cdTouched=true; } }
            if(cls=="CdStream"&&!cdReady){ passed=false; continue; }
            if(cls=="Common"&&nm.starts_with("CalcScreenCoors")&&!viewOk){ Write({{"event","error"},{"reason","view matrix address"}}); passed=false; continue; }
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                // flotante de prueba: rejilla, ceros con signo, magnitudes extremas y NaN silencioso opcional
                auto fv=[&](float lo,float hi,bool nan)->float{ switch(next()%16){
                    case 0: return 0.f; case 1: return -0.f; case 2: return float(int32(next()%41)-20);
                    case 3: return nan&&next()%2 ? std::bit_cast<float>(0x7FC00000u) : uni(lo,hi);
                    case 4: return uni(-1e-3f,1e-3f); case 5: return float(int32(next()%2001)-1000)*0.25f;
                    default: return uni(lo,hi); } };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC9u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    apiCalls=waitCalls=waitThrowAt=createCount=semCount=0; apiMask=next()%4 ? (next()|0x88888888u) : next(); errMask=next(); localTop=0; objBase=nullptr; objSize=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CAEWaveDecoder") {
                        alignas(16) std::array<uint8,0x3C> obj{}; for(auto& b:obj)b=uint8(next()); objBase=obj.data(); objSize=obj.size();
                        for(auto& s:fakeStreams){ for(auto& b:s)b=uint8(next()); }
                        *reinterpret_cast<uint32*>(obj.data())=0x862DE8;
                        *reinterpret_cast<void**>(obj.data()+4)=fakeStreams[0].data(); *reinterpret_cast<void**>(obj.data()+0xC)=next()%8 ? fakeStreams[1].data() : fakeStreams[0].data();
                        obj[8]=uint8(next()%4 ? 1 : 0);
                        static const uint32 rates[]{22050,32000,44100,48000,8000,11025,0x40000001u,0x3FFFFFFFu,0x20000000u,0x80000001u,1,96000};
                        auto& rate=*reinterpret_cast<uint32*>(obj.data()+0x28); rate=next()%3 ? rates[next()%4] : rates[next()%12];
                        auto& dataSize=*reinterpret_cast<uint32*>(obj.data()+0x38);
                        switch(next()%5){ case 0: dataSize=next(); break; case 1: dataSize=0x80000000u|next(); break; case 2: dataSize=0x01000000u+next()%0x10000000u; break;
                            case 3: dataSize=uint32(uint64(rate)*4u*7u)+next()%9-4; break; default: dataSize=next()%0x01000000u; }
                        streamPos=next()%5==0 ? next()%44 : next()%3 ? 44+next()%0x02000000u : next();
                        fillShort=next()%5==0 ? 1+next()%8 : 0;
                        // cabecera WAV de la fuente simulada
                        for(auto& b:wavSrc)b=uint8(next());
                        std::memcpy(wavSrc.data(),"RIFF",4); std::memcpy(wavSrc.data()+8,"WAVEfmt ",8); *reinterpret_cast<uint32*>(wavSrc.data()+0x10)=16;
                        *reinterpret_cast<uint16*>(wavSrc.data()+0x14)=1; *reinterpret_cast<uint16*>(wavSrc.data()+0x16)=uint16(next()%4 ? 2 : next()%4);
                        *reinterpret_cast<uint32*>(wavSrc.data()+0x18)=next()%3 ? rates[next()%4] : rates[next()%12]; *reinterpret_cast<uint16*>(wavSrc.data()+0x22)=uint16(next()%4 ? 16 : 8*(next()%4));
                        std::memcpy(wavSrc.data()+0x24,"data",4);
                        { const uint32 r4=*reinterpret_cast<uint32*>(wavSrc.data()+0x18)*4u; auto& ds=*reinterpret_cast<uint32*>(wavSrc.data()+0x28);
                          switch(next()%4){ case 0: ds=uint32(uint64(r4)*7u)+next()%9-4; break; case 1: ds=next(); break; case 2: ds=0x80000000u|next(); break; default: ds=uint32(uint64(r4)*(next()%20)); } }
                        if(nm=="Constructor"){ obj.fill(0xCD); void* s=next()%4 ? static_cast<void*>(fakeStreams[next()%2].data()) : nullptr;
                            ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*,void*)>(a)(obj.data(),s))-intptr_t(obj.data()); }
                        else if(nm=="Destructor"){ *reinterpret_cast<void**>(obj.data()+4)=nullptr; reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); } // +4 nulo: sin borrar el stream
                        else if(nm=="Initialise")ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="FillBuffer"){ readBuf.fill(0xCD); const size_t n=next()%3 ? 4*(next()%256) : next()%0x400;
                            ret=reinterpret_cast<size_t(__thiscall*)(void*,void*,size_t)>(a)(obj.data(),readBuf.data(),n); out["buf"]=Fnv(readBuf.data(),0x400); }
                        else if(nm=="GetStreamLengthMs")ret=reinterpret_cast<long(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="GetStreamPlayTimeMs")ret=reinterpret_cast<long(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="SetCursor"){ const unsigned long pos=next()%3 ? next()%600000 : next()%4 ? next() : 0x80000000u|next();
                            reinterpret_cast<void(__thiscall*)(void*,unsigned long)>(a)(obj.data(),pos); }
                        else if(nm=="GetSampleRate")ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(obj.data());
                        else ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(obj.data());
                        out["vptr"]=vtName(*reinterpret_cast<uint32*>(obj.data())); *reinterpret_cast<uint32*>(obj.data())=0;
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); out["streams"]=Fnv(fakeStreams.data(),sizeof(fakeStreams));
                    }
                    else if(cls=="CClock") {
                        for(int k=0;k<0x1C;++k)clockBlock[k]=uint8(next());
                        auto& month=clockBlock[0x11]; auto& days=clockBlock[0x10]; auto& hours=clockBlock[0xF]; auto& mins=clockBlock[0xE];
                        auto& secs=*reinterpret_cast<uint16*>(clockBlock+0xC); auto& weekDay=clockBlock[0xA];
                        auto& last=*reinterpret_cast<uint32*>(clockBlock+0x14); auto& msPer=*reinterpret_cast<uint32*>(clockBlock+0x18);
                        const bool tableSafe=nm=="Update"||nm=="OffsetClockByADay";
                        month=uint8(tableSafe ? 1+next()%12 : next()%3 ? 1+next()%12 : next()%256);
                        days=uint8(next()%3 ? 1+next()%31 : next()%4 ? (next()%2 ? 28+next()%4 : 1) : next()%256);
                        hours=uint8(next()%3 ? next()%24 : next()%4 ? 23 : next()%256); mins=uint8(next()%3 ? next()%60 : next()%4 ? 59 : next()%256);
                        secs=uint16(next()%3 ? next()%60 : next()); weekDay=uint8(next()%3 ? 1+next()%7 : next()%4 ? 7 : next()%256);
                        msPer=next()%4 ? 1+next()%5000 : 1+next(); timeMs=next();
                        switch(next()%4){ case 0: last=timeMs-msPer-next()%3+1; break; case 1: last=timeMs+next()%5000; break; case 2: last=timeMs-next()%(2*msPer+1); break; default: last=next(); }
                        cheats[11]=uint8(next()%5==0); cheats[55]=uint8(next()%8==0); cheats[56]=uint8(next()%8==0); CClock::gbFreezeTime=false;
                        if(nm=="GetGameClockMinutesUntil")ret=reinterpret_cast<uint16(__cdecl*)(uint8,uint8)>(a)(uint8(next()%3 ? next()%24 : next()),uint8(next()%3 ? next()%60 : next()));
                        else if(nm=="GetIsTimeInRange")ret=reinterpret_cast<bool(__cdecl*)(uint8,uint8)>(a)(uint8(next()%3 ? next()%25 : next()),uint8(next()%3 ? next()%25 : next()));
                        else if(nm=="Initialise")reinterpret_cast<void(__cdecl*)(uint32)>(a)(next()%2 ? 1000 : next());
                        else if(nm=="NormaliseGameClock")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="OffsetClockByADay")reinterpret_cast<void(__cdecl*)(uint32)>(a)(next()%2 ? 0u : next()%3 ? 1u : next());
                        else if(nm=="RestoreClock")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="SetGameClock")reinterpret_cast<void(__cdecl*)(uint8,uint8,uint8)>(a)(uint8(next()%3 ? next()%24 : next()),uint8(next()%3 ? next()%60 : next()),uint8(next()%3 ? next()%8 : next()));
                        else if(nm=="StoreClock")reinterpret_cast<void(__cdecl*)()>(a)();
                        else reinterpret_cast<void(__cdecl*)()>(a)();
                        out["clock"]=ReversingProbe::Bytes(clockBlock,0x1C); out["time"]=timeMs;
                        std::memcpy(clockBlock,savedClock.data(),0x1C); timeMs=savedTime; std::memcpy(cheats,savedCheats.data(),0x60); CClock::gbFreezeTime=savedFreeze;
                    }
                    else if(cls=="CdStream") {
                        const int32 count=nm=="CdStreamInitThread"||nm=="CdStreamShutdown" ? (next()%10==0 ? -1 : int32(next()%7)) : int32(1+next()%6);
                        auto count0=[](int32 c){ return uint32(c>0 ? c : 1); };
                        for(auto& b:streams)b=uint8(next()); for(auto& q:qbuf)q=int32(next()%count0(count));
                        for(int32 k=0;k<8;++k){ uint8* s=streams.data()+0x30*k;
                            *reinterpret_cast<uint32*>(s)=next()%0x10000; *reinterpret_cast<uint32*>(s+4)=next()%3 ? 0 : 1+next()%32;
                            *reinterpret_cast<void**>(s+8)=readBuf.data()+64*(k%8); s[0xD]=uint8(next()%2); s[0xE]=uint8(next()%4==0);
                            static const uint32 sts[]{0,0,250,254,255,7}; *reinterpret_cast<uint32*>(s+0x10)=sts[next()%6];
                            *reinterpret_cast<uintptr_t*>(s+0x14)=0x6600+k; *reinterpret_cast<uintptr_t*>(s+0x18)=next()%5 ? 0x6700+k : 0; }
                        auto u32=[&](uintptr_t ad)->uint32&{ return *reinterpret_cast<uint32*>(ad); };
                        u32(0x8E3FE0)=next()%2 ? 0x40000000u : 0x60000000u; u32(0x8E3FE4)=next()%2; u32(0x8E3FE8)=next()%2;
                        u32(0x8E3FEC)=uint32(uintptr_t(qbuf.data())); const int32 qsize=count>0 ? count+1 : 1; u32(0x8E3FF8)=uint32(qsize);
                        u32(0x8E3FF0)=next()%qsize; u32(0x8E3FF4)=next()%3 ? next()%qsize : u32(0x8E3FF0);
                        u32(0x8E3FFC)=uint32(uintptr_t(streams.data())); u32(0x8E4000)=next(); u32(0x8E4004)=0x6001; u32(0x8E4008)=0x6002; u32(0x8E400C)=next();
                        const uint32 freeSlot=next()%32;
                        for(uint32 k=0;k<32;++k)u32(0x8E4010+4*k)=k==freeSlot||next()%3==0 ? 0u : 0x6500u+k;
                        u32(0x8E4090)=uint32(count); u32(0x8E4094)=next()%33;
                        for(int k=0;k<0x800;++k)cdRegion[0xB8+k]=uint8('A'+next()%26);
                        u32(0x8E4898)=next();
                        readBuf.fill(0xCD); diskBps=0x801u+next()%4*0x800u; uint32 posFile=0;
                        bool threadStopped=false;
                        {
                            CdFakes fakes;
                            if(nm=="CdStreamGetStatus")ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(int32(next()%count0(count)));
                            else if(nm=="CdStreamSync")ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(int32(next()%count0(count)));
                            else if(nm=="CdStreamInit"){ // MallocAlign/FreeAlign solo durante la llamada
                                EntryPatch m1(bit_cast<uintptr_t>(static_cast<uint8*(*)(uint32,uint32,uint32)>(&CMemoryMgr::MallocAlign)),FakeMallocAlign), m1e(0x72F4C0,FakeMallocAlign);
                                EntryPatch m2(bit_cast<uintptr_t>(&CMemoryMgr::FreeAlign),FakeFreeAlign), m2e(0x72F4F0,FakeFreeAlign);
                                reinterpret_cast<void(__cdecl*)(int32)>(a)(count); }
                            else if(nm=="CdStreamInitThread")reinterpret_cast<void(__cdecl*)()>(a)();
                            else if(nm=="CdStreamOpen"){ static const char* names[]{"MODELS\\GTA3.IMG","MODELS\\GTA_INT.IMG","anim\\anim.img","x","DATA\\SCRIPT\\SCRIPT.IMG","audio\\SFX\\GENRL"};
                                ret=reinterpret_cast<uint32(__cdecl*)(const char*)>(a)(names[next()%6]); }
                            else if(nm=="CdStreamRead"){ posFile=next()%4 ? 0 : next()%8; const uint32 pos=(posFile<<24)|(next()%0x1000000);
                                ret=reinterpret_cast<bool(__cdecl*)(int32,void*,uint32,int32)>(a)(int32(next()%count0(count)),readBuf.data()+16*(next()%4),pos,int32(1+next()%16)); }
                            else if(nm=="CdStreamRemoveImages")reinterpret_cast<void(__cdecl*)()>(a)();
                            else if(nm=="CdStreamShutdown")reinterpret_cast<void(__cdecl*)()>(a)();
                            else { waitThrowAt=2; if(u32(0x8E3FF4)==u32(0x8E3FF0))u32(0x8E3FF4)=(u32(0x8E3FF0)+1)%qsize; // cola no vacía
                                threadStopped=RunThreadOnce(a); }
                        }
                        // FIX_BUGS de la DLL (gLastCdStreamPosn = sectores + Offset): con FileID != 0 no se compara
                        if(nm=="CdStreamRead"&&posFile)u32(0x8E4898)=0;
                        out["region"]=ReversingProbe::Bytes(cdRegion,CD_SIZE); out["streams"]=ReversingProbe::Bytes(streams.data(),0x30*8); out["queue"]=ReversingProbe::Bytes(qbuf.data(),sizeof(qbuf));
                        out["local"]=ReversingProbe::Bytes(localArena.data(),localTop); out["read"]=Fnv(readBuf.data(),readBuf.size()); out["thread_stopped"]=threadStopped;
                        std::memcpy(cdRegion,savedCd.data(),CD_SIZE);
                    }
                    else if(cls=="CGeneral") {
                        std::array<uint32,8> args{}; int nargs=0; bool fpu=true; bool useEax=false; float x1=std::bit_cast<float>(0x7F7F1234u), x2=x1;
                        if(nm=="LimitAngle"){ args[0]=F(fv(-5000.f,5000.f,false)); nargs=1; }
                        else if(nm=="LimitRadianAngle"){ const uint32 k=next()%8; args[0]=F(k==0 ? fv(-1e30f,1e30f,true) : fv(-40.f,40.f,true)); nargs=1; }
                        else if(nm=="GetRadianAngleBetweenPoints-"||nm=="GetAngleBetweenPoints"){ const bool nan=nm!="GetAngleBetweenPoints";
                            const float s=next()%4 ? 3000.f : 1e25f; for(int k=0;k<4;++k)args[k]=F(fv(-s,s,nan));
                            if(next()%6==0)args[2]=args[0]; if(next()%6==0)args[3]=args[1]; nargs=4; }
                        else if(nm=="GetATanOfXY"){ const float s=next()%4 ? 3000.f : 1e25f; args[0]=F(fv(-s,s,true)); args[1]=F(fv(-s,s,true));
                            if(next()%8==0)args[1]=args[0]^(next()%2 ? 0x80000000u : 0u); nargs=2; }
                        else if(nm=="GetNodeHeadingFromVector"){ args[0]=F(fv(-3000.f,3000.f,false)); args[1]=F(fv(-3000.f,3000.f,false)); nargs=2; fpu=false; useEax=true; }
                        else if(nm=="SolveQuadratic"){ auto av=[&]{ const float m=uni(0.01f,1000.f); return next()%2 ? m : -m; };
                            const float big=next()%4 ? 1000.f : 1e25f; args[0]=F(next()%10==0 ? float(int32(next()%9)-4)+0.5f : av());
                            args[1]=F(fv(-big,big,true)); args[2]=F(fv(-big,big,true)); args[3]=uint32(uintptr_t(&x1)); args[4]=uint32(uintptr_t(&x2)); nargs=5; fpu=false; }
                        else if(nm=="GetRandomNumberInRange<float>-"){ const float s=next()%4 ? 1000.f : 1e30f; args[0]=F(fv(-s,s,false)); args[1]=F(fv(-s,s,false)); nargs=2; }
                        else { int32 lo,hi; switch(next()%4){ case 0: lo=int32(next()%200)-100; hi=lo+int32(next()%300); break; case 1: lo=int32(next()); hi=int32(next()); break;
                                case 2: lo=-int32(next()%0x40000000); hi=int32(next()%0x40000000); break; default: lo=int32(next()%0x2000000); hi=lo+0x1000000+int32(next()%0x4000000); }
                            args[0]=uint32(lo); args[1]=uint32(hi); nargs=2; fpu=false; useEax=true; }
                        const auto o=CallRaw(a,args.data(),nargs,fpu);
                        if(fpu)out["st"]=std::vector<uint8>(o.st.begin(),o.st.end());
                        if(useEax)ret=o.eax; else if(nm=="SolveQuadratic"){ ret=o.eax&0xFF; out["x1"]=F(x1); out["x2"]=F(x2); }
                        out["top"]=o.top;
                    }
                    else if(cls=="Common") {
                        RsGlobal.maximumWidth=next()%4 ? int32(640+next()%3200) : int32(1+next()%100000); RsGlobal.maximumHeight=next()%4 ? int32(480+next()%2000) : int32(1+next()%100000);
                        const float W=float(RsGlobal.maximumWidth), H=float(RsGlobal.maximumHeight);
                        if(nm=="AsciiToGxtChar"||nm=="MakeUpperCase-"||nm=="MakeUpperCase-in"){
                            alignas(16) std::array<char,128> src{}, dst{}; dst.fill(char(0xCD)); const size_t len=next()%4 ? next()%40 : next()%100;
                            for(size_t k=0;k<len;++k){ const uint32 c=next()%5; src[k]=char(c==0 ? 'a'+next()%26 : c==1 ? 'A'+next()%26 : c==2 ? 0x60+next()%0x20 : c==3 ? 1+next()%255 : 0x20+next()%0x60); }
                            src[len]=0; for(size_t k=len+1;k<src.size();++k)src[k]=char(next());
                            if(nm=="AsciiToGxtChar")reinterpret_cast<void(__cdecl*)(const char*,char*)>(a)(src.data(),dst.data());
                            else if(nm=="MakeUpperCase-")ret=intptr_t(reinterpret_cast<char*(__cdecl*)(char*,const char*)>(a)(dst.data(),src.data()))-intptr_t(dst.data());
                            else reinterpret_cast<char*(__cdecl*)(char*)>(a)(src.data()); // el EXE no fija el valor devuelto
                            out["src"]=ReversingProbe::Bytes(src.data(),src.size()); out["dst"]=ReversingProbe::Bytes(dst.data(),dst.size()); }
                        else if(nm.starts_with("CalcScreenCoors")){
                            auto* m=reinterpret_cast<float*>(viewM); for(int k=0;k<16;++k)m[k]=(k%4==3) ? std::bit_cast<float>(next()) : k>=12 ? uni(-1000.f,1000.f) : uni(-1.f,1.f);
                            for(int k=3;k<16;k+=4)reinterpret_cast<uint32*>(m)[k]=next(); // relleno
                            CDraw::ms_fFOV=next()%4 ? uni(30.f,110.f) : 70.f;
                            CVector in{fv(-2000.f,2000.f,next()%16==0),fv(-2000.f,2000.f,false),fv(-2000.f,2000.f,false)}; CVector outV{std::bit_cast<float>(0x7F7F0001u),std::bit_cast<float>(0x7F7F0002u),std::bit_cast<float>(0x7F7F0003u)};
                            float sx=std::bit_cast<float>(0x7F7F0004u), sy=std::bit_cast<float>(0x7F7F0005u);
                            if(nm=="CalcScreenCoors-VV")ret=reinterpret_cast<bool(__cdecl*)(const CVector&,CVector&)>(a)(in,outV);
                            else ret=reinterpret_cast<bool(__cdecl*)(const CVector&,CVector&,float&,float&)>(a)(in,outV,sx,sy);
                            out["out"]={F(outV.x),F(outV.y),F(outV.z)}; out["scale"]={F(sx),F(sy)};
                            std::memcpy(viewM,savedView.data(),0x40); CDraw::ms_fFOV=savedFov; }
                        else if(nm=="DoesInfiniteLineTouchScreen"){
                            auto coord=[&](float lim){ switch(next()%6){ case 0: return 0.f; case 1: return lim; case 2: return uni(-500.f,lim+500.f); default: return uni(-3000.f,lim+3000.f); } };
                            const float ox=coord(W), oy=coord(H); float dx=fv(-10.f,10.f,false), dy=fv(-10.f,10.f,false);
                            ret=reinterpret_cast<bool(__cdecl*)(float,float,float,float)>(a)(ox,oy,dx,dy); }
                        else if(nm=="IsPointInsideLine"){ float v[7]; for(int k=0;k<6;++k)v[k]=fv(-3000.f,3000.f,next()%8==0); v[6]=fv(-500.f,500.f,next()%8==0);
                            ret=reinterpret_cast<bool(__cdecl*)(float,float,float,float,float,float,float)>(a)(v[0],v[1],v[2],v[3],v[4],v[5],v[6]); }
                        else if(nm=="LittleTest"){ im3d=int32(next()%3 ? next()%1000 : next()); reinterpret_cast<void(__cdecl*)()>(a)(); out["count"]=im3d; im3d=savedIm3d; }
                        else { alignas(16) std::array<uint8,0x40> raster{}; for(auto& b:raster)b=uint8(next());
                            *reinterpret_cast<int32*>(raster.data()+0xC)=int32(1+next()%2048); *reinterpret_cast<int32*>(raster.data()+0x10)=int32(1+next()%2048); *reinterpret_cast<int32*>(raster.data()+0x14)=int32(8*(1+next()%4));
                            imageMask=next()%4 ? 3 : next(); for(auto& b:fakeImage)b=uint8(next());
                            static const char* files[]{"snapshot.png","C:\\temp\\shot1.png","x.png","gallery\\gallery9.png"};
                            reinterpret_cast<void(__cdecl*)(void*,const char*)>(a)(raster.data(),files[next()%4]);
                            out["raster"]=ReversingProbe::Bytes(raster.data(),raster.size()); out["image"]=Fnv(fakeImage.data(),fakeImage.size()); }
                        RsGlobal.maximumWidth=savedW; RsGlobal.maximumHeight=savedH;
                    }
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                restoreAll();
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
    restoreAll();
    const bool restored=std::memcmp(clockBlock,savedClock.data(),0x1C)==0&&timeMs==savedTime&&std::memcmp(cheats,savedCheats.data(),0x60)==0&&CClock::gbFreezeTime==savedFreeze
        &&(!cdTouched||std::memcmp(cdRegion,savedCd.data(),CD_SIZE)==0)&&std::memcmp(viewM,savedView.data(),0x40)==0&&RsGlobal.maximumWidth==savedW&&RsGlobal.maximumHeight==savedH
        &&CDraw::ms_fFOV==savedFov&&im3d==savedIm3d;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
