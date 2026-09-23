// Included inside namespace PedCompletion.
namespace Kill {
inline int input{},randomCalls{};
inline CPed* victim{};
inline CVehicle* car{};
inline CObject* bonnet{};
inline CTask* active{};
inline CTask* destroy{};
inline Storage<CAnimBlendAssociation> animation;
inline int __fastcall Type(CTask*,void*) { return std::array{207,212,218,203}[(input/17)%4]; }
inline CTask* __fastcall Active(CTaskManager*,void*) { return active; }
inline CTask* __fastcall Destroy(CTaskManager*,void*,eTaskType) { return destroy; }
inline int __cdecl Random() { auto v=(input+randomCalls++*19)%256; Foot::discrete.push_back({"random",v}); return v; }
inline CVehicle* __cdecl Player(int32,bool) { return input%7==0 ? car:nullptr; }
inline CObject* __fastcall Bonnet(CAutomobile*,void*) { Foot::discrete.push_back({"bonnet"}); return input%2 ? bonnet:nullptr; }
inline bool __cdecl Knock() { Foot::discrete.push_back({"knock"}); return input%3!=0; }
inline void __cdecl Crime(eCrimeType type,CEntity*,CPed*) { Foot::discrete.push_back({"crime",int(type)}); }
inline bool __fastcall Affects(CEventDamage* event,void*,CPed*) {
    Foot::discrete.push_back({"affects",int(event->m_weaponType),int(event->m_pedPieceType),event->m_ucDirection,bool(event->m_bWitnessedInVehicle)}); return input%5!=0;
}
inline void __fastcall Damage(CPedDamageResponseCalculator* calc,void*,CPed*,CPedDamageResponse* response,bool speak) {
    Foot::discrete.push_back({"damage",int(calc->m_weaponType),int(calc->m_bodyPart),calc->m_bSpeak,speak}); Foot::numbers.push_back(calc->m_fDamageFactor);
    response->m_fDamageHealth=calc->m_fDamageFactor*.5f; response->m_bDamageCalculated=true;
}
inline CAnimBlendAssociation* __cdecl Blend(RpClump*,AssocGroupId group,AnimationId anim,float amount) {
    Foot::discrete.push_back({"blend",int(group),int(anim)}); Foot::numbers.push_back(amount); return animation.get();
}
inline CEvent* __fastcall Add(CEventGroup*,void*,CEventDamage* e,bool valid) {
    Foot::discrete.push_back({"event",valid,int(e->m_weaponType),int(e->m_nAnimGroup),int(e->m_nAnimID),e->m_ucDirection,bool(e->m_bAnimAdded),bool(e->m_damageResponse.m_bDamageCalculated)});
    for(float f:{e->m_fAnimBlend,e->m_fAnimSpeed,e->m_damageResponse.m_fDamageHealth,animation.get()->m_Speed}) Foot::numbers.push_back(f); return nullptr;
}
inline void __fastcall Say(CPed*,void*,int context,uint32 delay,float probability,bool a,bool b,bool c) {
    Foot::discrete.push_back({"say",context,delay,a,b,c}); Foot::numbers.push_back(probability);
}
inline void __fastcall Force(CPhysical*,void*,CVector force,CVector point,bool turn) {
    Foot::discrete.push_back({"force",turn}); Foot::Vec(force); Foot::Vec(point);
}
inline bool Run(CPlayerPed* player,int& pair,double& maximum) {
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if(c.Name()=="CPed") for(auto& h:c.Items()) if(h->Name()=="KillPedWithCar") hook=h.get(); });
    if(!hook) return false;
    const bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    CColModel model; model.m_boundBox.m_vecMin=CVector{-1.f,-2.f,-.5f}; model.m_boundBox.m_vecMax=CVector{1.f,2.f,1.5f}; Position::col=&model;
    ReversingProbe::EntryPatch col(0x535300,Position::Col),ac(0x6819D0,Active),de(0x681740,Destroy),ra(0x821B1E,Random),pl(0x56E0D0,Player),bo(0x6AA200,Bonnet),kn(0x56D290,Knock),cr(0x532010,Crime),af(0x4B35A0,Affects),da(0x4B5AC0,Damage),bl(0x4D4610,Blend),ad(0x4AB420,Add),sa(0x5EFFE0,Say),fo(0x542B50,Force),au(0x4F6420,Tyres::Audio),sh(0x53F920,Tyres::Shake);
    auto entry=reinterpret_cast<void(__thiscall*)(CPed*,CVehicle*,float,bool)>(0x5F0360); bool passed=true;
    for(int i=0;i<8192;++i) {
        input=i; PedInput a(player,i),b(player,i); Tyres::VehicleInput va(i,0),vb(i,0); Storage<CObject> oa,ob; CMatrixLink oma,omb; oma.SetUnity(); omb.SetUnity(); oa.get()->m_matrix=&oma; ob.get()->m_matrix=&omb; Storage<CTaskComplexDestroyCarMelee> ta,tb; std::array<void*,5> vtable{}; vtable[4]=reinterpret_cast<void*>(&Type); void** vt=vtable.data(); std::array<uint8,16> task{}; std::memcpy(task.data(),&vt,4);
        tHandlingData handling{}; handling.m_bIsBig=(i/19)%2;
        for(auto* v:{va.get(),vb.get()}) {
            v->SetType(ENTITY_TYPE_VEHICLE); v->SetStatus(i%3 ? STATUS_PHYSICS:STATUS_PLAYER);
            v->m_nVehicleType=std::array{VEHICLE_TYPE_AUTOMOBILE,VEHICLE_TYPE_BIKE,VEHICLE_TYPE_BOAT}[(i/11)%3];
            v->m_nVehicleSubType=std::array{VEHICLE_TYPE_AUTOMOBILE,VEHICLE_TYPE_TRAIN,VEHICLE_TYPE_PLANE}[(i/7)%3];
            v->m_pHandlingData=&handling; v->m_fMass=std::array{100.f,800.f,1600.f,2400.f}[(i/23)%4];
            v->m_matrix->SetRotate(float(i%5-2)*.3f,0.f,float(i%4)*.5f);
            v->m_vecMoveSpeed=v->GetForward()*std::array{-.2f,0.f,.15f,.2f,.5f}[(i/13)%5]; v->m_vecMoveSpeed.z+=.01f;
            v->m_pDriver=i%2 ? player:nullptr; v->m_pReferences=nullptr;
        }
        for(auto* p:{a.ped.get(),b.ped.get()}) {
            p->m_nPedType=i%4==0 ? PED_TYPE_PLAYER1:i%4==1 ? PED_TYPE_COP:PED_TYPE_CIVMALE;
            p->GetPosition()=CVector{std::array{-.99f,0.f,.5f,1.01f}[(i/3)%4],float((i/5)%5-2)*.6f,std::array{0.f,.1f,.3f,1.f}[(i/17)%4]};
            p->m_vecMoveSpeed=CVector{.1f,.2f,.3f}; p->m_vecLastCollisionImpactVelocity.z=std::array{-1.f,-.8f,0.f}[(i/2)%3];
            p->m_bHasHitWall=(i/31)%2; p->bIsStanding=true; p->bKnockedUpIntoAir=(i/37)%2; p->bInVehicle=(i/41)%2;
            p->m_pContactEntity=nullptr; p->m_pVehicle=nullptr; p->m_pEntityIgnoredCollision=nullptr; p->m_pReferences=nullptr;
        }
        for(auto* o:{oa.get(),ob.get()}) { o->m_fMass=20.f; o->m_fTurnMass=10.f; }
        a.ped.get()->m_pContactEntity=i%43==0 ? va.get():nullptr; b.ped.get()->m_pContactEntity=i%43==0 ? vb.get():nullptr;
        a.ped.get()->m_pVehicle=i%47==0 ? va.get():nullptr; b.ped.get()->m_pVehicle=i%47==0 ? vb.get():nullptr;
        a.ped.get()->m_pEntityIgnoredCollision=i%53==0 ? va.get():nullptr; b.ped.get()->m_pEntityIgnoredCollision=i%53==0 ? vb.get():nullptr;
        ta.get()->m_VehToDestroy=va.get(); tb.get()->m_VehToDestroy=vb.get();
        const auto beforeA=a.ped.bytes,beforeB=b.ped.bytes; const auto ia=a.intelligence.bytes,ib=b.intelligence.bytes; const auto carA=va.Snapshot(),carB=vb.Snapshot();
        auto run=[&](PedInput& ped,Tyres::VehicleInput& vehicle,Storage<CObject>& object,Storage<CTaskComplexDestroyCarMelee>& dt) {
            car=vehicle.get(); victim=ped.ped.get(); bonnet=object.get(); active=i%17==0 ? reinterpret_cast<CTask*>(task.data()):nullptr; destroy=i%29==0 ? dt.get():nullptr;
            Tyres::vehicles={car,nullptr,nullptr}; randomCalls=0; animation.bytes.fill(0); Foot::discrete=json::array(); Foot::numbers=json::array();
            entry(victim,car,std::array{0.f,3.f,3.01f,6.f,6.01f,10.f,10.01f,12.f,12.01f,30.f}[(i/2)%10],i%2);
            Foot::discrete.push_back({"state",bool(victim->bIsStanding),bool(victim->bKnockedUpIntoAir),victim->m_pEntityIgnoredCollision==car,ped.intelligence.get()->m_collisionScanner.m_bAlreadyHitByCar,car->m_pReferences==nullptr,victim->m_pReferences==nullptr});
            Foot::Vec(victim->GetPosition()); Foot::Vec(victim->m_vecMoveSpeed); Foot::Vec(bonnet->m_vecMoveSpeed); Foot::Vec(bonnet->m_vecTurnSpeed);
            return json{{"exact",Foot::discrete},{"numbers",Foot::numbers}};
        };
        hook->State(false); const auto start=PedCompletionProbe::calls[5]; const auto original=run(a,va,oa,ta); const auto middle=PedCompletionProbe::calls[5];
        hook->State(true); const auto candidate=run(b,vb,ob,tb); const auto end=PedCompletionProbe::calls[5];
        const auto boundedPed=[](const auto& before,const auto& after) { for(size_t n=0;n<before.size();++n) if(!(n>=0x44 && n<0x50) && !(n>=0x128 && n<0x12C) && n!=0x46C && n!=0x470 && before[n]!=after[n]) return false; return true; };
        auto boundedInt=[](const auto& before,const auto& after) { for(size_t n=0;n<before.size();++n) if(n!=0x260 && before[n]!=after[n]) return false; return true; };
        const bool bounded=boundedPed(beforeA,a.ped.bytes)&&boundedPed(beforeB,b.ped.bytes)&&boundedInt(ia,a.intelligence.bytes)&&boundedInt(ib,b.intelligence.bytes)&&va.Snapshot()==carA&&vb.Snapshot()==carB;
        const bool equal=Equal(original,candidate,maximum)&&bounded&&middle==start&&end==middle+1;
        passed &= equal;
        Write({{"event","comparison"},{"function","KillPedWithCar"},{"pair",pair++},{"input",i},{"original",original},{"candidate",candidate},{"equal",equal},{"bounded",bounded},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
    }
    hook->State(enabled); hook->LockState(locked); return passed;
}
}

