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
#include "SimpleTransform.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarCtrlWeave63A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco rutas de CCarCtrl; los colaboradores con efectos se sustituyen por falsos identicos en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL).

// Disposicion de los objetos reales frente a las constantes de Fixtures.hpp.
namespace CwALayout {
using namespace CwAFix;
struct MiProbe : CBaseModelInfo { static constexpr size_t ColOffset() { return offsetof(MiProbe, m_pColModel); } };
static_assert(offsetof(CPlaceable, m_placement) == kOffPos && offsetof(CPlaceable, m_matrix) == kOffMatrix, "CPlaceable");
static_assert(offsetof(CEntity, m_nModelIndex) == kOffModel, "CEntity::m_nModelIndex");
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == kOffSpeed, "CPhysical::m_vecMoveSpeed");
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390, "CVehicle::m_autoPilot");
static_assert(0x390 + offsetof(CAutoPilot, m_nTimeSwitchedToRealPhysics) == kOffTimeSw && 0x390 + offsetof(CAutoPilot, m_nCarDrivingStyle) == kOffStyle && 0x390 + offsetof(CAutoPilot, m_nCarMission) == kOffMission
              && 0x390 + offsetof(CAutoPilot, m_nTempAction) == kOffTempAct && 0x390 + offsetof(CAutoPilot, m_nTempActionTime) == kOffTempTime && 0x390 + offsetof(CAutoPilot, m_nCarCtrlFlags) == kOffCtrl
              && 0x390 + offsetof(CAutoPilot, m_TargetEntity) == kOffTarget && 0x390 + offsetof(CAutoPilot, m_ObstructingEntity) == kOffObstr, "CAutoPilot");
static_assert(offsetof(CColModel, m_boundBox) == 0 && offsetof(CBoundingBox, m_vecMin) == 0 && offsetof(CBoundingBox, m_vecMax) == 0xC, "CColModel::m_boundBox");
static_assert(MiProbe::ColOffset() == 0x14, "CBaseModelInfo::m_pColModel");
static_assert(sizeof(CMatrix) <= kMatWords * 4, "CMatrix");
inline bool MatrixLayoutMatches() {
    CMatrix m{};
    const uintptr_t b = uintptr_t(&m);
    return uintptr_t(&m.GetRight()) - b == 0 && uintptr_t(&m.GetForward()) - b == 0x10 && uintptr_t(&m.GetUp()) - b == 0x20 && uintptr_t(&m.GetPosition()) - b == 0x30;
}
}

namespace CwALive {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlweave63a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlweave63a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlweave63a");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH de las funciones bajo prueba: el constructor deja el hook apagado.
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CCarCtrl",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Colaboradores: cada uno con su ruta RH (apagada) si tiene hook; el cuerpo de la DLL se parchea ademas de la entrada del EXE.
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
    const StubSpec fpv=mk({"CWorld","FindPlayerVehicle",0x56E0D0,reinterpret_cast<void*>(&FkFpv),0});
    const StubSpec atan=mk({"CGeneral","GetATanOfXY",0x53CC70,reinterpret_cast<void*>(&FkAtan),0});
    const StubSpec sways=mk({"CBaseModelInfo","SwaysInWind",0x4212C0,reinterpret_cast<void*>(&FkSways),0});
    const StubSpec getmat=mk({"CPlaceable","GetMatrix",0x411990,reinterpret_cast<void*>(&FkGetMat),0});
    const StubSpec alloc=mk({"CPlaceable","AllocateMatrix",0x54F560,reinterpret_cast<void*>(&FkAlloc),0});
    const StubSpec upd=mk({"CSimpleTransform","UpdateMatrix",0x54F1B0,reinterpret_cast<void*>(&FkUpd),Body(std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix))});
    const StubSpec ref=mk({"CEntity","RegisterReference-",0x571B70,reinterpret_cast<void*>(&FkRef),0});
    const StubSpec sw=mk({"CCarCtrl","SwitchVehicleToRealPhysics",0x423FC0,reinterpret_cast<void*>(&FkSwitch),0});
    const StubSpec tcb=mk({"CCarCtrl","TestCollisionBetween2MovingRects",0x425B30,reinterpret_cast<void*>(&FkTcB),0});
    const StubSpec tcf=mk({"CCarCtrl","TestCollisionBetween2MovingRects_OnlyFrontBumper",0x425F70,reinterpret_cast<void*>(&FkTcF),0});
    const StubSpec sweep=mk({nullptr,nullptr,0x421A50,reinterpret_cast<void*>(&FkSweep),0});
    env.stubs[kTc]={};
    env.stubs[kFb]={};
    env.stubs[kSlow]={tcf,tcb,alloc,upd,ref,getmat,fpv,sw};
    env.stubs[kWoc]={fpv,sweep};
    env.stubs[kWfo]={sways,alloc,upd,atan};
    for(const StubSpec* s:{&fpv,&atan,&sways,&getmat,&alloc,&upd,&ref,&sw,&tcb,&tcf,&sweep})env.watches.emplace_back(s->pair.exe,16);
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f])stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    // Indices de modelo propios (kObjs entradas nulas consecutivas) e indices reales de los modelos especiales
    modelTable=uintptr_t(&CModelInfo::ms_modelInfoPtrs[0]);
    {
        const u32* table=reinterpret_cast<const u32*>(modelTable);
        u32 found=0;
        for(u32 i=kModelBase;i+kObjs<=u32(CModelInfo::NUM_MODEL_INFOS)&&!found;++i) {
            bool free=true;for(u32 k=0;k<kObjs;++k)free&=table[i+k]==0;
            if(free)found=i;
        }
        modelBase=found;ready&=found!=0&&found+kObjs<0x7FFF;
        miSpecial[0]=u16(ModelIndices::MI_TRAFFICLIGHTS);miSpecial[1]=u16(ModelIndices::MI_SINGLESTREETLIGHTS1);miSpecial[2]=u16(ModelIndices::MI_SINGLESTREETLIGHTS2);
        miSpecial[3]=u16(ModelIndices::MI_SINGLESTREETLIGHTS3);miSpecial[4]=u16(ModelIndices::MI_DOUBLESTREETLIGHTS);
        // los especiales deben ser distintos entre si y de los propios, y positivos (movsx)
        for(int a=0;a<5;++a){ready&=miSpecial[a]<0x7FFF;for(int b=a+1;b<5;++b)ready&=miSpecial[a]!=miSpecial[b];ready&=miSpecial[a]<modelBase||miSpecial[a]>=modelBase+kObjs;}
    }
    ready&=CwALayout::MatrixLayoutMatches();
    {
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        ready&=arena;
        if(!arena)Write({{"event","layout_error"},{"reason","arena del banco por encima de 0x80000000"}});
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
