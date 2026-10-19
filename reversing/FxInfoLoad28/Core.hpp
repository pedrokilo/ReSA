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

// Nucleo del banco FxInfoLoad28. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/FileLoaderLoad2dEffect27 (Core.hpp) y reversing/DecisionAdd27.
//
// Banco par a par de UNA funcion: FxInfoManager_c::Load (0x5C0B70, thiscall: ECX = this, 2 argumentos en pila (FILESTREAM file, int32 version), `ret 8`).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. En ambos lados se sustituyen, SOLO durante la llamada y con el MISMO falso,
// los colaboradores (parches de entrada en la direccion del EXE Y en el cuerpo de la DLL, calculado ANTES de parchear):
//   ReadLine (0x4AB250, jmp a CFileMgr::ReadLine; en la DLL: FxTools.cpp::ReadLine) -> sirve el flujo sintetico con la semantica de fgets(buf, 256, f)    (cdecl)
//   FxMemoryPool_c::GetMem (0x4A9CA0, thiscall, 2 args, `ret 8`)                    -> devuelve la arena de punteros del banco                             (thiscall)
//   FxInfoManager_c::AddFxInfo (0x4A7B00, thiscall, 1 arg, `ret 4`)                 -> devuelve un FxInfo falso (24 bytes) con vtable falsa                (thiscall)
//   FxInfo_c::Load virtual (vtable +4, thiscall, 2 args, `ret 8`)                   -> falso: lee `plan.body` lineas con el mismo falso de ReadLine, puede cambiar m_nType (plan.retype) y escribe 0x77 en el byte 6
// NO se sustituye (se ejecuta de verdad en cada lado): sscanf, que SIEMPRE es el del EXE (0x8220AD): la DLL lo llama por direccion, asi no hay diferencias de CRT.
// FILESTREAM es un puntero-testigo (nunca se desreferencia: el falso solo comprueba que le llega el mismo). EAX/ECX/EDX de salida: solo se registran.
namespace FxInfoLoad28Live {
using namespace FxInfoLoad28Fix;
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
// El EXE compara la version con `fild version ; fcomp qword [0.7]` (x87) y la DLL con SSE2: los registros ST vacios guardan restos distintos, la palabra de estado lleva
// banderas de excepcion/condicion distintas y los punteros de instruccion/operando difieren. Se compara lo observable:
//   FCW completa (modo de precision/redondeo/mascaras), TOP de la pila (bits 11-13 de FSW), la palabra de etiquetas y los ST VALIDOS (ninguno debe quedar: pila vacia).
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. El unico llamador del EXE (0x5C2403) no los lee (ver README.md): por defecto solo se registran.
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
alignas(16) inline std::array<u8,kMgrArena> mgrArena{};                                   // guarda | objeto FxInfoManager_c (0x14 de 0x20 reservados) | guarda
alignas(16) inline std::array<u8,kPtrArena> ptrArena{};                                   // guarda | 320 ranuras de punteros (m_pInfos) | guarda
alignas(16) inline std::array<std::array<u8,kInfoSize>,kPoolInfos> pool{};                // FxInfo falsos (24 bytes, como en la arena FX)
alignas(16) inline std::array<u8,kInfoSize> junkInfo{};                                   // FxInfo al que apuntan las ranuras sin inicializar (nombres desconocidos)
alignas(16) inline std::array<u8,kStreamArena> streamArena{};                             // texto del flujo + NUL + ceros
alignas(16) inline std::array<u8,16> fileBlock{};                                         // FILESTREAM testigo (nunca se desreferencia)
inline u32 streamLen{};
inline u32 cursor{};
inline u32 created{};
inline std::array<u8,kPoolInfos> planBody{};
inline std::array<u16,kPoolInfos> planRetype{};
inline void* const kFile=fileBlock.data();
inline constexpr uintptr_t kPoolThis=0xA9AF2C;                                            // FxManager_c::m_Pool en el EXE (g_fxMan = 0xA9AEF8 + 0x34)
inline u8* MgrObj(){return mgrArena.data()+kGuard;}

// ---- traza sin reservas: se resume en longitud + hash FNV-1a 64 (sobre kind,a,b,c de cada evento, little endian) + primeros 24 y ultimos 8 eventos
struct Ev {u32 kind,a,b,c;};
inline std::array<Ev,24> evHead{};
inline std::array<Ev,8> evTail{};
inline u32 nev{};
inline u64 evHash{};
inline void TraceReset(){nev=0;evHash=1469598103934665603ull;}
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0){
    const Ev e{k,a,b,c};
    if(nev<evHead.size())evHead[nev]=e;
    evTail[nev%evTail.size()]=e;
    for(const u32 w:{k,a,b,c})for(int s=0;s<32;s+=8){evHash^=u8(w>>s);evHash*=1099511628211ull;}
    ++nev;
}
inline json EvJson(const Ev& e){return json::array({e.kind,e.a,e.b,e.c});}

// Falsos. Los thiscall se declaran __fastcall (ECX = this, EDX ignorado, resto en pila y retirados por el callee: `ret 4`/`ret 8` coinciden con los del EXE).
inline u8 fakeVtbl[3*4]{};     // [0] destructor (nunca se llama), [1] Load, [2] GetValue (nunca se llama): se rellena en InitFakes
inline bool __cdecl FakeReadLine(void* file,char* buf,i32 size){
    const u32 before=cursor;
    bool ok=false;
    if(size>0&&cursor<streamLen){
        u32 take=std::min<u32>(u32(size)-1,streamLen-cursor);
        for(u32 i=0;i<take;++i)if(streamArena[cursor+i]=='\n'){take=i+1;break;}
        std::memcpy(buf,streamArena.data()+cursor,take);
        buf[take]=0;
        cursor+=take;
        ok=true;
    }
    Push(1,before,cursor,u32(size)|(file==kFile?1u<<16:0u)|(ok?1u<<17:0u));
    return ok;
}
inline void* __fastcall FakeGetMem(void* self,void*,i32 size,i32 align){
    Push(2,u32(size),u32(align),uintptr_t(self)==kPoolThis?1u:0u);
    return ptrArena.data()+kGuard;
}
inline u32 PoolLabel(const void* p){
    const u8* q=static_cast<const u8*>(p);
    const u8* b=pool.data()->data();
    if(q>=b&&q<b+sizeof(pool)&&(q-b)%kInfoSize==0)return u32((q-b)/kInfoSize);
    return 0xFFFFu;
}
inline void* __fastcall FakeAddFx(void* self,void*,i32 type){
    const u32 ord=created;
    Push(3,self==MgrObj()?1u:0u,u32(type),ord);
    if(ord>=kPoolInfos)return junkInfo.data();      // no ocurre con los fixtures validos
    u8* o=pool[ord].data();
    const u32 vp=u32(uintptr_t(fakeVtbl));
    std::memcpy(o,&vp,4);
    const u16 t=u16(type);
    std::memcpy(o+4,&t,2);
    o[6]=0;
    ++created;
    return o;
}
inline void __fastcall FakeInfoLoad(void* self,void*,void* file,i32 version){
    u8* o=static_cast<u8*>(self);
    const u32 ord=PoolLabel(o);
    u16 before{};
    std::memcpy(&before,o+4,2);
    Push(4,ord,u32(version),(file==kFile?1u:0u)|(u32(before)<<1));
    const unsigned body=ord<kPoolInfos?planBody[ord]:0u;
    char scratch[256];
    for(unsigned k=0;k<body;++k)FakeReadLine(file,scratch,256);
    if(ord<kPoolInfos&&planRetype[ord]){const u16 rt=planRetype[ord];std::memcpy(o+4,&rt,2);}
    o[6]=0x77;                                      // el manager debe sobrescribirlo DESPUES del Load
}
inline void InitFakes(){
    const u32 load=u32(uintptr_t(&FakeInfoLoad));
    std::memcpy(fakeVtbl+4,&load,4);
}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x5C0B70, 0x8220AD y las tres entradas parcheadas: deben seguir siendo el codigo de partida al volver
    Pair pRead{},pGetMem{},pAddFx{};     // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    uintptr_t poolThis{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,8> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){
    p.Add(e.pRead,&FakeReadLine);p.Add(e.pGetMem,&FakeGetMem);p.Add(e.pAddFx,&FakeAddFx);
}

// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). `exeOnly`: direcciones del EXE que no pueden saltar a la DLL
// (sscanf del EXE: se ejecuta de verdad). Los parches de los colaboradores se instalan despues de comprobar la ruta y se quitan antes de volver.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,std::initializer_list<uintptr_t> exeOnly,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        bool ok=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        for(const uintptr_t a:exeOnly)ok&=!ReversingProbe::JumpsToDll(a);
        side.routeOK=ok;
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
    json body=json::array(),retype=json::array();
    for(const auto& p:f.plan){body.push_back(p.body);retype.push_back(p.retype);}
    return {{"kind",f.kind},{"sub",f.sub},{"stream",f.stream},{"version",f.version},{"pre",HexBytes(f.pre.data(),kMgrSize)},{"junk_type",f.junkType},{"junk_b6",f.junkB6},
            {"plan_body",body},{"plan_retype",retype},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack},{"count",f.count}};
}
// Estado de partida (referencia) de un caso: se restablece antes de cada lado y se compara al terminar.
struct Refs {
    std::array<u8,kMgrArena> mgr{};std::array<u8,kPtrArena> ptr{};std::array<std::array<u8,kInfoSize>,kPoolInfos> pool{};std::array<u8,kInfoSize> junk{};std::array<u8,kStreamArena> stream{};
};
inline void FillRef(const Fx& f,unsigned n,Refs& r){
    FillWords(r.mgr.data(),r.mgr.size(),n*40503u+11u);
    std::memcpy(r.mgr.data()+kGuard,f.pre.data(),kMgrSize);
    FillWords(r.ptr.data(),r.ptr.size(),n*40503u+13u);
    const u32 jp=u32(uintptr_t(junkInfo.data()));
    for(u32 i=0;i<kMaxSlots;++i)std::memcpy(r.ptr.data()+kGuard+4*i,&jp,4);
    for(u32 j=0;j<kPoolInfos;++j)FillWords(r.pool[j].data(),kInfoSize,n*40503u+100u+j);
    FillWords(r.junk.data(),kInfoSize,n*40503u+17u);
    std::memcpy(r.junk.data()+4,&f.junkType,2);
    r.junk[6]=f.junkB6;
    r.stream.fill(0);
    std::memcpy(r.stream.data(),f.stream.data(),f.stream.size());
}
inline void ResetCase(const Fx& f,const Refs& r){
    mgrArena=r.mgr;ptrArena=r.ptr;pool=r.pool;junkInfo=r.junk;streamArena=r.stream;
    streamLen=u32(f.stream.size());cursor=0;created=0;TraceReset();
    planBody.fill(0);planRetype.fill(0);
    for(size_t i=0;i<f.plan.size()&&i<kPoolInfos;++i){planBody[i]=f.plan[i].body;planRetype[i]=f.plan[i].retype;}
}
// Lo observable tras la llamada (se serializa igual para el EXE, la DLL y el oraculo).
inline json Observe(const Refs& r){
    std::array<u8,kMgrSize> mgr{};
    std::memcpy(mgr.data(),MgrObj(),kMgrSize);
    u32 pinfosPtr{};std::memcpy(&pinfosPtr,mgr.data()+4,4);
    std::memset(mgr.data()+4,0,4);                                      // m_pInfos se compara como desplazamiento en la arena de punteros
    i32 count{};std::memcpy(&count,mgr.data(),4);
    bool guardOk=true;
    for(u32 i=0;i<kMgrArena;++i)if(!(i>=kGuard&&i<kGuard+kMgrSize)&&mgrArena[i]!=r.mgr[i])guardOk=false;
    const u32 cnt=count>0?std::min<u32>(u32(count),kMaxSlots):0u;
    json slots=json::array();
    for(u32 i=0;i<cnt;++i){
        u32 p{};std::memcpy(&p,ptrArena.data()+kGuard+4*i,4);
        const u32 lab=PoolLabel(reinterpret_cast<void*>(uintptr_t(p)));
        slots.push_back(lab!=0xFFFFu?lab:(p==u32(uintptr_t(junkInfo.data()))?0x10000u:0x20000u));
    }
    bool ptrOutside=true;
    for(u32 i=0;i<kPtrArena;++i)if(!(i>=kGuard&&i<kGuard+4*cnt)&&ptrArena[i]!=r.ptr[i])ptrOutside=false;
    json objs=json::array();
    const u32 vp=u32(uintptr_t(fakeVtbl));
    for(u32 j=0;j<created&&j<kPoolInfos;++j){
        u32 v{};std::memcpy(&v,pool[j].data(),4);
        u16 ty{};std::memcpy(&ty,pool[j].data()+4,2);
        const bool rest=v==vp&&std::memcmp(pool[j].data()+7,r.pool[j].data()+7,kInfoSize-7)==0;
        objs.push_back(json::array({ty,pool[j][6],rest}));
    }
    bool poolOutside=created<=kPoolInfos;
    for(u32 j=created;j<kPoolInfos;++j)if(pool[j]!=r.pool[j])poolOutside=false;
    u16 jt{};std::memcpy(&jt,junkInfo.data()+4,2);
    const bool junkRest=std::memcmp(junkInfo.data(),r.junk.data(),4)==0&&std::memcmp(junkInfo.data()+7,r.junk.data()+7,kInfoSize-7)==0;
    json head=json::array(),tail=json::array();
    for(u32 i=0;i<nev&&i<evHead.size();++i)head.push_back(EvJson(evHead[i]));
    for(u32 i=nev>evTail.size()?nev-u32(evTail.size()):0u;i<nev;++i)tail.push_back(EvJson(evTail[i%evTail.size()]));
    char hash[17];std::snprintf(hash,sizeof hash,"%016llx",static_cast<unsigned long long>(evHash));
    return {{"mgr",HexBytes(mgr.data(),kMgrSize)},{"pinfos",i32(pinfosPtr-u32(uintptr_t(ptrArena.data())))},{"guard_ok",guardOk},{"slots",slots},{"ptr_outside_ok",ptrOutside},
            {"objs",objs},{"pool_outside_ok",poolOutside},{"junk",json::array({jt,junkInfo[6],junkRest})},{"stream_ok",streamArena==r.stream},{"cursor",cursor},
            {"trace_len",nev},{"trace_hash",std::string(hash)},{"trace_head",head},{"trace_tail",tail}};
}
inline void CaseLoad(Env& env,Totals& t,unsigned n){
    const Fx f=Make(n);
    const bool fits=f.stream.size()+1<=kStreamMax&&f.count<=kMaxSlots&&f.plan.size()<=kPoolInfos;
    Refs refs;
    FillRef(f,n,refs);
    const json input=InputOf(f);
    Side sides[2];
    json rows[2];
    for(int s=0;s<2;++s){
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetCase(f,refs);
        if(fits){
            sides[s]=Execute(env,*env.route,0x5C0B70,{uintptr_t(0x8220AD)},candidate,[&](uintptr_t entry){
                const u32 args[2]{u32(uintptr_t(kFile)),u32(f.version)};
                return Gate(entry,args,2,f.eax,u32(uintptr_t(MgrObj())),f.edx,f.stack,0);      // thiscall con `ret 8`: el callee retira los argumentos
            });
        }else{   // fixture inseguro: no se llama (queda registrado y falla)
            sides[s].reg={0,0,0,0,true,true};
            SaveFP(sides[s].fp.data());sides[s].mxcsr=ReadMXCSR();
        }
        json observed=Observe(refs);
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);
        out["observed"]=observed;
        if(!fits)out["route_ok"]=false;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"FxInfoManager_c::Load",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x5C0B70),uintptr_t(0x8220AD),uintptr_t(0x4AB250),uintptr_t(0x4A9CA0),uintptr_t(0x4A7B00)})env.watches.emplace_back(at,16);
    json watched=json::array();
    for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    PrepareFloatingState();
    InitFakes();
    RestoreFloatingState();
    json stubbed=json::array();
    for(const Pair* p:{&env.pRead,&env.pGetMem,&env.pAddFx})stubbed.push_back({{"exe",p->exe},{"dll",p->dll}});
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},{"cases",{{"FxInfoManager_c::Load",kCases}}},
           {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
           {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
           {"collaborators",{{"exe_real",std::vector<uintptr_t>{0x8220AD}},{"stubbed",stubbed},{"virtual_load_fake",true}}},
           {"crt",{{"exe_sscanf",0x8220AD},{"exe_sscanf_in_game_image",ModuleOf(Resolve(0x8220AD))==env.game},{"dll_sscanf","no se usa: la DLL llama al 0x8220AD del EXE"}}},
           {"mempool_this",env.poolThis},{"mempool_this_expected",kPoolThis},
           {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready){
        for(unsigned c=0;c<kCases;++c)CaseLoad(env,totals,c);
        Write({{"event","function_summary"},{"function","FxInfoManager_c::Load"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
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
