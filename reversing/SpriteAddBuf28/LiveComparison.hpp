#pragma once
#include <fstream>
#include <memory>
#include "Sprite2d.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par SpriteAddBuf28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace SpriteAddBuf28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"spriteaddbuf28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"spriteaddbuf28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("spriteaddbuf28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CSprite2d","AddToBuffer",0x728200,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CSprite2d::AddToBuffer"]=reg;
    const uintptr_t body=env.route->Body();bodies["CSprite2d::AddToBuffer"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    // Cuerpos de la DLL de los colaboradores (la DLL los llama directamente): se resuelven saltando los E9 de enlace incremental ANTES de crear parches.
    using SetVerticesD3D9=void(*)(RwD3D9Vertex*,const CRect&,const CRGBA&,const CRGBA&,const CRGBA&,const CRGBA&,float,float,float,float,float,float,float,float);
    env.dllSetVertices=Resolve(reinterpret_cast<uintptr_t>(static_cast<SetVerticesD3D9>(&CSprite2d::SetVertices)));
    env.dllRender=Resolve(reinterpret_cast<uintptr_t>(&CSprite2d::RenderVertexBuffer));
    Run(env,ready,registered,bodies);
}
}
