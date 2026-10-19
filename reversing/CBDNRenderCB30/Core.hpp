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

// Nucleo del banco CBDNRenderCB30. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/FileLoaderLoad2dEffect27 (Core.hpp).
//
// Banco par a par de UNA funcion: CCustomBuildingDNPipeline::CustomPipeRenderCB (0x5D6480, cdecl, 4 argumentos (entry, object, type, flags), `ret` normal, void).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. En ambos lados se sustituyen, SOLO durante la llamada y con el MISMO falso, TODOS los
// colaboradores de RenderWare/D3D (parches de entrada en la direccion del EXE y, si la DLL tiene cuerpo propio, tambien en el cuerpo de la DLL, calculado ANTES de parchear):
//   0x756D90 _rwD3D9EnableClippingIfNeeded (la DLL tiene cuerpo en rpworld.cpp) | 0x7FC320 RwD3D9GetRenderState | 0x7FDE70 RwD3D9SetTexture | 0x7FC2D0 RwD3D9SetRenderState
//   0x7FC340 RwD3D9SetTextureStageState | 0x7FA1C0 SetIndices | 0x7FA090 _rwD3D9SetStreams | 0x7F9F70 SetVertexDeclaration | 0x7FE0A0 alfa por vertice | 0x7FC4D0 SetSurfaceProperties
//   0x5DA640 / 0x5DA6A0 funciones de dibujo de la malla | y fpRenderStateSet (la tabla del motor [[0xC97B24] + 0x20] se cambia durante la llamada y se restaura).
// NO se sustituye: el _ftol2 del EXE (0x821B40; la DLL convierte con su propio codigo: debe coincidir bit a bit). La matriz de textura global 0xC02C28 (64 bytes) se rellena con una
// imagen previa por caso, se compara y se restaura. El desplazamiento de plugin 0x8D12C4 se lee del juego en cada caso. EAX/ECX/EDX de salida: ver README.md.
namespace CBDNRenderCB30Live {
using namespace CBDNRenderFix;
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
// La funcion usa x87 (cartel: _ftol2 del EXE y conversion float->int64 de la DLL; sscanf de ambos CRT): los registros ST vacios guardan restos distintos, la palabra de estado lleva
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


struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};

// ------------------------------------------------------------------------------------------------ memoria de los casos
alignas(16) inline std::array<u8,kResArena> resArena{};
alignas(16) inline std::array<std::array<u8,kMatArena>,kNumMats> matArenas{};
alignas(16) inline std::array<std::array<u8,kEnvArena>,kNumEnvs> envArenas{};
alignas(16) inline std::array<u8,kObjArena> objArena{};
inline constexpr u32 kObjRaw=0x0BADF00Du;                                        // valor crudo del objeto con obj_kind == 2 (igual que en fixture_oracle.py)
inline u32 PtrOfAddr(const void* p){return u32(uintptr_t(p));}
inline u8* Res(){return resArena.data()+kResAt;}
inline u8* Mat(u32 i){return matArenas[i].data()+kMatAt;}
inline u8* EnvBlk(u32 j){return envArenas[j].data()+kEnvAt;}
inline u8* Obj(){return objArena.data()+kObjAt;}
inline u32 Dw(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
inline void SetDw(u8* p,u32 v){std::memcpy(p,&v,4);}
inline u32 curLighting{};
inline u32 matrixAddr=0xC02C28;
inline u32 headerAddr(){return PtrOfAddr(Res()+kEntryHdr);}

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
struct Ev {u32 kind,a,b,c,d;};
inline std::array<Ev,192> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c,d};++nev;}}
inline u32 MeshIndex(const void* mesh){return (PtrOfAddr(mesh)-(PtrOfAddr(Res())+kEntryHdr+kHeaderSize))/kMeshSize;}
inline u32 ObjLabel(const void* p){const u32 v=PtrOfAddr(p),base=PtrOfAddr(Obj());return v>=base&&v<base+kObjSize?v-base:v;}

// Falsos (todos cdecl).
inline void __cdecl FakeClip(void* object,u32 type){Push(1,ObjLabel(object),type);}
inline void __cdecl FakeGetRS(u32 state,u32* value){Push(2,state);*value=curLighting;}
inline int __cdecl FakeSetTex(void* tex,u32 stage){Push(3,PtrOfAddr(tex),stage);return 1;}
inline void __cdecl FakeSetRS(u32 state,u32 value){Push(4,state,value);}
inline void __cdecl FakeTSS(u32 stage,u32 type,u32 value){Push(5,stage,type,value);}
inline void __cdecl FakeSetIdx(void* ib){Push(6,PtrOfAddr(ib));}
inline void __cdecl FakeStreams(void* streams,u32 useOffsets){Push(7,PtrOfAddr(streams)-headerAddr(),useOffsets);}
inline void __cdecl FakeDecl(void* d){Push(8,PtrOfAddr(d));}
inline int __cdecl FakeFpRS(u32 state,void* value){Push(9,state,PtrOfAddr(value));return 1;}
inline void __cdecl FakeAlpha(u32 enable){Push(10,enable);}
inline int __cdecl FakeSurf(const void* surf,const void* color,u32 flags){
    u32 mi=0xFFFFFFFFu,so=0,co=0;
    for(u32 i=0;i<kNumMats;++i){const u32 b=PtrOfAddr(Mat(i));if(PtrOfAddr(surf)>=b&&PtrOfAddr(surf)<b+kMatWin){mi=i;so=PtrOfAddr(surf)-b;co=PtrOfAddr(color)-b;}}
    Push(11,mi,so|(co<<16),flags);return 1;
}
inline void __cdecl FakeRender(void* header,void* mesh){Push(12,PtrOfAddr(header)-PtrOfAddr(Res()),MeshIndex(mesh));}
inline void __cdecl FakeRenderLit(void* header,void* mesh,u32 flags,void* tex){Push(13,PtrOfAddr(header)-PtrOfAddr(Res()),MeshIndex(mesh),flags,PtrOfAddr(tex));}

struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x5D6480 y todas las entradas parcheadas / ejecutadas de verdad: deben seguir siendo el codigo original al volver cada llamada
    Pair pClip{};                        // _rwD3D9EnableClippingIfNeeded: EXE 0x756D90 + cuerpo de la DLL (resuelto ANTES de escribir ningun parche)
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// (direccion del EXE, falso) de los colaboradores que solo existen como direccion del EXE; el falso de _rwD3D9EnableClippingIfNeeded se instala ademas en el cuerpo de la DLL.
struct ExeFake {uintptr_t address;void* stub;};
inline const std::array<ExeFake,12>& ExeFakes(){
    static const std::array<ExeFake,12> t{{
        {0x756D90,reinterpret_cast<void*>(&FakeClip)},{0x7FC320,reinterpret_cast<void*>(&FakeGetRS)},{0x7FDE70,reinterpret_cast<void*>(&FakeSetTex)},{0x7FC2D0,reinterpret_cast<void*>(&FakeSetRS)},
        {0x7FC340,reinterpret_cast<void*>(&FakeTSS)},{0x7FA1C0,reinterpret_cast<void*>(&FakeSetIdx)},{0x7FA090,reinterpret_cast<void*>(&FakeStreams)},{0x7F9F70,reinterpret_cast<void*>(&FakeDecl)},
        {0x7FE0A0,reinterpret_cast<void*>(&FakeAlpha)},{0x7FC4D0,reinterpret_cast<void*>(&FakeSurf)},{0x5DA640,reinterpret_cast<void*>(&FakeRender)},{0x5DA6A0,reinterpret_cast<void*>(&FakeRenderLit)}}};
    return t;
}
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,16> p;int n{};
    void AddAt(uintptr_t at,void* stub){if(at)p[n++].emplace(at,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){
    for(const auto& f:ExeFakes())p.AddAt(f.address,f.stub);
    if(e.pClip.dll&&e.pClip.dll!=e.pClip.exe)p.AddAt(e.pClip.dll,reinterpret_cast<void*>(&FakeClip));
}
// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). `exeOnly`: direcciones del EXE que no pueden saltar a la DLL
// (sscanf, _ftol2, strncpy del EXE: se ejecutan de verdad). Los parches de los colaboradores se instalan despues de comprobar la ruta y se quitan antes de volver.
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
inline u32 SlotPtr(u32 label){return label==kEnvSlotNull?0u:(label>=kEnvSlotBase&&label<kEnvSlotBase+kNumEnvs?PtrOfAddr(EnvBlk(label-kEnvSlotBase)):label);}
inline void BuildArenas(const Fx& f,i32 envOff){
    resArena.fill(u8(kGuardByte));objArena.fill(u8(kGuardByte));
    for(auto& a:matArenas)a.fill(u8(kGuardByte));
    for(auto& a:envArenas)a.fill(u8(kGuardByte));
    std::memcpy(Res(),f.res.data(),kResSize);
    for(u32 m=0;m<kMaxMeshes;++m)SetDw(Res()+kEntryHdr+kHeaderSize+m*kMeshSize+8,PtrOfAddr(Mat(f.meshMat[m])));
    for(u32 i=0;i<kNumMats;++i){const auto img=MatImage(f,i,u32(envOff));std::memcpy(Mat(i),img.data(),kMatWin);SetDw(Mat(i)+envOff,SlotPtr(f.matSlot[i]));}
    for(u32 j=0;j<kNumEnvs;++j)std::memcpy(EnvBlk(j),f.env[j].data(),kEnvSize);
    std::memset(Obj(),0x3C,kObjSize);
    std::memcpy(reinterpret_cast<void*>(uintptr_t(0xC02C28)),f.mtx.data(),kMtxSize);
}
inline void CaseRun(Env& env, Totals& t, unsigned n) {
    const Fx f = Make(n);
    const i32 envOff = *reinterpret_cast<const i32*>(uintptr_t(0x8D12C4));
    const bool fits = envOff >= 0x14 && envOff % 4 == 0 && envOff + 4 <= i32(kMatWin) && (f.numMeshes <= kMaxMeshes || f.numMeshes >= 0x80000000u);
    const json input = json::parse(InputJson(f, u32(envOff)));
    u8* const matrix = reinterpret_cast<u8*>(uintptr_t(0xC02C28));
    const auto savedMatrix = ReversingProbe::Bytes(matrix, kMtxSize);
    void** const fpRS = reinterpret_cast<void**>(*reinterpret_cast<u8**>(uintptr_t(0xC97B24)) + 0x20);
    void* const savedFpRS = *fpRS;
    std::array<u8,kResArena> resRef{};
    std::array<std::array<u8,kMatArena>,kNumMats> matRef{};
    std::array<std::array<u8,kEnvArena>,kNumEnvs> envRef{};
    std::array<u8,kObjArena> objRef{};
    if (fits) { BuildArenas(f, envOff); resRef = resArena; matRef = matArenas; envRef = envArenas; objRef = objArena; }
    Side sides[2];
    json rows[2];
    bool fpRsRestored = true;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        if (fits) BuildArenas(f, envOff);
        nev = 0; curLighting = f.lighting;
        if (fits) {
            sides[s] = Execute(env, *env.route, 0x5D6480, {uintptr_t(0x821B40)}, candidate, [&](uintptr_t entry) {
                const u32 objectArg = f.objKind == 0 ? PtrOfAddr(Obj() + kObjProbeOffset) : (f.objKind == 1 ? 0u : kObjRaw);
                const u32 args[4]{PtrOfAddr(Res()), objectArg, f.typeArg, f.flags};
                *fpRS = reinterpret_cast<void*>(&FakeFpRS);                          // RwRenderStateSet va por la tabla del motor: se sustituye solo durante la llamada
                const Reg r = Gate(entry, args, 4, f.eax, f.ecx, f.edx, f.stack, 16);   // cdecl: el llamador retira los cuatro argumentos
                *fpRS = savedFpRS;
                return r;
            });
        } else {   // fixture inseguro: no se llama; queda registrado y falla
            sides[s].reg = {0, 0, 0, 0, true, true};
            SaveFP(sides[s].fp.data()); sides[s].mxcsr = ReadMXCSR();
        }
        fpRsRestored &= *fpRS == savedFpRS;
        json trace = json::array();
        for (unsigned i = 0; i < nev; ++i) trace.push_back(json::array({evs[i].kind, evs[i].a, evs[i].b, evs[i].c, evs[i].d}));
        bool matsOk = true, envsOk = true;
        for (u32 i = 0; i < kNumMats; ++i) matsOk &= matArenas[i] == matRef[i];
        for (u32 j = 0; j < kNumEnvs; ++j) envsOk &= envArenas[j] == envRef[j];
        json observed = {{"trace", trace}, {"matrix", HexBytes(matrix, kMtxSize)}, {"res_ok", fits && resArena == resRef}, {"mats_ok", fits && matsOk}, {"envs_ok", fits && envsOk}, {"obj_ok", fits && objArena == objRef}};
        if (kCompareVolatileRegisters) { observed["eax"] = sides[s].reg.eax; observed["ecx"] = sides[s].reg.ecx; observed["edx"] = sides[s].reg.edx; }
        auto fp = NormalizeFp(sides[s].fp);
        const bool watchesIntact = env.WatchesIntact();
        json out = CommonRow(sides[s], fp);
        out["observed"] = observed;
        if (!fits) out["route_ok"] = false;
        RestoreFloatingState();
        std::memcpy(matrix, savedMatrix.data(), kMtxSize);                           // matriz global restaurada despues de cada lado
        out["globals_restored"] = watchesIntact && env.WatchesIntact() && fpRsRestored && ReversingProbe::Bytes(matrix, kMtxSize) == savedMatrix;
        out["floating_state_restored"] = FloatingStateRestored();
        rows[s] = out;
    }
    Finish(env, t, "CCustomBuildingDNPipeline::CustomPipeRenderCB", n, input, rows, sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env, bool ready, const json& registered, const json& bodies) {
    const uintptr_t body = env.route ? env.route->Body() : 0;
    ready &= body && ModuleOf(body) == env.asi;
    ready &= env.pClip.dll && ModuleOf(env.pClip.dll) == env.asi;
    env.watches.emplace_back(0x5D6480, 16);
    env.watches.emplace_back(0x821B40, 16);
    for (const auto& f : ExeFakes()) env.watches.emplace_back(f.address, 16);
    json watched = json::array();
    for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), 16)}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const i32 envOff = *reinterpret_cast<const i32*>(uintptr_t(0x8D12C4));
    json stubbed = json::array();
    for (const auto& f : ExeFakes()) stubbed.push_back({{"exe", f.address}, {"dll", f.address == 0x756D90 ? env.pClip.dll : uintptr_t(0)}});
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", 1}, {"pairs", kCases}, {"cases", {{"CCustomBuildingDNPipeline::CustomPipeRenderCB", kCases}}},
           {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
           {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
           {"collaborators", {{"exe_real", std::vector<uintptr_t>{0x821B40}}, {"stubbed", stubbed}, {"engine_table", "[[0xC97B24]+0x20] fpRenderStateSet"}}}, {"plugin_offsets", {{"env", envOff}}},
           {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game}, {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, c);
        Write({{"event", "function_summary"}, {"function", "CCustomBuildingDNPipeline::CustomPipeRenderCB"}, {"cases", kCases}, {"failures", totals.failures}, {"state_restored", env.WatchesIntact()}});
    }
    const bool entriesIntact = env.WatchesIntact();
    bool stateRestored = entriesIntact;
    if (env.route) stateRestored &= env.route->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}
}
