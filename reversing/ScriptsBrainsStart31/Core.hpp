#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco ScriptsBrainsStart31. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/DecisionMakeDecision27 (Core.hpp).
//
// Banco par a par de CScriptsForBrains::StartNewStreamedScriptBrain (0x46B270). Original = bytes del EXE con el hook RH apagado; candidato = DLL con el hook encendido.
// Los colaboradores se sustituyen por el MISMO falso en ambos lados (en la direccion del EXE y en el cuerpo C++ de la DLL con su hook encendido); el falso registra los
// argumentos y las trazas se comparan. El estado x87 se compara normalizado (ver NormalizeFp); EAX/ECX/EDX solo se registran (ver README.md).
namespace ScriptsBrainsStart31Live {
using namespace ScriptsBrainsStart31Fix;
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

// ------------------------------------------------------------------------------------------------ colaboradores sustituidos
// El MISMO falso se parchea en la direccion del EXE y en el cuerpo C++ de la DLL (hook encendido) para cada colaborador:
//   CStreamedScripts::StartNewStreamedScript (0x470890, thiscall, 1 arg en pila, `ret 4`): registra (this, indice con signo) y devuelve la arena de script del caso.
//   CTheScripts::RemoveFromWaitingForScriptBrainArray (0x46ABC0, cdecl, 2 args): registra (entidad, modelo de 16 bits) y no toca la lista de espera.
// Los dos escriben en la misma traza, en orden de llamada.
struct Ev {u32 tag,a,b;};   // tag 1 = StartNewStreamedScript (a = this, b = indice); tag 2 = RemoveFromWaiting (a = entidad, b = modelo)
inline std::array<Ev,8> evs{};
inline unsigned evCount{};
inline void* scriptRet{};
inline void* __fastcall FakeStartNew(void* self,void* /*edx*/,u32 index) {
    if(evCount<evs.size())evs[evCount]={1,u32(uintptr_t(self)),index};
    ++evCount;
    return scriptRet;
}
inline void __cdecl FakeRemove(u32 entity,u32 model) {
    if(evCount<evs.size())evs[evCount]={2,entity,model};
    ++evCount;
}
inline bool JumpsTo(uintptr_t at,const void* target) {return *reinterpret_cast<const uint8*>(at)==0xE9&&at+5+*reinterpret_cast<const int32*>(at+1)==uintptr_t(target);}

// ------------------------------------------------------------------------------------------------ pools falsos
// Misma disposicion que CPool (0x14 bytes): almacenamiento, estados de slot, capacidad, ultimo libre, bools. El EXE solo lee [pool] y [pool + 4].
struct FakePool {u32 storage,slot,capacity;i32 lastFree;u8 owns,deal,pad0,pad1;};
static_assert(sizeof(FakePool)==0x14);
inline constexpr uintptr_t kPedPoolGlobal=0xB74490,kObjPoolGlobal=0xB7449C,kStreamedScripts=0xA47B60;
inline uint32& PedPoolGlobal() {return *reinterpret_cast<uint32*>(kPedPoolGlobal);}
inline uint32& ObjPoolGlobal() {return *reinterpret_cast<uint32*>(kObjPoolGlobal);}

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,kThisArena> thisArena{};
alignas(16) inline std::array<u8,kScriptArena> scriptArena{};
alignas(16) inline std::array<u8,kPedArena> pedArena{};
alignas(16) inline std::array<u8,kObjArena> objArena{};
alignas(16) inline FakePool pedPool{},objPool{};
alignas(16) inline std::array<u8,8> pedSlots{},objSlots{};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::unique_ptr<HookRoute> route,collabNew,collabRem;           // funcion bajo prueba y colaboradores sustituidos (todos con el hook apagado)
    uintptr_t entry{0x46B270},newExe{0x470890},remExe{0x46ABC0},newBody{},remBody{};
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;   // [0] StartNew EXE, [1] StartNew DLL, [2] Remove EXE, [3] Remove DLL
    std::vector<Watch> watches;
    uint32 realPedPool{},realObjPool{};
    bool layoutOk{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
    bool FakesInstalled()const{
        return patches.size()==4&&JumpsTo(newExe,reinterpret_cast<const void*>(&FakeStartNew))&&JumpsTo(newBody,reinterpret_cast<const void*>(&FakeStartNew))
            &&JumpsTo(remExe,reinterpret_cast<const void*>(&FakeRemove))&&JumpsTo(remBody,reinterpret_cast<const void*>(&FakeRemove));
    }
    bool PoolsRestored()const{return PedPoolGlobal()==realPedPool&&ObjPoolGlobal()==realObjPool;}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

template<class G> Side Execute(Env& env,bool candidate,G gate) {
    Side side{};
    auto call=[&]{
        const uintptr_t entry=env.entry;
        side.routeOK=(candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry))&&env.FakesInstalled();
        side.reg=gate(entry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route->Invoke(candidate,call);
    return side;
}

template<size_t N> inline json DiffJson(const std::array<u8,N>& before,const std::array<u8,N>& after) {
    json d=json::array();
    for(size_t i=0;i<N;++i)if(before[i]!=after[i])d.push_back({i,after[i]});
    return d;
}

// Un caso: CScriptsForBrains::StartNewStreamedScriptBrain(index, entity, bHasAScriptBrain) con `this` en la arena y los pools globales apuntando a pools falsos.
inline void CaseSB(Env& env,Totals& t,unsigned n) {
    const F f=MakeF(n);
    std::array<u8,kThisArena> thisRef{};thisRef.fill(0xC3);memcpy(thisRef.data()+kThisOffset,f.brains,kThisBytes);
    std::array<u8,kScriptArena> scriptRef{};scriptRef.fill(0xC3);memcpy(scriptRef.data()+kScriptOffset,f.script,kScriptBytes);
    std::array<u8,kPedArena> pedRef{};pedRef.fill(0xC3);memcpy(pedRef.data()+kPedOffset,f.ped,kPedBytes);
    std::array<u8,kObjArena> objRef{};objRef.fill(0xC3);memcpy(objRef.data()+kObjOffset,f.obj,kObjBytes);
    const bool fixtureOk=Fnv64(thisRef.data(),kThisArena)==f.thisFnv;
    u8* const thisPtr=thisArena.data()+kThisOffset;
    u8* const pedBase=pedArena.data()+kPedOffset;
    u8* const objBase=objArena.data()+kObjOffset;
    const u32 entPtr=f.entPool==1?u32(uintptr_t(pedBase+f.entSlot*kPedSize)):f.entPool==2?u32(uintptr_t(objBase+f.entSlot*kObjSize)):f.entPtrBad;
    json input=json::parse(InputJson(f));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        thisArena=thisRef;scriptArena=scriptRef;pedArena=pedRef;objArena=objRef;
        pedSlots.fill(0xC3);objSlots.fill(0xC3);
        memcpy(pedSlots.data(),f.pedState,kSlots);memcpy(objSlots.data(),f.objState,kSlots);
        pedPool={u32(uintptr_t(pedBase)),u32(uintptr_t(pedSlots.data())),kSlots,-1,0,0,0,0};
        objPool={u32(uintptr_t(objBase)),u32(uintptr_t(objSlots.data())),kSlots,-1,0,0,0,0};
        const FakePool pedPoolRef=pedPool,objPoolRef=objPool;
        const auto pedSlotsRef=pedSlots,objSlotsRef=objSlots;
        evCount=0;evs.fill({});scriptRet=scriptArena.data()+kScriptOffset;
        PedPoolGlobal()=u32(uintptr_t(&pedPool));ObjPoolGlobal()=u32(uintptr_t(&objPool));
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            const u32 args[3]{f.indexArg,entPtr,f.hasArg};
            return Gate(entry,args,3,f.eax,u32(uintptr_t(thisPtr)),f.edx,f.stack,0);
        });
        PedPoolGlobal()=env.realPedPool;ObjPoolGlobal()=env.realObjPool;   // los pools reales, de vuelta antes de tocar nada mas
        json calls=json::array();
        for(unsigned i=0;i<evCount&&i<evs.size();++i) {
            if(evs[i].tag==1)calls.push_back({"new",int(evs[i].a==kStreamedScripts),int32(evs[i].b)});
            else calls.push_back({"rem",int(evs[i].a==entPtr),evs[i].b&0xFFFFu});
        }
        const bool poolsOk=memcmp(&pedPool,&pedPoolRef,sizeof pedPool)==0&&memcmp(&objPool,&objPoolRef,sizeof objPool)==0&&pedSlots==pedSlotsRef&&objSlots==objSlotsRef;
        json observed={{"this_fnv",Hex64(Fnv64(thisArena.data(),kThisArena))},{"calls",calls},{"call_count",evCount},{"pools_ok",poolsOk},
                       {"script_diff",DiffJson(scriptRef,scriptArena)},{"ped_diff",DiffJson(pedRef,pedArena)},{"obj_diff",DiffJson(objRef,objArena)}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&env.PoolsRestored();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1]&&fixtureOk;
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function","CScriptsForBrains::StartNewStreamedScriptBrain"},{"case",n},{"pair",t.pairs++},{"input",input},{"fixture_ok",fixtureOk},
           {"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    env.newBody=env.collabNew?env.collabNew->Body():0;env.remBody=env.collabRem?env.collabRem->Body():0;
    ready&=env.newBody&&ModuleOf(env.newBody)==env.asi&&env.remBody&&ModuleOf(env.remBody)==env.asi&&env.layoutOk;
    json watched=json::array();
    env.realPedPool=PedPoolGlobal();env.realObjPool=ObjPoolGlobal();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    if(ready) {   // falsos de los colaboradores: en el EXE (hooks apagados) y en el cuerpo de la DLL; se quitan ANTES de restaurar los hooks
        env.patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(env.newExe,reinterpret_cast<void*>(&FakeStartNew)));
        env.patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(env.newBody,reinterpret_cast<void*>(&FakeStartNew)));
        env.patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(env.remExe,reinterpret_cast<void*>(&FakeRemove)));
        env.patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(env.remBody,reinterpret_cast<void*>(&FakeRemove)));
    }
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",env.cases},{"cases",{{"CScriptsForBrains::StartNewStreamedScriptBrain",env.cases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"stubbed_exe",std::vector<uintptr_t>{env.newExe,env.remExe}},{"stubbed_dll_body",std::vector<uintptr_t>{env.newBody,env.remBody}}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"layout_ok",env.layoutOk},
        {"fake_pools",true},{"real_ped_pool",env.realPedPool},{"real_obj_pool",env.realObjPool},
        {"this_arena",kThisArena},{"script_arena",kScriptArena},{"ped_arena",kPedArena},{"obj_arena",kObjArena},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<env.cases;++c)CaseSB(env,totals,c);
        Write({{"event","function_summary"},{"function","CScriptsForBrains::StartNewStreamedScriptBrain"},{"cases",env.cases},{"failures",totals.failures},{"state_restored",env.PoolsRestored()}});
    }
    const bool entriesIntact=env.WatchesIntact()&&env.PoolsRestored();
    while(!env.patches.empty())env.patches.pop_back();   // primero los falsos (en orden inverso), luego los hooks
    bool stateRestored=entriesIntact;
    if(env.route)stateRestored&=env.route->Restore();
    if(env.collabNew)stateRestored&=env.collabNew->Restore();
    if(env.collabRem)stateRestored&=env.collabRem->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"ped_pool",PedPoolGlobal()},{"obj_pool",ObjPoolGlobal()},{"real_ped_pool",env.realPedPool},{"real_obj_pool",env.realObjPool}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
