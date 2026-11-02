#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Vehicle/Vehicle.h"
#include "CarCtrl.h"
#include "Collision/Collision.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "World.h"
#include "Models/ModelInfo.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84D dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::DragCarToPoint (0x42EC90, hook S, cdecl con dos argumentos, void). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (sus hooks RH
// quedan apagados durante el banco): CCollision::ProcessLineSphere 0x412AA0 (cdecl, 4 punteros, bool), CCollision::IsStoredPolyStillValidVerticalLine 0x414D70 (cdecl: origen, float, punto, poligono; bool),
// CWorld::ProcessVerticalLine 0x5674E0 (cdecl, 11 argumentos; bool), CrossProduct 0x59C730 (cdecl, out/a/b) y CVehicle::UpdateLightingFromStoredPolys 0x6D0CC0 (thiscall). El virtual GetHeightAboveRoad
// (vtabla +0xD4) es una vtabla falsa del banco compartida por los dos lados. Corren de verdad, en ambos lados, la matriz del vehiculo, su informacion de modelo y su caja de colision (arenas enlazadas desde
// CModelInfo::ms_modelInfoPtrs; region restaurada tras cada lado) y las constantes de .rdata (se vigilan sus bytes).
// Los falsos registran sus argumentos EN ORDEN (punteros de arena por etiqueta, locales de pila por su marca), con el byte +0x3E1 y las banderas del vehiculo en ese instante, y escriben los valores que da el
// caso. Tras el evento numero mutAt el falso muta el estado (posicion de la matriz, banderas, suelos guardados, matriz del vehiculo, contador, banderas de las entidades) para comprobar que la funcion relee
// esos campos donde el EXE los relee.

namespace C84DLive {
using namespace C84DFix;
inline constexpr uintptr_t kEntry = 0x42EC90;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(sizeof(CVehicle)==0x5A0&&kVehWin==sizeof(CVehicle),"la arena del vehiculo es un CVehicle completo");
static_assert(offsetof(CVehicle,m_FrontCollPoly)==0x51C&&offsetof(CVehicle,m_RearCollPoly)==0x548&&offsetof(CVehicle,m_pEntityWeAreOn)==0x48C&&offsetof(CVehicle,m_fSteerAngle)==0x494
              &&offsetof(CVehicle,m_fVehicleFrontGroundZ)==0x4E4&&offsetof(CVehicle,m_fVehicleRearGroundZ)==0x4E8&&offsetof(CVehicle,m_matrix)==0x14&&offsetof(CEntity,m_nModelIndex)==0x22,"CVehicle: campos");
static_assert(offsetof(CBaseModelInfo,m_pColModel)==0x14&&offsetof(CColModel,m_boundBox)==0&&offsetof(CBoundingBox,m_vecMin)==0&&offsetof(CBoundingBox,m_vecMax)==0xC,"modelo y caja de colision");
static_assert(offsetof(CColLine,m_vecStart)==0&&offsetof(CColLine,m_vecEnd)==0x10&&offsetof(CColPoint,m_vecPoint)==0,"CColLine / CColPoint");

// Disposicion de CMatrix (miembros privados en algunos sitios: se mide con los accesores): derecha +0, adelante +0x10, arriba +0x20, posicion +0x30
inline bool MatrixLayoutOk() {
    CMatrix m;
    const auto base=uintptr_t(&m);
    return uintptr_t(&m.GetRight())-base==0x00&&uintptr_t(&m.GetForward())-base==0x10&&uintptr_t(&m.GetUp())-base==0x20&&uintptr_t(&m.GetPosition())-base==0x30;
}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(size_t off,u32 v) {const uint16 w=uint16(v);memcpy(m.data()+kGuard+off,&w,2);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    u32 Get8(size_t off) const {return m[kGuard+off];}
    // Cambios fuera de los campos que el banco compara por valor: known = [[inicio, fin)] respecto al objeto
    json Rest(std::initializer_list<std::pair<int64,int64>> known) const {
        json out=json::array();
        for(const auto& ch:ArenaChanges(m.data(),r.data(),N,kGuard)) {
            const int64 o=ch[0].get<int64>();
            bool isKnown=false;
            for(const auto& [lo,hi]:known)isKnown|=o>=lo&&o<hi;
            if(!isKnown)out.push_back(ch);
        }
        return out;
    }
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA,matB;
inline Arena<kMiArena> miA;
inline Arena<kColArena> colA;
inline Arena<kEntArena> entA,entB;
inline Arena<kTgtArena> tgtA;

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ falsos de los colaboradores
struct State {const C* c{};u32 evHash{};u32 unexpected{};u32 ev{};u32 isv{};u32 pvl{};u32 cross{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline u32 Raw(const void* p,unsigned i) {return BitsOf(static_cast<const u8*>(p)+4*i);}
inline u32 Counter() {return vehA.Get8(0x3E1);}
inline u32 VFlags() {return vehA.Get32(0x1C);}
inline uint8* CurMat() {return reinterpret_cast<uint8*>(uintptr_t(vehA.Get32(0x14)));}
inline void PutAt(uint8* p,u32 v) {memcpy(p,&v,4);}
// Mutaciones: 1 posicion.z de la matriz actual, 2 banderas del vehiculo, 3 suelo delantero guardado, 4 suelo trasero guardado, 5 la matriz del vehiculo pasa a la otra, 6 byte +0x3E1,
// 7 banderas de la entidad A, 8 banderas de la entidad B, 9 posicion.x y .y de la matriz actual
inline void ApplyMut(u32 kind,u32 v) {
    switch(kind) {
    case 1: PutAt(CurMat()+0x38,v);break;
    case 2: vehA.Put32(0x1C,v);break;
    case 3: vehA.Put32(0x4E4,v);break;
    case 4: vehA.Put32(0x4E8,v);break;
    case 5: vehA.Put32(0x14,u32(matB.Obj()));break;
    case 6: vehA.Put8(0x3E1,v);break;
    case 7: entA.Put32(0x1C,v);break;
    case 8: entB.Put32(0x1C,v);break;
    case 9: PutAt(CurMat()+0x30,v);PutAt(CurMat()+0x34,v);break;
    default: break;
    }
}
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    ApplyMut(c.mutKind,c.mutVal);
}
// Eventos: 1 ProcessLineSphere [byte +0x3E1, inicio(3), fin(3), centro(3), radio, profundidad, &linea, &esfera, &punto, &profundidad]
// 2 IsStoredPolyStillValidVerticalLine [byte, banderas, origen(3), distancia, &origen, &punto, &poligono]
// 3 ProcessVerticalLine [byte, banderas, origen(3), distancia, 6 booleanos, &origen, &punto, &entidad, &poligono]
// 4 CrossProduct [&out, &a, &b, a(3), b(3)]   5 GetHeightAboveRoad [&this, posicion(3), arriba(3)]   6 UpdateLightingFromStoredPolys [&this, posicion(3), arriba(3)]   99 virtual inesperado
inline bool __cdecl FakeSphere(void* line,void* sphere,void* cp,void* depth) {
    Ev& e=PushEv(1,16);e.v[0]=Counter();
    for(unsigned i=0;i<3;++i) {e.v[1+i]=Raw(line,i);e.v[4+i]=Raw(line,4+i);e.v[7+i]=Raw(sphere,i);}
    e.v[10]=Raw(sphere,3);e.v[11]=Raw(depth,0);e.v[12]=Tag(line);e.v[13]=Tag(sphere);e.v[14]=Tag(cp);e.v[15]=Tag(depth);HashEv(e);
    const C& c=*g.c;
    const bool ret=c.sRet!=0;
    if(ret)for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(cp)+4*i,&c.sCp[i],4);
    After();
    return ret;
}
inline bool __cdecl FakeIsValid(void* origin,u32 dist,void* cp,void* poly) {
    Ev& e=PushEv(2,9);e.v[0]=Counter();e.v[1]=VFlags();
    for(unsigned i=0;i<3;++i)e.v[2+i]=Raw(origin,i);
    e.v[5]=dist;e.v[6]=Tag(origin);e.v[7]=Tag(cp);e.v[8]=Tag(poly);HashEv(e);
    const C& c=*g.c;
    const unsigned k=g.isv<2?g.isv:1;++g.isv;
    const bool ret=c.vRet[k]!=0;
    if(ret)memcpy(static_cast<u8*>(cp)+8,&c.vZ[k],4);
    After();
    return ret;
}
inline bool __cdecl FakePvl(void* origin,u32 dist,void* cp,void** entity,u32 b0,u32 b1,u32 b2,u32 b3,u32 b4,u32 b5,void* poly) {
    Ev& e=PushEv(3,16);e.v[0]=Counter();e.v[1]=VFlags();
    for(unsigned i=0;i<3;++i)e.v[2+i]=Raw(origin,i);
    e.v[5]=dist;e.v[6]=b0&0xFFu;e.v[7]=b1&0xFFu;e.v[8]=b2&0xFFu;e.v[9]=b3&0xFFu;e.v[10]=b4&0xFFu;e.v[11]=b5&0xFFu;
    e.v[12]=Tag(origin);e.v[13]=Tag(cp);e.v[14]=Tag(static_cast<const void*>(entity));e.v[15]=Tag(poly);HashEv(e);
    const C& c=*g.c;
    const unsigned k=g.pvl<4?g.pvl:3;++g.pvl;
    const bool ret=c.pRet[k]!=0;
    if(ret) {
        memcpy(static_cast<u8*>(cp)+8,&c.pZ[k],4);
        *entity=reinterpret_cast<void*>(c.pEnt[k]==0?entA.Obj():entB.Obj());
    }
    After();
    return ret;
}
inline void* __cdecl FakeCross(void* out,void* a,void* b) {
    Ev& e=PushEv(4,9);e.v[0]=Tag(out);e.v[1]=Tag(a);e.v[2]=Tag(b);
    for(unsigned i=0;i<3;++i) {e.v[3+i]=Raw(a,i);e.v[6+i]=Raw(b,i);}
    HashEv(e);
    const unsigned k=g.cross<2?g.cross:1;++g.cross;
    for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(out)+4*i,&g.c->crs[k][i],4);
    After();
    return out;
}
inline void VehicleEvent(u32 kind,void* self) {
    Ev& e=PushEv(kind,7);e.v[0]=Tag(self);
    const uint8* m=CurMat();
    for(unsigned i=0;i<3;++i) {e.v[1+i]=Raw(m,12+i);e.v[4+i]=Raw(m,8+i);}
    HashEv(e);
}
inline float __fastcall FakeHeight(void* self,void*) {
    VehicleEvent(5,self);
    const float h=std::bit_cast<float>(g.c->height);
    After();
    return h;
}
inline void __fastcall FakeLighting(void* self,void*) {
    VehicleEvent(6,self);
    After();
}
inline u32 __fastcall FakeUnexpectedVirtual(void* self,void*) {++g.unexpected;Ev& e=PushEv(99,1);e.v[0]=Tag(self);HashEv(e);return 0;}
// vtabla falsa del vehiculo: solo la entrada 0xD4 (GetHeightAboveRoad) es valida
inline std::array<u32,96> vtbl{};
inline void InitVtbl() {
    for(auto& v:vtbl)v=u32(reinterpret_cast<uintptr_t>(&FakeUnexpectedVirtual));
    vtbl[0xD4/4]=u32(reinterpret_cast<uintptr_t>(&FakeHeight));
}

// ------------------------------------------------------------------------------------------------ microtests de la x87 (hardware): las reglas que supone el oraculo
// op: 0 fld a; fadd b | 1 fld a; fsub b | 2 fld a; fmul b | 3 fld a; fdiv b | 4 fld a; fsqrt | 5 fld a; fistp qword (trunca) -> EAX | 6 fld a; fabs. El resultado de 0..4 y 6 es el fstp dword.
inline u32 X87Micro(u32 op,u32 a,u32 b) {
    u32 r{};
    switch(op) {
    case 0: __asm {fld dword ptr a} __asm {fadd dword ptr b} __asm {fstp dword ptr r} break;
    case 1: __asm {fld dword ptr a} __asm {fsub dword ptr b} __asm {fstp dword ptr r} break;
    case 2: __asm {fld dword ptr a} __asm {fmul dword ptr b} __asm {fstp dword ptr r} break;
    case 3: __asm {fld dword ptr a} __asm {fdiv dword ptr b} __asm {fstp dword ptr r} break;
    case 4: __asm {fld dword ptr a} __asm {fsqrt} __asm {fstp dword ptr r} break;
    case 5: {
        uint16 cw{},cwTrunc{};uint64 wide{};
        __asm {fnstcw word ptr cw}
        cwTrunc=uint16(cw|0x0C00);
        __asm {fldcw word ptr cwTrunc}
        __asm {fld dword ptr a} __asm {fistp qword ptr wide}
        __asm {fldcw word ptr cw}
        r=u32(wide&0xFFFFFFFFu);
        break;
    }
    default: __asm {fld dword ptr a} __asm {fabs} __asm {fstp dword ptr r} break;
    }
    return r;
}
inline json MicroTests() {
    json out=json::array();
    PrepareFloatingState();
    static constexpr u32 vals[]{0x7FC00001u,0xFFC00002u,0x7FC12345u,0xFFC54321u,0x7F800001u,0xFFA00003u,0x7FA00000u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x40A00000u,0x00000001u,0x007FFFFFu,0x7F7FFFFFu,
        0xFFC00001u,0x7FC00002u,0x7FC00000u,0xFFC00000u,0xFF800001u,0x7FA00003u,0xFF800002u,0x7F800002u};
    for(u32 op=0;op<4;++op)for(u32 a:vals)for(u32 b:vals)out.push_back({op,a,b,X87Micro(op,a,b)});
    static constexpr u32 unary[]{0x7FC00001u,0x7F800001u,0xFFC12345u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x40000000u,0x40800000u,0x00000001u,0x007FFFFFu,0x7F7FFFFFu,0x41F00000u,0x4F000000u,0x4F800000u,0x5F000000u,0x5F7FFFFFu,0xCF000000u,0x4EFFFFFFu,0x2F800000u,0x3F7FFFFFu,0xBF7FFFFFu,0x4B000001u,0x5E800000u};
    for(u32 op:{4u,5u,6u})for(u32 a:unary)out.push_back({op,a,0u,X87Micro(op,a,0u)});
    RestoreFloatingState();
    return out;
}

// ================================================================================================ el caso
inline void CaseDrag(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*17u+1u);matA.Fill(c.seed*17u+2u);matB.Fill(c.seed*17u+3u);miA.Fill(c.seed*17u+4u);colA.Fill(c.seed*17u+5u);entA.Fill(c.seed*17u+6u);entB.Fill(c.seed*17u+7u);tgtA.Fill(c.seed*17u+8u);
        for(unsigned i=0;i<3;++i) {
            matA.Put32(4*i,c.mr[i]);matA.Put32(0x10+4*i,c.mf[i]);matA.Put32(0x20+4*i,c.mu[i]);matA.Put32(0x30+4*i,c.mp[i]);
            matB.Put32(4*i,c.br[i]);matB.Put32(0x10+4*i,c.bf[i]);matB.Put32(0x20+4*i,c.bu[i]);matB.Put32(0x30+4*i,c.bp[i]);
            tgtA.Put32(4*i,c.tgt[i]);
            vehA.Put32(4+4*i,c.place[i]);
        }
        colA.Put32(4,c.colMin);colA.Put32(0x10,c.colMax);
        miA.Put32(0x14,u32(colA.Obj()));
        entA.Put32(0x1C,c.eflags[0]);entB.Put32(0x1C,c.eflags[1]);
        vehA.Put32(0,u32(uintptr_t(vtbl.data())));
        vehA.Put32(0x14,u32(matA.Obj()));
        vehA.Put32(0x1C,c.vflags);
        vehA.Put16(0x22,kModelBase+c.mslot);
        vehA.Put8(0x3E1,c.cnt);
        vehA.Put32(0x48C,c.entp0);vehA.Put32(0x494,c.steer0);vehA.Put32(0x4E4,c.g4e4);vehA.Put32(0x4E8,c.g4e8);
        G32(kModelPtrs+(kModelBase+c.mslot)*4)=u32(miA.Obj());
        vehA.Snap();matA.Snap();matB.Snap();miA.Snap();colA.Snap();entA.Snap();entB.Snap();tgtA.Snap();
        TagClear();vehA.Tagged();matA.Tagged();matB.Tagged();miA.Tagged();colA.Tagged();entA.Tagged();entB.Tagged();tgtA.Tagged();
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        const u32 args[2]{u32(vehA.Obj()),u32(tgtA.Obj())};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,c.ecx,c.edx,c.stack,8);});
        json obs;
        obs["v"]=json::array({vehA.Get32(0x1C),vehA.Get8(0x3E1),Tag(uintptr_t(vehA.Get32(0x48C))),vehA.Get32(0x494),vehA.Get32(0x4E4),vehA.Get32(0x4E8),Tag(uintptr_t(vehA.Get32(0x14)))});
        {
            json mats=json::array();
            for(const auto* a:{&matA,&matB}) {
                json words=json::array();
                for(unsigned o:{0u,4u,8u,0x10u,0x14u,0x18u,0x20u,0x24u,0x28u,0x30u,0x34u,0x38u})words.push_back(a->Get32(o));
                mats.push_back(words);
            }
            obs["mat"]=mats;
        }
        obs["ent"]=json::array({entA.Get32(0x1C),entB.Get32(0x1C)});
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        obs["unexpected_virtuals"]=g.unexpected;
        {   // escrituras esperadas: vehiculo (+0x14, +0x1C, +0x3E1, +0x48C, +0x494, +0x4E4, +0x4E8), matrices (derecha, adelante, arriba, posicion) y banderas de las entidades; nada mas
            const std::pair<int64,int64> mat[]{{0,0xC},{0x10,0x1C},{0x20,0x2C},{0x30,0x3C}};
            obs["changes"]=json::array({vehA.Rest({{0x14,0x18},{0x1C,0x20},{0x3E1,0x3E2},{0x48C,0x490},{0x494,0x498},{0x4E4,0x4EC}}),
                matA.Rest({mat[0],mat[1],mat[2],mat[3]}),matB.Rest({mat[0],mat[1],mat[2],mat[3]}),miA.Rest({}),colA.Rest({}),entA.Rest({{0x1C,0x20}}),entB.Rest({{0x1C,0x20}}),tgtA.Rest({})});
        }
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(unsigned f=0;f<kFunctions;++f)cases[kFn[f]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseDrag(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe,std::string& cat,std::string& name) {
    bool found=false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for(auto& h:c.Items())
            if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple
                &&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress())==exe) {
                cat=std::string(c.Name());name=std::string(h->Name());found=true;
            }
    });
    return found;
}
// Colaborador: si la DLL tiene hook RH (se apaga al crear su ruta) se anota; el cuerpo de la DLL se toma siempre de la funcion C++ (puntero a funcion), de modo que tambien las funciones sin hook
// (llamadas directamente por la DLL) llevan el mismo falso.
inline Pair MakeCollab(Env& env,uintptr_t exe,uintptr_t dllFunction,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
    }
    else hooksFound.push_back({{"exe",exe},{"class",nullptr},{"name",nullptr}});
    p.dll=Body(dllFunction);
    return p;
}
template<class T> inline uintptr_t FnAddr(T f) {
    if constexpr (std::is_member_function_pointer_v<T>) return std::bit_cast<uintptr_t>(f); else return reinterpret_cast<uintptr_t>(f);
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"car84d-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car84d-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car84d");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=MatrixLayoutOk();json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    {
        std::string cat,name;
        const bool found=FindHook(kEntry,cat,name);
        if(found)env.route[0].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry,HookRoute::RegisteredTarget{}));
        registered[kFn[0]]=found&&env.route[0]->Registered();bodies[kFn[0]]=found&&env.route[0]->Body()!=0;
        ready&=found&&env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[0]]=found?json({{"class",cat},{"name",name},{"address",kEntry}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){Run(env,false,registered,bodies,collaborators);return;}
    InitVtbl();
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    using Void=void*;
    env.stubs[0]={
        S(MakeCollab(env,0x412AA0,FnAddr(&CCollision::ProcessLineSphere),hooksFound),Void(&FakeSphere)),
        S(MakeCollab(env,0x414D70,FnAddr(&CCollision::IsStoredPolyStillValidVerticalLine),hooksFound),Void(&FakeIsValid)),
        S(MakeCollab(env,0x5674E0,FnAddr(&CWorld::ProcessVerticalLine),hooksFound),Void(&FakePvl)),
        S(MakeCollab(env,0x59C730,FnAddr(static_cast<CVector*(*)(CVector*,CVector*,CVector*)>(&CrossProduct)),hooksFound),Void(&FakeCross)),
        S(MakeCollab(env,0x6D0CC0,FnAddr(&CVehicle::UpdateLightingFromStoredPolys),hooksFound),Void(&FakeLighting))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"matriz del vehiculo, informacion de modelo y caja de colision (arenas enlazadas desde CModelInfo::ms_modelInfoPtrs, region restaurada)","constantes de .rdata 0.95f, 0.5f, 1.5f, 2.0f, 1.0f, -1.0f, -10.0f y -0.5f (se vigilan sus bytes)","ayudantes x87 de X87Math.cpp (Veh83D*, Heli83GSinStore, Car84DAtan, Car84DCos)"})},
            {"virtual_fakes",json::array({"CVehicle::GetHeightAboveRoad (vtabla +0xD4, vtabla falsa compartida)"})},{"real_entries",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    for(const uintptr_t a:{uintptr_t(0x858EF0),uintptr_t(0x858B8C),uintptr_t(0x858CE8),uintptr_t(0x858CA0),uintptr_t(0x858624),uintptr_t(0x858C1C),uintptr_t(0x859004),uintptr_t(0x858F40)})env.watches.emplace_back(a,4);
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
