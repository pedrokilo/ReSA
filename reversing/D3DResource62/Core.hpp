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
// Nucleo del banco D3DResource62 (lote 62). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute. Plantilla: reversing/Misc61 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE).
//
// Banco par a par de SIETE funciones de D3DResourceSystem. Original = bytes del EXE con el hook RH de la funcion apagado; candidato = DLL con SOLO ese hook encendido.
// NO se toca el dispositivo D3D real: _RwD3DDevice (0xC97C28) apunta durante la llamada a un dispositivo falso y todos los recursos (texturas e index buffers) son objetos COM falsos con
// vtables propias. El estado de las caches (0xC87C60..0xC87FE8 y el byte 0x8D6084) se guarda antes de cada lado, se sustituye por el del caso y se restaura justo despues, antes de cualquier
// Write. Colaboradores del EXE sin reconstruir (D3DTextureBuffer::Pop/Setup/Push/PushWithoutIncreasingCounter/Resize, D3DIndexDataBuffer::Pop/Resize) y new/delete del EXE (0x82119A/0x8214BD,
// solo en el hilo del banco y con la ventana armada) se sustituyen por falsos identicos en ambos lados que registran sus argumentos EN ORDEN.
// EAX/ECX/EDX de salida solo se registran, salvo EAX (HRESULT) de CreateTexture / CreateIndexBuffer, que los llamadores del EXE (0x4CB935, 0x4CD9FE, 0x4C997C) si leen.

namespace D3D62Live {
using namespace D3D62Fix;
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
    std::vector<StubSpec> stubs;                                    // colaboradores sustituidos (los mismos para todas las funciones: cualquier llamada no esperada queda registrada)
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
        for(const auto& s:env.stubs) {
            const auto& pr=s.pair;
            collab&=!ReversingProbe::JumpsToDll(pr.exe)&&pr.dll==0&&!pr.hooked;
        }
        side.collabOK=collab;
        {
            Patches patches;
            for(const auto& s:env.stubs)patches.Add(s.pair,s.fake);
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

// ================================================================================================ arena del banco, falsos y fixtures
inline constexpr const char* kFn[kFns]{"D3DResourceSystem::CancelBuffering","D3DResourceSystem::CreateIndexBuffer","D3DResourceSystem::CreateTexture","D3DResourceSystem::DestroyIndexBuffer",
                                       "D3DResourceSystem::DestroyTexture","D3DResourceSystem::Init","D3DResourceSystem::Shutdown"};
inline constexpr const char* kName[kFns]{"CancelBuffering","CreateIndexBuffer","CreateTexture","DestroyIndexBuffer","DestroyTexture","Init","Shutdown"};
inline constexpr uintptr_t kEntry[kFns]{0x730900,0x7306A0,0x730510,0x730D30,0x730B70,0x730830,0x730A00};
inline constexpr u32 kResWordsTotal=kResCount*kResWords;
// Toda la memoria de los falsos vive aqui (una imagen de referencia por caso y una de trabajo por lado); lo que no sea la region de globales del EXE.
struct Arena {
    alignas(16) u32 res[2][kResCount][kResWords];      // 0 = texturas falsas, 1 = index buffers falsos
    alignas(16) u32 stacks[kStacks][kStackWords];
    alignas(16) u32 newMem[kNewBlocks][kNewBlockWords];
    alignas(16) u32 out[kOutWords];
    alignas(16) u32 dev[kResWords];
};
alignas(16) inline Arena W{},WRef{};
inline void* texVt[32]{};
inline void* ibVt[32]{};
inline void* devVt[128]{};

// ---- eventos de los falsos (nada aqui asigna memoria: se usan dentro de la ventana de parches)
struct Ev {u32 kind{};u32 n{};u32 ptrMask{};u32 v[10]{};};
inline std::array<Ev,640> evs{};inline unsigned nev{};inline Ev evSink{};
inline Ev& PushEv(u32 kind,u32 n,u32 ptrMask) {
    Ev& e=nev<evs.size()?evs[nev]:evSink;
    e=Ev{};e.kind=kind;e.n=n;e.ptrMask=ptrMask;++nev;return e;
}
struct FakeState {
    bool armed{};DWORD tid{};
    u32 popRet{},devRet{},devWrite{},devRes{},rszMode{};
    u32 reK{0xFFFFFFFFu},reKind{},reIdx{},reVal{};
    u32 releases{},newCount{};
    std::array<u32,18> newRet{};
};
inline FakeState F;
inline u32& RegionWord(u32 w){return *reinterpret_cast<u32*>(kRegionBase+4*w);}
inline u32 Self(const void* p){return u32(uintptr_t(p));}
inline u32 Local(const void* p){const char here=0;const auto d=intptr_t(uintptr_t(p))-intptr_t(uintptr_t(&here));return d>-0x4000&&d<0x4000;}

// ---- recursos COM falsos (IUnknown::Release = ranura 2; textura: GetLevelCount 13, GetLevelDesc 17; index buffer: GetDesc 13) y dispositivo (CreateTexture 23, CreateIndexBuffer 27)
inline void ApplyReentrancy() {
    switch(F.reKind) {
    case 1: RegionWord(0)=F.reVal;break;
    case 2: RegionWord(kTexBase+kTexStride*F.reIdx+5)=F.reVal;break;
    case 3: RegionWord(kIbBase+kIbStride*F.reIdx+4)=F.reVal;break;
    case 4: RegionWord(kGenBase+4)=F.reVal;break;
    default: break;
    }
}
inline unsigned long __stdcall FkRelease(u32* self) {
    Ev& e=PushEv(1,1,1);e.v[0]=Self(self);
    const u32 k=F.releases++;
    if(k==F.reK&&F.reKind)ApplyReentrancy();
    return 0;
}
inline unsigned long __stdcall FkGetLevelCount(u32* self) {Ev& e=PushEv(2,1,1);e.v[0]=Self(self);return self[2];}
inline long __stdcall FkGetLevelDesc(u32* self,u32 level,u32* desc) {
    Ev& e=PushEv(3,3,1);e.v[0]=Self(self);e.v[1]=level;e.v[2]=Local(desc);
    desc[0]=self[3];desc[1]=1;desc[2]=0;desc[3]=1;desc[4]=0;desc[5]=0;desc[6]=self[4];desc[7]=self[5];
    return 0;
}
inline long __stdcall FkGetDesc(u32* self,u32* desc) {
    Ev& e=PushEv(4,2,1);e.v[0]=Self(self);e.v[1]=Local(desc);
    desc[0]=0x65;desc[1]=10;desc[2]=8;desc[3]=1;desc[4]=self[6];
    return 0;
}
inline long __stdcall FkDevCreateTexture(u32* dev,u32 w,u32 h,u32 lv,u32 usage,u32 fmt,u32 pool,u32** pp,void* shared) {
    Ev& e=PushEv(5,9,0x81);e.v[0]=Self(dev);e.v[1]=w;e.v[2]=h;e.v[3]=lv;e.v[4]=usage;e.v[5]=fmt;e.v[6]=pool;e.v[7]=Self(pp);e.v[8]=Self(shared);
    if(F.devWrite==1)*pp=W.res[0][F.devRes];else if(F.devWrite==2)*pp=nullptr;
    return long(F.devRet);
}
inline long __stdcall FkDevCreateIb(u32* dev,u32 len,u32 usage,u32 fmt,u32 pool,u32** pp,void* shared) {
    Ev& e=PushEv(6,7,0x21);e.v[0]=Self(dev);e.v[1]=len;e.v[2]=usage;e.v[3]=fmt;e.v[4]=pool;e.v[5]=Self(pp);e.v[6]=Self(shared);
    if(F.devWrite==1)*pp=W.res[1][F.devRes];else if(F.devWrite==2)*pp=nullptr;
    return long(F.devRet);
}
template<int N> inline long __stdcall FkTrap() {Ev& e=PushEv(16,1,0);e.v[0]=N;return 0;}
template<int... N> inline void FillTraps(void** vt,std::integer_sequence<int,N...>) {((vt[N]=reinterpret_cast<void*>(&FkTrap<N>)),...);}

// ---- colaboradores del EXE sin reconstruir (thiscall con `ret n` = __fastcall con los mismos argumentos de pila)
inline void* PopResult(int kind) {return F.popRet?static_cast<void*>(W.res[kind][F.popRet-1]):nullptr;}
inline void* __fastcall FkTexPop(u32* self,void*,u32 fmt,u32 w,u32 h,u32 lv) {   // 0x72FF60, ret 10h
    Ev& e=PushEv(7,5,1);e.v[0]=Self(self);e.v[1]=fmt;e.v[2]=w;e.v[3]=h;e.v[4]=lv;return PopResult(0);
}
inline void __fastcall FkTexSetup(u32* self,void*,u32 fmt,u32 width,u32 lvl,u32 cap) {   // 0x72FE80, ret 10h (el real reserva cap*4 bytes con el new del EXE; aqui el bloque 19 del banco)
    Ev& e=PushEv(8,5,1);e.v[0]=Self(self);e.v[1]=fmt;e.v[2]=width;e.v[3]=lvl;e.v[4]=cap;
    self[0]=fmt;self[1]=width;self[2]=lvl;self[6]=Self(W.newMem[19]);self[3]=cap;self[5]=0;self[4]=0;
}
inline u32 __fastcall FkTexPush(u32* self,void*,u32 tex) {   // 0x72FFF0, ret 4: cuenta +1 y guarda si hay sitio
    Ev& e=PushEv(9,2,3);e.v[0]=Self(self);e.v[1]=tex;
    const i32 size=i32(self[5]),cap=i32(self[3]);
    self[4]=self[4]+1;
    if(size<cap){reinterpret_cast<u32*>(self[6])[size]=tex;self[5]=self[5]+1;return 1;}
    return 0;
}
inline u32 __fastcall FkTexPushWithout(u32* self,void*,u32 tex) {   // 0x730AD0, ret 4: guarda si hay sitio, sin tocar el contador
    Ev& e=PushEv(10,2,3);e.v[0]=Self(self);e.v[1]=tex;
    const i32 size=i32(self[5]),cap=i32(self[3]);
    if(size<cap){reinterpret_cast<u32*>(self[6])[size]=tex;self[5]=self[5]+1;return 1;}
    return 0;
}
inline void Resize(u32* capWord,u32 newCap) {
    switch(F.rszMode) {
    case 1: *capWord=newCap;break;
    case 2: *capWord=0;break;
    case 3: *capWord=newCap-1;break;
    default: break;
    }
}
inline void __fastcall FkTexResize(u32* self,void*,u32 cap) {Ev& e=PushEv(11,2,1);e.v[0]=Self(self);e.v[1]=cap;Resize(&self[3],cap);}   // 0x730020, ret 4
inline void* __fastcall FkIbPop(u32* self,void*,u32 count) {Ev& e=PushEv(12,2,1);e.v[0]=Self(self);e.v[1]=count;return PopResult(1);}   // 0x730270, ret 4
inline void __fastcall FkIbResize(u32* self,void*,u32 cap) {Ev& e=PushEv(13,2,1);e.v[0]=Self(self);e.v[1]=cap;Resize(&self[2],cap);}   // 0x730330, ret 4

// ---- new / delete del EXE (0x82119A / 0x8214BD): solo en la ventana armada y en el hilo del banco; en cualquier otro caso se reenvia al codigo real del EXE
inline void* ForeignNew(size_t size) {
    for(;;) {
        if(void* p=reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(size))return p;
        if(!reinterpret_cast<int(__cdecl*)(size_t)>(0x826A57)(size))reinterpret_cast<void(__cdecl*)()>(0x8212ED)();
    }
}
inline bool InNew(u32 p) {return p>=Self(W.newMem)&&p<Self(W.newMem)+sizeof(W.newMem);}
inline bool InStacks(u32 p) {return p>=Self(W.stacks)&&p<Self(W.stacks)+sizeof(W.stacks);}
inline void* __cdecl FkNew(size_t size) {
    if(!F.armed||GetCurrentThreadId()!=F.tid)return ForeignNew(size);
    Ev& e=PushEv(14,1,0);e.v[0]=u32(size);
    const u32 j=F.newCount++;
    const u32 want=j<F.newRet.size()?F.newRet[j]:1u;
    if(want==1)return W.newMem[j<kNewBlocks-1?j:kNewBlocks-2];
    return reinterpret_cast<void*>(uintptr_t(want));
}
inline void __cdecl FkDelete(void* p) {
    if(!F.armed||GetCurrentThreadId()!=F.tid||!(InNew(Self(p))||InStacks(Self(p)))) {
        if(F.armed&&GetCurrentThreadId()==F.tid)PushEv(15,1,1).v[0]=Self(p);
        reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p);
        return;
    }
    Ev& e=PushEv(15,1,1);e.v[0]=Self(p);
}

// ---- fichas simbolicas de punteros (el oraculo y el banco hablan en estas fichas, nunca en direcciones)
inline json Tok(u32 v) {
    if(v==0)return 0;
    char buf[48];
    auto within=[&](const void* base,size_t bytes,u32& off){const u32 b=Self(base);if(v>=b&&v<b+bytes){off=v-b;return true;}return false;};
    u32 off{};
    for(int kind=0;kind<2;++kind)
        if(within(W.res[kind],sizeof(W.res[kind]),off)) {
            const u32 id=off/(kResWords*4),rem=off%(kResWords*4);
            if(rem)snprintf(buf,sizeof(buf),"%c%u+%u",kind?'B':'R',id,rem);else snprintf(buf,sizeof(buf),"%c%u",kind?'B':'R',id);
            return std::string(buf);
        }
    if(within(W.stacks,sizeof(W.stacks),off)) {
        const u32 k=off/(kStackWords*4),rem=off%(kStackWords*4);
        const int slot=int(rem/4)-int(kStackGuard);
        if(rem==kStackGuard*4)snprintf(buf,sizeof(buf),"S%u",k);else snprintf(buf,sizeof(buf),"S%u@%d.%u",k,slot,rem%4);
        return std::string(buf);
    }
    if(within(W.out,sizeof(W.out),off)) {snprintf(buf,sizeof(buf),"O%u.%u",off/4,off%4);return std::string(buf);}
    if(within(W.newMem,sizeof(W.newMem),off)) {
        const u32 j=off/(kNewBlockWords*4),rem=off%(kNewBlockWords*4);
        if(rem)snprintf(buf,sizeof(buf),"N%u+%u",j,rem);else snprintf(buf,sizeof(buf),"N%u",j);
        return std::string(buf);
    }
    if(within(W.dev,sizeof(W.dev),off))return off?std::string("D+")+std::to_string(off):std::string("D");
    if(v>=kRegionBase&&v<kRegionBase+kRegionWords*4) {
        const u32 w=(v-kRegionBase)/4,rem=(v-kRegionBase)%4;
        if(w>=kTexBase&&w<kTexBase+kTexStride*kTexBuffers&&!rem) {
            const u32 i=(w-kTexBase)/kTexStride,f=(w-kTexBase)%kTexStride;
            if(f)snprintf(buf,sizeof(buf),"T%u+%u",i,f);else snprintf(buf,sizeof(buf),"T%u",i);
            return std::string(buf);
        }
        if(w>=kIbBase&&w<kGenBase&&!rem) {
            const u32 i=(w-kIbBase)/kIbStride,f=(w-kIbBase)%kIbStride;
            if(f)snprintf(buf,sizeof(buf),"I%u+%u",i,f);else snprintf(buf,sizeof(buf),"I%u",i);
            return std::string(buf);
        }
        if(w>=kGenBase&&w<kGenBase+6&&!rem) {const u32 f=w-kGenBase;if(f)snprintf(buf,sizeof(buf),"IG+%u",f);else snprintf(buf,sizeof(buf),"IG");return std::string(buf);}
        snprintf(buf,sizeof(buf),"G%u.%u",w,rem);return std::string(buf);
    }
    return v;
}
inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<evs.size();++i) {
        const Ev& e=evs[i];
        json row=json::array({e.kind});
        for(u32 k=0;k<e.n;++k)row.push_back((e.ptrMask>>k)&1u?Tok(e.v[k]):json(e.v[k]));
        out.push_back(row);
    }
    return out;
}

// ---- imagen de un caso
struct CaseImage {
    std::array<u32,kRegionWords> region{};
    u8 use{};
};
inline void BuildResources(Arena& a) {
    for(u32 kind=0;kind<2;++kind)
        for(u32 i=0;i<kResCount;++i) {
            u32* r=a.res[kind][i];
            r[0]=Self(kind?ibVt:texVt);r[1]=i;r[2]=1+i%3;r[3]=0x15+i%7;r[4]=16u<<(i%5);r[5]=r[4];r[6]=2*(100+i);r[7]=0;
        }
    a.dev[0]=Self(devVt);for(u32 i=1;i<kResWords;++i)a.dev[i]=0;
}
inline void* StackPtr(u32 k) {return &W.stacks[k][kStackGuard];}
inline void PlaceBuf(const Buf& b,bool tex,CaseImage& img,Arena& a,u32 n) {
    const u32 base=tex?kTexBase+kTexStride*b.idx:(b.idx<16?kIbBase+kIbStride*b.idx:kGenBase);
    const u32 stk=tex?kStackOfTex(b.idx):kStackOfIb(b.idx);
    u32* r=&img.region[base];
    if(tex){r[0]=b.a;r[1]=b.b;r[2]=b.c;r[3]=b.cap;r[4]=b.num;r[5]=b.size;r[6]=b.hasPtr?Self(&W.stacks[stk][kStackGuard]):0u;}
    else   {r[0]=b.a;r[1]=b.b;r[2]=b.cap;r[3]=b.num;r[4]=b.size;r[5]=b.hasPtr?Self(&W.stacks[stk][kStackGuard]):0u;}
    for(size_t j=0;j<b.items.size()&&j<kStackSlots;++j)a.stacks[stk][kStackGuard+j]=Self(W.res[tex?0:1][b.items[j]%kResCount]);   // el contenido va en WRef, los punteros apuntan a W
    (void)n;
}
// Construye la imagen de referencia del caso: regiones de globales y toda la arena. La direccion de las pilas/recursos es la de W (W y WRef son del mismo tipo; las direcciones
// que se guardan en la region apuntan siempre a W, y WRef solo aporta los CONTENIDOS iniciales).
inline void BuildCase(const Spec& s,unsigned n,CaseImage& img) {
    BuildResources(WRef);
    for(u32 k=0;k<kStacks;++k)for(u32 i=0;i<kStackWords;++i)WRef.stacks[k][i]=Garbage(n+k*64,i);
    for(u32 j=0;j<kNewBlocks;++j)for(u32 i=0;i<kNewBlockWords;++i)WRef.newMem[j][i]=Garbage(n+0x100+j,i);
    for(u32 i=0;i<kOutWords;++i)WRef.out[i]=Garbage(n+0x200,i);
    WRef.out[1]=s.outInit?s.outInit:Garbage(n+0x200,1);
    for(u32 i=0;i<kRegionWords;++i)img.region[i]=Garbage(n+0x300,i);
    img.use=u8(s.use);
    // recurso que se pasa como argumento (127): nivel, formato, ancho y alto de textura / tamano de index buffer
    WRef.res[0][kArgRes][2]=s.res[0];WRef.res[0][kArgRes][3]=s.res[1];WRef.res[0][kArgRes][4]=s.res[2];WRef.res[0][kArgRes][5]=s.res[3];
    WRef.res[1][kArgRes][6]=s.res[4];
    img.region[0]=s.ntex;
    if(s.fn==kInit&&s.prior) {
        img.region[0]=1;img.use=1;
        u32* t=&img.region[kTexBase];t[0]=0;t[1]=0;t[2]=0xFFFFFFFFu;t[3]=0x10;t[4]=s.prior==2?3u:0u;t[5]=s.prior==2?3u:0u;t[6]=0xC1000000u;
        for(u32 k=0;k<16;++k){u32* b=&img.region[kIbBase+kIbStride*k];b[0]=0x65;b[1]=100*(k+1);b[2]=0x10;b[3]=s.prior==2?k%4:0u;b[4]=s.prior==2?k%3:0u;b[5]=0xC1000001u+k;}
        u32* g=&img.region[kGenBase];g[0]=0x65;g[1]=0;g[2]=0x10;g[3]=s.prior==2?2u:0u;g[4]=s.prior==2?1u:0u;g[5]=0xC10000FFu;
    }
    if(s.hasFree){img.region[kFreeTexWord]=s.ftex;img.region[kFreeIbWord]=s.fib;}
    // las pilas de W se usan para calcular los punteros; los contenidos se colocan en WRef
    for(const Buf& b:s.tex)PlaceBuf(b,true,img,WRef,n);
    for(const Buf& b:s.ib)PlaceBuf(b,false,img,WRef,n);
}

// ---- instantaneas del estado real del juego (se restauran antes de cualquier Write)
inline std::array<u32,kRegionWords> snapRegion{};inline u8 snapUse{};inline u32 snapDev{};
inline void TakeSnapshot() {memcpy(snapRegion.data(),reinterpret_cast<void*>(kRegionBase),kRegionWords*4);snapUse=*reinterpret_cast<u8*>(kUseAddr);snapDev=*reinterpret_cast<u32*>(kDeviceAddr);}
inline void RestoreSnapshot() {memcpy(reinterpret_cast<void*>(kRegionBase),snapRegion.data(),kRegionWords*4);*reinterpret_cast<u8*>(kUseAddr)=snapUse;*reinterpret_cast<u32*>(kDeviceAddr)=snapDev;}
inline bool SnapshotMatches() {return memcmp(snapRegion.data(),reinterpret_cast<void*>(kRegionBase),kRegionWords*4)==0&&snapUse==*reinterpret_cast<u8*>(kUseAddr)&&snapDev==*reinterpret_cast<u32*>(kDeviceAddr);}

// ---- cambios observados (respecto a la imagen de referencia del caso), todos en fichas
inline json ChangesJson(const CaseImage& img,const std::array<u32,kRegionWords>& post) {
    json out=json::array();
    for(u32 i=0;i<kRegionWords;++i)if(post[i]!=img.region[i])out.push_back(json::array({"G",i,Tok(post[i])}));
    for(u32 kind=0;kind<2;++kind)
        for(u32 i=0;i<kResCount;++i)for(u32 w=0;w<kResWords;++w)
            if(W.res[kind][i][w]!=WRef.res[kind][i][w])out.push_back(json::array({kind?"B":"R",i,w,Tok(W.res[kind][i][w])}));
    for(u32 k=0;k<kStacks;++k)for(u32 w=0;w<kStackWords;++w)
        if(W.stacks[k][w]!=WRef.stacks[k][w])out.push_back(json::array({"S",k,int(w)-int(kStackGuard),Tok(W.stacks[k][w])}));
    for(u32 j=0;j<kNewBlocks;++j)for(u32 w=0;w<kNewBlockWords;++w)
        if(W.newMem[j][w]!=WRef.newMem[j][w])out.push_back(json::array({"N",j,w,Tok(W.newMem[j][w])}));
    for(u32 w=0;w<kOutWords;++w)if(W.out[w]!=WRef.out[w])out.push_back(json::array({"O",w,Tok(W.out[w])}));
    for(u32 w=0;w<kResWords;++w)if(W.dev[w]!=WRef.dev[w])out.push_back(json::array({"D",w,Tok(W.dev[w])}));
    return out;
}

// ================================================================================================ un caso (una funcion, un par EXE / DLL)
inline void ArmFakes(const Spec& s) {
    F=FakeState{};
    F.tid=GetCurrentThreadId();
    F.popRet=s.popRet;F.devRet=s.devRet;F.devWrite=s.devWrite;F.devRes=s.devRes;F.rszMode=s.rszMode;
    F.reK=s.reK;F.reKind=s.reKind;F.reIdx=s.reIdx;F.reVal=s.reVal;
    for(size_t i=0;i<F.newRet.size();++i)F.newRet[i]=i<s.newRet.size()?s.newRet[i]:1u;
}
inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    CaseImage img;
    BuildCase(s,n,img);
    u32 args[5]{};u32 argCount=0,pop=0;
    switch(fn) {
    case kCreateIb: args[0]=s.args[0];args[1]=s.args[1];args[2]=Self(&W.out[1]);argCount=3;pop=12;break;
    case kCreateTex: args[0]=s.args[0];args[1]=s.argx[0];args[2]=s.argx[1];args[3]=s.argx[2];args[4]=Self(&W.out[1]);argCount=5;pop=20;break;
    case kDestroyIb: args[0]=Self(W.res[1][kArgRes]);argCount=1;pop=4;break;
    case kDestroyTex: args[0]=Self(W.res[0][kArgRes]);argCount=1;pop=4;break;
    default: break;
    }
    Side sides[2];json rows[2];
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        memcpy(&W,&WRef,sizeof(W));
        W.dev[0]=Self(devVt);
        ArmFakes(s);nev=0;
        TakeSnapshot();
        memcpy(reinterpret_cast<void*>(kRegionBase),img.region.data(),kRegionWords*4);
        *reinterpret_cast<u8*>(kUseAddr)=img.use;
        *reinterpret_cast<u32*>(kDeviceAddr)=Self(W.dev);
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t entry){
            F.armed=true;
            const Reg r=Gate(entry,args,argCount,s.eax,s.ecx,s.edx,s.stack,pop);
            F.armed=false;
            return r;
        });
        F.armed=false;
        std::array<u32,kRegionWords> post{};
        memcpy(post.data(),reinterpret_cast<void*>(kRegionBase),kRegionWords*4);
        const u8 useAfter=*reinterpret_cast<u8*>(kUseAddr);
        const u32 devAfter=*reinterpret_cast<u32*>(kDeviceAddr);
        RestoreSnapshot();
        const bool restored=SnapshotMatches();
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;
        obs["changes"]=ChangesJson(img,post);
        obs["use"]=useAfter;
        obs["device_unchanged"]=devAfter==Self(W.dev);
        if(fn==kCreateIb||fn==kCreateTex)obs["eax"]=sides[sd].reg.eax;
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
    TakeSnapshot();
    const auto initialRegion=snapRegion;const u8 initialUse=snapUse;const u32 initialDev=snapDev;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena",Self(&W)},{"arena_bytes",sizeof(W)},
        {"initial_use",initialUse},{"initial_device",initialDev}});
    Totals totals;
    if(ready) {
        F.tid=GetCurrentThreadId();
        FillTraps(texVt,std::make_integer_sequence<int,32>{});
        FillTraps(ibVt,std::make_integer_sequence<int,32>{});
        FillTraps(devVt,std::make_integer_sequence<int,128>{});
        texVt[2]=reinterpret_cast<void*>(&FkRelease);texVt[13]=reinterpret_cast<void*>(&FkGetLevelCount);texVt[17]=reinterpret_cast<void*>(&FkGetLevelDesc);
        ibVt[2]=reinterpret_cast<void*>(&FkRelease);ibVt[13]=reinterpret_cast<void*>(&FkGetDesc);
        devVt[23]=reinterpret_cast<void*>(&FkDevCreateTexture);devVt[27]=reinterpret_cast<void*>(&FkDevCreateIb);
        for(u32 fn=0;fn<kFns;++fn) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)RunCase(env,totals,fn,c);
            Write({{"event","function_summary"},{"function",kFn[fn]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool regionNow=memcmp(initialRegion.data(),reinterpret_cast<void*>(kRegionBase),kRegionWords*4)==0&&initialUse==*reinterpret_cast<u8*>(kUseAddr)&&initialDev==*reinterpret_cast<u32*>(kDeviceAddr);
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"d3d_state_restored",regionNow}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"d3d_state_restored",regionNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
