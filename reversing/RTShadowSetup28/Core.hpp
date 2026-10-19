#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco RTShadowSetup28. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/FileLoaderLoad2dEffect27 y reversing/DecisionAdd27.
//
// Banco par a par de UNA funcion: CRealTimeShadow::SetupForThisEntity (0x706520, thiscall, ECX = this, 1 argumento CPhysical*, `ret 4`, devuelve bool en AL).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. En ambos lados se sustituyen, SOLO durante la llamada y con el MISMO falso,
// los seis colaboradores (parches de entrada en la direccion del EXE Y en el cuerpo de la DLL, calculado ANTES de parchear):
//   CRealTimeShadow::Destroy (0x705990)  _rpAtomicResyncInterpolatedSphere (0x7491F0)  RpClumpGetBoundingSphere (0x734FC0)  RwV3dTransformPoints (0x7EDD90)
//   CShadowCamera::SetFrustum (0x7054C0)  CShadowCamera::SetCenter (0x705590)
// Los falsos dejan traza y modifican la memoria de forma determinista (ver fixture_oracle.py) para que el ORDEN y los argumentos de las llamadas se comprueben.
// NO hay colaboradores reales: lo unico que se ejecuta de verdad en cada lado es el cuerpo de SetupForThisEntity (EXE: x87 24 bits para radio*1,1f; DLL: SSE).
// La palabra de control x87 se fija a 24 bits, al mas cercano y con TODAS las excepciones enmascaradas (estado del juego). No se toca ninguna global del juego.
// EAX/ECX/EDX de salida: solo se registran (ningun llamador los lee: ver README.md); el valor devuelto (AL) SI se compara.
namespace RTShadow28Live {
using namespace RTShadowFix;
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
// La funcion usa x87 en el EXE (fld/fmul/fstp del radio) y SSE en la DLL: los registros ST vacios guardan restos distintos, la palabra de estado lleva
// banderas de excepcion y codigos de condicion distintos y los punteros de instruccion/operando difieren. Se compara lo observable:
//   FCW completa (modo de precision/redondeo/mascaras), TOP de la pila (bits 11-13 de FSW), la palabra de etiquetas y los ST VALIDOS (ninguno debe quedar: pila vacia).
// Se ignoran: banderas de excepcion/condicion de FSW, FIP/FCS/FOP/FDP/FDS y el contenido de los registros marcados como vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md): por defecto solo se registran en original_residue/candidate_residue.
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
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile;};
// Igual que DecisionMakeDecision27::Gate: siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`.
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},outFlags{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov ebpBefore,ebp
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
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter};
}
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}


// ------------------------------------------------------------------------------------------------ memoria de los casos
alignas(16) inline std::array<u8,kShadowArena> shadowArena{};   // guarda | CRealTimeShadow (0x4C) | guarda
alignas(16) inline std::array<u8,kOwnerArena> ownerArena{};     // guarda | CPhysical falso (solo se lee +0x18) | guarda
alignas(16) inline std::array<u8,kObjArena> objArena{};         // guarda | RpAtomic/RpClump falso | guarda
alignas(16) inline std::array<u8,kFrameArena> frameArena{};     // guarda | 3 RwFrame falsos | guarda
inline constexpr u32 kOwnerLabel=0x0A110000u;                   // etiqueta del puntero a CPhysical guardado en m_pOwner

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
struct Ev {u32 k,a,b,c,d,e;};
inline std::array<Ev,32> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c,d,e};++nev;}}
inline u32 Lab(const void* p,const u8* base,size_t size){const u8* q=static_cast<const u8*>(p);return (q>=base&&q<base+size)?u32(q-base):u32(uintptr_t(p));}
inline u32 R32(const void* p,size_t at){u32 v{};std::memcpy(&v,static_cast<const u8*>(p)+at,4);return v;}
inline void W32(void* p,size_t at,u32 v){std::memcpy(static_cast<u8*>(p)+at,&v,4);}

// Falsos. Destroy y SetFrustum/SetCenter son thiscall (ECX = this; __fastcall con parametros de relleno lo reproduce; SetFrustum/SetCenter: un argumento en pila, `ret 4`).
// El argumento float de SetFrustum se lee como u32 (bits en bruto de la pila).
inline void __fastcall FakeDestroy(void* self,void*){Push(6,Lab(self,shadowArena.data(),shadowArena.size()));}
inline void __cdecl FakeResync(void* atomic){
    Push(1,Lab(atomic,objArena.data(),objArena.size()),R32(atomic,0x4C));
    for(u32 i=0;i<4;++i)W32(atomic,0x1C+4*i,R32(atomic,0x1C+4*i)^(0x13579BDFu+i*0x01010101u));
}
inline void* __cdecl FakeClumpBS(void* clump,void* sphere,u32 flag){
    Push(2,Lab(clump,objArena.data(),objArena.size()),Lab(sphere,shadowArena.data(),shadowArena.size()),flag);
    for(u32 i=0;i<4;++i)W32(sphere,4*i,R32(clump,0x1C+4*i)^(0x2468ACE0u+i*0x00FF00FFu));
    return clump;
}
inline void* __cdecl FakeTransform(void* out,const void* in,i32 count,const void* mat){
    Push(3,Lab(out,shadowArena.data(),shadowArena.size()),Lab(in,shadowArena.data(),shadowArena.size()),u32(count),Lab(mat,frameArena.data(),frameArena.size()));
    const u8* m=static_cast<const u8*>(mat);
    const bool inArena=m>=frameArena.data()&&m+0x40<=frameArena.data()+frameArena.size();
    for(i32 j=0;j<count;++j){
        for(u32 k=0;k<3;++k){
            const u32 M=inArena?R32(m,0x30+4*k):u32(uintptr_t(m))+k*0x1111u;
            const u32 v=R32(in,12*size_t(j)+4*k)^M^0x3C6EF372u;
            W32(out,12*size_t(j)+4*k,v);
        }
    }
    return out;
}
inline void __fastcall FakeSetFrustum(void* self,void*,u32 radiusBits){Push(4,Lab(self,shadowArena.data(),shadowArena.size()),radiusBits);}
inline void __fastcall FakeSetCenter(void* self,void*,const void* center){
    Push(5,Lab(self,shadowArena.data(),shadowArena.size()),Lab(center,shadowArena.data(),shadowArena.size()),R32(center,0),R32(center,4),R32(center,8));
}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x706520, las seis entradas parcheadas y la constante 1,1f (0x858F14): deben seguir intactas al volver
    Pair pDestroy{},pResync{},pClump{},pXform{},pFrustum{},pCenter{};   // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,24> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){
    p.Add(e.pDestroy,&FakeDestroy);p.Add(e.pResync,&FakeResync);p.Add(e.pClump,&FakeClumpBS);p.Add(e.pXform,&FakeTransform);p.Add(e.pFrustum,&FakeSetFrustum);p.Add(e.pCenter,&FakeSetCenter);
}

// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los parches de los colaboradores se instalan despues de
// comprobar la ruta y se quitan antes de volver.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        {
            Patches patches;
            InstallPatches(env,patches);
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK}};
}
inline json Finish(Env& env,Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}

// ------------------------------------------------------------------------------------------------ casos
inline json InputOf(const Fx& f) {
    return {{"group",f.group},{"family",f.family},{"obj_present",f.objPresent},{"object",HexBytes(f.object.data(),f.object.size())},{"parent_label",f.parentLabel},
            {"frame_pos",std::vector<u32>(f.framePos.begin(),f.framePos.end())},{"pre",HexBytes(f.pre.data(),f.pre.size())},{"owner_seed",f.ownerSeed},{"guard_seed",f.guardSeed},
            {"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseSetup(Env& env,Totals& t,unsigned n) {
    const Fx f=Make(n);
    const u32 ownerPtr=u32(uintptr_t(ownerArena.data()+kOwnerAt));
    const u32 objPtr=u32(uintptr_t(objArena.data()+kObjAt));
    const u32 parentPtr=f.parentLabel==0?0u:f.parentLabel==4?kBogusParent:u32(uintptr_t(frameArena.data()+kFrameAt+(f.parentLabel-1)*kFrameBytes));
    // Referencias (se reconstruyen para cada lado: los falsos mezclan memoria y la funcion escribe la sombra)
    std::array<u8,kShadowArena> shadowRef{};std::array<u8,kOwnerArena> ownerRef{};std::array<u8,kObjArena> objRef{};std::array<u8,kFrameArena> frameRef{};
    FillWords(shadowRef.data(),shadowRef.size(),f.guardSeed);std::memcpy(shadowRef.data()+kShadowAt,f.pre.data(),kShadowBytes);
    FillWords(ownerRef.data(),ownerRef.size(),f.ownerSeed);W32(ownerRef.data(),kOwnerAt+0x18,f.objPresent?objPtr:0u);
    FillWords(objRef.data(),objRef.size(),f.guardSeed^0x0BADF00Du);std::memcpy(objRef.data()+kObjAt,f.object.data(),kObjBytes);W32(objRef.data(),kObjAt+4,parentPtr);
    FillWords(frameRef.data(),frameRef.size(),f.ownerSeed^0x0F4A3E5u);
    for(u32 i=0;i<kNumFrames;++i)for(u32 k=0;k<3;++k)W32(frameRef.data(),kFrameAt+i*kFrameBytes+0x10+0x30+4*k,f.framePos[3*i+k]);
    const json input=InputOf(f);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s){
        const bool candidate=s==1;
        PrepareFloatingState();
        shadowArena=shadowRef;ownerArena=ownerRef;objArena=objRef;frameArena=frameRef;
        nev=0;
        sides[s]=Execute(env,*env.route,0x706520,candidate,[&](uintptr_t entry){
            const u32 args[1]{ownerPtr};
            return Gate(entry,args,1,f.eax,u32(uintptr_t(shadowArena.data()+kShadowAt)),f.edx,f.stack,0);   // thiscall: ECX = this; la funcion retira el argumento (`ret 4`)
        });
        // Observado: la sombra con m_pOwner como etiqueta; el objeto con el puntero al padre como etiqueta
        std::array<u8,kShadowBytes> sh{};std::memcpy(sh.data(),shadowArena.data()+kShadowAt,kShadowBytes);
        if(R32(sh.data(),0)==ownerPtr)W32(sh.data(),0,kOwnerLabel);
        std::array<u8,kObjBytes> ob{};std::memcpy(ob.data(),objArena.data()+kObjAt,kObjBytes);
        W32(ob.data(),4,R32(ob.data(),4)==parentPtr?f.parentLabel:0xFFFFFFFFu);
        auto outsideEqual=[](const auto& now,const auto& ref,u32 at,u32 len){for(size_t i=0;i<now.size();++i){if(i>=at&&i<at+len)continue;if(now[i]!=ref[i])return false;}return true;};
        json trace=json::array();
        for(unsigned i=0;i<nev;++i)trace.push_back(json::array({evs[i].k,evs[i].a,evs[i].b,evs[i].c,evs[i].d,evs[i].e}));
        json observed={{"ret",sides[s].reg.eax&0xFFu},{"shadow",HexBytes(sh.data(),sh.size())},{"object",HexBytes(ob.data(),ob.size())},{"trace",trace},
                       {"guards_ok",outsideEqual(shadowArena,shadowRef,kShadowAt,kShadowBytes)&&outsideEqual(objArena,objRef,kObjAt,kObjBytes)},
                       {"owner_ok",ownerArena==ownerRef},{"frames_ok",frameArena==frameRef}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);
        out["observed"]=observed;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CRealTimeShadow::SetupForThisEntity",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x706520),uintptr_t(0x705990),uintptr_t(0x7491F0),uintptr_t(0x734FC0),uintptr_t(0x7EDD90),uintptr_t(0x7054C0),uintptr_t(0x705590)})env.watches.emplace_back(at,16);
    env.watches.emplace_back(0x858F14,4);                       // 1,1f
    json watched=json::array();
    for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json stubbed=json::array();
    for(const Pair* p:{&env.pDestroy,&env.pResync,&env.pClump,&env.pXform,&env.pFrustum,&env.pCenter})stubbed.push_back({{"exe",p->exe},{"dll",p->dll}});
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},{"cases",{{"CRealTimeShadow::SetupForThisEntity",kCases}}},
           {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
           {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
           {"collaborators",{{"exe_real",json::array()},{"stubbed",stubbed}}},{"mul_constant_bits",R32(reinterpret_cast<void*>(uintptr_t(0x858F14)),0)},
           {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready){
        for(unsigned c=0;c<kCases;++c)CaseSetup(env,totals,c);
        Write({{"event","function_summary"},{"function","CRealTimeShadow::SetupForThisEntity"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
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
