#pragma once
// Banco dentro del motor: tareas simples B (lista de 8, 32 funciones): CTaskSimpleJump, CTaskSimpleShakeFist,
// CTaskSimpleSitDown y CTaskSimpleStandUp.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: los constructores se llaman en la
// direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del vptr); los metodos virtuales se
// llaman por la entrada de la vtable del EXE. Peaton falso (vtable trazada, hueco 13 TestCollision; matriz propia,
// nodos de los pies, inteligencia y datos de jugador falsos), clump (byte de tipo 2), entidad fisica falsa (suelo o
// entidad de escalada), cuatro asociaciones de animacion con su jerarquia, modelo falso (CBaseModelInfo/CColModel/
// CCollisionData con esferas propias, colocado solo durante la llamada en un hueco vacio de ms_modelInfoPtrs),
// jerarquia RpHAnim y matrices falsas, y grupo de eventos falso. Se simulan y trazan: CAnimManager::BlendAnimation,
// RpAnimBlendClumpGetAssociation, CAnimBlendAssociation::SetFinishCallback/SetDeleteCallback,
// CStats::GetFatAndMuscleModifier, CPhysical::ApplyMoveForce, CLocalisation::Blood, GetAnimHierarchyFromSkinClump,
// RpHAnimIDGetIndex, RpHAnimHierarchyGetMatrixArray, RwV3dTransformPoints, CShadows::AddPermanentShadow,
// CEventHandler::GetCurrentEventType, CWorld::TestSphereAgainstWorld, CTaskSimpleClimb::TestForClimb,
// SurfaceInfos_c::IsSteepSlope, CGameLogic::IsPlayerAllowedToGoInThisDirection, GetEventGlobalGroup, CEventGroup::Add
// y el generador aleatorio (rand 0x821B1E y CGeneral::GetRandomNumber, solo para detectar usos). Se fijan y restauran
// el reloj, CGame::currArea, el truco de megasalto y el hueco de ms_modelInfoPtrs. Las tareas nuevas del pool se siguen
// (m_LastFreeSlot restaurado y huecos libres con 0xCD).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskTypes/TaskSimpleJump.h"
#include "Tasks/TaskTypes/TaskSimpleShakeFist.h"
#include "Tasks/TaskTypes/TaskSimpleSitDown.h"
#include "Tasks/TaskTypes/TaskSimpleStandUp.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "PedIntelligence.h"
#include "Events/EventGroup.h"
#include "Events/EventGlobalGroup.h"
#include "Events/EventHandler.h"
#include "Events/EventSoundQuiet.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Stats.h"
#include "Localisation.h"
#include "Shadows.h"
#include "World.h"
#include "GameLogic.h"
#include "SurfaceInfos_c.h"
#include "RwHelper.h"
#include "../ProbeRuntime.hpp"

namespace STasks8bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE (CPed / CPhysical / CEntity)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_FLAGS_ENT=0x1C, P_MODEL=0x22, P_PHYS_FLAGS=0x40, P_MOVESPEED=0x44, P_SURFACE=0xBA,
                 P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_PLAYERDATA=0x480,
                 P_LFOOT=0x4AC, P_RFOOT=0x4B0, P_IKFLAGS=0x528, P_ROT_CUR=0x558, P_ROT_AIM=0x55C, P_STANDING_ON=0x568,
                 P_FIELD578=0x578, P_TYPE=0x598, P_DEATH_TIME=0x750;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_CURR_AREA=0xB72914, ADDR_MEGAJUMP=0x96916C, ADDR_MODEL_PTRS=0xA9B0C8,
                    ADDR_SURFACE_INFOS=0xB79538, ADDR_BLOOD_TEX=0xC40400,
                    EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_JUMP_CB=0x67A020, EXE_FIST_CB=0x690C60, EXE_SITDOWN_CB=0x631150, EXE_STANDUP_CB=0x631400;
constexpr int32 NUM_MODEL_PTRS=20000;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{};
alignas(16) inline std::array<uint8,0x200> ent{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{};
alignas(16) inline std::array<uint8,0x80> clump{}, hier{}, playerData{}, group{}, rwHier{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,4> anims{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim2{};
alignas(16) inline std::array<uint8,0x40> object{};
alignas(16) inline std::array<uint8,2*0x18> frames{};
alignas(16) inline std::array<uint8,8*0x40> rwMats{};
alignas(16) inline std::array<uint8,0x60> modelInfo{}, colModel{}, colData{};
alignas(16) inline std::array<uint8,8*0x14> spheres{};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_JUMP_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleJump::JumpAnimFinishCB))return "JumpAnimFinishCB";
    if(v==EXE_FIST_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleShakeFist::FinishAnimShakeFistCB))return "FinishAnimShakeFistCB";
    if(v==EXE_SITDOWN_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleSitDown::FinishAnimSitDownCB))return "FinishAnimSitDownCB";
    if(v==EXE_STANDUP_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleStandUp::FinishAnimStandUpCB))return "FinishAnimStandUpCB";
    if(v==*reinterpret_cast<uintptr_t*>(ADDR_BLOOD_TEX))return "bloodtex";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    // direcciones de la pila (locales del EXE o de la DLL): no comparables, se marcan como "stack"
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(mat.data(),mat.size(),"mat"))return *s;
    if(auto s=in(ent.data(),ent.size(),"ent"))return *s; if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(hier.data(),hier.size(),"hier"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"playerdata"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(auto s=in(rwHier.data(),rwHier.size(),"rwhier"))return *s;
    for(size_t k=0;k<anims.size();++k)if(auto s=in(anims[k].data(),anims[k].size(),("anims"+std::to_string(k)).c_str()))return *s;
    if(auto s=in(anim2.data(),anim2.size(),"anim2"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(auto s=in(frames.data(),frames.size(),"frames"))return *s; if(auto s=in(rwMats.data(),rwMats.size(),"rwmats"))return *s;
    if(auto s=in(modelInfo.data(),modelInfo.size(),"modelinfo"))return *s; if(auto s=in(colModel.data(),colModel.size(),"colmodel"))return *s;
    if(auto s=in(colData.data(),colData.size(),"coldata"))return *s; if(auto s=in(spheres.data(),spheres.size(),"spheres"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}
inline json PedPos() { const auto* m=reinterpret_cast<const float*>(mat.data()); return json{F(m[12]),F(m[13]),F(m[14])}; }

// configuracion por caso de las dependencias simuladas
inline uint32 assocMask{}, rngState{}; inline int32 eventTypeRet{};
inline bool bloodRet{}, collisionRet{}, steepRet{}, allowedRet{};
inline float modifierRet{};
inline uintptr_t sphereRet{}, climbRet{};
inline std::array<uint32,5> climbOut{};

// peaton: vtable falsa (todo trazado; hueco 13 = TestCollision)
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
inline bool __fastcall TestCollision(CPed* p,void*,bool applySpeed) {
    json radii=json::array(); for(size_t k=0;k<8;++k)radii.push_back(At(spheres.data(),k*0x14+0xC));
    trace.push_back({"ped_test_collision",Name(uintptr_t(p)),applySpeed,PedPos(),radii}); return collisionRet;
}

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 grp,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),grp,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(anim2.data()); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) {
    trace.push_back({"clump_get_association",Name(uintptr_t(c)),id});
    int bit=-1, slot=0;
    switch(id){ case ANIM_ID_SPRINT: bit=0; slot=0; break; case ANIM_ID_RUN: bit=1; slot=1; break; case ANIM_ID_WALK: bit=2; slot=2; break;
                case ANIM_ID_JUMP_LAUNCH: bit=3; slot=3; break; case ANIM_ID_JUMP_LAUNCH_R: bit=4; slot=3; break; default: break; }
    return bit>=0&&((assocMask>>bit)&1) ? reinterpret_cast<CAnimBlendAssociation*>(anims[size_t(slot)].data()) : nullptr;
}
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetDeleteCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_delete_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline float __cdecl Modifier(int32 stat) { trace.push_back({"fat_and_muscle_modifier",stat}); return modifierRet; }
inline void __fastcall MoveForce(CPhysical* e,void*,float x,float y,float z) { trace.push_back({"apply_move_force",Name(uintptr_t(e)),F(x),F(y),F(z)}); }
inline bool __cdecl Blood() { trace.push_back({"localisation_blood"}); return bloodRet; }
inline void* __cdecl SkinHier(RpClump* c) { trace.push_back({"anim_hierarchy_from_skin_clump",Name(uintptr_t(c))}); return rwHier.data(); }
inline int32 __cdecl HAnimIndex(void* h,int32 id) { trace.push_back({"hanim_id_get_index",Name(uintptr_t(h)),id}); return int32(uint32(id)%8); }
inline void* __cdecl HAnimMatrices(void* h) { trace.push_back({"hanim_get_matrix_array",Name(uintptr_t(h))}); return rwMats.data(); }
inline CVector* __cdecl TransformPoints(CVector* out,const CVector* in,int32 n,const void* m) {
    trace.push_back({"v3d_transform_points",Name(uintptr_t(out)),Name(uintptr_t(in)),n,Name(uintptr_t(m)),F(in->x),F(in->y),F(in->z)});
    const auto* pos=reinterpret_cast<const float*>(static_cast<const uint8*>(m)+0x30);
    *out=CVector{in->x+pos[0],in->y+pos[1],in->z+pos[2]}; return out;
}
inline void __cdecl PermanentShadow(uint32 type,void* tex,CVector* pos,float topX,float topY,float rightX,float rightY,uint32 intensity,uint32 r,uint32 g,uint32 b,float drawDist,uint32 time,float up) {
    trace.push_back({"add_permanent_shadow",type&0xFF,Name(uintptr_t(tex)),Name(uintptr_t(pos)),F(pos->x),F(pos->y),F(pos->z),F(topX),F(topY),F(rightX),F(rightY),
                     intensity&0xFFFF,r&0xFF,g&0xFF,b&0xFF,F(drawDist),time,F(up)});
}
inline int32 __fastcall CurrentEventType(void* h,void*) { trace.push_back({"current_event_type",Name(uintptr_t(h))}); return int32(eventTypeRet); }
inline CEntity* __cdecl TestSphere(CVector c,float r,CEntity* ign,uint32 b,uint32 v,uint32 p,uint32 o,uint32 d,uint32 cam) {
    trace.push_back({"test_sphere_against_world",F(c.x),F(c.y),F(c.z),F(r),Name(uintptr_t(ign)),b&0xFF,v&0xFF,p&0xFF,o&0xFF,d&0xFF,cam&0xFF});
    return reinterpret_cast<CEntity*>(sphereRet);
}
inline CEntity* __cdecl TestForClimb(CPed* p,CVector& pos,float& heading,eSurfaceType& surface,uint32 launch) {
    trace.push_back({"test_for_climb",Name(uintptr_t(p)),Name(uintptr_t(&pos)),Name(uintptr_t(&heading)),Name(uintptr_t(&surface)),launch&0xFF});
    if(climbRet){ std::memcpy(&pos,climbOut.data(),12); std::memcpy(&heading,&climbOut[3],4); *reinterpret_cast<uint8*>(&surface)=uint8(climbOut[4]); }
    return reinterpret_cast<CEntity*>(climbRet);
}
inline bool __fastcall SteepSlope(void* s,void*,uint32 id) { trace.push_back({"is_steep_slope",uintptr_t(s)==ADDR_SURFACE_INFOS,id&0xFF}); return steepRet; }
inline bool __cdecl Allowed(CPed* p,CVector dir,float dist) { trace.push_back({"player_allowed_direction",Name(uintptr_t(p)),F(dir.x),F(dir.y),F(dir.z),F(dist)}); return allowedRet; }
inline void* __cdecl GlobalGroup() { trace.push_back({"event_global_group"}); return group.data(); }
inline CEvent* __fastcall GroupAdd(void* g,void*,CEvent* e,bool valid) {
    trace.push_back({"event_group_add",Name(uintptr_t(g)),int(e->GetEventType()),At(e,4),Name(At(e,0x14)),At(e,0x18),At(e,0x1C),At(e,0x20),At(e,0x24),At(e,0x28),valid});
    return nullptr;
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { JUMP, FIST, SITDOWN, STANDUP };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; };
inline const std::array<Cls,4> classes{{
    {"CTaskSimpleJump",0x87054C,0x2C,JUMP}, {"CTaskSimpleShakeFist",0x870A94,0x10,FIST},
    {"CTaskSimpleSitDown",0x86DEF4,0x10,SITDOWN}, {"CTaskSimpleStandUp",0x86DF3C,0x10,STANDUP},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=32;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleJump","CheckIfJumpBlocked",0x67D590,0x87054C,-1,1024},{"CTaskSimpleJump","Clone",0,0x87054C,1,256},{"CTaskSimpleJump","Constructor",0x679AA0,0x87054C,-1,256},{"CTaskSimpleJump","JumpAnimFinishCB",0x67A020,0x87054C,-1,128},{"CTaskSimpleJump","Launch",0x679B80,0x87054C,-1,2048},{"CTaskSimpleJump","MakeAbortable",0,0x87054C,6,256},{"CTaskSimpleJump","ProcessPed",0,0x87054C,7,2048},{"CTaskSimpleJump","StartLaunchAnim",0x67D7A0,0x87054C,-1,1024},
    {"CTaskSimpleShakeFist","Clone",0,0x870A94,1,256},{"CTaskSimpleShakeFist","Constructor",0x690B80,0x870A94,-1,256},{"CTaskSimpleShakeFist","Destructor",0x690BB0,0x870A94,-1,128},{"CTaskSimpleShakeFist","FinishAnimShakeFistCB",0x690C60,0x870A94,-1,128},{"CTaskSimpleShakeFist","GetTaskType",0,0x870A94,4,64},{"CTaskSimpleShakeFist","MakeAbortable",0,0x870A94,6,256},{"CTaskSimpleShakeFist","ProcessPed",0,0x870A94,7,512},{"CTaskSimpleShakeFist","StartAnim",0x692DF0,0x870A94,-1,256},
    {"CTaskSimpleSitDown","Clone",0,0x86DEF4,1,256},{"CTaskSimpleSitDown","Constructor",0x631030,0x86DEF4,-1,256},{"CTaskSimpleSitDown","Destructor",0x631070,0x86DEF4,-1,128},{"CTaskSimpleSitDown","FinishAnimSitDownCB",0x631150,0x86DEF4,-1,128},{"CTaskSimpleSitDown","GetTaskType",0,0x86DEF4,4,64},{"CTaskSimpleSitDown","MakeAbortable",0,0x86DEF4,6,256},{"CTaskSimpleSitDown","ProcessPed",0,0x86DEF4,7,512},{"CTaskSimpleSitDown","StartAnim",0x6379F0,0x86DEF4,-1,256},
    {"CTaskSimpleStandUp","Clone",0,0x86DF3C,1,256},{"CTaskSimpleStandUp","Constructor",0x6312E0,0x86DF3C,-1,256},{"CTaskSimpleStandUp","Destructor",0x631320,0x86DF3C,-1,128},{"CTaskSimpleStandUp","FinishAnimStandUpCB",0x631400,0x86DF3C,-1,128},{"CTaskSimpleStandUp","GetTaskType",0,0x86DF3C,4,64},{"CTaskSimpleStandUp","MakeAbortable",0,0x86DF3C,6,256},{"CTaskSimpleStandUp","ProcessPed",0,0x86DF3C,7,512},{"CTaskSimpleStandUp","StartAnim",0x631410,0x86DF3C,-1,256},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(Ent());
}
// matriz con ejes dados
inline void WriteMatrix(void* dst,float fz,float rz,const CVector& pos) {
    CMatrix m{}; const float c=std::sqrt(1.f-fz*fz), cs=std::cos(rz), sn=std::sin(rz);
    m.GetRight()=CVector{cs,sn,0.f}; m.GetForward()=CVector{-sn*c,cs*c,fz}; m.GetUp()=CVector{sn*fz,-cs*fz,c}; m.GetPosition()=pos;
    std::memcpy(dst,&m,sizeof(CMatrix));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks8b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks8b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks8b");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks8b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // hueco vacio de ms_modelInfoPtrs para el modelo falso (solo ocupado durante cada llamada)
    auto* const modelPtrs=reinterpret_cast<void**>(ADDR_MODEL_PTRS); int32 fakeModel=-1;
    for(int32 m=NUM_MODEL_PTRS-1;m>=0&&fakeModel<0;--m)if(!modelPtrs[m])fakeModel=m;
    if(fakeModel<0){Write({{"event","error"},{"reason","no free model slot"}});return;}
    FillPTraps(std::make_integer_sequence<int,64>{}); pedVtbl[13]=reinterpret_cast<void*>(&TestCollision);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura al final (el hueco del modelo tambien tras cada llamada)
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& currArea=*reinterpret_cast<int32*>(ADDR_CURR_AREA); const int32 savedArea=currArea;
    auto& megaJump=*reinterpret_cast<uint8*>(ADDR_MEGAJUMP); const uint8 savedMega=megaJump;
    auto restoreGlobals=[&]{ timeMs=savedTime; currArea=savedArea; megaJump=savedMega; modelPtrs[fakeModel]=nullptr; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), a3e(0x4D68B0,GetAssoc);
        EntryPatch a6(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a6e(0x4CEBE0,SetFinishCb);
        EntryPatch a7(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetDeleteCallback),SetDeleteCb), a7e(0x4CEBC0,SetDeleteCb);
        EntryPatch s1(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier),Modifier), s1e(0x559AF0,Modifier);
        EntryPatch s2(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),MoveForce), s2e(0x5429F0,MoveForce);
        EntryPatch s3(bit_cast<uintptr_t>(&CLocalisation::Blood),Blood), s3e(0x56D230,Blood);
        EntryPatch w1(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),SkinHier), w1e(0x734A40,SkinHier);
        EntryPatch w2(bit_cast<uintptr_t>(static_cast<RwInt32(*)(RpHAnimHierarchy*,RwInt32)>(&RpHAnimIDGetIndex)),HAnimIndex), w2e(0x7C51A0,HAnimIndex);
        EntryPatch w3(bit_cast<uintptr_t>(static_cast<RwMatrix*(*)(RpHAnimHierarchy*)>(&RpHAnimHierarchyGetMatrixArray)),HAnimMatrices), w3e(0x7C5120,HAnimMatrices);
        EntryPatch w4(bit_cast<uintptr_t>(static_cast<RwV3d*(*)(RwV3d*,const RwV3d*,RwInt32,const RwMatrix*)>(&RwV3dTransformPoints)),TransformPoints), w4e(0x7EDD90,TransformPoints);
        EntryPatch w5(bit_cast<uintptr_t>(&CShadows::AddPermanentShadow),PermanentShadow), w5e(0x706F60,PermanentShadow);
        EntryPatch e1(bit_cast<uintptr_t>(&CEventHandler::GetCurrentEventType),CurrentEventType), e1e(0x4B8CC0,CurrentEventType);
        EntryPatch e2(bit_cast<uintptr_t>(&GetEventGlobalGroup),GlobalGroup), e2e(0x4ABA50,GlobalGroup);
        EntryPatch e3(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),GroupAdd), e3e(0x4AB420,GroupAdd);
        EntryPatch c1(bit_cast<uintptr_t>(static_cast<CEntity*(*)(CVector,float,CEntity*,bool,bool,bool,bool,bool,bool)>(&CWorld::TestSphereAgainstWorld)),TestSphere), c1e(0x569E20,TestSphere);
        EntryPatch c2(bit_cast<uintptr_t>(&CTaskSimpleClimb::TestForClimb),TestForClimb), c2e(0x6803A0,TestForClimb);
        EntryPatch c3(bit_cast<uintptr_t>(&SurfaceInfos_c::IsSteepSlope),SteepSlope), c3e(0x55E770,SteepSlope);
        EntryPatch c4(bit_cast<uintptr_t>(&CGameLogic::IsPlayerAllowedToGoInThisDirection),Allowed), c4e(0x441E10,Allowed);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; const auto& cl=ClassOf(fn.cls); int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x8B8Bu; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    rngState=next();
                    timeMs=next(); currArea=next()%3 ? 0 : int32(1+next()%17); megaJump=uint8(next()%3==0);
                    std::memset(pedA.data(),0,pedA.size()); std::memset(mat.data(),0,mat.size()); std::memset(ent.data(),0,ent.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto& x:intelA)x=uint8(next()); for(auto& x:playerData)x=uint8(next()); for(auto& x:group)x=uint8(next()); for(auto& x:rwHier)x=uint8(next());
                    // entidad fisica (suelo o entidad de escalada)
                    Ent()->SetType(ENTITY_TYPE_OBJECT); At(Ent(),P_PHYS_FLAGS)=next(); Ent()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    for(size_t k=0;k<3;++k)At<float>(Ent(),P_MOVESPEED+4*k)=next()%4 ? uni(-1.f,1.f) : 0.f;
                    // peaton (vtable falsa, matriz propia, clump con tipo 2, inteligencia falsa, modelo falso)
                    CPed* const pd=PedA();
                    pd->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(pd)=pedVtbl.data(); At<void*>(pd,P_CLUMP)=clump.data();
                    At(pd,P_FLAGS1)=next(); At(pd,P_FLAGS2)=next(); At(pd,P_FLAGS3)=next(); At(pd,P_FLAGS4)=next(); At(pd,P_IKFLAGS)=next();
                    At<uint8>(pd,P_FLAGS_ENT)=uint8(next()); At(pd,P_PHYS_FLAGS)=next(); At<int16>(pd,P_MODEL)=int16(fakeModel);
                    { static const uint32 types[]{0,1,0,1,4,6,7,8}; At(pd,P_TYPE)=types[next()%8]; }
                    At<void*>(pd,P_INTEL)=intelA.data(); At<void*>(pd,P_PLAYERDATA)=next()%2 ? static_cast<void*>(playerData.data()) : nullptr;
                    At<void*>(pd,P_LFOOT)=frames.data(); At<void*>(pd,P_RFOOT)=frames.data()+0x18;
                    { const CVector pos{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; pd->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data());
                      WriteMatrix(mat.data(),next()%3 ? 0.f : uni(-0.6f,0.6f),uni(-3.f,3.f),pos); }
                    { const uint32 m=next()%4; for(size_t k=0;k<3;++k)At<float>(pd,P_MOVESPEED+4*k)=m==0 ? 0.f : m==1 ? uni(-0.15f,0.15f) : uni(-1.f,1.f); }
                    At<float>(pd,P_ROT_CUR)=next()%5 ? uni(-6.5f,6.5f) : 0.f; At<float>(pd,P_ROT_AIM)=uni(-6.5f,6.5f);
                    At<void*>(pd,P_STANDING_ON)=next()%3==0 ? static_cast<void*>(Ent()) : nullptr;
                    { const uint32 m=next()%4; for(size_t k=0;k<3;++k)At<float>(pd,P_FIELD578+4*k)=m==0 ? 0.f : uni(-1.f,1.f); }
                    At<uint8>(pd,P_SURFACE)=uint8(next()%180);
                    { const uint32 m=next()%4; At(pd,P_DEATH_TIME)=m==0 ? next()%80 : m==1 ? 40u : m==2 ? 41u : next(); }
                    for(auto& b:frames)b=uint8(next());
                    for(auto& b:rwMats)b=uint8(next()); for(size_t k=0;k<8;++k)for(size_t c=0;c<3;++c)At<float>(rwMats.data(),k*0x40+0x30+c*4)=uni(-30.f,30.f);
                    // modelo falso: CBaseModelInfo(+0x14 colmodel) -> CColModel(+0x2C coldata) -> CCollisionData(+0 numero de esferas, +8 esferas)
                    for(auto& b:modelInfo)b=uint8(next()); for(auto& b:colModel)b=uint8(next()); for(auto& b:colData)b=uint8(next());
                    At<void*>(modelInfo.data(),0x14)=colModel.data(); At<void*>(colModel.data(),0x2C)=colData.data(); At<void*>(colData.data(),8)=spheres.data();
                    { static const uint16 counts[]{3,4,5,8,8,0xFFFE}; At<uint16>(colData.data(),0)=counts[next()%6]; }
                    for(size_t k=0;k<8;++k){ for(size_t c=0;c<3;++c)At<float>(spheres.data(),k*0x14+c*4)=uni(-1.f,1.f); At<float>(spheres.data(),k*0x14+0xC)=0.35f; At(spheres.data(),k*0x14+0x10)=next(); }
                    // asociaciones de animacion (floats controlados, sin NaN de senalizacion) y jerarquia
                    for(auto* a:{&anims[0],&anims[1],&anims[2],&anims[3],&anim2}){ for(auto& b:*a)b=uint8(next());
                        static const float blends[]{0.3f,0.f,1.f,0.29999998f,0.30000004f};
                        At<void*>(a->data(),0x14)=hier.data(); At<float>(a->data(),0x18)=next()%2 ? uni(0.f,1.f) : blends[next()%5];
                        At<float>(a->data(),0x1C)=uni(-4.f,4.f); At<float>(a->data(),0x20)=next()%5 ? uni(0.f,2.f) : 0.f; At<float>(a->data(),0x24)=uni(0.f,2.f); At<float>(a->data(),0x28)=uni(0.f,0.3f); }
                    for(auto& b:hier)b=uint8(next()); At<float>(hier.data(),0x10)=next()%6 ? uni(0.1f,2.f) : 0.f; // 0/0 da NaN y x/0 infinito
                    // dependencias simuladas
                    assocMask=(next()&7)|(uint32(next()%4==0)<<3)|(uint32(next()%4==0)<<4);
                    { static const int32 evs[]{EVENT_STUCK_IN_AIR,EVENT_STUCK_IN_AIR,EVENT_NONE,EVENT_DAMAGE}; eventTypeRet=next()%5 ? evs[next()%4] : int32(next()%100); }
                    bloodRet=next()%2; collisionRet=next()%2; steepRet=next()%3==0; allowedRet=next()%3!=0;
                    modifierRet=next()%3 ? 1.f : uni(0.5f,1.5f);
                    sphereRet=next()%3==0 ? uintptr_t(Ent()) : 0; climbRet=next()%3==0 ? uintptr_t(Ent()) : 0;
                    for(size_t k=0;k<4;++k)climbOut[k]=F(uni(-30.f,30.f)); climbOut[4]=next()%180;
                    const int prio=next()%8==0 ? 3 : int(next()%3);
                    // tarea probada
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    auto flag=[&]{ return uint8(next()%6==0 ? 0x80|next() : next()%2); };
                    switch(cl.kind){
                    case JUMP:
                        for(size_t k=0;k<4;++k)At<float>(o,8+4*k)=uni(-30.f,30.f); At(o,0x18)=next()%180;
                        At<void*>(o,0x1C)=next()%3==0 ? static_cast<void*>(Ent()) : nullptr;
                        for(size_t k=0x20;k<0x26;++k)At<uint8>(o,k)=flag();
                        if(nm=="ProcessPed"&&next()%3)At<uint8>(o,0x20)=1; // la rama principal (salto terminado) es la interesante
                        At<void*>(o,0x28)=next()%3 ? nullptr : static_cast<void*>(anim2.data());
                        break;
                    case FIST: case SITDOWN: case STANDUP:
                        At<uint8>(o,8)=uint8(next()%2); if(cl.kind!=FIST)At<uint8>(o,9)=uint8(next()%2);
                        At<void*>(o,0xC)=next()%3 ? static_cast<void*>(anim2.data()) : nullptr;
                        break;
                    }
                    const uint32 argA=next();
                    const auto occ=Occupancy();
                    modelPtrs[fakeModel]=modelInfo.data();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor",
                               isVoid=nm=="StartAnim"||nm=="Launch"||nm.ends_with("CB");
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        if(cl.kind==FIST)rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                        else rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,bool)>(target)(o,argA%2)); }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,nullptr)!=0;
                    else if(nm=="ProcessPed"||nm=="CheckIfJumpBlocked"||nm=="StartLaunchAnim")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*)>(target)(o,pd)!=0; // el EXE devuelve el byte crudo (p. ej. 0x80); la DLL lo normaliza a bool
                    else if(nm=="StartAnim"||nm=="Launch")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm.ends_with("CB"))reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(anim2.data(),o);
                    setState(false);
                    modelPtrs[fakeModel]=nullptr;
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor&&!isVoid)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),Ent()})refs.push_back(refList(e));
                    json animWords=json::array(); for(auto& a:anims)animWords.push_back(Region(a.data(),a.size()));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"ent",EntHash(ent)},
                            {"intelA",Hash(intelA.data(),intelA.size())},{"playerdata",Hash(playerData.data(),playerData.size())},
                            {"ped_fields",{At(pd,P_FLAGS1),At(pd,P_FLAGS2),At<uint8>(pd,P_FLAGS_ENT),At(pd,P_IKFLAGS),At(pd,P_ROT_CUR),At(pd,P_ROT_AIM),At(pd,P_DEATH_TIME),
                                           At(pd,P_MOVESPEED),At(pd,P_MOVESPEED+4),At(pd,P_MOVESPEED+8)}},
                            {"mat",Region(mat.data(),sizeof(CMatrix))},{"anims",animWords},{"anim2",Region(anim2.data(),anim2.size())},{"hier",Hash(hier.data(),hier.size())},
                            {"model",{Hash(modelInfo.data(),modelInfo.size()),Hash(colModel.data(),colModel.size()),Hash(colData.data(),colData.size()),Region(spheres.data(),spheres.size())}},
                            {"rw",{Hash(rwHier.data(),rwHier.size()),Hash(rwMats.data(),rwMats.size()),Hash(frames.data(),frames.size()),Hash(group.data(),group.size())}},
                            {"time",timeMs},{"area",currArea},{"megajump",megaJump}};
                    restoreGlobals(); // reloj, area, megasalto y hueco del modelo vuelven a su valor tras cada ejecucion
                    // borrado de las tareas creadas (su traza va aparte)
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["clean"]=clean; r["switches"]=switches;
                    freshIds.clear();
                    CleanRefs();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
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
    const bool globalsTouched=timeMs!=savedTime||currArea!=savedArea||megaJump!=savedMega||modelPtrs[fakeModel]!=nullptr;
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&currArea==savedArea&&megaJump==savedMega&&modelPtrs[fakeModel]==nullptr}});
}
}
