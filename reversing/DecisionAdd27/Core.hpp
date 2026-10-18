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
#include "Fixtures.hpp"

// Nucleo del banco DecisionAdd27. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/DecisionMakeDecision27 (Core.hpp).
//
// Banco par a par de DOS funciones, en rutas separadas (cada una con su HookRoute y sus compuertas):
//   CDecision::Add (0x600600, thiscall, 3 argumentos, `ret 0Ch`)                          -> CaseAdd
//   CDecisionMakerTypes::AddEventResponse (0x6044C0, thiscall, 5 argumentos, `ret 14h`)   -> CaseAer
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO el hook de esa ruta encendido. En la ruta de AddEventResponse el hook de Add (0x600600)
// queda APAGADO en ambos lados: el original llama al Add del EXE y el candidato llama al Add en C++ de la DLL (directamente, sin pasar por el EXE). Ambas rutas exigen
// ademas que _ftol2 (0x821B40) siga siendo el codigo del EXE (el EXE lo llama; la DLL usa su conversion float->int64).
// Sin RNG, sin colaboradores sustituidos. La palabra de control x87 se fija a 24 bits, redondeo al mas cercano y TODAS las excepciones enmascaradas (como el juego), porque las
// chances incluyen NaN/inf/enormes (el EXE usa fistp: con la mascara de operacion invalida quitada lanzaria una excepcion en vez de devolver el indefinido).
namespace DecisionAdd27Live {
using namespace DecisionAdd27Fix;
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
// Las dos funciones usan x87 (el EXE: fld/fistp dentro de _ftol2; la DLL: la conversion float->int64 del compilador): los registros ST vacios guardan restos distintos, la palabra de
// estado lleva banderas de excepcion (NaN/inf activan IE) y codigos de condicion distintos y los punteros de instruccion/operando difieren. Se compara lo observable:
//   FCW completa (modo de precision/redondeo/mascaras), TOP de la pila (bits 11-13 de FSW), la palabra de etiquetas y los ST VALIDOS (ninguno debe quedar: pila vacia).
// Se ignoran: banderas de excepcion/condicion de FSW, FIP/FCS/FOP/FDP/FDS y el contenido de los registros marcados como vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md): por defecto solo se registran en original_residue/candidate_residue.
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
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile;};
// Igual que DecisionMakeDecision27::Gate: siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`.
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},outFlags{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov ebpBefore,ebp
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
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter};
}

// ------------------------------------------------------------------------------------------------ memoria de los casos
alignas(16) inline std::array<u8,kObjArena> objArena{};       // Add: CDecision destino con guardas
alignas(16) inline std::array<u8,kArgsArena> argsArena{};     // chances (16 bytes) y flags (8 bytes) + guardas: se compara ENTERA tras la llamada
alignas(16) inline std::array<u8,kMem> memArena{};            // AddEventResponse: CDecisionMakerTypes sintetico
alignas(16) inline std::array<u8,kMem> memRef{};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> routeAdd,routeAer;
    std::vector<Watch> watches;          // 0x600600, 0x6044C0, 0x821B40: deben seguir siendo el codigo del EXE al terminar cada llamada
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook de la ruta apagado y NINGUN otro colaborador enganchado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI).
// `exeOnly`: direcciones del EXE que no pueden saltar a la DLL en ninguno de los dos lados.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,std::initializer_list<uintptr_t> exeOnly,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        bool ok=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        for(const uintptr_t a:exeOnly)ok&=!ReversingProbe::JumpsToDll(a);
        side.routeOK=ok;
        side.reg=gate(entry);
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK}};
}
inline json Finish(Env& env,Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}

// ------------------------------------------------------------------------------------------------ CDecision::Add
inline json AddInput(const F& f) {
    return {{"kind",f.kind},{"pre",HexBytes(f.obj,kObjBytes)},{"task",f.task},{"chances",std::vector<u32>{f.chances[0],f.chances[1],f.chances[2],f.chances[3]}},
            {"flags",std::vector<i32>{f.flags[0],f.flags[1]}},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseAdd(Env& env,Totals& t,unsigned n) {
    const F f=MakeF(n);
    std::array<u8,kObjArena> objRef{};objRef.fill(0xC3);
    memcpy(objRef.data()+kObjOffset,f.obj,kObjBytes);
    std::array<u8,kArgsArena> argsRef{};argsRef.fill(0xA7);
    memcpy(argsRef.data()+kChancesAt,f.chances,16);memcpy(argsRef.data()+kFlagsAt,f.flags,8);
    u8* const obj=objArena.data()+kObjOffset;
    const json input=AddInput(f);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        objArena=objRef;argsArena=argsRef;
        sides[s]=Execute(env,*env.routeAdd,0x600600,{uintptr_t(0x821B40)},candidate,[&](uintptr_t entry){
            const u32 args[3]{u32(f.task),u32(uintptr_t(argsArena.data()+kChancesAt)),u32(uintptr_t(argsArena.data()+kFlagsAt))};
            return Gate(entry,args,3,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);
        });
        const bool objGuards=memcmp(objArena.data(),objRef.data(),kObjOffset)==0&&memcmp(objArena.data()+kObjOffset+kObjBytes,objRef.data()+kObjOffset+kObjBytes,kObjArena-kObjOffset-kObjBytes)==0;
        const bool argsOk=argsArena==argsRef;   // chances y flags (y sus guardas) no se modifican
        json observed={{"obj",HexBytes(obj,kObjBytes)},{"guard_ok",objGuards},{"args_ok",argsOk}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CDecision::Add",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ CDecisionMakerTypes::AddEventResponse
inline json AerInput(const G& g) {
    json j=AddInput(g.f);
    j["mode"]=g.mode;j["dm"]=g.dm;j["ev_type"]=g.evType;j["ev_index"]=g.evIndex;j["this_off"]=g.thisOff;j["table_off"]=g.tableOff;j["window_off"]=g.windowOff;
    return j;
}
// Primer desfase (fuera de [w, w+len)) en el que a y b difieren; -1 si no hay.
inline long FirstDiffOutside(const u8* a,const u8* b,size_t n,size_t w,size_t len) {
    for(size_t i=0;i<n;++i) {if(i==w){i+=len-1;continue;}if(a[i]!=b[i])return long(i);}
    return -1;
}
inline void CaseAer(Env& env,Totals& t,unsigned n) {
    const G g=MakeG(n);
    const F& f=g.f;
    const bool generatorOk=WindowOK(g.windowOff)&&TableOK(g.tableOff,g.windowOff)&&g.windowOff==WindowOf(u32(g.dm),g.evIndex)&&g.tableOff==TableOf(u32(g.evType));
    std::array<u8,kArgsArena> argsRef{};argsRef.fill(0xA7);
    memcpy(argsRef.data()+kChancesAt,f.chances,16);memcpy(argsRef.data()+kFlagsAt,f.flags,8);
    // Arena de referencia: relleno pseudoaleatorio (tambien en las CDecision vecinas y en el resto de la tabla), la CDecision destino y el indice leido en P.
    FillWords(memRef.data(),kMem,g.arenaSeed);
    if(generatorOk) {
        memcpy(memRef.data()+g.windowOff,f.obj,kObjBytes);
        memcpy(memRef.data()+g.tableOff,&g.evIndex,4);
    }
    u8* const self=memArena.data()+kThis;
    const size_t wo=generatorOk?g.windowOff:0;   // si el fixture fuera inseguro no se toca nada fuera de la arena
    const json input=AerInput(g);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        memArena=memRef;argsArena=argsRef;
        if(generatorOk) {
            sides[s]=Execute(env,*env.routeAer,0x6044C0,{uintptr_t(0x600600),uintptr_t(0x821B40)},candidate,[&](uintptr_t entry){
                const u32 args[5]{u32(g.dm),u32(g.evType),u32(f.task),u32(uintptr_t(argsArena.data()+kChancesAt)),u32(uintptr_t(argsArena.data()+kFlagsAt))};
                return Gate(entry,args,5,f.eax,u32(uintptr_t(self)),f.edx,f.stack,0);
            });
        } else {   // fixture inseguro: no se llama (queda registrado y falla)
            sides[s].reg={0,0,0,0,true,true};SaveFP(sides[s].fp.data());sides[s].mxcsr=ReadMXCSR();
        }
        const long outside=FirstDiffOutside(memArena.data(),memRef.data(),kMem,wo,kObjBytes);
        const bool argsOk=argsArena==argsRef;
        json observed={{"window",HexBytes(memArena.data()+wo,kObjBytes)},{"outside_ok",outside<0},{"first_outside",outside},{"args_ok",argsOk},{"generator_ok",generatorOk}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        if(!generatorOk)out["route_ok"]=false;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CDecisionMakerTypes::AddEventResponse",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t bodyAdd=env.routeAdd?env.routeAdd->Body():0,bodyAer=env.routeAer?env.routeAer->Body():0;
    ready&=bodyAdd&&ModuleOf(bodyAdd)==env.asi&&bodyAer&&ModuleOf(bodyAer)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x600600),uintptr_t(0x6044C0),uintptr_t(0x821B40)})env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",2},{"pairs",kCasesAdd+kCasesAer},
        {"cases",{{"CDecision::Add",kCasesAdd},{"CDecisionMakerTypes::AddEventResponse",kCasesAer}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",false},
        {"collaborators",{{"exe",std::vector<uintptr_t>{0x821B40}},{"dll",std::vector<uintptr_t>{}}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCasesAdd;++c)CaseAdd(env,totals,c);
        Write({{"event","function_summary"},{"function","CDecision::Add"},{"cases",kCasesAdd},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
        const unsigned failuresAdd=totals.failures;
        for(unsigned c=0;c<kCasesAer;++c)CaseAer(env,totals,c);
        Write({{"event","function_summary"},{"function","CDecisionMakerTypes::AddEventResponse"},{"cases",kCasesAer},{"failures",totals.failures-failuresAdd},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    if(env.routeAdd)stateRestored&=env.routeAdd->Restore();
    if(env.routeAer)stateRestored&=env.routeAer->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
