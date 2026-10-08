#pragma once
// Banco dentro del motor: limpieza y proceso del mundo (CWorld, grupo D). CallOffChaseForArea y sus dos listas de sector,
// ClearCarsFromArea, ClearPedsFromArea, ClearExcitingStuffFromArea, ExtinguishAllCarFiresInArea, PrintCarChanges, Process,
// ProcessAttachedEntities, ProcessForAnimViewer, ProcessPedsAfterPreRender, RemoveEntityInsteadOfProcessingIt,
// RemoveFallenCars, RemoveFallenPeds, RemoveReferencesToDeletedObject y RemoveStaticObjects.
// Original (hooks del grupo desactivados) frente a candidato con fixtures deterministas. NUNCA se toca ningun pool, lista ni
// entidad real: los pools de vehiculos, peatones, objetos, edificios, dummies y nodos (simples y dobles) se sustituyen por
// pools falsos que viven en un arena propio (cada ejecucion lo reconstruye con la misma semilla y las mismas direcciones),
// y las listas de sectores, de sectores repetidos, la de entidades en movimiento y la de objetos con codigo de control se
// vacian y se rellenan con nodos del pool falso. Las entidades son memoria cruda con una vtable falsa trazada (destructor,
// Add, Remove, ProcessControl, ProcessCollision, ProcessShift, Teleport y RemoveRefsToVehicle), asi que nunca se destruye,
// se mueve ni se borra nada del juego. Se simulan y trazan (en el EXE y en la DLL): FindPlayerPed/Vehicle/Wanted,
// CPhysical::Add/RemoveFromMovingList, UpdateAnim, UpdateRwFrame y las dos UpdateRwMatrix, CPopulation::RemovePed,
// RemoveRefsToEntity, IsPlayer/CanBeDeleted/IsAlive, GetColModel, ExtinguishCarFire, RemovePassenger, CleanUpOldReference,
// el garaje, CPedGroups::GetPedsGroup, CTaskManager::FindActiveTaskByType, CTask::operator new/delete, CEventGroup::Add,
// los colas y grupos globales de eventos, el monitor de carga, IKChainManager, CMessages, CPlayerInfo::Process, los
// efectos de ClearExcitingStuffFromArea, FindNodeClosestToCoors y el printf del CRT. Todo el estado global tocado se guarda
// y se restaura en cada ejecucion y al final.
#include <fstream>
#include <set>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "World.h"
#include "Garages.h"
#include "SimpleTransform.h"
#include "Tasks/Task.h"
#include "Events/EventGroup.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Object/CutsceneObject.h"
#include "Entity/Building.h"
#include "Entity/Dummy/Dummy.h"
#include "../ProbeRuntime.hpp"

namespace WldDLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline uint32 Mix(uint32 a,uint32 b){ uint32 h=a*0x9E3779B1u^b*0x85EBCA77u; h^=h>>15; h*=0x2C1B3C6Du; h^=h>>12; h*=0x297A2D39u; h^=h>>15; return h; }
struct Rng {
    uint32 s{1};
    uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool chance(uint32 pct){ return next()%100<pct; }
    template<class T> T* pick(const std::vector<T*>& v){ return v.empty() ? nullptr : v[next()%v.size()]; }
};

// --- direcciones del EXE ---
constexpr uintptr_t POOL_NODE_S=0xB74484, POOL_NODE_D=0xB74488, POOL_PED=0xB74490, POOL_VEH=0xB74494, POOL_BLD=0xB74498, POOL_OBJ=0xB7449C, POOL_DUM=0xB744A0;
constexpr uintptr_t SECTORS=0xB7D0B8, REPEAT=0xB992B8, MOVING=0xB9ACC8, CTRLLIST=0xB9ACCC, WORLD_END=0xB9ACD0;
constexpr uintptr_t SCAN=0xB7CD78, FRAME=0xB7CB4C, STEP=0xB7CB5C, SKIP=0xB7CB89, TIMEMS=0xB7CB84, WFLAGS=0xB7CD6C, REPLAY=0xA43088;
constexpr uintptr_t CUT_RUN=0xB5F851, CUT_OBJS=0xBC3F18, PLAYERS=0xB7CD98, GROUP_ACTIVE=0xC098E0, GROUPS=0xC09920, MODELS=0xB7CBA8, PATH_NODES=0x96F854;
constexpr int PATH_AREA=7;

// --- memoria falsa: todo en un arena (mismas direcciones en las dos ejecuciones) ---
alignas(16) inline std::array<uint8,1<<18> arena{};
inline size_t bump{};
inline uint8* Alloc(size_t n,size_t align=16){ bump=(bump+align-1)&~(align-1); auto* p=arena.data()+bump; bump+=n; return p; }
inline std::vector<uint8> pathNodes(size_t(65536)*0x1C);
inline std::array<void*,64> vt{};
constexpr int NV=8, NP=8, NO=8, NB=4, ND=4, NC=4, NS=384, NDL=640;
constexpr size_t VEH_STRIDE=0xA18, PED_STRIDE=0x7C4, OBJ_STRIDE=0x19C, BLD_STRIDE=0x38;
inline std::vector<uint8*> V,P,O,B,D,C;
inline uint8 *sBase{}, *dBase{}; inline int nextS{}, nextD{};
inline void *playerPed{}, *playerVeh{}, *wantedBuf{}, *globalGroup{}, *taskBase{};
inline std::array<uint8*,3> colModels{};
inline std::array<uint8*,3> groupBufs{};
inline int taskNext{};
inline uint32 seedG{}, coinCounter{}, nodeRet{};
inline uint32 trapped{};

inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p); const auto a=uintptr_t(arena.data());
    if(u>=a&&u<a+arena.size()) return json::array({"a",u-a});
    if(u>=SECTORS&&u<WORLD_END) return json::array({"w",u-SECTORS});
    if(u>=GROUPS&&u<GROUPS+8*0x2D4) return json::array({"g",(u-GROUPS)/0x2D4,(u-GROUPS)%0x2D4});
    if(u>=PLAYERS&&u<PLAYERS+0x320) return json::array({"pl",(u-PLAYERS)/0x190,(u-PLAYERS)%0x190});
    if(u>=CUT_OBJS&&u<CUT_OBJS+200) return json::array({"cut",(u-CUT_OBJS)/4});
    if(u>=0x400000&&u<0x1000000) return json::array({"exe",u});
    return "ext"; }
inline bool Coin(const void* p,uint32 salt,uint32 permille=500){ return (Mix(seedG^uint32(uintptr_t(p)-uintptr_t(arena.data())),salt)%1000)<permille; }

// --- stubs: entidades y listas ---
inline void __fastcall S_RemoveFromMoving(void* self,void*){ trace.push_back({"remove_from_moving",Id(self)}); }
inline void __fastcall S_AddToMoving(void* self,void*){ trace.push_back({"add_to_moving",Id(self)}); }
inline void __fastcall S_UpdateAnim(void* self,void*){ trace.push_back({"update_anim",Id(self)}); }
inline void __fastcall S_UpdateRwFrame(void* self,void*){ trace.push_back({"update_rw_frame",Id(self)}); }
inline void __fastcall S_MatUpdateRw(void* self,void*,void* rw){ trace.push_back({"matrix_update_rw",Id(self),Id(rw)}); }
inline void __fastcall S_STUpdateRw(void* self,void*,void* rw){ trace.push_back({"transform_update_rw",Id(self),Id(rw)}); }
inline void __cdecl S_RemovePed(void* ped){ trace.push_back({"remove_ped",Id(ped)}); }
inline void __fastcall S_RemoveRefs(void* self,void*,void* ent){ trace.push_back({"remove_refs_to_entity",Id(self),Id(ent)}); }
inline bool __fastcall S_IsPlayer(void* self,void*){ const bool r=self==playerPed||Coin(self,1,150); trace.push_back({"ped_is_player",Id(self),r}); return r; }
inline bool __fastcall S_PedCanDelete(void* self,void*){ const bool r=Coin(self,2,700); trace.push_back({"ped_can_be_deleted",Id(self),r}); return r; }
inline bool __fastcall S_VehCanDelete(void* self,void*){ const bool r=Coin(self,3,700); trace.push_back({"veh_can_be_deleted",Id(self),r}); return r; }
inline bool __fastcall S_IsAlive(void* self,void*){ const bool r=Coin(self,4,750); trace.push_back({"ped_is_alive",Id(self),r}); return r; }
inline void* __fastcall S_GetColModel(void* self,void*){ void* r=colModels[Mix(uint32(uintptr_t(self)-uintptr_t(arena.data())),7)%colModels.size()]; trace.push_back({"get_col_model",Id(self),Id(r)}); return r; }
inline void __fastcall S_ExtinguishCarFire(void* self,void*){ trace.push_back({"extinguish_car_fire",Id(self)}); }
inline void __fastcall S_RemovePassenger(void* self,void*,void* ped){ trace.push_back({"remove_passenger",Id(self),Id(ped)}); }
inline void __fastcall S_CleanUpRef(void* self,void*,void* ref){ trace.push_back({"clean_up_old_reference",Id(self),Id(ref)}); }
inline bool __cdecl S_Interesting(void* veh){ const bool r=Coin(veh,5,400); trace.push_back({"is_interesting",Id(veh),r}); return r; }
inline void __cdecl S_StoreCar(void* veh){ trace.push_back({"store_car_in_garage",Id(veh)}); }
inline void __cdecl S_RemoveInteresting(void* veh){ trace.push_back({"remove_from_interesting",Id(veh)}); }
inline bool __cdecl S_Hideout(const float* p){ const bool r=Mix(seedG,F(p[0])^F(p[1])*3u)%100<25; trace.push_back({"hideout_garage",F(p[0]),F(p[1]),F(p[2]),r}); return r; }
inline void* __cdecl S_FindPlayerPed(int32 id){ trace.push_back({"find_player_ped",id}); return playerPed; }
inline void* __cdecl S_FindPlayerVehicle(int32 id,bool remote){ trace.push_back({"find_player_vehicle",id,remote}); return playerVeh; }
inline void* __cdecl S_FindPlayerWanted(int32 id){ trace.push_back({"find_player_wanted",id}); return wantedBuf; }
inline void* __cdecl S_GetPedsGroup(void* ped){ void* intel=At<void*>(ped,0x47C); void* g=intel ? At<void*>(intel,0x88) : nullptr; trace.push_back({"get_peds_group",Id(ped),Id(g)}); return g; }
inline void __fastcall S_RemovePursuitCop(void* self,void*,void* cop){ trace.push_back({"remove_pursuit_cop",Id(self),Id(cop)}); }
inline void* __fastcall S_FindActiveTask(void* self,void*,int32 type){ auto* intel=static_cast<uint8*>(self)-4; void* r=type==1000 ? At<void*>(intel,0x80) : (type==1003 ? At<void*>(intel,0x84) : nullptr);
    trace.push_back({"find_active_task",Id(intel),type,Id(r)}); return r; }
inline void* __cdecl S_TaskNew(size_t size){ void* r=static_cast<uint8*>(taskBase)+size_t(taskNext++%8)*0x40; trace.push_back({"task_new",size,Id(r)}); return r; }
inline void __cdecl S_TaskDelete(void* p){ trace.push_back({"task_delete",Id(p)}); }
inline void* __fastcall S_EventAdd(void* self,void*,void* ev,bool valid){ auto* t=At<uint8*>(ev,0x10);
    trace.push_back({"event_group_add",Id(self),At<int32>(ev,0xC),Id(t),At<uint8>(ev,0x14),valid,t ? Fnv(t+4,0x1C) : 0ull}); return nullptr; }
// --- stubs: Process ---
inline void* __cdecl S_GlobalGroup(){ trace.push_back({"get_event_global_group"}); return globalGroup; }
inline void __fastcall S_AddEventsToPed(void* self,void*,void* ped){ trace.push_back({"add_events_to_ped",Id(self),Id(ped)}); }
inline void __fastcall S_AddEventsToGroup(void* self,void*,void* g){ trace.push_back({"add_events_to_group",Id(self),Id(g)}); }
inline void __fastcall S_EventFlush(void* self,void*,bool b){ trace.push_back({"event_group_flush",Id(self),b}); }
inline void __cdecl S_InformFriends(){ trace.push_back({"inform_friends_process"}); }
inline void __cdecl S_InformGroup(){ trace.push_back({"inform_group_process"}); }
inline void __cdecl S_PruneRefs(){ trace.push_back({"prune_all_references"}); }
inline void __fastcall S_ControlLogic(void* self,void*){ trace.push_back({"process_control_logic",Id(self)}); }
inline void __fastcall S_LoadStart(void* self,void*,uint32 t){ trace.push_back({"load_monitor_start",Id(self),t}); }
inline void __fastcall S_LoadEnd(void* self,void*,uint32 t){ trace.push_back({"load_monitor_end",Id(self),t}); }
inline void __cdecl S_VehRec(){ trace.push_back({"vehicle_recording"}); }
inline void __fastcall S_IkUpdate(void* self,void*,float dt){ trace.push_back({"ik_chain_update",Id(self),F(dt)}); }
inline void __fastcall S_AfterProcCol(void* self,void*){ trace.push_back({"after_proc_col",Id(self)}); }
inline void __fastcall S_AfterPreRender(void* self,void*){ trace.push_back({"after_pre_render",Id(self)}); }
inline void __fastcall S_PosAttPed(void* self,void*){ trace.push_back({"position_attached_ped",Id(self)}); }
inline void __fastcall S_PosAttEnt(void* self,void*){ trace.push_back({"position_attached_entity",Id(self)}); }
inline void __cdecl S_Messages(){ trace.push_back({"messages_process"}); }
inline void __fastcall S_PlayerProcess(void* self,void*,uint32 n){ trace.push_back({"player_info_process",Id(self),n}); }
inline void __cdecl S_ScriptedTask(){ trace.push_back({"scripted_task_record_process"}); }
inline void __cdecl S_GroupsProcess(){ trace.push_back({"ped_groups_process"}); }
inline void __fastcall S_ApplyMove(void* self,void*){ trace.push_back({"apply_move_speed",Id(self),F(At<float>(self,0x44)),F(At<float>(self,0x48)),F(At<float>(self,0x4C))}); }
inline void __fastcall S_ApplyTurn(void* self,void*){ trace.push_back({"apply_turn_speed",Id(self)}); }
// --- stubs: ClearExcitingStuffFromArea ---
inline void __cdecl S_TempObjects(float x,float y,float z,float r){ trace.push_back({"delete_temp_objects",F(x),F(y),F(z),F(r)}); }
inline void __fastcall S_FireExtinguishPoint(void* self,void*,float x,float y,float z,float r){ trace.push_back({"fire_extinguish_point",Id(self),F(x),F(y),F(z),F(r)}); }
inline void __cdecl S_Explosions(float x,float y,float z,float r){ trace.push_back({"remove_explosions",F(x),F(y),F(z),F(r)}); }
inline void __cdecl S_Projectiles(){ trace.push_back({"remove_projectiles"}); }
inline void __cdecl S_Shadows(){ trace.push_back({"tidy_up_shadows"}); }
inline void __cdecl S_Pickups(const float* p,float r){ trace.push_back({"remove_unnecessary_pickups",F(p[0]),F(p[1]),F(p[2]),F(r)}); }
// --- stubs: nodos de ruta y printf ---
inline uint32* __fastcall S_Closest(void* pf,void*,uint32* ret,float x,float y,float z,int32 type,float maxd,uint32 a,int32 b,uint32 c,uint32 d,int32 e){
    trace.push_back({"find_node_closest_to_coors",uintptr_t(pf)==0x96F050,F(x),F(y),F(z),type,F(maxd),a&0xFFFF,b,c&0xFFFF,d&0xFFFF,e}); *ret=nodeRet; return ret; }
inline int __cdecl S_Printf(const char* fmt,int a,int b,int c){ trace.push_back({"printf",std::string(fmt),a,b,c}); return 0; }
// --- vtable falsa de las entidades ---
template<int N> inline void __fastcall VS(void* self,void*){ trace.push_back({"vt",N,Id(self)}); }
inline void* __fastcall VDtor(void* self,void*,uint8 flags){ trace.push_back({"vt_dtor",Id(self),flags}); return self; }
inline void __fastcall VTeleport(void* self,void*,float x,float y,float z,uint32 b){ trace.push_back({"vt_teleport",Id(self),F(x),F(y),F(z),b&0xFF}); }
inline void __fastcall VRemoveRefsVeh(void* self,void*,void* ent){ trace.push_back({"vt_remove_refs_to_vehicle",Id(self),Id(ent)}); }
inline void __fastcall VProcessControl(void* self,void*){ trace.push_back({"vt",10,Id(self)}); auto& fl=At<uint32>(self,0x1C); const uint32 r=Mix(seedG^0xA1u,coinCounter++);
    if(r%7==0)fl^=0x4; if(r%11==0)fl|=0x800; if(r%13==0)fl|=0x40; }
inline void __fastcall VProcessCollision(void* self,void*){ trace.push_back({"vt",11,Id(self)}); auto& fl=At<uint32>(self,0x1C); const uint32 r=Mix(seedG^0xB2u,coinCounter++);
    if(r%3==0)fl|=0x20; else if(r%3==1)fl&=~0x20u; }
inline void __fastcall VProcessShift(void* self,void*){ trace.push_back({"vt",12,Id(self)}); auto& fl=At<uint32>(self,0x1C); const uint32 r=Mix(seedG^0xC3u,coinCounter++);
    if(r%3==0)fl|=0x20; else if(r%3==1)fl&=~0x20u; }
inline void InitVt(){ [&]<size_t... I>(std::index_sequence<I...>){ ((vt[I]=reinterpret_cast<void*>(&VS<int(I)>)),...); }(std::make_index_sequence<64>{});
    vt[0]=reinterpret_cast<void*>(&VDtor); vt[10]=reinterpret_cast<void*>(&VProcessControl); vt[11]=reinterpret_cast<void*>(&VProcessCollision);
    vt[12]=reinterpret_cast<void*>(&VProcessShift); vt[14]=reinterpret_cast<void*>(&VTeleport); vt[40]=reinterpret_cast<void*>(&VRemoveRefsVeh); }

// --- listas con nodos del pool falso ---
inline void MakeHdr(uint8* h,void* store,uint8* st,int32 n){ At<void*>(h,0)=store; At<void*>(h,4)=st; At<int32>(h,8)=n; At<int32>(h,0xC)=n-1; At<uint8>(h,0x10)=0; }
inline void AddSingle(uint32* head,void* item){ if(nextS>=NS)return; auto* n=reinterpret_cast<uint32*>(sBase+size_t(nextS++)*8); n[0]=uint32(uintptr_t(item)); n[1]=*head; *head=uint32(uintptr_t(n)); }
inline void AddDouble(uint32* head,void* item){ if(nextD>=NDL)return; auto* n=reinterpret_cast<uint32*>(dBase+size_t(nextD++)*12); n[0]=uint32(uintptr_t(item)); n[1]=*head; n[2]=0;
    if(*head)reinterpret_cast<uint32*>(*head)[2]=uint32(uintptr_t(n)); *head=uint32(uintptr_t(n)); }
inline uint32* SectorB(int x,int y){ return reinterpret_cast<uint32*>(SECTORS+(size_t(y)*120+size_t(x))*8); }
inline uint32* SectorD(int x,int y){ return reinterpret_cast<uint32*>(SECTORS+(size_t(y)*120+size_t(x))*8+4); }
inline uint32* RepeatL(int x,int y,int k){ return reinterpret_cast<uint32*>(REPEAT+(size_t(y&15)*16+size_t(x&15))*12+size_t(k)*4); }
inline std::vector<uint8*> AllEnts(){ std::vector<uint8*> a; for(auto* v:{&V,&P,&O,&B,&D})a.insert(a.end(),v->begin(),v->end()); return a; }
inline std::vector<uint8*> Phys(){ std::vector<uint8*> a; for(auto* v:{&V,&P,&O})a.insert(a.end(),v->begin(),v->end()); return a; }
inline std::vector<uint8*> PhysAndCut(){ auto a=Phys(); a.insert(a.end(),C.begin(),C.end()); return a; }

// Construye el mundo falso completo: pools, entidades, colmodels, grupos y tareas. Las listas se rellenan despues.
inline void BuildEnv(Rng& r,uint16 scanNow) {
    std::memset(arena.data(),0,arena.size()); bump=0; nextS=nextD=0; taskNext=0; coinCounter=0;
    V.clear(); P.clear(); O.clear(); B.clear(); D.clear(); C.clear();
    std::memset(reinterpret_cast<void*>(SECTORS),0,WORLD_END-SECTORS);
    // pools de nodos
    { uint8* h=Alloc(0x14); uint8* st=Alloc(NS+1); sBase=Alloc(size_t(NS)*8); MakeHdr(h,sBase,st,NS); *reinterpret_cast<void**>(POOL_NODE_S)=h; }
    { uint8* h=Alloc(0x14); uint8* st=Alloc(NDL+1); dBase=Alloc(size_t(NDL)*12); MakeHdr(h,dBase,st,NDL); *reinterpret_cast<void**>(POOL_NODE_D)=h; }
    // pools de entidades (todas las ranuras ocupadas salvo algunas libres, nunca las dos primeras)
    auto Pool=[&](uintptr_t global,size_t stride,int n,std::vector<uint8*>& used)->uint8*{
        uint8* h=Alloc(0x14); uint8* store=Alloc(stride*size_t(n)); uint8* st=Alloc(size_t(n)+1); MakeHdr(h,store,st,n);
        for(int i=0;i<n;++i){ if(i>=2&&r.chance(15)){ st[i]=0x80; } else { st[i]=uint8(r.next()&0x7F); used.push_back(store+size_t(i)*stride); } }
        *reinterpret_cast<void**>(global)=h; return store; };
    Pool(POOL_VEH,VEH_STRIDE,NV,V); Pool(POOL_PED,PED_STRIDE,NP,P); Pool(POOL_OBJ,OBJ_STRIDE,NO,O); Pool(POOL_BLD,BLD_STRIDE,NB,B); Pool(POOL_DUM,BLD_STRIDE,ND,D);
    uint8* cutStore=Alloc(OBJ_STRIDE*NC); for(int i=0;i<NC;++i)C.push_back(cutStore+size_t(i)*OBJ_STRIDE);
    InitVt();
    // campos comunes
    auto Common=[&](uint8* e,int type,int matrixMode){
        At<void*>(e,0)=vt.data();
        const bool hasMat=matrixMode==1||(matrixMode==2&&r.chance(60));
        const CVector pos{r.uni(-300.f,300.f),r.uni(-300.f,300.f),r.chance(35) ? r.uni(-130.f,-90.f) : r.uni(-20.f,60.f)};
        if(hasMat){ uint8* m=Alloc(0x50); for(int k=0;k<12;++k)if(k%4!=3)At<float>(m,size_t(k)*4)=r.uni(-1.f,1.f); At<CVector>(m,0x30)=pos; At<void*>(e,0x14)=m; }
        else { At<CVector>(e,4)=pos; At<float>(e,0x10)=r.uni(-3.2f,3.2f); }
        if(r.chance(50)){ uint8* ro=Alloc(0x10); uint8* fr=Alloc(0x120); ro[0]=1; At<void*>(ro,4)=fr; At<void*>(e,0x18)=ro; }
        uint32 fl=0; if(r.chance(10))fl|=0x4; if(r.chance(5))fl|=0x40000; if(r.chance(60))fl|=0x20; if(r.chance(15))fl|=0x10; if(r.chance(15))fl|=0x40; if(r.chance(12))fl|=0x800;
        At<uint32>(e,0x1C)=fl;
        At<uint16>(e,0x22)=uint16(r.chance(12) ? 0x8000+r.next()%0x7FFF : r.next()%700);
        At<uint16>(e,0x2C)=r.chance(30) ? scanNow : uint16(r.next());
        At<uint8>(e,0x36)=uint8(type|((r.next()%5)<<3));
        if(type>=2&&type<=4){
            At<uint32>(e,0x40)=r.chance(30) ? 0x20000000u : 0u;
            for(int k=0;k<3;++k)At<float>(e,0x44+size_t(k)*4)=r.chance(15) ? 0.f : r.uni(-2.f,2.f); } };
    for(auto* e:V){ Common(e,2,1);
        { static constexpr int8 missions[]{0,1,2,3,4,5,60,61,62,7,8}; At<int8>(e,0x3BA)=missions[r.next()%11]; }
        At<uint8>(e,0x3BB)=uint8(r.next()%6); At<uint32>(e,0x3BC)=r.next();
        At<uint32>(e,0x428)=(r.chance(60)?1u:0u)|(r.chance(25)?8u:0u)|(r.chance(30)?0x100u:0u);
        At<uint8>(e,0x488)=uint8(r.next()%9); At<uint8>(e,0x4A4)=uint8(1+r.next()%4);
        { static constexpr uint32 vtypes[]{0,0,0,5,6,8,9}; At<uint32>(e,0x590)=vtypes[r.next()%7]; } }
    for(auto* e:P){ Common(e,3,1); /* con matriz siempre: RemoveFallenPeds escribe la z de la matriz sin comprobarla (0x565D86) */
        uint8* intel=Alloc(0x120); At<void*>(e,0x47C)=intel; At<uint8>(e,0x484)=uint8(1+r.next()%3);
        At<int32>(e,0x598)=r.chance(30) ? 6 : int32(r.next()%25); }
    for(auto* e:O)Common(e,4,2);
    for(auto* e:B)Common(e,1,2);
    for(auto* e:D)Common(e,5,2);
    for(auto* e:C){ Common(e,4,1); At<void*>(e,0xB0)=r.chance(60) ? static_cast<void*>(Alloc(16)) : nullptr; }
    // enlaces entre entidades
    auto AllMisc=AllEnts();
    for(auto* e:V){ At<void*>(e,0x460)=r.chance(60) ? static_cast<void*>(r.pick(P)) : nullptr; for(int k=0;k<8;++k)At<void*>(e,0x464+size_t(k)*4)=r.chance(35) ? static_cast<void*>(r.pick(P)) : nullptr;
        At<uint8>(e,0x484)=uint8(r.next()%9); if(r.chance(25))At<void*>(e,0xFC)=r.pick(AllMisc); if(r.chance(20))At<void*>(e,0x128)=r.pick(AllMisc); }
    for(auto* e:P){ if(r.chance(20))At<void*>(e,0x568)=r.pick(AllMisc); if(r.chance(25))At<void*>(e,0x584)=r.pick(V); if(r.chance(20))At<void*>(e,0xFC)=r.pick(AllMisc); if(r.chance(20))At<void*>(e,0x128)=r.pick(AllMisc); }
    for(auto* e:O)if(r.chance(25))At<void*>(e,0xFC)=r.pick(AllMisc);
    // jugador, grupos, tareas, colmodels
    playerPed=P.front(); playerVeh=r.chance(60) ? static_cast<void*>(r.pick(V)) : nullptr; wantedBuf=Alloc(0x40); globalGroup=Alloc(0x40);
    if(playerVeh&&r.chance(50))At<void*>(playerPed,0x584)=r.chance(60) ? playerVeh : static_cast<void*>(r.pick(V));
    for(auto& g:groupBufs){ g=Alloc(0x40); for(int k=0;k<7;++k)At<void*>(g,8+size_t(k)*4)=r.chance(35) ? static_cast<void*>(r.pick(P)) : nullptr; At<void*>(g,8+7*4)=r.chance(50) ? playerPed : static_cast<void*>(r.pick(P)); }
    for(auto* e:P){ uint8* intel=At<uint8*>(e,0x47C); if(r.chance(55))At<void*>(intel,0x88)=groupBufs[r.next()%groupBufs.size()]; }
    taskBase=Alloc(8*0x40); std::memset(taskBase,0xCD,8*0x40);
    for(auto* e:P){ uint8* intel=At<uint8*>(e,0x47C);
        if(r.chance(40)){ uint8* t=Alloc(0x30); At<void*>(t,0x10)=r.chance(55) ? playerPed : static_cast<void*>(r.pick(P)); At<void*>(intel,0x80)=t; }
        if(r.chance(35)){ uint8* t=Alloc(0x30); At<void*>(t,0x10)=r.chance(55) ? playerVeh : static_cast<void*>(r.pick(V)); At<void*>(intel,0x84)=t; } }
    for(auto& cm:colModels){ cm=Alloc(0x30); uint8* cd=Alloc(0x20); uint8* sp=Alloc(0x14*5); const uint32 k=r.next()%7;
        At<int16>(cd,0)=int16(k==6 ? int16(0x8001) : int16(k%5)); At<void*>(cd,8)=sp; At<void*>(cm,0x2C)=cd;
        for(int s=0;s<5;++s){ for(int c=0;c<3;++c)At<float>(sp,size_t(s)*0x14+size_t(c)*4)=r.uni(-4.f,4.f); At<float>(sp,size_t(s)*0x14+0xC)=r.uni(0.3f,3.f); } }
    // globales sueltos
    *reinterpret_cast<uint16*>(SCAN)=scanNow; *reinterpret_cast<uint32*>(TIMEMS)=r.next()%1000000;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,17> fns{{
    {"CWorld","CallOffChaseForArea",0x566A60,0,-1,512},{"CWorld","CallOffChaseForAreaSectorListPeds",0x563D00,0,-1,1024},
    {"CWorld","CallOffChaseForAreaSectorListVehicles",0x563A80,0,-1,1024},{"CWorld","ClearCarsFromArea",0x566610,0,-1,512},
    {"CWorld","ClearExcitingStuffFromArea",0x56A0D0,0,-1,512},{"CWorld","ClearPedsFromArea",0x5667F0,0,-1,256},
    {"CWorld","ExtinguishAllCarFiresInArea",0x566950,0,-1,256},{"CWorld","PrintCarChanges",0x566420,0,-1,256},
    {"CWorld","Process",0x5684A0,0,-1,256},{"CWorld","ProcessAttachedEntities",0x5647F0,0,-1,256},
    {"CWorld","ProcessForAnimViewer",0x5633D0,0,-1,128},{"CWorld","ProcessPedsAfterPreRender",0x563430,0,-1,128},
    {"CWorld","RemoveEntityInsteadOfProcessingIt",0x563A10,0,-1,256},{"CWorld","RemoveFallenCars",0x565E80,0,-1,256},
    {"CWorld","RemoveFallenPeds",0x565CB0,0,-1,256},{"CWorld","RemoveReferencesToDeletedObject",0x565510,0,-1,256},
    {"CWorld","RemoveStaticObjects",0x563840,0,-1,128},
}};

// posicion de una entidad: matriz (+0x14 -> +0x30) o transformacion simple (+4)
inline CVector* EntPos(uint8* e){ auto* m=At<uint8*>(e,0x14); return reinterpret_cast<CVector*>(m ? m+0x30 : e+4); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"wldd-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"wldd-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("wldd");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"wldd-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,17> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    using std::bit_cast;
    // los strides de los pools de la DLL tienen que ser los del EXE, o los pools falsos no valdrian
    if(sizeof(CCopPed)!=PED_STRIDE||sizeof(CHeli)!=VEH_STRIDE||sizeof(CCutsceneObject)!=OBJ_STRIDE||sizeof(CBuilding)!=BLD_STRIDE||sizeof(CDummy)!=BLD_STRIDE){
        Write({{"event","error"},{"reason","pool stride mismatch"},{"ped",sizeof(CCopPed)},{"veh",sizeof(CHeli)},{"obj",sizeof(CCutsceneObject)},{"bld",sizeof(CBuilding)},{"dum",sizeof(CDummy)}});return; }
    std::array<bool,17> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {0xB74484,0x20},{SECTORS,WORLD_END-SECTORS},{WFLAGS,8},{SCAN,2},{FRAME,4},{STEP,4},{SKIP,1},{TIMEMS,4},{REPLAY,1},{CUT_RUN,1},
            {CUT_OBJS,200},{PLAYERS,0x320},{GROUP_ACTIVE,8},{MODELS,440},{PATH_NODES+size_t(PATH_AREA)*4,4}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches; std::set<uintptr_t> patched;
        // parchea la direccion del EXE y, si es un salto a la DLL (hook activo), tambien su destino; "dll" fuerza el simbolo de la DLL
        auto Stub=[&](uintptr_t exeAddr,auto fn,uintptr_t dll=0){
            if(!dll&&ReversingProbe::JumpsToDll(exeAddr))dll=exeAddr+5+*reinterpret_cast<const int32*>(exeAddr+1);
            if(patched.insert(exeAddr).second)patches.push_back(std::make_unique<EntryPatch>(exeAddr,fn));
            if(dll&&patched.insert(dll).second)patches.push_back(std::make_unique<EntryPatch>(dll,fn));
        };
        Stub(0x56E210,S_FindPlayerPed); Stub(0x56E0D0,S_FindPlayerVehicle); Stub(0x56E230,S_FindPlayerWanted);
        Stub(0x542860,S_RemoveFromMoving); Stub(0x542800,S_AddToMoving); Stub(0x535F00,S_UpdateAnim); Stub(0x532B00,S_UpdateRwFrame);
        Stub(0x59AD70,S_MatUpdateRw,bit_cast<uintptr_t>(&CMatrix::UpdateRwMatrix)); Stub(0x54EF40,S_STUpdateRw,bit_cast<uintptr_t>(&CSimpleTransform::UpdateRwMatrix));
        Stub(0x610F20,S_RemovePed); Stub(0x544280,S_RemoveRefs); Stub(0x5DF8F0,S_IsPlayer); Stub(0x5DF100,S_PedCanDelete); Stub(0x6D1180,S_VehCanDelete); Stub(0x5E0170,S_IsAlive);
        Stub(0x535300,S_GetColModel); Stub(0x6D2460,S_ExtinguishCarFire); Stub(0x6D1610,S_RemovePassenger); Stub(0x571A00,S_CleanUpRef);
        Stub(0x423EA0,S_Interesting); Stub(0x44A3C0,S_StoreCar,bit_cast<uintptr_t>(&CGarages::StoreCarInNearestImpoundingGarage)); Stub(0x423ED0,S_RemoveInteresting); Stub(0x448900,S_Hideout);
        Stub(0x5F7E80,S_GetPedsGroup); Stub(0x562C10,S_RemovePursuitCop); Stub(0x681740,S_FindActiveTask); Stub(0x61A5A0,S_TaskNew); Stub(0x61A5B0,S_TaskDelete); Stub(0x4AB420,S_EventAdd);
        Stub(0x4ABA50,S_GlobalGroup); Stub(0x4AB9C0,S_AddEventsToPed); Stub(0x4AB8A0,S_AddEventsToGroup); Stub(0x4AB370,S_EventFlush);
        Stub(0x4AC2E0,S_InformFriends); Stub(0x4B2AE0,S_InformGroup); Stub(0x571CC0,S_PruneRefs);
        Stub(0x5A29A0,S_ControlLogic); Stub(0x53D050,S_LoadStart); Stub(0x53D070,S_LoadEnd); Stub(0x45A610,S_VehRec); Stub(0x6186D0,S_IkUpdate);
        Stub(0x6018F0,S_AfterProcCol); Stub(0x6019B0,S_AfterPreRender); Stub(0x5DFDF0,S_PosAttPed); Stub(0x546FF0,S_PosAttEnt); Stub(0x69EE60,S_Messages);
        Stub(0x56F8D0,S_PlayerProcess); Stub(0x6085E0,S_ScriptedTask); Stub(0x5FC800,S_GroupsProcess); Stub(0x542DD0,S_ApplyMove); Stub(0x542E20,S_ApplyTurn);
        Stub(0x5A1980,S_TempObjects); Stub(0x539450,S_FireExtinguishPoint); Stub(0x7369E0,S_Explosions); Stub(0x7399B0,S_Projectiles); Stub(0x707770,S_Shadows); Stub(0x4563A0,S_Pickups);
        Stub(0x44F460,S_Closest); Stub(0x821982,S_Printf);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    RestoreAll();
                    Rng rng; rng.s=Mix(uint32(i)*2654435761u^uint32(f*0x9E3779B9u),0x57D)|1u; seedG=rng.s^0x1234567u; trace=json::array();
                    const uint16 scanNow=uint16(rng.chance(4) ? 0xFFFF : rng.next());
                    BuildEnv(rng,scanNow);
                    // globales por defecto
                    *reinterpret_cast<uint32*>(FRAME)=rng.next(); *reinterpret_cast<float*>(STEP)=rng.uni(0.3f,3.f); *reinterpret_cast<uint8*>(SKIP)=0;
                    std::memset(reinterpret_cast<void*>(WFLAGS),0,8); *reinterpret_cast<uint8*>(REPLAY)=0; *reinterpret_cast<uint8*>(CUT_RUN)=0;
                    std::memset(reinterpret_cast<void*>(CUT_OBJS),0,200); std::memset(reinterpret_cast<void*>(PLAYERS),0,0x320);
                    for(int k=0;k<8;++k)*reinterpret_cast<uint8*>(GROUP_ACTIVE+size_t(k))=rng.chance(60);
                    // nodos de ruta falsos (area PATH_AREA) y su direccion de respuesta
                    for(int k=0;k<9;++k){ const size_t node=k<8 ? size_t(k) : size_t(0xFFFF); for(int c=0;c<3;++c)At<int16>(pathNodes.data(),node*0x1C+8+size_t(c)*2)=int16(int32(rng.next()%4000)-2000); }
                    *reinterpret_cast<void**>(PATH_NODES+size_t(PATH_AREA)*4)=pathNodes.data();
                    { const uint32 w=rng.next()%100; nodeRet=w<60 ? (uint32(PATH_AREA)|((rng.next()%8)<<16)) : (w<85 ? (0xFFFFu|((rng.next()%8)<<16)) : (uint32(PATH_AREA)|(0xFFFFu<<16))); }
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr; json args=json::object();
                    auto Shuffle=[&](std::vector<uint8*> v){ for(size_t k=v.size();k>1;--k)std::swap(v[k-1],v[rng.next()%k]); return v; };
                    auto Moving=[&](uint32 pct){ for(auto* e:Shuffle(Phys()))if(rng.chance(pct))AddDouble(reinterpret_cast<uint32*>(MOVING),e); };
                    auto SetXY=[&](uint8* e,float x,float y){ auto* p=EntPos(e); p->x=x; p->y=y; };
                    auto Area=[&](float& mnx,float& mny,float& mxx,float& mxy,float base,float span){ mnx=rng.uni(-base,base*0.5f); mny=rng.uni(-base,base*0.5f); mxx=mnx+rng.uni(1.f,span); mxy=mny+rng.uni(1.f,span); };

                    if(nm=="ProcessForAnimViewer"||nm=="ProcessPedsAfterPreRender"){
                        Moving(65); if(nm=="ProcessPedsAfterPreRender")*reinterpret_cast<uint8*>(SKIP)=rng.chance(25);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="RemoveStaticObjects"){
                        for(auto* b:B)for(uint32 k=1+rng.next()%3;k;--k)AddSingle(SectorB(int(rng.next()%120),int(rng.next()%120)),b);
                        for(auto* d:D)for(uint32 k=1+rng.next()%3;k;--k)AddDouble(SectorD(int(rng.next()%120),int(rng.next()%120)),d);
                        for(auto* o:O)for(uint32 k=1+rng.next()%2;k;--k)AddDouble(RepeatL(int(rng.next()%16),int(rng.next()%16),2),o);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="RemoveEntityInsteadOfProcessingIt"){
                        auto all=AllEnts(); auto* e=rng.pick(all); if((At<uint8>(e,0x36)&7)==3&&rng.chance(40))playerPed=e;
                        args["entity"]=Id(e); reinterpret_cast<void(__cdecl*)(void*)>(a)(e);
                    }
                    else if(nm=="CallOffChaseForAreaSectorListVehicles"||nm=="CallOffChaseForAreaSectorListPeds"){
                        const bool veh=nm.find("Vehicles")!=std::string::npos;
                        auto& src=veh ? V : P; uint32* head=reinterpret_cast<uint32*>(Alloc(4));
                        for(auto* e:Shuffle(src))if(rng.chance(80))AddDouble(head,e);
                        float x1,y1,x2,y2; Area(x1,y1,x2,y2,120.f,160.f);
                        float bx1=x1-10.f,by1=y1-10.f,bx2=x2+10.f,by2=y2+10.f;
                        if(rng.chance(20)){ bx1=rng.uni(-200.f,200.f); by1=rng.uni(-200.f,200.f); bx2=bx1+rng.uni(0.f,300.f); by2=by1+rng.uni(0.f,300.f); }
                        // el area contra la que se prueba la posicion: vehiculos, la ampliada; peatones, la exacta (se mira antes de cambiar x1..y2)
                        const float pminX=veh ? bx1 : x1, pminY=veh ? by1 : y1, pmaxX=veh ? bx2 : x2, pmaxY=veh ? by2 : y2;
                        if(veh&&rng.chance(6)){ x1=-std::numeric_limits<float>::infinity(); x2=std::numeric_limits<float>::infinity(); }
                        if(veh&&rng.chance(6)){ y1=-std::numeric_limits<float>::infinity(); y2=std::numeric_limits<float>::infinity(); }
                        for(auto* e:src){ if(rng.chance(55)){ SetXY(e,rng.uni(pminX,pmaxX),rng.uni(pminY,pmaxY)); }
                            if(rng.chance(14))SetXY(e,rng.chance(50) ? pminX : pmaxX,EntPos(e)->y);
                            if(rng.chance(14))SetXY(e,EntPos(e)->x,rng.chance(50) ? pminY : pmaxY);
                            if(rng.chance(3))EntPos(e)->x=std::bit_cast<float>(0x7FC00000u); }
                        args["area"]=json::array({F(x1),F(y1),F(x2),F(y2),F(bx1),F(by1),F(bx2),F(by2)});
                        reinterpret_cast<void(__cdecl*)(void*,float,float,float,float,float,float,float,float)>(a)(head,x1,y1,x2,y2,bx1,by1,bx2,by2);
                    }
                    else if(nm=="CallOffChaseForArea"){
                        float mnx,mny,mxx,mxy; Area(mnx,mny,mxx,mxy,150.f,100.f);
                        if(rng.chance(5)){ std::swap(mnx,mxx); }
                        auto Sec=[&](float v){ const float s=v*0.02f; return int32(int64(std::floor(double(s+60.0f)))); };
                        const int sx0=Sec(mnx-10.f), sx1=Sec(mxx+10.f), sy0=Sec(mny-10.f), sy1=Sec(mxy+10.f);
                        for(int y=sy0-1;y<=sy1+1&&y<sy0+14;++y)for(int x=sx0-1;x<=sx1+1&&x<sx0+14;++x){
                            for(auto* e:V)if(rng.chance(22))AddDouble(RepeatL(x,y,0),e);
                            for(auto* e:P)if(rng.chance(22))AddDouble(RepeatL(x,y,1),e); }
                        for(auto* e:Phys()){ if(rng.chance(60))SetXY(e,rng.uni(mnx-12.f,mxx+12.f),rng.uni(mny-12.f,mxy+12.f)); }
                        if(rng.chance(10))*reinterpret_cast<uint16*>(SCAN)=0xFFFF;
                        args["area"]=json::array({F(mnx),F(mny),F(mxx),F(mxy)});
                        reinterpret_cast<void(__cdecl*)(float,float,float,float)>(a)(mnx,mny,mxx,mxy);
                    }
                    else if(nm=="ClearCarsFromArea"||nm=="ClearPedsFromArea"){
                        float b6[6]; Area(b6[0],b6[1],b6[3],b6[4],300.f,300.f); b6[2]=rng.uni(-160.f,0.f); b6[5]=b6[2]+rng.uni(1.f,200.f);
                        auto& src=nm=="ClearCarsFromArea" ? V : P;
                        if(rng.chance(40))for(auto* e:src)if(rng.chance(60)){ auto* p=EntPos(e); p->x=rng.uni(b6[0],b6[3]); p->y=rng.uni(b6[1],b6[4]); p->z=rng.uni(b6[2],b6[5]); }
                        // limites exactamente sobre la posicion de una entidad (la prueba es inclusiva)
                        if(rng.chance(25)){ auto* e=rng.pick(src); const CVector p=*EntPos(e); const int k=int(rng.next()%6); b6[k]=k%3==0 ? p.x : (k%3==1 ? p.y : p.z);
                            if(rng.chance(50)){ const int j=int(rng.next()%3)+3; b6[j]=j%3==0 ? p.x : (j%3==1 ? p.y : p.z); } }
                        if(nm=="ClearCarsFromArea"&&rng.chance(40))At<void*>(playerPed,0x584)=rng.pick(V);
                        args["area"]=json::array({F(b6[0]),F(b6[1]),F(b6[2]),F(b6[3]),F(b6[4]),F(b6[5])});
                        reinterpret_cast<void(__cdecl*)(float,float,float,float,float,float)>(a)(b6[0],b6[1],b6[2],b6[3],b6[4],b6[5]);
                    }
                    else if(nm=="ExtinguishAllCarFiresInArea"||nm=="ClearExcitingStuffFromArea"){
                        CVector pt{rng.uni(-300.f,300.f),rng.uni(-300.f,300.f),rng.uni(-100.f,60.f)}; float radius=rng.uni(0.f,250.f);
                        if(rng.chance(15)){ auto* e=rng.pick(V); pt=*EntPos(e); radius=rng.chance(50) ? 0.f : rng.uni(0.f,1.f); }
                        if(rng.chance(30))for(auto* e:Phys())if(rng.chance(60)){ auto* p=EntPos(e); p->x=pt.x+rng.uni(-radius,radius); p->y=pt.y+rng.uni(-radius,radius); }
                        if(nm=="ClearExcitingStuffFromArea"){ if(rng.chance(40))At<void*>(playerPed,0x584)=rng.pick(V); const uint8 flag=rng.chance(50);
                            args["flag"]=flag; reinterpret_cast<void(__cdecl*)(const CVector*,float,uint8)>(a)(&pt,radius,flag); }
                        else reinterpret_cast<void(__cdecl*)(float,float,float,float)>(a)(pt.x,pt.y,pt.z,radius);
                        args["point"]=json::array({F(pt.x),F(pt.y),F(pt.z),F(radius)});
                    }
                    else if(nm=="PrintCarChanges"){
                        auto* models=reinterpret_cast<uint32*>(MODELS);
                        for(int k=0;k<110;++k)models[k]=rng.next();
                        auto* vstore=At<uint8*>(*reinterpret_cast<void**>(POOL_VEH),0);
                        std::array<bool,NV> usedSlot{};
                        for(auto* e:V){ const size_t slot=size_t(e-vstore)/VEH_STRIDE; usedSlot[slot]=true;
                            if(rng.chance(45))models[slot]=At<uint32>(e,0x590)==0 ? uint32(int32(int16(At<uint16>(e,0x22)))) : 0u; }
                        for(int k=0;k<NV;++k)if(!usedSlot[size_t(k)]&&rng.chance(45))models[k]=0;
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="ProcessAttachedEntities"){
                        auto all=AllEnts(); for(auto* e:V)At<void*>(e,0xFC)=rng.chance(45) ? static_cast<void*>(rng.pick(all)) : nullptr;
                        for(auto* e:O)At<void*>(e,0xFC)=rng.chance(45) ? static_cast<void*>(rng.pick(all)) : nullptr;
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="RemoveReferencesToDeletedObject"){
                        auto all=AllEnts(); auto* e=rng.pick(all);
                        for(auto* p:P)if(rng.chance(40))At<void*>(p,0x568)=e;
                        args["entity"]=Id(e); reinterpret_cast<void(__cdecl*)(void*)>(a)(e);
                    }
                    else if(nm=="RemoveFallenPeds"||nm=="RemoveFallenCars"){
                        for(auto* e:Phys()){ if(rng.chance(10))EntPos(e)->z=-100.f; if(rng.chance(5))EntPos(e)->z=std::bit_cast<float>(0x7FC00000u); if(rng.chance(40))EntPos(e)->z=rng.uni(-130.f,-90.f); }
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else { // Process
                        Moving(75);
                        for(auto* e:O)if(rng.chance(50))AddDouble(reinterpret_cast<uint32*>(CTRLLIST),e);
                        *reinterpret_cast<uint8*>(WFLAGS+1)=rng.chance(25); *reinterpret_cast<uint8*>(CUT_RUN)=rng.chance(60);
                        for(size_t k=0;k<C.size();++k)if(rng.chance(75))At<void*>(reinterpret_cast<void*>(CUT_OBJS),k*4)=C[k];
                        *reinterpret_cast<uint8*>(REPLAY)=rng.chance(25) ? 1 : (rng.chance(10) ? 2 : 0);
                        { uint32 fc=rng.next(); if(rng.chance(40))fc&=~63u; if(rng.chance(50))fc=(fc&~7u)|(rng.chance(50) ? 1u : 5u); *reinterpret_cast<uint32*>(FRAME)=fc; }
                        At<void*>(reinterpret_cast<void*>(PLAYERS),0)=rng.chance(80) ? playerPed : nullptr;
                        At<void*>(reinterpret_cast<void*>(PLAYERS+0x190),0)=rng.chance(30) ? static_cast<void*>(P.back()) : nullptr;
                        for(auto* e:Phys())if(rng.chance(25))EntPos(e)->z=rng.uni(-130.f,-90.f);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    out["args"]=args; out["trace"]=trace;
                    out["arena"]=Fnv(arena.data(),bump); out["world"]=Fnv(reinterpret_cast<void*>(SECTORS),WORLD_END-SECTORS);
                    { uint64 g=1469598103934665603ull; for(auto& s:saved)g=Fnv(reinterpret_cast<void*>(s.a),s.v.size(),g); out["globals"]=g; }
                    { json eh=json::array(); for(auto* v:{&V,&P,&O,&B,&D,&C}){ const size_t stride=v==&V ? VEH_STRIDE : (v==&P ? PED_STRIDE : (v==&B||v==&D ? BLD_STRIDE : OBJ_STRIDE));
                            for(auto* e:*v)eh.push_back(Fnv(e,stride)); } out["ents"]=eh; }
                    out["ret"]=ret; { const std::string dumped=out.dump(); out["digest"]=Fnv(dumped.data(),dumped.size()); }
                    RestoreAll();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},
                             {"original",json{{"ret",original["ret"]},{"events",original["trace"].size()},{"digest",original["digest"]}}},
                             {"candidate",json{{"ret",candidate["ret"]},{"events",candidate["trace"].size()},{"digest",candidate["digest"]}}}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        while(!patches.empty())patches.pop_back();
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
