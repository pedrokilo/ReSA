#pragma once
#include <fstream>
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par de 20 funciones de biblioteca de RenderWare (EXE con el hook RH apagado frente a la reconstruccion C++ con el hook encendido, dentro del juego):
//   RpWorld   _rpMeshHeaderCreate 0x758920, RpBuildMeshGenerateDefaultTriStrip 0x7591B0, RxNodeDefinitionGetD3D9WorldSectorAllInOne 0x75E9F0,
//             RpWorldSectorRegisterPlugin 0x761C90, RpWorldSectorRegisterPluginStream 0x761CC0
//   RpHAnim   RpHAnimFrameGetHierarchy 0x7C5160              RpSkin  _rpSkinDeinitialize 0x7C8820
//   RpUVAnim  _rpUVAnimCustomDataStreamGetSize 0x7CC010, RpUVAnimKeyFrameStreamGetSize 0x7CC9D0
//   RtAnim    RtAnimAnimationGetNumNodes 0x7CD4F0, RtAnimInterpolatorDestroy 0x7CD590
//   RtDict    RtDictSchemaDestruct 0x7CED70, RtDictSchemaGetCurrentDict 0x7CEEE0
//   RwPlCore  RwStreamWriteInt32 0x7ED460, RwMatrixOrthoNormalize 0x7F1920, RwEngineRegisterPlugin 0x7F2BB0, RwEngineGetPluginOffset 0x7F2BE0
//   RwCore    RwCameraSetViewOffset 0x7EE1A0, RwCameraRegisterPlugin 0x7EE450, RwFrameDirty 0x7F0340
// Los objetos son arenas propios del banco (Core.hpp). Colaboradores (RwMalloc/RwFree del motor por un RwGlobals falso; _rwPluginRegistryAdd*/GetPluginOffset, RwStreamWrite,
// _rwSListDestroy, RwFrameUpdateObjects, la rutina 0x7591D0 de malla y la de 0x7F1930 de ortonormalizacion) se sustituyen en AMBOS lados por el mismo falso trazado (EntryPatch en la
// direccion del EXE y en la entrada del envoltorio de la DLL). La traza forma parte de la comparacion.
namespace Dependencies25CLive {
using namespace Dependencies25CCore;
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}

struct LiveEnv : Env {
 std::vector<std::unique_ptr<OriginalDependencies499::HookRoute>> routes;
 uintptr_t asi;
 LiveEnv():asi(ModuleOf(uintptr_t(&ModuleMarker))) {
  // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado.
  for(const auto& f:Fns())routes.push_back(std::make_unique<OriginalDependencies499::HookRoute>(f.cls,f.name,f.entry,reinterpret_cast<void*>(f.dll)));
 }
 uintptr_t Entry(unsigned fn,bool) override {return Fns()[fn].entry;}
 void Invoke(unsigned fn,bool candidate,const std::function<void()>& call) override {routes[fn]->Invoke(candidate,[&]{call();return 0;});}
 bool IsDll(uintptr_t address) override {return ModuleOf(address)==asi;}
 bool RouteOK(unsigned fn,bool candidate) override {
  const uintptr_t entry=Fns()[fn].entry;
  return candidate?ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==asi:!ReversingProbe::JumpsToDll(entry);
 }
};

inline void Update() {
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencies25c-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies25c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies25c");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(GetModuleHandleA(nullptr));
 // Entradas de la DLL de los colaboradores: ANTES de construir las rutas (LiveEnv apaga los hooks propios; ver Core.hpp, InitCollabTable).
 InitCollabTable();const auto& table=collabTable;
 LiveEnv env;
 // Motivos exactos si ready es false (evento begin: not_ready_hooks / not_ready_collaborators / no_raw_collision).
 json notReadyHooks=json::array(),notReadyCollaborators=json::array();
 bool ready=true;for(unsigned fn=0;fn<kFnCount;++fn){const bool reg=env.routes[fn]->Registered(),body=ModuleOf(env.routes[fn]->Body())==asi;
  if(!reg||!body)notReadyHooks.push_back({{"function",FullName(fn)},{"registered",reg},{"body_in_asi",body},{"body",env.routes[fn]->Body()}});
  ready=ready&&reg&&body;}
 // Colaboradores: si la DLL tiene un envoltorio propio (c.dll) basta con que sea codigo de la sonda (el EXE puede o no estar enlazado a otra reconstruccion de otro grupo: se parchean las dos entradas);
 // si no (0x7591D0 y 0x7F1930, que la DLL llama por direccion), la entrada debe ser codigo del EXE sin enlazar y se parchea una sola vez.
 bool collaboratorsOK=true;for(const auto& c:table){const bool ok=c.dll?ModuleOf(c.dll)==asi:(!ReversingProbe::JumpsToDll(c.exe)&&ModuleOf(Resolve(c.exe))==gameExe);
  if(!ok)notReadyCollaborators.push_back({{"id",c.id},{"exe",c.exe},{"dll",c.dll},{"dll_in_asi",c.dll?ModuleOf(c.dll)==asi:false},{"exe_jumps_to_dll",ReversingProbe::JumpsToDll(c.exe)},{"exe_module",ModuleOf(Resolve(c.exe))}});
  collaboratorsOK=collaboratorsOK&&ok;}
 ready=ready&&collaboratorsOK;
 // Ningun valor crudo de las fixtures cae dentro del arena (el registro de la traza distingue "dentro del arena" por el valor).
 bool noCollision=true;{const uint32 lo=uint32(uintptr_t(ArenaBase()))-0x100,hi=lo+kArena+0x200;
  for(unsigned fn=0;fn<kFnCount;++fn)for(unsigned c=0;c<kCases[fn];++c){const Fx f=Make(fn,c);
   for(unsigned i=0;i<f.n;++i)if(f.a[i].kind==kRaw&&f.a[i].v>=lo&&f.a[i].v<hi)noCollision=false;
   for(uint32 v:f.x)if(v>=lo&&v<hi)noCollision=false;
   if(f.ret>=lo&&f.ret<hi)noCollision=false;}}
 ready=ready&&noCollision;
 std::vector<Watch> watches;
 for(const auto& f:Fns())watches.emplace_back(f.entry,16);
 // Los colaboradores que son a la vez una de las 20 funciones (0x7CC010) ya estan vigilados arriba: Restore() vuelve a enlazar su hook, que no es un cambio.
 auto isFnEntry=[&](uintptr_t a){for(const auto& f:Fns())if(f.entry==a)return true;return false;};
 for(const auto& c:table){if(!isFnEntry(c.exe))watches.emplace_back(c.exe,16);if(c.dll)watches.emplace_back(c.dll,16);}
 auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 const uint32 engineInitial=Var(kEngineInstanceVar),hanimInitial=Var(kHAnimOffsetVar);
 json watched=json::array();for(const auto& w:watches)watched.push_back({{"address",w.address},{"bytes",Hex(w.address,16)}});
 json cases=json::object(),bodies=json::object(),names=json::array();
 for(unsigned fn=0;fn<kFnCount;++fn){cases[FullName(fn)]=kCases[fn];bodies[FullName(fn)]=env.routes[fn]->Body();names.push_back(FullName(fn));}
 Write({{"event","begin"},{"registered_hook",ready},{"functions",kFnCount},{"pairs",kTotalPairs},{"cases",cases},{"compare_dead_x87_registers",kCompareDeadX87Registers},
  {"collaborators_stubbed",true},{"no_raw_collision",noCollision},{"not_ready_hooks",notReadyHooks},{"not_ready_collaborators",notReadyCollaborators},{"watched_entries",watched},{"bodies",bodies},{"names",names},{"asi_module",asi},{"original_module",gameExe},
  {"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},{"engine_instance",engineInitial},{"hanim_offset",hanimInitial}});
 int pairs{};
 const bool passed=RunComparisons(env,Write,watchesIntact,ready,pairs);
 // Las vigilancias se tomaron con los hooks apagados: se comprueban ANTES de Restore(), que vuelve a encender los hooks a su estado inicial.
 const bool entriesIntactBefore=watchesIntact();
 bool stateRestored=entriesIntactBefore;for(auto& r:env.routes)stateRestored&=r->Restore();
 for(size_t i=kFnCount;i<watches.size();++i)stateRestored&=watches[i].Intact();
 stateRestored&=Var(kEngineInstanceVar)==engineInitial&&Var(kHAnimOffsetVar)==hanimInitial;
 RestoreFloatingState();stateRestored&=FloatingStateRestored();
 Write({{"event","global_state_final"},{"floating_state",Hex(uintptr_t(savedFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntactBefore},
  {"engine_instance",Var(kEngineInstanceVar)},{"hanim_offset",Var(kHAnimOffsetVar)}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntactBefore}});
 RestoreFloatingState();savedFPActive=false;
}
}
