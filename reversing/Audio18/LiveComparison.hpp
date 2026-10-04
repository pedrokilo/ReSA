#pragma once
// Banco dentro del motor: CAESound (lista de 18), 16 funciones con hook plano (CAESound no es virtual).
// Original (hooks de CAESound desactivados) frente a candidato con fixtures deterministas:
// - El hilo de streaming de audio (AEAudioHardware.m_pStreamThread) se aparca durante todo el banco dentro de
//   IsForegroundApp (0x746060 + símbolo de la DLL), como en FixAudio/Audio13.
// - Todo corre sobre objetos falsos: dos CAESound (0x74 bytes), dos entidades físicas (posición en m_placement o
//   matriz falsa), dos entidades de audio con vtable propia (UpdateParameters en el hueco 0, trazado; puede cambiar
//   velocidad/variación del sonido para probar la lectura posterior).
// - Se simulan y registran en el EXE y en la DLL: CEntity::RegisterReference/CleanUpOldReference,
//   CAEAudioEnvironment::GetPositionRelativeToCamera (vec), GetDistanceAttenuation, GetDirectionalMikeAttenuation,
//   GetDopplerRelativeFrequency, CTimer::GetIsSlowMotionActive, CAEAudioHardware::GetSoundHeadroom, rand (0x821B1E) y
//   CGeneral::GetRandomNumber, y CAESound::SetPosition solo para UpdateParameters/SoundHasFinished.
// - Globales fijados por caso y restaurados tras cada pasada: CTimer::m_snTimeInMilliseconds, m_FrameCounter,
//   posición y matriz de TheCamera (matriz falsa o nula), m_nActiveCam, m_nMode de las tres cámaras y el factor de
//   cámara lenta del EXE (0x8CBA6C).
// - Los falsos solo registran en el hilo principal, se ponen después de fijar el estado de los hooks y se quitan en
//   orden inverso al acabar cada pasada (nunca están puestos durante Write/PumpMessages).
// - Fuera de los fixtures: NaN señalizadores (el x87 los silencia al copiarlos) y el assert NOTSA del constructor
//   (Initialise ya no lo usa).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/AESound.h"
#include "Audio/AEAudioEnvironment.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace Audio18Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
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
struct Rec { uint32 t, a, b, c, d, e; };
inline std::array<Rec,256> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0,uint32 e=0){ if(nrec<recs.size())recs[nrec]={t,a,b,c,d,e}; ++nrec; }
inline json Recs(){
    const size_t n=std::min(nrec,recs.size()); json r=json::array();
    for(size_t i=0;i<n;++i){ const auto& x=recs[i]; r.push_back(json::array({x.t,x.a,x.b,x.c,x.d,x.e})); }
    if(nrec>n)r.push_back(json::array({0xFFFFFFFFu,uint32(nrec)}));
    return r; }

// --- copia de regiones globales ---
struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }
inline void Put(const Saved& s){ std::memcpy(reinterpret_cast<void*>(s.at),s.bytes.data(),s.bytes.size()); }

// Parches de una sola ejecución: sin direcciones repetidas y deshechos en orden inverso.
struct Patches {
    std::array<std::unique_ptr<ReversingProbe::EntryPatch>,32> P{}; size_t n{}; std::array<uintptr_t,32> seen{};
    template<class Fn> void Add(uintptr_t at,Fn fake){ for(size_t k=0;k<n;++k)if(seen[k]==at)return; seen[n]=at; P[n++]=std::make_unique<ReversingProbe::EntryPatch>(at,fake); }
    ~Patches(){ while(n)P[--n].reset(); }
};

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// --- globales y objetos falsos ---
constexpr uintptr_t TIME_ADDR=0xB7CB84, FRAME_ADDR=0xB7CB4C, CAM_ADDR=0xB6F028, ACTIVE_CAM=0xB6F081, CAM_MODE0=0xB6F1A8, CAM_STRIDE=0x238,
                    SLOWMO_ADDR=0x8CBA6C, AEHW_ADDR=0xB5F8B8;
alignas(16) inline std::array<std::array<uint8,0x74>,2> snd{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> ent{};
alignas(16) inline std::array<std::array<uint8,0x48>,3> mat{}; // 0: cámara, 1..2: entidades
alignas(16) inline std::array<std::array<uint8,0x10>,2> aud{};
alignas(16) inline std::array<uintptr_t,4> audVt{};
alignas(16) inline std::array<uint32,4> outBuf{};
inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<snd.size();++k){ if(p==snd[k].data())return 0x10+uint32(k); if(p==snd[k].data()+8)return 0x18+uint32(k); }
    for(size_t k=0;k<ent.size();++k)if(p==ent[k].data())return 0x20+uint32(k);
    for(size_t k=0;k<aud.size();++k)if(p==aud[k].data())return 0x28+uint32(k);
    if(u==AEHW_ADDR)return 0x30;
    return 0xEEEEEEEEu; }
inline json SndOut(uint8* s){
    auto b=ReversingProbe::Bytes(s,0x74); const uint32 p4=P(At<void*>(s,4)), p8=P(At<void*>(s,8));
    std::memcpy(b.data()+4,&p4,4); std::memcpy(b.data()+8,&p8,4); return b; }

// --- falsos (registran solo en el hilo principal) ---
inline uint32 seedR{}, rngCalls{};
inline int __cdecl FakeRand(){ if(!Main())return 0; const int r=int((seedR*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; Push(1,uint32(r)); return r; }
inline uint16 __cdecl FakeRandomNumber(){ return uint16(FakeRand()); }
inline std::array<float,3> relOut{};
inline void __cdecl FakeRelPos(float* out,const float* in){
    if(!Main()){ out[0]=out[1]=out[2]=0.f; return; }
    Push(2,F(in[0]),F(in[1]),F(in[2])); out[0]=relOut[0]; out[1]=relOut[1]; out[2]=relOut[2]; }
inline float distRet{}, dirRet{}, dopRet{}, hrRet{}; inline bool slowRet{};
inline float __cdecl FakeDistAtt(float d){ if(!Main())return -100.f; Push(3,F(d)); return distRet; }
inline float __cdecl FakeDirAtt(const float* v){ if(!Main())return 0.f; Push(4,F(v[0]),F(v[1]),F(v[2])); return dirRet; }
inline float __cdecl FakeDoppler(float a,float b,uint32 c,uint32 d,float e){ if(!Main())return 1.f; Push(5,F(a),F(b),c,d,F(e)); return dopRet; }
inline bool __cdecl FakeSlowMo(){ if(!Main())return false; Push(6); return slowRet; }
inline float __fastcall FakeHeadroom(void* hw,void*,uint32 sfx,uint32 slot){ if(!Main())return 0.f; Push(7,P(hw),sfx&0xFFFF,slot&0xFFFF); return hrRet; }
inline void __fastcall FakeCleanUp(void* e,void*,void* ref){ if(!Main())return; Push(8,P(e),P(ref)); }
inline void __fastcall FakeRegister(void* e,void*,void* ref){ if(!Main())return; Push(9,P(e),P(ref)); }
inline void __fastcall FakeSetPos(void* s,void*,float x,float y,float z){ if(!Main())return; Push(10,P(s),F(x),F(y),F(z)); }
inline bool audWrite{}; inline float audSpeed{}, audVar{};
inline void __fastcall FakeAudUpdate(void* e,void*,uint8* s,uint32 pos){
    Push(11,P(e),P(s),pos&0xFFFF); if(audWrite&&s){ At<float>(s,0x1C)=audSpeed; At<float>(s,0x20)=audVar; } }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=16;
inline const std::array<Fn,NFN> fns{{
    {"CAESound","UnregisterWithPhysicalEntity",0x4EF1A0,0,-1,256},
    {"CAESound","StopSound",0x4EF1C0,0,-1,256},
    {"CAESound","SetFlags",0x4EF2B0,0,-1,512},
    {"CAESound","GetRelativePosition-orginal",0x4EF350,0,-1,512},
    {"CAESound","CalculateFrequency",0x4EF390,0,-1,1024},
    {"CAESound","UpdateFrequency",0x4EF3E0,0,-1,256},
    {"CAESound","GetRelativePlaybackFrequencyWithDoppler",0x4EF400,0,-1,512},
    {"CAESound","GetSlowMoFrequencyScalingFactor",0x4EF440,0,-1,256},
    {"CAESound","NewVPSLEntry",0x4EF7A0,0,-1,512},
    {"CAESound","RegisterWithPhysicalEntity",0x4EF820,0,-1,256},
    {"CAESound","StopSoundAndForget",0x4EF850,0,-1,256},
    {"CAESound","SetPosition",0x4EF880,0,-1,1024},
    {"CAESound","CalculateVolume",0x4EFA10,0,-1,1024},
    {"CAESound","Initialise",0x4EFE50,0,-1,1024},
    {"CAESound","UpdateParameters",0x4EFF50,0,-1,1024},
    {"CAESound","SoundHasFinished",0x4EFFD0,0,-1,512},
}};

inline std::vector<Saved> Globals(){
    return { Save(TIME_ADDR,4), Save(FRAME_ADDR,4), Save(CAM_ADDR+4,0x14), Save(ACTIVE_CAM,1),
             Save(CAM_MODE0,2), Save(CAM_MODE0+CAM_STRIDE,2), Save(CAM_MODE0+2*CAM_STRIDE,2), Save(SLOWMO_ADDR,4) }; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"audio18-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"audio18-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio18");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"audio18-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    using std::bit_cast;
    const uintptr_t dllForeground=Body(bit_cast<uintptr_t>(&IsForegroundApp));
    const auto savedGlobals=Globals();
    std::vector<Saved> savedAll{ Save(0x746060,5), Save(dllForeground,5) };
    for(uintptr_t a:{uintptr_t(0x4D80B0),uintptr_t(0x4D7F20),uintptr_t(0x4D7F60),uintptr_t(0x4D7E40),uintptr_t(0x561AD0),uintptr_t(0x4D8E30),
                     uintptr_t(0x571A00),uintptr_t(0x571B70),uintptr_t(0x821B1E),uintptr_t(0x4EF880)})savedAll.push_back(Save(a,5));
    for(auto& s:savedGlobals)savedAll.push_back(s);

    // aparcar el hilo de streaming de audio
    parkedEvt=CreateEventA(nullptr,FALSE,FALSE,nullptr); resumeEvt=CreateEventA(nullptr,TRUE,FALSE,nullptr);
    const HANDLE streamThread=AEAudioHardware.m_pStreamThread.m_nHandle;
    const bool streamRunning=streamThread&&WaitForSingleObject(streamThread,0)==WAIT_TIMEOUT;
    streamTid=streamRunning ? GetThreadId(streamThread) : 0;
    bool parkedOk=!streamRunning;
    std::array<bool,NFN> enabled{},locked{};
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch fg1(0x746060,FakeForeground), fg2(dllForeground,FakeForeground);
        if(streamRunning)parkedOk=WaitForSingleObject(parkedEvt,3000)==WAIT_OBJECT_0;
        if(!parkedOk){ SetEvent(resumeEvt); Write({{"event","error"},{"reason","audio stream thread not parked"},{"state_restored",true}}); return; }
        Write({{"event","stream_thread"},{"running",streamRunning},{"parked",streamRunning}});
        for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

        auto install=[&](Patches& pt,bool stubSetPosition){
            pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetPositionRelativeToCamera_Vector)),FakeRelPos); pt.Add(0x4D80B0,FakeRelPos);
            pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetDistanceAttenuation)),FakeDistAtt); pt.Add(0x4D7F20,FakeDistAtt);
            pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetDirectionalMikeAttenuation)),FakeDirAtt); pt.Add(0x4D7F60,FakeDirAtt);
            pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetDopplerRelativeFrequency)),FakeDoppler); pt.Add(0x4D7E40,FakeDoppler);
            pt.Add(Body(bit_cast<uintptr_t>(&CTimer::GetIsSlowMotionActive)),FakeSlowMo); pt.Add(0x561AD0,FakeSlowMo);
            pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::GetSoundHeadroom)),FakeHeadroom); pt.Add(0x4D8E30,FakeHeadroom);
            pt.Add(Body(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference))),FakeCleanUp); pt.Add(0x571A00,FakeCleanUp);
            pt.Add(Body(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference))),FakeRegister); pt.Add(0x571B70,FakeRegister);
            pt.Add(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber); pt.Add(0x821B1E,FakeRand);
            if(stubSetPosition){ pt.Add(Body(bit_cast<uintptr_t>(&CAESound::SetPosition)),FakeSetPos); pt.Add(0x4EF880,FakeSetPos); }
        };

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* w=static_cast<uint8*>(p); for(size_t k=0;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(w+k,&v,4); } for(size_t k=n&~size_t(3);k<n;++k)w[k]=uint8(next()); };
                auto qnan=[&]{ return std::bit_cast<float>((next()%2 ? 0x7FC00000u : 0xFFC00000u)|(next()%4==0 ? next()&0x3FFFFFu : 0u)); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA18u; if(!stt)stt=1; nrec=0; rngCalls=0; seedR=next();
                    const auto g=Globals();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // --- globales del caso ---
                    const uint32 frame=next()%8 ? 1+next()%100000 : (next()%2 ? 0u : next());
                    G<uint32>(TIME_ADDR)=next()%4 ? 1000+next()%3000000 : next(); G<uint32>(FRAME_ADDR)=frame;
                    fill(mat[0].data(),mat[0].size()); for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(mat[0].data(),o)=uni(-3000.f,3000.f);
                    for(uintptr_t o:{uintptr_t(4),uintptr_t(8),uintptr_t(0xC)})G<float>(CAM_ADDR+o)=uni(-3000.f,3000.f);
                    G<void*>(CAM_ADDR+0x14)=next()%3 ? static_cast<void*>(mat[0].data()) : nullptr;
                    G<uint8>(ACTIVE_CAM)=uint8(next()%3);
                    for(uintptr_t k=0;k<3;++k){ static const uint16 modes[]{46,46,4,16,18,7,53,0}; G<uint16>(CAM_MODE0+k*CAM_STRIDE)=next()%6 ? modes[next()%std::size(modes)] : uint16(next()); }
                    G<float>(SLOWMO_ADDR)=next()%2 ? 0.5f : uni(0.f,1.f);

                    // --- objetos falsos ---
                    audVt={bit_cast<uintptr_t>(&FakeAudUpdate),0xDEAD0001u,0xDEAD0002u,0xDEAD0003u};
                    for(auto& x:aud){ fill(x.data(),x.size()); At<uintptr_t>(x.data(),0)=uintptr_t(audVt.data()); }
                    for(size_t k=0;k<ent.size();++k){ uint8* e=ent[k].data(); uint8* m=mat[1+k].data(); fill(e,ent[k].size()); fill(m,mat[1+k].size());
                        for(size_t o:{size_t(4),size_t(8),size_t(0xC)})At<float>(e,o)=uni(-3000.f,3000.f);
                        for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(m,o)=uni(-3000.f,3000.f);
                        At<void*>(e,0x14)=next()%2 ? static_cast<void*>(m) : nullptr; }
                    auto pickEnt=[&]()->void*{ const uint32 r=next()%3; return r==0 ? nullptr : static_cast<void*>(ent[r-1].data()); };
                    auto pickAud=[&]()->void*{ const uint32 r=next()%3; return r==0 ? nullptr : static_cast<void*>(aud[r-1].data()); };
                    for(auto& sx:snd){ uint8* s=sx.data(); fill(s,sx.size());
                        At<float>(s,0x10)=uni(-1.f,2000.f); At<float>(s,0x14)=uni(-60.f,20.f); At<float>(s,0x18)=next()%16 ? uni(0.05f,4.f) : 0.f;
                        At<float>(s,0x1C)=uni(0.f,4.f); At<float>(s,0x20)=next()%2 ? 0.f : uni(0.f,0.5f);
                        for(size_t o=0x24;o<0x3C;o+=4)At<float>(s,o)=uni(-3000.f,3000.f);
                        At<float>(s,0x48)=uni(0.f,500.f); At<float>(s,0x4C)=uni(0.f,500.f); At<float>(s,0x50)=uni(0.f,2.f);
                        At<float>(s,0x60)=uni(-100.f,0.f); At<float>(s,0x64)=uni(0.f,2.f); At<float>(s,0x6C)=uni(0.f,10.f);
                        switch(next()%4){ case 0: At<int32>(s,0x3C)=0; break; case 1: At<int32>(s,0x3C)=int32(frame); break; case 2: At<int32>(s,0x3C)=int32(frame)-1; break; default: At<int32>(s,0x3C)=int32(next()); }
                        At<void*>(s,4)=pickAud(); At<void*>(s,8)=pickEnt(); }
                    uint8* const s0=snd[0].data();
                    relOut={uni(-200.f,200.f),uni(-200.f,200.f),uni(-200.f,200.f)}; if(next()%10==0)relOut={0.f,0.f,0.f};
                    distRet=uni(-100.f,0.f); dirRet=next()%3 ? 0.f : uni(-6.f,0.f); dopRet=next()%4 ? uni(0.9f,1.1f) : 1.f; hrRet=uni(0.f,1.f); slowRet=next()%3!=0;
                    audWrite=next()%2!=0; audSpeed=uni(0.f,3.f); audVar=next()%2 ? 0.f : uni(0.f,0.5f);
                    // variación/velocidad para CalculateFrequency, UpdateFrequency y NewVPSLEntry (incluye ±0 y NaN silenciosos)
                    auto pickVarSpeed=[&]{
                        float sp; switch(next()%6){ case 0: sp=qnan(); break; case 1: sp=uni(-2.f,0.f); break; default: sp=uni(0.f,4.f); }
                        float v; switch(next()%9){ case 0: v=0.f; break; case 1: v=-0.f; break; case 2: v=uni(-1.f,0.f); break; case 3: v=qnan(); break;
                            case 4: v=sp; break; case 5: v=uni(0.f,8.f); break; default: v=std::isnan(sp) ? uni(0.f,1.f) : sp*uni(0.f,1.f); }
                        At<float>(s0,0x1C)=sp; At<float>(s0,0x20)=v; };

                    {
                        Patches pt; install(pt,nm=="UpdateParameters"||nm=="SoundHasFinished");
                        using V=void(__thiscall*)(void*);
                        if(nm=="UnregisterWithPhysicalEntity"||nm=="StopSound"||nm=="StopSoundAndForget"){
                            reinterpret_cast<V>(a)(s0);
                        }
                        else if(nm=="SetFlags"){
                            static const uint32 bits[]{1u,2u,4u,8u,0x10u,0x20u,0x40u,0x80u,0x100u,0x200u,0x400u,0x800u,0x1000u};
                            const uint32 env=next()%3 ? bits[next()%std::size(bits)]|(next()%2 ? next()&0xFFFF0000u : 0u) : next();
                            const uint32 en=next()%4==0 ? 0x10000u*(1+next()%0xFFFF) : (next()%2 ? 0u : (next()%2 ? 1u : next()));
                            reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(a)(s0,env,en);
                        }
                        else if(nm=="GetRelativePosition-orginal"){
                            for(auto& w:outBuf)w=0xA5A5A5A5u;
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(s0,outBuf.data());
                            out["out"]=ReversingProbe::Bytes(outBuf.data(),sizeof(outBuf));
                        }
                        else if(nm=="CalculateFrequency"||nm=="NewVPSLEntry"){
                            pickVarSpeed();
                            if(nm=="NewVPSLEntry"){ At<int16>(s0,0)=int16(next()%3 ? next()%8 : next()); At<int16>(s0,2)=int16(next()%3 ? next()%400 : next()); }
                            reinterpret_cast<V>(a)(s0);
                        }
                        else if(nm=="UpdateFrequency"){
                            pickVarSpeed(); if(next()%3==0)At<float>(s0,0x20)=next()%2 ? 0.f : -0.f;
                            reinterpret_cast<V>(a)(s0);
                        }
                        else if(nm=="GetRelativePlaybackFrequencyWithDoppler"){
                            ret=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(s0));
                        }
                        else if(nm=="GetSlowMoFrequencyScalingFactor"){
                            ret=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(s0));
                        }
                        else if(nm=="RegisterWithPhysicalEntity"){
                            void* e=next()%4==0 ? At<void*>(s0,8) : pickEnt();
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(s0,e);
                        }
                        else if(nm=="SetPosition"){
                            float x=uni(-3000.f,3000.f), y=uni(-3000.f,3000.f), z=uni(-3000.f,3000.f);
                            if(next()%10==0){ const uintptr_t cp=G<uintptr_t>(CAM_ADDR+0x14) ? G<uintptr_t>(CAM_ADDR+0x14)+0x30 : CAM_ADDR+4; x=G<float>(cp); y=G<float>(cp+4); z=G<float>(cp+8); }
                            reinterpret_cast<void(__thiscall*)(void*,float,float,float)>(a)(s0,x,y,z);
                        }
                        else if(nm=="CalculateVolume"){
                            reinterpret_cast<V>(a)(s0);
                        }
                        else if(nm=="Initialise"){
                            const uint32 bank=next()%3 ? next()%8 : next(), sfx=next()%3 ? next()%400 : next();
                            void* ae=pickAud();
                            const float x=uni(-3000.f,3000.f), y=uni(-3000.f,3000.f), z=uni(-3000.f,3000.f);
                            const float vol=uni(-60.f,20.f), roll=uni(0.f,4.f), speed=uni(0.f,4.f), dop=uni(0.f,2.f), var=next()%2 ? 0.f : uni(0.f,0.5f);
                            const uint32 delay=next()%3 ? next()%8 : next(), flags=next(), play=next()%3 ? next()%5000 : next();
                            reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,void*,float,float,float,float,float,float,float,uint32,uint32,float,uint32)>(a)
                                (s0,bank,sfx,ae,x,y,z,vol,roll,speed,dop,delay,flags,var,play);
                        }
                        else { // UpdateParameters / SoundHasFinished
                            At<uint16>(s0,0x56)=uint16((next()&0xFF7B)|(next()%2 ? 0x80 : 0)|(next()%2 ? 4 : 0));
                            if(nm=="UpdateParameters"){ const uint32 pos=next()%3 ? 0xFFFFFFFFu : (next()%2 ? next()%5000 : next());
                                reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(s0,pos); }
                            else reinterpret_cast<V>(a)(s0);
                        }
                    }
                    for(auto& s:g)Put(s);
                    out["snd"]=SndOut(s0); out["snd1"]=SndOut(snd[1].data());
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
    for(auto& s:savedGlobals)Put(s);
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
