#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "RenderWare/D3DResourceSystem.h"
#include "RenderWare/D3DTextureBuffer.h"
#include "RenderWare/D3DIndexDataBuffer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par D3DResource63 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dos rutas: GetTotalPixelsSize (0x730660) y GetTotalIndexDataSize (0x7307F0).

// Disposicion de los objetos reales frente a las constantes de Fixtures.hpp.
namespace D3D63Layout {
static_assert(sizeof(D3DTextureBuffer) == 0x1C && sizeof(D3DIndexDataBuffer) == 0x18, "tamanos de los buffers");
}

namespace D3D63Live {
inline bool LayoutMatches() {
    using S = D3DResourceSystem;
    return uintptr_t(&S::NumTextureBuffers) == kRegionBase && uintptr_t(&S::TextureBuffers[0]) == kTexBufferBase && uintptr_t(&S::IndexDataBuffers[0]) == kIbBufferBase
        && uintptr_t(&S::IndexDataBuffer) == kGenericIb && sizeof(S::IndexDataBuffers) == kIbBuffers * kIbStride && sizeof(D3DTextureBuffer) == kTexStride && sizeof(D3DIndexDataBuffer) == kIbStride;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"d3dresource63-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"d3dresource63-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("d3dresource63");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH: se construyen antes que nada; el constructor deja el hook apagado.
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("D3DResourceSystem",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Colaboradores del EXE sin reconstruir (ninguno tiene hook en la DLL).
    env.stubs={{Pair{0x7300A0,0,false},reinterpret_cast<void*>(&FkTexTotal)},
               {Pair{0x7303B0,0,false},reinterpret_cast<void*>(&FkIbTotal)}};
    json stubbed=json::array();
    for(const auto& s:env.stubs) {
        env.watches.emplace_back(s.pair.exe,16);
        stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
    }
    for(unsigned f=0;f<kFns;++f)collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    {
        const bool layout=LayoutMatches();
        if(!layout)Write({{"event","layout_error"},{"reason","disposicion de D3DResourceSystem distinta de Fixtures.hpp"}});
        ready&=layout;
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
