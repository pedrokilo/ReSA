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
// Nucleo del banco Garage63 (lote 63). Plantilla: reversing/D3DResource63 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).

namespace G63Live {
using namespace G63Fix;
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
inline constexpr const char* kFn[kFns]{"CGarage::InitDoorsAtStart","CGarage::PlayerArrestedOrDied","CGarage::IsEntityEntirelyInside3D","CGarage::IsEntityEntirelyOutside","CGarage::IsStaticPlayerCarEntirelyInside",
                                      "CGarage::IsGarageEmpty","CGarage::RemoveCarsBlockingDoorNotInside","CGarage::TidyUpGarage","CGarage::TidyUpGarageClose"};
inline constexpr const char* kName[kFns]{"InitDoorsAtStart","PlayerArrestedOrDied","IsEntityEntirelyInside3D","IsEntityEntirelyOutside","IsStaticPlayerCarEntirelyInside","IsGarageEmpty",
                                        "RemoveCarsBlockingDoorNotInside","TidyUpGarage","TidyUpGarageClose"};
inline constexpr uintptr_t kEntry[kFns]{0x447600,0x4486C0,0x448BE0,0x448D30,0x44A830,0x44A9C0,0x449690,0x449C50,0x449D10};
inline constexpr unsigned kMaxEvents=400;

struct Arena {
    alignas(16) u32 gl[kGuardWords];
    u32 g[kGarageSize/4];
    u32 gr[kGuardWords];
    alignas(16) u32 veh[kMaxVeh][kVehSize/4];
    alignas(16) u8 poolFlags[16];
    alignas(16) u32 pool[8];
    alignas(16) u32 mat[kMaxVeh][kMatWords];
    alignas(16) u32 cm[kMaxVeh][kCmWords];
    alignas(16) u32 cd[kMaxVeh][kCdWords];
    alignas(16) u32 sph[kMaxVeh][kMaxSph][kSphWords];
    alignas(16) u32 ped[kPedWords];
    alignas(16) u32 intel[kIntelWords];
    alignas(16) u32 vt[4];
};
alignas(16) inline Arena W{},WRef{};
inline u32 Self(const void* p){return u32(uintptr_t(p));}
inline bool Within(const void* base,size_t bytes,u32 v,u32& off){const u32 b=Self(base);if(v>=b&&v<b+bytes){off=v-b;return true;}return false;}
inline u32 Local(u32 v){const char here=0;const auto d=intptr_t(v)-intptr_t(uintptr_t(&here));return d>-0x10000&&d<0x10000;}
inline u32 IdxOf(u32 ent){return (ent-Self(W.veh))/kVehSize;}

// ---- fichas simbolicas de punteros
inline json Tok(u32 v) {
    char buf[48];
    u32 off{};
    if(v==0)return 0;
    if(Within(W.g,sizeof(W.g),v,off)){if(off)snprintf(buf,sizeof(buf),"G+%u",off);else snprintf(buf,sizeof(buf),"G");return std::string(buf);}
    if(Within(W.veh,sizeof(W.veh),v,off)){const u32 i=off/kVehSize,r=off%kVehSize;if(r)snprintf(buf,sizeof(buf),"V%u+%u",i,r);else snprintf(buf,sizeof(buf),"V%u",i);return std::string(buf);}
    if(Within(W.mat,sizeof(W.mat),v,off)){const u32 i=off/(kMatWords*4),r=off%(kMatWords*4);if(r)snprintf(buf,sizeof(buf),"M%u+%u",i,r);else snprintf(buf,sizeof(buf),"M%u",i);return std::string(buf);}
    if(Within(W.cm,sizeof(W.cm),v,off)){const u32 i=off/(kCmWords*4);snprintf(buf,sizeof(buf),"CM%u+%u",i,off%(kCmWords*4));return std::string(buf);}
    if(Within(W.cd,sizeof(W.cd),v,off)){const u32 i=off/(kCdWords*4);snprintf(buf,sizeof(buf),"CD%u+%u",i,off%(kCdWords*4));return std::string(buf);}
    if(Within(W.sph,sizeof(W.sph),v,off)){const u32 per=kMaxSph*kSphWords*4,i=off/per,j=(off%per)/(kSphWords*4),r=off%(kSphWords*4);if(r)snprintf(buf,sizeof(buf),"S%u.%u+%u",i,j,r);else snprintf(buf,sizeof(buf),"S%u.%u",i,j);return std::string(buf);}
    if(Within(W.ped,sizeof(W.ped),v,off)){snprintf(buf,sizeof(buf),"PED+%u",off);return std::string(buf);}
    if(Within(W.intel,sizeof(W.intel),v,off)){snprintf(buf,sizeof(buf),"INT+%u",off);return std::string(buf);}
    if(Within(W.pool,sizeof(W.pool),v,off)){snprintf(buf,sizeof(buf),"POOL+%u",off);return std::string(buf);}
    if(Within(W.vt,sizeof(W.vt),v,off)){snprintf(buf,sizeof(buf),"VT+%u",off);return std::string(buf);}
    if(Local(v))return "L";
    return v;
}

// ---- eventos de los falsos (nada aqui asigna memoria)
struct Ev {u32 kind{};u32 n{};u32 ptrMask{};u32 v[16]{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline Ev& PushEv(u32 kind,u32 n,u32 ptrMask){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{};e.kind=kind;e.n=n;e.ptrMask=ptrMask;++nev;return e;}
inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){const Ev& e=evs[i];json row=json::array({e.kind});for(u32 k=0;k<e.n;++k)row.push_back((e.ptrMask>>k)&1u?Tok(e.v[k]):json(e.v[k]));out.push_back(row);}
    return out;
}
inline Spec S;                       // caso en curso
inline u32 cnt[32]{};
inline u32 NextK(unsigned kind){return cnt[kind]++;}
inline u32 Junk(u32 bit){return (bit&1u)|0xA5A5A500u;}   // AL = bit, resto de EAX basura (los llamadores del EXE usan `test al,al`)
inline u32 BitOf(u32 mask,u32 k){return (mask>>(k&31))&1u;}
enum {kECol=1,kEMat,kEAlloc,kEUpd,kEXform,kEIpr,kEIpp,kETouch,kEFind,kEFpv,kEFpp,kETask,kEDel,kERemove,kEDtor,kEIn3d};

inline void Reenter(u32 which,u32 k) {   // which 1 = en Transform, 2 = en IsPointInsideGarage(radio)
    if(S.reKind==which&&k==S.reK&&S.reIdx<kMaxVeh)*reinterpret_cast<u16*>(&W.cd[S.reIdx][0])=u16(S.reVal);
}
inline u32 __fastcall FkCol(u32 self,void*){PushEv(kECol,1,1).v[0]=self;return Self(W.cm[IdxOf(self)%kMaxVeh]);}                      // 0x535300
inline u32 __fastcall FkMat(u32 self,void*){PushEv(kEMat,1,1).v[0]=self;return Self(W.mat[IdxOf(self)%kMaxVeh]);}                      // 0x411990
inline void __fastcall FkAlloc(u32 self,void*){PushEv(kEAlloc,1,1).v[0]=self;*reinterpret_cast<u32*>(self+kOffMatrix)=Self(W.mat[IdxOf(self)%kMaxVeh]);}   // 0x54F560
inline void __fastcall FkUpd(u32 self,void*,u32 mat){Ev& e=PushEv(kEUpd,2,3);e.v[0]=self;e.v[1]=mat;}                              // 0x54F1B0, ret 4
inline u32 __cdecl FkXform(u32* out,u32 mat,u32 vec) {                                                                             // 0x59C890
    Ev& e=PushEv(kEXform,3,7);e.v[0]=Self(out);e.v[1]=mat;e.v[2]=vec;
    const u32 k=NextK(kEXform);
    for(u32 i=0;i<3;++i)out[i]=Rand(S.seed,k*4+i+1);
    Reenter(1,k);
    return Self(out);
}
#pragma runtime_checks("", off)
// Como los originales (0x4487D0 / 0x448740): ECX y EDX quedan intactos (los llamadores del EXE los reutilizan tras la llamada: TidyUpGarage guarda el puntero de matriz en EDX, 0x448BE0 la direccion de las esferas).
__declspec(safebuffers) inline u32 __fastcall FkIpr(u32 self,void* entryEdx,u32 x,u32 y,u32 z,u32 r) {                           // 0x4487D0 y CGarage::IsPointInsideGarage(point, radius), ret 10h
    Ev& e=PushEv(kEIpr,5,1);e.v[0]=self;e.v[1]=x;e.v[2]=y;e.v[3]=z;e.v[4]=r;
    const u32 k=NextK(kEIpr);
    Reenter(2,k);
    return ResidueRegsCpp(self,Self(entryEdx),Junk(BitOf(S.radMask,k)));
}
__declspec(safebuffers) inline u32 __fastcall FkIpp(u32 self,void* entryEdx,u32 x,u32 y,u32 z) {                                 // 0x448740 y CGarage::IsPointInsideGarage(point), ret 0Ch
    Ev& e=PushEv(kEIpp,4,1);e.v[0]=self;e.v[1]=x;e.v[2]=y;e.v[3]=z;
    return ResidueRegsCpp(self,Self(entryEdx),Junk(BitOf(S.pointMask,NextK(kEIpp))));
}
#pragma runtime_checks("", restore)
inline u32 __fastcall FkTouch(u32 self,void*,u32 ent){Ev& e=PushEv(kETouch,2,3);e.v[0]=self;e.v[1]=ent;return Junk(BitOf(S.touchMask,NextK(kETouch)));}   // 0x448EE0, ret 4
inline void __cdecl FkFind(const u32* a,const u32* b,u32 cntp,u32 mx,u32 list,u32 b1,u32 b2,u32 b3,u32 b4,u32 b5) {                // 0x568DD0
    Ev& e=PushEv(kEFind,14,(1u<<6)|(1u<<8));
    e.v[0]=a[0];e.v[1]=a[1];e.v[2]=a[2];e.v[3]=b[0];e.v[4]=b[1];e.v[5]=b[2];e.v[6]=cntp;e.v[7]=mx;e.v[8]=list;e.v[9]=b1;e.v[10]=b2;e.v[11]=b3;e.v[12]=b4;e.v[13]=b5;
    *reinterpret_cast<i16*>(cntp)=i16(S.cnt);
    for(u32 i=0;i<16;++i)reinterpret_cast<u32*>(list)[i]=Self(W.veh[S.listIdx[i]%kMaxVeh]);
}
inline u32 __cdecl FkFpv(u32 a,u32 b){Ev& e=PushEv(kEFpv,2,0);e.v[0]=a;e.v[1]=b;const u32 k=NextK(kEFpv);return (S.pv&&k!=S.pvNullAt)?Self(W.veh[0]):0u;}   // 0x56E0D0
inline u32 __cdecl FkFpp(u32 a){PushEv(kEFpp,1,0).v[0]=a;return Self(W.ped);}                                                         // 0x56E210
inline u32 __fastcall FkTask(u32 self,void*,u32 type){Ev& e=PushEv(kETask,2,1);e.v[0]=self;e.v[1]=type;return S.task?S.task:0u;}       // 0x681740, ret 4
inline u32 __fastcall FkDel(u32 self,void*){PushEv(kEDel,1,1).v[0]=self;return Junk(BitOf(S.delMask,NextK(kEDel)));}                    // 0x6D1180
inline void __cdecl FkRemove(u32 ent){PushEv(kERemove,1,1).v[0]=ent;}                                                                  // 0x563280
inline u32 __fastcall FkDtor(u32 self,void*,u32 flag){Ev& e=PushEv(kEDtor,2,1);e.v[0]=self;e.v[1]=flag;return self;}                    // vtable[0], ret 4
inline u32 __fastcall FkIn3d(u32 self,void*,u32 ent,u32 radius){Ev& e=PushEv(kEIn3d,3,3);e.v[0]=self;e.v[1]=ent;e.v[2]=radius;return Junk(BitOf(S.inMask,NextK(kEIn3d)));}   // 0x448BE0, ret 8

// ---- imagen de un caso
inline void BuildCase(const Spec& s,unsigned n,Arena& a) {
    u32* all=reinterpret_cast<u32*>(&a);
    for(size_t i=0;i<sizeof(Arena)/4;++i)all[i]=Garbage(n,u32(i));
    for(u32 i=0;i<kGuardWords;++i){a.gl[i]=0xA5A5A5A5u^i;a.gr[i]=0x5A5A5A5Au^i;}
    a.g[kGPosZ/4]=s.gb[4];a.g[kGTopZ/4]=s.gb[5];a.g[kGLeft/4]=s.gb[0];a.g[kGRight/4]=s.gb[1];a.g[kGFront/4]=s.gb[2];a.g[kGBack/4]=s.gb[3];a.g[kGDoorPos/4]=s.gdoor;a.g[kGTime/4]=s.gtime;
    a.g[kGType/4]=(s.gtype&0xFF)|((s.gstate&0xFF)<<8)|((s.gflags&0xFF)<<16)|(0xA5u<<24);
    for(u32 i=0;i<kMaxVeh;++i) {
        const Veh& v=s.veh[i];
        u32* e=a.veh[i];
        e[0]=Self(W.vt);
        for(int k=0;k<3;++k){e[1+k]=v.pos[k];e[kOffSpeed/4+k]=v.speed[k];}
        e[kOffMatrix/4]=v.hasMat?Self(W.mat[i]):0u;
        reinterpret_cast<u8*>(e)[kOffB36]=u8(v.b36);
        reinterpret_cast<u8*>(e)[kOffFlags428]=u8((reinterpret_cast<u8*>(e)[kOffFlags428]&0xF7u)|(v.locked?8u:0u));
        e[kOffVType/4]=v.type;
        a.mat[i][10]=v.upz;for(int k=0;k<3;++k)a.mat[i][12+k]=v.mpos[k];
        a.cm[i][11]=v.hasCol?Self(W.cd[i]):0u;
        a.cd[i][0]=0xDEAD0000u|(v.nsph&0xFFFFu);a.cd[i][2]=Self(W.sph[i][0]);
        for(u32 j=0;j<kMaxSph;++j)a.sph[i][j][3]=v.sphR[j];
        a.poolFlags[i]=u8(v.freeSlot?(0x80u|i):i);
    }
    a.pool[0]=Self(W.veh);a.pool[1]=Self(W.poolFlags);a.pool[2]=s.nveh;
    a.ped[0x47C/4]=Self(W.intel);
    a.vt[0]=Self(&FkDtor);
}
inline u32 snapPool{};
inline json ChangesJson(const Arena& post,const Arena& ref) {
    json out=json::array();
    const u32* pa=reinterpret_cast<const u32*>(&post);const u32* ra=reinterpret_cast<const u32*>(&ref);
    // D: CCollisionData
    for(u32 i=0;i<kMaxVeh;++i)for(u32 w=0;w<kCdWords;++w)if(post.cd[i][w]!=ref.cd[i][w])out.push_back(json::array({"D",i,w,post.cd[i][w]}));
    for(u32 w=0;w<kGarageSize/4;++w)if(post.g[w]!=ref.g[w])out.push_back(json::array({"G",0,w,post.g[w]}));
    for(u32 i=0;i<kMaxVeh;++i)for(u32 w=0;w<kVehSize/4;++w)if(post.veh[i][w]!=ref.veh[i][w])out.push_back(json::array({"V",i,w,Tok(post.veh[i][w])}));
    // X: cualquier otra palabra de la arena (guardas, matrices, colision, esferas, pool, jugador...): no deberia cambiar nunca
    for(size_t i=0;i<sizeof(Arena)/4;++i) {
        if(pa[i]==ra[i])continue;
        const u32 addr=Self(&pa[i])-Self(&post);
        auto in=[&](const void* base,size_t bytes){const u32 b=Self(base)-Self(&post);return addr>=b&&addr<b+bytes;};
        if(in(post.g,sizeof(post.g))||in(post.veh,sizeof(post.veh))||in(post.cd,sizeof(post.cd)))continue;
        out.push_back(json::array({"X",0,u32(i),pa[i]}));
    }
    return out;
}

inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    BuildCase(s,n,WRef);
    Side sides[2];json rows[2];
    u32 args[2]{};u32 argCount=0;
    if(fn==kInside3D||fn==kOutside){args[0]=Self(W.veh[0]);args[1]=s.radius;argCount=2;}
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        S=s;memset(cnt,0,sizeof(cnt));nev=0;
        const u32 poolBefore=*reinterpret_cast<u32*>(kPoolAddr);
        *reinterpret_cast<u32*>(kPoolAddr)=Self(W.pool);
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t entry){return Gate(entry,args,argCount,s.regs[1],Self(W.g),s.regs[3],s.regs[0],0);});
        const u32 poolAfter=*reinterpret_cast<u32*>(kPoolAddr);
        *reinterpret_cast<u32*>(kPoolAddr)=poolBefore;
        const bool restored=*reinterpret_cast<u32*>(kPoolAddr)==poolBefore&&poolAfter==Self(W.pool);
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["changes"]=ChangesJson(W,WRef);
        if(fn>=kInside3D&&fn<=kEmpty)obs["al"]=sides[sd].reg.eax&0xFFu;
        if(fn==kArrested){obs["ecx"]=Tok(sides[sd].reg.ecx);obs["edx"]=sides[sd].reg.edx;}
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
    const u32 initialPool=*reinterpret_cast<u32*>(kPoolAddr);
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)},{"initial_pool",initialPool}});
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
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool poolNow=*reinterpret_cast<u32*>(kPoolAddr)==initialPool;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"pool_restored",poolNow}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"pool_restored",poolNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
