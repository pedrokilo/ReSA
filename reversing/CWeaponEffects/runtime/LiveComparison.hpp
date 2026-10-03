#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"
#include "CarGenerator.h"
namespace WeaponEffectsLive {
inline bool done{};inline std::ofstream log;
inline void Write(const json& row){ { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } }log<<row.dump()<<'\n';log.flush();}
inline int Report(int type,char* message,int*){if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",message?message:""}});ExitProcess(91);}
inline bool Select(bool enabled){bool found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){if(category.Name()!="CWeaponEffects")return;for(auto& h:category.Items())if(h->Name()=="Render"){category.SetItemEnabled(h,enabled);found=h->Hooked()==enabled;}});return found;}
inline RwRenderStateSetFunction originalSet{};
inline RwBool Set(RwRenderState key,void* value){WeaponEffectsProbe::Record("state",{}, {key,(uintptr_t)value});return originalSet(key,value);}
struct State {
    std::array<CWeaponEffects,2> savedHair=gCrossHair;
    std::array<uint8,24> globals=StaticRef<std::array<uint8,24>>(0xC8A890);
    std::array<bool,2> expanding=StaticRef<std::array<bool,2>>(0x8D6144);
    uint32 time=CTimer::GetTimeInMS();bool wide=TheCamera.m_bWideScreenOn;
    CPlayerPed* p0=CWorld::Players[0].m_pPed;CPlayerPed* p1=CWorld::Players[1].m_pPed;
    uint32 aux0=CWorld::Players[0].m_nCrosshairActivated,aux1=CWorld::Players[1].m_nCrosshairActivated;
    eCamMode mode=TheCamera.GetActiveCam().m_nMode;
    float aimX=TheCamera.GetActiveCam().m_fX_Targetting,aimY=TheCamera.GetActiveCam().m_fY_Targetting;
    std::array<void*,32> states{};std::array<bool,32> valid{};
    State(){for(int i:{1,6,8,10,11,12,20})valid[i]=RwRenderStateGet((RwRenderState)i,&states[i])!=FALSE;}
    void Restore()const {
        gCrossHair=savedHair;StaticRef<std::array<uint8,24>>(0xC8A890)=globals;StaticRef<std::array<bool,2>>(0x8D6144)=expanding;
        CTimer::m_snTimeInMilliseconds=time;TheCamera.m_bWideScreenOn=wide;
        CWorld::Players[0].m_pPed=p0;CWorld::Players[1].m_pPed=p1;
        CWorld::Players[0].m_nCrosshairActivated=aux0;CWorld::Players[1].m_nCrosshairActivated=aux1;
        auto& cam=TheCamera.GetActiveCam();cam.m_nMode=mode;cam.m_fX_Targetting=aimX;cam.m_fY_Targetting=aimY;
        for(int i=1;i<32;++i)if(valid[i])RwRenderStateSet((RwRenderState)i,states[i]);
    }
};
inline json Snapshot(){
    json numbers=json::array(),exact=json::array();
    for(auto& c:gCrossHair){for(float f:{c.m_vecPosn.x,c.m_vecPosn.y,c.m_vecPosn.z,c.m_fSize,c.field_1C,c.field_20,c.m_fRotation})numbers.push_back(f);exact.push_back({c.m_bActive,c.m_nTimeWhenToDeactivate,c.m_color.r,c.m_color.g,c.m_color.b,c.m_color.a,c.m_bClearImmediately});}
    for(int a=0xC8A898;a<0xC8A8A8;a+=4)numbers.push_back(*reinterpret_cast<float*>(a));
    exact.push_back({StaticRef<uint32>(0xC8A890),StaticRef<uint32>(0xC8A894),StaticRef<bool>(0x8D6144),StaticRef<bool>(0x8D6145)});
    return {{"numbers",numbers},{"exact",exact}};
}
inline bool Equal(const json& a,const json& b,double& error){
    if(a.is_object()){if(a.size()!=b.size())return false;for(auto it=a.begin();it!=a.end();++it){if(!b.contains(it.key()))return false;if(it.key()=="exact"){if(it.value()!=b[it.key()])return false;}else if(!Equal(it.value(),b[it.key()],error))return false;}return true;}
    if(a.is_array()){if(a.size()!=b.size())return false;for(size_t i=0;i<a.size();++i)if(!Equal(a[i],b[i],error))return false;return true;}
    if(a.is_number_float()||b.is_number_float()){if(!a.is_number()||!b.is_number())return false;double x=a.get<double>(),y=b.get<double>();if(!std::isfinite(x)||!std::isfinite(y))return false;error=std::max(error,std::abs(x-y));return std::abs(x-y)<=0.002;}
    return a==b;
}
inline bool Compare(int pair,const char* group){
    State input;const auto before=WeaponEffectsProbe::calls;
    if(!Select(false)){Write({{"event","error"},{"reason","original hook"}});return false;}
    auto call=reinterpret_cast<void(__cdecl*)()>(0x742CF0);
    WeaponEffectsProbe::commands=json::array();WeaponEffectsProbe::recording=true;call();WeaponEffectsProbe::recording=false;
    const auto original=Snapshot(),drawOriginal=WeaponEffectsProbe::commands;const auto middle=WeaponEffectsProbe::calls;
    input.Restore();
    if(!Select(true)){Write({{"event","error"},{"reason","candidate hook"}});return false;}
    WeaponEffectsProbe::commands=json::array();WeaponEffectsProbe::recording=true;call();WeaponEffectsProbe::recording=false;
    const auto candidate=Snapshot(),drawCandidate=WeaponEffectsProbe::commands;const auto after=WeaponEffectsProbe::calls;
    double error{};bool equal=Equal(original,candidate,error)&&Equal(drawOriginal,drawCandidate,error)&&middle==before&&after==middle+1;
    Write({{"event","comparison"},{"pair",pair},{"group",group},{"equal",equal},{"max_error",error},{"original",original},{"candidate",candidate},{"draw_original",drawOriginal},{"draw_candidate",drawCandidate},{"original_candidate_calls",middle-before},{"candidate_calls",after-middle}});
    input.Restore();return equal;
}
inline void Update(){
    if(done)return;char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);const auto dir=std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(dir/"weaponeffects-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"weaponeffects-runtime.jsonl",std::ios::trunc);Write({{"event","waiting_for_player"}});_CrtSetReportHook(Report);}
    auto* p=FindPlayerPed();if(!p||CCutsceneMgr::IsRunning())return;done=true;
    const State saved;originalSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet;RWSRCGLOBAL(dOpenDevice).fpRenderStateSet=Set;
    Write({{"event","player_ready"}});int pair{};bool passed=true;
    const auto& camera=TheCamera.m_mCameraMatrix;
    const auto reset=[&]{saved.Restore();TheCamera.m_bWideScreenOn=false;TheCamera.GetActiveCam().m_nMode=MODE_FOLLOWPED;CTimer::m_snTimeInMilliseconds=10000;CWorld::Players[1].m_pPed=nullptr;CWorld::Players[0].m_nCrosshairActivated=0;CWorld::Players[1].m_nCrosshairActivated=0;gCrossHair={};StaticRef<std::array<uint8,24>>(0xC8A890).fill(0);for(auto& c:gCrossHair){c.m_vecPosn=camera.GetPosition()+camera.GetForward()*30.f;c.m_color=CRGBA(213,91,37,127);c.m_fSize=1.f;c.field_1C=.2f;c.field_20=300.f;c.m_nTimeWhenToDeactivate=-1;}gCrossHair[0].m_bActive=true;};
    for(int c=0;c<5&&passed;++c){reset();if(c==0)TheCamera.m_bWideScreenOn=true;if(c==1)gCrossHair[0].m_bActive=false;if(c==2)CWorld::Players[0].m_pPed=nullptr;if(c==3)gCrossHair[0].m_nTimeWhenToDeactivate=9999;if(c==4){gCrossHair[1].m_bActive=true;}passed=Compare(pair++,"early");}
    for(int slot=0;slot<2&&passed;++slot)for(int size=0;size<4&&passed;++size)for(int expiry=0;expiry<4&&passed;++expiry)for(int rotation=0;rotation<3&&passed;++rotation){reset();CWorld::Players[1].m_pPed=p;gCrossHair[0].m_bActive=false;auto& c=gCrossHair[slot];c.m_bActive=true;c.m_fSize=std::array{.1f,.3f,1.f,2.f}[size];c.m_nTimeWhenToDeactivate=std::array{-1,0,10000,10100}[expiry];c.field_1C=std::array{0.f,6.27f,-.2f}[rotation];StaticRef<std::array<bool,2>>(0x8D6144)[slot]=(rotation%2)==0;passed=Compare(pair++,"triangles");}
    for(int slot=0;slot<2&&passed;++slot)for(int time=0;time<5&&passed;++time)for(int radius=0;radius<3&&passed;++radius)for(int depth=0;depth<3&&passed;++depth){reset();CWorld::Players[1].m_pPed=p;gCrossHair[0].m_bActive=false;auto& c=gCrossHair[slot];c.m_bActive=true;c.m_bClearImmediately=true;CTimer::m_snTimeInMilliseconds=std::array{0u,1u,512u,1023u,1024u}[time];c.field_20=std::array{0.f,40.f,500.f}[radius];c.m_color.a=std::array{0,127,255}[radius];c.m_vecPosn=camera.GetPosition()+camera.GetForward()*std::array{-10.f,10.f,300.f}[depth];passed=Compare(pair++,"flight");}
    for(int coop=0;coop<2&&passed;++coop)for(int direction=0;direction<4&&passed;++direction){reset();if(coop)CWorld::Players[1].m_pPed=p;gCrossHair[0].m_vecPosn=camera.GetPosition()+camera.GetForward()*10.f+camera.GetRight()*(direction<2?(direction?100.f:-100.f):0.f)+camera.GetUp()*(direction>=2?(direction==2?100.f:-100.f):0.f);passed=Compare(pair++,"offscreen");}
    for(int coop=0;coop<2&&passed;++coop)for(int aim=0;aim<3&&passed;++aim){reset();gCrossHair[0].m_bActive=false;if(coop)CWorld::Players[1].m_pPed=p;auto& cam=TheCamera.GetActiveCam();cam.m_nMode=MODE_TWOPLAYER_IN_CAR_AND_SHOOTING;cam.m_fX_Targetting=float(aim-1)*.6f;cam.m_fY_Targetting=-.2f;passed=Compare(pair++,"coop_aim");}

    const auto oldWeapon=p->GetActiveWeapon().m_Type;const bool oldDuck=p->bIsDucking;const float oldAttack=p->GetPlayerData()->m_fAttackButtonCounter;
    for(auto weapon:{WEAPON_PISTOL,WEAPON_SHOTGUN,WEAPON_SNIPERRIFLE})for(int duck=0;duck<2&&passed;++duck)for(int attack=0;attack<2&&passed;++attack)for(int depth=0;depth<2&&passed;++depth){if(!passed)break;reset();p->GetActiveWeapon().m_Type=weapon;p->bIsDucking=duck;p->GetPlayerData()->m_fAttackButtonCounter=attack?8.f:0.f;gCrossHair[0].m_vecPosn=camera.GetPosition()+camera.GetForward()*(depth?30.f:1.8f);passed=Compare(pair++,"weapon_radius");}
    CVehicle* target{};CVehicle* host{};CStreaming::RequestModel(560,STREAMING_GAME_REQUIRED);CStreaming::LoadAllRequestedModels(false);if(CStreaming::IsModelLoaded(560)){target=CCarGenerator::CreateVehicle((eModelID)560,MISSION_VEHICLE);if(target)target->GetMatrix();host=CCarGenerator::CreateVehicle((eModelID)560,MISSION_VEHICLE);if(host){host->GetMatrix();host->SetPosn(camera.GetPosition()-camera.GetForward()*1000.f);}}
    if(passed&&(!target||!host)){Write({{"event","error"},{"reason","no real vehicle for target coverage"}});passed=false;}
    if(passed){
        const auto frenzyStatus=CDarkel::Status;const auto frenzyModels=CDarkel::ModelToKill;CDarkel::Status=eDarkelStatus::FRENZY_ON_GOING_2P;CDarkel::ModelToKill.fill(560);
        const auto targetPosition=target->GetPosition();auto* const oldVehicle=p->m_pVehicle;auto* const oldDriver=target->m_pDriver;
        for(int distance=0;distance<2&&passed;++distance)for(int elapsed=0;elapsed<6&&passed;++elapsed)for(int previous=0;previous<2&&passed;++previous)for(int role=0;role<3&&passed;++role){
            reset();gCrossHair[0].m_bActive=false;CWorld::Players[1].m_pPed=p;p->GetActiveWeapon().m_Type=WEAPON_SNIPERRIFLE;
            target->SetPosn(camera.GetPosition()+camera.GetForward()*(distance?150.f:30.f));p->m_pVehicle=role?host:nullptr;host->m_pDriver=role==1?p:nullptr;
            RwV3d projected;float w,h;CSprite::CalcScreenCoors(target->GetPosition(),&projected,&w,&h,true,true);
            auto& cam=TheCamera.GetActiveCam();cam.m_nMode=MODE_TWOPLAYER_IN_CAR_AND_SHOOTING;cam.m_fX_Targetting=projected.x/(float(RsGlobal.maximumWidth)*.5f)-1.f;cam.m_fY_Targetting=projected.y/(float(RsGlobal.maximumHeight)*.5f)-1.f;
            StaticRef<CEntity*>(0xC8A894)=previous?target:nullptr;StaticRef<uint32>(0xC8A890)=10000-std::array{-4,0,256,740,1024,2048}[elapsed];
            passed=Compare(pair++,"coop_lock");
            if(passed){int sprites{};for(auto& command:WeaponEffectsProbe::commands)if(command["kind"]=="sprite")++sprites;if(sprites!=2){Write({{"event","error"},{"reason","target reticle not drawn"},{"sprites",sprites}});passed=false;}}
        }
        target->SetPosn(targetPosition);target->m_pDriver=oldDriver;p->m_pVehicle=oldVehicle;host->m_pDriver=nullptr;CDarkel::Status=frenzyStatus;CDarkel::ModelToKill=frenzyModels;
    }
    p->GetActiveWeapon().m_Type=oldWeapon;p->bIsDucking=oldDuck;p->GetPlayerData()->m_fAttackButtonCounter=oldAttack;
    WeaponEffectsProbe::recording=false;RWSRCGLOBAL(dOpenDevice).fpRenderStateSet=originalSet;saved.Restore();if(target)delete target;if(host)delete host;Select(true);
    Write(passed?json{{"event","complete"},{"pairs",pair},{"state_restored",true}}:json{{"event","error"},{"reason","comparison failed"}});
}
}
