#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Object/Object.h"
#include "ObjectData.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "PlayerPedData.h"
#include "Weapon.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PedWrappers60 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Tres rutas, todas hooks VIRTUAL de CPed: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del candidato se toma de ReversibleHook::Virtual::GetHookOurAddress()
// (el hook Virtual parchea tambien la entrada del EXE con un salto a ese cuerpo).
namespace PW = PedWrappers60Fix;
// Los desplazamientos de las fixtures (mismos que usa el oraculo) frente a los tipos reales del juego.
static_assert(offsetof(CPhysical, m_nPhysicalFlags) == 0x40 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44 && offsetof(CPhysical, m_fElasticity) == 0x9C, "CPhysical +0x40 +0x44 +0x9C");
static_assert(offsetof(CPhysical, m_pAttachedTo) == 0xFC && offsetof(CPhysical, m_pEntityIgnoredCollision) == 0x128, "CPhysical +0xFC +0x128");
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CEntity, m_nModelIndex) == 0x22 && offsetof(CEntity, m_nFlags) == 0x1C && offsetof(CEntity, m_nRandomSeed) == 0x20, "CEntity +0x14 +0x1C +0x20 +0x22");
struct PedWrappers60PedProbe : CPed {   // los miembros protegidos se miden desde una clase derivada
    static constexpr size_t PedAudio() { return offsetof(PedWrappers60PedProbe, m_pedAudio); }
    static constexpr size_t WeaponAudio() { return offsetof(PedWrappers60PedProbe, m_weaponAudio); }
    static constexpr size_t Intel() { return offsetof(PedWrappers60PedProbe, m_pIntelligence); }
    static constexpr size_t PlayerData() { return offsetof(PedWrappers60PedProbe, m_pPlayerData); }
    static constexpr size_t CreatedBy() { return offsetof(PedWrappers60PedProbe, m_nCreatedBy); }
};
static_assert(PedWrappers60PedProbe::Intel() == 0x47C && PedWrappers60PedProbe::PlayerData() == 0x480 && PedWrappers60PedProbe::CreatedBy() == 0x484 && offsetof(CPed, m_pWeaponObject) == 0x4F4, "CPed +0x47C +0x480 +0x484 +0x4F4");
static_assert(offsetof(CPed, m_nWeaponGunflashAlphaMP1) == 0x504 && offsetof(CPed, m_nWeaponGunFlashAlphaProgMP1) == 0x506 && offsetof(CPed, m_nWeaponGunflashAlphaMP2) == 0x508 && offsetof(CPed, m_nWeaponGunFlashAlphaProgMP2) == 0x50A, "CPed +0x504..+0x50A");
static_assert(offsetof(CPed, m_nPedState) == 0x530 && offsetof(CPed, m_nMoveState) == 0x534 && offsetof(CPed, m_standingOnEntity) == 0x568 && offsetof(CPed, field_588) == 0x588 && offsetof(CPed, m_pVehicle) == 0x58C, "CPed +0x530 +0x534 +0x568 +0x588 +0x58C");
static_assert(offsetof(CPed, m_nPedType) == 0x598 && offsetof(CPed, m_aWeapons) == 0x5A0 && offsetof(CPed, m_nActiveWeaponSlot) == 0x718 && offsetof(CPed, field_72F) == 0x72F, "CPed +0x598 +0x5A0 +0x718 +0x72F");
static_assert(offsetof(CPed, m_vecAnimMovingShiftLocal) == 0x4D8, "CPed +0x4D8");
static_assert(sizeof(CWeapon) == 0x1C && offsetof(CWeapon, m_FxSystem) == 0x18, "CWeapon 0x1C, m_FxSystem +0x18");
static_assert(offsetof(CPlayerPedData, m_nPlayerGroup) == 0x38 && offsetof(CPlayerPedData, m_nWetness) == 0x8C && offsetof(CPlayerPedData, m_nWaterCoverPerc) == 0x8E, "CPlayerPedData +0x38 +0x8C +0x8E");
static_assert(offsetof(CObject, m_nObjectFlags) == 0x140 && offsetof(CObject, m_pObjectInfo) == 0x160 && offsetof(CObjectData, m_fUprootLimit) == 0x14, "CObject +0x140 +0x160, CObjectData +0x14");
static_assert(offsetof(CVehicle, m_nVehicleSubType) == 0x594, "CVehicle +0x594");
static_assert(offsetof(CBaseModelInfo, m_pColModel) == 0x14, "CBaseModelInfo +0x14");
static_assert(sizeof(CPed) == PW::kPedBytes, "CPed mide 0x79C");
static_assert(PedWrappers60PedProbe::PedAudio() == 0x138 && PedWrappers60PedProbe::WeaponAudio() == 0x394, "audio del ped +0x138 y del arma +0x394");
namespace PedWrappers60Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pedwrappers60-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pedwrappers60-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pedwrappers60");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    InitSlots();
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto route=[&](const char* fn,uintptr_t entry,std::unique_ptr<HookRoute>& out){
        auto* hook=FindHook("CPed",fn);
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        out.reset(new HookRoute("CPed",fn,entry,body));
        const std::string key=std::string("CPed::")+fn;
        registered[key]=out->Registered();bodies[key]=out->Body();
        ready&=out->Registered()&&out->Body()&&ModuleOf(out->Body())==env.asi;
    };
    route("SpecialEntityCalcCollisionSteps",0x5E3E90,env.routeCS);
    route("SpecialEntityPreCollisionStuff",0x5E3C30,env.routePS);
    route("ProcessControl",0x5E8CD0,env.routePC);
    Run(env,ready,registered,bodies);
}
}
