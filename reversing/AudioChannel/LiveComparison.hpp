#pragma once
// Banco dentro del motor: CAEAudioChannel (12 funciones; SetReverbAndDepth y SetNotInRoom con un
// IKsPropertySet falso que registra las propiedades EAX). Buffers DirectSound falsos (vtables COM con un stub por metodo
// que registra argumentos y devuelve valores controlados) y AESmoothFadeThread controlado. Se compara el
// objeto canal, la traza de llamadas COM y el retorno.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Hardware/AEAudioChannel.h"
#include "Audio/AESmoothFadeThread.h"
#include "../ProbeRuntime.hpp"

namespace ChannelLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
alignas(16) inline std::array<uint8,0x80> channel{};
struct FakeCom { void* vptr; int id; };
inline FakeCom buffer{nullptr,1}, buffer3d{nullptr,2}, propset{nullptr,3};
inline HRESULT qiResult{}; inline std::array<HRESULT,2> setResults{}; inline int setIndex{};
inline uint32 curPos{}, status{}; inline int restoreLost{}; inline bool fadeRet{};
inline json Name(uintptr_t v) { if(v==uintptr_t(&buffer))return "buffer"; if(v==uintptr_t(&buffer3d))return "buffer3d"; if(v==uintptr_t(&propset))return "propset"; if(!v)return nullptr; return v; }
template<int S,int N> struct Com;
#define COM_STUB(N, ...) template<int S> struct Com<S,N> { static HRESULT __stdcall F(FakeCom* self __VA_OPT__(,) __VA_ARGS__); };
// Implementacion generica: registra slot, objeto y argumentos (crudos) y aplica efectos controlados.
inline HRESULT Effect(FakeCom* self,int slot,const uint32* a,int n) {
    const auto* tib=reinterpret_cast<NT_TIB*>(NtCurrentTeb());
    json args=json::array(); for(int k=0;k<n;++k){ const bool onStack=a[k]>=uintptr_t(tib->StackLimit)&&a[k]<uintptr_t(tib->StackBase); args.push_back(onStack ? json("stack") : json(a[k])); }
    if(slot==0&&a[0])args[0]=ReversingProbe::Bytes(reinterpret_cast<const void*>(a[0]),16); // QueryInterface: el IID se compara por contenido (EXE y DLL tienen su propia copia)
    if(self->id==3&&slot==4&&a[4])args.push_back({"value",*reinterpret_cast<const int32*>(a[4])}); // IKsPropertySet::Set: valor escrito
    trace.push_back({self->id==1 ? "buffer": self->id==2 ? "buffer3d" : "propset",slot,args});
    if(self->id==2&&slot==0){ if(a[1])*reinterpret_cast<void**>(a[1])=SUCCEEDED(qiResult) ? &propset : nullptr; return qiResult; }
    if(self->id==3&&slot==4)return setResults[(setIndex++)%2];
    if(self->id==1) {
        if(slot==4){ if(a[0])*reinterpret_cast<uint32*>(a[0])=curPos; if(a[1])*reinterpret_cast<uint32*>(a[1])=curPos+100; return DS_OK; } // GetCurrentPosition
        if(slot==9){ if(a[0])*reinterpret_cast<uint32*>(a[0])=status; return DS_OK; }                                                  // GetStatus
        if(slot==20){ return restoreLost-->0 ? DSERR_BUFFERLOST : DS_OK; }                                                              // Restore
    }
    if(slot==0){ if(a[1])*reinterpret_cast<void**>(a[1])=nullptr; return E_NOINTERFACE; }
    return DS_OK;
}
template<int S> HRESULT __stdcall C0(FakeCom* s) { return Effect(s,S,nullptr,0); }
template<int S> HRESULT __stdcall C1(FakeCom* s,uint32 a) { const uint32 v[]{a}; return Effect(s,S,v,1); }
template<int S> HRESULT __stdcall C2(FakeCom* s,uint32 a,uint32 b) { const uint32 v[]{a,b}; return Effect(s,S,v,2); }
template<int S> HRESULT __stdcall C3(FakeCom* s,uint32 a,uint32 b,uint32 c) { const uint32 v[]{a,b,c}; return Effect(s,S,v,3); }
template<int S> HRESULT __stdcall C4(FakeCom* s,uint32 a,uint32 b,uint32 c,uint32 d) { const uint32 v[]{a,b,c,d}; return Effect(s,S,v,4); }
template<int S> HRESULT __stdcall C6(FakeCom* s,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e,uint32 f) { const uint32 v[]{a,b,c,d,e,f}; return Effect(s,S,v,6); }
template<int S> HRESULT __stdcall C7(FakeCom* s,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e,uint32 f,uint32 g) { const uint32 v[]{a,b,c,d,e,f,g}; return Effect(s,S,v,7); }
// IDirectSoundBuffer: QI AddRef Release GetCaps GetCurrentPosition GetFormat GetVolume GetPan GetFrequency GetStatus Initialize Lock Play SetCurrentPosition SetFormat SetVolume SetPan SetFrequency Stop Unlock Restore
inline void* bufferVtbl[]{(void*)&C2<0>,(void*)&C0<1>,(void*)&C0<2>,(void*)&C1<3>,(void*)&C2<4>,(void*)&C3<5>,(void*)&C1<6>,(void*)&C1<7>,(void*)&C1<8>,(void*)&C1<9>,(void*)&C2<10>,
                          (void*)&C7<11>,(void*)&C3<12>,(void*)&C1<13>,(void*)&C1<14>,(void*)&C1<15>,(void*)&C1<16>,(void*)&C1<17>,(void*)&C0<18>,(void*)&C4<19>,(void*)&C0<20>};
// IDirectSound3DBuffer: ... SetPosition (19) = x,y,z,apply
inline void* buffer3dVtbl[]{(void*)&C2<0>,(void*)&C0<1>,(void*)&C0<2>,(void*)&C1<3>,(void*)&C2<4>,(void*)&C1<5>,(void*)&C1<6>,(void*)&C1<7>,(void*)&C1<8>,(void*)&C1<9>,(void*)&C1<10>,
                            (void*)&C1<11>,(void*)&C2<12>,(void*)&C3<13>,(void*)&C4<14>,(void*)&C2<15>,(void*)&C2<16>,(void*)&C2<17>,(void*)&C2<18>,(void*)&C4<19>,(void*)&C4<20>};
// IKsPropertySet: QI AddRef Release Get Set QuerySupport
inline void* propsetVtbl[]{(void*)&C2<0>,(void*)&C0<1>,(void*)&C0<2>,(void*)&C7<3>,(void*)&C6<4>,(void*)&C3<5>};
inline bool __fastcall RequestFade(CAESmoothFadeThread*,void*,IDirectSoundBuffer* b,float v,int16 t,bool stop) { trace.push_back({"request_fade",Name(uintptr_t(b)),std::bit_cast<uint32>(v),t,stop}); return fadeRet; }
inline void __fastcall SetBufferVolume(CAESmoothFadeThread*,void*,IDirectSoundBuffer* b,float v) { trace.push_back({"set_buffer_volume",Name(uintptr_t(b)),std::bit_cast<uint32>(v)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int slot; int cases; };
constexpr uintptr_t VT_CH=0x85F03C;
inline const std::array<Fn,12> fns{{
    {"ConvertFromBytesToMS",0x4D79D0,-1,512},{"ConvertFromMsToBytes",0x4D79F0,-1,512},{"GetCurrentPlaybackPosition",0x4D79A0,-1,256},{"Lost",0x4D7A10,-1,64},
    {"SetFrequency",0x4D7A50,-1,256},{"SetFrequencyScalingFactor",0,8,1024},{"SetOriginalFrequency",0x4D7A70,-1,256},{"SetPosition",0x4D7950,-1,256},
    {"SetVolume",0x4D7C60,-1,1024},{"UpdateStatus",0x4D7BD0,-1,256},{"SetReverbAndDepth",0x4D7AA0,-1,512},{"SetNotInRoom",0x4D7B50,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"channel-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"channel-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("channel");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,12> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook("CAEAudioChannel",fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    buffer.vptr=bufferVtbl; buffer3d.vptr=buffer3dVtbl; propset.vptr=propsetVtbl;
    auto& fx3d=*reinterpret_cast<uint8*>(0xB60660); const uint8 savedFx3d=fx3d; // AEAudioHardware.m_n3dEffectsQueryResult
    std::array<bool,12> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        ReversingProbe::EntryPatch rf(std::bit_cast<uintptr_t>(&CAESmoothFadeThread::RequestFade),RequestFade), rfe(0x4EEAA0,RequestFade);
        ReversingProbe::EntryPatch sv(std::bit_cast<uintptr_t>(&CAESmoothFadeThread::SetBufferVolume),SetBufferVolume), sve(0x4EEC80,SetBufferVolume);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA0D10u; trace=json::array();
                    for(auto& b:channel)b=uint8(next());
                    auto* c=reinterpret_cast<CAEAudioChannel*>(channel.data()); *reinterpret_cast<uintptr_t*>(channel.data())=VT_CH;
                    c->m_pDirectSoundBuffer=next()%6 ? reinterpret_cast<IDirectSoundBuffer*>(&buffer):nullptr;
                    c->m_pDirectSound3DBuffer=next()%3 ? reinterpret_cast<IDirectSound3DBuffer*>(&buffer3d):nullptr;
                    c->m_WaveFormat.nSamplesPerSec=uint32(next()%3 ? 22050+next()%30000 : next()%200000); c->m_WaveFormat.nChannels=uint16(1+next()%2);
                    c->m_nFrequency=next()%4==0 ? 22050 : next()%60000; c->m_nOriginalFrequency=next()%60000; c->m_Volume=uni(-120.f,10.f); c->m_bPaused=next()%2; c->m_bLooped=next()%2;
                    c->m_nBufferStatus=next()%2 ? DSBSTATUS_PLAYING : next()%5;
                    curPos=next()%3 ? next()%100000 : 0; status=next()%4==0 ? DSBSTATUS_BUFFERLOST : next()%8; restoreLost=int(next()%3); fadeRet=next()%2;
                    fx3d=uint8(next()%4==0 ? next()%3 : 1); qiResult=next()%5==0 ? E_NOINTERFACE : next()%6==0 ? S_FALSE : S_OK; setResults={next()%5==0 ? E_FAIL : S_OK, next()%5==0 ? E_FAIL : S_OK}; setIndex=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(VT_CH)[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const std::string nm=fn.name; void* self=channel.data(); json ret=nullptr;
                    if(nm=="ConvertFromBytesToMS"||nm=="ConvertFromMsToBytes")ret=reinterpret_cast<uint32(__thiscall*)(void*,uint32)>(target)(self,next()%3 ? next()%2000000 : next());
                    else if(nm=="GetCurrentPlaybackPosition")ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(self);
                    else if(nm=="Lost"){ if(!c->m_pDirectSoundBuffer)c->m_pDirectSoundBuffer=reinterpret_cast<IDirectSoundBuffer*>(&buffer); ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(self); }
                    else if(nm=="SetFrequency"||nm=="SetOriginalFrequency")reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(self,next()%4==0 ? c->m_nFrequency : next()%60000);
                    else if(nm=="SetFrequencyScalingFactor")reinterpret_cast<void(__thiscall*)(void*,float)>(target)(self,next()%4==0 ? 0.f : uni(0.f,2.5f));
                    else if(nm=="SetPosition"){ static CVector p; p=CVector{uni(-500.f,500.f),uni(-500.f,500.f),uni(-50.f,50.f)}; reinterpret_cast<void(__thiscall*)(void*,const CVector&)>(target)(self,p); }
                    else if(nm=="SetVolume")reinterpret_cast<void(__thiscall*)(void*,float)>(target)(self,next()%3==0 ? c->m_Volume+uni(-100.f,100.f) : uni(-120.f,10.f));
                    else if(nm=="SetReverbAndDepth")ret=reinterpret_cast<bool(__thiscall*)(void*,uint32,uint32)>(target)(self,next()%5==0 ? next() : next()%26,next()%3==0 ? next() : next()%11);
                    else if(nm=="SetNotInRoom")reinterpret_cast<void(__thiscall*)(void*,bool)>(target)(self,next()%2);
                    else { if(!c->m_pDirectSoundBuffer)c->m_pDirectSoundBuffer=reinterpret_cast<IDirectSoundBuffer*>(&buffer); reinterpret_cast<void(__thiscall*)(void*)>(target)(self); }
                    json words=json::array(); const auto* w=reinterpret_cast<const uint32*>(channel.data()); for(size_t k=1;k<sizeof(CAEAudioChannel)/4;++k)words.push_back(Name(w[k]));
                    return {{"ret",ret},{"channel",words},{"trace",trace},{"restore_left",restoreLost},{"switches",switches}};
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CAEAudioChannel::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CAEAudioChannel::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
            }
            Write({{"event","function_summary"},{"function",std::string("CAEAudioChannel::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    fx3d=savedFx3d;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",true}});
}
}
