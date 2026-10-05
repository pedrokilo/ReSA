#pragma once
// Banco dentro del motor: CAEPedSpeechAudioEntity (lista de 34, 58 funciones con 8 virtuales de la vtable 0x85F310).
// Original (hooks de la clase desactivados) frente a candidato con fixtures deterministas:
// - Cuatro peatones falsos (0x7A0 bytes, tipo de entidad peatón) con su entidad de voz en +0x294 (vtable del EXE),
//   matriz, inteligencia con la tarea facial secundaria falsa, datos de jugador (CWanted, ropa y grupo) y vehículo.
// - Los huecos de voz (0xB61C38), la memoria de frases (0xB61418), los estáticos 0xB613D0..0xB61418, los tiempos de
//   contexto (0xB61670), la cámara (matriz nula y posición propia), el contador de fotogramas, el tiempo, el estado del
//   juego, UnderWaterness, el contador de peticiones de streaming, los ajustes de radio, LastMissionPassedTime y las
//   entradas 19990..19997 de CModelInfo::ms_modelInfoPtrs (infos de peatón falsas) se fijan por caso y se restauran.
// - El hilo de streaming de audio se aparca durante todo el banco dentro de IsForegroundApp (como FixAudio).
// - Se simulan y registran, en el EXE y en la DLL: rand (0x821B1E) y CGeneral::GetRandomNumber, ResolveProbability,
//   CAEAudioHardware::IsSoundLoaded/LoadSound, CAESound::Initialise/StopSound/SetPosition/StopSoundAndForget,
//   CAESoundManager::RequestNewSound, CTaskComplexFacial::StopAll/SetRequest, CPed::IsAlive/GetPedTalking/
//   CanPedHoldConversation, CStreaming::IsVeryBusy, CAEAudioUtility::FindVehicleOfPlayer y
//   GetBankAndSoundFromScriptSlotAudioEvent, FindPlayerPed, CStats::GetStatValue, CGameLogic::FindCityClosestToPoint,
//   CPedGroupMembership::CountMembersExcludingLeader/GetMember y CPedModelInfo::IncrementVoice.
// - Las tablas de contextos, voces y bancos del EXE se leen tal cual (solo lectura).
// - Fuera de los fixtures: m_PedSpeechSlotID = -1 donde el EXE indexa el hueco (escribiría en 0xB61C1C..0xB61C37),
//   voces fuera de la tabla de su tipo en GetSoundAndBankIDs (lecturas fuera de tabla) y peatones/tareas nulos donde
//   el EXE los usa sin comprobar.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Entities/AEPedSpeechAudioEntity.h"
#include "Audio/AEAudioUtility.h"
#include "Audio/AESound.h"
#include "Audio/Managers/AESoundManager.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Tasks/TaskTypes/TaskComplexFacial.h"
#include "PedGroupMembership.h"
#include "Models/PedModelInfo.h"
#include "Stats.h"
#include "GameLogic.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace PedSpeech58Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline std::string Hex(const void* p,size_t n){ static const char* d="0123456789abcdef"; std::string s; s.reserve(n*2); auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){ s.push_back(d[b[i]>>4]); s.push_back(d[b[i]&15]); } return s; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& G(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
// Sigue los saltos de la ILT de la DLL sin entrar en la imagen del EXE (como FixAudio)
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
inline void Restore(const Saved& s){ std::memcpy(reinterpret_cast<void*>(s.at),s.bytes.data(),s.bytes.size()); }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }

// Parches de una sola ejecución: sin direcciones repetidas y deshechos en orden inverso.
struct Patches {
    std::array<std::unique_ptr<ReversingProbe::EntryPatch>,64> P{}; size_t n{}; std::array<uintptr_t,64> seen{};
    template<class Fn> void Add(uintptr_t at,Fn fake){ for(size_t k=0;k<n;++k)if(seen[k]==at)return; seen[n]=at; P[n++]=std::make_unique<ReversingProbe::EntryPatch>(at,fake); }
    ~Patches(){ while(n)P[--n].reset(); }
};

// --- hilo de streaming aparcado en IsForegroundApp ---
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground(){
    if(streamTid&&GetCurrentThreadId()==streamTid){ SetEvent(parkedEvt); WaitForSingleObject(resumeEvt,INFINITE); }
    return *reinterpret_cast<volatile uint32*>(0x8D621C)!=0; }

// --- direcciones del EXE ---
constexpr uintptr_t SLOTS=0xB61C38, SLOTS_SIZE=6*0x1C, STATICS=0xB613D0, STATICS_SIZE=0x48, PHRASES=0xB61418, PHRASES_SIZE=150*4;
constexpr uintptr_t TIMES=0xB61670, TIMES_SIZE=360*4, CAM=0xB6F028, TIME_ADDR=0xB7CB84, FRAME_ADDR=0xB7CB4C;
constexpr uintptr_t GROUPS=0xC09920, SPEECH_VT=0x85F310;
constexpr int MI_FAKE=19990; constexpr size_t NPED=4, PED_SIZE=0x7A0, SPEECH_OFF=0x294;

// --- objetos falsos ---
alignas(16) inline std::array<std::array<uint8,PED_SIZE>,NPED> peds{};
alignas(16) inline std::array<std::array<uint8,0x48>,NPED> pmats{};
alignas(16) inline std::array<std::array<uint8,0x60>,NPED> intels{};
alignas(16) inline std::array<std::array<uint8,0x40>,NPED> facials{};
alignas(16) inline std::array<std::array<uint8,0x74>,4> snds{};
alignas(16) inline std::array<std::array<uint8,0x20>,2> vehs{};
alignas(16) inline std::array<std::array<uint8,0x60>,8> mis{};
alignas(16) inline std::array<uint8,0x60> pdata{}; alignas(16) inline std::array<uint8,0x40> wanted{}; alignas(16) inline std::array<uint8,0x80> desc{};
alignas(16) inline std::array<uint8,0x100> cbuf{};
alignas(16) inline std::array<uint8,0x40> pedVt{};
alignas(16) inline std::array<char,64> nameBuf{};
inline uint8* Speech(size_t k){ return peds[k].data()+SPEECH_OFF; }
inline uint32 P(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<NPED;++k){ if(p==peds[k].data())return 0x10+uint32(k); if(p==Speech(k))return 0x20+uint32(k); if(p==facials[k].data())return 0x40+uint32(k); if(p==intels[k].data())return 0x70+uint32(k); }
    for(size_t k=0;k<snds.size();++k)if(p==snds[k].data())return 0x30+uint32(k);
    for(size_t k=0;k<vehs.size();++k)if(p==vehs[k].data())return 0x50+uint32(k);
    for(size_t k=0;k<mis.size();++k)if(p==mis[k].data())return 0x60+uint32(k);
    if(u>=GROUPS&&u<GROUPS+8*0x2D4)return 0x100000+uint32(u-GROUPS);
    if(u==0xB5F8B8)return 0x80; if(u==0xB62CB0)return 0x81; if(p==cbuf.data())return 0x82;
    return 0xEEEEEEEEu; }

// --- estado de los falsos (fijado por caso) ---
inline std::array<uint16,16> rnd{}; inline uint32 rndIdx{};
inline uint32 bitMask{}, bitIdx{};
inline bool Bit(){ return (bitMask>>(bitIdx++%32))&1; }
inline float statFat{}, statMuscle{}, statRespect{}, statBudget{};
inline void* playerRet{}; inline void* vehRet{}; inline int32 cityRet{}; inline int32 countRet{};
inline std::array<void*,8> members{};
inline int32 scriptBank{}, scriptSound{}; inline uint32 scriptMode{};
inline uint32 reqIdx{}; inline uint32 reqNullMask{};

inline int __cdecl FakeRand(){ if(!Main())return 0; const uint16 r=rnd[rndIdx%16]; Push(1,rndIdx++,r); return r; }
inline bool __cdecl FakeProb(float p){ if(!Main())return true; const bool b=Bit(); Push(2,F(p),b); return b; }
inline bool __fastcall FakeIsLoaded(void* hw,void*,int32 bank,int32 sfx,int32 slot){ if(!Main())return false; const bool b=Bit(); Push(3,P(hw),uint32(bank)&0xFFFF,uint32(sfx)&0xFFFF,(uint32(slot)&0xFFFF)|(uint32(b)<<16)); return b; }
inline void __fastcall FakeLoadSound(void* hw,void*,int32 bank,int32 sfx,int32 slot){ if(!Main())return; Push(4,P(hw),uint32(bank)&0xFFFF,uint32(sfx)&0xFFFF,uint32(slot)&0xFFFF); }
inline void __fastcall FakeSndInit(uint8* s,void*,uint32 bank,uint32 sfx,void* ent,float x,float y,float z,float vol,float roll,float speed,float dop,uint32 delay,uint32 flags,float var,uint32 play){
    if(!Main())return;
    Push(5,(bank&0xFFFF)|((sfx&0xFFFF)<<16),P(ent),F(x)^F(y)*3^F(z)*7,F(vol)); Push(6,F(roll),F(speed),F(dop),(delay&0xFF)|((flags&0xFFFF)<<8)|((play&0xFFFF)<<24)); Push(7,F(var));
    // estado determinista del sonido (sin entidad física, para que el destructor no haga nada)
    std::memset(s,0,0x74); At<int16>(s,0)=int16(bank); At<int16>(s,2)=int16(sfx); At<void*>(s,4)=ent; At<float>(s,0x14)=vol; At<float>(s,0x18)=roll;
    At<float>(s,0x1C)=speed; At<float>(s,0x20)=var; At<float>(s,0x24)=x; At<float>(s,0x28)=y; At<float>(s,0x2C)=z; At<float>(s,0x50)=dop; s[0x54]=uint8(delay);
    At<uint16>(s,0x56)=uint16(flags); At<int16>(s,0x5C)=int16(play); At<uint16>(s,0x58)=1; }
inline void __fastcall FakeStopSound(void* s,void*){ if(!Main())return; Push(8,P(s)); }
inline void __fastcall FakeSetPos(void* s,void*,float x,float y,float z){ if(!Main())return; Push(9,P(s),F(x),F(y),F(z)); }
inline void __fastcall FakeStopForget(void* s,void*){ if(!Main())return; Push(10,P(s)); }
inline void* __fastcall FakeReqNew(void* m,void*,uint8* s){
    if(!Main())return nullptr;
    const uint32 k=reqIdx++; const bool fail=(reqNullMask>>(k%32))&1;
    Push(11,P(m),uint32(Fnv(s,0x74)),uint32(Fnv(s,0x74)>>32),P(At<void*>(s,4))|(uint32(fail)<<16));
    return fail ? nullptr : static_cast<void*>(snds[k%snds.size()].data()); }
inline void __fastcall FakeStopAll(void* t,void*){ if(!Main())return; Push(12,P(t)); }
inline void __fastcall FakeSetRequest(void* t,void*,int32 a,int32 da,int32 b,int32 db){ if(!Main())return; Push(13,P(t),uint32(a),uint32(da),uint32(b)^(uint32(db)<<20)); }
inline bool __fastcall FakeIsAlive(void* p,void*){ if(!Main())return true; const bool b=Bit(); Push(14,P(p),b); return b; }
inline bool __fastcall FakePedTalking(void* p,void*){ if(!Main())return false; const bool b=Bit(); Push(15,P(p),b); return b; }
inline bool __fastcall FakeCanHold(void* p,void*){ if(!Main())return false; const bool b=Bit(); Push(16,P(p),b); return b; }
inline bool __cdecl FakeBusy(){ if(!Main())return false; const bool b=Bit(); Push(17,b); return b; }
inline void* __cdecl FakeVehOfPlayer(){ if(!Main())return nullptr; Push(18,P(vehRet)); return vehRet; }
inline bool __cdecl FakeScriptBank(const int32* ev,int32* bank,int32* sound,int32 slot){
    if(!Main())return false;
    Push(19,uint32(*ev),uint32(slot),scriptMode);
    if(scriptMode&1)*bank=scriptBank; if(scriptMode&2)*sound=scriptSound; return (scriptMode&4)!=0; }
inline void* __cdecl FakeFindPlayer(int32 id){ if(!Main())return nullptr; Push(20,uint32(id),P(playerRet)); return playerRet; }
inline float __cdecl FakeStat(int32 s){
    if(!Main())return 0.f;
    const float v=s==21 ? statFat : s==23 ? statMuscle : s==66 ? statRespect : s==14 ? statBudget : 0.f; Push(21,uint32(s),F(v)); return v; }
inline int32 __cdecl FakeCity(float x,float y){ if(!Main())return 0; Push(22,F(x),F(y),uint32(cityRet)); return cityRet; }
inline int32 __fastcall FakeCount(void* m,void*){ if(!Main())return 0; Push(23,P(m),uint32(countRet)); return countRet; }
inline void* __fastcall FakeGetMember(void* m,void*,int32 i){ if(!Main())return nullptr; void* r=i>=0&&i<8 ? members[i] : nullptr; Push(24,P(m),uint32(i),P(r)); return r; }
inline void __fastcall FakeIncVoice(void* mi,void*){ if(!Main())return; Push(25,P(mi)); }
inline void __fastcall FakePedVBad(void* p,void*){ if(!Main())return; Push(26,P(p)); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=58;
inline const std::array<Fn,NFN> fns{{
    {"CAEPedSpeechAudioEntity","AddSayEvent",0x4E6550,0,-1,1024},
    {"CAEPedSpeechAudioEntity","AddScriptSayEvent",0,0x85F310,1,512},
    {"CAEPedSpeechAudioEntity","CanPedHoldConversation",0x4E69E0,0,-1,256},
    {"CAEPedSpeechAudioEntity","CanPedSayGlobalContext",0x4E5730,0,-1,768},
    {"CAEPedSpeechAudioEntity","CanWePlayGlobalSpeechContext",0x4E5F10,0,-1,768},
    {"CAEPedSpeechAudioEntity","CanWePlayScriptedSpeech",0x4E4950,0,-1,256},
    {"CAEPedSpeechAudioEntity","Constructor",0x4E4F10,0,-1,64},
    {"CAEPedSpeechAudioEntity","DisableAllPedSpeech",0x4E3EB0,0,-1,64},
    {"CAEPedSpeechAudioEntity","DisablePedSpeech",0x4E56D0,0,-1,256},
    {"CAEPedSpeechAudioEntity","DisablePedSpeechForScriptSpeech",0x4E5700,0,-1,256},
    {"CAEPedSpeechAudioEntity","EnableAllPedSpeech",0x4E3EC0,0,-1,64},
    {"CAEPedSpeechAudioEntity","EnablePedSpeech",0x4E3F70,0,-1,128},
    {"CAEPedSpeechAudioEntity","EnablePedSpeechForScriptSpeech",0x4E3F90,0,-1,128},
    {"CAEPedSpeechAudioEntity","GetAllocatedVoice",0,0x85F310,4,64},
    {"CAEPedSpeechAudioEntity","GetAudioPedType",0x4E3C60,0,-1,128},
    {"CAEPedSpeechAudioEntity","GetCurrentCJMood",0x4E53B0,0,-1,768},
    {"CAEPedSpeechAudioEntity","GetNextMoodToUse",0x4E4700,0,-1,64},
    {"CAEPedSpeechAudioEntity","GetNextPlayTime",0x4E49E0,0,-1,512},
    {"CAEPedSpeechAudioEntity","GetNumSlotsPlayingContext",0x4E49B0,0,-1,256},
    {"CAEPedSpeechAudioEntity","GetPedTalking",0x4E3F50,0,-1,64},
    {"CAEPedSpeechAudioEntity","GetPedType",0,0x85F310,6,64},
    {"CAEPedSpeechAudioEntity","GetRepeatTime",0x4E47E0,0,-1,512},
    {"CAEPedSpeechAudioEntity","GetSexForSpecialPed",0x4E4260,0,-1,64},
    {"CAEPedSpeechAudioEntity","GetSexFromModel",0x4E4200,0,-1,256},
    {"CAEPedSpeechAudioEntity","GetSoundAndBankIDs",0x4E5920,0,-1,1024},
    {"CAEPedSpeechAudioEntity","GetSoundAndBankIDsForScriptedSpeech",0x4E4400,0,-1,256},
    {"CAEPedSpeechAudioEntity","GetSpecificSpeechContext",0x4E4470,0,-1,512},
    {"CAEPedSpeechAudioEntity","GetSpeechContextVolumeOffset",0x4E4AE0,0,-1,512},
    {"CAEPedSpeechAudioEntity","GetVoice",0x4E3CD0,0,-1,512},
    {"CAEPedSpeechAudioEntity","GetVoiceAndTypeForSpecialPed",0x4E4170,0,-1,256},
    {"CAEPedSpeechAudioEntity","GetVoiceAndTypeFromModel",0x4E58C0,0,-1,256},
    {"CAEPedSpeechAudioEntity","GetVoiceForMood",0x4E4760,0,-1,128},
    {"CAEPedSpeechAudioEntity","Initialise",0x4E68D0,0,-1,256},
    {"CAEPedSpeechAudioEntity","IsCJDressedInForGangSpeech",0x4E4270,0,-1,512},
    {"CAEPedSpeechAudioEntity","IsGlobalContextImportantForInterupting",0x4E4600,0,-1,384},
    {"CAEPedSpeechAudioEntity","IsGlobalContextImportantForStreaming",0x4E4510,0,-1,512},
    {"CAEPedSpeechAudioEntity","IsGlobalContextImportantForWidescreen",0x4E46B0,0,-1,512},
    {"CAEPedSpeechAudioEntity","IsGlobalContextPain",0x4E44F0,0,-1,384},
    {"CAEPedSpeechAudioEntity","IsGlobalContextUberImportant",0x4E46F0,0,-1,64},
    {"CAEPedSpeechAudioEntity","IsPedFemaleForAudio",0,0x85F310,7,64},
    {"CAEPedSpeechAudioEntity","LoadAndPlaySpeech",0x4E4840,0,-1,512},
    {"CAEPedSpeechAudioEntity","PlayLoadedSound",0,0x85F310,3,768},
    {"CAEPedSpeechAudioEntity","ReleasePedConversation",0x4E52A0,0,-1,512},
    {"CAEPedSpeechAudioEntity","ReleasePlayerConversation",0x4E3960,0,-1,512},
    {"CAEPedSpeechAudioEntity","RequestPedConversation",0x4E50E0,0,-1,512},
    {"CAEPedSpeechAudioEntity","RequestPlayerConversation",0x4E38C0,0,-1,512},
    {"CAEPedSpeechAudioEntity","ReservePedConversationSpeechSlots",0x4E37F0,0,-1,256},
    {"CAEPedSpeechAudioEntity","ReservePlayerConversationSpeechSlot",0x4E3870,0,-1,256},
    {"CAEPedSpeechAudioEntity","Reset",0x4E37B0,0,-1,64},
    {"CAEPedSpeechAudioEntity","Service",0x4E3710,0,-1,768},
    {"CAEPedSpeechAudioEntity","SetCJMood",0x4E3ED0,0,-1,256},
    {"CAEPedSpeechAudioEntity","SetNextPlayTime",0x4E4A20,0,-1,512},
    {"CAEPedSpeechAudioEntity","SetUpConversation",0x4E3A00,0,-1,512},
    {"CAEPedSpeechAudioEntity","StaticInitialise",0x5B98C0,0,-1,64},
    {"CAEPedSpeechAudioEntity","StopCurrentSpeech",0x4E3FB0,0,-1,512},
    {"CAEPedSpeechAudioEntity","Terminate",0,0x85F310,2,512},
    {"CAEPedSpeechAudioEntity","UpdateParameters",0,0x85F310,0,768},
    {"CAEPedSpeechAudioEntity","WillPedChatAboutTopic",0,0x85F310,5,512},
}};

// Número de voces de cada tipo de audio en las tablas del EXE (GEN, EMG, PLAYER, GANG, GFD) y de SPC (sin tabla)
constexpr std::array<int32,6> NUM_VOICES{0xD1,0x2E,0x14,0x34,0x12,0x2D};
inline int16 BankOfVoice(int16 type,int16 voice){
    static const uintptr_t banks[]{0x8AF700,0x8BA470,0x8BBED0,0x8BE5B8,0x8C4288};
    return type>=0&&type<5 ? G<int16>(banks[type]+uintptr_t(voice)*2) : int16(-1); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"pedspeech58-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"pedspeech58-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pedspeech58");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dirPath/"pedspeech58-only.txt"); if(f)std::getline(f,only); }
    if(!G<int8*>(0xBD00F8)){Write({{"event","error"},{"reason","audio event volumes not loaded"},{"state_restored",true}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // Cuerpos de la DLL resueltos antes de escribir ningún parche
    using std::bit_cast;
    const uintptr_t dllForeground=Body(bit_cast<uintptr_t>(&IsForegroundApp));
    const std::vector<std::pair<uintptr_t,uintptr_t>> stubAddr{
        {0x821B1E,Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber))},
        {0x4D9C80,Body(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability))},
        {0x4D8EF0,Body(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundLoaded))},
        {0x4D8ED0,Body(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSound))},
        {0x4EFE50,Body(bit_cast<uintptr_t>(&CAESound::Initialise))},
        {0x4EF1C0,Body(bit_cast<uintptr_t>(&CAESound::StopSound))},
        {0x4EF880,Body(bit_cast<uintptr_t>(&CAESound::SetPosition))},
        {0x4EF850,Body(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget))},
        {0x4EFB10,Body(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound))},
        {0x691250,Body(bit_cast<uintptr_t>(&CTaskComplexFacial::StopAll))},
        {0x691230,Body(bit_cast<uintptr_t>(&CTaskComplexFacial::SetRequest))},
        {0x5E0170,Body(bit_cast<uintptr_t>(&CPed::IsAlive))},
        {0x5EFF50,Body(bit_cast<uintptr_t>(&CPed::GetPedTalking))},
        {0x5EFFA0,Body(bit_cast<uintptr_t>(&CPed::CanPedHoldConversation))},
        {0x4076A0,Body(bit_cast<uintptr_t>(&CStreaming::IsVeryBusy))},
        {0x4D9E10,Body(bit_cast<uintptr_t>(&CAEAudioUtility::FindVehicleOfPlayer))},
        {0x4D9CC0,Body(bit_cast<uintptr_t>(&CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent))},
        {0x56E210,Body(bit_cast<uintptr_t>(&FindPlayerPed))},
        {0x558E40,Body(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)))},
        {0x441B70,Body(bit_cast<uintptr_t>(&CGameLogic::FindCityClosestToPoint))},
        {0x5F6AA0,Body(bit_cast<uintptr_t>(&CPedGroupMembership::CountMembersExcludingLeader))},
        {0x5F69B0,Body(bit_cast<uintptr_t>(&CPedGroupMembership::GetMember))},
        {0x4C7300,Body(bit_cast<uintptr_t>(&CPedModelInfo::IncrementVoice))},
    };
    const std::array<void*,23> stubFake{
        reinterpret_cast<void*>(&FakeRand),reinterpret_cast<void*>(&FakeProb),reinterpret_cast<void*>(&FakeIsLoaded),reinterpret_cast<void*>(&FakeLoadSound),
        reinterpret_cast<void*>(&FakeSndInit),reinterpret_cast<void*>(&FakeStopSound),reinterpret_cast<void*>(&FakeSetPos),reinterpret_cast<void*>(&FakeStopForget),
        reinterpret_cast<void*>(&FakeReqNew),reinterpret_cast<void*>(&FakeStopAll),reinterpret_cast<void*>(&FakeSetRequest),reinterpret_cast<void*>(&FakeIsAlive),
        reinterpret_cast<void*>(&FakePedTalking),reinterpret_cast<void*>(&FakeCanHold),reinterpret_cast<void*>(&FakeBusy),reinterpret_cast<void*>(&FakeVehOfPlayer),
        reinterpret_cast<void*>(&FakeScriptBank),reinterpret_cast<void*>(&FakeFindPlayer),reinterpret_cast<void*>(&FakeStat),reinterpret_cast<void*>(&FakeCity),
        reinterpret_cast<void*>(&FakeCount),reinterpret_cast<void*>(&FakeGetMember),reinterpret_cast<void*>(&FakeIncVoice)};
    // Ninguna dirección de stub puede ser la entrada de una función de la clase (el stub saltaría a sí mismo)
    for(size_t f=0;f<fns.size();++f)if(fns[f].address)for(auto& [e,d]:stubAddr)if(e==fns[f].address||d==fns[f].address){Write({{"event","error"},{"reason","stub on class entry"}});return;}

    // Claves de las prendas de banda y de los nombres de peatones especiales (para acertar a veces)
    static const char* gangTex[]{"hoodyagreen","shirtbgang","tracktrgang","denimsgang","bandgang","hockey","capgang","capgangback","capgangside","capgangover","capgangup","bandred3","bandblue3","bandgang3","bandblack3"};
    std::array<uint32,15> gangKeys{}; for(size_t k=0;k<15;++k)gangKeys[k]=CKeyGen::GetUppercaseKey(gangTex[k]);
    std::array<uint32,45> specialKeys{}; for(size_t k=0;k<45;++k)specialKeys[k]=CKeyGen::GetUppercaseKey(reinterpret_cast<const char*>(0x8C65C0+k*20));
    static const uintptr_t voiceNames[5][2]{{0x8AE6A8,0x8AF6FC},{0x8BA0D8,0x8BA470},{0x8BBD40,0x8BBED0},{0x8BE1A8,0x8BE5B8},{0x8C4120,0x8C4288}};
    for(auto& s:pedVt)s=0; for(size_t k=0;k<pedVt.size()/4;++k)At<void*>(pedVt.data(),k*4)=reinterpret_cast<void*>(&FakePedVBad);

    // estado global vigilado
    const std::vector<Saved> savedAll{
        Save(SLOTS-0x28,SLOTS_SIZE+0x28), Save(STATICS,STATICS_SIZE), Save(PHRASES,PHRASES_SIZE), Save(TIMES,TIMES_SIZE), Save(CAM,0x48), Save(TIME_ADDR,4), Save(FRAME_ADDR,4),
        Save(0x96A8B0,4), Save(0xC8132C,4), Save(0x8E4CB8,4), Save(0xB6B98C,4), Save(0xA476A8,4), Save(0xA9B0C8+4*MI_FAKE,4*8) };
    // solo se comprueban al final (los parches de IsForegroundApp y la vtable cambian durante el banco)
    const std::vector<Saved> savedFinal{ Save(0x746060,5), Save(dllForeground,5), Save(SPEECH_VT,8*4) };
    auto restoreAll=[&]{ for(auto& s:savedAll)Restore(s); };

    // aparcar el hilo de streaming de audio
    parkedEvt=CreateEventA(nullptr,FALSE,FALSE,nullptr); resumeEvt=CreateEventA(nullptr,TRUE,FALSE,nullptr);
    const HANDLE streamThread=AEAudioHardware.m_pStreamThread.m_nHandle;
    const bool streamRunning=streamThread&&WaitForSingleObject(streamThread,0)==WAIT_TIMEOUT;
    streamTid=streamRunning ? GetThreadId(streamThread) : 0;
    std::array<bool,NFN> enabled{},locked{};
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch fg1(0x746060,FakeForeground), fg2(dllForeground,FakeForeground);
        const bool parkedOk=!streamRunning||WaitForSingleObject(parkedEvt,3000)==WAIT_OBJECT_0;
        if(!parkedOk){ SetEvent(resumeEvt); Write({{"event","error"},{"reason","audio stream thread not parked"},{"state_restored",true}}); return; }
        Write({{"event","stream_thread"},{"running",streamRunning},{"parked",streamRunning}});
        for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* w=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)w[k]=uint8(next()); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x58Au; if(!stt)stt=1;
                    nrec=0; rndIdx=0; bitIdx=0; reqIdx=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // ---------------- fixtures comunes ----------------
                    const uint32 now=next()%8 ? 200000+next()%2000000 : next(); G<uint32>(TIME_ADDR)=now;
                    G<uint32>(FRAME_ADDR)=next()%3==0 ? (next()&~0x3Fu) : next();
                    G<uint8>(0x96A8B0)=uint8(next()%6==0 ? 2 : next()%5);
                    static const uint32 waters[]{0x3F000000u,0x3EFFFFFFu,0x3F000001u,0u,0x3F800000u,0x7FC00000u,0xBF800000u};
                    G<uint32>(0xC8132C)=next()%2 ? waters[next()%7] : F(uni(-1.f,2.f));
                    G<int32>(0x8E4CB8)=next()%3 ? int32(next()%15) : int32(14+next()%4);
                    G<uint32>(0xB6B98C)=next()%2 ? 0x8C8000u : 0u;
                    G<uint32>(0xA476A8)=next()%3==0 ? 0xFFFFFFFFu : next()%3 ? now-next()%400000 : now+next()%1000;
                    // cámara sin matriz
                    At<void*>(reinterpret_cast<void*>(CAM),0x14)=nullptr; const float cx=uni(-500.f,500.f), cy=uni(-500.f,500.f), cz=uni(0.f,50.f);
                    G<float>(CAM+4)=cx; G<float>(CAM+8)=cy; G<float>(CAM+0xC)=cz; G<uint8>(CAM+0x3D)=uint8(next()%4==0);
                    // peatones falsos
                    auto voiceFor=[&](int16 type)->int16{ return type>=0&&type<6 ? int16(next()%NUM_VOICES[type]) : int16(next()%20); };
                    auto pickType=[&]()->int16{ const uint32 r=next()%16; return r<5 ? 0 : r<7 ? 1 : r<10 ? 2 : r<12 ? 3 : r<14 ? 4 : r<15 ? 5 : int16(int32(next()%4)-1+(next()%2)*7); };
                    for(size_t k=0;k<NPED;++k){
                        auto* pd=peds[k].data(); fill(pd,PED_SIZE); fill(pmats[k].data(),0x48); fill(intels[k].data(),0x60); fill(facials[k].data(),0x40);
                        At<void*>(pd,0)=pedVt.data(); pd[0x36]=uint8((pd[0x36]&~7)|3); At<int16>(pd,0x22)=int16(MI_FAKE+next()%8);
                        At<void*>(pd,0x14)=next()%3 ? nullptr : static_cast<void*>(pmats[k].data());
                        float* pos=At<void*>(pd,0x14) ? &At<float>(pmats[k].data(),0x30) : &At<float>(pd,4);
                        static const float exact[][3]{{24,32,0},{40,0,0},{0,0,40},{32,24,0},{24,32,0.01f},{0,40.0001f,0},{39.99f,0,0}};
                        if(next()%5==0){ const auto& e=exact[next()%7]; pos[0]=cx+e[0]; pos[1]=cy+e[1]; pos[2]=cz+e[2]; } else { pos[0]=cx+uni(-45.f,45.f); pos[1]=cy+uni(-45.f,45.f); pos[2]=cz+uni(-10.f,10.f); }
                        uint32 fl=next(); fl=(fl&~0x8100u)|(next()%3==0 ? 0x100u : 0u)|(next()%4==0 ? 0x8000u : 0u); At<uint32>(pd,0x46C)=fl;
                        At<void*>(pd,0x47C)=intels[k].data(); At<void*>(intels[k].data(),4+0x14+3*4)=facials[k].data();
                        At<void*>(pd,0x480)=k==0 ? static_cast<void*>(pdata.data()) : nullptr;
                        At<void*>(pd,0x58C)=next()%3 ? static_cast<void*>(vehs[next()%2].data()) : nullptr;
                        static const int32 pts[]{4,5,0x16,7,8,8,6,0,1,9,4,5};
                        At<int32>(pd,0x598)=pts[next()%12]; pd[0x754]=uint8(next()%2 ? 2 : next()%5);
                        // entidad de voz
                        uint8* sp=Speech(k);
                        At<uint32>(sp,0)=SPEECH_VT; At<void*>(sp,4)=next()%8 ? static_cast<void*>(pd) : static_cast<void*>(peds[next()%NPED].data());
                        sp[0x90]=uint8(next()%8 ? 1 : 0); const int16 type=pickType(); At<int16>(sp,0x92)=type; At<int16>(sp,0x94)=voiceFor(type);
                        At<int16>(sp,0x96)=int16(next()%6==0 ? 0x100 : next()%2);
                        sp[0x98]=uint8(next()%2); sp[0x99]=uint8(next()%5==0); sp[0x9A]=uint8(next()%7==0); sp[0x9B]=uint8(next()%4==0); sp[0x9C]=uint8(next()%4==0);
                        At<void*>(sp,0xA0)=next()%3 ? static_cast<void*>(snds[next()%4].data()) : nullptr;
                        At<int16>(sp,0xA4)=int16(next()%60); At<int16>(sp,0xA6)=int16(next()%200); At<int16>(sp,0xA8)=int16(next()%6);
                        At<float>(sp,0xAC)=uni(-30.f,10.f);
                        static const int16 ctxs[]{0x2D,0x151,0x152,0x153,0x154,0x160,0x162,0x163,0x165,0x166,0x167,0xD,0xF,0x7D,0x7F,0x74,0xA9,0xA1,0x30,0x37};
                        At<int16>(sp,0xB0)=next()%2 ? ctxs[next()%20] : int16(next()%0x168);
                        for(int t=0;t<19;++t)At<uint32>(sp,0xB4+t*4)=now+uint32(int32(next()%10000)-5000);
                    }
                    for(auto& s:snds)fill(s.data(),0x74);
                    for(auto& v:vehs)fill(v.data(),0x20);
                    // jugador
                    fill(pdata.data(),pdata.size()); fill(wanted.data(),wanted.size()); fill(desc.data(),desc.size());
                    At<void*>(pdata.data(),0)=wanted.data(); At<void*>(pdata.data(),4)=desc.data(); At<int32>(pdata.data(),0x38)=int32(next()%8);
                    At<int32>(wanted.data(),0x2C)=int32(next()%7);
                    auto key=[&](int lo,int hi){ return next()%3==0 ? gangKeys[lo+next()%(hi-lo)] : next(); };
                    At<uint32>(desc.data(),0x28)=key(0,2); At<uint32>(desc.data(),0x28+2*4)=key(2,4); At<uint32>(desc.data(),0x28+16*4)=key(4,11); At<uint32>(desc.data(),0x28+15*4)=key(11,15);
                    playerRet=next()%10 ? static_cast<void*>(peds[0].data()) : nullptr;
                    vehRet=next()%2 ? static_cast<void*>(vehs[next()%2].data()) : nullptr;
                    cityRet=next()%3 ? 1 : int32(next()%4);
                    static const int32 counts[]{0,1,1,1,2,3,0x10001,-1};
                    countRet=counts[next()%8];
                    for(auto& m:members)m=next()%3==0 ? static_cast<void*>(peds[next()%NPED].data()) : nullptr;
                    if(next()%2){ auto* m=Speech(1+next()%3); At<int16>(m,0x92)=int16(next()%3 ? 3 : next()%5); static const int16 vs[]{0x1A,0x25,0x20,0x1B}; At<int16>(m,0x94)=vs[next()%4]; }
                    auto stat=[&](float c,float w){ const uint32 r=next()%6; return r==0 ? c : r==1 ? std::nextafter(c,-1e9f) : r==2 ? std::bit_cast<float>(0x7FC00000u) : uni(c-w,c+w); };
                    statRespect=stat(650.f,300.f); statBudget=stat(10000.f,5000.f); statFat=stat(600.f,400.f);
                    statMuscle=next()%3==0 ? statFat-200.f : next()%8==0 ? std::bit_cast<float>(0x7FC00000u) : uni(0.f,1000.f);
                    if(nm=="GetSpeechContextVolumeOffset"&&next()%2){ const float d=next()%3==0 ? 1000.f : next()%2 ? -1000.f : uni(-1500.f,1500.f); statMuscle=uni(0.f,1000.f); statFat=statMuscle+d; if(next()%4==0)statFat=statMuscle+std::nextafter(d,0.f); }
                    // infos de modelo falsas
                    for(size_t k=0;k<mis.size();++k){ auto* m=mis[k].data(); fill(m,0x60);
                        const int16 at=pickType(); At<int16>(m,0x3C)=at; At<int16>(m,0x42)=next()%6==0 ? int16(-1-next()%3) : voiceFor(at);
                        static const int32 pts2[]{4,5,0x16,7,8,0,1};
                        At<int32>(m,0x28)=pts2[next()%7]; At<uint32>(m,4)=next()%2 ? specialKeys[next()%45] : next();
                        CModelInfo::ms_modelInfoPtrs[MI_FAKE+k]=reinterpret_cast<CBaseModelInfo*>(m); }
                    // falsos
                    for(auto& r:rnd){ static const uint16 ed[]{0,1,0x3FFF,0x4000,0x7FFF,0x7FFE,0x2000,0x6000}; r=next()%4==0 ? ed[next()%8] : uint16(next()%0x8000); }
                    bitMask=next(); if(next()%3==0)bitMask|=0x55555555u;
                    scriptMode=next()%8; scriptBank=next()%4 ? int32(next()%100) : -int32(next()%3); scriptSound=next()%4 ? int32(next()%200) : -int32(next()%3);
                    reqNullMask=next()%3 ? 0 : next();
                    // huecos de voz, estáticos, memoria de frases y tiempos
                    auto* slots=reinterpret_cast<uint8*>(SLOTS); fill(slots,SLOTS_SIZE);
                    for(int k=0;k<6;++k){ uint8* s=slots+k*0x1C;
                        const int16 st=int16(next()%6==0 ? 0 : next()%10==0 ? int16(next()%8) : next()%6); At<int16>(s,0)=st;
                        At<void*>(s,4)=next()%10==0&&st==0 ? nullptr : static_cast<void*>(Speech(next()%NPED));
                        At<void*>(s,8)=next()%2 ? static_cast<void*>(snds[next()%4].data()) : nullptr;
                        At<int16>(s,0xC)=int16(next()%60); At<int16>(s,0xE)=int16(next()%200);
                        At<uint32>(s,0x10)=next()%2 ? now-next()%3000 : now+next()%3000;
                        static const int16 ctxs[]{0x2D,0x151,0x152,0x160,0x166,0xD,0x7E,0xA9,0x30,0x10};
                        At<int16>(s,0x14)=next()%2 ? ctxs[next()%10] : int16(next()%0x168); At<int16>(s,0x16)=int16(next()%6);
                        s[0x18]=uint8(next()%3==0); s[0x19]=uint8(next()%5==0); s[0x1A]=uint8(next()%5==0); }
                    auto* st=reinterpret_cast<uint8*>(STATICS); fill(st,STATICS_SIZE);
                    G<uint32>(0xB613E0)=next()%2 ? now+next()%5000 : now-next()%5000; if(next()%8==0)G<uint32>(0xB613E0)=now;
                    static const int16 ovs[]{-1,-1,0,1,2,0x100,0x101};
                    for(uintptr_t o:{uintptr_t(0xB613D0),uintptr_t(0xB613D4),uintptr_t(0xB613D8)})G<int16>(o)=ovs[next()%7];
                    { static const int16 moods[]{-1,-1,0,1,2,3,4,5,6,7,8,9,10}; G<int16>(0xB613DC)=moods[next()%13]; }
                    G<uint8>(0xB613E4)=uint8(next()%2); G<uint8>(0xB613E5)=uint8(next()%3==0); G<uint8>(0xB613E6)=uint8(next()%8==0);
                    G<int16>(0xB613E8)=int16(next()%4);
                    G<uint8>(0xB613FC)=uint8(next()%3==0); G<uint8>(0xB613FD)=uint8(next()%3==0);
                    auto pedOrNull=[&]()->void*{ return next()%8==0 ? nullptr : static_cast<void*>(peds[next()%NPED].data()); };
                    G<void*>(0xB61400)=pedOrNull(); G<void*>(0xB61410)=pedOrNull(); G<void*>(0xB6140C)=pedOrNull();
                    G<int16>(0xB61408)=int16(int32(next()%7)-1); G<int16>(0xB61404)=int16(int32(next()%7)-1);
                    G<int16>(0xB61414)=next()%8 ? int16(next()%5) : int16(int32(next()%13)-3);
                    auto* ph=reinterpret_cast<uint8*>(PHRASES);
                    { const int16 bank=BankOfVoice(At<int16>(Speech(0),0x92),At<int16>(Speech(0),0x94)); const uint32 mode=next()%4;
                      for(int k=0;k<150;++k){ At<int16>(ph,k*4)=mode==0 ? int16(k) : int16(next()%40); At<int16>(ph,k*4+2)=mode<2||next()%2 ? bank : int16(next()%200); } }
                    for(int k=0;k<360;++k)G<uint32>(TIMES+k*4)=next()%4==0 ? now : now+uint32(int32(next()%10000)-5000);

                    uint8* self=Speech(0);
                    // GetSoundAndBankIDs con SPC y contexto de dolor lee rangos fuera de la tabla de dolor (posible rango vacío con
                    // memoria de pila sin inicializar en el EXE): fuera de los fixtures
                    if(nm=="AddSayEvent"||nm=="GetSoundAndBankIDs")for(size_t k=0;k<NPED;++k)if(At<int16>(Speech(k),0x92)==5)At<int16>(Speech(k),0x92)=0, At<int16>(Speech(k),0x94)=int16(next()%NUM_VOICES[0]);
                    // SetUpConversation lee el tipo de los dos peatones de la conversación sin comprobarlos
                    if(nm=="SetUpConversation"){ G<void*>(0xB61410)=peds[next()%NPED].data(); G<void*>(0xB6140C)=peds[next()%NPED].data(); }
                    {
                        Patches pt; for(size_t k=0;k<stubAddr.size();++k){ pt.Add(stubAddr[k].first,stubFake[k]); pt.Add(stubAddr[k].second,stubFake[k]); }
                        auto ctxArg=[&]()->int32{ static const int32 c[]{0x2D,0x151,0x152,0x153,0x154,0x155,0x160,0x162,0x163,0x165,0x166,0x167,0xD,0xF,0x7D,0x7E,0x7F,0x74,
                            0xA1,0xA5,0xA9,0xAB,0x30,0x37,0x40,0x41,0,1,0x168,-1,0x16C,0x150};
                            return next()%2 ? c[next()%std::size(c)] : int32(next()%0x168); };
                        if(nm=="AddSayEvent"){
                            if(next()%3==0){ G<int16>(0xB61408)=At<int16>(self,0xA8); }
                            const int32 ev=next()%8 ? 0x34 : int32(next()%0x60); const int32 g=ctxArg();
                            ret=int32(reinterpret_cast<int16(__thiscall*)(void*,int32,int32,uint32,float,uint32,uint32,uint32)>(a)(self,ev,g,next()%3000,uni(0.f,1.f),next()%2,next()%3==0,next()%3==0))&0xFFFF; }
                        else if(nm=="AddScriptSayEvent"){ const int32 ev=next()%6 ? 0x35 : int32(next()%0x60);
                            reinterpret_cast<void(__thiscall*)(void*,int32,int32,uint32,uint32,uint32)>(a)(self,ev,int32(next()%3000),next()%2,next()%3==0,next()%3==0); }
                        else if(nm=="CanPedHoldConversation"||nm=="GetPedTalking"||nm=="IsPedFemaleForAudio")ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(self)&0xFF;
                        else if(nm=="CanPedSayGlobalContext"||nm=="IsGlobalContextImportantForWidescreen"||nm=="IsGlobalContextImportantForStreaming"){
                            const int32 g=nm=="CanPedSayGlobalContext" ? ctxArg() : int32(i%0x170)-5;
                            ret=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,g)&0xFF; }
                        else if(nm=="CanWePlayGlobalSpeechContext"){
                            if(next()%3==0){ G<void*>(next()%2 ? 0xB61410 : 0xB6140C)=peds[0].data(); } if(next()%4==0)G<void*>(0xB61400)=peds[0].data();
                            ret=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,ctxArg())&0xFFFF; }
                        else if(nm=="CanWePlayScriptedSpeech"||nm=="GetCurrentCJMood")ret=reinterpret_cast<uint32(__cdecl*)()>(a)()&0xFFFF;
                        else if(nm=="Constructor"){ cbuf.fill(0xCD); ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(cbuf.data()))-intptr_t(cbuf.data()); out["cbuf"]=Hex(cbuf.data(),cbuf.size()); }
                        else if(nm=="DisableAllPedSpeech"||nm=="EnableAllPedSpeech"||nm=="Service"||nm=="Reset"||nm=="StaticInitialise"||nm=="ReleasePedConversation"||nm=="ReleasePlayerConversation"||nm=="SetUpConversation")
                            reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="ReservePedConversationSpeechSlots"||nm=="ReservePlayerConversationSpeechSlot"||nm=="IsCJDressedInForGangSpeech"){
                            if(nm!="IsCJDressedInForGangSpeech"&&next()%2)for(int k=0;k<6;++k)if(next()%2)At<int16>(slots+k*0x1C,0)=0;
                            ret=reinterpret_cast<uint32(__cdecl*)()>(a)()&0xFF; }
                        else if(nm=="DisablePedSpeech"||nm=="DisablePedSpeechForScriptSpeech"){ static const int32 v[]{0,1,0x10000,0x100,-1};
                            if(next()%5==0){ At<void*>(self,4)=peds[0].data(); At<void*>(peds[0].data(),0x47C)=next()%2 ? nullptr : static_cast<void*>(intels[0].data()); }
                            reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(self,v[next()%5]); }
                        else if(nm=="EnablePedSpeech"||nm=="EnablePedSpeechForScriptSpeech"||nm=="PlayLoadedSound"||nm=="StopCurrentSpeech"||nm=="Terminate"){
                            if(nm=="PlayLoadedSound"&&next()%10==0)At<void*>(self,4)=nullptr;
                            if((nm=="StopCurrentSpeech"||nm=="Terminate")&&next()%5==0){ auto* pd=static_cast<uint8*>(At<void*>(self,4)); if(next()%2)At<void*>(pd,0x47C)=nullptr; else At<void*>(intels[(P(pd)-0x10)%NPED].data(),0x24)=nullptr; }
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="GetAllocatedVoice"||nm=="GetPedType")ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(self)&0xFFFF;
                        else if(nm=="GetAudioPedType"){ static const char* names[]{"PED_TYPE_GEN","PED_TYPE_EMG","PED_TYPE_PLAYER","PED_TYPE_GANG","PED_TYPE_GFD","PED_TYPE_SPC","PED_TYPE_GENX","ped_type_gen","","PED_TYPE_GF","PED_TYPE_UNK"};
                            nameBuf.fill(0); std::strcpy(nameBuf.data(),names[next()%11]); ret=reinterpret_cast<uint32(__cdecl*)(const char*)>(a)(nameBuf.data())&0xFFFF; }
                        else if(nm=="GetVoice"){ const int32 t=next()%2 ? int32(next()%5) : int32(next()%9)-2; const uint32 nt=next()%5;
                            const uintptr_t b=voiceNames[nt][0], e=voiceNames[nt][1]; nameBuf.fill(0); std::strncpy(nameBuf.data(),reinterpret_cast<const char*>(b+20*(next()%((e-b)/20))),20);
                            if(next()%8==0)nameBuf[next()%4+9]^=0x20; if(next()%10==0)nameBuf[0]=0;
                            ret=reinterpret_cast<uint32(__cdecl*)(const char*,int32)>(a)(nameBuf.data(),t)&0xFFFF; }
                        else if(nm=="GetNextMoodToUse"||nm=="GetVoiceForMood")ret=reinterpret_cast<uint32(__stdcall*)(int32)>(a)(int32(next()%14)-2)&0xFFFF;
                        else if(nm=="GetNextPlayTime"){ const int32 g=next()%3 ? int32(0x150+next()%0x1C) : int32(next()%0x170)-5; ret=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,g); }
                        else if(nm=="GetNumSlotsPlayingContext"){ const int32 g=next()%3 ? int32(At<int16>(slots+(next()%6)*0x1C,0x14)) : ctxArg(); ret=reinterpret_cast<int32(__thiscall*)(void*,int32)>(a)(self,g); }
                        else if(nm=="GetRepeatTime")ret=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,int32(i%0x170)-5)&0xFFFF;
                        else if(nm=="GetSexForSpecialPed"||nm=="GetVoiceAndTypeForSpecialPed"){ const uint32 h=next()%3 ? specialKeys[next()%45] : next(); ret=reinterpret_cast<uint32(__thiscall*)(void*,uint32)>(a)(self,h)&0xFF; }
                        else if(nm=="GetSexFromModel"||nm=="GetVoiceAndTypeFromModel"){
                            if(nm=="GetSexFromModel"){ auto* m=mis[next()%8].data(); static const int16 ats[]{-2,-1,0,1,2,3,4,5,6,7}; At<int16>(m,0x3C)=ats[next()%10]; }
                            ret=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,MI_FAKE+int32(next()%8))&0xFF; }
                        else if(nm=="GetSoundAndBankIDs"){ int16 o[2]{int16(0xCDCD),int16(0xCDCD)};
                            if(next()%4==0){ { static const int16 ts[]{-1,0,1,2,3,4,6,7}; At<int16>(self,0x92)=next()%3 ? int16(2) : ts[next()%8]; } At<int16>(self,0x94)=voiceFor(At<int16>(self,0x92)); }
                            ret=reinterpret_cast<uint32(__thiscall*)(void*,int32,int16*)>(a)(self,ctxArg(),o)&0xFFFF; out["out"]=uint16(o[0]); }
                        else if(nm=="GetSoundAndBankIDsForScriptedSpeech")ret=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,int32(next()%0x80000))&0xFF;
                        else if(nm=="GetSpecificSpeechContext")ret=reinterpret_cast<uint32(__stdcall*)(int32,int32)>(a)(next()%4 ? int32(i%0x168) : ctxArg(),int32(next()%10)-2)&0xFFFF;
                        else if(nm=="GetSpeechContextVolumeOffset"){ const int32 g=next()%4 ? int32(0x150+next()%0x18) : ctxArg(); ret=F(reinterpret_cast<float(__stdcall*)(int32)>(a)(g)); }
                        else if(nm=="Initialise"){ auto* pd=peds[next()%NPED].data(); if(next()%8==0)At<int16>(pd,0x22)=int16(-1-next()%2);
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,pd); }
                        else if(nm=="IsGlobalContextImportantForInterupting"||nm=="IsGlobalContextPain"||nm=="IsGlobalContextUberImportant"){
                            const int32 g=fn.cases>64 ? int32(i%0x170)-5 : ctxArg(); ret=reinterpret_cast<uint32(__stdcall*)(int32)>(a)(g)&0xFF; }
                        else if(nm=="LoadAndPlaySpeech")reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(self,next()%2 ? next()%5000 : next());
                        else if(nm=="RequestPedConversation"){ const size_t pa=next()%NPED, pb=next()%3 ? (pa+1+next()%(NPED-1))%NPED : pa;
                            if(next()%2){ At<int16>(Speech(pb),0x94)=int16(At<int16>(Speech(pa),0x94)+1); }
                            if(next()%2){ G<uint8>(0xB613FC)=0; G<uint8>(0xB613FD)=0; G<uint8>(0xB613E6)=0; }
                            ret=reinterpret_cast<uint32(__cdecl*)(void*,void*)>(a)(peds[pa].data(),peds[pb].data())&0xFF; }
                        else if(nm=="RequestPlayerConversation"){ if(next()%2){ G<uint8>(0xB613FC)=0; G<uint8>(0xB613FD)=0; G<uint8>(0xB613E6)=0; }
                            ret=reinterpret_cast<uint32(__cdecl*)(void*)>(a)(peds[next()%NPED].data())&0xFF; }
                        else if(nm=="SetCJMood"){ static const int32 v[]{-1,0,1,2,0x100,0x10001};
                            reinterpret_cast<void(__cdecl*)(int32,uint32,int32,int32,int32)>(a)(int32(next()%14)-2,next()%2 ? next()%60000 : next(),v[next()%6],v[next()%6],v[next()%6]); }
                        else if(nm=="SetNextPlayTime"){ const int32 g=next()%3 ? int32(0x150+next()%0x1C) : int32(next()%0x170)-5; reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(self,g); }
                        else if(nm=="UpdateParameters"){ static const int16 times[]{-1,-1,-1,0,1,100,-2,32767};
                            if(next()%2){ G<uint8>(0xB613FD)=uint8(next()%2); G<void*>(0xB61410)=peds[next()%2 ? 0 : 1].data(); G<void*>(0xB6140C)=peds[next()%2 ? 0 : 2].data();
                                G<int16>(0xB61408)=next()%2 ? At<int16>(self,0xA8) : int16(next()%6); G<int16>(0xB61404)=next()%2 ? At<int16>(self,0xA8) : int16(next()%6);
                                G<uint8>(0xB613FC)=uint8(next()%2); G<void*>(0xB61400)=peds[next()%2 ? 0 : 3].data(); }
                            if(next()%5==0){ auto* pd=static_cast<uint8*>(At<void*>(self,4)); if(next()%2)At<void*>(pd,0x47C)=nullptr; else At<void*>(intels[(P(pd)-0x10)%NPED].data(),0x24)=nullptr; }
                            const int16 t=next()%2 ? times[next()%8] : int16(next());
                            reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(a)(self,snds[next()%4].data(),t); }
                        else if(nm=="WillPedChatAboutTopic")ret=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,int32(next()%14)-2)&0xFF;
                        else ret="unhandled";
                    }
                    // ---------------- salida ----------------
                    json sp=json::array(); for(size_t k=0;k<NPED;++k){ uint8* s=Speech(k); out["vptr"+std::to_string(k)]=At<uint32>(s,0)==SPEECH_VT; sp.push_back(Hex(s+4,0x100-4)); }
                    out["speech"]=sp;
                    out["slots"]=Hex(reinterpret_cast<void*>(SLOTS-0x28),SLOTS_SIZE+0x28); out["statics"]=Hex(reinterpret_cast<void*>(STATICS),STATICS_SIZE);
                    out["phrases"]=Fnv(reinterpret_cast<void*>(PHRASES),PHRASES_SIZE); out["phrase_head"]=Hex(reinterpret_cast<void*>(PHRASES),16);
                    out["times"]=Fnv(reinterpret_cast<void*>(TIMES),TIMES_SIZE); out["last_mission"]=G<uint32>(0xA476A8);
                    restoreAll();
                    out["ret"]=ret; out["recs"]=Recs(); return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["ret"]!="unhandled";
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
    restoreAll();
    bool restored=true; for(auto& s:savedAll)restored&=Same(s); for(auto& s:savedFinal)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
