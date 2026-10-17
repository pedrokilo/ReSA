#pragma once
// Banco dentro del motor: sostener objeto y jetpack (lista de 14, 27 funciones).
// CTaskSimpleHoldEntity (constructores 1/2/3, Clone, MakeAbortable, ProcessPed, SetPedPosition, ReleaseEntity,
// CanThrowEntity, PlayAnim, FinishAnimHoldEntityCB, StartAnim, DropEntity, ChoosePutDownHeight) y CTaskSimpleJetPack
// (constructor, destructor, Clone, GetTaskType, MakeAbortable, ProcessPed, RenderJetPack, ProcessThrust, ProcessAnims,
// ProcessControlInput, ApplyRollAndPitch, DoJetPackEffect, StopJetPackEffect).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 2 peatones (el 0 con inteligencia, datos de jugador, clump, huesos falsos y matriz; el 1 hace de entidad no objeto y de
// objetivo fijado), un CObject con vtable de trampas (SetIsStatic y TestCollision trazados), tareas falsas con vtable trazada
// (la tarea probada de HoldEntity lleva una vtable falsa para elegir el tipo HOLD/PICKUP/PUTDOWN/otro), asociaciones de
// animacion, jerarquias, matrices de huesos, frames, FX, modelo y evento falsos.
// Simulados con traza: RNG, BlendAnimation (dos versiones), GetAnimationBlock(nombre), GetAnimation, GetAnimBlockName,
// AddAnimBlockRef, RequestModel, ProcessVerticalLine, Add/RemoveFromMovingList, GetColModel, CEventGroup::Add (eventos
// serializados por campos), GetPadFromPlayer y los metodos del pad, GetAnimHierarchyFromSkinClump, RpHAnimIDGetIndex,
// RpHAnimHierarchyGetMatrixArray, RwV3dTransformPoints, RwMatrixTranslate/Rotate, RtQuatRotate, RwFrameGetLTM,
// RwFrameUpdateObjects, RpClumpRender, GetFrameFromName, GetClumpModelInfo, Add/RemoveRef del modelo, GetFirstAtomic,
// RpSkinGeometryGetSkin, RpClumpForAllAtomics, RpClumpDestroy, FX (CreateFxSystem, Play, Kill, SetLocalParticles,
// CopyParentMatrix, SetMatrix, SetOffsetPos, SetConstTime), audio del jetpack, armas al subir/bajar, SetRealMoveAnim,
// ApplyMoveForce (traza y efecto determinista), CalculateGroundHeight, GetLookDirection, GetWeaponSkill,
// ProcessPlayerWeapon y DropJetPack (validada aparte; crea pickups reales).
// Globales tocados (se guardan y restauran en cada ejecucion y al final): paso y reloj, nNoTempObjects, raton en tercera
// persona, camara activa, frentes de las camaras 0..2, orientacion de la camara, puntero ms_aAnimAssocGroups (se cambia por
// una tabla falsa), ms_aAnimBlocks, ms_modelInfoPtrs[370] y ms_aInfoForModel[370].
// Pool de tareas: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada ejecucion; lo nuevo se serializa y se
// libera sin destructor.
// Fuera del banco: m_ThrustFwd = -1 en DoJetPackEffect/RenderJetPack (FIX_BUGS: el DLL solo cuenta 1 como empuje para la
// llama; el EXE cualquier valor distinto de 0).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/TaskTypes/TaskSimpleJetPack.h"
#include "Tasks/TaskTypes/TaskSimplePlayerOnFoot.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Events/EventGroup.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventScriptCommand.h"
#include "Streaming.h"
#include "World.h"
#include "Camera.h"
#include "Pad.h"
#include "VisibilityPlugins.h"
#include "Models/ClumpModelInfo.h"
#include "Models/ModelInfo.h"
#include "RwHelper.h"
#include "WeaponInfo.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks14bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline json Vec(const float* v) { return json::array({F(v[0]),F(v[1]),F(v[2])}); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class T=uint32> T& G(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_HEADING=0x10, E_MATRIX=0x14, E_RWOBJ=0x18, E_FLAGS=0x1C, E_TYPE=0x36;          // CEntity
constexpr size_t PH_FLAGS=0x40, PH_SPEED=0x44, PH_MASS=0x8C;                                                  // CPhysical
constexpr size_t O_FAKEPHYS=0xB8, O_IGNORED=0x128, O_OBJTYPE=0x13C, O_OBJFLAGS=0x140, O_REMOVAL=0x150;       // CObject
constexpr size_t P_FLAGS=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_PDATA=0x480, P_BONES=0x488,
                 P_ANIMGROUP=0x4D4, P_SHIFT=0x4D8, P_MOVESTATE=0x534, P_HEALTH=0x540, P_CURROT=0x558, P_AIMROT=0x55C,
                 P_PEDTYPE=0x598, P_WEAPONS=0x5A0, P_ACTIVESLOT=0x718, P_TARGET=0x71C;                         // CPed
constexpr size_t I_PRIMARY_DEFAULT=0x14, I_SEC_PARTIAL=0x28;                                                  // CPedIntelligence
constexpr size_t PD_BLEND=0x14;                                                                               // CPlayerPedData
constexpr size_t AS_AMOUNT=0x18, AS_DELTA=0x1C, AS_TIME=0x20;                                                 // CAnimBlendAssociation
constexpr uintptr_t TIMESTEP=0xB7CB5C, TIMEMS=0xB7CB84, NOTEMP=0xBB4A70, MOUSE3RD=0xB6EC2E, ACTIVECAM=0xB6F081,
                    CAMFRONT0=0xB6F32C, CAMSTRIDE=0x238, CAMORIENT=0xB6F178, ASSOCGROUPS=0xB4EA34, MI_JETPACK=0xA9B690,
                    SI_JETPACK=0x8E69A8, FXMAN=0xA9AE80, CAMERA=0xB6F028;
constexpr uintptr_t VT_HOLD=0x870B2C, VT_JET=0x8705C4;
constexpr uintptr_t CB_HOLD_EXE=0x691740, CB_DEFAULT_EXE=0x4D1390, CB_REMOVESKIN_EXE=0x734B90;
constexpr int NBONES=19;
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_TYPE=0x68, FK_CNT=0x70;
constexpr size_t EV_TYPE=0x30, EV_PRIO=0x34, MI_INST=0x30;
constexpr int OTHER_TYPE=TASK_SIMPLE_STAND_STILL;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,2> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,3> mats{};   // 0 peaton 0, 1 peaton 1, 2 objeto
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdata{};
alignas(16) inline std::array<uint8,sizeof(CObject)> obj{};
alignas(16) inline std::array<uint8,0x100> object{};                              // tarea probada
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,2> fakes{};             // 0 secundaria (sostener), 1 primaria por defecto
alignas(16) inline std::array<std::array<uint8,0x40>,2> assocs{};
alignas(16) inline std::array<uint8,0x40> clump{}, jpClump{}, hier{}, hierAnim{}, colModel{}, mi{}, atomic{}, event{}, vecBuf{};
alignas(16) inline std::array<uint8,0x60> frame{}, jpFrame{};
alignas(16) inline std::array<std::array<uint8,0x60>,2> jbFrames{};
alignas(16) inline std::array<uint8,NBONES*0x40> boneMats{};
alignas(16) inline std::array<std::array<uint8,0x20>,NBONES> bones{};
alignas(16) inline std::array<std::array<uint8,0x20>,2> keyFrames{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> fxs{};                    // 0/1 ya creados, 2/3 los que devuelve CreateFxSystem
alignas(16) inline std::array<uint8,0x200> padBuf{};
alignas(16) inline std::array<uint8,0x14*128> groups{};
alignas(16) inline std::array<char,64> names{};
inline void* Ped(int n) { return peds[n].data(); }
// CAnimManager::ms_aAnimBlocks es privado: se lee por su dirección del EXE (0xB5D4A0, 180 bloques de 0x20)
constexpr uintptr_t ANIMBLOCKS=0xB5D4A0; constexpr size_t ANIMBLOCKS_SIZE=size_t(NUM_ANIM_BLOCKS)*0x20;
inline CAnimBlock* Block(int k) { return reinterpret_cast<CAnimBlock*>(ANIMBLOCKS)+k; }

// pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
constexpr size_t TASK_SLOT=0x80;
inline uint8* TaskHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* Storage(uint8* h) { return *reinterpret_cast<uint8**>(h); }
inline uint8* States(uint8* h) { return *reinterpret_cast<uint8**>(h+4); }
inline int32 Cap(uint8* h) { return *reinterpret_cast<int32*>(h+8); }
inline int32& Last(uint8* h) { return At<int32>(h,0xC); }
inline std::vector<bool> Occ(uint8* h) { std::vector<bool> r(size_t(Cap(h))); for(size_t k=0;k<r.size();++k)r[k]=!(States(h)[k]&0x80); return r; }
inline void ResetPool(uint8* h,size_t slot,int32 last,const std::vector<uint8>& statesStart,const std::vector<bool>& occStart) {
    Last(h)=last;
    if(Occ(h)==occStart)std::memcpy(States(h),statesStart.data(),statesStart.size());
    for(int32 k=0;k<Cap(h);++k)if(States(h)[k]&0x80)std::memset(Storage(h)+k*slot,0xCD,slot);
}
inline void FreeFresh(uint8* h,const std::vector<bool>& before) { for(size_t k=0;k<before.size();++k)if(!before[k]&&!(States(h)[k]&0x80))States(h)[k]|=0x80; }

inline std::array<void*,16> taskVtbl{};
inline std::array<void*,64> objVtbl{};
inline std::array<void*,20> eventVtbl{};
inline std::array<void*,24> miVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline uintptr_t dllHoldVt{}, dllJetVt{}, globalGroup{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(taskVtbl.data()))return "task_vt"; if(v==uintptr_t(objVtbl.data()))return "obj_vt";
    if(v==uintptr_t(eventVtbl.data()))return "event_vt"; if(v==uintptr_t(miVtbl.data()))return "mi_vt";
    if(v==VT_HOLD||v==dllHoldVt)return "hold_vt"; if(v==VT_JET||v==dllJetVt)return "jetpack_vt";
    if(v==CB_HOLD_EXE||v==std::bit_cast<uintptr_t>(&CTaskSimpleHoldEntity::FinishAnimHoldEntityCB))return "cb_finish_hold";
    if(v==CB_DEFAULT_EXE||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "cb_default";
    if(v==CB_REMOVESKIN_EXE||v==std::bit_cast<uintptr_t>(&AtomicRemoveAnimFromSkinCB))return "cb_remove_anim_from_skin";
    if(v==FXMAN)return "fx_manager"; if(v==CAMERA)return "camera"; if(v==globalGroup)return "global_events";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<2;++n)if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n);
    for(int n=0;n<3;++n)if(auto s=in(mats[n].data(),mats[n].size(),"mat"))return *s+"#"+std::to_string(n);
    if(auto s=in(intel.data(),intel.size(),"intel"))return *s; if(auto s=in(pdata.data(),pdata.size(),"pdata"))return *s;
    if(auto s=in(obj.data(),obj.size(),"obj"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    for(int n=0;n<2;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<2;++n)if(auto s=in(assocs[n].data(),assocs[n].size(),"assoc"))return *s+"#"+std::to_string(n);
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(jpClump.data(),jpClump.size(),"jpclump"))return *s;
    if(auto s=in(hier.data(),hier.size(),"hanim"))return *s; if(auto s=in(hierAnim.data(),hierAnim.size(),"anim_hier"))return *s;
    if(auto s=in(colModel.data(),colModel.size(),"colmodel"))return *s; if(auto s=in(mi.data(),mi.size(),"mi"))return *s;
    if(auto s=in(atomic.data(),atomic.size(),"atomic"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(vecBuf.data(),vecBuf.size(),"vec"))return *s; if(auto s=in(frame.data(),frame.size(),"frame"))return *s;
    if(auto s=in(jpFrame.data(),jpFrame.size(),"jpframe"))return *s;
    for(int n=0;n<2;++n)if(auto s=in(jbFrames[n].data(),jbFrames[n].size(),"jbframe"))return *s+"#"+std::to_string(n);
    if(auto s=in(boneMats.data(),boneMats.size(),"bonemats"))return *s;
    for(int n=0;n<NBONES;++n)if(auto s=in(bones[n].data(),bones[n].size(),"bone"))return *s+"#"+std::to_string(n);
    for(int n=0;n<2;++n)if(auto s=in(keyFrames[n].data(),keyFrames[n].size(),"keyframe"))return *s+"#"+std::to_string(n);
    for(int n=0;n<4;++n)if(auto s=in(fxs[n].data(),fxs[n].size(),"fx"))return *s+"#"+std::to_string(n);
    if(auto s=in(padBuf.data(),padBuf.size(),"pad"))return *s; if(auto s=in(groups.data(),groups.size(),"groups"))return *s;
    if(auto s=in(names.data(),names.size(),"names"))return *s;
    { const auto b=ANIMBLOCKS; if(v>=b&&v<b+ANIMBLOCKS_SIZE)return "ablock#"+std::to_string((v-b)/0x20)+"+"+std::to_string((v-b)%0x20); }
    { const auto s=uintptr_t(Storage(TaskHdr())); if(v>=s&&v<s+size_t(Cap(TaskHdr()))*TASK_SLOT)return "pool_task+"+std::to_string((v-s)%TASK_SLOT); }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    // locales de la pila (vectores y puntos de colision temporales): no comparables
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack"; }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { return json::array({RefList(Ped(0)),RefList(Ped(1)),RefList(obj.data())}); }
inline int32 TypeOf(const void* t) { return reinterpret_cast<int32(__thiscall*)(const void*)>((*reinterpret_cast<void* const* const*>(t))[4])(t); }
inline json SerTask(const void* p) { return {{"type",TypeOf(p)},{"words",Region(p,TASK_SLOT,true)}}; }
// eventos reales de la pila: tipo por la vtable y solo los campos propios (sin relleno)
inline json SerEvent(const CEvent* e) {
    if(!e)return nullptr;
    const int type=const_cast<CEvent*>(e)->GetEventType();
    json r={type,At<int32>(e,4),At<uint8>(e,8)};
    if(type==EVENT_SOUND_QUIET){ auto* s=static_cast<const CEventSoundQuiet*>(e); r.push_back(Name(s->m_entity)); r.push_back(F(s->m_fLocalSoundLevel)); r.push_back(s->m_startTimeInMs); r.push_back(Vec(&s->m_position.x)); }
    else if(type==EVENT_SCRIPT_COMMAND){ r.push_back(At<int32>(e,0xC)); r.push_back(At<const void*>(e,0x10) ? SerTask(At<const void*>(e,0x10)) : json(nullptr)); r.push_back(At<uint8>(e,0x14)); }
    return r;
}

// buffers de fixtures cuyos cambios se comparan (palabra a palabra, traducidas); la vptr de la tarea probada no (EXE/DLL)
struct Buf { std::string name; uint8* p; size_t n; };
inline std::vector<Buf> Buffers() {
    std::vector<Buf> b;
    for(int n=0;n<2;++n)b.push_back({"ped#"+std::to_string(n),peds[n].data(),peds[n].size()});
    for(int n=0;n<3;++n)b.push_back({"mat#"+std::to_string(n),mats[n].data(),mats[n].size()});
    b.push_back({"intel",intel.data(),intel.size()}); b.push_back({"pdata",pdata.data(),pdata.size()});
    b.push_back({"obj",obj.data(),obj.size()}); b.push_back({"object",object.data(),object.size()});
    for(int n=0;n<2;++n)b.push_back({"fk#"+std::to_string(n),fakes[n].data(),fakes[n].size()});
    for(int n=0;n<2;++n)b.push_back({"assoc#"+std::to_string(n),assocs[n].data(),assocs[n].size()});
    b.push_back({"clump",clump.data(),clump.size()}); b.push_back({"jpclump",jpClump.data(),jpClump.size()});
    b.push_back({"frame",frame.data(),frame.size()}); b.push_back({"jpframe",jpFrame.data(),jpFrame.size()});
    for(int n=0;n<2;++n)b.push_back({"jbframe#"+std::to_string(n),jbFrames[n].data(),jbFrames[n].size()});
    b.push_back({"bonemats",boneMats.data(),boneMats.size()});
    for(int n=0;n<NBONES;++n)b.push_back({"bone#"+std::to_string(n),bones[n].data(),bones[n].size()});
    for(int n=0;n<2;++n)b.push_back({"keyframe#"+std::to_string(n),keyFrames[n].data(),keyFrames[n].size()});
    for(int n=0;n<4;++n)b.push_back({"fx#"+std::to_string(n),fxs[n].data(),fxs[n].size()});
    b.push_back({"mi",mi.data(),mi.size()}); b.push_back({"atomic",atomic.data(),atomic.size()}); b.push_back({"event",event.data(),event.size()});
    b.push_back({"colmodel",colModel.data(),colModel.size()}); b.push_back({"vec",vecBuf.data(),vecBuf.size()}); b.push_back({"pad",padBuf.data(),padBuf.size()});
    return b;
}
inline std::vector<std::vector<uint8>> Snapshot() { std::vector<std::vector<uint8>> s; for(auto& b:Buffers())s.push_back(ReversingProbe::Bytes(b.p,b.n)); return s; }
inline json Changes(const std::vector<std::vector<uint8>>& before) {
    json r=json::array(); const auto bufs=Buffers();
    for(size_t k=0;k<bufs.size();++k)for(size_t w=0;w+4<=bufs[k].n;w+=4){
        if(bufs[k].p==object.data()&&w==0)continue;
        if(std::memcmp(bufs[k].p+w,before[k].data()+w,4))r.push_back({bufs[k].name,w,Name(At<uint32>(bufs[k].p,w))});
    }
    return r;
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint32 mTestCol{}, cTestCol{}, mExitJust{}, cExitJust{}, mFx{}, cFx{}, mFrame{}, cFrame{}, fxNext{};
inline bool pvlHit{}, padExit{}, padTarget{}, padLookBehind{}, padLookLeft{}, padLookRight{};
inline int16 padUD{}, padLR{}, padBrake{}, padAccel{};
inline float pvlZ{}, groundZ{}; inline int32 lookDir{}, weaponSkill{};
inline uintptr_t blockByName{}, instRet{}, atomicRet{}, skinRet{}, assocRet{};
inline bool Bit(uint32 mask,uint32& calls) { return (mask>>(calls++&31))&1; }

// RNG
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// tareas falsas: GetTaskType trazado; MakeAbortable segun una mascara por llamada (contador en el propio buffer)
inline void* __fastcall FDtor(void* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline int __fastcall FType(void* t,void*) { trace.push_back({"type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(void* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// objeto: SetIsStatic (hueco 4, emulado) y TestCollision (hueco 13)
inline void __fastcall OSetStatic(void* o,void*,uint32 b) { trace.push_back({"set_is_static",Name(o),b&0xFF}); auto& f=At<uint32>(o,E_FLAGS); f=(f&~4u)|((b&0xFF) ? 4u:0u); }
inline bool __fastcall OTestCol(void* o,void*,uint32 b) { const bool r=Bit(mTestCol,cTestCol); trace.push_back({"test_collision",Name(o),b&0xFF,r}); return r; }
// evento falso: tipo y prioridad
inline int32 __fastcall EType(void* e,void*) { trace.push_back({"event_type",Name(e)}); return At<int32>(e,EV_TYPE); }
inline int32 __fastcall EPrio(void* e,void*) { trace.push_back({"event_priority",Name(e)}); return At<int32>(e,EV_PRIO); }
// modelo falso del jetpack
inline void* __fastcall MICreate(void* m,void*) { trace.push_back({"create_instance",Name(m)}); return reinterpret_cast<void*>(instRet); }
inline void __fastcall MIAddRef(void* m,void*) { trace.push_back({"mi_add_ref",Name(m)}); }
inline void __fastcall MIRemoveRef(void* m,void*) { trace.push_back({"mi_remove_ref",Name(m)}); }
// animacion y streaming
inline void __cdecl FakeAddBlockRef(int32 i) { trace.push_back({"add_anim_block_ref",i}); }
inline void __cdecl FakeRequestModel(int32 m,int32 f) { trace.push_back({"request_model",m,f}); }
inline CAnimBlock* __cdecl FakeBlockByName(const char* n) { trace.push_back({"get_anim_block",std::string(n)}); return reinterpret_cast<CAnimBlock*>(blockByName); }
inline CAnimBlendHierarchy* __cdecl FakeGetAnimation(const char* n,const CAnimBlock* b) { trace.push_back({"get_animation",std::string(n),Name(b)}); return reinterpret_cast<CAnimBlendHierarchy*>(hierAnim.data()); }
inline const char* __cdecl FakeBlockName(int32 g) { trace.push_back({"get_anim_block_name",g}); return names.data()+32; }
inline CAnimBlendAssociation* __cdecl FakeBlendHier(RpClump* c,CAnimBlendHierarchy* h,int32 fl,float b) { trace.push_back({"blend_hier",Name(c),Name(h),fl,F(b)}); return reinterpret_cast<CAnimBlendAssociation*>(assocRet); }
inline CAnimBlendAssociation* __cdecl FakeBlendGroup(RpClump* c,int32 g,int32 id,float b) { trace.push_back({"blend_group",Name(c),g,id,F(b)}); return reinterpret_cast<CAnimBlendAssociation*>(assocRet); }
// mundo, fisica y eventos
inline bool __cdecl FakePVL(const CVector* o,float d,CColPoint* cp,CEntity** e,uint32 b1,uint32 b2,uint32 b3,uint32 b4,uint32 b5,uint32 b6,void* poly) {
    trace.push_back({"process_vertical_line",Name(o),Vec(&o->x),F(d),Name(cp),Name(e),b1&0xFF,b2&0xFF,b3&0xFF,b4&0xFF,b5&0xFF,b6&0xFF,Name(poly),pvlHit});
    if(pvlHit){ cp->m_vecPoint=CVector{o->x,o->y,pvlZ}; *e=nullptr; }
    return pvlHit;
}
inline void __fastcall FakeAddMoving(void* p,void*) { trace.push_back({"add_to_moving_list",Name(p)}); }
inline void __fastcall FakeRemoveMoving(void* p,void*) { trace.push_back({"remove_from_moving_list",Name(p)}); }
inline CColModel* __fastcall FakeGetColModel(void* e,void*) { trace.push_back({"get_col_model",Name(e)}); return reinterpret_cast<CColModel*>(colModel.data()); }
inline CEvent* __fastcall FakeEventAdd(void* g,void*,CEvent* e,uint32 f) { trace.push_back({"event_add",Name(g),Name(e),SerEvent(e),f&0xFF}); return nullptr; }
inline void __fastcall FakeApplyMoveForce(void* p,void*,CVector f) {
    trace.push_back({"apply_move_force",Name(p),Vec(&f.x)});
    auto& v=At<CVector>(p,PH_SPEED); v.x+=f.x*0.001f; v.y+=f.y*0.001f; v.z+=f.z*0.001f;
}
// pad
inline CPad* __fastcall FakeGetPad(void* p,void*) { trace.push_back({"get_pad",Name(p)}); return reinterpret_cast<CPad*>(padBuf.data()); }
inline bool __fastcall PadExitJust(void* p,void*) { const bool r=Bit(mExitJust,cExitJust); trace.push_back({"exit_vehicle_just_down",Name(p),r}); return r; }
inline bool __fastcall PadExit(void* p,void*) { trace.push_back({"get_exit_vehicle",Name(p),padExit}); return padExit; }
inline int16 __fastcall PadUD(void* p,void*) { trace.push_back({"walk_up_down",Name(p),padUD}); return padUD; }
inline int16 __fastcall PadLR(void* p,void*) { trace.push_back({"walk_left_right",Name(p),padLR}); return padLR; }
inline bool __fastcall PadTarget(void* p,void*) { trace.push_back({"get_target",Name(p),padTarget}); return padTarget; }
inline bool __fastcall PadLookBehind(void* p,void*) { trace.push_back({"look_behind",Name(p),padLookBehind}); return padLookBehind; }
inline bool __fastcall PadLookLeft(void* p,void*) { trace.push_back({"look_left",Name(p),padLookLeft}); return padLookLeft; }
inline bool __fastcall PadLookRight(void* p,void*) { trace.push_back({"look_right",Name(p),padLookRight}); return padLookRight; }
inline int16 __fastcall PadBrake(void* p,void*) { trace.push_back({"get_brake",Name(p),padBrake}); return padBrake; }
inline int16 __fastcall PadAccel(void* p,void*) { trace.push_back({"get_accelerate",Name(p),padAccel}); return padAccel; }
// RenderWare
inline RpHAnimHierarchy* __cdecl FakeGetHier(RpClump* c) { trace.push_back({"get_anim_hierarchy",Name(c)}); return reinterpret_cast<RpHAnimHierarchy*>(hier.data()); }
inline RwMatrix* __cdecl FakeGetMatArray(RpHAnimHierarchy* h) { trace.push_back({"get_matrix_array",Name(h)}); return reinterpret_cast<RwMatrix*>(boneMats.data()); }
inline int32 __cdecl FakeIDGetIndex(RpHAnimHierarchy* h,int32 id) { const int32 r=int32((uint32(id)*7u+3u)%uint32(NBONES)); trace.push_back({"id_get_index",Name(h),id,r}); return r; }
inline RwV3d* __cdecl FakeTransformPoints(RwV3d* out,const RwV3d* in,int32 n,const RwMatrix* m) {
    trace.push_back({"transform_points",Name(out),Name(in),Vec(&in->x),n,Name(m)});
    const RwV3d v=*in; out->x=v.x+m->pos.x; out->y=v.y+m->pos.y; out->z=v.z+m->pos.z; return out;
}
inline RwMatrix* __cdecl FakeMatTranslate(RwMatrix* m,const RwV3d* t,int32 op) { trace.push_back({"matrix_translate",Name(m),Vec(&t->x),op}); return m; }
inline RwMatrix* __cdecl FakeMatRotate(RwMatrix* m,const RwV3d* a,float ang,int32 op) { trace.push_back({"matrix_rotate",Name(m),Vec(&a->x),F(ang),op}); return m; }
inline RtQuat* __cdecl FakeQuatRotate(RtQuat* q,const RwV3d* a,float ang,int32 op) { trace.push_back({"quat_rotate",Name(q),Vec(&a->x),F(ang),op}); return q; }
inline RwFrame* __cdecl FakeFrameUpdate(RwFrame* f) { trace.push_back({"frame_update_objects",Name(f)}); return f; }
inline RpClump* __cdecl FakeClumpRender(RpClump* c) { trace.push_back({"clump_render",Name(c)}); return c; }
inline RwMatrix* __cdecl FakeGetLTM(RwFrame* f) { trace.push_back({"frame_get_ltm",Name(f)}); return f ? reinterpret_cast<RwMatrix*>(reinterpret_cast<uint8*>(f)+0x10) : nullptr; }
inline RwFrame* __cdecl FakeFrameFromName(RpClump* c,const char* n) {
    const bool ok=Bit(mFrame,cFrame); const std::string s(n); trace.push_back({"frame_from_name",Name(c),s,ok});
    return ok ? reinterpret_cast<RwFrame*>(jbFrames[s=="jetball2" ? 1:0].data()) : nullptr;
}
inline CClumpModelInfo* __cdecl FakeClumpMI(RpClump* c) { trace.push_back({"get_clump_model_info",Name(c)}); return reinterpret_cast<CClumpModelInfo*>(mi.data()); }
inline RpAtomic* __cdecl FakeFirstAtomic(RpClump* c) { trace.push_back({"get_first_atomic",Name(c)}); return reinterpret_cast<RpAtomic*>(atomicRet); }
inline RpSkin* __cdecl FakeGetSkin(RpGeometry* g) { trace.push_back({"get_skin",Name(g)}); return reinterpret_cast<RpSkin*>(skinRet); }
inline RpClump* __cdecl FakeForAllAtomics(RpClump* c,void* cb,void* d) { trace.push_back({"for_all_atomics",Name(c),Name(cb),Name(d)}); return c; }
inline int32 __cdecl FakeClumpDestroy(RpClump* c) { trace.push_back({"clump_destroy",Name(c)}); return 1; }
// FX
inline FxSystem_c* __fastcall FakeCreateFx(void* m,void*,const char* n,const CVector* p,RwMatrix* mat,uint32 b) {
    const bool ok=Bit(mFx,cFx); auto* r=ok&&fxNext<2 ? reinterpret_cast<FxSystem_c*>(fxs[2+fxNext++].data()) : nullptr;
    trace.push_back({"create_fx",Name(m),std::string(n),Name(p),Vec(&p->x),Name(mat),b&0xFF,Name(r)}); return r;
}
inline void __fastcall FxPlay(void* f,void*) { trace.push_back({"fx_play",Name(f)}); }
inline void __fastcall FxKill(void* f,void*) { trace.push_back({"fx_kill",Name(f)}); }
inline void __fastcall FxCopyParent(void* f,void*) { trace.push_back({"fx_copy_parent_matrix",Name(f)}); }
inline void __fastcall FxLocal(void* f,void*,uint32 b) { trace.push_back({"fx_local_particles",Name(f),b&0xFF}); }
inline void __fastcall FxSetMatrix(void* f,void*,RwMatrix* m) { trace.push_back({"fx_set_matrix",Name(f),Name(m)}); }
inline void __fastcall FxOffset(void* f,void*,const CVector* v) { trace.push_back({"fx_offset_pos",Name(f),Vec(&v->x)}); }
inline void __fastcall FxConstTime(void* f,void*,uint32 on,float t) { trace.push_back({"fx_const_time",Name(f),on&0xFF,F(t)}); }
// audio, armas, animacion del jugador, camara y tareas del jugador
inline void __fastcall AeOn(void* a,void*) { trace.push_back({"jetpack_on",Name(a)}); }
inline void __fastcall AeOff(void* a,void*) { trace.push_back({"jetpack_off",Name(a)}); }
inline void __fastcall AeUpdate(void* a,void*,float f,float ang) { trace.push_back({"jetpack_update",Name(a),F(f),F(ang)}); }
inline void __fastcall FakeReplaceWeapon(void* p,void*) { trace.push_back({"replace_weapon",Name(p)}); }
inline void __fastcall FakeRemoveWeapon(void* p,void*,int32 a) { trace.push_back({"remove_weapon",Name(p),a}); }
inline void __fastcall FakeSetRealMoveAnim(void* p,void*) { trace.push_back({"set_real_move_anim",Name(p)}); }
inline float __fastcall FakeGroundHeight(void* c,void*,int32 t) { trace.push_back({"ground_height",Name(c),t}); return groundZ; }
inline int32 __fastcall FakeLookDir(void* c,void*) { trace.push_back({"look_direction",Name(c)}); return lookDir; }
inline int32 __fastcall FakeWeaponSkill(void* p,void*) { trace.push_back({"weapon_skill",Name(p)}); return weaponSkill; }
inline void __fastcall FakePlayerWeapon(void* t,void*,void* p) { trace.push_back({"process_player_weapon",Name(t),Name(p)}); }
inline void __fastcall FakeDropJetPack(void* t,void*,void* p) { trace.push_back({"drop_jetpack",Name(t),Name(p)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=27;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleHoldEntity","CanThrowEntity",0x691700,0,-1,128},
    {"CTaskSimpleHoldEntity","ChoosePutDownHeight",0x693440,0,-1,512},
    {"CTaskSimpleHoldEntity","Clone",0,0x870B2C,1,256},
    {"CTaskSimpleHoldEntity","Constructor-1",0x6913A0,0,-1,256},
    {"CTaskSimpleHoldEntity","Constructor-2",0x691470,0,-1,256},
    {"CTaskSimpleHoldEntity","Constructor-3",0x691550,0,-1,256},
    {"CTaskSimpleHoldEntity","DropEntity",0x6930F0,0,-1,1024},
    {"CTaskSimpleHoldEntity","FinishAnimHoldEntityCB",0x691740,0,-1,256},
    {"CTaskSimpleHoldEntity","MakeAbortable",0,0x870B2C,6,512},
    {"CTaskSimpleHoldEntity","PlayAnim",0x691720,0,-1,128},
    {"CTaskSimpleHoldEntity","ProcessPed",0,0x870B2C,7,1024},
    {"CTaskSimpleHoldEntity","ReleaseEntity",0x6916E0,0,-1,128},
    {"CTaskSimpleHoldEntity","SetPedPosition",0,0x870B2C,8,1024},
    {"CTaskSimpleHoldEntity","StartAnim",0x692FF0,0,-1,512},
    {"CTaskSimpleJetPack","ApplyRollAndPitch",0x67B720,0,-1,256},
    {"CTaskSimpleJetPack","Clone",0,0x8705C4,1,128},
    {"CTaskSimpleJetPack","Constructor",0x67B4E0,0,-1,128},
    {"CTaskSimpleJetPack","Destructor",0x67E600,0,-1,256},
    {"CTaskSimpleJetPack","DoJetPackEffect",0x67B7F0,0,-1,512},
    {"CTaskSimpleJetPack","GetTaskType",0,0x8705C4,4,64},
    {"CTaskSimpleJetPack","MakeAbortable",0,0x8705C4,6,512},
    {"CTaskSimpleJetPack","ProcessAnims",0x67B5C0,0,-1,512},
    {"CTaskSimpleJetPack","ProcessControlInput",0x67E7B0,0,-1,1024},
    {"CTaskSimpleJetPack","ProcessPed",0,0x8705C4,7,1024},
    {"CTaskSimpleJetPack","ProcessThrust",0x67EF20,0,-1,1024},
    {"CTaskSimpleJetPack","RenderJetPack",0x67F6A0,0,-1,512},
    {"CTaskSimpleJetPack","StopJetPackEffect",0x67BA10,0,-1,128},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(Ped(0)); clean(Ped(1)); clean(obj.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }
inline void SetEntityType(void* e,uint8 type) { At<uint8>(e,E_TYPE)=uint8((At<uint8>(e,E_TYPE)&~7u)|type); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks14b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks14b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks14b");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks14b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // vtables del DLL (para nombrar la vptr de las tareas creadas) y grupo global de eventos
    { CTaskSimpleHoldEntity h(nullptr,static_cast<const CVector*>(nullptr),0); dllHoldVt=*reinterpret_cast<const uintptr_t*>(&h); }
    { CTaskSimpleJetPack j{}; dllJetVt=*reinterpret_cast<const uintptr_t*>(&j); }
    globalGroup=uintptr_t(GetEventGlobalGroup());
    FillTraps(taskVtbl,std::make_integer_sequence<int,16>{}); FillTraps(objVtbl,std::make_integer_sequence<int,64>{});
    FillTraps(eventVtbl,std::make_integer_sequence<int,20>{}); FillTraps(miVtbl,std::make_integer_sequence<int,24>{});
    taskVtbl[0]=reinterpret_cast<void*>(&FDtor); taskVtbl[4]=reinterpret_cast<void*>(&FType); taskVtbl[6]=reinterpret_cast<void*>(&FAbort);
    objVtbl[4]=reinterpret_cast<void*>(&OSetStatic); objVtbl[13]=reinterpret_cast<void*>(&OTestCol);
    eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    miVtbl[11]=reinterpret_cast<void*>(&MICreate);
    const auto occTaskStart=Occ(TaskHdr());
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lt0=Last(TaskHdr());
    const std::vector<uint8> stTask(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    auto resetPools=[&]{ ResetPool(TaskHdr(),TASK_SLOT,lt0,stTask,occTaskStart); };
    // estado global tocado: se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMESTEP,4},{TIMEMS,4},{NOTEMP,2},{MOUSE3RD,1},{ACTIVECAM,1},
        {CAMFRONT0,12},{CAMFRONT0+CAMSTRIDE,12},{CAMFRONT0+2*CAMSTRIDE,12},{CAMORIENT,4},{ASSOCGROUPS,4},{MI_JETPACK,4},{SI_JETPACK,0x14},
        {ANIMBLOCKS,ANIMBLOCKS_SIZE}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(Body(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef)),FakeAddBlockRef), a1e(0x4D3FB0,FakeAddBlockRef);
        EntryPatch a2(Body(bit_cast<uintptr_t>(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock))),FakeBlockByName), a2e(0x4D3940,FakeBlockByName);
        EntryPatch a3(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendHierarchy*(*)(const char*,const CAnimBlock*)>(&CAnimManager::GetAnimation))),FakeGetAnimation), a3e(0x4D42F0,FakeGetAnimation);
        EntryPatch a4(Body(bit_cast<uintptr_t>(&CAnimManager::GetAnimBlockName)),FakeBlockName), a4e(0x4D3A30,FakeBlockName);
        EntryPatch a5(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,CAnimBlendHierarchy*,int32,float)>(&CAnimManager::BlendAnimation))),FakeBlendHier), a5e(0x4D4410,FakeBlendHier);
        EntryPatch a6(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))),FakeBlendGroup), a6e(0x4D4610,FakeBlendGroup);
        EntryPatch t1(Body(bit_cast<uintptr_t>(&CStreaming::RequestModel)),FakeRequestModel), t1e(0x4087E0,FakeRequestModel);
        EntryPatch w1(Body(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine)),FakePVL), w1e(0x5674E0,FakePVL);
        EntryPatch m1(Body(MemberAddress(&CPhysical::AddToMovingList)),FakeAddMoving), m1e(0x542800,FakeAddMoving);
        EntryPatch m2(Body(MemberAddress(&CPhysical::RemoveFromMovingList)),FakeRemoveMoving), m2e(0x542860,FakeRemoveMoving);
        EntryPatch m3(Body(MemberAddress(&CEntity::GetColModel)),FakeGetColModel), m3e(0x535300,FakeGetColModel);
        EntryPatch m4(Body(MemberAddress(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce))),FakeApplyMoveForce), m4e(0x5429F0,FakeApplyMoveForce);
        EntryPatch e1(Body(MemberAddress(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FakeEventAdd), e1e(0x4AB420,FakeEventAdd);
        EntryPatch p1(Body(MemberAddress(&CPlayerPed::GetPadFromPlayer)),FakeGetPad), p1e(0x609560,FakeGetPad);
        EntryPatch p2(Body(MemberAddress(&CPad::ExitVehicleJustDown)),PadExitJust), p2e(0x540120,PadExitJust);
        EntryPatch p3(Body(MemberAddress(&CPad::GetExitVehicle)),PadExit), p3e(0x5400D0,PadExit);
        EntryPatch p4(Body(MemberAddress(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkUpDown))),PadUD), p4e(0x53FD30,PadUD);
        EntryPatch p5(Body(MemberAddress(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkLeftRight))),PadLR), p5e(0x53FC90,PadLR);
        EntryPatch p6(Body(MemberAddress(&CPad::GetTarget)),PadTarget), p6e(0x540670,PadTarget);
        EntryPatch p7(Body(MemberAddress(&CPad::GetLookBehindForCar)),PadLookBehind), p7e(0x53FE70,PadLookBehind);
        EntryPatch p8(Body(MemberAddress(&CPad::GetLookLeft)),PadLookLeft), p8e(0x53FDD0,PadLookLeft);
        EntryPatch p9(Body(MemberAddress(&CPad::GetLookRight)),PadLookRight), p9e(0x53FE10,PadLookRight);
        EntryPatch p10(Body(MemberAddress(&CPad::GetBrake)),PadBrake), p10e(0x540080,PadBrake);
        EntryPatch p11(Body(MemberAddress(&CPad::GetAccelerate)),PadAccel), p11e(0x5403F0,PadAccel);
        EntryPatch h1(Body(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump)),FakeGetHier), h1e(0x734A40,FakeGetHier);
        EntryPatch h2(Body(bit_cast<uintptr_t>(&RpHAnimHierarchyGetMatrixArray)),FakeGetMatArray), h2e(0x7C5120,FakeGetMatArray);
        EntryPatch h3(Body(bit_cast<uintptr_t>(&RpHAnimIDGetIndex)),FakeIDGetIndex), h3e(0x7C51A0,FakeIDGetIndex);
        EntryPatch h4(Body(bit_cast<uintptr_t>(&RwV3dTransformPoints)),FakeTransformPoints), h4e(0x7EDD90,FakeTransformPoints);
        EntryPatch g1(Body(bit_cast<uintptr_t>(&RwMatrixTranslate)),FakeMatTranslate), g1e(0x7F2450,FakeMatTranslate);
        EntryPatch g2(Body(bit_cast<uintptr_t>(&RwMatrixRotate)),FakeMatRotate), g2e(0x7F1FD0,FakeMatRotate);
        EntryPatch g3(Body(bit_cast<uintptr_t>(&RtQuatRotate)),FakeQuatRotate), g3e(0x7EB7C0,FakeQuatRotate);
        EntryPatch g4(Body(bit_cast<uintptr_t>(&RwFrameUpdateObjects)),FakeFrameUpdate), g4e(0x7F0910,FakeFrameUpdate);
        EntryPatch g5(Body(bit_cast<uintptr_t>(&RpClumpRender)),FakeClumpRender), g5e(0x749B20,FakeClumpRender);
        EntryPatch g6(Body(bit_cast<uintptr_t>(&RwFrameGetLTM)),FakeGetLTM), g6e(0x7F0990,FakeGetLTM);
        EntryPatch g7(Body(bit_cast<uintptr_t>(&CClumpModelInfo::GetFrameFromName)),FakeFrameFromName), g7e(0x4C5400,FakeFrameFromName);
        EntryPatch g8(Body(bit_cast<uintptr_t>(&CVisibilityPlugins::GetClumpModelInfo)),FakeClumpMI), g8e(0x732AC0,FakeClumpMI);
        EntryPatch g9(Body(bit_cast<uintptr_t>(&GetFirstAtomic)),FakeFirstAtomic), g9e(0x734820,FakeFirstAtomic);
        EntryPatch g10(Body(bit_cast<uintptr_t>(&RpSkinGeometryGetSkin)),FakeGetSkin), g10e(0x7C7550,FakeGetSkin);
        EntryPatch g11(Body(bit_cast<uintptr_t>(&RpClumpForAllAtomics)),FakeForAllAtomics), g11e(0x749B70,FakeForAllAtomics);
        EntryPatch g12(Body(bit_cast<uintptr_t>(&RpClumpDestroy)),FakeClumpDestroy), g12e(0x74A310,FakeClumpDestroy);
        EntryPatch i1(Body(MemberAddress(&CBaseModelInfo::AddRef)),MIAddRef), i1e(0x4C4BA0,MIAddRef);
        EntryPatch i2(Body(MemberAddress(&CBaseModelInfo::RemoveRef)),MIRemoveRef), i2e(0x4C4BB0,MIRemoveRef);
        EntryPatch f1(Body(MemberAddress(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem))),FakeCreateFx), f1e(0x4A9BE0,FakeCreateFx);
        EntryPatch f2(Body(MemberAddress(&FxSystem_c::Play)),FxPlay), f2e(0x4AA2F0,FxPlay);
        EntryPatch f3(Body(MemberAddress(&FxSystem_c::Kill)),FxKill), f3e(0x4AA3F0,FxKill);
        EntryPatch f4(Body(MemberAddress(&FxSystem_c::CopyParentMatrix)),FxCopyParent), f4e(0x4AA890,FxCopyParent);
        EntryPatch f5(Body(MemberAddress(&FxSystem_c::SetLocalParticles)),FxLocal), f5e(0x4AA910,FxLocal);
        EntryPatch f6(Body(MemberAddress(&FxSystem_c::SetMatrix)),FxSetMatrix), f6e(0x4AA630,FxSetMatrix);
        EntryPatch f7(Body(MemberAddress(&FxSystem_c::SetOffsetPos)),FxOffset), f7e(0x4AA660,FxOffset);
        EntryPatch f8(Body(MemberAddress(&FxSystem_c::SetConstTime)),FxConstTime), f8e(0x4AA6C0,FxConstTime);
        EntryPatch u1(Body(MemberAddress(&CAEPedAudioEntity::TurnOnJetPack)),AeOn), u1e(0x4E28A0,AeOn);
        EntryPatch u2(Body(MemberAddress(&CAEPedAudioEntity::TurnOffJetPack)),AeOff), u2e(0x4E2A70,AeOff);
        EntryPatch u3(Body(MemberAddress(&CAEPedAudioEntity::UpdateJetPack)),AeUpdate), u3e(0x4E0EE0,AeUpdate);
        EntryPatch q1(Body(MemberAddress(&CPed::ReplaceWeaponWhenExitingVehicle)),FakeReplaceWeapon), q1e(0x5E6490,FakeReplaceWeapon);
        EntryPatch q2(Body(MemberAddress(&CPed::RemoveWeaponWhenEnteringVehicle)),FakeRemoveWeapon), q2e(0x5E6370,FakeRemoveWeapon);
        EntryPatch q3(Body(MemberAddress(&CPlayerPed::SetRealMoveAnim)),FakeSetRealMoveAnim), q3e(0x60A9C0,FakeSetRealMoveAnim);
        EntryPatch q4(Body(MemberAddress(&CCamera::CalculateGroundHeight)),FakeGroundHeight), q4e(0x514B80,FakeGroundHeight);
        EntryPatch q5(Body(MemberAddress(&CCamera::GetLookDirection)),FakeLookDir), q5e(0x50AE90,FakeLookDir);
        EntryPatch q6(Body(MemberAddress(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill))),FakeWeaponSkill), q6e(0x5E6580,FakeWeaponSkill);
        EntryPatch q7(Body(MemberAddress(&CTaskSimplePlayerOnFoot::ProcessPlayerWeapon)),FakePlayerWeapon), q7e(0x6859A0,FakePlayerWeapon);
        EntryPatch q8(Body(MemberAddress(&CTaskSimpleJetPack::DropJetPack)),FakeDropJetPack), q8e(0x67B660,FakeDropJetPack);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool jet=cls=="CTaskSimpleJetPack";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    resetPools();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x14B0u; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=cTestCol=cExitJust=cFx=cFrame=fxNext=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    // globales: paso, reloj (a veces por encima de 2^24 o de 2^31), camara y raton
                    G<float>(TIMESTEP)=next()%6==0 ? 1.f : uni(0.2f,3.f);
                    { const uint32 m=next()%8; G<uint32>(TIMEMS)=m==0 ? 0x80000000u+next()%0x70000000u : m<3 ? 0x1000000u+next()%0x7000000u : next()%3000000u; }
                    G<uint16>(NOTEMP)=uint16(next()%50);
                    G<uint8>(MOUSE3RD)=uint8(next()%2); G<uint8>(ACTIVECAM)=uint8(next()%3);
                    for(int k=0;k<3;++k){ auto& v=G<CVector>(CAMFRONT0+k*CAMSTRIDE); v=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.5f,0.5f)}; if(next()%8==0)v.x=0.f; }
                    G<float>(CAMORIENT)=uni(-3.2f,3.2f);
                    // dependencias simuladas
                    mTestCol=next(); mExitJust=next(); mFx=next(); mFrame=next()|(next()%4 ? 3u:0u);
                    pvlHit=next()%4!=0; padExit=next()%3==0; padTarget=next()%3==0; padLookBehind=next()%5==0; padLookLeft=next()%3==0; padLookRight=next()%3==0;
                    { static const int16 vs[]{0,0,128,-128,64,-64,127,-127,32,-100}; padUD=vs[next()%10]; padLR=vs[next()%10]; }
                    padBrake=int16(next()%4==0 ? 255 : 0); padAccel=int16(next()%3==0 ? 255 : 0);
                    lookDir=next()%3 ? 3 : int32(next()%4); weaponSkill=int32(next()%4);
                    groundZ=uni(-20.f,60.f);
                    // peaton 0 (matriz salvo en algun caso de SetPedPosition sin PreRender)
                    auto* p=Ped(0); std::memset(p,0,peds[0].size()); std::memset(mats[0].data(),0,mats[0].size());
                    SetEntityType(p,ENTITY_TYPE_PED);
                    const bool pedMat=!(nm=="SetPedPosition"&&next()%6==0);
                    if(pedMat){ CMatrix m{}; m.SetRotate(uni(-0.4f,0.4f),uni(-0.4f,0.4f),uni(-3.2f,3.2f)); m.GetPosition()=CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,120.f)};
                                std::memcpy(mats[0].data(),&m,sizeof(CMatrix)); At<void*>(p,E_MATRIX)=mats[0].data(); }
                    else { At<CVector>(p,E_POS)=CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,120.f)}; At<float>(p,E_HEADING)=uni(-3.2f,3.2f); }
                    At<uint32>(p,E_FLAGS)=next();
                    At<CVector>(p,PH_SPEED)=CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-0.3f,0.3f)}; if(next()%10==0)At<CVector>(p,PH_SPEED)=CVector{0.f,0.f,0.f};
                    At<float>(p,PH_MASS)=uni(40.f,140.f); At<uint32>(p,PH_FLAGS)=next();
                    At<uint32>(p,P_FLAGS)=next(); At<uint32>(p,P_FLAGS2)=next(); At<uint32>(p,P_FLAGS3)=next(); At<uint32>(p,P_FLAGS4)=next();
                    At<void*>(p,P_INTEL)=intel.data(); std::memset(intel.data(),0,intel.size());
                    std::memset(pdata.data(),0,pdata.size()); At<float>(pdata.data(),PD_BLEND)=next()%4==0 ? 0.f : uni(0.f,1.5f);
                    At<void*>(p,P_PDATA)=(nm=="ProcessControlInput"||next()%4) ? pdata.data() : nullptr;
                    std::memset(clump.data(),0,clump.size()); clump[0]=2; At<void*>(clump.data(),4)=frame.data();
                    At<void*>(p,E_RWOBJ)=(nm=="DoJetPackEffect"&&next()%5==0) ? nullptr : clump.data();
                    for(auto& b:frame)b=uint8(next()); for(int k=0;k<4;++k)At<float>(frame.data(),0x10+0x30+4*k)=uni(-5.f,5.f);
                    for(int k=0;k<NBONES;++k){ auto* b=bones[k].data(); for(int w=0;w<8;++w)At<uint32>(b,4*w)=next()%0x10000; At<void*>(b,0x10)=keyFrames[k&1].data(); At<int32>(b,0x14)=int32(next()%300); At<void*>(p,P_BONES+4*k)=b; }
                    for(auto& kf:keyFrames)for(int w=0;w<8;++w)At<float>(kf.data(),4*w)=uni(-1.f,1.f);
                    for(int w=0;w<NBONES*16;++w)At<float>(boneMats.data(),4*w)=uni(-3.f,3.f);
                    At<int32>(p,P_ANIMGROUP)=int32(next()%118); At<CVector2D>(p,P_SHIFT)=CVector2D{uni(-1.f,1.f),uni(-1.f,1.f)};
                    At<int32>(p,P_MOVESTATE)=int32(next()%8);
                    { static const float hs[]{100.f,1.f,0.999f,0.5f,0.f,-1.f,200.f,qnan}; At<float>(p,P_HEALTH)=next()%3 ? hs[next()%8] : uni(-10.f,200.f); }
                    At<float>(p,P_CURROT)=uni(-3.2f,3.2f); At<float>(p,P_AIMROT)=next()%10==0 ? At<float>(p,P_CURROT)+3.14159274f : uni(-3.3f,3.3f);
                    At<int32>(p,P_PEDTYPE)=next()%3==0 ? int32(4+next()%10) : int32(next()%2);
                    if(nm=="ProcessPed"&&jet&&At<int32>(p,P_PEDTYPE)<2)At<void*>(p,P_PDATA)=pdata.data(); // ProcessControlInput escribe en los datos del jugador
                    { static const int32 ws[]{0,22,23,24,25,28,29,30,31,32,33,34,41,43};
                      for(int k=0;k<13;++k)At<int32>(p,P_WEAPONS+0x1C*k)=ws[next()%14]; At<uint8>(p,P_ACTIVESLOT)=uint8(next()%13); }
                    // peaton 1: entidad no objeto y objetivo fijado
                    auto* p1=Ped(1); std::memset(p1,0,peds[1].size()); std::memset(mats[1].data(),0,mats[1].size()); SetEntityType(p1,ENTITY_TYPE_PED);
                    { const CVector& c=static_cast<CEntity*>(p)->GetPosition(); const CVector pos{c.x+uni(-6.f,6.f),c.y+uni(-6.f,6.f),c.z+uni(-5.f,5.f)};
                      if(next()%4){ CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=pos; std::memcpy(mats[1].data(),&m,sizeof(CMatrix)); At<void*>(p1,E_MATRIX)=mats[1].data(); }
                      else { At<CVector>(p1,E_POS)=pos; At<float>(p1,E_HEADING)=uni(-3.f,3.f); } }
                    At<uint32>(p1,E_FLAGS)=next();
                    At<void*>(p,P_TARGET)=next()%3==0 ? p1 : nullptr;
                    // objeto sostenido
                    auto* ob=obj.data(); std::memset(ob,0,obj.size()); std::memset(mats[2].data(),0,mats[2].size());
                    At<void*>(ob,0)=objVtbl.data(); SetEntityType(ob,ENTITY_TYPE_OBJECT);
                    const bool objMat=nm=="SetPedPosition"||nm=="ProcessPed"||next()%4;
                    { const CVector& c=static_cast<CEntity*>(p)->GetPosition(); const CVector pos{c.x+uni(-2.f,2.f),c.y+uni(-2.f,2.f),c.z+uni(-1.f,2.f)};
                      if(objMat){ CMatrix m{}; m.SetRotate(uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-3.2f,3.2f)); m.GetPosition()=pos; std::memcpy(mats[2].data(),&m,sizeof(CMatrix)); At<void*>(ob,E_MATRIX)=mats[2].data(); }
                      else { At<CVector>(ob,E_POS)=pos; At<float>(ob,E_HEADING)=uni(-3.2f,3.2f); } }
                    At<uint32>(ob,E_FLAGS)=next(); At<uint32>(ob,PH_FLAGS)=next();
                    At<CVector>(ob,PH_SPEED)=CVector{uni(-0.2f,0.2f),uni(-0.2f,0.2f),uni(-0.2f,0.2f)};
                    At<uint8>(ob,O_OBJTYPE)=uint8(next()%7); At<uint32>(ob,O_OBJFLAGS)=next(); At<uint32>(ob,O_REMOVAL)=next(); At<uint8>(ob,O_FAKEPHYS)=uint8(next());
                    std::memset(colModel.data(),0,colModel.size()); At<float>(colModel.data(),8)=uni(-1.f,1.f);
                    // asociaciones (la 1 es la que devuelve BlendAnimation)
                    for(auto& a:assocs){ for(int w=0;w<16;++w)At<uint32>(a.data(),4*w)=next()%0x10000;
                        static const float bl[]{0.f,-0.f,0.5f,1.f,-0.5f,-4.f,4.f,qnan};
                        At<float>(a.data(),AS_AMOUNT)=bl[next()%8]; At<float>(a.data(),AS_DELTA)=bl[next()%8]; At<float>(a.data(),AS_TIME)=next()%4 ? uni(0.f,1.f) : bl[next()%8]; }
                    assocRet=uintptr_t(assocs[1].data());
                    // tabla falsa de grupos de animacion y bloques reales (IsLoaded elegido)
                    std::memset(groups.data(),0,groups.size());
                    for(int g=0;g<128;++g)At<void*>(groups.data(),0x14*size_t(g))=next()%3 ? Block(1+int(next()%8)) : nullptr;
                    for(int k=1;k<9;++k)Block(k)->IsLoaded=next()%3!=0;
                    G<void*>(ASSOCGROUPS)=groups.data();
                    blockByName=uintptr_t(Block(1+int(next()%8)));
                    std::memset(names.data(),0,names.size());
                    for(int k=0;k<3;++k){ const int len=1+int(next()%10); for(int c=0;c<len;++c)names[16*k+c]=char('a'+next()%26); }
                    std::memset(hierAnim.data(),0,hierAnim.size()); std::memset(hier.data(),0,hier.size());
                    // tareas falsas: 0 secundaria (sostener), 1 primaria por defecto
                    for(auto& fk:fakes){ std::memset(fk.data(),0,fk.size()); At<void*>(fk.data(),0)=taskVtbl.data(); At<uint32>(fk.data(),FK_ABORT)=next(); }
                    { auto* s=fakes[0].data(); At<int32>(s,FK_TYPE)=next()%4 ? int32(TASK_SIMPLE_HOLD_ENTITY) : OTHER_TYPE;
                      At<void*>(s,8)=next()%4 ? static_cast<void*>(ob) : nullptr; Reg(At<void*>(s,8),s+8);
                      At<CVector>(s,0xC)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                      { static const int32 gs[]{0,81,82,83,5}; At<int32>(s,0x24)=gs[next()%5]; } }
                    At<int32>(fakes[1].data(),FK_TYPE)=next()%3 ? int32(TASK_SIMPLE_PLAYER_ON_FOOT) : OTHER_TYPE;
                    At<void*>(intel.data(),I_PRIMARY_DEFAULT)=next()%4 ? fakes[1].data() : nullptr;
                    At<void*>(intel.data(),I_SEC_PARTIAL)=next()%4 ? fakes[0].data() : nullptr;
                    // clump, frames, FX, modelo, atomic y evento del jetpack
                    std::memset(jpClump.data(),0,jpClump.size()); jpClump[0]=2; At<void*>(jpClump.data(),4)=jpFrame.data();
                    for(auto* fr:{&jpFrame,&jbFrames[0],&jbFrames[1]}){ for(int w=0;w<24;++w)At<float>(fr->data(),4*w)=uni(-2.f,2.f); }
                    for(auto& fx:fxs)std::memset(fx.data(),0,fx.size());
                    std::memset(mi.data(),0,mi.size()); At<void*>(mi.data(),0)=miVtbl.data();
                    std::memset(atomic.data(),0,atomic.size()); At<void*>(atomic.data(),0x18)=vecBuf.data();
                    instRet=next()%4 ? uintptr_t(jpClump.data()) : 0; atomicRet=next()%4 ? uintptr_t(atomic.data()) : 0; skinRet=next()%2 ? uintptr_t(vecBuf.data()+8) : 0;
                    G<void*>(MI_JETPACK)=mi.data(); G<uint8>(SI_JETPACK+0x10)=uint8(next()%3==0 ? next()%4 : 1);
                    std::memset(event.data(),0,event.size()); At<void*>(event.data(),0)=eventVtbl.data();
                    { static const int32 ts[]{EVENT_DAMAGE,EVENT_ON_FIRE,EVENT_SCRIPT_COMMAND,EVENT_DAMAGE,EVENT_SOUND_QUIET}; At<int32>(event.data(),EV_TYPE)=ts[next()%5]; }
                    { static const int32 ps[]{62,63,64,10,70}; At<int32>(event.data(),EV_PRIO)=ps[next()%5]; }
                    std::memset(padBuf.data(),0,padBuf.size()); std::memset(vecBuf.data(),0,vecBuf.size());

                    // tarea probada
                    auto* o=object.data(); std::memset(o,0,object.size());
                    int32 a0=0,a1=0,a2=0; float f0=0.f; void* q0=nullptr; void* q1=nullptr; bool b0=false; const void* ev=nullptr; CVector ctorPos{};
                    if(!jet){
                        At<void*>(o,0)=taskVtbl.data();
                        { static const int32 ts[]{TASK_SIMPLE_HOLD_ENTITY,TASK_SIMPLE_PICKUP_ENTITY,TASK_SIMPLE_PUTDOWN_ENTITY,TASK_SIMPLE_HOLD_ENTITY,OTHER_TYPE};
                          At<int32>(o,FK_TYPE)=ts[next()%5]; }
                        At<uint32>(o,FK_ABORT)=next();
                        { const uint32 m=next()%20; void* e=m<11 ? static_cast<void*>(ob) : m<14 ? static_cast<void*>(p1) : nullptr; At<void*>(o,8)=e; if(!nm.starts_with("Constructor"))Reg(e,o+8); }
                        At<CVector>(o,0xC)=CVector{uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-0.5f,0.5f)};
                        At<uint8>(o,0x18)=uint8(next()%4 ? next()%NBONES : 6);
                        { static const uint8 bf[]{0x10,0x10,0,0x11,1}; At<uint8>(o,0x19)=bf[next()%5]; }
                        At<uint16>(o,0x1A)=uint16(next());
                        At<float>(o,0x1C)=uni(-3.2f,3.2f);
                        { static const int32 ids[]{ANIM_ID_NO_ANIMATION_SET,ANIM_ID_NO_ANIMATION_SET,ANIM_ID_PUTDWN,3,200,0}; At<int32>(o,0x20)=ids[next()%6]; }
                        { static const int32 gs[]{0,0,81,82,83,5,32}; At<int32>(o,0x24)=gs[next()%7]; }
                        At<uint32>(o,0x28)=next()%0x10000;
                        At<void*>(o,0x2C)=next()%3==0 ? Block(1+int(next()%8)) : nullptr;
                        At<void*>(o,0x30)=next()%3==0 ? hierAnim.data() : nullptr;
                        o[0x34]=uint8(next()%4==0); o[0x35]=uint8(next()%2); o[0x36]=uint8(next()%2); o[0x37]=uint8(next());
                        At<void*>(o,0x38)=next()%3 ? assocs[0].data() : nullptr;
                        { static const float pr[]{0.f,0.3f,0.5f,1.f,2.f}; At<float>(o,0x3C)=next()%3 ? uni(0.f,1.f) : pr[next()%5]; }
                        At<CVector>(o,0x40)=next()%4==0 ? CVector{0.f,0.f,uni(-1.f,1.f)} : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        if(next()%10==0)At<float>(o,0x40)=0.f;
                        // ajustes por funcion
                        if(nm=="ProcessPed"&&At<int32>(o,FK_TYPE)==TASK_SIMPLE_PUTDOWN_ENTITY&&next()%2){ // rama de PUTDOWN sin entidad: toma la de la secundaria
                            if(At<void*>(o,8)){ static_cast<CEntity*>(At<void*>(o,8))->CleanUpOldReference(reinterpret_cast<CEntity**>(o+8)); At<void*>(o,8)=nullptr; }
                            At<void*>(o,0x30)=nullptr; At<int32>(o,0x24)=0; o[0x35]=1; }
                        if(nm=="SetPedPosition"&&At<void*>(o,8)==p1&&!At<void*>(p1,E_MATRIX)){ CMatrix m{}; m.SetRotate(0.f,0.f,0.5f); m.GetPosition()=At<CVector>(p1,E_POS); std::memcpy(mats[1].data(),&m,sizeof(CMatrix)); At<void*>(p1,E_MATRIX)=mats[1].data(); }
                        if(nm=="SetPedPosition"&&!pedMat)At<uint32>(p,P_FLAGS3)&=~0x400u; // sin matriz el EXE solo usa la posicion simple
                        if(nm=="ProcessPed"&&At<int32>(o,FK_TYPE)==TASK_SIMPLE_PICKUP_ENTITY&&At<void*>(o,8)==p1&&!At<void*>(p1,E_MATRIX)){ CMatrix m{}; m.SetRotate(0.f,0.f,0.5f); m.GetPosition()=At<CVector>(p1,E_POS); std::memcpy(mats[1].data(),&m,sizeof(CMatrix)); At<void*>(p1,E_MATRIX)=mats[1].data(); }
                        if(nm=="StartAnim"&&next()%3==0)At<void*>(o,0x2C)=nullptr;
                        if(nm=="CanThrowEntity"){ static const uint8 bs[]{6,6,5,7}; At<uint8>(o,0x18)=bs[next()%4]; }
                        if(nm=="ChoosePutDownHeight"){ const float pz=static_cast<CEntity*>(p)->GetPosition().z; static const float d[]{-0.2f,-0.7f,0.f,-1.f,0.5f};
                            pvlZ=next()%3==0 ? pz+d[next()%5] : (next()%10==0 ? qnan : pz+uni(-1.5f,0.5f)); }
                        else pvlZ=static_cast<CEntity*>(p)->GetPosition().z+uni(-1.5f,0.5f);
                        if(nm=="FinishAnimHoldEntityCB")q0=assocs[0].data();
                        if(nm=="MakeAbortable"){ a0=int32(next()%3); ev=next()%3 ? event.data() : nullptr; }
                        if(nm=="PlayAnim"){ a0=int32(next()%400); a1=int32(next()%120); }
                        if(nm=="DropEntity")b0=next()%3!=0;
                        if(nm.starts_with("Constructor")){
                            q0=next()%4 ? static_cast<void*>(next()%4 ? static_cast<void*>(ob) : p1) : nullptr; // entidad
                            ctorPos=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; q1=next()%4 ? &ctorPos : nullptr;
                            a0=int32(next()%NBONES); a1=int32(next()%3 ? 0x10 : next()%256); a2=int32(next()%0x10000); b0=next()%2;
                        }
                    } else {
                        At<uintptr_t>(o,0)=VT_JET;
                        o[8]=uint8(next()%4==0); o[9]=uint8(next()%2); o[0xA]=uint8(next()%2); o[0xB]=uint8(next()%2); o[0xC]=uint8(next()%2);
                        o[0xD]=uint8(next()%3==0);
                        { const bool noDown=nm=="DoJetPackEffect"||nm=="RenderJetPack"; static const int8 fw[]{0,1,-1,1,0}; At<int8>(o,0xE)=fw[next()%(noDown ? 2:5)]; }
                        { static const float ss[]{0.f,-0.f,0.05f,-0.05f,0.5f,-0.5f,1.f,-1.f,2.5f,-2.9f,3.f,-3.f}; At<float>(o,0x10)=next()%3 ? ss[next()%12] : uni(-3.f,3.f); }
                        At<float>(o,0x14)=next()%8==0 ? 0.f : uni(-1.4f,1.4f);
                        for(int k=0;k<3;++k)At<float>(o,0x18+4*k)=next()%6==0 ? 0.f : uni(-0.9f,0.9f);
                        for(int k=0;k<3;++k)At<float>(o,0x24+4*k)=uni(-0.1f,0.1f);
                        At<CVector>(o,0x30)=CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-0.3f,0.3f)};
                        At<float>(o,0x3C)=next()%8==0 ? At<float>(p,P_AIMROT)-3.14159274f : uni(-3.3f,3.3f);
                        At<void*>(o,0x40)=next()%4 ? jpClump.data() : nullptr;
                        At<uint32>(o,0x44)=0;
                        At<CVector>(o,0x48)=CVector{uni(-100.f,100.f),uni(-100.f,100.f),uni(0.f,50.f)};
                        At<float>(o,0x54)=uni(0.f,30.f); At<int32>(o,0x58)=int32(next()%5000); At<uint32>(o,0x5C)=next()%100000;
                        At<void*>(o,0x60)=next()%2 ? p1 : nullptr; if(nm!="Constructor")Reg(At<void*>(o,0x60),o+0x60);
                        At<void*>(o,0x64)=next()%3 ? fxs[0].data() : nullptr; At<void*>(o,0x68)=next()%3 ? fxs[1].data() : nullptr;
                        { static const float kt[]{0.f,0.1f,0.95f,1.f,0.5f,-0.05f,1.05f,0.05f}; At<float>(o,0x6C)=next()%3 ? kt[next()%8] : uni(-0.2f,1.2f); }
                        if(nm=="Constructor"){ ctorPos=CVector{uni(-100.f,100.f),uni(-100.f,100.f),uni(0.f,50.f)}; q1=next()%4 ? &ctorPos : nullptr;
                            f0=uni(0.f,30.f); a0=int32(next()%5000); q0=next()%2 ? p1 : nullptr; }
                        if(nm=="MakeAbortable"){ a0=int32(next()%3); ev=next()%4 ? event.data() : nullptr; }
                        { // Estres del lote JetPack (flujo aleatorio aparte: no mueve las semillas del resto del banco). Solo toca entradas que el C++ SSE restante no combina
                          // (un NaN por operacion; la rama de parada de ProcessThrust y las fuerzas con la velocidad no se estresan: siguen en C++ SSE).
                          uint32 sx=uint32(i)*0x85EBCA6Bu^uint32(f)*0xC2B2AE35u^0x4A37E5u; if(!sx)sx=1;
                          auto nx=[&]{sx^=sx<<13;sx^=sx>>17;sx^=sx<<5;return sx;};
                          static const uint32 sa[]{0x7FC00000u,0xFFC00000u,0x7FA00000u,0xFFA00000u,0x7F800000u,0xFF800000u,0x00000001u,0x80000001u,0x80000000u,0x501502F9u,0xD01502F9u};
                          static const uint32 sv[]{0x7FC00000u,0xFFC00000u,0x7FA00000u,0xFFA00000u,0x7F800000u,0xFF800000u,0x00000001u,0x80000001u,0x80000000u,0x7149F2CAu,0xF149F2CAu,0x0DA24260u,0x8DA24260u,0x7F7FFFFFu,0xFF7FFFFFu};
                          const bool thrust=nm=="ProcessThrust", ctrl=nm=="ProcessControlInput";
                          if((thrust||ctrl||nm=="ProcessPed")&&nx()%6==0)At<uint32>(o,0x14)=sa[nx()%11]; // angulo de empuje: ThrustDir, Lerp
                          if(thrust&&!o[0xD]&&nx()%4==0){ // velocidad del peaton, velocidad previa, velocidades y angulos de balanceo: cola de ProcessThrust
                              const uint32 w=nx()%4, k=nx()%3, bits=sv[nx()%15];
                              if(w==0)At<uint32>(p,PH_SPEED+4*k)=bits; else if(w==1)At<uint32>(o,0x30+4*k)=bits; else if(w==2)At<uint32>(o,0x24+4*(k%2))=bits; else At<uint32>(o,0x18+4*k)=bits; }
                          if(ctrl&&nx()%6==0){ auto& v=G<CVector>(CAMFRONT0+(nx()%3)*CAMSTRIDE); (&v.x)[nx()%2]=std::bit_cast<float>(sv[nx()%15]); } // frente de la camara: fpatan
                        }
                    }
                    const auto before=Snapshot(); const auto occT=Occ(TaskHdr());
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; uintptr_t retPtr{}; bool isPtr=false;
                    CPed* pd=static_cast<CPed*>(p);
                    auto callPed=[&]{ return reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd); };
                    auto callPedVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd); };
                    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    if(nm=="Clone"){ retPtr=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); isPtr=true; }
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const void*)>(target)(o,pd,a0,ev);
                    else if(nm=="ProcessPed"||nm=="SetPedPosition"||nm=="ProcessAnims")ret=callPed();
                    else if(!jet){
                        if(nm=="Constructor-1"){ std::memset(o,0xCD,object.size()); retPtr=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,void*,uint32,uint32,int32,int32,uint32)>(target)(o,q0,q1,uint32(a0),uint32(a1),a2%400,a2%120,b0 ? 1u:0u)); isPtr=true; }
                        else if(nm=="Constructor-2"){ std::memset(o,0xCD,object.size()); retPtr=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,void*,uint32,uint32,const char*,const char*,int32)>(target)(o,q0,q1,uint32(a0),uint32(a1),names.data(),names.data()+16,a2)); isPtr=true; }
                        else if(nm=="Constructor-3"){ std::memset(o,0xCD,object.size()); retPtr=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,void*,uint32,uint32,void*,void*,int32)>(target)(o,q0,q1,uint32(a0),uint32(a1),Block(1+a2%8),a2%3 ? hierAnim.data() : nullptr,a2)); isPtr=true; }
                        else if(nm=="CanThrowEntity")ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(o);
                        else if(nm=="ReleaseEntity")callVoid();
                        else if(nm=="PlayAnim")reinterpret_cast<void(__thiscall*)(void*,int32,int32)>(target)(o,a0,a1);
                        else if(nm=="FinishAnimHoldEntityCB")reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(q0,o);
                        else if(nm=="StartAnim"||nm=="ChoosePutDownHeight")callPedVoid();
                        else if(nm=="DropEntity")reinterpret_cast<void(__thiscall*)(void*,CPed*,uint32)>(target)(o,pd,b0 ? 1u:0u);
                    } else {
                        if(nm=="Constructor"){ std::memset(o,0xCD,object.size()); retPtr=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const void*,float,int32,void*)>(target)(o,q1,f0,a0,q0)); isPtr=true; }
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="StopJetPackEffect")callVoid();
                        else callPedVoid(); // RenderJetPack, ProcessThrust, ProcessControlInput, ApplyRollAndPitch, DoJetPackEffect
                    }
                    for(auto* h:hooks)h->State(false);

                    // tareas nuevas (se nombran antes de serializar)
                    { auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                      for(size_t k=0;k<occT.size();++k)if(!occT[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                      json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks; }
                    if(isPtr)ret=Name(retPtr);
                    if(nm.starts_with("Constructor"))out["type"]=TypeOf(o);
                    out["changes"]=Changes(before); out["refs"]=AllRefs();
                    out["notemp"]=G<uint16>(NOTEMP);
                    out["ret"]=ret; out["trace"]=trace; out["rng"]=rngCalls;
                    CleanRefs();
                    FreeFresh(TaskHdr(),occT);
                    out["clean"]=Occ(TaskHdr())==occT;
                    restoreGlobals();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs(); restoreGlobals(); resetPools();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occ(TaskHdr())==occTaskStart&&freeRefs()==savedFree&&globalsIntact()&&Last(TaskHdr())==lt0;
    if(restored)std::memcpy(States(TaskHdr()),stTask.data(),stTask.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
