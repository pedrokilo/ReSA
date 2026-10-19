#pragma once
#include <fstream>
#include <memory>
#include "Quaternion.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par QuatSlerpT29 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace QuatSlerpT29Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"quatslerpt29-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"quatslerpt29-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("quatslerpt29");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado. La de Slerp(t) es la funcion probada (nombre del hook: "Slerp-t",
    // RH_ScopedOverloadedInstall(Slerp, "t", 0x59C630, ...)); CalcTheta y Slerp de cinco argumentos son colaboradores del EXE y se mantienen apagados toda la corrida.
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CQuaternion","Slerp-t",0x59C630,HookRoute::RegisteredTarget{}));
    env.calcTheta.reset(new HookRoute("CAnimBlendNode","CalcTheta",0x4D00E0,HookRoute::RegisteredTarget{}));
    env.slerp5.reset(new HookRoute("CQuaternion","Slerp-halftheta",0x59C300,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CQuaternion::Slerp(const CQuaternion&, const CQuaternion&, float)"]=reg;
    const uintptr_t body=env.route->Body();bodies["CQuaternion::Slerp(const CQuaternion&, const CQuaternion&, float)"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    // Los colaboradores deben existir como hooks RH registrados (para poder apagarlos); si no, el EXE seguiria su ruta normal igualmente, pero se exige para que el banco sea reproducible.
    registered["collaborator CAnimBlendNode::CalcTheta"]=env.calcTheta->Registered();registered["collaborator CQuaternion::Slerp(5 args)"]=env.slerp5->Registered();
    ready&=env.calcTheta->Registered()&&env.slerp5->Registered();
    Run(env,ready,registered,bodies);
}
}
