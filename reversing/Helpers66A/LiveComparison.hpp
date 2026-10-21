#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "PlayerPed.h"
#include "PedGroup.h"
#include "PedGroups.h"
#include "TagManager.h"
#include "Entity/Object/Object.h"
#include "TaskSimpleGangDriveBy.h"
#include "Fx/Fx.h"
#include "General.h"
#include "World.h"
#include "platform.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par Helpers66A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Hp66Layout {
struct PlayerProbe : CPlayerPed {
    static constexpr size_t OffPlayerData() {return offsetof(PlayerProbe, m_pPlayerData);}
    static constexpr size_t OffTargeted() {return offsetof(PlayerProbe, m_pTargetedObject);}
    static constexpr size_t OffMouse() {return offsetof(PlayerProbe, m_p3rdPersonMouseTarget);}
    static constexpr size_t OffSlot() {return offsetof(PlayerProbe, m_nActiveWeaponSlot);}
    static constexpr size_t OffWeapons() {return offsetof(PlayerProbe, m_aWeapons);}
};
static_assert(PlayerProbe::OffPlayerData() == 0x480 && PlayerProbe::OffTargeted() == 0x71C && PlayerProbe::OffMouse() == 0x79C && PlayerProbe::OffSlot() == 0x718 && PlayerProbe::OffWeapons() == 0x5A0, "CPlayerPed");
struct GangProbe : CTaskSimpleGangDriveBy {
    static constexpr size_t OffTime() {return offsetof(GangProbe, m_nLOSCheckTime);}
    static constexpr size_t OffBlocked() {return offsetof(GangProbe, m_nLOSBlocked);}
    static constexpr size_t OffTarget() {return offsetof(GangProbe, m_pTargetEntity);}
};
static_assert(GangProbe::OffTime() == 0x18 && GangProbe::OffBlocked() == 0x1C && GangProbe::OffTarget() == 0x34 && sizeof(CTaskSimpleGangDriveBy) == 0x44, "CTaskSimpleGangDriveBy");
static_assert(sizeof(CPedGroup) == 0x2D4, "CPedGroup");
static_assert(offsetof(RwCamera, nearPlane) == 0x80 && offsetof(RwCamera, farPlane) == 0x84, "RwCamera");
}

namespace Hp66Live {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Cuerpo de la DLL de una funcion con hook S o V por (clase, nombre)
inline uintptr_t DllBodyByName(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn);if(!h)return 0;uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a?Body(a):0;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"helpers66a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"helpers66a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("helpers66a");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* cls,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute(cls,fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"CPlayerPed","PlayerWantsToAttack",0x60CC50);
    simple(1,"CPlayerPed","EvaluateNeighbouringTarget",0x60D1C0);
    simple(2,"CTaskSimpleGangDriveBy","HasClearLineOfSight",0x621B10);
    simple(3,"Fx_c","AddPunchImpact",0x49F670);
    simple(4,"Rs","RsSavePresetView",0x619FA0);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    // 0: CPedGroupMembership::CountMembersExcludingLeader 0x5F6AA0, CPedGroupIntelligence::ReportAllBarScriptTasksFinished 0x5F8780, CTagManager::IsTag 0x49CCE0, CObject::CanBeTargetted 0x59F320,
    //    CPlayerPed::FindPedToAttack 0x60C5F0 y CPedGroup::PlayerGaveCommand_Attack 0x5F7CC0 (todos con hook).
    // 1: CPlayerPed::DoesTargetHaveToBeBroken 0x60C0C0 y CGeneral::GetATanOfXY 0x53CC70 (con hook).
    // 2: CWorld::GetIsLineOfSightClear 0x56A490 (con hook) y rand() del EXE 0x821B1E (sin hook: la DLL llama a la direccion del EXE).
    // 3: FxSystem_c::AddParticle(v3d) 0x4AA440 (con hook); el constructor FxPrtMult_c 0x4AB290 corre de verdad en ambos lados.
    // 4: psPathnameCreate 0x745470, psPathnameDestroy 0x7454E0 y psErrorMessage 0x7451D0 (con hook); RwFrameGetLTM 0x7F0990, RwV3dNormalize 0x7ED9B0 y RsLoadPresetViews 0x619D60 (sin cuerpo propio:
    //    la DLL llama a la direccion del EXE); RsSetPresetView 0x619780 (reconstruida sin hook). Ademas las tablas de RenderWare (fopen, fclose, fputs, sprintf, strlen) se sustituyen durante la llamada.
    //    RsErrorMessage 0x619B00 y RsKeyboardEventHandler 0x619560 corren de verdad (con hook): el EXE tiene RsErrorMessage expandida en linea en la rama de fallo de fopen.
    env.stubs[0]={{Pair{0x5F6AA0,DllBody(&CPedGroupMembership::CountMembersExcludingLeader),true},reinterpret_cast<void*>(&FakeCount)},
                  {Pair{0x5F8780,DllBody(&CPedGroupIntelligence::ReportAllBarScriptTasksFinished),true},reinterpret_cast<void*>(&FakeReport)},
                  {Pair{0x49CCE0,DllBody(&CTagManager::IsTag),true},reinterpret_cast<void*>(&FakeIsTag)},
                  {Pair{0x59F320,DllBody(&CObject::CanBeTargetted),true},reinterpret_cast<void*>(&FakeCanTarget)},
                  {Pair{0x60C5F0,DllBody(&CPlayerPed::FindPedToAttack),true},reinterpret_cast<void*>(&FakeFindPed)},
                  {Pair{0x5F7CC0,DllBody(&CPedGroup::PlayerGaveCommand_Attack),true},reinterpret_cast<void*>(&FakeAttack)}};
    env.stubs[1]={{Pair{0x60C0C0,DllBody(&CPlayerPed::DoesTargetHaveToBeBroken),true},reinterpret_cast<void*>(&FakeBroken)},
                  {Pair{0x53CC70,DllBody(static_cast<float(*)(float,float)>(&CGeneral::GetATanOfXY)),true},reinterpret_cast<void*>(&FakeAtan)}};
    env.stubs[2]={{Pair{0x56A490,DllBody(&CWorld::GetIsLineOfSightClear),true},reinterpret_cast<void*>(&FakeLos)},
                  {Pair{0x821B1E,0,false},reinterpret_cast<void*>(&FakeRand)}};
    env.stubs[3]={{Pair{0x4AA440,DllBodyByName("FxSystem_c","AddParticle-v3d"),true},reinterpret_cast<void*>(&FakeAddParticle)}};
    env.stubs[4]={{Pair{0x745470,DllBody(&psPathnameCreate),true},reinterpret_cast<void*>(&FakePathCreate)},
                  {Pair{0x7454E0,DllBody(&psPathnameDestroy),true},reinterpret_cast<void*>(&FakePathDestroy)},
                  {Pair{0x7451D0,DllBody(&psErrorMessage),true},reinterpret_cast<void*>(&FakeErrorBox)},
                  {Pair{0x7F0990,0,false},reinterpret_cast<void*>(&FakeGetLtm)},
                  {Pair{0x7ED9B0,0,false},reinterpret_cast<void*>(&FakeNormalize)},
                  {Pair{0x619D60,0,false},reinterpret_cast<void*>(&FakeLoad)},
                  {Pair{0x619780,DllBody(&RsSetPresetView),false},reinterpret_cast<void*>(&FakeSetView)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    collaborators[kFn[4]]["tables"]=json::array({"RwFileFunctions.fopen +4","RwFileFunctions.fclose +8","RwFileFunctions.fputs +0x18","RwStringFunctions.sprintf +0xF0","RwStringFunctions.strlen +0x120"});
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
