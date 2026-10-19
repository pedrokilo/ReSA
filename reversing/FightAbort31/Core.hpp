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
// Nucleo del banco FightAbort31: CTaskSimpleFight::MakeAbortable (0x6239F0, hook VIRTUAL, ranura 6 de la vtable 0x86D684). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32,
// ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}, OriginalDependencies499::HookRoute y los tipos del juego. Plantillas: reversing/PartnerChatAbort30 (Gate, hook Virtual, estructura).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Todo lo que la funcion toca es FALSO y comun a ambos lados: la tarea (`this`), las dos asociaciones
// de animacion, el ped, los datos del jugador y el evento (objeto con vtable falsa de 16 ranuras: solo la 2, GetEventPriority, es la correcta); los colaboradores
// CAnimBlendAssociation::SetDeleteCallback (0x4CEBC0), CAnimManager::BlendAnimation (0x4D4610), CPed::IsPlayer (0x5DF8F0) y CTaskSimpleFight::FindTargetOnGround (0x61C9B0) se sustituyen por el
// mismo falso en ambos lados (parche de entrada en el EXE y, si existe, en el cuerpo de la DLL resuelto antes de parchear). Los falsos registran sus argumentos en orden.

namespace FightAbort31Live {
using namespace FightAbort31Fix;
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
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};   // CTaskSimpleFight falso (guarda | 0x28 | guarda), se compara ENTERO
alignas(16) inline std::array<u8,kAnimArena> animArena{},animRef{};   // asociacion de golpe (m_pAnim)
alignas(16) inline std::array<u8,kAnimArena> idleArena{},idleRef{};   // asociacion de reposo (m_pIdleAnim)
alignas(16) inline std::array<u8,kPedArena> pedArena{},pedRef{};      // ventana del ped (hasta +0x4D8)
alignas(16) inline std::array<u8,kPdArena> pdArena{},pdRef{};         // datos del jugador
alignas(16) inline std::array<u8,kEvArena> evArena{},evRef{};         // evento falso (vtable en +0)
alignas(16) inline std::array<uintptr_t,kSlots> fakeVtbl{};           // vtable falsa del evento: cada entrada es un falso distinto
inline u8 *gThis{},*gAnim{},*gIdle{},*gPed{},*gPd{},*gEv{};
inline u32 gType{},gIsPlayer{},gRet{};

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
enum : u32 {EvType=1,EvSetDel=2,EvBlend=3,EvIsPlayer=4,EvFind=5};
struct Ev {u32 kind,a,b,c,d,e;};
inline std::array<Ev,32> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c,d,e};++nev;}}
// etiquetas de los objetos (el oraculo no conoce direcciones): 0 nulo, 1 tarea, 2 anim, 3 reposo, 4 ped, 5 evento, 6 datos del jugador, 0xBAD otro
inline u32 Label(const void* p){return !p?0u:p==gThis?1u:p==gAnim?2u:p==gIdle?3u:p==gPed?4u:p==gEv?5u:p==gPd?6u:0xBADu;}
// Vtable falsa del evento: thiscall sin argumentos de pila (`ret`) = __fastcall (this, edx). Una funcion distinta por entrada.
template<u32 K> u32 __fastcall FakeEvSlot(void* self,void*){Push(EvType,K,Label(self));return K==kGetEventPrioritySlot?gType:gRet;}
template<u32... Ks> inline void FillVtbl(std::integer_sequence<u32,Ks...>){((fakeVtbl[Ks]=reinterpret_cast<uintptr_t>(&FakeEvSlot<Ks>)),...);}
// CAnimBlendAssociation::SetDeleteCallback (0x4CEBC0, thiscall ret 8): registra (asociacion, callback, dato) tal como se pasan; no escribe nada.
inline u32 __fastcall FakeSetDelete(void* assoc,void*,u32 cb,u32 data){Push(EvSetDel,Label(assoc),cb,data);return gRet;}
// CAnimManager::BlendAnimation (0x4D4610, cdecl, 4 argumentos de 32 bits; el ultimo es un float): registra los bits tal como se pasan.
inline u32 __cdecl FakeBlend(u32 clump,u32 group,u32 anim,u32 delta){Push(EvBlend,clump,group,anim,delta);return gRet;}
// CPed::IsPlayer (0x5DF8F0, thiscall sin pila): AL = booleano del caso, los 24 bits altos de EAX son residuo.
inline u32 __fastcall FakeIsPlayer(void* ped,void*){Push(EvIsPlayer,Label(ped));return (gRet&~0xFFu)|gIsPlayer;}
// CTaskSimpleFight::FindTargetOnGround (0x61C9B0, thiscall ret 4): registra (tarea, ped).
inline u32 __fastcall FakeFind(void* task,void*,u32 ped){Push(EvFind,Label(task),Label(reinterpret_cast<void*>(uintptr_t(ped))));return gRet;}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;       // las entradas del EXE (parcheadas solo durante la llamada, con RAII): deben seguir siendo lo que eran
    Pair pSetDel{},pBlend{},pIsPlayer{},pFind{};   // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,10> p;int n{};
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
            patches.Add(env.pSetDel,&FakeSetDelete);
            patches.Add(env.pBlend,&FakeBlend);
            patches.Add(env.pIsPlayer,&FakeIsPlayer);
            patches.Add(env.pFind,&FakeFind);
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

// Cambios de un arena respecto a su imagen previa: [arena, desplazamiento respecto al objeto (negativo = guarda delantera), byte nuevo]
template<size_t N> inline void Diff(json& out,unsigned arena,const std::array<u8,N>& now,const std::array<u8,N>& ref,u32 off){
    for(u32 i=0;i<N;++i)if(now[i]!=ref[i])out.push_back({arena,int(i)-int(off),now[i]});
}

// ------------------------------------------------------------------------------------------------ CTaskSimpleFight::MakeAbortable
inline void CaseAbort(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    u8* const self=thisArena.data()+kThisOff;u8* const anim=animArena.data()+kAnimOff;u8* const idle=idleArena.data()+kAnimOff;u8* const ped=pedArena.data()+kPedOff;
    u8* const pd=pdArena.data()+kPdOff;u8* const ev=evArena.data()+kEvOff;
    gThis=self;gAnim=anim;gIdle=idle;gPed=c.pedKind?ped:nullptr;gPd=pd;gEv=c.hasEvent?ev:nullptr;gType=c.evType;gIsPlayer=c.pedKind==2;gRet=c.ret;
    const u32 pedClump=Rand(n,60),pedGroup=Rand(n,61);

    FillWords(thisRef.data(),kThisArena,Rand(n,50));FillWords(animRef.data(),kAnimArena,Rand(n,51));FillWords(idleRef.data(),kAnimArena,Rand(n,52));
    FillWords(pedRef.data(),kPedArena,Rand(n,53));FillWords(pdRef.data(),kPdArena,Rand(n,54));FillWords(evRef.data(),kEvArena,Rand(n,55));
    const u32 animPtr=c.hasAnim?u32(uintptr_t(anim)):0u,idlePtr=c.hasIdle?u32(uintptr_t(idle)):0u,pdPtr=u32(uintptr_t(pd));
    std::memcpy(thisRef.data()+kThisOff+kAnimAt,&animPtr,4);std::memcpy(thisRef.data()+kThisOff+kIdleAt,&idlePtr,4);
    thisRef[kThisOff+kFinishedAt]=u8(c.finished0);thisRef[kThisOff+kNextAt]=u8(c.next0);
    std::memcpy(animRef.data()+kAnimOff+kAnimDeltaAt,&c.animDelta,4);
    std::memcpy(idleRef.data()+kAnimOff+kAnimWeightAt,&c.idleWeight,4);std::memcpy(idleRef.data()+kAnimOff+kAnimDeltaAt,&c.idleDelta,4);
    std::memcpy(pedRef.data()+kPedOff+kPedClumpAt,&pedClump,4);std::memcpy(pedRef.data()+kPedOff+kPedGroupAt,&pedGroup,4);
    if(c.pedKind==2)std::memcpy(pedRef.data()+kPedOff+kPedPdAt,&pdPtr,4);
    std::memcpy(pdRef.data()+kPdOff+kPdFightAt,&c.pdX,4);std::memcpy(pdRef.data()+kPdOff+kPdFightAt+4,&c.pdY,4);
    const uintptr_t vt=uintptr_t(fakeVtbl.data());
    std::memcpy(evRef.data()+kEvOff,&vt,4);

    const json input={{"family",c.family},{"prio",c.prio},{"has_event",c.hasEvent},{"ev_type",c.evType},{"has_anim",c.hasAnim},{"has_idle",c.hasIdle},{"ped_kind",c.pedKind},
                      {"anim_delta",c.animDelta},{"idle_weight",c.idleWeight},{"idle_delta",c.idleDelta},{"pd_x",c.pdX},{"pd_y",c.pdY},{"finished0",c.finished0},{"next0",c.next0},
                      {"ret",c.ret},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack},{"anim_ptr",animPtr},{"idle_ptr",idlePtr},{"ped_clump",pedClump},{"ped_group",pedGroup},
                      {"this_off",kThisOff},{"slot",kGetEventPrioritySlot}};
    Side sides[2];json rows[2];
    const u32 args[3]{c.pedKind?u32(uintptr_t(ped)):0u,c.prio,c.hasEvent?u32(uintptr_t(ev)):0u};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        thisArena=thisRef;animArena=animRef;idleArena=idleRef;pedArena=pedRef;pdArena=pdRef;evArena=evRef;nev=0;
        sides[s]=Execute(env,*env.route,0x6239F0,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,u32(uintptr_t(self)),c.edx,c.stack,0);});

        json obs;
        obs["ret"]=sides[s].reg.eax&0xFFu;
        json evj=json::array();
        for(unsigned i=0;i<nev;++i)evj.push_back({evs[i].kind,evs[i].a,evs[i].b,evs[i].c,evs[i].d,evs[i].e});
        obs["events"]=evj;
        json changes=json::array();
        Diff(changes,0,thisArena,thisRef,kThisOff);Diff(changes,1,animArena,animRef,kAnimOff);Diff(changes,2,idleArena,idleRef,kAnimOff);
        Diff(changes,3,pedArena,pedRef,kPedOff);Diff(changes,4,pdArena,pdRef,kPdOff);Diff(changes,5,evArena,evRef,kEvOff);
        obs["changes"]=changes;
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CTaskSimpleFight::MakeAbortable",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    FillVtbl(std::make_integer_sequence<u32,kSlots>{});
    for(uintptr_t a:{uintptr_t(0x4CEBC0),uintptr_t(0x4D4610),uintptr_t(0x5DF8F0),uintptr_t(0x61C9B0)})env.watches.emplace_back(a,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json stubbed=json::array();
    for(const Pair* p:{&env.pSetDel,&env.pBlend,&env.pIsPlayer,&env.pFind})stubbed.push_back({{"exe",p->exe},{"dll",p->dll}});
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskSimpleFight::MakeAbortable",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe_real",json::array()},{"stubbed",stubbed},{"fake_event_slots",kSlots}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseAbort(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskSimpleFight::MakeAbortable"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
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
