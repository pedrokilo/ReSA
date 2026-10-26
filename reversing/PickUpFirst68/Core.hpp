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
// Nucleo del banco PickUpFirst68: CTaskComplexGoPickUpEntity::CreateFirstSubTask (0x693610). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego usados en LiveComparison.hpp.
// Plantillas: reversing/PartnerNext60 (hook Virtual, falsos de operator new, constructor real, objeto reservado en arena), reversing/LeaveCar67 y reversing/Helpers66A (ABI, falsos con parche de entrada en el EXE y en la DLL).
//
// Banco par a par de UNA funcion (hook virtual: HookRoute con el cuerpo del hook Virtual). Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido.
// Sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL, resuelto ANTES de escribir ningun parche): CMatrix::SetRotateZOnly 0x59B020,
// CMatrix::UpdateRwMatrix 0x59AD70, CSimpleTransform::UpdateRwMatrix 0x54EF40 y UpdateMatrix 0x54F1B0 (los cuatro reconstruidos pero sin hook en HEAD), CPlaceable::AllocateMatrix 0x54F560, CMatrix * CVector 0x59C890 y
// CTask::operator new 0x61A5A0 (con hook). Real en ambos lados: el constructor de CTaskComplexGoToPointAndStandStill 0x668120 (construye en una arena que devuelve el falso de operator new; se compara el objeto palabra a palabra).
// SetIsStatic (vtable +0x10 de la entidad) va por la vtable falsa de arena. CTimer::m_snTimeInMilliseconds (0xB7CB84) y la entrada de ms_modelInfoPtrs del caso se fijan por caso y se restauran.

namespace PickUp68Live {
using namespace PickUp68Fix;
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
// true: EAX/ECX/EDX de salida forman parte de la igualdad. EAX es el retorno (se compara siempre, como etiqueta, en `observed.ret`). El unico llamador del EXE que importa,
// CTaskManager::AddSubTasks (0x681A53: `call [edx+24h]`, luego `mov edi,eax`), sobrescribe ECX/EDX antes de usarlos; por defecto solo se registran.
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada (la funcion instala su propio marco SEH y lo retira al volver).
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

// ------------------------------------------------------------------------------------------------ memoria de los casos
alignas(16) inline std::array<u8,kTaskArena> taskArena{},taskRef{};      // CTaskComplexGoPickUpEntity falso (0x34 bytes), se compara ENTERO
alignas(16) inline std::array<u8,kEntArena> entArena{},entRef{};         // entidad falsa: +4 posicion, +0x10 rumbo, +0x14 matriz, +0x18 objeto RW, +0x22 modelo, +0x36 banderas, +0x44/+0x50 velocidades; vtable falsa
alignas(16) inline std::array<u8,kPedArena> pedArena{},pedRef{};         // ped falso (solo se lee +4 y +0x14)
alignas(16) inline std::array<u8,kMatArena> entMatArena{},entMatRef{};   // matriz propia de la entidad
alignas(16) inline std::array<u8,kMatArena> allocMatArena{},allocMatRef{}; // matriz que "reserva" el falso de AllocateMatrix
alignas(16) inline std::array<u8,kMatArena> pedMatArena{},pedMatRef{};   // matriz del ped (posicion +0x30)
alignas(16) inline std::array<u8,kMiArena> miArena{},miRef{};            // CBaseModelInfo falso
alignas(16) inline std::array<u8,kColArena> colArena{},colRef{};         // CColModel falso (caja en +0)
alignas(16) inline std::array<u8,kRwArena> rwArena{},rwRef{};            // objeto RW falso ([+4] = marco)
alignas(16) inline std::array<u8,kFrameArena> frameArena{},frameRef{};   // marco RW falso (solo su direccion)
alignas(16) inline std::array<u8,kAllocArena> allocArena{},allocRef{};   // objeto que "reserva" el falso de operator new

// Etiquetas para punteros: las arenas cambian de direccion entre ejecuciones, no de papel
struct Ctx {uintptr_t task{},ent{},ped{},entMat{},allocMat{},pedMat{},mi{},col{},rw{},frame{},alloc{};};
inline Ctx cx;
inline json Name(uintptr_t v) {
    const auto in=[&](uintptr_t base,u32 bytes,const char* tag)->std::optional<json>{if(v>=base&&v<base+bytes)return json(std::string(tag)+"+"+std::to_string(v-base));return std::nullopt;};
    if(auto r=in(cx.task,kTaskBytes,"T"))return *r;
    if(auto r=in(cx.ent,kEntBytes,"E"))return *r;
    if(auto r=in(cx.ped,kPedBytes,"P"))return *r;
    if(auto r=in(cx.entMat,kMatBytes,"M"))return *r;
    if(auto r=in(cx.allocMat,kMatBytes,"N"))return *r;
    if(auto r=in(cx.pedMat,kMatBytes,"Q"))return *r;
    if(auto r=in(cx.mi,kMiBytes,"I"))return *r;
    if(auto r=in(cx.col,kColBytes,"L"))return *r;
    if(auto r=in(cx.rw,kRwBytes,"R"))return *r;
    if(auto r=in(cx.frame,kFrameBytes,"F"))return *r;
    if(auto r=in(cx.alloc,kAllocBytes,"A"))return *r;
    return uint32(v);
}

// ---- falsos. Registro ordenado de eventos (sin reservas ni JSON dentro de los falsos)
enum : u32 {EvSetRot=1,EvMatRw=2,EvStRw=3,EvAlloc=4,EvStMat=5,EvXform=6,EvNew=7,EvStatic=8,EvTrap=9};
struct Ev {u32 kind,a,b,c,d,e;};
inline std::array<Ev,kMaxEvents> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0){if(nev<evs.size())evs[nev]=Ev{k,a,b,c,d,e};++nev;}
inline u32 gNewN{},gAllocMat{};inline std::array<u32,3> gTarget{};inline u32 gAllocPtr{};
inline u32 P(const void* p){return u32(uintptr_t(p));}
// 0x59B020 CMatrix::SetRotateZOnly(float): thiscall con 1 argumento de pila (`ret 4`); el angulo se declara como u32 para no tocarlo
inline void __fastcall FakeSetRot(void* self,void*,u32 bits){Push(EvSetRot,P(self),bits);}
// 0x59AD70 CMatrix::UpdateRwMatrix(RwMatrix*) const: thiscall, `ret 4`
inline void __fastcall FakeMatRw(void* self,void*,void* rw){Push(EvMatRw,P(self),P(rw));}
// 0x54EF40 CSimpleTransform::UpdateRwMatrix(RwMatrix*): thiscall, `ret 4` (this = &entidad->m_placement)
inline void __fastcall FakeStRw(void* self,void*,void* rw){Push(EvStRw,P(self),P(rw));}
// 0x54F560 CPlaceable::AllocateMatrix(): thiscall sin argumentos; deja la matriz reservada en [this+0x14]
inline void __fastcall FakeAlloc(void* self,void*){Push(EvAlloc,P(self));std::memcpy(static_cast<u8*>(self)+kMatrixAt,&gAllocMat,4);}
// 0x54F1B0 CSimpleTransform::UpdateMatrix(CMatrix*): thiscall, `ret 4` (this = &entidad->m_placement)
inline void __fastcall FakeStMat(void* self,void*,void* mat){Push(EvStMat,P(self),P(mat));}
// 0x59C890 CMatrix * CVector: cdecl, puntero de retorno oculto primero (EAX = destino). Registra la matriz, la direccion del vector y sus tres floats (la direccion del destino es de pila: no se registra)
inline u32 __cdecl FakeXform(u32* out,const u32* mat,const u32* vec){Push(EvXform,P(mat),P(vec),vec[0],vec[1],vec[2]);out[0]=gTarget[0];out[1]=gTarget[1];out[2]=gTarget[2];return P(out);}
// 0x61A5A0 CTask::operator new(size), cdecl: devuelve la arena de la reserva (o nulo)
inline void* __cdecl FakeNew(size_t size){Push(EvNew,u32(size));++gNewN;return reinterpret_cast<void*>(uintptr_t(gAllocPtr));}
// CEntity::SetIsStatic(bool) (vtable +0x10 de la entidad): thiscall, `ret 4`; el bool se registra por su byte bajo
inline void __fastcall FakeStatic(void* self,void*,u32 flag){Push(EvStatic,P(self),flag&0xFFu);}
// Ranuras no previstas de la vtable falsa: se registran (el oraculo no las espera) para que un cambio de ranura en la DLL salte a la vista
template<unsigned K> u32 __fastcall Trap(void* self,void*){Push(EvTrap,K,P(self));return 0;}
using TrapFn=u32(__fastcall*)(void*,void*);
template<unsigned... I> constexpr std::array<TrapFn,sizeof...(I)> MakeTraps(std::integer_sequence<unsigned,I...>){return {&Trap<I>...};}
inline const std::array<TrapFn,kVtSlots> kTraps=MakeTraps(std::make_integer_sequence<unsigned,kVtSlots>{});
alignas(16) inline std::array<uintptr_t,kVtSlots> vtEnt{};
inline void InitVtables() {
    for(unsigned k=0;k<kVtSlots;++k)vtEnt[k]=uintptr_t(kTraps[k]);
    vtEnt[4]=uintptr_t(&FakeStatic);
}

struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: la funcion de la DLL no tiene RH_ScopedInstall (la entrada del EXE sigue siendo codigo original)
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
inline constexpr unsigned kStubs=7;
inline constexpr uintptr_t kRealCtors[]{0x668120};   // constructor real (con hook en HEAD): debe ser un salto a la DLL en AMBOS lados
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;       // colaboradores sustituidos y constructor: deben seguir siendo el codigo que eran al empezar
    std::array<Pair,kStubs> stub{};   // 0 SetRotateZOnly, 1 CMatrix::UpdateRwMatrix, 2 CSimpleTransform::UpdateRwMatrix, 3 UpdateMatrix, 4 AllocateMatrix, 5 operator*, 6 operator new
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,2*kStubs> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). En ambos lados los colaboradores con hook deben ser saltos a la DLL (y los sin hook, codigo del EXE);
// los falsos se instalan DESPUES de comprobar rutas y colaboradores.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const auto& s:env.stub)collab&=(s.hooked?(ReversingProbe::JumpsToDll(s.exe)&&ModuleOf(Resolve(s.exe))==env.asi):!ReversingProbe::JumpsToDll(s.exe))&&s.dll!=0&&ModuleOf(s.dll)==env.asi;
        for(const uintptr_t a:kRealCtors)collab&=ReversingProbe::JumpsToDll(a)&&ModuleOf(Resolve(a))==env.asi;
        side.collabOK=collab;
        {
            Patches patches;
            patches.Add(env.stub[0],&FakeSetRot);
            patches.Add(env.stub[1],&FakeMatRw);
            patches.Add(env.stub[2],&FakeStRw);
            patches.Add(env.stub[3],&FakeStMat);
            patches.Add(env.stub[4],&FakeAlloc);
            patches.Add(env.stub[5],&FakeXform);
            patches.Add(env.stub[6],&FakeNew);
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

// ------------------------------------------------------------------------------------------------ CTaskComplexGoPickUpEntity::CreateFirstSubTask
inline constexpr const char* kFn="CTaskComplexGoPickUpEntity::CreateFirstSubTask";
inline json EventJson(const Ev& e) {
    switch(e.kind) {
    case EvSetRot: return json::array({"setrot",Name(e.a),e.b});
    case EvMatRw: return json::array({"matrw",Name(e.a),Name(e.b)});
    case EvStRw: return json::array({"strw",Name(e.a),Name(e.b)});
    case EvAlloc: return json::array({"alloc",Name(e.a)});
    case EvStMat: return json::array({"stmat",Name(e.a),Name(e.b)});
    case EvXform: return json::array({"xform",Name(e.a),Name(e.b),e.c,e.d,e.e});
    case EvNew: return json::array({"new",e.a});
    case EvStatic: return json::array({"static",Name(e.a),e.b});
    default: return json::array({"trap",e.a,Name(e.b)});
    }
}
inline void Put32(u8* p,u32 v){std::memcpy(p,&v,4);}
inline u32 Get32(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
// Objeto reservado: palabras (la vtable se anota aparte), con los punteros como etiquetas
inline json AllocJson(u32& vtable) {
    json o;
    const u8* obj=allocArena.data()+kG2;
    vtable=Get32(obj);
    json words=json::array();words.push_back(nullptr);   // la vtable se anota aparte
    for(u32 k=1;k<kAllocBytes/4;++k)words.push_back(Name(Get32(obj+4*k)));
    bool outside=true;for(u32 i=0;i<kAllocArena;++i)if((i<kG2||i>=kG2+kAllocBytes)&&allocArena[i]!=allocRef[i])outside=false;
    o["vtable_written"]=vtable!=Get32(allocRef.data()+kG2);o["words"]=words;o["outside_ok"]=outside;
    return o;
}
// Bytes distintos de la referencia, como [desplazamiento respecto al objeto, byte]; skipLo..skipHi (desplazamientos respecto al objeto) se omiten (el puntero de matriz de la entidad se anota aparte, por etiqueta)
template<size_t N> json Changes(const std::array<u8,N>& a,const std::array<u8,N>& r,u32 off,int skipLo=0,int skipHi=0) {
    json o=json::array();
    for(u32 i=0;i<N;++i){const int rel=int(i)-int(off);if(a[i]!=r[i]&&!(rel>=skipLo&&rel<skipHi))o.push_back({rel,a[i]});}
    return o;
}

// entrada de ms_modelInfoPtrs que el caso apunta al CBaseModelInfo falso (se restaura tras cada lado)
inline uintptr_t gSlot{};inline u32 gSlotSaved{};

inline void CasePickUp(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    u8* const task=taskArena.data()+kGuard;u8* const ent=entArena.data()+kGuard;u8* const ped=pedArena.data()+kGuard;
    u8* const entMat=entMatArena.data()+kG2;u8* const allocMat=allocMatArena.data()+kG2;u8* const pedMat=pedMatArena.data()+kG2;
    u8* const mi=miArena.data()+kG2;u8* const col=colArena.data()+kG2;u8* const rw=rwArena.data()+kG2;u8* const frame=frameArena.data()+kG2;u8* const alloc=allocArena.data()+kG2;
    cx={P(task),P(ent),P(ped),P(entMat),P(allocMat),P(pedMat),P(mi),P(col),P(rw),P(frame),P(alloc)};

    FillWords(taskRef.data(),kTaskArena,Rand(n,200));FillWords(entRef.data(),kEntArena,Rand(n,201));FillWords(pedRef.data(),kPedArena,Rand(n,202));
    FillWords(entMatRef.data(),kMatArena,Rand(n,203));FillWords(allocMatRef.data(),kMatArena,Rand(n,204));FillWords(pedMatRef.data(),kMatArena,Rand(n,205));
    FillWords(miRef.data(),kMiArena,Rand(n,206));FillWords(colRef.data(),kColArena,Rand(n,207));FillWords(rwRef.data(),kRwArena,Rand(n,208));FillWords(frameRef.data(),kFrameArena,Rand(n,209));
    std::memset(allocRef.data(),int(c.fill),kAllocArena);

    u8* const rt=taskRef.data()+kGuard;
    Put32(rt+kEntityAt,c.entNull?0u:P(ent));
    u8* const re=entRef.data()+kGuard;u8* const rp=pedRef.data()+kGuard;u8* const rem=entMatRef.data()+kG2;u8* const ram=allocMatRef.data()+kG2;u8* const rpm=pedMatRef.data()+kG2;
    Put32(re,P(vtEnt.data()));
    Put32(re+kMatrixAt,c.entMat?P(entMat):0u);
    Put32(re+kRwAt,c.rw?P(rw):0u);
    { const u16 m=u16(c.modelIdx);std::memcpy(re+kModelAt,&m,2); }
    re[kFlagsAt]=u8(c.flags);
    Put32(re+kHeadingAt,kSentHeading);
    for(u32 k=0;k<3;++k){
        Put32(re+kPlacementAt+4*k,c.entMat?c.entAlt[k]:c.ent[k]);    // la posicion activa: placement sin matriz, matriz con matriz
        Put32(rem+0x30+4*k,c.ent[k]);
        Put32(rem+4*k,c.right[k]);Put32(rem+0x10+4*k,c.fwd[k]);Put32(rem+0x20+4*k,c.up[k]);
        Put32(ram+4*k,c.aRight[k]);Put32(ram+0x10+4*k,c.aFwd[k]);Put32(ram+0x20+4*k,c.aUp[k]);Put32(ram+0x30+4*k,c.entAlt[k]);
        Put32(rp+kPlacementAt+4*k,c.pedMat?c.pedAlt[k]:c.ped[k]);
        Put32(rpm+0x30+4*k,c.ped[k]);
        Put32(colRef.data()+kG2+4*k,c.box[k]);Put32(colRef.data()+kG2+12+4*k,c.box[3+k]);
    }
    Put32(rp+kMatrixAt,c.pedMat?P(pedMat):0u);
    Put32(miRef.data()+kG2+kColAtMi,P(col));
    Put32(rwRef.data()+kG2+kParentAtRw,P(frame));
    for(u32 k=0;k<6;++k)Put32(re+kMoveAt+4*k,kSentSpeed);   // velocidades (+0x44 y +0x50) no nulas: el banco ve que se ponen a cero
    for(u32 k=0;k<3;++k){Put32(rt+kPosAt+4*k,kSentPos);Put32(rt+kPickupAt+4*k,kSentPickup);}
    Put32(rt+kTimeFieldAt,kSentTime);

    const json input={{"family",c.family},{"ent_null",c.entNull},{"ent_mat",c.entMat},{"flags",c.flags},{"model",c.modelIdx},{"rw",c.rw},{"ped_mat",c.pedMat},
        {"ent",std::vector<u32>(c.ent,c.ent+3)},{"ent_alt",std::vector<u32>(c.entAlt,c.entAlt+3)},{"ped",std::vector<u32>(c.ped,c.ped+3)},{"ped_alt",std::vector<u32>(c.pedAlt,c.pedAlt+3)},
        {"box",std::vector<u32>(c.box,c.box+6)},{"right",std::vector<u32>(c.right,c.right+3)},{"fwd",std::vector<u32>(c.fwd,c.fwd+3)},{"up",std::vector<u32>(c.up,c.up+3)},
        {"a_right",std::vector<u32>(c.aRight,c.aRight+3)},{"a_fwd",std::vector<u32>(c.aFwd,c.aFwd+3)},{"a_up",std::vector<u32>(c.aUp,c.aUp+3)},
        {"target",std::vector<u32>(c.target,c.target+3)},{"time_ms",c.timeMs},{"new_null",c.newNull},{"fill",c.fill},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack}};
    Side sides[2];json rows[2];
    const u32 args[1]{P(ped)};
    gSlot=kModelPtrs+4*c.modelIdx;
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskArena=taskRef;entArena=entRef;pedArena=pedRef;entMatArena=entMatRef;allocMatArena=allocMatRef;pedMatArena=pedMatRef;miArena=miRef;colArena=colRef;rwArena=rwRef;frameArena=frameRef;allocArena=allocRef;
        *reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))=c.timeMs;
        gSlotSaved=*reinterpret_cast<volatile u32*>(gSlot);
        *reinterpret_cast<volatile u32*>(gSlot)=P(mi);
        nev=0;gNewN=0;gAllocMat=P(allocMat);gTarget={c.target[0],c.target[1],c.target[2]};gAllocPtr=c.newNull?0u:P(alloc);
        sides[s]=Execute(env,*env.route,0x693610,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,P(task),c.edx,c.stack,0);});
        *reinterpret_cast<volatile u32*>(gSlot)=gSlotSaved;

        // ---- observacion
        json obs;
        obs["ret"]=Name(sides[s].reg.eax);
        json ej=json::array();
        for(unsigned i=0;i<nev&&i<kMaxEvents;++i)ej.push_back(EventJson(evs[i]));
        obs["events"]=ej;obs["nevents"]=nev;
        obs["task_changes"]=Changes(taskArena,taskRef,kGuard);
        obs["ent_changes"]=Changes(entArena,entRef,kGuard,int(kMatrixAt),int(kMatrixAt)+4);
        obs["ent_matrix"]=Name(Get32(entArena.data()+kGuard+kMatrixAt));
        obs["others_intact"]=pedArena==pedRef&&entMatArena==entMatRef&&allocMatArena==allocMatRef&&pedMatArena==pedMatRef&&miArena==miRef&&colArena==colRef&&rwArena==rwRef&&frameArena==frameRef;
        u32 vt0{};
        obs["alloc"]=AllocJson(vt0);
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        const bool timeIntact=*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))==c.timeMs;
        const bool slotIntact=*reinterpret_cast<volatile u32*>(gSlot)==gSlotSaved;
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        out["alloc_vtables"]=json::array({vt0});   // fuera de `observed`: el oraculo no conoce la vtable, pero EXE y DLL deben dejar la misma
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&timeIntact&&slotIntact;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn,n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    json stubbed=json::array();
    for(const auto& s:env.stub){env.watches.emplace_back(s.exe,16);stubbed.push_back({{"exe",s.exe},{"dll",s.dll},{"hooked",s.hooked}});ready&=s.dll!=0&&ModuleOf(s.dll)==env.asi;}
    for(const uintptr_t a:kRealCtors){env.watches.emplace_back(a,16);ready&=ReversingProbe::JumpsToDll(a);}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    ready&=uintptr_t(&CTimer::m_snTimeInMilliseconds)==kTimeAddr;
    InitVtables();
    const u32 savedTime=*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr));
    std::vector<u8> modelTable=ReversingProbe::Bytes(reinterpret_cast<void*>(uintptr_t(kModelPtrs)),4*kModelCount);
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{kFn,kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",std::vector<uintptr_t>(std::begin(kRealCtors),std::end(kRealCtors))},{"stubbed",stubbed}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CasePickUp(env,totals,c);
        Write({{"event","function_summary"},{"function",kFn},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    *reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))=savedTime;
    const bool tableIntact=modelTable==ReversingProbe::Bytes(reinterpret_cast<void*>(uintptr_t(kModelPtrs)),4*kModelCount);
    const bool entriesIntact=env.WatchesIntact()&&tableIntact;
    bool stateRestored=entriesIntact&&*reinterpret_cast<volatile u32*>(uintptr_t(kTimeAddr))==savedTime;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
