/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBoat
 * Reconstrucción: COMPLETA — 23/23 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/23 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Boat.h"
#include "CarCtrl.h"
#include "ControllerConfigManager.h"

const float SECOND_LOD_DISTANCE = 150.0f; // 0x872188

static const constexpr auto uiNumIndices{ 6u };
auto& KeepWaterOutIndices = StaticRef<RwImVertexIndex[uiNumIndices]>(0xC27988);
static const constexpr auto uiNumVertices{ 4u };
auto& KeepWaterOutVertices = StaticRef<RwIm3DVertex[uiNumVertices]>(0xC278F8);

const float BOAT_STEER_SMOOTH_RATE = 0.2f; // 0x87218C

const float BOAT_MOUSE_STEER_SCALE = -0.0035f; // 0x872190
const float BOAT_STEER_DAMP_THRESHOLD = 0.5f; // 0x872194
const float BOAT_STEER_DAMP_BASE = 0.985f; // 0x872198

const float fShapeLength = 0.4f; // 0x8D3944
const float fShapeTime = 0.05f; // 0x8D3948

auto& fRangeMult = StaticRef<float>(0x8D394C); // 0.6f
auto& fTimeMult = StaticRef<float>(0xC279CC); // 1.2f / CBoat::WAKE_LIFETIME

// Como el original: los contadores son las globales del EXE (no copias de la DLL)
auto& nWakeSkipCounters = StaticRef<int16[4]>(0xC279A4);
auto& nWakeSkipCounterVertex = StaticRef<int16[4]>(0xC279AC);

namespace {
// Constantes del EXE que no son el float más cercano al literal o que la DLL tenía imprecisas
const float BOAT_NO_LOCKED_HEADING = std::bit_cast<float>(0xC61C3FF6u); // -9999.99f
const float BOAT_TWO_PI            = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
const float BOAT_PROP_SPEED_SCALE  = std::bit_cast<float>(0x40A2F983u); // 0x8722D8 (32 / 2pi)
const float BOAT_SCORCH_MASS_MULT  = std::bit_cast<float>(0x3BD1B718u); // 0x871248 (0.0064)
const float BOAT_SCORCH_BUOY_MULT  = std::bit_cast<float>(0x370637BEu); // 0x871244 (8e-6)
const float BOAT_DOOR_PI_10        = std::bit_cast<float>(0x3EA0D97Cu); // pi / 10
const float BOAT_DOOR_2PI_10       = std::bit_cast<float>(0x3F20D97Cu); // 2pi / 10
const float BOAT_WHEEL_BUOYANCY    = std::bit_cast<float>(0x3DDA740Fu); // 8 / 75
const float BOAT_WAKE_MAX_DIST     = std::bit_cast<float>(0x497423FEu); // 999999.99f del EXE (0x6F2892)
const float BOAT_INV_255           = std::bit_cast<float>(0x3B808081u); // 0x859A3C (1/255)

// Como el original: Multiply3x3 (0x59C790) con el orden de sumas del EXE
CVector BoatMultiply3x3(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}
} // namespace

/**
 * Registra construcción, control, wake trail y comportamiento específico de barcos.
 */
void CBoat::InjectHooks() {
    RH_ScopedVirtualClass(CBoat, 0x8721a0, 66);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6F2940);
    RH_ScopedInstall(Destructor, 0x6F00F0);

    RH_ScopedVMTInstall(SetModelIndex, 0x6F1140);
    RH_ScopedVMTInstall(ProcessControl, 0x6F1770);
    RH_ScopedVMTInstall(Teleport, 0x6F20E0);
    RH_ScopedVMTInstall(PreRender, 0x6F1180);
    RH_ScopedVMTInstall(IsComponentPresent, 0x6F0200);
    RH_ScopedVMTInstall(Render, 0x6F0210);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6F0A10);
    RH_ScopedVMTInstall(GetComponentWorldPosition, 0x6F01D0);
    RH_ScopedVMTInstall(ProcessOpenDoor, 0x6F0190);
    RH_ScopedVMTInstall(BlowUpCar, 0x6F21B0);

    RH_ScopedInstall(PruneWakeTrail, 0x6F0E20);
    RH_ScopedInstall(AddWakePoint, 0x6F2550);
    RH_ScopedInstall(SetupModelNodes, 0x6F01A0);
    RH_ScopedInstall(DebugCode, 0x6F0D00);
    RH_ScopedInstall(ModifyHandlingValue, 0x6F0DE0);
    RH_ScopedInstall(DisplayHandlingData, 0x6F0D90);
    RH_ScopedInstall(FillBoatList, 0x6F2710);
    RH_ScopedInstall(IsSectorAffectedByWake, 0x6F0E80);
    RH_ScopedInstall(IsVertexAffectedByWake, 0x6F0F50);
    RH_ScopedInstall(CheckForSkippingCalculations, 0x6F10C0);
    RH_ScopedGlobalInstall(GetBoatAtomicObjectCB, 0x6F00D0);
}

// 0x6F2940
/**
 * Inicializa handling, nodos, buoyancy, audio y estado del wake a partir del model info.
 */
CBoat::CBoat(int32 modelIndex, eVehicleCreatedBy createdBy) : CVehicle(createdBy) {
    CVehicleModelInfo* mi = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();

    m_BoatDoor = {};

    m_nVehicleType = VEHICLE_TYPE_BOAT;
    m_nVehicleSubType = VEHICLE_TYPE_BOAT;

    m_PadNum = 0;
    m_Scan = 0.0f;

    m_PropellerAngle = 0.0f;

    m_OldMoveSpeed.Reset();
    m_OldTurnSpeed.Reset();

    m_NextTalkTimer = CTimer::GetTimeInMS();

    m_EngineSpeed   = 0.0f;
    CVehicle::SetModelIndex(modelIndex);
    SetupModelNodes();

    // Como el original: el índice del handling de vehículo es el byte bajo de m_nHandlingId (movzx bl)
    m_pHandlingData = gHandlingDataMgr.GetVehiclePointer(static_cast<uint8>(mi->m_nHandlingId));
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    m_pFlyingHandlingData = gHandlingDataMgr.GetFlyingPointer(static_cast<uint8>(mi->m_nHandlingId));
    m_BoatHandling = gHandlingDataMgr.GetBoatPointer(static_cast<uint8>(mi->m_nHandlingId));

    mi->ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);

    m_fMass = m_pHandlingData->m_fMass;
    m_fTurnMass = m_pHandlingData->m_fTurnMass * 0.5F;
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    m_fElasticity = 0.1F;
    m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;

    m_fAirResistance = GetDefaultAirResistance();

    physicalFlags.bTouchingWater = true;
    physicalFlags.bSubmergedInWater = true;
    m_nBoatFlags.bLockedToXY = true;
    m_nBoatFlags.bBoatEngineInWater = true;
    m_nBoatFlags.bBoatInWater = true;

    m_fSteerAngle = 0.0f;
    m_GasPedal = 0.0f;
    m_BrakePedal = 0.0f;
    m_fRawSteerAngle = 0.0f;
    m_CurrentField = 0;
    m_PrevVolume = 7.0F;
    m_TimeOfLastParticle = 0;

    m_LockedHeading = BOAT_NO_LOCKED_HEADING; // Como el original: -9999.99f
    m_BlowUpTimer = 0.0f;
    m_EntityThatSetUsOnFire = nullptr;
    m_NumWakeCoords = 0;
    rng::fill(m_WakePtCounters, 0.0f);

    m_nAmmoInClip = 20;
    // Como el original: ángulos pi/10 y 2pi/10 exactos del EXE y eje Z (byte 2) en los dos casos
    if (m_nModelIndex == MODEL_MARQUIS) {
        m_BoatDoor.Init(BOAT_DOOR_PI_10, -BOAT_DOOR_PI_10, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_NONE);
    } else {
        m_BoatDoor.Init(BOAT_DOOR_2PI_10, -BOAT_DOOR_2PI_10, DOOR_AXIS_NEG_X, DOOR_AXIS_Z, DOOR_EXTRA_NONE);
    }

    m_vehicleAudio.Initialise(this);
    rng::fill(m_fxSysProp, nullptr);
}

// 0x6F00F0
CBoat::~CBoat() {
    FxSystem_c::SafeKillAndClear(m_pFireParticle);
    for (auto& fx : m_fxSysProp) {
        FxSystem_c::SafeKillAndClear(fx);
    }
    m_vehicleAudio.Terminate();
}

// 0x6F01A0
// Reconstruye el array de RwFrame* de componentes a partir del clump del vehículo.
void CBoat::SetupModelNodes() {
    rng::fill(m_BoatNodes, nullptr);
    CClumpModelInfo::FillFrameArray(GetRpClump(), m_BoatNodes.data());
}

// debug function
// unused
// 0x6F0D00
void CBoat::DebugCode() {
    if (FindPlayerVehicle() != this) {
        return;
    }

    if (CPad::GetPad(m_PadNum)->NewState.Start) {
        return;
    }

    // Como el original: sigue solo si DPadLeft se acaba de pulsar (nuevo != 0 y anterior == 0)
    if (!CPad::GetPad()->IsDPadLeftPressed()) {
        return;
    }

    // Como el original: byte bajo de m_nHandlingId (movzx byte [mi+4Ah])
    const auto uiHandlingId = static_cast<uint8>(CModelInfo::GetModelInfo(m_nModelIndex)->AsVehicleModelInfoPtr()->m_nHandlingId);
    m_pHandlingData = &gHandlingDataMgr.m_aVehicleHandling.data()[uiHandlingId];
    SetupModelNodes();
}

// debug function
// unused
// 0x6F0D90
void CBoat::DisplayHandlingData() {
    // Como el original: dos sprintf sobre un búfer local de 64 bytes sin inicializar
    char cBuffer[64];
    sprintf(cBuffer, "Thrust %3.2f", m_pHandlingData->m_transmissionData.m_EngineAcceleration * m_pHandlingData->m_fMass);
    sprintf(cBuffer, "Rudder Angle  %3.2f", m_pHandlingData->m_fSteeringLock);
}

// debug function
// unused
// 0x6F0DE0
void CBoat::ModifyHandlingValue(const bool& plus) {
    float fSign;
    int32 nSign = 1;

    fSign = plus ? float(nSign) : float(-nSign);

    switch (m_CurrentField) {
    case 4: {
        m_pHandlingData->m_fSteeringLock += fSign;
        break;
    }
    }
}

// 0x6F0E20
// Envejece los puntos de wake y recorta m_NumWakeCoords al primer slot expirado.
void CBoat::PruneWakeTrail() {
    // Como el original: si los 32 puntos siguen vivos se sale sin escribir m_NumWakeCoords
    for (uint16 count = 0; count < 32; count++) {
        float& lifetime = m_WakePtCounters[count];
        if (!(lifetime > 0.0F)) {
            m_NumWakeCoords = count;
            return;
        }

        if (!(lifetime > CTimer::GetTimeStep())) {
            lifetime = 0.0F;
            m_NumWakeCoords = count;
            return;
        }

        lifetime -= CTimer::GetTimeStep();
    }
}

// 0x6F2550
/**
 * Inserta un nuevo punto en la cabeza del historial si el barco avanzó lo suficiente.
 * La longitud máxima del historial depende de player/mission/ambient.
 */
void CBoat::AddWakePoint(CVector pos) {
    // Como el original: _ftol2 (truncamiento a 64 bits) y se guarda el byte bajo
    const auto ucIntensity = static_cast<uint8>(static_cast<int64>(m_vecMoveSpeed.Magnitude() * 100.0F));

    // No wake points existing, early out
    if (m_WakePtCounters[0] <= 0.0F) {
        m_WakeCoords[0].Set(pos.x, pos.y);
        m_WakeBoatSpeed[0] = ucIntensity;
        m_WakePtCounters[0] = WAKE_LIFETIME;
        m_NumWakeCoords = 1;
        return;
    }

    // Too close to last wake point
    if (DistanceBetweenPointsSquared2D(m_WakeCoords[0], GetPosition()) <= sq(MIN_WAKE_INTERVAL)) {
        return;
    }

    // Determine max wake points based on entity status
    uint16 uiMaxWakePoints = 31;
    if (GetStatus() != STATUS_PLAYER) {
        if (GetCreatedBy() == eVehicleCreatedBy::MISSION_VEHICLE) {
            uiMaxWakePoints = 20;
        } else {
            uiMaxWakePoints = 15;
        }
    }

    // Get current water points count
    const uint16 uiCurWaterPoints = std::min(m_NumWakeCoords, uiMaxWakePoints);

    // Shift wake points if buffer has data
    if (uiCurWaterPoints > 0) {
        for (auto i = uiCurWaterPoints; i > 0; --i) {
            m_WakeCoords[i] = m_WakeCoords[i - 1];
            m_WakeBoatSpeed[i] = m_WakeBoatSpeed[i - 1];
            m_WakePtCounters[i] = m_WakePtCounters[i - 1];
        }
    }

    // Add new wake point
    m_WakeCoords[0].Set(pos.x, pos.y);
    m_WakePtCounters[0] = WAKE_LIFETIME;
    m_WakeBoatSpeed[0] = ucIntensity;

    // Increment counter if not at absolute max capacity
    if (m_NumWakeCoords < 32) { // todo: magic number
        ++m_NumWakeCoords;
    }
}

// NOTSA: trasladado desde CWaterLevel::RenderBoatWakes.
/**
 * Convierte el historial de wake en segmentos de agua con anchura y alpha dependientes
 * de edad, velocidad y distancia a cámara.
 */
void CBoat::RenderWakePoints() {
    if (m_NumWakeCoords <= 1) { // From 0x6EDAF7
        return;
    }

    // 0x6EDA1B
    const auto GetWidth = [
        &,
        maxWidth = [&] {
            const auto maxWidth = GetColModel()->GetBoundingBox().m_vecMax.x * 0.65f;
            return m_nModelIndex == MODEL_SKIMMER
                ? maxWidth * 0.4f
                : maxWidth;
        }()
    ](auto ptIdx) {
        const auto t = (CBoat::WAKE_LIFETIME - m_WakePtCounters[ptIdx]) * ((m_WakeBoatSpeed[ptIdx] / 25.f + 0.5f) / CBoat::WAKE_LIFETIME);
        return maxWidth + maxWidth * t;
    };

    // 0x6EDC6B
    const auto GetAlpha = [
        &,
        boatToCamDist2D = (TheCamera.GetPosition() - GetPosition()).Magnitude2D()
    ](auto ptIdx) {
        auto alpha = (1.f - (float)ptIdx / (float)m_NumWakeCoords) * 160.f;
        if (ptIdx < 3) {
            // Pirulax: Not really noticeable, but I think this is how it was supposed to be.
            //          otherwise they wouldn't be calculating it for the 0th point... (would just use 0 instead)
#ifdef FIX_BUGS
            alpha *= (float)(ptIdx + 1) / 3.f;
#else
            alpha *= (float)ptIdx / 3.f;
#endif
        }
        alpha *= (float)m_WakeBoatSpeed[ptIdx] / 100.f + 0.15f;
        if (boatToCamDist2D > 50.f) { // MAX_WAKE_LENGTH ?
            alpha *= (80.f - boatToCamDist2D) / 30.f;
        }
        return alpha;
    };

    auto prevWidth = GetWidth(0);
    auto prevAlpha = GetAlpha(0); // It actually comes out to be 0 for the 0th point... bug?
    auto prevDirToPrev = CVector2D{ GetForward() }; // Direction from the point before the previous to it
    for (auto wakePtIdx = 1; wakePtIdx < m_NumWakeCoords; wakePtIdx++) {
        const auto& currPos = m_WakeCoords[wakePtIdx];
        const auto& prevPos = m_WakeCoords[wakePtIdx - 1];

        auto currToPrevDir = prevPos - currPos;
        const auto distToPrevSq = currToPrevDir.SquaredMagnitude();
        if (distToPrevSq >= sq(3.f)) {
            currToPrevDir /= std::sqrt(distToPrevSq); // Normalize it
        }

        const auto currAlpha = GetAlpha(wakePtIdx);
        const auto currWidth = GetWidth(wakePtIdx);

        if (distToPrevSq <= sq(13.f)) {
            const auto GetCorners = [](auto& posOnDir, auto& dirToLieOn, float width) {
                const auto offset = dirToLieOn.GetPerpRight() * width;
                return std::make_pair(posOnDir - offset, posOnDir + offset);
            };

            const auto [prevA, prevB] = GetCorners(prevPos, prevDirToPrev, prevWidth);
            const auto [currB, currA] = GetCorners(currPos, currToPrevDir, currWidth); // yes, order is BA

            CWaterLevel::RenderWakeSegment(
                prevA, prevB, currA, currB, -0.03f, currWidth, prevAlpha, currAlpha, -0.03f
            );
        }

        prevWidth = currWidth;
        prevAlpha = currAlpha;
        prevDirToPrev = currToPrevDir;
    }
}

// NOTSA: Moved from 0x6ED9A0, because it just *makes sense*
void CBoat::RenderAllWakePointBoats() {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(CWaterLevel::texWaterwake)));

    FillBoatList();

    for (const auto boat : apFrameWakeGeneratingBoats) {
        if (!boat) {
            break;
        }
        boat->RenderWakePoints();
    }

    RenderBuffer::RenderStuffInBuffer();
}

// 0x6F0E80
bool CBoat::IsSectorAffectedByWake(CVector2D centreCoords, float semiSize, CBoat** ppBoats) {
    if (!apFrameWakeGeneratingBoats[0]) {
        return false;
    }

    bool bWakeFound = false;
    for (auto& boat : apFrameWakeGeneratingBoats) {
        // Como el original: el primer hueco vacío termina la búsqueda
        if (!boat) {
            break;
        }

        if (!boat->m_NumWakeCoords) {
            continue;
        }

        for (int32 iTrail = 0; iTrail < boat->m_NumWakeCoords; ++iTrail) {
            auto fDist = (WAKE_LIFETIME - boat->m_WakePtCounters[iTrail]) * fShapeTime + static_cast<float>(iTrail) * fShapeLength + semiSize;
            if (!(std::fabs(boat->m_WakeCoords[iTrail].x - centreCoords.x) < fDist) || !(std::fabs(boat->m_WakeCoords[iTrail].y - centreCoords.y) < fDist)) {
                continue;
            }

            ppBoats[bWakeFound] = boat;
            bWakeFound = true;
            break;
        }
    }

    return bWakeFound;
}

// 0x6F0F50
float CBoat::IsVertexAffectedByWake(CVector coords, CBoat* boat, int16 wakeQuadrant, bool forceCheck) {
    if (forceCheck) {
        nWakeSkipCounters[wakeQuadrant] = 0;
        nWakeSkipCounterVertex[wakeQuadrant] = 8;
    } else if (nWakeSkipCounters[wakeQuadrant] > 0) {
        return 0.0F;
    }

    if (!boat->m_NumWakeCoords) {
        return 0.0F;
    }

    for (uint16 iTrail = 0; iTrail < boat->m_NumWakeCoords; ++iTrail) {
        const float fWakeDist = (WAKE_LIFETIME - boat->m_WakePtCounters[iTrail]) * fShapeTime + static_cast<float>(iTrail) * fShapeLength;
        const float fWakeDistSquared = fWakeDist * fWakeDist;
        const float dx = boat->m_WakeCoords[iTrail].x - coords.x;
        const float dy = boat->m_WakeCoords[iTrail].y - coords.y;
        const float fTrailDistSquared = dy * dy + dx * dx;
        if (fTrailDistSquared < fWakeDistSquared) {
            nWakeSkipCounterVertex[wakeQuadrant] = 0;
            float fContrib = sqrtf(fTrailDistSquared / fWakeDistSquared) * fRangeMult + (WAKE_LIFETIME - boat->m_WakePtCounters[iTrail]) * fTimeMult;
            if (1.0F < fContrib) { // Como el original: NaN se conserva
                fContrib = 1.0F;
            }
            return 1.0F - fContrib;
        }

        auto fDistDiff = fTrailDistSquared - fWakeDistSquared;
        if (fDistDiff > 20.0F) {
            if (nWakeSkipCounterVertex[wakeQuadrant] > 3) {
                nWakeSkipCounterVertex[wakeQuadrant] = 3;
            }
        } else if (fDistDiff > 10.0F) {
            if (nWakeSkipCounterVertex[wakeQuadrant] > 2) {
                nWakeSkipCounterVertex[wakeQuadrant] = 2;
            }
        }
    }

    return 0.0F;
}

// 0x6F10C0
void CBoat::CheckForSkippingCalculations() {
    for (size_t ind = 0; ind < 4; ++ind) {
        const int32 iVal = nWakeSkipCounterVertex[ind];
        if (iVal > 0 && iVal < 8) {
            // Como el original: max(vértice, contador - 1)
            const int32 dec = nWakeSkipCounters[ind] - 1;
            nWakeSkipCounters[ind] = static_cast<int16>(iVal > dec ? iVal : dec);
        } else if (nWakeSkipCounters[ind] > 0) {
            // Como el original: el contador baja en uno (no se copia el valor del vértice)
            --nWakeSkipCounters[ind];
        }

        nWakeSkipCounterVertex[ind] = 8;
    }
}

// 0x6F2710
/**
 * Selecciona para este frame los barcos con wake relevantes para cámara, limitado al
 * pequeño array global de generación.
 */
void CBoat::FillBoatList() {
    apFrameWakeGeneratingBoats.fill(nullptr);

    const auto& vecCamPos = TheCamera.GetPosition();
    const float camX = vecCamPos.x, camY = vecCamPos.y;

    // Como el original: solo x/y del forward de m_mCameraMatrix (0xB6F9AC/0xB6F9B0), normalizado en 2D
    float dirX = *reinterpret_cast<const float*>(0xB6F9AC);
    float dirY = *reinterpret_cast<const float*>(0xB6F9B0);
    const float dirLen = std::sqrt(dirY * dirY + dirX * dirX);
    if (dirLen > 0.0F) {
        const float inv = 1.0F / dirLen;
        dirX *= inv;
        dirY *= inv;
    }

    // Como el original: el pool se recorre desde el último índice hacia el primero y se usan distancias al cuadrado
    auto* const pool = GetVehiclePool();
    int16 iCurBoat = 0;
    for (auto idx = static_cast<int32>(pool->GetSize()) - 1; idx >= 0; --idx) {
        if (pool->IsFreeSlotAtIndex(static_cast<size_t>(idx))) {
            continue;
        }

        auto* const vehicle = pool->GetAt(static_cast<size_t>(idx));
        if (vehicle->m_nVehicleType != VEHICLE_TYPE_BOAT) {
            continue;
        }

        auto* const boat = vehicle->AsBoat();
        if (!boat->m_NumWakeCoords) {
            continue;
        }

        const auto& vecBoatPos = boat->GetPosition();
        const float dx = vecBoatPos.x - camX;
        const float dy = vecBoatPos.y - camY;
        const float fCamDot = dy * dirY + dx * dirX;
        if (fCamDot > 100.0F || fCamDot < -15.0F) {
            continue;
        }

        const float fDistSq = dy * dy + dx * dx;
        if (fDistSq > 6400.0F) {
            continue;
        }

        // Early out, the list isn't full yet
        if (iCurBoat < NUM_WAKE_GEN_BOATS) {
            apFrameWakeGeneratingBoats[iCurBoat++] = boat;
            continue;
        }

        // Como el original: se busca el barco más cercano de la lista y se reemplaza si el nuevo está aún más cerca
        int16 iNewInd = -1;
        float fMinDistSq = BOAT_WAKE_MAX_DIST;
        for (int16 iCheckedInd = 0; iCheckedInd < NUM_WAKE_GEN_BOATS; ++iCheckedInd) {
            const auto& vecCheckedPos = apFrameWakeGeneratingBoats[iCheckedInd]->GetPosition();
            const float cx = vecCheckedPos.x - camX;
            const float cy = vecCheckedPos.y - camY;
            const float fCheckedDistSq = cy * cy + cx * cx;
            if (fCheckedDistSq < fMinDistSq) {
                fMinDistSq = fCheckedDistSq;
                iNewInd = iCheckedInd;
            }
        }

        if (iNewInd != -1 && (fDistSq < fMinDistSq || boat->GetStatus() == STATUS_PLAYER)) {
            apFrameWakeGeneratingBoats[iNewInd] = boat;
        }
    }
}

// 0x6F1140
void CBoat::SetModelIndex(uint32 index) {
    CVehicle::SetModelIndex(index);
    rng::fill(m_BoatNodes, nullptr);
    CClumpModelInfo::FillFrameArray(GetRpClump(), m_BoatNodes.data());
}

// 0x6F1770
/**
 * Orquesta audio, wake, alarmas, input/AI, buoyancy y física según eEntityStatus.
 */
void CBoat::ProcessControl() {
    m_vehicleAudio.Service();
    PruneWakeTrail();
    CVehicle::ProcessDelayedExplosion();

    // Como el original: constantes plegadas del EXE (0x871248 y 0x871244, no son el float más cercano)
    if (physicalFlags.bRenderScorched && m_fMass * BOAT_SCORCH_MASS_MULT < m_fBuoyancyConstant) {
        m_fBuoyancyConstant -= m_fMass * BOAT_SCORCH_BUOY_MULT;
    }

    // Como el original: FindPlayerPed()->GetPlayerData()->m_pWanted y nivel con signo
    const auto* wanted = FindPlayerPed()->GetPlayerData()->m_pWanted;
    if (static_cast<int32>(wanted->GetWantedLevel()) > 0 && m_nModelIndex == MODEL_PREDATOR) {
        auto vehicle = FindPlayerVehicle();
        if (vehicle && vehicle->GetVehicleAppearance() == eVehicleAppearance::VEHICLE_APPEARANCE_BOAT) {
            auto iCarMission = m_autoPilot.m_nCarMission;
            if (iCarMission == eCarMission::MISSION_BOAT_ATTACKPLAYER
                || (iCarMission >= eCarMission::MISSION_RAMPLAYER_FARAWAY
                    && iCarMission <= eCarMission::MISSION_BLOCKPLAYER_CLOSE)) {
                if (CTimer::GetTimeInMS() > m_NextTalkTimer) {
                    m_NextTalkTimer = CGeneral::GetRandomNumber() % 4096 + CTimer::GetTimeInMS() + 4500;
                }
            }
        }
    }

    CVehicle::UpdateClumpAlpha();
    CVehicle::ProcessCarAlarm();

    switch (GetStatus()) {
    case eEntityStatus::STATUS_PLAYER:
        m_nBoatFlags.bLockedToXY = false;
        m_LockedHeading = BOAT_NO_LOCKED_HEADING;
        if (m_pDriver) {
            ProcessControlInputs(m_pDriver->m_nPedType);
        }

        if (m_nModelIndex == MODEL_PREDATOR) {
            CVehicle::DoFixedMachineGuns();
        }

        CVehicle::DoDriveByShootings();
        break;
    case eEntityStatus::STATUS_SIMPLE:
        m_nBoatFlags.bLockedToXY = false;
        m_LockedHeading = BOAT_NO_LOCKED_HEADING;
        CCarAI::UpdateCarAI(this);
        CPhysical::ProcessControl();
        physicalFlags.bSubmergedInWater = true;
        m_nBoatFlags.bBoatInWater = true;
        m_nBoatFlags.bBoatEngineInWater = true;
        return;
    case eEntityStatus::STATUS_PHYSICS:
        m_nBoatFlags.bLockedToXY = false;
        m_LockedHeading = BOAT_NO_LOCKED_HEADING;
        CCarAI::UpdateCarAI(this);
        CCarCtrl::SteerAICarWithPhysics(this);
        break;
    case eEntityStatus::STATUS_ABANDONED:
    case eEntityStatus::STATUS_WRECKED: {
        vehicleFlags.bIsHandbrakeOn = false;
        physicalFlags.bSubmergedInWater = true;
        m_nBoatFlags.bBoatInWater = true;
        m_nBoatFlags.bBoatEngineInWater = true;

        m_fSteerAngle = 0.0F;
        m_BrakePedal = 0.5F;
        m_GasPedal = 0.0F;

        const auto fDist = (GetPosition() - FindPlayerCentreOfWorld_NoSniperShift(0)).Magnitude();
        if (fDist > SECOND_LOD_DISTANCE) {
            ResetMoveSpeed();
            ResetTurnSpeed();
            return;
        }
        break;
    }
    default:
        break;
    }

    if (GetStatus() == STATUS_PLAYER || GetStatus() == STATUS_REMOTE_CONTROLLED || GetStatus() == STATUS_PHYSICS) {
        auto fSTDPropSpeed = 0.0F;
        auto fROCPropSpeed = CPlane::PLANE_ROC_PROP_SPEED;
        if (m_nModelIndex == MODEL_SKIMMER) {
            fSTDPropSpeed = CPlane::PLANE_STD_PROP_SPEED;
        } else if (m_GasPedal != 0.0F) {
            fROCPropSpeed *= 5.0F;
        }

        if (m_GasPedal > 0.0F) {
            fSTDPropSpeed = (CPlane::PLANE_MAX_PROP_SPEED - CPlane::PLANE_STD_PROP_SPEED) * m_GasPedal + CPlane::PLANE_STD_PROP_SPEED;
        } else if (m_GasPedal < 0.0F) {
            fSTDPropSpeed = (CPlane::PLANE_STD_PROP_SPEED - CPlane::PLANE_MIN_PROP_SPEED) * m_GasPedal + CPlane::PLANE_STD_PROP_SPEED;
        }
        m_EngineSpeed += (fSTDPropSpeed - m_EngineSpeed) * CTimer::GetTimeStep() * fROCPropSpeed;
    } else if (m_EngineSpeed > 0.0F) {
        m_EngineSpeed *= 0.95F;
    }

    auto fDamagePower = m_fDamageIntensity * m_pHandlingData->m_fCollisionDamageMultiplier;
    if (fDamagePower > 25.0F && GetStatus() != STATUS_WRECKED && m_fHealth >= 250.0F) {
        const auto fSavedHealth = m_fHealth;
        if (GetStatus() == STATUS_PLAYER && CStats::GetPercentageProgress() >= 100.0F) {
            fDamagePower *= 0.5F;
        }

        // Como el original: se multiplica por 0.5/0.25 o por 1/6 y 1/12 (no se divide)
        auto fGivenDamage = fDamagePower;
        if (this == FindPlayerVehicle()) {
            fGivenDamage -= 25.0F;
            fGivenDamage *= vehicleFlags.bTakeLessDamage ? 1.0F / 6.0F : 0.5F;
        } else {
            if (fGivenDamage > 60.0F && m_pDriver) {
                m_pDriver->Say(CTX_GLOBAL_CRASH_GENERIC);
            }

            fGivenDamage -= 25.0F;
            fGivenDamage *= vehicleFlags.bTakeLessDamage ? 1.0F / 12.0F : 0.25F;
        }

        m_fHealth -= fGivenDamage;
        if (m_fHealth <= 0.0F && fSavedHealth > 0.0F) {
            m_fHealth = 1.0F;
            m_EntityThatSetUsOnFire = m_pDamageEntity;
        }
    }

    if (m_fHealth > 460.0F || GetStatus() == STATUS_WRECKED) {
        m_BlowUpTimer = 0.0F;
        FxSystem_c::SafeKillAndClear(m_pFireParticle);
    } else {
        auto vecDist = GetPosition() - TheCamera.GetPosition();
        if (std::fabs(vecDist.x) < 200.0F && std::fabs(vecDist.y) < 200.0F) {
            CVector vecFirePos(-1.5F, -0.5F, 1.2F);
            if (m_nModelIndex == MODEL_SPEEDER) {
                vecFirePos.Set(0.4F, -2.4F, 0.8F);
            } else if (m_nModelIndex == MODEL_REEFER) {
                vecFirePos.Set(2.0F, -1.0F, 0.5F);
            }
            vecFirePos.z += 1.0F; // Como el original: el fuego se crea 1 m más arriba

            if (m_fHealth < 250.0F) {
                if (!m_pFireParticle) {
                    if (const auto modellingMat = GetModellingMatrix()) {
                        m_pFireParticle = g_fxMan.CreateFxSystem("fire_car", vecFirePos, modellingMat, false);
                        if (m_pFireParticle) {
                            m_pFireParticle->Play();
                            CEventVehicleOnFire vehOnFireEvent(this);
                            GetEventGlobalGroup()->Add(&vehOnFireEvent, false);
                        }
                    }
                }

                // Como el original: se suman los ms del paso truncados a uint32 (_ftol2 y fild + 2^32)
                const auto ticks = static_cast<int32>(static_cast<int64>(CTimer::GetTimeStep() * 0.02F * 1000.0F));
                if (ticks < 0) {
                    m_BlowUpTimer += static_cast<float>(static_cast<double>(ticks) + 4294967296.0);
                } else {
                    m_BlowUpTimer = static_cast<float>(static_cast<double>(ticks) + static_cast<double>(m_BlowUpTimer));
                }
                if (m_BlowUpTimer > 5000.0F) {
                    BlowUpCar(m_EntityThatSetUsOnFire, false);
                }
            }
        }
    }

    // Como el original: m_bHasHitWall se lee antes de CPhysical::ProcessControl
    const bool bHasHitWall = GetHasHitWall();
    const bool bPostCollision = m_fDamageIntensity > 0.0F && m_vecLastCollisionImpactVelocity.z > 0.1F;
    CPhysical::ProcessControl();
    ProcessBoatControl(m_BoatHandling, &m_PrevVolume, bHasHitWall, bPostCollision);

    // Como el original: el Skimmer nunca consulta el truco de barcos voladores; la velocidad al cuadrado
    // (float) se compara con 0.05 en double
    if (m_nModelIndex == MODEL_SKIMMER) {
        if (m_EngineSpeed > CPlane::PLANE_MIN_PROP_SPEED || static_cast<double>(m_vecMoveSpeed.SquaredMagnitude()) > 0.05) {
            FlyingControl(FLIGHT_MODEL_PLANE, BOAT_NO_LOCKED_HEADING, BOAT_NO_LOCKED_HEADING, BOAT_NO_LOCKED_HEADING, BOAT_NO_LOCKED_HEADING);
        }
    } else if (CCheat::IsActive(CHEAT_BOATS_FLY)) {
        FlyingControl(FLIGHT_MODEL_BOAT, BOAT_NO_LOCKED_HEADING, BOAT_NO_LOCKED_HEADING, BOAT_NO_LOCKED_HEADING, BOAT_NO_LOCKED_HEADING);
    }

    if (m_nBoatFlags.bLockedToXY) {
        m_vecMoveSpeed.x = 0.0F;
        m_vecMoveSpeed.y = 0.0F;

        if (m_LockedHeading == BOAT_NO_LOCKED_HEADING) {
            m_LockedHeading = GetHeading();
        } else {
            // Como el original: locked - heading con el heading de fpatan sin redondear (x87)
            const CPlaceable* const self = this;
            const float locked = m_LockedHeading;
            float angle;
            __asm {
                mov ecx, self
                mov eax, dword ptr [ecx+14h]
                test eax, eax
                je no_matrix
                fld dword ptr [eax+10h]
                fchs
                fld dword ptr [eax+14h]
                fpatan
                jmp have_heading
            no_matrix:
                fld dword ptr [ecx+10h]
            have_heading:
                fsubr locked
                fstp angle
            }
            const CVector vecPos = GetPosition();
            m_matrix->RotateZ(angle);
            m_matrix->GetPosition() = vecPos;
        }
    }
}

// 0x6F20E0
void CBoat::Teleport(CVector newCoors, bool clearOrientation) {
    CWorld::Remove(this);
    SetPosn(newCoors);
    if (clearOrientation) {
        SetOrientation(CVector{ 0.0f });
    }

    ResetMoveSpeed();
    ResetTurnSpeed();
    CWorld::Add(this);
}

// 0x6F1180
void CBoat::PreRender() {
    CVehicle::PreRender();

    m_fContactSurfaceBrightness = 0.5F;
    SetComponentRotation(m_BoatNodes[BOAT_RUDDER],         AXIS_Z, -m_fSteerAngle, true);
    SetComponentRotation(m_BoatNodes[BOAT_REARFLAP_LEFT],  AXIS_Z, -m_fSteerAngle, true);
    SetComponentRotation(m_BoatNodes[BOAT_REARFLAP_RIGHT], AXIS_Z, -m_fSteerAngle, true);

    // Como el original: escala 32/2pi exacta del EXE, NaN se conserva y alpha = _ftol2 (sin truncar a byte)
    float fPropSpeed = m_EngineSpeed * BOAT_PROP_SPEED_SCALE;
    if (1.0F < fPropSpeed) {
        fPropSpeed = 1.0F;
    }
    const auto propAlpha = static_cast<int32>(static_cast<int64>((1.0F - fPropSpeed) * 255.0F));

    m_PropellerAngle += CTimer::GetTimeStep() * m_EngineSpeed;
    while (m_PropellerAngle > BOAT_TWO_PI) {
        m_PropellerAngle -= BOAT_TWO_PI;
    }

    // Como el original: hélice fija y móvil del primer par, después las del segundo
    const int32 movingAlpha = 150 - propAlpha < 0 ? 0 : 150 - propAlpha;
    ProcessBoatNodeRendering(BOAT_STATIC_PROP,   m_PropellerAngle * 2.0F,    propAlpha);
    ProcessBoatNodeRendering(BOAT_MOVING_PROP,   -m_PropellerAngle,          movingAlpha);
    ProcessBoatNodeRendering(BOAT_STATIC_PROP_2, -(m_PropellerAngle * 2.0F), propAlpha);
    ProcessBoatNodeRendering(BOAT_MOVING_PROP_2, m_PropellerAngle,           movingAlpha);

    if (m_nModelIndex == MODEL_MARQUIS) {
        if (const auto pFlap = m_BoatNodes[BOAT_FLAP_LEFT]) {
            CMatrix tempMat(RwFrameGetMatrix(pFlap), false);
            // Como el original: copia de la posición (SetRotate la pone a cero) y Multiply3x3 con m_matrix
            const CVector posCopy = tempMat.GetPosition();
            const CVector vecTransformed = BoatMultiply3x3(*m_matrix, posCopy);

            m_BoatDoor.Process(this, m_OldMoveSpeed, m_OldTurnSpeed, vecTransformed);
            CVector vecAxis{ 0.0F, 0.0F, 0.0F };
            vecAxis[m_BoatDoor.GetAxes()] = m_BoatDoor.m_angle;

            tempMat.SetRotate(vecAxis.x, vecAxis.y, vecAxis.z);
            tempMat.GetPosition() += posCopy;
            tempMat.UpdateRW();
        }
    }

    if (m_nModelIndex == MODEL_PREDATOR
        || m_nModelIndex == MODEL_REEFER
        || m_nModelIndex == MODEL_TROPIC) {
        if (const auto moving = m_BoatNodes[BOAT_MOVING]) {
            SetComponentRotation(moving, AXIS_Z, m_Scan, true);
            if (CCheat::IsActive(CHEAT_INVISIBLE_CAR)) {
                auto firstObj = reinterpret_cast<RpAtomic*>(GetFirstObject(moving));
                RpAtomicRenderMacro(firstObj);
            }
        }
        m_Scan += CTimer::GetTimeStep() * 0.02F; // Como el original: timestep * 0.02f (no / 50)
    }

    m_OldMoveSpeed = m_vecMoveSpeed + m_vecFrictionMoveSpeed;
    m_OldTurnSpeed = m_vecTurnSpeed + m_vecFrictionTurnSpeed;
    const float fSpeed = m_vecMoveSpeed.Magnitude();

    constexpr eBoatNodes propNodes[2]{ BOAT_STATIC_PROP, BOAT_STATIC_PROP_2 };
    for (auto i = 0; i < 2; ++i) {
        const auto prop = m_BoatNodes[propNodes[i]];
        RwMatrix* const splashMat = GetRwObject() ? GetRwMatrix() : nullptr;

        auto& splashFx = m_fxSysProp[i];
        if (!splashFx) {
            if (!splashMat || !prop) {
                continue;
            }
            const CVector vecPoint{ 0.0F, 0.0F, 0.0F };
            RwV3dAssign(RwMatrixGetPos(splashMat), RwMatrixGetPos(RwFrameGetLTM(prop)));
            RwMatrixUpdate(splashMat);
            splashFx = g_fxMan.CreateFxSystem("boat_prop", vecPoint, splashMat, false);
            if (!splashFx) {
                continue;
            }
            splashFx->Play();
            splashFx->SetLocalParticles(true);
            splashFx->CopyParentMatrix();
        }

        // Como el original: con el sistema ya creado no se comprueba la matriz
        if (physicalFlags.bSubmergedInWater) {
            if (splashFx->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED) {
                splashFx->Play();
            }

            RwV3dAssign(RwMatrixGetPos(splashMat), RwMatrixGetPos(RwFrameGetLTM(prop)));
            RwMatrixUpdate(splashMat);
            splashFx->SetMatrix(splashMat);
            float fTime = fSpeed + fSpeed;
            if (1.0F < fTime) {
                fTime = 1.0F;
            }
            splashFx->SetConstTime(true, fTime);
        } else if (splashFx->GetPlayStatus() == eFxSystemPlayStatus::FX_PLAYING) {
            splashFx->Stop();
        }
    }

    CVehicle::DoBoatSplashes(m_fxBuoyancyForce.Magnitude());
}

// NOTSA: optimization
inline void CBoat::ProcessBoatNodeRendering(eBoatNodes eNode, float fRotation, int32 alpha) {
    if (!m_BoatNodes[eNode]) {
        return;
    }

    SetComponentRotation(m_BoatNodes[eNode], AXIS_Y, fRotation, true);
    RpAtomic* atomic = nullptr; // Como el original: se inicializa antes de buscar
    RwFrameForAllObjects(m_BoatNodes[eNode], GetCurrentAtomicObjectCB, &atomic);
    if (atomic) {
        CVehicle::SetComponentAtomicAlpha(atomic, alpha);
    }
}

// 0x6F0210
void CBoat::Render() {
    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3'000;
    // Como el original: con el truco de coche invisible no se dibuja el vehículo, pero se siguen
    // preparando los vértices y los render states (solo se omiten las transformaciones/dibujos)
    if (!CCheat::IsActive(CHEAT_INVISIBLE_CAR)) {
        CVehicle::Render();
    }

    if (m_nModelIndex == MODEL_SKIMMER) {
        return;
    }

    // Code below is used to draw the "no water" zones, so the inside of the boat that is underwater surface, doesn't have water visible in it

    RwRGBA rwColor = CRGBA(255, 255, 255, 255).ToRwRGBA();
    // set color for all vertices
    for (auto& v : KeepWaterOutVertices) {
        RxObjSpace3DVertexSetPreLitColor(&v, &rwColor);
    }

    KeepWaterOutIndices[0] = 0;
    KeepWaterOutIndices[1] = 2;
    KeepWaterOutIndices[2] = 1;
    KeepWaterOutIndices[3] = 1;
    KeepWaterOutIndices[4] = 2;
    KeepWaterOutIndices[5] = 3;

    const auto SetQuad = [](float x0, float y0, float z0, float x1, float y1, float z1) {
        RwIm3DVertexSetPos(&KeepWaterOutVertices[0], -x0, y0, z0);
        RwIm3DVertexSetPos(&KeepWaterOutVertices[1], +x0, y0, z0);
        RwIm3DVertexSetPos(&KeepWaterOutVertices[2], -x1, y1, z1);
        RwIm3DVertexSetPos(&KeepWaterOutVertices[3], +x1, y1, z1);
    };
    // Como el original: u/v solo se escriben para los modelos con zona (el caso por defecto no las toca)
    const auto SetUVs = [] {
        RxObjSpace3DVertexSetU(&KeepWaterOutVertices[0], 0.0F);
        RxObjSpace3DVertexSetV(&KeepWaterOutVertices[0], 0.0F);
        RxObjSpace3DVertexSetU(&KeepWaterOutVertices[1], 1.0F);
        RxObjSpace3DVertexSetV(&KeepWaterOutVertices[1], 0.0F);
        RxObjSpace3DVertexSetU(&KeepWaterOutVertices[2], 0.0F);
        RxObjSpace3DVertexSetV(&KeepWaterOutVertices[2], 1.0F);
        RxObjSpace3DVertexSetU(&KeepWaterOutVertices[3], 1.0F);
        RxObjSpace3DVertexSetV(&KeepWaterOutVertices[3], 1.0F);
    };

    // Como el original: Speeder con y = +0.06 detrás y Reefer con el par trasero en los vértices 0/1
    switch (m_nModelIndex) {
    case MODEL_PREDATOR: SetQuad(1.45F,  1.90F, 0.96F,  1.45F, -3.75F, 0.96F); break;
    case MODEL_SQUALO:   SetQuad(1.222F, 2.004F, 1.409F, 1.240F, -1.367F, 0.846F); break;
    case MODEL_SPEEDER:  SetQuad(1.15F,  3.61F, 1.03F,  1.15F,  0.06F, 1.03F); break;
    case MODEL_REEFER:   SetQuad(1.66F, -4.48F, 0.83F,  1.90F,  2.83F, 1.00F); break;
    case MODEL_TROPIC:   SetQuad(1.886F, -2.347F, 0.787F, 1.886F, -4.670F, 0.842F); break;
    case MODEL_COASTG:   SetQuad(0.663F, 3.565F, 0.382F, 1.087F, 0.831F, 0.381F); break;
    case MODEL_DINGHY:   SetQuad(0.797F, 1.641F, 0.573F, 0.865F, -1.444F, 0.509F); break;
    case MODEL_MARQUIS:  SetQuad(1.246F, -1.373F, 0.787F, 1.023F, -5.322F, 0.787F); break;
    case MODEL_LAUNCH:   SetQuad(1.0F,   2.5F,  0.3F,   1.0F,  -5.4F,  0.3F); break;
    default:
        return;
    }
    SetUVs();

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(CWaterLevel::waterclear256Raster));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDZERO));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));

    RwMatrix tempMat;
    if (!CCheat::IsActive(CHEAT_INVISIBLE_CAR)) {
        m_matrix->UpdateRwMatrix(&tempMat);
        if (RwIm3DTransform(KeepWaterOutVertices, uiNumVertices, &tempMat, rwMATRIXTYPENORMAL)) {
            RwIm3DRenderIndexedPrimitive(RwPrimitiveType::rwPRIMTYPETRILIST, KeepWaterOutIndices, uiNumIndices);
            RwIm3DEnd();
        }
    }

    // Second tri list for Coastguard
    if (m_nModelIndex == MODEL_COASTG) {
        SetQuad(1.087F, 0.831F, 0.381F, 1.097F, -2.977F, 0.381F);
        SetUVs();

        if (!CCheat::IsActive(CHEAT_INVISIBLE_CAR)) {
            m_matrix->UpdateRwMatrix(&tempMat);
            if (RwIm3DTransform(KeepWaterOutVertices, uiNumVertices, &tempMat, rwMATRIXTYPENORMAL)) {
                RwIm3DRenderIndexedPrimitive(RwPrimitiveType::rwPRIMTYPETRILIST, KeepWaterOutIndices, uiNumIndices);
                RwIm3DEnd();
            }
        }
    }

    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
}

// 0x6F0A10
void CBoat::ProcessControlInputs(uint8 padNum) {
    m_PadNum = std::min<uint8>(padNum, 3);
    CPad* pad = CPad::GetPad(padNum);

    // Como el original: multiplicación por la constante 1/255 del EXE (0x859A3C), suavizado
    // (objetivo - freno) * 0.1 + freno y recorte que manda NaN a 0
    float newBrake = (static_cast<float>(pad->GetBrake()) * BOAT_INV_255 - m_BrakePedal) * 0.1F + m_BrakePedal;
    if (newBrake > 0.0F) {
        if (newBrake >= 1.0F) {
            newBrake = 1.0F;
        }
    } else {
        newBrake = 0.0F;
    }
    m_BrakePedal = newBrake;

    if (m_BrakePedal >= 0.05f) {
        m_GasPedal = m_BrakePedal * -0.3f;
    } else {
        m_BrakePedal = 0.0f;
        m_GasPedal = static_cast<float>(pad->GetAccelerate()) * BOAT_INV_255;
    }

    const auto KeyboardSteer = [&] {
        const float timeStep = CTimer::GetTimeStep();
        const auto steerInput = static_cast<int32>(pad->GetSteeringLeftRight());
        m_fRawSteerAngle += (static_cast<float>(-steerInput) * 0.0078125F - m_fRawSteerAngle) * BOAT_STEER_SMOOTH_RATE * timeStep;
        CVehicle::m_nLastControlInput = eControllerType::KEYBOARD;
    };

    // 0x6F0AE7
    if (CCamera::m_bUseMouse3rdPerson && CVehicle::m_bEnableMouseSteering) {
        // Como el original: el ratón manda si se movió, o si no se movió pero el volante sigue girado,
        // la última entrada fue el ratón y no hay dirección por teclado/mando
        const float mouseX = CPad::NewMouseControllerState.m_AmountMoved.x;
        bool useMouse = true;
        if (mouseX == 0.0f) {
            useMouse = std::fabs(m_fRawSteerAngle) > 0.0F
                && CVehicle::m_nLastControlInput == eControllerType::MOUSE
                && pad->GetSteeringLeftRight() == 0;
        }

        if (!useMouse) {
            // Como el original: sin dirección y con el ratón como última entrada no se toca nada
            if (pad->GetSteeringLeftRight() != 0 || CVehicle::m_nLastControlInput != eControllerType::MOUSE) {
                KeyboardSteer();
            }
        } else {
            if (!pad->NewState.m_bVehicleMouseLook) {
                m_fRawSteerAngle += mouseX * BOAT_MOUSE_STEER_SCALE;
            }
            CVehicle::m_nLastControlInput = eControllerType::MOUSE;

            if (std::fabs(m_fRawSteerAngle) < BOAT_STEER_DAMP_THRESHOLD || pad->NewState.m_bVehicleMouseLook) {
                // Como el original: pow del EXE (_CIpow 0x822130) en ST0 con precisión completa por el ángulo
                float* const rawPtr = &m_fRawSteerAngle;
                const float base = BOAT_STEER_DAMP_BASE;
                const float step = CTimer::GetTimeStep();
                __asm {
                    fld base
                    fld step
                    mov eax, 0x822130
                    call eax
                    mov edx, rawPtr
                    fmul dword ptr [edx]
                    fstp dword ptr [edx]
                }
            }
        }
    } else {
        KeyboardSteer();
    }

    // Como el original: recorte a [-1, 1] (NaN se conserva) y cuadrado con signo (-0 da +0)
    float raw = m_fRawSteerAngle;
    if (raw > 1.0F) {
        raw = 1.0F;
    } else if (raw < -1.0F) {
        raw = -1.0F;
    }
    m_fRawSteerAngle = raw;
    const float signedSquare = (raw < 0.0F || raw != raw) ? -(raw * raw) : raw * raw;
    m_fSteerAngle = m_pHandlingData->m_fSteeringLock * DEG_TO_RAD * signedSquare;
}

// 0x6F01D0
void CBoat::GetComponentWorldPosition(int32 componentId, CVector& posn) {
    posn = RwFrameGetLTM(m_BoatNodes[componentId])->pos;
}

// 0x6F0200
bool CBoat::IsComponentPresent(int32 component) const {
    return true;
} 

// 0x6F21B0
void CBoat::BlowUpCar(CEntity* culprit, bool inACutscene) {
    if (!vehicleFlags.bCanBeDamaged) {
        return;
    }

    physicalFlags.bRenderScorched = true;
    SetStatus(STATUS_WRECKED);
    CVisibilityPlugins::SetClumpForAllAtomicsFlag(GetRpClump(), eAtomicComponentFlag::ATOMIC_PIPE_NO_EXTRA_PASSES); // Como el original: 0x4000
    m_vecMoveSpeed.z += 0.13F;
    m_fHealth = 0.0F;
    m_wBombTimer = 0;

    const auto& vecPos = GetPosition();
    TheCamera.CamShake(0.4F, vecPos);
    CVehicle::KillPedsInVehicle();
    m_nOverrideLights = eVehicleOverrideLightsState::NO_CAR_LIGHT_OVERRIDE;
    vehicleFlags.bEngineOn = false;
    vehicleFlags.bLightsOn = false;
    CVehicle::ChangeLawEnforcerState(false);
    CExplosion::AddExplosion(this, culprit, eExplosionType::EXPLOSION_BOAT, vecPos, 0, 1, -1.0F, inACutscene);
    CDarkel::RegisterCarBlownUpByPlayer(*this, 0);

    auto movingComponent = m_BoatNodes[BOAT_MOVING];
    if (!movingComponent) {
        return;
    }

    RpAtomic* movingCompAtomic = nullptr; // Como el original: se inicializa antes de buscar
    RwFrameForAllObjects(movingComponent, GetBoatAtomicObjectCB, &movingCompAtomic);
    if (!movingCompAtomic) {
        return;
    }

    // Como el original: si el pool de objetos no da hueco no se construye nada
    void* const objMem = CObject::operator new(sizeof(CObject));
    if (!objMem) {
        return;
    }
    auto obj = ::new (objMem) CObject();
    obj->SetModelIndexNoCreate(MODEL_TEMPCOL_WHEEL1);
    obj->RefModelInfo(m_nModelIndex);

    auto movingCompMatrix = RwFrameGetLTM(movingComponent);
    auto newRwFrame = RwFrameCreate();
    auto movingCompAtomicClone = RpAtomicClone(movingCompAtomic);
    memcpy(RwFrameGetMatrix(newRwFrame), movingCompMatrix, sizeof(RwMatrix));
    RpAtomicSetFrame(movingCompAtomicClone, newRwFrame);
    CVisibilityPlugins::SetAtomicRenderCallback(movingCompAtomicClone, nullptr);
    obj->AttachToRwObject((RwObject*)movingCompAtomicClone, true);

    ++CObject::nNoTempObjects;
    obj->m_bDontStream = true;
    obj->m_fMass = 10.0F;
    obj->m_fTurnMass = 25.0F;
    obj->m_fAirResistance = 0.99F;
    obj->m_fElasticity = 0.1F;
    obj->m_fBuoyancyConstant = BOAT_WHEEL_BUOYANCY; // Como el original: 0x3DDA740F
    obj->m_nObjectType = eObjectType::OBJECT_TEMPORARY;
    obj->SetIsStatic(false);
    obj->objectFlags.bIsPickup = false;
    obj->m_nRemovalTime = CTimer::GetTimeInMS() + 20'000;

    obj->m_vecMoveSpeed = m_vecMoveSpeed;
    if (GetUp().z <= 0.0F) {
        obj->m_vecMoveSpeed.z = 0.0F;
    } else {
        obj->m_vecMoveSpeed.z = 0.3F;
    }

    obj->m_vecTurnSpeed.Set(0.5F, m_vecTurnSpeed.y * 2.0F, m_vecTurnSpeed.z * 2.0F);

    auto vecOffset = obj->GetPosition() - vecPos;
    vecOffset.Normalise();
    if (GetUp().z > 0.0F) {
        vecOffset += GetUp();
    }

    auto vecObjPos = obj->GetPosition() + vecOffset;
    obj->SetPosn(vecObjPos);

    CWorld::Add(obj);
    movingCompAtomic = nullptr;
    RwFrameForAllObjects(movingComponent, GetBoatAtomicObjectCB, &movingCompAtomic);
    if (movingCompAtomic) {
        RpAtomicSetFlags(movingCompAtomic, 0x0);
    }
}

// 0x6F00D0
RwObject* GetBoatAtomicObjectCB(RwObject* object, void* data) {
    if (RpAtomicGetFlags(object) & rpATOMICRENDER) {
        *static_cast<RpAtomic**>(data) = reinterpret_cast<RpAtomic*>(object);
    }

    return object;
}
