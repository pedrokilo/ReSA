#pragma once
// Infraestructura del banco AudioEngineFinal4 (CAudioEngine, 82 funciones): CAudioEngine falso en memoria propia, mundo falso,
// registro de trazas sin reservas, guiones de retornos de los dobles, dobles trazados (direccion del EXE + simbolo de la DLL)
// y utilidades. Ver README.md.
#include <fstream>
#include <array>
#include <vector>
#include <bit>
#include <cstring>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Audio/AudioEngine.h"
#include "Audio/Managers/AECutsceneTrackManager.h"
#include "Audio/Managers/AEAmbienceTrackManager.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace AudioEngineFinal4Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }

// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId()==owner; }

inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline float Fb(int32 v) { return std::bit_cast<float>(static_cast<uint32>(v)); }
template<class T=uint32> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
inline uint64 Fnv64(const void* p,size_t n){ uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 S(const char* s){ return s ? (Fnv32(s,strlen(s))|1u) : 0u; }

// ---------------------------------------------------------------------------------------------------------------------
// Memoria falsa. El CAudioEngine falso y el mundo son estaticos (direcciones estables); Id() convierte punteros en
// (region, desplazamiento) para las trazas.
// ---------------------------------------------------------------------------------------------------------------------
constexpr size_t ENG_SIZE=0x1FD8;
alignas(16) inline std::array<uint8,0x2000> eng{};
struct Vec3f { float x,y,z; };
struct World {
    alignas(16) uint8 ent[6][0x100];      // CEntity / CPed / CObject / CPhysical / CVehicle opacos (nunca se leen)
    alignas(16) uint8 vec[6][0x10];       // CVector (posiciones y normales)
    alignas(16) uint8 sett[2][0x80];      // tVehicleAudioSettings falsos
    alignas(16) uint8 gw[0xB0];           // CAEGlobalWeaponAudioEntity falso (0xA8)
    alignas(16) uint8 mtx[0x100];         // matriz falsa que devuelve AllocateStaticMatrix
    alignas(16) uint8 file[0x10];         // FILE* falso
    alignas(16) char  names[4][64];       // nombres de emisora
    alignas(16) uint8 outbuf[0x80];       // char* de GetRadioStationNameKey
    alignas(16) int32 listen[16];         // GetRadioStationListenTimes
    alignas(16) uint8 settRef[0x80];      // tVehicleAudioSettings& de StartRadio
};
inline World w;
inline std::array<std::pair<uintptr_t,size_t>,8> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }
inline bool InStack(uintptr_t p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=uintptr_t(tib->StackLimit)&&p<uintptr_t(tib->StackBase); }
inline bool IsCode(uintptr_t a){ MEMORY_BASIC_INFORMATION mi{}; return a>=0x10000&&VirtualQuery(reinterpret_cast<void*>(a),&mi,sizeof(mi))&&mi.State==MEM_COMMIT&&(mi.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)); }
// Sigue la cadena de saltos E9 (entrada incremental -> cuerpo) SIN entrar en el EXE (un hook desactivado deja un jmp al EXE).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){ const auto t=a+5+*reinterpret_cast<int32*>(a+1); if(t>=0x400000&&t<0x1000000)break; a=t; } return a; }
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first));
    if(InStack(u))return 0xD0000000u;
    static const uintptr_t exeBase=reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
    static const uintptr_t exeEnd=exeBase+reinterpret_cast<const IMAGE_NT_HEADERS*>(exeBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(exeBase)->e_lfanew)->OptionalHeader.SizeOfImage;
    if(u>=exeBase&&u<exeEnd)return uint32(u);   // singletons y datos del EXE
    if(u<0x10000)return uint32(u);              // valores pequenos (no son punteros)
    return 0xE0000000u;                         // memoria dinamica (direcciones distintas en cada lado)
}

// ---------------------------------------------------------------------------------------------------------------------
// Registro de trazas sin reservas
// ---------------------------------------------------------------------------------------------------------------------
struct Rec { uint32 t,a,b,c,d; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d}; else recOverflow=true; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a,recs[i].b,recs[i].c,recs[i].d}); if(recOverflow)r.push_back("overflow"); return r; }

enum : uint32 {
    T_NONE=0,
    T_HW_SMM,T_HW_SEM,T_HW_SMF,T_HW_SEF,T_HW_SNS,T_HW_SSF,T_HW_DIS,T_HW_ENA,T_HW_PAUSE,T_HW_RESUME,T_HW_TIME,T_HW_SVC,T_HW_BEAT,T_HW_BEATF,T_HW_CHAN,T_HW_GEF,T_HW_BANK,T_HW_TERM,T_HW_INIT,
    T_FE_ADD,T_FE_LOAD,T_FE_RETUNE,T_FE_RESET,T_FE_INIT,
    T_SC_PRE,T_SC_STAT,T_SC_PLAY,T_SC_FIN,T_SC_EVT,T_SC_POS,T_SC_CLR,T_SC_SETP,T_SC_ATT,T_SC_RVEC,T_SC_RPHY,T_SC_SVC,T_SC_RST,T_SC_INIT,
    T_CO_REP,T_CO_REP2,T_CO_REP3,T_CO_BUL,T_CO_DES,T_CO_GLS,T_CO_SPP,T_CO_SPV,T_CO_SVC,T_CO_RST,T_CO_PL,T_CO_INIT,
    T_GW_ADD,T_GW_AMB,T_GW_RST,T_GW_INIT,T_DR_ADD,T_PL_SAY,T_PL_SAY2,T_PL_INIT,
    T_RD_STOP,T_RD_AUTO,T_RD_BASSE,T_RD_BASS,T_RD_IDS,T_RD_ON,T_RD_JUST,T_RD_NAME,T_RD_KEY,T_RD_LIST,T_RD_DISP,T_RD_CUR,T_RD_VEH,T_RD_ST1,T_RD_ST2,T_RD_RET,T_RD_SVC,T_RD_RST,T_RD_INIT,T_RD_STAT,T_VE_SET,
    T_CT_PLAY,T_CT_STOP,T_CT_ACT,T_CT_STAT,T_CT_PAUSE,T_CT_SVC,T_CT_PBEAT,T_CT_PCUT,
    T_AM_ACT,T_AM_STOP,T_AM_RAD,T_AM_SVC,T_AM_SPEC,T_AM_RST,
    T_SM_SVC,T_SM_RST,T_SM_PAUSE,T_SM_TERM,T_SM_INIT,
    T_FIND,T_PS_RST,T_PS_SVC,T_PO_SVC,T_PO_RST,T_WE_RST,T_UR_SHUT,T_LS_PAUSE,T_LS_CONT,T_F_OPEN,T_F_READ,T_F_CLOSE,
    T_I_UTIL,T_I_PED,T_I_SPEECH,T_I_VEH,T_I_EXPL,T_I_WEATHER,T_I_DOOR,T_I_FIRE,T_I_POLICE,
    T_ASM,T_RAND,T_DEL,T_ENG_SVC,
    T_LAST };
static_assert(T_LAST<=192);

// ---------------------------------------------------------------------------------------------------------------------
// Guiones de retornos de los dobles: cada etiqueta tiene n valores iniciales y luego un valor final fijo. Sin reservas.
// ---------------------------------------------------------------------------------------------------------------------
struct Seq { int32 v[8]; int32 n; int32 pos; int32 tail; };
inline std::array<Seq,192> seqs{};
inline int32 Ask(uint32 t){ auto& s=seqs[t]; const int32 r= s.pos<s.n ? s.v[s.pos] : s.tail; ++s.pos; return r; }
inline void* Pick(int32 k,void* base,size_t stride,int n){ if(k<=0)return nullptr; return static_cast<uint8*>(base)+stride*size_t((k-1)%n); }

// ---------------------------------------------------------------------------------------------------------------------
// Dobles. Todos respetan la convencion y el 'ret N' del original: miembros = __fastcall(this, edx, args...), estaticos = __cdecl.
// Ignoran los hilos ajenos al del banco.
// ---------------------------------------------------------------------------------------------------------------------
// --- CAEAudioHardware (0xB5F8B8)
inline void __fastcall H_SetMusicMaster(void* s,void*,float f){ if(!Mine())return; Push(T_HW_SMM,Id(s),F(f)); }
inline void __fastcall H_SetEffMaster(void* s,void*,float f){ if(!Mine())return; Push(T_HW_SEM,Id(s),F(f)); }
inline void __fastcall H_SetMusicFader(void* s,void*,float f){ if(!Mine())return; Push(T_HW_SMF,Id(s),F(f)); }
inline void __fastcall H_SetEffFader(void* s,void*,float f){ if(!Mine())return; Push(T_HW_SEF,Id(s),F(f)); }
inline void __fastcall H_SetNonStream(void* s,void*,float f){ if(!Mine())return; Push(T_HW_SNS,Id(s),F(f)); }
inline void __fastcall H_SetStream(void* s,void*,float f){ if(!Mine())return; Push(T_HW_SSF,Id(s),F(f)); }
inline void __fastcall H_DisableFx(void* s,void*){ if(!Mine())return; Push(T_HW_DIS,Id(s)); }
inline void __fastcall H_EnableFx(void* s,void*){ if(!Mine())return; Push(T_HW_ENA,Id(s)); }
inline void __fastcall H_PauseAll(void* s,void*){ if(!Mine())return; Push(T_HW_PAUSE,Id(s)); }
inline void __fastcall H_ResumeAll(void* s,void*){ if(!Mine())return; Push(T_HW_RESUME,Id(s)); }
inline int32 __fastcall H_TrackTime(void* s,void*){ if(!Mine())return 0; Push(T_HW_TIME,Id(s)); return Ask(T_HW_TIME); }
inline void __fastcall H_Service(void* s,void*){ if(!Mine())return; Push(T_HW_SVC,Id(s)); }
// Rellena la estructura de ritmo con datos del guion: 0xAC bytes deterministas y el indicador de presencia en +0xA0.
inline void __fastcall H_BeatInfo(void* s,void*,void* info){
    if(!Mine())return; Push(T_HW_BEAT,Id(s),Id(info));
    if(!info)return;
    uint32 st=uint32(Ask(T_HW_BEAT))|1u; auto* b=static_cast<uint8*>(info);
    for(size_t k=0;k+4<=0xA0;k+=4){ st^=st<<13; st^=st>>17; st^=st<<5; std::memcpy(b+k,&st,4); }
    const int32 flag=Ask(T_HW_BEATF); std::memcpy(b+0xA0,&flag,4);
    st^=st<<13; st^=st>>17; st^=st<<5; std::memcpy(b+0xA4,&st,4); st^=st<<13; st^=st>>17; st^=st<<5; std::memcpy(b+0xA8,&st,4); }
inline void __fastcall H_ChanPlay(void* s,void*,uint32 ch,void* times){ if(!Mine())return; Push(T_HW_CHAN,Id(s),ch&0xFFFFu,Id(times)); }
inline float __fastcall H_GetEffFader(void* s,void*){ if(!Mine())return 0.f; Push(T_HW_GEF,Id(s)); return Fb(Ask(T_HW_GEF)); }
inline bool __fastcall H_BankLoaded(void* s,void*,uint32 bank,uint32 slot){ if(!Mine())return true; Push(T_HW_BANK,Id(s),bank,slot); return Ask(T_HW_BANK)!=0; }
inline void __fastcall H_Terminate(void* s,void*){ if(!Mine())return; Push(T_HW_TERM,Id(s)); }
inline bool __fastcall H_Init(void* s,void*){ if(!Mine())return false; Push(T_HW_INIT,Id(s)); return Ask(T_HW_INIT)!=0; }

// --- CAEFrontendAudioEntity (engine + 0xB4)
inline void __fastcall F_Add(void* s,void*,uint32 ev,float a,float b){ if(!Mine())return; Push(T_FE_ADD,Id(s),ev,F(a),F(b)); }
inline bool __fastcall F_LoadActive(void* s,void*){ if(!Mine())return false; Push(T_FE_LOAD,Id(s)); return Ask(T_FE_LOAD)!=0; }
inline bool __fastcall F_RetuneActive(void* s,void*){ if(!Mine())return false; Push(T_FE_RETUNE,Id(s)); return Ask(T_FE_RETUNE)!=0; }
inline void __fastcall F_Reset(void* s,void*){ if(!Mine())return; Push(T_FE_RESET,Id(s)); }
inline void __fastcall F_Init(void* s,void*){ if(!Mine())return; Push(T_FE_INIT,Id(s)); }

// --- CAEScriptAudioEntity (engine + 0x2A0)
inline void __fastcall SC_Preload(void* s,void*,uint32 slot,int32 ev){ if(!Mine())return; Push(T_SC_PRE,Id(s),slot,uint32(ev)); }
inline int8 __fastcall SC_Status(void* s,void*,uint32 slot){ if(!Mine())return 0; Push(T_SC_STAT,Id(s),slot); return int8(Ask(T_SC_STAT)); }
inline void __fastcall SC_Play(void* s,void*,uint32 slot){ if(!Mine())return; Push(T_SC_PLAY,Id(s),slot); }
inline bool __fastcall SC_Finished(void* s,void*,uint32 slot){ if(!Mine())return false; Push(T_SC_FIN,Id(s),slot); return Ask(T_SC_FIN)!=0; }
inline int32 __fastcall SC_Event(void* s,void*,uint32 slot){ if(!Mine())return 0; Push(T_SC_EVT,Id(s),slot); return Ask(T_SC_EVT); }
inline void* __fastcall SC_Pos(void* s,void*,uint32 slot){ if(!Mine())return nullptr; Push(T_SC_POS,Id(s),slot); return Pick(Ask(T_SC_POS),w.vec,0x10,6); }
inline void __fastcall SC_Clear(void* s,void*,uint32 slot){ if(!Mine())return; Push(T_SC_CLR,Id(s),slot); }
inline void __fastcall SC_SetPos(void* s,void*,uint32 slot,void* v){ if(!Mine())return; Push(T_SC_SETP,Id(s),slot,Id(v),v ? Fnv32(v,12) : 0u); }
inline void* __fastcall SC_Attach(void* s,void*,uint32 slot,void* ph){ if(!Mine())return nullptr; Push(T_SC_ATT,Id(s),slot,Id(ph)); return Pick(Ask(T_SC_ATT),w.vec,0x10,6); }
inline void __fastcall SC_RepVec(void* s,void*,uint32 ev,void* v){ if(!Mine())return; Push(T_SC_RVEC,Id(s),ev,Id(v),v ? Fnv32(v,12) : 0u); }
inline void __fastcall SC_RepPhys(void* s,void*,uint32 ev,void* ph,float a,float b){ if(!Mine())return; Push(T_SC_RPHY,Id(s),ev,Id(ph),F(a)); Push(T_SC_RPHY,F(b)); }
inline void __fastcall SC_Service(void* s,void*){ if(!Mine())return; Push(T_SC_SVC,Id(s)); }
inline void __fastcall SC_Reset(void* s,void*){ if(!Mine())return; Push(T_SC_RST,Id(s)); }
inline void __fastcall SC_Init(void* s,void*){ if(!Mine())return; Push(T_SC_INIT,Id(s)); }

// --- CAECollisionAudioEntity (engine + 0x4BC)
inline void __fastcall CO_Report(void* s,void*,void* e1,void* e2,uint32 su1,uint32 su2,const void* pos,const void* nrm,float f1,float f2,uint32 b1,uint32 b2){
    if(!Mine())return; Push(T_CO_REP,Id(s),Id(e1),Id(e2),su1); Push(T_CO_REP2,su2,Id(pos),pos ? Fnv32(pos,12) : 0u,Id(nrm));
    Push(T_CO_REP3,nrm ? Fnv32(nrm,12) : 0u,F(f1),F(f2),(b1&0xFFu)|((b2&0xFFu)<<8)); }
inline void __fastcall CO_Bullet(void* s,void*,void* e,uint32 su,const void* pos,float a){ if(!Mine())return; Push(T_CO_BUL,Id(s),Id(e),su,Id(pos)); Push(T_CO_BUL,pos ? Fnv32(pos,12) : 0u,F(a)); }
inline void __fastcall CO_Destroy(void* s,void*,void* e){ if(!Mine())return; Push(T_CO_DES,Id(s),Id(e)); }
inline void __fastcall CO_Glass(void* s,void*,uint32 ev,const void* pos,uint32 time){ if(!Mine())return; Push(T_CO_GLS,Id(s),ev,Id(pos),pos ? Fnv32(pos,12) : 0u); Push(T_CO_GLS,time); }
inline void __fastcall CO_SplashPhys(void* s,void*,void* ph,float v,uint32 b){ if(!Mine())return; Push(T_CO_SPP,Id(s),Id(ph),F(v),b&0xFFu); }
inline void __fastcall CO_SplashPos(void* s,void*,Vec3f p,float v){ if(!Mine())return; Push(T_CO_SPV,Id(s),F(p.x),F(p.y),F(p.z)); Push(T_CO_SPV,F(v)); }
inline void __fastcall CO_Service(void* s,void*){ if(!Mine())return; Push(T_CO_SVC,Id(s)); }
inline void __fastcall CO_Reset(void* s,void*){ if(!Mine())return; Push(T_CO_RST,Id(s)); }
inline void __fastcall CO_Init(void* s,void*){ if(!Mine())return; Push(T_CO_INIT,Id(s)); }
inline void __cdecl CO_PostLoading(){ if(!Mine())return; Push(T_CO_PL); }

// --- CAEGlobalWeaponAudioEntity / puerta / habla sin ped
inline void __fastcall GW_Add(void* s,void*,uint32 ev,uint32 wt,void* ph){ if(!Mine())return; Push(T_GW_ADD,Id(s),ev,wt,Id(ph)); }
inline void __fastcall GW_Ambient(void* s,void*){ if(!Mine())return; Push(T_GW_AMB,Id(s)); }
inline void __fastcall GW_Reset(void* s,void*){ if(!Mine())return; Push(T_GW_RST,Id(s)); }
inline void __fastcall GW_Init(void* s,void*){ if(!Mine())return; Push(T_GW_INIT,Id(s)); }
inline void __fastcall DR_Add(void* s,void*,uint32 ev,void* ph){ if(!Mine())return; Push(T_DR_ADD,Id(s),ev,Id(ph)); }
inline int16 __fastcall PL_Say(void* s,void*,uint32 ev,uint32 ctx,void* attach,uint32 delay,float prob,uint32 ovr,uint32 forceA,uint32 front){
    if(!Mine())return 0; Push(T_PL_SAY,Id(s),ev,ctx,Id(attach)); Push(T_PL_SAY2,delay,F(prob),(ovr&0xFFu)|((forceA&0xFFu)<<8)|((front&0xFFu)<<16)); return int16(Ask(T_PL_SAY)); }
inline void __fastcall PL_Init(void* s,void*){ if(!Mine())return; Push(T_PL_INIT,Id(s)); }

// --- CAERadioTrackManager (0x8CB6F8)
inline void __fastcall RD_Stop(void* s,void*,void* set,uint32 b){ if(!Mine())return; Push(T_RD_STOP,Id(s),Id(set),b&0xFFu); }
inline void __fastcall RD_Auto(void* s,void*,uint32 b){ if(!Mine())return; Push(T_RD_AUTO,Id(s),b&0xFFu); }
inline void __fastcall RD_BassEnh(void* s,void*,uint32 b){ if(!Mine())return; Push(T_RD_BASSE,Id(s),b&0xFFu); }
inline void __fastcall RD_Bass(void* s,void*,uint32 bass,float g){ if(!Mine())return; Push(T_RD_BASS,Id(s),bass,F(g)); }
inline void __fastcall RD_InitId(void* s,void*,uint32 id){ if(!Mine())return; Push(T_RD_IDS,Id(s),id); }
inline bool __fastcall RD_On(void* s,void*){ if(!Mine())return false; Push(T_RD_ON,Id(s)); return Ask(T_RD_ON)!=0; }
inline bool __fastcall RD_Just(void* s,void*){ if(!Mine())return false; Push(T_RD_JUST,Id(s)); return Ask(T_RD_JUST)!=0; }
inline const void* __fastcall RD_Name(void* s,void*,uint32 id){ if(!Mine())return nullptr; Push(T_RD_NAME,Id(s),id); return Pick(Ask(T_RD_NAME),w.names,64,4); }
inline void __fastcall RD_Key(void* s,void*,uint32 id,char* out){
    if(!Mine())return; Push(T_RD_KEY,Id(s),id,Id(out)); if(!out)return;
    const int32 k=Ask(T_RD_KEY); for(int i=0;i<8;++i)out[i]=char('A'+((k+i*3)&15)); out[8]=0; }
inline int32* __fastcall RD_Listen(void* s,void*){ if(!Mine())return nullptr; Push(T_RD_LIST,Id(s)); return Ask(T_RD_LIST) ? w.listen : nullptr; }
inline void __fastcall RD_Display(void* s,void*){ if(!Mine())return; Push(T_RD_DISP,Id(s)); }
inline int8 __fastcall RD_Cur(void* s,void*){ if(!Mine())return 0; Push(T_RD_CUR,Id(s)); return int8(Ask(T_RD_CUR)); }
inline bool __cdecl RD_Veh(){ if(!Mine())return false; Push(T_RD_VEH); return Ask(T_RD_VEH)!=0; }
inline void __fastcall RD_Start1(void* s,void*,uint32 id,uint32 bass,float g,uint32 skip){ if(!Mine())return; Push(T_RD_ST1,Id(s),id,bass,F(g)); Push(T_RD_ST1,skip&0xFFu); }
inline void __fastcall RD_Start2(void* s,void*,const void* set){ if(!Mine())return; Push(T_RD_ST2,Id(s),Id(set),set ? Fnv32(set,0x40) : 0u); }
inline void __fastcall RD_Retune(void* s,void*,uint32 id){ if(!Mine())return; Push(T_RD_RET,Id(s),id); }
inline void __fastcall RD_Service(void* s,void*,int32 t){ if(!Mine())return; Push(T_RD_SVC,Id(s),uint32(t)); }
inline void __fastcall RD_Reset(void* s,void*){ if(!Mine())return; Push(T_RD_RST,Id(s)); }
inline bool __fastcall RD_Init(void* s,void*,int32 ch){ if(!Mine())return false; Push(T_RD_INIT,Id(s),uint32(ch)); return Ask(T_RD_INIT)!=0; }
inline void __cdecl RD_ResetStats(){ if(!Mine())return; Push(T_RD_STAT); }
inline void* __cdecl VE_Settings(){ if(!Mine())return nullptr; Push(T_VE_SET); return Pick(Ask(T_VE_SET),w.sett,0x80,2); }

// --- CAECutsceneTrackManager (0x8AE554)
inline void __fastcall CT_Play(void* s,void*){ if(!Mine())return; Push(T_CT_PLAY,Id(s)); }
inline void __fastcall CT_Stop(void* s,void*){ if(!Mine())return; Push(T_CT_STOP,Id(s)); }
inline bool __fastcall CT_Active(void* s,void*){ if(!Mine())return false; Push(T_CT_ACT,Id(s)); return Ask(T_CT_ACT)!=0; }
inline uint8 __fastcall CT_Status(void* s,void*){ if(!Mine())return 0; Push(T_CT_STAT,Id(s)); return uint8(Ask(T_CT_STAT)); }
inline void __fastcall CT_Pause(void* s,void*,uint32 b){ if(!Mine())return; Push(T_CT_PAUSE,Id(s),b&0xFFu); }
inline void __fastcall CT_Service(void* s,void*,int32 t){ if(!Mine())return; Push(T_CT_SVC,Id(s),uint32(t)); }
inline void __fastcall CT_PreBeat(void* s,void*,uint32 id,uint32 b){ if(!Mine())return; Push(T_CT_PBEAT,Id(s),id&0xFFFFu,b&0xFFu); }
inline void __fastcall CT_PreCut(void* s,void*,uint32 id,uint32 b){ if(!Mine())return; Push(T_CT_PCUT,Id(s),id&0xFFFFu,b&0xFFu); }

// --- CAEAmbienceTrackManager (0x8AC15C)
inline bool __fastcall AM_Active(void* s,void*){ if(!Mine())return false; Push(T_AM_ACT,Id(s)); return Ask(T_AM_ACT)!=0; }
inline void __fastcall AM_Stop(void* s,void*){ if(!Mine())return; Push(T_AM_STOP,Id(s)); }
inline bool __fastcall AM_Radio(void* s,void*){ if(!Mine())return false; Push(T_AM_RAD,Id(s)); return Ask(T_AM_RAD)!=0; }
inline void __fastcall AM_Service(void* s,void*,int32 t){ if(!Mine())return; Push(T_AM_SVC,Id(s),uint32(t)); }
inline void __fastcall AM_Special(void* s,void*){ if(!Mine())return; Push(T_AM_SPEC,Id(s)); }
inline void __fastcall AM_Reset(void* s,void*){ if(!Mine())return; Push(T_AM_RST,Id(s)); }

// --- CAESoundManager (0xB62CB0)
inline void __fastcall SM_Service(void* s,void*){ if(!Mine())return; Push(T_SM_SVC,Id(s)); }
inline void __fastcall SM_Reset(void* s,void*){ if(!Mine())return; Push(T_SM_RST,Id(s)); }
inline void __fastcall SM_Pause(void* s,void*,uint32 b){ if(!Mine())return; Push(T_SM_PAUSE,Id(s),b&0xFFu); }
inline void __fastcall SM_Terminate(void* s,void*){ if(!Mine())return; Push(T_SM_TERM,Id(s)); }
inline bool __fastcall SM_Init(void* s,void*){ if(!Mine())return false; Push(T_SM_INIT,Id(s)); return Ask(T_SM_INIT)!=0; }

// --- varios
inline void* __cdecl X_FindPlayerPed(int32 id){ if(!Mine())return nullptr; Push(T_FIND,uint32(id)); return Pick(Ask(T_FIND),w.ent,0x100,6); }
inline void __cdecl X_PedSpeechReset(){ if(!Mine())return; Push(T_PS_RST); }
inline void __cdecl X_PedSpeechService(){ if(!Mine())return; Push(T_PS_SVC); }
inline void __cdecl X_PoliceService(){ if(!Mine())return; Push(T_PO_SVC); }
inline void __cdecl X_PoliceReset(){ if(!Mine())return; Push(T_PO_RST); }
inline void __cdecl X_WeatherReset(){ if(!Mine())return; Push(T_WE_RST); }
inline void __fastcall X_UserRadioShutdown(void* s,void*){ if(!Mine())return; Push(T_UR_SHUT,Id(s)); }
inline void __cdecl X_LoadPause(){ if(!Mine())return; Push(T_LS_PAUSE); }
inline void __cdecl X_LoadContinue(){ if(!Mine())return; Push(T_LS_CONT); }
inline void* __cdecl X_OpenFile(const char* path,const char* mode){ if(!Mine())return nullptr; Push(T_F_OPEN,S(path),S(mode)); return Ask(T_F_OPEN) ? static_cast<void*>(w.file) : nullptr; }
inline size_t __cdecl X_ReadFile(void* file,void* buf,size_t n){ if(!Mine())return 0; Push(T_F_READ,Id(file),Id(buf)==0 ? 0u : 1u,uint32(n)); return size_t(uint32(Ask(T_F_READ))); }
inline int32 __cdecl X_CloseFile(void* file){ if(!Mine())return 0; Push(T_F_CLOSE,Id(file)); return 0; }
inline void __cdecl X_InitUtil(){ if(!Mine())return; Push(T_I_UTIL); }
inline void __cdecl X_InitPed(){ if(!Mine())return; Push(T_I_PED); }
inline void __cdecl X_InitSpeech(){ if(!Mine())return; Push(T_I_SPEECH); }
inline void __cdecl X_InitVeh(){ if(!Mine())return; Push(T_I_VEH); }
inline void __cdecl X_InitExpl(){ if(!Mine())return; Push(T_I_EXPL); }
inline void __cdecl X_InitWeather(){ if(!Mine())return; Push(T_I_WEATHER); }
inline void __cdecl X_InitDoor(){ if(!Mine())return; Push(T_I_DOOR); }
inline void __cdecl X_InitFire(){ if(!Mine())return; Push(T_I_FIRE); }
inline void __cdecl X_InitPolice(){ if(!Mine())return; Push(T_I_POLICE); }
inline void __fastcall X_AllocStaticMatrix(void* self,void*){ if(!Mine())return; Push(T_ASM,Id(self)); if(self)At<void*>(self,0x14)=w.mtx; }
inline int __cdecl X_Rand(){ if(!Mine())return 0x1234; Push(T_RAND); return 0x2345; }
inline void __cdecl X_Delete(void* p){ if(!Mine()){ if(p)reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; } Push(T_DEL,Id(p)); }
inline void __fastcall X_EngineService(void* s,void*){ if(!Mine())return; Push(T_ENG_SVC,Id(s)); }

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
enum : uint32 { G_COMMON=1, G_INIT=2, G_CTOR=4, G_DEL=8, G_ENG=16, G_FIND=32 };
struct Stub { uint32 group; uintptr_t exe,dll,body; void* fn; };
inline std::vector<Stub> stubs;
inline void AddStub(uint32 group,uintptr_t exe,uintptr_t dllSym,void* fn){ stubs.push_back({group,exe,dllSym,dllSym ? Body(dllSym) : 0,fn}); }
// Conjunto de parches SIN reservas de memoria: los EntryPatch viven en almacenamiento estatico (el conjunto se cierra ANTES de
// construir el json de salida).
struct PatchSet {
    static constexpr size_t MAXP=640;
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

// Instantanea de globales tocados (se restaura al terminar cada ejecucion). Las que llevan 'hashed' entran en el hash de globales.
struct Saved { uintptr_t addr; std::vector<uint8> bytes; bool hashed; };
inline std::vector<Saved> saved;
inline void Save(uintptr_t a,size_t n,bool hashed=true){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n),hashed}); }
inline void RestoreAll(){ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size()); }
inline bool AllRestored(){ bool ok=true; for(auto& s:saved)ok&=std::memcmp(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size())==0; return ok; }

template<class R,class... A> inline R CallThis(uintptr_t a,void* self,A... args){ return reinterpret_cast<R(__thiscall*)(void*,A...)>(a)(self,args...); }
template<class R,class... A> inline R CallCdecl(uintptr_t a,A... args){ return reinterpret_cast<R(__cdecl*)(A...)>(a)(args...); }

// Memoria dinamica real (operator new[] / delete del EXE) para los buffers que las funciones reservan o liberan.
inline void* RealNewArr(size_t n){ return reinterpret_cast<void*(__cdecl*)(size_t)>(0x821195)(n); }
inline void RealDelete(void* p){ if(p)reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(p); }
inline void RealDeleteArr(void* p){ if(p)reinterpret_cast<void(__cdecl*)(void*)>(0x8213AE)(p); }
}
