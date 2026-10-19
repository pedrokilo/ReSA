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
// Nucleo del banco PartnerFirstSub31: CTaskComplexPartner::CreateFirstSubTask (0x681F20). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego. Plantillas: reversing/PartnerChatAbort30 (Gate, hook Virtual, falsos con parche de entrada en el EXE y en la DLL).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Colaboradores SUSTITUIDOS por el mismo falso en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL):
// CAEPedSpeechAudioEntity::RequestPedConversation (0x4E50E0, cdecl 2 args), CPed::StopPlayingHandSignal (0x5E0480, thiscall) y CTask::operator new (0x61A5A0, cdecl 1 arg; devuelve nulo o un objeto de arena).
// El constructor de CTaskSimpleStandStill (0x62F310) es REAL en ambos lados (ya en C++ y enganchado): el banco comprueba los bytes del objeto. Los falsos registran sus argumentos y el estado de los bytes
// +0x74/+0x75/+0x76 de `this` en el momento de la llamada (orden frente a las escrituras).

namespace PartnerFirstSub31Live {
using namespace PartnerFirstSub31Fix;
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

// ------------------------------------------------------------------------------------------------ memoria de los casos
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};   // CTaskComplexPartnerChat falso (guarda | objeto 0x78 | guarda), se compara ENTERO
alignas(16) inline std::array<u8,kObjArena> objArena{},objRef{};      // objeto que "reserva" el falso de operator new (guarda | 0x20 | guarda), relleno con un byte fijo por caso
inline u8* gThis{};inline u8* gObj{};inline u32 gReq{};inline u32 gAlloc{};

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
enum : u32 {EvReq=1,EvHand=2,EvNew=3};
struct Ev {u32 kind,a,b,c,d,e;};
inline std::array<Ev,16> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c,d,e};++nev;}}
// Falso de CAEPedSpeechAudioEntity::RequestPedConversation (0x4E50E0, cdecl con (pedA, pedB)): registra los argumentos y los bytes +0x74/+0x76/+0x75 de `this` EN ESA LLAMADA; devuelve gReq (solo AL cuenta)
inline u32 __cdecl FakeReq(u32 a,u32 b){Push(EvReq,a,b,gThis[kConvAt],gThis[kLoadedAt],gThis[kF75At]);return gReq;}
// Falso de CPed::StopPlayingHandSignal (0x5E0480, thiscall sin argumentos) = __fastcall(this=ped, edx): registra ped y los bytes +0x74/+0x76
inline void __fastcall FakeHand(void* ped,void*){Push(EvHand,u32(uintptr_t(ped)),gThis[kConvAt],gThis[kLoadedAt]);}
// Falso de CTask::operator new (0x61A5A0, cdecl con size): registra size y devuelve nulo o el objeto de arena
inline void* __cdecl FakeNew(size_t size){Push(EvNew,u32(size),gThis[kConvAt],gThis[kLoadedAt]);return reinterpret_cast<void*>(uintptr_t(gAlloc));}
inline u32 Label(u32 p){return p==0?0u:p==u32(uintptr_t(gObj))?1u:0xBADu;}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;       // las entradas 0x4E50E0 / 0x5E0480 / 0x61A5A0 del EXE (parcheadas solo durante la llamada, con RAII): deben seguir siendo lo que eran
    Pair pReq{},pHand{},pNew{};       // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,6> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los falsos se instalan DESPUES de comprobar la ruta.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        {
            Patches patches;
            patches.Add(env.pReq,&FakeReq);patches.Add(env.pHand,&FakeHand);patches.Add(env.pNew,&FakeNew);
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

// ------------------------------------------------------------------------------------------------ CTaskComplexPartner::CreateFirstSubTask
inline void CaseFirst(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    u8* const self=thisArena.data()+kThisOff;u8* const obj=objArena.data()+kObjOff;
    gThis=self;gObj=obj;gReq=(c.reqHigh<<8)|c.req;gAlloc=c.allocNull?0u:u32(uintptr_t(obj));

    FillWords(thisRef.data(),kThisArena,Rand(n,50));
    std::memset(objRef.data(),int(c.fill),kObjArena);
    u8* const rs=thisRef.data()+kThisOff;
    std::memcpy(rs+kTaskIdAt,&c.taskId,4);std::memcpy(rs+kPartnerAt,&c.partner,4);
    rs[kLeadAt]=u8(c.lead);rs[kConvAt]=u8(c.conv);rs[kF75At]=u8(c.f75);rs[kLoadedAt]=u8(c.loaded);

    const json input={{"family",c.family},{"lead",c.lead},{"task_id",c.taskId},{"conv",c.conv},{"f75",c.f75},{"loaded",c.loaded},{"req",c.req},{"req_high",c.reqHigh},
                      {"alloc_null",c.allocNull},{"fill",c.fill},{"ped",c.ped},{"partner",c.partner},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack},{"this_off",kThisOff},{"obj_off",kObjOff}};
    Side sides[2];json rows[2];
    const u32 args[1]{c.ped};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        thisArena=thisRef;objArena=objRef;nev=0;
        sides[s]=Execute(env,*env.route,0x681F20,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(uintptr_t(self)),c.edx,c.stack,0);});

        json obs;
        obs["ret"]=Label(sides[s].reg.eax);
        json evj=json::array();
        for(unsigned i=0;i<nev;++i)evj.push_back({evs[i].kind,evs[i].a,evs[i].b,evs[i].c,evs[i].d,evs[i].e});
        obs["events"]=evj;
        json changes=json::array();
        for(u32 i=0;i<kThisArena;++i)if(thisArena[i]!=thisRef[i])changes.push_back({int(i)-int(kThisOff),thisArena[i]});
        obs["this_changes"]=changes;
        // objeto reservado: la vtable (4 primeros bytes) solo se anota como "escrita"; el resto, byte a byte frente al relleno; fuera del objeto no debe cambiar nada
        u32 vt{},vt0{};std::memcpy(&vt,obj,4);std::memcpy(&vt0,objRef.data()+kObjOff,4);
        obs["obj_vtable_written"]=vt!=vt0;
        json oc=json::array();
        for(u32 i=4;i<kObjBytes;++i)if(obj[i]!=objRef[kObjOff+i])oc.push_back({i,obj[i]});
        obs["obj_changes"]=oc;
        bool outside=true;
        for(u32 i=0;i<kObjArena;++i)if((i<kObjOff||i>=kObjOff+kObjBytes)&&objArena[i]!=objRef[i])outside=false;
        obs["obj_outside_ok"]=outside;
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CTaskComplexPartner::CreateFirstSubTask",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    env.watches.emplace_back(0x4E50E0,16);env.watches.emplace_back(0x5E0480,16);env.watches.emplace_back(0x61A5A0,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskComplexPartner::CreateFirstSubTask",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe_real",json::array({0x62F310})},{"stubbed",json::array({json{{"exe",env.pReq.exe},{"dll",env.pReq.dll}},json{{"exe",env.pHand.exe},{"dll",env.pHand.dll}},json{{"exe",env.pNew.exe},{"dll",env.pNew.dll}}})}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseFirst(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskComplexPartner::CreateFirstSubTask"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
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
