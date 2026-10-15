/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTrain
 * Reconstrucción: COMPLETA — 40/40 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 40/40 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Train.h"

#include "Buoyancy.h"
#include "CarCtrl.h"
#include "X87Math.h"



auto& pTrackNodes = StaticRef<CTrainNode*[4]>(0xC38024);
auto& NumTrackNodes = StaticRef<std::array<int32, 4>>(0xC38014);
auto& arrTotalTrackLength = StaticRef<std::array<float, 4>>(0xC37FEC);
auto& StationDist = StaticRef<std::array<float, 6>>(0xC38034);

void CTrain::InjectHooks() {
    RH_ScopedVirtualClass(CTrain, 0x872370, 66);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6F6030);
    RH_ScopedInstall(InitTrains, 0x6F7440);
    RH_ScopedInstall(ReadAndInterpretTrackFile, 0x6F55D0);
    RH_ScopedInstall(Shutdown, 0x6F58D0);
    RH_ScopedInstall(UpdateTrains, 0x6F5900);
    RH_ScopedInstall(FindCoorsFromPositionOnTrack, 0x6F59E0);
    RH_ScopedInstall(FindMaximumSpeedToStopAtStations, 0x6F5BA0);
    RH_ScopedInstall(FindNumCarriagesPulled, 0x6F5CD0);
    RH_ScopedInstall(OpenTrainDoor, 0x6F5D80);
    RH_ScopedInstall(AddPassenger, 0x6F5D90);
    RH_ScopedInstall(RemovePassenger, 0x6F5DA0);
    RH_ScopedInstall(DisableRandomTrains, 0x6F5DB0);
    RH_ScopedInstall(RemoveOneMissionTrain, 0x6F5DC0);
    RH_ScopedInstall(ReleaseOneMissionTrain, 0x6F5DF0);
    RH_ScopedInstall(SetTrainSpeed, 0x6F5E20);
    RH_ScopedInstall(SetTrainCruiseSpeed, 0x6F5E50);
    RH_ScopedInstall(FindCaboose, 0x6F5E70);
    RH_ScopedInstall(FindEngine, 0x6F5E90);
    RH_ScopedInstall(FindCarriage, 0x6F5EB0);
    RH_ScopedInstall(FindSideStationIsOn, 0x6F5EF0);
    RH_ScopedInstall(FindNextStationPositionInDirection, 0x6F5F00);
    RH_ScopedInstall(IsInTunnel, 0x6F6320);
    RH_ScopedInstall(RemoveRandomPassenger, 0x6F6850);
    RH_ScopedInstall(RemoveMissionTrains, 0x6F6A20);
    RH_ScopedInstall(RemoveAllTrains, 0x6F6AA0);
    RH_ScopedInstall(ReleaseMissionTrains, 0x6F6B60);
    RH_ScopedInstall(FindClosestTrackNode, 0x6F6BD0);
    RH_ScopedInstall(FindPositionOnTrackFromCoors, 0x6F6CC0);
    RH_ScopedInstall(FindNearestTrain, 0x6F7090);
    RH_ScopedInstall(SetNewTrainPosition, 0x6F7140);
    RH_ScopedInstall(IsNextStationAllowed, 0x6F7260);
    RH_ScopedInstall(SkipToNextAllowedStation, 0x6F72F0);
    RH_ScopedInstall(CreateMissionTrain, 0x6F7550);
    RH_ScopedInstall(DoTrainGenerationAndRemoval, 0x6F7900);
    RH_ScopedInstall(AddNearbyPedAsRandomPassenger, 0x6F8170);
    RH_ScopedVMTInstall(ProcessControl, 0x6F86A0);

    RH_ScopedGlobalInstall(ProcessTrainAnnouncements, 0x6F5910);
    RH_ScopedGlobalInstall(PlayAnnouncement, 0x6F5920);
    RH_ScopedGlobalInstall(MarkSurroundingEntitiesForCollisionWithTrain, 0x6F6640);
    RH_ScopedGlobalInstall(TrainHitStuff<CPtrListSingleLink<CPhysical*>>, 0x6F5CF0);
}

// 0x6F6030
CTrain::CTrain(int32 modelIndex, eVehicleCreatedBy createdBy) : CVehicle(createdBy) {
    std::memset(m_aDoors.data(), 0, sizeof(m_aDoors));
    m_nVehicleSubType = VEHICLE_TYPE_TRAIN;
    m_nVehicleType = VEHICLE_TYPE_TRAIN;
    auto* model = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();
    m_pHandlingData = gHandlingDataMgr.GetVehiclePointer(uint8(model->m_nHandlingId));
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    CVehicle::SetModelIndex(modelIndex);
    SetupModelNodes();
    auto* doors = reinterpret_cast<uint8*>(m_aDoors.data());
    *reinterpret_cast<float*>(doors + 0x30) = m_nModelIndex == MODEL_STREAKC ? 1.25f : std::bit_cast<float>(0xBFA0D97Cu);
    *reinterpret_cast<float*>(doors + 0x34) = m_nModelIndex == MODEL_STREAKC ? .25f : 0.f;
    *reinterpret_cast<uint16*>(doors + 0x38) = 0x14;
    doors[0x3A] = 2;
    *reinterpret_cast<float*>(doors + 0x48) = m_nModelIndex == MODEL_STREAKC ? 1.25f : std::bit_cast<float>(0x3FA0D97Cu);
    *reinterpret_cast<float*>(doors + 0x4C) = m_nModelIndex == MODEL_STREAKC ? .25f : 0.f;
    *reinterpret_cast<uint16*>(doors + 0x50) = 0x14;
    doors[0x52] = 2;
    m_nTrainFlags = uint16((m_nTrainFlags & 0xFAD9u) | 0x0281u);
    m_nPassengersGenerationState = 0;
    m_nNumPassengersToLeave = CGeneral::GetRandomNumber() & 3;
    m_nNumPassengersToEnter = (CGeneral::GetRandomNumber() & 3) + 1;
    m_pTemporaryPassenger = nullptr;
    m_nMaxPassengers = 5;
    physicalFlags.bDisableSimpleCollision = true;
    SetUsesCollision(true);
    m_nTimeWhenCreated = CTimer::GetTimeInMS();
    field_5C8 = 0;
    m_nTrackId = 0;
    m_fCurrentRailDistance = 0.f;
    m_fTrainSpeed = 0.f;
    m_nTimeWhenStoppedAtStation = 0;
    model->ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);
    m_fMass = m_pHandlingData->m_fMass;
    m_fTurnMass = m_pHandlingData->m_fTurnMass;
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    m_fElasticity = .05f;
    m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;
    const auto drag = m_pHandlingData->m_fDragMult;
    m_fAirResistance = drag > StaticRef<float>(0x858C58) ? (drag / StaticRef<float>(0x872364)) * StaticRef<float>(0x858B8C) : drag;
    physicalFlags.bCollidable = false;
    physicalFlags.bDisableCollisionForce = true;
    m_bTunnelTransition = true;
    m_pPrevCarriage = nullptr;
    m_pNextCarriage = nullptr;
    SetStatus(STATUS_TRAIN_MOVING);
    m_autoPilot.m_speed = 0.f;
    m_autoPilot.SetCruiseSpeed(0);
    m_vehicleAudio.Initialise(this);
}

void CTrain::SetupModelNodes() {
    std::ranges::fill(m_aTrainNodes, nullptr);
    CClumpModelInfo::FillFrameArray(GetRpClump(), m_aTrainNodes.data());
}

// 0x6F7440
void CTrain::InitTrains() {
    bDisableRandomTrains = false;
    GenTrain_Status = 0;
    constexpr const char* filenames[]{"data\\paths\\tracks.dat", "data\\paths\\tracks3.dat", "data\\paths\\tracks2.dat", "data\\paths\\tracks4.dat"};
    for (int32 track = 0; track < 4; ++track) {
        if (!pTrackNodes[track]) ReadAndInterpretTrackFile(filenames[track], &pTrackNodes[track], &NumTrackNodes[track], &arrTotalTrackLength[track], track);
    }
    const auto* nodes = pTrackNodes[0];
    for (int32 station = 0; station < 6; ++station) {
        int32 track;
        const auto index = FindClosestTrackNode(aStationCoors[station], &track);
        StationDist[station] = float(nodes[index].m_nDistanceFromStart) * StaticRef<float>(0x859040);
    }
}

// 0x6F55D0
void CTrain::ReadAndInterpretTrackFile(const char* filename, CTrainNode** nodes, int32* lineCount, float* totalDist, int32 skipStations) {
    if (!*nodes) {
        auto* buffer = new char[0xB530];
        CFileMgr::LoadFile(filename, reinterpret_cast<uint8*>(buffer), 0xB530, "rb");
        size_t cursor{};
        const auto header = [&]() {
            size_t length{};
            while (buffer[cursor] != '\n') gString[length++] = buffer[cursor++];
            gString[length] = 0;
            ++cursor;
        };
        header();
        if (std::memcmp(gString, "processed", 10) == 0) header();
        sscanf(gString, "%d", lineCount);
        *nodes = new CTrainNode[*lineCount];
        auto* station = aStationCoors;
        float x, y, z;
        int32 marked;
        for (int32 i = 0; i < *lineCount; ++i) {
            size_t length{};
            while (buffer[cursor] != '\n') gString[length++] = buffer[cursor++];
            ++cursor; // Como el original: no termina cada fila con cero.
            sscanf(gString, "%f %f %f %d", &x, &y, &z, &marked);
            (*nodes)[i].x = int16(int64(x * 8.f));
            (*nodes)[i].y = int16(int64(y * 8.f));
            (*nodes)[i].z = int16(int64(z * 8.f));
            if (!skipStations && marked) *station++ = CVector{x, y, z};
        }
        delete[] buffer;
    }
    auto distance = 0.f;
    for (int32 i = 0; i < *lineCount; ++i) {
        (*nodes)[i].m_nDistanceFromStart = uint16(int64(3.f * distance));
        const auto next = (i + 1) % *lineCount;
        const auto dy = float((*nodes)[i].y) * .125f - float((*nodes)[next].y) * .125f;
        const auto dx = float((*nodes)[i].x) * .125f - float((*nodes)[next].x) * .125f;
        distance += std::sqrt(dx * dx + dy * dy);
    }
    *totalDist = distance;
}

// 0x6F58D0
void CTrain::Shutdown() {
    // Como el original (0x6F58D0): delete[] (0x8213AE) sobre cada puntero no nulo y se pone a cero la entrada de la tabla global (0xC38024..0xC38034)
    for (auto& node : pTrackNodes) {
        if (node) {
            delete[] node;
        }
        node = nullptr;
    }
}

// 0x6F5900
void CTrain::UpdateTrains() {
    ZoneScoped;

    // NOP
}

// 0x6F5910
void ProcessTrainAnnouncements() {
    // NOP
}

// 0x6F5920
void PlayAnnouncement(uint8 arg0, uint8 arg1) {
    // NOP
}

// 0x6F59E0
void CTrain::FindCoorsFromPositionOnTrack(float railDistance, int32 trackId, CVector* outCoors) {
    const auto* nodes = pTrackNodes[trackId];
    const auto count = NumTrackNodes[trackId];
    for (int32 i = 0; i < count; ++i) {
        const auto next = (i + 1) % count;
        const auto before = railDistance - float(nodes[i].m_nDistanceFromStart) * StaticRef<float>(0x859040);
        const auto after = float(nodes[next].m_nDistanceFromStart) * StaticRef<float>(0x859040) - railDistance;
        if (before >= 0.f && after >= 0.f) {
            const auto inverse = 1.f / (after + before);
            outCoors->x = (float(nodes[i].x) * .125f * after + float(nodes[next].x) * .125f * before) * inverse;
            outCoors->y = (float(nodes[i].y) * .125f * after + float(nodes[next].y) * .125f * before) * inverse;
            outCoors->z = (float(nodes[i].z) * .125f * after + float(nodes[next].z) * .125f * before) * inverse;
            return;
        }
    }
}

// 0x6F5BA0
bool CTrain::FindMaximumSpeedToStopAtStations(float* speed) {
    *speed = 50.f;
    if (m_nTrackId != 0) {
        return false;
    }
    auto closest = 10000.f;
    const auto total = arrTotalTrackLength[0];
    for (auto station : StationDist) {
        auto delta = station - m_fCurrentRailDistance;
        delta = trainFlags.bClockwiseDirection ? delta + 40.f : delta - 40.f;
        while (delta > total * .5f) {
            delta -= total;
        }
        while (delta < total * -.5f) {
            delta += total;
        }
        if (trainFlags.bClockwiseDirection ? delta > 0.f : delta < 0.f) {
            if (!trainFlags.bClockwiseDirection) {
                delta = -delta;
            }
            if (delta < closest) {
                closest = delta;
            }
        }
    }
    *speed = closest < 500.f ? (1.f - (500.f - closest) * .002f) * 50.f : 100000.f;
    return closest < 5.f;
}

// 0x6F5CD0
uint32 CTrain::FindNumCarriagesPulled() {
    uint32 num;
    CTrain* carriage = m_pNextCarriage;
    for (num = 0; carriage; ++num) {
        carriage = carriage->m_pNextCarriage;
    }
    return num;
}

// 0x6F5D80
void CTrain::OpenTrainDoor(float state) {
    // NOP
}

// 0x6F5D90
void CTrain::AddPassenger(CPed* ped) {
    // NOP
}

// 0x6F5DA0
void CTrain::RemovePassenger(CPed* ped) {
    // NOP
}

// 0x6F5DB0
void CTrain::DisableRandomTrains(bool disable) {
    bDisableRandomTrains = disable;
}

// 0x6F5DC0
void CTrain::RemoveOneMissionTrain(CTrain* train) {
    if (!train)
        return;

    CTrain* next;
    CTrain* _train = train;
    do {
        next = _train->m_pNextCarriage;
        CWorld::Remove(_train);
        delete _train;
        _train = next;
    } while (next);
}

// 0x6F5DF0
void CTrain::ReleaseOneMissionTrain(CTrain* train) {
    for (auto* head = train; head; head = head->m_pNextCarriage) {
        head->trainFlags.bMissionTrain = false;
    }
}

// 0x6F5E20
void CTrain::SetTrainSpeed(CTrain* train, float speed) {
    // Como el original (0x6F5E20): fmul por la constante 0x858B38 (0.02f), no una division entre 50
    train->m_fTrainSpeed = speed * 0.02f;
    if (!train->trainFlags.bClockwiseDirection) {
        train->m_fTrainSpeed = -train->m_fTrainSpeed;
    }
}

// 0x6F5E50
void CTrain::SetTrainCruiseSpeed(CTrain* train, float speed) {
    // Como el original (0x6F5E50): _ftol2 (truncado a 64 bits) y se guarda el byte bajo (mov [ecx+3D0h],al)
    train->m_autoPilot.SetCruiseSpeed(uint8(int64(speed)));
}

// 0x6F5E70
CTrain* CTrain::FindCaboose(CTrain* train) {
    while (train->m_pNextCarriage) {
        train = train->m_pNextCarriage;
    }
    return train;
}

// 0x6F5E90
CTrain* CTrain::FindEngine(CTrain* train) {
    while (train->m_pPrevCarriage) {
        train = train->m_pPrevCarriage;
    }
    return train;
}

// 0x6F5EB0
CTrain* CTrain::FindCarriage(CTrain* train, uint8 carriage) {
    for (uint8 i = 0; i < carriage; ++i) {
        train = train->m_pNextCarriage;
        if (!train) {
            return nullptr;
        }
    }
    return train;
}

// 0x6F5EF0
bool CTrain::FindSideStationIsOn() const {
    return trainFlags.bClockwiseDirection; // ?
}

// 0x6F5F00
void CTrain::FindNextStationPositionInDirection(bool clockwiseDirection, float distance, float* distanceToStation, int32* numStations) {
    int32 station = 0;
    while (station < 6 && !(StationDist[station] > distance)) {
        ++station;
    }
    if (station >= 6) {
        station = 0;
    }
    if (!clockwiseDirection && --station < 0) {
        station += 6;
    }
    if (std::abs(distance - StationDist[station]) < 100.f) {
        station += clockwiseDirection ? 1 : -1;
        if (station < 0) {
            station += 6;
        }
        if (station >= 6) {
            station = 0;
        }
    }
    *numStations = station;
    *distanceToStation = StationDist[station];
}

// 0x6F6320
bool CTrain::IsInTunnel() const {
    const auto& pos = GetPosition();
    // Como el original (0x6F6320): colPoint y colEntity quedan sin inicializar (el EXE no los pone a cero)
    CColPoint colPoint;
    CEntity* colEntity;
    return CWorld::ProcessVerticalLine(pos, pos.z + 100.0f, colPoint, colEntity, true);
}

// 0x6F6640
void MarkSurroundingEntitiesForCollisionWithTrain(CVector pos, float radius, CEntity* entity, bool bOnlyVehicles) {
    // Como el original (0x6F6640): el sector se calcula con fmul por 0x858B38 (0.02f) y fadd 0x858B34 (60.0f), no con una division entre 50
    const auto Sector = [](float v) { return int32(std::floor(v * 0.02f + 60.0f)); };
    int32 startSectorX = std::max(Sector(pos.x - radius), 0);
    int32 startSectorY = std::max(Sector(pos.y - radius), 0);
    int32 endSectorX   = std::min(Sector(pos.x + radius), MAX_SECTORS_X - 1);
    int32 endSectorY   = std::min(Sector(pos.y + radius), MAX_SECTORS_Y - 1);

    CWorld::AdvanceCurrentScanCode();

    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            auto& repeatSector = CWorld::GetRepeatSector(sectorX, sectorY);
            TrainHitStuff(repeatSector.Vehicles, entity);
            if (!bOnlyVehicles) {
                TrainHitStuff(repeatSector.Peds, entity);
                TrainHitStuff(repeatSector.Objects, entity);
            }
        }
    }
}

// 0x6F5CF0
template<typename PtrListType>
void TrainHitStuff(PtrListType& ptrList, CEntity* entity) {
    auto* node = ptrList.GetNode();
    while (node) {
        auto* physical = static_cast<CPhysical*>(node->Item);
        node = node->Next;
        if (physical == entity) continue;
        physical->physicalFlags.bProcessCollisionEvenIfStationary = true;
        if (physical->GetType() != ENTITY_TYPE_OBJECT || !physical->GetIsStatic()) continue;
        const auto model = int32(int16(physical->m_nModelIndex));
        if (model != int32(StaticRef<uint16>(0x8CD6C8)) && model != int32(StaticRef<uint16>(0x8CD6CC)) && model != int32(StaticRef<uint16>(0x8CD6D0))) continue;
        physical->SetIsStatic(false);
        physical->AddToMovingList();
        physical->m_nFakePhysics = 0;
    }
}

// 0x6F6850
void CTrain::RemoveRandomPassenger() {
    if (StaticRef<uint8>(0xA43088) == 1) return;
    auto* player = FindPlayerPed(-1);
    if (player->m_pVehicle == this) return;
    auto* task = player->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
    if (task && (int32(task->GetTaskType()) == 701 || int32(task->GetTaskType()) == 700) && *reinterpret_cast<CVehicle**>(reinterpret_cast<uint8*>(task) + 0xC) == this) return;
    const auto clockwise = bool(trainFlags.bClockwiseDirection);
    player = FindPlayerPed(-1);
    if (player->bInVehicle && player->m_pVehicle == this && ((clockwise && m_pDriver == player) || (!clockwise && m_pDriver != player))) return;
    if (m_nNumPassengersToLeave == 0) return;
    if (m_pTemporaryPassenger) {
        if (m_pTemporaryPassenger->bInVehicle) return;
        m_pTemporaryPassenger->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_pTemporaryPassenger));
        m_pTemporaryPassenger = nullptr;
    }
    auto rating = CGeneral::GetRandomIntInRange(0, 12);
    if (rating == 12) rating = 11;
    if ((CGeneral::GetRandomNumber() & 0xFFFF) < 100) rating = 25;
    auto* ped = CPopulation::AddPedInCar(this, !clockwise, rating, 0, false, false);
    if (!ped) return;
    *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x478) |= 0x40000u;
    m_nNumPassengersToLeave = uint8(m_nNumPassengersToLeave - 1);
    m_pTemporaryPassenger = ped;
    ped->RegisterReference(reinterpret_cast<CEntity**>(&m_pTemporaryPassenger));
    // Preserve the existing task constructor and task-manager dependency ABI.
    auto* storage = CTask::operator new(0x1C);
    auto* leave = storage ? reinterpret_cast<CTask*(__thiscall*)(void*,CVehicle*,int32,int32,bool)>(0x63BDD0)(storage, this, 0, 0, true) : nullptr;
    reinterpret_cast<void(__thiscall*)(CTaskManager*,CTask*,ePrimaryTasks,bool)>(0x681AF0)(&ped->GetTaskManager(), leave, TASK_PRIMARY_PRIMARY, false);
}


// 0x6F6A20
void CTrain::RemoveMissionTrains() {
    // Como el original (0x6F6A20): recorre el pool de vehiculos de la ultima ranura a la primera
    auto* pool = GetVehiclePool();
    for (int32 i = int32(pool->GetSize()); i > 0; ) {
        auto* vehicle = pool->GetAt(--i);
        if (vehicle && vehicle->IsTrain() &&
            vehicle != FindPlayerVehicle(-1, false) &&
            vehicle->AsTrain()->trainFlags.bMissionTrain
        ) {
            CWorld::Remove(vehicle);
            delete vehicle;
        }
    }
}

// 0x6F6AA0
void CTrain::RemoveAllTrains() {
    auto* pool = GetVehiclePool();
    for (int32 i = int32(pool->GetSize()) - 1; i >= 0; --i) {
        auto* vehicle = pool->GetAt(i);
        if (!vehicle || !vehicle->IsTrain()) continue;
        auto* train = vehicle->AsTrain();
        bool playerInside{};
        for (auto* carriage = train; carriage; carriage = carriage->m_pPrevCarriage) {
            if (carriage == FindPlayerVehicle(-1, false)) playerInside = true;
        }
        for (auto* carriage = train; carriage; carriage = carriage->m_pNextCarriage) {
            if (carriage == FindPlayerVehicle(-1, false)) playerInside = true;
        }
        if (!playerInside) {
            CWorld::Remove(train);
            delete train;
        }
    }
}

// 0x6F6B60
void CTrain::ReleaseMissionTrains() {
    // Como el original (0x6F6B60): recorre el pool de vehiculos de la ultima ranura a la primera (and byte [esi+5B8h],0DFh)
    auto* pool = GetVehiclePool();
    for (int32 i = int32(pool->GetSize()); i > 0; ) {
        auto* vehicle = pool->GetAt(--i);
        if (vehicle && vehicle->IsTrain() && vehicle != FindPlayerVehicle(-1, false)) {
            vehicle->AsTrain()->trainFlags.bMissionTrain = false;
        }
    }
}

// 0x6F6BD0
int32 CTrain::FindClosestTrackNode(CVector position, int32* outTrackId) {
    auto bestDistance = std::bit_cast<float>(0x47C34FF3u);
    int32 closest; // The EXE also leaves this indeterminate if no node improves the initial distance.
    for (int32 track = 0; track < 4; ++track) {
        const auto* nodes = pTrackNodes[track];
        for (int32 i = 0; i < NumTrackNodes[track]; ++i) {
            const auto x = position.x - float(nodes[i].x) * .125f;
            const auto y = position.y - float(nodes[i].y) * .125f;
            const auto z = position.z - float(nodes[i].z) * .125f;
            const auto distance = std::sqrt((z * z + y * y) + x * x);
            if (distance < bestDistance) {
                bestDistance = distance;
                closest = i;
                *outTrackId = track;
            }
        }
    }
    return closest;
}

// 0x6F6CC0
void CTrain::FindPositionOnTrackFromCoors() {
    for (int32 i = 0; i < NumTrackNodes[m_nTrackId]; ++i) {
        const auto next = (i + 1) % NumTrackNodes[m_nTrackId];
        auto* nodes = pTrackNodes[m_nTrackId];
        const auto x = float(nodes[i].x) * .125f;
        const auto y = float(nodes[i].y) * .125f;
        const auto nx = float(nodes[next].x) * .125f;
        const auto ny = float(nodes[next].y) * .125f;
        const auto& pos = GetPosition();
        const auto factor = TrainProjectionCpp(x, y, nx, ny, pos.x, pos.y);
        if (!(factor > StaticRef<float>(0x858CDC) && factor < StaticRef<float>(0x872490))) continue;
        const auto dx = nx - x;
        const auto dy = ny - y;
        const auto ax = dx * factor;
        const auto projectedX = ax + x;
        const auto projectedY = dy * factor + y;
        const auto errorX = projectedX - pos.x;
        const auto errorY = projectedY - pos.y;
        if (std::sqrt((errorX * errorX + errorY * errorY) + (factor * 0.f) * (factor * 0.f)) < 3.f) {
            const auto& box = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->m_boundBox;
            m_fCurrentRailDistance = TrainRailDistanceCpp(nodes[i].m_nDistanceFromStart, nodes[next].m_nDistanceFromStart, factor, box.m_vecMin.y, box.m_vecMax.y, arrTotalTrackLength[m_nTrackId]);
            m_fTrainSpeed = std::sqrt((m_vecMoveSpeed.z * m_vecMoveSpeed.z + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.x * m_vecMoveSpeed.x);
            const auto dot = (dy * m_vecMoveSpeed.y + m_vecMoveSpeed.z * 0.f) + dx * m_vecMoveSpeed.x;
            if (bool(trainFlags.bClockwiseDirection) == (dot > 0.f)) m_fTrainSpeed = -m_fTrainSpeed;
            return;
        }
        const auto lighting = reinterpret_cast<uint8(__thiscall*)(CTrainNode*)>(0x6F5F80)(&nodes[i]);
        const auto day = StaticRef<float>(0x8D12C0);
        const auto previous = TrainNodeLightingCpp(lighting & 15, lighting >> 4, day);
        const auto nextLighting = reinterpret_cast<uint8(__thiscall*)(CTrainNode*)>(0x6F5F80)(&nodes[next]);
        const auto nextDay = StaticRef<float>(0x8D12C0);
        m_fContactSurfaceBrightness = TrainLightingBlendCpp(nextLighting & 15, nextLighting >> 4, nextDay, previous, factor);
    }
}


// 0x6F7090
CTrain* CTrain::FindNearestTrain(CVector posn, bool mustBeMainTrain) {
    auto* pool = GetVehiclePool();
    auto bestDistance = 10000000.f;
    CTrain* best{};
    for (int32 i = int32(pool->GetSize()) - 1; i >= 0; --i) {
        auto* vehicle = pool->GetAt(i);
        if (!vehicle || !vehicle->IsTrain()) continue;
        auto* train = vehicle->AsTrain();
        const auto dx = train->GetPosition().x - posn.x;
        const auto dy = train->GetPosition().y - posn.y;
        const auto distance = std::sqrt(dy * dy + dx * dx);
        if (distance < bestDistance && (!mustBeMainTrain || train->trainFlags.bIsFrontCarriage)) {
            bestDistance = distance;
            best = train;
        }
    }
    return best;
}

// 0x6F7140
void CTrain::SetNewTrainPosition(CTrain* train, CVector posn) {
    train->SetPosn(posn);
    train->FindPositionOnTrackFromCoors();
}

// 0x6F7260
bool CTrain::IsNextStationAllowed(CTrain* train) {
    const auto* engine = FindEngine(train);
    auto distance = engine->m_fCurrentRailDistance;
    int32 station;
    FindNextStationPositionInDirection(engine->trainFlags.bClockwiseDirection, distance, &distance, &station);
    const auto level = CTheZones::GetLevelFromPosition(aStationCoors[station]);
    return CStats::GetStatValue(static_cast<eStats>(181)) + 1.f >= float(level);
}

// 0x6F72F0
void CTrain::SkipToNextAllowedStation(CTrain* train) {
    auto* engine = FindEngine(train);
    auto railDistance = engine->m_fCurrentRailDistance;
    int32 station;
    do {
        FindNextStationPositionInDirection(engine->trainFlags.bClockwiseDirection, railDistance, &railDistance, &station);
        const auto level = float(CTheZones::GetLevelFromPosition(aStationCoors[station]));
        if (CStats::GetStatValue(static_cast<eStats>(181)) + 1.f >= level) break;
    } while (true);
    engine->m_fCurrentRailDistance = railDistance;
    if (engine->trainFlags.bClockwiseDirection) {
        engine->m_fCurrentRailDistance = railDistance - 20.f;
        engine->m_fTrainSpeed = .1f;
    } else {
        engine->m_fCurrentRailDistance = railDistance + 20.f;
        engine->m_fTrainSpeed = -.1f;
    }
    CStreaming::LoadScene(aStationCoors[station]);
    CStreaming::LoadAllRequestedModels(false);
    const auto dx = aStationCoors[station].x - engine->GetPosition().x;
    const auto dy = aStationCoors[station].y - engine->GetPosition().y;
    const auto minutes = std::sqrt(dx * dx + dy * dy) * StaticRef<float>(0x858C28) + StaticRef<float>(0x858F84);
    CGameLogic::PassTime(uint32(int64(minutes)));
}

// 0x6F7550
void CTrain::CreateMissionTrain(CVector posn, bool clockwise, uint32 type, CTrain** first, CTrain** last, int32 nodeIndex, int32 track, bool mission) {
    if (nodeIndex < 0) nodeIndex = FindClosestTrackNode(posn, &track);
    auto rail = float(pTrackNodes[track][nodeIndex].m_nDistanceFromStart) * StaticRef<float>(0x859040);
    auto* models = reinterpret_cast<const int32*>(0x8D44F8 + type * 0x40);
    std::array<CTrain*,16> carriages;
    int32 count = 0;
    CTrain* previous = nullptr;
    for (; *models; ++models) {
        auto* train = new CTrain(*models, PERMANENT_VEHICLE);
        train->m_matrix->SetTranslate({0.f,0.f,0.f});
        train->m_nNodeIndex = int16(nodeIndex);
        train->SetStatus(STATUS_ABANDONED);
        train->vehicleFlags.bIsLocked = true;
        train->m_fCurrentRailDistance = rail;
        train->trainFlags.bMissionTrain = mission;
        train->trainFlags.bClockwiseDirection = clockwise;
        train->m_nTrackId = int8(track);
        if (mission) train->trainFlags.bStopsAtStations = false;
        if (!previous) {
            if (mission) {train->SetPosn(posn);train->FindPositionOnTrackFromCoors();}
            train->m_fLength = 0.f;
        } else if (!clockwise) {
            const auto& box = CModelInfo::GetModelInfo(previous->m_nModelIndex)->GetColModel()->m_boundBox;
            const auto length = box.m_vecMax.y - box.m_vecMin.y;
            train->m_fLength = length;
            rail += length;
        } else {
            const auto& box = CModelInfo::GetModelInfo(train->m_nModelIndex)->GetColModel()->m_boundBox;
            const auto length = box.m_vecMax.y - box.m_vecMin.y;
            train->m_fLength = -length;
            rail -= length;
        }
        carriages[size_t(count++)] = train;
        if (!previous) {train->trainFlags.bIsFrontCarriage = true;if(first)*first=train;}
        else {
            train->trainFlags.bIsFrontCarriage = false;
            *(reinterpret_cast<uint8*>(train) + 0x42A) |= 2;
            previous->trainFlags.bIsLastCarriage = false;
        }
        train->trainFlags.bIsLastCarriage = true;
        if (last) *last = train;
        train->m_pPrevCarriage = previous;
        if (previous) previous->RegisterReference(reinterpret_cast<CEntity**>(&train->m_pPrevCarriage));
        train->m_pNextCarriage = nullptr;
        if (previous) {previous->m_pNextCarriage = train;train->RegisterReference(reinterpret_cast<CEntity**>(&previous->m_pNextCarriage));}
        train->ProcessControl();
        previous = train;
    }
    while (--count >= 0) {CWorld::Remove(carriages[size_t(count)]);CWorld::Add(carriages[size_t(count)]);}
    if ((*first)->m_nModelIndex != 449) {
        CPopulation::AddPedInCar(*first, true, -1, 0, false, false);
        if (*first && (*first)->m_pDriver) (*first)->m_pDriver->GetIntelligence()->SetPedDecisionMakerType(6);
    }
    bool streak = false;
    for (auto* train=*first;train;train=train->m_pNextCarriage) if(train->m_nModelIndex==538){streak=true;break;}
    for (auto* train=*first;train;train=train->m_pNextCarriage) train->trainFlags.bIsStreakModel = streak;
}


// 0x6F7900
void CTrain::DoTrainGenerationAndRemoval() {
    auto& nearStation = StaticRef<uint8>(0xC38068);
    const auto now = StaticRef<uint32>(0xB7CB84);
    const auto previousTime = StaticRef<uint32>(0xB7CB78);
    if (now / 3000u != previousTime / 3000u) {
        nearStation = 0;
        for (const auto& station : aStationCoors) {
            const auto player = FindPlayerCoors(-1);
            const auto dy = station.y - player.y;
            const auto dx = station.x - player.x;
            if (std::sqrt(dy * dy + dx * dx) < 60.f) nearStation = 1;
        }
    }
    const auto period = nearStation ? 1u : 950u;
    const auto generationRadius = nearStation ? 100.f : 70.f;
    if (StaticRef<uint32>(0xB7CB84) / period == StaticRef<uint32>(0xB7CB78) / period) return;
    bool found = false;
    auto* pool = GetVehiclePool();
    for (int32 i = int32(pool->GetSize()) - 1; i >= 0; --i) {
        auto* vehicle = pool->GetAt(i);
        if (!vehicle || !vehicle->IsSubTrain()) continue;
        auto* train = vehicle->AsTrain();
        if (!train->trainFlags.bIsFrontCarriage || train->trainFlags.bMissionTrain) continue;
        found = true;
        bool remove = true;
        for (auto* carriage = train; carriage; carriage = carriage->m_pNextCarriage) {
            const auto delta = TheCamera.GetPosition() - carriage->GetPosition();
            auto* playerVehicle = FindPlayerVehicle(-1, false);
            if (carriage == playerVehicle || std::sqrt(delta.y * delta.y + delta.x * delta.x) < 220.f) remove = false;
        }
        if (remove) {
            while (train) {auto* next = train->m_pNextCarriage;CWorld::Remove(train);delete train;train = next;}
        }
    }
    if (bDisableRandomTrains) return;
    const auto models = [](uint32 type) {return reinterpret_cast<const int32*>(0x8D44F8 + type * 0x40);};
    if (GenTrain_Status != 0) {
        if (GenTrain_Status != 1) return;
        bool loaded = true;
        for (auto* model = models(GenTrain_TrainConfig); *model; ++model) {
            if (StaticRef<uint8>(0x8E4CD0 + *model * 20) != 1) {CStreaming::RequestModel(*model, 8);loaded = false;}
        }
        if (!loaded) return;
        const auto camera = TheCamera.GetPosition();
        auto& node = pTrackNodes[GenTrain_Track][GenTrain_GenerationNode];
        const auto dx = float(node.x) * .125f - camera.x;
        const auto dy = float(node.y) * .125f - camera.y;
        if (std::sqrt(dx * dx + dy * dy) > 60.f) {
            CTrain* created;
            CreateMissionTrain({0.f,0.f,0.f}, GenTrain_Direction != 0, GenTrain_TrainConfig, &created, nullptr, int32(GenTrain_GenerationNode), int32(GenTrain_Track), false);
            int32 speed;
            if (GenTrain_Track == 0) {
                speed = int32(CGeneral::GetRandomNumber()) % 30 + 15;
                auto& current = pTrackNodes[GenTrain_Track][GenTrain_GenerationNode];
                const auto x = float(current.x) * .125f - 2222.f;
                const auto y = float(current.y) * .125f + 1750.f;
                if (std::sqrt(x * x + y * y) < 300.f) speed /= 2;
            } else speed = int32(CGeneral::GetRandomNumber()) % 7 + 7;
            float maximum;
            created->FindMaximumSpeedToStopAtStations(&maximum);
            if (float(speed) < maximum) maximum = float(speed);
            SetTrainSpeed(created, maximum);
            created->m_autoPilot.m_nCruiseSpeed = uint8(speed);
        }
        for (auto* model = models(GenTrain_TrainConfig); *model; ++model) {CStreaming::SetModelIsDeletable(*model);CStreaming::SetModelTxdIsDeletable(*model);}
        GenTrain_Status = 0;
        return;
    }
    if (found) return;
    const auto camera = TheCamera.GetPosition();
    GenTrain_Track = 0;
    GenTrain_GenerationNode = uint32(int32(CGeneral::GetRandomNumber()) % NumTrackNodes[0]);
    const auto distance = [&]() {
        const auto& node = pTrackNodes[GenTrain_Track][GenTrain_GenerationNode];
        const auto dy = float(node.y) * .125f - camera.y;
        const auto dx = float(node.x) * .125f - camera.x;
        return std::sqrt(dy * dy + dx * dx);
    };
    auto dist = distance();
    const auto nodePos = CVector{float(pTrackNodes[0][GenTrain_GenerationNode].x) * .125f,float(pTrackNodes[0][GenTrain_GenerationNode].y) * .125f,float(pTrackNodes[0][GenTrain_GenerationNode].z) * .125f};
    bool skipFirst = dist >= generationRadius;
    if (!skipFirst) {
        if (nodePos.z + 6.f <= FindPlayerCoors(-1).z) {
            const auto player = FindPlayerCoors(-1);
            if (!(CCullZones::FindTunnelAttributesForCoors(player) & 0x880) && (CCullZones::FindTunnelAttributesForCoors(nodePos) & 0x800)) skipFirst = true;
        }
    }
    if (!skipFirst) {
        const auto direction = CGeneral::GetRandomNumber() & 1;
        GenTrain_Direction = uint8(direction);
        auto previousNode = GenTrain_GenerationNode;
        while (dist < 170.f) {
            previousNode = GenTrain_GenerationNode;
            if (!direction) GenTrain_GenerationNode = uint32((int32(GenTrain_GenerationNode) + 1) % NumTrackNodes[GenTrain_Track]);
            else {auto index = int32(GenTrain_GenerationNode) - 1;if(index < 0) index += NumTrackNodes[GenTrain_Track];GenTrain_GenerationNode = uint32(index);}
            dist = distance();
        }
        auto& lastType = StaticRef<int32>(0xC38064);
        GenTrain_TrainConfig = uint32(int32(uint32(lastType) + 1u) % 8);
        GenTrain_Status = 1;
        lastType = int32(GenTrain_TrainConfig);
        if (dist > 220.f) {GenTrain_GenerationNode = previousNode;return;}
        return;
    }
    if (GenTrain_Status != 0 || StaticRef<int32>(0xC81314) != 2) return;
    GenTrain_Track = 1;
    GenTrain_GenerationNode = uint32(int32(CGeneral::GetRandomNumber()) % NumTrackNodes[1]);
    dist = distance();
    if (!(dist < generationRadius)) return;
    const auto direction = CGeneral::GetRandomNumber() & 1;
    GenTrain_Direction = uint8(direction);
    while (dist < 170.f) {
        if (!direction) GenTrain_GenerationNode = uint32((int32(GenTrain_GenerationNode) + 1) % NumTrackNodes[GenTrain_Track]);
        else {auto index = int32(GenTrain_GenerationNode) - 1;if(index < 0) index += NumTrackNodes[GenTrain_Track];GenTrain_GenerationNode = uint32(index);}
        dist = distance();
    }
    if (dist < 220.f) {GenTrain_Status = 1;GenTrain_TrainConfig = uint32(int32(CGeneral::GetRandomNumber()) % 2 + 8);}
}


// 0x6F8170
void CTrain::AddNearbyPedAsRandomPassenger() {
    auto* player = FindPlayerPed(-1);
    if (player->m_pVehicle == this) return;
    auto* active = player->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
    if (active && (int32(active->GetTaskType()) == 701 || int32(active->GetTaskType()) == 700) && *reinterpret_cast<CVehicle**>(reinterpret_cast<uint8*>(active) + 0xC) == this) return;
    const auto clockwise = bool(trainFlags.bClockwiseDirection);
    if (m_nNumPassengersToLeave == m_nNumPassengersToEnter) return;
    if (m_pTemporaryPassenger) {
        const auto leftTrain = m_pTemporaryPassenger->bJustGotOffTrain;
        const auto inVehicle = m_pTemporaryPassenger->bInVehicle;
        if (leftTrain == inVehicle) return;
        if (m_pTemporaryPassenger->GetIntelligence()->FindTaskByType(static_cast<eTaskType>(707))) return;
        m_pTemporaryPassenger->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_pTemporaryPassenger));
        auto* previous = m_pTemporaryPassenger;
        if (!previous->bJustGotOffTrain && previous->bInVehicle && previous->m_pVehicle == this) CPopulation::RemovePed(previous);
        m_pTemporaryPassenger = nullptr;
    }
    const auto position = GetPosition();
    const auto right = GetRight();
    const auto originDot = (position.z * right.z + position.y * right.y) + position.x * right.x;
    CPed* closest = nullptr;
    float closestDistance = 999999.f;
    auto* pool = GetPedPool();
    for (int32 i = int32(pool->GetSize()) - 1; i >= 0; --i) {
        auto* ped = pool->GetAt(i);
        if (!ped || ped->GetCreatedBy() != 1 || CPedGroups::GetPedsGroup(ped) || ped->GetPlayerData() || int32(ped->m_nPedType) == 6 || ped->bInVehicle || ped->bJustGotOffTrain) continue;
        const auto state = int32(ped->m_nPedState);
        if (state == 54 || state == 55 || state == 56 || ped->GetIntelligence()->FindTaskByType(static_cast<eTaskType>(700))) continue;
        const auto& current = ped->GetPosition();
        const auto& trainPosition = GetPosition();
        const auto dx = current.x - trainPosition.x;
        const auto dy = current.y - trainPosition.y;
        const auto distance = dx * dx + dy * dy;
        if (!(distance <= 625.f)) continue;
        const auto side = (current.z * m_matrix->GetRight().z + current.y * m_matrix->GetRight().y) + current.x * m_matrix->GetRight().x - originDot;
        if ((!clockwise && side <= 0.f) || (clockwise && side >= 0.f)) {
            if (distance < closestDistance) {closest = ped;closestDistance = distance;}
        }
    }
    if (!closest) return;
    CVector doorPosition;
    int32 door = 0;
    CCarEnterExit::GetNearestCarDoor(closest, this, doorPosition, door);
    CMatrix inverse;
    Invert(*m_matrix, inverse);
    auto relative = inverse.TransformPoint(doorPosition);
    if (!clockwise) relative -= GetRight();
    else relative += GetRight();
    // These existing task constructors and sequencing calls remain dependencies outside this scope.
    auto* seekStorage = CTask::operator new(0x58);
    auto* seek = seekStorage ? reinterpret_cast<CTask*(__thiscall*)(void*,CEntity*,int32,int32,float,float,float,bool,bool)>(0x661DC0)(seekStorage, this, 50000, 1000, StaticRef<float>(0x86FC2C), StaticRef<float>(0x86FC28), StaticRef<float>(0x86FC30), true, true) : nullptr;
    *reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(seek) + 0x44) = relative;
    *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(seek) + 0x50) = 7;
    auto* sequenceStorage = CTask::operator new(0x40);
    auto* sequence = sequenceStorage ? reinterpret_cast<CTask*(__thiscall*)(void*)>(0x632BD0)(sequenceStorage) : nullptr;
    const auto append = reinterpret_cast<bool(__thiscall*)(CTask*,CTask*)>(0x632D10);
    append(sequence, seek);
    auto* enterStorage = CTask::operator new(0x50);
    CTask* enter = nullptr;
    if (enterStorage) {
        if (!clockwise) enter = reinterpret_cast<CTask*(__thiscall*)(void*,CVehicle*)>(0x6402F0)(enterStorage, this);
        else enter = reinterpret_cast<CTask*(__thiscall*)(void*,CVehicle*,int32,bool)>(0x640340)(enterStorage, this, 0, false);
    }
    append(sequence, enter);
    reinterpret_cast<void(__thiscall*)(CTaskManager*,CTask*,ePrimaryTasks,bool)>(0x681AF0)(&closest->GetTaskManager(), sequence, TASK_PRIMARY_PRIMARY, false);
    m_nNumPassengersToLeave = uint8(m_nNumPassengersToLeave + 1);
    m_pTemporaryPassenger = closest;
    closest->RegisterReference(reinterpret_cast<CEntity**>(&m_pTemporaryPassenger));
}



// Como el original: el EXE inlinea CTrainNode::GetDistanceFromStart (0x6F54B0) como uint16 * 0.3333333432674408 (0x859040), no / 3.0f.
static float NodeDistanceFromStart(const CTrainNode& node) {
    return static_cast<float>(node.m_nDistanceFromStart) * std::bit_cast<float>(0x3EAAAAABu);
}

// 0x6F86A0
void CTrain::ProcessControl() {
    vehicleFlags.bWarnedPeds = 0;
    m_vehicleAudio.Service();
    if (gbModelViewer) {
        return;
    }

    CVector vecOldTrainPosition = GetPosition();
    float fOldTrainHeading = GetHeading();

    const float& fTotalTrackLength = arrTotalTrackLength[m_nTrackId];
    CTrainNode* trainNodes = pTrackNodes[m_nTrackId];
    auto numTrackNodes = NumTrackNodes[m_nTrackId];

    if (trainFlags.bNotOnARailRoad == 0) {
        if (!trainFlags.bIsFrontCarriage) {
            if (m_pPrevCarriage) {
                m_fTrainSpeed = m_pPrevCarriage->m_fTrainSpeed;
                m_fCurrentRailDistance = m_pPrevCarriage->m_fCurrentRailDistance + m_fLength;
            } else {
                // Como el original (0x6F8E49..0x6F8E72): pow del EXE (0x822130) con base double 0.99f (0x872500) y producto sin redondear antes de guardar
                PowScaleFloatCpp(&m_fTrainSpeed, 0.99f, CTimer::GetTimeStep());
                m_fCurrentRailDistance += m_fTrainSpeed * CTimer::GetTimeStep();
            }

            if (trainFlags.b01 && trainFlags.bStoppedAtStation && m_nModelIndex == MODEL_STREAKC && !trainFlags.bMissionTrain) {
                // Como el original: FindPlayerPed(-1) solo se llama dentro de los estados 0 (0x6F8EAD) y 2 (0x6F8F13)
                if (m_nPassengersGenerationState == TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_LEAVE) {
                    if (FindPlayerPed(-1)->m_pVehicle == this) {
                        m_nNumPassengersToLeave = 0;
                    } else {
                        m_nNumPassengersToLeave = (CGeneral::GetRandomNumber() & 3) + 1; // [1, 4]
                    }
                    m_nPassengersGenerationState = TRAIN_PASSENGERS_TELL_PASSENGERS_TO_LEAVE;
                }

                if (m_nPassengersGenerationState == TRAIN_PASSENGERS_TELL_PASSENGERS_TO_LEAVE) {
                    RemoveRandomPassenger();
                    if (m_nNumPassengersToLeave == 0) {
                        m_nPassengersGenerationState = TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_ENTER;
                    }
                }

                if (m_nPassengersGenerationState == TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_ENTER) {
                    if (FindPlayerPed(-1)->m_pVehicle == this) {
                        m_nNumPassengersToEnter = 0;
                    } else {
                        m_nNumPassengersToEnter = CGeneral::GetRandomNumber() % 4 + 1; // rand(1, 4)
                    }
                    m_nPassengersGenerationState = TRAIN_PASSENGERS_TELL_PASSENGERS_TO_ENTER;
                }

                if (m_nPassengersGenerationState == TRAIN_PASSENGERS_TELL_PASSENGERS_TO_ENTER) {
                    if (trainFlags.bPassengersCanEnterAndLeave) {
                        AddNearbyPedAsRandomPassenger();
                        if (m_nNumPassengersToLeave == m_nNumPassengersToEnter) {
                            m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                        }
                    }
                }
            }
        } else {
            CPad* pad = CPad::GetPad();
            if (m_pDriver && m_pDriver->IsPlayer()) {
                pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
            }

            uint32 numCarriagesPulled = FindNumCarriagesPulled();
            if (!trainFlags.bClockwiseDirection) {
                m_fTrainSpeed = -m_fTrainSpeed;
            }

            if (GetStatus()) {
                bool bIsStreakModel = trainFlags.bIsStreakModel;
                auto fStopAtStationSpeed = static_cast<float>(m_autoPilot.m_nCruiseSpeed);

                uint32 timeAtStation = CTimer::GetTimeInMS() - m_nTimeWhenStoppedAtStation;
                if (timeAtStation >= (bIsStreakModel ? 20'000u : 10'000u)) {
                    if (timeAtStation >= (bIsStreakModel ? 28'000u : 18'000u)) {
                        if (timeAtStation >= (bIsStreakModel ? 32'000u : 22'000u)) {
                            if (trainFlags.bStopsAtStations) {
                                float maxTrainSpeed = 0.0f;
                                if (FindMaximumSpeedToStopAtStations(&maxTrainSpeed)) {
                                    fStopAtStationSpeed = 0.0f;
                                    m_nTimeWhenStoppedAtStation = CTimer::GetTimeInMS();
                                } else {
                                    if (fStopAtStationSpeed >= maxTrainSpeed) {
                                        fStopAtStationSpeed = maxTrainSpeed;
                                    }
                                }
                            }
                        } else if (trainFlags.bStoppedAtStation) {
                            CTrain* trainCarriage = this;
                            do {
                                // Como el original (0x6F8940): "and [eax+5B8h],0FDh" sobre cada vagon (eax = vagon), no sobre this
                                trainCarriage->trainFlags.bStoppedAtStation = false;
                                trainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                                trainCarriage = trainCarriage->m_pNextCarriage;
                            } while (trainCarriage);
                        }
                    } else {
                        fStopAtStationSpeed = 0.0f;
                        if (trainFlags.bStoppedAtStation) {
                            CTrain* trainCarriage = this;
                            do {
                                // Como el original (0x6F8910): "and [eax+5B8h],0FBh" sobre cada vagon
                                trainCarriage->trainFlags.bPassengersCanEnterAndLeave = false;
                                trainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                                trainCarriage = trainCarriage->m_pNextCarriage;
                            } while (trainCarriage);
                        }
                    }
                } else {
                    fStopAtStationSpeed = 0.0f;
                    if (!trainFlags.bStoppedAtStation) {
                        CTrain* trainCarriage = this;
                        do {
                            // Como el original (0x6F88D0): "or [eax+5B8h],6" sobre cada vagon
                            trainCarriage->trainFlags.bStoppedAtStation = true;
                            trainCarriage->trainFlags.bPassengersCanEnterAndLeave = true;
                            trainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_LEAVE;
                            trainCarriage = trainCarriage->m_pNextCarriage;
                        } while (trainCarriage);
                    }
                }

                fStopAtStationSpeed = fStopAtStationSpeed * 0.02f - m_fTrainSpeed; // Como el original (0x6F8991): fmul [858B38] (0.02f), no / 50
                if (fStopAtStationSpeed > 0.0f) {
                    m_fTrainGas = fStopAtStationSpeed * 30.0f;
                    if (m_fTrainGas >= 1.0f) {
                        m_fTrainGas = 1.0f;
                    }

                    m_fTrainGas *= 255.0f;
                    m_fTrainBrake = 0.0f;
                } else {
                    float fTrainSpeed = fStopAtStationSpeed * -30.0f;
                    m_fTrainGas = 0.0f;
                    if (fTrainSpeed >= 1.0f) {
                        fTrainSpeed = 1.0f;
                    }
                    m_fTrainBrake = fTrainSpeed * 255.0f;
                }
            } else {
                float fTrainSpeed = m_fTrainSpeed;
                if (fTrainSpeed < 0.0f) {
                    fTrainSpeed = -fTrainSpeed;
                }

                if (fTrainSpeed < 0.001f) {
                    m_fTrainBrake = 0.0f;
                    m_fTrainGas = static_cast<float>(pad->GetAccelerate() - pad->GetBrake());
                } else {
                    if (m_fTrainSpeed > 0.0f) {
                        m_fTrainBrake = static_cast<float>(pad->GetBrake());
                        m_fTrainGas = static_cast<float>(pad->GetAccelerate());
                    } else {
                        m_fTrainGas = static_cast<float>(-pad->GetBrake());
                        m_fTrainBrake = static_cast<float>(pad->GetAccelerate());
                    }
                }
            }

            if (trainFlags.bForceSlowDown) {
                const CVector& vecPoint = GetPosition();
                CVector vecDistance{};
                if (CGameLogic::CalcDistanceToForbiddenTrainCrossing(vecPoint, m_vecMoveSpeed, true, vecDistance) < 230.0f) {
                    if (DotProduct(GetForwardVector(), vecDistance) <= 0.0f) {
                        m_fTrainGas = std::max(0.0f, m_fTrainGas);
                    } else {
                        m_fTrainGas = std::min(0.0f, m_fTrainGas);
                    }

                    if (CGameLogic::CalcDistanceToForbiddenTrainCrossing(vecPoint, m_vecMoveSpeed, false, vecDistance) < 230.0f) {
                        m_fTrainBrake = 512.0f;
                    }
                }
            }

            numCarriagesPulled += 3;

            m_fTrainSpeed += m_fTrainGas / 256.0f * CTimer::GetTimeStep() * 0.002f / float(numCarriagesPulled);

            if (m_fTrainBrake != 0.0f) {
                float fTrainSpeed = m_fTrainSpeed;
                if (m_fTrainSpeed < 0.0f) {
                    fTrainSpeed = -fTrainSpeed;
                }
                float fBreak = m_fTrainBrake / 256.0f * CTimer::GetTimeStep() * 0.006f / float(numCarriagesPulled);
                if (fTrainSpeed >= fBreak) {
                    if (m_fTrainSpeed < 0.0f) {
                        m_fTrainSpeed += fBreak;
                    } else {
                        m_fTrainSpeed -= fBreak;
                    }
                } else {
                    m_fTrainSpeed = 0.0f;
                }
            }

            // Como el original (0x6F8BF2..0x6F8C10): pow del EXE con base double 0.99975 (0x872508), producto sin redondear
            PowScaleFloatCpp(&m_fTrainSpeed, 0.99975f, CTimer::GetTimeStep());
            if (!trainFlags.bClockwiseDirection) {
                m_fTrainSpeed = -m_fTrainSpeed;
            }

            m_fCurrentRailDistance += CTimer::GetTimeStep() * m_fTrainSpeed;

            if (GetStatus() == STATUS_PLAYER) {

                float fTheTrainSpeed = m_fTrainSpeed;
                if (fTheTrainSpeed < 0.0f) {
                    fTheTrainSpeed = -fTheTrainSpeed;
                }
                if (fTheTrainSpeed > 1.0f * 0.95f) {
                    CPad::GetPad()->StartShake(300, 70, 0);
                    TheCamera.CamShake(0.1f, GetPosition());
                }

                fTheTrainSpeed = m_fTrainSpeed;
                if (fTheTrainSpeed < 0.0f) {
                    fTheTrainSpeed = -fTheTrainSpeed;
                }

                if (fTheTrainSpeed > 1.0f) {
                    int32 nNodeIndex = m_nNodeIndex;
                    int32 previousNodeIndex = nNodeIndex - 1;
                    if (previousNodeIndex < 0) {
                        previousNodeIndex = numTrackNodes;
                    }

                    int32 previousNodeIndex2 = previousNodeIndex - 1;
                    if (previousNodeIndex2 < 0) {
                        previousNodeIndex2 = numTrackNodes;
                    }

                    CTrainNode* pCurrentTrainNode = &trainNodes[m_nNodeIndex];
                    CTrainNode* pPreviousTrainNode = &trainNodes[previousNodeIndex];
                    CTrainNode* pPreviousTrainNode2 = &trainNodes[previousNodeIndex2];

                    CVector vecDifference1 = pCurrentTrainNode->GetPosn() - pPreviousTrainNode->GetPosn();
                    CVector vecDifference2 = pPreviousTrainNode->GetPosn() - pPreviousTrainNode2->GetPosn();
                    vecDifference1.Normalise();
                    vecDifference2.Normalise();

                    // Como el original (0x6F8D93..0x6F8DAF): producto escalar (z*z + x*x) + y*y (no el orden z, y, x de DotProduct)
                    const float fDot = (vecDifference1.z * vecDifference2.z + vecDifference1.x * vecDifference2.x) + vecDifference1.y * vecDifference2.y;
                    if (fDot < 0.996f) {
                        CTrain* carriage = this;
                        bool bIsInTunnel = false;
                        while (!bIsInTunnel) {
                            bIsInTunnel = carriage->IsInTunnel();
                            carriage = carriage->m_pNextCarriage;
                            if (!carriage) {
                                if (!bIsInTunnel) {
                                    CTrain* theTrainCarriage = this;
                                    do {
                                        // Como el original (0x6F8DF0): "or [edi+5B9h],1" sobre cada vagon (edi)
                                        theTrainCarriage->trainFlags.bNotOnARailRoad = true;
                                        theTrainCarriage->physicalFlags.bDisableCollisionForce = false;
                                        theTrainCarriage->physicalFlags.bDisableSimpleCollision = false;
                                        theTrainCarriage->SetIsStatic(false);
                                        theTrainCarriage = theTrainCarriage->m_pNextCarriage;
                                    } while (theTrainCarriage);

                                    CPhysical::ProcessControl();
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (m_fCurrentRailDistance < 0.0f) {
            do {
                m_fCurrentRailDistance += fTotalTrackLength;
            } while (m_fCurrentRailDistance < 0.0f);
        }

        if (m_fCurrentRailDistance >= fTotalTrackLength) {
            do {
                m_fCurrentRailDistance -= fTotalTrackLength;
            } while (m_fCurrentRailDistance >= fTotalTrackLength);
        }

        float fNextNodeTrackLength = 0.0f;
        int32 nextNodeIndex = m_nNodeIndex + 1;
        if (nextNodeIndex < numTrackNodes) {
            CTrainNode* nextTrainNode = &trainNodes[nextNodeIndex];
            fNextNodeTrackLength = NodeDistanceFromStart(*nextTrainNode);
        } else {
            fNextNodeTrackLength = fTotalTrackLength;
            nextNodeIndex = 0;
        }

        CTrainNode* theTrainNode = &trainNodes[m_nNodeIndex];
        float fCurrentNodeTrackLength = NodeDistanceFromStart(*theTrainNode);
        while (m_fCurrentRailDistance < fCurrentNodeTrackLength || fNextNodeTrackLength < m_fCurrentRailDistance) {
            int32 newNodeIndex = m_nNodeIndex - 1; // previous node
            if (fCurrentNodeTrackLength <= m_fCurrentRailDistance) {
                newNodeIndex = m_nNodeIndex + 1; // next node
            }
            m_nNodeIndex = newNodeIndex % numTrackNodes;
            m_vehicleAudio.AddAudioEvent(AE_TRAIN_CLACK, 0.0f);

            theTrainNode = &trainNodes[m_nNodeIndex];
            fCurrentNodeTrackLength = NodeDistanceFromStart(*theTrainNode);

            nextNodeIndex = m_nNodeIndex + 1;
            if (nextNodeIndex < numTrackNodes) {
                CTrainNode* nextTrainNode = &trainNodes[nextNodeIndex];
                fNextNodeTrackLength = NodeDistanceFromStart(*nextTrainNode);
            } else {
                fNextNodeTrackLength = fTotalTrackLength;
                nextNodeIndex = 0;
            }
        }

        CTrainNode* nextTrainNode = &trainNodes[nextNodeIndex];
        fNextNodeTrackLength = NodeDistanceFromStart(*nextTrainNode);

        float fTrackNodeDifference = fNextNodeTrackLength - fCurrentNodeTrackLength;
        if (fTrackNodeDifference < 0.0f) {
            fTrackNodeDifference += fTotalTrackLength;
        }

        float fTheDistance = (m_fCurrentRailDistance - fCurrentNodeTrackLength) / fTrackNodeDifference;
        CVector vecPosition1 = theTrainNode->GetPosn() * (1.0f - fTheDistance) + nextTrainNode->GetPosn() * fTheDistance;

        CColModel* vehicleColModel = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel();
        const CBoundingBox& bbox = vehicleColModel->GetBoundingBox();
        float fTotalCurrentRailDistance = bbox.GetLength() + m_fCurrentRailDistance;
        if (fTotalCurrentRailDistance > fTotalTrackLength) {
            fTotalCurrentRailDistance -= fTotalTrackLength;
        }

        nextNodeIndex = m_nNodeIndex + 1;
        if (nextNodeIndex < numTrackNodes) {
            fNextNodeTrackLength = NodeDistanceFromStart(trainNodes[nextNodeIndex]);
        } else {
            fNextNodeTrackLength = fTotalTrackLength;
            nextNodeIndex = 0;
        }

        int32 trainNodeIndex = m_nNodeIndex;
        while (fTotalCurrentRailDistance < fCurrentNodeTrackLength || fTotalCurrentRailDistance > fNextNodeTrackLength) {
            trainNodeIndex = (trainNodeIndex + 1) % numTrackNodes;

            theTrainNode = &trainNodes[trainNodeIndex];
            fCurrentNodeTrackLength = NodeDistanceFromStart(*theTrainNode);

            nextNodeIndex = trainNodeIndex + 1;
            if (nextNodeIndex < numTrackNodes) {
                fNextNodeTrackLength = NodeDistanceFromStart(trainNodes[nextNodeIndex]);
            } else {
                fNextNodeTrackLength = fTotalTrackLength;
                nextNodeIndex = 0;
            }
        }

        fNextNodeTrackLength = NodeDistanceFromStart(trainNodes[nextNodeIndex]);

        fTrackNodeDifference = fNextNodeTrackLength - fCurrentNodeTrackLength;
        if (fTrackNodeDifference < 0.0f) {
            fTrackNodeDifference += fTotalTrackLength;
        }

        fTheDistance = (fTotalCurrentRailDistance - fCurrentNodeTrackLength) / fTrackNodeDifference;
        CVector vecPosition2 = theTrainNode->GetPosn() * (1.0f - fTheDistance) + trainNodes[nextNodeIndex].GetPosn() * fTheDistance;

        {
            CVector& vecVehiclePosition = GetPosition();
            vecVehiclePosition = (vecPosition1 + vecPosition2) / 2.0f;
            vecVehiclePosition.z += m_pHandlingData->m_fSuspensionLowerLimit - bbox.m_vecMin.z;
        }

        GetForward() = vecPosition2 - vecPosition1;
        GetForward().Normalise();
        if (!trainFlags.bClockwiseDirection) {
            GetForward() *= -1.0f;
        }

        CVector vecTemp(0.0f, 0.0f, 1.0f);
        CrossProduct(&GetRight(), &GetForward(), &vecTemp);
        GetRight().Normalise();
        CrossProduct(&GetUp(), &GetRight(), &GetForward());

        auto fTrainNodeLighting     = theTrainNode->GetLightingFromCollision().GetCurrentLighting();
        auto fTrainNextNodeLighting = trainNodes[nextNodeIndex].GetLightingFromCollision().GetCurrentLighting();

        fTrainNodeLighting += (fTrainNextNodeLighting - fTrainNodeLighting) * fTheDistance;
        m_fContactSurfaceBrightness = fTrainNodeLighting;
        m_vecMoveSpeed = (1.0f / CTimer::GetTimeStep()) * (GetPosition() - vecOldTrainPosition);

        // Como el original (0x6F9626..0x6F964C): rumbo nuevo en precision completa menos el viejo; constantes pi / 2*pi del EXE
        // (0x858CB8 = 0x40490FDB, 0x858CBC = 0x40C90FDB, 0x858CC0 = 0xC0490FDB), no el PI = 3.14159f de common.h
        float fHeading = m_matrix ? Atan2MinusCpp(-m_matrix->GetForward().x, m_matrix->GetForward().y, fOldTrainHeading) : m_placement.m_fHeading - fOldTrainHeading;
        if (fHeading <= std::bit_cast<float>(0x40490FDBu)) {
            if (fHeading < std::bit_cast<float>(0xC0490FDBu)) {
                fHeading += std::bit_cast<float>(0x40C90FDBu);
            }
        } else {
            fHeading -= std::bit_cast<float>(0x40C90FDBu);
        }

        m_vecTurnSpeed = CVector(0.0f, 0.0f, fHeading / CTimer::GetTimeStep());

        if (trainFlags.bNotOnARailRoad) {
            m_vecMoveSpeed *= -1.0f;
            m_vecTurnSpeed *= -1.0f;

            ApplyMoveSpeed();

            m_vecMoveSpeed *= -1.0f;
            m_vecTurnSpeed *= -1.0f;

            CPhysical::ProcessControl();
        } else {
            m_vecMoveSpeed.x = std::clamp(m_vecMoveSpeed.x, -2.0f, 2.0f);
            m_vecMoveSpeed.y = std::clamp(m_vecMoveSpeed.y, -2.0f, 2.0f);
            m_vecMoveSpeed.z = std::clamp(m_vecMoveSpeed.z, -2.0f, 2.0f);

            m_vecTurnSpeed.x = std::clamp(m_vecTurnSpeed.x, -0.1f, 0.1f);
            m_vecTurnSpeed.y = std::clamp(m_vecTurnSpeed.y, -0.1f, 0.1f);
            m_vecTurnSpeed.z = std::clamp(m_vecTurnSpeed.z, -0.1f, 0.1f);
        }

        UpdateRwMatrix();
        UpdateRwFrame();
        RemoveAndAdd();

        SetIsStuck(false);
        SetWasPostponed(false);
        SetIsInSafePosition(true);

        {
            // Como el original (0x6F9910..0x6F9939): (dz*dz + dy*dy) + dx*dx y fsqrt (DistanceBetweenPoints suma en orden x, y, z)
            const CVector vecDelta = vecOldTrainPosition - GetPosition();
            m_fMovingSpeed = std::sqrt((vecDelta.z * vecDelta.z + vecDelta.y * vecDelta.y) + vecDelta.x * vecDelta.x);
        }

        if (trainFlags.bIsFrontCarriage || trainFlags.bIsLastCarriage) {
            CVector vecPoint = bbox.m_vecMax.y * GetForward();
            vecPoint += GetPosition();
            vecPoint += CTimer::GetTimeStep() * m_vecMoveSpeed;

            MarkSurroundingEntitiesForCollisionWithTrain(vecPoint, 3.0f, this, false);
        }

        if (!vehicleFlags.bWarnedPeds) {
            CCarCtrl::ScanForPedDanger(this);
        }
        return;
    } else {
        if (!GetIsStuck()) {
            float fMaxForce = 0.003f;
            float fMaxTorque = 0.0009f;
            float fMaxMovingSpeed = 0.005f;

            if (GetStatus() != STATUS_PLAYER) {
                fMaxForce = 0.006f;
                fMaxTorque = 0.0015f;
                fMaxMovingSpeed = 0.015f;
            }

            float fMaxForceTimeStep  = (fMaxForce  * CTimer::GetTimeStep()) * (fMaxForce * CTimer::GetTimeStep());
            float fMaxTorqueTimeStep = (fMaxTorque * CTimer::GetTimeStep()) * (fMaxTorque * CTimer::GetTimeStep());

            m_vecForce  = (m_vecForce  + m_vecMoveSpeed) / 2.0f;
            m_vecTorque = (m_vecTorque + m_vecTurnSpeed) / 2.0f;

            if (m_vecForce.SquaredMagnitude() > fMaxForceTimeStep ||
                m_vecTorque.SquaredMagnitude() > fMaxTorqueTimeStep ||
                m_fMovingSpeed >= fMaxMovingSpeed ||
                m_fDamageIntensity > 0.0f && m_pDamageEntity != nullptr && m_pDamageEntity->GetIsTypePed()
            ) {
                m_nFakePhysics = 0;
            } else {
                m_nFakePhysics += 1;
                if (m_nFakePhysics > 10 /*&& !plugin::Call<0x424100>()*/) {
                    // if (m_nFakePhysics > 10) { // OG redundant check
                        m_nFakePhysics = 10;
                    // }

                    ResetMoveSpeed();
                    ResetTurnSpeed();
                    SkipPhysics();
                    return;
                }
            }
        }

        CPhysical::ProcessControl();

        // Como el original (0x6F9CA6..0x6F9CB7): sin inicializar; ProcessBuoyancy(this, constante, &puntoDeGiro [esp+68h], &fuerza [esp+44h]).
        // La fuerza reutiliza el espacio de vecOldTrainPosition en el EXE: todo lo que sigue (cociente, ApplyMoveForce y ApplyTurnForce)
        // lee la fuerza devuelta (0x6F9D05 fdivr [esp+4Ch]; 0x6F9D25/0x6F9D6E), no la posicion antigua del tren.
        CVector vecBuoyancyTurnPoint, vecBuoyancyForce;
        if (mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &vecBuoyancyTurnPoint, &vecBuoyancyForce)) {
            physicalFlags.bTouchingWater = true;

            float fTimeStep = 0.01f;
            if (CTimer::GetTimeStep() >= 0.01f) {
                fTimeStep = CTimer::GetTimeStep();
            }

            float fSpeedFactor = 1.0f - vecBuoyancyForce.z / (fTimeStep * m_fMass * 0.008f) * 0.05f;
            // Como el original (0x6F9D1B..0x6F9D66): pow del EXE sin redondear a float; multiplica m_vecMoveSpeed y m_vecTurnSpeed (+0x44..+0x58)
            PowScaleVector3Cpp(&m_vecMoveSpeed.x, fSpeedFactor, CTimer::GetTimeStep());
            PowScaleVector3Cpp(&m_vecTurnSpeed.x, fSpeedFactor, CTimer::GetTimeStep());
            ApplyMoveForce(vecBuoyancyForce.x, vecBuoyancyForce.y, vecBuoyancyForce.z);
            ApplyTurnForce(vecBuoyancyForce, vecBuoyancyTurnPoint);
        }
    }
}
