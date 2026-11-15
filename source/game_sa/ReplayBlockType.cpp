#include "StdInc.h"
#include "ReplayBlockType.h"
#include "Replay.h"
#include "X87Math.h"

// Lote 213A (sin validar): las escrituras de paquetes de vehiculo del EXE (0x45B730 coche, 0x45BF90 BMX, 0x45C070 moto) viven en Replay.cpp (ReplayStoreCarUpdate y ReplayStoreBikeUpdate, las que usa RecordThisFrame); aqui
// se quitaron sus copias sin uso (MakeVehicleUpdateData, MakeBikeUpdateData, MakeBmxUpdateData y MakeTrainUpdateData), que no eran fieles y duplicaban esas rutinas.

namespace {
// Constantes float del EXE usadas por la extraccion (todas se multiplican, el EXE no divide): 0x859C48 = 0.000125f (1/8000), 0x858B38 = 0.02f (1/50), 0x858C58 = 0.01f (1/100), 0x859C40 = 0.049448822f (1/20.222929).
// REPLAY_HEADING_DECOMPRESS (0x859C44) esta en Replay.h.
constexpr float REPLAY_SPEED_DECOMPRESS      = std::bit_cast<float>(0x3903126Fu); // 0x859C48
constexpr float REPLAY_FIFTIETH              = std::bit_cast<float>(0x3CA3D70Au); // 0x858B38
constexpr float REPLAY_HUNDREDTH             = std::bit_cast<float>(0x3C23D70Au); // 0x858C58
constexpr float REPLAY_DOOR_ANGLE_DECOMPRESS = std::bit_cast<float>(0x3D4A8AD9u); // 0x859C40
} // namespace

// 0x45BA90 (lote 213A, sin validar; 1236 bytes, marco SEH solo para las dos CMatrix locales): thiscall, ECX = paquete, vehiculo y factor de interpolacion (float) en la pila, `ret 8`; el wrapper del EXE (Replay.cpp) ya descarta el
// vehiculo nulo. Todos los bytes con signo del paquete se leen con movsx; los factores se MULTIPLICAN por la constante float del EXE (nunca se divide). Orden del EXE:
//  1. matriz descomprimida del paquete en una CMatrix local (DecompressIntoFullMatrix 0x59B9F0), copia de la matriz del vehiculo (CMatrix(const CMatrix&) 0x59BCF0); copia *= (1 - t) y descomprimida *= t con ScaleAll del EXE (0x45AF40,
//     x87: MatrixScaleAllCpp), suma (operator+= 0x59ADF0) y operator= (0x59BBC0) sobre la matriz del vehiculo (como CMatrix, sin copiar el CMatrixLink);
//  2. velocidad de giro a cero, vida = float(vida * 4), pedal = float(pedal) * 0.01f, paneles (CReplay::ApplyPanelDamageToCar 0x59CA10, create=true) solo en automoviles (tipo de vehiculo 0), velocidad de movimiento (x, y, z);
//  3. banderas de la entidad (+0x1C): bit 7 <- bit 1, bit 30 <- bit 2, bit 31 <- bit 3 de physicalFlags (tres pasos que releen el valor anterior);
//  4. direccion: rhino (modelo 0x1B0) -> m_fSteerAngle = 0 y rotacion de la torreta (+0x94C) = valor * 0x859C44; resto -> valor * 0.02f;
//  5. automoviles: pares (compresion de suspension * 0.02f, rotacion de rueda * 0x859C44), angulos de puerta delantera izquierda y derecha (* 0x859C40, copiados tambien a m_prevAngle), puertas abiertas (si el angulo no es cero se
//     escribe 1 en el estado de puerta 2 y 3 del CDamageManager sin mirar el estado previo) y puertas arrancadas segun doorStatus (si GetDoorStatus != NOTPRESENT: SetDoorStatus(NOTPRESENT) y SetDoorDamage(puerta, true));
//  6. bEngineOn = !bEngineBroken (todos los tipos), m_NumDriveWheelsOnGround = 4 (automoviles), area = CGame::currArea, CWorld::Remove y CWorld::Add, y bit 29 de las banderas fisicas (+0x40) <- bit 0 de physicalFlags.
void tReplayVehicleBlock::ExtractVehicleUpdateData(CVehicle& veh, float interpolation) {
    CMatrix decoded;
    matrix.DecompressIntoFullMatrix(decoded);
    CMatrix current{ static_cast<const CMatrix&>(veh.GetMatrix()) };
    MatrixScaleAllCpp(&current.GetRight().x, 1.0f - interpolation);
    MatrixScaleAllCpp(&decoded.GetRight().x, interpolation);
    current += decoded;
    static_cast<CMatrix&>(veh.GetMatrix()) = current;

    veh.GetTurnSpeed() = CVector{0.0f};
    veh.m_fHealth = (float)(int32(health) * 4);
    veh.m_GasPedal = (float)(int32)gasPedal * REPLAY_HUNDREDTH;
    const bool isAutomobile = veh.IsAutomobile();
    if (isAutomobile) {
        CReplay::ApplyPanelDamageToCar(panels, veh.AsAutomobile(), true); // 0x59CA10 (lote 178A, sin validar)
    }

    veh.GetMoveSpeed().x = (float)(int32)vecMoveSpeed.x * REPLAY_SPEED_DECOMPRESS;
    veh.GetMoveSpeed().y = (float)(int32)vecMoveSpeed.y * REPLAY_SPEED_DECOMPRESS;
    veh.GetMoveSpeed().z = (float)(int32)vecMoveSpeed.z * REPLAY_SPEED_DECOMPRESS;

    {
        const uint32 pf = physicalFlags;
        uint32 a = veh.m_nFlags;
        a ^= ((pf << 6) ^ a) & 0x80u;
        veh.m_nFlags = a;
        const uint32 b = a ^ (((pf << 28) ^ a) & 0x40000000u);
        veh.m_nFlags = b;
        veh.m_nFlags = (((pf << 28) ^ b) & 0x7FFFFFFFu) ^ (pf << 28);
    }

    if (veh.m_nModelIndex == MODEL_RHINO) {
        veh.m_fSteerAngle = 0.0f;
        veh.AsAutomobile()->m_fDoomVerticalRotation = (float)(int32)(int8)steerAngle_or_doomVerticalRot * REPLAY_HEADING_DECOMPRESS;
    } else {
        veh.m_fSteerAngle = (float)(int32)(int8)steerAngle_or_doomVerticalRot * REPLAY_FIFTIETH;
    }

    if (isAutomobile) {
        auto* const automobile = veh.AsAutomobile();

        for (auto i = 0; i < 4; i++) { // para cada rueda: el EXE alterna compresion y rotacion
            automobile->m_fWheelsSuspensionCompression[i] = (float)(int32)(int8)wheelsSuspensionCompression[i] * REPLAY_FIFTIETH;
            automobile->m_wheelRotation[i]                = (float)(int32)(int8)wheelRotation[i] * REPLAY_HEADING_DECOMPRESS;
        }
        automobile->m_doors[DOOR_LEFT_FRONT].m_angle = (float)(int32)(int8)angleDoorLF * REPLAY_DOOR_ANGLE_DECOMPRESS;
        automobile->m_doors[DOOR_LEFT_FRONT].m_prevAngle = automobile->m_doors[DOOR_LEFT_FRONT].m_angle;
        automobile->m_doors[DOOR_RIGHT_FRONT].m_angle = (float)(int32)(int8)angleDoorRF * REPLAY_DOOR_ANGLE_DECOMPRESS;
        automobile->m_doors[DOOR_RIGHT_FRONT].m_prevAngle = automobile->m_doors[DOOR_RIGHT_FRONT].m_angle;

        auto& damageManager = automobile->m_damageManager;
        if (angleDoorLF != 0) {
            damageManager.m_aDoorsStatus[DOOR_LEFT_FRONT] = DAMSTATE_OPENED;
        }
        if (angleDoorRF != 0) {
            damageManager.m_aDoorsStatus[DOOR_RIGHT_FRONT] = DAMSTATE_OPENED;
        }

        for (auto i = 0; i < MAX_DOORS; i++) {
            const auto door = (eDoors)i;
            if ((doorStatus & (1u << i)) && damageManager.GetDoorStatus(door) != DAMSTATE_NOTPRESENT) {
                damageManager.SetDoorStatus(door, DAMSTATE_NOTPRESENT);
                automobile->SetDoorDamage(door, true);
            }
        }
    }

    veh.vehicleFlags.bEngineOn = !veh.vehicleFlags.bEngineBroken;
    if (isAutomobile) {
        veh.AsAutomobile()->m_NumDriveWheelsOnGround = 4;
    }
    veh.SetAreaCode(static_cast<eAreaCodes>(CGame::currArea));

    CWorld::Remove(&veh);
    CWorld::Add(&veh);
    veh.m_nPhysicalFlags ^= (veh.m_nPhysicalFlags ^ ((uint32)physicalFlags << 29)) & 0x20000000u;
}

// 0x45C0D0 (moto) y 0x45BFF0 (BMX), lote 213A (sin validar; 116 bytes cada una, mismos bytes): thiscall, ECX = paquete, vehiculo y factor (float) en la pila, `ret 8`. Llaman a la extraccion de vehiculo (0x45BA90) y despues, para cada
// angulo, calculan float(byte con signo) * 0.02f ANTES de la llamada virtual GetRideAnimData (vtable +0xBC, dos llamadas, una por angulo): BarSteerAngle (+4) <- steerAngle (+0x35), LeanAngle (+8) <- animLean (+0x34).
// Despues m_bLeanMatrixCalculated (+0x5C8) = 0 y CBike::CalculateLeanMatrix (0x6B7150, con hook).
void tReplayBikeBlock::ExtractBikeUpdateData(CBike& bike, float interpolation) {
    As<tReplayVehicleBlock>()->ExtractVehicleUpdateData(static_cast<CVehicle&>(bike), interpolation);

    const float barSteer = (float)(int32)(int8)steerAngle * REPLAY_FIFTIETH;
    bike.GetRideAnimData()->BarSteerAngle = barSteer;
    const float lean = (float)(int32)(int8)animLean * REPLAY_FIFTIETH;
    bike.GetRideAnimData()->LeanAngle = lean;
    bike.m_bLeanMatrixCalculated = false;
    bike.CalculateLeanMatrix();
}

// 0x45BFF0 (lote 213A, sin validar): mismos bytes que 0x45C0D0 y mismo reparto de campos (animLean +0x34, steerAngle +0x35); el BMX es una CBike.
void tReplayBmxBlock::ExtractBmxUpdateData(CBmx& bmx, float interpolation) {
    reinterpret_cast<tReplayBikeBlock*>(this)->ExtractBikeUpdateData(static_cast<CBike&>(bmx), interpolation);
}

// 0x45CDB0 (lote 214A, sin validar; el listado decia CReplay): thiscall (ECX = paquete de tren), dos argumentos de pila (CTrain*, interpolacion float), `ret 8`, 226 bytes.
// Primero ExtractVehicleUpdateData (0x45BA90, sin hook); luego copia velocidad, distancia de via y longitud (dwords), el byte de via (+0x5C0) y, si el vagon anterior / siguiente del
// paquete no es 0, limpia la referencia vieja (solo si habia), busca el vagon con la tabla de conversion de CReplay (FindPoolIndexForVehicle, ref - 1) leyendo el pool en crudo como el EXE
// (sin comprobar limites; bit 0x80 del mapa = libre -> nullptr), lo guarda y registra la referencia (solo si no es nulo). Orden del EXE: anterior y luego siguiente.
void tReplayTrainBlock::ExtractTrainUpdateData(CTrain& train, float interpolation) {
    static_assert(offsetof(CTrain, m_fTrainSpeed) == 0x5A4 && offsetof(CTrain, m_fCurrentRailDistance) == 0x5A8 && offsetof(CTrain, m_fLength) == 0x5AC);
    static_assert(offsetof(CTrain, m_nTrackId) == 0x5C0 && offsetof(CTrain, m_pPrevCarriage) == 0x5D0 && offsetof(CTrain, m_pNextCarriage) == 0x5D4);
    static_assert(offsetof(tReplayTrainBlock, trainSpeed) == 0x34 && offsetof(tReplayTrainBlock, prevCarriageRef) == 0x40 && offsetof(tReplayTrainBlock, trackId) == 0x48);

    As<tReplayVehicleBlock>()->ExtractVehicleUpdateData(*train.AsVehicle(), interpolation);

    train.m_fTrainSpeed = trainSpeed;
    train.m_fCurrentRailDistance = currentRailDistance;
    train.m_fLength = length;
    train.m_nTrackId = trackId;

    // Pool de vehiculos en crudo como el EXE: objetos en +0 (0xA18 bytes por ranura), mapa de estado en +4 (bit 0x80 = libre).
    const auto findCarriage = [](uint32 ref) -> CTrain* {
        const int32 slot = CReplay::FindPoolIndexForVehicle(static_cast<int32>(ref) - 1);
        auto* const pool = GetVehiclePool();
        auto* const objects = reinterpret_cast<uint8**>(pool)[0];
        auto* const flags = reinterpret_cast<int8**>(pool)[1];
        return flags[slot] < 0 ? nullptr : reinterpret_cast<CTrain*>(objects + slot * 0xA18);
    };

    if (prevCarriageRef) {
        CEntity::SafeCleanUpRef(train.m_pPrevCarriage);
        train.m_pPrevCarriage = findCarriage(prevCarriageRef);
        CEntity::SafeRegisterRef(train.m_pPrevCarriage);
    }

    if (nextCarriageRef) {
        CEntity::SafeCleanUpRef(train.m_pNextCarriage);
        train.m_pNextCarriage = findCarriage(nextCarriageRef);
        CEntity::SafeRegisterRef(train.m_pNextCarriage);
    }
}
