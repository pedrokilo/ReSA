#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "CarCtrl.h"
#include "World.h"
#include "ModelInfo.h"
#include "PlayerInfo.h"
#include "SimpleTransform.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarCtrlWeave63B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Seis rutas de CCarCtrl (recorridos de listas de sector); los colaboradores con efectos se sustituyen por falsos identicos en ambos lados.

namespace CwBLayout {
using namespace CwBFix;
struct MiProbe : CBaseModelInfo { static constexpr size_t ColOffset() { return offsetof(MiProbe, m_pColModel); } };
static_assert(offsetof(CPlaceable, m_placement) == kOffPos && offsetof(CPlaceable, m_matrix) == kOffMatrix, "CPlaceable");
static_assert(offsetof(CEntity, m_nModelIndex) == 0x22, "CEntity::m_nModelIndex");
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == kOffSpeed, "CPhysical::m_vecMoveSpeed");
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390 && offsetof(CVehicle, m_pDriver) == kOffDriver && offsetof(CVehicle, m_nVehicleType) == kOffVType, "CVehicle");
static_assert(0x390 + offsetof(CAutoPilot, m_nCarDrivingStyle) == kOffStyle && 0x390 + offsetof(CAutoPilot, m_nTempAction) == kOffTempAct && 0x390 + offsetof(CAutoPilot, m_nTempActionTime) == kOffTempTime
              && 0x390 + offsetof(CAutoPilot, m_nCarCtrlFlags) == kOffCtrl, "CAutoPilot");
struct PedProbe : CPed { static constexpr size_t IntelOffset() { return offsetof(PedProbe, m_pIntelligence); } };
static_assert(PedProbe::IntelOffset() == kOffIntel, "CPed::m_pIntelligence");
static_assert(offsetof(CColModel, m_boundBox) == 0 && offsetof(CBoundingBox, m_vecMin) == 0 && offsetof(CBoundingBox, m_vecMax) == 0xC, "CColModel::m_boundBox");
static_assert(MiProbe::ColOffset() == 0x14, "CBaseModelInfo::m_pColModel");
static_assert(sizeof(CMatrix) <= kMatWords * 4 && sizeof(CPlayerInfo) == kPlayerStride && offsetof(CPlayerInfo, m_pLastTargetVehicle) == kRemoteOff, "CMatrix / CPlayerInfo");
inline bool MatrixLayoutMatches() {
    CMatrix m{};
    const uintptr_t b = uintptr_t(&m);
    return uintptr_t(&m.GetRight()) - b == 0 && uintptr_t(&m.GetForward()) - b == 0x10 && uintptr_t(&m.GetUp()) - b == 0x20 && uintptr_t(&m.GetPosition()) - b == 0x30;
}
inline bool WorldLayoutMatches() {
    return uintptr_t(&CWorld::ms_nCurrentScanCode) == kScanAddr && uintptr_t(&CWorld::PlayerInFocus) == kFocusAddr && uintptr_t(&CWorld::Players[0]) == kPlayersAddr;
}
}

namespace CwBLive {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlweave63b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlweave63b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlweave63b");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CCarCtrl",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    struct C {const char* cls;const char* fn;uintptr_t exe;void* fake;uintptr_t body;};
    auto mk=[&](const C& c){
        Pair p{c.exe,c.body,false};
        if(c.cls) {
            env.collab.emplace_back(new HookRoute(c.cls,c.fn,c.exe,HookRoute::RegisteredTarget{}));
            if(!p.dll&&env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
            else if(!env.collab.back()->Registered()&&!c.body)p.dll=0;
        }
        return StubSpec{p,c.fake};
    };
    const StubSpec bound=mk({"CEntity","GetBoundCentre-vec",0x534250,reinterpret_cast<void*>(&FkBound),0});
    const StubSpec dist=mk({"CCollision","DistAlongLine2D",0x412A80,reinterpret_cast<void*>(&FkDist),0});
    const StubSpec slowOther=mk({"CCarCtrl","SlowCarDownForOtherCar",0x42D0E0,reinterpret_cast<void*>(&FkSlowOther),0});
    const StubSpec slowObj=mk({"CCarCtrl","SlowCarDownForObject",0x426220,reinterpret_cast<void*>(&FkSlowObj),0});
    const StubSpec weaveCar=mk({"CCarCtrl","WeaveForOtherCar",0x426350,reinterpret_cast<void*>(&FkWeaveCar),0});
    const StubSpec weaveObj=mk({"CCarCtrl","WeaveForObject",0x426BC0,reinterpret_cast<void*>(&FkWeaveObj),0});
    const StubSpec weavePed=mk({nullptr,nullptr,0x426970,reinterpret_cast<void*>(&FkWeavePed),0});
    const StubSpec getmat=mk({"CPlaceable","GetMatrix",0x411990,reinterpret_cast<void*>(&FkGetMat),0});
    const StubSpec fpv=mk({"CWorld","FindPlayerVehicle",0x56E0D0,reinterpret_cast<void*>(&FkFpv),0});
    const StubSpec fpp=mk({"CWorld","FindPlayerPed",0x56E210,reinterpret_cast<void*>(&FkFpp),0});
    const StubSpec ref=mk({"CEntity","RegisterReference-",0x571B70,reinterpret_cast<void*>(&FkRef),0});
    const StubSpec clean=mk({"CEntity","CleanUpOldReference-",0x571A00,reinterpret_cast<void*>(&FkClean),0});
    const StubSpec add=mk({"CEventGroup","Add-",0x4AB420,reinterpret_cast<void*>(&FkAdd),0});
    const StubSpec isPlayer=mk({"CPed","IsPlayer",0x5DF8F0,reinterpret_cast<void*>(&FkIsPlayer),0});
    const StubSpec anger=mk({"CPedIntelligence","IncrementAngerAtPlayer",0x421050,reinterpret_cast<void*>(&FkAnger),0});
    env.stubs[kSlowCars]={bound,dist,slowOther};
    env.stubs[kSlowObjs]={bound,dist,slowObj};
    env.stubs[kSlowPeds]={fpv,fpp,dist,add,isPlayer,anger,ref,clean};
    env.stubs[kWeaveCars]={bound,weaveCar};
    env.stubs[kWeaveObjs]={getmat,weaveObj};
    env.stubs[kWeavePeds]={weavePed};
    for(const StubSpec* s:{&bound,&dist,&slowOther,&slowObj,&weaveCar,&weaveObj,&weavePed,&getmat,&fpv,&fpp,&ref,&clean,&add,&isPlayer,&anger})env.watches.emplace_back(s->pair.exe,16);
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f])stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    modelTable=uintptr_t(&CModelInfo::ms_modelInfoPtrs[0]);
    {
        const u32* table=reinterpret_cast<const u32*>(modelTable);
        u32 found=0;
        for(u32 i=kModelBase;i+kEnts<=u32(CModelInfo::NUM_MODEL_INFOS)&&!found;++i) {
            bool free=true;for(u32 k=0;k<kEnts;++k)free&=table[i+k]==0;
            if(free)found=i;
        }
        modelBase=found;ready&=found!=0&&found+kEnts<0x7FFF;
        miSpecial[0]=u16(ModelIndices::MI_ROADWORKBARRIER1);miSpecial[1]=u16(ModelIndices::MI_ROADBLOCKFUCKEDCAR1);miSpecial[2]=u16(ModelIndices::MI_ROADBLOCKFUCKEDCAR2);
        for(int a=0;a<3;++a){ready&=miSpecial[a]<0x7FFF;for(int b=a+1;b<3;++b)ready&=miSpecial[a]!=miSpecial[b];ready&=miSpecial[a]<modelBase||miSpecial[a]>=modelBase+kEnts;}
    }
    ready&=CwBLayout::MatrixLayoutMatches()&&CwBLayout::WorldLayoutMatches();
    focusIndex=CWorld::PlayerInFocus;ready&=focusIndex<MAX_PLAYERS;
    {
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        ready&=arena;
        if(!arena)Write({{"event","layout_error"},{"reason","arena del banco por encima de 0x80000000"}});
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
