#pragma once
// Banco dentro del motor: CAEAudioHardware (lista de 34), 53 de sus 54 funciones (Initialise queda fuera).
// Original (hooks de la clase desactivados) frente a candidato con fixtures deterministas:
// - Todo corre sobre un CAEAudioHardware falso (0x1100 bytes rellenos al azar por caso): nunca se toca AEAudioHardware,
//   DirectSound, EAX, los cargadores, los hilos ni los ficheros reales.
// - El hilo de streaming real se aparca en IsForegroundApp (0x746060 + símbolo de la DLL) durante todo el banco, como
//   FixAudio; en el hilo principal ese falso solo registra la llamada (Service la hace).
// - Canales falsos (64 objetos de 0x90) con vtable falsa de 9 huecos trazados (destructor, Service, IsSoundPlaying,
//   GetPlayTime/GetLength de 32 bits guionizados por canal y por llamada, Play, SynchPlayback, Stop,
//   SetFrequencyScalingFactor); el canal 0 hace también de canal de streaming.
// - Se simulan y registran en el EXE y en la DLL: CAEAudioChannel::SetVolume/SetPosition/SetReverbAndDepth,
//   CAEStreamingChannel::SetBassEQ, los métodos de CAEMP3BankLoader y CAEMP3TrackLoader que se llaman, los de
//   CAEStreamThread (sobre el hilo incrustado en el objeto falso), CAESmoothFadeThread::Stop/WaitForExit y
//   CAEAudioEnvironment::GetReverbEnvironmentAndDepth. operator delete (0x8214BD) y CMemoryMgr::Free solo se
//   parchean durante la llamada de GetBeatInfo/Terminate, con registro sin reservas.
// - DirectSound/EAX: dispositivo, buffer, buffer 3D, IKsPropertySet y oyente falsos con vtables COM propias; los HRESULT
//   y los punteros de salida salen de un guion consumido en orden de llamada.
// - El hook de CAEAudioUtility::AudioLog10 se desactiva durante todo el banco para que el RescaleChannelVolumes del EXE
//   use la función del EXE (devuelve el log en ST0 sin redondear); se restaura al final.
// - Fuera de los fixtures: NaN/infinitos en volúmenes y factores, longitud 0 (16 bits bajos) en canales con
//   FLAG_FADE_NEAR_END (división por cero x87), canales nulos donde el EXE no los comprueba (GetChannelPlayTimes,
//   PauseAllSounds, bucles de Service), índices de canal fuera del objeto, y sNaN.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/AEAudioUtility.h"
#include "Audio/AEAudioEnvironment.h"
#include "Audio/AESmoothFadeThread.h"
#include "Audio/AEStreamThread.h"
#include "Audio/Hardware/AEAudioChannel.h"
#include "Audio/Hardware/AEStreamingChannel.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Loaders/AEMP3BankLoader.h"
#include "Audio/Loaders/AEMP3TrackLoader.h"
#include "MemoryMgr.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace AudioHw54Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& G(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

// --- registro sin reservas (los falsos nunca crean json) ---
struct Rec { uint32 t, a, b, c, d; };
inline std::array<Rec,2048> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(!Main())return; if(nrec<recs.size())recs[nrec]={t,a,b,c,d}; ++nrec; }
inline json Recs(){
    const size_t n=std::min(nrec,recs.size()); json r=json::array();
    for(size_t i=0;i<n;++i){ const auto& x=recs[i]; r.push_back(json::array({x.t,x.a,x.b,x.c,x.d})); }
    if(nrec>n)r.push_back(json::array({0xFFFFFFFFu,uint32(nrec)}));
    return r; }

struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }

struct Patches {
    std::array<std::unique_ptr<ReversingProbe::EntryPatch>,64> P{}; size_t n{}; std::array<uintptr_t,64> seen{};
    template<class Fn> void Add(uintptr_t at,Fn fake){ for(size_t k=0;k<n;++k)if(seen[k]==at)return; seen[n]=at; P[n++]=std::make_unique<ReversingProbe::EntryPatch>(at,fake); }
    ~Patches(){ while(n)P[--n].reset(); }
};

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    Push(0x91);
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// --- objetos falsos ---
constexpr size_t HW_SIZE=0x1100, NCH=64, CH_SIZE=0x90, NBEAT=1100;
alignas(16) inline std::array<uint8,HW_SIZE> hw{};
alignas(16) inline std::array<std::array<uint8,CH_SIZE>,NCH> chans{};
alignas(16) inline std::array<uintptr_t,12> chVt{};
alignas(16) inline std::array<uint8,0x6E4> bank{};
alignas(16) inline std::array<uint8,0x40> trk{};
alignas(16) inline std::array<char,16> dvdPath{};
alignas(16) inline std::array<uint32,2*NBEAT+16> beats{};
alignas(16) inline std::array<uint8,0x100> outBuf{};
alignas(16) inline std::array<uint8,0x300> outArr{};
// COM: dispositivo, buffer, buffer 3D, IKsPropertySet, oyente (cada uno con su vtable de 18 huecos)
alignas(16) inline std::array<uintptr_t,4> dev{}, buf{}, b3d{}, prop{}, lis{};
alignas(16) inline std::array<uintptr_t,18> devVt{}, bufVt{}, b3dVt{}, propVt{}, lisVt{};

inline int Idx(const void* p){ const auto d=intptr_t(p)-intptr_t(chans.data()); return d>=0&&d<intptr_t(NCH*CH_SIZE)&&d%CH_SIZE==0 ? int(d/CH_SIZE) : -1; }
inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    if(const int k=Idx(p);k>=0)return 0x100+uint32(k);
    const auto h=uintptr_t(hw.data()); if(u>=h&&u<h+HW_SIZE)return 0x10000+uint32(u-h);
    if(p==bank.data())return 0x20; if(p==trk.data())return 0x21; if(p==beats.data())return 0x22; if(p==dvdPath.data())return 0x23;
    if(p==dev.data())return 0x30; if(p==buf.data())return 0x31; if(p==b3d.data())return 0x32; if(p==prop.data())return 0x33; if(p==lis.data())return 0x34;
    if(u==0xB608D0)return 0x40;
    return 0xEEEEEEEEu; }

// --- guiones (se fijan por caso, idénticos en las dos pasadas) ---
inline std::array<std::array<int32,2>,NCH> chPt{}; inline std::array<uint32,NCH> chPtN{}; inline std::array<int32,NCH> chLen{}; inline std::array<bool,NCH> chRev{};
inline bool bankBool{}, trkAvail{}; inline float headroom{}; inline void* trkInfoRet{};
inline int32 stActive{}, stPlay{}, stLen{}, stPlaying{}; inline int8 envRev{}; inline int32 envDepth{};
inline std::array<HRESULT,24> hrs{}; inline std::array<bool,24> outNull{}; inline uint32 hrIdx{};

// --- canales: vtable falsa ---
inline void* __fastcall ChDtor(void* s,void*,uint32 f){ Push(0x40,P(s),f); return s; }
inline void __fastcall ChService(void* s,void*){ Push(0x41,P(s)); }
inline bool __fastcall ChIsPlaying(void* s,void*){ Push(0x42,P(s)); return false; }
inline int32 __fastcall ChPlayTime(void* s,void*){ const int k=Idx(s); Push(0x43,P(s)); return k>=0 ? chPt[k][chPtN[k]++&1] : 0; }
inline int32 __fastcall ChLength(void* s,void*){ const int k=Idx(s); Push(0x44,P(s)); return k>=0 ? chLen[k] : 0; }
inline void __fastcall ChPlay(void* s,void*,uint32 off,uint32 fl,float fr){ Push(0x45,P(s),off&0xFFFF,fl&0xFF,F(fr)); }
inline void __fastcall ChSynch(void* s,void*){ Push(0x46,P(s)); }
inline void __fastcall ChStop(void* s,void*){ Push(0x47,P(s)); }
inline void __fastcall ChSetFreq(void* s,void*,float f){ Push(0x48,P(s),F(f)); }
// --- funciones no virtuales simuladas (EXE + DLL) ---
inline void __fastcall FakeSetVolume(void* s,void*,float v){ Push(0x50,P(s),F(v)); }
inline void __fastcall FakeSetPosition(void* s,void*,const float* v){ Push(0x51,P(s),F(v[0]),F(v[1]),F(v[2])); }
inline bool __fastcall FakeSetReverb(void* s,void*,uint32 env,uint32 depth){ const int k=Idx(s); Push(0x52,P(s),env&0xFF,depth); return k>=0&&chRev[k]; }
inline void __fastcall FakeSetBassEQ(void* s,void*,uint32 mode,float gain){ Push(0x53,P(s),mode&0xFF,F(gain)); }
inline void __fastcall BkLoadBank(void* s,void*,uint32 b,uint32 sl){ Push(0x60,P(s),b&0xFFFF,sl&0xFFFF); }
inline bool __fastcall BkIsBankLoaded(void* s,void*,uint32 b,uint32 sl){ Push(0x61,P(s),b&0xFFFF,sl&0xFFFF); return bankBool; }
inline bool __fastcall BkBankStatus(void* s,void*,uint32 b,uint32 sl){ Push(0x62,P(s),b&0xFFFF,sl&0xFFFF); return bankBool; }
inline void __fastcall BkLoadSound(void* s,void*,uint32 b,uint32 x,uint32 sl){ Push(0x63,P(s),b&0xFFFF,x&0xFFFF,sl&0xFFFF); }
inline bool __fastcall BkIsSoundLoaded(void* s,void*,uint32 b,uint32 x,uint32 sl){ Push(0x64,P(s),b&0xFFFF,x&0xFFFF,sl&0xFFFF); return bankBool; }
inline bool __fastcall BkSoundStatus(void* s,void*,uint32 b,uint32 x,uint32 sl){ Push(0x65,P(s),b&0xFFFF,x&0xFFFF,sl&0xFFFF); return bankBool; }
inline float __fastcall BkHeadroom(void* s,void*,uint32 x,uint32 sl){ Push(0x66,P(s),x&0xFFFF,sl&0xFFFF); return headroom; }
inline void __fastcall BkUpdateVc(void* s,void*,void* a,void* b,void* c){ Push(0x67,P(s),P(a),P(b),P(c)); }
inline void __fastcall BkService(void* s,void*){ Push(0x68,P(s)); }
inline void* __fastcall TkGetTrackInfo(void* s,void*,uint32 id){ Push(0x70,P(s),id); return trkInfoRet; }
inline bool __fastcall TkAvailable(void* s,void*){ Push(0x71,P(s)); return trkAvail; }
inline void __fastcall StPlayTrack(void* s,void*,uint32 id,uint32 next,uint32 off,uint32 fl,uint32 u1,uint32 u2){ Push(0x80,P(s),id,next,off); Push(0x81,fl&0xFF,u1&0xFF,u2&0xFF); }
inline void __fastcall StStopTrack(void* s,void*){ Push(0x82,P(s)); }
inline int32 __fastcall StGetPlayTime(void* s,void*){ Push(0x83,P(s)); return stPlay; }
inline int32 __fastcall StGetLength(void* s,void*){ Push(0x84,P(s)); return stLen; }
inline int32 __fastcall StGetActive(void* s,void*){ Push(0x85,P(s)); return stActive; }
inline int32 __fastcall StGetPlaying(void* s,void*){ Push(0x86,P(s)); return stPlaying; }
inline void __fastcall StStop(void* s,void*){ Push(0x87,P(s)); }
inline void __fastcall StWait(void* s,void*){ Push(0x88,P(s)); }
inline void __fastcall SfStop(void* s,void*){ Push(0x89,P(s)); }
inline void __fastcall SfWait(void* s,void*){ Push(0x8A,P(s)); }
inline void __cdecl FakeGetReverb(int8* r,int32* d){ Push(0x90); *r=envRev; *d=envDepth; }
inline void __cdecl FakeDelete(void* p){ Push(0x92,P(p)==0xEEEEEEEEu ? uint32(uintptr_t(p)) : P(p)); }
inline void __cdecl FakeFree(void* p){ Push(0x93,uint32(uintptr_t(p))); }

// --- COM falso (__stdcall) ---
inline uint32 Iid(const void* g){
    if(!std::memcmp(g,reinterpret_cast<const void*>(0x859390),16))return 1; // IID_IDirectSound3DBuffer
    if(!std::memcmp(g,reinterpret_cast<const void*>(0x8593A0),16))return 2; // IID_IDirectSound3DListener
    if(!std::memcmp(g,reinterpret_cast<const void*>(0x859340),16))return 3; // IID_IKsPropertySet
    if(!std::memcmp(g,reinterpret_cast<const void*>(0x85EFEC),16))return 4; // EAX context
    if(!std::memcmp(g,reinterpret_cast<const void*>(0x85EFDC),16))return 5; // EAX FX slot 0
    return 0xEE; }
inline HRESULT NextHr(bool& isNull){ const uint32 k=hrIdx++%hrs.size(); isNull=outNull[k]; return hrs[k]; }
inline HRESULT __stdcall ComQI(void* s,const GUID* riid,void** ppv){
    const uint32 code=Iid(riid); Push(0xA0,P(s),code); bool isNull; const HRESULT hr=NextHr(isNull);
    if(SUCCEEDED(hr)){ void* o=code==1 ? b3d.data() : code==2 ? static_cast<void*>(lis.data()) : code==3 ? static_cast<void*>(prop.data()) : nullptr;
        *ppv=(isNull&&code==1) ? nullptr : o; } // solo el buffer 3D puede salir nulo (el EXE comprueba ese puntero)
    return hr; }
inline ULONG __stdcall ComAddRef(void* s){ Push(0xA1,P(s)); return 1; }
inline ULONG __stdcall ComRelease(void* s){ Push(0xA2,P(s)); return 0; }
inline void PushFormat(uint32 t,const WAVEFORMATEX* f){
    if(!f){ Push(t,0xFFFFFFFFu); return; }
    Push(t,f->wFormatTag|uint32(f->nChannels)<<16,f->nSamplesPerSec,f->nAvgBytesPerSec,f->nBlockAlign|uint32(f->wBitsPerSample)<<16); Push(t+1,f->cbSize); }
inline HRESULT __stdcall DevCreate(void* s,const DSBUFFERDESC* d,void** pp,void* unk){
    Push(0xA3,P(s),d->dwSize,d->dwFlags,d->dwBufferBytes); const auto* g=reinterpret_cast<const uint32*>(&d->guid3DAlgorithm);
    Push(0xA4,d->dwReserved,uint32(unk!=nullptr),g[0]|g[1],g[2]|g[3]); PushFormat(0xA5,d->lpwfxFormat);
    bool isNull; const HRESULT hr=NextHr(isNull); if(SUCCEEDED(hr))*pp=buf.data(); return hr; } // nunca nulo: el EXE llama a QueryInterface sin comprobarlo
inline HRESULT __stdcall BufSetFormat(void* s,const WAVEFORMATEX* f){ Push(0xA7,P(s)); PushFormat(0xA5,f); bool n; return NextHr(n); }
inline HRESULT __stdcall PropSet(void* s,const GUID* g,ULONG id,void* inst,ULONG instLen,void* data,ULONG len){
    Push(0xA8,P(s),Iid(g),id,instLen|(inst ? 0x80000000u : 0u));
    std::array<uint32,4> v{}; if(data)std::memcpy(v.data(),data,std::min<size_t>(len,16)); Push(0xA9,len,v[0],v[1],v[2]|v[3]);
    bool n; return NextHr(n); }
inline HRESULT __stdcall LisDoppler(void* s,float f,DWORD a){ Push(0xB0,P(s),F(f),a); return S_OK; }
inline HRESULT __stdcall LisOrient(void* s,float a,float b,float c,float d,float e,float f,DWORD ap){ Push(0xB1,F(a),F(b),F(c)); Push(0xB1,F(d),F(e),F(f),ap); return S_OK; }
inline HRESULT __stdcall LisPosition(void* s,float x,float y,float z,DWORD a){ Push(0xB2,F(x),F(y),F(z),a); return S_OK; }
inline HRESULT __stdcall LisRolloff(void* s,float f,DWORD a){ Push(0xB3,P(s),F(f),a); return S_OK; }
inline HRESULT __stdcall LisCommit(void* s){ Push(0xB4,P(s)); return S_OK; }

inline void SetupFakes(){
    for(size_t k=0;k<chVt.size();++k)chVt[k]=0xDEAD0000u+uint32(k);
    using std::bit_cast;
    chVt[0]=bit_cast<uintptr_t>(&ChDtor); chVt[1]=bit_cast<uintptr_t>(&ChService); chVt[2]=bit_cast<uintptr_t>(&ChIsPlaying);
    chVt[3]=bit_cast<uintptr_t>(&ChPlayTime); chVt[4]=bit_cast<uintptr_t>(&ChLength); chVt[5]=bit_cast<uintptr_t>(&ChPlay);
    chVt[6]=bit_cast<uintptr_t>(&ChSynch); chVt[7]=bit_cast<uintptr_t>(&ChStop); chVt[8]=bit_cast<uintptr_t>(&ChSetFreq);
    for(auto* vt:{&devVt,&bufVt,&b3dVt,&propVt,&lisVt}){ for(size_t k=0;k<vt->size();++k)(*vt)[k]=0xDEAE0000u+uint32(k);
        (*vt)[0]=bit_cast<uintptr_t>(&ComQI); (*vt)[1]=bit_cast<uintptr_t>(&ComAddRef); (*vt)[2]=bit_cast<uintptr_t>(&ComRelease); }
    devVt[3]=bit_cast<uintptr_t>(&DevCreate); bufVt[14]=bit_cast<uintptr_t>(&BufSetFormat); propVt[4]=bit_cast<uintptr_t>(&PropSet);
    lisVt[12]=bit_cast<uintptr_t>(&LisDoppler); lisVt[13]=bit_cast<uintptr_t>(&LisOrient); lisVt[14]=bit_cast<uintptr_t>(&LisPosition);
    lisVt[15]=bit_cast<uintptr_t>(&LisRolloff); lisVt[17]=bit_cast<uintptr_t>(&LisCommit);
    dev[0]=uintptr_t(devVt.data()); buf[0]=uintptr_t(bufVt.data()); b3d[0]=uintptr_t(b3dVt.data()); prop[0]=uintptr_t(propVt.data()); lis[0]=uintptr_t(lisVt.data());
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=53;
inline const std::array<Fn,NFN> fns{{
    {"CAEAudioHardware","Constructor",0x4D83E0,0,-1,128},
    {"CAEAudioHardware","Destructor",0x4D83A0,0,-1,64},
    {"CAEAudioHardware","AllocateChannels",0x5B9340,0,-1,512},
    {"CAEAudioHardware","RequestVirtualChannelSoundInfo",0x4D8E60,0,-1,256},
    {"CAEAudioHardware","Query3DSoundEffects",0x4D8490,0,-1,512},
    {"CAEAudioHardware","GetNumAvailableChannels",0x4D8810,0,-1,64},
    {"CAEAudioHardware","GetChannelPlayTimes",0x4D8820,0,-1,256},
    {"CAEAudioHardware","SetChannelVolume",0x4D8870,0,-1,256},
    {"CAEAudioHardware","LoadSoundBank",0x4D88A0,0,-1,64},
    {"CAEAudioHardware","IsSoundBankLoaded",0x4D88C0,0,-1,64},
    {"CAEAudioHardware","GetSoundBankLoadingStatus",0x4D88D0,0,-1,64},
    {"CAEAudioHardware","LoadSound",0x4D8ED0,0,-1,64},
    {"CAEAudioHardware","IsSoundLoaded",0x4D8EF0,0,-1,64},
    {"CAEAudioHardware","GetSoundLoadingStatus",0x4D8F00,0,-1,64},
    {"CAEAudioHardware","StopSound",0x4D88E0,0,-1,256},
    {"CAEAudioHardware","SetChannelPosition",0x4D8920,0,-1,256},
    {"CAEAudioHardware","SetChannelFrequencyScalingFactor",0x4D8960,0,-1,256},
    {"CAEAudioHardware","RescaleChannelVolumes",0x4D8990,0,-1,1024},
    {"CAEAudioHardware","UpdateReverbEnvironment",0x4D8DA0,0,-1,256},
    {"CAEAudioHardware","GetSoundHeadroom",0x4D8E30,0,-1,64},
    {"CAEAudioHardware","EnableEffectsLoading",0x4D8E40,0,-1,64},
    {"CAEAudioHardware","DisableEffectsLoading",0x4D8E50,0,-1,64},
    {"CAEAudioHardware","GetVirtualChannelSoundLengths",0x4D8E90,0,-1,64},
    {"CAEAudioHardware","GetVirtualChannelSoundLoopStartTimes",0x4D8EB0,0,-1,64},
    {"CAEAudioHardware","PlayTrack",0x4D8F10,0,-1,128},
    {"CAEAudioHardware","StartTrackPlayback",0x4D8F30,0,-1,64},
    {"CAEAudioHardware","StopTrack",0x4D8F50,0,-1,64},
    {"CAEAudioHardware","GetTrackPlayTime",0x4D8F60,0,-1,64},
    {"CAEAudioHardware","GetTrackLengthMs",0x4D8F70,0,-1,64},
    {"CAEAudioHardware","GetActiveTrackID",0x4D8F80,0,-1,64},
    {"CAEAudioHardware","GetPlayingTrackID",0x4D8F90,0,-1,64},
    {"CAEAudioHardware","GetBeatInfo",0x4D8FA0,0,-1,1024},
    {"CAEAudioHardware","SetBassSetting",0x4D94A0,0,-1,128},
    {"CAEAudioHardware","DisableBassEq",0x4D94D0,0,-1,64},
    {"CAEAudioHardware","EnableBassEq",0x4D94E0,0,-1,64},
    {"CAEAudioHardware","SetChannelFlags",0x4D9500,0,-1,256},
    {"CAEAudioHardware","SetMusicMasterScalingFactor",0x4D9530,0,-1,64},
    {"CAEAudioHardware","SetEffectsMasterScalingFactor",0x4D9540,0,-1,64},
    {"CAEAudioHardware","SetMusicFaderScalingFactor",0x4D9550,0,-1,64},
    {"CAEAudioHardware","SetEffectsFaderScalingFactor",0x4D9560,0,-1,64},
    {"CAEAudioHardware","GetEffectsMasterScalingFactor",0x4D9570,0,-1,64},
    {"CAEAudioHardware","GetMusicMasterScalingFactor",0x4D9580,0,-1,64},
    {"CAEAudioHardware","GetEffectsFaderScalingFactor",0x4D9590,0,-1,64},
    {"CAEAudioHardware","SetNonStreamFaderScalingFactor",0x4D95A0,0,-1,64},
    {"CAEAudioHardware","SetStreamFaderScalingFactor",0x4D95B0,0,-1,64},
    {"CAEAudioHardware","IsStreamingFromDVD",0x4D95C0,0,-1,64},
    {"CAEAudioHardware","GetDVDDriveLetter",0x4D95D0,0,-1,64},
    {"CAEAudioHardware","CheckDVD",0x4D95E0,0,-1,64},
    {"CAEAudioHardware","PauseAllSounds",0x4D95F0,0,-1,128},
    {"CAEAudioHardware","ResumeAllSounds",0x4D9630,0,-1,256},
    {"CAEAudioHardware","InitDirectSoundListener",0x4D9640,0,-1,512},
    {"CAEAudioHardware","Terminate",0x4D97A0,0,-1,256},
    {"CAEAudioHardware","Service",0x4D9870,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"audiohw54-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"audiohw54-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audiohw54");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"audiohw54-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    auto* const logHook=FindHook("CAEAudioUtility","AudioLog10");
    if(!logHook){Write({{"event","error"},{"reason","hook not found"},{"function","CAEAudioUtility::AudioLog10"}});return;}
    SetupFakes();

    using std::bit_cast;
    // Cuerpos de la DLL resueltos ANTES de escribir ningún parche.
    const uintptr_t dllForeground=Body(bit_cast<uintptr_t>(&IsForegroundApp));
    struct Stub { uintptr_t exe, dll; const void* fake; };
    const std::array<Stub,26> stubs{{
        {0x4D7C60,Body(bit_cast<uintptr_t>(&CAEAudioChannel::SetVolume)),bit_cast<const void*>(&FakeSetVolume)},
        {0x4D7950,Body(bit_cast<uintptr_t>(&CAEAudioChannel::SetPosition)),bit_cast<const void*>(&FakeSetPosition)},
        {0x4D7AA0,Body(bit_cast<uintptr_t>(&CAEAudioChannel::SetReverbAndDepth)),bit_cast<const void*>(&FakeSetReverb)},
        {0x4F1F30,Body(bit_cast<uintptr_t>(&CAEStreamingChannel::SetBassEQ)),bit_cast<const void*>(&FakeSetBassEQ)},
        {0x4E0670,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::LoadSoundBank)),bit_cast<const void*>(&BkLoadBank)},
        {0x4E0220,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::IsSoundBankLoaded)),bit_cast<const void*>(&BkIsBankLoaded)},
        {0x4E0250,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::GetSoundBankLoadingStatus)),bit_cast<const void*>(&BkBankStatus)},
        {0x4E07A0,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::LoadSound)),bit_cast<const void*>(&BkLoadSound)},
        {0x4E03B0,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::IsSoundLoaded)),bit_cast<const void*>(&BkIsSoundLoaded)},
        {0x4E0400,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::GetSoundLoadingStatus)),bit_cast<const void*>(&BkSoundStatus)},
        {0x4E01E0,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::GetSoundHeadroom)),bit_cast<const void*>(&BkHeadroom)},
        {0x4E0450,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::UpdateVirtualChannels)),bit_cast<const void*>(&BkUpdateVc)},
        {0x4DFE30,Body(bit_cast<uintptr_t>(&CAEMP3BankLoader::Service)),bit_cast<const void*>(&BkService)},
        {0x4E0A70,Body(bit_cast<uintptr_t>(&CAEMP3TrackLoader::GetTrackInfo)),bit_cast<const void*>(&TkGetTrackInfo)},
        {0x4E0BD0,Body(bit_cast<uintptr_t>(&CAEMP3TrackLoader::IsCurrentAudioStreamAvailable)),bit_cast<const void*>(&TkAvailable)},
        {0x4F1230,Body(bit_cast<uintptr_t>(&CAEStreamThread::PlayTrack)),bit_cast<const void*>(&StPlayTrack)},
        {0x4F1580,Body(bit_cast<uintptr_t>(&CAEStreamThread::StopTrack)),bit_cast<const void*>(&StStopTrack)},
        {0x4F1530,Body(bit_cast<uintptr_t>(&CAEStreamThread::GetTrackPlayTime)),bit_cast<const void*>(&StGetPlayTime)},
        {0x4F1550,Body(bit_cast<uintptr_t>(&CAEStreamThread::GetTrackLengthMs)),bit_cast<const void*>(&StGetLength)},
        {0x4F1560,Body(bit_cast<uintptr_t>(&CAEStreamThread::GetActiveTrackID)),bit_cast<const void*>(&StGetActive)},
        {0x4F1570,Body(bit_cast<uintptr_t>(&CAEStreamThread::GetPlayingTrackID)),bit_cast<const void*>(&StGetPlaying)},
        {0x4F1590,Body(bit_cast<uintptr_t>(&CAEStreamThread::Stop)),bit_cast<const void*>(&StStop)},
        {0x4F1220,Body(bit_cast<uintptr_t>(&CAEStreamThread::WaitForExit)),bit_cast<const void*>(&StWait)},
        {0x4EEA30,Body(bit_cast<uintptr_t>(&CAESmoothFadeThread::Stop)),bit_cast<const void*>(&SfStop)},
        {0x4EEA40,Body(bit_cast<uintptr_t>(&CAESmoothFadeThread::WaitForExit)),bit_cast<const void*>(&SfWait)},
        {0x4D8010,Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetReverbEnvironmentAndDepth)),bit_cast<const void*>(&FakeGetReverb)},
    }};
    const uintptr_t dllFree=Body(bit_cast<uintptr_t>(&CMemoryMgr::Free));
    if(dllFree>=0x400000&&dllFree<0x1000000){Write({{"event","error"},{"reason","dll body inside exe"},{"exe",0x72F430}});return;}
    for(const auto& s:stubs)if(s.dll>=0x400000&&s.dll<0x1000000){Write({{"event","error"},{"reason","dll body inside exe"},{"exe",s.exe}});return;}

    // estado global vigilado (las entradas parcheadas deben quedar como estaban)
    std::vector<Saved> savedAll{ Save(0x746060,5), Save(dllForeground,5), Save(0x8214BD,5), Save(0x72F430,5), Save(dllFree,5) };
    for(const auto& s:stubs){ savedAll.push_back(Save(s.exe,5)); savedAll.push_back(Save(s.dll,5)); }

    parkedEvt=CreateEventA(nullptr,FALSE,FALSE,nullptr); resumeEvt=CreateEventA(nullptr,TRUE,FALSE,nullptr);
    const HANDLE streamThread=AEAudioHardware.m_pStreamThread.m_nHandle;
    const bool streamRunning=streamThread&&WaitForSingleObject(streamThread,0)==WAIT_TIMEOUT;
    streamTid=streamRunning ? GetThreadId(streamThread) : 0;
    bool parkedOk=!streamRunning;
    std::array<bool,NFN> enabled{},locked{};
    const bool logEnabled=logHook->Hooked(), logLocked=logHook->Locked();
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch fg1(0x746060,FakeForeground), fg2(dllForeground,FakeForeground);
        if(streamRunning)parkedOk=WaitForSingleObject(parkedEvt,3000)==WAIT_OBJECT_0;
        if(!parkedOk){ SetEvent(resumeEvt); Write({{"event","error"},{"reason","audio stream thread not parked"},{"state_restored",true}}); return; }
        Write({{"event","stream_thread"},{"running",streamRunning},{"parked",streamRunning}});
        for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
        logHook->LockState(false); logHook->State(false);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* w=static_cast<uint8*>(p); for(size_t k=0;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(w+k,&v,4); } for(size_t k=n&~size_t(3);k<n;++k)w[k]=uint8(next()); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA54u; if(!stt)stt=1; nrec=0; hrIdx=0; chPtN={};
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    uint8* const o=hw.data();

                    // --- fixtures comunes ---
                    fill(o,HW_SIZE); fill(bank.data(),bank.size()); fill(trk.data(),trk.size()); fill(outBuf.data(),outBuf.size());
                    std::memset(outArr.data(),0xCD,outArr.size());
                    for(auto& c:chans){ std::memset(c.data(),0,c.size()); At<uintptr_t>(c.data(),0)=uintptr_t(chVt.data()); }
                    for(size_t k=0;k<NCH;++k){
                        auto pt=[&]()->int32{ switch(next()%8){ case 0: return -1; case 1: return -2-int32(next()%8); case 2: return int32(next()); case 3: return 0; default: return int32(next()%60000); } };
                        const int32 p0=pt(); const int32 p1=next()%3 ? pt() : p0; chPt[k]={p0,p1};
                        int32 len; do{ switch(next()%6){ case 0: len=int32(next()); break; case 1: len=int32(0x8000+next()%0x8000); break; case 2: len=-int32(next()%30000); break; default: len=int32(1+next()%60000); } }while(int16(len)==0);
                        chLen[k]=len; chRev[k]=next()%3==0;
                    }
                    bankBool=next()%2; trkAvail=next()%2; headroom=uni(-30.f,30.f);
                    stActive=next()%5==0 ? -1 : int32(next()%200); stPlay=next()%4==0 ? int32(next()) : int32(next()%300000)-1000; stLen=int32(next()); stPlaying=next()%4==0 ? -1 : int32(next()%200);
                    envRev=int8(next()%6==0 ? next() : next()%26); envDepth=next()%3 ? -int32(next()%10000) : int32(next());
                    for(size_t k=0;k<hrs.size();++k){ const uint32 r=next()%10; hrs[k]=r<7 ? S_OK : r==7 ? S_FALSE : r==8 ? HRESULT(0x80004005) : HRESULT(0x88780078); outNull[k]=next()%10==0; }
                    trkInfoRet=beats.data();
                    // punteros y floats del objeto (sin NaN): volúmenes, ganancias, frecuencias, factores y máximos previos
                    At<void*>(o,0xD98)=bank.data(); At<void*>(o,0xD9C)=trk.data(); At<void*>(o,0xDA0)=dev.data();
                    At<void*>(o,0xE0C)=lis.data(); At<void*>(o,0xE10)=chans[0].data();
                    At<void*>(trk.data(),0x18)=dvdPath.data(); fill(dvdPath.data(),dvdPath.size());
                    for(size_t k=0;k<NCH;++k){ At<float>(o,0x110+4*k)=next()%10==0 ? -100.f : uni(-100.f,20.f); At<float>(o,0x210+4*k)=uni(0.f,2.f); At<float>(o,0x310+4*k)=uni(0.f,2.f);
                        At<void*>(o,0xE64+4*k)=chans[k].data(); }
                    for(size_t off=0x410;off<=0x424;off+=4)At<float>(o,off)=next()%6==0 ? (next()%2 ? 0.f : 1.f) : uni(0.f,1.5f);
                    At<float>(o,0x428)=uni(-30.f,30.f); At<float>(o,0x42C)=uni(-30.f,30.f); At<float>(o,0xD94)=uni(-5.f,5.f);
                    // ranuras de canales: cuentas que no se salen de los 64 canales
                    for(size_t k=0;k<NCH;++k)At<uint16>(o,0x90+2*k)=uint16(next()%3==0 ? 0 : next()%4 ? 1+next()%std::min<size_t>(4,NCH-k) : next()%(NCH-k+1));
                    auto chArg=[&]()->uint32{ const uint32 hi=next()%4==0 ? (next()&0xFFFF0000u) : 0; const int16 c=next()%8==0 ? int16(-1-int(next()%3)) : next()%10==0 ? int16(0x8000|next()) : int16(next()%NCH); return hi|uint16(c); };
                    auto idArg=[&](uint32 ch)->uint32{ const int16 c=int16(ch); const uint32 hi=next()%4==0 ? (next()&0xFFFF0000u) : 0;
                        const uint32 cnt=c>=0&&c<int16(NCH) ? At<uint16>(o,0x90+2*c) : 0; return hi|uint16(next()%3 ? next()%(cnt+2) : next()%8); };
                    // Como el EXE, el índice de canal + id no se comprueba contra 64: se mantiene dentro de los canales.
                    auto clampSlots=[&](uint32 ch,uint32 id){ const int16 c=int16(ch); if(c>=0&&c<int16(NCH)){ auto& n=At<uint16>(o,0x90+2*c); if(uint16(id)<n&&size_t(c)+uint16(id)>=NCH)n=uint16(id); } };
                    auto rescaleFixture=[&]{
                        At<uint16>(o,0x8E)=uint16(next()%8==0 ? next()%4 : next()%(NCH+1));
                        for(size_t k=0;k<NCH;++k){ if(next()%10==0)At<void*>(o,0xE64+4*k)=nullptr;
                            uint16 fl=uint16(next()&0xFF); if(next()%3==0)fl&=0x41|0x80; At<uint16>(o,6+2*k)=next()%10==0 ? uint16(next()) : fl; }
                        At<uint8>(o,2)=uint8(next()%4==0 ? next() : next()%2); At<uint8>(o,3)=uint8(next()%4==0 ? next() : next()%2);
                        if(next()%4==0){ At<float>(o,0x428)=-100.f; At<float>(o,0x42C)=-100.f; }
                    };
                    auto fullChannels=[&](bool allowZeroNull){ for(size_t k=0;k<NCH;++k)At<void*>(o,0xE64+4*k)=chans[k].data(); if(allowZeroNull&&next()%4==0)At<void*>(o,0xE64)=nullptr; };

                    // beats de la pista (GetBeatInfo)
                    auto beatFixture=[&]{
                        std::memset(beats.data(),0,beats.size()*4);
                        const uint32 n=next()%8==0 ? 990+next()%11 : next()%10==0 ? 0 : 1+next()%60;
                        int32 t=next()%6==0 ? -int32(next()%3000) : int32(next()%3000);
                        for(uint32 k=0;k<n&&k<1000;++k){ beats[2*k]=uint32(t); beats[2*k+1]=next()%10==0 ? next() : 1+next()%4; if(beats[2*k+1]==0||int32(beats[2*k+1])<0)beats[2*k+1]=1;
                            t+=int32(next()%6==0 ? 0 : next()%900); }
                        if(n<1000){ beats[2*n]=next()%3 ? uint32(t) : next(); beats[2*n+1]=next()%3 ? 0 : 0x80000000u|next(); }
                        for(uint32 k=n+1;k<1000;++k){ beats[2*k]=next(); beats[2*k+1]=next()%2 ? 0 : next(); }
                        switch(next()%8){ case 0: stPlay=-7; break; case 1: stPlay=-int32(1+next()%100); break; case 2: stPlay=0; break;
                            default: stPlay=n ? int32(beats[0])+int32(next()%(t-int32(beats[0])+200)) - 100 : int32(next()%5000); }
                        At<int32>(o,0x100C)=next()%3==0 ? -1 : next()%3==0 ? int32(next()%(n+2)) : int32(next()%70)-5;
                        if(next()%10==0)stActive=-1;
                    };

                    if(nm=="Constructor"){
                        auto* r=reinterpret_cast<void*(__thiscall*)(void*)>(a)(o); ret=r==o;
                    } else if(nm=="Destructor"){
                        InitializeCriticalSectionEx(&At<CRITICAL_SECTION>(o,0xE14+0x38),0,CRITICAL_SECTION_NO_DEBUG_INFO);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    } else if(nm=="AllocateChannels"){
                        if(next()%3==0)for(size_t k=0;k<NCH;++k)At<uint16>(o,0x90+2*k)=0;
                        if(next()%4==0)for(size_t k=0;k<NCH;k+=1+next()%3)At<uint16>(o,0x90+2*k)=uint16(1+next()%3);
                        At<uint16>(o,0x8C)=uint16(next()%4==0 ? next() : next()%70);
                        const uint32 n=(next()%4==0 ? (next()&0xFFFF0000u) : 0)|uint16(next()%6==0 ? next() : next()%8==0 ? 0 : 1+next()%12);
                        ret=uint32(reinterpret_cast<uint32(__thiscall*)(void*,uint32)>(a)(o,n))&0xFFFF;
                    } else if(nm=="GetNumAvailableChannels"){
                        ret=uint32(reinterpret_cast<uint32(__thiscall*)(void*)>(a)(o))&0xFFFF;
                    } else if(nm=="GetChannelPlayTimes"){
                        const uint32 ch=(next()%4==0 ? (next()&0xFFFF0000u) : 0)|(next()%NCH);
                        auto& n=At<uint16>(o,0x90+2*uint16(ch)); if(n+uint16(ch)>NCH+1)n=uint16(NCH+1-uint16(ch)); if(n==0&&uint16(ch)==0)n=1;
                        int16* const dst=next()%10==0 ? nullptr : reinterpret_cast<int16*>(outArr.data());
                        reinterpret_cast<void(__thiscall*)(void*,uint32,int16*)>(a)(o,ch,dst);
                        out["out"]=ReversingProbe::Bytes(outArr.data(),0x90);
                    } else if(nm=="SetChannelVolume"||nm=="SetChannelFrequencyScalingFactor"||nm=="SetChannelFlags"||nm=="StopSound"||nm=="SetChannelPosition"){
                        const uint32 ch=chArg(), id=idArg(ch); clampSlots(ch,id);
                        for(size_t k=0;k<NCH;++k)if(next()%6==0)At<void*>(o,0xE64+4*k)=nullptr;
                        if(nm=="SetChannelVolume")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,float,uint32)>(a)(o,ch,id,uni(-120.f,30.f),next());
                        else if(nm=="SetChannelFrequencyScalingFactor")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,float)>(a)(o,ch,id,uni(0.f,3.f));
                        else if(nm=="SetChannelFlags")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,uint32)>(a)(o,ch,id,next());
                        else if(nm=="StopSound")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(a)(o,ch,id);
                        else { std::array<float,3> v{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-3000.f,3000.f)};
                            reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,const float*,uint32)>(a)(o,ch,id,v.data(),next()); }
                    } else if(nm=="LoadSoundBank"||nm=="IsSoundBankLoaded"||nm=="GetSoundBankLoadingStatus"||nm=="GetSoundHeadroom"){
                        At<uint8>(o,1)=uint8(next()%3==0 ? next() : next()%2);
                        const uint32 x=next(), y=next();
                        if(nm=="LoadSoundBank")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(a)(o,x,y);
                        else if(nm=="GetSoundHeadroom")ret=F(reinterpret_cast<float(__thiscall*)(void*,uint32,uint32)>(a)(o,x,y));
                        else ret=uint32(reinterpret_cast<uint32(__thiscall*)(void*,uint32,uint32)>(a)(o,x,y))&0xFF;
                    } else if(nm=="LoadSound"||nm=="IsSoundLoaded"||nm=="GetSoundLoadingStatus"){
                        At<uint8>(o,1)=uint8(next()%3==0 ? next() : next()%2);
                        const uint32 x=next(), y=next(), z=next();
                        if(nm=="LoadSound")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,uint32)>(a)(o,x,y,z);
                        else ret=uint32(reinterpret_cast<uint32(__thiscall*)(void*,uint32,uint32,uint32)>(a)(o,x,y,z))&0xFF;
                    } else if(nm=="RescaleChannelVolumes"){
                        rescaleFixture();
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    } else if(nm=="UpdateReverbEnvironment"){
                        At<uint16>(o,0x8E)=uint16(next()%8==0 ? 0 : 1+next()%NCH);
                        for(size_t k=0;k<NCH;++k)if(next()%5==0)At<void*>(o,0xE64+4*k)=nullptr;
                        if(next()%3==0){ At<uint8>(o,5)=uint8(envRev); At<int32>(o,0x88)=next()%2 ? envDepth : envDepth+1; }
                        if(next()%4==0)chRev={};
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    } else if(nm=="EnableEffectsLoading"||nm=="DisableEffectsLoading"||nm=="StartTrackPlayback"||nm=="StopTrack"||nm=="DisableBassEq"||nm=="EnableBassEq"){
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    } else if(nm=="RequestVirtualChannelSoundInfo"){
                        const uint32 vch=(next()%4==0 ? (next()&0xFFFF0000u) : 0)|uint16(next()%6==0 ? next() : next()%6==0 ? 300+next()%3 : next()%300);
                        reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,uint32)>(a)(o,vch,next(),next());
                    } else if(nm=="GetVirtualChannelSoundLengths"||nm=="GetVirtualChannelSoundLoopStartTimes"){
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(o,outArr.data());
                        out["out"]=ReversingProbe::Bytes(outArr.data(),outArr.size());
                    } else if(nm=="PlayTrack"){
                        const uint32 fl=(next()%4==0 ? (next()&0xFFFFFF00u) : 0)|(next()&0xFF);
                        const uint32 u1=next()%4==0 ? next() : next()%2, u2=next()%4==0 ? next() : next()%2;
                        reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,uint32,uint32,uint32,uint32)>(a)(o,next(),next(),next(),fl,u1,u2);
                    } else if(nm=="GetTrackPlayTime"||nm=="GetTrackLengthMs"||nm=="GetActiveTrackID"||nm=="GetPlayingTrackID"){
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(o);
                    } else if(nm=="GetBeatInfo"){
                        beatFixture();
                        {   ReversingProbe::EntryPatch del(0x8214BD,FakeDelete); // solo durante la llamada
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(o,outBuf.data()); }
                        out["out"]=ReversingProbe::Bytes(outBuf.data(),0xB0);
                    } else if(nm=="SetBassSetting"){
                        const uint32 s=(next()%4==0 ? (next()&0xFFFFFF00u) : 0)|uint8(next()%5==0 ? next() : next()%3);
                        reinterpret_cast<void(__thiscall*)(void*,uint32,float)>(a)(o,s,uni(-10.f,10.f));
                    } else if(nm.starts_with("Set")&&nm.ends_with("ScalingFactor")){
                        reinterpret_cast<void(__thiscall*)(void*,float)>(a)(o,next()%6==0 ? 0.f : uni(-0.5f,2.f));
                    } else if(nm.starts_with("Get")&&nm.ends_with("ScalingFactor")){
                        ret=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(o));
                    } else if(nm=="IsStreamingFromDVD"||nm=="GetDVDDriveLetter"||nm=="CheckDVD"){
                        ret=uint32(reinterpret_cast<uint32(__thiscall*)(void*)>(a)(o))&0xFF;
                    } else if(nm=="PauseAllSounds"){
                        At<uint8>(o,0)=uint8(next()%3==0 ? next() : next()%2); At<uint16>(o,0x8E)=uint16(next()%(NCH+1)); fullChannels(false);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    } else if(nm=="ResumeAllSounds"){
                        At<uint8>(o,0)=uint8(next()%3==0 ? next() : next()%2); rescaleFixture();
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    } else if(nm=="Query3DSoundEffects"){
                        At<uint8>(o,0xDA8)=uint8(next()%2 ? 0 : next()%3==0 ? next() : 1+next()%2);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    } else if(nm=="InitDirectSoundListener"){
                        if(next()%8==0)At<void*>(o,0xDA0)=nullptr;
                        At<uint8>(o,0xDA8)=uint8(next()%2 ? 0 : 1+next()%2);
                        const uint32 nch=next()%4==0 ? next() : 1+next()%2, sps=next()%4==0 ? next() : (next()%2 ? 48000u : 44100u), bits=next()%4==0 ? next() : (next()%2 ? 16u : 8u);
                        ret=uint32(reinterpret_cast<uint32(__thiscall*)(void*,uint32,uint32,uint32)>(a)(o,nch,sps,bits))&0xFF;
                    } else if(nm=="Terminate"){
                        for(size_t k=0;k<NCH;++k)if(next()%3==0)At<void*>(o,0xE64+4*k)=nullptr;
                        if(next()%5==0)At<void*>(o,0xD98)=nullptr; if(next()%5==0)At<void*>(o,0xD9C)=nullptr;
                        if(next()%5==0)At<void*>(o,0xE0C)=nullptr; if(next()%5==0)At<void*>(o,0xDA0)=nullptr;
                        // cargadores falsos: los destructores reales solo liberan (operator delete y CMemoryMgr::Free simulados)
                        At<uint8>(bank.data(),0x14)=uint8(next()%2); for(size_t off:{size_t(0),size_t(4),size_t(0x1C),size_t(0x20)})At<uint32>(bank.data(),off)=next()%4==0 ? 0u : 0x5A000000u+uint32(off);
                        At<uint8>(trk.data(),0)=uint8(next()%2); At<uint32>(trk.data(),0xC)=0x5B00000Cu; At<uint32>(trk.data(),0x10)=next()%4==0 ? 0u : 0x5B000010u;
                        At<uint32>(trk.data(),0x18)=next()%3==0 ? 0u : 0x5B000018u;
                        {   ReversingProbe::EntryPatch del(0x8214BD,FakeDelete), fr1(0x72F430,FakeFree), fr2(dllFree,FakeFree); // solo durante la llamada
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(o); }
                        out["bank"]=ReversingProbe::Bytes(bank.data(),0x30); out["trk"]=ReversingProbe::Bytes(trk.data(),0x1C);
                    } else if(nm=="Service"){
                        rescaleFixture(); At<uint16>(o,0x8E)=uint16(next()%8==0 ? next()%2 : 1+next()%NCH); fullChannels(true);
                        At<uint8>(o,0xDA8)=uint8(next()%3==0 ? 0 : next()%3==0 ? next() : 1+next()%2);
                        if(next()%3==0){ At<uint8>(o,5)=uint8(envRev); At<int32>(o,0x88)=envDepth; }
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    }
                    out["hw"]=ReversingProbe::Bytes(o,HW_SIZE);
                    out["ret"]=ret; out["recs"]=Recs(); return out;
                };
                // Los parches de los falsos se ponen después de fijar el estado de los hooks (dentro de cada pasada).
                auto runPatched=[&](bool candidate)->json {
                    for(auto* h:hooks)h->State(candidate);
                    Patches pt; for(const auto& s:stubs){ pt.Add(s.exe,s.fake); pt.Add(s.dll,s.fake); }
                    return run(candidate);
                };
                const auto original=runPatched(false), candidate=runPatched(true);
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
        logHook->State(logEnabled); logHook->LockState(logLocked);
        SetEvent(resumeEvt); // el hilo de streaming sigue con todos los hooks como estaban
    }
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
