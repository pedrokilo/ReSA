#pragma once
// Banco dentro del motor: flujo de datos y audio de armas (lista de 20).
// - CAEDataStream: Constructor, Destructor, FillBuffer, GetCurrentPosition, Initialise y Seek (OG), llamadas por la
//   entrada del EXE sobre objetos falsos (0x28 bytes) con HANDLE propios sobre %TEMP%/gta_audio20/stream.bin (nunca
//   ficheros de audio del juego). La vptr se compara por tipo (vtable del EXE 0x85F0BC o la de la DLL) y el HANDLE se enmascara.
//   operator delete (0x8214BD) se sustituye solo durante el destructor, sin reservar memoria.
// - CAEWeaponAudioEntity: 18 funciones desbloqueadas (UpdateParameters por el hueco 0 de la vtable 0x862E68) sobre una
//   entidad de audio falsa (vptr 0x862E68, m_tempSound incluido), una entidad física falsa (peatón, coche o avión, con o
//   sin matriz) y sonidos falsos. Constructor/Destructor siguen bloqueados (0x5DE990/0x507560 no son de esta clase).
// Original (hooks del lote desactivados) frente a candidato con fixtures deterministas.
// - El hilo de streaming de audio se aparca en IsForegroundApp durante todo el banco, como en FixAudio/Audio13.
// - Se simulan y registran en el EXE y en la DLL: CAESoundManager::RequestNewSound (copia el CAESound pedido y devuelve
//   una ranura falsa o nulo), CancelSoundsOwnedByAudioEntity, AreSoundsOfThisEventPlayingForThisEntity,
//   AreSoundsPlayingInBankSlot, CancelSoundsInBankSlot, CAEAudioHardware::IsSoundBankLoaded/LoadSoundBank,
//   CAudioEngine::IsLoadingTuneActive, CAEAudioUtility::ResolveProbability y GetRandomNumberInRange<float>,
//   CAEAudioEnvironment::GetPositionRelativeToCamera (placeable) y GetDistanceAttenuation, y
//   CEntity::RegisterReference/CleanUpOldReference (el EXE las llama desde CAESound::Initialise/Register... en línea).
//   CAESound::Initialise/SetPosition/StopSoundAndForget son reales (sobre m_tempSound y sonidos falsos).
// - CTimer::m_snTimeInMilliseconds y CGame::currArea se fijan por caso y se restauran tras cada pasada y al final.
// - Los falsos solo registran en el hilo principal, se ponen después de fijar el estado de los hooks y se quitan en orden
//   inverso al acabar cada pasada (nunca están puestos durante Write/PumpMessages).
// - Fuera de los fixtures: valores NaN (distancias, volúmenes, velocidades, m_ClientVariable) y entidades nulas en las
//   funciones que el EXE desreferencia sin comprobar.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/AESound.h"
#include "Audio/AEAudioUtility.h"
#include "Audio/AEAudioEnvironment.h"
#include "Audio/AudioEngine.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Managers/AESoundManager.h"
#include "Audio/Loaders/AEDataStream.h"
#include "Audio/Entities/AEWeaponAudioEntity.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace Audio20Live {
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
inline bool InExe(uintptr_t a){ return a>=0x400000&&a<0x1000000; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

// --- registro sin reservas (los falsos nunca crean json) ---
struct Rec { uint32 t, a, b, c, d; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{};
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
    std::array<std::unique_ptr<ReversingProbe::EntryPatch>,40> P{}; size_t n{}; std::array<uintptr_t,40> seen{};
    template<class Fn> void Add(uintptr_t at,Fn fake){ for(size_t k=0;k<n;++k)if(seen[k]==at)return; seen[n]=at; P[n++]=std::make_unique<ReversingProbe::EntryPatch>(at,fake); }
    ~Patches(){ while(n)P[--n].reset(); }
};

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// --- objetos falsos ---
constexpr uintptr_t TIME_ADDR=0xB7CB84, AREA_ADDR=0xB72914, WEAPON_VT=0x862E68, STREAM_VT=0x85F0BC;
constexpr size_t ENT_SIZE=0xA0, PHYS_SIZE=0x600, NSND=6, SND_SIZE=0x74, DS_SIZE=0x28, BUF_SIZE=2048, FILE_SIZE_BYTES=4096;
alignas(16) inline std::array<uint8,ENT_SIZE+8> ent{};
alignas(16) inline std::array<uint8,PHYS_SIZE> phys{};
alignas(16) inline std::array<uint8,0x48> mat{};
alignas(16) inline std::array<std::array<uint8,SND_SIZE>,NSND> snd{};
alignas(16) inline std::array<uint8,DS_SIZE+8> ds{};
alignas(16) inline std::array<uint8,BUF_SIZE> buf{};
inline char nameBuf[MAX_PATH]{}, missBuf[MAX_PATH]{};
inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    auto in=[&](const void* b,size_t n){ return u>=uintptr_t(b)&&u<uintptr_t(b)+n; };
    auto off=[&](const void* b){ return uint32(u-uintptr_t(b)); };
    if(in(ent.data(),ent.size()))return 0x10000u|off(ent.data());
    if(in(phys.data(),phys.size()))return 0x20000u|off(phys.data());
    for(size_t k=0;k<NSND;++k)if(in(snd[k].data(),SND_SIZE))return (0x30000u+uint32(k)*0x1000u)|off(snd[k].data());
    if(in(ds.data(),ds.size()))return 0x40000u|off(ds.data());
    if(in(mat.data(),mat.size()))return 0x50000u|off(mat.data());
    if(in(nameBuf,sizeof(nameBuf)))return 0x60000u|off(nameBuf);
    if(in(missBuf,sizeof(missBuf)))return 0x70000u|off(missBuf);
    if(in(buf.data(),buf.size()))return 0x80000u|off(buf.data());
    return uint32(u); // solo constantes falsas (estables entre pasadas)
}

// --- estado de los falsos por caso ---
inline uint8 bankWpn{}, bankCol{}, bankVeh{}, bankSaw{}, tune{};
inline int16 evPlaying{}, slotPlaying{};
inline std::array<uint8,4> probs{}; inline uint32 probIdx{};
inline float rnd{};
inline std::array<CVector,2> rel{}; inline uint32 relIdx{};
inline std::array<void*,4> reqRet{}; inline uint32 reqIdx{};
inline std::array<std::array<uint8,SND_SIZE>,12> reqs{}; inline std::array<uint32,12> reqFrom{}; inline size_t nreq{};
inline json Reqs(){
    const size_t n=std::min(nreq,reqs.size()); json r=json::array();
    for(size_t i=0;i<n;++i){ json e=json::array(); e.push_back(reqFrom[i]); for(auto b:reqs[i])e.push_back(b); r.push_back(e); }
    if(nreq>n)r.push_back(json::array({0xFFFFFFFFu,uint32(nreq)}));
    return r; }

// --- falsos (registran solo en el hilo principal) ---
inline CAESound* __fastcall FakeRequest(void*,void*,CAESound* s){
    if(!Main())return nullptr;
    Push(1,P(s)); if(nreq<reqs.size()){ std::memcpy(reqs[nreq].data(),s,SND_SIZE); reqFrom[nreq]=P(s); } ++nreq;
    return static_cast<CAESound*>(reqRet[reqIdx++%reqRet.size()]); }
inline void __fastcall FakeCancelOwned(void*,void*,void* e,bool full){ if(!Main())return; Push(2,P(e),uint8(full)); }
inline int16 __fastcall FakeEventPlaying(void*,void*,int16 ev,void* e){ if(!Main())return 0; Push(3,uint16(ev),P(e)); return evPlaying; }
inline int16 __fastcall FakeSlotPlaying(void*,void*,int16 slot){ if(!Main())return 0; Push(4,uint16(slot)); return slotPlaying; }
inline void __fastcall FakeCancelSlot(void*,void*,int16 slot,bool full){ if(!Main())return; Push(5,uint16(slot),uint8(full)); }
inline bool __fastcall FakeBankLoaded(void*,void*,uint32 bank,uint32 slot){
    if(!Main())return true;
    const uint16 b=uint16(bank), s=uint16(slot); Push(6,b,s);
    switch(b){ case 0x8F: return bankWpn&1; case 0x27: return bankCol&1; case 0x8A: return bankVeh&1; case 0x24: return bankSaw&1; default: return false; } }
inline void __fastcall FakeLoadBank(void*,void*,uint32 bank,uint32 slot){ if(!Main())return; Push(7,uint16(bank),uint16(slot)); }
inline bool __fastcall FakeTune(void*,void*){ if(!Main())return false; Push(8); return tune&1; }
inline bool __cdecl FakeProb(float p){ if(!Main())return false; Push(9,F(p)); return probs[probIdx++%probs.size()]&1; }
inline float __cdecl FakeRnd(float a,float b){ if(!Main())return 0.f; Push(10,F(a),F(b)); return rnd; }
inline void __cdecl FakeRel(CVector* out,void* placeable){
    if(!Main())return; const CVector v=rel[relIdx%rel.size()]; Push(11,P(placeable),relIdx++); *out=v; }
inline float __cdecl FakeAtten(float d){ if(!Main())return 0.f; Push(12,F(d)); return -float(F(d)%6000u)*0.01f; }
inline void __fastcall FakeRegRef(void* e,void*,void* ref){ if(!Main())return; Push(13,P(e),P(ref)); }
inline void __fastcall FakeCleanRef(void* e,void*,void* ref){ if(!Main())return; Push(14,P(e),P(ref)); }
inline void __cdecl FakeDelete(void* p){ Push(15,P(p)); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=24;
inline const std::array<Fn,NFN> fns{{
    {"CAEDataStream","Constructor",0x4DC620,0,-1,256},
    {"CAEDataStream","Destructor",0x4DC490,0,-1,256},
    {"CAEDataStream","FillBuffer",0x4DC1C0,0,-1,1024},
    {"CAEDataStream","GetCurrentPosition",0x4DC230,0,-1,256},
    {"CAEDataStream","Initialise",0x4DC2B0,0,-1,512},
    {"CAEDataStream","Seek-OG",0x4DC250,0,-1,1024},
    {"CAEWeaponAudioEntity","Initialise",0x503450,0,-1,128},
    {"CAEWeaponAudioEntity","Terminate",0x503480,0,-1,64},
    {"CAEWeaponAudioEntity","Reset",0x503490,0,-1,128},
    {"CAEWeaponAudioEntity","StopFlameThrowerIdleGasLoop",0x5034E0,0,-1,128},
    {"CAEWeaponAudioEntity","PlayGoggleSound",0x503500,0,-1,256},
    {"CAEWeaponAudioEntity","WeaponReload",0x503690,0,-1,512},
    {"CAEWeaponAudioEntity","PlayFlameThrowerIdleGasLoop",0x503870,0,-1,128},
    {"CAEWeaponAudioEntity","ReportChainsawEvent",0x503910,0,-1,512},
    {"CAEWeaponAudioEntity","ReportStealthKill",0x503B20,0,-1,256},
    {"CAEWeaponAudioEntity","PlayGunSounds",0x503CE0,0,-1,1024},
    {"CAEWeaponAudioEntity","PlayFlameThrowerSounds",0x504470,0,-1,256},
    {"CAEWeaponAudioEntity","PlayWeaponLoopSound",0x504610,0,-1,256},
    {"CAEWeaponAudioEntity","PlayCameraSound",0x5046F0,0,-1,128},
    {"CAEWeaponAudioEntity","PlayMiniGunFireSounds",0x5047C0,0,-1,512},
    {"CAEWeaponAudioEntity","PlayMiniGunStopSound",0x504960,0,-1,256},
    {"CAEWeaponAudioEntity","PlayChainsawStopSound",0x504AA0,0,-1,256},
    {"CAEWeaponAudioEntity","UpdateParameters",0,0x862E68,0,1024},
    {"CAEWeaponAudioEntity","WeaponFire",0x504F80,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"audio20-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"audio20-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio20");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"audio20-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f){
        hooks[f]=FindHook(fns[f].cls,fns[f].name);
        const bool vmt=fns[f].slot>=0;
        if(!hooks[f]||(hooks[f]->Symbol()[0]=='V')!=vmt){Write({{"event","error"},{"reason","hook not found or wrong kind"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    }
    const uintptr_t dllStreamVt=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CAEDataStream"));

    // --- ficheros de prueba propios (nunca ficheros de audio del juego) ---
    const auto artifacts=std::filesystem::temp_directory_path()/"gta_audio20";
    std::filesystem::create_directories(artifacts);
    { std::ofstream file(artifacts/"stream.bin",std::ios::binary); for(unsigned k=0;k<FILE_SIZE_BYTES;++k)file.put(char((k*131+17)^(k>>5))); }
    std::filesystem::remove(artifacts/"missing.bin");
    strcpy(nameBuf,(artifacts/"stream.bin").string().c_str());
    strcpy(missBuf,(artifacts/"missing.bin").string().c_str());
    auto Open=[&]{ return CreateFileA(nameBuf,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr); };
    auto Alive=[](HANDLE h){ DWORD flags{}; return GetHandleInformation(h,&flags)!=FALSE; };

    // estado global vigilado
    const uintptr_t dllForeground=Body(std::bit_cast<uintptr_t>(&IsForegroundApp));
    const uint32 savedTime=G<uint32>(TIME_ADDR); const int32 savedArea=G<int32>(AREA_ADDR);
    std::vector<Saved> savedAll{ Save(WEAPON_VT,4), Save(0x86C2AC,4), Save(STREAM_VT,14*4), Save(0x746060,5), Save(dllForeground,5),
        Save(TIME_ADDR,4), Save(AREA_ADDR,4), Save(0x8214BD,5), Save(0x8CC298,12) };
    for(uintptr_t a:{0x4EFB10u,0x4EFCD0u,0x4EF570u,0x4EF520u,0x4EFC60u,0x4D88C0u,0x4D88A0u,0x506D90u,0x4D9C80u,0x4D9C50u,0x4D8340u,0x4D7F20u,0x571B70u,0x571A00u})savedAll.push_back(Save(a,5));

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
            pt.Add(0x4EFB10,FakeRequest); pt.Add(Body(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound)),FakeRequest);
            pt.Add(0x4EFCD0,FakeCancelOwned); pt.Add(Body(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsOwnedByAudioEntity)),FakeCancelOwned);
            pt.Add(0x4EF570,FakeEventPlaying); pt.Add(Body(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity)),FakeEventPlaying);
            pt.Add(0x4EF520,FakeSlotPlaying); pt.Add(Body(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsPlayingInBankSlot)),FakeSlotPlaying);
            pt.Add(0x4EFC60,FakeCancelSlot); pt.Add(Body(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsInBankSlot)),FakeCancelSlot);
            pt.Add(0x4D88C0,FakeBankLoaded); pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded)),FakeBankLoaded);
            pt.Add(0x4D88A0,FakeLoadBank); pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank)),FakeLoadBank);
            pt.Add(0x506D90,FakeTune); pt.Add(Body(bit_cast<uintptr_t>(&CAudioEngine::IsLoadingTuneActive)),FakeTune);
            pt.Add(0x4D9C80,FakeProb); pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability)),FakeProb);
            pt.Add(0x4D9C50,FakeRnd); pt.Add(Body(bit_cast<uintptr_t>(static_cast<float(*)(float,float)>(&CAEAudioUtility::GetRandomNumberInRange<float>))),FakeRnd);
            pt.Add(0x4D8340,FakeRel); pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetPositionRelativeToCamera_Placeable)),FakeRel);
            pt.Add(0x4D7F20,FakeAtten); pt.Add(Body(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetDistanceAttenuation)),FakeAtten);
            pt.Add(0x571B70,FakeRegRef); pt.Add(Body(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference))),FakeRegRef);
            pt.Add(0x571A00,FakeCleanRef); pt.Add(Body(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference))),FakeCleanRef);
        };

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* w=static_cast<uint8*>(p); for(size_t k=0;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(w+k,&v,4); } for(size_t k=n&~size_t(3);k<n;++k)w[k]=uint8(next()); };
                auto pick=[&](std::initializer_list<int32> l){ return *(l.begin()+next()%l.size()); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA20u; if(!stt)stt=1;
                    nrec=0; nreq=0; probIdx=0; relIdx=0; reqIdx=0;
                    for(auto* h:hooks)h->State(candidate);
                    // Como pide el banco: el conmutador se comprueba después de fijar el estado de la ejecución
                    const bool vmt=fn.slot>=0;
                    const uintptr_t vtTarget=vmt ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=vmt ? (candidate ? !InExe(vtTarget) : vtTarget==0x504B70) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=vmt ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // --- estado común del caso ---
                    const uint32 now=next()%8 ? 100000+next()%2000000 : (next()%2 ? next()%400 : 0xFFFFFF00u+next()%256);
                    G<uint32>(TIME_ADDR)=now;
                    G<int32>(AREA_ADDR)=pick({0,0,1,5,18});
                    bankWpn=uint8(next()%8!=0); bankCol=uint8(next()%6!=0); bankVeh=uint8(next()%6!=0); bankSaw=uint8(next()%6!=0); tune=uint8(next()%3==0);
                    evPlaying=int16(pick({0,0,0,1,2})); slotPlaying=int16(pick({0,0,1,2}));
                    for(auto& p:probs)p=uint8(next()&1);
                    rnd=uni(-0.02f,0.02f);
                    for(auto& r:rel){ const float m=next()%6==0 ? uni(0.f,2.f) : next()%6==0 ? uni(30.f,200.f) : uni(0.f,25.f);
                        CVector d{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float l=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z);
                        if(l<0.01f)d=CVector{1.f,0.f,0.f}; else d=CVector{d.x/l,d.y/l,d.z/l};
                        r=CVector{d.x*m,d.y*m,d.z*m}; }
                    for(auto& s:snd)fill(s.data(),SND_SIZE);
                    for(auto& r:reqRet)r=next()%5 ? static_cast<void*>(snd[2+next()%(NSND-2)].data()) : nullptr;

                    {
                        Patches pt; install(pt);

                        if(cls=="CAEDataStream"){
                            uint8* s=ds.data(); fill(s,ds.size());
                            At<uintptr_t>(s,0)=STREAM_VT;
                            auto vptrType=[&](uintptr_t v)->json{ return v==STREAM_VT||v==dllStreamVt ? json("CAEDataStream") : json(v); };
                            const uint32 start=uint32(pick({0,0,7,100,1000,4000,5000}));
                            const uint32 length=uint32(next()%5==0 ? next() : uint32(pick({0,16,512,1000,4096,8000})));
                            At<uint32>(s,0x14)=start; At<uint32>(s,0x18)=length;
                            At<uint32>(s,0x10)=next()%4 ? start+length+next()%600 : next()%3 ? start+next()%5000 : next();
                            At<uint8>(s,0x20)=uint8(next()%2); At<uint32>(s,0x1C)=next()%8;
                            HANDLE owned=INVALID_HANDLE_VALUE; bool closedByCall=false;
                            if(nm=="Constructor"){
                                std::memset(s,0xCD,ds.size());
                                const uint32 enc=uint32(next()%8==0 ? pick({2,0x80,0xFF}) : int32(next()%2)) | (next()&0xFFFFFF00u);
                                char* name=next()%4 ? nameBuf : next()%2 ? nullptr : reinterpret_cast<char*>(uintptr_t(0x5A5A0000u));
                                const uint32 st=next()%3 ? uint32(pick({0,7,100})) : next(); const uint32 ln=next()%3 ? uint32(pick({0,16,4096})) : next();
                                void* r=reinterpret_cast<void*(__thiscall*)(void*,int32,char*,uint32,uint32,uint32)>(a)(s,int32(next()),name,st,ln,enc);
                                ret=P(r);
                            }
                            else if(nm=="Destructor"){
                                const uint8 open=uint8(pick({0,1,1,2}));
                                At<uint8>(s,0xC)=open;
                                if(open){ owned=Open(); if(owned==INVALID_HANDLE_VALUE){ out["open_failed"]=true; return out; } At<HANDLE>(s,4)=owned; }
                                At<char*>(s,8)=next()%4 ? nameBuf : next()%2 ? nullptr : reinterpret_cast<char*>(uintptr_t(0x5A5A0010u));
                                { EntryPatch d(0x8214BD,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(a)(s); }
                                closedByCall=open!=0;
                            }
                            else if(nm=="Initialise"){
                                const uint8 open=uint8(next()%4==0 ? pick({1,2}) : 0);
                                At<uint8>(s,0xC)=open;
                                At<char*>(s,8)=next()%3 ? nameBuf : next()%4 ? missBuf : nullptr;
                                const bool r=reinterpret_cast<bool(__thiscall*)(void*)>(a)(s);
                                ret=uint8(r);
                                const HANDLE h=At<HANDLE>(s,4);
                                out["handle_invalid"]=h==INVALID_HANDLE_VALUE;
                                if(!open&&h!=INVALID_HANDLE_VALUE){ owned=h; }
                            }
                            else {
                                At<uint8>(s,0xC)=uint8(next()%4 ? 1 : 0);
                                owned=Open(); if(owned==INVALID_HANDLE_VALUE){ out["open_failed"]=true; return out; }
                                At<HANDLE>(s,4)=owned;
                                SetFilePointer(owned,LONG(next()%3 ? next()%FILE_SIZE_BYTES : next()%6000),nullptr,FILE_BEGIN);
                                if(nm=="FillBuffer"){
                                    buf.fill(0xA5);
                                    const uint32 size=next()%6==0 ? uint32(next()%BUF_SIZE) : uint32(pick({0,1,15,128,511,512,1024,2000}));
                                    // a veces la ventana del track deja pocos bytes por leer
                                    if(next()%3==0)At<uint32>(s,0x10)=start+length+next()%64;
                                    ret=uint32(reinterpret_cast<size_t(__thiscall*)(void*,void*,size_t)>(a)(s,buf.data(),size));
                                    out["buffer"]=ReversingProbe::Bytes(buf.data(),buf.size());
                                }
                                else if(nm=="GetCurrentPosition") ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(s);
                                else { // Seek-OG
                                    const int32 off=next()%5==0 ? int32(next()%20000)-10000 : pick({-1024,-1,0,1,17,128,512,4096});
                                    const int32 whence=next()%8 ? int32(next()%3) : pick({3,-1,7});
                                    ret=reinterpret_cast<uint32(__thiscall*)(void*,long,int32)>(a)(s,long(off),whence);
                                }
                            }
                            auto bytes=ReversingProbe::Bytes(s,DS_SIZE);
                            out["vptr"]=vptrType(At<uintptr_t>(s,0)); std::fill_n(bytes.begin(),4,0);
                            if(nm!="Constructor"){ out["handle_same"]=At<HANDLE>(s,4)==owned; std::fill_n(bytes.begin()+4,4,0); }
                            out["obj"]=bytes;
                            if(owned!=INVALID_HANDLE_VALUE){
                                if(closedByCall) out["handle_closed"]=!Alive(owned); // cerrado por el destructor: no se vuelve a cerrar
                                else { out["file_position"]=SetFilePointer(owned,0,nullptr,FILE_CURRENT); CloseHandle(owned); }
                            }
                        }
                        else { // CAEWeaponAudioEntity
                            uint8* e=ent.data(); fill(e,ent.size());
                            At<uintptr_t>(e,0)=WEAPON_VT;
                            // entidad física falsa: peatón, coche o avión, con o sin matriz
                            std::memset(phys.data(),0,phys.size()); fill(mat.data(),mat.size());
                            for(size_t o:{size_t(4),size_t(8),size_t(0xC)})At<float>(phys.data(),o)=uni(-3000.f,3000.f);
                            for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(mat.data(),o)=uni(-3000.f,3000.f);
                            At<void*>(phys.data(),0x14)=next()%3 ? static_cast<void*>(mat.data()) : nullptr;
                            At<uint8>(phys.data(),0x36)=uint8((next()&0xF8)|uint32(pick({2,2,3,4})));
                            At<int32>(phys.data(),0x594)=pick({4,4,0,3,5,9});
                            CPhysical* const pe=reinterpret_cast<CPhysical*>(phys.data());
                            // m_tempSound (+8): entidad física previa nula, la falsa o una constante (los falsos no la leen)
                            At<uint32>(e,0x10)=uint32(pick({0,0,1,2})==1 ? uintptr_t(pe) : 0);
                            if(next()%5==0)At<uint32>(e,0x10)=0x5A5A0020u;
                            At<uint8>(e,0x7C)=uint8(next()%8 ? next()%2 : next()); At<uint8>(e,0x7D)=uint8(next()%8 ? next()%2 : next());
                            At<uint8>(e,0x7E)=uint8(next()%10 ? next()%2 : pick({0x80,0xFF,0x7F,2}));
                            At<uint8>(e,0x7F)=uint8(next()%8 ? next()%4 : next()); At<uint8>(e,0x80)=uint8(next()%8 ? next()%5 : next());
                            for(size_t o=0x84;o<=0x98;o+=4){ switch(next()%6){ case 0: At<uint32>(e,o)=0; break; case 1: At<uint32>(e,o)=next(); break;
                                case 2: At<uint32>(e,o)=now-300+next()%3-1; break; case 3: At<uint32>(e,o)=now-400+next()%3-1; break; default: At<uint32>(e,o)=now-next()%700; } }
                            At<void*>(e,0x9C)=next()%3 ? nullptr : static_cast<void*>(snd[next()%2 ? 0 : 1].data());
                            // sonido de UpdateParameters
                            uint8* s0=snd[0].data();
                            At<void*>(s0,4)=e; At<void*>(s0,8)=next()%5 ? static_cast<void*>(pe) : nullptr;
                            At<int32>(s0,0xC)=next()%10 ? pick({1,2,3,4,5,6,7,8,9,10,11,12,13,14,0,15,-1}) : int32(next()%400);
                            At<float>(s0,0x10)=next()%6 ? float(int32(now)-int32(next()%4000)) : next()%2 ? uni(-5000.f,5000.f) : float(next()%0x7FFFFF)*float(1u<<(next()%9));
                            At<float>(s0,0x14)=next()%6 ? uni(-45.f,12.f) : float(pick({-30,0,-14,-16,-12}));
                            At<float>(s0,0x1C)=next()%6 ? uni(0.5f,1.3f) : std::bit_cast<float>(uint32(pick({0x3F59999A,0x3F800000,0x3F570A3D,0x3F59999B,0x3F7FFFFF})));
                            for(size_t k=1;k<NSND;++k){ At<void*>(snd[k].data(),4)=next()%2 ? static_cast<void*>(e) : nullptr; At<void*>(snd[k].data(),8)=next()%3 ? nullptr : static_cast<void*>(pe); }

                            const int32 evGun=next()%6 ? pick({0x91,0x91,0x95,0x96,0x97,0x98}) : int32(next()%300);
                            auto vol=[&]{ return next()%3 ? float(pick({0,-7,-14,-20,-6})) : uni(-30.f,10.f); };
                            using VoidFn=void(__thiscall*)(void*);
                            if(nm=="Initialise"||nm=="Terminate"||nm=="Reset"||nm=="StopFlameThrowerIdleGasLoop") reinterpret_cast<VoidFn>(a)(e);
                            else if(nm=="WeaponFire"){
                                const int32 type=next()%6 ? 22+int32(next()%24) : pick({0,1,4,9,16,21,46,47,-1});
                                reinterpret_cast<void(__thiscall*)(void*,int32,void*,int32)>(a)(e,type,next()%10 ? pe : nullptr,evGun);
                            }
                            else if(nm=="WeaponReload"){
                                const int32 type=next()%6 ? 22+int32(next()%13) : pick({0,4,35,37,38,46,21});
                                const int32 ev=next()%5 ? pick({0x92,0x93}) : int32(next()%300);
                                reinterpret_cast<void(__thiscall*)(void*,int32,void*,int32)>(a)(e,type,next()%10 ? pe : nullptr,ev);
                            }
                            else if(nm=="PlayGunSounds"){
                                auto id=[&](int32 v){ return int16(next()%6==0 ? -1 : v); };
                                const int16 dry=id(pick({0x34,0x4C,0x49,0x1D,0x21,0x31})), sub=id(pick({0x35,0x4D,0x4A,0x1E}));
                                const int16 ml=id(pick({6,0x18,0x15,0,0x11,3,0x1A,11})), mr=id(pick({7,0x18,0x16,1,0x12,4,0x1B,12})), tl=id(pick({8,0x17,2,5,13}));
                                const float spd=next()%3 ? std::bit_cast<float>(uint32(pick({0x3F800000,0x3FB504D5,0x3F71A177,0x3F4B2FEC,0x3FA1450F,0x3F63D70A}))) : uni(0.5f,1.5f);
                                const float tf=next()%3 ? std::bit_cast<float>(uint32(pick({0x3F800000,0x3F6E147B}))) : uni(0.5f,1.5f);
                                reinterpret_cast<void(__thiscall*)(void*,void*,int16,int16,int16,int16,int16,int32,float,float,float)>(a)(e,pe,dry,sub,ml,mr,tl,evGun,vol(),spd,tf);
                            }
                            else if(nm=="PlayGoggleSound"){
                                const int16 sfx=int16(next()%4 ? 0x40 : int16(next()%90));
                                reinterpret_cast<void(__thiscall*)(void*,int16,int32)>(a)(e,sfx,next()%4 ? 0x91 : int32(next()%300));
                            }
                            else if(nm=="ReportStealthKill"){
                                const int32 type=next()%5 ? 4 : pick({0,3,5,22});
                                reinterpret_cast<void(__thiscall*)(void*,int32,void*,int32)>(a)(e,type,pe,next()%4 ? pick({0x8B,0x91}) : int32(next()%300));
                            }
                            else if(nm=="ReportChainsawEvent"){
                                const int32 ev=next()%6 ? pick({0x99,0x9A,0x9B}) : int32(next()%300);
                                reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(a)(e,pe,ev);
                            }
                            else if(nm=="PlayChainsawStopSound"||nm=="PlayMiniGunStopSound"||nm=="PlayFlameThrowerIdleGasLoop"){
                                void* p=nm=="PlayFlameThrowerIdleGasLoop"||next()%8 ? static_cast<void*>(pe) : nullptr;
                                reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(e,p);
                            }
                            else if(nm=="PlayWeaponLoopSound"){
                                const int16 sfx=int16(pick({0x1C,9,0x1C,int32(next()%90)}));
                                const float spd=next()%2 ? std::bit_cast<float>(uint32(pick({0x3F800000,0x3F4B2FEC}))) : uni(0.5f,1.5f);
                                reinterpret_cast<void(__thiscall*)(void*,void*,int16,int32,float,float,int32)>(a)(e,pe,sfx,next()%4 ? 0x91 : int32(next()%300),vol(),spd,pick({3,4,3,4,1,7}));
                            }
                            else if(nm=="PlayMiniGunFireSounds"){
                                const int32 ev=next()%6 ? pick({0x91,0x95,0x96,0x97,0x98}) : int32(next()%300);
                                reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(a)(e,pe,ev);
                            }
                            else if(nm=="PlayFlameThrowerSounds"){
                                const int16 dry=int16(next()%5 ? 0x53 : -1), fl=int16(next()%5 ? 0x1A : -1);
                                const float spd=next()%2 ? 1.f : uni(0.5f,1.5f);
                                reinterpret_cast<void(__thiscall*)(void*,void*,int16,int16,int32,float,float)>(a)(e,pe,dry,fl,next()%4 ? 0x91 : int32(next()%300),vol(),spd);
                            }
                            else if(nm=="PlayCameraSound"){
                                reinterpret_cast<void(__thiscall*)(void*,void*,int32,float)>(a)(e,pe,next()%4 ? 0x91 : int32(next()%300),next()%2 ? -14.f : uni(-20.f,5.f));
                            }
                            else { // UpdateParameters (hueco 0 de la vtable 0x862E68)
                                if(next()%5==0)At<void*>(e,0x9C)=s0;
                                const int16 pp=next()%3 ? int16(pick({-1,-1,-2,0,1,500,1000,1001,32767,-32768})) : int16(next());
                                reinterpret_cast<void(__thiscall*)(void*,void*,int16)>(a)(e,s0,pp);
                            }
                            out["ent"]=ReversingProbe::Bytes(e,ENT_SIZE);
                            json sl=json::array(); for(auto& s:snd)sl.push_back(ReversingProbe::Bytes(s.data(),SND_SIZE)); out["snd"]=sl;
                            out["phys"]=Fnv(phys.data(),phys.size()); out["mat"]=Fnv(mat.data(),mat.size());
                        }
                    }
                    G<uint32>(TIME_ADDR)=savedTime; G<int32>(AREA_ADDR)=savedArea;
                    out["ret"]=ret; out["recs"]=Recs(); out["reqs"]=Reqs(); return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",std::min(nrec,recs.size())},{"requests",std::min(nreq,reqs.size())}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
        SetEvent(resumeEvt); // el hilo de streaming sigue con todos los hooks como estaban
    }
    G<uint32>(TIME_ADDR)=savedTime; G<int32>(AREA_ADDR)=savedArea;
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
