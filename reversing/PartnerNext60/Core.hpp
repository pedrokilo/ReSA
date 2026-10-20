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
// Nucleo del banco PartnerNext60: CTaskComplexPartner::CreateNextSubTask (0x683AD0). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (CTask, CTaskComplexPartner, CTaskManager, CPed, CEntity, CTimer y las tareas creadas).
// Plantillas: reversing/PartnerNext60, reversing/PartnerFirstSub31 y reversing/PartnerDealSeq31 (ABI, hook Virtual, falsos con parche de entrada en el EXE y en la DLL, argumentos de pila, constructores reales).
//
// Banco par a par de UNA funcion, CTaskComplexPartner::CreateNextSubTask (0x683AD0, hook virtual: HookRoute con el cuerpo del hook Virtual).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo
// de la DLL, resuelto ANTES de escribir ningun parche): FindActiveTaskByType 0x681740, GetNodeHeadingFromVector 0x53CDC0, CTask::operator new 0x61A5A0, CTaskManager::SetTask 0x681AF0 y 0x681FE0
// (sin reconstruir: solo EXE). Reales en ambos lados: los constructores de CTaskSimpleStandStill 0x62F310, CTaskComplexWanderStandard 0x48E4F0, CTaskComplexGoToPointAndStandStill 0x668120 y
// CTaskComplexTurnToFaceEntityOrCoord 0x66B890 (construyen en arenas que devuelve el falso de operator new; se compara el objeto palabra a palabra). Las llamadas virtuales (GetPartnerSequence de `this`,
// GetTaskType de la subtarea y de la tarea por defecto, GetWanderType) van por vtables falsas de arena. Los ayudantes internos del EXE sin hook (0x6822B0, 0x41CCB0, 0x44E480, 0x66B150) son codigo
// original en ambos lados: el C++ los reproduce y el banco lo comprueba por sus efectos. CTimer::m_snTimeInMilliseconds (0xB7CB84) se fija por caso y se restaura.

namespace PartnerNext60Live {
using namespace PartnerNext60Fix;
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
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};     // CTaskComplexPartner falso (0x74 bytes, el word +0x70 es de las derivadas), se compara ENTERO
alignas(16) inline std::array<u8,kPedArena> pedArena{},pedRef{};        // ped falso: +4 posicion, +0x10 rumbo, +0x14 matriz, +0x47C inteligencia; escribe +0x4D8/+0x4DC
alignas(16) inline std::array<u8,kPedArena> partArena{},partRef{};      // partner falso (tambien es la "entidad" que recibe TurnToFace; solo se lee)
alignas(16) inline std::array<u8,kMatArena> pedMatArena{},pedMatRef{};  // matriz del ped (right +0, forward +0x10, posicion +0x30)
alignas(16) inline std::array<u8,kMatArena> partMatArena{},partMatRef{};// matriz del partner
alignas(16) inline std::array<u8,kIntelArena> pedIntelArena{},pedIntelRef{};   // inteligencia del ped: [+0x14] = tarea por defecto; +4 = administrador (SetTask)
alignas(16) inline std::array<u8,kIntelArena> partIntelArena{},partIntelRef{}; // inteligencia del partner: +4 = administrador (Find)
alignas(16) inline std::array<u8,kTaskArena> subArena{},subRef{};       // subtarea falsa (vtable con GetTaskType)
alignas(16) inline std::array<u8,kTaskArena> defArena{},defRef{};       // tarea por defecto falsa (vtable con GetTaskType y GetWanderType)
alignas(16) inline std::array<u8,kOtherArena> otherArena{},otherRef{};  // tarea equivalente del partner
alignas(16) inline std::array<u8,kAllocArena> alloc0Arena{},alloc0Ref{},alloc1Arena{},alloc1Ref{};   // objetos que "reserva" el falso de operator new
alignas(16) inline std::array<u8,kSeqArena> seqArena{},seqRef{};        // secuencia falsa (solo su direccion)

// Etiquetas para punteros: las arenas cambian de direccion entre ejecuciones, no de papel
struct Ctx {uintptr_t self{},ped{},part{},pedMat{},partMat{},pedIntel{},partIntel{},sub{},def{},other{},a0{},a1{},seq{};};
inline Ctx cx;
inline json Name(uintptr_t v) {
    const auto in=[&](uintptr_t base,u32 bytes,const char* tag)->std::optional<json>{if(v>=base&&v<base+bytes)return json(std::string(tag)+"+"+std::to_string(v-base));return std::nullopt;};
    if(auto r=in(cx.self,kThisBytes,"T"))return *r;
    if(auto r=in(cx.ped,kPedBytes,"D"))return *r;
    if(auto r=in(cx.part,kPedBytes,"P"))return *r;
    if(auto r=in(cx.pedMat,kMatBytes,"M"))return *r;
    if(auto r=in(cx.partMat,kMatBytes,"N"))return *r;
    if(auto r=in(cx.pedIntel,kIntelBytes,"J"))return *r;
    if(auto r=in(cx.partIntel,kIntelBytes,"I"))return *r;
    if(auto r=in(cx.sub,kTaskBytes,"S"))return *r;
    if(auto r=in(cx.def,kTaskBytes,"W"))return *r;
    if(auto r=in(cx.other,kOtherBytes,"O"))return *r;
    if(auto r=in(cx.a0,kAllocBytes,"A0"))return *r;
    if(auto r=in(cx.a1,kAllocBytes,"A1"))return *r;
    if(auto r=in(cx.seq,kSeqBytes,"Q"))return *r;
    return uint32(v);
}

// ---- falsos. Registro ordenado de eventos (sin reservas ni JSON dentro de los falsos)
enum : u32 {EvFind=1,EvSeq=2,EvType=3,EvWType=4,EvNode=5,EvNew=6,EvSetTask=7,EvCalc=8,EvTrap=9};
struct Ev {u32 kind,a,b,c,d,e;};
inline std::array<Ev,kMaxEvents> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0){if(nev<evs.size())evs[nev]=Ev{k,a,b,c,d,e};++nev;}
inline u32 gFound{},gSubType{},gDefType{},gWanderType{},gSeqRet{},gHeadingRet{},gNewN{};
inline std::array<u32,2> gAllocPtr{};inline std::array<u32,3> gCalc{};
// 0x681740 CTaskManager::FindActiveTaskByType: thiscall(taskMgr, type) `ret 4` = __fastcall(ecx, edx, type)
inline u32 __fastcall FakeFind(void* mgr,void*,u32 type){Push(EvFind,u32(uintptr_t(mgr)),type);return gFound;}
// CTaskComplexPartner::GetPartnerSequence (vtable +0x34 de `this`)
inline u32 __fastcall FakeSeq(void* self,void*){Push(EvSeq,u32(uintptr_t(self)));return gSeqRet;}
// CTask::GetTaskType (vtable +0x10) de la subtarea y de la tarea por defecto
inline u32 __fastcall FakeTypeSub(void* self,void*){Push(EvType,u32(uintptr_t(self)));return gSubType;}
inline u32 __fastcall FakeTypeDef(void* self,void*){Push(EvType,u32(uintptr_t(self)));return gDefType;}
// CTaskComplexWander::GetWanderType (vtable +0x2C de la tarea por defecto)
inline u32 __fastcall FakeWType(void* self,void*){Push(EvWType,u32(uintptr_t(self)));return gWanderType;}
// 0x53CDC0 CGeneral::GetNodeHeadingFromVector(float x, float y), cdecl: se registran los bits de los argumentos (se declaran como u32 para no tocarlos); EAX = valor del caso (el original lee AX)
inline u32 __cdecl FakeNode(u32 xb,u32 yb){Push(EvNode,xb,yb);return gHeadingRet;}
// 0x61A5A0 CTask::operator new(size), cdecl: devuelve la arena de la reserva n-esima (o nulo)
inline void* __cdecl FakeNew(size_t size){Push(EvNew,u32(size));const u32 k=gNewN++;return k<2?reinterpret_cast<void*>(uintptr_t(gAllocPtr[k])):nullptr;}
// 0x681AF0 CTaskManager::SetTask(task, index, unused): thiscall con 3 argumentos de pila (`ret 0xC`); el bool se registra por su byte bajo
inline void __fastcall FakeSetTask(void* mgr,void*,void* task,u32 index,u32 unused){Push(EvSetTask,u32(uintptr_t(mgr)),u32(uintptr_t(task)),index,unused&0xFFu);}
// 0x681FE0 (sin simbolo, sin hook): thiscall(ped, &m_point, &m_targetPoint) `ret 0xC`; escribe los tres floats del caso en el tercer argumento
inline void __fastcall FakeCalc(void* self,void*,void* ped,void* p1,void* p2){Push(EvCalc,u32(uintptr_t(self)),u32(uintptr_t(ped)),u32(uintptr_t(p1)),u32(uintptr_t(p2)));std::memcpy(p2,gCalc.data(),12);}
// Ranuras no previstas de las vtables falsas: se registran (el oraculo no las espera) para que un cambio de ranura en la DLL salte a la vista
template<unsigned K> u32 __fastcall Trap(void* self,void*){Push(EvTrap,K,u32(uintptr_t(self)));return 0;}
using TrapFn=u32(__fastcall*)(void*,void*);
template<unsigned... I> constexpr std::array<TrapFn,sizeof...(I)> MakeTraps(std::integer_sequence<unsigned,I...>){return {&Trap<I>...};}
inline const std::array<TrapFn,kVtSlots> kTraps=MakeTraps(std::make_integer_sequence<unsigned,kVtSlots>{});
alignas(16) inline std::array<uintptr_t,kVtSlots> vtThis{},vtSub{},vtDef{};
inline void InitVtables() {
    for(unsigned k=0;k<kVtSlots;++k){vtThis[k]=uintptr_t(kTraps[k]);vtSub[k]=uintptr_t(kTraps[k]);vtDef[k]=uintptr_t(kTraps[k]);}
    vtThis[13]=uintptr_t(&FakeSeq);
    vtSub[4]=uintptr_t(&FakeTypeSub);
    vtDef[4]=uintptr_t(&FakeTypeDef);vtDef[11]=uintptr_t(&FakeWType);
}

struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: la funcion de la DLL no tiene RH_ScopedInstall (la entrada del EXE sigue siendo codigo original; dll=0 si no hay cuerpo en la DLL)
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
inline constexpr unsigned kStubs=5;
inline constexpr uintptr_t kRealCtors[]{0x62F310,0x48E4F0,0x668120,0x66B890};   // constructores reales (con hook en HEAD): deben ser saltos a la DLL en AMBOS lados
inline constexpr uintptr_t kExeHelpers[]{0x6822B0,0x41CCB0,0x44E480,0x66B150};  // ayudantes internos del EXE sin hook: codigo original en AMBOS lados (el C++ los reproduce)
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;       // colaboradores sustituidos, constructores y ayudantes: deben seguir siendo el codigo que eran al empezar
    std::array<Pair,kStubs> stub{};   // 0 Find, 1 GetNodeHeadingFromVector, 2 operator new, 3 SetTask, 4 0x681FE0 (cuerpo de la DLL resuelto ANTES de parchear)
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
        bool collab=true;
        for(const auto& s:env.stub)collab&=(s.hooked?(ReversingProbe::JumpsToDll(s.exe)&&ModuleOf(Resolve(s.exe))==env.asi):!ReversingProbe::JumpsToDll(s.exe))&&(s.dll==0||ModuleOf(s.dll)==env.asi);
        for(const uintptr_t a:kRealCtors)collab&=ReversingProbe::JumpsToDll(a)&&ModuleOf(Resolve(a))==env.asi;
        for(const uintptr_t a:kExeHelpers)collab&=!ReversingProbe::JumpsToDll(a);
        side.collabOK=collab;
        {
            Patches patches;
            patches.Add(env.stub[0],&FakeFind);
            patches.Add(env.stub[1],&FakeNode);
            patches.Add(env.stub[2],&FakeNew);
            patches.Add(env.stub[3],&FakeSetTask);
            patches.Add(env.stub[4],&FakeCalc);
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

// ------------------------------------------------------------------------------------------------ CTaskComplexPartner::CreateNextSubTask
inline json EventJson(const Ev& e) {
    switch(e.kind) {
    case EvFind: return json::array({"find",Name(e.a),e.b});
    case EvSeq: return json::array({"seq",Name(e.a)});
    case EvType: return json::array({"type",Name(e.a)});
    case EvWType: return json::array({"wtype",Name(e.a)});
    case EvNode: return json::array({"node",e.a,e.b});
    case EvNew: return json::array({"new",e.a});
    case EvSetTask: return json::array({"settask",Name(e.a),Name(e.b),e.c,e.d});
    case EvCalc: return json::array({"calc",Name(e.a),Name(e.b),Name(e.c),Name(e.d)});
    default: return json::array({"trap",e.a,Name(e.b)});
    }
}
inline void Put32(u8* p,u32 v){std::memcpy(p,&v,4);}
inline u32 Get32(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
// Objetos reservados: palabras (la vtable se anota aparte), con los punteros como etiquetas
inline json AllocJson(const std::array<u8,kAllocArena>& arena,const std::array<u8,kAllocArena>& ref,u32& vtable) {
    json o;
    const u8* obj=arena.data()+kAllocOff;
    vtable=Get32(obj);
    json words=json::array();words.push_back(nullptr);   // la vtable se anota aparte
    for(u32 k=1;k<kAllocBytes/4;++k)words.push_back(Name(Get32(obj+4*k)));
    bool outside=true;for(u32 i=0;i<kAllocArena;++i)if((i<kAllocOff||i>=kAllocOff+kAllocBytes)&&arena[i]!=ref[i])outside=false;
    o["vtable_written"]=vtable!=Get32(ref.data()+kAllocOff);o["words"]=words;o["outside_ok"]=outside;
    return o;
}
inline void CaseNext(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    u8* const self=thisArena.data()+kThisOff;u8* const ped=pedArena.data()+kPedOff;u8* const part=partArena.data()+kPedOff;
    u8* const pedMat=pedMatArena.data()+kMatOff;u8* const partMat=partMatArena.data()+kMatOff;u8* const pedIntel=pedIntelArena.data()+kIntelOff;u8* const partIntel=partIntelArena.data()+kIntelOff;
    u8* const sub=subArena.data()+kTaskOff;u8* const def=defArena.data()+kTaskOff;u8* const other=otherArena.data()+kOtherOff;u8* const a0=alloc0Arena.data()+kAllocOff;u8* const a1=alloc1Arena.data()+kAllocOff;u8* const seq=seqArena.data()+kSeqOff;
    cx={uintptr_t(self),uintptr_t(ped),uintptr_t(part),uintptr_t(pedMat),uintptr_t(partMat),uintptr_t(pedIntel),uintptr_t(partIntel),uintptr_t(sub),uintptr_t(def),uintptr_t(other),uintptr_t(a0),uintptr_t(a1),uintptr_t(seq)};

    FillWords(thisRef.data(),kThisArena,Rand(n,50));FillWords(pedRef.data(),kPedArena,Rand(n,51));FillWords(partRef.data(),kPedArena,Rand(n,52));
    FillWords(pedMatRef.data(),kMatArena,Rand(n,53));FillWords(partMatRef.data(),kMatArena,Rand(n,54));FillWords(pedIntelRef.data(),kIntelArena,Rand(n,55));FillWords(partIntelRef.data(),kIntelArena,Rand(n,56));
    FillWords(subRef.data(),kTaskArena,Rand(n,57));FillWords(defRef.data(),kTaskArena,Rand(n,58));FillWords(otherRef.data(),kOtherArena,Rand(n,59));FillWords(seqRef.data(),kSeqArena,Rand(n,60));
    std::memset(alloc0Ref.data(),int(c.fill0),kAllocArena);std::memset(alloc1Ref.data(),int(c.fill1),kAllocArena);

    u8* const rs=thisRef.data()+kThisOff;
    Put32(rs,u32(uintptr_t(vtThis.data())));
    Put32(rs+kSubAtThis,u32(uintptr_t(sub)));
    Put32(rs+kTaskIdAt,c.taskId);
    Put32(rs+kPartnerAt,c.partnerNull?0u:u32(uintptr_t(part)));
    rs[kStateAt]=u8(c.state);rs[kFirstAt]=u8(c.first);rs[kUpdAt]=u8(c.upd);rs[kFaceAt]=u8(c.face);rs[kStreamedAt]=u8(c.streamed);rs[kLeadAt]=u8(c.lead);
    const u16 counter=u16(c.counter);std::memcpy(rs+kCounterAt,&counter,2);
    for(u32 k=0;k<3;++k){Put32(rs+kPointAt+4*k,c.point[k]);Put32(rs+kTargetAt+4*k,c.target[k]);}
    Put32(subRef.data()+kTaskOff,u32(uintptr_t(vtSub.data())));
    Put32(defRef.data()+kTaskOff,u32(uintptr_t(vtDef.data())));
    // ped: posicion en la matriz (si la tiene) o en el placement; la otra fuente lleva un valor distinto
    u8* const rp=pedRef.data()+kPedOff;u8* const rq=partRef.data()+kPedOff;u8* const rm=pedMatRef.data()+kMatOff;u8* const rn=partMatRef.data()+kMatOff;
    Put32(rp+kMatrixAt,c.pedMat?u32(uintptr_t(pedMat)):0u);
    Put32(rp+kIntelAt,u32(uintptr_t(pedIntel)));
    Put32(rp+kHeadingAt,c.heading);
    Put32(rp+kPlacementAt,c.pedMat?c.pedAlt[0]:c.ped[0]);Put32(rp+kPlacementAt+4,c.pedMat?c.pedAlt[1]:c.ped[1]);
    Put32(rm+0x30,c.pedMat?c.ped[0]:c.pedAlt[0]);Put32(rm+0x34,c.pedMat?c.ped[1]:c.pedAlt[1]);
    Put32(rm,c.right[0]);Put32(rm+4,c.right[1]);Put32(rm+0x10,c.fwd[0]);Put32(rm+0x14,c.fwd[1]);
    Put32(rp+kShiftAt,0xA5A5A5A5u);Put32(rp+kShiftAt+4,0x5A5A5A5Au);
    // partner: siempre con matriz (la posicion sale de la matriz); la posicion alternativa en el placement
    Put32(rq+kMatrixAt,u32(uintptr_t(partMat)));
    Put32(rq+kIntelAt,u32(uintptr_t(partIntel)));
    Put32(rq+kPlacementAt,c.partAlt[0]);Put32(rq+kPlacementAt+4,c.partAlt[1]);
    Put32(rn+0x30,c.part[0]);Put32(rn+0x34,c.part[1]);
    {   // el partner es una "entidad" edificio sin IPL, ni temporal ni procedural: RegisterReference no hace nada (el registro de referencias lo valida el banco del constructor de TurnToFace)
        auto* e=reinterpret_cast<CEntity*>(rq);
        e->SetType(ENTITY_TYPE_BUILDING);e->m_bIsTempBuilding=false;e->m_bIsProcObject=false;e->SetIplIndex(0);
    }
    Put32(pedIntelRef.data()+kIntelOff+kDefAtIntel,c.defKind==0?0u:u32(uintptr_t(def)));
    Put32(otherRef.data()+kOtherOff+kOtherPartnerAt,c.otherIsPed?u32(uintptr_t(ped)):(Rand(n,61)%2?u32(uintptr_t(part)):0xA5A50000u+Rand(n,62)%0x1000u));
    otherRef[kOtherOff+kOtherStateAt]=u8(c.otherState);otherRef[kOtherOff+kOtherFirstAt]=u8(c.otherFirst);
    std::memset(otherRef.data()+kOtherOff+kOtherPointAt,0xE1,12);   // m_point de la tarea del partner: valor conocido por el oraculo

    const json input={{"family",c.family},{"state",c.state},{"task_id",c.taskId},{"partner_null",c.partnerNull},{"found",c.found},{"other_is_ped",c.otherIsPed},{"other_state",c.otherState},{"other_first",c.otherFirst},
        {"upd",c.upd},{"streamed",c.streamed},{"face",c.face},{"first",c.first},{"lead",c.lead},{"sub_type",c.subType},{"counter",c.counter},
        {"point",std::vector<u32>(c.point,c.point+3)},{"target",std::vector<u32>(c.target,c.target+3)},{"calc",std::vector<u32>(c.calc,c.calc+3)},
        {"def_kind",c.defKind},{"def_type",c.defType},{"wander_type",c.wanderType},{"ped_mat",c.pedMat},{"ped",std::vector<u32>(c.ped,c.ped+2)},{"part",std::vector<u32>(c.part,c.part+2)},
        {"heading",c.heading},{"fwd",std::vector<u32>(c.fwd,c.fwd+2)},{"right",std::vector<u32>(c.right,c.right+2)},{"heading_ret",c.headingRet},{"alloc0_null",c.alloc0Null},{"alloc1_null",c.alloc1Null},
        {"time_ms",c.timeMs},{"seq_ret",c.seqRet},{"fill0",c.fill0},{"fill1",c.fill1},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack},{"this_off",kThisOff}};
    Side sides[2];json rows[2];
    const u32 args[1]{u32(uintptr_t(ped))};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        thisArena=thisRef;pedArena=pedRef;partArena=partRef;pedMatArena=pedMatRef;partMatArena=partMatRef;pedIntelArena=pedIntelRef;partIntelArena=partIntelRef;
        subArena=subRef;defArena=defRef;otherArena=otherRef;alloc0Arena=alloc0Ref;alloc1Arena=alloc1Ref;seqArena=seqRef;
        *reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))=c.timeMs;
        nev=0;gNewN=0;
        gFound=(c.found&&!c.partnerNull)?u32(uintptr_t(other)):0u;gSubType=c.subType;gDefType=c.defType;gWanderType=c.wanderType;gHeadingRet=c.headingRet;
        gSeqRet=c.seqRet==0?0u:c.seqRet==1?u32(uintptr_t(seq)):c.seqRet;
        gAllocPtr={c.alloc0Null?0u:u32(uintptr_t(a0)),c.alloc1Null?0u:u32(uintptr_t(a1))};
        gCalc={c.calc[0],c.calc[1],c.calc[2]};
        sides[s]=Execute(env,*env.route,0x683AD0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(uintptr_t(self)),c.edx,c.stack,0);});

        // ---- observacion
        json obs;
        obs["ret"]=Name(sides[s].reg.eax);
        json ej=json::array();
        for(unsigned i=0;i<nev&&i<kMaxEvents;++i)ej.push_back(EventJson(evs[i]));
        obs["events"]=ej;obs["nevents"]=nev;
        json thisChanges=json::array();
        for(u32 i=0;i<kThisArena;++i)if(thisArena[i]!=thisRef[i])thisChanges.push_back({int(i)-int(kThisOff),thisArena[i]});
        obs["this_changes"]=thisChanges;
        json otherChanges=json::array();
        for(u32 i=0;i<kOtherArena;++i)if(otherArena[i]!=otherRef[i])otherChanges.push_back({int(i)-int(kOtherOff),otherArena[i]});
        obs["other_changes"]=otherChanges;
        {   // ped: solo pueden cambiar +0x4D8 / +0x4DC
            const u32 s0=Get32(pedArena.data()+kPedOff+kShiftAt),s1=Get32(pedArena.data()+kPedOff+kShiftAt+4);
            const bool written=s0!=0xA5A5A5A5u||s1!=0x5A5A5A5Au;
            obs["shift"]=written?json::array({s0,s1}):json(nullptr);
            auto tmp=pedArena;std::memcpy(tmp.data()+kPedOff+kShiftAt,pedRef.data()+kPedOff+kShiftAt,8);
            obs["ped_other_ok"]=tmp==pedRef;
        }
        obs["others_intact"]=partArena==partRef&&pedMatArena==pedMatRef&&partMatArena==partMatRef&&pedIntelArena==pedIntelRef&&partIntelArena==partIntelRef&&subArena==subRef&&defArena==defRef&&seqArena==seqRef;
        u32 vt0{},vt1{};
        obs["alloc0"]=AllocJson(alloc0Arena,alloc0Ref,vt0);obs["alloc1"]=AllocJson(alloc1Arena,alloc1Ref,vt1);
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        const bool timeIntact=*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))==c.timeMs;
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        out["alloc_vtables"]=json::array({vt0,vt1});   // fuera de `observed`: el oraculo no conoce las vtables, pero EXE y DLL deben dejar las mismas
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&timeIntact;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CTaskComplexPartner::CreateNextSubTask",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    json stubbed=json::array();
    for(const auto& s:env.stub){env.watches.emplace_back(s.exe,16);stubbed.push_back({{"exe",s.exe},{"dll",s.dll},{"hooked",s.hooked}});ready&=(s.dll!=0||!s.hooked)&&(s.dll==0||ModuleOf(s.dll)==env.asi);}
    for(const uintptr_t a:kRealCtors){env.watches.emplace_back(a,16);ready&=ReversingProbe::JumpsToDll(a);}
    for(const uintptr_t a:kExeHelpers){env.watches.emplace_back(a,16);ready&=!ReversingProbe::JumpsToDll(a);}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    ready&=uintptr_t(&CTimer::m_snTimeInMilliseconds)==kTimeAddr;
    InitVtables();
    const u32 savedTime=*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr));
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskComplexPartner::CreateNextSubTask",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",std::vector<uintptr_t>(std::begin(kRealCtors),std::end(kRealCtors))},{"exe_helpers",std::vector<uintptr_t>(std::begin(kExeHelpers),std::end(kExeHelpers))},{"stubbed",stubbed}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseNext(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskComplexPartner::CreateNextSubTask"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    *reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))=savedTime;
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))==savedTime;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
