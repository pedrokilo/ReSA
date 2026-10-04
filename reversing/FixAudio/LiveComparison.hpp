#pragma once
// Banco dentro del motor: arreglos de audio fuera de listas.
// 7 funciones: CAESound::UpdatePlayTime, CAEStreamingChannel::GetPlayTime/GetLength, CAEStaticChannel::GetPlayTime/
// GetLength, CAECollisionAudioEntity::UpdateParameters (nueva) y CAEPedAudioEntity::UpdateParameters (ahora hook de vtable).
// Original (hooks del lote desactivados) frente a candidato con fixtures deterministas:
// - El hilo de streaming de audio (AEAudioHardware.m_pStreamThread) se aparca durante todo el banco dentro de
//   IsForegroundApp (0x746060 + símbolo de la DLL), como en Audio13: GetPlayTime del canal de streaming lo usa ese hilo.
// - Todo corre sobre objetos falsos: CAESound (0x74 bytes), canal estático (vtable falsa copiada de 0x85F3CC con
//   IsSoundPlaying simulado; se llama por la entrada real de la vtable del EXE), canal de streaming (0x60098 bytes,
//   decoder falso con GetStreamLengthMs simulado), CAECollisionAudioEntity (vtable 0x862E64, lista de 300 entradas con
//   sonidos de un pool falso) y CAEPedAudioEntity (vtable 0x86C2A8, peatón falso con o sin matriz).
// - Se simulan y registran en el EXE y en la DLL: CAEAudioUtility::GetCurrentTimeInMS (en otros hilos devuelve la hora
//   real con QueryPerformanceCounter), CAEStreamingChannel::UpdatePlayTime, CAEAudioChannel::GetCurrentPlaybackPosition
//   y ConvertFromBytesToMS, CAESound::SetPosition y StopSoundAndForget. CTimer::m_snTimeInMilliseconds se fija por caso
//   y se restaura tras cada pasada y al final.
// - Los falsos solo registran en el hilo principal, se ponen después de fijar el estado de los hooks y se quitan en
//   orden inverso al acabar cada pasada (nunca están puestos durante Write/PumpMessages).
// - Fuera de los fixtures: soundLength 0 en UpdatePlayTime (bugfix reversible de la DLL; el EXE divide por cero) y
//   valores NaN/infinitos (velocidad del oyente, m_ClientVariable, volúmenes).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/AESound.h"
#include "Audio/AEAudioUtility.h"
#include "Audio/Hardware/AEAudioChannel.h"
#include "Audio/Hardware/AEStaticChannel.h"
#include "Audio/Hardware/AEStreamingChannel.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Entities/AECollisionAudioEntity.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace FixAudioLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& G(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
// Sigue los saltos de la ILT de la DLL, pero nunca entra en la imagen del EXE (un hook desactivado deja en la DLL un
// salto al EXE: entonces se parchea ese salto y no la entrada del EXE, que ya se parchea aparte).
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

// --- registro sin reservas (los falsos nunca crean json) ---
struct Rec { uint32 t, a, b, c, d; };
inline std::array<Rec,512> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec]={t,a,b,c,d}; ++nrec; }
inline json Recs(){
    const size_t n=std::min(nrec,recs.size()); json r=json::array();
    for(size_t i=0;i<n;++i){ const auto& x=recs[i]; r.push_back(json::array({x.t,x.a,x.b,x.c,x.d})); }
    if(nrec>n)r.push_back(json::array({0xFFFFFFFFu,uint32(nrec)}));
    return r; }

// --- copia de regiones globales ---
struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }

// Parches de una sola ejecución: sin direcciones repetidas y deshechos en orden inverso.
struct Patches {
    std::array<std::unique_ptr<ReversingProbe::EntryPatch>,24> P{}; size_t n{}; std::array<uintptr_t,24> seen{};
    template<class Fn> void Add(uintptr_t at,Fn fake){ for(size_t k=0;k<n;++k)if(seen[k]==at)return; seen[n]=at; P[n++]=std::make_unique<ReversingProbe::EntryPatch>(at,fake); }
    ~Patches(){ while(n)P[--n].reset(); }
};

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// --- objetos falsos ---
constexpr uintptr_t TIME_ADDR=0xB7CB84;
constexpr size_t NSND=8, COL_SIZE=0x1978, COL_LIST=0x208, COL_NENT=300, PED_SIZE=0x15C, STREAM_SIZE=0x60098;
alignas(16) inline std::array<std::array<uint8,0x74>,NSND> snd{};
alignas(16) inline std::array<uint8,COL_SIZE> colBuf{};
alignas(16) inline std::array<uint8,PED_SIZE> pedEnt{};
alignas(16) inline std::array<uint8,0x800> ped{};
alignas(16) inline std::array<uint8,0x48> pedMat{};
alignas(16) inline std::array<uint8,0x90> stat{};
alignas(16) inline std::array<uintptr_t,9> statVt{};
alignas(16) inline std::array<uint8,STREAM_SIZE> strm{};
alignas(16) inline std::array<uint8,0x30> dec{};
alignas(16) inline std::array<uintptr_t,8> decVt{};
inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<NSND;++k)if(p==snd[k].data())return 0x10+uint32(k);
    if(p==colBuf.data())return 0x20; if(p==pedEnt.data())return 0x21; if(p==ped.data())return 0x22;
    if(p==stat.data())return 0x23; if(p==strm.data())return 0x24; if(p==dec.data())return 0x25;
    return 0xEEEEEEEEu; }

// --- falsos (registran solo en el hilo principal) ---
inline std::array<uint64,4> timeRets{}; inline uint32 timeIdx{};
inline int32 updRet{}, lenRet{}; inline bool playing{}; inline uint32 curPosRet{}; inline std::array<uint32,2> msRets{}; inline uint32 msIdx{};
inline uint64 __cdecl FakeTime(){
    if(!Main()){ LARGE_INTEGER c{}; QueryPerformanceCounter(&c); const auto f=G<LARGE_INTEGER>(0xB610F0).QuadPart;
        return f ? uint64(double(c.QuadPart)/double(f)*1000.0)-G<uint64>(0xB610F8) : 0; }
    const uint64 r=timeRets[timeIdx%4]; Push(1,timeIdx++); return r; }
inline int32 __fastcall FakeUpdatePlayTime(void* s,void*){ if(!Main())return 0; Push(2,P(s)); return updRet; }
inline int32 __fastcall FakeStreamLen(void* s,void*){ if(!Main())return 0; Push(3,P(s)); return lenRet; }
inline bool __fastcall FakeIsPlaying(void* s,void*){ if(!Main())return false; Push(4,P(s)); return playing; }
inline uint32 __fastcall FakeCurPos(void* s,void*){ if(!Main())return 0; Push(5,P(s)); return curPosRet; }
inline uint32 __fastcall FakeBytesToMs(void* s,void*,uint32 bytes){ if(!Main())return 0; Push(6,P(s),bytes); return msRets[msIdx++%2]; }
inline void __fastcall FakeSetPosition(void* s,void*,float x,float y,float z){ if(!Main())return; Push(7,P(s),F(x),F(y),F(z)); }
inline void __fastcall FakeStop(void* s,void*){ if(!Main())return; Push(8,P(s)); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=7;
inline const std::array<Fn,NFN> fns{{
    {"CAESound","UpdatePlayTime",0x4EF2E0,0,-1,1024},
    {"CAEStreamingChannel","GetPlayTime",0,0x85F3F0,3,1024},
    {"CAEStreamingChannel","GetLength",0,0x85F3F0,4,256},
    {"CAEStaticChannel","GetPlayTime",0,0x85F3CC,3,512},
    {"CAEStaticChannel","GetLength",0,0x85F3CC,4,256},
    {"CAECollisionAudioEntity","UpdateParameters",0,0x862E64,0,1024},
    {"CAEPedAudioEntity","UpdateParameters",0,0x86C2A8,0,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"fixaudio-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"fixaudio-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fixaudio");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"fixaudio-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // estado global vigilado
    const uintptr_t dllForeground=Body(std::bit_cast<uintptr_t>(&IsForegroundApp));
    const uint32 savedTime=G<uint32>(TIME_ADDR);
    const std::vector<Saved> savedAll{ Save(0x85F3CC,9*4), Save(0x85F3F0,9*4), Save(0x862E64,4), Save(0x86C2A8,4), Save(0x746060,5), Save(dllForeground,5), Save(TIME_ADDR,4) };
    // volumen por defecto de AE_PED_SWIM_WAKE (tabla real del juego, solo lectura)
    const int8* const volTable=G<int8*>(0xBD00F8);
    const float swimVol=volTable ? float(volTable[76]) : 0.f;

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

        auto install=[&](Patches& pt){
            pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioUtility::GetCurrentTimeInMS)),FakeTime); pt.Add(0x4D9E80,FakeTime);
            pt.Add(Body(bit_cast<uintptr_t>(&CAEStreamingChannel::UpdatePlayTime)),FakeUpdatePlayTime); pt.Add(0x4F18A0,FakeUpdatePlayTime);
            pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioChannel::GetCurrentPlaybackPosition)),FakeCurPos); pt.Add(0x4D79A0,FakeCurPos);
            pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioChannel::ConvertFromBytesToMS)),FakeBytesToMs); pt.Add(0x4D79D0,FakeBytesToMs);
            pt.Add(Body(bit_cast<uintptr_t>(&CAESound::SetPosition)),FakeSetPosition); pt.Add(0x4EF880,FakeSetPosition);
            pt.Add(Body(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget)),FakeStop); pt.Add(0x4EF850,FakeStop);
        };

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* w=static_cast<uint8*>(p); for(size_t k=0;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(w+k,&v,4); } for(size_t k=n&~size_t(3);k<n;++k)w[k]=uint8(next()); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xF1Au; if(!stt)stt=1; nrec=0; timeIdx=0; msIdx=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // --- fixtures comunes ---
                    const uint32 now=next()%3 ? 100000+next()%2000000 : next(); G<uint32>(TIME_ADDR)=now;
                    for(auto& s:snd){ fill(s.data(),s.size());
                        At<float>(s.data(),0x10)=next()%4 ? float(int32(now)+int32(next()%600)-300) : (next()%2 ? uni(-5000.f,0.f) : float(next()%0x7FFFFF)*float(1u<<(next()%9)));
                        At<float>(s.data(),0x14)=uni(-60.f,20.f); At<float>(s.data(),0x18)=uni(0.f,4.f); At<float>(s.data(),0x1C)=uni(0.f,2.f);
                        At<float>(s.data(),0x20)=next()%2 ? 0.f : uni(0.f,0.1f); At<float>(s.data(),0x60)=uni(-100.f,0.f); At<float>(s.data(),0x64)=uni(0.f,2.f); At<float>(s.data(),0x6C)=uni(0.f,10.f);
                        for(size_t o:{size_t(0x24),size_t(0x28),size_t(0x2C),size_t(0x30),size_t(0x34),size_t(0x38),size_t(0x48),size_t(0x4C),size_t(0x50)})At<float>(s.data(),o)=uni(-1000.f,1000.f);
                        At<void*>(s.data(),4)=nullptr; At<void*>(s.data(),8)=nullptr; }
                    uint8* const s0=snd[0].data();

                    {
                        Patches pt; install(pt);

                        if(nm=="UpdatePlayTime"){
                            At<int16>(s0,0x5E)=int16(next()%6==0 ? (next()%2 ? 1 : int16(next())) : 0);
                            At<int16>(s0,0x68)=int16(next()%6==0 ? (next()%2 ? 1 : int16(next())) : 0);
                            switch(next()%6){ case 0: At<int16>(s0,0x5C)=int16(next()); break; case 1: At<int16>(s0,0x5C)=int16(-1-int(next()%3000)); break; default: At<int16>(s0,0x5C)=int16(next()%6000); }
                            float sp; switch(next()%8){ case 0: sp=1.f; break; case 1: sp=uni(-2.f,0.f); break; case 2: sp=uni(0.f,1000.f); break;
                                case 3: sp=std::bit_cast<float>(0x3F800000u+(next()%64)-32); break; case 4: sp=uni(-1e6f,1e6f); break; default: sp=uni(0.f,2.5f); }
                            At<float>(s0,0x64)=sp;
                            int16 len; do{ switch(next()%5){ case 0: len=int16(next()); break; case 1: len=int16(-1-int(next()%2000)); break; default: len=int16(1+next()%8000); } }while(len==0);
                            int16 loop; switch(next()%5){ case 0: loop=-1; break; case 1: loop=int16(next()); break; default: loop=int16(next()%4000); }
                            int16 prog; switch(next()%6){ case 0: prog=int16(next()); break; case 1: prog=int16(-int(next()%200)); break; case 2: prog=0; break; default: prog=int16(next()%300); }
                            reinterpret_cast<void(__thiscall*)(void*,int16,int16,int16)>(a)(s0,len,loop,prog);
                            out["snd"]=ReversingProbe::Bytes(s0,0x74);
                        }
                        else if(cls=="CAEStreamingChannel"){
                            uint8* o=strm.data(); fill(o,0x6C); fill(o+0x6006C,STREAM_SIZE-0x6006C);
                            At<uintptr_t>(o,0)=0x85F3F0; At<void*>(o,0x68)=nullptr; At<void*>(o,0x60070)=nullptr;
                            fill(dec.data(),dec.size()); for(size_t k=0;k<decVt.size();++k)decVt[k]=0xDEAD0000u+uint32(k); decVt[2]=bit_cast<uintptr_t>(&FakeStreamLen);
                            At<uintptr_t>(dec.data(),0)=uintptr_t(decVt.data());
                            At<void*>(o,0x6006C)=next()%5 ? static_cast<void*>(dec.data()) : nullptr;
                            static const int32 states[]{-1,-3,-1,-3,-1,-3,-7,-6,-5,-4,-2,0,1,5};
                            At<int32>(o,0x60074)=next()%8 ? states[next()%std::size(states)] : int32(next());
                            switch(next()%4){ case 0: At<int32>(o,0x6007C)=int32(next()); break; case 1: At<int32>(o,0x6007C)=-int32(next()%100000); break; default: At<int32>(o,0x6007C)=int32(next()%400000); }
                            const uint64 last=next()%4 ? uint64(next()%0x7FFFFFFF)+(next()%6==0 ? (uint64(1+next()%3)<<32) : 0) : (uint64(next())<<32|next());
                            At<uint64>(o,0x60088)=last;
                            auto t=[&]()->uint64{ switch(next()%10){ case 0: return last+999; case 1: return last+1000; case 2: return last-1-next()%50;
                                case 3: return last+(uint64(1)<<32)+next()%999; case 4: return uint64(next())<<32|next(); case 5: return last+1000+next()%100000; default: return last+next()%1000; } };
                            timeRets={t(),t(),t(),t()}; if(next()%2)timeRets[1]=timeRets[0]+next()%3;
                            switch(next()%4){ case 0: updRet=int32(next()); break; case 1: updRet=-1-int32(next()%8); break; default: updRet=int32(next()%400000); }
                            switch(next()%5){ case 0: lenRet=int32(next()); break; case 1: lenRet=-1; break; case 2: lenRet=int32(0x8000+next()%0x70000); break; default: lenRet=int32(next()%0x8000); }
                            ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(o);
                            out["head"]=ReversingProbe::Bytes(o,0x6C); out["tail"]=ReversingProbe::Bytes(o+0x6006C,STREAM_SIZE-0x6006C);
                            out["dec"]=ReversingProbe::Bytes(dec.data()+4,dec.size()-4);
                        }
                        else if(cls=="CAEStaticChannel"){
                            uint8* o=stat.data(); fill(o,stat.size());
                            for(size_t k=0;k<statVt.size();++k)statVt[k]=reinterpret_cast<uintptr_t*>(0x85F3CC)[k];
                            statVt[2]=bit_cast<uintptr_t>(&FakeIsPlaying);
                            At<uintptr_t>(o,0)=uintptr_t(statVt.data()); At<void*>(o,8)=nullptr; At<void*>(o,4)=nullptr;
                            playing=next()%5!=0; curPosRet=next()%3 ? next()%0x100000 : next();
                            for(auto& m:msRets)switch(next()%5){ case 0: m=next(); break; case 1: m=0x8000+next()%0x8000; break; case 2: m=0xFFFF0000u|next(); break; default: m=next()%0x8000; }
                            ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(o);
                            At<uintptr_t>(o,0)=0; out["obj"]=ReversingProbe::Bytes(o,stat.size());
                        }
                        else if(cls=="CAECollisionAudioEntity"){
                            uint8* e=colBuf.data(); fill(e,COL_SIZE);
                            At<uintptr_t>(e,0)=0x862E64; At<void*>(e,0xC)=nullptr; At<void*>(e,0x10)=nullptr;
                            At<int32>(e,0x204)=next()%10==0 ? int32(next()) : int32(next()%301);
                            for(size_t k=0;k<COL_NENT;++k){ uint8* en=e+COL_LIST+k*0x14;
                                At<void*>(en,8)=next()%3==0 ? static_cast<void*>(snd[1+next()%(NSND-1)].data()) : nullptr; }
                            // el sonido probado aparece en 0..3 entradas
                            const uint32 nIn=next()%5==0 ? 0u : 1u+next()%3;
                            for(uint32 k=0;k<nIn;++k)At<void*>(e+COL_LIST+(next()%COL_NENT)*0x14,8)=s0;
                            static const int32 evs[]{1,2,3,128,1,2,3,128,0,4,5,127,129,130,-1,255,256,2000};
                            At<int32>(s0,0xC)=next()%8 ? evs[next()%std::size(evs)] : int32(next());
                            static const int16 pps[]{-1,-1,-1,-2,0,0,1,-32768,32767};
                            const int16 pp=next()%3 ? pps[next()%std::size(pps)] : int16(next());
                            reinterpret_cast<void(__thiscall*)(void*,void*,int16)>(a)(e,s0,pp);
                            out["ent"]=Fnv(e,COL_SIZE); out["num"]=At<int32>(e,0x204); out["snd"]=ReversingProbe::Bytes(s0,0x74);
                            json found=json::array(); for(size_t k=0;k<COL_NENT;++k)if(At<void*>(e+COL_LIST+k*0x14,8)==s0)found.push_back(k); out["entries"]=found;
                        }
                        else { // CAEPedAudioEntity
                            uint8* e=pedEnt.data(); fill(e,PED_SIZE);
                            At<uintptr_t>(e,0)=0x86C2A8; At<void*>(e,0xC)=nullptr; At<void*>(e,0x10)=nullptr;
                            fill(ped.data(),ped.size()); fill(pedMat.data(),pedMat.size());
                            for(size_t o:{size_t(4),size_t(8),size_t(0xC)})At<float>(ped.data(),o)=uni(-3000.f,3000.f);
                            for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(pedMat.data(),o)=uni(-3000.f,3000.f);
                            At<void*>(ped.data(),0x14)=next()%3 ? static_cast<void*>(pedMat.data()) : nullptr;
                            At<void*>(e,0x94)=next()%6 ? static_cast<void*>(ped.data()) : nullptr;
                            auto pick=[&]()->void*{ const uint32 r=next()%6; return r==0 ? static_cast<void*>(s0) : r<3 ? nullptr : static_cast<void*>(snd[1+next()%(NSND-1)].data()); };
                            for(size_t o:{size_t(0x9C),size_t(0xA0),size_t(0xA4),size_t(0x150)})At<void*>(e,o)=pick();
                            switch(next()%5){ case 0: At<uint32>(e,0x80)=next(); break; case 1: At<uint32>(e,0x80)=0xFFFFFF00u+next()%256; break;
                                default: At<uint32>(e,0x80)=now-90+next()%30-int32(next()%2)*int32(next()%200); }
                            static const int32 evs[]{60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,76,76,76,59,77,0,-1};
                            At<int32>(s0,0xC)=next()%8 ? evs[next()%std::size(evs)] : int32(next()%128);
                            switch(next()%6){ case 0: At<float>(s0,0x14)=swimVol; break; case 1: At<float>(s0,0x14)=swimVol-20.f; break;
                                case 2: At<float>(s0,0x14)=swimVol-0.6f+uni(-0.01f,0.01f); break; case 3: At<float>(s0,0x14)=swimVol-19.4f+uni(-0.01f,0.01f); break;
                                default: At<float>(s0,0x14)=uni(swimVol-25.f,swimVol+3.f); }
                            static const int16 pps[]{-1,-1,-1,-2,0,0,1,-32768,32767};
                            const int16 pp=next()%3 ? pps[next()%std::size(pps)] : int16(next());
                            reinterpret_cast<void(__thiscall*)(void*,void*,int16)>(a)(e,s0,pp);
                            json sl=json::array(); for(size_t o:{size_t(0x9C),size_t(0xA0),size_t(0xA4),size_t(0x150)}){ sl.push_back(P(At<void*>(e,o))); At<uint32>(e,o)=0; }
                            At<uint32>(e,0x94)=0; out["slots"]=sl; out["ent"]=ReversingProbe::Bytes(e,PED_SIZE); out["snd"]=ReversingProbe::Bytes(s0,0x74);
                        }
                    }
                    G<uint32>(TIME_ADDR)=savedTime;
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
    G<uint32>(TIME_ADDR)=savedTime;
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
