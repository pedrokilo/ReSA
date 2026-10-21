#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include "Fixtures.hpp"
// Nucleo del banco D3DResource63 (lote 63). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute. Plantilla: reversing/D3DResource62 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE).
//
// Banco par a par de DOS funciones de D3DResourceSystem (GetTotalPixelsSize, GetTotalIndexDataSize). Original = bytes del EXE con el hook RH de la funcion apagado; candidato = DLL con
// SOLO ese hook encendido. Los colaboradores D3DTextureBuffer::GetTotalDataSize (0x7300A0) y D3DIndexDataBuffer::GetTotalDataSize (0x7303B0) (sin reconstruir) se sustituyen por falsos
// identicos en ambos lados que registran EN ORDEN su ECX (la direccion del buffer) y devuelven el valor del caso; el segundo puede reescribir una palabra del bloque de globales
// (0xC87C60..0xC87FE8) en la llamada k-esima (el EXE lee el contador una sola vez). Ninguna de las dos tiene llamadores directos en el EXE: EAX se compara, ECX/EDX solo se registran.

namespace D3D63Live {
using namespace D3D63Fix;
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
// Se compara lo observable de la x87 (FCW completa, TOP, etiquetas y ST validos); los registros vacios y las banderas de FSW no.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md): por defecto solo se registran.
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
// popBytes = bytes que limpia el LLAMADOR (cdecl: 4*n; `ret n` del callee: 0).
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
struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: sin RH_ScopedInstall (la entrada del EXE sigue siendo codigo original); dll=0: la DLL llama a la direccion del EXE
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
struct StubSpec {Pair pair;void* fake;};
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> p;
    Patches(){p.reserve(16);}   // sin reasignaciones mientras haya parches de new/delete activos
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    ~Patches(){while(!p.empty())p.pop_back();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::array<std::unique_ptr<HookRoute>,kFns> route;              // una por funcion
    std::vector<StubSpec> stubs;                                    // colaboradores sustituidos (los mismos para todas las funciones: cualquier llamada no esperada queda registrada)
    std::vector<Watch> watches;                                     // entradas del EXE de todos los colaboradores: deben seguir siendo lo que eran al empezar
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los colaboradores sustituidos no tienen hook en la DLL: su entrada del EXE debe seguir
// siendo codigo original en ambos lados; los falsos se instalan DESPUES de comprobar rutas y colaboradores y solo mientras dura la llamada.
template<class Fn> Side Execute(Env& env,unsigned fnIdx,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    HookRoute& route=*env.route[fnIdx];
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const auto& s:env.stubs) {
            const auto& pr=s.pair;
            collab&=!ReversingProbe::JumpsToDll(pr.exe)&&pr.dll==0&&!pr.hooked;
        }
        side.collabOK=collab;
        {
            Patches patches;
            for(const auto& s:env.stubs)patches.Add(s.pair,s.fake);
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
inline json Finish(Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    return row;
}

// ================================================================================================ falsos, fixtures y casos
inline constexpr const char* kFn[kFns]{"D3DResourceSystem::GetTotalPixelsSize","D3DResourceSystem::GetTotalIndexDataSize"};
inline constexpr const char* kName[kFns]{"GetTotalPixelsSize","GetTotalIndexDataSize"};
inline constexpr uintptr_t kEntry[kFns]{0x730660,0x7307F0};

// ---- eventos de los falsos (nada aqui asigna memoria: se usan dentro de la ventana de parches)
struct Ev {u32 kind{};u32 ecx{};u32 ret{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
struct FakeState {u32 mode{},seed{},reK{0xFFFFFFFFu},reKind{},reVal{};u32 calls{};};
inline FakeState F;
inline u32& RegionWord(u32 w){return *reinterpret_cast<u32*>(kRegionBase+4*w);}
inline u32 Fake(u32 kind,u32 self) {
    const u32 k=F.calls++;
    const u32 ret=RetVal(F.mode,F.seed,k);
    Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{kind,self,ret};++nev;
    if(k==F.reK) {
        switch(F.reKind) {
        case 1: RegionWord(0)=F.reVal;break;
        case 2: RegionWord(kGenWord+4)=F.reVal;break;
        case 3: RegionWord(100)=F.reVal;break;
        default: break;
        }
    }
    return ret;
}
// thiscall sin argumentos de pila (`ret` simple) = __fastcall con ECX = this
inline u32 __fastcall FkTexTotal(u32 self,void*) {return Fake(1,self);}   // 0x7300A0
inline u32 __fastcall FkIbTotal(u32 self,void*) {return Fake(2,self);}    // 0x7303B0

inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i)out.push_back(json::array({evs[i].kind,evs[i].ecx,evs[i].ret}));
    return out;
}

// ---- imagen del bloque de globales de un caso: basura salvo el contador
inline std::array<u32,kRegionWords> snapRegion{};
inline void TakeSnapshot() {memcpy(snapRegion.data(),reinterpret_cast<void*>(kRegionBase),kRegionWords*4);}
inline void RestoreSnapshot() {memcpy(reinterpret_cast<void*>(kRegionBase),snapRegion.data(),kRegionWords*4);}
inline bool SnapshotMatches() {return memcmp(snapRegion.data(),reinterpret_cast<void*>(kRegionBase),kRegionWords*4)==0;}
inline void BuildImage(const Spec& s,unsigned n,std::array<u32,kRegionWords>& img) {
    for(u32 i=0;i<kRegionWords;++i)img[i]=Garbage(n+0x300,i);
    img[0]=s.ntex;
}

inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    std::array<u32,kRegionWords> img{};
    BuildImage(s,n,img);
    Side sides[2];json rows[2];
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        F=FakeState{};F.mode=s.mode;F.seed=s.seed;F.reK=s.reK;F.reKind=s.reKind;F.reVal=s.reVal;nev=0;
        TakeSnapshot();
        memcpy(reinterpret_cast<void*>(kRegionBase),img.data(),kRegionWords*4);
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,s.eax,s.ecx,s.edx,s.stack,0);});
        std::array<u32,kRegionWords> post{};
        memcpy(post.data(),reinterpret_cast<void*>(kRegionBase),kRegionWords*4);
        RestoreSnapshot();
        const bool restored=SnapshotMatches();
        json changes=json::array();
        for(u32 i=0;i<kRegionWords;++i)if(post[i]!=img[i])changes.push_back(json::array({"G",i,post[i]}));
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["changes"]=changes;obs["eax"]=sides[sd].reg.eax;
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=restored&&watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[sd]=out;
    }
    Finish(t,kFn[fn],n,input,rows,sides);
}

inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFns;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    TakeSnapshot();
    const auto initialRegion=snapRegion;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(u32 fn=0;fn<kFns;++fn) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)RunCase(env,totals,fn,c);
            Write({{"event","function_summary"},{"function",kFn[fn]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool regionNow=memcmp(initialRegion.data(),reinterpret_cast<void*>(kRegionBase),kRegionWords*4)==0;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"d3d_state_restored",regionNow}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"d3d_state_restored",regionNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
