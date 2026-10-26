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
// Nucleo del banco LeaveCarFirst70: CTaskComplexLeaveCar::CreateFirstSubTask (0x641FC0). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego usados en LiveComparison.hpp.
// Plantilla: reversing/PickUpFirst68 (hook Virtual, falsos con parche de entrada en el EXE y en la DLL, constructores reales en arena) y reversing/LeaveCar67 (IsPlayer, rand).
//
// Banco par a par de UNA funcion (hook virtual: HookRoute con el cuerpo del hook Virtual). Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido.
// Sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL, resuelto ANTES de escribir ningun parche): con hook en HEAD CPed::IsPlayer 0x5DF8F0,
// CTask::operator new 0x61A5A0, CPedGroups::GetPedsGroup 0x5F7E80, CPedGroupMembership::IsLeader 0x5F69C0, CEntity::RegisterReference 0x571B70, CPedGroupIntelligence::AddEvent 0x5F7470,
// CEntity::CleanUpOldReference 0x571A00 y CPed::SetRadioStation 0x5DFD90; sin hook en HEAD CEvent::operator new 0x4B5620, CEvent::operator delete 0x4B5630 y
// CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver 0x4F5BA0 (CAEVehicleAudioEntity::InjectHooks no esta encendido; la entrada del EXE sigue siendo codigo original); y el printf del CRT del EXE 0x821982
// (con salto a la DLL en HEAD; la DLL lo llama por esa direccion: solo se parchea la del EXE, sin cuerpo aparte).
// Real en ambos lados (saltos a la DLL, se comparan las palabras de los objetos construidos en arena): CTaskSimplePause 0x48E750, CTaskComplexLeaveBoat 0x63BB60, CTaskSimpleCarDriveTimed 0x5FF940,
// CEventLeaderExitedCarAsDriver 0x4B8300; y el CEventGroupEvent local (constructor 0x4ADFD0 y destructor 0x4AE070 sin hook: codigo del EXE contra el de la DLL), con sus llamadas a RegisterReference /
// CleanUpOldReference / CEvent::operator delete cayendo en los falsos.

namespace Leave70Live {
using namespace Leave70Fix;
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
// true: EAX/ECX/EDX de salida forman parte de la igualdad. EAX es el retorno (se compara siempre, como etiqueta, en `observed.ret`). El unico llamador relevante del EXE,
// CTaskComplex::SetSubTask/CTaskManager (llamada virtual y luego `mov edi,eax`), sobrescribe ECX/EDX antes de usarlos; por defecto solo se registran.
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
alignas(16) inline std::array<u8,kTaskArena> taskArena{},taskRef{};      // CTaskComplexLeaveCar falso (0x34 bytes), se compara ENTERO
alignas(16) inline std::array<u8,kPedArena> pedArena{},pedRef{};         // ped falso (solo se lee +0x46C, +0x474, +0x530, +0x58C)
alignas(16) inline std::array<u8,kVehArena> vehArena{},vehRef{};         // vehiculo objetivo falso (+0x460, +0x590)
alignas(16) inline std::array<u8,kVehArena> othArena{},othRef{};         // otro vehiculo (m_pVehicle del ped / conductor distinto)
alignas(16) inline std::array<u8,kGrpArena> grpArena{},grpRef{};         // grupo falso (solo direcciones)
alignas(16) inline std::array<u8,kEvArena> evArena{},evRef{};            // evento que "reserva" el falso de CEvent::operator new
alignas(16) inline std::array<u8,kAllocArena> allocArena{},allocRef{};   // tarea que "reserva" el falso de CTask::operator new

// Etiquetas para punteros: las arenas cambian de direccion entre ejecuciones, no de papel
struct Ctx {uintptr_t task{},ped{},veh{},oth{},grp{},ev{},alloc{};};
inline Ctx cx;
inline json Name(uintptr_t v) {
    const auto in=[&](uintptr_t base,u32 bytes,const char* tag)->std::optional<json>{if(v>=base&&v<base+bytes)return json(std::string(tag)+"+"+std::to_string(v-base));return std::nullopt;};
    if(auto r=in(cx.task,kTaskBytes,"T"))return *r;
    if(auto r=in(cx.ped,kPedBytes,"P"))return *r;
    if(auto r=in(cx.veh,kVehBytes,"V"))return *r;
    if(auto r=in(cx.oth,kVehBytes,"W"))return *r;
    if(auto r=in(cx.grp,kGrpBytes,"G"))return *r;
    if(auto r=in(cx.ev,kEvBytes,"X"))return *r;
    if(auto r=in(cx.alloc,kAllocBytes,"A"))return *r;
    return uint32(v);
}

// ---- falsos. Registro ordenado de eventos (sin reservas ni JSON dentro de los falsos)
enum : u32 {EvIsPlayer=1,EvNewTask=2,EvPrintf=3,EvGetGroup=4,EvIsLeader=5,EvNewEv=6,EvReg=7,EvAdd=8,EvClean=9,EvDel=10,EvRadio=11,EvAudio=12};
struct Ev {u32 kind,a,b,c,d,e;};
inline std::array<Ev,kMaxEvents> evs{};inline unsigned nev{};
inline std::array<std::array<char,48>,kMaxEvents> evStr{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0){if(nev<evs.size())evs[nev]=Ev{k,a,b,c,d,e};++nev;}
inline u32 P(const void* p){return u32(uintptr_t(p));}
inline u32 Get32(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
inline void Put32(u8* p,u32 v){std::memcpy(p,&v,4);}
inline std::array<u32,kPlSeq> gPl{};inline u32 gPlN{},gLeader{},gGroupPtr{},gEvPtr{},gAllocPtr{};
// 0x5DF8F0 CPed::IsPlayer() const: thiscall sin argumentos; devuelve AL (el resto de EAX es basura del caso)
inline u32 __fastcall FakeIsPlayer(void* self,void*){Push(EvIsPlayer,P(self));const u32 k=gPlN++;return k<kPlSeq?gPl[k]:0u;}
// 0x61A5A0 CTask::operator new(size), cdecl: devuelve la arena de la reserva (o nulo)
inline void* __cdecl FakeNewTask(size_t size){Push(EvNewTask,u32(size));return reinterpret_cast<void*>(uintptr_t(gAllocPtr));}
// 0x821982 printf del CRT del EXE (cdecl, variadica): solo se registra el texto
inline int __cdecl FakePrintf(const char* fmt,...){
    if(nev<evs.size()){auto& s=evStr[nev];std::size_t i=0;for(;fmt&&fmt[i]&&i+1<s.size();++i)s[i]=fmt[i];s[i]=0;}
    Push(EvPrintf);return 0;
}
// 0x5F7E80 CPedGroups::GetPedsGroup(const CPed*), cdecl: devuelve el grupo del caso (o nulo)
inline u32 __cdecl FakeGetGroup(u32 ped){Push(EvGetGroup,ped);return gGroupPtr;}
// 0x5F69C0 CPedGroupMembership::IsLeader(const CPed*) const: thiscall, `ret 4`; devuelve AL
inline u32 __fastcall FakeIsLeader(void* self,void*,u32 ped){Push(EvIsLeader,P(self),ped);return gLeader;}
// 0x4B5620 CEvent::operator new(size), cdecl: arena del evento (o nulo)
inline void* __cdecl FakeNewEv(u32 size){Push(EvNewEv,size);return reinterpret_cast<void*>(uintptr_t(gEvPtr));}
// 0x571B70 CEntity::RegisterReference(CEntity**): thiscall, `ret 4`; se registra la entidad y el contenido de la ranura
inline void __fastcall FakeReg(void* self,void*,const u32* ref){Push(EvReg,P(self),ref[0]);}
// 0x5F7470 CPedGroupIntelligence::AddEvent(CEvent*): thiscall, `ret 4`; se registran los campos del CEventGroupEvent local (+4 tiempo activo, +8 valido, +0xC ped, +0x10 evento)
inline u32 __fastcall FakeAdd(void* self,void*,const u8* ev){Push(EvAdd,P(self),Get32(ev+4),ev[8],Get32(ev+0xC),Get32(ev+0x10));return 1;}
// 0x571A00 CEntity::CleanUpOldReference(CEntity**): thiscall, `ret 4`
inline void __fastcall FakeClean(void* self,void*,const u32* ref){Push(EvClean,P(self),ref[0]);}
// 0x4B5630 CEvent::operator delete(void*), cdecl
inline void __cdecl FakeDel(void* p){Push(EvDel,P(p));}
// 0x5DFD90 CPed::SetRadioStation(): thiscall sin argumentos
inline void __fastcall FakeRadio(void* self,void*){Push(EvRadio,P(self));}
// 0x4F5BA0 CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver(): thiscall sin argumentos
inline void __fastcall FakeAudio(void* self,void*){Push(EvAudio,P(self));}

struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: la entrada del EXE sigue siendo codigo original; dll=0: la DLL llama por la direccion del EXE
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
inline constexpr unsigned kStubs=12;
inline constexpr uintptr_t kRealCtors[]{0x48E750,0x63BB60,0x5FF940,0x4B8300};   // constructores reales (con hook en HEAD): deben ser saltos a la DLL en AMBOS lados
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;       // colaboradores sustituidos y constructores: deben seguir siendo el codigo que eran al empezar
    std::array<Pair,kStubs> stub{};   // 0 IsPlayer, 1 CTask::operator new, 2 printf, 3 GetPedsGroup, 4 IsLeader, 5 CEvent::operator new, 6 RegisterReference, 7 AddEvent, 8 CleanUpOldReference, 9 CEvent::operator delete, 10 SetRadioStation, 11 PlayerAboutToExitVehicleAsDriver
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
        for(const auto& s:env.stub)collab&=(s.hooked?(ReversingProbe::JumpsToDll(s.exe)&&ModuleOf(Resolve(s.exe))==env.asi):!ReversingProbe::JumpsToDll(s.exe))&&(s.dll==0||ModuleOf(s.dll)==env.asi);
        for(const uintptr_t a:kRealCtors)collab&=ReversingProbe::JumpsToDll(a)&&ModuleOf(Resolve(a))==env.asi;
        side.collabOK=collab;
        {
            Patches patches;
            patches.Add(env.stub[0],&FakeIsPlayer);
            patches.Add(env.stub[1],&FakeNewTask);
            patches.Add(env.stub[2],&FakePrintf);
            patches.Add(env.stub[3],&FakeGetGroup);
            patches.Add(env.stub[4],&FakeIsLeader);
            patches.Add(env.stub[5],&FakeNewEv);
            patches.Add(env.stub[6],&FakeReg);
            patches.Add(env.stub[7],&FakeAdd);
            patches.Add(env.stub[8],&FakeClean);
            patches.Add(env.stub[9],&FakeDel);
            patches.Add(env.stub[10],&FakeRadio);
            patches.Add(env.stub[11],&FakeAudio);
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

// ------------------------------------------------------------------------------------------------ CTaskComplexLeaveCar::CreateFirstSubTask
inline constexpr const char* kFn="CTaskComplexLeaveCar::CreateFirstSubTask";
inline json EventJson(const Ev& e,unsigned i) {
    switch(e.kind) {
    case EvIsPlayer: return json::array({"isplayer",Name(e.a)});
    case EvNewTask: return json::array({"newtask",e.a});
    case EvPrintf: return json::array({"printf",std::string(evStr[i].data())});
    case EvGetGroup: return json::array({"getgroup",Name(e.a)});
    case EvIsLeader: return json::array({"isleader",Name(e.a),Name(e.b)});
    case EvNewEv: return json::array({"newev",e.a});
    case EvReg: return json::array({"reg",Name(e.a),Name(e.b)});
    case EvAdd: return json::array({"add",Name(e.a),e.b,e.c,Name(e.d),Name(e.e)});
    case EvClean: return json::array({"clean",Name(e.a),Name(e.b)});
    case EvDel: return json::array({"del",Name(e.a)});
    case EvRadio: return json::array({"radio",Name(e.a)});
    default: return json::array({"audio",Name(e.a)});
    }
}
// Objeto reservado: palabras (la vtable se anota aparte), con los punteros como etiquetas; fuera = las guardas intactas
inline json AllocJson(u32& vtable) {
    json o;
    const u8* obj=allocArena.data()+kGuard;
    vtable=Get32(obj);
    json words=json::array();words.push_back(nullptr);   // la vtable se anota aparte
    for(u32 k=1;k<kAllocBytes/4;++k)words.push_back(Name(Get32(obj+4*k)));
    bool outside=true;for(u32 i=0;i<kAllocArena;++i)if((i<kGuard||i>=kGuard+kAllocBytes)&&allocArena[i]!=allocRef[i])outside=false;
    o["vtable_written"]=vtable!=Get32(allocRef.data()+kGuard);o["words"]=words;o["outside_ok"]=outside;
    return o;
}
// Evento reservado: bytes distintos de la referencia (relleno uniforme), salvo la vtable, como [desplazamiento, byte]; fuera = guardas intactas
inline json EventObjJson(u32& vtable) {
    json o;
    const u8* obj=evArena.data()+kGuard;
    vtable=Get32(obj);
    json changes=json::array();
    for(u32 i=4;i<kEvBytes;++i)if(obj[i]!=evRef[kGuard+i])changes.push_back({i,obj[i]});
    bool outside=true;for(u32 i=0;i<kEvArena;++i)if((i<kGuard||i>=kGuard+kEvBytes)&&evArena[i]!=evRef[i])outside=false;
    o["vtable_written"]=vtable!=Get32(evRef.data()+kGuard);o["changes"]=changes;o["outside_ok"]=outside;
    return o;
}

inline void CaseLeave(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    u8* const task=taskArena.data()+kGuard;u8* const ped=pedArena.data()+kGuard;u8* const veh=vehArena.data()+kGuard;u8* const oth=othArena.data()+kGuard;
    u8* const grp=grpArena.data()+kGuard;u8* const ev=evArena.data()+kGuard;u8* const alloc=allocArena.data()+kGuard;
    cx={P(task),P(ped),P(veh),P(oth),P(grp),P(ev),P(alloc)};

    FillWords(taskRef.data(),kTaskArena,Rand(n,200));FillWords(pedRef.data(),kPedArena,Rand(n,201));FillWords(vehRef.data(),kVehArena,Rand(n,202));
    FillWords(othRef.data(),kVehArena,Rand(n,203));FillWords(grpRef.data(),kGrpArena,Rand(n,204));
    std::memset(evRef.data(),int(c.evFill),kEvArena);std::memset(allocRef.data(),int(c.fill),kAllocArena);

    u8* const rt=taskRef.data()+kGuard;u8* const rp=pedRef.data()+kGuard;u8* const rv=vehRef.data()+kGuard;
    Put32(rt+kTaskVehAt,P(veh));Put32(rt+kTaskDelayAt,c.delay);
    Put32(rp+kPedFlagsAt,c.flagsA);Put32(rp+kPedFlags2At,c.flagsB);Put32(rp+kPedStateAt,c.state);
    Put32(rp+kPedVehAt,c.pedVehMode==0?0u:c.pedVehMode==1?P(veh):c.pedVehMode==2?P(oth):P(ped));
    Put32(rv+kVehDriverAt,c.driverMode==0?P(ped):c.driverMode==1?0u:c.driverMode==2?P(oth):c.driverMode==3?P(ped)+4u:P(grp));
    Put32(rv+kVehTypeAt,c.vtype);

    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    const u32 args[1]{P(ped)};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskArena=taskRef;pedArena=pedRef;vehArena=vehRef;othArena=othRef;grpArena=grpRef;evArena=evRef;allocArena=allocRef;
        nev=0;gPlN=0;for(u32 k=0;k<kPlSeq;++k)gPl[k]=c.pl[k];
        gLeader=c.leaderRet;gGroupPtr=c.hasGroup?P(grp):0u;gEvPtr=c.evNull?0u:P(ev);gAllocPtr=c.taskNull?0u:P(alloc);
        sides[s]=Execute(env,*env.route,0x641FC0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,P(task),c.edx,c.stack,0);});

        // ---- observacion
        json obs;
        obs["ret"]=Name(sides[s].reg.eax);
        json ej=json::array();
        for(unsigned i=0;i<nev&&i<kMaxEvents;++i)ej.push_back(EventJson(evs[i],i));
        obs["events"]=ej;obs["nevents"]=nev;
        obs["others_intact"]=taskArena==taskRef&&pedArena==pedRef&&vehArena==vehRef&&othArena==othRef&&grpArena==grpRef;
        u32 vtA{},vtX{};
        obs["alloc"]=AllocJson(vtA);
        obs["event_obj"]=EventObjJson(vtX);
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        out["alloc_vtables"]=json::array({vtA,vtX});   // fuera de `observed`: el oraculo no conoce las vtables, pero EXE y DLL deben dejar las mismas
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
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
    for(const auto& s:env.stub){env.watches.emplace_back(s.exe,16);stubbed.push_back({{"exe",s.exe},{"dll",s.dll},{"hooked",s.hooked}});ready&=(s.dll==0||ModuleOf(s.dll)==env.asi);}
    for(const uintptr_t a:kRealCtors){env.watches.emplace_back(a,16);ready&=ReversingProbe::JumpsToDll(a);}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
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
        for(unsigned c=0;c<kCases;++c)CaseLeave(env,totals,c);
        Write({{"event","function_summary"},{"function",kFn},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
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
