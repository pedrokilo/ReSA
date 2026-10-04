/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: cBuoyancy
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Buoyancy.h"

auto& mod_Buoyancy = StaticRef<cBuoyancy>(0xC1C890);

namespace {
// Como el original: peso de la normal vertical al promediar la normal del agua en los barcos (0x8D3384, 2.0)
auto& fBuoyancyBoatNormalWeight = StaticRef<float>(0x8D3384);
// Como el original: velocidad máxima usada para las salpicaduras (0x8D3388, 0.1)
auto& fBuoyancySplashMaxSpeed = StaticRef<float>(0x8D3388);

// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE
CVector BuoyancyMultiply3x3(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y,
    };
}

// Como el original: MultiplyMatrixWithVector (0x59C890), con el orden de sumas del EXE
CVector BuoyancyMultiplyPoint(const CMatrix& m, const CVector& v) {
    return CVector{
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}
} // namespace

void cBuoyancy::InjectHooks()
{
    RH_ScopedClass(cBuoyancy);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ProcessBuoyancy, 0x6C3EF0);
    RH_ScopedInstall(ProcessBuoyancyBoat, 0x6C3030);
    RH_ScopedInstall(CalcBuoyancyForce, 0x6C2750);
    RH_ScopedInstall(PreCalcSetup, 0x6C2B90);
    RH_ScopedInstall(AddSplashParticles, 0x6C34E0);
    RH_ScopedInstall(SimpleCalcBuoyancy, 0x6C3B00);
    RH_ScopedInstall(SimpleSumBuoyancyData, 0x6C2970);
    RH_ScopedInstall(FindWaterLevel, 0x6C2810);
    RH_ScopedInstall(FindWaterLevelNorm, 0x6C28C0);
}

// 0x6C3EF0
bool cBuoyancy::ProcessBuoyancy(CPhysical* entity, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce) {
    CVector& entityPosition = entity->GetPosition();
    if (!CWaterLevel::GetWaterLevel(entityPosition, m_fWaterLevel, entity->physicalFlags.bTouchingWater)) {
        return false;
    }

    m_EntityMatrix = *entity->m_matrix;
    PreCalcSetup(entity, fBuoyancy);
    if (entity->GetIsTypePed()) {
        entity->GetColModel(); // for some reason, this is here?

        m_bInWater = true;
        // Como el original: (nivel - z - (-1.0)) * 0.5263158 (0x871664), no una división entre 1.9
        m_fEntityWaterImmersion = (m_fWaterLevel - entityPosition.z - -1.0F) * std::bit_cast<float>(0x3F06BCA2u);
        if (m_fEntityWaterImmersion > 1.0F) { // clamp
            m_fEntityWaterImmersion = 1.0F;
        } else if (m_fEntityWaterImmersion < 0.0F) {
            m_fEntityWaterImmersion = 0.0F;
            m_bInWater = false;
        }

        m_vecTurnPoint = CVector{};
        if (m_fEntityWaterImmersion > 0.0F && m_fEntityWaterImmersion < 1.0F) {
            const CVector dir = -entity->GetForwardVector();
            const CVector vec = CVector(0.0F, 0.0F, m_fWaterLevel - entityPosition.z);
            cBuoyancy::AddSplashParticles(entity, vec, vec, dir, true);
        }
    } else {
        SimpleCalcBuoyancy(entity);
    }

    bool bCalcBuoyancyForce = CalcBuoyancyForce(entity, vecBuoyancyTurnPoint, vecBuoyancyForce);
    if (m_bProcessingBoat || bCalcBuoyancyForce) {
        return true;
    }

    return false;
}

// 0x6C3030
bool cBuoyancy::ProcessBuoyancyBoat(CVehicle* vehicle, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce, bool bUnderwater) {
    const CVector& entityPosition = vehicle->GetPosition();
    if (!CWaterLevel::GetWaterLevel(entityPosition, m_fWaterLevel, vehicle->physicalFlags.bTouchingWater)) {
        return false;
    }

    m_EntityMatrix = *vehicle->m_matrix;
    PreCalcSetup(vehicle, fBuoyancy);
    const float fBoatHeightRatio = 1.0F / ((m_vecBoundingMax.z - m_vecBoundingMin.z) * 9.0F);

    // Como el original: las coordenadas del punto se acumulan sumando el desplazamiento al centro en cada paso
    float fCurrentX = m_vecBoundingMin.x;
    for (int32 iXMult = 0; iXMult < 3; ++iXMult) {
        float fCurrentY = m_vecBoundingMin.y;
        for (int32 iYMult = 0; iYMult < 3; ++iYMult) {
            CVector vecCurPoint(fCurrentX, fCurrentY, 0.0F);

            auto vecWaveNormal = CVector(0.0F, 0.0F, 1.0F);
            tWaterLevel eState;
            FindWaterLevelNorm(m_vecInitialZPos, &vecCurPoint, &eState, &vecWaveNormal);

            // Como el original: (normal + (0, 0, 1) * K) * (1 / (K + 1)), con K global (0x8D3384) y x, y sumando K * 0
            const float fZeroWeight = fBuoyancyBoatNormalWeight * 0.0F;
            const float fSumX = fZeroWeight + vecWaveNormal.x;
            const float fSumY = fZeroWeight + vecWaveNormal.y;
            const float fSumZ = fBuoyancyBoatNormalWeight + vecWaveNormal.z;
            const float fInvWeight = 1.0F / (fBuoyancyBoatNormalWeight + 1.0F);
            vecWaveNormal.x = fSumX * fInvWeight;
            vecWaveNormal.y = fSumY * fInvWeight;
            vecWaveNormal.z = fSumZ * fInvWeight;

            switch (vehicle->GetModelIndex()) {
            case MODEL_SQUALO:
            case MODEL_SPEEDER:
            case MODEL_JETMAX:
            case MODEL_LAUNCH:
                fPointVolMultiplier = afBoatVolumeDistributionSpeed[iYMult][iXMult];
                break;
            case MODEL_COASTG:
            case MODEL_DINGHY:
                fPointVolMultiplier = afBoatVolumeDistributionDinghy[iYMult][iXMult];
                break;
            case MODEL_MARQUIS:
                fPointVolMultiplier = afBoatVolumeDistributionSail[iYMult][iXMult];
                break;
            default:
                fPointVolMultiplier = afBoatVolumeDistribution[iYMult][iXMult];
                break;
            }

            if (eState != tWaterLevel::COMPLETELY_ABOVE_WATER) {
                const float fAddedDistToSurface = SimpleSumBuoyancyData(&vecCurPoint, eState);
                const float fPointContribution = CTimer::GetTimeStep() * m_fBuoyancy * (fAddedDistToSurface * fBoatHeightRatio);

                // Como el original: Multiply3x3 con la matriz del vehículo (sin crearla) y producto escalar z, y, x
                const CVector vecOffset(fCurrentX, fCurrentY, 0.0F);
                const CVector vecTransformedPos = BuoyancyMultiply3x3(*vehicle->m_matrix, vecOffset);

                const CVector vecSpeedAtPoint = vehicle->GetSpeed(vecTransformedPos);
                const float fDot = (vecSpeedAtPoint.z * vecWaveNormal.z + vecSpeedAtPoint.y * vecWaveNormal.y) + vecSpeedAtPoint.x * vecWaveNormal.x;
                float fWavePower = 1.0F - fDot * vehicle->m_pHandlingData->m_fSuspensionDampingLevel;
                if (0.0F > fWavePower) {
                    fWavePower = 0.0F;
                }
                fWavePower *= fPointContribution;
                vecBuoyancyForce->z = fWavePower + vecBuoyancyForce->z;

                if (!bUnderwater) {
                    const CVector vecTurnForceAtPoint(vecWaveNormal.x * fWavePower, vecWaveNormal.y * fWavePower, vecWaveNormal.z * fWavePower);
                    const CVector vecAppliedForcePoint = BuoyancyMultiply3x3(m_EntityMatrix, vecCurPoint);
                    vehicle->ApplyTurnForce(vecTurnForceAtPoint, vecAppliedForcePoint);
                }
            }
            fCurrentY += m_vecCenterOffset.y;
        }
        fCurrentX += m_vecCenterOffset.x;
    }

    m_fEntityWaterImmersion = fBoatHeightRatio * m_fEntityWaterImmersion;
    *vecBuoyancyTurnPoint = BuoyancyMultiply3x3(m_EntityMatrix, m_vecTurnPoint);

    if (m_bProcessingBoat)
        return true;

    return m_bInWater;
}

// 0x6C2750
bool cBuoyancy::CalcBuoyancyForce(CPhysical* entity, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce)
{
    if (!m_bInWater)
        return false;

    *vecBuoyancyTurnPoint = BuoyancyMultiply3x3(m_EntityMatrix, m_vecTurnPoint);
    const float fCurrentBuoyancy = m_fEntityWaterImmersion * m_fBuoyancy * CTimer::GetTimeStep();
    vecBuoyancyForce->Set(0.0F, 0.0F, fCurrentBuoyancy);

    const float fMoveForceZ = entity->m_fMass * entity->m_vecMoveSpeed.z;
    // Como el original: test ah,41h, un NaN también sale aquí
    if (!(fMoveForceZ > fCurrentBuoyancy * 4.0F))
        return true;

    // Como el original: (0 > d) ? 0 : d (conserva -0 y NaN)
    const float fBuoyancy = fCurrentBuoyancy - fMoveForceZ;
    vecBuoyancyForce->z = 0.0F > fBuoyancy ? 0.0F : fBuoyancy;
    return true;
}

// 0x6C2B90
void cBuoyancy::PreCalcSetup(CPhysical* entity, float fBuoyancy)
{
    CVehicle* vehicle = entity->AsVehicle();
    m_bProcessingBoat = entity->GetIsTypeVehicle() && vehicle->IsBoat();
    auto cm = entity->GetColModel();
    m_vecBoundingMin = cm->m_boundBox.m_vecMin;
    m_vecBoundingMax = cm->m_boundBox.m_vecMax;

    if (!m_bProcessingBoat) {
        switch (entity->GetModelIndex()) {
        case MODEL_LEVIATHN: //417
            m_vecBoundingMin.y *= 0.4F;
            m_vecBoundingMax.y *= 1.15F;
            break;
        case MODEL_SKIMMER: //460
            m_vecBoundingMin.x *= 0.3F;
            m_vecBoundingMin.y = m_vecBoundingMax.y * -0.6F;
            m_vecBoundingMin.z -= 0.1F;
            m_vecBoundingMax.x *= 0.3F;
            m_vecBoundingMax.y *= 1.4F;
            break;
        default:
            if (entity->GetIsTypeVehicle() && vehicle->IsSubHeli()) {
                m_vecBoundingMin.y = -m_vecBoundingMax.y;
                m_vecBoundingMax.z = m_vecBoundingMin.z * -1.1F;
                m_vecBoundingMin.z *= 0.85F;
            }
            break;
        }
    }
    else {
        switch (entity->GetModelIndex()) {
        case MODEL_SQUALO: //446
            m_vecBoundingMax.y *= 0.9F;
            m_vecBoundingMin.y *= 0.9F;
            break;
        case MODEL_SPEEDER: //452
            m_vecBoundingMax.y *= 1.25F;
            m_vecBoundingMin.y *= 0.83F;
            break;
        case MODEL_REEFER: //453
        case MODEL_JETMAX: //493
            m_vecBoundingMin.y *= 0.9F;
            break;
        case MODEL_TROPIC: //454
            m_vecBoundingMax.y *= 1.3F;
            m_vecBoundingMin.y *= 0.82F;
            m_vecBoundingMin.z -= 0.2F;
            break;
        case MODEL_COASTG: //472
            m_vecBoundingMax.y *= 1.1F;
            m_vecBoundingMin.y *= 0.9F;
            m_vecBoundingMin.z -= 0.3F;
            break;
        case MODEL_DINGHY: //473
            m_vecBoundingMax.y *= 1.3F;
            m_vecBoundingMin.y *= 0.9F;
            m_vecBoundingMin.z -= 0.2F;
            break;
        case MODEL_MARQUIS: //484
            m_vecBoundingMax.y *= 1.1F;
            m_vecBoundingMin.y *= 0.9F;
            break;
        case MODEL_LAUNCH: //595
            m_vecBoundingMax.y *= 1.25F;
            m_vecBoundingMin.y *= 0.8F;
            m_vecBoundingMin.z -= 0.1F;
            break;
        default:
            m_vecBoundingMax.y *= 1.05F;
            m_vecBoundingMin.y *= 0.9F;
            break;
        }
    }

    // Calculate center offset and "normalized" version of it
    // (it's not normalized vector, just the offset scaled by multiplicative inverse of it's longest component
    //  so the longest component is equal to 1.0F)
    m_vecCenterOffset = (m_vecBoundingMax - m_vecBoundingMin) * 0.5F;
    const CVector& c = m_vecCenterOffset;
    // Como el original: la componente mayor se escribe como 1.0 exacto (no c * (1 / c))
    if ((c.z > c.x) && (c.z > c.y)) {
        const float fScale = 1.0F / c.z;
        m_vecNormalizedCenterOffset.x = c.x * fScale;
        m_vecNormalizedCenterOffset.y = fScale * c.y;
        m_vecNormalizedCenterOffset.z = 1.0F;
    } else if ((c.y > c.x) && (c.y > c.z)) {
        const float fScale = 1.0F / c.y;
        m_vecNormalizedCenterOffset.x = c.x * fScale;
        m_vecNormalizedCenterOffset.y = 1.0F;
        m_vecNormalizedCenterOffset.z = fScale * c.z;
    } else {
        const float fScale = 1.0F / c.x;
        m_vecNormalizedCenterOffset.x = 1.0F;
        m_vecNormalizedCenterOffset.y = fScale * c.y;
        m_vecNormalizedCenterOffset.z = fScale * c.z;
    }

    m_fNumCheckedPoints = 1.0F;
    m_vecTurnPoint.Set(0.0F, 0.0F, 0.0F);
    m_bInWater = false;
    m_fEntityWaterImmersion = 0.0F;
    m_vecPos = entity->GetPosition();
    m_fWaterLevel = m_fUnkn2 + m_fWaterLevel;
    m_vecInitialZPos.Set(0.0F, 0.0F, m_vecPos.z);
    m_fBuoyancy = fBuoyancy;
}

// 0x6C34E0
void cBuoyancy::AddSplashParticles(CPhysical* entity, CVector vecFrom, CVector vecTo, CVector vecSplashDir, uint8 bReduceParticleSize)
{
    // Como el original: distancia con sumas z, y, x y la interpolación es from + (to - from) * t
    const CVector vecDelta{ vecTo.x - vecFrom.x, vecTo.y - vecFrom.y, vecTo.z - vecFrom.z };
    const float fDistBetweenPoints = std::sqrt((vecDelta.z * vecDelta.z + vecDelta.y * vecDelta.y) + vecDelta.x * vecDelta.x);
    CVector vecUsedSpeed = entity->m_vecMoveSpeed;
    float fMoveSpeed = std::sqrt((vecUsedSpeed.z * vecUsedSpeed.z + vecUsedSpeed.y * vecUsedSpeed.y) + vecUsedSpeed.x * vecUsedSpeed.x);
    if (fMoveSpeed > fBuoyancySplashMaxSpeed) {
        // Como el original: (v * (1 / |v|)) * 0.1, con 0.1 leído de 0x8D3388
        const float fMult = 1.0F / fMoveSpeed;
        vecUsedSpeed.x = vecUsedSpeed.x * fMult * fBuoyancySplashMaxSpeed;
        vecUsedSpeed.y = vecUsedSpeed.y * fMult * fBuoyancySplashMaxSpeed;
        vecUsedSpeed.z = vecUsedSpeed.z * fMult * fBuoyancySplashMaxSpeed;
        fMoveSpeed = fBuoyancySplashMaxSpeed;
    }

    // Como el original: test ah,41h, un NaN también sale (sin informar al audio)
    if (!(fMoveSpeed > 0.05F))
        return;

    FxPrtMult_c curParticle(1.0F, 1.0F, 1.0F, 0.1F, 0.5F, 1.0F, 0.2F);
    if (bReduceParticleSize) {
        curParticle.m_fSize *= 0.5F;
        curParticle.m_fLife *= 0.5F;
    }

    // Como el original: _ftol2 (truncamiento a 64 bits)
    const int32 iFloorParticles = static_cast<int32>(static_cast<int64>(fDistBetweenPoints + fDistBetweenPoints));
    const int32 iNumParticles = iFloorParticles < 1 ? 1 : iFloorParticles;

    const CVector vecVelocityModifier{ vecUsedSpeed.x * 0.0F * 120.0F, vecUsedSpeed.y * 0.0F * 120.0F, vecUsedSpeed.z * -1.0F * 120.0F };
    // Como el original: el EXE suma además m_vecMoveSpeed * 0 a la velocidad (solo cambia el signo de los ceros)
    const CVector& vecEntityMoveSpeed = entity->m_vecMoveSpeed;
    const CVector vecZeroSpeed{ vecEntityMoveSpeed.x * 0.0F, vecEntityMoveSpeed.y * 0.0F, vecEntityMoveSpeed.z * 0.0F * 0.0F };

    const float fNumParticles = static_cast<float>(iNumParticles);
    for (int32 iIter = 0; iIter < iNumParticles; ++iIter) {
        const float fCurrentProgress = static_cast<float>(iIter) / fNumParticles;
        const CVector vecCurPoint{
            vecDelta.x * fCurrentProgress + vecFrom.x,
            vecDelta.y * fCurrentProgress + vecFrom.y,
            vecDelta.z * fCurrentProgress + vecFrom.z,
        };
        CVector vecTransformedPoint = BuoyancyMultiplyPoint(*entity->m_matrix, vecCurPoint);

        if (!entity->GetIsTypePed()) {
            const auto& vecEntPos = entity->GetPosition();
            vecSplashDir = vecTransformedPoint - vecEntPos;
        }
        vecSplashDir.z = 0.0F;
        vecSplashDir.Normalise();

        const CVector vecVelocity{
            (vecSplashDir.x * fMoveSpeed * 60.0F + vecVelocityModifier.x) + vecZeroSpeed.x,
            (vecSplashDir.y * fMoveSpeed * 60.0F + vecVelocityModifier.y) + vecZeroSpeed.y,
            (vecSplashDir.z * fMoveSpeed * 60.0F + vecVelocityModifier.z) + vecZeroSpeed.z,
        };

        // Como el original: rand() * (1 / 32767) * 0.5
        const float fRand = static_cast<float>(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000100u) * 0.5F;
        vecTransformedPoint.x = vecSplashDir.x * fRand + vecTransformedPoint.x;
        vecTransformedPoint.y = vecSplashDir.y * fRand + vecTransformedPoint.y;
        vecTransformedPoint.z = vecSplashDir.z * fRand + vecTransformedPoint.z;
        g_fx.m_WaterSplash->AddParticle(vecTransformedPoint, vecVelocity, 0.0F, curParticle);
    }

    if (entity->GetIsTypePed()) {
        auto ped = entity->AsPed();
        // Como el original: el ángulo, el multiplicador y la posición se calculan antes de consultar la tarea de nadar
        const CVector vecMatForward = entity->m_matrix->GetForward();
        float fPedAngle = CGeneral::GetAngleBetweenPoints(vecMatForward.x, vecMatForward.y, 0.0F, 0.0F);
        fPedAngle = CGeneral::LimitAngle(fPedAngle) + 180.0F;

        const FxPrtMult_c wakeParticle(1.0F, 1.0F, 1.0F, 0.2F, 0.4F, 0.0F, 0.5F);
        const CVector vecPedForward = entity->GetForwardVector();
        const CVector& vecPedPos = entity->GetPosition();
        CVector vecPedParticlePos{
            vecPedForward.x * 0.4F + vecPedPos.x,
            vecPedForward.y * 0.4F + vecPedPos.y,
            vecPedForward.z * 0.4F + vecPedPos.z,
        };

        if (!ped->GetIntelligence()->GetTaskSwim()) {
            if (ped->GetPlayerData())
                vecPedParticlePos.z = ped->GetPlayerData()->m_fWaterHeight;
            else
                vecPedParticlePos.z += 0.5F;

            g_fx.m_Wake->AddParticle(vecPedParticlePos, CVector{ 0.0F, 0.0F, 0.0F }, 0.0F, wakeParticle, fPedAngle);
            ped->GetAE().AddAudioEvent(eAudioEvents::AE_PED_SWIM_WAKE);
        }
    }

    AudioEngine.ReportWaterSplash(entity, -100.0F, false);
}

// 0x6C3B00
void cBuoyancy::SimpleCalcBuoyancy(CPhysical* entity)
{
    CVector vecAllPoints[3][3];

    // Como el original: las coordenadas del punto se acumulan sumando el desplazamiento al centro en cada paso
    float fCurrentX = m_vecBoundingMin.x;
    for (int32 iXMult = 0; iXMult < 3; ++iXMult) {
        float fCurrentY = m_vecBoundingMin.y;
        for (int32 iYMult = 0; iYMult < 3; ++iYMult) {
            auto& pCurVec = vecAllPoints[iXMult][iYMult];
            pCurVec.Set(0.0F, 0.0F, 0.0F);

            CVector vecCurPoint(fCurrentX, fCurrentY, 0.0F);
            tWaterLevel eState;
            FindWaterLevel(m_vecInitialZPos, &vecCurPoint, &eState);
            cBuoyancy::fPointVolMultiplier = 1.0F;

            if (eState == tWaterLevel::COMPLETELY_ABOVE_WATER)
                pCurVec.z = m_vecBoundingMin.z;
            else {
                SimpleSumBuoyancyData(&vecCurPoint, eState);
                pCurVec = vecCurPoint;
            }
            fCurrentY += m_vecCenterOffset.y;
        }
        fCurrentX += m_vecCenterOffset.x;
    }

    auto fHeight = m_vecBoundingMax.z - m_vecBoundingMin.z;
    m_fEntityWaterImmersion = m_fEntityWaterImmersion / (9.0F * fHeight);
    auto fHalfHeightZ = fHeight * 0.5F + m_vecBoundingMin.z;

    for (int32 i = 0; i < 2; ++i) {
        auto fMinYHeight = vecAllPoints[i][0].z;            // 000      000     ^y
        auto fMinYHeight2 = vecAllPoints[i + 1][0].z;       // 000  ->  000     │   x
        if (fMinYHeight > fHalfHeightZ                      // xx0      0xx     └───>
            && fMinYHeight < m_vecBoundingMax.z
            && fMinYHeight2 > fHalfHeightZ
            && fMinYHeight2 < m_vecBoundingMax.z) {

            CVector vecDir(0.0F, -1.0F, 0.0F);
            AddSplashParticles(entity, vecAllPoints[i][0], vecAllPoints[i + 1][0], vecDir, false);
        }

        auto fMaxYHeight = vecAllPoints[i][2].z;            // xx0      0xx     ^y
        auto fMaxYHeight2 = vecAllPoints[i + 1][2].z;       // 000  ->  000     │   x
        if (fMaxYHeight > fHalfHeightZ                      // 000      000     └───>
            && fMaxYHeight < m_vecBoundingMax.z
            && fMaxYHeight2 > fHalfHeightZ
            && fMaxYHeight2 < m_vecBoundingMax.z) {

            CVector vecDir(0.0F, 1.0F, 0.0F);
            AddSplashParticles(entity, vecAllPoints[i][2], vecAllPoints[i + 1][2], vecDir, false);
        }
    }

    for (int32 i = 0; i < 2; ++i) {
        auto fMinXHeight = vecAllPoints[0][i].z;            // 000      x00     ^y
        auto fMinXHeight2 = vecAllPoints[0][i + 1].z;       // x00  ->  x00     │   x
        if (fMinXHeight > fHalfHeightZ                      // x00      000     └───>
            && fMinXHeight < m_vecBoundingMax.z
            && fMinXHeight2 > fHalfHeightZ
            && fMinXHeight2 < m_vecBoundingMax.z) {

            CVector vecDir(-1.0F, 0.0F, 0.0F);
            AddSplashParticles(entity, vecAllPoints[0][i], vecAllPoints[0][i + 1], vecDir, false);
        }

        auto fMaxXHeight = vecAllPoints[2][i].z;            // 000      00x     ^y
        auto fMaxXHeight2 = vecAllPoints[2][i + 1].z;       // 00x  ->  00x     │   x
        if (fMaxXHeight > fHalfHeightZ                      // 00x      000     └───>
            && fMaxXHeight < m_vecBoundingMax.z
            && fMaxXHeight2 > fHalfHeightZ
            && fMaxXHeight2 < m_vecBoundingMax.z) {

            // Como el original: los extremos son los mismos puntos que se comprueban ([2][i] y [2][i + 1])
            CVector vecDir(1.0F, 0.0F, 0.0F);
            AddSplashParticles(entity, vecAllPoints[2][i], vecAllPoints[2][i + 1], vecDir, false);
        }
    }
}

// 0x6C2970
float cBuoyancy::SimpleSumBuoyancyData(CVector* vecWaterOffset, tWaterLevel ePointState)
{
    // Como el original: guarda de inicialización estática, solo prueba y pone el bit 0
    auto& summedFlags = reinterpret_cast<uint8&>(cBuoyancy::calcStruct.bBuoyancyDataSummed);
    if (!(summedFlags & 1))
        summedFlags |= 1;

    auto fPointDistanceToWaterSurface = fabs(vecWaterOffset->z - m_vecBoundingMin.z) - (1.0F - cBuoyancy::fPointVolMultiplier);
    cBuoyancy::calcStruct.fAddedDistToWaterSurface = fPointDistanceToWaterSurface;
    if (fPointDistanceToWaterSurface < 0.0F)
        return 0.0F;

    if (m_bProcessingBoat) {
        // Como el original: cuadrado con fmul (no pow)
        const float fScaled = cBuoyancy::fPointVolMultiplier * cBuoyancy::calcStruct.fAddedDistToWaterSurface;
        cBuoyancy::calcStruct.fAddedDistToWaterSurface = fScaled * fScaled;
    }

    m_fEntityWaterImmersion += cBuoyancy::calcStruct.fAddedDistToWaterSurface;

    cBuoyancy::calcStruct.vecCurOffsetTurnPoint.z = (vecWaterOffset->z + m_vecBoundingMin.z) * m_vecNormalizedCenterOffset.z * 0.5F;
    cBuoyancy::calcStruct.vecCurOffsetTurnPoint.x = m_vecNormalizedCenterOffset.x * vecWaterOffset->x;
    cBuoyancy::calcStruct.vecCurOffsetTurnPoint.y = m_vecNormalizedCenterOffset.y * vecWaterOffset->y;

    if (m_bFlipUnknVector)
        cBuoyancy::calcStruct.vecCurOffsetTurnPoint *= -1.0F;

    // Rolling average calculation, averages the move force from all checked points
    cBuoyancy::calcStruct.fNewPointContribution = 1.0F / m_fNumCheckedPoints;
    cBuoyancy::calcStruct.fCurrentAverageContribution = 1.0F - cBuoyancy::calcStruct.fNewPointContribution;
    auto vecNewPointContribution = cBuoyancy::calcStruct.vecCurOffsetTurnPoint * cBuoyancy::calcStruct.fNewPointContribution * cBuoyancy::calcStruct.fAddedDistToWaterSurface;
    auto vecCurrentAverageContibution = m_vecTurnPoint * cBuoyancy::calcStruct.fCurrentAverageContribution;
    m_vecTurnPoint = vecNewPointContribution + vecCurrentAverageContibution;

    m_fNumCheckedPoints += 1.0F;
    m_bInWater = true;

    return cBuoyancy::calcStruct.fAddedDistToWaterSurface;
}

// 0x6C2810
void cBuoyancy::FindWaterLevel(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState)
{
    // Como el original: el estado se escribe antes de consultar el agua
    *outInWaterState = tWaterLevel::COLLIDING_WITH_WATER;
    const CVector transformedPos = BuoyancyMultiply3x3(m_EntityMatrix, *outVecOffset);
    const auto vecWorldPos = transformedPos + m_vecPos;
    CWaterLevel::GetWaterLevel(vecWorldPos.x, vecWorldPos.y, m_vecPos.z, outVecOffset->z, true, nullptr);
    outVecOffset->z -= (transformedPos.z + vecInitialZPos.z);

    if (outVecOffset->z > m_vecBoundingMax.z) {
        outVecOffset->z = m_vecBoundingMax.z;
        *outInWaterState = tWaterLevel::COMPLETELY_UNDER_WATER;
    }
    else if (outVecOffset->z < m_vecBoundingMin.z) {
        outVecOffset->z = m_vecBoundingMin.z;
        *outInWaterState = tWaterLevel::COMPLETELY_ABOVE_WATER;
    }
}

// 0x6C28C0
void cBuoyancy::FindWaterLevelNorm(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState, CVector* outVecNormal)
{
    // Como el original: el estado se escribe antes de consultar el agua
    *outInWaterState = tWaterLevel::COLLIDING_WITH_WATER;
    const CVector transformedPos = BuoyancyMultiply3x3(m_EntityMatrix, *outVecOffset);
    const auto vecWorldPos = transformedPos + m_vecPos;
    CWaterLevel::GetWaterLevel(vecWorldPos.x, vecWorldPos.y, m_vecPos.z, outVecOffset->z, true, outVecNormal);
    outVecOffset->z -= (transformedPos.z + vecInitialZPos.z);

    if (outVecOffset->z > m_vecBoundingMax.z) {
        outVecOffset->z = m_vecBoundingMax.z;
        *outInWaterState = tWaterLevel::COMPLETELY_UNDER_WATER;
    }
    else if (outVecOffset->z < m_vecBoundingMin.z) {
        outVecOffset->z = m_vecBoundingMin.z;
        *outInWaterState = tWaterLevel::COMPLETELY_ABOVE_WATER;
    }
}
