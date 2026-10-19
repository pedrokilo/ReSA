#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco ScriptsBrainsCheckNew31. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/DecisionMakeDecision27 (Core.hpp).
//
// Banco par a par de CScriptsForBrains::CheckIfNewEntityNeedsScript (0x46FF20). Original = bytes del EXE con el hook RH apagado; candidato = DLL con el hook encendido.
// Los colaboradores se sustituyen por el MISMO falso en ambos lados (en la direccion del EXE y en el cuerpo C++ de la DLL con su hook encendido); el falso registra los
// argumentos y las trazas se comparan. El estado x87 se compara normalizado (ver NormalizeFp); EAX/ECX/EDX solo se registran (ver README.md).
namespace ScriptsBrainsCheckNew31Live {
using namespace ScriptsBrainsCheckNew31Fix;
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
// A diferencia de Dependencies25A, aqui el EXE SI ejecuta x87 (y la DLL puede usar SSE): los registros ST vacios guardan restos distintos, la palabra de estado
// lleva banderas de excepcion y codigos de condicion (C0-C3) distintos y los punteros de instruccion/operando difieren. Se compara lo observable:
//   FCW completa (modo de precision/redondeo/mascaras), TOP de la pila (bits 11-13 de FSW), la palabra de etiquetas y los ST VALIDOS (ninguno debe quedar).
// Se ignoran: banderas de excepcion/condicion de FSW, FIP/FCS/FOP/FDP/FDS y el contenido de los registros marcados como vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md) y la DLL no puede reproducirlos:
// por defecto solo se registran en original_residue/candidate_residue.
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
// Igual que Dependencies25A::Gate, mas EFLAGS justo tras el `call` (para comprobar DF=0). argsPtr[0] queda en la cima de la pila.
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
// ------------------------------------------------------------------------------------------------ RNG del EXE
inline void ExeSrand(u32 seed) {reinterpret_cast<void(__cdecl*)(u32)>(0x821B11)(seed);}
inline u32* ExeRandState() {return reinterpret_cast<u32*>(reinterpret_cast<uint8*(__cdecl*)()>(0x827B3D)()+0x14);}

// ------------------------------------------------------------------------------------------------ colaborador sustituido
// CScriptsForBrains::StartOrRequestNewStreamedScriptBrain (thiscall, 4 argumentos en pila, `ret 10h`). El MISMO falso se parchea en la direccion del EXE (0x46CD80)
// y en el cuerpo C++ de la DLL (hook encendido): ambos lados registran la llamada (this, indice, entidad, attachType, bAdd) y no hacen nada mas.
// __fastcall reproduce thiscall con cuatro argumentos en pila y limpieza por el llamado (ECX = this, EDX = no usado).
struct Call {u32 self,a,b,c,d;};
inline std::array<Call,16> trace{};
inline unsigned traceCount{};
inline void __fastcall FakeStartOrRequest(void* self,void* /*edx*/,u32 index,u32 entity,u32 attach,u32 add) {
    if(traceCount<trace.size())trace[traceCount]={u32(uintptr_t(self)),index,entity,attach,add};
    ++traceCount;
}
inline bool JumpsTo(uintptr_t at,const void* target) {return *reinterpret_cast<const uint8*>(at)==0xE9&&at+5+*reinterpret_cast<const int32*>(at+1)==uintptr_t(target);}

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,kThisArena> thisArena{};
alignas(16) inline std::array<u8,kEntArena> entArena{};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};u32 rngState{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::unique_ptr<HookRoute> route,collab;                       // funcion bajo prueba y colaborador sustituido (ambos con el hook apagado)
    uintptr_t entry{0x46FF20},collabExe{0x46CD80},collabBody{};
    std::unique_ptr<ReversingProbe::EntryPatch> patchExe,patchDll; // el falso, en el EXE y en el cuerpo de la DLL
    std::vector<Watch> watches;                                    // 0x821B1E, 0x821B11, 0x827B3D, 0x821B40: deben seguir siendo el codigo del EXE
    bool layoutOk{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
    bool FakesInstalled()const{return patchExe&&patchDll&&JumpsTo(collabExe,reinterpret_cast<const void*>(&FakeStartOrRequest))&&JumpsTo(collabBody,reinterpret_cast<const void*>(&FakeStartOrRequest));}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

template<class G> Side Execute(Env& env,bool candidate,G gate) {
    Side side{};
    auto call=[&]{
        const uintptr_t entry=env.entry;
        // original: la entrada es el codigo del EXE; candidato: salta al cuerpo C++ del ASI. En ambos, los dos falsos del colaborador siguen puestos.
        side.routeOK=(candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry))&&env.FakesInstalled();
        side.reg=gate(entry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route->Invoke(candidate,call);
    return side;
}

inline int RandCalls(u32 seed,u32 state) {u32 s=seed;for(int k=0;k<=80;++k){if(s==state)return k;s=NextState(s);}return -1;}

// Un caso: CScriptsForBrains::CheckIfNewEntityNeedsScript(entity, attachType, unused) con `this` en la arena y la semilla de rand fijada con srand del EXE.
inline void CaseCN(Env& env,Totals& t,unsigned n,uint32 savedRand) {
    const F f=MakeF(n);
    std::array<u8,kThisArena> thisRef{};thisRef.fill(0xC3);memcpy(thisRef.data()+kThisOffset,f.brains,kThisBytes);
    std::array<u8,kEntArena> entRef{};entRef.fill(0xC3);memcpy(entRef.data()+kEntOffset,f.ent,kEntBytes);
    const bool fixtureOk=Fnv64(thisRef.data(),kThisArena)==f.thisFnv&&Fnv64(entRef.data(),kEntArena)==f.entFnv;
    u8* const thisPtr=thisArena.data()+kThisOffset;
    u8* const entPtr=entArena.data()+kEntOffset;
    json input=json::parse(InputJson(f));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        thisArena=thisRef;entArena=entRef;
        traceCount=0;trace.fill({});
        ExeSrand(f.seed);
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            const u32 args[3]{u32(uintptr_t(entPtr)),f.attachArg,f.unusedArg};
            return Gate(entry,args,3,f.eax,u32(uintptr_t(thisPtr)),f.edx,f.stack,0);
        });
        const u32 rngState=*ExeRandState();sides[s].rngState=rngState;
        json calls=json::array();
        for(unsigned i=0;i<traceCount&&i<trace.size();++i)
            calls.push_back({int(trace[i].self==u32(uintptr_t(thisPtr))),trace[i].a&0xFFu,int(trace[i].b==u32(uintptr_t(entPtr))),trace[i].c&0xFFu,trace[i].d&0xFFu});
        json observed={{"this_fnv",Hex64(Fnv64(thisArena.data(),kThisArena))},{"ent_fnv",Hex64(Fnv64(entArena.data(),kEntArena))},{"calls",calls},{"call_count",traceCount},
                       {"rng_state",rngState},{"rand_calls",RandCalls(f.seed,rngState)}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        *ExeRandState()=savedRand;
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&*ExeRandState()==savedRand;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1]&&fixtureOk;
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function","CScriptsForBrains::CheckIfNewEntityNeedsScript"},{"case",n},{"pair",t.pairs++},{"input",input},{"fixture_ok",fixtureOk},{"original",rows[0]},{"candidate",rows[1]},
           {"fp_difference",FpDifference(fa,fb)},{"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    env.collabBody=env.collab?env.collab->Body():0;
    ready&=env.collabBody&&ModuleOf(env.collabBody)==env.asi&&env.layoutOk;
    for(const uintptr_t at:{uintptr_t(0x821B1E),uintptr_t(0x821B11),uintptr_t(0x827B3D),uintptr_t(0x821B40)})env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    const uint32 savedRand=*ExeRandState();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    if(ready) {   // falso del colaborador: en el EXE (hook de CheckIfNew y del colaborador apagados) y en el cuerpo de la DLL; se quitan ANTES de restaurar los hooks
        env.patchExe=std::make_unique<ReversingProbe::EntryPatch>(env.collabExe,reinterpret_cast<void*>(&FakeStartOrRequest));
        env.patchDll=std::make_unique<ReversingProbe::EntryPatch>(env.collabBody,reinterpret_cast<void*>(&FakeStartOrRequest));
    }
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",env.cases},{"cases",{{"CScriptsForBrains::CheckIfNewEntityNeedsScript",env.cases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe_real",std::vector<uintptr_t>{0x821B1E,0x821B40}},{"stubbed_exe",std::vector<uintptr_t>{env.collabExe}},{"stubbed_dll_body",std::vector<uintptr_t>{env.collabBody}}}},
        {"watched_entries",watched},{"bodies",bodies},{"collaborator_body",env.collabBody},{"layout_ok",env.layoutOk},{"asi_module",env.asi},{"original_module",env.game},
        {"this_arena",kThisArena},{"ent_arena",kEntArena},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_rand_state",savedRand}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<env.cases;++c)CaseCN(env,totals,c,savedRand);
        Write({{"event","function_summary"},{"function","CScriptsForBrains::CheckIfNewEntityNeedsScript"},{"cases",env.cases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    env.patchDll.reset();env.patchExe.reset();   // primero los falsos, luego los hooks (el EntryPatch restaura los bytes que guardo)
    bool stateRestored=entriesIntact&&*ExeRandState()==savedRand;
    if(env.route)stateRestored&=env.route->Restore();
    if(env.collab)stateRestored&=env.collab->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"rand_state",*ExeRandState()}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
