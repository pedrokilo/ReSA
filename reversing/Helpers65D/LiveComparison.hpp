#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "Matrix.h"
#include "Tasks/Task.h"
#include "TaskComplexWalkRoundObject.h"
#include "TaskComplexFollowPointRoute.h"
#include "TaskSimpleFight.h"
#include "PedGeometryAnalyser.h"
#include "Stats.h"
#include "PedStats.h"
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par Helpers65D dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Hp65Layout {
static_assert(offsetof(CTaskComplexWalkRoundObject, m_moveState) == 0xC && offsetof(CTaskComplexWalkRoundObject, m_pointRoute) == 0x20 && sizeof(CTaskComplexWalkRoundObject) == 0x54, "CTaskComplexWalkRoundObject");
static_assert(offsetof(CPointRoute, m_NumEntries) == 0 && offsetof(CPointRoute, m_Entries) == 4 && sizeof(CPointRoute) == 0x64, "CPointRoute");
static_assert(sizeof(CTaskComplexFollowPointRoute) <= Hp65Fix::kNewTaskWindow, "CTaskComplexFollowPointRoute cabe en la arena de la tarea nueva");
static_assert(offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nCurrentMove) == 0x25 && sizeof(CTaskSimpleFight) <= Hp65Fix::kFightWindow, "CTaskSimpleFight");
struct PedProbe : CPed {
    static constexpr size_t OffPlayerData() {return offsetof(PedProbe, m_pPlayerData);}
    static constexpr size_t OffStats() {return offsetof(PedProbe, m_pStats);}
    static constexpr size_t OffSlot() {return offsetof(PedProbe, m_nActiveWeaponSlot);}
    static constexpr size_t OffWeapons() {return offsetof(PedProbe, m_aWeapons);}
};
static_assert(PedProbe::OffPlayerData() == 0x480 && PedProbe::OffStats() == 0x59C && PedProbe::OffSlot() == 0x718 && PedProbe::OffWeapons() == 0x5A0 && sizeof(CWeapon) == 0x1C, "CPed");
static_assert(offsetof(CPedStat, m_fAttackStrength) == 0x28, "CPedStat");
static_assert(sizeof(CMatrix) >= 0x48, "CMatrix");
}

namespace Hp65Live {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"helpers65d-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"helpers65d-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("helpers65d");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* cls,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute(cls,fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"CMatrix","operator*-Vec",0x59C890);
    simple(1,"CTaskComplexWalkRoundObject","CreateRouteTask",0x655140);
    simple(2,"RwHelper","RwTexDictionaryGtaStreamRead",0x730FC0);
    simple(3,"CTaskSimpleFight","FightStrikeDamage",0x61C740);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    // 0: ninguno (aritmetica pura). 1: CTask::operator new (con hook) y CPointRoute::operator new (sin hook; el EXE salta a CPointRoutePool::New 0x41B5B0); el constructor de
    // CTaskComplexFollowPointRoute (0x671510, con hook) corre de verdad en ambos lados. 2: los siete RenderWare, que en la DLL son envolturas que llaman a la direccion del EXE (sin
    // hook ni cuerpo propio). 3: CPed::IsPlayer 0x5DF8F0 y CStats::GetFatAndMuscleModifier 0x559AF0 (ambos con hook).
    env.stubs[1]={{Pair{0x61A5A0,DllBody(static_cast<void*(*)(size_t)>(&CTask::operator new)),true},reinterpret_cast<void*>(&FakeTaskNew)},
                  {Pair{0x41B7C0,DllBody(static_cast<void*(*)(uint32)>(&CPointRoute::operator new)),false},reinterpret_cast<void*>(&FakeRouteNew)}};
    env.stubs[2]={{Pair{0x7ED2D0,0,false},reinterpret_cast<void*>(&FakeFindChunk)},
                  {Pair{0x7EC9D0,0,false},reinterpret_cast<void*>(&FakeStreamRead)},
                  {Pair{0x7F3600,0,false},reinterpret_cast<void*>(&FakeCreateDict)},
                  {Pair{0x730E60,0,false},reinterpret_cast<void*>(&FakeTexRead)},
                  {Pair{0x7F3980,0,false},reinterpret_cast<void*>(&FakeAddTex)},
                  {Pair{0x7F3730,0,false},reinterpret_cast<void*>(&FakeForAll)},
                  {Pair{0x7F36A0,0,false},reinterpret_cast<void*>(&FakeDestroyDict)}};
    env.stubs[3]={{Pair{0x5DF8F0,DllBody(&CPed::IsPlayer),true},reinterpret_cast<void*>(&FakeIsPlayer)},
                  {Pair{0x559AF0,DllBody(static_cast<float(*)(eStatModAbilities)>(&CStats::GetFatAndMuscleModifier)),true},reinterpret_cast<void*>(&FakeStat)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
