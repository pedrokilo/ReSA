#pragma once
#include "../TaskLifetimeComparison.hpp"

namespace RequestedTaskLifetimeExpanded {
using namespace RequestedTaskLifetime;
inline std::vector<json>* trace{};
inline void* expectedParent{};
inline CEntity* expectedLeader{};
inline int32 returnedAnimIndex{}, simulatedRefs{};
inline bool collaboratorsValid{};
inline void Record(json row) { if(trace) trace->push_back(std::move(row)); }
struct Child final : CTask {
    explicit Child(CTaskComplex* parent=nullptr){m_Parent=parent;}
    ~Child() override {Record({{"event","child_destructor"},{"parent_matches",m_Parent==expectedParent}});collaboratorsValid &= m_Parent==expectedParent;}
    static void* operator new(size_t n){return ::operator new(n);}
    static void operator delete(void* p){Record({{"event","child_delete"},{"nonnull",p!=nullptr}});::operator delete(p);}
    CTask* Clone() const override{return nullptr;}
    CTask* GetSubTask() const override{return nullptr;}
    bool IsSimple() const override{return true;}
    eTaskType GetTaskType() const override{return TASK_SIMPLE_STAND_STILL;}
    bool MakeAbortable(CPed*,eAbortPriority,const CEvent*) override{return true;}
};
inline int32 __cdecl AnimationIndex(const char* name){
    const bool matches=name&&strcmp(name,"gangs")==0;collaboratorsValid&=matches;
    Record({{"event","animation_index"},{"name_matches",matches},{"result",returnedAnimIndex}});return returnedAnimIndex;
}
inline void __cdecl RemoveRef(int32 index){
    collaboratorsValid &= index==returnedAnimIndex;
    Record({{"event","remove_anim_ref"},{"index",index},{"before",simulatedRefs},{"after",simulatedRefs-1}});--simulatedRefs;
}
inline void __fastcall CleanRef(CEntity* entity,void*,CEntity** slot){
    const bool entityMatch=entity==expectedLeader;
    const bool slotMatch=reinterpret_cast<uintptr_t>(slot)==reinterpret_cast<uintptr_t>(expectedParent)+0x10;
    const bool pointeeMatch=slot&&*slot==entity;
    collaboratorsValid&=entityMatch&&slotMatch&&pointeeMatch;
    Record({{"event","cleanup_reference"},{"entity_matches",entityMatch},{"slot_matches",slotMatch},{"pointee_matches",pointeeMatch}});
    // The real collaborator removes a reference-chain node, without nulling *slot.
}
inline void Update(){
    static bool done{};if(done)return;done=true;
    char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);
    const auto dir=std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(dir/"requested-task-expanded.start"))return;
    std::ofstream log(dir/"requested-task-expanded.jsonl");
    const auto write=[&](const json& j){log<<j.dump()<<'\n';log.flush();};
    std::vector<Snapshot> snapshots;std::map<std::string,Simple*> targets;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& holder:cat.Items()){
        auto* h=holder.get();Snapshot s{h,h->Hooked(),h->Locked()};
        if(h->Type()==Base::HookType::Simple){auto* p=static_cast<Simple*>(h);s.gta=p->GetHookGTAAddress();s.ours=p->GetHookOurAddress();targets[std::string(cat.Name())+"::"+h->Name()]=p;}
        else if(h->Type()==Base::HookType::Virtual){auto* p=static_cast<Virtual*>(h);s.gta=p->GetHookGTAAddress();s.ours=p->GetHookOurAddress();}
        if(s.gta){s.gtaBytes=ReversingProbe::Bytes(s.gta,52);s.ourBytes=ReversingProbe::Bytes(s.ours,52);}snapshots.push_back(std::move(s));
    }});
    const std::array<const char*,4> keys{"CTaskComplexGangJoinRespond::Destructor","CTaskComplexGoToAttractor::Destructor","CTaskComplexSmartFleePoint::Destructor","CTaskComplexFollowLeaderInFormation::Destructor"};
    for(auto key:keys)if(!targets.contains(key)){write({{"event","error"},{"missing",key}});return;}
    // No automatic lifetime follows this placement object. The explicit base destructor
    // supplies the expected native terminal CTask vptr; dead storage is only read as bytes.
    alignas(Child) std::array<uint8,sizeof(Child)> baseStorage;
    auto* baseObject=::new(baseStorage.data()) Child{};baseObject->CTask::~CTask();
    const auto nativeBaseVptr=*reinterpret_cast<uintptr_t*>(baseStorage.data());
    CTaskComplexGangJoinRespond gang{false};CTaskComplexGoToAttractor attr{nullptr,CVector{0,0,0},0,0,0,PEDMOVE_WALK};
    CTaskComplexSmartFleePoint smart{CVector{0,0,0},false,10,-1};CTaskComplexFollowLeaderInFormation follow{nullptr,nullptr,CVector{0,0,0},10};
    std::vector<std::pair<void*,std::vector<uint8>>> tables;
    for(auto* task:std::array<CTask*,4>{&gang,&attr,&smart,&follow}){auto* p=*reinterpret_cast<void**>(task);tables.emplace_back(p,ReversingProbe::Bytes(p,44));}
    for(uintptr_t p:std::array<uintptr_t,4>{0x86FB5C,0x86FF3C,0x86F744,0x870C3C})tables.emplace_back(reinterpret_cast<void*>(p),ReversingProbe::Bytes(reinterpret_cast<void*>(p),44));
    for(auto& s:snapshots){s.hook->LockState(false);s.hook->State(false);}
    const auto nativeRemove=reinterpret_cast<uintptr_t>(static_cast<void(*)(int32)>(&CAnimManager::RemoveAnimBlockRef));
    std::vector<uintptr_t> patchAddresses{0x4D3990,0x4D3FD0,0x571A00,nativeRemove};
    std::sort(patchAddresses.begin(),patchAddresses.end());patchAddresses.erase(std::unique(patchAddresses.begin(),patchAddresses.end()),patchAddresses.end());
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> patchBytes;
    for(auto p:patchAddresses)patchBytes.emplace_back(p,ReversingProbe::Bytes(reinterpret_cast<void*>(p),52));
    bool passed=true;int pairs=0;bool patchesRestored{};
    {
        ReversingProbe::EntryPatch indexPatch{0x4D3990,&AnimationIndex};
        ReversingProbe::EntryPatch removePatch{0x4D3FD0,&RemoveRef};
        ReversingProbe::EntryPatch cleanPatch{0x571A00,&CleanRef};
        std::unique_ptr<ReversingProbe::EntryPatch> nativeRemovePatch;
        if(nativeRemove!=0x4D3FD0)nativeRemovePatch=std::make_unique<ReversingProbe::EntryPatch>(nativeRemove,&RemoveRef);
        // Sentinel entity identity is never dereferenced: only the declared CleanRef stub receives it.
        std::array<uint8,32> leaderStorage{};expectedLeader=reinterpret_cast<CEntity*>(leaderStorage.data());
        const auto run=[&]<class T>(const char* key,int seed,bool child,bool branch,auto prepare){
            alignas(T) std::array<uint8,sizeof(T)> a,b;a.fill(uint8(0x5A+seed*37));b=a;
            std::array<std::vector<json>,2> calls;std::array<bool,2> gate{},valid{};
            std::array<int32,2> refResult{};
            auto* h=targets.at(key);using Fn=void(__thiscall*)(T*);
            for(int route=0;route<2;++route){
                auto* obj=reinterpret_cast<T*>((route?b:a).data());prepare(obj,branch);
                if(child)*reinterpret_cast<CTask**>(reinterpret_cast<uint8*>(obj)+8)=new Child{obj};
                expectedParent=obj;returnedAnimIndex=seed%3==0?3:seed%3==1?17:26;simulatedRefs=2+seed;collaboratorsValid=true;trace=&calls[route];
                h->State(route==1);
                gate[route]=h->Hooked()==(route==1)&&memcmp(h->GetHookGTAAddress(),route?static_cast<void*>(&h->m_HookContent):static_cast<void*>(h->m_OriginalFunctionContent),h->m_iHookedBytes)==0;
                reinterpret_cast<Fn>(h->GetHookGTAAddress())(obj);
                trace=nullptr;valid[route]=collaboratorsValid;refResult[route]=simulatedRefs;h->State(false);
            }
            const auto originalVptr=*reinterpret_cast<uintptr_t*>(a.data()),candidateVptr=*reinterpret_cast<uintptr_t*>(b.data());
            const bool payload=std::equal(a.begin()+4,a.end(),b.begin()+4);
            const bool terminal=originalVptr==0x86D48C&&candidateVptr==nativeBaseVptr;
            const bool subtasks=*reinterpret_cast<CTask**>(a.data()+8)==nullptr&&*reinterpret_cast<CTask**>(b.data()+8)==nullptr;
            std::vector<json> expected;
            if(std::string_view(key).starts_with("CTaskComplexGangJoinRespond")&&branch){expected.push_back({{"event","animation_index"},{"name_matches",true},{"result",returnedAnimIndex}});expected.push_back({{"event","remove_anim_ref"},{"index",returnedAnimIndex},{"before",2+seed},{"after",1+seed}});}
            if(std::string_view(key).starts_with("CTaskComplexFollowLeaderInFormation")&&branch)expected.push_back({{"event","cleanup_reference"},{"entity_matches",true},{"slot_matches",true},{"pointee_matches",true}});
            if(child){expected.push_back({{"event","child_destructor"},{"parent_matches",true}});expected.push_back({{"event","child_delete"},{"nonnull",true}});}
            const bool traces=calls[0]==expected&&calls[1]==expected;
            const bool equal=gate[0]&&gate[1]&&valid[0]&&valid[1]&&payload&&terminal&&subtasks&&traces&&refResult[0]==refResult[1];passed&=equal;++pairs;
            write({{"event","comparison"},{"function",key},{"seed",seed},{"child_nonnull",child},{"branch_active",branch},{"equal",equal},{"raw_equal",a==b},{"payload_equal",payload},{"terminal_vptr_original",originalVptr},{"terminal_vptr_candidate",candidateVptr},{"expected_native_base_vptr",nativeBaseVptr},{"terminal_vptr_valid",terminal},{"subtask_cleared",subtasks},{"original_gate",gate[0]},{"candidate_gate",gate[1]},{"trace_original",calls[0]},{"trace_candidate",calls[1]},{"trace_expected",expected},{"trace_equal",traces},{"refs_original",refResult[0]},{"refs_candidate",refResult[1]}});
        };
        for(int seed=0;seed<16;++seed)for(bool child:{false,true}){
            for(bool branch:{false,true}){
                run.operator()<CTaskComplexGangJoinRespond>(keys[0],seed,child,branch,[&](auto* p,bool active){::new(p) CTaskComplexGangJoinRespond{bool(seed&1)};p->m_animsReferenced=active;});
                run.operator()<CTaskComplexFollowLeaderInFormation>(keys[3],seed,child,branch,[&](auto* p,bool active){::new(p) CTaskComplexFollowLeaderInFormation{nullptr,nullptr,CVector{float(seed),2,3},10};*reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(p)+0x10)=active?expectedLeader:nullptr;});
            }
            run.operator()<CTaskComplexGoToAttractor>(keys[1],seed,child,false,[&](auto* p,bool){::new(p) CTaskComplexGoToAttractor{nullptr,CVector{float(seed),2,3},0,0,seed,PEDMOVE_WALK};});
            run.operator()<CTaskComplexSmartFleePoint>(keys[2],seed,child,false,[&](auto* p,bool){::new(p) CTaskComplexSmartFleePoint{CVector{float(seed),2,3},false,10,-1};});
            ReversingProbe::PumpMessages();
        }
    }
    patchesRestored=std::ranges::all_of(patchBytes,[](const auto& p){return p.second==ReversingProbe::Bytes(reinterpret_cast<void*>(p.first),52);});
    for(auto& s:snapshots){s.hook->LockState(false);s.hook->State(s.enabled);s.hook->LockState(s.locked);}
    bool flags=true,entries=true,candidates=true,vtables=true;
    for(const auto& s:snapshots){flags&=s.hook->Hooked()==s.enabled&&s.hook->Locked()==s.locked;if(s.gta){entries&=s.gtaBytes==ReversingProbe::Bytes(s.gta,52);candidates&=s.ourBytes==ReversingProbe::Bytes(s.ours,52);}}
    for(const auto& [address,bytes]:tables)vtables&=bytes==ReversingProbe::Bytes(address,bytes.size());
    const bool restored=flags&&entries&&candidates&&vtables&&patchesRestored;
    write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored},{"flags_restored",flags},{"entries_restored",entries},{"candidates_restored",candidates},{"tables_restored",vtables},{"collaborator_entries_restored",patchesRestored},{"scope","all destructor decision branches; isolated animation/reference collaborators; concrete native child destruction; no integration claim"}});
    expectedParent=nullptr;expectedLeader=nullptr;trace=nullptr;
}
}
