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

// Nucleo del banco CBDNMaterialSetup30. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/FileLoaderLoad2dEffect27 (Core.hpp).
//
// Banco par a par de UNA funcion: CCustomBuildingDNPipeline::CustomPipeMaterialSetup (0x5D7120, cdecl, 2 argumentos (material, data), `ret` normal).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. En ambos lados se sustituye, SOLO durante la llamada y con el MISMO falso, el unico
// colaborador que escribe estado: CCustomBuildingDNPipeline::SetFxEnvTexture (0x5D9570; parche de entrada en la direccion del EXE Y en el cuerpo de la DLL, calculado ANTES de parchear;
// el hook de SetFxEnvTexture esta encendido por defecto, asi que 0x5D9570 salta a la DLL y el parche lo tapa mientras dura la llamada). Siete modos (ver fixture_oracle.py).
// NO se sustituye (se ejecuta de verdad en cada lado): RpMatFXMaterialGetEffects (0x812140), lectura pura de la memoria del material.
// Los desplazamientos de plugin (0x8D12C4 entorno, 0xC9AB74 MatFX) se leen del juego en cada caso. No se escribe ninguna global del juego. EAX/ECX/EDX de salida: ver README.md.
namespace CBDNMaterialSetup30Live {
using namespace CBDNMatFix;
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
alignas(16) inline std::array<u8,kMatArena> matArena{};
alignas(16) inline std::array<u8,kEnvArena> envAArena{}, envBArena{};
alignas(16) inline std::array<u8,kTexArena> tex0Arena{}, tex1Arena{};
alignas(16) inline std::array<u8,kMfxArena> mfxArena{};
inline u32 PtrOfAddr(const void* p){return u32(uintptr_t(p));}
inline u8* MatBase(){return matArena.data()+kMatAt;}
inline u8* EnvA(){return envAArena.data()+kEnvAt;}
inline u8* EnvB(){return envBArena.data()+kEnvAt;}
inline u8* Tex0(){return tex0Arena.data()+kTexAt;}
inline u8* Tex1(){return tex1Arena.data()+kTexAt;}
inline u8* Mfx(){return mfxArena.data()+kMfxAt;}
// etiqueta -> puntero (los valores que no son etiqueta se tratan como punteros basura y se conservan tal cual)
inline u32 PtrOfLabel(u32 l){
    switch(l){case kLblNull:return 0;case kLblEnvA:return PtrOfAddr(EnvA());case kLblEnvB:return PtrOfAddr(EnvB());case kLblTex0:return PtrOfAddr(Tex0());
              case kLblTex1:return PtrOfAddr(Tex1());case kLblMfx:return PtrOfAddr(Mfx());default:return l;}
}
inline u32 LabelOfPtr(u32 p){
    if(p==0)return kLblNull;
    if(p==PtrOfAddr(EnvA()))return kLblEnvA;
    if(p==PtrOfAddr(EnvB()))return kLblEnvB;
    if(p==PtrOfAddr(Tex0()))return kLblTex0;
    if(p==PtrOfAddr(Tex1()))return kLblTex1;
    if(p==PtrOfAddr(Mfx()))return kLblMfx;
    return p;
}
inline u32 Dw(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
inline void SetDw(u8* p,u32 v){std::memcpy(p,&v,4);}

// ---- traza sin reservas (el falso no reserva memoria ni escribe JSON)
struct Ev {u32 kind,a,b,c;};
inline std::array<Ev,16> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c};++nev;}}
inline u32 curMode{},curFlagsWord{};

// Falso de SetFxEnvTexture(CustomEnvMapPipeMaterialData** slot), cdecl: devuelve el puntero de entorno o nulo (modos: ver fixture_oracle.py).
inline void* __cdecl FakeSetFx(void** slot){
    Push(1,u32(reinterpret_cast<u8*>(slot)-MatBase()),LabelOfPtr(PtrOfAddr(*slot)));
    switch(curMode){
    case 0:return *slot;
    case 1:*slot=EnvA();return EnvA();
    case 2:*slot=nullptr;return nullptr;
    case 3:*slot=EnvB();return EnvB();
    case 4:return EnvA();
    case 5:*slot=EnvA();return nullptr;
    default:SetDw(MatBase()+0x10,curFlagsWord);return *slot;
    }
}

struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x5D7120, 0x5D9570 y 0x812140: deben seguir siendo el codigo original al volver cada llamada
    Pair pSetFx{};                       // cuerpo de la DLL resuelto ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,8> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){p.Add(e.pSetFx,&FakeSetFx);}
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
inline bool GuardOk(const u8* arena,size_t arenaSize,size_t at,size_t size){
    for(size_t i=0;i<arenaSize;++i){if(i>=at&&i<at+size)continue;if(arena[i]!=u8(kGuardByte))return false;}
    return true;
}
inline void BuildArenas(const Fx& f,i32 envOff,i32 mfxOff){
    matArena.fill(u8(kGuardByte));envAArena.fill(u8(kGuardByte));envBArena.fill(u8(kGuardByte));tex0Arena.fill(u8(kGuardByte));tex1Arena.fill(u8(kGuardByte));mfxArena.fill(u8(kGuardByte));
    const auto m=MatImage(f,u32(envOff),u32(mfxOff));
    std::memcpy(MatBase(),m.data(),kMatWin);
    SetDw(MatBase()+0x10,f.preFlags);
    SetDw(MatBase()+envOff,PtrOfLabel(f.envPre));
    SetDw(MatBase()+mfxOff,f.mfxPresent?PtrOfAddr(Mfx()):0u);
    std::memcpy(EnvA(),f.envA.data(),kEnvSize);SetDw(EnvA()+8,PtrOfLabel(f.envATex));
    std::memcpy(EnvB(),f.envB.data(),kEnvSize);SetDw(EnvB()+8,PtrOfLabel(f.envBTex));
    std::memcpy(Tex0(),f.tex0.data(),kTexSize);std::memcpy(Tex1(),f.tex1.data(),kTexSize);
    std::memcpy(Mfx(),f.mfx.data(),kMfxSize);SetDw(Mfx()+4,PtrOfLabel(f.mfxTex));SetDw(Mfx()+0x30,f.effects);
}
inline void CaseRun(Env& env, Totals& t, unsigned n) {
    const Fx f = Make(n);
    const i32 envOff = *reinterpret_cast<const i32*>(uintptr_t(0x8D12C4)), mfxOff = *reinterpret_cast<const i32*>(uintptr_t(0xC9AB74));
    const bool fits = envOff >= 0x14 && mfxOff >= 0x14 && envOff % 4 == 0 && mfxOff % 4 == 0 && envOff + 4 <= i32(kMatWin) && mfxOff + 4 <= i32(kMatWin) && std::abs(envOff - mfxOff) >= 4;
    const json input = json::parse(InputJson(f, u32(envOff), u32(mfxOff)));
    Side sides[2];
    json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        if (fits) BuildArenas(f, envOff, mfxOff);
        nev = 0; curMode = f.mode; curFlagsWord = f.flagsWord;
        if (fits) {
            sides[s] = Execute(env, *env.route, 0x5D7120, {uintptr_t(0x812140)}, candidate, [&](uintptr_t entry) {
                const u32 args[2]{PtrOfAddr(MatBase()), f.data};
                return Gate(entry, args, 2, f.eax, f.ecx, f.edx, f.stack, 8);      // cdecl: el llamador retira los dos argumentos
            });
        } else {   // fixture inseguro (desplazamientos de plugin inesperados): no se llama; queda registrado y falla
            sides[s].reg = {0, 0, 0, 0, true, true};
            SaveFP(sides[s].fp.data()); sides[s].mxcsr = ReadMXCSR();
        }
        auto envDump = [&](u8* base) { std::array<u8,kEnvSize> d{}; std::memcpy(d.data(), base, kEnvSize); SetDw(d.data() + 8, 0); return HexBytes(d.data(), d.size()); };
        std::array<u8,kMatWin> mw{};
        std::memcpy(mw.data(), MatBase(), kMatWin);
        const u32 flagsNow = Dw(MatBase() + 0x10);
        const u32 envSlot = fits ? LabelOfPtr(Dw(MatBase() + envOff)) : 0xFFFFFFFFu, mfxSlot = fits ? LabelOfPtr(Dw(MatBase() + mfxOff)) : 0xFFFFFFFFu;
        if (fits) { SetDw(mw.data() + 0x10, 0); SetDw(mw.data() + envOff, 0); SetDw(mw.data() + mfxOff, 0); }
        std::array<u8,kMfxSize> mf{};
        std::memcpy(mf.data(), Mfx(), kMfxSize);
        SetDw(mf.data() + 4, 0); SetDw(mf.data() + 0x30, 0);
        json trace = json::array();
        for (unsigned i = 0; i < nev; ++i) trace.push_back(json::array({evs[i].kind, evs[i].a, evs[i].b, evs[i].c}));
        json observed = {{"ret_ok", sides[s].reg.eax == PtrOfAddr(MatBase())}, {"flags", flagsNow}, {"env_slot", envSlot}, {"mfx_slot", mfxSlot},
                         {"mat", HexBytes(mw.data(), mw.size())}, {"mat_guard_ok", GuardOk(matArena.data(), matArena.size(), kMatAt, kMatWin)},
                         {"env_a", envDump(EnvA())}, {"env_a_tex", LabelOfPtr(Dw(EnvA() + 8))}, {"env_a_guard_ok", GuardOk(envAArena.data(), envAArena.size(), kEnvAt, kEnvSize)},
                         {"env_b", envDump(EnvB())}, {"env_b_tex", LabelOfPtr(Dw(EnvB() + 8))}, {"env_b_guard_ok", GuardOk(envBArena.data(), envBArena.size(), kEnvAt, kEnvSize)},
                         {"tex0", HexBytes(Tex0(), kTexSize)}, {"tex0_guard_ok", GuardOk(tex0Arena.data(), tex0Arena.size(), kTexAt, kTexSize)},
                         {"tex1", HexBytes(Tex1(), kTexSize)}, {"tex1_guard_ok", GuardOk(tex1Arena.data(), tex1Arena.size(), kTexAt, kTexSize)},
                         {"mfx", HexBytes(mf.data(), mf.size())}, {"mfx_tex", LabelOfPtr(Dw(Mfx() + 4))}, {"mfx_eff", Dw(Mfx() + 0x30)},
                         {"mfx_guard_ok", GuardOk(mfxArena.data(), mfxArena.size(), kMfxAt, kMfxSize)}, {"trace", trace}};
        if (kCompareVolatileRegisters) { observed["eax"] = sides[s].reg.eax; observed["ecx"] = sides[s].reg.ecx; observed["edx"] = sides[s].reg.edx; }
        auto fp = NormalizeFp(sides[s].fp);
        const bool watchesIntact = env.WatchesIntact();
        json out = CommonRow(sides[s], fp);
        out["observed"] = observed;
        if (!fits) out["route_ok"] = false;
        RestoreFloatingState();
        out["globals_restored"] = watchesIntact && env.WatchesIntact();
        out["floating_state_restored"] = FloatingStateRestored();
        rows[s] = out;
    }
    Finish(env, t, "CCustomBuildingDNPipeline::CustomPipeMaterialSetup", n, input, rows, sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env, bool ready, const json& registered, const json& bodies) {
    const uintptr_t body = env.route ? env.route->Body() : 0;
    ready &= body && ModuleOf(body) == env.asi;
    ready &= env.pSetFx.dll && ModuleOf(env.pSetFx.dll) == env.asi;
    for (const uintptr_t at : {uintptr_t(0x5D7120), uintptr_t(0x5D9570), uintptr_t(0x812140)})
        env.watches.emplace_back(at, 16);
    json watched = json::array();
    for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), 16)}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const i32 envOff = *reinterpret_cast<const i32*>(uintptr_t(0x8D12C4)), mfxOff = *reinterpret_cast<const i32*>(uintptr_t(0xC9AB74));
    json stubbed = json::array();
    stubbed.push_back({{"exe", env.pSetFx.exe}, {"dll", env.pSetFx.dll}});
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", 1}, {"pairs", kCases}, {"cases", {{"CCustomBuildingDNPipeline::CustomPipeMaterialSetup", kCases}}},
           {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
           {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
           {"collaborators", {{"exe_real", std::vector<uintptr_t>{0x812140}}, {"stubbed", stubbed}}}, {"plugin_offsets", {{"env", envOff}, {"mfx", mfxOff}}},
           {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game}, {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, c);
        Write({{"event", "function_summary"}, {"function", "CCustomBuildingDNPipeline::CustomPipeMaterialSetup"}, {"cases", kCases}, {"failures", totals.failures}, {"state_restored", env.WatchesIntact()}});
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
