#pragma once
// Banco dentro del motor: CShopping (lista de 30, 30 funciones). Original (hooks de la clase desactivados) frente a
// candidato con fixtures deterministas e identicos. Nunca se leen ficheros reales: CFileMgr::OpenFile/CloseFile y
// CFileLoader::LoadLine se simulan con un shopping.dat generado en memoria (varios manejadores con cursor propio y un
// unico bufer de linea como el real). Nunca se toca el jugador real: peatones, CPlayerPedData, CPedClothesDesc,
// vehiculo, CWanted y modelos (entradas 19900..19963 de ms_modelInfoPtrs) son falsos con offsets crudos;
// CWorld::Players se sustituye por el fixture durante cada ejecucion. Se simulan y trazan: estadisticas, CTimer
// (Suspend/Resume), GetModelInfo por nombre, FindWeaponType, bloques de animacion, streaming, FindPlayerVehicle/
// Wanted, CWanted::ClearWantedLevelAndGoOnParole, ropa (SetTextureAndModel, GetTextureDependency), armas (GiveWeapon,
// GetWeaponSlot, SetCurrentWeapon), mejoras de vehiculo, CAutomobile (Fix*, Set*Damage), CDamageManager (Get/Set
// de ruedas, paneles y puertas), IsValidModForVehicle y el bufer de guardado (nunca partidas reales).
// Los cuerpos de la DLL se resuelven antes de parchear y nunca se parchea la direccion de un stub. La traza es un
// array fijo (sin reservas durante la llamada) que se convierte a json despues. Todas las globales de CShopping
// (0xA97258..0xA9A890: compras, estadisticas, claves, modificadores, precios, contenido, contadores, tienda cargada,
// ropa guardada, mejoras y danos guardados, propiedad actual), CWorld::Players, el jugador en foco, el nivel actual
// y las entradas de modelos tocadas se guardan y se restauran en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Shopping.h"
#include "World.h"
#include "PlayerInfo.h"
#include "Stats.h"
#include "Timer.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "Streaming.h"
#include "Animation/AnimManager.h"
#include "Models/ModelInfo.h"
#include "WeaponInfo.h"
#include "Wanted.h"
#include "Clothes.h"
#include "PedClothesDesc.h"
#include "DamageManager.h"
#include "Entity/Vehicle/Automobile.h"
#include "GenericGameStorage.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace Shop30Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T> inline T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline uint32 SHash(const char* s) { if(!s)return 0; uint32 h=2166136261u; for(;*s;++s){h^=uint8(*s);h*=16777619u;} return h; }

// ======================= globales y objetos falsos =======================
constexpr uintptr_t SHOP=0xA97258, SHOP_END=0xA9A890; constexpr size_t SHOPSZ=SHOP_END-SHOP;
constexpr uintptr_t PLAYERS=0xB7CD98; constexpr size_t PLSZ=0x190;
constexpr uintptr_t MI_PTRS=0xA9B0C8; constexpr int MI_FIRST=19900, MI_COUNT=64, MI_UP=19900, MI_VEH=19950;
constexpr size_t PEDSZ=0x800, PDSZ=0x100, CLSZ=0x100, VEHSZ=0xA00, WSZ=0x40, MIUPSZ=0x40, MIVSZ=0x80, VSTSZ=0x500;
alignas(16) inline std::array<uint8,2*PEDSZ> peds{};
alignas(16) inline std::array<uint8,2*PDSZ> pdatas{};
alignas(16) inline std::array<uint8,2*CLSZ> clothes{};
alignas(16) inline std::array<uint8,VEHSZ> veh{};
alignas(16) inline std::array<uint8,WSZ> wanted{};
alignas(16) inline std::array<uint8,32*MIUPSZ> miUp{};
alignas(16) inline std::array<uint8,4*MIVSZ> miVeh{};
alignas(16) inline std::array<uint8,4*VSTSZ> vstruct{};
alignas(16) inline std::array<char,512> lineBuf{};
alignas(16) inline std::array<char,64> arg1{}, arg2{};
inline uint8* Ped(int k) { return peds.data()+size_t(k)*PEDSZ; }
inline uint8* PData(int k) { return pdatas.data()+size_t(k)*PDSZ; }
inline uint8* Clo(int k) { return clothes.data()+size_t(k)*CLSZ; }
inline uint8* Plr(int k) { return reinterpret_cast<uint8*>(PLAYERS)+size_t(k)*PLSZ; }
inline CBaseModelInfo*& MiPtr(int idx) { return reinterpret_cast<CBaseModelInfo**>(MI_PTRS)[idx]; }

struct Rg { uintptr_t p; size_t n; size_t stride; const char* nm; };
inline std::vector<Rg> NameRegions() {
    return {{uintptr_t(peds.data()),peds.size(),PEDSZ,"ped"},{uintptr_t(pdatas.data()),pdatas.size(),PDSZ,"pdata"},{uintptr_t(clothes.data()),clothes.size(),CLSZ,"clothes"},
            {uintptr_t(veh.data()),VEHSZ,0,"veh"},{uintptr_t(wanted.data()),WSZ,0,"wanted"},{uintptr_t(miUp.data()),miUp.size(),MIUPSZ,"mi_up"},{uintptr_t(miVeh.data()),miVeh.size(),MIVSZ,"mi_veh"},
            {uintptr_t(vstruct.data()),vstruct.size(),VSTSZ,"vstruct"},{uintptr_t(lineBuf.data()),lineBuf.size(),0,"line"},{uintptr_t(arg1.data()),arg1.size(),0,"arg1"},{uintptr_t(arg2.data()),arg2.size(),0,"arg2"},
            {SHOP,SHOPSZ,0,"shop"},{PLAYERS,2*PLSZ,PLSZ,"player"},{0x5F00,0x200,0x10,"file"}};
}
inline json Name(uint32 v) {
    if(!v)return nullptr;
    static const auto regions=NameRegions();
    for(auto& r:regions){ if(v<r.p||v>=r.p+r.n)continue; const auto o=v-r.p;
        if(r.stride)return std::string(r.nm)+"#"+std::to_string(o/r.stride)+"+"+std::to_string(o%r.stride);
        return std::string(r.nm)+"+"+std::to_string(o); }
    return v;
}

// ======================= fichero falso (shopping.dat en memoria) =======================
inline std::vector<std::string> fileLines;      // se construye antes de la llamada; los stubs solo leen
inline std::array<int,32> cursors{}; inline int opens{}; inline bool keepCommas{};
inline int HandleIndex(FILESTREAM f) { const uint32 v=U(f); if(v<0x5F00||v>=0x5F00+0x10*32||(v-0x5F00)%0x10)return -1; return int((v-0x5F00)/0x10); }
inline FILESTREAM Handle(int k) { return reinterpret_cast<FILESTREAM>(uintptr_t(0x5F00+0x10*k)); }

// ======================= traza fija (sin reservas durante la llamada) =======================
enum Code : uint32 {
    C_STAT, C_OPEN, C_CLOSE, C_LINE, C_WTYPE, C_MINAME, C_ANIMBLK, C_REQ, C_LOADALL, C_DELETABLE, C_SUSPEND, C_RESUME,
    C_VALIDMOD, C_FINDVEH, C_FINDWANTED, C_PAROLE, C_TEXDEP, C_CLOTHSET, C_GIVEW, C_WSLOT, C_SETCUR,
    C_ADDUP, C_RMUP, C_REQUP, C_SETUPUP, C_FIXTYRE, C_FIXPANEL, C_FIXDOOR, C_PANELDMG, C_DOORDMG,
    C_GWHEEL, C_GPANEL, C_GDOOR, C_SWHEEL, C_SPANEL, C_SDOOR, C_SAVE, C_LOAD };
inline constexpr const char* CODE_NAMES[]{
    "modify_stat","open_file","close_file","load_line","find_weapon_type","model_info_by_name","anim_block_index","request_model","load_all_requested","set_deletable","timer_suspend","timer_resume",
    "valid_mod","find_player_vehicle","find_player_wanted","go_on_parole","texture_dependency","clothes_set","give_weapon","weapon_slot","set_current_weapon",
    "add_upgrade","remove_upgrade","request_upgrade","setup_upgrades","fix_tyre","fix_panel","fix_door","panel_damage","door_damage",
    "get_wheel","get_panel","get_door","set_wheel","set_panel","set_door","save_wb","load_wb"};
struct TR { uint32 c,a,b,d,e; };
inline std::array<TR,8192> tr{}; inline int ntr{}; inline bool trOverflow{};
inline void T(uint32 c,uint32 a=0,uint32 b=0,uint32 d=0,uint32 e=0) { if(ntr<int(tr.size()))tr[ntr++]={c,a,b,d,e}; else trOverflow=true; }
inline bool PtrArg(uint32 c,int q) {
    switch(c){
    case C_VALIDMOD: return q==1;
    case C_PAROLE: case C_CLOTHSET: case C_GIVEW: case C_WSLOT: case C_SETCUR: case C_ADDUP: case C_RMUP: case C_SETUPUP: case C_FIXTYRE: case C_FIXPANEL: case C_FIXDOOR:
    case C_PANELDMG: case C_DOORDMG: case C_GWHEEL: case C_GPANEL: case C_GDOOR: case C_SWHEEL: case C_SPANEL: case C_SDOOR: case C_SAVE: case C_LOAD: return q==0;
    default: return false; }
}
inline json TraceJson() {
    json r=json::array();
    for(int k=0;k<ntr;++k){ const auto& e=tr[k]; json x=json::array({CODE_NAMES[e.c]}); const uint32 v[]{e.a,e.b,e.d,e.e};
        for(int q=0;q<4;++q)x.push_back(PtrArg(e.c,q) ? Name(v[q]) : json(v[q]));
        r.push_back(x); }
    return r;
}

// ======================= valores de los stubs =======================
inline uint32 seedV{}, loadCounter{}, loadMods{}, loadBuy{};
inline std::array<uint8,32> texDep{}; inline std::array<uint8,24> dmgVals{};
inline uint32 Mix(uint32 x) { x^=seedV*0x9E3779B9u; x^=x>>15; x*=0x2C1B3C6Du; x^=x>>12; x*=0x297A2D39u; x^=x>>15; return x; }

// ======================= stubs =======================
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { const int k=id<0 ? int(*reinterpret_cast<uint8*>(0xB7CD74)) : id; return At<CPlayerPed*>(Plr(k),0); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool b) { T(C_FINDVEH,uint32(id),b); return reinterpret_cast<CVehicle*>(veh.data()); }
inline CWanted* __cdecl FakeFindPlayerWanted(int32 id) { T(C_FINDWANTED,uint32(id)); return reinterpret_cast<CWanted*>(wanted.data()); }
inline void __cdecl FakeModifyStat(uint32 stat,float v) { T(C_STAT,stat&0xFFFF,F(v)); }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m) { const int k=opens<32 ? opens : 31; if(opens<32){cursors[k]=0;++opens;} T(C_OPEN,SHash(p),SHash(m),uint32(k)); return Handle(k); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f) { T(C_CLOSE,uint32(HandleIndex(f))); return 0; }
inline char* __cdecl FakeLoadLine(FILESTREAM f) {
    const int k=HandleIndex(f);
    if(k<0||cursors[k]>=int(fileLines.size())){ T(C_LINE,uint32(k),0xFFFFFFFFu); return nullptr; }
    const auto& s=fileLines[size_t(cursors[k])]; T(C_LINE,uint32(k),uint32(cursors[k])); ++cursors[k];
    size_t n=std::min(s.size(),lineBuf.size()-1); std::memcpy(lineBuf.data(),s.data(),n); lineBuf[n]='\0';
    for(size_t q=0;q<n;++q){ if(uint8(lineBuf[q])<uint8(' ')||(!keepCommas&&lineBuf[q]==','))lineBuf[q]=' '; }   // como CFileLoader::LoadLine
    char* it=lineBuf.data(); while(*it&&uint8(*it)<=uint8(' '))++it; return it;
}
inline eWeaponType __cdecl FakeFindWeaponType(const char* s) { const uint32 h=Mix(SHash(s)); const int32 r=h%5==0 ? -1 : int32(h%47); T(C_WTYPE,SHash(s),uint32(r)); return eWeaponType(r); }
inline CBaseModelInfo* __cdecl FakeMiByName(const char* s,int32* idx) { const uint32 h=Mix(SHash(s)^0x55u); const bool found=h%4!=0; if(found&&idx)*idx=int32(MI_UP+h%40); T(C_MINAME,SHash(s),found,idx ? uint32(*idx) : 0xDEADu); return nullptr; }
inline int32 __cdecl FakeAnimBlock(const char* p) { const uint32 h=Mix(U(p)); const int32 r=h%3==0 ? -1 : int32(h%40); T(C_ANIMBLK,U(p),uint32(r)); return r; }
inline void __cdecl FakeRequestModel(int32 id,int32 fl) { T(C_REQ,uint32(id),uint32(fl)); }
inline void __cdecl FakeLoadAll(bool b) { T(C_LOADALL,b); }
inline void __cdecl FakeDeletable(int32 id) { T(C_DELETABLE,uint32(id)); }
inline void __cdecl FakeSuspend() { T(C_SUSPEND); }
inline void __cdecl FakeResume() { T(C_RESUME); }
inline bool __cdecl FakeValidMod(uint32 key,CVehicle* v) { const bool r=Mix(key*3u+1u)%3!=0; T(C_VALIDMOD,key,U(v),r); return r; }
inline void __fastcall FakeParole(void* self,void*) { T(C_PAROLE,U(self)); }
inline uint32 __cdecl FakeTexDep(uint32 part) { const uint32 r=texDep[part&31]; T(C_TEXDEP,part,r); return r; }
inline void __fastcall FakeClothSet(void* self,void*,uint32 a,uint32 b,uint32 part) { T(C_CLOTHSET,U(self),a,b,part); }
inline eWeaponSlot __fastcall FakeGiveWeapon(void* self,void*,uint32 type,uint32 ammo,bool like) { T(C_GIVEW,U(self),type,ammo,like); return eWeaponSlot(type%13); }
inline int32 __fastcall FakeWeaponSlot(void* self,void*,uint32 type) { const int32 r=int32(Mix(type)%13); T(C_WSLOT,U(self),type,uint32(r)); return r; }
inline void __fastcall FakeSetCurrent(void* self,void*,int32 slot) { T(C_SETCUR,U(self),uint32(slot)); }
inline void __fastcall FakeAddUpgrade(void* self,void*,int32 id) { T(C_ADDUP,U(self),uint32(id)); }
inline void __fastcall FakeRemoveUpgrade(void* self,void*,int32 id) { T(C_RMUP,U(self),uint32(id)); }
inline void __cdecl FakeRequestUpgrade(int32 id,int32 fl) { T(C_REQUP,uint32(id),uint32(fl)); }
inline void __fastcall FakeSetupUpgrades(void* self,void*) { T(C_SETUPUP,U(self)); }
inline void __fastcall FakeFixTyre(void* self,void*,uint32 w) { T(C_FIXTYRE,U(self),w&0xFF); }
inline void __fastcall FakeFixPanel(void* self,void*,uint32 node,uint32 panel) { T(C_FIXPANEL,U(self),node&0xFF,panel&0xFF); }
inline void __fastcall FakeFixDoor(void* self,void*,uint32 node,uint32 door) { T(C_FIXDOOR,U(self),node&0xFF,door&0xFF); }
inline void __fastcall FakePanelDamage(void* self,void*,uint32 panel,bool b) { T(C_PANELDMG,U(self),panel&0xFF,b); }
inline void __fastcall FakeDoorDamage(void* self,void*,uint32 door,bool b) { T(C_DOORDMG,U(self),door&0xFF,b); }
inline uint32 __fastcall FakeGetWheel(void* self,void*,uint32 w) { const uint32 r=dmgVals[(w&7)]; T(C_GWHEEL,U(self),w&0xFF,r); return r; }
inline uint32 __fastcall FakeGetPanel(void* self,void*,uint32 p) { const uint32 r=dmgVals[8+(p&7)]; T(C_GPANEL,U(self),p&0xFF,r); return r; }
inline uint32 __fastcall FakeGetDoor(void* self,void*,uint32 d) { const uint32 r=dmgVals[16+(d&7)]; T(C_GDOOR,U(self),d&0xFF,r); return r; }
inline void __fastcall FakeSetWheel(void* self,void*,uint32 w,uint32 s) { T(C_SWHEEL,U(self),w&0xFF,s&0xFF); }
inline void __fastcall FakeSetPanel(void* self,void*,uint32 p,uint32 s) { T(C_SPANEL,U(self),p&0xFF,s&0xFF); }
inline void __fastcall FakeSetDoor(void* self,void*,uint32 d,uint32 s) { T(C_SDOOR,U(self),d&0xFF,s&0xFF); }
inline bool __cdecl FakeSaveWB(void* p,int32 n) { uint64 h=1469598103934665603ull; for(int32 k=0;k<n&&k<0x4000;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} T(C_SAVE,U(p),uint32(n),uint32(h),uint32(h>>32)); return true; }
inline bool __cdecl FakeLoadWB(void* p,int32 n) { auto* b=static_cast<uint8*>(p);
    if(U(p)==0xA9A7D0&&n==4)*reinterpret_cast<int32*>(p)=int32(loadMods);
    else if(U(p)==0xA9A310&&n==4)*reinterpret_cast<uint32*>(p)=loadBuy;
    else for(int32 k=0;k<n;++k)b[k]=uint8(loadCounter++*37u+11u);
    T(C_LOAD,U(p),uint32(n)); return true; }

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
inline const std::array<Fn,30> fns{{
    {"CShopping","Buy",0x49BF70,0,-1,1024},{"CShopping","FindItem",0x49AD20,0,-1,256},
    {"CShopping","FindSection",0x49AE70,0,-1,512},{"CShopping","FindSectionInSection",0x49AF90,0,-1,256},
    {"CShopping","GetExtraInfo",0x49ADE0,0,-1,256},{"CShopping","GetItemIndex",0x49AB10,0,-1,256},
    {"CShopping","GetKey",0x49AB30,0,-1,512},{"CShopping","GetNameTag",0x49ADA0,0,-1,256},
    {"CShopping","GetNextSection",0x49AF10,0,-1,512},{"CShopping","GetPrice",0x49AD50,0,-1,512},
    {"CShopping","GetPriceSectionFromName",0x49AAD0,0,-1,256},{"CShopping","HasPlayerBought",0x49B5E0,0,-1,256},
    {"CShopping","IncrementStat",0x49BE70,0,-1,256},{"CShopping","IncrementStat2",0x49AFD0,0,-1,512},
    {"CShopping","Init",0x49C290,0,-1,128},{"CShopping","Load",0x5D3E40,0,-1,128},
    {"CShopping","LoadPrices",0x49B8D0,0,-1,512},{"CShopping","LoadShop",0x49BBE0,0,-1,512},
    {"CShopping","LoadStats",0x49B6A0,0,-1,256},{"CShopping","RemoveLoadedPrices",0x49AC90,0,-1,128},
    {"CShopping","RemoveLoadedShop",0x49AE30,0,-1,128},{"CShopping","RestoreClothesState",0x49B240,0,-1,128},
    {"CShopping","RestoreVehicleMods",0x49B3C0,0,-1,256},{"CShopping","Save",0x5D3DE0,0,-1,128},
    {"CShopping","SetCurrentProperty",0x49B1F0,0,-1,64},{"CShopping","SetPlayerHasBought",0x49B610,0,-1,256},
    {"CShopping","ShutdownForRestart",0x49B640,0,-1,128},{"CShopping","StoreClothesState",0x49B200,0,-1,128},
    {"CShopping","StoreVehicleMods",0x49B280,0,-1,256},{"CShopping","UpdateStats",0x49BEF0,0,-1,256},
}};

struct Region { uintptr_t at; size_t n; std::vector<uint8> saved; };
inline constexpr const char* SECTION_NAMES[]{"None","CarMods","CarPaintJobs","Furniture","Clothes","Haircuts","Tattoos","Gifts","Food","Weapons","Property"};
inline constexpr const char* STAT_NAMES[]{"fat","respect","sexy","health","stamina","calories","-","speed","Fat",""};
inline int GetPriceSectionFromNameSafe(const std::string& s) { for(int k=0;k<11;++k)if(!_stricmp(s.c_str(),SECTION_NAMES[k]))return k; return -1; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"shop30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"shop30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("shop30");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"shop30-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,30> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // estado global guardado
    std::vector<Region> regions{{SHOP,SHOPSZ,{}},{PLAYERS,2*PLSZ,{}},{0xB7CD74,1,{}},{0xBA6718,4,{}},{MI_PTRS+4*MI_FIRST,4*MI_COUNT,{}}};
    for(auto& r:regions)r.saved.assign(reinterpret_cast<uint8*>(r.at),reinterpret_cast<uint8*>(r.at)+r.n);
    auto RestoreAll=[&]{ for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.at),r.saved.data(),r.n); };
    auto AllRestored=[&]{ for(auto& r:regions)if(std::memcmp(reinterpret_cast<void*>(r.at),r.saved.data(),r.n))return false; return true; };

    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,auto stub){ void* s=reinterpret_cast<void*>(stub); if(exeAddr)plan.push_back({exeAddr,s}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,s}); } };
    add(0x56E210,uintptr_t(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),&FakeFindPlayerPed);
    add(0x56E0D0,uintptr_t(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),&FakeFindPlayerVehicle);
    add(0x56E230,uintptr_t(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),&FakeFindPlayerWanted);
    add(0x55D090,uintptr_t(&CStats::ModifyStat),&FakeModifyStat);
    add(0x538900,uintptr_t(&CFileMgr::OpenFile),&FakeOpenFile);
    add(0x5389D0,uintptr_t(&CFileMgr::CloseFile),&FakeCloseFile);
    add(0x536F80,uintptr_t(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),&FakeLoadLine);
    add(0x743D10,uintptr_t(&CWeaponInfo::FindWeaponType),&FakeFindWeaponType);
    add(0x4C5940,uintptr_t(static_cast<CBaseModelInfo*(*)(const char*,int32*)>(&CModelInfo::GetModelInfo)),&FakeMiByName);
    add(0x4D3990,uintptr_t(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),&FakeAnimBlock);
    add(0x4087E0,uintptr_t(&CStreaming::RequestModel),&FakeRequestModel);
    add(0x40EA10,uintptr_t(&CStreaming::LoadAllRequestedModels),&FakeLoadAll);
    add(0x409C10,uintptr_t(&CStreaming::SetModelIsDeletable),&FakeDeletable);
    add(0x408C70,uintptr_t(&CStreaming::RequestVehicleUpgrade),&FakeRequestUpgrade);
    add(0x5619D0,uintptr_t(&CTimer::Suspend),&FakeSuspend);
    add(0x561A00,uintptr_t(&CTimer::Resume),&FakeResume);
    add(0x49B010,uintptr_t(&IsValidModForVehicle),&FakeValidMod);
    add(0x5625A0,MAddr(&CWanted::ClearWantedLevelAndGoOnParole),&FakeParole);
    add(0x5A7EA0,uintptr_t(&CClothes::GetTextureDependency),&FakeTexDep);
    add(0x5A8050,MAddr(static_cast<void(CPedClothesDesc::*)(uint32,uint32,eClothesTexturePart)>(&CPedClothesDesc::SetTextureAndModel)),&FakeClothSet);
    add(0x5E6080,MAddr(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),&FakeGiveWeapon);
    add(0x5DF200,MAddr(static_cast<int32(CPed::*)(eWeaponType)>(&CPed::GetWeaponSlot)),&FakeWeaponSlot);
    add(0x5E61F0,MAddr(static_cast<void(CPed::*)(int32)>(&CPed::SetCurrentWeapon)),&FakeSetCurrent);
    add(0x6E3290,MAddr(&CVehicle::AddVehicleUpgrade),&FakeAddUpgrade);
    add(0x6DF930,MAddr(&CVehicle::RemoveVehicleUpgrade),&FakeRemoveUpgrade);
    add(0x6E3400,MAddr(&CVehicle::SetupUpgradesAfterLoad),&FakeSetupUpgrades);
    add(0x6A3580,MAddr(&CAutomobile::FixTyre),&FakeFixTyre);
    add(0x6A3670,MAddr(&CAutomobile::FixPanel),&FakeFixPanel);
    add(0x6A35A0,MAddr(&CAutomobile::FixDoor),&FakeFixDoor);
    add(0x6B1480,MAddr(&CAutomobile::SetPanelDamage),&FakePanelDamage);
    add(0x6B1600,MAddr(&CAutomobile::SetDoorDamage),&FakeDoorDamage);
    add(0x6C21B0,MAddr(&CDamageManager::GetWheelStatus),&FakeGetWheel);
    add(0x6C2180,MAddr(&CDamageManager::GetPanelStatus),&FakeGetPanel);
    add(0x6C2230,MAddr(&CDamageManager::GetDoorStatus),&FakeGetDoor);
    add(0x6C21A0,MAddr(&CDamageManager::SetWheelStatus),&FakeSetWheel);
    add(0x6C2150,MAddr(&CDamageManager::SetPanelStatus),&FakeSetPanel);
    add(0x6C21C0,MAddr(static_cast<void(CDamageManager::*)(eDoors,eDoorStatus)>(&CDamageManager::SetDoorStatus)),&FakeSetDoor);
    add(0x5D1270,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),&FakeSaveWB);
    add(0x5D1300,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),&FakeLoadWB);
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }

    std::array<bool,30> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        auto& focus=*reinterpret_cast<uint8*>(0xB7CD74);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto rnd=[&](uint32 n){ return next()%n; };
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A0Bu; if(!st)st=1;
                    ntr=0; trOverflow=false; seedV=uint32(i)*7919u+uint32(f); loadCounter=0; opens=0; cursors.fill(0);
                    loadMods=rnd(6)==0 ? rnd(25) : rnd(21); loadBuy=rnd(4)==0 ? rnd(600) : rnd(80);
                    keepCommas=rnd(3)==0;
                    for(auto& t:texDep)t=uint8(rnd(11)); for(auto& d:dmgVals)d=uint8(rnd(4)==0 ? next() : rnd(5));

                    // ---- claves y nombres ----
                    std::array<uint32,48> pool{};
                    for(size_t q=0;q<pool.size();++q){ const uint32 k=rnd(6); pool[q]=k==0 ? rnd(8) : k==1 ? uint32(MI_UP+rnd(32)) : k==2 ? rnd(50) : next(); }
                    pool[0]=48; pool[1]=uint32(MI_UP+rnd(32));
                    auto key=[&]{ return pool[rnd(uint32(pool.size()))]; };
                    auto word=[&](uint32 minLen,uint32 maxLen){ static const char al[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"; std::string s; const uint32 n=minLen+rnd(maxLen-minLen+1);
                        for(uint32 q=0;q<n;++q)s+=al[rnd(63)]; if(s.starts_with("end")||s.starts_with("section")||s=="type"||s=="item")s="x"+s; return s; };
                    auto caseMix=[&](std::string s){ for(auto& c:s)if(rnd(3)==0)c=char(std::isupper(uint8(c)) ? std::tolower(uint8(c)) : std::toupper(uint8(c))); return s; };
                    auto setArg=[&](std::array<char,64>& a,const std::string& s){ a.fill(0); std::memcpy(a.data(),s.data(),std::min<size_t>(s.size(),63)); };

                    // ---- globales de CShopping aleatorias ----
                    auto randShop=[&]{
                        auto* r=reinterpret_cast<uint8*>(SHOP); for(size_t q=0;q<SHOPSZ;++q)r[q]=uint8(next());
                        auto& keys=*reinterpret_cast<std::array<uint32,560>*>(0xA97D90);
                        for(auto& k:keys)k=rnd(3)==0 ? key() : rnd(2) ? 0u : next();
                        auto* bought=reinterpret_cast<uint8*>(0xA972A0); for(int q=0;q<560;++q)bought[q]=uint8(rnd(2));
                        *reinterpret_cast<uint8*>(0xA9729F)=uint8(rnd(2));
                        auto* sm=reinterpret_cast<int8*>(0xA974D0); for(int q=0;q<560;++q){ sm[4*q]=int8(int(rnd(7))-1); sm[4*q+1]=int8(int(rnd(21))-10); sm[4*q+2]=int8(int(rnd(7))-1); sm[4*q+3]=int8(int(rnd(21))-10); }
                        // ms_statModifiers[-1] = ms_bHasBought[556..559]: modificadores validos
                        bought[556]=uint8(int8(int(rnd(7))-1)); bought[557]=uint8(rnd(9)); bought[558]=uint8(int8(int(rnd(7))-1)); bought[559]=uint8(int8(int(rnd(9))-4));
                        auto* mods=reinterpret_cast<uint32*>(0xA98650); for(int q=0;q<20;++q){ mods[2*q]=rnd(2) ? key() : next(); mods[2*q+1]=rnd(50000); }
                        mods[2*18]=rnd(4)==0 ? 0xFFFFFFFFu : rnd(30); mods[2*18+1]=rnd(18);   // ms_prices[-1]: tipo de ropa valido
                        *reinterpret_cast<int32*>(0xA9A7D0)=int32(rnd(21));
                        auto* pr=reinterpret_cast<uint8*>(0xA986F0);
                        for(int q=0;q<300;++q){ uint8* p=pr+0x18*q; At<uint32>(p,0)=rnd(4) ? key() : next(); At<int32>(p,4)=rnd(8)==0 ? -int32(rnd(5000)) : int32(rnd(60000));
                            At<uint32>(p,8)=rnd(2) ? rnd(60) : next(); At<uint32>(p,0xC)=rnd(18); }
                        *reinterpret_cast<int32*>(0xA9A7CC)=rnd(10)==0 ? -int32(rnd(3)) : int32(rnd(4)==0 ? rnd(300) : rnd(40));
                        *reinterpret_cast<int32*>(0xA9A7F0)=int32(rnd(300)); *reinterpret_cast<uint32*>(0xA9A310)=rnd(561);
                        *reinterpret_cast<int32*>(0xA9A7C8)=int32(rnd(11));
                        { std::string s=rnd(3)==0 ? std::string(SECTION_NAMES[rnd(11)]) : word(0,20); std::memset(reinterpret_cast<void*>(0xA9A7D8),0,24); std::memcpy(reinterpret_cast<void*>(0xA9A7D8),s.data(),s.size()); }
                        *reinterpret_cast<uint8*>(0xA97298)=uint8(rnd(2));
                        *reinterpret_cast<uint32*>(0xBA6718)=rnd(4);
                    };

                    // ---- jugador, ropa, vehiculo y modelos falsos ----
                    auto setWorld=[&]{
                        std::memset(Plr(0),0,2*PLSZ);
                        for(int k=0;k<2;++k){ uint8* p=Ped(k); std::memset(p,0,PEDSZ); for(int q=0x540;q<0x550;++q)p[q]=uint8(next());
                            At<void*>(p,0x480)=PData(k); std::memset(PData(k),0,PDSZ); At<void*>(PData(k),4)=Clo(k);
                            for(size_t q=0;q<CLSZ;q+=4)At<uint32>(Clo(k),q)=rnd(3)==0 ? 0u : rnd(2) ? key() : next();
                            uint8* pl=Plr(k); At<void*>(pl,0)=p; At<int32>(pl,0xB8)=int32(rnd(100000))-5000; pl[0x150]=uint8(next()); }
                        focus=uint8(rnd(2));
                        std::memset(veh.data(),0,VEHSZ); for(int q=0x5A0;q<0x5C0;++q)veh[q]=uint8(next());
                        At<uint16>(veh.data(),0x22)=uint16(MI_VEH+rnd(4)); At<uint32>(veh.data(),0x590)=rnd(3)==0 ? 1+rnd(10) : 0;
                        for(int q=0;q<15;++q)At<int16>(veh.data(),0x43A+2*q)=rnd(3)==0 ? int16(-1) : int16(MI_UP+rnd(40));
                        std::memset(wanted.data(),0,WSZ);
                        for(int k=0;k<32;++k){ uint8* m=miUp.data()+k*MIUPSZ; std::memset(m,0,MIUPSZ); At<uint16>(m,0x12)=uint16(next()); MiPtr(MI_UP+k)=reinterpret_cast<CBaseModelInfo*>(m); }
                        for(int k=0;k<4;++k){ uint8* m=miVeh.data()+k*MIVSZ; std::memset(m,0,MIVSZ); uint8* s=vstruct.data()+k*VSTSZ; At<void*>(m,0x5C)=s;
                            for(int u=0;u<32;++u){ static const int32 parents[]{2,12,13,16,17,2,12,16,0,5,7,9,13,17,4,1}; At<int32>(s,0xB4+0x20*u+0x1C)=rnd(5)==0 ? int32(rnd(30)) : parents[rnd(16)]; }
                            MiPtr(MI_VEH+k)=reinterpret_cast<CBaseModelInfo*>(m); }
                        auto* dmg=reinterpret_cast<int8*>(0xA97258); for(int q=0;q<25;++q)dmg[q]=int8(rnd(3)==0 ? int(next()&0xFF)-128 : int(rnd(5)));
                        auto* sm=reinterpret_cast<int16*>(0xA97274); for(int q=0;q<15;++q)sm[q]=rnd(3)==0 ? int16(-1) : int16(MI_UP+rnd(40));
                    };

                    // ---- shopping.dat generado ----
                    struct Sec { int s; std::string name; std::vector<std::string> models; };
                    std::vector<Sec> secs; std::vector<std::string> shopNames;
                    auto sep=[&]{ static const char* const seps[]{" ","\t",", "," ,","\t "," ,\t"}; return std::string(seps[rnd(6)]); };
                    auto itemLine=[&](int s,const std::string& model){
                        std::vector<std::string> t{model, rnd(4)==0 ? word(8,11) : word(1,7)};
                        if(s==4||s==5){ t.push_back(word(2,8)); t.push_back(std::to_string(rnd(18))); }
                        else if(s==6){ t.push_back(rnd(3)==0 ? std::string("-") : std::to_string(rnd(12))); t.push_back(word(2,8)); }
                        else if(s==9){ t.push_back(std::to_string(rnd(500))); }
                        for(int q=0;q<2;++q){ t.push_back(STAT_NAMES[rnd(9)]); if(t.back().empty())t.back()="sexy"; t.push_back(std::to_string(int(rnd(21))-10)); }
                        t.push_back(rnd(8)==0 ? "-"+std::to_string(rnd(900)) : std::to_string(rnd(60000)));
                        if(rnd(6)==0)t.push_back(word(1,4));
                        std::string l=rnd(4)==0 ? std::string(rnd(2) ? "  " : "\t") : std::string();
                        for(size_t q=0;q<t.size();++q){ if(q)l+=sep(); l+=t[q]; } return l; };
                    auto noise=[&](std::vector<std::string>& out){ const uint32 k=rnd(10); if(k==0)out.push_back("# "+word(0,12)); else if(k==1)out.push_back(""); else if(k==2)out.push_back("   "); else if(k==3)out.push_back("#section "+word(1,6)); };
                    auto makeFile=[&](bool pricesLast,int maxItems,bool statsSafe=false){
                        fileLines.clear(); secs.clear(); shopNames.clear();
                        // secciones de precios: subconjunto y orden aleatorios (puede haber nombres desconocidos)
                        std::vector<int> order{1,2,3,4,5,6,7,8,9,10}; for(size_t q=order.size();q>1;--q)std::swap(order[q-1],order[rnd(uint32(q))]);
                        const uint32 nsec=1+rnd(10);
                        for(uint32 q=0;q<nsec;++q){ Sec sc{order[q],caseMix(SECTION_NAMES[order[q]]),{}}; const uint32 n=rnd(4)==0 ? 0 : rnd(uint32(maxItems)+1);
                            for(uint32 m=0;m<n;++m)sc.models.push_back(sc.s==9&&rnd(3)==0 ? std::string("armour") : word(3,10));
                            secs.push_back(std::move(sc)); }
                        // LoadStats: la primera seccion con elementos no es CarPaintJobs (el EXE usaria una clave sin inicializar)
                        if(statsSafe){ if(secs[0].s==2){ secs[0].s=4; secs[0].name=caseMix(SECTION_NAMES[4]); } if(secs[0].models.empty())secs[0].models.push_back(word(3,10)); }
                        std::vector<std::string> prices, shops;
                        prices.push_back(rnd(5)==0 ? "section\tprices" : "section prices");
                        for(auto& sc:secs){ noise(prices); prices.push_back((rnd(3)==0 ? "  " : "")+std::string("section ")+sc.name);
                            for(auto& m:sc.models){ noise(prices); prices.push_back(itemLine(sc.s,m)); } noise(prices); prices.push_back(rnd(4)==0 ? "end section" : "end"); }
                        prices.push_back("end");
                        shops.push_back("section shops");
                        const uint32 nshops=1+rnd(6);
                        for(uint32 q=0;q<nshops;++q){ const std::string sn=word(3,14); shopNames.push_back(sn); noise(shops); shops.push_back("section "+sn);
                            const uint32 nl=rnd(12);
                            for(uint32 m=0;m<nl;++m){ noise(shops); const uint32 k=rnd(5);
                                if(k==0||m==0){ const auto& sc=secs[rnd(uint32(secs.size()))]; shops.push_back("type"+sep().substr(0,1)+sc.name); }
                                else if(k==4)shops.push_back(word(2,6)+" "+word(2,6));
                                else { const auto& sc=secs[rnd(uint32(secs.size()))]; shops.push_back(std::string("item")+(rnd(2) ? " " : "\t")+(sc.models.empty()||rnd(5)==0 ? word(3,8) : sc.models[rnd(uint32(sc.models.size()))])); } }
                            shops.push_back("end"); }
                        shops.push_back("end");
                        if(rnd(3)==0){ fileLines.push_back("# shopping.dat"); fileLines.push_back(""); }
                        auto append=[&](const std::vector<std::string>& v){ for(auto& l:v)fileLines.push_back(l); };
                        if(pricesLast||rnd(2)){ append(shops); append(prices); } else { append(prices); append(shops); }
                    };

                    void* const A=reinterpret_cast<void*>(fn.address);
                    json ret=nullptr; bool switches=false;
                    auto on=[&]{ for(auto* h:hooks)h->State(candidate); switches=ReversingProbe::JumpsToDll(fn.address)==candidate; };
                    auto off=[&]{ for(auto* h:hooks)h->State(false); };
                    json extra=json::object();
                    randShop(); setWorld(); fileLines.clear();
                    auto& numPrices=*reinterpret_cast<int32*>(0xA9A7CC);
                    auto* pr=reinterpret_cast<uint8*>(0xA986F0); auto* keys=reinterpret_cast<uint32*>(0xA97D90);
                    // clave buscada: casi siempre una de las de precios/claves
                    auto pickPriceKey=[&]{ const int n=std::clamp(numPrices,0,300); return (n>0&&rnd(5)!=0) ? At<uint32>(pr+0x18*rnd(uint32(n)),0) : key(); };
                    auto pickItemKey=[&]{ return rnd(5)!=0 ? keys[rnd(560)] : key(); };

                    if(nm=="GetPriceSectionFromName"){ setArg(arg1,rnd(5)==0 ? word(0,12) : caseMix(SECTION_NAMES[rnd(11)]));
                        on(); ret=reinterpret_cast<int32(__cdecl*)(const char*)>(A)(arg1.data()); off(); }
                    else if(nm=="GetItemIndex"){ const uint32 k=pickItemKey(); on(); ret=reinterpret_cast<int32(__cdecl*)(uint32)>(A)(k); off(); }
                    else if(nm=="GetKey"){ setArg(arg1,rnd(6)==0 ? std::string("armour") : word(1,12)); const int32 s=rnd(10)==0 ? int32(next()) : int32(rnd(11));
                        on(); ret=reinterpret_cast<uint32(__cdecl*)(const char*,int32)>(A)(arg1.data(),s); off(); }
                    else if(nm=="FindItem"||nm=="GetPrice"||nm=="GetNameTag"||nm=="HasPlayerBought"||nm=="SetPlayerHasBought"){
                        const uint32 k=nm=="HasPlayerBought"||nm=="SetPlayerHasBought" ? pickItemKey() : pickPriceKey();
                        if(nm=="GetPrice"&&rnd(4)==0)*reinterpret_cast<uint32*>(0xBA6718)=rnd(4);
                        on();
                        if(nm=="FindItem")ret=reinterpret_cast<int32(__cdecl*)(uint32)>(A)(k);
                        else if(nm=="GetPrice")ret=reinterpret_cast<int32(__cdecl*)(uint32)>(A)(k);
                        else if(nm=="GetNameTag")ret=Name(U(reinterpret_cast<const char*(__cdecl*)(uint32)>(A)(k)));
                        else if(nm=="HasPlayerBought")ret=uint32(reinterpret_cast<bool(__cdecl*)(uint32)>(A)(k));
                        else reinterpret_cast<void(__cdecl*)(uint32)>(A)(k);
                        off(); }
                    else if(nm=="GetExtraInfo"){ const uint32 k=pickPriceKey(); const int32 idx=rnd(6)==0 ? int32(next()) : int32(rnd(3));
                        on(); ret=reinterpret_cast<int32(__cdecl*)(uint32,int32)>(A)(k,idx); off(); }
                    else if(nm=="IncrementStat"){ const uint32 s=rnd(4)==0 ? 0xFFFFFFFFu : rnd(6); const int32 c=rnd(5)==0 ? int32(next()%2001)-1000 : int32(rnd(21))-10;
                        on(); reinterpret_cast<void(__cdecl*)(uint32,int32)>(A)(s,c); off(); }
                    else if(nm=="IncrementStat2"){ static const int32 stats[]{-1,66,80,21,22,245,335,0x10042,0x50,int32(0xFFFF0042u)}; const int32 s=rnd(3)==0 ? int32(next()) : stats[rnd(10)];
                        const int32 c=rnd(5)==0 ? int32(next()%200001)-100000 : int32(rnd(21))-10;
                        on(); reinterpret_cast<void(__cdecl*)(int32,int32)>(A)(s,c); off(); }
                    else if(nm=="UpdateStats"){ const uint32 idx=rnd(8)==0 ? 0xFFFFFFFFu : rnd(560); const bool inc=rnd(2)!=0;
                        on(); reinterpret_cast<void(__cdecl*)(uint32,bool)>(A)(idx,inc); off(); }
                    else if(nm=="FindSection"||nm=="FindSectionInSection"||nm=="GetNextSection"){
                        makeFile(rnd(2)!=0,rnd(3)==0 ? 8 : 3);
                        if(rnd(4)==0){ fileLines.insert(fileLines.begin()+rnd(uint32(fileLines.size())),rnd(2) ? "sections "+word(2,6) : "Section "+word(2,6)); }
                        cursors[0]=rnd(3)==0 ? int(rnd(uint32(fileLines.size()))) : 0; opens=1;
                        auto pickName=[&]{ const uint32 k=rnd(6); if(k==0)return caseMix("prices"); if(k==1)return caseMix("shops");
                            if(k==2&&!shopNames.empty())return caseMix(shopNames[rnd(uint32(shopNames.size()))]); if(k==3)return word(2,8); return caseMix(secs[rnd(uint32(secs.size()))].name); };
                        setArg(arg1,pickName()); setArg(arg2,pickName());
                        on();
                        if(nm=="FindSection")ret=reinterpret_cast<bool(__cdecl*)(FILESTREAM,const char*)>(A)(Handle(0),arg1.data());
                        else if(nm=="FindSectionInSection")ret=reinterpret_cast<bool(__cdecl*)(FILESTREAM,const char*,const char*)>(A)(Handle(0),arg1.data(),arg2.data());
                        else { const char* s=reinterpret_cast<const char*(__cdecl*)(FILESTREAM)>(A)(Handle(0)); ret=Name(U(s)); if(s)extra["name"]=std::string(s); }
                        off(); }
                    else if(nm=="LoadPrices"){
                        const bool missing=rnd(8)==0; makeFile(missing,rnd(4)==0 ? 30 : 8);
                        const std::string name=missing ? (rnd(2) ? word(3,9) : caseMix(SECTION_NAMES[rnd(11)])) : caseMix(secs[rnd(uint32(secs.size()))].name);
                        setArg(arg1,name);
                        if(rnd(6)==0)*reinterpret_cast<int32*>(0xA9A7C8)=int32(GetPriceSectionFromNameSafe(name));
                        on(); reinterpret_cast<void(__cdecl*)(const char*)>(A)(arg1.data()); off(); }
                    else if(nm=="LoadShop"){
                        makeFile(rnd(2)!=0,rnd(4)==0 ? 20 : 6);
                        const uint32 k=rnd(8); std::string name=k==0 ? caseMix("bought") : k==1 ? word(3,10) : caseMix(shopNames[rnd(uint32(shopNames.size()))]);
                        setArg(arg1,name);
                        if(rnd(6)==0){ std::memset(reinterpret_cast<void*>(0xA9A7D8),0,24); const auto s=caseMix(name); std::memcpy(reinterpret_cast<void*>(0xA9A7D8),s.data(),std::min<size_t>(s.size(),23)); }
                        if(k==0){ numPrices=int32(rnd(4)==0 ? rnd(300) : rnd(40)); for(int q=0;q<numPrices;++q)if(rnd(2))keys[rnd(556)]=At<uint32>(pr+0x18*q,0); }
                        on(); reinterpret_cast<void(__cdecl*)(const char*)>(A)(arg1.data()); off(); }
                    else if(nm=="LoadStats"||nm=="Init"){
                        makeFile(rnd(2)!=0,rnd(6)==0 ? 50 : 10,true);
                        if(rnd(5)==0){ for(auto& l:fileLines)if(l.starts_with("section ")&&l!="section prices"&&l!="section shops"&&rnd(4)==0){ l="section "+word(3,8); } }
                        on(); reinterpret_cast<void(__cdecl*)()>(A)(); off(); }
                    else if(nm=="RemoveLoadedPrices"||nm=="RemoveLoadedShop"||nm=="ShutdownForRestart"||nm=="StoreClothesState"||nm=="RestoreClothesState"||nm=="StoreVehicleMods"||nm=="RestoreVehicleMods"){
                        on(); reinterpret_cast<void(__cdecl*)()>(A)(); off(); }
                    else if(nm=="Save"||nm=="Load"){ if(nm=="Save"&&rnd(4)==0)*reinterpret_cast<int32*>(0xA9A7D0)=int32(rnd(25));
                        on(); ret=reinterpret_cast<bool(__cdecl*)()>(A)(); off(); }
                    else if(nm=="SetCurrentProperty"){ void* p=rnd(4)==0 ? nullptr : reinterpret_cast<void*>(uintptr_t(next()));
                        on(); reinterpret_cast<void(__cdecl*)(void*)>(A)(p); off(); }
                    else if(nm=="Buy"){
                        const int32 sec=int32(rnd(12)==0 ? rnd(11) : 1+rnd(9)); *reinterpret_cast<int32*>(0xA9A7C8)=sec;
                        numPrices=int32(1+rnd(rnd(4)==0 ? 299 : 30));
                        for(int q=0;q<numPrices;++q){ uint8* p=pr+0x18*q; uint32& k=At<uint32>(p,0);
                            if(sec==1)k=uint32(MI_UP+rnd(32)); else if(sec==9)k=rnd(4)==0 ? 48u : rnd(47); else k=key();
                            if(rnd(3))keys[rnd(556)]=k; }
                        uint32 k=pickPriceKey(); if(sec==1&&(k<uint32(MI_UP)||k>=uint32(MI_UP+32)))k=uint32(MI_UP+rnd(32));
                        const int32 extraInfo=int32(next());
                        on(); reinterpret_cast<void(__cdecl*)(uint32,int32)>(A)(k,extraInfo); off(); }

                    // ---- volcado ----
                    auto* S=reinterpret_cast<uint8*>(SHOP);
                    json r={{"ret",ret},{"switches",switches},{"extra",extra},{"trace",TraceJson()},{"overflow",trOverflow},
                            {"files",json::array({opens,json(std::vector<int>(cursors.begin(),cursors.end()))})},
                            {"counts",json::array({*reinterpret_cast<uint32*>(0xA9A310),*reinterpret_cast<int32*>(0xA9A7CC),*reinterpret_cast<int32*>(0xA9A7D0),*reinterpret_cast<int32*>(0xA9A7F0),
                                *reinterpret_cast<int32*>(0xA9A7C8),*reinterpret_cast<uint8*>(0xA97298),*reinterpret_cast<uint32*>(0xA97294)})},
                            {"shop_loaded",ReversingProbe::Bytes(reinterpret_cast<void*>(0xA9A7D8),24)},
                            {"damage",ReversingProbe::Bytes(S,25)},{"stored_mods",ReversingProbe::Bytes(reinterpret_cast<void*>(0xA97274),30)},
                            {"bought",Hash(reinterpret_cast<void*>(0xA9729F),561)},{"stat_mods",Hash(reinterpret_cast<void*>(0xA974D0),0x8C0)},{"keys",Hash(reinterpret_cast<void*>(0xA97D90),0x8C0)},
                            {"price_mods",Hash(reinterpret_cast<void*>(0xA98650),0xA0)},{"prices",Hash(reinterpret_cast<void*>(0xA986F0),0x1C20)},
                            {"prices_head",ReversingProbe::Bytes(reinterpret_cast<void*>(0xA986F0),0x60)},
                            {"contents",Hash(reinterpret_cast<void*>(0xA9A318),0x4B0)},{"stored_clothes",Hash(reinterpret_cast<void*>(0xA9A810),0x78)},{"shop_all",Hash(S,SHOPSZ)},
                            {"players",Hash(Plr(0),2*PLSZ)},{"money",json::array({At<int32>(Plr(0),0xB8),At<int32>(Plr(1),0xB8)})},
                            {"peds",json::array({Hash(Ped(0),PEDSZ),Hash(Ped(1),PEDSZ),F(At<float>(Ped(0),0x548)),F(At<float>(Ped(1),0x548))})},
                            {"objs",json::array({Hash(pdatas.data(),pdatas.size()),Hash(clothes.data(),clothes.size()),Hash(veh.data(),VEHSZ),Hash(wanted.data(),WSZ),
                                Hash(miUp.data(),miUp.size()),Hash(miVeh.data(),miVeh.size()),Hash(vstruct.data(),vstruct.size()),Hash(arg1.data(),arg1.size())+Hash(arg2.data(),arg2.size())})},
                            {"veh_upgrades",ReversingProbe::Bytes(veh.data()+0x43A,30)}};
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
