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
// Nucleo del banco StoredCar64 (lote 64). Plantilla: reversing/GarageUpdate63 y reversing/Garage63 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).

namespace SC64Live {
using namespace SC64Fix;
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
    return kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);   // true: tambien los registros muertos (restos de las operaciones x87 del EXE)
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
inline constexpr const char* kFn[kFns]{"CStoredCar::StoreCar","CStoredCar::RestoreCar"};
inline constexpr const char* kName[kFns]{"StoreCar","RestoreCar"};
inline constexpr uintptr_t kEntry[kFns]{0x449760,0x447E40};
inline constexpr unsigned kMaxEvents = 64;

struct Arena {
    alignas(16) u32 gl[kGuardWords];
    u32 sc[kScWords];                       // CStoredCar (0x40 bytes)
    u32 g1[kGuardWords];
    alignas(16) u32 veh[2][kVehWords];      // vehiculo origen (StoreCar) / vehiculos nuevos (RestoreCar)
    u32 g2[kGuardWords];
    alignas(16) u32 mat[2][kMatWords];
    alignas(16) u32 mi[4][kMiWords];        // CVehicleModelInfo falsos de los modelos de prueba
    u32 gr[kGuardWords];
};
alignas(16) inline Arena W{},WRef{};
inline u32 Self(const void* p){return u32(uintptr_t(p));}
struct RegionRef {u32* p;u32 words;const char* name;int idx;};
inline std::array<RegionRef,kRegions> Regions(Arena& a){
    return {{{a.sc,kScWords,"S",-1},{a.veh[0],kVehWords,"V",0},{a.veh[1],kVehWords,"V",1},{a.mat[0],kMatWords,"M",0},{a.mat[1],kMatWords,"M",1},
             {a.mi[0],kMiWords,"MI",0},{a.mi[1],kMiWords,"MI",1},{a.mi[2],kMiWords,"MI",2},{a.mi[3],kMiWords,"MI",3}}};
}
inline u32 Local(u32 v){const char here=0;const auto d=intptr_t(v)-intptr_t(uintptr_t(&here));return d>-0x10000&&d<0x10000;}

// ---- fichas simbolicas de punteros: S, V0, V1, M0, M1, MI0.. (+desplazamiento en bytes), L = pila local, 0 = nulo
inline json Tok(u32 v) {
    if(v==0)return 0;
    for(const auto& r:Regions(W)) {
        const u32 b=Self(r.p);
        if(v>=b&&v<b+r.words*4) {
            std::string out=r.name;
            if(r.idx>=0)out+=std::to_string(r.idx);
            if(v!=b)out+="+"+std::to_string(v-b);
            return out;
        }
    }
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
inline u32 cnt[kKinds + 4]{};
inline u32 NextK(unsigned kind){return cnt[kind]++;}
inline u32 Junk(u32 bit){return (bit&1u)|0xA5A5A500u;}   // AL = bit, resto de EAX basura (los llamadores del EXE usan `test al,al`)

#define EV(kind,n,mask) Ev& e=PushEv(kind,n,mask)
inline u32 __fastcall FkGetRemap(u32 self,void*){EV(kGetRemap,1,1);e.v[0]=self;NextK(kGetRemap);return S.remap;}                          // 0x6D0B70
inline void __cdecl FkReqModel(u32 model,u32 flags){EV(kReqModel,2,0);e.v[0]=model;e.v[1]=flags;NextK(kReqModel);}                          // 0x4087E0
inline void __cdecl FkReqUpg(u32 model,u32 flags){EV(kReqUpg,2,0);e.v[0]=model;e.v[1]=flags;NextK(kReqUpg);}                                // 0x408C70
inline u32 __cdecl FkHasUpg(u32 model){EV(kHasUpg,1,0);e.v[0]=model;const u32 k=NextK(kHasUpg);return Junk((S.hasMask>>(k&31))&1u);}       // 0x407820
inline u32 __cdecl FkNew(u32 size){EV(kNew,1,0);e.v[0]=size;NextK(kNew);return Self(W.veh[0]);}                                           // 0x6E2D50
// constructores: thiscall (modelo, 1) ret 8 (variant 0..7) y el del coche (modelo, 1, 1) ret 0Ch (variant 8); devuelven this (o V1)
template<u32 V> inline u32 __fastcall FkCtor2(u32 self,void*,u32 model,u32 created){EV(kCtor,5,2);e.v[0]=V;e.v[1]=self;e.v[2]=model;e.v[3]=created;e.v[4]=0;NextK(kCtor);return S.ctor1?Self(W.veh[1]):self;}
inline u32 __fastcall FkCtorCar(u32 self,void*,u32 model,u32 a,u32 b){EV(kCtor,5,2);e.v[0]=8;e.v[1]=self;e.v[2]=model;e.v[3]=a;e.v[4]=b;NextK(kCtor);return S.ctor1?Self(W.veh[1]):self;}
inline void __fastcall FkSetup(u32 self,void*){EV(kSetup,1,1);e.v[0]=self;NextK(kSetup);}                                                 // 0x6E3400
inline void __fastcall FkSetRemap(u32 self,void*,u32 idx){EV(kSetRemap,2,1);e.v[0]=self;e.v[1]=idx;NextK(kSetRemap);}                       // 0x6D0C00, ret 4

// ---- estado real del EXE que leen o escriben las funciones: ms_compsToUse (0x8A6458/9) y, de cada modelo de prueba, su byte de estado de carga y su CModelInfo
struct RealWord {uintptr_t addr;unsigned size;};
inline const RealWord kReal[]{{kCompsAddr,1},{kCompsAddr+1,1},
    {kStateBase+20*kTm[0],1},{kModelPtrBase+4*kTm[0],4},{kStateBase+20*kTm[1],1},{kModelPtrBase+4*kTm[1],4},
    {kStateBase+20*kTm[2],1},{kModelPtrBase+4*kTm[2],4},{kStateBase+20*kTm[3],1},{kModelPtrBase+4*kTm[3],4}};
inline const char* const kRealName[]{"comps0","comps1","state0","mi0","state1","mi1","state2","mi2","state3","mi3"};
struct RealState {std::array<u32,std::size(kReal)> v{};};
inline u32 ReadReal(const RealWord& r){u32 v{};memcpy(&v,reinterpret_cast<void*>(r.addr),r.size);return v;}
inline void WriteReal(const RealWord& r,u32 v){memcpy(reinterpret_cast<void*>(r.addr),&v,r.size);}
inline RealState SnapReal(){RealState s;for(size_t i=0;i<std::size(kReal);++i)s.v[i]=ReadReal(kReal[i]);return s;}
inline void RestoreReal(const RealState& s){for(size_t i=0;i<std::size(kReal);++i)WriteReal(kReal[i],s.v[i]);}
inline void ApplySpecReal(const Spec& s){
    WriteReal(kReal[0],s.comps[0]);WriteReal(kReal[1],s.comps[1]);
    for(u32 k=0;k<4;++k){WriteReal(kReal[2+2*k],s.state[k]);WriteReal(kReal[3+2*k],Self(W.mi[k]));}
}
inline json RealChanges(const RealState& before,const RealState& after) {
    json out=json::array();
    for(size_t i=0;i<std::size(kReal);++i)if(before.v[i]!=after.v[i])out.push_back(json::array({kRealName[i],after.v[i]}));
    return out;
}

// ---- imagen de un caso: relleno determinista + escrituras previas del caso
inline void BuildCase(const Spec& s,unsigned n,Arena& a) {
    u32* all=reinterpret_cast<u32*>(&a);
    const u32 gs=GarbageSeed(s.fn,n);
    for(size_t i=0;i<sizeof(Arena)/4;++i)all[i]=Garbage(gs,u32(i));
    for(u32 i=0;i<kGuardWords;++i){a.gl[i]=0xA5A5A5A5u^i;a.g1[i]=0x3C3C3C3Cu^i;a.g2[i]=0xC3C3C3C3u^i;a.gr[i]=0x5A5A5A5Au^i;}
    const auto regions=Regions(a);
    for(const auto& p:s.pre) {
        u32 v=p.val;
        if(v==kMarkMat0)v=Self(W.mat[0]);else if(v==kMarkMat1)v=Self(W.mat[1]);
        memcpy(reinterpret_cast<u8*>(regions[p.region].p)+p.off,&v,p.size);   // little endian: los bytes bajos
    }
    for(u32 k=0;k<4;++k)a.mi[k][15]=s.vtype[k];   // CVehicleModelInfo +0x3C = tipo de vehiculo
}
inline json ChangesJson(const Arena& post,const Arena& ref) {
    json out=json::array();
    const u32* pa=reinterpret_cast<const u32*>(&post);const u32* ra=reinterpret_cast<const u32*>(&ref);
    Arena& postMut=const_cast<Arena&>(post);
    const auto regions=Regions(postMut);
    for(size_t i=0;i<sizeof(Arena)/4;++i) {
        if(pa[i]==ra[i])continue;
        bool found=false;
        for(const auto& r:regions) {
            const u32* base=r.p;
            if((&pa[i]>=base)&&(&pa[i]<base+r.words)) {
                json row=json::array({r.name,r.idx<0?0:r.idx,u32(&pa[i]-base),pa[i]});
                out.push_back(row);found=true;break;
            }
        }
        if(!found)out.push_back(json::array({"X",0,u32(i),pa[i]}));   // cualquier otra palabra (guardas): no deberia cambiar nunca
    }
    return out;
}

inline void RunCase(Env& env,Totals& t,unsigned fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    json input=json::parse(JsonSpec(s,n));
    BuildCase(s,n,WRef);
    {json lay=json::array();for(const auto& r:Regions(WRef))lay.push_back(u32(r.p-reinterpret_cast<u32*>(&WRef)));input["layout"]=lay;}
    Side sides[2];json rows[2];
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        S=s;memset(cnt,0,sizeof(cnt));nev=0;
        const RealState before=SnapReal();
        ApplySpecReal(s);
        const RealState applied=SnapReal();
        u32 args[1]{Self(W.veh[0])};
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t entry){
            return fn==0?Gate(entry,args,1,s.regs[1],Self(W.sc),s.regs[2],s.regs[0],0)
                        :Gate(entry,nullptr,0,s.regs[1],Self(W.sc),s.regs[2],s.regs[0],0);});
        const RealState after=SnapReal();
        RestoreReal(before);
        const bool restored=SnapReal().v==before.v;
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["changes"]=ChangesJson(W,WRef);obs["globals"]=RealChanges(applied,after);
        if(fn==1)obs["ret"]=Tok(sides[sd].reg.eax);   // RestoreCar: el vehiculo en EAX (los llamadores del EXE lo leen); StoreCar es void
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
    json cases=json::object();for(unsigned f=0;f<kFns;++f)cases[kFn[f]]=kCasesOf[f];
    const RealState initial=SnapReal();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kCasesOf[0]+kCasesOf[1]},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+st0..st7 (tambien los registros muertos); ignora FSW banderas/CC y FIP/FDP; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFns;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCasesOf[f];++c){RunCase(env,totals,f,c);}
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCasesOf[f]},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
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
