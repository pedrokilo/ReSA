#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Entity/Ped/Ped.h"
#include "PlayerPedData.h"
#include "Events/Event.h"
#include "Tasks/Task.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FightAbort31 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskSimpleFight::MakeAbortable (0x6239F0). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del candidato se
// toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x6239F0 con un salto a ese cuerpo).
static_assert(sizeof(CTaskSimpleFight) == FightAbort31Fix::kThisBytes, "CTaskSimpleFight mide 0x28");
static_assert(offsetof(CTaskSimpleFight, m_bIsFinished) == FightAbort31Fix::kFinishedAt, "m_bIsFinished en +8");
static_assert(offsetof(CTaskSimpleFight, m_pAnim) == FightAbort31Fix::kAnimAt, "m_pAnim en +0x1C");
static_assert(offsetof(CTaskSimpleFight, m_pIdleAnim) == FightAbort31Fix::kIdleAt, "m_pIdleAnim en +0x20");
static_assert(offsetof(CTaskSimpleFight, m_nNextCommand) == FightAbort31Fix::kNextAt, "m_nNextCommand en +0x26");
static_assert(offsetof(CAnimBlendAssociation, m_BlendAmount) == FightAbort31Fix::kAnimWeightAt, "m_BlendAmount en +0x18");
static_assert(offsetof(CAnimBlendAssociation, m_BlendDelta) == FightAbort31Fix::kAnimDeltaAt, "m_BlendDelta en +0x1C");
static_assert(offsetof(CAnimBlendAssociation, m_pCallbackData) + 4 == FightAbort31Fix::kAnimBytes, "la asociacion modelada llega hasta m_pCallbackData");
// m_pRwObject (clump, +0x18) es privado en CEntity: el desplazamiento sale del desensamblado (mov eax,[edi+18h]); no se puede comprobar con offsetof.
struct FightAbort31PedProbe : CPed { static constexpr size_t Pd() { return offsetof(FightAbort31PedProbe, m_pPlayerData); } };   // m_pPlayerData es protegido
static_assert(FightAbort31PedProbe::Pd() == FightAbort31Fix::kPedPdAt, "m_pPlayerData en +0x480");
static_assert(offsetof(CPed, m_nAnimGroup) == FightAbort31Fix::kPedGroupAt, "m_nAnimGroup en +0x4D4");
static_assert(offsetof(CPed, m_nAnimGroup) + 4 == FightAbort31Fix::kPedBytes, "la ventana del ped llega hasta m_nAnimGroup");
static_assert(offsetof(CPlayerPedData, m_vecFightMovement) == FightAbort31Fix::kPdFightAt, "m_vecFightMovement en +0xC");
static_assert(ABORT_PRIORITY_URGENT == 1 && ABORT_PRIORITY_IMMEDIATE == 2, "prioridades 1 y 2");
namespace FightAbort31Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fightabort31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fightabort31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fightabort31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskSimpleFight","MakeAbortable");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskSimpleFight","MakeAbortable",0x6239F0,body));
    registered["CTaskSimpleFight::MakeAbortable"]=env.route->Registered();bodies["CTaskSimpleFight::MakeAbortable"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas). Con sus hooks encendidos (estado normal del ASI) la entrada del EXE
    // salta a la DLL; Body() sigue el salto. 0x61C9B0 no esta reconstruido (solo existe la entrada del EXE).
    env.pSetDel={0x4CEBC0,Body(std::bit_cast<uintptr_t>(&CAnimBlendAssociation::SetDeleteCallback))};
    env.pBlend={0x4D4610,Body(std::bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)))};
    env.pIsPlayer={0x5DF8F0,Body(std::bit_cast<uintptr_t>(&CPed::IsPlayer))};
    env.pFind={0x61C9B0,0};
    Run(env,ready,registered,bodies);
}
}
