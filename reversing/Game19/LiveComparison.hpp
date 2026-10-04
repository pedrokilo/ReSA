#pragma once
// Banco dentro del motor: CDarkel (frenesís de muertes) y CTagManager (graffitis), lista de 19 (38 funciones).
// Original (hooks del banco desactivados) frente a candidato con fixtures deterministas. Se simulan y trazan:
// FindPlayerPed/FindPlayerVehicle, estadísticas, audio de interfaz, mensajes y fuentes del HUD, textos (TheText.Get),
// AsciiToGxtChar, GetYPosBasedOnHealth, la lógica de juego (ClearSkip, partida cooperativa), las armas del jugador
// (GiveWeapon, SetCurrentWeapon, MakeChangesForNewWeapon, RemoveWeaponModel/Anims, ClearWeaponTarget, SetWantedLevel),
// las referencias de model infos, IsMember de grupos, apariencia/policía de vehículos, guerras de bandas, PopCycle,
// valores de usuario de atómicos, TriggerMessage y el búfer de guardado (nunca se tocan ficheros reales).
// Peatones, vehículos, entidades, atómicos, geometrías, materiales y model infos son falsos (offsets crudos).
// El estado global de CDarkel/CTagManager y todo lo demás tocado se guarda y se restaura en cada ejecución y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Darkel.h"
#include "TagManager.h"
#include "Hud.h"
#include "Font.h"
#include "Messages.h"
#include "Stats.h"
#include "GangWars.h"
#include "PopCycle.h"
#include "Garages.h"
#include "GameLogic.h"
#include "GenericGameStorage.h"
#include "VisibilityPlugins.h"
#include "PedGroupMembership.h"
#include "GxtChar.h"
#include "../ProbeRuntime.hpp"

namespace Game19Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- objetos falsos ---
alignas(16) inline std::array<std::array<uint8,0x800>,3> peds{};      // 0 y 1: jugadores, 2: víctima
alignas(16) inline std::array<std::array<uint8,0x100>,2> pdata{};     // CPlayerPedData de los jugadores
alignas(16) inline std::array<std::array<uint8,0x800>,2> vehs{};
alignas(16) inline std::array<std::array<uint8,0x40>,24> ents{};      // entidades de graffiti
alignas(16) inline std::array<std::array<uint8,0x48>,24> mats{};      // CMatrix
alignas(16) inline std::array<std::array<uint8,0x80>,24> atoms{};     // RpAtomic
alignas(16) inline std::array<std::array<uint8,0x60>,4> geos{};       // RpGeometry
alignas(16) inline std::array<std::array<void*,2>,4> matLists{};
alignas(16) inline std::array<std::array<uint8,0x20>,8> materials{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> mis{};        // model infos falsos
alignas(16) inline std::array<std::array<GxtChar,16>,4> texts{};      // 0: PAGE_00, 1: KILLPA, 2: otra clave, 3: mensaje propio
inline std::array<uint16,24> userValues{};

struct Range { const void* base; size_t stride, count; const char* tag; };
inline json Name(const void* p) {
    if(!p)return nullptr; const auto u=uintptr_t(p);
    const Range ranges[]{{peds.data(),0x800,3,"ped"},{pdata.data(),0x100,2,"pdata"},{vehs.data(),0x800,2,"veh"},{ents.data(),0x40,24,"ent"},{mats.data(),0x48,24,"mat"},
                         {atoms.data(),0x80,24,"atomic"},{geos.data(),0x60,4,"geo"},{materials.data(),0x20,8,"material"},{mis.data(),0x40,4,"mi"},{texts.data(),16,4,"text"},
                         {reinterpret_cast<void*>(0xA9A8B8),8,152,"tagdesc"},{reinterpret_cast<void*>(0xC09920),0x2D4,8,"group"}};
    for(auto& r:ranges){ const auto b=uintptr_t(r.base); if(u>=b&&u<b+r.stride*r.count)return json::array({r.tag,(u-b)/r.stride,(u-b)%r.stride}); }
    if(p==gGxtString)return "gxt"; if(p==gString)return "gstring";
    return json::array({"raw",u});
}

// --- fuente de bits para los stubs ---
inline uint32 mask{}, maskCalls{};
inline bool Bit() { return (mask>>(maskCalls++%32))&1; }

// --- jugador ---
inline uint32 playersPresent{}; inline void* findResult{}; inline void* vehResult{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ trace.push_back({"find_player_ped",id}); void* r=id<0 ? findResult : (id<2&&((playersPresent>>id)&1)) ? peds[id].data() : nullptr; return static_cast<CPlayerPed*>(r); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote){ trace.push_back({"find_player_vehicle",id,remote}); return static_cast<CVehicle*>(vehResult); }

// --- estadísticas, audio, bandas, población ---
inline void __cdecl FakeInc(eStats s,float v){ trace.push_back({"inc",int(s),F(v)}); }
inline void __cdecl FakeDec(eStats s,float v){ trace.push_back({"dec",int(s),F(v)}); }
inline void __cdecl FakeDisplay(eStatUpdateState st,eStats s,float v){ trace.push_back({"display",int(st),int(s),F(v)}); }
inline void __fastcall FakeFrontend(void* self,void*,eAudioEvents ev,float v,float s){ trace.push_back({"frontend",uintptr_t(self)==0xB6BC90,int(ev),F(v),F(s)}); }
inline bool __fastcall FakeIsMember(const void* self,void*,const CPed* ped){ const bool r=Bit(); trace.push_back({"is_member",Name(self),Name(ped),r}); return r; }
inline void __cdecl FakeProvocation(ePedType t){ trace.push_back({"provocation",int(t)}); }
inline void __cdecl FakeDealer(){ trace.push_back({"dealer"}); }
inline void __cdecl FakeHelp(const GxtChar* t,bool q,bool p,bool b){ trace.push_back({"help",Name(t),q,p,b}); }

// --- lógica de juego y textos ---
inline void __cdecl FakeClearSkip(bool a){ trace.push_back({"clear_skip",a}); }
inline bool __cdecl FakeCoop(){ const bool r=Bit(); trace.push_back({"coop",r}); return r; }
inline void __cdecl FakeWeaponStuff(bool a){ trace.push_back({"weapon_stuff_2p",a}); }
inline const GxtChar* __fastcall FakeTextGet(void* self,void*,const char* key){
    const std::string k=key; trace.push_back({"text_get",uintptr_t(self)==0xC1B340,k});
    return texts[k=="PAGE_00" ? 0 : k=="KILLPA" ? 1 : 2].data(); }
inline void __cdecl FakeBigMessage(const GxtChar* t,uint32 time,eMessageStyle st){ trace.push_back({"big_message",Name(t),time,int(st)}); }

// --- fuentes y HUD ---
inline void __cdecl FakeProportional(bool on){ trace.push_back({"font_proportional",on}); }
inline void __cdecl FakeBackground(bool a,bool b){ trace.push_back({"font_background",a,b}); }
inline void __cdecl FakeScale(float w,float h){ trace.push_back({"font_scale",F(w),F(h)}); }
inline void __cdecl FakeOrientation(eFontAlignment a){ trace.push_back({"font_orientation",int(a)}); }
inline void __cdecl FakeRightWrap(float v){ trace.push_back({"font_right_wrap",F(v)}); }
inline void __cdecl FakeFontStyle(eFontStyle s){ trace.push_back({"font_style",int(s)}); }
inline void __cdecl FakeWrapx(float v){ trace.push_back({"font_wrapx",F(v)}); }
inline void __cdecl FakeEdge(int8 v){ trace.push_back({"font_edge",int(v)}); }
inline void __cdecl FakeDropColor(CRGBA c){ trace.push_back({"font_drop_color",c.r,c.g,c.b,c.a}); }
inline void __cdecl FakeColor(CRGBA c){ trace.push_back({"font_color",c.r,c.g,c.b,c.a}); }
inline void __cdecl FakePrint(float x,float y,const GxtChar* t){ trace.push_back({"font_print",F(x),F(y),Name(t)}); }
inline float __cdecl FakeYPos(uint8 id,float pos,int8 off){ trace.push_back({"ypos",int(id),F(pos),int(off)}); return pos*1.25f+float(id)*3.0f+float(off); }
inline void __cdecl FakeAsciiToGxt(const char* src,GxtChar* dst){ trace.push_back({"ascii_to_gxt",Name(src),std::string(src,strnlen(src,64)),Name(dst)}); }

// --- armas del jugador y model infos ---
inline void __fastcall FakeRemoveWeaponAnims(void* self,void*,int32 w,float bd){ trace.push_back({"remove_weapon_anims",Name(self),w,F(bd)}); }
inline eWeaponSlot __fastcall FakeGiveWeapon(void* self,void*,eWeaponType t,uint32 ammo,bool like){ trace.push_back({"give_weapon",Name(self),int(t),ammo,like}); return eWeaponSlot(uint32(t)%13); }
inline void __fastcall FakeSetWeaponType(void* self,void*,eWeaponType t){ trace.push_back({"set_current_weapon_type",Name(self),int(t)}); }
inline void __fastcall FakeSetWeaponSlot(void* self,void*,int32 s){ trace.push_back({"set_current_weapon_slot",Name(self),s}); }
inline void __fastcall FakeChangesType(void* self,void*,eWeaponType t){ trace.push_back({"changes_new_weapon_type",Name(self),int(t)}); }
inline void __fastcall FakeChangesSlot(void* self,void*,uint32 s){ trace.push_back({"changes_new_weapon_slot",Name(self),s}); }
inline void __fastcall FakeRemoveWeaponModel(void* self,void*,int32 m){ trace.push_back({"remove_weapon_model",Name(self),m}); }
inline void __fastcall FakeClearTarget(void* self,void*){ trace.push_back({"clear_weapon_target",Name(self)}); }
inline void __fastcall FakeSetWanted(void* self,void*,eWantedLevel l){ trace.push_back({"set_wanted",Name(self),int(l)}); }
inline void __fastcall FakeAddRef(void* self,void*){ trace.push_back({"mi_add_ref",uintptr_t(self)}); }
inline void __fastcall FakeRemoveRef(void* self,void*){ trace.push_back({"mi_remove_ref",uintptr_t(self)}); }

// --- vehículos ---
inline eVehicleAppearance __fastcall FakeAppearance(const void* self,void*){ const auto r=eVehicleAppearance(static_cast<const uint8*>(self)[0x7F0]); trace.push_back({"appearance",Name(self),int(r)}); return r; }
inline bool __fastcall FakeLawEnforcement(const void* self,void*){ const bool r=static_cast<const uint8*>(self)[0x7F1]&1; trace.push_back({"law_enforcement",Name(self),r}); return r; }

// --- graffitis ---
inline int AtomIndex(const void* a){ for(size_t k=0;k<atoms.size();++k)if(a==atoms[k].data())return int(k); return -1; }
inline uint16 __cdecl FakeGetUserValue(const RpAtomic* a){ const int k=AtomIndex(a); const uint16 v=k>=0 ? userValues[k] : 0x5A5A; trace.push_back({"get_user_value",k,v}); return v; }
inline void __cdecl FakeSetUserValue(RpAtomic* a,uint16 v){ const int k=AtomIndex(a); if(k>=0)userValues[k]=v; trace.push_back({"set_user_value",k,v}); }
inline void __cdecl FakeTriggerMessage(const char* m,int16 a,uint16 t,int16 b){ trace.push_back({"trigger_message",std::string(m,strnlen(m,16)),a,t,b}); }
inline RpAtomic* __cdecl FakeRender(RpAtomic* a){ trace.push_back({"render",AtomIndex(a)}); return a; }
inline uint32 wbCounter{};
inline bool __cdecl FakeSaveWB(void* p,int32 n){ trace.push_back({"save_wb",Name(p),n,n>0&&n<=8 ? json(ReversingProbe::Bytes(p,size_t(n))) : json(nullptr)}); return true; }
inline bool __cdecl FakeLoadWB(void* p,int32 n){ auto* b=static_cast<uint8*>(p); for(int32 k=0;k<n&&k<8;++k)b[k]=uint8(wbCounter++*37u+11u); trace.push_back({"load_wb",Name(p),n}); return true; }
inline uint32 __fastcall FakeMiRwType(const void* self,void*){ const uint32 r=*reinterpret_cast<const uint32*>(static_cast<const uint8*>(self)+0x3C); trace.push_back({"mi_rw_type",Name(self),r}); return r; }
inline void* __fastcall FakeMiAsAtomic(void* self,void*){ void* r=*reinterpret_cast<void**>(static_cast<uint8*>(self)+0x38); trace.push_back({"mi_as_atomic",Name(self),Name(r)}); return r; }
inline void* __fastcall FakeMiTrap(void* self,void*){ trace.push_back({"mi_trap",Name(self)}); return self; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,38> fns{{
    {"CDarkel","Init",0x43CEB0,0,-1,32},{"CDarkel","FrenzyOnGoing",0x43D1F0,0,-1,64},{"CDarkel","ReadStatus",0x43D1E0,0,-1,64},
    {"CDarkel","CalcFade",0x43D740,0,-1,1024},{"CDarkel","CheckDamagedWeaponType",0x43D9E0,0,-1,1024},
    {"CDarkel","QueryModelsKilledByPlayer",0x43D6C0,0,-1,256},{"CDarkel","ResetModelsKilledByPlayer",0x43D6A0,0,-1,64},{"CDarkel","FindTotalPedsKilledByPlayer",0x43D6E0,0,-1,256},
    {"CDarkel","ThisPedShouldBeKilledForFrenzy",0x43D2F0,0,-1,1024},{"CDarkel","ThisVehicleShouldBeKilledForFrenzy",0x43D350,0,-1,1024},
    {"CDarkel","DrawMessages",0x43CEC0,0,-1,512},{"CDarkel","RegisterKillNotByPlayer",0x43D210,0,-1,512},{"CDarkel","RegisterKillByPlayer",0x43DCD0,0,-1,1024},
    {"CDarkel","RegisterCarBlownUpByPlayer",0x43DF20,0,-1,512},{"CDarkel","StartFrenzy",0x43D3B0,0,-1,512},{"CDarkel","DealWithWeaponChangeAtEndOfFrenzy",0x43D7A0,0,-1,512},
    {"CDarkel","Update",0x43DAC0,0,-1,1024},{"CDarkel","ResetOnPlayerDeath",0x43DC10,0,-1,256},{"CDarkel","FailKillFrenzy",0x43DC60,0,-1,256},
    {"CTagManager","Init",0x49CC50,0,-1,32},{"CTagManager","ShutdownForRestart",0x49CC60,0,-1,128},{"CTagManager","AddTag",0x49CC90,0,-1,128},
    {"CTagManager","FindTagDesc",0x49CCB0,0,-1,512},{"CTagManager","IsTag",0x49CCE0,0,-1,512},
    {"CTagManager","GetAlpha-RpAtomic",0x49CD40,0,-1,256},{"CTagManager","SetAlpha-RpAtomic",0x49CD30,0,-1,256},
    {"CTagManager","GetAlpha-Entity",0x49CF90,0,-1,512},{"CTagManager","SetAlpha-Entity",0x49CEC0,0,-1,1024},
    {"CTagManager","GetPercentageTagged",0x49CDA0,0,-1,512},{"CTagManager","GetPercentageTaggedInArea",0x49D0B0,0,-1,1024},
    {"CTagManager","UpdateNumTagged",0x49CDE0,0,-1,256},{"CTagManager","SetAlphaInArea",0x49CFE0,0,-1,512},{"CTagManager","GetNearestTag",0x49D160,0,-1,1024},
    {"CTagManager","GetTagPos",0x49CEA0,0,-1,256},{"CTagManager","SetupAtomic",0x49CE10,0,-1,128},{"CTagManager","RenderTagForPC",0x49CE40,0,-1,512},
    {"CTagManager","Save",0x5D3D60,0,-1,128},{"CTagManager","Load",0x5D3DA0,0,-1,128},
}};

struct Region { uintptr_t at; size_t n; std::vector<uint8> saved; };
constexpr uintptr_t DK_BEGIN=0x969A48, DK_END=0x96A708, TAG_BEGIN=0xA9A8B8, TAG_END=0xA9AD80;

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"game19-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"game19-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("game19");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"game19-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,38> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,38> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // ranuras libres de ms_modelInfoPtrs para los model infos falsos (índices < 0x8000)
    std::array<int32,4> miSlots{}; { size_t k=0; for(int32 i=1;i<20000&&k<4;++i)if(!CModelInfo::ms_modelInfoPtrs[i])miSlots[k++]=i;
        if(k<4){Write({{"event","error"},{"reason","no free model info slots"}});return;} }
    // armas con model info válido (para RemoveRef/AddRef y m_nRefCount reales)
    std::vector<int32> validW; for(int32 t=1;t<47;++t){ const int32 m=CWeaponInfo::GetWeaponInfo(eWeaponType(t))->m_nModelId1; if(m>0&&m<20000&&CModelInfo::ms_modelInfoPtrs[m])validW.push_back(t); }
    if(validW.empty()||std::find(validW.begin(),validW.end(),28)==validW.end()){Write({{"event","error"},{"reason","weapon model infos"}});return;}

    // estado global guardado
    std::vector<Region> regions{{DK_BEGIN,DK_END-DK_BEGIN,{}},{0x96A8A8,0x10,{}},{TAG_BEGIN,TAG_END-TAG_BEGIN,{}},{0xB78E90,0x80,{}},{0x8D2534,4,{}},
        {0xB7CD98,4,{}},{0xB7CF28,4,{}},{0xB7CB84,4,{}},{0xB7CB4C,4,{}},{0xA43088,1,{}},{0xB7CD74,1,{}},{0xC17044,8,{}},{0xB6F065,1,{}},
        {0xB71670,352,{}},{0xC1B100,552,{}}};
    for(auto s:miSlots)regions.push_back({uintptr_t(&CModelInfo::ms_modelInfoPtrs[s]),4,{}});
    for(auto& r:regions)r.saved.assign(reinterpret_cast<uint8*>(r.at),reinterpret_cast<uint8*>(r.at)+r.n);
    auto RestoreAll=[&]{ for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.at),r.saved.data(),r.n); };
    auto AllRestored=[&]{ for(auto& r:regions)if(std::memcmp(reinterpret_cast<void*>(r.at),r.saved.data(),r.n))return false; return true; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), p2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch s1(bit_cast<uintptr_t>(&CStats::IncrementStat),FakeInc), s1e(0x55C180,FakeInc);
        EntryPatch s2(bit_cast<uintptr_t>(&CStats::DecrementStat),FakeDec), s2e(0x559FA0,FakeDec);
        EntryPatch s3(bit_cast<uintptr_t>(&CStats::DisplayScriptStatUpdateMessage),FakeDisplay), s3e(0x55B980,FakeDisplay);
        EntryPatch a1(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),FakeFrontend), a1e(0x506EA0,FakeFrontend);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGroupMembership::IsMember),FakeIsMember), g1e(0x5F6A10,FakeIsMember);
        EntryPatch g2(bit_cast<uintptr_t>(&CGangWars::AddKillToProvocation),FakeProvocation), g2e(0x443950,FakeProvocation);
        EntryPatch g3(bit_cast<uintptr_t>(&CPopCycle::PlayerKilledADealer),FakeDealer), g3e(0x610490,FakeDealer);
        EntryPatch h1(bit_cast<uintptr_t>(&CHud::SetHelpMessage),FakeHelp), h1e(0x588BE0,FakeHelp);
        EntryPatch h2(bit_cast<uintptr_t>(&CHud::GetYPosBasedOnHealth),FakeYPos), h2e(0x588B60,FakeYPos);
        EntryPatch l1(bit_cast<uintptr_t>(&CGameLogic::ClearSkip),FakeClearSkip), l1e(0x441560,FakeClearSkip);
        EntryPatch l2(bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn),FakeCoop), l2e(0x441390,FakeCoop);
        EntryPatch l3(bit_cast<uintptr_t>(&CGameLogic::DoWeaponStuffAtStartOf2PlayerGame),FakeWeaponStuff), l3e(0x4428B0,FakeWeaponStuff);
        EntryPatch t1(bit_cast<uintptr_t>(&CText::Get),FakeTextGet), t1e(0x6A0050,FakeTextGet);
        EntryPatch t2(bit_cast<uintptr_t>(&CMessages::AddBigMessage),FakeBigMessage), t2e(0x69F2B0,FakeBigMessage);
        EntryPatch t3(bit_cast<uintptr_t>(&AsciiToGxtChar),FakeAsciiToGxt), t3e(0x718600,FakeAsciiToGxt);
        EntryPatch f1(bit_cast<uintptr_t>(&CFont::SetProportional),FakeProportional), f1e(0x7195B0,FakeProportional);
        EntryPatch f2(bit_cast<uintptr_t>(&CFont::SetBackground),FakeBackground), f2e(0x7195C0,FakeBackground);
        EntryPatch f3(bit_cast<uintptr_t>(&CFont::SetScale),FakeScale), f3e(0x719380,FakeScale);
        EntryPatch f4(bit_cast<uintptr_t>(&CFont::SetOrientation),FakeOrientation), f4e(0x719610,FakeOrientation);
        EntryPatch f5(bit_cast<uintptr_t>(&CFont::SetRightJustifyWrap),FakeRightWrap), f5e(0x7194F0,FakeRightWrap);
        EntryPatch f6(bit_cast<uintptr_t>(&CFont::SetFontStyle),FakeFontStyle), f6e(0x719490,FakeFontStyle);
        EntryPatch f7(bit_cast<uintptr_t>(&CFont::SetWrapx),FakeWrapx), f7e(0x7194D0,FakeWrapx);
        EntryPatch f8(bit_cast<uintptr_t>(&CFont::SetEdge),FakeEdge), f8e(0x719590,FakeEdge);
        EntryPatch f9(bit_cast<uintptr_t>(&CFont::SetDropColor),FakeDropColor), f9e(0x719510,FakeDropColor);
        EntryPatch f10(bit_cast<uintptr_t>(&CFont::SetColor),FakeColor), f10e(0x719430,FakeColor);
        EntryPatch f11(bit_cast<uintptr_t>(&CFont::PrintString),FakePrint), f11e(0x71A700,FakePrint);
        EntryPatch w1(bit_cast<uintptr_t>(&CPed::RemoveWeaponAnims),FakeRemoveWeaponAnims), w1e(0x5F0250,FakeRemoveWeaponAnims);
        EntryPatch w2(bit_cast<uintptr_t>(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),FakeGiveWeapon), w2e(0x5E6080,FakeGiveWeapon);
        EntryPatch w3(bit_cast<uintptr_t>(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),FakeSetWeaponType), w3e(0x5E6280,FakeSetWeaponType);
        EntryPatch w4(bit_cast<uintptr_t>(static_cast<void(CPed::*)(int32)>(&CPed::SetCurrentWeapon)),FakeSetWeaponSlot), w4e(0x5E61F0,FakeSetWeaponSlot);
        EntryPatch w5(bit_cast<uintptr_t>(static_cast<void(CPlayerPed::*)(eWeaponType)>(&CPlayerPed::MakeChangesForNewWeapon)),FakeChangesType), w5e(0x60B460,FakeChangesType);
        EntryPatch w6(bit_cast<uintptr_t>(static_cast<void(CPlayerPed::*)(uint32)>(&CPlayerPed::MakeChangesForNewWeapon)),FakeChangesSlot), w6e(0x60D000,FakeChangesSlot);
        EntryPatch w7(bit_cast<uintptr_t>(&CPed::RemoveWeaponModel),FakeRemoveWeaponModel), w7e(0x5E3990,FakeRemoveWeaponModel);
        EntryPatch w8(bit_cast<uintptr_t>(&CPlayerPed::ClearWeaponTarget),FakeClearTarget), w8e(0x609C80,FakeClearTarget);
        EntryPatch w9(bit_cast<uintptr_t>(&CPlayerPed::SetWantedLevel),FakeSetWanted), w9e(0x609F10,FakeSetWanted);
        EntryPatch m1(bit_cast<uintptr_t>(&CBaseModelInfo::AddRef),FakeAddRef), m1e(0x4C4BA0,FakeAddRef);
        EntryPatch m2(bit_cast<uintptr_t>(&CBaseModelInfo::RemoveRef),FakeRemoveRef), m2e(0x4C4BB0,FakeRemoveRef);
        EntryPatch v1(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),FakeAppearance), v1e(0x6D1080,FakeAppearance);
        EntryPatch v2(bit_cast<uintptr_t>(&CVehicle::IsLawEnforcementVehicle),FakeLawEnforcement), v2e(0x6D2370,FakeLawEnforcement);
        EntryPatch u1(bit_cast<uintptr_t>(&CVisibilityPlugins::GetUserValue),FakeGetUserValue), u1e(0x7323A0,FakeGetUserValue);
        EntryPatch u2(bit_cast<uintptr_t>(&CVisibilityPlugins::SetUserValue),FakeSetUserValue), u2e(0x732380,FakeSetUserValue);
        EntryPatch u3(bit_cast<uintptr_t>(&CGarages::TriggerMessage),FakeTriggerMessage), u3e(0x447B80,FakeTriggerMessage);
        EntryPatch b1(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWB), b1e(0x5D1270,FakeSaveWB);
        EntryPatch b2(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FakeLoadWB), b2e(0x5D1300,FakeLoadWB);
        static std::array<void*,16> miVt{}; for(auto& v:miVt)v=reinterpret_cast<void*>(&FakeMiTrap); miVt[1]=reinterpret_cast<void*>(&FakeMiAsAtomic); miVt[9]=reinterpret_cast<void*>(&FakeMiRwType);
        static const char* const textKeys[4]{"PAGE_00","KILLPA","OTHER","MSG"};
        for(size_t k=0;k<texts.size();++k){ texts[k].fill(0); for(size_t c=0;textKeys[k][c];++c)texts[k][c]=GxtChar(textKeys[k][c]); }

        auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84);
        auto& player0=*reinterpret_cast<void**>(0xB7CD98); auto& player1=*reinterpret_cast<void**>(0xB7CF28);
        auto* dk=reinterpret_cast<uint8*>(DK_BEGIN); auto* tg=reinterpret_cast<uint8*>(TAG_BEGIN);
        auto* tagBase=reinterpret_cast<uint8*>(0xA9A8C0);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto rnd=[&](uint32 n){ return next()%n; };
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6A19u; if(!stt)stt=1; trace=json::array();
                    mask=next(); maskCalls=0; wbCounter=0; playersPresent=0; findResult=peds[0].data(); vehResult=nullptr;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // --- fixtures comunes ---
                    auto weaponAny=[&]{ return int32(validW[rnd(uint32(validW.size()))]); };
                    auto frenzyWeapon=[&]{ switch(rnd(8)){case 0:return int32(52);case 1:return int32(47+rnd(11));case 2:return int32(28);default:return weaponAny();} };
                    auto setPeds=[&]{
                        for(auto& p:peds)for(auto& b:p)b=uint8(next());
                        for(auto& d:pdata)for(auto& b:d)b=uint8(next());
                        for(size_t k=0;k<2;++k){ auto* p=peds[k].data();
                            for(int s=0;s<13;++s){ auto* w=p+0x5A0+s*0x1C; *reinterpret_cast<int32*>(w)=s==0||rnd(4)==0 ? 0 : weaponAny(); *reinterpret_cast<uint32*>(w+4)=rnd(5);
                                const uint32 clip=rnd(60); *reinterpret_cast<uint32*>(w+8)=clip; *reinterpret_cast<uint32*>(w+0xC)=rnd(3)==0 ? rnd(40) : clip+rnd(500); }
                            p[0x718]=uint8(rnd(13)); *reinterpret_cast<void**>(p+0x480)=pdata[k].data();
                            pdata[k][0x20]=uint8(rnd(13)); *reinterpret_cast<uint32*>(pdata[k].data()+0x38)=rnd(8);
                            *reinterpret_cast<uint32*>(p+0x598)=k; }
                    };
                    auto setDarkel=[&]{
                        for(uintptr_t k=0x96A6D0;k<DK_END;++k)dk[k-DK_BEGIN]=uint8(next());
                        for(int k=0;k<8;++k)dk[k]=uint8(next());
                        static const uint16 sts[]{0,1,1,4,4,2,3,5}; CDarkel::Status=eDarkelStatus(sts[rnd(8)]);
                        for(auto& m:CDarkel::ModelToKill){ switch(rnd(6)){case 0:m=-1;break;case 1:m=-2;break;case 2:m=-3;break;default:m=int32(rnd(800));} }
                        CDarkel::WeaponType=eWeaponType(frenzyWeapon()); CDarkel::KillsNeeded=int32(rnd(7))-3;
                        CDarkel::TimeLimit=rnd(4)==0 ? -int32(1+rnd(5)) : int32(rnd(200000)); timeMs=1000000+rnd(400000); CDarkel::TimeOfFrenzyStart=timeMs-rnd(220000);
                        CDarkel::PreviousTime=int32(rnd(300))-10; CDarkel::bHeadShotRequired=rnd(2)!=0; CDarkel::bStandardSoundAndMessages=rnd(2)!=0; CDarkel::bProperKillFrenzy=rnd(2)!=0;
                        CDarkel::pStartMessage=rnd(3) ? texts[3].data() : nullptr;
                        CDarkel::InterruptedWeaponType=eWeaponType(rnd(3)==0 ? 0 : weaponAny()); CDarkel::InterruptedWeaponTypeSelected=eWeaponType(rnd(47)); CDarkel::AmmoInterruptedWeapon=rnd(1000);
                        *reinterpret_cast<uint8*>(0x96A8B0)=uint8(rnd(5)); *reinterpret_cast<int32*>(0x96A8AC)=int32(next());
                    };
                    auto killsFill=[&]{ auto* k=reinterpret_cast<uint16*>(0x969A50); for(int e=0;e<1600;++e)k[e]=uint16(rnd(8)==0 ? next() : rnd(50)); };
                    auto victimModel=[&]{ const int32 m=CDarkel::ModelToKill[rnd(4)]; return rnd(3)==0&&m>=0 ? m : int32(rnd(800)); };
                    auto setTags=[&](bool allowNaN){
                        for(size_t k=0;k<ents.size();++k){ auto* e=ents[k].data(); for(auto& b:ents[k])b=uint8(next()); for(auto& b:mats[k])b=uint8(next()); for(auto& b:atoms[k])b=uint8(next());
                            auto pos=[&]{ return allowNaN&&rnd(24)==0 ? std::numeric_limits<float>::quiet_NaN() : uni(-100.f,100.f); };
                            *reinterpret_cast<void**>(e+0x14)=rnd(2) ? mats[k].data() : nullptr; for(int c=0;c<3;++c){ *reinterpret_cast<float*>(e+4+c*4)=pos(); *reinterpret_cast<float*>(mats[k].data()+0x30+c*4)=pos(); }
                            *reinterpret_cast<void**>(e+0x18)=rnd(3) ? atoms[k].data() : nullptr; *reinterpret_cast<uint16*>(e+0x22)=uint16(miSlots[rnd(4)]);
                            atoms[k][0]=1; *reinterpret_cast<void**>(atoms[k].data()+0x18)=geos[k%4].data(); *reinterpret_cast<void*(**)(void*)>(atoms[k].data()+0x48)=reinterpret_cast<void*(*)(void*)>(&FakeRender);
                            userValues[k]=uint16(rnd(3)==0 ? next() : 0xE0+rnd(10)); }
                        for(int k=0;k<150;++k){ auto* d=tagBase+k*8; *reinterpret_cast<void**>(d)=ents[rnd(24)].data(); d[4]=rnd(3)==0 ? uint8(0xE3+rnd(3)) : uint8(next()); d[5]=uint8(next()); d[6]=uint8(next()); d[7]=uint8(next()); }
                        CTagManager::ms_numTags=int32(rnd(151)); CTagManager::ms_numTagged=int32(rnd(160)); CTagManager::ms_pPipeline=reinterpret_cast<RxPipeline*>(uintptr_t(0x40000000+rnd(0x1000)*4));
                        for(size_t g=0;g<geos.size();++g){ for(auto& b:geos[g])b=uint8(next()); matLists[g][0]=materials[g*2].data(); matLists[g][1]=materials[g*2+1].data(); *reinterpret_cast<void**>(geos[g].data()+0x20)=matLists[g].data(); }
                        for(auto& m:materials)for(auto& b:m)b=uint8(next());
                        for(size_t k=0;k<mis.size();++k){ auto* m=mis[k].data(); for(auto& b:mis[k])b=uint8(next()); *reinterpret_cast<void**>(m)=miVt.data();
                            if(rnd(2)){ m[0x13]=uint8((m[0x13]&~0x78)|(6<<3)); } *reinterpret_cast<uint32*>(m+0x3C)=rnd(3)==0 ? 2 : 1; *reinterpret_cast<void**>(m+0x38)=mis[rnd(4)].data();
                            CModelInfo::ms_modelInfoPtrs[miSlots[k]]=reinterpret_cast<CBaseModelInfo*>(m); }
                    };
                    auto tagEntity=[&]{ return reinterpret_cast<CEntity*>(*reinterpret_cast<void**>(tagBase+8*rnd(uint32(std::max<int32>(CTagManager::ms_numTags,1))))); };
                    auto randRect=[&]{ CRect r; r.left=uni(-120.f,80.f); r.right=r.left+uni(-20.f,160.f); r.bottom=uni(-120.f,80.f); r.top=r.bottom+uni(-20.f,160.f);
                        if(rnd(32)==0)r.left=std::numeric_limits<float>::quiet_NaN(); if(rnd(32)==0)r.top=std::numeric_limits<float>::quiet_NaN(); return r; };
                    auto darkelOut=[&]{ out["dk"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96A6D0),DK_END-0x96A6D0); out["dk_flags"]=ReversingProbe::Bytes(dk,8);
                        out["kills"]=Fnv(reinterpret_cast<void*>(0x969A50),3200); out["gl"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96A8A8),0x10);
                        out["pk"]=Fnv(reinterpret_cast<void*>(0xB78E90),0x80); out["pop"]=*reinterpret_cast<int32*>(0x8D2534);
                        out["peds"]=Fnv(peds.data(),sizeof(peds)); out["pdata"]=Fnv(pdata.data(),sizeof(pdata)); };
                    auto tagOut=[&]{ out["tags"]=Fnv(tg,TAG_END-TAG_BEGIN); out["num"]=json::array({CTagManager::ms_numTags,CTagManager::ms_numTagged});
                        out["objs"]=json::array({Fnv(ents.data(),sizeof(ents)),Fnv(atoms.data(),sizeof(atoms)),Fnv(geos.data(),sizeof(geos)),Fnv(materials.data(),sizeof(materials))});
                        out["user"]=Fnv(userValues.data(),sizeof(userValues)); };

                    if(cls=="CDarkel") {
                        setDarkel(); setPeds();
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="FrenzyOnGoing"){ CDarkel::Status=eDarkelStatus(rnd(4)==0 ? uint16(next()) : uint16(rnd(6))); ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                        else if(nm=="ReadStatus"){ CDarkel::Status=eDarkelStatus(uint16(next())); ret=int(reinterpret_cast<eDarkelStatus(__cdecl*)()>(a)()); }
                        else if(nm=="CalcFade"){ const uint32 b=rnd(4)==0 ? next() : rnd(100000); const uint32 e=rnd(4)==0 ? next() : b+rnd(3000); uint32 t;
                            switch(rnd(5)){case 0:t=next();break;case 1:t=b+rnd(600);break;case 2:t=e-rnd(600);break;default:t=b+rnd(e-b+2);}
                            ret=int(reinterpret_cast<uint8(__cdecl*)(uint32,uint32,uint32)>(a)(t,b,e)); }
                        else if(nm=="CheckDamagedWeaponType"){ const int32 d=rnd(10)==0 ? -1 : rnd(10)==0 ? int32(next()) : int32(rnd(62)); const int32 x=rnd(3) ? int32(56+rnd(2)) : int32(rnd(60));
                            ret=reinterpret_cast<bool(__cdecl*)(eWeaponType,eWeaponType)>(a)(eWeaponType(d),eWeaponType(x)); }
                        else if(nm=="QueryModelsKilledByPlayer"){ killsFill(); ret=reinterpret_cast<int16(__cdecl*)(eModelID,int32)>(a)(eModelID(rnd(800)),int32(rnd(2))); }
                        else if(nm=="ResetModelsKilledByPlayer"){ killsFill(); reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(rnd(2))); }
                        else if(nm=="FindTotalPedsKilledByPlayer"){ killsFill(); ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(int32(rnd(2))); }
                        else if(nm=="ThisPedShouldBeKilledForFrenzy"){ auto* v=peds[2].data(); *reinterpret_cast<uint16*>(v+0x22)=uint16(rnd(8)==0 ? 0x8000+rnd(0x8000) : victimModel()); if(rnd(6)==0)*reinterpret_cast<uint16*>(v+0x22)=0xFFFF;
                            if(rnd(8)==0)CDarkel::ModelToKill[rnd(4)]=int32(int16(*reinterpret_cast<uint16*>(v+0x22)));
                            *reinterpret_cast<uint32*>(v+0x598)=rnd(4)==0 ? rnd(2) : rnd(32); ret=reinterpret_cast<bool(__cdecl*)(const void*)>(a)(v); }
                        else if(nm=="ThisVehicleShouldBeKilledForFrenzy"){ auto* v=vehs[0].data(); for(auto& b:vehs[0])b=uint8(next()); *reinterpret_cast<uint16*>(v+0x22)=uint16(rnd(8)==0 ? 0x8000+rnd(0x8000) : victimModel());
                            if(rnd(8)==0)CDarkel::ModelToKill[rnd(4)]=int32(int16(*reinterpret_cast<uint16*>(v+0x22)));
                            *reinterpret_cast<uint32*>(v+0x594)=rnd(3)==0 ? 9 : rnd(12); ret=reinterpret_cast<bool(__cdecl*)(const void*)>(a)(v); }
                        else if(nm=="DrawMessages"){ static const int32 res[][2]{{640,480},{800,600},{1024,768},{1280,720},{1366,768},{1600,900},{1920,1080},{3840,2160}};
                            const auto& rs=res[rnd(8)]; RsGlobal.maximumWidth=rs[0]; RsGlobal.maximumHeight=rs[1];
                            *reinterpret_cast<uint8*>(0xA43088)=uint8(rnd(5)==0 ? 1 : rnd(3)==0 ? 2 : 0); *reinterpret_cast<uint8*>(0xB7CD74)=uint8(rnd(2));
                            *reinterpret_cast<uint32*>(0xB7CB4C)=next(); player1=rnd(2) ? peds[1].data() : nullptr; CDarkel::KillsNeeded=int32(rnd(40))-5;
                            const uint32 el=rnd(4)==0 ? rnd(16000) : rnd(400000); CDarkel::TimeOfFrenzyStart=timeMs-el; if(rnd(4)==0)CDarkel::TimeLimit=int32(el+rnd(8000));
                            std::memset(gString,'Z',64); reinterpret_cast<void(__cdecl*)()>(a)(); out["gstring"]=std::string(gString,strnlen(gString,64)); }
                        else if(nm=="RegisterKillNotByPlayer"){ auto* v=peds[2].data(); *reinterpret_cast<uint32*>(v+0x598)=rnd(32); findResult=rnd(3)==0 ? v : peds[0].data();
                            reinterpret_cast<void(__cdecl*)(const void*)>(a)(v); }
                        else if(nm=="RegisterKillByPlayer"){ killsFill(); auto* v=peds[2].data(); *reinterpret_cast<uint16*>(v+0x22)=uint16(victimModel()); *reinterpret_cast<uint32*>(v+0x598)=rnd(4)==0 ? 6 : rnd(32);
                            player0=peds[0].data(); const int32 dmg=rnd(12)==0 ? -1 : rnd(3)==0 ? int32(CDarkel::WeaponType) : static_cast<int32>(rnd(4)==0 ? 49+rnd(5) : rnd(58));
                            if(rnd(6)==0){ static const int32 wt[]{56,57,28,49,50,18,51}; CDarkel::WeaponType=eWeaponType(wt[rnd(7)]); }
                            reinterpret_cast<void(__cdecl*)(const void*,eWeaponType,bool,int32)>(a)(v,eWeaponType(dmg),rnd(2)!=0,int32(rnd(2))); }
                        else if(nm=="RegisterCarBlownUpByPlayer"){ killsFill(); auto* v=vehs[1].data(); for(auto& b:vehs[1])b=uint8(next()); *reinterpret_cast<uint16*>(v+0x22)=uint16(rnd(5)==0 ? 432 : victimModel());
                            *reinterpret_cast<uint32*>(v+0x594)=rnd(3)==0 ? 9 : rnd(12); v[0x7F0]=uint8(rnd(7)); v[0x7F1]=uint8(next());
                            reinterpret_cast<void(__cdecl*)(void*,int32)>(a)(v,int32(rnd(2))); }
                        else if(nm=="StartFrenzy"){ playersPresent=rnd(4); vehResult=rnd(2) ? vehs[0].data() : nullptr;
                            const int32 w=rnd(4)==0 ? int32(rnd(47)) : frenzyWeapon(); const GxtChar* msg=rnd(3)==0 ? texts[0].data() : rnd(2) ? texts[3].data() : nullptr;
                            using Start=void(__cdecl*)(eWeaponType,int32,uint16,int32,const GxtChar*,int32,int32,int32,bool,bool);
                            reinterpret_cast<Start>(a)(eWeaponType(w),rnd(4)==0 ? -1 : int32(rnd(400000)),uint16(rnd(4)==0 ? next() : rnd(40)),int32(rnd(5)==0 ? -1-int32(rnd(3)) : rnd(800)),msg,
                                                       int32(rnd(800)),int32(rnd(800)),int32(rnd(800)),rnd(2)!=0,rnd(2)!=0); }
                        else if(nm=="DealWithWeaponChangeAtEndOfFrenzy"){ vehResult=rnd(2) ? vehs[0].data() : nullptr; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Update"||nm=="ResetOnPlayerDeath"||nm=="FailKillFrenzy"){ vehResult=rnd(2) ? vehs[0].data() : nullptr; player1=rnd(3) ? peds[1].data() : nullptr;
                            if(nm=="Update"&&rnd(3)==0)CDarkel::PreviousTime=(int32(CDarkel::TimeOfFrenzyStart+CDarkel::TimeLimit-timeMs))/1000;
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        darkelOut();
                    }
                    else if(cls=="CTagManager") {
                        const bool area=nm=="GetPercentageTaggedInArea"||nm=="SetAlphaInArea"; setTags(area);
                        if(nm=="Init"||nm=="ShutdownForRestart"||nm=="UpdateNumTagged")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="AddTag"){ CTagManager::ms_numTags=int32(rnd(150)); reinterpret_cast<void(__cdecl*)(void*)>(a)(ents[rnd(24)].data()); }
                        else if(nm=="FindTagDesc"){ void* e=rnd(4)==0 ? static_cast<void*>(ents[rnd(24)].data()) : static_cast<void*>(tagEntity()); ret=Name(reinterpret_cast<void*(__cdecl*)(const void*)>(a)(e)); }
                        else if(nm=="IsTag"){ auto* e=ents[rnd(24)].data(); ret=reinterpret_cast<bool(__cdecl*)(const void*)>(a)(e); }
                        else if(nm=="GetAlpha-RpAtomic"){ ret=int(reinterpret_cast<uint8(__cdecl*)(const void*)>(a)(atoms[rnd(24)].data())); }
                        else if(nm=="SetAlpha-RpAtomic"){ reinterpret_cast<void(__cdecl*)(void*,uint8)>(a)(atoms[rnd(24)].data(),uint8(next())); }
                        else if(nm=="GetAlpha-Entity"){ if(!CTagManager::ms_numTags)CTagManager::ms_numTags=1; ret=int(reinterpret_cast<uint8(__cdecl*)(const void*)>(a)(tagEntity())); }
                        else if(nm=="SetAlpha-Entity"){ if(!CTagManager::ms_numTags)CTagManager::ms_numTags=1; *reinterpret_cast<uint8*>(0xB6F065)=uint8(rnd(4)==0);
                            if(rnd(8)==0){ for(int k=0;k<CTagManager::ms_numTags;++k)tagBase[k*8+4]=0xFF; tagBase[rnd(uint32(CTagManager::ms_numTags))*8+4]=0x10; }
                            const uint8 alpha=rnd(3)==0 ? uint8(0xE3+rnd(4)) : uint8(next()); reinterpret_cast<void(__cdecl*)(void*,uint8)>(a)(tagEntity(),alpha); }
                        else if(nm=="GetPercentageTagged"){ if(rnd(10)==0)CTagManager::ms_numTags=0; CTagManager::ms_numTagged=int32(rnd(uint32(CTagManager::ms_numTags)+20)); ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                        else if(nm=="GetPercentageTaggedInArea"){ const CRect r=randRect(); ret=reinterpret_cast<int32(__cdecl*)(const CRect&)>(a)(r); }
                        else if(nm=="SetAlphaInArea"){ const CRect r=randRect(); reinterpret_cast<void(__cdecl*)(const CRect&,uint8)>(a)(r,rnd(3)==0 ? uint8(0xE3+rnd(4)) : uint8(next())); }
                        else if(nm=="GetNearestTag"){ if(!CTagManager::ms_numTags)CTagManager::ms_numTags=1+int32(rnd(150)); CVector p{uni(-120.f,120.f),uni(-120.f,120.f),uni(-10.f,10.f)};
                            if(rnd(8)==0){ const auto* e=static_cast<const uint8*>(static_cast<const void*>(tagEntity())); const auto* m=*reinterpret_cast<uint8* const*>(e+0x14);
                                p=*reinterpret_cast<const CVector*>(m ? m+0x30 : e+4); }
                            ret=Name(reinterpret_cast<void*(__cdecl*)(const CVector&)>(a)(p)); }
                        else if(nm=="GetTagPos"){ ret=Name(reinterpret_cast<const void*(__cdecl*)(int32)>(a)(int32(rnd(150)))); }
                        else if(nm=="SetupAtomic"){ reinterpret_cast<void(__cdecl*)(void*)>(a)(atoms[rnd(24)].data()); out["pipeline"]=uintptr_t(CTagManager::ms_pPipeline); }
                        else if(nm=="RenderTagForPC"){ reinterpret_cast<void(__cdecl*)(void*)>(a)(atoms[rnd(24)].data()); }
                        else if(nm=="Save"||nm=="Load"){ ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                        tagOut();
                    }
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreAll(); return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
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
