#pragma once
// Banco dentro del motor: CBmx, CQuadBike, CCarFXRenderer y CStencilShadows (lista 61b). Original (hooks de las cuatro
// clases desactivados) frente a candidato con fixtures deterministas sobre objetos falsos: bicicleta (0x838 bytes) y quad
// (0x9BC bytes) con offsets crudos del EXE, vtables falsas con trampas trazadas (huecos 24 ProcessControlInputs, 52
// DoBurstAndSoftGroundRatios y 69 Remove), conductor/jugador falsos, asociaciones de animacion falsas, marcos RW con matriz
// de modelado, handling, modelo de vehiculo y modelo de colision falsos. Se simulan y trazan (direccion del EXE + simbolo o
// cuerpo de la DLL): fuerzas (ApplyMoveForce/ApplyTurnForce), bases de CAutomobile/CBike/CVehicle/CPhysical (ProcessControl,
// ProcessAI, ProcessControlInputs, PreRender, suspension), CTaskManager::FindActiveTaskByType, GetPadFromPlayer,
// FindPlayerPed, PruneReferences, DoDriveByShootings, GetColModel, GetWheelPosn, SetTransmissionRotation, GetSpeed,
// ProcessWheelRotation, AddSingleWheelParticles, CalculateLeanMatrix, StoreShadowForVehicle, animaciones
// (RpAnimBlendClumpGetAssociation, BlendAnimation, SetCurrentTime, SetFinishCallback, SetMoveState), CPlayerPed (sprint),
// CStats, CCarAI/CCarCtrl, RW (RpClumpForAllAtomics, texturas, rasters, Im3D, estados de render, DrawRect), CMemoryMgr,
// CEntity::RegisterReference/TransformFromObjectSpace y el modelo de colision. Los pads, el raton, el reloj, los trucos,
// el pool de vehiculos y los sectores del mundo se guardan y se restauran en cada ejecucion.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Bmx.h"
#include "Entity/Vehicle/QuadBike.h"
#include "Entity/Vehicle/Bike.h"
#include "Fx/CarFXRenderer.h"
#include "Shadows.h"
#include "Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h"
#include "StencilShadows.h"
#include "StencilShadowObject.h"
#include "../ProbeRuntime.hpp"

namespace Misc61bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; } };

// --- buferes con nombre para identificar punteros en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    return "other";
}
template<class V> inline json Jv(V v){
    if constexpr(std::is_floating_point_v<V>) return F(float(v));
    else if constexpr(std::is_pointer_v<V>) return Name(reinterpret_cast<uintptr_t>(v));
    else return v;
}
template<class... A> inline void T(const char* tag,A... a){ json e=json::array(); e.push_back(tag); (e.push_back(Jv(a)),...); trace.push_back(std::move(e)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- modulo de la DLL y busqueda de cuerpos ---
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
// cuerpo de la DLL de una funcion enganchada (para las llamadas calificadas a la base que no pasan por la vtable)
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }

// ======================= objetos falsos =======================
alignas(16) inline std::array<uint8,0x9BC> quad{};
alignas(16) inline std::array<uint8,0x838> bmx{};
alignas(16) inline std::array<uint8,0x60> vmat{};            // matriz del vehiculo (CMatrixLink)
alignas(16) inline std::array<uint8,0x100> hd{};              // tHandlingData
alignas(16) inline std::array<uint8,0x40> bh{};               // tBikeHandlingData
alignas(16) inline std::array<uint8,0x7A0> ped{}, ped2{};     // conductor / jugador
alignas(16) inline std::array<uint8,0x40> pdata{}, intel{}, task{}, clump{};
alignas(16) inline std::array<std::array<uint8,0xA8>,12> frames{};     // RwFrame (matriz de modelado en +0x10)
alignas(16) inline std::array<uint8,0x80> mi{};               // CVehicleModelInfo
alignas(16) inline std::array<std::array<uint8,0x40>,16> assocs{};      // asociaciones por id (0xC4..0xD3)
alignas(16) inline std::array<std::array<uint8,0x40>,4> fresh{};        // asociaciones nuevas de BlendAnimation
alignas(16) inline std::array<uint8,0x20> hier{};
alignas(16) inline std::array<uint8,0x40> colModelB{};       // modelo de colision de la bici
alignas(16) inline std::array<uint8,0x40> colDataB{};
alignas(16) inline std::array<uint8,0x80> linesB{};
alignas(16) inline std::array<void*,80> vtQuad{}, vtBmx{};

// --- trampas de vtable ---
inline float trapLean{}; inline bool trapLeanSet{};
inline void __fastcall V_Trap(void* self,void*){ T("virtual_inesperada",self); }
inline void __fastcall V_PCI(void* self,void*,uint32 n){ T("virtual_ProcessControlInputs",self,uint8(n)); if(trapLeanSet)At<float>(self,0x99C)=trapLean; }
inline void __fastcall V_Hover(void* self,void*){ T("virtual_DoHoverSuspensionRatios",self); }
inline void __fastcall V_Burst(void* self,void*){ T("virtual_DoBurstAndSoftGroundRatios",self); }

// --- fuerzas ---
inline void __fastcall S_TurnForce(void* self,void*,CVector f,CVector p){ T("turn_force",self,f.x,f.y,f.z,p.x,p.y,p.z); }
inline void __fastcall S_MoveForce(void* self,void*,CVector f){ T("move_force",self,f.x,f.y,f.z); }

// --- bases de vehiculo ---
inline bool autoAiResult{};
inline void __fastcall S_AutoProcessControl(void* self,void*){ T("CAutomobile_ProcessControl",self); }
inline void __fastcall S_AutoPCI(void* self,void*,uint32 n){ T("CAutomobile_ProcessControlInputs",self,uint8(n)); }
inline bool __fastcall S_AutoProcessAI(void* self,void*,uint32* flags){ T("CAutomobile_ProcessAI",self,flags,*flags); return autoAiResult; }
inline void __fastcall S_AutoPreRender(void* self,void*){ T("CAutomobile_PreRender",self); }
inline void __fastcall S_AutoSusp(void* self,void*){ T("CAutomobile_ProcessSuspension",self); }
inline void __fastcall S_AutoReset(void* self,void*){ T("CAutomobile_ResetSuspension",self); }
inline void __fastcall S_AutoSetup(void* self,void*){ T("CAutomobile_SetupSuspensionLines",self,At<float>(self,0x9A8)); }
inline void __fastcall S_BikeProcessControl(void* self,void*){ T("CBike_ProcessControl",self); }
inline void __fastcall S_VehPreRender(void* self,void*){ T("CVehicle_PreRender",self); }
inline void __fastcall S_PhysProcessControl(void* self,void*){ T("CPhysical_ProcessControl",self); }
inline void __fastcall S_BumperDamage(void* self,void*,int32 panel,uint32 b){ T("SetBumperDamage",self,panel,uint8(b)); }
inline void __fastcall S_DoorDamage(void* self,void*,int32 door,uint32 b){ T("SetDoorDamage",self,door,uint8(b)); }

// --- entidad / vehiculo ---
inline void* colModelResult{};
inline void* __fastcall S_ColModel(void* self,void*){ T("GetColModel",self); return colModelResult; }
inline void __fastcall S_Prune(void* self,void*){ T("PruneReferences",self); }
inline void __fastcall S_DoDriveBy(void* self,void*){ T("DoDriveByShootings",self); }
inline void __fastcall S_ActivateBomb(void* self,void*){ T("ActivateBomb",self); }
inline void __fastcall S_PlayHorn(void* self,void*){ T("PlayHornIfNecessary",self); }
inline void __fastcall S_ReduceHorn(void* self,void*){ T("ReduceHornCounter",self); }
inline void __fastcall S_SoftGround(void* self,void*,uint32* flags){ T("DoSoftGroundResistance",self,*flags); }
inline void __fastcall S_CalcLean(void* self,void*){ T("CalculateLeanMatrix",self); }
inline uint32 Hash32(uint32 a,uint32 b=0,uint32 c=0){ uint32 h=2166136261u; for(uint32 v:{a,b,c}){ h^=v; h*=16777619u; h^=h>>15; } return h; }
inline float __fastcall S_WheelRot(void* self,void*,int32 state,const CVector* a,const CVector* b,float r){
    T("ProcessWheelRotation",self,state,a->x,a->y,a->z,b->x,b->y,b->z,r);
    return float(int32(Hash32(F(a->x)^F(b->y),F(r),uint32(state))%2001)-1000)*1e-3f; }
inline CVector* __fastcall S_GetSpeed(void* self,void*,CVector* ret,CVector p){
    T("GetSpeed",self,p.x,p.y,p.z); *ret=CVector{p.x*0.5f+1.f,p.y*0.25f-2.f,p.z*0.75f+0.5f}; return ret; }
inline bool __fastcall S_Particles(void* self,void*,int32 state,uint32 a1,float a2,float a3,void* col,CVector* pos,float dir,int32 w,uint32 skid,bool* blood,uint32 flags){
    T("AddSingleWheelParticles",self,state,a1,a2,a3,col,pos->x,pos->y,pos->z,dir,w,skid,blood,uint8(*blood),flags); return false; }
inline void __fastcall S_TransRot(void* self,void*,void* frame,float a,float b,CVector p,uint32 front){ T("SetTransmissionRotation",self,frame,a,b,p.x,p.y,p.z,uint8(front)); }
inline void __fastcall S_WheelPosn(void* self,void*,int32 wheel,CVector* out,uint32 local){
    T("GetWheelPosn",self,wheel,uint8(local)); *out=CVector{0.5f*float(wheel+1),-1.25f+0.5f*float(wheel),0.3f+0.1f*float(wheel)}; }
inline void __cdecl S_Shadow(void* veh,int32 type){ T("StoreShadowForVehicle",veh,type); }
inline void __fastcall S_BunnyHop(void* self,void*){ T("ProcessBunnyHop",self); }

// --- animaciones ---
inline std::array<bool,16> assocOn{}; inline int freshUsed{};
inline void* __cdecl S_GetAssoc(void* clumpP,uint32 id){ const bool ok=id>=0xC4&&id<0xD4&&assocOn[id-0xC4]; T("RpAnimBlendClumpGetAssociation",clumpP,id,ok); return ok ? assocs[id-0xC4].data() : nullptr; }
inline void* __cdecl S_Blend(void* clumpP,int32 group,int32 id,float delta){
    T("BlendAnimation",clumpP,group,id,delta); const int k=std::min(freshUsed,3); ++freshUsed; return fresh[k].data(); }
inline void __fastcall S_SetTime(void* self,void*,float t){ T("SetCurrentTime",self,t); At<float>(self,0x20)=t; }
inline uintptr_t dllLaunchCb{};
inline json CbName(uintptr_t p){ if(p==0x6C0390||FollowIn(p)==dllLaunchCb)return "LaunchBunnyHopCB"; return Name(p); }
inline void __fastcall S_FinishCb(void* self,void*,void* cb,void* data){ trace.push_back({"SetFinishCallback",Name(uintptr_t(self)),CbName(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall S_SetMoveState(void* self,void*,int32 st){ T("SetMoveState",self,st); }
inline void __cdecl S_RiderAnims(void* rider,void* veh,void* rideData,void* handling,int32 a5){ T("ProcessRiderAnims",rider,veh,rideData,handling,int16(a5)); }

// --- jugador / controles ---
inline void* padResult{}; inline void* playerResult{};
inline void* __fastcall S_GetPad(void* self,void*){ T("GetPadFromPlayer",self); return padResult; }
inline void* __cdecl S_FindPlayer(int32 id){ T("FindPlayerPed",id); return playerResult; }
inline float sprintResult{}, buttonResult{}, fatResult{}; inline bool energyResult{};
inline float __fastcall S_Sprint(void* self,void*,uint32 t){ T("ControlButtonSprint",self,t); return sprintResult; }
inline float __fastcall S_ButtonSprint(void* self,void*,uint32 t){ T("GetButtonSprintResults",self,t); return buttonResult; }
inline bool __fastcall S_Energy(void* self,void*,uint32 sprint,float a){ T("HandleSprintEnergy",self,uint8(sprint),a); return energyResult; }
inline void __cdecl S_UpdateStats(uint32 b,void* bmxP){ T("UpdateStatsWhenCycling",uint8(b),bmxP); }
inline float __cdecl S_Fat(int32 m){ T("GetFatAndMuscleModifier",m); return fatResult; }

// --- IA / mundo ---
inline std::array<bool,4> taskOn{}; inline uint32 taskType[3]{};
inline void* __fastcall S_FindTask(void* self,void*,int32 type){ const bool ok=((type==0x329)&&taskOn[0])||((type==0x337)&&taskOn[1])||((type==0x339)&&taskOn[2]); T("FindActiveTaskByType",self,type,ok); return ok ? task.data() : nullptr; }
inline void __cdecl S_UpdateCarAI(void* v){ T("UpdateCarAI",v); }
inline void __cdecl S_OnRails(void* v){ T("UpdateCarOnRails",v); }
inline void __cdecl S_Steer(void* v){ T("SteerAICarWithPhysics",v); }
inline void __fastcall S_CalcGear(void* self,void*,float speed,uint8* gear){ T("CalculateGearForSimpleCar",self,speed,*gear); *gear=uint8(F(speed)>>7); }
inline void* __cdecl S_ForAllAtomics(void* clumpP,void* cb,void* data); // definida mas abajo (necesita la lista de atomics)

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,42> fns{{
    {"CQuadBike","Fix",0,0x871AE8,50,256},
    {"CQuadBike","GetRideAnimData",0,0x871AE8,47,256},
    {"CQuadBike","PreRender",0,0x871AE8,17,512},
    {"CQuadBike","ProcessAI",0,0x871AE8,66,1024},
    {"CQuadBike","ProcessControl",0,0x871AE8,10,1024},
    {"CQuadBike","ProcessControlInputs",0,0x871AE8,24,1024},
    {"CQuadBike","ProcessDrivingAnims",0,0x871AE8,46,256},
    {"CQuadBike","ProcessSuspension",0,0x871AE8,70,256},
    {"CQuadBike","ResetSuspension",0,0x871AE8,67,256},
    {"CQuadBike","SetupDamageAfterLoad",0,0x871AE8,51,256},
    {"CQuadBike","SetupSuspensionLines",0,0x871AE8,48,256},
    {"CBmx","BurstTyre",0,0x871528,44,256},
    {"CBmx","FindWheelWidth",0,0x871528,63,256},
    {"CBmx","LaunchBunnyHopCB",0x6C0390,0,-1,512},
    {"CBmx","PreRender",0,0x871528,17,1024},
    {"CBmx","ProcessAI",0,0x871528,66,2048},
    {"CBmx","ProcessBunnyHop",0x6C0590,0,-1,1024},
    {"CBmx","ProcessControl",0,0x871528,10,512},
    {"CBmx","ProcessDrivingAnims",0,0x871528,46,2048},
    {"CBmx","SetUpWheelColModel",0,0x871528,43,256},
    {"CCarFXRenderer","CustomCarPipeAtomicSetup",0x5D5B20,0,-1,256},
    {"CCarFXRenderer","CustomCarPipeClumpSetup",0x5D5B40,0,-1,256},
    {"CCarFXRenderer","Initialise",0x5D5AC0,0,-1,256},
    {"CCarFXRenderer","InitialiseDirtTexture",0x5D5BC0,0,-1,256},
    {"CCarFXRenderer","IsCCPCPipelineAttached",0x5D5B80,0,-1,256},
    {"CCarFXRenderer","PreRenderUpdate",0x5D5B10,0,-1,256},
    {"CCarFXRenderer","RegisterPlugins",0x5D5B00,0,-1,256},
    {"CCarFXRenderer","SetCustomFXAtomicRenderPipelinesVMICB",0x5D5B60,0,-1,256},
    {"CCarFXRenderer","SetFxEnvMapLightMult",0x5D5BA0,0,-1,256},
    {"CCarFXRenderer","Shutdown",0x5D5AD0,0,-1,256},
    {"CStencilShadows","GraphicsHighQuality",0x70F9B0,0,-1,256},
    {"CStencilShadows","Init",0x70F9E0,0,-1,256},
    {"CStencilShadows","Process",0x711D90,0,-1,512},
        {"CStencilShadows","Render",0x710D50,0,-1,512},
    {"CStencilShadows","RenderBuffer",0x710B50,0,-1,256},
    {"CStencilShadows","RenderForObject",0x710310,0,-1,1024},
    {"CStencilShadows","RenderForVehicle",0x70FAE0,0,-1,1024},
    {"CStencilShadows","RenderStencilShadows",0x7113B0,0,-1,256},
    {"CStencilShadows","Shutdown",0x711390,0,-1,256},
    {"CStencilShadows","SunSetPositionFromEntity",0x710AF0,0,-1,256},
    {"CStencilShadows","UpdateHierarchy",0x710BC0,0,-1,512},
    {"CStencilShadows","sub_710CC0",0x710CC0,0,-1,512},
}};

// ======================= CCarFXRenderer =======================
alignas(16) inline std::array<std::array<uint8,0x60>,16> dirtTex{};    // texturas de la tabla (Shutdown) / copias (InitialiseDirtTexture)
alignas(16) inline std::array<uint8,0x60> srcTex{};                    // textura leida de "vehiclegrunge256"
alignas(16) inline std::array<std::array<uint8,0x40>,17> rasters{};    // 0 = origen, 1..16 = copias
alignas(16) inline std::array<std::array<uint8,0x100>,17> pixels{};
alignas(16) inline std::array<std::array<uint8,0x70>,3> atomsB{};      // RpAtomic falsos
alignas(16) inline std::array<uint8,0x40> clumpB{};
inline int atomCount{}; inline int createResult{}, pluginResult{}; inline uint32 pipeId{};
inline int slotResult{}; inline int copyIndex{};
inline void* __cdecl S_ForAllAtomics(void* clumpP,void* cb,void* data){
    // la rutina de recorrido de RW: se llama al callback con cada atomic mientras devuelva distinto de cero
    const bool known=(uintptr_t(cb)==0x5D5B30)||(uintptr_t(cb)==0x5D5B60)||FollowIn(uintptr_t(cb))==FollowIn(Addr(&CCarFXRenderer::SetCustomFXAtomicRenderPipelinesVMICB));
    trace.push_back({"RpClumpForAllAtomics",Name(uintptr_t(clumpP)),known ? json("SetCustomFX_callback") : Name(uintptr_t(cb)),Name(uintptr_t(data)),atomCount});
    for(int k=0;k<atomCount;++k){ const auto r=reinterpret_cast<void*(__cdecl*)(void*,void*)>(cb)(atomsB[k].data(),data); if(!r)break; }
    return clumpP; }
inline RwBool __cdecl S_CreatePipe(){ T("CreatePipe"); return createResult; }
inline void __cdecl S_DestroyPipe(){ T("DestroyPipe"); }
inline RwBool __cdecl S_RegisterPlugin(){ T("RegisterPlugin"); return pluginResult; }
inline void __cdecl S_PreRenderUpd(){ T("PreRenderUpdate"); }
inline void* __cdecl S_PipeAtomic(void* atomic){ T("CustomPipeAtomicSetup",atomic); return atomic; }
inline uint32 __cdecl S_PipeId(void* atomic){ T("GetPipelineID",atomic); return pipeId; }
inline int32 __cdecl S_FindSlot(const char* n){ T("FindTxdSlot",Fnv(n,strlen(n))); return slotResult; }
inline void __cdecl S_PushTxd(){ T("PushCurrentTxd"); }
inline void __cdecl S_PopTxd(){ T("PopCurrentTxd"); }
inline void __cdecl S_SetTxd(int32 s){ T("SetCurrentTxd",s); }
inline void* __cdecl S_TexRead(const char* n,const char* m){ T("RwTextureRead",Fnv(n,strlen(n)),m!=nullptr); return srcTex.data(); }
inline void* __cdecl S_TexDestroy(void* t){ T("RwTextureDestroy",t); return t; }
inline void* __cdecl S_TexName(void* t,const char* n){ T("RwTextureSetName",t,Fnv(n,strlen(n))); std::memcpy(static_cast<uint8*>(t)+0x10,n,strlen(n)+1); return t; }
inline void* __cdecl S_CopyTex(void* t){ T("CopyTexture",t); const int k=std::min(copyIndex,15); ++copyIndex; return dirtTex[k].data(); }
inline uint8* __cdecl S_RasterLock(void* r,uint8 level,int32 mode){ T("RwRasterLock",r,level,mode); const int k=int((static_cast<uint8*>(r)-reinterpret_cast<uint8*>(rasters.data()))/0x40); return pixels[k].data(); }
inline void* __cdecl S_RasterUnlock(void* r){ T("RwRasterUnlock",r); return r; }

// ======================= CStencilShadows =======================
alignas(16) inline std::array<uint8,0x100> camBuf{}, rasterBuf{};
alignas(16) inline std::array<std::array<uint8,0x20>,8> sobj{};           // CStencilShadowObject falsos (0x1C) para Render/UpdateHierarchy/Process
alignas(16) inline std::array<std::array<float,3>,1536> sdata{};          // vertices de sombra falsos (compartidos)
alignas(16) inline std::array<uint8,0x1000> arena{};  inline size_t arenaTop{};
alignas(16) inline std::array<uint8,0x60> smat{}, smat2{};
alignas(16) inline std::array<std::array<uint8,0x60>,3> sent{};           // entidades falsas
alignas(16) inline std::array<std::array<uint8,0x40>,3> scol{};           // modelos de colision falsos
alignas(16) inline std::array<std::array<uint8,0x40>,3> sdat{};           // CCollisionData falsos
alignas(16) inline std::array<std::array<uint8,0x0C0>,3> sverts{};        // vertices de sombra comprimidos (int16 x3) falsos
alignas(16) inline std::array<std::array<uint8,0x100>,3> stris{};         // triangulos de sombra (8 bytes)
alignas(16) inline std::array<uint8,0x6000> edgeBuf{};
alignas(16) inline std::array<CVector,2048> ptsObj{}, ptsWorld{};
inline int transformResult{};
inline RwBool __cdecl S_RenderState(RwRenderState st,void* v){ trace.push_back({"render_state",int(st),uint32(uintptr_t(v))}); return TRUE; }
inline void __cdecl S_Little(){ T("LittleTest"); }
inline void* __cdecl S_Im3DTransform(void* verts,uint32 n,void* ltm,uint32 flags){
    trace.push_back({"Im3DTransform",verts==reinterpret_cast<void*>(0xC4D958),n,Name(uintptr_t(ltm)),flags,n<=2048 ? json(Fnv(verts,size_t(n)*0x24)) : json("grande")}); return transformResult ? reinterpret_cast<void*>(1) : nullptr; }
inline RwBool __cdecl S_Im3DRender(int32 prim,uint16* idx,int32 n){ trace.push_back({"Im3DRenderIndexedPrimitive",prim,n,n>=0&&n<=4096 ? json(Fnv(idx,size_t(n)*2)) : json("grande")}); return TRUE; }
inline RwBool __cdecl S_Im3DEnd(){ T("Im3DEnd"); return TRUE; }
inline void __cdecl S_DrawRect(const CRect& r,const CRGBA& c){ T("DrawRect",r.left,r.top,r.right,r.bottom,c.r,c.g,c.b,c.a); }
inline void __cdecl S_InitPerFrame(){ T("InitPerFrame"); }
inline void __cdecl S_StencilClear(uint32 v){ T("SetStencilClear",v); }
inline void* __cdecl S_Malloc(uint32 n,uint32){ T("malloc",n); const size_t sz=(size_t(n)+15)&~size_t(15); if(arenaTop+sz>arena.size())return nullptr; void* p=arena.data()+arenaTop; arenaTop+=sz; return p; }
inline void __cdecl S_Free(void* p){ T("free",p); }
inline void __fastcall S_RegRef(void* self,void*,void* ref){ T("RegisterReference",self,ref); }
inline CVector* __fastcall S_Transform(void* self,void*,CVector* ret,const CVector* in){
    T("TransformFromObjectSpace",self,in->x,in->y,in->z); *ret=CVector{in->x+At<float>(self,4),in->y+At<float>(self,8),in->z+At<float>(self,0xC)}; return ret; }
inline void __cdecl S_Register(void* cam,uint32 b){ T("RegisterStencilShadows",F(At<float>(cam,0)),F(At<float>(cam,4)),F(At<float>(cam,8)),uint8(b)); }
inline uint32 __cdecl S_RenderVeh(void* o){ T("RenderForVehicle",o); return 1; }
inline uint32 __cdecl S_RenderObj(void* o){ T("RenderForObject",o); return 1; }
inline void* sVehBuf{}; // bufer del vehiculo de RenderForVehicle (copia de sent[0], fuera del array)
inline void* __fastcall S_ColModelS(void* self,void*){ T("GetColModel",self); if(self&&self==sVehBuf)return scol[0].data(); const int k=int((static_cast<uint8*>(self)-reinterpret_cast<uint8*>(sent.data()))/0x60); return (k>=0&&k<3) ? scol[k].data() : nullptr; }
inline void* __fastcall S_AllocMatrix(void* self,void*){ T("AllocateMatrix",self); At<void*>(self,0x14)=smat.data(); return self; }
inline void __fastcall S_UpdMatrix(void* self,void*,void* out){ T("UpdateMatrix",self,out); }
inline uint32 miType{};
inline uint32 __fastcall S_ModelType(void* self,void*){ T("GetModelType",self); return miType; }
alignas(16) inline std::array<uint8,0x40> miObj{}; inline std::array<void*,8> miVt{};

// ======================= fixtures comunes =======================
inline void FillMatrix(std::array<uint8,0x60>& m,Rng& r){
    for(int k=0;k<16;++k)At<float>(m.data(),4*k)=(k%4==3)?0.f:r.uni(-1.f,1.f);
    for(int k=0;k<3;++k)At<float>(m.data(),0x30+4*k)=r.uni(-50.f,50.f);
    At<uint32>(m.data(),0x44)=0; }
inline void FillPads(Rng& r){
    for(int n=0;n<2;++n){ uint8* pad=reinterpret_cast<uint8*>(0xB73458)+0x134*n;
        for(int k=0;k<0x134;++k)pad[k]=uint8(r.next());
        for(int off=0;off<0x60;off+=2)At<int16>(pad,off)=r.pct(30) ? int16(0) : int16(int(r.below(257))-128);
        At<uint16>(pad,0x10A)=uint16(r.below(4)); At<uint16>(pad,0x10E)=uint16(r.pct(15)?1:0);
        At<int16>(pad,0x2C)=int16(r.pct(30)?1:0);
        At<int32>(pad,0x74)=int32(r.below(10)); } // DrunkDrivingBufferUsed indexa SteeringLeftRightBuffer (std::array<int16,10>: la DLL de depuracion aborta fuera de rango)
    uint8* ms=reinterpret_cast<uint8*>(0xB73418); for(int k=0;k<7;++k)ms[k]=uint8(r.below(2)*0x80);
    At<float>(ms,8)=r.uni(-2.f,2.f); At<float>(ms,0xC)=r.pct(40)?0.f:r.uni(-30.f,30.f); At<float>(ms,0x10)=r.pct(40)?0.f:r.uni(-30.f,30.f); }
inline void FillHd(Rng& r){
    for(auto& b:hd)b=uint8(r.next());
    for(int k=0;k<3;++k)At<float>(hd.data(),0x14+4*k)=r.uni(-0.5f,0.5f);
    At<float>(hd.data(),4)=r.uni(500.f,3000.f); At<float>(hd.data(),0xA0)=r.uni(10.f,40.f); At<float>(hd.data(),0x88)=r.uni(5.f,150.f); }
inline void FillBh(Rng& r){
    for(auto& b:bh)b=uint8(r.next());
    for(int k=1;k<16;++k)At<float>(bh.data(),4*k)=r.uni(0.f,1.f);
    At<float>(bh.data(),0x1C)=r.uni(0.05f,0.99f); At<float>(bh.data(),0x2C)=r.uni(0.f,0.8f); At<float>(bh.data(),0x30)=r.uni(0.f,0.8f); }
inline void FillPed(std::array<uint8,0x7A0>& p,Rng& r,bool player){
    for(auto& b:p)b=uint8(r.next());
    At<int32>(p.data(),0x598)=player ? int32(r.pct(80)?0:1) : int32(2+r.below(20));
    At<int32>(p.data(),0x530)=r.pct(15) ? 63 : int32(r.below(60));
    At<void*>(p.data(),0x47C)=intel.data(); At<void*>(p.data(),0x480)=pdata.data(); At<void*>(p.data(),0x18)=clump.data();
    for(auto& b:pdata)b=uint8(r.next()); At<float>(pdata.data(),0x18)=r.pct(50)?r.uni(-100.f,-1.f):r.uni(0.f,100.f); At<float>(pdata.data(),0x1C)=r.uni(0.f,12.f); }
inline void FillCompression(uint8* v,Rng& r){
    const uint32 mode=r.below(4);
    for(int k=0;k<4;++k){ float c=r.pct(40)?1.f:r.uni(0.f,1.f); if(mode==0)c=1.f; if(mode==1&&(k==0||k==2))c=1.f; if(mode==2&&(k==1||k==3))c=1.f; At<float>(v,0x7D4+4*k)=c; }
    for(int k=0;k<4;++k){ float c=r.pct(40)?1.f:r.uni(0.f,2.f); if(mode==2&&(k==1||k==3))c=1.f; At<float>(v,0x7F4+4*k)=c; } }
inline void FillQuad(Rng& r){
    uint8* Q=quad.data();
    for(auto& b:quad)b=uint8(r.next());
    At<void*>(Q,0)=vtQuad.data(); At<void*>(Q,0x14)=vmat.data(); FillMatrix(vmat,r);
    At<void*>(Q,0x384)=hd.data(); FillHd(r); At<void*>(Q,0x988)=bh.data(); FillBh(r);
    At<uint8>(Q,0x36)=uint8((r.pct(65)?0u:r.below(12))<<3|(r.below(8)&7u));
    for(int k=0;k<3;++k){ At<float>(Q,0x44+4*k)=r.uni(-8.f,8.f); At<float>(Q,0x50+4*k)=r.uni(-3.f,3.f); At<float>(Q,0xA4+4*k)=r.uni(-0.6f,0.6f); }
    At<float>(Q,0x8C)=r.uni(100.f,4000.f); At<float>(Q,0x90)=r.uni(100.f,6000.f);
    FillCompression(Q,r);
    for(int k=0;k<7;++k)At<float>(Q,0x98C+4*k)=r.uni(-1.f,1.f); At<int32>(Q,0x98C)=int32(10);
    At<float>(Q,0x494)=r.uni(-0.7f,0.7f); At<float>(Q,0x49C)=r.pct(30)?0.f:r.uni(0.f,1.f); At<float>(Q,0x4A0)=r.pct(40)?0.f:r.pick({1.f,-0.5f,0.4f,r.uni(0.f,1.f)});
    At<float>(Q,0x58C)=r.pct(40)?0.f:r.uni(-1.f,1.f); At<uint8>(Q,0x960)=uint8(r.below(5));
    for(int k=0;k<4;++k)At<float>(Q,0x838+4*k)=r.uni(-0.3f,0.3f);
    At<float>(Q,0x9A8)=r.uni(0.f,2.f); }
inline void FillBmx(Rng& r){
    uint8* B=bmx.data();
    for(auto& b:bmx)b=uint8(r.next());
    At<void*>(B,0)=vtBmx.data(); At<void*>(B,0x14)=vmat.data(); FillMatrix(vmat,r);
    At<void*>(B,0x384)=hd.data(); FillHd(r); At<void*>(B,0x63C)=bh.data(); FillBh(r);
    At<uint8>(B,0x36)=uint8((r.pct(50)?0u:r.below(12))<<3|(r.below(8)&7u));
    At<int16>(B,0x22)=int16(r.pick({510.f,468.f,481.f,462.f})); At<void*>(B,0x18)=clump.data();
    for(int k=0;k<3;++k){ At<float>(B,0x44+4*k)=r.uni(-8.f,8.f); At<float>(B,0x50+4*k)=r.uni(-3.f,3.f); At<float>(B,0xA4+4*k)=r.uni(-0.6f,0.6f); }
    At<float>(B,0x8C)=r.uni(10.f,150.f); At<float>(B,0x90)=r.uni(10.f,200.f);
    At<int32>(B,0x640)=int32(7+r.below(3)); for(int k=1;k<7;++k)At<float>(B,0x640+4*k)=r.uni(-1.f,1.f);
    for(int k=0;k<4;++k){ At<float>(B,0x710+4*k)=r.pct(40)?1.f:r.uni(0.f,1.f); At<float>(B,0x720+4*k)=r.pct(40)?1.f:r.uni(0.f,1.f); At<float>(B,0x730+4*k)=r.pct(35)?0.f:r.uni(0.f,2.f); }
    At<float>(B,0x494)=r.uni(-0.7f,0.7f); At<float>(B,0x49C)=r.pct(30)?0.f:r.uni(0.f,1.f); At<float>(B,0x4A0)=r.pct(40)?0.f:r.pick({1.f,-0.5f,0.4f,r.uni(0.f,1.f)});
    At<uint8>(B,0x4B4)=uint8(r.below(6)); At<uint32>(B,0x514)=r.pct(50)?0u:r.below(5);
    At<float>(B,0x814)=r.pct(40)?0.f:r.uni(0.f,30.f); At<float>(B,0x818)=r.pct(40)?0.f:r.uni(0.f,8.f); At<float>(B,0x81C)=r.uni(-0.5f,0.5f);
    At<float>(B,0x820)=r.uni(-6.f,6.f); At<float>(B,0x82C)=r.uni(0.5f,1.2f); At<float>(B,0x830)=r.uni(0.5f,1.5f);
    At<float>(B,0x798)=r.uni(0.4f,1.5f); At<float>(B,0x79C)=r.uni(-0.2f,0.2f); At<float>(B,0x7A0)=r.uni(-0.2f,0.2f); At<float>(B,0x7A4)=r.uni(-0.5f,0.5f);
    for(int k=0;k<4;++k){ At<float>(B,0x770+4*k)=r.uni(0.1f,0.6f); At<float>(B,0x780+4*k)=r.uni(0.3f,1.2f); }
    for(int k=0;k<2;++k){ At<float>(B,0x760+4*k)=r.uni(-0.3f,0.3f); At<float>(B,0x768+4*k)=r.uni(-0.3f,0.3f); At<float>(B,0x750+4*k)=r.uni(-3.f,3.f); }
    At<uint8>(B,0x65C)=uint8(r.below(3)); At<uint8>(B,0x65D)=uint8(r.below(3));
    At<uint8>(B,0x7B4)=uint8(r.below(2)); At<uint8>(B,0x7B5)=uint8(r.below(2)); At<uint8>(B,0x834)=uint8(r.below(2));
    At<uint8>(B,0x804)=uint8(r.below(5)); At<int32>(B,0x80C)=int32(r.below(4)); At<int32>(B,0x810)=int32(r.pct(30)?3:r.below(3)); }
inline void FillAssocs(Rng& r){
    for(auto& a:assocs){ for(auto& b:a)b=uint8(r.next());
        At<void*>(a.data(),0x14)=hier.data();
        At<float>(a.data(),0x18)=r.pct(20)?0.f:r.pct(20)?1.f:r.uni(0.f,1.f);
        At<float>(a.data(),0x1C)=r.pct(30)?0.f:r.uni(-8.f,8.f);
        At<float>(a.data(),0x20)=r.uni(0.f,2.f); At<float>(a.data(),0x24)=r.uni(0.f,3.f); At<uint16>(a.data(),0x2E)=uint16(r.next()); }
    for(auto& a:fresh){ for(auto& b:a)b=uint8(r.next()); At<void*>(a.data(),0x14)=hier.data(); At<float>(a.data(),0x18)=r.uni(0.f,1.f); At<float>(a.data(),0x1C)=r.uni(-8.f,8.f); At<float>(a.data(),0x20)=r.uni(0.f,1.f); At<uint16>(a.data(),0x2E)=uint16(r.next()); }
    for(auto& b:hier)b=uint8(r.next()); At<float>(hier.data(),0x10)=r.uni(0.2f,3.f);
    for(auto& on:assocOn)on=r.pct(55); freshUsed=0; }
inline void FillFrame(std::array<uint8,0xA8>& fr,Rng& r){ for(auto& b:fr)b=uint8(r.next()); for(int k=0;k<12;++k)At<float>(fr.data(),0x10+4*k)=r.uni(-1.f,1.f); for(int k=0;k<3;++k)At<float>(fr.data(),0x40+4*k)=r.uni(-2.f,2.f); }

// ======================= grupos de parches =======================
inline void PForces(Patches& P){
    P.Add(0x542A50,S_TurnForce); P.Add(Addr(&CPhysical::ApplyTurnForce),S_TurnForce);
    P.Add(0x5429F0,S_MoveForce); P.Add(Addr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),S_MoveForce); }
inline void PAnims(Patches& P){
    P.Add(0x4D68B0,S_GetAssoc); P.Add(Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),S_GetAssoc);
    P.Add(0x4D4610,S_Blend); P.Add(Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),S_Blend);
    P.Add(0x4CEA80,S_SetTime); P.Add(Addr(&CAnimBlendAssociation::SetCurrentTime),S_SetTime);
    P.Add(0x4CEBE0,S_FinishCb); P.Add(Addr(&CAnimBlendAssociation::SetFinishCallback),S_FinishCb); }
inline void PPlayer(Patches& P){
    P.Add(0x609560,S_GetPad); P.Add(Addr(&CPlayerPed::GetPadFromPlayer),S_GetPad);
    P.Add(0x56E210,S_FindPlayer); P.Add(Addr(&FindPlayerPed),S_FindPlayer); }
inline void PMem(Patches& P){
    P.Add(0x72F420,S_Malloc); P.Add(Addr(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc)),S_Malloc);
    P.Add(0x72F430,S_Free); P.Add(Addr(&CMemoryMgr::Free),S_Free); }
inline void PIm3d(Patches& P){
    P.Add(0x541330,S_Little); P.Add(Addr(&LittleTest),S_Little);
    P.Add(0x7EF450,S_Im3DTransform); P.Add(0x7EF550,S_Im3DRender); P.Add(0x7EF520,S_Im3DEnd); }
inline void PSetBase(Patches& P,std::initializer_list<std::tuple<const char*,const char*,uintptr_t,void*>> l){
    for(auto& [cls,fn,exe,stub]:l){ P.Add(exe,reinterpret_cast<void(*)()>(stub)); const auto d=DllBody(cls,fn); if(d)P.Add(d,reinterpret_cast<void(*)()>(stub)); } }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"misc61b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"misc61b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("misc61b");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    InitModule();
    std::string only; { std::ifstream f(dir/"misc61b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,42> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,42> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtables falsas y nombres
    for(auto& v:vtQuad)v=reinterpret_cast<void*>(&V_Trap); vtQuad[24]=reinterpret_cast<void*>(&V_PCI); vtQuad[52]=reinterpret_cast<void*>(&V_Burst); vtQuad[69]=reinterpret_cast<void*>(&V_Hover);
    for(auto& v:vtBmx)v=reinterpret_cast<void*>(&V_Trap); vtBmx[24]=reinterpret_cast<void*>(&V_PCI); vtBmx[52]=reinterpret_cast<void*>(&V_Burst); vtBmx[69]=reinterpret_cast<void*>(&V_Hover);
    if(named.empty()){
        Nm(quad.data(),quad.size(),"veh"); Nm(bmx.data(),bmx.size(),"veh"); Nm(vmat.data(),vmat.size(),"vmat"); Nm(hd.data(),hd.size(),"hd"); Nm(bh.data(),bh.size(),"bh");
        Nm(ped.data(),ped.size(),"ped"); Nm(ped2.data(),ped2.size(),"player"); Nm(pdata.data(),pdata.size(),"pdata"); Nm(intel.data(),intel.size(),"intel"); Nm(task.data(),task.size(),"task");
        Nm(clump.data(),clump.size(),"clump"); Nm(frames.data(),sizeof(frames),"frames"); Nm(mi.data(),mi.size(),"mi"); Nm(assocs.data(),sizeof(assocs),"assocs"); Nm(fresh.data(),sizeof(fresh),"fresh");
        Nm(hier.data(),hier.size(),"hier"); Nm(colModelB.data(),colModelB.size(),"colModel"); Nm(colDataB.data(),colDataB.size(),"colData"); Nm(linesB.data(),linesB.size(),"lines");
        Nm(dirtTex.data(),sizeof(dirtTex),"dirtTex"); Nm(srcTex.data(),srcTex.size(),"srcTex"); Nm(rasters.data(),sizeof(rasters),"rasters"); Nm(atomsB.data(),sizeof(atomsB),"atoms"); Nm(clumpB.data(),clumpB.size(),"clumpB");
        Nm(camBuf.data(),camBuf.size(),"camera"); Nm(rasterBuf.data(),rasterBuf.size(),"raster"); Nm(sobj.data(),sizeof(sobj),"sobj"); Nm(sdata.data(),sizeof(sdata),"sdata"); Nm(arena.data(),arena.size(),"arena");
        Nm(smat.data(),smat.size(),"smat"); Nm(sent.data(),sizeof(sent),"sent"); Nm(scol.data(),sizeof(scol),"scol"); Nm(miObj.data(),miObj.size(),"miObj");
        Nm(reinterpret_cast<void*>(0xB73458),0x268,"pads"); Nm(reinterpret_cast<void*>(0xC6A198),0x700,"stencilObjects"); Nm(ptsObj.data(),sizeof(ptsObj),"ptsObj"); Nm(ptsWorld.data(),sizeof(ptsWorld),"ptsWorld"); Nm(edgeBuf.data(),edgeBuf.size(),"edges");
        dllLaunchCb=DllBody("CBmx","LaunchBunnyHopCB");
    }
    // los clumps falsos deben tener tipo 2 (rpCLUMP) en el byte 0: GetRpClump de la DLL lo comprueba con assert
    clump[0]=2; clumpB[0]=2;
    miVt.fill(nullptr); miVt[4]=reinterpret_cast<void*>(&S_ModelType); At<void*>(miObj.data(),0)=miVt.data();

    // estado global guardado
    if(regions.empty()){
        for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {0xB7CB5C,4},{0xB73458,0x268},{0xB73418,0x14},{0xB6EC2E,2},{0xC1CC00,8},{0x8D3450,0x14},{0x969130,0x70},{0x97D6C0,0x80},{0xC1C820,4},{0xC1C83C,1},
            {0xB72914,4},{0xB7CD78,2},{0xA9AE54,4},{0xC1703C,4},{0x8D12D0,4},{0xC02BD0,0x40},{0xB7C4E8,4},{0x8D5244,0xC},{0xC6A168,0x14},{0xC6A198,0x700},{0xC6A898,2},
            {0xC4B950,0x14008},{0xB74494,4},{0xA9B0C8+4*15,4},{0xA9B0C8+4*19990,40}})Snap(a,n); }
    auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet;

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; std::vector<std::pair<uintptr_t,uint32>> sectorSaves;
            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x61B2u; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); arenaTop=0; trapLeanSet=false;
                for(auto* h:hooks)h->State(candidate);
                const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                json out={{"switches",switches}}; json ret=nullptr;
                Patches P;
                uint8* Q=quad.data(); uint8* B=bmx.data();
                const float ts=r.pct(10)?0.f:r.uni(0.2f,3.f); AtA<float>(0xB7CB5C)=ts;
                auto Obj=[&](auto& buf){ return ReversingProbe::Bytes(buf.data(),buf.size()); };

                if(cls=="CQuadBike") {
                    FillQuad(r); FillPads(r); AtA<uint8>(0xB6EC2E)=uint8(r.below(2)); AtA<uint8>(0xC1CC02)=uint8(r.below(2)); AtA<uint32>(0xC1CC04)=r.below(4);
                    AtA<uint8>(0x8D3450)=uint8(r.pct(85)?1:0);
                    for(int k=0;k<3;++k)AtA<float>(0x8D3458+4*k)=r.pct(60)?(k==2?1.f:0.995f):r.uni(0.8f,1.f);
                    AtA<float>(0x8D3454)=r.pct(60)?-0.4f:r.uni(-1.f,1.f);
                    if(nm=="Fix") {
                        P.Add(0x749B70,S_ForAllAtomics); atomCount=0; At<void*>(Q,0x18)=clump.data();
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(Q);
                        out["obj"]=Obj(quad);
                    }
                    else if(nm=="GetRideAnimData") {
                        const auto p=reinterpret_cast<uintptr_t(__thiscall*)(void*)>(a)(Q); ret=p ? json(p-uintptr_t(Q)) : json(0);
                    }
                    else if(nm=="PreRender") {
                        const int idx=15; AtA<void*>(0xA9B0C8+4*idx)=mi.data(); At<int16>(Q,0x22)=int16(idx);
                        for(auto& b:mi)b=uint8(r.next());
                        for(int k=0;k<4;++k)FillFrame(frames[k],r);
                        const int nodes[4]{16,15,14,17}; for(int k=0;k<4;++k)At<void*>(Q,0x648+4*nodes[k])=(k==0||r.pct(70)) ? frames[k].data() : nullptr;
                        P.Add(0x4C7D20,S_WheelPosn); P.Add(Addr(&CVehicleModelInfo::GetWheelPosn),S_WheelPosn);
                        P.Add(0x6DBBB0,S_TransRot); P.Add(Addr(&CVehicle::SetTransmissionRotation),S_TransRot);
                        PSetBase(P,{{"CAutomobile","PreRender",0x6AAB50,reinterpret_cast<void*>(&S_AutoPreRender)}});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(Q);
                        out["obj"]=Obj(quad); out["frames"]=ReversingProbe::Bytes(frames.data(),4*0xA8);
                    }
                    else if(nm=="ProcessAI") {
                        At<int8>(Q,0x424)=int8(r.pct(60)?-1:int(r.below(8))); for(int k=0;k<8;++k)AtA<uint8>(0x97D6C0+k)=uint8(r.below(2));
                        const bool hasDriver=!r.pct(15);
                        At<void*>(Q,0x460)=hasDriver ? ped.data() : nullptr; FillPed(ped,r,true); At<void*>(Q,0x4C4)=nullptr;
                        for(int k=0;k<3;++k)taskOn[k]=r.pct(15); autoAiResult=r.pct(50);
                        padResult=reinterpret_cast<void*>(0xB73458+0x134*r.below(2));
                        trapLeanSet=r.pct(70); trapLean=r.uni(-1.f,1.f); AtA<uint8>(0x969152)=uint8(r.pct(15)?1:0);
                        colModelResult=colModelB.data();
                        P.Add(0x535300,S_ColModel); P.Add(Addr(&CEntity::GetColModel),S_ColModel);
                        P.Add(0x571A90,S_Prune); P.Add(Addr(&CEntity::PruneReferences),S_Prune);
                        P.Add(0x681740,S_FindTask); P.Add(Addr(&CTaskManager::FindActiveTaskByType),S_FindTask);
                        P.Add(0x741FD0,S_DoDriveBy); P.Add(Addr(&CVehicle::DoDriveByShootings),S_DoDriveBy);
                        PForces(P); PPlayer(P); PSetBase(P,{{"CAutomobile","ProcessAI",0x6B4800,reinterpret_cast<void*>(&S_AutoProcessAI)}});
                        uint32 extra=r.next(); const uint32 extra0=extra;
                        ret=reinterpret_cast<bool(__thiscall*)(void*,uint32*)>(a)(Q,&extra); out["extra"]=extra^extra0;
                        out["obj"]=Obj(quad);
                    }
                    else if(nm=="ProcessControl") {
                        PForces(P); PSetBase(P,{{"CAutomobile","ProcessControl",0x6B1880,reinterpret_cast<void*>(&S_AutoProcessControl)}});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(Q);
                        out["obj"]=Obj(quad);
                    }
                    else if(nm=="ProcessControlInputs") {
                        const uint8 n=uint8(r.below(2));
                        PSetBase(P,{{"CAutomobile","ProcessControlInputs",0x6AD690,reinterpret_cast<void*>(&S_AutoPCI)}});
                        reinterpret_cast<void(__thiscall*)(void*,uint8)>(a)(Q,n);
                        out["obj"]=Obj(quad); out["lastInput"]=AtA<uint32>(0xC1CC04);
                    }
                    else if(nm=="ProcessDrivingAnims") {
                        P.Add(0x6B7280,S_RiderAnims); P.Add(Addr(&CBike::ProcessRiderAnims),S_RiderAnims);
                        FillPed(ped,r,true);
                        reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(a)(Q,ped.data(),uint32(r.below(2)));
                        out["obj"]=Obj(quad);
                    }
                    else if(nm=="ProcessSuspension") {
                        PSetBase(P,{{"CAutomobile","ProcessSuspension",0x6AFB10,reinterpret_cast<void*>(&S_AutoSusp)}});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(Q); out["obj"]=Obj(quad);
                    }
                    else if(nm=="ResetSuspension") {
                        PSetBase(P,{{"CAutomobile","ResetSuspension",0x6A2AE0,reinterpret_cast<void*>(&S_AutoReset)}});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(Q); out["obj"]=Obj(quad);
                    }
                    else if(nm=="SetupSuspensionLines") {
                        PSetBase(P,{{"CAutomobile","SetupSuspensionLines",0x6A65D0,reinterpret_cast<void*>(&S_AutoSetup)}});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(Q); out["obj"]=Obj(quad);
                    }
                    else if(nm=="SetupDamageAfterLoad") {
                        At<void*>(Q,0x678)=r.pct(60)?frames[0].data():nullptr; At<void*>(Q,0x67C)=r.pct(60)?frames[1].data():nullptr;
                        P.Add(0x6B1350,S_BumperDamage); P.Add(Addr(&CAutomobile::SetBumperDamage),S_BumperDamage);
                        P.Add(0x6B1600,S_DoorDamage); P.Add(Addr(&CAutomobile::SetDoorDamage),S_DoorDamage);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(Q); out["obj"]=Obj(quad);
                    }
                }
                else if(cls=="CBmx") {
                    FillBmx(r); FillPads(r); FillAssocs(r); FillPed(ped,r,true); FillPed(ped2,r,true);
                    At<void*>(B,0x460)=r.pct(12)?nullptr:ped.data();
                    AtA<uint32>(0xC1C820)=F(r.pct(70)?0.f:r.uni(-1.f,1.f)); AtA<uint8>(0xC1C83C)=uint8(r.pct(20)?1:0);
                    if(nm=="BurstTyre") { ret=reinterpret_cast<bool(__thiscall*)(void*,uint8,bool)>(a)(B,uint8(r.below(4)),r.pct(50)); }
                    else if(nm=="SetUpWheelColModel") { ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(B,colModelB.data()); }
                    else if(nm=="FindWheelWidth") { ret=F(reinterpret_cast<float(__thiscall*)(void*,bool)>(a)(B,r.pct(50))); }
                    else if(nm=="LaunchBunnyHopCB") {
                        fatResult=r.uni(0.5f,2.f); AtA<uint8>(0x969161)=uint8(r.pct(25)?1:0); for(int k=0;k<4;++k)At<float>(B,0x730+4*k)=r.pct(40)?0.f:r.uni(0.f,2.f);
                        PForces(P); P.Add(0x559AF0,S_Fat); P.Add(Addr(&CStats::GetFatAndMuscleModifier),S_Fat);
                        reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(assocs[0].data(),B);
                        out["obj"]=Obj(bmx);
                    }
                    else if(nm=="ProcessBunnyHop") {
                        playerResult=ped2.data(); padResult=reinterpret_cast<void*>(0xB73458+0x134*r.below(2));
                        { uint8* pad=static_cast<uint8*>(padResult); At<int16>(pad,8)=int16(r.pct(55)?1:0); At<int16>(pad,0x38)=int16(r.pct(35)?1:0); }
                        PAnims(P); PPlayer(P); atomCount=0;
                        dllLaunchCb=DllBody("CBmx","LaunchBunnyHopCB");
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(B);
                        out["obj"]=Obj(bmx); out["assocs"]=ReversingProbe::Bytes(assocs.data(),sizeof(assocs)); out["fresh"]=ReversingProbe::Bytes(fresh.data(),sizeof(fresh)); out["pdata"]=Obj(pdata);
                    }
                    else if(nm=="ProcessControl") {
                        PAnims(P); PSetBase(P,{{"CBike","ProcessControl",0x6B9250,reinterpret_cast<void*>(&S_BikeProcessControl)}});
                        At<uint32>(B,0x1C)=(At<uint32>(B,0x1C)&~0x40u)|(r.pct(15)?0x40u:0u);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(B);
                        out["obj"]=Obj(bmx);
                    }
                    else if(nm=="ProcessDrivingAnims") {
                        PAnims(P); PForces(P);
                        P.Add(0x5DEC00,S_SetMoveState); P.Add(Addr(&CPed::SetMoveState),S_SetMoveState);
                        P.Add(0x6B7280,S_RiderAnims); P.Add(Addr(&CBike::ProcessRiderAnims),S_RiderAnims);
                        At<uint32>(B,0x1C)=(At<uint32>(B,0x1C)&~0x20000u)|(r.pct(20)?0x20000u:0u);
                        At<uint8>(B,0x428)=uint8(r.below(256)); At<uint8>(B,0x614)=uint8(r.below(256));
                        for(int k=0;k<3;++k)At<float>(B,0x738+4*(k%2))=r.pct(35)?0.f:r.uni(0.f,2.f);
                        if(r.pct(20))At<void*>(ped.data(),0x480)=nullptr;
                        FillPed(ped,r,r.pct(75)); if(r.pct(20))At<void*>(ped.data(),0x480)=nullptr;
                        // el conductor no puede ser nulo salvo con la bandera de pantalla (el original lo exige)
                        const bool offscreen=(At<uint32>(B,0x1C)&0x20000u)!=0; void* drv=(offscreen&&r.pct(15))?nullptr:ped.data();
                        reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(a)(B,drv,uint32(r.below(2)));
                        out["obj"]=Obj(bmx); out["assocs"]=ReversingProbe::Bytes(assocs.data(),sizeof(assocs)); out["fresh"]=ReversingProbe::Bytes(fresh.data(),sizeof(fresh));
                    }
                    else if(nm=="PreRender") {
                        const int idx=15; AtA<void*>(0xA9B0C8+4*idx)=mi.data(); At<int16>(B,0x22)=int16(idx);
                        for(auto& b:mi)b=uint8(r.next()); At<float>(mi.data(),0x40)=r.uni(0.2f,0.5f); At<float>(mi.data(),0x44)=r.uni(0.2f,0.5f); At<float>(mi.data(),0x58)=r.uni(5.f,40.f);
                        for(int k=0;k<10;++k){ FillFrame(frames[k],r); At<void*>(B,0x5A0+4*k)=frames[k].data(); }
                        for(int k:{1,2,3,6,7,8,9})if(r.pct(25))At<void*>(B,0x5A0+4*k)=nullptr;
                        for(auto& b:colModelB)b=uint8(r.next()); At<float>(colModelB.data(),8)=r.uni(-1.2f,-0.2f); At<void*>(colModelB.data(),0x2C)=colDataB.data();
                        for(auto& b:colDataB)b=uint8(r.next()); At<void*>(colDataB.data(),0x10)=linesB.data();
                        for(int k=0;k<4;++k){ for(int c=0;c<3;++c)At<float>(linesB.data(),0x20*k+4*c)=r.uni(-1.f,1.f); }
                        for(int k=0;k<16;++k)At<float>(B,0x5CC+4*k)=(k%4==3)?0.f:r.uni(-1.f,1.f);
                        At<uint32>(B,0x428)=(At<uint32>(B,0x428)&~0x01000000u)|(r.pct(60)?0x01000000u:0u);
                        for(int k=0;k<8;++k)At<uint8>(B,0x744+k)=uint8(r.below(4)==0?r.below(5):0);
                        for(int w=0;w<2;++w){ At<uint32>(B,0x744+4*w)=r.below(5); At<uint8>(B,0x74C+w)=uint8(r.below(2)); At<uint8>(B,0x74E+w)=uint8(r.below(2)); }
                        for(int k=0;k<4;++k){ At<float>(B,0x660+0x2C*k)=r.uni(-1.f,1.f); At<float>(B,0x664+0x2C*k)=r.uni(-1.f,1.f); At<float>(B,0x668+0x2C*k)=r.uni(-1.f,1.f); }
                        colModelResult=colModelB.data();
                        P.Add(0x535300,S_ColModel); P.Add(Addr(&CEntity::GetColModel),S_ColModel);
                        P.Add(0x6DE880,S_Particles); P.Add(Addr(&CVehicle::AddSingleWheelParticles),S_Particles);
                        P.Add(0x6B7150,S_CalcLean); P.Add(Addr(&CBike::CalculateLeanMatrix),S_CalcLean);
                        P.Add(0x70BDA0,S_Shadow); P.Add(Addr(&CShadows::StoreShadowForVehicle),S_Shadow);
                        P.Add(0x542CE0,S_GetSpeed); P.Add(Addr(&CPhysical::GetSpeed),S_GetSpeed);
                        P.Add(0x6D1230,S_WheelRot); P.Add(Addr(&CVehicle::ProcessWheelRotation),S_WheelRot);
                        PSetBase(P,{{"CVehicle","PreRender",0x6D6480,reinterpret_cast<void*>(&S_VehPreRender)}});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(B);
                        out["obj"]=Obj(bmx); out["frames"]=ReversingProbe::Bytes(frames.data(),10*0xA8);
                    }
                    else if(nm=="ProcessAI") {
                        padResult=reinterpret_cast<void*>(0xB73458+0x134*r.below(2));
                        At<uint32>(B,0x1C)=At<uint32>(B,0x1C); At<uint8>(B,0x3DB)=uint8(r.next());
                        At<uint8>(B,0x428)=uint8(r.next()); At<uint8>(B,0x42A)=uint8(r.next()); At<uint8>(B,0x42F)=uint8(r.next()); At<uint8>(B,0x614)=uint8(r.next());
                        At<void*>(B,0x464)=r.pct(30)?ped2.data():nullptr; At<float>(B,0x4A0)=r.pct(40)?0.f:r.pick({1.f,-0.5f,r.uni(0.f,1.f)});
                        At<float>(B,0x3C8)=r.uni(0.f,40.f); At<uint8>(B,0x805)=uint8(r.below(4)); At<uint8>(B,0x806)=uint8(r.below(4));
                        At<float>(B,0x818)=r.pct(30)?0.f:r.uni(0.f,9.f); At<float>(B,0x654)=r.uni(-1.f,1.f); At<float>(B,0x658)=r.uni(-1.f,1.f);
                        { std::array<float,4> c{}; const uint32 mode=r.below(3); for(int k=0;k<4;++k)c[k]=(mode==0||r.pct(40))?1.f:r.uni(0.f,1.f); for(int k=0;k<4;++k)At<float>(B,0x710+4*k)=c[k]; }
                        const int idx=15; AtA<void*>(0xA9B0C8+4*idx)=mi.data(); At<int16>(B,0x22)=int16(r.pct(25)?468:int(r.below(2)?510:481));
                        for(auto& b:mi)b=uint8(r.next()); At<float>(mi.data(),0x40)=r.uni(0.2f,0.5f); At<float>(mi.data(),0x44)=r.uni(0.2f,0.5f);
                        // el modelo de la bici se busca por indice 15 solo para la rama simple; para el resto se deja el indice real
                        if(!(r.pct(50)))At<int16>(B,0x22)=int16(r.pct(25)?468:int(r.below(2)?510:481)); else At<int16>(B,0x22)=int16(idx);
                        for(int k=0;k<3;++k)At<float>(B,0x738+4*(k%2))=At<float>(B,0x738+4*(k%2));
                        for(int k=0;k<4;++k)taskOn[k]=false;
                        for(int k=0;k<12;++k)assocOn[k]=r.pct(35);
                        sprintResult=r.pct(40)?r.uni(1.21f,3.f):r.uni(0.f,1.2f); buttonResult=r.pct(50)?r.uni(1.01f,2.f):r.uni(-0.5f,1.f); energyResult=r.pct(50); fatResult=r.uni(0.5f,2.f);
                        trapLeanSet=false;
                        PAnims(P); PForces(P); PPlayer(P);
                        P.Add(0x609560,S_GetPad); P.Add(Addr(&CPlayerPed::GetPadFromPlayer),S_GetPad);
                        P.Add(0x571A90,S_Prune); P.Add(Addr(&CEntity::PruneReferences),S_Prune);
                        P.Add(0x741FD0,S_DoDriveBy); P.Add(Addr(&CVehicle::DoDriveByShootings),S_DoDriveBy);
                        P.Add(0x6B6D40,S_SoftGround); P.Add(Addr(&CBike::DoSoftGroundResistance),S_SoftGround);
                        P.Add(0x6C0590,S_BunnyHop); P.Add(Addr(&CBmx::ProcessBunnyHop),S_BunnyHop);
                        P.Add(0x60A610,S_Sprint); P.Add(Addr(&CPlayerPed::ControlButtonSprint),S_Sprint);
                        P.Add(0x60A820,S_ButtonSprint); P.Add(Addr(&CPlayerPed::GetButtonSprintResults),S_ButtonSprint);
                        P.Add(0x60A550,S_Energy); P.Add(Addr(&CPlayerPed::HandleSprintEnergy),S_Energy);
                        P.Add(0x55C780,S_UpdateStats); P.Add(Addr(&CStats::UpdateStatsWhenCycling),S_UpdateStats);
                        P.Add(0x559AF0,S_Fat); P.Add(Addr(&CStats::GetFatAndMuscleModifier),S_Fat);
                        P.Add(0x6D24F0,S_ActivateBomb); P.Add(Addr(&CVehicle::ActivateBomb),S_ActivateBomb);
                        P.Add(0x41DA30,S_UpdateCarAI); P.Add(Addr(&CCarAI::UpdateCarAI),S_UpdateCarAI);
                        P.Add(0x436540,S_OnRails); P.Add(Addr(&CCarCtrl::UpdateCarOnRails),S_OnRails);
                        P.Add(0x437C20,S_Steer); P.Add(Addr(&CCarCtrl::SteerAICarWithPhysics),S_Steer);
                        P.Add(0x6D0530,S_CalcGear); P.Add(Addr(&cTransmission::CalculateGearForSimpleCar),S_CalcGear);
                        P.Add(0x6D1230,S_WheelRot); P.Add(Addr(&CVehicle::ProcessWheelRotation),S_WheelRot);
                        P.Add(0x6B7130,S_PlayHorn); P.Add(Addr(&CBike::PlayHornIfNecessary),S_PlayHorn);
                        P.Add(0x6B5F90,S_ReduceHorn); P.Add(Addr(&CBike::ReduceHornCounter),S_ReduceHorn);
                        PSetBase(P,{{"CPhysical","ProcessControl",0x5485E0,reinterpret_cast<void*>(&S_PhysProcessControl)}});
                        trapLeanSet=r.pct(60); trapLean=r.uni(-1.f,1.f); AtA<uint8>(0x969152)=uint8(r.below(2));
                        uint32 extra=r.next(); const uint32 extra0=extra;
                        ret=reinterpret_cast<bool(__thiscall*)(void*,uint32*)>(a)(B,&extra); out["extra"]=extra^extra0;
                        out["obj"]=Obj(bmx); out["assocs"]=ReversingProbe::Bytes(assocs.data(),sizeof(assocs)); out["pdata"]=Obj(pdata);
                    }
                }
                else if(cls=="CCarFXRenderer") {
                    for(auto& t:dirtTex)for(auto& b:t)b=uint8(r.next()); for(auto& b:srcTex)b=uint8(r.next());
                    createResult=int(r.pct(50)?0:int(1+r.below(5))); pluginResult=int(r.pct(50)?0:int(1+r.below(5))); atomCount=int(r.below(4)); pipeId=r.pct(50)?0x53F2009Au:r.next();
                    slotResult=int(r.below(100)); copyIndex=0; atomsB[0][0]=1;
                    for(int k=0;k<16;++k)AtA<void*>(0xC02BD0+4*k)=r.pct(20)?nullptr:static_cast<void*>(dirtTex[k].data());
                    AtA<float>(0x8D12D0)=r.uni(0.f,3.f);
                    if(nm=="Initialise") { P.Add(0x5DA020,S_CreatePipe); P.Add(Addr(&CCustomCarEnvMapPipeline::CreatePipe),S_CreatePipe); ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                    else if(nm=="RegisterPlugins") { P.Add(0x5DA450,S_RegisterPlugin); P.Add(Addr(&CCustomCarEnvMapPipeline::RegisterPlugin),S_RegisterPlugin); ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                    else if(nm=="PreRenderUpdate") { P.Add(0x5D8870,S_PreRenderUpd); P.Add(Addr(&CCustomCarEnvMapPipeline::PreRenderUpdate),S_PreRenderUpd); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="Shutdown") {
                        P.Add(0x7F3820,S_TexDestroy); P.Add(0x5DA130,S_DestroyPipe); P.Add(Addr(&CCustomCarEnvMapPipeline::DestroyPipe),S_DestroyPipe);
                        reinterpret_cast<void(__cdecl*)()>(a)(); out["table"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC02BD0),0x40);
                    }
                    else if(nm=="CustomCarPipeAtomicSetup") {
                        P.Add(0x5DA610,S_PipeAtomic); P.Add(Addr(&CCustomCarEnvMapPipeline::CustomPipeAtomicSetup),S_PipeAtomic);
                        ret=json(Name(uintptr_t(reinterpret_cast<void*(__cdecl*)(void*)>(a)(atomsB[r.below(3)].data()))));
                    }
                    else if(nm=="SetCustomFXAtomicRenderPipelinesVMICB") {
                        P.Add(0x5DA610,S_PipeAtomic); P.Add(Addr(&CCustomCarEnvMapPipeline::CustomPipeAtomicSetup),S_PipeAtomic);
                        ret=json(Name(uintptr_t(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atomsB[r.below(3)].data(),nullptr))));
                    }
                    else if(nm=="CustomCarPipeClumpSetup") {
                        P.Add(0x5DA610,S_PipeAtomic); P.Add(Addr(&CCustomCarEnvMapPipeline::CustomPipeAtomicSetup),S_PipeAtomic); P.Add(0x749B70,S_ForAllAtomics);
                        ret=json(Name(uintptr_t(reinterpret_cast<void*(__cdecl*)(void*)>(a)(clumpB.data()))));
                    }
                    else if(nm=="IsCCPCPipelineAttached") {
                        P.Add(0x72FC40,S_PipeId); P.Add(Addr(&GetPipelineID),S_PipeId);
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(atomsB[r.below(3)].data());
                    }
                    else if(nm=="SetFxEnvMapLightMult") {
                        const float v=r.pct(50)?r.uni(0.f,3.f):r.pick({0.f,1.f,-1.f,100.f});
                        reinterpret_cast<void(__cdecl*)(float)>(a)(v); out["mult"]=AtA<uint32>(0x8D12D0);
                    }
                    else if(nm=="InitialiseDirtTexture") {
                        P.Add(0x731850,S_FindSlot); P.Add(Addr(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),S_FindSlot);
                        P.Add(0x7316A0,S_PushTxd); P.Add(Addr(&CTxdStore::PushCurrentTxd),S_PushTxd);
                        P.Add(0x7316B0,S_PopTxd); P.Add(Addr(&CTxdStore::PopCurrentTxd),S_PopTxd);
                        P.Add(0x7319C0,S_SetTxd); P.Add(Addr(&CTxdStore::SetCurrentTxd),S_SetTxd);
                        P.Add(0x7F3AC0,S_TexRead); P.Add(0x7F38A0,S_TexName); P.Add(0x7FB2D0,S_RasterLock); P.Add(0x7FAEC0,S_RasterUnlock);
                        P.Add(0x5A5730,S_CopyTex); P.Add(Addr(&CClothesBuilder::CopyTexture),S_CopyTex);
                        for(auto& t:dirtTex){ for(auto& b:t)b=uint8(r.next()); }
                        for(int k=0;k<17;++k){ for(auto& b:rasters[k])b=uint8(r.next()); const int w=r.pct(12)?0:int(1+r.below(6)), h=r.pct(12)?0:int(1+r.below(6)); At<int32>(rasters[k].data(),0xC)=w; At<int32>(rasters[k].data(),0x10)=h; for(auto& b:pixels[k])b=uint8(r.next()); }
                        for(auto& b:srcTex)b=uint8(r.next()); At<void*>(srcTex.data(),0)=rasters[0].data();
                        for(int k=0;k<16;++k)At<void*>(dirtTex[k].data(),0)=rasters[k+1].data();
                        ret=reinterpret_cast<bool(__cdecl*)()>(a)();
                        out["table"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC02BD0),0x40); out["src"]=Obj(srcTex); out["dirt"]=ReversingProbe::Bytes(dirtTex.data(),sizeof(dirtTex)); out["pixels"]=ReversingProbe::Bytes(pixels.data(),sizeof(pixels));
                    }
                }
                else if(cls=="CStencilShadows") {
                    fpSet=S_RenderState;
                    auto* objs=reinterpret_cast<std::array<uint8,0x1C>*>(0xC6A198);
                    auto Link=[&](uint8* o,void* prev,void* next){ At<void*>(o,0x18)=prev; At<void*>(o,0x14)=next; };
                    // estado comun de camara y calidad (GraphicsHighQuality / RenderStencilShadows / Process)
                    AtA<void*>(0xC1703C)=camBuf.data(); At<void*>(camBuf.data(),0x60)=rasterBuf.data(); At<int32>(rasterBuf.data(),0x14)=int32(r.pick({16.f,24.f,31.f,32.f,32.f,33.f}));
                    AtA<int32>(0xA9AE54)=int32(r.pick({-1.f,0.f,1.f,1.f,2.f,3.f}));
                    if(nm=="GraphicsHighQuality") { ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                    else if(nm=="Init") {
                        P.Add(0x7F9D30,S_StencilClear); P.Add(Addr(&RwD3D9SetStencilClear),S_StencilClear);
                        for(int k=0;k<0x700;++k)At<uint8>(objs,k)=uint8(r.next()); AtA<void*>(0xC6A168)=reinterpret_cast<void*>(r.next()); AtA<void*>(0xC6A16C)=reinterpret_cast<void*>(r.next());
                        ret=reinterpret_cast<bool(__cdecl*)()>(a)();
                        out["objs"]=ReversingProbe::Bytes(objs,0x700); out["first"]=Name(AtA<uint32>(0xC6A168)); out["active"]=AtA<uint32>(0xC6A16C);
                    }
                    else if(nm=="Shutdown") {
                        PMem(P);
                        for(int k=0;k<0x700;++k)At<uint8>(objs,k)=uint8(r.next());
                        // lista activa: un subconjunto de los 64 objetos; el resto, disponibles
                        std::vector<int> act,fre; for(int k=0;k<64;++k)(r.pct(40)?act:fre).push_back(k);
                        auto O=[&](int k)->uint8*{ return reinterpret_cast<uint8*>(objs)+0x1C*k; };
                        for(size_t k=0;k<act.size();++k){ uint8* o=O(act[k]); At<void*>(o,0)=nullptr; At<void*>(o,0x10)=r.pct(70)?reinterpret_cast<void*>(arena.data()+0x10*r.below(100)):nullptr; Link(o,k?O(act[k-1]):nullptr,k+1<act.size()?O(act[k+1]):nullptr); }
                        for(size_t k=0;k<fre.size();++k){ uint8* o=O(fre[k]); At<void*>(o,0)=nullptr; At<void*>(o,0x10)=nullptr; Link(o,k?O(fre[k-1]):nullptr,k+1<fre.size()?O(fre[k+1]):nullptr); }
                        AtA<void*>(0xC6A16C)=act.empty()?nullptr:O(act[0]); AtA<void*>(0xC6A168)=fre.empty()?nullptr:O(fre[0]);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["objs"]=ReversingProbe::Bytes(objs,0x700); out["first"]=Name(AtA<uint32>(0xC6A168)); out["active"]=Name(AtA<uint32>(0xC6A16C));
                    }
                    else if(nm=="UpdateHierarchy") {
                        for(auto& o:sobj)for(auto& b:o)b=uint8(r.next());
                        const int n=int(2+r.below(6)); std::vector<int> av,ac; for(int k=0;k<n;++k)(r.pct(60)?av:ac).push_back(k); if(av.empty()){av.push_back(ac.back());ac.pop_back();}
                        for(size_t k=0;k<av.size();++k)Link(sobj[av[k]].data(),k?sobj[av[k-1]].data():nullptr,k+1<av.size()?sobj[av[k+1]].data():nullptr);
                        for(size_t k=0;k<ac.size();++k)Link(sobj[ac[k]].data(),k?sobj[ac[k-1]].data():nullptr,k+1<ac.size()?sobj[ac[k+1]].data():nullptr);
                        void* firstAv=sobj[av[0]].data(); void* firstAc=ac.empty()?nullptr:sobj[ac[0]].data();
                        auto* pick=sobj[av[r.below(uint32(av.size()))]].data();
                        reinterpret_cast<void(__cdecl*)(void**,void**,void*)>(a)(&firstAv,&firstAc,pick);
                        out["objs"]=ReversingProbe::Bytes(sobj.data(),sizeof(sobj)); out["firstAv"]=Name(uintptr_t(firstAv)); out["firstAc"]=Name(uintptr_t(firstAc));
                    }
                    else if(nm=="RenderBuffer") {
                        CVector v{r.uni(-5.f,5.f),r.uni(-5.f,5.f),r.uni(-5.f,5.f)}; if(r.pct(10))v=CVector{0.f,0.f,0.f};
                        for(int k=0;k<3;++k)AtA<float>(0x8D5244+4*k)=r.uni(-3.f,3.f);
                        ret=reinterpret_cast<bool(__cdecl*)(const CVector*)>(a)(&v); out["sun"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D5244),12);
                    }
                    else if(nm=="SunSetPositionFromEntity") {
                        for(auto& b:smat)b=uint8(r.next()); for(int k=0;k<3;++k)At<float>(smat.data(),0x30+4*k)=r.pct(10)?0.f:r.uni(-300.f,300.f);
                        for(auto& b:sent[0])b=uint8(r.next()); At<void*>(sent[0].data(),4)=smat.data();
                        for(int k=0;k<3;++k)AtA<float>(0x8D5244+4*k)=r.uni(-3.f,3.f);
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(r.pct(15)?nullptr:static_cast<void*>(sent[0].data()));
                        out["sun"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D5244),12);
                    }
                    else if(nm=="sub_710CC0") {
                        PIm3d(P); transformResult=int(r.pct(70));
                        for(int k=0;k<0x14008;++k)At<uint8>(reinterpret_cast<void*>(0xC4B950),k)=uint8(r.next());
                        const int32 idx=r.pct(40)?0:r.pct(30)?int32(r.below(4096)):int32(r.pick({0.f,4090.f,4095.f,4000.f,1.f})), vtx=r.pct(40)?0:r.pct(30)?int32(r.below(2048)):int32(r.pick({0.f,2040.f,2047.f,2000.f,1.f}));
                        AtA<int32>(0xC4B954)=idx; AtA<int32>(0xC4B950)=vtx;
                        reinterpret_cast<void(__cdecl*)(int32,int32)>(a)(int32(r.pick({3.f,6.f,6.f,3.f,12.f})),int32(r.pick({3.f,6.f,6.f,3.f,12.f})));
                        out["counters"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC4B950),8);
                    }
                    else if(nm=="Render") {
                        PIm3d(P); transformResult=int(r.pct(70));
                        for(int k=0;k<0x14008;++k)At<uint8>(reinterpret_cast<void*>(0xC4B950),k)=uint8(r.next());
                        for(auto& v:sdata)for(auto& c:v)c=r.uni(-50.f,50.f);
                        for(auto& o:sobj)for(auto& b:o)b=uint8(r.next());
                        const int n=int(r.below(5)); uint32 offs=0;
                        for(int k=0;k<n;++k){ uint8* o=sobj[k].data(); const uint32 cnt=r.pct(30)?uint32(r.below(7)):uint32(r.below(500)); const uint32 used=std::min<uint32>(cnt,uint32(sdata.size())-offs);
                            At<uint32>(o,0xC)=used; At<void*>(o,0x10)=&sdata[offs][0]; Link(o,nullptr,k+1<n?sobj[k+1].data():nullptr); offs+=used; if(offs>=sdata.size())offs=0; }
                        AtA<void*>(0xC6A16C)=n?sobj[0].data():nullptr;
                        const CRGBA c{uint8(r.next()),uint8(r.next()),uint8(r.next()),uint8(r.next())};
                        ret=reinterpret_cast<bool(__cdecl*)(const CRGBA*)>(a)(&c);
                        out["buf"]=Fnv(reinterpret_cast<void*>(0xC4B950),0x14008); out["counters"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC4B950),8);
                    }
                    else if(nm=="RenderStencilShadows") {
                        PIm3d(P); transformResult=int(r.pct(70));
                        P.Add(0x727B60,S_DrawRect); P.Add(Addr(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)),S_DrawRect);
                        P.Add(0x727350,S_InitPerFrame); P.Add(Addr(&CSprite2d::InitPerFrame),S_InitPerFrame);
                        AtA<uint32>(0xB7C4E8)=r.next(); if(r.pct(40))AtA<uint32>(0xB7C4E8)=r.below(256);
                        for(auto& v:sdata)for(auto& c:v)c=r.uni(-50.f,50.f); for(auto& o:sobj)for(auto& b:o)b=uint8(r.next());
                        const int n=int(r.below(3)); for(int k=0;k<n;++k){ uint8* o=sobj[k].data(); At<uint32>(o,0xC)=6*r.below(5)+3*r.below(2); At<void*>(o,0x10)=&sdata[0][0]; Link(o,nullptr,k+1<n?sobj[k+1].data():nullptr); }
                        AtA<void*>(0xC6A16C)=n?sobj[0].data():nullptr; AtA<int32>(0xC4B954)=0; AtA<int32>(0xC4B950)=0;
                        ret=reinterpret_cast<bool(__cdecl*)()>(a)();
                    }
                    else if(nm=="Process") {
                        PMem(P);
                        P.Add(0x711760,S_Register); P.Add(Addr(&CStencilShadows::RegisterStencilShadows),S_Register);
                        P.Add(0x70FAE0,S_RenderVeh); P.Add(Addr(&CStencilShadows::RenderForVehicle),S_RenderVeh);
                        P.Add(0x710310,S_RenderObj); P.Add(Addr(&CStencilShadows::RenderForObject),S_RenderObj);
                        AtA<uint8>(0xC6A899)=uint8(r.next()); AtA<uint8>(0xC6A898)=uint8(r.below(4));
                        for(auto& o:sobj)for(auto& b:o)b=uint8(r.next());
                        const int n=int(r.below(7)); for(int k=0;k<n;++k){ uint8* o=sobj[k].data(); At<uint8>(o,6)=uint8(r.below(4)); Link(o,nullptr,k+1<n?sobj[k+1].data():nullptr); }
                        AtA<void*>(0xC6A16C)=n?sobj[0].data():nullptr;
                        alignas(16) std::array<float,3> cam{r.uni(-100.f,100.f),r.uni(-100.f,100.f),r.uni(0.f,50.f)};
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(cam.data());
                        out["counters"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC6A898),2); out["bufs"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC6A170),12);
                    }
                    else if(nm=="RenderForVehicle"||nm=="RenderForObject") {
                        const bool veh=nm=="RenderForVehicle";
                        P.Add(0x535300,S_ColModelS); P.Add(Addr(&CEntity::GetColModel),S_ColModelS);
                        P.Add(0x54F560,S_AllocMatrix); P.Add(Addr(&CPlaceable::AllocateMatrix),S_AllocMatrix);
                        P.Add(0x54F1B0,S_UpdMatrix); P.Add(Addr(&CSimpleTransform::UpdateMatrix),S_UpdMatrix);
                        P.Add(0x6B7150,S_CalcLean); P.Add(Addr(&CBike::CalculateLeanMatrix),S_CalcLean);
                        for(auto& e:sent)for(auto& b:e)b=uint8(r.next()); for(auto& c:scol)for(auto& b:c)b=uint8(r.next());
                        for(int k=0;k<3;++k)for(auto& b:sdat[k])b=uint8(r.next());
                        // entidad 0: vehiculo (tipo 2) u objeto (tipo 4)
                        uint8* e=sent[0].data(); At<uint8>(e,0x36)=uint8((At<uint8>(e,0x36)&~7u)|(veh?2u:4u));
                        // la mitad de las veces la entidad tiene matriz propia
                        FillMatrix(smat,r); FillMatrix(smat2,r); At<void*>(e,0x14)=r.pct(70)?smat.data():nullptr;
                        // vehiculo: tipo/subtipo en +0x590/+0x594 (9 = moto con su matriz de inclinacion en +0x5CC)
                        alignas(16) static std::array<uint8,0x680> vbuf{}; (void)vbuf;
                        const int nv=int(r.pct(10)?0:1+r.below(30)); const int nt=int(r.pct(10)?0:1+r.below(14));
                        for(int k=0;k<nv;++k)for(int c=0;c<3;++c)At<int16>(sverts[0].data(),6*k+2*c)=int16(int(r.below(1025))-512);
                        for(int k=0;k<nt;++k){ for(int c=0;c<3;++c)At<uint16>(stris[0].data(),8*k+2*c)=uint16(r.below(uint32(std::max(nv,1)))); At<uint16>(stris[0].data(),8*k+6)=uint16(r.next()); }
                        uint8* cd=sdat[0].data(); At<uint32>(cd,0x20)=uint32(nt); At<uint32>(cd,0x24)=uint32(nv); At<void*>(cd,0x28)=sverts[0].data(); At<void*>(cd,0x2C)=stris[0].data();
                        At<void*>(scol[0].data(),0x2C)=cd;
                        if(r.pct(8))At<void*>(scol[0].data(),0x2C)=nullptr;
                        // objeto de sombra: capacidad variable (desbordamiento) y caras = nt (o distinto)
                        static std::array<CVector,1024> out3{}; for(auto& v:out3)v=CVector{0.f,0.f,0.f};
                        uint8* so=sobj[0].data(); for(auto& b:sobj[0])b=uint8(r.next());
                        At<void*>(so,0)=e; At<uint16>(so,4)=uint16(r.pct(10)?nt+1:nt); At<uint32>(so,8)=uint32(r.pct(30)?r.below(60):nt*15); At<uint32>(so,0xC)=uint32(r.below(9)); At<void*>(so,0x10)=out3.data();
                        for(int k=0;k<3;++k)AtA<float>(0x8D5244+4*k)=r.pct(10)?0.f:r.uni(-3.f,3.f);
                        for(auto& p:ptsObj)p=CVector{1.f,2.f,3.f}; for(auto& p:ptsWorld)p=CVector{4.f,5.f,6.f}; for(auto& b:edgeBuf)b=uint8(r.next());
                        AtA<void*>(0xC6A170)=edgeBuf.data(); AtA<void*>(0xC6A174)=ptsObj.data(); AtA<void*>(0xC6A178)=ptsWorld.data();
                        // el vehiculo necesita tipo/subtipo; se usa un bufer propio con tipo 2 y los campos de +0x590
                        alignas(16) static std::array<uint8,0x680> vh{}; for(auto& b:vh)b=uint8(r.next());
                        if(veh){ sVehBuf=vh.data(); std::memcpy(vh.data(),e,0x60); At<uint32>(vh.data(),0x590)=r.pct(40)?9u:r.below(9); At<uint32>(vh.data(),0x594)=uint32(r.pick({3.f,4.f,0.f,5.f,2.f})); At<void*>(so,0)=vh.data(); }
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(so);
                        out["used"]=At<uint32>(so,0xC); out["data"]=ReversingProbe::Bytes(out3.data(),sizeof(CVector)*std::min<size_t>(out3.size(),size_t(At<uint32>(so,8))+4));
                        out["edges"]=Fnv(edgeBuf.data(),edgeBuf.size()); out["pts"]=Fnv(ptsObj.data(),sizeof(ptsObj)); out["world"]=Fnv(ptsWorld.data(),sizeof(ptsWorld));
                        out["ent"]=Obj(sent[0]); out["vh"]=veh ? json(Fnv(vh.data(),vh.size())) : json(nullptr);
                    }
                    
                    fpSet=savedSet;
                }
                out["ret"]=ret; out["trace"]=trace; return out;
            };
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            const auto original=run(false), candidate=run(true);
            RestoreRegions();
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ return r["ret"].is_null() ? json(std::format("{:016x}",Fnv(r.dump().data(),r.dump().size()))) : r["ret"]; };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    fpSet=savedSet; RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()}});
}
}
