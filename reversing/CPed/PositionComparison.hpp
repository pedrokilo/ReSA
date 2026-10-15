// Included inside namespace PedCompletion.
namespace Position {
inline CColModel* col{};
inline CVehicle* vehicle{};
inline int input{},checks{};
inline CColModel* __fastcall Col(CEntity*,void*) { Foot::discrete.push_back({"col"}); return col; }
inline bool __fastcall Side(CVehicle*,void*) { Foot::discrete.push_back({"side"}); return input%5==0; }
inline bool __fastcall Collision(CPed* p,void*) {
    Foot::discrete.push_back({"collision",checks,bool(p->m_bUsesCollision),bool(p->physicalFlags.bForceHitReturnFalse),CWorld::pIgnoreEntity==vehicle}); Foot::Vec(p->GetPosition());
    const int target=(input/5)%14; return checks++!=target;
}
inline bool __cdecl Line(const CVector& a,const CVector& b,bool buildings,bool vehicles,bool peds,bool objects,bool dummies,bool see,bool camera) {
    Foot::discrete.push_back({"line",buildings,vehicles,peds,objects,dummies,see,camera}); Foot::Vec(a); Foot::Vec(b); return (input/70)%3!=0;
}
inline CVector* __cdecl Door(CVector* out,const CVehicle* v,int door) {
    Foot::discrete.push_back({"door",door}); *out=v->GetPosition()+CVector{float(door-9)*.5f,float(door%2),.25f}; return out;
}
inline CNodeAddress* __fastcall Closest(CPathFind*,void*,CNodeAddress* out,CVector pos,int type,float distance,int a,int b,int c,int d,int e) {
    Foot::discrete.push_back({"closest",type,a,b,c,d,e}); Foot::Vec(pos); Foot::numbers.push_back(distance);
    *out=((input/13) & (type==1 ? 1:2)) ? CNodeAddress{0,uint16(type)}:CNodeAddress{}; return out;
}
inline bool __cdecl Ground(CVector* pos) { Foot::discrete.push_back({"ground"}); Foot::Vec(*pos); pos->z=2.25f; return true; }
inline bool Run(CPlayerPed* player,int& pair,double& maximum) {
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if(c.Name()=="CPed") for(auto& h:c.Items()) if(h->Name()=="PositionPedOutOfCollision") hook=h.get(); });
    if(!hook) return false;
    const bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    CColModel model; model.m_boundBox.m_vecMin=CVector{-1.f,-2.f,-.5f}; model.m_boundBox.m_vecMax=CVector{1.25f,2.5f,1.5f}; col=&model;
    auto* savedIgnore=CWorld::pIgnoreEntity; auto* savedNodes=ThePaths.m_pPathNodes[0]; std::array<CPathNode,2> nodes{}; ThePaths.m_pPathNodes[0]=nodes.data();
    ReversingProbe::EntryPatch pc(0x535300,Col),ps(0x6D1DD0,Side),collision(0x54D920,Collision),line(0x56A490,Line),door(0x64E740,Door),doorDll(std::bit_cast<uintptr_t>(&CCarEnterExit::GetPositionToOpenCarDoor),Door),closest(0x44F460,Closest),ground(0x616920,Ground);
    auto entry=reinterpret_cast<bool(__thiscall*)(CPed*,int32,CVehicle*,bool)>(0x5E0820); bool passed=true;
    for(int i=0;i<4096 && passed;++i) {
        input=i; PedInput a(player,i),b(player,i); Tyres::VehicleInput va(i,0),vb(i,0);
        const int exit=std::array{0,8,9,10,11}[(i/3)%5]; const bool find=i%2;
        nodes[0].m_vPos=CVector{float((i/11)%7),-3.f,1.f}; nodes[1].m_vPos=CVector{-2.f,float((i/17)%7),1.f};
        for(auto* v:{va.get(),vb.get()}) { v->m_nVehicleType=std::array{VEHICLE_TYPE_AUTOMOBILE,VEHICLE_TYPE_BIKE,VEHICLE_TYPE_BOAT}[(i/7)%3]; v->m_vecTurnSpeed=CVector{.1f,.2f,.3f}; v->m_matrix->SetRotate(float(i%3)*.2f,float(i%4)*.1f,float(i%5)*.25f); }
        for(auto* p:{a.ped.get(),b.ped.get()}) { p->GetPosition()=CVector{.2f,.4f,1.f}; p->m_vecMoveSpeed=CVector{.2f,.3f,.4f}; p->m_vecTurnSpeed=CVector{.1f,.2f,.3f}; p->m_bUsesCollision=(i/2)%2; p->physicalFlags.bForceHitReturnFalse=(i/4)%2; p->bDonePositionOutOfCollision=i%37==0; }
        a.ped.get()->m_pVehicle=i%41 ? va.get():nullptr; b.ped.get()->m_pVehicle=i%41 ? vb.get():nullptr;
        const auto beforeA=a.ped.bytes,beforeB=b.ped.bytes; const auto carA=va.Snapshot(),carB=vb.Snapshot();
        auto run=[&](PedInput& ped,Tyres::VehicleInput& car) {
            vehicle=car.get(); checks=0; CWorld::pIgnoreEntity=nullptr; Foot::discrete=json::array(); Foot::numbers=json::array();
            auto* p=ped.ped.get(); const bool result=entry(p,exit,i%11 ? vehicle:nullptr,find);
            Foot::discrete.push_back({"result",result,bool(p->bDonePositionOutOfCollision),bool(p->m_bUsesCollision),bool(p->physicalFlags.bForceHitReturnFalse),CWorld::pIgnoreEntity==nullptr});
            Foot::Vec(p->GetPosition()); Foot::Vec(p->GetRight()); Foot::Vec(p->GetForward()); Foot::Vec(p->GetUp()); Foot::Vec(p->m_vecMoveSpeed); Foot::Vec(p->m_vecTurnSpeed); Foot::Vec(vehicle->m_vecMoveSpeed); Foot::Vec(vehicle->m_vecTurnSpeed);
            return json{{"exact",Foot::discrete},{"numbers",Foot::numbers}};
        };
        hook->State(false); const auto start=PedCompletionProbe::calls[4]; const auto original=run(a,va); const auto middle=PedCompletionProbe::calls[4];
        hook->State(true); const auto candidate=run(b,vb); const auto end=PedCompletionProbe::calls[4];
        const auto boundedPed=[](const auto& before,const auto& after) { for(size_t n=0;n<before.size();++n) if(!(n>=0x1C && n<0x20) && !(n>=0x40 && n<0x5C) && n!=0x474 && before[n]!=after[n]) return false; return true; };
        const bool bounded=boundedPed(beforeA,a.ped.bytes)&&boundedPed(beforeB,b.ped.bytes)&&va.Bounded(carA)&&vb.Bounded(carB);
        passed=Equal(original,candidate,maximum)&&bounded&&middle==start&&end==middle+1;
        Write({{"event","comparison"},{"function","PositionPedOutOfCollision"},{"pair",pair++},{"input",i},{"original",original},{"candidate",candidate},{"equal",passed},{"bounded",bounded},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
    }
    ThePaths.m_pPathNodes[0]=savedNodes; CWorld::pIgnoreEntity=savedIgnore; hook->State(enabled); hook->LockState(locked); return passed;
}
}
