#pragma once
#include <fstream>
#include "ControllerConfigManager.h"
#include "WinInput.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Rows.hpp"

// Banco par a par de una entrada del EXE contra su reconstruccion en C++ (hook RH apagado frente a encendido, dentro del juego):
//   WinInput::diPadInit   0x7485C0   (cdecl, sin argumentos, void; el EXE conserva EBX y usa EAX/ECX/EDX)   2048 pares
// La funcion lee los dos punteros de dispositivo de DirectInput de RsGlobal.ps (diDevice1/diDevice2), la tabla AllValidWinJoys (0xC92144, 2 x 0x10 bytes) y llama:
//   0x7477C0 (EXE sin reconstruir; la DLL lo llama por direccion -> un solo EntryPatch), 0x530B00 InitDefaultControlConfigJoyPad (hook RH de la DLL: EntryPatch en la
//   direccion del EXE y en el cuerpo de la DLL, mismo falso) y dos metodos COM (GetCapabilities +0xC, GetProperty +0x14) de objetos IDirectInputDevice8 FALSOS del banco.
// No se abre ningun dispositivo real; se guardan y restauran la tabla y los dos punteros de dispositivo. Ver README.md.
namespace Dependencies24BLive {
using namespace Dependencies24BFix;
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
// diPadInit no ejecuta ninguna instruccion x87 (ni el EXE ni el C++ de la DLL, que usa SSE): la compuerta se mantiene igualmente y NO se relaja.
inline constexpr bool kCompareDeadX87Registers=true;

inline std::string Hex(uintptr_t a,size_t n){return HexBytes(reinterpret_cast<const void*>(a),n);}
// Vigila bytes de codigo (entradas parcheadas temporalmente): deben volver a su contenido inicial tras cada llamada.
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};

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

struct Run {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
template<class G> Run Execute(OriginalDependencies499::HookRoute& route,bool candidate,uintptr_t entry,G gate){
 Run run{};
 route.Invoke(candidate,[&]{
  run.routeOK=candidate?ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==ModuleOf(uintptr_t(&ModuleMarker)):!ReversingProbe::JumpsToDll(entry);
  run.reg=gate();SaveFP(run.fp.data());run.mxcsr=ReadMXCSR();return 0;});
 return run;
}

inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencies24b-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies24b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies24b");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning()||!RsGlobal.ps)return;done=true;
 const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(GetModuleHandleA(nullptr));
 // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
 OriginalDependencies499::HookRoute route("Input","diPadInit",kEntry,&WinInput::diPadInit);
 // Entrada de la DLL del colaborador reconstruido (se resuelve saltando los E9 de enlace incremental) y direcciones del EXE.
 const uintptr_t initDll=Resolve(std::bit_cast<uintptr_t>(&CControllerConfigManager::InitDefaultControlConfigJoyPad));
 bool collaboratorsOK=ModuleOf(initDll)==asi;
 // 0x7477C0 debe ser, hoy, el codigo del EXE sin hook (la DLL lo llama por direccion).
 collaboratorsOK&=!ReversingProbe::JumpsToDll(kEnumExe)&&ModuleOf(Resolve(kEnumExe))==gameExe;
 const bool ready=route.Registered()&&collaboratorsOK&&ModuleOf(route.Body())==asi;
 std::vector<Watch> watches;
 for(uintptr_t at:{uintptr_t(kEntry),uintptr_t(kEnumExe),uintptr_t(kInitExe),initDll})watches.emplace_back(at,16);
 auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
 InitVtable();
 const Saved saved=SaveGlobals();
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 json watched=json::array();for(const auto& w:watches)watched.push_back({{"address",w.address},{"bytes",Hex(w.address,16)}});
 Write({{"event","begin"},{"registered_hook",ready},{"functions",1},{"pairs",kCases},{"cases",{{"diPadInit",kCases}}},
  {"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",true},
  {"collaborators",{{"exe",std::vector<uintptr_t>{kEnumExe,kInitExe}},{"dll",std::vector<uintptr_t>{initDll}}}},
  {"watched_entries",watched},{"bodies",{{"diPadInit",route.Body()}}},{"asi_module",asi},{"original_module",gameExe},
  {"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},{"initial_table",HexBytes(saved.table.data(),saved.table.size())},
  {"initial_dev1",saved.dev1},{"initial_dev2",saved.dev2},{"ps",*reinterpret_cast<uint32*>(kPsPtr)}});
 int pairs{};bool passed=ready;int failures{};
 auto prepare=[&]{RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);};
 alignas(16) static std::array<Fix,kCases> fixtures{};
 for(unsigned c=0;c<kCases&&ready;++c){
  fixtures[c]=Make(c);const Fix& fx=fixtures[c];Run runs[2];json rows[2];
  for(int side=0;side<2;++side){
   const bool candidate=side==1;prepare();Prepare(fx);
   runs[side]=Execute(route,candidate,kEntry,[&]{
    // Los tres parches (dos en el EXE, uno en la DLL) llevan el mismo falso y se retiran al salir.
    ReversingProbe::EntryPatch enumExe(kEnumExe,&StubEnum),initExe(kInitExe,&StubInit),initDllPatch(initDll,&StubInit);
    return GateCdecl0(kEntry,fx.eax,fx.ecx,fx.edx,fx.stack_seed);});
   const Observed obs=Observe();
   auto fp=runs[side].fp;memset(fp.data()+12,0,4);const auto logged=kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);
   json out={{"input",InputRow(c,fx)},{"observed",ObservedRow(obs)},{"post_floating_state",Hex(uintptr_t(logged.data()),108)},{"post_mxcsr",runs[side].mxcsr},
    {"stack_restored",runs[side].reg.stack},{"nonvolatile_preserved",runs[side].reg.nonvolatile},{"route_ok",runs[side].routeOK},{"traps_zero",obs.traps==0}};
   RestoreGlobals(saved);RestoreFloatingState();
   out["globals_restored"]=watchesIntact()&&GlobalsEqual(saved);out["floating_state_restored"]=FloatingStateRestored();
   rows[side]=out;
  }
  bool ok=rows[0]==rows[1];
  for(const char* k:{"stack_restored","nonvolatile_preserved","route_ok","traps_zero","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
  auto fa=runs[0].fp,fb=runs[1].fp;memset(fa.data()+12,0,4);memset(fb.data()+12,0,4);
  passed&=ok;failures+=!ok;
  Write({{"event","comparison"},{"function","WinInput::diPadInit"},{"case",c},{"pair",pairs++},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
   {"original_residue",{{"eax",runs[0].reg.eax},{"ecx",runs[0].reg.ecx},{"edx",runs[0].reg.edx}}},{"candidate_residue",{{"eax",runs[1].reg.eax},{"ecx",runs[1].reg.ecx},{"edx",runs[1].reg.edx}}},
   {"equal",ok},{"accepted",ok}});
 }
 Write({{"event","function_summary"},{"function","WinInput::diPadInit"},{"cases",int(kCases)},{"failures",failures},{"state_restored",watchesIntact()}});

 // Las vigilancias se tomaron con el hook apagado: se comprueban ANTES de Restore(), que vuelve a encenderlo.
 const bool entriesIntactBefore=watchesIntact();
 bool stateRestored=entriesIntactBefore&&GlobalsEqual(saved);stateRestored&=route.Restore();
 for(size_t i=1;i<watches.size();++i)stateRestored&=watches[i].Intact();
 RestoreFloatingState();stateRestored&=FloatingStateRestored();
 Write({{"event","global_state_final"},{"floating_state",Hex(uintptr_t(savedFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntactBefore},
  {"table",HexBytes(reinterpret_cast<void*>(kTable),kTableSize)},{"dev1",PsField(kPsDev1)},{"dev2",PsField(kPsDev2)}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntactBefore}});
 RestoreFloatingState();savedFPActive=false;
}
}
