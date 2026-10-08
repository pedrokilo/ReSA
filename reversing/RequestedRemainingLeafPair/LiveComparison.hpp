#pragma once
#include <bit>
#include <fstream>
#include <limits>
#include <cmath>
#include "InterestingEvents.h"
#include "Cheat.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Entity.h"
#include <map>
#include "PedDamageResponseCalculator.h"
#include "PedDamageResponse.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
#include <reversiblehooks/ReversibleHook/Virtual.h>

// Prepared bank only. Invoke from an isolated pregraphics harness on the game thread.
namespace RequestedRemainingLeafPair {
using Simple = ReversibleHooks::ReversibleHook::Simple;
static_assert(sizeof(CInterestingEvents)==0x158);
static_assert(sizeof(CPed)==0x79C);
inline json trace;
inline CInterestingEvents* expectedEvents{};
inline CEntity* expectedEntity{};
inline CPedDamageResponseCalculator* expectedCalculator{};
inline CPed* expectedPed{};
inline CPedDamageResponse* expectedResponse{};
inline bool forceResult{}, collaboratorsValid{};
inline void __fastcall Cleanup(CEntity* entity, void*, CEntity** slot) {
    int index=-1;
    for(int i=0;i<8;++i)if(slot==&expectedEvents->m_Events[i].entity)index=i;
    const bool valid=index>=0 && entity==expectedEntity && *slot==entity;
    collaboratorsValid&=valid;
    trace.push_back({{"event","cleanup_reference"},{"index",index},{"identity_valid",valid}});
}
inline bool __fastcall Force(CPedDamageResponseCalculator* calculator,void*,CPed* ped,CPedDamageResponse& response) {
    const bool valid=calculator==expectedCalculator&&ped==expectedPed && &response==expectedResponse;
    collaboratorsValid&=valid;
    trace.push_back({{"event","force_death"},{"identity_valid",valid},{"result",forceResult}});
    return forceResult;
}
inline int16 __fastcall Say(CPed* ped,void*,eGlobalSpeechContext context,uint32 delay,float probability,bool silence,bool audible,bool front) {
    const bool valid=ped==expectedPed;collaboratorsValid&=valid;
    trace.push_back({{"event","say"},{"identity_valid",valid},{"context",int(context)},{"delay",delay},{"probability",probability},{"silence",silence},{"audible",audible},{"front",front}});
    return 17;
}
inline json Speech(int context) {
    return {{"event","say"},{"identity_valid",true},{"context",context},{"delay",0},{"probability",1.f},{"silence",false},{"audible",false},{"front",false}};
}
inline void ModelDamage(CPedDamageResponseCalculator& calculator,CPed& ped,CPedDamageResponse& response,bool player,bool invincible,bool mega,bool force,bool speak,json& calls) {
    // Independent byte/decision model from the EXE dossier, with numeric contexts.
    const int weapon=int(calculator.m_weaponType);
    if(player&&invincible&&weapon<47)return;
    const float priorHealth=ped.m_fHealth;
    calls.push_back({{"event","force_death"},{"identity_valid",true},{"result",force}});
    response.m_bForceDeath=force;
    if(mega&&(weapon==0||weapon==1||weapon==46))calculator.m_fDamageFactor=ped.m_fHealth;
    if(force){response.m_bHealthZero=true;response.m_fDamageHealth=ped.m_fHealth;ped.m_fHealth=0.f;return;}
    float remaining=ped.m_fHealth-calculator.m_fDamageFactor;
    if(weapon==54&&calculator.m_bSpeak){
        if(remaining<5.f)remaining=5.f;
        response.m_bHealthZero=false;ped.m_fHealth=remaining;response.m_fDamageHealth=priorHealth-remaining;
        if(speak)calls.push_back(Speech(0x159));return;
    }
    if(remaining<1.f){response.m_bHealthZero=true;response.m_fDamageHealth=ped.m_fHealth;ped.m_fHealth=0.f;return;}
    response.m_bHealthZero=false;response.m_fDamageHealth=calculator.m_fDamageFactor;ped.m_fHealth=remaining;
    if(speak)calls.push_back(Speech(weapon==53?0x165:calculator.m_fDamageFactor>=5.f||remaining<=10.f?0x158:0x159));
}
inline void Update() {
    static bool done{};if(done)return;
    char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto dir=std::filesystem::path(executable).parent_path();
    if(!std::filesystem::exists(dir/"remaining-leaf-pair-runtime.start"))return;done=true;
    std::ofstream log(dir/"remaining-leaf-pair-runtime.jsonl");
    const auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    std::map<std::string,Simple*> targets;
    const auto flags=SmallPendingSupport::SnapshotHooks();
    std::vector<std::pair<void*,std::vector<uint8>>> entries;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& holder:cat.Items()){
        auto* base=holder.get();void* gta{};void* candidate{};
        if(base->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* h=static_cast<Simple*>(base);gta=h->GetHookGTAAddress();candidate=h->GetHookOurAddress();targets[std::string(cat.Name())+"::"+h->Name()]=h;}
        else if(base->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Virtual){auto* h=static_cast<ReversibleHooks::ReversibleHook::Virtual*>(base);gta=h->GetHookGTAAddress();candidate=h->GetHookOurAddress();}
        if(gta)entries.emplace_back(gta,ReversingProbe::Bytes(gta,52));if(candidate)entries.emplace_back(candidate,ReversingProbe::Bytes(candidate,52));
    }});
    const char* eventKey="CInterestingEvents::InvalidateEvent";const char* damageKey="CPedDamageResponseCalculator::ComputeWillKillPed";
    if(!targets.contains(eventKey)||!targets.contains(damageKey)){write({{"event","error"},{"reason","target missing"}});return;}
    const auto cheatsBefore=ReversingProbe::Bytes(CCheat::m_aCheatsActive,sizeof(CCheat::m_aCheatsActive));
    const auto cleanupDll=std::bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const auto forceDll=std::bit_cast<uintptr_t>(&CPedDamageResponseCalculator::ComputeWillForceDeath);
    const auto sayDll=std::bit_cast<uintptr_t>(&CPed::Say);
    std::vector<std::pair<uintptr_t,void*>> patches{{0x571A00,reinterpret_cast<void*>(&Cleanup)},{cleanupDll,reinterpret_cast<void*>(&Cleanup)},{0x4AD610,reinterpret_cast<void*>(&Force)},{forceDll,reinterpret_cast<void*>(&Force)},{0x5EFFE0,reinterpret_cast<void*>(&Say)},{sayDll,reinterpret_cast<void*>(&Say)}};
    std::sort(patches.begin(),patches.end(),[](auto a,auto b){return a.first<b.first;});patches.erase(std::unique(patches.begin(),patches.end(),[](auto a,auto b){return a.first==b.first;}),patches.end());
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> patchBytes;
    for(const auto& [address,replacement]:patches)patchBytes.emplace_back(address,ReversingProbe::Bytes(reinterpret_cast<void*>(address),52));
    for(const auto& state:flags)if(state.hook->Name()!="NOTSA_WinMain"){state.hook->LockState(false);state.hook->State(false);}
    bool passed=true;size_t pairs=0,failures=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> ownedPatches;
        for(const auto& [address,replacement]:patches)ownedPatches.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,replacement));
        alignas(CInterestingEvents) std::array<uint8,sizeof(CInterestingEvents)> eventBytes;
        alignas(CEntity) std::array<uint8,sizeof(CEntity)> entityBytes{};
        TInterestingEvent external{};expectedEntity=reinterpret_cast<CEntity*>(entityBytes.data());
        // 10 input identities: each member, nullptr and a distinct live external object.
        for(int selection=0;selection<10;++selection)for(int active=-1;active<8;++active)for(bool nonnull:{false,true})for(int poison=0;poison<4;++poison){
            eventBytes.fill(uint8(0x39+poison*47));auto* g=reinterpret_cast<CInterestingEvents*>(eventBytes.data());expectedEvents=g;
            g->m_nInterestingEvent=int8(active);for(int i=0;i<8;++i){g->m_Events[i].type=i;g->m_Events[i].time=uint32(0x1000+poison*17+i);g->m_Events[i].entity=nonnull?expectedEntity:nullptr;}
            const auto initial=eventBytes;alignas(CInterestingEvents) auto model=initial;auto* expected=reinterpret_cast<CInterestingEvents*>(model.data());json expectedTrace=json::array();
            if(selection<8){expected->m_Events[selection].time=0;expected->m_Events[selection].entity=nullptr;if(active==selection)expected->m_nInterestingEvent=-1;if(nonnull)expectedTrace.push_back({{"event","cleanup_reference"},{"index",selection},{"identity_valid",true}});}
            json results[2];
            for(int route=0;route<2;++route){eventBytes=initial;trace=json::array();collaboratorsValid=true;auto* target=selection<8?&g->m_Events[selection]:selection==8?nullptr:&external;auto* h=targets.at(eventKey);h->State(route!=0);const bool gate=SmallPendingSupport::ExactSwitch(*h,route!=0);reinterpret_cast<void(__thiscall*)(CInterestingEvents*,const TInterestingEvent*)>(0x602960)(g,target);results[route]={{"bytes",ReversingProbe::Bytes(eventBytes.data(),eventBytes.size())},{"trace",trace},{"gate",gate},{"collaborators_valid",collaboratorsValid}};h->State(false);}
            const bool equal=results[0]==results[1]&&results[0]["gate"]==true&&results[0]["collaborators_valid"]==true&&results[0]["bytes"]==ReversingProbe::Bytes(model.data(),model.size())&&results[0]["trace"]==expectedTrace;
            passed&=equal;failures+=!equal;write({{"event","comparison"},{"function",eventKey},{"pair",pairs++},{"selection",selection},{"active",active},{"nonnull",nonnull},{"poison",poison},{"equal",equal},{"original",results[0]},{"candidate",results[1]},{"model_bytes",ReversingProbe::Bytes(model.data(),model.size())},{"model_trace",expectedTrace}});
        }
        constexpr std::array<int,8> weapons{0,1,46,47,53,54,22,55};
        const std::array<std::pair<float,float>,12> healthDamage{{{1.f,0.f},{10.f,9.f},{10.f,std::nextafter(9.f,10.f)},{10.f,std::nextafter(9.f,0.f)},{20.f,std::nextafter(5.f,0.f)},{20.f,5.f},{5.f,100.f},{0.f,0.f},{14.f,4.f},{std::nextafter(14.f,20.f),4.f},{4.f,0.f},{5.f,0.f}}};
        alignas(CPed) std::array<uint8,sizeof(CPed)> pedBytes{};
        alignas(CPedDamageResponseCalculator) std::array<uint8,sizeof(CPedDamageResponseCalculator)> calculatorBytes;
        alignas(CPedDamageResponse) std::array<uint8,sizeof(CPedDamageResponse)> responseBytes;
        auto* ped=reinterpret_cast<CPed*>(pedBytes.data());auto* calculator=reinterpret_cast<CPedDamageResponseCalculator*>(calculatorBytes.data());auto* response=reinterpret_cast<CPedDamageResponse*>(responseBytes.data());expectedPed=ped;expectedCalculator=calculator;expectedResponse=response;
        for(int pedType=0;pedType<3;++pedType)for(int control=0;control<32;++control)for(int weapon:weapons)for(size_t boundary=0;boundary<healthDamage.size();++boundary){
            const bool player=pedType!=0,invincible=control&1,mega=control&2,force=control&4,memberSpeak=control&8,speak=control&16;
            pedBytes.fill(0);calculatorBytes.fill(0xCD);responseBytes.fill(0xA5);
            ped->m_nPedType=pedType==0?PED_TYPE_CIVMALE:pedType==1?PED_TYPE_PLAYER1:PED_TYPE_PLAYER2;ped->m_fHealth=healthDamage[boundary].first;
            calculator->m_pDamager=nullptr;calculator->m_fDamageFactor=healthDamage[boundary].second;calculator->m_bodyPart=PED_PIECE_TORSO;calculator->m_weaponType=eWeaponType(weapon);calculator->m_bSpeak=memberSpeak;
            response->m_fDamageHealth=17.f;response->m_fDamageArmor=19.f;response->m_bHealthZero=true;response->m_bForceDeath=!force;response->m_bDamageCalculated=bool(control&1);response->m_bCheckIfAffectsPed=bool(control&2);
            const auto pedInitial=pedBytes;const auto calcInitial=calculatorBytes;const auto rspInitial=responseBytes;alignas(CPed) auto modelPed=pedInitial;alignas(CPedDamageResponseCalculator) auto modelCalc=calcInitial;alignas(CPedDamageResponse) auto modelRsp=rspInitial;json expectedTrace=json::array();
            ModelDamage(*reinterpret_cast<CPedDamageResponseCalculator*>(modelCalc.data()),*reinterpret_cast<CPed*>(modelPed.data()),*reinterpret_cast<CPedDamageResponse*>(modelRsp.data()),player,invincible,mega,force,speak,expectedTrace);
            json results[2];
            for(int route=0;route<2;++route){pedBytes=pedInitial;calculatorBytes=calcInitial;responseBytes=rspInitial;CCheat::m_aCheatsActive[CHEAT_NO_ONE_CAN_STOP_US]=invincible;CCheat::m_aCheatsActive[CHEAT_MEGA_PUNCH]=mega;forceResult=force;trace=json::array();collaboratorsValid=true;auto* h=targets.at(damageKey);h->State(route!=0);const bool gate=SmallPendingSupport::ExactSwitch(*h,route!=0);reinterpret_cast<void(__thiscall*)(CPedDamageResponseCalculator*,CPed*,CPedDamageResponse&,bool)>(0x4B3210)(calculator,ped,*response,speak);results[route]={{"ped_bytes",ReversingProbe::Bytes(pedBytes.data(),pedBytes.size())},{"calculator_bytes",ReversingProbe::Bytes(calculatorBytes.data(),calculatorBytes.size())},{"response_bytes",ReversingProbe::Bytes(responseBytes.data(),responseBytes.size())},{"trace",trace},{"gate",gate},{"collaborators_valid",collaboratorsValid}};h->State(false);}
            const bool modelEqual=results[0]["ped_bytes"]==ReversingProbe::Bytes(modelPed.data(),modelPed.size())&&results[0]["calculator_bytes"]==ReversingProbe::Bytes(modelCalc.data(),modelCalc.size())&&results[0]["response_bytes"]==ReversingProbe::Bytes(modelRsp.data(),modelRsp.size())&&results[0]["trace"]==expectedTrace;
            const bool equal=results[0]==results[1]&&results[0]["gate"]==true&&results[0]["collaborators_valid"]==true&&modelEqual;passed&=equal;failures+=!equal;
            write({{"event","comparison"},{"function",damageKey},{"pair",pairs++},{"ped_type",pedType},{"controls",control},{"weapon",weapon},{"boundary",boundary},{"equal",equal},{"model_equal",modelEqual},{"original",results[0]},{"candidate",results[1]},{"model_trace",expectedTrace},{"model_ped_bytes",ReversingProbe::Bytes(modelPed.data(),modelPed.size())},{"model_calculator_bytes",ReversingProbe::Bytes(modelCalc.data(),modelCalc.size())},{"model_response_bytes",ReversingProbe::Bytes(modelRsp.data(),modelRsp.size())}});
            ReversingProbe::PumpMessages();
        }
    }
    std::memcpy(CCheat::m_aCheatsActive,cheatsBefore.data(),cheatsBefore.size());
    for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
    const bool entriesRestored=std::ranges::all_of(entries,[](const auto& r){return r.second==ReversingProbe::Bytes(r.first,r.second.size());});
    const bool collaboratorsRestored=std::ranges::all_of(patchBytes,[](const auto& r){return r.second==ReversingProbe::Bytes(reinterpret_cast<void*>(r.first),r.second.size());});
    const bool cheatsRestored=cheatsBefore==ReversingProbe::Bytes(CCheat::m_aCheatsActive,cheatsBefore.size()),flagsRestored=SmallPendingSupport::HooksUnchanged(flags);
    const bool restored=entriesRestored&&collaboratorsRestored&&cheatsRestored&&flagsRestored;
    write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"differences",failures},{"state_restored",restored},{"entries_restored",entriesRestored},{"collaborator_entries_restored",collaboratorsRestored},{"cheats_restored",cheatsRestored},{"flags_restored",flagsRestored},{"scope","finite private fixtures; references, force-death and speech isolated; no real-actor integration"}});
    expectedEvents=nullptr;expectedEntity=nullptr;expectedCalculator=nullptr;expectedPed=nullptr;expectedResponse=nullptr;trace=nullptr;
}
}
