#pragma once
// Banco dentro del motor: CReplay (47 funciones). Original (hooks de CReplay desactivados) frente a candidato
// (hooks activados) con fixtures deterministas. Nunca se graba ni se reproduce de verdad:
//  - pools de peatones y vehículos falsos (punteros globales 0xB74490/0xB74494 sustituidos durante el banco),
//    con objetos de bytes crudos, matrices propias y una vtable falsa (borrado y GetRideAnimData trazados);
//  - los buffers de repetición se usan solo en dos ventanas (cabeza y cola) de cada buffer, rellenadas en
//    cada ejecución; los 800 KB se guardan al empezar y se restauran al final;
//  - Mode, BufferStatus, Record/Playback, tablas de conversión, la cámara, el jugador 0, CTimer, reloj, tiempo,
//    población, trazas de bala, ratón, flags de menú/pausa y carga de modelos se guardan y restauran en cada ejecución;
//  - se simulan y trazan mundo (Add/Remove/línea de visión), streaming, referencias, animaciones, ficheros,
//    fuentes, audio, cámara (fundidos, matrices RW), FindPlayer*, teclado y las demás llamadas externas;
//  - las funciones de CReplay que llama la función probada se sustituyen por stubs solo dentro de su caso
//    (después de cambiar el estado de los hooks) cuando crearían peatones/vehículos reales o para aislarla.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Replay.h"
#include "Camera.h"
#include "Font.h"
#include "FileMgr.h"
#include "Clock.h"
#include "BulletTraces.h"
#include "SpecialFX.h"
#include "Skidmarks.h"
#include "WeaponEffects.h"
#include "Explosion.h"
#include "PlaneBanners.h"
#include "FireManager.h"
#include "Draw.h"
#include "TheZones.h"
#include "ControllerConfigManager.h"
#include "EntryExitManager.h"
#include "RealTimeShadowManager.h"
#include "PlayerPedData.h"
#include "../ProbeRuntime.hpp"

namespace Replay47Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- objetos falsos ---
constexpr int32 PED_SIZE=0x7C4, VEH_SIZE=0xA18, NPED=140, NVEH=110;
alignas(16) inline std::array<uint8,PED_SIZE*NPED> pedStore{};
alignas(16) inline std::array<uint8,VEH_SIZE*NVEH> vehStore{};
inline std::array<uint8,1+NPED> pedFlags{}; inline std::array<uint8,1+NVEH> vehFlags{};
struct RawPool { uint8* objects; uint8* flags; int32 size; int32 last; uint8 owns, deal, pad0, pad1; };
inline RawPool pedPool{}, vehPool{};
alignas(16) inline std::array<CMatrix,NPED> pedMats{}; alignas(16) inline std::array<CMatrix,NVEH> vehMats{};
alignas(16) inline std::array<std::array<float,8>,NVEH> rideData{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> clumps{};            // byte 0 = 2 (rpCLUMP)
alignas(16) inline std::array<std::array<uint8,0x60>,4> assocs{};            // asociaciones falsas
alignas(16) inline std::array<std::array<uint8,0x40>,4> entities{};          // FindPlayerPed/Vehicle falsos fuera de los pools
alignas(16) inline std::array<std::array<uint8,0x60>,4> playerDatas{};       // CPlayerPedData falsos (+4 = descripción de ropa)
alignas(16) inline std::array<std::array<uint8,0x78>,4> clothesDescs{};
alignas(16) inline std::array<std::array<uint8,20>,256> fakeGroups{};        // CAnimBlendAssocGroup falsos
alignas(16) inline std::array<std::array<uint8,0x20>,8> fakeBlocks{};        // CAnimBlock falsos (+0x10 = IsLoaded)
alignas(16) inline std::array<uint8,0x40> fakeRwCamera{}; alignas(16) inline std::array<uint8,0x100> fakeFrame{};
alignas(16) inline CMatrix camPlaceableMat{};
inline std::array<void*,64> fakeVt{};
inline uint8* const BUFFERS=reinterpret_cast<uint8*>(0x97FB88);
constexpr uint32 BUFSZ=100000, WIN=0x1000;
inline uint8* Ped(int32 i){ return pedStore.data()+i*PED_SIZE; }
inline uint8* Veh(int32 i){ return vehStore.data()+i*VEH_SIZE; }
inline uint8* Buf(uint32 s){ return BUFFERS+s*BUFSZ; }
// tamaños de paquete del EXE (tabla de 0x45C850)
inline uint32 PacketSize(uint8 t){ static const uint8 sz[20]{4,52,56,8,52,88,4,8,4,8,28,20,16,4,4,56,56,60,76,120}; return t<20 ? sz[t] : 0; }

inline json Id(const void* p) {
    if(!p)return nullptr; const auto u=uintptr_t(p);
    auto in=[&](const auto& a){ return u>=uintptr_t(a.data())&&u<uintptr_t(a.data())+sizeof(a); };
    auto rel=[&](const char* n,const auto& a,size_t st){ const auto o=u-uintptr_t(a.data()); return json::array({n,o/st,o%st}); };
    if(in(pedStore))return rel("ped",pedStore,PED_SIZE); if(in(vehStore))return rel("veh",vehStore,VEH_SIZE);
    if(u>=0x97FB88&&u<0x97FB88+8*BUFSZ)return json::array({"buf",(u-0x97FB88)/BUFSZ,(u-0x97FB88)%BUFSZ});
    if(in(clumps))return rel("clump",clumps,0x40); if(in(assocs))return rel("assoc",assocs,0x60); if(in(entities))return rel("ent",entities,0x40);
    if(in(playerDatas))return rel("pdata",playerDatas,0x60); if(in(clothesDescs))return rel("desc",clothesDescs,0x78);
    if(in(pedMats))return rel("pmat",pedMats,sizeof(CMatrix)); if(in(vehMats))return rel("vmat",vehMats,sizeof(CMatrix)); if(in(rideData))return rel("ride",rideData,32);
    if(u==uintptr_t(&TheCamera))return "camera"; if(u==uintptr_t(fakeRwCamera.data()))return "rwcam"; if(in(fakeFrame))return json::array({"frame",u-uintptr_t(fakeFrame.data())});
    if(u==uintptr_t(Scene.m_pRwCamera))return "scene_cam"; if(u==0xC40350)return "shadow_man"; if(u==0x97FB64)return "playback"; if(u==0x97FB70)return "record";
    if(u<0x10000)return json::array({"raw",u});
    return "other";
}

// --- stubs externos ---
inline void* findPedResult{}; inline void* findVehResult{};
inline std::array<CVector,8> coors{}; inline uint32 coorsCalls{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ trace.push_back({"find_ped",id}); return static_cast<CPlayerPed*>(findPedResult); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote){ trace.push_back({"find_veh",id,remote}); return static_cast<CVehicle*>(findVehResult); }
inline CVector __cdecl FakeFindPlayerCoors(int32 id){ trace.push_back({"find_coors",id}); return coors[coorsCalls++%8]; }
inline void __cdecl FakeWorldAdd(CEntity* e){ trace.push_back({"world_add",Id(e)}); }
inline void __cdecl FakeWorldRemove(CEntity* e){ trace.push_back({"world_remove",Id(e)}); }
inline bool losHit{}; inline CVector losPoint{};
inline bool __cdecl FakeLineOfSight(const CVector& o,const CVector& t,CColPoint& cp,CEntity*& e,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){
    trace.push_back({"los",F(o.x),F(o.y),F(o.z),F(t.x),F(t.y),F(t.z),b0,b1,b2,b3,b4,b5,b6,b7}); if(losHit)cp.m_vecPoint=losPoint; return losHit; }
inline void __cdecl FakeRequestModel(int32 m,int32 f){ trace.push_back({"request",m,f}); }
inline void __cdecl FakeLoadAll(bool p){ trace.push_back({"load_all",p}); }
inline void __cdecl FakeLoadScene(const CVector& v){ trace.push_back({"load_scene",F(v.x),F(v.y),F(v.z)}); }
inline uint32 levelResult{};
inline uint32 __cdecl FakeGetLevel(const CVector& v){ trace.push_back({"level",F(v.x),F(v.y),F(v.z)}); return levelResult; }
inline void __fastcall FakeRegisterRef(CEntity* s,void*,CEntity** r){ trace.push_back({"register_ref",Id(s),Id(r)}); }
inline void __fastcall FakeCleanUpRef(CEntity* s,void*,CEntity** r){ trace.push_back({"cleanup_ref",Id(s),Id(r)}); }
inline void __fastcall FakeRemoveWeapon(CPed* p,void*,int32 m){ trace.push_back({"remove_weapon",Id(p),m}); }
inline void __fastcall FakeAddWeapon(CPed* p,void*,int32 m){ trace.push_back({"add_weapon",Id(p),m}); }
inline void __fastcall FakeReApply(CPlayerPed* p,void*){ trace.push_back({"reapply",Id(p)}); }
inline void __fastcall FakeDeAllocate(void* p,void*){ trace.push_back({"dealloc",Id(p)}); }
inline void __fastcall FakeReturnShadow(void* s,void*,void* sh){ trace.push_back({"return_shadow",Id(s),Id(sh)}); }
inline void* __fastcall FakeDeleting(void* s,void*,uint32 flags){ trace.push_back({"delete",Id(s),flags}); return s; }
inline CRideAnimData* __fastcall FakeRideAnim(void* s,void*){ trace.push_back({"ride",Id(s)}); const auto o=uintptr_t(s)-uintptr_t(vehStore.data());
    return reinterpret_cast<CRideAnimData*>(rideData[o<vehStore.size() ? o/VEH_SIZE : 0].data()); }
inline void __fastcall FakeTrap(void* s,void*){ trace.push_back({"vtable_trap",Id(s)}); }
// animaciones
inline CAnimBlendAssociation* mainResult{}; inline CAnimBlendAssociation* secondResult{}; inline float mainBlend{}; inline CAnimBlendAssociation* partialResult{};
inline std::array<CAnimBlendAssociation*,3> blendResults{}; inline uint32 blendCalls{}; inline bool secondBlendNull{};
inline CAnimBlendAssociation* __cdecl FakeGetMain(RpClump* c,CAnimBlendAssociation** s,float* b){ trace.push_back({"get_main",Id(c)}); if(s)*s=secondResult; if(b)*b=mainBlend; return mainResult; }
inline CAnimBlendAssociation* __cdecl FakeGetPartial(RpClump* c){ trace.push_back({"get_partial",Id(c)}); return partialResult; }
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,AssocGroupId g,AnimationId id,float d){ trace.push_back({"blend",Id(c),int32(g),int32(id),F(d)}); const uint32 k=blendCalls++;
    if(k==1&&secondBlendNull&&d==100.f)return nullptr; return blendResults[k%3]; } // solo la segunda animación (delta 100) puede fallar: el EXE no comprueba las otras
inline void __cdecl FakeRemoveAssocs(RpClump* c,uint32 m){ trace.push_back({"remove_assocs",Id(c),m}); }
inline void __fastcall FakeSetCurrentTime(CAnimBlendAssociation* a,void*,float t){ trace.push_back({"set_time",Id(a),F(t)}); }
inline void __fastcall FakeSetBlend(CAnimBlendAssociation* a,void*,float b,float d){ trace.push_back({"set_blend",Id(a),F(b),F(d)}); }
// cámara y RW
inline void __fastcall FakeCalcDerived(void* c,void*,bool a,bool b){ trace.push_back({"derived",Id(c),a,b}); }
inline RwMatrix* __cdecl FakeMatrixUpdate(RwMatrix* m){ trace.push_back({"rw_matrix_update",Id(m)}); return m; }
inline RwFrame* __cdecl FakeFrameUpdate(RwFrame* f){ trace.push_back({"rw_frame_update",Id(f)}); return f; }
inline RwFrame* __cdecl FakeOrtho(RwFrame* f){ trace.push_back({"rw_ortho",Id(f)}); return f; }
inline RwCamera* __cdecl FakeNearClip(RwCamera* c,float z){ trace.push_back({"near_clip",Id(c),F(z)}); return c; }
inline void __fastcall FakeSetFadeColour(void* c,void*,uint8 r,uint8 g,uint8 b){ trace.push_back({"fade_colour",Id(c),r,g,b}); }
inline void __fastcall FakeFade(void* c,void*,float d,uint16 dir){ trace.push_back({"fade",Id(c),F(d),dir}); }
inline void __fastcall FakeProcessFade(void* c,void*){ trace.push_back({"process_fade",Id(c)}); }
inline void __fastcall FakeCamRestore(void* c,void*){ trace.push_back({"cam_restore",Id(c)}); }
inline void __cdecl FakeSetFOV(float f){ trace.push_back({"fov",F(f)}); }
// varios
template<int K> inline void __cdecl FakeVoid(){ trace.push_back({"call",K}); }
template<int K> inline void __fastcall FakeThisVoid(void* s,void*){ trace.push_back({"this_call",K,Id(s)}); }
inline void __cdecl FakeSetGameClock(uint8 h,uint8 m,uint8 d){ trace.push_back({"set_clock",h,m,d}); }
inline void __cdecl FakeDeleteTemp(CVector p,float r){ trace.push_back({"delete_temp",F(p.x),F(p.y),F(p.z),F(r)}); }
inline void __fastcall FakeSetEffects(void* s,void*,int8 v){ trace.push_back({"sfx_volume",v}); }
inline void __fastcall FakeSetMusic(void* s,void*,int8 v){ trace.push_back({"music_volume",v}); }
inline void __fastcall FakeRetune(void* s,void*,int8 id){ trace.push_back({"retune",id}); }
inline void __fastcall FakeStopRadio(void* s,void*,void* st,bool p){ trace.push_back({"stop_radio",Id(st),p}); }
inline int8 stationResult{};
inline int8 __fastcall FakeGetStation(void* s,void*){ trace.push_back({"get_station"}); return stationResult; }
inline void __fastcall FakeTurnOnRadio(void* s,void*){ trace.push_back({"radio_on",Id(s)}); }
inline uint32 keyMask{};
inline bool __fastcall FakeKeyJustDown(void* s,void*,int32 key){ trace.push_back({"key",key}); return (keyMask>>((key-1000)&31))&1; }
// texto y fuentes
inline const char fakeText[8]{'R',0,'E',0,0,0,0,0};
inline const GxtChar* __fastcall FakeTextGet(void* s,void*,const char* k){ trace.push_back({"text",std::string(k)}); return reinterpret_cast<const GxtChar*>(fakeText); }
inline void __cdecl FakeFontScale(float w,float h){ trace.push_back({"font_scale",F(w),F(h)}); }
inline void __cdecl FakeFontOrientation(uint8 a){ trace.push_back({"font_orientation",a}); }
inline void __cdecl FakeFontBackground(bool a,bool b){ trace.push_back({"font_background",a,b}); }
inline void __cdecl FakeFontCentre(float v){ trace.push_back({"font_centre",F(v)}); }
inline void __cdecl FakeFontProportional(bool v){ trace.push_back({"font_proportional",v}); }
inline void __cdecl FakeFontColor(uint32 c){ trace.push_back({"font_color",c}); }
inline void __cdecl FakeFontStyle(uint8 s){ trace.push_back({"font_style",s}); }
inline void __cdecl FakeFontPrint(float x,float y,const GxtChar* t){ trace.push_back({"font_print",F(x),F(y),t==reinterpret_cast<const GxtChar*>(fakeText)}); }
// ficheros
inline FILESTREAM openResult{}; inline uint32 readOk{}, readCalls{}; inline bool headerOk{};
inline void __cdecl FakeSetDir(const char* d){ trace.push_back({"set_dir",std::string(d)}); }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){ trace.push_back({"open",std::string(p),std::string(m)}); return openResult; }
inline FILESTREAM __cdecl FakeOpenWrite(const char* p){ trace.push_back({"open_write",std::string(p)}); return openResult; }
inline size_t __cdecl FakeFileRead(FILESTREAM f,void* b,size_t n){
    const uint32 k=readCalls++; trace.push_back({"read",uintptr_t(f),Id(b),n});
    if(k==0){ auto* s=static_cast<char*>(b); const char hdr[8]{'G','t','a','S','A','2','9',0}; std::memcpy(s,hdr,8); if(!headerOk)s[3]='X'; return 8; }
    if(k>readOk)return 0; auto* d=static_cast<uint8*>(b); for(int i=0;i<64;++i)d[i]=uint8(k*31+i); return n; }
inline size_t __cdecl FakeFileWrite(FILESTREAM f,const void* b,size_t n){
    const auto id=Id(b); trace.push_back({"write",uintptr_t(f),id.is_string()&&id=="other" ? json(std::string(static_cast<const char*>(b),strnlen(static_cast<const char*>(b),n))) : id,n}); return n; }
inline int32 __cdecl FakeClose(FILESTREAM f){ trace.push_back({"close",uintptr_t(f)}); return 0; }

// --- stubs de funciones de CReplay (solo dentro de su caso) ---
inline std::array<CPed*,4> dealResults{}; inline uint32 dealCalls{}; inline bool ppuAdvance{};
inline CPed* __cdecl StubDealWith(const tReplayPedHeaderBlock& p,bool load,tReplayClothesBlock* c){
    trace.push_back({"deal_with",Id(&p),load,c ? json(intptr_t(c)-intptr_t(&p)) : json(nullptr)}); return dealResults[dealCalls++%4]; }
inline void __cdecl StubProcessPedUpdate(CPed* ped,float t,CReplay::CAddressInReplayBuffer& a){
    trace.push_back({"process_ped_update",Id(ped),F(t),a.m_nOffset,Id(a.m_pBase)}); if(ppuAdvance)a.m_nOffset+=0x34; }
inline void __cdecl StubStorePedUpdate(CPed* ped,uint8 idx){
    trace.push_back({"store_ped_update",Id(ped),idx,CReplay::Record.m_nOffset}); auto* p=reinterpret_cast<uint8*>(CReplay::Record.m_pBase)+CReplay::Record.m_nOffset; p[0]=4; p[1]=idx; CReplay::Record.m_nOffset+=0x34; }
inline CPlayerPed* createResult{};
inline CPlayerPed* __cdecl StubCreatePlayer(){ trace.push_back({"create_player"}); return createResult; }
inline CVector firstFocus{};
inline void __cdecl StubFindFirst(CVector& o){ trace.push_back({"find_first"}); o=firstFocus; }
inline void __cdecl StubTrigger(eReplayCamMode m,CVector v,bool l){ trace.push_back({"trigger",int(m),F(v.x),F(v.y),F(v.z),l}); }
inline uint32 interpFinishAt{}, interpCalls{}; inline std::array<uint32,24> interpTimes{};
inline bool __cdecl StubInterp(CReplay::CAddressInReplayBuffer& a,float t,uint32* timer){
    const uint32 k=interpCalls++; trace.push_back({"interp",Id(&a),F(t),timer!=nullptr}); if(k>=interpFinishAt)return true; if(timer)*timer=interpTimes[k%24]; return false; }

// GetIsKeyboardKeyJustDown es protegido: se nombra a traves de una clase derivada (acceso protegido valido).
struct KeyAccess : CControllerConfigManager { static uintptr_t JustDownAddress(){ return std::bit_cast<uintptr_t>(&KeyAccess::GetIsKeyboardKeyJustDown); } };
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,47> fns{{
    {"CReplay","CanWeFindPoolIndexForPed",0x45C470,0,-1,256},{"CReplay","CanWeFindPoolIndexForVehicle",0x45C490,0,-1,256},
    {"CReplay","CreatePlayerPed",0x45D540,0,-1,512},{"CReplay","DealWithNewPedPacket",0x45CEA0,0,-1,512},
    {"CReplay","DisableReplays",0x45B150,0,-1,16},{"CReplay","Display",0x45C210,0,-1,256},
    {"CReplay","EmptyPedsAndVehiclePools_NoDestructors",0x45D390,0,-1,256},{"CReplay","EmptyReplayBuffer",0x45EC90,0,-1,128},
    {"CReplay","EnableReplays",0x45B160,0,-1,16},{"CReplay","FastForwardToTime",0x460350,0,-1,256},
    {"CReplay","FindFirstFocusCoordinate",0x45D6C0,0,-1,512},{"CReplay","FindPoolIndexForPed",0x45C450,0,-1,256},
    {"CReplay","FindPoolIndexForVehicle",0x45C460,0,-1,256},{"CReplay","FindSizeOfPacket",0x45C850,0,-1,256},
    {"CReplay","FinishPlayback",0x45F050,0,-1,256},{"CReplay","GoToNextBlock",0x45E2A0,0,-1,256},
    {"CReplay","Init",0x45E220,0,-1,128},{"CReplay","InitialisePedPoolConversionTable",0x45EF20,0,-1,512},
    {"CReplay","InitialisePoolConversionTables",0x45F370,0,-1,256},{"CReplay","InitialiseVehiclePoolConversionTable",0x45EFA0,0,-1,512},
    {"CReplay","IsThisPedUsedInRecording",0x45DDE0,0,-1,512},{"CReplay","IsThisVehicleUsedInRecording",0x45DE40,0,-1,512},
    {"CReplay","MarkEverythingAsNew",0x45D430,0,-1,128},{"CReplay","NumberFramesAvailableToPlay",0x45D670,0,-1,512},
    {"CReplay","PlayBackThisFrame",0x4604A0,0,-1,64},{"CReplay","PlayBackThisFrameInterpolation",0x45F380,0,-1,1024},
    {"CReplay","PlayReplayFromHD",0x460390,0,-1,256},{"CReplay","ProcessLookAroundCam",0x45D760,0,-1,1024},
    {"CReplay","ProcessPedUpdate",0x45CA70,0,-1,1024},{"CReplay","ProcessReplayCamera",0x45D060,0,-1,512},
    {"CReplay","RecordPedDeleted",0x45EC20,0,-1,256},{"CReplay","RecordThisFrame",0x45E300,0,-1,1024},
    {"CReplay","RecordVehicleDeleted",0x45EBB0,0,-1,256},{"CReplay","RestoreClothesDesc",0x45C7D0,0,-1,256},
    {"CReplay","RestorePlayerInfoVariables",0x45E1F0,0,-1,64},{"CReplay","RestoreStuffFromMem",0x45ECD0,0,-1,256},
    {"CReplay","RetrievePedAnimation",0x45B4D0,0,-1,1024},{"CReplay","SaveReplayToHD",0x45C340,0,-1,256},
    {"CReplay","ShouldStandardCameraBeProcessed",0x45C440,0,-1,64},{"CReplay","StoreClothesDesc",0x45C750,0,-1,256},
    {"CReplay","StorePedAnimation",0x45B170,0,-1,1024},{"CReplay","StorePedUpdate",0x45C940,0,-1,1024},
    {"CReplay","StorePlayerInfoVariables",0x45F020,0,-1,64},{"CReplay","StoreStuffInMem",0x45F180,0,-1,128},
    {"CReplay","StreamAllNecessaryCarsAndPeds",0x45D4B0,0,-1,256},{"CReplay","TriggerPlayback",0x4600F0,0,-1,256},
    {"CReplay","Update",0x460500,0,-1,512},
}};

struct Region { uintptr_t at; size_t n; const char* name; std::vector<uint8> saved; };
constexpr int32 MODEL_LO=280, MODEL_N=100; // entradas de ms_aInfoForModel que se fuerzan (cargado / no cargado)

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"replay47-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"replay47-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("replay47");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"replay47-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,47> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,47> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    const uintptr_t padDisable=uintptr_t(CPad::GetPad(0))+0x10E;
    std::vector<Region> regions{
        {0x97F670,0x518,"replay"},{0xA43088,0x1BC,"replay2"},{0x8A6160,1,"enabled"},{0xB6F028,0xD78,"camera"},{0xB7CD98,0x190,"player0"},
        {0xB7CB48,0x40,"timer"},{0xB70150,8,"clock"},{0xC81300,0x28,"weather"},{0xC0EC20,0x58,"population"},{0xB72900,0x20,"game"},
        {0xC7C700,0x350,"traces"},{0xB73418,0x40,"mouse"},{0xB6EC20,1,"first_person"},{0xC17044,8,"rsglobal"},{0xB5F852,1,"cutscene"},
        {padDisable,2,"pad"},{0xBA67A4,1,"menu"},{0x96A7CC,4,"enex"},{0xC3EFAB,1,"fade"},{0xB74490,8,"pools"},{0xB4EA34,4,"groups"},
        {0x8E4CC0+20*MODEL_LO,20*MODEL_N,"models"},{0xB71670,352,"gstring"},{0xB7CD74,1,"focus"}};
    for(auto& r:regions)r.saved=ReversingProbe::Bytes(reinterpret_cast<void*>(r.at),r.n);
    const std::vector<uint8> savedBuffers(BUFFERS,BUFFERS+8*BUFSZ);
    auto RestoreRegions=[&]{ for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.at),r.saved.data(),r.n); };
    auto& pStoredCam=*reinterpret_cast<uint8**>(0x97FAF8);

    fakeVt.fill(reinterpret_cast<void*>(&FakeTrap)); fakeVt[0]=reinterpret_cast<void*>(&FakeDeleting); fakeVt[47]=reinterpret_cast<void*>(&FakeRideAnim);
    pedPool.objects=pedStore.data(); pedPool.flags=pedFlags.data()+1; pedPool.size=NPED; pedPool.last=-1;
    vehPool.objects=vehStore.data(); vehPool.flags=vehFlags.data()+1; vehPool.size=NVEH; vehPool.last=-1;
    for(auto& c:clumps)c[0]=2;
    *reinterpret_cast<void**>(fakeRwCamera.data()+4)=fakeFrame.data();

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        // direcciones de la DLL resueltas antes de escribir ningún parche
        EntryPatch g1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), g1e(0x56E210,FakeFindPlayerPed);
        EntryPatch g2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), g2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch g3(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeFindPlayerCoors), g3e(0x56E010,FakeFindPlayerCoors);
        EntryPatch w1(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), w1e(0x563220,FakeWorldAdd);
        EntryPatch w2(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove), w2e(0x563280,FakeWorldRemove);
        EntryPatch w3(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),FakeLineOfSight), w3e(0x56BA00,FakeLineOfSight);
        EntryPatch s1(bit_cast<uintptr_t>(&CStreaming::RequestModel),FakeRequestModel), s1e(0x4087E0,FakeRequestModel);
        EntryPatch s2(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),FakeLoadAll), s2e(0x40EA10,FakeLoadAll);
        EntryPatch s3(bit_cast<uintptr_t>(&CStreaming::LoadScene),FakeLoadScene), s3e(0x40EB70,FakeLoadScene);
        EntryPatch s4(bit_cast<uintptr_t>(&CTheZones::GetLevelFromPosition),FakeGetLevel), s4e(0x572300,FakeGetLevel);
        EntryPatch s5(bit_cast<uintptr_t>(&CCollision::SortOutCollisionAfterLoad),FakeVoid<1>), s5e(0x411E30,FakeVoid<1>);
        EntryPatch r1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegisterRef), r1e(0x571B70,FakeRegisterRef);
        EntryPatch r2(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUpRef), r2e(0x571A00,FakeCleanUpRef);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::RemoveWeaponModel),FakeRemoveWeapon), p1e(0x5E3990,FakeRemoveWeapon);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::AddWeaponModel),FakeAddWeapon), p2e(0x5E5ED0,FakeAddWeapon);
        EntryPatch p3(bit_cast<uintptr_t>(&CPlayerPed::ReApplyMoveAnims),FakeReApply), p3e(0x609650,FakeReApply);
        EntryPatch p4(bit_cast<uintptr_t>(&CPlayerPedData::DeAllocateData),FakeDeAllocate), p4e(0x56F890,FakeDeAllocate);
        EntryPatch p5(bit_cast<uintptr_t>(&CRealTimeShadowManager::ReturnRealTimeShadow),FakeReturnShadow), p5e(0x705B30,FakeReturnShadow);
        EntryPatch a1(bit_cast<uintptr_t>(&RpAnimBlendClumpGetMainAssociation),FakeGetMain), a1e(0x4D6910,FakeGetMain);
        EntryPatch a2(bit_cast<uintptr_t>(&RpAnimBlendClumpGetMainPartialAssociation),FakeGetPartial), a2e(0x4D69A0,FakeGetPartial);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend), a3e(0x4D4610,FakeBlend);
        EntryPatch a4(bit_cast<uintptr_t>(&RpAnimBlendClumpRemoveAssociations),FakeRemoveAssocs), a4e(0x4D6820,FakeRemoveAssocs);
        EntryPatch a5(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime),FakeSetCurrentTime), a5e(0x4CEA80,FakeSetCurrentTime);
        EntryPatch a6(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetBlend),FakeSetBlend), a6e(0x4CEBA0,FakeSetBlend);
        EntryPatch c1(bit_cast<uintptr_t>(&CCamera::CalculateDerivedValues),FakeCalcDerived), c1e(0x5150E0,FakeCalcDerived);
        EntryPatch c2(0x7F18A0,FakeMatrixUpdate), c3(0x7F0910,FakeFrameUpdate), c4(0x7F1170,FakeOrtho), c5(0x7EE1D0,FakeNearClip); // los envoltorios RW de la DLL llaman al EXE
        EntryPatch c6(bit_cast<uintptr_t>(&CCamera::SetFadeColour),FakeSetFadeColour), c6e(0x50BF00,FakeSetFadeColour);
        EntryPatch c7(bit_cast<uintptr_t>(&CCamera::Fade),FakeFade), c7e(0x50AC20,FakeFade);
        EntryPatch c8(bit_cast<uintptr_t>(&CCamera::ProcessFade),FakeProcessFade), c8e(0x50B5D0,FakeProcessFade);
        EntryPatch c9(bit_cast<uintptr_t>(&CCamera::Restore),FakeCamRestore), c9e(0x50B930,FakeCamRestore);
        EntryPatch c10(bit_cast<uintptr_t>(&CDraw::SetFOV),FakeSetFOV), c10e(0x6FF410,FakeSetFOV);
        EntryPatch m1(bit_cast<uintptr_t>(&CClock::SetGameClock),FakeSetGameClock), m1e(0x52D150,FakeSetGameClock);
        EntryPatch m2(bit_cast<uintptr_t>(&CBulletTraces::Init),FakeVoid<2>), m2e(0x721D50,FakeVoid<2>);
        EntryPatch m3(bit_cast<uintptr_t>(&CSpecialFX::ReplayStarted),FakeVoid<3>), m3e(0x721D30,FakeVoid<3>);
        EntryPatch m4(bit_cast<uintptr_t>(&CObject::DeleteAllTempObjectsInArea),FakeDeleteTemp), m4e(0x5A1980,FakeDeleteTemp);
        EntryPatch m5(bit_cast<uintptr_t>(&CSkidmarks::Clear),FakeVoid<5>), m5e(0x720590,FakeVoid<5>);
        EntryPatch m6(bit_cast<uintptr_t>(&CWeaponEffects::ClearCrossHairs),FakeVoid<6>), m6e(0x742C80,FakeVoid<6>);
        EntryPatch m7(bit_cast<uintptr_t>(&CExplosion::ClearAllExplosions),FakeVoid<7>), m7e(0x736840,FakeVoid<7>);
        EntryPatch m8(bit_cast<uintptr_t>(&CPlaneBanners::Init),FakeVoid<8>), m8e(0x717820,FakeVoid<8>);
        EntryPatch m9(bit_cast<uintptr_t>(&CFireManager::DestroyAllFxSystems),FakeThisVoid<9>), m9e(0x539D10,FakeThisVoid<9>);
        EntryPatch m10(bit_cast<uintptr_t>(&CFireManager::CreateAllFxSystems),FakeThisVoid<10>), m10e(0x539D50,FakeThisVoid<10>);
        EntryPatch u1(bit_cast<uintptr_t>(&CAudioEngine::SetEffectsMasterVolume),FakeSetEffects), u1e(0x506E10,FakeSetEffects);
        EntryPatch u2(bit_cast<uintptr_t>(&CAudioEngine::SetMusicMasterVolume),FakeSetMusic), u2e(0x506DE0,FakeSetMusic);
        EntryPatch u3(bit_cast<uintptr_t>(&CAudioEngine::RetuneRadio),FakeRetune), u3e(0x507E10,FakeRetune);
        EntryPatch u4(bit_cast<uintptr_t>(&CAudioEngine::StopRadio),FakeStopRadio), u4e(0x506F70,FakeStopRadio);
        EntryPatch u5(bit_cast<uintptr_t>(&CAERadioTrackManager::GetCurrentRadioStationID),FakeGetStation), u5e(0x4E83F0,FakeGetStation);
        EntryPatch u6(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::TurnOnRadioForVehicle),FakeTurnOnRadio), u6e(0x4F5B20,FakeTurnOnRadio);
        EntryPatch k1(KeyAccess::JustDownAddress(),FakeKeyJustDown), k1e(0x52E450,FakeKeyJustDown);
        EntryPatch t1(bit_cast<uintptr_t>(&CText::Get),FakeTextGet), t1e(0x6A0050,FakeTextGet);
        EntryPatch f1(bit_cast<uintptr_t>(&CFont::SetScale),FakeFontScale), f1e(0x719380,FakeFontScale);
        EntryPatch f2(bit_cast<uintptr_t>(&CFont::SetOrientation),FakeFontOrientation), f2e(0x719610,FakeFontOrientation);
        EntryPatch f3(bit_cast<uintptr_t>(&CFont::SetBackground),FakeFontBackground), f3e(0x7195C0,FakeFontBackground);
        EntryPatch f4(bit_cast<uintptr_t>(&CFont::SetCentreSize),FakeFontCentre), f4e(0x7194E0,FakeFontCentre);
        EntryPatch f5(bit_cast<uintptr_t>(&CFont::SetProportional),FakeFontProportional), f5e(0x7195B0,FakeFontProportional);
        EntryPatch f6(bit_cast<uintptr_t>(&CFont::SetColor),FakeFontColor), f6e(0x719430,FakeFontColor);
        EntryPatch f7(bit_cast<uintptr_t>(&CFont::SetFontStyle),FakeFontStyle), f7e(0x719490,FakeFontStyle);
        EntryPatch f8(bit_cast<uintptr_t>(&CFont::PrintString),FakeFontPrint), f8e(0x71A700,FakeFontPrint);
        EntryPatch d1(bit_cast<uintptr_t>(&CFileMgr::SetDirMyDocuments),FakeVoid<11>), d1e(0x538860,FakeVoid<11>);
        EntryPatch d2(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), d2e(0x5387D0,FakeSetDir);
        EntryPatch d3(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), d3e(0x538900,FakeOpenFile);
        EntryPatch d4(bit_cast<uintptr_t>(&CFileMgr::OpenFileForWriting),FakeOpenWrite), d4e(0x538910,FakeOpenWrite);
        EntryPatch d5(bit_cast<uintptr_t>(&CFileMgr::Read),FakeFileRead), d5e(0x538950,FakeFileRead);
        EntryPatch d6(bit_cast<uintptr_t>(&CFileMgr::Write),FakeFileWrite), d6e(0x538970,FakeFileWrite);
        EntryPatch d7(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeClose), d7e(0x5389D0,FakeClose);

        // destinos de los stubs de CReplay (se aplican dentro de cada caso)
        const uintptr_t dllDealWith=bit_cast<uintptr_t>(&CReplay::DealWithNewPedPacket), dllPPU=bit_cast<uintptr_t>(&CReplay::ProcessPedUpdate),
            dllStorePedUpdate=bit_cast<uintptr_t>(&CReplay::StorePedUpdate), dllCreatePlayer=bit_cast<uintptr_t>(&CReplay::CreatePlayerPed),
            dllFindFirst=bit_cast<uintptr_t>(&CReplay::FindFirstFocusCoordinate), dllTrigger=bit_cast<uintptr_t>(&CReplay::TriggerPlayback),
            dllInterp=bit_cast<uintptr_t>(&CReplay::PlayBackThisFrameInterpolation), dllRestoreStuff=bit_cast<uintptr_t>(&CReplay::RestoreStuffFromMem),
            dllInit=bit_cast<uintptr_t>(&CReplay::Init), dllMark=bit_cast<uintptr_t>(&CReplay::MarkEverythingAsNew), dllStoreStuff=bit_cast<uintptr_t>(&CReplay::StoreStuffInMem),
            dllInitPed=bit_cast<uintptr_t>(&CReplay::InitialisePedPoolConversionTable), dllInitVeh=bit_cast<uintptr_t>(&CReplay::InitialiseVehiclePoolConversionTable),
            dllEmptyPools=bit_cast<uintptr_t>(&CReplay::EmptyPedsAndVehiclePools_NoDestructors), dllStream=bit_cast<uintptr_t>(&CReplay::StreamAllNecessaryCarsAndPeds),
            dllPlayBack=bit_cast<uintptr_t>(&CReplay::PlayBackThisFrame), dllRecord=bit_cast<uintptr_t>(&CReplay::RecordThisFrame), dllFinish=bit_cast<uintptr_t>(&CReplay::FinishPlayback),
            dllSave=bit_cast<uintptr_t>(&CReplay::SaveReplayToHD), dllPlayHD=bit_cast<uintptr_t>(&CReplay::PlayReplayFromHD), dllCamera=bit_cast<uintptr_t>(&CReplay::ProcessReplayCamera),
            dllLookAround=bit_cast<uintptr_t>(&CReplay::ProcessLookAroundCam);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7E91u; if(!stt)stt=1; trace=json::array();
                    coorsCalls=0; blendCalls=0; secondBlendNull=false; dealCalls=0; interpCalls=0; readCalls=0; ppuAdvance=false;
                    findPedResult=nullptr; findVehResult=nullptr; losHit=false;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    std::vector<std::unique_ptr<EntryPatch>> local;
                    auto Stub=[&](uintptr_t exeAt,uintptr_t dllAt,auto fp){ local.push_back(std::make_unique<EntryPatch>(exeAt,fp)); local.push_back(std::make_unique<EntryPatch>(dllAt,fp)); };

                    // --- fixture común ---
                    *reinterpret_cast<RawPool**>(0xB74490)=&pedPool; *reinterpret_cast<RawPool**>(0xB74494)=&vehPool;
                    pedFlags[0]=0x80; vehFlags[0]=0x80; // guarda del índice -1: libre
                    for(int32 k=0;k<NPED;++k)pedFlags[1+k]=uint8(0x80|(k&0x7F)); for(int32 k=0;k<NVEH;++k)vehFlags[1+k]=uint8(0x80|(k&0x7F));
                    auto RandMat=[&](CMatrix& m){ auto* p=reinterpret_cast<float*>(&m); for(int k=0;k<16;++k)p[k]=uni(-1.5f,1.5f); p[12]=uni(-3000.f,3000.f); p[13]=uni(-3000.f,3000.f); p[14]=uni(-50.f,300.f);
                        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(&m)+0x40)=0; *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(&m)+0x44)=0; };
                    std::vector<int32> usedPeds, usedVehs;
                    auto AddPed=[&](int32 k){ pedFlags[1+k]=uint8(k&0x7F); usedPeds.push_back(k); auto* p=Ped(k); for(int32 b=0;b<PED_SIZE;b+=4)*reinterpret_cast<uint32*>(p+b)=next();
                        *reinterpret_cast<void**>(p)=fakeVt.data(); RandMat(pedMats[k]); *reinterpret_cast<CMatrix**>(p+0x14)=&pedMats[k]; *reinterpret_cast<void**>(p+0x18)=next()%5 ? clumps[next()%4].data() : nullptr;
                        *reinterpret_cast<float*>(p+0x558)=uni(-3.2f,3.2f); *reinterpret_cast<float*>(p+0x55C)=uni(-3.2f,3.2f); *reinterpret_cast<float*>(p+0x12C)=uni(0.f,2.6f);
                        *reinterpret_cast<int32*>(p+0x4D4)=int32(next()%120); *reinterpret_cast<int16*>(p+0x740)=next()%3 ? int16(-1) : int16(330+next()%40);
                        *reinterpret_cast<uint32*>(p+0x598)=next()%3 ? 0u : next()%20; *reinterpret_cast<int16*>(p+0x22)=int16(next()%4 ? next()%320 : 290+next()%12);
                        *reinterpret_cast<void**>(p+0x480)=next()%2 ? playerDatas[next()%4].data() : nullptr; *reinterpret_cast<void**>(p+0x134)=next()%3 ? reinterpret_cast<void*>(uintptr_t(0x5000+next()%64*4)) : nullptr;
                        *reinterpret_cast<void**>(p+0x58C)=nullptr; return p; };
                    auto AddVeh=[&](int32 k){ vehFlags[1+k]=uint8(k&0x7F); usedVehs.push_back(k); auto* v=Veh(k); for(int32 b=0;b<VEH_SIZE;b+=4)*reinterpret_cast<uint32*>(v+b)=next();
                        *reinterpret_cast<void**>(v)=fakeVt.data(); RandMat(vehMats[k]); *reinterpret_cast<CMatrix**>(v+0x14)=&vehMats[k]; *reinterpret_cast<void**>(v+0x18)=next()%5 ? clumps[next()%4].data() : nullptr;
                        static const uint32 subs[]{0,1,2,3,4,5,6,9,10,11,7,8};
                        const uint32 sub=subs[next()%12]; *reinterpret_cast<uint32*>(v+0x594)=sub; *reinterpret_cast<uint32*>(v+0x590)=sub==0||sub==1||sub==2||sub==11 ? 0u : sub==9||sub==10 ? 1u : 2u+next()%3;
                        *reinterpret_cast<float*>(v+0x4C0)=next()%6==0 ? uni(1000.f,3000.f) : uni(-100.f,1000.f); *reinterpret_cast<float*>(v+0x49C)=uni(-1.f,1.f);
                        for(int c=0;c<3;++c)*reinterpret_cast<float*>(v+0x44+4*c)=next()%6==0 ? uni(-8.f,8.f) : uni(-0.02f,0.02f);
                        *reinterpret_cast<uint16*>(v+0x22)=next()%6==0 ? uint16(432) : uint16(400+next()%212); *reinterpret_cast<float*>(v+0x494)=uni(-0.7f,0.7f); *reinterpret_cast<float*>(v+0x94C)=uni(-3.f,3.f);
                        for(int c=0;c<4;++c){ *reinterpret_cast<float*>(v+0x7D4+4*c)=uni(0.f,1.f); *reinterpret_cast<float*>(v+0x828+4*c)=uni(-3.f,3.f); }
                        *reinterpret_cast<float*>(v+0x5F4)=uni(-1.2f,1.2f); *reinterpret_cast<float*>(v+0x60C)=uni(-1.2f,1.2f); for(int c=0;c<6;++c)v[0x5A9+c]=uint8(next()%5);
                        *reinterpret_cast<float*>(v+0x84C)=uni(0.f,1.f); *reinterpret_cast<float*>(v+0x9C4)=uni(0.f,1.f); *reinterpret_cast<float*>(v+0x9C8)=uni(0.f,1.f);
                        *reinterpret_cast<float*>(v+0x5A4)=uni(-1.f,1.f); *reinterpret_cast<float*>(v+0x5A8)=uni(0.f,3000.f); *reinterpret_cast<float*>(v+0x5AC)=uni(0.f,30.f);
                        *reinterpret_cast<void**>(v+0x5D0)=nullptr; *reinterpret_cast<void**>(v+0x5D4)=nullptr;
                        for(auto& r:rideData[k])r=uni(-1.f,1.f); return v; };
                    auto RandomPools=[&](int maxPeds,int maxVehs){ const int np=int(next()%(maxPeds+1)), nv=int(next()%(maxVehs+1));
                        for(int k=0;k<nv;++k){ const int32 s=int32(next()%NVEH); if(vehFlags[1+s]&0x80)AddVeh(s); }
                        for(int k=0;k<np;++k){ const int32 s=int32(next()%NPED); if(pedFlags[1+s]&0x80)AddPed(s); } };
                    for(auto& b:assocs)for(auto& x:b)x=uint8(next()); for(auto& b:entities)for(auto& x:b)x=uint8(next());
                    for(auto& b:playerDatas){ for(auto& x:b)x=uint8(next()); *reinterpret_cast<void**>(b.data()+4)=clothesDescs[&b-playerDatas.data()].data(); }
                    for(auto& d:clothesDescs){ for(int k=0;k<0x70;++k)d[k]=uint8(next()); *reinterpret_cast<float*>(d.data()+0x70)=uni(-2000.f,2000.f); *reinterpret_cast<float*>(d.data()+0x74)=uni(-40000.f,40000.f); }
                    for(auto& x:fakeFrame)x=uint8(next());
                    // ventanas de los buffers
                    for(uint32 s=0;s<8;++s){ for(uint32 k=0;k<WIN;k+=4){ *reinterpret_cast<uint32*>(Buf(s)+k)=next(); *reinterpret_cast<uint32*>(Buf(s)+BUFSZ-WIN+k)=next(); } }
                    auto Models=[&](bool any){ for(int32 m=0;m<MODEL_N;++m)reinterpret_cast<uint8*>(0x8E4CC0)[20*(MODEL_LO+m)+0x10]=uint8(any ? next()%2 : 0); };
                    Models(true);
                    auto& status=*reinterpret_cast<std::array<uint8,8>*>(0x97FB7C);
                    auto& record=CReplay::Record; auto& playback=CReplay::Playback;
                    auto* pedConv=reinterpret_cast<int32*>(0x97F838); auto* vehConv=reinterpret_cast<int32*>(0x97F680);
                    auto RandomConv=[&]{ for(int32 k=0;k<140;++k)pedConv[k]=next()%4==0 ? -1 : int32(next()%NPED); for(int32 k=0;k<110;++k)vehConv[k]=next()%4==0 ? -1 : int32(next()%NVEH); };
                    auto& mode=*reinterpret_cast<int8*>(0xA43088);
                    auto SaneFloats=[&](uint8* p,size_t n,float lo,float hi){ for(size_t k=0;k+4<=n;k+=4)*reinterpret_cast<float*>(p+k)=uni(lo,hi); };
                    bool vehSplit=false; // reproducción: vehículos 0..54 (slot libre) y borrados 55..109
                    // generador de paquetes bien formados
                    auto Packet=[&](uint8* p,uint8 type,int refMax){
                        p[0]=type;
                        switch(type){
                        case 1: case 2: case 15: case 16: case 17: case 18:
                            p[1]=uint8(next()%(vehSplit ? 55 : refMax)); SaneFloats(p+4,0x14,-100.f,100.f); *reinterpret_cast<int16*>(p+0x1A)=int16(MODEL_LO+next()%MODEL_N); p[0x30]=uint8(next()%3==0 ? 5 : next()%3); break;
                        case 3: p[1]=uint8(next()%refMax); *reinterpret_cast<int16*>(p+2)=int16(next()%6==0 ? next()%2000 : MODEL_LO+next()%MODEL_N); p[4]=uint8(next()%3==0 ? 0 : next()%20); break;
                        case 4: p[1]=uint8(next()%refMax); p[3]=uint8(next()%3 ? 0 : 1+next()%refMax); break;
                        case 5: SaneFloats(p+4,0x48,-2.f,2.f); *reinterpret_cast<uint32*>(p+0x44)=0; *reinterpret_cast<uint32*>(p+0x48)=0; SaneFloats(p+0x4C,12,-3000.f,3000.f); break;
                        case 7: *reinterpret_cast<float*>(p+4)=uni(0.f,1.f); break;
                        case 10: p[3]=uint8(next()%16); SaneFloats(p+4,0x18,-3000.f,3000.f); break;
                        case 12: *reinterpret_cast<float*>(p+8)=uni(0.f,1.f); break;
                        case 13: *reinterpret_cast<int16*>(p+2)=int16(vehSplit ? 55+next()%55 : next()%refMax); break;
                        case 14: *reinterpret_cast<int16*>(p+2)=int16(next()%refMax); break;
                        default: break; }
                        return PacketSize(type); };
                    auto Stream=[&](uint32 s,const std::vector<uint8>& types,int count,int refMax,uint8 last=0){ uint8* b=Buf(s); uint32 off=0;
                        for(int k=0;k<count;++k){ const uint8 t=types[next()%types.size()]; const uint32 sz=PacketSize(t); if(off+sz+0x80>WIN)break; off+=Packet(b+off,t,refMax); }
                        if(last!=0)off+=Packet(b+off,last,refMax); b[off]=0; return off; };
                    const std::vector<uint8> allTypes{1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19};
                    auto RandomStatus=[&](bool atLeastOne){ for(auto& st:status)st=uint8(next()%3); if(atLeastOne&&std::ranges::all_of(status,[](uint8 v){return v==0;}))status[next()%8]=1; };

                    if(nm=="CanWeFindPoolIndexForPed"||nm=="CanWeFindPoolIndexForVehicle"||nm=="FindPoolIndexForPed"||nm=="FindPoolIndexForVehicle") {
                        RandomConv(); const int32 idx=next()%8==0 ? int32(next()%300)-5 : int32(next()%(nm.find("Ped")!=std::string::npos ? 140 : 110));
                        if(nm.rfind("CanWe",0)==0)ret=reinterpret_cast<bool(__cdecl*)(int32)>(a)(idx); else ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(idx);
                    }
                    else if(nm=="FindSizeOfPacket") ret=reinterpret_cast<uint32(__cdecl*)(eReplayPacket)>(a)(eReplayPacket(uint8(i)));
                    else if(nm=="DisableReplays"||nm=="EnableReplays") { *reinterpret_cast<uint8*>(0x8A6160)=uint8(next()%2); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="ShouldStandardCameraBeProcessed") { mode=int8(next()%4); ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                    else if(nm=="Display") {
                        mode=int8(next()%3==0 ? 0 : next()%4==0 ? 2 : 1); *reinterpret_cast<uint32*>(0xA43240)=next()%3 ? next()%0x10000 : 0xFFFF;
                        RsGlobal.maximumWidth=int32(320+next()%3000); RsGlobal.maximumHeight=int32(200+next()%2000);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="StoreClothesDesc"||nm=="RestoreClothesDesc") {
                        auto& d=clothesDescs[0]; alignas(4) std::array<uint8,0x78> pk{}; for(auto& x:pk)x=uint8(next());
                        if(next()%4==0)*reinterpret_cast<float*>(d.data()+0x70)=uni(-1e9f,1e9f);
                        if(nm=="StoreClothesDesc")reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(d.data(),pk.data()); else reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(d.data(),pk.data());
                        out["packet"]=ReversingProbe::Bytes(pk.data(),pk.size()); out["desc"]=ReversingProbe::Bytes(d.data(),d.size());
                    }
                    else if(nm=="StorePedAnimation"||nm=="RetrievePedAnimation") {
                        auto* p=AddPed(int32(next()%NPED)); *reinterpret_cast<void**>(p+0x18)=clumps[next()%4].data();
                        for(auto& asc:assocs){ *reinterpret_cast<int16*>(asc.data()+0x2C)=int16(next()%5==0 ? -int32(next()%3) : int32(next()%400)); asc[0x0E]=uint8(next());
                            *reinterpret_cast<float*>(asc.data()+0x20)=next()%6==0 ? uni(-2.f,8.f) : uni(0.f,4.f); *reinterpret_cast<float*>(asc.data()+0x24)=next()%6==0 ? uni(-1.f,5.f) : uni(0.f,3.f);
                            *reinterpret_cast<float*>(asc.data()+0x18)=next()%6==0 ? uni(-1.f,3.f) : uni(0.f,2.f); }
                        mainResult=next()%4 ? reinterpret_cast<CAnimBlendAssociation*>(assocs[0].data()) : nullptr; secondResult=next()%3 ? reinterpret_cast<CAnimBlendAssociation*>(assocs[1].data()) : nullptr;
                        partialResult=next()%3 ? reinterpret_cast<CAnimBlendAssociation*>(assocs[2].data()) : nullptr; mainBlend=next()%6==0 ? uni(-1.f,3.f) : uni(0.f,2.f);
                        blendResults={reinterpret_cast<CAnimBlendAssociation*>(assocs[0].data()),reinterpret_cast<CAnimBlendAssociation*>(assocs[1].data()),reinterpret_cast<CAnimBlendAssociation*>(assocs[2].data())};
                        secondBlendNull=next()%4==0;
                        for(auto& g:fakeGroups){ *reinterpret_cast<void**>(g.data())=next()%4 ? fakeBlocks[next()%8].data() : nullptr; *reinterpret_cast<int32*>(g.data()+8)=int32(next()%6)-1; }
                        for(auto& b:fakeBlocks)b[0x10]=uint8(next()%2);
                        *reinterpret_cast<void**>(0xB4EA34)=fakeGroups.data();
                        alignas(4) std::array<uint8,0x12> st{}; for(auto& x:st)x=uint8(next());
                        if(nm=="RetrievePedAnimation"){ auto* s=reinterpret_cast<AnimationState*>(st.data());
                            for(int k=0;k<3;++k){ if(next()%3==0)s[k].m_nAnimId=int16(next()%5); if(next()%5==0)s[k].m_nAnimId=0; if(next()%5==0)s[k].m_nGroupId1=0; } }
                        reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(p,st.data());
                        out["state"]=ReversingProbe::Bytes(st.data(),st.size()); out["assocs"]=Fnv(assocs.data(),sizeof(assocs));
                    }
                    else if(nm=="StorePedUpdate"||nm=="ProcessPedUpdate") {
                        RandomPools(1,6); RandomConv();
                        const int32 k=int32(next()%NPED); auto* p=AddPed(k); *reinterpret_cast<void**>(p+0x18)=clumps[0].data();
                        if(!usedVehs.empty()&&next()%2)*reinterpret_cast<void**>(p+0x58C)=Veh(usedVehs[next()%usedVehs.size()]); else if(next()%3==0)*reinterpret_cast<void**>(p+0x58C)=Veh(int32(next()%NVEH));
                        findPedResult=next()%2 ? static_cast<void*>(p) : static_cast<void*>(entities[0].data()); *reinterpret_cast<uint8*>(0xB6EC20)=uint8(next()%2);
                        mainResult=next()%2 ? reinterpret_cast<CAnimBlendAssociation*>(assocs[0].data()) : nullptr; secondResult=nullptr; partialResult=nullptr; mainBlend=0.5f;
                        for(auto& asc:assocs){ *reinterpret_cast<float*>(asc.data()+0x20)=uni(0.f,4.f); *reinterpret_cast<float*>(asc.data()+0x24)=uni(0.f,3.f); *reinterpret_cast<float*>(asc.data()+0x18)=uni(0.f,2.f); }
                        blendResults={reinterpret_cast<CAnimBlendAssociation*>(assocs[0].data()),reinterpret_cast<CAnimBlendAssociation*>(assocs[1].data()),reinterpret_cast<CAnimBlendAssociation*>(assocs[2].data())};
                        for(auto& g:fakeGroups){ *reinterpret_cast<void**>(g.data())=next()%3 ? fakeBlocks[next()%8].data() : nullptr; *reinterpret_cast<int32*>(g.data()+8)=int32(next()%4); }
                        for(auto& b:fakeBlocks)b[0x10]=uint8(next()%2); *reinterpret_cast<void**>(0xB4EA34)=fakeGroups.data();
                        const uint32 slot=next()%8; const uint32 off=next()%4==0 ? BUFSZ-WIN+next()%(WIN-0x40) : next()%(WIN-0x40);
                        if(nm=="StorePedUpdate"){ record.m_pBase=reinterpret_cast<CReplay::tReplayBuffer*>(Buf(slot)); record.m_nOffset=off; record.m_bSlot=uint8(slot);
                            reinterpret_cast<void(__cdecl*)(CPed*,uint8)>(a)(reinterpret_cast<CPed*>(p),uint8(next())); }
                        else { uint8* pk=Buf(slot)+off; Packet(pk,4,110); pk[2]=uint8(next()); SaneFloats(pk+0x18,0x14,-50.f,50.f);
                            static const int16 wms[]{-1,0,300,330,345,350,355,379}; *reinterpret_cast<int16*>(pk+0x2C)=wms[next()%8];
                            *reinterpret_cast<int16*>(pk+0x2E)=next()%3==0 ? *reinterpret_cast<int16*>(p+0x4D4) : int16(next()%4==0 ? -int32(next()%3) : int32(next()%120));
                            for(int32 m=0;m<MODEL_N;++m)reinterpret_cast<uint8*>(0x8E4CC0)[20*(MODEL_LO+m)+0x10]=uint8(next()%2);
                            CReplay::CAddressInReplayBuffer addr; addr.m_nOffset=off; addr.m_pBase=reinterpret_cast<CReplay::tReplayBuffer*>(Buf(slot)); addr.m_bSlot=uint8(slot);
                            const float t=next()%3==0 ? 1.f : uni(0.f,1.f);
                            reinterpret_cast<void(__cdecl*)(CPed*,float,CReplay::CAddressInReplayBuffer&)>(a)(next()%10==0 ? nullptr : reinterpret_cast<CPed*>(p),t,addr);
                            out["address"]={addr.m_nOffset,Id(addr.m_pBase),addr.m_bSlot}; }
                        out["mat"]=ReversingProbe::Bytes(&pedMats[k],0x40);
                    }
                    else if(nm=="RecordPedDeleted"||nm=="RecordVehicleDeleted"||nm=="GoToNextBlock") {
                        RandomPools(4,4); mode=int8(next()%4==0 ? next()%3 : 0); const uint32 slot=next()%8; RandomStatus(false);
                        record.m_pBase=reinterpret_cast<CReplay::tReplayBuffer*>(Buf(slot)); record.m_bSlot=uint8(slot); record.m_nOffset=next()%3==0 ? 99984-next()%16+next()%8 : next()%(WIN-8);
                        if(nm=="GoToNextBlock")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="RecordPedDeleted"){ const int32 k=usedPeds.empty() ? int32(next()%NPED) : usedPeds[next()%usedPeds.size()]; reinterpret_cast<void(__cdecl*)(void*)>(a)(Ped(k)); }
                        else { const int32 k=usedVehs.empty() ? int32(next()%NVEH) : usedVehs[next()%usedVehs.size()]; reinterpret_cast<void(__cdecl*)(void*)>(a)(Veh(k)); }
                    }
                    else if(nm=="MarkEverythingAsNew"||nm=="EmptyReplayBuffer"||nm=="Init"||nm=="EmptyPedsAndVehiclePools_NoDestructors") {
                        RandomPools(12,12); mode=int8(next()%3==0 ? 1 : next()%4==0 ? 2 : 0); RandomStatus(false);
                        record.m_nOffset=next(); record.m_bSlot=uint8(next()); record.m_pBase=reinterpret_cast<CReplay::tReplayBuffer*>(Buf(next()%8));
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="IsThisPedUsedInRecording"||nm=="IsThisVehicleUsedInRecording"||nm=="FindFirstFocusCoordinate"||nm=="NumberFramesAvailableToPlay"
                            ||nm=="StreamAllNecessaryCarsAndPeds"||nm=="InitialisePedPoolConversionTable"||nm=="InitialiseVehiclePoolConversionTable"||nm=="InitialisePoolConversionTables") {
                        const bool vehInit=nm=="InitialiseVehiclePoolConversionTable"||nm=="InitialisePoolConversionTables";
                        RandomStatus(false); const int refMax=vehInit ? 100 : int(next()%2 ? 160 : 40);
                        for(uint32 s=0;s<8;++s)Stream(s,allTypes,int(next()%40),refMax);
                        if(nm.rfind("Initialise",0)==0){ RandomPools(30,30); RandomConv(); }
                        if(nm=="IsThisPedUsedInRecording"||nm=="IsThisVehicleUsedInRecording")ret=reinterpret_cast<bool(__cdecl*)(int32)>(a)(int32(next()%(refMax+4))-2);
                        else if(nm=="FindFirstFocusCoordinate"){ CVector o{uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)}; reinterpret_cast<void(__cdecl*)(CVector&)>(a)(o); ret={F(o.x),F(o.y),F(o.z)}; }
                        else if(nm=="NumberFramesAvailableToPlay")ret=reinterpret_cast<uint32(__cdecl*)()>(a)();
                        else reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="CreatePlayerPed") {
                        RandomPools(10,0); RandomConv(); RandomStatus(false); playback.m_bSlot=uint8(next()%8);
                        for(uint32 s=0;s<8;++s)Stream(s,{3,4,4,4,6,8,9,19},int(next()%30),40);
                        for(auto& r:dealResults)r=next()%3==0||usedPeds.empty() ? nullptr : reinterpret_cast<CPed*>(Ped(usedPeds[next()%usedPeds.size()]));
                        if(!usedPeds.empty())for(int32 k=0;k<40;++k)if(next()%2)pedConv[k]=usedPeds[next()%usedPeds.size()];
                        Stub(0x45CEA0,dllDealWith,StubDealWith); Stub(0x45CA70,dllPPU,StubProcessPedUpdate);
                        ret=Id(reinterpret_cast<CPlayerPed*(__cdecl*)()>(a)());
                    }
                    else if(nm=="DealWithNewPedPacket") {
                        RandomPools(20,0); RandomConv(); alignas(4) std::array<uint8,8> pk{}; for(auto& x:pk)x=uint8(next()); pk[0]=3; pk[1]=uint8(next()%140);
                        const bool load=next()%2; *reinterpret_cast<int16*>(pk.data()+2)=int16(MODEL_LO+next()%MODEL_N); pk[4]=uint8(next()%3==0 ? 0 : 1+next()%20);
                        auto* info=reinterpret_cast<uint8*>(0x8E4CC0)+20*(*reinterpret_cast<int16*>(pk.data()+2))+0x10;
                        const int32 conv=pedConv[pk[1]]; const bool slotUsed=conv>=-1&&conv<NPED&&!(pedFlags[1+conv]&0x80);
                        // fuera: crear peatones reales (jugador con modelo cargado o civil sin loadModel)
                        if(!slotUsed&&*info==1&&(pk[4]==0||!load))*info=0;
                        alignas(4) std::array<uint8,0x78> clothes{}; for(auto& x:clothes)x=uint8(next());
                        ret=Id(reinterpret_cast<CPed*(__cdecl*)(void*,bool,void*)>(a)(pk.data(),load,next()%2 ? clothes.data() : nullptr));
                    }
                    else if(nm=="SaveReplayToHD") {
                        for(auto& st:status)st=uint8(next()%2); status[next()%8]=2; static const uintptr_t handles[]{0x1234,0,0x80001000,0x7FFF0000};
                        openResult=reinterpret_cast<FILESTREAM>(handles[next()%4]); reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="PlayReplayFromHD") {
                        openResult=next()%5==0 ? nullptr : reinterpret_cast<FILESTREAM>(uintptr_t(0x4321)); headerOk=next()%4!=0; readOk=next()%9; RandomStatus(false);
                        Stub(0x4600F0,dllTrigger,StubTrigger); Stub(0x45D4B0,dllStream,FakeVoid<20>);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="ProcessReplayCamera"||nm=="ProcessLookAroundCam") {
                        *reinterpret_cast<void**>(0xB6F97C)=fakeRwCamera.data();
                        RandMat(camPlaceableMat); *reinterpret_cast<CMatrix**>(0xB6F03C)=next()%3 ? &camPlaceableMat : nullptr; SaneFloats(reinterpret_cast<uint8*>(0xB6F02C),12,-3000.f,3000.f);
                        SaneFloats(reinterpret_cast<uint8*>(0x97F670),12,-3000.f,3000.f); SaneFloats(reinterpret_cast<uint8*>(0x97FB24),12,-3000.f,3000.f);
                        if(next()%10==0)std::memcpy(reinterpret_cast<void*>(0x97FB24),reinterpret_cast<void*>(0x97F670),12);
                        SaneFloats(reinterpret_cast<uint8*>(0xB6F99C),0x40,-2.f,2.f);
                        if(nm=="ProcessReplayCamera"){ *reinterpret_cast<uint8*>(0x97FB30)=uint8(next()%4); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else { auto& ms=CPad::NewMouseControllerState; ms.isMouseLeftButtonPressed=uint8(next()%2 ? 0x80 : 0); ms.isMouseRightButtonPressed=uint8(next()%2 ? 0x80 : 0);
                            ms.m_AmountMoved.x=next()%3==0 ? 0.f : uni(-30.f,30.f); ms.m_AmountMoved.y=next()%3==0 ? uni(-2.f,2.f) : uni(-30.f,30.f);
                            *reinterpret_cast<uint8*>(0x97FAE0)=uint8(next()%5!=0); *reinterpret_cast<int32*>(0x97FAD0)=next()%2 ? 0 : int32(next()%70); *reinterpret_cast<uint8*>(0x97F66D)=uint8(next()%5==0);
                            *reinterpret_cast<float*>(0x97FAD4)=uni(2.f,16.f); *reinterpret_cast<float*>(0x97FAD8)=uni(0.f,1.6f); *reinterpret_cast<float*>(0x97FADC)=uni(-7.f,7.f);
                            const CVector cam=TheCamera.GetPosition(); for(auto& c:coors)c=next()%6==0 ? cam+CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)} : cam+CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(-5.f,5.f)};
                            losHit=next()%2; losPoint=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-50.f,300.f)};
                            findPedResult=next()%3 ? entities[0].data() : nullptr; findVehResult=next()%2 ? entities[1].data() : nullptr;
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["frame"]=ReversingProbe::Bytes(fakeFrame.data(),fakeFrame.size()); out["entities"]=ReversingProbe::Bytes(entities.data(),sizeof(entities));
                    }
                    else if(nm=="StorePlayerInfoVariables"||nm=="RestorePlayerInfoVariables") {
                        auto* src=reinterpret_cast<uint8*>(nm=="StorePlayerInfoVariables" ? 0xB7CD98 : 0xA430B0); auto* dst=reinterpret_cast<uint8*>(nm=="StorePlayerInfoVariables" ? 0xA430B0 : 0xB7CD98);
                        std::memcpy(dst,src,0x190); static const int offs[]{0,4,8,0xC,0x4C,0x50,0xB0,0xB4,0xB8,0xBC,0x184};
                        for(int o:offs)if(next()%2)*reinterpret_cast<uint32*>(src+o)=next();
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="StoreStuffInMem") {
                        for(int k=4;k<0x40;k+=4)*reinterpret_cast<uint32*>(0xB7CB48+k)=next(); *reinterpret_cast<uint16*>(0xB70152)=uint16(next());
                        for(uintptr_t fa:{0xB7CB58u,0xB7CB5Cu,0xB7CB64u})*reinterpret_cast<float*>(fa)=uni(0.f,3.f); // floats sin NaN
                        *reinterpret_cast<int16*>(0xC81320)=int16(next()%20); *reinterpret_cast<int16*>(0xC8131C)=int16(next()%20); *reinterpret_cast<float*>(0xC8130C)=uni(0.f,1.f);
                        for(int k=0;k<0x50;k+=4)*reinterpret_cast<uint32*>(0xC0EC24+k)=next()%100;
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["stored_cam"]=pStoredCam ? json(std::memcmp(pStoredCam,&TheCamera,0xD78)==0) : json(nullptr);
                        if(pStoredCam){ ::operator delete(pStoredCam); pStoredCam=nullptr; }
                    }
                    else if(nm=="RestoreStuffFromMem") {
                        RandomPools(10,10);
                        for(int32 k:usedPeds){ auto& fl=*reinterpret_cast<uint32*>(Ped(k)+0x478); fl=next()%2 ? fl|0x200000u : fl&~0x200000u; }
                        for(int32 k:usedVehs){ auto& fl=Veh(k)[0x42F]; fl=uint8(next()%2 ? fl|8u : fl&~8u); }
                        pStoredCam=static_cast<uint8*>(::operator new(0xD78)); std::memcpy(pStoredCam,&TheCamera,0xD78); for(int k=0;k<32;++k)pStoredCam[0x20+next()%0x900]=uint8(next());
                        *reinterpret_cast<uint32*>(pStoredCam+0x954)=uintptr_t(fakeRwCamera.data());
                        for(int k=0;k<0x30;k+=4)*reinterpret_cast<uint32*>(0x97FB34+k)=next(); *reinterpret_cast<uint16*>(0x97FAEE)=uint16(next());
                        for(uintptr_t fa:{0x97FB34u,0x97FB38u,0x97FB3Cu})*reinterpret_cast<float*>(fa)=uni(0.f,3.f); // floats sin NaN
                        *reinterpret_cast<int16*>(0x97FAEC)=int16(next()%20); *reinterpret_cast<int16*>(0x97FAE8)=int16(next()%20); *reinterpret_cast<float*>(0x97FAE4)=uni(0.f,1.f);
                        *reinterpret_cast<uint8*>(0x97FAB8)=uint8(next()); for(int k=0;k<0x50;k+=4)*reinterpret_cast<uint32*>(0x97FA68+k)=next()%100;
                        std::memcpy(reinterpret_cast<void*>(0xB7CD98),reinterpret_cast<void*>(0xA430B0),0x190); *reinterpret_cast<uint32*>(0xA430B0+0xB8)=next();
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["stored_cam_after"]=uintptr_t(pStoredCam);
                    }
                    else if(nm=="FinishPlayback") {
                        mode=int8(next()%4==0 ? 0 : 1); *reinterpret_cast<uint8*>(0x97FACC)=uint8(next()%2); *reinterpret_cast<uint8*>(0x97FAE1)=uint8(next()%2);
                        SaneFloats(reinterpret_cast<uint8*>(0x97FAC0),12,-3000.f,3000.f); levelResult=next()%5; *reinterpret_cast<int32*>(0x97FABC)=int32(next()%40)-1;
                        findVehResult=next()%2 ? Veh(0) : nullptr; *reinterpret_cast<void**>(Veh(0))=fakeVt.data();
                        Stub(0x45ECD0,dllRestoreStuff,FakeVoid<30>); Stub(0x45E220,dllInit,FakeVoid<31>); Stub(0x45D430,dllMark,FakeVoid<32>);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="RecordThisFrame") {
                        RandomPools(8,10); for(int32 k:usedPeds){ if(next()%2)Ped(k)[0x471]|=0x20; else Ped(k)[0x471]&=~0x20;
                            if(*reinterpret_cast<uint32*>(Ped(k)+0x598)<=1)*reinterpret_cast<void**>(Ped(k)+0x480)=playerDatas[next()%4].data(); } // el EXE no comprueba los datos del jugador
                        for(int32 k:usedVehs){ auto* v=Veh(k); if(*reinterpret_cast<uint32*>(v+0x594)==6&&!usedVehs.empty()){ if(next()%2)*reinterpret_cast<void**>(v+0x5D0)=Veh(usedVehs[next()%usedVehs.size()]); if(next()%2)*reinterpret_cast<void**>(v+0x5D4)=Veh(usedVehs[next()%usedVehs.size()]); } }
                        auto& traces=*reinterpret_cast<std::array<std::array<uint8,0x2C>,16>*>(0xC7C748);
                        for(auto& t:traces){ SaneFloats(t.data(),0x18,-3000.f,3000.f); t[0x18]=uint8(next()%4==0); }
                        *reinterpret_cast<uint8*>(0xC7C70C)=uint8(next()%2); *reinterpret_cast<uint8*>(0xC7C70D)=uint8(next()%2); *reinterpret_cast<int32*>(0xB72914)=int32(next()%18);
                        *reinterpret_cast<uint16*>(0xB70152)=uint16(next()); *reinterpret_cast<int16*>(0xC81320)=int16(next()%20); *reinterpret_cast<int16*>(0xC8131C)=int16(next()%20); *reinterpret_cast<float*>(0xC8130C)=uni(0.f,1.f);
                        *reinterpret_cast<uint32*>(0xB7CB84)=next(); SaneFloats(reinterpret_cast<uint8*>(0xB6F99C),0x40,-2.f,2.f); *reinterpret_cast<uint32*>(0xB6F084)=next(); *reinterpret_cast<float*>(0xB6F154)=uni(0.f,1.f);
                        *reinterpret_cast<uint8*>(0xB7CD74)=0; *reinterpret_cast<void**>(0xB7CD98+0xB0)=next()%3==0 ? Veh(0) : nullptr;
                        for(auto& c:coors)c=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-50.f,300.f)};
                        findVehResult=!usedVehs.empty()&&next()%3==0 ? Veh(usedVehs[0]) : nullptr; *reinterpret_cast<uint8*>(0xB6EC20)=uint8(next()%2);
                        const uint32 slot=next()%8; RandomStatus(false); record.m_pBase=reinterpret_cast<CReplay::tReplayBuffer*>(Buf(slot)); record.m_bSlot=uint8(slot);
                        record.m_nOffset=next()%3==0 ? 99984-0x74-20-next()%0x300 : next()%0x200;
                        Stub(0x45C940,dllStorePedUpdate,StubStorePedUpdate);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="PlayBackThisFrameInterpolation") {
                        RandomPools(10,10); RandomConv(); Models(false);
                        // vehículos: siempre con el slot libre y el modelo sin cargar (no se crean ni se extraen)
                        for(int32 k=0;k<55;++k)if(vehConv[k]>=0&&!(vehFlags[1+vehConv[k]]&0x80))vehConv[k]=-1;
                        if(!usedVehs.empty())for(int32 k=55;k<110;++k)if(next()%2)vehConv[k]=usedVehs[next()%usedVehs.size()];
                        if(!usedPeds.empty())for(int32 k=0;k<110;++k)if(next()%3==0)pedConv[k]=usedPeds[next()%usedPeds.size()];
                        vehSplit=true;
                        for(auto& st:status)st=uint8(next()%3); const uint32 s0=next()%8; const int chain=int(next()%3);
                        for(int c=0;c<=chain;++c){ const uint32 s=(s0+c)%8; const bool lastOne=c==chain; status[s]=lastOne ? 2 : uint8(next()%2);
                            Stream(s,{1,2,3,4,5,6,7,9,10,12,13,14,15,16,17,18,19},int(next()%25),110,lastOne&&next()%3 ? 8 : 0); }
                        *reinterpret_cast<void**>(0xB6F97C)=fakeRwCamera.data(); SaneFloats(reinterpret_cast<uint8*>(0xB6F99C),0x40,-2.f,2.f); SaneFloats(reinterpret_cast<uint8*>(0x97F670),12,-3000.f,3000.f);
                        ppuAdvance=true; for(auto& r:dealResults)r=nullptr;
                        Stub(0x45F050,dllFinish,FakeVoid<40>); Stub(0x45CEA0,dllDealWith,StubDealWith); Stub(0x45CA70,dllPPU,StubProcessPedUpdate); Stub(0x45D060,dllCamera,FakeVoid<41>);
                        CReplay::CAddressInReplayBuffer addr; addr.m_nOffset=0; addr.m_pBase=reinterpret_cast<CReplay::tReplayBuffer*>(Buf(s0)); addr.m_bSlot=uint8(s0);
                        uint32 timer=0xAAAA5555u; const float t=next()%3==0 ? 1.f : uni(0.f,1.f);
                        ret=reinterpret_cast<bool(__cdecl*)(CReplay::CAddressInReplayBuffer&,float,uint32*)>(a)(addr,t,next()%3 ? &timer : nullptr);
                        out["address"]={addr.m_nOffset,Id(addr.m_pBase),addr.m_bSlot}; out["timer"]=timer;
                    }
                    else if(nm=="FastForwardToTime"||nm=="PlayBackThisFrame") {
                        interpFinishAt=next()%20; for(auto& v:interpTimes)v=next()%3==0 ? next() : next()%5000; std::ranges::sort(interpTimes);
                        Stub(0x45F380,dllInterp,StubInterp);
                        if(nm=="FastForwardToTime"){ const uint32 st=next()%5==0 ? 0u : next()%4==0 ? next() : next()%6000; ret=reinterpret_cast<bool(__cdecl*)(uint32)>(a)(st); }
                        else { interpFinishAt=next()%2; Stub(0x45D760,dllLookAround,FakeVoid<42>); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    }
                    else if(nm=="TriggerPlayback") {
                        mode=int8(next()%5==0 ? 1 : 0); for(auto& st:status)st=uint8(next()%2); if(next()%4)status[next()%8]=2; if(std::ranges::all_of(status,[](uint8 v){return v==0;}))status[next()%8]=1;
                        findVehResult=next()%2 ? Veh(0) : nullptr; stationResult=int8(int32(next()%20)-1); levelResult=next()%5;
                        createResult=next()%2 ? reinterpret_cast<CPlayerPed*>(Ped(3)) : nullptr; firstFocus=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};
                        *reinterpret_cast<int32*>(0xB72914)=int32(next()%300)-20;
                        Stub(0x45F180,dllStoreStuff,FakeVoid<50>); Stub(0x45EF20,dllInitPed,FakeVoid<51>); Stub(0x45EFA0,dllInitVeh,FakeVoid<52>); Stub(0x45D390,dllEmptyPools,FakeVoid<53>);
                        Stub(0x45D4B0,dllStream,FakeVoid<54>); Stub(0x45D540,dllCreatePlayer,StubCreatePlayer); Stub(0x45D6C0,dllFindFirst,StubFindFirst);
                        const CVector fixed{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};
                        reinterpret_cast<void(__cdecl*)(eReplayCamMode,CVector,bool)>(a)(eReplayCamMode(next()%3),fixed,next()%2);
                    }
                    else if(nm=="Update") {
                        *reinterpret_cast<uint8*>(0xB5F852)=uint8(next()%8==0); *reinterpret_cast<uint16*>(padDisable)=uint16(next()%8==0 ? 1+next()%3 : 0);
                        *reinterpret_cast<uint8*>(0xBA67A4)=uint8(next()%8==0); *reinterpret_cast<int32*>(0x96A7CC)=next()%8==0 ? 1 : 0;
                        *reinterpret_cast<uint8*>(0xB7CB48)=uint8(next()%8==0); *reinterpret_cast<uint8*>(0xB7CB49)=uint8(next()%8==0);
                        mode=int8(next()%5==0 ? 2 : next()%2); *reinterpret_cast<uint32*>(0xB7CB84)=next(); *reinterpret_cast<uint32*>(0x97FB40)=*reinterpret_cast<uint32*>(0xB7CB84)-next()%40;
                        *reinterpret_cast<uint8*>(0xC3EFAB)=uint8(next()%4==0 ? next() : 0); *reinterpret_cast<uint8*>(0x8A6160)=uint8(next()%5!=0); keyMask=next()&0x7FE;
                        Stub(0x45E220,dllInit,FakeVoid<60>); Stub(0x4604A0,dllPlayBack,FakeVoid<61>); Stub(0x45E300,dllRecord,FakeVoid<62>); Stub(0x45F050,dllFinish,FakeVoid<63>);
                        Stub(0x4600F0,dllTrigger,StubTrigger); Stub(0x45C340,dllSave,FakeVoid<64>); Stub(0x460390,dllPlayHD,FakeVoid<65>);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    local.clear();

                    // --- salida: estado tocado ---
                    if(pStoredCam){ out["camera_copy"]=Fnv(pStoredCam,0xD78); ::operator delete(pStoredCam); pStoredCam=nullptr; }
                    json regs=json::object(); for(auto& r:regions)if(std::string(r.name)!="pools"&&std::string(r.name)!="groups")regs[r.name]=Fnv(reinterpret_cast<void*>(r.at),r.n);
                    out["regions"]=regs; out["replay_vars"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x97FAB8),0xD0);
                    uint64 bh=1469598103934665603ull; for(uint32 s=0;s<8;++s){ bh=Fnv(Buf(s),WIN,bh); bh=Fnv(Buf(s)+BUFSZ-WIN,WIN,bh); } out["buffers"]=bh;
                    uint64 ph=1469598103934665603ull; for(int32 k:usedPeds)ph=Fnv(Ped(k),PED_SIZE,ph); for(int32 k:usedVehs)ph=Fnv(Veh(k),VEH_SIZE,ph);
                    out["objects"]=ph; out["flags"]=Fnv(pedFlags.data(),pedFlags.size(),Fnv(vehFlags.data(),vehFlags.size()));
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreRegions();
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
    RestoreRegions(); std::memcpy(BUFFERS,savedBuffers.data(),savedBuffers.size());
    bool restored=std::memcmp(BUFFERS,savedBuffers.data(),savedBuffers.size())==0;
    for(auto& r:regions)restored&=std::memcmp(reinterpret_cast<void*>(r.at),r.saved.data(),r.n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
