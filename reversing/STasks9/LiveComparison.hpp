#pragma once
// Banco dentro del motor: tareas simples de coche (lista de 9, 45 funciones): CTaskSimpleCarFallOut,
// CTaskSimpleCarGetIn, CTaskSimpleCarOpenLockedDoorFromOutside, CTaskSimpleCarShuffle y CTaskSimpleCarSlowBeDraggedOut.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: los constructores se llaman en la
// direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del vptr); los metodos virtuales se
// llaman por la entrada de la vtable del EXE y los callbacks como __cdecl(anim, tarea).
// Peaton falso (vtable de trampas, clump con byte de tipo 2, banderas 0x46C con bInVehicle controlado, tipo de peaton),
// vehiculo falso del tamano de CAutomobile (vtable falsa: hueco 28 ProcessOpenDoor, 35 IsDoorClosedU32, 37
// IsDoorMissingU32; handling, matriz, masa, tipo/subtipo, banderas y puertas del CDamageManager propios), utilidad de
// alineacion falsa (buffer) y cuatro asociaciones de animacion falsas.
// Se simulan y trazan: CAnimManager::BlendAnimation, RpAnimBlendClumpGetAssociation, CAnimBlendAssociation::
// SetFinishCallback, CCarEnterExit::RemoveCarSitAnim, CVehicleAnimGroup::GetGroup, CTaskUtilityLineUpPedWithCar::
// ProcessPed, CPed::SetPedPositionInCar/RemoveWeaponWhenEnteringVehicle, CPhysical::ApplyMoveForce,
// CDamageManager::GetDoorStatus_Component/SetDoorStatus_Component (con la semantica del EXE: el hook de la DLL devuelve
// un uint8 y el EXE compara eax entero) y el generador aleatorio (solo para detectar usos).
// Se fijan y restauran CTimer::ms_fTimeStep y el truco CHEAT_SMASH_N_BOOM. Las tareas nuevas del pool se siguen
// (m_LastFreeSlot restaurado y huecos libres con 0xCD).
// FIX_BUGS: el destructor de CTaskSimpleCarFallOut en la DLL quita el callback de la animacion (el EXE no): sus
// fixtures de destructor van sin animacion.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Tasks/TaskTypes/TaskSimpleCarFallOut.h"
#include "Tasks/TaskTypes/TaskSimpleCarGetIn.h"
#include "Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.h"
#include "Tasks/TaskTypes/TaskSimpleCarShuffle.h"
#include "Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h"
#include "Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Entity/Vehicle/Automobile.h"
#include "VehicleAnimGroupData.h"
#include "DamageManager.h"
#include "CarEnterExit.h"
#include "../ProbeRuntime.hpp"

namespace STasks9Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_CLUMP=0x18, PH_FLAGS=0x40, PH_MASS=0x8C,                                  // CEntity / CPhysical
                 P_FLAGS=0x46C, P_PEDTYPE=0x598,                                                            // CPed
                 V_HANDLING=0x384, V_FLAGS=0x428, V_TYPE=0x590, V_SUBTYPE=0x594, V_DAMAGE=0x5A0,            // CVehicle / CAutomobile
                 H_ANIMGROUP=0xDE, A_BLEND=0x18, A_DELTA=0x1C, A_TIME=0x20, A_ID=0x2C, A_FLAGS=0x2E;
constexpr uintptr_t ADDR_TIMESTEP=0xB7CB5C, ADDR_CHEAT_SMASH=0x969164, ANIMGROUPS=0xC1CDC0, EXE_DEFAULT_ANIM_CB=0x4D1390;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehA{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> vehMat{};
alignas(16) inline std::array<uint8,0x100> handling{};
alignas(16) inline std::array<uint8,0x80> clump{};
alignas(16) inline std::array<uint8,0x40> lineup{}, object{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,3> anims{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim2{};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CVehicle* VehA() { return reinterpret_cast<CVehicle*>(vehA.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,80> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline std::unordered_map<uintptr_t,std::string> cbNames;   // callbacks del EXE y de la DLL

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=cbNames.find(v);it!=cbNames.end())return it->second;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    if(v>=ANIMGROUPS&&v<ANIMGROUPS+0x94*30)return "animgroup+"+std::to_string(v-ANIMGROUPS);
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(vehA.data(),vehA.size(),"vehA"))return *s;
    if(auto s=in(vehMat.data(),vehMat.size(),"vehmat"))return *s; if(auto s=in(handling.data(),handling.size(),"handling"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(lineup.data(),lineup.size(),"lineup"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(anim2.data(),anim2.size(),"anim2"))return *s;
    for(size_t k=0;k<anims.size();++k)if(auto s=in(anims[k].data(),anims[k].size(),("anims"+std::to_string(k)).c_str()))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s; if(auto s=in(vehVtbl.data(),sizeof(vehVtbl),"vehvtbl"))return *s;
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

// configuracion por caso de las dependencias simuladas
inline uint32 assocMask{}, rngState{}, doorMask{}; inline int32 groupRet{}; inline bool lineupRet{};

// vtables falsas: todo trazado
template<int N> void* __fastcall PTrap(void* p,void*) { trace.push_back({"ped_unexpected_slot",N,Name(uintptr_t(p))}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
template<int N> void* __fastcall VTrap(void* v,void*) { trace.push_back({"veh_unexpected_slot",N,Name(uintptr_t(v))}); return nullptr; }
template<int... N> void FillVTraps(std::integer_sequence<int,N...>) { ((vehVtbl[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
inline void __fastcall OpenDoor(CVehicle* v,void*,CPed* p,uint32 door,uint32 grp,uint32 id,float t) { trace.push_back({"process_open_door",Name(uintptr_t(v)),Name(uintptr_t(p)),door,grp,id,F(t)}); }
inline bool __fastcall DoorClosed(CVehicle* v,void*,uint32 door) { const bool r=(doorMask>>(door&15))&1; trace.push_back({"is_door_closed",Name(uintptr_t(v)),door,r}); return r; }
inline bool __fastcall DoorMissing(CVehicle* v,void*,uint32 door) { const bool r=(doorMask>>(16+(door&15)))&1; trace.push_back({"is_door_missing",Name(uintptr_t(v)),door,r}); return r; }

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 grp,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),grp,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(anim2.data()); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) {
    trace.push_back({"clump_get_association",Name(uintptr_t(c)),id});
    int bit=-1;
    switch(id){ case ANIM_ID_FLOOR_HIT_F: bit=0; break; case ANIM_ID_FLOOR_HIT: bit=1; break; case ANIM_ID_CAR_JACKEDLHS: case ANIM_ID_CAR_JACKEDRHS: bit=2; break; default: break; }
    return bit>=0&&((assocMask>>bit)&1) ? reinterpret_cast<CAnimBlendAssociation*>(anims[size_t(bit)].data()) : nullptr;
}
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __cdecl RemoveSitAnim(const CPed* p) { trace.push_back({"remove_car_sit_anim",Name(uintptr_t(p))}); }
inline int32 __fastcall GetGroup(const void* g,void*,int32 id) { trace.push_back({"anim_group_get_group",Name(uintptr_t(g)),id,groupRet}); return groupRet; }
inline bool __fastcall LineUp(void* u,void*,CPed* p,CVehicle* v,CAnimBlendAssociation* a) { trace.push_back({"line_up_process_ped",Name(uintptr_t(u)),Name(uintptr_t(p)),Name(uintptr_t(v)),Name(uintptr_t(a))}); return lineupRet; }
inline void __fastcall PosInCar(CPed* p,void*) { trace.push_back({"set_ped_position_in_car",Name(uintptr_t(p))}); }
inline void __fastcall RemoveWeapon(CPed* p,void*,int32 a) { trace.push_back({"remove_weapon_when_entering_vehicle",Name(uintptr_t(p)),a}); }
inline void __fastcall MoveForce(CPhysical* e,void*,float x,float y,float z) { trace.push_back({"apply_move_force",Name(uintptr_t(e)),F(x),F(y),F(z)}); }
// CDamageManager con la semantica del EXE (0x6C2250 / 0x6C21E0): puertas 8..11 en los bytes +0xB..+0xE
inline uint32 __fastcall GetDoorStatus(const void* dm,void*,int32 comp) {
    const uint32 r=uint32(comp-8)<=3 ? uint32(static_cast<const uint8*>(dm)[0xB+(comp-8)]) : 4u;
    trace.push_back({"get_door_status_component",Name(uintptr_t(dm)),comp,r}); return r;
}
inline void __fastcall SetDoorStatus(void* dm,void*,int32 comp,uint32 status) {
    trace.push_back({"set_door_status_component",Name(uintptr_t(dm)),comp,status&0xFF});
    if(uint32(comp-8)<=3)static_cast<uint8*>(dm)[0xB+(comp-8)]=uint8(status);
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t Follow(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

enum Kind { FALLOUT, GETIN, LOCKED, SHUFFLE, SLOWDRAG };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; const char* cb; uintptr_t cbAddr; size_t vehOff, doorOff, lineOff; };
inline const std::array<Cls,5> classes{{
    {"CTaskSimpleCarFallOut",0x86EFD0,0x1C,FALLOUT,"FinishAnimFallOutCB",0x648EE0,0x10,0x14,0x18},
    {"CTaskSimpleCarGetIn",0x86ED50,0x24,GETIN,"FinishAnimCarGetInCB",0x6467F0,0x18,0x1C,0x20},
    {"CTaskSimpleCarOpenLockedDoorFromOutside",0x86ECC0,0x1C,LOCKED,"FinishAnimCarOpenLockedDoorFromOutsideCB",0x6461A0,0x10,0x14,0x18},
    {"CTaskSimpleCarShuffle",0x86ED74,0x1C,SHUFFLE,"FinishAnimCarShuffleCB",0x646970,0x10,0x14,0x18},
    {"CTaskSimpleCarSlowBeDraggedOut",0x86EF0C,0x20,SLOWDRAG,"FinishAnimCarSlowBeDraggedOutCB",0x6483A0,0x10,0x14,0x1C},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=45;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleCarFallOut","Clone",0,0x86EFD0,1,128},
    {"CTaskSimpleCarFallOut","Constructor",0x648D60,0x86EFD0,-1,256},
    {"CTaskSimpleCarFallOut","Destructor",0x648DE0,0x86EFD0,-1,128},
    {"CTaskSimpleCarFallOut","FinishAnimFallOutCB",0x648EE0,0x86EFD0,-1,512},
    {"CTaskSimpleCarFallOut","GetTaskType",0,0x86EFD0,4,64},
    {"CTaskSimpleCarFallOut","MakeAbortable",0,0x86EFD0,6,256},
    {"CTaskSimpleCarFallOut","ProcessPed",0,0x86EFD0,7,1024},
    {"CTaskSimpleCarFallOut","SetPedPosition",0,0x86EFD0,8,512},
    {"CTaskSimpleCarFallOut","StartAnim",0x64CCE0,0x86EFD0,-1,256},
    {"CTaskSimpleCarGetIn","Clone",0,0x86ED50,1,128},
    {"CTaskSimpleCarGetIn","Constructor",0x646690,0x86ED50,-1,256},
    {"CTaskSimpleCarGetIn","Destructor",0x646710,0x86ED50,-1,128},
    {"CTaskSimpleCarGetIn","FinishAnimCarGetInCB",0x6467F0,0x86ED50,-1,256},
    {"CTaskSimpleCarGetIn","GetTaskType",0,0x86ED50,4,64},
    {"CTaskSimpleCarGetIn","MakeAbortable",0,0x86ED50,6,256},
    {"CTaskSimpleCarGetIn","ProcessPed",0,0x86ED50,7,2048},
    {"CTaskSimpleCarGetIn","SetPedPosition",0,0x86ED50,8,256},
    {"CTaskSimpleCarGetIn","StartAnim",0x64B2D0,0x86ED50,-1,512},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","Clone",0,0x86ECC0,1,128},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","Constructor",0x646070,0x86ECC0,-1,256},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","Destructor",0x6460F0,0x86ECC0,-1,128},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","FinishAnimCarOpenLockedDoorFromOutsideCB",0x6461A0,0x86ECC0,-1,256},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","GetTaskType",0,0x86ECC0,4,64},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","MakeAbortable",0,0x86ECC0,6,256},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","ProcessPed",0,0x86ECC0,7,512},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","SetPedPosition",0,0x86ECC0,8,256},
    {"CTaskSimpleCarOpenLockedDoorFromOutside","StartAnim",0x64AE00,0x86ECC0,-1,256},
    {"CTaskSimpleCarShuffle","Clone",0,0x86ED74,1,128},
    {"CTaskSimpleCarShuffle","Constructor",0x646810,0x86ED74,-1,256},
    {"CTaskSimpleCarShuffle","Destructor",0x646890,0x86ED74,-1,128},
    {"CTaskSimpleCarShuffle","FinishAnimCarShuffleCB",0x646970,0x86ED74,-1,256},
    {"CTaskSimpleCarShuffle","GetTaskType",0,0x86ED74,4,64},
    {"CTaskSimpleCarShuffle","MakeAbortable",0,0x86ED74,6,256},
    {"CTaskSimpleCarShuffle","ProcessPed",0,0x86ED74,7,512},
    {"CTaskSimpleCarShuffle","SetPedPosition",0,0x86ED74,8,512},
    {"CTaskSimpleCarShuffle","StartAnim",0x64B3E0,0x86ED74,-1,256},
    {"CTaskSimpleCarSlowBeDraggedOut","Clone",0,0x86EF0C,1,128},
    {"CTaskSimpleCarSlowBeDraggedOut","Constructor",0x648230,0x86EF0C,-1,256},
    {"CTaskSimpleCarSlowBeDraggedOut","Destructor",0x6482C0,0x86EF0C,-1,128},
    {"CTaskSimpleCarSlowBeDraggedOut","FinishAnimCarSlowBeDraggedOutCB",0x6483A0,0x86EF0C,-1,256},
    {"CTaskSimpleCarSlowBeDraggedOut","GetTaskType",0,0x86EF0C,4,64},
    {"CTaskSimpleCarSlowBeDraggedOut","MakeAbortable",0,0x86EF0C,6,512},
    {"CTaskSimpleCarSlowBeDraggedOut","ProcessPed",0,0x86EF0C,7,2048},
    {"CTaskSimpleCarSlowBeDraggedOut","SetPedPosition",0,0x86EF0C,8,1024},
    {"CTaskSimpleCarSlowBeDraggedOut","StartAnim",0x64C190,0x86EF0C,-1,512},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(VehA());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks9-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks9-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks9");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks9-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // nombres de los callbacks: direccion del EXE, funcion de la DLL (y su destino si es un thunk)
    cbNames.clear();
    cbNames[EXE_DEFAULT_ANIM_CB]="DefaultAnimCB"; cbNames[std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB)]="DefaultAnimCB";
    cbNames[Follow(std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))]="DefaultAnimCB";
    for(auto& c:classes){
        auto* h=FindHook(c.cls,c.cb);
        if(!h||h->Symbol()[0]!='S'){Write({{"event","error"},{"reason","callback hook not found"},{"function",std::string(c.cls)+"::"+c.cb}});return;}
        const auto our=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
        cbNames[c.cbAddr]=c.cb; cbNames[our]=c.cb; cbNames[Follow(our)]=c.cb;
    }
    FillPTraps(std::make_integer_sequence<int,64>{});
    FillVTraps(std::make_integer_sequence<int,80>{});
    vehVtbl[28]=reinterpret_cast<void*>(&OpenDoor); vehVtbl[35]=reinterpret_cast<void*>(&DoorClosed); vehVtbl[37]=reinterpret_cast<void*>(&DoorMissing);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIMESTEP); const float savedStep=timeStep;
    auto& cheat=*reinterpret_cast<uint8*>(ADDR_CHEAT_SMASH); const uint8 savedCheat=cheat;
    auto restoreGlobals=[&]{ timeStep=savedStep; cheat=savedCheat; };
    auto& lfRef=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); const int32 lf0=lfRef;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    // umbrales exactos del EXE
    const float thrGetIn=10.f*std::bit_cast<float>(0x3D088889u), thrDrag=1.7f, thrShuffle=0.9f;
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), a2e(0x4D68B0,GetAssoc);
        EntryPatch a3(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a3e(0x4CEBE0,SetFinishCb);
        EntryPatch c1(bit_cast<uintptr_t>(&CCarEnterExit::RemoveCarSitAnim),RemoveSitAnim), c1e(0x64F680,RemoveSitAnim);
        EntryPatch c2(bit_cast<uintptr_t>(static_cast<AssocGroupId(CVehicleAnimGroup::*)(AnimationId) const>(&CVehicleAnimGroup::GetGroup)),GetGroup), c2e(0x6E3B00,GetGroup);
        EntryPatch c3(bit_cast<uintptr_t>(&CTaskUtilityLineUpPedWithCar::ProcessPed),LineUp), c3e(0x6513A0,LineUp);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetPedPositionInCar),PosInCar), p1e(0x5DF910,PosInCar);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::RemoveWeaponWhenEnteringVehicle),RemoveWeapon), p2e(0x5E6370,RemoveWeapon);
        EntryPatch p3(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),MoveForce), p3e(0x5429F0,MoveForce);
        EntryPatch d1(bit_cast<uintptr_t>(&CDamageManager::GetDoorStatus_Component),GetDoorStatus), d1e(0x6C2250,GetDoorStatus);
        EntryPatch d2(bit_cast<uintptr_t>(&CDamageManager::SetDoorStatus_Component),SetDoorStatus), d2e(0x6C21E0,SetDoorStatus);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; const auto& cl=ClassOf(fn.cls); int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto around=[&](float t){ switch(next()%4){ case 0: return t; case 1: return std::nextafter(t,100.f); case 2: return std::nextafter(t,-100.f); default: return uni(0.f,2.f*t); } };
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { lfRef=lf0; auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x9C9Cu; if(!st)st=1; trace=json::array(); freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    rngState=next();
                    timeStep=next()%3 ? 1.f : uni(0.1f,3.f); cheat=uint8(next()%2);
                    // peaton
                    std::memset(pedA.data(),0,pedA.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    CPed* const pd=PedA(); pd->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(pd)=pedVtbl.data(); At<void*>(pd,E_CLUMP)=clump.data();
                    At(pd,P_FLAGS)=(next()&~0x100u)|(next()%2 ? 0x100u : 0u);
                    { static const uint32 types[]{0,1,0,1,4,6,7}; At(pd,P_PEDTYPE)=types[next()%7]; }
                    // vehiculo
                    std::memset(vehA.data(),0,vehA.size()); std::memset(vehMat.data(),0,vehMat.size());
                    CVehicle* const vh=VehA(); vh->SetType(ENTITY_TYPE_VEHICLE); *reinterpret_cast<void**>(vh)=vehVtbl.data();
                    for(auto& b:handling)b=uint8(next()); handling[H_ANIMGROUP]=uint8(next()%30); At<void*>(vh,V_HANDLING)=handling.data();
                    { CMatrix m{}; m.SetRotate(uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                      std::memcpy(vehMat.data(),&m,sizeof(CMatrix)); At<void*>(vh,E_MATRIX)=vehMat.data(); }
                    At<float>(vh,PH_MASS)=next()%4 ? uni(100.f,3000.f) : 1.f; At(vh,PH_FLAGS)=next(); At(vh,V_FLAGS)=next(); At(vh,V_FLAGS+4)=next();
                    At(vh,V_TYPE)=next()%4 ? 0u : uint32(1+next()%10);
                    { static const uint32 sub[]{10,10,0,9,3}; At(vh,V_SUBTYPE)=sub[next()%5]; }
                    for(size_t k=0;k<0x18;++k)vehA[V_DAMAGE+k]=uint8(next()); for(size_t k=0;k<4;++k)vehA[V_DAMAGE+0xB+k]=uint8(next()%5);
                    // utilidad de alineacion y asociaciones (floats controlados, sin NaN de senalizacion)
                    for(auto& b:lineup)b=uint8(next());
                    for(auto* a:{&anims[0],&anims[1],&anims[2],&anim2}){ for(auto& b:*a)b=uint8(next());
                        const float blends[]{0.f,1.f,thrShuffle,std::nextafter(thrShuffle,2.f),std::nextafter(thrShuffle,0.f),-0.f};
                        At<float>(a->data(),A_BLEND)=next()%2 ? uni(0.f,1.f) : blends[next()%6];
                        At<float>(a->data(),A_DELTA)=uni(-4.f,4.f);
                        At<float>(a->data(),A_TIME)=next()%2 ? around(thrGetIn) : around(thrDrag);
                        { static const int16 ids[]{359,360,361,362,363,372,378,379,0}; At<int16>(a->data(),A_ID)=next()%6 ? ids[next()%9] : int16(next()%500); }
                        At<uint16>(a->data(),A_FLAGS)=uint16(next()); }
                    assocMask=next()&7; doorMask=next(); lineupRet=next()%2;
                    { static const int32 grps[]{107,107,88,89,90,0}; groupRet=grps[next()%6]; }
                    const int prio=next()%8==0 ? 3 : int(next()%3);
                    // tarea probada
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor", isCB=nm.ends_with("CB");
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    At<uint8>(o,8)=uint8(next()%3==0);
                    At<void*>(o,0xC)=next()%3 ? static_cast<void*>(anim2.data()) : nullptr;
                    if(isDtor&&cl.kind==FALLOUT)At<void*>(o,0xC)=nullptr; // FIX_BUGS en la DLL (ver cabecera)
                    const bool needVeh=nm=="StartAnim"||(nm=="ProcessPed"&&At<void*>(o,0xC)==nullptr&&!At<uint8>(o,8));
                    At<void*>(o,cl.vehOff)=(needVeh||next()%6) ? static_cast<void*>(vh) : nullptr;
                    // puertas: GetIn (StartAnim) y SlowBeDraggedOut (ComputeAnimID) no tienen caso por defecto en el EXE (valor sin inicializar)
                    auto pickDoor=[&]()->int32{
                        if(cl.kind==GETIN){ static const int32 d[]{8,9,10,11,18,10}; return d[next()%6]; }
                        if(cl.kind==SLOWDRAG){ static const int32 d[]{8,9,10,11}; return d[next()%4]; }
                        static const int32 d[]{8,9,10,11,18,0,10,11}; return next()%8 ? d[next()%8] : int32(next()%40);
                    };
                    At<int32>(o,cl.doorOff)=pickDoor();
                    At<void*>(o,cl.lineOff)=lineup.data();
                    if(cl.kind==SLOWDRAG){ At<uint8>(o,0x18)=uint8(next()%2); At<uint8>(o,0x19)=uint8(next()%2); }
                    if(isDtor)Reg(At<void*>(o,cl.vehOff),static_cast<uint8*>(o)+cl.vehOff);
                    CVehicle* const ctorVeh=next()%6 ? vh : nullptr; const int32 ctorDoor=pickDoor(); const uint32 ctorDrag=next()%2;
                    void* const cbAnim=next()%2 ? static_cast<void*>(anim2.data()) : static_cast<void*>(anims[0].data());
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        if(cl.kind==SLOWDRAG)rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,void*,uint32)>(target)(o,ctorVeh,ctorDoor,lineup.data(),ctorDrag));
                        else rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,void*)>(target)(o,ctorVeh,ctorDoor,lineup.data())); }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,nullptr)!=0;
                    else if(nm=="ProcessPed"||nm=="SetPedPosition")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*)>(target)(o,pd)!=0;
                    else if(nm=="StartAnim")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(isCB)reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(cbAnim,o);
                    setState(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor&&nm!="StartAnim"&&!isCB)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(pd),static_cast<CEntity*>(vh)})refs.push_back(refList(e));
                    json animWords=json::array(); for(auto& a:anims)animWords.push_back(Region(a.data(),a.size())); animWords.push_back(Region(anim2.data(),anim2.size()));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"vehA",EntHash(vehA)},
                            {"veh_fields",{At(vh,PH_FLAGS),At(vh,V_FLAGS),At(vh,V_FLAGS+4),Region(vehA.data()+V_DAMAGE,0x18)}},
                            {"anims",animWords},{"lineup",Region(lineup.data(),lineup.size())},{"vehmat",Hash(vehMat.data(),vehMat.size())},
                            {"handling",Hash(handling.data(),handling.size())},{"clump",Hash(clump.data(),clump.size())},
                            {"timestep",F(timeStep)},{"cheat",cheat}};
                    restoreGlobals();
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
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs();
    const bool globalsTouched=timeStep!=savedStep||cheat!=savedCheat;
    restoreGlobals(); lfRef=lf0;
    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeStep==savedStep&&cheat==savedCheat;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},{"state_restored",restored}});
}
}
