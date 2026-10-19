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
#include <utility>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco PedSaveExtract28. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/FileLoaderLoad2dEffect27 (Core.hpp) y reversing/DecisionAdd27.
//
// Banco par a par de UNA funcion: CPedSaveStructure::Extract (0x5D44B0, thiscall, ECX = estructura de 0x18C bytes, 1 argumento `CPed* ped`, `ret 4`).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. En ambos lados se sustituyen, SOLO durante la llamada y con el MISMO falso,
// los seis colaboradores (parches de entrada en la direccion del EXE Y en el cuerpo de la DLL, calculado ANTES de parchear):
//   CPed::SetCharCreatedBy (0x5E47E0)  CWeaponInfo::GetWeaponInfo (0x743C60)  CStreaming::RequestModel (0x4087E0)  CStreaming::LoadAllRequestedModels (0x40EA10)
//   CPed::GiveWeapon (0x5E6080)        CPed::SetCurrentWeapon(int32) (0x5E61F0)
// Cada falso registra [tipo, a, b, c, d, resumen del estado del ped] y tiene un efecto en la memoria del ped (ver fixture_oracle.py) que prueba el ORDEN de las escrituras de Extract.
// El "ped" es una arena de 0x79C bytes (sin constructor); su matriz y el pool de entradas/salidas (global 0x96A7D8) tambien son falsos del banco. Ninguna otra global se toca.
// La palabra de control x87 se fija a 24 bits, al mas cercano y con todas las excepciones enmascaradas (estado del juego). EAX/ECX/EDX de salida: solo se registran.
namespace PedSaveExtract28Live {
using namespace PedSaveExtract28Fix;
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
// La funcion usa x87 (la posicion se copia con fld/fstp en el EXE): los registros ST vacios guardan restos distintos, la palabra de estado lleva
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
alignas(16) inline std::array<u8,kPedArena> pedArena{};          // guarda | ped (0x79C) | guarda
alignas(16) inline std::array<u8,kSaveArena> saveArena{};        // guarda | estructura guardada (0x18C) | guarda
alignas(16) inline std::array<u8,kMatArena> matArena{};          // guarda | matriz (0x48) | guarda
alignas(16) inline std::array<u8,kStatesArena> statesArena{};    // guarda | 16 + 2048 estados | guarda
alignas(16) inline std::array<u8,0x40> storageDummy{};           // el "almacenamiento" del pool: solo su direccion (el resultado es almacenamiento + indice * 0x3C)
struct FakePool {u32 storage,states,pad[6];};                    // los dos primeros miembros de CPool: almacenamiento y estados
inline FakePool fakePool{};
alignas(16) inline std::array<std::array<u8,0x70>,kMaxWeapons+1> infoBlocks{};   // bloques falsos de CWeaponInfo (+0Ch modelo 1, +10h modelo 2); el ultimo es el defecto (-1, -1)
inline std::array<u32,kMaxWeapons> infoTypes{};
inline unsigned infoCount{};
inline u8* gPed{};inline u8* gMat{};inline unsigned giveCalls{};
inline u32& PoolGlobal(){return *reinterpret_cast<u32*>(uintptr_t(0x96A7D8));}   // CEntryExitManager::mp_poolEntryExits

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
enum : u32 {EvCreated=1,EvInfo=2,EvRequest=3,EvLoad=4,EvGive=5,EvSetCur=6};
struct Ev {u32 kind,a,b,c,d,dig;};
inline std::array<Ev,160> evs{};inline unsigned nev{};
// Resumen del estado del ped en la entrada de cada llamada: ped[4..0F], matriz[30..3B], vida, armadura, bytes 718/72D/72E/2F y la municion del cargador de las trece armas.
inline u32 Digest(){
    u8 buf[88];u8* p=buf;
    std::memcpy(p,gPed+4,12);p+=12;std::memcpy(p,gMat+0x30,12);p+=12;std::memcpy(p,gPed+0x540,4);p+=4;std::memcpy(p,gPed+0x548,4);p+=4;
    *p++=gPed[0x718];*p++=gPed[0x72D];*p++=gPed[0x72E];*p++=gPed[0x2F];
    for(u32 i=0;i<kMaxWeapons;++i){std::memcpy(p,gPed+0x5A0+0x1C*i+8,4);p+=4;}
    return Fnv32(buf,sizeof buf);
}
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c,d,Digest()};++nev;}}
inline u32 Label(const void* self){return self==gPed?1u:0xBADu;}

// Falsos. thiscall con N argumentos en pila = __fastcall con (this, edx, args...) (el llamado retira N*4 bytes con `ret`); cdecl: el llamador retira los argumentos.
inline void __fastcall FakeCreated(void* self,void*,u32 createdBy){Push(EvCreated,createdBy&0xFFu,Label(self));gPed[0x72D]=0xA1;gPed[0x72E]=0xA2;gPed[0x2F]=0xA3;}
inline void* __cdecl FakeInfo(u32 type,u32 skill){
    Push(EvInfo,type,skill&0xFFu);
    for(unsigned i=0;i<infoCount;++i)if(infoTypes[i]==type)return infoBlocks[i].data();
    return infoBlocks[kMaxWeapons].data();
}
inline void __cdecl FakeRequest(i32 id,i32 flags){Push(EvRequest,u32(id),u32(flags));}
inline void __cdecl FakeLoad(u32 arg){Push(EvLoad,arg&0xFFu);}
inline u32 __fastcall FakeGive(void* self,void*,u32 type,u32 ammo,u32 likeUnused){
    Push(EvGive,Label(self),type,ammo,likeUnused&0xFFu);
    const u32 k=giveCalls%kMaxWeapons;
    const u32 mark=0xC0DE0000u|giveCalls;
    std::memcpy(gPed+0x5A0+0x1C*k+8,&mark,4);
    ++giveCalls;
    return 0;
}
inline void __fastcall FakeSetCur(void* self,void*,i32 slot){Push(EvSetCur,Label(self),u32(slot));gPed[0x718]=u8(slot);gPed[0x2F]=0xA5;}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x5D44B0 y las seis entradas parcheadas: deben seguir siendo el codigo original al volver
    Pair pCreated{},pInfo{},pRequest{},pLoad{},pGive{},pSetCur{};   // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,24> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){
    p.Add(e.pCreated,&FakeCreated);p.Add(e.pInfo,&FakeInfo);p.Add(e.pRequest,&FakeRequest);p.Add(e.pLoad,&FakeLoad);p.Add(e.pGive,&FakeGive);p.Add(e.pSetCur,&FakeSetCur);
}

// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). `exeOnly`: direcciones del EXE que no pueden saltar a la DLL
// (no se usa en este banco). Los parches de los colaboradores se instalan despues de comprobar la ruta y se quitan antes de volver.
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
inline json InputOf(const Fx& f) {
    json infos=json::array();
    for(const auto& i:f.infos)infos.push_back(json::array({i.type,i.model1,i.model2}));
    return {{"family",f.family},{"save",HexBytes(f.save.data(),f.save.size())},{"has_matrix",int(f.hasMatrix)},{"ped_mode",f.pedMode},{"ped_seed",f.pedSeed},{"ped_const",f.pedConst},
            {"mat_mode",f.matMode},{"mat_seed",f.matSeed},{"mat_const",f.matConst},{"states_mode",f.statesMode},{"states_seed",f.statesSeed},{"states_const",f.statesConst},
            {"state_valid",int(f.stateValid)},{"state_byte",f.stateByte},{"exit_idx",f.exitIdx},{"infos",infos},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseExtract(Env& env, Totals& t, unsigned n) {
    const Fx f = Make(n);
    std::array<u8,kPedArena> pedRef{};
    std::array<u8,kSaveArena> saveRef{};
    std::array<u8,kMatArena> matRef{};
    std::array<u8,kStatesArena> statesRef{};
    pedRef.fill(0xC3); saveRef.fill(0xA7); matRef.fill(0xB9); statesRef.fill(0xD5);
    FillPre(pedRef.data()+kPedAt,kPedSize,f.pedMode,f.pedSeed,f.pedConst);
    FillPre(matRef.data()+kMatAt,kMatSize,f.matMode,f.matSeed,f.matConst);
    FillPre(statesRef.data()+kStatesAt-16,16+2048,f.statesMode,f.statesSeed,f.statesConst);
    std::memcpy(saveRef.data()+kSaveAt,f.save.data(),kSaveSize);
    if(f.stateValid&&f.exitIdx>=kStatesLow&&f.exitIdx<=kStatesHigh)statesRef[kStatesAt+f.exitIdx]=u8(f.stateByte);
    const u32 matPtr=f.hasMatrix?u32(uintptr_t(matArena.data()+kMatAt)):0u;
    std::memcpy(pedRef.data()+kPedAt+0x14,&matPtr,4);
    const bool fits=!f.stateValid||(f.exitIdx>=kStatesLow&&f.exitIdx<=kStatesHigh);
    const json input = InputOf(f);
    Side sides[2];
    json rows[2];
    const u32 savedPool=PoolGlobal();
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        pedArena=pedRef;saveArena=saveRef;matArena=matRef;statesArena=statesRef;
        gPed=pedArena.data()+kPedAt;gMat=matArena.data()+kMatAt;giveCalls=0;nev=0;
        infoCount=unsigned(f.infos.size());
        for(auto& b:infoBlocks)b.fill(0);
        for(unsigned i=0;i<infoCount;++i){infoTypes[i]=f.infos[i].type;std::memcpy(infoBlocks[i].data()+0xC,&f.infos[i].model1,4);std::memcpy(infoBlocks[i].data()+0x10,&f.infos[i].model2,4);}
        {const i32 none=-1;std::memcpy(infoBlocks[kMaxWeapons].data()+0xC,&none,4);std::memcpy(infoBlocks[kMaxWeapons].data()+0x10,&none,4);}
        fakePool=FakePool{};fakePool.storage=u32(uintptr_t(storageDummy.data()));fakePool.states=u32(uintptr_t(statesArena.data()+kStatesAt));
        const FakePool poolRef=fakePool;
        if (fits) {
            PoolGlobal()=u32(uintptr_t(&fakePool));
            sides[s] = Execute(env, *env.route, 0x5D44B0, std::initializer_list<uintptr_t>{}, candidate, [&](uintptr_t entry) {
                const u32 args[1]{u32(uintptr_t(gPed))};
                return Gate(entry, args, 1, f.eax, u32(uintptr_t(saveArena.data()+kSaveAt)), f.edx, f.stack, 0);      // thiscall: el llamado retira el argumento
            });
            PoolGlobal()=savedPool;
        } else {   // fixture inseguro: no se llama (queda registrado y falla)
            sides[s].reg = {0, 0, 0, 0, true, true};
            SaveFP(sides[s].fp.data()); sides[s].mxcsr = ReadMXCSR();
        }
        // ---- observacion
        std::array<u8,0x40> head{};
        std::memcpy(head.data(),gPed,0x40);
        u32 matPtrNow{};std::memcpy(&matPtrNow,gPed+0x14,4);
        std::memset(head.data()+0x14,0,4);
        auto hex=[&](u32 from,u32 to){return HexBytes(gPed+from,to-from);};
        u32 rest=2166136261u;
        static constexpr u32 ranges[5][2]{{0x40,0x540},{0x550,0x5A0},{0x720,0x728},{0x730,0x78C},{0x790,0x79C}};
        for(const auto& r:ranges)rest=Fnv32(gPed+r[0],r[1]-r[0],rest);
        u32 enexRaw{};std::memcpy(&enexRaw,gPed+0x78C,4);
        json enex=nullptr;
        if(enexRaw!=0)enex=i32(enexRaw-fakePool.storage);
        const bool pedGuard=std::memcmp(pedArena.data(),pedRef.data(),kPedAt)==0&&std::memcmp(pedArena.data()+kPedAt+kPedSize,pedRef.data()+kPedAt+kPedSize,kGuard)==0;
        const bool matGuard=std::memcmp(matArena.data(),matRef.data(),kMatAt)==0&&std::memcmp(matArena.data()+kMatAt+kMatSize,matRef.data()+kMatAt+kMatSize,kGuard)==0;
        json trace = json::array();
        for (unsigned i = 0; i < nev; ++i) trace.push_back(json::array({evs[i].kind, evs[i].a, evs[i].b, evs[i].c, evs[i].d, evs[i].dig}));
        json observed = {{"ped_head", HexBytes(head.data(),0x40)}, {"ped_hp", hex(0x540,0x550)}, {"ped_wep", hex(0x5A0,0x720)}, {"ped_style", hex(0x728,0x730)}, {"ped_rest", rest},
                         {"enex", enex}, {"mat", HexBytes(gMat,kMatSize)}, {"trace", trace}, {"ped_guard_ok", pedGuard}, {"mat_guard_ok", matGuard},
                         {"save_ok", saveArena==saveRef}, {"states_ok", statesArena==statesRef}, {"pool_ok", std::memcmp(&fakePool,&poolRef,sizeof(FakePool))==0&&PoolGlobal()==savedPool},
                         {"mat_ptr_kept", matPtrNow==matPtr}};
        if (kCompareVolatileRegisters) { observed["eax"] = sides[s].reg.eax; observed["ecx"] = sides[s].reg.ecx; observed["edx"] = sides[s].reg.edx; }
        auto fp = NormalizeFp(sides[s].fp);
        const bool watchesIntact = env.WatchesIntact();
        json out = CommonRow(sides[s], fp);
        out["observed"] = observed;
        if (!fits) out["route_ok"] = false;
        RestoreFloatingState();
        PoolGlobal()=savedPool;
        out["globals_restored"] = watchesIntact && env.WatchesIntact() && PoolGlobal()==savedPool;
        out["floating_state_restored"] = FloatingStateRestored();
        rows[s] = out;
    }
    Finish(env, t, "CPedSaveStructure::Extract", n, input, rows, sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env, bool ready, const json& registered, const json& bodies) {
    const uintptr_t body = env.route ? env.route->Body() : 0;
    ready &= body && ModuleOf(body) == env.asi;
    for (const Pair* p : {&env.pCreated, &env.pInfo, &env.pRequest, &env.pLoad, &env.pGive, &env.pSetCur}) ready &= p->dll != 0 && p->exe != 0;
    for (const uintptr_t at : {uintptr_t(0x5D44B0), uintptr_t(0x5E47E0), uintptr_t(0x743C60), uintptr_t(0x4087E0), uintptr_t(0x40EA10), uintptr_t(0x5E6080), uintptr_t(0x5E61F0)})
        env.watches.emplace_back(at, 16);
    json watched = json::array();
    for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), 16)}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json stubbed = json::array();
    for (const Pair* p : {&env.pCreated, &env.pInfo, &env.pRequest, &env.pLoad, &env.pGive, &env.pSetCur}) stubbed.push_back({{"exe", p->exe}, {"dll", p->dll}});
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", 1}, {"pairs", kCases}, {"cases", {{"CPedSaveStructure::Extract", kCases}}},
           {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
           {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
           {"collaborators", {{"exe_real", json::array()}, {"stubbed", stubbed}}},
           {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game}, {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR},
           {"pool_global_before", PoolGlobal()}});
    Totals totals;
    if (ready) {
        for (unsigned c = 0; c < kCases; ++c) CaseExtract(env, totals, c);
        Write({{"event", "function_summary"}, {"function", "CPedSaveStructure::Extract"}, {"cases", kCases}, {"failures", totals.failures}, {"state_restored", env.WatchesIntact()}});
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
