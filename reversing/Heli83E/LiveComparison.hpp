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
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Physical.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Pad.h"
#include "Camera.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Heli83E dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::ProcessControlInputs (0x6C4830, hook V: RH_ScopedVMTInstall; incluye el salto directo en la entrada del EXE). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el
// cuerpo de la DLL (sus hooks RH quedan apagados durante el banco):
//   CPad::GetAccelerate 0x5403F0, GetBrake 0x540080, GetSteeringLeftRight 0x53FB80, GetSteeringUpDown 0x53FBD0, GetLookRight 0x53FE10, GetLookLeft 0x53FDD0 y GetHorn 0x53FEE0 (thiscall, sin argumentos; AX o AL),
//   FindPlayerPed 0x56E210 (cdecl), CPlayerPed::KeepAreaAroundPlayerClear 0x60C1E0 (thiscall), CVector::Normalise 0x59C910 (thiscall, in situ) y CrossProduct 0x59C730 (cdecl, out/a/b).
//   Corren de verdad en ambos lados: CPad::GetPad (0x53FB70, aritmetica sobre CPad::Pads), las estaticas 0xB6EC2E, 0xC1CC03, 0xC1CC04, el raton 0xB73424/0xB73428, CTimer::ms_fTimeStep, el pow del CRT del EXE (0x822130, en el
//   candidato su emulacion PowScaleFloatCpp), la matriz del helicoptero y los datos de manejo en vuelo (arenas) y las dos palabras del mando que la funcion lee directamente (+0x2C y +0x10E; regiones restauradas tras cada lado).
// Los falsos registran sus argumentos EN ORDEN (punteros de arena por etiqueta, locales de pila por su marca). Los de salida (EAX de los del mando, Normalise, CrossProduct) escriben los valores que da el caso.

namespace H83ELive {
using namespace H83EFix;
inline constexpr uintptr_t kEntry = 0x6C4830;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CHeli,m_fLeftRightSkid)==0x98C&&offsetof(CHeli,m_fSteeringUpDown)==0x990&&offsetof(CHeli,m_fSteeringLeftRight)==0x994&&offsetof(CHeli,m_fAccelerationBreakStatus)==0x998);
static_assert(offsetof(CVehicle,m_fSteerAngle)==0x494&&offsetof(CVehicle,m_GasPedal)==0x49C&&offsetof(CVehicle,m_BrakePedal)==0x4A0&&offsetof(CVehicle,m_fHealth)==0x4C0&&offsetof(CVehicle,m_pFlyingHandlingData)==0x388);
static_assert(offsetof(CPhysical,m_vecMoveSpeed)==0x44&&offsetof(CPlaceable,m_matrix)==0x14);
static_assert(offsetof(tFlyingHandlingData,m_fRollStab)==0x1C&&offsetof(tFlyingHandlingData,m_fPitchStab)==0x24);
static_assert(offsetof(CPad,NewState)+offsetof(CControllerState,m_bVehicleMouseLook)==0x2C&&offsetof(CPad,DisablePlayerControls)==0x10E&&sizeof(CPad)==kPadSize);
static_assert(sizeof(CHeli)<=kVehWin);

// Disposicion de CMatrix (miembros privados: se mide con los accesores): derecha +0, adelante +0x10, arriba +0x20
inline bool MatrixLayoutOk() {
    CMatrix m;
    const auto base=uintptr_t(&m);
    return uintptr_t(&m.GetRight())-base==0x00&&uintptr_t(&m.GetForward())-base==0x10&&uintptr_t(&m.GetUp())-base==0x20;
}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA;
inline Arena<kFlyArena> flyA;       // tFlyingHandlingData (+0x1C estabilidad de balanceo, +0x24 de cabeceo)
inline Arena<kPlayerArena> playerA; // el CPlayerPed que devuelve FindPlayerPed

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
struct State {const C* c{};u32 evHash{};u32 unexpected{};u32 normCalls{};u32 crossCalls{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline u32 Raw(const void* p,unsigned i) {return BitsOf(static_cast<const u8*>(p)+4*i);}
// Eventos: 1 CVector::Normalise [&v, x, y, z] (entrada), 2 CrossProduct [&out, &a, &b, a(3), b(3)], 10 GetAccelerate, 11 GetBrake, 12 GetSteeringLeftRight, 13 GetSteeringUpDown, 14 GetLookRight, 15 GetLookLeft,
// 16 GetHorn [this], 20 FindPlayerPed [playerId], 21 KeepAreaAroundPlayerClear [this], 99 virtual inesperado
inline void __fastcall FakeNormalise(void* v,void*) {
    Ev& e=PushEv(1,4);e.v[0]=Tag(v);for(unsigned i=0;i<3;++i)e.v[1+i]=Raw(v,i);HashEv(e);
    const unsigned k=g.normCalls<2?g.normCalls:1;++g.normCalls;
    for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(v)+4*i,&g.c->nrm[k][i],4);
}
inline void* __cdecl FakeCross(void* out,void* a,void* b) {
    Ev& e=PushEv(2,9);e.v[0]=Tag(out);e.v[1]=Tag(a);e.v[2]=Tag(b);
    for(unsigned i=0;i<3;++i) {e.v[3+i]=Raw(a,i);e.v[6+i]=Raw(b,i);}
    HashEv(e);
    const unsigned k=g.crossCalls<2?g.crossCalls:1;++g.crossCalls;
    for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(out)+4*i,&g.c->crs[k][i],4);
    return out;
}
inline u32 PadEvent(u32 kind,void* self,u32 value) {Ev& e=PushEv(kind,1);e.v[0]=Tag(self);HashEv(e);return value;}
inline u32 __fastcall FakeAccelerate(void* self,void*) {return PadEvent(10,self,g.c->acc);}
inline u32 __fastcall FakeBrake(void* self,void*) {return PadEvent(11,self,g.c->brk);}
inline u32 __fastcall FakeSteerLR(void* self,void*) {return PadEvent(12,self,g.c->lr);}
inline u32 __fastcall FakeSteerUD(void* self,void*) {return PadEvent(13,self,g.c->ud);}
inline u32 __fastcall FakeLookRight(void* self,void*) {return PadEvent(14,self,g.c->lookR);}
inline u32 __fastcall FakeLookLeft(void* self,void*) {return PadEvent(15,self,g.c->lookL);}
inline u32 __fastcall FakeHorn(void* self,void*) {return PadEvent(16,self,g.c->horn);}
inline void* __cdecl FakeFindPlayerPed(int32 playerId) {Ev& e=PushEv(20,1);e.v[0]=u32(playerId);HashEv(e);return reinterpret_cast<void*>(playerA.Obj());}
inline void __fastcall FakeKeepArea(void* self,void*) {Ev& e=PushEv(21,1);e.v[0]=Tag(self);HashEv(e);}
inline u32 __fastcall FakeUnexpectedVirtual(void* self,void*) {++g.unexpected;Ev& e=PushEv(99,1);e.v[0]=Tag(self);HashEv(e);return 0;}
// vtabla falsa del helicoptero: ninguna entrada es valida (la funcion no llama a virtuales)
inline std::array<u32,96> vtbl{};
inline void InitVtbl() {for(auto& v:vtbl)v=u32(reinterpret_cast<uintptr_t>(&FakeUnexpectedVirtual));}

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
inline uintptr_t PadAddr(unsigned index) {return kPadsBase+kPadSize*index;}
inline void Put16At(uintptr_t address,u32 value) {const uint16 w=uint16(value);memcpy(reinterpret_cast<void*>(address),&w,2);}
inline u32 Get16At(uintptr_t address) {uint16 w;memcpy(&w,reinterpret_cast<void*>(address),2);return w;}

inline void CaseHeli(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*17u+1u);matA.Fill(c.seed*17u+2u);flyA.Fill(c.seed*17u+3u);playerA.Fill(c.seed*17u+4u);
        // matriz del helicoptero: derecha (+0), adelante (+0x10), arriba (+0x20)
        for(unsigned i=0;i<3;++i) {matA.Put32(4*i,c.mr[i]);matA.Put32(0x10+4*i,c.mf[i]);matA.Put32(0x20+4*i,c.mu[i]);}
        flyA.Put32(0x1C,c.roll);flyA.Put32(0x24,c.pitch);
        // helicoptero
        vehA.Put32(0,u32(uintptr_t(vtbl.data())));
        vehA.Put32(0x14,u32(matA.Obj()));
        for(unsigned i=0;i<3;++i)vehA.Put32(0x44+4*i,c.ms[i]);
        vehA.Put8(0x428,c.flags);
        vehA.Put32(0x388,u32(flyA.Obj()));
        vehA.Put32(0x494,c.steer0);vehA.Put32(0x49C,c.gas0);vehA.Put32(0x4A0,c.brk0);vehA.Put32(0x4C0,c.health);
        vehA.Put32(0x98C,c.skid0);vehA.Put32(0x990,c.ud0);vehA.Put32(0x994,c.lr0);vehA.Put32(0x998,c.acc0);
        // globales y las dos palabras del mando que la funcion lee directamente (mando usado y el otro)
        {const unsigned pad=c.arg&1u;
         Put16At(PadAddr(pad)+0x2C,c.vml);Put16At(PadAddr(pad)+0x10E,c.dpc);
         Put16At(PadAddr(pad^1u)+0x2C,c.vmlOther);Put16At(PadAddr(pad^1u)+0x10E,c.dpcOther);}
        *reinterpret_cast<uint8*>(kUseMouse3rd)=uint8(c.m3);
        *reinterpret_cast<uint8*>(kMouseFly)=uint8(c.fly);
        G32(kLastControl)=c.mode;
        G32(kMouseX)=c.mx;G32(kMouseY)=c.my;
        G32(kTimeStep)=c.ts;
        vehA.Snap();matA.Snap();flyA.Snap();playerA.Snap();
        TagClear();vehA.Tagged();matA.Tagged();flyA.Tagged();playerA.Tagged();TagAdd(reinterpret_cast<void*>(kPadsBase),kPadsBytes);
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        const u32 args[1]{c.arg};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["mode"]=G32(kLastControl);
        obs["out"]=json::array({vehA.Get32(0x98C),vehA.Get32(0x990),vehA.Get32(0x994),vehA.Get32(0x998),vehA.Get32(0x494),vehA.Get32(0x49C),vehA.Get32(0x4A0),u32(vehA.m[kGuard+0x428]),vehA.Get32(0x44),vehA.Get32(0x48),vehA.Get32(0x4C)});
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        obs["unexpected_virtuals"]=g.unexpected;
        {   // escrituras esperadas en el helicoptero: 0x428 (byte), 0x44..0x4F, 0x494, 0x49C, 0x4A0 y 0x98C..0x99B; todo lo demas y las otras arenas debe quedar igual
            auto changes=vehA.Changes();
            json rest=json::array();
            for(const auto& ch:changes) {
                const int64 o=ch[0].get<int64>();
                const bool known=o==0x428||(o>=0x44&&o<0x50)||(o>=0x494&&o<0x498)||(o>=0x49C&&o<0x4A4)||(o>=0x98C&&o<0x99C);
                if(!known)rest.push_back(ch);
            }
            obs["changes"]=json::array({rest,matA.Changes(),flyA.Changes(),playerA.Changes()});
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
            for(unsigned c=0;c<kCases;++c)CaseHeli(env,totals,c);
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

// Busca el hook registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada). Hooks Simple (direccion del EXE) y Virtual (direccion de la entrada en la vtabla del EXE);
// de un Virtual devuelve ademas el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato.
inline bool FindHook(uintptr_t exe,std::string& cat,std::string& name,uintptr_t* virtualBody=nullptr) {
    bool found=false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for(auto& h:c.Items()) {
            if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple
                &&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress())==exe) {
                cat=std::string(c.Name());name=std::string(h->Name());found=true;
            }
            else if(virtualBody&&h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                &&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress())==exe) {
                cat=std::string(c.Name());name=std::string(h->Name());found=true;
                *virtualBody=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
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
    if(!std::filesystem::exists(dir/"heli83e-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"heli83e-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("heli83e");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=MatrixLayoutOk();json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado (el Virtual apaga tambien el salto de la entrada del EXE). El cuerpo de la DLL (entrada de la vtabla de la DLL)
    // se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    {
        std::string cat,name;uintptr_t body=0;
        const bool found=FindHook(kEntry,cat,name,&body);
        if(found&&body)env.route[0].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry,body));
        registered[kFn[0]]=found&&env.route[0]&&env.route[0]->Registered();bodies[kFn[0]]=found&&env.route[0]&&env.route[0]->Body()!=0;
        ready&=found&&body&&env.route[0]&&env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[0]]=found?json({{"class",cat},{"name",name},{"address",kEntry}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){Run(env,false,registered,bodies,collaborators);return;}
    InitVtbl();
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    using Void=void*;
    env.stubs[0]={
        S(MakeCollab(env,0x5403F0,FnAddr(&CPad::GetAccelerate),hooksFound),Void(&FakeAccelerate)),
        S(MakeCollab(env,0x540080,FnAddr(&CPad::GetBrake),hooksFound),Void(&FakeBrake)),
        S(MakeCollab(env,0x53FB80,FnAddr(&CPad::GetSteeringLeftRight),hooksFound),Void(&FakeSteerLR)),
        S(MakeCollab(env,0x53FBD0,FnAddr(&CPad::GetSteeringUpDown),hooksFound),Void(&FakeSteerUD)),
        S(MakeCollab(env,0x53FE10,FnAddr(&CPad::GetLookRight),hooksFound),Void(&FakeLookRight)),
        S(MakeCollab(env,0x53FDD0,FnAddr(&CPad::GetLookLeft),hooksFound),Void(&FakeLookLeft)),
        S(MakeCollab(env,0x53FEE0,FnAddr(&CPad::GetHorn),hooksFound),Void(&FakeHorn)),
        S(MakeCollab(env,0x56E210,FnAddr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),hooksFound),Void(&FakeFindPlayerPed)),
        S(MakeCollab(env,0x60C1E0,FnAddr(&CPlayerPed::KeepAreaAroundPlayerClear),hooksFound),Void(&FakeKeepArea)),
        S(MakeCollab(env,0x59C910,FnAddr(&CVector::Normalise),hooksFound),Void(&FakeNormalise)),
        S(MakeCollab(env,0x59C730,FnAddr(static_cast<CVector*(*)(CVector*,CVector*,CVector*)>(&CrossProduct)),hooksFound),Void(&FakeCross))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CPad::GetPad (0x53FB70, aritmetica sobre CPad::Pads)","estaticas 0xB6EC2E, 0xC1CC03, 0xC1CC04, raton 0xB73424/0xB73428 y CTimer::ms_fTimeStep (regiones restauradas)","pow del CRT del EXE (0x822130) en el original; PowScaleFloatCpp en el candidato","matriz del helicoptero, datos de manejo en vuelo y palabras +0x2C/+0x10E del mando (arenas y regiones)"})},
            {"virtual_fakes",json::array()},{"real_entries",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
