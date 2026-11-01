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
#include "Events/EventVehicleDamageWeapon.h"
#include "Events/EventGroup.h"
#include "Models/VehicleModelInfo.h"
#include "DamageManager.h"
#include "PlayerInfo.h"
#include "Crime.h"
#include "Stats.h"
#include "General.h"
#include "Pad.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Veh83B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::InflictDamage (0x6D7C90, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (sus hooks RH, si existen, quedan apagados durante el banco):
//   CVehicle::CanVehicleBeDamaged 0x6D1280, CStats::GetPercentageProgress 0x5591E0, FindPlayerPed 0x56E210, FindPlayerVehicle 0x56E0D0, CStats::IncrementStat 0x55C180, CPed::IsPlayer 0x5DF8F0,
//   CVehicle::GetVehicleAppearance 0x6D1080, CVehicle::FindTyreNearestPoint 0x6D7BC0, CEntity::RegisterReference 0x571B70, CEntity::CleanUpOldReference 0x571A00, CEventGroup::Add 0x4AB420,
//   CCrime::ReportCrime 0x532010, CDamageManager::SetEngineStatus 0x6C22A0 y CPlayerPed::SetWantedLevelNoDrop 0x609F30. Los virtuales BurstTyre (+0xB0) y BlowUpCar (+0xA4) los atiende la vtabla falsa del vehiculo.
//   Corren de verdad en ambos lados: rand() del CRT del EXE (0x821B1E), CEventVehicleDamageWeapon (constructor 0x61C2E0 y destructor 0x4B1990 / el de la DLL), CVector::Magnitude, la multiplicacion matriz*vector
//   (0x59C890 / Hp65MatrixTimesPointCpp), CVector != (0x509760) y min (0x404330).
// Los falsos registran sus argumentos EN ORDEN (punteros de pila por su marca, punteros de arena por etiqueta). Las arenas (guarda + objeto + guarda) contienen el vehiculo (0x990), seis peatones (0x700, con su
// CPedIntelligence en +0x600), entidades pequenas, el modelo y su estructura (dummy del deposito en +0x60) y la matriz del vehiculo. Se instalan CWorld::PlayerInFocus, CWorld::Players y los modelos del banco en ms_modelInfoPtrs.

namespace V83Live {
using namespace V83Fix;
inline constexpr uintptr_t kEntry=0x6D7C90;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CVehicle,m_nVehicleType)==0x590&&offsetof(CVehicle,m_nVehicleSubType)==0x594);
static_assert(offsetof(CVehicle,m_pDriver)==0x460&&offsetof(CVehicle,m_apPassengers)==0x464&&offsetof(CVehicle,m_nMaxPassengers)==0x488);
static_assert(offsetof(CVehicle,m_fHealth)==0x4C0&&offsetof(CVehicle,m_nLastWeaponDamageType)==0x508&&offsetof(CVehicle,m_pLastDamageEntity)==0x50C);
static_assert(offsetof(CVehicle,m_wBombTimer)==0x4DE&&offsetof(CVehicle,m_pWhoDetonatedMe)==0x4E0);
static_assert(offsetof(CPed,m_pVehicle)==0x58C&&offsetof(CPed,m_nPedType)==0x598);
static_assert(offsetof(CPedIntelligence,m_eventGroup)==0x68);
static_assert(offsetof(CEventVehicleDamage,m_vehicle)==0x14&&offsetof(CEventVehicleDamage,m_attacker)==0x18&&offsetof(CEventVehicleDamage,m_weaponType)==0x1C);
static_assert(offsetof(CPlayerInfo,m_nHavocCaused)==0x140&&offsetof(CPlayerInfo,m_fCurrentChaseValue)==0x148);
static_assert(offsetof(CAutomobile,m_damageManager)==0x5A0&&sizeof(CAutomobile)<=kVehWin);
static_assert(sizeof(CPlayerInfo)==0x190);

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
    u32 Get16(size_t off) const {uint16 v;memcpy(&v,m.data()+kGuard+off,2);return v;}
    u32 Get8(size_t off) const {return m[kGuard+off];}
    // Cambios respecto a la imagen de referencia, sin los campos que se comparan aparte (mask = [[desplazamiento del objeto, bytes]])
    json Changes(std::initializer_list<std::pair<size_t,size_t>> mask={}) const {
        auto a=m,b=r;
        for(const auto& [off,len]:mask)for(size_t i=0;i<len;++i){a[kGuard+off+i]=0;b[kGuard+off+i]=0;}
        return ArenaChanges(a.data(),b.data(),N,kGuard);
    }
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline std::array<Arena<kPedArena>,6> pedA;   // 0 D, 1 conductor, 2..5 p0..p3
inline Arena<kSmallArena> xA,plA,vpA,vsA;
inline Arena<kPedArena> vdA;   // vehiculo del atacante: se lee +0x594
inline Arena<kMiArena> miA;
inline Arena<kMatArena> matA;

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

// ================================================================================================ InflictDamage (0x6D7C90)
struct State {const C* c{};unsigned unexpected{};};
inline State g;
inline u32 PtrTag(const void* p) {const u32 t=Tag(p);return t==u32(uintptr_t(p))?0xFFFF0001u:t;}   // fuera de las arenas = pila
inline void* PedFor(u32 kind) {   // FindPlayerPed: 0 nulo, 1 D, 2 conductor, 3 p0, 4 PL
    switch(kind){case 1:return reinterpret_cast<void*>(pedA[0].Obj());case 2:return reinterpret_cast<void*>(pedA[1].Obj());case 3:return reinterpret_cast<void*>(pedA[2].Obj());
    case 4:return reinterpret_cast<void*>(plA.Obj());default:return nullptr;}
}
inline void* VehFor(u32 kind) {   // FindPlayerVehicle: 0 nulo, 1 el vehiculo, 2 X, 3 Vp
    switch(kind){case 1:return reinterpret_cast<void*>(vehA.Obj());case 2:return reinterpret_cast<void*>(xA.Obj());case 3:return reinterpret_cast<void*>(vpA.Obj());default:return nullptr;}
}
inline u32 DamagerPtr(u32 kind) {   // 0 nulo, 1 D, 2 X, 3 conductor, 4 p0
    switch(kind){case 1:return u32(pedA[0].Obj());case 2:return u32(xA.Obj());case 3:return u32(pedA[1].Obj());case 4:return u32(pedA[2].Obj());default:return 0;}
}
// Eventos: 1 CanVehicleBeDamaged [this, damager, arma, &salida], 2 GetPercentageProgress, 3 FindPlayerPed [id], 4 FindPlayerVehicle [id, bandera], 5 IncrementStat [estadistica, valor], 6 IsPlayer [this],
// 7 GetVehicleAppearance [this], 8 FindTyreNearestPoint [this, x, y], 9 BurstTyre [this, rueda, bandera], 10 BlowUpCar [this, atacante, bandera], 11 RegisterReference [this, &ref, *ref],
// 12 CleanUpOldReference [this, &ref, *ref], 13 CEventGroup::Add [grupo, vehiculo, atacante, arma, bandera], 14 ReportCrime [crimen, victima, informante], 15 SetEngineStatus [this, estado],
// 16 SetWantedLevelNoDrop [this, nivel], 99 virtual inesperado
inline u32 __fastcall FakeCanDamage(void* self,void*,void* damager,u32 weapon,uint8* out) {
    Ev& e=PushEv(1,4);e.v[0]=Tag(self);e.v[1]=Tag(damager);e.v[2]=weapon;e.v[3]=PtrTag(out);
    *out=uint8(g.c->canFire);
    return g.c->canRet;
}
inline float __cdecl FakeProgress() {PushEv(2,0);return F(g.c->progress);}
inline void* __cdecl FakeFindPed(int32 id) {Ev& e=PushEv(3,1);e.v[0]=u32(id);return PedFor(g.c->fpPed);}
inline void* __cdecl FakeFindVeh(int32 id,u32 flag) {Ev& e=PushEv(4,2);e.v[0]=u32(id);e.v[1]=flag&0xFFu;return VehFor(g.c->fpVeh);}
inline void __cdecl FakeInc(u32 stat,u32 value) {Ev& e=PushEv(5,2);e.v[0]=stat;e.v[1]=value;}
inline u32 __fastcall FakeIsPlayer(void* self,void*) {Ev& e=PushEv(6,1);e.v[0]=Tag(self);return g.c->isPlayerRet;}
inline u32 __fastcall FakeAppearance(void* self,void*) {Ev& e=PushEv(7,1);e.v[0]=Tag(self);return g.c->appearance;}
inline u32 __fastcall FakeTyreImpl(void* self,void*,u32 x,u32 y) {Ev& e=PushEv(8,3);e.v[0]=Tag(self);e.v[1]=x;e.v[2]=y;return g.c->tyreRet;}
// El EXE llama a BurstTyre justo despues, SIN recargar ECX: la funcion real (0x6D7BC0) no toca ECX, asi que el falso tambien lo devuelve intacto (thiscall, ret 8).
__declspec(naked) inline u32 FakeTyre() {
    __asm {
        push ecx
        mov edx,[esp+0xC]
        push edx
        mov edx,[esp+0xC]
        push edx
        xor edx,edx
        call FakeTyreImpl
        pop ecx
        ret 8
    }
}
inline u32 __fastcall FakeBurst(void* self,void*,u32 tyre,u32 flag) {Ev& e=PushEv(9,3);e.v[0]=Tag(self);e.v[1]=tyre&0xFFu;e.v[2]=flag&0xFFu;return 0;}
inline void __fastcall FakeBlowUp(void* self,void*,void* damager,u32 flag) {Ev& e=PushEv(10,3);e.v[0]=Tag(self);e.v[1]=Tag(damager);e.v[2]=flag&0xFFu;}
inline void __fastcall FakeRegRef(void* self,void*,void** ref) {Ev& e=PushEv(11,3);e.v[0]=Tag(self);e.v[1]=PtrTag(ref);e.v[2]=Tag(*ref);}
inline void __fastcall FakeCleanRef(void* self,void*,void** ref) {Ev& e=PushEv(12,3);e.v[0]=Tag(self);e.v[1]=PtrTag(ref);e.v[2]=Tag(*ref);}
inline void* __fastcall FakeGroupAdd(void* self,void*,uint8* event,u32 flag) {
    Ev& e=PushEv(13,5);e.v[0]=Tag(self);e.v[1]=Tag(*reinterpret_cast<void**>(event+0x14));e.v[2]=Tag(*reinterpret_cast<void**>(event+0x18));e.v[3]=*reinterpret_cast<u32*>(event+0x1C);e.v[4]=flag&0xFFu;
    return nullptr;
}
inline void __cdecl FakeCrime(u32 crime,void* victim,void* reporter) {Ev& e=PushEv(14,3);e.v[0]=crime;e.v[1]=Tag(victim);e.v[2]=Tag(reporter);}
inline void __fastcall FakeEngine(void* self,void*,u32 status) {Ev& e=PushEv(15,2);e.v[0]=Tag(self);e.v[1]=status;}
inline void __fastcall FakeWanted(void* self,void*,u32 level) {Ev& e=PushEv(16,2);e.v[0]=Tag(self);e.v[1]=level;}
inline u32 __fastcall FakeUnexpectedVirtual(void* self,void*) {++g.unexpected;PushEv(99,1).v[0]=Tag(self);return 0;}
// vtabla falsa compartida por todas las arenas: solo +0xA4 (41, BlowUpCar) y +0xB0 (44, BurstTyre) son validas
inline std::array<u32,96> vtbl{};
inline void InitVtbl() {
    for(auto& v:vtbl)v=u32(reinterpret_cast<uintptr_t>(&FakeUnexpectedVirtual));
    vtbl[41]=u32(reinterpret_cast<uintptr_t>(&FakeBlowUp));
    vtbl[44]=u32(reinterpret_cast<uintptr_t>(&FakeBurst));
}

inline void CaseInflict(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);
        for(unsigned i=0;i<6;++i)pedA[i].Fill(c.seed*13u+2u+i);
        xA.Fill(c.seed*13u+8u);vdA.Fill(c.seed*13u+9u);plA.Fill(c.seed*13u+10u);vpA.Fill(c.seed*13u+11u);vsA.Fill(c.seed*13u+12u);miA.Fill(c.seed*13u+13u);matA.Fill(c.seed*13u+14u);
        for(unsigned i=0;i<12;++i)matA.Put32(kMatOff[i],c.mat[i]);
        // vehiculo
        vehA.Put32(0,u32(uintptr_t(vtbl.data())));
        vehA.Put32(0x14,u32(matA.Obj()));
        vehA.Put16(0x22,kModels[c.model%kModels.size()]);
        vehA.Put8(0x36,(c.status<<3)|2u);
        vehA.SetBit(0x428,0,c.lawEnf);vehA.SetBit(0x42B,7,c.tyresDont);vehA.SetBit(0x42C,7,c.isRC);vehA.SetBit(0x42E,6,c.petrol);
        vehA.Put32(0x460,c.hasDriver?u32(pedA[1].Obj()):0u);
        for(unsigned i=0;i<8;++i)vehA.Put32(0x464+4*i,c.pass[i]?u32(pedA[1+c.pass[i]].Obj()):0u);   // p0..p3 = pedA[2..5]
        vehA.Put8(0x488,c.maxPass);
        vehA.Put32(0x4C0,c.health);
        vehA.Put32(0x590,c.vtype);vehA.Put32(0x594,c.vsub);
        vehA.Put8(0x508,0xA5);vehA.Put32(0x50C,0xDEAD0050u);vehA.Put16(0x4DE,0x1234u);vehA.Put32(0x4E0,0xDEAD00E0u);vehA.Put32(0x928,0xDEAD0928u);   // centinelas: el oraculo sabe cuales no cambian
        // peatones: D (0), conductor (1), p0..p3 (2..5)
        for(unsigned i=0;i<6;++i) {
            pedA[i].Put32(0,u32(uintptr_t(vtbl.data())));
            pedA[i].Put8(0x36,(Rand(c.n,600+i)%32u)<<3|3u);
            pedA[i].Put32(0x47C,u32(pedA[i].Obj(kGuard+kIntelOff)));
            pedA[i].Put32(0x58C,0u);
        }
        pedA[0].Put8(0x36,((Rand(c.n,610)%32u)<<3)|(c.dmgType&7u));
        pedA[0].Put32(0x58C,c.dmgHasVeh?u32(vdA.Obj()):0u);
        pedA[1].Put32(0x598,c.drvPedType);
        vdA.Put32(0x594,c.vdSub);
        {u32 t3=Rand(c.n,611)%7u;if(t3>=3)++t3;xA.Put8(0x36,((Rand(c.n,612)%32u)<<3)|t3);}
        // modelo
        vsA.Put32(0x60,c.dummy[0]);vsA.Put32(0x64,c.dummy[1]);vsA.Put32(0x68,c.dummy[2]);
        miA.Put32(0x5C,u32(vsA.Obj()));
        for(const u32 m:kModels)G32(kModelTable+4*m)=u32(miA.Obj());
        // mundo
        *reinterpret_cast<uint8*>(kPlayerInFocus)=uint8(c.inFocus);
        G32(kPlayers+c.inFocus*0x190+0x140)=c.havoc0;G32(kPlayers+c.inFocus*0x190+0x148)=c.chase0;
        *RandStatePtr()=c.randSeed;
        vehA.Snap();for(auto& p:pedA)p.Snap();xA.Snap();vdA.Snap();plA.Snap();vpA.Snap();vsA.Snap();miA.Snap();matA.Snap();
        TagClear();vehA.Tagged();for(auto& p:pedA)p.Tagged();xA.Tagged();vdA.Tagged();plA.Tagged();vpA.Tagged();vsA.Tagged();miA.Tagged();matA.Tagged();
        // etiquetas: 0 vehiculo, 1 D, 2 conductor, 3..6 pasajeros (no existen separadas: p0..p3 = arenas 3..6), 7 X, 8 Vd, 9 PL, 10 vs? (ver orden de TagAdd arriba)
        g=State{};g.c=&c;nev=0;
        const u32 args[6]{DamagerPtr(c.dmgKind),c.weapon,c.intensity,c.coord[0],c.coord[1],c.coord[2]};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["fields"]=json::array({vehA.Get32(0x4C0),vehA.Get8(0x508),Tag(uintptr_t(vehA.Get32(0x50C))),vehA.Get16(0x4DE),Tag(uintptr_t(vehA.Get32(0x4E0))),Tag(uintptr_t(vehA.Get32(0x928)))});
        obs["players"]=json::array({G32(kPlayers+c.inFocus*0x190+0x140),G32(kPlayers+c.inFocus*0x190+0x148)});
        obs["rand"]=*RandStatePtr();
        json ch=json::array();
        ch.push_back(vehA.Changes({{0x4C0,4},{0x508,1},{0x50C,4},{0x4DE,2},{0x4E0,4},{0x928,4}}));
        for(auto& p:pedA)ch.push_back(p.Changes());
        ch.push_back(xA.Changes());ch.push_back(vdA.Changes());ch.push_back(plA.Changes());ch.push_back(vpA.Changes());ch.push_back(vsA.Changes());ch.push_back(miA.Changes());ch.push_back(matA.Changes());
        obs["changes"]=ch;
        obs["events"]=EvsJson();
        obs["unexpected_virtuals"]=g.unexpected;
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ microtests de la x87 (hardware): `fld a; fcomp b` frente al modelo del oraculo
// FSW & 0x4500 tras `fld a; fcomp b`: 0x0000 si a > b, 0x0100 si a < b, 0x4000 si a == b, 0x4500 si no son ordenables (NaN)
inline u32 X87Cmp(u32 a,u32 b) {
    uint16 sw{};
    __asm {fld dword ptr a} __asm {fcomp dword ptr b} __asm {fnstsw word ptr sw}
    return u32(sw)&0x4500u;
}
inline json MicroTests() {
    json out=json::array();
    PrepareFloatingState();
    static constexpr u32 vals[]{0x7FC00001u,0xFFC00002u,0x7FC12345u,0x7F800001u,0xFFA00003u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x3F7FFFFFu,0x3F800001u,0xBF7FFFFFu,
        0xC1000000u,0x00000001u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x7F7FFFFFu,0xFF7FFFFFu,0x3F000000u,0x42C80000u,0xFFC00000u};
    for(u32 a:vals)for(u32 b:vals)out.push_back({0,a,b,X87Cmp(a,b)});
    RestoreFloatingState();
    return out;
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
    ready&=NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D);   // rand real del EXE en ambos lados
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},{"rng_native",NativeRandom()},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseInflict(env,totals,c);
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
    if(!std::filesystem::exists(dir/"veh83b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"veh83b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("veh83b");}
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
    {   // CPed::m_pIntelligence es protegido: se comprueba su desplazamiento (+0x47C) por el getter publico
        alignas(16) std::array<uint8,0x700> probe{};
        const uint32 marker=0x5A5A1234u;memcpy(probe.data()+0x47C,&marker,4);
        ready&=reinterpret_cast<CPed*>(probe.data())->GetIntelligence()==reinterpret_cast<CPedIntelligence*>(uintptr_t(marker));
    }
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    using Void=void*;
    env.stubs[0]={
        S(MakeCollab(env,0x6D1280,FnAddr(&CVehicle::CanVehicleBeDamaged),hooksFound),Void(&FakeCanDamage)),
        S(MakeCollab(env,0x5591E0,FnAddr(&CStats::GetPercentageProgress),hooksFound),Void(&FakeProgress)),
        S(MakeCollab(env,0x56E210,FnAddr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),hooksFound),Void(&FakeFindPed)),
        S(MakeCollab(env,0x56E0D0,FnAddr(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),hooksFound),Void(&FakeFindVeh)),
        S(MakeCollab(env,0x55C180,FnAddr(&CStats::IncrementStat),hooksFound),Void(&FakeInc)),
        S(MakeCollab(env,0x5DF8F0,FnAddr(&CPed::IsPlayer),hooksFound),Void(&FakeIsPlayer)),
        S(MakeCollab(env,0x6D1080,FnAddr(&CVehicle::GetVehicleAppearance),hooksFound),Void(&FakeAppearance)),
        S(MakeCollab(env,0x6D7BC0,FnAddr(&CVehicle::FindTyreNearestPoint),hooksFound),Void(&FakeTyre)),
        S(MakeCollab(env,0x571B70,FnAddr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),hooksFound),Void(&FakeRegRef)),
        S(MakeCollab(env,0x571A00,FnAddr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),hooksFound),Void(&FakeCleanRef)),
        S(MakeCollab(env,0x4AB420,FnAddr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),hooksFound),Void(&FakeGroupAdd)),
        S(MakeCollab(env,0x532010,FnAddr(&CCrime::ReportCrime),hooksFound),Void(&FakeCrime)),
        S(MakeCollab(env,0x6C22A0,FnAddr(&CDamageManager::SetEngineStatus),hooksFound),Void(&FakeEngine)),
        S(MakeCollab(env,0x609F30,FnAddr(&CPlayerPed::SetWantedLevelNoDrop),hooksFound),Void(&FakeWanted))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"rand() del CRT del EXE (0x821B1E)","CEventVehicleDamageWeapon (constructor 0x61C2E0, destructor)","CVector::Magnitude","matriz*vector (0x59C890 / Hp65MatrixTimesPointCpp)","CVector != (0x509760)","min (0x404330)","CWorld::Players / PlayerInFocus (arenas del mundo restauradas)"})},
            {"virtual_fakes",json::array({"BlowUpCar +0xA4","BurstTyre +0xB0"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    env.watches.emplace_back(0x821B1E,16);   // rand real del EXE (sin falso)
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
