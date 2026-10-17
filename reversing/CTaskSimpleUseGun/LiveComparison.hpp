#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskSimpleUseGun.h"
#include "TaskSimpleFight.h"
#include "Probe.hpp"
namespace GunLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } } log << r.dump() << '\n'; log.flush(); }
inline int Report(int type, char* msg, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event","assertion"},{"message",msg ? msg : ""}}); ExitProcess(91);
}
inline std::vector<uint8> Bytes(const void* ptr, size_t size) {
    const auto* p = static_cast<const uint8*>(ptr); return {p,p+size};
}
// FightStrike lo llama la DLL directamente (CTaskSimpleFight::FightStrike, hook 0x6240B0): se parchea la entrada del EXE y la de la DLL (sin thunks) con el mismo falso.
struct StrikePatch {
    std::array<uint8,5> saved, savedDll;
    uintptr_t dll;
    static uintptr_t Body(uintptr_t p) { for(int i=0;i<16&&*reinterpret_cast<uint8*>(p)==0xE9;++i) p+=5+*reinterpret_cast<int32*>(p+1); return p; }
    StrikePatch() : dll(Body(std::bit_cast<uintptr_t>(&CTaskSimpleFight::FightStrike))) {
        Install(0x6240B0, saved);
        Install(dll, savedDll);
    }
    static void Install(uintptr_t address, std::array<uint8,5>& save) {
        auto* p = reinterpret_cast<uint8*>(address); std::memcpy(save.data(),p,5);
        std::array<uint8,5> jump{0xE9}; const auto offset=uint32(reinterpret_cast<uintptr_t>(Strike)-address-5);
        std::memcpy(jump.data()+1,&offset,4); Put(address, jump);
    }
    static void __fastcall Strike(CTaskSimpleFight* fight, void*, CPed*, CVector* pos) {
        GunProbe::events.push_back({"FightStrike",pos->x,pos->y,pos->z,int(fight->m_nComboSet),int(fight->m_nCurrentMove),fight->m_nLastCommand,fight->m_pAnim!=nullptr});
    }
    static void Put(uintptr_t address, const std::array<uint8,5>& bytes) {
        void* p=reinterpret_cast<void*>(address); DWORD old,ignored;
        VirtualProtect(p,5,PAGE_EXECUTE_READWRITE,&old); std::memcpy(p,bytes.data(),5);
        VirtualProtect(p,5,old,&ignored); FlushInstructionCache(GetCurrentProcess(),p,5);
    }
    ~StrikePatch() { Put(dll, savedDll); Put(0x6240B0, saved); }
};
template<class T> struct Storage {
    alignas(T) std::array<uint8,sizeof(T)> bytes{};
    T* get() { return reinterpret_cast<T*>(bytes.data()); }
};
struct Fixture {
    Storage<CPlayerPed> ped;
    Storage<CPlayerPedData> data;
    Storage<CPedIntelligence> intelligence;
    Storage<CMatrixLink> matrix;
    Storage<CPlayerPed> target;
    Storage<CMatrixLink> targetMatrix;
    CWeaponInfo previousInfo{};
    CAnimBlendHierarchy hierarchy;
    CAnimBlendAssociation anim;
    CTaskSimpleUseGun task{nullptr};
    Fixture(CPlayerPed* source, int index) {
        std::memcpy(ped.get(),source,sizeof(CPlayerPed));
        std::memcpy(data.get(),source->GetPlayerData(),sizeof(CPlayerPedData));
        std::memcpy(matrix.get(),source->m_matrix,sizeof(CMatrixLink));
        auto* p=ped.get(); p->m_matrix=matrix.get(); auto* pd=data.get(); auto* pi=intelligence.get(); std::memcpy(ped.bytes.data()+0x480,&pd,4); std::memcpy(ped.bytes.data()+0x47C,&pi,4);
        intelligence.get()->m_pPed=p; intelligence.get()->m_TaskMgr.m_pPed=p;
        p->m_nPedType=PED_TYPE_PLAYER1; p->m_nActiveWeaponSlot=0; data.get()->m_nChosenWeapon=0;
        p->GetActiveWeapon().m_Type=WEAPON_PISTOL; p->GetActiveWeapon().m_State=WEAPONSTATE_READY;
        p->bIsDucking=p->bRightArmBlocked=p->bLeftArmBlocked=p->bDuckRightArmBlocked=false;
        p->m_pedIK.bUseArm=false;
        task.m_MoveCmd={.125f,-.25f}; task.m_BurstLength=3; task.m_BurstShots=2;
        task.m_IsFiringGunRightHandThisFrame=task.m_IsFiringGunLeftHandThisFrame=true;
        task.m_IsArmIKInUse=task.m_IsLookIKInUse=true;
        task.m_CountDownFrames=10;
        hierarchy.m_fTotalTime=2.f; hierarchy.m_bKeepCompressed=true;
        anim.m_NumBlendNodes=0; anim.m_BlendHier=&hierarchy;
        anim.m_AnimId=ANIM_ID_RELOAD; anim.m_CurrentTime=.5f; anim.m_TimeStep=.1f; anim.m_Flags=ANIMATION_IS_PLAYING;
        data.get()->m_fAttackButtonCounter=std::array{-2.f,0.f,.5f,10.f}[index%4];
        if (index<64) {
            task.m_IsInControl=false;
            task.m_CountDownFrames=uint8(index+1);
            if (index%4==0) data.get()->m_nChosenWeapon=1;
        } else {
            const int n=index-64, mode=n/81;
            task.m_LastCmd=eGunCommand(n%9-1); task.m_NextCmd=eGunCommand((n/9)%9-1);
            task.m_IsInControl=mode!=0; task.m_IsFinished=mode==1;
            task.m_HasMoveControl=(mode&1)!=0; task.m_HasFiredGun=(mode&2)!=0;
            task.m_CountDownFrames=uint8(n%3); task.m_BurstShots=std::array<uint16,4>{0,1,3,65535}[n%4];
            task.m_TargetPos=CVector{float((n%7)-3)*10.f,float((n%5)-2)*10.f,float(n%3)*10.f};
            task.m_SkipAim=true; task.m_IsLOSBlocked=(mode&1)!=0;
            if (mode%4) task.m_Anim=&anim;
            anim.m_AnimId=AnimationId(std::array{ANIM_ID_RELOAD,ANIM_ID_CROUCHRELOAD,ANIM_ID_IDLE}[n%3]);
            anim.m_CurrentTime=std::array{0.f,.249f,.25f,.251f,.49f,.5f,.51f,.75f,.751f,1.f,1.1f,2.f}[n%12];
            anim.m_TimeStep=std::array{0.f,.01f,.1f,.5f}[mode%4];
            anim.m_BlendAmount=std::array{0.f,.5f,.99f,1.f}[n%4];
            anim.m_BlendDelta=std::array{-4.f,0.f,4.f}[mode%3];
            anim.m_Flags=uint16((mode&1 ? ANIMATION_IS_PLAYING:0)|(mode&2 ? ANIMATION_DONT_ADD_TO_PARTIAL_BLEND:0));
            p->bIsDucking=(mode&2)!=0; p->bRightArmBlocked=mode%7==0; p->bDuckRightArmBlocked=mode%7==1; p->bLeftArmBlocked=mode%5==1;
            p->m_pedIK.bUseArm=(mode&4)!=0;
            if (mode%5==0) { std::memset(ped.bytes.data()+0x480,0,4); p->m_nPedType=PED_TYPE_CIVMALE; p->m_nWeaponSkill=eWeaponSkill::STD; }
            p->GetActiveWeapon().m_State=mode%4==0 ? WEAPONSTATE_RELOADING:WEAPONSTATE_READY;
        }
        if(index>=2656 && index<3168) {
            const int n=index-2656;
            std::memcpy(target.get(),source,sizeof(CPlayerPed));
            std::memcpy(targetMatrix.get(),source->m_matrix,sizeof(CMatrixLink));
            target.get()->m_matrix=targetMatrix.get(); target.get()->m_pReferences=nullptr;
            target.get()->bCalledPreRender=false;target.get()->m_fHealth=n&1 ? 0.f:100.f;
            if(n%3==0)target.get()->SetType(ENTITY_TYPE_OBJECT);
            const float angle=std::array{-3.14159f,-2.269f,-2.2689f,-2.f,-1.571f,-1.5707f,-.1f,0.f,.1f,1.3089f,1.309f,1.3091f,2.007f,2.00713f,2.0072f,3.14159f}[n%16];
            targetMatrix.get()->GetPosition()=p->GetPosition()+CVector{-std::sin(angle)*10.f,std::cos(angle)*10.f,n%4==0 ? -25.f:0.f};
            auto* entity=target.get();std::memcpy(reinterpret_cast<uint8*>(&task)+0x1C,&entity,4);
            task.m_IsInControl=true;task.m_IsFinished=false;task.m_LastCmd=n%7==0 ? eGunCommand::PISTOLWHIP:eGunCommand::AIM;task.m_NextCmd=eGunCommand::AIM;
            task.m_Anim=&anim;anim.m_BlendDelta=0.f;anim.m_CurrentTime=.3f;anim.m_BlendAmount=1.f;anim.m_TimeStep=.01f;anim.m_Flags=ANIMATION_IS_PLAYING;
            p->bRightArmBlocked=false;p->bDuckRightArmBlocked=false;p->m_pedIK.bUseArm=true;p->m_fCurrentRotation=0.f;
        }
        if(index>=3168) {
            task.m_IsInControl=true;task.m_IsFinished=false;
        }
    }
    ~Fixture() { task.m_Anim=nullptr;std::memset(reinterpret_cast<uint8*>(&task)+0x1C,0,4); }
    auto ReadOnlyStorage() {
        auto bytes=Bytes(matrix.get(),sizeof(CMatrixLink));
        for(const auto& extra : {Bytes(target.get(),sizeof(CPlayerPed)),Bytes(targetMatrix.get(),sizeof(CMatrixLink)),Bytes(intelligence.get(),sizeof(CPedIntelligence)),Bytes(&hierarchy,sizeof(hierarchy))}) bytes.insert(bytes.end(),extra.begin(),extra.end());
        return bytes;
    }
    json Snapshot(bool result) {
        auto t=Bytes(&task,sizeof(task));
        for (int off : {0,4,28,44,48}) std::fill(t.begin()+off,t.begin()+off+4,0);
        // The six unused bits sharing the firing flags have no C++ value.
        t[13]&=3;
        auto p=Bytes(ped.get(),sizeof(CPlayerPed));
        for (size_t off : {size_t(0x14),size_t(0x480),size_t(0x47C)}) std::fill(p.begin()+off,p.begin()+off+4,0);
        auto d=data.bytes; std::fill(d.begin()+offsetof(CPlayerPedData,m_fAttackButtonCounter),d.begin()+offsetof(CPlayerPedData,m_fAttackButtonCounter)+4,0);
        auto callback=reinterpret_cast<uintptr_t>(anim.m_pCallbackFunc);
        if(callback==0x4D1390 || anim.m_pCallbackFunc==CDefaultAnimCallback::DefaultAnimCB) callback=1;
        const bool pointersValid=(!task.m_Anim || task.m_Anim==&anim) && (!task.m_WeaponInfo || task.m_WeaponInfo==&previousInfo || task.m_WeaponInfo==&ped.get()->GetActiveWeapon().GetWeaponInfo(ped.get()));
        return {{"exact",{result,t,p,d,task.m_Anim!=nullptr,task.m_WeaponInfo!=nullptr,anim.m_Flags,int16(anim.m_AnimId),anim.m_nCallbackType,callback,anim.m_pCallbackData==nullptr,pointersValid}},
                {"numbers",{anim.m_BlendAmount,anim.m_BlendDelta,anim.m_CurrentTime,anim.m_TimeStep,data.get()->m_fAttackButtonCounter}}, {"events",GunProbe::events}};
    }
};
inline bool Equal(const json& a,const json& b) {
    if(a.is_object()) { if(!b.is_object()||a.size()!=b.size())return false; for(auto it=a.begin();it!=a.end();++it)if(!b.contains(it.key()) || (it.key()=="exact" ? it.value()!=b[it.key()]:!Equal(it.value(),b[it.key()])))return false; return true; }
    if(a.is_array()) { if(!b.is_array()||a.size()!=b.size())return false;for(size_t i=0;i<a.size();++i)if(!Equal(a[i],b[i]))return false;return true; }
    if(a.is_number_float()||b.is_number_float()) { if(!a.is_number()||!b.is_number())return false;double x=a,y=b;return std::isfinite(x)&&std::isfinite(y)&&std::abs(x-y)<=.00001; }
    return a==b;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"usegun-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"usegun-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed();if(!player||!player->GetPlayerData()||!player->m_matrix||CCutsceneMgr::IsRunning())return;
    done=true;auto report=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CTaskSimpleUseGun")for(auto& h:c.Items())if(h->Name()=="ProcessPed")hook=h.get();});
    if(!hook){Write({{"event","error"},{"reason","missing hook"}});_CrtSetReportHook(report);return;}
    const auto livePed=Bytes(player,sizeof(CPlayerPed)),liveData=Bytes(player->GetPlayerData(),sizeof(CPlayerPedData)),liveIntelligence=Bytes(player->GetIntelligence(),sizeof(CPedIntelligence)),liveMatrix=Bytes(player->m_matrix,sizeof(CMatrixLink));
    const auto step=CTimer::GetTimeStep();const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    Write({{"event","player_ready"},{"independent_inputs",true}});
    bool passed=true;int pairs=0;
    {
        StrikePatch strike;
        for(int i=0;i<3232 && passed;++i) {
            Fixture a(player,i),b(player,i);
            auto* info=&a.ped.get()->GetActiveWeapon().GetWeaponInfo(a.ped.get());
            const CWeaponInfo savedInfo=*info;
            const int mode=i<64 ? i%4:(i-64)/81;
            info->m_nWeaponFire=(i<64 && mode==1)?WEAPON_FIRE_MELEE:WEAPON_FIRE_INSTANT_HIT;
            info->flags.bThrow=i<64 && mode==2;
            info->flags.bAimWithArm=(mode&1)!=0;info->flags.b1stPerson=(mode&2)!=0;
            info->flags.bContinuosFire=(mode&4)!=0;info->flags.bTwinPistol=(mode&8)!=0;info->flags.bExpands=(mode&16)!=0;
            info->m_fAnimLoopStart=info->m_fAnimLoop2Start=.25f;info->m_fAnimLoopEnd=info->m_fAnimLoop2End=1.f;
            info->m_fAnimLoopFire=info->m_fAnimLoop2Fire=.5f;info->m_fBreakoutTime=.75f;
            if(i>=64){a.task.m_WeaponInfo=b.task.m_WeaponInfo=info;}
            if(i>=3168){a.previousInfo=b.previousInfo=*info;a.task.m_WeaponInfo=&a.previousInfo;b.task.m_WeaponInfo=&b.previousInfo;}
            const auto aReadOnly=a.ReadOnlyStorage(),bReadOnly=b.ReadOnlyStorage();const auto inputInfo=Bytes(info,sizeof(*info));
            const bool independent=a.ped.get()!=b.ped.get()&&a.data.get()!=b.data.get()&&a.intelligence.get()!=b.intelligence.get()&&a.matrix.get()!=b.matrix.get()&&a.target.get()!=b.target.get()&&&a.anim!=&b.anim&&&a.hierarchy!=&b.hierarchy;
            CTimer::SetTimeStep(std::array{0.f,.5f,1.f,2.f}[i%4]);
            GunProbe::controlled=i>=64;GunProbe::startCreates=(mode&1)!=0;
            hook->State(false);GunProbe::events=json::array();GunProbe::nextAnim=&a.anim;GunProbe::recording=true;
            const auto start=GunProbe::calls;
            auto entry=reinterpret_cast<bool(__thiscall*)(CTaskSimpleUseGun*,CPed*)>(0x62A380);
            const bool ar=entry(&a.task,a.ped.get());GunProbe::recording=false;const auto middle=GunProbe::calls;const auto original=a.Snapshot(ar);
            const bool originalInputsUnchanged=a.ReadOnlyStorage()==aReadOnly&&Bytes(info,sizeof(*info))==inputInfo;
            hook->State(true);GunProbe::events=json::array();GunProbe::nextAnim=&b.anim;GunProbe::recording=true;
            const bool br=entry(&b.task,b.ped.get());GunProbe::recording=false;const auto end=GunProbe::calls;const auto candidate=b.Snapshot(br);
            const bool inputsUnchanged=originalInputsUnchanged&&b.ReadOnlyStorage()==bReadOnly&&Bytes(info,sizeof(*info))==inputInfo;
            passed=Equal(original,candidate)&&middle==start&&end==middle+1&&independent&&inputsUnchanged;
            Write({{"event","comparison"},{"pair",i},{"controlled",i>=64},{"original",original},{"candidate",candidate},{"equal",passed},{"independent_inputs",independent},{"inputs_unchanged",inputsUnchanged},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
            *info=savedInfo;++pairs;
        }
    }
    CTimer::SetTimeStep(step);hook->State(enabled);hook->LockState(locked);
    const bool restored=hook->Hooked()==enabled&&hook->Locked()==locked&&Bytes(player,sizeof(CPlayerPed))==livePed&&Bytes(player->GetPlayerData(),sizeof(CPlayerPedData))==liveData&&Bytes(player->GetIntelligence(),sizeof(CPedIntelligence))==liveIntelligence&&Bytes(player->m_matrix,sizeof(CMatrixLink))==liveMatrix;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pairs},{"state_restored",restored}});_CrtSetReportHook(report);
}
}
