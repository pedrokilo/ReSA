#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco PlateCharset69. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y las cabeceras del juego que usa LiveComparison.hpp. Plantilla: reversing/Helpers66A (Gate de ABI, falsos identicos en ambos lados con parche
// de entrada en el EXE y en el cuerpo de la DLL) y reversing/Helpers67S (arena y CRC).
//
// Banco par a par de UNA funcion (16384 casos): CCustomCarPlateMgr::LoadPlatecharsetDat 0x6FDC00 (cdecl, `ret`; AL = 1 / 0). Original = bytes del EXE con el hook RH apagado;
// candidato = DLL con SOLO ese hook encendido.
// Colaboradores de ARCHIVO sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL): CFileMgr::SetDir 0x5387D0,
// CFileMgr::OpenFile 0x538900, CFileLoader::LoadLine(FILESTREAM) 0x536F80 y CFileMgr::CloseFile 0x5389D0 (los cuatro con hook). El falso de LoadLine entrega las lineas del caso
// desde un buffer controlado; los falsos registran sus argumentos EN ORDEN. strtok (0x82244B) y atoi (0x82258E) NO se sustituyen: el lado EXE ejecuta el CRT del EXE y el candidato el
// strtok del CRT de la DLL y PlatecharAtoi, de modo que el banco los compara de verdad.
// Se compara: AL, la traza de eventos de los falsos (SetDir/OpenFile/LoadLine/CloseFile con sus argumentos), la arena de salida completa (CRC-32, guardas incluidas) y los dwords escritos,
// el buffer de linea tras el strtok (el CRT escribe NUL en los separadores) y las compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, cadena SEH, x87 normalizado).
namespace Pc69Live {
using namespace Pc69Fix;
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
// El estado x87 se compara normalizado (FCW completa, TOP, etiquetas y ST VALIDOS). Los registros muertos no se comparan.
inline constexpr bool kCompareDeadX87Registers=false;
// EAX/ECX/EDX de salida: la funcion no tiene llamadores. Solo AL se compara (en `observed`); el resto se registra.
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
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; empuja los argumentos (args[0] arriba), llama y captura registros y EFLAGS justo tras el `call`;
// popBytes = bytes que limpia el LLAMADOR (cdecl: 4*n).
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

// ------------------------------------------------------------------------------------------------ colaboradores, parches y rutas
struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: sin RH_ScopedInstall; dll=0: la DLL llama a la direccion del EXE
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
struct StubSpec {Pair pair;void* fake;};
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> p;
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    void Reset(){while(!p.empty())p.pop_back();}
    ~Patches(){Reset();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
inline constexpr uintptr_t kEntry=0x6FDC00;
inline constexpr const char* kName="CCustomCarPlateMgr::LoadPlatecharsetDat";
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<StubSpec> stubs;
    std::vector<Watch> watches;                                     // entradas del EXE de los colaboradores y de la funcion: deben seguir siendo lo que eran al empezar
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la funcion encendido (cuerpo en el modulo ASI). En ambos lados los colaboradores con hook deben ser saltos a la DLL;
// los falsos se instalan DESPUES de comprobar rutas y colaboradores y se quitan antes de volver (globals_restored se comprueba despues).
template<class Fn> Side Execute(Env& env,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(kEntry)&&ModuleOf(Resolve(kEntry))==env.asi):!ReversingProbe::JumpsToDll(kEntry);
        bool collab=true;
        for(const auto& s:env.stubs) {
            const auto& pr=s.pair;
            collab&=(pr.hooked?(ReversingProbe::JumpsToDll(pr.exe)&&ModuleOf(Resolve(pr.exe))==env.asi):!ReversingProbe::JumpsToDll(pr.exe))
                    &&(pr.hooked?(pr.dll!=0&&ModuleOf(pr.dll)==env.asi):(pr.dll==0||ModuleOf(pr.dll)==env.asi));
        }
        side.collabOK=collab;
        {
            Patches patches;
            for(const auto& s:env.stubs)patches.Add(s.pair,s.fake);
            side.reg=gate(kEntry);
        }
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"seh_restored",side.reg.seh},{"route_ok",side.routeOK},{"collaborators_dll",side.collabOK}};
}
inline u32 Crc32(const void* p,u32 n){
    static u32 table[256]{};
    if(!table[1])for(u32 i=0;i<256;++i){u32 c=i;for(int k=0;k<8;++k)c=(c&1)?(0xEDB88320u^(c>>1)):(c>>1);table[i]=c;}
    u32 c=0xFFFFFFFFu;const auto* b=static_cast<const u8*>(p);
    for(u32 i=0;i<n;++i)c=table[(c^b[i])&0xFF]^(c>>8);
    return ~c;
}
inline u32 Fnv(const void* p,size_t n) {u32 h=2166136261u;for(size_t i=0;i<n;++i){h^=static_cast<const u8*>(p)[i];h*=16777619u;}return h;}
inline u32 FnvStr(const char* s) {return s?Fnv(s,strlen(s)):0u;}

// ------------------------------------------------------------------------------------------------ falsos de los colaboradores de archivo
// Traza de eventos en un arreglo fijo (los falsos no asignan memoria): [tipo, a, b]
struct Ev {u32 kind{},a{},b{};};
inline std::array<Ev,128> evs{};inline unsigned nev{};
inline void PushEv(u32 kind,u32 a,u32 b) {if(nev<evs.size())evs[nev]=Ev{kind,a,b};++nev;}
inline json EvsJson() {json out=json::array();for(unsigned i=0;i<nev&&i<evs.size();++i)out.push_back(json::array({evs[i].kind,evs[i].a,evs[i].b}));return out;}

alignas(16) inline std::array<u8,kArena> arena{};
alignas(16) inline std::array<u8,kArena> refImage{};
inline u8* AB(){return arena.data();}
alignas(16) inline char nameBuf[kNameBuf]{};
alignas(16) inline char lineBuf[kLineBuf]{};
inline const Case* cur{};
inline u32 lineIdx{};
inline int32 __cdecl FakeSetDir(const char* path) {PushEv(1,FnvStr(path),0);return 0;}                                                      // 0x5387D0, cdecl
inline void* __cdecl FakeOpenFile(const char* path,const char* mode) {PushEv(2,FnvStr(path),FnvStr(mode));return reinterpret_cast<void*>(uintptr_t(cur->openRet));}   // 0x538900, cdecl
inline char* __cdecl FakeLoadLine(void* file) {                                                                                             // 0x536F80, cdecl
    const u32 idx=lineIdx;
    PushEv(3,u32(uintptr_t(file)),idx);
    if(!file||idx>=cur->lines.size())return nullptr;
    ++lineIdx;
    const auto& s=cur->lines[idx];
    memcpy(lineBuf,s.data(),s.size());lineBuf[s.size()]=0;
    return lineBuf;
}
inline int32 __cdecl FakeCloseFile(void* file) {PushEv(4,u32(uintptr_t(file)),0);return int32(cur->closeRet);}                                  // 0x5389D0, cdecl
// Campo _token del tiempo de ejecucion por hilo del EXE (lo escribe el strtok del EXE): se guarda y restaura alrededor de cada llamada.
inline uint32& ExeStrtokToken() {return *reinterpret_cast<uint32*>(reinterpret_cast<uint8*(__cdecl*)()>(uintptr_t(0x827B3D))()+0x18);}

// ------------------------------------------------------------------------------------------------ el caso
inline void CaseLoad(Env& env,Totals& t,unsigned n) {
    const Case f=Make(n);
    cur=&f;
    FillArena(refImage.data(),kArena,f.mode,f.seedArena);
    const json input=json::parse(JsonOf(f));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        memcpy(AB(),refImage.data(),kArena);
        memset(nameBuf,0,sizeof(nameBuf));memcpy(nameBuf,f.name.data(),f.name.size());
        memset(lineBuf,0xDD,sizeof(lineBuf));
        lineIdx=0;nev=0;
        u8* const data=AB()+f.dataOff;
        const uint32 tokenBefore=ExeStrtokToken();
        const u32 args[2]{u32(uintptr_t(nameBuf)),u32(uintptr_t(data))};
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){return Gate(entry,args,2,f.eax,f.ecx,f.edx,f.stack,8);});
        ExeStrtokToken()=tokenBefore;
        json out=json::array();
        {
            int last=-1;
            for(u32 i=0;i<kOutWords;++i)if(memcmp(data+4*i,refImage.data()+f.dataOff+4*i,4)!=0)last=int(i);
            for(int i=0;i<=last;++i){u32 w;memcpy(&w,data+4*i,4);out.push_back(w);}
        }
        json observed={{"al",sides[s].reg.eax&0xFFu},{"events",EvsJson()},{"arena_crc",Crc32(AB(),kArena)},{"out",out},{"line_hash",Fnv(lineBuf,sizeof(lineBuf))},{"events_total",nev}};
        auto fp=NormalizeFp(sides[s].fp);
        json row=CommonRow(sides[s],fp);row["observed"]=observed;
        RestoreFloatingState();
        row["globals_restored"]=env.WatchesIntact()&&ExeStrtokToken()==tokenBefore;
        row["floating_state_restored"]=FloatingStateRestored();
        rows[s]=row;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",kName},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    {const uintptr_t body=env.route?env.route->Body():0;ready&=body&&ModuleOf(body)==env.asi;}
    for(const auto& s:env.stubs)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},{"cases",json::object({{kName,kCases}})},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},
        {"layout",{{"arena",kArena},{"out_words",kOutWords},{"max_lines",kMaxLines},{"line_buf",kLineBuf}}}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseLoad(env,totals,c);
        Write({{"event","function_summary"},{"function",kName},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
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
