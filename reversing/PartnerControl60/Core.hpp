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
// Nucleo del banco PartnerControl60: CTaskComplexPartner::ControlSubTask (0x6840D0). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (CTask, CTaskComplexPartner, CTaskManager, CPed, IKChainManager_c).
// Plantillas: reversing/PartnerControl60 y reversing/PartnerFirstSub31 (ABI, hook Virtual, falsos con parche de entrada en el EXE y en la DLL, argumentos de pila).
//
// Banco par a par de UNA funcion, CTaskComplexPartner::ControlSubTask (0x6840D0, hook virtual: HookRoute con el cuerpo del hook Virtual).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Los cuatro colaboradores no virtuales (FindActiveTaskByType 0x681740, DropEntityThatThisPedIsHolding 0x5E0360,
// IKChainManager_c::IsLooking 0x6181A0 y AbortLookAt 0x618280) estan reconstruidos y enganchados: se SUSTITUYEN por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo
// de la DLL, resuelto ANTES de escribir ningun parche). Las tres llamadas virtuales (GetTaskType y MakeAbortable de la subtarea, StreamRequiredAnims de `this`) van por vtables falsas de arena: la subtarea y
// `this` son objetos de arena con un puntero a esas vtables; cualquier ranura no prevista registra un evento "trap". Cada falso registra sus argumentos EN ORDEN.

namespace PartnerControl60Live {
using namespace PartnerControl60Fix;
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
// La funcion no usa x87 (empuja 4.0f como entero), pero sus colaboradores (constructores) pueden: se compara lo observable (FCW completa, TOP, etiquetas y ST validos).
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. EAX es el retorno (se compara siempre, como etiqueta, en `observed.ret`); ECX/EDX no los lee ningun llamador
// (unico llamador: CTaskComplexPartner::ControlSubTask, `call [edx+2Ch]` en 0x684166, que sobrescribe ECX con `mov ecx,0C15448h` y no lee EDX), por defecto solo se registran.
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada (esta funcion no instala marco SEH; se comprueba igualmente).
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Igual que PartnerChatSeq30::Gate (la funcion es thiscall sin parametros y `ret`, void): siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; captura los registros y EFLAGS justo tras el `call`.
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
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};   // CTaskComplexPartner falso (0x74 bytes, el word +0x70 es de las derivadas), se compara ENTERO
alignas(16) inline std::array<u8,kSubArena> subArena{},subRef{};      // subtarea falsa (vtable de arena con GetTaskType y MakeAbortable falsos)
alignas(16) inline std::array<u8,kPedArena> pedArena{},pedRef{};      // ped opaco: la funcion solo usa su direccion
alignas(16) inline std::array<u8,kPartArena> partArena{},partRef{};   // partner falso: solo se lee [+0x47C]
alignas(16) inline std::array<u8,kIntelArena> intelArena{},intelRef{};// inteligencia falsa: solo su direccion (+4 = administrador de tareas)
alignas(16) inline std::array<u8,kOtherArena> otherArena{},otherRef{};// tarea equivalente del partner: solo se lee [+0x38]

// Etiquetas para punteros: las arenas cambian de direccion entre ejecuciones, no de papel
struct Ctx {uintptr_t self{},sub{},ped{},part{},intel{},other{};};
inline Ctx cx;
inline json Name(uintptr_t v) {
    const auto in=[&](uintptr_t base,u32 bytes,const char* tag)->json{return "";};
    (void)in;
    if(v>=cx.self&&v<cx.self+kThisBytes)return "T+"+std::to_string(v-cx.self);
    if(v>=cx.sub&&v<cx.sub+kSubBytes)return "S+"+std::to_string(v-cx.sub);
    if(v>=cx.ped&&v<cx.ped+kPedBytes)return "D+"+std::to_string(v-cx.ped);
    if(v>=cx.part&&v<cx.part+kPartBytes)return "P+"+std::to_string(v-cx.part);
    if(v>=cx.intel&&v<cx.intel+kIntelBytes)return "I+"+std::to_string(v-cx.intel);
    if(v>=cx.other&&v<cx.other+kOtherBytes)return "O+"+std::to_string(v-cx.other);
    return uint32(v);
}

// ---- falsos. Registro ordenado de eventos (sin reservas ni JSON dentro de los falsos)
enum : u32 {EvFind=1,EvAbort=2,EvType=3,EvStream=4,EvDrop=5,EvLook=6,EvAbortLook=7,EvTrap=8};
struct Ev {u32 kind,a,b,c,d;};
inline std::array<Ev,kMaxEvents> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0){if(nev<evs.size())evs[nev]=Ev{k,a,b,c,d};++nev;}
inline u32 gFound{},gSubType{},gLooking{},gAbortN{};inline std::array<u32,2> gAbort{};
// 0x681740 CTaskManager::FindActiveTaskByType: thiscall(taskMgr, type) `ret 4` = __fastcall(ecx, edx, type)
inline u32 __fastcall FakeFind(void* mgr,void*,u32 type){Push(EvFind,u32(uintptr_t(mgr)),type);return gFound;}
// CTask::MakeAbortable (vtable +0x18 de la subtarea): thiscall(this, ped, priority, event) `ret 0xC`
inline u32 __fastcall FakeAbort(void* self,void*,void* ped,u32 priority,void* event){Push(EvAbort,u32(uintptr_t(self)),u32(uintptr_t(ped)),priority,u32(uintptr_t(event)));return gAbort[(gAbortN++)&1];}
// CTask::GetTaskType (vtable +0x10 de la subtarea)
inline u32 __fastcall FakeType(void* self,void*){Push(EvType,u32(uintptr_t(self)));return gSubType;}
// CTaskComplexPartner::StreamRequiredAnims (vtable +0x2C de `this`)
inline void __fastcall FakeStream(void* self,void*){Push(EvStream,u32(uintptr_t(self)));}
// 0x5E0360 CPed::DropEntityThatThisPedIsHolding(bool): thiscall con un argumento de pila (se registra solo el byte bajo)
inline void __fastcall FakeDrop(void* ped,void*,u32 flag){Push(EvDrop,u32(uintptr_t(ped)),flag&0xFFu);}
// 0x6181A0 IKChainManager_c::IsLooking(ped): thiscall(ik, ped)
inline u32 __fastcall FakeLooking(void* ik,void*,void* ped){Push(EvLook,u32(uintptr_t(ik)),u32(uintptr_t(ped)));return gLooking;}
// 0x618280 IKChainManager_c::AbortLookAt(ped, blend): thiscall(ik, ped, blend)
inline void __fastcall FakeAbortLook(void* ik,void*,void* ped,u32 blend){Push(EvAbortLook,u32(uintptr_t(ik)),u32(uintptr_t(ped)),blend);}
// Ranuras no previstas de las vtables falsas: se registran (el oraculo no las espera) para que un cambio de ranura en la DLL salte a la vista
template<unsigned K> u32 __fastcall Trap(void* self,void*){Push(EvTrap,K,u32(uintptr_t(self)));return 0;}
using TrapFn=u32(__fastcall*)(void*,void*);
template<unsigned... I> constexpr std::array<TrapFn,sizeof...(I)> MakeTraps(std::integer_sequence<unsigned,I...>){return {&Trap<I>...};}
inline const std::array<TrapFn,kVtSlots> kTraps=MakeTraps(std::make_integer_sequence<unsigned,kVtSlots>{});
alignas(16) inline std::array<uintptr_t,kVtSlots> vtThis{},vtSub{};
inline void InitVtables() {
    for(unsigned k=0;k<kVtSlots;++k){vtThis[k]=uintptr_t(kTraps[k]);vtSub[k]=uintptr_t(kTraps[k]);}
    vtThis[11]=uintptr_t(&FakeStream);
    vtSub[4]=uintptr_t(&FakeType);vtSub[6]=uintptr_t(&FakeAbort);
}

struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: la funcion de la DLL no tiene RH_ScopedInstall (la entrada del EXE sigue siendo codigo original)
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
inline constexpr unsigned kStubs=4;
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;       // los colaboradores sustituidos: deben seguir siendo el codigo que eran al empezar
    std::array<Pair,kStubs> stub{};   // 0 Find, 1 Drop, 2 IsLooking, 3 AbortLookAt (cuerpo de la DLL resuelto ANTES de parchear)
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,2*kStubs> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). En ambos lados los colaboradores con hook deben ser saltos a la DLL (y los sin hook, codigo del EXE);
// los falsos se instalan DESPUES de comprobar rutas y colaboradores.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;for(const auto& s:env.stub)collab&=(s.hooked?(ReversingProbe::JumpsToDll(s.exe)&&ModuleOf(Resolve(s.exe))==env.asi):!ReversingProbe::JumpsToDll(s.exe))&&(s.dll==0||ModuleOf(s.dll)==env.asi);
        side.collabOK=collab;
        {
            Patches patches;
            patches.Add(env.stub[0],&FakeFind);
            patches.Add(env.stub[1],&FakeDrop);
            patches.Add(env.stub[2],&FakeLooking);
            patches.Add(env.stub[3],&FakeAbortLook);
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
inline json Finish(Env& env,Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}

// ------------------------------------------------------------------------------------------------ CTaskComplexPartner::ControlSubTask
inline json EventJson(const Ev& e) {
    switch(e.kind) {
    case EvFind: return json::array({"find",Name(e.a),e.b});
    case EvAbort: return json::array({"abort",Name(e.a),Name(e.b),e.c,e.d});
    case EvType: return json::array({"type",Name(e.a)});
    case EvStream: return json::array({"stream",Name(e.a)});
    case EvDrop: return json::array({"drop",Name(e.a),e.b});
    case EvLook: return json::array({"looking",e.a,Name(e.b)});
    case EvAbortLook: return json::array({"abort_look",e.a,Name(e.b),e.c});
    default: return json::array({"trap",e.a,Name(e.b)});
    }
}
inline void CaseControl(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    u8* const self=thisArena.data()+kThisOff;u8* const sub=subArena.data()+kSubOff;u8* const ped=pedArena.data()+kPedOff;
    u8* const part=partArena.data()+kPartOff;u8* const intel=intelArena.data()+kIntelOff;u8* const other=otherArena.data()+kOtherOff;
    cx.self=uintptr_t(self);cx.sub=uintptr_t(sub);cx.ped=uintptr_t(ped);cx.part=uintptr_t(part);cx.intel=uintptr_t(intel);cx.other=uintptr_t(other);

    FillWords(thisRef.data(),kThisArena,Rand(n,50));FillWords(subRef.data(),kSubArena,Rand(n,51));FillWords(pedRef.data(),kPedArena,Rand(n,52));
    FillWords(partRef.data(),kPartArena,Rand(n,53));FillWords(intelRef.data(),kIntelArena,Rand(n,54));FillWords(otherRef.data(),kOtherArena,Rand(n,55));
    u8* const rs=thisRef.data()+kThisOff;
    const u32 vt=u32(uintptr_t(vtThis.data()));std::memcpy(rs,&vt,4);
    const u32 subPtr=u32(uintptr_t(sub));std::memcpy(rs+kSubAtThis,&subPtr,4);
    std::memcpy(rs+kTaskIdAt,&c.taskId,4);
    const u32 partnerPtr=c.partnerNull?0u:u32(uintptr_t(part));std::memcpy(rs+kPartnerAt,&partnerPtr,4);
    rs[kStateAt]=u8(c.state);rs[kCompletedAt]=u8(c.completed);
    const u16 counter=u16(c.counter);std::memcpy(rs+kCounterAt,&counter,2);
    const u32 svt=u32(uintptr_t(vtSub.data()));std::memcpy(subRef.data()+kSubOff,&svt,4);
    const u32 intelPtr=u32(uintptr_t(intel));std::memcpy(partRef.data()+kPartOff+kIntelAtPartner,&intelPtr,4);
    const u32 otherPartner=c.otherIsPed?u32(uintptr_t(ped)):(Rand(n,56)%2?u32(uintptr_t(part)):0xA5A50000u+Rand(n,57)%0x1000u);   // lo que ve la comparacion `other->m_partner == ped`
    std::memcpy(otherRef.data()+kOtherOff+kOtherPartnerAt,&otherPartner,4);
    gFound=c.found?u32(uintptr_t(other)):0u;gSubType=c.subType;gLooking=c.looking;gAbort={c.abort1,c.abort2};

    const json input={{"family",c.family},{"state",c.state},{"partner_null",c.partnerNull},{"completed",c.completed},{"found",c.found},{"other_is_ped",c.otherIsPed},
        {"abort1",c.abort1},{"abort2",c.abort2},{"sub_type",c.subType},{"counter",c.counter},{"looking",c.looking},{"task_id",c.taskId},
        {"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack},{"this_off",kThisOff}};
    Side sides[2];json rows[2];
    const u32 args[1]{u32(uintptr_t(ped))};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        thisArena=thisRef;subArena=subRef;pedArena=pedRef;partArena=partRef;intelArena=intelRef;otherArena=otherRef;
        nev=0;gAbortN=0;
        sides[s]=Execute(env,*env.route,0x6840D0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(uintptr_t(self)),c.edx,c.stack,0);});

        // ---- observacion
        json obs;
        obs["ret"]=Name(sides[s].reg.eax);
        json ej=json::array();
        for(unsigned i=0;i<nev&&i<kMaxEvents;++i)ej.push_back(EventJson(evs[i]));
        obs["events"]=ej;obs["nevents"]=nev;
        json thisChanges=json::array();
        for(u32 i=0;i<kThisArena;++i)if(thisArena[i]!=thisRef[i])thisChanges.push_back({int(i)-int(kThisOff),thisArena[i]});
        obs["this_changes"]=thisChanges;
        obs["others_intact"]=subArena==subRef&&pedArena==pedRef&&partArena==partRef&&intelArena==intelRef&&otherArena==otherRef;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CTaskComplexPartner::ControlSubTask",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    json stubbed=json::array();
    for(const auto& s:env.stub){env.watches.emplace_back(s.exe,16);stubbed.push_back({{"exe",s.exe},{"dll",s.dll},{"hooked",s.hooked}});ready&=s.dll!=0&&ModuleOf(s.dll)==env.asi;}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    ready&=uintptr_t(&g_ikChainMan)==kIkGlobal;
    InitVtables();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskComplexPartner::ControlSubTask",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",json::array()},{"stubbed",stubbed}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseControl(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskComplexPartner::ControlSubTask"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
