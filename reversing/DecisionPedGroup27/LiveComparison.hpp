#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "DecisionMakers/Decision.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "PedIntelligence.h"
#include "PedGroup.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"

// Banco par a par DecisionPedGroup27 dentro del juego: Update() se llama desde CGame::Process (probe_build.py).
// Derivado de reversing/DependenciesDecisionPed7 (fixtures y contrato de CDecisionMakerTypes::MakeDecision Ped 0x606E70 / Group 0x606F80) y de
// reversing/DecisionMakeDecision27 (llamada al EXE/DLL con semilla srand 0x821B11 compartida y comparacion del estado de rand).
//
// Original = EXE con los TRES hooks apagados (0x606E70, 0x606F80 y 0x6040D0): el EXE ejecuta todo, incluidos 0x6040D0, su colaborador 0x6007A0 y rand 0x821B1E reales.
// Candidato = DLL con los hooks de 0x606E70/0x606F80 y de 0x6040D0 encendidos: los dos despachadores y CDecision::MakeDecision (con 0x6007A0 en linea y rand del EXE por
// direccion) se ejecutan en C++. 0x606060 (CDecisionMaker, ruta -1 de Ped) sigue siendo el del EXE en AMBOS lados; como 0x606060 llama a 0x6040D0, en el candidato esa ruta
// entra a la DLL por el hook de 0x6040D0 (esa es la composicion que se valida). Ningun colaborador se sustituye (`collaborators_stubbed=false`).
// El RNG es real: ambos lados arrancan con el MISMO estado (srand del EXE 0x821B11; tiddata+0x14 via 0x827B3D) y se compara el estado tras la llamada y el numero de rand consumidos.
namespace DecisionPedGroup27Live {
using namespace DecisionPedGroup27Fix;
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
// El EXE ejecuta x87 en 0x6040D0 y la DLL puede usar SSE: se compara lo observable (FCW completa, TOP, etiquetas y ST validos; ninguno debe quedar) y se ignoran las banderas
// de excepcion/condicion de FSW, FIP/FCS/FOP/FDP/FDS y el contenido de los registros marcados como vacios (igual que DecisionMakeDecision27).
inline constexpr bool kCompareDeadX87Registers=false;
// Contrato de registros (README.md, seccion «Analisis de llamadores»): EAX/ECX/EDX de salida de AMBAS funciones se registran (original_residue/candidate_residue) pero NO forman parte
// de la igualdad, salvo el EAX de Group, que es su VALOR DE RETORNO (movsx eax,word ptr [ranura]) y que el llamador 0x4B5841/0x4B58C3 lee (`mov [edi+0Eh],ax`): ese va en observed.return_eax.
inline constexpr bool kCompareVolatileRegisters=false;
inline constexpr bool kCompareGroupReturnEax=true;

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

// ------------------------------------------------------------------------------------------------ bytes canonicos del EXE (16 por direccion, extraidos de D:\Grand Theft Auto San Andreas\gta-sa.exe)
struct Canon {uintptr_t at;std::array<uint8,16> bytes;};
inline const Canon kCanon[]{
    {0x6040D0,{0x83,0xEC,0x60,0x53,0x55,0x56,0x8B,0xD9,0x33,0xF6,0x57,0x89,0x5C,0x24,0x10,0x33}},   // CDecision::MakeDecision
    {0x606060,{0x8B,0x54,0x24,0x20,0x8B,0xC1,0x8B,0x4C,0x24,0x24,0x51,0x8B,0x4C,0x24,0x20,0x52}},   // CDecisionMaker (ruta -1 de Ped)
    {0x6007A0,{0x8B,0x54,0x24,0x04,0x83,0xFA,0xFF,0x53,0x8B,0x5C,0x24,0x0C,0x55,0x8B,0x6C,0x24}},   // colaborador de 0x6040D0 (seleccion)
    {0x821B1E,{0xE8,0x1A,0x60,0x00,0x00,0x8B,0x48,0x14,0x69,0xC9,0xFD,0x43,0x03,0x00,0x81,0xC1}},   // rand
    {0x821B11,{0xE8,0x27,0x60,0x00,0x00,0x8B,0x4C,0x24,0x04,0x89,0x48,0x14,0xC3,0xE8,0x1A,0x60}},   // srand
    {0x827B3D,{0x53,0x56,0xFF,0x15,0x94,0x80,0x85,0x00,0xFF,0x35,0x10,0x31,0x8E,0x00,0x8B,0xD8}},   // getptd
    {0x606E70,{0x8A,0x44,0x24,0x24,0x84,0xC0,0x53,0x8B,0x5C,0x24,0x08,0x56,0x57,0x74,0x0E,0x8B}},   // MakeDecision Ped
    {0x606F80,{0x8B,0x54,0x24,0x08,0x8B,0x94,0x91,0x34,0xC0,0x00,0x00,0x56,0x8B,0x74,0x24,0x08}},   // MakeDecision Group
};
inline bool CanonicalBytes() {for(const auto& c:kCanon)if(memcmp(c.bytes.data(),reinterpret_cast<void*>(c.at),16)!=0)return false;return true;}

// ------------------------------------------------------------------------------------------------ llamada con medicion de ABI
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// thiscall con argumentos en pila (argsPtr[0] queda en la cima), callee limpia (ret 2Ch / ret 20h). EAX/EDX/pila sembrados; EBX/ESI/EDI sembrados y comprobados; EBP y la cabeza
// de la cadena SEH (fs:[0]) comparadas antes/despues; EFLAGS justo tras el call (DF).
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},outFlags{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{},sehBefore{},sehAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov eax,dword ptr fs:[0]
        mov sehBefore,eax
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
        mov espAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
        mov eax,dword ptr fs:[0]
        mov sehAfter,eax
        pop edi
        pop esi
        pop ebx
    }
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter,sehBefore==sehAfter};
}
// ------------------------------------------------------------------------------------------------ RNG del EXE
inline void ExeSrand(u32 seed) {reinterpret_cast<void(__cdecl*)(u32)>(0x821B11)(seed);}
inline u32* ExeRandState() {return reinterpret_cast<u32*>(reinterpret_cast<uint8*(__cdecl*)()>(0x827B3D)()+0x14);}
// numero de rand consumidos: pasos del LCG desde la semilla hasta el estado observado (-1 = estado inesperado). La ruta -1 de Ped pasa por 0x606060 -> 0x6040D0 una sola vez.
inline int RandCalls(u32 seed,u32 state){u32 s=seed;for(int n=0;n<=8;++n){if(s==state)return n;s=NextState(s);}return -1;}

// ------------------------------------------------------------------------------------------------ memoria del caso
inline std::vector<u8> maker,makerRef;
alignas(16) inline std::array<u8,sizeof(CPed)> ped{},pedRef{};
alignas(16) inline std::array<u8,sizeof(CPedIntelligence)> intel{},intelRef{};
alignas(16) inline std::array<u8,sizeof(CPedGroup)> group{},groupRef{};
inline std::array<int16,4> outputs{};
inline constexpr std::array<int16,4> kOutputCanary{int16(0x5A51),int16(0x5A52),int16(0x5A53),int16(0x5A54)};

using PedDecision=void(CDecisionMakerTypes::*)(CPed*,eEventType,int32,bool,eTaskType,eTaskType,eTaskType,eTaskType,bool,int16&,int16&);
using GroupDecision=eTaskType(CDecisionMakerTypes::*)(CPedGroup*,eEventType,int32,bool,eTaskType,eTaskType,eTaskType,eTaskType);
inline constexpr uintptr_t kEntries[2]{0x606E70,0x606F80};
inline const char* const kNames[2]{"CDecisionMakerTypes::MakeDecision-Ped","CDecisionMakerTypes::MakeDecision-Group"};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};u32 rngState{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> ped,group,decision;   // 0x606E70, 0x606F80 y 0x6040D0 (CDecision::MakeDecision)
    std::vector<Watch> watches;                      // codigo del EXE que debe seguir intacto (colaboradores y cuerpos de las entradas tras el parche de 5 bytes)
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

template<class G> Side Execute(Env& env,u32 func,bool candidate,G gate) {
    Side side{};
    const uintptr_t entry=kEntries[func],other=kEntries[1-func];
    auto call=[&]{
        auto inAsi=[&](uintptr_t a){return ReversingProbe::JumpsToDll(a)&&ModuleOf(Resolve(a))==env.asi;};
        // original: ninguno de los tres hooks salta a la DLL; candidato: la entrada y 0x6040D0 si, la otra entrada no. 0x606060 y 0x6007A0 son SIEMPRE los del EXE.
        const bool hooks=candidate?(inAsi(entry)&&inAsi(0x6040D0)):(!ReversingProbe::JumpsToDll(entry)&&!ReversingProbe::JumpsToDll(0x6040D0));
        side.routeOK=hooks&&!ReversingProbe::JumpsToDll(other)&&!ReversingProbe::JumpsToDll(0x606060)&&!ReversingProbe::JumpsToDll(0x6007A0);
        side.reg=gate(entry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    HookRoute& entryRoute=func==0?*env.ped:*env.group;
    env.decision->Invoke(candidate,[&]{return entryRoute.Invoke(candidate,call);});
    return side;
}

// Un caso de una funcion: indice global de pares `pair`, caso `n` de la funcion `func`.
inline void CaseRun(Env& env,Totals& t,u32 func,u32 n,u32 savedRand) {
    const Case f=MakeCase(func,n);
    auto put32=[](u8* p,u32 v){memcpy(p,&v,4);};
    // objetos de referencia (identicos para los dos lados)
    FillWords(makerRef.data(),makerRef.size(),f.fillSeed);
    put32(makerRef.data()+0xC034+f.eventId*4,f.eventIndex);
    memcpy(makerRef.data()+f.selectedOffset,f.choice,kChoiceBytes);
    pedRef.fill(0);intelRef.fill(0);groupRef.fill(0);
    put32(pedRef.data()+0x47C,uint32(reinterpret_cast<uintptr_t>(intel.data())));pedRef[0x484]=uint8(f.pedType);
    put32(intelRef.data()+0xB4,uint32(f.dmB4));put32(intelRef.data()+0xB8,uint32(f.dmB8));
    put32(groupRef.data()+0x2C8,uint32(f.groupId));groupRef[0x2D0]=uint8(f.missionFlag);
    const json input={{"func",func},{"case",n},{"family",f.family},{"dm_b4",f.dmB4},{"dm_b8",f.dmB8},{"use_group",f.useGroup},{"group_id",f.groupId},{"ped_type",f.pedType},{"mission_flag",f.missionFlag},
                      {"event_id",f.eventId},{"event_index",f.eventIndex},{"st",f.sourceType},{"in_veh",f.inVehicle},{"avoid",{f.avoid[0],f.avoid[1],f.avoid[2]}},{"seek",f.seek},
                      {"seed",f.randState},{"selected_offset",f.selectedOffset},{"obj",HexBytes(f.choice,kChoiceBytes)}};
    const uint32 stackSeed=Rand(n,77)+func,edxSeed=0xED123411u^n;
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        maker=makerRef;ped=pedRef;intel=intelRef;group=groupRef;outputs=kOutputCanary;
        ExeSrand(f.randState);
        sides[s]=Execute(env,func,candidate,[&](uintptr_t entry){
            std::array<u32,11> args{};u32 argc;
            if(func==0){
                args={uint32(reinterpret_cast<uintptr_t>(ped.data())),f.eventId,f.sourceType,f.inVehicle,u32(f.avoid[0]),u32(f.avoid[1]),u32(f.avoid[2]),u32(f.seek),f.useGroup,
                      uint32(reinterpret_cast<uintptr_t>(&outputs[1])),uint32(reinterpret_cast<uintptr_t>(&outputs[2]))};argc=11;
            }else{
                args={uint32(reinterpret_cast<uintptr_t>(group.data())),f.eventId,f.sourceType,f.inVehicle,u32(f.avoid[0]),u32(f.avoid[1]),u32(f.avoid[2]),u32(f.seek)};argc=8;
            }
            return Gate(entry,args.data(),argc,0xA5123456u,uint32(reinterpret_cast<uintptr_t>(maker.data())),edxSeed,stackSeed);
        });
        const u32 rngState=*ExeRandState();sides[s].rngState=rngState;
        const bool objectsUnchanged=maker==makerRef&&ped==pedRef&&intel==intelRef&&group==groupRef;
        bool outputsOk=outputs[0]==kOutputCanary[0]&&outputs[3]==kOutputCanary[3];
        if(func==1)outputsOk&=outputs[1]==kOutputCanary[1]&&outputs[2]==kOutputCanary[2];   // Group no recibe salidas externas
        json observed={{"selected_decision",HexBytes(maker.data()+f.selectedOffset,kChoiceBytes)},{"objects_unchanged",objectsUnchanged},{"outputs_guard_ok",outputsOk},
                       {"rng_state",rngState},{"rand_calls",RandCalls(f.randState,rngState)}};
        if(func==0){observed["task_slot"]=int(outputs[1]);observed["face_slot"]=int(outputs[2]);}
        else if(kCompareGroupReturnEax)observed["return_eax"]=sides[s].reg.eax;
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        *ExeRandState()=savedRand;
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"seh_restored",sides[s].reg.seh},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&*ExeRandState()==savedRand;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","seh_restored","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    ok&=rows[0]["observed"]["objects_unchanged"]==true&&rows[0]["observed"]["outputs_guard_ok"]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function",kNames[func]},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    for(const uintptr_t at:{uintptr_t(0x606060),uintptr_t(0x6007A0),uintptr_t(0x821B1E),uintptr_t(0x821B11),uintptr_t(0x827B3D)})env.watches.emplace_back(at,16);
    for(const uintptr_t at:{uintptr_t(0x6040D5),uintptr_t(0x606E75),uintptr_t(0x606F85)})env.watches.emplace_back(at,11);   // cuerpo de cada entrada tras el parche de 5 bytes
    const bool canonical=CanonicalBytes();   // los tres hooks estan apagados en este punto (el constructor de HookRoute los deja apagados)
    ready&=canonical&&!ReversingProbe::JumpsToDll(0x6040D0)&&!ReversingProbe::JumpsToDll(0x606E70)&&!ReversingProbe::JumpsToDll(0x606F80)&&!ReversingProbe::JumpsToDll(0x606060)&&!ReversingProbe::JumpsToDll(0x6007A0);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    const uint32 savedRand=*ExeRandState();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    maker.assign(sizeof(CDecisionMakerTypes),0);makerRef.assign(sizeof(CDecisionMakerTypes),0);
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",2},{"pairs",2*kCasesPerFunction},
        {"cases",{{kNames[0],kCasesPerFunction},{kNames[1],kCasesPerFunction}}},{"direct_functions",json::array()},
        {"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},{"compare_group_return_eax",kCompareGroupReturnEax},
        {"register_policy","EAX/ECX/EDX de salida solo registrados (original_residue/candidate_residue) salvo el EAX de Group, que es el valor de retorno y se compara en observed.return_eax"},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",false},
        {"collaborators",{{"exe",std::vector<uintptr_t>{0x606060,0x6007A0,0x821B1E}},{"dll",std::vector<uintptr_t>{0x6040D0}}}},
        {"canonical_bytes_ok",canonical},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_rand_state",savedRand}});
    Totals totals;
    for(u32 func=0;func<2;++func){
        const unsigned failuresBefore=totals.failures;
        if(ready)for(u32 c=0;c<kCasesPerFunction;++c)CaseRun(env,totals,func,c,savedRand);
        Write({{"event","function_summary"},{"function",kNames[func]},{"cases",kCasesPerFunction},{"failures",totals.failures-failuresBefore},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&*ExeRandState()==savedRand;
    for(auto* route:{env.ped.get(),env.group.get(),env.decision.get()})if(route)stateRestored&=route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"rand_state",*ExeRandState()}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"decisionpedgroup27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"decisionpedgroup27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("decisionpedgroup27");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja cada hook apagado.
    env.ped.reset(new HookRoute("CDecisionMakerTypes","MakeDecision-Ped",0x606E70,FunctionToVoidPtr(static_cast<PedDecision>(&CDecisionMakerTypes::MakeDecision))));
    env.group.reset(new HookRoute("CDecisionMakerTypes","MakeDecision-Group",0x606F80,FunctionToVoidPtr(static_cast<GroupDecision>(&CDecisionMakerTypes::MakeDecision))));
    env.decision.reset(new HookRoute("CDecision","MakeDecision",0x6040D0,HookRoute::RegisteredTarget{}));
    bool ready=true;json registered=json::object();json bodies=json::object();
    const struct {const char* name;HookRoute* route;} routes[]{{"CDecisionMakerTypes::MakeDecision-Ped",env.ped.get()},{"CDecisionMakerTypes::MakeDecision-Group",env.group.get()},{"CDecision::MakeDecision",env.decision.get()}};
    for(const auto& r:routes){
        registered[r.name]=r.route->Registered();bodies[r.name]=r.route->Body();
        ready&=r.route->Registered()&&r.route->Body()&&ModuleOf(r.route->Body())==env.asi;
    }
    Run(env,ready,registered,bodies);
}
}
