#pragma once
// Infraestructura del banco VehAudioFinal4 (CAEVehicleAudioEntity, 102 funciones): mundo falso estatico (entidades de audio,
// vehiculos, matrices, sonidos), registro de trazas sin reservas, falsos trazados (direccion del EXE + simbolo de la DLL)
// del gestor de sonidos, del hardware de audio, del azar, del pad, del entorno de audio y de la radio, y utilidades.
// Ver README.md. Los falsos solo registran en el hilo del banco (el hilo de audio y otros llaman a funciones parcheadas).
#include <fstream>
#include <array>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <bit>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Audio/Entities/AEVehicleAudioEntity.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Managers/AESoundManager.h"
#include "Audio/AEAudioUtility.h"
#include "Audio/AEAudioEnvironment.h"
#include "Audio/AudioEngine.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Pad.h"
#include "Timer.h"
#include "General.h"
#include "Camera.h"
#include "Weather.h"
#include "../ProbeRuntime.hpp"

namespace VehAudioFinal4Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }

// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId()==owner; }

inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline float  Fl(uint32 u) { return std::bit_cast<float>(u); }
template<class T=uint32> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
template<class T=uint32> inline const T& AtC(const void* b,size_t o) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(b)+o); }
template<class T=uint32> inline T& G(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
inline uint64 Fnv64(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline std::string Hex(const void* p,size_t n){ static const char d[]="0123456789abcdef"; std::string s; s.reserve(n*2); auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){ s+=d[b[i]>>4]; s+=d[b[i]&15]; } return s; }

// ---------------------------------------------------------------------------------------------------------------------
// Mundo falso (direcciones estaticas: son iguales en el lado EXE y en el lado DLL)
// ---------------------------------------------------------------------------------------------------------------------
constexpr size_t ENT_SIZE=0x24C, ENT_BUF=0x260, SND_SIZE=0x74, SND_BUF=0x80, VEH_SIZE=0x1000, NSND=40, NVEH=3, NENT=2, NMAT=6, NPED=2;
constexpr size_t TEMP_SOUND_OFF=0x8, TEMP_SOUND_END=0x7C;   // CAEAudioEntity::m_tempSound (escritorio de sonidos; no se compara)
struct World {
    alignas(16) uint8 ent[NENT][ENT_BUF];       // CAEVehicleAudioEntity falsos (0x24C bytes + guarda)
    alignas(16) uint8 veh[NVEH][VEH_SIZE];      // vehiculos falsos (cualquier tipo: automovil, moto, bici, barco, tren, avion, helicoptero)
    alignas(16) uint8 mat[NMAT][0x60];          // CMatrix falsos (0x48 bytes + guarda)
    alignas(16) uint8 snd[NSND][SND_BUF];       // reserva de CAESound falsos que devuelve RequestNewSound
    alignas(16) uint8 tr[NVEH][0x100];          // cTransmission falsos (0x68 bytes + guarda)
    alignas(16) uint8 ped[NPED][0x800];         // CPed falsos (conductor, jugador)
    alignas(16) uint8 settings[0x40];           // tVehicleAudioSettings falso (0x24 bytes + guarda)
    alignas(16) uint8 params[0x60];             // tVehicleParams falso (0x4C bytes + guarda)
    alignas(16) uint8 wheel[0x20];              // tWheelState[4]
    alignas(16) float gas[4];                   // GasPedalAudioRevs
    alignas(16) uint8 misc[0x400];              // zona libre para los grupos
};
inline World w;

// Registro de regiones para Id()
inline std::array<std::pair<uintptr_t,size_t>,64> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }
inline bool InStack(uintptr_t p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=uintptr_t(tib->StackLimit)&&p<uintptr_t(tib->StackBase); }
// Identificador estable de un puntero para las trazas: region + desplazamiento (los falsos son estaticos); pila -> constante.
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first));
    if(InStack(u))return 0xD0000000u;
    if(u<0x1000000)return uint32(u);
    return 0xE0000000u;
}
inline bool Known(const void* p){ const auto u=uintptr_t(p); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return true; return false; }

// Sigue la cadena de saltos E9 (entrada incremental -> cuerpo) SIN entrar en el EXE (un hook desactivado deja un jmp al EXE).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){ const auto t=a+5+*reinterpret_cast<int32*>(a+1); if(t>=0x400000&&t<0x1000000)break; a=t; } return a; }
inline bool InExe(uintptr_t a){ return a>=0x400000&&a<0x1000000; }

// Hash de todo el mundo falso, sin el escritorio m_tempSound de las entidades de audio (el EXE usa m_tempSound; la DLL puede usar un temporal)
inline uint64 WorldHash(){
    uint64 h=1469598103934665603ull;
    for(size_t k=0;k<NENT;++k){ h=Fnv64(w.ent[k],TEMP_SOUND_OFF,h); h=Fnv64(w.ent[k]+TEMP_SOUND_END,ENT_BUF-TEMP_SOUND_END,h); }
    h=Fnv64(w.veh,sizeof(w.veh),h); h=Fnv64(w.mat,sizeof(w.mat),h); h=Fnv64(w.snd,sizeof(w.snd),h); h=Fnv64(w.tr,sizeof(w.tr),h);
    h=Fnv64(w.ped,sizeof(w.ped),h); h=Fnv64(w.settings,sizeof(w.settings),h); h=Fnv64(w.wheel,sizeof(w.wheel),h);
    h=Fnv64(w.gas,sizeof(w.gas),h); h=Fnv64(w.misc,sizeof(w.misc),h);
    return h;
}

// ---------------------------------------------------------------------------------------------------------------------
// Registro de trazas sin reservas
// ---------------------------------------------------------------------------------------------------------------------
struct Rec { uint32 t,a,b,c,d; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d}; else recOverflow=true; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a,recs[i].b,recs[i].c,recs[i].d}); if(recOverflow)r.push_back("overflow"); return r; }

enum : uint32 {
    T_REQ=1,T_EVT,T_SLOTPLAY,T_CANCELSLOT,T_CANCELOWN,T_BANK,T_LOADBANK,T_RAND,T_BRAKE,T_ACCEL,T_REL,T_ATT,T_REGREF,T_CLEANREF,T_STOPRADIO,T_STARTRADIO,
    T_GROUP=100 };   // los grupos usan tipos >= T_GROUP

// ---------------------------------------------------------------------------------------------------------------------
// Estado de los falsos (lo fija cada caso)
// ---------------------------------------------------------------------------------------------------------------------
struct Fakes {
    int    bankMode;            // 0 = todos cargados, 1 = ninguno, 2 = segun bankBits
    uint32 bankBits;
    std::array<int16,4> evSeq;  uint32 evIdx;        // AreSoundsOfThisEventPlayingForThisEntity
    std::array<int16,4> slotSeq; uint32 slotIdx;     // AreSoundsPlayingInBankSlot
    uint32 reqFail;             // bit k: la k-esima peticion de sonido devuelve nulo (reserva llena)
    uint32 sndNext;             // siguiente ranura falsa
    uint32 sndLimit;            // las ranuras >= sndLimit son sonidos preexistentes de la entidad (no las reparte RequestNewSound)
    int16  padAccel, padBrake;
    std::array<CVector,2> rel;  uint32 relIdx;
    float  attenScale;
};
inline Fakes fk{};
inline uint32 rngState{1};
inline std::array<std::array<uint8,SND_SIZE>,NSND> reqs{}; inline std::array<int32,NSND> reqSlot{}; inline size_t nreq{};

// Sonido tal y como lo deja el gestor: los campos que el EXE no escribe en CAESound::Initialise (relleno, tiempos, distancias a la
// camara, indicador de hardware, bits altos de las banderas) se ponen a cero para que EXE y DLL se comparen solo en lo escrito.
inline void CanonSound(uint8* dst,const void* src){
    std::memcpy(dst,src,SND_SIZE);
    auto* d=reinterpret_cast<CAESound*>(dst);
    d->__pad=0; d->m_Flags=uint16(d->m_Flags&0x1FFF); d->m_CurrTimeUpdateMs=0; d->m_PrevTimeUpdateMs=0; d->m_CurrCamDist=0.f; d->m_PrevCamDist=0.f; d->m_IsAudioHardwareAware=0;
    // relleno de CAESound que Initialise no escribe (+0x6A, +0x72)
    dst[0x6A]=0; dst[0x6B]=0; dst[0x72]=0; dst[0x73]=0;
}
// Sonido "recien creado" (campos por defecto del gestor): ranura k de la reserva
inline void FreshSound(uint8* dst){
    std::memset(dst,0,SND_SIZE);
    auto* d=reinterpret_cast<CAESound*>(dst);
    d->m_Event=-1; d->m_ClientVariable=-1.f; d->m_IsInUse=1; d->m_ListenerVolume=-100.f; d->m_ListenerSpeed=1.f; d->m_Length=-1; d->m_BankSlot=SND_BANK_SLOT_NONE;
}
inline json Reqs(){
    json r=json::array(); const size_t n=std::min(nreq,reqs.size());
    for(size_t i=0;i<n;++i)r.push_back({{"slot",reqSlot[i]},{"fnv",Fnv32(reqs[i].data(),SND_SIZE)},{"bytes",Hex(reqs[i].data(),SND_SIZE)}});
    if(nreq>n)r.push_back("overflow");
    return r;
}
inline uint32 ReqsHash(){ uint64 h=1469598103934665603ull; const size_t n=std::min(nreq,reqs.size()); for(size_t i=0;i<n;++i){ h=Fnv64(reqs[i].data(),SND_SIZE,h); h=Fnv64(&reqSlot[i],4,h); } return uint32(h^(h>>32)^uint32(nreq)); }

// ---------------------------------------------------------------------------------------------------------------------
// Falsos (todos respetan la convencion y el 'ret N' del original)
// ---------------------------------------------------------------------------------------------------------------------
// CAESoundManager::RequestNewSound (0x4EFB10, ret 4): copia el sonido pedido a una ranura falsa (o devuelve nulo)
inline CAESound* __fastcall S_Request(void*,void*,CAESound* s){
    if(!Mine())return nullptr;
    const size_t k=nreq++;
    int32 slot=-1; CAESound* ret=nullptr;
    if(k<reqs.size()){ CanonSound(reqs[k].data(),s); }
    if(!((fk.reqFail>>(k&31))&1u) && fk.sndNext<fk.sndLimit&&fk.sndNext<NSND){ slot=int32(fk.sndNext++); CanonSound(w.snd[slot],s); std::memset(w.snd[slot]+SND_SIZE,0,SND_BUF-SND_SIZE); ret=reinterpret_cast<CAESound*>(w.snd[slot]); }
    if(k<reqs.size())reqSlot[k]=slot;
    Push(T_REQ,k<reqs.size()?Fnv32(reqs[k].data(),SND_SIZE):0u,uint32(slot));
    return ret;
}
// 0x4EF570 (ret 8)
inline int16 __fastcall S_EventPlaying(void*,void*,int16 ev,void* e){ if(!Mine())return 0; const int16 r=fk.evSeq[fk.evIdx++%4]; Push(T_EVT,uint16(ev),Id(e),uint16(r)); return r; }
// 0x4EF520 (ret 4)
inline int16 __fastcall S_SlotPlaying(void*,void*,int16 slot){ if(!Mine())return 0; const int16 r=fk.slotSeq[fk.slotIdx++%4]; Push(T_SLOTPLAY,uint16(slot),uint16(r)); return r; }
// 0x4EFC60 (ret 8)
inline void __fastcall S_CancelSlot(void*,void*,int16 slot,bool full){ if(!Mine())return; Push(T_CANCELSLOT,uint16(slot),full?1u:0u); }
// 0x4EFCD0 (ret 8)
inline void __fastcall S_CancelOwned(void*,void*,void* e,bool full){ if(!Mine())return; Push(T_CANCELOWN,Id(e),full?1u:0u); }
// CAEAudioHardware::IsSoundBankLoaded (0x4D88C0, ret 8) / LoadSoundBank (0x4D88A0, ret 8)
inline bool __fastcall S_BankLoaded(void*,void*,uint32 bank,uint32 slot){
    if(!Mine())return true;
    const uint16 b=uint16(bank), s=uint16(slot);
    const bool r= fk.bankMode==0 ? true : fk.bankMode==1 ? false : (((fk.bankBits>>((uint32(b)*13u+uint32(s)*7u)&31u))&1u)!=0);
    Push(T_BANK,b,s,r?1u:0u); return r;
}
inline void __fastcall S_LoadBank(void*,void*,uint32 bank,uint32 slot){ if(!Mine())return; Push(T_LOADBANK,uint16(bank),uint16(slot)); }
// Azar: rand del CRT (0x821B1E) y CGeneral::GetRandomNumber de la DLL
inline int __cdecl S_Rand(){ if(!Mine())return 0x1234; rngState^=rngState<<13; rngState^=rngState>>17; rngState^=rngState<<5; const uint32 v=rngState&0x7FFF; Push(T_RAND,v); return int(v); }
// CPad::GetBrake (0x540080) / GetAccelerate (0x5403F0)
inline int16 __fastcall S_Brake(void*,void*){ if(!Mine())return 0; Push(T_BRAKE,uint16(fk.padBrake)); return fk.padBrake; }
inline int16 __fastcall S_Accel(void*,void*){ if(!Mine())return 0; Push(T_ACCEL,uint16(fk.padAccel)); return fk.padAccel; }
// CAEAudioEnvironment::GetPositionRelativeToCamera_Placeable (0x4D8340) / GetDistanceAttenuation (0x4D7F20)
inline void __cdecl S_Rel(CVector* out,void* placeable){ if(!Mine())return; const CVector v=fk.rel[fk.relIdx%2]; Push(T_REL,Id(placeable),fk.relIdx); ++fk.relIdx; *out=v; }
inline float __cdecl S_Atten(float d){ if(!Mine())return 0.f; Push(T_ATT,F(d)); return -float(F(d)%6000u)*0.01f*fk.attenScale; }
// CEntity::RegisterReference (0x571B70) / CleanUpOldReference (0x571A00)
inline void __fastcall S_RegRef(void* e,void*,void* ref){ if(!Mine())return; Push(T_REGREF,Id(e),Id(ref)); }
inline void __fastcall S_CleanRef(void* e,void*,void* ref){ if(!Mine())return; Push(T_CLEANREF,Id(e),Id(ref)); }
// CAudioEngine::StopRadio (0x506F70, ret 8) / StartRadio(const tVehicleAudioSettings&) (0x507DF0, ret 4)
inline void __fastcall S_StopRadio(void*,void*,void* settings,bool pause){ if(!Mine())return; Push(T_STOPRADIO,Id(settings),pause?1u:0u,settings?Fnv32(settings,0x24):0u); }
inline void __fastcall S_StartRadio(void*,void*,const void* settings){ if(!Mine())return; Push(T_STARTRADIO,Id(settings),settings?Fnv32(settings,0x24):0u); }

// ---------------------------------------------------------------------------------------------------------------------
// Hooks, direcciones de la DLL y conjuntos de parches
// ---------------------------------------------------------------------------------------------------------------------
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurAddress(ReversibleHooks::ReversibleHook::Base* h) {
    if(!h)return 0;
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Virtual)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return 0;
}
// Grupos de falsos (mascara). G_X1.. son libres para los grupos de funciones.
enum : uint32 { G_SND=1,G_HW=2,G_RNG=4,G_PAD=8,G_ENV=0x10,G_REF=0x20,G_RADIO=0x40,
                G_STD=G_SND|G_HW|G_RNG|G_ENV|G_REF,   // conjunto habitual (sin pad ni radio: se piden aparte)
                G_X1=0x100,G_X2=0x200,G_X3=0x400,G_X4=0x800,G_X5=0x1000,G_X6=0x2000,G_X7=0x4000,G_X8=0x8000,G_X9=0x10000,G_X10=0x20000,G_X11=0x40000,G_X12=0x80000 };
struct Stub { uint32 group; uintptr_t exe,dll,body; void* fn; };
inline std::vector<Stub> stubs;
inline void AddStub(uint32 group,uintptr_t exe,uintptr_t dllSym,void* fn){ stubs.push_back({group,exe,dllSym,dllSym ? Body(dllSym) : 0,fn}); }
// Conjunto de parches SIN reservas de memoria: los EntryPatch viven en almacenamiento estatico.
struct PatchSet {
    static constexpr size_t MAXP=256;
    alignas(EntryPatch) static inline unsigned char storage[MAXP][sizeof(EntryPatch)];
    static inline uintptr_t used[MAXP];
    size_t n{};
    explicit PatchSet(uint32 mask){ for(auto& s:stubs)if(s.group&mask){ Put(s.exe,s.fn); Put(s.dll,s.fn); Put(s.body,s.fn); } }
    PatchSet(const PatchSet&)=delete; PatchSet& operator=(const PatchSet&)=delete;
    void Put(uintptr_t at,void* fn){ if(!at||n>=MAXP)return; for(size_t k=0;k<n;++k)if(used[k]==at)return; used[n]=at; new(storage[n]) EntryPatch(at,fn); ++n; }
    ~PatchSet(){ while(n>0){ --n; reinterpret_cast<EntryPatch*>(storage[n])->~EntryPatch(); } }
};
inline PatchSet MakePatches(uint32 mask){ return PatchSet(mask); }
template<class P> inline void* FnPtr(P f){ return reinterpret_cast<void*>(f); }
inline bool AddHookStub(uint32 group,uintptr_t exe,const char* cls,const char* name,void* fn){
    auto* h=FindHook(cls,name); const auto our=OurAddress(h); if(!h||!our)return false;
    AddStub(group,exe,our,fn); return true;
}

// Registro de los falsos habituales (antes de crear ningun EntryPatch: Body() se calcula aqui)
inline void RegisterStdStubs(){
    stubs.clear();
    AddStub(G_SND,0x4EFB10,bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),FnPtr(&S_Request));
    AddStub(G_SND,0x4EF570,bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity),FnPtr(&S_EventPlaying));
    AddStub(G_SND,0x4EF520,bit_cast<uintptr_t>(&CAESoundManager::AreSoundsPlayingInBankSlot),FnPtr(&S_SlotPlaying));
    AddStub(G_SND,0x4EFC60,bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsInBankSlot),FnPtr(&S_CancelSlot));
    AddStub(G_SND,0x4EFCD0,bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsOwnedByAudioEntity),FnPtr(&S_CancelOwned));
    AddStub(G_HW,0x4D88C0,bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),FnPtr(&S_BankLoaded));
    AddStub(G_HW,0x4D88A0,bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),FnPtr(&S_LoadBank));
    AddStub(G_RNG,0x821B1E,bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FnPtr(&S_Rand));
    AddStub(G_PAD,0x540080,bit_cast<uintptr_t>(&CPad::GetBrake),FnPtr(&S_Brake));
    AddStub(G_PAD,0x5403F0,bit_cast<uintptr_t>(&CPad::GetAccelerate),FnPtr(&S_Accel));
    AddStub(G_ENV,0x4D8340,bit_cast<uintptr_t>(&CAEAudioEnvironment::GetPositionRelativeToCamera_Placeable),FnPtr(&S_Rel));
    AddStub(G_ENV,0x4D7F20,bit_cast<uintptr_t>(&CAEAudioEnvironment::GetDistanceAttenuation),FnPtr(&S_Atten));
    AddStub(G_REF,0x571B70,bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FnPtr(&S_RegRef));
    AddStub(G_REF,0x571A00,bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FnPtr(&S_CleanRef));
    AddStub(G_RADIO,0x506F70,bit_cast<uintptr_t>(&CAudioEngine::StopRadio),FnPtr(&S_StopRadio));
    AddStub(G_RADIO,0x507DF0,bit_cast<uintptr_t>(static_cast<void(CAudioEngine::*)(const tVehicleAudioSettings&)>(&CAudioEngine::StartRadio)),FnPtr(&S_StartRadio));
}

// Instantanea de globales (se restaura al terminar cada ejecucion)
struct Saved { uintptr_t addr; std::vector<uint8> bytes; };
inline std::vector<Saved> saved;
inline void Save(uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); }
inline void RestoreAll(){ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size()); }
inline bool AllRestored(){ bool ok=true; for(auto& s:saved)ok&=std::memcmp(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size())==0; return ok; }
inline uint64 SavedHash(){ uint64 g=1469598103934665603ull; for(auto& s:saved){ g^=Fnv64(reinterpret_cast<void*>(s.addr),s.bytes.size()); g*=1099511628211ull; } return g; }

template<class R,class... A> inline R CallThis(uintptr_t a,void* self,A... args){ return reinterpret_cast<R(__thiscall*)(void*,A...)>(a)(self,args...); }
template<class R,class... A> inline R CallCdecl(uintptr_t a,A... args){ return reinterpret_cast<R(__cdecl*)(A...)>(a)(args...); }
template<class R,class... A> inline R CallStd(uintptr_t a,A... args){ return reinterpret_cast<R(__stdcall*)(A...)>(a)(args...); }

// Llamada thiscall con ECX = this y EDX sembrado, que devuelve el float de ST0 y captura ECX/EDX justo despues del `ret N` del destino. Las envolturas de
// GetFreqForIdle (0x4F5C60), GetVehicleDriveWheelSkidValue (0x4F5F30), GetVehicleNonDriveWheelSkidValue (0x4F6000) y GetVolForPlayerEngineSound (0x4F5D00)
// existen para dejar ECX/EDX como el EXE (sus llamadores los reutilizan), asi que el banco los compara en bits (EAX no: en estas funciones solo es residuo de
// fnstsw). Los argumentos de pila se pasan como palabras de 32 bits (puntero, entero o float) y se apilan de derecha a izquierda; el destino los retira.
inline float CallThisRegsN(uintptr_t a,void* self,const uint32* words,int count,uint32 seedEdx,uint32* ecxOut,uint32* edxOut){
    float res=0.f; uint32 ec=0,ed=0;
    __asm {
        mov eax, count
        mov edx, words
    pushloop:
        test eax, eax
        jz pushed
        dec eax
        push dword ptr [edx+eax*4]
        jmp pushloop
    pushed:
        mov eax, a
        mov ecx, self
        mov edx, seedEdx
        call eax
        mov ec, ecx
        mov ed, edx
        fstp res
    }
    *ecxOut=ec; *edxOut=ed;
    return res;
}
template<class T> inline uint32 RegWord(T v){ static_assert(sizeof(T)==4); uint32 w; std::memcpy(&w,&v,4); return w; }
template<class... A> inline float CallThisRegs(uintptr_t a,void* self,uint32 seedEdx,uint32* ecxOut,uint32* edxOut,A... args){
    const uint32 words[sizeof...(A)+1]={RegWord(args)...};
    return CallThisRegsN(a,self,words,int(sizeof...(A)),seedEdx,ecxOut,edxOut);
}
// Semilla de EDX por caso y por funcion, sin consumir el generador del caso (los fixtures existentes no cambian).
inline uint32 RegSeed(int caseIndex,uint32 fnIndex){ return 0x5EED0000u ^ (uint32(caseIndex)*0x9E3779B1u) ^ (fnIndex*0x85EBCA6Bu) ^ 0x00001234u; }
}
