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
// Nucleo del banco WalkRoundFirst31: CTaskComplexWalkRoundObject::CreateFirstSubTask (0x657380). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (CTask, CTaskPool/GetTaskPool, CTaskComplexWalkRoundObject).
// Plantillas: reversing/PartnerChatSeq30 (pool sintetico, hook Virtual, falsos con parche de entrada en el EXE y en la DLL) y reversing/WalkRoundControl31.
//
// Banco par a par de UNA funcion, CTaskComplexWalkRoundObject::CreateFirstSubTask (0x657380, hook virtual: HookRoute con el cuerpo del hook Virtual).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Colaboradores REALES (reconstruidos en C++ y enganchados, se ejecutan en ambos lados): CTask::operator new
// 0x61A5A0, CVector::NormaliseAndMag 0x59C970, CGeneral::LimitRadianAngle 0x53CB50 y GetRadianAngleBetweenPoints 0x53CBE0 (x87 dentro; el banco los exige enganchados a la DLL).
// SUSTITUIDOS por un falso identico en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): CPlaceable::AllocateMatrix (0x54F560) y CSimpleTransform::UpdateMatrix (0x54F1B0); solo en el EXE
// (la DLL los llama por plugin a la direccion del EXE): la funcion sin simbolo 0x6551D0 (rellena la ruta y devuelve un float en ST0) y CreateRouteTask (0x655140).
// El pool de tareas se sustituye por un pool SINTETICO por caso y se restaura; el reloj (0xB7CB84) se fija por caso y se restaura. El constructor de CTaskSimpleAchieveHeading (0x667E20) sigue en el EXE (solo se vigila).
// Las ranuras de la ventana se rellenan con 0xCD para que los bytes que los constructores no escriben sean iguales en ambos lados.

namespace WalkRoundFirst31Live {
using namespace WalkRoundFirst31Fix;
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
// La funcion usa x87 (X87Math) y deja la pila vacia como el EXE: se compara lo observable (FCW completa, TOP, etiquetas y ST validos).
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. EAX es el retorno (se compara siempre, como etiqueta, en `observed.ret`); ECX/EDX no los lee ningun llamador
// (CTaskComplex::ProcessPed usa EAX), por defecto solo se registran.
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada (la funcion no instala marco SEH, pero sus colaboradores si pueden).
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Igual que PartnerChatSeq30::Gate: siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; empuja `argCount` argumentos de pila; captura los registros y EFLAGS justo tras el `call`.
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
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};   // CTaskComplexWalkRoundObject falso (se compara ENTERO)
alignas(16) inline std::array<u8,kObjArena> objArena{},objRef{};      // objeto falso (solo puede cambiar m_matrix, por el falso de AllocateMatrix)
alignas(16) inline std::array<u8,kMatArena> matArena{},matRef{};      // matriz falsa del objeto
alignas(16) inline std::array<u8,kPedArena> pedArena{},pedRef{};      // peaton falso (se compara entero)
alignas(16) inline std::array<u8,kPMatArena> pmatArena{},pmatRef{};   // matriz falsa del peaton
alignas(16) inline std::array<u8,kRouteArena> routeArena{},routeRef{};// CPointRoute falsa (contador + 8 vectores)
// Mundo real que se restaura tras cada ejecucion
inline std::vector<uint8> realStates,realStorage,poolPre;
inline int32 realLast{};
inline bool realDealWithNoMemory{};
inline uint32 realTime{};
inline uint32& Timer() {return *reinterpret_cast<uint32*>(uintptr_t(0xB7CB84));}   // CTimer::m_snTimeInMilliseconds

// Pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* PoolHdr() {return reinterpret_cast<uint8*>(GetTaskPool());}
inline uint8* PoolStorage() {return *reinterpret_cast<uint8**>(PoolHdr());}
inline uint8* PoolStates() {return *reinterpret_cast<uint8**>(PoolHdr()+4);}
inline int32 PoolCap() {return *reinterpret_cast<int32*>(PoolHdr()+8);}
inline int32& PoolLast() {return *reinterpret_cast<int32*>(PoolHdr()+0xC);}
static_assert(sizeof(CTaskSimpleSlideToCoord)==kSlot,"el almacenamiento de una ranura del pool de tareas es de 0x80 bytes");

// Etiquetas para punteros: el pool y las arenas cambian de direccion entre ejecuciones, no de papel
struct Ctx {uintptr_t thisObj{},obj{},mat{},ped{},pmat{},route{};std::array<uintptr_t,kWindowMax> win{};size_t winN{};};
inline Ctx cx;
inline json Name(uintptr_t v) {
    if(v>=cx.thisObj&&v<cx.thisObj+kThisBytes)return "T+"+std::to_string(v-cx.thisObj);
    if(v>=cx.obj&&v<cx.obj+kObjBytes)return "O+"+std::to_string(v-cx.obj);
    if(v>=cx.mat&&v<cx.mat+kMatBytes)return "M+"+std::to_string(v-cx.mat);
    if(v>=cx.ped&&v<cx.ped+kPedBytes)return "P+"+std::to_string(v-cx.ped);
    if(v>=cx.pmat&&v<cx.pmat+kPMatBytes)return "PM+"+std::to_string(v-cx.pmat);
    if(v>=cx.route&&v<cx.route+kRouteBytes)return "R+"+std::to_string(v-cx.route);
    for(size_t k=0;k<cx.winN;++k)if(v>=cx.win[k]&&v<cx.win[k]+kSlot)return "W"+std::to_string(k)+"+"+std::to_string(v-cx.win[k]);
    return uint32(v);
}

// ---- falsos: AllocateMatrix / UpdateMatrix del objeto, 0x6551D0 (rellena la ruta, devuelve un float en ST0) y CreateRouteTask 0x655140
// kind: 1 AllocateMatrix(self) ; 2 UpdateMatrix(self, matriz) ; 3 CreateRouteTask(self, ped) ; 4 0x6551D0(self, ped)
struct Ev {u32 kind,a,b;};
inline std::array<Ev,24> evs{};inline unsigned nev{};inline unsigned evOverflow{};
inline u32 gMat{},gRouteCount{},gRouteDist{},gRouteRet{};inline std::array<u32,kRouteWords-1> gRouteEntries{};
inline void Push(const Ev& e) {if(nev<evs.size())evs[nev++]=e;else ++evOverflow;}
inline void __fastcall FakeAllocateMatrix(void* self,void*) {Push(Ev{1,u32(uintptr_t(self)),0});*reinterpret_cast<u32*>(uintptr_t(self)+kMatrixAt)=gMat;}
inline void __fastcall FakeUpdateMatrix(void* self,void*,void* matrix) {Push(Ev{2,u32(uintptr_t(self)),u32(uintptr_t(matrix))});}
inline void* __fastcall FakeCreateRoute(void* self,void*,void* ped) {Push(Ev{3,u32(uintptr_t(self)),u32(uintptr_t(ped))});return reinterpret_cast<void*>(uintptr_t(gRouteRet));}
inline float __fastcall FakeCalcRoute(void* self,void*,void* ped) {
    Push(Ev{4,u32(uintptr_t(self)),u32(uintptr_t(ped))});
    uint8* const route=*reinterpret_cast<uint8**>(uintptr_t(self)+kRouteAt);
    std::memcpy(route,&gRouteCount,4);std::memcpy(route+4,gRouteEntries.data(),sizeof(u32)*gRouteEntries.size());
    float d{};std::memcpy(&d,&gRouteDist,4);return d;
}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;   // colaboradores: deben seguir siendo el codigo que eran al empezar
    Pair pAlloc{},pUpdate{},pCalc{},pCreateRoute{};   // cuerpos de la DLL resueltos ANTES de escribir ningun parche (Calc y CreateRoute: solo EXE)
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,8> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}
// Colaboradores reales que deben ser saltos a la DLL en AMBOS lados (LimitRadianAngle, GetRadianAngleBetweenPoints y operator new); solo se vigilan el resto (NormaliseAndMag 0x59C970 tiene el hook apagado de fabrica: el lado original ejecuta el codigo del EXE y el candidato el cuerpo C++ de la DLL)
inline constexpr uintptr_t kCollaborators[]{0x61A5A0,0x53CB50,0x53CBE0};
inline constexpr uintptr_t kWatchedOnly[]{0x667E20,0x59C970,0x54F560,0x54F1B0,0x6551D0,0x655140};

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). En ambos lados los colaboradores reales deben ser saltos a la DLL;
// los falsos se instalan DESPUES de comprobar rutas y colaboradores.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;for(const uintptr_t a:kCollaborators)collab&=ReversingProbe::JumpsToDll(a)&&ModuleOf(Resolve(a))==env.asi;
        side.collabOK=collab;
        {
            Patches patches;
            patches.Add(env.pAlloc,&FakeAllocateMatrix);
            patches.Add(env.pUpdate,&FakeUpdateMatrix);
            patches.Add(env.pCalc,&FakeCalcRoute);
            patches.Add(env.pCreateRoute,&FakeCreateRoute);
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
inline json Finish(Env& env,Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}

// ------------------------------------------------------------------------------------------------ CTaskComplexWalkRoundObject::CreateFirstSubTask
inline void RestoreWorld(size_t cap) {
    std::memcpy(PoolStates(),realStates.data(),cap);
    PoolLast()=realLast;
    std::memcpy(PoolStorage(),realStorage.data(),cap*kSlot);
    Timer()=realTime;
}
inline bool WorldRestored(size_t cap) {
    return std::memcmp(PoolStates(),realStates.data(),cap)==0&&PoolLast()==realLast&&std::memcmp(PoolStorage(),realStorage.data(),cap*kSlot)==0&&Timer()==realTime;
}
inline json SerTask(uintptr_t addr) {
    const auto* w=reinterpret_cast<const u32*>(addr);
    json words=json::array();words.push_back(nullptr);   // la vtable se sustituye por GetTaskType
    for(u32 k=1;k<kSlot/4;++k)words.push_back(Name(w[k]));
    return {{"type",int(reinterpret_cast<const CTask*>(addr)->GetTaskType())},{"words",words}};
}
inline json Words(const u8* base,u32 n) {json a=json::array();for(u32 i=0;i<n;++i){u32 v{};std::memcpy(&v,base+i*4,4);a.push_back(v);}return a;}
template<size_t N> inline json Vec3(const std::array<u8,N>& a,u32 at) {u32 v[3]{};std::memcpy(v,a.data()+at,12);return json::array({v[0],v[1],v[2]});}

inline void CaseFirst(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const u32 cap=u32(PoolCap());
    const Pool ps=MakePool(c,cap,n);

    // direcciones para las etiquetas
    u8* const self=thisArena.data()+kThisOff;u8* const obj=objArena.data()+kObjOff;u8* const mat=matArena.data()+kMatOff;u8* const ped=pedArena.data()+kPedOff;
    u8* const pmat=pmatArena.data()+kPMatOff;u8* const route=routeArena.data()+kRouteOff;
    cx.thisObj=uintptr_t(self);cx.obj=uintptr_t(obj);cx.mat=uintptr_t(mat);cx.ped=uintptr_t(ped);cx.pmat=uintptr_t(pmat);cx.route=uintptr_t(route);
    cx.winN=ps.win.size();for(size_t k=0;k<ps.win.size();++k)cx.win[k]=uintptr_t(PoolStorage()+size_t(ps.win[k].idx)*kSlot);
    gMat=u32(uintptr_t(mat));

    // imagenes de referencia de las arenas
    FillWords(thisRef.data(),kThisArena,Rand(n,50));
    FillWords(objRef.data(),kObjArena,Rand(n,51));
    FillWords(matRef.data(),kMatArena,Rand(n,52));
    FillWords(pedRef.data(),kPedArena,Rand(n,53));
    FillWords(pmatRef.data(),kPMatArena,Rand(n,54));
    FillWords(routeRef.data(),kRouteArena,Rand(n,55));
    {   // this
        u8* const t0=thisRef.data()+kThisOff;
        const u32 objPtr=c.objNull?0u:u32(uintptr_t(obj)),routePtr=u32(uintptr_t(route));
        std::memcpy(t0+kObjectAt,&objPtr,4);std::memcpy(t0+kRouteAt,&routePtr,4);std::memcpy(t0+kMoveAt,&c.moveState,4);
    }
    {   // objeto falso: posicion del placement y matriz
        u8* const o0=objRef.data()+kObjOff;
        std::memcpy(o0+kPlaceAt,c.place,12);
        const u32 matPtr=c.matPresent?u32(uintptr_t(mat)):0u;
        std::memcpy(o0+kMatrixAt,&matPtr,4);
    }
    {   // matriz del objeto
        u8* const m0=matRef.data()+kMatOff;
        std::memcpy(m0+kMatRightAt,c.mright,12);std::memcpy(m0+kMatFwdAt,c.mfwd,12);std::memcpy(m0+kMatPosAt,c.mpos,12);
    }
    {   // peaton -> matriz (frente y posicion)
        u8* const p0=pedRef.data()+kPedOff;
        const u32 pm=u32(uintptr_t(pmat));
        std::memcpy(p0+kMatrixAt,&pm,4);std::memcpy(p0+kPlaceAt,c.pedPlace,12);
        u8* const q0=pmatRef.data()+kPMatOff;
        std::memcpy(q0+kMatFwdAt,c.pedFwd,12);std::memcpy(q0+kMatPosAt,c.pedPos,12);
    }
    gRouteCount=c.routeCount;std::copy(std::begin(c.routeEntries),std::end(c.routeEntries),gRouteEntries.begin());gRouteDist=c.routeDist;gRouteRet=c.routeRet;

    json routeInit=Words(routeRef.data()+kRouteOff,kRouteWords);
    json thisInit=Words(thisRef.data()+kThisOff+kF24At,12);
    json routeEntries=json::array();for(u32 j=0;j<kRouteWords-1;++j)routeEntries.push_back(c.routeEntries[j]);
    json win=json::array();for(const auto& w:ps.win)win.push_back({w.idx,w.state});
    const json input={{"family",c.family},{"obj_null",c.objNull},{"move_state",c.moveState},{"mat_present",c.matPresent},{"route_count",c.routeCount},{"route_entries",routeEntries},
        {"route_dist",c.routeDist},{"route_ret",c.routeRet},{"time",c.time},{"place",json::array({c.place[0],c.place[1],c.place[2]})},{"mat_pos",json::array({c.mpos[0],c.mpos[1],c.mpos[2]})},
        {"mat_fwd",json::array({c.mfwd[0],c.mfwd[1],c.mfwd[2]})},{"mat_right",json::array({c.mright[0],c.mright[1],c.mright[2]})},{"ped_pos",json::array({c.pedPos[0],c.pedPos[1],c.pedPos[2]})},
        {"ped_fwd",json::array({c.pedFwd[0],c.pedFwd[1],c.pedFwd[2]})},{"ped_place",json::array({c.pedPlace[0],c.pedPlace[1],c.pedPlace[2]})},{"route_init",routeInit},{"this_init",thisInit},
        {"pool_mode",c.poolMode},{"pool_k",c.poolK},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack},{"this_off",kThisOff},
        {"pool",{{"cap",cap},{"last",ps.last},{"free_total",ps.freeTotal},{"win",win}}}};

    // almacenamiento previo "visto" por la funcion: el real con las ranuras de la ventana a 0xCD
    poolPre=realStorage;
    for(const auto& w:ps.win)std::memset(poolPre.data()+size_t(w.idx)*kSlot,0xCD,kSlot);

    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(PoolStates(),ps.states.data(),cap);PoolLast()=int32(ps.last);
        std::memcpy(PoolStorage(),poolPre.data(),size_t(cap)*kSlot);
        thisArena=thisRef;objArena=objRef;matArena=matRef;pedArena=pedRef;pmatArena=pmatRef;routeArena=routeRef;nev=0;evOverflow=0;
        Timer()=c.time;
        sides[s]=Execute(env,*env.route,0x657380,candidate,[&](uintptr_t entry){const u32 arg=u32(uintptr_t(ped));return Gate(entry,&arg,1,c.eax,u32(uintptr_t(self)),c.edx,c.stack,0);});

        // ---- observacion
        json obs;
        obs["ret"]=Name(sides[s].reg.eax);
        json calls=json::array();
        for(unsigned i=0;i<nev;++i) {
            const Ev& e=evs[i];
            if(e.kind==1)calls.push_back(json::array({"AM",Name(e.a)}));
            else if(e.kind==2)calls.push_back(json::array({"UM",Name(e.a),Name(e.b)}));
            else if(e.kind==3)calls.push_back(json::array({"CR",Name(e.a),Name(e.b)}));
            else calls.push_back(json::array({"CF",Name(e.a),Name(e.b)}));
        }
        if(evOverflow)calls.push_back(json::array({"overflow",evOverflow}));
        obs["calls"]=calls;
        obs["last_after"]=PoolLast();
        json changes=json::array();std::vector<bool> allocated(ps.win.size());
        for(u32 i=0;i<cap;++i)if(PoolStates()[i]!=ps.states[i])changes.push_back({i,PoolStates()[i]});
        for(size_t k=0;k<ps.win.size();++k)allocated[k]=PoolStates()[ps.win[k].idx]!=ps.states[ps.win[k].idx];
        obs["state_changes"]=changes;
        json tasks=json::array();
        for(size_t k=0;k<ps.win.size();++k)if(allocated[k]) {json tk=SerTask(cx.win[k]);tk["slot"]=k;tasks.push_back(tk);}
        obs["tasks"]=tasks;
        long firstOutside=-1;
        for(u32 j=0;j<cap&&firstOutside<0;++j) {
            bool isAllocated=false;for(size_t k=0;k<ps.win.size();++k)if(ps.win[k].idx==j&&allocated[k])isAllocated=true;
            if(!isAllocated&&std::memcmp(PoolStorage()+size_t(j)*kSlot,poolPre.data()+size_t(j)*kSlot,kSlot)!=0)firstOutside=long(j);
        }
        obs["pool_outside_ok"]=firstOutside<0;obs["first_pool_outside"]=firstOutside;
        json thisChanges=json::array();
        for(u32 i=0;i<kThisArena;++i)if(thisArena[i]!=thisRef[i])thisChanges.push_back({int(i)-int(kThisOff),thisArena[i]});
        obs["this_changes"]=thisChanges;
        {   // el objeto solo puede cambiar m_matrix (+0x14..+0x17)
            auto tmp=objArena;std::memcpy(tmp.data()+kObjOff+kMatrixAt,objRef.data()+kObjOff+kMatrixAt,4);
            obs["obj_other_ok"]=tmp==objRef;
            u32 mp{};std::memcpy(&mp,objArena.data()+kObjOff+kMatrixAt,4);
            obs["obj_matrix"]=Name(mp);
        }
        obs["mat_ok"]=matArena==matRef;
        obs["ped_ok"]=pedArena==pedRef;
        obs["pmat_ok"]=pmatArena==pmatRef;
        {   // la ruta: solo sus palabras pueden cambiar; el resto de la arena debe quedar intacta
            auto tmp=routeArena;std::memcpy(tmp.data()+kRouteOff,routeRef.data()+kRouteOff,kRouteBytes);
            obs["route_guards_ok"]=tmp==routeRef;
            obs["route_after"]=Words(routeArena.data()+kRouteOff,kRouteWords);
        }
        obs["timer_ok"]=Timer()==c.time;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        RestoreWorld(cap);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&WorldRestored(cap);
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CTaskComplexWalkRoundObject::CreateFirstSubTask",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:kCollaborators)env.watches.emplace_back(at,16);
    for(const uintptr_t at:kWatchedOnly)env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    const size_t cap=size_t(PoolCap());
    ready&=cap>=256&&env.pAlloc.dll!=0&&env.pUpdate.dll!=0;
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskComplexWalkRoundObject::CreateFirstSubTask",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",std::vector<uintptr_t>(std::begin(kCollaborators),std::end(kCollaborators))},{"watched_only",std::vector<uintptr_t>(std::begin(kWatchedOnly),std::end(kWatchedOnly))},
                          {"stubbed",json::array({json{{"exe",env.pAlloc.exe},{"dll",env.pAlloc.dll}},json{{"exe",env.pUpdate.exe},{"dll",env.pUpdate.dll}},
                                                  json{{"exe",env.pCalc.exe},{"dll",env.pCalc.dll}},json{{"exe",env.pCreateRoute.exe},{"dll",env.pCreateRoute.dll}}})}}},
        {"pool_capacity",cap},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        realStates.assign(PoolStates(),PoolStates()+cap);
        realStorage.assign(PoolStorage(),PoolStorage()+cap*kSlot);
        realLast=PoolLast();realTime=Timer();
        realDealWithNoMemory=GetTaskPool()->CanDealWithNoMemory();
        GetTaskPool()->SetDealWithNoMemory(true);   // los casos sin ranura libre agotan el pool: sin esto la DLL se detendria en el punto de interrupcion de depuracion
        for(unsigned c=0;c<kCases;++c)CaseFirst(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskComplexWalkRoundObject::CreateFirstSubTask"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
        GetTaskPool()->SetDealWithNoMemory(realDealWithNoMemory);
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    if(ready)stateRestored&=WorldRestored(cap);
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
