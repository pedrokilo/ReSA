#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco FxEmitterCreateParticle30. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos FxEmitter_c/FxManager_c/FxPrimBP_c/FxEmitterBP_c (LiveComparison.hpp). Plantilla: reversing/QuatGet28 (Core.hpp).
//
// Banco par a par de FxEmitter_c::CreateParticle (0x4A2580, thiscall, 7 argumentos de pila, `ret 1Ch`). Original = bytes del EXE con el hook RH apagado; candidato = DLL con el hook
// encendido (cuerpo C++ en FxEmitter.cpp + ayudantes FxPrt* de X87Math.cpp, /O2 /Oi /arch:IA32 /fp:fast). El objeto, el sistema, el prim, la lista de particulas y las dos particulas
// posibles viven en un arena propio del banco (Fixtures.hpp); los colaboradores (GetParticle, FreeUpParticle, GetRWMatrix, RwV3dNormalize, RwV3dTransformVectors,
// RotateVecIntoVec y UpdateParticle) se sustituyen en AMBOS lados por los mismos stubs trazados (EntryPatch en la direccion del EXE y en la entrada de la DLL).
// rand() es el del CRT del EXE (0x821B1E): la semilla de la hebra (0x827B3D + 0x14) se fija antes de cada lado y se restaura al final; el valor que devolveria la SIGUIENTE
// tirada forma parte de la comparacion (cuenta cuantas tiradas hizo cada lado). TList_c::AddItem (0x4A8DF0) NO se sustituye: el EXE ejecuta su codigo y la DLL su version en linea.
// La funcion usa x87 a 24 bits (el banco lo fija con _controlfp): el estado x87 se compara normalizado (ver NormalizeFp) y los flags de MXCSR se ignoran.
namespace FxCp30Live {
using namespace CpFix;
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
// La funcion ejecuta x87 (y la DLL con ayudantes de X87Math.cpp): los registros ST vacios guardan restos distintos, FSW lleva banderas y codigos de condicion distintos y los
// punteros FIP/FDP difieren. Se compara lo observable: FCW completa, TOP, etiquetas y los ST VALIDOS (ninguno debe quedar). Se ignoran las banderas de FSW, FIP/FCS/FOP/FDP/FDS y los
// registros vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE lee ECX/EDX (el unico que lee EAX es la particula, que ya se compara como `ret`): solo se registran.
inline constexpr bool kCompareVolatileRegisters=false;

inline std::string HexBytes(const void* p,size_t n) {constexpr char d[]="0123456789abcdef";const auto* b=static_cast<const u8*>(p);std::string o;o.reserve(n*2);for(size_t i=0;i<n;++i){o+=d[b[i]>>4];o+=d[b[i]&15];}return o;}
inline bool AllBytes(const u8* p,size_t n,u8 v){for(size_t i=0;i<n;++i)if(p[i]!=v)return false;return true;}
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

// ------------------------------------------------------------------------------------------------ rand() del CRT del EXE
// 0x827B3D devuelve los datos de la hebra; la semilla de rand/srand esta en +0x14 (0x821B11: mov [eax+14h], ecx; 0x821B1E: seed = seed * 0x343FD + 0x269EC3, devuelve (seed >> 16) & 0x7FFF).
// Punteros de funcion (y no llamadas directas) para que el arnes offline/livetest.cpp pueda ejecutar este mismo nucleo sin el juego.
inline uint8*(__cdecl* ExeThreadDataFn)()=reinterpret_cast<uint8*(__cdecl*)()>(0x827B3D);
inline int(__cdecl* ExeRandFn)()=reinterpret_cast<int(__cdecl*)()>(0x821B1E);
inline const void* SinTableAt=reinterpret_cast<const void*>(0xBB3E00);   // tabla de senos del juego (256 floats)
inline uint8* ExeThreadData() {return ExeThreadDataFn();}
inline uint32 ExeSeedGet() {return *reinterpret_cast<uint32*>(ExeThreadData()+0x14);}
inline void ExeSeedSet(uint32 s) {*reinterpret_cast<uint32*>(ExeThreadData()+0x14)=s;}
inline uint32 ExeRand() {return uint32(ExeRandFn());}

// ------------------------------------------------------------------------------------------------ llamada con medicion de ABI
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile;};
// thiscall con 7 argumentos de pila (argsPtr[0] queda en la cima): ECX = this, EAX/EDX sembrados; el llamado limpia la pila (`ret 1Ch`, popBytes = 0).
// Siembra 64 palabras bajo ESP, EBX/ESI/EDI con valores conocidos y mide ESP, no volatiles y DF justo tras el `call`.
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

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,kArena+2*kGuard> arena{};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::unique_ptr<HookRoute> route;
    uintptr_t entry{0x4A2580};
    // direcciones de los colaboradores: EXE y entrada de la DLL (resuelta)
    uintptr_t exeAt[7]{0x4A93C0,0x4A9400,0x4A9DC0,0x7ED9B0,0x7EDDF0,0x4A1660,0x4A21D0};
    uintptr_t dllAt[7]{};
    std::vector<Watch> watches;
    uint32 savedSeed{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}
inline json Words(const u32* w,size_t n){json a=json::array();for(size_t i=0;i<n;++i)a.push_back(w[i]);return a;}

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

// Parchea los siete colaboradores en el EXE y en la DLL con los mismos stubs (RAII: se retiran al salir del ambito).
struct Patches {
    ReversingProbe::EntryPatch e0,e1,e2,e3,e4,e5,e6,d0,d1,d2,d3,d4,d5,d6;
    explicit Patches(const Env& env):
        e0(env.exeAt[0],&StubGetParticle),e1(env.exeAt[1],&StubFreeUpParticle),e2(env.exeAt[2],&StubGetRWMatrix),e3(env.exeAt[3],&StubNormalize),e4(env.exeAt[4],&StubTransform),
        e5(env.exeAt[5],&StubRotate),e6(env.exeAt[6],&StubUpdateParticle),
        d0(env.dllAt[0],&StubGetParticle),d1(env.dllAt[1],&StubFreeUpParticle),d2(env.dllAt[2],&StubGetRWMatrix),d3(env.dllAt[3],&StubNormalize),d4(env.dllAt[4],&StubTransform),
        d5(env.dllAt[5],&StubRotate),d6(env.dllAt[6],&StubUpdateParticle) {}
};

inline json InputJson(const CpFx& f,unsigned n,uintptr_t base) {
    return {{"case",n},{"family",f.family},{"emission",Words(f.emission,kEmissionWords)},{"mult",Words(f.mult,7)},{"wld",Words(f.wld,16)},{"getRw",Words(f.getRw,12)},
            {"velOverride",Words(f.velOverride,3)},{"velAdd",Words(f.velAdd,3)},{"timeSince",f.timeSince},{"brightness",f.brightness},{"hasVel",f.hasVel},{"createLocal",f.createLocal},
            {"mustCreate",f.mustCreate},{"poolMode",f.poolMode},{"listMode",f.listMode},{"flagsOther",f.flagsOther},{"radiusKind",f.radiusKind},{"dirKind",f.dirKind},
            {"seed",f.seed},{"fill",f.fill},{"base",base}};
}

// Un caso: FxEmitter_c::CreateParticle con `this` = el emisor del arena.
inline void CaseCreate(Env& env,Totals& t,unsigned n) {
    const CpFx f=Make(n);
    u8* const base=arena.data()+kGuard;
    const json input=InputJson(f,n,reinterpret_cast<uintptr_t>(base));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        arena.fill(0xA5);BuildArena(base,f);
        trace.clear();traceBase=base;poolCalls=0;poolMode=f.poolMode;std::memcpy(getRwWords,f.getRw,sizeof getRwWords);
        ExeSeedSet(f.seed);
        const u32 args[7]{Ptr(base,kEmission),Ptr(base,kWld),f.hasVel?Ptr(base,kVel):0u,f.timeSince,Ptr(base,kMult),f.brightness,f.createLocal};
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            Patches patches(env);
            return Gate(entry,args,7,0xA5123456u,Ptr(base,kEmitter),0xC0DE5921u,Rand(n,9000+uint32(s)),0);   // `ret 1Ch`: el llamado limpia los 7 argumentos
        });
        const u32 randNext=ExeRand();
        const bool guards=AllBytes(arena.data(),kGuard,0xA5)&&AllBytes(arena.data()+kGuard+kArena,kGuard,0xA5);
        const u32 ret=sides[s].reg.eax?sides[s].reg.eax-u32(reinterpret_cast<uintptr_t>(base)):0xFFFFFFFFu;
        json tr=json::array();for(u32 v:trace)tr.push_back(v);
        json observed={{"arena",HexBytes(base,kArena)},{"guard_ok",guards},{"trace",tr},{"ret",ret},{"rand_next",randNext}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    ok&=rows[0]["observed"]["guard_ok"]==true&&rows[1]["observed"]["guard_ok"]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function","FxEmitter_c::CreateParticle"},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    for(unsigned i=0;i<7;++i)ready&=env.dllAt[i]&&ModuleOf(env.dllAt[i])==env.asi;
    std::vector<uintptr_t> watched{env.entry,body};
    for(unsigned i=0;i<7;++i){watched.push_back(env.exeAt[i]);watched.push_back(env.dllAt[i]);}
    for(const uintptr_t at:watched)if(at)env.watches.emplace_back(at,16);
    json watchedJson=json::array();for(const auto& w:env.watches)watchedJson.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    env.savedSeed=ExeSeedGet();
    json exeJson=json::array(),dllJson=json::array();
    for(unsigned i=0;i<7;++i){exeJson.push_back(env.exeAt[i]);dllJson.push_back(env.dllAt[i]);}
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",env.cases},{"cases",{{"FxEmitter_c::CreateParticle",env.cases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",exeJson},{"dll",dllJson}}},
        {"watched_entries",watchedJson},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"sin_table",HexBytes(SinTableAt,1024)},{"initial_seed",env.savedSeed},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<env.cases;++c)CaseCreate(env,totals,c);
        Write({{"event","function_summary"},{"function","FxEmitter_c::CreateParticle"},{"cases",env.cases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    ExeSeedSet(env.savedSeed);
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&ExeSeedGet()==env.savedSeed;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"seed",ExeSeedGet()}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
