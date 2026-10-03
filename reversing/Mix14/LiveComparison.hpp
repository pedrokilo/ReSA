#pragma once
// Banco dentro del motor: gestor de atractores y matriculas (lista de 14, 28 funciones de 2 clases).
// CPedAttractorManager (las 14 funciones enganchadas) corre sobre un gestor FALSO (0xA0 bytes, 10 listas SArray con
// almacenamiento propio) y un pool de atractores FALSO (20 slots de 0xC4 colocados temporalmente en 0xB744C4) con
// atractores falsos: vtable propia (GetType, destructor con borrado y BroadcastDeparture trazados), listas de peatones
// propias, efectos 2D y entidades falsos. GetPedAttractorManager usa 0xC09840 (guardado y restaurado).
// CCustomCarPlateMgr (14 funciones) usa texturas, rasters, materiales, geometrias, atomics y clumps falsos con las
// primitivas RW simuladas; las globales 0xC3EF5C..0xC3EF84 y CWeather::WeatherRegion se guardan y restauran.
// Original (hooks de las dos clases desactivados) frente a candidato con fixtures identicos. Se simulan y trazan (sin
// reservar memoria): operator new/delete/new[]/delete[] (arena fija, solo durante la llamada), rand (0x821B1E y
// CGeneral::GetRandomNumber), CPedAttractor::BroadcastArrival/DeRegisterPed/RegisterPed/AbortPedTasks,
// CTaskManager::FindActiveTaskByType, CScripted2dEffects::GetIndex, CEventAttractor::IsEffectActive,
// CWorld::GetIsLineOfSightClear, CPedGeometryAnalyser::IsWanderPathClear, CEntity::RegisterReference, CTxdStore
// (FindTxdSlot, Push/Pop/SetCurrentTxd, RemoveTxd, RemoveTxdSlot) y RwTextureRead/Create/SetName/Destroy,
// RwRasterCreate/Destroy/Lock/Unlock, RpMaterialSetTexture, RpGeometryForAllMaterials, RpClumpForAllAtomics y
// RpGeometryReplaceOldMaterialWithNewMaterial (las de iteracion llaman de verdad a la callback recibida).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblebugfixes/Bugs.hpp>
#include "PedAttractorManager.h"
#include "PedAttractor.h"
#include "CustomCarPlateMgr.h"
#include "TxdStore.h"
#include "Scripted2dEffects.h"
#include "Events/EventAttractor.h"
#include "PedGeometryAnalyser.h"
#include "TaskManager.h"
#include "../ProbeRuntime.hpp"

namespace Mix14Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- registro sin reservas (todos los stubs: pueden correr con operator new sustituido) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,8192> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }

// --- fixtures del gestor de atractores ---
constexpr int NA=20, NP=8, NFX=6, NE=4, ASZ=0xC4;
alignas(16) inline std::array<uint8,NA*ASZ> att{};
alignas(16) inline std::array<uint8,NA> poolMap{};
alignas(16) inline std::array<uint8,0x14> pool{};
alignas(16) inline std::array<uint8,0xA0> mgr{};
alignas(16) inline std::array<std::array<void*,16>,10> mgrStore{};
alignas(16) inline std::array<std::array<std::array<void*,8>,2>,NA> pedArr{};
alignas(16) inline std::array<std::array<uint8,0x800>,NP> peds{};
alignas(16) inline std::array<std::array<uint8,0x100>,NP> intel{};
alignas(16) inline std::array<std::array<float,18>,NP> pedMat{};
alignas(16) inline std::array<std::array<uint8,0x20>,4> tasks{};
alignas(16) inline std::array<std::array<uint8,0x40>,NFX> fxs{};
alignas(16) inline std::array<std::array<uint8,0x200>,NE> ents{};
alignas(16) inline std::array<std::array<float,18>,NE> entMat{};
alignas(16) inline std::array<std::array<float,18>,1> argMat{};
// 64 KB: los casos usan menos de 4 KB; el resto deja sitio a las reservas del manejador de fallos si algo falla con operator new sustituido
alignas(16) inline std::array<uint8,65536> arena{}; inline size_t arenaTop{};
alignas(16) inline std::array<void*,8> fakeVt{};
// --- fixtures de matriculas ---
alignas(16) inline std::array<std::array<uint8,0x58>,6> texs{};
alignas(16) inline std::array<std::array<uint8,0x34>,4> rasts{};
alignas(16) inline std::array<uint8,24576> charsetBuf{};
alignas(16) inline std::array<uint8,16384> plateBuf{};
alignas(16) inline std::array<std::array<uint8,0x20>,8> mats{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> geoms{};
alignas(16) inline std::array<std::array<uint8,0x20>,6> atoms{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> clumps{};
alignas(16) inline std::array<char,16> plateText{};
alignas(16) inline std::array<char,64> outBuf{};
alignas(16) inline std::array<uint8,0x20> dataBuf{};
inline std::array<std::array<int8,5>,4> geomMats{};   // indices de material, -1 = fin
inline std::array<std::array<int8,5>,2> clumpAtoms{}; // indices de atomic, -1 = fin

struct Range { const void* base; size_t size, stride; uint32 cat; };
inline std::vector<Range> ranges;
inline void InitRanges() {
    ranges={{att.data(),sizeof(att),ASZ,1},{peds.data(),sizeof(peds),0x800,2},{fxs.data(),sizeof(fxs),0x40,3},{ents.data(),sizeof(ents),0x200,4},
            {arena.data(),sizeof(arena),65536,5},{mgrStore.data(),sizeof(mgrStore),64,6},{pedArr.data(),sizeof(pedArr),32,7},{intel.data(),sizeof(intel),0x100,8},
            {tasks.data(),sizeof(tasks),0x20,9},{fakeVt.data(),sizeof(fakeVt),32,10},{mgr.data(),sizeof(mgr),0xA0,11},{pool.data(),sizeof(pool),0x14,12},
            {poolMap.data(),sizeof(poolMap),NA,13},{pedMat.data(),sizeof(pedMat),0x48,14},{entMat.data(),sizeof(entMat),0x48,15},{argMat.data(),sizeof(argMat),0x48,16},
            {texs.data(),sizeof(texs),0x58,20},{rasts.data(),sizeof(rasts),0x34,21},{charsetBuf.data(),sizeof(charsetBuf),4096,22},{plateBuf.data(),sizeof(plateBuf),4096,23},
            {mats.data(),sizeof(mats),0x20,24},{geoms.data(),sizeof(geoms),0x60,25},{atoms.data(),sizeof(atoms),0x20,26},{clumps.data(),sizeof(clumps),0x40,27},
            {plateText.data(),sizeof(plateText),16,28},{outBuf.data(),sizeof(outBuf),64,29},{dataBuf.data(),sizeof(dataBuf),0x20,30}};
}
// Identificador estable de un puntero de los fixtures (categoria, indice, desplazamiento); 0 = nulo, ~0 = ajeno.
inline uint32 Id(const void* p) {
    const auto u=uintptr_t(p); if(!u) return 0;
    for(auto& r:ranges){ const auto b=uintptr_t(r.base); if(u>=b&&u<b+r.size){ const auto off=u-b; return (r.cat<<24)|(uint32(off/r.stride)<<12)|uint32(off%r.stride); } }
    return 0xFFFFFFFFu;
}
inline json Map(uint32 v) { const uint32 id=Id(reinterpret_cast<const void*>(uintptr_t(v))); return (v&&id!=0xFFFFFFFFu) ? json(int64(id)+(int64(1)<<32)) : json(v); }
inline json Dw(const void* p,size_t n) { json r=json::array(); auto* d=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Map(d[k])); return r; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- stubs comunes ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { Push(51); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline void* __cdecl FakeNew(size_t n){ const size_t at=arenaTop; const size_t sz=(n+15)&~size_t(15); if(at+sz>arena.size()){Push(1,uint32(n),0xFFFF);return nullptr;} arenaTop+=sz; Push(1,uint32(n),uint32(at)); return arena.data()+at; }
inline void* __cdecl FakeNewArr(size_t n){ Push(3,uint32(n)); return FakeNew(n); }
inline void __cdecl FakeDelete(void* p){ Push(2,Id(p)); }
inline void __cdecl FakeDeleteArr(void* p){ Push(4,Id(p)); }

// --- stubs del gestor de atractores ---
inline uint32 arrMask{}, arrCalls{}, emptyMask{}, emptyCalls{}, retMask{}, retCalls{}, taskMask{}, taskCalls{}, losMask{}, losCalls{}, wanderVals{}, wanderCalls{};
inline int32 indexValue{}; inline uint32 activeMask{}, activeCalls{};
inline bool Bit(uint32 m,uint32& c){ return (m>>(c++%32))&1; }
inline void MaybeEmpty(void* self){ if(Bit(emptyMask,emptyCalls)){ auto* a=static_cast<uint8*>(self); *reinterpret_cast<uint32*>(a+0x14)=*reinterpret_cast<uint32*>(a+0x10); *reinterpret_cast<uint32*>(a+0x24)=*reinterpret_cast<uint32*>(a+0x20); Push(9,Id(self)); } }
inline int32 __fastcall FakeGetType(void* self,void*){ Push(10,Id(self)); return *reinterpret_cast<int32*>(static_cast<uint8*>(self)+0xC0); }
inline void* __fastcall FakeDeletingDtor(void* self,void*,uint32 flags){ Push(11,Id(self),flags); return self; }
inline bool __fastcall FakeBroadcastDeparture(void* self,void*,CPed* ped){ Push(12,Id(self),Id(ped)); MaybeEmpty(self); return Bit(retMask,retCalls); }
inline void __fastcall FakeTrapVt(void* self,void*){ Push(13,Id(self)); }
inline bool __fastcall FakeBroadcastArrival(void* self,void*,CPed* ped){ Push(14,Id(self),Id(ped)); return Bit(arrMask,arrCalls); }
inline bool __fastcall FakeDeRegisterPed(void* self,void*,CPed* ped){ Push(15,Id(self),Id(ped)); MaybeEmpty(self); return Bit(retMask,retCalls); }
inline void __fastcall FakeAbortPedTasks(void* self,void*){ Push(16,Id(self)); }
inline bool __fastcall FakeRegisterPed(void* self,void*,CPed* ped){ Push(17,Id(self),Id(ped)); return Bit(retMask,retCalls); }
inline CTask* __fastcall FakeFindActiveTaskByType(void* self,void*,int32 type){ Push(18,Id(self),uint32(type)); return Bit(taskMask,taskCalls) ? reinterpret_cast<CTask*>(tasks[taskCalls%4].data()) : nullptr; }
inline int32 __cdecl FakeGetIndex(const void* fx){ Push(19,Id(fx)); return indexValue; }
inline bool __cdecl FakeIsEffectActive(void* e,const void* fx){ Push(20,Id(e),Id(fx)); return Bit(activeMask,activeCalls); }
inline bool __cdecl FakeLineOfSight(const CVector& a,const CVector& b,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){
    Push(21,uint32(Fnv(&a,12)),uint32(Fnv(&b,12))); Push(21,uint32(b1)|uint32(b2)<<1|uint32(b3)<<2|uint32(b4)<<3|uint32(b5)<<4|uint32(b6)<<5|uint32(b7)<<6); return Bit(losMask,losCalls); }
inline uint32 __cdecl FakeWanderPathClear(const CVector& a,const CVector& b,float h,int32 s){ Push(22,uint32(Fnv(&a,12)),uint32(Fnv(&b,12))); Push(22,F(h),uint32(s)); return (wanderVals>>(2*(wanderCalls++%16)))&3 ? 4u : uint32(wanderCalls%4); }
inline void __fastcall FakeRegisterReference(void* self,void*,CEntity** ref){ Push(23,Id(self),Id(ref)); }

// --- stubs RW / CTxdStore ---
inline uint32 lockMask{}, lockCalls{}, rasterMask{}, rasterCalls{}, texMask{}, texCalls{}, readCalls{}; inline int32 txdSlot{};
inline RwTexture* __cdecl FakeTextureRead(const char* n,const char* m){ Push(30,uint32(Fnv(n,strlen(n))),m ? Id(m) : 0); return reinterpret_cast<RwTexture*>(texs[readCalls++%4].data()); }
inline RwUInt8* __cdecl FakeRasterLock(RwRaster* r,RwUInt8 level,RwInt32 mode){ Push(31,Id(r),uint32(level)|uint32(mode)<<8); if(!Bit(lockMask,lockCalls))return nullptr; return reinterpret_cast<uint8*>(r)==rasts[1].data() ? plateBuf.data() : charsetBuf.data(); }
inline RwRaster* __cdecl FakeRasterUnlock(RwRaster* r){ Push(32,Id(r)); return r; }
inline RwBool __cdecl FakeTextureDestroy(RwTexture* t){ Push(33,Id(t)); return TRUE; }
inline RwRaster* __cdecl FakeRasterCreate(RwInt32 w,RwInt32 h,RwInt32 d,RwInt32 f){ Push(34,uint32(w)|uint32(h)<<16,uint32(d)|uint32(f)<<8); return Bit(rasterMask,rasterCalls) ? reinterpret_cast<RwRaster*>(rasts[1].data()) : nullptr; }
inline RwBool __cdecl FakeRasterDestroy(RwRaster* r){ Push(35,Id(r)); return TRUE; }
inline RwTexture* __cdecl FakeTextureCreate(RwRaster* r){ Push(36,Id(r)); return Bit(texMask,texCalls) ? reinterpret_cast<RwTexture*>(texs[5].data()) : nullptr; }
inline RwTexture* __cdecl FakeTextureSetName(RwTexture* t,const char* n){ Push(37,Id(t),uint32(Fnv(n,strnlen(n,32)))); return t; }
inline RpMaterial* __cdecl FakeMaterialSetTexture(RpMaterial* m,RwTexture* t){ Push(38,Id(m),Id(t)); return m; }
inline RpGeometry* __cdecl FakeForAllMaterials(RpGeometry* g,RpMaterialCallBack cb,void* data){
    Push(39,Id(g),Id(data)); const auto gi=Id(g);
    if(gi>>24==25&&(gi&0xFFF)==0) for(int8 m:geomMats[(gi>>12)&0xFFF]){ if(m<0)break; auto* mat=reinterpret_cast<RpMaterial*>(mats[m].data()); auto* r=cb(mat,data); Push(40,Id(mat),Id(r)); if(!r)break; }
    return g; }
inline RpClump* __cdecl FakeForAllAtomics(RpClump* c,RpAtomicCallBack cb,void* data){
    Push(41,Id(c),Id(data)); const auto ci=Id(c);
    if(ci>>24==27&&(ci&0xFFF)==0) for(int8 a:clumpAtoms[(ci>>12)&0xFFF]){ if(a<0)break; auto* at=reinterpret_cast<RpAtomic*>(atoms[a].data()); auto* r=cb(at,data); Push(42,Id(at),Id(r)); if(!r)break; }
    return c; }
inline bool __cdecl FakeReplaceMaterial(RpGeometry* g,RpMaterial* o,RpMaterial* n){ Push(43,Id(g),Id(o)); Push(44,Id(n)); return true; }
inline int32 __cdecl FakeFindTxdSlot(const char* n){ Push(45,uint32(Fnv(n,strlen(n)))); return txdSlot; }
inline void __cdecl FakePushTxd(){ Push(46); }
inline void __cdecl FakePopTxd(){ Push(47); }
inline void __cdecl FakeSetTxd(int32 i){ Push(48,uint32(i)); }
inline void __cdecl FakeRemoveTxd(int32 i){ Push(49,uint32(i)); }
inline void __cdecl FakeRemoveTxdSlot(int32 i){ Push(50,uint32(i)); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,28> fns{{
    {"CPedAttractorManager","BroadcastArrival-",0x5EFE20,0,-1,512},{"CPedAttractorManager","BroadcastDeparture-",0x5EC980,0,-1,512},
    {"CPedAttractorManager","DeRegisterPed-",0x5EC850,0,-1,512},{"CPedAttractorManager","GetPedAttractorManager",0x5EE190,0,-1,64},
    {"CPedAttractorManager","GetPedUsingEffect-",0x5EBE50,0,-1,512},{"CPedAttractorManager","GetRelevantAttractor-",0x5EBF50,0,-1,512},
    {"CPedAttractorManager","HasEmptySlot",0x5EBB00,0,-1,512},{"CPedAttractorManager","HasQueueTailArrivedAtSlot",0x5EBBA0,0,-1,512},
    {"CPedAttractorManager","IsApproachable",0x5EA220,0,-1,1024},{"CPedAttractorManager","IsPedRegisteredWithEffect-Effect",0x5EBD70,0,-1,512},
    {"CPedAttractorManager","IsPedRegisteredWithEffect-Ped",0x5EBCB0,0,-1,512},{"CPedAttractorManager","RegisterPedWithAttractor",0x5EFCA0,0,-1,512},
    {"CPedAttractorManager","RemoveEffect-",0x5EBA30,0,-1,512},{"CPedAttractorManager","RestoreStuffFromMem",0x5EF710,0,-1,256},
    {"CCustomCarPlateMgr","AtomicSetCarplateTextureCB",0x6FE0D0,0,-1,256},{"CCustomCarPlateMgr","AtomicUpgradeSetCarplateTextureCB",0x6FDFC0,0,-1,256},
    {"CCustomCarPlateMgr","CreatePlateTexture",0x6FDEA0,0,-1,512},{"CCustomCarPlateMgr","GeneratePlateText",0x6FD5B0,0,-1,1024},
    {"CCustomCarPlateMgr","GetMapRegionPlateDesign",0x6FD7A0,0,-1,64},{"CCustomCarPlateMgr","Initialise",0x6FD500,0,-1,128},
    {"CCustomCarPlateMgr","MaterialSetCarplateTextureCB",0x6FE060,0,-1,512},{"CCustomCarPlateMgr","MaterialUpgradeSetCarplateTextureCB",0x6FDF50,0,-1,512},
    {"CCustomCarPlateMgr","RenderLicenseplateTextToRaster",0x6FDD70,0,-1,512},{"CCustomCarPlateMgr","SetupClump",0x6FE0F0,0,-1,256},
    {"CCustomCarPlateMgr","SetupClumpAfterVehicleUpgrade",0x6FDFE0,0,-1,256},{"CCustomCarPlateMgr","SetupMaterialPlateTexture",0x6FE020,0,-1,256},
    {"CCustomCarPlateMgr","SetupMaterialPlatebackTexture",0x6FDE50,0,-1,256},{"CCustomCarPlateMgr","Shutdown",0x6FD720,0,-1,128},
}};

constexpr uintptr_t PLATE_GLOBALS=0xC3EF5C; constexpr size_t NPG=0x28;
// Orden de las listas del gestor (+0x00 asientos, +0x10 cajeros, ...) y su tipo de atractor
constexpr std::array<int32,10> listType{1,0,2,3,4,5,6,7,8,9};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"mix14-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"mix14-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("mix14");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"mix14-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,28> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,28> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    InitRanges();
    for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrapVt);
    fakeVt[0]=reinterpret_cast<void*>(&FakeGetType); fakeVt[1]=reinterpret_cast<void*>(&FakeDeletingDtor); fakeVt[5]=reinterpret_cast<void*>(&FakeBroadcastDeparture);
    { uint32 s=0xC0FFEEu; for(auto& b:charsetBuf){ s^=s<<13;s^=s>>17;s^=s<<5; b=uint8(s); } } // solo se lee

    // estado global guardado
    auto& poolPtr=*reinterpret_cast<void**>(0xB744C4); void* const savedPool=poolPtr;
    auto& mgrPtr=*reinterpret_cast<void**>(0xC09840); void* const savedMgr=mgrPtr;
    auto* pg=reinterpret_cast<uint8*>(PLATE_GLOBALS); std::array<uint8,NPG> savedPg{}; std::memcpy(savedPg.data(),pg,NPG);
    auto& region=*reinterpret_cast<int16*>(0xC81314); const int16 savedRegion=region;
    // La correccion reversible NOTSA de GeneratePlateText (A..Z / 0..9) se desactiva durante el banco: se compara el comportamiento del EXE
    auto& plateFix=const_cast<bool&>(notsa::bugfixes::CCustomCarPlateMgr_GeneratePlateText_MissingLettersAndDigits.Enabled); const bool plateFixSaved=plateFix; plateFix=false;
    auto restoreAll=[&]{ poolPtr=savedPool; mgrPtr=savedMgr; std::memcpy(pg,savedPg.data(),NPG); region=savedRegion; plateFix=plateFixSaved; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(&CPedAttractor::BroadcastArrival),FakeBroadcastArrival), s1e(0x5EEF80,FakeBroadcastArrival);
        EntryPatch s2(bit_cast<uintptr_t>(&CPedAttractor::DeRegisterPed),FakeDeRegisterPed), s2e(0x5EC5B0,FakeDeRegisterPed);
        EntryPatch s3(bit_cast<uintptr_t>(&CPedAttractor::RegisterPed),FakeRegisterPed), s3e(0x5EEE30,FakeRegisterPed);
        EntryPatch s4(bit_cast<uintptr_t>(&CPedAttractor::AbortPedTasks),FakeAbortPedTasks), s4e(0x5EAF60,FakeAbortPedTasks);
        EntryPatch s5(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),FakeFindActiveTaskByType), s5e(0x681740,FakeFindActiveTaskByType);
        EntryPatch s6(bit_cast<uintptr_t>(&CScripted2dEffects::GetIndex),FakeGetIndex), s6e(0x6F9F60,FakeGetIndex);
        EntryPatch s7(bit_cast<uintptr_t>(&CEventAttractor::IsEffectActive),FakeIsEffectActive), s7e(0x4AF460,FakeIsEffectActive);
        EntryPatch s8(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLineOfSight), s8e(0x56A490,FakeLineOfSight);
        EntryPatch s9(bit_cast<uintptr_t>(&CPedGeometryAnalyser::IsWanderPathClear),FakeWanderPathClear), s9e(0x5F2F70,FakeWanderPathClear);
        EntryPatch s10(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegisterReference), s10e(0x571B70,FakeRegisterReference);
        EntryPatch t1(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),FakeFindTxdSlot), t1e(0x731850,FakeFindTxdSlot);
        EntryPatch t2(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),FakePushTxd), t2e(0x7316A0,FakePushTxd);
        EntryPatch t3(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),FakePopTxd), t3e(0x7316B0,FakePopTxd);
        EntryPatch t4(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),FakeSetTxd), t4e(0x7319C0,FakeSetTxd);
        EntryPatch t5(bit_cast<uintptr_t>(&CTxdStore::RemoveTxd),FakeRemoveTxd), t5e(0x731E90,FakeRemoveTxd);
        EntryPatch t6(bit_cast<uintptr_t>(&CTxdStore::RemoveTxdSlot),FakeRemoveTxdSlot), t6e(0x731CD0,FakeRemoveTxdSlot);
        EntryPatch w1(0x7F3AC0,FakeTextureRead), w2(0x7FB2D0,FakeRasterLock), w3(0x7FAEC0,FakeRasterUnlock), w4(0x7F3820,FakeTextureDestroy);
        EntryPatch w5(0x7FB230,FakeRasterCreate), w6(0x7FB020,FakeRasterDestroy), w7(0x7F37C0,FakeTextureCreate), w8(0x7F38A0,FakeTextureSetName);
        EntryPatch w9(0x74DBC0,FakeMaterialSetTexture), w10(0x74C790,FakeForAllMaterials), w11(0x749B70,FakeForAllAtomics), w12(0x734DE0,FakeReplaceMaterial);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x14A7u; if(!stt)stt=1; seed=uint32(i)*7u+uint32(f); rngCalls=0; nrec=0;
                    arrMask=next(); emptyMask=next(); retMask=next(); taskMask=next(); losMask=next(); wanderVals=next(); activeMask=next();
                    arrCalls=emptyCalls=retCalls=taskCalls=losCalls=wanderCalls=activeCalls=0; indexValue=next()%3 ? -1 : int32(next()%4);
                    lockMask=next()|(next()%3 ? 1u : 0u); rasterMask=next()|(next()%4 ? 1u : 0u); texMask=next()|(next()%4 ? 1u : 0u); lockCalls=rasterCalls=texCalls=readCalls=0;
                    txdSlot=next()%4 ? int32(next()%5000) : -1;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CPedAttractorManager") {
                        // ---- fixture: pool, atractores, peatones, efectos, entidades y gestor ----
                        for(auto& b:att)b=uint8(next());
                        auto A=[&](int k)->uint8*{ return att.data()+k*ASZ; };
                        auto D=[&](void* base,size_t off)->uint32&{ return *reinterpret_cast<uint32*>(static_cast<uint8*>(base)+off); };
                        auto P=[&](const void* p){ return uint32(uintptr_t(p)); };
                        for(int k=0;k<NA;++k) poolMap[k]=uint8(next()%4==0 ? 0x80|(next()&0x7F) : next()&0x7F);
                        if(next()%10==0) for(auto& m:poolMap)m&=0x7F; // pool lleno
                        D(pool.data(),0)=P(att.data()); D(pool.data(),4)=P(poolMap.data()); D(pool.data(),8)=NA; D(pool.data(),0xC)=uint32(int32(next()%(NA+1))-1); pool[0x10]=0; pool[0x11]=0; pool[0x12]=0; pool[0x13]=0;
                        for(int k=0;k<NP;++k){ auto* p=peds[k].data(); std::memset(p,0,0x800); D(p,0x47C)=P(intel[k].data());
                            auto& m=pedMat[k]; for(auto& v:m)v=0.f; m[0]=1.f; m[5]=1.f; m[10]=1.f; m[12]=uni(-20.f,20.f); m[13]=uni(-20.f,20.f); m[14]=uni(-2.f,2.f);
                            *reinterpret_cast<float*>(p+4)=uni(-20.f,20.f); *reinterpret_cast<float*>(p+8)=uni(-20.f,20.f); *reinterpret_cast<float*>(p+12)=uni(-2.f,2.f);
                            D(p,0x14)=next()%2 ? P(m.data()) : 0; }
                        for(int k=0;k<4;++k){ for(auto& b:tasks[k])b=uint8(next()); D(tasks[k].data(),0x10)=uint32(int32(next()%6)-1); }
                        for(int k=0;k<NFX;++k){ auto* x=fxs[k].data(); for(int o=0;o<0x38;++o)x[o]=uint8(next());
                            float* fl=reinterpret_cast<float*>(x); fl[0]=uni(-10.f,10.f); fl[1]=uni(-10.f,10.f); fl[2]=uni(-2.f,2.f);
                            for(int o=4;o<13;++o)fl[o]=uni(-1.f,1.f);
                            x[0xC]=uint8(next()%6==0 ? next()%10 : 3); x[0x34]=uint8(next()%8==0 ? (next()%2 ? 10+next()%246 : 0xFF) : next()%10);
                            std::memset(x+0x38,0,8); x[0x38]='S'; x[0x39]='C'; x[0x3A]='R'; x[0x3B]=char('0'+k); }
                        for(int k=0;k<NE;++k){ auto* e=ents[k].data(); std::memset(e,0,0x200); auto& m=entMat[k];
                            for(auto& v:m)v=0.f; m[0]=1.f; m[5]=1.f; m[10]=1.f; m[12]=uni(-50.f,50.f); m[13]=uni(-50.f,50.f); m[14]=uni(-5.f,5.f); D(e,0x14)=P(m.data()); }
                        auto pickPed=[&]{ return next()%10==0 ? nullptr : peds[next()%NP].data(); };
                        auto pickFx=[&]()->uint8*{ return fxs[next()%NFX].data(); };
                        auto pickEnt=[&]()->uint8*{ return next()%3==0 ? nullptr : ents[next()%NE].data(); };
                        auto fillArr=[&](uint8* base,size_t off,void** store,uint32 maxN){ D(base,off-4)=next();
                            if(next()%5==0){ D(base,off)=D(base,off+4)=D(base,off+8)=0; return; }
                            const uint32 n=next()%(maxN+1), cap=std::min<uint32>(8,n+next()%3); for(uint32 e=0;e<8;++e)store[e]=e<n ? static_cast<void*>(pickPed()) : reinterpret_cast<void*>(uintptr_t(next()));
                            if(!cap&&next()%2){ D(base,off)=D(base,off+4)=D(base,off+8)=0; return; }
                            D(base,off)=P(store); D(base,off+4)=P(store+n); D(base,off+8)=P(store+cap); };
                        for(int k=0;k<NA;++k){ auto* o=A(k); D(o,0)=P(fakeVt.data()); D(o,4)=next()%12==0 ? next() : P(pickFx()); D(o,8)=P(pickEnt());
                            fillArr(o,0x10,pedArr[k][0].data(),3); fillArr(o,0x20,pedArr[k][1].data(),3); D(o,0x3C)=next()%6;
                            D(o,0xC0)=uint32(next()%10==0 ? (next()%2 ? 10+int32(next()%2) : -1) : int32(next()%10)); }
                        for(int L=0;L<10;++L){ auto* base=mgr.data()+L*0x10; D(base,0)=next(); auto* st=mgrStore[L].data();
                            for(auto& v:mgrStore[L])v=reinterpret_cast<void*>(uintptr_t(next()));
                            if(next()%6==0){ D(base,4)=D(base,8)=D(base,0xC)=0; continue; }
                            const uint32 n=next()%5, cap=std::min<uint32>(16,n+next()%3);
                            for(uint32 e=0;e<n;++e){ const int k=int(next()%NA); st[e]=A(k); if(next()%8)D(A(k),0xC0)=uint32(listType[L]); }
                            if(!cap&&next()%2){ D(base,4)=D(base,8)=D(base,0xC)=0; continue; }
                            D(base,4)=P(st); D(base,8)=P(st+n); D(base,0xC)=P(st+cap); }
                        for(size_t o=0;o<4096;++o)arena[o]=uint8(0xA0^o); arenaTop=0;
                        poolPtr=pool.data();
                        // objetivo: atractor presente en alguna lista (casi siempre), con su efecto/entidad/peaton
                        uint8* target=A(int(next()%NA));
                        if(next()%4){ for(int tries=0;tries<8;++tries){ const int L=int(next()%10); auto* base=mgr.data()+L*0x10; const uint32 f0=D(base,4), l0=D(base,8);
                            if(f0&&l0>f0){ target=reinterpret_cast<uint8*>(uintptr_t(reinterpret_cast<uint32*>(uintptr_t(f0))[next()%((l0-f0)/4)])); break; } } }
                        auto targetFx=[&]()->uint8*{ const uint32 v=D(target,4); return next()%4 && Id(reinterpret_cast<void*>(uintptr_t(v)))>>24==3 ? reinterpret_cast<uint8*>(uintptr_t(v)) : pickFx(); };
                        auto targetEnt=[&]()->uint8*{ return next()%4 ? reinterpret_cast<uint8*>(uintptr_t(D(target,8))) : pickEnt(); };
                        auto targetPed=[&]()->uint8*{ for(int s=0;s<2;++s){ const uint32 f0=D(target,0x10+s*0x10), l0=D(target,0x14+s*0x10); if(next()%3&&f0&&l0>f0)return reinterpret_cast<uint8*>(uintptr_t(reinterpret_cast<uint32*>(uintptr_t(f0))[next()%((l0-f0)/4)])); } return pickPed(); };
                        std::array<uint8,NA> mapBefore=poolMap;
                        void* M=mgr.data();
                        auto call=[&](auto&& fnc){ using ReversingProbe::EntryPatch;
                            EntryPatch n1(0x82119A,FakeNew), n2(0x821195,FakeNewArr), d1(0x8214BD,FakeDelete), d2(0x8213AE,FakeDeleteArr); fnc(); };

                        if(nm=="BroadcastArrival-"||nm=="BroadcastDeparture-"||nm=="DeRegisterPed-"){
                            void* ped=targetPed(); void* at=next()%20==0 ? nullptr : target; bool r{};
                            call([&]{ r=reinterpret_cast<bool(__thiscall*)(void*,void*,void*)>(a)(M,ped,at); }); ret=r; }
                        else if(nm=="GetPedAttractorManager"){ const uint32 mode=next()%3; mgrPtr=mode==0 ? M : nullptr; if(mode==2){ for(size_t o=0;o<4096;++o)arena[o]=uint8(next()); }
                            void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)()>(a)(); }); ret=Map(P(r)); out["global"]=Map(P(mgrPtr)); out["new"]=ReversingProbe::Bytes(arena.data(),0xB0); mgrPtr=savedMgr; }
                        else if(nm=="GetPedUsingEffect-"){ void* fx=targetFx(); void* ent=targetEnt(); void* r{};
                            call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*,void*,void*)>(a)(M,fx,ent); }); ret=Map(P(r)); }
                        else if(nm=="GetRelevantAttractor-"||nm=="IsPedRegisteredWithEffect-Effect"){
                            // a veces se duplica el objetivo en otra lista (mismo efecto, entidad y peaton) para probar el orden de busqueda
                            if(next()%3==0){ const int L=int(next()%10); auto* base=mgr.data()+L*0x10; if(D(base,4)&&D(base,8)>D(base,4)){ auto* other=reinterpret_cast<uint8*>(uintptr_t(*reinterpret_cast<uint32*>(uintptr_t(D(base,4))))); if(other!=target){ D(other,4)=D(target,4); D(other,8)=D(target,8); std::memcpy(pedArr[(other-att.data())/ASZ][1].data(),pedArr[(target-att.data())/ASZ][1].data(),sizeof(void*)*8); std::memcpy(pedArr[(other-att.data())/ASZ][0].data(),pedArr[(target-att.data())/ASZ][0].data(),sizeof(void*)*8); } } }
                            void* ped=targetPed(); void* fx=targetFx(); void* ent=targetEnt();
                            if(nm=="GetRelevantAttractor-"){ void* r{}; call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*,void*,void*,void*)>(a)(M,ped,fx,ent); }); ret=Map(P(r)); }
                            else { bool r{}; call([&]{ r=reinterpret_cast<bool(__thiscall*)(void*,void*,void*,void*)>(a)(M,ped,fx,ent); }); ret=r; } }
                        else if(nm=="HasEmptySlot"||nm=="HasQueueTailArrivedAtSlot"){ void* fx=next()%16==0 ? nullptr : targetFx(); void* ent=targetEnt();
                            if(nm=="HasQueueTailArrivedAtSlot"){ const uint32 f0=D(target,0x20), l0=D(target,0x24); if(f0&&l0>f0&&next()%2){ const int32 sz=int32((l0-f0)/4); for(auto& t:tasks)D(t.data(),0x10)=uint32(next()%3 ? sz-1 : int32(next()%6)-1); } }
                            bool r{}; call([&]{ r=reinterpret_cast<bool(__thiscall*)(void*,void*,void*)>(a)(M,fx,ent); }); ret=r; }
                        else if(nm=="IsApproachable"){ auto* fx=pickFx(); if(next()%4==0)fx[0x34]=4;
                            auto& m=argMat[0]; const float h=uni(-3.2f,3.2f), c=std::cos(h), s=std::sin(h); for(auto& v:m)v=0.f;
                            m[0]=c; m[1]=s; m[4]=-s; m[5]=c; m[10]=1.f; if(next()%4==0){ m[2]=uni(-0.3f,0.3f); m[8]=uni(-0.3f,0.3f); }
                            m[12]=uni(-10.f,10.f); m[13]=uni(-10.f,10.f); m[14]=uni(-1.f,1.f);
                            auto* ped=peds[next()%NP].data(); float* pp=D(ped,0x14) ? reinterpret_cast<float*>(uintptr_t(D(ped,0x14)))+12 : reinterpret_cast<float*>(ped+4);
                            const float* fp=reinterpret_cast<const float*>(fx); pp[0]=m[12]+fp[0]+uni(-4.f,4.f); pp[1]=m[13]+fp[1]+uni(-4.f,4.f); pp[2]=m[14]+fp[2]+uni(-1.f,1.f);
                            if(next()%10==0){ pp[0]=m[12]+fp[0]*c-fp[1]*s; pp[1]=m[13]+fp[0]*s+fp[1]*c; }
                            const int32 slot=int32(next()); bool r{};
                            call([&]{ r=reinterpret_cast<bool(__cdecl*)(void*,const void*,int32,void*)>(a)(fx,m.data(),slot,ped); }); ret=r; }
                        else if(nm=="IsPedRegisteredWithEffect-Ped"){ void* ped=targetPed(); bool r{};
                            call([&]{ r=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(M,ped); }); ret=r; }
                        else if(nm=="RegisterPedWithAttractor"){ void* ped=targetPed(); uint8* fx=next()%3 ? targetFx() : pickFx(); void* ent=next()%3 ? targetEnt() : pickEnt();
                            if(next()%3==0){ const uint8 t=fx[0x34]; fx[0x34]=uint8(next()%10); if(next()%2)fx[0x34]=t; }
                            const int32 ms=int32(next()%8); void* r{};
                            call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*,void*,void*,void*,int32)>(a)(M,ped,fx,ent,ms); }); ret=Map(P(r)); }
                        else if(nm=="RemoveEffect-"){ void* fx=targetFx(); bool r{};
                            call([&]{ r=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(M,fx); }); ret=r; }
                        else if(nm=="RestoreStuffFromMem"){ call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(M); }); }

                        // ---- estado resultante ----
                        out["mgr"]=Dw(mgr.data(),mgr.size()); out["store"]=Dw(mgrStore.data(),sizeof(mgrStore)); out["peds"]=Dw(pedArr.data(),sizeof(pedArr));
                        out["pool"]=Dw(pool.data(),0x10); out["map"]=ReversingProbe::Bytes(poolMap.data(),NA); out["arena"]=Dw(arena.data(),1024);
                        json objs=json::array();
                        for(int k=0;k<NA;++k){ auto* o=A(k);
                            if((mapBefore[k]&0x80)&&!(poolMap[k]&0x80)){ // atractor nuevo (constructor real): tipo, efecto, entidad, listas, maximo, modo y script
                                const int8 type=int8(reinterpret_cast<int32(__thiscall*)(void*)>((*reinterpret_cast<void***>(o))[0])(o));
                                objs.push_back({"new",type,Map(D(o,4)),Map(D(o,8)),D(o,0x10),D(o,0x14),D(o,0x18),D(o,0x20),D(o,0x24),D(o,0x28),D(o,0x30),D(o,0x34),D(o,0x38),D(o,0x3C),D(o,0x80),std::string(reinterpret_cast<char*>(o+0x84),strnlen(reinterpret_cast<char*>(o+0x84),8))}); }
                            else objs.push_back({Dw(o,0x40),Map(D(o,0xC0))}); }
                        out["att"]=objs;
                        poolPtr=savedPool; mgrPtr=savedMgr;
                    }
                    else { // CCustomCarPlateMgr
                        auto D=[&](void* base,size_t off)->uint32&{ return *reinterpret_cast<uint32*>(static_cast<uint8*>(base)+off); };
                        auto P=[&](const void* p){ return uint32(uintptr_t(p)); };
                        static const char* names[]{"carplate","CARPLATE","CarPlate","carpback","CARPBACK","carpback2","careplate","plateback1","platecharset","","carplat","vehiclelights128"};
                        for(auto& t:texs){ for(auto& b:t)b=uint8(next()); std::memset(t.data()+0x10,0,0x20); const char* n=names[next()%std::size(names)]; std::memcpy(t.data()+0x10,n,strlen(n)); }
                        for(auto& r:rasts)for(auto& b:r)b=uint8(next());
                        D(texs[0].data(),0)=next()%8 ? P(rasts[0].data()) : 0;
                        D(rasts[0].data(),0x18)=next()%10==0 ? 0 : (next()%4 ? 128 : 1+next()%200);
                        D(rasts[1].data(),0x18)=next()%10==0 ? 0 : (next()%4 ? 256 : 32+next()%269);
                        for(auto& m:mats){ for(auto& b:m)b=uint8(next()); D(m.data(),0)=next()%6==0 ? 0 : P(texs[1+next()%4].data()); }
                        for(auto& g:geoms)for(auto& b:g)b=uint8(next());
                        for(auto& gm:geomMats){ const uint32 n=next()%5; for(uint32 e=0;e<5;++e)gm[e]=e<n ? int8(next()%8) : int8(-1); }
                        for(auto& at:atoms){ for(auto& b:at)b=uint8(next()); D(at.data(),0x18)=next()%8==0 ? 0 : P(geoms[next()%4].data()); }
                        for(auto& c:clumps)for(auto& b:c)b=uint8(next());
                        for(auto& ca:clumpAtoms){ const uint32 n=next()%5; for(uint32 e=0;e<5;++e)ca[e]=e<n ? int8(next()%6) : int8(-1); }
                        static const char chars[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 -.!?@[`{";
                        for(int k=0;k<8;++k)plateText[k]=next()%12==0 ? char(next()) : chars[next()%(sizeof(chars)-1)]; for(int k=8;k<16;++k)plateText[k]=0;
                        for(auto& b:outBuf)b=char(0x5A); for(auto& b:dataBuf)b=uint8(next());
                        std::memset(plateBuf.data(),0xEE,plateBuf.size());
                        auto pickType=[&]{ static const uint8 t[]{0,1,2,255}; return t[next()%4]; };
                        // globales
                        D(pg,0x00)=nm=="Shutdown"&&next()%4==0 ? 0 : P(texs[0].data()); // el EXE lee pCharsetTex->raster sin comprobar (salvo Shutdown)
                        for(int k=0;k<3;++k)D(pg,0x04+k*4)=next()%6==0 ? 0 : P(texs[1+k].data());
                        for(int k=0;k<3;++k)D(pg,0x10+k*4)=0x7000+k*0x10+(next()&0xF);
                        D(pg,0x1C)=next()%8==0 ? 0 : P(charsetBuf.data());
                        D(pg,0x20)=next()%3==0 ? 0 : P(mats[next()%8].data());
                        pg[0x24]=pickType(); pg[0x25]=uint8(next()); pg[0x26]=uint8(next()); pg[0x27]=uint8(next());
                        region=int16(next()%6==0 ? int16(next()) : int16(int32(next()%8)-1));

                        if(nm=="Initialise"){ ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                        else if(nm=="Shutdown"){ reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="GetMapRegionPlateDesign"){ ret=int(reinterpret_cast<int8(__cdecl*)()>(a)()); }
                        else if(nm=="GeneratePlateText"){ int32 len; switch(next()%6){case 0:len=int32(next()%4);break;case 1:len=-int32(1+next()%5);break;case 2:len=int32(0x80000000u|next());break;default:len=int32(4+next()%45);}
                            ret=reinterpret_cast<bool(__cdecl*)(char*,int32)>(a)(outBuf.data(),len); out["text"]=ReversingProbe::Bytes(outBuf.data(),outBuf.size()); }
                        else if(nm=="SetupMaterialPlatebackTexture"){ reinterpret_cast<void(__cdecl*)(void*,uint8)>(a)(mats[next()%8].data(),pickType()); }
                        else if(nm=="CreatePlateTexture"){ auto* r=reinterpret_cast<void*(__cdecl*)(const char*,uint8)>(a)(plateText.data(),pickType()); ret=Map(P(r)); }
                        else if(nm=="RenderLicenseplateTextToRaster"){ void* cr=next()%2 ? rasts[0].data() : rasts[2].data(); D(rasts[2].data(),0x18)=next()%8==0 ? 0 : 128;
                            ret=reinterpret_cast<bool(__cdecl*)(const char*,void*,void*,void*)>(a)(plateText.data(),cr,reinterpret_cast<void*>(uintptr_t(0x7123)),rasts[1].data()); }
                        else if(nm=="MaterialUpgradeSetCarplateTextureCB"){ auto* r=reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(mats[next()%8].data(),geoms[next()%4].data()); ret=Map(P(r)); }
                        else if(nm=="MaterialSetCarplateTextureCB"){ auto* r=reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(mats[next()%8].data(),plateText.data()); ret=Map(P(r)); }
                        else if(nm=="AtomicUpgradeSetCarplateTextureCB"){ auto* r=reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atoms[next()%6].data(),dataBuf.data()); ret=Map(P(r)); }
                        else if(nm=="AtomicSetCarplateTextureCB"){ auto* r=reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atoms[next()%6].data(),plateText.data()); ret=Map(P(r)); }
                        else if(nm=="SetupClumpAfterVehicleUpgrade"){ void* m=next()%5==0 ? nullptr : mats[next()%8].data();
                            ret=int(reinterpret_cast<int8(__cdecl*)(void*,void*,uint8)>(a)(clumps[next()%2].data(),m,pickType())); }
                        else if(nm=="SetupMaterialPlateTexture"){ void* m=next()%6==0 ? nullptr : mats[next()%8].data();
                            reinterpret_cast<void(__cdecl*)(void*,const char*,uint8)>(a)(m,plateText.data(),pickType()); } // la DLL es void (los llamadores del EXE ignoran EAX)
                        else if(nm=="SetupClump"){ auto* r=reinterpret_cast<void*(__cdecl*)(void*,char*,uint8)>(a)(clumps[next()%2].data(),plateText.data(),pickType()); ret=Map(P(r)); }

                        out["globals"]=Dw(pg,NPG-4); out["type"]=pg[0x24]; out["region"]=region;
                        json th=json::array(); for(auto& t:texs)th.push_back(std::to_string(Fnv(t.data(),t.size()))); out["texs"]=th;
                        out["plate"]=std::to_string(Fnv(plateBuf.data(),plateBuf.size()));
                        std::memcpy(pg,savedPg.data(),NPG); region=savedRegion;
                    }
                    out["ret"]=ret; out["recs"]=Recs(); return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["recs"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        restoreAll();
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restoreAll();
    const bool restored=poolPtr==savedPool&&mgrPtr==savedMgr&&std::memcmp(pg,savedPg.data(),NPG)==0&&region==savedRegion&&plateFix==plateFixSaved;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
