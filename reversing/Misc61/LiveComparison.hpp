#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "Memory/MemoryHeap.h"
#include "SearchLight.h"
#include "TrainNode.h"
#include "Tasks/TaskTypes/TaskSimpleSlideToCoord.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleRunNamedAnim.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "General.h"
#include "World.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par Misc61 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.
// Cuatro rutas: CMemoryHeap::IntegrityCheck (0x72E8E0), CSearchLight::IsSpottedEntity (0x493900), CTaskSimpleSlideToCoord::ProcessPed (0x66C4E0, hook VIRTUAL: el cuerpo del candidato
// sale de ReversibleHook::Virtual::GetHookOurAddress()) y CTrainNode::GetLightingFromCollision (0x6F5F80).

// Disposicion de los objetos reales frente a las constantes de Fixtures.hpp (los miembros protegidos se miden con una derivada de prueba).
namespace Misc61Layout {
struct HeapProbe : CMemoryHeap {
    static constexpr size_t OffFirst() { return offsetof(HeapProbe, m_FirstBlock); }
    static constexpr size_t OffLast() { return offsetof(HeapProbe, m_LastBlock); }
    static constexpr size_t OffBegin() { return offsetof(HeapProbe, m_FreeListBegin); }
    static constexpr size_t OffEnd() { return offsetof(HeapProbe, m_FreeListEnd); }
    static constexpr size_t OffCommon() { return offsetof(HeapProbe, m_paCommonSizes); }
};
struct PedProbe : CPed {
    static constexpr size_t OffIntel() { return offsetof(PedProbe, m_pIntelligence); }
};
static_assert(sizeof(CMemoryHeap) == Misc61Fix::kHeapThisBytes && HeapProbe::OffFirst() == 0 && HeapProbe::OffLast() == 4 && HeapProbe::OffBegin() == 8 && HeapProbe::OffEnd() == 0x20 && HeapProbe::OffCommon() == 0x38, "CMemoryHeap");
static_assert(sizeof(HeapFreeBlockDesc) == Misc61Fix::kFreeNodeBytes && offsetof(HeapFreeBlockDesc, m_Next) == 0x10 && sizeof(CommonSize) == Misc61Fix::kCommonSizeBytes, "HeapFreeBlockDesc / CommonSize");
static_assert(offsetof(CommonSize, m_ListStart) == 0 && offsetof(CommonSize, m_ListEnd) == 0x18, "CommonSize");
static_assert(offsetof(HeapBlockDesc, m_nSize) == 0 && offsetof(HeapBlockDesc, m_bInUse) == 4 && offsetof(HeapBlockDesc, m_nMemId) == 6 && offsetof(HeapBlockDesc, m_PrevBlock) == 0xC, "HeapBlockDesc");
static_assert(sizeof(CEntity) == Misc61Fix::kEntityBytes && offsetof(CPlaceable, m_placement) == Misc61Fix::kEntityPlacementAt && offsetof(CPlaceable, m_matrix) == Misc61Fix::kEntityMatrixAt, "CEntity / CPlaceable");
static_assert(sizeof(CTrainNode) == Misc61Fix::kNodeBytes && offsetof(CTrainNode, x) == 0 && offsetof(CTrainNode, y) == 2 && offsetof(CTrainNode, z) == 4
              && offsetof(CTrainNode, m_nSurfaceLighting) == Misc61Fix::kNodeLightAt && offsetof(CTrainNode, m_bSurfLightingFound) == Misc61Fix::kNodeFoundAt, "CTrainNode");
static_assert(sizeof(CColPoint) == 0x2C && offsetof(CColPoint, m_nLightingB) == 0x25, "CColPoint");
static_assert(sizeof(CTaskSimpleSlideToCoord) == Misc61Fix::kTaskBytes && offsetof(CTaskSimpleSlideToCoord, m_SlideToPos) == Misc61Fix::kTaskPosAt && offsetof(CTaskSimpleSlideToCoord, m_fAimingRotation) == Misc61Fix::kTaskAimAt
              && offsetof(CTaskSimpleSlideToCoord, m_Speed) == Misc61Fix::kTaskSpeedAt && offsetof(CTaskSimpleSlideToCoord, m_Timer) == Misc61Fix::kTaskTimerAt, "CTaskSimpleSlideToCoord");
static_assert(sizeof(CPed) == Misc61Fix::kPedBytes, "sizeof(CPed)");
static_assert(PedProbe::OffIntel() == Misc61Fix::kPedIntelAt, "CPed::m_pIntelligence");
static_assert(offsetof(CPed, m_vecAnimMovingShiftLocal) == Misc61Fix::kPedShiftAt, "CPed::m_vecAnimMovingShiftLocal");
static_assert(offsetof(CPed, m_fCurrentRotation) == Misc61Fix::kPedCurRotAt, "CPed::m_fCurrentRotation");
static_assert(offsetof(CPed, m_fAimingRotation) == Misc61Fix::kPedAimRotAt, "CPed::m_fAimingRotation");
static_assert(offsetof(CPed, m_nPedType) == Misc61Fix::kPedTypeAt, "CPed::m_nPedType");
static_assert(offsetof(CPedIntelligence, m_TaskMgr) + offsetof(CTaskManager, m_aPrimaryTasks) + sizeof(CTask*) * TASK_PRIMARY_DEFAULT == Misc61Fix::kIntelPrimaryDefaultAt, "CPedIntelligence::m_TaskMgr");
static_assert(STAND_STILL_TIME == Misc61Fix::kStandTime && ABORT_PRIORITY_IMMEDIATE == 2 && SCRIPT_THING_SEARCH_LIGHT == 2, "constantes de las llamadas");
}

namespace Misc61Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Cuerpo de la DLL de un hook VIRTUAL (0 si no esta registrado como virtual)
inline uintptr_t VirtualBody(const char* cls,const char* fn) {
    auto* h=FindHook(cls,fn);
    return h&&!strcmp(h->Symbol(),"V")?Body(uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress())):0;
}
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"misc61-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"misc61-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("misc61");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH: se construyen antes que nada; el constructor deja el hook apagado.
    auto simple=[&](unsigned idx,const char* cls,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute(cls,fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"CMemoryHeap","IntegrityCheck",0x72E8E0);
    simple(1,"CSearchLight","IsSpottedEntity",0x493900);
    {   // hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los Simple, asi que el cuerpo del candidato se toma del hook Virtual
        const uintptr_t body=VirtualBody("CTaskSimpleSlideToCoord","ProcessPed");
        env.route[2].reset(new HookRoute("CTaskSimpleSlideToCoord","ProcessPed",0x66C4E0,body));
        registered[kFn[2]]=env.route[2]->Registered();bodies[kFn[2]]=env.route[2]->Body()!=0;
        ready&=env.route[2]->Registered()&&env.route[2]->Body()&&ModuleOf(env.route[2]->Body())==env.asi;
    }
    simple(3,"CTrainNode","GetLightingFromCollision",0x6F5F80);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    using Pvl = bool(*)(const CVector&,float,CColPoint&,CEntity*&,bool,bool,bool,bool,bool,bool,CStoredCollPoly*);
    env.stubs[1]={{Pair{0x4839A0,DllBody(&CTheScripts::GetActualScriptThingIndex),true},reinterpret_cast<void*>(&FakeScriptIndex)},
                  {Pair{0x493280,0,false},reinterpret_cast<void*>(&FakeSpotted)}};   // 0x493280 sin reconstruir: la DLL llama a la direccion del EXE
    env.stubs[2]={{Pair{0x61BF20,VirtualBody("CTaskSimpleRunNamedAnim","ProcessPed"),true},reinterpret_cast<void*>(&FakeAnim)},
                  {Pair{0x62F370,VirtualBody("CTaskSimpleStandStill","ProcessPed"),true},reinterpret_cast<void*>(&FakeStand)},
                  {Pair{0x53CB50,DllBody(&CGeneral::LimitRadianAngle),true},reinterpret_cast<void*>(&FakeLimit)}};
    env.stubs[3]={{Pair{0x5674E0,DllBody(static_cast<Pvl>(&CWorld::ProcessVerticalLine)),true},reinterpret_cast<void*>(&FakePvl)}};
    for(unsigned f=0;f<4;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    {   // CMatrix tiene miembros privados: se mide sobre un bufer sin construir (solo aritmetica de punteros)
        alignas(16) static uint8 raw[sizeof(CMatrix)];
        auto* m=reinterpret_cast<CMatrix*>(raw);
        const auto off=[&](const void* p){return size_t(reinterpret_cast<const uint8*>(p)-raw);};
        const bool layout=sizeof(CMatrix)>=Misc61Fix::kMatrixBytes&&off(&m->GetRight())==0&&off(&m->GetForward())==0x10&&off(&m->GetPosition())==Misc61Fix::kMatrixPosAt;
        if(!layout)Write({{"event","layout_error"},{"reason","disposicion de CMatrix distinta de Fixtures.hpp"}});
        ready&=layout;
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
