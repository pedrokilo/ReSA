#pragma once
// Banco dentro del motor: gestor de sonidos e hilo de streaming de audio (lista de 14).
// 28 funciones de CAESoundManager y CAEStreamThread. Original (hooks del lote desactivados) frente a candidato con
// fixtures deterministas. Nada toca el AESoundManager real, el hilo de streaming real, AEAudioHardware ni ficheros:
// - El hilo de streaming de audio (AEAudioHardware.m_pStreamThread) se aparca durante todo el banco dentro de
//   IsForegroundApp (0x746060 + símbolo de la DLL), al principio de su bucle. En el hilo principal ese mismo falso
//   devuelve una secuencia guionizada solo durante MainLoop. Si no se aparca en 3 s el banco no corre.
// - CAESoundManager trabaja sobre un gestor falso (con relleno delante) con 300 CAESound falsas, entidades y audio
//   entities falsas (vtable propia) y listas física/posición/prioridad propias. Se simulan las funciones de
//   AEAudioHardware que usa, CAEAudioEnvironment::GetDopplerRelativeFrequency/GetPositionRelativeToCamera,
//   CAESound::SetPosition/CalculateVolume/NewVPSLEntry/operator= (llamadas en el EXE), CEntity::Register/
//   CleanUpOldReference y printf del EXE. Lo que el EXE mete en línea (StopSound, StopSoundAndForget,
//   UnregisterWithPhysicalEntity, SoundHasFinished, UpdateParameters, SetFlags, frecuencia con Doppler, factor de
//   cámara lenta, posición relativa) corre de verdad en la DLL. CTimer (pausas, tiempos, escala) se fija por caso
//   y se restaura tras cada ejecución.
// - CAEStreamThread trabaja sobre un hilo falso (handles falsos) y un canal de streaming falso (vtable propia y
//   simulación de UpdatePlayTime/GetPlayingTrackID/GetActiveTrackID/SetNextStream/SetReady/PrepareStream/Pause).
//   CreateThread, SetThreadPriority, Initialize/Enter/LeaveCriticalSection, Suspend/ResumeThread,
//   WaitForSingleObject y Sleep se sustituyen en la IAT del EXE y de la DLL solo alrededor de cada llamada y solo
//   para el hilo principal (los demás hilos van a la API real): nunca se crea un hilo. Se simulan LoadUserTrack,
//   GetDataStream, CAEDataStream::Initialise, CAEMP3TrackLoader::Initialise, CMemoryMgr::Free,
//   CAEAudioUtility::GetCurrentTimeInMS y (en MainLoop) CAEStreamThread::Service; los decoders Vorbis creados se
//   construyen de verdad y sus huecos Initialise/GetStreamLengthMs/SetCursor/destructor (vtables del EXE 0x862D70
//   y de la DLL) se sustituyen durante cada ejecución.
// - operator new/delete se sustituyen solo durante la llamada (registro sin reservas). Ninguna función probada puede
//   usar spdlog en esa ventana: con el backtrace activo guarda el mensaje (reserva y libera) aunque el nivel esté
//   apagado, y el arena acabaría liberado en el montón real.
// - Todos los falsos se ponen y se quitan en cada ejecución (después de cambiar el estado de los hooks), sin
//   direcciones repetidas y deshechos en orden inverso; nunca están puestos durante Write/PumpMessages. Un
//   vigilante deja EIP y pila del hilo principal en el log si una ejecución tarda más de 10 s.
#include <fstream>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Managers/AESoundManager.h"
#include "Audio/Managers/AEUserRadioTrackManager.h"
#include "Audio/AEStreamThread.h"
#include "Audio/AESound.h"
#include "Audio/AEAudioEnvironment.h"
#include "Audio/AEAudioUtility.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Hardware/AEStreamingChannel.h"
#include "Audio/Loaders/AEMP3TrackLoader.h"
#include "Audio/Loaders/AETrackLoader.h"
#include "Audio/Loaders/AEDataStream.h"
#include "Audio/Loaders/AEVorbisDecoder.h"
#include "MemoryMgr.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace Audio14Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint64 Hash64(const void* p,size_t n){ uint64 h=0x9E3779B97F4A7C15ull; auto* w=static_cast<const uint64*>(p); for(size_t i=0;i<n/8;++i){h^=w[i];h*=0x100000001B3ull;h^=h>>29;} return Fnv(static_cast<const uint8*>(p)+n/8*8,n%8,h); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline uint32 F(float f){ return std::bit_cast<uint32>(f); }
// Sigue los saltos de la ILT de la DLL, pero nunca entra en la imagen del EXE (un hook desactivado deja en la DLL un
// salto al EXE: entonces se parchea ese salto y no la entrada del EXE, que ya se parchea aparte).
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

// --- vigilante: si el hilo principal no avanza en 10 s, deja su EIP y su pila en el log (sin CRT ni montón) ---
inline volatile LONG progress{}, curFn{-1}, curCase{}, curRun{}; inline volatile bool watchStop{};
inline HANDLE mainThread{}; inline char logPath[MAX_PATH]{}; inline uintptr_t dllBase{};
inline void WatchdogReport(){
    CONTEXT ctx{}; ctx.ContextFlags=CONTEXT_FULL; uint32 st[48]{}; int nst=0;
    if(SuspendThread(mainThread)==DWORD(-1))return;
    const bool got=GetThreadContext(mainThread,&ctx)!=0;
    if(got)for(;nst<48;++nst){ MEMORY_BASIC_INFORMATION mi{}; const auto* sp=reinterpret_cast<const uint32*>(ctx.Esp)+nst;
        if(!VirtualQuery(sp,&mi,sizeof(mi))||mi.State!=MEM_COMMIT||(mi.Protect&(PAGE_NOACCESS|PAGE_GUARD)))break; st[nst]=*sp; }
    ResumeThread(mainThread);
    char buf[2048]; int n=_snprintf_s(buf,sizeof(buf),_TRUNCATE,"{\"event\":\"watchdog\",\"fn\":%ld,\"case\":%ld,\"run\":%ld,\"dll_base\":%u,\"eip\":%u,\"esp\":%u,\"ebp\":%u,\"stack\":[",
        long(curFn),long(curCase),long(curRun),uint32(dllBase),uint32(ctx.Eip),uint32(ctx.Esp),uint32(ctx.Ebp));
    for(int k=0;k<nst&&n>0&&n<int(sizeof(buf))-16;++k)n+=_snprintf_s(buf+n,sizeof(buf)-n,_TRUNCATE,k ? ",%u" : "%u",st[k]);
    if(n>0&&n<int(sizeof(buf))-4){ buf[n++]=']'; buf[n++]='}'; buf[n++]='\n'; }
    const HANDLE h=CreateFileA(logPath,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(h!=INVALID_HANDLE_VALUE){ DWORD w{}; WriteFile(h,buf,DWORD(n>0 ? n : 0),&w,nullptr); CloseHandle(h); } }
inline DWORD WINAPI Watchdog(LPVOID){
    LONG last=-1; DWORD since=GetTickCount(); bool reported=false;
    while(!watchStop){ Sleep(250); const LONG p=progress;
        if(p!=last){ last=p; since=GetTickCount(); reported=false; continue; }
        if(!reported&&GetTickCount()-since>10000){ reported=true; WatchdogReport(); } }
    return 0; }

// --- registro sin reservas (los falsos nunca crean json) ---
struct Rec { uint32 t, a, b, c, d; };
inline std::array<Rec,8192> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec]={t,a,b,c,d}; ++nrec; }
inline json Recs(){
    const size_t n=std::min(nrec,recs.size());
    auto one=[&](size_t i){ const auto& r=recs[i]; return json::array({r.t,r.a,r.b,r.c,r.d}); };
    if(nrec<=96){ json r=json::array(); for(size_t i=0;i<n;++i)r.push_back(one(i)); return r; }
    json head=json::array(); for(size_t i=0;i<32;++i)head.push_back(one(i));
    return {{"count",nrec},{"hash",Hash64(recs.data(),n*sizeof(Rec))},{"head",head}}; }

// --- objetos falsos ---
constexpr size_t MGR_PRE=0x200, MGR_SIZE=0x8CBC, LPAD=16;
alignas(16) inline std::array<uint8,MGR_PRE+MGR_SIZE+0x100> mgrBuf{};
inline uint8* Mgr(){ return mgrBuf.data()+MGR_PRE; }
inline uint8* Snd(int i){ return Mgr()+4+0x74*i; }
alignas(16) inline std::array<int16,LPAD+300+LPAD> physArr{}, posArr{}, prioArr{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> ents{};
alignas(16) inline std::array<std::array<uint8,0x48>,2> mats{};
alignas(16) inline std::array<std::array<uint8,0x10>,3> aes{};
inline std::array<void*,4> aeVt{};
alignas(16) inline std::array<uint8,0x80> psound{};
alignas(16) inline std::array<uint8,0x60> tobj{};
alignas(16) inline std::array<uint8,0x80> chan{};
inline std::array<void*,9> chanVt{};
alignas(16) inline std::array<uint8,0x20> loader{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> dsBuf{};
alignas(16) inline std::array<std::array<uint8,0x28>,2> userDec{};
inline std::array<void*,8> userDecVt{};
alignas(16) inline std::array<uint8,0x20> dvd{};
alignas(16) inline std::array<uint8,0x1000> arena{}; inline size_t arenaTop{}; inline uint32 newCount{};
inline std::array<uint32,16> allocOff{}, allocSize{}; inline uint32 nAlloc{};

inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    auto in=[&](const void* b,size_t n){ return u>=uintptr_t(b)&&u<uintptr_t(b)+n; };
    auto off=[&](const void* b){ return uint32(u-uintptr_t(b)); };
    if(in(mgrBuf.data(),mgrBuf.size()))return 0x10000000|off(mgrBuf.data());
    if(in(arena.data(),arena.size()))return 0x20000000|off(arena.data());
    if(in(physArr.data(),sizeof(physArr)))return 0x30000000|off(physArr.data());
    if(in(posArr.data(),sizeof(posArr)))return 0x31000000|off(posArr.data());
    if(in(prioArr.data(),sizeof(prioArr)))return 0x32000000|off(prioArr.data());
    for(uint32 k=0;k<ents.size();++k)if(in(ents[k].data(),ents[k].size()))return 0x40000000|(k<<8)|off(ents[k].data());
    for(uint32 k=0;k<aes.size();++k)if(in(aes[k].data(),aes[k].size()))return 0x41000000|(k<<8)|off(aes[k].data());
    if(in(psound.data(),psound.size()))return 0x42000000|off(psound.data());
    if(in(tobj.data(),tobj.size()))return 0x50000000|off(tobj.data());
    if(in(chan.data(),chan.size()))return 0x51000000|off(chan.data());
    if(in(loader.data(),loader.size()))return 0x52000000|off(loader.data());
    for(uint32 k=0;k<dsBuf.size();++k)if(in(dsBuf[k].data(),dsBuf[k].size()))return 0x53000000|(k<<8)|off(dsBuf[k].data());
    for(uint32 k=0;k<userDec.size();++k)if(in(userDec[k].data(),userDec[k].size()))return 0x54000000|(k<<8)|off(userDec[k].data());
    if(in(dvd.data(),dvd.size()))return 0x55000000|off(dvd.data());
    if(u==0xB5F8B8)return 0x60000001; if(u==0xB6B970)return 0x60000002;
    return 0xEEEEEEEEu; }

inline void* __cdecl FakeNew(size_t n){
    Push(1,uint32(n),newCount++);
    if(arenaTop+n>arena.size())return nullptr;
    void* p=arena.data()+arenaTop; if(nAlloc<allocOff.size()){ allocOff[nAlloc]=uint32(arenaTop); allocSize[nAlloc]=uint32(n); ++nAlloc; }
    arenaTop=(arenaTop+n+15)&~size_t(15); return p; }
inline void __cdecl FakeDelete(void* p){ Push(2,P(p)); }
inline void ResetArena(){ arena.fill(0xCD); arenaTop=0; newCount=0; nAlloc=0; }
inline int __cdecl FakePrintf(const char*,...){ return 0; } // printf del EXE (stdout sin consola): sin traza

// --- copia de regiones globales ---
struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }
inline void Restore(const Saved& s){ std::memcpy(reinterpret_cast<void*>(s.at),s.bytes.data(),s.bytes.size()); }

// --- puntero (hueco de vtable o de IAT) sustituido mientras vive el objeto ---
inline void SetSlot(uintptr_t* at,uintptr_t v){ DWORD o{}; VirtualProtect(at,4,PAGE_READWRITE,&o); *at=v; VirtualProtect(at,4,o,&o); }
struct PtrSlot {
    uintptr_t* at; uintptr_t saved{};
    PtrSlot(uintptr_t a,uintptr_t v) : at(reinterpret_cast<uintptr_t*>(a)) { saved=*at; SetSlot(at,v); }
    ~PtrSlot(){ SetSlot(at,saved); }
};
// Parches de una sola ejecución: sin direcciones repetidas y deshechos en orden inverso.
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> P; std::vector<std::unique_ptr<PtrSlot>> S; std::vector<uintptr_t> seen;
    template<class F> void Add(uintptr_t at,F fake){ if(std::ranges::find(seen,at)!=seen.end())return; seen.push_back(at); P.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,fake)); }
    void Ptr(uintptr_t at,const void* v){ if(std::ranges::find(seen,at)!=seen.end())return; seen.push_back(at); S.push_back(std::make_unique<PtrSlot>(at,uintptr_t(v))); }
    ~Patches(){ while(!S.empty())S.pop_back(); while(!P.empty())P.pop_back(); }
};
// operator new/delete del EXE (la DLL los reenvía aquí) solo durante la llamada
struct NewScope {
    ReversingProbe::EntryPatch n1{0x82119A,FakeNew}, n2{0x821195,FakeNew}, d1{0x8214BD,FakeDelete}, d2{0x8213AE,FakeDelete};
};

// --- hilo de streaming aparcado en IsForegroundApp; en el hilo principal, secuencia guionizada (MainLoop) ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline volatile bool fgScripted{}; inline uint32 fgBits{}, fgIdx{};
inline bool __cdecl FakeForeground(){
    const DWORD tid=GetCurrentThreadId();
    if(streamTid&&tid==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    else if(fgScripted&&tid==mainTid){ const uint32 k=fgIdx++; Push(40,k); return ((fgBits>>(k%32))&1)!=0; }
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// ======================= Win32 por IAT (EXE y DLL, solo el hilo principal) =======================
enum ApiId { A_CREATE, A_PRIO, A_INITCS, A_ENTER, A_LEAVE, A_SUSPEND, A_RESUME, A_WAIT, A_SLEEP, NAPI };
struct Api { const char* name; uintptr_t exeSlot; uintptr_t fake{}; uintptr_t real{}; std::vector<uintptr_t*> slots; int exeCount{}, dllCount{}; bool exeSlotOk{}; };
inline std::array<Api,NAPI> apis{{ {"CreateThread",0x8580B4}, {"SetThreadPriority",0x8580A8}, {"InitializeCriticalSection",0x8580F0},
    {"EnterCriticalSection",0x8580EC}, {"LeaveCriticalSection",0x8580E8}, {"SuspendThread",0x8580E4}, {"ResumeThread",0x8580A4},
    {"WaitForSingleObject",0x858088}, {"Sleep",0x8580C4} }};
template<class T> inline T Real(ApiId i){ return reinterpret_cast<T>(apis[i].real); }
inline HANDLE createRet{}; inline DWORD threadIdOut{}; inline uint32 sleepCount{}, sleepLimit{}; inline uintptr_t dllMainLoop[2]{};
inline uint32 StartTag(const void* s){ const auto u=uintptr_t(s); return (u==0x4F15C0||u==dllMainLoop[0]||u==dllMainLoop[1]) ? 1u : u; }
inline HANDLE WINAPI FakeCreateThread(LPSECURITY_ATTRIBUTES sa,SIZE_T st,LPTHREAD_START_ROUTINE start,LPVOID prm,DWORD fl,LPDWORD id){
    if(!Main())return Real<decltype(&CreateThread)>(A_CREATE)(sa,st,start,prm,fl,id);
    Push(70,uint32(uintptr_t(sa)),uint32(st),StartTag(reinterpret_cast<const void*>(start)),P(prm)); Push(71,fl,P(id));
    if(id)*id=threadIdOut; return createRet; }
inline BOOL WINAPI FakeSetThreadPriority(HANDLE h,int p){
    if(!Main())return Real<decltype(&SetThreadPriority)>(A_PRIO)(h,p);
    Push(72,uint32(uintptr_t(h)),uint32(p)); return TRUE; }
inline void WINAPI FakeInitCS(LPCRITICAL_SECTION cs){
    if(!Main()){ Real<decltype(&InitializeCriticalSection)>(A_INITCS)(cs); return; }
    Push(73,P(cs)); }
inline void WINAPI FakeEnterCS(LPCRITICAL_SECTION cs){
    if(!Main()){ Real<decltype(&EnterCriticalSection)>(A_ENTER)(cs); return; }
    Push(74,P(cs)); }
inline void WINAPI FakeLeaveCS(LPCRITICAL_SECTION cs){
    if(!Main()){ Real<decltype(&LeaveCriticalSection)>(A_LEAVE)(cs); return; }
    Push(75,P(cs)); }
inline DWORD WINAPI FakeSuspend(HANDLE h){
    if(!Main())return Real<decltype(&SuspendThread)>(A_SUSPEND)(h);
    Push(76,uint32(uintptr_t(h))); return 0; }
inline DWORD WINAPI FakeResume(HANDLE h){
    if(!Main())return Real<decltype(&ResumeThread)>(A_RESUME)(h);
    Push(77,uint32(uintptr_t(h))); return 1; }
inline DWORD WINAPI FakeWait(HANDLE h,DWORD ms){
    if(!Main())return Real<decltype(&WaitForSingleObject)>(A_WAIT)(h,ms);
    Push(78,uint32(uintptr_t(h)),ms); return WAIT_OBJECT_0; }
inline void WINAPI FakeSleep(DWORD ms){
    if(!Main()){ Real<decltype(&Sleep)>(A_SLEEP)(ms); return; }
    Push(41,ms); if(++sleepCount>=sleepLimit)tobj[0x11]=0; }
inline HMODULE SelfModule(){ HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&SelfModule),&m); return m; }
inline void FindApiSlots(){
    const void* fakes[NAPI]{reinterpret_cast<const void*>(&FakeCreateThread),reinterpret_cast<const void*>(&FakeSetThreadPriority),reinterpret_cast<const void*>(&FakeInitCS),
        reinterpret_cast<const void*>(&FakeEnterCS),reinterpret_cast<const void*>(&FakeLeaveCS),reinterpret_cast<const void*>(&FakeSuspend),
        reinterpret_cast<const void*>(&FakeResume),reinterpret_cast<const void*>(&FakeWait),reinterpret_cast<const void*>(&FakeSleep)};
    const HMODULE k32=GetModuleHandleA("kernel32.dll"), kb=GetModuleHandleA("kernelbase.dll");
    for(int i=0;i<NAPI;++i){ auto& a=apis[i]; a.fake=uintptr_t(fakes[i]); a.slots.clear(); a.exeCount=a.dllCount=0;
        a.real=k32 ? uintptr_t(GetProcAddress(k32,a.name)) : 0; const uintptr_t kbv=kb ? uintptr_t(GetProcAddress(kb,a.name)) : 0;
        if(!a.real)continue;
        a.exeSlotOk=*reinterpret_cast<uintptr_t*>(a.exeSlot)==a.real||(kbv&&*reinterpret_cast<uintptr_t*>(a.exeSlot)==kbv);
        const HMODULE mods[2]{GetModuleHandleA(nullptr),SelfModule()};
        for(int mi=0;mi<2;++mi){
            auto* base=reinterpret_cast<uint8*>(mods[mi]); auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
            const auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if(!dir.VirtualAddress)continue;
            for(auto* d=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);d->Name;++d)
                for(auto* t=reinterpret_cast<uintptr_t*>(base+d->FirstThunk);*t;++t)
                    if(*t==a.real||(kbv&&*t==kbv)){ if(std::ranges::find(a.slots,t)==a.slots.end()){ a.slots.push_back(t); ++(mi ? a.dllCount : a.exeCount); } }
        } } }
inline bool ApisReady(){ for(auto& a:apis)if(!a.real||!a.exeSlotOk||!a.exeCount||!a.dllCount)return false; return true; }
// Huecos de la IAT con los falsos solo alrededor de la llamada (sin reservas)
struct ApiScope {
    std::array<std::pair<uintptr_t*,uintptr_t>,128> saved{}; size_t n{};
    ApiScope(){ for(auto& a:apis)for(auto* s:a.slots)if(n<saved.size()){ saved[n++]={s,*s}; SetSlot(s,a.fake); } }
    ~ApiScope(){ while(n){ --n; SetSlot(saved[n].first,saved[n].second); } }
};

// ======================= CAESoundManager: AEAudioHardware, entorno, CAESound y CEntity falsos =======================
inline uint16 hwCount{}, hwNumAvail{}; inline int16 hwAllocRet{};
alignas(16) inline std::array<int16,300> hwPos{}, hwLen{}, hwLoop{};
inline bool Hw(const void* s){ return uintptr_t(s)==0xB5F8B8; }
inline uint32 __fastcall FakeNumAvail(void* s,void*){ Push(10,Hw(s)); return 0xBEEF0000u|hwNumAvail; }
inline uint32 __fastcall FakeAllocate(void* s,void*,uint32 n){ Push(11,Hw(s),n&0xFFFF); return 0xCAFE0000u|uint16(hwAllocRet); }
inline void __fastcall FakeRequestInfo(void* s,void*,uint32 vch,uint32 sfx,uint32 slot){ Push(12,Hw(s),vch&0xFFFF,sfx&0xFFFF,slot&0xFFFF); }
inline void __fastcall FakeChannelPlayTimes(void* s,void*,uint32 ch,int16* out){ Push(13,Hw(s),ch&0xFFFF,P(out)); if(out)std::memcpy(out,hwPos.data(),size_t(std::min<uint16>(hwCount,300))*2); }
inline void __fastcall FakeSoundLengths(void* s,void*,int16* out){ Push(14,Hw(s),P(out)); std::memcpy(out,hwLen.data(),600); }
inline void __fastcall FakeLoopStarts(void* s,void*,int16* out){ Push(15,Hw(s),P(out)); std::memcpy(out,hwLoop.data(),600); }
inline void __fastcall FakeStopSound(void* s,void*,uint32 ch,uint32 slot){ Push(16,Hw(s),(ch&0xFFFF)|(slot<<16)); }
inline void __fastcall FakePlaySound(void* s,void*,uint32 ch,uint32 slot,uint32 sfx,uint32 bank,uint32 pos,uint32 fl,float speed){
    Push(17,(ch&0xFFFF)|(slot<<16),(sfx&0xFFFF)|(bank<<16),(pos&0xFFFF)|(fl<<16),F(speed)^(Hw(s) ? 0u : 0x80000000u)); }
inline void __fastcall FakeSetVolume(void* s,void*,uint32 ch,uint32 slot,float vol,uint32 unused){ Push(18,(ch&0xFFFF)|(slot<<16),F(vol),(unused&0xFF)|(Hw(s)<<8)); }
inline void __fastcall FakeSetPos(void* s,void*,uint32 ch,uint32 slot,const float* v,uint32 unused){ Push(19,(ch&0xFFFF)|(slot<<16),F(v[0]),F(v[1]),F(v[2])^((unused&0xFF)<<24)^(Hw(s) ? 0u : 1u)); }
inline void __fastcall FakeSetFreq(void* s,void*,uint32 ch,uint32 slot,float f){ Push(20,(ch&0xFFFF)|(slot<<16),F(f),Hw(s)); }
inline void __fastcall FakeHwService(void* s,void*){ Push(21,Hw(s)); }
inline float __cdecl FakeDoppler(float prevDist,float curDist,uint32 prevT,uint32 curT,float scale){
    Push(22,F(prevDist),F(curDist),prevT*31u^curT,F(scale)); return 1.0f+float((prevT^curT)&0xFF)/256.0f; }
inline void __cdecl FakeRelPos(float* out,const float* in){ Push(23,P(in)); out[0]=in[0]*0.5f+1.0f; out[1]=in[1]-2.0f; out[2]=in[2]*2.0f; }
inline void __fastcall FakeSetPosition(void* s,void*,float x,float y,float z){ Push(24,P(s),F(x),F(y),F(z)); }
inline void __fastcall FakeCalcVolume(void* s,void*){ Push(25,P(s)); At<float>(s,0x60)=At<float>(s,0x14)-At<float>(s,0x6C); }
inline void __fastcall FakeNewVPSL(void* s,void*){ Push(26,P(s)); At<uint16>(s,0x58)=1; At<int16>(s,0x5E)=0; }
inline void* __fastcall FakeAssign(void* d,void*,const void* src){ Push(27,P(d),P(src)); std::memcpy(d,src,0x74); return d; }
inline void __fastcall FakeCleanUp(void* e,void*,void** ref){ Push(28,P(e),P(ref)); }
inline void __fastcall FakeRegister(void* e,void*,void** ref){ Push(29,P(e),P(ref)); }
inline void __fastcall FakeAeUpdate(void* ae,void*,void* snd,uint32 pos){ Push(30,P(ae),P(snd),pos&0xFFFF); }
inline void __fastcall FakeBadVirtual(void* s,void*){ Push(31,P(s)); }

// ======================= CAEStreamThread: canal, cargadores y decoders falsos =======================
inline int32 chPlayTime{}, chLength{}, chPlaying{}, chActive{}, chUpdate{};
inline uint32 ch0(void* s){ return P(s); }
inline void* __fastcall ChDtor(void* s,void*,uint32 f){ Push(52,ch0(s),f); return s; }
inline void __fastcall ChService(void* s,void*){ Push(44,ch0(s)); }
inline bool __fastcall ChIsPlaying(void* s,void*){ Push(45,ch0(s)); return false; }
inline int32 __fastcall ChGetPlayTime(void* s,void*){ Push(46,ch0(s)); return chPlayTime; }
inline int32 __fastcall ChGetLength(void* s,void*){ Push(47,ch0(s)); return chLength; }
inline void __fastcall ChPlay(void* s,void*,uint32 off,uint32 fl,float f){ Push(48,ch0(s),off&0xFFFF,fl&0xFF,F(f)); }
inline void __fastcall ChSynch(void* s,void*){ Push(49,ch0(s)); }
inline void __fastcall ChStop(void* s,void*){ Push(50,ch0(s),tobj[0x16]); }
inline void __fastcall ChSetFreq(void* s,void*,float f){ Push(51,ch0(s),F(f)); }
inline int32 __fastcall ChUpdatePlayTime(void* s,void*){ Push(53,ch0(s)); return chUpdate; }
inline int32 __fastcall ChGetPlaying(void* s,void*){ Push(54,ch0(s)); return chPlaying; }
inline int32 __fastcall ChGetActive(void* s,void*){ Push(55,ch0(s)); return chActive; }
inline uint32 dllVorbisVt{}, dllBaseVt{}; inline std::array<uint32,4> dllCb{};
inline void __fastcall ChSetNext(void* s,void*,void* d){ Push(56,ch0(s),P(d)); }
inline void __fastcall ChSetReady(void* s,void*){ Push(57,ch0(s)); }
inline void __fastcall ChPrepare(void* s,void*,void* d,uint32 fl,uint32 bytes){ Push(58,ch0(s),P(d),fl&0xFF,bytes); }
inline void __fastcall ChPause(void* s,void*){ Push(59,ch0(s)); }
inline uint32 dsMask{}, decInitMask{}, decInitIdx{}, userMask{}; inline std::array<int32,4> decLens{}; inline uint32 decLenIdx{}, dsIdx{}, userIdx{};
inline void* __fastcall FakeGetDataStream(void* s,void*,uint32 id){ Push(60,P(s),id); const uint32 k=dsIdx++; return (dsMask>>(k%32))&1 ? static_cast<void*>(dsBuf[k%2].data()) : nullptr; }
inline bool __fastcall FakeDsInit(void* s,void*){ Push(61,P(s)); return true; }
inline void* __fastcall FakeLoadUser(void* s,void*,int32 id){ Push(62,P(s),uint32(id)); const uint32 k=userIdx++; return (userMask>>(k%32))&1 ? static_cast<void*>(userDec[k%2].data()) : nullptr; }
inline bool __fastcall DecInit(void* s,void*){ const uint32 k=decInitIdx++; Push(63,P(s),k); return ((decInitMask>>(k%32))&1)!=0; }
inline int32 __fastcall DecLen(void* s,void*){ const uint32 k=decLenIdx++; Push(64,P(s),k); return decLens[k%4]; }
inline void __fastcall DecSetCursor(void* s,void*,uint32 pos){ Push(65,P(s),pos); }
inline void* __fastcall DecDelete(void* s,void*,uint32 f){ Push(66,P(s),f); return s; }
inline void __fastcall DecBad(void* s,void*){ Push(67,P(s)); }
inline bool mp3InitRet{};
inline bool __fastcall FakeMp3Init(void* s,void*){ Push(68,P(s)); return mp3InitRet; }
inline void __cdecl FakeFree(void* p){ Push(69,P(p)); }
inline void __fastcall FakeThreadService(void* s,void*){ Push(43,P(s)); }
inline std::array<uint64,32> times{}; inline uint32 timeIdx{};
// Otros hilos (fundidos, canales) también piden la hora mientras está puesto: reciben la real (QPC / frecuencia
// 0xB610F0 · 1000 menos el inicio 0xB610F8) y no dejan registro.
inline uint64 __cdecl FakeTime(){
    if(!Main()){ LARGE_INTEGER c{}; QueryPerformanceCounter(&c); const auto fq=*reinterpret_cast<const volatile int64*>(0xB610F0);
        const double ms=fq ? double(c.QuadPart)/double(fq)*1000.0 : 0.0; return uint64(ms)-*reinterpret_cast<const volatile uint64*>(0xB610F8); }
    const uint32 k=timeIdx++; Push(42,k); return times[k%32]; }
// Decoders del arena: vtable y callbacks normalizados, punteros a etiquetas
inline void NormaliseDecoders(){
    static constexpr std::array<uint32,4> exeCb{0x502580,0x5025B0,0x5025D0,0x5025F0};
    for(uint32 k=0;k<nAlloc;++k){ if(allocSize[k]!=0x28)continue; auto* d=arena.data()+allocOff[k];
        uint32& vt=At<uint32>(d,0); vt=(vt==0x862D70||vt==dllVorbisVt) ? 1u : (vt==0x85F418||vt==dllBaseVt) ? 2u : vt;
        for(size_t c=0;c<4;++c){ uint32& cb=At<uint32>(d,8+4*c); if(cb==exeCb[c]||cb==dllCb[c])cb=uint32(c+1); }
        At<uint32>(d,4)=P(At<void*>(d,4)); At<uint32>(d,0x20)=P(At<void*>(d,0x20)); } }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=28;
inline const std::array<Fn,NFN> fns{{
    {"CAESoundManager","AreSoundsOfThisEventPlayingForThisEntity",0x4EF570,0,-1,512},
    {"CAESoundManager","AreSoundsOfThisEventPlayingForThisEntityAndPhysical",0x4EF5D0,0,-1,512},
    {"CAESoundManager","AreSoundsPlayingInBankSlot",0x4EF520,0,-1,512},{"CAESoundManager","CancelSoundsInBankSlot",0x4EFC60,0,-1,512},
    {"CAESoundManager","CancelSoundsOfThisEventPlayingForThisEntity",0x4EFB90,0,-1,512},
    {"CAESoundManager","CancelSoundsOfThisEventPlayingForThisEntityAndPhysical",0x4EFBF0,0,-1,512},
    {"CAESoundManager","CancelSoundsOwnedByAudioEntity",0x4EFCD0,0,-1,512},{"CAESoundManager","GetVirtualChannelForPhysicalChannel",0x4EF630,0,-1,1024},
    {"CAESoundManager","Initialise",0x5B9690,0,-1,256},{"CAESoundManager","PauseManually",0x4EF510,0,-1,128},
    {"CAESoundManager","RequestNewSound",0x4EFB10,0,-1,512},{"CAESoundManager","Reset",0x4EF4D0,0,-1,256},
    {"CAESoundManager","Service",0x4F0000,0,-1,1024},{"CAESoundManager","Terminate",0x4EFAA0,0,-1,128},
    {"CAEStreamThread","GetActiveTrackID",0x4F1560,0,-1,64},{"CAEStreamThread","GetPlayingTrackID",0x4F1570,0,-1,64},
    {"CAEStreamThread","GetTrackLengthMs",0x4F1550,0,-1,64},{"CAEStreamThread","GetTrackPlayTime",0x4F1530,0,-1,256},
    {"CAEStreamThread","Initialise",0x4F1680,0,-1,256},{"CAEStreamThread","MainLoop",0x4F15C0,0,-1,512},
    {"CAEStreamThread","Pause",0x4F1200,0,-1,64},{"CAEStreamThread","PlayTrack",0x4F1230,0,-1,512},
    {"CAEStreamThread","Resume",0x4F1210,0,-1,64},{"CAEStreamThread","Service",0x4F1290,0,-1,1024},
    {"CAEStreamThread","Start",0x4F11F0,0,-1,64},{"CAEStreamThread","Stop",0x4F1590,0,-1,256},
    {"CAEStreamThread","StopTrack",0x4F1580,0,-1,64},{"CAEStreamThread","WaitForExit",0x4F1220,0,-1,64},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"audio14-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"audio14-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio14");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"audio14-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // vtable y callbacks del decoder Vorbis de la DLL (se construye y destruye uno sin stream ni handle)
    { alignas(16) std::array<uint8,0x40> tmp{}; auto* d=new(tmp.data()) CAEVorbisDecoder(nullptr,0); dllVorbisVt=At<uint32>(tmp.data(),0);
      for(size_t k=0;k<4;++k)dllCb[k]=At<uint32>(tmp.data(),8+4*k); d->~CAEVorbisDecoder(); dllBaseVt=At<uint32>(tmp.data(),0); }
    dllMainLoop[0]=std::bit_cast<uintptr_t>(&CAEStreamThread::MainLoop); dllMainLoop[1]=Body(dllMainLoop[0]);
    // vtables falsas
    aeVt={reinterpret_cast<void*>(&FakeAeUpdate),reinterpret_cast<void*>(&FakeBadVirtual),reinterpret_cast<void*>(&FakeBadVirtual),reinterpret_cast<void*>(&FakeBadVirtual)};
    chanVt={reinterpret_cast<void*>(&ChDtor),reinterpret_cast<void*>(&ChService),reinterpret_cast<void*>(&ChIsPlaying),reinterpret_cast<void*>(&ChGetPlayTime),
            reinterpret_cast<void*>(&ChGetLength),reinterpret_cast<void*>(&ChPlay),reinterpret_cast<void*>(&ChSynch),reinterpret_cast<void*>(&ChStop),reinterpret_cast<void*>(&ChSetFreq)};
    userDecVt={reinterpret_cast<void*>(&DecInit),reinterpret_cast<void*>(&DecBad),reinterpret_cast<void*>(&DecLen),reinterpret_cast<void*>(&DecBad),
               reinterpret_cast<void*>(&DecSetCursor),reinterpret_cast<void*>(&DecBad),reinterpret_cast<void*>(&DecDelete),reinterpret_cast<void*>(&DecBad)};
    FindApiSlots();
    { json a=json::array(); for(auto& x:apis)a.push_back({x.name,x.exeCount,x.dllCount,x.exeSlotOk}); Write({{"event","api_slots"},{"apis",a},{"ready",ApisReady()}}); }
    const bool apisReady=ApisReady();

    // estado global vigilado
    const uintptr_t dllForeground=Body(std::bit_cast<uintptr_t>(&IsForegroundApp));
    const Saved timerSaved=Save(0xB7CB40,0x50);
    const std::vector<Saved> savedAll{ Save(0x858088,0x70), Save(0x862D70,0x20), Save(dllVorbisVt,0x20), Save(0x746060,5), Save(dllForeground,5),
                                       Save(0xB62CB0,MGR_SIZE), Save(0xB5F8B8+0xE14,0x50), timerSaved };

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
        strncpy_s(logPath,(dirPath/"audio14-runtime.jsonl").string().c_str(),_TRUNCATE);
        { HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Watchdog),&m); dllBase=uintptr_t(m); }
        DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&mainThread,THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,0);
        watchStop=false; const HANDLE watch=CreateThread(nullptr,0,&Watchdog,nullptr,0,nullptr);
        for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            Write({{"event","function_start"},{"function",full},{"index",f}});
            const bool sm=cls=="CAESoundManager";
            if(!sm&&!apisReady){ Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","IAT slots not found"}}); passed=false; continue; }
            // Los falsos solo están puestos durante cada ejecución (nunca durante Write/PumpMessages).
            auto install=[&](Patches& pt){
            if(sm) {
                // Cada función solo con los falsos que puede alcanzar (las de consulta y Terminate no llaman a nada simulado)
                const bool svc=nm=="Service", req=nm=="RequestNewSound";
                if(nm=="Initialise"){
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::GetNumAvailableChannels)),FakeNumAvail); pt.Add(0x4D8810,FakeNumAvail);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::AllocateChannels)),FakeAllocate); pt.Add(0x5B9340,FakeAllocate);
                pt.Add(0x821982,FakePrintf);
                }
                if(req){
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::RequestVirtualChannelSoundInfo)),FakeRequestInfo); pt.Add(0x4D8E60,FakeRequestInfo);
                pt.Add(Body(bit_cast<uintptr_t>(&CAESound::NewVPSLEntry)),FakeNewVPSL); pt.Add(0x4EF7A0,FakeNewVPSL);
                pt.Add(Body(bit_cast<uintptr_t>(static_cast<CAESound&(CAESound::*)(const CAESound&)>(&CAESound::operator=))),FakeAssign); pt.Add(0x4EF680,FakeAssign);
                }
                if(svc||req||nm=="Reset"||nm.starts_with("Cancel")){
                pt.Add(Body(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference))),FakeCleanUp); pt.Add(0x571A00,FakeCleanUp);
                pt.Add(Body(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference))),FakeRegister); pt.Add(0x571B70,FakeRegister);
                }
                if(svc){
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::GetChannelPlayTimes)),FakeChannelPlayTimes); pt.Add(0x4D8820,FakeChannelPlayTimes);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::GetVirtualChannelSoundLengths)),FakeSoundLengths); pt.Add(0x4D8E90,FakeSoundLengths);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::GetVirtualChannelSoundLoopStartTimes)),FakeLoopStarts); pt.Add(0x4D8EB0,FakeLoopStarts);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::StopSound)),FakeStopSound); pt.Add(0x4D88E0,FakeStopSound);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::PlaySound)),FakePlaySound); pt.Add(0x4D86B0,FakePlaySound);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelVolume)),FakeSetVolume); pt.Add(0x4D8870,FakeSetVolume);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelPosition)),FakeSetPos); pt.Add(0x4D8920,FakeSetPos);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelFrequencyScalingFactor)),FakeSetFreq); pt.Add(0x4D8960,FakeSetFreq);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::Service)),FakeHwService); pt.Add(0x4D9870,FakeHwService);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetDopplerRelativeFrequency)),FakeDoppler); pt.Add(0x4D7E40,FakeDoppler);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetPositionRelativeToCamera_Vector)),FakeRelPos); pt.Add(0x4D80B0,FakeRelPos);
                pt.Add(Body(bit_cast<uintptr_t>(&CAESound::SetPosition)),FakeSetPosition); pt.Add(0x4EF880,FakeSetPosition);
                pt.Add(Body(bit_cast<uintptr_t>(&CAESound::CalculateVolume)),FakeCalcVolume); pt.Add(0x4EFA10,FakeCalcVolume);
                }
            } else {
                pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamingChannel::UpdatePlayTime)),ChUpdatePlayTime); pt.Add(0x4F18A0,ChUpdatePlayTime);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamingChannel::GetPlayingTrackID)),ChGetPlaying); pt.Add(0x4F1A60,ChGetPlaying);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamingChannel::GetActiveTrackID)),ChGetActive); pt.Add(0x4F1A40,ChGetActive);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamingChannel::SetNextStream)),ChSetNext); pt.Add(0x4F1DE0,ChSetNext);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamingChannel::SetReady)),ChSetReady); pt.Add(0x4F1FF0,ChSetReady);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamingChannel::PrepareStream)),ChPrepare); pt.Add(0x4F23D0,ChPrepare);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamingChannel::Pause)),ChPause); pt.Add(0x4F2170,ChPause);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::LoadUserTrack)),FakeLoadUser); pt.Add(0x4F35F0,FakeLoadUser);
                pt.Add(Body(bit_cast<uintptr_t>(&CAETrackLoader::GetDataStream)),FakeGetDataStream); pt.Add(0x4E0D20,FakeGetDataStream);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEDataStream::Initialise)),FakeDsInit); pt.Add(0x4DC2B0,FakeDsInit);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEMP3TrackLoader::Initialise)),FakeMp3Init); pt.Add(0x4E0C50,FakeMp3Init);
                if(nm=="Stop"){ pt.Add(Body(bit_cast<uintptr_t>(static_cast<void(*)(void*)>(&CMemoryMgr::Free))),FakeFree); pt.Add(0x72F430,FakeFree); } // solo donde se alcanza (otros hilos liberan memoria)
                if(nm=="MainLoop"){ pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioUtility::GetCurrentTimeInMS)),FakeTime); pt.Add(0x4D9E80,FakeTime);
                                    pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamThread::Service)),FakeThreadService); pt.Add(0x4F1290,FakeThreadService); }
                if(nm=="Service")for(uintptr_t vt:{uintptr_t(0x862D70),uintptr_t(dllVorbisVt)}){
                    pt.Ptr(vt+0x00,reinterpret_cast<const void*>(&DecInit)); pt.Ptr(vt+0x08,reinterpret_cast<const void*>(&DecLen));
                    pt.Ptr(vt+0x10,reinterpret_cast<const void*>(&DecSetCursor)); pt.Ptr(vt+0x18,reinterpret_cast<const void*>(&DecDelete)); }
            }
            };
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto fill=[&](void* p,size_t n){ auto* w=static_cast<uint8*>(p); for(size_t k=0;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(w+k,&v,4); } for(size_t k=n&~size_t(3);k<n;++k)w[k]=uint8(next()); };
                auto pickf=[&](std::initializer_list<float> l){ return *(l.begin()+next()%l.size()); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x14Du; if(!stt)stt=1; nrec=0;
                    curFn=LONG(f); curCase=i; curRun=candidate; InterlockedIncrement(&progress);
                    for(auto* h:hooks)h->State(candidate);
                    Patches pt; install(pt);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(sm) {
                        const float qnan=std::bit_cast<float>(0x7FC00000u);
                        auto* m=Mgr(); fill(mgrBuf.data(),mgrBuf.size());
                        fill(physArr.data(),sizeof(physArr)); fill(posArr.data(),sizeof(posArr)); fill(prioArr.data(),sizeof(prioArr));
                        for(auto& e:ents){ fill(e.data(),e.size()); for(size_t k=4;k<0x10;k+=4)At<float>(e.data(),k)=float(int(next()%2000)-1000)*0.25f; }
                        for(auto& mt:mats){ fill(mt.data(),mt.size()); for(size_t k=0x30;k<0x3C;k+=4)At<float>(mt.data(),k)=float(int(next()%2000)-1000)*0.5f; }
                        At<void*>(ents[0].data(),0x14)=mats[0].data(); At<void*>(ents[1].data(),0x14)=nullptr; At<void*>(ents[2].data(),0x14)=mats[1].data();
                        for(auto& e:aes){ fill(e.data(),e.size()); At<void*>(e.data(),0)=aeVt.data(); }
                        static const int32 evs[]{0,1,2,3,-1,0x8003,int32(0xFFFF8003u),0x10002,7};
                        auto aePick=[&]()->void*{ const uint32 r=next()%5; return r<3 ? static_cast<void*>(aes[r].data()) : nullptr; };
                        auto entPick=[&]()->void*{ const uint32 r=next()%5; return r<3 ? static_cast<void*>(ents[r].data()) : nullptr; };
                        auto fillSound=[&](uint8* s){
                            fill(s,0x74);
                            At<int16>(s,0)=next()%6 ? int16(next()%6) : int16(next()); At<int16>(s,2)=int16(next()%50);
                            At<void*>(s,4)=aePick(); At<void*>(s,8)=next()%3==0 ? entPick() : nullptr; At<int32>(s,0xC)=evs[next()%9];
                            At<float>(s,0x10)=float(next()%100); At<float>(s,0x14)=next()%40==0 ? qnan : pickf({-20.f,-10.f,0.f,5.f,-3.5f});
                            At<float>(s,0x18)=pickf({1.f,2.f,0.5f}); At<float>(s,0x1C)=pickf({1.f,0.5f,2.f,0.8f}); At<float>(s,0x20)=pickf({0.f,0.f,0.1f});
                            for(size_t k=0x24;k<0x3C;k+=4)At<float>(s,k)=float(int(next()%4000)-2000)*0.125f;
                            At<float>(s,0x48)=float(next()%500); At<float>(s,0x4C)=float(next()%500); At<float>(s,0x50)=pickf({1.f,0.5f,0.f});
                            { const uint32 r=next()%8; s[0x54]=r==0 ? uint8(1+next()%3) : r==1 ? uint8(0x80|next()) : uint8(0); }
                            At<uint16>(s,0x56)=uint16(next()&0x1FFF); At<uint16>(s,0x58)=uint16(next()%2); At<int16>(s,0x5A)=int16(next()%3 ? 1 : 0);
                            { const uint32 r=next()%6; At<int16>(s,0x5C)=r==0 ? int16(-1) : r==1 ? int16(next()) : int16(next()%3000); }
                            At<int16>(s,0x5E)=int16(next()%3==0); At<float>(s,0x60)=float(int(next()%60)-50);
                            At<float>(s,0x64)=pickf({1.f,0.5f,2.f,1.5f,0.f,-1.f,1.25f}); At<int16>(s,0x68)=int16(next()%4==0);
                            At<float>(s,0x6C)=pickf({0.f,5.f,2.5f}); At<int16>(s,0x70)=int16(next()); };
                        uint16 count; { const uint32 r=next()%10; count=r==0 ? uint16(0) : r==1 ? uint16(300) : r==2 ? uint16(1) : uint16(2+next()%48); }
                        At<uint16>(m,0)=count; At<int16>(m,2)=int16(next()%64);
                        for(int s=0;s<300;++s)fillSound(Snd(s));
                        for(int c=0;c<300;++c)physArr[LPAD+c]=next()%5<2 ? int16(-1) : int16(next()%300);
                        At<void*>(m,0x87F4)=physArr.data()+LPAD; At<void*>(m,0x87F8)=posArr.data()+LPAD; At<void*>(m,0x87FC)=prioArr.data()+LPAD;
                        m[0x8CB4]=uint8(next()%2); m[0x8CB5]=uint8(next()%3==0);
                        // CTimer de este caso (se restaura tras la ejecución)
                        const uint32 now=next(); At<uint32>(m,0x8CB0)=next()%3 ? now-next()%200 : next(); At<uint32>(m,0x8CB8)=next()%3 ? now-next()%400 : next();
                        *reinterpret_cast<uint8*>(0xB7CB49)=uint8(next()%6==0); *reinterpret_cast<uint8*>(0xB7CB48)=uint8(next()%6==0);
                        *reinterpret_cast<uint32*>(0xB7CB84)=now; *reinterpret_cast<uint32*>(0xB7CB7C)=next()%3 ? now+next()%300 : next();
                        *reinterpret_cast<float*>(0xB7CB64)=next()%4==0 ? 0.5f : 1.0f;
                        auto arg16=[&](int32 v){ return (next()<<16)|uint32(uint16(v)); };
                        auto argB=[&](){ return (next()<<8)|uint32(next()%2); };
                        auto evArg=[&](){ return arg16(evs[next()%9]); };
                        auto bias=[&](auto&& apply){ for(int k=0,n=int(next()%4);k<n;++k)apply(Snd(int(next()%300))); };
                        if(nm=="AreSoundsPlayingInBankSlot"||nm=="CancelSoundsInBankSlot"){
                            const int16 bank=next()%6 ? int16(next()%6) : int16(next());
                            bias([&](uint8* s){ At<int16>(s,0)=bank; At<uint16>(s,0x58)=1; if(next()%2)At<int16>(s,0x5E)=int16(next()%2); });
                            if(nm[0]=='A')ret=reinterpret_cast<uint32(__thiscall*)(void*,uint32)>(a)(m,arg16(bank))&0xFFFF;
                            else reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(a)(m,arg16(bank),argB()); }
                        else if(nm=="AreSoundsOfThisEventPlayingForThisEntity"||nm=="CancelSoundsOfThisEventPlayingForThisEntity"
                              ||nm=="AreSoundsOfThisEventPlayingForThisEntityAndPhysical"||nm=="CancelSoundsOfThisEventPlayingForThisEntityAndPhysical"){
                            const uint32 ev=evArg(); void* ae=next()%8 ? aePick() : static_cast<void*>(m); void* ph=entPick();
                            bias([&](uint8* s){ At<int32>(s,0xC)=next()%4 ? int32(int16(ev)) : int32(uint16(ev)); At<void*>(s,4)=ae; if(next()%3)At<void*>(s,8)=ph; At<uint16>(s,0x58)=uint16(next()%4!=0); });
                            if(nm=="AreSoundsOfThisEventPlayingForThisEntity")ret=reinterpret_cast<uint32(__thiscall*)(void*,uint32,void*)>(a)(m,ev,ae)&0xFFFF;
                            else if(nm=="CancelSoundsOfThisEventPlayingForThisEntity")reinterpret_cast<void(__thiscall*)(void*,uint32,void*)>(a)(m,ev,ae);
                            else if(nm[0]=='A')ret=reinterpret_cast<uint32(__thiscall*)(void*,uint32,void*,void*)>(a)(m,ev,ae,ph)&0xFFFF;
                            else reinterpret_cast<void(__thiscall*)(void*,uint32,void*,void*)>(a)(m,ev,ae,ph); }
                        else if(nm=="CancelSoundsOwnedByAudioEntity"){ void* ae=next()%8 ? aePick() : static_cast<void*>(m);
                            reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(a)(m,ae,argB()); }
                        else if(nm=="Reset")reinterpret_cast<void(__thiscall*)(void*)>(a)(m);
                        else if(nm=="PauseManually")reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(m,next());
                        else if(nm=="GetVirtualChannelForPhysicalChannel"){
                            static const int16 hs[]{0,1,5,32767,-32768,32000,-32000,-1};
                            const int16 h=next()%3 ? int16(next()%64) : hs[next()%8]; At<int16>(m,2)=h;
                            int32 d; const uint32 r=next()%4; d=r==0 ? int32(next()%(uint32(count)+10))-5 : r==1 ? int32(next()) : int32(next()%std::max<uint32>(count,1));
                            const int16 phys=int16(int32(h)+d);
                            ret=reinterpret_cast<uint32(__thiscall*)(void*,uint32)>(a)(m,arg16(phys))&0xFFFF; }
                        else if(nm=="RequestNewSound"){
                            const uint32 r=next()%8; const int freeAt=r==0 ? 300 : r==1 ? int(next()%300) : int(next()%20);
                            for(int s=0;s<300;++s)if(s<freeAt)At<uint16>(Snd(s),0x58)=1; else if(s==freeAt)At<uint16>(Snd(s),0x58)=0;
                            fillSound(psound.data()); At<void*>(psound.data(),8)=entPick();
                            ret=P(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(m,psound.data()));
                            out["psound"]=Hash64(psound.data(),0x74); }
                        else if(nm=="Initialise"){
                            static const uint16 av[]{0,5,10,11,12,64,299,300,301,0x7FFF,0x8000,0xFFFF};
                            hwNumAvail=next()%3 ? uint16(11+next()%60) : next()%2 ? av[next()%12] : uint16(next());
                            hwAllocRet=next()%6==0 ? int16(-1) : next()%2 ? int16(next()%64) : int16(next());
                            ResetArena();
                            { NewScope ns; ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(m)&0xFF; }
                            out["arena"]=Hash64(arena.data(),arena.size()); }
                        else if(nm=="Terminate"){
                            ResetArena(); fill(arena.data(),arena.size());
                            At<uint32>(arena.data(),0x10)=next()%2 ? uint32(count) : next()%300;
                            At<void*>(m,0x87F4)=next()%6 ? static_cast<void*>(arena.data()+0x14) : nullptr;
                            At<void*>(m,0x87F8)=next()%6 ? static_cast<void*>(arena.data()+0x400) : nullptr;
                            At<void*>(m,0x87FC)=next()%6 ? static_cast<void*>(arena.data()+0x800) : nullptr;
                            { NewScope ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(m); } }
                        else { // Service
                            hwCount=count;
                            for(int c=0;c<300;++c){ hwPos[c]=next()%4==0 ? int16(-1) : int16(next()%4000);
                                hwLen[c]=next()%10==0 ? int16(int(next()%50)-40) : int16(1+next()%5000);
                                hwLoop[c]=next()%3==0 ? int16(-1) : int16(next()%800);
                                if(hwLen[c]==0)hwLoop[c]=-1; }
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(m);
                            out["pos"]=Hash64(posArr.data(),sizeof(posArr)); out["prio"]=Hash64(prioArr.data(),sizeof(prioArr)); }
                        // punteros del gestor normalizados antes de comparar
                        for(size_t off:{size_t(0x87F4),size_t(0x87F8),size_t(0x87FC)})At<uint32>(m,off)=P(At<void*>(m,off));
                        json sh=json::array(); for(int k=0;k<300;k+=30)sh.push_back(Hash64(Snd(k),0x74*30));
                        out["head"]=ReversingProbe::Bytes(m,4); out["ptrs"]={At<uint32>(m,0x87F4),At<uint32>(m,0x87F8),At<uint32>(m,0x87FC)};
                        out["sounds"]=sh; out["tables"]=Hash64(m+0x8800,0x4B0); out["tail"]=ReversingProbe::Bytes(m+0x8CB0,0xC);
                        out["pre"]=Hash64(mgrBuf.data(),MGR_PRE); out["post"]=Hash64(m+MGR_SIZE,mgrBuf.size()-MGR_PRE-MGR_SIZE);
                        out["phys"]=Hash64(physArr.data(),sizeof(physArr));
                        out["ents"]={Hash64(ents.data(),sizeof(ents)),Hash64(aes.data(),sizeof(aes))};
                        Restore(timerSaved);
                    }
                    else { // CAEStreamThread
                        auto* o=tobj.data();
                        fill(o,tobj.size()); fill(chan.data(),chan.size()); fill(loader.data(),loader.size());
                        for(auto& d:dsBuf)fill(d.data(),d.size()); for(auto& d:userDec){ fill(d.data(),d.size()); At<void*>(d.data(),0)=userDecVt.data(); }
                        At<uint32>(o,0)=0x1000u+(next()&0xFFF)*4; At<void*>(o,8)=chan.data(); At<void*>(o,0xC)=loader.data();
                        for(size_t k=0x10;k<0x17;++k)o[k]=uint8(next()%2);
                        At<void*>(chan.data(),0)=chanVt.data();
                        loader[0]=uint8(next()%2); At<void*>(loader.data(),0x18)=next()%2 ? static_cast<void*>(dvd.data()) : nullptr;
                        { const uint32 r=next()%6; chPlayTime=r==0 ? -2 : r==1 ? int32(next()) : r==2 ? int32(200000+next()%100000) : int32(next()%30000); }
                        chLength=next()%3 ? int32(60000+next()%400000) : int32(next()); chPlaying=int32(next()%40); chActive=int32(next()%40); chUpdate=int32(next());
                        dsMask=next()|0x1; decInitMask=next(); userMask=next(); decInitIdx=decLenIdx=dsIdx=userIdx=0;
                        for(auto& l:decLens){ const uint32 r=next()%5; l=r==0 ? int32(1+next()%5) : r==1 ? -int32(1+next()%1000) : int32(1000+next()%400000); }
                        createRet=reinterpret_cast<HANDLE>(uintptr_t(0x6000u+(next()&0xFF)*4)); threadIdOut=0x5150u+uint32(i); mp3InitRet=next()%2;
                        ResetArena();
                        if(nm=="GetActiveTrackID"||nm=="GetPlayingTrackID"||nm=="GetTrackLengthMs")ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(o);
                        else if(nm=="GetTrackPlayTime"){ { static const uint8 pv[]{0,0,1,2}; o[0x15]=pv[next()%4]; }
                            ApiScope as; ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(o); }
                        else if(nm=="StopTrack")reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                        else if(nm=="Start"||nm=="Pause"||nm=="Resume"||nm=="WaitForExit"){ ApiScope as; reinterpret_cast<void(__thiscall*)(void*)>(a)(o); }
                        else if(nm=="Stop"){ if(next()%6==0)At<void*>(o,0xC)=nullptr;
                            { ApiScope as; NewScope ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(o); } out["loader"]=Hash64(loader.data(),loader.size()); }
                        else if(nm=="PlayTrack"){
                            const uint32 id=next()%3 ? next()%200 : next(); const int32 nx=next()%3==0 ? -1 : int32(next()%3 ? next()%200 : next());
                            const uint32 a3=next(), a4=next(), u=(next()<<8)|(next()%2), nu=(next()<<8)|(next()%2);
                            ApiScope as; reinterpret_cast<void(__thiscall*)(void*,uint32,int32,uint32,uint32,uint32,uint32)>(a)(o,id,nx,a3,a4,u,nu); }
                        else if(nm=="Initialise"){
                            { ApiScope as; NewScope ns; ret=reinterpret_cast<uint32(__thiscall*)(void*,void*)>(a)(o,chan.data())&0xFF; }
                            At<uint32>(o,0xC)=P(At<void*>(o,0xC)); out["arena"]=Hash64(arena.data(),0x40); }
                        else if(nm=="Service"){
                            o[0x12]=uint8(next()%5!=0); o[0x16]=uint8(next()%3==0); o[0x15]=uint8(next()%3==0);
                            At<uint32>(o,0x18)=next()%3 ? next()%40 : next(); At<int32>(o,0x20)=next()%3==0 ? -1 : int32(next()%3 ? next()%40 : next()); At<uint32>(o,0x1C)=next();
                            if(next()%3==0)chPlaying=At<int32>(o,0x18);
                            { ApiScope as; NewScope ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(o); }
                            NormaliseDecoders(); out["arena"]=Hash64(arena.data(),0x100); out["userdec"]=Hash64(userDec.data(),sizeof(userDec)); }
                        else { // MainLoop
                            o[0x11]=uint8(next()%8!=0); fgBits=next(); fgIdx=0; sleepCount=0; sleepLimit=1+next()%12; timeIdx=0;
                            for(auto& t:times){ const uint32 hi=next()%4==0 ? next()%3 : 0u; t=(uint64(hi)<<32)|next(); }
                            for(size_t k=1;k<times.size();k+=2)if(next()%3){ const int32 dl=int32(next()%12)-3; times[k]=(times[k]&0xFFFFFFFF00000000ull)|uint32(uint32(times[k-1])+uint32(dl)); }
                            At<uint32>(chan.data(),0x5C)=next();
                            fgScripted=true;
                            { ApiScope as; ret=reinterpret_cast<DWORD(__stdcall*)(void*)>(a)(o); }
                            fgScripted=false; }
                        out["obj"]=ReversingProbe::Bytes(o,0x50); out["chan"]=Hash64(chan.data(),chan.size());
                        out["ds"]=Hash64(dsBuf.data(),sizeof(dsBuf));
                    }
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
        watchStop=true; if(watch){ WaitForSingleObject(watch,2000); CloseHandle(watch); }
        for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
        Restore(timerSaved);
        SetEvent(resumeEvt); // el hilo de streaming sigue con todos los hooks como estaban
    }
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
