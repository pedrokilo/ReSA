#pragma once
// Banco dentro del motor: almacen de colisiones (lista de 23; las 23 funciones de CColStore).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures deterministas sobre un
// pool falso de 255 ColDef (tamano del EXE: los bucles recorren 255 slots fijos) y un arbol cuaternario cuya raiz y
// nodos salen del pool real de nodos y cuyas listas usan nodos reales de CPtrNodeSingleLink (m_LastFreeSlot de
// ambos pools se restaura en cada ejecucion). Se sustituyen y restauran: los punteros globales del pool y del arbol,
// los pools de peatones y vehiculos (pools falsos para la limpieza de mision), una ventana de 40 punteros de
// ms_modelInfoPtrs (model infos y modelos de colision falsos), las entradas de streaming de los 255 COL, la limpieza
// de mision, el area actual y los globales de CColStore. Se simulan y trazan (sin reservas): FindPlayerPed,
// FindPlayerVehicle, el streaming, CColAccel, la carga de ficheros COL de CFileLoader, RemoveCollisionVolumes,
// GetScreenFadeStatus, MessageScreen, CTimer::Suspend/Resume y operator new/delete (solo en Initialise/Shutdown).
// Nunca se cargan ficheros COL reales ni se tocan model infos reales.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Collision/ColStore.h"
#include "Collision/ColAccel.h"
#include "Collision/ColModel.h"
#include "Core/QuadTreeNode.h"
#include "FileLoader.h"
#include "Streaming.h"
#include "Timer.h"
#include "Camera.h"
#include "MenuManager.h"
#include "TheScripts.h"
#include "CutsceneMgr.h"
#include "PlayerPed.h"
#include "../ProbeRuntime.hpp"

namespace ColStore23Live {
using ColTreeNode = CQuadTreeNode<ColDef*>;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr int32 NSLOT=255; constexpr size_t DEFSZ=0x2C;
constexpr uintptr_t COLPOOL=0x965560, QUADTREE=0x96555C, ENT_AREA=0x965554, COL_NEEDED=0x965558, COL_VEC=0x965580;
constexpr uintptr_t AREA=0xB72914, DISABLE_STREAMING=0x9654B0, PED_POOL=0xB74490, VEH_POOL=0xB74494, MI_PTRS=0xA9B0C8;
constexpr uintptr_t STREAM_COL=0x8E4CC0+25000*0x14; constexpr size_t STREAM_COL_SIZE=NSLOT*0x14;
constexpr int32 MI_BASE=19900, NMI=40;
constexpr int32 NPED=6, NVEH=6; constexpr size_t PEDSZ=0x7C4, VEHSZ=0xA18;

// --- fixtures ---
alignas(16) inline std::array<uint8,NSLOT*DEFSZ> storage{};
inline std::array<uint8,NSLOT> states{};
alignas(16) inline std::array<uint8,0x14> poolHdr{};
alignas(16) inline std::array<std::array<uint8,0x20>,NMI> mis{};
alignas(16) inline std::array<std::array<uint8,0x30>,NMI> cms{};
alignas(16) inline std::array<uint8,NPED*PEDSZ> pedStore{}; inline std::array<uint8,NPED> pedMap{}; alignas(16) inline std::array<uint8,0x14> pedHdr{};
alignas(16) inline std::array<uint8,NVEH*VEHSZ> vehStore{}; inline std::array<uint8,NVEH> vehMap{}; alignas(16) inline std::array<uint8,0x14> vehHdr{};
alignas(16) inline std::array<std::array<float,16>,NPED+NVEH> mats{};
alignas(16) inline std::array<uint8,0x40> ped{}, veh{};
alignas(16) inline std::array<uint8,0x4000> arena{}; inline size_t arenaTop{};
alignas(16) inline std::array<uint8,0x100> colData{};
inline char colFile[32]{};
inline ColTreeNode* root{};
inline std::vector<int32> used;
inline uint8* Slot(int32 i) { return storage.data()+size_t(i)*DEFSZ; }
inline ColDef* Def(int32 i) { return reinterpret_cast<ColDef*>(Slot(i)); }

inline uint32 Name(const void* p) {
    // direcciones de los fixtures como desplazamiento etiquetado (estable entre ejecuciones)
    const auto v=uintptr_t(p); if(!v) return 0;
    auto in=[&](const void* b,size_t n,uint32 tag)->std::optional<uint32>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return (tag<<24)|uint32(v-uintptr_t(b)); return std::nullopt; };
    if(auto r=in(storage.data(),storage.size(),1))return *r; if(auto r=in(states.data(),states.size(),2))return *r; if(auto r=in(poolHdr.data(),poolHdr.size(),3))return *r;
    if(auto r=in(arena.data(),arena.size(),4))return *r; if(auto r=in(mis.data(),sizeof(mis),5))return *r; if(auto r=in(cms.data(),sizeof(cms),6))return *r;
    if(auto r=in(colData.data(),colData.size(),7))return *r; if(auto r=in(colFile,sizeof(colFile),8))return *r; if(auto r=in(ped.data(),ped.size(),9))return *r;
    if(auto r=in(veh.data(),veh.size(),10))return *r; if(auto r=in(pedStore.data(),pedStore.size(),11))return *r; if(auto r=in(vehStore.data(),vehStore.size(),12))return *r;
    if(root&&v==uintptr_t(root))return 13u<<24;
    return 0xEEEEEEEEu; // puntero ajeno a los fixtures
}

// --- traza sin reservas ---
struct Ev { uint32 kind, a; }; inline std::array<Ev,16384> evs{}; inline int nev{};
inline void Push(uint32 k,uint32 a) { if(nev<int(evs.size()))evs[nev++]={k,a}; }
inline void Tag(uint32 t) { Push(0,t); }
inline void PushP(const void* p) { Push(1,Name(p)); }
inline void PushV(uint32 v) { Push(2,v); }
inline void PushF(float f) { Push(3,F(f)); }
inline void PushBytes(const void* p,size_t n) { for(size_t k=0;k+4<=n;k+=4)Push(4,*reinterpret_cast<const uint32*>(static_cast<const uint8*>(p)+k)); }
inline uint32 masks[4]{}, calls[4]{};
inline bool Bit(int m) { return (masks[m]>>(calls[m]++%32))&1; }

// --- stubs ---
inline void* pedResult{}; inline void* vehResult{}; inline int32 fadeStatus{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { Tag(100); PushV(uint32(id)); return static_cast<CPlayerPed*>(pedResult); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote) { Tag(101); PushV(uint32(id)); PushV(remote); return static_cast<CVehicle*>(vehResult); }
inline void __cdecl FakeRequestModel(int32 id,int32 f) { Tag(102); PushV(uint32(id)); PushV(uint32(f)); }
inline void __cdecl FakeRemoveModel(int32 id) { Tag(103); PushV(uint32(id)); }
inline void __cdecl FakeLoadAllRequested(bool b) { Tag(104); PushV(b); }
inline bool __cdecl FakeCacheLoading() { Tag(105); return Bit(0); }
inline void __cdecl FakeCacheLoadCol() { Tag(106); }
inline void __cdecl FakeAddColDef(ColDef d) { Tag(107); PushBytes(&d,sizeof(d)); }
inline void __cdecl FakeGetColDef(ColDef& d) { Tag(108); PushP(&d); const uint32 k=uint32(reinterpret_cast<uint8*>(&d)-storage.data())/DEFSZ;
    d.m_Area=CRect(-500.f+float(k),-400.f-float(k),300.f+float(k*3),200.f+float(k)); }
inline void __cdecl FakeLoadColFileName(const char* name,uint8 slot) { Tag(109); PushP(name); PushV(slot); }
inline bool __cdecl FakeLoadColBuffer(uint8* data,uint32 n,uint8 slot) { Tag(110); PushP(data); PushV(n); PushV(slot); return Bit(1); }
inline bool __cdecl FakeLoadColFirstTime(uint8* data,uint32 n,uint8 slot) { Tag(111); PushP(data); PushV(n); PushV(slot); return Bit(1); }
inline void __fastcall FakeRemoveVolumes(void* cm,void*) { Tag(112); PushP(cm); }
inline int32 __fastcall FakeFadeStatus(void* cam,void*) { Tag(113); PushV(uint32(uintptr_t(cam)-0xB6F028)); return fadeStatus; }
inline void __fastcall FakeMessageScreen(void* mm,void*,const char* key,bool a,bool b) { Tag(114); PushV(uint32(uintptr_t(mm)-0xBA6748)); PushV(key ? uint32(Fnv(key,std::strlen(key))) : 0u); PushV(a); PushV(b); }
inline void __cdecl FakeSuspend() { Tag(115); }
inline void __cdecl FakeResume() { Tag(116); }
inline void* __cdecl FakeNew(size_t n) { Tag(117); PushV(uint32(n)); arenaTop=(arenaTop+15)&~size_t(15); void* r=arena.data()+arenaTop; arenaTop+=n; if(arenaTop>arena.size())arenaTop=0; return r; }
inline void* __cdecl FakeNewArr(size_t n) { Tag(118); PushV(uint32(n)); arenaTop=(arenaTop+15)&~size_t(15); void* r=arena.data()+arenaTop; arenaTop+=n; if(arenaTop>arena.size())arenaTop=0; return r; }
inline void __cdecl FakeDelete(void* p) { Tag(119); PushP(p); }
inline void __cdecl FakeDeleteArr(void* p) { Tag(120); PushP(p); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,23> fns{{
    {"CColStore","AddColSlot",0x411140,0,-1,512},{"CColStore","AddCollisionNeededAtPosn",0x4103A0,0,-1,128},
    {"CColStore","AddRef",0x4107A0,0,-1,256},{"CColStore","BoundingBoxesPostProcess",0x410EC0,0,-1,256},
    {"CColStore","EnsureCollisionIsInMemory",0x410AD0,0,-1,512},{"CColStore","GetBoundingBox",0x410800,0,-1,256},
    {"CColStore","HasCollisionLoaded",0x410CE0,0,-1,768},{"CColStore","IncludeModelIndex",0x410820,0,-1,512},
    {"CColStore","Initialise",0x4113F0,0,-1,128},{"CColStore","LoadAllBoundingBoxes",0x4113D0,0,-1,128},
    {"CColStore","LoadAllCollision",0x410E60,0,-1,128},{"CColStore","LoadCol-",0x410690,0,-1,256},
    {"CColStore","LoadCol-2",0x4106D0,0,-1,512},{"CColStore","LoadCollision",0x410860,0,-1,1024},
    {"CColStore","RemoveAllCollision",0x410E00,0,-1,256},{"CColStore","RemoveCol",0x410730,0,-1,512},
    {"CColStore","RemoveColSlot",0x411330,0,-1,512},{"CColStore","RemoveRef",0x4107D0,0,-1,256},
    {"CColStore","RequestCollision",0x410C00,0,-1,768},{"CColStore","SetCollisionRequired",0x4104E0,0,-1,768},
    {"CColStore","SetIfCollisionIsRequired",0x4103D0,0,-1,1024},{"CColStore","SetIfCollisionIsRequiredReducedBB",0x410470,0,-1,1024},
    {"CColStore","Shutdown",0x4114D0,0,-1,256},
}};

// Pool falso: campos finitos, ~1/3 de slots ocupados (el 0 siempre: "generic"), rangos de modelos dentro de la ventana
template<class N,class U> inline void BuildPool(N& next,U& uni) {
    used.clear();
    for(int32 i=0;i<NSLOT;++i){
        uint8* e=Slot(i); for(size_t k=0;k<DEFSZ;k+=4){ const uint32 v=next(); std::memcpy(e+k,&v,4); }
        auto* d=Def(i);
        if(next()%10==0) d->m_Area=CRect();
        else { const float cx=uni(-3200.f,3200.f), cy=uni(-3200.f,3200.f), w=uni(0.f,600.f), h=uni(0.f,600.f); d->m_Area=CRect(cx-w,cy-h,cx+w,cy+h); }
        switch(next()%6){
        case 0: d->m_nModelIdStart=SHRT_MAX; d->m_nModelIdEnd=SHRT_MIN; break;
        case 1: d->m_nModelIdStart=int16(MI_BASE+20+next()%10); d->m_nModelIdEnd=int16(MI_BASE+next()%20); break;
        default: { const int32 s=MI_BASE+int32(next()%NMI); d->m_nModelIdStart=int16(s); d->m_nModelIdEnd=int16((std::min)(s+int32(next()%12),MI_BASE+NMI-1)); }
        }
        static const uint16 refs[]{0,0,0,1,2,5,0x7FFF,0x8000,0xFFFF,0xFFFE};
        d->m_nRefCount=refs[next()%std::size(refs)];
        d->m_bActive=next()%2; d->m_bCollisionIsRequired=next()%3==0; d->m_bProcedural=next()%5==0; d->m_bInterior=next()%4==0;
        const bool isUsed=i==0||next()%3==0;
        states[i]=uint8(isUsed ? (next()&0x7F) : (0x80|(next()&0x7F)));
        if(isUsed) used.push_back(i);
    }
    poolHdr.fill(0);
    At<void*>(poolHdr.data(),0)=storage.data(); At<void*>(poolHdr.data(),4)=states.data(); At<int32>(poolHdr.data(),8)=NSLOT;
    At<int32>(poolHdr.data(),0xC)=int32(next()%(NSLOT+1))-1; poolHdr[0x10]=uint8(next()%2); poolHdr[0x11]=uint8(next()%2);
}
template<class N> inline int32 PickUsed(N& next) { return used.empty() ? 0 : used[next()%used.size()]; }
template<class N> inline bool BuildTree(N& next) {
    root=new ColTreeNode(CRect(-3000.f,-3000.f,3000.f,3000.f),int32(next()%3));
    if(!root) return false;
    for(int32 i:used) if(next()%6){ auto* d=Def(i); const CRect r=d->m_Area; root->AddItem(d,r); }
    return true;
}
inline json DumpTree(ColTreeNode* n) {
    if(!n) return nullptr;
    json items=json::array(); for(auto* it:n->m_ItemList) items.push_back(Name(it));
    json ch=json::array(); for(auto* c:n->m_apChildren) ch.push_back(DumpTree(c));
    return {{"rect",{F(n->m_Rect.left),F(n->m_Rect.top),F(n->m_Rect.right),F(n->m_Rect.bottom)}},{"level",n->m_nLevel},{"items",items},{"children",ch}};
}
inline json PoolDiff(const std::vector<uint8>& s0,const std::vector<uint8>& st0) {
    json d=json::array();
    for(int32 i=0;i<NSLOT;++i) if(states[i]!=st0[i]||std::memcmp(Slot(i),&s0[size_t(i)*DEFSZ],DEFSZ)!=0) d.push_back({i,states[i],ReversingProbe::Bytes(Slot(i),DEFSZ)});
    return d;
}
inline json Header(const void* h) {
    return json::array({Name(At<void*>(const_cast<void*>(h),0)),Name(At<void*>(const_cast<void*>(h),4)),At<int32>(const_cast<void*>(h),8),At<int32>(const_cast<void*>(h),0xC),
                        static_cast<const uint8*>(h)[0x10],static_cast<const uint8*>(h)[0x11]});
}
// Entidad falsa de la limpieza de mision (peaton o vehiculo)
template<class N,class U> inline void FakeEntity(N& next,U& uni,uint8* e,size_t sz,size_t mi,bool isPed) {
    std::memset(e,0,sz);
    static const uint8 areas[]{0,0,1,2,3,200};
    e[0x2F]=areas[next()%std::size(areas)];
    const uint8 status=uint8(next()%10==0 ? 5 : next()%8);
    e[0x36]=uint8((isPed ? 3 : 2)|(status<<3));
    At<uint32>(e,0x40)=(next()%4==0 ? 0x2000u : 0u)|(next()%4==0 ? 0x4000u : 0u)|(next()&0x9FFFu);
    CVector p{uni(-3300.f,3300.f),uni(-3300.f,3300.f),uni(0.f,50.f)};
    if(!used.empty()&&next()%2){ auto* d=Def(PickUsed(next)); p.x=uni(d->m_Area.left-150.f,d->m_Area.right+150.f); p.y=uni(d->m_Area.bottom-150.f,d->m_Area.top+150.f); }
    if(next()%2){ auto& m=mats[mi]; m.fill(0.f); m[12]=p.x; m[13]=p.y; m[14]=p.z; At<void*>(e,0x14)=m.data(); }
    else { At<void*>(e,0x14)=nullptr; At<float>(e,4)=p.x; At<float>(e,8)=p.y; At<float>(e,0xC)=p.z; }
    if(isPed){ static const uint32 st[]{0x36,0x37,1,0,0x29}; At<uint32>(e,0x530)=st[next()%std::size(st)]; }
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"colstore23-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"colstore23-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("colstore23");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"colstore23-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,23> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,23> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // pools reales de nodos (arbol y listas): solo se restaura m_LastFreeSlot
    auto* quadPool=reinterpret_cast<uint8*>(*reinterpret_cast<void**>(0xB745BC)); auto* ptrPool=reinterpret_cast<uint8*>(GetPtrNodeSingleLinkPool());
    if(!quadPool||!ptrPool){Write({{"event","error"},{"reason","node pools missing"}});return;}
    auto& quadLast=At<int32>(quadPool,0xC); auto& ptrLast=At<int32>(ptrPool,0xC); const int32 savedQuadLast=quadLast, savedPtrLast=ptrLast;
    auto usedOf=[](uint8* p){ int32 n=0; const int32 sz=At<int32>(p,8); auto* m=At<uint8*>(p,4); for(int32 k=0;k<sz;++k)n+=!(m[k]&0x80); return n; };
    const int32 quadUsed0=usedOf(quadPool), ptrUsed0=usedOf(ptrPool);
    if(At<int32>(quadPool,8)-quadUsed0<32){Write({{"event","error"},{"reason","quadtree node pool almost full"},{"used",quadUsed0}});return;}

    // estado real guardado
    struct Blob { uintptr_t p; size_t n; std::vector<uint8> saved; };
    std::vector<Blob> blobs{{COLPOOL,4},{QUADTREE,4},{ENT_AREA,4},{COL_NEEDED,1},{COL_VEC,12},{AREA,4},{DISABLE_STREAMING,1},{PED_POOL,8},
        {MI_PTRS+MI_BASE*4,NMI*4},{STREAM_COL,STREAM_COL_SIZE},{uintptr_t(&CTheScripts::MissionCleanUp),sizeof(CTheScripts::MissionCleanUp)}};
    for(auto& b:blobs)b.saved.assign(reinterpret_cast<uint8*>(b.p),reinterpret_cast<uint8*>(b.p)+b.n);
    auto restoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.p),b.saved.data(),b.n); quadLast=savedQuadLast; ptrLast=savedPtrLast; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), p2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch s1(bit_cast<uintptr_t>(&CStreaming::RequestModel),FakeRequestModel), s1e(0x4087E0,FakeRequestModel);
        EntryPatch s2(bit_cast<uintptr_t>(&CStreaming::RemoveModel),FakeRemoveModel), s2e(0x4089A0,FakeRemoveModel);
        EntryPatch s3(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),FakeLoadAllRequested), s3e(0x40EA10,FakeLoadAllRequested);
        EntryPatch a1(bit_cast<uintptr_t>(&CColAccel::isCacheLoading),FakeCacheLoading), a1e(0x5B2AC0,FakeCacheLoading);
        EntryPatch a2(bit_cast<uintptr_t>(&CColAccel::cacheLoadCol),FakeCacheLoadCol), a2e(0x5B2CC0,FakeCacheLoadCol);
        EntryPatch a3(bit_cast<uintptr_t>(&CColAccel::addColDef),FakeAddColDef), a3e(0x5B2DD0,FakeAddColDef);
        EntryPatch a4(bit_cast<uintptr_t>(&CColAccel::getColDef),FakeGetColDef), a4e(0x5B2E60,FakeGetColDef);
        EntryPatch l1(bit_cast<uintptr_t>(static_cast<void(*)(const char*,uint8)>(&CFileLoader::LoadCollisionFile)),FakeLoadColFileName), l1e(0x5B4E60,FakeLoadColFileName);
        EntryPatch l2(bit_cast<uintptr_t>(static_cast<bool(*)(uint8*,uint32,uint8)>(&CFileLoader::LoadCollisionFile)),FakeLoadColBuffer), l2e(0x538440,FakeLoadColBuffer);
        EntryPatch l3(bit_cast<uintptr_t>(&CFileLoader::LoadCollisionFileFirstTime),FakeLoadColFirstTime), l3e(0x5B5000,FakeLoadColFirstTime);
        EntryPatch c1(bit_cast<uintptr_t>(&CColModel::RemoveCollisionVolumes),FakeRemoveVolumes), c1e(0x40F9E0,FakeRemoveVolumes);
        EntryPatch c2(bit_cast<uintptr_t>(&CCamera::GetScreenFadeStatus),FakeFadeStatus), c2e(0x50AE20,FakeFadeStatus);
        EntryPatch c3(bit_cast<uintptr_t>(&CMenuManager::MessageScreen),FakeMessageScreen), c3e(0x579330,FakeMessageScreen);
        EntryPatch t1(bit_cast<uintptr_t>(&CTimer::Suspend),FakeSuspend), t1e(0x5619D0,FakeSuspend);
        EntryPatch t2(bit_cast<uintptr_t>(&CTimer::Resume),FakeResume), t2e(0x561A00,FakeResume);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC0123u; if(!stt)stt=1; nev=0; arenaTop=0;
                    for(int k=0;k<4;++k){ masks[k]=next(); calls[k]=0; }
                    restoreAll();
                    BuildPool(next,uni);
                    // jugador, vehiculo, area, globales
                    static const uint8 areas[]{0,0,0,1,2,3,200};
                    ped.fill(0); veh.fill(0); ped[0x2F]=areas[next()%std::size(areas)];
                    At<CVector>(veh.data(),0x44)=CVector{uni(-1.5f,1.5f),uni(-1.5f,1.5f),uni(-1.f,1.f)};
                    pedResult=next()%5 ? static_cast<void*>(ped.data()) : nullptr; vehResult=next()%3 ? static_cast<void*>(veh.data()) : nullptr;
                    fadeStatus=int32(next()%3==0 ? 0 : 1+next()%2);
                    At<int32>(reinterpret_cast<void*>(AREA),0)=next()%3==0 ? int32(ped[0x2F]) : int32(areas[next()%std::size(areas)]);
                    At<uint8>(reinterpret_cast<void*>(DISABLE_STREAMING),0)=uint8(next()%10==0);
                    At<int32>(reinterpret_cast<void*>(ENT_AREA),0)=int32(next()%4); At<uint8>(reinterpret_cast<void*>(COL_NEEDED),0)=uint8(next()%2);
                    At<CVector>(reinterpret_cast<void*>(COL_VEC),0)=CVector{uni(-3300.f,3300.f),uni(-3300.f,3300.f),uni(0.f,50.f)};
                    // streaming de los COL: banderas y estado de carga aleatorios
                    for(int32 k=0;k<NSLOT;++k){ auto* si=reinterpret_cast<uint8*>(STREAM_COL)+k*0x14; static const uint8 fl[]{0,0,2,4,6,8,0x10,0x12};
                        si[6]=fl[next()%std::size(fl)]; si[0x10]=uint8(next()%4); }
                    // model infos falsos en la ventana
                    const int32 target=next()%8 ? PickUsed(next) : 0;
                    for(int32 k=0;k<NMI;++k){ auto& m=mis[k]; m.fill(0); auto& c=cms[k]; c.fill(0);
                        m[0x12]=uint8(next()); At<void*>(m.data(),0x14)=next()%4==0 ? nullptr : c.data();
                        c[0x28]=next()%2 ? uint8(target) : next()%2 ? uint8(PickUsed(next)) : uint8(next());
                        reinterpret_cast<void**>(MI_PTRS)[MI_BASE+k]=next()%5==0 ? nullptr : m.data(); }
                    // limpieza de mision sobre pools falsos de peatones y vehiculos
                    for(int32 k=0;k<NPED;++k){ FakeEntity(next,uni,pedStore.data()+k*PEDSZ,PEDSZ,size_t(k),true); pedMap[k]=uint8(next()%5==0 ? 0x80|(next()&0x7F) : next()&0x7F); }
                    for(int32 k=0;k<NVEH;++k){ FakeEntity(next,uni,vehStore.data()+k*VEHSZ,VEHSZ,size_t(NPED+k),false); vehMap[k]=uint8(next()%5==0 ? 0x80|(next()&0x7F) : next()&0x7F); }
                    auto setPool=[&](std::array<uint8,0x14>& h,uint8* st0,uint8* mp,int32 n){ h.fill(0); At<uint8*>(h.data(),0)=st0; At<uint8*>(h.data(),4)=mp; At<int32>(h.data(),8)=n; At<int32>(h.data(),0xC)=-1; };
                    setPool(pedHdr,pedStore.data(),pedMap.data(),NPED); setPool(vehHdr,vehStore.data(),vehMap.data(),NVEH);
                    At<void*>(reinterpret_cast<void*>(PED_POOL),0)=pedHdr.data(); At<void*>(reinterpret_cast<void*>(VEH_POOL),0)=vehHdr.data();
                    for(auto& m:CTheScripts::MissionCleanUp.m_Objects){ const uint32 r=next()%8;
                        m.type=int8(r<2 ? 2 : r<4 ? 1 : r==4 ? 3 : 0);
                        if(m.type==1||m.type==2){ const int32 n=m.type==2 ? NPED : NVEH; const auto* mp=m.type==2 ? pedMap.data() : vehMap.data(); const int32 k=int32(next()%n);
                            m.handle=(k<<8)|(next()%6==0 ? int32(next()&0xFF) : int32(mp[k])); }
                        else m.handle=int32(next()); }
                    // datos de entrada de LoadCol
                    for(auto& b:colData)b=uint8(next()); std::snprintf(colFile,sizeof(colFile),"MODELS\\COLL\\F%u.COL",unsigned(next()%100));
                    auto pickPos=[&]{ CVector p{uni(-3300.f,3300.f),uni(-3300.f,3300.f),uni(0.f,60.f)};
                        if(!used.empty()&&next()%3){ auto* d=Def(PickUsed(next)); p.x=uni(d->m_Area.left-150.f,d->m_Area.right+150.f); p.y=uni(d->m_Area.bottom-150.f,d->m_Area.top+150.f); }
                        return p; };
                    static const int32 areaArgs[]{-1,-1,-1,0,0,1,2,3,200};
                    const bool needTree=nm!="Initialise";
                    if(needTree&&!BuildTree(next)) return {{"error","no tree node"}};
                    At<void*>(reinterpret_cast<void*>(COLPOOL),0)=poolHdr.data(); At<void*>(reinterpret_cast<void*>(QUADTREE),0)=needTree ? static_cast<void*>(root) : nullptr;
                    const std::vector<uint8> s0(storage.begin(),storage.end()), st0(states.begin(),states.end());

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr; bool treeOut=true;

                    if(nm=="AddColSlot"){
                        static const char* const names[]{"generic","procobj","PROCOBJ","proc_int","Proc_Int2","proc_int3","procob","int_la","INT_LA_x","int_l","int_sf1","int_veg","int_vegas",
                            "int_cont","int_con","gen_int1","GEN_INT2a","gen_int3","gen_int4","gen_int5","gen_int6","gen_intb","gen_intc","savehous","SAVEHOUSE","levelmap","levelma",
                            "stadint","stadin","props","PROPS","props2","props3","prop","countn2","vegasN","",""};
                        if(next()%4==0){ const int32 k=int32(next()%NSLOT); states[k]|=0x80; } // asegura un hueco libre
                        bool anyFree=false; for(auto s:states)anyFree|=(s&0x80)!=0; if(!anyFree)states[NSLOT-1]|=0x80;
                        ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(names[next()%std::size(names)]);
                    }
                    else if(nm=="AddCollisionNeededAtPosn"){ const CVector p=pickPos(); reinterpret_cast<void(__cdecl*)(const CVector&)>(a)(p); treeOut=false; }
                    else if(nm=="AddRef"){ reinterpret_cast<void(__cdecl*)(int32)>(a)(PickUsed(next)); treeOut=false; }
                    else if(nm=="RemoveRef"){ reinterpret_cast<void(__cdecl*)(int32)>(a)(PickUsed(next)); treeOut=false; }
                    else if(nm=="BoundingBoxesPostProcess"){ reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="EnsureCollisionIsInMemory"){ const CVector p=pickPos(); reinterpret_cast<void(__cdecl*)(const CVector&)>(a)(p); }
                    else if(nm=="GetBoundingBox"){ const int32 k=next()%8 ? PickUsed(next) : int32(next()%NSLOT); ret=Name(reinterpret_cast<CRect*(__cdecl*)(int32)>(a)(k)); treeOut=false; }
                    else if(nm=="HasCollisionLoaded"){ const CVector p=pickPos(); ret=reinterpret_cast<bool(__cdecl*)(const CVector&,int32)>(a)(p,areaArgs[next()%std::size(areaArgs)]); }
                    else if(nm=="IncludeModelIndex"){
                        const int32 k=PickUsed(next); if(next()%3==0){ Def(k)->m_nModelIdStart=int16(next()); Def(k)->m_nModelIdEnd=int16(next()); }
                        const uint32 r=next()%5; const int32 id=r==0 ? int32(next()) : r==1 ? int32(next()%70000)-35000 : int32(next()%20000);
                        reinterpret_cast<void(__cdecl*)(int32,int32)>(a)(k,id); treeOut=false;
                    }
                    else if(nm=="Initialise"){
                        const bool keep=next()%4==0; At<void*>(reinterpret_cast<void*>(COLPOOL),0)=keep ? static_cast<void*>(poolHdr.data()) : nullptr;
                        arena.fill(0x5A); treeOut=false;
                        { EntryPatch n1(0x82119A,FakeNew), n2(0x821195,FakeNewArr), d1(0x8214BD,FakeDelete), d2(0x8213AE,FakeDeleteArr); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        auto* np=At<uint8*>(reinterpret_cast<void*>(COLPOOL),0);
                        if(np&&np!=poolHdr.data()){ out["new_pool"]=Header(np); auto* objs=At<uint8*>(np,0); auto* mp=At<uint8*>(np,4);
                            out["new_map"]=Fnv(mp,NSLOT); out["slot0"]=ReversingProbe::Bytes(objs,DEFSZ); out["new_storage"]=Fnv(objs,NSLOT*DEFSZ); }
                        auto* nt=At<ColTreeNode*>(reinterpret_cast<void*>(QUADTREE),0); out["new_tree"]=DumpTree(nt);
                    }
                    else if(nm=="LoadAllBoundingBoxes"){ reinterpret_cast<void(__cdecl*)()>(a)(); treeOut=false; }
                    else if(nm=="LoadAllCollision"){ reinterpret_cast<void(__cdecl*)()>(a)(); treeOut=false; }
                    else if(nm=="LoadCol-"){ reinterpret_cast<void(__cdecl*)(int32,const char*)>(a)(PickUsed(next),colFile); treeOut=false; }
                    else if(nm=="LoadCol-2"){
                        const int32 k=PickUsed(next); if(next()%3==0){ Def(k)->m_nModelIdStart=int16(next()); Def(k)->m_nModelIdEnd=int16(next()); }
                        ret=reinterpret_cast<bool(__cdecl*)(int32,uint8*,int32)>(a)(k,colData.data(),int32(next()%0x100)); treeOut=false;
                    }
                    else if(nm=="LoadCollision"){ const CVector p=pickPos(); reinterpret_cast<void(__cdecl*)(CVector,bool)>(a)(p,next()%3==0); }
                    else if(nm=="RemoveAllCollision"){ reinterpret_cast<void(__cdecl*)()>(a)(); treeOut=false; }
                    else if(nm=="RemoveCol"){ reinterpret_cast<void(__cdecl*)(int32)>(a)(target); treeOut=false; }
                    else if(nm=="RemoveColSlot"){ reinterpret_cast<void(__cdecl*)(int32)>(a)(target); out["header"]=Header(poolHdr.data()); }
                    else if(nm=="RequestCollision"){ const CVector p=pickPos(); reinterpret_cast<void(__cdecl*)(const CVector&,int32)>(a)(p,areaArgs[next()%std::size(areaArgs)]); }
                    else if(nm=="SetCollisionRequired"){ const CVector p=pickPos(); reinterpret_cast<void(__cdecl*)(const CVector&,int32)>(a)(p,areaArgs[next()%std::size(areaArgs)]); }
                    else if(nm=="SetIfCollisionIsRequired"||nm=="SetIfCollisionIsRequiredReducedBB"){
                        const int32 k=next()%4 ? PickUsed(next) : int32(next()%NSLOT); auto* d=Def(k);
                        CVector2D p{uni(-3300.f,3300.f),uni(-3300.f,3300.f)};
                        const uint32 r=next()%8;
                        if(r<5){ p.x=uni(d->m_Area.left-100.f,d->m_Area.right+100.f); p.y=uni(d->m_Area.bottom-100.f,d->m_Area.top+100.f); }
                        if(r==5){ const float m=nm=="SetIfCollisionIsRequired" ? 0.f : 80.f; p.x=next()%2 ? d->m_Area.left+m : d->m_Area.right-m; p.y=next()%2 ? d->m_Area.bottom+m : d->m_Area.top-m; } // bordes exactos
                        if(next()%24==0) p.x=std::numeric_limits<float>::quiet_NaN();
                        if(next()%24==0) p.y=std::numeric_limits<float>::quiet_NaN();
                        reinterpret_cast<void(__cdecl*)(const CVector2D&,ColDef*)>(a)(p,d); treeOut=false;
                    }
                    else if(nm=="Shutdown"){
                        { EntryPatch d1(0x8214BD,FakeDelete), d2(0x8213AE,FakeDeleteArr); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["header"]=Header(poolHdr.data()); out["quad_used"]=usedOf(quadPool)-quadUsed0; out["ptr_used"]=usedOf(ptrPool)-ptrUsed0; treeOut=false;
                    }

                    out["pool"]=PoolDiff(s0,st0);
                    out["globals"]={Name(At<void*>(reinterpret_cast<void*>(COLPOOL),0)),Name(At<void*>(reinterpret_cast<void*>(QUADTREE),0)),At<int32>(reinterpret_cast<void*>(ENT_AREA),0),
                                    At<uint8>(reinterpret_cast<void*>(COL_NEEDED),0),Fnv(reinterpret_cast<void*>(COL_VEC),12),Fnv(reinterpret_cast<void*>(STREAM_COL),STREAM_COL_SIZE),
                                    Fnv(mis.data(),sizeof(mis)),Fnv(cms.data(),sizeof(cms)),Fnv(pedStore.data(),pedStore.size()),Fnv(vehStore.data(),vehStore.size())};
                    if(treeOut&&At<void*>(reinterpret_cast<void*>(QUADTREE),0)==root){ out["tree"]=DumpTree(root); out["quad_used"]=usedOf(quadPool)-quadUsed0; out["ptr_used"]=usedOf(ptrPool)-ptrUsed0; }
                    out["ret"]=ret;
                    { json t=json::array(); for(int k=0;k<nev;++k)t.push_back({evs[k].kind,evs[k].a}); out["trace"]=t; }
                    // desmontaje: arboles de vuelta al pool real de nodos (salvo que Shutdown ya liberara el suyo)
                    for(auto* h:hooks)h->State(false);
                    auto* tree=At<ColTreeNode*>(reinterpret_cast<void*>(QUADTREE),0);
                    if(nm=="Initialise"){ if(tree)delete tree; }
                    else if(nm!="Shutdown"||tree==root){ delete root; }
                    root=nullptr;
                    restoreAll();
                    out["leak"]=json::array({usedOf(quadPool)-quadUsed0,usedOf(ptrPool)-ptrUsed0});
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
    restoreAll();
    bool restored=quadLast==savedQuadLast&&ptrLast==savedPtrLast&&usedOf(quadPool)==quadUsed0&&usedOf(ptrPool)==ptrUsed0;
    for(auto& b:blobs)restored&=std::memcmp(reinterpret_cast<void*>(b.p),b.saved.data(),b.n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
