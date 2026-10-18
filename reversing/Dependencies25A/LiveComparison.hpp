#pragma once
#include <fstream>
#include <memory>
#include "Garages.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "CutsceneMgr.h"
#include "VisibilityPlugins.h"
#include "RwHelper.h"
#include "AutoPilot.h"
#include "RenderWare/rw/rphanim.h"
#include "RenderWare/rw/rwcore.h"
#include "Tasks/Task.h"
#include "Pools/VehiclePool.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "DecisionMakers/Decision.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Dependencies25A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace Dependencies25ALive {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependencies25a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependencies25a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies25a");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja cada hook apagado. 0x734A70 no tiene hook (ruta nula): el candidato es la funcion de la DLL llamada directamente.
    struct Row {const char* cls;const char* name;uintptr_t address;};
    static constexpr Row rows[kFunctions]{
        {"CTask","Destructor",0x61A3A0},{"CVehiclePool","GetAt",0x41CC10},{"CAutoPilot","RemoveOnePathNode",0x41B950},{"CPedGroups","RemoveAllFollowersFromGroup",0x5FB8A0},
        {"CDecisionMakerTypes","FlushDecisionMakerEventResponse",0x604490},{"CGarages","CountCarsInHideoutGarage",0x44A210},{"CVehicle","FindReplacementUpgradeCB",0x6D3490},
        {"",nullptr,0x734A70},{"CAEStreamingDecoder","ScalarDeletingDestructor",0x4F2860}};
    bool ready=true;json registered=json::object();json bodies=json::object();
    for(unsigned i=0;i<kFunctions;++i) {
        if(!rows[i].name)continue;
        env.routes[i].reset(new HookRoute(rows[i].cls,rows[i].name,rows[i].address,HookRoute::RegisteredTarget{}));
        const bool reg=env.routes[i]->Registered();registered[std::string(rows[i].cls)+"::"+rows[i].name]=reg;
        const uintptr_t body=env.routes[i]->Body();bodies[std::string(rows[i].cls)+"::"+rows[i].name]=body;
        ready&=reg&&body&&ModuleOf(body)==env.asi;
    }
    // Cuerpos de la DLL de los colaboradores reconstruidos que la DLL llama directamente (se resuelven saltando los E9 de enlace incremental ANTES de crear parches).
    env.directDll=Resolve(reinterpret_cast<uintptr_t>(&GetAnimHierarchyFromFrameCallback));
    env.dllRemoveAll=Resolve(std::bit_cast<uintptr_t>(&CPedGroup::RemoveAllFollowers));
    env.dllSetDefault=Resolve(std::bit_cast<uintptr_t>(&CDecision::SetDefault));
    env.dllFindSafehouse=Resolve(reinterpret_cast<uintptr_t>(&CGarages::FindSafeHouseIndexForGarageType));
    env.dllAtomicId=Resolve(reinterpret_cast<uintptr_t>(&CVisibilityPlugins::GetAtomicId));
    env.dllModelIndex=Resolve(reinterpret_cast<uintptr_t>(&CVisibilityPlugins::GetModelInfoIndex));
    env.dllHierarchy=Resolve(reinterpret_cast<uintptr_t>(&RpHAnimFrameGetHierarchy));
    env.dllChildren=Resolve(reinterpret_cast<uintptr_t>(&RwFrameForAllChildren));
    Run(env,ready,registered,bodies);
}
}
