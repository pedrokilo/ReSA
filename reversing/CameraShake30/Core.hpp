#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco CameraShake30. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/ModifySpeed27 y reversing/CameraShake30 (Core.hpp).
//
// Banco par a par de CCamera::ProcessShake(float) (0x516560, thiscall, `ret 4`; el argumento se ignora, se lee m_fShakeIntensity +0xC90). Original = bytes del EXE con el hook RH apagado;
// candidato = DLL con el hook encendido. Unico colaborador sustituido: CHandShaker::Process (0x50D930, thiscall, 1 float, `ret 4`), con un falso en la direccion del EXE y en el cuerpo de la DLL
// que registra (indice de entrada, bits del argumento) y escribe en la entrada m_ang.z (+8) y los 9 floats de m_resultMat que lee la funcion (right, forward, up) con valores del caso.
// La tabla de CHandShaker del juego (0xB6ECA0, 6 x 0x94 bytes) y la bandera 0xB70048 se escriben antes de cada lado y se restauran al final del caso. Las llamadas a 0x59C810 / 0x59C910 /
// 0x59C730 NO se sustituyen: el EXE llama a las suyas y la DLL ejecuta FlcX87 (X87Math.cpp), y ese es precisamente el resultado que se compara (front y up en bits, todo el resto del objeto sin cambios).
// La funcion usa x87 a 24 bits (el banco lo fija con _controlfp): el estado x87 se compara normalizado (ver NormalizeFp) y los flags de MXCSR se ignoran.
namespace CameraShake30Live {
using namespace CameraShake30Fix;
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
// Aqui el EXE SI ejecuta x87 (y la DLL puede usar SSE): se compara FCW completa, TOP, etiquetas y los ST VALIDOS (ninguno debe quedar); se ignoran banderas de excepcion/condicion de FSW,
// FIP/FCS/FOP/FDP/FDS y el contenido de los registros marcados como vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Los dos llamadores del EXE (0x51A72B y 0x52BA68) no los leen y la DLL no puede reproducirlos (void):
// por defecto solo se registran en original_residue/candidate_residue.
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
// argsPtr[0] queda en la cima de la pila; EFLAGS justo tras el `call` (para comprobar DF=0).
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

// ------------------------------------------------------------------------------------------------ colaborador falso
struct Ctx {std::vector<u32> trace;u32 calls{};u32 angZ{};u32 mat[9]{};};
inline Ctx ctx;
// 0x50D930 CHandShaker::Process (thiscall: ECX = entrada, [esp+4] = grado, `ret 4`). Se declara __fastcall (ECX, EDX ficticio, resto en la pila, limpia la pila el llamado) para recibirlo igual.
// Registra el indice de la entrada dentro de la tabla (con signo) y los bits del argumento, y escribe m_ang.z y los 9 floats de la matriz. Mas de una llamada es un fallo (`traps_zero`).
inline void __fastcall FakeHandShakerProcess(u8* self,void* /*edx*/,u32 degreeBits) {
    ++ctx.calls;
    ctx.trace.push_back(u32((i32(uintptr_t(self))-i32(kTableAddress))/i32(kEntryBytes)));
    ctx.trace.push_back(degreeBits);
    memcpy(self+kEntryAngZ,&ctx.angZ,4);
    for(u32 i=0;i<9;++i)memcpy(self+kEntryMatrix+4*kMatrixWordIndex[i],&ctx.mat[i],4);
}

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,kArena> arena{};
inline constexpr uintptr_t kEntryOfCollaborator=0x50D930;

// Hooks RH de las rutinas CVector del EXE que llama el original (0x59C910 Normalise, 0x59C730 CrossProduct): el lado EXE debe ejecutar el codigo del EXE y no saltar al C++ de la DLL.
// Se apagan durante todo el banco (la DLL no las llama: usa FlcX87 en X87Math.cpp) y se restauran al final.
class HookOff {
    ReversibleHooks::ReversibleHook::Base* m_hook{};
    bool m_enabled{},m_locked{},m_restored{};
public:
    HookOff(const char* cls,const char* fn) {
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
            if(category.Name()==cls)for(auto& hook:category.Items())if(hook->Name()==fn)m_hook=hook.get();
        });
        if(m_hook){m_enabled=m_hook->Hooked();m_locked=m_hook->Locked();m_hook->LockState(false);m_hook->State(false);}
    }
    bool Found()const{return m_hook!=nullptr;}
    bool WasEnabled()const{return m_enabled;}
    bool StillOff()const{return !m_hook||!m_hook->Hooked();}
    bool Restore() {
        if(m_hook&&!m_restored){m_hook->State(m_enabled);m_hook->LockState(m_locked);}
        m_restored=true;
        return !m_hook||(m_hook->Hooked()==m_enabled&&m_hook->Locked()==m_locked);
    }
    ~HookOff(){Restore();}
};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::unique_ptr<HookRoute> route;
    uintptr_t entry{0x516560};
    uintptr_t dllCollab{};      // cuerpo de la DLL de CHandShaker::Process (la DLL lo llama directamente)
    std::vector<std::unique_ptr<HookOff>> exeHelpers;   // hooks de 0x59C910 y 0x59C730 apagados durante el banco
    std::vector<Watch> watches;
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

template<class G> Side Execute(Env& env,bool candidate,G gate) {
    Side side{};
    auto call=[&]{
        const uintptr_t entry=env.entry;
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        side.reg=gate(entry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route->Invoke(candidate,call);
    return side;
}

inline void Put32(u8* base,u32 offset,u32 value){memcpy(base+offset,&value,4);}

// Un caso: CCamera::ProcessShake(x) con `this` = objeto de la arena (CCamera completo). El EXE ignora el argumento: se pasa la intensidad del caso.
inline void CaseShake(Env& env,Totals& t,unsigned n,const std::array<u8,kTableBytes>& savedTable,u8 savedFlag) {
    const F f=MakeF(n);
    u8* const obj=arena.data()+kObjOffset;
    const u32 camBase=kOffCams+f.active*kCamStride;
    // ---- referencia
    std::array<u8,kArena> ref{};ref.fill(0xC3);
    u8* const ro=ref.data()+kObjOffset;
    FillWords(ro,kObjBytes,f.headSeed);
    ro[kOffActive]=u8(f.active);
    Put32(ro,kOffIntensity,f.intensity);Put32(ro,kOffType,f.type);
    for(u32 i=0;i<3;++i){Put32(ro,camBase+kOffFront+4*i,f.front[i]);Put32(ro,camBase+kOffUp+4*i,f.up[i]);}
    std::array<u8,kTableBytes> tableRef{};FillWords(tableRef.data(),kTableBytes,f.tableSeed);
    const json input={{"family",f.family},{"type",f.type},{"intensity",f.intensity},{"active",f.active},{"front",std::vector<u32>{f.front[0],f.front[1],f.front[2]}},
                      {"up",std::vector<u32>{f.up[0],f.up[1],f.up[2]}},{"ang_z",f.angZ},{"mat",std::vector<u32>(f.mat,f.mat+9)},{"flag",f.flag},
                      {"table_in",HexBytes(tableRef.data(),kTableBytes)},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
    u8* const table=reinterpret_cast<u8*>(uintptr_t(kTableAddress));
    u8* const flag=reinterpret_cast<u8*>(uintptr_t(kFlagAddress));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        ctx.trace.clear();ctx.calls=0;ctx.angZ=f.angZ;for(u32 i=0;i<9;++i)ctx.mat[i]=f.mat[i];
        arena=ref;memcpy(table,tableRef.data(),kTableBytes);*flag=u8(f.flag);
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            ReversingProbe::EntryPatch exe(kEntryOfCollaborator,&FakeHandShakerProcess),dll(env.dllCollab,&FakeHandShakerProcess);
            const u32 args[1]{f.intensity};
            return Gate(entry,args,1,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);
        });
        // guardas y resto del objeto (sin front y up de la camara activa)
        bool guards=memcmp(arena.data(),ref.data(),kObjOffset)==0&&memcmp(arena.data()+kObjOffset+kObjBytes,ref.data()+kObjOffset+kObjBytes,kGuard)==0;
        std::array<u8,kObjBytes> a{},b{};memcpy(a.data(),obj,kObjBytes);memcpy(b.data(),ro,kObjBytes);
        memset(a.data()+camBase+kOffFront,0,12);memset(b.data()+camBase+kOffFront,0,12);memset(a.data()+camBase+kOffUp,0,12);memset(b.data()+camBase+kOffUp,0,12);
        const bool rest=a==b;
        json observed={{"front",HexBytes(obj+camBase+kOffFront,12)},{"up",HexBytes(obj+camBase+kOffUp,12)},{"rest_ok",rest},{"guard_ok",guards},
                       {"table",HexBytes(table,kTableBytes)},{"flag",int(*flag)},{"calls",ctx.calls},{"trace",ctx.trace}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact()&&std::ranges::all_of(env.exeHelpers,[](const auto& h){return h->StillOff();});
        memcpy(table,savedTable.data(),kTableBytes);*flag=savedFlag;
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK},{"traps_zero",ctx.calls==1}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&memcmp(table,savedTable.data(),kTableBytes)==0&&*flag==savedFlag;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","traps_zero","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    for(const char* k:{"rest_ok","guard_ok"})ok&=rows[0]["observed"][k]==true&&rows[1]["observed"][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function","CCamera::ProcessShake"},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    const bool collaboratorsOK=env.dllCollab&&ModuleOf(env.dllCollab)==env.asi;
    ready&=collaboratorsOK&&body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x516560),kEntryOfCollaborator,env.dllCollab,body})if(at)env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    json exeHelpers=json::array();
    for(const auto& h:env.exeHelpers)exeHelpers.push_back({{"found",h->Found()},{"was_enabled",h->WasEnabled()}});
    std::array<u8,kTableBytes> savedTable{};memcpy(savedTable.data(),reinterpret_cast<void*>(uintptr_t(kTableAddress)),kTableBytes);
    const u8 savedFlag=*reinterpret_cast<u8*>(uintptr_t(kFlagAddress));
    ctx.trace.reserve(16);
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",env.cases},{"cases",{{"CCamera::ProcessShake",env.cases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",std::vector<uintptr_t>{kEntryOfCollaborator}},{"dll",std::vector<uintptr_t>{env.dllCollab}}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"exe_helper_hooks",exeHelpers},{"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_table",HexBytes(savedTable.data(),kTableBytes)},{"initial_flag",int(savedFlag)},
        {"layout",{{"arena",kArena},{"obj_offset",kObjOffset},{"obj_bytes",kObjBytes},{"table_bytes",kTableBytes}}}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<env.cases;++c)CaseShake(env,totals,c,savedTable,savedFlag);
        Write({{"event","function_summary"},{"function","CCamera::ProcessShake"},{"cases",env.cases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&memcmp(reinterpret_cast<void*>(uintptr_t(kTableAddress)),savedTable.data(),kTableBytes)==0&&*reinterpret_cast<u8*>(uintptr_t(kFlagAddress))==savedFlag;
    if(env.route)stateRestored&=env.route->Restore();
    for(auto& h:env.exeHelpers)stateRestored&=h->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"table",HexBytes(reinterpret_cast<void*>(uintptr_t(kTableAddress)),kTableBytes)},{"flag",int(*reinterpret_cast<u8*>(uintptr_t(kFlagAddress)))}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
