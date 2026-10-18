// Arnes aislado del banco Dependencies25C (sin juego): ejecuta los bytes REALES de las 20 funciones del EXE en sus direcciones verdaderas (memoria reservada en
// 0x700000..0x8FFFFF y 0xC90000..0xCAFFFF del proceso del arnes) frente a la reconstruccion C++ compilada de los fuentes parcheados (rpworld.obj, rphanim.obj, ..., enlazados),
// con los mismos colaboradores falsos, las mismas fixtures y el mismo registro (Core.hpp) que la sonda dentro del juego. Escribe el jsonl que run_local.py --analyze valida.
// Esto NO es una corrida dentro del juego: no ejercita el hook RH real ni la ruta EXE -> thunk -> DLL; es evidencia previa.
#include "StdInc.h"
#include "../Core.hpp"
#include "exebytes.inc"
#include <fstream>

using namespace Dependencies25CCore;
extern "C" unsigned char g_exeMem[0xA00000];

struct OfflineEnv : Env {
 static bool InExe(uintptr_t a){return a>=uintptr_t(g_exeMem)&&a<uintptr_t(g_exeMem)+sizeof(g_exeMem);}
 uintptr_t Entry(unsigned fn,bool candidate) override {return candidate?Fns()[fn].dll:Fns()[fn].entry;}
 void Invoke(unsigned,bool,const std::function<void()>& call) override {call();}
 bool IsDll(uintptr_t address) override {return !InExe(address);}
 bool RouteOK(unsigned fn,bool candidate) override {
  const uintptr_t entry=Entry(fn,candidate);
  return candidate?IsDll(Resolve(entry)):InExe(Resolve(entry));   // el original ejecuta bytes del EXE; el candidato, codigo del arnes (la DLL reconstruida)
 }
};

int main(int argc,char** argv) {
 const char* outPath=argc>1?argv[1]:"comparisons.jsonl";
 // Memoria del "EXE": la seccion .exemem de exemem.cpp (RWX, 10 MB) cubre las direcciones del EXE usadas por las funciones, sus colaboradores y las variables globales.
 if(uintptr_t(g_exeMem)>0x700000||uintptr_t(g_exeMem)+sizeof(g_exeMem)<0xCA0000){std::fprintf(stderr,"la seccion .exemem no cubre las direcciones del EXE (%p)\n",static_cast<void*>(g_exeMem));return 2;}
 for(const auto& f:kExeFunctions)std::memcpy(reinterpret_cast<void*>(f.address),f.bytes,f.size);
 std::ofstream log(outPath);
 auto write=[&](const json& row){RestoreFloatingState();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();};
 OfflineEnv env;
 const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(g_exeMem);   // en el arnes, "el EXE" es la seccion .exemem de la propia imagen
 InitCollabTable();const auto& table=collabTable;
 bool ready=true;
 for(const auto& f:Fns())ready=ready&&env.IsDll(Resolve(f.dll));
 for(const auto& c:table)ready=ready&&OfflineEnv::InExe(c.exe)&&(!c.dll||env.IsDll(c.dll));
 bool noCollision=true;{const uint32 lo=uint32(uintptr_t(ArenaBase()))-0x100,hi=lo+kArena+0x200;
  for(unsigned fn=0;fn<kFnCount;++fn)for(unsigned c=0;c<kCases[fn];++c){const Fx f=Make(fn,c);
   for(unsigned i=0;i<f.n;++i)if(f.a[i].kind==kRaw&&f.a[i].v>=lo&&f.a[i].v<hi)noCollision=false;
   for(uint32 v:f.x)if(v>=lo&&v<hi)noCollision=false;
   if(f.ret>=lo&&f.ret<hi)noCollision=false;}}
 ready=ready&&noCollision;
 std::vector<Watch> watches;
 for(const auto& f:Fns())watches.emplace_back(f.entry,16);
 auto isFnEntry=[&](uintptr_t a){for(const auto& f:Fns())if(f.entry==a)return true;return false;};
 for(const auto& c:table){if(!isFnEntry(c.exe))watches.emplace_back(c.exe,16);if(c.dll)watches.emplace_back(c.dll,16);}
 auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 const uint32 engineInitial=Var(kEngineInstanceVar),hanimInitial=Var(kHAnimOffsetVar);
 json watchedJson=json::array();for(const auto& w:watches)watchedJson.push_back({{"address",w.address},{"bytes",Hex(w.address,16)}});
 json cases=json::object(),bodies=json::object(),names=json::array();
 for(unsigned fn=0;fn<kFnCount;++fn){cases[FullName(fn)]=kCases[fn];bodies[FullName(fn)]=Fns()[fn].dll;names.push_back(FullName(fn));}
 write({{"event","begin"},{"offline",true},{"registered_hook",ready},{"functions",kFnCount},{"pairs",kTotalPairs},{"cases",cases},{"compare_dead_x87_registers",kCompareDeadX87Registers},
  {"collaborators_stubbed",true},{"no_raw_collision",noCollision},{"watched_entries",watchedJson},{"bodies",bodies},{"names",names},{"asi_module",asi},{"original_module",gameExe},
  {"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},{"engine_instance",engineInitial},{"hanim_offset",hanimInitial}});
 int pairs{};
 const bool passed=RunComparisons(env,write,watchesIntact,ready,pairs);
 const bool entriesIntact=watchesIntact();
 bool stateRestored=entriesIntact&&Var(kEngineInstanceVar)==engineInitial&&Var(kHAnimOffsetVar)==hanimInitial;
 RestoreFloatingState();stateRestored&=FloatingStateRestored();
 write({{"event","global_state_final"},{"floating_state",Hex(uintptr_t(savedFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
  {"engine_instance",Var(kEngineInstanceVar)},{"hanim_offset",Var(kHAnimOffsetVar)}});
 write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
 std::printf("pares: %d, aprobado: %d\n",pairs,int(passed&&stateRestored));
 return passed&&stateRestored?0:1;
}
