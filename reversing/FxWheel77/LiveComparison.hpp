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
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "SimpleTransform.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FxWheel77 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Ocho funciones de Fx_c (hook S): AddWheelDust, AddWheelSand, AddGlass, AddDebris, AddWheelGrass, AddWheelGravel, AddWheelMud (y TriggerGunshot). Colaboradores sustituidos por falsos identicos en la direccion
// del EXE y en el cuerpo de la DLL: FxSystem_c::AddParticle(v3d) 0x4AA440, FxSystem_c::EnablePrim 0x4AA610, FindPlayerPed 0x56E210, FindPlayerVehicle 0x56E0D0. rand() del EXE (0x821B1E) sin falso.
// Los constructores FxPrtMult_c 0x4AB270 / 0x4AB290 corren de verdad en cada lado (el EXE los suyos, la DLL los de FxPrtMult.cpp).

namespace Fw77Live {
using namespace Fw77Fix;
inline constexpr uintptr_t kEntry[kFunctions]{0x4A09C0, 0x4A0610, 0x49F970, 0x49F750, 0x49FF20, 0x4A0170, 0x4A03C0, 0x4A0DE0};
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(size_t off,u32 v) {const u16 w=u16(v);memcpy(m.data()+kGuard+off,&w,2);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVecArena> posA;
inline Arena<kColArena> colA;
inline Arena<kMatArena> matCam;
inline Arena<kSysArena> sysA,sysB;
inline Arena<kThisArena> thisA;
inline Arena<kVehArena> vehA;
inline Arena<kPedArena> pedA,pedB;
inline Arena<kEntArena> entA;
inline Arena<kMatArena> matE;       // matriz de la entidad (posicion en +0x30)
inline Arena<kRwArena> rwA;
inline Arena<kFrameArena> frA;
inline Arena<kRwMatArena> matC;     // matriz que devuelve FxRwMatrixCreate
inline Arena<kVecArena> dirA;       // vector destino de TriggerGunshot

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
struct State {const C* c{};u32 evHash{};u32 particleCalls{};u32 fxCalls{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
// 0x4AA440: FxSystem_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& mult, float rotZ, float light, float limit, bool createLocal), thiscall, `ret 20h`.
// Registra el sistema (ECX), la direccion del vector de posicion (etiquetada) y TODOS los valores; con swap_slot cambia el sistema del hueco de g_fx de la funcion entre dos sistemas (el EXE lo relee en cada vuelta).
inline void __fastcall FakeAddParticle(void* self,void*,const void* pos,const void* vel,u32 timeSince,const void* mult,u32 rotZ,u32 light,u32 limit,u32 createLocal) {
    Ev& e=PushEv(31,20);
    e.v[0]=Tag(self);e.v[1]=Tag(pos);
    for(unsigned i=0;i<3;++i) {e.v[2+i]=BitsOf(static_cast<const u8*>(pos)+4*i);e.v[5+i]=BitsOf(static_cast<const u8*>(vel)+4*i);}
    e.v[8]=timeSince;
    for(unsigned i=0;i<7;++i) e.v[9+i]=BitsOf(static_cast<const u8*>(mult)+4*i);
    e.v[16]=rotZ;e.v[17]=light;e.v[18]=limit;e.v[19]=createLocal;
    HashEv(e);
    ++g.particleCalls;
    const uintptr_t slot=SlotOf(g.c->fn);
    if(g.c->swapSlot)G32(slot)=G32(slot)==u32(sysA.Obj())?u32(sysB.Obj()):u32(sysA.Obj());
}
// 0x4AA610: FxSystem_c::EnablePrim(int primIndex, bool enable), thiscall, `ret 8`.
inline void __fastcall FakeEnablePrim(void* self,void*,u32 index,u32 enable) {
    Ev& e=PushEv(32,3);
    e.v[0]=Tag(self);e.v[1]=index;e.v[2]=enable;
    HashEv(e);
}
// 0x56E210: FindPlayerPed(int playerId), cdecl. Devuelve el ped A (id 0), el B (id 1) o nulo segun el caso.
inline void* __cdecl FakePlayerPed(u32 id) {
    Ev& e=PushEv(12,1);e.v[0]=id;HashEv(e);
    return reinterpret_cast<void*>(id==0?(g.c->ped0Kind?pedA.Obj():0):id==1?(g.c->ped1Kind?pedB.Obj():0):0);
}
// 0x56E0D0: FindPlayerVehicle(int playerId, bool includeRemote), cdecl.
inline void* __cdecl FakePlayerVehicle(u32 id,u32 remote) {
    Ev& e=PushEv(13,2);e.v[0]=id;e.v[1]=remote;HashEv(e);
    return reinterpret_cast<void*>(g.c->pvKind==0?0:g.c->pvKind==1?vehA.Obj():0xDEAD1000u);
}

// ---- TriggerGunshot (0x4A0DE0): colaboradores
// 0x54F560 CPlaceable::AllocateMatrix() thiscall: crea la matriz de la entidad (el falso pone la matriz falsa en +0x14).
inline void __fastcall FakeAllocMatrix(void* self,void*) {
    Ev& e=PushEv(21,1);e.v[0]=Tag(self);HashEv(e);
    const u32 matrixPtr=u32(matE.Obj());
    memcpy(static_cast<u8*>(self)+0x14,&matrixPtr,4);
}
// 0x54F1B0 CSimpleTransform::UpdateMatrix(CMatrix* out) thiscall `ret 4`. En el EXE es codigo del EXE; en la DLL, el cuerpo de CSimpleTransform::UpdateMatrix (sin hook RH: se parchea el cuerpo por su direccion).
inline void __fastcall FakeUpdateMatrix(void* self,void*,void* out) {
    Ev& e=PushEv(22,2);e.v[0]=Tag(self);e.v[1]=Tag(out);HashEv(e);
}
// 0x4A9440 FxManager_c::FxRwMatrixCreate() thiscall
inline void* __fastcall FakeRwCreate(void* self,void*) {
    Ev& e=PushEv(23,1);e.v[0]=Tag(self);HashEv(e);
    return g.c->createdKind?reinterpret_cast<void*>(matC.Obj()):nullptr;
}
// 0x4A9460 FxManager_c::FxRwMatrixDestroy(RwMatrix*) thiscall `ret 4`
inline void __fastcall FakeRwDestroy(void* self,void*,void* mat) {
    Ev& e=PushEv(24,2);e.v[0]=Tag(self);e.v[1]=Tag(mat);HashEv(e);
}
// 0x49E950 Fx_c::CreateMatFromVec(RwMatrix* out, const CVector* origin, const CVector* direction) thiscall `ret 0Ch`
inline void __fastcall FakeCreateMat(void* self,void*,void* mat,const void* origin,const void* dir) {
    Ev& e=PushEv(25,10);e.v[0]=Tag(self);e.v[1]=Tag(mat);e.v[2]=Tag(origin);e.v[3]=Tag(dir);
    for(unsigned i=0;i<3;++i) {e.v[4+i]=BitsOf(static_cast<const u8*>(origin)+4*i);e.v[7+i]=BitsOf(static_cast<const u8*>(dir)+4*i);}
    HashEv(e);
}
// 0x4A9BE0 FxManager_c::CreateFxSystem(const char* name, const RwV3d* offset, RwMatrix* parent, bool constTime) thiscall `ret 10h`. El nombre se identifica por su contenido (el puntero es del EXE o de la DLL).
inline void* __fastcall FakeCreateFx(void* self,void*,const char* name,const void* offset,void* matrix,u32 flag) {
    Ev& e=PushEv(26,8);e.v[0]=Tag(self);e.v[1]=FnvStr(name);e.v[2]=Tag(offset);
    for(unsigned i=0;i<3;++i)e.v[3+i]=BitsOf(static_cast<const u8*>(offset)+4*i);
    e.v[6]=Tag(matrix);e.v[7]=flag;HashEv(e);
    const u32 kind=g.fxCalls++==0?g.c->fxA:g.c->fxB;
    return kind==0?nullptr:reinterpret_cast<void*>(kind==1?sysA.Obj():sysB.Obj());
}
// 0x4AA890 FxSystem_c::CopyParentMatrix() y 0x4AA3D0 FxSystem_c::PlayAndKill(), thiscall sin argumentos
inline void __fastcall FakeCopyParent(void* self,void*) {Ev& e=PushEv(27,1);e.v[0]=Tag(self);HashEv(e);}
inline void __fastcall FakePlayAndKill(void* self,void*) {Ev& e=PushEv(28,1);e.v[0]=Tag(self);HashEv(e);}

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
    // NaN silenciosos y de senal con cargas utiles distintas (la de mayor mantisa gana; con iguales y distinto signo no se compara), infinitos, ceros, normales y desnormales
    static constexpr u32 vals[]{0x7FC00001u,0xFFC00002u,0x7FC12345u,0xFFC54321u,0x7F800001u,0xFFA00003u,0x7FA00000u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x40A00000u,0x00000001u,0x007FFFFFu,0x7F7FFFFFu,
        0xFFC00001u,0x7FC00002u,0x7FC00000u,0xFFC00000u,0xFF800001u,0x7FA00003u,0xFF800002u,0x7F800002u};   // incluye parejas de igual mantisa y signo distinto
    for(u32 op=0;op<4;++op)for(u32 a:vals)for(u32 b:vals)out.push_back({op,a,b,X87Micro(op,a,b)});
    static constexpr u32 unary[]{0x7FC00001u,0x7F800001u,0xFFC12345u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x40000000u,0x40800000u,0x00000001u,0x007FFFFFu,0x7F7FFFFFu,0x41F00000u,0x4F000000u,0x4F800000u,0x5F000000u,0x5F7FFFFFu,0xCF000000u,0x4EFFFFFFu,0x2F800000u,0x3F7FFFFFu,0xBF7FFFFFu,0x4B000001u,0x5E800000u};
    for(u32 op:{4u,5u,6u})for(u32 a:unary)out.push_back({op,a,0u,X87Micro(op,a,0u)});
    RestoreFloatingState();
    return out;
}

// ================================================================================================ el caso
inline void CaseFx(Env& env,Totals& t,unsigned fnIdx,unsigned n) {
    const C c=MakeC(fnIdx,n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        posA.Fill(c.seed*11u+1u);colA.Fill(c.seed*11u+2u);matCam.Fill(c.seed*11u+3u);sysA.Fill(c.seed*11u+4u);sysB.Fill(c.seed*11u+5u);thisA.Fill(c.seed*11u+6u);
        vehA.Fill(c.seed*11u+7u);pedA.Fill(c.seed*11u+8u);pedB.Fill(c.seed*11u+9u);
        entA.Fill(c.seed*11u+10u);matE.Fill(c.seed*11u+11u);rwA.Fill(c.seed*11u+12u);frA.Fill(c.seed*11u+13u);matC.Fill(c.seed*11u+14u);dirA.Fill(c.seed*11u+15u);
        for(unsigned i=0;i<3;++i) posA.Put32(4*i,c.pos[i]);
        colA.Put32(0,c.color);
        if(fnIdx==kGunshot) {
            // entidad: transformacion simple (+4), matriz (+0x14, con la posicion en +0x30) y objeto RenderWare (+0x18, padre en +4, matriz de modelado en el marco +0x10)
            for(unsigned i=0;i<3;++i) {entA.Put32(4+4*i,c.hasEntMatrix?c.eother[i]:c.epos[i]);matE.Put32(0x30+4*i,c.hasEntMatrix?c.epos[i]:c.eother[i]);dirA.Put32(4*i,c.target[i]);}
            entA.Put32(0x14,c.hasEntMatrix?u32(matE.Obj()):0u);
            entA.Put32(0x18,c.rwKind?u32(rwA.Obj()):0u);
            rwA.Put32(4,u32(frA.Obj()));
        }
        thisA.Put32(0x54,c.quality);
        // vehiculo: modelo (+0x22), velocidad (+0x44), conductor (+0x460), pedal (+0x49C) y subtipo (+0x594)
        vehA.Put16(0x22,c.model);
        for(unsigned i=0;i<3;++i) vehA.Put32(0x44+4*i,c.vel[i]);
        vehA.Put32(0x460,c.driverKind==0?0u:c.driverKind==1?u32(pedA.Obj()):c.driverKind==2?u32(pedB.Obj()):0x12340000u|(Rand(n,77)&0xFFFFu));
        vehA.Put32(0x49C,c.gas);
        vehA.Put32(0x594,c.subtype);
        SetCamera(c.hasMat,c.cam,c.other);
        G32(SlotOf(fnIdx))=u32(sysA.Obj());
        G32(kPrimCounter)=c.counter;
        G32(kFrameCounter)=c.frame;
        G32(kTimeStep)=c.timestep;
        posA.Snap();colA.Snap();matCam.Snap();sysA.Snap();sysB.Snap();thisA.Snap();vehA.Snap();pedA.Snap();pedB.Snap();entA.Snap();matE.Snap();rwA.Snap();frA.Snap();matC.Snap();dirA.Snap();
        TagClear();posA.Tagged();colA.Tagged();matCam.Tagged();sysA.Tagged();sysB.Tagged();thisA.Tagged();vehA.Tagged();pedA.Tagged();pedB.Tagged();entA.Tagged();matE.Tagged();rwA.Tagged();frA.Tagged();matC.Tagged();dirA.Tagged();
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g.c=&c;g.evHash=2166136261u;g.particleCalls=0;g.fxCalls=0;nev=0;
        *RandStatePtr()=c.randSeed;   // mismo estado inicial del rand() real del EXE en ambos lados
        const u32 thisPtr=c.thisKind==0?u32(thisA.Obj()):c.thisKind==1?kFxBase:c.thisKind==2?(0xDEAD0000u|(Rand(n,78)&0xFFFFu)):0u;
        if(IsWheel(fnIdx)) {
            const u32 args[6]{u32(vehA.Obj()),c.pos[0],c.pos[1],c.pos[2],c.spinning,c.light};
            sides[s]=Execute(env,fnIdx,kEntry[fnIdx],candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,thisPtr,c.edx,c.stack,0);});
        } else if(fnIdx==kGunshot) {
            const u32 args[4]{c.entKind?u32(entA.Obj()):0u,u32(posA.Obj()),u32(dirA.Obj()),c.flash};
            sides[s]=Execute(env,fnIdx,kEntry[fnIdx],candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,thisPtr,c.edx,c.stack,0);});
        } else {
            const u32 args[4]{u32(posA.Obj()),u32(colA.Obj()),c.scale,c.amount};
            sides[s]=Execute(env,fnIdx,kEntry[fnIdx],candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,thisPtr,c.edx,c.stack,0);});
        }
        json obs;   // void: se observan la traza, los sorteos consumidos y los bytes cambiados
        obs["rand_state"]=*RandStatePtr();
        obs["ev_hash"]=g.evHash;
        obs["particle_calls"]=g.particleCalls;
        obs["system_slot"]=Tag(uintptr_t(G32(SlotOf(fnIdx))));
        obs["prim_counter"]=G32(kPrimCounter);
        obs["quality_used"]=c.thisKind==0?c.quality:c.thisKind==1?G32(kFxBase+0x54):0u;   // medicion (no resultado): la calidad que leyo la funcion de `this`
        obs["changes"]=json::array({posA.Changes(),colA.Changes(),matCam.Changes(),sysA.Changes(),sysB.Changes(),thisA.Changes(),vehA.Changes(),pedA.Changes(),pedB.Changes(),entA.Changes(),matE.Changes(),rwA.Changes(),frA.Changes(),matC.Changes(),dirA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[fnIdx],n,input,rows,sides);
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
            for(unsigned c=0;c<kCases;++c)CaseFx(env,totals,f,c);
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
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env,uintptr_t exe,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
        if(env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
    }
    return p;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fxwheel77-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fxwheel77-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fxwheel77");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    for(unsigned f=0;f<kFunctions;++f) {
        std::string cat,name;
        const bool found=FindHook(kEntry[f],cat,name);
        if(found)env.route[f].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=found&&env.route[f]->Registered();bodies[kFn[f]]=found&&env.route[f]->Body()!=0;
        ready&=found&&env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[f]]=found?json({{"class",cat},{"name",name},{"address",kEntry[f]}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){Run(env,false,registered,bodies,collaborators);return;}
    const Pair pAdd=MakeCollab(env,0x4AA440,hooksFound);
    const Pair pEnable=MakeCollab(env,0x4AA610,hooksFound);
    const Pair pPed=MakeCollab(env,0x56E210,hooksFound);
    const Pair pVeh=MakeCollab(env,0x56E0D0,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    const StubSpec sAdd=S(pAdd,reinterpret_cast<void*>(&FakeAddParticle)),sEnable=S(pEnable,reinterpret_cast<void*>(&FakeEnablePrim)),
                   sPed=S(pPed,reinterpret_cast<void*>(&FakePlayerPed)),sVeh=S(pVeh,reinterpret_cast<void*>(&FakePlayerVehicle));
    env.stubs[kDust]={sAdd,sVeh};
    env.stubs[kSand]={sAdd,sVeh};
    env.stubs[kGlass]={sAdd};
    env.stubs[kDebris]={sAdd,sEnable};
    env.stubs[kGrass]={sAdd,sPed,sVeh};
    env.stubs[kGravel]={sAdd,sPed,sVeh};
    env.stubs[kMud]={sAdd,sPed,sVeh};
    {   // TriggerGunshot: AllocateMatrix, UpdateMatrix (sin hook RH: se parchea el cuerpo de la DLL por su direccion), FxRwMatrixCreate/Destroy, CreateMatFromVec, CreateFxSystem, CopyParentMatrix, PlayAndKill
        const Pair pUpdate{0x54F1B0,Body(std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix)),false};
        env.stubs[kGunshot]={S(MakeCollab(env,0x54F560,hooksFound),reinterpret_cast<void*>(&FakeAllocMatrix)),S(pUpdate,reinterpret_cast<void*>(&FakeUpdateMatrix)),
                             S(MakeCollab(env,0x4A9440,hooksFound),reinterpret_cast<void*>(&FakeRwCreate)),S(MakeCollab(env,0x4A9460,hooksFound),reinterpret_cast<void*>(&FakeRwDestroy)),
                             S(MakeCollab(env,0x49E950,hooksFound),reinterpret_cast<void*>(&FakeCreateMat)),S(MakeCollab(env,0x4A9BE0,hooksFound),reinterpret_cast<void*>(&FakeCreateFx)),
                             S(MakeCollab(env,0x4AA890,hooksFound),reinterpret_cast<void*>(&FakeCopyParent)),S(MakeCollab(env,0x4AA3D0,hooksFound),reinterpret_cast<void*>(&FakePlayAndKill))};
    }
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    for(unsigned f=0;f<kFunctions;++f)env.watches.emplace_back(kEntry[f],16);
    env.watches.emplace_back(0x821B1E,16);   // rand real del EXE (sin falso)
    env.watches.emplace_back(0x827B3D,16);   // getptd
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
