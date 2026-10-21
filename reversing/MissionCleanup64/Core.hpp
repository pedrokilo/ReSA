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
// Nucleo del banco MissionCleanup64. Plantilla: reversing/GarageUpdate63 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).

namespace MC64Live {
using namespace MC64Fix;
using HookRoute = OriginalDependencies499::HookRoute;
inline bool done{};
inline unsigned dbgFirst{0}, dbgLast{0xFFFFFFFFu};
inline bool dbgMode{};
inline int dbgFn{-1};
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
// true: tambien se comparan los registros x87 vacios (crudos). Ni el EXE ni la DLL tocan la x87 en estas dos funciones; la sonda deja residuos viejos en ST0..ST7 y deben quedar iguales.
inline constexpr bool kCompareDeadX87Registers=true;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md): solo se registran.
inline constexpr bool kCompareVolatileRegisters=false;

inline std::string HexBytes(const void* p,size_t n) {constexpr char d[]="0123456789abcdef";const auto* b=static_cast<const u8*>(p);std::string o;o.reserve(n*2);for(size_t i=0;i<n;++i){o+=d[b[i]>>4];o+=d[b[i]&15];}return o;}
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline std::array<uint8,108> MaskDeadRegisters(std::array<uint8,108> fp){for(unsigned st=0;st<8;++st)if(FpRegisterIsDead(fp,st))memset(fp.data()+28+10*st,0,10);return fp;}
inline std::array<uint8,108> NormalizeFp(std::array<uint8,108> fp){
    uint16 fsw{};memcpy(&fsw,fp.data()+4,2);fsw&=0x3800;memcpy(fp.data()+4,&fsw,2);   // solo TOP
    memset(fp.data()+2,0,2);memset(fp.data()+6,0,2);memset(fp.data()+10,0,2);          // mitades reservadas
    memset(fp.data()+12,0,16);                                                         // FIP, FCS/FOP, FDP, FDS
    return kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);
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
    Patches(){p.reserve(160);}   // sin reasignaciones mientras haya parches de new/delete activos
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    ~Patches(){while(!p.empty())p.pop_back();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::array<std::unique_ptr<HookRoute>,kFns> route;              // una por funcion
    std::vector<std::unique_ptr<HookRoute>> collab;                 // rutas de los colaboradores con hook (apagados durante el banco)
    std::array<std::vector<StubSpec>,kFns> stubs;                   // colaboradores sustituidos por funcion
    std::vector<Watch> watches;                                     // entradas del EXE de todos los colaboradores: deben seguir siendo lo que eran al empezar
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

template<class Fn> Side Execute(Env& env,unsigned fnIdx,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    HookRoute& route=*env.route[fnIdx];
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const auto& s:env.stubs[fnIdx]) {
            const auto& pr=s.pair;
            collab&=!ReversingProbe::JumpsToDll(pr.exe)&&ModuleOf(Resolve(pr.exe))==env.game&&(pr.dll==0||ModuleOf(pr.dll)==env.asi);
        }
        side.collabOK=collab;
        {
            Patches patches;
            for(const auto& s:env.stubs[fnIdx])patches.Add(s.pair,s.fake);
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

// ================================================================================================ arena, falsos y casos
inline constexpr const char* kFn[kFns]{"CMissionCleanup::Process","CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects"};
inline constexpr const char* kName[kFns]{"Process","CheckIfCollisionHasLoadedForMissionObjects"};
inline constexpr uintptr_t kEntry[kFns]{0x468560,0x4652D0};
inline constexpr unsigned kCases[kFns]{kCasesP,kCasesC};
inline constexpr unsigned kMaxEvents = 700;

struct Arena {
    alignas(16) u32 gl[kGuardWords];
    alignas(16) u8 veh[kSlots][kVehSize];
    alignas(16) u8 ped[kSlots][kPedSize];
    alignas(16) u8 obj[kSlots][kObjSize];
    alignas(16) u32 mat[kEnts][16];
    alignas(16) u8 flags[3][8];
    alignas(16) u32 pool[3][4];          // [0] = base de las entidades, [1] = bytes de banderas
    alignas(16) u8 clean[0x25C];         // la lista de la limpieza (this)
    alignas(16) u32 fped[0x200];         // jugador falso (0x800 bytes)
    alignas(16) u32 intel[16];           // +4 = gestor de tareas
    alignas(16) u32 pd[32];              // datos del jugador (+0 = CWanted*, +0x34 banderas, +0x38 grupo, +0x40 palabra con el byte 0x43)
    alignas(16) u32 wanted[16];
    alignas(16) u32 task[8];
    alignas(16) u32 vt[8];
    alignas(16) u32 gr[kGuardWords];
};
alignas(16) inline Arena W{},WRef{};
inline u32 Self(const void* p){return u32(uintptr_t(p));}
inline bool Within(const void* base,size_t bytes,u32 v,u32& off){const u32 b=Self(base);if(v>=b&&v<b+bytes){off=v-b;return true;}return false;}

inline json Tok(u32 v) {
    char buf[48];
    u32 off{};
    if(v==0)return 0;
    auto sym=[&](const char* nm,u32 idx,bool indexed,u32 o){if(indexed)snprintf(buf,sizeof(buf),o?"%s%u+%u":"%s%u",nm,idx,o);else snprintf(buf,sizeof(buf),o?"%s+%u":"%s",nm,o);return json(std::string(buf));};
    if(Within(W.veh,sizeof(W.veh),v,off)){const u32 i=off/kVehSize,r=off%kVehSize;snprintf(buf,sizeof(buf),r?"V%u+%u":"V%u",i,r);return std::string(buf);}
    if(Within(W.ped,sizeof(W.ped),v,off)){const u32 i=off/kPedSize,r=off%kPedSize;snprintf(buf,sizeof(buf),r?"P%u+%u":"P%u",i,r);return std::string(buf);}
    if(Within(W.obj,sizeof(W.obj),v,off)){const u32 i=off/kObjSize,r=off%kObjSize;snprintf(buf,sizeof(buf),r?"O%u+%u":"O%u",i,r);return std::string(buf);}
    if(Within(W.mat,sizeof(W.mat),v,off)){const u32 i=off/64,r=off%64;snprintf(buf,sizeof(buf),r?"M%u+%u":"M%u",i,r);return std::string(buf);}
    if(Within(W.clean,sizeof(W.clean),v,off))return sym("CLEAN",0,false,off);
    if(Within(W.fped,sizeof(W.fped),v,off))return sym("PED",0,false,off);
    if(Within(W.intel,sizeof(W.intel),v,off))return sym("INTEL",0,false,off);
    if(Within(W.pd,sizeof(W.pd),v,off))return sym("PD",0,false,off);
    if(Within(W.wanted,sizeof(W.wanted),v,off))return sym("W",0,false,off);
    if(Within(W.task,sizeof(W.task),v,off))return sym("TASK",0,false,off);
    return v;
}

// ---- eventos de los falsos (nada aqui asigna memoria)
struct Ev {u32 kind{};u32 n{};u32 ptrMask{};u32 v[8]{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline u32 tailCount{},tailHash{};
inline void TailAdd(u32 x){tailHash=(tailHash^x)*16777619u;}
inline Ev& PushEv(u32 kind,u32 n,u32 ptrMask){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{};e.kind=kind;e.n=n;e.ptrMask=ptrMask;++nev;return e;}
inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){const Ev& e=evs[i];json row=json::array({e.kind});for(u32 k=0;k<e.n;++k)row.push_back((e.ptrMask>>k)&1u?Tok(e.v[k]):json(e.v[k]));out.push_back(row);}
    return out;
}
inline Spec S;                       // caso en curso
inline u32 cnt[kKinds + 4]{};
inline u32 NextK(unsigned kind){return cnt[kind]++;}
inline u32 Junk(u32 bit){return (bit&1u)|0xA5A5A500u;}   // AL = bit, resto de EAX basura

#define EV(kind,n,mask) Ev& e=PushEv(kind,n,mask)
// valores de retorno de los falsos (el oraculo los reproduce)
inline u32 PedFlagOf(u32 type){return type*0x9E3779B1u+7u;}
// ---- Process (cdecl: sin sufijo de pila; thiscall: __fastcall con `ret n`)
inline void __fastcall FkFade(u32 self,void*,u32 dur,u32 dir){EV(kFade,3,1);e.v[0]=self;e.v[1]=dur;e.v[2]=dir&0xFFFFu;}                          // 0x50AC20, ret 8
inline void __cdecl FkDisableTrains(u32 a){EV(kDisableTrains,1,0);e.v[0]=a&0xFFu;}                                                           // 0x6F5DB0
inline void __cdecl FkReleaseTrains(){EV(kReleaseTrains,0,0);(void)e;}                                                                       // 0x6F6B60
inline void __cdecl FkAmbientPlanes(u32 a){EV(kAmbientPlanes,1,0);e.v[0]=a&0xFFu;}                                                           // 0x6CCC50
inline void __cdecl FkFillZones(u32 a){EV(kFillZones,1,0);e.v[0]=a&0xFFu;}                                                                   // 0x572440
inline void __cdecl FkClearGangWar(){EV(kClearGangWar,0,0);(void)e;}                                                                         // 0x443FF0
inline void __cdecl FkRespraysOpen(u32 a){EV(kRespraysOpen,1,0);e.v[0]=a&0xFFu;}                                                             // 0x448B30
inline u32 __cdecl FkFpw(u32 a){EV(kFpw,1,0);e.v[0]=a;return Self(W.wanted);}                                                                // 0x56E230
inline void __cdecl FkRemovePickups(){EV(kRemovePickups,0,0);(void)e;}                                                                       // 0x456DE0
inline void __cdecl FkRoadBlocks(){EV(kRoadBlocks,0,0);(void)e;}                                                                             // 0x460EC0
inline void __cdecl FkCopBikes(u32 a){EV(kCopBikes,1,0);e.v[0]=a&0xFFu;}                                                                     // 0x407D10
inline void __fastcall FkReleaseNodes(u32 self,void*){EV(kReleaseNodes,1,0);e.v[0]=self;}                                                    // 0x44DD00
inline void __fastcall FkUnMark(u32 self,void*){EV(kUnMark,1,0);e.v[0]=self;}                                                                // 0x44D400
inline void __fastcall FkTidyNodes(u32 self,void*){EV(kTidyNodes,1,0);e.v[0]=self;}                                                          // 0x44D3B0
inline void __cdecl FkRecordings(){EV(kRecordings,0,0);(void)e;}                                                                             // 0x45A160
inline void __fastcall FkWideOff(u32 self,void*){EV(kWideOff,1,0);e.v[0]=self;}                                                              // 0x50C150
inline void __fastcall FkInitScriptable(u32 self,void*){EV(kInitScriptable,1,0);e.v[0]=self;}                                                // 0x50D2D0
inline void __fastcall FkDucking(u32 self,void*,u32 ped){EV(kDucking,2,2);e.v[0]=self;e.v[1]=ped;}                                           // 0x50CEF0, ret 4
inline void __cdecl FkPostFx(){EV(kPostFx,0,0);(void)e;}                                                                                     // 0x7010F0
inline void __cdecl FkExtraColour(u32 a){EV(kExtraColour,1,0);e.v[0]=a&0xFFu;}                                                               // 0x55FF20
inline void __fastcall FkMissionAudio(u32 self,void*,u32 slot){EV(kMissionAudio,2,0);e.v[0]=self;e.v[1]=slot&0xFFu;}                         // 0x5072F0, ret 4
inline void __cdecl FkWeather(){EV(kWeather,0,0);(void)e;}                                                                                   // 0x72A510
inline void __cdecl FkSpecialChar(u32 slot){EV(kSpecialChar,1,0);e.v[0]=slot;}                                                               // 0x40B490
inline void __cdecl FkSuppressed(){EV(kSuppressed,0,0);(void)e;}                                                                             // 0x46A7C0
inline void __cdecl FkForceRender(u32 a){EV(kForceRender,1,0);e.v[0]=a&0xFFu;}                                                               // 0x722870
inline void __fastcall FkAllocator(u32 self,void*,u32 t){EV(kAllocator,2,0);e.v[0]=self;e.v[1]=t;}                                           // 0x5FBB70, ret 4
inline void __fastcall FkDecisionMaker(u32 self,void*,u32 t){EV(kDecisionMaker,2,0);e.v[0]=self;e.v[1]=t;}                                   // 0x5F7340, ret 4
inline void __fastcall FkAlwaysFollow(u32 self,void*,u32 a){EV(kAlwaysFollow,2,1);e.v[0]=self;e.v[1]=a&0xFFu;}                               // 0x60C7C0, ret 4
inline void __fastcall FkNeverFollow(u32 self,void*,u32 a){EV(kNeverFollow,2,1);e.v[0]=self;e.v[1]=a&0xFFu;}                                 // 0x60C800, ret 4
inline void __fastcall FkReappear(u32 self,void*){EV(kReappear,1,1);e.v[0]=self;}                                                            // 0x60A4B0
inline void __fastcall FkSpeech(u32 self,void*){EV(kSpeech,1,1);e.v[0]=self;}                                                                // 0x5EFF70
inline void __fastcall FkSpeechScript(u32 self,void*){EV(kSpeechScript,1,1);e.v[0]=self;}                                                    // 0x5EFF90
inline u32 __fastcall FkFindTask(u32 self,void*,u32 idx,u32 type){EV(kFindTask,3,1);e.v[0]=self;e.v[1]=idx;e.v[2]=type;return S.hasTask?Self(W.task):0u;}   // 0x6817D0, ret 8
inline void __fastcall FkTaskFunc(u32 self,void*,u32 ped){EV(kTaskFunc,2,3);e.v[0]=self;e.v[1]=ped;}                                         // 0x634A40, ret 4
inline void __cdecl FkClearSkip(u32 a){EV(kClearSkip,1,0);e.v[0]=a&0xFFu;}                                                                   // 0x441560
inline u32 __cdecl FkFpp(u32 a){EV(kFpp,1,0);e.v[0]=a;return Self(W.fped);}                                                                  // 0x56E210
inline void __cdecl FkRespawn(){EV(kRespawn,0,0);(void)e;}                                                                                   // 0x460840
inline void __fastcall FkFireFlags(u32 self,void*){EV(kFireFlags,1,0);e.v[0]=self;}                                                          // 0x5397A0
inline void __fastcall FkStuckInit(u32 self,void*){EV(kStuckInit,1,0);e.v[0]=self;}                                                          // 0x4639E0
inline void __fastcall FkCamRestore(u32 self,void*){EV(kCamRestore,1,0);e.v[0]=self;}                                                        // 0x50B930
inline void __fastcall FkSafe(u32 self,void*,u32 move,u32 radius){EV(kSafe,3,0);e.v[0]=self;e.v[1]=move&0xFFu;e.v[2]=radius;}                // 0x56E870, ret 8
inline void __cdecl FkHelp(u32 text,u32 a,u32 b,u32 c){EV(kHelp,4,0);e.v[0]=text;e.v[1]=a&0xFFu;e.v[2]=b&0xFFu;e.v[3]=c&0xFFu;}              // 0x588BE0
inline void __cdecl FkCleanVeh(u32 p){EV(kCleanVeh,1,1);e.v[0]=p;}                                                                           // 0x486670
inline void __cdecl FkCleanPed(u32 p){EV(kCleanPed,1,1);e.v[0]=p;}                                                                           // 0x486300
inline void __cdecl FkCleanObj(u32 p){EV(kCleanObj,1,1);e.v[0]=p;}                                                                           // 0x4866C0
inline u32 __cdecl FkGetIdx(u32 handle,u32 type){EV(kGetIdx,2,0);e.v[0]=handle;e.v[1]=type&0xFFu;return u32(S.gsi[NextK(kGetIdx)%16]);}     // 0x4839A0
inline void __fastcall FkFxKill(u32 self,void*){EV(kFxKill,1,0);e.v[0]=self;}                                                                // 0x4AA3F0
inline void __cdecl FkRemoveFx(u32 h){EV(kRemoveFx,1,0);e.v[0]=h;}                                                                           // 0x492FD0
inline void __cdecl FkRemoveGroup(u32 i){EV(kRemoveGroup,1,0);e.v[0]=i;}                                                                     // 0x5FB870
inline void __fastcall FkFlush(u32 self,void*){EV(kFlush,1,0);e.v[0]=self;}                                                                  // 0x632C10
inline void __cdecl FkUnloadDm(u32 i){EV(kUnloadDm,1,0);e.v[0]=i;}                                                                           // 0x607A70
inline void __cdecl FkRemoveSearch(u32 h){EV(kRemoveSearch,1,0);e.v[0]=h;}                                                                   // 0x493160
inline void __cdecl FkRemoveCheckpoint(u32 h){EV(kRemoveCheckpoint,1,0);e.v[0]=h;}                                                           // 0x4936C0
inline void __cdecl FkRemoveTxd(){EV(kRemoveTxd,0,0);(void)e;}                                                                               // 0x465A40
inline void __fastcall FkRemoveEntity(u32 self,void*,u32 handle,u32 type){EV(kRemoveEntity,3,1);e.v[0]=self;e.v[1]=handle;e.v[2]=type&0xFFu;}  // 0x4654B0, ret 8
inline u32 __cdecl FkPedFlag(u32 t){++tailCount;TailAdd(kPedFlag);TailAdd(t);return PedFlagOf(t);}                                           // 0x608830
inline void __cdecl FkAcqOne(u32 a,u32 b,u32 c){++tailCount;TailAdd(kAcqOne);TailAdd(a);TailAdd(b);TailAdd(c);}                              // 0x6089F0
inline void __cdecl FkAcqAll(u32 a,u32 b){++tailCount;TailAdd(kAcqAll);TailAdd(a);TailAdd(b);}                                               // 0x608A20
inline u32 __fastcall FkTaskType(u32 self,void*){EV(kTaskType,1,1);e.v[0]=self;return S.taskType;}                                           // CTask::GetTaskType (vtable +0x10)
// ---- CheckIf
inline u32 __cdecl FkHasCol(u32 pos,u32 area){EV(kHasCol,2,1);e.v[0]=pos;e.v[1]=area;return Junk((S.hcl>>(NextK(kHasCol)&31))&1u);}          // 0x410CE0
inline u32 __cdecl FkHaveIpl(u32 pos,u32 area){EV(kHaveIpl,2,1);e.v[0]=pos;e.v[1]=area;return Junk((S.ipl>>(NextK(kHaveIpl)&31))&1u);}        // 0x405600
inline u32 __fastcall FkIsStatic(u32 self,void*){EV(kIsStatic,1,1);e.v[0]=self;return Junk((S.stat>>(NextK(kIsStatic)&31))&1u);}            // 0x4633E0
inline void __fastcall FkMoving(u32 self,void*){EV(kMoving,1,1);e.v[0]=self;}                                                                // 0x542800
inline void __fastcall FkPlaceAuto(u32 self,void*){EV(kPlaceAuto,1,1);e.v[0]=self;}                                                          // 0x6AF420
inline void __fastcall FkPlaceBike(u32 self,void*){EV(kPlaceBike,1,1);e.v[0]=self;}                                                          // 0x6BEEB0

// ---- memoria real (regiones absolutas del EXE) -----------------------------------------------------------------------------------------------------------------------
inline std::vector<u8> ReadRegions(){std::vector<u8> v;for(const auto& r:kRegions){const auto* p=reinterpret_cast<const u8*>(r.addr);v.insert(v.end(),p,p+r.size);}return v;}
inline void WriteRegions(const std::vector<u8>& v){size_t at=0;for(const auto& r:kRegions){memcpy(reinterpret_cast<void*>(r.addr),v.data()+at,r.size);at+=r.size;}}
inline u32 ReadPtr(u32 a){return *reinterpret_cast<u32*>(a);}
inline void WritePtr(u32 a,u32 v){*reinterpret_cast<u32*>(a)=v;}
inline void Poke(u32 a,u32 v,unsigned size){memcpy(reinterpret_cast<void*>(a),&v,size);}
inline void ApplyBackground(unsigned n){for(const auto& r:kRegions)for(u32 j=0;j<r.size;++j)*reinterpret_cast<u8*>(r.addr+j)=u8(BgByte(n,r.addr+j));}
inline void ApplyControls(const Spec& s){
    Poke(0xA4448A,s.c4448A,1);Poke(0xB7CE74,s.mode,1);Poke(0x96AB91,s.g96AB91,1);Poke(0x96AB90,s.g96AB90,1);Poke(0xB72914,s.v72914,4);Poke(0x96A8B0,s.g96A8B0,1);Poke(0xA444A8,s.gA444A8,1);
    Poke(0xB7CD74,s.focus,1);Poke(0xB7CE48+s.focus*0x190,s.money,4);Poke(0x86C6C0,s.f86C6C0,4);
    for(u32 i=0;i<4;++i){Poke(kSeqBase+i*kSeqStride+0x3C,s.seq3C[i],4);Poke(kFxBase+i*8,s.fx[i],4);}
}
inline json RegionChanges(const std::vector<u8>& applied,const std::vector<u8>& after){
    json out=json::array();size_t at=0;
    for(const auto& r:kRegions){for(u32 j=0;j<r.size;++j)if(applied[at+j]!=after[at+j])out.push_back(json::array({r.addr+j,after[at+j]}));at+=r.size;}
    return out;
}

// ---- imagen de un caso
struct Block {const char* name;u32 idx;const void* ptr;size_t bytes;};
inline std::vector<Block> Blocks(const Arena& a){
    std::vector<Block> b;
    for(u32 i=0;i<kSlots;++i)b.push_back({"V",i,a.veh[i],sizeof(a.veh[i])});
    for(u32 i=0;i<kSlots;++i)b.push_back({"P",i,a.ped[i],sizeof(a.ped[i])});
    for(u32 i=0;i<kSlots;++i)b.push_back({"O",i,a.obj[i],sizeof(a.obj[i])});
    for(u32 i=0;i<kEnts;++i)b.push_back({"M",i,a.mat[i],sizeof(a.mat[i])});
    b.push_back({"F",0,a.flags,sizeof(a.flags)});b.push_back({"POOL",0,a.pool,sizeof(a.pool)});b.push_back({"CLEAN",0,a.clean,sizeof(a.clean)});
    b.push_back({"PED",0,a.fped,sizeof(a.fped)});b.push_back({"INTEL",0,a.intel,sizeof(a.intel)});b.push_back({"PD",0,a.pd,sizeof(a.pd)});b.push_back({"W",0,a.wanted,sizeof(a.wanted)});
    b.push_back({"TASK",0,a.task,sizeof(a.task)});b.push_back({"VT",0,a.vt,sizeof(a.vt)});
    return b;
}
inline void BuildCase(const Spec& s,Arena& a) {
    memset(&a,0,sizeof(a));
    for(u32 i=0;i<kGuardWords;++i){a.gl[i]=0xA5A5A5A5u^i;a.gr[i]=0x5A5A5A5Au^i;}
    for(u32 p=0;p<3;++p)for(u32 i=0;i<8;++i)a.flags[p][i]=u8(s.flags[p][i]);
    a.pool[0][0]=Self(W.veh);a.pool[0][1]=Self(W.flags[0]);a.pool[1][0]=Self(W.ped);a.pool[1][1]=Self(W.flags[1]);a.pool[2][0]=Self(W.obj);a.pool[2][1]=Self(W.flags[2]);
    // la lista de la limpieza: {tipo (1 byte), relleno de 3 bytes, handle}; el relleno y el contador no los toca la funcion
    for(u32 i=0;i<kEntries;++i){a.clean[i*8]=u8(s.type[i]);a.clean[i*8+1]=0xA5;a.clean[i*8+2]=0x5A;a.clean[i*8+3]=0xA5;memcpy(a.clean+i*8+4,&s.handle[i],4);}
    a.clean[0x258]=0x33;
    // entidades (CheckIf): +4 posicion, +0x14 matriz, +0x1C banderas, +0x2F area, +0x590 tipo de vehiculo
    for(u32 i=0;i<kEnts;++i){
        u8* b=i<kSlots?a.veh[i]:(i<2*kSlots?a.ped[i-kSlots]:a.obj[i-2*kSlots]);
        const Ent& e=s.ent[i];
        auto w32=[&](u32 off,u32 v){memcpy(b+off,&v,4);};
        for(int k=0;k<3;++k)w32(4+4*k,e.pos[k]);
        w32(0x14,e.hasMat?Self(W.mat[i]):0u);
        w32(0x1C,e.flags);b[0x2F]=u8(e.area);
        if(i<kSlots)w32(0x590,e.vtype);
        for(int k=0;k<3;++k)a.mat[i][12+k]=e.mpos[k];
    }
    // jugador falso
    u8* p=reinterpret_cast<u8*>(a.fped);
    auto pw=[&](u32 off,u32 v){memcpy(p+off,&v,4);};
    pw(0x47C,Self(W.intel));pw(0x480,Self(W.pd));pw(0x530,s.state530);pw(0x734,s.p734);
    a.pd[0]=Self(W.wanted);a.pd[13]=s.pd34;a.pd[14]=s.pdGroup;a.pd[16]=s.pd40;
    a.wanted[5]=s.w14;a.wanted[7]=s.w1C;
    a.task[0]=Self(W.vt);a.vt[4]=Self(&FkTaskType);
}
inline json ChangesJson(const Arena& post,const Arena& ref) {
    json out=json::array();
    const auto bp=Blocks(post),br=Blocks(ref);
    for(size_t k=0;k<bp.size();++k){
        const u32* pa=static_cast<const u32*>(bp[k].ptr);const u32* ra=static_cast<const u32*>(br[k].ptr);
        for(size_t w=0;w<bp[k].bytes/4;++w)if(pa[w]!=ra[w])out.push_back(json::array({bp[k].name,bp[k].idx,w,pa[w]}));
    }
    // X: cualquier otra palabra (guardas, relleno...): no deberia cambiar nunca
    const u32* pa=reinterpret_cast<const u32*>(&post);const u32* ra=reinterpret_cast<const u32*>(&ref);
    for(size_t i=0;i<sizeof(Arena)/4;++i){
        if(pa[i]==ra[i])continue;
        const u32 addr=Self(&pa[i])-Self(&post);
        bool known=false;
        for(const auto& b:bp){const u32 lo=Self(b.ptr)-Self(&post);if(addr>=lo&&addr<lo+b.bytes){known=true;break;}}
        if(!known)out.push_back(json::array({"X",0,u32(i),pa[i]}));
    }
    return out;
}

inline void RunCase(Env& env,Totals& t,unsigned f,unsigned n) {
    const Spec s=MakeSpec(f,n);
    const json input=json::parse(JsonSpec(s));
    BuildCase(s,WRef);
    Side sides[2];json rows[2];
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        S=s;memset(cnt,0,sizeof(cnt));nev=0;tailCount=0;tailHash=2166136261u;
        const u32 swapSave[4]{ReadPtr(kPtrPlayer),ReadPtr(kPtrPedPool),ReadPtr(kPtrVehPool),ReadPtr(kPtrObjPool)};
        const std::vector<u8> before=ReadRegions();
        std::vector<u8> applied=before;
        if(f==0){ApplyBackground(n);ApplyControls(s);applied=ReadRegions();}
        WritePtr(kPtrPlayer,Self(W.fped));WritePtr(kPtrPedPool,Self(W.pool[1]));WritePtr(kPtrVehPool,Self(W.pool[0]));WritePtr(kPtrObjPool,Self(W.pool[2]));
        sides[sd]=Execute(env,f,kEntry[f],candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,s.regs[1],Self(W.clean),s.regs[3],s.regs[0],0);});
        const std::vector<u8> after=ReadRegions();
        WritePtr(kPtrPlayer,swapSave[0]);WritePtr(kPtrPedPool,swapSave[1]);WritePtr(kPtrVehPool,swapSave[2]);WritePtr(kPtrObjPool,swapSave[3]);
        WriteRegions(before);
        const bool restored=ReadRegions()==before&&ReadPtr(kPtrPlayer)==swapSave[0]&&ReadPtr(kPtrPedPool)==swapSave[1]&&ReadPtr(kPtrVehPool)==swapSave[2]&&ReadPtr(kPtrObjPool)==swapSave[3];
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["tail"]=json::array({tailCount,tailHash});obs["changes"]=ChangesJson(W,WRef);obs["real"]=RegionChanges(applied,after);
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=restored&&watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[sd]=out;
    }
    Finish(t,kFn[f],n,input,rows,sides);
}

inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFns;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();unsigned total=0;for(unsigned f=0;f<kFns;++f){cases[kFn[f]]=kCases[f];total+=kCases[f];}
    const std::vector<u8> initial=ReadRegions();
    const u32 swaps0[4]{ReadPtr(kPtrPlayer),ReadPtr(kPtrPedPool),ReadPtr(kPtrVehPool),ReadPtr(kPtrObjPool)};
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",total},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+st0..st7 crudos (vacios incluidos); ignora FSW banderas/CC y FIP/FDP; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFns;++f) {
            if(dbgFn>=0&&int(f)!=dbgFn)continue;
            const unsigned before=totals.failures;
            for(unsigned c=dbgFirst;c<kCases[f]&&c<=dbgLast;++c){if(dbgMode)Write({{"event","case_start"},{"fn",f},{"case",c}});RunCase(env,totals,f,c);}
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases[f]},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool realNow=ReadRegions()==initial&&ReadPtr(kPtrPlayer)==swaps0[0]&&ReadPtr(kPtrPedPool)==swaps0[1]&&ReadPtr(kPtrVehPool)==swaps0[2]&&ReadPtr(kPtrObjPool)==swaps0[3];
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"pool_restored",realNow}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"pool_restored",realNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
