#pragma once
// Banco dentro del motor: audio de colisiones (lista de 17).
// 17 funciones de CAECollisionAudioEntity. Original (hooks del lote desactivados) frente a candidato con fixtures
// deterministas. Nada toca el AudioEngine real, AESoundManager, AEAudioHardware, el cargador de bancos ni el mundo:
// - Todo corre sobre una CAECollisionAudioEntity falsa (vtable del EXE 0x862E64, contenido aleatorio por caso) y
//   entidades falsas (tipo, modelo, matriz, velocidades, flags físicos, inteligencia y subtipo de vehículo con los
//   offsets crudos del EXE). Los sonidos de la lista apuntan a un pool de CAESound falsas.
// - Se simulan y trazan, en el EXE y en la DLL: CAESound::Initialise (escribe los mismos campos en las dos
//   pasadas), RegisterWithPhysicalEntity, SetPosition, StopSoundAndForget, CAESoundManager::RequestNewSound
//   (registra la voz pedida y si es m_tempSound o una local) y AreSoundsOfThisEventPlayingForThisEntityAndPhysical,
//   CAEAudioHardware::IsSoundBankLoaded/LoadSoundBank, CAudioEngine::IsLoadingTuneActive,
//   CPedIntelligence::GetTaskSwim, rand del EXE y CGeneral::GetRandomNumber.
// - ReportCollision corre con PlayOneShot/PlayLooping/UpdateLoopingCollisionSound simulados (se registran sus
//   argumentos; GetCollisionSoundStatus es real) y ReportBulletHit con PlayBulletHitCollisionSound simulado.
// - Los falsos solo registran en el hilo principal, se ponen después de fijar el estado de los hooks y se quitan
//   en orden inverso al acabar cada pasada (nunca están puestos durante Write/PumpMessages).
// - CTimer::m_snTimeInMilliseconds, MI_BASKETBALL/MI_PUNCHBAG/MI_GRASSHOUSE/MI_IMY_GRAY_CRATE y la entrada 19990
//   de CModelInfo::ms_modelInfoPtrs se fijan por caso y se restauran tras cada pasada y al final.
// - Fuera de los fixtures (rutas FIX_BUGS/NOTSA de la DLL o lecturas fuera de tabla del EXE): superficies >= 179 en
//   PlayLoopingCollisionSound y PlayBulletHitCollisionSound, superficies >= 195 en las demás, y edificios con
//   superficie especial en ReportCollision (GetFrictionEffect fuera de tabla).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Entities/AECollisionAudioEntity.h"
#include "Audio/AESound.h"
#include "Audio/Managers/AESoundManager.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/AudioEngine.h"
#include "PedIntelligence.h"
#include "ModelIndices.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace Audio17Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& G(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

// --- traza plana sin reservas: [n, tag, valores...] ---
inline std::array<uint32,32768> tr{}; inline size_t ntr{};
template<class... A> inline void Ev(uint32 tag,A... a){
    if(!Main())return;
    const uint32 v[]{tag,uint32(a)...}; constexpr size_t n=sizeof...(A)+1;
    if(ntr+n+1<=tr.size()){ tr[ntr++]=uint32(n); for(size_t k=0;k<n;++k)tr[ntr++]=v[k]; } else ntr=tr.size(); }
inline json Trace(){
    if(ntr<=600){ json r=json::array(); for(size_t i=0;i<ntr;++i)r.push_back(tr[i]); return r; }
    json head=json::array(); for(size_t i=0;i<128;++i)head.push_back(tr[i]);
    return {{"count",ntr},{"hash",Fnv(tr.data(),ntr*4)},{"head",head}}; }

// --- objetos falsos ---
constexpr size_t ENT_SIZE=0x1978, NSND=16, NOBJ=6, LIST=0x208, NENT=300;
constexpr uintptr_t MI_BASKET=0x8CD774, MI_PUNCH=0x8CD77C, MI_GRASS=0x8CD6F4, MI_CRATE=0x8CD780, MI_PTRS=0xA9B0C8, TIME_ADDR=0xB7CB84;
constexpr int32 COL_MODEL=19990;
alignas(16) inline std::array<uint8,ENT_SIZE> entBuf{};
inline uint8* E(){ return entBuf.data(); }
alignas(16) inline std::array<std::array<uint8,0x74>,NSND> sndPool{};
alignas(16) inline std::array<std::array<uint8,0x800>,NOBJ> objs{};
alignas(16) inline std::array<std::array<uint8,0x48>,NOBJ> mats{};
alignas(16) inline std::array<float,8> posBuf{}, normalBuf{};
alignas(16) inline std::array<uint8,0x40> intelBuf{}, swimBuf{}, miBuf{}, colModelBuf{}, colDataBuf{}, sphBuf{}, boxBuf{}, triBuf{};

struct Region { uintptr_t base; size_t size; uint32 tag; };
inline std::array<Region,24> regions{}; inline size_t nregions{};
inline void AddRegion(const void* b,size_t n,uint32 tag){ if(nregions<regions.size())regions[nregions++]={uintptr_t(b),n,tag}; }
inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregions;++k){ const auto& r=regions[k]; if(u>=r.base&&u<r.base+r.size)return (r.tag<<20)|uint32(u-r.base); }
    return 0xEEEEEEEEu; }
// 1 = m_tempSound de la entidad falsa; 2 = otra voz (local en la pila); resto, región conocida.
inline uint32 Where(const void* s){ if(s==E()+8)return 1u; const uint32 p=P(s); return p==0xEEEEEEEEu ? 2u : p; }

// --- falsos (registran solo en el hilo principal) ---
inline uint32 seed{}, rngCalls{};
inline uint32 bankMask{}, bankCalls{}, tuneMask{}, tuneCalls{}, playMask{}, playCalls{}, reqMask{}, reqCalls{}, swimMask{}, swimCalls{};
inline bool Bit(uint32 m,uint32& c){ return (m>>(c++%32))&1u; }
inline int __cdecl FakeRand(){ if(!Main())return 0; Ev(1); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline void __fastcall FakeInit(uint8* s,void*,uint32 slot,uint32 id,void* ent,float px,float py,float pz,float vol,float roll,float spd,float dop,uint32 fd,uint32 flags,float var,uint32 pt){
    if(!Main())return;
    Ev(2,Where(s),slot&0xFFFF,id&0xFFFF,P(ent),F(px),F(py),F(pz),F(vol),F(roll),F(spd),F(dop),fd&0xFF,flags&0xFFFF,F(var),pt&0xFFFF);
    if(At<void*>(s,8)){ Ev(3,P(At<void*>(s,8))); At<void*>(s,8)=nullptr; }
    At<int16>(s,0)=int16(slot); At<int16>(s,2)=int16(id); At<void*>(s,4)=ent; At<int32>(s,0xC)=-1; At<float>(s,0x10)=-1.f;
    At<float>(s,0x14)=vol; At<float>(s,0x18)=roll; At<float>(s,0x1C)=spd; At<float>(s,0x20)=var;
    At<float>(s,0x24)=px; At<float>(s,0x28)=py; At<float>(s,0x2C)=pz; At<float>(s,0x50)=dop;
    At<uint8>(s,0x54)=uint8(fd); At<uint16>(s,0x56)=uint16(flags); At<int16>(s,0x5C)=int16(pt); }
inline CAESound* __fastcall FakeRequest(void*,void*,uint8* s){
    if(!Main())return nullptr;
    Ev(4,Where(s),At<uint16>(s,0),At<uint16>(s,2),P(At<void*>(s,4)),P(At<void*>(s,8)),At<uint32>(s,0xC),At<uint32>(s,0x10),At<uint32>(s,0x14),
       At<uint32>(s,0x18),At<uint32>(s,0x1C),At<uint32>(s,0x20),At<uint32>(s,0x24),At<uint32>(s,0x28),At<uint32>(s,0x2C),At<uint32>(s,0x50),
       At<uint8>(s,0x54),At<uint16>(s,0x56),At<uint16>(s,0x5C));
    if(!Bit(reqMask,reqCalls))return nullptr;
    return reinterpret_cast<CAESound*>(sndPool[reqCalls%NSND].data()); }
inline void __fastcall FakeRegister(uint8* s,void*,void* e){ if(!Main())return; Ev(5,Where(s),P(e)); At<void*>(s,8)=e; }
inline void __fastcall FakeSetPosition(uint8* s,void*,float x,float y,float z){ if(!Main())return; Ev(6,Where(s),F(x),F(y),F(z)); }
inline void __fastcall FakeStop(uint8* s,void*){ if(!Main())return; Ev(7,Where(s)); }
inline bool __fastcall FakeIsLoaded(void*,void*,uint32 bank,uint32 slot){ if(!Main())return true; Ev(8,bank&0xFFFF,slot&0xFFFF); return Bit(bankMask,bankCalls); }
inline void __fastcall FakeLoad(void*,void*,uint32 bank,uint32 slot){ if(!Main())return; Ev(9,bank&0xFFFF,slot&0xFFFF); }
inline bool __fastcall FakeTune(void*,void*){ if(!Main())return false; Ev(10); return Bit(tuneMask,tuneCalls); }
inline int16 __fastcall FakeArePlaying(void*,void*,uint32 ev,void* ae,void* phys){ if(!Main())return 0; Ev(11,ev&0xFFFF,P(ae),P(phys)); return int16(Bit(playMask,playCalls)); }
inline void* __fastcall FakeSwim(void* intel,void*){ if(!Main())return nullptr; Ev(12,P(intel)); return Bit(swimMask,swimCalls) ? swimBuf.data() : nullptr; }
// Funciones de la propia clase simuladas en ReportCollision/ReportBulletHit (bytes de superficie y bool enmascarados).
inline void __fastcall FakeOneShot(void* self,void*,void* a,void* b,uint32 sA,uint32 sB,float imp,const CVector* pos){
    if(!Main())return; Ev(20,P(self),P(a),P(b),sA&0xFF,sB&0xFF,F(imp),P(pos)); }
inline void __fastcall FakeLooping(void* self,void*,void* a,void* b,uint32 sA,uint32 sB,float imp,const CVector* pos,uint32 force){
    if(!Main())return; Ev(21,P(self),P(a),P(b),sA&0xFF,sB&0xFF,F(imp),P(pos),force&0xFF); }
inline void __fastcall FakeUpdate(void* self,void*,void* snd,void* a,void* b,uint32 sA,uint32 sB,float imp,const CVector* pos,uint32 force){
    if(!Main())return; Ev(22,P(self),P(snd),P(a),P(b),sA&0xFF,sB&0xFF,F(imp),P(pos),force&0xFF); }
inline void __fastcall FakeBullet(void* self,void*,uint32 surf,const CVector* pos,float angle){
    if(!Main())return; Ev(23,P(self),surf&0xFF,P(pos),F(angle)); }

// --- parches de una pasada: se deshacen en orden inverso ---
struct Patches {
    std::array<std::unique_ptr<ReversingProbe::EntryPatch>,40> v{}; size_t n{};
    template<class Fn> void Add(uintptr_t at,Fn f){ v[n++]=std::make_unique<ReversingProbe::EntryPatch>(at,f); }
    ~Patches(){ while(n)v[--n].reset(); }
};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,17> fns{{
    {"CAECollisionAudioEntity","Initialise",0x5B9BD0,0,-1,64},
    {"CAECollisionAudioEntity","InitialisePostLoading",0x4DA050,0,-1,16},
    {"CAECollisionAudioEntity","Reset",0x4DA320,0,-1,256},
    {"CAECollisionAudioEntity","Service",0x4DA2C0,0,-1,512},
    {"CAECollisionAudioEntity","AddCollisionSoundToList",0x4DAAC0,0,-1,512},
    {"CAECollisionAudioEntity","GetCollisionSoundStatus",0x4DA830,0,-1,1024},
    {"CAECollisionAudioEntity","ChooseCollisionSoundID",0x4DAA50,0,-1,1024},
    {"CAECollisionAudioEntity","PlayOneShotCollisionSound",0x4DB150,0,-1,1024},
    {"CAECollisionAudioEntity","PlayLoopingCollisionSound",0x4DB450,0,-1,1024},
    {"CAECollisionAudioEntity","UpdateLoopingCollisionSound",0x4DA540,0,-1,1024},
    {"CAECollisionAudioEntity","PlayBulletHitCollisionSound",0x4DB7C0,0,-1,1024},
    {"CAECollisionAudioEntity","ReportGlassCollisionEvent",0x4DA070,0,-1,512},
    {"CAECollisionAudioEntity","ReportWaterSplash-at-position",0x4DA190,0,-1,256},
    {"CAECollisionAudioEntity","ReportWaterSplash-for-physical",0x4DAE40,0,-1,1024},
    {"CAECollisionAudioEntity","ReportObjectDestruction",0x4DAB60,0,-1,512},
    {"CAECollisionAudioEntity","ReportCollision",0x4DBA10,0,-1,1024},
    {"CAECollisionAudioEntity","ReportBulletHit",0x4DBDF0,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audio17-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"audio17-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio17");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"audio17-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,17> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,17> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    AddRegion(entBuf.data(),entBuf.size(),1); AddRegion(sndPool.data(),sizeof(sndPool),2); AddRegion(objs.data(),sizeof(objs),3); AddRegion(mats.data(),sizeof(mats),4);
    AddRegion(posBuf.data(),sizeof(posBuf),5); AddRegion(normalBuf.data(),sizeof(normalBuf),6); AddRegion(intelBuf.data(),intelBuf.size(),7); AddRegion(swimBuf.data(),swimBuf.size(),8);
    AddRegion(miBuf.data(),miBuf.size(),9); AddRegion(colModelBuf.data(),colModelBuf.size(),10); AddRegion(colDataBuf.data(),colDataBuf.size(),11);

    // estado global guardado
    const uint32 savedTime=G<uint32>(TIME_ADDR);
    const uint16 savedBasket=G<uint16>(MI_BASKET), savedPunch=G<uint16>(MI_PUNCH), savedGrass=G<uint16>(MI_GRASS), savedCrate=G<uint16>(MI_CRATE);
    void* const savedMi=G<void*>(MI_PTRS+4*COL_MODEL);
    auto restore=[&]{ G<uint32>(TIME_ADDR)=savedTime; G<uint16>(MI_BASKET)=savedBasket; G<uint16>(MI_PUNCH)=savedPunch; G<uint16>(MI_GRASS)=savedGrass; G<uint16>(MI_CRATE)=savedCrate; G<void*>(MI_PTRS+4*COL_MODEL)=savedMi; };

    bool passed=true; int pair=0;
    using std::bit_cast;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA17u; if(!stt)stt=1; ntr=0; seed=uint32(i)*7919u+uint32(f); rngCalls=0;
                bankCalls=tuneCalls=playCalls=reqCalls=swimCalls=0; bankMask=next(); tuneMask=next(); playMask=next(); reqMask=next(); swimMask=next();
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                const uintptr_t a=fn.address;
                json out={{"switches",switches}}; json ret=nullptr;
                uint8* const e=E();

                // --- fixtures comunes ---
                const uint32 now=next()%3 ? 100000+next()%2000000 : next(); G<uint32>(TIME_ADDR)=now;
                for(auto& b:entBuf)b=uint8(next());
                At<uintptr_t>(e,0)=0x862E64; At<void*>(e,0x10)=next()%4==0 ? static_cast<void*>(objs[5].data()) : nullptr; // m_tempSound.m_PhysicalEntity
                for(int k=0;k<195;++k)At<int16>(e,0x7C+2*k)=int16(int(next()%75)-2);
                At<int16>(e,0x202)=int16(int(next()%24)-1);
                At<int32>(e,0x204)=next()%10==0 ? 300 : int32(next()%301);
                auto pickObj=[&](int range)->void*{ const uint32 r=next()%uint32(range+1); return r==uint32(range) ? nullptr : static_cast<void*>(objs[r].data()); };
                const bool fullList=next()%8==0;
                for(size_t k=0;k<NENT;++k){ uint8* en=e+LIST+k*0x14;
                    At<void*>(en,0)=pickObj(4); At<void*>(en,4)=pickObj(4);
                    At<void*>(en,8)=(fullList||next()%3==0) ? static_cast<void*>(sndPool[next()%NSND].data()) : nullptr;
                    At<uint32>(en,0xC)=now+uint32(int32(next()%400)-200); en[0x10]=uint8(next()%4); en[0x11]=uint8(next()%196); en[0x12]=uint8(next()%196); en[0x13]=uint8(next()); }
                for(auto& s:sndPool)for(auto& b:s)b=uint8(next());
                for(auto& s:sndPool){ At<float>(s.data(),0x14)=uni(-40.f,10.f); At<float>(s.data(),0x1C)=uni(0.f,1.5f); At<void*>(s.data(),8)=nullptr; }
                // tipos: 1 edificio, 2 vehículo, 3 peatón, 4 objeto; buildings/otros solo en ReportCollision y ReportBulletHit
                const bool anyType=nm=="ReportCollision"||nm=="ReportBulletHit"||nm=="ReportObjectDestruction";
                for(size_t k=0;k<NOBJ;++k){ uint8* o=objs[k].data(); std::fill(o,o+0x800,uint8(0));
                    static const uint8 phys[]{2,3,4,2}; static const uint8 all[]{1,2,3,4,2,0,5,1};
                    const uint8 type=anyType ? all[next()%8] : phys[next()%4];
                    o[0x36]=uint8(type|(next()&0xF8));
                    uint8* m=mats[k].data(); for(int c=0;c<12;++c)At<float>(m,c*4+(c/3)*4)=uni(-1.f,1.f);
                    At<float>(m,0x30)=uni(-3000.f,3000.f); At<float>(m,0x34)=uni(-3000.f,3000.f); At<float>(m,0x38)=uni(0.f,100.f);
                    At<void*>(o,0x14)=(type==2||next()%3) ? static_cast<void*>(m) : nullptr;
                    At<float>(o,4)=uni(-3000.f,3000.f); At<float>(o,8)=uni(-3000.f,3000.f); At<float>(o,0xC)=uni(0.f,100.f);
                    At<int16>(o,0x22)=int16(400+next()%212);
                    o[0x40]=uint8(next()); o[0x41]=uint8(next());
                    const bool still=next()%8==0;
                    for(int c=0;c<6;++c)At<float>(o,0x44+4*c)=still ? 0.f : (next()%6==0 ? 0.f : uni(-0.6f,0.6f));
                    At<void*>(o,0x47C)=intelBuf.data();
                    At<uint32>(o,0x594)=next()%3==0 ? 10u : next()%12; }
                for(int c=0;c<3;++c)posBuf[c]=uni(-3000.f,3000.f);
                const auto* pos=reinterpret_cast<const CVector*>(posBuf.data());
                bool showTemp=false;

                {
                    Patches pt;
                    pt.Add(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber); pt.Add(0x821B1E,FakeRand);
                    pt.Add(bit_cast<uintptr_t>(&CAESound::Initialise),FakeInit); pt.Add(0x4EFE50,FakeInit);
                    pt.Add(bit_cast<uintptr_t>(&CAESound::RegisterWithPhysicalEntity),FakeRegister); pt.Add(0x4EF820,FakeRegister);
                    pt.Add(bit_cast<uintptr_t>(&CAESound::SetPosition),FakeSetPosition); pt.Add(0x4EF880,FakeSetPosition);
                    pt.Add(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget),FakeStop); pt.Add(0x4EF850,FakeStop);
                    pt.Add(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),FakeRequest); pt.Add(0x4EFB10,FakeRequest);
                    pt.Add(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntityAndPhysical),FakeArePlaying); pt.Add(0x4EF5D0,FakeArePlaying);
                    pt.Add(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),FakeIsLoaded); pt.Add(0x4D88C0,FakeIsLoaded);
                    pt.Add(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),FakeLoad); pt.Add(0x4D88A0,FakeLoad);
                    pt.Add(bit_cast<uintptr_t>(&CAudioEngine::IsLoadingTuneActive),FakeTune); pt.Add(0x506D90,FakeTune);
                    pt.Add(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),FakeSwim); pt.Add(0x601070,FakeSwim);
                    if(nm=="ReportCollision"){
                        pt.Add(bit_cast<uintptr_t>(&CAECollisionAudioEntity::PlayOneShotCollisionSound),FakeOneShot); pt.Add(0x4DB150,FakeOneShot);
                        pt.Add(bit_cast<uintptr_t>(&CAECollisionAudioEntity::PlayLoopingCollisionSound),FakeLooping); pt.Add(0x4DB450,FakeLooping);
                        pt.Add(bit_cast<uintptr_t>(&CAECollisionAudioEntity::UpdateLoopingCollisionSound),FakeUpdate); pt.Add(0x4DA540,FakeUpdate);
                    }
                    if(nm=="ReportBulletHit"){ pt.Add(bit_cast<uintptr_t>(&CAECollisionAudioEntity::PlayBulletHitCollisionSound),FakeBullet); pt.Add(0x4DB7C0,FakeBullet); }

                    if(nm=="Initialise") reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                    else if(nm=="InitialisePostLoading") reinterpret_cast<void(__cdecl*)()>(a)();
                    else if(nm=="Reset"||nm=="Service") reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                    else if(nm=="AddCollisionSoundToList"){
                        void* A=pickObj(5); void* B=pickObj(5); const uint32 sA=next()%256, sB=next()%256;
                        void* snd=next()%6 ? static_cast<void*>(sndPool[next()%NSND].data()) : nullptr; const uint32 st=next()%8==0 ? 3u : next()%3;
                        reinterpret_cast<void(__thiscall*)(void*,void*,void*,uint32,uint32,void*,uint32)>(a)(e,A,B,sA,sB,snd,st); }
                    else if(nm=="GetCollisionSoundStatus"){
                        void* A=pickObj(4); void* B=pickObj(4); int32 idx=0x7777;
                        ret=reinterpret_cast<uint8(__thiscall*)(void*,void*,void*,uint32,uint32,int32*)>(a)(e,A,B,next()%196,next()%196,&idx); out["index"]=idx; }
                    else if(nm=="ChooseCollisionSoundID"){
                        const uint32 s=next()%195; const int32 mn=G<int32>(0x8AD908+s*16), mx=G<int32>(0x8AD90C+s*16);
                        At<int16>(e,0x7C+2*s)=int16(mn-1+int32(next()%uint32(mx-mn+3)));
                        ret=reinterpret_cast<int16(__thiscall*)(void*,uint32)>(a)(e,s); }
                    else if(nm=="PlayOneShotCollisionSound"){
                        static const uint8 sp[]{62,63,188,192,192,193,194,9,45,0,175,189};
                        auto surf=[&]{ return next()%3==0 ? uint32(sp[next()%std::size(sp)]) : next()%195; };
                        const uint32 sA=surf(), sB=surf();
                        float imp; switch(next()%8){case 0:imp=uni(0.f,300.f);break;case 1:imp=0.f;break;case 2:imp=uni(-0.5f,0.f);break;default:imp=uni(0.f,1.5f);}
                        reinterpret_cast<void(__thiscall*)(void*,void*,void*,uint32,uint32,float,const CVector*)>(a)(e,pickObj(5),pickObj(5),sA,sB,imp,pos); }
                    else if(nm=="PlayLoopingCollisionSound"||nm=="UpdateLoopingCollisionSound"){
                        const bool upd=nm=="UpdateLoopingCollisionSound"; const uint32 lim=upd ? 195 : 179;
                        auto surf=[&]{ switch(next()%5){case 0:case 1:return 63u;case 2:return 62u;default:return next()%lim;} };
                        const uint32 sA=surf(), sB=surf(); void* A=objs[next()%NOBJ].data(); void* B=next()%6==0 ? A : static_cast<void*>(objs[next()%NOBJ].data());
                        const float imp=uni(0.f,2.f); const uint32 force=next()%2;
                        if(upd){ uint8* s=sndPool[next()%NSND].data();
                            if(next()%8==0)At<float>(s,0x14)=-100.f; if(next()%8==0)At<float>(s,0x1C)=0.6f;
                            reinterpret_cast<void(__thiscall*)(void*,void*,void*,void*,uint32,uint32,float,const CVector*,uint32)>(a)(e,s,A,B,sA,sB,imp,pos,force);
                            out["snd"]=ReversingProbe::Bytes(s,0x74); }
                        else reinterpret_cast<void(__thiscall*)(void*,void*,void*,uint32,uint32,float,const CVector*,uint32)>(a)(e,A,B,sA,sB,imp,pos,force); }
                    else if(nm=="PlayBulletHitCollisionSound"){
                        float ang; switch(next()%6){case 0:ang=90.f;break;case 1:ang=-90.f;break;default:ang=uni(-100.f,280.f);}
                        reinterpret_cast<void(__thiscall*)(void*,uint32,const CVector*,float)>(a)(e,next()%179,pos,ang); }
                    else if(nm=="ReportGlassCollisionEvent"){
                        const uint32 ev=0x78+next()%13; const uint32 t=next()%2 ? 0u : (next()%3 ? next()%2000 : next());
                        reinterpret_cast<void(__thiscall*)(void*,uint32,const CVector*,uint32)>(a)(e,ev,pos,t); showTemp=true; }
                    else if(nm=="ReportWaterSplash-at-position"){
                        reinterpret_cast<void(__thiscall*)(void*,float,float,float,float)>(a)(e,posBuf[0],posBuf[1],posBuf[2],uni(-30.f,10.f)); showTemp=true; }
                    else if(nm=="ReportWaterSplash-for-physical"){
                        uint8* o=objs[0].data();
                        switch(next()%8){case 0:At<float>(o,0x4C)=-0.1f;break;case 1:case 2:At<float>(o,0x4C)=uni(-0.1f,0.4f);break;default:At<float>(o,0x4C)=uni(-0.7f,-0.1f);}
                        float vol; switch(next()%6){case 0:case 1:vol=-100.f;break;case 2:vol=uni(-160.f,-100.f);break;default:vol=uni(-30.f,10.f);}
                        reinterpret_cast<void(__thiscall*)(void*,void*,float,uint32)>(a)(e,o,vol,next()%2); showTemp=true; }
                    else if(nm=="ReportObjectDestruction"){
                        uint8* o=objs[0].data();
                        G<uint16>(MI_GRASS)=uint16(next()%4==0 ? 344 : 19991);
                        G<void*>(MI_PTRS+4*COL_MODEL)=miBuf.data();
                        At<void*>(miBuf.data(),0x14)=colModelBuf.data();
                        At<void*>(colModelBuf.data(),0x2C)=next()%6==0 ? nullptr : static_cast<void*>(colDataBuf.data());
                        uint8* cd=colDataBuf.data(); for(int c=0;c<3;++c)At<int16>(cd,2*c)=int16(int(next()%5)-2);
                        At<void*>(cd,8)=sphBuf.data(); At<void*>(cd,0xC)=boxBuf.data(); At<void*>(cd,0x18)=triBuf.data();
                        static const uint8 ss[]{42,70,172,173,174,45,45,175,1,9};
                        auto surf=[&]{ return uint8(next()%2 ? ss[next()%std::size(ss)] : next()%179); };
                        sphBuf[0x10]=surf(); boxBuf[0x18]=surf(); triBuf[6]=surf();
                        switch(next()%5){case 0:At<int16>(o,0x22)=344;break;case 1:At<int16>(o,0x22)=int16(G<uint16>(MI_GRASS));break;default:At<int16>(o,0x22)=int16(COL_MODEL);}
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(e,next()%16==0 ? nullptr : o); showTemp=true; }
                    else if(nm=="ReportCollision"){
                        G<uint16>(MI_BASKET)=uint16(next()%6==0 ? 0xFFFF : 19992); G<uint16>(MI_PUNCH)=uint16(next()%6==0 ? 0xFFFF : 19993);
                        G<uint16>(MI_GRASS)=uint16(next()%6==0 ? 0xFFFF : 19994); G<uint16>(MI_CRATE)=uint16(next()%6==0 ? 0xFFFF : 19995);
                        static const int16 models[]{342,343,344,363,364,19992,19993,19994,19995,-1};
                        for(size_t k=0;k<NOBJ;++k){ uint8* o=objs[k].data();
                            if((o[0x36]&7)==1) At<int16>(o,0x22)=int16(1000+next()%100); // edificios sin superficie especial
                            else if(next()%2) At<int16>(o,0x22)=models[next()%std::size(models)]; }
                        uint8* A=objs[next()%NOBJ].data(); uint8* B=objs[next()%NOBJ].data();
                        // entradas de la lista con el par (A,B) o (B,A)
                        const int nPairs=int(next()%4); for(int k=0;k<nPairs;++k){ uint8* en=e+LIST+(next()%NENT)*0x14; const bool sw=next()%2;
                            At<void*>(en,0)=sw ? B : A; At<void*>(en,4)=sw ? A : B; en[0x10]=uint8(next()%3); }
                        for(int c=0;c<3;++c)normalBuf[c]=uni(-1.f,1.f);
                        const CVector* nrm=next()%4==0 ? nullptr : reinterpret_cast<const CVector*>(normalBuf.data());
                        float rel; switch(next()%8){case 0:rel=0.f;break;case 1:rel=uni(-1.f,0.f);break;default:rel=uni(0.f,4.f);}
                        reinterpret_cast<void(__thiscall*)(void*,void*,void*,uint32,uint32,const CVector*,const CVector*,float,float,uint32,uint32)>(a)
                            (e,A,B,next()%179,next()%179,pos,nrm,uni(0.f,3.f),rel,next()%3==0,next()%2); }
                    else if(nm=="ReportBulletHit"){
                        uint8* o=objs[0].data();
                        reinterpret_cast<void(__thiscall*)(void*,void*,uint32,const CVector*,float)>(a)(e,next()%5==0 ? nullptr : o,next()%256,pos,uni(-90.f,90.f)); }
                }
                if(showTemp)out["temp"]=ReversingProbe::Bytes(e+8,0x74);
                out["ent"]=Fnv(e,ENT_SIZE); out["ret"]=ret; out["trace"]=Trace();
                restore();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",ntr}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restore();
    const bool restored=G<uint32>(TIME_ADDR)==savedTime&&G<uint16>(MI_BASKET)==savedBasket&&G<uint16>(MI_PUNCH)==savedPunch&&G<uint16>(MI_GRASS)==savedGrass
        &&G<uint16>(MI_CRATE)==savedCrate&&G<void*>(MI_PTRS+4*COL_MODEL)==savedMi;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
