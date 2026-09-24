#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AECollisionAudioEntity.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace BulletLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { log << r.dump() << '\n'; log.flush(); }
inline json Signature(json r) { for (auto& sound : r) sound.erase("id"); return r; }
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir/"bullet-hit-runtime.start")) { done=true; return; }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done=true; log.open(dir/"bullet-hit-runtime.jsonl");
    using Hook=ReversibleHooks::ReversibleHook::Base;
    Hook* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if(c.Name()=="CAECollisionAudioEntity") for(auto& h:c.Items())
            if(h->Name()=="PlayBulletHitCollisionSound") hook=h.get();
    });
    if (!hook) { Write({{"event","error"},{"reason","missing hook"}}); return; }
    const auto enabled=hook->Hooked(),locked=hook->Locked();
    Write({{"event","hook_state"},{"enabled",enabled},{"locked",locked}});
    hook->LockState(false);
    CAECollisionAudioEntity entity;
    const CVector position{5.f,-7.f,11.f};
    bool passed=true; int pairs=0, bounds=0, reports=0;
    BulletProbe::active=true;
    const auto Run=[&](int surface,float angle,int last,bool candidate) {
        hook->State(candidate); entity.m_LastBulletHitSoundID=last;
        BulletProbe::captured=json::array();
        const auto before=BulletProbe::calls;
        plugin::CallMethod<0x4DB7C0>(&entity,static_cast<eSurfaceType>(surface),&position,angle);
        const auto r=BulletProbe::captured;
        const bool noRepeat=r.empty() || int(r[0]["id"])!=last;
        const bool counter=BulletProbe::calls-before==(candidate?1u:0u);
        passed=passed&&noRepeat&&counter;
        return r;
    };
    // Normal surfaces: same sound family and non-random parameters as the EXE.
    // +/-90 degrees makes the metal ricochet probability deterministic (0/1).
    for(int surface=0;surface<179&&passed;++surface) for(float angle:{-90.f,90.f})
        for(int last=-1;last<=21&&passed;++last) {
            const auto a=Run(surface,angle,last,false),b=Run(surface,angle,last,true);
            passed=passed && a.size()==1 && b.size()==1 && Signature(a)==Signature(b);
            Write({{"event","comparison"},{"case",pairs++},{"surface",surface},{"angle",angle},
                {"last",last},{"original",a},{"candidate",b},{"equal",passed}});
        }
    // All byte values, including reserved gaps, every audio-only ID and NONE.
    for(int surface=0;surface<256&&passed;++surface) {
        const auto r=Run(surface,90.f,-1,true);
        const bool valid=surface<179 || surface==188;
        bool equal=r.size()==(valid?1u:0u);
        if(surface==188) equal=equal&&Signature(r)==Signature(Run(SURFACE_CAR,90.f,-1,false));
        passed=passed&&equal;
        Write({{"event","bounds"},{"surface",surface},{"sounds",r.size()},{"equal",equal}}); ++bounds;
    }
    hook->State(true);
    // Minimal vehicle fixture: only type/subtype are read by ReportBulletHit.
    alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> storage{};
    auto* vehicle=reinterpret_cast<CVehicle*>(storage.data());
    vehicle->SetTypeVehicle();
    for(const bool bank:{false,true}) for(const auto subtype:{VEHICLE_TYPE_BMX,VEHICLE_TYPE_AUTOMOBILE}) {
        BulletProbe::bankLoaded=bank; vehicle->m_nVehicleSubType=subtype;
        entity.m_LastBulletHitSoundID=-1; BulletProbe::captured=json::array();
        entity.ReportBulletHit(vehicle,SURFACE_DEFAULT,position,90.f);
        const auto r=BulletProbe::captured;
        const bool equal=bank ? r.size()==1 && Signature(r)==Signature(Run(SURFACE_CAR,90.f,-1,false)) : r.empty();
        hook->State(true); passed=passed&&equal;
        Write({{"event","report"},{"bank",bank},{"subtype",int(subtype)},{"equal",equal}}); ++reports;
    }
    BulletProbe::active=false;
    hook->State(enabled); hook->LockState(locked);
    const bool restored=hook->Hooked()==enabled&&hook->Locked()==locked;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"bounds",bounds},
        {"reports",reports},{"state_restored",restored}});
}
}
