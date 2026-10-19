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
// Nucleo del banco FightMeleeData31: CTaskSimpleFight::LoadMeleeData (0x5BEDC0, estatica, cdecl sin argumentos, void). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, FILESTREAM,
// ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}, OriginalDependencies499::HookRoute y los tipos del juego. Plantillas: reversing/PartnerChatAbort30 (Gate, estructura) y
// reversing/FileLoaderLoad2dEffect27 (modelo de sscanf y comodines; aqui no hacen falta comodines: las fixtures solo usan lecturas modeladas).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. La memoria que toca la funcion son globales del EXE (tabla de combos, niveles y lo que hay
// despues): se observa la REGION [0xC17090, 0xC17940) rellenada con un patron antes de cada lado y restaurada al final. Colaboradores sustituidos por el mismo falso en ambos lados
// (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto antes de parchear): CFileMgr::OpenFile (0x538900), CFileLoader::LoadLine (0x536F80), CFileMgr::CloseFile (0x5389D0) y
// CAnimManager::GetAnimGroupName (0x4D3A20); el fichero es la lista de lineas del caso. sscanf, strncmp y strcmp se ejecutan de verdad (sscanf del EXE por direccion en ambos lados).

namespace FightMeleeData31Live {
using namespace FightMeleeData31Fix;
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
// (CTaskComplexPartner::CreateNextSubTask 0x683B46 los sobrescribe: `mov ecx,[esp+30h]` en 0x6840AC), por defecto solo se registran.
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada (el EXE instala un marco SEH en 0x683120..0x683228 y lo retira).
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Igual que DecisionAdd27::Gate (sin argumentos: la funcion es thiscall sin parametros y `ret`): siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; captura los registros y EFLAGS justo tras el `call`.
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

// ------------------------------------------------------------------------------------------------ memoria y falsos
inline const C* gCase{};
inline unsigned gLineIdx{};
inline char gLineBuf[1024];
enum : u32 {EvOpen=1,EvRead=2,EvClose=3,EvName=4};
inline std::array<u32,kMaxEvents*3> evw{};inline unsigned nev{};
inline unsigned nOpen{},nRead{},nClose{},nName{};
inline void Push(u32 k,u32 a,u32 b){if(nev<kMaxEvents){evw[nev*3]=k;evw[nev*3+1]=a;evw[nev*3+2]=b;}++nev;}
inline u32 Fnv32(const char* s){u32 h=2166136261u;for(;*s;++s)h=(h^u8(*s))*16777619u;return h;}
inline u64 Fnv64(const void* p,size_t n){u64 h=1469598103934665603ull;const auto* b=static_cast<const u8*>(p);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
// CFileMgr::OpenFile (0x538900, cdecl): registra el hash del nombre y del modo; devuelve un identificador fijo.
inline FILESTREAM __cdecl FakeOpen(const char* path,const char* mode){++nOpen;Push(EvOpen,Fnv32(path),Fnv32(mode));return reinterpret_cast<FILESTREAM>(0x00ABC123);}
// CFileLoader::LoadLine (0x536F80, cdecl): entrega la linea siguiente copiada a un buffer propio, nulo al terminar.
inline char* __cdecl FakeLoadLine(FILESTREAM f){
    ++nRead;
    if(gLineIdx>=gCase->lines.size()){Push(EvRead,0xFFFFFFFFu,u32(uintptr_t(f)));return nullptr;}
    Push(EvRead,gLineIdx,u32(uintptr_t(f)));
    std::strncpy(gLineBuf,gCase->lines[gLineIdx].c_str(),sizeof(gLineBuf)-1);gLineBuf[sizeof(gLineBuf)-1]=0;
    ++gLineIdx;
    return gLineBuf;
}
// CFileMgr::CloseFile (0x5389D0, cdecl)
inline int __cdecl FakeClose(FILESTREAM f){++nClose;Push(EvClose,u32(uintptr_t(f)),0);return 0;}
// CAnimManager::GetAnimGroupName (0x4D3A20, cdecl): nombres del caso (el indice fuera de ellos devuelve "?")
inline const char* __cdecl FakeGroupName(int i){++nName;Push(EvName,u32(i),0);return i>=0&&size_t(i)<gCase->groups.size()?gCase->groups[size_t(i)].c_str():"?";}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;       // las entradas del EXE (parcheadas solo durante la llamada, con RAII): deben seguir siendo lo que eran
    Pair pOpen{},pRead{},pClose{},pName{};   // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,10> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Region observada: copia de seguridad del original (se restaura tras cada caso) y contador de grupos (0xB4EA28)
inline std::array<u8,kRegion> savedRegion{};inline u32 savedGroupCount{};inline bool regionSaved{};
inline u8* RegionPtr(){return reinterpret_cast<u8*>(kRegionBase);}
inline u32& GroupCount(){return *reinterpret_cast<u32*>(kGroupCountAddress);}
inline void SaveRegion(){std::memcpy(savedRegion.data(),RegionPtr(),kRegion);savedGroupCount=GroupCount();regionSaved=true;}
inline void RestoreRegion(){if(regionSaved){std::memcpy(RegionPtr(),savedRegion.data(),kRegion);GroupCount()=savedGroupCount;}}
inline bool RegionRestored(){return !regionSaved||(std::memcmp(RegionPtr(),savedRegion.data(),kRegion)==0&&GroupCount()==savedGroupCount);}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los falsos se instalan DESPUES de comprobar la ruta.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        {
            Patches patches;
            patches.Add(env.pOpen,&FakeOpen);
            patches.Add(env.pRead,&FakeLoadLine);
            patches.Add(env.pClose,&FakeClose);
            patches.Add(env.pName,&FakeGroupName);
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"seh_restored",side.reg.seh},{"route_ok",side.routeOK}};
}
inline json Finish(Env& env,Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}
inline std::string Hex64(u64 v){char b[17];std::snprintf(b,sizeof b,"%016llx",static_cast<unsigned long long>(v));return b;}

// ------------------------------------------------------------------------------------------------ CTaskSimpleFight::LoadMeleeData
inline void CaseLoad(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    gCase=&c;
    std::array<u8,kRegion> prefill{};
    FillWords(prefill.data(),kRegion,c.prefillSeed);
    json lines=json::array();for(const auto& l:c.lines)lines.push_back(l);
    json groups=json::array();for(const auto& g:c.groups)groups.push_back(g);
    const json input={{"family",c.family},{"num_groups",c.numGroups},{"groups",groups},{"lines",lines},{"prefill_seed",c.prefillSeed},{"prefill_hash",Hex64(Fnv64(prefill.data(),kRegion))},
                      {"eax",c.eax},{"ecx",c.ecx},{"edx",c.edx},{"stack_seed",c.stack},{"region_base",kRegionBase},{"region_off",kRegionOff},{"region_bytes",kRegionBytes}};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(RegionPtr(),prefill.data(),kRegion);GroupCount()=c.numGroups;
        gLineIdx=0;nev=0;nOpen=nRead=nClose=nName=0;
        sides[s]=Execute(env,*env.route,0x5BEDC0,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,c.ecx,c.edx,c.stack,0);});

        json obs;
        std::array<u8,kRegion> post{};std::memcpy(post.data(),RegionPtr(),kRegion);
        const u32 count=GroupCount();
        obs["events_hash"]=Hex64(Fnv64(evw.data(),std::min<size_t>(nev,kMaxEvents)*12));
        obs["n_events"]=nev;obs["n_open"]=nOpen;obs["n_read"]=nRead;obs["n_close"]=nClose;obs["n_name"]=nName;
        obs["group_count_after"]=count;
        obs["window_hash"]=Hex64(Fnv64(post.data(),kRegion));
        json first=json::array();unsigned changed=0;
        for(u32 i=0;i<kRegion;i+=4){u32 a,b;std::memcpy(&a,post.data()+i,4);std::memcpy(&b,prefill.data()+i,4);if(a!=b){if(++changed<=48)first.push_back({i,a});}}
        obs["changed_words"]=changed;obs["first_changes"]=first;
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreRegion();
        RestoreFloatingState();
        out["globals_restored"]=env.WatchesIntact()&&RegionRestored();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    gCase=nullptr;
    Finish(env,t,"CTaskSimpleFight::LoadMeleeData",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    for(uintptr_t a:{uintptr_t(0x538900),uintptr_t(0x536F80),uintptr_t(0x5389D0),uintptr_t(0x4D3A20)})env.watches.emplace_back(a,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    SaveRegion();
    json stubbed=json::array();
    for(const Pair* p:{&env.pOpen,&env.pRead,&env.pClose,&env.pName})stubbed.push_back({{"exe",p->exe},{"dll",p->dll}});
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskSimpleFight::LoadMeleeData",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe_real",json::array({json{{"sscanf",0x8220AD}}})},{"stubbed",stubbed}}},
        {"region",{{"base",kRegionBase},{"bytes",kRegion},{"table",kTableAt},{"levels",kLevelsAt},{"group_count",kGroupCountAddress}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseLoad(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskSimpleFight::LoadMeleeData"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()&&RegionRestored()}});
    }
    RestoreRegion();
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&RegionRestored();
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"region_restored",RegionRestored()}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
