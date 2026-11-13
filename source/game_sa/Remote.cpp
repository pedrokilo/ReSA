/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRemote
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Remote.h"
#include "X87Math.h"

void CRemote::InjectHooks() {
    RH_ScopedClass(CRemote);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GivePlayerRemoteControlledCar, 0x45AB10);
    RH_ScopedInstall(TakeRemoteControlledCarFromPlayer, 0x45AE80);
    RH_ScopedInstall(SetPlayerRemoteControlledCar, 0x45AD40); // lote 181A (sin validar)
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

// 0x45AD40 (lote 181A, sin validar; el listado decia CVehicleRecording; la clase sale del solape con 0x45AB10/0x45AE80 y de su unico llamador, 0x46D560, el comando de script que entrega a CRemote un vehiculo ya existente)
// Como el original: cdecl, void, un argumento (el vehiculo), `ret`, 316 bytes. Cada acceso al jugador en foco es una lectura nueva de 0xB7CD74 (el EXE la repite en cada tramo).
// 1) Si el jugador ya tiene vehiculo remoto (+0xB0 del jugador en foco): su estado pasa a 3 (byte +0x36 = (&7)|0x18) y, si no es el mismo vehiculo (se relee el campo), se recuerda con una bandera local.
// 2) El vehiculo nuevo: estado 8 (STATUS_REMOTE_CONTROLLED, tipo intacto), +0x428 |= 8, a cero +0x3BA, +0x3BB, +0x3B9, +0x3C8 = 9.0f, +0x3D0 = 9, +0x3B7 y +0x3B8 a cero; el bit 0x10 de +0x428 se pone o quita
//    segun el bit 2 de +0x42D (leido ANTES de las escrituras): bit puesto -> se quita, no puesto -> se pone. No llama a JoinCarWithRoadSystem ni a CWorld::Add.
// 3) Si FindPlayerVehicle(-1, false) (0x56E0D0) no es nulo, se vuelve a llamar (sin comprobar el nulo) y su estado pasa a 9 ((&7)|0x48).
// 4) Si el jugador tenia vehiculo remoto, CEntity::CleanUpOldReference (0x571A00, this = el anterior, argumento = la direccion del campo); despues el campo se asigna y se registra con RegisterReference (0x571B70, this = el nuevo).
// 5) TheCamera.TakeControl(vehiculo, 0x12, bandera ? JUMPCUT(2) : INTERPOLATION(1), 1) (0x50C7C0) y SetZoomValueCamStringScript(1) (0x50C1B0).
void CRemote::SetPlayerRemoteControlledCar(CVehicle* vehicle) {
    bool hadOtherRemoteVehicle = false; // byte local [esp+0xB]
    if (CVehicle* const current = CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle) { // 0x45AD41..0x45AD57
        RemoteField<uint8>(current, 0x36) = (RemoteField<uint8>(current, 0x36) & 7) | 0x18;  // 0x45AD64..0x45AD6D
        if (CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle != vehicle) {            // 0x45AD70..0x45AD83
            hadOtherRemoteVehicle = true;
        }
    }

    // 0x45AD8A..0x45ADE8: se leen +0x36, +0x428 y +0x42D (en ese orden) tras el tramo anterior (el vehiculo puede ser el mismo).
    const uint8 statusByte = RemoteField<uint8>(vehicle, 0x36);
    const uint8 flags428   = RemoteField<uint8>(vehicle, 0x428);
    const uint8 flags42D   = RemoteField<uint8>(vehicle, 0x42D);
    RemoteField<uint8>(vehicle, 0x36)  = (statusByte & 7) | 0x40;                       // estado 8, tipo intacto
    const uint8 newFlags428 = flags428 | 8;
    RemoteField<uint8>(vehicle, 0x428) = newFlags428;
    RemoteField<uint8>(vehicle, 0x3BA) = 0;
    RemoteField<uint8>(vehicle, 0x3BB) = 0;
    RemoteField<uint8>(vehicle, 0x3B9) = 0;
    RemoteField<uint32>(vehicle, 0x3C8) = 0x41100000u; // 9.0f
    RemoteField<uint8>(vehicle, 0x3D0) = 9;
    RemoteField<uint8>(vehicle, 0x3B7) = 0;
    RemoteField<uint8>(vehicle, 0x3B8) = 0;
    RemoteField<uint8>(vehicle, 0x428) = (flags42D & 2) ? (newFlags428 & 0xEF) : (newFlags428 | 0x10);

    if (FindPlayerVehicle(-1, false)) { // 0x56E0D0 dos veces: el EXE vuelve a llamarla y no comprueba el nulo la segunda vez
        CVehicle* const playerVehicle = FindPlayerVehicle(-1, false);
        RemoteField<uint8>(playerVehicle, 0x36) = (RemoteField<uint8>(playerVehicle, 0x36) & 7) | 0x48; // estado 9
    }

    // 0x45AE11..0x45AE4C: el campo se vuelve a leer y su direccion se recalcula con el jugador en foco.
    CVehicle* const previous = CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle;
    if (previous) {
        previous->CleanUpOldReference(reinterpret_cast<CEntity**>(&CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle)); // 0x571A00
    }
    CPlayerInfo& player = CWorld::Players[CWorld::PlayerInFocus];
    player.m_pRemoteVehicle = vehicle;
    vehicle->RegisterReference(reinterpret_cast<CEntity**>(&player.m_pRemoteVehicle)); // 0x571B70

    TheCamera.TakeControl(vehicle, MODE_CAM_ON_A_STRING, hadOtherRemoteVehicle ? eSwitchType::JUMPCUT : eSwitchType::INTERPOLATION, 1); // 0x50C7C0(camara, vehiculo, 0x12, 2 o 1, 1)
    TheCamera.SetZoomValueCamStringScript(1);                                                                                             // 0x50C1B0
}
