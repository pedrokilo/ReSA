#include "StdInc.h"

#include "World.h"
#include "EntryExitManager.h"
#include "X87Math.h"

// inlined
CPlayerInfo& FindPlayerInfo(int32 playerId) {
    return CWorld::Players[playerId < 0 ? CWorld::PlayerInFocus : playerId];
}

// Returns player ped
// 0x56E210
CPlayerPed* FindPlayerPed(int32 playerId) {
    return FindPlayerInfo(playerId).m_pPed;
}

// Returns player vehicle
// 0x56E0D0
CVehicle* FindPlayerVehicle(int32 playerId, bool bIncludeRemote) {
    // Como el original: el vehiculo remoto sale de la MISMA ranura de Players[] que el jugador (no se busca por el ped)
    auto& info = FindPlayerInfo(playerId);
    CPlayerPed* player = info.m_pPed;
    if (!player || !player->bInVehicle)
        return nullptr;

    if (bIncludeRemote && info.m_pRemoteVehicle) {
        return info.m_pRemoteVehicle;
    }

    return player->m_pVehicle;
}

// Returns player coors
// 0x56E010
CVector FindPlayerCoors(int32 playerId) {
    CEntity* entity = FindPlayerEntity(playerId);
    return entity ? entity->GetPosition() : CVector{};
}

// Returns player speed
// 0x56E090
CVector& FindPlayerSpeed(int32 playerId) {
    return static_cast<CPhysical*>(FindPlayerEntity(playerId))->m_vecMoveSpeed;
}

// Returns player ped or player vehicle if he's driving
// 0x56E120
CEntity* FindPlayerEntity(int32 playerId) {
    auto player = FindPlayerPed(playerId);
    if (!player)
        return nullptr;

    if (player->IsInVehicle())
        return player->m_pVehicle;

    return player;
}

// Returns player train
// 0x56E160
CTrain* FindPlayerTrain(int32 playerId) {
    auto vehicle = FindPlayerVehicle(playerId);
    if (vehicle && vehicle->IsTrain())
        return vehicle->AsTrain();
    else
        return nullptr;
}

// Returns player coords
// 0x56E250
const CVector& FindPlayerCentreOfWorld(int32 playerId) {
    if (CCarCtrl::bCarsGeneratedAroundCamera)
        return TheCamera.GetPosition();

    // Como el original: el vehiculo remoto se mira SIN exigir que el jugador este en un vehiculo, y el vehiculo normal
    // se busca siempre en el jugador en foco (FindPlayerVehicle(-1)), no en `playerId`
    auto& info = FindPlayerInfo(playerId);
    if (CVehicle* remote = info.m_pRemoteVehicle)
        return remote->GetPosition();

    if (CVehicle* vehicle = FindPlayerVehicle(-1, false))
        return vehicle->GetPosition();

    return info.m_pPed->GetPosition();
}

// Returns player coords with skipping sniper shift
// 0x56E320
const CVector& FindPlayerCentreOfWorld_NoSniperShift(int32 playerId) {
    if (CCarCtrl::bCarsGeneratedAroundCamera) {
        return TheCamera.GetPosition();
    }
    // Como el original: el vehiculo remoto y el ped final se leen con el indice SIN resolver (playerId crudo, sin
    // sustituir los negativos por PlayerInFocus); solo la comprobacion del vehiculo normal usa el indice resuelto
    auto* const rawInfo = reinterpret_cast<CPlayerInfo*>(reinterpret_cast<uint8*>(CWorld::Players) + static_cast<ptrdiff_t>(playerId) * static_cast<ptrdiff_t>(sizeof(CPlayerInfo)));
    if (const auto* const veh = rawInfo->m_pRemoteVehicle) {
        return veh->GetPosition();
    }
    if (const auto* const veh = FindPlayerVehicle(playerId, false)) {
        return veh->GetPosition();
    }
    if (const auto* const player = rawInfo->m_pPed) {
        return player->GetPosition();
    }
    return StaticRef<CVector>(0xB9B970); // vector global a cero del EXE
}

// Returns player coords with skipping interior shift
// 0x56E400
CVector FindPlayerCentreOfWorldForMap(int32 playerId) {
    CVector centre = FindPlayerCentreOfWorld(playerId);
    CEntryExitManager::GetPositionRelativeToOutsideWorld(centre);
    return centre;
}

// Como el original: el rumbo es atan2(-adelante.x, adelante.y) con fpatan (TrigAtan2Cpp) y, sin matriz, el rumbo compacto de la entidad;
// el vehiculo remoto y el ped de reserva se leen con el indice crudo (sin resolver el negativo) y el vehiculo normal con el indice resuelto
static float EntityHeading(const CPlaceable* entity) {
    if (const auto* const matrix = entity->m_matrix) {
        const CVector& forward = matrix->GetForward();
        return TrigAtan2Cpp(-forward.x, forward.y);
    }
    return entity->m_placement.m_fHeading;
}

// Returns player angle in radians
// 0x56E450
float FindPlayerHeading(int32 playerId) {
    const auto rawInfo = [](int32 index) {
        return reinterpret_cast<const CPlayerInfo*>(reinterpret_cast<uintptr_t>(&CWorld::Players[0]) + static_cast<uintptr_t>(index) * sizeof(CPlayerInfo));
    };
    const CPlayerInfo* const info = rawInfo(playerId);
    if (const auto* const remote = info->m_pRemoteVehicle) {
        return EntityHeading(remote);
    }
    const auto* const resolved = rawInfo(playerId < 0 ? static_cast<int32>(CWorld::PlayerInFocus) : playerId);
    if (const auto* const ped = resolved->m_pPed; ped && ped->bInVehicle && ped->m_pVehicle) {
        return EntityHeading(ped->m_pVehicle);
    }
    return EntityHeading(info->m_pPed);
}

// Returns Z coord for active player
// NOTSA: Added playerId param
// 0x56E520
float FindPlayerHeight(int32 playerId) {
    return FindPlayerPed(playerId)->GetPosition().z;
}

// Returns player wanted
// 0x56E230
CWanted* FindPlayerWanted(int32 playerId) {
    return FindPlayerInfo(playerId).m_PlayerData.m_pWanted;
}

// Returns player's group
CPedGroup& FindPlayerGroup(int32 playerId) {
    return FindPlayerPed(playerId)->GetPlayerGroup();
}
