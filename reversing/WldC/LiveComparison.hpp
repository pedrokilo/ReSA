#pragma once
// Banco dentro del motor: CWorld, grupo C (27 funciones estaticas):
// Initialise, Add, Remove, CameraToIgnoreThisObject, CastShadowSectorList<CPtrListDoubleLink<CPhysical*>>, ClearForRestart, ClearScanCodes,
// FindPlayerCentreOfWorld, FindPlayerCentreOfWorldForMap, FindPlayerCentreOfWorld_NoSniperShift, FindPlayerCoors, FindPlayerEntity,
// FindPlayerHeading, FindPlayerPed, FindPlayerSlotWithPedPointer, FindPlayerSlotWithRemoteVehiclePointer (0x563FD0),
// FindPlayerSlotWithVehiclePointer (0x564000), FindPlayerSpeed, FindPlayerTrain, FindPlayerVehicle, FindPlayerWanted,
// FindUnsuspectingTargetCar, FindUnsuspectingTargetPed, GetCurrentScanCode, GetLodPtrList, GetRepeatSector y GetSector.
// Original (hooks del grupo desactivados) frente a candidato (DLL) con fixtures identicos y comparacion bit a bit.
// Jugadores FALSOS: CWorld::Players[], PlayerInFocus, la camara (posicion y matriz), CCarCtrl::bCarsGeneratedAroundCamera, peatones y vehiculos
// de un pool FALSO (bloques de 0x7C4 y 0xA18 bytes con los campos en sus desplazamientos reales) y matrices falsas; todo se guarda y se restaura.
// Pools FALSOS para FindUnsuspectingTarget*: pools de 8 peatones y 8 vehiculos intercambiados en CPools (0xB74490/0xB74494). Se simulan y trazan
// CPed::IsAlive y CTaskManager::GetActiveTask (el tipo de tarea sale de una vtable falsa), de modo que el orden de recorrido queda en la traza.
// Mundo FALSO para ClearScanCodes, CastShadowSectorList y ClearForRestart: listas de sectores, de repeticion y LOD vacias + pools de nodos LOCALES
// (simple y doble) + pools de entidades FALSOS que cubren el bloque de entidades (la DLL de depuracion valida nodos e Items contra esos pools).
// Add/Remove/ClearForRestart usan una vtable FALSA (destructor, Add, Remove trazados; el resto aborta). Se simulan y trazan (EXE y DLL):
// CEntity::UpdateRwFrame, CMatrix::UpdateRwMatrix, CSimpleTransform::UpdateRwMatrix, CPhysical::AddToMovingList/RemoveFromMovingList,
// CGarages::IsModelIndexADoor, CEntryExitManager::GetPositionRelativeToOutsideWorld, CIplStore::Initialise, CCutsceneMgr::DeleteCutsceneData,
// CProjectileInfo::RemoveAllProjectiles, CObject::DeleteAllTempObjects/DeleteAllMissionObjects, CPopulation::ConvertAllObjectsToDummyObjects,
// CPickups::ReInit y CPools::CheckPoolsEmpty.
// FindPlayerHeading devuelve un float en ST0 (se guardan los 80 bits). FindPlayerHeading y FindPlayerCentreOfWorld_NoSniperShift solo se
// prueban con indices 0 y 1: con un indice negativo el EXE lee Players[-1] (memoria ajena) en los accesos con el indice sin resolver.
// Estado global tocado: se guarda y se restaura antes de cada ejecucion y al final (Players, PlayerInFocus, camara, bCarsGeneratedAroundCamera,
// globales del mundo 0xB7CD60.., codigo de escaneo, punteros de pools, sectores, repeticion, LOD, estado de la cinematica y gCurCamColVars).
#include <fstream>
#include <functional>
#include <limits>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity.h"
#include "Physical.h"
#include "Ped.h"
#include "Building.h"
#include "Dummy.h"
#include "Object.h"
#include "ObjectData.h"
#include "Vehicle.h"
#include "World.h"
#include "Camera.h"
#include "CarCtrl.h"
#include "CutsceneMgr.h"
#include "EntryExitManager.h"
#include "Garages.h"
#include "IplStore.h"
#include "Pickups.h"
#include "Population.h"
#include "ProjectileInfo.h"
#include "PedIntelligence.h"
#include "PlayerInfo.h"
#include "Pools/Pools.h"
#include "Tasks/TaskManager.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "Pools/PtrNodeSingleLinkPool.h"
#include "../ProbeRuntime.hpp"

namespace WldCLive {
inline bool done{}; inline std::ofstream log; inline json trace; inline DWORD mainThread{};
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class V=uint32> inline V& At(void* base,size_t off) { return *reinterpret_cast<V*>(static_cast<uint8*>(base)+off); }
template<class V=uint32> inline V& AtA(uintptr_t addr) { return *reinterpret_cast<V*>(addr); }
// Sigue los saltos de la ILT de la DLL sin entrar en la imagen del EXE
inline uintptr_t Body(uintptr_t p) {
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p;
}
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline std::string Hex80(const uint8* p) { std::string s; for(size_t k=10;k-->0;)s+=std::format("{:02x}",p[k]); return s; }

// --- direcciones del EXE ---
constexpr uintptr_t POOLS=0xB74484, SECTORS=0xB7D0B8, REPEAT=0xB992B8, LODL=0xB99EB8, PLAYERS=0xB7CD98, PIF=0xB7CD74, CARS_CAM=0x9690C1,
    CAMERA=0xB6F028, GLOBALS=0xB7CD60, SCAN=0xB7CD78, CUTSCENE=0xB5F84C, CAMCOLVARS=0x8CCB80;
constexpr size_t SECTORS_SIZE=120*120*8, REPEAT_SIZE=16*16*12, LOD_SIZE=30*30*4, POOLS_SIZE=0x20, PLAYERS_SIZE=0x320, CAMERA_SIZE=0x20, GLOBALS_SIZE=0x1C;
constexpr size_t PS=0x7C4, VS=0xA18, NP=8; // paso de CPed y CVehicle en sus pools
constexpr size_t ES=0xA20; constexpr int NE=8, NDBL=8192, NSGL=8192;

// --- memoria falsa (direcciones fijas: iguales en las dos ejecuciones) ---
alignas(16) inline std::array<uint8,PS*NP> pedMem{};
alignas(16) inline std::array<uint8,VS*NP> vehMem{};
inline std::array<uint8,NP> pedStates{}, vehStates{};
alignas(16) inline std::array<uint8,0x14> pedPoolHdr{}, vehPoolHdr{};
alignas(16) inline std::array<std::array<uint8,0x60>,32> mats{}; // 0..7 peatones, 8..15 vehiculos, 16 camara, 17 entidad de Add/Remove
alignas(16) inline std::array<std::array<uint8,0x40>,NP> intelBuf{};
alignas(16) inline std::array<std::array<uint8,0x20>,NP*2> taskBuf{};
inline std::array<void*,NP> taskOf{}; inline std::array<bool,NP> aliveOf{};
inline std::array<void*,8> taskVt{};
alignas(16) inline std::array<uint8,ES*NE> entMem{};
inline uint8* E(int k) { return entMem.data()+size_t(k)*ES; }
inline uint8* PED(size_t k) { return pedMem.data()+k*PS; }
inline uint8* VEH(size_t k) { return vehMem.data()+k*VS; }
inline std::array<uint8,ES*NE/0x38+1> entStates{};
alignas(16) inline std::array<uint8,0x100> frameBuf{}, clumpBuf{}, objInfoBuf{};
alignas(16) inline std::array<uint8,0x20> outBuf{}, lstBuf{};
alignas(16) inline std::array<uint8,NDBL*12> dblStore{}; inline std::array<uint8,NDBL> dblStates{};
alignas(16) inline std::array<uint8,NSGL*8> sglStore{}; inline std::array<uint8,NSGL> sglStates{};
alignas(16) inline std::array<uint8,0x20> dblHdr{}, sglHdr{}, dblImg{}, sglImg{};
alignas(16) inline std::array<uint8,0x14> wVehHdr{}, wPedHdr{}, wObjHdr{}, wBldHdr{}, wDumHdr{};
inline std::array<void*,64> fvt{};

inline std::string Name(const void* p) {
    const auto u=uintptr_t(p); if(!u)return "null";
    struct R { uintptr_t base; size_t size, stride; const char* tag; };
    const R regions[]{
        {uintptr_t(pedMem.data()),pedMem.size(),PS,"ped"},{uintptr_t(vehMem.data()),vehMem.size(),VS,"veh"},
        {uintptr_t(entMem.data()),entMem.size(),ES,"e"},{uintptr_t(mats.data()),sizeof(mats),sizeof(mats[0]),"m"},
        {uintptr_t(frameBuf.data()),frameBuf.size(),frameBuf.size(),"frame"},{uintptr_t(outBuf.data()),outBuf.size(),outBuf.size(),"out"},
        {uintptr_t(intelBuf.data()),sizeof(intelBuf),0x40,"intel"},
        {uintptr_t(dblStore.data()),dblStore.size(),12,"dn"},{uintptr_t(sglStore.data()),sglStore.size(),8,"sn"},
        {REPEAT,REPEAT_SIZE,12,"rs"},{SECTORS,SECTORS_SIZE,8,"sec"},{LODL,LOD_SIZE,4,"lod"},
        {PLAYERS,PLAYERS_SIZE,0x190,"pl"},{CAMERA,CAMERA_SIZE,CAMERA_SIZE,"cam"},{GLOBALS,GLOBALS_SIZE,GLOBALS_SIZE,"glob"}};
    for(auto& r:regions)if(u>=r.base&&u<r.base+r.size)return std::format("{}#{}+{:x}",r.tag,(u-r.base)/r.stride,(u-r.base)%r.stride);
    const auto* tib=reinterpret_cast<NT_TIB*>(NtCurrentTeb()); if(u>=uintptr_t(tib->StackLimit)&&u<uintptr_t(tib->StackBase))return "stack";
    return std::format("{:08x}",u);
}
inline json V3(const CVector* v) { return v ? json{F(v->x),F(v->y),F(v->z)} : json(nullptr); }

// --- generador de los stubs (mismo estado al empezar cada ejecucion) ---
inline uint32 sst{}, doorSeed{};
inline uint32 Mix(uint32 a,uint32 b) { uint32 h=a*2654435761u^(b+0x9E3779B9u+(a<<6)+(a>>2)); h^=h>>15; h*=2246822519u; h^=h>>13; return h; }
inline void Tr(json j) { if(GetCurrentThreadId()==mainThread)trace.push_back(std::move(j)); }

// --- dependencias simuladas ---
template<int K> void __cdecl NoArgStub() { Tr({"call",K}); }
inline void __fastcall AddStub(uint8* e,void*) { Tr({"add",Name(e)}); }
inline void __fastcall RemoveStub(uint8* e,void*) { Tr({"remove",Name(e)}); }
inline void __fastcall DtorStub(uint8* e,void*,uint32 flag) { Tr({"delete",Name(e),flag}); }
inline void __fastcall MovingAddStub(uint8* e,void*) { Tr({"add_to_moving_list",Name(e)}); }
inline void __fastcall MovingRemStub(uint8* e,void*) { Tr({"remove_from_moving_list",Name(e)}); }
inline void __fastcall RwFrameStub(uint8* e,void*) { Tr({"update_rw_frame",Name(e)}); }
inline void __fastcall MatUpdStub(void* m,void*,void* rw) { Tr({"matrix_update_rw_matrix",Name(m),Name(rw)}); }
inline void __fastcall PlaceUpdStub(void* t,void*,void* rw) { Tr({"placement_update_rw_matrix",Name(t),Name(rw)}); }
inline bool __cdecl DoorStub(int32 model) { Tr({"is_door",model}); return (Mix(doorSeed,uint32(model))&1u)!=0; }
inline void __cdecl OutsideStub(CVector& v) { Tr({"outside_world",V3(&v)}); v.x+=1000.f; v.y-=500.f; v.z*=0.5f; }
inline bool __fastcall AliveStub(uint8* p,void*) { const auto i=size_t((p-pedMem.data())/ptrdiff_t(PS)); Tr({"alive",i}); return i<NP&&aliveOf[i]; }
inline void* __fastcall ActiveTaskStub(uint8* tm,void*) {
    const auto i=size_t((tm-4-reinterpret_cast<uint8*>(intelBuf.data()))/0x40); Tr({"active_task",i}); return i<NP ? taskOf[i] : nullptr;
}
inline int32 __fastcall TaskTypeStub(uint8* t,void*) { return At<int32>(t,8); }

// --- vtable FALSA de entidad ---
template<int K> void __fastcall VTrap(void* self,void*) {
    Write({{"event","error"},{"reason","unexpected virtual call"},{"slot",K},{"self",Name(self)}}); log.flush(); ExitProcess(95);
}
template<int... K> std::array<void*,sizeof...(K)> TrapTable(std::integer_sequence<int,K...>) { return {reinterpret_cast<void*>(&VTrap<K>)...}; }

// --- llamada cdecl con un argumento entero que devuelve un float en ST0 (se guardan los 80 bits) ---
inline std::string CallCdeclIntF0(uintptr_t fnAddr,int32 arg0) {
    uint8 raw[10]{};
    __asm { push arg0
            mov eax, fnAddr
            call eax
            add esp, 4
            fstp tbyte ptr raw }
    return Hex80(raw);
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=27;
inline const std::array<Fn,NFNS> fns{{
    {"CWorld","Initialise",0x5631E0,0,-1,256},{"CWorld","Add",0x563220,0,-1,768},{"CWorld","Remove",0x563280,0,-1,512},
    {"CWorld","CameraToIgnoreThisObject",0x563F40,0,-1,512},
    {"CWorld","CastShadowSectorList<CPtrListDoubleLink<CPhysical*>>",0x563390,0,-1,512},
    {"CWorld","ClearForRestart",0x564360,0,-1,256},{"CWorld","ClearScanCodes",0x563470,0,-1,256},
    {"CWorld","FindPlayerCentreOfWorld",0x56E250,0,-1,768},{"CWorld","FindPlayerCentreOfWorldForMap",0x56E400,0,-1,512},
    {"CWorld","FindPlayerCentreOfWorld_NoSniperShift",0x56E320,0,-1,768},{"CWorld","FindPlayerCoors",0x56E010,0,-1,512},
    {"CWorld","FindPlayerEntity",0x56E120,0,-1,512},{"CWorld","FindPlayerHeading",0x56E450,0,-1,768},
    {"CWorld","FindPlayerPed",0x56E210,0,-1,256},{"CWorld","FindPlayerSlotWithPedPointer",0x563FA0,0,-1,256},
    {"CWorld","FindPlayerSlotWithRemoteVehiclePointer",0x563FD0,0,-1,256},{"CWorld","FindPlayerSlotWithVehiclePointer",0x564000,0,-1,256},
    {"CWorld","FindPlayerSpeed",0x56E090,0,-1,256},{"CWorld","FindPlayerTrain",0x56E160,0,-1,512},
    {"CWorld","FindPlayerVehicle",0x56E0D0,0,-1,768},{"CWorld","FindPlayerWanted",0x56E230,0,-1,256},
    {"CWorld","FindUnsuspectingTargetCar",0x566C90,0,-1,1024},{"CWorld","FindUnsuspectingTargetPed",0x566DA0,0,-1,1024},
    {"CWorld","GetCurrentScanCode",0x407250,0,-1,64},{"CWorld","GetLodPtrList",0x4072C0,0,-1,256},
    {"CWorld","GetRepeatSector",0x4072A0,0,-1,512},{"CWorld","GetSector",0x407260,0,-1,512},
}};

// comprobacion de la disposicion de campos de la DLL frente al EXE (solo direcciones, sin leer nada)
inline std::string LayoutErrors() {
    std::string why;
    auto* pe=reinterpret_cast<CPed*>(PED(0)); auto* ve=reinterpret_cast<CVehicle*>(VEH(0)); auto* en=reinterpret_cast<CEntity*>(E(0));
    auto* ph=reinterpret_cast<CPhysical*>(E(0)); auto* ob=reinterpret_cast<CObject*>(E(0));
    auto* in=reinterpret_cast<CPedIntelligence*>(intelBuf[0].data());
    auto chk=[&](const char* nm,const void* addr,const void* base,size_t off){ if(uintptr_t(addr)-uintptr_t(base)!=off){ why+=nm; why+=' '; } };
    chk("ped.m_pVehicle",&pe->m_pVehicle,pe,0x58C); chk("ped.m_nPedType",&pe->m_nPedType,pe,0x598);
    chk("ped.m_matrix",&pe->m_matrix,pe,0x14); chk("ped.m_vecMoveSpeed",&pe->m_vecMoveSpeed,pe,0x44);
    chk("veh.m_nCreatedBy",&ve->m_nCreatedBy,ve,0x4A4); chk("veh.m_nVehicleType",&ve->m_nVehicleType,ve,0x590); chk("veh.m_nVehicleSubType",&ve->m_nVehicleSubType,ve,0x594);
    chk("veh.m_matrix",&ve->m_matrix,ve,0x14);
    chk("ent.m_matrix",&en->m_matrix,en,0x14); chk("ent.m_placement",&en->m_placement,en,4); chk("ent.m_nFlags",&en->m_nFlags,en,0x1C);
    chk("ent.m_nModelIndex",&en->m_nModelIndex,en,0x22);
    { auto* od=reinterpret_cast<CObjectData*>(objInfoBuf.data()); chk("objdata.m_nCameraAvoidObject",&od->m_nCameraAvoidObject,od,0x1E); }
    chk("obj.m_pObjectInfo",&ob->m_pObjectInfo,ob,0x160); chk("intel.m_TaskMgr",&in->m_TaskMgr,in,4);
    chk("players[1]",&CWorld::Players[1],&CWorld::Players[0],0x190); chk("players.m_pRemoteVehicle",&CWorld::Players[0].m_pRemoteVehicle,&CWorld::Players[0],0xB0);
    chk("players.m_pWanted",&CWorld::Players[0].m_PlayerData.m_pWanted,&CWorld::Players[0],4);
    if(uintptr_t(&CWorld::Players[0])!=PLAYERS)why+="Players ";
    if(uintptr_t(&CWorld::PlayerInFocus)!=PIF)why+="PlayerInFocus ";
    if(uintptr_t(&CWorld::ms_nCurrentScanCode)!=SCAN)why+="ms_nCurrentScanCode ";
    (void)ph;
    return why;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"wldc-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"wldc-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("wldc");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainThread=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"wldc-only.txt"); if(f)std::getline(f,only); }
    if(auto bad=LayoutErrors(); !bad.empty()){Write({{"event","error"},{"reason","layout mismatch"},{"fields",bad}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };

    // --- tablas falsas ---
    { const auto traps=TrapTable(std::make_integer_sequence<int,64>{}); for(int k=0;k<64;++k)fvt[k]=traps[k];
      fvt[0]=reinterpret_cast<void*>(&DtorStub); fvt[2]=reinterpret_cast<void*>(&AddStub); fvt[3]=reinterpret_cast<void*>(&RemoveStub);
      const auto ttraps=TrapTable(std::make_integer_sequence<int,8>{}); for(int k=0;k<8;++k)taskVt[size_t(k)]=ttraps[size_t(k)];
      taskVt[4]=reinterpret_cast<void*>(&TaskTypeStub); }

    // --- estado global guardado ---
    struct Blob { uintptr_t at; std::vector<uint8> data; };
    std::vector<Blob> blobs;
    auto Save=[&](uintptr_t at,size_t n){ blobs.push_back({at,std::vector<uint8>(reinterpret_cast<uint8*>(at),reinterpret_cast<uint8*>(at)+n)}); };
    Save(POOLS,POOLS_SIZE); Save(SECTORS,SECTORS_SIZE); Save(REPEAT,REPEAT_SIZE); Save(LODL,LOD_SIZE); Save(PLAYERS,PLAYERS_SIZE);
    Save(CAMERA,CAMERA_SIZE); Save(CARS_CAM,1); Save(GLOBALS,GLOBALS_SIZE); Save(CUTSCENE,4); Save(CAMCOLVARS,1);
    auto RestoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size()); };
    auto Restored=[&]{ bool ok=true; for(auto& b:blobs)ok&=std::memcmp(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size())==0; return ok; };

    // pools de nodos locales: cabecera construida una sola vez; pools de entidades falsos que cubren el bloque de entidades
    new (dblHdr.data()) CPtrNodeDoubleLinkPool(NDBL,dblStore.data(),dblStates.data()); dblImg=dblHdr;
    new (sglHdr.data()) CPtrNodeSingleLinkPool(NSGL,sglStore.data(),sglStates.data()); sglImg=sglHdr;
    auto PoolHdr=[&](std::array<uint8,0x14>& h,uint8* storage,uint8* states,int32 count){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=storage; At<void*>(h.data(),4)=states;
                                                                                           At<int32>(h.data(),8)=count; At<int32>(h.data(),0xC)=-1; };
    // la capacidad se calcula con el paso mas pequeno (0x38): cubre todo entMem sea cual sea el tamano real del tipo almacenado
    for(auto* h:{&wVehHdr,&wPedHdr,&wObjHdr,&wBldHdr,&wDumHdr})PoolHdr(*h,entMem.data(),entStates.data(),int32(entMem.size()/0x38));
    PoolHdr(pedPoolHdr,pedMem.data(),pedStates.data(),int32(NP)); PoolHdr(vehPoolHdr,vehMem.data(),vehStates.data(),int32(NP));
    auto* const poolPtrs=reinterpret_cast<void**>(POOLS); // 0 simple, 1 doble, 2 entry, 3 peatones, 4 vehiculos, 5 edificios, 6 objetos, 7 dummies

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        std::vector<std::unique_ptr<EntryPatch>> patches; std::vector<uintptr_t> seen;
        auto add=[&](uintptr_t exeAddr,auto fake,uintptr_t dllSym){
            for(uintptr_t at:{exeAddr,dllSym ? Body(dllSym) : uintptr_t(0)}) { if(!at||std::find(seen.begin(),seen.end(),at)!=seen.end())continue; seen.push_back(at); patches.push_back(std::make_unique<EntryPatch>(at,fake)); }
        };
        add(0x405EC0,NoArgStub<0>,bit_cast<uintptr_t>(&CIplStore::Initialise));
        add(0x4D5ED0,NoArgStub<1>,bit_cast<uintptr_t>(&CCutsceneMgr::DeleteCutsceneData));
        add(0x7399B0,NoArgStub<2>,bit_cast<uintptr_t>(&CProjectileInfo::RemoveAllProjectiles));
        add(0x5A18B0,NoArgStub<3>,bit_cast<uintptr_t>(&CObject::DeleteAllTempObjects));
        add(0x5A1910,NoArgStub<4>,bit_cast<uintptr_t>(&CObject::DeleteAllMissionObjects));
        add(0x616420,NoArgStub<5>,bit_cast<uintptr_t>(&CPopulation::ConvertAllObjectsToDummyObjects));
        add(0x456E60,NoArgStub<6>,bit_cast<uintptr_t>(&CPickups::ReInit));
        add(0x551950,NoArgStub<7>,bit_cast<uintptr_t>(&CPools::CheckPoolsEmpty));
        add(0x542800,MovingAddStub,bit_cast<uintptr_t>(&CPhysical::AddToMovingList));
        add(0x542860,MovingRemStub,bit_cast<uintptr_t>(&CPhysical::RemoveFromMovingList));
        add(0x532B00,RwFrameStub,bit_cast<uintptr_t>(&CEntity::UpdateRwFrame));
        add(0x59AD70,MatUpdStub,bit_cast<uintptr_t>(&CMatrix::UpdateRwMatrix));
        add(0x54EF40,PlaceUpdStub,bit_cast<uintptr_t>(&CSimpleTransform::UpdateRwMatrix));
        add(0x448AF0,DoorStub,bit_cast<uintptr_t>(&CGarages::IsModelIndexADoor));
        add(0x43F150,OutsideStub,bit_cast<uintptr_t>(&CEntryExitManager::GetPositionRelativeToOutsideWorld));
        add(0x5E0170,AliveStub,bit_cast<uintptr_t>(&CPed::IsAlive));
        add(0x681720,ActiveTaskStub,bit_cast<uintptr_t>(&CTaskManager::GetActiveTask));

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool isWorldFn = nm=="ClearScanCodes"||nm.starts_with("CastShadowSectorList")||nm=="ClearForRestart";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto sp=[&](float lo,float hi){ const uint32 m=next()%10; return m==0 ? 0.f : m==1 ? 1.f : m==2 ? -1.f : m==3 ? uni(-0.01f,0.01f) : uni(lo,hi); };
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3A3Au; if(!st)st=1; sst=st^0xA5A5A5A5u; if(!sst)sst=7;
                    trace=json::array();
                    // ---------- mundo y pools (identicos en las dos ejecuciones) ----------
                    RestoreAll();
                    std::memcpy(dblHdr.data(),dblImg.data(),dblImg.size()); dblStore.fill(0xFD); dblStates.fill(0x80);
                    std::memcpy(sglHdr.data(),sglImg.data(),sglImg.size()); sglStore.fill(0xFD); sglStates.fill(0x80);
                    entStates.fill(0);
                    poolPtrs[0]=sglHdr.data(); poolPtrs[1]=dblHdr.data(); poolPtrs[3]=wPedHdr.data(); poolPtrs[4]=wVehHdr.data(); poolPtrs[5]=wBldHdr.data(); poolPtrs[6]=wObjHdr.data(); poolPtrs[7]=wDumHdr.data();
                    std::memset(reinterpret_cast<void*>(SECTORS),0,SECTORS_SIZE); std::memset(reinterpret_cast<void*>(REPEAT),0,REPEAT_SIZE); std::memset(reinterpret_cast<void*>(LODL),0,LOD_SIZE);
                    // ---------- buffers ----------
                    for(auto& m:mats){ std::memset(m.data(),0,m.size()); }
                    entMem.fill(0xCD); pedMem.fill(0xCD); vehMem.fill(0xCD);
                    for(auto* b:{&frameBuf,&clumpBuf,&objInfoBuf})for(auto& x:*b)x=uint8(next());
                    outBuf.fill(0xCD);
                    clumpBuf[0]=2; clumpBuf[1]=0; At<void*>(clumpBuf.data(),4)=frameBuf.data();     // rpCLUMP con su marco
                    for(size_t k=0;k<12;++k)At<float>(frameBuf.data(),0x10+size_t(k/3)*0x10+size_t(k%3)*4)=uni(-2.f,2.f);
                    doorSeed=next();
                    const float QNAN=std::numeric_limits<float>::quiet_NaN();

                    auto FillMat=[&](void* m,const CVector& pos){
                        auto unit=[&]{ CVector v{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float l=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z); return l<0.1f ? CVector{1.f,0.f,0.f} : CVector{v.x/l,v.y/l,v.z/l}; };
                        auto cross=[&](const CVector& a,const CVector& b){ return CVector{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; };
                        CVector r{1.f,0.f,0.f}, fw{0.f,1.f,0.f}, u{0.f,0.f,1.f};
                        const uint32 kind=next()%5;
                        if(kind==0){ const float h=uni(-3.2f,3.2f), c=std::cos(h), s=std::sin(h); r=CVector{c,s,0.f}; fw=CVector{-s,c,0.f}; }
                        else if(kind!=1){ r=unit(); CVector u0=unit(); fw=cross(u0,r); const float l=std::sqrt(fw.x*fw.x+fw.y*fw.y+fw.z*fw.z); fw=l<0.05f ? CVector{0.f,1.f,0.f} : CVector{fw.x/l,fw.y/l,fw.z/l}; u=cross(r,fw); }
                        std::memset(m,0,0x48);
                        At<CVector>(m,0)=r; At<CVector>(m,0x10)=fw; At<CVector>(m,0x20)=u; At<CVector>(m,0x30)=pos;
                        for(size_t o:{size_t(0xC),size_t(0x1C),size_t(0x2C),size_t(0x3C)})At<uint32>(m,o)=next();
                    };
                    std::vector<CVector> posList;
                    CVector argPoint{}, argPlayer{};
                    // posicion adversaria: empates exactos con otra entidad o con los argumentos, lejanas y con NaN
                    auto RandPos=[&](bool adv)->CVector{
                        CVector p{uni(-300.f,300.f),uni(-300.f,300.f),uni(-20.f,120.f)};
                        if(adv){ const uint32 m=next()%14;
                            if(m<2&&!posList.empty())p=posList[next()%posList.size()];
                            else if(m==2)p=argPoint; else if(m==3)p=argPlayer;
                            else if(m==4)p=CVector{uni(-14000.f,14000.f),uni(-14000.f,14000.f),uni(-20.f,120.f)};
                            else if(m==5){ const uint32 c=next()%3; (c==0 ? p.x : c==1 ? p.y : p.z)=QNAN; } }
                        posList.push_back(p); return p;
                    };
                    auto InitPed=[&](size_t k,bool needMat,bool adv){
                        uint8* e=PED(k); std::memset(e,0xCD,PS);
                        const CVector pos=RandPos(adv);
                        const bool hasMat=needMat||next()%4!=0;
                        At<void*>(e,0x14)=nullptr; At<void*>(e,0x18)=nullptr;
                        At<uint8>(e,0x36)=uint8(ENTITY_TYPE_PED|((next()%32)<<3));
                        if(hasMat){ FillMat(mats[k].data(),pos); At<void*>(e,0x14)=mats[k].data(); }
                        else { At<CVector>(e,4)=pos; At<float>(e,0x10)=next()%5==0 ? 0.f : uni(-7.f,7.f); }
                        At<CVector>(e,0x44)=CVector{sp(-2.f,2.f),sp(-2.f,2.f),sp(-2.f,2.f)};
                        At<uint32>(e,0x46C)=(next()&~0x100u)|(next()%100<60 ? 0x100u : 0u);
                        At<void*>(e,0x58C)= next()%5==0 ? nullptr : static_cast<void*>(VEH(next()%4));
                        At<void*>(e,0x47C)=intelBuf[k].data();
                        At<uint8>(e,0x484)=uint8(next()%5!=0 ? 1u : next()%4);
                        { static const uint32 types[]{4,7,16,6,3,17,0,5,10,13,8,15,9,2,1}; At<uint32>(e,0x598)=next()%3 ? types[next()%std::size(types)] : next()%26; }
                    };
                    auto InitVeh=[&](size_t k,bool needMat,bool adv){
                        uint8* e=VEH(k); std::memset(e,0xCD,VS);
                        const CVector pos=RandPos(adv);
                        const bool hasMat=needMat||next()%4!=0;
                        At<void*>(e,0x14)=nullptr; At<void*>(e,0x18)=nullptr;
                        { const uint32 m=next()%3; const uint32 status= m==0 ? 2u : m==1 ? 3u : next()%32; At<uint8>(e,0x36)=uint8(ENTITY_TYPE_VEHICLE|(status<<3)); }
                        if(hasMat){ FillMat(mats[8+k].data(),pos); At<void*>(e,0x14)=mats[8+k].data(); }
                        else { At<CVector>(e,4)=pos; At<float>(e,0x10)=next()%5==0 ? 0.f : uni(-7.f,7.f); }
                        At<CVector>(e,0x44)=CVector{sp(-2.f,2.f),sp(-2.f,2.f),sp(-2.f,2.f)};
                        At<uint8>(e,0x4A4)=uint8(next()%5!=0 ? 1u : next()%4);
                        At<int32>(e,0x590)= next()%3==0 ? 6 : int32(next()%12);
                        At<int32>(e,0x594)= next()%5!=0 ? 0 : int32(next()%12);
                    };
                    // jugadores, camara y banderas globales para las funciones FindPlayer*
                    auto GenPlayers=[&](bool needPed,bool idsPositive)->int{
                        for(size_t k=0;k<NP;++k){ InitPed(k,false,false); InitVeh(k,false,false); }
                        AtA<uint8>(PIF)=uint8(next()%2); AtA<uint8>(CARS_CAM)=uint8(next()%5==0);
                        { uint8* cam=reinterpret_cast<uint8*>(CAMERA); std::memset(cam,0xCD,CAMERA_SIZE);
                          At<CVector>(cam,4)=CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-20.f,120.f)};
                          if(next()%5<3){ FillMat(mats[16].data(),CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-20.f,120.f)}); At<void*>(cam,0x14)=mats[16].data(); } else At<void*>(cam,0x14)=nullptr; }
                        for(size_t p=0;p<2;++p){ uint8* pl=reinterpret_cast<uint8*>(PLAYERS)+p*0x190;
                            At<void*>(pl,0)= next()%8==0 ? nullptr : static_cast<void*>(PED(next()%3));
                            At<uint32>(pl,4)=next();
                            At<void*>(pl,0xB0)= next()%100<35 ? static_cast<void*>(VEH(next()%4)) : nullptr; }
                        const int id= idsPositive ? int(next()%2) : (next()%3==0 ? -1 : int(next()%2));
                        const int res= id<0 ? int(AtA<uint8>(PIF)) : id;
                        if(needPed){ uint8* pl=reinterpret_cast<uint8*>(PLAYERS)+size_t(res)*0x190; if(!At<void*>(pl,0))At<void*>(pl,0)=PED(next()%3); }
                        return id;
                    };

                    // ---------- fixture por funcion ----------
                    uintptr_t target=fn.address;
                    json ret=nullptr; std::string f80; bool compareOut=false;
                    std::function<void()> call;
                    auto PtrRet=[&](const void* p){ ret=Name(p); };
                    // entidad del mundo falsa
                    auto InitWorldEnt=[&](int k,int type){
                        uint8* e=E(k); std::memset(e,0xCD,ES);
                        At<void*>(e,0)=fvt.data(); At<void*>(e,0x14)=nullptr; At<void*>(e,0x18)=nullptr;
                        At<uint32>(e,0x1C)=next(); At<uint16>(e,0x2C)=uint16(next()%4==0 ? 0 : next());
                        At<uint8>(e,0x36)=uint8(type|((next()%32)<<3));
                        At<CVector>(e,4)=CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-20.f,120.f)}; At<float>(e,0x10)=uni(-3.f,3.f);
                    };

                    if(nm=="Initialise"){
                        for(size_t o=0;o<0x14;++o)At<uint8>(reinterpret_cast<void*>(GLOBALS),o)=uint8(next());
                        call=[&]{ reinterpret_cast<void(__cdecl*)()>(target)(); };
                    }
                    else if(nm=="Add"||nm=="Remove"){
                        const int type= next()%3==0 ? (next()%2 ? 1 : 5) : int(next()%8);
                        InitWorldEnt(0,type);
                        { uint32& fl=At<uint32>(E(0),0x1C); if(next()%3==0)fl|=4u; else fl&=~4u; if(next()%3==0)fl|=0x40000u; else fl&=~0x40000u; }
                        if(next()%4!=0){ At<void*>(E(0),0x18)=clumpBuf.data(); }
                        if(next()%2){ FillMat(mats[17].data(),CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-20.f,120.f)}); At<void*>(E(0),0x14)=mats[17].data(); }
                        call=[&]{ reinterpret_cast<void(__cdecl*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(E(0))); };
                    }
                    else if(nm=="CameraToIgnoreThisObject"){
                        InitWorldEnt(0,ENTITY_TYPE_OBJECT);
                        At<int16>(E(0),0x22)=int16(next()%3==0 ? int(next()%65536)-32768 : int(next()%1500)-20);
                        At<void*>(E(0),0x160)=objInfoBuf.data(); At<uint8>(objInfoBuf.data(),0x1E)=uint8(next()%6);
                        { static const uint8 cv[]{0,1,9,10,11,12,255,200,5}; AtA<uint8>(CAMCOLVARS)=next()%2 ? cv[next()%std::size(cv)] : uint8(next()); }
                        call=[&]{ ret=int(reinterpret_cast<uint32(__cdecl*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(E(0)))&0xFF); };
                    }
                    else if(nm=="ClearScanCodes"||nm.starts_with("CastShadowSectorList")||nm=="ClearForRestart"){
                        for(int k=0;k<NE;++k)InitWorldEnt(k,int(next()%8));
                        AtA<uint16>(SCAN)= next()%3==0 ? uint16(next()%4) : uint16(next());
                        for(int k=0;k<NE;++k)if(next()%3==0)At<uint16>(E(k),0x2C)=AtA<uint16>(SCAN);
                        auto Ent=[&]{ return E(int(next()%NE)); };
                        if(nm=="ClearScanCodes"){
                            for(int n=0;n<40;++n){ const int x=int(next()%120), y=int(next()%120); auto& s=CWorld::GetSector(x,y);
                                for(uint32 c=next()%4;c>0;--c){ if(next()%2)s.Buildings.AddItem(reinterpret_cast<CBuilding*>(Ent())); else s.Dummies.AddItem(reinterpret_cast<CDummy*>(Ent())); } }
                            for(int n=0;n<24;++n){ auto& r=CWorld::ms_aRepeatSectors[next()%16][next()%16];
                                for(uint32 c=next()%4;c>0;--c){ switch(next()%3){ case 0: r.Vehicles.AddItem(reinterpret_cast<CVehicle*>(Ent())); break;
                                                                                   case 1: r.Peds.AddItem(reinterpret_cast<CPed*>(Ent())); break;
                                                                                   default: r.Objects.AddItem(reinterpret_cast<CObject*>(Ent())); } } }
                            call=[&]{ reinterpret_cast<void(__cdecl*)()>(target)(); };
                        }
                        else if(nm=="ClearForRestart"){
                            AtA<uint32>(CUTSCENE)= next()%2 ? 2u : next()%3;
                            for(int n=0;n<24;++n){ auto& r=CWorld::ms_aRepeatSectors[next()%16][next()%16];
                                for(uint32 c=next()%4;c>0;--c){ if(next()%2)r.Peds.AddItem(reinterpret_cast<CPed*>(Ent())); else r.Vehicles.AddItem(reinterpret_cast<CVehicle*>(Ent())); }
                                if(next()%3==0)r.Objects.AddItem(reinterpret_cast<CObject*>(Ent())); }
                            call=[&]{ reinterpret_cast<void(__cdecl*)()>(target)(); };
                        }
                        else { // CastShadowSectorList sobre una lista doble propia (la cabeza es un unico puntero)
                            std::memset(lstBuf.data(),0,lstBuf.size());
                            auto* lst=reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(lstBuf.data());
                            for(uint32 c=next()%7;c>0;--c)lst->AddItem(reinterpret_cast<CPhysical*>(Ent()));
                            call=[&]{ reinterpret_cast<void(__cdecl*)(void*,float,float,float,float)>(target)(lst,0.f,0.f,0.f,0.f); };
                        }
                    }
                    else if(nm=="GetCurrentScanCode"){
                        AtA<uint16>(SCAN)= next()%4==0 ? uint16(next()%3) : next()%4==0 ? uint16(0xFFFF-next()%3) : uint16(next());
                        call=[&]{ ret=int(reinterpret_cast<uint16(__cdecl*)()>(target)()); };
                    }
                    else if(nm=="GetSector"||nm=="GetRepeatSector"||nm=="GetLodPtrList"){
                        auto Coord=[&]()->int32{
                            if(nm=="GetLodPtrList")return int32(next()%30);
                            const uint32 m=next()%12;
                            if(m==0)return std::numeric_limits<int32>::min(); if(m==1)return std::numeric_limits<int32>::max();
                            if(m==2)return -int32(next()%100000); if(m==3)return int32(next()%100000);
                            if(m<8)return int32(next()%160)-20; return int32(next()%122)-1; };
                        const int32 x=Coord(), y=Coord();
                        call=[&,x,y]{ PtrRet(reinterpret_cast<void*(__cdecl*)(int32,int32)>(target)(x,y)); };
                    }
                    else if(nm=="FindPlayerPed"||nm=="FindPlayerWanted"||nm=="FindPlayerTrain"||nm=="FindPlayerVehicle"||nm=="FindPlayerCoors"){
                        const int id=GenPlayers(false,false);
                        if(nm=="FindPlayerCoors"){ compareOut=true;
                            call=[&,id]{ PtrRet(reinterpret_cast<void*(__cdecl*)(void*,int32)>(target)(outBuf.data(),id)); };
                        }
                        else if(nm=="FindPlayerVehicle"){ const int remote=int(next()%2);
                            call=[&,id,remote]{ PtrRet(reinterpret_cast<void*(__cdecl*)(int32,uint32)>(target)(id,uint32(remote))); };
                        }
                        else if(nm=="FindPlayerWanted"){
                            call=[&,id]{ const auto v=reinterpret_cast<uint32(__cdecl*)(int32)>(target)(id); ret=std::format("{:08x}",v); };
                        }
                        else call=[&,id]{ PtrRet(reinterpret_cast<void*(__cdecl*)(int32)>(target)(id)); };
                    }
                    else if(nm=="FindPlayerEntity"||nm=="FindPlayerSpeed"){
                        const int id=GenPlayers(true,false);
                        call=[&,id]{ PtrRet(reinterpret_cast<void*(__cdecl*)(int32)>(target)(id)); };
                    }
                    else if(nm=="FindPlayerCentreOfWorld"||nm=="FindPlayerCentreOfWorldForMap"){
                        const int id=GenPlayers(true,false);
                        if(nm=="FindPlayerCentreOfWorld")
                            call=[&,id]{ PtrRet(reinterpret_cast<void*(__cdecl*)(int32)>(target)(id)); };
                        else { compareOut=true;
                            call=[&,id]{ PtrRet(reinterpret_cast<void*(__cdecl*)(void*,int32)>(target)(outBuf.data(),id)); }; }
                    }
                    else if(nm=="FindPlayerCentreOfWorld_NoSniperShift"){
                        const int id=GenPlayers(false,true);
                        call=[&,id]{ PtrRet(reinterpret_cast<void*(__cdecl*)(int32)>(target)(id)); };
                    }
                    else if(nm=="FindPlayerHeading"){
                        const int id=GenPlayers(true,true);
                        call=[&,id]{ f80=CallCdeclIntF0(target,id); };
                    }
                    else if(nm=="FindPlayerSlotWithPedPointer"||nm=="FindPlayerSlotWithRemoteVehiclePointer"||nm=="FindPlayerSlotWithVehiclePointer"){
                        GenPlayers(false,false);
                        void* arg{};
                        const uint32 m=next()%6;
                        uint8* pl0=reinterpret_cast<uint8*>(PLAYERS); uint8* pl1=pl0+0x190;
                        if(nm=="FindPlayerSlotWithPedPointer") arg= m<2 ? At<void*>(pl0,0) : m<4 ? At<void*>(pl1,0) : m==4 ? nullptr : static_cast<void*>(PED(next()%8));
                        else if(nm=="FindPlayerSlotWithRemoteVehiclePointer") arg= m<2 ? At<void*>(pl0,0xB0) : m<4 ? At<void*>(pl1,0xB0) : m==4 ? nullptr : static_cast<void*>(VEH(next()%8));
                        else { void* p0=At<void*>(pl0,0); void* p1=At<void*>(pl1,0);
                               arg= m<2&&p0 ? At<void*>(p0,0x58C) : m<4&&p1 ? At<void*>(p1,0x58C) : m==4 ? nullptr : static_cast<void*>(VEH(next()%8)); }
                        call=[&,arg]{ ret=int(reinterpret_cast<int32(__cdecl*)(void*)>(target)(arg)); };
                    }
                    else if(nm=="FindUnsuspectingTargetCar"||nm=="FindUnsuspectingTargetPed"){
                        const bool cars= nm=="FindUnsuspectingTargetCar";
                        poolPtrs[3]=pedPoolHdr.data(); poolPtrs[4]=vehPoolHdr.data();
                        for(size_t k=0;k<NP;++k){ pedStates[k]= next()%4==0 ? 0x80 : uint8(next()&0x7F); vehStates[k]= next()%4==0 ? 0x80 : uint8(next()&0x7F); }
                        argPoint=CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-20.f,120.f)};
                        argPlayer=CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-20.f,120.f)};
                        { const uint32 m=next()%12; if(m==0)argPoint.x=QNAN; else if(m==1)argPlayer.y=QNAN; else if(m==2)argPlayer=argPoint; else if(m==3)argPoint.z=QNAN; }
                        for(size_t k=0;k<NP;++k){ InitPed(k,true,true); InitVeh(k,true,true); }
                        // orientaciones adversarias: nula, con NaN o copia de la anterior
                        auto TweakFwd=[&](void* mat,void* prevMat){ const uint32 m=next()%10; uint8* fw=static_cast<uint8*>(mat)+0x10;
                            if(m==0)At<CVector>(fw,0)=CVector{0.f,0.f,0.f}; else if(m==1)At<float>(fw,4)=QNAN; else if(m==2&&prevMat)At<CVector>(fw,0)=At<CVector>(static_cast<uint8*>(prevMat),0x10); };
                        for(size_t k=0;k<NP;++k){ TweakFwd(mats[k].data(),k ? mats[k-1].data() : nullptr); TweakFwd(mats[8+k].data(),k ? mats[8+k-1].data() : nullptr); }
                        for(size_t k=0;k<NP;++k){
                            std::memset(intelBuf[k].data(),0xCD,intelBuf[k].size());
                            aliveOf[k]=next()%5!=0;
                            auto& tb=taskBuf[k]; std::memset(tb.data(),0xCD,tb.size()); At<void*>(tb.data(),0)=taskVt.data();
                            const uint32 m=next()%6; At<int32>(tb.data(),8)= (m<3) ? 912 : int32(200+next()%1300);
                            if(At<int32>(tb.data(),8)==912&&m>=3)At<int32>(tb.data(),8)=913;
                            const uint32 t=next()%5; taskOf[k]= t<2 ? nullptr : static_cast<void*>(tb.data()); }
                        const CVector pt=argPoint, pl=argPlayer;
                        if(cars) call=[&,pt,pl]{ PtrRet(reinterpret_cast<void*(__cdecl*)(CVector,CVector)>(target)(pt,pl)); };
                        else call=[&,pt,pl]{ PtrRet(reinterpret_cast<void*(__cdecl*)(CVector,CVector)>(target)(pt,pl)); };
                    }

                    // ---------- ejecucion ----------
                    setState(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    call();
                    setState(false);
                    json st2=json::object();
                    st2["peds"]=Hash(pedMem.data(),pedMem.size()); st2["vehs"]=Hash(vehMem.data(),vehMem.size()); st2["ents"]=Hash(entMem.data(),entMem.size());
                    st2["players"]=Hash(reinterpret_cast<void*>(PLAYERS),PLAYERS_SIZE); st2["camera"]=Hash(reinterpret_cast<void*>(CAMERA),CAMERA_SIZE);
                    st2["globals"]=Hash(reinterpret_cast<void*>(GLOBALS),GLOBALS_SIZE); st2["out"]=Hash(outBuf.data(),outBuf.size());
                    st2["pools"]=Hash(reinterpret_cast<void*>(POOLS),POOLS_SIZE); st2["frame"]=Hash(frameBuf.data(),frameBuf.size());
                    if(isWorldFn){
                        st2["world"]={Hash(reinterpret_cast<void*>(SECTORS),SECTORS_SIZE),Hash(reinterpret_cast<void*>(REPEAT),REPEAT_SIZE),Hash(reinterpret_cast<void*>(LODL),LOD_SIZE),
                                      Hash(dblStore.data(),dblStore.size()),Hash(dblStates.data(),dblStates.size()),Hash(dblHdr.data(),dblHdr.size()),
                                      Hash(sglStore.data(),sglStore.size()),Hash(sglStates.data(),sglStates.size()),Hash(sglHdr.data(),sglHdr.size()),Hash(lstBuf.data(),lstBuf.size())}; }
                    json r={{"ret",ret},{"f80",f80},{"state",st2},{"out_words",compareOut ? json(Hash(outBuf.data(),12)) : json(nullptr)},{"trace",trace},{"switches",switches}};
                    RestoreAll();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"].is_null() ? original["f80"] : original["ret"]},{"candidate",candidate["ret"].is_null() ? candidate["f80"] : candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    const bool intactBefore=Restored();
    RestoreAll();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",!intactBefore},{"state_restored",Restored()}});
}
}
