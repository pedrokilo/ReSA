#pragma once
// Banco dentro del motor: radio de usuario y grupos de animaciones (lista de 15).
// 30 funciones de CAEUserRadioTrackManager y CAnimBlendAssocGroup. Original (hooks del lote desactivados) frente a
// candidato con fixtures deterministas. Nada toca el AEUserRadioTrackManager real, archivos del usuario, hilos ni
// las animaciones o modelos reales:
// - CAEUserRadioTrackManager trabaja sobre un gestor falso con su array de pistas (con relleno delante para los
//   índices negativos). CFileMgr (SetDirMyDocuments/SetDir/OpenFile/GetTotalSize/Read/Write/Seek/CloseFile) se
//   simula en el EXE y en la DLL: devuelve handles falsos y nunca abre nada. FindFirstFileA/FindNextFileA/
//   FindClose/GetFileAttributesA describen un árbol de carpetas virtual (función pura de la ruta y la semilla del
//   caso), CoCreateInstance da un IShellLinkA/IPersistFile falsos, CreateThread no crea hilos, TerminateThread y
//   DeleteFileA solo registran (DeleteUserTracksInfo no se llama si sus huecos de la IAT no están sustituidos), y
//   CoInitialize/CoUninitialize solo registran. Todo eso va por la IAT del EXE y de la DLL, solo alrededor de cada
//   llamada y solo para el hilo principal (los demás hilos van a la API real). Se simulan también
//   CAEDataStream::Initialise, InitLibrary/Shutdown de los decoders WMA y MediaFoundation/QuickTime, CMemoryMgr
//   (0x72F420/0x72F430), rand del EXE y CGeneral::GetRandomNumber. Los constructores reales de CAEDataStream y de
//   los decoders corren en el arena (sus hooks se fuerzan activos en las dos pasadas) y sus vtables/callbacks se
//   normalizan. El tipo 4 (MediaFoundation, NOTSA) queda fuera: su decoder siempre está "no soportado".
//   CFileMgr::ms_dirName, el índice/modo de la radio del menú y la tabla de extensiones del EXE se fijan por caso
//   y se restauran tras cada ejecución.
// - CAnimBlendAssocGroup trabaja sobre un grupo falso, arrays de asociaciones estáticas falsos (vtable propia: el
//   hueco 0 registra el destructor vectorial), jerarquías, bloque y model infos falsos. Se simulan
//   GetAnimationBlock, GetAnimation(nombre, bloque), GetModelInfo/GetModelInfoFromHashKey, CreateInstance (hueco 11
//   de la vtable falsa), RpAnimBlendClumpInit, CAnimBlendStaticAssociation::Init, GetFirstAtomic,
//   RpSkinGeometryGetSkin, RpClumpForAllAtomics, RpClumpDestroy, UncompressAnimation, AddAnimBlockRef y
//   CAnimBlendAssociation::Init(estática). Los hashes de CAnimManager::ms_aAnimations del rango del bloque se
//   escriben por caso y se restauran tras cada ejecución, igual que CModelInfo::ms_lastPositionSearched.
// - El hilo de streaming de audio se aparca en IsForegroundApp durante todo el banco (como Audio14).
// - Los arreglos reversibles GenericCrashing, GenericUB y CAEUserRadioTrackManager_UnicodeUserTracks se apagan
//   durante el banco y se restauran al final (las rutas NOTSA no se prueban).
// - operator new/delete y CMemoryMgr::Malloc/Free se sustituyen solo durante la llamada (registro sin reservas; los
//   otros hilos van al malloc/free del EXE). Ninguna función probada usa spdlog.
// - Todos los falsos se ponen y se quitan en cada ejecución (después de cambiar el estado de los hooks), sin
//   direcciones repetidas y deshechos en orden inverso; nunca están puestos durante Write/PumpMessages.
#include <fstream>
#include <new>
#include <ShObjIdl.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblebugfixes/Bugs.hpp>
#include "Audio/Managers/AEUserRadioTrackManager.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Loaders/AEDataStream.h"
#include "Audio/Loaders/AEVorbisDecoder.h"
#include "Audio/Loaders/AEWaveDecoder.h"
#include "Audio/Loaders/AEWMADecoder.h"
#include "Audio/Loaders/AEMFDecoder.h"
#include "Animation/AnimBlendAssocGroup.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendStaticAssociation.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlock.h"
#include "Models/ModelInfo.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "RwHelper.h"
#include "FileMgr.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace Audio15Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 Fs(const char* s){ return s ? uint32(Fnv(s,strlen(s))) : 0xDEADu; }
inline uint64 Hash64(const void* p,size_t n){ uint64 h=0x9E3779B97F4A7C15ull; auto* w=static_cast<const uint64*>(p); for(size_t i=0;i<n/8;++i){h^=w[i];h*=0x100000001B3ull;h^=h>>29;} return Fnv(static_cast<const uint8*>(p)+n/8*8,n%8,h); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// Sigue los saltos de la ILT de la DLL, pero nunca entra en la imagen del EXE.
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
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(GetCurrentThreadId()!=mainTid)return; if(nrec<recs.size())recs[nrec]={t,a,b,c,d}; ++nrec; }
inline json Recs(){
    const size_t n=std::min(nrec,recs.size());
    auto one=[&](size_t i){ const auto& r=recs[i]; return json::array({r.t,r.a,r.b,r.c,r.d}); };
    if(nrec<=96){ json r=json::array(); for(size_t i=0;i<n;++i)r.push_back(one(i)); return r; }
    json head=json::array(); for(size_t i=0;i<32;++i)head.push_back(one(i));
    return {{"count",nrec},{"hash",Hash64(recs.data(),n*sizeof(Rec))},{"head",head}}; }

// --- objetos falsos ---
constexpr size_t ARENA=0x22000, NTRK=80, TRK_PRE=16, NFIX=24, NHIER=24;
alignas(16) inline std::array<uint8,ARENA> arena{}; inline size_t arenaTop{}; inline uint32 newCount{};
alignas(16) inline std::array<uint8,0x80> mgrBuf{};
inline uint8* Mgr(){ return mgrBuf.data()+0x20; }
alignas(16) inline std::array<uint8,12*NTRK> tracksBuf{};
inline uint8* Tracks(){ return tracksBuf.data()+12*TRK_PRE; }
alignas(16) inline std::array<char,1024> strBuf{}, strBuf2{}, nameBuf{}, modelBuf{};
alignas(16) inline std::array<uint8,0x40> vecObj{}, lenBox{};
alignas(16) inline std::array<uint8,0x100> vecStore{};
alignas(16) inline std::array<uint8,12*16> recStore{};
alignas(16) inline std::array<uint8,0x40> grpBuf{};
inline uint8* Grp(){ return grpBuf.data()+0x10; }
alignas(16) inline std::array<uint8,0x20+0x14*NFIX+0x20> fixBuf{};
inline uint8* FixArr(){ return fixBuf.data()+0x20; }
alignas(16) inline std::array<uint8,0x18*NHIER> hierBuf{};
alignas(16) inline std::array<uint8,0x20> blockBuf{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> miBuf{};
alignas(16) inline std::array<uint8,0x40> clumpBuf{}, atomBuf{}, geomBuf{}, skinBuf{}, seqBuf{};
alignas(16) inline std::array<uint8,0x10> slObj{}, pfObj{};
alignas(16) inline std::array<const char*,16> namePtrs{};
inline std::array<void*,24> miVt{}, fixVt{}, slVt{}, pfVt{};

struct Region { uintptr_t base; size_t size; uint32 tag; };
inline std::array<Region,48> regions{}; inline size_t nregions{};
inline void AddRegion(const void* b,size_t n,uint32 tag){ if(nregions<regions.size())regions[nregions++]={uintptr_t(b),n,tag}; }
inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregions;++k){ const auto& r=regions[k]; if(u>=r.base&&u<r.base+r.size)return (r.tag<<20)|uint32(u-r.base); }
    return 0xEEEEEEEEu; }
inline bool InArena(const void* p,size_t n){ const auto u=uintptr_t(p); return u>=uintptr_t(arena.data())&&n<=ARENA&&u+n<=uintptr_t(arena.data())+ARENA; }

// operator new/delete (la DLL los reenvía aquí) y CMemoryMgr::Malloc/Free del EXE: solo durante la llamada.
// Los demás hilos van al malloc/free del EXE (lo mismo que hacen los originales).
inline void* ArenaAlloc(size_t n){
    if(n>ARENA||arenaTop+n>ARENA)return nullptr;
    void* p=arena.data()+arenaTop; arenaTop=(arenaTop+n+15)&~size_t(15); return p; }
inline void* __cdecl FakeNew(size_t n){
    if(!Main())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);
    Push(1,uint32(n),newCount++); return ArenaAlloc(n); }
inline void __cdecl FakeDelete(void* p){
    if(!Main()){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    Push(2,P(p)); }
inline void* __cdecl FakeMalloc(uint32 n){
    if(!Main())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);
    Push(3,n,newCount++); return ArenaAlloc(n); }
inline void __cdecl FakeFree(void* p){
    if(!Main()){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    Push(4,P(p)); }
inline void ResetArena(){ arena.fill(0xCD); arenaTop=0; newCount=0; }
struct NewScope {
    ReversingProbe::EntryPatch n1{0x82119A,FakeNew}, n2{0x821195,FakeNew}, d1{0x8214BD,FakeDelete}, d2{0x8213AE,FakeDelete},
                               m1{0x72F420,FakeMalloc}, f1{0x72F430,FakeFree};
};

// --- copia de regiones globales ---
struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }
inline void Restore(const Saved& s){ std::memcpy(reinterpret_cast<void*>(s.at),s.bytes.data(),s.bytes.size()); }

inline void SetSlot(uintptr_t* at,uintptr_t v){ DWORD o{}; VirtualProtect(at,4,PAGE_READWRITE,&o); *at=v; VirtualProtect(at,4,o,&o); }
// Parches de una sola ejecución: sin direcciones repetidas y deshechos en orden inverso.
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> P; std::vector<uintptr_t> seen;
    template<class F> void Add(uintptr_t at,F fake){ if(!at||std::ranges::find(seen,at)!=seen.end())return; seen.push_back(at); P.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,fake)); }
    ~Patches(){ while(!P.empty())P.pop_back(); }
};

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// ======================= Win32/COM por IAT (EXE y DLL, solo el hilo principal) =======================
enum ApiId { A_CREATE, A_TERM, A_DEL, A_FFIRST, A_FNEXT, A_FCLOSE, A_ATTR, A_COCREATE, A_COINIT, A_COUNINIT, NAPI };
struct Api { const char* name; uintptr_t exeSlot; uintptr_t fake{}; uintptr_t real{}; std::vector<uintptr_t*> slots; int exeCount{}, dllCount{}; bool exeSlotOk{}; };
inline std::array<Api,NAPI> apis{{ {"CreateThread",0x8580B4}, {"TerminateThread",0x8580FC}, {"DeleteFileA",0x8580F8},
    {"FindFirstFileA",0x85810C}, {"FindNextFileA",0x858104}, {"FindClose",0x858100}, {"GetFileAttributesA",0x858108},
    {"CoCreateInstance",0x85834C}, {"CoInitialize",0x858350}, {"CoUninitialize",0x858354} }};
template<class T> inline T Real(ApiId i){ return reinterpret_cast<T>(apis[i].real); }

inline HANDLE createRet{}; inline DWORD threadIdOut{}; inline uintptr_t dllThreadBody{};
inline uint32 StartTag(const void* s){ const auto u=uintptr_t(s); return (u==0x4F4A20||(dllThreadBody&&Body(u)==dllThreadBody)) ? 1u : 0xBADu; }
inline HANDLE WINAPI FakeCreateThread(LPSECURITY_ATTRIBUTES sa,SIZE_T st,LPTHREAD_START_ROUTINE start,LPVOID prm,DWORD fl,LPDWORD id){
    if(!Main())return Real<decltype(&CreateThread)>(A_CREATE)(sa,st,start,prm,fl,id);
    Push(70,uint32(uintptr_t(sa)),uint32(st),StartTag(reinterpret_cast<const void*>(start)),P(prm)); Push(71,fl,P(id));
    if(id)*id=threadIdOut; return createRet; }
inline BOOL WINAPI FakeTerminate(HANDLE h,DWORD code){
    if(!Main())return Real<decltype(&TerminateThread)>(A_TERM)(h,code);
    Push(72,uint32(uintptr_t(h)),code); return TRUE; }
inline BOOL WINAPI FakeDeleteFile(LPCSTR name){
    if(!Main())return Real<decltype(&DeleteFileA)>(A_DEL)(name);
    Push(73,Fs(name)); return TRUE; }
// Árbol virtual: cada carpeta es función pura de (hash del patrón, semilla); sin subcarpetas a partir de 7 separadores.
struct Enum { uint32 hash; uint32 pos, count; bool dirs, used; };
inline std::array<Enum,32> enums{}; inline uint32 fsSeed{};
inline int Seps(const char* s){ int n=0; for(;*s;++s)n+=*s=='\\'; return n; }
inline void Entry(uint32 h,uint32 idx,bool dirs,WIN32_FIND_DATAA* fd){
    std::memset(fd,0,sizeof(WIN32_FIND_DATAA));
    uint32 x=h^(idx*0x9E3779B9u)^fsSeed; x^=x<<13; x^=x>>17; x^=x<<5; if(!x)x=1;
    static const char* const exts[]{".mp3",".OGG",".wav",".wma",".txt",".m4a",".mov",".aac",".wmv",".Mp3",".flac",".ogg",".lnk.mp3",".WAV"};
    const uint32 kind=x%20, num=(x>>8)%100; char* n=fd->cFileName; DWORD attr=FILE_ATTRIBUTE_ARCHIVE;
    if(kind==0){ strcpy_s(n,MAX_PATH,"."); attr=FILE_ATTRIBUTE_DIRECTORY; }
    else if(kind==1){ strcpy_s(n,MAX_PATH,".."); attr=FILE_ATTRIBUTE_DIRECTORY; }
    else if(kind==2)sprintf_s(n,MAX_PATH,".h%u.mp3",num);
    else if((kind==3||kind==4)&&dirs){ sprintf_s(n,MAX_PATH,"d%02u",num); attr=FILE_ATTRIBUTE_DIRECTORY; }
    else if(kind==5)sprintf_s(n,MAX_PATH,"noext%u",num);
    else if(kind==6)sprintf_s(n,MAX_PATH,"l%u.lnk",num);
    else if(kind==7)sprintf_s(n,MAX_PATH,"L%u.LNK",num);
    else { sprintf_s(n,MAX_PATH,"s%u%s",num,exts[(x>>16)%14]); if((x>>24)%4==0)attr|=FILE_ATTRIBUTE_READONLY; }
    fd->dwFileAttributes=attr; fd->nFileSizeLow=x&0xFFFF; }
inline HANDLE WINAPI FakeFindFirst(LPCSTR pat,LPWIN32_FIND_DATAA fd){
    if(!Main())return Real<decltype(&FindFirstFileA)>(A_FFIRST)(pat,fd);
    const uint32 h=Fs(pat); Push(90,h);
    if((h^fsSeed)%11==0)return INVALID_HANDLE_VALUE;
    for(uint32 k=0;k<enums.size();++k)if(!enums[k].used){
        enums[k]={h,0,1+((h^(fsSeed>>3))%7),Seps(pat)<7,true}; Entry(h,0,enums[k].dirs,fd);
        return reinterpret_cast<HANDLE>(uintptr_t(0x7700+k*4)); }
    return INVALID_HANDLE_VALUE; }
inline Enum* EnumOf(HANDLE h){ const auto u=uintptr_t(h); if(u<0x7700||u>=0x7700+enums.size()*4||(u&3))return nullptr; auto* e=&enums[(u-0x7700)/4]; return e->used ? e : nullptr; }
inline BOOL WINAPI FakeFindNext(HANDLE h,LPWIN32_FIND_DATAA fd){
    if(!Main())return Real<decltype(&FindNextFileA)>(A_FNEXT)(h,fd);
    auto* e=EnumOf(h); Push(91,uint32(uintptr_t(h)),e ? e->pos+1 : 0xFFFF);
    if(!e||++e->pos>=e->count)return FALSE;
    Entry(e->hash,e->pos,e->dirs,fd); return TRUE; }
inline BOOL WINAPI FakeFindClose(HANDLE h){
    if(!Main())return Real<decltype(&FindClose)>(A_FCLOSE)(h);
    auto* e=EnumOf(h); Push(92,uint32(uintptr_t(h)),e!=nullptr); if(e)e->used=false; return TRUE; }
inline DWORD WINAPI FakeAttributes(LPCSTR path){
    if(!Main())return Real<decltype(&GetFileAttributesA)>(A_ATTR)(path);
    Push(93,Fs(path));
    if(strstr(path,"missing"))return INVALID_FILE_ATTRIBUTES;
    return strstr(path,"\\dir") ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE; }
// COM: IShellLinkA y IPersistFile falsos con resultados guionizados por orden de llamada
inline uint32 comBits{}, comIdx{}, lastLoad{};
inline bool ComOk(){ const uint32 k=comIdx++; return ((comBits>>(k%32))&1)!=0; }
inline HRESULT WINAPI FakeCoCreate(REFCLSID clsid,LPUNKNOWN outer,DWORD ctx,REFIID riid,LPVOID* ppv){
    if(!Main())return Real<decltype(&CoCreateInstance)>(A_COCREATE)(clsid,outer,ctx,riid,ppv);
    Push(100,uint32(Fnv(&clsid,16)),P(outer),ctx,uint32(Fnv(&riid,16)));
    if(ComOk()){ *ppv=slObj.data(); return S_OK; } *ppv=nullptr; return REGDB_E_CLASSNOTREG; }
inline HRESULT WINAPI FakeCoInit(LPVOID r){
    if(!Main())return Real<decltype(&CoInitialize)>(A_COINIT)(r);
    Push(101,P(r)); return S_OK; }
inline void WINAPI FakeCoUninit(){
    if(!Main()){ Real<decltype(&CoUninitialize)>(A_COUNINIT)(); return; }
    Push(102); }
inline HRESULT __stdcall SlQI(void* self,REFIID riid,void** ppv){ Push(103,P(self),uint32(Fnv(&riid,16))); if(ComOk()){ *ppv=pfObj.data(); return S_OK; } *ppv=nullptr; return E_NOINTERFACE; }
inline ULONG __stdcall ComAddRef(void* self){ Push(104,P(self)); return 2; }
inline ULONG __stdcall ComRelease(void* self){ Push(105,P(self)); return 1; }
inline HRESULT __stdcall ComBad(void* self){ Push(106,P(self)); return E_NOTIMPL; }
inline HRESULT __stdcall PfLoad(void* self,LPCOLESTR name,DWORD mode){
    lastLoad=name ? uint32(Fnv(name,wcslen(name)*2)) : 0; Push(107,P(self),lastLoad,mode); return ComOk() ? S_OK : E_FAIL; }
inline HRESULT __stdcall SlGetPath(void* self,LPSTR buf,int cch,WIN32_FIND_DATAA* fd,DWORD flags){
    Push(108,P(self),P(buf),uint32(cch),flags); (void)fd;
    if(!ComOk())return E_FAIL;
    uint32 x=lastLoad^fsSeed^0x5BD1E995u; x^=x<<13; x^=x>>17; x^=x<<5;
    char t[MAX_PATH]{}; const uint32 num=(x>>8)%50;
    switch(x%7){
    case 0: case 1: sprintf_s(t,"C:\\T\\dir%u",num); break;
    case 2: sprintf_s(t,"C:\\T\\s%u.mp3",num); break;
    case 3: sprintf_s(t,"C:\\T\\missing%u.ogg",num); break;
    case 4: sprintf_s(t,"C:\\T\\x%u.txt",num); break;
    case 5: sprintf_s(t,"C:\\T\\s%u.WAV",num); break;
    default: break; }
    if(buf&&cch>0)strncpy_s(buf,size_t(cch),t,_TRUNCATE);
    return (x>>20)%5==0 ? S_FALSE : S_OK; }

inline HMODULE SelfModule(){ HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&SelfModule),&m); return m; }
inline void FindApiSlots(){
    const void* fakes[NAPI]{reinterpret_cast<const void*>(&FakeCreateThread),reinterpret_cast<const void*>(&FakeTerminate),reinterpret_cast<const void*>(&FakeDeleteFile),
        reinterpret_cast<const void*>(&FakeFindFirst),reinterpret_cast<const void*>(&FakeFindNext),reinterpret_cast<const void*>(&FakeFindClose),
        reinterpret_cast<const void*>(&FakeAttributes),reinterpret_cast<const void*>(&FakeCoCreate),reinterpret_cast<const void*>(&FakeCoInit),
        reinterpret_cast<const void*>(&FakeCoUninit)};
    const HMODULE mods[4]{GetModuleHandleA("kernel32.dll"),GetModuleHandleA("kernelbase.dll"),GetModuleHandleA("ole32.dll"),GetModuleHandleA("combase.dll")};
    for(int i=0;i<NAPI;++i){ auto& a=apis[i]; a.fake=uintptr_t(fakes[i]); a.slots.clear(); a.exeCount=a.dllCount=0; a.real=0; a.exeSlotOk=false;
        std::array<uintptr_t,4> cand{}; for(int m=0;m<4;++m)cand[m]=mods[m] ? uintptr_t(GetProcAddress(mods[m],a.name)) : 0;
        auto isReal=[&](uintptr_t v){ if(!v)return false; for(auto c:cand)if(c&&c==v)return true; return false; };
        const uintptr_t exeVal=*reinterpret_cast<uintptr_t*>(a.exeSlot);
        a.exeSlotOk=isReal(exeVal); a.real=a.exeSlotOk ? exeVal : 0;
        if(!a.real)for(auto c:cand)if(c){ a.real=c; break; }
        if(!a.real)continue;
        const HMODULE imgs[2]{GetModuleHandleA(nullptr),SelfModule()};
        for(int mi=0;mi<2;++mi){
            auto* base=reinterpret_cast<uint8*>(imgs[mi]); auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
            const auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if(!dir.VirtualAddress)continue;
            for(auto* d=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);d->Name;++d)
                for(auto* t=reinterpret_cast<uintptr_t*>(base+d->FirstThunk);*t;++t)
                    if(isReal(*t)&&std::ranges::find(a.slots,t)==a.slots.end()){ a.slots.push_back(t); ++(mi ? a.dllCount : a.exeCount); }
        } } }
inline bool ApiReady(ApiId i){ const auto& a=apis[i]; return a.real&&a.exeSlotOk&&a.exeCount&&a.dllCount; }
inline bool SlotsHold(ApiId i){ const auto& a=apis[i]; if(a.slots.empty())return false; for(auto* s:a.slots)if(*s!=a.fake)return false; return true; }
// Huecos de la IAT con los falsos solo alrededor de la llamada (sin reservas)
struct ApiScope {
    std::array<std::pair<uintptr_t*,uintptr_t>,128> saved{}; size_t n{};
    ApiScope(){ for(auto& a:apis)for(auto* s:a.slots)if(n<saved.size()){ saved[n++]={s,*s}; SetSlot(s,a.fake); } }
    ~ApiScope(){ while(n){ --n; SetSlot(saved[n].first,saved[n].second); } }
};

// ======================= CFileMgr, decoders y RNG falsos =======================
inline std::array<uint32,4> openRet{}; inline uint32 openIdx{}, readSeed{}, readIdx{}; inline int32 totalSize{};
inline int32 __cdecl FakeDocs(){ if(Main())Push(110); return 0; }
inline int32 __cdecl FakeSetDir(const char* p){ if(Main())Push(111,Fs(p)); return 0; }
inline FILE* __cdecl FakeOpen(const char* path,const char* mode){
    if(!Main()){ FILE* f{}; fopen_s(&f,path,mode); return f; }
    const uint32 k=openIdx++; Push(112,Fs(path),Fs(mode),k); return reinterpret_cast<FILE*>(uintptr_t(openRet[k%4])); }
inline int32 __cdecl FakeTotal(FILE* f){
    if(!Main()){ const long cur=ftell(f); fseek(f,0,SEEK_END); const long n=ftell(f); fseek(f,cur,SEEK_SET); return n; }
    Push(113,uint32(uintptr_t(f))); return totalSize; }
inline size_t __cdecl FakeRead(FILE* f,void* buf,size_t n){
    if(!Main())return fread(buf,1,n,f);
    const uint32 k=readIdx++; Push(114,uint32(uintptr_t(f)),P(buf),uint32(n),k);
    if(buf&&(InArena(buf,n)))for(size_t i=0;i<n;++i)static_cast<char*>(buf)[i]=char('a'+(readSeed+k*31+uint32(i)*7)%26);
    return n; }
inline size_t __cdecl FakeWrite(FILE* f,const void* buf,size_t n){
    if(!Main())return fwrite(buf,1,n,f);
    Push(115,uint32(uintptr_t(f)),buf&&n<=0x1000 ? uint32(Fnv(buf,n)) : P(buf),uint32(n)); return n; }
inline bool __cdecl FakeSeek(FILE* f,long off,int32 org){
    if(!Main())return fseek(f,off,org)!=0;
    Push(116,uint32(uintptr_t(f)),uint32(off),uint32(org)); return false; }
inline int32 __cdecl FakeClose(FILE* f){
    if(!Main())return fclose(f);
    Push(117,uint32(uintptr_t(f))); return 0; }
inline bool wmaInitRet{}, mfInitRet{}, dsInitRet{};
inline bool __cdecl FakeWmaInit(){ Push(120); return wmaInitRet; }
inline bool __cdecl FakeMfInit(){ Push(121); return mfInitRet; }
inline void __cdecl FakeWmaShutdown(){ Push(122); }
inline void __cdecl FakeMfShutdown(){ Push(123); }
inline bool __fastcall FakeDsInit(void* s,void*){ if(!Main())return false; Push(124,P(s)); return dsInitRet; }
inline uint32 randBase{}, randIdx{};
inline uint32 NextRand(){ const uint32 k=randIdx++; if(Main())Push(80,k); return (randBase+k*7919u)&0x7FFF; }
inline int __cdecl FakeRand(){ return int(NextRand()); }
inline uint16 __cdecl FakeRandomNumber(){ return uint16(NextRand()); }

// ======================= CAnimBlendAssocGroup: animaciones, modelos y RW falsos =======================
inline uint32 atomBits{}, atomIdx{}, skinBits{}, skinIdx{}; inline uintptr_t dllSkinCb[2]{};
inline void* MiFor(uint32 key){ return key%5==0 ? nullptr : static_cast<void*>(miBuf[key%3].data()); }
inline CAnimBlock* __cdecl FakeGetBlock(const char* name){ Push(130,Fs(name)); return reinterpret_cast<CAnimBlock*>(blockBuf.data()); }
inline void* __cdecl FakeMiFromKey(uint32 key,int32* idx){ Push(131,key,P(idx)); return MiFor(key); }
inline void* __cdecl FakeMiByName(const char* name,int32* idx){ Push(132,Fs(name),P(idx),uint32(CModelInfo::ms_lastPositionSearched)); return MiFor(Fs(name)>>1); }
inline void __cdecl FakeClumpInit(RpClump* c){ Push(133,P(c)); }
inline void __fastcall FakeStaticInit(void* s,void*,void* clump,void* hier){ Push(134,P(s),P(clump),P(hier)); At<void*>(s,0x10)=hier; At<uint16>(s,4)=uint16(3+(P(hier)&7)); }
inline void* __cdecl FakeFirstAtomic(RpClump* c){ const uint32 k=atomIdx++; Push(135,P(c),k); return (atomBits>>(k%32))&1 ? static_cast<void*>(atomBuf.data()) : nullptr; }
inline void* __cdecl FakeGetSkin(void* g){ const uint32 k=skinIdx++; Push(136,P(g),k); return (skinBits>>(k%32))&1 ? static_cast<void*>(skinBuf.data()) : nullptr; }
inline uint32 CbTag(const void* cb){ const auto u=uintptr_t(cb); return (u==0x734B90||u==dllSkinCb[0]||u==dllSkinCb[1]||Body(u)==dllSkinCb[1]) ? 1u : 0xBADu; }
inline void* __cdecl FakeForAll(RpClump* c,void* cb,void* data){ Push(137,P(c),CbTag(cb),P(data)); return c; }
inline int32 __cdecl FakeClumpDestroy(RpClump* c){ Push(138,P(c)); return 1; }
inline void* __cdecl FakeGetAnim(const char* name,const CAnimBlock* block){
    const uint32 h=Fs(name); Push(139,h,P(block)); return h%5==0 ? nullptr : static_cast<void*>(hierBuf.data()+0x18*(h%NHIER)); }
inline void __cdecl FakeUncompress(void* hier){ Push(140,P(hier)); }
inline void __cdecl FakeAddBlockRef(int32 idx){ Push(141,uint32(idx)); }
inline void __fastcall FakeAssocInit(void* s,void*,void* st){ Push(142,P(s),P(st)); At<void*>(s,0x14)=At<void*>(st,0x10); At<uint16>(s,0x2E)=At<uint16>(st,0xA); }
inline void* __fastcall FakeCreateInstance(void* mi,void*){ Push(143,P(mi)); return clumpBuf.data(); }
inline void* __fastcall FakeCreateInstanceM(void* mi,void*,void* m){ Push(144,P(mi),P(m)); return clumpBuf.data(); }
inline void __fastcall FakeBadVirtual(void* s,void*){ Push(145,P(s)); }
inline void* __fastcall FakeVecDtor(void* s,void*,uint32 flags){ Push(146,P(s),flags); return s; }

// vtables y callbacks de la DLL y del EXE normalizados en el arena
inline std::vector<std::pair<uint32,uint32>> vtMap;
inline void NormaliseArena(){
    for(size_t k=0;k+4<=arenaTop;k+=4){ uint32& v=At<uint32>(arena.data(),k); for(const auto& [from,to]:vtMap)if(v==from){ v=to; break; } } }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=30;
inline const std::array<Fn,NFN> fns{{
    {"CAEUserRadioTrackManager","DeleteUserTracksInfo",0x4F32F0,0,-1,128},{"CAEUserRadioTrackManager","GetAudioFileType",0x4F31F0,0,-1,512},
    {"CAEUserRadioTrackManager","GetTrackPath",0x4F3050,0,-1,512},{"CAEUserRadioTrackManager","GetUserTrackPlayMode",0x4F3330,0,-1,64},
    {"CAEUserRadioTrackManager","Initialise",0x4F35B0,0,-1,256},{"CAEUserRadioTrackManager","IsShortcut",0x4F32C0,0,-1,256},
    {"CAEUserRadioTrackManager","LoadUserTrack",0x4F35F0,0,-1,1024},{"CAEUserRadioTrackManager","ReadUserTracks",0x4F2FD0,0,-1,512},
    {"CAEUserRadioTrackManager","ResolveShortcut-",0x4F30F0,0,-1,512},{"CAEUserRadioTrackManager","ScanUserTracks",0x4F4BA0,0,-1,128},
    {"CAEUserRadioTrackManager","SelectUserTrackIndex",0x4F3250,0,-1,1024},{"CAEUserRadioTrackManager","SetUserTrackIndex",0x4F3340,0,-1,64},
    {"CAEUserRadioTrackManager","Shutdown",0x4F2FB0,0,-1,128},{"CAEUserRadioTrackManager","WriteUserTracksFile-",0x4F4690,0,-1,1024},
    {"CAEUserRadioTrackManager","WriteUserTracksThread",0x4F4A20,0,-1,512},
    {"CAnimBlendAssocGroup","Constructor",0x4CDE70,0,-1,64},{"CAnimBlendAssocGroup","CopyAnimation-AnimationId",0x4CE130,0,-1,256},
    {"CAnimBlendAssocGroup","CopyAnimation-Name",0x4CE0B0,0,-1,256},{"CAnimBlendAssocGroup","CreateAssociations-1",0x4CE220,0,-1,512},
    {"CAnimBlendAssocGroup","CreateAssociations-2",0x4CE6E0,0,-1,512},{"CAnimBlendAssocGroup","CreateAssociations-3",0x4CE3B0,0,-1,512},
    {"CAnimBlendAssocGroup","DestroyAssociations",0x4CDFF0,0,-1,256},{"CAnimBlendAssocGroup","Destructor",0x4CE1D0,0,-1,256},
    {"CAnimBlendAssocGroup","GetAnimBlock",0x45B060,0,-1,64},{"CAnimBlendAssocGroup","GetAnimation-AnimationId",0x4CE090,0,-1,256},
    {"CAnimBlendAssocGroup","GetAnimation-Name",0x4CE040,0,-1,512},{"CAnimBlendAssocGroup","GetAnimationId",0x4CE1B0,0,-1,512},
    {"CAnimBlendAssocGroup","GetNumAnimations",0x45B050,0,-1,64},{"CAnimBlendAssocGroup","InitEmptyAssociations",0x4CDFB0,0,-1,256},
    {"CAnimBlendAssocGroup","IsCreated",0x4D37A0,0,-1,64},
}};
// Hooks de constructores/destructores de los que dependen las funciones probadas: activos en las dos pasadas.
constexpr size_t NDEP=7;
inline const std::array<std::pair<const char*,const char*>,NDEP> deps{{
    {"CAEDataStream","Constructor"},{"CAEDataStream","Destructor"},{"CAEVorbisDecoder","Constructor"},{"CAEWaveDecoder","Constructor"},
    {"CAEWMADecoder","Constructor"},{"CAnimBlendAssociation","Constructor3-static_blend"},{"CAnimBlendStaticAssociation","Constructor1"},
}};
alignas(16) inline std::array<uint8,0x20100> wmaTmp{};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"audio15-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"audio15-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio15");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"audio15-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,NDEP> depHooks{};
    { json missing=json::array(); for(size_t k=0;k<deps.size();++k)if(!(depHooks[k]=FindHook(deps[k].first,deps[k].second)))missing.push_back(std::string(deps[k].first)+"::"+deps[k].second);
      Write({{"event","dependency_hooks"},{"missing",missing}}); }

    // regiones para normalizar punteros
    nregions=0;
    AddRegion(arena.data(),arena.size(),0x02); AddRegion(mgrBuf.data(),mgrBuf.size(),0x03); AddRegion(tracksBuf.data(),tracksBuf.size(),0x04);
    AddRegion(strBuf.data(),strBuf.size(),0x05); AddRegion(strBuf2.data(),strBuf2.size(),0x06); AddRegion(nameBuf.data(),nameBuf.size(),0x07);
    AddRegion(modelBuf.data(),modelBuf.size(),0x08); AddRegion(vecObj.data(),vecObj.size(),0x09); AddRegion(lenBox.data(),lenBox.size(),0x0A);
    AddRegion(vecStore.data(),vecStore.size(),0x0B); AddRegion(recStore.data(),recStore.size(),0x0C); AddRegion(grpBuf.data(),grpBuf.size(),0x0D);
    AddRegion(fixBuf.data(),fixBuf.size(),0x0E); AddRegion(hierBuf.data(),hierBuf.size(),0x0F); AddRegion(blockBuf.data(),blockBuf.size(),0x10);
    for(uint32 k=0;k<3;++k)AddRegion(miBuf[k].data(),miBuf[k].size(),0x11+k);
    AddRegion(clumpBuf.data(),clumpBuf.size(),0x14); AddRegion(atomBuf.data(),atomBuf.size(),0x15); AddRegion(geomBuf.data(),geomBuf.size(),0x16);
    AddRegion(skinBuf.data(),skinBuf.size(),0x17); AddRegion(seqBuf.data(),seqBuf.size(),0x18); AddRegion(slObj.data(),slObj.size(),0x19);
    AddRegion(pfObj.data(),pfObj.size(),0x1A); AddRegion(namePtrs.data(),sizeof(namePtrs),0x1B);
    AddRegion(reinterpret_cast<void*>(0xB4EA40),2500*0x18,0x70); AddRegion(reinterpret_cast<void*>(0xBA67F4),8,0x71);

    // vtables de la DLL (objetos temporales construidos sin efectos) y del EXE
    uint32 dllStaticVt{}, dllAssocVt{}, dllDsVt{}, dllVorbisVt{}, dllWaveVt{}, dllWmaVt{}; std::array<uint32,4> dllCb{};
    { alignas(16) std::array<uint8,0x40> t{}; new(t.data()) CAnimBlendStaticAssociation(); dllStaticVt=At<uint32>(t.data(),0); }
    { alignas(16) std::array<uint8,0x80> t{}; new(t.data()) CAnimBlendAssociation(); dllAssocVt=At<uint32>(t.data(),0); }
    { alignas(16) std::array<uint8,0x40> t{}; new(t.data()) CAEDataStream(0,nullptr,0,0,false); dllDsVt=At<uint32>(t.data(),0); }
    { alignas(16) std::array<uint8,0x40> t{}; new(t.data()) CAEVorbisDecoder(nullptr,0); dllVorbisVt=At<uint32>(t.data(),0); for(size_t k=0;k<4;++k)dllCb[k]=At<uint32>(t.data(),8+4*k); }
    { alignas(16) std::array<uint8,0x80> t{}; new(t.data()) CAEWaveDecoder(nullptr); dllWaveVt=At<uint32>(t.data(),0); }
    { new(wmaTmp.data()) CAEWMADecoder(nullptr); dllWmaVt=At<uint32>(wmaTmp.data(),0); }
    vtMap={{0x85C6CCu,0x1001u},{dllStaticVt,0x1001u},{0x85C6D0u,0x1002u},{dllAssocVt,0x1002u},{0x85F0BCu,0x1003u},{dllDsVt,0x1003u},
           {0x862D70u,0x1004u},{dllVorbisVt,0x1004u},{0x862DE8u,0x1005u},{dllWaveVt,0x1005u},{0x862D90u,0x1006u},{dllWmaVt,0x1006u},
           {0x502580u,0x1011u},{0x5025B0u,0x1012u},{0x5025D0u,0x1013u},{0x5025F0u,0x1014u}};
    for(size_t k=0;k<4;++k)vtMap.push_back({dllCb[k],0x1011u+uint32(k)});
    dllSkinCb[0]=std::bit_cast<uintptr_t>(&AtomicRemoveAnimFromSkinCB); dllSkinCb[1]=Body(dllSkinCb[0]);
    Write({{"event","vtables"},{"static",dllStaticVt},{"assoc",dllAssocVt},{"ds",dllDsVt},{"vorbis",dllVorbisVt},{"wave",dllWaveVt},{"wma",dllWmaVt}});

    // vtables falsas
    for(auto& v:miVt)v=reinterpret_cast<void*>(&FakeBadVirtual);
    miVt[10]=reinterpret_cast<void*>(&FakeCreateInstanceM); miVt[11]=reinterpret_cast<void*>(&FakeCreateInstance);
    for(auto& v:fixVt)v=reinterpret_cast<void*>(&FakeBadVirtual);
    fixVt[0]=reinterpret_cast<void*>(&FakeVecDtor);
    for(auto& v:slVt)v=reinterpret_cast<void*>(&ComBad);
    slVt[0]=reinterpret_cast<void*>(&SlQI); slVt[1]=reinterpret_cast<void*>(&ComAddRef); slVt[2]=reinterpret_cast<void*>(&ComRelease); slVt[3]=reinterpret_cast<void*>(&SlGetPath);
    for(auto& v:pfVt)v=reinterpret_cast<void*>(&ComBad);
    pfVt[1]=reinterpret_cast<void*>(&ComAddRef); pfVt[2]=reinterpret_cast<void*>(&ComRelease); pfVt[5]=reinterpret_cast<void*>(&PfLoad);
    At<void*>(slObj.data(),0)=slVt.data(); At<void*>(pfObj.data(),0)=pfVt.data();

    FindApiSlots();
    { json a=json::array(); for(auto& x:apis)a.push_back({x.name,x.exeCount,x.dllCount,x.exeSlotOk}); Write({{"event","api_slots"},{"apis",a}}); }

    // dirección del hilo de la DLL (destino del salto del hook en 0x4F4A20)
    { auto* th=hooks[14]; const bool was=th->Hooked(), lockedTh=th->Locked(); th->LockState(false); th->State(true);
      if(ReversingProbe::JumpsToDll(0x4F4A20))dllThreadBody=Body(0x4F4A25+*reinterpret_cast<const int32*>(0x4F4A21));
      th->State(was); th->LockState(lockedTh); }

    // estado global vigilado
    const uintptr_t dllForeground=Body(std::bit_cast<uintptr_t>(&IsForegroundApp));
    const std::vector<Saved> savedAll{ Save(0x858080,0x90), Save(0x85834C,0x10), Save(0x746060,5), Save(dllForeground,5), Save(0xB6B970,0x1C),
                                       Save(0xBA67F4,8), Save(0x8CBB28,0x60), Save(0xB71A60,128), Save(0xAAE948,4), Save(0xB4EA40,2500*0x18) };
    const Saved menuSaved=Save(0xBA67F4,8), tableSaved=Save(0x8CBB28,0x60), dirSaved=Save(0xB71A60,128), lastPosSaved=Save(0xAAE948,4);

    // arreglos reversibles apagados durante el banco
    auto& fixCrash=const_cast<bool&>(notsa::bugfixes::GenericCrashing.Enabled);
    auto& fixUB=const_cast<bool&>(notsa::bugfixes::GenericUB.Enabled);
    auto& fixUnicode=const_cast<bool&>(notsa::bugfixes::CAEUserRadioTrackManager_UnicodeUserTracks.Enabled);
    const bool fixCrashSaved=fixCrash, fixUBSaved=fixUB, fixUnicodeSaved=fixUnicode;

    // aparcar el hilo de streaming de audio
    parkedEvt=CreateEventA(nullptr,FALSE,FALSE,nullptr); resumeEvt=CreateEventA(nullptr,TRUE,FALSE,nullptr);
    const HANDLE streamThread=AEAudioHardware.m_pStreamThread.m_nHandle;
    const bool streamRunning=streamThread&&WaitForSingleObject(streamThread,0)==WAIT_TIMEOUT;
    streamTid=streamRunning ? GetThreadId(streamThread) : 0;
    std::array<bool,NFN> enabled{},locked{}; std::array<bool,NDEP> depEnabled{},depLocked{};
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch fg1(0x746060,FakeForeground), fg2(dllForeground,FakeForeground);
        const bool parkedOk=!streamRunning||WaitForSingleObject(parkedEvt,3000)==WAIT_OBJECT_0;
        if(!parkedOk){ SetEvent(resumeEvt); Write({{"event","error"},{"reason","audio stream thread not parked"},{"state_restored",true}}); return; }
        Write({{"event","stream_thread"},{"running",streamRunning},{"parked",streamRunning}});
        strncpy_s(logPath,(dirPath/"audio15-runtime.jsonl").string().c_str(),_TRUNCATE);
        { HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Watchdog),&m); dllBase=uintptr_t(m); }
        DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&mainThread,THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,0);
        watchStop=false; const HANDLE watch=CreateThread(nullptr,0,&Watchdog,nullptr,0,nullptr);
        for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
        for(size_t k=0;k<deps.size();++k)if(depHooks[k]){depEnabled[k]=depHooks[k]->Hooked();depLocked[k]=depHooks[k]->Locked();depHooks[k]->LockState(false);depHooks[k]->State(true);}
        fixCrash=false; fixUB=false; fixUnicode=false;

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            Write({{"event","function_start"},{"function",full},{"index",f}});
            const bool radio=cls=="CAEUserRadioTrackManager";
            // APIs que necesita cada función (por la IAT)
            std::vector<ApiId> need;
            if(nm=="DeleteUserTracksInfo")need={A_TERM,A_DEL};
            else if(nm=="ScanUserTracks")need={A_CREATE};
            else if(nm=="ResolveShortcut-")need={A_COCREATE};
            else if(nm=="WriteUserTracksFile-")need={A_FFIRST,A_FNEXT,A_FCLOSE,A_ATTR,A_COCREATE};
            else if(nm=="WriteUserTracksThread")need={A_FFIRST,A_FNEXT,A_FCLOSE,A_ATTR,A_COCREATE,A_COINIT,A_COUNINIT};
            bool ready=true; for(auto a:need)ready&=ApiReady(a);
            if(!ready){ Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","IAT slots not found"}}); passed=false; continue; }
            const bool useApi=!need.empty();
            // Los falsos solo están puestos durante cada ejecución (nunca durante Write/PumpMessages).
            auto install=[&](Patches& pt){
                auto both=[&](uintptr_t exeAt,uintptr_t dllAt,auto fake){ pt.Add(exeAt,fake); pt.Add(Body(dllAt),fake); };
                const bool files=nm=="GetTrackPath"||nm=="LoadUserTrack"||nm=="ReadUserTracks"||nm=="Initialise"||nm=="DeleteUserTracksInfo"
                               ||nm=="WriteUserTracksFile-"||nm=="WriteUserTracksThread";
                if(radio&&files){
                    both(0x538860,bit_cast<uintptr_t>(&CFileMgr::SetDirMyDocuments),FakeDocs);
                    both(0x5387D0,bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir);
                    both(0x538900,bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpen);
                    both(0x5389E0,bit_cast<uintptr_t>(&CFileMgr::GetTotalSize),FakeTotal);
                    both(0x538950,bit_cast<uintptr_t>(&CFileMgr::Read),FakeRead);
                    both(0x538970,bit_cast<uintptr_t>(&CFileMgr::Write),FakeWrite);
                    both(0x538990,bit_cast<uintptr_t>(&CFileMgr::Seek),FakeSeek);
                    both(0x5389D0,bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeClose);
                }
                if(nm=="Initialise"){
                    both(0x502B80,bit_cast<uintptr_t>(&CAEWMADecoder::InitLibrary),FakeWmaInit);
                    both(0x4E7C70,bit_cast<uintptr_t>(&CAEMFDecoder::InitLibrary),FakeMfInit);
                }
                if(nm=="Shutdown"){
                    both(0x502BF0,bit_cast<uintptr_t>(&CAEWMADecoder::Shutdown),FakeWmaShutdown);
                    both(0x4E7850,bit_cast<uintptr_t>(&CAEMFDecoder::Shutdown),FakeMfShutdown);
                }
                if(nm=="LoadUserTrack")both(0x4DC2B0,bit_cast<uintptr_t>(&CAEDataStream::Initialise),FakeDsInit);
                if(nm=="SelectUserTrackIndex"){ pt.Add(0x821B1E,FakeRand); pt.Add(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber); }
                if(nm.starts_with("CreateAssociations")){
                    both(0x4D3940,bit_cast<uintptr_t>(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock)),FakeGetBlock);
                    both(0x4CEC20,bit_cast<uintptr_t>(&CAnimBlendStaticAssociation::Init),FakeStaticInit);
                }
                if(nm=="CreateAssociations-1"||nm=="CreateAssociations-3"){
                    both(0x4C59B0,bit_cast<uintptr_t>(&CModelInfo::GetModelInfoFromHashKey),FakeMiFromKey);
                    both(0x4C5940,bit_cast<uintptr_t>(static_cast<CBaseModelInfo*(*)(const char*,int32*)>(&CModelInfo::GetModelInfo)),FakeMiByName);
                    both(0x4D6720,bit_cast<uintptr_t>(&RpAnimBlendClumpInit),FakeClumpInit);
                    both(0x734820,bit_cast<uintptr_t>(&GetFirstAtomic),FakeFirstAtomic);
                    pt.Add(0x7C7550,FakeGetSkin); pt.Add(0x749B70,FakeForAll); pt.Add(0x74A310,FakeClumpDestroy); // la DLL los llama en el EXE
                }
                if(nm=="CreateAssociations-2")
                    both(0x4D42F0,bit_cast<uintptr_t>(static_cast<CAnimBlendHierarchy*(*)(const char*,const CAnimBlock*)>(&CAnimManager::GetAnimation)),FakeGetAnim);
                if(nm=="InitEmptyAssociations")both(0x4CEC20,bit_cast<uintptr_t>(&CAnimBlendStaticAssociation::Init),FakeStaticInit);
                if(nm.starts_with("CopyAnimation")){
                    both(0x4D41C0,bit_cast<uintptr_t>(&CAnimManager::UncompressAnimation),FakeUncompress);
                    both(0x4D3FB0,bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),FakeAddBlockRef);
                    both(0x4CEEC0,bit_cast<uintptr_t>(static_cast<void(CAnimBlendAssociation::*)(CAnimBlendStaticAssociation&)>(&CAnimBlendAssociation::Init)),FakeAssocInit);
                }
            };
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto fill=[&](void* p,size_t n){ auto* w=static_cast<uint8*>(p); for(size_t k=0;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(w+k,&v,4); } for(size_t k=n&~size_t(3);k<n;++k)w[k]=uint8(next()); };
                auto pick=[&](std::initializer_list<const char*> l){ return *(l.begin()+next()%l.size()); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x15Du; if(!stt)stt=1; nrec=0;
                    curFn=LONG(f); curCase=i; curRun=candidate; InterlockedIncrement(&progress);
                    for(auto* h:hooks)h->State(candidate);
                    Patches pt; install(pt);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // contadores de los falsos
                    openIdx=readIdx=comIdx=randIdx=atomIdx=skinIdx=0; lastLoad=0; for(auto& e:enums)e.used=false;
                    ResetArena();
                    fsSeed=next(); comBits=next()|next()|next(); readSeed=next(); randBase=next();
                    atomBits=next(); skinBits=next(); wmaInitRet=next()%2; mfInitRet=next()%3!=0; dsInitRet=next()%4!=0;
                    { const uint32 r=next()%8; openRet[0]=r==0 ? 0u : r==1 ? 0x80007100u : 0x7104u; }
                    openRet[1]=next()%7==0 ? 0u : 0x7108u; openRet[2]=0x710Cu; openRet[3]=0x7110u;
                    { const uint32 r=next()%8; totalSize=r==0 ? 0 : r==1 ? int32(0xFFFFFFF4u) : r==2 ? int32(12*(1+next()%40)+1+next()%11) : int32(12*(1+next()%60)); }
                    createRet=reinterpret_cast<HANDLE>(uintptr_t(0x6000u+(next()&0xFF)*4)); threadIdOut=0x5150u+uint32(i);
                    std::vector<Saved> animSaved;

                    if(radio) {
                        auto* m=Mgr(); fill(mgrBuf.data(),mgrBuf.size());
                        auto* tr=tracksBuf.data();
                        for(size_t k=0;k<NTRK;++k){ At<uint32>(tr,12*k)=next()%5000; At<uint32>(tr,12*k+4)=next()%120;
                            static const uint32 types[]{0,1,1,1,1,2,2,2,3,3,3,4,5,7,0x10,2}; At<uint32>(tr,12*k+8)=types[next()%16]; }
                        At<void*>(m,0)=Tracks(); m[4]=uint8(next()%6!=0); m[5]=uint8(next()); At<uint16>(m,6)=uint16(next()%65);
                        for(size_t k=8;k<0x10;++k)m[k]=uint8(next()%3!=0 ? 1 : 0);
                        At<uint32>(m,0x14)=0x6000u+(next()&0xFF)*4; At<uint32>(m,0x18)=next()%6;
                        // menú: índice y modo de la radio de usuario (se restauran tras la ejecución)
                        *reinterpret_cast<uint32*>(0xBA67F4)=next()%4==0 ? next() : next()%70;
                        { static const int8 modes[]{0,1,2,0,1,2,-1,3,int8(0x80),127}; *reinterpret_cast<int8*>(0xBA67F8)=modes[next()%10]; }
                        auto* s=strBuf.data(); std::memset(s,0,strBuf.size());
                        if(nm=="DeleteUserTracksInfo"){
                            bool safe=false;
                            { ApiScope as; if(SlotsHold(A_DEL)&&SlotsHold(A_TERM)){ safe=true; NewScope ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(m); } }
                            out["safe"]=safe; }
                        else if(nm=="GetAudioFileType"||nm=="IsShortcut"){
                            const char* base=pick({"","song","a.b","C:\\x\\y","noext","..","x.","Track 01","dir.lnk\\f"});
                            const char* ext=nm=="IsShortcut" ? pick({".lnk",".LNK",".Lnk",".lnk.mp3",".lnkx","",".",".mp3",".lnk ",".lnk"})
                                                              : pick({".ogg",".OGG",".mp3",".Mp3",".wav",".wma",".wmv",".aac",".m4a",".mov",".MOV",".flac",".lnk",".txt","",".",".mp3.txt",".mp33",".WaV"});
                            sprintf_s(s,strBuf.size(),"%s%s",base,ext);
                            *reinterpret_cast<uint32*>(0x8CBB3C)=next()%2 ? 4u : 3u;
                            uint32 r;
                            { NewScope ns; r=reinterpret_cast<uint32(__thiscall*)(void*,const char*)>(a)(m,s); }
                            ret=nm=="IsShortcut" ? r&0xFF : r; }
                        else if(nm=="GetTrackPath"||nm=="LoadUserTrack"){
                            const uint16 count=next()%10==0 ? uint16(65535) : uint16(next()%65); At<uint16>(m,6)=count;
                            const int32 id=count==65535 ? int32(next()%80)-16 : next()%8==0 ? int32(next()%20)-16 : int32(next()%(uint32(count)+4));
                            if(nm=="LoadUserTrack"){ m[8+4]=0; if(next()%4==0)m[8]=1; }
                            void* r;
                            { NewScope ns; r=reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(m,id); }
                            ret=P(r);
                            if(nm=="GetTrackPath"&&r&&InArena(r,1)){ const auto* c=static_cast<const char*>(r); size_t n=0; while(n<200&&InArena(c+n,1)&&c[n])++n; out["path"]=Fnv(c,n); }
                            NormaliseArena(); out["arena"]=Hash64(arena.data(),arenaTop); }
                        else if(nm=="ReadUserTracks"||nm=="Initialise"){
                            { const uint32 r=next()%4; At<void*>(m,0)=r==0 ? nullptr : r==1 ? static_cast<void*>(recStore.data()) : Tracks(); }
                            if(nm=="Initialise")*reinterpret_cast<uint32*>(0x8CBB3C)=next()%2 ? 4u : 3u;
                            { NewScope ns; ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(m)&0xFF; }
                            if(nm=="Initialise")out["table"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8CBB28),0x60);
                            out["arena"]=Hash64(arena.data(),arenaTop); }
                        else if(nm=="ResolveShortcut-"){
                            sprintf_s(s,strBuf.size(),"%s\\%s%u.lnk",pick({"C:\\UT","C:\\UT\\sub","D:\\Music",""}),pick({"l","L","link "}),next()%40);
                            void* r;
                            { ApiScope as; NewScope ns; r=reinterpret_cast<void*(__thiscall*)(void*,const char*)>(a)(m,s); }
                            ret=P(r);
                            if(r&&InArena(r,1)){ const auto* c=static_cast<const char*>(r); size_t n=0; while(n<MAX_PATH&&InArena(c+n,1)&&c[n])++n; out["target"]=Fnv(c,n); }
                            out["arena"]=Hash64(arena.data(),arenaTop); }
                        else if(nm=="ScanUserTracks"){
                            static const uint32 st[]{0,0,0,1,2,3,4,0xFFFFFFFFu,5};
                            At<uint32>(m,0x18)=st[next()%9];
                            { ApiScope as; ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(m)&0xFF; } }
                        else if(nm=="SelectUserTrackIndex"){
                            { const uint32 r=next()%10; At<uint16>(m,6)=r==0 ? uint16(0) : r==1 ? uint16(1) : r==2 ? uint16(2) : r==3 ? uint16(65535) : uint16(3+next()%30); }
                            { const uint32 r=next()%4; uint32& idx=*reinterpret_cast<uint32*>(0xBA67F4); idx=r==0 ? 0xFFFFFFFFu : r==1 ? next() : next()%(uint32(At<uint16>(m,6))+3); }
                            ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(m); }
                        else if(nm=="SetUserTrackIndex"){
                            const int32 v=next()%4==0 ? -1 : next()%3==0 ? int32(next()) : int32(next()%100);
                            reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(m,v); }
                        else if(nm=="GetUserTrackPlayMode"){
                            *reinterpret_cast<int8*>(0xBA67F8)=int8(next());
                            ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(m)&0xFF; }
                        else if(nm=="Shutdown"){
                            { const uint32 r=next()%3; At<void*>(m,0)=r==0 ? nullptr : r==1 ? static_cast<void*>(recStore.data()) : Tracks(); }
                            NewScope ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(m); }
                        else if(nm=="WriteUserTracksFile-"){
                            sprintf_s(s,strBuf.size(),"%s",pick({"C:\\UT","C:\\UT","C:\\UT\\","D:\\Music","C:\\T\\dir7",""}));
                            auto* v=reinterpret_cast<UserTracksLegacyVector*>(vecObj.data()); fill(vecObj.data(),vecObj.size()); fill(recStore.data(),recStore.size());
                            fill(vecStore.data(),vecStore.size());
                            if(next()%3==0){ v->first=v->last=v->end=nullptr; }
                            else { const uint32 cap=1+next()%8, cnt=next()%(cap+1); auto** st=reinterpret_cast<tUserTracksInfo**>(vecStore.data());
                                for(uint32 k=0;k<cnt;++k)st[k]=reinterpret_cast<tUserTracksInfo*>(recStore.data()+12*k);
                                v->first=st; v->last=st+cnt; v->end=st+cap; }
                            auto& len=At<uint32>(lenBox.data(),0); len=next()%3==0 ? 0u : next()%1000;
                            static const int32 depths[]{0,0,0,0,0,0,1,2,14,15,16,-1};
                            const int32 depth=depths[next()%12];
                            { ApiScope as; NewScope ns; ret=reinterpret_cast<int32(__thiscall*)(void*,const char*,void*,FILE*,uint32*,int32)>(a)(m,s,v,reinterpret_cast<FILE*>(0x7104),&len,depth); }
                            out["len"]=len;
                            json ents=json::array();
                            if(v->first&&v->last){ const ptrdiff_t n=v->last-v->first;
                                for(ptrdiff_t k=0;k<n&&k<64;++k){ auto* e=v->first[k]; ents.push_back(P(e)!=0xEEEEEEEEu&&e ? json(ReversingProbe::Bytes(e,12)) : json(P(e))); } }
                            out["vec"]={P(v->first),P(v->last),P(v->end)}; out["entries"]=ents;
                            out["arena"]=Hash64(arena.data(),arenaTop); }
                        else { // WriteUserTracksThread
                            char* dir=reinterpret_cast<char*>(0xB71A60); std::memset(dir,0,128);
                            strcpy_s(dir,128,pick({"C:\\GTA","C:\\Users\\X\\Documents\\GTA San Andreas User Files","D:\\G",""}));
                            openRet[0]=next()%6==0 ? 0u : 0x7104u; openRet[1]=next()%6==0 ? 0u : 0x7108u;
                            { ApiScope as; NewScope ns; ret=reinterpret_cast<DWORD(__stdcall*)(void*)>(a)(m); }
                            out["arena"]=Hash64(arena.data(),arenaTop); }
                        // gestor y menú normalizados antes de comparar
                        At<uint32>(m,0)=P(At<void*>(m,0));
                        out["mgr"]=ReversingProbe::Bytes(m,0x1C); out["mgrpad"]=Hash64(mgrBuf.data(),mgrBuf.size());
                        out["tracks"]=Hash64(tracksBuf.data(),tracksBuf.size()); out["menu"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA67F4),8);
                        out["store"]={Hash64(recStore.data(),recStore.size()),Hash64(vecStore.data(),vecStore.size())};
                        Restore(menuSaved); Restore(tableSaved); Restore(dirSaved);
                    }
                    else { // CAnimBlendAssocGroup
                        auto* g=Grp(); fill(grpBuf.data(),grpBuf.size());
                        // jerarquías con nombres conocidos (clave uppercase real) y bloque de animaciones
                        auto* nb=nameBuf.data(); std::memset(nb,0,nameBuf.size());
                        auto nameOf=[&](uint32 j){ return nb+32*(j%NHIER); };
                        const uint32 nameSalt=next()%1000;
                        for(uint32 j=0;j<NHIER;++j){ sprintf_s(nameOf(j),32,"%s_%u",j%3==0 ? "ped_walk" : j%3==1 ? "Run" : "idle_Stance",nameSalt+j); }
                        fill(hierBuf.data(),hierBuf.size());
                        for(uint32 j=0;j<NHIER;++j)At<uint32>(hierBuf.data(),0x18*j)=CKeyGen::GetUppercaseKey(nameOf(next()%3==0 ? next() : j));
                        fill(blockBuf.data(),blockBuf.size());
                        for(auto& mb:miBuf){ fill(mb.data(),mb.size()); At<void*>(mb.data(),0)=miVt.data(); }
                        fill(clumpBuf.data(),clumpBuf.size()); clumpBuf[0]=2; fill(atomBuf.data(),atomBuf.size()); At<void*>(atomBuf.data(),0x18)=geomBuf.data();
                        // array de asociaciones estáticas de partida (vtable falsa: el hueco 0 registra el destructor vectorial)
                        fill(fixBuf.data(),fixBuf.size());
                        const uint32 n=next()%(NFIX+1);
                        At<uint32>(fixBuf.data(),0x1C)=next()%4==0 ? 0u : n;
                        for(uint32 k=0;k<NFIX;++k){ auto* e=FixArr()+0x14*k; At<void*>(e,0)=fixVt.data(); At<int16>(e,6)=int16(next()%3==0 ? -1 : int32(next()%400));
                            At<void*>(e,0xC)=next()%3==0 ? nullptr : static_cast<void*>(seqBuf.data()); At<void*>(e,0x10)=hierBuf.data()+0x18*(next()%NHIER); }
                        At<void*>(g,0)=blockBuf.data(); At<void*>(g,4)=next()%5==0 ? nullptr : static_cast<void*>(FixArr());
                        At<uint32>(g,8)=next()%16==0 ? 0x80000000u|next()%8 : n; At<uint32>(g,0xC)=next()%3==0 ? next()%2000 : next()%40;
                        At<int32>(g,0x10)=next()%8==0 ? -1 : int32(next()%120);
                        // sin array no se puede recorrer: las funciones que lo leen reciben entonces m_NumAnims 0
                        if(!At<void*>(g,4)&&(nm=="GetAnimation-Name"||nm=="GetAnimationId"||nm=="CopyAnimation-Name"||nm=="InitEmptyAssociations"))At<uint32>(g,8)=0;
                        auto* s=strBuf.data(); std::memset(s,0,strBuf.size());
                        auto query=[&]{ const uint32 r=next()%8; if(r<5){ strcpy_s(s,strBuf.size(),nameOf(next()));
                                            if(r==1)for(char* c=s;*c;++c)*c=char(toupper(uint8(*c))); else if(r==2)for(char* c=s;*c;++c)*c=char(tolower(uint8(*c))); }
                                        else if(r<7)sprintf_s(s,strBuf.size(),"unknown_%u",next()%50); };
                        if(nm=="Constructor"){ std::memset(grpBuf.data(),0xCD,grpBuf.size()); ret=P(reinterpret_cast<void*(__thiscall*)(void*)>(a)(g)); }
                        else if(nm=="GetAnimBlock")ret=uint32(reinterpret_cast<uintptr_t(__thiscall*)(void*)>(a)(g));
                        else if(nm=="GetNumAnimations"){ At<uint32>(g,8)=next(); ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(g); }
                        else if(nm=="IsCreated"){ if(next()%3==0)At<uint32>(g,8)=0; ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(g)&0xFF; }
                        else if(nm=="GetAnimation-Name"||nm=="GetAnimationId"){ query();
                            if(nm=="GetAnimationId")ret=reinterpret_cast<int32(__thiscall*)(void*,const char*)>(a)(g,s);
                            else ret=P(reinterpret_cast<void*(__thiscall*)(void*,const char*)>(a)(g,s)); }
                        else if(nm=="GetAnimation-AnimationId"){
                            const int32 id=int32(At<uint32>(g,0xC))+int32(next()%(NFIX+8))-4;
                            const auto r=reinterpret_cast<uintptr_t(__thiscall*)(void*,int32)>(a)(g,id);
                            ret=uint32(r-uintptr_t(At<void*>(g,4))); }
                        else if(nm=="CopyAnimation-Name"||nm=="CopyAnimation-AnimationId"){
                            for(uint32 k=0;k<NFIX;++k)At<uint16>(FixArr()+0x14*k,0xA)=uint16(next()%2 ? next() : next()&~0x4000u);
                            void* r;
                            if(nm=="CopyAnimation-Name"){ query(); NewScope ns; r=reinterpret_cast<void*(__thiscall*)(void*,const char*)>(a)(g,s); }
                            else { if(At<void*>(g,4)&&n==0)At<void*>(g,4)=nullptr;
                                   const int32 id=int32(At<uint32>(g,0xC))+(At<void*>(g,4) ? int32(next()%std::max<uint32>(n,1)) : 0);
                                   NewScope ns; r=reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(g,id); }
                            ret=P(r); NormaliseArena(); out["arena"]=Hash64(arena.data(),arenaTop); }
                        else if(nm=="DestroyAssociations"||nm=="Destructor"){
                            NewScope ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(g); }
                        else if(nm=="InitEmptyAssociations"){
                            void* clump=next()%4==0 ? reinterpret_cast<void*>(uintptr_t(0x12340000u|(next()&0xFFF0))) : static_cast<void*>(clumpBuf.data());
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(g,clump); }
                        else { // CreateAssociations-1/2/3
                            sprintf_s(s,strBuf.size(),"%s",pick({"ped","int_house","gangs","","fat"}));
                            // bloque: rango de ms_aAnimations con hashes del caso (se restaura tras la ejecución)
                            const uint32 numAnims=next()%8==0 ? 0u : 1+next()%12; const int32 first=int32(next()%(2500-16));
                            At<int32>(blockBuf.data(),0x14)=first; At<uint32>(blockBuf.data(),0x18)=numAnims;
                            if(nm!="CreateAssociations-2"&&numAnims){ animSaved.push_back(Save(0xB4EA40+uintptr_t(first)*0x18,0x18*numAnims));
                                for(uint32 k=0;k<numAnims;++k){ auto& key=*reinterpret_cast<uint32*>(0xB4EA40+(uintptr_t(first)+k)*0x18);
                                    key=next()%4==0 ? next() : CKeyGen::GetUppercaseKey(nameOf(next())); } }
                            if(nm=="CreateAssociations-1"){ NewScope ns; reinterpret_cast<void(__thiscall*)(void*,const char*)>(a)(g,s); }
                            else if(nm=="CreateAssociations-3"){
                                static const uint32 strides[]{16,24,32};
                                const uint32 stride=strides[next()%3];
                                auto* an=strBuf2.data(); auto* mn=modelBuf.data(); std::memset(an,0,strBuf2.size()); std::memset(mn,0,modelBuf.size());
                                for(uint32 k=0;k<numAnims;++k){
                                    if(next()%5!=0)strncpy_s(an+k*stride,stride,nameOf(next()),_TRUNCATE); else sprintf_s(an+k*stride,stride,"zz%u",next()%100);
                                    sprintf_s(mn+k*stride,stride,"mdl_%u",next()%60); }
                                const char* models=next()%8==0 ? nullptr : mn;
                                *reinterpret_cast<int32*>(0xAAE948)=int32(next()%500);
                                { NewScope ns; reinterpret_cast<void(__thiscall*)(void*,const char*,const char*,const char*,uint32)>(a)(g,s,an,models,stride); }
                                out["lastpos"]=*reinterpret_cast<int32*>(0xAAE948); }
                            else { // -2
                                const uint32 cnt=next()%11; auto* nm2=strBuf2.data(); std::memset(nm2,0,strBuf2.size());
                                for(uint32 k=0;k<cnt;++k){ char* p=nm2+32*k; if(next()%4!=0)sprintf_s(p,32,"anim_%u",next()%300); namePtrs[k]=p; }
                                void* clump=next()%4==0 ? nullptr : static_cast<void*>(clumpBuf.data());
                                NewScope ns; reinterpret_cast<void(__thiscall*)(void*,const char*,void*,const char**,uint32)>(a)(g,s,clump,namePtrs.data(),cnt); }
                            NormaliseArena(); out["arena"]=Hash64(arena.data(),arenaTop); }
                        // grupo normalizado antes de comparar
                        At<uint32>(g,0)=P(At<void*>(g,0)); At<uint32>(g,4)=P(At<void*>(g,4));
                        out["grp"]=ReversingProbe::Bytes(g,0x14); out["grppad"]=Hash64(grpBuf.data(),grpBuf.size());
                        out["fix"]=Hash64(fixBuf.data(),fixBuf.size()); out["hier"]=Hash64(hierBuf.data(),hierBuf.size());
                        out["clump"]=Hash64(clumpBuf.data(),clumpBuf.size());
                        for(auto it=animSaved.rbegin();it!=animSaved.rend();++it)Restore(*it);
                        Restore(lastPosSaved);
                    }
                    out["ret"]=ret; out["recs"]=Recs(); return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original.value("safe",true);
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",std::min(nrec,recs.size())}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            (void)useApi;
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        watchStop=true; if(watch){ WaitForSingleObject(watch,2000); CloseHandle(watch); }
        for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
        for(size_t k=0;k<deps.size();++k)if(depHooks[k]){depHooks[k]->State(depEnabled[k]);depHooks[k]->LockState(depLocked[k]);}
        fixCrash=fixCrashSaved; fixUB=fixUBSaved; fixUnicode=fixUnicodeSaved;
        Restore(menuSaved); Restore(tableSaved); Restore(dirSaved); Restore(lastPosSaved);
        SetEvent(resumeEvt); // el hilo de streaming sigue con todos los hooks como estaban
    }
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
