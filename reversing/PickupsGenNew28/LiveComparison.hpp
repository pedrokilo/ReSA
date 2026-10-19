#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Pickups.h"
#include "Pickup.h"
#include "World.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PickupsGenNew28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CPickups::GenerateNewOne (0x456F20), hook "GenerateNewOne" -> CPickups::GenerateNewOne_ExeAbi. Los colaboradores sustituidos y el que se ejecuta de verdad estan en Core.hpp y README.md.
static_assert(sizeof(CPickup) == 0x20, "el banco trabaja con ranuras de 0x20 bytes");
static_assert(MAX_NUM_PICKUPS == PickupsGen28Fix::kSlots, "620 ranuras");
static_assert(offsetof(CPickup, m_pObject) == 4 && offsetof(CPickup, m_nReferenceIndex) == 0x1A && offsetof(CPickup, m_nPickupType) == 0x1C, "disposicion de CPickup");
namespace PickupsGen28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pickupsgen28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pickupsgen28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pickupsgen28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CPickups","GenerateNewOne",0x456F20,HookRoute::RegisteredTarget{}));
    registered["CPickups::GenerateNewOne"]=env.route->Registered();bodies["CPickups::GenerateNewOne"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    auto B=[](auto fp){return Body(std::bit_cast<uintptr_t>(fp));};
    env.pText={0x455500,B(&CPickup::FindTextIndexForString)};
    env.pVis={0x454C70,B(&CPickup::IsVisible)};
    env.pGive={0x4567E0,B(&CPickup::GiveUsAPickUpObject)};
    env.pRemove={0x563280,B(&CWorld::Remove)};
    env.pAdd={0x563220,B(&CWorld::Add)};
    Run(env,ready,registered,bodies);
}
}
