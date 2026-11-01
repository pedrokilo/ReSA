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
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/PlayerPed.h"
#include "Core/Vector.h"
#include "Camera.h"
#include "Coronas.h"
#include "Shadows.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Heli83G dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::SearchLightCone (0x6C58E0, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (sus hooks RH quedan apagados durante el banco):
//   CVector::Normalise 0x59C910 (thiscall, in situ), CrossProduct 0x59C730 (cdecl, out/a/b), CWorld::ProcessLineOfSight 0x56BA00 (cdecl, 12), CCoronas::RegisterCorona 0x6FC580 (cdecl, 21),
//   CShadows::StoreShadowToBeRendered 0x707390 (cdecl, 16) y las tres de RenderWare RwIm3DTransform 0x7EF450, RwIm3DRenderIndexedPrimitive 0x7EF550 y RwIm3DEnd 0x7EF520 (sin hook: la DLL llama a la
//   direccion del EXE, que lleva el falso).
//   Corren de verdad en ambos lados: _ftol2 del EXE (0x821B40), fsin/fcos de la x87, TheCamera (puntero a la matriz y posicion), gpShadowExplosionTex y el bufer temporal de vertices e indices (regiones
//   restauradas tras cada lado).
// Los falsos registran sus argumentos EN ORDEN (punteros de arena por etiqueta, locales de pila por su marca; los parametros estrechos solo con sus bits validos). Normalise y CrossProduct calculan de verdad
// con los ayudantes x87 de X87Math (Veh83CNormalise, Veh83D*); ProcessLineOfSight, Im3DTransform y el resto escriben o devuelven los valores del caso.

namespace H83GLive {
using namespace H83GFix;
inline constexpr uintptr_t kEntry = 0x6C58E0;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CPlaceable,m_matrix)==0x14&&offsetof(CPlaceable,m_placement)==0x4);
static_assert(sizeof(RwIm3DVertex)==kVertexStride&&offsetof(RwIm3DVertex,color)==0x18);
static_assert(offsetof(CColPoint,m_vecPoint)==0);

// ================================================================================================ arenas
template<size_t N> struct ArenaT {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline ArenaT<kOutArena> outA;     // 0 useless0 (+0), useless1 (+0x10), useless2 (+0x20)
inline ArenaT<kCamArena> camA;     // 1 matriz de la camara (posicion +0x30)

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
struct State {const C* c{};u32 evHash{};u32 unexpected{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a kTraceKeep)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline u32 Raw(const void* p,unsigned i) {return BitsOf(static_cast<const u8*>(p)+4*i);}
// Eventos: 1 CVector::Normalise [&v, x, y, z] (entrada), 2 CrossProduct [&out, &a, &b, a(3), b(3)], 3 ProcessLineOfSight [&origen, &destino, &colpoint, &entidad, origen(3), destino(3), 8 booleanos],
// 4 RegisterCorona [id, adjunto, r, g, b, intensidad, &pos, pos(3), tamano, alcance, tipo, destello, reflejo, linea de vision, estela, angulo, neon, acercar, brillo inicial, desvanecer, desde abajo, nucleo],
// 5 StoreShadowToBeRendered [tipo, textura, &pos, pos(3), arriba x, arriba y, derecha x, derecha y, intensidad, r, g, b, distancia z, agua, escala, sombra en tiempo real, edificios],
// 6 RwIm3DTransform [vertices, numero, matriz, banderas], 7 RwIm3DRenderIndexedPrimitive [primitiva, indices, numero], 8 RwIm3DEnd, 99 inesperado
inline void __fastcall FakeNormalise(void* v,void*) {
    Ev& e=PushEv(1,4);e.v[0]=Tag(v);for(unsigned i=0;i<3;++i)e.v[1+i]=Raw(v,i);HashEv(e);
    (void)Veh83CNormalise(static_cast<float*>(v));   // CVector::NormaliseAndMag del EXE: normaliza en el sitio (x2 + y2) + z2 -> 1 / raiz
}
inline void* __cdecl FakeCross(void* out,void* a,void* b) {
    Ev& e=PushEv(2,9);e.v[0]=Tag(out);e.v[1]=Tag(a);e.v[2]=Tag(b);
    for(unsigned i=0;i<3;++i) {e.v[3+i]=Raw(a,i);e.v[6+i]=Raw(b,i);}
    HashEv(e);
    const float* p=static_cast<const float*>(a);const float* q=static_cast<const float*>(b);
    float* o=static_cast<float*>(out);
    // a x b: (ay*bz - az*by, az*bx - ax*bz, ax*by - ay*bx), cada producto a 24 bits y el resultado guardado como float
    const float x=Veh83DStore(Veh83DSub(Veh83DMul(p[1],q[2]),Veh83DMul(p[2],q[1])));
    const float y=Veh83DStore(Veh83DSub(Veh83DMul(p[2],q[0]),Veh83DMul(p[0],q[2])));
    const float z=Veh83DStore(Veh83DSub(Veh83DMul(p[0],q[1]),Veh83DMul(p[1],q[0])));
    o[0]=x;o[1]=y;o[2]=z;
    return out;
}
inline u32 __cdecl FakeLos(void* origin,void* target,void* col,void* entity,u32 b0,u32 b1,u32 b2,u32 b3,u32 b4,u32 b5,u32 b6,u32 b7) {
    Ev& e=PushEv(3,18);e.v[0]=Tag(origin);e.v[1]=Tag(target);e.v[2]=Tag(col);e.v[3]=Tag(entity);
    for(unsigned i=0;i<3;++i) {e.v[4+i]=Raw(origin,i);e.v[7+i]=Raw(target,i);}
    const u32 f[8]{b0,b1,b2,b3,b4,b5,b6,b7};
    for(unsigned i=0;i<8;++i)e.v[10+i]=f[i]&0xFFu;
    HashEv(e);
    if(g.c->los)for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(col)+4*i,&g.c->col[i],4);
    return g.c->los?1u:0u;
}
inline void __cdecl FakeCorona(u32 id,u32 attach,u32 r,u32 gr,u32 b,u32 a,void* pos,u32 size,u32 range,u32 type,u32 flare,u32 refl,u32 los,u32 trail,u32 angle,u32 neon,u32 pull,u32 full,u32 fade,u32 below,u32 white) {
    Ev& e=PushEv(4,24);
    e.v[0]=id;e.v[1]=Tag(uintptr_t(attach));e.v[2]=r&0xFFu;e.v[3]=gr&0xFFu;e.v[4]=b&0xFFu;e.v[5]=a&0xFFu;e.v[6]=Tag(pos);
    for(unsigned i=0;i<3;++i)e.v[7+i]=Raw(pos,i);
    e.v[10]=size;e.v[11]=range;e.v[12]=type&0xFFu;e.v[13]=flare&0xFFu;e.v[14]=refl&0xFFu;e.v[15]=los&0xFFu;e.v[16]=trail&0xFFu;e.v[17]=angle;e.v[18]=neon&0xFFu;e.v[19]=pull;e.v[20]=full&0xFFu;e.v[21]=fade;e.v[22]=below&0xFFu;e.v[23]=white&0xFFu;
    HashEv(e);
}
inline void __cdecl FakeShadow(u32 type,u32 tex,void* pos,u32 tx,u32 ty,u32 rx,u32 ry,u32 inten,u32 r,u32 gr,u32 b,u32 zd,u32 water,u32 scale,void* rts,u32 onb) {
    Ev& e=PushEv(5,19);
    e.v[0]=type&0xFFu;e.v[1]=tex;e.v[2]=Tag(pos);
    for(unsigned i=0;i<3;++i)e.v[3+i]=Raw(pos,i);
    e.v[6]=tx;e.v[7]=ty;e.v[8]=rx;e.v[9]=ry;e.v[10]=inten&0xFFFFu;e.v[11]=r&0xFFu;e.v[12]=gr&0xFFu;e.v[13]=b&0xFFu;e.v[14]=zd;e.v[15]=water&0xFFu;e.v[16]=scale;e.v[17]=Tag(rts);e.v[18]=onb&0xFFu;
    HashEv(e);
}
inline u32 __cdecl FakeIm3DTransform(u32 verts,u32 n,u32 ltm,u32 flags) {
    Ev& e=PushEv(6,4);e.v[0]=verts;e.v[1]=n;e.v[2]=ltm;e.v[3]=flags;HashEv(e);
    return g.c->transformOk?verts:0u;
}
inline u32 __cdecl FakeIm3DRender(u32 prim,u32 idx,u32 n) {Ev& e=PushEv(7,3);e.v[0]=prim;e.v[1]=idx;e.v[2]=n;HashEv(e);return 1u;}
inline u32 __cdecl FakeIm3DEnd() {Ev& e=PushEv(8,0);HashEv(e);return 1u;}

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
inline std::string VertexHex(unsigned count) {
    std::string out;
    for(unsigned v=0;v<count&&v<kMaxVertices;++v) {
        const u8* p=reinterpret_cast<const u8*>(kTempVertices+kVertexStride*v);
        out+=HexBytes(p,12);out+=HexBytes(p+0x18,4);
    }
    return out;
}
inline void CaseLight(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        outA.Fill(c.seed*17u+1u);camA.Fill(c.seed*17u+2u);
        FillWords(reinterpret_cast<u8*>(kTempVertices),kTempVerticesBytes,c.seed*17u+3u);
        FillWords(reinterpret_cast<u8*>(kTempIndices),kTempIndicesBytes,c.seed*17u+4u);
        G32(kTempCounters)=SeedReg(c.n,5);G32(kTempCounters+4)=SeedReg(c.n,6);   // la funcion los pone a 0
        for(unsigned i=0;i<3;++i)camA.Put32(0x30+4*i,c.camMat[i]);
        G32(kCamMatrixPtr)=c.camNull?0u:u32(camA.Obj());
        memcpy(reinterpret_cast<void*>(kCamPlacement),c.camPos,12);
        G32(kShadowTex)=c.tex;
        outA.Snap();camA.Snap();
        TagClear();outA.Tagged();camA.Tagged();
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        const u32 args[19]{c.corona,c.o[0],c.o[1],c.o[2],c.t[0],c.t[1],c.t[2],c.radius,c.power,c.clip,c.shadow,u32(outA.Obj(kGuard)),u32(outA.Obj(kGuard+0x10)),u32(outA.Obj(kGuard+0x20)),c.a11,c.base,c.a13,c.a14,c.a15};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,19,c.eax,c.ecx,c.edx,c.stack,76);});
        json obs;
        const u32 vcount=G32(kTempCounters),icount=G32(kTempCounters+4);
        obs["counters"]=json::array({vcount,icount});
        obs["vertices"]=VertexHex(vcount);
        obs["indices"]=HexBytes(reinterpret_cast<const void*>(kTempIndices),2*(icount<240u?icount:240u));
        {
            json w=json::array();
            for(const u32 off:{0u,4u,8u,0x10u,0x14u,0x18u,0x20u,0x24u,0x28u})w.push_back(outA.Get32(off));
            obs["out"]=w;
        }
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        obs["unexpected_virtuals"]=g.unexpected;
        {   // las unicas escrituras esperadas en las arenas son las nueve palabras de salida
            json rest=json::array();
            for(const auto& ch:outA.Changes()) {
                const int64 o=ch[0].get<int64>();
                const bool known=(o>=0&&o<12)||(o>=0x10&&o<0x1C)||(o>=0x20&&o<0x2C);
                if(!known)rest.push_back(ch);
            }
            obs["changes"]=json::array({rest,camA.Changes()});
        }
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    ready&=!ReversingProbe::JumpsToDll(0x821B40);   // _ftol2 del EXE en ambos lados
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
            for(unsigned c=0;c<kCases;++c)CaseLight(env,totals,c);
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
// (llamadas directamente por la DLL) llevan el mismo falso. dllFunction = 0: la DLL llama a la direccion del EXE (sin cuerpo propio).
inline Pair MakeCollab(Env& env,uintptr_t exe,uintptr_t dllFunction,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
    }
    else hooksFound.push_back({{"exe",exe},{"class",nullptr},{"name",nullptr}});
    p.dll=dllFunction?Body(dllFunction):0;
    return p;
}
template<class T> inline uintptr_t FnAddr(T f) {
    if constexpr (std::is_member_function_pointer_v<T>) return std::bit_cast<uintptr_t>(f); else return reinterpret_cast<uintptr_t>(f);
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"heli83g-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"heli83g-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("heli83g");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
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
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    using Void=void*;
    const auto los=static_cast<bool(*)(const CVector&,const CVector&,CColPoint&,CEntity*&,bool,bool,bool,bool,bool,bool,bool,bool)>(&CWorld::ProcessLineOfSight);
    const auto corona=static_cast<void(*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector&,float,float,eCoronaType,eCoronaFlareType,eCoronaReflType,eCoronaLOSCheck,eCoronaTrail,float,bool,float,bool,float,bool,bool)>(&CCoronas::RegisterCorona);
    const auto shadow=static_cast<void(*)(uint8,RwTexture*,const CVector&,float,float,float,float,int16,uint8,uint8,uint8,float,bool,float,CRealTimeShadow*,bool)>(&CShadows::StoreShadowToBeRendered);
    env.stubs[0]={
        S(MakeCollab(env,0x59C910,FnAddr(&CVector::Normalise),hooksFound),Void(&FakeNormalise)),
        S(MakeCollab(env,0x59C730,FnAddr(static_cast<CVector*(*)(CVector*,CVector*,CVector*)>(&CrossProduct)),hooksFound),Void(&FakeCross)),
        S(MakeCollab(env,0x56BA00,FnAddr(los),hooksFound),Void(&FakeLos)),
        S(MakeCollab(env,0x6FC580,FnAddr(corona),hooksFound),Void(&FakeCorona)),
        S(MakeCollab(env,0x707390,FnAddr(shadow),hooksFound),Void(&FakeShadow)),
        S(MakeCollab(env,0x7EF450,0,hooksFound),Void(&FakeIm3DTransform)),
        S(MakeCollab(env,0x7EF550,0,hooksFound),Void(&FakeIm3DRender)),
        S(MakeCollab(env,0x7EF520,0,hooksFound),Void(&FakeIm3DEnd))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"_ftol2 del EXE (0x821B40)","fsin/fcos de la x87","TheCamera (puntero a la matriz y posicion), gpShadowExplosionTex y el bufer temporal de vertices e indices (regiones restauradas)"})},
            {"virtual_fakes",json::array()},{"real_entries",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    env.watches.emplace_back(0x821B40,16);   // _ftol2 real del EXE (sin falso)
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
