/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: cHandlingDataMgr
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "cHandlingDataMgr.h"
#include "X87Math.h"

auto& VehicleNames = StaticRef<char[210][14]>(0x8D3978);

//! 0xC2B9B4 - Used for acceleration conversion from file to game units
constexpr auto ACCEL_CONST = 1.f / sq(50.f); // This number 50 seems to be coming up a lot...;

// 0xC2B9BC - Used for velocity conversion from file to game units
constexpr auto VELOCITY_CONST = 0.277778f / 50.f;

void cHandlingDataMgr::InjectHooks() {
    RH_ScopedClass(cHandlingDataMgr);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(LoadHandlingData, 0x5BD830);
    RH_ScopedInstall(GetFlyingPointer, 0x6F52D0);
    RH_ScopedInstall(GetBoatPointer, 0x6F5300);
    RH_ScopedInstall(HasFrontWheelDrive, 0x6A0480);
    RH_ScopedInstall(HasRearWheelDrive, 0x6A04B0);
    RH_ScopedInstall(GetHandlingId, 0x6F4FD0);
    RH_ScopedInstall(ConvertDataToWorldUnits, 0x6F5010);
    RH_ScopedInstall(ConvertDataToGameUnits, 0x6F5080);
    RH_ScopedInstall(ConvertBikeDataToWorldUnits, 0x6F5240);
    RH_ScopedInstall(ConvertBikeDataToGameUnits, 0x6F5290);
    RH_ScopedInstall(FindExactWord, 0x006F4F30);
};

// NOTSA: Helper
int32 tHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const auto n = sscanf_s(
        line,
        "%*s\t%f\t%f\t%f\t%f\t%f\t%f\t%hhu\t%f\t%f\t%f\t%hhu\t%f\t%f\t%f\t%c\t%c\t%f\t%f\t%hhu\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\t%x\t%x\t%hhu\t%hhu\t%hhu",
        &m_fMass, // 1
        &m_fTurnMass,
        &m_fDragMult,

        &m_vecCentreOfMass.x, // 4
        &m_vecCentreOfMass.y,
        &m_vecCentreOfMass.z,

        &m_nPercentSubmerged, // 7

        &m_fTractionMultiplier, // 8
        &m_fTractionLoss,
        &m_fTractionBias,

        &m_transmissionData.m_nNumberOfGears, // 11
        &m_transmissionData.m_MaxVelocity,
        &m_transmissionData.m_EngineAcceleration,
        &m_transmissionData.m_EngineInertia,
        &m_transmissionData.m_nDriveType, 1u,
        &m_transmissionData.m_nEngineType, 1u,

        &m_fBrakeDeceleration, // 18
        &m_fBrakeBias,
        &m_bABS,
        &m_fSteeringLock,
        &m_fSuspensionForceLevel,

        &m_fSuspensionDampingLevel, // 22
        &m_fSuspensionHighSpdComDamp,
        &m_fSuspensionUpperLimit,
        &m_fSuspensionLowerLimit,
        &m_fSuspensionBiasBetweenFrontAndRear,
        &m_fSuspensionAntiDiveMultiplier,

        &m_fSeatOffsetDistance,
        &m_fCollisionDamageMultiplier,

        &m_nMonetaryValue, // 31
        &m_nModelFlags,
        &m_nHandlingFlags,
        &m_nFrontLights,
        &m_nRearLights,
        &m_nAnimGroup
    );
    m_transmissionData.m_handlingFlags = m_nHandlingFlags;
    m_transmissionData.m_EngineAcceleration *= 0.4f;
    gHandlingDataMgr.ConvertDataToGameUnits(this);
    return n == 35 ? -1 : n;
}

// NOTSA: Helper
int32 tBoatHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const auto n = sscanf_s(
        line,
        "%*s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
        &m_fThrustY,
        &m_fThrustZ,
        &m_fThrustAppZ,
        &m_fAqPlaneForce,
        &m_fAqPlaneLimit,
        &m_fAqPlaneOffset,
        &m_fWaveAudioMult,

        &m_vecMoveRes.x,
        &m_vecMoveRes.y,
        &m_vecMoveRes.z,

        &m_vecTurnRes.x,
        &m_vecTurnRes.y,
        &m_vecTurnRes.z,

        &m_fLookLRBehindCamHeight
    );
    return n == 14 ? -1 : n;
}

// NOTSA: Helper
int32 tFlyingHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const auto n = sscanf_s(
        line,
        "%*s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f%*c\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
        &m_fThrust, // 1                                            ^^^ there's an extra `s` for the `RCRAIDER` that we have to ignore
        &m_fThrustFallOff,
        &m_fYaw,
        &m_fYawStab,
        &m_fSideSlip, // 5
        &m_fRoll,
        &m_fRollStab,
        &m_fPitch,
        &m_fPitchStab,
        &m_fFormLift, // 10
        &m_fAttackLift,
        &m_fGearUpR,
        &m_fGearDownL,
        &m_fWindMult,
        &m_fMoveRes, // 15

        &m_vecTurnRes.x, // 16
        &m_vecTurnRes.y,
        &m_vecTurnRes.z,

        &m_vecSpeedRes.x, // 19
        &m_vecSpeedRes.y,
        &m_vecSpeedRes.z
    );
    return n == 21 ? -1 : n;
}

// NOTSA: Helper
int32 tBikeHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const auto n = sscanf_s(
        line,
        "%*s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
        &m_fLeanFwdCOM,
        &m_fLeanFwdForce,
        &m_fLeanBakCOM,
        &m_fLeanBakForce,
        &m_fMaxLean,
        &m_fFullAnimLean,
        &m_fDesLean,
        &m_fSpeedSteer,
        &m_fSlipSteer,
        &m_fNoPlayerCOMz,
        &m_fWheelieAng,
        &m_fStoppieAng,
        &m_fWheelieSteer,
        &m_fWheelieStabMult,
        &m_fStoppieStabMult
    );
    gHandlingDataMgr.ConvertBikeDataToGameUnits(this);
    return n == 15 ? -1 : n;
}

// Como el original: helpers del bucle de LoadHandlingData (registros por desplazamiento crudo, como el EXE)
namespace {
template<typename T>
T& HandlingField(void* record, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(record) + offset);
}
} // namespace

// 0x5BD830
void cHandlingDataMgr::LoadHandlingData() {
    CFileMgr::SetDir("DATA");
    const auto file = CFileMgr::OpenFile("HANDLING.CFG", "rb");
    CFileMgr::SetDir("");

    // Como el original: el registro actual de cada tipo persiste entre lineas (empieza nulo)
    uint8* vehicle = nullptr;
    uint8* boat    = nullptr;
    uint8* bike    = nullptr;
    uint8* flying  = nullptr;
    auto* const self  = reinterpret_cast<uint8*>(this);
    const auto  names = &VehicleNames[0][0];

    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (!memcmp(line, ";the end", 9)) { // Como el original: repe cmpsb de 9 bytes
            break;
        }
        if (line[0] == ';') {
            continue;
        }

        uint8 field = 0;
        switch (line[0]) {
        case '!': { // Moto: el primer token es "!", despues el nombre y 15 floats
            for (auto token = strtok(line, " \t"); token; token = strtok(nullptr, " \t"), field++) {
                if (field == 1) {
                    const auto id = FindExactWord(token, names, 14, 210);
                    bike = self + 0x8F54 + id * 0x40; // Como el original: sin restar VT_BIKE ni acotar
                    HandlingField<int32>(bike, 0) = id;
                } else if (field >= 2 && field <= 16) {
                    HandlingField<float>(bike, 4 * (field - 1)) = (float)atof(token);
                }
            }
            ConvertBikeDataToGameUnits(reinterpret_cast<tBikeHandlingData*>(bike));
            break;
        }
        case '$': { // Aviones/helicopteros: "$", nombre y 21 floats (sin conversion)
            for (auto token = strtok(line, " \t"); token; token = strtok(nullptr, " \t"), field++) {
                if (field == 1) {
                    const auto id = FindExactWord(token, names, 14, 210);
                    flying = self + 0x7B24 + id * 0x58;
                    HandlingField<int32>(flying, 0) = id;
                } else if (field >= 2 && field <= 22) {
                    HandlingField<float>(flying, 4 * (field - 1)) = (float)atof(token);
                }
            }
            break;
        }
        case '%': { // Barcos: "%", nombre y 14 floats; los campos 9 a 14 van a +0x24..+0x38 y el 15 a +0x20
            for (auto token = strtok(line, " \t"); token; token = strtok(nullptr, " \t"), field++) {
                if (field == 1) {
                    const auto id = FindExactWord(token, names, 14, 210);
                    boat = reinterpret_cast<uint8*>(GetBoatPointer((uint8)id));
                    HandlingField<int32>(boat, 0) = id;
                } else if (field >= 2 && field <= 8) {
                    HandlingField<float>(boat, 4 * (field - 1)) = (float)atof(token);
                } else if (field >= 9 && field <= 14) {
                    HandlingField<float>(boat, 4 * field) = (float)atof(token);
                } else if (field == 15) {
                    HandlingField<float>(boat, 0x20) = (float)atof(token);
                }
            }
            break;
        }
        case '^': { // Grupo de animaciones: se monta en locales y se copia a CVehicleAnimGroupData::m_vehicleAnimGroups
            int32 groupId = 0, firstGroup = 0, secondGroup = 0; // En el EXE quedan sin inicializar si faltan campos
            int32 animFlags = 0, specialFlags = 0;
            sVehAnimGroupGeneralTiming generalTiming;
            sVehAnimGroupInOutTiming   startTiming, endTiming;
            float* const general = reinterpret_cast<float*>(&generalTiming);
            float* const start   = reinterpret_cast<float*>(&startTiming);
            float* const end     = reinterpret_cast<float*>(&endTiming);
            for (auto token = strtok(line, " \t"); token; token = strtok(nullptr, " \t"), field++) {
                if (field == 1) {
                    groupId = atoi(token);
                } else if (field == 2) {
                    firstGroup = atoi(token);
                } else if (field == 3) {
                    secondGroup = atoi(token);
                } else if (field >= 4 && field <= 21) {
                    if (atoi(token)) {
                        animFlags |= 1 << (field - 4); // Como el original: (int)pow(2, field - 4)
                    }
                } else if (field >= 22 && field <= 26) {
                    general[field - 22] = (float)atof(token);
                } else if (field >= 27 && field <= 34) { // Como el original: alternan inicio y fin
                    ((field - 27) % 2 ? end : start)[(field - 27) / 2] = (float)atof(token);
                } else if (field == 35) {
                    specialFlags = atoi(token);
                }
            }
            CVehicleAnimGroup group;
            group.InitAnimGroup((uint8)(firstGroup + 0x58), (uint8)(secondGroup + 0x58), animFlags, specialFlags, &generalTiming, &startTiming, &endTiming);
            CVehicleAnimGroupData::m_vehicleAnimGroups[groupId].CopyAnimGroup(&group);
            break;
        }
        default: { // Coche: el primer token es el nombre
            for (auto token = strtok(line, " \t"); token; token = strtok(nullptr, " \t"), field++) {
                switch (field) {
                case 0: {
                    const auto id = FindExactWord(token, names, 14, 210);
                    vehicle = self + 0x14 + id * 0xE0;
                    HandlingField<int32>(vehicle, 0) = id;
                    break;
                }
                case 1:  HandlingField<float>(vehicle, 0x04) = (float)atof(token); break;
                case 2:  HandlingField<float>(vehicle, 0x0C) = (float)atof(token); break;
                case 3:  HandlingField<float>(vehicle, 0x10) = (float)atof(token); break;
                case 4:  HandlingField<float>(vehicle, 0x14) = (float)atof(token); break;
                case 5:  HandlingField<float>(vehicle, 0x18) = (float)atof(token); break;
                case 6:  HandlingField<float>(vehicle, 0x1C) = (float)atof(token); break;
                case 7:  HandlingField<uint8>(vehicle, 0x20) = (uint8)atoi(token); break;
                case 8:  HandlingField<float>(vehicle, 0x28) = (float)atof(token); break;
                case 9:  HandlingField<float>(vehicle, 0xA4) = (float)atof(token); break;
                case 10: HandlingField<float>(vehicle, 0xA8) = (float)atof(token); break;
                case 11: HandlingField<uint8>(vehicle, 0x76) = (uint8)atoi(token); break;
                case 12: HandlingField<float>(vehicle, 0x84) = (float)atof(token); break;
                case 13: HandlingField<float>(vehicle, 0x7C) = HandlingAtofTimes04Cpp(atof(token)); break; // Como el original: atof * 0.4 (double, 0x86A950), un solo redondeo (0x5BE2AC)
                case 14: HandlingField<float>(vehicle, 0x80) = (float)atof(token); break;
                case 15: HandlingField<char>(vehicle, 0x74) = token[0]; break;
                case 16: HandlingField<char>(vehicle, 0x75) = token[0]; break;
                case 17: HandlingField<float>(vehicle, 0x94) = (float)atof(token); break;
                case 18: HandlingField<float>(vehicle, 0x98) = (float)atof(token); break;
                case 19: HandlingField<uint8>(vehicle, 0x9C) = atoi(token) != 0; break;
                case 20: HandlingField<float>(vehicle, 0xA0) = (float)atof(token); break;
                case 21: HandlingField<float>(vehicle, 0xAC) = (float)atof(token); break;
                case 22: HandlingField<float>(vehicle, 0xB0) = (float)atof(token); break;
                case 23: HandlingField<float>(vehicle, 0xB4) = (float)atof(token); break;
                case 24: HandlingField<float>(vehicle, 0xB8) = (float)atof(token); break;
                case 25: HandlingField<float>(vehicle, 0xBC) = (float)atof(token); break;
                case 26: HandlingField<float>(vehicle, 0xC0) = (float)atof(token); break;
                case 27: HandlingField<float>(vehicle, 0xC4) = (float)atof(token); break;
                case 28: HandlingField<float>(vehicle, 0xD4) = (float)atof(token); break;
                case 29: HandlingField<float>(vehicle, 0xC8) = (float)atof(token); break;
                case 30: HandlingField<int32>(vehicle, 0xD8) = atoi(token); break;
                case 31: sscanf(token, "%x", &HandlingField<uint32>(vehicle, 0xCC)); break;
                case 32:
                    sscanf(token, "%x", &HandlingField<uint32>(vehicle, 0xD0));
                    HandlingField<uint32>(vehicle, 0x78) = HandlingField<uint32>(vehicle, 0xD0);
                    break;
                case 33: HandlingField<uint8>(vehicle, 0xDC) = (uint8)atoi(token); break;
                case 34: HandlingField<uint8>(vehicle, 0xDD) = (uint8)atoi(token); break;
                case 35: HandlingField<uint8>(vehicle, 0xDE) = (uint8)atoi(token); break;
                default: break;
                }
            }
            ConvertDataToGameUnits(reinterpret_cast<tHandlingData*>(vehicle));
            break;
        }
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x005BF3D0
void cHandlingDataMgr::Initialise() {
    LoadHandlingData();

    field_0 = 0.1f;
    fWheelFriction = 0.9f;
    field_8 = 1.0f;
    field_C = 0.8f;
    field_10 = 0.98f;
}

// 0x6F52D0
tFlyingHandlingData* cHandlingDataMgr::GetFlyingPointer(uint8 handlingId) {
    if (handlingId >= 186 && handlingId <= 209) {
        return &m_aFlyingHandling[handlingId - 186];
    }
    return &m_aFlyingHandling[0];
}

// 0x6F5300
tBoatHandlingData* cHandlingDataMgr::GetBoatPointer(uint8 handlingId) {
    if (handlingId >= 175u && handlingId <= 186u) {
        return &m_aBoatHandling[handlingId - 175u];
    }
    return &m_aBoatHandling[0];
}

tHandlingData* cHandlingDataMgr::GetVehiclePointer(uint32 handlingId) {
    return &m_aVehicleHandling.data()[handlingId]; // Como el original: sin comprobar el indice (0x6A0480 acepta 0..255)
}

tBikeHandlingData* cHandlingDataMgr::GetBikeHandlingPointer(uint32 handlingId) {
    // Como el original (inlinado en 0x6BF520 y 0x6CE3C6): movzx del byte bajo, shl 6 y suma de 0xC3491C = &m_aBikeHandling[0] - 162 entradas
    // (para ids menores que 162 el puntero queda antes de m_aBikeHandling, dentro de m_aVehicleHandling; sin comprobar el indice)
    return m_aBikeHandling.data() + (static_cast<ptrdiff_t>(static_cast<uint8>(handlingId)) - 162); // NOTE: 162 = VT_BIKE
}

// 0x6A0480
bool cHandlingDataMgr::HasFrontWheelDrive(uint8 handlingId) {
    return GetVehiclePointer(handlingId)->GetTransmission().m_nDriveType != 'R';
};

// 0x6A04B0
bool cHandlingDataMgr::HasRearWheelDrive(uint8 handlingId) {
    return GetVehiclePointer(handlingId)->GetTransmission().m_nDriveType != 'F';
}

// get handling id by name
// 0x6F4FD0
int32 cHandlingDataMgr::GetHandlingId(const char* nameToFind) {
    // Como el original: strncmp de 14 caracteres y, si no se encuentra, devuelve 210
    int32 id = 0;
    for (; id < (int32)std::size(VehicleNames); id++) {
        if (!strncmp(nameToFind, VehicleNames[id], 14)) {
            break;
        }
    }
    return id;
}

// Como el original: constantes globales del EXE calculadas al arrancar (0x854350 y 0x854390)
static inline auto& s_HandlingAccelConst    = StaticRef<float>(0xC2B9B4); // 1 / (50 * 50)
static inline auto& s_HandlingVelocityConst = StaticRef<float>(0xC2B9BC); // 0.277778 / 50

// Como el original: 0x859878 = 57.29578f (0x42652EE0)
static const float s_HandlingRadToDeg = std::bit_cast<float>(0x42652EE0u);

namespace {

// Como el original: 0x8595EC = 0.017453292f
constexpr float HANDLING_DEG_TO_RAD = 0.01745329238474369f;

} // namespace

// 0x6F5010
// update some handling variables with some world-specific multipliers
void cHandlingDataMgr::ConvertDataToWorldUnits(tHandlingData* h) {
    const auto t = &h->GetTransmission();

    // Como el original: se multiplica por el inverso (1 / ACCEL), primero la aceleracion y despues el freno
    const float invAccel = 1.0f / s_HandlingAccelConst;
    const float accel    = invAccel * t->m_EngineAcceleration;
    t->m_EngineAcceleration  = accel;
    t->m_MaxVelocity         = t->m_MaxVelocity / s_HandlingVelocityConst;
    h->m_fBrakeDeceleration  = invAccel * h->m_fBrakeDeceleration;
    t->m_EngineAcceleration  = t->m_nDriveType == '4' ? accel * 4.0f : accel + accel;
    h->m_fCollisionDamageMultiplier = h->m_fMass * h->m_fCollisionDamageMultiplier * 0.0005f;
}

// update some handling variables with some game-specific multipliers
// 0x6F5080
void cHandlingDataMgr::ConvertDataToGameUnits(tHandlingData* h) {
    const auto t = &h->GetTransmission();

    const float accel = s_HandlingAccelConst * t->m_EngineAcceleration;
    t->m_EngineAcceleration = accel;
    const float velocity = s_HandlingVelocityConst * t->m_MaxVelocity; // Como el original: se guarda aparte (0x6F50A5)
    t->m_MaxVelocity = velocity;
    h->m_fBrakeDeceleration = s_HandlingAccelConst * h->m_fBrakeDeceleration;
    const float massRecpr = 1.0f / h->m_fMass;
    h->m_fMassRecpr = massRecpr;
    h->m_fBuoyancyConstant = h->m_fMass * 0.8f / (float)h->m_nPercentSubmerged;
    h->m_fCollisionDamageMultiplier = massRecpr * h->m_fCollisionDamageMultiplier * 2000.0f;

    // Como el original (0x6F50F0..0x6F5158): el bucle de reduccion va entero en X87Math (0,01 y 1/6 en double, NaN sale)
    const float maxVelocity = HandlingReduceVelocityCpp(velocity, accel, h->m_fDragMult);

    if (h->m_nVehicleId == VT_RCBANDIT) { // Como el original: usa la velocidad sin reducir
        t->m_MaxFlatVelocity    = velocity;
        t->m_MaxReverseVelocity = -velocity;
    } else if (h->m_bUseMaxspLimit) { // Como el original: velocidad sin reducir por 0.8333333f (0x863278)
        t->m_MaxFlatVelocity    = velocity * 0.8333333134651184f;
        t->m_MaxReverseVelocity = std::min(t->m_MaxFlatVelocity * -0.25f, -0.2f);
    } else if (h->m_nVehicleId >= VT_BIKE && h->m_nVehicleId <= VT_FREEWAY) { // 2 wheelers
        t->m_MaxFlatVelocity    = maxVelocity;
        t->m_MaxReverseVelocity = -0.05f;
        t->m_MaxVelocity        = maxVelocity * 1.2f;
    } else {
        t->m_MaxFlatVelocity    = maxVelocity;
        t->m_MaxVelocity        = maxVelocity * 1.2f;
        t->m_MaxReverseVelocity = std::min(t->m_MaxFlatVelocity * -0.3f, -0.2f);
    }

    t->m_EngineAcceleration = accel * (t->m_nDriveType == '4' ? 0.25f : 0.5f);
    t->InitGearRatios();
}

// 0x6F5240
void cHandlingDataMgr::ConvertBikeDataToWorldUnits(tBikeHandlingData* bikeHandling) {
    bikeHandling->m_fMaxLean      = ExeAsinDegreesCpp(bikeHandling->m_fMaxLean);
    bikeHandling->m_fFullAnimLean = HandlingTimesFloatCpp(bikeHandling->m_fFullAnimLean, s_HandlingRadToDeg);
    bikeHandling->m_fWheelieAng   = ExeAsinDegreesCpp(bikeHandling->m_fWheelieAng);
    bikeHandling->m_fStoppieAng   = ExeAsinDegreesCpp(bikeHandling->m_fStoppieAng);
}

// 0x6F5290
void cHandlingDataMgr::ConvertBikeDataToGameUnits(tBikeHandlingData* bikeHandling) {
    bikeHandling->m_fMaxLean      = HandlingSinScaledCpp(bikeHandling->m_fMaxLean, HANDLING_DEG_TO_RAD);
    bikeHandling->m_fFullAnimLean = HandlingTimesFloatCpp(bikeHandling->m_fFullAnimLean, HANDLING_DEG_TO_RAD);
    bikeHandling->m_fWheelieAng   = HandlingSinScaledCpp(bikeHandling->m_fWheelieAng, HANDLING_DEG_TO_RAD);
    bikeHandling->m_fStoppieAng   = HandlingSinScaledCpp(bikeHandling->m_fStoppieAng, HANDLING_DEG_TO_RAD);
}

// 0x006F4F30
int32 cHandlingDataMgr::FindExactWord(const char* name, const char* nameTable, uint32 entrySize, uint32 entryCount) {
    // Como el original: do-while con contador con signo (siempre mira la primera entrada) y, si no se encuentra,
    // devuelve entryCount
    int32 i = 0;
    do {
        const auto entry = &nameTable[entrySize * i];
        if (!strncmp(name, entry, strlen(entry))) {
            return i;
        }
        i++;
    } while (i < (int32)entryCount);
    return (int32)entryCount;
}
