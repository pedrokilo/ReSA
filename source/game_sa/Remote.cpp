#include "StdInc.h"

#include "Remote.h"
#include "X87Math.h"

void CRemote::InjectHooks() {
    RH_ScopedClass(CRemote);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GivePlayerRemoteControlledCar, 0x45AB10);
    RH_ScopedInstall(TakeRemoteControlledCarFromPlayer, 0x45AE80);
}

// Campos del vehiculo y del jugador sin nombre en las cabeceras, por desplazamiento (como el desensamblado).
template<class T>
static T& RemoteField(void* base, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(base) + offset);
}

// 0x45AE80
// Como el original: cdecl, void, un argumento. Ninguno de los tres llamadores del EXE (0x56EA4B, 0x6B4BE8, 0x6B4C26) lee EAX/ECX/EDX tras la llamada.
// Cada acceso al jugador en foco y a su vehiculo remoto es una lectura nueva (el EXE vuelve a leer 0xB7CD74 y [jugador + 0xB0] en cada tramo), igual que las lecturas del pool.
// El vehiculo remoto se desreferencia sin comprobar (como el EXE).
void CRemote::TakeRemoteControlledCarFromPlayer(bool bCreateRemoteVehicleExplosion) {
    auto remoteVehicle = []() -> CVehicle*& { return CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle; };

    CVehicle* vehicle = remoteVehicle();                                  // 0x45AE80..0x45AE8D
    if (RemoteField<uint8>(vehicle, 0x4A4) == MISSION_VEHICLE) {          // m_nCreatedBy, 0x45AE93
        vehicle->SetVehicleCreatedBy(RANDOM_VEHICLE);                     // 0x6D5D70(this = vehiculo, 1)

        // 0x45AEA4..0x45AEE4: handle del pool de vehiculos (indice por division con signo del desplazamiento respecto al almacenamiento, sin comprobar pertenencia).
        auto* const pool = reinterpret_cast<uint8**>(GetVehiclePool());   // [0xB74494]: [0] = almacenamiento, [1] = estados de los huecos
        CVehicle* const slotVehicle = remoteVehicle();
        const int32 index = static_cast<int32>(intptr_t(slotVehicle) - intptr_t(pool[0])) / 0xA18;
        const int32 handle = static_cast<int32>(pool[1][index]) + static_cast<int32>(static_cast<uint32>(index) << 8);
        CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_VEHICLE); // 0x4654B0(0xA90850, handle, 1)
    }

    RemoteField<uint8>(remoteVehicle(), 0x428) &= 0xF7;                  // 0x45AEEA..0x45AEFD

    // 0x45AF04..0x45AF2E: cuatro escrituras en el jugador en foco (offsets 0xDD, 0xDE, 0xDF y 0xE0 de CPlayerInfo).
    CPlayerInfo& player = CWorld::Players[CWorld::PlayerInFocus];
    const uint32 time = CTimer::GetTimeInMS();
    player.m_nTimeOfRemoteVehicleExplosion = time;
    player.m_bAfterRemoteVehicleExplosion = true;
    RemoteField<uint8>(&player, 0xDE) = std::bit_cast<uint8>(bCreateRemoteVehicleExplosion); // el EXE copia el byte tal cual
    player.m_bFadeAfterRemoteVehicleExplosion = true;
}

// 0x45AB10
// Como el original: cdecl, void, cinco dwords (x, y, z, rotacion y el modelo, del que solo se lee la palabra baja SIN signo: movzx). El EXE crea el vehiculo con el operador new del
// pool (0x6E2D50, tamano ignorado) y el constructor segun el modelo: helicoptero (0xA18, 0x6C4190), avion (0xA04, 0x6C8E20) o automovil (0x988, 0x6B0A90). Si el operador new
// devuelve nulo el EXE sigue con un vehiculo nulo y falla al leer [vehiculo + 0x14]: ese camino no se cubre.
void CRemote::GivePlayerRemoteControlledCar(CVector pos, float rotation, int16 modelId) {
    const int32 model = static_cast<uint16>(modelId); // movzx esi, word ptr [esp + 0x2C]

    CVehicle* vehicle;
    if (CModelInfo::IsHeliModel(model)) {                                                   // 0x4C5B00
        void* const memory = CVehicle::operator new(0xA18u);                                // 0x6E2D50
        vehicle = memory ? reinterpret_cast<CVehicle*(__thiscall*)(void*, int32, int32)>(0x6C4190)(memory, model, MISSION_VEHICLE) : nullptr;
    } else if (CModelInfo::IsPlaneModel(model)) {                                           // 0x4C5B30
        void* const memory = CVehicle::operator new(0xA04u);
        vehicle = memory ? reinterpret_cast<CVehicle*(__thiscall*)(void*, int32, int32)>(0x6C8E20)(memory, model, MISSION_VEHICLE) : nullptr;
    } else {
        void* const memory = CVehicle::operator new(0x988u);
        vehicle = memory ? reinterpret_cast<CVehicle*(__thiscall*)(void*, int32, int32, int32)>(0x6B0A90)(memory, model, MISSION_VEHICLE, 1) : nullptr;
    }

    // 0x45ABC8..0x45AC4E: z = suelo bajo (x, y, z + 2.0f) + distancia del centro de masa a la base; x e y se copian como enteros (sin pasar por la x87).
    const float probeZ = pos.z + 2.0f;
    bool groundFound{};
    const float ground = CWorld::FindGroundZFor3DCoord(CVector{ pos.x, pos.y, probeZ }, &groundFound, nullptr); // 0x5696C0
    const float baseDistance = vehicle->GetDistanceFromCentreOfMassToBaseOfModel();                              // 0x536BE0
    float z;
    RemoteGiveGroundZCpp(ground, baseDistance, &z); // fld/fadd/fstp: la suma queda en el registro x87 muerto T-1

    reinterpret_cast<CMatrix*>(RemoteField<void*>(vehicle, 0x14))->SetRotateZOnly(rotation); // 0x59B020 (sobre la matriz leida, aunque sea nula)
    if (void* const matrix = RemoteField<void*>(vehicle, 0x14)) {
        RemoteField<uint32>(matrix, 0x30) = std::bit_cast<uint32>(pos.x);
        RemoteField<uint32>(RemoteField<void*>(vehicle, 0x14), 0x34) = std::bit_cast<uint32>(pos.y);
        RemoteField<float>(RemoteField<void*>(vehicle, 0x14), 0x38) = z;
    } else {
        RemoteField<uint32>(vehicle, 0x04) = std::bit_cast<uint32>(pos.x);
        RemoteField<uint32>(vehicle, 0x08) = std::bit_cast<uint32>(pos.y);
        RemoteField<float>(vehicle, 0x0C) = z;
    }

    // 0x45AC51..0x45ACBE
    RemoteField<uint8>(vehicle, 0x36) = (RemoteField<uint8>(vehicle, 0x36) & 7) | 0x40; // estado 8 (STATUS_REMOTE_CONTROLLED), tipo intacto
    RemoteField<uint8>(vehicle, 0x428) |= 8;
    CCarCtrl::JoinCarWithRoadSystem(vehicle);                                           // 0x42F5A0
    const bool bit2 = (RemoteField<uint8>(vehicle, 0x42D) & 2) != 0;                    // se lee tras la llamada
    RemoteField<uint8>(vehicle, 0x3BA) = 0;
    RemoteField<uint8>(vehicle, 0x3BB) = 0;
    RemoteField<uint8>(vehicle, 0x3B9) = 0;
    RemoteField<uint32>(vehicle, 0x3C8) = 0x41100000u; // 9.0f
    RemoteField<uint8>(vehicle, 0x3D0) = 9;
    RemoteField<uint8>(vehicle, 0x3B7) = 0;
    RemoteField<uint8>(vehicle, 0x3B8) = 0;
    if (bit2) {
        RemoteField<uint8>(vehicle, 0x428) &= 0xEF;
    } else {
        RemoteField<uint8>(vehicle, 0x428) |= 0x10;
    }

    CWorld::Add(vehicle); // 0x563220
    if (FindPlayerVehicle(-1, false)) { // 0x56E0D0 dos veces: el EXE vuelve a llamarla para el vehiculo del jugador
        CVehicle* const playerVehicle = FindPlayerVehicle(-1, false);
        RemoteField<uint8>(playerVehicle, 0x36) = (RemoteField<uint8>(playerVehicle, 0x36) & 7) | 0x48; // estado 9
    }

    // 0x45ACE9..0x45AD1E
    CPlayerInfo& player = CWorld::Players[CWorld::PlayerInFocus];
    player.m_pRemoteVehicle = vehicle;
    vehicle->RegisterReference(reinterpret_cast<CEntity**>(&player.m_pRemoteVehicle)); // 0x571B70
    TheCamera.TakeControl(vehicle, MODE_CAM_ON_A_STRING, eSwitchType::INTERPOLATION, 1); // 0x50C7C0(camara, vehiculo, 0x12, 1, 1)
    TheCamera.SetZoomValueCamStringScript(1);                                            // 0x50C1B0
}
