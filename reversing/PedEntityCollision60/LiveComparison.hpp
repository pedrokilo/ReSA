#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Physical.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Collision/ColModel.h"
#include "Camera.h"
#include "Collision/TempColModels.h"
#include "SurfaceInfos_c.h"
#include "CutsceneMgr.h"
#include "SimpleTransform.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PedEntityCollision60 dentro del juego: Update() se llama desde CGame::Process (build_probe.py). Todo el nucleo esta en Core.hpp.
// Ruta: hook VIRTUAL de CPed (HookRoute::RegisteredTarget solo conoce los Simple): el cuerpo del candidato sale de ReversibleHook::Virtual::GetHookOurAddress().
namespace PE = PedEntityCollision60Fix;
// Los desplazamientos de las fixtures frente a los tipos reales del juego.
static_assert(offsetof(CPhysical, m_nPhysicalFlags) == 0x40 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44 && offsetof(CPhysical, m_pAttachedTo) == 0xFC && offsetof(CPhysical, m_pEntityIgnoredCollision) == 0x128, "CPhysical");
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CEntity, m_nModelIndex) == 0x22 && offsetof(CEntity, m_nFlags) == 0x1C, "CEntity +0x14 +0x1C +0x22");
static_assert(offsetof(CPed, m_standingOnEntity) == 0x568 && offsetof(CPed, field_588) == 0x588 && offsetof(CPed, m_pVehicle) == 0x58C && offsetof(CPed, m_nPedType) == 0x598 && offsetof(CPed, m_nPedState) == 0x530, "CPed +0x530 +0x568 +0x588 +0x58C +0x598");
static_assert(offsetof(CPed, m_pContactEntity) == 0x584 && offsetof(CPed, field_56C) == 0x56C && offsetof(CPed, field_578) == 0x578 && offsetof(CPed, m_nContactSurface) == 0xBA, "CPed +0x56C +0x578 +0x584 +0xBA");
static_assert(sizeof(CPed) == PE::kPedBytes && sizeof(CColPoint) == 0x2C, "CPed 0x79C, CColPoint 0x2C");
static_assert(offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(CBaseModelInfo, m_pColModel) == 0x14, "CVehicle +0x594, CBaseModelInfo +0x14");
struct PedEntityCollision60PedProbe : CPed {   // los miembros protegidos se miden desde una clase derivada
    static constexpr size_t Intel() { return offsetof(PedEntityCollision60PedProbe, m_pIntelligence); }
};
static_assert(PedEntityCollision60PedProbe::Intel() == 0x47C, "CPed::m_pIntelligence +0x47C");
namespace PedEntityCollision60Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Disposicion en memoria viva de lo que el banco escribe/lee por direccion absoluta (comprobada una vez; si algo no cuadra no se corre ningun caso).
inline json LayoutCheck(bool& ok) {
    json j;
    auto chk=[&](const char* name,bool v){j[name]=v;ok&=v;};
    chk("cam_active",uintptr_t(&TheCamera.m_nActiveCam)==PE::kGCamActive);
    chk("cam_mode",uintptr_t(&TheCamera.m_aCams[0].m_nMode)==PE::kGCamMode);
    chk("cam_front",uintptr_t(&TheCamera.m_aCams[0].m_vecFront)==PE::kGCamFront);
    chk("cam_stride",uintptr_t(&TheCamera.m_aCams[1])-uintptr_t(&TheCamera.m_aCams[0])==PE::kGCamStride);
    chk("temp_colmodel",uintptr_t(&CTempColModels::ms_colModelPed2)==PE::kGTempCm);
    chk("timestep",uintptr_t(&CTimer::ms_fTimeStep)==PE::kGTimestep);
    chk("time_ms",uintptr_t(&CTimer::m_snTimeInMilliseconds)==PE::kGTimeMs);
    chk("model_info",uintptr_t(&CModelInfo::ms_modelInfoPtrs[0])==PE::kGModelInfo);
    chk("surface_infos",uintptr_t(&g_surfaceInfos)==PE::kGSurface);
    return j;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pedentitycollision60-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pedentitycollision60-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pedentitycollision60");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    InitSlots();
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    gAsiBase=env.asi;
    gUpdMatFallback=std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix);
    gRotZFallback=std::bit_cast<uintptr_t>(&CMatrix::SetRotateZOnly);
    bool ready=true;json registered=json::object();json bodies=json::object();
    {
        auto* hook=FindHook("CPed","ProcessEntityCollision");
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        env.route.reset(new HookRoute("CPed","ProcessEntityCollision",0x5E2530,body));
        registered["CPed::ProcessEntityCollision"]=env.route->Registered();bodies["CPed::ProcessEntityCollision"]=env.route->Body();
        ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    }
    bool layoutOk=true;const json layout=LayoutCheck(layoutOk);
    ready&=layoutOk;
    Run(env,ready,registered,bodies,layout);
}
}
