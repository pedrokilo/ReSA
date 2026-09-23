#pragma once
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCar.h"
namespace PlayerInfoLive {
inline CVehicle* interactionCar{};
inline bool __fastcall ExitDown(const CPad*,void*) {trace.push_back({"exit_down"});return seed%3!=0;}
inline bool __fastcall ExitHeld(const CPad*,void*) {trace.push_back({"exit_held"});return seed%5!=0;}
inline CEntity* __fastcall ObjectToSteal(CPlayerInfo*,void*,CPed*) {trace.push_back({"object_search"});return nullptr;}
inline void __fastcall ClosestCar(CPlayerInfo*,void*,CPtrListDoubleLink<CVehicle*>&,CPed*,float x,float y,float xx,float yy,float* score,CVehicle** result) {
    trace.push_back({"car_search",x,y,xx,yy});if(seed%7){*score=5.0f;*result=interactionCar;}
}
inline CTaskSimpleHoldEntity* __fastcall Hold(CPedIntelligence*,void*,bool check) {trace.push_back({"hold",check});return nullptr;}
inline CTaskSimpleFight* __fastcall Fight(CPedIntelligence*,void*) {trace.push_back({"fight"});return nullptr;}
inline bool __fastcall EnterCar(CVehicle*,void*) {trace.push_back({"can_enter"});return seed%5!=0;}
inline bool __fastcall StepOut(const CVehicle*,void*,bool ignore) {trace.push_back({"step_out",ignore});return seed%3!=0;}
inline bool __fastcall JumpOut(CVehicle*,void*,CPed*) {trace.push_back({"jump_out"});return seed%7!=0;}
inline void __fastcall GiveParachute(const CPlayerInfo*,void*) {trace.push_back({"give_parachute"});}
inline bool __fastcall Simple(CTask*,void*) {return true;}
inline CTask* __fastcall SubTask(const CTask*,void*) {return nullptr;}
inline eTaskType __fastcall TaskType(const CTask*,void*) {return TASK_SIMPLE_STAND_STILL;}
inline bool __fastcall Abort(CTask*,void*,CPed*,eAbortPriority priority,const CEvent*) {trace.push_back({"abort",int(priority)});return seed%11!=0;}
inline void __fastcall AssignTask(CTaskManager*,void*,CTask* task,ePrimaryTasks slot,bool force) {
    if(!task){trace.push_back({"assign_null",int(slot),force});return;}
    const auto type=task->GetTaskType();json detail;
    if(type==TASK_COMPLEX_LEAVE_CAR){auto* t=static_cast<CTaskComplexLeaveCar*>(task);detail={t->m_pTargetVehicle==interactionCar,t->m_nTargetDoor,t->m_nDelayTime,t->m_bSensibleLeaveCar,t->m_bForceGetOut};}
    else detail={static_cast<CTaskComplexEnterCar*>(task)->GetTargetCar()==interactionCar};
    trace.push_back({"assign",int(type),int(slot),force,detail});delete task;
}
inline void __fastcall Hunger(CPlayerInfo*,void*) {trace.push_back({"hunger"});}
inline float __cdecl Fat(eStatModAbilities stat) {trace.push_back({"fat",int(stat)});return 75.0f+float(seed%150);}
inline float __cdecl Stat(eStats stat) {trace.push_back({"stat",int(stat)});return seed%2?450.0f:100.0f;}
inline void __cdecl Record(eStats stat,float value) {trace.push_back({"record",int(stat),value});}
inline void __cdecl Increment(eStats stat,float value) {trace.push_back({"increment",int(stat),value});}
inline void __fastcall Parachute(CPlayerInfo*,void*,bool on) {trace.push_back({"parachute",on});}
inline bool __cdecl Coop() {trace.push_back({"coop"});return false;}
inline void __cdecl Deletable(int32 model) {trace.push_back({"deletable",model});}
inline float __fastcall Density(CPathFind*,void*,float x,float y) {trace.push_back({"density",x,y});return float(seed%20)*0.2f;}
inline bool __fastcall Target(const CPad*,void*) {trace.push_back({"target"});return seed%2!=0;}
inline CTaskSimpleSwim* __fastcall Swim(CPedIntelligence*,void*) {trace.push_back({"swim"});return nullptr;}
inline CTaskSimpleJetPack* __fastcall Jet(CPedIntelligence*,void*) {trace.push_back({"jet"});return nullptr;}
inline eVehicleAppearance __fastcall Appearance(const CVehicle*,void*) {trace.push_back({"appearance"});return eVehicleAppearance(seed%6);}
inline void __cdecl Driving(CVehicle*) {trace.push_back({"driving"});}
inline void __cdecl Flying(CVehicle*) {trace.push_back({"flying"});}
inline void __cdecl Biking(CBike*) {trace.push_back({"biking"});}
inline bool __cdecl Mission() {trace.push_back({"mission"});return seed%11==0;}
inline bool __cdecl NoPolice() {trace.push_back({"no_police"});return seed%7==0;}
inline bool __cdecl Abandon() {trace.push_back({"abandon"});return seed%5==0;}
inline CNodeAddress __fastcall ClosestNode(CPathFind*,void*,CVector pos,ePathType type,float distance,uint16 a,int32 b,uint16 c,uint16 d,int32 e) {
    trace.push_back({"node",pos.x,pos.y,pos.z,int(type),distance,a,b,c,d,e});return seed%3?CNodeAddress(1,2):CNodeAddress();
}
inline CNodeAddress* __fastcall ClosestNodeOriginal(CPathFind* self,void*,CNodeAddress* out,CVector pos,ePathType type,float distance,uint16 a,int32 b,uint16 c,uint16 d,int32 e) {
    *out=ClosestNode(self,nullptr,pos,type,distance,a,b,c,d,e);return out;
}
inline void __fastcall Crosshair(uint32* state,void*,uint32 index,CPad* pad) {
    trace.push_back({"crosshair",uintptr_t(state)-uintptr_t(&CWorld::Players[0]),*state,index,pad==CPad::GetPad(index)});
    *state^=0x55;
}
inline bool ProcessTests(ReversibleHooks::ReversibleHook::Base* hook,int& pair) {
    auto* globals=reinterpret_cast<void*>(0xB7CD98);auto saved=ReversingProbe::Bytes(globals,800);
    auto timer=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB4C),0x3C);
    auto chase=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB9B994),16);
    auto chaseFlags=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8CDF20),2);
    auto focus=CWorld::PlayerInFocus;auto mode=CReplay::Mode;auto area=CGame::currArea;auto scan=CWorld::ms_nCurrentScanCode;
    bool passed=true;const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    {
        Patch hunger(&CPlayerInfo::WorkOutEnergyFromHunger,0x56E610,Hunger),fat(CStats::GetFatAndMuscleModifier,0x559AF0,Fat),stat(static_cast<float(*)(eStats)>(&CStats::GetStatValue),0x558E40,Stat);
        Patch record(CStats::SetNewRecordStat,0x55C410,Record),increment(CStats::IncrementStat,0x55C180,Increment);
        Patch parachute(&CPlayerInfo::StreamParachuteWeapon,0x56EB30,Parachute),coop(CGameLogic::IsCoopGameGoingOn,0x441390,Coop),deletable(CStreaming::SetModelIsDeletable,0x409C10,Deletable);
        Patch density(&CPathFind::CalcRoadDensity,0x44EFC0,Density),target(&CPad::GetTarget,0x540670,Target);
        Patch swim(&CPedIntelligence::GetTaskSwim,0x601070,Swim),jet(&CPedIntelligence::GetTaskJetPack,0x601110,Jet);
        Patch appearance(&CVehicle::GetVehicleAppearance,0x6D1080,Appearance),driving(CStats::UpdateStatsWhenDriving,0x55CAC0,Driving),flying(CStats::UpdateStatsWhenFlying,0x55CC00,Flying),biking(CStats::UpdateStatsWhenOnMotorBike,0x55CD60,Biking);
        Patch mission(CTheScripts::IsPlayerOnAMission,0x464D50,Mission),noPolice(CCullZones::NoPolice,0x72DD50,NoPolice),abandon(CCullZones::PoliceAbandonCars,0x72DD60,Abandon);
        ReversingProbe::EntryPatch nodeDll(Address(&CPathFind::FindNodeClosestToCoors),ClosestNode),nodeExe(0x44F460,ClosestNodeOriginal);
        ReversingProbe::EntryPatch crosshair(0x56EC80,Crosshair);
        Patch exitDown(&CPad::ExitVehicleJustDown,0x540120,ExitDown),exitHeld(&CPad::GetExitVehicle,0x5400D0,ExitHeld);
        Patch objectSearch(&CPlayerInfo::FindObjectToSteal,0x56DBD0,ObjectToSteal),carSearch(&CPlayerInfo::FindClosestCarSectorList,0x56F4E0,ClosestCar);
        Patch hold(&CPedIntelligence::GetTaskHold,0x600FF0,Hold),fight(&CPedIntelligence::GetTaskFighting,0x600F30,Fight),enter(&CVehicle::CanPedEnterCar,0x6D5400,EnterCar);
        Patch give(&CPlayerInfo::GivePlayerParachute,0x56EC40,GiveParachute),assign(&CTaskManager::SetTask,0x681AF0,AssignTask);
        Raw<CPlayerPed> ped;Raw<CAutomobile> car;Raw<CWanted> wanted;Raw<CPedIntelligence> intelligence;
        Raw<CTask> activeTask;
        std::array<uintptr_t,11> taskVtable{};taskVtable[2]=Address(SubTask);taskVtable[3]=Address(Simple);taskVtable[4]=Address(TaskType);taskVtable[6]=Address(Abort);
        std::array<uintptr_t,64> carVtable{};carVtable[0xE4/4]=Address(StepOut);carVtable[0xE8/4]=Address(JumpOut);
        CMatrix pedMatrix,carMatrix;
        for(int i=0;i<8704;++i) {
            seed=i;auto& info=CWorld::Players[0];
            const auto setup=[&] {
                trace=json::array();std::memset(globals,0,800);ped.data.fill(0);car.data.fill(0);wanted.data.fill(0);intelligence.data.fill(0);
                CWorld::PlayerInFocus=0;CWorld::ms_nCurrentScanCode=100;CReplay::Mode=(i%67==0)?MODE_PLAYBACK:MODE_RECORD;
                CGame::currArea=eAreaCodes(i%3);
                CTimer::SetFrameCounter((i%4==0)?32:uint32(i|1));CTimer::SetTimeStep(0.25f*float(i%12+1));
                CTimer::SetTimeInMS(20000+uint32(i%5));CTimer::SetPreviousTimeInMS((i&8)?19999:20000);
                std::memset(reinterpret_cast<void*>(0xB9B994),0,16);*reinterpret_cast<uint16*>(0x8CDF20)=uint16(i%4==0?0x101:0);
                pedMatrix.SetUnity();carMatrix.SetUnity();pedMatrix.GetPosition()=CVector{float(i%13),float(i%17),3.0f};carMatrix.GetPosition()=CVector{float(i%7),float(i%11),4.0f};carMatrix.GetUp().z=float(i%5)*0.5f-1.0f;
                SetMatrix(ped.get(),&pedMatrix);SetMatrix(car.get(),&carMatrix);SetIntelligence(ped.get(),intelligence.get());
                info.m_pPed=ped.get();info.m_PlayerData.m_pWanted=wanted.get();*reinterpret_cast<CPlayerPedData**>(ped.data.data()+0x480)=&info.m_PlayerData;
                *reinterpret_cast<uint32*>(wanted.data.data()+0x2C)=uint32(i%7);
                ped.get()->bInVehicle=i%4!=0;ped.get()->bCanExitCar=false;ped.get()->m_pVehicle=car.get();ped.get()->m_fMovingSpeed=float(i%3)*0.125f;
                car.get()->SetType(ENTITY_TYPE_VEHICLE);car.get()->m_nVehicleType=eVehicleType(i%5==0?5:i%3==0?9:0);car.get()->m_nVehicleSubType=eVehicleType(i%13==0?10:car.get()->m_nVehicleType);
                car.get()->m_nModelIndex=i%7==0?MODEL_CADDY:i%2?MODEL_TAXI:MODEL_CABBIE;
                car.get()->m_pDriver=(i%7)?ped.get():nullptr;car.get()->m_nNumPassengers=i%3;
                car.get()->m_fMovingSpeed=float(i%10)*0.125f;car.get()->m_vecMoveSpeed=CVector{float(i%4)*0.02f,0.0f,0.0f};
                car.get()->m_fDamageIntensity=(i&16)?1.0f:0.0f;car.get()->m_fHealth=1000.0f;car.get()->vehicleFlags.bCanBeDamaged=i%2!=0;car.get()->physicalFlags.bSubmergedInWater=i%7==0;
                car.get()->m_nNumContactWheels=i%5;
                for(int n=0;n<4;++n) {
                    float ratio=(i&(1<<n))?1.0f:0.5f;
                    *reinterpret_cast<float*>(car.data.data()+0x7E4+n*4)=ratio;
                    *reinterpret_cast<float*>(car.data.data()+0x720+n*4)=ratio;
                }
                info.m_bTaxiTimerScore=(i&4)!=0;info.m_nTaxiTimer=20000-uint32(i%5)*999;
                const std::array<int32,10> money{0,1,-1,50,51,1000,1001,10001,100001,1000000100};
                info.m_nMoney=money[i%10];info.m_nDisplayMoney=(i&32)?money[(i+3)%10]:0;
                info.m_nBustedAudioStatus=7;info.m_nCrosshairActivated=i%2;
                info.m_bParachuteReferenced=i%3==0;info.m_nRequireParachuteTimer=123;
                info.m_fRoadDensityAroundPlayer=float(i%20)*0.2f;info.m_fCurrentChaseValue=float(i%8)*200.0f;
                info.m_nTimesUpsideDownInARow=i%9;
                const std::array<uint32,8> counters{0,1,1999,2000,4999,5000,8000,12000};
                info.m_nCarTwoWheelCounter=counters[(i/16)%8];info.m_nBikeRearWheelCounter=counters[(i/32)%8];info.m_nBikeFrontWheelCounter=(i&512)?0:counters[(i/64)%8];
                info.m_nCarLess3WheelCounter=123;info.m_nTempBufferCounter=uint32(i%4)*250;
                info.m_fCarTwoWheelDist=3.0f;info.m_fBikeRearWheelDist=4.0f;info.m_fBikeFrontWheelDist=5.0f;
                // RC cleanup without invoking camera/streaming; full audiovisual branch is outside this bank.
                info.m_nPlayerState=PLAYERSTATE_HAS_DIED;info.m_bAfterRemoteVehicleExplosion=(i&128)!=0;info.m_nTimeOfRemoteVehicleExplosion=18000-(i%3);info.m_pRemoteVehicle=(i&256)?car.get():nullptr;
                if(i>=8192) {
                    if(i&16)pedMatrix.GetPosition().x=(i&32)?-3005.0f:3005.0f;
                    interactionCar=car.get();*reinterpret_cast<uintptr_t**>(car.get())=carVtable.data();
                    activeTask.data.fill(0);*reinterpret_cast<uintptr_t**>(activeTask.get())=taskVtable.data();
                    intelligence.get()->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_DEFAULT]=activeTask.get();
                    ped.get()->bCanExitCar=true;ped.get()->bUsingMobilePhone=i%17==0;
                    ped.get()->bInVehicle=(i&4)!=0;
                    info.m_bTryingToExitCar=(i&8)!=0;
                    info.m_pRemoteVehicle=(i%29==0)?car.get():nullptr;
                    car.get()->SetStatus(eEntityStatus((i/2)%7));car.get()->m_nDoorLock=eCarLock(i%13==0?4:0);
                }
            };
            auto run=[&] {
                setup();reinterpret_cast<void(__thiscall*)(CPlayerInfo*,uint32)>(0x56F8D0)(&info,0);
                auto bytes=ReversingProbe::Bytes(&info,sizeof(info));json floats=json::array();
                for(auto offset:{0xFC,0x108,0x110,0x11C,0x124,0x12C,0x134,0x148}){float value;std::memcpy(&value,bytes.data()+offset,4);floats.push_back(value);std::memset(bytes.data()+offset,0,4);}
                return json{{"info",bytes},{"floats",floats},{"ped",ped.data},{"car",car.data},{"wanted",wanted.data},{"intelligence",intelligence.data},{"trace",trace},{"chase",ReversingProbe::Bytes(reinterpret_cast<void*>(0xB9B994),16)},{"flags",ReversingProbe::Bytes(reinterpret_cast<void*>(0x8CDF20),2)}};
            };
            hook->State(false);auto start=PlayerInfoProbe::calls;auto original=run();auto middle=PlayerInfoProbe::calls;
            hook->State(true);auto candidate=run();auto end=PlayerInfoProbe::calls;
            const bool equal=Equal(original,candidate)&&start==middle&&end[2]==middle[2]+1;
            passed=passed&&equal;
            Write({{"event","comparison"},{"pair",pair++},{"function",2},{"case",i},{"original",original},{"candidate",candidate},{"equal",equal},{"original_candidate_calls",middle[2]-start[2]},{"candidate_calls",end[2]-middle[2]}});
        }
    }
    std::memcpy(globals,saved.data(),saved.size());std::memcpy(reinterpret_cast<void*>(0xB7CB4C),timer.data(),timer.size());
    std::memcpy(reinterpret_cast<void*>(0xB9B994),chase.data(),chase.size());std::memcpy(reinterpret_cast<void*>(0x8CDF20),chaseFlags.data(),2);
    CWorld::PlayerInFocus=focus;CWorld::ms_nCurrentScanCode=scan;CReplay::Mode=mode;CGame::currArea=area;hook->State(enabled);hook->LockState(locked);
    return passed && ReversingProbe::Bytes(globals,800)==saved && ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB4C),0x3C)==timer
        && ReversingProbe::Bytes(reinterpret_cast<void*>(0xB9B994),16)==chase && ReversingProbe::Bytes(reinterpret_cast<void*>(0x8CDF20),2)==chaseFlags
        && CWorld::PlayerInFocus==focus && CWorld::ms_nCurrentScanCode==scan && CReplay::Mode==mode && CGame::currArea==area;
}
}
