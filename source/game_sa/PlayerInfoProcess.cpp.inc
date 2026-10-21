// Implementation helpers for CPlayerInfo::Process (0x56F8D0).
// Included by PlayerInfo.cpp so the hook and its helpers share one translation unit.
namespace {
void ProcessPlayerStunts(CPlayerInfo& p) {
    auto* vehicle = p.m_pPed->bInVehicle ? p.m_pPed->m_pVehicle : nullptr;
    const auto elapsed = uint32(CTimer::GetTimeStep() * 0.02f * 1000.0f);
    const auto finish = [&](uint32& counter, float& distance, uint32& bestTime, float& bestDistance, uint32 threshold, int timeStat, int distanceStat) {
        if (counter >= threshold) {
            bestDistance = distance;
            bestTime = counter;
            CStats::SetNewRecordStat(eStats(timeStat), float(counter / 1000));
            CStats::SetNewRecordStat(eStats(distanceStat), distance);
        }
        counter = 0;
        distance = 0.0f;
        p.m_nTempBufferCounter = 0;
    };
    const auto advance = [&](uint32& counter, float& distance, float decay) {
        counter += elapsed;
        distance += vehicle->m_fMovingSpeed;
        p.m_nTempBufferCounter = uint32(std::max(0.0f, float(p.m_nTempBufferCounter) - float(elapsed) * decay));
    };
    if (vehicle && vehicle->IsAutomobile()) {
        auto* car = vehicle->AsAutomobile();
        if (car->m_nNumContactWheels < 3) p.m_nCarLess3WheelCounter += elapsed;
        else p.m_nCarLess3WheelCounter = 0;
        const auto& s = car->m_fWheelsSuspensionCompressionPrev;
        const bool rightAir = s[2] == 1.0f && s[3] == 1.0f;
        const bool leftAir = s[0] == 1.0f && s[1] == 1.0f;
        const bool active = (rightAir ? s[0] < 1.0f && s[1] < 1.0f : leftAir && s[2] < 1.0f && s[3] < 1.0f) && car->m_fDamageIntensity == 0.0f;
        if (active) advance(p.m_nCarTwoWheelCounter, p.m_fCarTwoWheelDist, 0.5f);
        else if ((rightAir || leftAir) && p.m_nCarTwoWheelCounter && p.m_nTempBufferCounter < 500) p.m_nTempBufferCounter += elapsed;
        else if (rightAir || leftAir || p.m_nCarTwoWheelCounter) finish(p.m_nCarTwoWheelCounter, p.m_fCarTwoWheelDist, p.m_nBestCarTwoWheelsTimeMs, p.m_fBestCarTwoWheelsDistM, 2000, 19, 12);
        p.m_nBikeRearWheelCounter = p.m_nBikeFrontWheelCounter = 0;
    } else if (vehicle && vehicle->IsBike()) {
        const auto& s = vehicle->AsBike()->m_aRatioHistory;
        const bool frontAir = s[0] == 1.0f && s[1] == 1.0f;
        const auto finishRear = [&] { finish(p.m_nBikeRearWheelCounter, p.m_fBikeRearWheelDist, p.m_nBestBikeWheelieTimeMs, p.m_fBestBikeWheelieDistM, 5000, 17, 10); };
        const auto finishFront = [&] { finish(p.m_nBikeFrontWheelCounter, p.m_fBikeFrontWheelDist, p.m_nBestBikeStoppieTimeMs, p.m_fBestBikeStoppieDistM, 2000, 18, 11); };
        if (frontAir && !p.m_nBikeFrontWheelCounter) {
            if (s[2] < 1.0f || (s[3] < 1.0f && vehicle->m_fDamageIntensity == 0.0f)) advance(p.m_nBikeRearWheelCounter, p.m_fBikeRearWheelDist, 0.2f);
            else if (p.m_nBikeRearWheelCounter && p.m_nTempBufferCounter < 500) p.m_nTempBufferCounter += elapsed;
            else finishRear();
            p.m_nBikeFrontWheelCounter = 0;
        } else if (p.m_nBikeRearWheelCounter) {
            finishRear();
        } else {
            if (s[2] == 1.0f && s[3] == 1.0f) {
                if (s[0] < 1.0f || (s[1] < 1.0f && vehicle->m_fDamageIntensity == 0.0f)) advance(p.m_nBikeFrontWheelCounter, p.m_fBikeFrontWheelDist, 0.2f);
                else if (p.m_nBikeFrontWheelCounter && p.m_nTempBufferCounter < 500) p.m_nTempBufferCounter += elapsed;
                else finishFront();
            } else finishFront();
            p.m_nBikeRearWheelCounter = 0;
        }
        p.m_nCarTwoWheelCounter = p.m_nCarLess3WheelCounter = 0;
    } else {
        p.m_nCarLess3WheelCounter = p.m_nTempBufferCounter = p.m_nCarTwoWheelCounter = p.m_nBikeRearWheelCounter = p.m_nBikeFrontWheelCounter = 0;
    }
}

void ProcessPlayerVehicleInteraction(CPlayerInfo& p, uint32 playerIndex, CPad* pad) {
    auto* ped = p.m_pPed;
    if (pad->GetTarget() || !ped->bCanExitCar || ped->bUsingMobilePhone) { p.m_bTryingToExitCar = false; return; }
    if (!pad->ExitVehicleJustDown() && !(p.m_bTryingToExitCar && pad->GetExitVehicle() && ped->bInVehicle)) {
        p.m_bTryingToExitCar = false;
        return;
    }
    p.m_bTryingToExitCar = false;
    auto& intelligence = *ped->GetIntelligence();
    auto& tasks = intelligence.m_TaskMgr;
    const auto abort = [&] { auto* task = tasks.GetActiveTask(); return !task || task->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr); };
    if (ped->bInVehicle) {
        auto* car = ped->m_pVehicle;
        if (p.m_pRemoteVehicle || (car->m_pEntityWeAreOn && CBridge::ThisIsABridgeObjectMovingUp(car->m_pEntityWeAreOn->m_nModelIndex)) || car->GetStatus() == STATUS_WRECKED || car->GetStatus() == STATUS_TRAIN_MOVING || car->m_nDoorLock == eCarLock(4)) return;
        if (!abort()) { p.m_bTryingToExitCar = true; return; }
        if (car->IsBoat()) {
            tasks.SetTask(new CTaskComplexLeaveCar(car, 0, 0, true, false), TASK_PRIMARY_PRIMARY, false);
            ped->bTryingToReachDryLand = true;
        } else if (car->CanPedStepOutCar(false) || car->CanPedJumpOutCar(ped)) {
            tasks.SetTask(new CTaskComplexLeaveCar(car, 0, 0, true, false), TASK_PRIMARY_PRIMARY, false);
            p.m_bTryingToExitCar = true;
            p.GivePlayerParachute();
        } else if (car->GetStatus() == STATUS_PLAYER) p.m_bTryingToExitCar = true;
        return;
    }
    if (ped->m_pAttachedTo) return;
    auto* object = p.FindObjectToSteal(ped);
    auto* simplest = tasks.GetSimplestActiveTask();
    auto* hold = intelligence.GetTaskHold(false);
    if (simplest->GetTaskType() == TASK_SIMPLE_CLIMB || intelligence.GetTaskFighting() || (hold && hold->m_pEntityToHold)) return;
    if (object) {
        if (!abort()) return;
        // The EXE checks the null of CTask::operator new (0x61A5A0) before constructing.
        void* const memory = CTask::operator new(sizeof(CTaskComplexGoPickUpEntity));
        CTask* const task = memory ? ::new (memory) CTaskComplexGoPickUpEntity(object, 81) : nullptr;
        CEventScriptCommand event(TASK_PRIMARY_PRIMARY, task, false);
        FindPlayerPed()->GetEventGroup().Add(&event, false);
        return;
    }
    const auto pos = ped->GetPosition();
    const auto minX = pos.x - 10.0f, minY = pos.y - 10.0f, maxX = pos.x + 10.0f, maxY = pos.y + 10.0f;
    CWorld::AdvanceCurrentScanCode();
    float best = 0.0f;
    CVehicle* car = nullptr;
    for (auto y = CWorld::GetSectorY(minY); y <= CWorld::GetSectorY(maxY); ++y) {
        for (auto x = CWorld::GetSectorX(minX); x <= CWorld::GetSectorX(maxX); ++x) {
            p.FindClosestCarSectorList(CWorld::GetRepeatSector(x,y).Vehicles, ped, minX, minY, maxX, maxY, &best, &car);
        }
    }
    if (car && car->IsSubTrailer() && car->m_pTowingVehicle) car = car->m_pTowingVehicle;
    if (!car || !car->CanPedEnterCar()) return;
    if (intelligence.GetTaskJetPack()) intelligence.GetTaskJetPack()->DropJetPack(ped);
    if (car->IsBoat()) {
        if (!car->m_pDriver && abort()) tasks.SetTask(new CTaskComplexEnterCarAsDriver(car), TASK_PRIMARY_PRIMARY, false);
        return;
    }
    if (auto* task = tasks.GetActiveTask()) {
        bool keepSwimming = false;
        if (task->GetTaskType() == TASK_COMPLEX_IN_WATER && intelligence.GetTaskSwim() && (car->m_nModelIndex == MODEL_SKIMMER || car->m_nModelIndex == MODEL_VORTEX || car->m_nModelIndex == MODEL_SEASPAR || car->m_nModelIndex == MODEL_LEVIATHN)) {
            CVector doorPos; int32 door = 0;
            if (CCarEnterExit::GetNearestCarDoor(ped, car, doorPos, door)) {
                auto* swim = intelligence.GetTaskSwim();
                swim->m_vecPos = doorPos;
                swim->m_nTimeStep = 5000;
                keepSwimming = true;
            }
        }
        if (!keepSwimming && !task->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) return;
    }
    if (CWorld::Players[1].m_pPed && !CGameLogic::bPlayersCanBeInSeparateCars) {
        auto* otherPed = CWorld::Players[(playerIndex + 1) % 2].m_pPed;
        auto& other = otherPed->GetTaskManager();
        auto* primary = other.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
        auto* defaultTask = other.m_aPrimaryTasks[TASK_PRIMARY_DEFAULT];
        const auto primaryType = primary ? primary->GetTaskType() : eTaskType(0);
        const auto defaultType = defaultTask ? defaultTask->GetTaskType() : eTaskType(0);
        CVehicle* driverCar = nullptr;
        CVehicle* passengerCar = nullptr;
        if (primaryType == TASK_COMPLEX_ENTER_CAR_AS_DRIVER || primaryType == TASK_COMPLEX_DRAG_PED_FROM_CAR) driverCar = static_cast<CTaskComplexEnterCar*>(primary)->GetTargetCar();
        else if (primaryType == TASK_COMPLEX_STEAL_CAR) driverCar = static_cast<CTaskComplexStealCar*>(primary)->m_veh;
        else if (primaryType == TASK_COMPLEX_ENTER_BOAT_AS_DRIVER) driverCar = static_cast<CTaskComplexEnterBoatAsDriver*>(primary)->GetTargetCar();
        if (primaryType == TASK_SIMPLE_PLAYER_IN_CAR || defaultType == TASK_SIMPLE_PLAYER_IN_CAR || primaryType == TASK_SIMPLE_CAR_DRIVE || defaultType == TASK_SIMPLE_CAR_DRIVE) {
            auto* otherCar = otherPed->m_pVehicle;
            if (otherCar->m_pDriver == otherPed) driverCar = otherCar;
            else passengerCar = otherCar;
        }
        if (primaryType == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER) passengerCar = static_cast<CTaskComplexEnterCar*>(primary)->GetTargetCar();
        if (driverCar == car) tasks.SetTask(new CTaskComplexEnterCarAsPassenger(car, 0, false), TASK_PRIMARY_PRIMARY, false);
        else if (passengerCar == car || (!driverCar && !passengerCar)) tasks.SetTask(new CTaskComplexEnterCarAsDriver(car), TASK_PRIMARY_PRIMARY, false);
    } else tasks.SetTask(new CTaskComplexEnterCarAsDriver(car), TASK_PRIMARY_PRIMARY, false);
}
}
