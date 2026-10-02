#pragma once
// Banco dentro del motor: las 8 funciones pesadas de CIplStore (Initialise, Shutdown, RemoveIpl,
// RemoveIplSlot, LoadIplBoundingBox, LoadIpl, LoadIpls, SetupRelatedIpls). Original (hooks del lote
// desactivados) frente a candidato. Para no tocar el mundo real, durante cada llamada se sustituyen:
// los pools de edificios/objetos/dummies por pools propios con entidades falsas (vtable propia), las
// operaciones del quadtree (AddItem/DeleteItem) por stubs, operator new/delete por un asignador lineal
// y LoadObjectInstance/LoadCarGenerator/CWorld::Add/Remove/RemoveCarGenerators/CColAccel/streaming por
// stubs trazados. El pool de IPL real se restaura tras cada ejecucion. Se compara el pool de IPL, los
// pools falsos, los buffers de datos, los globales, el retorno y la traza (sin reservas en los stubs).
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "IplStore.h"
#include "IplDef.h"
#include "FileLoader.h"
#include "FileObjectInstance.h"
#include "tBinaryIplFile.h"
#include "TheCarGenerators.h"
#include "Collision/ColAccel.h"
#include "Core/QuadTreeNode.h"
#include "../ProbeRuntime.hpp"

namespace Ipl2Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
using IplNode = CQuadTreeNode<IplDef*>;
inline uint8*& IplPoolPtr() { return *reinterpret_cast<uint8**>(0x8E3FB0); }
inline void*& QuadTree() { return *reinterpret_cast<void**>(0x8E3FAC); }
inline uint8* Objects(uint8* pool) { return *reinterpret_cast<uint8**>(pool); }
inline uint8* Map(uint8* pool) { return *reinterpret_cast<uint8**>(pool+4); }
inline int32 Size(uint8* pool) { return *reinterpret_cast<int32*>(pool+8); }
inline IplDef* Def(int i) { auto* p=IplPoolPtr(); return (Map(p)[i]&0x80) ? nullptr : reinterpret_cast<IplDef*>(Objects(p)+i*sizeof(IplDef)); }

// traza sin reservas
struct Ev { int kind; uint32 a; }; inline std::array<Ev,16384> evs{}; inline int nev{};
inline void Push(int k,uint32 a) { if(nev<int(evs.size()))evs[nev++]={k,a}; }
inline void PushF(float f) { Push(1,std::bit_cast<uint32>(f)); }
inline void PushBytes(const void* p,size_t n) { for(size_t k=0;k+4<=n;k+=4)Push(2,*reinterpret_cast<const uint32*>(static_cast<const uint8*>(p)+k)); }
enum K { K_TAG=0, K_F=1, K_W=2, K_V=3, K_P=4 };
inline void Tag(uint32 t) { Push(K_TAG,t); }

// pools falsos (edificios 0x38, objetos 0x19C, dummies 0x38)
constexpr int NB=24, NO=8, ND=16;
alignas(16) inline std::array<uint8,NB*0x38> bStore{}; alignas(16) inline std::array<uint8,NO*0x19C> oStore{}; alignas(16) inline std::array<uint8,ND*0x38> dStore{};
inline std::array<uint8,NB> bMap{}; inline std::array<uint8,NO> oMap{}; inline std::array<uint8,ND> dMap{};
alignas(16) inline std::array<uint8,0x14> bPool{}, oPool{}, dPool{};
alignas(16) inline std::array<uint8,0x40> lods{};                 // entidades LOD (array de punteros)
alignas(16) inline std::array<std::array<uint8,0x60>,8> lodEnts{};
alignas(16) inline std::array<uint8,0x40000> arena{}; inline size_t arenaTop{};
alignas(16) inline std::array<uint8,0x2000> data{};              // buffer IPL
alignas(16) inline std::array<uint8,0x14> fakeIplPool{};          // pool de IPL propio (Shutdown)
alignas(16) inline std::array<uint8,256*sizeof(IplDef)> fakeIplStore{}; inline std::array<uint8,256> fakeIplMap{};
alignas(16) inline std::array<uint8,0x80> modelInfo{}, vehicle{};
inline std::array<CEntity*,64> loadedBuildings{};
constexpr int FAKE_MODEL=19991;
inline int nextEntity{}; inline bool cacheLoading{}; inline int32 lastStatic{};
inline bool vehicleExists{}; inline void* benchNode{};

inline uint32 Name(uintptr_t v) {
    // direcciones de los fixtures como desplazamiento etiquetado (estable entre ejecuciones)
    auto in=[&](const void* b,size_t n,uint32 tag)->std::optional<uint32>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return (tag<<24)|uint32(v-uintptr_t(b)); return std::nullopt; };
    if(!v)return 0;
    if(auto r=in(arena.data(),arena.size(),1))return *r; if(auto r=in(bStore.data(),bStore.size(),2))return *r; if(auto r=in(oStore.data(),oStore.size(),3))return *r;
    if(auto r=in(dStore.data(),dStore.size(),4))return *r; if(auto r=in(data.data(),data.size(),5))return *r; if(auto r=in(lodEnts.data(),sizeof(lodEnts),6))return *r;
    if(auto r=in(fakeIplStore.data(),fakeIplStore.size(),7))return *r; if(IplPoolPtr()){ if(auto r=in(Objects(IplPoolPtr()),Size(IplPoolPtr())*sizeof(IplDef),8))return *r; }
    if(benchNode&&v==uintptr_t(benchNode))return 11u<<24;
    if(auto r=in(loadedBuildings.data(),sizeof(loadedBuildings),9))return *r; if(auto r=in(vehicle.data(),vehicle.size(),10))return *r;
    return uint32(v);
}
inline void PushP(const void* p) { Push(K_P,Name(uintptr_t(p))); }

// asignacion
inline void* __cdecl FakeNew(size_t n) { Tag(100); Push(K_V,uint32(n)); arenaTop=(arenaTop+15)&~size_t(15); void* r=arena.data()+arenaTop; arenaTop+=n; if(arenaTop>arena.size())arenaTop=0; return r; }
inline void __cdecl FakeDelete(void* p) { Tag(101); PushP(p); }
// streaming y CColAccel
inline bool __cdecl RequestModel(int32 id,int32 f) { Tag(102); Push(K_V,id); Push(K_V,f); return true; }
inline void __cdecl RemoveModel(int32 id) { Tag(103); Push(K_V,id); }
inline void __cdecl LoadAllRequested(bool b) { Tag(104); Push(K_V,b); auto& pp=*reinterpret_cast<CEntity***>(0x8E3EFC); if(pp){ for(int k=0;k<2;++k){ *pp=reinterpret_cast<CEntity*>(lodEnts[k].data()); ++pp; } } }
inline bool __cdecl CacheLoading() { Tag(105); return cacheLoading; }
inline IplDef __cdecl GetIplDef(int32 i) { Tag(106); Push(K_V,i); IplDef d{}; d.bb=CRect(-100.f-i,-200.f-i,300.f+i,400.f+i); strcpy_s(d.name,"cached"); d.staticIdx=int16(77); d.loaded=1; d.isLarge=char(i&1); return d; }
inline void __cdecl SetIplDef(int32 i,IplDef d) { Tag(107); Push(K_V,i); PushBytes(&d,sizeof(d)); }
// quadtree
inline void __fastcall AddItem(void* node,void*,IplDef* item,const CRect& r) { Tag(108); PushP(node); PushP(item); PushF(r.left); PushF(r.top); PushF(r.right); PushF(r.bottom); }
inline void __fastcall DeleteItem(void* node,void*,IplDef* item) { Tag(109); PushP(node); PushP(item); }
// entidades
inline void __fastcall EntDtor(void* e,void*,uint8 f) { Tag(110); PushP(e); Push(K_V,f); }
inline void __fastcall EntAdd(void* e,void*) { Tag(111); PushP(e); }
inline std::array<CRect,NB> rects{};
inline CRect* __fastcall EntBoundRect(void* e,void*,CRect* out) { Tag(112); PushP(e); const auto k=(uintptr_t(e)-uintptr_t(bStore.data()))/0x38; *out=k<NB ? rects[k] : CRect{}; return out; }
template<int N> void* __fastcall EntTrap(void* e,void*) { Tag(199); Push(K_V,N); return nullptr; }
inline std::array<void*,24> entVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((entVtbl[N]=reinterpret_cast<void*>(&EntTrap<N>)),...); }
inline CEntity* NextLoaded() { // entidades devueltas por LoadObjectInstance: dentro del pool falso de edificios
    const int k=nextEntity++%NB; auto* e=bStore.data()+k*0x38; return reinterpret_cast<CEntity*>(e); }
inline uint32 seedEnt{}; inline bool allowLod{};
inline void PrepEntity(CEntity* ent,uint32 r) {
    auto* e=reinterpret_cast<uint8*>(ent); std::memset(e,0,0x38); *reinterpret_cast<void**>(e)=entVtbl.data();
    e[0x36]=uint8(ENTITY_TYPE_BUILDING); *reinterpret_cast<int16*>(e+0x22)=int16(FAKE_MODEL);
    *reinterpret_cast<int32*>(e+0x30)=(!allowLod||r%3==0) ? -1 : int32(r%4); // indice de LOD (solo si el IPL tiene array de LOD, como en el juego)
    const auto k=(uintptr_t(e)-uintptr_t(bStore.data()))/0x38; const float l=float(int(r%200))-100.f, t=float(int((r>>8)%200)); if(k<NB)rects[k]=CRect(l,t-float((r>>4)%50),l+float(r%50),t); // GetBoundRect
}
inline CEntity* __cdecl LoadInstBin(CFileObjectInstance* inst,const char* name) { Tag(113); PushP(inst); PushP(name); auto* e=NextLoaded(); PrepEntity(e,seedEnt*2654435761u+uint32(nextEntity)*40503u); return e; }
inline CEntity* __cdecl LoadInstText(const char* line) { Tag(114); PushP(line); auto* e=NextLoaded(); PrepEntity(e,seedEnt*2654435761u+uint32(nextEntity)*40503u); return e; }
inline void __cdecl LoadCarGen(CFileCarGenerator* cg,int32 ipl) { Tag(115); PushP(cg); Push(K_V,ipl); }
inline void __cdecl WorldRemove(CEntity* e) { Tag(116); PushP(e); }
inline void __cdecl WorldAdd(CEntity* e) { Tag(117); PushP(e); }
inline void __cdecl RemoveCarGens(uint8 ipl) { Tag(118); Push(K_V,ipl); }
inline CVehicle* __cdecl FindVehicle(int32 id,bool remote) { Tag(119); Push(K_V,uint32(id)); Push(K_V,remote); return vehicleExists ? reinterpret_cast<CVehicle*>(vehicle.data()) : nullptr; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CIplStore")for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,8> fns{{
    {"RemoveIpl",0x404B20,512},{"RemoveIplSlot",0x405B60,256},{"LoadIplBoundingBox",0x405C00,512},{"LoadIpl",0x406080,512},
    {"SetupRelatedIpls",0x404DE0,512},{"LoadIpls",0x405170,512},{"Initialise",0x405EC0,64},{"Shutdown",0x405FA0,128},
}};

inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ipl2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ipl2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ipl2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,8> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,24>{}); entVtbl[0]=reinterpret_cast<void*>(&EntDtor); entVtbl[2]=reinterpret_cast<void*>(&EntAdd); entVtbl[9]=reinterpret_cast<void*>(&EntBoundRect);
    // estado real que se guarda y restaura
    uint8* const realPool=IplPoolPtr(); void* const realTree=QuadTree(); const int32 size=Size(realPool);
    const std::vector<uint8> savedHeader(realPool,realPool+0x14), savedObjects(Objects(realPool),Objects(realPool)+size*sizeof(IplDef)), savedMap(Map(realPool),Map(realPool)+size);
    struct Blob { void* p; size_t n; std::vector<uint8> saved; };
    std::vector<Blob> blobs{{reinterpret_cast<void*>(0x8E3F00),4},{reinterpret_cast<void*>(0x8E3F08),40*4},{reinterpret_cast<void*>(0x8E3FA8),1},{reinterpret_cast<void*>(0x8E3FD0),12},
        {reinterpret_cast<void*>(0x8E3EF8),4},{reinterpret_cast<void*>(0x8E3EFC),4},{&CStreaming::ms_disableStreaming,1},{&CTheScripts::MissionCleanUp,sizeof(CTheScripts::MissionCleanUp)},
        {reinterpret_cast<void*>(0xB74498),12},{&CModelInfo::ms_modelInfoPtrs[FAKE_MODEL],4}};
    for(auto& b:blobs)b.saved.assign(static_cast<uint8*>(b.p),static_cast<uint8*>(b.p)+b.n);
    auto restoreReal=[&]{ IplPoolPtr()=realPool; QuadTree()=realTree; std::memcpy(realPool,savedHeader.data(),savedHeader.size()); std::memcpy(Objects(realPool),savedObjects.data(),savedObjects.size()); std::memcpy(Map(realPool),savedMap.data(),savedMap.size());
                          for(auto& b:blobs)std::memcpy(b.p,b.saved.data(),b.n); };
    // referencias validas para la limpieza de mision
    std::vector<int32> pedRefs, vehRefs; for(int k=0;k<int(GetPedPool()->GetSize());++k)if(auto* p=GetPedPool()->GetAt(k))pedRefs.push_back(GetPedPool()->GetRef(p));
    for(int k=0;k<int(GetVehiclePool()->GetSize());++k)if(auto* v=GetVehiclePool()->GetAt(k))vehRefs.push_back(GetVehiclePool()->GetRef(v));
    std::array<bool,8> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch p1(bit_cast<uintptr_t>(&CStreaming::RequestModel),RequestModel), p1e(0x4087E0,RequestModel);
        EntryPatch p2(bit_cast<uintptr_t>(&CStreaming::RemoveModel),RemoveModel), p2e(0x4089A0,RemoveModel);
        EntryPatch p3(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),LoadAllRequested), p3e(0x40EA10,LoadAllRequested);
        EntryPatch p4(bit_cast<uintptr_t>(&CColAccel::isCacheLoading),CacheLoading), p4e(0x5B2AC0,CacheLoading);
        EntryPatch p5(bit_cast<uintptr_t>(&CColAccel::getIplDef),GetIplDef), p5e(0x5B2EF0,GetIplDef);
        EntryPatch p6(bit_cast<uintptr_t>(&CColAccel::setIplDef),SetIplDef), p6e(0x5B2ED0,SetIplDef);
        EntryPatch p7(bit_cast<uintptr_t>(&IplNode::AddItem),AddItem), p7e(0x552CD0,AddItem);
        EntryPatch p8(bit_cast<uintptr_t>(static_cast<void(IplNode::*)(IplDef*)>(&IplNode::DeleteItem)),DeleteItem), p8e(0x552A40,DeleteItem);
        EntryPatch p9(bit_cast<uintptr_t>(static_cast<CEntity*(*)(CFileObjectInstance*,const char*)>(&CFileLoader::LoadObjectInstance)),LoadInstBin), p9e(0x538090,LoadInstBin);
        EntryPatch p10(bit_cast<uintptr_t>(static_cast<CEntity*(*)(const char*)>(&CFileLoader::LoadObjectInstance)),LoadInstText), p10e(0x538690,LoadInstText);
        EntryPatch p11(bit_cast<uintptr_t>(static_cast<void(*)(CFileCarGenerator*,int32)>(&CFileLoader::LoadCarGenerator)),LoadCarGen), p11e(0x537990,LoadCarGen);
        EntryPatch p12(bit_cast<uintptr_t>(&CWorld::Remove),WorldRemove), p12e(0x563280,WorldRemove);
        EntryPatch p13(bit_cast<uintptr_t>(&CWorld::Add),WorldAdd), p13e(0x563220,WorldAdd);
        EntryPatch p14(bit_cast<uintptr_t>(&CTheCarGenerators::RemoveCarGenerators),RemoveCarGens), p14e(0x6F3240,RemoveCarGens);
        EntryPatch p15(bit_cast<uintptr_t>(&FindPlayerVehicle),FindVehicle), p15e(0x56E0D0,FindVehicle);
        std::string only; if(std::ifstream of(dir/"ipl2-only.txt"); of)std::getline(of,only);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; if(!only.empty()&&only!=fn.name)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2B2Bu; nev=0; arenaTop=0; nextEntity=0; seedEnt=uint32(i);
                    restoreReal();
                    // banderas aleatorias en los IPL reales
                    std::vector<int> valid; for(int k=1;k<size;++k)if(auto* d=Def(k)){ valid.push_back(k);
                        d->loaded=char(next()%2); d->loadRequested=next()%2; d->disableDynamicStreaming=next()%3==0; d->ignoreWhenDeleted=char(next()%2); d->isInterior=next()%4==0; }
                    const int slot=valid.empty() ? 1 : valid[next()%valid.size()];
                    // pools falsos
                    auto setPool=[&](std::array<uint8,0x14>& h,uint8* st0,uint8* mp,int n){ std::memset(h.data(),0,h.size()); *reinterpret_cast<uint8**>(h.data())=st0; *reinterpret_cast<uint8**>(h.data()+4)=mp; *reinterpret_cast<int32*>(h.data()+8)=n; };
                    setPool(bPool,bStore.data(),bMap.data(),NB); setPool(oPool,oStore.data(),oMap.data(),NO); setPool(dPool,dStore.data(),dMap.data(),ND);
                    *reinterpret_cast<void**>(0xB74498)=bPool.data(); *reinterpret_cast<void**>(0xB7449C)=oPool.data(); *reinterpret_cast<void**>(0xB744A0)=dPool.data();
                    auto fillEnts=[&](uint8* st0,uint8* mp,int n,size_t es,bool obj){ for(int k=0;k<n;++k){ auto* e=st0+k*es; std::memset(e,0,es); *reinterpret_cast<void**>(e)=entVtbl.data();
                        e[0x2E]=uint8(next()%3==0 ? slot : valid.empty() ? 0 : valid[next()%valid.size()]); mp[k]=uint8(next()%5==0 ? 0x80|(next()%0x7F) : next()%0x7F);
                        e[0x36]=uint8(obj ? ENTITY_TYPE_OBJECT : ENTITY_TYPE_BUILDING);
                        if(obj&&next()%3==0)*reinterpret_cast<void**>(e+0x170)=dStore.data()+(next()%ND)*0x38; } };
                    fillEnts(bStore.data(),bMap.data(),NB,0x38,false); fillEnts(oStore.data(),oMap.data(),NO,0x19C,true); fillEnts(dStore.data(),dMap.data(),ND,0x38,false);
                    for(int k=0;k<ND;++k)dStore[k*0x38+0x36]=uint8(ENTITY_TYPE_DUMMY);
                    if(auto* d=Def(slot)){ d->firstBuilding=int16(next()%NB); d->lastBuilding=int16(next()%NB); d->firstDummy=int16(next()%ND); d->lastDummy=int16(next()%ND);
                        if(next()%4==0){ d->firstBuilding=SHRT_MAX; d->lastBuilding=SHRT_MIN; } }
                    // LOD y modelo
                    for(auto& le:lodEnts)std::memset(le.data(),0,le.size());
                    for(int k=0;k<8;++k)reinterpret_cast<void**>(lods.data())[k]=lodEnts[k].data();
                    reinterpret_cast<void**>(0x8E3F08)[3]=lods.data(); if(auto* d=Def(slot))d->staticIdx=int16(next()%3==0 ? -1 : 3); allowLod=Def(slot)&&Def(slot)->staticIdx!=-1;
                    std::memset(modelInfo.data(),0,modelInfo.size()); *reinterpret_cast<float*>(modelInfo.data()+0x18)=next()%2 ? uni(0.f,149.f) : uni(150.f,400.f);
                    CModelInfo::ms_modelInfoPtrs[FAKE_MODEL]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());
                    cacheLoading=next()%3==0; CStreaming::ms_disableStreaming=next()%8==0;
                    // buffer de datos
                    std::memset(data.data(),0,data.size()); int32 dataSize=0; const bool binary=next()%2;
                    if(binary){ auto* h=reinterpret_cast<tBinaryIplFile*>(data.data()); std::memcpy(h->fourcc,"bnry",4);
                        h->numInst=next()%6==0 ? 0 : 1+next()%6; if(next()%8==0)h->numInst|=0x10000; h->offsetInst=0x4C; h->numCarGens=next()%3; h->offsetCarGens=0x4C+0x28*8;
                        for(size_t b=0x4C;b<0x4C+0x28*8+0x30*4;++b)data[b]=uint8(next()); dataSize=int32(0x4C+0x28*8+0x30*4); }
                    else { std::string t="# comentario\n\ninst\n"; const int n=int(next()%5); for(int k=0;k<n;++k)t+=std::format("{}, modelo{}, 0, 1.0, 2.0, 3.0, 0, 0, 0, 1, -1\n",1000+k,k);
                        t+="end\ncars\n1, 2, 3\nend\n"; if(next()%3==0)t+="inst\n5, x, 0, 0,0,0, 0,0,0,1, -1\nend\n"; std::memcpy(data.data(),t.data(),t.size()); dataSize=int32(t.size()); }
                    // limpieza de mision
                    for(auto& m:CTheScripts::MissionCleanUp.m_Objects){ const uint32 r=next()%6; m.type=int8(r==0 ? MISSION_CLEANUP_ENTITY_TYPE_PED : r==1 ? MISSION_CLEANUP_ENTITY_TYPE_VEHICLE : 0);
                        m.handle=m.type==MISSION_CLEANUP_ENTITY_TYPE_PED ? (pedRefs.empty()||next()%4==0 ? 0x101 : pedRefs[next()%pedRefs.size()]) : m.type==MISSION_CLEANUP_ENTITY_TYPE_VEHICLE ? (vehRefs.empty()||next()%4==0 ? 0x101 : vehRefs[next()%vehRefs.size()]) : 0; }
                    std::memset(vehicle.data(),0,vehicle.size()); vehicleExists=next()%2; *reinterpret_cast<CVector*>(vehicle.data()+0x44)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f};
                    // pool de IPL propio (Shutdown) y arrays de entidades
                    std::memset(fakeIplStore.data(),0xEE,fakeIplStore.size()); for(int k=0;k<256;++k)fakeIplMap[k]=uint8(next()%4==0 ? next()%0x7F : 0x80|(next()%0x7F));
                    for(int k=0;k<256;++k)if(!(fakeIplMap[k]&0x80)){ auto* d=reinterpret_cast<IplDef*>(fakeIplStore.data()+k*sizeof(IplDef)); *d=IplDef{}; d->loaded=0; }
                    setPool(fakeIplPool,fakeIplStore.data(),fakeIplMap.data(),256); fakeIplPool[0x10]=uint8(next()%2);
                    *reinterpret_cast<int32*>(0x8E3F00)=int32(next()%5); for(int k=0;k<40;++k)reinterpret_cast<uint32*>(0x8E3F08)[k]=k==3 ? uint32(uintptr_t(lods.data())) : 0x7000000u+k;
                    *reinterpret_cast<bool*>(0x8E3FA8)=next()%2; *reinterpret_cast<CVector*>(0x8E3FD0)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,50.f)};
                    *reinterpret_cast<int32*>(0x8E3EF8)=int32(next()%3); *reinterpret_cast<void**>(0x8E3EFC)=nullptr;
                    CVector pos{uni(-3000.f,3000.f),uni(-3000.f,3000.f),next()%4==0 ? uni(900.f,1100.f) : uni(0.f,100.f)};
                    if(auto* d=Def(slot); d&&next()%2){ pos.x=uni(d->bb.left-250.f,d->bb.right+250.f); pos.y=uni(d->bb.top-250.f,d->bb.bottom+250.f); }

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const std::string nm=fn.name; int64 ret=-12345;
                    if(nm=="RemoveIpl")reinterpret_cast<void(__cdecl*)(int32)>(fn.address)(slot);
                    else if(nm=="RemoveIplSlot")reinterpret_cast<void(__cdecl*)(int32)>(fn.address)(slot);
                    else if(nm=="LoadIplBoundingBox"||nm=="LoadIpl"){
                        if(nm=="LoadIpl"&&Def(slot)&&next()%2){ Def(slot)->bb=CRect{}; } // caja aun sin cargar (invertida)
                        if(next()%3==0)*reinterpret_cast<CEntity***>(0x8E3EFC)=loadedBuildings.data();
                        ret=reinterpret_cast<bool(__cdecl*)(int32,char*,int32)>(fn.address)(slot,reinterpret_cast<char*>(data.data()),dataSize); }
                    else if(nm=="SetupRelatedIpls"){ std::string fname="DATA\\MAPS\\x\\";
                        static const char* ints[]{"gen_int1","GEN_INT5","savehous","levelmap","int_veg","noint"};
                        if(next()%3==0)fname+=ints[next()%6];
                        else if(auto* d=Def(valid.empty()?1:valid[next()%valid.size()])){ std::string n(d->name, strnlen(d->name,18)); if(auto u=n.find("_stream");u!=std::string::npos)n=n.substr(0,u); fname+=n; }
                        fname+=".IPL"; if(next()%10==0)fname="sinbarra.ipl"; if(next()%10==0)fname="DATA\\sinpunto";
                        static std::string keep; keep=fname;
                        ret=reinterpret_cast<int32(__cdecl*)(const char*,int32,CEntity**)>(fn.address)(keep.c_str(),int32(next()%5),loadedBuildings.data()); }
                    else if(nm=="LoadIpls")reinterpret_cast<void(__cdecl*)(CVector,bool)>(fn.address)(pos,next()%2);
                    else if(nm=="Initialise"){ IplPoolPtr()=next()%4==0 ? fakeIplPool.data() : nullptr; QuadTree()=nullptr;
                        EntryPatch nw(0x82119A,FakeNew), na(0x821195,FakeNew), dl(0x8214BD,FakeDelete), da(0x8213AE,FakeDelete);
                        reinterpret_cast<void(__cdecl*)()>(fn.address)(); }
                    else if(nm=="Shutdown"){ IplPoolPtr()=fakeIplPool.data();
                        // nodo real del quadtree (pool de nodos del juego): el original y la DLL lo destruyen
                        auto* node=reinterpret_cast<void*(__cdecl*)(size_t)>(0x552C80)(0x28); CRect wb(-3000.f,-3000.f,3000.f,3000.f); reinterpret_cast<void*(__thiscall*)(void*,const CRect*,int32)>(0x552830)(node,&wb,3); QuadTree()=node; benchNode=node;
                        EntryPatch nw(0x82119A,FakeNew), na(0x821195,FakeNew), dl(0x8214BD,FakeDelete), da(0x8213AE,FakeDelete);
                        reinterpret_cast<void(__cdecl*)()>(fn.address)(); }
                    // resultado
                    json r=json::object(); r["ret"]=ret; r["switches"]=switches;
                    { json t=json::array(); for(int k=0;k<nev;++k)t.push_back({evs[k].kind,evs[k].a}); r["trace"]=t; }
                    if(IplPoolPtr()==realPool){ json ch=json::array(); for(int k=0;k<size;++k){ auto* o=Objects(realPool)+k*sizeof(IplDef); const bool freeNow=Map(realPool)[k]&0x80;
                            if(!freeNow&&std::memcmp(o,savedObjects.data()+k*sizeof(IplDef),sizeof(IplDef)))ch.push_back({k,ReversingProbe::Bytes(o,sizeof(IplDef))}); } r["defs"]=ch;
                        r["map"]=Hash(Map(realPool),size); r["header"]=ReversingProbe::Bytes(realPool,0x14); }
                    else r["ipl_pool"]=json::array({Name(uintptr_t(IplPoolPtr())), IplPoolPtr() ? json(ReversingProbe::Bytes(IplPoolPtr(),0x12)) : json(nullptr)});
                    if(nm=="Initialise"){ auto* p=IplPoolPtr(); if(p&&p!=fakeIplPool.data()){ r["new_pool_map"]=Hash(Map(p),Size(p)); auto* d0=Objects(p); auto* d=reinterpret_cast<IplDef*>(d0);
                            const auto len=strnlen(d->name,18); std::memset(d->name+len,0,18-len); d0[0x32]=d0[0x33]=0; r["slot0"]=ReversingProbe::Bytes(d0,sizeof(IplDef)); }
                        r["tree"]=QuadTree() ? json(ReversingProbe::Bytes(QuadTree(),0x28)) : json(nullptr);
                        if(QuadTree()){ reinterpret_cast<void(__cdecl*)(void*)>(0x552C90)(QuadTree()); } }
                    if(nm=="Shutdown"){ r["fake_pool_header"]=ReversingProbe::Bytes(fakeIplPool.data(),0x12);
                        // el nodo debe haber vuelto al pool de nodos del juego
                        auto* np=*reinterpret_cast<uint8**>(0xB745BC); r["node_freed"]=np&&benchNode ? json((Map(np)[(uintptr_t(benchNode)-uintptr_t(Objects(np)))/0x28]&0x80)!=0) : json(nullptr); benchNode=nullptr; }
                    r["bmap"]=ReversingProbe::Bytes(bMap.data(),NB); r["ents"]=Hash(bStore.data(),bStore.size());
                    r["data"]=Hash(data.data(),data.size()); r["loaded"]=Hash(loadedBuildings.data(),sizeof(loadedBuildings));
                    r["lods"]=json::array(); for(auto& le:lodEnts)r["lods"].push_back(le[0x34]);
                    r["globals"]=json::array({*reinterpret_cast<int32*>(0x8E3F00),Hash(reinterpret_cast<void*>(0x8E3F08),160),*reinterpret_cast<uint8*>(0x8E3FA8),*reinterpret_cast<int32*>(0x8E3EF8),
                                               Name(uintptr_t(*reinterpret_cast<void**>(0x8E3EFC))),Name(uintptr_t(QuadTree())==uintptr_t(realTree) ? 1 : uintptr_t(QuadTree()))});
                    restoreReal();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CIplStore::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CIplStore::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",std::string("CIplStore::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restoreReal();
    const bool restored=std::memcmp(Objects(realPool),savedObjects.data(),savedObjects.size())==0&&std::memcmp(Map(realPool),savedMap.data(),savedMap.size())==0&&IplPoolPtr()==realPool&&QuadTree()==realTree;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
