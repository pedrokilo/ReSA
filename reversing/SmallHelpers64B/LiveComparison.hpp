#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "Population.h"
#include "PopCycle.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par SmallHelpers64B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.
// Tres rutas, todas con hook Simple (RH_ScopedGlobalInstall): GetPopcycleGroupPedModel (CGangWars, 0x406E80), FlushWaterRenderBuffer (CWaterLevel, 0x6E7680)
// y CTaskComplex::Destructor (0x61A3D0, __fastcall con EDX sin usar == thiscall sin argumentos de pila).

namespace Small64Layout {
static_assert(sizeof(CPopulation::m_TranslationArray) == Small64Fix::kTransMain && sizeof(tPedGroupTranslationData) == 12, "m_TranslationArray");
static_assert(sizeof(CPopulation::m_PedGroups) == Small64Fix::kPedMain, "m_PedGroups");
static_assert(POPCYCLE_TOTAL_GROUPS == Small64Fix::kNumGroups && POPCYCLE_TOTAL_PEDGROUPS == Small64Fix::kNumPedGroups, "numero de grupos");
}

namespace Small64Live {
inline bool LayoutOk() {
    const bool pop=uintptr_t(&CPopulation::m_TranslationArray)==kTransBase&&uintptr_t(&CPopulation::m_PedGroups)==kPedBase;
    const bool buffers=uintptr_t(&uiTempBufferVerticesStored)==kAddrVtx&&uintptr_t(&aTempBufferIndices)==kAddrIndices&&uintptr_t(&TempBufferVertices)==kAddrVerts
        &&uintptr_t(&g_nNumIm3dDrawCalls)==kAddrDraw&&kAddrIdx==kAddrVtx+4;
    if(!pop||!buffers)Write({{"event","layout_error"},{"reason","disposicion de las tablas o del bufer distinta de Fixtures.hpp / del EXE"},{"pop",pop},{"buffers",buffers}});
    return pop&&buffers;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"smallhelpers64b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"smallhelpers64b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("smallhelpers64b");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH: se construyen antes que nada; el constructor deja el hook apagado.
    auto simple=[&](unsigned idx,const char* cls,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute(cls,fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"CGangWars","GetPopcycleGroupPedModel",0x406E80);
    simple(1,"CWaterLevel","FlushWaterRenderBuffer",0x6E7680);
    simple(2,"CTaskComplex","Destructor",0x61A3D0);
    // Colaboradores sustituidos (RenderWare, sin hook ni reconstruccion: la DLL los llama por direccion del EXE): RwIm3DTransform, RwIm3DRenderIndexedPrimitive, RwIm3DEnd.
    env.stubs[1]={{Pair{0x7EF450,0,false},reinterpret_cast<void*>(&FakeTransform)},
                  {Pair{0x7EF550,0,false},reinterpret_cast<void*>(&FakeRender)},
                  {Pair{0x7EF520,0,false},reinterpret_cast<void*>(&FakeEnd)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    ready&=LayoutOk();
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
