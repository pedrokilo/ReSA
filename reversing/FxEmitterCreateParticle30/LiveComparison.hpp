#pragma once
#include <fstream>
#include <memory>
#include "FxEmitter.h"
#include "FxEmitterBP.h"
#include "FxManager.h"
#include "FxPrimBP.h"
#include "FxEmitterPrt.h"
#include "EmissionInfo.h"
#include "Fx.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FxEmitterCreateParticle30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace FxCp30Live {
inline uintptr_t Entry(auto member){return Resolve(std::bit_cast<uintptr_t>(member));}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fxemittercreateparticle30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fxemittercreateparticle30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fxemittercreateparticle30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado. CreateParticle no es virtual: el candidato es un puntero a miembro normal.
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("FxEmitter_c","CreateParticle",0x4A2580,&FxEmitter_c::CreateParticle));
    const bool reg=env.route->Registered();registered["FxEmitter_c::CreateParticle"]=reg;
    const uintptr_t body=env.route->Body();bodies["FxEmitter_c::CreateParticle"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    // Entradas de la DLL de los colaboradores (se resuelven saltando los E9 de enlace incremental): GetParticle, FreeUpParticle, GetRWMatrix, RwV3dNormalize,
    // RwV3dTransformVectors, RotateVecIntoVec y UpdateParticle. Los de RenderWare son envolturas de la DLL que llaman por direccion al EXE (se sustituyen ambas).
    env.dllAt[0]=Entry(&FxManager_c::GetParticle);
    env.dllAt[1]=Entry(&FxManager_c::FreeUpParticle);
    env.dllAt[2]=Entry(&FxPrimBP_c::GetRWMatrix);
    env.dllAt[3]=Resolve(uintptr_t(&RwV3dNormalize));
    env.dllAt[4]=Resolve(uintptr_t(&RwV3dTransformVectors));
    env.dllAt[5]=Resolve(uintptr_t(&RotateVecIntoVec));
    env.dllAt[6]=Entry(&FxEmitterBP_c::UpdateParticle);
    Run(env,ready,registered,bodies);
}
}
