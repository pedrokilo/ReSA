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
// Nucleo del banco CarCtrlAir63 (lote 63, CCarCtrl parte A). Plantilla: reversing/Garage63 (que a su vez usa reversing/D3DResource63 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).

namespace A63Live {
using namespace A63Fix;
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
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes,u32 primeDelta=0,u32 primeValue=0){
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
        mov eax,primeDelta
        test eax,eax
        jz noprime
        mov edx,esp
        sub edx,eax
        mov ecx,primeValue
        mov [edx],ecx
    noprime:
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
    std::array<std::unique_ptr<HookRoute>,kFnCount> route;              // una por funcion
    std::vector<std::unique_ptr<HookRoute>> collab;                 // rutas de los colaboradores con hook (apagados durante el banco)
    std::array<std::vector<StubSpec>,kFnCount> stubs;                   // colaboradores sustituidos por funcion
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
inline constexpr const char* kFn[kFnCount]{"CCarCtrl::GetAIHeliToFlyInDirection","CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer","CCarCtrl::SteerAIHeliToCrashAndBurn","CCarCtrl::SteerAIPlaneToCrashAndBurn",
                                          "CCarCtrl::SteerAIPlaneTowardsTargetCoors","CCarCtrl::SteerAIPlaneToFollowEntity","CCarCtrl::SteerAIHeliTowardsTargetCoors","CCarCtrl::SteerAIHeliFlyingAwayFromPlayer",
                                          "CCarCtrl::SteerAIHeliToLand","CCarCtrl::SteerAIHeliAsPoliceHeli","CCarCtrl::TestWhetherToFirePlaneGuns","CCarCtrl::TriggerDogFightMoves","CCarCtrl::GetAIPlaneToAttackPlayer",
                                          "CCarCtrl::GetAIPlaneToDoDogFight","CCarCtrl::SteerAIHeliToFollowEntity","CCarCtrl::SteerAIHeliToKeepEntityInView","CCarCtrl::GetAIHeliToAttackPlayer","CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget","CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer",
                                          "CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer","CCarCtrl::FireHeliRocketsAtTarget","CCarCtrl::FlyAIHeliToTarget_FixedOrientation","CCarCtrl::FlyAIPlaneInCertainDirection","CCarCtrl::FlyAIHeliInCertainDirection"};
inline constexpr const char* kName[kFnCount]{"GetAIHeliToFlyInDirection","GetAIPlaneToDoDogFightAgainstPlayer","SteerAIHeliToCrashAndBurn","SteerAIPlaneToCrashAndBurn","SteerAIPlaneTowardsTargetCoors",
                                            "SteerAIPlaneToFollowEntity","SteerAIHeliTowardsTargetCoors","SteerAIHeliFlyingAwayFromPlayer","SteerAIHeliToLand","SteerAIHeliAsPoliceHeli","TestWhetherToFirePlaneGuns",
                                            "TriggerDogFightMoves","GetAIPlaneToAttackPlayer","GetAIPlaneToDoDogFight","SteerAIHeliToFollowEntity","SteerAIHeliToKeepEntityInView","GetAIHeliToAttackPlayer","SteerAIBoatWithPhysicsHeadingForTarget","SteerAIBoatWithPhysicsAttackingPlayer",
                                            "SteerAIBoatWithPhysicsCirclingPlayer","FireHeliRocketsAtTarget","FlyAIHeliToTarget_FixedOrientation","FlyAIPlaneInCertainDirection","FlyAIHeliInCertainDirection"};
inline constexpr uintptr_t kEntry[kFnCount]{0x42A730,0x42F370,0x4238E0,0x423880,0x423790,0x4237F0,0x42A630,0x42ACB0,0x42AD30,0x42AAD0,0x429520,0x429300,0x429780,0x429890,0x42A750,0x42AEB0,0x42F3C0,0x428BE0,0x428DE0,0x429090,0x42B270,0x423940,0x423000,0x429A70};
inline constexpr std::array<unsigned,kFnCount> kActive{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};   // funciones de esta tanda (indices de Fn)
inline constexpr unsigned kMaxEvents=64;

struct Arena {
    alignas(16) u32 gl[kGuardWords];
    alignas(16) u32 a[kObjWords];
    alignas(16) u32 gr[kGuardWords];
    alignas(16) u32 t[kObjWords];
    alignas(16) u32 p[kObjWords];
    alignas(16) u32 mat[3][kMatWords];
    alignas(16) u32 ped[kPedWords];
    alignas(16) u32 scratch[64];
    alignas(16) u32 vt[8];
    alignas(16) u32 intel[8];
    alignas(16) u32 task[8];
};
alignas(16) inline Arena W{},WRef{};
inline u32 Self(const void* p){return u32(uintptr_t(p));}
inline bool Within(const void* base,size_t bytes,u32 v,u32& off){const u32 b=Self(base);if(v>=b&&v<b+bytes){off=v-b;return true;}return false;}
inline u32 Local(u32 v){const char here=0;const auto d=intptr_t(v)-intptr_t(uintptr_t(&here));return d>-0x10000&&d<0x10000;}

// ---- fichas simbolicas de punteros
inline json Tok(u32 v) {
    char buf[48];
    u32 off{};
    if(v==0)return 0;
    if(Within(W.a,sizeof(W.a),v,off)){if(off)snprintf(buf,sizeof(buf),"A+%u",off);else snprintf(buf,sizeof(buf),"A");return std::string(buf);}
    if(Within(W.t,sizeof(W.t),v,off)){if(off)snprintf(buf,sizeof(buf),"T+%u",off);else snprintf(buf,sizeof(buf),"T");return std::string(buf);}
    if(Within(W.p,sizeof(W.p),v,off)){if(off)snprintf(buf,sizeof(buf),"P+%u",off);else snprintf(buf,sizeof(buf),"P");return std::string(buf);}
    if(Within(W.mat,sizeof(W.mat),v,off)){const u32 i=off/(kMatWords*4),r=off%(kMatWords*4);if(r)snprintf(buf,sizeof(buf),"M%u+%u",i,r);else snprintf(buf,sizeof(buf),"M%u",i);return std::string(buf);}
    if(Within(W.ped,sizeof(W.ped),v,off)){snprintf(buf,sizeof(buf),"PED+%u",off);return std::string(buf);}
    if(Within(W.scratch,sizeof(W.scratch),v,off)){snprintf(buf,sizeof(buf),"SCR+%u",off);return std::string(buf);}
    if(Within(W.vt,sizeof(W.vt),v,off)){snprintf(buf,sizeof(buf),"VT+%u",off);return std::string(buf);}
    if(Within(W.intel,sizeof(W.intel),v,off)){snprintf(buf,sizeof(buf),"INT+%u",off);return std::string(buf);}
    if(Within(W.task,sizeof(W.task),v,off)){snprintf(buf,sizeof(buf),"TASK+%u",off);return std::string(buf);}
    if(Local(v))return "L";
    return v;
}

// ---- eventos de los falsos (nada aqui asigna memoria)
struct Ev {u32 kind{};u32 n{};u32 ptrMask{};u32 v[16]{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline Ev& PushEv(u32 kind,u32 n,u32 ptrMask){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{};e.kind=kind;e.n=n;e.ptrMask=ptrMask;++nev;return e;}
inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){const Ev& e=evs[i];json row=json::array({e.kind});for(u32 k=0;k<e.n;++k)row.push_back((e.ptrMask>>k)&1u?Tok(e.v[k]):json(e.v[k]));out.push_back(row);}
    return out;
}
inline Spec S;                       // caso en curso
inline u32 cnt[32]{};
inline u32 NextK(unsigned kind){return cnt[kind]++;}
enum {kEAtan=1,kEFlyHeliDir,kEFlyHeliFix,kEFlyPlane,kEDog,kEFpv,kEFpp,kEFpc,kENorm,kERand,kESpeed,kEApp,kEMissile,kETest,kETrig,kEAlloc,kEUpd,kETaskMgr,kETaskType,kEFpe,kERockets,kEFollow,kEBoat,kEDistLine,kEAddProj,kELos,kEProbe,kEGear};
inline constexpr uintptr_t kTimerAddr=0xB7CB84;      // CTimer::m_snTimeInMilliseconds
inline constexpr uintptr_t kPrevTimerAddr=0xB7CB78;  // CTimer::m_snPreviousTimeInMilliseconds
inline constexpr uintptr_t kStepAddr=0xB7CB5C;       // CTimer::ms_fTimeStep

inline u32 PtrOfKind(u32 kind){switch(kind){case 1:return Self(W.t);case 2:return Self(W.p);case 3:return Self(W.ped);case 4:return Self(W.a);default:return 0u;}}
// GetATanOfXY 0x53CC70 (cdecl, float x, float y -> ST0). Los argumentos llegan como dwords: se registran sus bits exactos.
inline float __cdecl FkAtan(u32 x,u32 y){Ev& e=PushEv(kEAtan,2,0);e.v[0]=x;e.v[1]=y;return Flt(S.atan[NextK(kEAtan)&7]);}
inline void __cdecl FkFlyHeliDir(u32 heli,u32 a2,u32 a3,u32 a4){Ev& e=PushEv(kEFlyHeliDir,4,1);e.v[0]=heli;e.v[1]=a2;e.v[2]=a3;e.v[3]=a4;}                 // 0x429A70 (heli, float, float, bool)
inline void __cdecl FkFlyHeliFix(u32 heli,u32 o,u32 x,u32 y,u32 z){Ev& e=PushEv(kEFlyHeliFix,5,1);e.v[0]=heli;e.v[1]=o;e.v[2]=x;e.v[3]=y;e.v[4]=z;}          // 0x423940 (heli, float, CVector)
inline void __cdecl FkFlyPlane(u32 plane){PushEv(kEFlyPlane,1,1).v[0]=plane;}                                                                              // 0x423000
inline void __cdecl FkDog(u32 veh){PushEv(kEDog,1,1).v[0]=veh;}                                                                                            // 0x429890
inline u32 __cdecl FkFpv(u32 id,u32 flag){Ev& e=PushEv(kEFpv,2,0);e.v[0]=id;e.v[1]=flag;const u32 k=NextK(kEFpv)&7;return S.fpv[k]==1?Self(W.p):S.fpv[k]==2?Self(W.t):S.fpv[k]==3?Self(W.a):0u;}   // 0x56E0D0
inline u32 __cdecl FkFpp(u32 id){PushEv(kEFpp,1,0).v[0]=id;return Self(W.ped);}                                                                           // 0x56E210
inline u32 __cdecl FkFpc(u32* out,u32 id){Ev& e=PushEv(kEFpc,2,1);e.v[0]=Self(out);e.v[1]=id;const u32 k=NextK(kEFpc)&7;out[0]=S.pc[k][0];out[1]=S.pc[k][1];out[2]=S.pc[k][2];return Self(out);}   // 0x56E010 (CVector*, int)

#pragma runtime_checks("", off)
// CVector::Normalise 0x59C910 (thiscall, in situ): deja en el vector el valor que dicta el caso
__declspec(safebuffers) inline void __fastcall FkNorm(u32 self,void*){PushEv(kENorm,1,1).v[0]=self;const u32 k=NextK(kENorm)&3;u32* v=reinterpret_cast<u32*>(self);v[0]=S.norm[k][0];v[1]=S.norm[k][1];v[2]=S.norm[k][2];}
// CVehicle::GetVehicleAppearance 0x6D1080 (thiscall)
__declspec(safebuffers) inline u32 __fastcall FkApp(u32 self,void*){PushEv(kEApp,1,1).v[0]=self;return S.app[NextK(kEApp)&3];}
#pragma runtime_checks("", restore)
inline int __cdecl FkRand(){PushEv(kERand,0,0);return int(S.rnd[NextK(kERand)&7]);}                                                      // 0x821B1E
inline u32 __cdecl FkSpeed(u32 id){PushEv(kESpeed,1,0).v[0]=id;return Self(W.scratch);}                                                 // 0x56E090 (id) -> CVector*
inline void __cdecl FkMissile(u32 veh,u32 ent){Ev& e=PushEv(kEMissile,2,3);e.v[0]=veh;e.v[1]=ent;}                                      // 0x429600
inline void __cdecl FkTest(u32 veh,u32 ent){Ev& e=PushEv(kETest,2,3);e.v[0]=veh;e.v[1]=ent;}                                            // 0x429520
inline void __cdecl FkTrig(u32 v1,u32 v2){Ev& e=PushEv(kETrig,2,3);e.v[0]=v1;e.v[1]=v2;}                                                // 0x429300

#pragma runtime_checks("", off)
__declspec(safebuffers) inline void __fastcall FkBoat(u32 self,void*){PushEv(kEBoat,1,1).v[0]=self;}                                                                // 0x73DF00 (thiscall)
#pragma runtime_checks("", restore)
inline float __cdecl FkDistLine(u32 a,u32 b,u32 c){Ev& e=PushEv(kEDistLine,3,7);e.v[0]=a;e.v[1]=b;e.v[2]=c;return Flt(S.dline[NextK(kEDistLine)&1]);}   // 0x412970 (lineStart, lineEnd, point) -> ST0
inline u32 __cdecl FkAddProj(u32 creator,u32 type,u32 x,u32 y,u32 z,u32 force,u32 dir,u32 victim){Ev& e=PushEv(kEAddProj,8,(1u<<0)|(1u<<6)|(1u<<7));e.v[0]=creator;e.v[1]=type;e.v[2]=x;e.v[3]=y;e.v[4]=z;e.v[5]=force;e.v[6]=dir;e.v[7]=victim;return 0xA5A5A500u;}   // 0x737C80
// CWorld::ProcessLineOfSight 0x56BA00 (cdecl, 12 dwords): registra origen y destino (vectores apuntados) y los ocho indicadores; deja el punto de choque en el CColPoint y devuelve AL
inline u32 __cdecl FkLos(const u32* origin,const u32* target,u32* colPoint,u32 entPtr,u32 b1,u32 b2,u32 b3,u32 b4,u32 b5,u32 b6,u32 b7,u32 b8){
    Ev& e=PushEv(kELos,15,0);e.v[0]=origin[0];e.v[1]=origin[1];e.v[2]=origin[2];e.v[3]=target[0];e.v[4]=target[1];e.v[5]=target[2];e.v[6]=Local(entPtr)?1u:0u;
    e.v[7]=b1;e.v[8]=b2;e.v[9]=b3;e.v[10]=b4;e.v[11]=b5;e.v[12]=b6;e.v[13]=b7;e.v[14]=b8;
    const u32 k=NextK(kELos)&7;colPoint[0]=S.los[k][0];colPoint[1]=S.los[k][1];colPoint[2]=S.los[k][2];
    return (S.losHit[k]&1u)|0xA5A5A500u;
}
inline float __cdecl FkProbe(u32 plane,u32 t){Ev& e=PushEv(kEProbe,2,1);e.v[0]=plane;e.v[1]=t;return Flt(S.probe[NextK(kEProbe)&31]);}                  // 0x422F80 (plane, float) -> ST0
#pragma runtime_checks("", off)
__declspec(safebuffers) inline void __fastcall FkGear(u32 self,void*){PushEv(kEGear,1,1).v[0]=self;}                                                          // CPlane::SetGearUp 0x6CAC20 (thiscall)
#pragma runtime_checks("", restore)
inline void __cdecl FkFollow(u32 heli){PushEv(kEFollow,1,1).v[0]=heli;}                                                                         // 0x42A750
inline void __cdecl FkRockets(u32 heli,u32 ent){Ev& e=PushEv(kERockets,2,3);e.v[0]=heli;e.v[1]=ent;}                                       // 0x42B270
inline u32 __cdecl FkFpe(u32 id){PushEv(kEFpe,1,0).v[0]=id;const u32 k=NextK(kEFpe)&3;return S.fpe[k]==1?Self(W.t):S.fpe[k]==2?Self(W.p):S.fpe[k]==3?Self(W.a):0u;}   // 0x56E120
#pragma runtime_checks("", off)
// CPlaceable::AllocateMatrix 0x54F560 (thiscall): deja la matriz de T en m_matrix
__declspec(safebuffers) inline void __fastcall FkAlloc(u32 self,void*){PushEv(kEAlloc,1,1).v[0]=self;*reinterpret_cast<u32*>(self+kOffMatrix)=Self(self==Self(W.p)?W.mat[2]:W.mat[1]);}
// CSimpleTransform::UpdateMatrix 0x54F1B0 (thiscall, ret 4)
__declspec(safebuffers) inline void __fastcall FkUpd(u32 self,void*,u32 mat){Ev& e=PushEv(kEUpd,2,3);e.v[0]=self;e.v[1]=mat;}
// CTaskManager::GetActiveTask 0x681720 (thiscall)
__declspec(safebuffers) inline u32 __fastcall FkTaskMgr(u32 self,void*){PushEv(kETaskMgr,1,1).v[0]=self;return S.task?Self(W.task):0u;}
// CTask::GetTaskType (vtable +0x10, thiscall)
__declspec(safebuffers) inline u32 __fastcall FkTaskType(u32 self,void*){PushEv(kETaskType,1,1).v[0]=self;return S.taskType;}
#pragma runtime_checks("", restore)

// ---- imagen de un caso
inline void BuildCase(const Spec& s,Arena& a) {
    u32* all=reinterpret_cast<u32*>(&a);
    for(size_t i=0;i<sizeof(Arena)/4;++i)all[i]=Garbage(s.n,u32(i));
    for(u32 i=0;i<kGuardWords;++i){a.gl[i]=0xA5A5A5A5u^i;a.gr[i]=0x5A5A5A5Au^i;}
    memcpy(a.a,s.a,sizeof(a.a));memcpy(a.t,s.t,sizeof(a.t));memcpy(a.p,s.p,sizeof(a.p));memcpy(a.mat,s.mat,sizeof(a.mat));
    a.a[kOffMatrix/4]=s.hasMat[0]?Self(a.mat[0]):0u;
    a.t[kOffMatrix/4]=s.hasMat[1]?Self(a.mat[1]):0u;
    a.p[kOffMatrix/4]=s.hasMat[2]?Self(a.mat[2]):0u;
    a.a[kOffTarget/4]=PtrOfKind(s.tgtKind);
    a.a[0]=Self(a.vt);a.t[0]=Self(a.vt);a.p[0]=Self(a.vt);
    a.scratch[0]=s.speedV[0];a.scratch[1]=s.speedV[1];a.scratch[2]=s.speedV[2];
    a.t[kOffIntel/4]=Self(a.intel);a.p[kOffIntel/4]=Self(a.intel);                  // m_pIntelligence (CTaskManager = intel + 4)
    a.task[0]=Self(a.vt);a.task[3]=0xC0DE0003u;                                    // tarea falsa: vtable y m_nMoveState (+0xC)
    a.vt[4]=Self(reinterpret_cast<const void*>(&FkTaskType));
}
inline u32 ArgPtr(u32 kind){switch(kind){case 1:return Self(W.t);case 2:return Self(W.p);case 3:return Self(W.a);default:return 0u;}}
inline json ChangesJson(const Arena& post,const Arena& ref) {
    json out=json::array();
    const u32* pa=reinterpret_cast<const u32*>(&post);const u32* ra=reinterpret_cast<const u32*>(&ref);
    struct R {const char* name;const void* base;size_t bytes;};
    const R regions[]{{"A",post.a,sizeof(post.a)},{"T",post.t,sizeof(post.t)},{"P",post.p,sizeof(post.p)},{"M",post.mat,sizeof(post.mat)},{"D",post.ped,sizeof(post.ped)},{"S",post.scratch,sizeof(post.scratch)},
                       {"I",post.intel,sizeof(post.intel)},{"K",post.task,sizeof(post.task)}};
    for(size_t i=0;i<sizeof(Arena)/4;++i) {
        if(pa[i]==ra[i])continue;
        const u32 addr=Self(&pa[i])-Self(&post);
        const char* name="X";u32 idx=u32(i);
        for(const auto& r:regions){const u32 b=Self(r.base)-Self(&post);if(addr>=b&&addr<b+r.bytes){name=r.name;idx=(addr-b)/4;break;}}
        out.push_back(json::array({name,idx,Tok(pa[i])}));
    }
    return out;
}

inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    BuildCase(s,WRef);
    Side sides[2];json rows[2];
    u32 args[2]{Self(W.a),0};u32 argCount=1;u32 popBytes=4;
    if(fn==kTestFire){args[1]=ArgPtr(s.arg2Kind);argCount=2;popBytes=8;}
    if(fn==kDogMoves){args[1]=Self(W.t);argCount=2;popBytes=8;}
    if(fn==kRockets){args[1]=ArgPtr(s.arg2Kind);argCount=2;popBytes=8;}
    u32 argsFixed[5]{Self(W.a),s.fargs[0],s.dv[0],s.dv[1],s.dv[2]};
    u32 argsHeliDir[4]{Self(W.a),s.fargs[0],s.fargs[1],s.dv[0]};
    u32 argsBig[6]{Self(W.a),0,0,0,0,0};
    if(fn==kBoatHead){argsBig[1]=s.fargs[0];argsBig[2]=s.fargs[1];argsBig[3]=Self(&W.scratch[8]);argsBig[4]=Self(&W.scratch[9]);argsBig[5]=Self(&W.scratch[10]);}
    if(fn==kBoatAttack||fn==kBoatCircle){argsBig[1]=Self(&W.scratch[8]);argsBig[2]=Self(&W.scratch[9]);argsBig[3]=Self(&W.scratch[10]);argsBig[4]=Self(&W.scratch[11]);}
    const u32* argPtr=args;
    if(fn==kBoatHead){argPtr=argsBig;argCount=6;popBytes=24;}
    if(fn==kBoatAttack||fn==kBoatCircle){argPtr=argsBig;argCount=5;popBytes=20;}
    if(fn==kFixed){argPtr=argsFixed;argCount=5;popBytes=20;}
    if(fn==kHeliDir){argPtr=argsHeliDir;argCount=4;popBytes=16;}
    const u32 primeDelta=0u;
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        S=s;memset(cnt,0,sizeof(cnt));nev=0;
        const u32 timerBefore=*reinterpret_cast<u32*>(kTimerAddr),prevTimerBefore=*reinterpret_cast<u32*>(kPrevTimerAddr),stepBefore=*reinterpret_cast<u32*>(kStepAddr);
        if(fn==kPlaneDir||fn==kHeliDir){*reinterpret_cast<u32*>(kStepAddr)=s.stepBits;}
        *reinterpret_cast<u32*>(kTimerAddr)=s.timeMs;*reinterpret_cast<u32*>(kPrevTimerAddr)=s.prevMs;
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t entry){return Gate(entry,argPtr,argCount,s.regs[1],s.regs[2],s.regs[3],s.regs[0],popBytes,primeDelta,s.stale);});
        const bool timerUntouched=*reinterpret_cast<u32*>(kTimerAddr)==s.timeMs&&*reinterpret_cast<u32*>(kPrevTimerAddr)==s.prevMs;
        *reinterpret_cast<u32*>(kTimerAddr)=timerBefore;*reinterpret_cast<u32*>(kPrevTimerAddr)=prevTimerBefore;
        const bool stepUntouched=(fn!=kPlaneDir&&fn!=kHeliDir)||*reinterpret_cast<u32*>(kStepAddr)==s.stepBits;
        *reinterpret_cast<u32*>(kStepAddr)=stepBefore;
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["changes"]=ChangesJson(W,WRef);
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=timerUntouched&&stepUntouched&&watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[sd]=out;
    }
    Finish(t,kFn[fn],n,input,rows,sides);
}

inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f:kActive) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(unsigned f:kActive)cases[kFn[f]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",unsigned(kActive.size())},{"pairs",unsigned(kActive.size())*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)}});
    Totals totals;
    if(ready) {
        for(unsigned fn:kActive) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)RunCase(env,totals,fn,c);
            Write({{"event","function_summary"},{"function",kFn[fn]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
