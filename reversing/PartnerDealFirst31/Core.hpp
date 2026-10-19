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
// Nucleo del banco PartnerDealFirst31: CTaskComplexPartnerDeal::CreateFirstSubTask (0x6823B0). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (CTask, CPed, CTaskComplexPartnerDeal).
// Plantilla: reversing/PartnerChatSeq30 (ABI, hook Virtual, falso con parche de entrada).
//
// Banco par a par de UNA funcion, CTaskComplexPartnerDeal::CreateFirstSubTask (0x6823B0, hook virtual: HookRoute con el cuerpo del hook Virtual). El EXE es un unico `jmp 0x681F20`
// (CTaskComplexPartner::CreateFirstSubTask). Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO el hook de Deal encendido.
// ASUME (API prevista, aun no en HEAD): CTaskComplexPartner::CreateFirstSubTask esta reconstruida en C++ con RH_ScopedVMTInstall(CreateFirstSubTask, 0x681F20) (hook Virtual "V" de la categoria
// CTaskComplexPartner). Esa base se SUSTITUYE por un falso identico en ambos lados (registra (this, ped) y devuelve un valor fijado por el caso), con el hook de la base decidido POR LADO (patron de
// reversing/CTaskSimpleJetPack y PickWeap1): ORIGINAL = hook de la base APAGADO (0x681F20 = codigo del EXE) y el falso se parchea en 0x681F20; CANDIDATO = hook de la base ENCENDIDO (0x681F20 salta a la
// DLL) y el MISMO falso se parchea en el cuerpo real de la DLL de la base (resuelto con el hook ENCENDIDO antes de crear ningun parche). Cada parche se crea DESPUES de fijar el estado del hook de ese lado
// (State() reescribe los 5 bytes de 0x681F20 y los de la entrada de la DLL) y se destruye antes del siguiente cambio de estado.

namespace PartnerDealFirst31Live {
using namespace PartnerDealFirst31Fix;
using HookRoute = OriginalDependencies499::HookRoute;
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(address),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline uintptr_t Resolve(uintptr_t address) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i)address+=5+*reinterpret_cast<int32*>(address+1);return address;}
// Cuerpo real de una funcion de la DLL (salta los thunks ILT que apuntan dentro de la DLL; se detiene ante un salto al EXE).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
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
// (llamadas virtuales +0x24 de la maquinaria de CTaskComplex: ningun sitio de llamada de tareas lee ECX/EDX tras la llamada, comprobado con un desensamblado de todo .text), por defecto solo se registran.
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
// Igual que PartnerChatSeq30::Gate (thiscall con un argumento de pila que limpia el llamado, `ret 4`): siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; captura los registros y EFLAGS justo tras el `call`.
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
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};    // CTaskComplexPartnerDeal falso (se compara ENTERO: la funcion no lo toca)

// Etiquetas para punteros: solo `this` cambia de direccion entre ejecuciones
inline uintptr_t gSelf{};
inline json Name(uintptr_t v) {
    if(v>=gSelf&&v<gSelf+kThisBytes)return "T+"+std::to_string(v-gSelf);
    return uint32(v);
}

// ---- falso de CTaskComplexPartner::CreateFirstSubTask (0x681F20: thiscall, 1 argumento de pila, `ret 4`): registra (this, ped) y devuelve el valor del caso
struct CallEv {u32 self,ped;};
inline std::array<CallEv,4> calls{};inline unsigned ncalls{};inline u32 gRet{};
inline u32 __fastcall FakeBase(void* self,void*,u32 ped){if(ncalls<calls.size())calls[ncalls++]=CallEv{u32(uintptr_t(self)),ped};return gRet;}
inline constexpr uintptr_t kBase=0x681F20;

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};bool patchOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;   // 0x681F20 y la entrada de la DLL de la base, tal como estaban al empezar (estado de produccion): deben recuperarse al final
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
    // Hook Virtual de CTaskComplexPartner::CreateFirstSubTask (0x681F20): estado inicial, entrada de la DLL (GetHookOurAddress), cuerpo real resuelto con el hook ENCENDIDO y bytes de referencia por lado
    ReversibleHooks::ReversibleHook::Base* partner{};
    bool partnerEnabled{},partnerLocked{},partnerRestored{};
    uintptr_t partnerOur{},partnerBody{};
    std::vector<uint8> exeBytes,bodyBytes;   // 16 bytes de 0x681F20 con el hook apagado / del cuerpo con el hook encendido, antes de parchear
    bool RestorePartner(){
        if(!partner)return partnerRestored=true;
        partner->State(partnerEnabled);partner->LockState(partnerLocked);
        return partnerRestored=partner->Hooked()==partnerEnabled&&partner->Locked()==partnerLocked;
    }
};
inline bool JumpsToExe(uintptr_t at){
    if(*reinterpret_cast<const uint8*>(at)!=0xE9)return false;
    const uintptr_t target=at+5+*reinterpret_cast<const int32*>(at+1);
    return target>=0x400000&&target<0x1000000;
}
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook de Deal apagado; candidato = SOLO el hook de Deal encendido (cuerpo en el modulo ASI). El hook de la BASE (0x681F20) se fija por lado: original apagado (0x681F20 = EXE, no salta
// a la DLL), candidato encendido (0x681F20 salta a la entrada de la DLL de la base y el cuerpo real esta en el ASI). El falso se instala DESPUES de fijar el estado y de comprobar rutas/bytes de referencia,
// en 0x681F20 (original) o en el cuerpo de la DLL (candidato); el mismo FakeBase en los dos.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        env.partner->State(candidate);   // los parches se crean DESPUES de este cambio (State reescribe 0x681F20 y la entrada de la DLL)
        const uintptr_t at=candidate?env.partnerBody:kBase;
        const auto before=ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        if(candidate) {
            const uintptr_t hop=kBase+5+*reinterpret_cast<const int32*>(kBase+1);
            side.collabOK=env.partner->Hooked()&&ReversingProbe::JumpsToDll(kBase)&&hop==env.partnerOur&&ModuleOf(Resolve(kBase))==env.asi
                &&ModuleOf(env.partnerBody)==env.asi&&!JumpsToExe(env.partnerBody)&&before==env.bodyBytes;
        } else {
            side.collabOK=!env.partner->Hooked()&&!ReversingProbe::JumpsToDll(kBase)&&ModuleOf(Resolve(kBase))!=env.asi&&before==env.exeBytes;
        }
        {
            std::optional<ReversingProbe::EntryPatch> patch;
            patch.emplace(at,&FakeBase);
            side.reg=gate(entry);
        }
        side.patchOK=before==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
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

// ------------------------------------------------------------------------------------------------ CTaskComplexPartnerDeal::CreateFirstSubTask
inline void CaseFirst(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    u8* const self=thisArena.data()+kThisOff;
    gSelf=uintptr_t(self);
    FillWords(thisRef.data(),kThisArena,Rand(n,50));
    const u32 pedArg=c.pedKind==1?u32(gSelf):c.ped;
    gRet=c.retKind==1?u32(gSelf):c.retKind==2?pedArg:c.ret;
    const json input={{"family",c.family},{"ped_kind",c.pedKind},{"ped",c.ped},{"ret_kind",c.retKind},{"ret",c.ret},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack}};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        thisArena=thisRef;ncalls=0;
        const u32 args[1]{pedArg};
        sides[s]=Execute(env,*env.route,0x6823B0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(uintptr_t(self)),c.edx,c.stack,0);});

        // ---- observacion
        json obs;
        obs["ret"]=Name(sides[s].reg.eax);
        json cj=json::array();for(unsigned i=0;i<ncalls;++i)cj.push_back({Name(calls[i].self),Name(calls[i].ped)});
        obs["calls"]=cj;obs["ncalls"]=ncalls;
        json thisChanges=json::array();
        for(u32 i=0;i<kThisArena;++i)if(thisArena[i]!=thisRef[i])thisChanges.push_back({int(i)-int(kThisOff),thisArena[i]});
        obs["this_changes"]=thisChanges;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=sides[s].patchOK;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CTaskComplexPartnerDeal::CreateFirstSubTask",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    // ---- hook Virtual de la base (0x681F20): estado inicial -> vigilancia -> cuerpo con el hook ENCENDIDO -> bytes de referencia con el hook APAGADO (antes de crear ningun parche)
    ready&=env.partner&&env.partnerOur;
    env.watches.emplace_back(kBase,16);
    if(env.partnerOur)env.watches.emplace_back(env.partnerOur,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    if(env.partner) {
        env.partnerEnabled=env.partner->Hooked();env.partnerLocked=env.partner->Locked();env.partner->LockState(false);
        env.partner->State(true);
        env.partnerBody=Body(env.partnerOur);env.bodyBytes=ReversingProbe::Bytes(reinterpret_cast<void*>(env.partnerBody),16);
        ready&=env.partner->Hooked()&&ReversingProbe::JumpsToDll(kBase)&&env.partnerBody>=0x1000000&&ModuleOf(env.partnerBody)==env.asi&&!JumpsToExe(env.partnerBody);
        env.partner->State(false);
        env.exeBytes=ReversingProbe::Bytes(reinterpret_cast<void*>(kBase),16);
        ready&=!env.partner->Hooked()&&!ReversingProbe::JumpsToDll(kBase)&&ModuleOf(Resolve(kBase))!=env.asi;
    }
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskComplexPartnerDeal::CreateFirstSubTask",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",json::array()},{"stubbed",json::array({json{{"exe",kBase},{"dll",env.partnerBody}}})}}},
        {"partner_hook",{{"registered",env.partner!=nullptr},{"our_address",env.partnerOur},{"body",env.partnerBody},{"enabled_initially",env.partnerEnabled},{"locked_initially",env.partnerLocked}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseFirst(env,totals,c);
        env.RestorePartner();   // antes del resumen: el hook de la base vuelve a su estado inicial y se comprueba la vigilancia de 0x681F20 / entrada de la DLL
        Write({{"event","function_summary"},{"function","CTaskComplexPartnerDeal::CreateFirstSubTask"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    stateRestored&=env.RestorePartner()&&env.WatchesIntact();
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
