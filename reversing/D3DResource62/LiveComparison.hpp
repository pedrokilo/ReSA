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

// Banco par a par D3DResource62 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Siete rutas: CancelBuffering (0x730900), CreateIndexBuffer (0x7306A0), CreateTexture (0x730510), DestroyIndexBuffer (0x730D30), DestroyTexture (0x730B70), Init (0x730830) y Shutdown (0x730A00).

// Disposicion de los objetos reales frente a las constantes de Fixtures.hpp.
namespace D3D62Layout {
static_assert(sizeof(D3DTextureBuffer) == 0x1C && offsetof(D3DTextureBuffer, m_nFormat) == 0 && offsetof(D3DTextureBuffer, m_nWidth) == 4 && offsetof(D3DTextureBuffer, m_nLevels) == 8
              && offsetof(D3DTextureBuffer, m_nCapcacity) == 0xC && offsetof(D3DTextureBuffer, m_nNumTexturesInBuffer) == 0x10 && offsetof(D3DTextureBuffer, m_nSize) == 0x14
              && offsetof(D3DTextureBuffer, m_apTextures) == 0x18, "D3DTextureBuffer");
static_assert(sizeof(D3DIndexDataBuffer) == 0x18 && offsetof(D3DIndexDataBuffer, m_nFormat) == 0 && offsetof(D3DIndexDataBuffer, field_4) == 4 && offsetof(D3DIndexDataBuffer, m_nCapcacity) == 8
              && offsetof(D3DIndexDataBuffer, m_nNumDatasInBuffer) == 0xC && offsetof(D3DIndexDataBuffer, m_nSize) == 0x10 && offsetof(D3DIndexDataBuffer, m_apIndexData) == 0x14, "D3DIndexDataBuffer");
}

namespace D3D62Live {
inline bool LayoutMatches() {
    using S = D3DResourceSystem;
    return uintptr_t(&S::NumTextureBuffers) == kRegionBase && uintptr_t(&S::TextureBuffers[0]) == kRegionBase + 4 * kTexBase
        && uintptr_t(&S::IndexDataBuffers[0]) == kRegionBase + 4 * kIbBase && uintptr_t(&S::IndexDataBuffer) == kRegionBase + 4 * kGenBase
        && uintptr_t(&S::FreeTextureBufferIndex) == kRegionBase + 4 * kFreeTexWord && uintptr_t(&S::FreeIndexBufferIndex) == kRegionBase + 4 * kFreeIbWord
        && uintptr_t(&S::UseD3DResourceBuffering) == kUseAddr && sizeof(S::TextureBuffers) == kTexBuffers * 0x1C && sizeof(S::IndexDataBuffers) == kIbBuffers * 0x18;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"d3dresource62-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"d3dresource62-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("d3dresource62");}
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
    // Colaboradores del EXE sin reconstruir (ninguno tiene hook en la DLL): el orden importa (new/delete los ultimos: ver Patches).
    env.stubs={{Pair{0x72FF60,0,false},reinterpret_cast<void*>(&FkTexPop)},
               {Pair{0x72FE80,0,false},reinterpret_cast<void*>(&FkTexSetup)},
               {Pair{0x72FFF0,0,false},reinterpret_cast<void*>(&FkTexPush)},
               {Pair{0x730AD0,0,false},reinterpret_cast<void*>(&FkTexPushWithout)},
               {Pair{0x730020,0,false},reinterpret_cast<void*>(&FkTexResize)},
               {Pair{0x730270,0,false},reinterpret_cast<void*>(&FkIbPop)},
               {Pair{0x730330,0,false},reinterpret_cast<void*>(&FkIbResize)},
               {Pair{0x82119A,0,false},reinterpret_cast<void*>(&FkNew)},
               {Pair{0x8214BD,0,false},reinterpret_cast<void*>(&FkDelete)}};
    json stubbed=json::array();
    for(const auto& s:env.stubs) {
        env.watches.emplace_back(s.pair.exe,16);
        stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
    }
    for(unsigned f=0;f<kFns;++f)collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    {   // la arena del banco debe quedar por debajo de las fichas de basura (0xC0000000..) y la disposicion de las clases coincidir con Fixtures.hpp
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        const bool layout=LayoutMatches();
        if(!arena||!layout)Write({{"event","layout_error"},{"reason",!arena?"arena del banco por encima de 0x80000000":"disposicion de D3DResourceSystem distinta de Fixtures.hpp"}});
        ready&=arena&&layout;
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
