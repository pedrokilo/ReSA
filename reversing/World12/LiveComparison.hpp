#pragma once
// Banco dentro del motor: semaforos, pickups y explosiones (lista de 12; 36 funciones).
// CTrafficLights, CPickup y CExplosion. Original (hooks del grupo desactivados, codigo del EXE) frente a candidato con
// fixtures deterministas sobre objetos falsos (entidad de semaforo, vehiculos, nodos de ruta, model info con efectos 2D,
// objeto del pickup con vtable propia, jugador con armas/datos/busqueda, pad, pool de vehiculos, entidades victima con
// RwObject/frame) y sobre arrays reales (aPickUps, aExplosions, Players, Pads, camara, ciclo horario, punteros de
// ThePaths, model infos y del pool de vehiculos) guardados, rellenados y restaurados en cada ejecucion. Nada pinta, suena
// ni crea objetos: se simulan y trazan rand, coronas, luces, sombras, textos brillantes, efectos FX, audio, HUD, texto,
// pads, peds (municion, armas, voz, misiones), agua, suelo, lineas verticales, fuegos, estadisticas, referencias,
// CWorld::Add/Remove, CObject::Create, CRadar, FindPlayerPed/Vehicle/Coors y GetATanOfXY.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TrafficLights.h"
#include "Pickup.h"
#include "Pickups.h"
#include "Explosion.h"
#include "Coronas.h"
#include "BrightLights.h"
#include "PointLights.h"
#include "Shadows.h"
#include "ShinyTexts.h"
#include "GameLogic.h"
#include "Radar.h"
#include "Hud.h"
#include "Messages.h"
#include "Text/Text.h"
#include "Darkel.h"
#include "Localisation.h"
#include "WaterLevel.h"
#include "Collision/Collision.h"
#include "Pools/Pools.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Audio/Entities/AEExplosionAudioEntity.h"
#include "Audio/AudioEngine.h"
#include "InterestingEvents.h"
#include "FireManager.h"
#include "CreepingFire.h"
#include "Stats.h"
#include "Crime.h"
#include "Camera.h"
#include "Pad.h"
#include "PlayerPed.h"
#include "PedIntelligence.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Streaming.h"
#include "Object.h"
#include "Scripts/TheScripts.h"
#include "../ProbeRuntime.hpp"

namespace World12Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline const float QNAN=std::bit_cast<float>(0x7FC00000u);

// --- direcciones del EXE ---
constexpr uintptr_t CAMERA=0xB6F028, CAMERA_SIZE=0xD78, TIME=0xB7CB84, TSTEP=0xB7CB5C, FRAME=0xB7CB4C, MIPTRS=0xA9B0C8;
constexpr uintptr_t EXPL=0xC88950, EXPL_SIZE=16*0x7C, PICKUPS=0x9788C0, PICKUPS_SIZE=620*0x20, PADS=0xB73458, PLAYERS=0xB7CD98;
constexpr uintptr_t STREAMINFO=0x8E4CC0, PATHNODES=0x96F854, NAVINODES=0x96F974, PEDLIGHTS=0xA9ADB0, VEHPOOL=0xB74494;

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<uint8,0x200> tlEnt{};
alignas(16) inline std::array<uint8,0x48> tlMat{}, camMat{}, vehMat{}, objMat{}, obj2Mat{}, pedMat{}, ped2Mat{};
alignas(16) inline std::array<uint8,0x80> fakeMi{}, fakeVehMi{};
alignas(16) inline std::array<std::array<uint8,0x40>,8> effects{};
alignas(16) inline std::array<uint8,8*64*14> links{};
alignas(16) inline std::array<uint8,0x40> nodeDummy{};
alignas(16) inline std::array<std::array<uint8,0xA18>,4> vehs{};
alignas(16) inline std::array<uint8,0x20> vehPool{};
alignas(16) inline std::array<uint8,8> vehFlags{};
alignas(16) inline std::array<uint8,0x180> obj{}, obj2{};
alignas(16) inline std::array<uint8,0x7A0> ped{}, ped2{};
alignas(16) inline std::array<uint8,0x100> intel{}, playerData{}, wanted{}, fakeTask{}, vehStruct{};
alignas(16) inline std::array<uint8,0x134> fakePad{};
alignas(16) inline std::array<std::array<uint8,0x200>,4> ents{};
alignas(16) inline std::array<std::array<uint8,0x48>,4> entMats{};
alignas(16) inline std::array<std::array<uint8,0x10>,4> rwObjs{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> frames{};
alignas(16) inline std::array<uint8,0x40> fakeFx{}, outBuf{}, gxt{};
inline std::array<void*,16> objVt{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); }
    if(u>=EXPL&&u<EXPL+EXPL_SIZE) return json::array({"expl",(u-EXPL)/0x7C,(u-EXPL)%0x7C});
    if(u>=PICKUPS&&u<PICKUPS+PICKUPS_SIZE) return json::array({"pickup",(u-PICKUPS)/0x20,(u-PICKUPS)%0x20});
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); // punteros a la pila: los marcos difieren entre EXE y DLL
      if(u>=uintptr_t(tib->StackLimit)&&u<uintptr_t(tib->StackBase)) return "stack"; }
    if(u>=0x400000&&u<0x1000000) return json::array({"exe",u});
    return "ext"; }
inline json Val(const void* p){ const auto u=uintptr_t(p); return u<0x10000 ? json(u) : Id(p); }
inline json Vec(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }

// --- generador de las respuestas de los stubs (se reinicia igual en cada ejecucion) ---
inline uint32 stub{};
inline uint32 Nx(){ stub^=stub<<13; stub^=stub>>17; stub^=stub<<5; return stub; }
inline float Un(float a,float b){ return a+(b-a)*float(Nx()%1000000)/1000000.f; }
inline std::array<uint32,24> pct{};
inline bool Roll(int k){ return Nx()%100<pct[k]; }

// --- rand ---
inline uint32 rngCalls{};
inline int __cdecl FakeRand(){ trace.push_back({"rand"}); return int((stub*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }

// --- semaforos ---
inline uint32 riotsMask{}, riotsCalls{};
inline bool __cdecl FakeRiots(){ const bool r=(riotsMask>>(riotsCalls++%32))&1; trace.push_back({"riots",r}); return r; }
inline std::array<int32,2> appearance{}; inline uint32 appCalls{};
inline int32 __fastcall FakeAppearance(const void* self,void*){ const int32 r=appearance[appCalls++%2]; trace.push_back({"appearance",Id(self),r}); return r; }
inline float atanResult{};
inline float __cdecl FakeATan(float x,float y){ trace.push_back({"atan",F(x),F(y)}); return atanResult; }
inline void* __fastcall FakeGet2dEffect(const void* self,void*,int32 i){ trace.push_back({"get_2dfx",Id(self),i}); return effects[uint32(i)%8].data(); }
inline void __cdecl FakeCorona(uint32 id,CEntity* att,uint8 r,uint8 g,uint8 b,uint8 in,const CVector& p,float size,float range,uint8 type,uint8 flare,uint8 refl,uint8 los,uint8 trail,
                               float ang,uint8 neon,float pull,uint8 fb,float fade,uint8 below,uint8 white){
    trace.push_back({"corona",Val(reinterpret_cast<void*>(id)),Id(att),r,g,b,in,Vec(p),F(size),F(range),type,flare,refl,los,trail,F(ang),neon,F(pull),fb,F(fade),below,white}); }
inline void __cdecl FakeBright(CVector p,CVector t,CVector r,CVector at,uint8 c,uint8 a5,uint8 a6,uint8 a7){ trace.push_back({"bright",Vec(p),Vec(t),Vec(r),Vec(at),c,a5,a6,a7}); }
inline void __cdecl FakeAddLight(uint8 type,CVector p,CVector d,float rad,float r,float g,float b,uint8 fog,uint8 sh,CEntity* e){ trace.push_back({"light",type,Vec(p),Vec(d),F(rad),F(r),F(g),F(b),fog,sh,Id(e)}); }
inline bool __cdecl FakeStaticShadow(uint32 id,uint8 type,RwTexture* tex,const CVector* p,float fx,float fy,float sx,float sy,int16 in,uint8 r,uint8 g,uint8 b,float z,float sc,float dd,uint8 tmp,float up){
    trace.push_back({"static_shadow",Val(reinterpret_cast<void*>(id)),type,Val(tex),Vec(*p),F(fx),F(fy),F(sx),F(sy),in,r,g,b,F(z),F(sc),F(dd),tmp,F(up)}); return true; }
inline void __cdecl FakeShiny(CVector a,CVector b,CVector c,CVector d,float u1,float v1,float u2,float v2,float u3,float v3,float u4,float v4,uint8 al,uint8 r,uint8 g,uint8 bl,float md){
    trace.push_back({"shiny",Vec(a),Vec(b),Vec(c),Vec(d),F(u1),F(v1),F(u2),F(v2),F(u3),F(v3),F(u4),F(v4),al,r,g,bl,F(md)}); }

// --- pickups ---
inline void __cdecl FakeClearBlip(uint8 type,int32 handle){ trace.push_back({"clear_blip",type,handle}); }
inline void __cdecl FakeWorldRemove(CEntity* e){ trace.push_back({"world_remove",Id(e)}); }
inline void __cdecl FakeWorldAdd(CEntity* e){ trace.push_back({"world_add",Id(e)}); }
inline void __fastcall FakeObjDtor(void* self,void*,int32 flags){ trace.push_back({"delete",Id(self),flags}); }
inline void __fastcall FakeVTrap(void* self,void*){ trace.push_back({"vtrap",Id(self)}); }
inline void* createResult{};
inline CObject* __cdecl FakeCreate(int32 model,uint8 b){ trace.push_back({"create",model,b}); return static_cast<CObject*>(createResult); }
inline void** makeSureRef{}; inline void* makeSureFill{};
inline void __cdecl FakeMakeSure(int32 slot){ trace.push_back({"make_sure_slot",slot}); if(makeSureRef&&makeSureFill)*makeSureRef=makeSureFill; }
inline void __fastcall FakeSetOrient(void* self,void*,float x,float y,float z){ trace.push_back({"set_orientation",Id(self),F(x),F(y),F(z)}); }
inline void __fastcall FakeRegRef(void* self,void*,CEntity** slot){ trace.push_back({"register_reference",Id(self),Id(slot)}); }
inline void __fastcall FakeCleanRef(void* self,void*,CEntity** slot){ trace.push_back({"cleanup_reference",Id(self),Id(slot)}); }
inline int32 __cdecl FakeWeaponForModel(int32 model){ const int32 w=Nx()%5==0 ? 0 : int32(Nx()%47); trace.push_back({"weapon_for_model",model,w}); return w; }
inline bool __cdecl FakeCanBe2P(int32 w){ const bool r=Roll(0); trace.push_back({"can_be_2p",w,r}); return r; }
template<int K> inline bool __cdecl FakeFlag(){ const bool r=Roll(K); trace.push_back({"flag",K,r}); return r; }
inline std::array<void*,3> findPed{}; inline void* findVeh{};
inline CPlayerPed* __cdecl FakeFindPed(int32 id){ trace.push_back({"find_ped",id}); return static_cast<CPlayerPed*>(findPed[id<0 ? 0 : (id>1 ? 2 : id+1)]); }
inline CVehicle* __cdecl FakeFindVeh(int32 id,uint8 remote){ trace.push_back({"find_vehicle",id,remote}); return static_cast<CVehicle*>(findVeh); }
inline CVector playerCoors{};
inline CVector* __cdecl FakeCoors(CVector* out,int32 id){ *out=playerCoors; out->x+=float(Nx()%4)*0.25f; out->y-=float(Nx()%4)*0.25f; trace.push_back({"find_coors",id,Vec(*out)}); return out; }
inline float __fastcall FakeHeading(const void* self,void*){ const float h=Un(-3.f,3.f); trace.push_back({"heading",Id(self),F(h)}); return h; }
inline void* padResult{};
inline void* __fastcall FakePadFromPlayer(const void* self,void*){ trace.push_back({"pad_from_player",Id(self)}); return padResult; }
inline bool __fastcall FakeCanStart(void* self,void*){ const bool r=Roll(1); trace.push_back({"can_start_mission",Id(self),r}); return r; }
inline bool __fastcall FakeIsAlive(const void* self,void*){ const bool r=Roll(2); trace.push_back({"is_alive",Id(self),r}); return r; }
inline void __fastcall FakeGrantAmmo(void* self,void*,int32 w,uint32 a){ trace.push_back({"grant_ammo",Id(self),w,a}); }
inline void __fastcall FakeGiveDelayed(void* self,void*,int32 w,uint32 a){ trace.push_back({"give_delayed_weapon",Id(self),w,a}); }
inline bool __fastcall FakeWantWeapon(void* self,void*,int32 w,uint8 b){ const bool r=Roll(3); trace.push_back({"want_weapon",Id(self),w,b,r}); return r; }
inline int32 __fastcall FakeWeaponSlot(void* self,void*,int32 w){ const int32 s=int32(uint32(w)*7u%13u); trace.push_back({"weapon_slot",Id(self),w,s}); return s; }
inline int16 __fastcall FakeSay(void* self,void*,int16 ctx,uint32 d,float p,uint8 a,uint8 b,uint8 c){ trace.push_back({"say",Id(self),ctx,d,F(p),a,b,c}); return 0; }
inline void* jetpackResult{};
inline void* __fastcall FakeJetPack(void* self,void*){ trace.push_back({"task_jetpack",Id(self)}); return jetpackResult; }
inline void __fastcall FakeReload(void* self,void*,CPed* owner){ trace.push_back({"reload",Id(self),Id(owner)}); }
inline bool __fastcall FakeGetTarget(const void* self,void*){ const bool r=Roll(4); trace.push_back({"get_target",Id(self),r}); return r; }
inline void __fastcall FakeStartShake(void* self,void*,int16 t,uint8 f,uint32 a2){ trace.push_back({"start_shake",Id(self),t,f,a2}); }
inline uint32 __fastcall FakeTouched(const void* self,void*){ const uint32 r=Nx()%1000; trace.push_back({"touched_delta",Id(self),r}); return r; }
inline void __fastcall FakeShakeDistance(void* self,void*,int16 t,uint8 f,float x,float y,float z){ trace.push_back({"shake_distance",Id(self),t,f,F(x),F(y),F(z)}); }
inline const char* __fastcall FakeTextGet(void* self,void*,const char* key){ trace.push_back({"text_get",std::string(key,strnlen(key,8))}); return reinterpret_cast<const char*>(gxt.data()); }
inline void __cdecl FakeSetHelp(const void* text,uint8 q,uint8 p,uint8 b){ trace.push_back({"set_help",Id(text),q,p,b}); }
inline bool __cdecl FakeHelpShown(){ const bool r=Roll(5); trace.push_back({"help_displayed",r}); return r; }
inline void __cdecl FakeInsertNumber(const void* src,int32 n1,int32 n2,int32 n3,int32 n4,int32 n5,int32 n6,void* dst){ trace.push_back({"insert_number",Id(src),n1,n2,n3,n4,n5,n6,Id(dst)}); }
inline void __cdecl FakeModifyLabel(char* label){ trace.push_back({"modify_label",std::string(label,strnlen(label,8))}); }
inline void __fastcall FakeFrontend(void* self,void*,int32 ev,float vol,float speed){ trace.push_back({"frontend_audio",ev,F(vol),F(speed)}); }
inline bool __cdecl FakeGoodies(uint16 model,int32 id){ const bool r=Roll(6); trace.push_back({"goodies",model,id,r}); return r; }
inline bool __cdecl FakeCanPickUp(int32 w){ const bool r=Roll(7); trace.push_back({"can_pick_up",w,r}); return r; }
inline bool __fastcall FakeTouching(void* self,void*,float x,float y,float z,float r){ const bool t=Roll(8); trace.push_back({"sphere_touching",Id(self),F(x),F(y),F(z),F(r),t}); return t; }
inline bool __cdecl FakeWaterLevel(float x,float y,float z,float& level,uint8 touching,CVector* n){ const bool ok=Roll(9); if(ok)level=z-5.f+Un(-2.f,2.f); trace.push_back({"water_level",F(x),F(y),F(z),touching,Id(n),ok}); return ok; }
inline bool __cdecl FakeLineSphere(const void* line,const void* sphere){ const bool r=Roll(10);
    trace.push_back({"line_sphere",Vec(*static_cast<const CVector*>(line)),Vec(*reinterpret_cast<const CVector*>(static_cast<const uint8*>(line)+0x10)),Vec(*static_cast<const CVector*>(sphere)),F(*reinterpret_cast<const float*>(static_cast<const uint8*>(sphere)+0xC)),r}); return r; }

// --- explosiones ---
inline FxSystem_c* __fastcall FakeCreateFx(void* self,void*,const char* name,const CVector* p,RwMatrix* m,uint8 ign){ const bool ok=Roll(11);
    trace.push_back({"create_fx",std::string(name),Vec(*p),Id(m),ign,ok}); return ok ? reinterpret_cast<FxSystem_c*>(fakeFx.data()) : nullptr; }
inline void __fastcall FakePlayAndKill(void* self,void*){ trace.push_back({"play_and_kill",Id(self)}); }
inline void __fastcall FakeAudioEvent(void* self,void*,int32 ev,CVector* p,float vol){ trace.push_back({"audio_event",ev,Id(p),Vec(*p),F(vol)}); }
inline float __cdecl FakeGroundZ(float x,float y,float z,bool* hit,CEntity** e){ const bool h=Roll(12); const float r=z-Un(0.f,15.f); if(hit)*hit=h; if(e)*e=nullptr; trace.push_back({"ground_z",F(x),F(y),F(z),Id(hit),Id(e),h,F(r)}); return r; }
inline bool __cdecl FakeWaterNoWaves(float x,float y,float z,float* lvl,float* u1,float* u2){ const bool ok=Roll(13); if(ok&&lvl)*lvl=z+Un(-3.f,3.f); trace.push_back({"water_no_waves",F(x),F(y),F(z),Id(u1),Id(u2),ok}); return ok; }
inline void __cdecl FakeReportCrime(int32 type,CEntity* e,CPed* p){ trace.push_back({"report_crime",type,Id(e),Id(p)}); }
inline void __fastcall FakeInteresting(void* self,void*,int32 type,CEntity* e){ trace.push_back({"interesting",type,Id(e)}); }
inline void* __fastcall FakeStartFire(void* self,void*,CVector p,float size,uint8 u1,CEntity* c,uint32 t,int8 g,uint8 u2){ trace.push_back({"start_fire",Vec(p),F(size),u1,Id(c),t,g,u2}); return nullptr; }
inline void __cdecl FakeIncStat(uint16 s,float v){ trace.push_back({"inc_stat",s,F(v)}); }
inline void __cdecl FakePermShadow(uint8 type,RwTexture* tex,CVector* p,float tx,float ty,float rx,float ry,int16 in,uint8 r,uint8 g,uint8 b,float dd,uint32 time,float up){
    trace.push_back({"perm_shadow",type,Val(tex),Vec(*p),F(tx),F(ty),F(rx),F(ry),in,r,g,b,F(dd),time,F(up)}); }
inline void __cdecl FakeTrigger(const CVector& p,float rad,float vis,CEntity* v,CEntity* c,uint8 bomb,float dmg){ trace.push_back({"trigger_explosion",Vec(p),F(rad),F(vis),Id(v),Id(c),bomb,F(dmg)}); }
inline void __fastcall FakeCamShake(void* self,void*,float s,float x,float y,float z){ trace.push_back({"cam_shake",Id(self),F(s),F(x),F(y),F(z)}); }
inline bool __cdecl FakeTryFire(float x,float y,float z,uint8 g,uint8 a5,uint8 sc,float d){ trace.push_back({"try_fire",F(x),F(y),F(z),g,a5,sc,F(d)}); return false; }
inline void __cdecl FakePedsOnFire(float x,float y,float z,float r,CEntity* c){ trace.push_back({"peds_on_fire",F(x),F(y),F(z),F(r),Id(c)}); }
inline void __cdecl FakeWorldOnFire(float x,float y,float z,float r,CEntity* c){ trace.push_back({"world_on_fire",F(x),F(y),F(z),F(r),Id(c)}); }
inline void __cdecl FakeCarsOnFire(float x,float y,float z,float r,CEntity* c){ trace.push_back({"cars_on_fire",F(x),F(y),F(z),F(r),Id(c)}); }
inline bool __cdecl FakeVLine(const CVector& o,float d,CColPoint& cp,CEntity*& e,uint8 b1,uint8 b2,uint8 b3,uint8 b4,uint8 b5,uint8 b6,CStoredCollPoly* poly){
    const bool hit=Roll(14); trace.push_back({"vline",Vec(o),F(d),b1,b2,b3,b4,b5,b6,Id(poly),hit}); if(hit)cp.m_vecPoint.z=o.z-Un(0.f,30.f); return hit; }

// Llama a IsVisible (thiscall sin argumentos) con un centinela en edx y devuelve al y edx
inline uint32 CallKeepEdx0(uintptr_t fn,void* self,uint8& ret){
    uint32 edxOut; uint8 retByte;
    __asm {
        mov  ecx, self
        mov  edx, 5A17C0DEh
        call fn
        mov  edxOut, edx
        mov  retByte, al
    }
    ret=retByte;
    return edxOut;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,36> fns{{
    {"CTrafficLights","DisplayActualLight",0x49DAB0,0,-1,512},{"CTrafficLights","FindOrientationForTrafficLightType",0x49D520,0,-1,256},
    {"CTrafficLights","FindTrafficLightType",0x49D580,0,-1,256},{"CTrafficLights","FindTrafficLightTypeFromOrientation",0x49D4D0,0,-1,512},
    {"CTrafficLights","IsMITrafficLight",0x49D5A0,0,-1,256},{"CTrafficLights","LightForCars1",0x49D2D0,0,-1,256},
    {"CTrafficLights","LightForCars1_Visual",0x49D350,0,-1,256},{"CTrafficLights","LightForCars2",0x49D310,0,-1,256},
    {"CTrafficLights","LightForCars2_Visual",0x49D3A0,0,-1,256},{"CTrafficLights","LightForPeds",0x49D400,0,-1,256},
    {"CTrafficLights","ShouldCarStopForBridge",0x49D420,0,-1,256},{"CTrafficLights","ShouldCarStopForLight",0x49D610,0,-1,1024},
    {"CPickup","ExtractAmmoFromPickup",0x454BE0,0,-1,256},{"CPickup","FindStringForTextIndex",0x455540,0,-1,64},
    {"CPickup","FindTextIndexForString",0x455500,0,-1,128},{"CPickup","GetPosn",0x4549A0,0,-1,256},
    {"CPickup","GetRidOfObjects",0x454CF0,0,-1,64},{"CPickup","GiveUsAPickUpObject",0x4567E0,0,-1,512},
    {"CPickup","IsVisible",0x454C70,0,-1,256},{"CPickup","PickUpShouldBeInvisible",0x454D20,0,-1,512},
    {"CPickup","ProcessGunShot",0x4588B0,0,-1,256},{"CPickup","Remove",0x4556C0,0,-1,128},
    {"CPickup","SetPosn",0x454960,0,-1,256},{"CPickup","Update",0x457410,0,-1,1024},
    {"CExplosion","AddExplosion",0x736A50,0,-1,1024},{"CExplosion","ClearAllExplosions",0x736840,0,-1,64},
    {"CExplosion","DoesExplosionMakeSound",0x736920,0,-1,64},{"CExplosion","GetExplosionActiveCounter",0x736900,0,-1,64},
    {"CExplosion","GetExplosionPosition",0x736940,0,-1,64},{"CExplosion","GetExplosionType",0x736930,0,-1,64},
    {"CExplosion","Initialise",0x736A40,0,-1,64},{"CExplosion","RemoveAllExplosionsInArea",0x7369E0,0,-1,256},
    {"CExplosion","ResetExplosionActiveCounter",0x736910,0,-1,64},{"CExplosion","Shutdown",0x7368F0,0,-1,64},
    {"CExplosion","TestForExplosionInArea",0x736950,0,-1,512},{"CExplosion","Update",0x737620,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"world12-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"world12-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("world12");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"world12-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,36> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,36> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    using std::bit_cast;
    if(regions.empty()){ Reg(tlEnt.data(),tlEnt.size()); for(auto* b:{&tlMat,&camMat,&vehMat,&objMat,&obj2Mat,&pedMat,&ped2Mat})Reg(b->data(),b->size());
        Reg(fakeMi.data(),fakeMi.size()); Reg(fakeVehMi.data(),fakeVehMi.size()); for(auto& b:effects)Reg(b.data(),b.size()); Reg(links.data(),links.size()); Reg(nodeDummy.data(),nodeDummy.size());
        for(auto& b:vehs)Reg(b.data(),b.size()); Reg(vehPool.data(),vehPool.size()); Reg(vehFlags.data(),vehFlags.size()); Reg(obj.data(),obj.size()); Reg(obj2.data(),obj2.size());
        Reg(ped.data(),ped.size()); Reg(ped2.data(),ped2.size()); for(auto* b:{&intel,&playerData,&wanted,&fakeTask,&vehStruct})Reg(b->data(),b->size()); Reg(fakePad.data(),fakePad.size());
        for(auto& b:ents)Reg(b.data(),b.size()); for(auto& b:entMats)Reg(b.data(),b.size()); for(auto& b:rwObjs)Reg(b.data(),b.size()); for(auto& b:frames)Reg(b.data(),b.size());
        Reg(fakeFx.data(),fakeFx.size()); Reg(outBuf.data(),outBuf.size()); Reg(gxt.data(),gxt.size()); Reg(objVt.data(),sizeof(objVt));
        Reg(reinterpret_cast<void*>(CAMERA),CAMERA_SIZE); Reg(reinterpret_cast<void*>(PADS),2*0x134); Reg(reinterpret_cast<void*>(PLAYERS),2*0x190); }
    for(auto& v:objVt)v=reinterpret_cast<void*>(&FakeVTrap); objVt[0]=reinterpret_cast<void*>(&FakeObjDtor);

    // modelos usados (indices validos del EXE)
    std::vector<uint16> tlModels; for(uintptr_t a=0x8CD4F4;a<=0x8CD514;a+=4){ const uint16 m=*reinterpret_cast<uint16*>(a); if(m<20000)tlModels.push_back(m); }
    const auto* const miPtrs=reinterpret_cast<void**>(MIPTRS);
    std::vector<uint16> pkModels; for(uintptr_t a=0x8CD5BC;a<=0x8CD5F8;a+=4){ const uint16 m=*reinterpret_cast<uint16*>(a); if(m<20000&&miPtrs[m])pkModels.push_back(m); }
    for(uint16 m:{uint16(*reinterpret_cast<uint16*>(0x8CD750)),uint16(366),uint16(370),uint16(355)})if(m<20000&&miPtrs[m])pkModels.push_back(m);
    if(tlModels.empty()||pkModels.empty()){ Write({{"event","error"},{"reason","no models"}}); return; }
    constexpr uint16 OTHER_TL=1500, VEH_MODEL=400;

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    std::vector<std::pair<uintptr_t,size_t>> areas{
        {CAMERA,CAMERA_SIZE},{TIME,4},{TSTEP,4},{FRAME,4},{0x969130,0x60},{0xB7C4A0,0x80},{0xC812A8,4},{PEDLIGHTS,0x34},{PATHNODES,0x100},{NAVINODES,0x100},
        {PICKUPS,PICKUPS_SIZE},{0x8A5F48,1},{0x97D640,8},{0x978618,12},{0x97D660,12},{PLAYERS,2*0x190},{0xB7CD74,1},{0xBAA45D,1},{0x96A9AC,12},{0x96A840,4},
        {0xB5F84C,8},{PADS,2*0x134},{VEHPOOL,4},{EXPL,EXPL_SIZE}};
    for(uint16 m:tlModels)areas.emplace_back(MIPTRS+m*4u,4); areas.emplace_back(MIPTRS+OTHER_TL*4u,4); areas.emplace_back(MIPTRS+VEH_MODEL*4u,4);
    for(uint16 m:pkModels)areas.emplace_back(STREAMINFO+m*20u,20);
    for(auto [a,n]:areas)saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch r1(0x821B1E,FakeRand), r2(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber);
        // semaforos
        EntryPatch t1(bit_cast<uintptr_t>(&CGameLogic::LaRiotsActiveHere),FakeRiots), t1e(0x441C10,FakeRiots);
        EntryPatch t2(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),FakeAppearance), t2e(0x6D1080,FakeAppearance);
        EntryPatch t3(bit_cast<uintptr_t>(&CGeneral::GetATanOfXY),FakeATan), t3e(0x53CC70,FakeATan);
        EntryPatch t4(bit_cast<uintptr_t>(&CBaseModelInfo::Get2dEffect),FakeGet2dEffect), t4e(0x4C4C70,FakeGet2dEffect);
        EntryPatch t5(bit_cast<uintptr_t>(static_cast<void(*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector&,float,float,eCoronaType,eCoronaFlareType,eCoronaReflType,eCoronaLOSCheck,eCoronaTrail,float,bool,float,bool,float,bool,bool)>(&CCoronas::RegisterCorona)),FakeCorona), t5e(0x6FC580,FakeCorona);
        EntryPatch t6(bit_cast<uintptr_t>(&CBrightLights::RegisterOne),FakeBright), t6e(0x724770,FakeBright);
        EntryPatch t7(bit_cast<uintptr_t>(&CPointLights::AddLight),FakeAddLight), t7e(0x7000E0,FakeAddLight);
        EntryPatch t8(bit_cast<uintptr_t>(&CShadows::StoreStaticShadow),FakeStaticShadow), t8e(0x70BA00,FakeStaticShadow);
        EntryPatch t9(bit_cast<uintptr_t>(&CShinyTexts::RegisterOne),FakeShiny), t9e(0x724B60,FakeShiny);
        // pickups
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<void(*)(eBlipType,int32)>(&CRadar::ClearBlipForEntity)),FakeClearBlip), p1e(0x587C60,FakeClearBlip);
        EntryPatch p2(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove), p2e(0x563280,FakeWorldRemove);
        EntryPatch p3(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), p3e(0x563220,FakeWorldAdd);
        EntryPatch p4(bit_cast<uintptr_t>(static_cast<CObject*(*)(int32,bool)>(&CObject::Create)),FakeCreate), p4e(0x5A1F60,FakeCreate);
        EntryPatch p5(bit_cast<uintptr_t>(&CPools::MakeSureSlotInObjectPoolIsEmpty),FakeMakeSure), p5e(0x550080,FakeMakeSure);
        EntryPatch p6(bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation)),FakeSetOrient), p6e(0x439A80,FakeSetOrient);
        EntryPatch p7(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegRef), p7e(0x571B70,FakeRegRef);
        EntryPatch p8(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanRef), p8e(0x571A00,FakeCleanRef);
        EntryPatch p9(bit_cast<uintptr_t>(&CPickups::WeaponForModel),FakeWeaponForModel), p9e(0x454AE0,FakeWeaponForModel);
        EntryPatch p10(bit_cast<uintptr_t>(static_cast<bool(*)(eWeaponType)>(&CWeapon::CanBeUsedFor2Player)),FakeCanBe2P), p10e(0x73B240,FakeCanBe2P);
        EntryPatch p11(bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission),FakeFlag<15>), p11e(0x464D50,FakeFlag<15>);
        EntryPatch p12(bit_cast<uintptr_t>(&CDarkel::FrenzyOnGoing),FakeFlag<16>), p12e(0x43D1F0,FakeFlag<16>);
        EntryPatch p13(bit_cast<uintptr_t>(&CLocalisation::KillFrenzy),FakeFlag<17>), p13e(0x56D2A0,FakeFlag<17>);
        EntryPatch p14(bit_cast<uintptr_t>(&CLocalisation::GermanGame),FakeFlag<18>), p14e(0x56D200,FakeFlag<18>);
        EntryPatch p15(bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn),FakeFlag<19>), p15e(0x441390,FakeFlag<19>);
        EntryPatch p16(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPed), p16e(0x56E210,FakeFindPed);
        EntryPatch p17(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindVeh), p17e(0x56E0D0,FakeFindVeh);
        EntryPatch p18(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeCoors), p18e(0x56E010,FakeCoors);
        EntryPatch p19(bit_cast<uintptr_t>(&CPlaceable::GetHeading),FakeHeading), p19e(0x441DB0,FakeHeading);
        EntryPatch p20(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),FakePadFromPlayer), p20e(0x609560,FakePadFromPlayer);
        EntryPatch p21(bit_cast<uintptr_t>(&CPlayerPed::CanPlayerStartMission),FakeCanStart), p21e(0x609590,FakeCanStart);
        EntryPatch p22(bit_cast<uintptr_t>(&CPed::IsAlive),FakeIsAlive), p22e(0x5E0170,FakeIsAlive);
        EntryPatch p23(bit_cast<uintptr_t>(&CPed::GrantAmmo),FakeGrantAmmo), p23e(0x5DF220,FakeGrantAmmo);
        EntryPatch p24(bit_cast<uintptr_t>(&CPed::GiveDelayedWeapon),FakeGiveDelayed), p24e(0x5E89B0,FakeGiveDelayed);
        EntryPatch p25(bit_cast<uintptr_t>(&CPlayerPed::DoesPlayerWantNewWeapon),FakeWantWeapon), p25e(0x609710,FakeWantWeapon);
        EntryPatch p26(bit_cast<uintptr_t>(&CPed::GetWeaponSlot),FakeWeaponSlot), p26e(0x5DF200,FakeWeaponSlot);
        EntryPatch p27(bit_cast<uintptr_t>(&CPed::Say),FakeSay), p27e(0x5EFFE0,FakeSay);
        EntryPatch p28(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskJetPack),FakeJetPack), p28e(0x601110,FakeJetPack);
        EntryPatch p29(bit_cast<uintptr_t>(&CWeapon::Reload),FakeReload), p29e(0x73AEB0,FakeReload);
        EntryPatch p30(bit_cast<uintptr_t>(&CPad::GetTarget),FakeGetTarget), p30e(0x540670,FakeGetTarget);
        EntryPatch p31(bit_cast<uintptr_t>(&CPad::StartShake),FakeStartShake), p31e(0x53F920,FakeStartShake);
        EntryPatch p32(bit_cast<uintptr_t>(&CPad::GetTouchedTimeDelta),FakeTouched), p32e(0x53F210,FakeTouched);
        EntryPatch p33(bit_cast<uintptr_t>(&CPad::StartShake_Distance),FakeShakeDistance), p33e(0x53F9A0,FakeShakeDistance);
        EntryPatch p34(bit_cast<uintptr_t>(&CText::Get),FakeTextGet), p34e(0x6A0050,FakeTextGet);
        EntryPatch p35(bit_cast<uintptr_t>(&CHud::SetHelpMessage),FakeSetHelp), p35e(0x588BE0,FakeSetHelp);
        EntryPatch p36(bit_cast<uintptr_t>(&CHud::HelpMessageDisplayed),FakeHelpShown), p36e(0x588B50,FakeHelpShown);
        EntryPatch p37(bit_cast<uintptr_t>(&CMessages::InsertNumberInString),FakeInsertNumber), p37e(0x69DE90,FakeInsertNumber);
        EntryPatch p38(bit_cast<uintptr_t>(&ModifyStringLabelForControlSetting),FakeModifyLabel), p38e(0x454B70,FakeModifyLabel);
        EntryPatch p39(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),FakeFrontend), p39e(0x506EA0,FakeFrontend);
        EntryPatch p40(bit_cast<uintptr_t>(&CPickups::GivePlayerGoodiesWithPickUpMI),FakeGoodies), p40e(0x4564F0,FakeGoodies);
        EntryPatch p41(bit_cast<uintptr_t>(&CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment),FakeCanPickUp), p41e(0x4554C0,FakeCanPickUp);
        EntryPatch p42(bit_cast<uintptr_t>(&CVehicle::IsSphereTouchingVehicle),FakeTouching), p42e(0x6D84D0,FakeTouching);
        EntryPatch p43(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),FakeWaterLevel), p43e(0x6EB690,FakeWaterLevel);
        EntryPatch p44(bit_cast<uintptr_t>(&CCollision::TestLineSphere),FakeLineSphere), p44e(0x417470,FakeLineSphere);
        // explosiones
        EntryPatch e1(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),FakeCreateFx), e1e(0x4A9BE0,FakeCreateFx);
        EntryPatch e2(bit_cast<uintptr_t>(&FxSystem_c::PlayAndKill),FakePlayAndKill), e2e(0x4AA3D0,FakePlayAndKill);
        EntryPatch e3(bit_cast<uintptr_t>(&CAEExplosionAudioEntity::AddAudioEvent),FakeAudioEvent), e3e(0x4DCBE0,FakeAudioEvent);
        EntryPatch e4(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),FakeGroundZ), e4e(0x5696C0,FakeGroundZ);
        EntryPatch e5(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWaves),FakeWaterNoWaves), e5e(0x6E8580,FakeWaterNoWaves);
        EntryPatch e6(bit_cast<uintptr_t>(&CCrime::ReportCrime),FakeReportCrime), e6e(0x532010,FakeReportCrime);
        EntryPatch e7(bit_cast<uintptr_t>(&CInterestingEvents::Add),FakeInteresting), e7e(0x602590,FakeInteresting);
        EntryPatch e8(bit_cast<uintptr_t>(static_cast<CFire*(CFireManager::*)(CVector,float,uint8,CEntity*,uint32,int8,uint8)>(&CFireManager::StartFire)),FakeStartFire), e8e(0x539F00,FakeStartFire);
        EntryPatch e9(bit_cast<uintptr_t>(static_cast<void(*)(eStats,float)>(&CStats::IncrementStat)),FakeIncStat), e9e(0x55C180,FakeIncStat);
        EntryPatch e10(bit_cast<uintptr_t>(&CShadows::AddPermanentShadow),FakePermShadow), e10e(0x706F60,FakePermShadow);
        EntryPatch e11(bit_cast<uintptr_t>(&CWorld::TriggerExplosion),FakeTrigger), e11e(0x56B790,FakeTrigger);
        EntryPatch e12(bit_cast<uintptr_t>(&CCamera::CamShake),FakeCamShake), e12e(0x50A9F0,FakeCamShake);
        EntryPatch e13(bit_cast<uintptr_t>(&CCreepingFire::TryToStartFireAtCoors),FakeTryFire), e13e(0x53A450,FakeTryFire);
        EntryPatch e14(bit_cast<uintptr_t>(&CWorld::SetPedsOnFire),FakePedsOnFire), e14e(0x565610,FakePedsOnFire);
        EntryPatch e15(bit_cast<uintptr_t>(&CWorld::SetWorldOnFire),FakeWorldOnFire), e15e(0x5659F0,FakeWorldOnFire);
        EntryPatch e16(bit_cast<uintptr_t>(&CWorld::SetCarsOnFire),FakeCarsOnFire), e16e(0x56B910,FakeCarsOnFire);
        EntryPatch e17(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FakeVLine), e17e(0x5674E0,FakeVLine);

        auto& camMatrix=*reinterpret_cast<void**>(CAMERA+0x14); auto& camPlacement=*reinterpret_cast<CVector*>(CAMERA+4);
        auto& timeMs=*reinterpret_cast<uint32*>(TIME); auto& timeStep=*reinterpret_cast<float*>(TSTEP); auto& frameCounter=*reinterpret_cast<uint32*>(FRAME);
        auto* pathNodes=reinterpret_cast<void**>(PATHNODES); auto* naviNodes=reinterpret_cast<void**>(NAVINODES); auto* miSlots=reinterpret_cast<void**>(MIPTRS);
        auto* expl=reinterpret_cast<uint8*>(EXPL);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; }; // sin NaN
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto fillMatrix=[&](void* m,const CVector& pos){ fillWords(m,0x48); for(int k=0;k<12;++k)if(k!=3&&k!=7&&k!=11)At<float>(m,k*4)=uni(-1.f,1.f); At<CVector>(m,0x30)=pos; At<void*>(m,0x40)=nullptr; };
                auto setCamera=[&](const CVector& p){ if(next()%3){ fillMatrix(camMat.data(),p); camMatrix=camMat.data(); camPlacement=randVec(-10.f,10.f); } else { camMatrix=nullptr; camPlacement=p; } };
                auto pickModel=[&]{ return pkModels[next()%pkModels.size()]; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1212u; if(!stt)stt=1; trace=json::array();
                    stub=stt*0x2545F491u|1u; rngCalls=0; riotsCalls=0; appCalls=0;
                    for(auto& p:pct)p=next()%101;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // tiempo con mascaras cerca de los umbrales de los semaforos
                    { static const uint32 marks[]{0,4999,5000,5999,6000,10999,11000,11999,12000,15382,15383,15384,16383};
                      const uint32 m=next()%3 ? marks[next()%std::size(marks)] : next()%16384; timeMs=(m<<1)|(next()&1)|((next()&0x7FFFu)<<15); }
                    timeStep=next()%6==0 ? uni(3.f,10.f) : uni(0.f,2.f); frameCounter=next();

                    if(cls=="CTrafficLights") {
                        riotsMask=next()%3 ? 0 : next(); *reinterpret_cast<uint8*>(0x96914E)=uint8(next()%4==0);
                        auto setupEntity=[&](uint8* e,uint8* m){ fillWords(e,0x200); fillMatrix(m,randVec(-3000.f,3000.f)); At<void*>(e,0x14)=m;
                            At<float>(m,0x28)=next()%6==0 ? (next()%2 ? 0.96f : std::bit_cast<float>(0x3F75C28Eu)) : uni(0.9f,1.f);
                            At<uint32>(e,0x1C)=next()&~(next()%8 ? 0x200u : 0u);
                            At<uint16>(e,0x22)=next()%5 ? tlModels[next()%tlModels.size()] : OTHER_TL; };
                        const float degs=next()%4==0 ? float(std::array{60,150,240,330}[next()%4]) : uni(-30.f,400.f);
                        atanResult=next()%25==0 ? QNAN : degs/57.2957763671875f;
                        if(nm=="LightForCars1"||nm=="LightForCars2"||nm=="LightForCars1_Visual"||nm=="LightForCars2_Visual"||nm=="LightForPeds")
                            ret=reinterpret_cast<uint8(__cdecl*)()>(a)();
                        else if(nm=="IsMITrafficLight"){ int32 m; const uint32 k=next()%8;
                            m=k<5 ? int32(tlModels[next()%tlModels.size()]) : k==5 ? -1 : k==6 ? int32(tlModels[next()%tlModels.size()]|0x10000u) : int32(next()%20000);
                            ret=reinterpret_cast<uint8(__cdecl*)(int32)>(a)(m); }
                        else if(nm=="FindTrafficLightTypeFromOrientation"){ float o;
                            static const float edges[]{60.f,150.f,240.f,330.f};
                            const uint32 k=next()%10; const float e=edges[next()%4];
                            o=k==0 ? e : k==1 ? std::nextafter(e,0.f) : k==2 ? std::nextafter(e,1000.f) : k==3 ? QNAN : k==4 ? -uni(0.f,400.f) : uni(0.f,420.f);
                            ret=reinterpret_cast<int32(__cdecl*)(float)>(a)(o); }
                        else if(nm=="FindOrientationForTrafficLightType"){ setupEntity(tlEnt.data(),tlMat.data()); ret=F(reinterpret_cast<float(__cdecl*)(void*)>(a)(tlEnt.data())); }
                        else if(nm=="FindTrafficLightType"){ setupEntity(tlEnt.data(),tlMat.data()); ret=reinterpret_cast<int32(__cdecl*)(void*)>(a)(tlEnt.data()); }
                        else if(nm=="DisplayActualLight"){
                            setupEntity(tlEnt.data(),tlMat.data());
                            fillWords(fakeMi.data(),fakeMi.size()); fakeMi[0x0D]=uint8(next()%5==0 ? 0 : 1+next()%8);
                            for(uint16 m:tlModels)miSlots[m]=fakeMi.data(); miSlots[OTHER_TL]=fakeMi.data();
                            for(auto& e:effects){ fillWords(e.data(),0x40); At<CVector>(e.data(),0)=randVec(-3.f,3.f); if(next()%8==0)At<float>(e.data(),4)=0.f;
                                e[0x0C]=uint8(next()%6==0 ? 1+next()%8 : 0); e[0x10]=next()%2 ? uint8(201+next()%55) : uint8(next()%2 ? 200 : next()%200);
                                e[0x11]=next()%2 ? uint8(101+next()%155) : uint8(next()%2 ? 100 : next()%100); e[0x12]=uint8(next()); }
                            At<CVector>(reinterpret_cast<void*>(CAMERA+0x984),0)=randVec(-1.f,1.f);
                            auto* tc=reinterpret_cast<uint8*>(0xB7C4A0); At<float>(tc,0x40)=uni(0.f,3.f); At<float>(tc,0x44)=uni(0.f,30.f); At<float>(tc,0x58)=uni(0.f,40.f);
                            const uint32 tk=next()%6; *reinterpret_cast<float*>(0xC812A8)=tk==0 ? 0.5f : tk==1 ? 0.05f : uni(0.f,1.f);
                            fillWords(reinterpret_cast<void*>(PEDLIGHTS),0x30); for(int k=0;k<12;++k)At<float>(reinterpret_cast<void*>(PEDLIGHTS),k*4)=uni(-3.f,3.f);
                            *reinterpret_cast<uint32*>(0xA9ADE0)=next()%3==0 ? next()%16 : (next()%2 ? 15 : 0);
                            reinterpret_cast<void(__cdecl*)(void*)>(a)(tlEnt.data());
                            out["ped_lights"]=ReversingProbe::Bytes(reinterpret_cast<void*>(PEDLIGHTS),0x34); }
                        else { // ShouldCarStopForLight / ShouldCarStopForBridge
                            uint8* v=vehs[0].data(); fillWords(v,0xA18);
                            const CVector vp=randVec(-2900.f,2900.f);
                            if(next()%4){ fillMatrix(vehMat.data(),vp); At<void*>(v,0x14)=vehMat.data(); } else { At<void*>(v,0x14)=nullptr; At<CVector>(v,4)=vp; }
                            v[0x36]=uint8((next()&7)|((next()%3==0 ? 3 : next()%10)<<3));
                            appearance={int32(next()%5==0 ? 3 : next()%6==0 ? 5 : next()%7),int32(next()%5==0 ? 5 : next()%7)};
                            fillWords(nodeDummy.data(),nodeDummy.size());
                            for(int k=0;k<64;++k){ pathNodes[k]=k<10&&next()%5 ? nodeDummy.data() : nullptr; naviNodes[k]=links.data()+(k%8)*64*14; }
                            static const uint32 addrs[]{0x00050001u,0x00070002u,0x00010001u,0x00050002u};
                            for(int k=0;k<8*64;++k){ uint8* l=links.data()+k*14;
                                At<int16>(l,0)=int16(vp.x*8.f+uni(-160.f,160.f)); At<int16>(l,2)=int16(vp.y*8.f+uni(-160.f,160.f));
                                At<uint32>(l,4)=addrs[next()%4]; l[8]=uint8(int8(int32(next()%201)-100)); l[9]=uint8(int8(int32(next()%201)-100)); l[0xA]=uint8(next());
                                l[0xB]=uint8(next()); At<uint16>(l,0xC)=uint16((next()&0xFFF8u)|(next()%5==0 ? 0u : next()%4==0 ? 3u : 1u+next()%2)|(next()%2 ? 4u : 0u)); }
                            for(size_t o:{0x390u,0x394u,0x398u})At<uint32>(v,o)=addrs[next()%4];
                            for(size_t o:{0x3A4u,0x3A6u,0x3A8u})At<uint16>(v,o)=next()%8==0 ? 0xFFFFu : uint16(((next()%12)<<10)|(next()%64));
                            for(size_t o:{0x3B4u,0x3B5u,0x3B6u})v[o]=next()%2 ? 0xFF : uint8(next());
                            if(nm=="ShouldCarStopForBridge")ret=reinterpret_cast<uint8(__cdecl*)(void*)>(a)(v);
                            else ret=reinterpret_cast<uint8(__cdecl*)(void*,uint8)>(a)(v,uint8(next()%3==0)); }
                    }
                    else if(cls=="CPickup") {
                        const uint32 idx=next()%620; uint8* const p=reinterpret_cast<uint8*>(PICKUPS+idx*0x20);
                        playerCoors=randVec(-2900.f,2900.f);
                        // jugador
                        fillWords(ped.data(),ped.size()); fillMatrix(pedMat.data(),playerCoors); At<void*>(ped.data(),0x14)=pedMat.data(); At<void*>(ped.data(),0x18)=nullptr;
                        At<void*>(ped.data(),0x47C)=intel.data(); At<void*>(ped.data(),0x480)=playerData.data(); At<float>(ped.data(),0x540)=uni(0.f,200.f); At<float>(ped.data(),0x548)=uni(0.f,200.f);
                        for(int s=0;s<13;++s){ At<int32>(ped.data(),0x5A0+s*0x1C)=next()%3==0 ? 0 : int32(next()%47); At<uint32>(ped.data(),0x5AC+s*0x1C)=next()%3==0 ? 0 : next()%500; }
                        fillWords(intel.data(),intel.size()); fillWords(playerData.data(),playerData.size()); At<void*>(playerData.data(),0)=wanted.data();
                        playerData[0x20]=uint8(next()%4==0 ? 0 : next()%14); At<uint32>(playerData.data(),0x6C)=timeMs-(next()%4==0 ? 1500u : next()%3000u);
                        fillWords(wanted.data(),wanted.size()); At<uint32>(wanted.data(),0x2C)=next()%3==0 ? 0 : next()%7;
                        fillWords(ped2.data(),ped2.size()); fillMatrix(ped2Mat.data(),playerCoors); At<void*>(ped2.data(),0x14)=ped2Mat.data(); At<void*>(ped2.data(),0x480)=playerData.data();
                        findPed={ped.data(),ped.data(),next()%2 ? ped2.data() : nullptr}; findVeh=next()%2 ? vehs[1].data() : nullptr;
                        fillWords(fakePad.data(),fakePad.size()); At<uint16>(fakePad.data(),0x10E)=next()%4==0 ? uint16(1+next()%3) : 0; padResult=next()%5 ? fakePad.data() : nullptr;
                        jetpackResult=next()%5==0 ? fakeTask.data() : nullptr;
                        At<int16>(reinterpret_cast<void*>(PADS),0x10A)=int16(next()%5);
                        // objetos
                        auto setupObj=[&](uint8* o,uint8* m){ fillWords(o,0x180); At<void*>(o,0)=objVt.data(); fillMatrix(m,playerCoors+CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-3.f,3.f)});
                            At<void*>(o,0x14)=m; At<void*>(o,0x18)=nullptr; At<uint16>(o,0x22)=pickModel(); At<CVector>(o,0x44)=randVec(-0.3f,0.3f);
                            At<uint32>(o,0x140)=next()&~(next()%10 ? 0x2000000u : 0u); };
                        setupObj(obj.data(),objMat.data()); setupObj(obj2.data(),obj2Mat.data());
                        createResult=next()%5 ? obj2.data() : nullptr; makeSureRef=nullptr; makeSureFill=nullptr;
                        for(uint16 m:pkModels)reinterpret_cast<uint8*>(STREAMINFO+m*20u)[0x10]=uint8(next()%4 ? 1 : next()%5);
                        // vehiculos y pool
                        for(auto& vb:vehs){ fillWords(vb.data(),0xA18); At<void*>(vb.data(),0x14)=nullptr; At<CVector>(vb.data(),4)=playerCoors; }
                        At<void*>(vehPool.data(),0)=vehs[0].data(); At<void*>(vehPool.data(),4)=vehFlags.data(); At<int32>(vehPool.data(),8)=4; At<int32>(vehPool.data(),0xC)=-1;
                        for(int k=0;k<4;++k)vehFlags[k]=next()%3==0 ? uint8(0x80|(next()&0x7F)) : uint8(next()&0x7F);
                        *reinterpret_cast<void**>(VEHPOOL)=vehPool.data();
                        // pickup
                        std::memset(p,0,0x20); At<float>(p,0)=next()%20==0 ? QNAN : uni(0.f,60.f); At<void*>(p,4)=next()%7 ? obj.data() : nullptr;
                        At<uint32>(p,8)=next()%4==0 ? 0 : next()%10==0 ? (0x80000000u|next()) : next()%2000; At<uint32>(p,0xC)=timeMs+uint32(int32(next()%40000)-20000);
                        const CVector pp=playerCoors+CVector{uni(-15.f,15.f),uni(-15.f,15.f),uni(-3.f,3.f)};
                        At<int16>(p,0x10)=int16(pp.x*8.f); At<int16>(p,0x12)=int16(pp.y*8.f); At<int16>(p,0x14)=int16(pp.z*8.f);
                        At<uint16>(p,0x16)=uint16(next()); At<int16>(p,0x18)=int16(pickModel()); At<int16>(p,0x1A)=int16(next());
                        static const uint8 types[]{0,1,2,2,3,3,4,4,5,6,7,8,8,9,10,11,12,13,14,15,15,16,16,17,18,18,19,20,21,21,22,22};
                        p[0x1C]=types[next()%std::size(types)]; p[0x1D]=uint8((next()%4==0 ? 1 : 0)|(next()&0x7E));
                        *reinterpret_cast<uint8*>(0x8A5F48)=uint8(next()%4); *reinterpret_cast<int32*>(0x97D644)=int32(next()%3);
                        *reinterpret_cast<int32*>(0x97D640)=int32(next()%8); *reinterpret_cast<uint8*>(0xB7CD74)=uint8(next()%2);
                        for(int k=0;k<2;++k){ uint8* pl=reinterpret_cast<uint8*>(PLAYERS+k*0x190); At<int32>(pl,0xB8)=int32(next()%3==0 ? next()%100 : next()%100000); pl[0x14F]=uint8(100+next()%100); pl[0x150]=uint8(100+next()%100); }
                        *reinterpret_cast<int32*>(0xB5F84C)=next()%8==0 ? 2 : int32(next()%4); *reinterpret_cast<uint8*>(0xB5F851)=uint8(next()%8==0);
                        *reinterpret_cast<uint8*>(CAMERA+0x3D)=uint8(next()%4==0);

                        if(nm=="SetPosn"){ CVector v=randVec(-4000.f,4000.f); const uint32 k=next()%12;
                            if(k==0)v.x=QNAN; else if(k==1)v.y=3.0e9f; else if(k==2)v.z=-5000.f; else if(k==3)v.x=-0.06f;
                            reinterpret_cast<void(__thiscall*)(void*,CVector)>(a)(p,v); }
                        else if(nm=="GetPosn"){ At<uint32>(p,0x10)=next(); At<int16>(p,0x14)=int16(next()); auto* o=reinterpret_cast<CVector*>(outBuf.data());
                            ret=Id(reinterpret_cast<CVector*(__thiscall*)(void*,CVector*)>(a)(p,o)); out["v"]=Vec(*o); }
                        else if(nm=="IsVisible"){ setCamera(pp+CVector{uni(-150.f,150.f),uni(-150.f,150.f),uni(-50.f,50.f)});
                            if(next()%6==0){ const float ang=uni(0.f,6.28f); const CVector c{float(At<int16>(p,0x10))*0.125f+std::cos(ang)*100.f,float(At<int16>(p,0x12))*0.125f+std::sin(ang)*100.f,0.f}; setCamera(c); }
                            uint8 r{}; const uint32 edx=CallKeepEdx0(a,p,r); ret=r; out["edx"]=edx; }
                        else if(nm=="GetRidOfObjects")reinterpret_cast<void(__thiscall*)(void*)>(a)(p);
                        else if(nm=="FindStringForTextIndex"){ static const int32 idxs[]{0,1,2,3,4,7,-1,0x101}; const int32 k=idxs[next()%std::size(idxs)];
                            ret=std::string(reinterpret_cast<const char*(__cdecl*)(int32)>(a)(k)); }
                        else if(nm=="FindTextIndexForString"){ static const char* strs[]{nullptr,"PROP_3","prop_3","PROP_4","Prop_4","PROP_5","","FESZ_CA","PROP_3 ","PROP"};
                            ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(strs[next()%std::size(strs)]); }
                        else if(nm=="ExtractAmmoFromPickup"){ At<void*>(p,4)=obj.data(); reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(p,ped.data()); }
                        else if(nm=="PickUpShouldBeInvisible"){
                            if(next()%3==0){ static const CVector parks[]{{1479.f,-1658.f,0.f},{2511.f,1208.f,0.f}}; const CVector& c=parks[next()%2]; const float ang=uni(0.f,6.28f), d=next()%5==0 ? 10.f : uni(0.f,14.f);
                                At<int16>(p,0x10)=int16((c.x+std::cos(ang)*d)*8.f); At<int16>(p,0x12)=int16((c.y+std::sin(ang)*d)*8.f); }
                            ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(p); }
                        else if(nm=="GiveUsAPickUpObject"){ auto** ref=next()%2 ? reinterpret_cast<void**>(p+4) : reinterpret_cast<void**>(outBuf.data()); *ref=next()%2 ? obj.data() : nullptr;
                            int32 slot=-1; if(next()%3==0){ slot=int32(next()%700); makeSureRef=ref; makeSureFill=obj.data(); }
                            reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(a)(p,ref,slot); out["ref"]=Id(*ref); }
                        else if(nm=="ProcessGunShot"){ auto* s=reinterpret_cast<CVector*>(outBuf.data()); auto* e=reinterpret_cast<CVector*>(outBuf.data()+0x10); *s=playerCoors+randVec(-10.f,10.f); *e=playerCoors+randVec(-10.f,10.f);
                            for(int k=0;k<16;++k){ std::memset(expl+k*0x7C,0,0x7C); expl[k*0x7C+0x28]=uint8(next()%3 ? 1 : 0); }
                            reinterpret_cast<void(__thiscall*)(void*,const CVector*,const CVector*)>(a)(p,s,e); out["expl"]=ReversingProbe::Bytes(expl,EXPL_SIZE); }
                        else if(nm=="Remove")reinterpret_cast<void(__thiscall*)(void*)>(a)(p);
                        else { // Update
                            for(int k=0;k<16;++k){ std::memset(expl+k*0x7C,0,0x7C); expl[k*0x7C+0x28]=uint8(next()%3 ? 1 : 0); }
                            void* veh=next()%5<3 ? nullptr : vehs[next()%4].data(); const int32 id=int32(next()%2);
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,void*,void*,int32)>(a)(p,ped.data(),veh,id);
                            out["expl"]=Fnv(expl,EXPL_SIZE); out["player_data"]=ReversingProbe::Bytes(playerData.data(),0xAC);
                            out["players"]=ReversingProbe::Bytes(reinterpret_cast<void*>(PLAYERS+0xB8),4); out["players2"]=ReversingProbe::Bytes(reinterpret_cast<void*>(PLAYERS+0x190+0xB8),4);
                            out["globals"]=json::array({ReversingProbe::Bytes(reinterpret_cast<void*>(0x8A5F48),1),ReversingProbe::Bytes(reinterpret_cast<void*>(0x97D640),8),
                                ReversingProbe::Bytes(reinterpret_cast<void*>(0x978618),12),ReversingProbe::Bytes(reinterpret_cast<void*>(0x97D660),12),ReversingProbe::Bytes(reinterpret_cast<void*>(0xBAA45D),1),
                                ReversingProbe::Bytes(reinterpret_cast<void*>(0x96A9AC),12),ReversingProbe::Bytes(reinterpret_cast<void*>(0x96A840),4)}); }
                        out["pickup"]=ReversingProbe::Bytes(p,0x20); out["obj"]=ReversingProbe::Bytes(obj.data(),0x180); out["obj2"]=ReversingProbe::Bytes(obj2.data(),0x180);
                        out["obj_mat"]=ReversingProbe::Bytes(objMat.data(),0x48);
                    }
                    else { // CExplosion
                        // entidades: con o sin matriz, con o sin RwObject; el modelo apunta a un vehiculo falso con dummy en +0x54
                        fillWords(fakeVehMi.data(),fakeVehMi.size()); fillWords(vehStruct.data(),vehStruct.size()); At<void*>(fakeVehMi.data(),0x5C)=vehStruct.data();
                        At<CVector>(vehStruct.data(),0x54)=randVec(-3.f,3.f); miSlots[VEH_MODEL]=fakeVehMi.data();
                        for(int k=0;k<4;++k){ uint8* e=ents[k].data(); fillWords(e,0x200); const CVector pos=randVec(-2900.f,2900.f);
                            if(next()%4){ fillMatrix(entMats[k].data(),pos); At<void*>(e,0x14)=entMats[k].data(); } else { At<void*>(e,0x14)=nullptr; At<CVector>(e,4)=pos; }
                            fillWords(rwObjs[k].data(),0x10); fillWords(frames[k].data(),0x60); At<void*>(rwObjs[k].data(),4)=frames[k].data();
                            At<void*>(e,0x18)=next()%4 ? rwObjs[k].data() : nullptr; At<uint16>(e,0x22)=VEH_MODEL;
                            e[0x36]=uint8((next()&0xF8)|(next()%2 ? 3 : 2)); At<int32>(e,0x598)=next()%2 ? 0 : int32(next()%4); }
                        At<void*>(ped.data(),0x14)=nullptr; At<CVector>(ped.data(),4)=randVec(-100.f,100.f); ped[0x36]=3; At<int32>(ped.data(),0x598)=0;
                        findPed={ped.data(),ped.data(),nullptr};
                        auto entity=[&]()->void*{ const uint32 k=next()%6; return k<2 ? nullptr : ents[k-2].data(); };
                        auto fillExplosions=[&](uint32 activePct){ for(int k=0;k<16;++k){ uint8* e=expl+k*0x7C; fillWords(e,0x7C);
                            At<int32>(e,0)=next()%10==0 ? int32(13+next()%3) : int32(next()%13); At<CVector>(e,4)=randVec(-2900.f,2900.f);
                            At<float>(e,0x10)=uni(0.f,20.f); At<float>(e,0x14)=uni(0.f,1.f); At<void*>(e,0x18)=entity(); At<void*>(e,0x1C)=entity();
                            At<float>(e,0x20)=float(timeMs)+uni(-3000.f,3000.f); At<float>(e,0x24)=uni(0.f,1.f);
                            e[0x28]=next()%100<activePct ? uint8(next()%4==0 ? 1 : 1+next()%30) : 0; e[0x29]=uint8(next()); e[0x2A]=uint8(next()%2);
                            At<float>(e,0x2C)=uni(0.f,1e6f); At<uint32>(e,0x30)=next()%2 ? 0 : timeMs+uint32(int32(next()%4000)-2000);
                            At<float>(e,0x34)=next()%4==0 ? 0.f : uni(0.f,600.f); At<float>(e,0x38)=uni(-100.f,100.f); At<int32>(e,0x3C)=next()%3==0 ? int32(next()%600)-100 : int32(next()%250);
                            for(int q=0;q<3;++q){ At<CVector>(e,0x40+q*12)=randVec(-1.f,1.f); At<float>(e,0x64+q*4)=next()%3==0 ? -uni(0.f,1.f) : (next()%5==0 ? 0.f : uni(0.f,2.f)); At<float>(e,0x70+q*4)=uni(20.f,30.f); } } };
                        const uint8 id=uint8(next()%16);
                        if(nm=="AddExplosion"){ fillExplosions(next()%8==0 ? 100 : 60);
                            auto* pl=reinterpret_cast<uint8*>(PLAYERS+*reinterpret_cast<uint8*>(0xB7CD74)*0x190); At<int32>(pl,0x140)=int32(next()%1000); At<float>(pl,0x148)=uni(0.f,100.f);
                            *reinterpret_cast<uint8*>(0xB7CD74)=uint8(next()%2);
                            void* victim=entity(); const uint32 ck=next()%4; void* creator=ck==0 ? nullptr : ck==1 ? static_cast<void*>(ped.data()) : entity();
                            const int32 type=next()%12==0 ? int32(next()%2 ? -1 : 13) : int32(next()%13);
                            const CVector pos=randVec(-2900.f,2900.f); const uint32 life=next()%3==0 ? 0 : next()%5000;
                            const uint32 sk=next()%8; const float shake=sk<3 ? -1.f : sk==3 ? QNAN : uni(0.f,2.f);
                            reinterpret_cast<void(__cdecl*)(void*,void*,int32,CVector,uint32,uint8,float,uint8)>(a)(victim,creator,type,pos,life,uint8(next()%3),shake,uint8(next()%3==0));
                            out["players"]=ReversingProbe::Bytes(reinterpret_cast<void*>(PLAYERS),2*0x190); }
                        else if(nm=="ClearAllExplosions"||nm=="Initialise"||nm=="Shutdown"){ fillExplosions(50); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="GetExplosionActiveCounter"||nm=="DoesExplosionMakeSound"){ fillExplosions(50); ret=reinterpret_cast<uint8(__cdecl*)(uint8)>(a)(id); }
                        else if(nm=="GetExplosionType"){ fillExplosions(50); ret=reinterpret_cast<int32(__cdecl*)(uint8)>(a)(id); }
                        else if(nm=="GetExplosionPosition"){ fillExplosions(50); ret=Id(reinterpret_cast<const CVector*(__cdecl*)(uint8)>(a)(id)); }
                        else if(nm=="ResetExplosionActiveCounter"){ fillExplosions(50); reinterpret_cast<void(__cdecl*)(uint8)>(a)(id); }
                        else if(nm=="TestForExplosionInArea"){ fillExplosions(40);
                            if(next()%10==0)At<float>(expl+id*0x7C,4+4*(next()%3))=QNAN;
                            const CVector c=At<CVector>(expl+id*0x7C,4); float b[6];
                            for(int k=0;k<3;++k){ const float cc=(&c.x)[k]; const uint32 m=next()%8; b[k*2]=m==0 ? cc : cc-uni(0.f,30.f); b[k*2+1]=m==1 ? cc : cc+uni(-5.f,30.f); }
                            const int32 type=next()%3==0 ? -1 : int32(next()%14);
                            ret=reinterpret_cast<uint8(__cdecl*)(int32,float,float,float,float,float,float)>(a)(type,b[0],b[1],b[2],b[3],b[4],b[5]); }
                        else if(nm=="RemoveAllExplosionsInArea"){ fillExplosions(70);
                            CVector c=At<CVector>(expl+id*0x7C,4)+randVec(-20.f,20.f); float r=next()%10==0 ? QNAN : uni(0.f,40.f);
                            if(next()%6==0){ const CVector& t=At<CVector>(expl+(next()%16)*0x7C,4); c=t+CVector{3.f,4.f,0.f}; r=5.f; }
                            reinterpret_cast<void(__cdecl*)(CVector,float)>(a)(c,r); }
                        else { // Update
                            fillExplosions(70);
                            for(int k=0;k<16;++k){ uint8* e=expl+k*0x7C; if(next()%4==0)At<float>(e,0x20)=float(timeMs+uint32(next()%3)); if(next()%5==0)At<int32>(e,0)=1; if(next()%5==0)At<int32>(e,0)=7; }
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["expl"]=ReversingProbe::Bytes(expl,EXPL_SIZE);
                    }
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreAll();
                    return out;
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
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
