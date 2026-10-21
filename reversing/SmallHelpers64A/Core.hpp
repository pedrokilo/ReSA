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
// Nucleo del banco SmallHelpers64A (lote 64). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute. Plantilla: reversing/WorldScan64 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).
//
// Banco par a par de CUATRO funciones sueltas: GetMillisecondTime (0x5617C0), DIReleaseMouse (0x746F70), RemoveUpgradeCB (0x6D3300) y FindUpgradeCB (0x6D3370).
// Original = bytes del EXE con el hook RH de la funcion apagado; candidato = DLL con SOLO ese hook encendido.
// Colaboradores sustituidos por falsos identicos en ambos lados: RsTimer (0x619410); GetAtomicId (0x732370), GetModelInfo (0x732260), RpClumpRemoveAtomic (0x74A4C0), RpAtomicDestroy (0x749DC0),
// RwFrameDestroy (0x7F05A0), CBaseModelInfo::RemoveRef (0x4C4BB0). DIReleaseMouse: el dispositivo DirectInput es un objeto COM FALSO de la arena (tabla virtual propia) y RsGlobal.ps
// apunta a estructuras psGlobal falsas mientras dura la llamada; el dispositivo real y su psGlobal no se tocan.
// Registros de salida: GetMillisecondTime devuelve uint64 en EDX:EAX y sus llamadores del EXE leen ambos (CTimer::Suspend/Resume: 0x5619EF/0x5619F4, 0x561A17/0x561A1D): se EXIGEN EAX y EDX.
// RemoveUpgradeCB/FindUpgradeCB: el unico llamador (RpClumpForAllAtomics 0x749B70) solo lee EAX (test eax,eax): se exige EAX; ECX/EDX solo registrados.
// DIReleaseMouse: ningun llamador (0x576CCE, 0x576EBC, 0x576F8C) lee EAX/ECX/EDX: solo registrados.

namespace SH64Live {
using namespace SH64Fix;
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
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Aqui NO: lo que se exige (EAX/EDX de GetMillisecondTime, EAX de los callbacks) va en `observed`; el resto solo se registra.
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
struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: sin RH_ScopedInstall activo (la entrada del EXE sigue siendo codigo original); dll=0: la DLL llama a la direccion del EXE
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
struct StubSpec {Pair pair;void* fake;};
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> p;
    Patches(){p.reserve(32);}
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    void Reset(){while(!p.empty())p.pop_back();}   // orden inverso al de instalacion
    ~Patches(){Reset();}
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

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los colaboradores sustituidos tienen su hook apagado: su entrada del EXE debe seguir
// siendo codigo original en ambos lados; los falsos se instalan DESPUES de comprobar rutas y colaboradores y solo mientras dura la llamada.
// Los parches de falsos se quitan (Reset) ANTES de leer el estado final: `after` corre ya sin parches.
template<class Fn,class After> Side Execute(Env& env,unsigned fnIdx,uintptr_t entry,bool candidate,Fn gate,After after) {
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
            patches.Reset();
        }
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        after();
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

// ================================================================================================ arena, falsos, fixtures y casos
inline constexpr const char* kFn[kFns]{"GetMillisecondTime","DIReleaseMouse","RemoveUpgradeCB","FindUpgradeCB"};
inline constexpr uintptr_t kEntry[kFns]{0x5617C0,0x746F70,0x6D3300,0x6D3370};
inline constexpr uintptr_t kRsTimer=0x619410,kGetAtomicId=0x732370,kGetModelInfo=0x732260,kClumpRemoveAtomic=0x74A4C0,kAtomicDestroy=0x749DC0,kFrameDestroy=0x7F05A0,kRemoveRef=0x4C4BB0;
inline constexpr uintptr_t kRsGlobalPs=0xC17054;   // RsGlobal.ps (RsGlobal = 0xC17040, +0x14)

alignas(16) inline std::array<u32,kArenaWords> Arena{};
alignas(16) inline std::array<u32,kArenaWords> ArenaInit{};
inline u32 AB(){return u32(uintptr_t(Arena.data()));}
inline u32 A(u32 off){return AB()+off;}
inline u64 Lab(u32 v){return (v>=AB()&&v<AB()+kArenaSize)?kLabel+(v-AB()):u64(v);}
inline u64 Lab(const void* p){return Lab(u32(uintptr_t(p)));}
inline u32& W(u32 off){return Arena[off/4];}
inline u32& RsPs(){return *reinterpret_cast<u32*>(kRsGlobalPs);}

// ---- eventos de los falsos (nada aqui asigna memoria: se usan dentro de la ventana de parches)
// 1 GetAtomicId(atomo)->banderas; 2 GetModelInfo(atomo)->modelo; 3 RpClumpRemoveAtomic(clump, atomo)->ret; 4 RpAtomicDestroy(atomo)->ret; 5 RwFrameDestroy(marco)->ret;
// 6 RemoveRef(this); 7 RsTimer()->ret; 8 Unacquire(dispositivo)->ret; 9 Release(dispositivo)->ret; 10 ranura de la tabla virtual inesperada
struct Ev {u32 kind{};u64 a{};u64 b{};u64 c{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline void Push(u32 kind,u64 a,u64 b,u64 c){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{kind,a,b,c};++nev;}
inline Spec cur{};
inline u32 gmiCalls{};

inline u32 __cdecl FkRsTimer(){Push(7,cur.v[0],0,0);return cur.v[0];}
inline u32 __cdecl FkGetAtomicId(void* atomic){const u32 r=cur.v[0];Push(1,Lab(atomic),r,0);return r;}
inline void* __cdecl FkGetModelInfo(void* atomic){
    const u32 idx=gmiCalls++;
    const u32 ret=idx==0?A(oMi1):(cur.v[3]==0?0u:(cur.v[3]==1?A(oMi2):A(oMi1)));
    Push(2,Lab(atomic),Lab(ret),0);
    return reinterpret_cast<void*>(ret);
}
inline void* __cdecl FkClumpRemove(void* clump,void* atomic){Push(3,Lab(clump),Lab(atomic),cur.v[6]);return reinterpret_cast<void*>(cur.v[6]);}
inline int __cdecl FkAtomicDestroy(void* atomic){Push(4,Lab(atomic),cur.v[7],0);return int(cur.v[7]);}
inline int __cdecl FkFrameDestroy(void* frame){Push(5,Lab(frame),cur.v[8],0);return int(cur.v[8]);}
inline void __fastcall FkRemoveRef(void* self,void*){Push(6,Lab(self),0,0);}

// ---- dispositivo DirectInput falso: tabla virtual propia (ranura 2 = Release +0x08, ranura 8 = Unacquire +0x20)
inline u32 DevId(const void* p){const u32 a=u32(uintptr_t(p));return a==A(oDevA)?1:a==A(oDevB)?2:a==A(oDevC)?3:0xFFFFu;}
inline void ApplyAction(u32 act){
    const u32 ps=RsPs();
    switch(act) {
    case 1: case 2: case 3: *reinterpret_cast<u32*>(ps+kPsMouse)=A(oDevA+0x20*(act-1)); break;
    case 4: RsPs()=A(oPs2); break;
    case 5: RsPs()=A(oPs1); break;
    case 6: *reinterpret_cast<u32*>(ps+kPsMouse)=0; break;
    default: break;
    }
}
inline u32 __stdcall FkUnacquire(void* self){
    const u32 d=DevId(self);
    if(d<1||d>3){Push(10,Lab(self),0,0);return 0x80004005u;}
    const u32* p=&cur.v[kMouseV0+4*(d-1)];
    Push(8,d,p[1],0);
    ApplyAction(p[0]);
    return p[1];
}
inline u32 __stdcall FkRelease(void* self){
    const u32 d=DevId(self);
    if(d<1||d>3){Push(10,Lab(self),0,0);return 0x80004005u;}
    const u32* p=&cur.v[kMouseV0+4*(d-1)];
    Push(9,d,p[3],0);
    ApplyAction(p[2]);
    return p[3];
}
inline u32 __stdcall FkTrap(void* self){Push(10,Lab(self),0,0);return 0x80004005u;}
alignas(16) inline std::array<void*,40> Vtbl{};
inline void InitVtbl(){
    for(auto& s:Vtbl)s=reinterpret_cast<void*>(&FkTrap);
    Vtbl[2]=reinterpret_cast<void*>(&FkRelease);
    Vtbl[8]=reinterpret_cast<void*>(&FkUnacquire);
}

inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i)out.push_back(json::array({evs[i].kind,evs[i].a,evs[i].b,evs[i].c}));
    return out;
}

// Imagen inicial de la arena de un caso (relleno + campos de la funcion). Debe coincidir con lo que modela fixture_oracle.py.
inline void BuildArena(const Spec& s,unsigned n) {
    for(u32 i=0;i<kArenaWords;++i)Arena[i]=FillWord(s.fn,n,i);
    switch(s.fn) {
    case kMouse:
        for(u32 d=0;d<3;++d){W(oDevA+0x20*d)=u32(uintptr_t(Vtbl.data()));W(oDevA+0x20*d+4)=d+1;}
        W(oPs1+kPsMouse)=s.v[0]?A(oDevA+0x20*(s.v[0]-1)):0u;
        W(oPs2+kPsMouse)=s.v[1]?A(oDevA+0x20*(s.v[1]-1)):0u;
        break;
    case kRemove: case kFind: {
        W(oAtom+kAtomFrame)=s.v[5];W(oAtom+kAtomClump)=s.v[4];
        const u16 w=u16(s.v[1]);memcpy(reinterpret_cast<u8*>(Arena.data())+oMi1+kMiFlags,&w,2);
        if(s.fn==kFind){W(oData)=s.v[2];W(oData+4)=s.v[9];}
        break;
    }
    default: break;
    }
    ArenaInit=Arena;
}

inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    Side sides[2];json rows[2];
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        nev=0;gmiCalls=0;cur=s;
        BuildArena(s,n);
        // el psGlobal REAL (RsGlobal.ps) y su contenido no se tocan: se comprueba que siguen igual
        const u32 realPs=RsPs();
        const auto realPsBytes=ReversingProbe::Bytes(reinterpret_cast<void*>(realPs),0x28);
        u32 psFinal=0;
        auto gate=[&](uintptr_t entry){
            Reg r{};
            if(fn==kMouse)RsPs()=A(oPs1);
            if(fn==kTimer)r=Gate(entry,nullptr,0,s.eax,s.ecx,s.edx,s.stack,0);
            else if(fn==kMouse)r=Gate(entry,nullptr,0,s.eax,s.ecx,s.edx,s.stack,0);
            else {
                const u32 args[2]{A(oAtom),fn==kFind?A(oData):s.v[2]};
                r=Gate(entry,args,2,s.eax,s.ecx,s.edx,s.stack,8);
            }
            if(fn==kMouse){psFinal=RsPs();RsPs()=realPs;}
            return r;
        };
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,gate,[]{});
        json mem=json::array();
        for(u32 i=0;i<kArenaWords;++i)if(Arena[i]!=ArenaInit[i])mem.push_back(json::array({i,Lab(Arena[i])}));
        json obs;
        const Reg& rg=sides[sd].reg;
        if(fn==kTimer)obs["ret"]=json::array({rg.eax,rg.edx});
        else if(fn==kMouse)obs["ret"]=json::array();
        else obs["ret"]=json::array({Lab(rg.eax)});
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["mem"]=mem;obs["ps"]=fn==kMouse?Lab(psFinal):u64(0);
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        const bool realOK=RsPs()==realPs&&ReversingProbe::Bytes(reinterpret_cast<void*>(realPs),0x28)==realPsBytes;
        out["globals_restored"]=realOK&&watchesIntact&&env.WatchesIntact();
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
    const u32 realPs0=RsPs();
    const auto realPsInit=ReversingProbe::Bytes(reinterpret_cast<void*>(realPs0),0x28);
    InitVtbl();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"arena_size",kArenaSize},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(u32 fn=0;fn<kFns;++fn) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)RunCase(env,totals,fn,c);
            Write({{"event","function_summary"},{"function",kFn[fn]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool psNow=RsPs()==realPs0&&ReversingProbe::Bytes(reinterpret_cast<void*>(realPs0),0x28)==realPsInit;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"region_restored",psNow}});
    Write({{"event",ready&&totals.passed&&stateRestored&&psNow?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"region_restored",psNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
