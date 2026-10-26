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

// Nucleo del banco Helpers67S. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y las cabeceras de ReversibleHook (Base/Simple). Plantilla: reversing/Helpers66D (constructor, Gate de ABI, trazas) y reversing/Helpers65A.
//
// Banco par a par de UNA funcion (16384 casos): CDecisionMakerTypes::CDecisionMakerTypes 0x4650F0 (constructor thiscall, ECX = this, `ret`).
// Original = bytes del EXE con el hook RH del constructor apagado; candidato = DLL con SOLO ese hook encendido.
// Colaboradores del EXE (0x821D30 `eh vector constructor iterator`, 0x4650A0 constructor de CDecisionMaker, 0x6040C0 constructor de CDecision y 0x600530 CDecision::SetDefault):
//   - el hook de CDecision::SetDefault (0x600530) queda ENCENDIDO en ambos lados (Simple::Switch con el hook apagado reescribe el cuerpo de la DLL con un salto al EXE, asi que apagarlo haria que el
//     candidato llamara al EXE): en el lado EXE 0x600530 salta a SetDefault de la DLL (validada en su propio banco) y en el candidato la DLL la llama directamente;
//   - en el lado candidato las otras tres entradas del EXE (0x821D30, 0x4650A0, 0x6040C0), sin hook, se sustituyen por trampas que cuentan la llamada: la DLL no debe llamar al EXE (esperado: cero).
// Se compara: el objeto de 0xF1C0 bytes completo (CRC-32; incluye m_EventIndices intacto), la arena fuera del objeto y las guardas, la region de globales de 0x70 bytes (banderas de grupo
// 0xC0AFE0, ScriptReferenceIndex 0xC0AFF4, m_IsActive 0xC0B01C y 16 bytes a cada lado; la instancia 0xC0B030 conserva su valor), EAX = this, ECX = fs:[0] de entrada, EDX = 0, y las compuertas
// de ABI (ESP, EBX/ESI/EDI/EBP, DF, cadena SEH, x87 normalizado).
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
// EAX/ECX/EDX de salida: el EXE los deja en (this, fs:[0] de entrada, 0); GetInstance (0x4684F0) toma el EDX del constructor como residuo propio. Aqui se EXIGEN los tres en ambos lados
// (observed: eax_is_this, ecx_is_head, edx_zero), ademas de registrarlos.
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
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;u32 sehHead;};
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; captura los registros y EFLAGS justo tras el `call` y comprueba ESP, EBX/ESI/EDI/EBP y la cadena SEH (fs:[0]). Sin argumentos de pila
// (el constructor es `ret` a secas). sehHead = fs:[0] con el que se entra a la funcion.
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed){
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
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter,sehBefore==sehAfter,sehBefore};
}

// ------------------------------------------------------------------------------------------------ estado
alignas(16) inline std::array<u8,kArena> arena{};
inline u8* AB(){return arena.data();}
alignas(16) inline std::array<u8,kArena> refImage{};
inline u32 Crc32(const void* p,u32 n){
    static u32 table[256]{};
    if(!table[1])for(u32 i=0;i<256;++i){u32 c=i;for(int k=0;k<8;++k)c=(c&1)?(0xEDB88320u^(c>>1)):(c>>1);table[i]=c;}
    u32 c=0xFFFFFFFFu;const auto* b=static_cast<const u8*>(p);
    for(u32 i=0;i<n;++i)c=table[(c^b[i])&0xFF]^(c>>8);
    return ~c;
}

// Trampas de los colaboradores del EXE (solo en el lado candidato, solo durante la llamada): la DLL no debe llamarlos. Cuentan y devuelven con la convencion de cada uno.
inline u32 trapCalls{};
inline void* __fastcall TrapCtorThis(void* self,void*){++trapCalls;return self;}                 // 0x4650A0 (EAX = this) y 0x6040C0 (EAX = ECX)
inline void __fastcall TrapSetDefault(void*,void*){++trapCalls;}                                  // 0x600530
inline void __stdcall TrapVecCtor(u32,u32,u32,u32,u32){++trapCalls;}                              // 0x821D30 (`ret 14h`)

struct Pair {uintptr_t exe,dll;};
struct Collab {Pair pair;void* fake;};
inline constexpr uintptr_t kEntry=0x4650F0;
inline constexpr const char* kName="CDecisionMakerTypes::CDecisionMakerTypes";
inline constexpr uintptr_t kSetDefault=0x600530;
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Collab> collabs;                          // colaboradores del EXE con trampa en el lado candidato
    std::vector<Watch> watches;                           // entradas del EXE y cuerpos de la DLL: deben seguir intactos al volver
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,8> p;int n{};
    void Add(const Pair& pr,void* stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};u32 traps{};};

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Las trampas se instalan DESPUES de comprobar rutas y colaboradores, solo en el lado candidato y
// solo mientras dura la llamada; los globales se comprueban DESPUES de quitar los parches (leccion 3 de BRIEF60).
template<class Fn> Side Execute(Env& env,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(kEntry)&&ModuleOf(Resolve(kEntry))==env.asi):!ReversingProbe::JumpsToDll(kEntry);
        bool collab=true;
        for(const auto& c:env.collabs)collab&=!ReversingProbe::JumpsToDll(c.pair.exe);   // sin hook: son codigo del EXE
        collab&=ReversingProbe::JumpsToDll(kSetDefault)&&ModuleOf(Resolve(kSetDefault))==env.asi;   // SetDefault: hook encendido, cuerpo en la DLL
        side.collabOK=collab;
        trapCalls=0;
        {
            Patches patches;
            if(candidate)for(const auto& c:env.collabs)patches.Add(c.pair,c.fake);
            side.reg=gate(kEntry);
        }
        side.traps=trapCalls;
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"seh_chain_restored",side.reg.seh},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK},{"collaborators_dll",side.collabOK}};
}
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};

// ------------------------------------------------------------------------------------------------ regiones del juego
inline u8* RegionPtr(){return reinterpret_cast<u8*>(uintptr_t(kRegionAddr));}
struct Region {
    std::array<u8,kRegionBytes> bytes{};
    void Save(){std::memcpy(bytes.data(),RegionPtr(),kRegionBytes);}
    void Restore()const{std::memcpy(RegionPtr(),bytes.data(),kRegionBytes);}
    bool Equal(const Region& o)const{return bytes==o.bytes;}
    u32 Instance()const{u32 v{};std::memcpy(&v,bytes.data()+kInstanceOff,4);return v;}
};
inline Region initialRegion;   // valores del juego antes del banco

// ------------------------------------------------------------------------------------------------ el constructor
inline json CtorInput(const Case& f,const std::array<u8,kRegionBytes>& pre){
    return {{"func",0},{"n",f.n},{"family",f.family},{"obj_off",f.objOff},{"mode",f.mode},{"seed",f.seedArena},{"gmode",f.gmode},{"gseed",f.gseed},
            {"region_pre",HexBytes(pre.data(),kRegionBytes)},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseCtor(Env& env,Totals& t,unsigned n) {
    const Case f=Make(n);
    BuildRef(f,refImage.data());
    std::array<u8,kRegionBytes> pre{};
    BuildRegionPre(f,initialRegion.Instance(),pre.data());
    const json input=CtorInput(f,pre);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(AB(),refImage.data(),kArena);
        u8* const obj=AB()+f.objOff;
        std::memcpy(RegionPtr(),pre.data(),kRegionBytes);
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){return Gate(entry,f.eax,u32(uintptr_t(obj)),f.edx,f.stack);});
        Region after;after.Save();
        initialRegion.Restore();
        bool rest=true;
        for(u32 i=0;i<kArena;++i){if(i>=f.objOff&&i<f.objOff+kObjBytes)continue;if(AB()[i]!=refImage[i]){rest=false;break;}}
        json observed={{"obj_crc",Crc32(obj,kObjBytes)},{"rest_ok",rest},{"region",HexBytes(after.bytes.data(),kRegionBytes)},
                       {"eax_is_this",sides[s].reg.eax==u32(uintptr_t(obj))},{"ecx_is_head",sides[s].reg.ecx==sides[s].reg.sehHead},{"edx_zero",sides[s].reg.edx==0u}};
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        Region now;now.Save();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&now.Equal(initialRegion);
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","seh_chain_restored","direction_flag_clear","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    ok&=sides[1].traps==0;   // la DLL no llama a ningun colaborador del EXE
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",kName},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"candidate_exe_calls",sides[1].traps},{"equal",ok},{"accepted",ok}};
    Write(row);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    ready&=env.route&&env.route->Registered()&&ReversingProbe::JumpsToDll(kSetDefault)&&ModuleOf(Resolve(kSetDefault))==env.asi;
    {const uintptr_t body=env.route?env.route->Body():0;ready&=body&&ModuleOf(body)==env.asi;}
    json collaborators=json::array();
    for(const auto& c:env.collabs){
        collaborators.push_back({{"exe",c.pair.exe},{"dll",c.pair.dll},{"hooked",false}});
        env.watches.emplace_back(c.pair.exe,16);
        ready&=!ReversingProbe::JumpsToDll(c.pair.exe);
    }
    env.watches.emplace_back(kSetDefault,16);env.watches.emplace_back(kEntry,16);if(env.route)env.watches.emplace_back(env.route->Body(),16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    PrepareFloatingState();
    initialRegion.Save();
    RestoreFloatingState();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",json::object({{kName,kCases}})},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"set_default_hook_on",ready},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},
        {"initial_region",HexBytes(initialRegion.bytes.data(),kRegionBytes)},
        {"layout",{{"arena",kArena},{"obj_bytes",kObjBytes},{"region_addr",kRegionAddr},{"region_bytes",kRegionBytes}}}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseCtor(env,totals,c);
        Write({{"event","function_summary"},{"function",kName},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    Region fin;fin.Save();
    bool stateRestored=entriesIntact&&fin.Equal(initialRegion);
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"region",HexBytes(fin.bytes.data(),kRegionBytes)}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
