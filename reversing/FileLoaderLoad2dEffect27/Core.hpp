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

// Nucleo del banco FileLoaderLoad2dEffect27. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/DecisionAdd27 (Core.hpp) y reversing/DecisionMakeDecision27.
//
// Banco par a par de UNA funcion: CFileLoader::Load2dEffect (0x5B7670, cdecl, 1 argumento `const char* line`, `ret` normal; el EXE termina con `jmp 0x7316B0`).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. En ambos lados se sustituyen, SOLO durante la llamada y con el MISMO falso,
// los colaboradores que tocan estado del juego (se instalan parches de entrada en la direccion del EXE Y en el cuerpo de la DLL, calculado ANTES de parchear):
//   CModelInfo::Get2dEffectStore (0x4C5A60)  -> devuelve la tienda falsa (arena propia: [u32 contador][16 bloques de 0x40])
//   CBaseModelInfo::Add2dEffect  (0x4C4D20)  -> traza (this, puntero al bloque, contador)       (this = model info falso de ms_modelInfoPtrs[modelId] o nullptr)
//   CTxdStore::PushCurrentTxd (0x7316A0), FindTxdSlot(const char*) (0x731850), SetCurrentTxd (0x7319C0), PopCurrentTxd (0x7316B0) -> traza (el par Push/Set/Pop y el nombre "particle")
//   RwTextureRead (0x7F3AC0)                 -> falso que registra nombres y devuelve punteros distintos y deterministas
//   CMemoryMgr::Malloc (0x72F420)            -> devuelve un bloque controlado del banco
// NO se sustituyen (se ejecutan de verdad en cada lado): sscanf (EXE: 0x8220AD, CRT estatico del juego; DLL: sscanf del CRT estatico /MTd de la DLL), _ftol2 (0x821B40, solo EXE;
// la DLL convierte con su propio CRT), strncpy (0x821F40, solo EXE). Las diferencias entre CRT no se ocultan: aparecen como diferencias de pares (ver README.md).
// La palabra de control x87 se fija a 24 bits, al mas cercano y con TODAS las excepciones enmascaradas (estado del juego): el sscanf del EXE y el _ftol2 corren con ella.
// ms_modelInfoPtrs[modelId] (0xA9B0C8) se escribe por caso y se restaura; no se toca ninguna otra global del juego. EAX/ECX/EDX de salida: solo se registran.
namespace FileLoader2dFx27Live {
using namespace FileLoader2dFxFix;
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


// ------------------------------------------------------------------------------------------------ memoria de los casos
alignas(16) inline std::array<u8,kLineArena> lineArena{};                       // linea + NUL + cola + NUL + ceros
alignas(16) inline std::array<u8,kStoreArena> storeArena{};                     // tienda falsa: guarda | u32 contador + 16 bloques de 0x40 | guarda
alignas(16) inline std::array<u8,kBlockArena> blockArena{};                      // bloque de Malloc(0x40) con guardas
alignas(16) inline std::array<std::array<u8,0x40>,kNumMiLabels> miObjects{};     // model infos falsos: nunca se desreferencian (Add2dEffect esta sustituido)
inline constexpr u32 kBlockLabel=0xB10C0000u;                                    // etiqueta del puntero del bloque Malloc en +0x28
inline constexpr u8 kBlockPre=0xB7;

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
struct Ev {u32 kind,a,b,c;};
inline std::array<Ev,64> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c};++nev;}}
inline std::array<char,64> findName{};
inline std::array<std::array<char,256>,4> texName{};
inline std::array<u8,4> texMaskNull{};
inline unsigned texCount{};
inline i32 findResult{};
inline void CopyName(char* dst,size_t cap,const char* src){size_t i=0;if(src)for(;src[i]&&i+1<cap;++i)dst[i]=src[i];dst[i]=0;}

// Falsos (cdecl salvo Add2dEffect, que es thiscall: ECX = this, un argumento en pila, `ret 4`; __fastcall con tres parametros lo reproduce).
inline void* __cdecl FakeStore(){Push(4);return storeArena.data()+kStoreAt;}
inline void __cdecl FakePush(){Push(1);}
inline i32 __cdecl FakeFind(const char* name){Push(2,Fnv32(name),u32(std::strlen(name)));CopyName(findName.data(),findName.size(),name);return findResult;}
inline void __cdecl FakeSet(i32 slot){Push(3,u32(slot));}
inline void __cdecl FakePop(){Push(8);}
inline void* __cdecl FakeTex(const char* name,const char* mask){
    const unsigned k=texCount++;
    Push(6,k,Fnv32(name),mask==nullptr?1u:0u);
    if(k<texName.size()){CopyName(texName[k].data(),texName[k].size(),name);texMaskNull[k]=mask==nullptr?1:0;}
    return reinterpret_cast<void*>(uintptr_t(0x4E000000u+((k+1)<<16)+(Fnv32(name)&0xFFFFu)));
}
inline void* __cdecl FakeMalloc(u32 size,u32 hint){Push(7,size,hint);return blockArena.data()+kBlockAt;}
inline void __fastcall FakeAdd2d(void* self,void*,void* effect){
    const u8* base=miObjects.data()->data();
    const u8* s=static_cast<const u8*>(self);
    u32 label=0xFFFF;
    if(s>=base&&s<base+sizeof(miObjects))label=u32((s-base)/0x40);
    const u8* st=storeArena.data()+kStoreAt;
    u32 count{};std::memcpy(&count,st,4);
    Push(5,label,u32(static_cast<const u8*>(effect)-st),count);
}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x5B7670, 0x8220AD, 0x821B40, 0x821F40 y las ocho entradas parcheadas: deben seguir siendo el codigo original al volver
    Pair pStore{},pAdd2d{},pPush{},pFind{},pSet{},pPop{},pTex{},pMalloc{};   // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,24> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){
    p.Add(e.pStore,&FakeStore);p.Add(e.pAdd2d,&FakeAdd2d);p.Add(e.pPush,&FakePush);p.Add(e.pFind,&FakeFind);p.Add(e.pSet,&FakeSet);p.Add(e.pPop,&FakePop);
    p.Add(e.pTex,&FakeTex);p.Add(e.pMalloc,&FakeMalloc);
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


// ------------------------------------------------------------------------------------------------ sondeo del CRT (informativo; no es una compuerta)
// Ejecuta el sscanf del EXE (0x8220AD) y el de la DLL sobre una bateria de textos con %d, %f y %s y registra ambos resultados en el evento `begin`: evidencia directa de las
// diferencias entre CRT, independiente de Load2dEffect. Las diferencias reales entre el EXE y la reconstruccion aparecen ademas como pares distintos.
using ScanFn = int(__cdecl*)(const char*, const char*, ...);
inline json CrtProbe() {
    static const char* const inputs[] = {
        "0", "7", "-7", "+7", "007", "2147483647", "2147483648", "2147483649", "4294967295", "4294967296", "99999999999", "18446744073709551615", "18446744073709551616", "-2147483648", "-2147483649",
        "0x1F", "1e3", "3.9", "abc", "", "-", "+", "1,5", "1.5", "-.5", "5.", "+2.5", "1E-3", "1e", "1e+", "1e39", "-1e39", "1e400", "1e-50", "1e-400", "inf", "-inf", "infinity", "nan", "NaN(1)", "0x1p3", "0x10",
        "1#INF", "1.#QNAN", "16777217", "16777217.00000000000000000001", "1.000000059604644775390625", "1.00000005960464477539062500000000000000000000000001",
        "1.00000005960464477539062499999999999999999999999999", "0.1", "0.30000002682209014892578125", "3.4028235e38", "3.4028236e38", "1.17549435e-38", "1e-45", "1.4e-45", "7e-46",
        "255.99999", "255.999993", "255.9999999999", "65536.0039062500000000000000000000000001", "100.00000381469726562500000000000000000000000001", "ab cd", "  x", "a,b", "%d"};
    const ScanFn exe = reinterpret_cast<ScanFn>(uintptr_t(0x8220AD));
    json rows = json::array();
    unsigned differences = 0;
    for (const char* in : inputs) {
        json r = {{"in", in}};
        {
            int a = 0x5A5A5A5A, b = 0x5A5A5A5A;
            const int ra = exe(in, "%d", &a), rb = sscanf(in, "%d", &b);
            r["d"] = {{"exe", json::array({ra, a})}, {"dll", json::array({rb, b})}};
            differences += (ra != rb || a != b);
        }
        {
            u32 pa = 0x5A5A5A5Au, pb = pa;
            float a{}, b{};
            std::memcpy(&a, &pa, 4);
            std::memcpy(&b, &pb, 4);
            const int ra = exe(in, "%f", &a), rb = sscanf(in, "%f", &b);
            std::memcpy(&pa, &a, 4);
            std::memcpy(&pb, &b, 4);
            r["f"] = {{"exe", json::array({ra, pa})}, {"dll", json::array({rb, pb})}};
            differences += (ra != rb || pa != pb);
        }
        {
            char a[96], b[96];
            std::memset(a, 0x5A, sizeof a);
            std::memset(b, 0x5A, sizeof b);
            a[95] = b[95] = 0;
            const int ra = exe(in, "%s", a), rb = sscanf(in, "%s", b);
            r["s"] = {{"exe", json::array({ra, HexBytes(a, std::strlen(a))})}, {"dll", json::array({rb, HexBytes(b, std::strlen(b))})}};
            differences += (ra != rb || std::strcmp(a, b) != 0);
        }
        rows.push_back(r);
    }
    return {{"rows", rows}, {"differences", differences}, {"format", "exe y dll: [valor devuelto, valor leido (int / bits del float / hex de la cadena)]"}};
}

// ------------------------------------------------------------------------------------------------ casos
inline json InputOf(const Fx& f) {
    return {{"group", f.group}, {"family", f.family}, {"line", f.line}, {"tail", f.tail}, {"model_id", f.modelId}, {"mi_label", f.miLabel}, {"store_k", f.storeK},
            {"pre_entry", HexBytes(f.preEntry.data(), kEntry)}, {"pre_over", HexBytes(f.preOver.data(), 2 * kEntry)}, {"find_result", f.findResult},
            {"mask", f.mask}, {"crt", f.crt}, {"eax", f.eax}, {"ecx", f.ecx}, {"edx", f.edx}, {"stack_seed", f.stack}};
}
inline void FillRef(const Fx& f, std::array<u8,kLineArena>& lineRef, std::array<u8,kStoreArena>& storeRef, std::array<u8,kBlockArena>& blockRef) {
    lineRef.fill(0);
    std::memcpy(lineRef.data() + kLineAt, f.line.data(), f.line.size());
    std::memcpy(lineRef.data() + kLineAt + f.line.size() + 1, f.tail.data(), f.tail.size());
    FillWords(storeRef.data(), storeRef.size(), f.n * 40503u + 17u);
    std::memcpy(storeRef.data() + kStoreAt, &f.storeK, 4);
    std::memcpy(storeRef.data() + kStoreAt + 4 + f.storeK * kEntry, f.preEntry.data(), kEntry);
    std::memcpy(storeRef.data() + kStoreAt + 4 + (f.storeK + 1) * kEntry, f.preOver.data(), 2 * kEntry);
    blockRef.fill(kBlockPre);
}
inline void CaseLoad(Env& env, Totals& t, unsigned n) {
    const Fx f = Make(n);
    const bool fits = kLineAt + f.line.size() + f.tail.size() + 2 + 64 <= kLineArena && f.modelId >= 0 && f.modelId < i32(kModelTable) && f.storeK <= kMaxStoreK;
    std::array<u8,kLineArena> lineRef{};
    std::array<u8,kStoreArena> storeRef{};
    std::array<u8,kBlockArena> blockRef{};
    if (fits) FillRef(f, lineRef, storeRef, blockRef);
    void** const tbl = reinterpret_cast<void**>(uintptr_t(0xA9B0C8));          // ms_modelInfoPtrs
    void* const fakeMi = f.miLabel == kNumMiLabels - 1 ? nullptr : static_cast<void*>(miObjects[f.miLabel].data());
    const json input = InputOf(f);
    Side sides[2];
    json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        lineArena = lineRef; storeArena = storeRef; blockArena = blockRef;
        nev = 0; texCount = 0; findResult = f.findResult;
        findName.fill(0); for (auto& x : texName) x.fill(0); texMaskNull.fill(0);
        void* savedMi = nullptr;
        if (fits) {
            savedMi = tbl[f.modelId];
            tbl[f.modelId] = fakeMi;
            sides[s] = Execute(env, *env.route, 0x5B7670, {uintptr_t(0x8220AD), uintptr_t(0x821B40), uintptr_t(0x821F40)}, candidate, [&](uintptr_t entry) {
                const u32 args[1]{u32(uintptr_t(lineArena.data() + kLineAt))};
                return Gate(entry, args, 1, f.eax, f.ecx, f.edx, f.stack, 4);      // cdecl: el llamador retira el argumento
            });
        } else {   // fixture inseguro: no se llama (queda registrado y falla)
            sides[s].reg = {0, 0, 0, 0, true, true};
            SaveFP(sides[s].fp.data()); sides[s].mxcsr = ReadMXCSR();
        }
        u8* const st = storeArena.data() + kStoreAt;
        u8* const entry = st + 4 + f.storeK * kEntry;
        std::array<u8,kEntry> ebytes{};
        std::memcpy(ebytes.data(), entry, kEntry);
        u32 v28{};
        std::memcpy(&v28, ebytes.data() + 0x28, 4);
        if (v28 == u32(uintptr_t(blockArena.data() + kBlockAt))) {   // el puntero del bloque Malloc se compara como etiqueta
            const u32 label = kBlockLabel;
            std::memcpy(ebytes.data() + 0x28, &label, 4);
        }
        for (u32 i = 0; i < kEntry; ++i) if ((f.mask >> i) & 1) ebytes[i] = 0;   // bytes que son basura de pila en el EXE (solo familias con `mask`)
        u32 countNow{};
        std::memcpy(&countNow, st, 4);
        long outside = -1;
        {
            const size_t w0 = kStoreAt + 4 + f.storeK * kEntry, w1 = w0 + 3 * kEntry;
            for (size_t i = 0; i < storeArena.size(); ++i) {
                if (i >= kStoreAt && i < kStoreAt + 4) continue;
                if (i >= w0 && i < w1) continue;
                if (storeArena[i] != storeRef[i]) { outside = long(i); break; }
            }
        }
        bool blockGuard = true;
        for (size_t i = 0; i < blockArena.size(); ++i) {
            if (i >= kBlockAt && i < kBlockAt + 0x40) continue;
            if (blockArena[i] != blockRef[i]) blockGuard = false;
        }
        json trace = json::array();
        for (unsigned i = 0; i < nev; ++i) trace.push_back(json::array({evs[i].kind, evs[i].a, evs[i].b, evs[i].c}));
        json tex = json::array();
        for (unsigned k = 0; k < texCount && k < texName.size(); ++k) tex.push_back(json::array({HexBytes(texName[k].data(), std::strlen(texName[k].data())), int(texMaskNull[k])}));
        json observed = {{"entry", HexBytes(ebytes.data(), kEntry)}, {"over", HexBytes(entry + kEntry, 2 * kEntry)}, {"count", countNow}, {"outside_ok", outside < 0}, {"first_outside", outside},
                         {"trace", trace}, {"find_name", HexBytes(findName.data(), std::strlen(findName.data()))}, {"tex", tex}, {"block", HexBytes(blockArena.data() + kBlockAt, 0x40)},
                         {"block_guard_ok", blockGuard}, {"table_ok", fits && tbl[f.modelId] == fakeMi}, {"line_ok", lineArena == lineRef}};
        if (kCompareVolatileRegisters) { observed["eax"] = sides[s].reg.eax; observed["ecx"] = sides[s].reg.ecx; observed["edx"] = sides[s].reg.edx; }
        auto fp = NormalizeFp(sides[s].fp);
        const bool watchesIntact = env.WatchesIntact();
        json out = CommonRow(sides[s], fp);
        out["observed"] = observed;
        if (!fits) out["route_ok"] = false;
        RestoreFloatingState();
        if (fits) tbl[f.modelId] = savedMi;
        out["globals_restored"] = watchesIntact && env.WatchesIntact() && (!fits || tbl[f.modelId] == savedMi);
        out["floating_state_restored"] = FloatingStateRestored();
        rows[s] = out;
    }
    Finish(env, t, "CFileLoader::Load2dEffect", n, input, rows, sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env, bool ready, const json& registered, const json& bodies) {
    const uintptr_t body = env.route ? env.route->Body() : 0;
    ready &= body && ModuleOf(body) == env.asi;
    for (const uintptr_t at : {uintptr_t(0x5B7670), uintptr_t(0x8220AD), uintptr_t(0x821B40), uintptr_t(0x821F40), uintptr_t(0x4C5A60), uintptr_t(0x4C4D20), uintptr_t(0x7F3AC0), uintptr_t(0x72F420),
                               uintptr_t(0x7316A0), uintptr_t(0x731850), uintptr_t(0x7319C0), uintptr_t(0x7316B0)})
        env.watches.emplace_back(at, 16);
    json watched = json::array();
    for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), 16)}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    PrepareFloatingState();
    const json probe = CrtProbe();
    RestoreFloatingState();
    json stubbed = json::array();
    for (const Pair* p : {&env.pStore, &env.pAdd2d, &env.pPush, &env.pFind, &env.pSet, &env.pPop, &env.pTex, &env.pMalloc}) stubbed.push_back({{"exe", p->exe}, {"dll", p->dll}});
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", 1}, {"pairs", kCases}, {"cases", {{"CFileLoader::Load2dEffect", kCases}}},
           {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
           {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
           {"collaborators", {{"exe_real", std::vector<uintptr_t>{0x8220AD, 0x821B40, 0x821F40}}, {"stubbed", stubbed}}},
           {"crt", {{"exe_sscanf", 0x8220AD}, {"exe_sscanf_in_game_image", ModuleOf(Resolve(0x8220AD)) == env.game}, {"dll_sscanf", "sscanf del CRT estatico de la DLL (/MTd); se ejecuta de verdad"}, {"probe", probe}}},
           {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game}, {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned c = 0; c < kCases; ++c) CaseLoad(env, totals, c);
        Write({{"event", "function_summary"}, {"function", "CFileLoader::Load2dEffect"}, {"cases", kCases}, {"failures", totals.failures}, {"state_restored", env.WatchesIntact()}});
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
