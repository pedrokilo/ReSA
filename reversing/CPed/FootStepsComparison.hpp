// Included inside namespace PedCompletion.
namespace Steps {
inline std::array<CAnimBlendAssociation*,3> associations{};
inline int count{},adhesion{},randomValue{};
inline bool mask{},leftLand{},rightLand{};
inline CAnimBlendAssociation* __cdecl First(RpClump*) { return associations[0]; }
inline CAnimBlendAssociation* __cdecl Next(CAnimBlendAssociation* a) {
    for(int i=0;i<count;++i) if(associations[i]==a) return i+1<count ? associations[i+1]:nullptr;
    std::abort();
}
inline uint32 __fastcall Adhesion(void*,void*,uint32 surface) { Foot::discrete.push_back({"adhesion",surface}); return adhesion; }
inline int __cdecl Random() { Foot::discrete.push_back({"random"}); return randomValue; }
inline bool __fastcall Mask(void*,void*) { Foot::discrete.push_back({"mask"}); return mask; }
inline bool __fastcall Left(void*,void*) { Foot::discrete.push_back({"land_left"}); return leftLand; }
inline bool __fastcall Right(void*,void*) { Foot::discrete.push_back({"land_right"}); return rightLand; }
inline void __fastcall Land(CPed*,void*,bool left,uint8 move) { Foot::discrete.push_back({"land",left,move}); }
inline CEvent* __fastcall Event(void*,void*,CEvent* event,bool valid) {
    auto* sound=static_cast<CEventSoundQuiet*>(event);
    Foot::discrete.push_back({"quiet",int(event->GetEventType()),valid,sound->m_startTimeInMs});
    Foot::numbers.push_back(sound->m_fLocalSoundLevel); Foot::Vec(sound->m_position); return nullptr;
}
inline json State(CPed* p) {
    auto exact=Foot::discrete; exact.push_back({"state",bool(p->bDoBloodyFootprints),p->m_nDeathTimeMS});
    auto numbers=Foot::numbers; numbers.push_back(p->m_vecAnimMovingShiftLocal.x); numbers.push_back(p->m_vecAnimMovingShiftLocal.y);
    return {{"exact",exact},{"numbers",numbers}};
}
inline bool Run(CPlayerPed* player,int& pair,double& maximum) {
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if(c.Name()=="CPed") for(auto& h:c.Items()) if(h->Name()=="PlayFootSteps") hook=h.get(); });
    if(!hook) return false;
    const bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    ReversingProbe::EntryPatch first(0x4D15E0,First),next(0x4D6AB0,Next),adh(0x55E5C0,Adhesion),rnd(0x821B1E,Random),msk(0x5A7950,Mask),l(0x679010,Left),r(0x678FE0,Right),land(0x5E5380,Land),audio(0x4E2BB0,Foot::Audio),event(0x4AB420,Event);
    bool passed=true; auto entry=reinterpret_cast<void(__thiscall*)(CPed*)>(0x5E57F0);
    for(int i=0;i<8192 && passed;++i) {
        PedInput a(player,i),b(player,i);
        Storage<CPlayerPedData> dataA,dataB;
        std::memcpy(dataA.get(),player->GetPlayerData(),sizeof(CPlayerPedData)); dataA.get()->m_fMoveBlendRatio=std::array{0.f,1.1f,1.2f,1.5f,1.6f,2.f,2.5f}[(i/11)%7]; dataB.bytes=dataA.bytes;
        auto* da=dataA.get(); auto* db=dataB.get(); std::memcpy(a.ped.bytes.data()+0x480,&da,4); std::memcpy(b.ped.bytes.data()+0x480,&db,4);
        CTaskSimpleLand taskA(ANIM_ID_IDLE),taskB(ANIM_ID_IDLE);
        a.intelligence.get()->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_DEFAULT]=&taskA;
        b.intelligence.get()->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_DEFAULT]=&taskB;
        for(auto* p:{a.ped.get(),b.ped.get()}) {
            p->m_pReferences=nullptr;
            p->bDoBloodyFootprints=(i&1)!=0; p->m_nDeathTimeMS=std::array{0,1,2,299,300,-1}[(i/2)%6];
            p->bIsStanding=(i%29)!=0; p->bIsLanding=(i%7)==0; p->bIsDucking=(i&2)!=0;
            p->m_pStats=&CPedStats::ms_apPedStats[i%3 ? int(ePedStats::PLAYER):int(ePedStats::SKATER)];
            p->GetAE().m_bCanAddEvent=(i&4)!=0;
            p->m_nMoveState=eMoveState((i/8)%8); p->m_nAnimGroup=(i&8) ? ANIM_GROUP_PLAYERSNEAK:ANIM_GROUP_PLAYER;
            p->GetPosition()=CVector{20.f,30.f,4.f};
        }
        std::array<CAnimBlendAssociation,3> aa,bb;
        std::array<CAnimBlendHierarchy,3> ha,hb;
        count=1+(i/17)%3; adhesion=(i/13)%6; randomValue=std::array{0,1,63,64,127,128}[(i/19)%6]; mask=(i&16)!=0; leftLand=(i&32)!=0; rightLand=(i&64)!=0;
        for(int j=0;j<3;++j) {
            ha[j].m_fTotalTime=std::array{1.f,1.5f,3.f}[(i/23)%3]; hb[j].m_fTotalTime=ha[j].m_fTotalTime;
            for(auto* an:{&aa[j],&bb[j]}) {
                an->m_AnimId=std::array{ANIM_ID_WALK,ANIM_ID_RUN,ANIM_ID_SPRINT,ANIM_ID_FIGHT_IDLE,ANIM_ID_IDLE}[(i+j)%5];
                an->m_Flags=std::array<uint16,6>{0,ANIMATION_WALK,ANIMATION_IS_PARTIAL,ANIMATION_DONT_ADD_TO_PARTIAL_BLEND,ANIMATION_WALK|ANIMATION_IS_PARTIAL,ANIMATION_WALK}[(i/5+j)%6];
                an->m_BlendAmount=std::array{0.f,.25f,.5f,.51f,1.f}[(i/7+j)%5];
                const float total=ha[j].m_fTotalTime, low=total*(1.f/15.f)+(a.ped.get()->bIsDucking ? .2f:0.f),high=total*.5f+low;
                an->m_CurrentTime=std::array{0.f,.01f,.1f,low-.001f,low,low+.001f,high-.001f,high,high+.001f,.34f,.54f,1.f}[(i/3)%12];
                an->m_TimeStep=std::array{0.f,.001f,.02f,.1f,1.f}[(i/31)%5];
                an->m_NumBlendNodes=0;
            }
            aa[j].m_BlendHier=&ha[j]; bb[j].m_BlendHier=&hb[j];
        }
        const auto beforeA=a.ped.bytes,beforeB=b.ped.bytes;
        for(int j=0;j<3;++j) associations[j]=&aa[j]; Foot::discrete=json::array(); Foot::numbers=json::array();
        hook->State(false); const auto start=PedCompletionProbe::calls[2]; entry(a.ped.get()); const auto middle=PedCompletionProbe::calls[2]; const auto original=State(a.ped.get());
        for(int j=0;j<3;++j) associations[j]=&bb[j]; Foot::discrete=json::array(); Foot::numbers=json::array();
        hook->State(true); entry(b.ped.get()); const auto end=PedCompletionProbe::calls[2]; const auto candidate=State(b.ped.get());
        bool bounded=true;
        for(size_t n=0;n<beforeA.size();++n) if(n!=0x46F && !(n>=0x750 && n<0x754) && !(n>=0x4D8 && n<0x4E0)) bounded &= a.ped.bytes[n]==beforeA[n] && b.ped.bytes[n]==beforeB[n];
        passed=Equal(original,candidate,maximum)&&bounded&&middle==start&&end==middle+1;
        Write({{"event","comparison"},{"function","PlayFootSteps"},{"pair",pair++},{"input",i},{"original",original},{"candidate",candidate},{"equal",passed},{"bounded",bounded},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
    }
    hook->State(enabled); hook->LockState(locked); return passed;
}
}

