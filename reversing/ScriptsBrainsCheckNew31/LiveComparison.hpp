#pragma once
#include <fstream>
#include <memory>
#include "ScriptsForBrains.h"
#include "Scripts/TheScripts.h"
#include "Scripts/RunningScript.h"
#include "Streaming.h"
#include "Pools/Pools.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par ScriptsBrainsCheckNew31 dentro del juego: Update() se llama desde CGame::Process (build_probe.py). Todo el nucleo esta en Core.hpp.
namespace ScriptsBrainsCheckNew31Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"scriptsbrainschecknew31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"scriptsbrainschecknew31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("scriptsbrainschecknew31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado (se restauran al final de Run).
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CScriptsForBrains","CheckIfNewEntityNeedsScript",0x46FF20,HookRoute::RegisteredTarget{}));
    {const bool reg=env.route->Registered();registered["CScriptsForBrains::CheckIfNewEntityNeedsScript"]=reg;const uintptr_t b=env.route->Body();bodies["CScriptsForBrains::CheckIfNewEntityNeedsScript"]=b;ready&=reg&&b&&ModuleOf(b)==env.asi;}
    env.collab.reset(new HookRoute("CScriptsForBrains","StartOrRequestNewStreamedScriptBrain",0x46CD80,HookRoute::RegisteredTarget{}));
    {const bool reg=env.collab->Registered();registered["CScriptsForBrains::StartOrRequestNewStreamedScriptBrain"]=reg;const uintptr_t b=env.collab->Body();bodies["CScriptsForBrains::StartOrRequestNewStreamedScriptBrain"]=b;ready&=reg&&b&&ModuleOf(b)==env.asi;}
    env.layoutOk=sizeof(tScriptForBrains)==20&&offsetof(tScriptForBrains,m_TypeOfBrain)==2&&offsetof(tScriptForBrains,m_PedModelOrPedGeneratorIndex)==0xC&&offsetof(tScriptForBrains,m_PercentageChance)==0xE
        &&offsetof(CEntity,m_nModelIndex)==0x22&&offsetof(CObject,m_nObjectFlags)==0x140&&sizeof(CScriptsForBrains)==70*20;
    Run(env,ready,registered,bodies);
}
}
