#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco Helpers67W. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{Bytes,JumpsToDll,PumpMessages}, OriginalDependencies499::HookRoute y las cabeceras
// de ReversibleHook (Base/Simple/Virtual). Plantillas: reversing/Helpers66D (Gate de ABI, trazas, rutas RH) y reversing/Helpers66A.
//
// Banco par a par de UNA funcion (16384 casos): CWaterLevel::RenderSeaBedSegment 0x6E6870 (cdecl, 6 argumentos, `ret` del llamador). Original = bytes del EXE con el hook RH apagado;
// candidato = DLL con el hook encendido. La funcion no llama a ninguna otra (aritmetica x87 y escrituras al bufer temporal de agua), asi que NO hay colaboradores sustituidos.
// Estado que toca: los dos contadores (0xC4B950, 0xC4B954), cuatro vertices de 36 bytes y seis indices del bufer temporal (0xC4B958 / 0xC4D958). El banco guarda toda la region
// [0xC4B950, 0xC5F958) antes del primer caso, la rellena por caso (ruido / 00 / FF / CD / A5), y la restaura al terminar.
// ABI: ESP, EBX/ESI/EDI/EBP, DF, cadena SEH (fs:[0]) y x87 (FCW 24 bits, pila vacia). EAX/ECX/EDX de salida: solo registrados (los dos llamadores del EXE, 0x6EF822 y 0x6EF861, no los leen).
namespace H67Live {
using namespace H67Fix;
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
// El estado x87 se compara normalizado (FCW completa, TOP, etiquetas y ST VALIDOS: ninguno debe quedar). Los registros muertos no se comparan.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Los llamadores del EXE no los leen: solo se registran.
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
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`, retira popBytes (0 en `ret n`, 4*n en
// una funcion cdecl) y comprueba ESP, EBX/ESI/EDI/EBP y la cadena SEH (fs:[0]).
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
        add esp,popBytes
        mov espAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
        mov eax,dword ptr fs:[0]
        mov sehAfter,eax
        pop edi
        pop esi
        pop ebx
    }
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter,sehBefore==sehAfter};
}

// ------------------------------------------------------------------------------------------------ rutas y estado
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
inline constexpr unsigned kFunc=kFunctions;
inline constexpr const char* kName="CWaterLevel::RenderSeaBedSegment";
inline constexpr uintptr_t kEntry=0x6E6870;
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;                         // entrada del EXE y cuerpo de la DLL de la funcion: deben seguir intactos al volver
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};

struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). La region se comprueba DESPUES de la llamada (leccion 3 de BRIEF60).
template<class Fn> Side Execute(Env& env,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(kEntry)&&ModuleOf(Resolve(kEntry))==env.asi):!ReversingProbe::JumpsToDll(kEntry);
        side.reg=gate(kEntry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route->Invoke(candidate,call);
    return side;
}
inline void PrepareFloatingState() {
    RestoreFloatingState();PrimeX87();
    _controlfp(_PC_24|_RC_NEAR|_MCW_EM,_MCW_PC|_MCW_RC|_MCW_EM);   // 24 bits, al mas cercano, todas las excepciones enmascaradas (estado del juego)
}
inline json CommonRow(const Side& side,const std::array<uint8,108>& fp) {
    return {{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",side.mxcsr&0xFFC0u},{"stack_restored",side.reg.stack},
            {"nonvolatile_preserved",side.reg.nonvolatile},{"seh_chain_restored",side.reg.seh},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK},{"collaborators_dll",true}};
}
inline json Finish(Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","seh_chain_restored","direction_flag_clear","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    return row;
}

// ------------------------------------------------------------------------------------------------ region del bufer temporal de agua
inline u32 Fnv(const void* p,u32 n){u32 h=2166136261u;const auto* b=static_cast<const u8*>(p);for(u32 i=0;i<n;++i){h^=b[i];h*=16777619u;}return h;}
inline u8* Region(){return reinterpret_cast<u8*>(uintptr_t(kRegionAddress));}
inline std::vector<u8> savedRegion;                      // estado del juego antes del banco
inline std::array<u8,kGuardBytes> savedBefore{},savedAfter{};   // bytes vigilados fuera de la region (antes de ella y justo despues)
inline void SaveRegion(){savedRegion.assign(Region(),Region()+kRegionBytes);}
inline void RestoreRegion(){std::memcpy(Region(),savedRegion.data(),kRegionBytes);}
inline bool RegionRestored(){return std::memcmp(Region(),savedRegion.data(),kRegionBytes)==0;}
inline bool GuardsIntact(){return std::memcmp(Region()-kGuardBytes,savedBefore.data(),kGuardBytes)==0&&std::memcmp(Region()+kRegionBytes,savedAfter.data(),kGuardBytes)==0;}

// ------------------------------------------------------------------------------------------------ la funcion
inline json CaseInput(const Case& f,const u8* ref){
    const u32 v=f.vtx,i=f.idx;
    return {{"func",0},{"n",f.n},{"family",f.family},{"x",f.x},{"y",f.y},{"min_x",f.minX},{"max_x",f.maxX},{"min_y",f.minY},{"max_y",f.maxY},{"vtx",v},{"idx",i},
            {"pre_vertices",HexBytes(ref+kVtxOff+v*kVertexBytes,4*kVertexBytes)},{"pre_indices",HexBytes(ref+kIdxOff+2*i,12)},
            {"mode",f.mode},{"seed",f.seedRegion},{"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseSegment(Env& env,Totals& t,unsigned n) {
    const Case f=MakeCase(n);
    static std::array<u8,kRegionBytes> ref{};
    BuildRegionRef(f,ref.data());
    const json input=CaseInput(f,ref.data());
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(Region(),ref.data(),kRegionBytes);
        std::memcpy(savedBefore.data(),Region()-kGuardBytes,kGuardBytes);std::memcpy(savedAfter.data(),Region()+kRegionBytes,kGuardBytes);
        const u32 args[6]{u32(f.x),u32(f.y),f.minX,f.maxX,f.minY,f.maxY};
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){return Gate(entry,args,6,f.eax,f.ecx,f.edx,f.stack,24);});   // cdecl: el llamador retira 24 bytes
        const u8* after=Region();
        u32 vtxAfter{},idxAfter{};std::memcpy(&vtxAfter,after,4);std::memcpy(&idxAfter,after+4,4);
        // El resto de la region (todo menos los contadores, los seis indices y los cuatro vertices) debe quedar igual que la imagen previa.
        const u32 v0=kVtxOff+f.vtx*kVertexBytes,i0=kIdxOff+2*f.idx;
        bool rest=true;
        for(u32 i=0;i<kRegionBytes&&rest;++i){
            if(i<8||(i>=i0&&i<i0+12)||(i>=v0&&i<v0+4*kVertexBytes))continue;
            rest=after[i]==ref[i];
        }
        json observed={{"vertices",HexBytes(after+v0,4*kVertexBytes)},{"indices",HexBytes(after+i0,12)},{"vtx_after",vtxAfter},{"idx_after",idxAfter},{"rest_ok",rest},{"guard_ok",GuardsIntact()}};
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        RestoreRegion();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&RegionRestored();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kName,n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    {const uintptr_t body=env.route?env.route->Body():0;ready&=body&&ModuleOf(body)==env.asi;}
    env.watches.emplace_back(kEntry,16);
    if(env.route)env.watches.emplace_back(env.route->Body(),16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    PrepareFloatingState();
    SaveRegion();
    RestoreFloatingState();
    json cases=json::object();cases[kName]=kCases;
    u32 vtx0{},idx0{};std::memcpy(&vtx0,savedRegion.data(),4);std::memcpy(&idx0,savedRegion.data()+4,4);
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunc},{"pairs",kFunc*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",false},
        {"collaborators",json::object()},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},
        {"initial_globals",{{"vertices_stored",vtx0},{"indices_stored",idx0},{"region_fnv",Fnv(savedRegion.data(),kRegionBytes)}}},
        {"layout",{{"region_address",kRegionAddress},{"region_bytes",kRegionBytes},{"idx_off",kIdxOff},{"vtx_off",kVtxOff},{"vertex_bytes",kVertexBytes},{"guard_bytes",kGuardBytes}}}});
    Totals totals;
    if(ready) {
        Totals ft;
        for(unsigned c=0;c<kCases;++c)CaseSegment(env,ft,c);
        totals.pairs=ft.pairs;totals.failures+=ft.failures;totals.passed&=ft.passed;
        Write({{"event","function_summary"},{"function",kName},{"cases",kCases},{"failures",ft.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    RestoreRegion();
    bool stateRestored=entriesIntact&&RegionRestored();
    u32 vtx1{},idx1{};std::memcpy(&vtx1,Region(),4);std::memcpy(&idx1,Region()+4,4);
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"globals",{{"vertices_stored",vtx1},{"indices_stored",idx1},{"region_fnv",Fnv(Region(),kRegionBytes)}}}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
