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
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/PlayerPed.h"
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Audio/AudioEngine.h"
#include "SurfaceInfos_c.h"
#include "WaterLevel.h"
#include "Skidmarks.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Veh83C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::AddSingleWheelParticles (0x6DE880, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (sus hooks RH, si existen, quedan apagados durante el banco):
//   FindPlayerVehicle 0x56E0D0, SurfaceInfos_c::IsWater 0x55E710, CWaterLevel::GetWaterLevel 0x6EB690, SurfaceInfos_c::GetFrictionEffect 0x55E650, Fx_c::AddSparks 0x49F040, FindPlayerPed 0x56E210,
//   CAudioEngine::ReportCollision 0x506EB0, CVehicle::MakeDirty 0x6D2BF0, CVehicle::AddWheelDirtAndWater 0x6D2D50, FxSystem_c::AddParticle (v3d) 0x4AA440 y CSkidmarks::RegisterOne 0x720930.
//   El virtual FindWheelWidth (+0xFC) lo atiende la vtabla falsa del vehiculo. Corren de verdad en ambos lados: rand() del CRT del EXE (0x821B1E), CVector::Magnitude/NormaliseAndMag, el constructor de FxPrtMult_c
//   y la aritmetica de vectores (0x40FEC0, 0x40FE60, 0x411A00, 0x406D70).
// Los falsos registran sus argumentos EN ORDEN (punteros de arena por etiqueta, locales de pila por su marca). Las arenas (guarda + objeto + guarda) contienen el vehiculo (0x990), su matriz, la matriz de la camara,
// dos sistemas de particulas, el CColPoint, la posicion de la rueda, el byte de sangre y los punteros de FindPlayerPed / FindPlayerVehicle.

namespace V83CLive {
using namespace V83CFix;
inline constexpr uintptr_t kEntry = 0x6DE880;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CVehicle,m_nVehicleSubType)==0x594&&offsetof(CVehicle,m_GasPedal)==0x49C);
static_assert(offsetof(CPhysical,m_vecMoveSpeed)==0x44&&offsetof(CPhysical,m_fContactSurfaceBrightness)==0x12C);
static_assert(offsetof(CPlaceable,m_matrix)==0x14&&offsetof(CEntity,m_nModelIndex)==0x22&&offsetof(CEntity,m_nFlags)==0x1C);
static_assert(sizeof(CAutomobile)<=kVehWin);

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(size_t off,u32 v) {const uint16 w=uint16(v);memcpy(m.data()+kGuard+off,&w,2);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    void SetBit(size_t off,unsigned bit,bool on) {uint8& b=m[kGuard+off];b=uint8(on?(b|(1u<<bit)):(b&~(1u<<bit)));}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA;
inline Arena<kCamArena> matCam;
inline Arena<kSysArena> sysA,sysB;
inline Arena<kColArena> colA;
inline Arena<kVecArena> posA;
inline Arena<kByteArena> bloodA;
inline Arena<kPlArena> plA,pvA;

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
// La camara: con matriz, la posicion usada esta en +0x30 de la matriz y la compacta (+4) es ruido; sin matriz (puntero nulo), al reves.
inline void SetCamera(bool hasMat,const u32* used,const u32* other) {
    for(unsigned i=0;i<3;++i) {G32(kCamPlacement+4*i)=hasMat?other[i]:used[i];matCam.Put32(0x30+4*i,hasMat?used[i]:other[i]);}
    G32(kCamMatrixPtr)=hasMat?u32(matCam.Obj()):0u;
}

// ================================================================================================ falsos de los colaboradores
struct State {const C* c{};u32 evHash{};u32 unexpected{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline u32 Raw(const void* p,unsigned i) {return BitsOf(static_cast<const u8*>(p)+4*i);}
// Eventos: 1 FindPlayerVehicle [id, bandera], 2 IsWater [superficie], 3 GetWaterLevel [x, y, z, &nivel, bandera, vector], 4 GetFrictionEffect [superficie],
// 5 AddSparks [&pos, pos(3), dir(3), fuerza, cantidad, across(3), tipo, dispersion, vida], 6 FindPlayerPed [id], 7 ReportCollision [e1, e2, sup1, sup2, &pos, &normal, i1, i2, b1, b2],
// 8 MakeDirty [this, &colpoint], 9 AddWheelDirtAndWater [this, &colpoint, a, b, c], 10 AddParticle [sistema, &pos, pos(3), vel(3), tiempo, mult(7), rotZ, luz, limite, bandera],
// 11 RegisterOne [id - this, &pos, pos(3), dirX, dirY, tipo, &sangre, longitud], 12 FindWheelWidth [this, trasera], 99 virtual inesperado
inline void* __cdecl FakeFindVeh(u32 id,u32 flag) {
    Ev& e=PushEv(1,2);e.v[0]=id;e.v[1]=flag;HashEv(e);
    return reinterpret_cast<void*>(g.c->pvKind==0?0:pvA.Obj());
}
inline u32 __fastcall FakeIsWater(void*,void*,u32 id) {Ev& e=PushEv(2,1);e.v[0]=id;HashEv(e);return g.c->isWater;}
inline u32 __cdecl FakeWaterLevel(u32 x,u32 y,u32 z,u32* out,u32 flag,void* vec) {
    Ev& e=PushEv(3,6);e.v[0]=x;e.v[1]=y;e.v[2]=z;e.v[3]=Tag(out);e.v[4]=flag;e.v[5]=Tag(vec);HashEv(e);
    *out=g.c->waterLevel;
    return g.c->waterRet;
}
inline u32 __fastcall FakeFriction(void*,void*,u32 id) {Ev& e=PushEv(4,1);e.v[0]=id;HashEv(e);return g.c->friction;}
inline void __fastcall FakeSparks(void*,void*,const void* pos,const void* dir,u32 force,u32 count,u32 ax,u32 ay,u32 az,u32 blur,u32 spread,u32 life) {
    Ev& e=PushEv(5,15);e.v[0]=Tag(pos);
    for(unsigned i=0;i<3;++i) {e.v[1+i]=Raw(pos,i);e.v[4+i]=Raw(dir,i);}
    e.v[7]=force;e.v[8]=count;e.v[9]=ax;e.v[10]=ay;e.v[11]=az;e.v[12]=blur;e.v[13]=spread;e.v[14]=life;
    HashEv(e);
}
inline void* __cdecl FakeFindPed(u32 id) {Ev& e=PushEv(6,1);e.v[0]=id;HashEv(e);return reinterpret_cast<void*>(g.c->ppKind==0?0:plA.Obj());}
inline void __fastcall FakeReport(void*,void*,void* e1,void* e2,u32 s1,u32 s2,const void* pos,const void* normal,u32 i1,u32 i2,u32 b1,u32 b2) {
    Ev& e=PushEv(7,10);e.v[0]=Tag(e1);e.v[1]=Tag(e2);e.v[2]=s1&0xFFu;e.v[3]=s2&0xFFu;e.v[4]=Tag(pos);e.v[5]=Tag(normal);e.v[6]=i1;e.v[7]=i2;e.v[8]=b1&0xFFu;e.v[9]=b2&0xFFu;HashEv(e);
}
inline void __fastcall FakeMakeDirty(void* self,void*,void* cp) {Ev& e=PushEv(8,2);e.v[0]=Tag(self);e.v[1]=Tag(cp);HashEv(e);}
inline u32 __fastcall FakeDirt(void* self,void*,void* cp,u32 a,u32 b,u32 c) {
    Ev& e=PushEv(9,5);e.v[0]=Tag(self);e.v[1]=Tag(cp);e.v[2]=a&0xFFu;e.v[3]=b&0xFFu;e.v[4]=c&0xFFu;HashEv(e);
    return g.c->dirtRet;
}
inline void __fastcall FakeAddParticle(void* self,void*,const void* pos,const void* vel,u32 ts,const void* mult,u32 rotZ,u32 light,u32 limit,u32 flag) {
    Ev& e=PushEv(10,20);
    e.v[0]=Tag(self);e.v[1]=Tag(pos);
    for(unsigned i=0;i<3;++i) {e.v[2+i]=Raw(pos,i);e.v[5+i]=Raw(vel,i);}
    e.v[8]=ts;
    for(unsigned i=0;i<7;++i) e.v[9+i]=Raw(mult,i);
    e.v[16]=rotZ;e.v[17]=light;e.v[18]=limit;e.v[19]=flag&0xFFu;
    HashEv(e);
    if(g.c->swapSlot)G32(kSlotSmoke)=G32(kSlotSmoke)==u32(sysA.Obj())?u32(sysB.Obj()):u32(sysA.Obj());
}
inline void __cdecl FakeRegister(u32 id,const void* pos,u32 dx,u32 dy,u32 type,void* blood,u32 length) {
    Ev& e=PushEv(11,10);e.v[0]=id-u32(vehA.Obj());e.v[1]=Tag(pos);
    for(unsigned i=0;i<3;++i) e.v[2+i]=Raw(pos,i);
    e.v[5]=dx;e.v[6]=dy;e.v[7]=type;e.v[8]=Tag(blood);e.v[9]=length;
    HashEv(e);
}
inline float __fastcall FakeWidth(void* self,void*,u32 rear) {Ev& e=PushEv(12,2);e.v[0]=Tag(self);e.v[1]=rear&0xFFu;HashEv(e);return F(g.c->width);}
inline u32 __fastcall FakeUnexpectedVirtual(void* self,void*) {++g.unexpected;Ev& e=PushEv(99,1);e.v[0]=Tag(self);HashEv(e);return 0;}
// vtabla falsa del vehiculo: solo +0xFC (63, FindWheelWidth) es valida
inline std::array<u32,96> vtbl{};
inline void InitVtbl() {
    for(auto& v:vtbl)v=u32(reinterpret_cast<uintptr_t>(&FakeUnexpectedVirtual));
    vtbl[63]=u32(reinterpret_cast<uintptr_t>(&FakeWidth));
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
inline void CaseWheel(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*17u+1u);matA.Fill(c.seed*17u+2u);matCam.Fill(c.seed*17u+3u);sysA.Fill(c.seed*17u+4u);sysB.Fill(c.seed*17u+5u);
        colA.Fill(c.seed*17u+6u);posA.Fill(c.seed*17u+7u);bloodA.Fill(c.seed*17u+8u);plA.Fill(c.seed*17u+9u);pvA.Fill(c.seed*17u+10u);
        for(unsigned i=0;i<12;++i) matA.Put32(kMatOff[i],c.mat[i]);
        // vehiculo
        vehA.Put32(0,u32(uintptr_t(vtbl.data())));
        vehA.Put32(0x14,u32(matA.Obj()));
        vehA.Put16(0x22,c.model);
        vehA.Put32(0x1C,(Rand(c.seed,31)&~0x20000u)|(c.offscreen?0x20000u:0u));
        vehA.Put32(0x40,(Rand(c.seed,32)&~0x8000000u)|(c.touchWater?0x8000000u:0u));
        vehA.SetBit(0x42D,0,c.alwaysSkid);
        for(unsigned i=0;i<3;++i) vehA.Put32(0x44+4*i,c.vel[i]);
        vehA.Put32(0x12C,c.bright);
        vehA.Put32(0x49C,c.gas);
        vehA.Put32(0x594,c.subtype);
        colA.Put8(0x20,c.surfA);colA.Put8(0x23,c.surfB);
        for(unsigned i=0;i<3;++i) posA.Put32(4*i,c.pos[i]);
        bloodA.Put8(0,c.blood);
        SetCamera(c.hasMat,c.cam,c.camOther);
        G32(kSlotSmoke)=u32(sysA.Obj());
        G32(kFrameCounter)=c.frame;
        G32(kTimeStep)=c.timestep;
        vehA.Snap();matA.Snap();matCam.Snap();sysA.Snap();sysB.Snap();colA.Snap();posA.Snap();bloodA.Snap();plA.Snap();pvA.Snap();
        TagClear();vehA.Tagged();matA.Tagged();matCam.Tagged();sysA.Tagged();sysB.Tagged();colA.Tagged();posA.Tagged();bloodA.Tagged();plA.Tagged();pvA.Tagged();
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        *RandStatePtr()=c.randSeed;   // mismo estado inicial del rand() real del EXE en ambos lados
        const u32 args[11]{c.ws,c.wstatus,c.a2,c.a3,u32(colA.Obj()),u32(posA.Obj()),c.a6,c.idx,c.skid,u32(bloodA.Obj()),c.flags};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,11,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["ret_al"]=sides[s].reg.eax&0xFFu;
        obs["rand_state"]=*RandStatePtr();
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        obs["system_slot"]=Tag(uintptr_t(G32(kSlotSmoke)));
        obs["unexpected_virtuals"]=g.unexpected;
        obs["changes"]=json::array({vehA.Changes(),matA.Changes(),matCam.Changes(),sysA.Changes(),sysB.Changes(),colA.Changes(),posA.Changes(),bloodA.Changes(),plA.Changes(),pvA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    ready&=NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D);   // rand real del EXE en ambos lados
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
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},{"rng_native",NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D)},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseWheel(env,totals,c);
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
    if(!std::filesystem::exists(dir/"veh83c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"veh83c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("veh83c");}
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
    InitVtbl();
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    using Void=void*;
    env.stubs[0]={
        S(MakeCollab(env,0x56E0D0,FnAddr(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),hooksFound),Void(&FakeFindVeh)),
        S(MakeCollab(env,0x55E710,FnAddr(&SurfaceInfos_c::IsWater),hooksFound),Void(&FakeIsWater)),
        S(MakeCollab(env,0x6EB690,FnAddr(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),hooksFound),Void(&FakeWaterLevel)),
        S(MakeCollab(env,0x55E650,FnAddr(&SurfaceInfos_c::GetFrictionEffect),hooksFound),Void(&FakeFriction)),
        S(MakeCollab(env,0x49F040,FnAddr(&Fx_c::AddSparks),hooksFound),Void(&FakeSparks)),
        S(MakeCollab(env,0x56E210,FnAddr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),hooksFound),Void(&FakeFindPed)),
        S(MakeCollab(env,0x506EB0,FnAddr(&CAudioEngine::ReportCollision),hooksFound),Void(&FakeReport)),
        S(MakeCollab(env,0x6D2BF0,FnAddr(&CVehicle::MakeDirty),hooksFound),Void(&FakeMakeDirty)),
        S(MakeCollab(env,0x6D2D50,FnAddr(&CVehicle::AddWheelDirtAndWater),hooksFound),Void(&FakeDirt)),
        S(MakeCollab(env,0x4AA440,FnAddr(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),hooksFound),Void(&FakeAddParticle)),
        S(MakeCollab(env,0x720930,FnAddr(static_cast<void(*)(uint32,const CVector&,float,float,eSkidmarkType,bool*,float)>(&CSkidmarks::RegisterOne)),hooksFound),Void(&FakeRegister))};
    // Entradas del EXE que la DLL tiene enganchadas y que corren de verdad en ambos lados: sus hooks se apagan (el lado original ejecuta los bytes del EXE, incluido el rand() del EXE en 0x41BD90)
    json realEntries=json::array();
    for(const uintptr_t a:{uintptr_t(0x41BD90),uintptr_t(0x4082C0),uintptr_t(0x59C970),uintptr_t(0x4AB290)}) {
        std::string cat,name;
        const bool hooked=FindHook(a,cat,name);
        if(hooked)env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),a,HookRoute::RegisteredTarget{}));
        env.watches.emplace_back(a,16);
        ready&=!ReversingProbe::JumpsToDll(a);
        realEntries.push_back({{"exe",a},{"was_hooked",hooked}});
    }
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"rand() del CRT del EXE (0x821B1E)","CVector::Magnitude / NormaliseAndMag","FxPrtMult_c (constructor)","aritmetica de vectores (0x40FEC0, 0x40FE60, 0x411A00, 0x406D70)","TheCamera, CTimer (regiones restauradas)"})},
            {"virtual_fakes",json::array({"FindWheelWidth +0xFC"})},{"real_entries",realEntries},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    env.watches.emplace_back(0x821B1E,16);   // rand real del EXE (sin falso)
    env.watches.emplace_back(0x827B3D,16);   // getptd
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
