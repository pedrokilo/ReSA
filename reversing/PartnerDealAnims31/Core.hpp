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
// Nucleo del banco PartnerDealAnims31: CTaskComplexPartnerDeal::StreamRequiredAnims (0x6823C0). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (CTask, CTaskComplexPartnerDeal, CTaskComplexGangLeader, CAnimManager, CStreaming).
// Plantillas: reversing/PartnerChatSeq30 (ABI, hook Virtual, falso con parche de entrada en el EXE y en la DLL) y reversing/CTaskSimpleJetPack / PickWeap1 (colaboradores reconstruidos con el hook encendido).
//
// Banco par a par de UNA funcion, CTaskComplexPartnerDeal::StreamRequiredAnims (0x6823C0, hook virtual: HookRoute con el cuerpo del hook Virtual).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Los CINCO colaboradores estan reconstruidos en C++ (cuatro enganchados; RemoveAnimBlockRef 0x4D3FD0 NO tiene hook: su entrada del EXE es codigo original); se SUSTITUYEN por falsos identicos en
// ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL, resuelto ANTES de escribir ningun parche, con su hook encendido): ShouldLoadGangAnims 0x65E7F0, GetAnimationBlockIndex(const char*)
// 0x4D3990, CStreaming::RequestModel 0x4087E0, AddAnimBlockRef 0x4D3FB0 y RemoveAnimBlockRef 0x4D3FD0. Cada falso registra sus argumentos EN ORDEN. El byte IsLoaded del bloque que devuelve el falso de indice se
// escribe con el valor del caso (se guarda y restaura la entrada completa de 0x20 bytes de ms_aAnimBlocks).

namespace PartnerDealAnims31Live {
using namespace PartnerDealAnims31Fix;
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
// (unico llamador: CTaskComplexPartner::ControlSubTask, `call [edx+2Ch]` en 0x684166, que sobrescribe ECX con `mov ecx,0C15448h` y no lee EDX), por defecto solo se registran.
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada (esta funcion no instala marco SEH; se comprueba igualmente).
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Igual que PartnerChatSeq30::Gate (la funcion es thiscall sin parametros y `ret`, void): siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; captura los registros y EFLAGS justo tras el `call`.
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

// ------------------------------------------------------------------------------------------------ memoria de los casos
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};    // CTaskComplexPartnerDeal falso (se compara ENTERO: la funcion solo puede cambiar +0x6E)

// Etiquetas para punteros: solo `this` cambia de direccion entre ejecuciones
inline uintptr_t gSelf{};
inline json Name(uintptr_t v) {
    if(v>=gSelf&&v<gSelf+kThisBytes)return "T+"+std::to_string(v-gSelf);
    return uint32(v);
}

// ---- falsos de los colaboradores (cdecl). Registro ordenado de eventos: 1 ShouldLoad, 2 indice(nombre), 3 RequestModel(modelo, flags), 4 AddAnimBlockRef(idx), 5 RemoveAnimBlockRef(idx)
struct Ev {u32 kind,a,b;};
inline std::array<Ev,kMaxEvents> evs{};inline unsigned nev{};inline u32 gShould{},gIdx{};
inline void Push(u32 k,u32 a,u32 b){if(nev<evs.size())evs[nev]=Ev{k,a,b};++nev;}
inline u32 __cdecl FakeShould(){Push(1,0,0);return gShould;}
inline int32 __cdecl FakeIndex(const char* name){Push(2,u32(uintptr_t(name)),0);return int32(gIdx);}
inline void __cdecl FakeRequest(int32 model,int32 flags){Push(3,u32(model),u32(flags));}
inline void __cdecl FakeAdd(int32 idx){Push(4,u32(idx),0);}
inline void __cdecl FakeRemove(int32 idx){Push(5,u32(idx),0);}

struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: la funcion de la DLL no tiene RH_ScopedInstall (la entrada del EXE sigue siendo codigo original)
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
inline constexpr unsigned kStubs=5;
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;   // los cinco colaboradores: deben seguir siendo el codigo que eran al empezar
    std::array<Pair,kStubs> stub{};   // 0 ShouldLoad, 1 indice, 2 RequestModel, 3 Add, 4 Remove (cuerpo de la DLL resuelto ANTES de parchear)
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,2*kStubs> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). En ambos lados los cinco colaboradores deben ser saltos a la DLL (hooks encendidos);
// los falsos se instalan DESPUES de comprobar rutas y colaboradores.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;for(const auto& s:env.stub)collab&=(s.hooked?(ReversingProbe::JumpsToDll(s.exe)&&ModuleOf(Resolve(s.exe))==env.asi):!ReversingProbe::JumpsToDll(s.exe))&&s.dll!=0&&ModuleOf(s.dll)==env.asi;
        side.collabOK=collab;
        {
            Patches patches;
            patches.Add(env.stub[0],&FakeShould);
            patches.Add(env.stub[1],&FakeIndex);
            patches.Add(env.stub[2],&FakeRequest);
            patches.Add(env.stub[3],&FakeAdd);
            patches.Add(env.stub[4],&FakeRemove);
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
inline json Finish(Env& env,Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}

// ------------------------------------------------------------------------------------------------ CTaskComplexPartnerDeal::StreamRequiredAnims
inline uint32 NumBlocks() {return *reinterpret_cast<const uint32*>(uintptr_t(kNumBlocksAt));}
inline uint8* BlockEntry(u32 idx) {return reinterpret_cast<uint8*>(uintptr_t(kBlocksBase)+uintptr_t(idx)*kBlockSize);}

inline void CaseStream(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const u32 nb=NumBlocks();
    const u32 idx=IdxFor(c,nb);
    u8* const self=thisArena.data()+kThisOff;
    gSelf=uintptr_t(self);
    FillWords(thisRef.data(),kThisArena,Rand(n,50));
    thisRef[kThisOff+kFlagAt]=u8(c.flag);
    gShould=c.shouldHi|c.should;gIdx=idx;
    // entrada del bloque: se guarda completa y se restaura; solo cambia IsLoaded
    std::array<u8,kBlockSize> savedBlock{};std::memcpy(savedBlock.data(),BlockEntry(idx),kBlockSize);
    auto blockWith=savedBlock;blockWith[kLoadedAt]=u8(c.loaded);
    const json input={{"family",c.family},{"flag",c.flag},{"should",c.should},{"should_hi",c.shouldHi},{"idx_kind",c.idxKind},{"idx",idx},{"num_blocks",nb},{"loaded",c.loaded},
        {"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack},{"this_off",kThisOff}};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        thisArena=thisRef;nev=0;
        std::memcpy(BlockEntry(idx),blockWith.data(),kBlockSize);
        sides[s]=Execute(env,*env.route,0x6823C0,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(uintptr_t(self)),c.edx,c.stack,0);});
        const bool blockIntact=std::memcmp(BlockEntry(idx),blockWith.data(),kBlockSize)==0;
        std::memcpy(BlockEntry(idx),savedBlock.data(),kBlockSize);   // restaura antes de cualquier otra cosa

        // ---- observacion
        json obs;
        json ej=json::array();
        for(unsigned i=0;i<nev&&i<kMaxEvents;++i) {
            const Ev& e=evs[i];
            if(e.kind==1)ej.push_back(json::array({"should"}));
            else if(e.kind==2)ej.push_back(json::array({"index",Name(e.a)}));
            else if(e.kind==3)ej.push_back(json::array({"request",e.a,e.b}));
            else if(e.kind==4)ej.push_back(json::array({"add_ref",e.a}));
            else ej.push_back(json::array({"remove_ref",e.a}));
        }
        obs["events"]=ej;obs["nevents"]=nev;
        obs["block_intact"]=blockIntact;
        json thisChanges=json::array();
        for(u32 i=0;i<kThisArena;++i)if(thisArena[i]!=thisRef[i])thisChanges.push_back({int(i)-int(kThisOff),thisArena[i]});
        obs["this_changes"]=thisChanges;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&std::memcmp(BlockEntry(idx),savedBlock.data(),kBlockSize)==0;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CTaskComplexPartnerDeal::StreamRequiredAnims",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    json stubbed=json::array();
    for(const auto& s:env.stub){env.watches.emplace_back(s.exe,16);stubbed.push_back({{"exe",s.exe},{"dll",s.dll},{"hooked",s.hooked}});ready&=s.dll!=0&&ModuleOf(s.dll)==env.asi;}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    const u32 nb=NumBlocks();
    ready&=nb>=4&&nb<=180;   // NUM_ANIM_BLOCKS = 180
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskComplexPartnerDeal::StreamRequiredAnims",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",json::array()},{"stubbed",stubbed}}},{"num_anim_blocks",nb},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseStream(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskComplexPartnerDeal::StreamRequiredAnims"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
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
