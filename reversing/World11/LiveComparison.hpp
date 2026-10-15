#pragma once
// Banco dentro del motor: coronas, entradas/salidas, reaparicion y manejo de vehiculos (lista de 11; 44 funciones).
// CCoronas, CEntryExit, CRestart y cHandlingDataMgr. Original (hooks del grupo desactivados, codigo del EXE) frente a
// candidato con fixtures deterministas sobre el array real de coronas, los puntos de reaparicion, las variables
// estaticas de las entradas/salidas y los grupos de animacion de vehiculos (todo guardado, rellenado y restaurado en
// cada ejecucion), o sobre objetos falsos (gestor de manejo, entradas/salidas, peatones, vehiculo, entidades,
// matrices, texturas, raster y camara RW). Nada pinta ni lee ficheros: se simulan y trazan RwRenderStateSet (puntero
// del motor), CSprite (proyeccion, sprites, buffer), RwIm3D, RwTextureRead/Destroy, CTxdStore, LittleTest,
// ProcessLineOfSight/ProcessVerticalLine, CRegisteredCorona::Update, CCamera (direccion, fundidos), rand,
// CTheZones::GetLevelFromPosition, CStats::GetStatValue, Save/LoadDataToWorkBuffer, CFileMgr, CFileLoader::LoadLine,
// cTransmission::InitGearRatios, CVehicleAnimGroup::InitAnimGroup/CopyAnimGroup, streaming, poblacion, interiores,
// colision del mundo, grupos de peatones, teletransporte, texto y HUD. TransitionStarted/TransitionFinished solo se
// prueban por los caminos que no crean tareas, eventos ni cargan la escena (ver README).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Coronas.h"
#include "RegisteredCorona.h"
#include "EntryExit.h"
#include "EntryExitManager.h"
#include "Restart.h"
#include "cHandlingDataMgr.h"
#include "cTransmission.h"
#include "VehicleAnimGroupData.h"
#include "Sprite.h"
#include "TxdStore.h"
#include "GenericGameStorage.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "TheZones.h"
#include "Stats.h"
#include "Renderer.h"
#include "Streaming.h"
#include "Population.h"
#include "PedGroups.h"
#include "PedGroupMembership.h"
#include "PedIntelligence.h"
#include "PlayerPed.h"
#include "Clock.h"
#include "Camera.h"
#include "IplStore.h"
#include "PathFind.h"
#include "Hud.h"
#include "ColStore.h"
#include "Text/Text.h"
#include "Interior/InteriorManager_c.h"
#include <reversiblebugfixes/Bugs.hpp>
#include "../ProbeRuntime.hpp"

namespace World11Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr uintptr_t CORONAS=0xC3E058, CORONA_SIZE=64*0x3C, CORONA_GLOB=0xC3E000, TEMP_CNT=0xC4B950, TEMP_IDX=0xC4B958, TEMP_VTX=0xC4D958;
constexpr uintptr_t CAMERA=0xB6F028, SCENE_CAM=0xC1703C, ENGINE=0xC97B24, WEATHER=0xC812D0, AREA=0xB72914;
constexpr uintptr_t RESTART=0xA43248, RESTART_SIZE=0xA43438-0xA43248, ANIMGROUPS=0xC1CDC0, ANIMGROUPS_SIZE=30*0x94;
constexpr uintptr_t ENEX_STATICS=0x96A700, ENEX_STATICS_SIZE=0xE0, VEH_NAMES=0x8D3978;

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<uint8,0x700> fakeEnt{};
alignas(16) inline std::array<uint8,0x48> entMat{}, camMat{}, pedMat{};
alignas(16) inline std::array<std::array<uint8,0x10>,4> fakeTex{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> rasters{};
alignas(16) inline std::array<uint8,0x100> fakeRwCam{};
alignas(16) inline std::array<uint8,0x40> camRaster{};
alignas(16) inline std::array<uint8,0x10000> mgr{};
alignas(16) inline std::array<char,32*24> nameTable{};
alignas(16) inline std::array<std::array<uint8,0x40>,8> enex{};
alignas(16) inline std::array<std::array<uint8,0x7A0>,8> peds{};
alignas(16) inline std::array<std::array<uint8,0x40>,8> intel{};
alignas(16) inline std::array<uint8,0x100> playerData{}, fakeHandling{}, fakeTask{};
alignas(16) inline std::array<uint8,0x600> fakeVeh{};
alignas(16) inline std::array<uint8,0x180> fakeObj{};
alignas(16) inline std::array<uint8,0x40> outBuf{}, gxtText{};
inline std::array<void*,64> pedVt{};
inline std::array<void*,16> taskVt{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); }
    if(u>=CORONAS&&u<CORONAS+CORONA_SIZE) return json::array({"corona",(u-CORONAS)/0x3C,(u-CORONAS)%0x3C});
    if(u>=ANIMGROUPS&&u<ANIMGROUPS+ANIMGROUPS_SIZE+0x94*8) return json::array({"animgroup",intptr_t(u-ANIMGROUPS)});
    if(u>=0xC09920&&u<0xC09920+8*0x2D4) return json::array({"groups",(u-0xC09920)/0x2D4,(u-0xC09920)%0x2D4});
    if(u>=TEMP_IDX&&u<TEMP_IDX+0x2000) return json::array({"temp_idx",u-TEMP_IDX});
    if(u>=TEMP_VTX&&u<TEMP_VTX+0x4000) return json::array({"temp_vtx",u-TEMP_VTX});
    if(u>=0x400000&&u<0x1000000) return json::array({"exe",u});
    return "ext"; }
inline json Val(const void* p){ const auto u=uintptr_t(p); return u<0x10000 ? json(u) : Id(p); }
inline json Vec(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }
inline json Vec(const RwV3d& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }

// --- generador de las respuestas de los stubs (se reinicia igual en cada ejecucion) ---
inline uint32 stub{};
inline uint32 Nx(){ stub^=stub<<13; stub^=stub>>17; stub^=stub<<5; return stub; }
inline float Un(float a,float b){ return a+(b-a)*float(Nx()%1000000)/1000000.f; }

// --- rand ---
inline uint32 rngCalls{};
inline int __cdecl FakeRand(){ trace.push_back({"rand"}); return int((stub*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }

// --- RW y sprites ---
inline RwBool __cdecl FakeRenderState(int32 s,void* v){ trace.push_back({"rs",s,Val(v)}); return TRUE; }
inline int32 rasterW{}, rasterH{}; inline uint32 calcOkPct{}; inline std::array<float,8> zHints{};
inline bool __cdecl FakeCalc(const RwV3d& in,RwV3d* out,float* w,float* h,bool a,bool b){
    const bool ok=Nx()%100<calcOkPct; trace.push_back({"calc",Vec(in),a,b,ok});
    if(ok){ out->x=Un(-60.f,float(rasterW)+60.f); out->y=Un(-60.f,float(rasterH)+60.f); out->z=Un(0.5f,350.f);
        if(Nx()%6==0){ const float z=zHints[Nx()%zHints.size()]; out->z=Nx()%2 ? z : z*0.5f; }
        if(Nx()%12==0)out->x=float(rasterW); if(Nx()%12==0)out->y=0.f; if(Nx()%16==0)out->x=-0.f;
        *w=Un(0.f,300.f); *h=Un(0.f,300.f); }
    return ok; }
inline void __cdecl FakeRotAspect(CVector p,CVector2D s,uint8 r,uint8 g,uint8 b,int16 i,float rz,float rot,uint8 a){ trace.push_back({"xlu_rot",Vec(p),F(s.x),F(s.y),r,g,b,i,F(rz),F(rot),a}); }
inline void __cdecl FakeBuffered(CVector p,CVector2D s,uint8 r,uint8 g,uint8 b,int16 i,float rnz,uint8 a){ trace.push_back({"xlu_buf",Vec(p),F(s.x),F(s.y),r,g,b,i,F(rnz),a}); }
inline void __cdecl FakeBuffered2D(CVector2D p,CVector2D s,const RwRGBA& c,int16 i,uint8 a){ trace.push_back({"xlu_2d",F(p.x),F(p.y),F(s.x),F(s.y),c.red,c.green,c.blue,c.alpha,i,a}); }
inline void __cdecl FakeFlush(){ trace.push_back({"flush_sprites"}); }
inline void __cdecl FakeInitSprites(){ trace.push_back({"init_sprites"}); }
inline uint32 losPct{}, vlPct{};
inline bool __cdecl FakeLOS(const CVector& o,const CVector& t,CColPoint&,CEntity*&,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7,bool b8){
    const bool hit=Nx()%100<losPct; trace.push_back({"los",Vec(o),Vec(t),b1,b2,b3,b4,b5,b6,b7,b8,hit}); return hit; }
inline bool __cdecl FakeVLine(const CVector& o,float d,CColPoint& cp,CEntity*&,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,CStoredCollPoly* poly){
    const bool hit=Nx()%100<vlPct; trace.push_back({"vline",Vec(o),F(d),b1,b2,b3,b4,b5,b6,Id(poly),hit}); if(hit)cp.m_vecPoint.z=o.z-Un(-3.f,30.f); return hit; }
inline void __fastcall FakeCoronaUpdate(void* self,void*){ trace.push_back({"corona_update",Id(self)}); }
inline int32 lookDir{};
inline int32 __fastcall FakeLookDir(void* self,void*){ trace.push_back({"look_dir",Id(self)}); return lookDir; }
inline void __fastcall FakeRegRef(void* self,void*,CEntity** slot){ trace.push_back({"register_reference",Id(self),Id(slot)}); }
inline RwTexture* __cdecl FakeTexRead(const char* n,const char* m){ trace.push_back({"tex_read",std::string(n),std::string(m)}); return Nx()%4 ? reinterpret_cast<RwTexture*>(fakeTex[Nx()%4].data()) : nullptr; }
inline RwBool __cdecl FakeTexDestroy(RwTexture* t){ trace.push_back({"tex_destroy",Id(t)}); return TRUE; }
inline void __cdecl FakePushTxd(){ trace.push_back({"push_txd"}); }
inline void __cdecl FakePopTxd(){ trace.push_back({"pop_txd"}); }
inline int32 __cdecl FakeFindTxd(const char* n){ trace.push_back({"find_txd",std::string(n)}); return int32(Nx()%40); }
inline void __cdecl FakeSetTxd(int32 i){ trace.push_back({"set_txd",i}); }
inline void __cdecl FakeLittleTest(){ trace.push_back({"little_test"}); }
inline void* __cdecl FakeIm3DTransform(void* v,uint32 n,void* m,uint32 f){ const bool ok=Nx()%4!=0; trace.push_back({"im3d_transform",Id(v),n,Id(m),f,ok}); return ok ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(int32 t,void* idx,int32 n){ trace.push_back({"im3d_render",t,Id(idx),n}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }

// --- reaparicion ---
inline float __cdecl FakeStat(eStats s){ const uint32 r=Nx()%20; const float v=r==0 ? std::bit_cast<float>(0x7FC00000u) : float(Nx()%4)+(r==1 ? 0.5f : 0.f); trace.push_back({"stat",int(s),F(v)}); return v; }
inline eLevelName __cdecl FakeLevel(const CVector& p){ const auto l=eLevelName(Nx()%4); trace.push_back({"level",Vec(p),int(l)}); return l; }
inline uint32 loadState{};
inline bool __cdecl FakeSaveWB(void* data,int32 size){ trace.push_back({"save_wb",Id(data),size,Fnv(data,size_t(size))}); return true; }
inline bool __cdecl FakeLoadWB(void* data,int32 size){ trace.push_back({"load_wb",Id(data),size}); auto* b=static_cast<uint8*>(data);
    for(int32 k=0;k<size;++k){ loadState=loadState*1103515245u+12345u; b[k]=uint8(loadState>>16); } if(size==2)At<uint16>(data,0)%=11; return true; }

// --- manejo ---
inline void __fastcall FakeGearRatios(void* self,void*){ trace.push_back({"init_gear_ratios",Id(self)}); }
inline std::vector<std::string> lines; inline size_t lineIdx{}; inline char lineBuf[512]{};
inline int32 __cdecl FakeSetDir(const char* p){ trace.push_back({"set_dir",std::string(p)}); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){ trace.push_back({"open",std::string(p),std::string(m)}); return reinterpret_cast<FILESTREAM>(0x4321); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ trace.push_back({"close",uintptr_t(f)}); return 0; }
inline char* __cdecl FakeLoadLine(FILESTREAM f){ trace.push_back({"load_line",uintptr_t(f)}); if(lineIdx>=lines.size())return nullptr;
    std::memset(lineBuf,0,sizeof(lineBuf)); std::memcpy(lineBuf,lines[lineIdx].data(),std::min<size_t>(lines[lineIdx].size(),sizeof(lineBuf)-1)); ++lineIdx; return lineBuf; }
inline void __fastcall FakeInitAnim(void* self,void*,uint8 a,uint8 b,int32 fl,int32 sp,const float* g,const float* st,const float* en){
    trace.push_back({"init_anim_group",a,b,fl,sp,json::array({F(g[0]),F(g[1]),F(g[2]),F(g[3]),F(g[4])}),json::array({F(st[0]),F(st[1]),F(st[2]),F(st[3])}),json::array({F(en[0]),F(en[1]),F(en[2]),F(en[3])})}); }
inline void __fastcall FakeCopyAnim(void* self,void*,void*){ trace.push_back({"copy_anim_group",Id(self)}); }

// --- entradas/salidas ---
inline void __cdecl FakeReqDir(const CVector& p,float a,int32 f){ trace.push_back({"request_objects_in_direction",Vec(p),F(a),f}); }
inline void __cdecl FakeStreamPeds(const int32* m){ trace.push_back({"stream_peds",json::array({m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7]})}); }
inline void __cdecl FakeRemoveDodgy(){ trace.push_back({"remove_dodgy_peds"}); }
inline void __cdecl FakeCoppers(CVector p,float h){ trace.push_back({"create_waiting_coppers",Vec(p),F(h)}); }
inline void __cdecl FakePopulate(int32 n,CVector p){ trace.push_back({"populate_interior",n,Vec(p)}); }
inline void* heldResult{};
inline CEntity* __fastcall FakeHolding(void* self,void*){ trace.push_back({"holding",Id(self)}); return static_cast<CEntity*>(heldResult); }
inline int32 stealableResult{};
inline int32 __fastcall FakeFindStealable(void* self,void*,CEntity* e){ trace.push_back({"find_stealable",Id(self),Id(e)}); return stealableResult; }
inline void __fastcall FakeSetStolen(void* self,void*,CEntity* e,uint8 s){ trace.push_back({"set_stolen",Id(self),Id(e),s}); }
inline uint32 sphereMask{}, losMask{}, sphereCalls{}, losCalls{};
inline CEntity* __cdecl FakeTestSphere(CVector c,float r,CEntity* ign,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6){
    const bool hit=(sphereMask>>(sphereCalls++%32))&1; trace.push_back({"test_sphere",Vec(c),F(r),Id(ign),b1,b2,b3,b4,b5,b6,hit}); return hit ? reinterpret_cast<CEntity*>(fakeObj.data()) : nullptr; }
inline bool __cdecl FakeLOSClear(const CVector& a,const CVector& b,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){
    const bool clear=(losMask>>(losCalls++%32))&1; trace.push_back({"los_clear",Vec(a),Vec(b),b1,b2,b3,b4,b5,b6,b7,clear}); return clear; }
inline bool leaderResult{}; inline std::array<void*,8> memberTable{};
inline bool __fastcall FakeIsLeader(void* self,void*,const CPed* p){ trace.push_back({"is_leader",Id(self),Id(p)}); return leaderResult; }
inline CPed* __fastcall FakeGetMember(void* self,void*,int32 i){ trace.push_back({"get_member",Id(self),i}); return static_cast<CPed*>(memberTable[uint32(i)%8]); }
inline void __fastcall FakeFlushIntel(void* self,void*,bool b){ trace.push_back({"flush_immediately",Id(self),b}); }
inline void __fastcall FakeTeleport(void* self,void*,CVector p,bool b){ trace.push_back({"teleport",Id(self),Vec(p),b}); }
inline void __fastcall FakeVTrap(void* self,void*){ trace.push_back({"vtrap",Id(self)}); }
inline bool timeInRange{}, fadingResult{}; inline int32 taskTypeResult{};
inline bool __cdecl FakeTimeInRange(uint8 a,uint8 b){ trace.push_back({"time_in_range",a,b}); return timeInRange; }
inline bool __fastcall FakeCanStart(void* self,void*){ trace.push_back({"can_start_mission",Id(self)}); return false; }
inline bool __fastcall FakeGetFading(void* self,void*){ trace.push_back({"get_fading",Id(self)}); return fadingResult; }
inline void __fastcall FakeFade(void* self,void*,float d,uint16 dir){ trace.push_back({"fade",Id(self),F(d),dir}); }
inline void __fastcall FakeFadeColour(void* self,void*,uint8 r,uint8 g,uint8 b){ trace.push_back({"fade_colour",Id(self),r,g,b}); }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p){ trace.push_back({"peds_group",Id(p)}); return nullptr; }
inline void __cdecl FakeAddCol(const CVector& p){ trace.push_back({"add_col",Vec(p)}); }
inline void __cdecl FakeAddIpl(const CVector& p){ trace.push_back({"add_ipl",Vec(p)}); }
inline void __cdecl FakeAddModels(const CVector& p,int32 f){ trace.push_back({"add_models",Vec(p),f}); }
inline void __fastcall FakeSetPaths(void* self,void*,const CVector& p){ trace.push_back({"set_paths",Id(self),Vec(p)}); }
inline void __cdecl FakeSetAreaVisible(){ trace.push_back({"set_area_visible"}); }
inline const GxtChar* __fastcall FakeTextGet(void* self,void*,const char* key){ trace.push_back({"text_get",Id(self),std::string(key,strnlen(key,8))}); return gxtText.data(); }
inline void __cdecl FakeZoneName(const GxtChar* n,bool b){ trace.push_back({"zone_name",Id(n),b}); }
inline int32 __fastcall FakeTaskType(void* self,void*){ trace.push_back({"task_type",Id(self)}); return taskTypeResult; }

// Llama a una funcion thiscall (ret 4) con un centinela en edx y devuelve edx al volver
inline uint32 CallKeepEdx(uintptr_t fn,void* self,void* arg){
    uint32 edxOut;
    __asm {
        mov  ecx, self
        push arg
        mov  edx, 5A17C0DEh
        call fn
        mov  edxOut, edx
    }
    return edxOut;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,44> fns{{
    {"CCoronas","Init",0x6FAA70,0,-1,64},{"CCoronas","Shutdown",0x6FAB00,0,-1,64},{"CCoronas","Update",0x6FADF0,0,-1,256},
    {"CCoronas","Render",0x6FAEC0,0,-1,512},{"CCoronas","RenderReflections",0x6FB630,0,-1,512},
    {"CCoronas","RenderOutGeometryBufferForReflections",0x6FAA10,0,-1,128},{"CCoronas","RenderSunReflection",0x6FBAA0,0,-1,512},
    {"CCoronas","RegisterCorona-type",0x6FC180,0,-1,1024},{"CCoronas","RegisterCorona-texture",0x6FC580,0,-1,512},
    {"CCoronas","UpdateCoronaCoors",0x6FC4D0,0,-1,512},{"CCoronas","DoSunAndMoon",0x6FC5A0,0,-1,512},
    {"CEntryExit","FindValidTeleportPoint",0x43EAF0,0,-1,512},{"CEntryExit","GenerateAmbientPeds",0x43E8B0,0,-1,256},
    {"CEntryExit","GetEntryExitToDisplayNameOf",0x43E650,0,-1,256},{"CEntryExit","GetPositionRelativeToOutsideWorld",0x43EA00,0,-1,256},
    {"CEntryExit","IsInArea",0x43E460,0,-1,1024},{"CEntryExit","ProcessStealableObjects",0x43E990,0,-1,256},
    {"CEntryExit","RequestAmbientPeds",0x43E6D0,0,-1,256},{"CEntryExit","RequestObjectsInFrustum",0x43E690,0,-1,128},
    {"CEntryExit","TransitionFinished",0x4404A0,0,-1,512},{"CEntryExit","TransitionStarted",0x43FFD0,0,-1,512},
    {"CEntryExit","WarpGangWithPlayer",0x43F1F0,0,-1,512},
    {"CRestart","AddHospitalRestartPoint",0x460730,0,-1,256},{"CRestart","AddPoliceRestartPoint",0x460780,0,-1,256},
    {"CRestart","CancelOverrideRestart",0x460800,0,-1,64},{"CRestart","ClearRespawnPointForDurationOfMission",0x460840,0,-1,64},
    {"CRestart","FindClosestHospitalRestartPoint",0x460850,0,-1,1024},{"CRestart","FindClosestPoliceRestartPoint",0x460A50,0,-1,1024},
    {"CRestart","Initialise",0x460630,0,-1,64},{"CRestart","Load",0x5D3770,0,-1,128},{"CRestart","OverrideNextRestart",0x4607D0,0,-1,128},
    {"CRestart","Save",0x5D3620,0,-1,128},{"CRestart","SetRespawnPointForDurationOfMission",0x460810,0,-1,128},
    {"cHandlingDataMgr","ConvertBikeDataToGameUnits",0x6F5290,0,-1,256},{"cHandlingDataMgr","ConvertBikeDataToWorldUnits",0x6F5240,0,-1,256},
    {"cHandlingDataMgr","ConvertDataToGameUnits",0x6F5080,0,-1,1024},{"cHandlingDataMgr","ConvertDataToWorldUnits",0x6F5010,0,-1,512},
    {"cHandlingDataMgr","FindExactWord",0x6F4F30,0,-1,512},{"cHandlingDataMgr","GetBoatPointer",0x6F5300,0,-1,256},
    {"cHandlingDataMgr","GetFlyingPointer",0x6F52D0,0,-1,256},{"cHandlingDataMgr","GetHandlingId",0x6F4FD0,0,-1,256},
    {"cHandlingDataMgr","HasFrontWheelDrive",0x6A0480,0,-1,256},{"cHandlingDataMgr","HasRearWheelDrive",0x6A04B0,0,-1,256},
    {"cHandlingDataMgr","LoadHandlingData",0x5BD830,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"world11-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"world11-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("world11");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"world11-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,44> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,44> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    // CMatrix::SetRotateZ/SetRotateZOnly de la DLL usan sin/cos de C: durante el banco el EXE usa su propio codigo (fsin/fcos)
    std::vector<std::tuple<ReversibleHooks::ReversibleHook::Base*,bool,bool>> extra;
    for(const char* nm:{"SetRotateZ","SetRotateZOnly"})if(auto* h=FindHook("CMatrix",nm)){ extra.emplace_back(h,h->Hooked(),h->Locked()); h->LockState(false); h->State(false); }
    // RenderReflections: la correccion reversible PS2CoronaRendering (desviacion NOTSA) pasa z de pantalla en vez de
    // la z cercana del motor; durante el banco se desactiva para comparar con el EXE
    auto& ps2Coronas=const_cast<bool&>(notsa::bugfixes::PS2CoronaRendering.Enabled); const bool ps2Saved=ps2Coronas; ps2Coronas=false;
    using std::bit_cast;
    if(regions.empty()){ Reg(fakeEnt.data(),fakeEnt.size()); Reg(entMat.data(),entMat.size()); Reg(camMat.data(),camMat.size()); Reg(pedMat.data(),pedMat.size());
        for(auto& b:fakeTex)Reg(b.data(),b.size()); for(auto& b:rasters)Reg(b.data(),b.size()); Reg(fakeRwCam.data(),fakeRwCam.size()); Reg(camRaster.data(),camRaster.size());
        Reg(mgr.data(),mgr.size()); Reg(nameTable.data(),nameTable.size()); for(auto& b:enex)Reg(b.data(),b.size()); for(auto& b:peds)Reg(b.data(),b.size());
        for(auto& b:intel)Reg(b.data(),b.size()); Reg(playerData.data(),playerData.size()); Reg(fakeHandling.data(),fakeHandling.size()); Reg(fakeTask.data(),fakeTask.size());
        Reg(fakeVeh.data(),fakeVeh.size()); Reg(fakeObj.data(),fakeObj.size()); Reg(outBuf.data(),outBuf.size()); Reg(gxtText.data(),gxtText.size());
        Reg(reinterpret_cast<void*>(CAMERA),0xD78); Reg(reinterpret_cast<void*>(0xBAF670),0x100); Reg(reinterpret_cast<void*>(0x96F050),0x100); Reg(reinterpret_cast<void*>(0xC1B340),0x40);
        Reg(reinterpret_cast<void*>(VEH_NAMES),210*14); Reg(reinterpret_cast<void*>(ENEX_STATICS),ENEX_STATICS_SIZE); }
    for(auto& v:pedVt)v=reinterpret_cast<void*>(&FakeVTrap); pedVt[14]=reinterpret_cast<void*>(&FakeTeleport);
    for(auto& v:taskVt)v=reinterpret_cast<void*>(&FakeVTrap); taskVt[4]=reinterpret_cast<void*>(&FakeTaskType);

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {CORONAS,CORONA_SIZE},{CORONA_GLOB,0x58},{0xC3EF58,4},{0xC3EF9C,4},{0x8D4B5C,8},{TEMP_CNT,8},{TEMP_IDX,0x200},{TEMP_VTX,0x900},
            {CAMERA,0x900},{SCENE_CAM,4},{0xB7CB5C,4},{0xB7CB4C,4},{0xB7CB84,4},{WEATHER,0x40},{AREA,4},{0xB79FD0,4},{0xB7CA50,16*12},{0xB7C4D0,0x10},
            {RESTART,RESTART_SIZE},{ANIMGROUPS,ANIMGROUPS_SIZE},{ENEX_STATICS,ENEX_STATICS_SIZE},{0xC0FCB6,1},{0x8D253C,4},{0xB76850,1},{0xC098E0,8}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto* const engine=*reinterpret_cast<uint8**>(ENGINE); auto& rsSlot=At<void*>(engine,0x20); void* const rsSaved=rsSlot;
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); rsSlot=rsSaved; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch r1(0x821B1E,FakeRand), r2(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber);
        EntryPatch s1(bit_cast<uintptr_t>(&CSprite::CalcScreenCoors),FakeCalc), s1e(0x70CE30,FakeCalc);
        EntryPatch s2(bit_cast<uintptr_t>(&CSprite::RenderOneXLUSprite_Rotate_Aspect),FakeRotAspect), s2e(0x70D490,FakeRotAspect);
        EntryPatch s3(bit_cast<uintptr_t>(&CSprite::RenderBufferedOneXLUSprite),FakeBuffered), s3e(0x70E4A0,FakeBuffered);
        EntryPatch s4(bit_cast<uintptr_t>(&CSprite::RenderBufferedOneXLUSprite2D),FakeBuffered2D), s4e(0x70F440,FakeBuffered2D);
        EntryPatch s5(bit_cast<uintptr_t>(&CSprite::FlushSpriteBuffer),FakeFlush), s5e(0x70CF20,FakeFlush);
        EntryPatch s6(bit_cast<uintptr_t>(&CSprite::InitSpriteBuffer),FakeInitSprites), s6e(0x70CFB0,FakeInitSprites);
        EntryPatch w1(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),FakeLOS), w1e(0x56BA00,FakeLOS);
        EntryPatch w2(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FakeVLine), w2e(0x5674E0,FakeVLine);
        EntryPatch c1(bit_cast<uintptr_t>(&CRegisteredCorona::Update),FakeCoronaUpdate), c1e(0x6FABF0,FakeCoronaUpdate);
        EntryPatch c2(bit_cast<uintptr_t>(&CCamera::GetLookDirection),FakeLookDir), c2e(0x50AE90,FakeLookDir);
        EntryPatch c3(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegRef), c3e(0x571B70,FakeRegRef);
        EntryPatch t1e(0x7F3AC0,FakeTexRead), t2e(0x7F3820,FakeTexDestroy);
        EntryPatch x1(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),FakePushTxd), x1e(0x7316A0,FakePushTxd);
        EntryPatch x2(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),FakePopTxd), x2e(0x7316B0,FakePopTxd);
        EntryPatch x3(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),FakeFindTxd), x3e(0x731850,FakeFindTxd);
        EntryPatch x4(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),FakeSetTxd), x4e(0x7319C0,FakeSetTxd);
        EntryPatch lt(bit_cast<uintptr_t>(&LittleTest),FakeLittleTest), lte(0x541330,FakeLittleTest);
        EntryPatch i1e(0x7EF450,FakeIm3DTransform), i2e(0x7EF550,FakeIm3DRender), i3e(0x7EF520,FakeIm3DEnd);
        EntryPatch st(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),FakeStat), ste(0x558E40,FakeStat);
        EntryPatch lv(bit_cast<uintptr_t>(&CTheZones::GetLevelFromPosition),FakeLevel), lve(0x572300,FakeLevel);
        EntryPatch gs(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWB), gse(0x5D1270,FakeSaveWB);
        EntryPatch gl(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FakeLoadWB), gle(0x5D1300,FakeLoadWB);
        EntryPatch gr(bit_cast<uintptr_t>(&cTransmission::InitGearRatios),FakeGearRatios), gre(0x6D0460,FakeGearRatios);
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), f1e(0x5387D0,FakeSetDir);
        EntryPatch f2(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), f2e(0x538900,FakeOpenFile);
        EntryPatch f3(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), f3e(0x5389D0,FakeCloseFile);
        EntryPatch f4(bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),FakeLoadLine), f4e(0x536F80,FakeLoadLine);
        EntryPatch a1(bit_cast<uintptr_t>(&CVehicleAnimGroup::InitAnimGroup),FakeInitAnim), a1e(0x5B9EB0,FakeInitAnim);
        EntryPatch a2(bit_cast<uintptr_t>(&CVehicleAnimGroup::CopyAnimGroup),FakeCopyAnim), a2e(0x5B9E40,FakeCopyAnim);
        EntryPatch e1(bit_cast<uintptr_t>(&CRenderer::RequestObjectsInDirection),FakeReqDir), e1e(0x555CB0,FakeReqDir);
        EntryPatch e2(bit_cast<uintptr_t>(&CStreaming::StreamPedsIntoRandomSlots),FakeStreamPeds), e2e(0x40BDA0,FakeStreamPeds);
        EntryPatch e3(bit_cast<uintptr_t>(&CStreaming::RemoveDodgyPedsFromRandomSlots),FakeRemoveDodgy), e3e(0x40BE60,FakeRemoveDodgy);
        EntryPatch e4(bit_cast<uintptr_t>(&CPopulation::CreateWaitingCoppers),FakeCoppers), e4e(0x6133F0,FakeCoppers);
        EntryPatch e5(bit_cast<uintptr_t>(&CPopulation::PopulateInterior),FakePopulate), e5e(0x616470,FakePopulate);
        EntryPatch e6(bit_cast<uintptr_t>(&CPed::GetEntityThatThisPedIsHolding),FakeHolding), e6e(0x5E02E0,FakeHolding);
        EntryPatch e7(bit_cast<uintptr_t>(static_cast<int32(InteriorManager_c::*)(CEntity*) const>(&InteriorManager_c::FindStealableObjectId)),FakeFindStealable), e7e(0x598360,FakeFindStealable);
        EntryPatch e8(bit_cast<uintptr_t>(&InteriorManager_c::SetStealableObjectStolen),FakeSetStolen), e8e(0x598390,FakeSetStolen);
        EntryPatch e9(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld),FakeTestSphere), e9e(0x569E20,FakeTestSphere);
        EntryPatch e10(bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,const CVector&,bool,bool,bool,bool,bool,bool,bool)>(&CWorld::GetIsLineOfSightClear)),FakeLOSClear), e10e(0x56A490,FakeLOSClear);
        EntryPatch e12(bit_cast<uintptr_t>(&CPedGroupMembership::IsLeader),FakeIsLeader), e12e(0x5F69C0,FakeIsLeader);
        EntryPatch e13(bit_cast<uintptr_t>(&CPedGroupMembership::GetMember),FakeGetMember), e13e(0x5F69B0,FakeGetMember);
        EntryPatch e14(bit_cast<uintptr_t>(&CPedIntelligence::FlushImmediately),FakeFlushIntel), e14e(0x601640,FakeFlushIntel);
        EntryPatch e15(bit_cast<uintptr_t>(&CClock::GetIsTimeInRange),FakeTimeInRange), e15e(0x52CEE0,FakeTimeInRange);
        EntryPatch e16(bit_cast<uintptr_t>(&CPlayerPed::CanPlayerStartMission),FakeCanStart), e16e(0x609590,FakeCanStart);
        EntryPatch e17(bit_cast<uintptr_t>(&CCamera::GetFading),FakeGetFading), e17e(0x50ADE0,FakeGetFading);
        EntryPatch e18(bit_cast<uintptr_t>(&CCamera::Fade),FakeFade), e18e(0x50AC20,FakeFade);
        EntryPatch e19(bit_cast<uintptr_t>(&CCamera::SetFadeColour),FakeFadeColour), e19e(0x50BF00,FakeFadeColour);
        EntryPatch e20(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),FakePedsGroup), e20e(0x5F7E80,FakePedsGroup);
        EntryPatch e21(bit_cast<uintptr_t>(&CColStore::AddCollisionNeededAtPosn),FakeAddCol), e21e(0x4103A0,FakeAddCol);
        EntryPatch e22(bit_cast<uintptr_t>(&CIplStore::AddIplsNeededAtPosn),FakeAddIpl), e22e(0x4045B0,FakeAddIpl);
        EntryPatch e23(bit_cast<uintptr_t>(&CStreaming::AddModelsToRequestList),FakeAddModels), e23e(0x40D3F0,FakeAddModels);
        EntryPatch e24(bit_cast<uintptr_t>(&CPathFind::SetPathsNeededAtPosition),FakeSetPaths), e24e(0x44DCD0,FakeSetPaths);
        EntryPatch e25(bit_cast<uintptr_t>(&CEntryExitManager::SetAreaCodeForVisibleObjects),FakeSetAreaVisible), e25e(0x43ECF0,FakeSetAreaVisible);
        EntryPatch e26(bit_cast<uintptr_t>(&CText::Get),FakeTextGet), e26e(0x6A0050,FakeTextGet);
        EntryPatch e27(bit_cast<uintptr_t>(&CHud::SetZoneName),FakeZoneName), e27e(0x588BB0,FakeZoneName);

        auto& sceneCam=*reinterpret_cast<void**>(SCENE_CAM); auto& camMatrix=*reinterpret_cast<void**>(CAMERA+0x14); auto& camPlacement=*reinterpret_cast<CVector*>(CAMERA+4);
        auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C); auto& frameCounter=*reinterpret_cast<uint32*>(0xB7CB4C); auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84);
        auto& currArea=*reinterpret_cast<int32*>(AREA); auto& storedValue=*reinterpret_cast<uint32*>(0xB79FD0); auto* vecToSun=reinterpret_cast<CVector*>(0xB7CA50);
        auto& spectrum=*reinterpret_cast<float*>(0xC812D0); auto& wind=*reinterpret_cast<float*>(0xC812F0); auto& fog=*reinterpret_cast<float*>(0xC81300);
        auto& clouds=*reinterpret_cast<float*>(0xC81304); auto& wetRoads=*reinterpret_cast<float*>(0xC81308);
        auto* textures=reinterpret_cast<void**>(CORONA_GLOB); auto& numCoronas=*reinterpret_cast<uint32*>(0xC3E038); auto* cor=reinterpret_cast<uint8*>(CORONAS);
        auto& spawnPoint=*reinterpret_cast<void**>(0x96A7C0); auto* enexStack=reinterpret_cast<void**>(0x96A720); auto& stackPosn=*reinterpret_cast<int32*>(0x96A7C4);
        auto& exitEnterState=*reinterpret_cast<int32*>(0x96A7CC); auto& warping=*reinterpret_cast<uint8*>(0x96A7B8);
        auto* restart=reinterpret_cast<uint8*>(RESTART);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; }; // sin NaN
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto fillMatrix=[&](void* m,const CVector& pos){ fillWords(m,0x48); for(int k=0;k<12;++k)if(k!=3&&k!=7&&k!=11)At<float>(m,k*4)=uni(-1.f,1.f); At<CVector>(m,0x30)=pos; At<void*>(m,0x40)=nullptr; };
                auto setCamera=[&](const CVector& p){ if(next()%3){ fillMatrix(camMat.data(),p); camMatrix=camMat.data(); camPlacement=randVec(-10.f,10.f); } else { camMatrix=nullptr; camPlacement=p; } };
                auto camPos=[&]()->CVector{ return camMatrix ? At<CVector>(camMat.data(),0x30) : camPlacement; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1111u; if(!stt)stt=1; trace=json::array();
                    stub=stt*0x2545F491u|1u; rngCalls=0; sphereCalls=losCalls=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    rsSlot=reinterpret_cast<void*>(&FakeRenderState);

                    if(cls=="CCoronas") {
                        // camara, raster, texturas y coronas
                        setCamera(randVec(-2500.f,2500.f)); const CVector cam=camPos();
                        fillWords(fakeRwCam.data(),fakeRwCam.size()); fillWords(camRaster.data(),camRaster.size());
                        rasterW=int32(320+next()%1700); rasterH=int32(200+next()%1000);
                        At<void*>(fakeRwCam.data(),0x60)=camRaster.data(); At<int32>(camRaster.data(),0xC)=rasterW; At<int32>(camRaster.data(),0x10)=rasterH;
                        At<float>(fakeRwCam.data(),0x80)=uni(0.05f,2.f); At<float>(fakeRwCam.data(),0x84)=uni(100.f,3000.f); sceneCam=fakeRwCam.data();
                        for(size_t k=0;k<fakeTex.size();++k){ fillWords(fakeTex[k].data(),0x10); At<void*>(fakeTex[k].data(),0)=rasters[k].data(); }
                        for(int k=0;k<10;++k)textures[k]=next()%5 ? static_cast<void*>(fakeTex[next()%4].data()) : nullptr;
                        // Render/RenderReflections/RenderSunReflection leen el raster de las texturas 0, 3 y 4 sin comprobar nulo (0x6FB32C, 0x6FB6BB, 0x6FC0CF)
                        for(int k:{0,3,4})if(!textures[k])textures[k]=fakeTex[k%4].data();
                        fillWords(fakeEnt.data(),fakeEnt.size()); fillMatrix(entMat.data(),cam+randVec(-80.f,80.f)); At<void*>(fakeEnt.data(),0x14)=entMat.data();
                        fakeEnt[0x36]=uint8((next()&0xF8)|(next()%2 ? 2 : 3)); At<int32>(fakeEnt.data(),0x594)=next()%2 ? 9 : int32(next()%12);
                        fillMatrix(fakeEnt.data()+0x5CC,cam+randVec(-80.f,80.f));
                        const uint32 activePct=nm=="Render"||nm=="RenderReflections" ? 4+next()%14 : 20+next()%70;
                        fillWords(cor,CORONA_SIZE);
                        for(int k=0;k<64;++k){ uint8* c=cor+k*0x3C;
                            At<uint32>(c,0xC)=next()%100<activePct ? 1+next()%90 : 0; At<CVector>(c,0)=next()%10==0 ? randVec(-3.f,3.f) : cam+randVec(-150.f,150.f);
                            At<void*>(c,0x10)=next()%3 ? static_cast<void*>(fakeTex[next()%4].data()) : nullptr;
                            At<float>(c,0x14)=uni(0.1f,6.f); At<float>(c,0x18)=uni(-3.f,3.f); At<float>(c,0x1C)=uni(5.f,300.f); At<float>(c,0x20)=uni(0.f,3.f);
                            At<float>(c,0x24)=next()%8==0 ? 20.f : uni(-5.f,30.f); At<float>(c,0x28)=uni(0.f,20.f);
                            c[0x2C]=uint8(next()); c[0x2D]=uint8(next()); c[0x2E]=uint8(next()); c[0x2F]=next()%5==0 ? 0 : uint8(next());
                            c[0x30]=next()%4==0 ? 0 : uint8(next()); c[0x31]=uint8(next()%2); c[0x32]=uint8(next()%3); c[0x33]=uint8(next()%2);
                            c[0x34]=uint8(next()); c[0x35]=uint8(next()%2); c[0x36]=uint8(next());
                            At<void*>(c,0x38)=next()%8==0 ? static_cast<void*>(fakeEnt.data()) : nullptr; }
                        for(size_t k=0;k<zHints.size();++k)zHints[k]=At<float>(cor+(next()%64)*0x3C,0x1C)*(next()%2 ? 1.f : 0.75f);
                        numCoronas=next()%65; calcOkPct=60+next()%40; losPct=next()%100; vlPct=next()%100;
                        timeStep=next()%6==0 ? uni(5.f,60.f) : uni(0.f,3.f); frameCounter=next(); timeMs=next();
                        currArea=next()%4==0 ? int32(1+next()%17) : 0; fog=uni(0.f,1.f); spectrum=next()%3==0 ? 0.f : uni(0.f,1.f);
                        wind=uni(0.f,1.f); clouds=uni(0.f,1.f); wetRoads=next()%5==0 ? -uni(0.f,1.f) : (next()%8==0 ? 0.f : uni(0.f,1.f));
                        storedValue=next()%16; for(int k=0;k<16;++k){ CVector v=randVec(-1.f,1.f); if(next()%8==0)v.z=next()%2 ? -0.05f : -0.1f; if(next()%10==0)v.z=0.f; vecToSun[k]=v; }
                        fill(reinterpret_cast<void*>(0xB7C4D0),12); At<float>(reinterpret_cast<void*>(0xB7C4D0),0xC)=uni(0.f,8.f);
                        *reinterpret_cast<float*>(0xC3EF9C)=uni(50.f,3000.f);
                        auto coronaId=[&]()->uint32{ if(next()%2){ for(int t=0;t<8;++t){ const uint32 id=At<uint32>(cor+(next()%64)*0x3C,0xC); if(id)return id; } } return 1+next()%120; };
                        if(nm=="Init"){ fill(textures,40); for(int k=0;k<10;++k)if(next()%2)textures[k]=nullptr; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Shutdown"){ for(int k=0;k<10;++k)if(next()%3==0)textures[k]=nullptr; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Update"){ *reinterpret_cast<float*>(0x8D4B5C)=next()%4==0 ? uni(0.9f,1.1f) : uni(0.f,1.f); auto& camIdx=*reinterpret_cast<uint8*>(CAMERA+0x59); camIdx=uint8(next()%2);
                            for(int k=0;k<3;++k)for(int b=0;b<3;++b)*reinterpret_cast<uint8*>(CAMERA+0x17B+k*0x238+b)=next()%3==0 ? 1 : 0;
                            lookDir=next()%2 ? 0 : int32(1+next()%3); *reinterpret_cast<int32*>(0xC3EF58)=next()%2 ? int32(next()%16) : 0; *reinterpret_cast<int32*>(0xC3E034)=int32(next()%6)-1;
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["globals"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC3E034),4); out["last"]=*reinterpret_cast<int32*>(0xC3EF58); out["lights"]=F(*reinterpret_cast<float*>(0x8D4B5C)); }
                        else if(nm=="Render"||nm=="RenderReflections"){ reinterpret_cast<void(__cdecl*)()>(a)();
                            json fl=json::array(); for(int k=0;k<64;++k)fl.push_back(json::array({cor[k*0x3C+0x34],cor[k*0x3C+0x36],F(At<float>(cor+k*0x3C,0x24))})); out["flags"]=fl; }
                        else if(nm=="RenderOutGeometryBufferForReflections"){ auto* cnt=reinterpret_cast<uint32*>(TEMP_CNT); cnt[0]=next()%3==0 ? 0 : next()%600; cnt[1]=next()%3==0 ? 0 : next()%900;
                            if(next()%5==0)cnt[1]=0x10000+next()%10; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="RenderSunReflection"){ fillWords(reinterpret_cast<void*>(TEMP_VTX),0x900); fill(reinterpret_cast<void*>(TEMP_IDX),0x200); fill(reinterpret_cast<void*>(TEMP_CNT),8);
                            static const CVector spots[]{{611.f,875.f,0.f},{-929.f,2364.f,0.f},{-1034.f,2640.f,0.f},{2372.f,-1854.f,0.f},{-1633.f,106.f,0.f}};
                            if(next()%2){ const CVector& s=spots[next()%5]; const float ang=uni(0.f,6.28f), d=next()%6==0 ? (next()%2 ? 250.f : 350.f) : uni(0.f,500.f);
                                setCamera(CVector{s.x+std::cos(ang)*d,s.y+std::sin(ang)*d,uni(0.f,50.f)}); }
                            CVector& v=vecToSun[storedValue]; if(next()%3)v.z=uni(-0.05f,0.6f); if(next()%5==0){ clouds=0.f; fog=0.f; wind=uni(0.f,0.3f); }
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            out["vtx"]=ReversingProbe::Bytes(reinterpret_cast<void*>(TEMP_VTX),44*0x24); out["idx"]=ReversingProbe::Bytes(reinterpret_cast<void*>(TEMP_IDX),126*2); }
                        else if(nm=="RegisterCorona-type"||nm=="RegisterCorona-texture"){
                            if(next()%10==0)for(int k=0;k<64;++k)if(!At<uint32>(cor+k*0x3C,0xC))At<uint32>(cor+k*0x3C,0xC)=200+k;
                            const uint32 id=coronaId(); CEntity* attach=next()%5==0 ? reinterpret_cast<CEntity*>(fakeEnt.data()) : nullptr;
                            const bool neon=next()%3==0; const float range=uni(5.f,120.f);
                            CVector pos=attach ? randVec(-5.f,5.f) : cam+randVec(-90.f,90.f);
                            if(neon&&!attach){ const CVector d=randVec(-1.f,1.f); const float m=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z)+1e-3f; const float dist=next()%8==0 ? (next()%2 ? 35.f : 50.f) : uni(20.f,70.f);
                                pos=CVector{cam.x+d.x/m*dist,cam.y+d.y/m*dist,cam.z+d.z/m*dist}; }
                            auto* p=reinterpret_cast<CVector*>(outBuf.data()); *p=pos;
                            const uint8 r=uint8(next()), g=uint8(next()), b=uint8(next()), inten=next()%5==0 ? 0 : uint8(next());
                            const float size=uni(0.f,8.f), angle=uni(-3.f,3.f), pull=uni(0.f,3.f), fade=uni(0.f,30.f);
                            const uint8 flare=uint8(next()%3), refl=uint8(next()%2), los=uint8(next()%2), trail=uint8(next()%2);
                            const bool fullBright=next()%2, below=next()%2, white=next()%2;
                            if(nm=="RegisterCorona-type")
                                reinterpret_cast<void(__cdecl*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector*,float,float,void*,uint8,uint8,uint8,uint8,float,bool,float,bool,float,bool,bool)>(a)(
                                    id,attach,r,g,b,inten,p,size,range,next()%4 ? static_cast<void*>(fakeTex[next()%4].data()) : nullptr,flare,refl,los,trail,angle,neon,pull,fullBright,fade,below,white);
                            else { const int32 type=next()%8==0 ? int32(0x100+next()%10) : int32(next()%10);
                                reinterpret_cast<void(__cdecl*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector*,float,float,int32,uint8,uint8,uint8,uint8,float,bool,float,bool,float,bool,bool)>(a)(
                                    id,attach,r,g,b,inten,p,size,range,type,flare,refl,los,trail,angle,neon,pull,fullBright,fade,below,white); }
                            out["cor"]=ReversingProbe::Bytes(cor,CORONA_SIZE); }
                        else if(nm=="UpdateCoronaCoors"){ auto* p=reinterpret_cast<CVector*>(outBuf.data()); *p=cam+randVec(-90.f,90.f);
                            reinterpret_cast<void(__cdecl*)(uint32,const CVector*,float,float)>(a)(coronaId(),p,next()%10==0 ? std::sqrt((cam.x-p->x)*(cam.x-p->x)+(cam.y-p->y)*(cam.y-p->y)) : uni(5.f,120.f),uni(-3.f,3.f));
                            out["cor"]=ReversingProbe::Bytes(cor,CORONA_SIZE); }
                        else { // DoSunAndMoon
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["cor"]=ReversingProbe::Bytes(cor,CORONA_SIZE); }
                        out["cor_hash"]=Fnv(cor,CORONA_SIZE); out["glob"]=ReversingProbe::Bytes(reinterpret_cast<void*>(CORONA_GLOB),0x3C); out["temp"]=ReversingProbe::Bytes(reinterpret_cast<void*>(TEMP_CNT),8);
                    }
                    else if(cls=="CRestart") {
                        fillWords(restart,RESTART_SIZE);
                        auto& nHosp=*reinterpret_cast<uint16*>(0xA4326C); auto& nPol=*reinterpret_cast<uint16*>(0xA43268);
                        const bool add=nm=="AddHospitalRestartPoint"||nm=="AddPoliceRestartPoint";
                        nHosp=uint16(next()%(add ? 10 : 11)); nPol=uint16(next()%(add ? 10 : 11));
                        for(int k=0;k<10;++k){ *reinterpret_cast<int32*>(0xA432C0+k*4)=int32(next()%5)-1; *reinterpret_cast<int32*>(0xA43270+k*4)=int32(next()%5)-1; }
                        const CVector base=randVec(-3000.f,3000.f);
                        for(int k=0;k<10;++k){ *reinterpret_cast<CVector*>(0xA43318+k*12)=base+randVec(-1500.f,1500.f); *reinterpret_cast<CVector*>(0xA43390+k*12)=base+randVec(-1500.f,1500.f); }
                        *reinterpret_cast<uint8*>(0xA43264)=next()%4==0; *reinterpret_cast<uint8*>(0xA43248)=next()%4==0;
                        auto setExtra=[&](uintptr_t coors,uintptr_t radius){ *reinterpret_cast<CVector*>(coors)=base+randVec(-300.f,300.f); const uint32 m=next()%6;
                            *reinterpret_cast<float*>(radius)=m==0 ? -uni(0.f,100.f) : m==1 ? 0.f : m==2 ? std::bit_cast<float>(0x7FC00000u) : uni(0.f,500.f); };
                        setExtra(0xA43414,0xA43258); setExtra(0xA43420,0xA43250);
                        auto* outPos=reinterpret_cast<CVector*>(outBuf.data()); auto* outAng=reinterpret_cast<float*>(outBuf.data()+0x10); fillWords(outBuf.data(),outBuf.size());
                        if(nm=="Initialise")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(add){ *outPos=randVec(-3000.f,3000.f); reinterpret_cast<void(__cdecl*)(const CVector*,float,int32)>(a)(outPos,uni(-400.f,400.f),int32(next()%6)-1); }
                        else if(nm=="OverrideNextRestart"){ *outPos=randVec(-3000.f,3000.f); reinterpret_cast<void(__cdecl*)(const CVector*,float)>(a)(outPos,uni(-400.f,400.f)); }
                        else if(nm=="CancelOverrideRestart"||nm=="ClearRespawnPointForDurationOfMission")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="SetRespawnPointForDurationOfMission")reinterpret_cast<void(__cdecl*)(CVector)>(a)(randVec(-3000.f,3000.f));
                        else if(nm=="Load"){ loadState=next(); ret=reinterpret_cast<uint8(__cdecl*)()>(a)(); }
                        else if(nm=="Save")ret=reinterpret_cast<uint8(__cdecl*)()>(a)();
                        else { CVector p=base+randVec(-1500.f,1500.f); if(next()%5==0)p=*reinterpret_cast<CVector*>(next()%2 ? 0xA43414 : 0xA43420)+randVec(-50.f,50.f);
                            reinterpret_cast<void(__cdecl*)(CVector,CVector*,float*)>(a)(p,outPos,outAng); }
                        out["restart"]=ReversingProbe::Bytes(restart,RESTART_SIZE); out["out"]=ReversingProbe::Bytes(outBuf.data(),0x14);
                    }
                    else if(cls=="cHandlingDataMgr") {
                        fillWords(mgr.data(),mgr.size()); void* const self=mgr.data();
                        auto fillVehicle=[&](uint8* h){ At<int32>(h,0)=next()%5==0 ? 38 : next()%4==0 ? int32(162+next()%13) : int32(next()%210);
                            At<float>(h,4)=uni(100.f,20000.f); At<float>(h,0xC)=uni(100.f,50000.f); At<float>(h,0x10)=next()%4==0 ? uni(0.f,0.0099f) : (next()%10==0 ? 0.01f : uni(0.01f,20.f));
                            h[0x20]=uint8(10+next()%111); At<float>(h,0x28)=uni(0.5f,2.f); h[0x74]="FR4"[next()%3]; h[0x75]="PDE"[next()%3]; h[0x76]=uint8(1+next()%5);
                            At<float>(h,0x84)=uni(20.f,350.f); At<float>(h,0x7C)=uni(1.f,40.f)*0.4f; At<float>(h,0x80)=uni(1.f,50.f); At<float>(h,0x94)=uni(1.f,15.f);
                            At<float>(h,0xC8)=uni(0.1f,1.5f); At<uint32>(h,0xD0)=(next()&0x00FFFFFFu)|(next()%3==0 ? 0x01000000u : 0u)|(next()&0xFE000000u); };
                        if(nm=="ConvertDataToGameUnits"||nm=="ConvertDataToWorldUnits"){ uint8* h=mgr.data()+0x14+(next()%210)*0xE0; fillVehicle(h);
                            if(nm=="ConvertDataToWorldUnits"){ At<float>(h,0x7C)=uni(0.f,0.02f); At<float>(h,0x84)=uni(0.f,1.5f); At<float>(h,0x94)=uni(0.f,0.01f); }
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,h); out["rec"]=ReversingProbe::Bytes(h,0xE0); }
                        else if(nm=="ConvertBikeDataToGameUnits"||nm=="ConvertBikeDataToWorldUnits"){ uint8* h=mgr.data()+0x8F54+(next()%13)*0x40; fillWords(h,0x40);
                            const bool game=nm=="ConvertBikeDataToGameUnits"; for(size_t o:{0x14u,0x18u,0x2Cu,0x30u})At<float>(h,o)=game ? uni(-90.f,90.f) : uni(-0.99f,0.99f);
                            if(game&&next()%8==0)At<float>(h,0x14)=next()%2 ? 90.f : 0.f;
                            if(!game){ // _CIasin fuera de |x|<1: +-1, +-1.5, +-2, +-1e30, NaN, +-inf, +-0, subnormales y vecinos de 1 (X87Math.cpp ExeAsinDoubleCpp)
                                static const uint32 asinEdge[]{0x3F800000u,0xBF800000u,0x3FC00000u,0xBFC00000u,0x40000000u,0xC0000000u,0x7149F2CAu,0xF149F2CAu,0x7FC00000u,0xFFC00000u,0x7F800000u,0xFF800000u,
                                    0x00000000u,0x80000000u,0x00000001u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x00800000u,0x3F800001u,0x3F7FFFFFu,0xBF800001u,0xBF7FFFFFu,0x7F7FFFFFu,0xFF7FFFFFu,0x7F800001u,0x7FA00000u};
                                for(size_t o:{0x14u,0x18u,0x2Cu,0x30u})if(next()%3==0)At<uint32>(h,o)=asinEdge[next()%std::size(asinEdge)]; }
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,h); out["rec"]=ReversingProbe::Bytes(h,0x40); }
                        else if(nm=="FindExactWord"){ static const char* words[]{"BIKE","BMX","LANDSTAL","CAR","BIKEX","landstal","BOAT","SPEEDER","RCBANDIT","RC","ZZZ",""};
                            const uint32 size=8+next()%10, count=1+next()%20; std::memset(nameTable.data(),0,nameTable.size());
                            for(uint32 k=0;k<count;++k){ const char* w=words[next()%std::size(words)]; std::memcpy(nameTable.data()+k*size,w,std::min<size_t>(std::strlen(w),size-1)); }
                            const char* q=words[next()%(std::size(words)-1)]; uint32 cnt=next()%6==0 ? 0 : count; if(next()%20==0)cnt=0xFFFFFFFFu;
                            ret=reinterpret_cast<int32(__thiscall*)(void*,const char*,const char*,uint32,uint32)>(a)(self,q,nameTable.data(),size,cnt); }
                        else if(nm=="GetHandlingId"){ const auto* names=reinterpret_cast<const char(*)[14]>(VEH_NAMES); std::string q=names[next()%210];
                            switch(next()%5){ case 0: q+="X"; break; case 1: q=q.substr(0,q.size()/2); break; case 2: for(auto& ch:q)ch=char(std::tolower(ch)); break; default: break; }
                            if(next()%10==0)q="NOSUCHCAR";
                            ret=reinterpret_cast<int32(__thiscall*)(void*,const char*)>(a)(self,q.c_str()); }
                        else if(nm=="GetBoatPointer"||nm=="GetFlyingPointer")ret=Id(reinterpret_cast<void*(__thiscall*)(void*,uint32)>(a)(self,next()%256|(next()&0xFFFFFF00u)));
                        else if(nm=="HasFrontWheelDrive"||nm=="HasRearWheelDrive"){ for(int k=0;k<256;++k)mgr[0x88+k*0xE0]=uint8(next()%4==0 ? next() : "FR4"[next()%3]);
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,uint32)>(a)(self,next()%256|(next()&0xFFFFFF00u)); }
                        else { // LoadHandlingData
                            // Los campos que una linea no escribe (linea de coche incompleta) quedan del relleno y pasan por
                            // ConvertDataToGameUnits: floats normales en [0.5, 1) para no generar denormales (el x87 a 24 bits
                            // redondea dos veces al guardarlos y SSE una sola: diferencia que no es del codigo probado)
                            { auto* w=reinterpret_cast<uint32*>(mgr.data()); for(size_t k=0;k<mgr.size()/4;++k)w[k]=0x3F000000u|(next()&0x007FFFFFu); }
                            lines.clear(); lineIdx=0; const auto* names=reinterpret_cast<const char(*)[14]>(VEH_NAMES);
                            auto num=[&](float lo,float hi,int dec){ char b[32]; std::snprintf(b,sizeof(b),"%.*f",dec,uni(lo,hi)); return std::string(b); };
                            auto name=[&](int lo,int hi){ return next()%12==0 ? std::string("NOSUCH") : std::string(names[lo+int(next()%uint32(hi-lo))]); };
                            const uint32 n=1+next()%7;
                            for(uint32 k=0;k<n;++k){ std::string s; const uint32 kind=next()%7;
                                if(kind==0)s=";comment "+num(0.f,9.f,0);
                                else if(kind==1){ s="!\t"+name(162,175); for(int t=0;t<15;++t)s+="\t"+num(-60.f,60.f,3); }
                                else if(kind==2){ s="$\t"+name(186,210); for(int t=0;t<21;++t)s+="\t"+num(-5.f,5.f,4); if(next()%4==0)s+="s"; }
                                else if(kind==3){ s="%\t"+name(175,187); for(int t=0;t<14;++t)s+="\t"+num(-3.f,3.f,3); }
                                else if(kind==4){ s="^\t"+std::to_string(next()%30)+"\t"+std::to_string(next()%12)+"\t"+std::to_string(next()%12);
                                    for(int t=0;t<18;++t)s+="\t"+std::to_string(next()%3==0 ? 1 : 0); for(int t=0;t<13;++t)s+="\t"+num(0.f,2.f,2); s+="\t"+std::to_string(next()%128); }
                                else { s=name(0,210)+"\t"+num(500.f,9000.f,1)+"\t"+num(500.f,40000.f,1)+"\t"+num(0.f,8.f,1)+"\t"+num(-1.f,1.f,2)+"\t"+num(-1.f,1.f,2)+"\t"+num(-1.f,1.f,2)
                                      +"\t"+std::to_string(20+next()%100)+"\t"+num(0.5f,1.5f,2)+"\t"+num(0.5f,1.f,2)+"\t"+num(0.3f,0.7f,2)+"\t"+std::to_string(1+next()%5)
                                      +"\t"+num(60.f,300.f,1)+"\t"+num(5.f,40.f,1)+"\t"+num(5.f,40.f,1)+"\t"+std::string(1,"FR4"[next()%3])+"\t"+std::string(1,"PDE"[next()%3])
                                      +"\t"+num(3.f,12.f,1)+"\t"+num(0.3f,0.7f,2)+"\t"+std::to_string(next()%2)+"\t"+num(25.f,45.f,1);
                                      for(int t=0;t<8;++t)s+="\t"+num(-1.f,2.f,2); s+="\t"+num(0.1f,1.f,2)+"\t"+std::to_string(next()%60000);
                                      char hx[24]; std::snprintf(hx,sizeof(hx),"\t%x\t%x",next(),next()); s+=hx; for(int t=0;t<3;++t)s+="\t"+std::to_string(next()%4); }
                                lines.push_back(s); }
                            if(next()%4==0)lines.push_back(";the end"); if(next()%6==0)lines.push_back(name(0,210)+"\t1000.0");
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                            json blocks=json::array(); for(size_t o=0;o<0xC700;o+=0x700)blocks.push_back(Fnv(mgr.data()+o,0x700)); out["blocks"]=blocks; out["lines"]=lineIdx; }
                        out["mgr"]=Fnv(mgr.data(),mgr.size());
                    }
                    else { // CEntryExit
                        static const char* enexNames[]{"bar1","BAR1","strip2","LAstrip","lastrip","MAFCAS","mafcas","TRICAS","POLICE1","police2","POLICE3","POLICE4","POLICE5","GYM1","","abcdefg"};
                        auto setupEnex=[&](uint8* e){ fillWords(e,0x40); std::memset(e,0,8); const char* nmz=enexNames[next()%std::size(enexNames)]; std::memcpy(e,nmz,std::strlen(nmz));
                            const float left=uni(-3000.f,3000.f), bottom=uni(-3000.f,3000.f);
                            At<float>(e,0x08)=left; At<float>(e,0x10)=left+(next()%10==0 ? -uni(0.f,5.f) : uni(0.f,30.f)); At<float>(e,0x14)=bottom; At<float>(e,0x0C)=bottom+(next()%10==0 ? -uni(0.f,5.f) : uni(0.f,30.f));
                            At<float>(e,0x18)=uni(-50.f,200.f); const uint32 am=next()%40; At<float>(e,0x1C)=am<13 ? 0.f : am==13 ? std::bit_cast<float>(0x7FC00000u) : uni(-6.3f,6.3f);
                            At<CVector>(e,0x20)=randVec(-3000.f,3000.f); At<float>(e,0x2C)=uni(-360.f,360.f); At<uint16>(e,0x30)=uint16(next());
                            e[0x32]=next()%3==0 ? 0 : (next()%8==0 ? uint8(0x80+next()%0x80) : uint8(1+next()%18)); e[0x33]=uint8(next()%4); e[0x34]=uint8(next()%24); e[0x35]=uint8(next()%24);
                            e[0x36]=next()%8==0 ? uint8(0x80+next()%0x80) : uint8(next()%20); e[0x37]=0; At<void*>(e,0x38)=nullptr; };
                        for(auto& e:enex)setupEnex(e.data());
                        for(auto& e:enex)if(next()%3==0)At<void*>(e.data(),0x38)=enex[next()%enex.size()].data();
                        uint8* const self=enex[0].data();
                        spawnPoint=enex[next()%enex.size()].data(); currArea=next()%3==0 ? 0 : int32(1+next()%18);
                        for(int k=0;k<4;++k)enexStack[k]=next()%5 ? static_cast<void*>(enex[next()%enex.size()].data()) : nullptr;
                        if(next()%3==0)enexStack[next()%4]=spawnPoint;
                        stackPosn=next()%10==0 ? int32(0x80000000u) : int32(next()%5)-(next()%8==0 ? 1 : 0); exitEnterState=int32(next()%4); warping=uint8(next()%2);
                        auto* outV=reinterpret_cast<CVector*>(outBuf.data()); fillWords(outBuf.data(),outBuf.size());
                        auto setupPed=[&](size_t k){ uint8* p=peds[k].data(); fillWords(p,0x7A0); At<void*>(p,0)=pedVt.data(); At<CVector>(p,4)=randVec(-3000.f,3000.f); At<void*>(p,0x14)=nullptr;
                            p[0x2F]=uint8(next()%19); fillWords(intel[k].data(),0x40); At<void*>(p,0x47C)=intel[k].data(); At<void*>(p,0x480)=playerData.data();
                            At<uint32>(p,0x46C)=next()&~0x100u; At<void*>(p,0x58C)=nullptr; At<void*>(p,0x78C)=next()%2 ? static_cast<void*>(enex[next()%enex.size()].data()) : nullptr; };
                        for(size_t k=0;k<peds.size();++k)setupPed(k);
                        uint8* const ped=peds[0].data();
                        if(nm=="IsInArea"){ const float l=At<float>(self,8), r=At<float>(self,0x10), b=At<float>(self,0x14), t=At<float>(self,0xC), z=At<float>(self,0x18);
                            CVector p{uni(l-10.f,r+10.f),uni(b-10.f,t+10.f),z+uni(-2.f,2.f)}; const uint32 m=next()%10;
                            if(m==0)p.x=l; else if(m==1)p.x=r; else if(m==2)p.y=b; else if(m==3)p.y=t; else if(m==4)p.z=z+(next()%2 ? 1.f : -1.f); else if(m==5)p.z=std::nextafter(z+1.f,0.f);
                            if(At<float>(self,0x1C)!=0.f&&next()%2){ const float cx=(r-l)*0.5f+l, cy=(t-b)*0.5f+b; p.x=cx+uni(-20.f,20.f); p.y=cy+uni(-20.f,20.f); }
                            *outV=p; ret=reinterpret_cast<uint8(__thiscall*)(void*,const CVector*)>(a)(self,outV); }
                        else if(nm=="GetEntryExitToDisplayNameOf")ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(enex[next()%enex.size()].data()));
                        else if(nm=="GetPositionRelativeToOutsideWorld"){ *outV=randVec(-3000.f,3000.f); const uint32 edx=CallKeepEdx(a,self,outV); out["edx"]=edx; out["v"]=Vec(*outV); }
                        else if(nm=="RequestObjectsInFrustum")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="RequestAmbientPeds")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="GenerateAmbientPeds"){ *reinterpret_cast<uint8*>(0xC0FCB6)=uint8(next()); *reinterpret_cast<int32*>(0x8D253C)=int32(next());
                            *outV=randVec(-3000.f,3000.f); reinterpret_cast<void(__thiscall*)(void*,const CVector*)>(a)(self,outV);
                            out["police"]=*reinterpret_cast<uint8*>(0xC0FCB6); out["inUse"]=*reinterpret_cast<int32*>(0x8D253C); }
                        else if(nm=="ProcessStealableObjects"){ fillWords(fakeObj.data(),fakeObj.size()); fakeObj[0x36]=uint8((next()&0xF8)|(next()%4 ? 4 : next()%8));
                            At<uint32>(fakeObj.data(),0x140)=next()%4 ? (next()|0x2000u) : (next()&~0x2000u); fakeObj[0x13C]=uint8(next()%4==0 ? 2 : next()%4==0 ? 6 : next()%8); fakeObj[0x2F]=uint8(next()%19);
                            heldResult=next()%6 ? static_cast<void*>(fakeObj.data()) : nullptr; stealableResult=next()%3==0 ? -1 : int32(next()%30);
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,ped); out["obj"]=ReversingProbe::Bytes(fakeObj.data(),0x40); }
                        else if(nm=="FindValidTeleportPoint"){ sphereMask=next()%4==0 ? 0 : (next()|1u); losMask=next(); if(next()%5==0){ sphereMask=0xFFFFFFFFu; losMask=0; }
                            reinterpret_cast<void(__thiscall*)(void*,CVector*)>(a)(self,outV); out["v"]=Vec(*outV); }
                        else if(nm=="WarpGangWithPlayer"){ fillWords(playerData.data(),playerData.size()); const uint32 gid=next()%8; At<uint32>(playerData.data(),0x38)=gid;
                            auto* active=reinterpret_cast<uint8*>(0xC098E0); for(int k=0;k<8;++k)active[k]=uint8(next()%3==0 ? 0 : 1+next()%2); if(next()%6==0)active[gid]=0;
                            leaderResult=next()%6!=0; for(int k=0;k<8;++k){ const uint32 m=next()%8; memberTable[k]=m==0 ? nullptr : m==1 ? static_cast<void*>(ped) : static_cast<void*>(peds[1+k%7].data()); }
                            sphereMask=next()%4==0 ? 0 : next()&next(); losMask=next()%5==0 ? 0 : next()|next(); if(next()%2){ fillMatrix(pedMat.data(),randVec(-3000.f,3000.f)); At<void*>(ped,0x14)=pedMat.data(); }
                            for(size_t k=1;k<peds.size();++k)At<CVector>(peds[k].data(),4)=At<CVector>(ped,4)+randVec(-10.f,10.f);
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,ped);
                            json pj=json::array(); for(auto& p:peds)pj.push_back(json::array({ReversingProbe::Bytes(p.data(),0x30),At<uint32>(p.data(),0x558),At<uint32>(p.data(),0x55C),Id(At<void*>(p.data(),0x78C))})); out["peds"]=pj; }
                        else if(nm=="TransitionStarted"){ exitEnterState=next()%4==0 ? int32(1+next()%3) : 0; timeInRange=next()%5!=0;
                            At<uint16>(self,0x30)=uint16(next()|(next()%5 ? 0x4000u : 0u));
                            if(next()%2){ At<uint32>(ped,0x46C)|=0x100u; fillWords(fakeVeh.data(),fakeVeh.size()); fillWords(fakeHandling.data(),fakeHandling.size()); At<void*>(ped,0x58C)=fakeVeh.data();
                                At<void*>(fakeVeh.data(),0x384)=fakeHandling.data(); fakeHandling[0xCC]=uint8(next()%4==0 ? 8 : next()&~8u); const uint32 vt=next()%4; At<int32>(fakeVeh.data(),0x590)=vt==0 ? 0 : vt==1 ? 9 : int32(next()%12); }
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,void*)>(a)(self,ped); }
                        else { // TransitionFinished: solo caminos que vuelven con false antes de la carga de la escena
                            if(next()%2)At<uint32>(ped,0x46C)|=0x100u;
                            const bool pathA=(At<uint16>(self,0x30)&0x202)||(At<uint32>(ped,0x46C)&0x100u);
                            exitEnterState=pathA ? (next()%2 ? 0 : 2) : int32(next()%3); fadingResult=pathA ? next()%2!=0 : (exitEnterState==2 ? true : next()%2!=0);
                            heldResult=next()%3 ? static_cast<void*>(fakeObj.data()) : nullptr; fillWords(fakeObj.data(),fakeObj.size());
                            fillWords(fakeTask.data(),fakeTask.size()); At<void*>(fakeTask.data(),0)=taskVt.data(); At<void*>(intel[0].data(),0x10)=next()%4 ? static_cast<void*>(fakeTask.data()) : nullptr;
                            taskTypeResult=next()%2 ? 932 : int32(next()%1000); *reinterpret_cast<uint8*>(0xB76850)=uint8(next()%2);
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,void*)>(a)(self,ped);
                            out["obj_area"]=fakeObj[0x2F]; out["loading"]=*reinterpret_cast<uint8*>(0xB76850); }
                        out["statics"]=ReversingProbe::Bytes(reinterpret_cast<void*>(ENEX_STATICS),ENEX_STATICS_SIZE); out["area"]=currArea;
                        json ej=json::array(); for(auto& e:enex)ej.push_back(Fnv(e.data(),0x40)); out["enex"]=ej; out["out"]=ReversingProbe::Bytes(outBuf.data(),0x10);
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
    for(auto& [h,en,lk]:extra){ h->State(en); h->LockState(lk); }
    ps2Coronas=ps2Saved;
    RestoreAll();
    bool restored=rsSlot==rsSaved; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
