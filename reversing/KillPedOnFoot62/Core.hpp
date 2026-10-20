#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include "Fixtures.hpp"
// Nucleo del banco KillPedOnFoot62: CTaskComplexKillPedOnFoot::{ControlSubTask 0x626260, CreateFirstSubTask 0x62B490, CreateNextSubTask 0x62B150, CreateSubTask 0x625E70, MakeAbortable 0x625E40}.
// Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}, OriginalDependencies499::HookRoute y los tipos del juego.
// Plantillas: reversing/PartnerNext60 y reversing/PartnerControl60 (ABI, hook Virtual, falsos con parche de entrada en el EXE y en la DLL, argumentos de pila, constructores reales).
//
// Banco par a par de CINCO funciones. Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO el hook de la funcion probada encendido (los otros cuatro de la clase quedan
// apagados: sus llamadas entran en el codigo del EXE en ambos lados). Sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto con el hook
// encendido ANTES de escribir ningun parche): ver `Stub`. Reales en ambos lados: CPed::IsPlayer, CTaskTimer::IsOutOfTime, IsPedTypeGang, los constructores de las tareas creadas y de CEventAreaCodes
// (construyen en la arena que devuelve el falso de operator new; se compara el objeto byte a byte y su tipo con una llamada virtual a GetTaskType). El generador aleatorio: el EXE original de
// CGeneral::GetRandomIntInRange (0x407180, con su hook apagado) pide el numero a rand del EXE (0x821B1E) y la DLL a CGeneral::GetRandomNumber (cuerpo de la DLL): los dos falsos devuelven el mismo.
// Las llamadas virtuales (GetTaskType y MakeAbortable de la subtarea) van por una vtable falsa de arena; la vtable de `this` es la REAL (0x86D894) porque ControlSubTask llama a CreateFirstSubTask
// por la ranura 9.

namespace KillPedOnFoot62Live {
using namespace KillPedOnFootFix;
using HookRoute = OriginalDependencies499::HookRoute;
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(address),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline uintptr_t Resolve(uintptr_t address) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i)address+=5+*reinterpret_cast<int32*>(address+1);return address;}
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}
inline void PrimeX87() {
    static const double stale[8]{1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8};
    const double* p=stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// Estos EXE usan x87 y sus colaboradores tambien: se compara lo observable (FCW completa, TOP, etiquetas y ST validos), no los registros muertos.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. EAX es el retorno (se compara siempre en `observed.ret`); ECX/EDX no los lee ningun llamador (ver README.md): solo se registran.
inline constexpr bool kCompareVolatileRegisters=false;

inline std::string HexBytes(const void* p,size_t n) {constexpr char d[]="0123456789abcdef";const auto* b=static_cast<const u8*>(p);std::string o;o.reserve(n*2);for(size_t i=0;i<n;++i){o+=d[b[i]>>4];o+=d[b[i]&15];}return o;}
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline std::array<uint8,108> MaskDeadRegisters(std::array<uint8,108> fp){for(unsigned st=0;st<8;++st)if(FpRegisterIsDead(fp,st))memset(fp.data()+28+10*st,0,10);return fp;}
inline std::array<uint8,108> NormalizeFp(std::array<uint8,108> fp){
    uint16 fsw{};memcpy(&fsw,fp.data()+4,2);fsw&=0x3800;memcpy(fp.data()+4,&fsw,2);   // solo TOP
    memset(fp.data()+2,0,2);memset(fp.data()+6,0,2);memset(fp.data()+10,0,2);          // mitades reservadas
    memset(fp.data()+12,0,16);                                                         // FIP, FCS/FOP, FDP, FDS
    return MaskDeadRegisters(fp);
}
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
    json out=json::array();
    const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw_top",4,2},{"ftw",8,2}};
    for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
    for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
    return out;
}
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};

// ------------------------------------------------------------------------------------------------ llamada con medicion de ABI
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},outFlags{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{},sehBefore{},sehAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov ebpBefore,ebp
        mov eax,dword ptr fs:[0]
        mov sehBefore,eax
        mov edx,stackSeed
        xor esi,esi
        mov eax,esp
        mov ecx,64
    seedloop:
        sub eax,4
        add esi,9E3779B1h
        mov edi,edx
        xor edi,esi
        mov [eax],edi
        dec ecx
        jnz seedloop
        mov espBefore,esp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        mov ecx,argCount
        mov edx,argsPtr
    pushloop:
        test ecx,ecx
        jz pushed
        push dword ptr [edx+ecx*4-4]
        dec ecx
        jmp pushloop
    pushed:
        mov eax,seedEax
        mov ecx,seedEcx
        mov edx,seedEdx
        call entryAddress
        mov outEax,eax
        mov outEcx,ecx
        mov outEdx,edx
        pushfd
        pop eax
        mov outFlags,eax
        mov eax,dword ptr fs:[0]
        mov sehAfter,eax
        add esp,popBytes
        mov espAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
        pop edi
        pop esi
        pop ebx
    }
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter,sehBefore==sehAfter};
}

// ------------------------------------------------------------------------------------------------ el mundo: una sola arena con regiones
alignas(16) inline std::array<u8,kArena> world{},worldRef{};
inline u32 Off(u32 region,u32 at=0){return RegionOff(region)+at;}
inline u8* W(u32 region,u32 at=0){return world.data()+Off(region,at);}
inline u8* Rf(u32 region,u32 at=0){return worldRef.data()+Off(region,at);}
inline u32 Addr(u32 region,u32 at=0){return u32(uintptr_t(world.data())+Off(region,at));}
inline void Put8(u8* p,u32 v){*p=u8(v);}
inline void Put16(u8* p,u32 v){const u16 x=u16(v);std::memcpy(p,&x,2);}
inline void Put32(u8* p,u32 v){std::memcpy(p,&v,4);}
inline u32 Get32(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}

constexpr const char* kRegionTag[kRegions]{"T","D","G","M","N","V","X","J","WP","PD","WT","CP","CT","GR","S","A","O","L"};
// Etiqueta de un puntero: region+desplazamiento (las arenas no cambian de direccion durante la ejecucion, pero si entre ejecuciones del juego)
inline json Name(u32 v) {
    const u32 base=u32(uintptr_t(world.data()));
    if(v>=base&&v<base+kArena){
        const u32 o=v-base;
        for(u32 r=0;r<kRegions;++r){const u32 s=RegionOff(r);if(o>=s&&o<s+Round16(kRegionBytes[r])+kGuard)return json(std::string(kRegionTag[r])+"+"+std::to_string(o-s));}
        return json(std::string("W+")+std::to_string(o));
    }
    return json(v);
}

// ---- falsos. Registro ordenado de eventos (sin reservas ni JSON dentro de los falsos)
enum Stub : unsigned {S_WANTED,S_PLAYERPED,S_NOPOLICE,S_MELEE,S_SAY,S_GIVE,S_SETTYPE,S_SETSLOT,S_MOVESTATE,S_DROP,S_OPEN,S_DRIVER,S_PASSENGER,S_DOOR,S_ROOM,S_HEALTHY,S_GROUP,S_LEADER,S_NEW,S_STILL,S_ADD,S_C65,S_RANDEXE,S_RANDDLL,kStubs};
enum : u32 {EvWanted=1,EvPlayerPed,EvNoPolice,EvMelee,EvSay,EvGive,EvSetType,EvSetSlot,EvMoveState,EvDrop,EvOpen,EvDriver,EvPassenger,EvDoor,EvRoom,EvHealthy,EvGroup,EvLeader,EvNew,EvStill,EvAdd,EvC65,EvRand,EvType,EvAbort,EvTrap};
struct Ev {u32 kind,a,b,c,d,e;};
inline std::array<Ev,kMaxEvents> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0){if(nev<evs.size())evs[nev]=Ev{k,a,b,c,d,e};++nev;}
inline const C* gc{};
inline std::array<unsigned,32> cnt{};
inline u32 gNewN{};
// Valor booleano de la llamada k-esima del falso `which`: bit k de la mascara en el bit 0 y basura en los bits 8..31 (los llamadores solo leen AL)
inline u32 BoolRet(u32 mask,unsigned which){const unsigned k=cnt[which]++;return ((mask>>(k&31))&1u)|((gc->garbage*0x9E3779B1u+k*0x85EBCA6Bu+which*0x1234567u)&0xFFFFFF00u);}
inline u32 Choice(u32 sel,unsigned k){   // 0 nulo, 1 objetivo, 2 ped, 3 otra direccion
    const u32 s=(sel>>(2*(k&15)))&3u;
    return s==0?0u:s==1?Addr(R_TGT):s==2?Addr(R_PED):Addr(R_OTHER);
}
inline u32 __cdecl FakeWanted(u32 id){Push(EvWanted,id);return Addr(R_WP);}
inline u32 __cdecl FakePlayerPed(u32 id){const unsigned k=cnt[S_PLAYERPED]++;Push(EvPlayerPed,id);return Choice(gc->selPlayer,k);}
inline u32 __cdecl FakeNoPolice(){Push(EvNoPolice);return BoolRet(gc->mNoPolice,S_NOPOLICE);}
inline u32 __fastcall FakeMelee(void* self,void*){Push(EvMelee,u32(uintptr_t(self)));return BoolRet(gc->mMelee,S_MELEE);}
// CPed::Say(context, startTimeDelay, probability, overrideSilence, isForceAudible, isFrontEnd): thiscall 6 argumentos de pila (`ret 0x18`); los bool se registran por su byte bajo
inline u32 __fastcall FakeSay(void* self,void*,u32 ctx,u32 delay,u32 prob,u32 b1,u32 b2,u32 b3){Push(EvSay,u32(uintptr_t(self)),ctx,delay,prob,(b1&0xFFu)|((b2&0xFFu)<<8)|((b3&0xFFu)<<16));return 0;}
inline u32 __fastcall FakeGive(void* self,void*,u32 type,u32 ammo,u32 flag){Push(EvGive,u32(uintptr_t(self)),type,ammo,flag&0xFFu);return 0;}
inline void __fastcall FakeSetType(void* self,void*,u32 type){Push(EvSetType,u32(uintptr_t(self)),type);}
inline void __fastcall FakeSetSlot(void* self,void*,u32 slot){Push(EvSetSlot,u32(uintptr_t(self)),slot);}
inline void __fastcall FakeMoveState(void* self,void*,u32 state){Push(EvMoveState,u32(uintptr_t(self)),state);}
inline void __fastcall FakeDrop(void* self,void*,u32 del){Push(EvDrop,u32(uintptr_t(self)),del&0xFFu);}
inline u32 __fastcall FakeOpen(void* veh,void*,void* ped){Push(EvOpen,u32(uintptr_t(veh)),u32(uintptr_t(ped)));return BoolRet(gc->mCanOpen,S_OPEN);}
inline u32 __fastcall FakeDriver(void* veh,void*,void* ped){Push(EvDriver,u32(uintptr_t(veh)),u32(uintptr_t(ped)));return BoolRet(gc->mDriver,S_DRIVER);}
inline u32 __fastcall FakePassenger(void* veh,void*,void* ped){Push(EvPassenger,u32(uintptr_t(veh)),u32(uintptr_t(ped)));return BoolRet(gc->mPassenger,S_PASSENGER);}
inline u32 __cdecl FakeDoor(void* veh,void* ped){Push(EvDoor,u32(uintptr_t(veh)),u32(uintptr_t(ped)));return gc->door;}
inline u32 __cdecl FakeRoom(void* veh,u32 door,void* pos){Push(EvRoom,u32(uintptr_t(veh)),door,u32(uintptr_t(pos)));return BoolRet(gc->mRoom,S_ROOM);}
inline u32 __cdecl FakeHealthy(void* veh){Push(EvHealthy,u32(uintptr_t(veh)));return BoolRet(gc->mHealthy,S_HEALTHY);}
inline u32 __cdecl FakeGroup(void* ped){const unsigned k=cnt[S_GROUP]++;Push(EvGroup,u32(uintptr_t(ped)));return ((gc->selGroup>>(k&31))&1u)?Addr(R_GRP):0u;}
inline u32 __fastcall FakeLeader(void* self,void*){const unsigned k=cnt[S_LEADER]++;Push(EvLeader,u32(uintptr_t(self)));return Choice(gc->selLeader,k);}
// 0x61A5A0 CTask::operator new(size): devuelve la region de reserva en la primera llamada (o nulo si el caso lo pide) y nulo en las demas
inline void* __cdecl FakeNew(size_t size){Push(EvNew,u32(size));const u32 k=gNewN++;return (k==0&&!gc->allocNull)?reinterpret_cast<void*>(uintptr_t(Addr(R_ALLOC))):nullptr;}
// CTaskSimpleStandStill::ProcessPed del objeto temporal de la pila (no se registra `this`)
inline u32 __fastcall FakeStill(void*,void*,void* ped){Push(EvStill,u32(uintptr_t(ped)));return BoolRet(gc->mStillProc,S_STILL);}
// CEventGroup::Add(event, valid): se registra el CPed* del evento (CEventAreaCodes::m_ped) y el byte de validez
inline u32 __fastcall FakeAdd(void* grp,void*,void* evt,u32 valid){Push(EvAdd,u32(uintptr_t(grp)),u32(uintptr_t(reinterpret_cast<CEventAreaCodes*>(evt)->m_ped)),valid&0xFFu);return 0;}
inline void __cdecl FakeC65(void* ped,void* target){Push(EvC65,u32(uintptr_t(ped)),u32(uintptr_t(target)));}
// rand del EXE (0x821B1E) y CGeneral::GetRandomNumber de la DLL: el mismo numero (el EXE enmascara con 0xFFFF)
inline u32 __cdecl FakeRandExe(){Push(EvRand);return gc->randVal;}
inline u32 __cdecl FakeRandDll(){Push(EvRand);return gc->randVal&0xFFFFu;}
inline void* StubFn(unsigned s) {
    switch(s) {
    case S_WANTED: return reinterpret_cast<void*>(&FakeWanted);
    case S_PLAYERPED: return reinterpret_cast<void*>(&FakePlayerPed);
    case S_NOPOLICE: return reinterpret_cast<void*>(&FakeNoPolice);
    case S_MELEE: return reinterpret_cast<void*>(&FakeMelee);
    case S_SAY: return reinterpret_cast<void*>(&FakeSay);
    case S_GIVE: return reinterpret_cast<void*>(&FakeGive);
    case S_SETTYPE: return reinterpret_cast<void*>(&FakeSetType);
    case S_SETSLOT: return reinterpret_cast<void*>(&FakeSetSlot);
    case S_MOVESTATE: return reinterpret_cast<void*>(&FakeMoveState);
    case S_DROP: return reinterpret_cast<void*>(&FakeDrop);
    case S_OPEN: return reinterpret_cast<void*>(&FakeOpen);
    case S_DRIVER: return reinterpret_cast<void*>(&FakeDriver);
    case S_PASSENGER: return reinterpret_cast<void*>(&FakePassenger);
    case S_DOOR: return reinterpret_cast<void*>(&FakeDoor);
    case S_ROOM: return reinterpret_cast<void*>(&FakeRoom);
    case S_HEALTHY: return reinterpret_cast<void*>(&FakeHealthy);
    case S_GROUP: return reinterpret_cast<void*>(&FakeGroup);
    case S_LEADER: return reinterpret_cast<void*>(&FakeLeader);
    case S_NEW: return reinterpret_cast<void*>(&FakeNew);
    case S_STILL: return reinterpret_cast<void*>(&FakeStill);
    case S_ADD: return reinterpret_cast<void*>(&FakeAdd);
    case S_C65: return reinterpret_cast<void*>(&FakeC65);
    case S_RANDEXE: return reinterpret_cast<void*>(&FakeRandExe);
    default: return reinterpret_cast<void*>(&FakeRandDll);
    }
}
// vtable falsa de la subtarea: GetTaskType (ranura 4) y MakeAbortable (ranura 6, thiscall 3 argumentos)
inline u32 __fastcall FakeSubType(void* self,void*){Push(EvType,u32(uintptr_t(self)));return gc->subType;}
inline u32 __fastcall FakeSubAbort(void* self,void*,void* ped,u32 prio,void* evt){Push(EvAbort,u32(uintptr_t(self)),u32(uintptr_t(ped)),prio,u32(uintptr_t(evt)));return BoolRet(gc->mAbort,32-1);}
template<unsigned K> u32 __fastcall Trap(void* self,void*){Push(EvTrap,K,u32(uintptr_t(self)));return 0;}
using TrapFn=u32(__fastcall*)(void*,void*);
template<unsigned... I> constexpr std::array<TrapFn,sizeof...(I)> MakeTraps(std::integer_sequence<unsigned,I...>){return {&Trap<I>...};}
inline const std::array<TrapFn,kVtSlots> kTraps=MakeTraps(std::make_integer_sequence<unsigned,kVtSlots>{});
alignas(16) inline std::array<uintptr_t,kVtSlots> vtSub{};
inline void InitVtables() {
    for(unsigned k=0;k<kVtSlots;++k)vtSub[k]=uintptr_t(kTraps[k]);
    vtSub[4]=uintptr_t(&FakeSubType);
    vtSub[6]=uintptr_t(&FakeSubAbort);
}

struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: la funcion de la DLL no tiene RH_ScopedInstall (la entrada del EXE sigue siendo codigo original; dll=0 si no hay cuerpo en la DLL)
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<8&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t<0x1000000)break;p=t;}return p;}

struct RouteInfo {const char* fn;const char* name;uintptr_t entry;u32 argc;};
inline constexpr RouteInfo kInfo[kRoutes]{
    {"ControlSubTask","CTaskComplexKillPedOnFoot::ControlSubTask",0x626260,1},
    {"CreateFirstSubTask","CTaskComplexKillPedOnFoot::CreateFirstSubTask",0x62B490,1},
    {"CreateNextSubTask","CTaskComplexKillPedOnFoot::CreateNextSubTask",0x62B150,1},
    {"CreateSubTask","CTaskComplexKillPedOnFoot::CreateSubTask",0x625E70,2},
    {"MakeAbortable","CTaskComplexKillPedOnFoot::MakeAbortable",0x625E40,3},
};
// Constructores y ayudantes REALES (no se sustituyen): con hook en HEAD deben ser saltos a la DLL en AMBOS lados; sin hook, codigo del EXE
inline constexpr uintptr_t kRealHooked[]{0x5DF8F0,0x420E30,0x62F310,0x463540,0x48E750,0x621000,0x621190,0x5FF940,0x621C00,0x660A30,0x4B2190};
inline constexpr uintptr_t kRealExe[]{0x5FE9C0,0x640430,0x63B8C0,0x4B2210};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::array<std::unique_ptr<HookRoute>,kRoutes> route;
    std::unique_ptr<HookRoute> randRoute;   // 0x407180 con el hook APAGADO: el EXE original usa rand (0x821B1E)
    std::vector<Watch> watches;
    std::array<Pair,kStubs> stub{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,2*kStubs> p;int n{};
    void Add(const Pair& pr,void* stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};std::array<unsigned,kRoutes> perRoute{},failRoute{};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

template<class Fn> Side Execute(Env& env,unsigned routeIdx,bool candidate,Fn gate) {
    Side side{};
    const uintptr_t entry=kInfo[routeIdx].entry;
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const auto& s:env.stub){
            if(s.exe)collab&=(s.hooked?(ReversingProbe::JumpsToDll(s.exe)&&ModuleOf(Resolve(s.exe))==env.asi):!ReversingProbe::JumpsToDll(s.exe));
            collab&=(s.dll==0||ModuleOf(s.dll)==env.asi);
        }
        for(const uintptr_t a:kRealHooked)collab&=ReversingProbe::JumpsToDll(a)&&ModuleOf(Resolve(a))==env.asi;
        for(const uintptr_t a:kRealExe)collab&=!ReversingProbe::JumpsToDll(a);
        for(unsigned r=0;r<kRoutes;++r)if(r!=routeIdx)collab&=!ReversingProbe::JumpsToDll(kInfo[r].entry);   // las otras cuatro de la clase, apagadas (codigo del EXE en ambos lados)
        collab&=!ReversingProbe::JumpsToDll(0x407180);                                                         // GetRandomIntInRange del EXE original
        side.collabOK=collab;
        {
            Patches patches;
            for(unsigned s=0;s<kStubs;++s)patches.Add(env.stub[s],StubFn(s));
            side.reg=gate(entry);
        }
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route[routeIdx]->Invoke(candidate,call);
    return side;
}
inline void PrepareFloatingState() {
    RestoreFloatingState();PrimeX87();
    _controlfp(_PC_24|_RC_NEAR|_MCW_EM,_MCW_PC|_MCW_RC|_MCW_EM);   // 24 bits, al mas cercano, todas las excepciones enmascaradas (estado del juego)
}
inline json CommonRow(const Side& side,const std::array<uint8,108>& fp) {
    return {{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",side.mxcsr&0xFFC0u},{"stack_restored",side.reg.stack},
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"seh_restored",side.reg.seh},{"route_ok",side.routeOK},{"collaborators_dll",side.collabOK}};
}
inline json Finish(Env& env,Totals& t,const char* function,unsigned routeIdx,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    ok&=fa==fb;
    t.passed&=ok;t.failures+=!ok;++t.perRoute[routeIdx];t.failRoute[routeIdx]+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}

inline json EventJson(const Ev& e) {
    switch(e.kind) {
    case EvWanted: return json::array({"wanted",e.a});
    case EvPlayerPed: return json::array({"playerped",e.a});
    case EvNoPolice: return json::array({"nopolice"});
    case EvMelee: return json::array({"melee",Name(e.a)});
    case EvSay: return json::array({"say",Name(e.a),e.b,e.c,e.d,e.e});
    case EvGive: return json::array({"give",Name(e.a),e.b,e.c,e.d});
    case EvSetType: return json::array({"setwtype",Name(e.a),e.b});
    case EvSetSlot: return json::array({"setwslot",Name(e.a),e.b});
    case EvMoveState: return json::array({"movestate",Name(e.a),e.b});
    case EvDrop: return json::array({"drop",Name(e.a),e.b});
    case EvOpen: return json::array({"canopen",Name(e.a),Name(e.b)});
    case EvDriver: return json::array({"isdriver",Name(e.a),Name(e.b)});
    case EvPassenger: return json::array({"ispassenger",Name(e.a),Name(e.b)});
    case EvDoor: return json::array({"door",Name(e.a),Name(e.b)});
    case EvRoom: return json::array({"room",Name(e.a),e.b,e.c});
    case EvHealthy: return json::array({"healthy",Name(e.a)});
    case EvGroup: return json::array({"group",Name(e.a)});
    case EvLeader: return json::array({"leader",Name(e.a)});
    case EvNew: return json::array({"new",e.a});
    case EvStill: return json::array({"still",Name(e.a)});
    case EvAdd: return json::array({"add",Name(e.a),Name(e.b),e.c});
    case EvC65: return json::array({"c65",Name(e.a),Name(e.b)});
    case EvRand: return json::array({"rand"});
    case EvType: return json::array({"type",Name(e.a)});
    case EvAbort: return json::array({"abort",Name(e.a),Name(e.b),e.c,Name(e.d)});
    default: return json::array({"trap",e.a,Name(e.b)});
    }
}
inline json InputJson(const C& c) {
    const auto arr=[](const u32* p,unsigned k){return std::vector<u32>(p,p+k);};
    return {{"route",c.route},{"family",c.family},{"idx",c.idx},{"tflags",c.tflags},{"tgt_null",c.tgtNull},{"ped_flags_t",c.pedFlagsT},{"delay_t",c.delayT},{"chance_t",c.chanceT},{"comp_t",c.compT},{"start_t",c.startT},
        {"time_t",c.timeT},{"tm_start",c.tmStart},{"tm_interval",c.tmInterval},{"tm_started",c.tmStarted},{"tm_stopped",c.tmStopped},{"time_ms",c.timeMs},{"sub_null",c.subNull},{"sub_type",c.subType},
        {"sub_b10",c.subB10},{"sub_b30",c.subB30},{"sub_b33",c.subB33},{"sub_b35",c.subB35},{"p_type",c.pType},{"p_pos",arr(c.pPos,3)},{"p_alt",arr(c.pAlt,3)},{"p_mat",c.pMat},{"p_f46c",c.pF46C},{"p_f474",c.pF474},
        {"p_f478",c.pF478},{"p_created",c.pCreated},{"p_stand",c.pStand},{"p_contact",c.pContact},{"p_veh",c.pVeh},{"p_cop",c.pCop},{"p_slot",c.pSlot},{"p_ammo",arr(c.pAmmo,13)},{"t_type",c.tType},
        {"t_pos",arr(c.tPos,3)},{"t_alt",arr(c.tAlt,3)},{"t_mat",c.tMat},{"t_health",c.tHealth},{"t_f46c",c.tF46C},{"t_stand",c.tStand},{"t_contact",c.tContact},{"t_veh",c.tVeh},{"t_pd_flags",c.tPdFlags},
        {"tw_chance",c.twChance},{"tw_bits",c.twBits},{"wp_chance",c.wpChance},{"wp_bits",c.wpBits},{"vx",c.vx},{"vy",c.vy},{"v_type",c.vType},{"v_sub",c.vSub},{"cp_code",c.cpCode},{"ct_code",c.ctCode},
        {"m_melee",c.mMelee},{"m_driver",c.mDriver},{"m_passenger",c.mPassenger},{"m_can_open",c.mCanOpen},{"m_room",c.mRoom},{"m_healthy",c.mHealthy},{"m_no_police",c.mNoPolice},{"m_abort",c.mAbort},
        {"m_still",c.mStillProc},{"garbage",c.garbage},{"sel_player",c.selPlayer},{"sel_group",c.selGroup},{"sel_leader",c.selLeader},{"door",c.door},{"rand_val",c.randVal},{"prio",c.prio},{"evt",c.evt},
        {"task_id",c.taskId},{"alloc_null",c.allocNull},{"fill_alloc",c.fillAlloc},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack}};
}

// Prepara worldRef (el estado inicial) para el caso
inline void SetupEntity(u8* e) {
    // edificio sin IPL, ni temporal ni procedural: RegisterReference no hace nada (el registro de referencias lo validan los bancos de los constructores)
    auto* ent=reinterpret_cast<CEntity*>(e);
    ent->SetType(ENTITY_TYPE_BUILDING);ent->m_bIsTempBuilding=false;ent->m_bIsProcObject=false;ent->SetIplIndex(0);
    Put32(e+kRefsAt,0u);   // m_pReferences vacia (CleanUpOldReference del destructor del evento recorre la lista)
}
inline void SetupWorld(const C& c) {
    for(u32 r=0;r<kRegions;++r)FillWords(Rf(r),Round16(kRegionBytes[r])+kGuard,Rand(c.idx*7u+c.route,500u+r));
    FillWords(worldRef.data(),kGuard,Rand(c.idx,499));
    std::memset(Rf(R_ALLOC),int(c.fillAlloc),kRegionBytes[R_ALLOC]);
    u8* const self=Rf(R_THIS);
    Put32(self,0x86D894u);
    Put32(self+kSubAt,c.subNull?0u:Addr(R_SUB));
    Put8(self+kFlagsAt,c.tflags);
    Put32(self+kTargetAt,c.tgtNull?0u:Addr(R_TGT));
    Put32(self+kPedFlagsAt,c.pedFlagsT);Put32(self+kDelayAt,c.delayT);Put32(self+kChanceAt,c.chanceT);Put8(self+kCompAt,c.compT);
    Put32(self+kStartAt,c.startT);Put32(self+kTimeAt,c.timeT);Put32(self+kTmStartAt,c.tmStart);Put32(self+kTmIntervalAt,c.tmInterval);Put8(self+kTmStartedAt,c.tmStarted);Put8(self+kTmStoppedAt,c.tmStopped);
    u8* const sub=Rf(R_SUB);
    Put32(sub,u32(uintptr_t(vtSub.data())));
    Put8(sub+0x10,c.subB10);Put8(sub+0x30,c.subB30);Put8(sub+0x33,c.subB33);Put8(sub+0x35,c.subB35);
    // peatones
    constexpr u32 kNone=0xFFFFFFFFu;
    const auto Ped=[&](u32 region,u32 matRegion,bool mat,const u32* pos,const u32* alt,u32 vehRegion,bool veh,u32 f46c,u32 type,u32 stand,u32 contactRegion) {
        u8* const p=Rf(region);u8* const m=Rf(matRegion);
        SetupEntity(p);
        const u32* placement=mat?alt:pos;const u32* inMat=mat?pos:alt;
        for(u32 k=0;k<3;++k){Put32(p+kPosAt+4*k,placement[k]);Put32(m+kMatPosAt+4*k,inMat[k]);}
        Put32(p+kMatrixAt,mat?Addr(matRegion):0u);
        Put32(p+kF46CAt,f46c);Put32(p+kIntelAt,Addr(R_INTEL));
        Put32(p+kStandAt,stand==0?0u:stand==1?Addr(R_LAB):Addr(R_LAB,0x10));
        Put32(p+kContactAt,contactRegion==kNone?0u:Addr(contactRegion));
        Put32(p+kVehAt,veh?Addr(vehRegion):0u);
        Put32(p+kTypeAt,type);
    };
    // contacto del ped: 0 nulo, 1 y 3 su entidad, 2 la del objetivo; contacto del objetivo: 0 nulo, 1 y 3 la suyo, 2 la del ped
    Ped(R_PED,R_PMAT,c.pMat,c.pPos,c.pAlt,R_PVEH,c.pVeh,c.pF46C,c.pType,c.pStand,c.pContact==0?kNone:(c.pContact==2?R_CT:R_CP));
    Ped(R_TGT,R_TMAT,c.tMat,c.tPos,c.tAlt,R_TVEH,c.tVeh,c.tF46C,c.tType,c.tStand,c.tContact==0?kNone:(c.tContact==2?R_CP:R_CT));
    u8* const ped=Rf(R_PED);u8* const tgt=Rf(R_TGT);
    Put32(ped+kF474At,c.pF474);Put32(ped+kF478At,c.pF478);Put8(ped+kCreatedAt,c.pCreated);Put8(ped+kSlotAt,c.pSlot);Put8(ped+kCopAt,c.pCop);
    for(u32 k=0;k<13;++k)Put32(ped+kWeaponsAt+kWeaponSize*k+kAmmoInWeapon,c.pAmmo[k]);
    Put32(tgt+kPDataAt,Addr(R_PD));Put32(tgt+kHealthAt,c.tHealth);
    Put32(Rf(R_PD)+kPdWantedAt,Addr(R_WT));Put8(Rf(R_PD)+kPdFlagsAt,c.tPdFlags);
    Put16(Rf(R_WT)+kWChanceAt,c.twChance);Put8(Rf(R_WT)+kWBitsAt,c.twBits);
    Put16(Rf(R_WP)+kWChanceAt,c.wpChance);Put8(Rf(R_WP)+kWBitsAt,c.wpBits);
    SetupEntity(Rf(R_PVEH));SetupEntity(Rf(R_TVEH));
    u8* const tv=Rf(R_TVEH);
    Put32(tv+kVelAt,c.vx);Put32(tv+kVelAt+4,c.vy);Put32(tv+kVTypeAt,c.vType);Put32(tv+kVSubAt,c.vSub);
    Put8(Rf(R_CP)+kAreaAt,c.cpCode);Put8(Rf(R_CT)+kAreaAt,c.ctCode);
}

// ------------------------------------------------------------------------------------------------ un caso
inline void RunCase(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const unsigned ri=c.route;
    SetupWorld(c);
    const json input=InputJson(c);
    Side sides[2];json rows[2];
    const u32 ped=Addr(R_PED);
    const u32 args5[3]{ri==kSub?c.taskId:(ri==kAbort?ped:ped),ri==kSub?ped:c.prio,c.evt?Addr(R_LAB,0x20):0u};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        world=worldRef;
        *reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))=c.timeMs;
        nev=0;gNewN=0;cnt.fill(0);gc=&c;
        const u32 self=Addr(R_THIS);
        sides[s]=Execute(env,ri,candidate,[&](uintptr_t entry){return Gate(entry,args5,kInfo[ri].argc,c.eax,self,c.edx,c.stack,0);});
        // ---- observacion
        json obs;
        obs["ret"]=ri==kAbort?json(sides[s].reg.eax&0xFFu):Name(sides[s].reg.eax);
        json ej=json::array();
        for(unsigned i=0;i<nev&&i<kMaxEvents;++i)ej.push_back(EventJson(evs[i]));
        obs["events"]=ej;obs["nevents"]=nev;
        json changes=json::array();
        const u32 allocVt=Off(R_ALLOC);
        for(u32 i=0;i<kArena;++i){
            if(world[i]==worldRef[i])continue;
            if(i>=allocVt&&i<allocVt+4)continue;   // la vtable del objeto reservado se compara por GetTaskType
            if(i<RegionOff(0)){changes.push_back({kRegions,i,world[i]});continue;}   // guarda inicial
            u32 r=0;while(r+1<kRegions&&i>=RegionOff(r+1))++r;
            changes.push_back({r,i-RegionOff(r),world[i]});
        }
        obs["changes"]=changes;
        // el objeto reservado: tipo de tarea (llamada virtual a GetTaskType, ranura 4) si el constructor escribio su vtable
        const u32 vt=Get32(W(R_ALLOC));
        const bool built=vt!=Get32(Rf(R_ALLOC));
        obs["alloc_built"]=built;
        if(built){
            const auto fn=reinterpret_cast<u32(__fastcall*)(void*,void*)>(*reinterpret_cast<uintptr_t*>(uintptr_t(vt)+4*4));
            obs["alloc_type"]=fn(W(R_ALLOC),nullptr);
        }
        const bool watchesIntact=env.WatchesIntact();
        const bool timeIntact=*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))==c.timeMs;
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&timeIntact;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,kInfo[ri].name,ri,n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    for(unsigned r=0;r<kRoutes;++r){const uintptr_t body=env.route[r]?env.route[r]->Body():0;ready&=body&&ModuleOf(body)==env.asi;}
    json stubbed=json::array();
    for(const auto& s:env.stub){
        if(s.exe)env.watches.emplace_back(s.exe,16);
        stubbed.push_back({{"exe",s.exe},{"dll",s.dll},{"hooked",s.hooked}});
        ready&=(s.dll!=0||!s.hooked)&&(s.dll==0||ModuleOf(s.dll)==env.asi);
    }
    for(const uintptr_t a:kRealHooked){env.watches.emplace_back(a,16);ready&=ReversingProbe::JumpsToDll(a);}
    for(const uintptr_t a:kRealExe){env.watches.emplace_back(a,16);ready&=!ReversingProbe::JumpsToDll(a);}
    env.watches.emplace_back(0x407180,16);
    ready&=!ReversingProbe::JumpsToDll(0x407180)&&env.randRoute&&env.randRoute->Registered();
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    ready&=uintptr_t(&CTimer::m_snTimeInMilliseconds)==kTimeAddr;
    InitVtables();
    const u32 savedTime=*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr));
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(unsigned r=0;r<kRoutes;++r)cases[kInfo[r].name]=kRouteCases[r];
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kRoutes},{"pairs",kCases},{"cases",cases},{"direct_functions",json::array()},
        {"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real_hooked",std::vector<uintptr_t>(std::begin(kRealHooked),std::end(kRealHooked))},{"exe_real",std::vector<uintptr_t>(std::begin(kRealExe),std::end(kRealExe))},{"stubbed",stubbed}}},
        {"watched_entries",watched},{"bodies",bodies},{"arena_bytes",kArena},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned r=0;r<kRoutes;++r) {
            const unsigned before=totals.failures;
            for(unsigned i=0;i<kRouteCases[r];++i)RunCase(env,totals,RouteStart(r)+i);
            Write({{"event","function_summary"},{"function",kInfo[r].name},{"cases",kRouteCases[r]},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    *reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))=savedTime;
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))==savedTime;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    if(env.randRoute)stateRestored&=env.randRoute->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
