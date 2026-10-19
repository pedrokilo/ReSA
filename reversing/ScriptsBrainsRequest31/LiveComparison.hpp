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

// Banco par a par ScriptsBrainsRequest31 dentro del juego: Update() se llama desde CGame::Process (build_probe.py). Todo el nucleo esta en Core.hpp.
namespace ScriptsBrainsRequest31Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"scriptsbrainsrequest31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"scriptsbrainsrequest31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("scriptsbrainsrequest31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado (se restauran al final de Run).
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CScriptsForBrains","RequestAttractorScriptBrainWithThisName",0x46AA80,HookRoute::RegisteredTarget{}));
    {const bool reg=env.route->Registered();registered["CScriptsForBrains::RequestAttractorScriptBrainWithThisName"]=reg;const uintptr_t b=env.route->Body();bodies["CScriptsForBrains::RequestAttractorScriptBrainWithThisName"]=b;ready&=reg&&b&&ModuleOf(b)==env.asi;}
    env.collabIdx.reset(new HookRoute("CScriptsForBrains","GetIndexOfScriptBrainWithThisName",0x46AA30,HookRoute::RegisteredTarget{}));
    {const bool reg=env.collabIdx->Registered();registered["CScriptsForBrains::GetIndexOfScriptBrainWithThisName"]=reg;const uintptr_t b=env.collabIdx->Body();bodies["CScriptsForBrains::GetIndexOfScriptBrainWithThisName"]=b;ready&=reg&&b&&ModuleOf(b)==env.asi;}
    env.collabReq.reset(new HookRoute("CStreaming","RequestModel",0x4087E0,HookRoute::RegisteredTarget{}));
    {const bool reg=env.collabReq->Registered();registered["CStreaming::RequestModel"]=reg;const uintptr_t b=env.collabReq->Body();bodies["CStreaming::RequestModel"]=b;ready&=reg&&b&&ModuleOf(b)==env.asi;}
    env.layoutOk=sizeof(tScriptForBrains)==20&&offsetof(tScriptForBrains,m_StreamedScriptIndex)==0&&SCMToModelId(0)==0x6676&&CStreamedScripts::NUM_STREAMED_SCRIPTS==82&&sizeof(CStreamedScripts::CStreamedScriptInfo)==32&&sizeof(CStreamingInfo)==20
        &&uintptr_t(&CTheScripts::StreamedScripts.m_aScripts[0].m_NumberOfUsers)==0xA47B64&&uintptr_t(&CStreaming::GetInfo(SCMToModelId(0)).m_LoadState)==0x964E08;
    Run(env,ready,registered,bodies);
}
}
