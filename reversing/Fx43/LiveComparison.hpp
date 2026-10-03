#pragma once
// Banco dentro del motor: audio del clima, efectos, fuego, entrada y JPEG (lista de 43; 44 funciones). CAEWeatherAudioEntity,
// CBulletTraces, CFire, CMovingThings, CShotInfo, CWaterCannon, FxMemoryPool_c, Input (app_input) y JPegPlugin. Original
// (hooks del grupo desactivados) frente a candidato con fixtures deterministas sobre objetos falsos. Se simulan y trazan:
// rand y los aleatorios de audio, FindPlayerPed/FindPlayerVehicle, el gestor de sonidos (CAESound::Initialise,
// RequestNewSound, bancos), los CAETwinLoopSoundEntity de lluvia, CCullZones, lineas de vision, estados de render e Im3D,
// el gestor de efectos (CreateFxSystem/Kill/Play/SetOffsetPos/SetConstTime/AddParticle sobre sistemas falsos), StartFire,
// ExtinguishPointWithWater, CPointLights, CEscalators/CPlaneTrails, CWeaponInfo, eventos, CMemoryMgr (Malloc/Free/Realloc
// sobre buferes falsos), RsInputDeviceAttach, CControllerConfigManager, CFileMgr y la libjpeg (la del EXE y la de la DLL,
// cada una con su disposicion de jpeg_compress_struct/jpeg_decompress_struct). Los pools de peatones, vehiculos y
// objetos se sustituyen por pools falsos. Todo el estado global tocado se guarda y se restaura en cada ejecucion.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "AEWeatherAudioEntity.h"
#include "AETwinLoopSoundEntity.h"
#include "AEWaterCannonAudioEntity.h"
#include "AESound.h"
#include "AEAudioUtility.h"
#include "AESoundManager.h"
#include "AEAudioHardware.h"
#include "AudioEngine.h"
#include "BulletTraces.h"
#include "BulletTrace.h"
#include "Fire.h"
#include "FireManager.h"
#include "CreepingFire.h"
#include "MovingThings.h"
#include "PlaneTrails.h"
#include "Escalators.h"
#include "Escalator.h"
#include "PointLights.h"
#include "ShotInfo.h"
#include "WaterCannon.h"
#include "FxMemoryPool.h"
#include "FxManager.h"
#include "FxSystem.h"
#include "EventGroup.h"
#include "CullZones.h"
#include "WeaponInfo.h"
#include "Weapon.h"
#include "MemoryMgr.h"
#include "FileMgr.h"
#include "ControllerConfigManager.h"
#include "app_input.h"
#include "platform.h"
#include "Plugins/JPegCompressPlugin/JPegCompress.h"
#include "jpeglib.h"
#include "../ProbeRuntime.hpp"

namespace Fx43Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> inline T& G(uintptr_t a){ return *reinterpret_cast<T*>(a); }
inline uint32 maskA{}, callsA{}, maskB{}, callsB{}, maskC{}, callsC{};
inline bool BitA(){ return (maskA>>(callsA++%32))&1; }
inline bool BitB(){ return (maskB>>(callsB++%32))&1; }
inline bool BitC(){ return (maskC>>(callsC++%32))&1; }

// --- direcciones del EXE ---
constexpr uintptr_t RAIN_L=0xB6BB18, RAIN_R=0xB6BBC0, TRACES=0xC7C748, FIRES=0xB71F80, SHOTS=0xC89690, RANDTAB=0xC89628;
constexpr uintptr_t CAM_MAT=0xB6F99C, SCREEN_BUF=0xBD0B78, SCAN_BUF=0xBD0170, WC_VERTS=0xC80550, WC_IDX=0xC80700;
constexpr int32 MI=596; // entrada de CModelInfo::ms_modelInfoPtrs sustituida temporalmente

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<std::array<uint8,0xA18>,6> ent{};            // peds/vehiculos/objetos sueltos
alignas(16) inline std::array<std::array<uint8,0x60>,8> mats{};            // CMatrix(Link) falsas
alignas(16) inline std::array<uint8,0x60> camMat{};
alignas(16) inline std::array<std::array<uint8,0x200>,2> intel{};
alignas(16) inline std::array<uint8,0x80> weather{};
alignas(16) inline std::array<uint8,0x80> snd{};
alignas(16) inline std::array<std::array<uint8,0x28>,2> fire{};
alignas(16) inline std::array<std::array<uint8,0x100>,3> fx{};
alignas(16) inline std::array<uint8,0x80> weapInfo{};
alignas(16) inline std::array<uint8,0x400> cannon{};
alignas(16) inline std::array<uint8,0x60> modelInfo{};
alignas(16) inline std::array<uint8,0x40> vehStruct{};
alignas(16) inline std::array<std::array<uint8,0x14>,3> pools{};
alignas(16) inline std::array<uint8,3*0xA18> vehStore{};
alignas(16) inline std::array<uint8,4*0x7C4> pedStore{};
alignas(16) inline std::array<uint8,3*0x19C> objStore{};
alignas(16) inline std::array<std::array<uint8,8>,3> poolFlags{};
alignas(16) inline std::array<uint8,0x40> memPool{};
alignas(16) inline std::array<uint8,0x40> smallBuf{};
alignas(16) inline std::array<uint8,0x100> outVec{};
alignas(16) inline std::array<uint8,0x40> ksBuf{};
alignas(16) inline std::array<uint8,0x40> image{}, raster{}, camera{}, dstMgr{}, srcMgr{};
alignas(16) inline std::array<uint8,64*16*4> pixels{};
alignas(16) inline std::array<uint8,0x400> cinfoFake{};
alignas(16) inline std::array<uint8,0x40> handle{};
alignas(16) inline std::array<int8,0x100> volTable{};
alignas(16) inline std::array<uint8,0x100000> bigBuf{};                      // Malloc / buffer de salida JPEG / pixeles
alignas(16) inline std::array<uint8,3*0x32000> gallery{};
inline std::array<void*,64> vehVt{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }
inline json Vec(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }
inline json Vp(const void* p){ const auto* f=static_cast<const float*>(p); return json::array({F(f[0]),F(f[1]),F(f[2])}); }

// --- comunes ---
inline void* playerPed{}; inline void* playerVeh{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32){ return static_cast<CPlayerPed*>(playerPed); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32,bool){ return static_cast<CVehicle*>(playerVeh); }
inline RwBool __cdecl StateSet(RwRenderState st,void* v){ trace.push_back({"render_state_set",int(st),uintptr_t(v)}); return TRUE; }
inline bool im3dOk{};
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){ json vs=json::array(); for(RwUInt32 k=0;k<n&&k<32;++k){ const auto* w=reinterpret_cast<const uint32*>(static_cast<uint8*>(v)+k*0x24); vs.push_back({w[0],w[1],w[2],w[6]}); }
    trace.push_back({"im3d_transform",intptr_t(v)==intptr_t(WC_VERTS) ? json("wc_verts") : json("stack"),n,uintptr_t(m),f,vs}); return im3dOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ json is=json::array(); for(RwInt32 k=0;k<n&&k<64;++k)is.push_back(idx[k]); trace.push_back({"im3d_render_indexed",int(t),n,is}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }
inline bool losHit{}; inline void* losEntity{}; inline CVector losPoint{}, losNormal{};
inline bool __cdecl FakeLos(const CVector& o,const CVector& t,CColPoint& cp,CEntity*& e,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){
    const bool r=BitB(); trace.push_back({"line_of_sight",Vec(o),Vec(t),b0,b1,b2,b3,b4,b5,b6,b7,r});
    if(r){ cp.m_vecPoint=losPoint; cp.m_vecNormal=losNormal; e=static_cast<CEntity*>(losEntity); } return r; }
inline void __fastcall FakeReportFrontend(void* self,void*,int32 ev,float vol,float speed){ trace.push_back({"report_frontend",intptr_t(self)-0xB6BC90,ev,F(vol),F(speed)}); }
inline void* startFireResult{};
inline CFire* __fastcall FakeStartFireEnt(void* self,void*,CEntity* target,CEntity* creator,float size,uint8 a3,uint32 time,int8 gens){
    trace.push_back({"start_fire",intptr_t(self)-intptr_t(FIRES),Id(target),Id(creator),F(size),a3,time,gens}); return static_cast<CFire*>(startFireResult); }
inline bool __fastcall FakeExtinguishPoint(void* self,void*,CVector p,float r,float s){ const bool b=BitA(); trace.push_back({"extinguish_point",intptr_t(self)-intptr_t(FIRES),Vec(p),F(r),F(s),b}); return b; }
inline void __fastcall FakeAddParticle(void* self,void*,const CVector& pos,const CVector& vel,float t,const FxPrtMult_c& m,float rz,float lm,float ll,bool local){
    trace.push_back({"add_particle",Id(self),Vec(pos),Vec(vel),F(t),ReversingProbe::Bytes(&m,0x1C),F(rz),F(lm),F(ll),local}); }

// --- audio ---
inline int16 __fastcall FakeAreSounds(void* self,void*,int32 ev,void* e){ const int16 r=BitA() ? int16(1+BitA()) : 0; trace.push_back({"are_sounds_playing",intptr_t(self)-0xB62CB0,int16(ev),Id(e),r}); return r; }
inline bool __fastcall FakeBankLoaded(void* self,void*,int32 bank,int32 slot){ const bool r=BitB(); trace.push_back({"bank_loaded",intptr_t(self)-0xB5F8B8,int16(bank),int16(slot),r}); return r; }
inline void __fastcall FakeLoadBank(void* self,void*,int32 bank,int32 slot){ trace.push_back({"load_bank",intptr_t(self)-0xB5F8B8,int16(bank),int16(slot)}); }
inline void __fastcall FakeSoundInit(void* self,void*,int32 slot,int32 sfx,void* e,CVector pos,float vol,float roll,float speed,float doppler,uint32 delay,uint32 flags,float var,int32 play){
    trace.push_back({"sound_init",int16(slot),int16(sfx),Id(e),Vec(pos),F(vol),F(roll),F(speed),F(doppler),uint8(delay),uint16(flags),F(var),int16(play)});
    At<int16>(self,0)=int16(slot); At<int16>(self,2)=int16(sfx); At<void*>(self,4)=e; At<int32>(self,0xC)=-1; At<float>(self,0x10)=-1.f; At<uint16>(self,0x56)=uint16(flags); }
inline CAESound* __fastcall FakeRequestSound(void* self,void*,CAESound* s){
    trace.push_back({"request_sound",intptr_t(self)-0xB62CB0,At<int16>(s,0),At<int16>(s,2),Id(At<void*>(s,4)),At<int32>(s,0xC),F(At<float>(s,0x10)),At<uint16>(s,0x56)}); return nullptr; }
inline json Twin(const void* p){ return uintptr_t(p)==RAIN_L ? json("L") : uintptr_t(p)==RAIN_R ? json("R") : Id(p); }
inline void __fastcall FakeTwinInit(void* self,void*,int32 slot,int32 a,int32 b,void* parent,uint32 mn,uint32 mx,int32 pa,int32 pb){
    trace.push_back({"twin_init",Twin(self),int16(slot),int16(a),int16(b),Id(parent),uint16(mn),uint16(mx),int16(pa),int16(pb)}); }
inline void __fastcall FakeTwinStop(void* self,void*){ trace.push_back({"twin_stop",Twin(self)}); }
inline void __fastcall FakeTwinUpdate(void* self,void*,CVector p,float v,float s){ trace.push_back({"twin_update",Twin(self),Vec(p),F(v),F(s)}); }
inline bool __fastcall FakeTwinSwitch(void* self,void*){ const bool r=BitC(); trace.push_back({"twin_switch",Twin(self),r}); return r; }
inline void __fastcall FakeTwinPlay(void* self,void*,CVector p,float v,float s,float roll,float dop,int32 flags){ trace.push_back({"twin_play",Twin(self),Vec(p),F(v),F(s),F(roll),F(dop),flags}); }
inline uint32 rangeCalls{};
inline float __cdecl FakeRange(float a,float b){ const float r=a+(b-a)*float((seed*7u+rangeCalls++*13u)%101u)/100.f; trace.push_back({"random_range",F(a),F(b),F(r)}); return r; }
inline bool __cdecl FakeProb(float p){ const bool r=BitC(); trace.push_back({"resolve_probability",F(p),r}); return r; }
inline bool __cdecl FakePlayerNoRain(){ const bool r=BitA(); trace.push_back({"player_no_rain",r}); return r; }
inline bool __cdecl FakeCamNoRain(){ const bool r=BitA(); trace.push_back({"cam_no_rain",r}); return r; }
inline float __cdecl FakePiecewise(float x,int32 n,const float (*d)[2]){ const float r=x*0.5f+float(uintptr_t(d)&0xFF)/64.f; trace.push_back({"piecewise",F(x),int16(n),uintptr_t(d),F(r)}); return r; }
inline void __fastcall FakeSoundStop(void* self,void*){ trace.push_back({"sound_stop_and_forget",Id(self)}); }

// --- efectos / fuego ---
inline void* fxResult{};
inline FxSystem_c* __fastcall FakeCreateFx(void* self,void*,const char* name,const CVector& p,RwMatrix* m,bool ignore){
    trace.push_back({"create_fx",intptr_t(self)-0xA9AE80,std::string(name),Vec(p),Id(m),ignore,Id(fxResult)}); return static_cast<FxSystem_c*>(fxResult); }
inline void __fastcall FakeFxKill(void* self,void*){ trace.push_back({"fx_kill",Id(self)}); }
inline void __fastcall FakeFxPlay(void* self,void*){ trace.push_back({"fx_play",Id(self)}); }
inline void __fastcall FakeFxOffset(void* self,void*,const CVector& p){ trace.push_back({"fx_offset",Id(self),Vec(p)}); }
inline void __fastcall FakeFxConstTime(void* self,void*,bool on,float t){ trace.push_back({"fx_const_time",Id(self),on,F(t)}); }
inline void __fastcall FakeCleanUp(void* self,void*,CEntity** ref){ trace.push_back({"clean_up_ref",Id(self),Id(ref)}); }
inline void __fastcall FakeInflict(void* self,void*,CEntity* d,int32 w,float i,CVector c){ trace.push_back({"inflict_damage",Id(self),Id(d),w,F(i),Vec(c)}); }
inline void __fastcall FakeGoOnFire(void* self,void*){ trace.push_back({"go_on_fire",Id(self)}); }
inline int32 __fastcall FakeNearestTyre(void* self,void*,CVector2D p){ const int32 r=int32((F(p.x)^F(p.y))>>7)&3; trace.push_back({"nearest_tyre",Id(self),F(p.x),F(p.y),r}); return r; }
inline bool __fastcall FakeBurstTyre(void* self,void*,uint8 c,bool phys){ trace.push_back({"burst_tyre",Id(self),c,phys}); return false; }
inline void __fastcall FakeVTrap(void* self,void*){ trace.push_back({"vtrap",Id(self)}); }
inline void __fastcall FakeObjFireDamage(void* self,void*,float d,CEntity* e){ trace.push_back({"object_fire_damage",Id(self),F(d),Id(e)}); }
inline bool __cdecl FakeTryFire(CVector p,uint8 gens,bool a5,bool script,float dist){ trace.push_back({"try_start_fire",Vec(p),gens,a5,script,F(dist)}); return false; }
inline void __cdecl FakeAddLight(uint8 type,CVector p,CVector d,float r,float red,float green,float blue,uint8 fog,bool shadows,CEntity* e){
    trace.push_back({"add_light",type,Vec(p),Vec(d),F(r),F(red),F(green),F(blue),fog,shadows,Id(e)}); }
inline void __cdecl FakeLightsInit(){ trace.push_back({"point_lights_init"}); }
inline void __cdecl FakeEscInit(){ trace.push_back({"escalators_init"}); }
inline void __cdecl FakeEscUpdate(){ trace.push_back({"escalators_update"}); }
inline void __fastcall FakeEscSwitchOff(void* self,void*){ trace.push_back({"escalator_switch_off",intptr_t(self)-0xC6E9A8}); }
inline void __cdecl FakeTrailsUpdate(){ trace.push_back({"plane_trails_update"}); }
inline void __cdecl FakeTrailsRender(){ trace.push_back({"plane_trails_render"}); }

// --- disparos ---
inline CWeaponInfo* __cdecl FakeWeaponInfo(int32 type,int32 skill){ trace.push_back({"weapon_info",type,skill}); return reinterpret_cast<CWeaponInfo*>(weapInfo.data()); }
inline bool __fastcall FakePtrValid(void* self,void*){ const bool r=BitA(); trace.push_back({"pointer_valid",Id(self),r}); return r; }
inline bool __fastcall FakeInControl(void* self,void*){ const bool r=BitB(); trace.push_back({"in_control",Id(self),r}); return r; }
inline int32 __fastcall FakeLocalDir(void* self,void*,const CVector2D& p){ const int32 r=int32(F(p.x)>>9)&3; trace.push_back({"local_direction",Id(self),F(p.x),F(p.y),r}); return r; }
inline bool __cdecl FakeDamageEvent(CPed* v,CEntity* c,int32 w,int32 dmg,int32 piece,uint32 dir){ trace.push_back({"damage_event",Id(v),Id(c),w,dmg,piece,uint8(dir)}); return true; }
inline int32 sprayState{}; inline CVector sprayDir{};
inline int32 __cdecl FakeSpray(CVector& p,CVector& out,float r,bool b){ trace.push_back({"spray_paint",Vec(p),F(r),b,sprayState}); out=sprayDir; return sprayState; }
inline void __cdecl FakeCarsOnFire(CVector p,float r,CEntity* e){ trace.push_back({"cars_on_fire",Vec(p),F(r),Id(e)}); }
inline void __cdecl FakeWorldOnFire(CVector p,float r,CEntity* e){ trace.push_back({"world_on_fire",Vec(p),F(r),Id(e)}); }

// --- caniones de agua ---
inline void __fastcall FakeWcAudioInit(void* self,void*,CWaterCannon* c){ trace.push_back({"wc_audio_init",Id(self),Id(c)}); }
inline void __fastcall FakeWcClearSplash(void* self,void*){ trace.push_back({"wc_clear_splash",Id(self)}); }
inline void __fastcall FakeWcSetSplash(void* self,void*,CVector p,float m){ trace.push_back({"wc_set_splash",Id(self),Vec(p),F(m)}); }
inline CEvent* __fastcall FakeEventAdd(void* self,void*,CEvent* e,bool valid){ trace.push_back({"event_add",Id(self),ReversingProbe::Bytes(reinterpret_cast<uint8*>(e)+0xC,0x18),valid}); return nullptr; }
inline void __fastcall FakeMoveForce(void* self,void*,CVector f){ trace.push_back({"apply_move_force",Id(self),Vec(f)}); }

// --- memoria ---
inline void* mallocResult{};
inline void* __cdecl FakeMalloc(uint32 n){ trace.push_back({"malloc",n}); return mallocResult; }
inline void __cdecl FakeFree(void* p){ trace.push_back({"free",Id(p)}); }
inline void* __cdecl FakeRealloc(void* p,uint32 n){ trace.push_back({"realloc",Id(p),n}); return smallBuf.data(); }

// --- entrada ---
inline std::vector<std::pair<int32,void*>> attached;
inline int32 __cdecl FakeAttach(int32 type,void* h){ attached.emplace_back(type,h); trace.push_back({"device_attach",type}); return 1; }
inline void __fastcall FakeJoyState(void* self,void*,int32 pad){ trace.push_back({"joy_state",intptr_t(self)-0xB70198,pad}); }
inline void __fastcall FakeJoyCfgDown(void* self,void*,int32 b,int32 pad){ trace.push_back({"joy_cfg_down",intptr_t(self)-0xB70198,b,pad}); }
inline void __fastcall FakeJoyDown(void* self,void*,int32 b,int32 type){ trace.push_back({"joy_down",intptr_t(self)-0xB70198,b,type}); }
inline void __fastcall FakeJoyCfgUp(void* self,void*,int32 b,int32 pad){ trace.push_back({"joy_cfg_up",intptr_t(self)-0xB70198,b,pad}); }
inline void __fastcall FakeJoyUp(void* self,void*,int32 b,int32 type){ trace.push_back({"joy_up",intptr_t(self)-0xB70198,b,type}); }

// --- JPEG ---
inline void* __cdecl FakeGrab(void* cam){ trace.push_back({"grab_screen",Id(cam)}); return image.data(); }
inline RwBool __cdecl FakeImageDestroy(RwImage* i){ trace.push_back({"image_destroy",Id(i)}); return TRUE; }
inline void* __cdecl FakeStdError(void* err){ trace.push_back({"std_error"}); return err; }
inline json ColorSpace(int32 v,bool exe){ return (exe ? v==2 : v==int32(JCS_EXT_RGBA)) ? json("rgba") : json(v); } // 2 (JCS_RGB) en el EXE, JCS_EXT_RGBA en libjpeg-turbo
inline uint32 scanMask{}, scanCalls{}; inline bool ScanOk(){ return (scanMask>>(scanCalls++%32))&1; }
// libjpeg del EXE (jpeg_compress_struct: dest 0x18, ancho 0x1C, alto 0x20, componentes 0x24, espacio 0x28, dct 0xB8, next_scanline 0xD0)
inline void __cdecl FakeCreateCompressE(void* c,int32,uint32){ trace.push_back({"create_compress"}); At<uint32>(c,0xD0)=0; }
inline void __cdecl FakeSetDefaultsE(void* c){ trace.push_back({"set_defaults",ColorSpace(At<int32>(c,0x28),true)}); }
inline void __cdecl FakeStartCompressE(void* c,int32 all){ trace.push_back({"start_compress",At<uint32>(c,0x1C),At<uint32>(c,0x20),At<int32>(c,0x24),At<int32>(c,0xB8),Id(At<void*>(c,0x18)),uint8(all)}); }
inline uint32 __cdecl FakeWriteScanE(void* c,uint8** lines,uint32 n){ const uint32 r=ScanOk(); trace.push_back({"write_scanlines",Id(lines[0]),n,r}); if(r)At<uint32>(c,0xD0)+=1; return r; }
inline void __cdecl FakeFinishCompressE(void*){ trace.push_back({"finish_compress"}); }
inline void __cdecl FakeDestroyCompressE(void*){ trace.push_back({"destroy_compress"}); }
// libjpeg-turbo de la DLL
inline void __cdecl FakeCreateCompressD(j_compress_ptr c,int32,size_t){ trace.push_back({"create_compress"}); c->next_scanline=0; }
inline void __cdecl FakeSetDefaultsD(j_compress_ptr c){ trace.push_back({"set_defaults",ColorSpace(int32(c->in_color_space),false)}); }
inline void __cdecl FakeStartCompressD(j_compress_ptr c,boolean all){ trace.push_back({"start_compress",uint32(c->image_width),uint32(c->image_height),int32(c->input_components),int32(c->dct_method),Id(c->dest),uint8(all)}); }
inline JDIMENSION __cdecl FakeWriteScanD(j_compress_ptr c,JSAMPARRAY lines,JDIMENSION n){ const uint32 r=ScanOk(); trace.push_back({"write_scanlines",Id(lines[0]),uint32(n),r}); if(r)c->next_scanline+=1; return r; }
inline void __cdecl FakeFinishCompressD(j_compress_ptr){ trace.push_back({"finish_compress"}); }
inline void __cdecl FakeDestroyCompressD(j_compress_ptr){ trace.push_back({"destroy_compress"}); }
// descompresion (EXE: src 0x18, dct 0x44, output_height 0x60, output_scanline 0x78)
inline uint32 outHeight{}; inline int32 headerResult{};
inline void FillScan(uint32 row){ auto* s=reinterpret_cast<uint8*>(SCAN_BUF); for(uint32 k=0;k<0x900;++k)s[k]=uint8(k*7u+row*31u+seed); }
inline void __cdecl FakeCreateDecompressE(void* c,int32,uint32){ trace.push_back({"create_decompress"}); At<uint32>(c,0x78)=0; At<uint32>(c,0x60)=outHeight; }
inline int32 __cdecl FakeReadHeaderE(void* c,int32 req){ trace.push_back({"read_header",uint8(req),Id(At<void*>(c,0x18)),headerResult}); return headerResult; }
inline uint8 __cdecl FakeStartDecompressE(void* c){ const bool r=BitA(); trace.push_back({"start_decompress",At<int32>(c,0x44),r}); return r; }
inline uint32 __cdecl FakeReadScanE(void* c,uint8** s,uint32 n){ const uint32 r=ScanOk(); trace.push_back({"read_scanlines",intptr_t(s[0])-intptr_t(SCAN_BUF),n,r}); if(r){FillScan(At<uint32>(c,0x78));At<uint32>(c,0x78)+=1;} return r; }
inline uint8 __cdecl FakeFinishDecompressE(void*){ trace.push_back({"finish_decompress"}); return 1; }
inline void __cdecl FakeDestroyDecompressE(void*){ trace.push_back({"destroy_decompress"}); }
inline void __cdecl FakeCreateDecompressD(j_decompress_ptr c,int32,size_t){ trace.push_back({"create_decompress"}); c->output_scanline=0; c->output_height=outHeight; }
inline int __cdecl FakeReadHeaderD(j_decompress_ptr c,boolean req){ trace.push_back({"read_header",uint8(req),Id(c->src),headerResult}); return headerResult; }
inline boolean __cdecl FakeStartDecompressD(j_decompress_ptr c){ const bool r=BitA(); trace.push_back({"start_decompress",int32(c->dct_method),r}); return r; }
inline JDIMENSION __cdecl FakeReadScanD(j_decompress_ptr c,JSAMPARRAY s,JDIMENSION n){ const uint32 r=ScanOk(); trace.push_back({"read_scanlines",intptr_t(s[0])-intptr_t(SCAN_BUF),uint32(n),r}); if(r){FillScan(c->output_scanline);c->output_scanline+=1;} return r; }
inline boolean __cdecl FakeFinishDecompressD(j_decompress_ptr){ trace.push_back({"finish_decompress"}); return TRUE; }
inline void __cdecl FakeDestroyDecompressD(j_decompress_ptr){ trace.push_back({"destroy_decompress"}); }
inline RwUInt8* __cdecl FakeRasterLock(RwRaster* r,RwUInt8 level,RwInt32 mode){ trace.push_back({"raster_lock",Id(r),level,mode}); return bigBuf.data(); }
inline RwRaster* __cdecl FakeRasterUnlock(RwRaster* r){ trace.push_back({"raster_unlock",Id(r)}); return r; }
inline RwUInt32 __cdecl FakeRgbaToPixel(RwRGBA* c,RwInt32 fmt){ return (uint32(c->red)|uint32(c->green)<<8|uint32(c->blue)<<16|uint32(c->alpha)<<24)^uint32(fmt)*0x9E3779B1u; }
// destino / origen de libjpeg manejados por una JPegCompressScreen / JPegDecompressToRaster falsas
inline uint32 emptyCount{}, termRemain{};
inline void __cdecl FakeCompressScreen(void* cam,jpeg_destination_mgr* d){
    trace.push_back({"compress_screen",Id(cam),Id(d)}); cinfoFake.fill(0); At<void*>(cinfoFake.data(),0x18)=d; auto* c=reinterpret_cast<j_compress_ptr>(cinfoFake.data());
    auto* buf=reinterpret_cast<uint8*>(SCREEN_BUF);
    d->init_destination(c); trace.push_back({"init_destination",intptr_t(d->next_output_byte)-intptr_t(SCREEN_BUF),uint32(d->free_in_buffer)});
    for(uint32 k=0;k<emptyCount;++k){ for(uint32 j=0;j<0x32000;j+=4)*reinterpret_cast<uint32*>(buf+j)=j*2654435761u^k^seed; d->next_output_byte=buf+0x32000; d->free_in_buffer=0;
        const auto r=uint8(d->empty_output_buffer(c)); trace.push_back({"empty_output_buffer",r,intptr_t(d->next_output_byte)-intptr_t(SCREEN_BUF),uint32(d->free_in_buffer)}); }
    for(uint32 j=0;j<termRemain;++j)buf[j]=uint8(j*13u+seed); d->next_output_byte=buf+termRemain; d->free_in_buffer=0x32000-termRemain;
    d->term_destination(c); trace.push_back({"term_destination",intptr_t(d->next_output_byte)-intptr_t(SCREEN_BUF),uint32(d->free_in_buffer)}); }
inline uintptr_t dllResync{};
inline uint8 __cdecl FakeDecompressToRaster(void* r,jpeg_source_mgr* s){
    const auto res=uintptr_t(s->resync_to_restart);
    trace.push_back({"decompress_to_raster",Id(r),intptr_t(s->next_input_byte)-intptr_t(SCREEN_BUF),res==0x5CC6A0||res==dllResync ? json("resync") : json(res)});
    cinfoFake.fill(0); At<void*>(cinfoFake.data(),0x18)=s; auto* c=reinterpret_cast<j_decompress_ptr>(cinfoFake.data());
    s->init_source(c);
    for(int k=0;k<2;++k){ const auto ok=uint8(s->fill_input_buffer(c)); trace.push_back({"fill_input_buffer",ok,intptr_t(s->next_input_byte)-intptr_t(SCREEN_BUF),uint32(s->bytes_in_buffer),G<uint32>(0xBD0B70),Fnv(reinterpret_cast<void*>(SCREEN_BUF),0x32000)}); }
    s->skip_input_data(c,7); s->term_source(c); return 1; }
inline FILESTREAM openResult{};
inline FILESTREAM __cdecl FakeOpenFile(const char* path,const char* mode){ trace.push_back({"open_file",std::string(path),std::string(mode)}); return openResult; }
inline size_t __cdecl FakeWriteFile(FILESTREAM f,const void* b,size_t n){ trace.push_back({"write_file",Id(f),intptr_t(b)-intptr_t(SCREEN_BUF),uint32(n),Fnv(b,std::min<size_t>(n,0x32000))}); return n; }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ trace.push_back({"close_file",Id(f)}); return 0; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,44> fns{{
    {"CAEWeatherAudioEntity","Service",0x5052F0,0,-1,1024},{"CAEWeatherAudioEntity","StaticInitialise",0x5B9A70,0,-1,64},
    {"CAEWeatherAudioEntity","StaticReset",0x5052B0,0,-1,128},{"CAEWeatherAudioEntity","UpdateParameters",0,0x872A74,0,1024},
    {"CBulletTraces","AddTrace-",0x723750,0,-1,1024},{"CBulletTraces","AddTrace-Wrapper",0x726AF0,0,-1,512},{"CBulletTraces","Init",0x721D50,0,-1,64},
    {"CBulletTraces","Render",0x723C10,0,-1,512},{"CBulletTraces","Update",0x723FB0,0,-1,256},
    {"CFire","Constructor",0x539D90,0,-1,64},{"CFire","CreateFxSysForStrength",0x539360,0,-1,256},{"CFire","Extinguish",0x5393F0,0,-1,256},
    {"CFire","Initialise",0x538B30,0,-1,64},{"CFire","ProcessFire",0x53A570,0,-1,1024},
    {"CMovingThings","Init",0x717D00,0,-1,64},{"CMovingThings","Render",0x717150,0,-1,64},{"CMovingThings","Render_BeforeClouds",0x7178F0,0,-1,64},
    {"CMovingThings","Shutdown",0x717C20,0,-1,64},{"CMovingThings","Update",0x7185B0,0,-1,64},
    {"CShotInfo","AddShot",0x739C30,0,-1,1024},{"CShotInfo","GetFlameThrowerShotPosn",0x739DE0,0,-1,256},{"CShotInfo","Initialise",0x739B60,0,-1,64},
    {"CShotInfo","Shutdown",0x739C20,0,-1,64},{"CShotInfo","Update",0x739E60,0,-1,512},
    {"CWaterCannon","Init",0x728B40,0,-1,64},{"CWaterCannon","PushPeds",0x7295E0,0,-1,512},{"CWaterCannon","Render",0x728DA0,0,-1,512},
    {"CWaterCannon","Update_NewInput",0x728C20,0,-1,256},{"CWaterCannon","Update_OncePerFrame",0x72A280,0,-1,512},
    {"FxMemoryPool_c","Exit",0x4A9C80,0,-1,128},{"FxMemoryPool_c","GetMem",0x4A9CA0,0,-1,512},{"FxMemoryPool_c","Init",0x4A9C30,0,-1,64},
    {"FxMemoryPool_c","Optimise",0x4A9CD0,0,-1,128},{"FxMemoryPool_c","Reset",0x4A9C90,0,-1,64},
    {"Input","AttachInputDevices",0x744A20,0,-1,64},{"Input","HandleKeyDown",0x743DF0,0,-1,1024},{"Input","HandleKeyUp",0x7443C0,0,-1,1024},
    {"Input","KeyboardHandler",0x744880,0,-1,512},{"Input","PadHandler",0x7449F0,0,-1,512},
    {"JPegPlugin","JPegCompressScreen",0x5D0470,0,-1,256},{"JPegPlugin","JPegCompressScreenToBuffer",0x5D0740,0,-1,256},
    {"JPegPlugin","JPegCompressScreenToFile",0x5D0820,0,-1,256},{"JPegPlugin","JPegDecompressToRaster",0x5D05F0,0,-1,256},
    {"JPegPlugin","JPegDecompressToVramFromBuffer",0x5D07A0,0,-1,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fx43-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fx43-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fx43");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"fx43-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,44> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,44> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    using std::bit_cast;
    dllResync=reinterpret_cast<uintptr_t>(&jpeg_resync_to_restart);
    const uintptr_t padsAddr=reinterpret_cast<uintptr_t>(CPad::GetPad(0));
    if(regions.empty()){ for(auto& b:ent)Reg(b.data(),b.size()); for(auto& b:mats)Reg(b.data(),b.size()); Reg(camMat.data(),camMat.size()); for(auto& b:intel)Reg(b.data(),b.size());
        Reg(weather.data(),weather.size()); Reg(snd.data(),snd.size()); for(auto& b:fire)Reg(b.data(),b.size()); for(auto& b:fx)Reg(b.data(),b.size()); Reg(weapInfo.data(),weapInfo.size());
        Reg(cannon.data(),cannon.size()); Reg(modelInfo.data(),modelInfo.size()); Reg(vehStruct.data(),vehStruct.size()); Reg(vehStore.data(),vehStore.size()); Reg(pedStore.data(),pedStore.size());
        Reg(objStore.data(),objStore.size()); Reg(memPool.data(),memPool.size()); Reg(smallBuf.data(),smallBuf.size()); Reg(outVec.data(),outVec.size()); Reg(image.data(),image.size());
        Reg(raster.data(),raster.size()); Reg(camera.data(),camera.size()); Reg(dstMgr.data(),dstMgr.size()); Reg(srcMgr.data(),srcMgr.size()); Reg(pixels.data(),pixels.size()); Reg(handle.data(),handle.size());
        Reg(bigBuf.data(),bigBuf.size()); Reg(reinterpret_cast<void*>(FIRES),0x964); Reg(reinterpret_cast<void*>(SHOTS),0x1130); Reg(reinterpret_cast<void*>(RAIN_L),0x150); }
    for(auto& v:vehVt)v=reinterpret_cast<void*>(&FakeVTrap); vehVt[0xB0/4]=reinterpret_cast<void*>(&FakeBurstTyre);

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {0x8CC2A0,0x80},{0xB6BAF8,0x180},{0xB72914,4},{0xC812A0,0xA0},{0xBD00F8,4},{0xB6F028,0x18},{CAM_MAT,0x40},{0xB6F081,1},{0xB6F1A8,0x238*3},
            {0xB7CB4C,0x3C},{TRACES,0x2C0},{FIRES,0x964},{0xB74490,0x10},{0xA9B0C8+4*MI,4},{0xC713A8,0x300},{0xC6E9A8,32*0x150},{RANDTAB,0x11A0},
            {WC_VERTS,0x1D4},{0xA9AE00,0x80},{0xB72CB0,0x270},{0xB73400,1},{padsAddr,2*0x134},{0xB70CF0,0x20},{0xBA67A0,0x48},{0xBD0160,0xC02B84-0xBD0160}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        // comunes
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), p2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF550,FakeIm3DRender), i3(0x7EF520,FakeIm3DEnd);
        EntryPatch los(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),FakeLos), lose(0x56BA00,FakeLos);
        EntryPatch rf(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),FakeReportFrontend), rfe(0x506EA0,FakeReportFrontend);
        EntryPatch sf(bit_cast<uintptr_t>(static_cast<CFire*(CFireManager::*)(CEntity*,CEntity*,float,uint8,uint32,int8)>(&CFireManager::StartFire)),FakeStartFireEnt), sfe(0x53A050,FakeStartFireEnt);
        EntryPatch ex(bit_cast<uintptr_t>(&CFireManager::ExtinguishPointWithWater),FakeExtinguishPoint), exe1(0x5394C0,FakeExtinguishPoint);
        EntryPatch ap(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),FakeAddParticle), ape(0x4AA440,FakeAddParticle);
        // audio
        EntryPatch a1(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity),FakeAreSounds), a1e(0x4EF570,FakeAreSounds);
        EntryPatch a2(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),FakeBankLoaded), a2e(0x4D88C0,FakeBankLoaded);
        EntryPatch a3(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),FakeLoadBank), a3e(0x4D88A0,FakeLoadBank);
        EntryPatch a4(bit_cast<uintptr_t>(&CAESound::Initialise),FakeSoundInit), a4e(0x4EFE50,FakeSoundInit);
        EntryPatch a5(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),FakeRequestSound), a5e(0x4EFB10,FakeRequestSound);
        EntryPatch a6(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::Initialise),FakeTwinInit), a6e(0x4F28A0,FakeTwinInit);
        EntryPatch a7(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::StopSoundAndForget),FakeTwinStop), a7e(0x4F2960,FakeTwinStop);
        EntryPatch a8(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::UpdateTwinLoopSound),FakeTwinUpdate), a8e(0x4F29A0,FakeTwinUpdate);
        EntryPatch a9(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::DoSoundsSwitchThisFrame),FakeTwinSwitch), a9e(0x4F2CA0,FakeTwinSwitch);
        EntryPatch a10(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::PlayTwinLoopSound),FakeTwinPlay), a10e(0x4F2CB0,FakeTwinPlay);
        EntryPatch a11(bit_cast<uintptr_t>(static_cast<float(*)(float,float)>(&CAEAudioUtility::GetRandomNumberInRange<float>)),FakeRange), a11e(0x4D9C50,FakeRange);
        EntryPatch a12(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability),FakeProb), a12e(0x4D9C80,FakeProb);
        EntryPatch a13(bit_cast<uintptr_t>(&CCullZones::PlayerNoRain),FakePlayerNoRain), a13e(0x72DDC0,FakePlayerNoRain);
        EntryPatch a14(bit_cast<uintptr_t>(&CCullZones::CamNoRain),FakeCamNoRain), a14e(0x72DDB0,FakeCamNoRain);
        EntryPatch a15(bit_cast<uintptr_t>(&CAEAudioUtility::GetPiecewiseLinear),FakePiecewise), a15e(0x4D9D90,FakePiecewise);
        EntryPatch a16(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget),FakeSoundStop), a16e(0x4EF850,FakeSoundStop);
        // efectos / fuego
        EntryPatch f1(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(Const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),FakeCreateFx), f1e(0x4A9BE0,FakeCreateFx);
        EntryPatch f2(bit_cast<uintptr_t>(&FxSystem_c::Kill),FakeFxKill), f2e(0x4AA3F0,FakeFxKill);
        EntryPatch f3(bit_cast<uintptr_t>(&FxSystem_c::Play),FakeFxPlay), f3e(0x4AA2F0,FakeFxPlay);
        EntryPatch f4(bit_cast<uintptr_t>(&FxSystem_c::SetOffsetPos),FakeFxOffset), f4e(0x4AA660,FakeFxOffset);
        EntryPatch f5(bit_cast<uintptr_t>(&FxSystem_c::SetConstTime),FakeFxConstTime), f5e(0x4AA6C0,FakeFxConstTime);
        EntryPatch f6(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUp), f6e(0x571A00,FakeCleanUp);
        EntryPatch f7(0x6D7C90,FakeInflict);
        EntryPatch f8(bit_cast<uintptr_t>(&CPlayerPed::DoStuffToGoOnFire),FakeGoOnFire), f8e(0x60A020,FakeGoOnFire);
        EntryPatch f9(bit_cast<uintptr_t>(&CVehicle::FindTyreNearestPoint),FakeNearestTyre), f9e(0x6D7BC0,FakeNearestTyre);
        EntryPatch f10(bit_cast<uintptr_t>(&CObject::ObjectFireDamage),FakeObjFireDamage), f10e(0x5A1580,FakeObjFireDamage);
        EntryPatch f11(bit_cast<uintptr_t>(&CCreepingFire::TryToStartFireAtCoors),FakeTryFire), f11e(0x53A450,FakeTryFire);
        EntryPatch f12(0x7000E0,FakeAddLight);
        EntryPatch f13(0x6FFB40,FakeLightsInit);
        EntryPatch f14(bit_cast<uintptr_t>(&CEscalators::Init),FakeEscInit), f14e(0x717C50,FakeEscInit);
        EntryPatch f15(bit_cast<uintptr_t>(&CEscalators::Update),FakeEscUpdate), f15e(0x718580,FakeEscUpdate);
        EntryPatch f16(bit_cast<uintptr_t>(&CEscalator::SwitchOff),FakeEscSwitchOff), f16e(0x717860,FakeEscSwitchOff);
        EntryPatch f17(bit_cast<uintptr_t>(&CPlaneTrails::Update),FakeTrailsUpdate), f17e(0x7174F0,FakeTrailsUpdate);
        EntryPatch f18(bit_cast<uintptr_t>(&CPlaneTrails::Render),FakeTrailsRender), f18e(0x7173A0,FakeTrailsRender);
        // disparos
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),FakeWeaponInfo), s1e(0x743C60,FakeWeaponInfo);
        EntryPatch s2(bit_cast<uintptr_t>(&CPed::IsPointerValid),FakePtrValid), s2e(0x5E4220,FakePtrValid);
        EntryPatch s3(bit_cast<uintptr_t>(&CPed::IsPedInControl),FakeInControl), s3e(0x5E3960,FakeInControl);
        EntryPatch s4(bit_cast<uintptr_t>(&CPed::GetLocalDirection),FakeLocalDir), s4e(0x5DEF60,FakeLocalDir);
        EntryPatch s5(bit_cast<uintptr_t>(&CWeapon::GenerateDamageEvent),FakeDamageEvent), s5e(0x73A530,FakeDamageEvent);
        EntryPatch s6(bit_cast<uintptr_t>(&CWorld::SprayPaintWorld),FakeSpray), s6e(0x565B70,FakeSpray);
        EntryPatch s7(bit_cast<uintptr_t>(&CWorld::SetCarsOnFire),FakeCarsOnFire), s7e(0x5659F0,FakeCarsOnFire);
        EntryPatch s8(bit_cast<uintptr_t>(&CWorld::SetWorldOnFire),FakeWorldOnFire), s8e(0x56B910,FakeWorldOnFire);
        // caniones de agua
        EntryPatch w1(bit_cast<uintptr_t>(&CAEWaterCannonAudioEntity::Initialise),FakeWcAudioInit), w1e(0x503060,FakeWcAudioInit);
        EntryPatch w2(bit_cast<uintptr_t>(&CAEWaterCannonAudioEntity::ClearSplashInfo),FakeWcClearSplash), w2e(0x502F20,FakeWcClearSplash);
        EntryPatch w3(bit_cast<uintptr_t>(&CAEWaterCannonAudioEntity::SetSplashInfo),FakeWcSetSplash), w3e(0x502F50,FakeWcSetSplash);
        EntryPatch w4(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeEventAdd), w4e(0x4AB420,FakeEventAdd);
        EntryPatch w5(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),FakeMoveForce), w5e(0x5429F0,FakeMoveForce);
        // entrada
        EntryPatch n1(bit_cast<uintptr_t>(&RsInputDeviceAttach),FakeAttach), n1e(0x6194C0,FakeAttach);
        EntryPatch n2(bit_cast<uintptr_t>(&CControllerConfigManager::UpdateJoyButtonState),FakeJoyState), n2e(0x52F510,FakeJoyState);
        EntryPatch n3(bit_cast<uintptr_t>(&CControllerConfigManager::UpdateJoyInConfigMenus_ButtonDown),FakeJoyCfgDown), n3e(0x52DAB0,FakeJoyCfgDown);
        EntryPatch n4(bit_cast<uintptr_t>(&CControllerConfigManager::UpdateJoy_ButtonDown),FakeJoyDown), n4e(0x530ED0,FakeJoyDown);
        EntryPatch n5(bit_cast<uintptr_t>(&CControllerConfigManager::UpdateJoyInConfigMenus_ButtonUp),FakeJoyCfgUp), n5e(0x52DC20,FakeJoyCfgUp);
        EntryPatch n6(bit_cast<uintptr_t>(&CControllerConfigManager::AffectControllerStateOn_ButtonUp),FakeJoyUp), n6e(0x531070,FakeJoyUp);
        // JPEG y ficheros
        EntryPatch j1(bit_cast<uintptr_t>(&RsGrabScreen),FakeGrab), j1e(0x619AF0,FakeGrab);
        EntryPatch j2(bit_cast<uintptr_t>(&RwImageDestroy),FakeImageDestroy), j2e(0x802740,FakeImageDestroy);
        EntryPatch j3(reinterpret_cast<uintptr_t>(&jpeg_std_error),FakeStdError), j3e(0x5CD810,FakeStdError);
        EntryPatch j4(reinterpret_cast<uintptr_t>(&jpeg_CreateCompress),FakeCreateCompressD), j4e(0x5CF960,FakeCreateCompressE);
        EntryPatch j5(reinterpret_cast<uintptr_t>(&jpeg_set_defaults),FakeSetDefaultsD), j5e(0x5C7590,FakeSetDefaultsE);
        EntryPatch j6(reinterpret_cast<uintptr_t>(&jpeg_start_compress),FakeStartCompressD), j6e(0x5C26B0,FakeStartCompressE);
        EntryPatch j7(reinterpret_cast<uintptr_t>(&jpeg_write_scanlines),FakeWriteScanD), j7e(0x5C2730,FakeWriteScanE);
        EntryPatch j8(reinterpret_cast<uintptr_t>(&jpeg_finish_compress),FakeFinishCompressD), j8e(0x5CFAC0,FakeFinishCompressE);
        EntryPatch j9(reinterpret_cast<uintptr_t>(&jpeg_destroy_compress),FakeDestroyCompressD), j9e(0x5CFA30,FakeDestroyCompressE);
        EntryPatch j10(reinterpret_cast<uintptr_t>(&jpeg_CreateDecompress),FakeCreateDecompressD), j10e(0x5CFD20,FakeCreateDecompressE);
        EntryPatch j11(reinterpret_cast<uintptr_t>(&jpeg_read_header),FakeReadHeaderD), j11e(0x5D01D0,FakeReadHeaderE);
        EntryPatch j12(reinterpret_cast<uintptr_t>(&jpeg_start_decompress),FakeStartDecompressD), j12e(0x5C87E0,FakeStartDecompressE);
        EntryPatch j13(reinterpret_cast<uintptr_t>(&jpeg_read_scanlines),FakeReadScanD), j13e(0x5C8690,FakeReadScanE);
        EntryPatch j14(reinterpret_cast<uintptr_t>(&jpeg_finish_decompress),FakeFinishDecompressD), j14e(0x5D0110,FakeFinishDecompressE);
        EntryPatch j15(reinterpret_cast<uintptr_t>(&jpeg_destroy_decompress),FakeDestroyDecompressD), j15e(0x5CFE00,FakeDestroyDecompressE);
        EntryPatch j16(bit_cast<uintptr_t>(&RwRasterLock),FakeRasterLock), j16e(0x7FB2D0,FakeRasterLock);
        EntryPatch j17(bit_cast<uintptr_t>(&RwRasterUnlock),FakeRasterUnlock), j17e(0x7FAEC0,FakeRasterUnlock);
        EntryPatch j18(bit_cast<uintptr_t>(&RwRGBAToPixel),FakeRgbaToPixel), j18e(0x803740,FakeRgbaToPixel);
        EntryPatch j19(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), j19e(0x538900,FakeOpenFile);
        EntryPatch j20(bit_cast<uintptr_t>(&CFileMgr::Write),FakeWriteFile), j20e(0x538970,FakeWriteFile);
        EntryPatch j21(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), j21e(0x5389D0,FakeCloseFile);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=StateSet;
        auto& camMatrix=G<void*>(0xB6F03C); auto& camPlace=G<CVector>(0xB6F02C);
        auto& timeMs=G<uint32>(0xB7CB84); auto& timeStep=G<float>(0xB7CB5C); auto& frames=G<uint32>(0xB7CB4C);
        auto& currArea=G<int32>(0xB72914); auto& activeCam=G<uint8>(0xB6F081);
        auto CamMode=[](int k)->uint16& { return G<uint16>(0xB6F1A8+0x238*k); };
        const uintptr_t dllCompressScreen=bit_cast<uintptr_t>(&JPegCompressScreen), dllDecompressToRaster=bit_cast<uintptr_t>(static_cast<bool(*)(RwRaster*,jpeg_source_mgr&)>(&JPegDecompressToRaster));

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto unitVec=[&]{ CVector v=randVec(-1.f,1.f); const float m=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z)+1e-3f; return CVector{v.x/m,v.y/m,v.z/m}; };
                // matriz falsa ortonormal aproximada en mats[k] (CMatrixLink)
                auto mkMat=[&](int k,const CVector& pos){ auto* m=mats[k].data(); fill(m,0x60); const CVector r=unitVec(), u0=unitVec();
                    CVector fw{u0.y*r.z-u0.z*r.y,u0.z*r.x-u0.x*r.z,u0.x*r.y-u0.y*r.x}; const float l=std::sqrt(fw.x*fw.x+fw.y*fw.y+fw.z*fw.z)+1e-3f; fw=CVector{fw.x/l,fw.y/l,fw.z/l};
                    const CVector up{r.y*fw.z-r.z*fw.y,r.z*fw.x-r.x*fw.z,r.x*fw.y-r.y*fw.x};
                    At<CVector>(m,0)=r; At<CVector>(m,0x10)=fw; At<CVector>(m,0x20)=up; At<CVector>(m,0x30)=pos; At<void*>(m,0x40)=nullptr; At<uint32>(m,0x44)=0; return m; };
                // entidad falsa: tipo en 0x36, matriz o placement
                auto mkEnt=[&](void* e,int type,const CVector& pos,int matIdx){ fill(e,0x80); At<uint8>(e,0x36)=uint8((next()&0xF8)|type); At<void*>(e,0x18)=nullptr;
                    if(matIdx>=0){ At<void*>(e,0x14)=mkMat(matIdx,pos); } else { At<void*>(e,0x14)=nullptr; At<CVector>(e,4)=pos; } At<CVector>(e,0x44)=randVec(-0.5f,0.5f); };
                auto setCamera=[&]{ const CVector p{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(-10.f,600.f)}; fill(camMat.data(),0x60);
                    const CVector r=unitVec(); At<CVector>(camMat.data(),0)=r; At<CVector>(camMat.data(),0x10)=unitVec(); At<CVector>(camMat.data(),0x20)=unitVec(); At<CVector>(camMat.data(),0x30)=p;
                    At<void*>(camMat.data(),0x40)=nullptr; camMatrix=camMat.data(); camPlace=randVec(-5.f,5.f);
                    auto* cm=reinterpret_cast<uint8*>(CAM_MAT); At<CVector>(cm,0)=unitVec(); At<CVector>(cm,0x10)=unitVec(); At<CVector>(cm,0x20)=unitVec(); At<CVector>(cm,0x30)=p; return p; };
                auto mkPool=[&](int k,uint8* store,size_t stride,int n,uintptr_t global){ auto* p=pools[k].data(); At<void*>(p,0)=store; At<void*>(p,4)=poolFlags[k].data(); At<int32>(p,8)=n; At<int32>(p,0xC)=-1; At<uint8>(p,0x10)=0; At<uint8>(p,0x11)=0;
                    for(int j=0;j<n;++j)poolFlags[k][j]=uint8(next()%4==0 ? 0x80|(next()&0x7F) : next()&0x7F); G<void*>(global)=p; (void)stride; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4343u; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0; rangeCalls=0;
                    maskA=next(); maskB=next(); maskC=next(); callsA=callsB=callsC=0; scanMask=next()|1; scanCalls=0; attached.clear();
                    im3dOk=next()%5!=0; playerPed=nullptr; playerVeh=nullptr; losEntity=nullptr;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    timeMs=next()%3 ? 100000+next()%5000000 : next(); timeStep=next()%6==0 ? 1.f : uni(0.2f,3.f); frames=next();

                    if(cls=="CAEWeatherAudioEntity") {
                        fill(weather.data(),weather.size()); for(auto& v:volTable)v=int8(int32(next()%45)-35); G<int8*>(0xBD00F8)=volTable.data();
                        currArea=next()%4==0 ? int32(1+next()%5) : 0;
                        auto& rainVol=G<float>(0x8CC30C);
                        if(nm=="StaticInitialise") reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="StaticReset"){ rainVol=uni(-120.f,10.f); At<uint16>(reinterpret_cast<void*>(RAIN_L),0x88)=uint16(next()%3 ? next()%2 : next()); At<uint16>(reinterpret_cast<void*>(RAIN_R),0x88)=uint16(next()%3 ? next()%2 : next());
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["rain_volume"]=F(rainVol); }
                        else if(nm=="Service"){
                            auto& rain=G<float>(0xC81324);
                            switch(next()%8){ case 0: rain=0.f; break; case 1: rain=-uni(0.f,1.f); break; case 2: rain=0.2f; break; case 3: rain=uni(0.f,0.25f); break; case 4: rain=1.f; break; default: rain=uni(0.f,1.f); }
                            switch(next()%7){ case 0: rainVol=-100.f; break; case 1: rainVol=-50.f; break; case 2: rainVol=-50.25f; break; case 3: rainVol=uni(-55.f,-45.f); break; default: rainVol=uni(-120.f,10.f); }
                            G<int32>(0x8CC310)=next()%5==0 ? int32(next()) : int32(next()%14);
                            At<uint16>(reinterpret_cast<void*>(RAIN_L),0x88)=uint16(next()%3 ? 1 : 0); At<uint16>(reinterpret_cast<void*>(RAIN_R),0x88)=uint16(next()%3 ? 1 : 0);
                            reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(weather.data(),nullptr);
                            out["rain_volume"]=F(rainVol); out["drop_id"]=G<int32>(0x8CC310); }
                        else { // UpdateParameters
                            fill(snd.data(),0x74); static const int32 evs[]{1,1,1,3,3,3,3,4,5,4,5,2,0,7};
                            At<int32>(snd.data(),0xC)=evs[next()%std::size(evs)];
                            const uint32 posKind=next()%4; At<CVector>(snd.data(),0x24)=posKind==0 ? CVector{-0.906f,0.423f,0.f} : posKind==1 ? CVector{0.906f,0.423f,0.f} : posKind==2 ? CVector{-0.906f,0.f,0.423f} : randVec(-1.f,1.f);
                            if(next()%2) At<CVector>(snd.data(),0x24)=CVector{-0.906f,0.423f,0.f};
                            At<float>(snd.data(),0x14)=next()%8==0 ? -50.f : uni(-70.f,20.f); At<float>(snd.data(),0x1C)=uni(0.5f,2.f);
                            const int16 pos=int16(next()%10==0 ? -int32(next()%50) : next()%6==0 ? int32(next()%32768) : int32(next()%900));
                            G<uint32>(0xC812C4)=next()%8==0 ? next() : next()%31; G<float>(0xC812EC)=uni(-0.3f,1.3f);
                            // viento: valores del juego con variaciones
                            G<float>(0x8CC2A4)=next()%4 ? -33.f : uni(-60.f,0.f); G<float>(0x8CC2A8)=next()%4 ? 0.6f : uni(0.1f,2.f); G<float>(0x8CC2AC)=next()%4 ? 0.6f : uni(0.1f,2.f);
                            G<float>(0x8CC2B0)=next()%4 ? 21.f : uni(0.f,40.f); G<float>(0x8CC2B4)=next()%4 ? 1.2f : uni(0.f,3.f); G<float>(0x8CC2B8)=next()%4 ? 0.1f : uni(0.01f,1.f); G<float>(0x8CC2BC)=next()%4 ? 0.1f : uni(0.01f,1.f);
                            G<uint8>(0x8CC2C0)=uint8(next()%5==0 ? 2 : next()%2); G<float>(0x8CC2C4)=uni(0.5f,3.f); G<float>(0x8CC2CC)=uni(0.f,0.2f);
                            G<float>(0xB6BAF8)=uni(0.f,3.f); G<float>(0xB6BAFC)=uni(0.f,30.f);
                            const CVector cam=setCamera();
                            if(next()%4){ mkEnt(ent[0].data(),3,CVector{cam.x+uni(-3.f,3.f),cam.y+uni(-3.f,3.f),cam.z+uni(-8.f,8.f)},next()%2 ? 0 : -1); playerPed=ent[0].data();
                                At<uint32>(ent[0].data(),0x46C)=next(); At<void*>(ent[0].data(),0x58C)=next()%3 ? ent[1].data() : nullptr; mkEnt(ent[1].data(),2,cam,1); }
                            mkEnt(ent[2].data(),next()%3 ? 2 : 1,cam,2); mkEnt(ent[3].data(),next()%2 ? 4 : 5,cam,-1);
                            losEntity=next()%5==0 ? nullptr : (next()%2 ? ent[2].data() : ent[3].data()); losPoint=CVector{cam.x+uni(-2.f,2.f),cam.y+uni(-2.f,2.f),cam.z+uni(-2.f,2.f)}; losNormal=unitVec();
                            G<void*>(0xB6BC74)=next()%4==0 ? losEntity : (next()%2 ? nullptr : ent[3].data()); G<void*>(0xB6BC6C)=next()%4==0 ? losEntity : nullptr;
                            G<float>(0xB6BC70)=next()%3==0 ? 0.f : uni(-1.f,3.f); G<float>(0xB6BC68)=next()%3==0 ? 0.f : uni(-1.f,3.f);
                            reinterpret_cast<void(__fastcall*)(void*,void*,CAESound*,int32)>(a)(weather.data(),nullptr,reinterpret_cast<CAESound*>(snd.data()),pos);
                            out["sound"]=ReversingProbe::Bytes(snd.data(),0x74); out["wind"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8CC2A4),0x2C);
                            out["wind2"]=json::array({F(G<float>(0xB6BAF8)),F(G<float>(0xB6BAFC))}); out["residues"]=json::array({F(G<float>(0xB6BC70)),F(G<float>(0xB6BC68))});
                            out["last"]=json::array({Id(G<void*>(0xB6BC74)),Id(G<void*>(0xB6BC6C))}); }
                    }
                    else if(cls=="CBulletTraces") {
                        auto* tr=reinterpret_cast<uint8*>(TRACES);
                        auto mkTraces=[&](int existing){ fill(tr,0x2C0); for(int k=0;k<16;++k){ uint8* t=tr+k*0x2C; At<CVector>(t,0)=randVec(-3000.f,3000.f); At<CVector>(t,0xC)=At<CVector>(t,0)+randVec(-20.f,20.f);
                            t[0x18]=uint8(existing<0 ? next()%2 : (k<existing)); At<uint32>(t,0x1C)=timeMs-next()%400; At<uint32>(t,0x20)=next()%8==0 ? 0u : 1+next()%400; At<float>(t,0x24)=uni(0.f,0.1f); } };
                        if(nm=="Init"){ mkTraces(-1); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Update"){ mkTraces(-1); if(next()%4==0)for(int k=0;k<16;++k)At<uint32>(tr+k*0x2C,0x1C)=next(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Render"){ mkTraces(-1); setCamera(); for(int k=0;k<16;++k){ uint8* t=tr+k*0x2C; At<uint32>(t,0x1C)=timeMs-next()%300; At<uint32>(t,0x20)=next()%10==0 ? next() : 1+next()%300; t[0x28]=uint8(next()); }
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="AddTrace-"){
                            const int existing=next()%4==0 ? 16 : int(next()%17); mkTraces(-1); { std::array<int,16> idx{}; for(int k=0;k<16;++k)idx[k]=k; for(int k=15;k>0;--k)std::swap(idx[k],idx[next()%(k+1)]); for(int k=0;k<16;++k)tr[idx[k]*0x2C+0x18]=uint8(k<existing); }
                            const CVector cam=setCamera(); const auto* cm=reinterpret_cast<uint8*>(CAM_MAT);
                            auto around=[&](float fwdSign){ const CVector& r=At<CVector>(const_cast<uint8*>(cm),0); const CVector& fw=At<CVector>(const_cast<uint8*>(cm),0x10); const CVector& u=At<CVector>(const_cast<uint8*>(cm),0x20);
                                const float a1=fwdSign*uni(0.f,30.f), b1=uni(-2.5f,2.5f), c1=uni(-2.5f,2.5f); return CVector{cam.x+fw.x*a1+r.x*b1+u.x*c1,cam.y+fw.y*a1+r.y*b1+u.y*c1,cam.z+fw.z*a1+r.z*b1+u.z*c1}; };
                            CVector from=around(1.f), to=next()%4 ? around(-1.f) : around(1.f); if(next()%8==0){from=randVec(-3000.f,3000.f);to=randVec(-3000.f,3000.f);}
                            const uint32 dis=next()%5==0 ? next() : next()%2000;
                            reinterpret_cast<void(__cdecl*)(const CVector&,const CVector&,float,uint32,uint8)>(a)(from,to,uni(0.f,0.1f),dis,uint8(next())); }
                        else { // AddTrace-Wrapper
                            mkTraces(-1); setCamera();
                            mkEnt(ent[0].data(),3,randVec(-100.f,100.f),-1); mkEnt(ent[1].data(),2,randVec(-100.f,100.f),0);
                            At<CVector>(ent[0].data(),0x44)=next()%2 ? randVec(-0.03f,0.03f) : randVec(-1.f,1.f); At<CVector>(ent[1].data(),0x44)=next()%2 ? randVec(-0.03f,0.03f) : randVec(-1.f,1.f);
                            playerPed=ent[0].data(); playerVeh=next()%3==0 ? ent[1].data() : nullptr;
                            void* from=nullptr; switch(next()%4){ case 0: from=ent[0].data(); break; case 1: from=ent[1].data(); break; case 2: from=ent[2].data(); break; default: break; }
                            activeCam=uint8(next()%3); static const uint16 modes[]{34,7,46,8,51,42,39,40,52,45,4,16,53};
                            for(int k=0;k<3;++k)CamMode(k)=modes[next()%std::size(modes)];
                            const CVector muzzle=randVec(-3000.f,3000.f); const CVector hit=next()%8==0 ? muzzle : muzzle+randVec(-60.f,60.f);
                            reinterpret_cast<void(__cdecl*)(const CVector&,const CVector&,int32,void*)>(a)(muzzle,hit,int32(next()%47),from); }
                        out["traces"]=ReversingProbe::Bytes(tr,0x2C0);
                    }
                    else if(cls=="CFire") {
                        auto* fr=fire[0].data();
                        auto mkFire=[&](uint8* p,const CVector& pos){ fill(p,0x28); At<CVector>(p,4)=pos; At<void*>(p,0x10)=nullptr; At<void*>(p,0x14)=next()%2 ? ent[4].data() : nullptr;
                            const float st[]{0.5f,1.f,1.5f,2.f,2.5f,0.999f,1.999f,3.f,2.0001f}; At<float>(p,0x1C)=next()%3 ? uni(0.2f,3.2f) : st[next()%std::size(st)];
                            At<void*>(p,0x24)=next()%3 ? fx[next()%2].data() : nullptr; At<uint32>(p,0x18)=timeMs+next()%20000-5000; };
                        if(nm=="Constructor"||nm=="Initialise"){ fr[0]=0; if(next()%2)std::memset(fr,0xCD,0x28); else fill(fr,0x28);
                            if(nm=="Constructor") ret=Id(reinterpret_cast<void*(__fastcall*)(void*,void*)>(a)(fr,nullptr)); else reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(fr,nullptr); }
                        else if(nm=="CreateFxSysForStrength"){ mkFire(fr,randVec(-3000.f,3000.f)); if(next()%10==0)At<uint32>(fr,0x1C)=0x7FC00000u; fxResult=next()%3 ? fx[2].data() : nullptr;
                            const CVector pt=randVec(-3000.f,3000.f); fill(smallBuf.data(),0x40);
                            reinterpret_cast<void(__fastcall*)(void*,void*,const CVector&,RwMatrix*)>(a)(fr,nullptr,pt,next()%2 ? reinterpret_cast<RwMatrix*>(smallBuf.data()) : nullptr); }
                        else if(nm=="Extinguish"){ mkFire(fr,randVec(-3000.f,3000.f)); const int t=int(next()%4); if(t<3){ mkEnt(ent[1].data(),t==0 ? 3 : t==1 ? 2 : 4,randVec(-10.f,10.f),-1);
                                At<void*>(ent[1].data(),0x730)=next()%2 ? fr : ent[5].data(); At<void*>(ent[1].data(),0x490)=next()%2 ? fr : ent[5].data(); At<void*>(fr,0x10)=ent[1].data(); }
                            reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(fr,nullptr);
                            out["ent_fire"]=json::array({Id(At<void*>(ent[1].data(),0x730)),Id(At<void*>(ent[1].data(),0x490))}); }
                        else { // ProcessFire
                            const CVector cam=setCamera(); const CVector base{cam.x+uni(-40.f,40.f),cam.y+uni(-40.f,40.f),cam.z+uni(-5.f,5.f)};
                            // fuegos del gestor (vecinos)
                            auto* fm=reinterpret_cast<uint8*>(FIRES); for(int k=0;k<60;++k){ uint8* p=fm+k*0x28; fill(p,0x28); p[0]=uint8((next()&0xE0)|(next()%10<7 ? 1 : 0)|(next()%5==0 ? 2 : 0)|(next()&0x1C));
                                At<CVector>(p,4)=base+randVec(-3.f,3.f); At<void*>(p,0x10)=nullptr; At<void*>(p,0x14)=nullptr; At<float>(p,0x1C)=uni(0.2f,1.5f); At<void*>(p,0x24)=next()%2 ? fx[1].data() : nullptr; }
                            uint8* self=next()%3==0 ? fm+(next()%60)*0x28 : fr; mkFire(self,base); self[0]=uint8((self[0]&0xFC)|1|(next()%5==0 ? 2 : 0)); self[0x20]=uint8(next()%3 ? next()%8 : next()); self[0x21]=uint8(next()%120);
                            At<float>(self,0x1C)=next()%3 ? uni(0.2f,3.2f) : float(1+next()%3)-(next()%2 ? 0.0001f : 0.f);
                            // entidad ardiendo
                            const int et=int(next()%5); mkEnt(ent[0].data(),3,base+randVec(-1.f,1.f),next()%2 ? 3 : -1); playerPed=ent[0].data();
                            At<void*>(ent[0].data(),0x730)=next()%4==0 ? nullptr : fire[1].data(); At<uint32>(ent[0].data(),0x40)=next(); At<void*>(ent[0].data(),0xFC)=next()%4==0 ? ent[5].data() : nullptr;
                            playerVeh=next()%4==0 ? ent[5].data() : nullptr;
                            if(et==1||et==2){ void* e=ent[1].data(); mkEnt(e,et==1 ? 3 : 2,base+randVec(-1.f,1.f),next()%2 ? 4 : -1); At<void*>(self,0x10)=e;
                                if(et==1){ At<void*>(e,0x730)=next()%5 ? self : nullptr; At<int32>(e,0x530)=next()%3==0 ? int32(54+next()%2) : int32(next()%60); At<uint32>(e,0x46C)=next(); At<int32>(e,0x598)=int32(next()%4);
                                    mkEnt(ent[2].data(),2,base,-1); At<void*>(e,0x58C)=next()%3 ? ent[2].data() : nullptr; At<int16>(ent[2].data(),0x22)=int16(next()%3==0 ? 407 : 400+next()%100); At<int32>(ent[2].data(),0x590)=int32(next()%3); }
                                else { At<void*>(e,0)=vehVt.data(); At<void*>(e,0x490)=next()%5 ? self : nullptr; At<int16>(e,0x22)=int16(MI); At<int32>(e,0x590)=int32(next()%3); At<float>(e,0x4C0)=uni(0.f,1000.f);
                                    fill(modelInfo.data(),0x60); fill(vehStruct.data(),0x40); At<void*>(modelInfo.data(),0x5C)=vehStruct.data(); At<CVector>(vehStruct.data(),0)=randVec(-3.f,3.f);
                                    G<void*>(0xA9B0C8+4*MI)=modelInfo.data(); if(!At<void*>(e,0x14))At<void*>(e,0x14)=mkMat(4,base); } }
                            // pools de vehiculos y objetos falsos
                            mkPool(0,vehStore.data(),0xA18,3,0xB74494); for(int k=0;k<3;++k){ uint8* v=vehStore.data()+k*0xA18; mkEnt(v,2,base+randVec(-3.f,3.f),next()%2 ? 5+k%3 : -1); At<void*>(v,0)=vehVt.data();
                                At<void*>(v,0x490)=next()%4==0 ? fr : nullptr; At<int16>(v,0x22)=int16(next()%5==0 ? 407 : 400+next()%100); At<int32>(v,0x594)=next()%3==0 ? 10 : int32(next()%12); }
                            mkPool(2,objStore.data(),0x19C,3,0xB7449C); for(int k=0;k<3;++k)mkEnt(objStore.data()+k*0x19C,4,base+randVec(-4.f,4.f),-1);
                            reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(self,nullptr);
                            out["fires"]=Fnv(fm,0x960); out["fires_flags"]=[&]{ json j=json::array(); for(int k=0;k<60;++k)j.push_back(fm[k*0x28]); return j; }();
                            out["self"]=ReversingProbe::Bytes(self,0x28); out["ent1"]=ReversingProbe::Bytes(ent[1].data()+0x40,4); out["veh_health"]=F(At<float>(ent[2].data(),0x4C0)); out["ent1_health"]=F(At<float>(ent[1].data(),0x4C0)); }
                        out["fire"]=ReversingProbe::Bytes(fr,0x28);
                    }
                    else if(cls=="CMovingThings") {
                        fill(reinterpret_cast<void*>(0xC713A8),0x300); auto* esc=reinterpret_cast<uint8*>(0xC6E9A8); for(int k=0;k<32;++k)esc[k*0x150+0x78]=uint8(next()%2);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["trails"]=Fnv(reinterpret_cast<void*>(0xC713A8),0x300); json ex=json::array(); for(int k=0;k<32;++k)ex.push_back(esc[k*0x150+0x78]); out["esc"]=ex;
                    }
                    else if(cls=="CShotInfo") {
                        auto* sh=reinterpret_cast<uint8*>(SHOTS); auto* rt=reinterpret_cast<float*>(RANDTAB);
                        fill(sh,0x1130); for(int k=0;k<20;++k)rt[k]=-0.05f+0.005f*float(k)+(next()%4==0 ? uni(-0.01f,0.01f) : 0.f);
                        for(int k=0;k<100;++k){ uint8* s=sh+k*0x2C; At<CVector>(s,4)=randVec(-3000.f,3000.f); At<CVector>(s,0x10)=randVec(-2.f,2.f); At<float>(s,0x1C)=uni(0.f,5.f); At<void*>(s,0x20)=nullptr;
                            At<float>(s,0x24)=float(timeMs+next()%2000)-1000.f; s[0x28]=0; s[0x29]=uint8(next()%2); }
                        fill(weapInfo.data(),0x80); At<uint32>(weapInfo.data(),0x18)=next(); At<uint16>(weapInfo.data(),0x22)=uint16(next()%100); At<float>(weapInfo.data(),0x5C)=uni(0.f,2.f);
                        At<float>(weapInfo.data(),0x60)=uni(0.f,5.f); At<float>(weapInfo.data(),0x64)=uni(0.f,3000.f); At<float>(weapInfo.data(),0x68)=next()%3==0 ? 0.f : uni(-1.f,1.f);
                        if(nm=="Initialise"||nm=="Shutdown") reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="AddShot"){ const int existing=next()%6==0 ? 100 : int(next()%100); for(int k=0;k<100;++k)sh[k*0x2C+0x28]=uint8(next()%100<uint32(existing) ? 1 : 0); if(existing==100)for(int k=0;k<100;++k)sh[k*0x2C+0x28]=1;
                            if(next()%8==0)timeMs|=0x80000000u;
                            mkEnt(ent[0].data(),3,CVector{},-1);
                            ret=int(reinterpret_cast<uint8(__cdecl*)(void*,int32,CVector,CVector)>(a)(next()%2 ? ent[0].data() : nullptr,int32(next()%47),randVec(-3000.f,3000.f),randVec(-3000.f,3000.f))); }
                        else if(nm=="GetFlameThrowerShotPosn"){ const uint8 id=uint8(next()%100); for(int k=0;k<100;++k){ At<int32>(sh+k*0x2C,0)=next()%2 ? 37 : int32(next()%47); sh[k*0x2C+0x28]=uint8(next()%2); }
                            fill(outVec.data(),0x20); ret=int(reinterpret_cast<uint8(__cdecl*)(uint8,CVector&)>(a)(id,At<CVector>(outVec.data(),0))); out["pos"]=ReversingProbe::Bytes(outVec.data(),0x10); }
                        else { // Update
                            mkEnt(ent[0].data(),3,randVec(-50.f,50.f),-1); mkEnt(ent[1].data(),3,randVec(-50.f,50.f),0); mkEnt(ent[2].data(),2,randVec(-50.f,50.f),-1);
                            At<void*>(ent[0].data(),0x47C)=intel[0].data(); At<void*>(ent[1].data(),0x47C)=intel[1].data(); At<void*>(ent[2].data(),0x47C)=intel[1].data(); // el EXE lee 0x47C sin mirar el tipo playerPed=next()%2 ? ent[0].data() : nullptr;
                            for(int q=0;q<2;++q){ fill(intel[q].data(),0x200); for(int k=0;k<=16;++k)At<void*>(intel[q].data(),0x130+4*k)=next()%3==0 ? nullptr : (next()%2 ? ent[3].data() : ent[4].data()); }
                            for(int q=3;q<5;++q){ mkEnt(ent[q].data(),3,randVec(-3.f,3.f),-1); At<uint32>(ent[q].data(),0x46C)=next(); At<uint32>(ent[q].data(),0x40)=next(); }
                            static const int32 types[]{37,41,42,18,22,37,41,42};
                            for(int k=0;k<100;++k){ uint8* s=sh+k*0x2C; s[0x28]=uint8(next()%4==0); At<int32>(s,0)=types[next()%std::size(types)]; At<CVector>(s,4)=randVec(-3.f,3.f);
                                const uint32 c=next()%5; At<void*>(s,0x20)=c==0 ? ent[0].data() : c==1 ? ent[1].data() : c==2 ? ent[2].data() : nullptr; s[0x29]=uint8(next()%3==0); }
                            sprayState=int32(next()%3); sprayDir=unitVec();
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["shots"]=Fnv(sh,0x1130); out["shots0"]=ReversingProbe::Bytes(sh,0x2C*8); out["exist"]=[&]{ json j=json::array(); for(int k=0;k<100;++k)j.push_back(sh[k*0x2C+0x28]|sh[k*0x2C+0x29]<<1); return j; }();
                        out["rand_table"]=Fnv(rt,80); out["rand_table0"]=json::array({F(rt[0]),F(rt[7]),F(rt[19])});
                    }
                    else if(cls=="CWaterCannon") {
                        auto* c=cannon.data(); fill(c,0x3CC); At<int16>(c,4)=int16(next()%32); At<uint32>(c,8)=timeMs-next()%300;
                        const CVector base=randVec(-3000.f,3000.f); for(int k=0;k<32;++k){ At<CVector>(c,0xC+k*12)=base+randVec(-6.f,6.f)*float(k%4); At<CVector>(c,0x18C+k*12)=randVec(-2.f,2.f); c[0x30C+k]=uint8(next()%3!=0); }
                        if(next()%6==0)for(int k=0;k<32;++k)c[0x30C+k]=0;
                        fill(reinterpret_cast<void*>(WC_IDX),0x24);
                        G<void*>(0xA9AE24)=fx[0].data(); G<void*>(0xA9AE38)=fx[1].data();
                        // pool de peatones falso
                        mkPool(1,pedStore.data(),0x7C4,4,0xB74490); for(int k=0;k<4;++k){ uint8* p=pedStore.data()+k*0x7C4; mkEnt(p,3,At<CVector>(c,0xC+(next()%32)*12)+randVec(-2.5f,2.5f),next()%2 ? 4+k : -1);
                            At<uint8>(p,0x1C)=uint8(next()%4 ? (next()|1) : (next()&0xFE)); At<void*>(p,0x47C)=intel[k%2].data(); At<uint32>(p,0x46C)=next(); }
                        if(nm=="Init") reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(c,nullptr);
                        else if(nm=="Update_NewInput"){ const CVector s=randVec(-3000.f,3000.f), e=randVec(-3.f,3.f); reinterpret_cast<void(__fastcall*)(void*,void*,const CVector*,const CVector*)>(a)(c,nullptr,&s,&e); }
                        else if(nm=="Update_OncePerFrame"){ if(next()%2)frames&=~3u; reinterpret_cast<void(__fastcall*)(void*,void*,int32)>(a)(c,nullptr,int32(next()%4)); }
                        else if(nm=="PushPeds") reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(c,nullptr);
                        else { // Render
                            setCamera(); losPoint=base+randVec(-3.f,3.f); losNormal=unitVec(); fill(reinterpret_cast<void*>(WC_VERTS),12*0x24);
                            reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(c,nullptr);
                            json vs=json::array(); for(int k=0;k<12;++k){ const auto* w=reinterpret_cast<const uint32*>(WC_VERTS+k*0x24); vs.push_back({w[0],w[1],w[2],w[6]}); } out["verts"]=vs; }
                        out["cannon"]=ReversingProbe::Bytes(c,0x3CC); out["indices"]=ReversingProbe::Bytes(reinterpret_cast<void*>(WC_IDX),0x24);
                        out["peds"]=[&]{ json j=json::array(); for(int k=0;k<4;++k){ uint8* p=pedStore.data()+k*0x7C4; j.push_back({Vp(p+0x44),At<uint32>(p,0x46C)}); } return j; }();
                    }
                    else if(cls=="FxMemoryPool_c") {
                        auto* mp=memPool.data(); fill(mp,0x40);
                        EntryPatch m1(0x72F420,FakeMalloc), m2(0x72F430,FakeFree), m3(0x72F440,FakeRealloc); // CMemoryMgr de la DLL llama a estas direcciones del EXE
                        if(nm=="Init"){ mallocResult=next()%4 ? bigBuf.data() : nullptr; bigBuf[0]=1; bigBuf[0xFFFFF]=2;
                            ret=int(reinterpret_cast<uint8(__fastcall*)(void*,void*)>(a)(mp,nullptr)); out["big"]=json::array({bigBuf[0],bigBuf[0x7FFFF],bigBuf[0xFFFFF],Fnv(bigBuf.data(),0x1000)}); }
                        else if(nm=="Exit"){ At<void*>(mp,0)=next()%4 ? smallBuf.data() : nullptr; reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(mp,nullptr); }
                        else if(nm=="Reset") reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(mp,nullptr);
                        else if(nm=="Optimise"){ At<void*>(mp,0)=next()%4 ? bigBuf.data() : nullptr; reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(mp,nullptr); }
                        else { // GetMem
                            At<void*>(mp,0)=bigBuf.data(); At<uint32>(mp,8)=next()%4==0 ? next() : next()%0x10000;
                            static const int32 aligns[]{1,2,4,8,16,3,5,7,64,int32(0x80000010),int32(0xFFFFFFF0)}; const int32 al=aligns[next()%std::size(aligns)];
                            if(al<0||al>64)At<uint32>(mp,8)=0x80000000u|next();
                            ret=int64(reinterpret_cast<uint8*(__fastcall*)(void*,void*,int32,int32)>(a)(mp,nullptr,int32(next()%0x1000),al)-bigBuf.data()); }
                        out["pool"]=json::array({Id(At<void*>(mp,0)),At<uint32>(mp,4),At<uint32>(mp,8)});
                    }
                    else if(cls=="Input") {
                        auto& tks=G<std::array<uint8,0x270>>(0xB72CB0); fill(tks.data(),0x270); auto* pads=reinterpret_cast<uint8*>(padsAddr); fill(pads,2*0x134);
                        G<uint8>(0xB73400)=uint8(next()%3==0 ? 0 : next()%3); fill(ksBuf.data(),0x40);
                        if(nm=="AttachInputDevices"){
                            ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)());
                            json probe=json::array(); At<int32>(ksBuf.data(),0)=1056; for(auto& [t,h]:attached)probe.push_back({t,int(reinterpret_cast<int32(__cdecl*)(int32,void*)>(h)(28,ksBuf.data()))}); out["probe"]=probe; }
                        else if(nm=="HandleKeyDown"||nm=="HandleKeyUp"){
                            // 0..255 dos veces, 1000..1056 cuatro veces y el resto aleatorio (teclas del pad, negativos pequenos, codigos altos)
                            int32 code=i<512 ? i/2 : i<512+57*4 ? 1000+(i-512)/4 : int32(next()%3==0 ? int32(next()%13)-12 : int32(next()%0x500));
                            if(i>=512+57*4&&next()%2==0){ static const int32 pk[]{'D','A','W','S','J','G','Y','H','Z','X','C','V','O','L','K',';','B','N','M',',','.','/',1047,1050,1056}; code=pk[next()%std::size(pk)]; }
                            At<int32>(ksBuf.data(),0)=code; ret=int(reinterpret_cast<int32(__cdecl*)(void*)>(a)(ksBuf.data())); }
                        else if(nm=="KeyboardHandler"){ static const int32 evs[]{28,29,28,29,0,30,31,27}; const int32 ev=evs[next()%std::size(evs)]; At<int32>(ksBuf.data(),0)=int32(next()%0x430);
                            ret=int(reinterpret_cast<int32(__cdecl*)(int32,void*)>(a)(ev,ksBuf.data())); }
                        else { // PadHandler
                            static const int32 evs[]{31,32,31,32,28,0,33}; const int32 ev=evs[next()%std::size(evs)]; At<int32>(ksBuf.data(),0)=int32(next()%3);
                            for(int k=0;k<17;++k)G<uint8>(0xB70CF4+k)=uint8(next()%5==0 ? 2 : next()%2); G<uint8>(0xBA67A4)=uint8(next()%3==0);
                            activeCam=uint8(next()%3); static const uint16 modes[]{15,17,4,16,18}; for(int k=0;k<3;++k)CamMode(k)=modes[next()%std::size(modes)];
                            ret=int(reinterpret_cast<int32(__cdecl*)(int32,void*)>(a)(ev,ksBuf.data())); }
                        out["tks"]=ReversingProbe::Bytes(tks.data(),0x270); out["pad1"]=ReversingProbe::Bytes(pads+0x134+0x78,0x30); out["pads"]=Fnv(pads,2*0x134);
                    }
                    else if(cls=="JPegPlugin") {
                        fill(camera.data(),0x40); fill(image.data(),0x40); fill(raster.data(),0x40); fill(dstMgr.data(),0x40); fill(srcMgr.data(),0x40);
                        At<int32>(image.data(),4)=int32(1+next()%64); At<int32>(image.data(),8)=int32(next()%17); At<void*>(image.data(),0x14)=pixels.data();
                        G<uint32>(0xBD0B70)=next(); G<uint32>(0xC02B80)=next(); emptyCount=next()%4; termRemain=next()%4==0 ? 0u : next()%0x32000;
                        if(nm=="JPegCompressScreen"){ auto* d=reinterpret_cast<jpeg_destination_mgr*>(dstMgr.data());
                            reinterpret_cast<void(__cdecl*)(void*,jpeg_destination_mgr*)>(a)(camera.data(),d); }
                        else if(nm=="JPegCompressScreenToBuffer"){ EntryPatch cs(dllCompressScreen,FakeCompressScreen), cse(0x5D0470,FakeCompressScreen);
                            static uint8* outPtr{}; outPtr=bigBuf.data(); std::memset(bigBuf.data(),0x5A,0x32000*4+0x100); size_t size=next()%3==0 ? next()%0x40000 : next()%(0x32000*4);
                            reinterpret_cast<void(__cdecl*)(void*,uint8**,size_t&)>(a)(camera.data(),&outPtr,size);
                            out["size"]=uint32(size); out["out"]=Fnv(bigBuf.data(),0x32000*4+0x100); out["globals"]=json::array({G<uint32>(0xC02B7C),G<uint32>(0xC02B80)}); }
                        else if(nm=="JPegCompressScreenToFile"){ EntryPatch cs(dllCompressScreen,FakeCompressScreen), cse(0x5D0470,FakeCompressScreen);
                            const uint32 k=next()%4; openResult=k==0 ? nullptr : k==1 ? reinterpret_cast<FILESTREAM>(uintptr_t(0x80001000u)) : reinterpret_cast<FILESTREAM>(handle.data());
                            static const char* paths[]{"gallery\\gallery1.jpg","snap.jpg","",""}; reinterpret_cast<void(__cdecl*)(void*,const char*)>(a)(camera.data(),paths[next()%4]);
                            out["handle"]=Id(G<void*>(0xBD0B74)); out["handle_raw"]=G<uint32>(0xBD0B74)==0x80001000u; }
                        else if(nm=="JPegDecompressToRaster"){ outHeight=next()%4==0 ? 0u : next()%6; headerResult=next()%5==0 ? int32(next()%3) : 1; At<uint8>(raster.data(),0x23)=uint8(next());
                            fill(reinterpret_cast<void*>(SCAN_BUF),0x900); G<void*>(0xBD0160)=next()%2 ? smallBuf.data() : nullptr; std::memset(bigBuf.data(),0,0x200*4*8);
                            ret=int(reinterpret_cast<uint8(__cdecl*)(void*,void*)>(a)(raster.data(),srcMgr.data()));
                            out["pixels"]=Fnv(bigBuf.data(),0x200*4*8); out["decoding"]=Id(G<void*>(0xBD0160)); out["scan"]=Fnv(reinterpret_cast<void*>(SCAN_BUF),0x900); }
                        else { // JPegDecompressToVramFromBuffer
                            EntryPatch dr(dllDecompressToRaster,FakeDecompressToRaster), dre(0x5D05F0,FakeDecompressToRaster);
                            for(size_t k=0;k<gallery.size();k+=4)*reinterpret_cast<uint32*>(gallery.data()+k)=uint32(k)*2246822519u^seed; G<void*>(0xBA67E0)=gallery.data();
                            ret=int(reinterpret_cast<uint8(__cdecl*)(void*,int32)>(a)(raster.data(),int32(next()%4 ? 1 : 0))); out["file_ptr"]=G<uint32>(0xBD0B70); }
                        out["screen_buf"]=Fnv(reinterpret_cast<void*>(SCREEN_BUF),0x32000);
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
        fpSet=savedSet;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
