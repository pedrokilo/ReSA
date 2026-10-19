#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendHierarchy.h"
#include "Animation/AnimBlock.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/PlayerPed.h"
#include "Pad.h"
#include "WeaponInfo.h"
#include "Streaming.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Audio/Entities/AEPedWeaponAudioEntity.h"
#include "Camera.h"
#include "Tasks/Task.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FightProcessPed31 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskSimpleFight::ProcessPed (0x629920). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del candidato se
// toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x629920 con un salto a ese cuerpo).
namespace FP = FightProcessPed31Fix;
static_assert(sizeof(CTaskSimpleFight) == FP::kThisBytes, "CTaskSimpleFight mide 0x28");
static_assert(offsetof(CTaskSimpleFight, m_bIsFinished) == 8 && offsetof(CTaskSimpleFight, m_bIsInControl) == 9 && offsetof(CTaskSimpleFight, m_bAnimsReferenced) == 0xA, "+8 +9 +0xA");
static_assert(offsetof(CTaskSimpleFight, m_nRequiredAnimGroup) == 0xC && offsetof(CTaskSimpleFight, m_nIdlePeriod) == 0x10 && offsetof(CTaskSimpleFight, m_nIdleCounter) == 0x12, "+0xC +0x10 +0x12");
static_assert(offsetof(CTaskSimpleFight, m_pTargetEntity) == 0x18 && offsetof(CTaskSimpleFight, m_pAnim) == 0x1C && offsetof(CTaskSimpleFight, m_pIdleAnim) == 0x20, "+0x18 +0x1C +0x20");
static_assert(offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nCurrentMove) == 0x25 && offsetof(CTaskSimpleFight, m_nNextCommand) == 0x26 && offsetof(CTaskSimpleFight, m_nLastCommand) == 0x27, "+0x24..+0x27");
static_assert(offsetof(CAnimBlendAssociation, m_BlendHier) == FP::kAnimHier && offsetof(CAnimBlendAssociation, m_BlendAmount) == FP::kAnimWeight && offsetof(CAnimBlendAssociation, m_BlendDelta) == FP::kAnimDelta, "asociacion +0x14 +0x18 +0x1C");
static_assert(offsetof(CAnimBlendAssociation, m_CurrentTime) == FP::kAnimTime && offsetof(CAnimBlendAssociation, m_TimeStep) == FP::kAnimStep && offsetof(CAnimBlendAssociation, m_AnimId) == FP::kAnimId && offsetof(CAnimBlendAssociation, m_Flags) == FP::kAnimFlags, "asociacion +0x20 +0x28 +0x2C +0x2E");
static_assert(offsetof(CAnimBlendAssociation, m_pCallbackData) + 4 == FP::kAnimBytes, "la asociacion modelada llega hasta m_pCallbackData");
static_assert(offsetof(CAnimBlendHierarchy, m_fTotalTime) == FP::kHierTotal, "m_fTotalTime en +0x10");
static_assert(offsetof(CAnimBlock, IsLoaded) == 0x10 && sizeof(CAnimBlock) == 0x20, "CAnimBlock: IsLoaded en +0x10, 0x20 bytes");
static_assert(sizeof(CAnimBlendAssocGroup) == 0x14 && offsetof(CAnimBlendAssocGroup, m_AnimBlock) == 0, "CAnimBlendAssocGroup: 0x14 bytes, m_AnimBlock en +0");
static_assert(offsetof(CWeaponInfo, m_nBaseCombo) == 0x6E, "m_nBaseCombo en +0x6E");
static_assert(offsetof(CPlaceable, m_matrix) == FP::kPedMatrix, "m_matrix en +0x14");
// m_pRwObject (clump, +0x18) es privado en CEntity: el desplazamiento sale del desensamblado (mov eax,[ebx+18h]); no se puede comprobar con offsetof.
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == FP::kPedSpeed, "m_vecMoveSpeed en +0x44");
static_assert(offsetof(CPed, m_nAnimGroup) == FP::kPedGroup && offsetof(CPed, m_nMoveState) == FP::kPedMoveState && offsetof(CPed, m_nSwimmingMoveState) == FP::kPedSwim, "ped +0x4D4 +0x534 +0x538");
static_assert(offsetof(CPed, m_fAimingRotation) == FP::kPedAim && offsetof(CPed, m_aWeapons) == FP::kPedWeapons && offsetof(CPed, m_nActiveWeaponSlot) == FP::kPedSlot && offsetof(CPed, m_nFightingStyle) == FP::kPedStyle, "ped +0x55C +0x5A0 +0x718 +0x72D");
struct FightProcessPed31PedProbe : CPed { static constexpr size_t PedAudio() { return offsetof(FightProcessPed31PedProbe, m_pedAudio); } static constexpr size_t WeaponAudio() { return offsetof(FightProcessPed31PedProbe, m_weaponAudio); } };   // m_pedAudio y m_weaponAudio son protegidos
static_assert(FightProcessPed31PedProbe::PedAudio() == 0x138 && FightProcessPed31PedProbe::WeaponAudio() == 0x394, "audio del ped +0x138 y del arma +0x394");
static_assert(sizeof(CWeapon) == FP::kWeaponStride, "CWeapon mide 0x1C");
static_assert(offsetof(CPed, m_nAnimGroup) + 4 <= FP::kPedBytes && offsetof(CPed, m_nFightingStyle) + 1 <= FP::kPedBytes, "la ventana del ped cubre todos los campos");
static_assert(FP::kRegionBase + FP::kRegion == 0xC17940u && FP::kRegionBase + 0x40 == FP::kTableAt, "region 0xC17090..0xC17940");
namespace FightProcessPed31Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fightprocessped31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fightprocessped31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fightprocessped31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskSimpleFight","ProcessPed");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskSimpleFight","ProcessPed",0x629920,body));
    registered["CTaskSimpleFight::ProcessPed"]=env.route->Registered();bodies["CTaskSimpleFight::ProcessPed"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas). Con sus hooks encendidos (estado normal del ASI) la entrada del EXE
    // salta a la DLL; Body() sigue el salto. 0x61C7F0, 0x61C9B0, 0x624710, 0x624A40 y 0x623B10 no estan reconstruidos (solo existe la entrada del EXE).
    env.pSetDel={0x4CEBC0,Body(std::bit_cast<uintptr_t>(&CAnimBlendAssociation::SetDeleteCallback))};
    env.pBlend={0x4D4610,Body(std::bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)))};
    env.pSetTime={0x4CEA80,Body(std::bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime))};
    env.pIsPlayer={0x5DF8F0,Body(std::bit_cast<uintptr_t>(&CPed::IsPlayer))};
    env.pMoveState={0x5DEC00,Body(std::bit_cast<uintptr_t>(&CPed::SetMoveState))};
    env.pAddRef={0x4D3FB0,Body(std::bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef))};
    env.pRemRef={0x4D3FD0,Body(std::bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef))};
    env.pBlockName={0x4D3A30,Body(std::bit_cast<uintptr_t>(&CAnimManager::GetAnimBlockName))};
    env.pGetBlock={0x4D3940,Body(std::bit_cast<uintptr_t>(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock)))};
    env.pRequest={0x4087E0,Body(std::bit_cast<uintptr_t>(&CStreaming::RequestModel))};
    env.pWinfo={0x743C60,Body(std::bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)))};
    env.pPedAE={0x4E2BB0,Body(std::bit_cast<uintptr_t>(&CAEPedAudioEntity::AddAudioEvent))};
    env.pWeapAE={0x4E69F0,Body(std::bit_cast<uintptr_t>(&CAEPedWeaponAudioEntity::AddAudioEvent))};
    env.pStrike={0x6240B0,Body(std::bit_cast<uintptr_t>(&CTaskSimpleFight::FightStrike))};
    env.pAvail={0x61C7F0,0};env.pFind={0x61C9B0,0};env.pChooseP={0x624710,0};env.pChooseAI={0x624A40,0};env.pStart={0x623B10,0};
    env.pGetPad={0x609560,Body(std::bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer))};
    env.pGetTarget={0x540670,Body(std::bit_cast<uintptr_t>(&CPad::GetTarget))};
    Run(env,ready,registered,bodies);
}
}
