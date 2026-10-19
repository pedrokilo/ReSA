#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "RealTimeShadow.h"
#include "ShadowCamera.h"
#include "RwHelper.h"
#include "Entity/Physical.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par RTShadowSetup28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CRealTimeShadow::SetupForThisEntity (0x706520) con su HookRoute. Los seis colaboradores sustituidos estan descritos en Core.hpp y README.md.
static_assert(sizeof(CRealTimeShadow) == RTShadowFix::kShadowBytes, "el banco trabaja con sizeof(CRealTimeShadow) == 0x4C a mano");
static_assert(offsetof(CRealTimeShadow, m_pOwner) == 0x00 && offsetof(CRealTimeShadow, m_camera) == 0x08 && offsetof(CRealTimeShadow, m_nRwObjectType) == 0x24
              && offsetof(CRealTimeShadow, m_boundingSphere) == 0x2C && offsetof(CRealTimeShadow, m_baseSphere) == 0x3C, "disposicion de CRealTimeShadow segun el EXE");
static_assert(offsetof(RpAtomic, boundingSphere) == 0x1C && offsetof(RpAtomic, interpolator) == 0x4C, "disposicion de RpAtomic segun el EXE");
namespace RTShadow28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"rtshadowsetup28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"rtshadowsetup28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("rtshadowsetup28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CRealTimeShadow","SetupForThisEntity",0x706520,HookRoute::RegisteredTarget{}));
    registered["CRealTimeShadow::SetupForThisEntity"]=env.route->Registered();bodies["CRealTimeShadow::SetupForThisEntity"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    auto B=[](auto fp){return Body(std::bit_cast<uintptr_t>(fp));};
    env.pDestroy={0x705990,B(&CRealTimeShadow::Destroy)};
    env.pResync={0x7491F0,B(&_rpAtomicResyncInterpolatedSphere)};
    env.pClump={0x734FC0,B(&RpClumpGetBoundingSphere)};
    env.pXform={0x7EDD90,B(static_cast<RwV3d*(*)(RwV3d*,const RwV3d*,RwInt32,const RwMatrix*)>(&RwV3dTransformPoints))};
    env.pFrustum={0x7054C0,B(&CShadowCamera::SetFrustum)};
    env.pCenter={0x705590,B(&CShadowCamera::SetCenter)};
    Run(env,ready,registered,bodies);
}
}
