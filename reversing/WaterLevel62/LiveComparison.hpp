#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "WaterLevel.h"
#include "Camera.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par WaterLevel62 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.
// Cinco rutas: CWaterLevel::FillQuadsAndTrianglesList (0x6E7B30), GetWaterDepth (0x6EA960), RenderHighDetailWaterRectangle (0x6EB810), RenderHighDetailWaterTriangle (0x6EDDC0)
// y TestLineAgainstWater (0x6E61B0), todas con hook Simple (RH_ScopedGlobalInstall).

// Disposicion de los objetos reales frente a las constantes de Fixtures.hpp.
namespace Water62Layout {
static_assert(sizeof(CWaterVertex) == Water62Fix::kVertexBytes && offsetof(CWaterVertex, x) == 0 && offsetof(CWaterVertex, y) == 2, "CWaterVertex");
static_assert(sizeof(CWaterQuad) == Water62Fix::kQuadBytes, "CWaterQuad");
static_assert(sizeof(CWaterTriangle) == Water62Fix::kTriBytes, "CWaterTriangle");
static_assert(sizeof(CWaterLevel::PolyInfo) == 2 && sizeof(CRenPar) == 16 && offsetof(CRenPar, z) == 0 && offsetof(CRenPar, bigWaves) == 4 && offsetof(CRenPar, smallWaves) == 8
              && offsetof(CRenPar, flowX) == 12 && offsetof(CRenPar, flowY) == 13, "CRenPar / PolyInfo");
static_assert(std::tuple_size_v<std::remove_reference_t<decltype(CWaterLevel::m_aVertices)>> == Water62Fix::kVertexCount && std::tuple_size_v<std::remove_reference_t<decltype(CWaterLevel::WaterQuads)>> == Water62Fix::kQuadCount
              && std::tuple_size_v<std::remove_reference_t<decltype(CWaterLevel::WaterTriangles)>> == Water62Fix::kTriCount && std::tuple_size_v<std::remove_reference_t<decltype(CWaterLevel::m_PolyCombos)>> == Water62Fix::kComboWords, "tamanos de los arreglos");
}

namespace Water62Live {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline bool LayoutOk() {
    const bool atlas=uintptr_t(CWaterLevel::m_aVertices.data())==0xC22910&&uintptr_t(CWaterLevel::WaterQuads.data())==0xC21C90&&uintptr_t(CWaterLevel::WaterTriangles.data())==0xC22854
        &&uintptr_t(&CWaterLevel::m_BlockPolyInfo)==0xC21B70&&uintptr_t(CWaterLevel::m_PolyCombos.data())==0xC215F8
        &&uintptr_t(&CWaterLevel::NumWaterTriangles)==0xC22884&&uintptr_t(&CWaterLevel::NumWaterQuads)==0xC22888&&uintptr_t(&CWaterLevel::NumWaterVertices)==0xC2288C;
    const bool camera=uintptr_t(&TheCamera)==0xB6F028&&uintptr_t(&TheCamera.m_bMirrorActive)==0xB6F998&&uintptr_t(&TheCamera.m_mMatInverse)==0xB6FA74&&uintptr_t(&TheCamera.m_mMatMirrorInverse)==0xB6FABC;
    if(!atlas||!camera)Write({{"event","layout_error"},{"reason","disposicion del atlas o de la camara distinta de Fixtures.hpp / del EXE"},{"atlas",atlas},{"camera",camera}});
    return atlas&&camera;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"waterlevel62-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"waterlevel62-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("waterlevel62");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH: se construyen antes que nada; el constructor deja el hook apagado.
    auto simple=[&](unsigned idx,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute("CWaterLevel",fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"FillQuadsAndTrianglesList",0x6E7B30);
    simple(1,"GetWaterDepth",0x6EA960);
    simple(2,"RenderHighDetailWaterRectangle",0x6EB810);
    simple(3,"RenderHighDetailWaterTriangle",0x6EDDC0);
    simple(4,"TestLineAgainstWater",0x6E61B0);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    using Sphere = bool(CCamera::*)(const CVector&,float,RwMatrix*);
    const Pair sphere{0x420C40,DllBody(static_cast<Sphere>(&CCamera::IsSphereVisible)),true};
    env.stubs[0]={{Pair{0x6E5750,0,false},reinterpret_cast<void*>(&FakeAdd)}};   // AddToQuadsAndTrianglesList: sin reconstruir (la DLL llama a la direccion del EXE)
    env.stubs[1]={{Pair{0x6E8580,DllBody(&CWaterLevel::GetWaterLevelNoWaves),true},reinterpret_cast<void*>(&FakeLevel)},
                  {Pair{0x6EA8A0,0,false},reinterpret_cast<void*>(&FakeGround)}};   // GetGroundLevel: sin reconstruir
    env.stubs[2]={{sphere,reinterpret_cast<void*>(&FakeSphere)},
                  {Pair{0x6EA9F0,DllBody(&CWaterLevel::SetUpWaterFog),true},reinterpret_cast<void*>(&FakeFog)},
                  {Pair{0x6E91D0,0,false},reinterpret_cast<void*>(&FakeRectLayer)},   // RenderHighDetailWaterRectangle_OneLayer: sin reconstruir
                  {Pair{0x6E73A0,DllBody(&CWaterLevel::SplitWaterRectangleAlongXLine),true},reinterpret_cast<void*>(&FakeSplitX)},
                  {Pair{0x6EC5D0,DllBody(&CWaterLevel::RenderWaterRectangle),true},reinterpret_cast<void*>(&FakeRenderRect)}};
    env.stubs[3]={{sphere,reinterpret_cast<void*>(&FakeSphere)},
                  {Pair{0x6E8780,0,false},reinterpret_cast<void*>(&FakeTriLayer)},   // RenderHighDetailWaterTriangle_OneLayer: sin reconstruir
                  {Pair{0x6ECF00,DllBody(&CWaterLevel::SplitWaterTriangleAlongXLine),true},reinterpret_cast<void*>(&FakeSplitTri)}};
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
