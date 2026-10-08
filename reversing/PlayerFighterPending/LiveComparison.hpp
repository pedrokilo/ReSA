#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskSimplePlayerOnFoot.h"
#include "TaskSimpleFight.h"
#include "TaskSimpleDuck.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"
#include "GameLogic.h"
#include "../ProbeRuntime.hpp"

namespace PlayerFighterPendingLive {
inline bool done{};
inline std::ofstream log;
inline json trace;
inline void Write(const json& j) { ReversingProbe::PumpMessages(); log << j.dump() << '\n'; log.flush(); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
template<typename T> T& At(void* p, size_t n) { return *reinterpret_cast<T*>(static_cast<uint8*>(p)+n); }
alignas(16) inline std::array<uint8,sizeof(CPlayerPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CEntity)> target{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> data{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,sizeof(CPad)> pad{};
alignas(16) inline std::array<uint8,sizeof(CTaskSimpleFight)> fight{};
alignas(16) inline std::array<uint8,sizeof(CTaskSimplePlayerOnFoot)> task{};
alignas(16) inline std::array<uint8,sizeof(CWeaponInfo)> weapon{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matrix{}, targetMatrix{};
inline bool fightPresent{}, coop{}, allowed{}, targetButton{}, sprint{}, duck{}, duckDown{}, canDuck{};
inline int16 stickX{}, stickY{};
inline float angleResult{}, limitedResult{};
inline CTaskSimpleFight* __fastcall GetFight(void*,void*) { trace.push_back({"get_fight",fightPresent}); return fightPresent ? reinterpret_cast<CTaskSimpleFight*>(fight.data()) : nullptr; }
inline CPad* __fastcall GetPad(void*,void*) { trace.push_back({"get_pad"}); return reinterpret_cast<CPad*>(pad.data()); }
inline CWeaponInfo* __cdecl GetWeapon(int32 type,uint32 skill) { trace.push_back({"weapon",type,skill&255}); return reinterpret_cast<CWeaponInfo*>(weapon.data()); }
inline int16 __fastcall StickX(void*,void*) { trace.push_back({"stick_x",stickX}); return stickX; }
inline int16 __fastcall StickY(void*,void*) { trace.push_back({"stick_y",stickY}); return stickY; }
inline bool __cdecl Coop(CPed*) { trace.push_back({"coop",coop}); return coop; }
inline bool __cdecl Allowed(CPed*,CVector dir,float limit) { trace.push_back({"allowed",F(dir.x),F(dir.y),F(dir.z),F(limit),allowed}); return allowed; }
inline bool __fastcall TargetButton(void*,void*) { trace.push_back({"target",targetButton}); return targetButton; }
inline bool __fastcall Sprint(void*,void*) { trace.push_back({"sprint",sprint}); return sprint; }
inline bool __fastcall Duck(void*,void*) { trace.push_back({"duck",duck}); return duck; }
inline bool __fastcall DuckDown(void*,void*) { trace.push_back({"duck_down",duckDown}); return duckDown; }
inline bool __cdecl CanDuck(CPed*) { trace.push_back({"can_duck",canDuck}); return canDuck; }
inline void __fastcall SetDuck(void*,void*,uint32 length) { trace.push_back({"set_duck",length&65535}); }
inline bool __fastcall Control(void*,void*,CEntity* entity,uint32 command) { trace.push_back({"control",entity==nullptr,command&255}); return true; }
inline float __cdecl Angle(float x1,float y1,float x2,float y2) { trace.push_back({"angle",F(x1),F(y1),F(x2),F(y2),F(angleResult)}); return angleResult; }
inline float __cdecl Limit(float angle) { trace.push_back({"limit",F(angle),F(limitedResult)}); return limitedResult; }
inline ReversibleHooks::ReversibleHook::Base* FindHook() {
    ReversibleHooks::ReversibleHook::Base* result{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if(c.Name()=="CTaskSimplePlayerOnFoot") for(auto& h:c.Items()) if(h->Name()=="PlayerControlFighter") result=h.get(); });
    return result;
}
struct Fn { const char* cls; const char* name; uintptr_t address,vt; int slot,cases; };
inline const Fn fn{"CTaskSimplePlayerOnFoot","PlayerControlFighter",0x687530,0,-1,4096};
inline void Update() {
    if(done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"player-fighter-pending-runtime.start")) {done=true;return;}
    if(!log.is_open()) {log.open(dir/"player-fighter-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("player-fighter-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning()) return;
    done=true;
    auto* hook=FindHook();
    if(!hook) {Write({{"event","error"},{"reason","hook not found"}});return;}
    const bool enabled=hook->Hooked(), locked=hook->Locked(); hook->LockState(false);
    const float timeSaved=CTimer::GetTimeStep(), orientationSaved=TheCamera.m_fOrientation;
    const bool mouseSaved=CCamera::m_bUseMouse3rdPerson;
    int failures=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch a1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskFighting),GetFight),e1(0x600F30,GetFight);
        EntryPatch a2(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),GetPad),e2(0x609560,GetPad);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),GetWeapon),e3(0x743C60,GetWeapon);
        EntryPatch a4(bit_cast<uintptr_t>(static_cast<int16(CPad::*)()const>(&CPad::GetPedWalkLeftRight)),StickX),e4(0x53FC90,StickX);
        EntryPatch a5(bit_cast<uintptr_t>(static_cast<int16(CPad::*)()const>(&CPad::GetPedWalkUpDown)),StickY),e5(0x53FD30,StickY);
        EntryPatch a6(bit_cast<uintptr_t>(&CGameLogic::IsPlayerUse2PlayerControls),Coop),e6(0x442020,Coop);
        EntryPatch a7(bit_cast<uintptr_t>(&CGameLogic::IsPlayerAllowedToGoInThisDirection),Allowed),e7(0x441E10,Allowed);
        EntryPatch a8(bit_cast<uintptr_t>(&CPad::GetTarget),TargetButton),e8(0x540670,TargetButton);
        EntryPatch a9(bit_cast<uintptr_t>(&CPad::GetSprint),Sprint),e9(0x5407A0,Sprint);
        EntryPatch b1(bit_cast<uintptr_t>(&CPad::GetDuck),Duck),f1(0x540700,Duck);
        EntryPatch b2(bit_cast<uintptr_t>(&CPad::DuckJustDown),DuckDown),f2(0x540720,DuckDown);
        EntryPatch b3(bit_cast<uintptr_t>(&CTaskSimpleDuck::CanPedDuck),CanDuck),f3(0x692610,CanDuck);
        EntryPatch b4(bit_cast<uintptr_t>(&CPedIntelligence::SetTaskDuckSecondary),SetDuck),f4(0x601230,SetDuck);
        EntryPatch b5(bit_cast<uintptr_t>(&CTaskSimpleFight::ControlFight),Control),f5(0x61C5E0,Control);
        EntryPatch b6(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),Angle),f6(0x53CBE0,Angle);
        EntryPatch b7(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),Limit),f7(0x53CB50,Limit);
        for(int i=0;i<fn.cases;++i) {
            std::array<json,2> result;
            for(int candidate=0;candidate<2;++candidate) {
                uint32 rng=0x5731A5u+uint32(i)*0x9E3779B9u;
                auto next=[&] {rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;};
                auto rnd=[&](float scale) {return (float(int32(next()%20001)-10000)/10000.0f)*scale;};
                ped.fill(0); target.fill(0); data.fill(0xCD); intel.fill(0); pad.fill(0); fight.fill(0xCD); task.fill(0xCD); weapon.fill(0); matrix.fill(0); targetMatrix.fill(0);
                At<void*>(ped.data(),0x14)=matrix.data(); At<void*>(ped.data(),0x47C)=intel.data(); At<void*>(ped.data(),0x480)=data.data();
                At<void*>(ped.data(),0x71C)=next()%3 ? nullptr : target.data();
                At<void*>(target.data(),0x14)=next()%2 ? targetMatrix.data() : nullptr;
                At<CVector>(target.data(),4)=CVector{rnd(10),rnd(10),rnd(10)};
                for(size_t j=0;j<0x40;j+=4) {At<float>(matrix.data(),j)=rnd(1.5f);At<float>(targetMatrix.data(),j)=rnd(8.0f);}
                // Distancia exactamente 4 y vecinos, para la salida del combate a distancia.
                if(i%17==0) {At<CVector>(matrix.data(),0x30)=CVector{0,0,0}; At<void*>(target.data(),0x14)=nullptr;At<CVector>(target.data(),4)=CVector{i%51==0 ? std::nextafter(4.0f,0.0f) : i%51==17 ? 4.0f : std::nextafter(4.0f,5.0f),0,0};}
                At<float>(ped.data(),0x55C)=rnd(4.0f);
                At<float>(data.data(),0xC)=rnd(1.2f);At<float>(data.data(),0x10)=rnd(1.2f);
                At<uint32>(task.data(),0)=0x8708EC;At<uint32>(task.data(),4)=0;At<uint32>(task.data(),0x10)=i%29==0 ? 1999u : i%29==1 ? 2000u : i%29==2 ? 2001u : next()%3 ? next()%2500 : 0xFFFFFFF0;
                At<uint8>(fight.data(),0x27)=i%23==0 ? 0xFF : uint8(next()%16);
                for(size_t w=0;w<13;++w) At<int32>(ped.data(),0x5A0+0x1C*w)=int32(next()%50);
                At<uint8>(ped.data(),0x718)=uint8(next()%13);
                At<int32>(weapon.data(),0)=next()%3 ? 0 : 1;
                static const int16 axis[]{0,1,-1,64,-64,128,-128,180,-180,32767,-32768};
                stickX=axis[next()%std::size(axis)];stickY=axis[next()%std::size(axis)];
                fightPresent=i%31!=0;coop=next()%2!=0;allowed=next()%2!=0;targetButton=next()%2!=0;sprint=next()%4==0;duck=next()%4==0;duckDown=next()%3==0;canDuck=next()%2!=0;
                angleResult=rnd(4);limitedResult=rnd(4);
                CCamera::m_bUseMouse3rdPerson=next()%2!=0;TheCamera.m_fOrientation=rnd(4);
                *reinterpret_cast<float*>(0xB7CB5C)=i%19==0 ? 0.0f : float(next()%160+1)/32.0f;
                trace=json::array();hook->State(candidate!=0);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==(candidate!=0);
                reinterpret_cast<void(__thiscall*)(void*,CPlayerPed*)>(fn.address)(task.data(),reinterpret_cast<CPlayerPed*>(ped.data()));
                auto words=[](void* p,size_t start,size_t end) {json v=json::array();for(size_t k=start;k<end;k+=4)v.push_back(At<uint32>(p,k));return v;};
                const bool pointersIntact=At<void*>(ped.data(),0x14)==matrix.data()&&At<void*>(ped.data(),0x47C)==intel.data()&&At<void*>(ped.data(),0x480)==data.data();
                // Normalizamos solo punteros de fixtures; comparamos todos los demás bytes.
                At<void*>(ped.data(),0x14)=nullptr;At<void*>(ped.data(),0x47C)=nullptr;At<void*>(ped.data(),0x480)=nullptr;At<void*>(ped.data(),0x71C)=nullptr;At<void*>(target.data(),0x14)=nullptr;
                result[candidate]={{"switches",switches},{"pointers_intact",pointersIntact},{"pad",words(pad.data(),0,pad.size())},{"weapon",words(weapon.data(),0,weapon.size())},{"target_matrix",words(targetMatrix.data(),0,targetMatrix.size())},{"trace",trace},{"ped",words(ped.data(),0,ped.size())},{"data",words(data.data(),0,data.size())},{"fight",words(fight.data(),0,fight.size())},{"task",words(task.data(),0,task.size())},{"intel",words(intel.data(),0,intel.size())},{"matrix",words(matrix.data(),0,matrix.size())},{"target",words(target.data(),0,target.size())}};
            }
            const bool equal=result[0]==result[1]&&result[0]["switches"]==true;
            failures+=!equal;
            Write({{"event","comparison"},{"function","CTaskSimplePlayerOnFoot::PlayerControlFighter"},{"pair",i},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
        }
    }
    hook->State(enabled);hook->LockState(locked);
    *reinterpret_cast<float*>(0xB7CB5C)=timeSaved;TheCamera.m_fOrientation=orientationSaved;CCamera::m_bUseMouse3rdPerson=mouseSaved;
    const bool restored=F(CTimer::GetTimeStep())==F(timeSaved)&&F(TheCamera.m_fOrientation)==F(orientationSaved)&&CCamera::m_bUseMouse3rdPerson==mouseSaved&&hook->Hooked()==enabled&&hook->Locked()==locked;
    Write({{"event","function_summary"},{"function","CTaskSimplePlayerOnFoot::PlayerControlFighter"},{"cases",fn.cases},{"failures",failures}});
    Write({{"event","complete"},{"state_restored",restored},{"passed",failures==0&&restored}});
}
}
