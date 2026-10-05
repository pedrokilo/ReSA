#pragma once
// Banco dentro del motor: CPlayerPed (lista de 34, 50 de sus 51 funciones). Original (hooks de la clase desactivados)
// frente a candidato con fixtures deterministas e identicos. El jugador es un bufer falso de 0x800 bytes con offsets
// crudos (CPlayerPedData en +0x480, inteligencia/gestor de tareas en +0x47C, armas en +0x5A0, estadisticas en +0x59C);
// nunca se usa el jugador real. CWorld::Players, el jugador en foco, el grupo del jugador (CPedGroups::ms_groups) y el
// pool de peatones solo se sustituyen por fixtures durante cada ejecucion y se restauran. Se simulan y trazan todos los
// callees con efectos: CStats, CWanted, CWorld (Add/Remove/LOS/slot), luz, animaciones, habilidades de arma, camara,
// mirilla, referencias, grupos (miembros, inteligencia, distancias), eventos (new/delete del pool de eventos, respuesta,
// clon), tareas, mando, audio, CMBlur, agua, modelo de colision, aleatorios (rand y CGeneral::GetRandomNumber), dano,
// bufer de guardado y CPed::Load/Save/ProcessControl. Las virtuales (Load 25, Save 24, SetMoveAnim 23,
// ProcessControl 10) se llaman por la vtable del EXE 0x86D168. La traza es un array fijo (sin reservas durante la
// llamada) que se convierte a json despues; los punteros a la pila se enmascaran como "stack".
// SetupPlayerPed no se valida: construye un CPlayerPed real (pools, audio, inteligencia, grupos).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHooks.h>
#include "PlayerPed.h"
#include "PlayerInfo.h"
#include "World.h"
#include "Stats.h"
#include "Wanted.h"
#include "Camera.h"
#include "Pad.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"
#include "GameLogic.h"
#include "WeaponEffects.h"
#include "WeaponInfo.h"
#include "Weapon.h"
#include "TagManager.h"
#include "Collision/Collision.h"
#include "WaterLevel.h"
#include "CarCtrl.h"
#include "MBlur.h"
#include "EntryExitManager.h"
#include "GenericGameStorage.h"
#include "Events/EventEditableResponse.h"
#include "Events/Event.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Audio/AudioEngine.h"
#include "Audio/Entities/AEPedWeaponAudioEntity.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace PlayerPed51Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T> inline T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// ======================= objetos falsos =======================
constexpr size_t PEDSZ=0x800, MATSZ=0x48, INTSZ=0x300, PDSZ=0xAC, WSZ=0x300, VEHSZ=0x800, OBJSZ=0x200, TASKSZ=0x80, ASZ=0x40, EVSZ=0x80;
constexpr size_t NPED=4, NMEM=7, NPOOL=10, POOLSZ=0x7C4, GROUPSZ=0x2D4;
constexpr uintptr_t PLAYERS=0xB7CD98, GROUPS=0xC09920, CPADS=0xB73458, CAMERA=0xB6F028;
alignas(16) inline std::array<uint8,NPED*PEDSZ> peds{};       // 0 = jugador probado, 1 = otro jugador, 2-3 objetivos
alignas(16) inline std::array<uint8,NMEM*PEDSZ> members{};    // miembros del grupo
alignas(16) inline std::array<uint8,24*MATSZ> mats{};      // 0-3 peatones, 8-9 vehiculos, 10 objeto, 11 edificio, 12-15 miembros, 16-19 pool
alignas(16) inline std::array<uint8,NPED*INTSZ> intels{};
alignas(16) inline std::array<uint8,2*PDSZ> pdatas{};
alignas(16) inline std::array<uint8,WSZ> wanted{};
alignas(16) inline std::array<uint8,0x40> statsObj{};
alignas(16) inline std::array<uint8,0x80> clothes{};
alignas(16) inline std::array<uint8,2*VEHSZ> vehs{};
alignas(16) inline std::array<uint8,OBJSZ> objBuf{};
alignas(16) inline std::array<uint8,0x40> bldBuf{};
alignas(16) inline std::array<uint8,6*TASKSZ> tasks{};
alignas(16) inline std::array<uint8,4*ASZ> assocs{};          // 0-1 asociaciones del clump, 2 nueva, 3 la de disparo
alignas(16) inline std::array<uint8,2*ASZ> statics{};         // asociaciones estaticas
alignas(16) inline std::array<uint8,4*0x10> hiers{};          // jerarquias (hash en +0)
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<uint8,0x40> colModel{};
alignas(16) inline std::array<uint8,2*EVSZ> evs{};            // 0 evento de dano, 1 su clon
alignas(16) inline std::array<uint8,EVSZ> arena{};            // operator new del pool de eventos
alignas(16) inline std::array<uint8,0x20> poolObj{};
alignas(16) inline std::array<uint8,NPOOL*POOLSZ> poolMem{};
alignas(16) inline std::array<uint8,16> poolMap{};
alignas(16) inline std::array<uint8,0x1C> weaponArg{};        // CWeapon suelto para CanIKReachThisTarget
inline std::array<void*,64> vt{}, taskVt{}, evVt{};
inline uint8* Ped(int k) { return peds.data()+size_t(k)*PEDSZ; }
inline uint8* Mem(int k) { return members.data()+size_t(k)*PEDSZ; }
inline uint8* Mat(int k) { return mats.data()+size_t(k)*MATSZ; }
inline uint8* Intel(int k) { return intels.data()+size_t(k)*INTSZ; }
inline uint8* PData(int k) { return pdatas.data()+size_t(k)*PDSZ; }
inline uint8* Veh(int k) { return vehs.data()+size_t(k)*VEHSZ; }
inline uint8* Task(int k) { return tasks.data()+size_t(k)*TASKSZ; }
inline uint8* Assoc(int k) { return assocs.data()+size_t(k)*ASZ; }
inline uint8* Stat(int k) { return statics.data()+size_t(k)*ASZ; }
inline uint8* Hier(int k) { return hiers.data()+size_t(k)*0x10; }
inline uint8* Ev(int k) { return evs.data()+size_t(k)*EVSZ; }
inline uint8* PoolPed(int k) { return poolMem.data()+size_t(k)*POOLSZ; }
inline uint8* Plr(int k) { return reinterpret_cast<uint8*>(PLAYERS)+size_t(k)*0x190; }

struct Rg { uintptr_t p; size_t n; size_t stride; const char* nm; };
inline std::vector<Rg> NameRegions() {
    return {{uintptr_t(peds.data()),peds.size(),PEDSZ,"ped"},{uintptr_t(members.data()),members.size(),PEDSZ,"member"},{uintptr_t(mats.data()),mats.size(),MATSZ,"mat"},
            {uintptr_t(intels.data()),intels.size(),INTSZ,"intel"},{uintptr_t(pdatas.data()),pdatas.size(),PDSZ,"pdata"},{uintptr_t(wanted.data()),WSZ,0,"wanted"},
            {uintptr_t(statsObj.data()),statsObj.size(),0,"stats"},{uintptr_t(clothes.data()),clothes.size(),0,"clothes"},{uintptr_t(vehs.data()),vehs.size(),VEHSZ,"veh"},
            {uintptr_t(objBuf.data()),OBJSZ,0,"object"},{uintptr_t(bldBuf.data()),bldBuf.size(),0,"building"},{uintptr_t(tasks.data()),tasks.size(),TASKSZ,"task"},
            {uintptr_t(assocs.data()),assocs.size(),ASZ,"assoc"},{uintptr_t(statics.data()),statics.size(),ASZ,"static"},{uintptr_t(hiers.data()),hiers.size(),0x10,"hier"},
            {uintptr_t(clump.data()),clump.size(),0,"clump"},{uintptr_t(colModel.data()),colModel.size(),0,"colmodel"},{uintptr_t(evs.data()),evs.size(),EVSZ,"event"},
            {uintptr_t(arena.data()),arena.size(),0,"arena"},{uintptr_t(poolMem.data()),poolMem.size(),POOLSZ,"pool"},{uintptr_t(weaponArg.data()),weaponArg.size(),0,"weapon"},
            {PLAYERS,2*0x190,0x190,"player"},{GROUPS,8*GROUPSZ,GROUPSZ,"group"},{CPADS,2*0x134,0x134,"cpad"},{CAMERA,0xD78,0,"camera"},{0xB6BC90,0x100,0,"audio_engine"}};
}
inline json Name(uint32 v) {
    if(!v)return nullptr;
    static const auto regions=NameRegions();
    for(auto& r:regions){ if(v<r.p||v>=r.p+r.n)continue; const auto o=v-r.p;
        if(r.stride)return std::string(r.nm)+"#"+std::to_string(o/r.stride)+"+"+std::to_string(o%r.stride);
        return std::string(r.nm)+"+"+std::to_string(o); }
    const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
    if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack";
    return v;
}

// ======================= traza fija (sin reservas durante la llamada) =======================
enum Code : uint32 {
    C_FATMUSCLE, C_WSET, C_WNODROP, C_WCHEAT, C_WUPDATE, C_WADD, C_WREMOVE, C_LIGHT, C_GETASSOC, C_STATASSOC, C_ADDANIM, C_SKILL, C_SKILLT,
    C_CAMMODE, C_CAMCLEAR, C_CAM1ST, C_CROSSHAIR, C_MARK, C_CLEANREF, C_REGREF, C_SAMEGROUP, C_FINDTASK, C_EVENTOFTYPE, C_JETPACK, C_USEGUN,
    C_SWIM, C_COOP, C_STOPEFFECT, C_WUPD, C_SETWEAPON, C_RANGEMULT, C_ISTAG, C_TAGALPHA, C_LOS, C_LOSFLAGS, C_BONE, C_TBONE, C_REALMOVE,
    C_WANTSATTACK, C_WEAPSWITCH, C_ANIMGROUPS, C_GROUPBEH, C_COORS, C_PLAYERVEH, C_DISTLINE, C_COUNT, C_GETMEMBER, C_REMOVEFOLLOW, C_ISMEMBER,
    C_ALLOCTYPE, C_ADDEVENT, C_ADDEVENT2, C_FURTHEST, C_NEAREST, C_COMPUTERESP, C_WILLRESPOND, C_NEW, C_DELETE, C_SAY, C_RAND, C_DAMAGE,
    C_SAVE, C_LOAD, C_PEDLOAD, C_PEDSAVE, C_PEDPC, C_DRUNKCLEAR, C_DRUNKSET, C_PRUNE, C_GETWEAPON, C_WJUSTDOWN, C_GETTARGET, C_AUDIO,
    C_FRONTEND, C_SLOT, C_LOOKCLEAR, C_LOOKSET, C_LOOKTIMER, C_VEHINTEREST, C_RPHANIM, C_WATER, C_COLMODEL, C_INTERIOR, C_CLONE, C_DTOR,
    C_TRAP };
inline constexpr const char* CODE_NAMES[]{
    "fat_muscle","wanted_set","wanted_nodrop","wanted_cheat","wanted_update","world_add","world_remove","lighting","clump_assoc","static_assoc","add_anim","skill","skill_type",
    "cam_set_mode","cam_clear_mode","cam_1st","clear_crosshair","mark_target","clean_up_ref","register_ref","same_group","find_task","event_of_type","task_jetpack","task_use_gun",
    "task_swim","coop","stop_weapon_effect","weapon_update","set_current_weapon","range_mult","is_tag","tag_alpha","los","los_flags","bone_pos","transformed_bone","set_real_move_anim",
    "wants_to_attack","weapon_switch","anim_groups","group_behaviour","player_coors","player_vehicle","dist_to_line","count_members","get_member","remove_followers","is_member",
    "allocator_type","add_event","add_event_detail","dist_furthest","dist_nearest","compute_response","will_respond","event_new","event_delete","say","rand","damage_event",
    "save_wb","load_wb","ped_load","ped_save","ped_process_control","clear_drunk_blur","set_drunk_blur","prune_refs","pad_get_weapon","pad_weapon_just_down","pad_get_target","weapon_audio",
    "frontend_audio","player_slot","clear_look","set_look","look_timer","vehicle_of_interest","update_rphanim","water_level","col_model","interior_transition","clone","dtor",
    "trap"};
struct TR { uint32 c,mask,a,b,d,e,f,g; };   // mask: bit q = argumento q es un puntero
inline std::array<TR,512> tr{}; inline int ntr{}; inline bool trOverflow{};
inline void T(uint32 c,uint32 mask,uint32 a=0,uint32 b=0,uint32 d=0,uint32 e=0,uint32 f=0,uint32 g=0) { if(ntr<int(tr.size()))tr[ntr++]={c,mask,a,b,d,e,f,g}; else trOverflow=true; }
inline json TraceJson() {
    json r=json::array();
    for(int k=0;k<ntr;++k){ const auto& e=tr[k]; json x=json::array({CODE_NAMES[e.c]});
        const uint32 v[]{e.a,e.b,e.d,e.e,e.f,e.g};
        for(int q=0;q<6;++q)x.push_back((e.mask>>q)&1 ? Name(v[q]) : json(v[q]));
        r.push_back(x); }
    return r;
}

// ======================= resultados de los stubs (por caso) =======================
inline float lightResult{}, rangeMult{}, distFurthest{}, distNearest{}, waterLevel{};
inline uint32 skillResult{}, countResult{}, memberMask{}, isMemberSalt{}, randCounter{}, randSalt{}, eventOfTypeMask{}, findTaskMask{};
inline bool coopResult{}, sameGroupResult{}, isTagResult{}, losResult{}, wantsAttack{}, willRespond{}, getWeaponResult{}, weaponJustDown{}, getTarget{},
            using1st{}, waterResult{}, interiorResult{}, playerVehResult{};
inline uint8 tagAlpha{}; inline int16 taskIdResult{}; inline int32 slotResult{};
inline void* losHit{}; inline void* jetpackTask{}; inline void* useGunTask{}; inline void* swimTask{}; inline void* clumpAssoc[2]{}; inline uint32 nFindTask{}, nEventOfType{};
inline uint32 loadCounter{}; inline std::array<std::array<uint8,0x88>,4> saveBytes{}; inline std::array<int32,4> saveSizes{}; inline int nSave{};
inline uint32 newCount{};

// ======================= stubs =======================
inline float __cdecl FakeFatMuscle(eStatModAbilities m) { const float v=900.f+float(uint32(m))*13.25f; T(C_FATMUSCLE,0,uint32(m),F(v)); return v; }
inline void __fastcall FakeWSet(void* self,void*,int32 l) { T(C_WSET,1,U(self),uint32(l)); }
inline void __fastcall FakeWNoDrop(void* self,void*,int32 l) { T(C_WNODROP,1,U(self),uint32(l)); }
inline void __fastcall FakeWCheat(void* self,void*,int32 l) { T(C_WCHEAT,1,U(self),uint32(l)); }
inline void __fastcall FakeWUpdate(void* self,void*) { T(C_WUPDATE,1,U(self)); }
inline void __cdecl FakeWorldAdd(CEntity* e) { T(C_WADD,1,U(e)); }
inline void __cdecl FakeWorldRemove(CEntity* e) { T(C_WREMOVE,1,U(e)); }
inline float __fastcall FakeLight(void* self,void*) { T(C_LIGHT,1,U(self),F(lightResult)); return lightResult; }
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id) { void* r=id<=5 ? clumpAssoc[id&1] : id==0xE0 ? clumpAssoc[0] : nullptr; T(C_GETASSOC,5,U(c),id,U(r)); return static_cast<CAnimBlendAssociation*>(r); }
inline CAnimBlendStaticAssociation* __cdecl FakeStatAssoc(int32 g,int32 id) { void* r=Stat(int(uint32(g+id)&1)); T(C_STATASSOC,4,uint32(g),uint32(id),U(r)); return static_cast<CAnimBlendStaticAssociation*>(r); }
inline CAnimBlendAssociation* __cdecl FakeAddAnim(RpClump* c,int32 g,int32 id) { uint8* a=Assoc(2); At<float>(a,0x18)=-7.f; At<float>(a,0x1C)=-9.f; T(C_ADDANIM,9,U(c),uint32(g),uint32(id),U(a)); return reinterpret_cast<CAnimBlendAssociation*>(a); }
inline int32 __fastcall FakeSkill(void* self,void*) { T(C_SKILL,1,U(self),skillResult); return int32(skillResult); }
inline int32 __fastcall FakeSkillT(void* self,void*,int32 t) { T(C_SKILLT,1,U(self),uint32(t),skillResult); return int32(skillResult); }
inline void __fastcall FakeCamMode(void* self,void*,int32 m,int32 a,int32 b) { T(C_CAMMODE,1,U(self),uint32(m)&0xFFFF,uint32(a)&0xFFFF,uint32(b)&0xFFFF); }
inline void __fastcall FakeCamClear(void* self,void*) { T(C_CAMCLEAR,1,U(self)); }
inline bool __fastcall FakeCam1st(void* self,void*) { T(C_CAM1ST,1,U(self),using1st); return using1st; }
inline void __cdecl FakeCrossHair(int32 id) { T(C_CROSSHAIR,0,uint32(id)); }
inline void __cdecl FakeMark(int32 id,CVector p,uint8 r,uint8 g,uint8 b,uint8 a,float size,uint8 clear) { T(C_MARK,0,uint32(id)|uint32(clear)<<16,F(p.x),F(p.y),F(p.z),uint32(r)|uint32(g)<<8|uint32(b)<<16|uint32(a)<<24,F(size)); }
inline void __fastcall FakeCleanUpRef(void* self,void*,CEntity** ref) { T(C_CLEANREF,3,U(self),U(ref)); }
inline void __fastcall FakeRegisterRef(void* self,void*,CEntity** ref) { T(C_REGREF,3,U(self),U(ref)); }
inline bool __cdecl FakeSameGroup(const CPed* a,const CPed* b) { T(C_SAMEGROUP,3,U(a),U(b),sameGroupResult); return sameGroupResult; }
inline void* __fastcall FakeFindTask(void* self,void*,int32 t) { const uint32 n=nFindTask++; void* r=(findTaskMask>>(n%16))&1 ? Task(4) : nullptr; T(C_FINDTASK,9,U(self),uint32(t),n,U(r)); return r; }
inline void* __fastcall FakeEventOfType(void* self,void*,int32 t) { const uint32 n=nEventOfType++; void* r=(eventOfTypeMask>>(n%16))&1 ? Ev(1) : nullptr; T(C_EVENTOFTYPE,5,U(self),uint32(t),U(r)); return r; }
inline void* __fastcall FakeJetpack(void* self,void*) { T(C_JETPACK,3,U(self),U(jetpackTask)); return jetpackTask; }
inline void* __fastcall FakeUseGun(void* self,void*) { T(C_USEGUN,3,U(self),U(useGunTask)); return useGunTask; }
inline void* __fastcall FakeSwim(void* self,void*) { T(C_SWIM,3,U(self),U(swimTask)); return swimTask; }
inline bool __cdecl FakeCoop() { T(C_COOP,0,coopResult); return coopResult; }
inline void __fastcall FakeStopEffect(void* self,void*) { T(C_STOPEFFECT,1,U(self)); }
inline void __fastcall FakeWeaponUpdate(void* self,void*,CPed* owner) { T(C_WUPD,3,U(self),U(owner)); }
// SetCurrentWeapon: el stub fija el slot activo de forma determinista (tipo % 13)
inline void __fastcall FakeSetWeapon(void* self,void*,int32 t) { T(C_SETWEAPON,1,U(self),uint32(t)); At<int8>(self,0x718)=int8(uint32(t)%13); }
inline float __cdecl FakeRangeMult(CEntity* t,CEntity* o) { T(C_RANGEMULT,3,U(t),U(o),F(rangeMult)); return rangeMult; }
inline bool __cdecl FakeIsTag(const CEntity& e) { T(C_ISTAG,1,U(&e),isTagResult); return isTagResult; }
inline uint8 __cdecl FakeTagAlpha(const CEntity& e) { T(C_TAGALPHA,1,U(&e),tagAlpha); return tagAlpha; }
inline bool __cdecl FakeLOS(const CVector& o,const CVector& t,CColPoint& cp,CEntity*& hit,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7,bool b8) {
    T(C_LOS,0,F(o.x),F(o.y),F(o.z),F(t.x),F(t.y),F(t.z));
    T(C_LOSFLAGS,0,uint32(b1)|b2<<1|b3<<2|b4<<3|b5<<4|b6<<5|b7<<6|b8<<7,losResult);
    if(losResult)hit=static_cast<CEntity*>(losHit);
    return losResult; }
inline void __fastcall FakeBone(void* self,void*,CVector* out,int32 bone,bool upd) {
    const CVector& p=*reinterpret_cast<CVector*>(At<uint8*>(self,0x14) ? At<uint8*>(self,0x14)+0x30 : static_cast<uint8*>(self)+4);
    *out=CVector{p.x+0.125f*float(bone),p.y-0.25f,p.z+0.5f+0.0625f*float(bone)}; T(C_BONE,1,U(self),uint32(bone),upd); }
inline void __fastcall FakeTBone(void* self,void*,CVector* io,int32 bone,bool upd) {
    T(C_TBONE,1,U(self),F(io->x),F(io->y),F(io->z),uint32(bone)|uint32(upd)<<16);
    const CVector& p=*reinterpret_cast<CVector*>(At<uint8*>(self,0x14) ? At<uint8*>(self,0x14)+0x30 : static_cast<uint8*>(self)+4);
    *io=CVector{p.x+io->x,p.y+io->y*2.f,p.z+1.f+float(bone)*0.01f}; }
inline void __fastcall FakeRealMove(void* self,void*) { T(C_REALMOVE,1,U(self)); }
inline bool __fastcall FakeWantsAttack(void* self,void*) { T(C_WANTSATTACK,1,U(self),wantsAttack); return wantsAttack; }
inline void __fastcall FakeWeaponSwitch(void* self,void*,CPad* pad) { T(C_WEAPSWITCH,3,U(self),U(pad)); }
inline void __fastcall FakeAnimGroups(void* self,void*) { T(C_ANIMGROUPS,1,U(self)); }
inline void __fastcall FakeGroupBeh(void* self,void*,CPad* pad) { T(C_GROUPBEH,3,U(self),U(pad)); }
inline CVector playerCoors{};
inline CVector* __cdecl FakeCoors(CVector* out,int32 id) { *out=playerCoors; T(C_COORS,0,uint32(id)); return out; }
inline void* __cdecl FakePlayerVeh(int32 id,bool remote) { void* r=playerVehResult ? Veh(1) : nullptr; T(C_PLAYERVEH,4,uint32(id),remote,U(r)); return r; }
inline float __cdecl FakeDistLine(const CVector* a,const CVector* b,const CVector* p) {
    const float r=std::fabs(p->x-a->x)*0.75f+std::fabs(p->y-b->y)*0.5f+std::fabs(a->z)+std::fabs(b->z)+std::fabs(p->z);
    T(C_DISTLINE,0,F(a->x),F(a->y),F(b->x),F(b->y),F(p->x),F(p->y)); return r; }
inline int32 __fastcall FakeCount(void* self,void*) { T(C_COUNT,1,U(self),countResult); return int32(countResult); }
inline void* __fastcall FakeGetMember(void* self,void*,int32 i) { void* r=(memberMask>>(uint32(i)%8))&1 ? Mem(int(uint32(i)%NMEM)) : nullptr; T(C_GETMEMBER,5,U(self),uint32(i),U(r)); return r; }
inline void __fastcall FakeRemoveFollowers(void* self,void*,bool b) { T(C_REMOVEFOLLOW,1,U(self),b); }
inline bool __fastcall FakeIsMember(void* self,void*,const CPed* p) { const bool r=((U(p)>>4)^isMemberSalt)&1; T(C_ISMEMBER,3,U(self),U(p),r); return r; }
inline void __fastcall FakeAllocType(void* self,void*,uint32 t) { T(C_ALLOCTYPE,1,U(self),t); }
inline bool __fastcall FakeAddEvent(void* self,void*,uint8* ge) {
    uint8* ev=At<uint8*>(ge,0x10);
    T(C_ADDEVENT,0x19,U(self),At<uint32>(ge,4),At<uint32>(ge,8)&0xFF,U(At<void*>(ge,0xC)),U(ev));
    if(ev>=arena.data()&&ev<arena.data()+arena.size())
        T(C_ADDEVENT2,0x10,uint32(reinterpret_cast<CEvent*>(ev)->GetEventType()),At<uint32>(ev,4),uint32(ev[8])|uint32(ev[0xC])<<8|uint32(At<uint16>(ev,0x10))<<16,uint32(At<uint16>(ev,0xE)),U(At<void*>(ev,0x18)),At<uint32>(ev,0x14));
    return true; }
inline float __fastcall FakeFurthest(void* self,void*) { T(C_FURTHEST,1,U(self),F(distFurthest)); return distFurthest; }
inline float __fastcall FakeNearest(void* self,void*,CPed** out) { T(C_NEAREST,3,U(self),U(out),F(distNearest)); return distNearest; }
inline void __fastcall FakeComputeResp(uint8* self,void*,CPedGroup* g) {
    // relleno sin inicializar en +9..+0xB y +0xD: solo se comparan los campos
    T(C_COMPUTERESP,0x23,U(self),U(g),At<uint32>(self,4),uint32(self[8])|uint32(self[0xC])<<8|uint32(At<uint16>(self,0x10))<<16,uint32(At<uint16>(self,0xE))|At<uint32>(self,0x14)<<16,U(At<void*>(self,0x18)));
    At<int16>(self,0xE)=taskIdResult; }
inline bool __fastcall FakeWillRespond(uint8* self,void*) { T(C_WILLRESPOND,1,U(self),uint32(At<uint16>(self,0xE)),willRespond); return willRespond; }
inline void* __cdecl FakeEventNew(unsigned n) { ++newCount; std::memset(arena.data(),0xCD,arena.size()); T(C_NEW,0,n); return arena.data(); }
inline void __cdecl FakeEventDelete(void* p) { T(C_DELETE,1,U(p)); }
inline int16 __fastcall FakeSay(void* self,void*,uint32 ctx,uint32 delay,float prob,bool a,bool b,bool c) { T(C_SAY,1,U(self),ctx,delay,F(prob),uint32(a)|b<<1|c<<2); return int16(-1); }
inline uint32 NextRand() { randCounter=randCounter*1103515245u+12345u+randSalt; return (randCounter>>16)&0x7FFF; }
inline int __cdecl FakeRand() { const uint32 v=NextRand(); T(C_RAND,0,v); return int(v); }
inline uint16 __cdecl FakeRandomNumber() { const uint32 v=NextRand(); T(C_RAND,0,v); return uint16(v); }
inline bool __cdecl FakeDamage(CPed* v,CEntity* c,int32 type,int32 amount,int32 piece,uint8 dir) { T(C_DAMAGE,3,U(v),U(c),uint32(type),uint32(amount),uint32(piece),dir); return true; }
inline bool __cdecl FakeSaveWB(void* p,int32 n) {
    if(nSave<int(saveBytes.size())){ auto& b=saveBytes[nSave]; b.fill(0); saveSizes[nSave]=n; if(n>0)std::memcpy(b.data(),p,std::min<size_t>(size_t(n),b.size()));
        // El EXE deja sin inicializar los bytes altos del nivel de busqueda (+5..+7) y del arma elegida (+0x81..+0x83)
        if(n==0x84){ b[5]=b[6]=b[7]=0; b[0x81]=b[0x82]=b[0x83]=0; } ++nSave; }
    T(C_SAVE,0,uint32(n)); return true; }
inline bool __cdecl FakeLoadWB(void* p,int32 n) { auto* b=static_cast<uint8*>(p); for(int32 k=0;k<n;++k)b[k]=uint8(loadCounter++*37u+11u); T(C_LOAD,0,uint32(n)); return true; }
inline bool __fastcall FakePedLoad(void* self,void*) { T(C_PEDLOAD,1,U(self)); return true; }
inline bool __fastcall FakePedSave(void* self,void*) { T(C_PEDSAVE,1,U(self)); return true; }
// CPed::ProcessControl: ademas de trazar, marca el aplazamiento segun el caso (bit 6 del byte 0x1C)
inline bool postpone{};
inline void __fastcall FakePedPC(void* self,void*) { T(C_PEDPC,1,U(self)); if(postpone)At<uint8>(self,0x1C)|=0x40; }
inline void __cdecl FakeDrunkClear() { T(C_DRUNKCLEAR,0); }
inline void __cdecl FakeDrunkSet(float f) { T(C_DRUNKSET,0,F(f)); }
inline void __fastcall FakePrune(void* self,void*) { T(C_PRUNE,1,U(self)); }
inline int32 __fastcall FakeGetWeapon(void* self,void*,CPed* p) { T(C_GETWEAPON,3,U(self),U(p),getWeaponResult); return getWeaponResult ? 255 : 0; }
inline bool __fastcall FakeJustDown(void* self,void*,CPed* p) { T(C_WJUSTDOWN,3,U(self),U(p),weaponJustDown); return weaponJustDown; }
inline bool __fastcall FakeGetTarget(void* self,void*) { T(C_GETTARGET,1,U(self),getTarget); return getTarget; }
inline void __fastcall FakeWeapAudio(void* self,void*,int32 e) { T(C_AUDIO,1,U(self),uint32(e)); }
inline void __fastcall FakeFrontend(void* self,void*,int32 e,float v,float s) { T(C_FRONTEND,1,U(self),uint32(e),F(v),F(s)); }
inline int32 __cdecl FakeSlot(void* p) { T(C_SLOT,1,U(p),uint32(slotResult)); return slotResult; }
inline void __fastcall FakeLookClear(void* self,void*) { T(C_LOOKCLEAR,1,U(self)); }
inline void __fastcall FakeLookSet(void* self,void*,float d,bool a,bool b) { T(C_LOOKSET,1,U(self),F(d),a,b); }
inline void __fastcall FakeLookTimer(void* self,void*,uint32 t) { T(C_LOOKTIMER,1,U(self),t); }
inline void __cdecl FakeVehInterest(CVehicle* v) { T(C_VEHINTEREST,1,U(v)); }
inline void __fastcall FakeRpHAnim(void* self,void*) { T(C_RPHANIM,1,U(self)); }
inline bool __cdecl FakeWater(float x,float y,float z,float* out,uint8 touch,CVector* n) { T(C_WATER,0x28,F(x),F(y),F(z),U(out),touch,U(n)); if(waterResult)*out=waterLevel; return waterResult; }
inline void* __fastcall FakeColModel(void* self,void*) { T(C_COLMODEL,1,U(self)); return colModel.data(); }
inline bool __cdecl FakeInterior() { T(C_INTERIOR,0,interiorResult); return interiorResult; }
inline void* __fastcall FakeClone(void* self,void*) { T(C_CLONE,1,U(self)); return Ev(1); }
inline void* __fastcall FakeDtor(void* self,void*,uint32 flags) { T(C_DTOR,1,U(self),flags); return self; }
inline int32 __fastcall FakeTaskType(void* self,void*) { return At<int32>(self,0x40); }   // sin traza: getter puro
template<int N> void __fastcall Trap(void* self,void*) { T(C_TRAP,1,U(self),uint32(N)); }
template<int... N> void FillTraps(std::array<void*,64>& v,std::integer_sequence<int,N...>) { ((v[N]=reinterpret_cast<void*>(&Trap<N>)),...); }

// ======================= hooks y parches =======================
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }
inline uintptr_t Body(uintptr_t a) {
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
template<class M> inline uintptr_t MAddr(M m) { static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,50> fns{{
    {"CPlayerPed","AnnoyPlayerPed",0x60A040,0,-1,256},{"CPlayerPed","Busted",0x609EF0,0,-1,64},
    {"CPlayerPed","CanIKReachThisTarget",0x609F80,0,-1,512},{"CPlayerPed","CanPlayerStartMission",0x609590,0,-1,512},
    {"CPlayerPed","CheatWantedLevel",0x609F50,0,-1,64},{"CPlayerPed","Clear3rdPersonMouseTarget",0x609ED0,0,-1,64},
    {"CPlayerPed","ClearAdrenaline",0x60A070,0,-1,128},{"CPlayerPed","ClearWeaponTarget",0x609C80,0,-1,128},
    {"CPlayerPed","DeactivatePlayerPed",0x609520,0,-1,64},{"CPlayerPed","DisbandPlayerGroup",0x60A0A0,0,-1,128},
    {"CPlayerPed","DoStuffToGoOnFire",0x60A020,0,-1,64},{"CPlayerPed","DoesPlayerWantNewWeapon",0x609710,0,-1,512},
    {"CPlayerPed","DoesTargetHaveToBeBroken",0x60C0C0,0,-1,512},{"CPlayerPed","EvaluateTarget",0x60D020,0,-1,1024},
    {"CPlayerPed","FindPedToAttack",0x60C5F0,0,-1,256},{"CPlayerPed","FindTargetPriority",0x609DE0,0,-1,512},
    {"CPlayerPed","ForceGroupToAlwaysFollow",0x60C7C0,0,-1,128},{"CPlayerPed","ForceGroupToNeverFollow",0x60C800,0,-1,128},
    {"CPlayerPed","GetButtonSprintResults",0x60A820,0,-1,256},{"CPlayerPed","GetPadFromPlayer",0x609560,0,-1,64},
    {"CPlayerPed","GetPlayerInfoForThisPlayerPed",0x609FF0,0,-1,64},{"CPlayerPed","GetWantedLevel",0x41BE60,0,-1,64},
    {"CPlayerPed","GetWeaponRadiusOnScreen",0x609CD0,0,-1,512},{"CPlayerPed","HandlePlayerBreath",0x60A8D0,0,-1,512},
    {"CPlayerPed","HandleSprintEnergy",0x60A550,0,-1,512},{"CPlayerPed","IsHidden",0x609620,0,-1,128},
    {"CPlayerPed","LOSBlockedBetweenPeds",0x60B550,0,-1,256},{"CPlayerPed","Load",0,0x86D168,25,128},
    {"CPlayerPed","MakeChangesForNewWeapon-",0x60B460,0,-1,256},{"CPlayerPed","MakeChangesForNewWeapon-BySlot",0x60D000,0,-1,256},
    {"CPlayerPed","MakeGroupRespondToPlayerTakingDamage",0x60A110,0,-1,128},{"CPlayerPed","MakePlayerGroupDisappear",0x60A440,0,-1,128},
    {"CPlayerPed","MakePlayerGroupReappear",0x60A4B0,0,-1,128},{"CPlayerPed","PedCanBeTargettedVehicleWise",0x609D90,0,-1,128},
    {"CPlayerPed","PickWeaponAllowedFor2Player",0x609800,0,-1,128},{"CPlayerPed","PlayerHasJustAttackedSomeone",0x60D5A0,0,-1,64},
    {"CPlayerPed","ProcessControl",0,0x86D168,10,1024},{"CPlayerPed","ProcessPlayerWeapon",0x6097F0,0,-1,64},
    {"CPlayerPed","ReApplyMoveAnims",0x609650,0,-1,256},{"CPlayerPed","ReactivatePlayerPed",0x609540,0,-1,64},
    {"CPlayerPed","RemovePlayerPed",0x6094A0,0,-1,128},{"CPlayerPed","ResetPlayerBreath",0x60A8A0,0,-1,64},
    {"CPlayerPed","ResetSprintEnergy",0x60A530,0,-1,64},{"CPlayerPed","Save",0,0x86D168,24,128},
    {"CPlayerPed","SetMoveAnim",0,0x86D168,23,64},{"CPlayerPed","SetPlayerMoveBlendRatio",0x60C520,0,-1,256},
    {"CPlayerPed","SetWantedLevel",0x609F10,0,-1,64},{"CPlayerPed","SetWantedLevelNoDrop",0x609F30,0,-1,64},
    {"CPlayerPed","TellGroupToStartFollowingPlayer",0x60A1D0,0,-1,512},{"CPlayerPed","UpdateCameraWeaponModes",0x609830,0,-1,128},
}};

struct Region { uintptr_t at; size_t n; std::vector<uint8> saved; };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"playerped51-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playerped51-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("playerped51");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"playerped51-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,50> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    FillTraps(vt,std::make_integer_sequence<int,64>{}); FillTraps(taskVt,std::make_integer_sequence<int,64>{}); FillTraps(evVt,std::make_integer_sequence<int,64>{});
    vt[0]=reinterpret_cast<void*>(&FakeDtor);
    taskVt[4]=reinterpret_cast<void*>(&FakeTaskType);
    evVt[0]=reinterpret_cast<void*>(&FakeDtor); evVt[4]=reinterpret_cast<void*>(&FakeClone);

    // estado global guardado
    std::vector<Region> regions{{PLAYERS,2*0x190,{}},{0xB7CD74,1,{}},{0xB7CB4C,4,{}},{0xB7CB5C,4,{}},{0xB7CB64,4,{}},{0xB7CB84,4,{}},{0x96916E,1,{}},
        {0x96A8B0,1,{}},{0xB72914,4,{}},{CAMERA,0xD78,{}},{CPADS,2*0x134,{}},{GROUPS,8*GROUPSZ,{}},{0xC0BC08,7,{}},{0xB74490,4,{}}};
    for(auto& r:regions)r.saved.assign(reinterpret_cast<uint8*>(r.at),reinterpret_cast<uint8*>(r.at)+r.n);
    auto RestoreAll=[&]{ for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.at),r.saved.data(),r.n); };
    auto AllRestored=[&]{ for(auto& r:regions)if(std::memcmp(reinterpret_cast<void*>(r.at),r.saved.data(),r.n))return false; return true; };

    // CPed::Load/Save/ProcessControl de la DLL: cuerpos tomados de la vtable exportada de CPed (no del thunk virtual)
    void** const pedVt=ReversibleHooks::detail::GetVTableAddress("CPed");
    if(!pedVt){Write({{"event","error"},{"reason","CPed vtable not found"}});return;}

    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,auto stub){ void* s=reinterpret_cast<void*>(stub); if(exeAddr)plan.push_back({exeAddr,s}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,s}); } };
    add(0x559AF0,uintptr_t(&CStats::GetFatAndMuscleModifier),&FakeFatMuscle);
    add(0x562470,MAddr(&CWanted::SetWantedLevel),&FakeWSet);
    add(0x562570,MAddr(&CWanted::SetWantedLevelNoDrop),&FakeWNoDrop);
    add(0x562540,MAddr(&CWanted::CheatWantedLevel),&FakeWCheat);
    add(0x562C90,MAddr(&CWanted::Update),&FakeWUpdate);
    add(0x563220,uintptr_t(&CWorld::Add),&FakeWorldAdd);
    add(0x563280,uintptr_t(&CWorld::Remove),&FakeWorldRemove);
    add(0x544850,MAddr(&CPhysical::GetLightingTotal),&FakeLight);
    add(0x4D68B0,uintptr_t(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),&FakeGetAssoc);
    add(0x4D3A60,uintptr_t(static_cast<CAnimBlendStaticAssociation*(*)(AssocGroupId,AnimationId)>(&CAnimManager::GetAnimAssociation)),&FakeStatAssoc);
    add(0x4D3AA0,uintptr_t(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation)),&FakeAddAnim);
    add(0x5E6580,MAddr(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)),&FakeSkill);
    add(0x5E3B60,MAddr(static_cast<eWeaponSkill(CPed::*)(eWeaponType)>(&CPed::GetWeaponSkill)),&FakeSkillT);
    add(0x50BFB0,MAddr(&CCamera::SetNewPlayerWeaponMode),&FakeCamMode);
    add(0x50AB10,MAddr(&CCamera::ClearPlayerWeaponMode),&FakeCamClear);
    add(0x50BFF0,MAddr(&CCamera::Using1stPersonWeaponMode),&FakeCam1st);
    add(0x742C60,uintptr_t(&CWeaponEffects::ClearCrossHair),&FakeCrossHair);
    add(0x742BF0,uintptr_t(&CWeaponEffects::MarkTarget),&FakeMark);
    add(0x571A00,MAddr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),&FakeCleanUpRef);
    add(0x571B70,MAddr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),&FakeRegisterRef);
    add(0x5F7F40,uintptr_t(&CPedGroups::AreInSameGroup),&FakeSameGroup);
    add(0x681740,MAddr(&CTaskManager::FindActiveTaskByType),&FakeFindTask);
    add(0x4AB650,MAddr(&CEventGroup::GetEventOfType),&FakeEventOfType);
    add(0x601110,MAddr(&CPedIntelligence::GetTaskJetPack),&FakeJetpack);
    add(0x600F70,MAddr(&CPedIntelligence::GetTaskUseGun),&FakeUseGun);
    add(0x601070,MAddr(&CPedIntelligence::GetTaskSwim),&FakeSwim);
    add(0x441390,uintptr_t(&CGameLogic::IsCoopGameGoingOn),&FakeCoop);
    add(0x73B360,MAddr(&CWeapon::StopWeaponEffect),&FakeStopEffect);
    add(0x73DB40,MAddr(&CWeapon::Update),&FakeWeaponUpdate);
    add(0x5E6280,MAddr(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),&FakeSetWeapon);
    add(0x73B380,uintptr_t(&CWeapon::TargetWeaponRangeMultiplier),&FakeRangeMult);
    add(0x49CCE0,uintptr_t(&CTagManager::IsTag),&FakeIsTag);
    add(0x49CF90,uintptr_t(static_cast<uint8(*)(const CEntity&)>(&CTagManager::GetAlpha)),&FakeTagAlpha);
    add(0x56BA00,uintptr_t(&CWorld::ProcessLineOfSight),&FakeLOS);
    add(0x5E4280,MAddr(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),&FakeBone);
    add(0x5E01C0,MAddr(&CPed::GetTransformedBonePosition),&FakeTBone);
    add(0x60A9C0,MAddr(&CPlayerPed::SetRealMoveAnim),&FakeRealMove);
    add(0x60CC50,MAddr(&CPlayerPed::PlayerWantsToAttack),&FakeWantsAttack);
    add(0x60D850,MAddr(&CPlayerPed::ProcessWeaponSwitch),&FakeWeaponSwitch);
    add(0x6098F0,MAddr(&CPlayerPed::ProcessAnimGroups),&FakeAnimGroups);
    add(0x60D350,MAddr(&CPlayerPed::ProcessGroupBehaviour),&FakeGroupBeh);
    add(0x56E010,uintptr_t(&FindPlayerCoors),&FakeCoors);
    add(0x56E0D0,uintptr_t(&FindPlayerVehicle),&FakePlayerVeh);
    add(0x417610,uintptr_t(&CCollision::DistToLine),&FakeDistLine);
    add(0x5F6AA0,MAddr(&CPedGroupMembership::CountMembersExcludingLeader),&FakeCount);
    add(0x5F69B0,MAddr(&CPedGroupMembership::GetMember),&FakeGetMember);
    add(0x5FB190,MAddr(&CPedGroupMembership::RemoveAllFollowers),&FakeRemoveFollowers);
    add(0x5F6A10,MAddr(&CPedGroupMembership::IsMember),&FakeIsMember);
    add(0x5FBB70,MAddr(&CPedGroupIntelligence::SetDefaultTaskAllocatorType),&FakeAllocType);
    add(0x5F7470,MAddr(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent)),&FakeAddEvent);
    add(0x5FB010,MAddr(&CPedGroup::FindDistanceToFurthestMember),&FakeFurthest);
    add(0x5FB0A0,MAddr(&CPedGroup::FindDistanceToNearestMember),&FakeNearest);
    add(0x4B57A0,MAddr(static_cast<void(CEventEditableResponse::*)(CPedGroup*)>(&CEventEditableResponse::ComputeResponseTaskType)),&FakeComputeResp);
    add(0x4AC490,MAddr(&CEventEditableResponse::WillRespond),&FakeWillRespond);
    add(0x4B5620,uintptr_t(static_cast<void*(*)(unsigned)>(&CEvent::operator new)),&FakeEventNew);
    add(0x4B5630,uintptr_t(static_cast<void(*)(void*)>(&CEvent::operator delete)),&FakeEventDelete);
    add(0x5EFFE0,MAddr(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),&FakeSay);
    add(0x821B1E,0,&FakeRand);
    add(0,uintptr_t(&CGeneral::GetRandomNumber),&FakeRandomNumber);
    add(0x73A530,uintptr_t(&CWeapon::GenerateDamageEvent),&FakeDamage);
    add(0x5D1270,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),&FakeSaveWB);
    add(0x5D1300,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),&FakeLoadWB);
    add(0x5D4640,uintptr_t(pedVt[25]),&FakePedLoad);
    add(0x5D5730,uintptr_t(pedVt[24]),&FakePedSave);
    add(0x5E8CD0,uintptr_t(pedVt[10]),&FakePedPC);
    add(0x71D780,uintptr_t(&CMBlur::ClearDrunkBlur),&FakeDrunkClear);
    add(0x71D730,uintptr_t(&CMBlur::SetDrunkBlur),&FakeDrunkSet);
    add(0x571A90,MAddr(&CEntity::PruneReferences),&FakePrune);
    add(0x540180,MAddr(&CPad::GetWeapon),&FakeGetWeapon);
    add(0x540250,MAddr(&CPad::WeaponJustDown),&FakeJustDown);
    add(0x540670,MAddr(&CPad::GetTarget),&FakeGetTarget);
    add(0x4E69F0,MAddr(&CAEPedWeaponAudioEntity::AddAudioEvent),&FakeWeapAudio);
    add(0x506EA0,MAddr(&CAudioEngine::ReportFrontendAudioEvent),&FakeFrontend);
    add(0x563FA0,uintptr_t(&CWorld::FindPlayerSlotWithPedPointer),&FakeSlot);
    add(0x5E1950,MAddr(&CPed::ClearLookFlag),&FakeLookClear);
    add(0x5DEDC0,MAddr(static_cast<void(CPed::*)(float,bool,bool)>(&CPed::SetLookFlag)),&FakeLookSet);
    add(0x5DF8D0,MAddr(&CPed::SetLookTimer),&FakeLookTimer);
    add(0x423DE0,uintptr_t(&CCarCtrl::RegisterVehicleOfInterest),&FakeVehInterest);
    add(0x532B20,MAddr(&CEntity::UpdateRpHAnim),&FakeRpHAnim);
    add(0x6EB690,uintptr_t(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),&FakeWater);
    add(0x535300,MAddr(&CEntity::GetColModel),&FakeColModel);
    add(0x43E400,uintptr_t(&CEntryExitManager::WeAreInInteriorTransition),&FakeInterior);
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }

    std::array<bool,50> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        auto& focus=*reinterpret_cast<uint8*>(0xB7CD74);
        auto& pedPoolPtr=*reinterpret_cast<void**>(0xB74490);
        const float qnan=std::numeric_limits<float>::quiet_NaN();
        const float inf=std::numeric_limits<float>::infinity();
        static const int32 weaponTypes[]{0,1,4,8,9,16,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,41,42,43,46};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto rnd=[&](uint32 n){ return next()%n; };
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5051u; if(!st)st=1;
                    ntr=0; trOverflow=false; nSave=0; for(auto& b:saveBytes)b.fill(0); saveSizes.fill(-1); loadCounter=rnd(1000); newCount=0; nFindTask=0; nEventOfType=0;
                    randCounter=next(); randSalt=next()|1;
                    lightResult=rnd(16)==0 ? qnan : rnd(4)==0 ? 0.05f : uni(-0.2f,1.2f);
                    rangeMult=rnd(8)==0 ? 1.f : uni(0.f,3.f); distFurthest=rnd(16)==0 ? qnan : rnd(6)==0 ? (rnd(2) ? 3.f : 10.f) : uni(0.f,30.f);
                    distNearest=rnd(16)==0 ? qnan : rnd(6)==0 ? (rnd(2) ? 20.f : 100.f) : uni(0.f,150.f); waterLevel=uni(-5.f,25.f); if(rnd(32)==0)waterLevel=qnan;
                    static const uint32 skills[]{1,1,1,1,0,2,3}; skillResult=skills[rnd(7)];
                    countResult=rnd(4)==0 ? 0 : rnd(8)==0 ? uint32(-1) : rnd(8); memberMask=next(); isMemberSalt=next(); eventOfTypeMask=next(); findTaskMask=rnd(2) ? 0 : next();
                    coopResult=rnd(6)==0; sameGroupResult=rnd(4)==0; isTagResult=rnd(2); losResult=rnd(2); wantsAttack=rnd(2); willRespond=rnd(2); getWeaponResult=rnd(2);
                    weaponJustDown=rnd(2); getTarget=rnd(2); using1st=rnd(2); waterResult=rnd(4)!=0; interiorResult=rnd(4)==0; playerVehResult=rnd(4)==0;
                    tagAlpha=uint8(rnd(2) ? 255 : next()); taskIdResult=int16(rnd(2) ? -1 : rnd(2000)); slotResult=int32(rnd(2)); postpone=rnd(8)==0;
                    playerCoors=CVector{uni(-100.f,100.f),uni(-100.f,100.f),uni(-10.f,50.f)};

                    // ---- fixtures ----
                    auto randMat=[&](uint8* m,const CVector& pos){ std::memset(m,0,MATSZ); const float a=uni(-3.2f,3.2f), c=std::cos(a), s=std::sin(a);
                        At<CVector>(m,0x00)=CVector{c,s,0.f}; At<CVector>(m,0x10)=CVector{-s,c,0.f}; At<CVector>(m,0x20)=CVector{0.f,0.f,1.f}; At<CVector>(m,0x30)=pos; };
                    const CVector centre{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                    auto nearPos=[&](float r){ return centre+CVector{uni(-r,r),uni(-r,r),uni(-r*0.3f,r*0.3f)}; };
                    auto setEntityBase=[&](uint8* e,int matIdx,uint8 type,bool needMat){
                        At<void*>(e,0)=vt.data(); const CVector pos=nearPos(30.f); At<CVector>(e,4)=pos; At<float>(e,0x10)=uni(-3.2f,3.2f);
                        if(needMat||rnd(4)){ randMat(Mat(matIdx),pos+CVector{uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-0.5f,0.5f)}); At<void*>(e,0x14)=Mat(matIdx); } else { std::memset(Mat(matIdx),0,MATSZ); At<void*>(e,0x14)=nullptr; }
                        At<void*>(e,0x18)=clump.data();
                        for(int q=0x1C;q<0x20;++q)e[q]=uint8(next()); e[0x36]=uint8((next()&0xF8)|type);
                        At<uint32>(e,0x40)=next(); for(int q=0;q<6;++q)At<float>(e,0x44+4*q)=rnd(4)==0 ? 0.f : uni(-1.f,1.f); };
                    auto setPed=[&](uint8* p,int matIdx,int k){
                        std::memset(p,0,PEDSZ); setEntityBase(p,matIdx,3,false);
                        for(int q=0x46C;q<0x47C;++q)p[q]=uint8(next());
                        At<void*>(p,0x47C)=Intel(k); At<void*>(p,0x480)=k<2 ? PData(k) : nullptr; p[0x484]=uint8(rnd(4));
                        At<int32>(p,0x4D4)=int32(rnd(120)); At<uint32>(p,0x530)=rnd(4)==0 ? (rnd(2) ? 54u+rnd(2) : 12u+rnd(12)) : rnd(64); At<uint32>(p,0x534)=rnd(8);
                        At<float>(p,0x540)=rnd(5)==0 ? 0.f : uni(-50.f,200.f); At<float>(p,0x544)=rnd(8)==0 ? 0.f : uni(50.f,200.f); At<float>(p,0x558)=uni(-3.2f,3.2f);
                        At<void*>(p,0x58C)=rnd(3) ? Veh(int(rnd(2))) : nullptr; At<uint32>(p,0x598)=rnd(3)==0 ? rnd(2) : rnd(20); At<void*>(p,0x59C)=statsObj.data();
                        for(int w=0;w<13;++w){ uint8* wp=p+0x5A0+w*0x1C; At<int32>(wp,0)=weaponTypes[rnd(27)]; At<uint32>(wp,4)=rnd(5); At<uint32>(wp,8)=rnd(100);
                            At<int32>(wp,0xC)=rnd(4)==0 ? int32(next()) : int32(rnd(600))-50; }
                        At<int8>(p,0x718)=int8(rnd(13)); p[0x71A]=uint8(next()); At<void*>(p,0x71C)=nullptr; At<float>(p,0x73C)=rnd(3)==0 ? 999999.f : uni(-3.2f,3.2f);
                        At<uint32>(p,0x748)=next(); At<void*>(p,0x79C)=rnd(2) ? Ped(3) : nullptr; At<uint32>(p,0x7A0)=next(); };
                    auto setVeh=[&](int k){ uint8* v=Veh(k); std::memset(v,0,VEHSZ); setEntityBase(v,8+k,2,false);
                        v[0x42D]=uint8(next()); At<void*>(v,0x460)=rnd(2) ? Ped(0) : Ped(2); At<float>(v,0x49C)=uni(-1.f,1.f); At<float>(v,0x4A0)=uni(-1.f,1.f);
                        At<float>(v,0x4C0)=rnd(8)==0 ? qnan : rnd(4)==0 ? 0.f : uni(-100.f,2000.f);
                        static const uint32 types[]{0,9,10,5,3,9,0,10}; At<uint32>(v,0x590)=types[rnd(8)]; At<uint32>(v,0x594)=types[rnd(8)]; };
                    // jugadores y peatones
                    for(int k=0;k<int(NPED);++k)setPed(Ped(k),k,k);
                    for(int k=0;k<int(NMEM);++k)setPed(Mem(k),12+k%4,3);
                    setVeh(0); setVeh(1);
                    std::memset(objBuf.data(),0,OBJSZ); setEntityBase(objBuf.data(),10,4,false); At<uint8>(objBuf.data(),0x13C)=uint8(rnd(8)); At<float>(objBuf.data(),0x154)=rnd(6)==0 ? -5.f : uni(0.f,2000.f);
                    std::memset(bldBuf.data(),0,bldBuf.size()); setEntityBase(bldBuf.data(),11,1,false);
                    for(int k=0;k<int(NPED);++k){ uint8* in=Intel(k); std::memset(in,0,INTSZ); At<void*>(in,0)=Ped(k); At<void*>(in,0x30)=Ped(k); }
                    for(int k=0;k<2;++k){ uint8* pd=PData(k); for(size_t q=0;q<PDSZ;++q)pd[q]=uint8(next());
                        At<void*>(pd,0)=wanted.data(); At<void*>(pd,4)=clothes.data(); At<void*>(pd,8)=nullptr; pd[0x20]=uint8(rnd(13)); At<void*>(pd,0x30)=rnd(2) ? Veh(0) : nullptr;
                        At<uint32>(pd,0x38)=rnd(8); At<float>(pd,0x14)=uni(-1.f,3.f); At<float>(pd,0x18)=rnd(8)==0 ? qnan : rnd(8)==0 ? -150.f : uni(-200.f,1200.f);
                        At<float>(pd,0x1C)=rnd(6)==0 ? 5.f : uni(-1.f,12.f); At<float>(pd,0x2C)=uni(0.f,20.f); At<float>(pd,0x44)=rnd(8)==0 ? 0.f : rnd(16)==0 ? qnan : uni(-50.f,1200.f);
                        At<float>(pd,0x64)=rnd(4)==0 ? 0.45f : uni(-0.1f,0.6f); At<CVector>(pd,0x74)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<float>(pd,0x90)=uni(-5.f,25.f); }
                    for(auto& b:wanted)b=uint8(next()); for(auto& b:statsObj)b=uint8(next()); for(auto& b:clothes)b=uint8(next());
                    // tareas falsas: tipo en +0x40, animacion de disparo en +0x2C
                    for(int k=0;k<6;++k){ uint8* t=Task(k); std::memset(t,0,TASKSZ); At<void*>(t,0)=taskVt.data(); At<int32>(t,0x40)=rnd(3)==0 ? 709 : int32(rnd(1500)); At<void*>(t,0x2C)=rnd(4) ? Assoc(1) : nullptr; }
                    jetpackTask=rnd(3)==0 ? Task(0) : nullptr; useGunTask=rnd(3) ? Task(1) : nullptr; swimTask=rnd(4)==0 ? Task(2) : nullptr;
                    // asociaciones y jerarquias
                    for(int k=0;k<4;++k){ uint8* h=Hier(k); At<uint32>(h,0)=rnd(3)==0 ? 0x1234u : next(); }
                    for(int k=0;k<4;++k){ uint8* a=Assoc(k); std::memset(a,0,ASZ); At<void*>(a,0x14)=Hier(k%2 ? 1 : int(rnd(4))); At<float>(a,0x18)=uni(0.f,1.f); At<float>(a,0x1C)=uni(-4.f,4.f);
                        At<float>(a,0x20)=uni(0.f,2.f); At<float>(a,0x28)=rnd(4)==0 ? 0.f : uni(0.f,0.2f); At<uint16>(a,0x2E)=uint16(next()); }
                    for(int k=0;k<2;++k){ uint8* s=Stat(k); std::memset(s,0,ASZ); At<void*>(s,0x10)=Hier(int(rnd(4))); }
                    clumpAssoc[0]=rnd(3) ? Assoc(0) : nullptr; clumpAssoc[1]=rnd(3) ? Assoc(1) : nullptr;
                    std::memset(clump.data(),0,clump.size()); clump[0]=2;
                    std::memset(colModel.data(),0,colModel.size()); At<CVector>(colModel.data(),0)=CVector{-0.5f,-0.5f,uni(-1.5f,-0.5f)}; At<CVector>(colModel.data(),0xC)=CVector{0.5f,0.5f,uni(0.2f,1.2f)};
                    // eventos falsos (dano y su clon)
                    for(int k=0;k<2;++k){ uint8* e=Ev(k); std::memset(e,0,EVSZ); At<void*>(e,0)=evVt.data(); for(int q=4;q<0x40;++q)e[q]=uint8(next()); }
                    At<void*>(Ev(0),0x14)=rnd(4) ? Ped(2) : nullptr;
                    std::memset(arena.data(),0xCD,arena.size());
                    std::memset(weaponArg.data(),0,weaponArg.size()); At<int32>(weaponArg.data(),0)=weaponTypes[rnd(27)];
                    losHit=rnd(2) ? Ped(2) : rnd(2) ? static_cast<void*>(Veh(0)) : nullptr;
                    // CWorld::Players, foco, tiempo, camara, mandos y grupo del jugador
                    std::memset(Plr(0),0,2*0x190);
                    for(int k=0;k<2;++k){ At<void*>(Plr(k),0)=rnd(5) ? Ped(k) : nullptr; At<void*>(Plr(k),0xB0)=rnd(5)==0 ? Veh(0) : nullptr; Plr(k)[0x14C]=uint8(rnd(5)==0); Plr(k)[0xDC]=uint8(next()); }
                    At<uint32>(Ped(0),0x598)=rnd(6)==0 ? rnd(20) : 0; At<uint32>(Ped(1),0x598)=1;
                    focus=uint8(rnd(2));
                    *reinterpret_cast<uint32*>(0xB7CB4C)=rnd(3)==0 ? (next()&~0x7Fu) : next();
                    *reinterpret_cast<float*>(0xB7CB5C)=rnd(4)==0 ? 1.f : uni(0.f,3.f);
                    *reinterpret_cast<float*>(0xB7CB64)=uni(0.f,2.f);
                    *reinterpret_cast<uint32*>(0xB7CB84)=1000000+rnd(200000);
                    *reinterpret_cast<uint8*>(0x96916E)=uint8(rnd(6)==0);
                    *reinterpret_cast<uint8*>(0x96A8B0)=uint8(rnd(5)==0 ? 1+rnd(3) : 0);
                    *reinterpret_cast<uint32*>(0xB72914)=rnd(5)==0 ? 1+rnd(3) : 0;
                    { auto* cam=reinterpret_cast<uint8*>(CAMERA); const uint8 ac=uint8(rnd(2)); cam[0x59]=ac; uint8* c=cam+0x174+ac*0x238;
                      At<uint16>(c,0xC)=uint16(rnd(3) ? 4 : rnd(60)); At<uint32>(c,0x28)=rnd(4)==0 ? 1 : 0;
                      const float a=uni(-3.2f,3.2f); At<float>(c,0x190)=std::cos(a); At<float>(c,0x194)=rnd(16)==0 ? 0.f : std::sin(a); At<float>(c,0x198)=uni(-0.5f,0.5f);
                      At<CVector>(cam,0x984)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; }
                    { auto* pad=reinterpret_cast<uint8*>(CPADS); At<uint16>(pad,0x10)=uint16(rnd(2) ? 255 : 0); At<uint16>(pad,0x12)=uint16(rnd(2) ? 255 : 0);
                      At<uint16>(pad,0x40)=uint16(rnd(2) ? 255 : 0); At<uint16>(pad,0x42)=uint16(rnd(2) ? 255 : 0); }
                    const uint32 grp=At<uint32>(PData(0),0x38); reinterpret_cast<uint8*>(GROUPS)[grp*GROUPSZ+4]=uint8(rnd(2));
                    for(int k=0;k<7;++k)reinterpret_cast<uint8*>(0xC0BC08)[k]=uint8(rnd(2));
                    // pool de peatones falso (solo FindPedToAttack lo instala)
                    std::memset(poolMem.data(),0,poolMem.size());
                    for(int k=0;k<int(NPOOL);++k){ uint8* p=PoolPed(k); setEntityBase(p,16+k%4,3,false); At<void*>(p,0x14)=nullptr; At<CVector>(p,4)=playerCoors+CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(-5.f,5.f)};
                        At<uint32>(p,0x530)=rnd(6)==0 ? 54+rnd(2) : rnd(50); At<uint32>(p,0x598)=rnd(4)==0 ? rnd(2) : rnd(20); poolMap[k]=uint8(rnd(4)==0 ? 0x80 : rnd(0x7F)); }
                    std::memset(poolObj.data(),0,poolObj.size()); At<void*>(poolObj.data(),0)=poolMem.data(); At<void*>(poolObj.data(),4)=poolMap.data(); At<int32>(poolObj.data(),8)=int32(NPOOL);
                    At<int32>(poolObj.data(),0xC)=-1; poolObj[0x10]=0;

                    uint8* const me=Ped(0);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    json ret=nullptr; bool switches=false; uintptr_t A=fn.address;
                    auto on=[&]{ for(auto* h:hooks)h->State(candidate);
                        if(fn.slot>=0){ const uintptr_t t=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]; switches=InExe(t)!=candidate; A=t; }
                        else switches=ReversingProbe::JumpsToDll(fn.address)==candidate; };
                    auto off=[&]{ for(auto* h:hooks)h->State(false); };
                    (void)vtTarget;
                    json extra=json::object();
                    auto pickTarget=[&]()->uint8*{ switch(rnd(6)){ case 0: return Veh(0); case 1: return objBuf.data(); case 2: return bldBuf.data(); case 3: return Ped(3); default: return Ped(2);} };

                    if(nm=="AnnoyPlayerPed"){
                        static const uint8 tv[]{0,45,46,51,52,53,54,55,56,100,127,128,200,255}; statsObj[0x25]=rnd(3) ? tv[rnd(14)] : uint8(next()); const bool b=rnd(2);
                        on(); reinterpret_cast<void(__thiscall*)(void*,bool)>(A)(me,b); off(); extra["temper"]=statsObj[0x25]; }
                    else if(nm=="Busted"||nm=="GetWantedLevel"){
                        on(); if(nm=="Busted")reinterpret_cast<void(__thiscall*)(void*)>(A)(me); else ret=reinterpret_cast<int32(__thiscall*)(void*)>(A)(me); off(); }
                    else if(nm=="SetWantedLevel"||nm=="SetWantedLevelNoDrop"||nm=="CheatWantedLevel"){
                        const int32 l=int32(rnd(8))-1; on(); reinterpret_cast<void(__thiscall*)(void*,int32)>(A)(me,l); off(); }
                    else if(nm=="CanIKReachThisTarget"){
                        if(!At<void*>(me,0x14)){ randMat(Mat(0),At<CVector>(me,4)); At<void*>(me,0x14)=Mat(0); }
                        const CVector pp=At<CVector>(Mat(0),0x30); CVector p=pp+CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(-10.f,10.f)};
                        if(rnd(8)==0)p=pp; if(rnd(16)==0)p.z=qnan; if(rnd(8)==0){ const float d=uni(0.f,5.f); p=pp+CVector{d,0.f,-d}; }
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,CVector,CWeapon*,bool)>(A)(me,p,reinterpret_cast<CWeapon*>(weaponArg.data()),rnd(2)!=0); off(); }
                    else if(nm=="CanPlayerStartMission"){
                        uint8* in=Intel(0); for(int q=0;q<5;++q)At<void*>(in,4+4*q)=rnd(3)==0 ? Task(3+q%3) : nullptr; for(int q=0;q<6;++q)At<void*>(in,0x18+4*q)=rnd(4)==0 ? Task(3) : nullptr;
                        if(rnd(2)){ At<void*>(in,4)=nullptr; At<void*>(in,0xC)=nullptr; At<void*>(in,0x18)=nullptr; } if(rnd(3)==0)At<int32>(Task(3+3%3),0x40)=709;
                        if(rnd(2))At<uint32>(me,0x530)=rnd(2) ? 50 : 1; if(rnd(2)){ me[0x46C]&=~0x03; me[0x46E]=0; }
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*)>(A)(me); off(); }
                    else if(nm=="Clear3rdPersonMouseTarget"){
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(me); off(); }
                    else if(nm=="ClearAdrenaline"){
                        At<uint32>(PData(0),0x3C)=rnd(3)==0 ? 0 : next();
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(me); off(); extra["scale"]=F(*reinterpret_cast<float*>(0xB7CB64)); }
                    else if(nm=="ClearWeaponTarget"||nm=="DoStuffToGoOnFire"||nm=="ProcessPlayerWeapon"||nm=="SetMoveAnim"||nm=="ReApplyMoveAnims"||nm=="PickWeaponAllowedFor2Player"||nm=="ResetSprintEnergy"||nm=="ResetPlayerBreath"){
                        At<void*>(me,0x71C)=rnd(3) ? pickTarget() : nullptr; if(rnd(3)==0)At<uint32>(me,0x530)=12;
                        if(nm=="ProcessPlayerWeapon"){ on(); reinterpret_cast<void(__thiscall*)(void*,CPad*)>(A)(me,CPad::GetPad(0)); off(); }
                        else { on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(me); off(); } }
                    else if(nm=="DeactivatePlayerPed"||nm=="ReactivatePlayerPed"||nm=="RemovePlayerPed"){
                        const int32 id=int32(rnd(2));
                        on(); reinterpret_cast<void(__cdecl*)(int32)>(A)(id); off(); }
                    else if(nm=="DisbandPlayerGroup"||nm=="MakePlayerGroupDisappear"||nm=="MakePlayerGroupReappear"){
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(me); off(); extra["temp"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0BC08),7); }
                    else if(nm=="DoesPlayerWantNewWeapon"){
                        const int32 t=weaponTypes[rnd(27)]; if(rnd(4)==0){ const auto s=CWeaponInfo::GetWeaponInfo(eWeaponType(t),eWeaponSkill::STD)->m_nSlot; At<int32>(me,0x5A0+s*0x1C)=rnd(2) ? t : 0; At<int8>(me,0x718)=int8(s); }
                        static const uint32 states[]{16,22,1,0,16,22,12,50}; At<uint32>(me,0x530)=states[rnd(8)];
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,int32,bool)>(A)(me,t,rnd(3)==0); off(); }
                    else if(nm=="DoesTargetHaveToBeBroken"||nm=="EvaluateTarget"){
                        uint8* tg=pickTarget(); if(rnd(4)==0)At<CVector>(tg,4)=At<CVector>(me,4)+CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-2.f,2.f)};
                        if(At<void*>(tg,0x14)&&rnd(2))At<CVector>(At<uint8*>(tg,0x14),0x30)=At<void*>(me,0x14) ? At<CVector>(At<uint8*>(me,0x14),0x30)+CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(-3.f,3.f)} : At<CVector>(tg,4);
                        if(!At<void*>(me,0x14)){ randMat(Mat(0),At<CVector>(me,4)); At<void*>(me,0x14)=Mat(0); }   // CanIKReachThisTarget lee la matriz
                        if(rnd(3)!=0)tg[0x1C]|=0x80;   // visible (bit 7)
                        CWeapon* w=reinterpret_cast<CWeapon*>(rnd(2) ? weaponArg.data() : me+0x5A0+At<int8>(me,0x718)*0x1C); if(rnd(4)==0)At<int32>(w,0)=41;
                        if(nm=="DoesTargetHaveToBeBroken"){ on(); ret=reinterpret_cast<bool(__thiscall*)(void*,CEntity*,CWeapon*)>(A)(me,reinterpret_cast<CEntity*>(tg),w); off(); }
                        else {
                            CEntity* outT=rnd(2) ? reinterpret_cast<CEntity*>(Ped(3)) : nullptr; float outP=rnd(4)==0 ? -1.f : rnd(16)==0 ? qnan : uni(0.f,0.6f);
                            const float maxD=rnd(16)==0 ? qnan : uni(1.f,80.f); const float comp=uni(-7.f,7.f);
                            on(); reinterpret_cast<void(__thiscall*)(void*,CEntity*,CEntity**,float*,float,float,bool)>(A)(me,reinterpret_cast<CEntity*>(tg),&outT,&outP,maxD,comp,rnd(2)!=0); off();
                            extra["out"]=json::array({Name(U(outT)),F(outP)}); } }
                    else if(nm=="FindPedToAttack"){
                        pedPoolPtr=poolObj.data();
                        on(); ret=Name(U(reinterpret_cast<void*(__thiscall*)(void*)>(A)(me))); off(); }   // el puntero del pool se restaura con el resto de globales
                    else if(nm=="FindTargetPriority"){
                        uint8* tg=pickTarget(); if(rnd(3)==0)tg[0x36]=uint8((tg[0x36]&0xF8)|rnd(8));
                        on(); ret=F(reinterpret_cast<float(__thiscall*)(void*,CEntity*)>(A)(me,reinterpret_cast<CEntity*>(tg))); off(); }
                    else if(nm=="ForceGroupToAlwaysFollow"||nm=="ForceGroupToNeverFollow"){
                        const bool b=rnd(3)!=0; on(); reinterpret_cast<void(__thiscall*)(void*,bool)>(A)(me,b); off(); }
                    else if(nm=="GetButtonSprintResults"){
                        float sp=rnd(4)==0 ? 5.f : rnd(8)==0 ? 0.f : rnd(16)==0 ? qnan : rnd(16)==0 ? -inf : uni(-3.f,30.f); At<float>(PData(0),0x1C)=sp;
                        on(); ret=F(reinterpret_cast<float(__thiscall*)(void*,int32)>(A)(me,int32(rnd(4)))); off(); }
                    else if(nm=="GetPadFromPlayer"){
                        At<uint32>(me,0x598)=rnd(3) ? rnd(2) : rnd(20); on(); ret=Name(U(reinterpret_cast<void*(__thiscall*)(void*)>(A)(me))); off(); }
                    else if(nm=="GetPlayerInfoForThisPlayerPed"){
                        if(rnd(3)==0)At<void*>(Plr(rnd(2)),0)=me;
                        on(); ret=Name(U(reinterpret_cast<void*(__thiscall*)(void*)>(A)(me))); off(); }
                    else if(nm=="GetWeaponRadiusOnScreen"){
                        if(rnd(2))At<int32>(me,0x5A0+At<int8>(me,0x718)*0x1C)=rnd(2) ? 25+int32(rnd(3)) : int32(rnd(10));
                        on(); ret=F(reinterpret_cast<float(__thiscall*)(void*)>(A)(me)); off(); }
                    else if(nm=="HandlePlayerBreath"){
                        const bool b=rnd(3)!=0; const float m=rnd(8)==0 ? 0.f : uni(0.f,4.f);
                        on(); reinterpret_cast<void(__thiscall*)(void*,bool,float)>(A)(me,b,m); off(); }
                    else if(nm=="HandleSprintEnergy"){
                        const bool b=rnd(2); const float m=rnd(6)==0 ? 0.f : rnd(16)==0 ? qnan : uni(0.f,5.f);
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,bool,float)>(A)(me,b,m); off(); }
                    else if(nm=="IsHidden"){
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*)>(A)(me); off(); }
                    else if(nm=="LOSBlockedBetweenPeds"){
                        uint8* a=rnd(3) ? Ped(int(rnd(4))) : pickTarget(); uint8* b=rnd(3) ? Ped(int(rnd(4))) : pickTarget(); if(rnd(3)==0)losHit=b;
                        on(); ret=reinterpret_cast<bool(__cdecl*)(void*,void*)>(A)(a,b); off(); }
                    else if(nm=="Load"||nm=="Save"){
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*)>(A)(me); off(); }
                    else if(nm=="MakeChangesForNewWeapon-"||nm=="MakeChangesForNewWeapon-BySlot"){
                        if(rnd(3)==0)At<uint32>(me,0x530)=12; for(int w=0;w<13;++w)if(rnd(4)==0)At<int32>(me,0x5A0+w*0x1C+0xC)=rnd(2) ? -5 : 1000;
                        if(nm=="MakeChangesForNewWeapon-"){ const int32 t=weaponTypes[rnd(27)]; on(); reinterpret_cast<void(__thiscall*)(void*,int32)>(A)(me,t); off(); }
                        else { const int32 s=rnd(4)==0 ? -1 : int32(rnd(13)); on(); reinterpret_cast<void(__thiscall*)(void*,int32)>(A)(me,s); off(); } }
                    else if(nm=="MakeGroupRespondToPlayerTakingDamage"){
                        on(); reinterpret_cast<void(__thiscall*)(void*,void*)>(A)(me,Ev(0)); off(); }
                    else if(nm=="PedCanBeTargettedVehicleWise"){
                        uint8* p=Ped(int(rnd(4))); if(rnd(4)==0)At<void*>(p,0x58C)=nullptr;
                        on(); ret=reinterpret_cast<bool(__cdecl*)(void*)>(A)(p); off(); }
                    else if(nm=="PlayerHasJustAttackedSomeone"){
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*)>(A)(me); off(); }
                    else if(nm=="ProcessControl"){
                        uint8* pd=PData(0);
                        pd[0x21]=uint8(rnd(3)==0 ? 0 : rnd(4)); pd[0x40]=uint8(rnd(3)==0 ? 0 : rnd(4)==0 ? 1 : next()); pd[0x41]=uint8(rnd(2)); pd[0x85]=uint8(rnd(2)); pd[0x34]=uint8(next());
                        At<uint32>(pd,0x24)=rnd(3)==0 ? 0 : 1000000+rnd(200000);
                        if(rnd(3)==0)At<int32>(me,0x5A0+At<int8>(me,0x718)*0x1C)=rnd(2) ? 38 : 9;
                        if(rnd(2))At<void*>(me,0x71C)=pickTarget();
                        if(rnd(3)==0)At<float>(me,0x544)=0.f;
                        if(rnd(4)==0)me[0x46D]&=~0x01;
                        At<uint32>(me,0x40)=rnd(2) ? (At<uint32>(me,0x40)|0x100) : (At<uint32>(me,0x40)&~0x100u);
                        if(rnd(3)==0){ At<float>(me,0x44)=uni(-0.05f,0.05f); At<float>(me,0x48)=uni(-0.05f,0.05f); }
                        me[0x1C]&=~0x40;
                        on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(me); off(); }
                    else if(nm=="SetPlayerMoveBlendRatio"){
                        CVector p=At<CVector>(me,4)+CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f)}; if(rnd(16)==0)p.x=qnan;
                        At<uint32>(me,0x534)=rnd(8);
                        on(); reinterpret_cast<void(__thiscall*)(void*,CVector*)>(A)(me,rnd(3)==0 ? nullptr : &p); off(); }
                    else if(nm=="TellGroupToStartFollowingPlayer"){
                        const bool a=rnd(2),b=rnd(2),c=rnd(2);
                        on(); reinterpret_cast<void(__thiscall*)(void*,bool,bool,bool)>(A)(me,a,b,c); off(); extra["new"]=newCount; }
                    else if(nm=="UpdateCameraWeaponModes"){
                        if(rnd(2))At<int32>(me,0x5A0+At<int8>(me,0x718)*0x1C)=31+int32(rnd(14));
                        on(); reinterpret_cast<void(__thiscall*)(void*,CPad*)>(A)(me,CPad::GetPad(0)); off(); }

                    // ---- volcado ----
                    json saves=json::array(); for(int k=0;k<nSave;++k)saves.push_back(json::array({saveSizes[k],ReversingProbe::Bytes(saveBytes[k].data(),saveBytes[k].size())}));
                    json r={{"ret",ret},{"switches",switches},{"extra",extra},{"trace",TraceJson()},{"overflow",trOverflow},{"saves",saves},
                            {"me",ReversingProbe::Bytes(me,0x7A4)},{"pdata",ReversingProbe::Bytes(PData(0),PDSZ)},
                            {"peds",json::array({Hash(Ped(1),PEDSZ),Hash(Ped(2),PEDSZ),Hash(Ped(3),PEDSZ),Hash(members.data(),members.size())})},
                            {"objs",Hash(vehs.data(),vehs.size())+Hash(objBuf.data(),OBJSZ)+Hash(bldBuf.data(),bldBuf.size())+Hash(mats.data(),mats.size())+Hash(intels.data(),intels.size())},
                            {"misc",Hash(pdatas.data()+PDSZ,PDSZ)+Hash(wanted.data(),WSZ)+Hash(statsObj.data(),statsObj.size())+Hash(clothes.data(),clothes.size())+Hash(tasks.data(),tasks.size())},
                            {"anims",Hash(assocs.data(),assocs.size())+Hash(statics.data(),statics.size())+Hash(evs.data(),evs.size())+Hash(poolMem.data(),poolMem.size())+Hash(weaponArg.data(),weaponArg.size())},
                            {"players",Hash(Plr(0),2*0x190)},{"focus",focus},
                            {"globals",json::array({*reinterpret_cast<uint32*>(0xB7CB4C),F(*reinterpret_cast<float*>(0xB7CB5C)),F(*reinterpret_cast<float*>(0xB7CB64)),*reinterpret_cast<uint32*>(0xB7CB84),
                                Hash(reinterpret_cast<void*>(GROUPS),8*GROUPSZ),Hash(reinterpret_cast<void*>(CAMERA),0xD78),Hash(reinterpret_cast<void*>(CPADS),2*0x134),ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0BC08),7)})}};
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
