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
// Nucleo del banco Misc61 (lote 61). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego que usa LiveComparison.hpp.
// Plantilla: reversing/PartnerRemove60 (misma estructura: Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).
//
// Banco par a par de CUATRO funciones: CMemoryHeap::IntegrityCheck (0x72E8E0), CSearchLight::IsSpottedEntity (0x493900), CTaskSimpleSlideToCoord::ProcessPed (0x66C4E0, VIRTUAL)
// y CTrainNode::GetLightingFromCollision (0x6F5F80). Original = bytes del EXE con el hook RH de la funcion apagado; candidato = DLL con SOLO ese hook encendido.
// Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL, resuelto ANTES de escribir ningun parche),
// que registran sus argumentos EN ORDEN. EAX/ECX/EDX de salida solo se registran (ningun llamador del EXE los lee; el AL de bool/tColLighting si se compara).

namespace Misc61Live {
using namespace Misc61Fix;
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
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    ~Patches(){while(!p.empty())p.pop_back();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::array<std::unique_ptr<HookRoute>,4> route;                 // 0 heap, 1 spot, 2 slide, 3 node
    std::array<std::vector<StubSpec>,4> stubs;                      // por funcion
    std::vector<Watch> watches;                                     // entradas del EXE de todos los colaboradores: deben seguir siendo lo que eran al empezar
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). En ambos lados los colaboradores con hook deben ser saltos a la DLL
// (y los sin hook, codigo del EXE); los falsos se instalan DESPUES de comprobar rutas y colaboradores.
template<class Fn> Side Execute(Env& env,unsigned fnIdx,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    HookRoute& route=*env.route[fnIdx];
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const auto& s:env.stubs[fnIdx]) {
            const auto& pr=s.pair;
            collab&=(pr.hooked?(ReversingProbe::JumpsToDll(pr.exe)&&ModuleOf(Resolve(pr.exe))==env.asi):!ReversingProbe::JumpsToDll(pr.exe))
                    &&(pr.hooked?(pr.dll!=0&&ModuleOf(pr.dll)==env.asi):(pr.dll==0||ModuleOf(pr.dll)==env.asi));
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
// Cambios de un objeto de una arena respecto a su imagen de referencia: [[desplazamiento respecto al objeto, byte nuevo]] (negativo = guarda anterior).
inline json ArenaChanges(const uint8* mem,const uint8* ref,size_t bytes,size_t objOff) {
    json out=json::array();
    for(size_t i=0;i<bytes;++i)if(mem[i]!=ref[i])out.push_back(json::array({int64(i)-int64(objOff),mem[i]}));
    return out;
}
inline u32 BitsOf(const void* p) {u32 v;memcpy(&v,p,4);return v;}
inline float FloatOfBits(u32 b) {float f;memcpy(&f,&b,4);return f;}
// Evento generico de los falsos: etiqueta numerica + hasta 14 valores
struct Ev {u32 kind{};std::array<u32,14> v{};};
inline std::array<Ev,16> evs{};inline unsigned nev{};
inline Ev& PushEv(u32 kind) {Ev& e=evs[nev<evs.size()?nev:evs.size()-1];if(nev<evs.size())e=Ev{};e.kind=kind;++nev;return e;}
inline constexpr const char* kFn[4]{"CMemoryHeap::IntegrityCheck","CSearchLight::IsSpottedEntity","CTaskSimpleSlideToCoord::ProcessPed","CTrainNode::GetLightingFromCollision"};

// ================================================================================================ 0: CMemoryHeap::IntegrityCheck
inline uint8* gHeapBase{};
inline bool gGuardActive{};
inline std::array<u32,96> gTrace{};inline unsigned gTraceCount{};
inline LONG CALLBACK GuardHandler(PEXCEPTION_POINTERS p) {
    if(p->ExceptionRecord->ExceptionCode==STATUS_GUARD_PAGE_VIOLATION&&gGuardActive&&p->ExceptionRecord->NumberParameters>1) {
        const uintptr_t a=p->ExceptionRecord->ExceptionInformation[1],b=uintptr_t(gHeapBase);
        if(a>=b&&a<b+size_t(kHeapPages)*kPageBytes) {
            if(gTraceCount<gTrace.size())gTrace[gTraceCount++]=u32((a-b)/kPageBytes);
            return EXCEPTION_CONTINUE_EXECUTION;   // el sistema ya quito PAGE_GUARD de esa pagina
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
inline void BuildHeapImage(const C1& c,uint8* img,uintptr_t base) {
    const size_t total=size_t(kHeapPages)*kPageBytes;
    memset(img,0,total);
    for(u32 p=0;p<c.usedPages;++p)FillWords(img+size_t(p)*kPageBytes,kPageBytes,c.fill+p*0x101u);   // basura arbitraria en toda pagina usada
    auto put=[&](uintptr_t addr,u32 v){memcpy(img+(addr-base),&v,4);};
    auto page=[&](u32 pg,u32 off){return base+uintptr_t(pg)*kPageBytes+off;};
    const uintptr_t self=page(0,c.thisOff);
    const uintptr_t term=page(c.termPage,0);
    const uintptr_t endNode=self+0x20;
    // objeto: m_FirstBlock, m_LastBlock, m_FreeListBegin (+8..), m_FreeListEnd (+0x20..), m_paCommonSizes (+0x38)
    put(self+0,c.blkPage.empty()?u32(term):u32(page(c.blkPage[0],0)));
    put(self+4,u32(term));
    put(self+0x18,c.freePage.empty()?u32(endNode):u32(page(c.freePage[0],c.freeOff[0])));
    put(self+0x38,c.hasCs?u32(page(1,0)):0u);
    for(size_t k=0;k<c.freePage.size();++k)put(page(c.freePage[k],c.freeOff[k])+0x10,k+1<c.freePage.size()?u32(page(c.freePage[k+1],c.freeOff[k+1])):u32(endNode));
    if(c.hasCs) {
        for(u32 e=0;e<kCommonSizeCount;++e) {
            const uintptr_t ent=page(1,e*kCommonSizeBytes);
            std::vector<size_t> nodes;
            for(size_t k=0;k<c.csEntry.size();++k)if(c.csEntry[k]==e)nodes.push_back(k);
            put(ent+0x10,nodes.empty()?u32(ent+0x18):u32(page(c.csPage[nodes[0]],c.csOff[nodes[0]])));
            for(size_t j=0;j<nodes.size();++j) {
                const size_t k=nodes[j];
                put(page(c.csPage[k],c.csOff[k])+0x10,j+1<nodes.size()?u32(page(c.csPage[nodes[j+1]],c.csOff[nodes[j+1]])):u32(ent+0x18));
            }
        }
    }
    for(size_t k=0;k<c.blkPage.size();++k) {
        const uintptr_t h=page(c.blkPage[k],0);
        put(h+0,c.blkLen[k]*kPageBytes-0x10u);
        img[h-base+4]=u8(c.blkInUse[k]);
        const u16 mid=u16(c.blkMemId[k]);memcpy(img+(h-base)+6,&mid,2);
        put(h+0xC,(c.prevMode==1)?(k==0?0u:u32(page(c.blkPage[k-1],0))):0xDEAD0000u+u32(k));   // solo la cadena por los bloques sigue estos punteros
    }
    img[term-base+4]=1;{const u16 mid=0xFFFFu;memcpy(img+(term-base)+6,&mid,2);}put(term+0,0u);
    put(term+0xC,c.prevMode==0?0u:c.prevMode==1?(c.blkPage.empty()?0u:u32(page(c.blkPage.back(),0))):u32(page(c.ghostPage[0],c.ghostOff[0])));
    for(size_t j=0;j<c.ghostPage.size();++j)put(page(c.ghostPage[j],c.ghostOff[j])+0xC,j+1<c.ghostPage.size()?u32(page(c.ghostPage[j+1],c.ghostOff[j+1])):0u);
}
inline void SetGuard(bool on,const C1& c) {
    for(u32 p=0;p<c.usedPages;++p) {
        DWORD old{};
        VirtualProtect(gHeapBase+size_t(p)*kPageBytes,kPageBytes,on?(PAGE_READWRITE|PAGE_GUARD):PAGE_READWRITE,&old);
    }
}
inline void CaseHeap(Env& env,Totals& t,unsigned n) {
    const C1 c=MakeC1(n);
    static std::vector<uint8> ref(size_t(kHeapPages)*kPageBytes);
    BuildHeapImage(c,ref.data(),uintptr_t(gHeapBase));
    const uintptr_t self=uintptr_t(gHeapBase)+c.thisOff;
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        memcpy(gHeapBase,ref.data(),ref.size());
        gTraceCount=0;
        SetGuard(true,c);gGuardActive=true;
        sides[s]=Execute(env,0,0x72E8E0,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(self),c.edx,c.stack,0);});
        gGuardActive=false;SetGuard(false,c);
        json obs;
        json tr=json::array();for(unsigned i=0;i<gTraceCount;++i)tr.push_back(gTrace[i]);
        obs["trace"]=tr;
        obs["arena_unchanged"]=memcmp(gHeapBase,ref.data(),ref.size())==0;
        obs["completed"]=true;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn[0],n,input,rows,sides);
}
}
