#pragma once
// Banco dentro del motor: CObject (48 funciones, lista 34, 10 de ellas por la vtable del EXE 0x866F60).
// Original (hooks de CObject desactivados) frente a candidato con fixtures deterministas. Nunca se toca un objeto
// real: el puntero del pool de objetos (0xB7449C) se cambia por un pool FALSO de 8 huecos de 0x19C (los huecos libres
// llevan 0xCD; CPool::New/Delete del EXE y de la DLL son exactos) y se restaura en cada ejecución. Los objetos falsos se
// construyen con offsets crudos sobre 0xCD con la vptr del EXE (0x866F60); vehículo, peatón, dummy, model infos
// (vtables del EXE de CAtomicModelInfo/CClumpModelInfo/CWeaponModelInfo/CVehicleModelInfo), modelos de colisión,
// CObjectData, RwObject y RwFrame son búferes propios. Los model infos falsos se colocan temporalmente en
// ms_modelInfoPtrs (índice 1/2/3 o el MI_* que se prueba). Create usa además una CMatrixLinkList falsa (los centinelas de
// gMatrixList 0xB74288 se guardan y se restauran) para que AllocateStaticMatrix/AllocateMatrix no toquen matrices reales.
// AddTo/RemoveFromControlCodeList usan nodos reales del pool de nodos dobles (cabecera, estados y almacenamiento
// guardados y restaurados) con objetos del pool falso, y la cabeza de CWorld::ms_listObjectsWithControlCode.
// Se simulan y trazan (dirección del EXE + símbolo y cuerpo de la DLL): rand (0x821B1E y CGeneral::GetRandomNumber),
// FindPlayerPed/Vehicle, GetVehicleAppearance, GetTaskJetPack, AddProjectile, FindNearestTrain, SetLinksBridgeLights,
// CRopes::FindRope/FindPickupHeight/RegisterRope/IsCarriedByRope, CRope::ReleasePickedUpObject, RegisterReference,
// CTxdStore/CColStore::AddRef, CGarages::IsModelIndexADoor/FindGarageForObject, CGarage::BuildRotatedDoorMatrix,
// CanPhysicalBeDamaged, g_breakMan.Add, Add/RemoveFromMovingList, ReportObjectDestruction/ReportDoorMovement,
// CreateMatFromVec, CreateFxSystem (matriz y punto), PlayAndKill, AddParticle, AddExplosion, AddPermanentShadow,
// ProcessVerticalLine, CEntity::PreRender/Render/CreateRwObject/DeleteRwObject/UpdateRpHAnim/IsVisible,
// Get/SetClumpAlpha, RwMatrixScale, RwFrameUpdateObjects, SetVehicleColour, luces (scorched, direccional, ambiente,
// SetupLightingForEntity, RemoveLightsAffectingObject), CWorld::Add/Remove, CPhysical::ProcessControl/ApplyMoveForce/
// ApplyTurnForce/GetSpeed/AttachEntityToEntity, cBuoyancy::ProcessBuoyancy, TryToFreeUpWaterCreatures, el búfer de
// trabajo de las partidas y el destructor con borrado del EXE (0x5A2BD0; libera el hueco del pool falso como el EXE).
// AddToControlCodeList se simula solo dentro de Init y Create. CMatrix, CGeneral::GetATanOfXY, SetObjectData, los
// _CIpow/_CIacos del CRT y las funciones de CObject anidadas (Explode, ObjectDamage, LockDoor, SetIsStatic, ...)
// se ejecutan de verdad. Globales tocadas (reloj, contador de fotogramas, DN balance, SAM, textura de remapeo, humo de
// g_fx, cuerdas, garajes, lista de control, pool de nodos, gMatrixList, ms_modelInfoPtrs) se restauran en cada ejecución.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Object/Object.h"
#include "Entity/Dummy/DummyObject.h"
#include "Ropes.h"
#include "Rope.h"
#include "Garages.h"
#include "Garage.h"
#include "BreakManager_c.h"
#include "Buoyancy.h"
#include "Fx/Fx.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Explosion.h"
#include "Shadows.h"
#include "ProjectileInfo.h"
#include "Entity/Vehicle/Train.h"
#include "PathFind.h"
#include "Renderer.h"
#include "PointLights.h"
#include "VisibilityPlugins.h"
#include "GenericGameStorage.h"
#include "ObjectSaveStructure.h"
#include "Audio/AudioEngine.h"
#include "Models/VehicleModelInfo.h"
#include "PedIntelligence.h"
#include "TxdStore.h"
#include "Collision/ColStore.h"
#include "WaterCreatureManager_c.h"
#include "Pools/Pools.h"
#include "../ProbeRuntime.hpp"

namespace Object48Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
inline DWORD mainThread{};
inline bool Main(){ return GetCurrentThreadId()==mainThread; }
inline void T(json j){ if(Main())trace.push_back(std::move(j)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- módulo de la DLL y cuerpos ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }

// --- direcciones y tamaños del EXE ---
constexpr uintptr_t OBJ_VT=0x866F60, ADDR_OBJ_POOL=0xB7449C, ADDR_MI_PTRS=0xA9B0C8, ADDR_ROPES=0xB768B8, ADDR_CCL_HEAD=0xB9ACCC,
    ADDR_GARAGES=0x96C048, ADDR_MATLIST=0xB74288, ADDR_TIMESTEP=0xB7CB5C, ADDR_TIME_MS=0xB7CB84, ADDR_PREV_MS=0xB7CB78, ADDR_FRAMES=0xB7CB4C,
    ADDR_DN=0x8D12C0, ADDR_SAM_CARRIER=0x8D0A24, ADDR_SAM_AREA51=0xBB4A72, ADDR_REMAP=0xB4E47C, ADDR_SMOKE=0xA9AE24, ADDR_OBJDATA=0xBB4A90,
    ADDR_PATHS=0x96F050, ADDR_BREAKMAN=0xBB4240, ADDR_AUDIO=0xB6BC90, ADDR_GFX=0xA9AE00, ADDR_FXMAN=0xA9AE80, ADDR_BUOY=0xC1C890, ADDR_WCM=0xC1DF30,
    VT_ATOMIC_MI=0x85BBF0, VT_CLUMP_MI=0x85BD30, VT_WEAPON_MI=0x85BD78, VT_VEHICLE_MI=0x85C5C8;
constexpr size_t OBJ_STRIDE=0x19C, OBJ_SIZE=0x17C, ROPE_SIZE=0x328, ROPES_BYTES=8*0x328, GARAGE_SIZE=0xD8, GARAGES_BYTES=50*0xD8,
                 MATLIST_BYTES=0x1FC, LINK=0x54, OBJDATA_BYTES=160*0x50, NODE=0xC;
constexpr int OBJ_CAP=8, NLINK=6, NMAT=8;
constexpr int16 MI_A=1, MI_B=2, MI_R=3;
constexpr uint32 QNAN=0x7FC00000u;

alignas(16) inline std::array<uint8,0x14> objHdr{};
alignas(16) inline std::array<uint8,OBJ_CAP*OBJ_STRIDE> objStore{};
alignas(16) inline std::array<uint8,OBJ_CAP> objStates{};
alignas(16) inline std::array<uint8,0xA00> veh{};
alignas(16) inline std::array<uint8,0x7C4> ped{};
alignas(16) inline std::array<uint8,0x40> dummy{};
alignas(16) inline std::array<std::array<uint8,0x48>,NMAT> mats{};
alignas(16) inline std::array<uint8,0x40> miA{}, miB{}, miR{};
alignas(16) inline std::array<uint8,0x30> colA{}, colB{}, colData{};
alignas(16) inline std::array<uint8,0x50> infoA{}, infoB{};
alignas(16) inline std::array<uint8,0x40> fxBp{}, fxSys{}, smoke{}, intel{}, jetTask{}, train{}, texA{}, texB{}, fire{}, collNode{}, fxVecs{};
alignas(16) inline std::array<uint8,0x20> rwObj{};
alignas(16) inline std::array<uint8,0x100> frame{};
alignas(16) inline std::array<std::array<uint8,LINK>,NLINK> links{};
inline uint8* Obj(int k) { return objStore.data()+k*OBJ_STRIDE; }
inline uint8* Mat(int k) { return mats[k].data(); }

// --- nombres estables para punteros ---
struct Named { uintptr_t p; size_t n; std::string nm; size_t stride; };
inline std::vector<Named> named;
inline void Reg(const void* p,size_t n,std::string nm,size_t stride=0){ named.push_back({uintptr_t(p),n,std::move(nm),stride}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v==OBJ_VT)return "vt_object";
    for(auto& b:named)if(v>=b.p&&v<b.p+b.n){ const size_t off=v-b.p; return b.stride ? b.nm+"#"+std::to_string(off/b.stride)+"+"+std::to_string(off%b.stride) : b.nm+"+"+std::to_string(off); }
    return v;
}
inline json Name(const void* p){ return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n){ json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json V(const CVector* v){ return v ? json{F(v->x),F(v->y),F(v->z)} : json(nullptr); }
inline json Floats(const void* p,int n){ json r=json::array(); for(int k=0;k<n;++k)r.push_back(static_cast<const uint32*>(p)[k]); return r; }

// --- configuración por caso de las dependencias simuladas ---
inline uintptr_t playerPedRet{}, playerVehRet{}, jetTaskRet{}, trainRet{}, fxRet{};
inline int32 appearance{}, ropeIndex{}, garageFound{}, clumpAlpha{};
inline bool canDamage{}, carriedByRope{}, isDoor{}, buoyRet{}, vlineRet{}, lightRet{};
inline float pickupHeight{}; inline uint8 vlineLighting{};
inline CVector buoyTurn{}, buoyForce{};
inline std::array<uint8,0x30> loadBuf{}; inline uint32 loadSize{};
inline uint32 rngState{};

// --- dependencias simuladas ---
inline int __cdecl FakeRand(){ if(!Main())return 0x1234; T({"rand"}); rngState=rngState*1103515245u+12345u; return int((rngState>>16)&0x7FFF); }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ T({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerPedRet); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,uint32 remote){ T({"find_player_vehicle",id,remote&0xFF}); return reinterpret_cast<CVehicle*>(playerVehRet); }
inline int32 __fastcall FakeAppearance(void* v,void*){ T({"vehicle_appearance",Name(v)}); return appearance; }
inline void* __fastcall FakeJetPack(void* in,void*){ T({"task_jetpack",Name(in)}); return reinterpret_cast<void*>(jetTaskRet); }
inline bool __cdecl FakeAddProjectile(void* creator,int32 type,float x,float y,float z,float force,const CVector* dir,void* victim){
    T({"add_projectile",Name(creator),type,F(x),F(y),F(z),F(force),V(dir),Name(victim)}); return true; }
inline void* __cdecl FakeFindNearestTrain(float x,float y,float z,uint32 mainTrain){ T({"find_nearest_train",F(x),F(y),F(z),mainTrain&0xFF}); return reinterpret_cast<void*>(trainRet); }
inline void __fastcall FakeBridgeLights(void* self,void*,float a,float b,float c,float d,uint32 on){ T({"bridge_lights",uintptr_t(self)==ADDR_PATHS,F(a),F(b),F(c),F(d),on&0xFF}); }
inline int32 __cdecl FakeFindRope(uint32 id){ T({"find_rope",Name(id)}); return ropeIndex; }
inline void __fastcall FakeReleaseRope(void* rope,void*){ T({"release_picked_up",Name(rope)}); }
inline float __cdecl FakePickupHeight(void* e){ T({"pickup_height",Name(e)}); return pickupHeight; }
inline bool __cdecl FakeRegisterRope(uint32 id,uint32 type,float x,float y,float z,uint32 expires,uint32 segs,uint32 flags,void* holder,uint32 time){
    T({"register_rope",Name(id),type,F(x),F(y),F(z),expires&0xFF,segs&0xFF,flags&0xFF,Name(holder),time}); return true; }
inline bool __cdecl FakeCarriedByRope(void* e){ T({"is_carried_by_rope",Name(e)}); return carriedByRope; }
inline void __fastcall FakeRegisterReference(void* e,void*,void* slot){ T({"register_reference",Name(e),Name(slot)}); }
inline void __cdecl FakeTxdAddRef(int32 i){ T({"txd_add_ref",i}); }
inline void __cdecl FakeColAddRef(int32 i){ T({"col_add_ref",i}); }
inline bool __cdecl FakeIsDoor(int32 m){ T({"is_model_door",m}); return isDoor; }
inline int32 __cdecl FakeFindGarage(void* o){ T({"find_garage",Name(o)}); return garageFound; }
inline void __cdecl FakeRotatedDoor(void* e,float p){ T({"rotated_door_matrix",Name(e),F(p)}); }
inline void __fastcall FakeAddToCcl(void* o,void*){ T({"add_to_control_code_list",Name(o)}); }
inline bool __fastcall FakeCanBeDamaged(void* self,void*,int32 w,bool* out){ T({"can_be_damaged",Name(self),w,out!=nullptr}); return canDamage; }
inline bool __fastcall FakeBreakAdd(void* self,void*,void* o,const CVector* v,float r,uint32 smash){ T({"break_add",uintptr_t(self)==ADDR_BREAKMAN,Name(o),V(v),F(r),smash&0xFF}); return true; }
inline void __fastcall FakeRemoveMoving(void* s,void*){ T({"remove_from_moving_list",Name(s)}); }
inline void __fastcall FakeAddMoving(void* s,void*){ T({"add_to_moving_list",Name(s)}); }
inline void __fastcall FakeReportDestruction(void* self,void*,void* e){ T({"report_destruction",uintptr_t(self)==ADDR_AUDIO,Name(e)}); }
inline void __fastcall FakeReportDoor(void* self,void*,void* e){ T({"report_door",uintptr_t(self)==ADDR_AUDIO,Name(e)}); }
// la matriz de salida es un local del llamador: se rellena entera de forma determinista y no se traza su dirección
inline void __fastcall FakeMatFromVec(void* self,void*,RwMatrix* out,const CVector* o,const CVector* d){
    T({"mat_from_vec",uintptr_t(self)==ADDR_GFX,V(o),V(d)});
    auto* w=reinterpret_cast<float*>(out); for(int k=0;k<16;++k)w[k]=(o ? o->x : 0.f)*float(k)+(d ? d->y : 0.f)-float(k)*0.25f; }
inline FxSystem_c* __fastcall FakeCreateFxMat(void* self,void*,void* bp,const RwMatrix* m,RwMatrix* objMat,uint32 ign){
    T({"create_fx_matrix",uintptr_t(self)==ADDR_FXMAN,Name(bp),Floats(m,16),Name(objMat),ign&0xFF}); return reinterpret_cast<FxSystem_c*>(fxRet); }
inline FxSystem_c* __fastcall FakeCreateFxPoint(void* self,void*,void* bp,const CVector* p,RwMatrix* objMat,uint32 ign){
    T({"create_fx_point",uintptr_t(self)==ADDR_FXMAN,Name(bp),V(p),Name(objMat),ign&0xFF}); return reinterpret_cast<FxSystem_c*>(fxRet); }
inline void __fastcall FakePlayAndKill(void* fx,void*){ T({"play_and_kill",Name(fx)}); }
inline void __fastcall FakeAddParticle(void* self,void*,const CVector* pos,const CVector* vel,float t,const FxPrtMult_c* m,float rz,float lm,float lml,uint32 local){
    T({"particle",Name(self),V(pos),V(vel),F(t),Floats(m,7),F(rz),F(lm),F(lml),local&0xFF}); }
inline void __cdecl FakeAddExplosion(void* victim,void* creator,int32 type,float x,float y,float z,uint32 life,uint32 sound,float shake,uint32 vis){
    T({"add_explosion",Name(victim),Name(creator),type,F(x),F(y),F(z),life,sound&0xFF,F(shake),vis&0xFF}); }
inline void __cdecl FakePermShadow(uint32 type,void* tex,const CVector* pos,float tx,float ty,float rx,float ry,int32 inten,uint32 r,uint32 g,uint32 b,float dd,uint32 time,float up){
    T({"permanent_shadow",type&0xFF,Name(tex),V(pos),F(tx),F(ty),F(rx),F(ry),int16(inten),r&0xFF,g&0xFF,b&0xFF,F(dd),time,F(up)}); }
// el punto de colisión es un local del llamador (en el EXE sin inicializar): solo se escribe la iluminación B
inline bool __cdecl FakeVerticalLine(const CVector* o,float dist,CColPoint* cp,CEntity** ent,uint32 b1,uint32 b2,uint32 b3,uint32 b4,uint32 b5,uint32 b6,void* poly){
    T({"vertical_line",V(o),F(dist),b1&0xFF,b2&0xFF,b3&0xFF,b4&0xFF,b5&0xFF,b6&0xFF,Name(poly)});
    if(vlineRet)reinterpret_cast<uint8*>(cp)[0x25]=vlineLighting; return vlineRet; }
inline void __fastcall FakeEntityPreRender(void* e,void*){ T({"entity_prerender",Name(e)}); }
inline void __fastcall FakeUpdateRpHAnim(void* e,void*){ T({"update_rphanim",Name(e)}); }
inline int32 __cdecl FakeGetClumpAlpha(void* c){ T({"get_clump_alpha",Name(c)}); return clumpAlpha; }
inline void __cdecl FakeSetClumpAlpha(void* c,int32 a){ T({"set_clump_alpha",Name(c),a}); }
inline RwMatrix* __cdecl FakeMatrixScale(RwMatrix* m,const RwV3d* s,int32 op){ T({"matrix_scale",Name(m),F(s->x),F(s->y),F(s->z),op}); return m; }
inline RwFrame* __cdecl FakeFrameUpdate(RwFrame* f){ T({"frame_update_objects",Name(f)}); return f; }
inline void __fastcall FakeEntityRender(void* e,void*){ T({"entity_render",Name(e)}); }
inline void __fastcall FakeSetVehColour(void* mi,void*,uint32 a,uint32 b,uint32 c,uint32 d){ T({"set_vehicle_colour",Name(mi),a&0xFF,b&0xFF,c&0xFF,d&0xFF,Name(*reinterpret_cast<uintptr_t*>(ADDR_REMAP))}); }
inline void __cdecl FakeScorched(void* world,float l){ T({"scorched",uintptr_t(world),F(l)}); }
inline void __cdecl FakeActivateDir(){ T({"activate_directional"}); }
inline void __cdecl FakeDeactivateDir(){ T({"deactivate_directional"}); }
inline void __cdecl FakeAmbient(){ T({"set_ambient_colours"}); }
inline bool __cdecl FakeSetupLightingEntity(void* e){ T({"setup_lighting_entity",Name(e)}); return lightRet; }
inline void __cdecl FakeRemoveLights(){ T({"remove_lights_affecting_object"}); }
inline void __fastcall FakeEntityCreateRw(void* e,void*){ T({"entity_create_rw",Name(e)}); }
inline void __cdecl FakeWorldRemove(void* e){ T({"world_remove",Name(e)}); }
inline void __cdecl FakeWorldAdd(void* e){ T({"world_add",Name(e)}); }
// visible = bandera de visibilidad y bit 3 de la semilla (sin cámara)
inline bool __fastcall FakeIsVisible(void* e,void*){ T({"is_visible",Name(e)}); return (At<uint32>(e,0x1C)&0x80)&&((At<uint16>(e,0x20)>>3)&1); }
inline void __fastcall FakeDeleteRw(void* e,void*){ T({"delete_rw_object",Name(e)}); At<uint32>(e,0x18)=0; }
// destructor con borrado del EXE: libera el hueco del pool falso como CPool::Delete del EXE (sin rellenar)
inline void* __fastcall FakeDeletingDtor(void* o,void*,uint32 flags){
    T({"deleting_dtor",Name(o),flags});
    const auto d=intptr_t(o)-intptr_t(objStore.data());
    if(d>=0&&d<intptr_t(objStore.size())&&d%intptr_t(OBJ_STRIDE)==0){ const int32 idx=int32(d/intptr_t(OBJ_STRIDE)); objStates[idx]|=0x80; auto& last=At<int32>(objHdr.data(),0xC); if(idx<last)last=idx; }
    return o; }
inline void __fastcall FakePhysProcessControl(void* e,void*){ T({"physical_process_control",Name(e)}); }
inline void __fastcall FakeApplyMoveForce(void* e,void*,float x,float y,float z){ T({"apply_move_force",Name(e),F(x),F(y),F(z)}); }
inline void __fastcall FakeApplyTurnForce(void* e,void*,float fx,float fy,float fz,float px,float py,float pz){ T({"apply_turn_force",Name(e),F(fx),F(fy),F(fz),F(px),F(py),F(pz)}); }
inline CVector* __fastcall FakeGetSpeed(void* e,void*,CVector* out,float px,float py,float pz){
    T({"get_speed",Name(e),F(px),F(py),F(pz)});
    const auto* ms=reinterpret_cast<const float*>(static_cast<uint8*>(e)+0x44);
    *out=CVector{ms[0]+0.05f*py-0.01f*pz, ms[1]-0.05f*px, ms[2]+0.02f*px-0.03f*py}; return out; }
inline void __fastcall FakeAttach(void* e,void*,void* to,CVector* off,void* rot){ T({"attach_entity",Name(e),Name(to),Name(off),Name(rot)}); }
inline bool __fastcall FakeProcessBuoyancy(void* self,void*,void* e,float buoy,CVector* turn,CVector* force){
    T({"process_buoyancy",uintptr_t(self)==ADDR_BUOY,Name(e),F(buoy)}); if(buoyRet){ *turn=buoyTurn; *force=buoyForce; } return buoyRet; }
inline int32 __fastcall FakeWaterCreatures(void* self,void*,int32 n){ T({"free_water_creatures",uintptr_t(self)==ADDR_WCM,n}); return 0; }
// búfer de trabajo de las partidas: el struct guardado es un local (en el EXE con relleno sin inicializar):
// se trazan los campos y se enmascaran los rellenos (0x12..0x17, 0x19, 0x25..0x27)
inline bool __cdecl FakeSaveWork(void* data,int32 size){
    const auto* b=static_cast<const uint8*>(data); json bytes=json::array();
    for(int32 k=0;k<size;++k){ const bool pad=size==0x30&&((k>=0x12&&k<0x18)||k==0x19||(k>=0x25&&k<0x28)); bytes.push_back(pad ? -1 : int(b[k])); }
    T({"save_work",size,bytes}); return true; }
inline bool __cdecl FakeLoadWork(void* data,int32 size){
    T({"load_work",size}); if(size==4)std::memcpy(data,&loadSize,4); else std::memcpy(data,loadBuf.data(),std::min<size_t>(size_t(size),loadBuf.size())); return true; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=48;
inline const std::array<Fn,NFNS> fns{{
    {"CObject","CanBeDeleted",0x59F120,0,-1,128},
    {"CObject","CanBeTargetted",0x59F320,0,-1,128},
    {"CObject","SetObjectTargettable",0x59F300,0,-1,128},
    {"CObject","TryToExplode",0x59F2D0,0,-1,256},
    {"CObject","RefModelInfo",0x59F330,0,-1,128},
    {"CObject","SetRemapTexture",0x59F350,0,-1,128},
    {"CObject","SetRelatedDummy",0x59F160,0,-1,128},
    {"CObject","GetRopeHeight",0x59F380,0,-1,128},
    {"CObject","SetRopeHeight",0x59F3A0,0,-1,128},
    {"CObject","GetObjectCarriedWithRope",0x59F3C0,0,-1,128},
    {"CObject","ReleaseObjectCarriedWithRope",0x59F3E0,0,-1,128},
    {"CObject","GrabObjectToCarryWithRope",0x5A1AB0,0,-1,512},
    {"CObject","AddToControlCodeList",0x59F400,0,-1,256},
    {"CObject","RemoveFromControlCodeList",0x59F450,0,-1,256},
    {"CObject","ResetDoorAngle",0x59F4B0,0,-1,256},
    {"CObject","LockDoor",0x59F5C0,0,-1,256},
    {"CObject","SetIsStatic",0,0x866F60,4,512},
    {"CObject","Init",0x59F840,0,-1,1024},
    {"CObject","DoBurnEffect",0x59FB50,0,-1,512},
    {"CObject","GetLightingFromCollisionBelow",0x59FD00,0,-1,256},
    {"CObject","PreRender",0,0x866F60,17,512},
    {"CObject","Render",0,0x866F60,18,256},
    {"CObject","SetupLighting",0,0x866F60,19,128},
    {"CObject","RemoveLighting",0,0x866F60,20,128},
    {"CObject","CreateRwObject",0,0x866F60,7,64},
    {"CObject","Teleport",0,0x866F60,14,128},
    {"CObject","SpecialEntityPreCollisionStuff",0,0x866F60,15,1024},
    {"CObject","SpecialEntityCalcCollisionSteps",0,0x866F60,16,1024},
    {"CObject","ProcessSamSiteBehaviour",0x5A07D0,0,-1,1024},
    {"CObject","ProcessTrainCrossingBehaviour",0x5A0B50,0,-1,512},
    {"CObject","SetMatrixForTrainCrossing",0x59F200,0,-1,512},
    {"CObject","ProcessGarageDoorBehaviour",0x44A4D0,0,-1,512},
    {"CObject","ProcessControlLogic",0x5A29A0,0,-1,512},
    {"CObject","ObjectDamage",0x5A0D90,0,-1,1024},
    {"CObject","Explode",0x5A1340,0,-1,512},
    {"CObject","ObjectFireDamage",0x5A1580,0,-1,512},
    {"CObject","CanBeUsedToTakeCoverBehind",0x5A1B60,0,-1,512},
    {"CObject","ProcessControl",0,0x866F60,10,1024},
    {"CObject","TryToFreeUpTempObjects",0x5A1840,0,-1,256},
    {"CObject","DeleteAllTempObjects",0x5A18B0,0,-1,128},
    {"CObject","DeleteAllMissionObjects",0x5A1910,0,-1,128},
    {"CObject","DeleteAllTempObjectsInArea",0x5A1980,0,-1,512},
    {"CObject","Create-intbool",0x5A1F60,0,-1,256},
    {"CObject","Create-dummy",0x5A2070,0,-1,256},
    {"CObject","IsObjectPointerValid",0x5A2C20,0,-1,512},
    {"CObject","IsObjectPointerValid_NotInWorld",0x5A2B90,0,-1,512},
    {"CObject","Save",0x5D2830,0,-1,256},
    {"CObject","Load",0x5D2870,0,-1,256},
}};

// lectura cruda de un ModelIndices::MI_* (word del EXE); 0 si no está cargado
inline int16 MiAt(uintptr_t a){ const uint16 v=*reinterpret_cast<const uint16*>(a); return v>0&&v<20000 ? int16(v) : int16(0); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"object48-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"object48-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("object48");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"object48-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // cuerpos de la DLL de funciones virtuales de la base (el símbolo de una virtual es un thunk de llamada virtual)
    struct Base { const char* cls; const char* name; uintptr_t exe; uintptr_t stub; uintptr_t dll; };
    std::vector<Base> bases{
        {"CEntity","PreRender",0x535FA0,Addr(&FakeEntityPreRender),0},{"CEntity","Render",0x534310,Addr(&FakeEntityRender),0},
        {"CEntity","CreateRwObject",0x533D30,Addr(&FakeEntityCreateRw),0},{"CEntity","DeleteRwObject",0x534030,Addr(&FakeDeleteRw),0},
        {"CPhysical","ProcessControl",0x5485E0,Addr(&FakePhysProcessControl),0}};
    for(auto& bs:bases)if(!(bs.dll=DllBody(bs.cls,bs.name))){Write({{"event","error"},{"reason","base body not found"},{"function",std::string(bs.cls)+"::"+bs.name}});return;}
    // cuerpo de la DLL de AddToControlCodeList (se simula dentro de Init y Create)
    const uintptr_t cclDll=Addr(&CObject::AddToControlCodeList), cclBody=FollowIn(cclDll);

    // nombres
    named.clear();
    Reg(objHdr.data(),objHdr.size(),"objHdr"); Reg(objStore.data(),objStore.size(),"obj",OBJ_STRIDE); Reg(objStates.data(),objStates.size(),"objState");
    Reg(veh.data(),veh.size(),"veh"); Reg(ped.data(),ped.size(),"ped"); Reg(dummy.data(),dummy.size(),"dummy"); Reg(mats.data(),sizeof(mats),"mat",0x48);
    Reg(miA.data(),miA.size(),"miA"); Reg(miB.data(),miB.size(),"miB"); Reg(miR.data(),miR.size(),"miR");
    Reg(colA.data(),colA.size(),"colA"); Reg(colB.data(),colB.size(),"colB"); Reg(colData.data(),colData.size(),"colData");
    Reg(infoA.data(),infoA.size(),"infoA"); Reg(infoB.data(),infoB.size(),"infoB");
    Reg(fxBp.data(),fxBp.size(),"fxBp"); Reg(fxSys.data(),fxSys.size(),"fxSys"); Reg(smoke.data(),smoke.size(),"smoke"); Reg(intel.data(),intel.size(),"intel");
    Reg(jetTask.data(),jetTask.size(),"jetTask"); Reg(train.data(),train.size(),"train"); Reg(texA.data(),texA.size(),"texA"); Reg(texB.data(),texB.size(),"texB");
    Reg(fire.data(),fire.size(),"fire"); Reg(collNode.data(),collNode.size(),"collNode"); Reg(fxVecs.data(),fxVecs.size(),"fxVecs");
    Reg(rwObj.data(),rwObj.size(),"rw"); Reg(frame.data(),frame.size(),"frame"); Reg(links.data(),sizeof(links),"link",LINK);
    Reg(reinterpret_cast<void*>(ADDR_ROPES),ROPES_BYTES,"rope",ROPE_SIZE); Reg(reinterpret_cast<void*>(ADDR_GARAGES),GARAGES_BYTES,"garage",GARAGE_SIZE);
    Reg(reinterpret_cast<void*>(ADDR_OBJDATA),OBJDATA_BYTES,"objdata",0x50); Reg(reinterpret_cast<void*>(ADDR_MATLIST),MATLIST_BYTES,"matlist",LINK);
    auto* const nodePool=reinterpret_cast<uint8*>(GetPtrNodeDoubleLinkPool());
    uint8* const nodeStore=At<uint8*>(nodePool,0); uint8* const nodeStates=At<uint8*>(nodePool,4); const int32 nodeCap=At<int32>(nodePool,8);
    Reg(nodeStore,size_t(nodeCap)*NODE,"node",NODE);

    // estado global guardado y restaurado en cada ejecución
    auto& objPoolPtr=*reinterpret_cast<void**>(ADDR_OBJ_POOL); void* const savedObjPool=objPoolPtr;
    auto** const miPtrs=reinterpret_cast<void**>(ADDR_MI_PTRS);
    std::vector<std::pair<int,void*>> miTouched;
    struct Saved { void* p; size_t n; std::vector<uint8> saved; };
    std::vector<Saved> regions;
    auto Snap=[&](uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); };
    Snap(ADDR_TIMESTEP,4); Snap(ADDR_TIME_MS,4); Snap(ADDR_PREV_MS,4); Snap(ADDR_FRAMES,4); Snap(ADDR_DN,4); Snap(ADDR_SAM_CARRIER,1); Snap(ADDR_SAM_AREA51,1);
    Snap(ADDR_REMAP,4); Snap(ADDR_SMOKE,4); Snap(ADDR_ROPES,ROPES_BYTES); Snap(ADDR_CCL_HEAD,4); Snap(ADDR_GARAGES,GARAGES_BYTES); Snap(ADDR_MATLIST,MATLIST_BYTES);
    Snap(uintptr_t(nodePool),0x14); Snap(uintptr_t(nodeStates),size_t(nodeCap)); Snap(uintptr_t(nodeStore),size_t(nodeCap)*NODE);
    const std::vector<uint8> savedNodeStates(nodeStates,nodeStates+nodeCap);
    auto restoreAll=[&]{
        for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n);
        for(auto it=miTouched.rbegin();it!=miTouched.rend();++it)miPtrs[it->first]=it->second;
        miTouched.clear(); objPoolPtr=savedObjPool; };
    auto stateRestored=[&]{ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return objPoolPtr==savedObjPool&&miTouched.empty(); };
    auto setMi=[&](int idx,void* p){ if(idx<=0||idx>=20000)return; miTouched.push_back({idx,miPtrs[idx]}); miPtrs[idx]=p; };

    // modelos MI_* del EXE que se prueban
    const int16 MI_BUOY=MiAt(0x8CD608), MI_SAMSITE=MiAt(0x8CD6AC), MI_SAMSITE2=MiAt(0x8CD6B0), MI_TRAINCROSSING=MiAt(0x8CD6B4), MI_TRAINCROSSING1=MiAt(0x8CD6B8),
        MI_MAGNOCRANE=MiAt(0x8CD6BC), MI_CRANETROLLEY=MiAt(0x8CD6C0), MI_QUARRYCRANE_ARM=MiAt(0x8CD6C4), MI_RCBOMB=MiAt(0x8CD634), MI_GRASSPLANT=MiAt(0x8CD6F8),
        MI_GRASSHOUSE=MiAt(0x8CD6F4), MI_FIRE_HYDRANT=MiAt(0x8CD594), MI_SHASH_WALL=MiAt(0x8CD748), MI_MLAMPPOST=MiAt(0x8CD614), MI_TRAFFIC1=MiAt(0x8CD4FC),
        MI_STREET1=MiAt(0x8CD518), MI_DOUBLESTREET=MiAt(0x8CD524), MI_CRANE_MAGNET=MiAt(0x8CD700), MI_WRECKING_BALL=MiAt(0x8CD71C);

    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using std::bit_cast;
        Patches p;
        // Cuerpos de la DLL resueltos ANTES de escribir parches; nunca se parchea un stub ya registrado.
        std::set<uintptr_t> stubSet;
        auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
            const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
            stubSet.insert(st); stubSet.insert(stb);
            auto ok=[&](uintptr_t a){ return a&&!stubSet.count(a); };
            if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
        for(auto& bs:bases){ stubSet.insert(bs.stub); stubSet.insert(FollowIn(bs.stub)); }
        for(auto& bs:bases){ p.Add(bs.exe,reinterpret_cast<void*>(bs.stub)); p.Add(bs.dll,reinterpret_cast<void*>(bs.stub)); }
        P(0x821B1E,0,FakeRand); P(0,Addr(&CGeneral::GetRandomNumber),FakeRandomNumber);
        P(0x56E210,Addr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed);
        P(0x56E0D0,Addr(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle);
        P(0x6D1080,Addr(&CVehicle::GetVehicleAppearance),FakeAppearance); P(0,Addr(&CVehicle::GetVehicleAppearance_Reversed),FakeAppearance);
        P(0x601110,Addr(&CPedIntelligence::GetTaskJetPack),FakeJetPack);
        P(0x737C80,Addr(&CProjectileInfo::AddProjectile),FakeAddProjectile);
        P(0x6F7090,Addr(&CTrain::FindNearestTrain),FakeFindNearestTrain);
        P(0x44D960,Addr(&CPathFind::SetLinksBridgeLights),FakeBridgeLights);
        P(0x556000,Addr(&CRopes::FindRope),FakeFindRope); P(0x556030,Addr(&CRope::ReleasePickedUpObject),FakeReleaseRope);
        P(0x556760,Addr(&CRopes::FindPickupHeight),FakePickupHeight); P(0x556B40,Addr(&CRopes::RegisterRope),FakeRegisterRope);
        P(0x555F80,Addr(&CRopes::IsCarriedByRope),FakeCarriedByRope);
        P(0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegisterReference);
        P(0x731A00,Addr(&CTxdStore::AddRef),FakeTxdAddRef); P(0x4107A0,Addr(&CColStore::AddRef),FakeColAddRef);
        P(0x448AF0,Addr(&CGarages::IsModelIndexADoor),FakeIsDoor); P(0x44A240,Addr(&CGarages::FindGarageForObject),FakeFindGarage);
        P(0x4479F0,Addr(&CGarage::BuildRotatedDoorMatrix),FakeRotatedDoor);
        P(0x5448B0,Addr(&CPhysical::CanPhysicalBeDamaged),FakeCanBeDamaged);
        P(0x59E9B0,Addr(&BreakManager_c::Add),FakeBreakAdd);
        P(0x542860,Addr(&CPhysical::RemoveFromMovingList),FakeRemoveMoving); P(0x542800,Addr(&CPhysical::AddToMovingList),FakeAddMoving);
        P(0x506ED0,Addr(&CAudioEngine::ReportObjectDestruction),FakeReportDestruction); P(0x506F50,Addr(&CAudioEngine::ReportDoorMovement),FakeReportDoor);
        P(0x49E950,Addr(&Fx_c::CreateMatFromVec),FakeMatFromVec);
        P(0x4A95C0,Addr(static_cast<FxSystem_c*(FxManager_c::*)(FxSystemBP_c*,const RwMatrix&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),FakeCreateFxMat);
        P(0x4A96B0,Addr(static_cast<FxSystem_c*(FxManager_c::*)(FxSystemBP_c*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),FakeCreateFxPoint);
        P(0x4AA3D0,Addr(&FxSystem_c::PlayAndKill),FakePlayAndKill);
        P(0x4AA440,Addr(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),FakeAddParticle);
        P(0x736A50,Addr(&CExplosion::AddExplosion),FakeAddExplosion);
        P(0x706F60,Addr(&CShadows::AddPermanentShadow),FakePermShadow);
        P(0x5674E0,Addr(&CWorld::ProcessVerticalLine),FakeVerticalLine);
        P(0x532B20,Addr(&CEntity::UpdateRpHAnim),FakeUpdateRpHAnim);
        P(0x732B20,Addr(&CVisibilityPlugins::GetClumpAlpha),FakeGetClumpAlpha); P(0x732B00,Addr(&CVisibilityPlugins::SetClumpAlpha),FakeSetClumpAlpha);
        P(0x7F22C0,Addr(&RwMatrixScale),FakeMatrixScale); P(0x7F0910,Addr(&RwFrameUpdateObjects),FakeFrameUpdate);
        P(0x4C84B0,Addr(&CVehicleModelInfo::SetVehicleColour),FakeSetVehColour);
        P(0x7357E0,Addr(&WorldReplaceNormalLightsWithScorched),FakeScorched); P(0x735C80,Addr(&ActivateDirectional),FakeActivateDir);
        P(0x735C70,Addr(&DeActivateDirectional),FakeDeactivateDir); P(0x735D30,Addr(static_cast<void(*)()>(&SetAmbientColours)),FakeAmbient);
        P(0x553E40,Addr(&CRenderer::SetupLightingForEntity),FakeSetupLightingEntity); P(0x6FFFE0,Addr(&CPointLights::RemoveLightsAffectingObject),FakeRemoveLights);
        P(0x563280,Addr(&CWorld::Remove),FakeWorldRemove); P(0x563220,Addr(&CWorld::Add),FakeWorldAdd);
        P(0x536BC0,Addr(&CEntity::IsVisible),FakeIsVisible);
        P(0x5A2BD0,0,FakeDeletingDtor);
        P(0x5429F0,Addr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),FakeApplyMoveForce);
        P(0x542A50,Addr(&CPhysical::ApplyTurnForce),FakeApplyTurnForce); P(0x542CE0,Addr(&CPhysical::GetSpeed),FakeGetSpeed);
        P(0x54D690,Addr(static_cast<void(CPhysical::*)(CPhysical*,CVector*,CQuaternion*)>(&CPhysical::AttachEntityToEntity)),FakeAttach);
        P(0x6C3EF0,Addr(&cBuoyancy::ProcessBuoyancy),FakeProcessBuoyancy);
        P(0x6E40E0,Addr(&WaterCreatureManager_c::TryToFreeUpWaterCreatures),FakeWaterCreatures);
        P(0x5D1270,Addr(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWork);
        P(0x5D1300,Addr(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FakeLoadWork);
        if(stubSet.count(cclDll)||stubSet.count(cclBody)){Write({{"event","error"},{"reason","AddToControlCodeList collides with a stub"}});return;}

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto vec=[&](float r){return CVector{uni(-r,r),uni(-r,r),uni(-r,r)};};
                auto pickI=[&](std::initializer_list<int32> l){return *(l.begin()+next()%l.size());};
                auto pickF=[&](std::initializer_list<float> l){return *(l.begin()+next()%l.size());};
                auto coin=[&](int n){return next()%uint32(n)==0;};

                // matriz: z con inclinación pequeña (o arbitraria) y posición; columnas de relleno a 0
                auto fillMat=[&](uint8* m,const CVector& pos,bool tilt){
                    std::memset(m,0,0x48); const float a=uni(-3.2f,3.2f), c=std::cos(a), s=std::sin(a);
                    At<CVector>(m,0x00)=CVector{c,s,tilt ? uni(-0.4f,0.4f) : uni(-0.05f,0.05f)};
                    At<CVector>(m,0x10)=CVector{-s,c,tilt ? uni(-0.4f,0.4f) : uni(-0.05f,0.05f)};
                    At<CVector>(m,0x20)=CVector{tilt ? uni(-0.6f,0.6f) : uni(-0.05f,0.05f),tilt ? uni(-0.6f,0.6f) : uni(-0.05f,0.05f),tilt ? uni(-1.f,1.f) : uni(0.9f,1.f)};
                    At<CVector>(m,0x30)=pos; };
                auto entPos=[&](uint8* e)->CVector{ void* m=At<void*>(e,0x14); return m ? At<CVector>(m,0x30) : At<CVector>(e,0x04); };
                // objeto falso en el hueco k del pool falso (0xCD, vptr del EXE, campos crudos)
                auto makeObject=[&](int k,int16 model,void* info,bool withMatrix){
                    uint8* o=Obj(k); std::memset(o,0xCD,OBJ_STRIDE);
                    At<uintptr_t>(o,0)=OBJ_VT; const CVector pos=vec(60.f); At<CVector>(o,0x04)=pos; At<float>(o,0x10)=uni(-3.2f,3.2f);
                    fillMat(Mat(k),pos+vec(next()%3 ? 0.f : 1.f),coin(4)); At<void*>(o,0x14)=withMatrix ? Mat(k) : nullptr;
                    At<void*>(o,0x18)=nullptr; At<uint32>(o,0x1C)=next(); At<uint16>(o,0x20)=uint16(next()); At<int16>(o,0x22)=model;
                    At<void*>(o,0x24)=nullptr; At<void*>(o,0x28)=nullptr; At<uint16>(o,0x2C)=0; o[0x2E]=uint8(next()); o[0x2F]=uint8(next());
                    At<void*>(o,0x30)=nullptr; o[0x34]=0; o[0x35]=0; o[0x36]=uint8((next()&0xF8)|4); o[0x37]=uint8(next());
                    At<float>(o,0x38)=100.f; At<uint32>(o,0x3C)=next(); At<uint32>(o,0x40)=next();
                    for(size_t off=0x44;off<0x8C;off+=4)At<float>(o,off)=next()%4 ? uni(-0.1f,0.1f) : uni(-0.002f,0.002f);
                    At<float>(o,0x8C)=uni(1.f,500.f); At<float>(o,0x90)=uni(1.f,800.f); for(size_t off=0x94;off<0xB0;off+=4)At<float>(o,off)=uni(0.f,1.f);
                    At<void*>(o,0xB0)=coin(2) ? collNode.data() : nullptr; At<void*>(o,0xB4)=nullptr; o[0xB8]=uint8(next()%13); o[0xB9]=0; o[0xBA]=uint8(next()); o[0xBB]=uint8(next());
                    for(size_t off=0xBC;off<0xD8;off+=4)At<void*>(o,off)=nullptr;
                    At<float>(o,0xD8)=coin(3) ? 0.f : uni(0.f,30.f); At<void*>(o,0xDC)=nullptr;
                    At<CVector>(o,0xE0)=vec(1.f); At<CVector>(o,0xEC)=pos+vec(2.f); At<uint32>(o,0xF8)=next();
                    At<void*>(o,0xFC)=nullptr; for(size_t off=0x100;off<0x128;off+=4)At<float>(o,off)=uni(-1.f,1.f);
                    At<void*>(o,0x128)=nullptr; At<float>(o,0x12C)=uni(0.f,1.f); At<float>(o,0x130)=uni(0.f,1.f); At<void*>(o,0x134)=nullptr;
                    At<void*>(o,0x138)=nullptr; o[0x13C]=uint8(next()%7); o[0x13D]=uint8(next()); At<uint16>(o,0x13E)=uint16(next()); At<uint32>(o,0x140)=next();
                    o[0x144]=uint8(pickI({0,1,20,21,200,202,50})); o[0x145]=uint8(next()%10); o[0x146]=uint8(next()); o[0x147]=uint8(pickI({-1,-1,0,1,2,5}));
                    o[0x148]=uint8(next()); o[0x149]=uint8(next()); At<int16>(o,0x14A)=-1; At<uint32>(o,0x14C)=next(); At<uint32>(o,0x150)=next();
                    At<float>(o,0x154)=pickF({1000.f,0.f,-0.f,150.f})*(coin(2) ? 1.f : uni(0.f,1.f)); At<float>(o,0x158)=coin(2) ? -1001.f : uni(-3.2f,3.2f);
                    At<float>(o,0x15C)=coin(2) ? 1.f : uni(0.2f,3.f); At<void*>(o,0x160)=info; At<void*>(o,0x164)=nullptr; At<int16>(o,0x168)=-1;
                    At<int16>(o,0x16A)=-1; At<void*>(o,0x16C)=coin(2) ? texA.data() : nullptr; At<void*>(o,0x170)=dummy.data();
                    At<uint32>(o,0x174)=next()%2 ? 0 : next()%20000; At<float>(o,0x178)=uni(0.f,2.f);
                    objStates[k]=uint8(next()&0x7F); };
                auto makeInfo=[&](uint8* in){
                    for(size_t off=0;off<0x50;off+=4)At<float>(in,off)=uni(0.f,5.f);
                    At<float>(in,0x00)=uni(1.f,500.f); At<float>(in,0x04)=uni(1.f,800.f); At<float>(in,0x14)=pickF({0.f,-1.f,uni(0.f,50.f)});
                    At<float>(in,0x18)=pickF({1.f,0.5f,2.f,uni(0.f,4.f)}); in[0x1C]=uint8(pickI({0,1,20,21,200,202})); in[0x1D]=uint8(pickI({0,1,2,3,4,5,6,7,8,9}));
                    in[0x1E]=uint8(next()%3); in[0x1F]=uint8(pickI({0,0,1,2})); in[0x20]=uint8(pickI({0,1,2,3,4}));
                    At<CVector>(in,0x24)=coin(3) ? CVector{-1000.f,0.f,0.f} : vec(2.f); At<void*>(in,0x30)=fxBp.data(); At<float>(in,0x34)=uni(0.f,3.f);
                    At<CVector>(in,0x38)=vec(1.f); At<float>(in,0x44)=uni(0.f,1.f); At<uint32>(in,0x48)=next()%4; At<uint32>(in,0x4C)=next()%2; };
                auto makeCol=[&](uint8* c,float scale){
                    std::memset(c,0,0x30);
                    CVector mn{-uni(0.1f,scale),-uni(0.1f,scale),-uni(0.1f,scale)}, mx{uni(0.1f,scale),uni(0.1f,scale),uni(0.1f,scale)};
                    if(coin(8))std::swap(mn.z,mx.z);
                    At<CVector>(c,0)=mn; At<CVector>(c,0xC)=mx; At<CVector>(c,0x18)=vec(0.5f); At<float>(c,0x24)=coin(16) ? 0.f : uni(0.05f,scale*1.5f);
                    c[0x28]=uint8(next()%200); c[0x29]=uint8(next()); At<void*>(c,0x2C)=coin(3) ? nullptr : colData.data(); };
                auto makeMi=[&](uint8* m,uintptr_t vt,void* col){
                    std::memset(m,0xCD,0x40); At<uintptr_t>(m,0)=vt; At<uint32>(m,4)=next(); At<uint16>(m,8)=uint16(next()%50); At<int16>(m,0xA)=int16(next()%300);
                    m[0xC]=255; m[0xD]=0; At<int16>(m,0xE)=-1; At<int16>(m,0x10)=int16(pickI({-1,-1,0,1,2,3,5,10,40,100,159}));
                    At<uint16>(m,0x12)=uint16(pickI({0,0,0x800,0x1000,0x5800,0x100,0x0100|0x0800,0x2000}))|uint16(next()&0x00FE&~0x0040);
                    At<void*>(m,0x14)=col; At<float>(m,0x18)=100.f; At<void*>(m,0x1C)=nullptr; };
                auto makeVehicle=[&](int16 model){
                    std::memset(veh.data(),0xCD,veh.size()); uint8* v=veh.data(); At<uintptr_t>(v,0)=0; const CVector pos=vec(60.f);
                    At<CVector>(v,4)=pos; fillMat(Mat(3),pos,coin(4)); At<void*>(v,0x14)=Mat(3); At<void*>(v,0x18)=nullptr; At<uint32>(v,0x1C)=next();
                    At<int16>(v,0x22)=model; v[0x36]=uint8((next()&0xF8)|2); At<uint32>(v,0x40)=next(); At<CVector>(v,0x44)=vec(0.2f); At<CVector>(v,0x50)=vec(0.05f);
                    At<float>(v,0x8C)=uni(500.f,3000.f); At<void*>(v,0xFC)=nullptr; At<void*>(v,0x128)=nullptr; At<uint16>(v,0x86C)=uint16(coin(2) ? 0 : next()%3000); };
                auto makePed=[&](){
                    std::memset(ped.data(),0xCD,ped.size()); uint8* pd=ped.data(); At<uintptr_t>(pd,0)=0; const CVector pos=vec(60.f);
                    At<CVector>(pd,4)=pos; fillMat(Mat(4),pos,false); At<void*>(pd,0x14)=Mat(4); At<uint32>(pd,0x1C)=next(); At<int16>(pd,0x22)=int16(next()%300);
                    pd[0x36]=uint8((next()&0xF8)|3); At<uint32>(pd,0x40)=next(); At<float>(pd,0x8C)=uni(50.f,120.f); At<void*>(pd,0xFC)=nullptr; At<void*>(pd,0x128)=nullptr;
                    At<uint32>(pd,0x46C)=next(); At<void*>(pd,0x47C)=intel.data(); At<void*>(pd,0x58C)=coin(2) ? veh.data() : nullptr; };
                auto makeDummy=[&](int16 model,bool withMatrix){
                    std::memset(dummy.data(),0xCD,dummy.size()); uint8* d=dummy.data(); At<uintptr_t>(d,0)=0; const CVector pos=vec(60.f);
                    At<CVector>(d,4)=pos; At<float>(d,0x10)=uni(-3.f,3.f); fillMat(Mat(5),pos,coin(3)); At<void*>(d,0x14)=withMatrix ? Mat(5) : nullptr;
                    At<void*>(d,0x18)=nullptr; At<uint32>(d,0x1C)=next(); At<uint16>(d,0x20)=uint16(next()); At<int16>(d,0x22)=model; At<void*>(d,0x24)=nullptr;
                    At<void*>(d,0x28)=nullptr; d[0x2E]=uint8(next()); d[0x2F]=uint8(next()); At<void*>(d,0x30)=nullptr; d[0x36]=uint8((next()&0xF8)|5); };
                auto makeRw=[&](uint8 type){
                    std::memset(rwObj.data(),0,rwObj.size()); rwObj[0]=type; At<void*>(rwObj.data(),4)=frame.data();
                    for(size_t off=0;off<frame.size();off+=4)At<float>(frame.data(),off)=uni(-1.f,1.f); return static_cast<void*>(rwObj.data()); };
                // pool falso: cabecera (almacenamiento, estados, capacidad, último libre, banderas)
                auto setupPool=[&](int32 lastFree){
                    At<void*>(objHdr.data(),0)=objStore.data(); At<void*>(objHdr.data(),4)=objStates.data(); At<int32>(objHdr.data(),8)=OBJ_CAP;
                    At<int32>(objHdr.data(),0xC)=lastFree; objHdr[0x10]=0; objHdr[0x11]=0; objHdr[0x12]=0; objHdr[0x13]=0; objPoolPtr=objHdr.data(); };
                // lista de matrices falsa: centinelas de gMatrixList apuntando a enlaces propios (libres)
                auto setupMatList=[&]{
                    auto* ml=reinterpret_cast<uint8*>(ADDR_MATLIST); auto L=[&](int k){ return ml+k*LINK; };
                    for(auto& l:links){ std::memset(l.data(),0xCD,LINK); At<void*>(l.data(),0x40)=nullptr; At<uint32>(l.data(),0x44)=0; At<void*>(l.data(),0x48)=nullptr; }
                    At<void*>(L(0),0x50)=L(1); At<void*>(L(1),0x4C)=L(0); At<void*>(L(2),0x50)=L(3); At<void*>(L(3),0x4C)=L(2);
                    At<void*>(L(4),0x50)=links[0].data(); At<void*>(links[0].data(),0x4C)=L(4);
                    for(int k=0;k<NLINK;++k){ At<void*>(links[k].data(),0x50)=k+1<NLINK ? static_cast<void*>(links[k+1].data()) : static_cast<void*>(L(5)); if(k)At<void*>(links[k].data(),0x4C)=links[k-1].data(); }
                    At<void*>(L(5),0x4C)=links[NLINK-1].data(); };
                // nodos reales del pool de nodos dobles (los estados se restauran después)
                auto newNode=[&](void* item)->uint8*{ auto* n=reinterpret_cast<uint8*>(GetPtrNodeDoubleLinkPool()->New()); if(!n)return nullptr; At<void*>(n,0)=item; At<void*>(n,4)=nullptr; At<void*>(n,8)=nullptr; return n; };
                auto listJson=[&]{ json l=json::array(); int n=0; for(auto* nd=*reinterpret_cast<uint8**>(ADDR_CCL_HEAD);nd&&n<16;nd=At<uint8*>(nd,4),++n)l.push_back({Name(nd),Name(At<void*>(nd,0)),Name(At<void*>(nd,4)),Name(At<void*>(nd,8))}); return l; };
                auto nodePoolJson=[&]{ json s=json::array(); for(int32 k=0;k<nodeCap;++k)if(nodeStates[k]!=savedNodeStates[size_t(k)])s.push_back({k,nodeStates[k]}); return json{At<int32>(nodePool,0xC),s}; };

                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4848u; if(!st)st=1; trace=json::array(); rngState=uint32(i)*7919u+uint32(f);
                    restoreAll();
                    // reloj y globales
                    At<float>(reinterpret_cast<void*>(ADDR_TIMESTEP),0)=uni(0.2f,3.f); At<uint32>(reinterpret_cast<void*>(ADDR_TIME_MS),0)=40000+next()%200000;
                    At<uint32>(reinterpret_cast<void*>(ADDR_PREV_MS),0)=At<uint32>(reinterpret_cast<void*>(ADDR_TIME_MS),0)-next()%60; At<uint32>(reinterpret_cast<void*>(ADDR_FRAMES),0)=next();
                    At<float>(reinterpret_cast<void*>(ADDR_DN),0)=uni(0.f,1.f); At<uint8>(reinterpret_cast<void*>(ADDR_SAM_CARRIER),0)=uint8(coin(3)); At<uint8>(reinterpret_cast<void*>(ADDR_SAM_AREA51),0)=uint8(coin(3));
                    At<void*>(reinterpret_cast<void*>(ADDR_SMOKE),0)=smoke.data();
                    // dependencias
                    playerPedRet=coin(6) ? 0 : uintptr_t(ped.data()); playerVehRet=coin(2) ? 0 : uintptr_t(veh.data()); jetTaskRet=coin(2) ? uintptr_t(jetTask.data()) : 0;
                    trainRet=coin(3) ? 0 : uintptr_t(train.data()); fxRet=coin(3) ? 0 : uintptr_t(fxSys.data());
                    appearance=pickI({0,1,2,3,4,5}); ropeIndex=int32(next()%8); garageFound=pickI({-1,0,1,3}); clumpAlpha=pickI({0,8,16,17,200,255});
                    canDamage=!coin(4); carriedByRope=coin(4); isDoor=coin(4); buoyRet=coin(3); vlineRet=!coin(3); lightRet=coin(2);
                    pickupHeight=uni(-3.f,3.f); vlineLighting=uint8(next()); buoyTurn=vec(1.f); buoyForce=vec(1.f);
                    // entidades base
                    makeInfo(infoA.data()); makeInfo(infoB.data()); makeCol(colA.data(),pickF({0.6f,1.2f,3.f})); makeCol(colB.data(),1.5f);
                    for(size_t off=0;off<colData.size();off+=4)At<uint32>(colData.data(),off)=next()%40;
                    const uintptr_t miVt=pickI({int32(VT_ATOMIC_MI),int32(VT_ATOMIC_MI),int32(VT_CLUMP_MI),int32(VT_WEAPON_MI)});
                    makeMi(miA.data(),miVt,colA.data()); makeMi(miB.data(),VT_ATOMIC_MI,colB.data()); makeMi(miR.data(),VT_VEHICLE_MI,colB.data());
                    for(auto& b:{&fxBp,&fxSys,&smoke,&intel,&jetTask,&train,&texA,&texB,&fire,&collNode,&fxVecs})std::memset(b->data(),0xCD,b->size());
                    { const CVector tp=vec(60.f); At<CVector>(train.data(),4)=tp; At<void*>(train.data(),0x14)=nullptr; }
                    setupPool(-1);
                    for(int k=0;k<OBJ_CAP;++k){ std::memset(Obj(k),0xCD,OBJ_STRIDE); objStates[k]=0x80; }
                    makeObject(0,MI_A,infoA.data(),true); makeObject(1,MI_B,infoB.data(),true); makeObject(2,MI_B,infoB.data(),coin(4)!=0);
                    makeVehicle(int16(pickI({406,530,486,601,400,560}))); makePed(); makeDummy(MI_A,true);
                    setMi(MI_A,miA.data()); setMi(MI_B,miB.data()); setMi(MI_R,miR.data());
                    uint8* const A=Obj(0); uint8* const B=Obj(1); uint8* const C=Obj(2);
                    auto setModelA=[&](int16 m){ At<int16>(A,0x22)=m; setMi(m,miA.data()); };
                    std::vector<int16> special{MI_BUOY,MI_SAMSITE,MI_SAMSITE2,MI_TRAINCROSSING,MI_TRAINCROSSING1,MI_MAGNOCRANE,MI_CRANETROLLEY,MI_QUARRYCRANE_ARM,MI_RCBOMB,
                        MI_GRASSPLANT,MI_GRASSHOUSE,MI_FIRE_HYDRANT,MI_SHASH_WALL,MI_MLAMPPOST,MI_TRAFFIC1,MI_STREET1,MI_DOUBLESTREET,MI_CRANE_MAGNET,MI_WRECKING_BALL};
                    auto pickModel=[&](std::initializer_list<int16> l){ const int16 m=*(l.begin()+next()%l.size()); return m ? m : MI_A; };
                    json out; json ret=nullptr; const uintptr_t a=fn.address;

                    // --- fixtures por función ---
                    if(nm=="Init"){
                        int16 m=coin(2) ? MI_A : special[next()%special.size()]; if(!m)m=MI_A; if(coin(10))m=int16(pickI({342,374,381}));
                        setModelA(m); At<uint32>(A,0x40)&=~0x20u|(coin(2) ? 0x20u : 0u);
                    }
                    if(nm=="Create-intbool"||nm=="Create-dummy"||nm=="TryToFreeUpTempObjects"||nm.starts_with("DeleteAll")||nm.starts_with("IsObjectPointerValid")){
                        // pool con huecos ocupados (objetos temporales, de misión y otros) y libres rellenos con 0xCD
                        int32 freeCount=0;
                        for(int k=0;k<OBJ_CAP;++k){
                            if(k>=3||coin(2)){ if(coin(3)){ std::memset(Obj(k),0xCD,OBJ_STRIDE); objStates[k]=uint8(0x80|(next()&0x7F)); ++freeCount; continue; }
                                makeObject(k,coin(2) ? MI_A : MI_B,coin(2) ? infoA.data() : infoB.data(),coin(4)!=0); }
                            Obj(k)[0x13C]=uint8(pickI({1,2,3,3,4,5,6,0})); }
                        if((nm.starts_with("Create"))&&freeCount==0){ Obj(OBJ_CAP-1)[0x13C]=3; At<uint32>(Obj(OBJ_CAP-1),0x1C)&=~0x80u; }
                        setupPool(int32(pickI({-1,0,2,5,7})));
                    }
                    for(int k=0;k<OBJ_CAP;++k)if(objStates[k]&0x80)std::memset(Obj(k),0xCD,OBJ_STRIDE);

                    // conmutación de hooks y comprobación después de State
                    for(auto* h:hooks)h->State(candidate);
                    bool switches;
                    if(fn.vt){ const uintptr_t t=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]; switches=(t<0x400000||t>=0x1000000)==candidate; }
                    else switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    out["switches"]=switches;
                    auto vfn=[&]{ return reinterpret_cast<uintptr_t*>(OBJ_VT)[fn.slot]; };
                    // AddToControlCodeList se simula dentro de Init y Create (parches después de State, deshechos antes del siguiente)
                    Patches local;
                    if(nm=="Init"||nm.starts_with("Create")){ local.Add(0x59F400,FakeAddToCcl); local.Add(cclDll,FakeAddToCcl); local.Add(cclBody,FakeAddToCcl); }

                    if(nm=="CanBeDeleted"){ A[0x13C]=uint8(next()%9); ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(A); }
                    else if(nm=="CanBeTargetted"){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(A); }
                    else if(nm=="SetObjectTargettable"){ const uint32 v=uint32(pickI({0,1,0x100,0x101,0xFF00,0x7F01})); reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(A,v); }
                    else if(nm=="TryToExplode"){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(A); }
                    else if(nm=="RefModelInfo"){ const int32 m=coin(2) ? MI_A : MI_B; reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(A,m); }
                    else if(nm=="SetRemapTexture"){ void* tex=coin(3) ? nullptr : texB.data(); const int32 txd=pickI({-1,-1,0,5,300,0xFFFF,-2,0x10005});
                        reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(a)(A,tex,txd); }
                    else if(nm=="SetRelatedDummy"){ reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(A,coin(2) ? dummy.data() : static_cast<void*>(B)); }
                    else if(nm=="GetRopeHeight"||nm=="SetRopeHeight"||nm=="GetObjectCarriedWithRope"||nm=="ReleaseObjectCarriedWithRope"||nm=="GrabObjectToCarryWithRope"){
                        uint8* rope=reinterpret_cast<uint8*>(ADDR_ROPES)+ropeIndex*ROPE_SIZE;
                        At<float>(rope,0x31C)=uni(0.f,2.f); At<void*>(rope,0x314)=coin(5) ? static_cast<void*>(veh.data()) : static_cast<void*>(C); At<void*>(rope,0x318)=coin(3) ? nullptr : static_cast<void*>(B);
                        if(nm=="GetRopeHeight")ret=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(A));
                        else if(nm=="SetRopeHeight")reinterpret_cast<void(__thiscall*)(void*,float)>(a)(A,uni(-1.f,3.f));
                        else if(nm=="GetObjectCarriedWithRope")ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(a)(A));
                        else if(nm=="ReleaseObjectCarriedWithRope")reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                        else reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(A,coin(2) ? static_cast<void*>(veh.data()) : static_cast<void*>(B));
                        out["rope"]=Region(rope,ROPE_SIZE); out["C"]=Region(C,OBJ_SIZE); out["matC"]=Floats(Mat(2),18);
                    }
                    else if(nm=="AddToControlCodeList"||nm=="RemoveFromControlCodeList"){
                        // lista de control falsa con nodos reales; los objetos son del pool falso
                        uint8* head=nullptr; std::vector<uint8*> nodes; const int n=int(next()%4)+(nm=="RemoveFromControlCodeList" ? 1 : 0);
                        const int mine=nm=="RemoveFromControlCodeList"&&!coin(5) ? int(next()%uint32(n)) : -1;
                        for(int k=0;k<n;++k){ void* item=k==mine ? static_cast<void*>(A) : static_cast<void*>(Obj(1+k%2)); uint8* nd=newNode(item); if(!nd)break; nodes.push_back(nd); }
                        for(size_t k=0;k<nodes.size();++k){ At<void*>(nodes[k],4)=k+1<nodes.size() ? nodes[k+1] : nullptr; At<void*>(nodes[k],8)=k ? nodes[k-1] : nullptr; }
                        head=nodes.empty() ? nullptr : nodes[0]; *reinterpret_cast<uint8**>(ADDR_CCL_HEAD)=head;
                        At<void*>(A,0x138)=mine>=0&&size_t(mine)<nodes.size() ? static_cast<void*>(nodes[size_t(mine)]) : nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                        out["list"]=listJson(); out["nodePool"]=nodePoolJson();
                    }
                    else if(nm=="ResetDoorAngle"||nm=="LockDoor"||nm=="SetIsStatic"){
                        if(coin(3))At<void*>(A,0x14)=nullptr; if(coin(2))At<void*>(A,0x18)=makeRw(uint8(pickI({1,2})));
                        if(!coin(4))At<uint32>(A,0x40)|=0x20u; At<float>(A,0x158)=pickF({-1001.f,-1000.f,-999.99f,uni(-3.2f,3.2f),uni(-3.2f,3.2f),std::bit_cast<float>(QNAN)});
                        if(nm=="SetIsStatic")reinterpret_cast<void(__thiscall*)(void*,uint32)>(vfn())(A,uint32(pickI({0,1,0x100,0x201})));
                        else reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                        out["frame"]=Floats(frame.data(),0x40);
                    }
                    else if(nm=="Init"){
                        if(coin(3))At<void*>(A,0x14)=nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                        out["miA"]=Region(miA.data(),0x20); out["colData"]=Region(colData.data(),0x30);
                    }
                    else if(nm=="DoBurnEffect"){
                        makeCol(colA.data(),pickF({0.5f,1.5f,2.5f})); At<float>(A,0x178)=uni(-0.2f,2.f);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                    }
                    else if(nm=="GetLightingFromCollisionBelow"){
                        if(coin(3))At<void*>(A,0x14)=nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                    }
                    else if(nm=="PreRender"){
                        At<uint32>(A,0x140)=(At<uint32>(A,0x140)&~0x00C20000u)|(coin(2) ? 0x800000u : 0u)|(coin(2) ? 0x400000u : 0u)|(coin(3) ? 0x20000u : 0u);
                        At<uint32>(A,0x174)=pickI({0,0,1,int32(At<uint32>(reinterpret_cast<void*>(ADDR_TIME_MS),0)),int32(At<uint32>(reinterpret_cast<void*>(ADDR_TIME_MS),0)+500)});
                        makeCol(colA.data(),pickF({0.5f,1.5f})); At<float>(A,0x178)=uni(0.f,1.f);
                        if(coin(3))At<void*>(A,0xFC)=veh.data();
                        if(!coin(4))At<void*>(A,0x18)=makeRw(uint8(pickI({1,2})));
                        if(!At<void*>(A,0x18)){ At<float>(A,0x15C)=1.f; At<uint32>(A,0x140)&=~0x20000u; } else At<float>(A,0x15C)=pickF({1.f,1.f,0.5f,2.f,1.0000001f});
                        reinterpret_cast<void(__thiscall*)(void*)>(vfn())(A);
                        out["frame"]=Floats(frame.data(),0x40);
                    }
                    else if(nm=="Render"){
                        At<uint32>(A,0x140)^=next()&0x02000080u; if(coin(2))A[0x13C]=3; At<int16>(A,0x14A)=coin(3) ? int16(-1) : MI_R;
                        At<void*>(reinterpret_cast<void*>(ADDR_REMAP),0)=texB.data();
                        reinterpret_cast<void(__thiscall*)(void*)>(vfn())(A);
                        out["remap"]=Name(*reinterpret_cast<uintptr_t*>(ADDR_REMAP));
                    }
                    else if(nm=="SetupLighting"){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(vfn())(A); }
                    else if(nm=="RemoveLighting"){ reinterpret_cast<void(__thiscall*)(void*,uint32)>(vfn())(A,uint32(pickI({0,1,0x100,0x301}))); }
                    else if(nm=="CreateRwObject"){ reinterpret_cast<void(__thiscall*)(void*)>(vfn())(A); }
                    else if(nm=="Teleport"){
                        if(coin(3))At<void*>(A,0x14)=nullptr; if(coin(2))At<void*>(A,0x18)=makeRw(1); const CVector d=vec(500.f);
                        reinterpret_cast<void(__thiscall*)(void*,float,float,float,uint32)>(vfn())(A,d.x,d.y,d.z,uint32(next()%2));
                        out["frame"]=Floats(frame.data(),0x40);
                    }
                    else if(nm=="SpecialEntityPreCollisionStuff"){
                        uint8* col=pickI({0,1,2})==0 ? veh.data() : coin(2) ? ped.data() : B;
                        if(coin(3))setModelA(int16(pickI({342,374,377,381,MI_A})));
                        At<uint32>(A,0x140)^=next()&0x140u; At<uint32>(A,0x40)&=next()|~0xE6u;
                        At<float>(Mat(0),0x28)=pickF({0.66f,0.5f,0.9f,0.6600001f,uni(-1.f,1.f)});
                        makeCol(colA.data(),pickF({0.5f,0.9f,1.2f}));
                        if(coin(4))At<void*>(A,0xFC)=col; if(coin(6))At<void*>(col,0xFC)=A; if(coin(6)){ At<void*>(A,0xFC)=veh.data(); At<void*>(col,0xFC)=veh.data(); }
                        if(coin(8))At<void*>(A,0x128)=col; if(coin(8))At<void*>(col,0x128)=A;
                        At<CVector>(Mat(0),0x30).z=entPos(col).z+uni(-2.f,2.f);
                        bool flags[4]{coin(5),coin(5),coin(5),coin(5)}; const uint32 ignore=uint32(coin(3));
                        reinterpret_cast<void(__thiscall*)(void*,void*,uint32,bool*,bool*,bool*,bool*)>(vfn())(A,col,ignore,&flags[0],&flags[1],&flags[2],&flags[3]);
                        out["flags"]={flags[0],flags[1],flags[2],flags[3]}; out["colEnt"]=Name(col);
                    }
                    else if(nm=="SpecialEntityCalcCollisionSteps"){
                        At<uint32>(A,0x40)=(At<uint32>(A,0x40)&~0xE0u)|uint32(pickI({0,0,0x80,0x20,0x40,0x60,0xE0}));
                        At<uint32>(A,0x140)^=next()&0x2000u; infoA[0x1D]=uint8(pickI({0,1,2,4,5,3,6}));
                        const float sp=pickF({0.01f,0.1f,0.5f,2.f}); At<CVector>(A,0x44)=vec(sp); At<CVector>(A,0x50)=vec(sp);
                        bool b1=coin(2), b2=coin(2);
                        ret=reinterpret_cast<uint8(__thiscall*)(void*,bool*,bool*)>(vfn())(A,&b1,&b2); out["bools"]={b1,b2};
                    }
                    else if(nm=="ProcessSamSiteBehaviour"){
                        setModelA(pickModel({MI_SAMSITE,MI_SAMSITE2})); At<uint32>(A,0x140)&=coin(5) ? ~0u : ~0x400u;
                        At<CVector>(Mat(0),0x30).x=pickF({-1000.f,-999.f,-1001.f,uni(-2000.f,2000.f)});
                        uint8* tgt=playerVehRet ? veh.data() : ped.data(); const CVector me=At<CVector>(Mat(0),0x30);
                        At<CVector>(Mat(tgt==veh.data() ? 3 : 4),0x30)=me+CVector{uni(-150.f,150.f),uni(-150.f,150.f),uni(-4.f,6.f)};
                        if(coin(4))At<CVector>(Mat(tgt==veh.data() ? 3 : 4),0x30).y=pickF({2100.f,2100.5f,2099.f});
                        if(coin(3))At<uint32>(reinterpret_cast<void*>(ADDR_PREV_MS),0)=(At<uint32>(reinterpret_cast<void*>(ADDR_TIME_MS),0)/4000)*4000-1;
                        if(!playerPedRet)playerPedRet=uintptr_t(ped.data());
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                    }
                    else if(nm=="ProcessTrainCrossingBehaviour"){
                        setModelA(pickModel({MI_TRAINCROSSING,MI_TRAINCROSSING1,MI_TRAINCROSSING}));
                        { const float uz=pickF({1.f,0.f,uni(-1.f,1.f),uni(0.f,1.f),0.2177f}); At<float>(Mat(0),0x28)=uz; }
                        At<CVector>(train.data(),4)=entPos(A)+CVector{uni(-150.f,150.f),uni(-150.f,150.f),0.f};
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                    }
                    else if(nm=="SetMatrixForTrainCrossing"){
                        const float ang=pickF({0.f,-0.f,1.3508849f,1.5707964f,uni(-4.f,4.f),uni(0.f,1.4f)});
                        reinterpret_cast<void(__cdecl*)(void*,float)>(a)(Mat(0),ang);
                    }
                    else if(nm=="ProcessGarageDoorBehaviour"||nm=="ProcessControlLogic"){
                        if(!playerPedRet)playerPedRet=uintptr_t(ped.data()); // SamSite desreferencia el peatón del jugador
                        if(nm=="ProcessControlLogic")setModelA(pickModel({MI_A,MI_SAMSITE,MI_TRAINCROSSING,MI_TRAINCROSSING1,MI_MAGNOCRANE,MI_CRANETROLLEY,MI_QUARRYCRANE_ARM,MI_MAGNOCRANE}));
                        if(coin(4))ropeIndex=-1;
                        for(int g=0;g<4;++g){ uint8* gr=reinterpret_cast<uint8*>(ADDR_GARAGES)+g*GARAGE_SIZE; At<float>(gr,0x38)=uni(0.f,1.f); gr[0x4C]=uint8(pickI({44,45,1,14,2})); gr[0x4E]=uint8(next()); }
                        A[0x147]=uint8(pickI({-1,-1,0,1,2,3})); garageFound=pickI({-1,0,1,2,3});
                        if(ropeIndex>=0){ uint8* rope=reinterpret_cast<uint8*>(ADDR_ROPES)+ropeIndex*ROPE_SIZE; At<float>(rope,0x31C)=uni(0.f,2.f); At<float>(rope,0x308)=uni(0.f,30.f); At<float>(rope,0x30C)=uni(0.f,2.f); }
                        if(coin(2))At<void*>(A,0x18)=makeRw(1);
                        At<CVector>(train.data(),4)=entPos(A)+CVector{uni(-150.f,150.f),uni(-150.f,150.f),0.f};
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                        out["garages"]=Fnv(reinterpret_cast<void*>(ADDR_GARAGES),4*GARAGE_SIZE); out["frame"]=Floats(frame.data(),0x40);
                    }
                    else if(nm=="ObjectDamage"||nm=="Explode"||nm=="ObjectFireDamage"){
                        At<uint32>(A,0x1C)|=coin(6) ? 0u : 1u;
                        if(nm=="ObjectFireDamage")setModelA(pickModel({MI_GRASSPLANT,MI_GRASSHOUSE,MI_GRASSPLANT,MI_A}));
                        else if(coin(6))setModelA(pickModel({MI_SHASH_WALL}));
                        if(!coin(3))At<uint32>(A,0x40)&=~0x400000u;
                        At<uint8>(ped.data(),0x46D)=uint8(next()); At<int16>(veh.data(),0x22)=int16(pickI({601,530,400,601}));
                        At<float>(A,0x154)=pickF({1000.f,10.f,0.f,uni(0.f,300.f)});
                        void* damager=reinterpret_cast<void*>(uintptr_t(pickI({0,int32(uintptr_t(veh.data())),int32(uintptr_t(ped.data())),int32(uintptr_t(B)),int32(uintptr_t(A))})));
                        if(coin(4))playerPedRet=uintptr_t(damager);
                        if(nm=="ObjectDamage"){
                            At<CVector>(fxVecs.data(),0)=vec(50.f); At<CVector>(fxVecs.data(),0x10)=vec(1.f);
                            const CVector* org=coin(4) ? nullptr : reinterpret_cast<CVector*>(fxVecs.data()); const CVector* dirv=coin(4) ? nullptr : reinterpret_cast<CVector*>(fxVecs.data()+0x10);
                            const float dmg=pickF({uni(0.f,40.f),uni(0.f,400.f),150.f,31.f,30.f,10000.f});
                            reinterpret_cast<void(__thiscall*)(void*,float,const CVector*,const CVector*,void*,int32)>(a)(A,dmg,org,dirv,damager,pickI({55,55,49,51,31,0,22}));
                        } else if(nm=="Explode") reinterpret_cast<void(__thiscall*)(void*)>(a)(A);
                        else reinterpret_cast<void(__thiscall*)(void*,float,void*)>(a)(A,pickF({uni(0.f,500.f),2000.f,1000.f}),damager);
                    }
                    else if(nm=="CanBeUsedToTakeCoverBehind"){
                        if(coin(4))setModelA(pickModel({MI_FIRE_HYDRANT}));
                        { const float h=pickF({0.75f,1.25f,1.f,0.7500001f,1.2499999f,uni(0.f,2.f)}); At<float>(colA.data(),8)=-h/2.f; At<float>(colA.data(),0x14)=h/2.f; if(coin(10))At<uint32>(colA.data(),0x14)=QNAN; }
                        At<float>(Mat(0),0x28)=pickF({0.9f,0.9000001f,uni(0.f,1.f),1.f});
                        ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(A);
                    }
                    else if(nm=="ProcessControl"){
                        setModelA(pickModel({MI_A,MI_A,MI_BUOY,MI_RCBOMB,MI_CRANE_MAGNET,MI_WRECKING_BALL}));
                        At<uint16>(miA.data(),0x12)^=uint16(next()&0x0100); if(coin(3))At<void*>(A,0x18)=makeRw(2);
                        At<uint32>(A,0x140)^=next()&0x00080040u; At<uint32>(A,0x40)&=next()|0xFDFFFF1Fu|(coin(2) ? 0u : 0x020000E0u);
                        At<uint32>(A,0x1C)&=coin(2) ? ~0x40014u : ~0u;
                        if(coin(2))At<void*>(A,0xDC)=veh.data(); if(coin(3))At<void*>(A,0xFC)=nullptr;
                        At<float>(A,0xE8)=pickF({0.31f,0.3f,uni(-1.f,1.f)});
                        if(coin(2)){ At<CVector>(A,0x44)=vec(0.0005f); At<CVector>(A,0x50)=vec(0.0005f); At<CVector>(A,0x74)=vec(0.0005f); At<CVector>(A,0x80)=vec(0.0005f); A[0xB8]=uint8(pickI({8,9,10,11})); }
                        if(coin(2)){ At<uint32>(A,0x40)|=0x20u; At<float>(A,0x158)=uni(-3.2f,3.2f); At<uint32>(A,0x140)^=next()&0xC000u; At<float>(A,0x58)=pickF({0.f,0.019f,-0.021f,uni(-0.05f,0.05f)}); }
                        reinterpret_cast<void(__thiscall*)(void*)>(vfn())(A);
                        out["frame"]=Floats(frame.data(),0x40);
                    }
                    else if(nm=="TryToFreeUpTempObjects"){ reinterpret_cast<void(__cdecl*)(int32)>(a)(pickI({-1,0,1,2,3,5,10})); }
                    else if(nm=="DeleteAllTempObjects"||nm=="DeleteAllMissionObjects"){ reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="DeleteAllTempObjectsInArea"){
                        // el centro se toma de un hueco ocupado (los libres son 0xCD: su matriz no es válida)
                        int ck=int(next()%OBJ_CAP); for(int t=0;t<OBJ_CAP&&(objStates[ck]&0x80);++t)ck=(ck+1)%OBJ_CAP;
                        const CVector c=((objStates[ck]&0x80) ? CVector{0.f,0.f,0.f} : entPos(Obj(ck)))+vec(30.f); const float r=pickF({0.f,10.f,50.f,uni(0.f,120.f),-20.f});
                        reinterpret_cast<void(__cdecl*)(float,float,float,float)>(a)(c.x,c.y,c.z,r);
                    }
                    else if(nm=="Create-intbool"||nm=="Create-dummy"){
                        setupMatList(); At<uint16>(miA.data(),0x12)=0; miA[0xD]=0;
                        void* r;
                        if(nm=="Create-intbool") r=reinterpret_cast<void*(__cdecl*)(int32,uint32)>(a)(MI_A,uint32(next()%2));
                        else { makeDummy(MI_A,!coin(3)); At<uint32>(dummy.data(),0x1C)^=next()&0x200u; r=reinterpret_cast<void*(__cdecl*)(void*)>(a)(dummy.data()); }
                        ret=Name(r);
                        // la vptr se compara por tipo (EXE 0x866F60 o la vtable de CObject de la DLL)
                        json pool=json::array();
                        for(int k=0;k<OBJ_CAP;++k){ json s=Region(Obj(k),OBJ_SIZE); const uintptr_t vp=At<uintptr_t>(Obj(k),0); if(vp==OBJ_VT||InDll(vp))s[0]="CObject_vt"; pool.push_back(std::move(s)); }
                        out["pool"]=pool; out["links"]=Region(links.data(),sizeof(links)); out["matlist"]=Region(reinterpret_cast<void*>(ADDR_MATLIST),MATLIST_BYTES); out["dummy"]=Region(dummy.data(),0x38);
                    }
                    else if(nm=="IsObjectPointerValid"||nm=="IsObjectPointerValid_NotInWorld"){
                        const int k=int(next()%OBJ_CAP); const intptr_t off=pickI({0,0,0,4,0x100,0x19B,-4,-0x19B,-0x19C,-0x400});
                        const uintptr_t ptr=coin(12) ? 0 : uintptr_t(intptr_t(Obj(k))+off+(coin(10) ? intptr_t(OBJ_STRIDE)*OBJ_CAP : 0));
                        // el objeto validado se lee (+0x1C, +0xB0): solo punteros dentro del almacenamiento falso
                        const bool readable=ptr>=uintptr_t(objStore.data())&&ptr+0xB4<=uintptr_t(objStore.data()+objStore.size());
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(reinterpret_cast<void*>(readable||nm=="IsObjectPointerValid_NotInWorld" ? ptr : uintptr_t(Obj(k))));
                    }
                    else if(nm=="Save"){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(A); }
                    else if(nm=="Load"){
                        loadSize=0x30; for(auto& b:loadBuf)b=uint8(next());
                        for(int k=0;k<3;++k)At<float>(loadBuf.data(),4*k)=uni(-500.f,500.f);
                        ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(A);
                    }
                    {
                        // Create construye en el hueco A o B: la vptr se compara por tipo (EXE 0x866F60 o la vtable de CObject de la DLL)
                        const bool isCreate=nm.starts_with("Create-");
                        out["A"]=Region(A,OBJ_SIZE);
                        std::vector<uint8> bCopy(B,B+OBJ_SIZE);
                        if(isCreate){ if(At<uintptr_t>(A,0)==OBJ_VT||InDll(At<uintptr_t>(A,0)))out["A"][0]="CObject_vt"; if(At<uintptr_t>(B,0)==OBJ_VT||InDll(At<uintptr_t>(B,0)))At<uintptr_t>(bCopy.data(),0)=OBJ_VT; }
                        out["B"]=Fnv(bCopy.data(),OBJ_SIZE);
                    }
                    out["matA"]=Floats(Mat(0),18);
                    out["states"]=ReversingProbe::Bytes(objStates.data(),OBJ_CAP); out["hdr"]=Region(objHdr.data(),0x14);
                    out["veh"]=Fnv(veh.data(),veh.size()); out["ped"]=Fnv(ped.data(),ped.size()); out["dummy"]=Fnv(dummy.data(),dummy.size());
                    out["mats"]=Fnv(mats.data(),sizeof(mats)); out["miA"]=Fnv(miA.data(),0x20);
                    out["ccl"]=Name(*reinterpret_cast<uintptr_t*>(ADDR_CCL_HEAD));
                    out["ret"]=ret; out["trace"]=trace;
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                restoreAll();
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
    restoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateRestored()}});
}
}
