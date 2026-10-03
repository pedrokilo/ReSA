#pragma once
// Banco dentro del motor: banco MP3 y decodificador Vorbis (lista de 13).
// 26 funciones de CAEMP3BankLoader y CAEVorbisDecoder. Original (hooks del lote desactivados) frente a candidato con
// fixtures deterministas. Nada toca ficheros, CdStream, libvorbis ni el banco de sonido reales:
// - El hilo de streaming de audio (AEAudioHardware.m_pStreamThread) se aparca durante todo el banco dentro de
//   IsForegroundApp (0x746060 + símbolo de la DLL), al principio de su bucle y fuera de cualquier decoder: desactivar
//   los hooks de CAEVorbisDecoder redirige también la vtable de la DLL al EXE (y el EXE usa el vorbisFile.dll del juego,
//   incompatible con los handles de la libvorbis de la DLL). Si no se aparca en 3 s el banco no corre.
// - CAEMP3BankLoader trabaja sobre un objeto falso con slots, lookups, handles, buffer y datos de stream falsos (con
//   relleno delante y detrás para índices negativos y lecturas fuera de rango del propio EXE); se simulan
//   CdStreamGetStatus, CdStreamRead, CMemoryMgr::Malloc/Free (solo durante Service), CAEAudioUtility::ConvertFromBytesToMS
//   y CAEBankLoader::LoadBankSlotFile/LoadBankLookupFile/LoadSFXPakLookupFile. GetBankLookup queda real (función pura).
// - CAEVorbisDecoder trabaja sobre un objeto falso (vtable del EXE 0x862D70) con streams de datos, OggVorbis_File y
//   vorbis_info falsos. Las funciones de vorbisFile.dll se sustituyen en la IAT del EXE (0x85835C..0x858374) y las de la
//   libvorbis estática de la DLL con EntryPatch; CAEDataStream (Initialise, FillBuffer, Seek, Close, GetCurrentPosition)
//   se simula en el EXE y en la DLL. operator new/delete se sustituyen solo en Initialise y Destructor; los falsos que
//   corren entonces registran sin reservar memoria (array fijo, se convierte a json después de la llamada).
// - Todos los falsos se ponen y se quitan en cada ejecución (después de cambiar el estado de los hooks), sin direcciones
//   repetidas; nunca están puestos durante Write/PumpMessages. Un vigilante deja EIP y pila del hilo principal en el log
//   (evento "watchdog", con dll_base) si una ejecución tarda más de 10 s.
#include <fstream>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Loaders/AEMP3BankLoader.h"
#include "Audio/Loaders/AEVorbisDecoder.h"
#include "Audio/Loaders/AEDataStream.h"
#include "Audio/AEAudioUtility.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "CdStreamInfo.h"
#include "MemoryMgr.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace Audio13Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint64 Hash64(const void* p,size_t n){ uint64 h=0x9E3779B97F4A7C15ull; auto* w=static_cast<const uint64*>(p); for(size_t i=0;i<n/8;++i){h^=w[i];h*=0x100000001B3ull;h^=h>>29;} return Fnv(static_cast<const uint8*>(p)+n/8*8,n%8,h); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// Sigue los saltos de la ILT de la DLL, pero nunca entra en la imagen del EXE (un hook desactivado deja en la DLL un
// salto al EXE: entonces se parchea ese salto y no la entrada del EXE, que ya se parchea aparte).
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
inline DWORD mainTid{};

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
inline std::array<Rec,2048> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec]={t,a,b,c,d}; ++nrec; }
inline json Recs(){
    const size_t n=std::min(nrec,recs.size());
    auto one=[&](size_t i){ const auto& r=recs[i]; return json::array({r.t,r.a,r.b,r.c,r.d}); };
    if(nrec<=96){ json r=json::array(); for(size_t i=0;i<n;++i)r.push_back(one(i)); return r; }
    json head=json::array(); for(size_t i=0;i<32;++i)head.push_back(one(i));
    return {{"count",nrec},{"hash",Hash64(recs.data(),n*sizeof(Rec))},{"head",head}}; }
alignas(16) inline std::array<uint8,0x1000> arena{}; inline size_t arenaTop{}; inline uint32 newCount{};
inline void* __cdecl FakeNew(size_t n){
    Push(1,uint32(n),newCount++);
    if(arenaTop+n>arena.size())return nullptr;
    void* p=arena.data()+arenaTop; arenaTop=(arenaTop+n+15)&~size_t(15); return p; }
inline void __cdecl FakeDelete(void* p){ Push(2,uint32(uintptr_t(p))); }
inline void ResetArena(){ arena.fill(0xCD); arenaTop=0; newCount=0; }

// --- copia de regiones globales ---
struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }

// --- entrada de la IAT del EXE sustituida mientras vive el objeto ---
struct IatSlot {
    uintptr_t* at; uintptr_t saved{};
    IatSlot(uintptr_t a,const void* v) : at(reinterpret_cast<uintptr_t*>(a)) { DWORD o{}; VirtualProtect(at,4,PAGE_READWRITE,&o); saved=*at; *at=uintptr_t(v); VirtualProtect(at,4,o,&o); }
    ~IatSlot(){ DWORD o{}; VirtualProtect(at,4,PAGE_READWRITE,&o); *at=saved; VirtualProtect(at,4,o,&o); }
};
// Parches de una sola ejecución: sin direcciones repetidas y deshechos en orden inverso.
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> P; std::vector<std::unique_ptr<IatSlot>> I; std::vector<uintptr_t> seen;
    template<class F> void Add(uintptr_t at,F fake){ if(std::ranges::find(seen,at)!=seen.end())return; seen.push_back(at); P.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,fake)); }
    void Iat(uintptr_t at,const void* fake){ I.push_back(std::make_unique<IatSlot>(at,fake)); }
    ~Patches(){ while(!P.empty())P.pop_back(); while(!I.empty())I.pop_back(); }
};

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// ======================= CAEMP3BankLoader =======================
constexpr size_t SLOT=0x12D4, PRE=3, NSLOTS=14, LKPRE=3;
alignas(16) inline std::array<uint8,0x700> mobj{};
alignas(16) inline std::array<uint8,NSLOTS*SLOT> mslots{};
alignas(16) inline std::array<uint8,16*12> mlk{};
alignas(16) inline std::array<uint8,0x10000> mbuf{};
alignas(16) inline std::array<int32,256> mhandles{};
alignas(16) inline std::array<uint8,0x6000> sbuf{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> malloced{};
alignas(16) inline std::array<uint8,0x4B0> vcs{};
alignas(16) inline std::array<int16,304> vlen{}, vloop{};
inline uint8* SlotAt(int idx){ return mslots.data()+(PRE+idx)*SLOT; }
inline std::array<int32,64> cdStatus{}; inline uint32 cdIdx{}, mallocIdx{}, loadMask{};
inline int32 __cdecl FakeCdStatus(int32 ch){ Push(10,uint32(ch)); return cdStatus[cdIdx++%64]; }
inline bool __cdecl FakeCdRead(int32 ch,void* buf,uint32 pos,int32 n){ Push(11,uint32(ch),uint32(uintptr_t(buf)),pos,uint32(n)); return true; }
inline void* __cdecl FakeMalloc(uint32 n,uint32 hint){ Push(12,n,hint); return malloced[mallocIdx++%2].data(); }
inline void __cdecl FakeFree(void* p){ Push(13,uint32(uintptr_t(p))); }
inline uint32 __cdecl FakeBytesToMs(uint32 len,uint32 rate,uint32 ch){ Push(14,len,rate,ch&0xFFFF); return (len*2654435761u)^rate^((ch&0xFFFF)<<7); }
inline bool __fastcall FakeLoadSlots(void* s,void*){ Push(15,s==mobj.data()); return (loadMask>>0)&1; }
inline bool __fastcall FakeLoadLookup(void* s,void*){ Push(16,s==mobj.data()); return (loadMask>>1)&1; }
inline bool __fastcall FakeLoadPak(void* s,void*){ Push(17,s==mobj.data()); return (loadMask>>2)&1; }

// ======================= CAEVorbisDecoder =======================
alignas(16) inline std::array<uint8,0x40> vobj{};
alignas(16) inline std::array<std::array<uint8,0x28>,2> vds{};
alignas(16) inline std::array<uint8,0x2D0> vf{};
alignas(16) inline std::array<uint8,0x20> vi{};
alignas(16) inline std::array<uint8,0x5000> vdest{};
alignas(16) inline std::array<uint8,0x40> vtmp{};
inline std::array<int32,16> readRets{}; inline uint32 readIdx{};
inline int32 openRet{}; inline bool infoNull{}; inline double timeRet{}; inline uint32 dsRet{};
inline uint32 V(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    if(p==vf.data())return 0x10000000; if(u>=uintptr_t(arena.data())&&u<uintptr_t(arena.data())+arena.size())return 0x20000000|uint32(u-uintptr_t(arena.data()));
    for(uint32 k=0;k<2;++k)if(p==vds[k].data())return 0x30000000|k;
    if(u>=uintptr_t(vdest.data())&&u<uintptr_t(vdest.data())+vdest.size())return 0x40000000|uint32(u-uintptr_t(vdest.data()));
    if(p==vi.data())return 0x50000000; if(p==vobj.data())return 0x60000000; return 0xEEEEEEEEu; }
inline int __cdecl FakeOvOpen(void* ds,OggVorbis_File* f,const char* init,long ib,ov_callbacks cb){
    Push(20,V(ds),V(f),uint32(uintptr_t(init)),uint32(ib));
    Push(21,uint32(uintptr_t(cb.read_func)),uint32(uintptr_t(cb.seek_func)),uint32(uintptr_t(cb.close_func)),uint32(uintptr_t(cb.tell_func))); return openRet; }
inline vorbis_info* __cdecl FakeOvInfo(OggVorbis_File* f,int link){ Push(22,V(f),uint32(link)); return infoNull ? nullptr : reinterpret_cast<vorbis_info*>(vi.data()); }
inline int __cdecl FakeOvClear(OggVorbis_File* f){ Push(23,V(f)); return 0; }
inline double __cdecl FakeOvTotal(OggVorbis_File* f,int i){ Push(24,V(f),uint32(i)); return timeRet; }
inline double __cdecl FakeOvTell(OggVorbis_File* f){ Push(25,V(f)); return timeRet; }
inline int __cdecl FakeOvSeek(OggVorbis_File* f,double s){ const auto b=std::bit_cast<uint64>(s); Push(26,V(f),uint32(b),uint32(b>>32)); return 0; }
inline long __cdecl FakeOvRead(OggVorbis_File* f,char* buf,int len,int be,int word,int sgn,int* bs){
    const uint32 k=readIdx++;
    Push(27,V(buf),uint32(len),uint32(be&0xFF)|(uint32(word&0xFF)<<8)|(uint32(sgn&0xFF)<<16)|(uint32(f==reinterpret_cast<OggVorbis_File*>(vf.data()))<<24),bs ? uint32(*bs) : 0xDEADu);
    long r=readRets[k%16]; if(r>len)r=len;
    for(long j=0;j<r;++j)buf[j]=char(k*31+j);
    if(bs)*bs=int(k); return r; }
inline uint32 __fastcall FakeDsFill(void* s,void*,void* dest,uint32 n){ Push(30,V(s),V(dest),n); return dsRet; }
inline uint32 __fastcall FakeDsSeek(void* s,void*,int32 off,int32 wh){ Push(31,V(s),uint32(off),uint32(wh)); return dsRet; }
inline uint32 __fastcall FakeDsClose(void* s,void*){ Push(32,V(s)); return dsRet&1; }
inline uint32 __fastcall FakeDsTell(void* s,void*){ Push(33,V(s)); return dsRet; }
inline bool __fastcall FakeDsInit(void* s,void*){ Push(34,V(s)); return (dsRet&1)!=0; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=26;
inline const std::array<Fn,NFN> fns{{
    {"CAEMP3BankLoader","Constructor",0x4DFB10,0,-1,64},{"CAEMP3BankLoader","GetBankSlotBuffer",0x4DFDE0,0,-1,512},
    {"CAEMP3BankLoader","GetSoundBankLoadingStatus",0x4E0250,0,-1,256},{"CAEMP3BankLoader","GetSoundBuffer",0x4E0280,0,-1,1024},
    {"CAEMP3BankLoader","GetSoundHeadroom",0x4E01E0,0,-1,512},{"CAEMP3BankLoader","GetSoundLoadingStatus",0x4E0400,0,-1,256},
    {"CAEMP3BankLoader","Initialise",0x4E08F0,0,-1,128},{"CAEMP3BankLoader","IsSoundBankLoaded",0x4E0220,0,-1,256},
    {"CAEMP3BankLoader","IsSoundLoaded",0x4E03B0,0,-1,512},{"CAEMP3BankLoader","LoadSound",0x4E07A0,0,-1,1024},
    {"CAEMP3BankLoader","LoadSoundBank",0x4E0670,0,-1,1024},{"CAEMP3BankLoader","Service",0x4DFE30,0,-1,1024},
    {"CAEMP3BankLoader","UpdateVirtualChannels",0x4E0450,0,-1,256},
    {"CAEVorbisDecoder","CloseCallback",0x5025D0,0,-1,128},{"CAEVorbisDecoder","Constructor",0x5026B0,0,-1,128},
    {"CAEVorbisDecoder","Destructor",0x5023F0,0,-1,256},{"CAEVorbisDecoder","FillBuffer",0,0x862D70,1,1024},
    {"CAEVorbisDecoder","GetSampleRate",0,0x862D70,5,128},{"CAEVorbisDecoder","GetStreamID",0,0x862D70,7,64},
    {"CAEVorbisDecoder","GetStreamLengthMs",0,0x862D70,2,512},{"CAEVorbisDecoder","GetStreamPlayTimeMs",0,0x862D70,3,512},
    {"CAEVorbisDecoder","Initialise",0,0x862D70,0,512},{"CAEVorbisDecoder","ReadCallback",0x502580,0,-1,512},
    {"CAEVorbisDecoder","SeekCallback",0x5025B0,0,-1,256},{"CAEVorbisDecoder","SetCursor",0,0x862D70,4,512},
    {"CAEVorbisDecoder","TellCallback",0x5025F0,0,-1,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"audio13-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"audio13-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio13");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"audio13-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // vtables y callbacks de la DLL (se construye y destruye un decoder sin stream ni handle, con los hooks activos)
    uint32 dllVorbisVt{}, dllBaseVt{}; std::array<uint32,4> dllCb{};
    { vtmp.fill(0); auto* d=new(vtmp.data()) CAEVorbisDecoder(nullptr,0); dllVorbisVt=At<uint32>(vtmp.data(),0);
      for(size_t k=0;k<4;++k)dllCb[k]=At<uint32>(vtmp.data(),8+4*k); d->~CAEVorbisDecoder(); dllBaseVt=At<uint32>(vtmp.data(),0); }
    auto vtName=[&](uint32 v)->json{ if(v==0x862D70||v==dllVorbisVt)return "CAEVorbisDecoder"; if(v==0x85F418||v==dllBaseVt)return "CAEStreamingDecoder"; return v; };
    static constexpr std::array<uint32,4> exeCb{0x502580,0x5025B0,0x5025D0,0x5025F0}; static const char* cbNames[]{"read","seek","close","tell"};
    auto cbName=[&](uint32 v)->json{ for(size_t k=0;k<4;++k)if(v==exeCb[k]||v==dllCb[k])return cbNames[k]; return v; };

    // estado global vigilado
    const uintptr_t dllForeground=Body(std::bit_cast<uintptr_t>(&IsForegroundApp));
    const std::vector<Saved> savedAll{ Save(0x85835C,0x1C), Save(0x862D70,0x20), Save(0x746060,5), Save(dllForeground,5) };

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
        // vigilante (hilo propio; solo escribe con WriteFile si el hilo principal se queda 10 s sin avanzar)
        strncpy_s(logPath,(dirPath/"audio13-runtime.jsonl").string().c_str(),_TRUNCATE);
        { HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Watchdog),&m); dllBase=uintptr_t(m); }
        DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&mainThread,THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,0);
        watchStop=false; const HANDLE watch=CreateThread(nullptr,0,&Watchdog,nullptr,0,nullptr);
        for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            Write({{"event","function_start"},{"function",full},{"index",f}});
            // Los falsos solo están puestos durante cada ejecución (nunca durante Write/PumpMessages).
            auto install=[&](Patches& pt){
            if(cls=="CAEMP3BankLoader") {
                pt.Add(Body(bit_cast<uintptr_t>(&CdStreamGetStatus)),FakeCdStatus); pt.Add(0x4063E0,FakeCdStatus);
                pt.Add(Body(bit_cast<uintptr_t>(&CdStreamRead)),FakeCdRead); pt.Add(0x406A20,FakeCdRead);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioUtility::ConvertFromBytesToMS)),FakeBytesToMs); pt.Add(0x4D9EF0,FakeBytesToMs);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEBankLoader::LoadBankSlotFile)),FakeLoadSlots); pt.Add(0x4E0590,FakeLoadSlots);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEBankLoader::LoadBankLookupFile)),FakeLoadLookup); pt.Add(0x4DFBD0,FakeLoadLookup);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEBankLoader::LoadSFXPakLookupFile)),FakeLoadPak); pt.Add(0x4DFC70,FakeLoadPak);
            } else {
                pt.Iat(0x85835C,reinterpret_cast<const void*>(&FakeOvOpen)); pt.Add(Body(bit_cast<uintptr_t>(&ov_open_callbacks)),FakeOvOpen);
                pt.Iat(0x858360,reinterpret_cast<const void*>(&FakeOvClear)); pt.Add(Body(bit_cast<uintptr_t>(&ov_clear)),FakeOvClear);
                pt.Iat(0x858364,reinterpret_cast<const void*>(&FakeOvTotal)); pt.Add(Body(bit_cast<uintptr_t>(&ov_time_total)),FakeOvTotal);
                pt.Iat(0x858368,reinterpret_cast<const void*>(&FakeOvTell)); pt.Add(Body(bit_cast<uintptr_t>(&ov_time_tell)),FakeOvTell);
                pt.Iat(0x85836C,reinterpret_cast<const void*>(&FakeOvRead)); pt.Add(Body(bit_cast<uintptr_t>(&ov_read)),FakeOvRead);
                pt.Iat(0x858370,reinterpret_cast<const void*>(&FakeOvInfo)); pt.Add(Body(bit_cast<uintptr_t>(&ov_info)),FakeOvInfo);
                pt.Iat(0x858374,reinterpret_cast<const void*>(&FakeOvSeek)); pt.Add(Body(bit_cast<uintptr_t>(&ov_time_seek)),FakeOvSeek);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEDataStream::Initialise)),FakeDsInit); pt.Add(0x4DC2B0,FakeDsInit);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEDataStream::FillBuffer)),FakeDsFill); pt.Add(0x4DC1C0,FakeDsFill);
                pt.Add(Body(bit_cast<uintptr_t>(static_cast<uint32(CAEDataStream::*)(long,int32)>(&CAEDataStream::Seek))),FakeDsSeek); pt.Add(0x4DC250,FakeDsSeek);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEDataStream::Close)),FakeDsClose); pt.Add(0x4DC2A0,FakeDsClose);
                pt.Add(Body(bit_cast<uintptr_t>(&CAEDataStream::GetCurrentPosition)),FakeDsTell); pt.Add(0x4DC230,FakeDsTell);
            }
            };
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto fill=[&](void* p,size_t n){ auto* w=static_cast<uint8*>(p); for(size_t k=0;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(w+k,&v,4); } for(size_t k=n&~size_t(3);k<n;++k)w[k]=uint8(next()); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x13Du; if(!stt)stt=1; nrec=0;
                    curFn=LONG(f); curCase=i; curRun=candidate; InterlockedIncrement(&progress);
                    for(auto* h:hooks)h->State(candidate);
                    Patches pt; install(pt);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CAEMP3BankLoader") {
                        auto* o=mobj.data();
                        fill(o,mobj.size()); fill(mslots.data(),mslots.size()); fill(mlk.data(),mlk.size()); fill(mhandles.data(),sizeof(mhandles));
                        At<void*>(o,0)=SlotAt(0); At<void*>(o,4)=mlk.data()+LKPRE*12; At<uint32>(o,8)=0x5A5A5A5Au;
                        const uint16 slotCnt=next()%6 ? uint16(next()%9) : (next()%2 ? uint16(0xFFFF) : uint16(0x8001));
                        const uint16 lkCnt=next()%6 ? uint16(1+next()%10) : uint16(next()%2 ? 0xFFFF : 0);
                        At<uint16>(o,0xC)=slotCnt; At<uint16>(o,0xE)=lkCnt; o[0x14]=uint8(next()%8 ? 1 : 0);
                        At<void*>(o,0x1C)=mbuf.data(); At<void*>(o,0x20)=mhandles.data();
                        At<uint16>(o,0x66A)=next()%4 ? uint16(4) : uint16(next()%3 ? next()%8 : 0xFFFF);
                        for(int k=0;k<60;++k)At<int16>(o,0x66C+2*k)=next()%3 ? int16(-1) : int16(next()%460);
                        for(int s=-int(PRE);s<int(NSLOTS-PRE);++s){ auto* sl=SlotAt(s);
                            At<uint32>(sl,0)=next()%0x8000; At<int16>(sl,0x10)=next()%4 ? int16(next()%8) : int16(-1);
                            const uint32 r=next()%8; At<int16>(sl,0x12)=r==0 ? int16(-1) : r==1 ? int16(-2-int(next()%3)) : r==2 ? int16(0) : int16(next()%401);
                            for(int k=0;k<400;++k)if(next()%4==0)At<uint32>(sl,0x14+12*k+4)=0xFFFFFFFFu; }
                        const auto sNum=[&](int s){ return At<int16>(SlotAt(s),0x12); };
                        const auto sBank=[&](int s){ return At<int16>(SlotAt(s),0x10); };
                        auto req=[&](int k){ return o+0x24+0x20*k; };
                        auto slotArg=[&]()->int32{ return next()%5 ? int32(next()%std::max<uint32>(1,std::min<uint32>(slotCnt,8))) : int32(next()%12)-2; }; // -2..9
                        std::fill(cdStatus.begin(),cdStatus.end(),0); for(auto& c:cdStatus)if(next()%4==0){ static const int32 st[]{255,250,254,1}; c=st[next()%4]; }
                        cdIdx=0; mallocIdx=0; loadMask=next();
                        if(nm=="Constructor"){ mobj.fill(0xCD); ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(o))-intptr_t(o); }
                        else if(nm=="Initialise")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(o);
                        else if(nm=="GetBankSlotBuffer"){ const int32 s=next()%3 ? slotArg() : int32(int16(slotCnt))+int32(next()%3)-1;
                            uint32 len=0xA5A5A5A5u; uint8* r=reinterpret_cast<uint8*(__thiscall*)(void*,int32,uint32*)>(a)(o,int16(s),&len);
                            ret=r ? json(intptr_t(r)-intptr_t(mbuf.data())) : json(nullptr); out["len"]=len; }
                        else if(nm=="GetSoundHeadroom"){ const int32 s=slotArg(); const int32 sfx=int32(next()%460);
                            ret=std::bit_cast<uint32>(reinterpret_cast<float(__thiscall*)(void*,int32,int32)>(a)(o,sfx,s)); }
                        else if(nm=="IsSoundBankLoaded"||nm=="GetSoundBankLoadingStatus"){ const int32 s=slotArg();
                            int32 bank=next()%2 ? int32(sBank(s)) : int32(int16(next()%3 ? next()%10 : next()));
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,int32,int32)>(a)(o,int16(bank),s); }
                        else if(nm=="IsSoundLoaded"||nm=="GetSoundLoadingStatus"){ const int32 s=slotArg();
                            const int32 bank=next()%3 ? int32(sBank(s)) : int32(int16(next()%3 ? next()%10 : next()));
                            const int32 sfx=next()%3 ? int32(At<int16>(o,0x66C+2*s)) : int32(int16(next()%3 ? next()%460 : next()));
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,int32,int32,int32)>(a)(o,int16(bank),int16(sfx),s); }
                        else if(nm=="GetSoundBuffer"){ const int32 s=slotArg(); int32 sfx;
                            switch(next()%5){ case 0: sfx=int32(int16(next())); break; case 1: sfx=-int32(1+next()%3); break;
                                case 2: sfx=int32(next()%460); At<int16>(o,0x66C+2*s)=int16(sfx); At<int16>(SlotAt(s),0x12)=-1; break;
                                default: sfx=sNum(s)>0 ? int32(next()%(uint32(sNum(s))+1)) : int32(next()%460); }
                            if(next()%3==0&&sNum(s)>0)sfx=sNum(s)-1;
                            uint32 size=0xA5A5A5A5u, rate=0xA5A5A5A5u;
                            uint8* r=reinterpret_cast<uint8*(__thiscall*)(void*,int32,int32,uint32*,uint16*)>(a)(o,sfx,s,&size,reinterpret_cast<uint16*>(&rate));
                            ret=r ? json(int64(intptr_t(r)-intptr_t(mbuf.data()))) : json(nullptr); out["size"]=size; out["rate"]=rate; }
                        else if(nm=="UpdateVirtualChannels"){
                            for(int k=0;k<300;++k){ const int16 s=next()%6 ? int16(next()%8) : int16(int(next()%13)-3);
                                int16 sfx=next()%8==0 ? int16(-1-int(next()%3)) : int16(next()%3 ? next()%60 : next()%460);
                                if(s>=0&&next()%4==0&&sNum(s)>0)sfx=int16(sNum(s)-1);
                                At<int16>(vcs.data(),2*k)=s; At<int16>(vcs.data(),0x258+2*k)=sfx; }
                            std::fill(vlen.begin(),vlen.end(),int16(0x5A5A)); std::fill(vloop.begin(),vloop.end(),int16(0x5A5A));
                            reinterpret_cast<void(__thiscall*)(void*,void*,int16*,int16*)>(a)(o,vcs.data(),vlen.data(),vloop.data());
                            out["len"]=Hash64(vlen.data(),sizeof(vlen)); out["loop"]=Hash64(vloop.data(),sizeof(vloop));
                            out["len_head"]=std::vector<int16>(vlen.begin(),vlen.begin()+24); out["loop_head"]=std::vector<int16>(vloop.begin(),vloop.begin()+24); }
                        else if(nm=="LoadSoundBank"||nm=="LoadSound"){
                            for(int k=0;k<50;++k){ auto* r=req(k); At<int16>(r,0x18)=next()%4 ? int16(next()%10) : int16(-1); At<int16>(r,0x1A)=int16(int(next()%12)-2);
                                At<int16>(r,0x1C)=next()%3 ? int16(-1) : int16(next()%400); }
                            At<uint16>(o,0x668)=next()%8 ? uint16(next()%50) : uint16(0xFFFF);
                            const int32 s=next()%4 ? slotArg() : int32(int16(slotCnt))+int32(next()%2);
                            int32 bank;
                            if(next()%5){ bank=int32(next()%std::max<uint32>(1,std::min<uint32>(lkCnt,10))); }
                            else { static const int32 bs[]{-1,-2,0x7FFF,11,12}; bank=bs[next()%5]; }
                            const int32 sfx=next()%4 ? int32(next()%400) : int32(std::array<int32,5>{400,399,-1,0x7FFF,450}[next()%5]);
                            if(next()%3==0){ auto* r=req(int(next()%50)); At<int16>(r,0x18)=int16(bank); At<int16>(r,0x1A)=int16(s); if(next()%2)At<int16>(r,0x1C)=nm=="LoadSound" ? int16(sfx) : int16(-1); }
                            if(next()%4==0){ At<int16>(SlotAt(std::clamp(s,-2,9)),0x10)=int16(bank); if(next()%2)At<int16>(o,0x66C+2*std::clamp(s,-2,9))=int16(sfx); }
                            // banco == contador: el EXE pasa los filtros y GetBankLookup devuelve nulo (se cae): fuera del fixture
                            if(int16(lkCnt)>=0&&bank==int32(int16(lkCnt)))bank=int16(lkCnt)+1;
                            if(nm=="LoadSoundBank")reinterpret_cast<void(__thiscall*)(void*,int32,int32)>(a)(o,int16(bank),int16(s));
                            else reinterpret_cast<void(__thiscall*)(void*,int32,int32,int32)>(a)(o,int16(bank),int16(sfx),int16(s)); }
                        else { // Service
                            fill(mbuf.data(),mbuf.size()); fill(sbuf.data(),sbuf.size());
                            for(int k=0;k<50;++k){ auto* r=req(k); const uint32 st=next()%16==0 ? 4+next()%3 : next()%4;
                                const int sl=next()%5 ? int(next()%8) : int(next()%12)-2;
                                At<void*>(r,0)=SlotAt(next()%6 ? sl : int(next()%12)-2);
                                At<uint32>(r,8)=next()%8==0&&st==1 ? next() : next()%0x3000;
                                At<void*>(r,0xC)=sbuf.data()+0x80+next()%0x700;
                                At<uint32>(r,0x14)=st; At<int16>(r,0x18)=next()%6 ? int16(next()%8) : int16(-1-int(next()%2)); At<int16>(r,0x1A)=int16(sl);
                                const uint32 q=next()%4; At<int16>(r,0x1C)=q==0 ? int16(-1) : q==1 ? int16(next()%400) : q==2 ? int16(next()%40) : (next()%8==0 ? int16(-2-int(next()%5)) : int16(next()%460)); }
                            At<uint16>(o,0x664)=next()%4 ? uint16(next()%50) : uint16(next());
                            { EntryPatch m1(Body(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc))),FakeMalloc),m1e(0x72F420,FakeMalloc),
                                         m2(Body(bit_cast<uintptr_t>(static_cast<void(*)(void*)>(&CMemoryMgr::Free))),FakeFree),m2e(0x72F430,FakeFree);
                              reinterpret_cast<void(__thiscall*)(void*)>(a)(o); }
                            out["buf"]=Hash64(mbuf.data(),mbuf.size()); }
                        json rq=json::array(); for(int k=0;k<50;++k)rq.push_back(Hash64(req(k),0x20));
                        json sl=json::array(); for(size_t k=0;k<NSLOTS;++k)sl.push_back(Hash64(mslots.data()+k*SLOT,SLOT));
                        out["head"]=ReversingProbe::Bytes(o,0x24); out["reqs"]=rq; out["tail"]=ReversingProbe::Bytes(o+0x664,0x80); out["slots"]=sl;
                        out["lookups"]=Hash64(mlk.data(),mlk.size()); out["beyond"]=Hash64(o+0x6E4,mobj.size()-0x6E4);
                    }
                    else { // CAEVorbisDecoder
                        auto* o=vobj.data();
                        fill(o,vobj.size()); for(auto& d:vds)fill(d.data(),d.size()); fill(vf.data(),vf.size()); fill(vi.data(),vi.size());
                        At<uint32>(o,0)=0x862D70; At<void*>(o,4)=vds[0].data(); At<void*>(o,0x18)=next()%4 ? static_cast<void*>(vf.data()) : nullptr;
                        o[0x1C]=uint8(next()%4 ? 1 : 0); { static const uint8 u[]{0,1,2,0x80}; o[0x1D]=u[next()%4]; }
                        At<void*>(o,0x20)=vds[1].data(); At<void*>(o,0x24)=vi.data();
                        { static const int32 ch[]{2,2,0,-1,3,6}; At<int32>(vi.data(),4)=ch[next()%6]; }
                        dsRet=next()%3 ? next()%0x2000 : next();
                        auto randTime=[&]()->double{
                            static const double fixed[]{7.0,6.999,6.9999999,7.0000001,6.9995,0.0,-131.0,-0.0004,100.5,2147483.0};
                            if(next()%3==0)return fixed[next()%10];
                            const uint64 frac=(uint64(next())<<32|next())&0x000FFFFFFFFFFFFFull; const int e=int(next()%32)-8;
                            const double m=std::bit_cast<double>(0x3FF0000000000000ull|frac); return std::ldexp(m,e)*(next()%8==0 ? -1.0 : 1.0); };
                        if(nm=="Constructor"){ vobj.fill(0xCD); void* ds=next()%3 ? nullptr : static_cast<void*>(vds[0].data());
                            static const int32 us[]{0,1,2,0x100,0x1FF,-1,0x80};
                            const int32 user=next()%4 ? us[next()%7] : int32(next());
                            ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*,void*,int32)>(a)(o,ds,user))-intptr_t(o);
                            json cb=json::array(); for(size_t k=0;k<4;++k){ cb.push_back(cbName(At<uint32>(o,8+4*k))); At<uint32>(o,8+4*k)=0; } out["callbacks"]=cb; }
                        else if(nm=="Destructor"){ At<void*>(o,4)=nullptr; ResetArena();
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(a)(o); } }
                        else if(nm=="Initialise"){
                            { static const int32 ch[]{1,2,2,0,-1}; At<int32>(vi.data(),4)=ch[next()%5]; }
                            At<uint32>(o,0x18)=next(); At<uint32>(o,0x24)=next();
                            openRet=next()%5 ? 0 : std::array<int32,5>{-132,-133,-130,1,-1}[next()%5]; infoNull=next()%10==0; timeRet=randTime();
                            ResetArena();
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete);
                              ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(o); }
                            out["arena"]=Hash64(arena.data(),0x300); out["info"]=V(At<void*>(o,0x24)); At<uint32>(o,0x24)=0; }
                        else if(nm=="FillBuffer"){
                            static const uint32 sizes[]{0,1,2,3,4,4095,4096,0x4000};
                            const uint32 n=next()%3 ? next()%0x4000 : sizes[next()%8];
                            for(auto& r:readRets){ const uint32 q=next()%10; r=q==0 ? std::array<int32,4>{0,-1,-3,-128}[next()%4] : q==1 ? int32(0x7FFFFFFF) : int32(next()%5000); }
                            readIdx=0; if(next()%8==0)At<void*>(o,0x24)=nullptr;
                            std::memset(vdest.data(),0xAB,n+64);
                            ret=uint32(reinterpret_cast<size_t(__thiscall*)(void*,void*,size_t)>(a)(o,vdest.data(),n));
                            out["dest"]=Hash64(vdest.data(),n+64); }
                        else if(nm=="GetStreamLengthMs"||nm=="GetStreamPlayTimeMs"){ { static const uint8 iv[]{0,1,1,1,2}; o[0x1C]=iv[next()%5]; } timeRet=randTime();
                            ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(o); }
                        else if(nm=="SetCursor"){ static const uint32 ps[]{0,1,16777217,16777219,0x7FFFFFFF,0x80000000,0x80000001,0xFFFFFFFF,123456789,999};
                            const uint32 pos=next()%3 ? next()%600000 : next()%2 ? ps[next()%10] : next();
                            reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(o,pos); }
                        else if(nm=="GetSampleRate"){ { static const uint8 iv[]{0,1,1,2}; o[0x1C]=iv[next()%4]; } ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(o); }
                        else if(nm=="GetStreamID")ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(o);
                        else if(nm=="ReadCallback"){ static const uint32 sz[]{1,2,4,0x10000,0xFFFFFFFF};
                            const uint32 size=next()%4 ? 1+next()%4 : sz[next()%5]; const uint32 nmemb=next()%4 ? next()%0x1000 : next();
                            void* op=next()%6 ? static_cast<void*>(vds[0].data()) : nullptr; dsRet=next()%3 ? next()%(size*nmemb+1==0 ? 1u : size*nmemb+1) : next();
                            ret=reinterpret_cast<uint32(__cdecl*)(void*,uint32,uint32,void*)>(a)(vdest.data(),size,nmemb,op); }
                        else if(nm=="SeekCallback"){ void* op=next()%6 ? static_cast<void*>(vds[0].data()) : nullptr;
                            const int64 off=next()%3 ? int64(next()%100000) : int64((uint64(next())<<32)|next()); const int32 wh=next()%4 ? int32(next()%3) : int32(next());
                            ret=reinterpret_cast<int32(__cdecl*)(void*,int64,int32)>(a)(op,off,wh); }
                        else { void* op=next()%6 ? static_cast<void*>(vds[0].data()) : nullptr; // CloseCallback / TellCallback
                            ret=reinterpret_cast<int32(__cdecl*)(void*)>(a)(op); }
                        out["vptr"]=vtName(At<uint32>(o,0)); At<uint32>(o,0)=0;
                        for(size_t off:{size_t(4),size_t(0x18),size_t(0x20),size_t(0x24)}){ const uint32 v=V(At<void*>(o,off)); if(v!=0xEEEEEEEEu)At<uint32>(o,off)=v; }
                        out["obj"]=ReversingProbe::Bytes(o,vobj.size());
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
        SetEvent(resumeEvt); // el hilo de streaming sigue con todos los hooks como estaban
    }
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
