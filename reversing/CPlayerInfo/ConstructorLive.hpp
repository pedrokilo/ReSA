#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cheat.h"
#include "ConstructorProbe.hpp"
#include "../ProbeSupport.hpp"
namespace PlayerInfoConstructorLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& row) {log<<row.dump()<<'\n';log.flush();}
inline int Report(int type,char* msg,int*) {if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",msg?msg:""}});ExitProcess(91);}
template<class F> uintptr_t Address(F f){return std::bit_cast<uintptr_t>(f);}
inline void __fastcall Visual(CPed*,void*,int32 model){trace.push_back({"weapon_model",model});}
inline void __cdecl Request(int32 model,int32 flags){trace.push_back({"request",model,flags});}
inline void __cdecl Load(bool priority){trace.push_back({"load",priority});}
inline void __cdecl Deletable(int32 model,bool mission){trace.push_back({"deletable",model,mission});}
inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"playerinfo-constructor-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playerinfo-constructor-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
    auto report=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook=nullptr;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPlayerInfo")for(auto& h:c.Items())if(h->Name()=="Constructor")hook=h.get();});
    if(!hook){Write({{"event","error"}});_CrtSetReportHook(report);return;}
    const auto enabled=hook->Hooked(),locked=hook->Locked();
    Write({{"event","hook_state"},{"enabled",enabled},{"locked",locked},{"startup_candidate_calls",PlayerInfoConstructorProbe::calls}});
    hook->LockState(false);
    const auto savedPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    const auto savedGlobals=ReversingProbe::Bytes(CWorld::Players,sizeof(CPlayerInfo)*2);
    alignas(CPlayerInfo) std::array<uint8,sizeof(CPlayerInfo)+64> storage;
    auto* info=reinterpret_cast<CPlayerInfo*>(storage.data()+32);
    bool passed=true;int pair=0;
    for(int i=0;i<4096&&passed;++i){
        std::array<uint8,sizeof(CPlayerInfo)+64> input;uint32 rng=uint32(i)*1664525u+1013904223u;
        for(auto& b:input){rng=rng*1664525u+1013904223u;b=i<3?uint8(i==0?0:i==1?255:204):uint8(rng>>24);}
        const auto run=[&]{storage=input;auto* ret=reinterpret_cast<CPlayerInfo*(__thiscall*)(CPlayerInfo*)>(0x571920)(info);
            return json{{"bytes",storage},{"returned_this",ret==info},{"guards",std::equal(storage.begin(),storage.begin()+32,input.begin())&&std::equal(storage.end()-32,storage.end(),input.end()-32)}};};
        hook->State(false);auto a=PlayerInfoConstructorProbe::calls;auto ia=PlayerInfoConstructorProbe::initializations;auto original=run();auto b=PlayerInfoConstructorProbe::calls;auto ib=PlayerInfoConstructorProbe::initializations;
        hook->State(true);auto candidate=run();auto c=PlayerInfoConstructorProbe::calls;auto ic=PlayerInfoConstructorProbe::initializations;
        passed=original==candidate&&original["returned_this"]==true&&original["guards"]==true&&a==b&&c==b+1&&ib==ia+1&&ic==ib+1;
        Write({{"event","comparison"},{"pair",pair++},{"kind","constructor"},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b},{"original_initializations",ib-ia},{"candidate_initializations",ic-ib}});
    }
    // Real inventory, ammo and model-reference operations; only asset streaming and
    // visual attachment are intercepted. Never use the live player's inventory.
    {
        ReversingProbe::EntryPatch add(Address(&CPed::AddWeaponModel),Visual),remove(Address(&CPed::RemoveWeaponModel),Visual);
        ReversingProbe::EntryPatch addExe(0x5E5ED0,Visual),removeExe(0x5E3990,Visual);
        ReversingProbe::EntryPatch request(Address(&CStreaming::RequestModel),Request),load(Address(&CStreaming::LoadAllRequestedModels),Load),deletable(Address(&CStreaming::SetModelIsDeletable),Deletable);
        alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> pedBytes{};auto* ped=reinterpret_cast<CPlayerPed*>(pedBytes.data());
        for(int i=0;i<128&&passed;++i){
            const auto run=[&]{
                storage.fill(0);reinterpret_cast<CPlayerInfo*(__thiscall*)(CPlayerInfo*)>(0x571920)(info);
                pedBytes.fill(0);ped->m_nPedType=PED_TYPE_PLAYER1;*reinterpret_cast<CPlayerPedData**>(pedBytes.data()+0x480)=&info->m_PlayerData;ped->m_nActiveWeaponSlot=0;
                CWorld::Players[0].m_pPed=ped;CWorld::Players[1].m_pPed=nullptr;trace=json::array();
                std::vector<std::pair<CBaseModelInfo*,uint16>> refs;
                for(int w=0;w<=46;++w)for(auto model:CWeaponInfo::GetWeaponInfo(eWeaponType(w))->GetModels())if(model!=MODEL_INVALID){auto* m=CModelInfo::GetModelInfo(model);if(m)refs.emplace_back(m,m->m_nRefCount);}
                for(auto& weapon:ped->m_aWeapons)weapon.Initialise(WEAPON_UNARMED,0,ped);
                for(int repeat=0;repeat<1+i/4%4;++repeat){switch(i%4){case 0:CCheat::WeaponCheat1();break;case 1:CCheat::WeaponCheat2();break;case 2:CCheat::WeaponCheat3();break;default:CCheat::WeaponCheat4();break;}}
                json inventory=json::array();bool usable=true;
                for(int slot=1;slot<NUM_WEAPON_SLOTS;++slot){auto& w=ped->m_aWeapons[slot];if(w.m_Type==WEAPON_UNARMED)continue;
                    ped->SetCurrentWeapon(slot);usable=usable&&ped->m_nActiveWeaponSlot==slot&&info->m_PlayerData.m_nChosenWeapon==slot;
                    inventory.push_back({int(w.m_Type),w.m_TotalAmmo,w.m_AmmoInClip,int(w.m_State)});
                }
                auto result=json{{"inventory",inventory},{"usable",usable&&!inventory.empty()&&!info->m_PlayerData.m_bDontAllowWeaponChange&&!info->m_PlayerData.m_bInVehicleDontAllowWeaponChange},{"data",ReversingProbe::Bytes(&info->m_PlayerData,sizeof(CPlayerPedData))},{"trace",trace}};
                for(auto& weapon:ped->m_aWeapons)weapon.Shutdown();
                result["references_restored"]=std::ranges::all_of(refs,[](auto item){return item.first->m_nRefCount==item.second;});
                return result;
            };
            hook->State(false);auto a=PlayerInfoConstructorProbe::calls;auto original=run();auto b=PlayerInfoConstructorProbe::calls;
            hook->State(true);auto candidate=run();auto c=PlayerInfoConstructorProbe::calls;
            passed=original==candidate&&original["usable"]==true&&original["references_restored"]==true&&a==b&&c==b+1;
            Write({{"event","comparison"},{"pair",pair++},{"kind","weapons"},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        }
    }
    std::memcpy(CWorld::Players,savedGlobals.data(),savedGlobals.size());hook->State(enabled);hook->LockState(locked);
    const bool restored=savedPlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&savedGlobals==ReversingProbe::Bytes(CWorld::Players,savedGlobals.size());
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});_CrtSetReportHook(report);
}
}
