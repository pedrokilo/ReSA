#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "CarCtrl.h"
#include "Heli.h"
#include "Plane.h"
#include "SimpleTransform.h"
#include "World.h"
#include "General.h"
#include "X87Math.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarCtrlAir63 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cada funcion de CCarCtrl (parte A) se ejecuta con el hook RH apagado (EXE) y encendido (DLL); todos los colaboradores se sustituyen por falsos identicos en ambos lados
// (parche en la entrada del EXE y en el cuerpo de la DLL).
namespace A63Layout {
static_assert(offsetof(CHeli, m_fLeftRightSkid) == 0x98C && offsetof(CHeli, m_fSteeringUpDown) == 0x990 && offsetof(CHeli, m_fSteeringLeftRight) == 0x994 && offsetof(CHeli, m_fAccelerationBreakStatus) == 0x998
              && offsetof(CHeli, m_fMaxAltitude) == 0x9A8 && offsetof(CHeli, m_fMinAltitude) == 0x9B0 && offsetof(CHeli, field_9B4) == 0x9B4 && offsetof(CHeli, field_A14) == 0xA14 && sizeof(CHeli) == 0xA18, "CHeli");
static_assert(offsetof(CPlane, m_fLeftRightSkid) == 0x988 && offsetof(CPlane, m_fSteeringUpDown) == 0x98C && offsetof(CPlane, m_fSteeringLeftRight) == 0x990 && offsetof(CPlane, m_fAccelerationBreakStatus) == 0x994
              && offsetof(CPlane, m_maxAltitude) == 0x9A8 && offsetof(CPlane, m_planeHeading) == 0x9B4, "CPlane");
static_assert(offsetof(CPlane, m_planeCreationHeading) == 0x9A4 && offsetof(CPlane, m_altitude) == 0x9AC && offsetof(CPlane, m_minAltitude) == 0x9B0 && offsetof(CPlane, m_planeHeadingPrev) == 0x9B8
              && offsetof(CPlane, m_forwardZ) == 0x9BC && offsetof(CPlane, m_nStartedFlyingTime) == 0x9C0 && offsetof(CPlane, m_fLandingGearStatus) == 0x9CC && offsetof(CPlane, m_fSteeringFactor) == 0x99C
              && offsetof(CPlane, m_fAccelerationBreakStatusPrev) == 0x998, "CPlane 2");
static_assert(offsetof(CHeli, m_autoPilot) + offsetof(CAutoPilot, m_vecDestinationCoors) == 0x3EC && offsetof(CHeli, m_autoPilot) + offsetof(CAutoPilot, m_TargetEntity) == 0x41C
              && offsetof(CHeli, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA && offsetof(CHeli, m_autoPilot) + offsetof(CAutoPilot, m_nTempAction) == 0x3BB
              && offsetof(CHeli, m_autoPilot) + offsetof(CAutoPilot, m_nTempActionTime) == 0x3BC, "CAutoPilot");
static_assert(offsetof(CHeli, m_nRandomSeed) == 0x20 && offsetof(CHeli, m_matrix) == 0x14 && offsetof(CHeli, m_vecMoveSpeed) == 0x44 && offsetof(CHeli, m_fHealth) == 0x4C0
              && offsetof(CHeli, m_fWheelsSuspensionCompression) == 0x7D4 && offsetof(CHeli, m_fForcedOrientation) == 0x954, "CVehicle/CAutomobile");
}

namespace A63Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlair63-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlair63-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlair63");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH de las funciones bajo prueba: el constructor deja el hook apagado.
    for(unsigned f:kActive) {
        env.route[f].reset(new HookRoute("CCarCtrl",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Colaboradores: cada uno con su ruta RH (apagada) si tiene hook; el cuerpo de la DLL se parchea ademas de la entrada del EXE.
    struct C {const char* cls;const char* fn;uintptr_t exe;void* fake;uintptr_t body;};
    auto mk=[&](const C& c){
        Pair p{c.exe,c.body,false};
        // funcion de CCarCtrl que tambien se prueba en este banco: se reutiliza su ruta (dos rutas sobre el mismo hook dejarian mal su estado final)
        for(unsigned f:kActive)
            if(c.cls&&!strcmp(c.cls,"CCarCtrl")&&!strcmp(c.fn,kName[f])&&env.route[f]) {p.dll=Body(env.route[f]->Body());return StubSpec{p,c.fake};}
        if(c.cls) {
            env.collab.emplace_back(new HookRoute(c.cls,c.fn,c.exe,HookRoute::RegisteredTarget{}));
            if(!p.dll&&env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
            else if(!env.collab.back()->Registered()&&!c.body)p.dll=0;
        }
        return StubSpec{p,c.fake};
    };
    const StubSpec atan=mk({"CGeneral","GetATanOfXY",0x53CC70,reinterpret_cast<void*>(&FkAtan),0});
    const StubSpec flyHeliDir=mk({"CCarCtrl","FlyAIHeliInCertainDirection",0x429A70,reinterpret_cast<void*>(&FkFlyHeliDir),0});
    const StubSpec flyHeliFix=mk({"CCarCtrl","FlyAIHeliToTarget_FixedOrientation",0x423940,reinterpret_cast<void*>(&FkFlyHeliFix),0});
    const StubSpec flyPlane=mk({"CCarCtrl","FlyAIPlaneInCertainDirection",0x423000,reinterpret_cast<void*>(&FkFlyPlane),0});
    const StubSpec dog=mk({"CCarCtrl","GetAIPlaneToDoDogFight",0x429890,reinterpret_cast<void*>(&FkDog),0});
    const StubSpec norm=mk({nullptr,nullptr,0x59C910,reinterpret_cast<void*>(&FkNorm),0});
    const StubSpec rnd=mk({nullptr,nullptr,0x821B1E,reinterpret_cast<void*>(&FkRand),0});
    const StubSpec speed=mk({"CWorld","FindPlayerSpeed",0x56E090,reinterpret_cast<void*>(&FkSpeed),0});
    const StubSpec app=mk({"CVehicle","GetVehicleAppearance",0x6D1080,reinterpret_cast<void*>(&FkApp),Body(std::bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance))});
    const StubSpec missile=mk({"CCarCtrl","PossiblyFireHSMissile",0x429600,reinterpret_cast<void*>(&FkMissile),0});
    const StubSpec testFire=mk({"CCarCtrl","TestWhetherToFirePlaneGuns",0x429520,reinterpret_cast<void*>(&FkTest),0});
    const StubSpec alloc=mk({"CPlaceable","AllocateMatrix",0x54F560,reinterpret_cast<void*>(&FkAlloc),0});
    const StubSpec upd=mk({"CSimpleTransform","UpdateMatrix",0x54F1B0,reinterpret_cast<void*>(&FkUpd),Body(std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix))});
    const StubSpec taskMgr=mk({"CTaskManager","GetActiveTask",0x681720,reinterpret_cast<void*>(&FkTaskMgr),0});
    const StubSpec follow=mk({"CCarCtrl","SteerAIHeliToFollowEntity",0x42A750,reinterpret_cast<void*>(&FkFollow),0});
    const StubSpec fpe=mk({"CWorld","FindPlayerEntity",0x56E120,reinterpret_cast<void*>(&FkFpe),0});
    const StubSpec rockets=mk({"CCarCtrl","FireHeliRocketsAtTarget",0x42B270,reinterpret_cast<void*>(&FkRockets),0});
    const StubSpec boat=mk({nullptr,nullptr,0x73DF00,reinterpret_cast<void*>(&FkBoat),0});
    const StubSpec distLine=mk({"CCollision","DistToMathematicalLine",0x412970,reinterpret_cast<void*>(&FkDistLine),0});
    const StubSpec addProj=mk({"CProjectileInfo","AddProjectile",0x737C80,reinterpret_cast<void*>(&FkAddProj),0});
    const StubSpec los=mk({"CWorld","ProcessLineOfSight",0x56BA00,reinterpret_cast<void*>(&FkLos),0});
    const StubSpec probe=mk({nullptr,nullptr,0x422F80,reinterpret_cast<void*>(&FkProbe),0});
    const StubSpec gear=mk({"CPlane","SetGearUp",0x6CAC20,reinterpret_cast<void*>(&FkGear),0});
    const StubSpec trig=mk({"CCarCtrl","TriggerDogFightMoves",0x429300,reinterpret_cast<void*>(&FkTrig),0});
    const StubSpec fpv=mk({"CWorld","FindPlayerVehicle",0x56E0D0,reinterpret_cast<void*>(&FkFpv),0});
    const StubSpec fpp=mk({"CWorld","FindPlayerPed",0x56E210,reinterpret_cast<void*>(&FkFpp),0});
    const StubSpec fpc=mk({"CWorld","FindPlayerCoors",0x56E010,reinterpret_cast<void*>(&FkFpc),0});
    env.stubs[kHeliFlyDir]={flyHeliDir};
    env.stubs[kDogAgainstPlayer]={fpv,fpp,dog};
    env.stubs[kHeliCrash]={};
    env.stubs[kPlaneCrash]={};
    env.stubs[kPlaneTowards]={atan,flyPlane};
    env.stubs[kPlaneFollow]={atan,flyPlane};
    env.stubs[kHeliTowards]={atan,flyHeliDir,flyHeliFix};
    env.stubs[kHeliAway]={fpc,atan,flyHeliDir};
    env.stubs[kHeliLand]={atan,flyHeliDir};
    env.stubs[kHeliPolice]={atan,flyHeliDir};
    env.stubs[kTestFire]={norm};
    env.stubs[kDogMoves]={norm,rnd};
    env.stubs[kPlaneAttack]={speed,fpc,atan,flyPlane,fpv,app,trig,testFire,missile};
    env.stubs[kPlaneDog]={atan,rnd,flyPlane,testFire,missile};
    env.stubs[kHeliFollow]={atan,flyHeliDir,flyHeliFix,alloc,upd,taskMgr};
    env.stubs[kHeliView]={atan,rnd,follow};
    env.stubs[kHeliAttack]={fpc,atan,flyHeliDir,testFire,fpe,rockets};
    env.stubs[kBoatHead]={atan};
    env.stubs[kBoatAttack]={fpc,speed,atan,boat};
    env.stubs[kBoatCircle]={fpc,norm,atan};
    env.stubs[kRockets]={distLine,addProj};
    env.stubs[kFixed]={norm,los,atan,rnd};
    env.stubs[kPlaneDir]={atan,probe,gear};
    env.stubs[kHeliDir]={norm,los,atan,rnd};
    for(const StubSpec* s:{&atan,&flyHeliDir,&flyHeliFix,&flyPlane,&dog,&fpv,&fpp,&fpc,&norm,&rnd,&speed,&app,&missile,&testFire,&trig,&alloc,&upd,&taskMgr,&follow,&fpe,&rockets,&boat,&distLine,&addProj,&los,&probe,&gear})env.watches.emplace_back(s->pair.exe,16);
    for(unsigned f:kActive) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f])stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    {
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        ready&=arena;
        if(!arena)Write({{"event","layout_error"},{"reason","arena del banco por encima de 0x80000000"}});
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
