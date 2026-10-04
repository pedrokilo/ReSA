#pragma once
// Banco dentro del motor: CPlayerInfo (lista de 28, 24 funciones). Original (hooks de la clase desactivados) frente a
// candidato con fixtures deterministas e identicos. Los CPlayerInfo son buferes falsos de 0x190 bytes; peatones,
// vehiculos, matrices, CWanted, CPlayerPedData del peaton, mando y texturas son falsos con offsets crudos y una vtable
// trazada (BlowUpCar en el slot 41, el resto trampas). CWorld::Players y el jugador en foco solo se sustituyen por el
// fixture durante cada ejecucion y se restauran. Se simulan y trazan: CDarkel, CGangWars, CStats (Get/Increment/
// Decrement/DisplayScriptStatUpdateMessage/GetFatAndMuscleModifier), TheText.Get, CMessages::AddBigMessage,
// CHud::SetHelpMessage, CRemote, CGameLogic::IsCoopGameGoingOn, CStreaming (RequestModel/SetModelIsDeletable),
// TheCamera.CalculateGroundHeight, CPed (GiveWeapon/Say), CPlayerPed (GetPadFromPlayer/ClearAdrenaline/
// ClearWeaponTarget), CPedIntelligence::ClearTasks, CPad (StartShake/StopPadsShaking), CWorld (StopAllLawEnforcers/
// SetAllCarsCanBeDamaged/ExtinguishAllCarFiresInArea), gFireManager.ExtinguishPoint, CExplosion, CProjectileInfo,
// CReplay, referencias (CleanUpOldReference/RegisterReference), CPlayerSkin::GetSkinTexture, RwTextureDestroy y el
// bufer de guardado (nunca ficheros). Los cuerpos de la DLL se resuelven antes de parchear y nunca se parchea la
// direccion de un stub. La traza es un array fijo (sin reservas durante la llamada) que se convierte a json despues.
// Todas las globales tocadas (CWorld::Players, foco, CTimer, reloj, trucos, estaticas del hambre, mandos, menus,
// pantalla ancha, cinematica, CGameLogic, estado de carga del paracaidas) se guardan y se restauran en cada ejecucion
// y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PlayerInfo.h"
#include "World.h"
#include "Darkel.h"
#include "GangWars.h"
#include "Remote.h"
#include "Stats.h"
#include "Messages.h"
#include "Hud.h"
#include "Text/Text.h"
#include "GameLogic.h"
#include "Streaming.h"
#include "Camera.h"
#include "Pad.h"
#include "PedIntelligence.h"
#include "FireManager.h"
#include "Explosion.h"
#include "ProjectileInfo.h"
#include "Replay.h"
#include "Entity/Ped/PlayerSkin.h"
#include "GenericGameStorage.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace Player28Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T> inline T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline uint32 Hash32(const void* p,size_t n) { uint32 h=2166136261u; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=16777619u;} return h; }

// ======================= objetos falsos =======================
constexpr size_t INFOSZ=0x190, PEDSZ=0x800, VEHSZ=0x800, MATSZ=0x48, INTSZ=0x300, WSZ=0x300, PDSZ=0xAC, PADSZ=0x134, TEXSZ=0x60;
constexpr uintptr_t PLAYERS=0xB7CD98;
alignas(16) inline std::array<uint8,2*INFOSZ> infos{};
alignas(16) inline std::array<uint8,3*PEDSZ> peds{};
alignas(16) inline std::array<uint8,2*VEHSZ> vehs{};
alignas(16) inline std::array<uint8,5*MATSZ> mats{};     // 0-2 peatones, 3-4 vehiculos
alignas(16) inline std::array<uint8,3*INTSZ> intels{};
alignas(16) inline std::array<uint8,WSZ> wanted{};
alignas(16) inline std::array<uint8,3*PDSZ> pdatas{};
alignas(16) inline std::array<uint8,PADSZ> padObj{};
alignas(16) inline std::array<uint8,4*TEXSZ> texs{};
alignas(16) inline std::array<char,64> nameBuf{};
alignas(16) inline std::array<std::array<GxtChar,16>,3> texts{}; // DEAD, NOTEAT, otra
inline std::array<void*,64> vt{};
inline uint8* Info(int k) { return infos.data()+size_t(k)*INFOSZ; }
inline uint8* Ped(int k) { return peds.data()+size_t(k)*PEDSZ; }
inline uint8* Veh(int k) { return vehs.data()+size_t(k)*VEHSZ; }
inline uint8* Mat(int k) { return mats.data()+size_t(k)*MATSZ; }
inline uint8* Intel(int k) { return intels.data()+size_t(k)*INTSZ; }
inline uint8* PData(int k) { return pdatas.data()+size_t(k)*PDSZ; }
inline uint8* Tex(int k) { return texs.data()+size_t(k)*TEXSZ; }
inline uint8* Plr(int k) { return reinterpret_cast<uint8*>(PLAYERS)+size_t(k)*INFOSZ; }

struct Rg { uintptr_t p; size_t n; size_t stride; const char* nm; };
inline std::vector<Rg> NameRegions() {
    return {{uintptr_t(infos.data()),infos.size(),INFOSZ,"info"},{uintptr_t(peds.data()),peds.size(),PEDSZ,"ped"},{uintptr_t(vehs.data()),vehs.size(),VEHSZ,"veh"},
            {uintptr_t(mats.data()),mats.size(),MATSZ,"mat"},{uintptr_t(intels.data()),intels.size(),INTSZ,"intel"},{uintptr_t(wanted.data()),WSZ,0,"wanted"},
            {uintptr_t(pdatas.data()),pdatas.size(),PDSZ,"pdata"},{uintptr_t(padObj.data()),PADSZ,0,"pad"},{uintptr_t(texs.data()),texs.size(),TEXSZ,"tex"},
            {uintptr_t(nameBuf.data()),nameBuf.size(),0,"name"},{uintptr_t(texts.data()),sizeof(texts),16*sizeof(GxtChar),"text"},
            {PLAYERS,2*INFOSZ,INFOSZ,"player"},{0xB73458,2*PADSZ,PADSZ,"cpad"},{0xB6F028,0xD78,0,"camera"},{0xB71F80,0x1000,0,"firemgr"},{0xC1B340,4,0,"text_obj"}};
}
inline json Name(uint32 v) {
    if(!v)return nullptr;
    static const auto regions=NameRegions();
    for(auto& r:regions){ if(v<r.p||v>=r.p+r.n)continue; const auto o=v-r.p;
        if(r.stride)return std::string(r.nm)+"#"+std::to_string(o/r.stride)+"+"+std::to_string(o%r.stride);
        return std::string(r.nm)+"+"+std::to_string(o); }
    return v;
}

// ======================= traza fija (sin reservas durante la llamada) =======================
enum Code : uint32 {
    C_DARKEL, C_GANGWAR, C_STAT_GET, C_STAT_INC, C_STAT_DEC, C_STAT_MSG, C_FATMUSCLE, C_TEXT, C_BIGMSG, C_HELP, C_REMOTE, C_COOP,
    C_REQUEST, C_DELETABLE, C_GROUND, C_GIVEW, C_SAY, C_GETPAD, C_ADREN, C_TARGET, C_CLEARTASKS, C_SHAKE, C_STOPSHAKE,
    C_STOPLAW, C_CARSDMG, C_CARFIRES, C_EXTPOINT, C_EXPLOSIONS, C_PROJECTILES, C_REPLAY_OFF, C_REPLAY_ON, C_CLEANREF, C_REGREF,
    C_SKINTEX, C_TEXDESTROY, C_SAVE, C_LOAD, C_BLOWUP, C_TRAP };
inline constexpr const char* CODE_NAMES[]{
    "darkel_reset","end_gang_war","get_stat","inc_stat","dec_stat","stat_message","fat_muscle","text_get","big_message","help","take_remote","coop",
    "request_model","model_deletable","ground_height","give_weapon","say","get_pad","clear_adrenaline","clear_weapon_target","clear_tasks","start_shake","stop_pads_shaking",
    "stop_law","cars_can_be_damaged","extinguish_car_fires","extinguish_point","remove_explosions","remove_projectiles","disable_replays","enable_replays","clean_up_ref","register_ref",
    "skin_texture","texture_destroy","save_wb","load_wb","blow_up_car","trap"};
struct TR { uint32 c,mask,a,b,d,e,f,g; };   // mask: bit q = argumento q es un puntero
inline std::array<TR,256> tr{}; inline int ntr{}; inline bool trOverflow{};
inline void T(uint32 c,uint32 mask,uint32 a=0,uint32 b=0,uint32 d=0,uint32 e=0,uint32 f=0,uint32 g=0) { if(ntr<int(tr.size()))tr[ntr++]={c,mask,a,b,d,e,f,g}; else trOverflow=true; }
inline json TraceJson() {
    json r=json::array();
    for(int k=0;k<ntr;++k){ const auto& e=tr[k]; json x=json::array({CODE_NAMES[e.c]});
        const uint32 v[]{e.a,e.b,e.d,e.e,e.f,e.g};
        for(int q=0;q<6;++q)x.push_back((e.mask>>q)&1 ? Name(v[q]) : json(v[q]));
        r.push_back(x); }
    return r;
}

// ======================= resultados de los stubs =======================
inline float fatValue{}, muscleValue{}, otherStat{}, groundResult{};
inline bool coopResult{};
inline uint32 skinMask{}, nSkin{}, loadCounter{};
// guardado: bytes copiados por el stub (la estructura de 0x28 tiene relleno sin inicializar en +7, +0x11..0x13 y +0x25)
inline std::array<std::array<uint8,0x28>,4> saveBytes{}; inline std::array<int32,4> saveSizes{}; inline int nSave{};

// ======================= stubs =======================
inline void __cdecl FakeDarkel() { T(C_DARKEL,0); }
inline void __cdecl FakeEndGangWar(bool b) { T(C_GANGWAR,0,b); }
inline float __cdecl FakeGetStat(eStats s) { const float v=s==STAT_FAT ? fatValue : s==STAT_MUSCLE ? muscleValue : otherStat; T(C_STAT_GET,0,uint32(s),F(v)); return v; }
inline void __cdecl FakeIncStat(eStats s,float v) { T(C_STAT_INC,0,uint32(s),F(v)); }
inline void __cdecl FakeDecStat(eStats s,float v) { T(C_STAT_DEC,0,uint32(s),F(v)); }
inline void __cdecl FakeStatMsg(eStatUpdateState st,eStats s,float v) { T(C_STAT_MSG,0,uint32(st),uint32(s),F(v)); }
inline float __cdecl FakeFatMuscle(eStatModAbilities m) { const float v=1000.f+float(uint32(m))*3.5f; T(C_FATMUSCLE,0,uint32(m),F(v)); return v; }
inline const GxtChar* __fastcall FakeTextGet(void* self,void*,const char* key) {
    const uint32 k=!strcmp(key,"DEAD") ? 0 : !strcmp(key,"NOTEAT") ? 1 : 2; T(C_TEXT,1,U(self),k); return texts[k].data(); }
inline void __cdecl FakeBigMessage(const GxtChar* t,uint32 time,eMessageStyle st) { T(C_BIGMSG,1,U(t),time,uint32(st)); }
inline void __cdecl FakeHelp(const GxtChar* t,bool q,bool p,bool b) { T(C_HELP,1,U(t),q,p,b); }
inline void __cdecl FakeRemote(bool b) { T(C_REMOTE,0,b); }
inline bool __cdecl FakeCoop() { T(C_COOP,0,coopResult); return coopResult; }
inline void __cdecl FakeRequest(int32 m,int32 f) { T(C_REQUEST,0,uint32(m),uint32(f)); }
inline void __cdecl FakeDeletable(int32 m) { T(C_DELETABLE,0,uint32(m)); }   // el EXE solo empuja el modelo
inline float __fastcall FakeGround(void* self,void*,eGroundHeightType t) { T(C_GROUND,1,U(self),uint32(t),F(groundResult)); return groundResult; }
inline eWeaponSlot __fastcall FakeGiveWeapon(void* self,void*,eWeaponType t,uint32 ammo,bool like) { T(C_GIVEW,1,U(self),uint32(t),ammo,like); return eWeaponSlot(uint32(t)%13); }
inline int16 __fastcall FakeSay(void* self,void*,uint32 ctx,uint32 delay,float prob,bool a,bool b,bool c) { T(C_SAY,1,U(self),ctx,delay,F(prob),uint32(a)|b<<1|c<<2); return int16(-1); }
inline void* __fastcall FakeGetPad(void* self,void*) { T(C_GETPAD,1,U(self)); return padObj.data(); }
inline void __fastcall FakeAdrenaline(void* self,void*) { T(C_ADREN,1,U(self)); }
inline void __fastcall FakeClearTarget(void* self,void*) { T(C_TARGET,1,U(self)); }
inline void __fastcall FakeClearTasks(void* self,void*,bool a,bool b) { T(C_CLEARTASKS,1,U(self),a,b); }
inline void __fastcall FakeShake(void* self,void*,int16 time,uint8 freq,uint32 a) { T(C_SHAKE,1,U(self),uint32(time),freq,a); }
inline void __cdecl FakeStopShaking() { T(C_STOPSHAKE,0); }
inline void __cdecl FakeStopLaw() { T(C_STOPLAW,0); }
inline void __cdecl FakeCarsDamage(bool b) { T(C_CARSDMG,0,b); }
inline void __cdecl FakeCarFires(CVector p,float r) { T(C_CARFIRES,0,F(p.x),F(p.y),F(p.z),F(r)); }
inline void __fastcall FakeExtPoint(void* self,void*,CVector p,float r) { T(C_EXTPOINT,1,U(self),F(p.x),F(p.y),F(p.z),F(r)); }
inline void __cdecl FakeExplosions(CVector p,float r) { T(C_EXPLOSIONS,0,F(p.x),F(p.y),F(p.z),F(r)); }
inline void __cdecl FakeProjectiles() { T(C_PROJECTILES,0); }
inline void __cdecl FakeReplayOff() { T(C_REPLAY_OFF,0); }
inline void __cdecl FakeReplayOn() { T(C_REPLAY_ON,0); }
inline void __fastcall FakeCleanUpRef(void* self,void*,CEntity** ref) { T(C_CLEANREF,3,U(self),U(ref)); }
inline void __fastcall FakeRegisterRef(void* self,void*,CEntity** ref) { T(C_REGREF,3,U(self),U(ref)); }
// lee exactamente los 32 bytes del nombre (strncpy puede dejarlo sin terminador)
inline RwTexture* __cdecl FakeSkinTexture(const char* name) {
    const uint32 n=nSkin++; RwTexture* r=(skinMask>>(n%32))&1 ? reinterpret_cast<RwTexture*>(Tex(int(1+n%3))) : nullptr;
    T(C_SKINTEX,5,U(name),Hash32(name,32),U(r)); return r; }
inline RwBool __cdecl FakeTexDestroy(RwTexture* t) { T(C_TEXDESTROY,1,U(t)); return TRUE; }
inline bool __cdecl FakeSaveWB(void* p,int32 n) {
    if(nSave<int(saveBytes.size())){ auto& b=saveBytes[nSave]; b.fill(0); saveSizes[nSave]=n; if(n>0)std::memcpy(b.data(),p,std::min<size_t>(size_t(n),b.size()));
        if(n==0x28){ b[7]=0; b[0x11]=b[0x12]=b[0x13]=0; b[0x25]=0; } ++nSave; }
    T(C_SAVE,0,uint32(n)); return true; }
inline bool __cdecl FakeLoadWB(void* p,int32 n) { auto* b=static_cast<uint8*>(p); for(int32 k=0;k<n;++k)b[k]=uint8(loadCounter++*37u+11u); T(C_LOAD,0,uint32(n)); return true; }
inline void __fastcall FakeBlowUp(void* self,void*,CEntity* damager,bool hide) { T(C_BLOWUP,3,U(self),U(damager),hide); }
template<int N> void __fastcall Trap(void* self,void*) { T(C_TRAP,1,U(self),uint32(N)); }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&Trap<N>)),...); }

// ======================= hooks y parches =======================
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }
// cuerpo de una funcion de la DLL: sigue los saltos del ILT mientras no vuelvan al EXE
inline uintptr_t Body(uintptr_t a) {
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
template<class M> inline uintptr_t MAddr(M m) { static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,24> fns{{
    {"CPlayerInfo","AddHealth",0x56EAB0,0,-1,1024},{"CPlayerInfo","ArrestPlayer",0x56E5D0,0,-1,128},
    {"CPlayerInfo","BlowUpRCBuggy",0x56EA30,0,-1,256},{"CPlayerInfo","Clear",0x56F330,0,-1,256},
    {"CPlayerInfo","Constructor",0x571920,0,-1,64},{"CPlayerInfo","DeletePlayerSkin",0x56EA80,0,-1,64},
    {"CPlayerInfo","EvaluateCarPosition",0x56DAD0,0,-1,1024},{"CPlayerInfo","GetPos_Hook",0x56DFB0,0,-1,256},
    {"CPlayerInfo","GetSpeed_Hook",0x56DF50,0,-1,256},{"CPlayerInfo","GivePlayerParachute",0x56EC40,0,-1,256},
    {"CPlayerInfo","IsPlayerInRemoteMode",0x56DAB0,0,-1,128},{"CPlayerInfo","IsRestartingAfterArrest",0x56E560,0,-1,64},
    {"CPlayerInfo","IsRestartingAfterDeath",0x56E550,0,-1,64},{"CPlayerInfo","IsRestartingAfterMissionFailed",0x56E570,0,-1,64},
    {"CPlayerInfo","KillPlayer",0x56E580,0,-1,128},{"CPlayerInfo","Load",0x5D3B00,0,-1,128},
    {"CPlayerInfo","LoadPlayerSkin",0x56F7D0,0,-1,64},{"CPlayerInfo","MakePlayerSafe",0x56E870,0,-1,512},
    {"CPlayerInfo","PlayerFailedCriticalMission",0x56E830,0,-1,128},{"CPlayerInfo","Save",0x5D3AC0,0,-1,128},
    {"CPlayerInfo","SetLastTargetVehicle",0x56DA80,0,-1,256},{"CPlayerInfo","SetPlayerSkin",0x5717F0,0,-1,256},
    {"CPlayerInfo","StreamParachuteWeapon",0x56EB30,0,-1,1024},{"CPlayerInfo","WorkOutEnergyFromHunger",0x56E610,0,-1,1024},
}};

struct Region { uintptr_t at; size_t n; std::vector<uint8> saved; };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"player28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"player28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("player28");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"player28-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,24> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    FillTraps(std::make_integer_sequence<int,64>{}); vt[41]=reinterpret_cast<void*>(&FakeBlowUp); // 0xA4 / 4 = BlowUpCar
    static const char* const textKeys[3]{"DEAD","NOTEAT","OTHER"};
    for(size_t k=0;k<texts.size();++k){ texts[k].fill(0); for(size_t c=0;textKeys[k][c];++c)texts[k][c]=GxtChar(textKeys[k][c]); }

    // estado global guardado
    std::vector<Region> regions{{PLAYERS,2*INFOSZ,{}},{0xB7CD74,1,{}},{0xB7CB84,4,{}},{0xB7CB5C,4,{}},{0xB70153,1,{}},{0x969174,1,{}},{0xB9B8F0,8,{}},
        {0xB73458,2*PADSZ,{}},{0xBA82E3,1,{}},{0xB6F065,1,{}},{0xB5F851,1,{}},{0x96A8AC,8,{}},{0x8E69BC,0x14,{}}};
    for(auto& r:regions)r.saved.assign(reinterpret_cast<uint8*>(r.at),reinterpret_cast<uint8*>(r.at)+r.n);
    auto RestoreAll=[&]{ for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.at),r.saved.data(),r.n); };
    auto AllRestored=[&]{ for(auto& r:regions)if(std::memcmp(reinterpret_cast<void*>(r.at),r.saved.data(),r.n))return false; return true; };

    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,auto stub){ void* s=reinterpret_cast<void*>(stub); if(exeAddr)plan.push_back({exeAddr,s}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,s}); } };
    add(0x43DC10,uintptr_t(&CDarkel::ResetOnPlayerDeath),&FakeDarkel);
    add(0x4464C0,uintptr_t(&CGangWars::EndGangWar),&FakeEndGangWar);
    add(0x558E40,uintptr_t(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),&FakeGetStat);
    add(0x55C180,uintptr_t(&CStats::IncrementStat),&FakeIncStat);
    add(0x559FA0,uintptr_t(&CStats::DecrementStat),&FakeDecStat);
    add(0x55B980,uintptr_t(&CStats::DisplayScriptStatUpdateMessage),&FakeStatMsg);
    add(0x559AF0,uintptr_t(&CStats::GetFatAndMuscleModifier),&FakeFatMuscle);
    add(0x6A0050,MAddr(&CText::Get),&FakeTextGet);
    add(0x69F2B0,uintptr_t(&CMessages::AddBigMessage),&FakeBigMessage);
    add(0x588BE0,uintptr_t(&CHud::SetHelpMessage),&FakeHelp);
    add(0x45AE80,uintptr_t(&CRemote::TakeRemoteControlledCarFromPlayer),&FakeRemote);
    add(0x441390,uintptr_t(&CGameLogic::IsCoopGameGoingOn),&FakeCoop);
    add(0x4087E0,uintptr_t(&CStreaming::RequestModel),&FakeRequest);
    add(0x409C10,uintptr_t(&CStreaming::SetModelIsDeletable),&FakeDeletable);
    add(0x514B80,MAddr(&CCamera::CalculateGroundHeight),&FakeGround);
    add(0x5E6080,MAddr(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),&FakeGiveWeapon);
    add(0x5EFFE0,MAddr(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),&FakeSay);
    add(0x609560,MAddr(&CPlayerPed::GetPadFromPlayer),&FakeGetPad);
    add(0x60A070,MAddr(&CPlayerPed::ClearAdrenaline),&FakeAdrenaline);
    add(0x609C80,MAddr(&CPlayerPed::ClearWeaponTarget),&FakeClearTarget);
    add(0x601420,MAddr(&CPedIntelligence::ClearTasks),&FakeClearTasks);
    add(0x53F920,MAddr(&CPad::StartShake),&FakeShake);
    add(0x541D70,uintptr_t(&CPad::StopPadsShaking),&FakeStopShaking);
    add(0x566C10,uintptr_t(&CWorld::StopAllLawEnforcersInTheirTracks),&FakeStopLaw);
    add(0x5668F0,uintptr_t(&CWorld::SetAllCarsCanBeDamaged),&FakeCarsDamage);
    add(0x566950,uintptr_t(&CWorld::ExtinguishAllCarFiresInArea),&FakeCarFires);
    add(0x539450,MAddr(&CFireManager::ExtinguishPoint),&FakeExtPoint);
    add(0x7369E0,uintptr_t(&CExplosion::RemoveAllExplosionsInArea),&FakeExplosions);
    add(0x7399B0,uintptr_t(&CProjectileInfo::RemoveAllProjectiles),&FakeProjectiles);
    add(0x45B150,uintptr_t(&CReplay::DisableReplays),&FakeReplayOff);
    add(0x45B160,uintptr_t(&CReplay::EnableReplays),&FakeReplayOn);
    add(0x571A00,MAddr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),&FakeCleanUpRef);
    add(0x571B70,MAddr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),&FakeRegisterRef);
    add(0x6FFA10,uintptr_t(&CPlayerSkin::GetSkinTexture),&FakeSkinTexture);
    add(0x7F3820,uintptr_t(&RwTextureDestroy),&FakeTexDestroy);
    add(0x5D1270,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),&FakeSaveWB);
    add(0x5D1300,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),&FakeLoadWB);
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }

    std::array<bool,24> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        auto& focus=*reinterpret_cast<uint8*>(0xB7CD74);
        const float qnan=std::numeric_limits<float>::quiet_NaN();

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto rnd=[&](uint32 n){ return next()%n; };
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A28u; if(!st)st=1;
                    ntr=0; trOverflow=false; nSkin=0; loadCounter=0; nSave=0; for(auto& b:saveBytes)b.fill(0); saveSizes.fill(-1);
                    skinMask=next(); coopResult=rnd(6)==0; groundResult=rnd(32)==0 ? qnan : uni(-20.f,120.f);
                    fatValue=rnd(3)==0 ? 0.f : rnd(16)==0 ? qnan : uni(-5.f,500.f); muscleValue=rnd(3)==0 ? 0.f : rnd(16)==0 ? qnan : uni(-5.f,500.f); otherStat=uni(0.f,10.f);

                    // ---- fixtures ----
                    auto randMat=[&](uint8* m,const CVector& pos){ std::memset(m,0,MATSZ); const float a=uni(-3.2f,3.2f), c=std::cos(a), s=std::sin(a);
                        At<CVector>(m,0x00)=CVector{c,s,0.f}; At<CVector>(m,0x10)=CVector{-s,c,rnd(4)==0 ? uni(-0.3f,0.3f) : 0.f}; At<CVector>(m,0x20)=CVector{0.f,0.f,1.f}; At<CVector>(m,0x30)=pos; };
                    auto randPos=[&]{ return CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-10.f,200.f)}; };
                    auto setVeh=[&](int k){
                        uint8* v=Veh(k); std::memset(v,0,VEHSZ); At<void*>(v,0)=vt.data();
                        const CVector pos=randPos(); At<CVector>(v,4)=pos;
                        if(rnd(3)){ randMat(Mat(3+k),pos+CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}); At<void*>(v,0x14)=Mat(3+k); } else std::memset(Mat(3+k),0,MATSZ);
                        for(int q=0x1C;q<0x20;++q)v[q]=uint8(next()); for(int q=0x40;q<0x44;++q)v[q]=uint8(next());
                        for(int q=0;q<3;++q)At<float>(v,0x44+4*q)=uni(-2.f,2.f);
                        static const uint32 types[]{0,3,4,5,9,3,4,2};
                        At<uint32>(v,0x590)=types[rnd(8)]; At<uint32>(v,0x594)=rnd(6)==0 ? rnd(12) : types[rnd(8)];
                    };
                    auto setPed=[&](int k){
                        uint8* p=Ped(k); std::memset(p,0,PEDSZ); At<void*>(p,0)=vt.data();
                        const CVector pos=randPos(); At<CVector>(p,4)=pos; At<float>(p,0x10)=uni(-3.2f,3.2f);
                        if(rnd(3)){ randMat(Mat(k),pos+CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f}); At<void*>(p,0x14)=Mat(k); } else std::memset(Mat(k),0,MATSZ);
                        for(int q=0x1C;q<0x20;++q)p[q]=uint8(next()); for(int q=0x40;q<0x44;++q)p[q]=uint8(next());
                        for(int q=0;q<3;++q)At<float>(p,0x44+4*q)=rnd(4)==0 ? 0.f : uni(-2.f,2.f);
                        At<void*>(p,0xFC)=rnd(8)==0 ? Ped(2) : nullptr;
                        for(int q=0x46C;q<0x47C;++q)p[q]=uint8(next());
                        At<void*>(p,0x47C)=Intel(k); std::memset(Intel(k),0,INTSZ);
                        At<void*>(p,0x480)=PData(k); for(size_t q=0;q<PDSZ;++q)PData(k)[q]=uint8(next());
                        At<float>(p,0x540)=rnd(5)==0 ? 0.f : uni(-50.f,200.f);
                        At<void*>(p,0x58C)=rnd(3) ? Veh(k%2) : nullptr;
                        At<uint32>(p,0x70C)=rnd(47);
                    };
                    auto setInfo=[&](int k){ uint8* in=Info(k); for(size_t q=0;q<INFOSZ;++q)in[q]=uint8(next());
                        At<void*>(in,0)=Ped(k); At<void*>(in,4)=wanted.data(); At<void*>(in,0xB0)=nullptr; At<void*>(in,0xB4)=nullptr; At<void*>(in,0xD8)=nullptr; At<void*>(in,0x184)=nullptr; };
                    auto setWorld=[&]{
                        for(int k=0;k<3;++k)setPed(k); setVeh(0); setVeh(1); setInfo(0); setInfo(1);
                        for(auto& b:wanted)b=uint8(next()); for(auto& b:padObj)b=uint8(next()); for(auto& b:texs)b=uint8(next());
                        // CWorld::Players del fixture
                        std::memset(Plr(0),0,2*INFOSZ);
                        for(int k=0;k<2;++k){ At<void*>(Plr(k),0)=rnd(4) ? Ped(k) : nullptr; At<uint16>(Plr(k),0x144)=uint16(next()); }
                        focus=uint8(rnd(2));
                        // tiempo, reloj, mandos y banderas
                        *reinterpret_cast<uint32*>(0xB7CB84)=1000000+rnd(200000);
                        *reinterpret_cast<float*>(0xB7CB5C)=rnd(4)==0 ? 1.f : uni(0.f,3.f);
                        *reinterpret_cast<uint8*>(0xB70153)=uint8(rnd(24));
                        *reinterpret_cast<uint8*>(0x969174)=uint8(rnd(10)==0);
                        *reinterpret_cast<uint8*>(0xBA82E3)=uint8(rnd(10)==0 ? 1+rnd(2) : 0);
                        *reinterpret_cast<uint8*>(0xB6F065)=uint8(rnd(10)==0);
                        *reinterpret_cast<uint8*>(0xB5F851)=uint8(rnd(10)==0);
                        At<uint16>(reinterpret_cast<void*>(0xB73458),0x10E)=uint16(rnd(8)==0 ? 1u<<rnd(16) : 0);
                        *reinterpret_cast<uint8*>(0x96A8B0)=uint8(rnd(5)); *reinterpret_cast<int32*>(0x96A8AC)=int32(next());
                        *reinterpret_cast<uint8*>(0x8E69CC)=uint8(rnd(3)==0 ? rnd(5) : 1);
                    };

                    setWorld();
                    uint8* const info=Info(0);
                    void* const A=reinterpret_cast<void*>(fn.address);
                    json ret=nullptr; bool switches=false;
                    auto on=[&]{ for(auto* h:hooks)h->State(candidate); switches=ReversingProbe::JumpsToDll(fn.address)==candidate; };
                    auto off=[&]{ for(auto* h:hooks)h->State(false); };
                    json extra=json::object();

                    if(nm=="AddHealth"){
                        static const int32 am[]{0,1,-1,10,25,50,100,150,200,255,-100,16777217,-16777217,33554435,2147483647,INT32_MIN};
                        int32 amount=rnd(3)==0 ? am[rnd(16)] : int32(rnd(400))-100; if(rnd(16)==0)amount=int32(next());
                        info[0x14F]=uint8(rnd(4)==0 ? 100 : next());
                        float h=rnd(4)==0 ? float(info[0x14F]) : uni(-50.f,300.f);
                        if(rnd(16)==0)h=uni(-1.e8f,-1.e6f)+0.5f; if(rnd(16)==0)h=uni(-3.e7f,-1.6e7f); if(rnd(32)==0)h=qnan; if(rnd(32)==0)h=uni(0.f,1.f);
                        At<float>(Ped(0),0x540)=h;
                        on(); reinterpret_cast<void(__thiscall*)(void*,int32)>(A)(info,amount); off();
                        extra["health"]=F(At<float>(Ped(0),0x540)); }
                    else if(nm=="ArrestPlayer"||nm=="KillPlayer"||nm=="PlayerFailedCriticalMission"){
                        info[0xDC]=uint8(rnd(3) ? 0 : rnd(4)==0 ? next() : rnd(5));
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(info); off(); }
                    else if(nm=="BlowUpRCBuggy"){
                        At<void*>(info,0xB0)=rnd(4) ? Veh(int(rnd(2))) : nullptr; const bool b=rnd(2)!=0;
                        if(rnd(3)==0)Veh(0)[0x1D]&=~0x08; if(rnd(3)==0)Veh(1)[0x1D]&=~0x08;
                        on(); reinterpret_cast<void(__thiscall*)(void*,bool)>(A)(info,b); off(); }
                    else if(nm=="Clear"){
                        At<void*>(info,0xB4)=rnd(3) ? Veh(int(rnd(2))) : nullptr; info[0x188]=uint8(rnd(3)==0 ? 0 : rnd(4)==0 ? 2 : 1);
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(info); off(); }
                    else if(nm=="Constructor"){
                        std::memset(info,0xCD,INFOSZ);
                        on(); ret=Name(U(reinterpret_cast<void*(__thiscall*)(void*)>(A)(info))); off(); }
                    else if(nm=="DeletePlayerSkin"||nm=="LoadPlayerSkin"){
                        At<void*>(info,0x184)=rnd(3) ? Tex(0) : nullptr;
                        if(rnd(2)){ const uint32 n=rnd(40); for(uint32 q=0;q<32;++q)info[0x164+q]=q<n ? char('a'+rnd(26)) : 0; }
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(info); off(); }
                    else if(nm=="EvaluateCarPosition"){
                        uint8* ped=Ped(0); if(!At<void*>(ped,0x14)){ randMat(Mat(0),At<CVector>(ped,4)); At<void*>(ped,0x14)=Mat(0); }
                        const bool useVeh=rnd(4)!=0; uint8* car=useVeh ? Veh(0) : Ped(1);
                        const CVector pp=At<CVector>(Mat(0),0x30);
                        CVector cp=pp+CVector{uni(-15.f,15.f),uni(-15.f,15.f),uni(-3.f,3.f)};
                        if(rnd(16)==0)cp=pp;
                        if(rnd(6)==0){ // justo detras: el angulo relativo ronda +-PI
                            const CVector fw=At<CVector>(Mat(0),0x10); const float d=uni(0.5f,9.f); cp=pp-CVector{fw.x*d,fw.y*d,0.f}+CVector{uni(-0.01f,0.01f),uni(-0.01f,0.01f),0.f}; }
                        At<CVector>(car,4)=cp; if(At<void*>(car,0x14))At<CVector>(At<uint8*>(car,0x14),0x30)=cp;
                        if(rnd(4)==0)At<void*>(car,0x14)=nullptr;
                        const float dist=rnd(8)==0 ? 10.f : rnd(16)==0 ? 0.f : uni(-2.f,12.f);
                        float outDist=rnd(4)==0 ? -1000.f : uni(-5.f,10.f); if(rnd(32)==0)outDist=qnan;
                        CVehicle* outVeh=reinterpret_cast<CVehicle*>(Veh(1));
                        on(); reinterpret_cast<void(__thiscall*)(void*,CEntity*,CPed*,float,float*,CVehicle**)>(A)(info,reinterpret_cast<CEntity*>(car),reinterpret_cast<CPed*>(ped),dist,&outDist,&outVeh); off();
                        extra["out"]=json::array({F(outDist),Name(U(outVeh))}); }
                    else if(nm=="GetPos_Hook"||nm=="GetSpeed_Hook"){
                        CVector out{-77.f,-77.f,-77.f};
                        on(); ret=Name(U(reinterpret_cast<CVector*(__thiscall*)(void*,CVector*)>(A)(info,&out))); off();
                        extra["out"]=json::array({F(out.x),F(out.y),F(out.z)}); }
                    else if(nm=="GivePlayerParachute"){
                        At<uint32>(info,0x18C)=rnd(3)==0 ? 0 : next();
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(info); off(); }
                    else if(nm=="IsPlayerInRemoteMode"||nm=="IsRestartingAfterArrest"||nm=="IsRestartingAfterDeath"||nm=="IsRestartingAfterMissionFailed"){
                        At<void*>(info,0xB0)=rnd(3)==0 ? Veh(0) : nullptr; info[0xDD]=uint8(rnd(2)==0 ? 0 : rnd(3)==0 ? next() : 1);
                        info[0xDC]=uint8(rnd(4)==0 ? next() : rnd(5));
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*)>(A)(info); off(); }
                    else if(nm=="Load"||nm=="Save"){
                        loadCounter=rnd(1000);
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*)>(A)(info); off(); }
                    else if(nm=="MakePlayerSafe"){
                        const float tcr[]{0.f,-1.f,-0.f,5000.f,uni(-3000.f,3000.f),qnan}; At<float>(info,0x1C)=tcr[rnd(6)];
                        const bool en=rnd(2)!=0; const float radius=rnd(8)==0 ? 0.f : uni(0.f,60.f);
                        on(); reinterpret_cast<void(__thiscall*)(void*,bool,float)>(A)(info,en,radius); off();
                        extra["flags"]=json::array({At<uint32>(Ped(0),0x40),wanted[0x1E],At<uint16>(padObj.data(),0x10E),At<uint32>(PData(0),0x34),F(At<float>(info,0x1C))}); }
                    else if(nm=="SetLastTargetVehicle"){
                        At<void*>(info,0xD8)=rnd(3) ? Veh(int(rnd(2))) : nullptr; void* v=rnd(3) ? Veh(int(rnd(2))) : nullptr;
                        on(); reinterpret_cast<void(__thiscall*)(void*,CVehicle*)>(A)(info,static_cast<CVehicle*>(v)); off(); }
                    else if(nm=="SetPlayerSkin"){
                        At<void*>(info,0x184)=rnd(3) ? Tex(0) : nullptr;
                        const uint32 n=rnd(4)==0 ? 28+rnd(12) : rnd(32); for(uint32 q=0;q<nameBuf.size();++q)nameBuf[q]=q<n ? char('a'+rnd(26)) : 0;
                        on(); reinterpret_cast<void(__thiscall*)(void*,const char*)>(A)(info,nameBuf.data()); off(); }
                    else if(nm=="StreamParachuteWeapon"){
                        if(rnd(8)==0)At<void*>(info,0)=nullptr;
                        uint8* ped=Ped(0); if(rnd(4))ped[0x46D]|=0x01; if(rnd(6)==0)At<void*>(ped,0x58C)=nullptr; else At<void*>(ped,0x58C)=Veh(0);
                        static const uint32 sub[]{3,4,3,4,0,5,9,2}; At<uint32>(Veh(0),0x594)=sub[rnd(8)];
                        { const float z=rnd(6)==0 ? groundResult+50.f : groundResult+uni(-10.f,110.f); At<float>(Veh(0),4+8)=z; if(At<void*>(Veh(0),0x14))At<float>(At<uint8*>(Veh(0),0x14),0x38)=z; }
                        static const uint32 tm[]{0,0,1,20,33,34,5000,4980}; At<uint32>(info,0x18C)=rnd(6)==0 ? next() : rnd(4)==0 ? rnd(200) : tm[rnd(8)];
                        info[0x188]=uint8(rnd(2));
                        const bool arg=rnd(4)!=0;
                        on(); reinterpret_cast<void(__thiscall*)(void*,bool)>(A)(info,arg); off(); }
                    else if(nm=="WorkOutEnergyFromHunger"){
                        auto* sv=reinterpret_cast<uint8*>(0xB9B8F0);
                        const uint8 hour=*reinterpret_cast<uint8*>(0xB70153);
                        sv[0]=uint8(rnd(2)); sv[1]=uint8(rnd(3)==0 ? 0 : rnd(4)==0 ? next() : rnd(100)); sv[2]=uint8(rnd(3)==0 ? hour : rnd(2) ? (hour+23)%24 : rnd(24)); sv[3]=uint8(next());
                        At<uint32>(sv,4)=rnd(8)==0 ? 0 : rnd(8)==0 ? (next()&~1u) : (next()|1u);
                        static const uint16 hrs[]{0,1,24,47,48,49,50,72,100,0x7FFF,0x8000,0xFFFF};
                        At<uint16>(info,0x144)=rnd(3)==0 ? uint16(rnd(120)) : rnd(8)==0 ? uint16(next()) : hrs[rnd(12)];
                        if(rnd(8)!=0){ *reinterpret_cast<uint8*>(0x969174)=0; *reinterpret_cast<uint8*>(0xBA82E3)=0; *reinterpret_cast<uint8*>(0xB6F065)=0; *reinterpret_cast<uint8*>(0xB5F851)=0;
                            At<uint16>(reinterpret_cast<void*>(0xB73458),0x10E)=0; coopResult=false; At<void*>(Ped(0),0xFC)=nullptr; }
                        if(rnd(10)==0)At<void*>(info,0xB0)=Veh(1); if(rnd(16)==0)At<void*>(info,0)=nullptr;
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(info); off();
                        extra["statics"]=ReversingProbe::Bytes(sv,8); extra["health"]=F(At<float>(Ped(0),0x540)); }

                    // ---- volcado ----
                    json saves=json::array(); for(int k=0;k<nSave;++k)saves.push_back(json::array({saveSizes[k],ReversingProbe::Bytes(saveBytes[k].data(),saveBytes[k].size())}));
                    json r={{"ret",ret},{"switches",switches},{"extra",extra},{"trace",TraceJson()},{"overflow",trOverflow},{"saves",saves},
                            {"info",ReversingProbe::Bytes(Info(0),INFOSZ)},{"info1",Hash(Info(1),INFOSZ)},
                            {"peds",json::array({Hash(Ped(0),PEDSZ),Hash(Ped(1),PEDSZ),Hash(Ped(2),PEDSZ)})},{"vehs",Hash(vehs.data(),vehs.size())},{"mats",Hash(mats.data(),mats.size())},
                            {"objs",Hash(wanted.data(),WSZ)+Hash(pdatas.data(),pdatas.size())+Hash(padObj.data(),PADSZ)+Hash(texs.data(),texs.size())+Hash(intels.data(),intels.size())},
                            {"players",Hash(Plr(0),2*INFOSZ)},{"focus",focus},
                            {"globals",json::array({*reinterpret_cast<uint8*>(0x96A8B0),*reinterpret_cast<int32*>(0x96A8AC),ReversingProbe::Bytes(reinterpret_cast<void*>(0xB9B8F0),8),
                                At<uint16>(reinterpret_cast<void*>(0xB73458),0x10E),*reinterpret_cast<uint8*>(0x8E69CC),Hash(reinterpret_cast<void*>(0xB73458),2*PADSZ)})}};
                    RestoreAll();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["overflow"]==false;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",AllRestored()}});
}
}
