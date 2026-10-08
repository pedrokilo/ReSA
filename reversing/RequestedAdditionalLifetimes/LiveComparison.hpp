#pragma once
#include <map>
#include "../RequestedParallelTasks/TaskLifetimeExpanded/Expanded.hpp"
#include "TaskSimpleFightingControl.h"
#include "TaskComplexProstituteSolicit.h"
#include "PlayerPed.h"
#include "PlayerPedData.h"
namespace RequestedAdditionalLifetimeLive {
using namespace RequestedTaskLifetime;
inline std::vector<json>* calls{};
inline CEntity* referenceEntity{};
inline CEntity** referenceSlot{};
inline CPlayerPed* fixturePlayer{};
inline bool valid{};
inline void RefCall(const char* kind,CEntity* entity,CEntity** slot){
    const bool sameEntity=entity==referenceEntity,sameSlot=slot==referenceSlot,samePointee=slot&&*slot==entity;
    valid&=sameEntity&&sameSlot&&samePointee;
    calls->push_back({{"event",kind},{"entity_matches",sameEntity},{"slot_matches",sameSlot},{"pointee_matches",samePointee}});
}
inline void __fastcall Register(CEntity* entity,void*,CEntity** slot){RefCall("register_reference",entity,slot);}
inline void __fastcall Cleanup(CEntity* entity,void*,CEntity** slot){RefCall("cleanup_reference",entity,slot);}
inline CPlayerPed* __cdecl Player(int32 index){valid&=index==-1;calls->push_back({{"event","find_player"},{"index",index},{"nonnull",fixturePlayer!=nullptr}});return fixturePlayer;}
inline void Update(){
    static bool done{};if(done)return;done=true;
    char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);const auto dir=std::filesystem::path(executable).parent_path();
    if(!std::filesystem::exists(dir/"additional-lifetime-runtime.start"))return;
    std::ofstream log(dir/"additional-lifetime-runtime.jsonl");const auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    auto* fightingTable=reinterpret_cast<void*>(GetProcAddress(module,"??_7CTaskSimpleFightingControl@@6B@"));
    auto* solicitTable=reinterpret_cast<void*>(GetProcAddress(module,"??_7CTaskComplexProstituteSolicit@@6B@"));
    auto* baseTable=reinterpret_cast<void*>(GetProcAddress(module,"??_7CTask@@6B@"));
    if(!fightingTable||!solicitTable||!baseTable){write({{"event","error"},{"reason","missing exported target/base vtable"}});return;}
    std::vector<std::pair<void*,std::vector<uint8>>> tables;
    for(auto [address,count]:std::array<std::pair<void*,size_t>,4>{{{fightingTable,9},{solicitTable,11},{reinterpret_cast<void*>(0x86D700),9},{reinterpret_cast<void*>(0x86FB88),11}}})tables.emplace_back(address,ReversingProbe::Bytes(address,count*4));
    std::vector<Snapshot> snapshots;std::map<std::string,Simple*> targets;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& holder:cat.Items()){
        auto* h=holder.get();Snapshot s{h,h->Hooked(),h->Locked()};
        if(h->Type()==Base::HookType::Simple){auto* p=static_cast<Simple*>(h);s.gta=p->GetHookGTAAddress();s.ours=p->GetHookOurAddress();targets[std::string(cat.Name())+"::"+h->Name()]=p;}
        else if(h->Type()==Base::HookType::Virtual){auto* p=static_cast<Virtual*>(h);s.gta=p->GetHookGTAAddress();s.ours=p->GetHookOurAddress();}
        if(s.gta){s.gtaBytes=ReversingProbe::Bytes(s.gta,52);s.ourBytes=ReversingProbe::Bytes(s.ours,52);}snapshots.push_back(std::move(s));
    }});
    const std::array<const char*,4> keys{"CTaskSimpleFightingControl::Constructor","CTaskSimpleFightingControl::Destructor","CTaskComplexProstituteSolicit::Constructor","CTaskComplexProstituteSolicit::Destructor"};
    for(auto key:keys)if(!targets.contains(key)){write({{"event","error"},{"missing",key}});return;}
    for(auto& s:snapshots){s.hook->LockState(false);s.hook->State(false);}
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> patched;
    for(uintptr_t at:{0x571B70u,0x571A00u,0x56E210u})patched.emplace_back(at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),52));
    bool passed=true,patchRestored{};int pair=0;
    {
        ReversingProbe::EntryPatch reg{0x571B70,&Register},clean{0x571A00,&Cleanup},player{0x56E210,&Player};
        std::array<uint8,32> entity{};const auto identity=reinterpret_cast<CEntity*>(entity.data());
        alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> playerBytes{};
        alignas(CPlayerPedData) std::array<uint8,sizeof(CPlayerPedData)> dataBytes{};
        auto* playerPointer=reinterpret_cast<CPlayerPed*>(playerBytes.data());
        auto* dataPointer=reinterpret_cast<CPlayerPedData*>(dataBytes.data());
        memcpy(playerBytes.data()+0x480,&dataPointer,4);
        const auto gate=[&](Simple* h,bool candidate){h->State(candidate);return h->Hooked()==candidate&&memcmp(h->GetHookGTAAddress(),candidate?static_cast<void*>(&h->m_HookContent):static_cast<void*>(h->m_OriginalFunctionContent),h->m_iHookedBytes)==0;};
        const auto ctor=[&]<class T>(const char* key,uintptr_t gtaVptr,void* nativeVptr,int seed,bool nonnull,auto invoke){
            alignas(T) std::array<uint8,sizeof(T)> left,right;left.fill(uint8(0x5A+37*seed));right=left;
            std::array<std::vector<json>,2> traces;std::array<bool,2> selection{},contract{},returned{};auto* h=targets.at(key);
            for(int route=0;route<2;++route){auto* obj=reinterpret_cast<T*>((route?right:left).data());calls=&traces[route];valid=true;referenceEntity=identity;referenceSlot=reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(obj)+0xC);selection[route]=gate(h,route==1);returned[route]=invoke(obj,h->GetHookGTAAddress(),nonnull?identity:nullptr)==obj;contract[route]=valid;h->State(false);}
            std::vector<json> expected;if(nonnull)expected.push_back({{"event","register_reference"},{"entity_matches",true},{"slot_matches",true},{"pointee_matches",true}});
            std::vector<size_t> changed;for(size_t offset=4;offset<sizeof(T);++offset)if(left[offset]!=right[offset])changed.push_back(offset);
            const bool payload=changed.empty(),vptr=*reinterpret_cast<uintptr_t*>(left.data())==gtaVptr&&*reinterpret_cast<void**>(right.data())==nativeVptr;
            const bool equal=selection[0]&&selection[1]&&contract[0]&&contract[1]&&returned[0]&&returned[1]&&payload&&vptr&&traces[0]==expected&&traces[1]==expected;passed&=equal;
            write({{"event","comparison"},{"pair",pair++},{"function",key},{"seed",seed},{"reference_nonnull",nonnull},{"equal",equal},{"payload_equal",payload},{"differing_payload_offsets",changed},{"original_bytes",left},{"candidate_bytes",right},{"vptr_valid",vptr},{"trace_original",traces[0]},{"trace_candidate",traces[1]},{"trace_expected",expected},{"original_gate",selection[0]},{"candidate_gate",selection[1]}});
        };
        for(int seed=0;seed<128;++seed){
            for(bool nonnull:{false,true})ctor.operator()<CTaskSimpleFightingControl>(keys[0],0x86D700,fightingTable,seed,nonnull,[&](auto* t,void* at,CEntity* target){using F=CTaskSimpleFightingControl*(__thiscall*)(CTaskSimpleFightingControl*,CEntity*,float,float);return reinterpret_cast<F>(at)(t,target,float(seed-64)*.125f,float(seed)*.25f);});
            ctor.operator()<CTaskComplexProstituteSolicit>(keys[2],0x86FB88,solicitTable,seed,true,[&](auto* t,void* at,CEntity* target){using F=CTaskComplexProstituteSolicit*(__thiscall*)(CTaskComplexProstituteSolicit*,CPed*);return reinterpret_cast<F>(at)(t,reinterpret_cast<CPed*>(target));});
        }
        const auto dtor=[&]<class T>(const char* key,int seed,bool nonnull,bool playerPresent,bool camera,bool child){
            alignas(T) std::array<uint8,sizeof(T)> left,right;left.fill(uint8(0x5A+37*seed));right=left;
            std::array<std::vector<json>,2> traces;std::array<bool,2> selection{},contract{};std::array<std::vector<uint8>,2> dataAfter;auto* h=targets.at(key);
            constexpr bool solicit=std::is_same_v<T,CTaskComplexProstituteSolicit>;
            for(int route=0;route<2;++route){auto* obj=reinterpret_cast<T*>((route?right:left).data());memset(reinterpret_cast<uint8*>(obj)+4,0,4);
                if constexpr(solicit){*reinterpret_cast<CTask**>(reinterpret_cast<uint8*>(obj)+8)=child?new RequestedTaskLifetimeExpanded::Child{obj}:nullptr;uint16 flags=uint16((seed*0x110)&~0x100)|uint16(camera?0x100:0);memcpy(reinterpret_cast<uint8*>(obj)+0x2C,&flags,2);}
                else{reinterpret_cast<uint8*>(obj)[8]=0;reinterpret_cast<uint8*>(obj)[9]=0;*reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(obj)+0xC)=nonnull?identity:nullptr;}
                dataBytes.fill(0);*reinterpret_cast<CEntity**>(dataBytes.data()+0xA4)=nonnull?identity:nullptr;fixturePlayer=playerPresent?playerPointer:nullptr;
                calls=&traces[route];valid=true;referenceEntity=identity;referenceSlot=solicit?reinterpret_cast<CEntity**>(dataBytes.data()+0xA4):reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(obj)+0xC);
                RequestedTaskLifetimeExpanded::trace=&traces[route];RequestedTaskLifetimeExpanded::expectedParent=obj;RequestedTaskLifetimeExpanded::collaboratorsValid=true;
                selection[route]=gate(h,route==1);using F=void(__thiscall*)(T*);reinterpret_cast<F>(h->GetHookGTAAddress())(obj);contract[route]=valid&&RequestedTaskLifetimeExpanded::collaboratorsValid;dataAfter[route]=ReversingProbe::Bytes(dataBytes.data(),dataBytes.size());h->State(false);
            }
            RequestedTaskLifetimeExpanded::trace=nullptr;
            std::vector<json> expected;
            if constexpr(solicit)expected.push_back({{"event","find_player"},{"index",-1},{"nonnull",playerPresent}});
            if(nonnull&&(!solicit||playerPresent))expected.push_back({{"event","cleanup_reference"},{"entity_matches",true},{"slot_matches",true},{"pointee_matches",true}});
            if(child){expected.push_back({{"event","child_destructor"},{"parent_matches",true}});expected.push_back({{"event","child_delete"},{"nonnull",true}});}
            const bool payload=std::equal(left.begin()+4,left.end(),right.begin()+4),vptr=*reinterpret_cast<uintptr_t*>(left.data())==0x86D48C&&*reinterpret_cast<void**>(right.data())==baseTable;
            const bool equal=selection[0]&&selection[1]&&contract[0]&&contract[1]&&payload&&vptr&&dataAfter[0]==dataAfter[1]&&traces[0]==expected&&traces[1]==expected;passed&=equal;
            write({{"event","comparison"},{"pair",pair++},{"function",key},{"seed",seed},{"reference_nonnull",nonnull},{"player_present",playerPresent},{"camera",camera},{"child_nonnull",child},{"equal",equal},{"payload_equal",payload},{"vptr_valid",vptr},{"data_original",dataAfter[0]},{"data_candidate",dataAfter[1]},{"trace_original",traces[0]},{"trace_candidate",traces[1]},{"trace_expected",expected},{"original_gate",selection[0]},{"candidate_gate",selection[1]}});
        };
        for(int seed=0;seed<16;++seed){for(bool nonnull:{false,true})dtor.operator()<CTaskSimpleFightingControl>(keys[1],seed,nonnull,false,false,false);
            for(bool playerPresent:{false,true})for(bool nonnull:{false,true})for(bool camera:{false,true})for(bool child:{false,true})dtor.operator()<CTaskComplexProstituteSolicit>(keys[3],seed,nonnull,playerPresent,camera,child);
            ReversingProbe::PumpMessages();
        }
    }
    patchRestored=std::ranges::all_of(patched,[](const auto& p){return p.second==ReversingProbe::Bytes(reinterpret_cast<void*>(p.first),52);});
    for(auto& s:snapshots){s.hook->LockState(false);s.hook->State(s.enabled);s.hook->LockState(s.locked);}
    bool flags=true,entries=true,candidates=true,vtables=true;
    for(const auto& s:snapshots){flags&=s.hook->Hooked()==s.enabled&&s.hook->Locked()==s.locked;if(s.gta){entries&=s.gtaBytes==ReversingProbe::Bytes(s.gta,52);candidates&=s.ourBytes==ReversingProbe::Bytes(s.ours,52);}}
    for(const auto& [at,bytes]:tables)vtables&=bytes==ReversingProbe::Bytes(at,bytes.size());
    const bool restored=flags&&entries&&candidates&&vtables&&patchRestored;
    write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored},{"flags_restored",flags},{"entries_restored",entries},{"candidates_restored",candidates},{"tables_restored",vtables},{"collaborator_entries_restored",patchRestored},{"scope","reference/player ABI fixtures and native child; isolated contracts, no real game actors"}});calls=nullptr;
}
}
