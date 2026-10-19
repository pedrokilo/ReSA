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

// Banco par a par ScriptsBrainsStart31 dentro del juego: Update() se llama desde CGame::Process (build_probe.py). Todo el nucleo esta en Core.hpp.
namespace ScriptsBrainsStart31Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"scriptsbrainsstart31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"scriptsbrainsstart31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("scriptsbrainsstart31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado (se restauran al final de Run).
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CScriptsForBrains","StartNewStreamedScriptBrain",0x46B270,HookRoute::RegisteredTarget{}));
    {const bool reg=env.route->Registered();registered["CScriptsForBrains::StartNewStreamedScriptBrain"]=reg;const uintptr_t b=env.route->Body();bodies["CScriptsForBrains::StartNewStreamedScriptBrain"]=b;ready&=reg&&b&&ModuleOf(b)==env.asi;}
    env.collabNew.reset(new HookRoute("CStreamedScripts","StartNewStreamedScript",0x470890,HookRoute::RegisteredTarget{}));
    {const bool reg=env.collabNew->Registered();registered["CStreamedScripts::StartNewStreamedScript"]=reg;const uintptr_t b=env.collabNew->Body();bodies["CStreamedScripts::StartNewStreamedScript"]=b;ready&=reg&&b&&ModuleOf(b)==env.asi;}
    env.collabRem.reset(new HookRoute("CTheScripts","RemoveFromWaitingForScriptBrainArray",0x46ABC0,HookRoute::RegisteredTarget{}));
    {const bool reg=env.collabRem->Registered();registered["CTheScripts::RemoveFromWaitingForScriptBrainArray"]=reg;const uintptr_t b=env.collabRem->Body();bodies["CTheScripts::RemoveFromWaitingForScriptBrainArray"]=b;ready&=reg&&b&&ModuleOf(b)==env.asi;}
    {   // los simbolos de la DLL (pools, offsets de CRunningScript/CPed/CObject) deben coincidir con las direcciones y desplazamientos que usa el EXE
        const uint32 savedPed=PedPoolGlobal(),savedObj=ObjPoolGlobal();
        PedPoolGlobal()=uint32(uintptr_t(&pedPool));ObjPoolGlobal()=uint32(uintptr_t(&objPool));
        env.layoutOk=sizeof(tScriptForBrains)==20&&offsetof(tScriptForBrains,m_StreamedScriptIndex)==0&&offsetof(tScriptForBrains,m_TypeOfBrain)==2&&uintptr_t(GetPedPool())==uintptr_t(&pedPool)&&uintptr_t(GetObjectPool())==uintptr_t(&objPool)&&sizeof(CPedPool)==sizeof(FakePool)&&sizeof(CObjectPool)==sizeof(FakePool)
            &&offsetof(CRunningScript,m_LocalVars)==0x3C&&offsetof(CRunningScript,m_ExternalType)==0xC9&&sizeof(CRunningScript)==0xE0
            &&offsetof(CPed,m_StreamedScriptBrainToLoad)==0x794&&offsetof(CObject,m_nObjectFlags)==0x140&&uintptr_t(&CTheScripts::StreamedScripts)==kStreamedScripts;
        PedPoolGlobal()=savedPed;ObjPoolGlobal()=savedObj;
    }
    Run(env,ready,registered,bodies);
}
}
