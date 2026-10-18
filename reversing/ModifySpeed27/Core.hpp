#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco ModifySpeed27. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/Dependencies25A (Core.hpp).
//
// Banco par a par de CAutoPilot::ModifySpeed (0x41B980). Original = bytes del EXE con el hook RH apagado; candidato = DLL con el hook encendido.
// El unico colaborador que se llama es CCurves::CalcSpeedScaleFactor (0x43C710, cdecl, 2 punteros + 4 floats, resultado en ST0): se sustituye por un falso que
// registra los 10 dwords de sus argumentos (inicio, fin, 4 direcciones) y devuelve un float del caso. El desplazamiento de carril 0x44DB00 ya no es una llamada
// (CCarPathLink::OneWayLaneOffset es inline en la DLL): se ejercita por los bytes +0xA/+0xB de los enlaces y su efecto aparece en los argumentos registrados.
// La funcion usa x87 a 24 bits (el banco lo fija con _controlfp): el estado x87 se compara normalizado (ver NormalizeFp) y los flags de MXCSR se ignoran.
namespace ModifySpeed27Live {
using namespace ModifySpeed27Fix;
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
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (callers-residue.txt) y la DLL no puede reproducirlos (void):
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

// ------------------------------------------------------------------------------------------------ colaborador falso
struct Ctx {std::vector<u32> trace;u32 traps{};u32 scaleRet{};u32 calls{};};
inline Ctx ctx;
// 0x43C710 CCurves::CalcSpeedScaleFactor (cdecl): registra los bits de inicio (3), fin (3) y las cuatro direcciones; devuelve el float del caso en ST0.
// El EXE y la DLL (referencias = punteros) llaman igual. Mas de una llamada por ModifySpeed seria un fallo: se cuenta en `traps`.
inline float __cdecl FakeCalcSpeedScaleFactor(const float* start,const float* end,float a,float b,float c,float d) {
    ctx.trace.push_back(1);
    for(int i=0;i<3;++i){u32 v;memcpy(&v,start+i,4);ctx.trace.push_back(v);}
    for(int i=0;i<3;++i){u32 v;memcpy(&v,end+i,4);ctx.trace.push_back(v);}
    for(const float x:{a,b,c,d}){u32 v;memcpy(&v,&x,4);ctx.trace.push_back(v);}
    if(++ctx.calls>1)++ctx.traps;
    float r;const u32 bits=ctx.scaleRet;memcpy(&r,&bits,4);
    return r;
}

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,kApArena> apArena{};
alignas(16) inline std::array<u8,kNaviArena> naviA{},naviB{},naviPoison{};
alignas(16) inline std::array<u8,kPathsBytes> pathsFixture{};
inline constexpr uintptr_t kDummyPathNode=0x00001000;   // no nulo; nunca se desreferencia

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::unique_ptr<HookRoute> route;
    uintptr_t entry{0x41B980};
    uintptr_t dllCalc{};        // cuerpo de la DLL de CCurves::CalcSpeedScaleFactor (la DLL lo llama directamente)
    std::vector<Watch> watches;
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

template<class G> Side Execute(Env& env,bool candidate,G gate) {
    Side side{};
    auto call=[&]{
        const uintptr_t entry=env.entry;
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        side.reg=gate(entry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route->Invoke(candidate,call);
    return side;
}

// Un caso: CAutoPilot::ModifySpeed(target) con `this` en la arena y ThePaths (m_pPathNodes/m_pNaviNodes) apuntando a arenas propias.
inline void CaseMS(Env& env,Totals& t,unsigned n,const std::array<u8,kPathsBytes>& savedPaths,uint32 savedTimer) {
    const F f=MakeF(n);
    // ---- objeto
    std::array<u8,kApArena> apRef{};apRef.fill(0xC3);
    u8* const headRef=apRef.data()+kApOffset;
    FillWords(headRef,kApBytes,f.headSeed);
    memcpy(headRef+0xC,&f.fieldC,4);memcpy(headRef+0x10,&f.speedScale,4);memcpy(headRef+0x14,&f.cur,2);memcpy(headRef+0x16,&f.next,2);
    headRef[0x25]=u8(f.smthCurr);headRef[0x26]=u8(f.smthNext);headRef[0x27]=u8(f.lane);headRef[0x28]=u8(f.nextLane);
    // ---- areas de enlaces
    const unsigned areaA=f.cur>>10,idxA=f.cur&0x3FF,areaB=f.next>>10,idxB=f.next&0x3FF;
    std::array<u8,kNaviArena> refA{},refB{};refA.fill(0xA5);refB.fill(0xA5);
    memcpy(refA.data()+idxA*kLinkBytes,f.linkA,kLinkBytes);
    if(areaA==areaB)memcpy(refA.data()+idxB*kLinkBytes,f.linkB,kLinkBytes);else memcpy(refB.data()+idxB*kLinkBytes,f.linkB,kLinkBytes);
    std::array<u8,kNaviArena> refPoison{};refPoison.fill(0x5A);
    // ---- ThePaths: nodos de ruta (nulo = area sin cargar) y enlaces (el area sin cargar apunta a una arena veneno que no se debe tocar)
    std::array<u32,kAreas> pathNodes{},naviNodes{};
    for(auto& v:naviNodes)v=u32(uintptr_t(naviPoison.data()));
    const bool curLoaded=f.pres>=1,nextLoaded=f.pres==2||f.pres==0;   // pres 0: la actual falta; la siguiente puede estar cargada (la funcion ni la mira)
    if(curLoaded){pathNodes[areaA]=u32(kDummyPathNode);naviNodes[areaA]=u32(uintptr_t(naviA.data()));}
    if(f.pres==0&&areaB!=areaA){pathNodes[areaB]=u32(kDummyPathNode);naviNodes[areaB]=u32(uintptr_t(naviB.data()));}
    if(f.pres==2){pathNodes[areaB]=u32(kDummyPathNode);naviNodes[areaB]=areaA==areaB?u32(uintptr_t(naviA.data())):u32(uintptr_t(naviB.data()));}
    (void)nextLoaded;
    std::array<u8,kPathsBytes> pathsRef{};
    memcpy(pathsRef.data(),pathNodes.data(),kAreas*4);memcpy(pathsRef.data()+(kNaviNodesAddress-kPathNodesAddress),naviNodes.data(),kAreas*4);
    u8* const obj=apArena.data()+kApOffset;
    const u32 targetBits=f.target;
    const json input={{"kind",f.kind},{"target",targetBits},{"now",f.now},{"field_c",f.fieldC},{"speed_scale",f.speedScale},{"cur",f.cur},{"next",f.next},{"pres",f.pres},
                      {"smth_curr",int(f.smthCurr)},{"smth_next",int(f.smthNext)},{"lane",int(f.lane)},{"next_lane",int(f.nextLane)},
                      {"link_a",HexBytes(f.linkA,kLinkBytes)},{"link_b",HexBytes(f.linkB,kLinkBytes)},{"scale_ret",f.scaleRet},{"head_in",HexBytes(headRef,kApBytes)},
                      {"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
    u32* const timer=reinterpret_cast<u32*>(uintptr_t(kTimerAddress));
    u8* const paths=reinterpret_cast<u8*>(uintptr_t(kPathNodesAddress));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        ctx.trace.clear();ctx.traps=0;ctx.calls=0;ctx.scaleRet=f.scaleRet;
        apArena=apRef;naviA=refA;naviB=refB;naviPoison=refPoison;memcpy(paths,pathsRef.data(),kPathsBytes);*timer=f.now;
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            ReversingProbe::EntryPatch exe(0x43C710,&FakeCalcSpeedScaleFactor),dll(env.dllCalc,&FakeCalcSpeedScaleFactor);
            const u32 args[1]{targetBits};
            return Gate(entry,args,1,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);
        });
        const bool apGuards=memcmp(apArena.data(),apRef.data(),kApOffset)==0&&memcmp(apArena.data()+kApOffset+kApBytes,apRef.data()+kApOffset+kApBytes,kApArena-kApOffset-kApBytes)==0;
        const bool arenas=naviA==refA&&naviB==refB&&naviPoison==refPoison;
        const bool tables=memcmp(paths,pathsRef.data(),kPathsBytes)==0&&*timer==f.now;
        json observed={{"head",HexBytes(obj,kApBytes)},{"guard_ok",apGuards},{"arenas_unchanged",arenas},{"tables_unchanged",tables},{"trace",ctx.trace}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        memcpy(paths,savedPaths.data(),kPathsBytes);*timer=savedTimer;
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK},{"traps_zero",ctx.traps==0},
                  {"calls",ctx.calls}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&memcmp(paths,savedPaths.data(),kPathsBytes)==0&&*timer==savedTimer;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","traps_zero","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function","CAutoPilot::ModifySpeed"},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    bool collaboratorsOK=env.dllCalc&&ModuleOf(env.dllCalc)==env.asi;
    ready&=collaboratorsOK&&body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x41B980),uintptr_t(0x43C710),env.dllCalc})env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    std::array<u8,kPathsBytes> savedPaths{};memcpy(savedPaths.data(),reinterpret_cast<void*>(uintptr_t(kPathNodesAddress)),kPathsBytes);
    const uint32 savedTimer=*reinterpret_cast<uint32*>(uintptr_t(kTimerAddress));
    ctx.trace.reserve(64);
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",env.cases},{"cases",{{"CAutoPilot::ModifySpeed",env.cases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",std::vector<uintptr_t>{0x43C710}},{"dll",std::vector<uintptr_t>{env.dllCalc}}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_paths_hash",Fnv64(savedPaths.data(),savedPaths.size())},{"initial_timer",savedTimer}});
    Totals totals;
    if(ready) {
        naviPoison.fill(0x5A);
        for(unsigned c=0;c<env.cases;++c)CaseMS(env,totals,c,savedPaths,savedTimer);
        Write({{"event","function_summary"},{"function","CAutoPilot::ModifySpeed"},{"cases",env.cases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&memcmp(reinterpret_cast<void*>(uintptr_t(kPathNodesAddress)),savedPaths.data(),kPathsBytes)==0&&*reinterpret_cast<uint32*>(uintptr_t(kTimerAddress))==savedTimer;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"paths_hash",Fnv64(reinterpret_cast<const u8*>(uintptr_t(kPathNodesAddress)),kPathsBytes)},{"timer",*reinterpret_cast<uint32*>(uintptr_t(kTimerAddress))}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
