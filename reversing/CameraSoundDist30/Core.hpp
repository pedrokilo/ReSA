#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco CameraSoundDist30. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/ModifySpeed27 (Core.hpp).
//
// Banco par a par de CCamera::UpdateSoundDistances (0x515BD0, thiscall, sin argumentos). Original = bytes del EXE con el hook RH apagado; candidato = DLL con el hook encendido.
// Unico colaborador: CWorld::ProcessVerticalLine (0x5674E0, cdecl, 11 dwords en la pila, resultado en AL; solo se llama cada 12 fotogramas). Se sustituye por un falso, en la
// direccion del EXE y en el cuerpo de la DLL, que registra sus argumentos (origen, distancia = z + 20, banderas de capas, puntero de poligono), escribe la z del punto de choque
// del caso en colPoint y devuelve 0/1 con 24 bits altos de EAX de relleno. Se compara el objeto CCamera completo (salvo +0x158..+0x163, que se comparan aparte contra el oraculo),
// la entidad objetivo falsa, la matriz falsa, las guardas y el contador de fotogramas global (CTimer::m_FrameCounter, 0xB7CB4C).
// La funcion usa x87 a 24 bits (el banco lo fija con _controlfp): el estado x87 se compara normalizado (ver NormalizeFp) y los flags de MXCSR se ignoran.
namespace CameraSoundDist30Live {
using namespace CameraSoundDist30Fix;
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
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Los dos llamadores del EXE (0x52C9B6 y el de Process) no los leen y la DLL no puede reproducirlos (void):
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
struct Ctx {std::vector<u32> trace;u32 calls{};u32 hit{};u32 hitZ{};u32 retHigh{};};
inline Ctx ctx;
// 0x5674E0 CWorld::ProcessVerticalLine (cdecl, 11 dwords): registra origen (3 bits), distancia (bits), buildings, vehicles, peds, objects, dummies, doSeeThroughCheck (solo el byte bajo: es un bool)
// y el puntero de poligono (debe ser 0). Escribe en colPoint (x, y, z): dos palabras fijas y la z del caso. Devuelve AL = hit con los 24 bits altos de EAX del caso.
// El EXE y la DLL (referencias = punteros) llaman igual. Mas de una llamada por UpdateSoundDistances es un fallo (el oraculo espera 0 o 1).
inline u32 __cdecl FakeProcessVerticalLine(const u32* origin,u32 distanceBits,u32* colPoint,u32** outEntity,u32 buildings,u32 vehicles,u32 peds,u32 objects,u32 dummies,u32 seeThrough,u32 collPoly) {
    ++ctx.calls;
    for(int i=0;i<3;++i)ctx.trace.push_back(origin[i]);
    ctx.trace.push_back(distanceBits);
    for(const u32 b:{buildings,vehicles,peds,objects,dummies,seeThrough})ctx.trace.push_back(b&0xFFu);
    ctx.trace.push_back(collPoly);
    colPoint[0]=0x3F000000u;colPoint[1]=0xBF000000u;colPoint[2]=ctx.hitZ;
    *outEntity=nullptr;
    return (ctx.retHigh<<8)|ctx.hit;
}

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,kArena> arena{};
inline constexpr uintptr_t kEntryOfCollaborator=0x5674E0;

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::unique_ptr<HookRoute> route;
    uintptr_t entry{0x515BD0};
    uintptr_t dllCollab{};      // cuerpo de la DLL de CWorld::ProcessVerticalLine (la DLL lo llama directamente)
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

// Un caso: CCamera::UpdateSoundDistances() con `this` = objeto de la arena (CCamera completo), entidad objetivo y matriz tambien en la arena.
inline void CaseSD(Env& env,Totals& t,unsigned n,uint32 savedFrame) {
    const F f=MakeF(n);
    u8* const obj=arena.data()+kObjOffset;
    u8* const entity=arena.data()+kEntityOffset;
    u8* const matrix=arena.data()+kMatrixOffset;
    const u32 entityPtr=u32(uintptr_t(entity)),matrixPtr=u32(uintptr_t(matrix));
    // ---- referencia
    std::array<u8,kArena> ref{};ref.fill(0xC3);
    u8* const ro=ref.data()+kObjOffset;u8* const re=ref.data()+kEntityOffset;u8* const rm=ref.data()+kMatrixOffset;
    FillWords(ro,kObjBytes,f.headSeed);FillWords(re,kEntityBytes,f.entSeed);FillWords(rm,kMatrixBytes,f.matSeed);
    for(u32 i=0;i<3;++i){Put32(ro,kOffPlacePos+4*i,f.placePos[i]);Put32(rm,kMatrixPosOffset+4*i,f.matPos[i]);Put32(ro,kOffForward+4*i,f.fwd[i]);}
    Put32(ro,kOffMatrix,f.hasMatrix?matrixPtr:0u);
    ro[kOffActive]=u8(f.active);
    for(u32 i=0,other=0;i<3;++i)Put32(ro,kOffCams+i*kCamStride+kOffCamMode,i==f.active?f.mode:f.otherModes[other++]);
    Put32(ro,kOffTarget,entityPtr);
    Put32(ro,kOffSoundUp,f.old158);Put32(ro,kOffSoundNew,f.old15C);Put32(ro,kOffSoundOld,f.old160);
    re[kEntityTypeOffset]=u8(f.etype);
    const json input={{"family",f.family},{"mode",f.mode},{"active",f.active},{"etype",f.etype},{"fwd",std::vector<u32>{f.fwd[0],f.fwd[1],f.fwd[2]}},{"has_matrix",f.hasMatrix},
                      {"mat_pos",std::vector<u32>{f.matPos[0],f.matPos[1],f.matPos[2]}},{"place_pos",std::vector<u32>{f.placePos[0],f.placePos[1],f.placePos[2]}},{"frame",f.frame},
                      {"old158",f.old158},{"old15c",f.old15C},{"old160",f.old160},{"hit",f.hit},{"hit_z",f.hitZ},{"ret_high",f.retHigh},
                      {"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
    u32* const frame=reinterpret_cast<u32*>(uintptr_t(kFrameCounterAddress));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        ctx.trace.clear();ctx.calls=0;ctx.hit=f.hit;ctx.hitZ=f.hitZ;ctx.retHigh=f.retHigh;
        arena=ref;*frame=f.frame;
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            ReversingProbe::EntryPatch exe(kEntryOfCollaborator,&FakeProcessVerticalLine),dll(env.dllCollab,&FakeProcessVerticalLine);
            return Gate(entry,nullptr,0,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);
        });
        const bool frameOK=*frame==f.frame;
        // guardas: todo lo que no es objeto, entidad ni matriz
        bool guards=memcmp(arena.data(),ref.data(),kObjOffset)==0;
        guards&=memcmp(arena.data()+kObjOffset+kObjBytes,ref.data()+kObjOffset+kObjBytes,kGuard)==0;
        guards&=memcmp(arena.data()+kEntityOffset+kEntityBytes,ref.data()+kEntityOffset+kEntityBytes,kGuard)==0;
        guards&=memcmp(arena.data()+kMatrixOffset+kMatrixBytes,ref.data()+kMatrixOffset+kMatrixBytes,kGuard)==0;
        // objeto sin los 12 bytes que escribe la funcion (+0x158..+0x163)
        bool rest=memcmp(obj,ro,kOffSoundUp)==0&&memcmp(obj+kOffSoundUp+kWrittenBytes,ro+kOffSoundUp+kWrittenBytes,kObjBytes-kOffSoundUp-kWrittenBytes)==0;
        json observed={{"written",HexBytes(obj+kOffSoundUp,kWrittenBytes)},{"rest_ok",rest},{"guard_ok",guards},{"entity_ok",memcmp(entity,re,kEntityBytes)==0},
                       {"matrix_ok",memcmp(matrix,rm,kMatrixBytes)==0},{"frame_ok",frameOK},{"calls",ctx.calls},{"trace",ctx.trace}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        *frame=savedFrame;
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK},{"traps_zero",ctx.calls<=1}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&*frame==savedFrame;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","traps_zero","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    for(const char* k:{"rest_ok","guard_ok","entity_ok","matrix_ok","frame_ok"})ok&=rows[0]["observed"][k]==true&&rows[1]["observed"][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function","CCamera::UpdateSoundDistances"},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    const bool collaboratorsOK=env.dllCollab&&ModuleOf(env.dllCollab)==env.asi;
    ready&=collaboratorsOK&&body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x515BD0),kEntryOfCollaborator,env.dllCollab,body})if(at)env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    const uint32 savedFrame=*reinterpret_cast<uint32*>(uintptr_t(kFrameCounterAddress));
    ctx.trace.reserve(32);
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",env.cases},{"cases",{{"CCamera::UpdateSoundDistances",env.cases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",std::vector<uintptr_t>{kEntryOfCollaborator}},{"dll",std::vector<uintptr_t>{env.dllCollab}}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_frame",savedFrame},
        {"layout",{{"arena",kArena},{"obj_offset",kObjOffset},{"obj_bytes",kObjBytes},{"entity_offset",kEntityOffset},{"matrix_offset",kMatrixOffset}}}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<env.cases;++c)CaseSD(env,totals,c,savedFrame);
        Write({{"event","function_summary"},{"function","CCamera::UpdateSoundDistances"},{"cases",env.cases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&*reinterpret_cast<uint32*>(uintptr_t(kFrameCounterAddress))==savedFrame;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"frame",*reinterpret_cast<uint32*>(uintptr_t(kFrameCounterAddress))}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
