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
// Nucleo del banco Remote64 (lote 64). Plantilla: reversing/GarageUpdate63 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).

namespace R64Live {
using namespace R64Fix;
using HookRoute = OriginalDependencies499::HookRoute;
inline bool done{};
inline unsigned dbgFirst{0},dbgLast{0xFFFFFFFFu};inline bool dbgMode{};
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
// true (regla dura del proyecto): se comparan tambien los diez bytes de cada registro x87 VACIO (la DLL deja la suma z en T-1 como el EXE); de FSW solo TOP, no las banderas.
inline constexpr bool kCompareDeadX87Registers=true;
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
    Patches(){p.reserve(16);}   // sin reasignaciones mientras haya parches de new/delete activos
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
inline constexpr const char* kFn[kFns]{"CRemote::GivePlayerRemoteControlledCar","CRemote::TakeRemoteControlledCarFromPlayer"};
inline constexpr const char* kName[kFns]{"GivePlayerRemoteControlledCar","TakeRemoteControlledCarFromPlayer"};
inline constexpr uintptr_t kEntry[kFns]{0x45AB10,0x45AE80};
inline constexpr unsigned kCasesOf[kFns]{kCasesGive,kCasesTake};
inline constexpr unsigned kMaxEvents = 24;

struct Arena {
    alignas(16) u32 gl[kGuardWords];
    alignas(16) u32 veh[kMaxVeh][kVehWords];
    alignas(16) u32 mat[kMaxVeh][kMatWords];
    alignas(16) u32 pool[kPoolWords];
    alignas(16) u8 states[kStatesBytes];
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
    if(Within(W.veh,sizeof(W.veh),v,off)){const u32 i=off/kVehSize,r=off%kVehSize;if(r)snprintf(buf,sizeof(buf),"V%u+%u",i,r);else snprintf(buf,sizeof(buf),"V%u",i);return std::string(buf);}
    if(Within(W.mat,sizeof(W.mat),v,off)){const u32 i=off/(kMatWords*4),r=off%(kMatWords*4);if(r)snprintf(buf,sizeof(buf),"M%u+%u",i,r);else snprintf(buf,sizeof(buf),"M%u",i);return std::string(buf);}
    if(Local(v))return "L";
    return v;
}

// ---- eventos de los falsos (nada aqui asigna memoria)
struct Ev {u32 kind{};u32 n{};u32 ptrMask{};u32 v[8]{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline Ev& PushEv(u32 kind,u32 n,u32 ptrMask){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{};e.kind=kind;e.n=n;e.ptrMask=ptrMask;++nev;return e;}
inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){const Ev& e=evs[i];json row=json::array({e.kind});for(u32 k=0;k<e.n;++k)row.push_back((e.ptrMask>>k)&1u?Tok(e.v[k]):json(e.v[k]));out.push_back(row);}
    return out;
}
inline Spec S;                       // caso en curso
inline u32 Junk(u32 bit){return (bit&1u)|0xA5A5A500u;}   // AL = bit, resto de EAX basura
inline u32 VehPtr(u32 idx,u32 off=0){return idx==kNone?0u:Self(W.veh[idx])+off;}
inline u32 RetOf(u32 self){return S.retIdx==kNone?self:VehPtr(S.retIdx);}
inline void Mutate(const Mut& m) {
    if(m.kind&2)*reinterpret_cast<u32*>(kPlayers+m.pl*kPlayerStride+kRemoteOff)=VehPtr(m.idx,m.off);
    if(m.kind&1)*reinterpret_cast<u8*>(kFocusAddr)=u8(m.focus);
}

#define EV(kind,n,mask) Ev& e=PushEv(kind,n,mask)
// ---- Give
inline u32 __cdecl FkIsHeli(u32 model){EV(kIsHeli,1,0);e.v[0]=model;return Junk(S.heli);}                                                // 0x4C5B00
inline u32 __cdecl FkIsPlane(u32 model){EV(kIsPlane,1,0);e.v[0]=model;return Junk(S.plane);}                                            // 0x4C5B30
inline u32 __cdecl FkNew(u32 size){EV(kNew,1,0);e.v[0]=size;return VehPtr(S.newIdx);}                                                     // 0x6E2D50
inline u32 __fastcall FkCtorHeli(u32 self,void*,u32 model,u32 by){EV(kCtorHeli,3,1);e.v[0]=self;e.v[1]=model;e.v[2]=by;return RetOf(self);}       // 0x6C4190, ret 8
inline u32 __fastcall FkCtorPlane(u32 self,void*,u32 model,u32 by){EV(kCtorPlane,3,1);e.v[0]=self;e.v[1]=model;e.v[2]=by;return RetOf(self);}   // 0x6C8E20, ret 8
inline u32 __fastcall FkCtorAuto(u32 self,void*,u32 model,u32 by,u32 flag){EV(kCtorAuto,4,1);e.v[0]=self;e.v[1]=model;e.v[2]=by;e.v[3]=flag;return RetOf(self);}   // 0x6B0A90, ret 0Ch
inline float __cdecl FkGround(u32 x,u32 y,u32 z,u32* out,u32 entity){                                                                    // 0x5696C0
    EV(kGround,5,0x08);e.v[0]=x;e.v[1]=y;e.v[2]=z;e.v[3]=Self(out);e.v[4]=entity;
    *reinterpret_cast<u8*>(out)=u8(S.gflag);
    return Flt(S.ground);
}
inline float __fastcall FkDist(u32 self,void*){EV(kDist,1,1);e.v[0]=self;return Flt(S.dist);}                                              // 0x536BE0
inline void __fastcall FkRotZ(u32 self,void*,u32 angle){EV(kRotZ,2,1);e.v[0]=self;e.v[1]=angle;}                                          // 0x59B020, ret 4
inline void __cdecl FkJoin(u32 veh){                                                                                                      // 0x42F5A0
    EV(kJoin,1,1);e.v[0]=veh;
    reinterpret_cast<u8*>(veh)[0x428]^=u8(S.j428);reinterpret_cast<u8*>(veh)[0x42D]^=u8(S.j42D);   // el original reescribe campos del vehiculo
}
inline void __cdecl FkAdd(u32 veh){EV(kAdd,1,1);e.v[0]=veh;}                                                                              // 0x563220
inline u32 __cdecl FkFpv(u32 a,u32 b){EV(kFpv,2,0);e.v[0]=a;e.v[1]=b;return S.pv?VehPtr(S.pvIdx):0u;}                                      // 0x56E0D0
inline void __fastcall FkRegRef(u32 self,void*,u32 ref){EV(kRegRef,2,1);e.v[0]=self;e.v[1]=ref;}                                          // 0x571B70, ret 4
inline void __fastcall FkTakeCtl(u32 self,void*,u32 target,u32 mode,u32 sw,u32 who){EV(kTakeCtl,5,2);e.v[0]=self;e.v[1]=target;e.v[2]=mode;e.v[3]=sw;e.v[4]=who;}   // 0x50C7C0, ret 10h
inline void __fastcall FkZoom(u32 self,void*,u32 zoom){EV(kZoom,2,0);e.v[0]=self;e.v[1]=zoom;}                                            // 0x50C1B0, ret 4
// ---- Take
inline void __fastcall FkSetCreated(u32 self,void*,u32 by){EV(kSetCreated,2,1);e.v[0]=self;e.v[1]=by;Mutate(S.mA);}                        // 0x6D5D70, ret 4
inline void __fastcall FkRemove(u32 self,void*,u32 handle,u32 type){EV(kRemove,3,0);e.v[0]=self;e.v[1]=handle;e.v[2]=type;Mutate(S.mB);}  // 0x4654B0, ret 8

// ---- estado real del EXE que las funciones leen o escriben
struct RealWord {uintptr_t addr;unsigned size;};
inline constexpr RealWord kReal[]{{kFocusAddr,1},{kTimerAddr,4},{kPoolPtrAddr,4},
    {kPlayers+kRemoteOff,4},{kPlayers+kFlagDD,1},{kPlayers+kFlagDE,1},{kPlayers+kFlagDF,1},{kPlayers+kTimeOff,4},
    {kPlayers+kPlayerStride+kRemoteOff,4},{kPlayers+kPlayerStride+kFlagDD,1},{kPlayers+kPlayerStride+kFlagDE,1},{kPlayers+kPlayerStride+kFlagDF,1},{kPlayers+kPlayerStride+kTimeOff,4}};
inline const char* const kRealName[]{"focus","timer","pool","p0_remote","p0_dd","p0_de","p0_df","p0_time","p1_remote","p1_dd","p1_de","p1_df","p1_time"};
struct RealState {std::array<u32,std::size(kReal)> v{};};
inline u32 ReadReal(const RealWord& r){u32 v{};memcpy(&v,reinterpret_cast<void*>(r.addr),r.size);return v;}
inline void WriteReal(const RealWord& r,u32 v){memcpy(reinterpret_cast<void*>(r.addr),&v,r.size);}
inline RealState SnapReal(){RealState s;for(size_t i=0;i<std::size(kReal);++i)s.v[i]=ReadReal(kReal[i]);return s;}
inline void RestoreReal(const RealState& s){for(size_t i=0;i<std::size(kReal);++i)WriteReal(kReal[i],s.v[i]);}
inline void ApplySpecReal(const Spec& s){
    WriteReal(kReal[0],s.focus);WriteReal(kReal[1],s.timer);WriteReal(kReal[2],Self(W.pool));
    for(int p=0;p<2;++p) {
        const size_t b=3+5*p;
        WriteReal(kReal[b],VehPtr(s.slot[p][0],s.slot[p][1]));
        WriteReal(kReal[b+1],s.fl[p][0]);WriteReal(kReal[b+2],s.fl[p][1]);WriteReal(kReal[b+3],s.fl[p][2]);WriteReal(kReal[b+4],s.fl[p][3]);
    }
}
inline json RealChanges(const RealState& before,const RealState& after) {
    json out=json::array();
    for(size_t i=0;i<std::size(kReal);++i)if(before.v[i]!=after.v[i]) {
        const bool ptr=i==2||i==3||i==8;
        out.push_back(json::array({kRealName[i],ptr?Tok(after.v[i]):json(after.v[i])}));
    }
    return out;
}

// ---- imagen de un caso
inline void BuildCase(const Spec& s,unsigned n,Arena& a) {
    u32* all=reinterpret_cast<u32*>(&a);
    for(size_t i=0;i<sizeof(Arena)/4;++i)all[i]=Garbage(n,u32(i));
    for(u32 i=0;i<kGuardWords;++i){a.gl[i]=0xA5A5A5A5u^i;a.gr[i]=0x5A5A5A5Au^i;}
    for(u32 i=0;i<kMaxVeh;++i) {
        for(u32 w=0;w<kVehWords;++w)a.veh[i][w]=VehJunk(n,i,w);
        a.veh[i][5]=s.hasMat[i]?Self(W.mat[i]):0u;                                   // CPlaceable::m_matrix (+0x14)
        for(u32 w=0;w<kMatWords;++w)a.mat[i][w]=MatJunk(n,i,w);
    }
    for(const auto& pk:s.pokes)if(pk[0]<kMaxVeh)reinterpret_cast<u8*>(a.veh[pk[0]])[pk[1]]=u8(pk[2]);
    for(u32 w=0;w<kPoolWords;++w)a.pool[w]=Garbage(n,100+w);
    a.pool[0]=VehPtr(s.stor[0],s.stor[1]);                                           // CPool::m_Storage
    a.pool[1]=Self(&W.states[kStatesBase]);                                          // CPool::m_SlotState (el indice puede ser negativo)
    for(u32 i=0;i<kStatesBytes;++i)a.states[i]=u8(s.states[i]);
}
inline json ChangesJson(const Arena& post,const Arena& ref) {
    json out=json::array();
    for(u32 i=0;i<kMaxVeh;++i)for(u32 w=0;w<kVehWords;++w)if(post.veh[i][w]!=ref.veh[i][w])out.push_back(json::array({"V",i,w,post.veh[i][w]}));
    for(u32 i=0;i<kMaxVeh;++i)for(u32 w=0;w<kMatWords;++w)if(post.mat[i][w]!=ref.mat[i][w])out.push_back(json::array({"M",i,w,post.mat[i][w]}));
    // X: cualquier otra palabra (guardas, pool falso, estados): no deberia cambiar nunca
    const u32* pa=reinterpret_cast<const u32*>(&post);const u32* ra=reinterpret_cast<const u32*>(&ref);
    for(size_t i=0;i<sizeof(Arena)/4;++i) {
        if(pa[i]==ra[i])continue;
        const u32 addr=Self(&pa[i])-Self(&post);
        auto in=[&](const void* base,size_t bytes){const u32 b=Self(base)-Self(&post);return addr>=b&&addr<b+bytes;};
        if(in(post.veh,sizeof(post.veh))||in(post.mat,sizeof(post.mat)))continue;
        out.push_back(json::array({"X",0,u32(i),pa[i]}));
    }
    return out;
}

inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    BuildCase(s,n,WRef);
    Side sides[2];json rows[2];
    u32 args[5]{};u32 argc=0;
    if(fn==kGive){args[0]=s.pos[0];args[1]=s.pos[1];args[2]=s.pos[2];args[3]=s.rot;args[4]=s.model;argc=5;}
    else {args[0]=s.barg;argc=1;}
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        S=s;nev=0;
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
    json cases=json::object();unsigned totalPairs=0;for(unsigned f=0;f<kFns;++f){cases[kFn[f]]=kCasesOf[f];totalPairs+=kCasesOf[f];}
    const RealState initial=SnapReal();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",totalPairs},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+los diez bytes de cada registro x87 (tambien los vacios); ignora FSW banderas/CC y FIP/FDP; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)}});
    Totals totals;
    if(ready) {
        for(u32 fn=0;fn<kFns;++fn) {
            const unsigned before=totals.failures;
            const unsigned last=dbgMode?std::min<unsigned>(dbgLast,kCasesOf[fn]-1):kCasesOf[fn]-1;
            for(unsigned c=dbgMode?dbgFirst:0;c<=last;++c){if(dbgMode)Write({{"event","case_start"},{"fn",fn},{"case",c}});RunCase(env,totals,fn,c);}
            Write({{"event","function_summary"},{"function",kFn[fn]},{"cases",kCasesOf[fn]},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool realNow=SnapReal().v==initial.v;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"real_state_restored",realNow}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"real_state_restored",realNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
