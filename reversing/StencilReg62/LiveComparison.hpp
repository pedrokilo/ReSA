#pragma once
// Banco dentro del motor: CStencilShadows::RegisterStencilShadows (0x711760). Original (hooks de RegisterStencilShadows y de
// CStencilShadowObject::Destroy desactivados) frente a candidato con fixtures deterministas (semilla por caso).
// La funcion recorre el pool de vehiculos y los sectores reales del mundo; para benchearla sin tocar nada vivo:
//  * el puntero global del pool de vehiculos (0xB74494) se cambia temporalmente por una cabecera falsa de CPool (0x14 bytes)
//    con hasta 24 vehiculos falsos de 0xA18 bytes (ranuras libres/ocupadas al azar) y se restaura al acabar cada ejecucion;
//  * las cabezas de las listas Buildings (sector) y Objects (sector repetido) de las celdas de la ventana de la camara se
//    sustituyen por nodos propios (item +0, siguiente +4; el codigo los lee crudos) y se restauran; ademas se guardan y se
//    comparan los 115.200 bytes de ms_aSectors y los 3.072 de ms_aRepeatSectors completos;
//  * entidades falsas (objetos, edificios, dummies, vehiculos, peatones) con matriz propia o sin ella (CSimpleTransform),
//    punteros de modelo 19990..19999 sustituidos por CBaseModelInfo falsos (vtable con GetModelType trazado y trampas para
//    cualquier otra ranura), modelos de colision y CCollisionData falsos (esfera, bHasShadowInfo), CVehicle::m_aSpecialColModel
//    (0xC1CC78) rellenado y restaurado;
//  * el array real de 64 CStencilShadowObject (0xC6A198), sus cabezas 0xC6A168/0xC6A16C, el contador de scan codes 0xB7CD78,
//    currArea 0xB72914, pEmptyList de referencias (0xBA1768) y los contadores 0xC6A898/0xC6A899 se vuelven a escribir en cada
//    ejecucion con listas activa/libre al azar (de 0 a 64 activos) y se restauran al final de cada una y del banco.
// Dobles trazados (sin reservar memoria: eventos en un array fijo, convertidos a json despues de la llamada), parcheando la
// direccion del EXE y el cuerpo de la DLL (resuelto con Body() antes de parchear): CStencilShadowObject::Create (0x711280:
// toma el primer objeto libre, fija duenio/tipo/caras y lo enlaza como primero de los activos; puede fallar segun una mascara
// y no crea ni reserva nada), CWorld::ClearScanCodes (0x563470) y CMemoryMgr::Free (0x72F430, solo traza). Destroy (0x711310)
// se ejecuta de verdad: EXE en el original y DLL en el candidato (su hook se conmuta con el de Register).
// Se compara retorno (solo AL), traza completa, los 0x700 bytes del array de objetos de sombra (palabras con punteros
// traducidos), cabezas, scan code, pEmptyList, hash de entidades/vehiculos/referencias y los sectores reales.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "StencilShadows.h"
#include "StencilShadowObject.h"
#include "World.h"
#include "../ProbeRuntime.hpp"

namespace StencilReg62Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }   // salta los saltos de la tabla de enlace incremental
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; } };

// --- constantes del EXE ---
constexpr uintptr_t SOBJ=0xC6A198, SHADOW_FREE=0xC6A168, SHADOW_ACTIVE=0xC6A16C, SCAN=0xB7CD78, CURR_AREA=0xB72914, VEH_POOL=0xB74494,
                    EMPTY_REFS=0xBA1768, MODEL_PTRS=0xA9B0C8, SPECIAL_COL=0xC1CC78, SECTORS=0xB7D0B8, REPEAT=0xB992B8;
constexpr size_t SOBJ_SIZE=0x1C, SOBJ_COUNT=64, SECTORS_SIZE=120*120*8, REPEAT_SIZE=16*16*12;
constexpr int MI_BASE=19990, NMI=10;                 // ranuras de ms_modelInfoPtrs sin usar
constexpr int NENT=40, NVEH=24, NNODE=700, NREF=32, NBLK=70;
constexpr size_t ENT_SZ=0x500, VEH_STRIDE=0xA18;     // 0xA18 = sizeof(CHeli), el mas grande del pool de vehiculos

// --- buferes de fixtures (estaticos: misma direccion en original y candidato) ---
alignas(16) inline std::array<std::array<uint8,ENT_SZ>,NENT> ents{};
alignas(16) inline std::array<uint8,NVEH*VEH_STRIDE> vehStore{};
alignas(16) inline std::array<uint8,NVEH> vehState{};
alignas(16) inline std::array<uint32,8> poolHdr{};                           // CPool: almacenamiento, estados, capacidad, ultima libre, 2 bytes
alignas(16) inline std::array<std::array<uint8,0x48>,NENT+NVEH> mats{};
alignas(16) inline std::array<std::array<uint8,0x40>,NMI> mis{}, cols{}, cdat{};
alignas(16) inline std::array<void*,16> miVt{};
struct RNode { void* item; void* next; }; alignas(16) inline std::array<RNode,NNODE> nodes{};
struct RRef { void* next; void* pp; };     alignas(16) inline std::array<RRef,NREF> refs{};
alignas(16) inline std::array<std::array<uint8,16>,NBLK> blocks{};            // "datos de sombra" falsos (solo identidad, nunca se reserva memoria)
alignas(16) inline CVector cam3{};

// --- nombres de punteros para las trazas ---
struct Named { uintptr_t p; size_t n; size_t stride; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name,size_t stride=0){ named.push_back({uintptr_t(p),n,stride,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=g.p&&v<g.p+g.n){ const auto o=v-g.p;
        if(g.stride)return std::string(g.name)+"#"+std::to_string(o/g.stride)+"+"+std::to_string(o%g.stride);
        return std::string(g.name)+"+"+std::to_string(o); }
    return "other";
}
inline json Word(uint32 v){ for(auto& g:named)if(v>=g.p&&v<g.p+g.n)return Name(v); return v; }

// --- eventos de los dobles: array fijo, sin reservas (se convierten a json despues de la llamada) ---
struct Ev { uint32 tag, a, b, c; };
inline std::array<Ev,2048> evs{}; inline uint32 evCount{}, evOverflow{};
inline void Push(uint32 tag,uint32 a=0,uint32 b=0,uint32 c=0){ if(evCount<evs.size())evs[evCount++]={tag,a,b,c}; else ++evOverflow; }
inline DWORD mainThread{};
inline uint32 failMask{}, createCalls{}, blockCtr{};
// 0x711280 (this = primer objeto libre; devuelve this o nullptr). Como el original: el objeto pasa a ser el primero de la lista activa
inline void* __fastcall S_Create(void* self,void*,void* ent,uint32 type){
    Push(1,uint32(uintptr_t(self)),uint32(uintptr_t(ent)),uint8(type));
    if((failMask>>(createCalls++%32))&1)return nullptr;
    auto* o=static_cast<uint8*>(self);
    const uint32 faces=1+(At<uint32>(ent,0x30)&7);
    At<void*>(o,0)=ent; At<uint16>(o,4)=uint16(faces); o[6]=uint8(type); At<uint32>(o,8)=faces*15; At<uint32>(o,0xC)=0;
    At<void*>(o,0x10)=blocks[blockCtr++%NBLK].data();
    auto*& avail=AtA<uint8*>(SHADOW_FREE); auto*& active=AtA<uint8*>(SHADOW_ACTIVE);
    avail=At<uint8*>(o,0x14); if(avail)At<void*>(avail,0x18)=nullptr;          // fuera de los libres
    At<uint8*>(o,0x14)=active; At<void*>(o,0x18)=nullptr; if(active)At<void*>(active,0x18)=o; active=o;
    return self;
}
inline void __cdecl S_ClearScan(){ Push(2); }
inline void __cdecl S_Free(void* p){ if(GetCurrentThreadId()==mainThread)Push(4,uint32(uintptr_t(p))); }
inline uint32 __fastcall S_ModelType(void* self,void*){ Push(3,uint32(uintptr_t(self))); return At<uint32>(self,0x3C); }
inline void __fastcall S_Trap(void* self,void*){ Push(9,uint32(uintptr_t(self))); }

inline json Trace(){
    json t=json::array();
    for(uint32 k=0;k<evCount;++k){ const auto& e=evs[k];
        switch(e.tag){
        case 1: t.push_back({"Create",Name(e.a),Name(e.b),e.c}); break;
        case 2: t.push_back({"ClearScanCodes"}); break;
        case 3: t.push_back({"GetModelType",Name(e.a)}); break;
        case 4: t.push_back({"Free",Name(e.a)}); break;
        default: t.push_back({"virtual_inesperada",Name(e.a)}); }
    }
    if(evOverflow)t.push_back({"overflow",evOverflow});
    return t;
}

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=1;
inline const std::array<Fn,NFNS> fns{{
    {"CStencilShadows","RegisterStencilShadows",0x711760,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stencilreg62-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stencilreg62-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stencilreg62");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"stencilreg62-only.txt"); if(f)std::getline(f,only); }
    auto* hReg=FindHook("CStencilShadows","RegisterStencilShadows"); auto* hDes=FindHook("CStencilShadowObject","Destroy");
    if(!hReg||!hDes){Write({{"event","error"},{"reason","hook not found"}});return;}
    ReversibleHooks::ReversibleHook::Base* hooks[2]{hReg,hDes};
    bool enabled[2]{},locked[2]{};
    for(int k=0;k<2;++k){enabled[k]=hooks[k]->Hooked();locked[k]=hooks[k]->Locked();hooks[k]->LockState(false);}

    // desplazamientos que usa el EXE: si la DLL difiere, se avisa antes de empezar
    {   auto* v=reinterpret_cast<CVehicle*>(vehStore.data()); auto* o=reinterpret_cast<CObject*>(ents[0].data());
        const auto base=uintptr_t(v);
        const bool ok=uintptr_t(&v->m_pCollisionList.m_node)-base==0xB0 && uintptr_t(&v->m_nModelIndex)-base==0x22 && uintptr_t(&v->m_vehicleSpecialColIndex)-base==0x48B
                      && uintptr_t(&o->m_nObjectFlags)-uintptr_t(o)==0x140;
        if(!ok){Write({{"event","error"},{"reason","offsets de la DLL distintos de los del EXE"}});for(int k=0;k<2;++k){hooks[k]->LockState(locked[k]);}return;}
    }

    // nombres, vtable de los modelos falsos
    if(named.empty()){
        Nm(ents.data(),sizeof(ents),"ent",ENT_SZ); Nm(vehStore.data(),vehStore.size(),"veh",VEH_STRIDE); Nm(mats.data(),sizeof(mats),"mat",0x48);
        Nm(mis.data(),sizeof(mis),"mi",0x40); Nm(cols.data(),sizeof(cols),"col",0x40); Nm(cdat.data(),sizeof(cdat),"cdat",0x40);
        Nm(nodes.data(),sizeof(nodes),"node",8); Nm(refs.data(),sizeof(refs),"ref",8); Nm(blocks.data(),sizeof(blocks),"block",16);
        Nm(reinterpret_cast<void*>(SOBJ),SOBJ_COUNT*SOBJ_SIZE,"so",SOBJ_SIZE); Nm(poolHdr.data(),sizeof(poolHdr),"poolHdr"); Nm(vehState.data(),vehState.size(),"vehState");
        Nm(&cam3,sizeof(cam3),"cam"); Nm(miVt.data(),sizeof(miVt),"miVt");
    }
    for(auto& v:miVt)v=reinterpret_cast<void*>(&S_Trap); miVt[4]=reinterpret_cast<void*>(&S_ModelType);   // GetModelType = ranura 4

    // estado global guardado (y comparado al final)
    if(regions.empty()){
        for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {SHADOW_FREE,0x14},{SOBJ,SOBJ_COUNT*SOBJ_SIZE},{0xC6A898,2},{CURR_AREA,4},{SCAN,2},{VEH_POOL,4},{EMPTY_REFS,4},
            {MODEL_PTRS+4*MI_BASE,4*NMI},{SPECIAL_COL,0xC0},{SECTORS,SECTORS_SIZE},{REPEAT,REPEAT_SIZE}})Snap(a,n); }

    // direcciones de las dependencias de la DLL: se resuelven ANTES de parchear nada
    const uintptr_t dllCreate=Body(MemberAddress(&CStencilShadowObject::Create));
    const uintptr_t dllClear=Body(Addr(&CWorld::ClearScanCodes));
    const uintptr_t dllFree=Body(Addr(&CMemoryMgr::Free));

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^0x62C0DE5u; if(!stt)stt=1; Rng r{stt};
                evCount=0; evOverflow=0; createCalls=0; blockCtr=0;
                for(auto* h:hooks)h->State(candidate);
                const bool switches=(ReversingProbe::JumpsToDll(0x711760)==candidate)&&(ReversingProbe::JumpsToDll(0x711310)==candidate);
                const uintptr_t a=fn.address;
                json out={{"switches",switches}};

                // ---------------- fixture ----------------
                const bool noCreate=r.pct(22);
                float cx,cy;
                switch(r.below(20)){
                case 0: case 1: cx=r.uni(-3150.f,-2900.f); cy=r.uni(-3000.f,3000.f); break;                     // borde oeste (recorte a 0)
                case 2: case 3: cx=r.uni(-3000.f,3000.f); cy=r.uni(-3150.f,-2900.f); break;                     // borde sur
                case 4: case 5: cx=r.uni(2900.f,3150.f); cy=r.uni(-3000.f,3000.f); break;                       // borde este (recorte a 0x77)
                case 6: cx=r.uni(-3000.f,3000.f); cy=r.uni(2900.f,3150.f); break;                               // borde norte
                case 7: cx=r.uni(-30000.f,30000.f); cy=r.uni(-30000.f,30000.f); break;                         // fuera del mundo
                case 8: case 9: { static const float eps[]{0.f,1e-4f,-1e-4f,1e-3f,-1e-3f,0.0078125f,-0.0078125f};   // x-50 / y-50 justo en el limite de una celda
                    cx=50.f*float(int(r.below(120))-60)+50.f+eps[r.below(7)]; cy=50.f*float(int(r.below(120))-60)+50.f+eps[r.below(7)]; break; }
                default: cx=r.uni(-3000.f,3000.f); cy=r.uni(-3000.f,3000.f); }
                cam3=CVector{cx,cy,r.uni(-50.f,300.f)};
                const uint8 currArea=uint8(r.pct(60) ? r.below(4) : r.pct(40) ? 13 : r.below(20));
                AtA<uint32>(CURR_AREA)=currArea;
                const uint16 scan0=uint16(r.pct(6) ? 65535 : r.pct(50) ? r.below(8) : r.below(65535));
                AtA<uint16>(SCAN)=scan0;
                const uint16 scanNew=uint16(scan0>=65535 ? 1 : scan0+1);
                failMask=r.pct(55) ? 0u : r.pct(50) ? r.next()&r.next() : r.next();
                AtA<uint8>(0xC6A899)=uint8(r.next()); AtA<uint8>(0xC6A898)=uint8(r.next());

                // modelos de colision y de modelo falsos
                static const float eps24[]{0.f,3.8147e-6f,-3.8147e-6f,7.6294e-6f,-7.6294e-6f,1.1444e-5f,-1.1444e-5f,1e-4f,-1e-4f};
                for(int k=0;k<NMI;++k){
                    mis[k].fill(0); cols[k].fill(0); for(auto& b:cdat[k])b=uint8(r.next());
                    At<void*>(mis[k].data(),0)=miVt.data(); At<void*>(mis[k].data(),0x14)=cols[k].data();
                    At<uint32>(mis[k].data(),0x3C)=r.pct(70) ? 1u : r.pct(50) ? r.below(8) : r.below(256);
                    At<float>(cols[k].data(),0x18)=r.uni(-8.f,8.f); At<float>(cols[k].data(),0x1C)=r.uni(-8.f,8.f); At<float>(cols[k].data(),0x20)=r.uni(-8.f,8.f);
                    At<float>(cols[k].data(),0x24)=r.pct(8) ? 0.f : r.uni(0.f,25.f);
                    At<void*>(cols[k].data(),0x2C)=r.pct(12) ? nullptr : cdat[k].data();
                    At<uint8>(cdat[k].data(),7)=uint8((At<uint8>(cdat[k].data(),7)&~4u)|(r.pct(85) ? 4u : 0u));
                    AtA<void*>(MODEL_PTRS+4*(MI_BASE+k))=mis[k].data();
                }
                for(int j=0;j<4;++j){ const uintptr_t sc=SPECIAL_COL+0x30*j; const int k=int(r.below(NMI));
                    AtA<float>(sc+0x18)=At<float>(cols[k].data(),0x18)+0.5f; AtA<float>(sc+0x1C)=At<float>(cols[k].data(),0x1C); AtA<float>(sc+0x20)=At<float>(cols[k].data(),0x20);
                    AtA<float>(sc+0x24)=r.uni(0.f,25.f); AtA<void*>(sc+0x2C)=r.pct(15) ? nullptr : cdat[k].data(); }

                // entidades (NENT generales + NVEH del pool)
                auto FillEntity=[&](uint8* b,size_t sz,int idx,bool vehicle){
                    std::memset(b,0,sz);
                    uint32 type;
                    if(vehicle)type=2; else { const auto t=r.below(100); type= t<55 ? 4u : t<75 ? 1u : t<85 ? 5u : t<90 ? 2u : t<95 ? 3u : 6u; }
                    uint32 flags=r.next()&~((1u<<7)|(1u<<26)); if(r.pct(88))flags|=1u<<7; if(r.pct(8))flags|=1u<<26;
                    At<uint32>(b,0x1C)=flags; At<uint8>(b,0x36)=uint8(type|(r.next()&0xF8));
                    const int k=int(r.below(NMI)); At<int16>(b,0x22)=int16(MI_BASE+k);
                    At<uint16>(b,0x2C)=uint16(r.pct(14) ? scanNew : r.pct(30) ? r.below(8) : r.below(65536));
                    At<uint8>(b,0x2F)=uint8(r.pct(65) ? currArea : r.pct(50) ? 13 : r.below(20));
                    uint32 of=r.next(); of=(of&~((1u<<6)|(1u<<10)))|(r.pct(8) ? 1u<<6 : 0u)|(r.pct(8) ? 1u<<10 : 0u); At<uint32>(b,0x140)=of;
                    At<uint32>(b,0x30)=r.next();
                    if(type==2||vehicle)At<int8>(b,0x48B)=int8(r.pct(85) ? -1 : int(r.below(4)));
                    if(vehicle)At<void*>(b,0xB0)=r.pct(80) ? static_cast<void*>(&nodes[r.below(NNODE)]) : nullptr;
                    // posicion: cerca de la camara o justo en el limite de los 50 de distancia a la superficie de la esfera
                    const float rad=At<float>(cols[k].data(),0x24);
                    CVector pos{cam3.x+r.uni(-90.f,90.f),cam3.y+r.uni(-90.f,90.f),cam3.z+r.uni(-40.f,40.f)}; float heading=r.uni(-7.f,7.f);
                    const bool edge=r.pct(30);
                    if(edge){ const double az=double(r.uni(0.f,6.2831853f)), el=double(r.uni(-1.2f,1.2f));
                        const float D=rad+50.f+eps24[r.below(9)];
                        const CVector c{At<float>(cols[k].data(),0x18),At<float>(cols[k].data(),0x1C),At<float>(cols[k].data(),0x20)};
                        pos=CVector{cam3.x+float(std::cos(el)*std::cos(az))*D-c.x,cam3.y+float(std::cos(el)*std::sin(az))*D-c.y,cam3.z+float(std::sin(el))*D-c.z}; heading=0.f; }
                    At<float>(b,4)=pos.x; At<float>(b,8)=pos.y; At<float>(b,0xC)=pos.z; At<float>(b,0x10)=heading;
                    if(!edge&&r.pct(60)){ auto& m=mats[idx]; for(int q=0;q<12;++q)At<float>(m.data(),q*4)=r.uni(-1.2f,1.2f);
                        At<float>(m.data(),0x30)=pos.x; At<float>(m.data(),0x34)=pos.y; At<float>(m.data(),0x38)=pos.z; At<void*>(b,0x14)=m.data(); }
                    (void)idx; };
                for(int e=0;e<NENT;++e)FillEntity(ents[e].data(),ENT_SZ,e,false);
                for(int v=0;v<NVEH;++v)FillEntity(vehStore.data()+size_t(v)*VEH_STRIDE,VEH_STRIDE,NENT+v,true);
                // pool de vehiculos falso
                const int nveh=r.pct(15) ? 0 : int(r.below(NVEH+1));
                for(int v=0;v<NVEH;++v)vehState[v]=uint8(v<nveh ? ((r.pct(30) ? 0x80 : 0)|r.below(128)) : 0x80);
                poolHdr.fill(0); poolHdr[0]=uint32(uintptr_t(vehStore.data())); poolHdr[1]=uint32(uintptr_t(vehState.data())); poolHdr[2]=uint32(nveh); poolHdr[3]=uint32(-1);
                AtA<void*>(VEH_POOL)=poolHdr.data();

                // listas de los sectores de la ventana de la camara
                size_t nn=0;
                auto Node=[&](void* item,void* next)->RNode*{ if(nn>=nodes.size())return nullptr; auto* n=&nodes[nn++]; n->item=item; n->next=next; return n; };
                auto RandEntity=[&]()->void*{ return r.pct(12) ? static_cast<void*>(vehStore.data()+size_t(r.below(NVEH))*VEH_STRIDE) : static_cast<void*>(ents[r.below(NENT)].data()); };
                auto BuildList=[&](RNode*& head){ head=nullptr; const uint32 n=r.pct(35) ? 0 : 1+r.below(r.pct(15) ? 12 : 4); for(uint32 q=0;q<n;++q){ head=Node(RandEntity(),head); if(!head)break; } };
                const int gx0=int(std::floor(double(cam3.x)*0.02+60.0)), gy0=int(std::floor(double(cam3.y)*0.02+60.0));
                for(int gy=gy0-2;gy<=gy0+2;++gy)for(int gx=gx0-2;gx<=gx0+2;++gx){
                    if(gx<0||gx>119||gy<0||gy>119)continue;
                    BuildList(*reinterpret_cast<RNode**>(&CWorld::GetSector(gx,gy).Buildings));
                    BuildList(*reinterpret_cast<RNode**>(&CWorld::GetRepeatSector(gx,gy).Objects)); }

                // objetos de sombra: listas activa y libre
                uint8* so=reinterpret_cast<uint8*>(SOBJ); std::memset(so,0,SOBJ_COUNT*SOBJ_SIZE);
                int perm[SOBJ_COUNT]; for(int q=0;q<int(SOBJ_COUNT);++q)perm[q]=q;
                for(int q=int(SOBJ_COUNT)-1;q>0;--q)std::swap(perm[q],perm[r.below(uint32(q+1))]);
                const uint32 roll=r.below(100);
                const int nact= roll<10 ? 0 : roll<35 ? int(1+r.below(6)) : roll<60 ? int(7+r.below(30)) : roll<85 ? int(40+r.below(20)) : int(60+r.below(5));
                const int nfree=int(SOBJ_COUNT)-nact;
                auto SO=[&](int q)->uint8*{ return so+size_t(perm[q])*SOBJ_SIZE; };
                for(int q=0;q<nact;++q){ At<void*>(SO(q),0x18)=q ? SO(q-1) : nullptr; At<void*>(SO(q),0x14)=q+1<nact ? SO(q+1) : nullptr; }
                for(int q=0;q<nfree;++q){ uint8* o=SO(nact+q); At<void*>(o,0x18)=q ? SO(nact+q-1) : nullptr; At<void*>(o,0x14)=q+1<nfree ? SO(nact+q+1) : nullptr; }
                AtA<void*>(SHADOW_ACTIVE)=nact ? SO(0) : nullptr; AtA<void*>(SHADOW_FREE)=nfree ? SO(nact) : nullptr;
                // referencias de las entidades duenias (para CleanUpOldReference dentro de Destroy) y lista de referencias libres
                int nr=0;
                auto AddRef=[&](void* owner,void* pp){ if(nr>=24)return; auto* n=&refs[nr++]; n->next=At<void*>(owner,0x24); n->pp=pp; At<void*>(owner,0x24)=n; };
                refs.fill(RRef{});
                for(int q=0;q<nact;++q){
                    uint8* o=SO(q); const uint32 tr=r.below(100); const uint32 t= tr<42 ? 1u : tr<84 ? 2u : tr<92 ? 0u : 3u+r.below(3);
                    void* owner;
                    if(r.pct(7))owner=nullptr;
                    else if(t==2||r.pct(6))owner=vehStore.data()+size_t(r.below(NVEH))*VEH_STRIDE;
                    else owner=ents[r.below(NENT)].data();
                    At<void*>(o,0)=owner; At<uint16>(o,4)=uint16(1+r.below(15)); o[6]=uint8(t);
                    At<uint32>(o,8)=r.below(200); At<uint32>(o,0xC)=r.below(100); At<void*>(o,0x10)=r.pct(75) ? static_cast<void*>(blocks[blockCtr++%NBLK].data()) : nullptr;
                    if(owner&&r.pct(45)){ const bool decoyFirst=r.pct(50); if(decoyFirst)AddRef(owner,blocks[r.below(NBLK)].data()); if(r.pct(75))AddRef(owner,o); if(!decoyFirst&&r.pct(50))AddRef(owner,blocks[r.below(NBLK)].data()); }
                }
                { void* head=nullptr; if(r.pct(70)){ for(int q=NREF-1;q>=24;--q){ refs[q].next=head; head=&refs[q]; } } AtA<void*>(EMPTY_REFS)=head; }

                // ---------------- llamada ----------------
                bool ret{};
                {   Patches P;
                    P.Add(0x711280,S_Create); P.Add(dllCreate,S_Create);
                    P.Add(0x563470,S_ClearScan); P.Add(dllClear,S_ClearScan);
                    P.Add(0x72F430,S_Free); P.Add(dllFree,S_Free);
                    ret=reinterpret_cast<bool(__cdecl*)(CVector*,bool)>(a)(&cam3,noCreate);
                }
                out["ret"]=uint32(uint8(ret)); out["noCreate"]=noCreate; out["trace"]=Trace();
                json w=json::array(); const auto* words=reinterpret_cast<const uint32*>(SOBJ); for(size_t k=0;k<SOBJ_COUNT*SOBJ_SIZE/4;++k)w.push_back(Word(words[k]));
                out["objs"]=w; out["free"]=Word(AtA<uint32>(SHADOW_FREE)); out["active"]=Word(AtA<uint32>(SHADOW_ACTIVE));
                { json walk=json::array(); const auto* p=AtA<uint8*>(SHADOW_ACTIVE); for(int g=0;p&&g<70;++g){ walk.push_back(Name(uintptr_t(p))); p=At<uint8*>(const_cast<uint8*>(p),0x14); } out["walk"]=walk; }
                out["scan"]=AtA<uint16>(SCAN); out["scanExpected"]=scanNew; out["empty"]=Word(AtA<uint32>(EMPTY_REFS));
                out["counters"]=json::array({AtA<uint8>(0xC6A899),AtA<uint8>(0xC6A898)});
                out["ents"]=Fnv(ents.data(),sizeof(ents)); out["veh"]=Fnv(vehStore.data(),vehStore.size()); out["refs"]=Fnv(refs.data(),sizeof(refs));
                out["sectors"]=Fnv(reinterpret_cast<void*>(SECTORS),SECTORS_SIZE); out["repeat"]=Fnv(reinterpret_cast<void*>(REPEAT),REPEAT_SIZE);
                out["pool"]=Fnv(poolHdr.data(),sizeof(poolHdr)); out["poolPtr"]=Word(AtA<uint32>(VEH_POOL));
                for(auto* h:hooks)h->State(false);
                RestoreRegions();
                return out;
            };
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
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
    for(int k=0;k<2;++k){hooks[k]->State(enabled[k]);hooks[k]->LockState(locked[k]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()}});
}
}
