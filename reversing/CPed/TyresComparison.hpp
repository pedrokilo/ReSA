// Included inside namespace PedCompletion.
namespace Tyres {
inline CVehicle* playerVehicle{};
inline std::array<CVehicle*,3> vehicles{};
inline int Id(CVehicle* p) { for(int i=0;i<3;++i) if(vehicles[i]==p) return i; return -1; }
inline void __fastcall Audio(CAEVehicleAudioEntity* a,void*,eAudioEvents e,float volume) {
    Foot::discrete.push_back({"vehicle_audio",Id(reinterpret_cast<CVehicle*>(reinterpret_cast<uint8*>(a)-0x138)),int(e)}); Foot::numbers.push_back(volume);
}
inline CVehicle* __cdecl Player(int32 id,bool remote) { Foot::discrete.push_back({"player_vehicle",id,remote}); return playerVehicle; }
inline void __fastcall Shake(CPad*,void*,int16 time,uint8 frequency,uint32 arg) { Foot::discrete.push_back({"shake",time,frequency,arg}); }
struct VehicleInput {
    Storage<CAutomobile> car;
    Storage<CBike> bike;
    CMatrixLink matrix;
    bool isBike;
    CVehicle* get() { return isBike ? static_cast<CVehicle*>(bike.get()):static_cast<CVehicle*>(car.get()); }
    VehicleInput(int i,int index) : isBike(index==1) {
        auto* v=get(); v->m_nVehicleType=index==2 ? VEHICLE_TYPE_BOAT:isBike ? VEHICLE_TYPE_BIKE:VEHICLE_TYPE_AUTOMOBILE;
        v->m_matrix=&matrix; matrix.SetRotateZ(float(i%5)*.25f); matrix.GetPosition()=CVector{float(index)*.1f,0.f,0.f};
        v->m_nModelIndex=400; v->m_fMass=std::array{50.f,100.f,101.f,500.f,501.f,1000.f}[(i/3)%6]; v->m_fTurnMass=300.f;
        v->m_vecMoveSpeed=CVector{std::array{0.f,.1f,.23f,.5f}[(i/7)%4],0.f,.1f};
        v->SetScanCode((i%13)==0 ? CWorld::GetCurrentScanCode():uint16(CWorld::GetCurrentScanCode()-1));
        v->physicalFlags.bInfiniteMass=(i%23)==0; v->physicalFlags.bDisableTurnForce=(i%29)==0;
        for(int n=0;n<4;++n) {
            float compression=std::array{0.f,.9f,1.f,1.1f}[(i+n)%4];
            if(isBike) bike.get()->m_aWheelRatios[n]=compression;
            else car.get()->m_fWheelsSuspensionCompression[n]=compression;
        }
        if(isBike) bike.get()->m_bWheelBloody={bool(i&1),bool(i&2)};
        else car.get()->m_wheelSkidmarkBloodState={bool(i&1),bool(i&2),bool(i&4),bool(i&8)};
    }
    json State() {
        auto* v=get(); json flags=json::array();
        if(isBike) for(bool b:bike.get()->m_bWheelBloody) flags.push_back(b);
        else for(bool b:car.get()->m_wheelSkidmarkBloodState) flags.push_back(b);
        Foot::discrete.push_back({"vehicle_state",v->GetScanCode(),flags}); Foot::Vec(v->m_vecMoveSpeed); Foot::Vec(v->m_vecTurnSpeed); return nullptr;
    }
    std::vector<uint8> Snapshot() { return Bytes(get(),isBike ? sizeof(CBike):sizeof(CAutomobile)); }
    bool Bounded(const std::vector<uint8>& before) {
        auto after=Snapshot(); for(size_t n=0;n<before.size();++n) if(!(n>=0x2C && n<0x2E) && !(n>=0x44 && n<0x5C) && !(n>=(isBike ? 0x74Cu:0x820u) && n<(isBike ? 0x74Eu:0x824u)) && before[n]!=after[n]) return false; return true;
    }
};
inline bool Run(CPlayerPed* player,int& pair,double& maximum) {
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if(c.Name()=="CPed") for(auto& h:c.Items()) if(h->Name().find("MakeTyresMuddySectorList")!=std::string_view::npos) hook=h.get(); });
    if(!hook) return false;
    const bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    auto* mi=CModelInfo::GetModelInfo(400); auto* savedCol=mi->m_pColModel;
    CColModel col; col.m_boundBox.m_vecMin=CVector{-1.f,-2.f,-.5f}; col.m_boundBox.m_vecMax=CVector{1.f,2.f,1.f}; mi->m_pColModel=&col;
    ReversingProbe::EntryPatch audio(0x4F6420,Audio),playerPatch(0x56E0D0,Player),shake(0x53F920,Shake),blood(0x56D230,Foot::Blood);
    auto entry=reinterpret_cast<void(__thiscall*)(CPed*,CPtrListSingleLink<CPhysical*>&)>(0x6AE0D0); bool passed=true;
    for(int i=0;i<2048 && passed;++i) {
        PedInput a(player,i),b(player,i);
        for(auto* p:{a.ped.get(),b.ped.get()}) {
            p->GetPosition()=CVector{std::array{0.f,1.f,1.999f,2.f,9.999f,10.f}[(i/11)%6],std::array{0.f,1.6f,2.f,-2.f}[(i/5)%4],std::array{0.f,1.999f,2.f,-2.f}[(i/17)%4]};
            p->m_fMass=std::array{50.f,1000.f,2000.f}[(i/19)%3]; p->m_fTurnMass=std::array{70.f,2000.f,3000.f}[(i/31)%3];
        }
        std::array<VehicleInput,3> va{VehicleInput(i,0),VehicleInput(i,1),VehicleInput(i,2)},vb{VehicleInput(i,0),VehicleInput(i,1),VehicleInput(i,2)};
        const auto paBefore=a.ped.bytes,pbBefore=b.ped.bytes;
        std::array<std::vector<uint8>,3> savedA,savedB;
        for(int n=0;n<3;++n) { savedA[n]=va[n].Snapshot(); savedB[n]=vb[n].Snapshot(); }
        Foot::blood=(i/2)%2;
        auto run=[&](PedInput& ped,auto& cars) {
            for(int n=0;n<3;++n) vehicles[n]=cars[n].get(); playerVehicle=(i%3)==0 ? vehicles[(i/3)%2]:nullptr;
            struct Node { CPhysical* Item{}; Node* Next{}; }; std::array<Node,4> nodes;
            const std::array<int,4> order=(i&16) ? std::array{1,2,0,1}:std::array{0,2,1,0};
            for(int n=0;n<4;++n) { nodes[n].Item=vehicles[order[n]]; nodes[n].Next=n<3 ? &nodes[n+1]:nullptr; }
            CPtrListSingleLink<CPhysical*> list; auto* first=(i%37)==0 ? nullptr:nodes.data(); std::memcpy(&list,&first,4);
            Foot::discrete=json::array(); Foot::numbers=json::array(); entry(ped.ped.get(),list); first=nullptr; std::memcpy(&list,&first,4);
            for(auto& c:cars) c.State(); return json{{"exact",Foot::discrete},{"numbers",Foot::numbers}};
        };
        hook->State(false); const auto start=PedCompletionProbe::calls[3]; const auto original=run(a,va); const auto middle=PedCompletionProbe::calls[3];
        hook->State(true); const auto candidate=run(b,vb); const auto end=PedCompletionProbe::calls[3];
        bool bounded=a.ped.bytes==paBefore && b.ped.bytes==pbBefore;
        for(int n=0;n<3;++n) bounded &= va[n].Bounded(savedA[n]) && vb[n].Bounded(savedB[n]);
        passed=Equal(original,candidate,maximum)&&bounded&&middle==start&&end==middle+1;
        Write({{"event","comparison"},{"function","MakeTyresMuddySectorList"},{"pair",pair++},{"input",i},{"original",original},{"candidate",candidate},{"equal",passed},{"bounded",bounded},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
    }
    mi->m_pColModel=savedCol; hook->State(enabled); hook->LockState(locked); return passed;
}
}

