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
// Nucleo del banco FollowFootsteps62. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (CTask, CTaskComplexFollowPedFootsteps, CPed, CEntity, CPointRoute...).
// Plantillas: reversing/PartnerControl60 (ABI, hook Virtual, falsos con parche de entrada en el EXE y en la DLL, vtables falsas de arena) y reversing/PartnerNext60 (constructores reales, operator new falso).
//
// Banco par a par de CINCO funciones virtuales de CTaskComplexFollowPedFootsteps (hooks VIRTUAL: HookRoute con el cuerpo del hook Virtual):
//   ControlSubTask 0x695090, CreateSubTask 0x695E40, CreateNextSubTask 0x694EE0, CreateFirstSubTask 0x695000, MakeAbortable 0x694ED0.
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO el hook de la funcion probada encendido.
// SUSTITUIDOS por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL, resuelto ANTES de escribir ningun parche):
//   CPed::IsAlive 0x5E0170, CWorld::GetIsLineOfSightClear 0x56A490, CTaskManager::GetSimplestActiveTask 0x6819D0, CTask::IsGoToTask 0x61A360 (sin hook: la DLL tiene su cuerpo),
//   CTaskSimpleGoToPoint::UpdatePoint 0x645700 y CTask::operator new 0x61A5A0. REALES en ambos lados (con hook en HEAD, saltos a la DLL): los constructores de CTaskSimpleGoToPoint 0x667CD0,
//   CTaskSimpleStandStill 0x62F310, CTaskSimpleHitHead 0x653060 y CTaskComplexSeekEntityStandard 0x46AC10 (construyen en arenas que devuelve el falso de operator new; se compara el objeto palabra a palabra).
//   El resto (CVector, CPointRoute::AddUnlessFull/Remove 0x48E180/0x5F13A0, Normalise 0x59C910, CTimer) NO se sustituye: EXE contra DLL.
// Las llamadas virtuales (GetTaskType y MakeAbortable de la subtarea, CreateSubTask de `this`) van por vtables falsas de arena; cualquier otra ranura registra un evento "trap".

namespace FollowFootsteps62Live {
using namespace FollowFootsteps62Fix;
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
// Las funciones probadas usan x87 (ControlSubTask, CreateNext/First): se compara FCW completa, TOP, etiquetas y ST validos; los registros vacios se ignoran.
inline constexpr bool kCompareDeadX87Registers=false;
// true: ECX/EDX de salida forman parte de la igualdad. EAX es el retorno (se compara siempre). Los cinco metodos son ranuras de la vtable de CTaskComplex que llaman
// CTaskComplex::ProcessPed / MakeAbortable / CTaskComplex::CreateFirstSubTask... (dispatcher generico) y CreateNext/Control entre si: ningun llamador del EXE lee ECX/EDX de salida (ver README.md).
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

// ------------------------------------------------------------------------------------------------ memoria de los casos
#define FF_ARENA(NAME,SIZE) alignas(16) inline std::array<u8,SIZE> NAME##Arena{},NAME##Ref{}
FF_ARENA(this_,kThisArena);   // CTaskComplexFollowPedFootsteps falso (0x20 bytes), se compara ENTERO
FF_ARENA(sub,kSubArena);      // subtarea falsa (vtable de arena con GetTaskType y MakeAbortable falsos); GoToPoint: +8 = m_moveState
FF_ARENA(ped,kPedArena);      // peaton
FF_ARENA(tgt,kPedArena);      // peaton objetivo (entidad "edificio sin IPL": RegisterReference no hace nada)
FF_ARENA(pedMat,kMatArena);   // matriz del peaton
FF_ARENA(tgtMat,kMatArena);   // matriz del objetivo
FF_ARENA(intel,kIntelArena);  // inteligencia del peaton (solo +0x274 y la direccion +4)
FF_ARENA(route,kRouteArena);  // CPointRoute (0x64)
FF_ARENA(go,kGoArena);        // tarea que devuelve GetSimplestActiveTask (solo se usa su direccion)
FF_ARENA(alloc,kAllocArena);  // tarea que "reserva" operator new; la construyen los constructores reales

struct Ctx {uintptr_t self{},sub{},ped{},tgt{},intel{},route{},go{},alloc{},pedMat{},tgtMat{};};
inline Ctx cx;
inline json Name(uintptr_t v) {
    const auto in=[&](uintptr_t base,u32 bytes,const char* tag)->std::optional<json>{if(base&&v>=base&&v<base+bytes)return std::string(tag)+"+"+std::to_string(v-base);return std::nullopt;};
    if(auto r=in(cx.self,kThisBytes,"T"))return *r;
    if(auto r=in(cx.sub,kSubBytes,"S"))return *r;
    if(auto r=in(cx.ped,kPedBytes,"D"))return *r;
    if(auto r=in(cx.tgt,kPedBytes,"G"))return *r;
    if(auto r=in(cx.intel,kIntelBytes,"I"))return *r;
    if(auto r=in(cx.route,kRouteBytes,"R"))return *r;
    if(auto r=in(cx.go,kGoBytes,"K"))return *r;
    if(auto r=in(cx.alloc,kAllocBytes,"A"))return *r;
    if(auto r=in(cx.pedMat,kMatBytes,"PM"))return *r;
    if(auto r=in(cx.tgtMat,kMatBytes,"TM"))return *r;
    return uint32(v);
}

// ---- falsos. Registro ordenado de eventos (sin reservas ni JSON dentro de los falsos)
enum : u32 {EvAlive=1,EvLos=2,EvType=3,EvAbort=4,EvSimplest=5,EvIsGoTo=6,EvUpdate=7,EvNew=8,EvCreate=9,EvTrap=10};
struct Ev {u32 kind;u32 a[10];};
inline std::array<Ev,kMaxEvents> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a0=0,u32 a1=0,u32 a2=0,u32 a3=0,u32 a4=0,u32 a5=0,u32 a6=0,u32 a7=0,u32 a8=0,u32 a9=0){if(nev<evs.size())evs[nev]=Ev{k,{a0,a1,a2,a3,a4,a5,a6,a7,a8,a9}};++nev;}
inline u32 gAlive{},gLos{},gSimplest{},gIsGoTo{},gAbort{},gSubType{},gNew{},gCreate{};
// 0x5E0170 CPed::IsAlive: thiscall(ped) sin argumentos de pila
inline u32 __fastcall FakeAlive(void* ped,void*){Push(EvAlive,u32(uintptr_t(ped)));return gAlive;}
// 0x56A490 CWorld::GetIsLineOfSightClear: cdecl, 9 argumentos (origen*, destino*, 7 booleanos); se registran los punteros, el contenido de los vectores y el byte bajo de cada booleano
inline u32 __cdecl FakeLos(const float* o,const float* t,u32 a3,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9){
    const u32* ow=reinterpret_cast<const u32*>(o);const u32* tw=reinterpret_cast<const u32*>(t);
    const u32 flags=(a3&0xFFu)|((a4&0xFFu)<<8)|((a5&0xFFu)<<16)|((a6&0xFFu)<<24);
    const u32 flags2=(a7&0xFFu)|((a8&0xFFu)<<8)|((a9&0xFFu)<<16);
    Push(EvLos,u32(uintptr_t(o)),u32(uintptr_t(t)),ow[0],ow[1],ow[2],tw[0],tw[1],tw[2],flags,flags2);
    return gLos;
}
// 0x6819D0 CTaskManager::GetSimplestActiveTask: thiscall(mgr)
inline u32 __fastcall FakeSimplest(void* mgr,void*){Push(EvSimplest,u32(uintptr_t(mgr)));return gSimplest;}
// 0x61A360 CTask::IsGoToTask: cdecl(task)
inline u32 __cdecl FakeIsGoTo(void* task){Push(EvIsGoTo,u32(uintptr_t(task)));return gIsGoTo;}
// 0x645700 CTaskSimpleGoToPoint::UpdatePoint: thiscall(this, const CVector*, float radius, bool) `ret 0xC`
inline void __fastcall FakeUpdate(void* self,void*,const float* pt,u32 radius,u32 flag){const u32* w=reinterpret_cast<const u32*>(pt);Push(EvUpdate,u32(uintptr_t(self)),u32(uintptr_t(pt)),w[0],w[1],w[2],radius,flag&0xFFu);}
// 0x61A5A0 CTask::operator new(size): cdecl
inline u32 __cdecl FakeNew(u32 size){Push(EvNew,size);return gNew;}
// CTask::GetTaskType (vtable +0x10 de la subtarea)
inline u32 __fastcall FakeType(void* self,void*){Push(EvType,u32(uintptr_t(self)));return gSubType;}
// CTask::MakeAbortable (vtable +0x18 de la subtarea): thiscall(this, ped, priority, event) `ret 0xC`
inline u32 __fastcall FakeAbort(void* self,void*,void* ped,u32 priority,void* event){Push(EvAbort,u32(uintptr_t(self)),u32(uintptr_t(ped)),priority,u32(uintptr_t(event)));return gAbort;}
// CTaskComplexFollowPedFootsteps::CreateSubTask (vtable +0x2C de `this`): thiscall(this, type, ped) `ret 8`
inline u32 __fastcall FakeCreate(void* self,void*,u32 type,void* ped){Push(EvCreate,u32(uintptr_t(self)),type,u32(uintptr_t(ped)));return gCreate;}
template<unsigned K> u32 __fastcall Trap(void* self,void*){Push(EvTrap,K,u32(uintptr_t(self)));return 0;}
using TrapFn=u32(__fastcall*)(void*,void*);
template<unsigned... I> constexpr std::array<TrapFn,sizeof...(I)> MakeTraps(std::integer_sequence<unsigned,I...>){return {&Trap<I>...};}
inline const std::array<TrapFn,kVtSlots> kTraps=MakeTraps(std::make_integer_sequence<unsigned,kVtSlots>{});
alignas(16) inline std::array<uintptr_t,kVtSlots> vtThis{},vtSub{};
inline void InitVtables() {
    for(unsigned k=0;k<kVtSlots;++k){vtThis[k]=uintptr_t(kTraps[k]);vtSub[k]=uintptr_t(kTraps[k]);}
    vtThis[11]=uintptr_t(&FakeCreate);
    vtSub[4]=uintptr_t(&FakeType);vtSub[6]=uintptr_t(&FakeAbort);
}

struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: la funcion de la DLL no tiene RH_ScopedInstall (la entrada del EXE sigue siendo codigo original)
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

enum : unsigned {StAlive=0,StLos,StSimplest,StIsGoTo,StUpdate,StNew,kStubs};
inline const std::array<void*,kStubs> kFakes{reinterpret_cast<void*>(&FakeAlive),reinterpret_cast<void*>(&FakeLos),reinterpret_cast<void*>(&FakeSimplest),reinterpret_cast<void*>(&FakeIsGoTo),reinterpret_cast<void*>(&FakeUpdate),reinterpret_cast<void*>(&FakeNew)};
inline constexpr uintptr_t kRealCtors[]{0x667CD0,0x62F310,0x653060,0x46AC10};   // constructores reales (con hook en HEAD): deben ser saltos a la DLL en AMBOS lados

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Fn {const char* name;const char* id;uintptr_t entry;std::vector<unsigned> use;u32 argc;};
struct Env {
    uintptr_t asi{},game{};
    std::array<std::unique_ptr<HookRoute>,kFns> route;
    std::vector<Watch> watches;       // los colaboradores sustituidos y los constructores reales: deben seguir siendo el codigo que eran al empezar
    std::array<Pair,kStubs> stub{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,2*kStubs> p;int n{};
    void Add(const Pair& pr,void* stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    void Reset(){for(int i=n-1;i>=0;--i)p[i].reset();n=0;}
    ~Patches(){Reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). En ambos lados los colaboradores con hook deben ser saltos a la DLL (y los sin hook, codigo del EXE);
// los falsos se instalan DESPUES de comprobar rutas y colaboradores.
template<class Fn2> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,const std::vector<unsigned>& use,Fn2 gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;for(const auto& s:env.stub)collab&=(s.hooked?(ReversingProbe::JumpsToDll(s.exe)&&ModuleOf(Resolve(s.exe))==env.asi):!ReversingProbe::JumpsToDll(s.exe))&&(s.dll==0||ModuleOf(s.dll)==env.asi);
        for(const uintptr_t a:kRealCtors)collab&=ReversingProbe::JumpsToDll(a)&&ModuleOf(Resolve(a))==env.asi;
        side.collabOK=collab;
        {
            Patches patches;
            for(const unsigned i:use)patches.Add(env.stub[i],kFakes[i]);
            side.reg=gate(entry);
        }
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    route.Invoke(candidate,call);
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
inline json Finish(Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    return row;
}

// ------------------------------------------------------------------------------------------------ colocacion de un caso
inline u32 Mem32(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
inline void Put32(u8* p,u32 v){std::memcpy(p,&v,4);}
inline void Put16(u8* p,u16 v){std::memcpy(p,&v,2);}
inline u32 TimerNow(){return *reinterpret_cast<volatile u32*>(kTimerGlobal);}
inline void TimerSet(u32 v){*reinterpret_cast<volatile u32*>(kTimerGlobal)=v;}
inline u32 CallCreate(const C& c,u32 allocPtr){switch(c.createKind){case 0:return allocPtr;case 1:return 0u;default:return c.createVal;}}

// Escribe en las arenas de referencia (*Ref) el estado inicial del caso; las arenas vivas se copian de ellas antes de cada pasada.
inline void PlaceCase(const C& c,unsigned n) {
    FillWords(this_Ref.data(),kThisArena,Rand(n,50));FillWords(subRef.data(),kSubArena,Rand(n,51));FillWords(pedRef.data(),kPedArena,Rand(n,52));FillWords(tgtRef.data(),kPedArena,Rand(n,53));
    FillWords(pedMatRef.data(),kMatArena,Rand(n,54));FillWords(tgtMatRef.data(),kMatArena,Rand(n,55));FillWords(intelRef.data(),kIntelArena,Rand(n,56));FillWords(routeRef.data(),kRouteArena,Rand(n,57));
    FillWords(goRef.data(),kGoArena,Rand(n,58));FillWords(allocRef.data(),kAllocArena,Rand(n,59));
    u8* const rt=this_Ref.data()+kThisOff;u8* const rs=subRef.data()+kSubOff;u8* const rp=pedRef.data()+kPedOff;u8* const rg=tgtRef.data()+kPedOff;
    u8* const rpm=pedMatRef.data()+kMatOff;u8* const rgm=tgtMatRef.data()+kMatOff;u8* const ri=intelRef.data()+kIntelOff;u8* const rr=routeRef.data()+kRouteOff;
    // this
    Put32(rt,u32(uintptr_t(vtThis.data())));
    Put32(rt+kSubAtThis,u32(uintptr_t(cx.sub)));
    Put32(rt+kTargetAt,c.tgtNull?0u:u32(uintptr_t(cx.tgt)));
    Put32(rt+kCreateTimerAt,c.now-c.createTimerDelta);
    Put32(rt+kLosTimerAt,c.now-c.losTimerDelta);
    Put32(rt+kRouteAt,c.routeNull?0u:u32(uintptr_t(cx.route)));
    Put16(rt+kMoveAt,u16(c.moveState));
    rt[kFlagsAt]=u8(c.flagsByte);
    // subtarea
    Put32(rs,u32(uintptr_t(vtSub.data())));
    Put32(rs+kSubMoveAt,0x5A5A0000u+Rand(n,60)%0x10000u);
    // peaton y objetivo: la posicion activa (matriz o placement) = ped/tgt; la otra = alternativa
    const auto SetPos=[&](u8* ped,u8* mat,u32 matPtr,bool useMat,const u32* pos,const u32* alt){
        Put32(ped+kMatAt,useMat?matPtr:0u);
        for(int i=0;i<3;++i){Put32(ped+kPlaceAt+4*i,useMat?alt[i]:pos[i]);Put32(mat+kMatPosAt+4*i,useMat?pos[i]:alt[i]);}
    };
    SetPos(rp,rpm,u32(uintptr_t(cx.pedMat)),c.pedMat,c.ped,c.pedAlt);
    SetPos(rg,rgm,u32(uintptr_t(cx.tgtMat)),c.tgtMat,c.tgt,c.tgtAlt);
    Put32(rp+kIntelAt,u32(uintptr_t(cx.intel)));
    Put32(rg+kIntelAt,u32(uintptr_t(cx.intel)));
    Put32(rg+kPedFlagsAt,(c.tgtNoise&~0x100u)|(c.tgtInVeh?0x100u:0u));   // bit 8 = bInVehicle
    Put32(ri+kCounterAt,c.counter);
    // objetivo = "edificio" sin IPL, ni temporal ni procedural: CEntity::RegisterReference no hace nada (como en PartnerNext60)
    {auto* e=reinterpret_cast<CEntity*>(rg);e->SetType(ENTITY_TYPE_BUILDING);e->m_bIsTempBuilding=false;e->m_bIsProcObject=false;e->SetIplIndex(0);}
    // ruta
    Put32(rr,c.count);
    for(u32 i=0;i<24;++i)Put32(rr+4+4*i,c.ent[i]);
}

// ------------------------------------------------------------------------------------------------ ejecucion de un caso (cualquiera de las cinco funciones)
inline json EventJson(const Ev& e) {
    const auto V3=[&](unsigned i){return json::array({e.a[i],e.a[i+1],e.a[i+2]});};
    switch(e.kind) {
    case EvAlive: return json::array({"alive",Name(e.a[0])});
    case EvLos: return json::array({"los",Name(e.a[0]),V3(2),Name(e.a[1]),V3(5),e.a[8],e.a[9]});
    case EvType: return json::array({"type",Name(e.a[0])});
    case EvAbort: return json::array({"abort",Name(e.a[0]),Name(e.a[1]),e.a[2],e.a[3]});
    case EvSimplest: return json::array({"simplest",Name(e.a[0])});
    case EvIsGoTo: return json::array({"isgoto",Name(e.a[0])});
    case EvUpdate: return json::array({"update",Name(e.a[0]),Name(e.a[1]),V3(2),e.a[5],e.a[6]});
    case EvNew: return json::array({"new",e.a[0]});
    case EvCreate: return json::array({"create",Name(e.a[0]),e.a[1],Name(e.a[2])});
    default: return json::array({"trap",e.a[0],Name(e.a[1])});
    }
}
template<size_t N> inline json DiffList(const std::array<u8,N>& cur,const std::array<u8,N>& ref,u32 objOff) {
    json out=json::array();for(u32 i=0;i<N;++i)if(cur[i]!=ref[i])out.push_back({int(i)-int(objOff),cur[i]});return out;
}

inline const std::array<Fn,kFns>& Fns() {
    static const std::array<Fn,kFns> f{{
        {"CTaskComplexFollowPedFootsteps::ControlSubTask","ControlSubTask",0x695090,{StAlive,StLos,StSimplest,StIsGoTo,StUpdate},1},
        {"CTaskComplexFollowPedFootsteps::CreateSubTask","CreateSubTask",0x695E40,{StNew},2},
        {"CTaskComplexFollowPedFootsteps::CreateNextSubTask","CreateNextSubTask",0x694EE0,{},1},
        {"CTaskComplexFollowPedFootsteps::CreateFirstSubTask","CreateFirstSubTask",0x695000,{},1},
        {"CTaskComplexFollowPedFootsteps::MakeAbortable","MakeAbortable",0x694ED0,{},3},
    }};
    return f;
}

inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    const C c=MakeC(fn,n);
    const Fn& F=Fns()[fn];
    cx.self=uintptr_t(this_Arena.data()+kThisOff);cx.sub=uintptr_t(subArena.data()+kSubOff);cx.ped=uintptr_t(pedArena.data()+kPedOff);cx.tgt=uintptr_t(tgtArena.data()+kPedOff);
    cx.intel=uintptr_t(intelArena.data()+kIntelOff);cx.route=uintptr_t(routeArena.data()+kRouteOff);cx.go=uintptr_t(goArena.data()+kGoOff);cx.alloc=uintptr_t(allocArena.data()+kAllocOff);
    cx.pedMat=uintptr_t(pedMatArena.data()+kMatOff);cx.tgtMat=uintptr_t(tgtMatArena.data()+kMatOff);
    PlaceCase(c,n);
    gAlive=c.aliveRet;gLos=c.losRet;gSimplest=c.simplestNull?0u:u32(cx.go);gIsGoTo=c.isGoToRet;gAbort=c.abortRet;gSubType=c.subType;
    gNew=c.newNull?0u:u32(cx.alloc);gCreate=CallCreate(c,u32(cx.alloc));
    const u32 savedTimer=TimerNow();
    json input={{"fn",c.fn},{"family",c.family},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack},{"tgt_null",c.tgtNull},{"route_null",c.routeNull},{"move_state",c.moveState&0xFFFFu},
        {"flags_byte",c.flagsByte&0xFFu},{"create_timer_delta",c.createTimerDelta},{"los_timer_delta",c.losTimerDelta},{"now",c.now},{"tgt_mat",c.tgtMat},{"ped_mat",c.pedMat},
        {"tgt_in_veh",c.tgtInVeh},{"tgt_noise",c.tgtNoise},{"counter",c.counter},{"tgt",std::vector<u32>(c.tgt,c.tgt+3)},{"tgt_alt",std::vector<u32>(c.tgtAlt,c.tgtAlt+3)},
        {"ped",std::vector<u32>(c.ped,c.ped+3)},{"ped_alt",std::vector<u32>(c.pedAlt,c.pedAlt+3)},{"count",c.count},{"ent",std::vector<u32>(c.ent,c.ent+24)},
        {"sub_type",c.subType},{"alive_ret",c.aliveRet},{"los_ret",c.losRet},{"simplest_null",c.simplestNull},{"is_goto_ret",c.isGoToRet},{"abort_ret",c.abortRet},
        {"create_kind",c.createKind},{"create_val",c.createVal},{"new_null",c.newNull},{"type",c.type},{"prio",c.prio},{"ev_ptr",c.evPtr},{"ped_arg",c.pedArg},
        {"sub_move0",Mem32(subRef.data()+kSubOff+kSubMoveAt)}};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        this_Arena=this_Ref;subArena=subRef;pedArena=pedRef;tgtArena=tgtRef;pedMatArena=pedMatRef;tgtMatArena=tgtMatRef;intelArena=intelRef;routeArena=routeRef;goArena=goRef;allocArena=allocRef;
        nev=0;TimerSet(c.now);
        u32 args[3]{};u32 popBytes=0;
        switch(fn) {
        case FnControl: case FnNext: case FnFirst: args[0]=u32(cx.ped);break;
        case FnCreateSub: args[0]=c.type;args[1]=u32(cx.ped);break;
        default: args[0]=c.pedArg;args[1]=c.prio;args[2]=c.evPtr;break;
        }
        sides[s]=Execute(env,*env.route[fn],F.entry,candidate,F.use,[&](uintptr_t entry){return Gate(entry,args,F.argc,c.eax,u32(cx.self),c.edx,c.stack,popBytes);});
        const u32 timerAfter=TimerNow();TimerSet(savedTimer);

        // ---- observacion
        json obs;
        if(fn==FnAbortable)obs["ret_al"]=sides[s].reg.eax&0xFFu;else obs["ret"]=Name(sides[s].reg.eax);
        json ej=json::array();
        for(unsigned i=0;i<nev&&i<kMaxEvents;++i)ej.push_back(EventJson(evs[i]));
        obs["events"]=ej;obs["nevents"]=nev;
        obs["this_changes"]=DiffList(this_Arena,this_Ref,kThisOff);
        obs["route_changes"]=DiffList(routeArena,routeRef,kRouteOff);
        obs["sub_move"]=Mem32(subArena.data()+kSubOff+kSubMoveAt);
        {auto tmp=subArena;std::memcpy(tmp.data()+kSubOff+kSubMoveAt,subRef.data()+kSubOff+kSubMoveAt,4);obs["sub_rest_intact"]=tmp==subRef;}
        if(fn==FnCreateSub) {
            json words=json::array();for(u32 i=0;i<kAllocBytes;i+=4)words.push_back(Name(Mem32(allocArena.data()+kAllocOff+i)));
            obs["alloc"]=words;
            auto tmp=allocArena;std::memcpy(tmp.data()+kAllocOff,allocRef.data()+kAllocOff,kAllocBytes);obs["alloc_guards_intact"]=tmp==allocRef;
        }
        obs["others_intact"]=pedArena==pedRef&&tgtArena==tgtRef&&pedMatArena==pedMatRef&&tgtMatArena==tgtMatRef&&intelArena==intelRef&&goArena==goRef&&(fn==FnCreateSub||allocArena==allocRef);
        obs["timer"]=timerAfter;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&TimerNow()==savedTimer;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,F.name,n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    for(unsigned i=0;i<kFns;++i){const uintptr_t body=env.route[i]?env.route[i]->Body():0;ready&=body&&ModuleOf(body)==env.asi;}
    json stubbed=json::array();
    for(const auto& s:env.stub){env.watches.emplace_back(s.exe,16);stubbed.push_back({{"exe",s.exe},{"dll",s.dll},{"hooked",s.hooked}});ready&=s.dll!=0&&ModuleOf(s.dll)==env.asi;}
    for(const uintptr_t a:kRealCtors){env.watches.emplace_back(a,16);ready&=ReversingProbe::JumpsToDll(a);}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    InitVtables();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const auto& f:Fns())cases[f.name]=kCasesPer;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kCasesPer*kFns},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",std::vector<uintptr_t>(std::begin(kRealCtors),std::end(kRealCtors))},{"stubbed",stubbed}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(u32 fn=0;fn<kFns;++fn) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCasesPer;++c)RunCase(env,totals,fn,c);
            Write({{"event","function_summary"},{"function",Fns()[fn].name},{"cases",kCasesPer},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
