#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco Dependencies25A: compartido por LiveComparison.hpp (dentro del juego) y offline/main.cpp (bytes del EXE mapeados en un proceso hijo). Requiere, declarado antes:
// json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages} y OriginalDependencies499::HookRoute.
// Banco par a par Dependencies25A (grupo g25a de BRIEF_21): nueve entradas del EXE de nivel "1 Muy facil" del cohorte 2 de dependencias originales.
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con el hook encendido (la 0x734A70 no tiene hook: el candidato es la funcion de la DLL llamada
// directamente). Los colaboradores que la funcion llama son falsos y se parchean con EntryPatch en la direccion del EXE y, cuando la DLL los llama directamente, tambien
// en la entrada de su cuerpo (ver README.md). Cada par registra en bits: valor de retorno, memoria tocada, traza de colaboradores, ESP, EBX/ESI/EDI/EBP, ruta,
// estado x87 completo (FNSAVE de 108 bytes salvo FIP) y MXCSR; EAX/ECX/EDX de salida solo se comparan donde un llamador del EXE los lee (CTask::Destructor).
namespace Dependencies25ALive {
using namespace Dependencies25AFix;
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
// Deja en los ocho registros ST un residuo conocido y distinto: el FNSAVE posterior compara tambien el contenido de los registros vacios.
inline void PrimeX87() {
    static const double stale[8]{1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8};
    const double* p=stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE (salvo FIP), incluido el contenido crudo de los registros ST marcados como vacios en FTW.
// Ninguna de las nueve funciones ejecuta instrucciones x87 (ni el EXE ni el C++ de la DLL): la compuerta se mantiene y NO se relaja.
inline constexpr bool kCompareDeadX87Registers=true;

inline std::string HexBytes(const void* p,size_t n) {constexpr char d[]="0123456789abcdef";const auto* b=static_cast<const u8*>(p);std::string o;o.reserve(n*2);for(size_t i=0;i<n;++i){o+=d[b[i]>>4];o+=d[b[i]&15];}return o;}
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline std::array<uint8,108> MaskDeadRegisters(std::array<uint8,108> fp){for(unsigned st=0;st<8;++st)if(FpRegisterIsDead(fp,st))memset(fp.data()+28+10*st,0,10);return fp;}
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
    json out=json::array();
    const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw",4,2},{"ftw",8,2},{"fip",12,4},{"fcs_fop",16,4},{"fdp",20,4},{"fds",24,4}};
    for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
    for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
    return out;
}
// Vigila bytes de codigo (entradas parcheadas temporalmente): deben volver a su contenido inicial tras cada llamada.
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};

// ------------------------------------------------------------------------------------------------ llamada con medicion de ABI
struct Reg {u32 eax,ecx,edx;bool stack,nonvolatile;};
// Rellena 64 dwords bajo el ESP con SlotValue(stackSeed, k), siembra EBX/ESI/EDI, apila `argCount` argumentos (argsPtr[0] queda en la cima), carga EAX/ECX/EDX y llama.
// popBytes = bytes que debe retirar el llamador (cdecl: 4 * argumentos; thiscall/stdcall con `ret N`: 0). stack == ESP identico al de antes de apilar los argumentos.
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{};
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
    return {outEax,outEcx,outEdx,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter};
}

// ------------------------------------------------------------------------------------------------ colaboradores falsos (los mismos para el EXE y para la DLL)
struct Ctx {
    std::vector<u32> trace;
    u32 traps{};
    uintptr_t typesBase{};                                 // F4
    i32 safehouseIndex{};                                  // F5
    uintptr_t objectPtr{}; u32 atomicId{}; i32 modelIndex{};   // F6
    uintptr_t frameBase{}, foundPtr{}, cbExe{}, cbDll{}; u32 hierarchy[kNodes]{}; std::array<std::vector<unsigned>,kNodes> children{}; int depth{};   // F7
    uintptr_t decoderPtr{}, streamPtr{}, namePtr{};        // F8
};
inline Ctx ctx;
// 0x5FB7D0 CPedGroup::RemoveAllFollowers (thiscall sin pila): se registra el ECX con que se llama.
inline void __fastcall FakeRemoveAll(void* group,void*) {ctx.trace.push_back(u32(uintptr_t(group)));}
// 0x600530 CDecision::SetDefault (thiscall sin pila): se registra la posicion de la decision relativa al CDecisionMakerTypes del caso.
inline void __fastcall FakeSetDefault(void* decision,void*) {ctx.trace.push_back(u32(uintptr_t(decision))-u32(ctx.typesBase));}
// 0x4489F0 CGarages::FindSafeHouseIndexForGarageType (cdecl, 1 argumento de 32 bits en pila).
// El original (0x4489F0) solo escribe EAX: 0x44A210 carga EDX = 0 ANTES de llamarla y lo usa como contador DESPUES (resid.py lo detecta), asi que el falso tiene que
// devolver con ECX y EDX intactos. Se envuelve en ensamblador (permitido en bancos): guarda ECX/EDX, llama al nucleo en C++ y los restaura.
inline i32 __cdecl FakeFindSafehouseCore(u32 type) {ctx.trace.push_back(type);return ctx.safehouseIndex;}
inline __declspec(naked) i32 __cdecl FakeFindSafehouse(u32 type) {
    __asm {
        push ecx
        push edx
        push dword ptr [esp+12]
        call FakeFindSafehouseCore
        add esp,4
        pop edx
        pop ecx
        ret
    }
}
// 0x732370 CVisibilityPlugins::GetAtomicId y 0x732250 GetModelInfoIndex (cdecl, un atomo).
inline u32 __cdecl FakeAtomicId(void* atomic) {ctx.trace.push_back(1);ctx.trace.push_back(uintptr_t(atomic)==ctx.objectPtr);return ctx.atomicId;}
inline i32 __cdecl FakeModelIndex(void* atomic) {ctx.trace.push_back(2);ctx.trace.push_back(uintptr_t(atomic)==ctx.objectPtr);return ctx.modelIndex;}
// 0x7C5160 RpHAnimFrameGetHierarchy (cdecl) y 0x7F0DC0 RwFrameForAllChildren (cdecl): arbol de marcos falsos (el cuerpo es el del EXE: llama al callback con cada hijo hasta que devuelve nulo).
inline unsigned NodeIndex(void* frame) {return unsigned((uintptr_t(frame)-ctx.frameBase)/kFrameStride);}
inline void* __cdecl FakeHierarchy(void* frame) {const unsigned i=NodeIndex(frame);ctx.trace.push_back(1);ctx.trace.push_back(i);return reinterpret_cast<void*>(uintptr_t(i<kNodes?ctx.hierarchy[i]:0));}
inline void* __cdecl FakeChildren(void* frame,void* callback,void* data) {
    const unsigned i=NodeIndex(frame);
    ctx.trace.push_back(2);ctx.trace.push_back(i);{const uintptr_t cb=Resolve(uintptr_t(callback));ctx.trace.push_back(cb==ctx.cbExe||cb==ctx.cbDll?1u:2u);}ctx.trace.push_back(uintptr_t(data)==ctx.foundPtr?1u:0u);
    if(i>=kNodes||++ctx.depth>32){++ctx.traps;return frame;}
    for(const unsigned child:ctx.children[i]) {
        void* childFrame=reinterpret_cast<void*>(ctx.frameBase+child*kFrameStride);
        if(!reinterpret_cast<void*(__cdecl*)(void*,void*)>(callback)(childFrame,data))break;
    }
    --ctx.depth;
    return frame;
}
// 0x8214BD operator delete (cdecl): la DLL lo llama por esta misma direccion (MemoryMgr.cpp). No libera nada.
inline void __cdecl FakeDelete(void* p) {
    const uintptr_t v=uintptr_t(p);
    ctx.trace.push_back(v==ctx.decoderPtr?1u:v==ctx.streamPtr?2u:v==ctx.namePtr?3u:u32(v));
}

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,128> f0Arena{};
alignas(16) inline std::array<u8,64> f1Pool{};
alignas(16) inline std::array<u8,kPoolSlots+32> f1States{};
alignas(16) inline std::array<u8,kAutoPilotArena> f2Arena{};
alignas(16) inline std::array<u8,kTypesBytes+2*kTypesPad> f4Arena{};
alignas(16) inline std::array<u8,64> f6Object{};
alignas(16) inline std::array<u8,32> f6Data{};
alignas(16) inline std::array<u8,kNodes*kFrameStride> f7Frames{};
inline u32 f7Found{};
alignas(16) inline std::array<u8,32> f8Decoder{};
alignas(16) inline std::array<u8,0x60> f8Stream{};
alignas(16) inline std::array<u8,32> f8Name{};

// ------------------------------------------------------------------------------------------------ ejecucion de un lado
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::array<std::unique_ptr<HookRoute>,kFunctions> routes{};
    std::array<uintptr_t,kFunctions> entry{};
    uintptr_t directDll{};      // F7: funcion de la DLL (sin hook)
    uintptr_t dllRemoveAll{},dllSetDefault{},dllFindSafehouse{},dllAtomicId{},dllModelIndex{},dllHierarchy{},dllChildren{};
    std::vector<Watch> watches;
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
template<class G> Side Execute(Env& env,unsigned fn,bool candidate,G gate) {
    Side side{};
    const bool direct=!env.routes[fn];
    const uintptr_t entry=direct&&candidate?env.directDll:env.entry[fn];
    auto call=[&]{
        if(direct)side.routeOK=candidate?ModuleOf(Resolve(entry))==env.asi:(!ReversingProbe::JumpsToDll(entry)&&ModuleOf(entry)==env.game);
        else side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        side.reg=gate(entry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    if(direct)call();else env.routes[fn]->Invoke(candidate,call);
    return side;
}
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx}};}
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
// Corre un caso en los dos lados y escribe la fila `comparison`. prepare() deja la memoria del caso; gate(entry) llama; observe(side) lee lo que la funcion deja;
// restore() devuelve la memoria global tocada; restored() lo comprueba.
template<class P,class G,class O,class R,class RC>
void RunPair(Env& env,Totals& totals,unsigned fn,const char* name,unsigned n,const json& input,P prepare,G gate,O observe,R restore,RC restored) {
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        ctx.trace.clear();ctx.traps=0;ctx.depth=0;
        prepare();
        sides[s]=Execute(env,fn,candidate,gate);
        json observed=observe(sides[s]);
        auto fp=sides[s].fp;memset(fp.data()+12,0,4);
        const auto logged=kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);
        const bool watchesIntact=env.WatchesIntact();
        restore();
        json out={{"observed",observed},{"post_floating_state",HexBytes(logged.data(),108)},{"post_mxcsr",sides[s].mxcsr},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"route_ok",sides[s].routeOK},{"traps_zero",ctx.traps==0}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&restored();out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","route_ok","traps_zero","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    auto fa=sides[0].fp,fb=sides[1].fp;memset(fa.data()+12,0,4);memset(fb.data()+12,0,4);
    totals.passed&=ok;totals.failures+=!ok;
    Write({{"event","comparison"},{"function",name},{"case",n},{"pair",totals.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ 0 CTask::Destructor (0x61A3A0)
inline void CaseF0(Env& env,Totals& t,unsigned n) {
    const F0 f=MakeF0(n);
    std::array<u8,128> initial{};initial.fill(0xA5);
    u8* const obj=f0Arena.data()+32+f.offset;u8* const objInitial=initial.data()+32+f.offset;
    FillWords(objInitial,kF0Bytes,f.seed);memcpy(objInitial,&f.first,4);
    const json input={{"offset",f.offset},{"object_in",HexBytes(objInitial,kF0Bytes)},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,0,"CTask::Destructor",n,input,
        [&]{f0Arena=initial;},
        [&](uintptr_t entry){return Gate(entry,nullptr,0,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);},
        [&](const Side& s){
            const bool guards=memcmp(f0Arena.data(),initial.data(),32+f.offset)==0&&memcmp(obj+kF0Bytes,objInitial+kF0Bytes,f0Arena.size()-32-f.offset-kF0Bytes)==0;
            // Los 259 llamadores del EXE conservan EAX/ECX/EDX: se comparan enteros y el oraculo exige EAX/EDX de entrada y ECX = el objeto.
            return json{{"object",HexBytes(obj,kF0Bytes)},{"guards_ok",guards},{"eax",s.reg.eax},{"ecx_is_object",s.reg.ecx==u32(uintptr_t(obj))},{"edx",s.reg.edx}};
        },
        [&]{f0Arena.fill(0);},[&]{return true;});
}

// ------------------------------------------------------------------------------------------------ 1 CVehiclePool::GetAt (0x41CC10)
inline void CaseF1(Env& env,Totals& t,unsigned n) {
    const F1 f=MakeF1(n);
    std::array<u8,64> pool{};FillWords(pool.data(),pool.size(),f.poolSeed);
    std::array<u8,kPoolSlots+32> states{};states.fill(0xA5);BuildStates(f,states.data());
    const u32 statesPtr=u32(uintptr_t(f1States.data())),capacity=kPoolSlots;
    memcpy(pool.data(),&f.storage,4);memcpy(pool.data()+4,&statesPtr,4);memcpy(pool.data()+8,&capacity,4);
    const json input={{"idx",f.idx},{"storage",f.storage},{"state_kind",f.stateKind},{"state_byte",states[f.idx]},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,1,"CVehiclePool::GetAt",n,input,
        [&]{f1Pool=pool;f1States=states;},
        [&](uintptr_t entry){const u32 args[1]{f.idx};return Gate(entry,args,1,f.eax,u32(uintptr_t(f1Pool.data())),f.edx,f.stack,0);},
        [&](const Side& s){return json{{"eax",s.reg.eax},{"pool_unchanged",f1Pool==pool},{"states_unchanged",f1States==states}};},
        [&]{},[&]{return true;});
}

// ------------------------------------------------------------------------------------------------ 2 CAutoPilot::RemoveOnePathNode (0x41B950)
inline void CaseF2(Env& env,Totals& t,unsigned n) {
    const F2 f=MakeF2(n);
    std::vector<u8> ref(f.window+kAutoPilotGuard,0xC3);
    FillWords(ref.data(),f.window,f.seed);
    const u16 count=u16(f.count);memcpy(ref.data()+0x88,&count,2);memcpy(ref.data()+0x8C,&f.w8c,4);
    u8* const obj=f2Arena.data()+kAutoPilotOffset;
    const json input={{"count",f.count},{"w8c",f.w8c},{"seed",f.seed},{"window",f.window},{"head_in",HexBytes(ref.data(),0xA0)},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,2,"CAutoPilot::RemoveOnePathNode",n,input,
        [&]{memcpy(obj,ref.data(),ref.size());},
        [&](uintptr_t entry){return Gate(entry,nullptr,0,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);},
        [&](const Side&){
            u16 after{};memcpy(&after,obj+0x88,2);
            return json{{"hash",Fnv64(obj,f.window)},{"head",HexBytes(obj,0xA0)},{"guard_ok",memcmp(obj+f.window,ref.data()+f.window,kAutoPilotGuard)==0},{"count_after",after}};
        },
        [&]{},[&]{return true;});
}

// ------------------------------------------------------------------------------------------------ 3 CPedGroups::RemoveAllFollowersFromGroup (0x5FB8A0)
inline void CaseF3(Env& env,Totals& t,unsigned n,const std::array<u8,kGroupRegionBytes>& savedRegion) {
    const F3 f=MakeF3(n);
    u8* const region=reinterpret_cast<u8*>(kGroupRegion);
    std::array<u8,kGroupRegionBytes> fixture{};BuildGroupRegion(f,fixture.data());
    // Byte que lee la funcion en ms_activeGroups[id]: de la region si cae dentro (ya con la fixture) y, si no, memoria viva del EXE (no modificada por el banco).
    const uintptr_t at=uintptr_t(kActiveGroups)+uintptr_t(f.id);
    const u8 byteForId=at>=kGroupRegion&&at<kGroupRegion+kGroupRegionBytes?fixture[at-kGroupRegion]:*reinterpret_cast<u8*>(at);
    const json input={{"id",f.id},{"region",HexBytes(fixture.data(),fixture.size())},{"byte_at_id",byteForId},{"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,3,"CPedGroups::RemoveAllFollowersFromGroup",n,input,
        [&]{memcpy(region,fixture.data(),fixture.size());},
        [&](uintptr_t entry){
            ReversingProbe::EntryPatch exe(0x5FB7D0,&FakeRemoveAll),dll(env.dllRemoveAll,&FakeRemoveAll);
            const u32 args[1]{u32(f.id)};return Gate(entry,args,1,f.eax,f.ecx,f.edx,f.stack,4);
        },
        [&](const Side&){return json{{"trace",ctx.trace},{"region_after",HexBytes(region,kGroupRegionBytes)}};},
        [&]{memcpy(region,savedRegion.data(),kGroupRegionBytes);},[&]{return memcmp(region,savedRegion.data(),kGroupRegionBytes)==0;});
}

// ------------------------------------------------------------------------------------------------ 4 CDecisionMakerTypes::FlushDecisionMakerEventResponse (0x604490)
inline void CaseF4(Env& env,Totals& t,unsigned n) {
    const F4 f=MakeF4(n);
    std::array<u8,kTypesBytes+2*kTypesPad> ref{};ref.fill(0xA5);
    u8* const types=f4Arena.data()+kTypesPad;
    FillWords(ref.data()+kTypesPad,kTypesBytes,f.seed);
    u32 raw{};memcpy(&raw,ref.data()+kTypesPad+kEventIndices+4*std::ptrdiff_t(f.b),4);
    ctx.typesBase=uintptr_t(types);
    const json input={{"a",f.a},{"b",f.b},{"seed",f.seed},{"event_index_raw",raw},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,4,"CDecisionMakerTypes::FlushDecisionMakerEventResponse",n,input,
        [&]{f4Arena=ref;ctx.typesBase=uintptr_t(types);},
        [&](uintptr_t entry){
            ReversingProbe::EntryPatch exe(0x600530,&FakeSetDefault),dll(env.dllSetDefault,&FakeSetDefault);
            const u32 args[2]{u32(f.a),u32(f.b)};return Gate(entry,args,2,f.eax,u32(uintptr_t(types)),f.edx,f.stack,0);
        },
        [&](const Side&){return json{{"trace",ctx.trace},{"arena_unchanged",f4Arena==ref}};},
        [&]{},[&]{return true;});
}

// ------------------------------------------------------------------------------------------------ 5 CGarages::CountCarsInHideoutGarage (0x44A210)
inline void CaseF5(Env& env,Totals& t,unsigned n,const std::vector<u8>& savedTable) {
    const F5 f=MakeF5(n);
    std::vector<u8> table(kSafehouseBytes);FillWords(table.data(),table.size(),f.seed);
    for(u32 s=0;s<20;++s)for(u32 k=0;k<4;++k) {
        const u16 model=StoredModel(Rand(f.seed,1000+s*4+k));
        memcpy(table.data()+s*kSafehouseStride+k*kSlotStride+kSafehouseModel,&model,2);
    }
    if(f.index>=0&&f.index<20)for(u32 k=0;k<4;++k) {
        const u32 mode=(f.zeroMask>>(2*k))&3;u16 model{};
        u8* slot=table.data()+u32(f.index)*kSafehouseStride+k*kSlotStride+kSafehouseModel;
        if(mode==0)model=0;else if(mode==1)model=u16(Rand(f.seed,2000+k)|1);else{memcpy(&model,slot,2);}
        memcpy(slot,&model,2);
    }
    u8* const live=reinterpret_cast<u8*>(kSafehouses);
    // Palabras de modelo que leera la funcion (para indices fuera de la tabla son memoria viva del EXE, no modificada por el banco).
    auto readModels=[&]{json words=json::array();for(u32 k=0;k<4;++k){u16 w{};memcpy(&w,reinterpret_cast<void*>(uintptr_t(kSafehouses)+kSafehouseModel+uintptr_t(i32(f.index))*kSafehouseStride+k*kSlotStride),2);words.push_back(w);}return words;};
    memcpy(live,table.data(),table.size());
    const json input={{"type_arg",f.typeArg},{"index",f.index},{"seed",f.seed},{"model_words",readModels()},{"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,5,"CGarages::CountCarsInHideoutGarage",n,input,
        [&]{memcpy(live,table.data(),table.size());ctx.safehouseIndex=f.index;},
        [&](uintptr_t entry){
            ReversingProbe::EntryPatch exe(0x4489F0,&FakeFindSafehouse),dll(env.dllFindSafehouse,&FakeFindSafehouse);
            const u32 args[1]{f.typeArg};return Gate(entry,args,1,f.eax,f.ecx,f.edx,f.stack,4);
        },
        [&](const Side& s){return json{{"eax",s.reg.eax},{"trace",ctx.trace},{"table_unchanged",memcmp(live,table.data(),table.size())==0},{"models_after",readModels()}};},
        [&]{memcpy(live,savedTable.data(),savedTable.size());},[&]{return memcmp(live,savedTable.data(),savedTable.size())==0;});
}

// ------------------------------------------------------------------------------------------------ 6 FindReplacementUpgradeCB (0x6D3490)
inline void CaseF6(Env& env,Totals& t,unsigned n) {
    const F6 f=MakeF6(n);
    std::array<u8,64> object{};object.fill(0xA5);FillWords(object.data(),16,f.objectSeed);object[0]=f.type;
    std::array<u8,32> data{};data.fill(0xA5);FillWords(data.data(),16,f.dataSeed);
    const uintptr_t objectPtr=uintptr_t(f6Object.data()),dataPtr=uintptr_t(f6Data.data());
    const json input={{"type",f.type},{"atomic_id",f.atomicId},{"model_index",f.modelIndex},{"object_ptr",u32(objectPtr)},{"data_ptr",u32(dataPtr)},{"data_in",HexBytes(data.data(),16)},
                      {"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,6,"FindReplacementUpgradeCB",n,input,
        [&]{f6Object=object;f6Data=data;ctx.objectPtr=objectPtr;ctx.atomicId=f.atomicId;ctx.modelIndex=f.modelIndex;},
        [&](uintptr_t entry){
            ReversingProbe::EntryPatch idExe(0x732370,&FakeAtomicId),idDll(env.dllAtomicId,&FakeAtomicId),miExe(0x732250,&FakeModelIndex),miDll(env.dllModelIndex,&FakeModelIndex);
            const u32 args[2]{u32(objectPtr),u32(dataPtr)};return Gate(entry,args,2,f.eax,f.ecx,f.edx,f.stack,8);
        },
        [&](const Side& s){return json{{"eax",s.reg.eax},{"data",HexBytes(f6Data.data(),16)},{"data_guard_ok",memcmp(f6Data.data()+16,data.data()+16,16)==0},{"object_unchanged",f6Object==object},{"trace",ctx.trace}};},
        [&]{},[&]{return true;});
}

// ------------------------------------------------------------------------------------------------ 7 GetAnimHierarchyFromFrameCallback (0x734A70, sin hook)
inline void CaseF7(Env& env,Totals& t,unsigned n) {
    const F7 f=MakeF7(n);
    for(auto& c:ctx.children)c.clear();
    for(unsigned i=1;i<f.nodeCount;++i)ctx.children[unsigned(f.parent[i])].push_back(i);
    json parents=json::array(),hier=json::array();
    for(unsigned i=0;i<kNodes;++i){parents.push_back(f.parent[i]);hier.push_back(f.hierarchy[i]);}
    const json input={{"node_count",f.nodeCount},{"hierarchy",hier},{"parent",parents},{"found_in",f.found},{"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,7,"RwHelper::GetAnimHierarchyFromFrameCallback",n,input,
        [&]{
            ctx.frameBase=uintptr_t(f7Frames.data());ctx.foundPtr=uintptr_t(&f7Found);ctx.cbExe=0x734A70;ctx.cbDll=env.directDll;
            for(unsigned i=0;i<kNodes;++i)ctx.hierarchy[i]=f.hierarchy[i];
            f7Found=f.found;
        },
        [&](uintptr_t entry){
            // El grupo g25b puede sustituir RpHAnimFrameGetHierarchy por una reconstruccion con cuerpo propio: se parchean la direccion del EXE y el cuerpo de la DLL de ambos.
            ReversingProbe::EntryPatch hierarchy(0x7C5160,&FakeHierarchy),hierarchyDll(env.dllHierarchy,&FakeHierarchy),children(0x7F0DC0,&FakeChildren),childrenDll(env.dllChildren,&FakeChildren);
            const u32 args[2]{u32(ctx.frameBase),u32(ctx.foundPtr)};return Gate(entry,args,2,f.eax,f.ecx,f.edx,f.stack,8);
        },
        [&](const Side& s){
            i32 returned=-1;
            if(s.reg.eax!=0){const uintptr_t off=uintptr_t(s.reg.eax)-ctx.frameBase;returned=(off%kFrameStride==0&&off/kFrameStride<kNodes)?i32(off/kFrameStride):1000;}
            return json{{"returned_node",returned},{"found",f7Found},{"trace",ctx.trace}};
        },
        [&]{f7Found=0;},[&]{return true;});
}

// ------------------------------------------------------------------------------------------------ 8 CAEStreamingDecoder scalar deleting destructor (0x4F2860)
inline void CaseF8(Env& env,Totals& t,unsigned n) {
    const F8 f=MakeF8(n);
    std::array<u8,32> decoder{};decoder.fill(0xA5);std::array<u8,0x60> stream{};stream.fill(0xA5);
    const uintptr_t decoderPtr=uintptr_t(f8Decoder.data()),streamPtr=uintptr_t(f8Stream.data()),namePtr=uintptr_t(f8Name.data());
    FillWords(decoder.data(),8,f.decoderSeed);memcpy(decoder.data(),&f.vptr,4);
    const u32 streamField=f.streamPresent?u32(streamPtr):0u;memcpy(decoder.data()+4,&streamField,4);
    FillWords(stream.data(),kStreamBytes,f.streamSeed);
    stream[0xC]=0;   // m_bIsOpen = false: no se cierra ningun HANDLE
    const u32 nameField=f.filenameKind==0?0u:f.filenameKind==1?u32(namePtr):f.filenameKind==2?0xDEADBEE0u:0x00BADF00u;memcpy(stream.data()+8,&nameField,4);
    const json input={{"flags",f.flags},{"stream_present",f.streamPresent},{"filename_kind",f.filenameKind},{"decoder_in",HexBytes(decoder.data(),8)},{"stream_in",HexBytes(stream.data(),kStreamBytes)},
                      {"decoder_ptr",u32(decoderPtr)},{"stream_ptr",u32(streamPtr)},{"name_ptr",u32(namePtr)},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
    RunPair(env,t,8,"CAEStreamingDecoder::ScalarDeletingDestructor",n,input,
        [&]{f8Decoder=decoder;f8Stream=stream;ctx.decoderPtr=decoderPtr;ctx.streamPtr=streamPtr;ctx.namePtr=namePtr;},
        [&](uintptr_t entry){
            ReversingProbe::EntryPatch del(0x8214BD,&FakeDelete);
            const u32 args[1]{f.flags};return Gate(entry,args,1,f.eax,u32(decoderPtr),f.edx,f.stack,0);
        },
        [&](const Side& s){
            // El primer dword del flujo es la vtable que deja el destructor de CAEDataStream (la de la DLL cuando 0x4DC490 esta enganchada): se enmascara.
            std::array<u8,kStreamBytes> after{};memcpy(after.data(),f8Stream.data(),kStreamBytes);memset(after.data(),0,4);
            return json{{"eax_is_decoder",s.reg.eax==u32(decoderPtr)},{"decoder",HexBytes(f8Decoder.data(),8)},{"stream",HexBytes(after.data(),kStreamBytes)},
                        {"stream_guard_ok",memcmp(f8Stream.data()+kStreamBytes,stream.data()+kStreamBytes,f8Stream.size()-kStreamBytes)==0},{"trace",ctx.trace}};
        },
        [&]{},[&]{return true;});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
// El llamador (Update en el juego, Child en el arnes sin juego) rellena Env: modulos, rutas RH (nula para la 0x734A70, sin hook) y cuerpos de la DLL.
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    env.entry={0x61A3A0,0x41CC10,0x41B950,0x5FB8A0,0x604490,0x44A210,0x6D3490,0x734A70,0x4F2860};
    bool collaboratorsOK=true;
    for(const uintptr_t body:{env.directDll,env.dllRemoveAll,env.dllSetDefault,env.dllFindSafehouse,env.dllAtomicId,env.dllModelIndex,env.dllHierarchy,env.dllChildren})collaboratorsOK&=body&&ModuleOf(body)==env.asi;
    // La 0x734A70 (funcion bajo prueba sin hook) debe ser hoy codigo del EXE sin salto a la DLL; los colaboradores pueden estar enganchados (el EntryPatch pisa el salto y se restaura).
    collaboratorsOK&=!ReversingProbe::JumpsToDll(0x734A70)&&ModuleOf(Resolve(0x734A70))==env.game;
    ready&=collaboratorsOK;
    for(const uintptr_t at:{uintptr_t(0x61A3A0),uintptr_t(0x41CC10),uintptr_t(0x41B950),uintptr_t(0x5FB8A0),uintptr_t(0x604490),uintptr_t(0x44A210),uintptr_t(0x6D3490),uintptr_t(0x734A70),uintptr_t(0x4F2860),
                            uintptr_t(0x5FB7D0),uintptr_t(0x600530),uintptr_t(0x4489F0),uintptr_t(0x732370),uintptr_t(0x732250),uintptr_t(0x7C5160),uintptr_t(0x7F0DC0),uintptr_t(0x8214BD),
                            env.directDll,env.dllRemoveAll,env.dllSetDefault,env.dllFindSafehouse,env.dllAtomicId,env.dllModelIndex,env.dllHierarchy,env.dllChildren})env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    std::array<u8,kGroupRegionBytes> savedActive{};memcpy(savedActive.data(),reinterpret_cast<void*>(kGroupRegion),kGroupRegionBytes);
    std::vector<u8> savedTable(kSafehouseBytes);memcpy(savedTable.data(),reinterpret_cast<void*>(kSafehouses),kSafehouseBytes);
    ctx.trace.reserve(256);
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();
    static constexpr const char* names[kFunctions]{"CTask::Destructor","CVehiclePool::GetAt","CAutoPilot::RemoveOnePathNode","CPedGroups::RemoveAllFollowersFromGroup",
        "CDecisionMakerTypes::FlushDecisionMakerEventResponse","CGarages::CountCarsInHideoutGarage","FindReplacementUpgradeCB","RwHelper::GetAnimHierarchyFromFrameCallback","CAEStreamingDecoder::ScalarDeletingDestructor"};
    for(const char* name:names)cases[name]=env.cases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*env.cases},{"cases",cases},{"direct_functions",json::array({"RwHelper::GetAnimHierarchyFromFrameCallback"})},
        {"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",std::vector<uintptr_t>{0x5FB7D0,0x600530,0x4489F0,0x732370,0x732250,0x7C5160,0x7F0DC0,0x8214BD}},
                          {"dll",std::vector<uintptr_t>{env.dllRemoveAll,env.dllSetDefault,env.dllFindSafehouse,env.dllAtomicId,env.dllModelIndex,env.dllHierarchy,env.dllChildren}}}},
        {"watched_entries",watched},{"bodies",bodies},{"direct_dll",env.directDll},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_active_groups",HexBytes(savedActive.data(),savedActive.size())},
        {"initial_safehouse_hash",Fnv64(savedTable.data(),savedTable.size())}});
    Totals totals;
    for(unsigned fi=0;fi<kFunctions&&ready;++fi) {
        const unsigned failuresBefore=totals.failures;
        for(unsigned c=0;c<env.cases;++c) {
            switch(fi) {
            case 0:CaseF0(env,totals,c);break;
            case 1:CaseF1(env,totals,c);break;
            case 2:CaseF2(env,totals,c);break;
            case 3:CaseF3(env,totals,c,savedActive);break;
            case 4:CaseF4(env,totals,c);break;
            case 5:CaseF5(env,totals,c,savedTable);break;
            case 6:CaseF6(env,totals,c);break;
            case 7:CaseF7(env,totals,c);break;
            default:CaseF8(env,totals,c);break;
            }
        }
        Write({{"event","function_summary"},{"function",names[fi]},{"cases",env.cases},{"failures",totals.failures-failuresBefore},{"state_restored",env.WatchesIntact()}});
    }
    // Las vigilancias se tomaron con los hooks apagados: se comprueban ANTES de Restore(), que vuelve a encenderlos.
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&memcmp(reinterpret_cast<void*>(kGroupRegion),savedActive.data(),kGroupRegionBytes)==0&&memcmp(reinterpret_cast<void*>(kSafehouses),savedTable.data(),savedTable.size())==0;
    for(auto& r:env.routes)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"active_groups",HexBytes(reinterpret_cast<void*>(kGroupRegion),kGroupRegionBytes)},{"safehouse_hash",Fnv64(reinterpret_cast<const u8*>(kSafehouses),kSafehouseBytes)}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
