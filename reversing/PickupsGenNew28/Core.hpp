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

// Nucleo del banco PickupsGenNew28. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/FileLoaderLoad2dEffect27 (Core.hpp), reversing/DecisionAdd27.
//
// Banco par a par de UNA funcion: CPickups::GenerateNewOne (0x456F20, cdecl, 9 dwords de argumentos, `ret` normal; el handle vuelve en EAX). El hook RH se llama "GenerateNewOne" y apunta
// a CPickups::GenerateNewOne_ExeAbi (devuelve int32; isEmpty como uint8). Original = bytes del EXE con el hook apagado; candidato = DLL con SOLO ese hook encendido.
// Estado global: la tabla CPickups::aPickUps (0x9788C0, 620 x 0x20) se SUSTITUYE por la del caso en cada lado (se guarda la real y se restaura al final) y CTimer::m_snTimeInMilliseconds
// (0xB7CB84) se fija por caso y se restaura. En ambos lados se sustituyen, SOLO durante la llamada y con el MISMO falso (parche de entrada en la direccion del EXE Y en el cuerpo de la DLL,
// calculado ANTES de parchear): CPickup::FindTextIndexForString (0x455500), CPickup::IsVisible (0x454C70), CPickup::GiveUsAPickUpObject (0x4567E0), CWorld::Remove (0x563280),
// CWorld::Add (0x563220); el destructor virtual del objeto lo atiende una vtbl falsa de los objetos falsos del banco. Cada falso anota (tipo, argumentos, primera ranura distinta de la
// inicial y sus 32 bytes), asi que el orden de las escrituras respecto de cada llamada queda comparado. NO se sustituye (corre de verdad): _ftol2 0x821B40 del EXE (la DLL convierte con su
// CRT). No hay rand ni sscanf. La palabra de control x87 se fija a 24 bits, al mas cercano y con TODAS las excepciones enmascaradas (estado del juego).
// EAX es el valor de retorno y se compara; ECX/EDX de salida: solo se registran (ningun llamador del EXE los lee, ver README.md).
namespace PickupsGen28Live {
using namespace PickupsGen28Fix;
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
// La funcion usa x87 (_ftol2 del EXE y conversion float->int64 de la DLL): los registros ST vacios guardan restos distintos, la palabra de estado lleva
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
inline u8* Live() {return reinterpret_cast<u8*>(uintptr_t(kTableAddress));}
inline uint32* NowPtr() {return reinterpret_cast<uint32*>(uintptr_t(kTimeAddress));}
alignas(16) inline std::array<u8,kTableBytes> refTable{};      // tabla inicial del caso (con punteros reales a los objetos falsos)
alignas(16) inline std::array<u8,kTableBytes> savedTable{};    // tabla real del juego, restaurada al final
inline uint32 savedNow{};
alignas(16) inline std::array<std::array<u8,0x40>,kNumObjs> objs{};
alignas(16) inline std::array<std::array<u8,0x40>,kNumObjs> objsRef{};
alignas(16) inline std::array<char,kMsgArena> msgArena{};
alignas(16) inline std::array<char,kMsgArena> msgRef{};
inline void* fakeVtbl[2]{};

inline u32 Lbl(u32 p) {   // puntero -> etiqueta (0, objeto falso k -> kLabelBase+k, otro valor: crudo)
    const uintptr_t b=uintptr_t(objs.data());
    if(p>=b&&p<b+sizeof(objs))return kLabelBase+1+u32((p-b)/0x40);
    return p;
}
inline u32 Ptr(u32 label) {   // etiqueta -> puntero real
    if(label>kLabelBase&&label<=kLabelBase+kNumObjs)return u32(uintptr_t(objs.data())+(label-kLabelBase-1)*0x40);
    return label;
}
inline void Xlate(u8* slot) {u32 p{};std::memcpy(&p,slot+4,4);p=Lbl(p);std::memcpy(slot+4,&p,4);}

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
struct Ev {u32 kind,a,b,c,snapIdx;std::array<u8,32> snap;};
inline std::array<Ev,16> evs{};inline unsigned nev{};
inline u32 textRet{},visibleRet{};inline void* giveObjPtr{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0) {
    if(nev>=evs.size())return;
    Ev e{k,a,b,c,0xFFFFu,{}};
    const u8* live=Live();
    for(u32 i=0;i<kSlots;++i)if(std::memcmp(live+i*kSlotBytes,refTable.data()+i*kSlotBytes,kSlotBytes)!=0){e.snapIdx=i;std::memcpy(e.snap.data(),live+i*kSlotBytes,kSlotBytes);Xlate(e.snap.data());break;}
    evs[nev++]=e;
}
inline u32 SlotOf(const void* p) {const uintptr_t d=uintptr_t(p)-kTableAddress;return d<kTableBytes&&d%kSlotBytes==0?u32(d/kSlotBytes):0xFFFFFFFFu;}

// Falsos. Remove/Add/FindText: cdecl. IsVisible: thiscall (ECX = ranura), devuelve AL (el resto de EAX lleva basura). GiveUsAPickUpObject: thiscall con (CObject*& obj, int32 slot), `ret 8`.
// Destructor virtual: thiscall con un argumento (flag), `ret 4`.
inline void __cdecl FakeRemove(void* obj){Push(1,Lbl(u32(uintptr_t(obj))));}
inline void __cdecl FakeAdd(void* obj){Push(6,Lbl(u32(uintptr_t(obj))));}
inline void __fastcall FakeDtor(void* self,void*,u32 flag){Push(2,Lbl(u32(uintptr_t(self))),flag);}
inline u32 __cdecl FakeText(const char* msg){Push(3,msg?1u:0u,msg?Fnv32(msg):0u);return textRet;}
inline u32 __fastcall FakeVisible(void* self,void*){Push(4,SlotOf(self));return visibleRet;}
inline void __fastcall FakeGive(void* self,void*,void** out,i32 slot){Push(5,SlotOf(self),u32(uintptr_t(out)-uintptr_t(self)),u32(slot));*out=giveObjPtr;}
inline void ResetObjs() {
    fakeVtbl[0]=reinterpret_cast<void*>(&FakeDtor);fakeVtbl[1]=nullptr;
    for(unsigned k=0;k<kNumObjs;++k){objs[k].fill(u8(0xC0+k));const u32 v=u32(uintptr_t(&fakeVtbl[0]));std::memcpy(objs[k].data(),&v,4);}
    objsRef=objs;
}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x456F20, 0x821B40 y las cinco entradas parcheadas: deben seguir siendo el codigo original al volver
    Pair pText{},pVis{},pGive{},pRemove{},pAdd{};   // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,24> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){
    p.Add(e.pText,&FakeText);p.Add(e.pVis,&FakeVisible);p.Add(e.pGive,&FakeGive);p.Add(e.pRemove,&FakeRemove);p.Add(e.pAdd,&FakeAdd);
}

// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). `exeOnly`: direcciones del EXE que no pueden saltar a la DLL
// (_ftol2 del EXE: se ejecuta de verdad). Los parches de los colaboradores se instalan despues de comprobar la ruta y se quitan antes de volver.
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
inline json ChangedJson() {
    json out=json::array();
    const u8* live=Live();
    for(u32 i=0;i<kSlots;++i)if(std::memcmp(live+i*kSlotBytes,refTable.data()+i*kSlotBytes,kSlotBytes)!=0){
        std::array<u8,kSlotBytes> tmp{};std::memcpy(tmp.data(),live+i*kSlotBytes,kSlotBytes);Xlate(tmp.data());
        out.push_back(json::array({i,HexBytes(tmp.data(),kSlotBytes)}));
    }
    return out;
}
inline void BuildReference(const Fx& f) {
    for(u32 i=0;i<kSlots;++i){u8* s=refTable.data()+i*kSlotBytes;std::memset(s,0xA5,kSlotBytes);s[0x1C]=u8(f.fillerType);}
    for(const auto& sl:f.slots){
        u8* s=refTable.data()+sl.index*kSlotBytes;
        std::memcpy(s,sl.bytes.data(),kSlotBytes);
        u32 label{};std::memcpy(&label,s+4,4);const u32 p=Ptr(label);std::memcpy(s+4,&p,4);   // etiqueta -> puntero real
    }
    msgRef.fill(0);
    if(!f.msgNull)std::memcpy(msgRef.data(),f.msg.data(),f.msg.size());
}
inline void CaseGenerate(Env& env, Totals& t, unsigned n) {
    const Fx f = Make(n);
    const json input = json::parse(InputLine(f));
    ResetObjs();
    BuildReference(f);
    Side sides[2];
    json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        std::memcpy(Live(), refTable.data(), kTableBytes);
        *NowPtr() = f.now;
        ResetObjs();
        msgArena = msgRef;
        nev = 0; textRet = f.textRet; visibleRet = f.visibleRet; giveObjPtr = reinterpret_cast<void*>(uintptr_t(Ptr(f.giveObj)));
        sides[s] = Execute(env, *env.route, 0x456F20, {uintptr_t(0x821B40)}, candidate, [&](uintptr_t entry) {
            const u32 args[9]{f.x, f.y, f.z, f.model, f.ptype, f.ammo, f.money, f.empty, f.msgNull ? 0u : u32(uintptr_t(msgArena.data()))};
            return Gate(entry, args, 9, f.eax, f.ecx, f.edx, f.stack, 0x24);       // cdecl: el llamador retira los 9 dwords
        });
        json trace = json::array();
        for (unsigned i = 0; i < nev; ++i) trace.push_back(json::array({evs[i].kind, evs[i].a, evs[i].b, evs[i].c, evs[i].snapIdx, HexBytes(evs[i].snap.data(), 32)}));
        json observed = {{"ret", sides[s].reg.eax}, {"changed", ChangedJson()}, {"trace", trace}, {"time_ok", *NowPtr() == f.now}, {"msg_ok", msgArena == msgRef}, {"objs_ok", objs == objsRef}};
        auto fp = NormalizeFp(sides[s].fp);
        const bool watchesIntact = env.WatchesIntact();
        json out = CommonRow(sides[s], fp);
        out["observed"] = observed;
        RestoreFloatingState();
        out["globals_restored"] = watchesIntact && env.WatchesIntact();
        out["floating_state_restored"] = FloatingStateRestored();
        rows[s] = out;
    }
    Finish(env, t, "CPickups::GenerateNewOne", n, input, rows, sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env, bool ready, const json& registered, const json& bodies) {
    const uintptr_t body = env.route ? env.route->Body() : 0;
    ready &= body && ModuleOf(body) == env.asi;
    for (const uintptr_t at : {uintptr_t(0x456F20), uintptr_t(0x821B40), uintptr_t(0x455500), uintptr_t(0x454C70), uintptr_t(0x4567E0), uintptr_t(0x563280), uintptr_t(0x563220)})
        env.watches.emplace_back(at, 16);
    json watched = json::array();
    for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), 16)}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    std::memcpy(savedTable.data(), Live(), kTableBytes); savedNow = *NowPtr();
    PrepareFloatingState();
    RestoreFloatingState();
    json stubbed = json::array();
    for (const Pair* p : {&env.pText, &env.pVis, &env.pGive, &env.pRemove, &env.pAdd}) stubbed.push_back({{"exe", p->exe}, {"dll", p->dll}});
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", 1}, {"pairs", kCases}, {"cases", {{"CPickups::GenerateNewOne", kCases}}},
           {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
           {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
           {"collaborators", {{"exe_real", std::vector<uintptr_t>{0x821B40}}, {"stubbed", stubbed}}}, {"slots", kSlots}, {"table_address", kTableAddress},
           {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game}, {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned c = 0; c < kCases; ++c) CaseGenerate(env, totals, c);
        Write({{"event", "function_summary"}, {"function", "CPickups::GenerateNewOne"}, {"cases", kCases}, {"failures", totals.failures}, {"state_restored", env.WatchesIntact()}});
    }
    std::memcpy(Live(), savedTable.data(), kTableBytes); *NowPtr() = savedNow;      // la tabla y la hora del juego vuelven a como estaban
    const bool tableRestored = std::memcmp(Live(), savedTable.data(), kTableBytes) == 0 && *NowPtr() == savedNow;
    const bool entriesIntact = env.WatchesIntact();
    bool stateRestored = entriesIntact && tableRestored;
    if (env.route) stateRestored &= env.route->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}, {"table_restored", tableRestored}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}
}
