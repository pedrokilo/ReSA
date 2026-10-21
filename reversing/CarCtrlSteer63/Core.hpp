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
// Nucleo del banco CarCtrlSteer63 (lote 63, CCarCtrl parte B1). Plantilla: reversing/GarageUpdate63 (compuertas de ABI, falsos identicos en ambos lados con parche de entrada
// en el EXE y en el cuerpo de la DLL). Tres rutas: Stop (0x428990), HeadingForTarget (0x433280) y TryingToBlockTarget (0x4335E0), todas cdecl y void.

namespace CS63Live {
using namespace CS63Fix;
using HookRoute = OriginalDependencies499::HookRoute;
inline bool done{};
inline unsigned dbgFirst{0},dbgLast{kCases-1};inline bool dbgMode{};
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
    Patches(){p.reserve(16);}
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    void Reset(){while(!p.empty())p.pop_back();}
    ~Patches(){Reset();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};Patches* patches{};};
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

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los falsos se instalan DESPUES de comprobar rutas y colaboradores
// y solo mientras dura la llamada. El estado de los parches se comprueba (globals_restored) tras quitarlos.
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
inline constexpr const char* kFn[kFns]{"CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop","CCarCtrl::SteerAICarWithPhysicsHeadingForTarget","CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget"};
inline constexpr const char* kName[kFns]{"SteerAICarWithPhysicsTryingToBlockTarget_Stop","SteerAICarWithPhysicsHeadingForTarget","SteerAICarWithPhysicsTryingToBlockTarget"};
inline constexpr uintptr_t kEntry[kFns]{0x428990,0x433280,0x4335E0};
inline constexpr u32 kArgc[kFns]{9,8,10};
inline constexpr unsigned kMaxEvents = 64;

struct Arena {
    alignas(16) u32 gl[kGuardWords];
    alignas(16) u32 veh[kVehSize/4];
    alignas(16) u32 mat[kMatWords];
    alignas(16) u32 pv[kVehSize/4];
    alignas(16) u32 ped[kPedSize/4];
    alignas(16) u32 intel[kIntelWords];
    alignas(16) u32 task[kTaskWords];
    alignas(16) u32 vt[kVtWords];
    alignas(16) u32 out[kOutWords];
    alignas(16) u32 gr[kGuardWords];
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
    const struct {const void* base;size_t bytes;const char* name;} regions[]{{W.veh,sizeof(W.veh),"V"},{W.mat,sizeof(W.mat),"M"},{W.pv,sizeof(W.pv),"PV"},{W.ped,sizeof(W.ped),"PED"},{W.intel,sizeof(W.intel),"I"},
                                                                                {W.task,sizeof(W.task),"T"},{W.vt,sizeof(W.vt),"VT"},{W.out,sizeof(W.out),"O"}};
    for(const auto& r:regions)if(Within(r.base,r.bytes,v,off)){if(off)snprintf(buf,sizeof(buf),"%s+%u",r.name,off);else snprintf(buf,sizeof(buf),"%s",r.name);return std::string(buf);}
    if(Local(v))return "L";
    return v;
}

// ---- eventos de los falsos (nada aqui asigna memoria)
struct Ev {u32 kind{};u32 n{};u32 ptrMask{};u32 v[12]{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline Ev& PushEv(u32 kind,u32 n,u32 ptrMask){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{};e.kind=kind;e.n=n;e.ptrMask=ptrMask;++nev;return e;}
inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){const Ev& e=evs[i];json row=json::array({e.kind});for(u32 k=0;k<e.n;++k)row.push_back((e.ptrMask>>k)&1u?Tok(e.v[k]):json(e.v[k]));out.push_back(row);}
    return out;
}
inline Spec S;                       // caso en curso
inline u32 cnt[kKinds + 4]{};
inline u32 NextK(unsigned kind){return cnt[kind]++;}
inline u32 Junk(u32 bit){return (bit&1u)|0xA5A5A500u;}   // AL = bit, resto de EAX basura (los llamadores del EXE usan `test al,al`)

#define EV(kind,n,mask) Ev& e=PushEv(kind,n,mask)
// ---- Stop
inline u32 __cdecl FkFpv(u32 a,u32 b){EV(kFpv,2,0);e.v[0]=a;e.v[1]=b;const u32 k=NextK(kFpv);return (S.pvMask>>(k&31))&1u?Self(W.pv):0u;}                  // 0x56E0D0 FindPlayerVehicle
inline u32 __fastcall FkGetActive(u32 self,void*){EV(kGetActive,1,1);e.v[0]=self;NextK(kGetActive);return S.taskKind?Self(W.task):0u;}                      // 0x681720 CTaskManager::GetActiveTask
inline u32 __fastcall FkUpsideDown(u32 self,void*){EV(kUpsideDown,1,1);e.v[0]=self;const u32 k=NextK(kUpsideDown);return Junk((S.upMask>>(k&31))&1u);}       // 0x6D1D90 CVehicle::IsUpsideDown
inline void __cdecl FkTell(u32 veh){EV(kTell,1,1);e.v[0]=veh;NextK(kTell);}                                                                              // 0x41C760 CCarAI::TellOccupantsToLeaveCar
inline u32 __fastcall FkTaskType(u32 self,void*){EV(kTaskType,1,1);e.v[0]=self;NextK(kTaskType);return S.taskKind==1?0x451u:0x123u;}                      // CTask::GetTaskType (virtual, vtable +0x10)
// ---- Heading
inline double __cdecl FkAtan(u32 x,u32 y){EV(kAtan,2,0);e.v[0]=x;e.v[1]=y;const u32 k=NextK(kAtan);return DFlt(S.atan[k%3]);}                              // 0x53CC70 CGeneral::GetATanOfXY (ST0 sin redondear)
inline float __cdecl FkWeave(u32 veh,u32 phys,u32 a,u32 b,u32 c){EV(kWeave,5,3);e.v[0]=veh;e.v[1]=phys;e.v[2]=a;e.v[3]=b;e.v[4]=c;NextK(kWeave);return Flt(S.weave);}   // 0x4325C0
inline float __cdecl FkMaxSteer(u32 veh){EV(kMaxSteer,1,1);e.v[0]=veh;NextK(kMaxSteer);return Flt(S.maxSteer);}                                           // 0x427FE0
inline float __cdecl FkSpeedMul(u32 a,u32 b,u32 c,u32 d){EV(kSpeedMul,4,0);e.v[0]=a;e.v[1]=b;e.v[2]=c;e.v[3]=d;NextK(kSpeedMul);return Flt(S.speedMul);}   // 0x4224E0
// ---- Block
inline void __cdecl FkHeadingFake(u32 veh,u32 target,u32 x,u32 y,u32* steer,u32* accel,u32* brake,u8* handbrake){                                          // 0x433280
    EV(kHeadingFake,8,0xF3);e.v[0]=veh;e.v[1]=target;e.v[2]=x;e.v[3]=y;e.v[4]=Self(steer);e.v[5]=Self(accel);e.v[6]=Self(brake);e.v[7]=Self(handbrake);NextK(kHeadingFake);
    *steer=S.hOut[0];*accel=S.hOut[1];*brake=S.hOut[2];*handbrake=u8(S.hOut[3]);
}
#undef EV

// ---- estado real del EXE que las funciones leen
struct RealWord {uintptr_t addr;unsigned size;};
inline const RealWord kReal[]{{kTimeStepAddr,4}};
struct RealState {std::array<u32,std::size(kReal)> v{};};
inline u32 ReadReal(const RealWord& r){u32 v{};memcpy(&v,reinterpret_cast<void*>(r.addr),r.size);return v;}
inline void WriteReal(const RealWord& r,u32 v){memcpy(reinterpret_cast<void*>(r.addr),&v,r.size);}
inline RealState SnapReal(){RealState s;for(size_t i=0;i<std::size(kReal);++i)s.v[i]=ReadReal(kReal[i]);return s;}
inline void RestoreReal(const RealState& s){for(size_t i=0;i<std::size(kReal);++i)WriteReal(kReal[i],s.v[i]);}
inline void ApplySpecReal(const Spec& s){WriteReal(kReal[0],s.step);}
inline const char* const kRealName[]{"timestep"};
inline json RealChanges(const RealState& before,const RealState& after) {
    json out=json::array();
    for(size_t i=0;i<std::size(kReal);++i)if(before.v[i]!=after.v[i])out.push_back(json::array({kRealName[i],after.v[i]}));
    return out;
}

// ---- imagen de un caso
inline void BuildCase(const Spec& s,unsigned n,Arena& a) {
    u32* all=reinterpret_cast<u32*>(&a);
    for(size_t i=0;i<sizeof(Arena)/4;++i)all[i]=Garbage(n,u32(i));
    for(u32 i=0;i<kGuardWords;++i){a.gl[i]=0xA5A5A5A5u^i;a.gr[i]=0x5A5A5A5Au^i;}
    memset(a.veh,0,sizeof(a.veh));memset(a.mat,0,sizeof(a.mat));memset(a.pv,0,sizeof(a.pv));memset(a.ped,0,sizeof(a.ped));memset(a.intel,0,sizeof(a.intel));
    memset(a.task,0,sizeof(a.task));memset(a.vt,0,sizeof(a.vt));memset(a.out,0,sizeof(a.out));
    u8* b=reinterpret_cast<u8*>(a.veh);
    auto W32=[&](u32 off,u32 val){memcpy(b+off,&val,4);};
    for(int k=0;k<3;++k){W32(kVPos+4*k,s.vpos[k]);W32(kVSpeed+4*k,s.msp[k]);}
    W32(kVMat,s.hasMat?Self(W.mat):0u);
    b[kVStyle]=u8(s.style);b[kVMission]=u8(s.mission);b[kVCruise]=u8(s.cruise);b[kVFlags]=u8(s.flags428);
    W32(kVDriver,s.hasDriver?Self(W.ped):0u);
    {const u16 cops=u16(s.cops);memcpy(b+kVCops,&cops,2);}
    W32(kVSub,s.sub594);W32(kVTimer,s.f818);
    for(int k=0;k<3;++k){a.mat[kMTop/4+k]=s.mtop[k];a.mat[kMPos/4+k]=s.mpos[k];}
    u8* pvb=reinterpret_cast<u8*>(a.pv);
    for(int k=0;k<3;++k)memcpy(pvb+kVSpeed+4*k,&s.pv[k],4);
    {const u32 intel=s.intelNull?0u:Self(W.intel);memcpy(reinterpret_cast<u8*>(a.ped)+kPedIntel,&intel,4);}
    a.task[0]=Self(W.vt);a.vt[4]=Self(&FkTaskType);
    for(int k=0;k<4;++k)a.out[k]=s.outPre[k];
}
inline json ChangesJson(const Arena& post,const Arena& ref) {
    json out=json::array();
    const u32* pa=reinterpret_cast<const u32*>(&post);const u32* ra=reinterpret_cast<const u32*>(&ref);
    const struct {const u32* a;const u32* r;size_t words;const char* name;} regions[]{{post.veh,ref.veh,sizeof(post.veh)/4,"V"},{post.mat,ref.mat,sizeof(post.mat)/4,"M"},{post.pv,ref.pv,sizeof(post.pv)/4,"PV"},
                                                                                       {post.out,ref.out,sizeof(post.out)/4,"O"}};
    for(const auto& r:regions)for(size_t w=0;w<r.words;++w)if(r.a[w]!=r.r[w])out.push_back(json::array({r.name,0,w,r.a[w]}));
    // X: cualquier otra palabra (guardas, ped, intel, task, vtable...): no deberia cambiar nunca
    for(size_t i=0;i<sizeof(Arena)/4;++i) {
        if(pa[i]==ra[i])continue;
        const u32 addr=Self(&pa[i])-Self(&post);
        auto in=[&](const void* base,size_t bytes){const u32 b=Self(base)-Self(&post);return addr>=b&&addr<b+bytes;};
        if(in(post.veh,sizeof(post.veh))||in(post.mat,sizeof(post.mat))||in(post.pv,sizeof(post.pv))||in(post.out,sizeof(post.out)))continue;
        out.push_back(json::array({"X",0,u32(i),pa[i]}));
    }
    return out;
}

inline void RunCase(Env& env,Totals& t,unsigned n) {
    const Spec s=MakeSpec(n);
    const unsigned fn=s.fn;
    const json input=json::parse(JsonSpec(s));
    BuildCase(s,n,WRef);
    Side sides[2];json rows[2];
    u32 args[10]{};
    const u32 veh=Self(W.veh);
    u32 argc=kArgc[fn];
    switch(fn) {
    case kStop:    {const u32 a[9]{veh,s.in[0],s.in[1],s.in[2],s.in[3],Self(&W.out[0]),Self(&W.out[1]),Self(&W.out[2]),Self(&W.out[3])};memcpy(args,a,sizeof(a));break;}
    case kHeading: {const u32 a[8]{veh,s.target?Self(W.pv):0u,s.in[0],s.in[1],Self(&W.out[0]),Self(&W.out[1]),Self(&W.out[2]),Self(&W.out[3])};memcpy(args,a,sizeof(a));break;}
    default:       {const u32 a[10]{veh,s.unusedArg,s.in[0],s.in[1],s.in[2],s.in[3],Self(&W.out[0]),Self(&W.out[1]),Self(&W.out[2]),Self(&W.out[3])};memcpy(args,a,sizeof(a));break;}
    }
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        S=s;memset(cnt,0,sizeof(cnt));nev=0;
        const RealState before=SnapReal();
        ApplySpecReal(s);
        const RealState applied=SnapReal();
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t entry){return Gate(entry,args,argc,s.regs[1],s.regs[2],s.regs[3],s.regs[0],4*argc);});
        const RealState after=SnapReal();
        RestoreReal(before);
        const bool restored=SnapReal().v==before.v;
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["changes"]=ChangesJson(W,WRef);obs["globals"]=RealChanges(applied,after);
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
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kPerFn;
    const RealState initial=SnapReal();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFns;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=f*kPerFn;c<(f+1)*kPerFn;++c){if(c<dbgFirst||c>dbgLast)continue;if(dbgMode)Write({{"event","case_start"},{"case",c}});RunCase(env,totals,c);}
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kPerFn},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool realNow=SnapReal().v==initial.v;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"pool_restored",realNow}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"pool_restored",realNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
