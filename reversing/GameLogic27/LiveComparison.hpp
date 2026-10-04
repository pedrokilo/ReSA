#pragma once
// Banco dentro del motor: CGameLogic (lista de 27, 27 funciones). Original (hooks de la clase desactivados) frente a
// candidato con fixtures deterministas e identicos. Peatones (con inteligencia y CPlayerPedData dentro de
// CWorld::Players), vehiculos, matrices, CWanted, CPedClothesDesc, fuego, entrada/salida y tareas son falsos con
// offsets crudos y vtables trazadas (destructor y Teleport). CWorld::Players se sustituye por el fixture durante la
// ejecucion y se restaura. Se simulan y trazan: FindPlayerPed/Vehicle/Coors/Wanted (sin traza), estadisticas, reloj,
// pickups, camara (fundidos, desenfoque, posicion), CPad::StopShaking, mensajes y ayuda del HUD, TheText.Get, CWorld
// (Add/Remove, ClearExcitingStuffFromArea, linea de vision, suelo), referencias (Register/CleanUpOldReference y
// RemoveReferencesToPlayer), CPlayerInfo/CPlayerPed/CPed (armas, estados, Say, huesos, en pantalla), tareas (nadar,
// jetpack, buscar tarea), CWanted::Reset, desenfoque, fuego, jetpack de audio, conocidos, scripts, streaming, garajes,
// tiendas, posefectos, ciclo de tiempo, ropa, zonas de audio, escaner de policia, agua, CTimer (Stop/Update/Suspend/
// Resume), plano lejano de la camara RW, peticiones de objetos, colocacion en carretera, entradas/salidas, puntos de
// reinicio, punto de mira, rand, RemovePassenger, Test2DLineAgainst2DLine y el bufer de guardado (nunca ficheros).
// Los cuerpos de la DLL se resuelven antes de parchear y nunca se parchea la direccion de un stub. La traza es un
// array fijo (sin reservas durante la llamada) que se convierte a json despues. Todas las globales de CGameLogic,
// CWorld::Players, el jugador en foco, CTimer, el reloj, la camara, los mandos (CPad::Pads), las banderas de
// CRestart, cuentas atras y demas globales tocadas se guardan y se restauran en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "GameLogic.h"
#include "World.h"
#include "PlayerInfo.h"
#include "Stats.h"
#include "Clock.h"
#include "Pickups.h"
#include "Camera.h"
#include "Pad.h"
#include "Messages.h"
#include "Hud.h"
#include "Text/Text.h"
#include "CarCtrl.h"
#include "References.h"
#include "PedIntelligence.h"
#include "Tasks/TaskManager.h"
#include "Wanted.h"
#include "MBlur.h"
#include "Fire.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "PedType.h"
#include "Scripts/TheScripts.h"
#include "Streaming.h"
#include "Game.h"
#include "Garages.h"
#include "Shopping.h"
#include "PostEffects.h"
#include "TimeCycle.h"
#include "PedClothesDesc.h"
#include "Clothes.h"
#include "Audio/AudioZones.h"
#include "Audio/AudioEngine.h"
#include "WaterLevel.h"
#include "Timer.h"
#include "Renderer.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Automobile.h"
#include "EntryExit.h"
#include "Restart.h"
#include "WeaponEffects.h"
#include "WeaponInfo.h"
#include "General.h"
#include "Collision/Collision.h"
#include "GenericGameStorage.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace GameLogic27Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T> inline T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// ======================= objetos falsos =======================
constexpr size_t PEDSZ=0x800, INTSZ=0x300, WSZ=0x300, CLSZ=0x80, VEHSZ=0x800, MATSZ=0x48, SMALL=0x40;
constexpr uintptr_t PLAYERS=0xB7CD98; constexpr size_t PLSZ=0x190;
alignas(16) inline std::array<uint8,3*PEDSZ> peds{};       // 0 y 1: jugadores, 2: otro peaton
alignas(16) inline std::array<uint8,3*INTSZ> intels{};
alignas(16) inline std::array<uint8,2*WSZ> wanteds{};
alignas(16) inline std::array<uint8,2*CLSZ> clothes{};
alignas(16) inline std::array<uint8,2*VEHSZ> vehs{};
alignas(16) inline std::array<uint8,6*MATSZ> mats{};       // 0-2 peatones, 3-4 vehiculos
alignas(16) inline std::array<uint8,SMALL> fireObj{}, enexObj{}, taskObj{}, acqObj{};
alignas(16) inline std::array<CVector,16> points{};
alignas(16) inline std::array<std::array<GxtChar,16>,6> texts{}; // WRN_2P, WRN1_2P, WRN2_2P, BUSTED, SKIP, otra
alignas(16) inline CVector camPos{};
inline std::array<void*,64> vt{};
inline uint8* Ped(int k) { return peds.data()+size_t(k)*PEDSZ; }
inline uint8* Intel(int k) { return intels.data()+size_t(k)*INTSZ; }
inline uint8* Wnt(int k) { return wanteds.data()+size_t(k)*WSZ; }
inline uint8* Clo(int k) { return clothes.data()+size_t(k)*CLSZ; }
inline uint8* Veh(int k) { return vehs.data()+size_t(k)*VEHSZ; }
inline uint8* Mat(int k) { return mats.data()+size_t(k)*MATSZ; }
inline uint8* Plr(int k) { return reinterpret_cast<uint8*>(PLAYERS)+size_t(k)*PLSZ; }
inline int PedIndex(const void* p) { for(int k=0;k<3;++k)if(p==Ped(k))return k; return -1; }

struct Rg { uintptr_t p; size_t n; size_t stride; const char* nm; };
inline std::vector<Rg> NameRegions() {
    return {{uintptr_t(peds.data()),peds.size(),PEDSZ,"ped"},{uintptr_t(intels.data()),intels.size(),INTSZ,"intel"},{uintptr_t(wanteds.data()),wanteds.size(),WSZ,"wanted"},
            {uintptr_t(clothes.data()),clothes.size(),CLSZ,"clothes"},{uintptr_t(vehs.data()),vehs.size(),VEHSZ,"veh"},{uintptr_t(mats.data()),mats.size(),MATSZ,"mat"},
            {uintptr_t(fireObj.data()),SMALL,0,"fire"},{uintptr_t(enexObj.data()),SMALL,0,"enex"},{uintptr_t(taskObj.data()),SMALL,0,"task"},{uintptr_t(acqObj.data()),SMALL,0,"acq"},
            {uintptr_t(points.data()),sizeof(points),12,"point"},{uintptr_t(texts.data()),sizeof(texts),16,"text"},{uintptr_t(&camPos),12,0,"campos"},
            {PLAYERS,2*PLSZ,PLSZ,"player"},{0x96A840,0x2E8,0,"gl"},{0xB73458,2*0x134,0x134,"pad"},{0xB6F028,0xD78,0,"camera"},{0xB6BC90,4,0,"audio"},{0xC1B340,4,0,"text_obj"}};
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
    C_STAT_GET, C_STAT_INC, C_STAT_SET, C_STAT_RESPAWN, C_STAT_RESPECT, C_STAT_SEX, C_CLOCK, C_PICKUPS,
    C_FADE_COL, C_FADE, C_BLUR, C_CAMPOS, C_CAM_BEHIND, C_CAM_JUMPCUT, C_PAD_SHAKE,
    C_MSG_CLEAR, C_BIGMSG, C_HELP, C_HUDMSG, C_WASTED, C_TEXT, C_INTERESTING, C_EXCITING, C_WREMOVE, C_WADD, C_LOS, C_GROUNDZ, C_REFS,
    C_GETPOS, C_KILL, C_ARREST, C_SAFE, C_INFO, C_ADREN, C_SPRINT, C_TARGET, C_INITSTATE, C_SETWANTED, C_PICK2P,
    C_CLEARW, C_GIVEW, C_BONE, C_ANIMS, C_HEADRATE, C_SAY, C_ONSCREEN, C_CLEANREF, C_REGREF, C_SWIM, C_JET, C_FINDTASK,
    C_WRESET, C_DRUNKBLUR, C_EXTING, C_JETOFF, C_ACQ, C_CLEARSPACE, C_MISSION,
    C_RMBUILD, C_ZONEMODELS, C_FLUSH, C_DELRW, C_RMUNUSED, C_LSCOL, C_LSCENE, C_TIDY, C_GARAGE_DIED, C_GARAGE_HIDE, C_SHOP, C_PFX_RESET,
    C_EXTRACOL, C_FARCLIP, C_CLOTH_SET, C_REBUILD, C_REBUILD_IF, C_AZONES, C_SCANNER, C_WATER, C_TSTOP, C_TUPDATE, C_TSUSPEND, C_TRESUME,
    C_FARPLANE, C_REQDIR, C_BIKE_ROAD, C_CAR_ROAD, C_ENEX, C_HOSP, C_POLICE, C_CROSSHAIR, C_RAND, C_REMPASS, C_LINE, C_SAVE, C_LOAD,
    C_DTOR, C_TELEPORT, C_TRAP };
inline constexpr const char* CODE_NAMES[]{
    "get_stat","inc_stat","set_stat","stats_respawn","respect","sex_appeal","set_clock","pickups_pass_time",
    "fade_colour","fade","motion_blur","game_cam_pos","cam_behind","cam_jump_cut","stop_shaking",
    "clear_messages","big_message","help","hud_message","reset_wasted","text_get","clear_interesting","clear_exciting","world_remove","world_add","line_of_sight","ground_z","remove_refs_player",
    "info_get_pos","kill_player","arrest_player","make_safe","info_for_ped","clear_adrenaline","reset_sprint","clear_weapon_target","set_initial_state","set_wanted","pick_weapon_2p",
    "clear_weapons","give_weapon","bone_position","restart_anims","restore_heading_rate","say","on_screen","clean_up_ref","register_ref","task_swim","task_jetpack","find_task",
    "wanted_reset","clear_drunk_blur","extinguish","turn_off_jetpack","acquaintances","clear_space","on_mission",
    "remove_buildings","stream_zone_models","flush_requests","delete_rw_after_death","remove_unused","load_scene_collision","load_scene","drastic_tidy","garages_died","garages_hideout","remove_shop","pfx_reset",
    "stop_extra_colour","far_clip","clothes_set","rebuild_player","rebuild_if_needed","audio_zones","police_scanner","nearest_water","timer_stop","timer_update","timer_suspend","timer_resume",
    "far_plane","request_objects","bike_on_road","car_on_road","enex_outside","hospital_point","police_point","clear_cross_hairs","rand","remove_passenger","line_2d","save_wb","load_wb",
    "delete","teleport","trap"};
struct TR { uint32 c,a,b,d,e,f,g,h; };
inline std::array<TR,2048> tr{}; inline int ntr{}; inline bool trOverflow{};
inline void T(uint32 c,uint32 a=0,uint32 b=0,uint32 d=0,uint32 e=0,uint32 f=0,uint32 g=0,uint32 h=0) { if(ntr<int(tr.size()))tr[ntr++]={c,a,b,d,e,f,g,h}; else trOverflow=true; }
inline json TraceJson() {
    json r=json::array();
    for(int k=0;k<ntr;++k){ const auto& e=tr[k]; json x=json::array({CODE_NAMES[e.c]});
        switch(e.c){
        // argumentos que son punteros
        case C_FADE_COL: case C_FADE: case C_BLUR: case C_CAMPOS: case C_CAM_BEHIND: case C_CAM_JUMPCUT: case C_PAD_SHAKE: case C_BIGMSG: case C_HELP: case C_HUDMSG: case C_TEXT:
        case C_WREMOVE: case C_WADD: case C_GETPOS: case C_KILL: case C_ARREST: case C_SAFE: case C_INFO: case C_ADREN: case C_SPRINT: case C_TARGET: case C_INITSTATE: case C_SETWANTED:
        case C_PICK2P: case C_CLEARW: case C_GIVEW: case C_BONE: case C_ANIMS: case C_HEADRATE: case C_SAY: case C_ONSCREEN: case C_CLEANREF: case C_REGREF: case C_SWIM: case C_JET:
        case C_FINDTASK: case C_WRESET: case C_EXTING: case C_JETOFF: case C_CLOTH_SET: case C_REBUILD: case C_REBUILD_IF: case C_SCANNER: case C_FARPLANE: case C_BIKE_ROAD: case C_CAR_ROAD:
        case C_ENEX: case C_REMPASS: case C_SAVE: case C_LOAD: case C_DTOR: case C_TELEPORT: case C_TRAP: case C_CLEARSPACE:
            x.push_back(Name(e.a)); break;
        default: x.push_back(e.a); }
        const uint32 rest[]{e.b,e.d,e.e,e.f,e.g,e.h};
        for(int q=0;q<6;++q){ const bool ptr=(e.c==C_CLEANREF||e.c==C_REGREF||e.c==C_REMPASS||e.c==C_FINDTASK||e.c==C_SWIM||e.c==C_JET||e.c==C_INFO)&&q==0 || (e.c==C_GETPOS&&q==0);
            x.push_back(ptr ? Name(rest[q]) : json(rest[q])); }
        r.push_back(x); }
    return r;
}

// ======================= fuente de bits =======================
inline uint32 maskLos{}, nLos{}, maskScreen{}, nScreen{}, maskSwim{}, nSwim{}, maskJet{}, nJet{}, maskTask{}, nTask{}, maskMission{}, nMission{}, maskLine{}, nLine{};
inline bool Bit(uint32 m,uint32& n) { return (m>>(n++%32))&1; }
inline uint32 seed{}, rngCalls{}, loadCounter{}, loadCount{};
inline float statValue{}; inline CVector coorsResult{}; inline void* vehResult{};
inline float groundZ{}, farClip{};

// ======================= stubs =======================
// jugador (sin traza: getters puros con el CWorld::Players del fixture)
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { const int k=id<0 ? int(*reinterpret_cast<uint8*>(0xB7CD74)) : id; return At<CPlayerPed*>(Plr(k),0); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32,bool) { return static_cast<CVehicle*>(vehResult); }
inline CVector* __cdecl FakeFindPlayerCoors(CVector* out,int32) { *out=coorsResult; return out; }
inline CWanted* __cdecl FakeFindPlayerWanted(int32 id) { const int k=id<0 ? int(*reinterpret_cast<uint8*>(0xB7CD74)) : id; return At<CWanted*>(Plr(k),4); }
// estadisticas, reloj, pickups
inline float __cdecl FakeGetStat(eStats s) { T(C_STAT_GET,uint32(s),F(statValue)); return statValue; }
inline void __cdecl FakeIncStat(eStats s,float v) { T(C_STAT_INC,uint32(s),F(v)); }
inline void __cdecl FakeSetStat(eStats s,float v) { T(C_STAT_SET,uint32(s),F(v)); }
inline void __cdecl FakeRespawn() { T(C_STAT_RESPAWN); }
inline void __cdecl FakeRespect(uint8 a) { T(C_STAT_RESPECT,a); }
inline void __cdecl FakeSexAppeal() { T(C_STAT_SEX); }
inline void __cdecl FakeSetClock(uint8 h,uint8 m,uint8 d) { T(C_CLOCK,h,m,d); }
inline void __cdecl FakePickupsPassTime(uint32 t) { T(C_PICKUPS,t); }
// camara y mando
inline void __fastcall FakeFadeColour(void* self,void*,uint8 r,uint8 g,uint8 b) { T(C_FADE_COL,U(self),r,g,b); }
inline void __fastcall FakeFade(void* self,void*,float d,uint16 dir) { T(C_FADE,U(self),F(d),dir); }
inline void __fastcall FakeMotionBlur(void* self,void*,uint8 r,uint8 g,uint8 b,int32 a,uint32 type) { T(C_BLUR,U(self),r,g,b,uint32(a),type); }
inline CVector* __fastcall FakeGameCamPos(void* self,void*) { T(C_CAMPOS,U(self)); return &camPos; }
inline void __fastcall FakeCamBehind(void* self,void*) { T(C_CAM_BEHIND,U(self)); }
inline void __fastcall FakeCamJumpCut(void* self,void*) { T(C_CAM_JUMPCUT,U(self)); }
inline void __fastcall FakeStopShaking(void* self,void*,int16 a) { T(C_PAD_SHAKE,U(self),uint32(a)); }
// mensajes y textos
inline void __cdecl FakeClearMessages(bool f) { T(C_MSG_CLEAR,f); }
inline void __cdecl FakeBigMessage(const GxtChar* t,uint32 time,eMessageStyle st) { T(C_BIGMSG,U(t),time,uint32(st)); }
inline void __cdecl FakeHelp(const GxtChar* t,bool q,bool p,bool b) { T(C_HELP,U(t),q,p,b); }
inline void __cdecl FakeHudMessage(const GxtChar* t) { T(C_HUDMSG,U(t)); }
inline void __cdecl FakeResetWasted() { T(C_WASTED); }
inline const GxtChar* __fastcall FakeTextGet(void* self,void*,const char* key) {
    static const char* const keys[]{"WRN_2P","WRN1_2P","WRN2_2P","BUSTED","SKIP"}; uint32 k=5;
    for(uint32 q=0;q<5;++q)if(!strcmp(key,keys[q]))k=q;
    T(C_TEXT,U(self),k); return texts[k].data(); }
// mundo
inline void __cdecl FakeInteresting() { T(C_INTERESTING); }
inline void __cdecl FakeExciting(const CVector& p,float r,uint8 f) { T(C_EXCITING,F(p.x),F(p.y),F(p.z),F(r),f); }
inline void __cdecl FakeWorldRemove(CEntity* e) { T(C_WREMOVE,U(e)); }
inline void __cdecl FakeWorldAdd(CEntity* e) { T(C_WADD,U(e)); }
inline bool __cdecl FakeLos(const CVector& o,const CVector& t,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7) {
    const bool r=Bit(maskLos,nLos); T(C_LOS,F(o.x),F(o.y),F(o.z),F(t.x),F(t.y),F(t.z),uint32(b1)|b2<<1|b3<<2|b4<<3|b5<<4|b6<<5|b7<<6|uint32(r)<<8); return r; }
inline float __cdecl FakeGroundZ(float x,float y) { T(C_GROUNDZ,F(x),F(y),F(groundZ)); return groundZ; }
inline void __cdecl FakeRemoveRefsPlayer() { T(C_REFS); }
// CPlayerInfo, CPlayerPed y CPed
inline CVector EntityPos(const void* e) { const auto* m=At<uint8*>(const_cast<void*>(e),0x14); return m ? At<CVector>(const_cast<uint8*>(m),0x30) : At<CVector>(const_cast<void*>(e),4); }
inline CVector* __fastcall FakeInfoGetPos(void* self,void*,CVector* out) { void* ped=At<void*>(self,0); *out=ped ? EntityPos(ped)+CVector{0.5f,-0.25f,0.125f} : CVector{1.f,2.f,3.f}; T(C_GETPOS,U(self),U(ped)); return out; }
inline void __fastcall FakeKill(void* self,void*) { T(C_KILL,U(self)); }
inline void __fastcall FakeArrest(void* self,void*) { T(C_ARREST,U(self)); }
inline void __fastcall FakeMakeSafe(void* self,void*,bool c,float r) { T(C_SAFE,U(self),c,F(r)); }
inline void* __fastcall FakeInfoForPed(void* self,void*) { const int k=PedIndex(self); void* r=k==1 ? Plr(1) : Plr(0); T(C_INFO,U(self),U(r)); return r; }
inline void __fastcall FakeAdrenaline(void* self,void*) { T(C_ADREN,U(self)); }
inline void __fastcall FakeSprint(void* self,void*) { T(C_SPRINT,U(self)); }
inline void __fastcall FakeClearTarget(void* self,void*) { T(C_TARGET,U(self)); }
inline void __fastcall FakeInitialState(void* self,void*,bool g) { T(C_INITSTATE,U(self),g); }
inline void __fastcall FakeSetWanted(void* self,void*,int32 l) { T(C_SETWANTED,U(self),uint32(l)); }
inline void __fastcall FakePick2P(void* self,void*) { T(C_PICK2P,U(self)); }
inline void __fastcall FakeClearWeapons(void* self,void*) { T(C_CLEARW,U(self)); }
inline eWeaponSlot __fastcall FakeGiveWeapon(void* self,void*,eWeaponType t,uint32 ammo,bool like) { T(C_GIVEW,U(self),uint32(t),ammo,like); return eWeaponSlot(uint32(t)%13); }
inline void __fastcall FakeBone(void* self,void*,RwV3d& p,uint32 bone,bool upd) { T(C_BONE,U(self),bone,upd,F(p.x),F(p.y),F(p.z)); p.x+=1.5f; p.y-=2.25f; p.z+=0.75f; }
inline void __fastcall FakeAnims(void* self,void*) { T(C_ANIMS,U(self)); }
inline void __fastcall FakeHeadRate(void* self,void*) { T(C_HEADRATE,U(self)); }
inline int16 __fastcall FakeSay(void* self,void*,uint32 ctx,uint32 delay,float prob,bool a,bool b,bool c) { T(C_SAY,U(self),ctx,delay,F(prob),uint32(a)|b<<1|c<<2); return int16(-1); }
inline bool __fastcall FakeOnScreen(void* self,void*) { const bool r=Bit(maskScreen,nScreen); T(C_ONSCREEN,U(self),r); return r; }
inline void __fastcall FakeCleanUpRef(void* self,void*,CEntity** ref) { T(C_CLEANREF,U(self),U(ref)); }
inline void __fastcall FakeRegisterRef(void* self,void*,CEntity** ref) { T(C_REGREF,U(self),U(ref)); }
inline void* __fastcall FakeSwim(void* self,void*) { void* r=Bit(maskSwim,nSwim) ? taskObj.data() : nullptr; T(C_SWIM,U(self),U(r)); return r; }
inline void* __fastcall FakeJet(void* self,void*) { void* r=Bit(maskJet,nJet) ? taskObj.data() : nullptr; T(C_JET,U(self),U(r)); return r; }
inline void* __fastcall FakeFindTask(void* self,void*,uint32 type) { void* r=Bit(maskTask,nTask) ? taskObj.data() : nullptr; T(C_FINDTASK,U(self),U(r),type); return r; }
inline void __fastcall FakeWantedReset(void* self,void*) { T(C_WRESET,U(self)); }
inline void __cdecl FakeDrunkBlur() { T(C_DRUNKBLUR); }
inline void __fastcall FakeExtinguish(void* self,void*) { T(C_EXTING,U(self),*static_cast<uint8*>(self),U(At<void*>(Ped(0),0x730)),U(At<void*>(Ped(1),0x730))); }
inline void __fastcall FakeJetOff(void* self,void*) { T(C_JETOFF,U(self)); }
inline void* __cdecl FakeAcq(uint32 type) { T(C_ACQ,type); for(int q=0;q<5;++q)At<uint32>(acqObj.data(),q*4)=type*0x01010101u+q; return acqObj.data(); }
inline void __cdecl FakeClearSpace(const CVector& p,CEntity* e) { T(C_CLEARSPACE,U(e),F(p.x),F(p.y),F(p.z)); }
inline bool __cdecl FakeOnMission() { const bool r=Bit(maskMission,nMission); T(C_MISSION,r); return r; }
// streaming, memoria y demas
inline void __cdecl FakeRmBuild(int32 a) { T(C_RMBUILD,uint32(a)); }
inline void __cdecl FakeZoneModels(const CVector& p) { T(C_ZONEMODELS,F(p.x),F(p.y),F(p.z)); }
inline void __cdecl FakeFlush() { T(C_FLUSH); }
inline void __cdecl FakeDelRw(const CVector& p) { T(C_DELRW,F(p.x),F(p.y),F(p.z)); }
inline void __cdecl FakeRmUnused() { T(C_RMUNUSED); }
inline void __cdecl FakeLsCol(const CVector& p) { T(C_LSCOL,F(p.x),F(p.y),F(p.z)); }
inline void __cdecl FakeLsScene(const CVector& p) { T(C_LSCENE,F(p.x),F(p.y),F(p.z)); }
inline void __cdecl FakeTidy(bool a) { T(C_TIDY,a); }
inline void __cdecl FakeGarageDied() { T(C_GARAGE_DIED); }
inline void __cdecl FakeGarageHide() { T(C_GARAGE_HIDE); }
inline void __cdecl FakeShop() { T(C_SHOP); }
inline void __cdecl FakePfxReset() { T(C_PFX_RESET); }
inline void __cdecl FakeExtraColour(bool a) { T(C_EXTRACOL,a); }
inline float __cdecl FakeFarClip(CVector p) { T(C_FARCLIP,F(p.x),F(p.y),F(p.z),F(farClip)); return farClip; }
inline void __fastcall FakeClothSet(void* self,void*,uint32 t,uint32 m,uint32 part) { T(C_CLOTH_SET,U(self),t,m,part); }
inline void __cdecl FakeRebuild(CPlayerPed* p,bool f) { T(C_REBUILD,U(p),f); }
inline void __cdecl FakeRebuildIf(CPlayerPed* p) { T(C_REBUILD_IF,U(p)); }
inline void __cdecl FakeAudioZones(bool f,CVector p) { T(C_AZONES,f,F(p.x),F(p.y),F(p.z)); }
inline void __fastcall FakeScanner(void* self,void*,uint8 f) { T(C_SCANNER,U(self),f); }
inline void __cdecl FakeWater() { T(C_WATER); }
inline void __cdecl FakeTStop() { T(C_TSTOP); }
inline void __cdecl FakeTUpdate() { T(C_TUPDATE); }
inline void __cdecl FakeTSuspend() { T(C_TSUSPEND); }
inline void __cdecl FakeTResume() { T(C_TRESUME); }
inline RwCamera* __cdecl FakeFarPlane(RwCamera* c,float f) { T(C_FARPLANE,U(c),F(f)); return c; }
inline void __cdecl FakeReqDir(const CVector& p,float a,int32 f) { T(C_REQDIR,F(p.x),F(p.y),F(p.z),F(a),uint32(f)); }
inline void __fastcall FakeBikeRoad(void* self,void*) { T(C_BIKE_ROAD,U(self)); }
inline void __fastcall FakeCarRoad(void* self,void*) { T(C_CAR_ROAD,U(self)); }
inline void __fastcall FakeEnex(void* self,void*,CVector& p) { T(C_ENEX,U(self),F(p.x),F(p.y),F(p.z)); p.x+=100.f; p.y=-p.y; p.z+=7.5f; }
inline void __cdecl FakeHospital(CVector p,CVector& out,float& a) { T(C_HOSP,F(p.x),F(p.y),F(p.z)); out=CVector{p.y*0.5f+11.f,p.x-3.f,p.z+1.25f}; a=p.x*0.01f; }
inline void __cdecl FakePolice(CVector p,CVector& out,float& a) { T(C_POLICE,F(p.x),F(p.y),F(p.z)); out=CVector{p.x+33.f,p.y*0.25f,p.z-2.5f}; a=-p.y*0.02f; }
inline void __cdecl FakeCrossHairs() { T(C_CROSSHAIR); }
inline int __cdecl FakeRand() { T(C_RAND); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline void __fastcall FakeRemovePassenger(void* self,void*,CPed* p) { T(C_REMPASS,U(self),U(p)); }
inline bool __cdecl FakeLine(float a,float b,float c,float d,float e,float f,float g,float h) {
    const bool r=Bit(maskLine,nLine); uint32 x=2166136261u; for(float v:{a,b,c,d,e,f,g,h}){ x^=F(v); x*=16777619u; } T(C_LINE,x,F(a),F(b),F(e),F(f),r); return r; }
inline bool __cdecl FakeSaveWB(void* p,int32 n) { uint32 b[3]{}; if(n>0)std::memcpy(b,p,std::min<size_t>(size_t(n),12)); T(C_SAVE,U(p),uint32(n),b[0],b[1],b[2]); return true; }
inline bool __cdecl FakeLoadWB(void* p,int32 n) { auto* b=static_cast<uint8*>(p);
    if(U(p)==0x96A890&&n==4)*reinterpret_cast<int32*>(p)=int32(loadCount);
    else for(int32 k=0;k<n;++k)b[k]=uint8(loadCounter++*37u+11u);
    T(C_LOAD,U(p),uint32(n)); return true; }
// vtable de peatones y vehiculos falsos: [0] destructor, [14] Teleport, el resto trampas
inline void* __fastcall FakeDtor(void* self,void*,uint32 flags) { T(C_DTOR,U(self),flags); return self; }
inline void __fastcall FakeTeleport(void* self,void*,CVector p,bool clear) { T(C_TELEPORT,U(self),F(p.x),F(p.y),F(p.z),clear);
    auto* m=At<uint8*>(self,0x14); if(m)At<CVector>(m,0x30)=p; else At<CVector>(self,4)=p; }
template<int N> void __fastcall Trap(void* self,void*) { T(C_TRAP,U(self),uint32(N)); }
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
inline const std::array<Fn,27> fns{{
    {"CGameLogic","CalcDistanceToForbiddenTrainCrossing",0x4418E0,0,-1,1024},{"CGameLogic","ClearSkip",0x441560,0,-1,128},
    {"CGameLogic","DoWeaponStuffAtStartOf2PlayerGame",0x4428B0,0,-1,256},{"CGameLogic","FindCityClosestToPoint",0x441B70,0,-1,1024},
    {"CGameLogic","ForceDeathRestart",0x441240,0,-1,64},{"CGameLogic","InitAtStartOfGame",0x441210,0,-1,64},
    {"CGameLogic","IsCoopGameGoingOn",0x441390,0,-1,128},{"CGameLogic","IsPlayerAllowedToGoInThisDirection",0x441E10,0,-1,1024},
    {"CGameLogic","IsPlayerUse2PlayerControls",0x442020,0,-1,256},{"CGameLogic","IsPointWithinLineArea",0x4416E0,0,-1,1024},
    {"CGameLogic","IsSkipWaitingForScriptToFadeIn",0x4416C0,0,-1,64},{"CGameLogic","LaRiotsActiveHere",0x441C10,0,-1,1024},
    {"CGameLogic","Load",0x5D3440,0,-1,128},{"CGameLogic","PassTime",0x4414C0,0,-1,1024},
    {"CGameLogic","Remove2ndPlayerIfPresent",0x4413C0,0,-1,128},{"CGameLogic","ResetStuffUponResurrection",0x442980,0,-1,256},
    {"CGameLogic","RestorePedsWeapons",0x441D30,0,-1,512},{"CGameLogic","RestorePlayerStuffDuringResurrection",0x442060,0,-1,512},
    {"CGameLogic","Save",0x5D33C0,0,-1,128},{"CGameLogic","SetPlayerWantedLevelForForbiddenTerritories",0x441770,0,-1,1024},
    {"CGameLogic","SetUpSkip",0x4423C0,0,-1,256},{"CGameLogic","SkipCanBeActivated",0x4415C0,0,-1,512},
    {"CGameLogic","SortOutStreamingAndMemory",0x441440,0,-1,128},{"CGameLogic","StopPlayerMovingFromDirection",0x441290,0,-1,1024},
    {"CGameLogic","StorePedsWeapons",0x441D00,0,-1,128},{"CGameLogic","Update",0x442AD0,0,-1,1024},
    {"CGameLogic","UpdateSkip",0x442480,0,-1,1024},
}};

struct Region { uintptr_t at; size_t n; std::vector<uint8> saved; };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"gamelogic27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"gamelogic27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("gamelogic27");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"gamelogic27-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,27> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // armas validas (tipos 0..46)
    FillTraps(std::make_integer_sequence<int,64>{}); vt[0]=reinterpret_cast<void*>(&FakeDtor); vt[14]=reinterpret_cast<void*>(&FakeTeleport);
    static const char* const textKeys[6]{"WRN_2P","WRN1_2P","WRN2_2P","BUSTED","SKIP","OTHER"};
    for(size_t k=0;k<texts.size();++k){ texts[k].fill(0); for(size_t c=0;textKeys[k][c];++c)texts[k][c]=GxtChar(textKeys[k][c]); }

    // estado global guardado
    std::vector<Region> regions{{0x96A7C4,4,{}},{0x96A840,0x2E8,{}},{0x8A5E48,0x10,{}},{PLAYERS,2*PLSZ,{}},{0xB7CD74,1,{}},{0xB7CB4C,4,{}},{0xB7CB78,0x10,{}},
        {0xB70140,0x20,{}},{0xB72914,4,{}},{0xB6F028,0xD78,{}},{0xB73458,2*0x134,{}},{0xB5F852,1,{}},{0x969175,1,{}},{0xB72958,1,{}},{0xBAADC0,1,{}},
        {0xA4325C,2,{}},{0x9690C0,1,{}},{0xC0EC23,1,{}},{0xC0FCB6,1,{}},{0xC402D3,1,{}}};
    for(auto& r:regions)r.saved.assign(reinterpret_cast<uint8*>(r.at),reinterpret_cast<uint8*>(r.at)+r.n);
    auto RestoreAll=[&]{ for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.at),r.saved.data(),r.n); };
    auto AllRestored=[&]{ for(auto& r:regions)if(std::memcmp(reinterpret_cast<void*>(r.at),r.saved.data(),r.n))return false; return true; };

    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,auto stub){ void* s=reinterpret_cast<void*>(stub); if(exeAddr)plan.push_back({exeAddr,s}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,s}); } };
    using std::bit_cast;
    add(0x56E210,uintptr_t(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),&FakeFindPlayerPed);
    add(0x56E0D0,uintptr_t(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),&FakeFindPlayerVehicle);
    add(0x56E010,uintptr_t(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),&FakeFindPlayerCoors);
    add(0x56E230,uintptr_t(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),&FakeFindPlayerWanted);
    add(0x558E40,uintptr_t(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),&FakeGetStat);
    add(0x55C180,uintptr_t(&CStats::IncrementStat),&FakeIncStat);
    add(0x55A070,uintptr_t(&CStats::SetStatValue),&FakeSetStat);
    add(0x55CFC0,uintptr_t(&CStats::UpdateStatsOnRespawn),&FakeRespawn);
    add(0x55BC50,uintptr_t(&CStats::UpdateRespectStat),&FakeRespect);
    add(0x55BF20,uintptr_t(&CStats::UpdateSexAppealStat),&FakeSexAppeal);
    add(0x52D150,uintptr_t(&CClock::SetGameClock),&FakeSetClock);
    add(0x455200,uintptr_t(&CPickups::PassTime),&FakePickupsPassTime);
    add(0x50BF00,MAddr(&CCamera::SetFadeColour),&FakeFadeColour);
    add(0x50AC20,MAddr(&CCamera::Fade),&FakeFade);
    add(0x50BF40,MAddr(&CCamera::SetMotionBlur),&FakeMotionBlur);
    add(0x50AE50,MAddr(&CCamera::GetGameCamPosition),&FakeGameCamPos);
    add(0x50BD40,MAddr(&CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString),&FakeCamBehind);
    add(0x50BAB0,MAddr(&CCamera::RestoreWithJumpCut),&FakeCamJumpCut);
    add(0x53FB50,MAddr(&CPad::StopShaking),&FakeStopShaking);
    add(0x69DCD0,uintptr_t(&CMessages::ClearMessages),&FakeClearMessages);
    add(0x69F2B0,uintptr_t(&CMessages::AddBigMessage),&FakeBigMessage);
    add(0x588BE0,uintptr_t(&CHud::SetHelpMessage),&FakeHelp);
    add(0x588F60,uintptr_t(&CHud::SetMessage),&FakeHudMessage);
    add(0x589070,uintptr_t(&CHud::ResetWastedText),&FakeResetWasted);
    add(0x6A0050,MAddr(&CText::Get),&FakeTextGet);
    add(0x423F00,uintptr_t(&CCarCtrl::ClearInterestingVehicleList),&FakeInteresting);
    add(0x56A0D0,uintptr_t(&CWorld::ClearExcitingStuffFromArea),&FakeExciting);
    add(0x563280,uintptr_t(static_cast<void(*)(CEntity*)>(&CWorld::Remove)),&FakeWorldRemove);
    add(0x563220,uintptr_t(static_cast<void(*)(CEntity*)>(&CWorld::Add)),&FakeWorldAdd);
    add(0x56A490,uintptr_t(&CWorld::GetIsLineOfSightClear),&FakeLos);
    add(0x569660,uintptr_t(static_cast<float(*)(float,float)>(&CWorld::FindGroundZForCoord)),&FakeGroundZ);
    add(0x571AD0,uintptr_t(&CReferences::RemoveReferencesToPlayer),&FakeRemoveRefsPlayer);
    add(0x56DFB0,MAddr(static_cast<CVector(CPlayerInfo::*)()const>(&CPlayerInfo::GetPos)),&FakeInfoGetPos);
    add(0x56E580,MAddr(&CPlayerInfo::KillPlayer),&FakeKill);
    add(0x56E5D0,MAddr(&CPlayerInfo::ArrestPlayer),&FakeArrest);
    add(0x56E870,MAddr(&CPlayerInfo::MakePlayerSafe),&FakeMakeSafe);
    add(0x609FF0,MAddr(&CPlayerPed::GetPlayerInfoForThisPlayerPed),&FakeInfoForPed);
    add(0x60A070,MAddr(&CPlayerPed::ClearAdrenaline),&FakeAdrenaline);
    add(0x60A530,MAddr(&CPlayerPed::ResetSprintEnergy),&FakeSprint);
    add(0x609C80,MAddr(&CPlayerPed::ClearWeaponTarget),&FakeClearTarget);
    add(0x60CD20,MAddr(&CPlayerPed::SetInitialState),&FakeInitialState);
    add(0x609F10,MAddr(&CPlayerPed::SetWantedLevel),&FakeSetWanted);
    add(0x609800,MAddr(&CPlayerPed::PickWeaponAllowedFor2Player),&FakePick2P);
    add(0x5E6320,MAddr(&CPed::ClearWeapons),&FakeClearWeapons);
    add(0x5E6080,MAddr(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),&FakeGiveWeapon);
    add(0x5E01C0,MAddr(&CPed::GetTransformedBonePosition),&FakeBone);
    add(0x5DED50,MAddr(&CPed::RestartNonPartialAnims),&FakeAnims);
    add(0x5DFD60,MAddr(&CPed::RestoreHeadingRate),&FakeHeadRate);
    add(0x5EFFE0,MAddr(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),&FakeSay);
    add(0x534540,MAddr(&CEntity::GetIsOnScreen),&FakeOnScreen);
    add(0x571A00,MAddr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),&FakeCleanUpRef);
    add(0x571B70,MAddr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),&FakeRegisterRef);
    add(0x601070,MAddr(static_cast<CTaskSimpleSwim*(CPedIntelligence::*)()>(&CPedIntelligence::GetTaskSwim)),&FakeSwim);
    add(0x601110,MAddr(static_cast<CTaskSimpleJetPack*(CPedIntelligence::*)()>(&CPedIntelligence::GetTaskJetPack)),&FakeJet);
    add(0x681740,MAddr(static_cast<CTask*(CTaskManager::*)(eTaskType)>(&CTaskManager::FindActiveTaskByType)),&FakeFindTask);
    add(0x562400,MAddr(&CWanted::Reset),&FakeWantedReset);
    add(0x71D780,uintptr_t(&CMBlur::ClearDrunkBlur),&FakeDrunkBlur);
    add(0x5393F0,MAddr(&CFire::Extinguish),&FakeExtinguish);
    add(0x4E2A70,MAddr(&CAEPedAudioEntity::TurnOffJetPack),&FakeJetOff);
    add(0x6089B0,uintptr_t(static_cast<CAcquaintance&(*)(ePedType)>(&CPedType::GetPedTypeAcquaintances)),&FakeAcq);
    add(0x486B00,uintptr_t(&CTheScripts::ClearSpaceForMissionEntity),&FakeClearSpace);
    add(0x464D50,uintptr_t(&CTheScripts::IsPlayerOnAMission),&FakeOnMission);
    add(0x4094B0,uintptr_t(&CStreaming::RemoveBuildingsNotInArea),&FakeRmBuild);
    add(0x40A560,uintptr_t(&CStreaming::StreamZoneModels),&FakeZoneModels);
    add(0x40E4E0,uintptr_t(&CStreaming::FlushRequestList),&FakeFlush);
    add(0x409210,uintptr_t(&CStreaming::DeleteRwObjectsAfterDeath),&FakeDelRw);
    add(0x407AC0,uintptr_t(&CStreaming::RemoveUnusedModelsInLoadedList),&FakeRmUnused);
    add(0x40ED80,uintptr_t(&CStreaming::LoadSceneCollision),&FakeLsCol);
    add(0x40EB70,uintptr_t(&CStreaming::LoadScene),&FakeLsScene);
    add(0x53C810,uintptr_t(&CGame::DrasticTidyUpMemory),&FakeTidy);
    add(0x449E60,uintptr_t(&CGarages::PlayerArrestedOrDied),&FakeGarageDied);
    add(0x44A170,uintptr_t(&CGarages::CloseHideOutGaragesBeforeSave),&FakeGarageHide);
    add(0x49AE30,uintptr_t(&CShopping::RemoveLoadedShop),&FakeShop);
    add(0x7010F0,uintptr_t(&CPostEffects::ScriptResetForEffects),&FakePfxReset);
    add(0x55FF20,uintptr_t(&CTimeCycle::StopExtraColour),&FakeExtraColour);
    add(0x5616E0,uintptr_t(&CTimeCycle::FindFarClipForCoors),&FakeFarClip);
    add(0x5A8050,MAddr(static_cast<void(CPedClothesDesc::*)(uint32,uint32,eClothesTexturePart)>(&CPedClothesDesc::SetTextureAndModel)),&FakeClothSet);
    add(0x5A82C0,uintptr_t(&CClothes::RebuildPlayer),&FakeRebuild);
    add(0x5A8390,uintptr_t(&CClothes::RebuildPlayerIfNeeded),&FakeRebuildIf);
    add(0x5083C0,uintptr_t(&CAudioZones::Update),&FakeAudioZones);
    add(0x507400,MAddr(&CAudioEngine::StopPoliceScanner),&FakeScanner);
    add(0x6E9D70,uintptr_t(&CWaterLevel::FindNearestWaterAndItsFlow),&FakeWater);
    add(0x561AA0,uintptr_t(&CTimer::Stop),&FakeTStop);
    add(0x561B10,uintptr_t(&CTimer::Update),&FakeTUpdate);
    add(0x5619D0,uintptr_t(&CTimer::Suspend),&FakeTSuspend);
    add(0x561A00,uintptr_t(&CTimer::Resume),&FakeTResume);
    add(0x7EE2A0,uintptr_t(&RwCameraSetFarClipPlane),&FakeFarPlane);
    add(0x555CB0,uintptr_t(&CRenderer::RequestObjectsInDirection),&FakeReqDir);
    add(0x6BEEB0,MAddr(&CBike::PlaceOnRoadProperly),&FakeBikeRoad);
    add(0x6AF420,MAddr(&CAutomobile::PlaceOnRoadProperly),&FakeCarRoad);
    add(0x43EA00,MAddr(&CEntryExit::GetPositionRelativeToOutsideWorld),&FakeEnex);
    add(0x460850,uintptr_t(&CRestart::FindClosestHospitalRestartPoint),&FakeHospital);
    add(0x460A50,uintptr_t(&CRestart::FindClosestPoliceRestartPoint),&FakePolice);
    add(0x742C80,uintptr_t(&CWeaponEffects::ClearCrossHairs),&FakeCrossHairs);
    add(0x821B1E,0,&FakeRand);
    add(0,uintptr_t(&CGeneral::GetRandomNumber),&FakeRandomNumber);
    add(0x6D1610,MAddr(&CVehicle::RemovePassenger),&FakeRemovePassenger);
    add(0x4138D0,uintptr_t(&CCollision::Test2DLineAgainst2DLine),&FakeLine);
    add(0x5D1270,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),&FakeSaveWB);
    add(0x5D1300,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),&FakeLoadWB);
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }

    std::array<bool,27> enabled{},locked{};
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
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x27A1u; if(!st)st=1;
                    ntr=0; trOverflow=false; seed=uint32(i); rngCalls=0; loadCounter=0;
                    maskLos=next(); maskScreen=next(); maskSwim=next(); maskJet=next(); maskTask=next(); maskMission=next(); maskLine=next();
                    nLos=nScreen=nSwim=nJet=nTask=nMission=nLine=0;
                    statValue=[&]{ static const float sv[]{0.f,1.f,2.f,0.5f,-1.f,3.f}; return rnd(6)==0 ? uni(-2.f,3.f) : sv[rnd(6)]; }();
                    groundZ=uni(-20.f,80.f); farClip=uni(100.f,1500.f); loadCount=rnd(17);
                    camPos=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};

                    // ---- fixtures comunes ----
                    auto randMat=[&](uint8* m,const CVector& pos){ std::memset(m,0,MATSZ); const float a=uni(-3.2f,3.2f), c=std::cos(a), s=std::sin(a);
                        At<CVector>(m,0x00)=CVector{c,s,0.f}; At<CVector>(m,0x10)=CVector{-s,c,rnd(4)==0 ? uni(-0.3f,0.3f) : 0.f}; At<CVector>(m,0x20)=CVector{0.f,0.f,1.f}; At<CVector>(m,0x30)=pos; };
                    auto randPos=[&]{ return CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),rnd(8)==0 ? uni(900.f,1000.f) : uni(-10.f,200.f)}; };
                    auto setPed=[&](int k){
                        uint8* p=Ped(k); std::memset(p,0,PEDSZ); At<void*>(p,0)=vt.data();
                        const CVector pos=randPos(); At<CVector>(p,4)=pos; At<float>(p,0x10)=uni(-3.2f,3.2f);
                        if(rnd(3)){ randMat(Mat(k),pos+CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f}); At<void*>(p,0x14)=Mat(k); } else std::memset(Mat(k),0,MATSZ);
                        for(int q=0x1C;q<0x20;++q)p[q]=uint8(next()); for(int q=0x40;q<0x44;++q)p[q]=uint8(next());
                        for(int q=0;q<3;++q)At<float>(p,0x44+4*q)=rnd(4)==0 ? 0.f : uni(-2.f,2.f);
                        p[0x2F]=uint8(rnd(4)==0 ? next() : 0);
                        for(int q=0x46C;q<0x47C;++q)p[q]=uint8(next());
                        At<void*>(p,0x47C)=Intel(k); std::memset(Intel(k),0,INTSZ);
                        At<void*>(p,0x480)=k<2 ? Plr(k)+4 : nullptr;
                        for(int q=0x4E0;q<0x4F4;++q)p[q]=uint8(next());
                        static const uint32 states[]{1,54,55,63,0,41};
                        At<uint32>(p,0x530)=rnd(5)==0 ? rnd(70) : states[rnd(6)];
                        At<float>(p,0x540)=rnd(5)==0 ? 0.f : uni(-50.f,200.f); At<float>(p,0x548)=uni(0.f,100.f);
                        At<float>(p,0x558)=uni(-7.f,7.f); At<float>(p,0x55C)=uni(-7.f,7.f);
                        for(int s=0;s<13;++s){ uint8* w=p+0x5A0+s*0x1C; At<uint32>(w,0)=rnd(3)==0 ? 0 : rnd(47); At<uint32>(w,4)=rnd(4); At<uint32>(w,8)=rnd(60); At<uint32>(w,0xC)=rnd(4)==0 ? 0 : rnd(5000);
                            for(int q=0x10;q<0x1C;++q)w[q]=uint8(next()); }
                        At<void*>(p,0x58C)=rnd(3) ? Veh(k%2) : nullptr;
                        At<uint32>(p,0x598)=rnd(32);
                        At<void*>(p,0x730)=rnd(3)==0 ? fireObj.data() : nullptr;
                        At<uint32>(p,0x750)=next();
                        At<void*>(p,0x78C)=rnd(3)==0 ? enexObj.data() : nullptr;
                    };
                    auto setVeh=[&](int k){
                        uint8* v=Veh(k); std::memset(v,0,VEHSZ); At<void*>(v,0)=vt.data();
                        const CVector pos=randPos(); At<CVector>(v,4)=pos; randMat(Mat(3+k),pos); At<void*>(v,0x14)=Mat(3+k);
                        v[0x36]=uint8(next()); for(int q=0;q<3;++q)At<float>(v,0x44+4*q)=uni(-1.f,1.f);
                        static const uint32 types[]{0,9,0,9,5,3,1,2,10,11,6};
                        At<uint32>(v,0x590)=types[rnd(11)]; At<uint32>(v,0x594)=rnd(4)==0 ? rnd(12) : types[rnd(11)];
                        const uint32 d=rnd(4); At<void*>(v,0x460)=d==0 ? nullptr : d==1 ? Ped(0) : d==2 ? Ped(1) : Ped(2);
                    };
                    auto setPlayers=[&](bool p0,bool p1){
                        std::memset(Plr(0),0,2*PLSZ);
                        for(int k=0;k<2;++k){ uint8* pl=Plr(k); At<void*>(pl,0)=(k==0 ? p0 : p1) ? Ped(k) : nullptr;
                            At<void*>(pl,4)=Wnt(k); At<void*>(pl,8)=Clo(k); pl[4+0x20]=uint8(rnd(13)); pl[4+0x40]=uint8(next()); pl[4+0x41]=uint8(next()); pl[4+0x42]=uint8(next());
                            At<int32>(pl,0xB8)=rnd(5)==0 ? -int32(rnd(500)) : rnd(6)==0 ? 0 : int32(rnd(5000)); pl[0xDC]=uint8(rnd(3) ? 0 : rnd(5));
                            At<uint16>(pl,0x144)=uint16(next()); pl[0x14F]=uint8(next()); pl[0x151]=uint8(rnd(2)); pl[0x152]=uint8(rnd(2)); pl[0x154]=uint8(rnd(3)==0 ? rnd(3) : 0);
                            At<uint16>(pl,0x156)=uint16(rnd(40));
                            std::memset(Wnt(k),0,WSZ); At<uint32>(Wnt(k),0x2C)=rnd(8)==0 ? rnd(100) : rnd(7); At<uint32>(Wnt(k),0xC)=next();
                            std::memset(Clo(k),0,CLSZ); At<uint32>(Clo(k),0x24)=rnd(2) ? next() : 0; }
                    };
                    auto setWorld=[&](bool p0,bool p1){
                        for(int k=0;k<3;++k)setPed(k); setVeh(0); setVeh(1); setPlayers(p0,p1);
                        for(auto& b:fireObj)b=uint8(next()); std::memset(enexObj.data(),0,SMALL); std::memset(taskObj.data(),0,SMALL); std::memset(acqObj.data(),0,SMALL);
                        focus=uint8(rnd(2)); if(!p1)focus=0;
                        // globales de CGameLogic
                        CGameLogic::SkipState=eSkipState(rnd(6)); CGameLogic::SkipTimer=next()%3 ? 1000000+rnd(100000) : next();
                        CGameLogic::SkipPosition=randPos(); CGameLogic::SkipHeading=uni(-360.f,360.f); CGameLogic::SkipToBeFinishedByScript=rnd(2)!=0;
                        { const uint32 v=rnd(4); CGameLogic::SkipVehicle=v==0 ? nullptr : reinterpret_cast<CVehicle*>(Veh(v%2)); }
                        *reinterpret_cast<uint8*>(0x96A8B0)=uint8(rnd(8)==0 ? next() : rnd(5)); CGameLogic::ActivePlayers=uint8(next());
                        CGameLogic::bPlayersCannotTargetEachOther=rnd(2)!=0; CGameLogic::bPlayersCanBeInSeparateCars=rnd(2)!=0; CGameLogic::bScriptCoopGameGoingOn=rnd(2)!=0;
                        CGameLogic::nPrintFocusHelpTimer=int32(rnd(4)==0 ? 0 : 1000000+rnd(200000)); CGameLogic::nPrintFocusHelpCounter=int32(rnd(8)==0 ? next() : rnd(7));
                        CGameLogic::NumAfterDeathStartPoints=int32(rnd(17)); for(auto& p:CGameLogic::AfterDeathStartPoints)p=randPos(); for(auto& o:CGameLogic::AfterDeathStartPointOrientations)o=uni(-360.f,360.f);
                        CGameLogic::f2PlayerStartHeading=uni(-6.3f,6.3f); CGameLogic::vec2PlayerStartLocation=randPos();
                        for(int s=0;s<13;++s){ uint8* w=reinterpret_cast<uint8*>(0x96A9B8)+s*0x1C; At<uint32>(w,0)=rnd(3)==0 ? 0 : rnd(47); At<uint32>(w,4)=rnd(4); At<uint32>(w,8)=rnd(60); At<uint32>(w,0xC)=rnd(5000);
                            for(int q=0x10;q<0x1C;++q)w[q]=uint8(next()); }
                        *reinterpret_cast<float*>(0x96AB24)=uni(0.f,140.f);
                        CGameLogic::bPenaltyForDeathApplies=rnd(3)!=0; CGameLogic::bPenaltyForArrestApplies=rnd(3)!=0; CGameLogic::bLimitPlayerDistance=rnd(3)!=0;
                        CGameLogic::MaxPlayerDistance=rnd(4)==0 ? 20.f : uni(5.f,120.f); CGameLogic::n2PlayerPedInFocus=eFocusedPlayer(rnd(3));
                        // tiempo, reloj, mundo
                        const uint32 now=1000000+rnd(200000); *reinterpret_cast<uint32*>(0xB7CB84)=now; *reinterpret_cast<uint32*>(0xB7CB78)=now-rnd(100);
                        *reinterpret_cast<uint32*>(0xB7CB4C)=rnd(3)==0 ? 18+32*rnd(1000) : next();
                        CGameLogic::TimeOfLastEvent=int32(rnd(6)==0 ? next() : now-rnd(6000));
                        *reinterpret_cast<uint8*>(0xB7014E)=uint8(rnd(8)==0 ? rnd(12) : 1+rnd(7)); *reinterpret_cast<uint8*>(0xB70152)=uint8(rnd(8)==0 ? rnd(256) : rnd(60));
                        *reinterpret_cast<uint8*>(0xB70153)=uint8(rnd(8)==0 ? rnd(256) : rnd(24));
                        *reinterpret_cast<int32*>(0xB72914)=rnd(3)==0 ? int32(rnd(18)) : 0; *reinterpret_cast<uint8*>(0xB6F079)=uint8(rnd(4)==0);
                        *reinterpret_cast<uint8*>(0xB5F852)=uint8(rnd(10)==0); *reinterpret_cast<uint8*>(0x969175)=uint8(rnd(5)==0); *reinterpret_cast<uint8*>(0xB72958)=uint8(rnd(2));
                        *reinterpret_cast<uint8*>(0xBAADC0)=uint8(rnd(3)==0 ? 'A' : 0); *reinterpret_cast<uint8*>(0xA4325C)=uint8(rnd(2)); *reinterpret_cast<uint8*>(0xA4325D)=uint8(rnd(2));
                        auto* pad=reinterpret_cast<uint8*>(0xB73458); At<uint16>(pad,0x16)=uint16(rnd(2) ? 255 : 0); At<uint16>(pad,0x46)=uint16(rnd(3)==0 ? 255 : 0); At<uint16>(pad,0x10E)=uint16(next());
                        At<uint16>(pad+0x134,0x10E)=uint16(next());
                        coorsResult=rnd(3)==0 ? CGameLogic::SkipPosition+CVector{uni(-30.f,30.f),uni(-30.f,30.f),0.f} : randPos();
                        { const uint32 v=rnd(3); vehResult=v==0 ? nullptr : Veh(v-1); }
                    };

                    void* const A=reinterpret_cast<void*>(fn.address);
                    json ret=nullptr; bool switches=false;
                    auto on=[&]{ for(auto* h:hooks)h->State(candidate); switches=ReversingProbe::JumpsToDll(fn.address)==candidate; };
                    auto off=[&]{ for(auto* h:hooks)h->State(false); };
                    json extra=json::object();

                    if(nm=="CalcDistanceToForbiddenTrainCrossing"){ setWorld(true,false);
                        static const float cx[]{82.f,-1568.f,2770.f,2270.f}, cy[]{-1021.f,537.f,277.f,277.f}; const uint32 c=rnd(4);
                        CVector p=rnd(4)==0 ? randPos() : CVector{cx[c]+uni(-400.f,400.f),cy[c]+uni(-400.f,400.f),rnd(3)==0 ? 0.f : uni(-50.f,50.f)};
                        if(rnd(16)==0)p=CVector{cx[c],cy[c],uni(-5.f,5.f)};
                        CVector ms=rnd(6)==0 ? CVector{0.f,0.f,0.f} : CVector{uni(-2.f,2.f),uni(-2.f,2.f),rnd(2) ? 0.f : uni(-1.f,1.f)};
                        const bool ign=rnd(3)==0; CVector out{-77.f,-77.f,-77.f};
                        on(); const float r=reinterpret_cast<float(__cdecl*)(CVector,CVector,bool,CVector&)>(A)(p,ms,ign,out); off();
                        ret=F(r); extra["out"]=json::array({F(out.x),F(out.y),F(out.z)}); }
                    else if(nm=="ClearSkip"){ setWorld(true,rnd(2)!=0); const bool am=rnd(2)!=0; on(); reinterpret_cast<void(__cdecl*)(bool)>(A)(am); off(); }
                    else if(nm=="DoWeaponStuffAtStartOf2PlayerGame"){ setWorld(true,true); const bool sh=rnd(4)!=0; on(); reinterpret_cast<void(__cdecl*)(bool)>(A)(sh); off(); }
                    else if(nm=="FindCityClosestToPoint"){ setWorld(true,false); static const float cx[]{1670.f,-1810.f,2161.f}, cy[]{-1137.f,884.f,2140.f};
                        CVector2D p{uni(-3500.f,3500.f),uni(-3500.f,3500.f)};
                        if(rnd(4)==0){ const uint32 a=rnd(3), b=(a+1+rnd(2))%3; const float t=rnd(2) ? 0.5f : uni(0.4f,0.6f); p=CVector2D{cx[a]+(cx[b]-cx[a])*t,cy[a]+(cy[b]-cy[a])*t}; }
                        if(rnd(32)==0)p.x=std::numeric_limits<float>::quiet_NaN();
                        on(); ret=int(reinterpret_cast<int32(__cdecl*)(CVector2D)>(A)(p)); off(); }
                    else if(nm=="ForceDeathRestart"||nm=="InitAtStartOfGame"||nm=="Remove2ndPlayerIfPresent"||nm=="ResetStuffUponResurrection"||nm=="Update"||nm=="UpdateSkip"){
                        const bool p1=nm=="Remove2ndPlayerIfPresent" ? rnd(3)!=0 : rnd(2)!=0; setWorld(true,p1);
                        if(nm=="Update"){ if(rnd(3)==0)*reinterpret_cast<uint8*>(0x96A8B0)=uint8(1+rnd(4)); if(rnd(4)==0)CGameLogic::TimeOfLastEvent=int32(*reinterpret_cast<uint32*>(0xB7CB84)-3000-rnd(3)); }
                        if(nm=="UpdateSkip"&&rnd(3)==0)CGameLogic::SkipTimer=*reinterpret_cast<uint32*>(0xB7CB84)-rnd(3);
                        on(); reinterpret_cast<void(__cdecl*)()>(A)(); off(); }
                    else if(nm=="IsCoopGameGoingOn"||nm=="IsSkipWaitingForScriptToFadeIn"||nm=="LaRiotsActiveHere"||nm=="SkipCanBeActivated"||nm=="Save"||nm=="Load"){
                        setWorld(rnd(5)!=0,rnd(2)!=0);
                        if(nm=="LaRiotsActiveHere"){ static const float ex[]{1620.f,2824.f,157.f,1630.f}, ey[]{-2178.f,-1067.f,-1950.f,-1192.f};
                            coorsResult=CVector{uni(0.f,3000.f),uni(-2400.f,-900.f),rnd(6)==0 ? 950.f : uni(900.f,1000.f)}; if(rnd(2))coorsResult.z=uni(-10.f,100.f);
                            if(rnd(4)==0)coorsResult.x=ex[rnd(4)]; if(rnd(4)==0)coorsResult.y=ey[rnd(4)]; }
                        if(nm=="SkipCanBeActivated"&&rnd(2))*reinterpret_cast<int32*>(0xB72914)=0;
                        if(!At<void*>(Plr(focus),0))At<void*>(Plr(focus),0)=Ped(focus);
                        on(); ret=reinterpret_cast<bool(__cdecl*)()>(A)(); off(); }
                    else if(nm=="IsPlayerAllowedToGoInThisDirection"||nm=="IsPlayerUse2PlayerControls"){ setWorld(rnd(6)!=0,rnd(6)!=0);
                        const uint32 pk=rnd(4); CPed* ped=reinterpret_cast<CPed*>(Ped(pk==3 ? 2 : int(pk%3)));
                        if(rnd(3)==0){ At<CVector>(Ped(1),4)=At<CVector>(Ped(0),4)+CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(-5.f,5.f)}; At<void*>(Ped(1),0x14)=nullptr; At<void*>(Ped(0),0x14)=nullptr; }
                        if(nm=="IsPlayerUse2PlayerControls"){ on(); ret=reinterpret_cast<bool(__cdecl*)(CPed*)>(A)(ped); off(); }
                        else { CVector d{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; if(rnd(8)==0)d=CVector{0.f,0.f,0.f}; const float lim=rnd(4)==0 ? 0.f : uni(-10.f,40.f);
                            on(); ret=reinterpret_cast<bool(__cdecl*)(CPed*,CVector,float)>(A)(ped,d,lim); off(); } }
                    else if(nm=="IsPointWithinLineArea"){ setWorld(true,false); for(auto& p:points)p=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-5.f,5.f)};
                        const uint32 n=rnd(10)==0 ? 0 : rnd(16)+1; const float x=uni(-3000.f,3000.f), y=uni(-3000.f,3000.f);
                        on(); ret=reinterpret_cast<bool(__cdecl*)(const CVector*,uint32,float,float)>(A)(points.data(),n,x,y); off(); }
                    else if(nm=="PassTime"){ setWorld(true,false); const uint32 m=rnd(4)==0 ? rnd(100000) : rnd(8)==0 ? 0 : rnd(3000);
                        on(); reinterpret_cast<void(__cdecl*)(uint32)>(A)(m); off(); }
                    else if(nm=="RestorePedsWeapons"||nm=="StorePedsWeapons"){ setWorld(true,rnd(2)!=0); CPed* ped=reinterpret_cast<CPed*>(Ped(int(rnd(3))));
                        on(); reinterpret_cast<void(__cdecl*)(CPed*)>(A)(ped); off(); }
                    else if(nm=="RestorePlayerStuffDuringResurrection"){ setWorld(true,true); const int k=int(rnd(2)); const CVector p=randPos(); const float h=rnd(6)==0 ? 0.f : uni(-400.f,400.f);
                        on(); reinterpret_cast<void(__cdecl*)(CPlayerPed*,CVector,float)>(A)(reinterpret_cast<CPlayerPed*>(Ped(k)),p,h); off(); }
                    else if(nm=="SetPlayerWantedLevelForForbiddenTerritories"){ setWorld(true,rnd(2)!=0); const bool imm=rnd(2)!=0;
                        on(); reinterpret_cast<void(__cdecl*)(bool)>(A)(imm); off(); }
                    else if(nm=="SetUpSkip"){ setWorld(true,false); const CVector p=randPos(); const float ang=uni(-360.f,360.f); const bool am=rnd(2)!=0;
                        void* v=rnd(3)==0 ? nullptr : Veh(int(rnd(2))); const bool fs=rnd(2)!=0;
                        on(); reinterpret_cast<void(__cdecl*)(CVector,float,bool,CEntity*,bool)>(A)(p,ang,am,static_cast<CEntity*>(v),fs); off(); }
                    else if(nm=="SortOutStreamingAndMemory"){ setWorld(true,false); const CVector p=randPos(); const float ang=uni(-7.f,7.f);
                        on(); reinterpret_cast<void(__cdecl*)(const CVector&,float)>(A)(p,ang); off(); }
                    else if(nm=="StopPlayerMovingFromDirection"){ setWorld(true,true); const int32 id=int32(rnd(2)); CVector d{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; if(rnd(6)==0)d.z=0.f;
                        on(); reinterpret_cast<void(__cdecl*)(int32,CVector)>(A)(id,d); off(); }

                    // ---- volcado ----
                    json r={{"ret",ret},{"switches",switches},{"extra",extra},{"trace",TraceJson()},{"overflow",trOverflow},{"rand",rngCalls},
                            {"gl_skip",ReversingProbe::Bytes(reinterpret_cast<void*>(0x96A890),0x58)},{"gl",Hash(reinterpret_cast<void*>(0x96A840),0x2E8)},
                            {"gl_cfg",ReversingProbe::Bytes(reinterpret_cast<void*>(0x8A5E48),0xC)},{"gl_focus",*reinterpret_cast<uint32*>(0x8A5E54-4)},
                            {"players",Hash(Plr(0),2*PLSZ)},{"player_bytes",json::array({ReversingProbe::Bytes(Plr(0)+0xB8,4),Plr(0)[0xDC],Plr(1)[0xDC],At<uint16>(Plr(0),0x144),At<uint16>(Plr(1),0x144),
                                Plr(0)[0x151],Plr(0)[0x152],Plr(0)[0x154],At<uint16>(Plr(0),0x156),Name(At<uint32>(Plr(0),0)),Name(At<uint32>(Plr(1),0))})},
                            {"peds",json::array({Hash(Ped(0),PEDSZ),Hash(Ped(1),PEDSZ),Hash(Ped(2),PEDSZ)})},{"vehs",Hash(vehs.data(),vehs.size())},{"mats",Hash(mats.data(),mats.size())},
                            {"objs",Hash(wanteds.data(),wanteds.size())+Hash(clothes.data(),clothes.size())+Hash(fireObj.data(),SMALL)+Hash(intels.data(),intels.size())},
                            {"camera",Hash(reinterpret_cast<void*>(0xB6F028),0xD78)},{"pads",json::array({At<uint16>(reinterpret_cast<void*>(0xB73458),0x10E),At<uint16>(reinterpret_cast<void*>(0xB73458+0x134),0x10E)})},
                            {"misc",json::array({*reinterpret_cast<int32*>(0x96A7C4),*reinterpret_cast<int32*>(0xB72914),*reinterpret_cast<uint8*>(0xA4325C),*reinterpret_cast<uint8*>(0xA4325D),
                                *reinterpret_cast<uint8*>(0x9690C0),*reinterpret_cast<uint8*>(0xC0EC23),*reinterpret_cast<uint8*>(0xC0FCB6),*reinterpret_cast<uint8*>(0xC402D3),
                                *reinterpret_cast<uint32*>(0xB7CB84),*reinterpret_cast<uint8*>(0xB7014E),*reinterpret_cast<uint8*>(0xB70152),*reinterpret_cast<uint8*>(0xB70153)})}};
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
