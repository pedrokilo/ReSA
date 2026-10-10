#pragma once
// Incluido dentro de HookUnlockAudioLive. La fase real no instala stubs.
inline int liveKind=-1; // LiveVehicles.hpp posee liveVehicle.
inline bool pairsFinished{}, liveStarted{};
inline int liveFrame{}, waitedFrames{};
inline std::array<ReversibleHooks::ReversibleHook::Base*,3> liveHooks{};
inline std::array<bool,3> liveEnabled{},liveLocked{};
inline std::array<uintptr_t,3> liveDll{};
inline std::array<uintptr_t,3> liveExe{0x4FBB10,0x501E10,0x502280};
inline std::array<std::vector<uint8>,3> liveExeBytes{},liveDllBytes{};
inline std::array<std::array<int,3>,32> liveCalls{};
inline std::array<int,32> liveEngineBankLoaded{};
struct LiveInputs {
    CVehicle* vehicle;
    CPad* pad;
    CControllerState state;
    int16 mode;
    uint16 disabled;
    uint8 gear;
    float gas,brake;
    CVector speed;
    bool handbrake;
    bool restored{};
    LiveInputs(CVehicle* v,CPad* p):vehicle(v),pad(p),mode(p->Mode),disabled(p->DisablePlayerControls),
        gear(v->m_nCurrentGear),gas(v->m_GasPedal),brake(v->m_BrakePedal),speed(v->m_vecMoveSpeed),handbrake(v->vehicleFlags.bIsHandbrakeOn) {
        memcpy(&state,&pad->NewState,sizeof(state));
    }
    bool Restore() {
        memcpy(&pad->NewState,&state,sizeof(state));pad->Mode=mode;pad->DisablePlayerControls=disabled;
        vehicle->m_nCurrentGear=gear;vehicle->m_GasPedal=gas;vehicle->m_BrakePedal=brake;
        vehicle->m_vecMoveSpeed=speed;vehicle->vehicleFlags.bIsHandbrakeOn=handbrake;
        const bool ok=memcmp(&pad->NewState,&state,sizeof(state))==0 && pad->Mode==mode && pad->DisablePlayerControls==disabled &&
            vehicle->m_nCurrentGear==gear && vehicle->m_GasPedal==gas && vehicle->m_BrakePedal==brake &&
            memcmp(&vehicle->m_vecMoveSpeed,&speed,sizeof(speed))==0 && vehicle->vehicleFlags.bIsHandbrakeOn==handbrake;
        restored=true;
        return ok;
    }
    ~LiveInputs(){if(!restored)Restore();}
};
inline json EngineChannels(const CAEVehicleAudioEntity& audio) {
    json result=json::array();
    for(size_t i=0;i<audio.m_EngineSounds.size();++i)
        result.push_back({{"channel",i},{"type",audio.m_EngineSounds[i].EngineSoundType},{"sound",uintptr_t(audio.m_EngineSounds[i].Sound)}});
    return result;
}
inline int LiveCall(uintptr_t target,void* self,void* arg,bool hasArg) {
    uint32 before,after;
    if(hasArg) {
        __asm {
            mov before,esp
            mov ecx,self
            push arg
            call target
            mov after,esp
        }
    } else {
        __asm {
            mov before,esp
            mov ecx,self
            call target
            mov after,esp
        }
    }
    return int(after-before);
}
inline bool RestoreLiveHooks() {
    bool restored=true;
    for(int i=0;i<3;++i) {
        liveHooks[i]->LockState(false);liveHooks[i]->State(liveEnabled[i]);liveHooks[i]->LockState(liveLocked[i]);
        restored &= liveHooks[i]->Hooked()==liveEnabled[i] && liveHooks[i]->Locked()==liveLocked[i];
        restored &= liveExeBytes[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(liveExe[i]),5);
        restored &= liveDllBytes[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(liveDll[i]),5);
    }
    return restored;
}
inline void Update() {
    if(!pairsFinished) {
        PairUpdate();
        if(!done)return;
        pairsFinished=true;done=false;
    }
    if(done)return;
    const int kind=liveFrame/240, mask=(liveFrame/30)%8, localFrame=liveFrame%30;
    if(kind!=liveKind) {
        const bool setup=kind==3 ? SetupLiveTrain() : SetupLiveVehicle(kind);
        if(!setup) {
            const bool restored=!liveStarted || RestoreLiveHooks();
            CleanupLiveVehicle();done=true;Write({{"event","error"},{"reason","live vehicle setup unavailable"},{"vehicle_kind",kind},{"state_restored",restored}});return;
        }
        liveKind=kind;
    }
    auto* vehicle=liveVehicle;
    auto* player=FindPlayerPed();
    if(!vehicle || !player || !vehicle->m_vehicleAudio.m_IsInitialized || !vehicle->m_pHandlingData || !vehicle->m_matrix) {
        if(++waitedFrames%120==0)Write({{"event","waiting_for_live_entity"},{"frames",waitedFrames},{"vehicle_kind",kind}});
        if(waitedFrames>=3600) {
            const bool restored=!liveStarted || RestoreLiveHooks();
            CleanupLiveVehicle();done=true;Write({{"event","error"},{"reason","live entity not initialized"},{"state_restored",restored}});
        }
        return;
    }
    if(!liveStarted) {
        constexpr std::array<const char*,3> names{"ProcessPlayerVehicleEngine","ProcessVehicle","Service"};
        for(int i=0;i<3;++i) {
            liveHooks[i]=FindHook("CAEVehicleAudioEntity",names[i]);
            if(!liveHooks[i]){done=true;Write({{"event","error"},{"reason","live hook absent"}});return;}
            liveEnabled[i]=liveHooks[i]->Hooked();liveLocked[i]=liveHooks[i]->Locked();
            liveDll[i]=i==0 ? bit_cast<uintptr_t>(&CAEVehicleAudioEntity::ProcessPlayerVehicleEngine) :
                i==1 ? bit_cast<uintptr_t>(&CAEVehicleAudioEntity::ProcessVehicle) : bit_cast<uintptr_t>(&CAEVehicleAudioEntity::Service);
            liveExeBytes[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(liveExe[i]),5);
            liveDllBytes[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(liveDll[i]),5);
        }
        liveStarted=true;
    }
    const int sample=kind*8+mask;
    bool routeOk=true;
    for(int i=0;i<3;++i) {
        liveHooks[i]->LockState(false);liveHooks[i]->State((mask&(1<<i))!=0);
        routeOk &= ReversingProbe::JumpsToDll(liveExe[i])==((mask&(1<<i))!=0);
    }
    auto& audio=vehicle->m_vehicleAudio;
    bool transitionOk=true;
    if(kind<2 && localFrame==10) {
        player->GetIntelligence()->FlushImmediately(false);
        CTaskSimpleCarSetPedOut task{vehicle,TARGET_DOOR_DRIVER,false,true};
        transitionOk=task.ProcessPed(player) && !player->bInVehicle;
        audio.TurnOffRadioForVehicle();
    } else if(kind<2 && localFrame==20) {
        player->GetIntelligence()->FlushImmediately(false);
        CTaskSimpleCarSetPedInAsDriver task{vehicle,true,nullptr};
        transitionOk=task.ProcessPed(player) && vehicle->m_pDriver==player;
        vehicle->vehicleFlags.bEngineOn=true;
        audio.TurnOnRadioForVehicle();
    } else if(kind<2 && localFrame==0) audio.TurnOnRadioForVehicle();
    const bool playerDriver=kind<2 && vehicle->m_pDriver==player && player->bInVehicle;

    auto* const pad=player->GetPadFromPlayer();
    LiveInputs inputs(vehicle,pad);
    const int demandValues[]{0,128,255};
    const int demand=demandValues[localFrame%3];
    const bool braking=localFrame%7==3;
    if(kind<2) {
        pad->Mode=int16(liveFrame%4);pad->DisablePlayerControls=0;
        pad->NewState.ButtonCross=int16(demand);
        pad->NewState.ButtonSquare=int16(braking?128:0);
        // Mode3 comparte eje entre acelerador y freno; máximo físico -127 =>254.
        pad->NewState.RightStickY=int16(braking?64:-(demand/2));
        const auto& transmission=vehicle->m_pHandlingData->m_transmissionData;
        const int gears=std::clamp<int>(transmission.m_nNumberOfGears,1,6);
        vehicle->m_nCurrentGear=uint8(1+localFrame%gears);
        vehicle->m_GasPedal=float(pad->GetAccelerate())/255.f;
        vehicle->m_BrakePedal=float(pad->GetBrake())/255.f;
        vehicle->vehicleFlags.bIsHandbrakeOn=localFrame%11==4;
        const float ratios[]{0.f,.15f,.5f};
        const float testedSpeed=transmission.m_MaxVelocity*ratios[localFrame%3];
        vehicle->m_vecMoveSpeed=vehicle->m_matrix->GetForward()*testedSpeed;
    }
    const int actualAccelerate=pad->GetAccelerate(),actualBrake=pad->GetBrake(),testedGear=vehicle->m_nCurrentGear,testedMode=pad->Mode;
    const json channelsBefore=EngineChannels(audio);

    CAEVehicleAudioEntity::tVehicleParams vp{};
    vp.Vehicle=vehicle;vp.ModelIndexMinusOffset=vehicle->GetModelIndex()-400;
    vp.BaseVehicleType=vehicle->m_nVehicleType;vp.SpecificVehicleType=vehicle->m_nVehicleSubType;
    vp.Transmission=&vehicle->m_pHandlingData->m_transmissionData;
    if(vehicle->GetStatus()==STATUS_SIMPLE)vp.Speed=vehicle->m_autoPilot.m_speed*std::bit_cast<float>(0x3CA3D70Au);
    else {
        const auto& f=vehicle->m_matrix->GetForward();const auto& v=vehicle->m_vecMoveSpeed;
        const float z=v.z*f.z,y=v.y*f.y,zy=z+y,x=v.x*f.x;vp.Speed=zy+x;
    }
    const bool loaded=playerDriver && audio.m_DummySlot>=0 && AEAudioHardware.IsSoundBankLoaded(audio.m_DummyEngineBank,audio.m_DummySlot) &&
        AEAudioHardware.IsSoundBankLoaded(audio.m_PlayerEngineBank,SND_BANK_SLOT_PLAYER_ENGINE_P);
    liveEngineBankLoaded[sample]+=loaded && playerDriver;
    json calls=json::array();bool stackOk=true;
    for(int i=0;i<3;++i) {
        if(i==0 && !playerDriver)continue;
        const bool viaDll=(liveFrame&1)!=0;
        const auto before=Fnv32(&audio,sizeof(audio));
        const int delta=LiveCall(viaDll?liveDll[i]:liveExe[i],&audio,i==0?static_cast<void*>(&vp):static_cast<void*>(vehicle),i!=2);
        stackOk &= delta==0;++liveCalls[sample][i];
        calls.push_back({{"function",i},{"entry",viaDll?"dll":"exe"},{"esp_delta",delta},{"before_hash",before},{"after_hash",Fnv32(&audio,sizeof(audio))},{"engine_state",int(audio.m_State)},{"channels",EngineChannels(audio)}});
    }
    const json channelsAfter=EngineChannels(audio);
    const bool inputsRestored=inputs.Restore();
    Write({{"event","live_audio_execution"},{"frame",liveFrame},{"mask",mask},{"vehicle_kind",kind},{"local_frame",localFrame},{"transition_ok",transitionOk},{"route_ok",routeOk},{"stack_ok",stackOk},{"inputs_restored",inputsRestored},{"pad_mode",testedMode},{"accelerate_requested",demand},{"accelerate_actual",actualAccelerate},{"brake_actual",actualBrake},{"tested_gear",testedGear},{"channels_before",channelsBefore},{"channels_after",channelsAfter},{"dummy_bank",int(audio.m_DummyEngineBank)},{"dummy_slot",int(audio.m_DummySlot)},{"player_bank",int(audio.m_PlayerEngineBank)},{"bank_loaded",loaded},{"player_driver",audio.m_IsPlayerDriver},{"calls",calls}});
    if(++liveFrame==960 || !routeOk || !stackOk || !transitionOk || !inputsRestored) {
        const bool restored=RestoreLiveHooks();
        bool coverage=true;
        for(int k=0;k<4;++k)for(int m=0;m<8;++m) {
            const int s=k*8+m;
            if(k<2)coverage &= liveEngineBankLoaded[s]>0 && liveCalls[s][0]==20;
            else coverage &= liveCalls[s][0]==0;
            coverage &= liveCalls[s][1]==30 && liveCalls[s][2]==30;
        }
        const bool cleaned=CleanupLiveVehicle();done=true;Write({{"event",routeOk&&stackOk&&transitionOk&&inputsRestored&&coverage&&restored&&cleaned?"complete":"error"},{"live_frames",liveFrame},{"coverage",coverage},{"live_calls",liveCalls},{"loaded_frames",liveEngineBankLoaded},{"fixture_cleanup",cleaned},{"session_disposable",true},{"state_restored",restored&&cleaned}});
    }
}
