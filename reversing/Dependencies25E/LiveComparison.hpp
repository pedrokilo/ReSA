#pragma once
#include <fstream>
#include <functional>
#include <memory>
#include "World.h"
#include "CutsceneMgr.h"
#include "Entity/Ped/PlayerPed.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"

// Banco par a par de 16 funciones de biblioteca del EXE (RenderWare y CRT estatica de MSVC) contra su reconstruccion en C++ (hook RH apagado frente a
// encendido, dentro del juego), 1024 pares por funcion (16384 en total):
//   RwCore     RxHeapGetGlobalHeap 0x8052F0 | _rwFrameListDeinitialize 0x807720 | RwD3D9RasterIsCompressed 0x820C90
//   RwPlCore   _rwPluginRegistryGetPluginOffset 0x808470 | _rwSListDestroyEndEntries 0x809400 | _rwSListGetNumEntries 0x8094B0 | _rwSListGetEntry 0x809510
//              _rwSListGetBegin 0x809530 | _rwSListGetEnd 0x809540 | _rwStringClose 0x80A440 | _rwStringStreamGetSize 0x80A470
//   CRT        CrtAtexit 0x821D1E | CrtLockFile2 0x823FD1 | CrtUnlockFile2 0x824023 | CrtMalloc 0x824257 | CrtErrno 0x82B181
// Todas son cdecl (ret sin parametros). Los objetos son arenas propios del banco (Fixtures.hpp). Colaboradores y como se tratan:
//   * Punteros a funcion del motor RenderWare (rwfree [engine+0x138], rwstrlen [engine+0x120]): se sustituyen en AMBOS lados por el mismo doble trazado y se restauran.
//   * _onexit (0x821CE6): doble trazado con EntryPatch en la direccion del EXE en AMBOS lados (la DLL lo llama por esa misma direccion). Sin el doble, atexit registraria una
//     funcion del banco para el cierre del proceso.
//   * _lock/_unlock (0x82ADBE/0x82AD12), _getptd (0x827B3D), _nh_malloc (0x82422B): REALES (los usan otros hilos; no se parchean). El estado que tocan (seccion critica del
//     cerrojo de flujo 16 + n, errno, montón) se observa y se restaura; el manejador de falta de memoria [0xC9AC68] se sustituye por un doble trazado.
namespace Dependencies25ELive {
using namespace Dependencies25EFix;
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
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE (TOP, FCW, FSW, FTW, FIP/FCS/FDP/FDS y los diez bytes de cada registro ST, incluidos los vacios).
// Ninguna de las 16 funciones (ni sus colaboradores reales) ejecuta x87: FIP debe ser el del ultimo fstp de PrimeX87 en ambos lados.
// Relajarla es una decision de politica del propietario del proyecto: no se ha tomado.
inline constexpr bool kCompareDeadX87Registers=true;

struct Result {uint32 eax,ecx,edx;bool stack,nonvolatile;};
// cdecl con `n` palabras apiladas (la primera de `args` queda en la cima); EAX/ECX/EDX con semillas por caso, capturados justo tras la llamada.
__declspec(noinline) inline Result Gate(uintptr_t entry,const uint32* args,uint32 n,uint32 seedA,uint32 seedC,uint32 seedD){uint32 before{},after{},a{},c{},d{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {mov ecx,n} __asm {mov edx,args}
 push_loop: __asm {test ecx,ecx} __asm {jz push_end} __asm {push dword ptr [edx+ecx*4-4]} __asm {dec ecx} __asm {jmp push_loop}
 push_end: __asm {mov eax,seedA} __asm {mov ecx,seedC} __asm {mov edx,seedD} __asm {call entry}
 __asm {mov a,eax} __asm {mov c,ecx} __asm {mov d,edx} __asm {mov ecx,n} __asm {lea esp,[esp+ecx*4]}
 __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {a,c,d,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}

inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=digits[b[i]>>4];out+=digits[b[i]&15];}return out;}
struct Block {uintptr_t address;std::vector<uint8> saved;Block(uintptr_t a,size_t n):address(a),saved(n){memcpy(saved.data(),reinterpret_cast<void*>(a),n);}void Restore()const{memcpy(reinterpret_cast<void*>(address),saved.data(),saved.size());}bool Restored()const{return !memcmp(reinterpret_cast<void*>(address),saved.data(),saved.size());}std::string Observe()const{return Hex(address,saved.size());}};
// Vigila bytes de codigo (entradas parcheadas temporalmente): deben volver a su contenido inicial tras cada llamada.
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};
inline uint32 ReadInt(uintptr_t a){uint32 v{};memcpy(&v,reinterpret_cast<void*>(a),4);return v;}
inline void PutInt(uintptr_t a,uint32 v){memcpy(reinterpret_cast<void*>(a),&v,4);}

// ---- FNSAVE (108 bytes): +0 FCW, +4 FSW, +8 FTW (2 bits por registro fisico, 3 = vacio), +12 FIP, +16 FCS|FOP<<16, +20 FDP, +24 FDS, +28 ST0..ST7 (10 bytes, orden de pila).
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
 json out=json::array();
 const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw",4,2},{"ftw",8,2},{"fip",12,4},{"fcs_fop",16,4},{"fdp",20,4},{"fds",24,4}};
 for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
 for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
 return out;
}

// ---- Direcciones del EXE
inline constexpr uintptr_t kEngineInstance=0xC97B24, kEngineFreeOffset=0x138, kEngineStrlenOffset=0x120;       // RwEngineInstance (RwGlobals*): memoryFuncs.rwfree / stringFuncs.vecStrlen
inline constexpr uintptr_t kRxHeapGlobal=0xC9A6B0, kRasterExtOffset=0xB4E9E0, kNewMode=0xC9ADCC, kNewHandler=0xC9AC68;
inline constexpr uintptr_t kLockTable=0x8E31C0, kLockTableBytes=0x120, kLockStreamBase=16, kGetPtd=0x827B3D, kRawLock=0x82ADBE, kRawUnlock=0x82AD12, kOnexit=0x821CE6, kRawFree=0x82413F;

// ---- Dobles trazados (los mismos en los dos lados)
inline std::vector<uint32> trace;
inline uint32 strlenValue{}, onexitValue{};
inline void __cdecl FreeStub(void* block) {trace.push_back(uint32(reinterpret_cast<uintptr_t>(block)));}
inline size_t __cdecl StrlenStub(const char* string) {trace.push_back(uint32(reinterpret_cast<uintptr_t>(string)));return strlenValue;}
inline void* __cdecl OnexitStub(void* function) {trace.push_back(uint32(reinterpret_cast<uintptr_t>(function)));return reinterpret_cast<void*>(uintptr_t(onexitValue));}
inline int __cdecl NewHandlerStub(size_t size) {trace.push_back(uint32(size));return 0;}   // 0: no reintentar -> malloc devuelve NULL

inline uintptr_t GetPtd(){uintptr_t value{};const auto fn=kGetPtd;__asm {call fn} __asm {mov value,eax}return value;}
inline void RawLock(uint32 n){const auto fn=kRawLock;__asm {push n} __asm {call fn} __asm {add esp,4}}
inline void RawUnlock(uint32 n){const auto fn=kRawUnlock;__asm {push n} __asm {call fn} __asm {add esp,4}}
inline void RawFree(void* p){const auto fn=kRawFree;__asm {push p} __asm {call fn} __asm {add esp,4}}
// Arena propio: [guarda][interior][guarda]
template<size_t Interior> struct Arena {
 alignas(16) std::array<uint8,Interior+2*kGuard> mem{};
 uint8* Interior_() {return mem.data()+kGuard;}
 void Reset() {memset(mem.data(),kGuardByte,mem.size());}
 bool GuardsOk() const {for(size_t i=0;i<kGuard;++i)if(mem[i]!=kGuardByte||mem[mem.size()-1-i]!=kGuardByte)return false;return true;}
 std::string Hex_() const {return Hex(uintptr_t(mem.data()),mem.size());}
};

// ---- Ejecucion de un caso en un lado
struct Prepared {
 std::vector<uint32> args;
 json input;
 uint32 base{};                                    // direccion del interior del arena (0 si no hay arena)
 std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
 std::function<json(const Result&)> observe;      // campos propios de la funcion tras la llamada (en ambos lados)
 std::function<std::string()> arenaHex;           // arena completo (guardas incluidas), o vacio
 std::function<bool()> arenaGuardsOk;
 std::function<void()> cleanup;                   // acciones que los Block no cubren (soltar cerrojos...), antes de restaurar
};

inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencies25e-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies25e-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies25e");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(GetModuleHandleA(nullptr));
 using Route=OriginalDependencies499::HookRoute;
 const auto Reg=Route::RegisteredTarget{};
 // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado.
 struct Entry {const char* cls;const char* name;uintptr_t address;std::unique_ptr<Route> route;};
 std::vector<Entry> entries;
 const auto add=[&](const char* cls,const char* name,uintptr_t address){entries.push_back({cls,name,address,std::make_unique<Route>(cls,name,address,Reg)});return entries.back().route.get();};
 Route* rxHeap=add("RwCore","RxHeapGetGlobalHeap",0x8052F0);
 Route* frameList=add("RwCore","_rwFrameListDeinitialize",0x807720);
 Route* raster=add("RwCore","RwD3D9RasterIsCompressed",0x820C90);
 Route* plugin=add("RwPlCore","_rwPluginRegistryGetPluginOffset",0x808470);
 Route* slDestroy=add("RwPlCore","_rwSListDestroyEndEntries",0x809400);
 Route* slNum=add("RwPlCore","_rwSListGetNumEntries",0x8094B0);
 Route* slEntry=add("RwPlCore","_rwSListGetEntry",0x809510);
 Route* slBegin=add("RwPlCore","_rwSListGetBegin",0x809530);
 Route* slEnd=add("RwPlCore","_rwSListGetEnd",0x809540);
 Route* strClose=add("RwPlCore","_rwStringClose",0x80A440);
 Route* strSize=add("RwPlCore","_rwStringStreamGetSize",0x80A470);
 Route* atexitRoute=add("CRT","CrtAtexit",0x821D1E);
 Route* lockRoute=add("CRT","CrtLockFile2",0x823FD1);
 Route* unlockRoute=add("CRT","CrtUnlockFile2",0x824023);
 Route* mallocRoute=add("CRT","CrtMalloc",0x824257);
 Route* errnoRoute=add("CRT","CrtErrno",0x82B181);
 bool ready=true;json bodies=json::object();
 for(const auto& e:entries){const bool ok=e.route->Registered()&&e.route->Body()&&ModuleOf(e.route->Body())==asi;ready&=ok;bodies[std::string(e.cls)+"::"+e.name]=e.route->Body();}
 const uintptr_t engine=ReadInt(kEngineInstance),ptd=GetPtd();
 ready&=engine!=0&&ptd!=0;
 // Calentamiento (fuera de la comparacion, con los hooks apagados): crea las secciones criticas de los 20 cerrojos de flujo (16 + n) y fija el estado del montón.
 if(ready){for(uint32 n=0;n<20;++n){RawLock(kLockStreamBase+n);RawUnlock(kLockStreamBase+n);}void* warm=nullptr;{const auto fn=uintptr_t(0x82422B);const uint32 argBytes=64,argFlag=0;__asm {push argFlag} __asm {push argBytes} __asm {call fn} __asm {add esp,8} __asm {mov warm,eax}}if(warm)RawFree(warm);}
 static CRITICAL_SECTION bankCs;InitializeCriticalSection(&bankCs);
 // Codificacion de LockCount de ESTE sistema operativo (sin otros hilos: la seccion es del banco): libre y tomada una vez sin espera. Windows 8+ usa -1 / -2; las anteriores -1 / 0.
 const int32 csUnlockedCount=bankCs.LockCount;EnterCriticalSection(&bankCs);const int32 csLockedCount=bankCs.LockCount;LeaveCriticalSection(&bankCs);   // seccion critica propia para los indices >= 20 de _lock_file2 (stream = &seccion - 0x20)
 std::vector<Watch> watches;
 for(uintptr_t at:{uintptr_t(0x8052F0),uintptr_t(0x807720),uintptr_t(0x820C90),uintptr_t(0x808470),uintptr_t(0x809400),uintptr_t(0x8094B0),uintptr_t(0x809510),uintptr_t(0x809530),uintptr_t(0x809540),uintptr_t(0x80A440),uintptr_t(0x80A470),
  uintptr_t(0x821D1E),uintptr_t(0x823FD1),uintptr_t(0x824023),uintptr_t(0x824257),uintptr_t(0x82B181),kOnexit})watches.emplace_back(at,16);
 auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 json watched=json::array();for(const auto& w:watches)watched.push_back({{"address",w.address},{"bytes",Hex(w.address,16)}});
 constexpr unsigned total=16*kCases;
 json caseCounts=json::object();for(const auto& e:entries)caseCounts[std::string(e.cls)+"::"+e.name]=kCases;
 Write({{"event","begin"},{"registered_hook",ready},{"functions",16},{"pairs",total},{"cases",caseCounts},{"compare_dead_x87_registers",kCompareDeadX87Registers},
  {"collaborators_stubbed",true},{"stubbed_collaborators",json::array({"rwfree [engine+0x138] (doble trazado, ambos lados)","rwstrlen [engine+0x120] (doble trazado, ambos lados)","_onexit 0x821CE6 (EntryPatch, ambos lados)","manejador de falta de memoria [0xC9AC68] (doble trazado)"})},
  {"real_collaborators",json::array({"_lock 0x82ADBE","_unlock 0x82AD12","_getptd 0x827B3D","_nh_malloc 0x82422B","EnterCriticalSection/LeaveCriticalSection"})},
  {"watched_entries",watched},{"bodies",bodies},{"asi_module",asi},{"original_module",gameExe},{"engine",engine},{"cs_unlocked_lock_count",csUnlockedCount},{"cs_locked_lock_count",csLockedCount},{"ptd",ptd},{"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR}});
 int pairs{};bool passed=ready;
 json functionResults=json::array();

 // Ejecuta los kCases casos de una funcion. `blocks`: memoria global tocada (se restaura antes de cada lado y se observa tras la llamada).
 auto runSection=[&](const char* function,Route* route,uintptr_t entry,std::vector<Block>& blocks,const std::function<Prepared(unsigned)>& prepare){
  int failures{};
  for(unsigned scenario=0;scenario<kCases&&ready;++scenario){
   std::array<std::array<uint8,108>,2> fpRaw{};json rows[2];json infos[2]={json::object(),json::object()};Result results[2]{};
   for(int side=0;side<2;++side){
    const bool candidate=side==1;
    RestoreFloatingState();for(const auto& b:blocks)b.Restore();trace.clear();
    Prepared p=prepare(scenario);
    alignas(16)std::array<uint8,108> fp{};uint32 mx{};bool routeOK{};Result result{};
    PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
    route->Invoke(candidate,[&]{
     routeOK=candidate?ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==asi:!ReversingProbe::JumpsToDll(entry);
     result=Gate(entry,p.args.data(),uint32(p.args.size()),Seed(scenario,0),Seed(scenario,1),Seed(scenario,2));
     SaveFP(fp.data());mx=ReadMXCSR();return 0;});
    fpRaw[side]=fp;results[side]=result;
    json state=json::array();for(const auto& b:blocks)state.push_back(b.Observe());
    json out={{"input",p.input},{"base",p.base},{"state",state},{"arena",p.arenaHex?p.arenaHex():std::string()},{"post_floating_state",Hex(uintptr_t(fp.data()),108)},{"post_mxcsr",mx},
     {"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOK},{"guards_ok",p.arenaGuardsOk?p.arenaGuardsOk():true}};
    const json extra=p.observe(result);
    for(auto it=extra.begin();it!=extra.end();++it)out[it.key()]=it.value();
    out["trace"]=trace;
    if(p.cleanup)p.cleanup();
    p.patches.clear();
    RestoreFloatingState();for(const auto& b:blocks)b.Restore();
    out["globals_restored"]=watchesIntact()&&std::ranges::all_of(blocks,[](const auto& b){return b.Restored();});out["floating_state_restored"]=FloatingStateRestored();
    // Campos "info_*": se registran pero no entran en la igualdad (propiedades del estado del montón del EXE, no de la función; ver README).
    for(const char* k:{"info_align16"})if(out.contains(k)){infos[side][k]=out[k];out.erase(k);}
    rows[side]=out;
   }
   // ECX/EDX al volver: ningun llamador del EXE los lee (ver g25e-classification.md); se registran sin exigir igualdad entre lados.
   bool ok=rows[0]==rows[1];
   for(const char* k:{"stack_restored","nonvolatile_preserved","route_ok","guards_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
   const auto fpDifference=FpDifference(fpRaw[0],fpRaw[1]);ok&=fpDifference.empty();
   passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"function",function},{"case",scenario},{"pair",pairs++},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",fpDifference},{"original_info",infos[0]},{"candidate_info",infos[1]},
    {"original_residue",{{"eax",results[0].eax},{"ecx",results[0].ecx},{"edx",results[0].edx}}},{"candidate_residue",{{"eax",results[1].eax},{"ecx",results[1].ecx},{"edx",results[1].edx}}},{"equal",ok},{"accepted",ok}});
  }
  Write({{"event","function_summary"},{"function",function},{"cases",int(kCases)},{"failures",failures},{"state_restored",watchesIntact()}});
 };

 // Fabrica de arenas: cada seccion tiene el suyo (direccion fija entre los dos lados y entre pasadas).
 static Arena<kFrameListInterior> frameListArena;static Arena<kPluginInterior> pluginArena;static Arena<kSListInterior> slistArena;static Arena<kStringCloseInterior> closeArena;
 static Arena<kStringInterior> stringArena;static Arena<kRasterInterior> rasterArena;static Arena<kAtexitInterior> atexitArena;
 const auto bindArena=[](Prepared& p,auto& arena){p.base=Ptr(arena.Interior_(),0);p.arenaHex=[&arena]{return arena.Hex_();};p.arenaGuardsOk=[&arena]{return arena.GuardsOk();};};
 const auto noEax=[](const Result&){return json::object();};
 const auto eaxOnly=[](const Result& r){return json{{"eax",r.eax}};};

 // ============================================================ RxHeapGetGlobalHeap (0x8052F0)
 {std::vector<Block> blocks;blocks.emplace_back(kRxHeapGlobal,4);
  runSection("RwCore::RxHeapGetGlobalHeap",rxHeap,0x8052F0,blocks,[&](unsigned c){Prepared p;PutInt(kRxHeapGlobal,RxHeapValue(c));p.input={{"case",c},{"value",RxHeapValue(c)}};p.observe=eaxOnly;return p;});}

 // ============================================================ _rwFrameListDeinitialize (0x807720)
 {std::vector<Block> blocks;blocks.emplace_back(engine+kEngineFreeOffset,4);
  runSection("RwCore::_rwFrameListDeinitialize",frameList,0x807720,blocks,[&](unsigned c){Prepared p;frameListArena.Reset();BuildFrameList(frameListArena.Interior_(),c);
   PutInt(engine+kEngineFreeOffset,uint32(uintptr_t(&FreeStub)));const auto f=FrameList(c);
   p.args={Ptr(frameListArena.Interior_(),kFrameListObject)};p.input={{"case",c},{"frames",f.frames},{"num_frames",f.numFrames}};bindArena(p,frameListArena);p.observe=eaxOnly;return p;});}

 // ============================================================ RwD3D9RasterIsCompressed (0x820C90)
 {std::vector<Block> blocks;blocks.emplace_back(kRasterExtOffset,4);
  runSection("RwCore::RwD3D9RasterIsCompressed",raster,0x820C90,blocks,[&](unsigned c){Prepared p;rasterArena.Reset();BuildRaster(rasterArena.Interior_(),c);const auto f=Raster(c);
   PutInt(kRasterExtOffset,f.delta);const uint32 flagAddress=Ptr(rasterArena.Interior_(),kRasterFlag);
   p.args={flagAddress-0xAu-f.delta};p.input={{"case",c},{"flags",f.flags},{"delta",f.delta}};bindArena(p,rasterArena);p.observe=eaxOnly;return p;});}

 // ============================================================ _rwPluginRegistryGetPluginOffset (0x808470)
 {std::vector<Block> blocks;
  runSection("RwPlCore::_rwPluginRegistryGetPluginOffset",plugin,0x808470,blocks,[&](unsigned c){Prepared p;pluginArena.Reset();BuildPlugin(pluginArena.Interior_(),c);const auto f=Plugin(c);
   p.args={Ptr(pluginArena.Interior_(),kPluginRegistry),f.query};
   p.input={{"case",c},{"n",f.n},{"mode",f.mode},{"query",f.query},{"slot",std::vector<unsigned>(f.slot,f.slot+8)},{"id",std::vector<uint32>(f.id,f.id+8)},{"offset",std::vector<uint32>(f.offset,f.offset+8)}};
   bindArena(p,pluginArena);p.observe=eaxOnly;return p;});}

 // ============================================================ _rwSList* (0x809400, 0x8094B0, 0x809510, 0x809530, 0x809540)
 const auto slistCase=[&](unsigned c,unsigned argCount){Prepared p;slistArena.Reset();BuildSList(slistArena.Interior_(),c);const auto f=SList(c);
   p.args={Ptr(slistArena.Interior_(),kSListObject)};if(argCount)p.args.push_back(f.arg);
   p.input={{"case",c},{"elements",f.elements},{"filled",f.filled},{"alloced",f.alloced},{"entry_size",f.entrySize},{"arg",f.arg}};bindArena(p,slistArena);return p;};
 {std::vector<Block> blocks;
  runSection("RwPlCore::_rwSListDestroyEndEntries",slDestroy,0x809400,blocks,[&](unsigned c){auto p=slistCase(c,1);p.observe=noEax;return p;});
  runSection("RwPlCore::_rwSListGetNumEntries",slNum,0x8094B0,blocks,[&](unsigned c){auto p=slistCase(c,0);p.observe=eaxOnly;return p;});
  runSection("RwPlCore::_rwSListGetEntry",slEntry,0x809510,blocks,[&](unsigned c){auto p=slistCase(c,1);p.observe=eaxOnly;return p;});
  runSection("RwPlCore::_rwSListGetBegin",slBegin,0x809530,blocks,[&](unsigned c){auto p=slistCase(c,0);p.observe=eaxOnly;return p;});
  runSection("RwPlCore::_rwSListGetEnd",slEnd,0x809540,blocks,[&](unsigned c){auto p=slistCase(c,0);p.observe=eaxOnly;return p;});}

 // ============================================================ _rwStringClose (0x80A440)
 {std::vector<Block> blocks;
  runSection("RwPlCore::_rwStringClose",strClose,0x80A440,blocks,[&](unsigned c){Prepared p;closeArena.Reset();BuildStringClose(closeArena.Interior_(),c);p.input={{"case",c}};bindArena(p,closeArena);p.observe=noEax;return p;});}

 // ============================================================ _rwStringStreamGetSize (0x80A470)
 {std::vector<Block> blocks;blocks.emplace_back(engine+kEngineStrlenOffset,4);
  runSection("RwPlCore::_rwStringStreamGetSize",strSize,0x80A470,blocks,[&](unsigned c){Prepared p;stringArena.Reset();BuildString(stringArena.Interior_(),c);const auto f=String(c);
   PutInt(engine+kEngineStrlenOffset,uint32(uintptr_t(&StrlenStub)));strlenValue=f.length;
   p.args={f.null?0u:Ptr(stringArena.Interior_(),kStringObject)};p.input={{"case",c},{"null",f.null},{"length",f.length},{"object",uint32(uintptr_t(stringArena.Interior_())+kStringObject)}};
   bindArena(p,stringArena);p.observe=eaxOnly;return p;});}

 // ============================================================ CrtAtexit (0x821D1E)
 {std::vector<Block> blocks;
  runSection("CRT::CrtAtexit",atexitRoute,0x821D1E,blocks,[&](unsigned c){Prepared p;atexitArena.Reset();BuildAtexit(atexitArena.Interior_(),c);const auto f=Atexit(c);
   onexitValue=f.result;p.patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(kOnexit,&OnexitStub));
   p.args={f.func};p.input={{"case",c},{"func",f.func},{"result",f.result}};bindArena(p,atexitArena);p.observe=eaxOnly;return p;});}

 // ============================================================ CrtLockFile2 (0x823FD1) / CrtUnlockFile2 (0x824023)
 // Estado observado: tabla de cerrojos del CRT, las 20 secciones criticas de flujo (16 + n) y la seccion critica del banco.
 {std::vector<Block> blocks;blocks.emplace_back(kLockTable,kLockTableBytes);
  for(uint32 n=0;n<20;++n){const uint32 cs=ReadInt(kLockTable+8*(kLockStreamBase+n));if(cs)blocks.emplace_back(cs,sizeof(CRITICAL_SECTION));else ready=false;}
  blocks.emplace_back(uintptr_t(&bankCs),sizeof(CRITICAL_SECTION));
  const auto criticalSection=[&](uint32 index)->CRITICAL_SECTION*{return index<20?reinterpret_cast<CRITICAL_SECTION*>(ReadInt(kLockTable+8*(kLockStreamBase+index))):&bankCs;};
  const auto release=[&](uint32 index){auto* cs=criticalSection(index);for(int i=0;i<8&&cs->RecursionCount>0;++i){if(index<20)RawUnlock(kLockStreamBase+index);else LeaveCriticalSection(cs);}};
  const auto lockObserve=[&](uint32 index,uint32 tid){return [=](const Result&){auto* cs=criticalSection(index);return json{{"lock_count",int32(cs->LockCount)},{"recursion",int32(cs->RecursionCount)},{"owner_is_this_thread",uint32(uintptr_t(cs->OwningThread))==tid},{"thread",tid}};};};
  runSection("CRT::CrtLockFile2",lockRoute,0x823FD1,blocks,[&](unsigned c){Prepared p;const auto f=Lock(c);const uint32 tid=GetCurrentThreadId();
   p.args={f.index,f.real?f.stream:uint32(uintptr_t(&bankCs)-0x20)};p.input={{"case",c},{"index",f.index},{"real",f.real},{"stream_is_section",!f.real}};
   p.observe=lockObserve(f.index,tid);p.cleanup=[=]{release(f.index);};return p;});
  runSection("CRT::CrtUnlockFile2",unlockRoute,0x824023,blocks,[&](unsigned c){Prepared p;const auto f=Lock(c);const uint32 tid=GetCurrentThreadId();
   if(f.real)RawLock(kLockStreamBase+f.index);else EnterCriticalSection(&bankCs);   // precondicion: el cerrojo esta tomado una vez por este hilo
   p.args={f.index,f.real?f.stream:uint32(uintptr_t(&bankCs)-0x20)};p.input={{"case",c},{"index",f.index},{"real",f.real},{"stream_is_section",!f.real}};
   p.observe=lockObserve(f.index,tid);p.cleanup=[=]{release(f.index);};return p;});}

 // ============================================================ CrtMalloc (0x824257)
 {std::vector<Block> blocks;blocks.emplace_back(kNewMode,4);blocks.emplace_back(kNewHandler,4);blocks.emplace_back(ptd+8,4);
  runSection("CRT::CrtMalloc",mallocRoute,0x824257,blocks,[&](unsigned c){Prepared p;const auto f=Malloc(c);
   PutInt(kNewMode,f.newmode);PutInt(kNewHandler,uint32(uintptr_t(&NewHandlerStub)));
   p.args={f.size};p.input={{"case",c},{"success",f.success},{"size",f.size},{"newmode",f.newmode}};
   // El bloque se escribe y se libera tras capturar el x87 y el registro; solo se registran clases del puntero (no el valor).
   p.observe=[=](const Result& r){void* block=reinterpret_cast<void*>(uintptr_t(r.eax));json out={{"null",block==nullptr},{"info_align16",r.eax&15u},{"align8",r.eax&7u}};
    if(block){memset(block,0xCD,std::min<size_t>(f.size,0x10000));RawFree(block);}return out;};
   return p;});}

 // ============================================================ CrtErrno (0x82B181)
 {std::vector<Block> blocks;blocks.emplace_back(ptd+8,4);
  runSection("CRT::CrtErrno",errnoRoute,0x82B181,blocks,[&](unsigned c){Prepared p;PutInt(ptd+8,ErrnoValue(c));p.input={{"case",c},{"value",ErrnoValue(c)}};
   p.observe=[=](const Result& r){return json{{"eax_minus_ptd",uint32(r.eax-uint32(ptd))},{"readback",r.eax==uint32(ptd+8)?ReadInt(r.eax):0xDEADBEEFu}};};return p;});}

 // Las vigilancias se tomaron con los hooks apagados (el constructor de HookRoute los apaga): se comprueban ANTES de Restore(), que vuelve a encender los hooks.
 const bool entriesIntactBefore=watchesIntact();
 bool stateRestored=entriesIntactBefore;
 for(auto& e:entries)stateRestored&=e.route->Restore();
 DeleteCriticalSection(&bankCs);
 RestoreFloatingState();stateRestored&=FloatingStateRestored();
 Write({{"event","global_state_final"},{"floating_state",Hex(uintptr_t(savedFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntactBefore}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntactBefore}});
 RestoreFloatingState();savedFPActive=false;
}
}
