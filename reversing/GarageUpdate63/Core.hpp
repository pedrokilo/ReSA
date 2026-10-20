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
// Nucleo del banco GarageUpdate63 (lote 63b). Plantilla: reversing/Garage63 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).

namespace GU63Live {
using namespace GU63Fix;
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
inline constexpr const char* kFn[kFns]{"CGarage::Update"};
inline constexpr const char* kName[kFns]{"Update"};
inline constexpr uintptr_t kEntry[kFns]{0x44AA50};
inline constexpr unsigned kMaxEvents = 600;

struct Arena {
    alignas(16) u32 gl[kGuardWords];
    u32 g[kGarageSize/4];
    u32 gr[kGuardWords];
    alignas(16) u32 veh[kMaxVeh][kVehSize/4];
    alignas(16) u32 mat[kMaxVeh][kMatWords];
    alignas(16) u32 ped[kPedWords];
    alignas(16) u32 wanted[kWantedWords];
    alignas(16) u32 vt[kVtWords];
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
    if(Within(W.g,sizeof(W.g),v,off)){if(off)snprintf(buf,sizeof(buf),"G+%u",off);else snprintf(buf,sizeof(buf),"G");return std::string(buf);}
    if(Within(W.veh,sizeof(W.veh),v,off)){const u32 i=off/kVehSize,r=off%kVehSize;if(r)snprintf(buf,sizeof(buf),"V%u+%u",i,r);else snprintf(buf,sizeof(buf),"V%u",i);return std::string(buf);}
    if(Within(W.mat,sizeof(W.mat),v,off)){const u32 i=off/(kMatWords*4),r=off%(kMatWords*4);if(r)snprintf(buf,sizeof(buf),"M%u+%u",i,r);else snprintf(buf,sizeof(buf),"M%u",i);return std::string(buf);}
    if(Within(W.ped,sizeof(W.ped),v,off)){if(off)snprintf(buf,sizeof(buf),"PED+%u",off);else snprintf(buf,sizeof(buf),"PED");return std::string(buf);}
    if(Within(W.wanted,sizeof(W.wanted),v,off)){if(off)snprintf(buf,sizeof(buf),"W+%u",off);else snprintf(buf,sizeof(buf),"W");return std::string(buf);}
    if(Local(v))return "L";
    return v;
}

// ---- eventos de los falsos (nada aqui asigna memoria)
struct Ev {u32 kind{};u32 n{};u32 ptrMask{};u32 v[24]{};};
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
inline u32 Bit(u32 kind,u32 k){return (S.mask[kind]>>(k&31))&1u;}
inline u32 Res(u32 kind){return Junk(Bit(kind,NextK(kind)));}
inline u32 Idx(u32 p){return (p-Self(W.veh))/kVehSize;}

#define EV(kind,n,mask) Ev& e=PushEv(kind,n,mask)
// ---- jugador, mundo
inline u32 __cdecl FkFpv(u32 a,u32 b){EV(kFpv,2,0);e.v[0]=a;e.v[1]=b;NextK(kFpv);return S.pv?Self(W.veh[S.pvIdx]):0u;}            // 0x56E0D0
inline u32 __cdecl FkFpp(u32 a){EV(kFpp,1,0);e.v[0]=a;NextK(kFpp);return S.pedNull?0u:Self(W.ped);}                                   // 0x56E210
inline u32 __cdecl FkFpc(u32* out,u32 id){EV(kFpc,1,0);e.v[0]=id;const u32 k=NextK(kFpc);for(int i=0;i<3;++i)out[i]=S.coord[k&7][i];return Self(out);}   // 0x56E010
inline u32 __cdecl FkFpw(u32 a){EV(kFpw,1,0);e.v[0]=a;NextK(kFpw);return Self(W.wanted);}                                             // 0x56E230
// ---- CGarage (C++)
inline u32 __fastcall FkIn3d(u32 self,void*,u32 ent,u32 radius){EV(kIn3d,3,3);e.v[0]=self;e.v[1]=ent;e.v[2]=radius;return Res(kIn3d);}   // 0x448BE0, ret 8
inline u32 __fastcall FkOut(u32 self,void*,u32 ent,u32 radius){EV(kOut,3,3);e.v[0]=self;e.v[1]=ent;e.v[2]=radius;return Res(kOut);}     // 0x448D30, ret 8
inline u32 __fastcall FkStatic(u32 self,void*){EV(kStatic,1,1);e.v[0]=self;return Res(kStatic);}                                       // 0x44A830
inline u32 __fastcall FkEmpty(u32 self,void*){EV(kEmpty,1,1);e.v[0]=self;return Res(kEmpty);}                                          // 0x44A9C0
inline u32 __fastcall FkTouch(u32 self,void*,u32 ent){EV(kTouch,2,3);e.v[0]=self;e.v[1]=ent;return Res(kTouch);}                       // 0x448EE0, ret 4
inline void __fastcall FkRemoveCars(u32 self,void*){EV(kRemoveCars,1,1);e.v[0]=self;NextK(kRemoveCars);}                                // 0x449690
inline void __fastcall FkStoreHide(u32 self,void*,u32 cars,u32 maxSlot){EV(kStoreHide,3,3);e.v[0]=self;e.v[1]=cars;e.v[2]=maxSlot;NextK(kStoreHide);}   // 0x449900, ret 8
// ---- CGarage (solo EXE)
inline u32 __fastcall FkSlideOpen(u32 self,void*){EV(kSlideOpen,1,1);e.v[0]=self;return Res(kSlideOpen);}                               // 0x44A660
inline u32 __fastcall FkSlideClosed(u32 self,void*){EV(kSlideClosed,1,1);e.v[0]=self;return Res(kSlideClosed);}                         // 0x44A750
inline u32 __fastcall FkIsOutside(u32 self,void*,u32 radius){EV(kIsOutside,2,1);e.v[0]=self;e.v[1]=radius;return Res(kIsOutside);}      // 0x448E50, ret 4
inline u32 __fastcall FkBlocking(u32 self,void*){EV(kBlocking,1,1);e.v[0]=self;return Res(kBlocking);}                                 // 0x4494F0
inline u32 __fastcall FkCountCenter(u32 self,void*,u32 veh){EV(kCountCenter,2,3);e.v[0]=self;e.v[1]=veh;return S.countCenter[NextK(kCountCenter)%3];}   // 0x4495F0, ret 4
inline u32 __fastcall FkOtherPed(u32 self,void*,u32 ped){EV(kOtherPed,2,3);e.v[0]=self;e.v[1]=ped;return Res(kOtherPed);}              // 0x449100, ret 4
inline u32 __fastcall FkOtherCar(u32 self,void*,u32 veh){EV(kOtherCar,2,3);e.v[0]=self;e.v[1]=veh;return Res(kOtherCar);}              // 0x447720, ret 4
inline void __fastcall FkThrow(u32 self,void*,u32 veh){EV(kThrow,2,3);e.v[0]=self;e.v[1]=veh;NextK(kThrow);}                           // 0x449220, ret 4
inline float __fastcall FkCalcDist(u32 self,void*,u32 x,u32 y){EV(kCalcDist,3,1);e.v[0]=self;e.v[1]=x;e.v[2]=y;return Flt(S.dist[NextK(kCalcDist)%6]);}   // 0x447D80, ret 8
inline u32 __fastcall FkRestoreHide(u32 self,void*,u32 cars){EV(kRestoreHide,2,1);e.v[0]=self;e.v[1]=cars;return Res(kRestoreHide);}    // 0x448550, ret 4
inline void __fastcall FkNeatly(u32 self,void*,u32 cars){EV(kNeatly,2,1);e.v[0]=self;e.v[1]=cars;NextK(kNeatly);}                      // 0x448330, ret 4
inline u32 __fastcall FkRestoreImp(u32 self,void*,u32 cars){EV(kRestoreImp,2,1);e.v[0]=self;e.v[1]=cars;return Res(kRestoreImp);}       // 0x4485C0, ret 4
inline void __fastcall FkStoreImp(u32 self,void*,u32 cars,u32 maxSlot){EV(kStoreImp,3,1);e.v[0]=self;e.v[1]=cars;e.v[2]=maxSlot;NextK(kStoreImp);}   // 0x449A50, ret 8
inline void __fastcall FkFindDoors(u32 self,void*,u32* first,u32* second){                                                              // 0x449FF0, ret 8
    EV(kFindDoors,3,7);e.v[0]=self;e.v[1]=Self(first);e.v[2]=Self(second);NextK(kFindDoors);
    *first=S.doors[0]<kMaxVeh?Self(W.veh[S.doors[0]]):0u;*second=S.doors[1]<kMaxVeh?Self(W.veh[S.doors[1]]):0u;
}
// ---- CGarages (C++)
inline void __cdecl FkMsg(u32 key,u32 a,u32 t,u32 b){EV(kMsg,4,0);e.v[0]=key;e.v[1]=a;e.v[2]=t;e.v[3]=b;NextK(kMsg);}                  // 0x447B80
inline u32 __cdecl FkFindSafe(u32 type){EV(kFindSafe,1,0);e.v[0]=type;NextK(kFindSafe);return (type*5u+1u)%17u;}                          // 0x4489F0
inline u32 __cdecl FkCountHide(u32 type){EV(kCountHide,1,0);e.v[0]=type;return S.countHide[NextK(kCountHide)%2];}                       // 0x44A210
inline u32 __cdecl FkSprayable(u32 veh){EV(kSprayable,1,1);e.v[0]=veh;return Res(kSprayable);}                                          // 0x4479A0
inline void __cdecl FkDetonator(){EV(kDetonator,0,0);(void)e;NextK(kDetonator);}                                                       // 0x448660
// ---- resto del mundo
inline void __cdecl FkCallOff(u32 x1,u32 y1,u32 x2,u32 y2){EV(kCallOff,4,0);e.v[0]=x1;e.v[1]=y1;e.v[2]=x2;e.v[3]=y2;NextK(kCallOff);}   // 0x566A60
inline void __cdecl FkIncStat(u32 stat,u32 value){EV(kIncStat,2,0);e.v[0]=stat;e.v[1]=value;NextK(kIncStat);}                           // 0x55C180
inline float __cdecl FkGetStat(u32 stat){EV(kGetStat,1,0);e.v[0]=stat;NextK(kGetStat);return Flt(S.stat);}                              // 0x558E40
inline void __cdecl FkSetStat(u32 stat,u32 value){EV(kSetStat,2,0);e.v[0]=stat;e.v[1]=value;NextK(kSetStat);}                           // 0x55A070
inline void __fastcall FkAudio(u32 self,void*,u32 event,u32 vol,u32 speed){EV(kAudio,3,0);(void)self;e.v[0]=event;e.v[1]=vol;e.v[2]=speed;NextK(kAudio);}   // 0x506EA0, ret 0Ch
inline void __cdecl FkReqModel(u32 model,u32 flags){EV(kReqModel,2,0);e.v[0]=model;e.v[1]=flags;NextK(kReqModel);}                       // 0x4087E0
inline void __cdecl FkHelp(u32 text,u32 b1,u32 b2,u32 b3){EV(kHelp,4,0);e.v[0]=text;e.v[1]=b1&0xFFu;e.v[2]=b2&0xFFu;e.v[3]=b3&0xFFu;NextK(kHelp);}   // 0x588BE0
inline u32 __fastcall FkTextGet(u32 self,void*,u32 key){EV(kTextGet,1,0);(void)self;e.v[0]=key;NextK(kTextGet);return key+0x1000u;}      // 0x6A0050, ret 4
inline void __fastcall FkClearWanted(u32 self,void*){EV(kClearWanted,1,1);e.v[0]=self;NextK(kClearWanted);}                            // 0x5625A0
inline u32 __fastcall FkGetRemap(u32 self,void*){EV(kGetRemap,1,1);e.v[0]=self;return S.remap[NextK(kGetRemap)%2];}                      // 0x6D0B70
inline void __fastcall FkSetRemap(u32 self,void*,u32 idx){EV(kSetRemap,2,1);e.v[0]=self;e.v[1]=idx;NextK(kSetRemap);}                    // 0x6D0C00, ret 4
inline u32 __fastcall FkAppearance(u32 self,void*){EV(kAppearance,1,1);e.v[0]=self;return S.appear[NextK(kAppearance)%4];}              // 0x6D1080
inline void __cdecl FkDestroy(u32 veh){EV(kDestroy,1,1);e.v[0]=veh;NextK(kDestroy);}                                                   // 0x6D2250
inline void __fastcall FkChoose(u32 self,void*,u8* prim,u8* sec,u8* tert,u8* quat,u32 shift){                                           // 0x4C8500, ret 14h
    EV(kChoose,2,0);(void)self;e.v[0]=1;e.v[1]=shift;NextK(kChoose);
    *prim=u8(S.colors[0]);*sec=u8(S.colors[1]);*tert=u8(S.colors[2]);*quat=u8(S.colors[3]);
}
inline void __fastcall FkAddParticle(u32 self,void*,const u32* pos,const u32* vel,u32 t,const u32* mult,u32 rz,u32 lm,u32 ll,u32 b){      // 0x4AA440, ret 20h
    EV(kAddParticle,19,0);(void)self;
    e.v[0]=1;for(int i=0;i<3;++i){e.v[1+i]=pos[i];e.v[4+i]=vel[i];}e.v[7]=t;for(int i=0;i<7;++i)e.v[8+i]=mult[i];e.v[15]=rz;e.v[16]=lm;e.v[17]=ll;e.v[18]=b&0xFFu;
    NextK(kAddParticle);
}
inline float __cdecl FkRandF(u32 lo,u32 hi){EV(kRandF,2,0);e.v[0]=lo;e.v[1]=hi;return Flt(S.randf[NextK(kRandF)%16]);}                   // 0x41BD90
inline void __fastcall FkFix(u32 self,void*){EV(kFix,1,1);e.v[0]=self;NextK(kFix);}                                                     // CVehicle::Fix (vtable +0xC8)
inline u32 __cdecl FkRand(){EV(kRand,0,0);(void)e;return Junk(Bit(kRand,NextK(kRand)));}                                                 // 0x821B1E (rand del CRT del EXE)

// ---- estado real del EXE que Update lee o escribe
struct RealWord {uintptr_t addr;unsigned size;};
inline const RealWord kReal[]{{kTimerAddr,4},{kFrameAddr,4},{kFocusAddr,1},{kMoneyBase,4},{kNoResprays,1},{kRespraysFree,1},{kBombsFree,1},{kLastGarage,4},{kLastHelp,4},{kCamOutside,1},{kCamIn,4},{kCamOut,4},
                              {kBombShop,1},{kPadW,2},{kPadB,1}};
struct RealState {std::array<u32,std::size(kReal)> v{};};
inline u32 ReadReal(const RealWord& r){u32 v{};memcpy(&v,reinterpret_cast<void*>(r.addr),r.size);return v;}
inline void WriteReal(const RealWord& r,u32 v){memcpy(reinterpret_cast<void*>(r.addr),&v,r.size);}
inline RealState SnapReal(){RealState s;for(size_t i=0;i<std::size(kReal);++i)s.v[i]=ReadReal(kReal[i]);return s;}
inline void RestoreReal(const RealState& s){for(size_t i=0;i<std::size(kReal);++i)WriteReal(kReal[i],s.v[i]);}
// El dinero del jugador en foco: PlayerInFocus se fija primero (kReal[2]) y la direccion de dinero depende de el.
inline void ApplySpecReal(const Spec& s){
    WriteReal(kReal[0],s.timer);WriteReal(kReal[1],s.frame);WriteReal(kReal[2],s.focus);
    *reinterpret_cast<u32*>(kMoneyBase+s.focus*kPlayerStride)=s.money;
    WriteReal(kReal[4],s.noResprays);WriteReal(kReal[5],s.respraysFree);WriteReal(kReal[6],s.bombsFree);WriteReal(kReal[7],s.lastGarage);WriteReal(kReal[8],s.lastHelp);
    WriteReal(kReal[9],s.camOutside);WriteReal(kReal[10],s.camIn);WriteReal(kReal[11],s.camOut);WriteReal(kReal[12],s.bombShop);WriteReal(kReal[13],s.padW);WriteReal(kReal[14],s.padB);
}
inline const char* const kRealName[]{"timer","frame","focus","money","noResprays","respraysFree","bombsFree","lastGarage","lastHelp","camOutside","camIn","camOut","bombShop","padW","padB"};
inline json RealChanges(const RealState& before,const RealState& after,u32 focus) {
    json out=json::array();
    for(size_t i=0;i<std::size(kReal);++i)if(before.v[i]!=after.v[i]) {
        u32 v=after.v[i];
        out.push_back(json::array({kRealName[i],(i==10||i==11)?Tok(v):json(v)}));
    }
    (void)focus;
    return out;
}

// ---- imagen de un caso
inline void BuildCase(const Spec& s,unsigned n,Arena& a) {
    u32* all=reinterpret_cast<u32*>(&a);
    for(size_t i=0;i<sizeof(Arena)/4;++i)all[i]=Garbage(n,u32(i));
    for(u32 i=0;i<kGuardWords;++i){a.gl[i]=0xA5A5A5A5u^i;a.gr[i]=0x5A5A5A5Au^i;}
    memset(a.veh,0,sizeof(a.veh));memset(a.mat,0,sizeof(a.mat));memset(a.ped,0,sizeof(a.ped));memset(a.wanted,0,sizeof(a.wanted));
    a.g[kGPosZ/4]=s.gb[4];a.g[kGTopZ/4]=s.gb[5];a.g[kGLeft/4]=s.gb[0];a.g[kGRight/4]=s.gb[1];a.g[kGFront/4]=s.gb[2];a.g[kGBack/4]=s.gb[3];a.g[kGDoorPos/4]=s.gdoor;a.g[kGTime/4]=s.gtime;
    a.g[kGTarget/4]=s.target<kMaxVeh?Self(W.veh[s.target]):0u;
    a.g[kGType/4]=(s.gtype&0xFF)|((s.gstate&0xFF)<<8)|((s.gflags&0xFF)<<16)|(0xA5u<<24);
    for(u32 i=0;i<kMaxVeh;++i) {
        const Veh& v=s.veh[i];
        u8* b=reinterpret_cast<u8*>(a.veh[i]);
        auto W32=[&](u32 off,u32 val){memcpy(b+off,&val,4);};
        W32(0,Self(W.vt));
        for(int k=0;k<3;++k)W32(4+4*k,v.pos[k]);
        W32(0x14,v.hasMat?Self(W.mat[i]):0u);
        {const u16 m=u16(v.model);memcpy(b+0x22,&m,2);}
        b[0x42E]=u8(v.f42E);b[0x42F]=u8(v.f42F);for(int k=0;k<4;++k)b[0x434+k]=u8(v.col[k]);
        b[0x4A8]=u8(v.f4A8);W32(0x4B0,v.f4B0);W32(0x4C0,v.health);W32(0x4CC,v.f4CC);W32(0x590,v.t590);W32(0x594,v.t594);W32(0x7BC,v.f7BC);b[0x868]=u8(v.f868);W32(0x8E4,v.f8E4);
        for(int k=0;k<3;++k){a.mat[i][k]=v.mright[k];a.mat[i][8+k]=v.mup[k];a.mat[i][12+k]=v.mpos[k];}
    }
    u8* p=reinterpret_cast<u8*>(a.ped);
    memcpy(p+0x46C,&s.pedFlags,4);
    {const u32 pv=s.pedVeh<kMaxVeh?Self(W.veh[s.pedVeh]):0u;memcpy(p+0x58C,&pv,4);}
    u8* w=reinterpret_cast<u8*>(a.wanted);
    w[0x1E]=u8(s.wByte);memcpy(w+0x2C,&s.wDword,4);
    a.vt[50]=Self(&FkFix);
}
inline json ChangesJson(const Arena& post,const Arena& ref) {
    json out=json::array();
    const u32* pa=reinterpret_cast<const u32*>(&post);const u32* ra=reinterpret_cast<const u32*>(&ref);
    for(u32 w=0;w<kGarageSize/4;++w)if(post.g[w]!=ref.g[w])out.push_back(json::array({"G",0,w,post.g[w]}));
    for(u32 i=0;i<kMaxVeh;++i)for(u32 w=0;w<kVehSize/4;++w)if(post.veh[i][w]!=ref.veh[i][w])out.push_back(json::array({"V",i,w,Tok(post.veh[i][w])}));
    for(u32 i=0;i<kMaxVeh;++i)for(u32 w=0;w<kMatWords;++w)if(post.mat[i][w]!=ref.mat[i][w])out.push_back(json::array({"M",i,w,post.mat[i][w]}));
    for(u32 w=0;w<kWantedWords;++w)if(post.wanted[w]!=ref.wanted[w])out.push_back(json::array({"W",0,w,post.wanted[w]}));
    // X: cualquier otra palabra (guardas, jugador, vtable...): no deberia cambiar nunca
    for(size_t i=0;i<sizeof(Arena)/4;++i) {
        if(pa[i]==ra[i])continue;
        const u32 addr=Self(&pa[i])-Self(&post);
        auto in=[&](const void* base,size_t bytes){const u32 b=Self(base)-Self(&post);return addr>=b&&addr<b+bytes;};
        if(in(post.g,sizeof(post.g))||in(post.veh,sizeof(post.veh))||in(post.mat,sizeof(post.mat))||in(post.wanted,sizeof(post.wanted)))continue;
        out.push_back(json::array({"X",0,u32(i),pa[i]}));
    }
    return out;
}

inline void RunCase(Env& env,Totals& t,unsigned n) {
    const Spec s=MakeSpec(n);
    const json input=json::parse(JsonSpec(s));
    BuildCase(s,n,WRef);
    Side sides[2];json rows[2];
    u32 args[1]{s.garageId};
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        S=s;memset(cnt,0,sizeof(cnt));nev=0;
        const RealState before=SnapReal();
        ApplySpecReal(s);
        const RealState applied=SnapReal();
        sides[sd]=Execute(env,0,kEntry[0],candidate,[&](uintptr_t entry){return Gate(entry,args,1,s.regs[1],Self(W.g),s.regs[3],s.regs[0],0);});
        const RealState after=SnapReal();
        RestoreReal(before);
        const bool restored=SnapReal().v==before.v;
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["changes"]=ChangesJson(W,WRef);obs["globals"]=RealChanges(applied,after,s.focus);
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=restored&&watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[sd]=out;
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFns;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    json colors=json::array();for(int i=0;i<256;++i)colors.push_back(*reinterpret_cast<u32*>(0xB4E480+4*i));
    const RealState initial=SnapReal();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)},{"color_table",colors}});
    Totals totals;
    if(ready) {
        const unsigned before=totals.failures;
        for(unsigned c=dbgFirst;c<=dbgLast&&c<kCases;++c){if(dbgMode)Write({{"event","case_start"},{"case",c}});RunCase(env,totals,c);}
        Write({{"event","function_summary"},{"function",kFn[0]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
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
