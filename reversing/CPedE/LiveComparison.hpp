#pragma once
// Banco dentro del motor: CPed, grupo E (posicion, enganches, voz y percepcion; 25 funciones):
// AttachPedToBike, AttachPedToEntity, DettachPedFromEntity, PositionAnyPedOutOfCollision, PositionAttachedPed,
// UpdatePosition, Teleport (vtable 0x86C358, ranura 14), SetPedPositionInCar, GetBonePosition-Original,
// GetTransformedBonePosition, CanSeeEntity, OurPedCanSeeThisEntity, DoesLOSBulletHitPed, IsPedHeadAbovePos,
// ReleaseCoverPoint, Say, SayScript, DisablePedSpeech(ForScriptSpeech), EnablePedSpeech(ForScriptSpeech), GetPedTalking,
// CanPedHoldConversation, PedIsInvolvedInConversation y PedIsReadyForConversation.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos y comparacion bit a bit.
// Peatones FALSOS (buffer del tamano de CPed rellenado con 0xCD y solo los campos que el EXE lee), nunca el jugador ni
// peatones reales: matrices propias (o posicion compacta sin matriz), vehiculo falso (CBike/CAutomobile/CBmx en un buffer
// de 0xA00 bytes) con informacion de modelo falsa (se intercambian temporalmente las entradas 532 y 19990 de
// CModelInfo::ms_modelInfoPtrs), jerarquia de huesos y matrices falsas, objeto falso, inteligencia/datos de jugador/
// CWanted falsos y entidad de voz falsa (vtable propia en +0x294).
// Se simulan y trazan (en el EXE y en la DLL): CWorld::Add/Remove/TestSphereAgainstWorld/ProcessLineOfSight,
// CPedIntelligence::FlushImmediately/GetTaskSwim/GetTaskJetPack/GetTaskFighting/FindTaskByType,
// CTaskManager::FindActiveTaskByType, CEventGroup::GetEventOfType, CPhysical::GetSpeed/ApplyMoveForce,
// CEntity::GetColModel/UpdateRpHAnim, GetAnimHierarchyFromSkinClump, RpHAnimIDGetIndex/GetMatrixArray,
// RwV3dTransformPoints, RwFrameGetLTM, CPed::GiveWeapon/SetCurrentWeapon(ranura), CPlayerPed::MakeChangesForNewWeapon/
// ClearWeaponTarget, CCamera::SetNewPlayerWeaponMode, CAnimManager::BlendAnimation, CBike::CalculateLeanMatrix,
// SurfaceInfos_c::IsSteepSlope, CCoverPoint::ReleaseCoverPointForPed, FindPlayerPed y los metodos de CAEPedSpeechAudioEntity
// (GetPedTalking, Enable/DisablePedSpeech(ForScriptSpeech), CanPedHoldConversation, AddSayEvent, AddScriptSayEvent).
// Se guardan y restauran CReplay::Mode, ms_fTimeStep, CPedToPlayerConversations::m_pPed y las dos entradas de
// ms_modelInfoPtrs. Las referencias de las entidades falsas se limpian tras cada ejecucion.
#include <fstream>
#include <functional>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Bmx.h"
#include "Entity/Vehicle/Automobile.h"
#include "PlayerPedData.h"
#include "Wanted.h"
#include "PedIntelligence.h"
#include "Tasks/TaskManager.h"
#include "Events/EventGroup.h"
#include "CoverPoint.h"
#include "World.h"
#include "PedPlacement.h"
#include "Camera.h"
#include "Timer.h"
#include "Replay.h"
#include "SurfaceInfos_c.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Audio/Entities/AEPedSpeechAudioEntity.h"
#include "Conversations/PedToPlayerConversations.h"
#include "Animation/AnimManager.h"
#include "WeaponInfo.h"
#include "RwHelper.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace CPedELive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
// Sigue los saltos de la ILT de la DLL sin entrar en la imagen del EXE (como PedSpeech58/FixAudio)
inline uintptr_t Body(uintptr_t p) {
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p;
}
// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_CLUMP=0x18, E_FLAGS=0x1C, E_MODEL=0x22, E_TYPE=0x36, E_PHYS=0x40, E_MOVE=0x44, E_TURN=0x50,
                 E_DAMAGE=0xDC, E_ATTACHED=0xFC, E_IGNORED=0x128, E_CONTACTSURF=0xBA,
                 P_SPEECH=0x294, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_PLAYERDATA=0x480,
                 P_CREATEDBY=0x484, P_BONES=0x488, P_ANIMGROUP=0x4D4, P_STATE=0x530, P_MOVESTATE=0x534, P_SHIFT=0x550, P_ROT=0x558,
                 P_AIMROT=0x55C, P_STANDING=0x568, P_F56C=0x56C, P_F578=0x578, P_VEHICLE=0x58C, P_PEDTYPE=0x598, P_WEAPONS=0x5A0,
                 P_SAVEDW=0x70C, P_WSLOT=0x718, P_TOFF=0x76C, P_TA=0x778, P_TB=0x77C, P_TMODE=0x780, P_TAMMO=0x784, P_COVER=0x788,
                 V_HANDLING=0x384, V_DRIVER=0x460, V_PASS=0x464, V_TYPE=0x590, V_SUBTYPE=0x594, V_LEAN=0x5CC, V_NODES=0x648,
                 V_BMXJUMP=0x814, V_DOOM=0x94C;
constexpr uintptr_t ADDR_REPLAY=0xA43088, ADDR_TIME_STEP=0xB7CB5C, ADDR_CONV_PED=0x9691C0, ADDR_MODELINFOS=0xA9B0C8;
constexpr int VEH_MODEL_A=532 /* MODEL_COMBINE */, VEH_MODEL_B=19990, MODEL_ATTACH_FIRELA=544, MODEL_ATTACH_POOLCUE=338;
constexpr size_t NBONE_MATS=8;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matA{}, matB{}, matV{}, matO{}, matLean{};
alignas(16) inline std::array<uint8,0x200> obj{};
alignas(16) inline std::array<uint8,0xA00> veh{};
alignas(16) inline std::array<uint8,0x100> handling{}, vinfoA{}, vinfoB{}, vstructA{}, vstructB{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdataA{}, pdataB{};
alignas(16) inline std::array<uint8,sizeof(CWanted)> wanted{};
alignas(16) inline std::array<uint8,0x400> clump{};
alignas(16) inline std::array<uint8,0x100> boneFrame{}, task{}, frameBuf{}, colModel{}, colPoint{}, hierBuf{};
alignas(16) inline std::array<uint8,0x40*NBONE_MATS> boneMats{};
alignas(16) inline std::array<uint8,0x100> ltmBuf{}, rwMatNode{}, outVec{}, vecBuf{}, coverBuf{}, weaponBuf{};
alignas(16) inline std::array<void*,64> pedVtbl{};
alignas(16) inline std::array<void*,16> speechVtbl{}, taskVtbl{};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Obj() { return reinterpret_cast<CEntity*>(obj.data()); }
inline CEntity* Veh() { return reinterpret_cast<CEntity*>(veh.data()); }
struct Buf { const void* p; size_t n; const char* nm; };
inline std::vector<Buf> bufs;
inline void InitBufs() {
    bufs={{pedA.data(),pedA.size(),"pedA"},{pedB.data(),pedB.size(),"pedB"},{matA.data(),matA.size(),"matA"},{matB.data(),matB.size(),"matB"},
          {matV.data(),matV.size(),"matV"},{matO.data(),matO.size(),"matO"},{matLean.data(),matLean.size(),"matLean"},{obj.data(),obj.size(),"obj"},
          {veh.data(),veh.size(),"veh"},{handling.data(),handling.size(),"handling"},{vinfoA.data(),vinfoA.size(),"vinfoA"},{vinfoB.data(),vinfoB.size(),"vinfoB"},
          {vstructA.data(),vstructA.size(),"vstructA"},{vstructB.data(),vstructB.size(),"vstructB"},{intel.data(),intel.size(),"intel"},
          {pdataA.data(),pdataA.size(),"pdataA"},{pdataB.data(),pdataB.size(),"pdataB"},{wanted.data(),wanted.size(),"wanted"},{clump.data(),clump.size(),"clump"},
          {boneFrame.data(),boneFrame.size(),"boneFrame"},{task.data(),task.size(),"task"},{frameBuf.data(),frameBuf.size(),"frame"},
          {colModel.data(),colModel.size(),"colModel"},{colPoint.data(),colPoint.size(),"colPoint"},{hierBuf.data(),hierBuf.size(),"hier"},
          {boneMats.data(),boneMats.size(),"boneMats"},{ltmBuf.data(),ltmBuf.size(),"ltm"},{rwMatNode.data(),rwMatNode.size(),"rwMatNode"},
          {outVec.data(),outVec.size(),"outVec"},{vecBuf.data(),vecBuf.size(),"vec"},{coverBuf.data(),coverBuf.size(),"cover"},
          {pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"},{speechVtbl.data(),sizeof(speechVtbl),"speechvtbl"},{taskVtbl.data(),sizeof(taskVtbl),"taskvtbl"}};
}
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return std::string(b.nm)+"+"+std::to_string(v-uintptr_t(b.p));
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
// puntero de un argumento: los buffers del banco por nombre; las variables locales (pila) del EXE y de la DLL no son comparables
inline json Ptr(uintptr_t v) { if(!v)return nullptr; auto n=Name(v); return n.is_string() ? n : json("other"); }
// relleno de matriz que el EXE deja con basura de pila: las palabras +0x0C/+0x1C/+0x2C/+0x3C (pads/flags) se normalizan a 0
template<size_t N> std::array<uint8,N> MaskMat(const std::array<uint8,N>& m) { auto c=m; for(size_t o:{size_t(0xC),size_t(0x1C),size_t(0x2C),size_t(0x3C)})std::memset(c.data()+o,0,4); return c; }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}
// valores del caso consultados por los falsos
inline uint32 stubSt{};
inline uint32 SNext() { stubSt^=stubSt<<13; stubSt^=stubSt>>17; stubSt^=stubSt<<5; return stubSt; }
inline float SUni(float a,float b) { return a+(b-a)*float(SNext()%100000)/100000.f; }
inline int sphMode{}; inline uint32 sphSeed{}; inline bool vehSeen{};
inline uint32 losSeed{}; inline int losMode{};
inline bool hierNull{}, steepRet{}, leaveCarRet{}, fightRet{}, findTaskRet{}, eventRet{}, swimRet{}, jetRet{};
inline int primTaskType{};
inline uint32 sayRet{}; inline uint32 giveSlot{};
inline CVector colMax{};

// vtables falsas trazadas
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
template<int N> void* __fastcall STrap(void*,void*) { trace.push_back({"speech_unexpected_slot",N}); return nullptr; }
template<int... N> void FillSTraps(std::integer_sequence<int,N...>) { ((speechVtbl[N]=reinterpret_cast<void*>(&STrap<N>)),...); }
inline int __fastcall TType(void* t,void*) { trace.push_back({"task_get_type",Name(uintptr_t(t))}); return primTaskType; }
template<int N> void* __fastcall TTrap(void*,void*) { trace.push_back({"task_unexpected_slot",N}); return nullptr; }
template<int... N> void FillTTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&TTrap<N>)),...); }

inline json Vec3(const CVector& v) { return json{F(v.x),F(v.y),F(v.z)}; }
// ---- dependencias simuladas ----
inline void __cdecl WAdd(CEntity* e) { trace.push_back({"world_add",Name(uintptr_t(e))}); }
inline void __cdecl WRemove(CEntity* e) { trace.push_back({"world_remove",Name(uintptr_t(e))}); }
inline void __fastcall FlushStub(void* i,void*,uint32 b) { trace.push_back({"flush_immediately",Name(uintptr_t(i)),b&0xFF}); }
inline void* __fastcall FindActiveStub(void* m,void*,int type) { trace.push_back({"find_active_task_by_type",Name(uintptr_t(m)),type}); return leaveCarRet ? task.data() : nullptr; }
inline void* __fastcall SwimStub(void* i,void*) { trace.push_back({"get_task_swim",Name(uintptr_t(i))}); return swimRet ? task.data() : nullptr; }
inline void* __fastcall JetStub(void* i,void*) { trace.push_back({"get_task_jetpack",Name(uintptr_t(i))}); return jetRet ? task.data() : nullptr; }
inline void* __fastcall FightStub(void* i,void*) { trace.push_back({"get_task_fighting",Name(uintptr_t(i))}); return fightRet ? task.data() : nullptr; }
inline void* __fastcall FindTaskStub(void* i,void*,int type) { trace.push_back({"find_task_by_type",Name(uintptr_t(i)),type}); return findTaskRet ? task.data() : nullptr; }
inline void* __fastcall EventOfTypeStub(void* g,void*,int type) { trace.push_back({"get_event_of_type",Name(uintptr_t(g)),type}); return eventRet ? task.data() : nullptr; }
inline void __fastcall UpdRpHAnim(CEntity* e,void*) { trace.push_back({"update_rp_hanim",Name(uintptr_t(e))}); }
inline RpHAnimHierarchy* __cdecl HierStub(RpClump* c) { trace.push_back({"anim_hierarchy_from_skin_clump",Name(uintptr_t(c))}); return hierNull ? nullptr : reinterpret_cast<RpHAnimHierarchy*>(hierBuf.data()); }
inline RwInt32 __cdecl IdIndexStub(RpHAnimHierarchy* h,RwInt32 id) { trace.push_back({"anim_id_get_index",Name(uintptr_t(h)),id}); return RwInt32(uint32(id)*7u%NBONE_MATS); }
inline RwMatrix* __cdecl MatArrayStub(RpHAnimHierarchy* h) { trace.push_back({"anim_matrix_array",Name(uintptr_t(h))}); return reinterpret_cast<RwMatrix*>(boneMats.data()); }
// transformacion propia (flotantes sencillos): el resultado solo depende de los bytes de la matriz y del punto
inline void XformPoint(const float* in,const float* m,float* out) {
    const float x=in[0]*m[0]+in[1]*m[4]+in[2]*m[8]+m[12], y=in[0]*m[1]+in[1]*m[5]+in[2]*m[9]+m[13], z=in[0]*m[2]+in[1]*m[6]+in[2]*m[10]+m[14];
    out[0]=x; out[1]=y; out[2]=z;
}
inline RwV3d* __cdecl XformPointsStub(RwV3d* out,const RwV3d* in,RwInt32 n,const RwMatrix* m) {
    trace.push_back({"rw_v3d_transform_points",Ptr(uintptr_t(out)),Ptr(uintptr_t(in)),n,Ptr(uintptr_t(m)),F(in->x),F(in->y),F(in->z)});
    for(RwInt32 k=0;k<n;++k){ float t[3]; XformPoint(reinterpret_cast<const float*>(in+k),reinterpret_cast<const float*>(m),t); std::memcpy(out+k,t,12); }
    return out;
}
inline RwMatrix* __cdecl LtmStub(RwFrame* f) { trace.push_back({"rw_frame_get_ltm",Name(uintptr_t(f))}); return reinterpret_cast<RwMatrix*>(ltmBuf.data()); }
inline int __fastcall GiveWeaponStub(void* p,void*,uint32 w,uint32 ammo,uint32 b) { trace.push_back({"give_weapon",Name(uintptr_t(p)),w,ammo,b&0xFF}); return int(giveSlot); }
inline void __fastcall SetCurWeaponStub(void* p,void*,int32 slot) { trace.push_back({"set_current_weapon_slot",Name(uintptr_t(p)),slot}); }
inline void __fastcall MakeChangesStub(void* p,void*,uint32 w) { trace.push_back({"make_changes_for_new_weapon",Name(uintptr_t(p)),w}); }
inline void __fastcall ClearTargetStub(void* p,void*) { trace.push_back({"clear_weapon_target",Name(uintptr_t(p))}); }
inline void __fastcall CamModeStub(void* c,void*,int32 mode,uint32 a,uint32 b) { trace.push_back({"camera_set_new_player_weapon_mode",mode,a&0xFFFF,b&0xFFFF}); }
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return nullptr; }
inline void __fastcall MoveForceStub(void* p,void*,CVector f) { trace.push_back({"apply_move_force",Name(uintptr_t(p)),Vec3(f)}); }
inline void __fastcall LeanStub(void* b,void*) {
    trace.push_back({"calculate_lean_matrix",Name(uintptr_t(b))});
    for(size_t k=0;k<16;++k)At<float>(veh.data(),V_LEAN+4*k)=SUni(-1.f,1.f);
    At<uint32>(veh.data(),V_LEAN+0x40)=0; At<uint32>(veh.data(),V_LEAN+0x44)=0;
}
inline CVector* __fastcall GetSpeedStub(void* self,void*,CVector* out,CVector pt) {
    trace.push_back({"get_speed",Name(uintptr_t(self)),Vec3(pt)});
    *out=CVector{SUni(-1.f,1.f),SUni(-1.f,1.f),SUni(-1.f,1.f)}; return out;
}
inline bool __fastcall SteepStub(void* s,void*,uint32 id) { trace.push_back({"is_steep_slope",id&0xFF}); return steepRet; }
inline CEntity* __cdecl SphStub(CVector c,float r,CEntity* ig,uint32 b,uint32 v,uint32 p,uint32 o,uint32 d,uint32 cam) {
    trace.push_back({"test_sphere",Vec3(c),F(r),Name(uintptr_t(ig)),b&0xFF,v&0xFF,p&0xFF,o&0xFF,d&0xFF,cam&0xFF});
    uint32 h=(F(c.x)*2654435761u)^(F(c.y)*2246822519u)^sphSeed; h^=h>>15; h*=2246822519u; h^=h>>13;
    if(b&0xFF) return sphMode==0 ? nullptr : sphMode==2 ? (h%97==0 ? nullptr : Obj()) : (h%5==0 ? Obj() : nullptr);
    if(v&0xFF) { if(sphMode==3||(sphMode==1&&h%3==0))vehSeen=true; return vehSeen ? Veh() : nullptr; }
    return nullptr;
}
inline bool __cdecl LosStub(const CVector& a,const CVector& b,CColPoint&,CEntity*&,uint32 bu,uint32 ve,uint32 pe,uint32 ob,uint32 du,uint32 see,uint32 cam,uint32 shoot) {
    trace.push_back({"process_line_of_sight",Vec3(a),Vec3(b),bu&0xFF,ve&0xFF,pe&0xFF,ob&0xFF,du&0xFF,see&0xFF,cam&0xFF,shoot&0xFF});
    uint32 h=(F(a.x)*2654435761u)^(F(b.y)*2246822519u)^losSeed; h^=h>>15; h*=2246822519u; h^=h>>13;
    return losMode==0 ? false : losMode==1 ? true : (h%3==0);
}
inline void FindZCalc(CVector& p,bool& found) {
    uint32 h=(F(p.x)*2654435761u)^(F(p.y)*2246822519u)^sphSeed; h^=h>>15; h*=2246822519u; h^=h>>13;
    found=h%3!=0; if(found)p.z+=float(h%50)*0.01f;
}
// 0x616920 del EXE: corrige la z en el sitio y devuelve si hubo suelo
inline bool __cdecl FindZExe(CVector* p) { trace.push_back({"find_z_for_ped",Vec3(*p)}); bool f; FindZCalc(*p,f); return f; }
inline std::pair<CVector,bool> __cdecl FindZDll(CVector pos) { trace.push_back({"find_z_for_ped",Vec3(pos)}); bool f; FindZCalc(pos,f); return {pos,f}; }
inline CColModel* __fastcall ColStub(CEntity* e,void*) { trace.push_back({"get_col_model",Name(uintptr_t(e))}); return reinterpret_cast<CColModel*>(colModel.data()); }
inline void __fastcall CoverStub(void* c,void*,CPed* p) { trace.push_back({"release_cover_point_for_ped",Name(uintptr_t(c)),Name(uintptr_t(p))}); }
inline CPlayerPed* __cdecl FindPlayerStub(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(PedB()); }
// entidad de voz
inline uint32 __fastcall SpGetTalking(void* s,void*) { trace.push_back({"speech_get_ped_talking",Name(uintptr_t(s))}); return SNext()&1u; }
inline uint32 __fastcall SpCanHold(void* s,void*) { trace.push_back({"speech_can_hold_conversation",Name(uintptr_t(s))}); return SNext()&1u; }
inline void __fastcall SpEnable(void* s,void*) { trace.push_back({"speech_enable",Name(uintptr_t(s))}); }
inline void __fastcall SpEnableScript(void* s,void*) { trace.push_back({"speech_enable_script",Name(uintptr_t(s))}); }
inline void __fastcall SpDisable(void* s,void*,uint32 a) { trace.push_back({"speech_disable",Name(uintptr_t(s)),a&0xFFFF}); }
inline void __fastcall SpDisableScript(void* s,void*,uint32 a) { trace.push_back({"speech_disable_script",Name(uintptr_t(s)),a&0xFFFF}); }
inline int __fastcall SpAddSay(void* s,void*,uint32 ev,uint32 ctx,uint32 delay,float prob,uint32 ov,uint32 fa,uint32 fe) {
    trace.push_back({"speech_add_say_event",Name(uintptr_t(s)),ev,ctx,delay,F(prob),ov&0xFF,fa&0xFF,fe&0xFF}); return int(sayRet);
}
inline void __fastcall SpAddScript(void* s,void*,uint32 ev,uint32 script,uint32 ov,uint32 fa,uint32 fe) {
    trace.push_back({"speech_add_script_say_event",Name(uintptr_t(s)),ev,script,ov&0xFF,fa&0xFF,fe&0xFF});
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=25;
inline const std::array<Fn,NFNS> fns{{
    {"CPed","AttachPedToBike",0x5E7E60,0,-1,512},{"CPed","AttachPedToEntity",0x5E7CB0,0,-1,1024},{"CPed","DettachPedFromEntity",0x5E7EC0,0,-1,768},
    {"CPed","PositionAnyPedOutOfCollision",0x5E13C0,0,-1,256},{"CPed","PositionAttachedPed",0x5DFDF0,0,-1,1024},{"CPed","UpdatePosition",0x5E1B10,0,-1,1024},
    {"CPed","Teleport",0,0x86C358,14,256},{"CPed","SetPedPositionInCar",0x5DF910,0,-1,1024},{"CPed","GetBonePosition-Original",0x5E4280,0,-1,512},
    {"CPed","GetTransformedBonePosition",0x5E01C0,0,-1,512},{"CPed","CanSeeEntity",0x5E0730,0,-1,1024},{"CPed","OurPedCanSeeThisEntity",0x5E1660,0,-1,512},
    {"CPed","DoesLOSBulletHitPed",0x5F01A0,0,-1,256},{"CPed","IsPedHeadAbovePos",0x5F02C0,0,-1,256},{"CPed","ReleaseCoverPoint",0x5E0270,0,-1,64},
    {"CPed","Say",0x5EFFE0,0,-1,256},{"CPed","SayScript",0x5EFFB0,0,-1,128},{"CPed","DisablePedSpeech",0x5EFF60,0,-1,64},
    {"CPed","DisablePedSpeechForScriptSpeech",0x5EFF80,0,-1,64},{"CPed","EnablePedSpeech",0x5EFF70,0,-1,64},{"CPed","EnablePedSpeechForScriptSpeech",0x5EFF90,0,-1,64},
    {"CPed","GetPedTalking",0x5EFF50,0,-1,64},{"CPed","CanPedHoldConversation",0x5EFFA0,0,-1,64},{"CPed","PedIsInvolvedInConversation",0x43AB90,0,-1,64},
    {"CPed","PedIsReadyForConversation",0x43ABA0,0,-1,512},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Obj()); clean(Veh());
}
// referencia registrada en un campo del objeto probado (como si la hubiera creado el juego)
inline void RegisterAt(void* o,size_t off,CEntity* e) { At<CEntity*>(o,off)=e; if(e)e->RegisterReference(&At<CEntity*>(o,off)); }
// matriz de rotacion con posicion (solo los 0x40 bytes de la matriz: no toca el enlace/RwMatrix)
inline void RandomMatrix(void* dst,float yaw,float pitch,float roll,const CVector& pos) {
    CMatrix m{}; m.SetRotate(pitch,roll,yaw); m.GetPosition()=pos; std::memcpy(dst,&m,0x40);
}
// comprobacion de la disposicion de campos de la DLL frente al EXE (solo direcciones, sin leer nada)
inline std::string LayoutErrors() {
    std::string why;
    auto* p=PedA(); auto* v=reinterpret_cast<CBmx*>(veh.data()); auto* a=reinterpret_cast<CAutomobile*>(veh.data());
    auto* pd=reinterpret_cast<CPlayerPedData*>(pdataA.data()); auto* w=reinterpret_cast<CWanted*>(wanted.data());
    using VStruct=std::remove_pointer_t<decltype(CVehicleModelInfo::m_pVehicleStruct)>;
    auto* mi=reinterpret_cast<CVehicleModelInfo*>(vinfoA.data()); auto* vs=reinterpret_cast<VStruct*>(vstructA.data());
    auto* bf=reinterpret_cast<AnimBlendFrameData*>(boneFrame.data()); auto* ig=reinterpret_cast<CPedIntelligence*>(intel.data());
    auto* h=reinterpret_cast<tHandlingData*>(handling.data());
    auto chk=[&](const char* nm,const void* addr,const void* base,size_t off){ if(uintptr_t(addr)-uintptr_t(base)!=off){ why+=nm; why+=' '; } };
    chk("m_pDamageEntity",&p->m_pDamageEntity,p,E_DAMAGE); chk("m_pAttachedTo",&p->m_pAttachedTo,p,E_ATTACHED); chk("m_pEntityIgnoredCollision",&p->m_pEntityIgnoredCollision,p,E_IGNORED);
    chk("m_nContactSurface",&p->m_nContactSurface,p,E_CONTACTSURF); chk("m_nAnimGroup",&p->m_nAnimGroup,p,P_ANIMGROUP);
    chk("m_apBones",&p->m_apBones,p,P_BONES); chk("m_nPedState",&p->m_nPedState,p,P_STATE); chk("m_nMoveState",&p->m_nMoveState,p,P_MOVESTATE);
    chk("m_vecAnimMovingShift",&p->m_vecAnimMovingShift,p,P_SHIFT); chk("m_fCurrentRotation",&p->m_fCurrentRotation,p,P_ROT); chk("m_fAimingRotation",&p->m_fAimingRotation,p,P_AIMROT);
    chk("m_standingOnEntity",&p->m_standingOnEntity,p,P_STANDING); chk("field_56C",&p->field_56C,p,P_F56C); chk("field_578",&p->field_578,p,P_F578);
    chk("m_pVehicle",&p->m_pVehicle,p,P_VEHICLE); chk("m_nPedType",&p->m_nPedType,p,P_PEDTYPE); chk("m_aWeapons",&p->m_aWeapons,p,P_WEAPONS);
    chk("m_nSavedWeapon",&p->m_nSavedWeapon,p,P_SAVEDW); chk("m_nActiveWeaponSlot",&p->m_nActiveWeaponSlot,p,P_WSLOT); chk("m_vecTurretOffset",&p->m_vecTurretOffset,p,P_TOFF);
    chk("m_fTurretAngleA",&p->m_fTurretAngleA,p,P_TA); chk("m_fTurretAngleB",&p->m_fTurretAngleB,p,P_TB); chk("m_nTurretPosnMode",&p->m_nTurretPosnMode,p,P_TMODE);
    chk("m_nTurretAmmo",&p->m_nTurretAmmo,p,P_TAMMO); chk("m_pCoverPoint",&p->m_pCoverPoint,p,P_COVER); chk("m_vecMoveSpeed",&p->m_vecMoveSpeed,p,E_MOVE); chk("m_vecTurnSpeed",&p->m_vecTurnSpeed,p,E_TURN);
    chk("pd.m_pWanted",&pd->m_pWanted,pd,0); chk("pd.m_nChosenWeapon",&pd->m_nChosenWeapon,pd,0x20); chk("pd.m_nPlayerFlags",&pd->m_nPlayerFlags,pd,0x34);
    chk("w.m_WantedLevel",&w->m_WantedLevel,w,0x2C);
    chk("v.m_pDriver",&v->m_pDriver,v,V_DRIVER); chk("v.m_apPassengers",&v->m_apPassengers,v,V_PASS); chk("v.m_nVehicleType",&v->m_nVehicleType,v,V_TYPE);
    chk("v.m_nVehicleSubType",&v->m_nVehicleSubType,v,V_SUBTYPE); chk("v.m_pHandlingData",&v->m_pHandlingData,v,V_HANDLING); chk("v.m_fControlJump",&v->m_fControlJump,v,V_BMXJUMP);
    chk("v.m_mLeanMatrix",&v->m_mLeanMatrix,v,V_LEAN); chk("a.m_aCarNodes",&a->m_aCarNodes,a,V_NODES); chk("a.m_fDoomVerticalRotation",&a->m_fDoomVerticalRotation,a,V_DOOM);
    chk("mi.m_nVehicleType",&mi->m_nVehicleType,mi,0x3C); chk("mi.m_pVehicleStruct",&mi->m_pVehicleStruct,mi,0x5C); chk("vs.m_avDummyPos",&vs->m_avDummyPos,vs,0);
    chk("frame.BoneTag",&bf->BoneTag,bf,0x14); chk("intel.m_TaskMgr",&ig->m_TaskMgr,ig,4); chk("intel.m_eventGroup",&ig->m_eventGroup,ig,0x68);
    chk("handling.m_nAnimGroup",&h->m_nAnimGroup,h,0xDE);
    chk("v.matrix",&reinterpret_cast<CEntity*>(veh.data())->m_matrix,veh.data(),E_MATRIX); chk("e.m_nModelIndex",&p->m_nModelIndex,p,E_MODEL);
    return why;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpede-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpede-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpede");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"cpede-only.txt"); if(f)std::getline(f,only); }
    InitBufs();
    if(auto bad=LayoutErrors(); !bad.empty()){Write({{"event","error"},{"reason","layout mismatch"},{"fields",bad}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const int16 pedModel=player->m_nModelIndex;
    // cuerpo real de AddScriptSayEvent en la DLL (la llamada de CPed::SayScript va directa al miembro): se lee de la vtable con el hook activo
    uintptr_t dllAddScript=0;
    {
        auto* h=FindHook("CAEPedSpeechAudioEntity","AddScriptSayEvent");
        if(h){ const bool en=h->Hooked(), lk=h->Locked(); h->LockState(false); h->State(true);
            dllAddScript=Body(AtA<uintptr_t>(0x85F310+4)); h->State(en); h->LockState(lk); }
    }
    FillPTraps(std::make_integer_sequence<int,64>{}); FillSTraps(std::make_integer_sequence<int,16>{}); FillTTraps(std::make_integer_sequence<int,16>{});
    taskVtbl[4]=reinterpret_cast<void*>(&TType); speechVtbl[1]=reinterpret_cast<void*>(&SpAddScript);
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura antes de cada ejecucion y al final
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> saved;
    auto snap=[&](uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); };
    snap(ADDR_REPLAY,1); snap(ADDR_TIME_STEP,4); snap(ADDR_CONV_PED,4); snap(ADDR_MODELINFOS+4*VEH_MODEL_A,4); snap(ADDR_MODELINFOS+4*VEH_MODEL_B,4);
    auto restoreGlobals=[&]{ for(auto& [a,b]:saved)std::memcpy(reinterpret_cast<void*>(a),b.data(),b.size()); };
    auto globalsIntact=[&]{ for(auto& [a,b]:saved)if(std::memcmp(reinterpret_cast<void*>(a),b.data(),b.size()))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        std::vector<std::unique_ptr<EntryPatch>> patches; std::vector<uintptr_t> seen;
        auto add=[&](uintptr_t exeAddr,auto fake,uintptr_t dllSym){
            for(uintptr_t at:{exeAddr,dllSym ? Body(dllSym) : uintptr_t(0)}) { if(!at||std::find(seen.begin(),seen.end(),at)!=seen.end())continue; seen.push_back(at); patches.push_back(std::make_unique<EntryPatch>(at,fake)); }
        };
        add(0x563220,WAdd,bit_cast<uintptr_t>(&CWorld::Add)); add(0x563280,WRemove,bit_cast<uintptr_t>(&CWorld::Remove));
        add(0x601640,FlushStub,bit_cast<uintptr_t>(&CPedIntelligence::FlushImmediately));
        add(0x681740,FindActiveStub,bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType));
        add(0x601070,SwimStub,bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim)); add(0x601110,JetStub,bit_cast<uintptr_t>(&CPedIntelligence::GetTaskJetPack));
        add(0x600F30,FightStub,bit_cast<uintptr_t>(&CPedIntelligence::GetTaskFighting)); add(0x600EE0,FindTaskStub,bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType));
        add(0x4AB650,EventOfTypeStub,bit_cast<uintptr_t>(&CEventGroup::GetEventOfType));
        add(0x532B20,UpdRpHAnim,bit_cast<uintptr_t>(&CEntity::UpdateRpHAnim));
        add(0x734A40,HierStub,bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump));
        add(0x7C51A0,IdIndexStub,0); add(0x7C5120,MatArrayStub,0); add(0x7EDD90,XformPointsStub,0); add(0x7F0990,LtmStub,0);
        add(0x5E6080,GiveWeaponStub,bit_cast<uintptr_t>(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)));
        add(0x5E61F0,SetCurWeaponStub,bit_cast<uintptr_t>(static_cast<void(CPed::*)(int32)>(&CPed::SetCurrentWeapon)));
        add(0x60B460,MakeChangesStub,bit_cast<uintptr_t>(static_cast<void(CPlayerPed::*)(eWeaponType)>(&CPlayerPed::MakeChangesForNewWeapon)));
        add(0x609C80,ClearTargetStub,bit_cast<uintptr_t>(&CPlayerPed::ClearWeaponTarget));
        add(0x50BFB0,CamModeStub,bit_cast<uintptr_t>(&CCamera::SetNewPlayerWeaponMode));
        add(0x4D4610,Blend,bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)));
        add(0x5429F0,MoveForceStub,bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)));
        add(0x6B7150,LeanStub,bit_cast<uintptr_t>(&CBike::CalculateLeanMatrix));
        add(0x542CE0,GetSpeedStub,bit_cast<uintptr_t>(&CPhysical::GetSpeed));
        add(0x55E770,SteepStub,bit_cast<uintptr_t>(&SurfaceInfos_c::IsSteepSlope));
        add(0x569E20,SphStub,bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld)); add(0x56BA00,LosStub,bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight));
        add(0x616920,FindZExe,0); if(const auto b=Body(bit_cast<uintptr_t>(&CPedPlacement::FindZCoorForPed)); std::find(seen.begin(),seen.end(),b)==seen.end()){ seen.push_back(b); patches.push_back(std::make_unique<EntryPatch>(b,FindZDll)); }
        add(0x535300,ColStub,bit_cast<uintptr_t>(&CEntity::GetColModel));
        add(0x698EF0,CoverStub,bit_cast<uintptr_t>(&CCoverPoint::ReleaseCoverPointForPed));
        add(0x56E210,FindPlayerStub,bit_cast<uintptr_t>(&FindPlayerPed));
        add(0x4E3F50,SpGetTalking,bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::GetPedTalking)); add(0x4E69E0,SpCanHold,bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::CanPedHoldConversation));
        add(0x4E3F70,SpEnable,bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::EnablePedSpeech)); add(0x4E3F90,SpEnableScript,bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::EnablePedSpeechForScriptSpeech));
        add(0x4E56D0,SpDisable,bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::DisablePedSpeech)); add(0x4E5700,SpDisableScript,bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::DisablePedSpeechForScriptSpeech));
        add(0x4E6550,SpAddSay,bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::AddSayEvent));
        if(dllAddScript)add(0,SpAddScript,dllAddScript);
        static const int32 bonePool[]{0,1,2,3,4,5,6,7,8,21,22,23,24,25,26,31,32,33,34,35,36,41,42,43,44,51,52,53,54,201,301,302};
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                // valor "interesante": 0, +-1, pequeno, o uniforme
                auto sp=[&](float lo,float hi){ const uint32 m=next()%10; return m==0 ? 0.f : m==1 ? 1.f : m==2 ? -1.f : m==3 ? uni(-0.01f,0.01f) : uni(lo,hi); };
                auto run=[&](bool candidate)->json {
                    restoreGlobals();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7C7Cu; if(!st)st=1; trace=json::array();
                    stubSt=st^0x5A5A1234u; if(!stubSt)stubSt=7;
                    CleanRefs();
                    // --- configuracion por caso de los falsos ---
                    sphMode=int(next()%4); sphSeed=next(); vehSeen=false; losSeed=next(); losMode=int(next()%4);
                    hierNull=next()%10==0; steepRet=next()%2; leaveCarRet=next()%2; fightRet=next()%6==0; findTaskRet=next()%6==0; eventRet=next()%6==0;
                    swimRet=next()%5==0; jetRet=next()%5==0; sayRet=next(); giveSlot=next()%13;
                    { static const int types[]{0x6C,0x6C,0x6C,5,100,1200,704,0}; primTaskType=types[next()%8]; }
                    colMax=CVector{uni(0.f,3.f),uni(0.f,3.f),next()%5 ? uni(0.f,3.f) : 0.f};
                    AtA<uint8>(ADDR_REPLAY)=uint8(next()%24==0 ? 1 : 0);
                    AtA<float>(ADDR_TIME_STEP)=next()%3 ? 1.f : uni(0.1f,3.f);
                    // --- buffers ---
                    for(auto* b:{&pedA,&pedB})std::memset(b->data(),0xCD,b->size());
                    for(auto* b:{&matA,&matB,&matV,&matO,&matLean})std::memset(b->data(),0,b->size());
                    std::memset(obj.data(),0xCD,obj.size()); std::memset(veh.data(),0xCD,veh.size());
                    for(auto* b:{&handling,&vinfoA,&vinfoB,&vstructA,&vstructB,&task,&frameBuf,&colModel,&colPoint,&hierBuf,&ltmBuf,&rwMatNode,&outVec,&vecBuf,&coverBuf,&weaponBuf,&boneFrame})
                        for(auto& x:*b)x=uint8(next());
                    for(auto& x:boneMats)x=uint8(next());
                    for(auto& x:intel)x=0; for(auto& x:pdataA)x=uint8(next()); for(auto& x:pdataB)x=uint8(next()); for(auto& x:wanted)x=uint8(next());
                    std::memset(clump.data(),0,clump.size()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    *reinterpret_cast<void**>(task.data())=taskVtbl.data();
                    for(size_t k=0;k<NBONE_MATS*16;++k)At<float>(boneMats.data(),4*k)=uni(-2.f,2.f);
                    for(size_t k=0;k<16;++k){ At<float>(ltmBuf.data(),4*k)=uni(-2.f,2.f); At<float>(rwMatNode.data(),4*k)=uni(-2.f,2.f); }
                    At<float>(colModel.data(),0)=-colMax.x; At<float>(colModel.data(),4)=-colMax.y; At<float>(colModel.data(),8)=-colMax.z;
                    At<float>(colModel.data(),0xC)=colMax.x; At<float>(colModel.data(),0x10)=colMax.y; At<float>(colModel.data(),0x14)=colMax.z;
                    // --- entidades ---
                    auto* A=PedA(); auto* B=PedB(); auto* O=Obj(); auto* V=Veh();
                    auto fillMat=[&](void* m,const CVector& pos){
                        if(next()%4==0){ for(size_t k=0;k<12;++k)At<float>(m,4*k)=uni(-1.5f,1.5f); At<float>(m,0x30)=pos.x; At<float>(m,0x34)=pos.y; At<float>(m,0x38)=pos.z; }
                        else RandomMatrix(m,uni(-3.2f,3.2f),next()%3==0 ? 0.f : uni(-0.8f,0.8f),next()%3==0 ? 0.f : uni(-0.8f,0.8f),pos);
                    };
                    auto randPos=[&]{ return CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; };
                    auto setPos=[&](CEntity* e,void* matBuf,bool useMatrix){ // matriz propia o posicion compacta
                        const CVector p=randPos();
                        if(useMatrix){ fillMat(matBuf,p); At<void*>(e,E_MATRIX)=matBuf; }
                        else { At<void*>(e,E_MATRIX)=nullptr; e->SetPosn(p); At<float>(e,0x10)=uni(-3.f,3.f); }
                    };
                    const uint32 pedType=next()%3==0 ? next()%2 : 2+next()%20; // 0/1 = jugador
                    for(auto* p:{A,B}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,E_CLUMP)=clump.data();
                        At(p,E_FLAGS)=next(); At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next();
                        At<int16>(p,E_MODEL)=pedModel; At(p,E_PHYS)=next(); At<void*>(p,E_ATTACHED)=nullptr; At<void*>(p,E_DAMAGE)=nullptr; At<void*>(p,E_IGNORED)=nullptr;
                        At<void*>(p,P_INTEL)=intel.data(); At<void*>(p,P_PLAYERDATA)=p==A ? pdataA.data() : pdataB.data();
                        At<void*>(p,P_VEHICLE)=nullptr; At<void*>(p,P_STANDING)=nullptr; At<void*>(p,P_COVER)=nullptr;
                        At<int32>(p,P_PEDTYPE)=int32(p==A ? pedType : 2); At(p,P_CREATEDBY)=next()%4; At<uint8>(p,P_WSLOT)=uint8(next()%13);
                        for(size_t k=0;k<13*7;++k)At<uint32>(p,P_WEAPONS+4*k)=next()%47;
                        for(size_t k=0;k<19;++k)At<void*>(p,P_BONES+4*k)=boneFrame.data();
                        At<float>(p,P_ROT)=uni(-7.f,7.f); At<float>(p,P_AIMROT)=uni(-7.f,7.f);
                    }
                    At<void*>(A,P_BONES+8)=boneFrame.data(); At<uint32>(boneFrame.data(),0x14)=uint32(bonePool[next()%32]);
                    O->SetType(ENTITY_TYPE_OBJECT); At<int16>(O,E_MODEL)=int16(next()%2 ? MODEL_ATTACH_POOLCUE : 100+next()%50);
                    At(O,E_FLAGS)=next(); At<uint32>(O,E_PHYS)=next(); At<void*>(O,E_ATTACHED)=nullptr;
                    V->SetType(ENTITY_TYPE_VEHICLE); At(V,E_FLAGS)=next(); At<uint32>(V,E_PHYS)=next(); At<void*>(V,E_ATTACHED)=nullptr;
                    At<int16>(V,E_MODEL)=int16(next()%2 ? VEH_MODEL_A : VEH_MODEL_B);
                    // la lista de referencias no puede quedar con el relleno 0xCD (RegisterReference/CleanUpOldReference la recorren)
                    for(auto* e:{static_cast<CEntity*>(A),static_cast<CEntity*>(B),O,V})e->m_pReferences=nullptr;
                    // matrices siempre presentes (GetMatrix/SetMatrix reservarian matrices del pool real); las posiciones compactas se usan donde es seguro
                    setPos(A,matA.data(),true); setPos(B,matB.data(),true); setPos(O,matO.data(),true); setPos(V,matV.data(),true);
                    auto placement=[&](CEntity* e){ const CVector p=e->GetPosition(); At<void*>(e,E_MATRIX)=nullptr; e->SetPosn(p); At<float>(e,0x10)=uni(-3.f,3.f); };
                    for(auto* e:{static_cast<CEntity*>(O),static_cast<CEntity*>(V),static_cast<CEntity*>(B)}) for(size_t k=0;k<3;++k){ At<float>(e,E_MOVE+4*k)=sp(-2.f,2.f); At<float>(e,E_TURN+4*k)=sp(-2.f,2.f); }
                    for(size_t k=0;k<3;++k){ At<float>(A,E_MOVE+4*k)=sp(-2.f,2.f); At<float>(A,E_TURN+4*k)=sp(-2.f,2.f); }
                    // vehiculo: tipo, handling, modelo falso, ocupantes
                    { static const int32 vt[]{0,0,9,5,2,10,1,3,9,0}; static const int32 vs[]{0,0,2,10,2,0,5,9,0,10};
                      At<int32>(V,V_TYPE)=vt[next()%10]; At<int32>(V,V_SUBTYPE)=vs[next()%10]; }
                    At<void*>(V,V_HANDLING)=handling.data(); handling[0xDE]=uint8(next()%2 ? 13 : next()%30);
                    At<void*>(V,V_DRIVER)=nullptr; for(size_t k=0;k<8;++k)At<void*>(V,V_PASS+4*k)=nullptr;
                    At<float>(V,V_BMXJUMP)=uni(-300.f,300.f); At<float>(V,V_DOOM)=uni(-2.f,2.f); At<int8>(V,0x48B)=int8(-1);
                    for(size_t k=0;k<16;++k)At<float>(V,V_LEAN+4*k)=uni(-1.f,1.f);
                    At<void*>(V,V_LEAN+0x40)=nullptr; At<uint32>(V,V_LEAN+0x44)=0;
                    for(size_t k=0;k<0x40;++k)At<void*>(V,V_NODES+4*k)=nullptr;
                    At<void*>(V,V_NODES+4)=next()%4 ? frameBuf.data() : nullptr; At<void*>(V,V_NODES+0x54)=next()%3 ? frameBuf.data() : nullptr; // CAR_CHASSIS y CAR_MISC_B
                    for(auto* vi:{&vinfoA,&vinfoB}){ At<int32>(vi->data(),0x3C)=next()%3==0 ? 5 : int32(next()%12); At<void*>(vi->data(),0x5C)= vi==&vinfoA ? vstructA.data() : vstructB.data(); }
                    for(auto* vs:{&vstructA,&vstructB})for(size_t k=0;k<30;++k)At<float>(vs->data(),4*k)=uni(-1.5f,1.5f);
                    AtA<void*>(ADDR_MODELINFOS+4*VEH_MODEL_A)=vinfoA.data(); AtA<void*>(ADDR_MODELINFOS+4*VEH_MODEL_B)=vinfoB.data();
                    std::memset(intel.data(),0,intel.size());
                    At<void*>(intel.data(),0x10)=next()%3 ? task.data() : nullptr; // m_TaskMgr.m_aPrimaryTasks[3]
                    At<void*>(pdataB.data(),0)=wanted.data(); At<void*>(pdataA.data(),0)=wanted.data(); At<uint32>(wanted.data(),0x2C)=next()%3==0 ? next()%7 : 0;
                    for(size_t k=0;k<3;++k)At<float>(colPoint.data(),4*k)=uni(-2.f,2.f);
                    AtA<CPed*>(ADDR_CONV_PED)=next()%2 ? A : (next()%2 ? B : nullptr);
                    // la entidad de voz falsa dentro del peatón
                    for(auto* p:{A,B}) *reinterpret_cast<void**>(static_cast<uint8*>(static_cast<void*>(p))+P_SPEECH)=speechVtbl.data();
                    // --- argumentos y fixture por funcion ---
                    uintptr_t rawRet=0; json ret=nullptr; bool isVoid=false; bool compareOut=false;
                    uintptr_t target=fn.address; // las virtuales leen la ranura DESPUES de h->State(candidate)
                    bool aNullOk=false; // el peatón puede quedar sin matriz (posicion compacta) donde el EXE/DLL no reservan matriz
                    std::function<void()> call;
                    // estado de enganche: variantes de entidad
                    CEntity* const attachables[]{V,O,B};
                    auto setupAttached=[&](bool allowNull){
                        CEntity* e=(allowNull&&next()%12==0) ? nullptr : attachables[next()%3];
                        At<CEntity*>(A,E_ATTACHED)=e;
                        if(e){ At<int16>(e,E_MODEL)=int16(e==V&&next()%3==0 ? MODEL_ATTACH_FIRELA : (next()%2 ? 100+next()%100 : MODEL_ATTACH_POOLCUE));
                            if(e==V&&At<int16>(e,E_MODEL)==MODEL_ATTACH_FIRELA)At<void*>(e,V_NODES+0x54)=next()%4 ? frameBuf.data() : nullptr; }
                        { const uint32 m=next()%5; At<float>(A,P_TOFF)=sp(-3.f,3.f); At<float>(A,P_TOFF+4)=sp(-3.f,3.f); At<float>(A,P_TOFF+8)= m==0 ? uni(-1500.f,-901.f) : sp(-3.f,3.f); }
                        { static const uint16 angles[]{0,1,2,3,4,0xFFFF,1,2,3}; At<uint16>(A,P_TA)=angles[next()%9]; At<float>(A,P_TB)=next()%6==0 ? 0.f : uni(0.f,3.2f); }
                        At<float>(A,P_ROT)=next()%3==0 ? uni(-12.f,12.f) : uni(-3.5f,3.5f);
                    };
                    const std::string nmf=nm;
                    if(nmf=="AttachPedToEntity"||nmf=="AttachPedToBike"){
                        // peatón: jugador o no, dentro o fuera del vehiculo, con o sin arma guardada
                        At<uint32>(A,P_FLAGS1)=(At<uint32>(A,P_FLAGS1)&~0x100u)|(next()%6==0 ? 0x100u : 0u);
                        At<int32>(A,P_SAVEDW)=next()%2 ? 0x37 : int32(next()%47); At<int32>(A,P_TAMMO)=int32(next());
                        CEntity* e=next()%10==0 ? nullptr : attachables[next()%3];
                        if(e){ At<int16>(e,E_MODEL)=int16(e==V&&next()%3==0 ? MODEL_ATTACH_FIRELA : (next()%2 ? MODEL_ATTACH_POOLCUE : 100+next()%100)); }
                        if(e==V&&At<int16>(V,E_MODEL)==MODEL_ATTACH_FIRELA)At<void*>(V,V_NODES+0x54)=next()%3 ? frameBuf.data() : nullptr;
                        const CVector off{sp(-3.f,3.f),sp(-3.f,3.f),next()%5==0 ? uni(-1500.f,-901.f) : sp(-3.f,3.f)};
                        static const uint16 angs[]{0,1,2,3,4,0xFFFF}; const uint32 ta=next()%2 ? angs[next()%6] : (next()&0xFFFF);
                        const float tb=next()%6==0 ? 0.f : uni(0.f,3.2f), tmode=sp(-2.f,2.f);
                        static const uint32 weapons[]{0,43,43,22,29,30,35,46,17,1,2,10,38};
                        const uint32 weapon=weapons[next()%13];
                        At<uint8>(A,P_WSLOT)=uint8(next()%13);
                        if(nmf=="AttachPedToEntity"){
                            call=[&]{ rawRet=uintptr_t(reinterpret_cast<CEntity*(__thiscall*)(CPed*,CEntity*,CVector,uint32,float,uint32)>(target)(A,e,off,ta,tb,weapon)); };
                        } else { isVoid=true;
                            call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*,CEntity*,CVector,uint32,float,float,uint32)>(target)(A,e,off,ta,tb,tmode,weapon); };
                        }
                    }
                    else if(nmf=="DettachPedFromEntity"){
                        isVoid=true; setupAttached(true);
                        { static const uint32 states[]{0x36,0x37,1,7,12,0x36,0x37,0x2A}; At<uint32>(A,P_STATE)=next()%4 ? states[next()%8] : next()%60; }
                        At<int32>(A,P_SAVEDW)=next()%2 ? 0x37 : int32(next()%47); At<int32>(A,P_TAMMO)=int32(next());
                        At<uint8>(A,P_WSLOT)=uint8(next()%13); At<int32>(A,P_ANIMGROUP)=int32(next()%100);
                        if(!At<CEntity*>(A,E_ATTACHED)) { /* el EXE usa la entidad tras anularla: sin entidad solo el estado DEAD no la usa */ At<uint32>(A,P_STATE)=0x37; }
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*)>(target)(A); };
                    }
                    else if(nmf=="PositionAnyPedOutOfCollision"){
                        aNullOk=true;
                        call=[&]{ ret=(reinterpret_cast<uint32(__thiscall*)(CPed*)>(target)(A)&0xFF)!=0; };
                    }
                    else if(nmf=="PositionAttachedPed"){
                        isVoid=true; setupAttached(true);
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*)>(target)(A); };
                    }
                    else if(nmf=="UpdatePosition"){
                        isVoid=true; aNullOk=true;
                        { static const uint32 states[]{0x36,0x37,1,7,12,0x2A}; At<uint32>(A,P_STATE)=next()%3 ? states[next()%6] : next()%60; }
                        if(next()%3==0)At<void*>(A,E_ATTACHED)=B;
                        // peatón de pie o no
                        { uint32 f1=At<uint32>(A,P_FLAGS1); f1=(f1&~1u)|(next()%4!=0 ? 1u : 0u); At<uint32>(A,P_FLAGS1)=f1; }
                        At<float>(A,P_SHIFT)=sp(-0.3f,0.3f); At<float>(A,P_SHIFT+4)=sp(-0.3f,0.3f);
                        At<float>(A,E_MOVE)=sp(-0.3f,0.3f); At<float>(A,E_MOVE+4)=sp(-0.3f,0.3f);
                        { const uint32 m=next()%6; At<float>(A,P_F578)= m==0 ? 0.f : sp(-1.f,1.f); At<float>(A,P_F578+4)= m==1||m==0 ? 0.f : sp(-1.f,1.f); At<float>(A,P_F578+8)=sp(-1.f,1.f); }
                        for(size_t k=0;k<3;++k)At<float>(A,P_F56C+4*k)=sp(-3.f,3.f);
                        At<uint8>(A,E_CONTACTSURF)=uint8(next()%180);
                        // lo que hay bajo los pies
                        CEntity* standing[]{nullptr,nullptr,V,V,O,B};
                        CEntity* so=standing[next()%6]; At<CEntity*>(A,P_STANDING)=so;
                        if(so){ At<uint32>(so,E_PHYS)=next(); At<void*>(so,E_ATTACHED)=next()%6==0 ? static_cast<void*>(O) : nullptr;
                            if(so==V){ static const int32 vt[]{5,5,9,0,0,2,10,1}; static const int32 vs[]{0,2,0,10}; At<int32>(V,V_TYPE)=vt[next()%8]; At<int32>(V,V_SUBTYPE)=vs[next()%4]; }
                            { uint32 f=At<uint32>(so,E_PHYS); if(next()%3==0)f|=4u; if(next()%2)f|=8u; else f&=~8u; At<uint32>(so,E_PHYS)=f; } }
                        if(so&&next()%3==0)placement(so);
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*)>(target)(A); };
                    }
                    else if(nmf=="Teleport"){
                        isVoid=true; aNullOk=true;
                        { CEntity* dam=next()%2 ? (next()%2 ? O : B) : nullptr; if(dam)RegisterAt(A,E_DAMAGE,dam); else At<void*>(A,E_DAMAGE)=nullptr; }
                        const CVector dest=randPos(); const uint32 reset=next()%2;
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*,CVector,uint32)>(target)(A,dest,reset); };
                    }
                    else if(nmf=="SetPedPositionInCar"){
                        isVoid=true;
                        At<void*>(A,P_VEHICLE)=V;
                        // ocupantes: el peatón es conductor, uno de los cuatro pasajeros o ninguno; el resto lo ocupa otro peatón a veces
                        { for(size_t k=0;k<5;++k)At<void*>(V,V_DRIVER+4*k)=next()%3==0 ? static_cast<void*>(B) : nullptr;
                          const uint32 seat=next()%8; if(seat<=4)At<void*>(V,V_DRIVER+4*seat)=A; }
                        At<uint32>(A,P_FLAGS1)|=0x100u;
                        if(next()%4==0){ At<int32>(V,V_TYPE)=9; }
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*)>(target)(A); };
                    }
                    else if(nmf=="GetBonePosition-Original"){
                        isVoid=true; compareOut=true;
                        At<uint32>(A,P_FLAGS3)=(At<uint32>(A,P_FLAGS3)&~0x400u)|(next()%2 ? 0x400u : 0u);
                        const uint32 bone=next()%2 ? uint32(bonePool[next()%32]) : next()%303, upd=next()%2;
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*,CVector*,uint32,uint32)>(target)(A,reinterpret_cast<CVector*>(outVec.data()),bone,upd); };
                    }
                    else if(nmf=="GetTransformedBonePosition"){
                        isVoid=true; compareOut=true;
                        At<uint32>(A,P_FLAGS3)=(At<uint32>(A,P_FLAGS3)&~0x400u)|(next()%2 ? 0x400u : 0u);
                        for(size_t k=0;k<3;++k)At<float>(outVec.data(),4*k)=uni(-3.f,3.f);
                        const uint32 bone=next()%2 ? uint32(bonePool[next()%32]) : next()%303, upd=next()%2;
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*,RwV3d*,uint32,uint32)>(target)(A,reinterpret_cast<RwV3d*>(outVec.data()),bone,upd); };
                    }
                    else if(nmf=="CanSeeEntity"){
                        aNullOk=true; CEntity* e=next()%2 ? O : static_cast<CEntity*>(B); if(next()%3==0)placement(e);
                        { const uint32 m=next()%5; At<float>(A,P_ROT)= m==0 ? uni(-12.f,12.f) : m==1 ? uni(6.2f,6.4f) : m==2 ? uni(-0.1f,0.1f) : uni(-3.5f,7.f); }
                        // entidad a veces justo en la direccion del peatón
                        const float lim=next()%5==0 ? sp(0.f,3.2f) : uni(0.f,4.f);
                        call=[&]{ ret=(reinterpret_cast<uint32(__thiscall*)(CPed*,CEntity*,float)>(target)(A,e,lim)&0xFF)!=0; };
                    }
                    else if(nmf=="OurPedCanSeeThisEntity"){
                        CEntity* e[]{O,B,V}; CEntity* ent=e[next()%3];
                        if(next()%3==0){ const CVector d=A->GetPosition(); const float ang=uni(-3.2f,3.2f), dist=next()%3==0 ? uni(39.f,41.f) : uni(0.f,60.f); ent->SetPosn(d.x+dist*std::sin(ang),d.y+dist*std::cos(ang),d.z+sp(-3.f,3.f)); }
                        if(next()%3==0)placement(ent);
                        const uint32 spotted=next()%2;
                        call=[&]{ ret=(reinterpret_cast<uint32(__thiscall*)(CPed*,CEntity*,uint32)>(target)(A,ent,spotted)&0xFF)!=0; };
                    }
                    else if(nmf=="DoesLOSBulletHitPed"||nmf=="IsPedHeadAbovePos"){
                        At<uint32>(A,P_STATE)=next()%4==0 ? 0x2A : next()%60; aNullOk=nmf=="IsPedHeadAbovePos";
                        const float headZ=[&]{ float in[3]{0,0,0},o[3]; const int idx=int(uint32(At<uint32>(boneFrame.data(),0x14))*7u%NBONE_MATS); XformPoint(in,reinterpret_cast<const float*>(boneMats.data()+0x40*idx),o); return o[2]; }();
                        if(nmf=="DoesLOSBulletHitPed"){
                            static const float deltas[]{-1.f,-0.001f,0.f,0.1f,0.2f,0.20000002f,0.19999999f,0.5f,3.f};
                            At<float>(colPoint.data(),8)= next()%2 ? headZ+deltas[next()%9] : uni(-3.f,45.f);
                            if(next()%40==0)At<uint32>(colPoint.data(),8)=0x7FC00000u;
                            call=[&]{ ret=reinterpret_cast<uint32(__thiscall*)(CPed*,void*)>(target)(A,colPoint.data())&0xFF; };
                        } else {
                            const float posZ=A->GetPosition().z; float z=next()%2 ? headZ-posZ+sp(-0.5f,0.5f) : uni(-3.f,3.f); if(next()%12==0)z=headZ-posZ;
                            call=[&]{ ret=(reinterpret_cast<uint32(__thiscall*)(CPed*,float)>(target)(A,z)&0xFF)!=0; };
                        }
                    }
                    else if(nmf=="ReleaseCoverPoint"){
                        isVoid=true; At<void*>(A,P_COVER)=next()%3 ? coverBuf.data() : nullptr;
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*)>(target)(A); };
                    }
                    else if(nmf=="Say"){
                        const uint32 ctx=next()%3==0 ? 0 : 1+next()%400; /* contextos reales: 0..0x16F (la DLL pasa el int16 extendido con signo) */
                        const uint32 delay=next()%2 ? 0 : next()%60000; const float prob=sp(0.f,100.f);
                        const uint32 b1=next()%2, b2=next()%2, b3=next()%2;
                        call=[&]{ ret=int(int16(reinterpret_cast<uint32(__thiscall*)(CPed*,uint32,uint32,float,uint32,uint32,uint32)>(target)(A,ctx,delay,prob,b1,b2,b3)&0xFFFF)); };
                    }
                    else if(nmf=="SayScript"){
                        isVoid=true; const uint32 script=next()%100, b1=next()%2, b2=next()%2, b3=next()%2;
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*,uint32,uint32,uint32,uint32)>(target)(A,script,b1,b2,b3); };
                    }
                    else if(nmf=="DisablePedSpeech"||nmf=="DisablePedSpeechForScriptSpeech"){
                        isVoid=true; const uint32 stop=next()%2;
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*,uint32)>(target)(A,stop); };
                    }
                    else if(nmf=="EnablePedSpeech"||nmf=="EnablePedSpeechForScriptSpeech"){
                        isVoid=true; call=[&]{ reinterpret_cast<void(__thiscall*)(CPed*)>(target)(A); };
                    }
                    else if(nmf=="GetPedTalking"||nmf=="CanPedHoldConversation"||nmf=="PedIsInvolvedInConversation"){
                        call=[&]{ ret=(reinterpret_cast<uint32(__thiscall*)(CPed*)>(target)(A)&0xFF)!=0; };
                    }
                    else if(nmf=="PedIsReadyForConversation"){
                        At<uint32>(A,P_FLAGS1)=(At<uint32>(A,P_FLAGS1)&~0x100u)|(next()%5==0 ? 0x100u : 0u);
                        At<uint32>(A,P_MOVESTATE)=next()%11;
                        const uint32 check=next()%2;
                        call=[&]{ ret=(reinterpret_cast<uint32(__thiscall*)(CPed*,uint32)>(target)(A,check)&0xFF)!=0; };
                    }
                    // --- ejecucion ---
                    if(aNullOk&&next()%6==0){ const CVector p=A->GetPosition(); At<void*>(A,E_MATRIX)=nullptr; A->SetPosn(p); At<float>(A,0x10)=uni(-3.f,3.f); }
                    setState(candidate);
                    bool switches{};
                    if(fn.slot>=0){ target=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]; switches=(target>=0x400000&&target<0x1000000)!=candidate; }
                    else switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    json pre={{"matA",Region(matA.data(),0x48)},{"matB",Region(matB.data(),0x40)},{"matV",Region(matV.data(),0x40)},{"matO",Region(matO.data(),0x40)},
                              {"ped_0x550",Region(pedA.data()+0x550,0x10)},{"turret",Region(pedA.data()+P_TOFF,0x18)},{"attached",Name(At<uintptr_t>(A,E_ATTACHED))},{"pedtype",At<uint32>(A,P_PEDTYPE)}};
                    call();
                    setState(false);
                    json r={{"ret",isVoid ? json(nullptr) : (ret.is_null() ? Name(rawRet) : ret)},
                            {"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"obj",EntHash(obj)},{"veh",EntHash(veh)},
                            {"pedA_words",Region(pedA.data(),pedA.size())},
                            {"matA",Hash(MaskMat(matA).data(),matA.size())},{"matA_words",Region(MaskMat(matA).data(),0x48)},{"pre",pre},{"matB",Hash(MaskMat(matB).data(),matB.size())},{"matV",Hash(MaskMat(matV).data(),matV.size())},{"matO",Hash(MaskMat(matO).data(),matO.size())},
                            {"intel",Hash(intel.data(),intel.size())},{"pdataA",Hash(pdataA.data(),pdataA.size())},{"pdataB",Hash(pdataB.data(),pdataB.size())},
                            {"wanted",Hash(wanted.data(),wanted.size())},{"out",compareOut ? Region(outVec.data(),0xC) : json(nullptr)},
                            {"others",{Hash(boneMats.data(),boneMats.size()),Hash(ltmBuf.data(),ltmBuf.size()),Hash(clump.data(),clump.size()),Hash(handling.data(),handling.size()),
                                       Hash(colPoint.data(),colPoint.size()),Hash(boneFrame.data(),boneFrame.size()),Hash(coverBuf.data(),coverBuf.size())}},
                            {"conv_ped",Name(AtA<uintptr_t>(ADDR_CONV_PED))},{"replay",AtA<uint8>(ADDR_REPLAY)},{"time_step",F(AtA<float>(ADDR_TIME_STEP))}};
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* q=e->m_pReferences;q;q=q->m_pNext)l.push_back(Name(uintptr_t(q->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),Obj(),Veh()})refs.push_back(refList(e));
                    r["refs"]=refs; r["trace"]=trace; r["switches"]=switches;
                    CleanRefs(); restoreGlobals();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs();
    const bool intactBefore=globalsIntact();
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",!intactBefore},
           {"state_restored",freeRefs()==savedFree&&globalsIntact()}});
}
}
