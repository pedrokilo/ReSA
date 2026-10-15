/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTrailer
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Trailer.h"
#include "VehicleRecording.h"
#include "X87Math.h"

// define?
constexpr float BAGGAGE_TRAILER_TOWED_RATIO = -1000.0f;

const float TRAILER_TOWED_MINRATIO = 0.9f; // 0x8D346C
const float TRAILER_SUPPORT_RETRACTION_RATE = 0.002f; // 0x871C14
const float TRAILER_SUPPORT_EXTENSION_RATE = 0.002f; // 0x871C18
const float TRAILER_SUPPORT_WAIT_EXTENSION_RATE = 0.0005f; // 0x871C1C

const float TRAILER_TOWBAR_OFFSET_Y = -0.05f; // 0x871C20

const float RELINK_TRAILER_DIFF_LIMIT_XY = 0.4f; // 0x8D3470
const float RELINK_TRAILER_DIFF_LIMIT_Z = 1.0f; // 0x8D3474

const float TWO_PI_EXE = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC

namespace {
// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
CVector TransformPoint59C890(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// Como el original: CColPoint::operator= del EXE (0x40FC80) no copia field_C, field_1C ni el relleno 0x26
void CopyColPoint(CColPoint& dst, const CColPoint& src) {
    dst.m_vecPoint       = src.m_vecPoint;
    dst.m_vecNormal      = src.m_vecNormal;
    dst.m_nSurfaceTypeA  = src.m_nSurfaceTypeA;
    dst.m_nPieceTypeA    = src.m_nPieceTypeA;
    dst.m_nLightingA     = src.m_nLightingA;
    dst.m_nSurfaceTypeB  = src.m_nSurfaceTypeB;
    dst.m_nPieceTypeB    = src.m_nPieceTypeB;
    dst.m_nLightingB     = src.m_nLightingB;
    dst.m_fDepth         = src.m_fDepth;
}

// Como el original: rumbo de CPlaceable con fpatan (atan2(-forward.x, forward.y)) o el de m_placement sin matriz,
// guardado a float.
float HeadingX87(const CPlaceable* placeable) {
    if (placeable->m_matrix) {
        const CVector& forward = placeable->m_matrix->GetForward();
        return TrigAtan2Cpp(-forward.x, forward.y); // X87Math.cpp: float(atan2(y, x)) con un solo redondeo
    }
    return placeable->m_placement.m_fHeading;
}

// Como el original (0x6CF5FC): ángulo de giro de los remolques de equipaje hacia el rumbo del tractor. Los dos
// rumbos quedan sin redondear (fpatan) y se comparan con rumbo propio ± π antes de restar (X87Math.cpp).
float SteerTowardsTractor(const CPlaceable* tractor, const CPlaceable* self) {
    const float* tractorForward = tractor->m_matrix ? &tractor->m_matrix->GetForward().x : nullptr;
    const float* selfForward    = self->m_matrix ? &self->m_matrix->GetForward().x : nullptr;
    return TrailerSteerCpp(tractorForward, tractor->m_placement.m_fHeading, selfForward, self->m_placement.m_fHeading);
}
} // namespace

void CTrailer::InjectHooks() {
    RH_ScopedVirtualClass(CTrailer, 0x871c28, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6D03A0);
    RH_ScopedInstall(ScanForTowLink, 0x6CF030);

    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6CF1A0);
    RH_ScopedVMTInstall(SetTowLink, 0x6CFDF0);
    RH_ScopedVMTInstall(ResetSuspension, 0x6CEE50);
    RH_ScopedVMTInstall(ProcessSuspension, 0x6CF6A0);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6CFFD0);
    RH_ScopedVMTInstall(ProcessControl, 0x6CED20);
    RH_ScopedVMTInstall(ProcessAI, 0x6CF590);
    RH_ScopedVMTInstall(PreRender, 0x6CFAC0);
    RH_ScopedVMTInstall(GetTowHitchPos, 0x6CEEA0);
    RH_ScopedVMTInstall(GetTowBarPos, 0x6CFD60);
    RH_ScopedVMTInstall(BreakTowLink, 0x6CEFB0);
}

// 0x6D03A0
CTrailer::CTrailer(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, false) {
    m_nVehicleSubType = VEHICLE_TYPE_TRAILER;

    if (m_nModelIndex == MODEL_BAGBOXA || m_nModelIndex == MODEL_BAGBOXB) {
        m_fTrailerTowedRatio = BAGGAGE_TRAILER_TOWED_RATIO;
    }

    SetupSuspensionLines();
    SetStatus(eEntityStatus::STATUS_ABANDONED);
}

// 0x6CF1A0
void CTrailer::SetupSuspensionLines() {
    if (m_fTrailerTowedRatio == BAGGAGE_TRAILER_TOWED_RATIO) {
        CAutomobile::SetupSuspensionLines();
        return;
    }

    auto* mi = CModelInfo::ms_modelInfoPtrs[m_nModelIndex]->AsVehicleModelInfoPtr();
    CColModel* cl = GetColModel();

    CCollisionData* colData = cl->m_pColData;

    if (!colData->m_pLines) {
        colData->m_nNumLines = NUM_TRAILER_SUSP_LINES;
        colData->m_pLines = (CColLine*)CMemoryMgr::Malloc(sizeof(CColLine) * NUM_TRAILER_SUSP_LINES);
    }

    for (int32 i = 0; i < NUM_TRAILER_WHEELS; ++i) {
        CVector wheelPos;
        mi->GetWheelPosn(i, wheelPos, false);
        m_wheelPosition[i] = wheelPos.z;

        CVector lineStart = wheelPos;
        lineStart.z += m_pHandlingData->m_fSuspensionUpperLimit;
        colData->m_pLines[i].m_vecStart = lineStart;

        CVector lineEnd = lineStart;
        lineEnd.z += m_pHandlingData->m_fSuspensionLowerLimit - m_pHandlingData->m_fSuspensionUpperLimit - mi->m_fWheelSizeFront / 2;
        colData->m_pLines[i].m_vecEnd = lineEnd;

        m_aSuspensionSpringLength[i] = m_pHandlingData->m_fSuspensionUpperLimit - m_pHandlingData->m_fSuspensionLowerLimit;
        m_aSuspensionLineLength[i] = colData->m_pLines[i].m_vecStart.z - colData->m_pLines[i].m_vecEnd.z;
    }

    cl->m_boundBox.m_vecMin.z = std::min(cl->m_boundBox.m_vecMin.z, colData->m_pLines[0].m_vecEnd.z);

    // 0x6CF313 - Como el original: se queda con |min| solo si es estrictamente mayor (NaN -> |max|)
    const float minDist = cl->m_boundBox.m_vecMin.Magnitude();
    const float maxDist = minDist > cl->m_boundBox.m_vecMax.Magnitude() ? minDist : cl->m_boundBox.m_vecMax.Magnitude();
    if (maxDist > cl->m_boundSphere.m_fRadius) {
        cl->m_boundSphere.m_fRadius = maxDist;
    }

    if (m_aCarNodes[TRAILER_MISC_A]) {
        RwMatrix matrix;
        RwFrame* wheelFront = m_aCarNodes[TRAILER_MISC_A];
        matrix = *RwFrameGetMatrix(wheelFront);

        if (auto parent = RwFrameGetParent(wheelFront)) {
            do {
                RwMatrixTransform(&matrix, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
                parent = RwFrameGetParent(parent);
            } while (parent != wheelFront && parent);
        }
        m_fHeight = matrix.pos.z;
    }

    CColLine* supportLines = &colData->m_pLines[NUM_TRAILER_WHEELS];
    for (int32 i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
        CVector startPos;
        startPos.x = (i == 0) ? cl->m_boundBox.m_vecMin.x : -cl->m_boundBox.m_vecMin.x;
        startPos.y = cl->m_boundBox.m_vecMax.y;
        startPos.z = cl->m_boundBox.m_vecMax.z;
        supportLines[i].m_vecStart = startPos;

        CVector endPos = startPos;
        endPos.z = cl->m_boundBox.m_vecMin.z;
        supportLines[i].m_vecEnd = endPos;
    }

    const float supportLegY = fabs(supportLines[0].m_vecStart.y);
    // Como el original: valor absoluto de la suma (y1 + y0), no suma de valores absolutos
    const float avgWheelY = fabs(colData->m_pLines[1].m_vecStart.y + colData->m_pLines[0].m_vecStart.y) * 0.5f;
    const float weightRatio = supportLegY / (avgWheelY + supportLegY);

    const float suspensionForceFactor = m_pHandlingData->m_fSuspensionForceLevel * 4.0f;
    // Como el original: P + (rueda / 2 - inicio.z)
    const float heightAboveRoad = m_aSuspensionSpringLength[0] * (1.0f - weightRatio / suspensionForceFactor)
                                + (mi->m_fWheelSizeFront * 0.5f - colData->m_pLines[0].m_vecStart.z);

    m_fFrontHeightAboveRoad = heightAboveRoad;
    m_fRearHeightAboveRoad  = heightAboveRoad;

    const float supportForce = CTrailer::m_fTrailerSuspensionForce * 2.0f;
    const float newEndZ = supportLines[0].m_vecStart.z - (supportLines[0].m_vecStart.z + heightAboveRoad)
                        / (1.0f - (1.0f - weightRatio) / supportForce);

    for (int i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
        supportLines[i].m_vecEnd.z = newEndZ;
    }
    // Como el original: las cuatro posiciones de rueda (la DLL solo escribía dos)
    for (int i = 0; i < NUM_TRAILER_WHEELS; ++i) {
        m_wheelPosition[i] = mi->m_fWheelSizeFront * 0.5f - m_fFrontHeightAboveRoad;
    }
}

// 0x6CFDF0
// Establece las referencias mutuas del remolque y actualiza su pertenencia a las
// listas de movimiento; opcionalmente alinea el enganche con la barra tractora.
bool CTrailer::SetTowLink(CVehicle* vehicle, bool setMyPosToTowBar) {
    if (!vehicle) {
        return false;
    }

    const auto status = GetStatus();
    if (status != STATUS_PHYSICS && status != STATUS_IS_TOWED && status != STATUS_ABANDONED) {
        return false;
    }

    SetStatus(STATUS_IS_TOWED);

    m_pTowingVehicle = vehicle;
    m_pTowingVehicle->RegisterReference(m_pTowingVehicle);

    vehicle->m_pVehicleBeingTowed = this;
    vehicle->m_pVehicleBeingTowed->RegisterReference(vehicle->m_pVehicleBeingTowed);

    CPhysical::RemoveFromMovingList();
    vehicle->RemoveFromMovingList();

    AddToMovingList();
    vehicle->AddToMovingList();

    if (!setMyPosToTowBar) {
        vehicle->UpdateTractorLink(true, false); // Como el original: sobre el tractor (ecx = vehículo)
        vehicle->m_vehicleAudio.AddAudioEvent(AE_TRAILER_ATTACH, 0.0f);
        return true;
    }

    // Como el original: solo se recoge si es mayor que el valor de equipaje (la DLL hacía un clamp con límites invertidos)
    if (m_fTrailerTowedRatio > BAGGAGE_TRAILER_TOWED_RATIO) {
        m_fTrailerTowedRatio = 0.0f;
    }

    SetHeading(HeadingX87(vehicle)); // Como el original: rumbo del tractor (fpatan), no el propio

    CVector towBarPos{}, towHitchPos{};
    if (!GetTowHitchPos(towHitchPos, true, this) || !vehicle->GetTowBarPos(towBarPos, true, this)) {
        return false;
    }

    towHitchPos -= GetPosition();

    SetPosn(towBarPos - towHitchPos);
    m_vecMoveSpeed = vehicle->m_vecMoveSpeed;
    PlaceOnRoadProperly();

    vehicle->m_vehicleAudio.AddAudioEvent(AE_TRAILER_ATTACH, 0.0f);
    return true;
}

// 0x6CF030
// Vuelve a enlazar un remolque abandonado con un vehículo cercano si las
// posiciones transformadas del enganche y la barra cumplen los límites fijados.
void CTrailer::ScanForTowLink() {
    if (m_pTowingVehicle || CVehicleRecording::IsPlaybackGoingOnForCar(this) || m_nModelIndex == MODEL_FARMTR1) {
        return;
    }

    CVector towHitchPos{}, towBarPos{}; // Como el original: ambos a cero una sola vez (towBarPos se reutiliza en el bucle)
    if (!GetTowHitchPos(towHitchPos, false, this)) {
        return;
    }

    int16 count = 0;
    std::array<CEntity*, 16> objects{};
    CWorld::FindObjectsInRange(towHitchPos, 10.0f, true, &count, int16(objects.size()), objects.data(), false, true, false, false, false);

    for (const auto& entity : std::span{ objects.data(), (size_t)count }) {
        if (auto* vehicle = entity->AsVehicle()) {
            if (vehicle == this) {
                continue;
            }

            if (!vehicle->GetTowBarPos(towBarPos, false, this)) {
                continue;
            }

            if (DistanceBetweenPoints2D(towBarPos, towHitchPos) < RELINK_TRAILER_DIFF_LIMIT_XY && std::fabs(towHitchPos.z - towBarPos.z) < RELINK_TRAILER_DIFF_LIMIT_Z) {
                SetTowLink(vehicle, false);
                return;
            }
        }
    }
}

// 0x6CEE50
void CTrailer::ResetSuspension() {
    CAutomobile::ResetSuspension();
    rng::fill(m_supportRatios, 1.f);
    if (m_pTowingVehicle && m_fTrailerTowedRatio > BAGGAGE_TRAILER_TOWED_RATIO) {
        m_fTrailerTowedRatio = 0.0f;
    } else {
        m_fTrailerTowedRatio = 1.0f;
    }
}

// 0x6CF6A0
void CTrailer::ProcessSuspension() {
    CAutomobile::ProcessSuspension();
    if (m_fTrailerTowedRatio == BAGGAGE_TRAILER_TOWED_RATIO) {
        return;
    }

    CVector contactPoints[NUM_TRAILER_SUPPORTS]{};
    CVector contactSpeeds[NUM_TRAILER_SUPPORTS]{};
    float impulseMagnitudes[NUM_TRAILER_SUPPORTS]{};

    // Como el original: la condición se evalúa con comparaciones directas (un NaN la hace falsa)
    const auto IsSupportTouching = [this](int32 i) {
        return m_supportRatios[i] < m_fTrailerTowedRatio2 && m_fTrailerTowedRatio2 > 0.1f;
    };

    // Como el original: tres pasadas separadas (muelles de las dos patas, luego velocidades, luego amortiguación),
    // no las tres cosas por pata.
    for (int32 i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
        if (!IsSupportTouching(i)) {
            continue;
        }
        const float compression = m_supportRatios[i] / m_fTrailerTowedRatio2;
        CVector forceDir = m_matrix->GetDown();
        contactPoints[i] = m_supportCPs[i].m_vecPoint - GetPosition();
        CPhysical::ApplySpringCollisionAlt(m_fTrailerSuspensionForce, forceDir, contactPoints[i], compression, 1.0f, m_supportCPs[i].m_vecNormal, impulseMagnitudes[i]);
    }

    for (int32 i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
        if (IsSupportTouching(i)) {
            contactSpeeds[i] = GetSpeed(contactPoints[i]);
        }
    }

    for (int32 i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
        if (!IsSupportTouching(i)) {
            continue;
        }
        CVector dampeningDir = m_matrix->GetDown();
        if (m_supportCPs[i].m_vecNormal.z > 0.35f) {
            dampeningDir = -m_supportCPs[i].m_vecNormal;
        }
        CPhysical::ApplySpringDampening(m_fTrailerDampingForce, impulseMagnitudes[i], dampeningDir, contactPoints[i], contactSpeeds[i]);
    }

    const auto status = GetStatus();
    if (status != STATUS_IS_TOWED && status != STATUS_IS_SIMPLE_TOWED) {
        eCarWheelStatus originalWheelStatus[NUM_TRAILER_SUPPORTS];
        for (int32 i = 0; i < NUM_TRAILER_SUPPORTS; ++i) {
            originalWheelStatus[i] = m_damageManager.GetWheelStatus((eCarWheel)i);

            if (IsSupportTouching(i)) {
                contactSpeeds[i] = GetSpeed(contactPoints[i]);
                m_damageManager.SetWheelStatus((eCarWheel)i, eCarWheelStatus::WHEEL_STATUS_OK);
            } else {
                m_damageManager.SetWheelStatus((eCarWheel)i, eCarWheelStatus::WHEEL_STATUS_MISSING); // Como el original: 2 (la DLL ponía BURST = 1)
            }
        }

        // Como el original: escala global 0xC1CB28 (1 / (50 · 50) · 10, calculada en float al iniciar; no es 0.004f exacto)
        const float traction = StaticRef<float>(0xC1CB28) * m_pHandlingData->m_fTractionMultiplier * 0.25f;
        CAutomobile::ProcessCarWheelPair(CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_REAR_LEFT, 0.0f, contactSpeeds, contactPoints, traction, 0.0f, 1000.0f, true);

        m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_LEFT, originalWheelStatus[0]);
        m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_LEFT, originalWheelStatus[1]);
    }
}

// 0x6CFFD0
int32 CTrailer::ProcessEntityCollision(CEntity* entity, CColPoint* outColPoints) {
    if (m_fTrailerTowedRatio == BAGGAGE_TRAILER_TOWED_RATIO) {
        return CAutomobile::ProcessEntityCollision(entity, outColPoints);
    }

    if (GetStatus() != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true;
    }

    const auto tcd = GetColData();

    // Como el original: sin comprobar que haya datos de colisión
    if (physicalFlags.bSkipLineCol || physicalFlags.bProcessingShift || entity->GetIsTypePed() || entity->GetIsTypeVehicle()) {
        tcd->m_nNumLines = 0; // Later reset back to original value
    }

    // Como el original: con el tractor o el remolque se ocultan los triángulos de ambos; se guardan con signo
    // (movsx) y solo se reponen si el valor guardado es >= 0
    int32 savedThisNumTriangles = -1, savedEntityNumTriangles = -1;
    if (m_pTowingVehicle == entity || m_pVehicleBeingTowed == entity) {
        savedThisNumTriangles = (int16)tcd->m_nNumTriangles;
        tcd->m_nNumTriangles = 0;
        savedEntityNumTriangles = (int16)entity->GetColData()->m_nNumTriangles;
        entity->GetColData()->m_nNumTriangles = 0;
    }

    std::array<CColPoint, NUM_TRAILER_SUSP_LINES> suspLineCPs;
    std::array<float, NUM_TRAILER_SUSP_LINES> suspLineTouchDists;

    rng::copy(m_fWheelsSuspensionCompression, suspLineTouchDists.begin());
    rng::copy(m_supportRatios, suspLineTouchDists.begin() + m_fWheelsSuspensionCompression.size());

    const auto numColPts = CCollision::ProcessColModels(
        GetMatrix(), *GetColModel(),
        entity->GetMatrix(), *entity->GetColModel(),
        *(std::array<CColPoint, 32>*)(outColPoints),
        suspLineCPs.data(),
        suspLineTouchDists.data(),
        false
    );

    // Restore hidden triangles
    if (savedThisNumTriangles >= 0) {
        tcd->m_nNumTriangles = (uint16)savedThisNumTriangles;
    }
    if (savedEntityNumTriangles >= 0) {
        entity->GetColData()->m_nNumTriangles = (uint16)savedEntityNumTriangles;
    }

    int32 numProcessedLines{};
    if (tcd->m_nNumLines) {
        // Process the real wheel's susp lines
        for (auto i = 0; i < NUM_TRAILER_WHEELS; i++) {
            // 0x6AD0D4
            const auto touchDist = suspLineTouchDists[i];
            if (touchDist >= BILLS_EXTENSION_LIMIT || touchDist >= m_fWheelsSuspensionCompression[i]) {
                continue;
            }
            numProcessedLines++;
            m_fWheelsSuspensionCompression[i] = touchDist;
            CopyColPoint(m_wheelColPoint[i], suspLineCPs[i]);
            m_anCollisionLighting[i] = m_wheelColPoint[i].m_nLightingB;

            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT: {
                // Como el original: se asigna y se registra la referencia sin limpiar la anterior; no copia la
                // iluminación del vehículo
                m_apWheelCollisionEntity[i] = entity->AsPhysical();
                m_apWheelCollisionEntity[i]->RegisterReference(reinterpret_cast<CEntity**>(&m_apWheelCollisionEntity[i]));
                m_vWheelCollisionPos[i] = m_wheelColPoint[i].m_vecPoint - entity->GetPosition();
                break;
            }
            default:
                break;
            }

            m_nContactSurface = m_wheelColPoint[i].m_nSurfaceTypeB;
            if (entity->GetIsTypeBuilding()) {
                m_pEntityWeAreOn = entity;
                m_bTunnel = entity->m_bTunnel;
                m_bTunnelTransition = entity->m_bTunnelTransition;
            }
        }

        // Process trailer support lines (Como el original: después del bucle de ruedas, no dentro)
        for (auto i = 0; i < NUM_TRAILER_SUPPORTS; i++) {
            const auto suspLineIdx = NUM_TRAILER_WHEELS + i;

            const auto touchDist = suspLineTouchDists[suspLineIdx];
            if (touchDist >= BILLS_EXTENSION_LIMIT || touchDist >= m_supportRatios[i]) {
                continue;
            }

            numProcessedLines++;
            m_supportRatios[i] = touchDist;
            CopyColPoint(m_supportCPs[i], suspLineCPs[suspLineIdx]);
        }
    } else {
        tcd->m_nNumLines = NUM_TRAILER_SUSP_LINES;
    }

    if (numColPts > 0 || numProcessedLines > 0) {
        AddCollisionRecord(entity);
        if (!entity->GetIsTypeBuilding()) {
            entity->AsPhysical()->AddCollisionRecord(this);
        }
        // Como el original: ambos casos exigen puntos de colisión
        if (numColPts > 0 && (entity->GetIsTypeBuilding()
            || (entity->GetIsTypeObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce))) {
            SetHasHitWall(true);
        }
    }

    return numColPts;
}

// 0x6CED20
// Ejecuta primero el control del automóvil y después retrae o extiende los
// soportes; al remolcarlo, aplica el límite mínimo de extensión correspondiente.
void CTrailer::ProcessControl() {
    CAutomobile::ProcessControl();
    if ((m_nFlags & 0x40) == 0 && m_fTrailerTowedRatio != BAGGAGE_TRAILER_TOWED_RATIO) {
        const float timeStep = CTimer::GetTimeStep();
        float& currentExtension = m_fTrailerTowedRatio;

        if (m_pTowingVehicle && currentExtension > 0.0f) {
            // up
            currentExtension = std::max(currentExtension - timeStep * TRAILER_SUPPORT_RETRACTION_RATE, 0.0f);
        } else if (!m_pTowingVehicle && currentExtension < 1.0f) {
            // down
            // Como el original: el tramo lento (<= 0.1) no se limita a 1
            if (currentExtension > 0.1f) {
                currentExtension = std::min(timeStep * TRAILER_SUPPORT_EXTENSION_RATE + currentExtension, 1.0f);
            } else {
                currentExtension = timeStep * TRAILER_SUPPORT_WAIT_EXTENSION_RATE + currentExtension;
            }
            m_nFakePhysics = 0;
        }

        const auto status = GetStatus();
        if ((status == STATUS_IS_TOWED || status == STATUS_IS_SIMPLE_TOWED) && TRAILER_TOWED_MINRATIO > currentExtension) {
            m_fTrailerTowedRatio2 = TRAILER_TOWED_MINRATIO;
        } else {
            m_fTrailerTowedRatio2 = currentExtension;
        }

        rng::fill(m_supportRatios, m_fTrailerTowedRatio2);
    }
}

// 0x6CF590
// Añade el comportamiento de remolque tras la IA del automóvil; los remolques
// de equipaje también orientan su dirección hacia el rumbo del tractor.
bool CTrailer::ProcessAI(uint32& extraHandlingFlags) {
    const auto returnValue = CAutomobile::ProcessAI(extraHandlingFlags);

    if (GetStatus() == STATUS_ABANDONED && (m_fTrailerTowedRatio > 0.5f || m_fTrailerTowedRatio == BAGGAGE_TRAILER_TOWED_RATIO)) {
        ScanForTowLink();
        return returnValue;
    }

    if (GetStatus() != STATUS_IS_TOWED || !m_pTowingVehicle) { // Como el original: estado 10 (remolcado)
        return returnValue;
    }

    if (m_fTrailerTowedRatio != BAGGAGE_TRAILER_TOWED_RATIO) {
        m_BrakePedal = m_pTowingVehicle->m_BrakePedal;
        return returnValue;
    }

    // Wheel turning for luggage trailers
    m_fSteerAngle = SteerTowardsTractor(m_pTowingVehicle, this);

    return returnValue;
}

// 0x6CFAC0
void CTrailer::PreRender() {
    CAutomobile::PreRender();

    // Support animation
    if (m_aCarNodes[TRAILER_MISC_A]) {
        auto* colData = CModelInfo::GetModelInfo(m_nModelIndex)->m_pColModel->m_pColData;
        if ((int8)colData->m_nNumLines > NUM_TRAILER_WHEELS && (!m_vecMoveSpeed.IsZero() || !m_vecTurnSpeed.IsZero() || !m_nFakePhysics)) {
            CMatrix mat;
            mat.Attach(&m_aCarNodes[TRAILER_MISC_A]->modelling, false);

            const float avgSupportRatio = (m_supportRatios[0] + m_supportRatios[1]) * 0.5f;
            const float supportExtensionRatio = std::min(avgSupportRatio, m_fTrailerTowedRatio);

            const auto& supportLegLine = colData->m_pLines[NUM_TRAILER_WHEELS];

            const float supportTravel = supportExtensionRatio * (supportLegLine.m_vecEnd.z - supportLegLine.m_vecStart.z);
            const float verticalPosRatio = mat.GetPosition().y / supportLegLine.m_vecStart.y;

            CVector newPos = mat.GetPosition();
            const float targetZ = (supportTravel + supportLegLine.m_vecStart.z) * verticalPosRatio
                - (1.0f - verticalPosRatio) * m_fFrontHeightAboveRoad;
            newPos.z = std::min(targetZ, m_fHeight);

            mat.SetTranslate(newPos);
            mat.UpdateRW();
        }
    }

    // Animation for FARMTR1
    if (m_nModelIndex == MODEL_FARMTR1) {
        // Como el original: resta 2π una sola vez si lo supera (no fmod)
        if (m_fPropRotate > TWO_PI_EXE) {
            m_fPropRotate = m_fPropRotate - TWO_PI_EXE;
        }
        // Como el original: (z·fz + y·fy) + x·fx
        const auto& forward = m_matrix->GetForward();
        const float forwardSpeed = m_vecMoveSpeed.z * forward.z + m_vecMoveSpeed.y * forward.y + m_vecMoveSpeed.x * forward.x;
        const float rotationSpeedFactor = std::clamp(forwardSpeed, 0.0f, 0.1f);
        m_fPropRotate -= rotationSpeedFactor * CTimer::GetTimeStep();
        CVehicle::SetComponentRotation(m_aCarNodes[TRAILER_MISC_B], AXIS_X, m_fPropRotate, true);
    }
}

// 0x6CEEA0
bool CTrailer::GetTowHitchPos(CVector& outPos, bool bCheckModelInfo, CVehicle* vehicle) {
    auto mi = GetVehicleModelInfo();
    outPos = mi->GetModelDummyPosition(DUMMY_TRAILER_ATTACH);

    if (vehicle && vehicle->m_nModelIndex == MODEL_TOWTRUCK) {
        return false;
    }

    if (outPos.IsZero()) {
        if (!bCheckModelInfo) {
            return false;
        }
        outPos.x = 0.0f;
        outPos.y = mi->GetColModel()->GetBoundingBox().m_vecMax.y + 1.0f;
        outPos.z = 0.5f - m_fFrontHeightAboveRoad;
        outPos = TransformPoint59C890(*m_matrix, outPos);
        return true;
    }
    outPos = TransformPoint59C890(*m_matrix, outPos);
    return true;
}

// 0x6CFD60
bool CTrailer::GetTowBarPos(CVector& outPos, bool bCheckModelInfo, CVehicle* vehicle) {
    if (!bCheckModelInfo) {
        if (m_nModelIndex != MODEL_BAGBOXA && m_nModelIndex != MODEL_BAGBOXB) {
            return false;
        }
    }

    auto mi = GetVehicleModelInfo();
    outPos.x = 0.0f;
    outPos.y = mi->GetColModel()->GetBoundingBox().m_vecMin.y - TRAILER_TOWBAR_OFFSET_Y;
    outPos.z = 0.5f - m_fFrontHeightAboveRoad;
    outPos = TransformPoint59C890(*m_matrix, outPos);
    return true;
}

// 0x6CEFB0
bool CTrailer::BreakTowLink() {
    if (m_pTowingVehicle) {
        m_pTowingVehicle->m_vehicleAudio.AddAudioEvent(AE_TRAILER_DETACH, 0.0f);
    }

    if (m_pTowingVehicle) {
        CEntity::ClearReference(m_pTowingVehicle->m_pVehicleBeingTowed);
        CEntity::ClearReference(m_pTowingVehicle);
    }

    const auto status = GetStatus();
    if (status != STATUS_IS_TOWED && status != STATUS_IS_SIMPLE_TOWED) {
        return false;
    }

    SetStatus(STATUS_ABANDONED);
    return true;
}
