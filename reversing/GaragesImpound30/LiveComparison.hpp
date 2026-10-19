#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Garages.h"
#include "Garage.h"
#include "Placeable.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par GaragesImpound30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CGarages::StoreCarInNearestImpoundingGarage (0x44A3C0), hook "StoreCarInNearestImpoundingGarage" de la clase CGarages. Colaboradores sustituidos: FindSafeHouseIndexForGarageType y StoreCar.
static_assert(sizeof(CGarage) == GaragesImpound30Fix::kEntryBytes, "el banco trabaja con ranuras de 0xD8 bytes");
static_assert(sizeof(CStoredCar) == GaragesImpound30Fix::kSlotBytes, "ranuras de coche guardado de 0x40 bytes");
static_assert(CGarages::MAX_NUM_GARAGES == GaragesImpound30Fix::kMaxGarages, "50 garajes");
static_assert(CGarages::MAX_NUM_SAFEHOUSES == GaragesImpound30Fix::kSafehouses && CGarages::MAX_CARS_IN_SAFEHOUSE == GaragesImpound30Fix::kSlotsPerHouse, "20 refugios de 4 coches");
static_assert(offsetof(CGarage, m_vPosn) == 0 && offsetof(CGarage, m_nType) == 0x4C, "disposicion de CGarage");
static_assert(offsetof(CStoredCar, m_wModelIndex) == 0x12, "disposicion de CStoredCar");
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "disposicion de CPlaceable");
namespace GaragesImpound30Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"garagesimpound30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"garagesimpound30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("garagesimpound30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CGarages","StoreCarInNearestImpoundingGarage",0x44A3C0,HookRoute::RegisteredTarget{}));
    registered["CGarages::StoreCarInNearestImpoundingGarage"]=env.route->Registered();bodies["CGarages::StoreCarInNearestImpoundingGarage"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas) y con sus hooks en el estado por defecto.
    auto B=[](auto fp){return Body(std::bit_cast<uintptr_t>(fp));};
    env.pFind={0x4489F0,B(&CGarages::FindSafeHouseIndexForGarageType)};
    env.pStore={0x449760,B(&CStoredCar::StoreCar)};
    Run(env,ready,registered,bodies);
}
}
