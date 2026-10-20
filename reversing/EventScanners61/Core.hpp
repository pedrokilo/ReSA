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
// Nucleo del banco EventScanners61. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego que use cada caso. Plantillas: reversing/PartnerControl60 (ABI, falsos con parche de entrada en el EXE y en la DLL).
//
// Banco par a par de CINCO funciones, cada una en su ruta (HookRoute) y con sus colaboradores sustituidos por falsos identicos en ambos lados:
//   original  = bytes del EXE con el hook RH de la ruta apagado;  candidato = DLL con SOLO ese hook encendido.
// Los colaboradores reconstruidos (con hook en HEAD) se parchean en la direccion del EXE Y en el cuerpo de la DLL (resuelto ANTES de escribir ningun parche);
// los no reconstruidos solo en la direccion del EXE. Cada falso registra sus argumentos EN ORDEN (evs).

namespace EventScanners61Live {
using namespace EventScanners61Fix;
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
// Las funciones usan x87: el estado x87 se compara normalizado (FCW completa, TOP, etiquetas y ST validos).
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md): solo se registran en original_residue/candidate_residue.
// (el valor de retorno bool de TestPedStuck, AL, se compara siempre como `ret`).
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada.
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`.
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

// ------------------------------------------------------------------------------------------------ etiquetas para punteros (las arenas cambian de direccion entre ejecuciones, no de papel)
struct Region {uintptr_t base;u32 bytes;const char* tag;};
inline std::vector<Region> regions;
inline json Name(uintptr_t v) {
    for(const auto& r:regions)if(v>=r.base&&v<r.base+r.bytes)return std::string(r.tag)+"+"+std::to_string(v-r.base);
    return uint32(v);
}
// Registro ordenado de eventos de los falsos (sin reservas ni JSON dentro de los falsos). `mask`: bit i -> el argumento i es un puntero (se etiqueta con Name).
struct Ev {u32 kind,mask,n;std::array<u32,24> a;};
inline std::array<Ev,kMaxEvents> evs{};inline unsigned nev{};
template<class... A> inline void Push(u32 kind,u32 mask,A... args){
    static_assert(sizeof...(A)<=24);
    if(nev<evs.size()){Ev e{kind,mask,u32(sizeof...(A)),{}};unsigned i=0;((e.a[i++]=u32(args)),...);evs[nev]=e;}
    ++nev;
}
// Cada funcion registra los nombres de sus tipos de evento (indice = kind >= 1). Los argumentos se vuelcan tal cual (bit i de `mask`: el argumento i es un puntero y se etiqueta con Name).
inline std::vector<const char*>& EvKinds() {static std::vector<const char*> v;return v;}
inline u32 DefEv(const char* name) {EvKinds().push_back(name);return u32(EvKinds().size());}
inline json EventJson(const Ev& e) {
    json j=json::array();
    if(e.kind>=1000){j.push_back("trap");j.push_back(e.kind-1000);j.push_back(Name(e.a[0]));return j;}
    if(e.kind==0||e.kind>EvKinds().size()){j.push_back("?");return j;}
    j.push_back(EvKinds()[e.kind-1]);
    for(unsigned i=0;i<e.n;++i)j.push_back((e.mask>>i)&1?Name(e.a[i]):json(e.a[i]));
    return j;
}
inline json EventsJson() {
    json ej=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i)ej.push_back(EventJson(evs[i]));
    return ej;
}

// Ranuras no previstas de las vtables falsas: se registran (el oraculo no las espera) para que un cambio de ranura en la DLL salte a la vista
inline constexpr unsigned kVtSlots=40;
template<unsigned K> u32 __fastcall Trap(void* self,void*){Push(1000+K,1,u32(uintptr_t(self)));return 0;}
using TrapFn=u32(__fastcall*)(void*,void*);
template<unsigned... I> constexpr std::array<TrapFn,sizeof...(I)> MakeTraps(std::integer_sequence<unsigned,I...>){return {&Trap<I>...};}
inline const std::array<TrapFn,kVtSlots> kTraps=MakeTraps(std::make_integer_sequence<unsigned,kVtSlots>{});

// ------------------------------------------------------------------------------------------------ colaboradores sustituidos
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI; con el hook apagado la entrada de la DLL salta al EXE).
inline uintptr_t Body(uintptr_t p,uintptr_t asi){for(int k=0;k<8&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(ModuleOf(t)!=asi)break;p=t;}return p;}
struct Stub {uintptr_t exe{},dll{};bool hooked{true};void* fake{};const char* name{};};   // hooked=false: sin RH_ScopedInstall (la entrada del EXE sigue siendo codigo original); dll=0: no hay cuerpo en la DLL

// Ids de los colaboradores sustituidos (indice en Env::stubs)
enum StubId : unsigned {S_ADD=0,S_SIMPLEST,S_ISGOTO,S_COLMODEL,S_LOS,S_MOVE,S_REGREF,S_CLEANREF,S_VPCSCAN,S_X606580,S_OBJSCAN,S_ACQSCAN,S_ATTRSCAN,S_FIRESCAN,S_SEXYSCAN,S_MENTAL,S_ISINAIR,S_GETCUR,S_ISPLAYER,S_FINDTASK,S_GENDMG,S_FINDPLAYER,S_ISALIVE,S_HASEVENT,S_SWIM,S_JET,S_CLIMB,S_NORM,S_PVL,S_LOSV,S_SETPOSN,S_FORCE,S_ORIENT,S_ANGLE,S_LIMIT,S_GETEV,S_COUNT};
// CEventGroup::Add(CEvent*, bool) 0x4AB420 (thiscall, 2 argumentos, `ret 8`): registra el tipo/prioridad/estado del evento que recibe (llamadas virtuales sobre el propio evento,
// que en el EXE es un objeto de pila con la vtable del EXE y en la DLL uno con la vtable de la DLL) y el argumento bool (solo su byte bajo).
// El volcado depende del tipo de evento: [grupo*, tipo, prioridad, tiempo activo, valido, editable, anadir al grupo, tarea, expresion facial, arg bool, extras del tipo...]
// (los campos de CEventEditableResponse solo se leen si el evento es editable: HasEditableResponse, ranura 9).
// Extras: AreaCodes: +0xC (ped*); PedEnteredMyVehicle: +0x14*, +0x18*, +0x1C; PotentialWalkIntoVehicle: +0x14*, +0x18; Damage: +0x14*, +0x18, +0x1C, +0x20, +0x24, +0x25&0x1F, +0x28..+0x3C, +0x40.
inline const u32 kEvAdd=DefEv("add");
inline const u32 kEvHas=DefEv("has_event");
inline u32 DumpEvent(u32 kind,const void* group,void* ev,u32 argBool,bool isAdd){
    using Fn=int32(__thiscall*)(void*);
    void** vt=*reinterpret_cast<void***>(ev);
    const int32 type=reinterpret_cast<Fn>(vt[1])(ev);       // GetEventType
    const int32 priority=type==EVENT_STUCK_IN_AIR?0:reinterpret_cast<Fn>(vt[2])(ev);   // GetEventPriority (CEventStuckInAir lo calcula leyendo la inteligencia del peaton: no se llama)
    const bool editable=(reinterpret_cast<Fn>(vt[9])(ev)&0xFF)!=0;   // HasEditableResponse
    const u8* e=static_cast<const u8*>(ev);
    int32 timeActive{};memcpy(&timeActive,e+4,4);
    i16 taskId{},facial{};u32 addToGroup{};
    if(editable){memcpy(&taskId,e+0xE,2);memcpy(&facial,e+0x10,2);addToGroup=e[0xC];}
    std::array<u32,24> a{};unsigned n=0;u32 mask=0;(void)isAdd;
    const auto put=[&](u32 v,bool label=false){if(label)mask|=1u<<n;a[n++]=v;};
    const auto word=[&](unsigned off){u32 w{};memcpy(&w,e+off,4);return w;};
    put(u32(uintptr_t(group)),true);
    put(u32(type));put(u32(priority));put(u32(timeActive));put(e[8]);put(editable?1u:0u);put(addToGroup);put(u32(u16(taskId)));put(u32(u16(facial)));put(argBool&0xFFu);
    switch(type){
    case EVENT_AREA_CODES: put(word(0xC),true);break;
    case EVENT_STUCK_IN_AIR: put(word(0xC),true);break;
    case EVENT_PED_ENTERED_MY_VEHICLE: put(word(0x14),true);put(word(0x18),true);put(word(0x1C));break;
    case EVENT_POTENTIAL_WALK_INTO_VEHICLE: put(word(0x14),true);put(word(0x18));break;
    case EVENT_DAMAGE:
        put(word(0x14),true);put(word(0x18));put(word(0x1C));put(word(0x20));put(e[0x24]);put(e[0x25]&0x1Fu);
        for(unsigned o=0x28;o<=0x3C;o+=4)put(word(o));
        put(word(0x40));
        break;
    default: break;
    }
    if(nev<evs.size()){Ev r{kind,mask,n,a};evs[nev]=r;}
    ++nev;
    return u32(type);
}
inline void* __fastcall FakeAdd(void* group,void*,void* ev,u32 valid){DumpEvent(kEvAdd,group,ev,valid,true);return nullptr;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route[8];
    std::vector<Watch> watches;       // los colaboradores sustituidos: deben seguir siendo el codigo que eran al empezar
    std::vector<Stub> stubs;
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> p;
    void Add(const Stub& s){
        if(s.exe)p.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.exe,s.fake));
        if(s.dll&&s.dll!=s.exe)p.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.dll,s.fake));
    }
    void Reset(){while(!p.empty())p.pop_back();}
    ~Patches(){Reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). En ambos lados los colaboradores con hook deben ser saltos a la DLL (y los sin hook, codigo del EXE);
// los falsos se instalan DESPUES de comprobar rutas y colaboradores y se quitan (Reset) antes de volver, para comprobar globals_restored sin parches.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,const std::vector<const Stub*>& stubs,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const Stub* s:stubs)collab&=(s->hooked?(ReversingProbe::JumpsToDll(s->exe)&&ModuleOf(Resolve(s->exe))==env.asi):!ReversingProbe::JumpsToDll(s->exe))&&(s->dll==0||ModuleOf(s->dll)==env.asi);
        side.collabOK=collab;
        {
            Patches patches;
            for(const Stub* s:stubs)patches.Add(*s);
            side.reg=gate(entry);
            SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        }
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
// Primer desfase en que a y b difieren dentro de [0,n); -1 si no hay.
inline long FirstDiff(const u8* a,const u8* b,size_t n){for(size_t i=0;i<n;++i)if(a[i]!=b[i])return long(i);return -1;}
// Cambios (desfase relativo al objeto, byte nuevo) de un objeto dentro de su arena.
template<size_t N> json Changes(const std::array<u8,N>& now,const std::array<u8,N>& ref,u32 objOff,u32 objBytes){
    json ch=json::array();
    for(u32 i=objOff;i<objOff+objBytes;++i)if(now[i]!=ref[i])ch.push_back({int(i)-int(objOff),now[i]});
    return ch;
}
template<size_t N> bool GuardsOk(const std::array<u8,N>& now,const std::array<u8,N>& ref,u32 objOff,u32 objBytes){
    return memcmp(now.data(),ref.data(),objOff)==0&&memcmp(now.data()+objOff+objBytes,ref.data()+objOff+objBytes,N-objOff-objBytes)==0;
}

#include "CaseMental.hpp"
#include "CaseVpc.hpp"
#include "CaseScan.hpp"
#include "CaseStuck.hpp"

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline json ArenaConstants(){json j=json::object();for(const auto& [k,v]:ArenaTable())j[k]=v;return j;}
struct FnDesc {const char* name;unsigned cases;void(*run)(Env&,Totals&,unsigned);unsigned bit;};
inline const std::vector<FnDesc>& Table() {
    static const std::vector<FnDesc> t{
        {"CMentalState::Process",kCasesMental,&CaseMental,kFnMental},
        {"CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents",kCasesVpc,&CaseVpc,kFnVpc},
        {"CEventScanner::ScanForEvents",kCasesScan,&CaseScan,kFnScan},
        {"CPedStuckChecker::TestPedStuck",kCasesStuck,&CaseStuck,kFnStuck},
    };
    return t;
}
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    unsigned pairs=0,functions=0;json cases=json::object();
    for(const auto& f:Table())if(kEnabled&f.bit){pairs+=f.cases;++functions;cases[f.name]=f.cases;}
    json stubbed=json::array();
    for(const auto& s:env.stubs){if(!s.exe)continue;env.watches.emplace_back(s.exe,16);stubbed.push_back({{"name",s.name},{"exe",s.exe},{"dll",s.dll},{"hooked",s.hooked},{"exe_jumps_to_dll",ReversingProbe::JumpsToDll(s.exe)},{"dll_in_asi",s.dll==0||ModuleOf(s.dll)==env.asi}});if(s.dll)ready&=ModuleOf(s.dll)==env.asi;}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    InitScanVtables();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",functions},{"pairs",pairs},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",json::array()},{"stubbed",stubbed}}},{"arenas",ArenaConstants()},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(const auto& f:Table()) {
            if(!(kEnabled&f.bit))continue;
            const unsigned before=totals.failures;
            for(unsigned c=0;c<f.cases;++c)f.run(env,totals,c);
            Write({{"event","function_summary"},{"function",f.name},{"cases",f.cases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
