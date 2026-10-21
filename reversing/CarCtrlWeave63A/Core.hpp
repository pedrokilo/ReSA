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
// Nucleo del banco CarCtrlWeave63A (lote 63, CCarCtrl parte B2, tanda A). Plantilla: reversing/Garage63 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el
// cuerpo de la DLL resuelto ANTES de escribir ningun parche).

namespace CwALive {
using namespace CwAFix;
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
// popBytes = bytes que limpia el LLAMADOR (cdecl: 4*n; `ret n` del callee: 0). Si st0Out != 0, guarda ST0 como float (sin sacarlo de la pila) justo tras la llamada.
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes,u32* st0Out){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},outFlags{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{},sehBefore{},sehAfter{};
    u32 wantSt0=st0Out?1u:0u;u32 st0Value{};
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
        cmp wantSt0,0
        je nost0
        fst dword ptr [st0Value]
    nost0:
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
    if(st0Out)*st0Out=st0Value;
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter,sehBefore==sehAfter};
}

// ------------------------------------------------------------------------------------------------ colaboradores, parches y rutas
struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: sin RH_ScopedInstall activo durante el banco; dll=0: la DLL llama a la direccion del EXE (o no hay cuerpo propio)
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
struct StubSpec {Pair pair;void* fake;};
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> p;
    Patches(){p.reserve(16);}
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    ~Patches(){while(!p.empty())p.pop_back();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::array<std::unique_ptr<HookRoute>,kFns> route;
    std::vector<std::unique_ptr<HookRoute>> collab;
    std::array<std::vector<StubSpec>,kFns> stubs;
    std::vector<Watch> watches;
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
inline constexpr const char* kFn[kFns]{"CCarCtrl::TestCollisionBetween2MovingRects","CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper","CCarCtrl::SlowCarDownForOtherCar",
                                      "CCarCtrl::WeaveForOtherCar","CCarCtrl::WeaveForObject"};
inline constexpr const char* kName[kFns]{"TestCollisionBetween2MovingRects","TestCollisionBetween2MovingRects_OnlyFrontBumper","SlowCarDownForOtherCar","WeaveForOtherCar","WeaveForObject"};
inline constexpr uintptr_t kEntry[kFns]{0x425B30,0x425F70,0x42D0E0,0x426350,0x426BC0};
inline constexpr unsigned kMaxEvents=300;

struct Arena {
    alignas(16) u32 gl[kGuardWords];
    alignas(16) u32 ent[kObjs][kEntWords];
    alignas(16) u32 mat[kObjs][kMatWords];
    alignas(16) u32 cm[kObjs][kCmWords];
    alignas(16) u32 mi[kObjs][kMiWords];
    alignas(16) u32 out[4];
    alignas(16) u32 vec[2][4];
    alignas(16) u32 gr[kGuardWords];
};
alignas(16) inline Arena W{},WRef{};
inline u32 Self(const void* p){return u32(uintptr_t(p));}
inline bool Within(const void* base,size_t bytes,u32 v,u32& off){const u32 b=Self(base);if(v>=b&&v<b+bytes){off=v-b;return true;}return false;}
inline u32 Local(u32 v){const char here=0;const auto d=intptr_t(v)-intptr_t(uintptr_t(&here));return d>-0x10000&&d<0x10000;}
inline u32 IdxOfEnt(u32 e){return (e-Self(W.ent))/(kEntWords*4);}
inline u32 IdxOfMi(u32 e){return (e-Self(W.mi))/(kMiWords*4);}

// ---- fichas simbolicas de punteros
inline json Tok(u32 v) {
    char buf[48];
    u32 off{};
    if(v==0)return 0;
    if(Within(W.ent,sizeof(W.ent),v,off)){const u32 i=off/(kEntWords*4),r=off%(kEntWords*4);if(r)snprintf(buf,sizeof(buf),"E%u+%u",i,r);else snprintf(buf,sizeof(buf),"E%u",i);return std::string(buf);}
    if(Within(W.mat,sizeof(W.mat),v,off)){const u32 i=off/(kMatWords*4),r=off%(kMatWords*4);if(r)snprintf(buf,sizeof(buf),"M%u+%u",i,r);else snprintf(buf,sizeof(buf),"M%u",i);return std::string(buf);}
    if(Within(W.cm,sizeof(W.cm),v,off)){const u32 i=off/(kCmWords*4);snprintf(buf,sizeof(buf),"CM%u+%u",i,off%(kCmWords*4));return std::string(buf);}
    if(Within(W.mi,sizeof(W.mi),v,off)){const u32 i=off/(kMiWords*4);snprintf(buf,sizeof(buf),"MI%u+%u",i,off%(kMiWords*4));return std::string(buf);}
    if(Within(W.out,sizeof(W.out),v,off)){snprintf(buf,sizeof(buf),"OUT+%u",off);return std::string(buf);}
    if(Within(W.vec,sizeof(W.vec),v,off)){snprintf(buf,sizeof(buf),"VEC%u+%u",off/16,off%16);return std::string(buf);}
    if(Local(v))return "L";
    return v;
}

// ---- eventos de los falsos (nada aqui asigna memoria)
struct Ev {u32 kind{};u32 n{};u64 ptrMask{};u32 v[48]{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline Ev& PushEv(u32 kind,u32 n,u64 ptrMask){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{};e.kind=kind;e.n=n;e.ptrMask=ptrMask;++nev;return e;}
inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){const Ev& e=evs[i];json row=json::array({e.kind});for(u32 k=0;k<e.n;++k)row.push_back((e.ptrMask>>k)&1u?Tok(e.v[k]):json(e.v[k]));out.push_back(row);}
    return out;
}
inline Spec S;
inline u32 cnt[32]{};
inline u32 NextK(unsigned kind){return cnt[kind]++;}
inline u32 Junk(u32 bit){return (bit&1u)|0xA5A5A500u;}   // AL = bit, resto de EAX basura (los llamadores del EXE usan `test al,al`)
enum {kEFpv=1,kEAtan,kESways,kEGetMat,kEAlloc,kEUpd,kERef,kESwitch,kETcB,kETcF,kESweep};

inline u32 __cdecl FkFpv(u32 a,u32 b){Ev& e=PushEv(kEFpv,2,0);e.v[0]=a;e.v[1]=b;return S.player?Self(W.ent[(S.player-1)%kObjs]):0u;}               // 0x56E0D0
inline float __cdecl FkAtan(u32 x,u32 y){Ev& e=PushEv(kEAtan,2,0);e.v[0]=x;e.v[1]=y;return Flt(S.atanBits);}                                       // 0x53CC70
inline u32 __fastcall FkSways(u32 self,void*){PushEv(kESways,1,1).v[0]=self;return S.o[IdxOfMi(self)%kObjs].sways;}                                // 0x4212C0
inline u32 __fastcall FkGetMat(u32 self,void*){PushEv(kEGetMat,1,1).v[0]=self;return Self(W.mat[IdxOfEnt(self)%kObjs]);}                           // 0x411990
inline void __fastcall FkAlloc(u32 self,void*){PushEv(kEAlloc,1,1).v[0]=self;*reinterpret_cast<u32*>(self+kOffMatrix)=Self(W.mat[IdxOfEnt(self)%kObjs]);}   // 0x54F560
inline void __fastcall FkUpd(u32 self,void*,u32 mat){Ev& e=PushEv(kEUpd,2,3);e.v[0]=self;e.v[1]=mat;}                                               // 0x54F1B0, ret 4
inline void __fastcall FkRef(u32 self,void*,u32 ref){Ev& e=PushEv(kERef,2,3);e.v[0]=self;e.v[1]=ref;}                                              // 0x571B70, ret 4
inline void __cdecl FkSwitch(u32 veh){PushEv(kESwitch,1,1).v[0]=veh;}                                                                              // 0x423FC0
inline void RecVec(Ev& e,u32 at,u32 p){const u32* v=reinterpret_cast<const u32*>(p);e.v[at]=v[0];e.v[at+1]=v[1];e.v[at+2]=v[2];}
inline float __cdecl FkTcB(u32 v1,u32 v2,u32 a3,u32 a4,u32 pa,u32 pb){Ev& e=PushEv(kETcB,12,3);e.v[0]=v1;e.v[1]=v2;e.v[2]=a3;e.v[3]=a4;RecVec(e,4,pa);RecVec(e,7,pb);e.v[10]=pa;e.v[11]=pb;e.ptrMask|=(1ull<<10)|(1ull<<11);return Flt(S.tcB);}   // 0x425B30
inline float __cdecl FkTcF(u32 v1,u32 v2,u32 a3,u32 a4,u32 pa,u32 pb){Ev& e=PushEv(kETcF,12,3);e.v[0]=v1;e.v[1]=v2;e.v[2]=a3;e.v[3]=a4;RecVec(e,4,pa);RecVec(e,7,pb);e.v[10]=pa;e.v[11]=pb;e.ptrMask|=(1ull<<10)|(1ull<<11);return Flt(S.tcF);}   // 0x425F70
inline u32 __cdecl FkSweep(u32 a1,u32 p2,u32 p3,u32 p4,u32 p5,u32 p6,u32 p7,u32 p8,u32 p9,u32 a10,u32 a11,u32 a12,u32 a13) {                               // 0x421A50
    Ev& e=PushEv(kESweep,13+24,0);
    const u32 k=NextK(kESweep);
    e.v[0]=a1;e.v[9]=a10;e.v[10]=a11;e.v[11]=a12;e.v[12]=a13;
    const u32 ps[8]{p2,p3,p4,p5,p6,p7,p8,p9};
    for(u32 i=0;i<8;++i){e.v[1+i]=Local(ps[i])?1u:0u;RecVec(e,13+3*i,ps[i]);}   // 1 = el puntero es un local de la pila (se registran sus tres floats)
    return Junk((S.sweepMask>>(k&31))&1u);
}

// ---- imagen de un caso
inline u32 modelBase{};     // primer indice de modelo propio (kObjs entradas nulas consecutivas)
inline u32 miSpecial[5]{};  // MI_TRAFFICLIGHTS, MI_SINGLESTREETLIGHTS1/2/3, MI_DOUBLESTREETLIGHTS (leidos del juego)
inline u32 TargetOf(u32 sel,u32 junk){return sel==0?0u:(sel<=kObjs?Self(W.ent[sel-1]):junk);}
inline void BuildCase(const Spec& s,unsigned n,Arena& a) {
    u32* all=reinterpret_cast<u32*>(&a);
    for(size_t i=0;i<sizeof(Arena)/4;++i)all[i]=Garbage(n,u32(i));
    for(u32 i=0;i<kGuardWords;++i){a.gl[i]=0xA5A5A5A5u^i;a.gr[i]=0x5A5A5A5Au^i;}
    for(u32 i=0;i<kObjs;++i) {
        const Obj& o=s.o[i];
        u8* e=reinterpret_cast<u8*>(a.ent[i]);
        auto w32=[&](u32 off,u32 v){memcpy(e+off,&v,4);};
        for(int k=0;k<3;++k){w32(kOffPos+4*k,o.pos[k]);w32(kOffSpeed+4*k,o.speed[k]);}
        w32(kOffMatrix,o.hasMat?Self(W.mat[i]):0u);
        const u16 model=u16(o.modelSel<5?miSpecial[o.modelSel]:modelBase+i);
        memcpy(e+kOffModel,&model,2);
        e[kOffB36]=u8(o.b36);
        w32(kOffTimeSw,o.timeSw);
        e[kOffStyle]=u8(o.style);e[kOffMission]=u8(o.mission);e[kOffTempAct]=u8(o.tempAct);w32(kOffTempTime,o.tempTime);e[kOffCtrl]=u8(o.ctrl);
        w32(kOffTarget,TargetOf(o.target,Garbage(n,200+i)));
        e[kOff42B]=u8(o.b42B);w32(kOff46C,o.d46C);w32(kOff58C,TargetOf(o.d58C,Garbage(n,210+i)));
        u32* m=a.mat[i];
        for(int k=0;k<3;++k){m[k]=o.right[k];m[4+k]=o.fwd[k];m[8+k]=o.up[k];m[12+k]=o.mpos[k];}
        for(int k=0;k<6;++k)a.cm[i][k]=o.box[k];
        a.mi[i][5]=Self(W.cm[i]);
    }
    a.out[0]=s.out3;a.out[1]=s.out4;
    for(int k=0;k<3;++k){a.vec[0][k]=s.vecA[k];a.vec[1][k]=s.vecB[k];}
}
inline json ChangesJson(const Arena& post,const Arena& ref) {
    json out=json::array();
    const u32* pa=reinterpret_cast<const u32*>(&post);const u32* ra=reinterpret_cast<const u32*>(&ref);
    for(u32 i=0;i<kObjs;++i)for(u32 w=0;w<kEntWords;++w)if(post.ent[i][w]!=ref.ent[i][w])out.push_back(json::array({"E",i,w,Tok(post.ent[i][w])}));
    for(u32 i=0;i<kObjs;++i)for(u32 w=0;w<kMatWords;++w)if(post.mat[i][w]!=ref.mat[i][w])out.push_back(json::array({"M",i,w,post.mat[i][w]}));
    for(u32 w=0;w<4;++w)if(post.out[w]!=ref.out[w])out.push_back(json::array({"O",0,w,post.out[w]}));
    for(u32 i=0;i<2;++i)for(u32 w=0;w<4;++w)if(post.vec[i][w]!=ref.vec[i][w])out.push_back(json::array({"V",i,w,post.vec[i][w]}));
    for(size_t i=0;i<sizeof(Arena)/4;++i) {   // X: cualquier otra palabra (guardas, col models, info de modelo...): no deberia cambiar nunca
        if(pa[i]==ra[i])continue;
        const u32 addr=Self(&pa[i])-Self(&post);
        auto in=[&](const void* base,size_t bytes){const u32 b=Self(base)-Self(&post);return addr>=b&&addr<b+bytes;};
        if(in(post.ent,sizeof(post.ent))||in(post.mat,sizeof(post.mat))||in(post.out,sizeof(post.out))||in(post.vec,sizeof(post.vec)))continue;
        out.push_back(json::array({"X",0,u32(i),pa[i]}));
    }
    return out;
}
inline uintptr_t modelTable{};   // direccion de ms_modelInfoPtrs[0]

inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    BuildCase(s,n,WRef);
    Side sides[2];json rows[2];
    u32 args[6]{};u32 argCount=0;
    const u32 e1=Self(W.ent[s.ia]),e2=Self(W.ent[s.ib]);
    switch(fn) {
    case kTc:case kFb:args[0]=e1;args[1]=e2;args[2]=s.a3;args[3]=s.a4;args[4]=Self(W.vec[0]);args[5]=Self(W.vec[1]);argCount=6;break;
    case kSlow:args[0]=e1;args[1]=e2;args[2]=Self(W.out);args[3]=s.a4;argCount=4;break;
    default:args[0]=e1;args[1]=e2;args[2]=Self(W.out);args[3]=Self(W.out)+4;argCount=4;break;
    }
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        S=s;memset(cnt,0,sizeof(cnt));nev=0;
        volatile u32* timer=reinterpret_cast<volatile u32*>(kTimerAddr);
        const u32 timerBefore=*timer;
        *timer=s.now;
        u32 st0{};
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t entry){return Gate(entry,args,argCount,s.regs[1],s.regs[2],s.regs[3],s.regs[0],4*argCount,fn<=kFb?&st0:nullptr);});
        const u32 timerAfter=*timer;
        *timer=timerBefore;
        const bool restored=*timer==timerBefore&&timerAfter==s.now;
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["changes"]=ChangesJson(W,WRef);
        if(fn<=kFb)obs["ret"]=st0;
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
    const u32 initialTimer=*reinterpret_cast<u32*>(kTimerAddr);
    u32* table=reinterpret_cast<u32*>(modelTable);
    u32 savedTable[kObjs]{};
    for(u32 i=0;i<kObjs;++i)savedTable[i]=table[modelBase+i];
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)},{"initial_timer",initialTimer},{"model_base",modelBase},
        {"model_special",json::array({miSpecial[0],miSpecial[1],miSpecial[2],miSpecial[3],miSpecial[4]})}});
    Totals totals;
    if(ready) {
        for(u32 i=0;i<kObjs;++i)table[modelBase+i]=Self(W.mi[i]);
        for(u32 fn=0;fn<kFns;++fn) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)RunCase(env,totals,fn,c);
            Write({{"event","function_summary"},{"function",kFn[fn]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    for(u32 i=0;i<kObjs;++i)table[modelBase+i]=savedTable[i];
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    bool tableNow=true;for(u32 i=0;i<kObjs;++i)tableNow&=table[modelBase+i]==savedTable[i];
    const bool timerNow=*reinterpret_cast<u32*>(kTimerAddr)==initialTimer;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"model_table_restored",tableNow},{"timer_restored",timerNow}});
    Write({{"event",ready&&totals.passed&&stateRestored&&tableNow&&timerNow?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"model_table_restored",tableNow},{"timer_restored",timerNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
