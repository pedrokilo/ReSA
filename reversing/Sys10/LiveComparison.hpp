#pragma once
// Banco dentro del motor: decodificador MF, fundido de audio, conversaciones, directorio e historial de eventos
// (lista de 10). 46 funciones de CAEMFDecoder, CAESmoothFadeThread, CConversations, CDirectory y
// CEventHandlerHistory. Original (hooks del lote desactivados) frente a candidato con fixtures deterministas.
// Nada toca audio, hilos, Media Foundation ni ficheros reales:
// - CAEMFDecoder trabaja sobre un objeto falso; las virtuales se llaman por el hueco de la vtable del EXE 0x85F370.
//   Initialise/FillBuffer/InitLibrary quedan fuera (QuickTime en el EXE, Media Foundation en la DLL).
// - CAESmoothFadeThread trabaja sobre un objeto falso de 0x81C bytes y IDirectSoundBuffer falsos (vtable propia
//   trazada). CreateThread, SetThreadPriority, ResumeThread, WaitForSingleObject, Sleep y QueryPerformanceCounter
//   se sustituyen por IAT solo en el hilo principal (los demás hilos van a la API real). g_numSoundChannelsUsed se
//   compara por diferencia y se devuelve a su valor tras cada ejecución.
// - CConversations: la región global 0x9691C4..0x969A20 se rellena con fixtures y se restaura; se simulan
//   CEntity::RegisterReference, CConversationForPed::Update/IsPlayerInPositionForConversation y el audio de misión.
// - CDirectory: entradas y directorio falsos; se simulan CFileMgr (abrir, leer, escribir, cerrar), operator
//   new/delete (solo durante la llamada, registro sin reservas) y printf del EXE (sin traza: la DLL usa su log).
// - CEventHandlerHistory: eventos falsos (vtable propia con borrado, tipo, prioridad, Clone y TakesPriorityOver
//   trazados); CTimer::m_snTimeInMilliseconds se fija y se restaura.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Loaders/AEMFDecoder.h"
#include "Audio/AESmoothFadeThread.h"
#include "Audio/AEAudioUtility.h"
#include "Audio/AudioEngine.h"
#include "Conversations/Conversations.h"
#include "Conversations/ConversationForPed.h"
#include "Directory.h"
#include "Events/EventHandlerHistory.h"
#include "Events/Event.h"
#include "FileMgr.h"
#include "Core/KeyGen.h"
#include "../ProbeRuntime.hpp"

namespace Sys10Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

// --- registro sin reservas (mientras operator new/delete están sustituidos) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,256> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }
alignas(16) inline std::array<uint8,0x800> arena{};
alignas(16) inline std::array<uint8,0x40> nameBuf{};
inline uint32 Off(const void* p){
    const auto u=uintptr_t(p);
    if(!u)return 0xFFFFFFFFu;
    if(u>=uintptr_t(arena.data())&&u<uintptr_t(arena.data())+arena.size())return uint32(u-uintptr_t(arena.data()));
    if(u>=uintptr_t(nameBuf.data())&&u<uintptr_t(nameBuf.data())+nameBuf.size())return 0x10000u+uint32(u-uintptr_t(nameBuf.data()));
    return 0xEEEEEEEEu; }
inline void* __cdecl FakeNew(size_t n){ Push(1,uint32(n)); return n<=arena.size() ? arena.data() : nullptr; }
inline void __cdecl FakeDelete(void* p){ Push(2,Off(p)); }
inline int __cdecl FakePrintf(const char*,...){ return 0; } // printf del EXE (stdout sin consola): sin traza

// --- Win32 por IAT (solo el hilo principal; los demás hilos van a la API real) ---
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

// --- CAESmoothFadeThread: objeto, IDirectSoundBuffer falsos y API Win32 ---
alignas(16) inline std::array<uint8,0x81C> sf{};
constexpr int NBUF=5;
alignas(16) inline std::array<std::array<uint8,0x10>,NBUF> dsb{};
inline std::array<void*,21> dsVt{};
inline std::array<uint32,NBUF> dsStatus{}; inline std::array<int32,NBUF> dsVolume{};
inline json Buf(const void* p){ for(int k=0;k<NBUF;++k)if(p==dsb[k].data())return k; return p ? json("other") : json(nullptr); }
inline int BufIdx(const void* p){ for(int k=0;k<NBUF;++k)if(p==dsb[k].data())return k; return 0; }
inline ULONG __stdcall DsAddRef(void* s){ trace.push_back({"AddRef",Buf(s)}); return 2; }
inline ULONG __stdcall DsRelease(void* s){ trace.push_back({"Release",Buf(s)}); return 1; }
inline HRESULT __stdcall DsGetVolume(void* s,LONG* v){ trace.push_back({"GetVolume",Buf(s)}); *v=dsVolume[BufIdx(s)]; return 0; }
inline HRESULT __stdcall DsGetStatus(void* s,DWORD* st){ trace.push_back({"GetStatus",Buf(s)}); *st=dsStatus[BufIdx(s)]; return 0; }
inline HRESULT __stdcall DsSetVolume(void* s,LONG v){ trace.push_back({"SetVolume",Buf(s),v}); return 0; }
inline HRESULT __stdcall DsStop(void* s){ trace.push_back({"Stop",Buf(s)}); return 0; }
inline HRESULT __stdcall DsBad(void* s){ trace.push_back({"bad_slot",Buf(s)}); return 0; }
inline int64 qpcBase{}; inline uint32 qpcCalls{}, sleepCalls{}, sleepLimit{}; inline HANDLE createResult{};
inline uintptr_t dllFadeProc{};
inline BOOL WINAPI FakeQPC(LARGE_INTEGER* p){
    if(!Main())return Real<decltype(&QueryPerformanceCounter)>("QueryPerformanceCounter")(p);
    trace.push_back({"QPC"}); p->QuadPart=qpcBase+int64(qpcCalls++)*37777; return TRUE; }
inline HANDLE WINAPI FakeCreateThread(LPSECURITY_ATTRIBUTES sa,SIZE_T st,LPTHREAD_START_ROUTINE start,LPVOID prm,DWORD fl,LPDWORD id){
    if(!Main())return Real<decltype(&CreateThread)>("CreateThread")(sa,st,start,prm,fl,id);
    const auto s=uintptr_t(start); const json who=(s==0x4EEE90||s==dllFadeProc) ? json("SmoothFadeProc") : json(s<0x1000000 ? s : 0xD11u);
    trace.push_back({"CreateThread",sa!=nullptr,st,who,prm==sf.data(),fl,id==reinterpret_cast<LPDWORD>(sf.data()+4)}); if(id)*id=0x4321; return createResult; }
inline BOOL WINAPI FakeSetThreadPriority(HANDLE h,int p){
    if(!Main())return Real<decltype(&SetThreadPriority)>("SetThreadPriority")(h,p);
    trace.push_back({"SetThreadPriority",uint32(uintptr_t(h)),p}); return TRUE; }
inline DWORD WINAPI FakeResumeThread(HANDLE h){
    if(!Main())return Real<decltype(&ResumeThread)>("ResumeThread")(h);
    trace.push_back({"ResumeThread",uint32(uintptr_t(h))}); return 1; }
inline DWORD WINAPI FakeWait(HANDLE h,DWORD ms){
    if(!Main())return Real<decltype(&WaitForSingleObject)>("WaitForSingleObject")(h,ms);
    trace.push_back({"Wait",uint32(uintptr_t(h)),ms}); return WAIT_OBJECT_0; }
inline void WINAPI FakeSleep(DWORD ms){
    if(!Main()){ Real<decltype(&Sleep)>("Sleep")(ms); return; }
    trace.push_back({"Sleep",ms}); if(++sleepCalls>=sleepLimit)sf[0x809]=0; }
struct FadeFakes {
    IatPatch i1{"CreateThread",FakeCreateThread}, i2{"SetThreadPriority",FakeSetThreadPriority}, i3{"ResumeThread",FakeResumeThread};
    IatPatch i4{"WaitForSingleObject",FakeWait}, i5{"Sleep",FakeSleep}, i6{"QueryPerformanceCounter",FakeQPC};
};
// SmoothFadeProc: el EXE vuelve con ret (sin limpiar) y la DLL con ret 4; se restaura esp en ambos casos
__declspec(noinline) inline uint32 CallThreadProc(uintptr_t fn,void* arg){
    uint32 r=0;
    __asm {
        mov ebx, esp
        push arg
        call dword ptr fn
        mov esp, ebx
        mov r, eax
    }
    return r; }

// --- CConversations ---
constexpr uintptr_t CONV=0x9691C4; constexpr size_t CONV_SIZE=0x969A20-0x9691C4;
constexpr uintptr_t CONV_SLOTS=0x9691D8, CONV_NODES=0x969570, CONV_TEMP=0x969360;
alignas(16) inline std::array<std::array<uint8,0x40>,6> peds{};
inline json Ped(const void* p){ for(int k=0;k<6;++k)if(p==peds[k].data())return k; return p ? json("other") : json(nullptr); }
inline json Slot(const void* p){ const auto u=uintptr_t(p); if(!u)return nullptr; if(u>=CONV_SLOTS&&u<CONV_SLOTS+14*0x1C&&(u-CONV_SLOTS)%0x1C==0)return int((u-CONV_SLOTS)/0x1C); return "other"; }
inline uint32 inPosMask{}; inline int32 loadStatus{}; inline uint32 finished{};
inline void __fastcall FakeRegisterReference(void* self,void*,void** ref){ trace.push_back({"RegisterReference",Ped(self),uint32(uintptr_t(ref))}); }
inline void __fastcall FakeConvUpdate(void* self,void*){ trace.push_back({"ConversationUpdate",Slot(self)}); }
inline bool __fastcall FakeInPosition(void* self,void*,uint32 random){ const bool r=(inPosMask>>(trace.size()%32))&1; trace.push_back({"IsPlayerInPosition",Slot(self),random&0xFF,r}); return r; }
inline void __fastcall FakePreload(void* self,void*,uint32 slot,int32 ev){ trace.push_back({"PreloadMissionAudio",self==&AudioEngine,slot&0xFF,ev}); }
inline CVector* __fastcall FakeAttach(void* self,void*,uint32 slot,void* ped){ trace.push_back({"AttachMissionAudioToPed",self==&AudioEngine,slot&0xFF,Ped(ped)}); return nullptr; }
inline int8 __fastcall FakeLoadStatus(void* self,void*,uint32 slot){ trace.push_back({"GetMissionAudioLoadingStatus",self==&AudioEngine,slot&0xFF}); return int8(loadStatus); }
inline void __fastcall FakePlayLoaded(void* self,void*,uint32 slot){ trace.push_back({"PlayLoadedMissionAudio",self==&AudioEngine,slot&0xFF}); }
inline bool __fastcall FakeFinished(void* self,void*,uint32 slot){ trace.push_back({"IsMissionAudioSampleFinished",self==&AudioEngine,slot&0xFF}); return finished!=0; }

// --- CDirectory ---
alignas(16) inline std::array<uint8,0x10> dir{};
alignas(16) inline std::array<uint8,32*24> ents{};
alignas(16) inline std::array<uint8,32> item{};
alignas(16) inline std::array<uint8,8+32*16> fileData{}; inline size_t fileOff{};
inline uint32 writeMask{};
inline json Ent(const void* p){ const auto u=uintptr_t(p); if(!u)return nullptr; if(u>=uintptr_t(ents.data())&&u<uintptr_t(ents.data())+ents.size())return int(u-uintptr_t(ents.data())); return "other"; }
inline json Str(const char* s){ return s ? json(std::string(s,strnlen(s,64))) : json(nullptr); }
inline FILE* __cdecl FakeOpenFile(const char* p,const char* m){ trace.push_back({"OpenFile",Str(p),Str(m)}); return reinterpret_cast<FILE*>(uintptr_t(0x6A00)); }
inline FILE* __cdecl FakeOpenForWriting(const char* p){ trace.push_back({"OpenFileForWriting",Str(p)}); return reinterpret_cast<FILE*>(uintptr_t(0x6A10)); }
inline size_t __cdecl FakeRead(FILE* f,void* buf,size_t n){
    trace.push_back({"Read",uint32(uintptr_t(f)),n}); auto* b=static_cast<uint8*>(buf);
    for(size_t k=0;k<n;++k){ b[k]=fileOff<fileData.size() ? fileData[fileOff] : 0; ++fileOff; } return n; }
inline size_t __cdecl FakeWrite(FILE* f,const void* buf,size_t n){
    trace.push_back({"Write",uint32(uintptr_t(f)),Ent(buf),n,Fnv(buf,std::min<size_t>(n,ents.size()))});
    switch(writeMask%4){ case 0: return n-1; case 1: return 0; default: return n; } }
inline int32 __cdecl FakeCloseFile(FILE* f){ trace.push_back({"CloseFile",uint32(uintptr_t(f))}); return 0; }

// --- CEventHandlerHistory: eventos falsos ---
alignas(16) inline std::array<std::array<uint8,0x40>,8> evs{};
inline std::array<void*,20> evVt{};
inline uint64 prioMask{}; inline uint32 cloneN{};
alignas(16) inline std::array<uint8,0x1C> hist{};
inline int EvIdx(const void* p){ for(int k=0;k<8;++k)if(p==evs[k].data())return k; return p ? 99 : -1; }
inline void* __fastcall EvDtor(uint8* s,void*,uint32 flag){ trace.push_back({"delete",EvIdx(s),flag}); return s; }
inline int32 __fastcall EvType(uint8* s,void*){ trace.push_back({"type",EvIdx(s)}); return At<int32>(s,0x30); }
inline int32 __fastcall EvPrio(uint8* s,void*){ trace.push_back({"prio",EvIdx(s)}); return At<int32>(s,0x34); }
inline void* __fastcall EvClone(uint8* s,void*){ auto* c=evs[4+cloneN++%4].data(); trace.push_back({"clone",EvIdx(s),EvIdx(c)}); return c; }
inline bool __fastcall EvTakesPriority(uint8* s,void*,uint8* o){ const bool r=(prioMask>>((EvIdx(s)*8+EvIdx(o))&63))&1; trace.push_back({"takes_priority",EvIdx(s),EvIdx(o),r}); return r; }
inline void __fastcall EvBad(uint8* s,void*){ trace.push_back({"bad_slot",EvIdx(s)}); }
inline json EvName(uint32 v){ const int k=EvIdx(reinterpret_cast<void*>(uintptr_t(v))); return k==99 ? json(v) : json(k); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=46;
inline const std::array<Fn,NFN> fns{{
    {"CAEMFDecoder","Constructor",0x4E7770,0,-1,128},{"CAEMFDecoder","Destructor",0x4E77C0,0,-1,128},
    {"CAEMFDecoder","GetStreamLengthMs",0,0x85F370,2,256},{"CAEMFDecoder","GetStreamPlayTimeMs",0,0x85F370,3,1024},
    {"CAEMFDecoder","SetCursor",0,0x85F370,4,1024},{"CAEMFDecoder","GetSampleRate",0,0x85F370,5,256},
    {"CAEMFDecoder","GetStreamID",0,0x85F370,7,128},
    {"CAESmoothFadeThread","CancelFade",0x4EEA50,0,-1,512},{"CAESmoothFadeThread","Initialise",0x4EEEC0,0,-1,256},
    {"CAESmoothFadeThread","InitialiseRequestSlots",0x4EE9F0,0,-1,64},{"CAESmoothFadeThread","RequestFade",0x4EEAA0,0,-1,1024},
    {"CAESmoothFadeThread","Service",0x4EED10,0,-1,1024},{"CAESmoothFadeThread","SetBufferVolume",0x4EEC80,0,-1,1024},
    {"CAESmoothFadeThread","SmoothFadeProc",0x4EEE90,0,-1,256},{"CAESmoothFadeThread","Start",0x4EEA10,0,-1,64},
    {"CAESmoothFadeThread","Stop",0x4EEA30,0,-1,64},{"CAESmoothFadeThread","WaitForExit",0x4EEA40,0,-1,64},
    {"CConversations","AwkwardSay",0x43A810,0,-1,128},{"CConversations","Clear",0x43A7B0,0,-1,64},
    {"CConversations","IsConversationAtNode",0x43B000,0,-1,512},{"CConversations","IsConversationGoingOn",0x43AAC0,0,-1,256},
    {"CConversations","IsPlayerInPositionForConversation",0x43B0B0,0,-1,256},{"CConversations","RemoveConversationForPed",0x43A960,0,-1,512},
    {"CConversations","SetUpConversationNode",0x43A870,0,-1,512},{"CConversations","StartSettingUpConversation",0x43A840,0,-1,128},
    {"CConversations","Update",0x43C590,0,-1,512},
    {"CDirectory","AddItem-",0x532310,0,-1,512},{"CDirectory","Constructor-Capacity",0x5322A0,0,-1,128},
    {"CDirectory","Constructor-Empty",0x532290,0,-1,64},{"CDirectory","Destructor",0x5322D0,0,-1,128},
    {"CDirectory","FindItem-",0x532450,0,-1,512},{"CDirectory","FindItem-ByHash",0x5324D0,0,-1,512},
    {"CDirectory","FindItem-ByName",0x5324A0,0,-1,512},{"CDirectory","Init",0x5322F0,0,-1,64},
    {"CDirectory","ReadDirFile",0x532350,0,-1,256},{"CDirectory","WriteDirFile",0x532410,0,-1,256},
    {"CEventHandlerHistory","ClearAllEvents",0x4BC550,0,-1,256},{"CEventHandlerHistory","ClearNonTempEvent",0x4B8C60,0,-1,128},
    {"CEventHandlerHistory","ClearTempEvent",0x4B8C40,0,-1,128},{"CEventHandlerHistory","GetCurrentEvent",0x4B8CA0,0,-1,128},
    {"CEventHandlerHistory","GetCurrentEventPriority",0x4B8C80,0,-1,256},{"CEventHandlerHistory","IsRespondingToEvent",0x4B8B90,0,-1,512},
    {"CEventHandlerHistory","RecordCurrentEvent",0x4BC4B0,0,-1,1024},{"CEventHandlerHistory","StoreActiveEvent",0x4B8BF0,0,-1,256},
    {"CEventHandlerHistory","TakesPriorityOverCurrentEvent",0x4BC580,0,-1,1024},{"CEventHandlerHistory","TickStoredEvent",0x4B8C20,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"sys10-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"sys10-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("sys10");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"sys10-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtables de la DLL: se leen construyendo y destruyendo un CAEMFDecoder de la DLL (sin nombre ni stream)
    uint32 dllMfVt{}, dllBaseVt{};
    { alignas(16) std::array<uint8,sizeof(CAEMFDecoder)> tmp{}; auto* d=new(tmp.data()) CAEMFDecoder(nullptr,0); dllMfVt=At<uint32>(tmp.data(),0); d->~CAEMFDecoder(); dllBaseVt=At<uint32>(tmp.data(),0); }
    auto vtName=[&](uint32 v)->json{ if(v==0x85F370||v==dllMfVt)return "CAEMFDecoder"; if(v==0x85F418||v==dllBaseVt)return "CAEStreamingDecoder"; return v; };
    dllFadeProc=std::bit_cast<uintptr_t>(&CAESmoothFadeThread::SmoothFadeProc);
    for(auto& s:dsVt)s=reinterpret_cast<void*>(&DsBad);
    dsVt[1]=reinterpret_cast<void*>(&DsAddRef); dsVt[2]=reinterpret_cast<void*>(&DsRelease); dsVt[6]=reinterpret_cast<void*>(&DsGetVolume);
    dsVt[9]=reinterpret_cast<void*>(&DsGetStatus); dsVt[15]=reinterpret_cast<void*>(&DsSetVolume); dsVt[18]=reinterpret_cast<void*>(&DsStop);
    for(auto& s:evVt)s=reinterpret_cast<void*>(&EvBad);
    evVt[0]=reinterpret_cast<void*>(&EvDtor); evVt[1]=reinterpret_cast<void*>(&EvType); evVt[2]=reinterpret_cast<void*>(&EvPrio);
    evVt[4]=reinterpret_cast<void*>(&EvClone); evVt[11]=reinterpret_cast<void*>(&EvTakesPriority);

    // estado global guardado
    auto* conv=reinterpret_cast<uint8*>(CONV); const auto savedConv=ReversingProbe::Bytes(conv,CONV_SIZE);
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& chans=*reinterpret_cast<volatile LONG*>(0xB5F898);
    auto restoreAll=[&]{ std::memcpy(conv,savedConv.data(),CONV_SIZE); timeMs=savedTime; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch c1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegisterReference), c1e(0x571B70,FakeRegisterReference);
        EntryPatch c2(bit_cast<uintptr_t>(&CConversationForPed::Update),FakeConvUpdate), c2e(0x43C190,FakeConvUpdate);
        EntryPatch c3(bit_cast<uintptr_t>(&CConversationForPed::IsPlayerInPositionForConversation),FakeInPosition), c3e(0x43AC40,FakeInPosition);
        EntryPatch a1(bit_cast<uintptr_t>(&CAudioEngine::PreloadMissionAudio),FakePreload), a1e(0x507290,FakePreload);
        EntryPatch a2(bit_cast<uintptr_t>(&CAudioEngine::AttachMissionAudioToPed),FakeAttach), a2e(0x507310,FakeAttach);
        EntryPatch a3(bit_cast<uintptr_t>(&CAudioEngine::GetMissionAudioLoadingStatus),FakeLoadStatus), a3e(0x5072A0,FakeLoadStatus);
        EntryPatch a4(bit_cast<uintptr_t>(&CAudioEngine::PlayLoadedMissionAudio),FakePlayLoaded), a4e(0x5072B0,FakePlayLoaded);
        EntryPatch a5(bit_cast<uintptr_t>(&CAudioEngine::IsMissionAudioSampleFinished),FakeFinished), a5e(0x5072C0,FakeFinished);
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), f1e(0x538900,FakeOpenFile);
        EntryPatch f2(bit_cast<uintptr_t>(&CFileMgr::OpenFileForWriting),FakeOpenForWriting), f2e(0x538910,FakeOpenForWriting);
        EntryPatch f3(bit_cast<uintptr_t>(&CFileMgr::Read),FakeRead), f3e(0x538950,FakeRead);
        EntryPatch f4(bit_cast<uintptr_t>(&CFileMgr::Write),FakeWrite), f4e(0x538970,FakeWrite);
        EntryPatch f5(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), f5e(0x5389D0,FakeCloseFile);
        EntryPatch pr(0x821982,FakePrintf);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            std::unique_ptr<FadeFakes> fadeFakes; if(cls=="CAESmoothFadeThread")fadeFakes=std::make_unique<FadeFakes>();
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5Au; if(!stt)stt=1; trace=json::array(); nrec=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CAEMFDecoder") {
                        alignas(16) static std::array<uint8,sizeof(CAEMFDecoder)> mf{}; /* 0xC4 */
                        for(auto& b:mf)b=uint8(next());
                        At<uint32>(mf.data(),0)=0x85F370; At<uint32>(mf.data(),4)=0; mf[0xC]=uint8(next()%4 ? 1 : 0);
                        static const uint32 divs[]{0x3F800000u,0x42306666u,0x3D34A234u,0x3A83126Fu,0x447A0000u,0u,0xBF800000u,0x0DA24260u,0x7149F2CAu,0x7FC00000u,0x3E2AAAABu,0x00000001u};
                        At<uint32>(mf.data(),0xC0)=next()%3 ? divs[next()%5] : next()%4 ? divs[next()%12] : F(uni(-100.f,100.f));
                        { auto& pt=At<int32>(mf.data(),0xA0); switch(next()%5){ case 0: pt=int32(next()); break; case 1: pt=-int32(next()%100000); break;
                            case 2: pt=int32(0x00FFFFF0u+next()%64); break; default: pt=int32(next()%600000); } }
                        if(nm=="Constructor"){ mf.fill(0xCD); char* name=next()%4 ? reinterpret_cast<char*>(nameBuf.data()+8*(next()%4)) : nullptr; const int32 track=int32(next());
                            ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*,char*,int32)>(a)(mf.data(),name,track))-intptr_t(mf.data()); }
                        else if(nm=="Destructor"){ mf[0xC]=0; At<void*>(mf.data(),8)=next()%4 ? static_cast<void*>(nameBuf.data()+8*(next()%4)) : nullptr; // sin inicializar: QuickTime/MF fuera
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(a)(mf.data()); }
                            out["recs"]=Recs(); }
                        else if(nm=="SetCursor"){ const uint32 pos=next()%3 ? next()%600000 : next()%4 ? next() : 0x80000000u|next();
                            reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(mf.data(),pos); }
                        else ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(mf.data());
                        out["vptr"]=vtName(At<uint32>(mf.data(),0)); At<uint32>(mf.data(),0)=0;
                        out["obj"]=ReversingProbe::Bytes(mf.data(),mf.size());
                    }
                    else if(cls=="CAESmoothFadeThread") {
                        for(auto& b:sf)b=uint8(next());
                        for(int k=0;k<NBUF;++k){ dsb[k].fill(0); At<void*>(dsb[k].data(),0)=dsVt.data();
                            dsStatus[k]=next()%4 ? (1u|(next()%2 ? 8u : 0u)) : next()%16; dsVolume[k]=next()%6 ? -int32(next()%10001) : int32(next()); }
                        qpcBase=int64(next())|(int64(next()%64)<<32); qpcCalls=0; sleepCalls=0; sleepLimit=1+next()%3;
                        createResult=next()%4 ? reinterpret_cast<HANDLE>(uintptr_t(0x6300)) : INVALID_HANDLE_VALUE;
                        const uint32 now=uint32(CAEAudioUtility::GetCurrentTimeInMS()); qpcCalls=0; trace=json::array(); // la primera lectura de la función verá este tiempo
                        sf[0x808]=uint8(next()%6 ? 1 : 0); sf[0x809]=uint8(next()%4 ? 1 : 0); sf[0x814]=uint8(next()%6==0 ? 1 : 0);
                        const uint32 cur=next()%2 ? now : next(); At<uint32>(sf.data(),0x810)=cur;
                        { auto& n=At<uint32>(sf.data(),0x818); switch(next()%4){ case 0: n=64; break; case 1: n=next()%65; break; case 2: n=0x80000000u|next(); break; default: n=1+next()%8; } }
                        const bool svc=nm=="Service"||nm=="SmoothFadeProc";
                        const uint32 base=svc ? now : cur;
                        for(int e=0;e<64;++e){ uint8* en=sf.data()+8+32*e;
                            const uint32 r=next()%10; uint32 st=r<4 ? 0 : r<7 ? 1 : r<9 ? 2 : (next()%2 ? 3 : 0xFFFFFFFFu);
                            if(nm=="SmoothFadeProc"&&next()%2)st=0;
                            At<uint32>(en,0x18)=st;
                            At<void*>(en,0)=next()%6 ? static_cast<void*>(dsb[next()%NBUF].data()) : nullptr;
                            const float start=uni(-100.f,0.f), target=next()%4 ? uni(-100.f,0.f) : -100.f;
                            At<float>(en,4)=start; At<float>(en,8)=target; At<float>(en,0xC)=next()%8 ? target-start : uni(-200.f,200.f);
                            const int bi=BufIdx(At<void*>(en,0));
                            At<float>(en,0x10)=next()%3 ? float(dsVolume[bi])*0.01f+(next()%2 ? 0.f : uni(-0.02f,0.02f)) : uni(-100.f,0.f);
                            en[0x14]=uint8(next()%3==0 ? 1 : 0);
                            At<uint16>(en,0x16)=uint16(next()%4 ? 20+next()%40 : next()%4 ? 0 : next());
                            switch(next()%5){ case 0: At<uint32>(en,0x1C)=base+next()%8; break; case 1: At<uint32>(en,0x1C)=next(); break;
                                case 2: At<uint32>(en,0x1C)=base-At<uint16>(en,0x16); break; default: At<uint32>(en,0x1C)=base-next()%80; } }
                        const LONG chans0=chans;
                        if(nm=="CancelFade"){ void* b=next()%6 ? static_cast<void*>(dsb[next()%NBUF].data()) : nullptr;
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(sf.data(),b); }
                        else if(nm=="Initialise")ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(sf.data());
                        else if(nm=="InitialiseRequestSlots")reinterpret_cast<void(__thiscall*)(void*)>(a)(sf.data());
                        else if(nm=="RequestFade"){ const int bi=int(next()%NBUF); const float fcur=float(dsVolume[bi])*0.01f;
                            static const float deltas[]{0.f,0.005f,-0.005f,0.01f,-0.01f,0.0099f,-0.0101f,0.5f,-3.f};
                            const float target=next()%3 ? fcur+deltas[next()%9] : uni(-100.f,0.f);
                            static const int32 times[]{-2,-1,35,0,1,2,100};
                            const int32 fadeTime=next()%5 ? times[next()%7] : int32(int16(next()));
                            ret=reinterpret_cast<bool(__thiscall*)(void*,void*,float,int32,uint32)>(a)(sf.data(),dsb[bi].data(),target,fadeTime,next()%3==0 ? 1u : 0u); }
                        else if(nm=="Service")reinterpret_cast<void(__thiscall*)(void*)>(a)(sf.data());
                        else if(nm=="SetBufferVolume"){ const int bi=int(next()%NBUF); float vol=uni(-100.f,0.f);
                            if(next()%2){ for(int e=0;e<64;++e){ uint8* en=sf.data()+8+32*e; if(At<void*>(en,0)==dsb[bi].data()){ static const float deltas[]{0.f,0.005f,-0.0099f,0.01f,0.02f}; vol=At<float>(en,8)+deltas[next()%5]; break; } } }
                            reinterpret_cast<void(__thiscall*)(void*,void*,float)>(a)(sf.data(),dsb[bi].data(),vol); }
                        else if(nm=="SmoothFadeProc")ret=CallThreadProc(a,sf.data());
                        else if(nm=="Start")reinterpret_cast<void(__thiscall*)(void*)>(a)(sf.data());
                        else if(nm=="Stop")reinterpret_cast<void(__thiscall*)(void*)>(a)(sf.data());
                        else reinterpret_cast<void(__thiscall*)(void*)>(a)(sf.data());
                        const LONG delta=chans-chans0; InterlockedExchangeAdd(&chans,-delta);
                        // punteros a buffers falsos por índice para el json
                        json entries=json::array();
                        for(int e=0;e<64;++e){ uint8* en=sf.data()+8+32*e; entries.push_back({Buf(At<void*>(en,0)),ReversingProbe::Bytes(en+4,0x1C)}); }
                        out["entries"]=entries; out["head"]=ReversingProbe::Bytes(sf.data(),8); out["tail"]=ReversingProbe::Bytes(sf.data()+0x808,0x14); out["channels"]=delta;
                    }
                    else if(cls=="CConversations") {
                        for(size_t k=0;k<CONV_SIZE;++k)conv[k]=uint8(next());
                        auto pedPtr=[&](int k)->CPed*{ return reinterpret_cast<CPed*>(peds[k].data()); };
                        static const int32 states[]{0,1,2,3,-1};
                        At<int32>(conv,0)=next()%4 ? int32(next()%3) : states[next()%5];
                        At<int32>(conv,4)=int32(next()%12); At<CPed*>(conv,8)=next()%2 ? pedPtr(next()%6) : nullptr; conv[0xC]=uint8(next()%2);
                        for(int s=0;s<14;++s){ uint8* sl=reinterpret_cast<uint8*>(CONV_SLOTS)+0x1C*s;
                            At<int32>(sl,0)=int32(next()%50); At<int32>(sl,4)=next()%4 ? int32(next()%50) : next()%2 ? -1 : -int32(next()%1000);
                            At<CPed*>(sl,8)=next()%3 ? pedPtr(next()%6) : nullptr; At<uint32>(sl,0x14)=next()%5 ? next()%4 : next(); sl[0x18]=uint8(next()%2); sl[0x19]=uint8(next()%2); }
                        static const char charset[]="ABCXYZ019_";
                        for(int n=0;n<50;++n){ uint8* nd=reinterpret_cast<uint8*>(CONV_NODES)+0x18*n;
                            const int len=next()%4==0 ? 0 : 1+int(next()%5); for(int k=0;k<len;++k)nd[k]=uint8(charset[next()%10]); nd[len]=0;
                            if(next()%8==0&&len)nd[next()%len]=uint8('a'+next()%26);
                            auto child=[&]()->int16{ if(next()%5<3)return -1; const int c=n+1+int(next()%6); return int16(c<50 ? c : -1); };
                            At<int16>(nd,8)=child(); At<int16>(nd,0xA)=child(); }
                        if(nm=="AwkwardSay")reinterpret_cast<void(__cdecl*)(int32,CPed*)>(a)(int32(next()),next()%4 ? pedPtr(next()%6) : nullptr);
                        else if(nm=="Clear")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="IsConversationAtNode"){ const int s=int(next()%14); uint8* sl=reinterpret_cast<uint8*>(CONV_SLOTS)+0x1C*s; const int pk=int(next()%6);
                            At<CPed*>(sl,8)=pedPtr(pk); std::array<char,16> name{}; for(auto& c:name)c=char(next());
                            int first=-1; for(int t=0;t<14;++t)if(At<CPed*>(reinterpret_cast<uint8*>(CONV_SLOTS)+0x1C*t,8)==pedPtr(pk)){first=t;break;}
                            const int32 curNode=At<int32>(reinterpret_cast<uint8*>(CONV_SLOTS)+0x1C*first,4);
                            if(curNode>=0&&curNode<50&&next()%3){ const char* nn=reinterpret_cast<const char*>(CONV_NODES+0x18*curNode); const size_t len=strnlen(nn,7);
                                for(size_t k=0;k<len;++k){ char c=nn[k]; if(next()%3==0&&c>='A'&&c<='Z')c=char(c+32); name[k]=c; } name[len]=0; if(next()%6==0&&len)name[len-1]='Q'; }
                            else { const int len=int(next()%7); for(int k=0;k<len;++k)name[k]=charset[next()%10]; name[len]=0; }
                            ret=reinterpret_cast<bool(__cdecl*)(char*,CPed*)>(a)(name.data(),pedPtr(pk)); out["name"]=ReversingProbe::Bytes(name.data(),name.size()); }
                        else if(nm=="IsConversationGoingOn"){ if(next()%2)for(int s=0;s<14;++s)At<uint32>(reinterpret_cast<uint8*>(CONV_SLOTS)+0x1C*s,0x14)=next()%6==0 ? next()%4 : 0;
                            ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                        else if(nm=="IsPlayerInPositionForConversation"){ inPosMask=next(); CPed* p=next()%8 ? pedPtr(next()%6) : nullptr;
                            ret=reinterpret_cast<bool(__cdecl*)(CPed*,uint32)>(a)(p,next()%2); }
                        else if(nm=="RemoveConversationForPed"){ CPed* p=next()%6 ? pedPtr(next()%6) : nullptr;
                            reinterpret_cast<void(__cdecl*)(CPed*)>(a)(p); }
                        else if(nm=="SetUpConversationNode"){ static std::array<std::array<char,16>,3> keys{};
                            for(auto& k:keys){ for(auto& c:k)c=char(next()); const int len=int(next()%11); for(int j=0;j<len;++j){ const uint32 t=next()%4; k[j]=char(t==0 ? 'a'+next()%26 : t==1 ? 'A'+next()%26 : t==2 ? '0'+next()%10 : 0x20+next()%0x5F); } k[len]=0; }
                            const char* y=next()%4 ? keys[1].data() : nullptr; const char* n=next()%4 ? keys[2].data() : nullptr;
                            reinterpret_cast<void(__cdecl*)(const char*,const char*,const char*,int32,int32,int32)>(a)(keys[0].data(),y,n,int32(next()),int32(next()),int32(next())); }
                        else if(nm=="StartSettingUpConversation")reinterpret_cast<void(__cdecl*)(CPed*)>(a)(pedPtr(next()%6));
                        else { static const int32 ls[]{0,1,2,-1,3}; loadStatus=ls[next()%5]; finished=next()%2;
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["globals"]=ReversingProbe::Bytes(conv,CONV_SIZE);
                        std::memcpy(conv,savedConv.data(),CONV_SIZE);
                    }
                    else if(cls=="CDirectory") {
                        for(auto& b:dir)b=uint8(next()); for(auto& b:ents)b=uint8(next()); for(auto& b:item)b=uint8(next()); arena.fill(0xAB);
                        static const char letters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                        auto makeName=[&](char* nmb,int idx,char lead){ int len=0; if(lead)nmb[len++]=lead; const int base=1+int(next()%8);
                            for(int k=0;k<base;++k){ char c=letters[next()%26]; if(next()%3==0)c=char(c+32); nmb[len++]=c; }
                            nmb[len++]=char('0'+idx/10); nmb[len++]=char('0'+idx%10); if(next()%3==0){ nmb[len++]='.'; nmb[len++]='d'; nmb[len++]='f'; nmb[len++]='f'; } nmb[len]=0; };
                        for(int k=0;k<24;++k)makeName(reinterpret_cast<char*>(ents.data()+32*k+8),k,0);
                        makeName(reinterpret_cast<char*>(item.data()+8),int(next()%100),'#');
                        int32 cap=int32(next()%25), cnt=cap ? int32(next()%(cap+1)) : 0;
                        switch(next()%10){ case 0: cnt=int32(0x80000000u+next()%12); break; /* con signo: escribe en la entrada (cnt·32 mod 2^32) */ case 1: cap=-int32(1+next()%100); break; case 2: cnt=cap; break; default: break; }
                        At<void*>(dir.data(),0)=ents.data(); At<int32>(dir.data(),4)=cap; At<int32>(dir.data(),8)=cnt; dir[0xC]=uint8(next()%2);
                        auto* self=dir.data();
                        auto existing=[&](bool inRange)->std::string{ const int32 n=cnt>0&&cnt<=24 ? cnt : 24; const int k=inRange&&n>0 ? int(next()%n) : int(next()%24);
                            std::string s(reinterpret_cast<const char*>(ents.data()+32*k+8)); for(auto& c:s){ if(next()%2&&c>='A'&&c<='Z')c=char(c+32); else if(next()%2&&c>='a'&&c<='z')c=char(c-32); } return s; };
                        if(nm=="AddItem-")reinterpret_cast<void(__thiscall*)(void*,const void*)>(a)(self,item.data());
                        else if(nm=="Constructor-Capacity"){ dir.fill(0xCD); const uint32 n=next()%5==0 ? next() : next()%70;
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*,uint32)>(a)(self,n))-intptr_t(self); }
                            out["recs"]=Recs(); At<uint32>(dir.data(),0)=Off(At<void*>(dir.data(),0)); }
                        else if(nm=="Constructor-Empty"){ dir.fill(0xCD); ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(self))-intptr_t(self); }
                        else if(nm=="Destructor"){ const uint32 w=next()%4; At<void*>(dir.data(),0)=w==0 ? nullptr : w==1 ? static_cast<void*>(ents.data()) : static_cast<void*>(arena.data());
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                            out["recs"]=Recs(); }
                        else if(nm=="FindItem-"||nm=="FindItem-ByName"){ std::string q=next()%4 ? existing(next()%4!=0) : std::string("NOPE")+char('0'+next()%10);
                            if(nm=="FindItem-")ret=Ent(reinterpret_cast<void*(__thiscall*)(void*,const char*)>(a)(self,q.c_str()));
                            else { uint32 pos=0xCDCDCDCDu, size=0xCDCDCDCDu; ret=reinterpret_cast<bool(__thiscall*)(void*,const char*,uint32*,uint32*)>(a)(self,q.c_str(),&pos,&size); out["pos"]=pos; out["size"]=size; } }
                        else if(nm=="FindItem-ByHash"){ const uint32 h=next()%4 ? CKeyGen::GetUppercaseKey(existing(next()%4!=0).c_str()) : next();
                            uint32 pos=0xCDCDCDCDu, size=0xCDCDCDCDu; ret=reinterpret_cast<bool(__thiscall*)(void*,uint32,uint32*,uint32*)>(a)(self,h,&pos,&size); out["pos"]=pos; out["size"]=size; }
                        else if(nm=="Init"){ reinterpret_cast<void(__thiscall*)(void*,int32,void*)>(a)(self,int32(next()),next()%4 ? static_cast<void*>(ents.data()+32*(next()%4)) : nullptr); }
                        else if(nm=="ReadDirFile"){ for(auto& b:fileData)b=uint8(next()); const uint32 n=next()%8==0 ? 0 : 1+next()%12; At<uint32>(fileData.data(),4)=n;
                            for(uint32 k=0;k<16;++k)makeName(reinterpret_cast<char*>(fileData.data()+8+32*k+8),int(k),'#'); fileOff=0;
                            static const char* files[]{"MODELS\\GTA3.IMG","MODELS\\GTA_INT.IMG","anim\\cuts.img","x.dir"};
                            reinterpret_cast<void(__thiscall*)(void*,const char*)>(a)(self,files[next()%4]); }
                        else { writeMask=next(); static const char* files[]{"MODELS\\GTA3.DIR","out.dir"};
                            if(cnt<0||cnt>24)At<int32>(dir.data(),8)=int32(next()%25);
                            ret=reinterpret_cast<bool(__thiscall*)(void*,const char*)>(a)(self,files[next()%2]); }
                        out["dir"]=ReversingProbe::Bytes(dir.data(),dir.size()); out["ents"]=ReversingProbe::Bytes(ents.data(),ents.size()); out["arena"]=Fnv(arena.data(),arena.size());
                    }
                    else { // CEventHandlerHistory
                        static const int32 types[]{1,2,3,4,5,6,0x21,0x38,0x3C,0x2C,0xD,0x2E,0x43,0x5D,0xC,0x4A,0x3B,0x20,0x9,0x1F,0x50,0,-1,0x11};
                        for(int k=0;k<8;++k){ for(auto& b:evs[k])b=uint8(next()); At<void*>(evs[k].data(),0)=evVt.data();
                            At<int32>(evs[k].data(),0x30)=types[next()%24]; At<int32>(evs[k].data(),0x34)=int32(next()%80); }
                        prioMask=(uint64(next())<<32)|next(); cloneN=next()%4;
                        for(auto& b:hist)b=uint8(next());
                        for(size_t off:{size_t(4),size_t(8),size_t(0xC)})At<void*>(hist.data(),off)=next()%3 ? static_cast<void*>(evs[next()%4].data()) : nullptr;
                        if(next()%3==0)At<void*>(hist.data(),0)=evs[next()%4].data();
                        timeMs=next();
                        auto* self=hist.data(); uint8* ev=evs[next()%4].data();
                        if(nm=="ClearAllEvents")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="ClearNonTempEvent")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="ClearTempEvent")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="GetCurrentEvent")ret=EvName(uint32(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self))));
                        else if(nm=="GetCurrentEventPriority")ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(self);
                        else if(nm=="IsRespondingToEvent"){ const int32 t=next()%4==0 ? -1 : next()%5==0 ? int32(next()%100) : At<int32>(evs[next()%4].data(),0x30);
                            ret=reinterpret_cast<bool(__thiscall*)(void*,int32)>(a)(self,t); }
                        else if(nm=="RecordCurrentEvent")reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(a)(self,peds[next()%6].data(),ev);
                        else if(nm=="StoreActiveEvent")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="TakesPriorityOverCurrentEvent")ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(self,ev);
                        else reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,peds[next()%6].data());
                        json h=json::array(); for(size_t off=0;off<0x10;off+=4)h.push_back(EvName(At<uint32>(self,off)));
                        out["hist"]=h; out["timer"]=ReversingProbe::Bytes(self+0x10,0xC);
                        timeMs=savedTime;
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
    const bool restored=ReversingProbe::Bytes(conv,CONV_SIZE)==savedConv&&timeMs==savedTime;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
