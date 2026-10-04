#pragma once
// Banco dentro del motor: coberturas (CCover, 16 funciones) y CPlaceable (16 funciones) de la lista de 16.
// Original (hooks del lote desactivados) frente a candidato con fixtures deterministas por caso.
// Se simulan y trazan: FindPlayerCoors, FindPlayerVehicle, CObject::CanBeUsedToTakeCoverBehind,
// CBaseModelInfo::Get2dEffect y operator new/delete (solo durante InitMatrixArray/ShutdownMatrixArray, sin reservar).
// Estado global que se toca y se restaura en cada ejecucion: la zona de CCover (contador, 100 puntos y lista de
// edificios procesados), CReferences::aRefs + pEmptyList, gMatrixList (0x1FC bytes; las listas falsas viven dentro
// de sus centinelas), los punteros de los pools de vehiculos y objetos (se sustituyen por pools FALSOS), el pool REAL de
// nodos dobles (cabecera, estados y almacenamiento: la lista de procesados usa nodos pedidos a ese pool porque la DLL
// valida cada nodo contra el), el puntero del pool de edificios (se sustituye por uno FALSO que cubre las entidades
// falsas, porque la DLL tambien valida el Item de cada nodo contra PoolOf<CBuilding>),
// un bloque de 8x8 sectores del mundo, el codigo de busqueda, el contador de fotogramas, el modo de replay y la
// entrada 1 de ms_modelInfoPtrs (model info falsa con colision falsa).
// Fuera de los fixtures (desviaciones FIX_BUGS/NOTSA o lecturas sin inicializar del EXE):
// - AddCoverPoint: con FIX_BUGS la DLL solo mira la distancia de los puntos de tipo 3; los puntos de otro tipo
//   (y los libres, que AddCoverPoint puede reutilizar sin escribir la posicion) se colocan lejos (> 2 unidades).
// - AddCoverPoint sin posicion ni entidad (el EXE usa una posicion sin inicializar) y tipo 3 sin posicion (se cae).
// - CanAccommodateAnotherPed (FIX_BUGS) para vehiculos: el segundo peaton de los puntos de vehiculo es siempre nulo.
// - Update con FIX_BUGS registra una referencia al edificio en el nodo de la lista de procesados: las listas de
//   referencias se comparan sin las entradas que apuntan al pool de nodos y el campo m_pReferences no entra en el hash.
// - Tipos de punto fuera de 0..3 en FindAndReserveCoverPoint (el EXE usaria una posicion sin inicializar).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cover.h"
#include "MatrixLinkList.h"
#include "References.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "../ProbeRuntime.hpp"

namespace World16Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// hash de un bloque de entidades sin el campo m_pReferences (+0x24) de cada una
inline std::string HashEnts(const void* p,size_t n,size_t stride) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){const size_t o=k%stride; h^=(o>=0x24&&o<0x28) ? 0 : static_cast<const uint8*>(p)[k]; h*=1099511628211ull;} return std::format("{:016x}",h); }
inline std::string Hex(const uint8* p,size_t n) { std::string s; for(size_t k=n;k-->0;)s+=std::format("{:02x}",p[k]); return s; }

// --- direcciones del EXE ---
constexpr uintptr_t COVER_BEGIN=0xC197A4, COVER_END=0xC1A2BC, POINTS=0xC197C8, LIST_HEAD=0xC1A2B8;
constexpr uintptr_t MATRIX_LIST=0xB74288; constexpr size_t MATRIX_LIST_SIZE=0x1FC;
constexpr uintptr_t MI_PTRS=0xA9B0C8, VEH_POOL=0xB74494, BLD_POOL=0xB74498, OBJ_POOL=0xB7449C, NODE_POOL=0xB74488, SECTORS=0xB7D0B8, SCAN_CODE=0xB7CD78, FRAME=0xB7CB4C, REPLAY_MODE=0xA43088;
constexpr int16 MI_FAKE=1;
constexpr size_t E_POS=0x04, E_HEADING=0x10, E_MATRIX=0x14, E_FLAGS=0x1C, E_MODEL=0x22, E_REFS=0x24, E_SCAN=0x2C, E_IPL=0x2E, E_TYPE=0x36, E_SPEED=0x44;
constexpr size_t V_FLAGS=0x42E, V_TYPE=0x590, O_COVER=0x140, PT=0x1C;
constexpr int NENT=12, NVEH=4, NOBJ=6, NSECNODE=40, NFX=6, NLINK=8, NPL=8, NMAT=4, SEC_LO=56, SEC_HI=63;
constexpr size_t ENT_SIZE=0xA18, VEH_STRIDE=0xA18, OBJ_STRIDE=0x19C, ARENA_ELEMS=900;
constexpr uint32 QNAN=0x7FC00000u;

// --- memoria falsa (direcciones fijas: iguales en las dos ejecuciones) ---
alignas(16) inline std::array<std::array<uint8,ENT_SIZE>,NENT> ents{};            // 0..3 edificios, 4..5 objetos, 6..7 vehiculos, 8..10 peatones
alignas(16) inline std::array<std::array<uint8,0x48>,NENT+NVEH+NOBJ> mats{};      // matrices de las entidades
alignas(16) inline std::array<uint8,NVEH*VEH_STRIDE> vehStore{};
alignas(16) inline std::array<uint8,NOBJ*OBJ_STRIDE> objStore{};
inline uintptr_t nodeBase{}; inline size_t nodeBytes{};   // almacenamiento del pool REAL de nodos dobles
inline std::array<uint8,8> vehStates{}, objStates{}; 
alignas(16) inline std::array<uint8,0x14> vehHdr{}, objHdr{}, bldHdr{};
// pool de edificios FALSO que cubre ents: la DLL (debug) valida el Item de cada nodo de la lista de procesados contra
// PoolOf<CBuilding> (GetBuildingPool); el indice es (ptr - ents)/sizeof(CBuilding) y todos los huecos estan ocupados
constexpr size_t BLD_STRIDE=0x38, BLD_CAP=NENT*ENT_SIZE/BLD_STRIDE;
inline std::array<uint8,BLD_CAP> bldStates{};
alignas(16) inline std::array<uint32,NSECNODE*3> secNodes{};
alignas(16) inline std::array<uint8,0x60> miBuf{}; alignas(16) inline std::array<uint8,0x30> colBuf{};
alignas(16) inline std::array<std::array<uint8,0x40>,NFX> fxBuf{};
alignas(16) inline std::array<uint8,0x20> ptBuf{};
alignas(16) inline std::array<std::array<uint8,0x54>,NLINK> links{};
alignas(16) inline std::array<std::array<uint8,0x20>,NPL> pls{};
alignas(16) inline std::array<std::array<uint8,0x48>,NMAT> pmats{};
alignas(16) inline std::array<uint8,4+0x54*ARENA_ELEMS> arena{};

inline std::string Name(const void* p) {
    const auto u=uintptr_t(p); if(!u)return "null";
    struct R { uintptr_t base; size_t size, stride; const char* tag; };
    const R regions[]{
        {POINTS,100*PT,PT,"pt"},{uintptr_t(ptBuf.data()),ptBuf.size(),ptBuf.size(),"pt1"},{uintptr_t(ents.data()),sizeof(ents),ENT_SIZE,"ent"},
        {uintptr_t(vehStore.data()),vehStore.size(),VEH_STRIDE,"veh"},{uintptr_t(objStore.data()),objStore.size(),OBJ_STRIDE,"obj"},
        {nodeBase,nodeBytes,12,"node"},{uintptr_t(secNodes.data()),sizeof(secNodes),12,"sec"},
        {uintptr_t(links.data()),sizeof(links),0x54,"ln"},{uintptr_t(pls.data()),sizeof(pls),0x20,"pl"},{uintptr_t(mats.data()),sizeof(mats),0x48,"mat"},
        {uintptr_t(pmats.data()),sizeof(pmats),0x48,"pmat"},{MATRIX_LIST,MATRIX_LIST_SIZE,MATRIX_LIST_SIZE,"ml"},{uintptr_t(fxBuf.data()),sizeof(fxBuf),0x40,"fx"},{uintptr_t(miBuf.data()),miBuf.size(),miBuf.size(),"mi"},
        {LIST_HEAD,4,4,"listHead"}};
    for(auto& r:regions)if(u>=r.base&&u<r.base+r.size)return std::format("{}#{}+{:x}",r.tag,(u-r.base)/r.stride,(u-r.base)%r.stride);
    return std::format("{:08x}",u);
}

// --- registro sin reservas (operator new/delete sustituidos) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,64> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0) { if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs() { json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }
inline void* __cdecl FakeNew(size_t n) { Push(1,uint32(n)); return n<=arena.size() ? arena.data() : nullptr; }
inline void __cdecl FakeDelete(void* p) { Push(2,uint32(intptr_t(p)-intptr_t(arena.data()))); }

// --- dependencias simuladas ---
inline CVector playerPos{}; inline void* fakeVehResult{};
inline CVector* __cdecl FakeCoors(CVector* out,int32 id) { trace.push_back({"coors",id}); *out=playerPos; return out; }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote) { trace.push_back({"find_player_vehicle",id,remote}); return static_cast<CVehicle*>(fakeVehResult); }
inline bool __fastcall FakeCanCover(void* obj,void*) { trace.push_back({"can_cover",Name(obj)}); return (At<uint8>(obj,O_COVER)&1)!=0; }
inline void* __fastcall FakeGet2dEffect(void* mi,void*,int32 idx) { trace.push_back({"fx",Name(mi),idx}); return fxBuf[uint32(idx)%NFX].data(); }

// --- llamadas que devuelven un float en ST0 (se guarda el valor de 80 bits) ---
inline std::string CallThisF(uintptr_t fnAddr,void* self) {
    uint8 raw[10]{}; uint16 swv{};
    __asm {
        mov ecx, self
        mov eax, fnAddr
        call eax
        fstp tbyte ptr raw
        fnstsw word ptr swv
    }
    return Hex(raw,10)+std::format("/{}",(swv>>11)&7);
}
inline std::string CallSizeF(uintptr_t fnAddr,void* tri,void* verts) {
    uint8 raw[10]{}; uint16 swv{};
    __asm {
        push verts
        push tri
        mov eax, fnAddr
        call eax
        add esp, 8
        fstp tbyte ptr raw
        fnstsw word ptr swv
    }
    return Hex(raw,10)+std::format("/{}",(swv>>11)&7);
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=32;
inline const std::array<Fn,NFNS> fns{{
    {"CCover","Init",0x698710,0,-1,128},{"CCover","RemoveCoverPointIfEntityLost",0x698DB0,0,-1,512},
    {"CCover","RemoveCoverPointsForThisEntity",0x698740,0,-1,512},{"CCover","ShouldThisBuildingHaveItsCoverPointsCreated",0x699230,0,-1,512},
    {"CCover","Update",0x6997E0,0,-1,512},{"CCover","AddCoverPoint",0x698F30,0,-1,1024},
    {"CCover","CalculateHorizontalSize",0x6987F0,0,-1,1024},{"CCover","DoLineCheckWithinObject",0x698990,0,-1,1024},
    {"CCover","DoesCoverPointStillProvideCover",0x698DD0,0,-1,1024},{"CCover","Find2HighestPoints",0x6988E0,0,-1,1024},
    {"CCover","FindAndReserveCoverPoint",0x6992B0,0,-1,1024},{"CCover","FindCoordinatesCoverPoint",0x699570,0,-1,1024},
    {"CCover","FindCoverPointsForThisBuilding",0x699120,0,-1,512},{"CCover","FindDirFromVector",0x698D40,0,-1,512},
    {"CCover","FindVectorFromDir",0x698D60,0,-1,512},{"CCover","FindVectorFromFirstToMissingVertex",0x698790,0,-1,1024},
    {"CPlaceable","SetPosn-xyz",0x420B80,0,-1,256},{"CPlaceable","SetPosn-vector",0x4241C0,0,-1,256},
    {"CPlaceable","SetOrientation-xyz",0x439A80,0,-1,512},{"CPlaceable","SetHeading",0x43E0C0,0,-1,512},
    {"CPlaceable","GetHeading",0x441DB0,0,-1,1024},{"CPlaceable","GetRoll",0x420B30,0,-1,1024},
    {"CPlaceable","IsWithinArea-xy",0x54F200,0,-1,512},{"CPlaceable","IsWithinArea-xyz",0x54F2B0,0,-1,512},
    {"CPlaceable","RemoveMatrix",0x54F3B0,0,-1,512},{"CPlaceable","AllocateStaticMatrix",0x54F4C0,0,-1,512},
    {"CPlaceable","AllocateMatrix",0x54F560,0,-1,512},{"CPlaceable","SetMatrix",0x54F610,0,-1,512},
    {"CPlaceable","GetMatrix",0x411990,0,-1,512},{"CPlaceable","ShutdownMatrixArray",0x54EFD0,0,-1,64},
    {"CPlaceable","InitMatrixArray",0x54F3A0,0,-1,32},{"CPlaceable","FreeStaticMatrix",0x54F010,0,-1,512},
}};

inline json RefList(void* e) {
    json l=json::array(); int n=0;
    for(auto* r=At<CReference*>(e,E_REFS);r&&n<64;r=r->m_pNext,++n) {
        const auto u=uintptr_t(r->m_ppEntity);
        if(u>=nodeBase&&u<nodeBase+nodeBytes)continue; // FIX_BUGS de Update (ver cabecera)
        l.push_back(Name(r->m_ppEntity));
    }
    return l;
}
inline std::vector<void*> AllEnts() {
    std::vector<void*> v; for(auto& e:ents)v.push_back(e.data());
    for(int k=0;k<NVEH;++k)v.push_back(vehStore.data()+k*VEH_STRIDE); for(int k=0;k<NOBJ;++k)v.push_back(objStore.data()+k*OBJ_STRIDE);
    return v;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"world16-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"world16-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("world16");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"world16-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // referencias libres suficientes (RegisterReference no debe podar los pools reales)
    { int freeRefs=0; for(auto* r=CReferences::pEmptyList;r&&freeRefs<1000;r=r->m_pNext)++freeRefs;
      if(freeRefs<600){Write({{"event","error"},{"reason","not enough free references"},{"free",freeRefs}});return;} }

    // estado global guardado
    auto* const coverMem=reinterpret_cast<uint8*>(COVER_BEGIN); const std::vector<uint8> savedCover(coverMem,coverMem+(COVER_END-COVER_BEGIN));
    const std::vector<uint8> savedRefs(reinterpret_cast<uint8*>(&CReferences::aRefs[0]),reinterpret_cast<uint8*>(&CReferences::aRefs[0])+sizeof(CReferences::aRefs));
    CReference* const savedEmpty=CReferences::pEmptyList;
    auto* const mlMem=reinterpret_cast<uint8*>(MATRIX_LIST); const std::vector<uint8> savedMl(mlMem,mlMem+MATRIX_LIST_SIZE);
    auto& vehPool=*reinterpret_cast<void**>(VEH_POOL); auto& objPool=*reinterpret_cast<void**>(OBJ_POOL); auto& nodePool=*reinterpret_cast<void**>(NODE_POOL);
    void* const savedVehPool=vehPool; void* const savedObjPool=objPool; void* const savedNodePool=nodePool;
    auto& bldPool=*reinterpret_cast<void**>(BLD_POOL); void* const savedBldPool=bldPool;
    std::memset(bldHdr.data(),0,bldHdr.size()); At<void*>(bldHdr.data(),0)=ents.data(); At<void*>(bldHdr.data(),4)=bldStates.data();
    At<uint32>(bldHdr.data(),8)=uint32(BLD_CAP); At<int32>(bldHdr.data(),0xC)=-1; bldStates.fill(0); // 0 = ocupado
    // pool REAL de nodos dobles: la lista de edificios procesados usa nodos de verdad (la DLL valida cada nodo contra
    // este pool); se guardan cabecera, estados y almacenamiento y se restauran en cada ejecucion
    auto* const nodePoolObj=GetPtrNodeDoubleLinkPool(); auto* const poolHdr=reinterpret_cast<uint8*>(nodePoolObj);
    const size_t nodeCap=At<uint32>(poolHdr,8); auto* const nodeSlots=At<uint8*>(poolHdr,4); nodeBase=uintptr_t(At<uint8*>(poolHdr,0)); nodeBytes=nodeCap*12;
    const std::vector<uint8> savedPoolHdr(poolHdr,poolHdr+0x14), savedSlots(nodeSlots,nodeSlots+nodeCap),
                             savedNodes(reinterpret_cast<uint8*>(nodeBase),reinterpret_cast<uint8*>(nodeBase)+nodeBytes);
    auto& scanCode=*reinterpret_cast<uint16*>(SCAN_CODE); const uint16 savedScan=scanCode;
    auto& frame=*reinterpret_cast<uint32*>(FRAME); const uint32 savedFrame=frame;
    auto& replayMode=*reinterpret_cast<uint8*>(REPLAY_MODE); const uint8 savedReplay=replayMode;
    auto& miSlot=reinterpret_cast<void**>(MI_PTRS)[MI_FAKE]; void* const savedMi=miSlot;
    auto SectorRow=[](int y){ return reinterpret_cast<uint8*>(SECTORS+(size_t(y)*120+SEC_LO)*8); };
    constexpr size_t ROW=size_t(SEC_HI-SEC_LO+1)*8;
    std::vector<uint8> savedSectors; for(int y=SEC_LO;y<=SEC_HI;++y){ auto* r=SectorRow(y); savedSectors.insert(savedSectors.end(),r,r+ROW); }
    auto RestoreAll=[&]{
        std::memcpy(coverMem,savedCover.data(),savedCover.size());
        std::memcpy(&CReferences::aRefs[0],savedRefs.data(),savedRefs.size()); CReferences::pEmptyList=savedEmpty;
        std::memcpy(mlMem,savedMl.data(),savedMl.size());
        vehPool=savedVehPool; objPool=savedObjPool; nodePool=savedNodePool; bldPool=savedBldPool; scanCode=savedScan; frame=savedFrame; replayMode=savedReplay; miSlot=savedMi;
        for(int y=SEC_LO;y<=SEC_HI;++y)std::memcpy(SectorRow(y),savedSectors.data()+size_t(y-SEC_LO)*ROW,ROW);
        std::memcpy(poolHdr,savedPoolHdr.data(),savedPoolHdr.size()); std::memcpy(nodeSlots,savedSlots.data(),savedSlots.size());
        std::memcpy(reinterpret_cast<uint8*>(nodeBase),savedNodes.data(),savedNodes.size());
    };
    auto Restored=[&]{
        bool ok=std::memcmp(coverMem,savedCover.data(),savedCover.size())==0&&std::memcmp(&CReferences::aRefs[0],savedRefs.data(),savedRefs.size())==0
            &&CReferences::pEmptyList==savedEmpty&&std::memcmp(mlMem,savedMl.data(),savedMl.size())==0&&vehPool==savedVehPool&&objPool==savedObjPool
            &&nodePool==savedNodePool&&bldPool==savedBldPool&&scanCode==savedScan&&frame==savedFrame&&replayMode==savedReplay&&miSlot==savedMi;
        for(int y=SEC_LO;y<=SEC_HI;++y)ok&=std::memcmp(SectorRow(y),savedSectors.data()+size_t(y-SEC_LO)*ROW,ROW)==0;
        ok&=std::memcmp(poolHdr,savedPoolHdr.data(),savedPoolHdr.size())==0&&std::memcmp(nodeSlots,savedSlots.data(),savedSlots.size())==0
            &&std::memcmp(reinterpret_cast<uint8*>(nodeBase),savedNodes.data(),savedNodes.size())==0;
        return ok;
    };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch c1(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeCoors), c1e(0x56E010,FakeCoors);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), p2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch o1(bit_cast<uintptr_t>(&CObject::CanBeUsedToTakeCoverBehind),FakeCanCover), o1e(0x5A1B60,FakeCanCover);
        EntryPatch m1(bit_cast<uintptr_t>(&CBaseModelInfo::Get2dEffect),FakeGet2dEffect), m1e(0x4C4C70,FakeGet2dEffect);
        const CVector C{20.f,-15.f,10.f}; // centro de los fixtures de coberturas (sectores 58..61)

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x16C0u; if(!stt)stt=1; trace=json::array(); nrec=0; fakeVehResult=nullptr;
                    const uintptr_t a=fn.address;
                    json out; json ret=nullptr;

                    // ---------- piezas comunes de los fixtures ----------
                    std::array<CVector,4> hot{}; for(auto& h:hot)h=C+CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(-2.f,2.f)};
                    auto NearC=[&]{ return next()%4==0 ? hot[next()%4]+(next()%2 ? CVector{0.f,0.f,0.f} : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.5f,0.5f)})
                                                       : C+CVector{uni(-14.f,14.f),uni(-14.f,14.f),uni(-5.f,5.f)}; };
                    auto Far=[&]{ return CVector{3000.f+uni(0.f,500.f),-3000.f-uni(0.f,500.f),uni(-100.f,100.f)}; };
                    auto MakeMat=[&](uint8* m,const CVector& pos){ std::memset(m,0,0x48); auto* v=reinterpret_cast<float*>(m);
                        const float h=uni(-3.2f,3.2f), c=std::cos(h), s=std::sin(h);
                        v[0]=c; v[1]=s; v[4]=-s; v[5]=c; v[10]=1.f; v[12]=pos.x; v[13]=pos.y; v[14]=pos.z;
                        if(next()%4==0)for(int k:{0,1,2,4,5,6,8,9,10})v[k]+=uni(-0.3f,0.3f); };
                    auto InitEnt=[&](uint8* e,size_t size,int type,uint8* mat,const CVector& pos){
                        std::memset(e,0,size); At<uint32>(e,E_FLAGS)=next(); At<CVector>(e,E_POS)=pos; At<float>(e,E_HEADING)=uni(-3.2f,3.2f);
                        if(mat){ MakeMat(mat,next()%4 ? pos : pos+CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-1.f,1.f)}); At<void*>(e,E_MATRIX)=mat; }
                        At<int16>(e,E_MODEL)=MI_FAKE; At<uint16>(e,E_SCAN)=uint16(next()); At<uint8>(e,E_IPL)=uint8(next()%3 ? 0 : 1+next()%5);
                        At<uint8>(e,E_TYPE)=uint8((next()&0xF8)|uint32(type));
                        if(size>E_SPEED+12){ At<CVector>(e,E_SPEED)=next()%2 ? CVector{uni(-0.003f,0.003f),uni(-0.003f,0.003f),uni(-0.002f,0.002f)} : CVector{uni(-0.2f,0.2f),uni(-0.2f,0.2f),uni(-0.1f,0.1f)}; }
                        if(size>O_COVER)At<uint8>(e,O_COVER)=uint8(next());
                        if(size>V_TYPE+4){ At<uint8>(e,V_FLAGS)=uint8(next()); At<int32>(e,V_TYPE)=next()%4 ? 0 : int32(next()%6); } };
                    // entidades: 0..3 edificios, 4..5 objetos, 6..7 vehiculos, 8..10 peatones, 11 libre
                    auto InitEnts=[&](bool allMatrices){
                        static const int types[NENT]{1,1,1,1,4,4,2,2,3,3,3,1};
                        for(int k=0;k<NENT;++k)InitEnt(ents[k].data(),ENT_SIZE,types[k],(allMatrices||next()%3) ? mats[k].data() : nullptr,NearC());
                        // model info y colision falsas (entrada 1)
                        std::memset(miBuf.data(),0,miBuf.size()); At<void*>(miBuf.data(),0x14)=colBuf.data(); miBuf[0xD]=uint8(next()%7);
                        auto& mn=At<CVector>(colBuf.data(),0); auto& mx=At<CVector>(colBuf.data(),0xC);
                        mn=CVector{uni(-4.f,-0.3f),uni(-4.f,-0.3f),uni(-2.f,-0.1f)}; mx=CVector{uni(0.3f,4.f),uni(0.3f,4.f),uni(0.1f,3.f)}; if(next()%8==0)mx.y=mx.x;
                        At<CVector>(colBuf.data(),0x18)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; At<float>(colBuf.data(),0x24)=uni(0.f,40.f);
                        miSlot=miBuf.data(); };
                    auto EntPick=[&]()->uint8*{ return ents[next()%8].data(); };
                    auto PedPick=[&]()->uint8*{ return ents[8+next()%3].data(); };
                    auto* const pts=reinterpret_cast<uint8*>(POINTS);
                    auto& numPoints=*reinterpret_cast<int32*>(COVER_BEGIN);
                    // 100 puntos: tipo 3 cerca del centro, el resto lejos (FIX_BUGS de AddCoverPoint); el segundo peaton de
                    // los vehiculos es nulo (FIX_BUGS de CanAccommodateAnotherPed). Siempre queda al menos un hueco libre.
                    auto FillPoints=[&](uint32 pct,const std::function<uint8*()>& entPick){
                        int used=0;
                        for(int k=0;k<100;++k){ uint8* p=pts+k*PT; for(size_t b=0;b<PT;++b)p[b]=uint8(next());
                            const int type=next()%100<pct ? int(1+next()%3) : 0; At<int8>(p,0)=int8(type); At<int8>(p,1)=int8(next()%3);
                            At<CVector>(p,4)=type==3 ? NearC() : Far();
                            At<void*>(p,0x10)=next()%8==0 ? nullptr : entPick();
                            At<void*>(p,0x14)=next()%2 ? nullptr : PedPick(); At<void*>(p,0x18)=(type==2||next()%3) ? nullptr : PedPick();
                            used+=int(type!=0); }
                        const int freeIdx=int(next()%100); used-=int(At<int8>(pts+freeIdx*PT,0)!=0); At<int8>(pts+freeIdx*PT,0)=0;
                        numPoints=used;
                        for(int k=0;k<100;++k){ uint8* p=pts+k*PT; auto* e=At<CEntity*>(p,0x10);
                            if(At<int8>(p,0)&&e&&next()%2)e->RegisterReference(&At<CEntity*>(p,0x10));
                            for(size_t o:{size_t(0x14),size_t(0x18)})if(auto* pd=At<CEntity*>(p,o);pd&&next()%3==0)pd->RegisterReference(&At<CEntity*>(p,o)); } };
                    // lista de edificios procesados (0..4 nodos) con nodos pedidos al pool REAL (restaurado al final de cada ejecucion)
                    auto SetupNodePool=[&](int nodes){
                        std::array<uint32*,4> nd{}; int cnt=0;
                        for(int k=0;k<nodes&&k<4;++k){ auto* n=reinterpret_cast<uint32*>(nodePoolObj->New()); if(!n)break; nd[cnt++]=n; }
                        for(int k=0;k<cnt;++k){ nd[k][0]=uint32(uintptr_t(ents[next()%4].data())); nd[k][1]=k+1<cnt ? uint32(uintptr_t(nd[k+1])) : 0; nd[k][2]=k ? uint32(uintptr_t(nd[k-1])) : 0; }
                        At<void*>(coverMem,LIST_HEAD-COVER_BEGIN)=cnt ? nd[0] : nullptr; };
                    // efectos 2D falsos (Get2dEffect simulado): tipo 9 = punto de cobertura
                    auto FillFx=[&]{
                        for(auto& fx:fxBuf){ for(auto& c:fx)c=uint8(next()); At<CVector>(fx.data(),0)=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(-2.f,2.f)}; fx[0xC]=uint8(next()%4 ? 9 : next()%12);
                            const float h=uni(-3.2f,3.2f); At<float>(fx.data(),0x10)=next()%4 ? std::cos(h) : uni(-2.f,2.f); At<float>(fx.data(),0x14)=next()%4 ? std::sin(h) : uni(-2.f,2.f);
                            if(next()%16==0){ At<float>(fx.data(),0x10)=0.f; At<float>(fx.data(),0x14)=0.f; } fx[0x18]=uint8(next()%4 ? next()%3 : next()); } };
                    auto CoverOut=[&]{
                        out["points"]=Hash(coverMem,COVER_END-COVER_BEGIN); out["count"]=numPoints; out["head"]=Name(At<void*>(coverMem,LIST_HEAD-COVER_BEGIN));
                        out["nodes"]={Hash(poolHdr,0x14),Hash(nodeSlots,nodeCap),Hash(reinterpret_cast<const void*>(nodeBase),nodeBytes)};
                        json refs=json::array(); for(auto* e:AllEnts())refs.push_back(RefList(e)); out["refs"]=refs;
                        out["ents"]={HashEnts(ents.data(),sizeof(ents),ENT_SIZE),HashEnts(vehStore.data(),vehStore.size(),VEH_STRIDE),HashEnts(objStore.data(),objStore.size(),OBJ_STRIDE),
                                     Hash(mats.data(),sizeof(mats))}; };

                    // ---------- piezas comunes de CPlaceable ----------
                    auto RandF=[&]{ switch(next()%10){case 0:return 0.f;case 1:return -0.f;case 2:return std::bit_cast<float>(QNAN);case 3:return uni(-1e4f,1e4f);default:return uni(-3.f,3.f);} };
                    auto InitPls=[&]{ for(auto& p:pls){ for(auto& b:p)b=uint8(next()); At<uint32>(p.data(),0)=0x863C40; At<CVector>(p.data(),4)=CVector{uni(-500.f,500.f),uni(-500.f,500.f),uni(-50.f,50.f)};
                                                       At<float>(p.data(),0x10)=uni(-6.3f,6.3f); At<void*>(p.data(),0x14)=nullptr; }
                                      for(auto& m:pmats){ for(int k=0;k<16;++k)At<float>(m.data(),k*4)=RandF(); At<uint32>(m.data(),0x40)=0; At<uint32>(m.data(),0x44)=0; } };
                    // listas falsas dentro de gMatrixList: 1 = principal, 2 = estatica, 3 = libre; los duenos de 1 y 2 son placeables
                    std::array<int,NLINK> linkList{}; std::array<int,NLINK> linkOwner{};
                    auto SetupLists=[&]{
                        uint8* L=mlMem; constexpr size_t heads[3]{0,0xA8,0x150}, tails[3]{0x54,0xFC,0x1A4};
                        auto P=[&](void* base,size_t off)->uint8*&{ return *reinterpret_cast<uint8**>(static_cast<uint8*>(base)+off); };
                        for(auto& l:links){ for(auto& b:l)b=uint8(next()); for(int k=0;k<16;++k)At<float>(l.data(),k*4)=uni(-2.f,2.f); At<uint32>(l.data(),0x40)=0; At<uint32>(l.data(),0x44)=0; }
                        std::array<std::vector<int>,3> lists; for(int k=0;k<NLINK;++k)lists[next()%3].push_back(k);
                        if(lists[2].empty()&&lists[0].empty()){ lists[0].push_back(lists[1].back()); lists[1].pop_back(); }   // la mas antigua de la lista 1 debe existir
                        if(lists[0].empty()&&lists[1].empty()){ lists[0].push_back(lists[2].back()); lists[2].pop_back(); }   // al menos un placeable con matriz
                        int owner=0;
                        for(int l=0;l<3;++l){ uint8* prev=L+heads[l]; for(int k:lists[l]){ uint8* n=links[k].data(); P(prev,0x50)=n; P(n,0x4C)=prev; prev=n; linkList[k]=l;
                                if(l<2){ linkOwner[k]=owner; At<void*>(n,0x48)=pls[owner].data(); At<void*>(pls[owner].data(),0x14)=n; ++owner; }
                                else { linkOwner[k]=-1; At<void*>(n,0x48)=next()%2 ? nullptr : reinterpret_cast<void*>(uintptr_t(next())); } }
                            P(prev,0x50)=L+tails[l]; P(L+tails[l],0x4C)=prev; } };
                    auto PlOut=[&]{ out["pl"]=ReversingProbe::Bytes(pls.data(),sizeof(pls)); out["ln"]=ReversingProbe::Bytes(links.data(),sizeof(links));
                                    out["ml"]=ReversingProbe::Bytes(mlMem,MATRIX_LIST_SIZE); out["pm"]=Hash(pmats.data(),sizeof(pmats)); };
                    auto WithMatrix=[&]()->uint8*{ std::vector<int> c; for(int k=0;k<NPL;++k)if(At<void*>(pls[k].data(),0x14))c.push_back(k); return c.empty() ? nullptr : pls[c[next()%c.size()]].data(); };
                    auto WithoutMatrix=[&]()->uint8*{ std::vector<int> c; for(int k=0;k<NPL;++k)if(!At<void*>(pls[k].data(),0x14))c.push_back(k); return c.empty() ? nullptr : pls[c[next()%c.size()]].data(); };

                    // ---------- fixture por funcion (identico en las dos ejecuciones) ----------
                    std::function<void()> call;
                    if(cls=="CCover") {
                        InitEnts(nm=="Update"||nm=="FindCoverPointsForThisBuilding");
                        std::memset(vehStore.data(),0,vehStore.size()); std::memset(objStore.data(),0,objStore.size()); // solo Update usa los pools falsos
                        bldPool=bldHdr.data(); // edificios falsos validos para PoolOf<CBuilding> (restaurado en RestoreAll)
                        SetupNodePool(0); playerPos=C+CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-4.f,4.f)};
                        if(nm=="Init"){ FillPoints(next()%100,EntPick); SetupNodePool(int(next()%5)); call=[&]{ reinterpret_cast<void(__cdecl*)()>(a)(); }; }
                        else if(nm=="RemoveCoverPointIfEntityLost"){ FillPoints(50,EntPick); static const int8 types[]{0,1,2,3,1,2,3,-1,4,5,-128,127};
                            uint8* p=next()%2 ? ptBuf.data() : pts+(next()%100)*PT; for(size_t b=0;b<PT;++b)p[b]=uint8(next());
                            At<int8>(p,0)=types[next()%std::size(types)]; At<void*>(p,0x10)=next()%2 ? nullptr : EntPick(); if(next()%4==0)numPoints=int32(next());
                            call=[&,p]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(p); }; }
                        else if(nm=="RemoveCoverPointsForThisEntity"){ FillPoints(30+next()%70,EntPick); uint8* e=next()%8==0 ? nullptr : EntPick();
                            call=[&,e]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(e); }; }
                        else if(nm=="ShouldThisBuildingHaveItsCoverPointsCreated"){ FillPoints(20,EntPick); uint8* b=ents[next()%4].data();
                            if(next()%4==0)playerPos=At<void*>(b,E_MATRIX) ? At<CVector>(At<void*>(b,E_MATRIX),0x30) : At<CVector>(b,E_POS);
                            if(next()%16==0)At<float>(colBuf.data(),0x24)=std::bit_cast<float>(QNAN);
                            call=[&,b]{ ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(b); }; }
                        else if(nm=="Update"){
                            auto pick=[&]()->uint8*{ const uint32 r=next()%18; return r<8 ? ents[r].data() : r<12 ? vehStore.data()+(r-8)*VEH_STRIDE : objStore.data()+(r-12)%NOBJ*OBJ_STRIDE; };
                            // pools falsos de vehiculos y objetos
                            for(int k=0;k<NVEH;++k){ uint8* v=vehStore.data()+k*VEH_STRIDE; InitEnt(v,VEH_STRIDE,2,mats[NENT+k].data(),playerPos+CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-3.f,3.f)});
                                if(next()%2)At<uint8>(v,V_FLAGS)=uint8(At<uint8>(v,V_FLAGS)|4); vehStates[k]=uint8(next()%4 ? (next()&0x7F) : (0x80|next())); }
                            for(int k=0;k<NOBJ;++k){ uint8* o=objStore.data()+k*OBJ_STRIDE; uint8* m=mats[NENT+NVEH+k].data(); InitEnt(o,OBJ_STRIDE,4,m,playerPos+CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-3.f,3.f)});
                                static const float upz[]{1.f,1.f,0.96f,0.95f,0.94f,0.9f,0.5f}; At<float>(m,0x28)=upz[next()%std::size(upz)]; objStates[k]=uint8(next()%4 ? (next()&0x7F) : (0x80|next())); }
                            // los objetos y vehiculos de los puntos pueden estar inclinados o moverse
                            for(int k=0;k<NENT;++k)At<float>(mats[k].data(),0x28)=next()%3 ? 1.f : uni(0.8f,1.f);
                            auto hdr=[&](std::array<uint8,0x14>& h,void* storage,uint8* states,int cap){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=storage; At<void*>(h.data(),4)=states; At<int32>(h.data(),8)=cap; At<int32>(h.data(),0xC)=-1; };
                            hdr(vehHdr,vehStore.data(),vehStates.data(),NVEH); hdr(objHdr,objStore.data(),objStates.data(),NOBJ); vehPool=vehHdr.data(); objPool=objHdr.data();
                            FillPoints(next()%3==0 ? 95 : 10+next()%60,pick); SetupNodePool(int(next()%5)); FillFx();
                            // sectores 58..61 con nodos falsos de edificios
                            for(int y=SEC_LO;y<=SEC_HI;++y)std::memset(SectorRow(y),0,ROW);
                            const int nsec=int(next()%NSECNODE);
                            for(int k=0;k<nsec;++k){ const int sx=58+int(next()%4), sy=58+int(next()%4); auto& head=*reinterpret_cast<uint32*>(SECTORS+(size_t(sy)*120+sx)*8);
                                uint32* n=&secNodes[k*3]; n[0]=uint32(uintptr_t(ents[next()%4].data())); n[1]=head; n[2]=0; if(head)reinterpret_cast<uint32*>(uintptr_t(head))[2]=uint32(uintptr_t(n)); head=uint32(uintptr_t(n)); }
                            scanCode=uint16(1+next()%0xFFF0); for(int k=0;k<4;++k)if(next()%3==0)At<uint16>(ents[k].data(),E_SCAN)=uint16(scanCode+1);
                            static const uint32 mods[]{26,28,5,13,21,29,26,28,5,13};
                            frame=(next()&~31u)|(next()%3 ? mods[next()%std::size(mods)] : next()%32);
                            replayMode=uint8(next()%16==0 ? 1 : next()%16==0 ? 2 : 0);
                            fakeVehResult=next()%4==0 ? vehStore.data() : nullptr;
                            call=[&]{ reinterpret_cast<void(__cdecl*)()>(a)(); }; }
                        else if(nm=="AddCoverPoint"){ static const uint32 pcts[]{10,50,90,99}; FillPoints(pcts[next()%4],EntPick);
                            int32 type=int32(1+next()%3); if(next()%32==0)type=0;
                            const bool withPos=type==3||next()%4==0; uint8* e=(type!=3&&!withPos)||next()%4 ? EntPick() : nullptr;
                            static CVector pos; pos=NearC();
                            if(next()%3==0){ for(int t=0;t<20;++t){ uint8* p=pts+(next()%100)*PT; if(At<int8>(p,0)==3){ pos=At<CVector>(p,4)+(next()%2 ? CVector{0.f,0.f,0.f} : CVector{uni(-1.5f,1.5f),uni(-1.5f,1.5f),uni(-0.5f,0.5f)}); break; } } }
                            if(e&&next()%3==0){ auto* m=At<uint8*>(e,E_MATRIX); auto& ep=m ? At<CVector>(m,0x30) : At<CVector>(e,E_POS); for(int t=0;t<20;++t){ uint8* p=pts+(next()%100)*PT; if(At<int8>(p,0)==3){ ep=At<CVector>(p,4)+CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f}; break; } } }
                            switch(next()%16){case 0:numPoints=100;break;case 1:numPoints=99;break;case 2:numPoints=-int32(next()%1000);break;default:break;}
                            const int32 usage=int32(next()%4 ? next()%3 : next()), dirv=int32(next());
                            call=[&,type,e,withPos,usage,dirv]{ reinterpret_cast<void(__cdecl*)(int32,void*,const CVector*,int32,int32)>(a)(type,e,withPos ? &pos : nullptr,usage,dirv); }; }
                        else if(nm=="CalculateHorizontalSize"||nm=="Find2HighestPoints"||nm=="FindVectorFromFirstToMissingVertex"||nm=="DoLineCheckWithinObject"){
                            static std::array<CVector,8> verts; static std::array<std::array<uint16,4>,6> tris; static std::array<CVector,6> normals;
                            for(auto& v:verts)v=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-5.f,5.f)};
                            if(next()%4==0)verts[next()%8]=verts[next()%8]; if(next()%4==0)verts[next()%8].z=verts[next()%8].z; if(next()%6==0)verts[next()%8].x=verts[next()%8].x;
                            if(next()%32==0)(next()%2 ? verts[next()%8].x : verts[next()%8].z)=std::bit_cast<float>(QNAN);
                            for(auto& t:tris){ t[0]=uint16(next()%8); t[1]=uint16(next()%8); t[2]=uint16(next()%8); t[3]=uint16(next()); }
                            if(nm=="CalculateHorizontalSize")call=[&]{ ret=CallSizeF(a,tris[0].data(),verts.data()); };
                            else if(nm=="Find2HighestPoints"){ static int32 o1, o2; o1=o2=0x55555555; call=[&]{ reinterpret_cast<void(__cdecl*)(void*,void*,int32*,int32*)>(a)(tris[0].data(),verts.data(),&o1,&o2); ret={o1,o2}; }; }
                            else if(nm=="FindVectorFromFirstToMissingVertex"){ static std::array<int32,2> ref; auto& t=tris[0];
                                ref[0]=next()%4 ? t[next()%3] : int32(next()%8); ref[1]=next()%5 ? t[next()%3] : next()%2 ? int32(next()%8) : int32(0x10000|t[next()%3]);
                                call=[&]{ CVector o{-1234.f,-1234.f,-1234.f}; auto* r=reinterpret_cast<CVector*(__cdecl*)(CVector*,void*,int32*,void*)>(a)(&o,tris[0].data(),ref.data(),verts.data());
                                    ret={F(o.x),F(o.y),F(o.z),r==&o}; }; }
                            else { const int32 n=int32(next()%7);
                                for(int k=0;k<6;++k){ auto& t=tris[k]; const CVector e1=verts[t[1]]-verts[t[0]], e2=verts[t[2]]-verts[t[0]];
                                    normals[k]=next()%4 ? CVector{e1.y*e2.z-e1.z*e2.y,e1.z*e2.x-e1.x*e2.z,e1.x*e2.y-e1.y*e2.x} : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; }
                                static CVector st, en; st=CVector{uni(-8.f,8.f),uni(-8.f,8.f),uni(-8.f,8.f)}; en=next()%3 ? CVector{-st.x,-st.y,-st.z}+CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f)} : CVector{uni(-8.f,8.f),uni(-8.f,8.f),uni(-8.f,8.f)};
                                if(next()%8==0)en=st;
                                call=[&,n]{ ret=reinterpret_cast<bool(__cdecl*)(void*,int32,void*,void*,CVector,CVector)>(a)(tris.data(),n,verts.data(),normals.data(),st,en); }; } }
                        else if(nm=="DoesCoverPointStillProvideCover"){ FillPoints(50,EntPick); static const int8 types[]{0,1,2,3,3,3,3,-1,4};
                            uint8* p=next()%16==0 ? nullptr : next()%2 ? ptBuf.data() : pts+(next()%100)*PT;
                            if(p==ptBuf.data()){ for(size_t b=0;b<PT;++b)p[b]=uint8(next()); At<int8>(p,0)=types[next()%std::size(types)]; At<CVector>(p,4)=NearC(); At<void*>(p,0x10)=next()%3 ? EntPick() : nullptr; }
                            CVector pos=p ? At<CVector>(p,4)+CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-2.f,2.f)} : NearC(); if(p&&next()%16==0)pos=At<CVector>(p,4);
                            call=[&,p,pos]{ ret=reinterpret_cast<bool(__cdecl*)(void*,CVector)>(a)(p,pos); }; }
                        else if(nm=="FindAndReserveCoverPoint"){ FillPoints(10+next()%90,EntPick); uint8* ped=PedPick();
                            if(next()%4==0)for(int k=0;k<100;++k){ uint8* p=pts+k*PT; if(next()%4==0)At<void*>(p,0x14)=ped; }
                            static CVector target; target=NearC()+CVector{uni(-10.f,10.f),uni(-10.f,10.f),0.f}; const bool attack=next()%2;
                            call=[&,ped,attack]{ ret=Name(reinterpret_cast<void*(__cdecl*)(void*,const CVector&,bool)>(a)(ped,target,attack)); }; }
                        else if(nm=="FindCoordinatesCoverPoint"){ uint8* p=ptBuf.data(); for(size_t b=0;b<PT;++b)p[b]=uint8(next());
                            static const int8 types[]{1,2,3,1,2,2,0,4,-1}; const int8 type=types[next()%std::size(types)]; At<int8>(p,0)=type; At<CVector>(p,4)=NearC();
                            uint8* ped=ents[8].data(); const int entIdx=type==2 ? 6+int(next()%2) : 4+int(next()%2); uint8* ent=ents[entIdx].data();
                            if(type==2){ MakeMat(mats[entIdx].data(),At<CVector>(ent,E_POS)); At<void*>(ent,E_MATRIX)=mats[entIdx].data(); } // matriz obligatoria (FindCoverPointsBehindBox)
                            At<void*>(p,0x10)=next()%10==0 ? nullptr : ent;
                            switch(next()%5){case 0:At<void*>(p,0x14)=ped;At<void*>(p,0x18)=nullptr;break;case 1:At<void*>(p,0x14)=ents[9].data();At<void*>(p,0x18)=ped;break;
                                             case 2:At<void*>(p,0x14)=ents[9].data();At<void*>(p,0x18)=ents[10].data();break;case 3:At<void*>(p,0x14)=nullptr;At<void*>(p,0x18)=nullptr;break;default:At<void*>(p,0x14)=ped;At<void*>(p,0x18)=ped;break;}
                            static CVector pos; auto* em=At<uint8*>(ent,E_MATRIX); const CVector ep=em ? At<CVector>(em,0x30) : At<CVector>(ent,E_POS);
                            pos=ep+CVector{uni(-15.f,15.f),uni(-15.f,15.f),next()%6==0 ? uni(-12.f,12.f) : uni(-2.f,2.f)}; if(next()%16==0)pos=ep;
                            if(next()%16==0){ pos.x=ep.x; pos.y=ep.y; }
                            call=[&,p,ped]{ CVector o{-1234.f,-1234.f,-1234.f}; const bool r=reinterpret_cast<bool(__cdecl*)(void*,void*,const CVector&,CVector&)>(a)(p,ped,pos,o);
                                ret={r,F(o.x),F(o.y),F(o.z)}; }; }
                        else if(nm=="FindCoverPointsForThisBuilding"){ FillPoints(next()%3==0 ? 98 : 10+next()%60,EntPick); uint8* b=ents[next()%4].data();
                            FillFx();
                            call=[&,b]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(b); }; }
                        else if(nm=="FindDirFromVector"){ CVector v{uni(-5.f,5.f),uni(-5.f,5.f),uni(-5.f,5.f)};
                            switch(next()%12){case 0:v.x=0.f;break;case 1:v.y=0.f;break;case 2:v.x=v.y=0.f;break;case 3:v.x=-0.f;v.y=uni(-1.f,1.f);break;case 4:v.y=-0.f;break;
                                              case 5:v=CVector{uni(-1e6f,1e6f),uni(-1e6f,1e6f),0.f};break;case 6:{const int d=int(next()%256);v=CVector{-std::sin(float(d)*0.024543693f),std::cos(float(d)*0.024543693f),0.f};break;}
                                              case 7:if(next()%4==0)v.x=std::bit_cast<float>(QNAN);break;default:break;}
                            call=[&,v]{ ret=reinterpret_cast<int32(__cdecl*)(CVector)>(a)(v); }; }
                        else if(nm=="FindVectorFromDir"){ const uint32 d=next();
                            call=[&,d]{ CVector o{-1234.f,-1234.f,-1234.f}; auto* r=reinterpret_cast<CVector*(__cdecl*)(CVector*,uint32)>(a)(&o,d); ret={F(o.x),F(o.y),F(o.z),r==&o}; }; }
                    }
                    else { // CPlaceable
                        InitPls();
                        uint8* self=pls[0].data();
                        const bool lists=nm=="RemoveMatrix"||nm=="AllocateStaticMatrix"||nm=="AllocateMatrix"||nm=="SetMatrix"||nm=="GetMatrix"||nm=="FreeStaticMatrix";
                        if(lists){ SetupLists();
                            if(nm=="RemoveMatrix"||nm=="FreeStaticMatrix")self=WithMatrix();
                            else self=next()%4 ? WithoutMatrix() : WithMatrix();
                            if(!self)self=WithMatrix(); if(!self)self=WithoutMatrix(); }
                        else if(next()%2) At<void*>(self,0x14)=pmats[0].data();
                        if(nm=="SetPosn-xyz"){ const float x=RandF(), y=RandF(), z=RandF(); call=[&,self,x,y,z]{ reinterpret_cast<void(__thiscall*)(void*,float,float,float)>(a)(self,x,y,z); }; }
                        else if(nm=="SetPosn-vector"){ static CVector v; v=CVector{RandF(),RandF(),RandF()}; call=[&,self]{ reinterpret_cast<void(__thiscall*)(void*,const CVector&)>(a)(self,v); }; }
                        else if(nm=="SetOrientation-xyz"){ const float x=uni(-7.f,7.f), y=uni(-7.f,7.f), z=next()%8==0 ? RandF() : uni(-7.f,7.f);
                            if(At<void*>(self,0x14))for(int k=0;k<12;++k)At<float>(pmats[0].data(),k*4)=uni(-3.f,3.f);
                            At<CVector>(pmats[0].data(),0x30)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-100.f,100.f)};
                            call=[&,self,x,y,z]{ reinterpret_cast<void(__thiscall*)(void*,float,float,float)>(a)(self,x,y,z); }; }
                        else if(nm=="SetHeading"){ const float h=next()%8==0 ? RandF() : uni(-7.f,7.f); call=[&,self,h]{ reinterpret_cast<void(__thiscall*)(void*,float)>(a)(self,h); }; }
                        else if(nm=="GetHeading"||nm=="GetRoll"){ auto* m=pmats[0].data();
                            for(int k=0;k<12;++k)At<float>(m,k*4)=next()%6 ? uni(-1.f,1.f) : RandF();
                            if(next()%4==0)At<float>(m,0x28)=next()%2 ? 0.f : -0.f;
                            call=[&,self]{ ret=CallThisF(a,self); }; }
                        else if(nm=="IsWithinArea-xy"||nm=="IsWithinArea-xyz"){
                            auto& pos=At<void*>(self,0x14) ? At<CVector>(pmats[0].data(),0x30) : At<CVector>(self,4); pos=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(-10.f,10.f)};
                            float v[6]; for(auto& x:v)x=uni(-12.f,12.f);
                            for(int k=0;k<3;++k){ if(next()%6==0)v[k]=(&pos.x)[k]; if(next()%6==0)v[k+3]=(&pos.x)[k]; if(next()%10==0)v[k+3]=v[k]; if(next()%40==0)v[next()%6]=std::bit_cast<float>(QNAN); }
                            if(next()%40==0)pos.x=std::bit_cast<float>(QNAN);
                            if(nm=="IsWithinArea-xy")call=[&,self,v0=v[0],v1=v[1],v3=v[3],v4=v[4]]{ ret=reinterpret_cast<bool(__thiscall*)(void*,float,float,float,float)>(a)(self,v0,v1,v3,v4); };
                            else call=[&,self,v0=v[0],v1=v[1],v2=v[2],v3=v[3],v4=v[4],v5=v[5]]{ ret=reinterpret_cast<bool(__thiscall*)(void*,float,float,float,float,float,float)>(a)(self,v0,v1,v2,v3,v4,v5); }; }
                        else if(nm=="RemoveMatrix"||nm=="AllocateStaticMatrix"||nm=="AllocateMatrix"||nm=="FreeStaticMatrix"){ call=[&,self]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }; }
                        else if(nm=="GetMatrix"){ call=[&,self]{ ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self)); }; }
                        else if(nm=="SetMatrix"){ auto* m=pmats[1].data(); for(int k=0;k<12;++k)At<float>(m,k*4)=next()%8 ? uni(-2.f,2.f) : RandF();
                            if(next()%2)At<float>(m,0x28)=1.f; At<CVector>(m,0x30)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-100.f,100.f)};
                            call=[&,self,m]{ reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,m); }; }
                        else if(nm=="InitMatrixArray"){ for(auto& b:arena)b=uint8(next());
                            call=[&]{ { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__cdecl*)()>(a)(); } }; }
                        else if(nm=="ShutdownMatrixArray"){ const uint32 cnt=next()%17; At<uint32>(arena.data(),0)=cnt;
                            for(uint32 e=0;e<cnt;++e){ for(size_t k=0;k<0x54;++k)arena[4+e*0x54+k]=uint8(next()); At<uint32>(arena.data(),4+e*0x54+0x40)=0; At<uint32>(arena.data(),4+e*0x54+0x44)=0; }
                            At<uint8*>(mlMem,0x1F8)=next()%4 ? arena.data()+4 : nullptr;
                            call=[&]{ { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__cdecl*)()>(a)(); } }; }
                    }

                    // ---------- llamada ----------
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    out["switches"]=switches;
                    call();

                    // ---------- resultado ----------
                    if(cls=="CCover"){ CoverOut(); if(nm=="Update")out["world"]={scanCode,Hash(reinterpret_cast<const void*>(SECTORS+(size_t(SEC_LO)*120)*8),size_t(SEC_HI-SEC_LO+1)*120*8),Hash(secNodes.data(),sizeof(secNodes)),
                                                                                Hash(vehStates.data(),vehStates.size()),Hash(objStates.data(),objStates.size())};
                                       if(nm=="FindCoverPointsForThisBuilding"||nm=="ShouldThisBuildingHaveItsCoverPointsCreated")out["col"]=Hash(colBuf.data(),colBuf.size()); }
                    else if(nm=="InitMatrixArray"||nm=="ShutdownMatrixArray"){
                        alignas(16) static std::array<uint8,4+0x54*ARENA_ELEMS> c; c=arena; const uint32 cnt=std::min<uint32>(At<uint32>(c.data(),0),uint32(ARENA_ELEMS));
                        for(uint32 e=0;e<cnt;++e)for(int k=0x45;k<0x48;++k)c[4+e*0x54+k]=0; // relleno tras el bool m_bOwnsAttachedMatrix (como Small2)
                        out["arena"]=Hash(c.data(),c.size()); out["cookie"]=At<uint32>(arena.data(),0); out["recs"]=Recs();
                        json ml=json::array(); for(size_t k=0;k<MATRIX_LIST_SIZE;k+=4){ const auto v=At<uintptr_t>(mlMem,k); const auto ab=uintptr_t(arena.data());
                            ml.push_back(v>=ab&&v<ab+arena.size() ? json(std::format("arena+{:x}",v-ab)) : v>=MATRIX_LIST&&v<MATRIX_LIST+MATRIX_LIST_SIZE ? json(std::format("ml+{:x}",v-MATRIX_LIST)) : json(v)); }
                        out["ml"]=ml; }
                    else PlOut();
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreAll();
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
    RestoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Restored()}});
}
}
