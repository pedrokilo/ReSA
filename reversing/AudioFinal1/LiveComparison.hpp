#pragma once
// Banco dentro del motor: CAEAudioHardware::Initialise (EXE 0x4D9930) y CAEFrontendAudioEntity::{Initialise 0x5B9AB0,
// IsLoadingTuneActive 0x4DD470, Reset 0x4DD440} (GTA SA 1.0 US). Original (hook desactivado) frente a candidato.
//
// Seguridad (Initialise de CAEAudioHardware abre el dispositivo de audio real; aqui NUNCA llega a hacerlo):
// - El objeto es un CAEAudioHardware FALSO (0x1100 bytes, relleno pseudoaleatorio por caso). No se toca AEAudioHardware.
// - DirectSoundCreate (importada por el EXE, IAT 0x85801C) y DirectSoundCreate8 (importada por la DLL) se parchean en
//   dsound.dll SOLO durante la llamada y devuelven un dispositivo COM falso (GetCaps, SetCooperativeLevel,
//   GetSpeakerConfig con vtable propia y HRESULT/capacidades guionizados). El dispositivo real nunca se abre.
// - operator new (0x82119A; el de la DLL reenvia a el) se parchea SOLO durante la llamada y reparte bloques de una arena
//   estatica rellena con 0xCD (hilos ajenos: reenvian al original por un trampolin); asi se comparan los canales y
//   cargadores construidos de verdad (constructores del EXE y de la DLL) byte a byte, sin heredar basura del heap.
// - Simulados y trazados con argumentos (EXE y DLL): InitDirectSoundListener, CAEMP3BankLoader/TrackLoader::Initialise,
//   CAEStreamingChannel::Initialise, CAEAudioChannel::SetVolume, CAEUserRadioTrackManager::{ScanUserTracks, Initialise},
//   CAESmoothFadeThread::{Initialise, Start}, CAEStreamThread::{Initialise, Start}. Ningun hilo real se crea ni arranca.
// - El hilo de streaming real se aparca en IsForegroundApp durante todo el banco (como AudioHw54); los falsos ignoran
//   hilos ajenos (Mine()).
// - Frontend: objeto falso de 0x1EC bytes; simulados LoadSoundBank/LoadSound (CAEAudioHardware), GetRandomNumberInRange<int>,
//   AreSoundsOfThisEventPlayingForThisEntity y CancelSoundsOfThisEventPlayingForThisEntity (CAESoundManager).
// - Fuera del banco: el bucle de espera con Sleep(100) con CAEUserRadioTrackManager::m_nUserTracksScanState == IN_PROGRESS
//   (nadie lo cambia sin un hilo de escaneo real), y los vtables (punteros a vtable de los canales) que se enmascaran.
#include <fstream>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/AEAudioUtility.h"
#include "Audio/AESmoothFadeThread.h"
#include "Audio/AEStreamThread.h"
#include "Audio/Hardware/AEAudioChannel.h"
#include "Audio/Hardware/AEStreamingChannel.h"
#include "Audio/Hardware/AEStaticChannel.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Loaders/AEMP3BankLoader.h"
#include "Audio/Loaders/AEMP3TrackLoader.h"
#include "Audio/Managers/AEUserRadioTrackManager.h"
#include "Audio/Managers/AESoundManager.h"
#include "Audio/Entities/AEFrontendAudioEntity.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace AudioFinal1Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& G(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
inline uint64 Fnv(const void* p,size_t n,size_t skipFirst4=0){ uint64 h=1469598103934665603ull; const auto* b=static_cast<const uint8*>(p);
    for(size_t i=0;i<n;++i){ const uint8 c= i<skipFirst4 ? uint8(0) : b[i]; h^=c; h*=1099511628211ull; } return h; }
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

static_assert(offsetof(CAEAudioHardware,m_bInitialised)==0x00);
static_assert(offsetof(CAEAudioHardware,m_IsHardwareMixAvailable)==0x04);
static_assert(offsetof(CAEAudioHardware,m_nNumAvailableChannels)==0x8C);
static_assert(offsetof(CAEAudioHardware,m_nNumChannels)==0x8E);
static_assert(offsetof(CAEAudioHardware,m_pMP3BankLoader)==0xD98);
static_assert(offsetof(CAEAudioHardware,m_pDSDevice)==0xDA0);
static_assert(offsetof(CAEAudioHardware,m_nSpeakerConfig)==0xDA4);
static_assert(offsetof(CAEAudioHardware,m_dsCaps)==0xDAC);
static_assert(offsetof(CAEAudioHardware,m_pStreamingChannel)==0xE10);
static_assert(offsetof(CAEAudioHardware,m_pStreamThread)==0xE14);
static_assert(offsetof(CAEAudioHardware,m_aChannels)==0xE64);
static_assert(offsetof(CAESmoothFadeThread,m_nNumAvailableBuffers)==0xB610E8-0xB608D0);
static_assert(sizeof(DSCAPS)==0x60 && offsetof(DSCAPS,dwFreeHw3DAllBuffers)==0x38);

// --- registro sin reservas (los falsos nunca crean json ni usan memoria dinamica) ---
struct Rec { uint32 t, a, b, c, d; };
inline std::array<Rec,2048> recs{}; inline size_t nrec{};
// Tras el fallo de Initialise de los cargadores MP3 el EXE sale sin mas; la DLL llama antes a NOTSA_LOG_ERR (spdlog reserva
// std::string/buffers con operator new: ruido NOTSA, no hay equivalente en el EXE). Desde ese punto se congela la traza.
inline bool frozen{}; inline size_t frozenUsed{}, frozenBlks{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(!Main()||frozen)return; if(nrec<recs.size())recs[nrec]={t,a,b,c,d}; ++nrec; }
inline json Recs(){
    const size_t n=std::min(nrec,recs.size()); json r=json::array();
    for(size_t i=0;i<n;++i){ const auto& x=recs[i]; r.push_back(json::array({x.t,x.a,x.b,x.c,x.d})); }
    if(nrec>n)r.push_back(json::array({0xFFFFFFFFu,uint32(nrec)}));
    return r; }

struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }

// Parches sin reservas de memoria (el de operator new se instala el ultimo y se retira el primero).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,96> P{}; size_t n{}; std::array<uintptr_t,96> seen{};
    template<class Fn> void Add(uintptr_t at,Fn fake){ for(size_t k=0;k<n;++k)if(seen[k]==at)return; seen[n]=at; P[n++].emplace(at,fake); }
    ~Patches(){ while(n)P[--n].reset(); }
};

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    Push(0x91);
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// --- objetos falsos ---
constexpr size_t HW_SIZE=0x1100, FE_SIZE=sizeof(CAEFrontendAudioEntity), ARENA_SIZE=0x100000;
static_assert(FE_SIZE==0x1EC);
alignas(16) inline std::array<uint8,HW_SIZE> hw{};
alignas(16) inline std::array<uint8,FE_SIZE+16> fe{};
alignas(16) inline std::array<uint8,ARENA_SIZE> arena{};
inline size_t arenaUsed{}; struct Blk { size_t off, size; }; inline std::array<Blk,128> blks{}; inline size_t nblk{};
alignas(16) inline std::array<uintptr_t,4> dev{};
alignas(16) inline std::array<uintptr_t,18> devVt{};

inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    const auto a=uintptr_t(arena.data()); if(u>=a&&u<a+ARENA_SIZE)return 0x10000000u+uint32(u-a);
    const auto h=uintptr_t(hw.data()); if(u>=h&&u<h+HW_SIZE)return 0x20000000u+uint32(u-h);
    const auto f=uintptr_t(fe.data()); if(u>=f&&u<f+FE_SIZE)return 0x40000000u+uint32(u-f);
    if(p==dev.data())return 0x30;
    if(u==0xB5F8B8)return 0x50; if(u==0xB62CB0)return 0x51; if(u==0xB608D0)return 0x52; if(u==0xB6B970)return 0x53;
    return 0xEEEEEEEEu; }

// --- guiones (se fijan por caso, identicos en las dos pasadas) ---
struct Script {
    HRESULT dsHr{}, coopHr{}, capsHr{}; uint32 speaker{}, free3d{}; bool listenerOk{}, bankOk{}, trkOk{};
    uint8 autoScan{}; int32 scanState{}; uint32 fadePre{}; std::array<uint8,0x60> caps{};
    int32 randVal{}; int32 playing{}; // frontend
};
inline Script sc{};

// __declspec(naked): trampolin hacia operator new del EXE saltando los 5 bytes parcheados (push esi; mov esi,[esp+8]).
static __declspec(naked) void* __cdecl OrigNew(size_t) { __asm { push esi
    mov esi,dword ptr [esp+8]
    mov eax,0x82119F
    jmp eax } }
inline void* __cdecl FakeNew(size_t n){
    if(!Main())return OrigNew(n);
    Push(0x01,uint32(n));
    if(nblk>=blks.size()||arenaUsed+n>ARENA_SIZE)return nullptr;
    void* const r=arena.data()+arenaUsed; if(!frozen)blks[nblk++]={arenaUsed,n}; arenaUsed+=(n+15)&~size_t(15); return r; }

inline void Freeze(){ if(!Main()||frozen)return; frozen=true; frozenUsed=arenaUsed; frozenBlks=nblk; }

// --- COM falso del dispositivo (__stdcall; las ranuras 0..2 son IUnknown) ---
inline HRESULT __stdcall DsCreate(const GUID* g,void** pp,void* unk){
    Push(0xC0,uint32(g!=nullptr),uint32(unk!=nullptr),P(pp));
    if(SUCCEEDED(sc.dsHr)&&pp)*pp=dev.data();
    return sc.dsHr; }
inline HRESULT __stdcall ComQI(void* s,const GUID*,void**){ Push(0xA0,P(s)); return E_NOINTERFACE; }
inline ULONG __stdcall ComAddRef(void* s){ Push(0xA1,P(s)); return 1; }
inline ULONG __stdcall ComRelease(void* s){ Push(0xA2,P(s)); return 0; }
inline HRESULT __stdcall DevGetCaps(void* s,DSCAPS* c){
    Push(0xA4,P(s),c ? c->dwSize : 0xFFFFFFFFu,P(c));
    if(c){ const DWORD sz=c->dwSize; std::memcpy(c,sc.caps.data(),sizeof(DSCAPS)); c->dwSize=sz; c->dwFreeHw3DAllBuffers=sc.free3d; }
    return sc.capsHr; }
inline HRESULT __stdcall DevCoop(void* s,HWND h,DWORD level){ Push(0xA6,P(s),uint32(uintptr_t(h)),level); return sc.coopHr; }
inline HRESULT __stdcall DevSpeaker(void* s,DWORD* out){ Push(0xA8,P(s),P(out)); if(out)*out=sc.speaker; return S_OK; }

// --- funciones no virtuales simuladas (EXE + DLL; thiscall = fastcall con 2 primeros registros) ---
inline bool __fastcall HwListener(void* s,void*,uint32 ch,uint32 sps,uint32 bits){ Push(0xC4,P(s),ch,sps,bits); return sc.listenerOk; }
inline void Freeze();
inline bool __fastcall BkInit(void* s,void*){ Push(0xD0,P(s)); if(!sc.bankOk)Freeze(); return sc.bankOk; }
inline bool __fastcall TkInit(void* s,void*){ Push(0xD1,P(s)); if(!sc.trkOk)Freeze(); return sc.trkOk; }
inline void __fastcall StChInit(void* s,void*){ Push(0xD2,P(s)); }
inline void __fastcall ChSetVolume(void* s,void*,float v){ Push(0xD3,P(s),F(v)); }
inline bool __fastcall UrScan(void* s,void*){ Push(0xD4,P(s)); if(Main())G<int32>(0xB6B988)=sc.scanState; return true; }
inline bool __fastcall UrInit(void* s,void*){ Push(0xD5,P(s)); return true; }
inline bool __fastcall SfInit(void* s,void*){ Push(0xD6,P(s)); return true; }
inline void __fastcall SfStart(void* s,void*){ Push(0xD7,P(s)); }
inline bool __fastcall StThInit(void* s,void*,void* ch){ Push(0xD8,P(s),P(ch)); return true; }
inline void __fastcall StThStart(void* s,void*){ Push(0xD9,P(s)); }
// frontend
inline void __fastcall FeLoadBank(void* s,void*,uint32 b,uint32 sl){ Push(0xE1,P(s),b&0xFFFF,sl&0xFFFF); }
inline void __fastcall FeLoadSound(void* s,void*,uint32 b,uint32 id,uint32 sl){ Push(0xE2,P(s),b&0xFFFF,id&0xFFFF,sl&0xFFFF); }
inline int32 __cdecl FeRandom(int32 lo,int32 hi){ Push(0xE0,uint32(lo),uint32(hi)); return sc.randVal; }
inline int32 __fastcall FePlaying(void* s,void*,uint32 ev,void* ent){ Push(0xE3,P(s),ev&0xFFFF,P(ent)); return sc.playing; }
inline void __fastcall FeCancel(void* s,void*,uint32 ev,void* ent){ Push(0xE4,P(s),ev&0xFFFF,P(ent)); }

inline void SetupFakes(){
    using std::bit_cast;
    for(size_t k=0;k<devVt.size();++k)devVt[k]=0xDEAE0000u+uint32(k);
    devVt[0]=bit_cast<uintptr_t>(&ComQI); devVt[1]=bit_cast<uintptr_t>(&ComAddRef); devVt[2]=bit_cast<uintptr_t>(&ComRelease);
    devVt[4]=bit_cast<uintptr_t>(&DevGetCaps); devVt[6]=bit_cast<uintptr_t>(&DevCoop); devVt[8]=bit_cast<uintptr_t>(&DevSpeaker);
    dev[0]=uintptr_t(devVt.data());
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,4> fns{{
    {"CAEAudioHardware","Initialise",0x4D9930,0,-1,1536},
    {"CAEFrontendAudioEntity","Initialise",0x5B9AB0,0,-1,512},
    {"CAEFrontendAudioEntity","IsLoadingTuneActive",0x4DD470,0,-1,128},
    {"CAEFrontendAudioEntity","Reset",0x4DD440,0,-1,128},
}};

inline std::vector<size_t> DiffOffsets(const json& a,const json& b,size_t max=16){
    std::vector<size_t> r; if(!a.is_array()||!b.is_array())return r;
    for(size_t k=0;k<std::min(a.size(),b.size())&&r.size()<max;++k)if(a[k]!=b[k])r.push_back(k);
    return r; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"audiofinal1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"audiofinal1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audiofinal1");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"audiofinal1-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,fns.size()> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    SetupFakes();

    using std::bit_cast;
    // Cuerpos de la DLL resueltos ANTES de escribir ningun parche.
    const uintptr_t dllForeground=Body(bit_cast<uintptr_t>(&IsForegroundApp));
    struct Stub { uintptr_t exe, dll; const void* fake; };
    const std::array<Stub,16> stubs{{
        {0x4D9640,Body(bit_cast<uintptr_t>(&CAEAudioHardware::InitDirectSoundListener)),bit_cast<const void*>(&HwListener)},
        {0x4E08F0,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::Initialise)),bit_cast<const void*>(&BkInit)},
        {0x4E0C50,Body(bit_cast<uintptr_t>(&CAEMP3TrackLoader::Initialise)),bit_cast<const void*>(&TkInit)},
        {0x4F22F0,Body(bit_cast<uintptr_t>(&CAEStreamingChannel::Initialise)),bit_cast<const void*>(&StChInit)},
        {0x4D7C60,Body(bit_cast<uintptr_t>(&CAEAudioChannel::SetVolume)),bit_cast<const void*>(&ChSetVolume)},
        {0x4F4BA0,Body(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::ScanUserTracks)),bit_cast<const void*>(&UrScan)},
        {0x4F35B0,Body(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::Initialise)),bit_cast<const void*>(&UrInit)},
        {0x4EEEC0,Body(bit_cast<uintptr_t>(&CAESmoothFadeThread::Initialise)),bit_cast<const void*>(&SfInit)},
        {0x4EEA10,Body(bit_cast<uintptr_t>(&CAESmoothFadeThread::Start)),bit_cast<const void*>(&SfStart)},
        {0x4F1680,Body(bit_cast<uintptr_t>(&CAEStreamThread::Initialise)),bit_cast<const void*>(&StThInit)},
        {0x4F11F0,Body(bit_cast<uintptr_t>(&CAEStreamThread::Start)),bit_cast<const void*>(&StThStart)},
        {0x4D88A0,Body(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank)),bit_cast<const void*>(&FeLoadBank)},
        {0x4D8ED0,Body(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSound)),bit_cast<const void*>(&FeLoadSound)},
        {0x4D9C10,Body(bit_cast<uintptr_t>(&CAEAudioUtility::GetRandomNumberInRange<int32>)),bit_cast<const void*>(&FeRandom)},
        {0x4EF570,Body(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity)),bit_cast<const void*>(&FePlaying)},
        {0x4EFB90,Body(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntity)),bit_cast<const void*>(&FeCancel)}
    }};
    // dsound: la IAT del EXE (0x85801C) ya esta resuelta; la DLL importa DirectSoundCreate8. Se parchean las funciones reales.
    const uintptr_t exeDs=G<uintptr_t>(0x85801C);
    const HMODULE dsMod=GetModuleHandleA("dsound.dll");
    const uintptr_t dllDs8=dsMod ? uintptr_t(GetProcAddress(dsMod,"DirectSoundCreate8")) : 0;
    if(!dsMod||exeDs<0x1000000||dllDs8<0x1000000){Write({{"event","error"},{"reason","dsound not resolved"},{"exe_ds",exeDs},{"dll_ds8",dllDs8}});return;}
    const size_t NST=stubs.size();
    for(size_t k=0;k<NST;++k)if(stubs[k].dll>=0x400000&&stubs[k].dll<0x1000000){Write({{"event","error"},{"reason","dll body inside exe"},{"exe",stubs[k].exe}});return;}

    // estado global vigilado (las entradas parcheadas deben quedar como estaban)
    std::vector<Saved> savedAll{ Save(0x82119A,5), Save(exeDs,5), Save(dllDs8,5) };
    for(size_t k=0;k<NST;++k){ savedAll.push_back(Save(stubs[k].exe,5)); savedAll.push_back(Save(stubs[k].dll,5)); }
    const Saved gFade=Save(0xB610E8,4), gAuto=Save(0xBA680D,1), gScan=Save(0xB6B988,4);

    parkedEvt=CreateEventA(nullptr,FALSE,FALSE,nullptr); resumeEvt=CreateEventA(nullptr,TRUE,FALSE,nullptr);
    const HANDLE streamThread=AEAudioHardware.m_pStreamThread.m_nHandle;
    const bool streamRunning=streamThread&&WaitForSingleObject(streamThread,0)==WAIT_TIMEOUT;
    streamTid=streamRunning ? GetThreadId(streamThread) : 0;
    bool parkedOk=!streamRunning;
    std::array<bool,fns.size()> enabled{},locked{};
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch fg1(0x746060,FakeForeground), fg2(dllForeground,FakeForeground);
        if(streamRunning)parkedOk=WaitForSingleObject(parkedEvt,3000)==WAIT_OBJECT_0;
        if(!parkedOk){ SetEvent(resumeEvt); Write({{"event","error"},{"reason","audio stream thread not parked"},{"state_restored",true}}); return; }
        Write({{"event","stream_thread"},{"running",streamRunning},{"parked",streamRunning},{"exe_ds",exeDs},{"dll_ds8",dllDs8}});
        for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool isHw=cls=="CAEAudioHardware";
            int okCount=0, mixCount=0, noMixCount=0, failCount=0, scanCount=0, signCount=0, activeCount=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA54u; if(!stt)stt=1;
                auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto fill=[](uint32 seed,void* p,size_t n){ uint32 s=seed?seed:1; auto* w=static_cast<uint8*>(p);
                    auto nx=[&]{s^=s<<13;s^=s>>17;s^=s<<5;return s;};
                    for(size_t k=0;k+4<=n;k+=4){ const uint32 v=nx(); std::memcpy(w+k,&v,4); } for(size_t k=n&~size_t(3);k<n;++k)w[k]=uint8(nx()); };
                // --- guion del caso ---
                sc={};
                const uint32 fillSeed=next();
                if(isHw){
                    static const std::array<uint32,12> free3d{0u,1u,0x17u,0x18u,0x19u,0x1Fu,0x30u,0x3Fu,0x40u,0x41u,0x1000u,0xFFFFFFFFu};
                    static const std::array<int32,6> scanStates{0,2,3,4,-1,0x7FFFFFFF};
                    sc.dsHr   = next()%8==0 ? HRESULT(0x80004005) : next()%7==0 ? S_FALSE : S_OK;
                    sc.coopHr = next()%8==0 ? HRESULT(0x88780078) : next()%7==0 ? S_FALSE : S_OK;
                    sc.capsHr = next()%4==0 ? HRESULT(0x80004005) : S_OK;
                    sc.speaker = next(); sc.free3d = free3d[size_t(i)%free3d.size()];
                    sc.listenerOk = next()%8!=0; sc.bankOk = next()%8!=0; sc.trkOk = next()%8!=0;
                    sc.autoScan = next()%3==0 ? uint8(0) : next()%4==0 ? uint8(0x80) : uint8(1);
                    sc.scanState = scanStates[next()%scanStates.size()]; sc.fadePre = next();
                    fill(next(),sc.caps.data(),sc.caps.size());
                } else {
                    static const std::array<int32,14> seeds{0,1,2,3,126,127,128,129,200,255,256,300,-1,0x1234};
                    sc.randVal = seeds[size_t(i)%seeds.size()];
                    sc.playing = next()%2;
                }
                auto run=[&](bool candidate,bool verbose)->json {
                    nrec=0; frozen=false; frozenUsed=frozenBlks=0; hooks[f]->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                    json out={{"switches",switches}}; json ret=nullptr;
                    uint32 retv=0;
                    if(isHw){
                        fill(fillSeed,hw.data(),HW_SIZE);
                        std::memset(arena.data(),0xCD,ARENA_SIZE); arenaUsed=0; nblk=0;
                        G<uint32>(0xB610E8)=sc.fadePre; G<uint8>(0xBA680D)=sc.autoScan; G<int32>(0xB6B988)=int32(0x5A5A5A5A);
                        {   Patches pt;
                            for(size_t k=0;k<NST;++k){ pt.Add(stubs[k].exe,stubs[k].fake); pt.Add(stubs[k].dll,stubs[k].fake); }
                            pt.Add(exeDs,DsCreate); pt.Add(dllDs8,DsCreate);
                            pt.Add(0x82119A,FakeNew);                                    // el ultimo: sin reservas hasta aqui
                            retv=uint32(reinterpret_cast<uint8(__thiscall*)(void*)>(fn.address)(hw.data()))&0xFF;
                        }   // ... y el primero en retirarse
                        out["hw_fnv"]=Fnv(hw.data(),HW_SIZE);
                        json blocks=json::array();
                        for(size_t k=0;k<nblk;++k){
                            const uint8* b=arena.data()+blks[k].off; const size_t sz=blks[k].size;
                            // el puntero a vtable (dword 0) de los canales difiere entre el constructor del EXE y el de la DLL
                            const bool channel=sz==0x90||sz==0x60098;
                            blocks.push_back(json::array({blks[k].off,sz,Fnv(b,sz,channel ? 4 : 0)}));
                        }
                        out["blocks"]=blocks; out["arena_used"]=frozen ? frozenUsed : arenaUsed;
                        out["fade"]=G<uint32>(0xB610E8); out["scan_state"]=G<int32>(0xB6B988);
                        if(verbose){
                            out["hw"]=ReversingProbe::Bytes(hw.data(),HW_SIZE);
                            json detail=json::array();
                            for(size_t k=0;k<std::min<size_t>(nblk,4);++k){
                                const uint8* b=arena.data()+blks[k].off; const size_t sz=blks[k].size;
                                auto head=ReversingProbe::Bytes(b,std::min<size_t>(sz,0xA0)); if(sz==0x90||sz==0x60098)head[0]=head[1]=head[2]=head[3]=0;
                                detail.push_back({{"off",blks[k].off},{"size",sz},{"head",head},{"tail",ReversingProbe::Bytes(b+sz-std::min<size_t>(sz,0x40),std::min<size_t>(sz,0x40))}});
                            }
                            out["detail"]=detail;
                        }
                    } else {
                        fill(fillSeed,fe.data(),FE_SIZE); std::memset(fe.data()+FE_SIZE,0xCD,16);
                        {   Patches pt; for(size_t k=0;k<NST;++k){ pt.Add(stubs[k].exe,stubs[k].fake); pt.Add(stubs[k].dll,stubs[k].fake); }
                            retv=uint32(reinterpret_cast<uint8(__thiscall*)(void*)>(fn.address)(fe.data()))&0xFF; }
                        out["fe_fnv"]=Fnv(fe.data(),FE_SIZE+16);
                        if(verbose)out["fe"]=ReversingProbe::Bytes(fe.data(),FE_SIZE+16);
                    }
                    ret=(nm=="Initialise"&&!isHw) || nm=="Reset" ? json(nullptr) : json(retv);   // void: el valor de AL no cuenta
                    out["ret"]=ret; out["recs"]=Recs(); return out;
                };
                const auto original=run(false,false), candidate=run(true,false);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                // cobertura (del original): caminos realmente recorridos
                if(isHw){
                    const auto& r=original["recs"]; bool sawStream=false; for(auto& e:r)if(e[0]==0xD9)sawStream=true;
                    if(sawStream){ ++okCount; if(sc.free3d>=0x18)++mixCount; else ++noMixCount; } else ++failCount;
                    bool sawScan=false; for(auto& e:r)if(e[0]==0xD4)sawScan=true; scanCount+=sawScan;
                } else if(nm=="Initialise"){ signCount+= uint8(sc.randVal)>=128; }
                else if(nm=="IsLoadingTuneActive"){ activeCount+= sc.playing!=0; }
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate},{"events",std::min(nrec,recs.size())}});
                else {
                    json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    json vo=nullptr, vc=nullptr, hwDiff=json::array();
                    if(failures<=2){ vo=run(false,true); vc=run(true,true);
                        if(vo.contains("hw")){ for(auto k:DiffOffsets(vo["hw"],vc["hw"],24))hwDiff.push_back(k); }
                        if(vo.contains("fe")){ for(auto k:DiffOffsets(vo["fe"],vc["fe"],24))hwDiff.push_back(k); } }
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"object_diff_offsets",hwDiff},
                           {"fixture",{{"ds_hr",sc.dsHr},{"coop_hr",sc.coopHr},{"free3d",sc.free3d},{"listener",sc.listenerOk},{"bank",sc.bankOk},{"trk",sc.trkOk},{"auto_scan",sc.autoScan},{"scan_state",sc.scanState},{"rand",sc.randVal},{"playing",sc.playing}}},
                           {"original",failures<=2 ? (vo.is_null() ? original : vo):json(nullptr)},{"candidate",failures<=2 ? (vc.is_null() ? candidate : vc):json(nullptr)}}); }
            }
            bool coverage=true; json cov={{"ok_paths",okCount},{"fail_paths",failCount},{"hardware_mix",mixCount},{"software_mix",noMixCount},{"scans",scanCount},{"seed_ge_128",signCount},{"playing",activeCount}};
            if(isHw)coverage=okCount>=50&&failCount>=50&&mixCount>=20&&noMixCount>=20&&scanCount>=10;
            else if(nm=="Initialise")coverage=signCount>=10;
            else if(nm=="IsLoadingTuneActive")coverage=activeCount>=10&&activeCount<fn.cases-10;
            cov["accepted"]=coverage; if(!coverage){passed=false;++failures;}
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases},{"coverage",cov}});
        }
        for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
        SetEvent(resumeEvt); // el hilo de streaming sigue con todos los hooks como estaban
    }
    // restaurar globales tocados por el banco
    std::memcpy(reinterpret_cast<void*>(gFade.at),gFade.bytes.data(),4); std::memcpy(reinterpret_cast<void*>(gAuto.at),gAuto.bytes.data(),1); std::memcpy(reinterpret_cast<void*>(gScan.at),gScan.bytes.data(),4);
    bool restored=Same(gFade)&&Same(gAuto)&&Same(gScan); for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
