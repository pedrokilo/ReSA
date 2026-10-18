#pragma once
#include <fstream>
#include <list>
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Scenes.hpp"

// Banco par a par de 20 funciones de biblioteca (RenderWare SDK / controlador D3D9 de RenderWare) contra su reconstruccion en C++ (hook RH apagado frente a encendido,
// dentro del juego), 1024 pares por funcion. Las funciones leen/escriben solo seis globales del motor y llaman, por direccion del EXE, a cuatro colaboradores sin reconstruir:
//   0x8084A0 _rwPluginRegistryAddPlugin, 0x8088E0 _rwPluginRegistryAddPluginStream, 0x8019B0 RwFreeListCreateAndPreallocateSpace, 0x4C9AB0 (registro del plugin de raster D3D9)
// que se sustituyen en AMBOS lados por el mismo falso trazado (EntryPatch en la direccion del EXE; la DLL los llama por esa direccion, sin hook propio, asi que no hace falta
// parche en la DLL). Todo el mundo (motor, modulos, objetos COM, nodos) vive en un arena propio del banco (Scenes.hpp); los seis globales se apuntan al arena durante cada
// llamada y se restauran antes de escribir en el registro. Ver README.md.
namespace Dependencies25DLive {
using namespace Dependencies25DScene;
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
// Deja en los ocho registros ST un residuo conocido y distinto (carga ocho doubles y los descarta): el FNSAVE posterior compara tambien el contenido de los registros vacios.
inline void PrimeX87() {
    static const double stale[8]{1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8};
    const double* p=stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE, incluido el contenido crudo de los registros ST marcados como vacios en FTW. Ninguna de las 20 funciones ejecuta
// instrucciones x87 (ni el EXE ni el C++ de la DLL): el estado x87 debe llegar intacto y FIP es el del ultimo fstp de PrimeX87 (en el ASI) en ambos lados. NO se relaja.
inline constexpr bool kCompareDeadX87Registers=true;

inline std::string Hex(uintptr_t a,size_t n){return HexOf(reinterpret_cast<const void*>(a),n);}
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

struct Run {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{},ip{};bool routeOK{};};
template<class G> Run Execute(OriginalDependencies499::HookRoute& route,bool candidate,uintptr_t entry,G gate){
 Run run{};
 route.Invoke(candidate,[&]{
  run.routeOK=candidate?ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==ModuleOf(uintptr_t(&ModuleMarker)):!ReversingProbe::JumpsToDll(entry);
  run.reg=gate();SaveFP(run.fp.data());run.mxcsr=ReadMXCSR();return 0;});
 memcpy(&run.ip,run.fp.data()+12,4);return run;
}

inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencies25d-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies25d-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies25d");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(GetModuleHandleA(nullptr));
 InitVtable();
 // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado. Orden = kSpecs (por direccion).
 const std::array<uintptr_t,20> candidates{
  std::bit_cast<uintptr_t>(&RwTextureGetMipmapping),std::bit_cast<uintptr_t>(&RwTextureGetAutoMipmapping),std::bit_cast<uintptr_t>(&RwTextureRegisterPlugin),
  std::bit_cast<uintptr_t>(&RwTexDictionaryRegisterPlugin),std::bit_cast<uintptr_t>(&RwTextureRasterGenerateMipmaps),std::bit_cast<uintptr_t>(&RwD3D9DynamicVertexBufferDestroy),
  std::bit_cast<uintptr_t>(&RwD3D9DynamicVertexBufferUnlock),std::bit_cast<uintptr_t>(&_rwD3D9SetDepthStencilSurface),std::bit_cast<uintptr_t>(&_rwDeviceRegisterPlugin),
  std::bit_cast<uintptr_t>(&RwD3D9DeleteVertexShader),std::bit_cast<uintptr_t>(&RwD3D9CreatePixelShader),std::bit_cast<uintptr_t>(&RwD3D9DeletePixelShader),
  std::bit_cast<uintptr_t>(&RwRasterGetCurrentContext),std::bit_cast<uintptr_t>(&RwRasterRegisterPlugin),std::bit_cast<uintptr_t>(&RwRasterLockPalette),
  std::bit_cast<uintptr_t>(&RwFreeListCreate),std::bit_cast<uintptr_t>(&RwFreeListCreateAndPreallocateSpace),std::bit_cast<uintptr_t>(&RwRGBAToPixel),
  std::bit_cast<uintptr_t>(&RwTextureRegisterPluginStream),std::bit_cast<uintptr_t>(&_rwObjectHasFrameReleaseFrame)};
 std::list<OriginalDependencies499::HookRoute> routes;
 for(size_t i=0;i<kSpecs.size();++i)routes.emplace_back(kSpecs[i].cls,kSpecs[i].fn,kSpecs[i].entry,candidates[i]);
 std::vector<OriginalDependencies499::HookRoute*> route;for(auto& r:routes)route.push_back(&r);
 // Colaboradores: deben ser, hoy, codigo del EXE sin hook (la DLL los llama por esas direcciones).
 constexpr std::array<uintptr_t,4> collaborators{0x8084A0,0x8088E0,0x8019B0,0x4C9AB0};
 bool collaboratorsOK=true;
 for(uintptr_t at:collaborators)collaboratorsOK&=!ReversingProbe::JumpsToDll(at)&&ModuleOf(Resolve(at))==gameExe;
 bool ready=collaboratorsOK;
 for(size_t i=0;i<route.size();++i)ready=ready&&route[i]->Registered()&&ModuleOf(route[i]->Body())==asi;
 std::vector<Watch> watches;
 for(const auto& spec:kSpecs)watches.emplace_back(spec.entry,16);
 for(uintptr_t at:collaborators)watches.emplace_back(at,16);
 auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
 const EngineGlobals savedGlobals=ReadGlobals();
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 json watched=json::array();for(const auto& w:watches)watched.push_back({{"address",w.address},{"bytes",Hex(w.address,16)}});
 json casesMap=json::object(),bodies=json::object();
 for(size_t i=0;i<kSpecs.size();++i){casesMap[kSpecs[i].name]=kCases;bodies[kSpecs[i].name]=route[i]->Body();}
 constexpr unsigned total=20*kCases;
 Write({{"event","begin"},{"registered_hook",ready},{"functions",20},{"pairs",total},{"cases",casesMap},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",true},
  {"collaborators",std::vector<uintptr_t>(collaborators.begin(),collaborators.end())},{"watched_entries",watched},{"bodies",bodies},{"asi_module",asi},{"original_module",gameExe},
  {"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},{"initial_globals",GlobalsJson(savedGlobals)},{"arena_base",Addr(0)},{"arena_size",kMem}});
 int pairs{};bool passed=ready;
 auto prepare=[&]{RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);};
 for(size_t fi=0;fi<kSpecs.size();++fi){
  const FnSpec& spec=kSpecs[fi];int failures{};
  for(unsigned c=0;c<kCases&&ready;++c){
   Run runs[2];json rows[2];json input;std::string beforeWindow;
   for(int side=0;side<2;++side){
    const bool candidate=side==1;prepare();
    CaseScene scene;traceLog.clear();trapCount=0;spec.build(c,scene);
    WriteGlobals(scene.g);
    const Snap before=Snapshot(scene);
    if(side==0){input=scene.input;input["window_lo"]=scene.winLo;input["window_len"]=scene.winLen;input["globals_in"]=GlobalsJson(scene.g);input["seeds"]={scene.seeds.eax,scene.seeds.ecx,scene.seeds.edx};input["args"]=std::vector<uint32>(scene.args,scene.args+6);beforeWindow=before.window;}
    runs[side]=Execute(*route[fi],candidate,spec.entry,[&]{
     std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
     for(const auto& p:scene.patches)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.first,p.second));
     return GateCdecl6(spec.entry,scene.args,scene.seeds);});
    const EngineGlobals after=ReadGlobals();
    json out=ObserveJson(scene,before,runs[side].reg,spec.voidFn,after);
    WriteGlobals(savedGlobals);
    auto fp=runs[side].fp;const auto logged=kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);
    out["post_floating_state"]=Hex(uintptr_t(logged.data()),108);out["post_mxcsr"]=runs[side].mxcsr;out["fp_ip_route_ok"]=ModuleOf(runs[side].ip)==asi;
    out["stack_restored"]=runs[side].reg.stack;out["nonvolatile_preserved"]=runs[side].reg.nonvolatile;out["route_ok"]=runs[side].routeOK;
    RestoreFloatingState();out["globals_restored"]=watchesIntact()&&GlobalsEqual(ReadGlobals(),savedGlobals);out["floating_state_restored"]=FloatingStateRestored();
    rows[side]=out;
   }
   bool ok=rows[0]==rows[1];
   for(const char* k:{"fp_ip_route_ok","stack_restored","nonvolatile_preserved","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
   ok&=rows[0]["outside_unchanged"]==true&&rows[0]["traps"]==0;
   auto fa=runs[0].fp,fb=runs[1].fp;
   passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"function",spec.name},{"case",c},{"pair",pairs++},{"input",input},{"before",beforeWindow},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
    {"original_residue",{{"eax",runs[0].reg.eax},{"ecx",runs[0].reg.ecx},{"edx",runs[0].reg.edx}}},{"candidate_residue",{{"eax",runs[1].reg.eax},{"ecx",runs[1].reg.ecx},{"edx",runs[1].reg.edx}}},
    {"equal",ok},{"accepted",ok}});
  }
  Write({{"event","function_summary"},{"function",spec.name},{"cases",int(kCases)},{"failures",failures},{"state_restored",watchesIntact()}});
 }

 // Las vigilancias se tomaron con los hooks apagados (el constructor de HookRoute los apaga): se comprueban ANTES de Restore(), que vuelve a encenderlos.
 // Restore() verifica por su cuenta que los 5 bytes de cada entrada y el estado Hooked/Locked vuelven a lo inicial; los colaboradores no cambian con Restore().
 const bool entriesIntactBefore=watchesIntact();
 bool stateRestored=entriesIntactBefore&&GlobalsEqual(ReadGlobals(),savedGlobals);
 for(auto* r:route)stateRestored&=r->Restore();
 for(size_t i=kSpecs.size();i<watches.size();++i)stateRestored&=watches[i].Intact();
 RestoreFloatingState();stateRestored&=FloatingStateRestored();
 Write({{"event","global_state_final"},{"floating_state",Hex(uintptr_t(savedFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntactBefore},{"globals",GlobalsJson(ReadGlobals())}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntactBefore}});
 RestoreFloatingState();savedFPActive=false;
}
}
