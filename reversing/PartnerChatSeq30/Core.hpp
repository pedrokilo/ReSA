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
// Nucleo del banco PartnerChatSeq30: CTaskComplexPartnerChat::GetPartnerSequence (0x684380). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (CTask, CTaskPool/GetTaskPool, CReferences, CReference, CEntity, CTaskComplexPartnerChat, CPedIntelligence).
// Plantillas: reversing/PartnerShoveSeq28 (pool sintetico, referencias, hook Virtual) y reversing/PedSaveExtract28 (falsos con parche de entrada en el EXE y en la DLL).
//
// Banco par a par de UNA funcion, CTaskComplexPartnerChat::GetPartnerSequence (0x684380, hook virtual: HookRoute con el cuerpo del hook Virtual).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Colaboradores REALES (reconstruidos en C++ y enganchados, se ejecutan en ambos lados): operator new
// 0x61A5A0, constructores 0x632BD0 / 0x66B890 / 0x682CB0 / 0x62F310 / 0x61ABE0 (el de CTaskSimpleChat 0x5F65A0 sigue en el EXE y llama a 0x61ABE0), AddTask 0x632D10, RegisterReference 0x571B70.
// SUSTITUIDO por un falso identico en ambos lados (parche de entrada en 0x600EE0 y en el cuerpo de la DLL): CPedIntelligence::FindTaskByType (devuelve una tarea falsa cuyo byte +0x74 fija el caso).
// El pool de tareas se sustituye por un pool SINTETICO por caso (estados y m_LastFreeSlot) y se restauran los estados, el cursor, todo el almacenamiento, la lista de referencias libres y
// la region de s_Conversation que el caso reescribe (0xB613E0..0xB61404). Las ranuras de la ventana se rellenan con 0xCD para que los bytes que los constructores no escriben sean iguales en ambos lados.

namespace PartnerChatSeq30Live {
using namespace PartnerChatSeq30Fix;
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
// La funcion no usa x87 (empuja 4.0f como entero), pero sus colaboradores (constructores) pueden: se compara lo observable (FCW completa, TOP, etiquetas y ST validos).
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. EAX es el retorno (se compara siempre, como etiqueta, en `observed.ret`); ECX/EDX no los lee ningun llamador
// (CTaskComplexPartner::CreateNextSubTask 0x683B46 los sobrescribe: `mov ecx,[esp+30h]` en 0x6840AC), por defecto solo se registran.
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada (el EXE instala un marco SEH en 0x683120..0x683228 y lo retira).
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Igual que DecisionAdd27::Gate (sin argumentos: la funcion es thiscall sin parametros y `ret`): siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; captura los registros y EFLAGS justo tras el `call`.
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
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};    // CTaskComplexPartnerChat falso (se compara ENTERO; la funcion solo puede cambiar +0x5B y +0x74)
alignas(16) inline std::array<u8,kEntArena> entArena{},entRef{};       // partner falso (CPed hasta +0x480; solo cambia m_pReferences)
alignas(16) inline std::array<u8,kIntArena> intArena{},intRef{};       // CPedIntelligence falsa (solo es el `this` del falso de FindTaskByType)
alignas(16) inline std::array<u8,kOtherArena> otherArena{},otherRef{}; // la tarea que devuelve el falso de FindTaskByType (byte +0x74)
alignas(16) inline std::array<u8,kNodeArena> nodeArena{},nodeRef{};    // hasta 3 CReference falsas
// Mundo real que se restaura tras cada ejecucion
inline std::vector<uint8> realStates,realStorage,poolPre,realRefs;
inline int32 realLast{};
inline CReference* realHead{};
inline bool realDealWithNoMemory{};
inline std::array<u8,kConvSaveBytes> realConv{},convImg{};

// Pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* PoolHdr() {return reinterpret_cast<uint8*>(GetTaskPool());}
inline uint8* PoolStorage() {return *reinterpret_cast<uint8**>(PoolHdr());}
inline uint8* PoolStates() {return *reinterpret_cast<uint8**>(PoolHdr()+4);}
inline int32 PoolCap() {return *reinterpret_cast<int32*>(PoolHdr()+8);}
inline int32& PoolLast() {return *reinterpret_cast<int32*>(PoolHdr()+0xC);}
static_assert(sizeof(CTaskSimpleSlideToCoord)==kSlot,"el almacenamiento de una ranura del pool de tareas es de 0x80 bytes");
static_assert(sizeof(CReference)==kNodeSize,"CReference mide 8");
constexpr size_t kRefsBytes=sizeof(CReferences::aRefs);

// Region de s_Conversation que el banco reescribe (0xB613E0..0xB61404): se guarda una vez y se restaura tras cada lado
inline uint8* ConvRegion() {return reinterpret_cast<uint8*>(uintptr_t(kConvSaveAt));}
inline void ApplyConv(const C& c) {
    std::memcpy(ConvRegion(),realConv.data(),kConvSaveBytes);
    if(c.convEff)for(const auto& w:ConvWrites(c)) {
        const u16 v=u16(w.value);
        std::memcpy(reinterpret_cast<void*>(intptr_t(kConvBase)+intptr_t(w.idx)*2),&v,2);
    }
    std::memcpy(convImg.data(),ConvRegion(),kConvSaveBytes);
}
inline bool ConvIntact() {return std::memcmp(ConvRegion(),convImg.data(),kConvSaveBytes)==0;}
inline void RestoreConv() {std::memcpy(ConvRegion(),realConv.data(),kConvSaveBytes);}
inline bool ConvRestored() {return std::memcmp(ConvRegion(),realConv.data(),kConvSaveBytes)==0;}

// Etiquetas para punteros: el pool, las arenas y aRefs cambian de direccion entre ejecuciones, no de papel
struct Ctx {uintptr_t thisObj{},ent{},intl{},other{},node{},refs{};std::array<uintptr_t,kWindowMax> win{};size_t winN{};};
inline Ctx cx;
inline json Name(uintptr_t v) {
    if(v>=cx.thisObj&&v<cx.thisObj+kThisBytes)return "T+"+std::to_string(v-cx.thisObj);
    if(v>=cx.ent&&v<cx.ent+kEntBytes)return "E+"+std::to_string(v-cx.ent);
    if(v>=cx.intl&&v<cx.intl+kIntBytes)return "I+"+std::to_string(v-cx.intl);
    if(v>=cx.other&&v<cx.other+kOtherBytes)return "O+"+std::to_string(v-cx.other);
    if(v>=cx.node&&v<cx.node+3*kNodeSize){const auto o=v-cx.node;return "N"+std::to_string(o/kNodeSize)+(o%kNodeSize?"+"+std::to_string(o%kNodeSize):std::string());}
    for(size_t k=0;k<cx.winN;++k)if(v>=cx.win[k]&&v<cx.win[k]+kSlot)return "W"+std::to_string(k)+"+"+std::to_string(v-cx.win[k]);
    if(v>=cx.refs&&v<cx.refs+kRefsBytes){const auto o=v-cx.refs;return "R"+std::to_string(o/kNodeSize)+(o%kNodeSize?"+"+std::to_string(o%kNodeSize):std::string());}
    return uint32(v);
}

// ---- falso de CPedIntelligence::FindTaskByType (0x600EE0: thiscall, 1 argumento de pila, `ret 4`): registra (this, tipo) y devuelve la tarea falsa
struct FindEv {u32 self,type;};
inline std::array<FindEv,4> finds{};inline unsigned nfind{};inline u32 gOther{};
inline u32 __fastcall FakeFind(void* self,void*,u32 type){if(nfind<finds.size())finds[nfind++]=FindEv{u32(uintptr_t(self)),type};return gOther;}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;   // colaboradores y 0x5F65A0 / 0x600EE0: deben seguir siendo el codigo que eran al empezar
    Pair pFind{};                 // cuerpo de la DLL de FindTaskByType resuelto ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,4> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}
// Colaboradores reales que deben ser saltos a la DLL en AMBOS lados (0x5F65A0, el constructor de CTaskSimpleChat, sigue siendo codigo del EXE; solo se vigila)
inline constexpr uintptr_t kCollaborators[]{0x61A5A0,0x632BD0,0x632D10,0x66B890,0x682CB0,0x62F310,0x571B70,0x61ABE0};
inline constexpr uintptr_t kWatchedOnly[]{0x5F65A0,0x600EE0};

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). En ambos lados los colaboradores reales deben ser saltos a la DLL;
// el falso de FindTaskByType se instala DESPUES de comprobar rutas y colaboradores.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;for(const uintptr_t a:kCollaborators)collab&=ReversingProbe::JumpsToDll(a)&&ModuleOf(Resolve(a))==env.asi;
        side.collabOK=collab;
        {
            Patches patches;
            patches.Add(env.pFind,&FakeFind);
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
inline json Finish(Env& env,Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}

// ------------------------------------------------------------------------------------------------ CTaskComplexPartnerChat::GetPartnerSequence
inline void RestoreWorld(size_t cap) {
    std::memcpy(PoolStates(),realStates.data(),cap);
    PoolLast()=realLast;
    std::memcpy(PoolStorage(),realStorage.data(),cap*kSlot);
    std::memcpy(&CReferences::aRefs[0],realRefs.data(),kRefsBytes);
    CReferences::pEmptyList=realHead;
    RestoreConv();
}
inline bool WorldRestored(size_t cap) {
    return std::memcmp(PoolStates(),realStates.data(),cap)==0&&PoolLast()==realLast&&std::memcmp(PoolStorage(),realStorage.data(),cap*kSlot)==0
        &&std::memcmp(&CReferences::aRefs[0],realRefs.data(),kRefsBytes)==0&&CReferences::pEmptyList==realHead&&ConvRestored();
}
inline int RefIndex(const CReference* r) {return r?int(r-&CReferences::aRefs[0]):-1;}
inline json SerTask(uintptr_t addr) {
    const auto* w=reinterpret_cast<const u32*>(addr);
    json words=json::array();words.push_back(nullptr);   // la vtable se sustituye por GetTaskType
    for(u32 k=1;k<kSlot/4;++k)words.push_back(Name(w[k]));
    return {{"type",int(reinterpret_cast<const CTask*>(addr)->GetTaskType())},{"words",words}};
}

inline void CaseChat(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const u32 cap=u32(PoolCap());
    const Pool ps=MakePool(c,cap,n);
    const size_t refsOff=[]{auto* e=reinterpret_cast<CEntity*>(entRef.data()+kEntOff);return size_t(reinterpret_cast<u8*>(&e->m_pReferences)-reinterpret_cast<u8*>(e));}();
    std::array<int,4> chain{};
    {const CReference* r=CReferences::pEmptyList;for(auto& v:chain){v=RefIndex(r);r=r?r->m_pNext:nullptr;}}

    // direcciones para las etiquetas
    u8* const self=thisArena.data()+kThisOff;u8* const ent=entArena.data()+kEntOff;u8* const nodes=nodeArena.data()+kNodeOff;u8* const intl=intArena.data()+kIntOff;u8* const other=otherArena.data()+kOtherOff;
    cx.thisObj=uintptr_t(self);cx.ent=uintptr_t(ent);cx.node=uintptr_t(nodes);cx.intl=uintptr_t(intl);cx.other=uintptr_t(other);cx.refs=uintptr_t(&CReferences::aRefs[0]);
    cx.winN=ps.win.size();for(size_t k=0;k<ps.win.size();++k)cx.win[k]=uintptr_t(PoolStorage()+size_t(ps.win[k].idx)*kSlot);
    gOther=u32(uintptr_t(other));

    // imagenes de referencia de las arenas
    FillWords(thisRef.data(),kThisArena,Rand(n,50));
    FillWords(entRef.data(),kEntArena,Rand(n,51));
    FillWords(nodeRef.data(),kNodeArena,Rand(n,52));
    FillWords(intRef.data(),kIntArena,Rand(n,53));
    FillWords(otherRef.data(),kOtherArena,Rand(n,54));
    thisRef[kThisOff+kLeadAt]=u8(c.lead);thisRef[kThisOff+kNAt]=u8(c.nByte);thisRef[kThisOff+kConvAt]=u8(c.conv);
    std::memcpy(thisRef.data()+kThisOff+kPointAt,c.pt,12);
    otherRef[kOtherOff+kConvAt]=u8(c.otherConv);
    {   // partner falso
        auto* e=reinterpret_cast<CEntity*>(entRef.data()+kEntOff);
        e->SetType(static_cast<eEntityType>(c.etype));e->m_bIsTempBuilding=c.temp!=0;e->m_bIsProcObject=c.proc!=0;e->SetIplIndex(u8(c.ipl));
        e->m_pReferences=nullptr;
        const u32 intPtr=u32(uintptr_t(intl));
        std::memcpy(entRef.data()+kEntOff+kIntAt,&intPtr,4);
    }
    json nodePp=json::array();
    {   // referencias previas del partner: nodos falsos encadenados
        auto* nd=reinterpret_cast<CReference*>(nodeRef.data()+kNodeOff);
        const u32 dupIdx=c.refsLen?Rand(n,40)%c.refsLen:0;
        for(u32 j=0;j<3;++j) {
            nd[j].m_pNext=j+1<c.refsLen?reinterpret_cast<CReference*>(nodes+(j+1)*kNodeSize):nullptr;
            nd[j].m_ppEntity=reinterpret_cast<CEntity**>(0xA5A50000u+j*0x10u+(Rand(n,41+j)&0xFu));
        }
        // dupKind 1: la referencia ya apunta al campo del Turn (ranura 1, +0xC); dupKind 2: al campo de la primera CTaskComplexChat (ranura 2, +0x10)
        if(c.dupKind==1&&c.refsLen>0&&ps.win.size()>=2)nd[dupIdx].m_ppEntity=reinterpret_cast<CEntity**>(cx.win[1]+0xC);
        if(c.dupKind==2&&c.refsLen>0&&ps.win.size()>=3)nd[dupIdx].m_ppEntity=reinterpret_cast<CEntity**>(cx.win[2]+0x10);
        for(u32 j=0;j<c.refsLen;++j)nodePp.push_back(Name(uintptr_t(nd[j].m_ppEntity)));
        if(c.hasPartner&&c.refsLen)reinterpret_cast<CEntity*>(entRef.data()+kEntOff)->m_pReferences=reinterpret_cast<CReference*>(nodes);
    }
    const uintptr_t partnerPtr=c.hasPartner?uintptr_t(ent):0;
    std::memcpy(thisRef.data()+kThisOff+kPartnerAt,&partnerPtr,4);

    json win=json::array();for(const auto& w:ps.win)win.push_back({w.idx,w.state});
    json convWrites=json::array();if(c.convEff)for(const auto& w:ConvWrites(c))convWrites.push_back({w.idx,w.value});
    const int nSigned=int(int32(c.nByte<<24)>>24);
    const json input={{"family",c.family},{"lead",c.lead},{"has_partner",c.hasPartner},{"etype",c.etype},{"temp",c.temp},{"proc",c.proc},{"ipl",c.ipl},{"refs_len",c.refsLen},
        {"dup_kind",c.dupKind},{"pool_mode",c.poolMode},{"pool_k",c.poolK},{"conv",c.conv},{"other_conv",c.otherConv},{"conv_eff",c.convEff},{"n",nSigned},
        {"pt",json::array({c.pt[0],c.pt[1],c.pt[2]})},{"conv_writes",convWrites},
        {"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack},{"refs_off",refsOff},{"this_off",kThisOff},
        {"pool",{{"cap",cap},{"last",ps.last},{"free_total",ps.freeTotal},{"win",win}}},{"refs",{{"chain",chain}}},{"node_pp",nodePp}};

    // almacenamiento previo "visto" por la funcion: el real con las ranuras de la ventana a 0xCD
    poolPre=realStorage;
    for(const auto& w:ps.win)std::memset(poolPre.data()+size_t(w.idx)*kSlot,0xCD,kSlot);

    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(PoolStates(),ps.states.data(),cap);PoolLast()=int32(ps.last);
        std::memcpy(PoolStorage(),poolPre.data(),size_t(cap)*kSlot);
        thisArena=thisRef;entArena=entRef;nodeArena=nodeRef;intArena=intRef;otherArena=otherRef;nfind=0;
        ApplyConv(c);
        sides[s]=Execute(env,*env.route,0x684380,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(uintptr_t(self)),c.edx,c.stack,0);});

        // ---- observacion
        json obs;
        obs["ret"]=Name(sides[s].reg.eax);
        obs["last_after"]=PoolLast();
        json changes=json::array();std::vector<bool> allocated(ps.win.size());
        for(u32 i=0;i<cap;++i)if(PoolStates()[i]!=ps.states[i])changes.push_back({i,PoolStates()[i]});
        for(size_t k=0;k<ps.win.size();++k)allocated[k]=PoolStates()[ps.win[k].idx]!=ps.states[ps.win[k].idx];
        obs["state_changes"]=changes;
        json tasks=json::array();
        for(size_t k=0;k<ps.win.size();++k)if(allocated[k]) {json tk=SerTask(cx.win[k]);tk["slot"]=k;tasks.push_back(tk);}
        obs["tasks"]=tasks;
        long firstOutside=-1;
        for(u32 j=0;j<cap&&firstOutside<0;++j) {
            bool isAllocated=false;for(size_t k=0;k<ps.win.size();++k)if(ps.win[k].idx==j&&allocated[k])isAllocated=true;
            if(!isAllocated&&std::memcmp(PoolStorage()+size_t(j)*kSlot,poolPre.data()+size_t(j)*kSlot,kSlot)!=0)firstOutside=long(j);
        }
        obs["pool_outside_ok"]=firstOutside<0;obs["first_pool_outside"]=firstOutside;
        json thisChanges=json::array();
        for(u32 i=0;i<kThisArena;++i)if(thisArena[i]!=thisRef[i])thisChanges.push_back({int(i)-int(kThisOff),thisArena[i]});
        obs["this_changes"]=thisChanges;
        {   // el partner solo puede cambiar su lista de referencias
            auto tmp=entArena;std::memcpy(tmp.data()+kEntOff+refsOff,entRef.data()+kEntOff+refsOff,4);
            obs["partner_other_ok"]=tmp==entRef;
        }
        obs["int_ok"]=intArena==intRef;
        obs["other_ok"]=otherArena==otherRef;
        obs["nodes_ok"]=nodeArena==nodeRef;
        obs["conv_ok"]=ConvIntact();
        json fj=json::array();for(unsigned i=0;i<nfind;++i)fj.push_back({Name(finds[i].self),finds[i].type});
        obs["finds"]=fj;
        json pchain=json::array();
        if(c.hasPartner) {
            uintptr_t cur=uintptr_t(reinterpret_cast<CEntity*>(ent)->m_pReferences);
            for(int step=0;step<6&&cur;++step) {
                const bool inRefs=cur>=cx.refs&&cur+kNodeSize<=cx.refs+kRefsBytes,inNodes=cur>=cx.node&&cur+kNodeSize<=cx.node+3*kNodeSize;
                if(!inRefs&&!inNodes){pchain.push_back(Name(cur));break;}
                const auto* nd=reinterpret_cast<const CReference*>(cur);
                pchain.push_back({Name(cur),Name(uintptr_t(nd->m_ppEntity))});
                cur=uintptr_t(nd->m_pNext);
            }
        }
        obs["partner_refs"]=pchain;
        obs["empty_head"]=Name(uintptr_t(CReferences::pEmptyList));
        json refChanges=json::array();
        for(size_t i=0;i<kRefsBytes/kNodeSize;++i)
            if(std::memcmp(&CReferences::aRefs[i],realRefs.data()+i*kNodeSize,kNodeSize)!=0)refChanges.push_back({i,Name(uintptr_t(CReferences::aRefs[i].m_pNext)),Name(uintptr_t(CReferences::aRefs[i].m_ppEntity))});
        obs["ref_changes"]=refChanges;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        RestoreWorld(cap);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&WorldRestored(cap);
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CTaskComplexPartnerChat::GetPartnerSequence",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:kCollaborators)env.watches.emplace_back(at,16);
    for(const uintptr_t at:kWatchedOnly)env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    const size_t cap=size_t(PoolCap());
    const int freeRefs=[]{int k=0;for(auto* r=CReferences::pEmptyList;r&&k<100000;r=r->m_pNext)++k;return k;}();
    ready&=cap>=256&&CReferences::pEmptyList!=nullptr&&freeRefs>=8&&env.pFind.dll!=0;
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskComplexPartnerChat::GetPartnerSequence",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"dll_real",std::vector<uintptr_t>(std::begin(kCollaborators),std::end(kCollaborators))},{"watched_only",std::vector<uintptr_t>(std::begin(kWatchedOnly),std::end(kWatchedOnly))},
                          {"stubbed",json::array({json{{"exe",env.pFind.exe},{"dll",env.pFind.dll}}})}}},
        {"pool_capacity",cap},{"free_references",freeRefs},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        realStates.assign(PoolStates(),PoolStates()+cap);
        realStorage.assign(PoolStorage(),PoolStorage()+cap*kSlot);
        realRefs.assign(reinterpret_cast<uint8*>(&CReferences::aRefs[0]),reinterpret_cast<uint8*>(&CReferences::aRefs[0])+kRefsBytes);
        realLast=PoolLast();realHead=CReferences::pEmptyList;
        std::memcpy(realConv.data(),ConvRegion(),kConvSaveBytes);
        realDealWithNoMemory=GetTaskPool()->CanDealWithNoMemory();
        GetTaskPool()->SetDealWithNoMemory(true);   // los casos con 1 a 3 ranuras libres agotan el pool: sin esto la DLL se detendria en el punto de interrupcion de depuracion
        for(unsigned c=0;c<kCases;++c)CaseChat(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskComplexPartnerChat::GetPartnerSequence"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
        GetTaskPool()->SetDealWithNoMemory(realDealWithNoMemory);
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    if(ready)stateRestored&=WorldRestored(cap);
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
