#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/PedSaveStructure.h"
#include "EntryExitManager.h"
#include "Streaming.h"
#include "WeaponInfo.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PedSaveExtract28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CPedSaveStructure::Extract (0x5D44B0) con su HookRoute. Los seis colaboradores sustituidos estan descritos en Core.hpp y README.md.
static_assert(sizeof(CPed) == PedSaveExtract28Fix::kPedSize, "el banco trabaja con un ped de 0x79C bytes");
static_assert(sizeof(CPedSaveStructure) == PedSaveExtract28Fix::kSaveSize, "la estructura guardada mide 0x18C bytes");
static_assert(sizeof(CMatrix) == PedSaveExtract28Fix::kMatSize, "la matriz mide 0x48 bytes");
static_assert(sizeof(CEntryExit) == 0x3C, "el EXE multiplica el indice de salida por 0x3C");
static_assert(sizeof(CWeapon) == 0x1C, "las armas guardadas y las del ped miden 0x1C bytes");
static_assert(offsetof(CPed, m_fHealth) == 0x540 && offsetof(CPed, m_fArmour) == 0x548, "vida y armadura del ped");
static_assert(offsetof(CPed, m_aWeapons) == 0x5A0 && offsetof(CPed, m_nActiveWeaponSlot) == 0x718, "armas y ranura activa del ped");
static_assert(offsetof(CPed, m_nFightingStyle) == 0x72D && offsetof(CPed, m_nAllowedAttackMoves) == 0x72E && offsetof(CPed, m_pEnex) == 0x78C, "estilo de lucha, movimientos y entrada/salida");
static_assert(offsetof(CWeapon, m_AmmoInClip) == 8 && offsetof(CWeapon, m_TotalAmmo) == 0xC, "municion del cargador y total");
static_assert(offsetof(CWeaponInfo, m_nModelId1) == 0xC && offsetof(CWeaponInfo, m_nModelId2) == 0x10, "modelos del arma");
namespace PedSaveExtract28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pedsaveextract28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pedsaveextract28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pedsaveextract28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CPedSaveStructure","Extract",0x5D44B0,HookRoute::RegisteredTarget{}));
    registered["CPedSaveStructure::Extract"]=env.route->Registered();bodies["CPedSaveStructure::Extract"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    auto B=[](auto fp){return Body(std::bit_cast<uintptr_t>(fp));};
    env.pCreated={0x5E47E0,B(&CPed::SetCharCreatedBy)};
    env.pInfo={0x743C60,B(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo))};
    env.pRequest={0x4087E0,B(static_cast<void(*)(int32,int32)>(&CStreaming::RequestModel))};
    env.pLoad={0x40EA10,B(static_cast<void(*)(bool)>(&CStreaming::LoadAllRequestedModels))};
    env.pGive={0x5E6080,B(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon))};
    env.pSetCur={0x5E61F0,B(static_cast<void(CPed::*)(int32)>(&CPed::SetCurrentWeapon))};
    Run(env,ready,registered,bodies);
}
}
