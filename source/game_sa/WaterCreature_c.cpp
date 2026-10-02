/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: WaterCreature_c
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

#include "WaterCreature_c.h"

void WaterCreature_c::InjectHooks()
{
    RH_ScopedClass(WaterCreature_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6E4160);
    RH_ScopedInstall(Exit, 0x6E3E60);
    RH_ScopedInstall(Update, 0x6E4670);
}

namespace {
// Como el original: constantes exactas del EXE (0x858CB8 y 0x858CBC); PI/TWO_PI de common.h no lo son
constexpr float PI_6E4 = std::bit_cast<float>(0x40490FDBu);     // 3.1415927f
constexpr float TWO_PI_6E4 = std::bit_cast<float>(0x40C90FDBu); // 6.2831855f

// Como el original: GetHeading en linea con fpatan (precision completa, redondeo al guardar)
float GetHeadingX87_6E4(const CPlaceable* placeable) {
    const CMatrix* m = placeable->m_matrix;
    if (!m) {
        return placeable->m_placement.m_fHeading;
    }
    const float* fwd = &m->GetForward().x;
    float result;
    __asm {
        mov eax, fwd
        fld dword ptr [eax]
        fchs
        fld dword ptr [eax+4]
        fpatan
        fstp result
    }
    return result;
}

// Como el original: vector frontal en linea; sin matriz usa fsin/fcos del x87 sobre el heading
CVector GetForwardX87_6E4(const CPlaceable* placeable) {
    if (const CMatrix* m = placeable->m_matrix) {
        return m->GetForward();
    }
    const float heading = placeable->m_placement.m_fHeading;
    float s, c;
    __asm {
        fld heading
        fsin
        fchs
        fstp s
        fld heading
        fcos
        fstp c
    }
    return { s, c, 0.f };
}

// Como el original: SetHeading en linea (SetRotateZOnly o heading de m_placement)
void SetHeading_6E4(CPlaceable* placeable, float heading) {
    if (placeable->m_matrix) {
        placeable->m_matrix->SetRotateZOnly(heading);
    } else {
        placeable->m_placement.m_fHeading = heading;
    }
}

// Como el original: rand() / 32768 * 100 truncado a entero
int32 RandomPercent_6E4() {
    return (int32)(int64)((float)CGeneral::GetRandomNumber() * (1.f / 32768.f) * 100.f);
}
} // namespace

// 0x6E4160
bool WaterCreature_c::Init(int32 nType, CVector* vecPos, WaterCreature_c* parent, float fWaterLevel, float fWaterDepth)
{
    if (CObject::nNoTempObjects >= 150)
        return false;

    const auto& info = WaterCreatureManager_c::GetCreatureInfo(nType);
    if (!parent)
    {
        auto fMaxZ = fWaterLevel - info.m_fMinSpawnDepth;
        auto fMinZ = fWaterLevel - fWaterDepth + 2.0F;
        const auto fLowest = fWaterLevel - 50.0F;
        fMinZ = fMinZ > fLowest ? fMinZ : fLowest; // Como el original

        if (fMaxZ < fMinZ)
            return false;

        vecPos->z = CGeneral::GetRandomNumberInRange(fMinZ, fMaxZ);
    }

    GetObjectPool()->SetDealWithNoMemory(true);
    m_pObject = new CObject(*info.m_pModelId, false);
    GetObjectPool()->SetDealWithNoMemory(false);

    if (!m_pObject)
        return false;

    m_pObject->SetAreaCode(static_cast<eAreaCodes>(CGame::currArea));
    m_pObject->SetIsStatic(true);
    m_pObject->m_bUnderwater = true;
    m_pObject->physicalFlags.bApplyGravity = false;
    m_pObject->SetUsesCollision(false);
    m_pObject->m_nObjectType = eObjectType::OBJECT_TYPE_DECORATION;
    if (nType == JELLYFISH1 || nType == JELLYFISH2) // Como el original: compara el tipo recibido
        m_pObject->m_bDrawLast = true;

    m_nCreatureType = nType;
    m_pFollowedCreature = parent;
    m_bShouldBeDeleted = false;

    if (parent)
    {
        CVector vecDirection;
        vecDirection.x = CGeneral::GetRandomNumberInRange(-1.0F, 1.0F);
        vecDirection.y = CGeneral::GetRandomNumberInRange(-1.0F, 1.0F);
        vecDirection.z = CGeneral::GetRandomNumberInRange(-0.5F, 0.5F);
        const auto fDist = CGeneral::GetRandomNumberInRange(info.m_fMinDistFromFollowed, info.m_fMaxDistFromFollowed);

        // Como el original: se coloca en pos + dir * dist y el desplazamiento guardado es pos - esa posicion
        const CVector offset{ vecDirection.x * fDist, fDist * vecDirection.y, vecDirection.z * fDist };
        const CVector newPos{ offset.x + vecPos->x, offset.y + vecPos->y, offset.z + vecPos->z };
        m_vecOffsetFromFollowed = CVector{ vecPos->x - newPos.x, vecPos->y - newPos.y, vecPos->z - newPos.z };
        m_pObject->SetPosn(newPos);
        m_fHeading = GetHeadingX87_6E4(parent->GetObject());
        m_fDefaultSpeed = CGeneral::GetRandomNumberInRange(info.m_fMinSpeed, parent->m_fDefaultSpeed);
    }
    else
    {
        m_vecOffsetFromFollowed.Set(0.0F, 0.0F, 0.0F);
        m_pObject->SetPosn(*vecPos);
        m_fHeading = CGeneral::GetRandomNumberInRange(-PI_6E4, PI_6E4);
        m_fDefaultSpeed = CGeneral::GetRandomNumberInRange(info.m_fMinSpeed, info.m_fMaxSpeed);
    }

    m_ucTargetSwimSpeed = 0;
    m_bChangedDir = true;
    SetHeading_6E4(m_pObject, m_fHeading);
    m_pObject->UpdateRwMatrix();
    m_pObject->UpdateRwFrame();

    CWorld::Add(m_pObject);
    ++CObject::nNoTempObjects;
    return true;
}

// 0x6E3E60
void WaterCreature_c::Exit()
{
    g_waterCreatureMan.m_waterCreatureList.RemoveItem(this);
    g_waterCreatureMan.m_waterCreaturePool.AddItem(this);
    CWorld::Remove(m_pObject);
    delete m_pObject;
    m_pObject = nullptr;
    --CObject::nNoTempObjects;
}

// 0x6E4670
void WaterCreature_c::Update(float fTimeStep)
{
    const auto& pInfo = WaterCreatureManager_c::GetCreatureInfo(m_nCreatureType);
    if (m_pFollowedCreature)
    {
        const auto& vecParentPos = m_pFollowedCreature->GetObject()->GetPosition();
        const CVector vecPosInGroup{ m_vecOffsetFromFollowed.x + vecParentPos.x, m_vecOffsetFromFollowed.y + vecParentPos.y, m_vecOffsetFromFollowed.z + vecParentPos.z };

        auto vecDir = vecPosInGroup - m_pObject->GetPosition();
        const auto fDistFromGroup = vecDir.NormaliseAndMag();
        if (fDistFromGroup > 0.0F)
        {
            auto fNewHeading = CGeneral::GetRadianAngleBetweenPoints(vecDir.x, vecDir.y, 0.0F, 0.0F);
            fNewHeading = CGeneral::LimitRadianAngle(fNewHeading);
            SetHeading_6E4(m_pObject, fNewHeading);
        }

        // Como el original: rand() / 32768 * 100 truncado; 5% de probabilidad de cambiar de velocidad
        if (RandomPercent_6E4() < 5)
            m_fDefaultSpeed = CGeneral::GetRandomNumberInRange(pInfo.m_fMinSpeed, m_pFollowedCreature->m_fDefaultSpeed);

        auto fSpeed = m_fDefaultSpeed;
        if ((int8)m_pFollowedCreature->m_ucTargetSwimSpeed > 0) // Como el original: comparacion con signo
            fSpeed = m_pFollowedCreature->m_fCurSpeed;

        if (fDistFromGroup > 2.0F)
            fSpeed = (fDistFromGroup - 2.0F) * fSpeed * 1.1F; // Catch up with group (Como el original: este orden)

        // Como el original: min(speed * step, max(0, dist - 0.01))
        const float fRemaining = fDistFromGroup - 0.01F;
        const float fMaxStep = fRemaining > 0.0F ? fRemaining : 0.0F;
        fSpeed *= fTimeStep;
        fSpeed = fSpeed < fMaxStep ? fSpeed : fMaxStep;

        const auto vecPos = m_pObject->GetPosition();
        const auto vecForward = GetForwardX87_6E4(m_pObject);
        m_pObject->SetPosn(CVector{ vecForward.x * fSpeed + vecPos.x, vecPos.y + vecForward.y * fSpeed, vecPos.z + vecForward.z * fSpeed });

        if (pInfo.m_fMaxDistFromFollowed * 5.0F < fDistFromGroup) // Como el original: estrictamente mayor
            m_pFollowedCreature = nullptr; // Break out from group
    }
    else
    {
        auto bMove = false;
        if (!m_bChangedDir && !m_ucTargetSwimSpeed)
        {
            if (CGeneral::GetRandomNumberInRange(0.0F, 1.0F) < pInfo.m_fChanceToRandomizeRotation)
            {
                m_fHeading = CGeneral::GetRandomNumberInRange(-PI_6E4, PI_6E4);
                bMove = true;
            }
        }

        const auto vecPos = m_pObject->GetPosition();
        // Como el original: el numero aleatorio se pide siempre, aunque m_Speed no sea positivo
        const float fRand01 = (float)CGeneral::GetRandomNumber() * (1.f / 32768.f);
        if (pInfo.m_Speed > 0.0F)
        {
            const auto nRand = (int32)(int64)(fRand01 * 100.f);
            if (nRand < 10 || bMove || m_bChangedDir)
            {
                const auto vecFwd = m_pObject->GetForwardVector(); // Como el original: llamada a 0x41CCB0
                const auto& vecCurPos = m_pObject->GetPosition();
                const CVector vecTargetPos{ pInfo.m_Speed * vecFwd.x + vecCurPos.x, pInfo.m_Speed * vecFwd.y + vecCurPos.y, pInfo.m_Speed * vecFwd.z + vecCurPos.z };
                CColPoint colPoint;
                CEntity* entity;
                if (CWorld::ProcessLineOfSight(vecPos, vecTargetPos, colPoint, entity, true, false, false, false, false, false, false, false)) {
                    const auto fHeading = CGeneral::GetRadianAngleBetweenPoints(colPoint.m_vecNormal.x, colPoint.m_vecNormal.y, 0.0F, 0.0F);
                    m_fHeading = CGeneral::LimitRadianAngle(fHeading);
                    m_bChangedDir = true;
                }
                else
                {
                    m_bChangedDir = false;
                }
            }
        }

        auto fHeading = GetHeadingX87_6E4(m_pObject);
        // Como el original: primero si el objetivo quedo por encima, luego si quedo por debajo
        if (fHeading + PI_6E4 < m_fHeading)
        {
            m_fHeading -= TWO_PI_6E4;
        }
        else if (fHeading - PI_6E4 > m_fHeading)
        {
            m_fHeading += TWO_PI_6E4;
        }

        const auto fHeadingDiff = m_fHeading - fHeading;
        if (fabs(fHeadingDiff) > pInfo.m_fMaxHeadingChange)
        {
            if (fHeadingDiff < 0.0F)
                fHeading -= pInfo.m_fMaxHeadingChange;
            else
                fHeading = pInfo.m_fMaxHeadingChange + fHeading;
        }

        SetHeading_6E4(m_pObject, fHeading);
        if (m_bChangedDir)
        {
            // Como el original: GetRandomIntInRange (0x407180)
            if (!m_ucTargetSwimSpeed && CGeneral::GetRandomIntInRange(0, 1000) < 8)
            {
                m_ucTargetSwimSpeed = (uint8)CGeneral::GetRandomIntInRange(5, 15);
                m_wSpeedChangeCurTime = 0;
                m_wSpeedChangeTotalTime = (int16)CGeneral::GetRandomIntInRange(1000, 2500);
            }
        }
        else
        {
            auto* player = FindPlayerPed(0);
            auto& vecPlayerPos = player->GetPosition();
            auto vecSwimDir = vecPos - vecPlayerPos;
            if (vecSwimDir.NormaliseAndMag() < 5.0F) // Swim away from player
            {
                m_ucTargetSwimSpeed = (uint8)CGeneral::GetRandomIntInRange(35, 50);
                m_wSpeedChangeCurTime = 0;
                m_wSpeedChangeTotalTime = (int16)CGeneral::GetRandomIntInRange(2000, 4500);

                const auto fNewHeading = CGeneral::GetRadianAngleBetweenPoints(vecSwimDir.x, vecSwimDir.y, 0.0F, 0.0F);
                m_fHeading = CGeneral::LimitRadianAngle(fNewHeading);
                if (IsSmallFish())
                    SetHeading_6E4(m_pObject, m_fHeading);
            }
        }

        auto fCurSpeed = m_fDefaultSpeed;
        const auto nTargetSpeed = (int8)m_ucTargetSwimSpeed; // Como el original: movsx
        if (nTargetSpeed > 0)
        {
            m_wSpeedChangeCurTime += (int16)(int64)(fTimeStep * 1000.0F);
            // Como el original: tiempos sin signo (movzx) y division x87 entre enteros
            const auto nCurTime = (uint16)m_wSpeedChangeCurTime, nTotalTime = (uint16)m_wSpeedChangeTotalTime;
            const float fProgress = 1.0F - (float)nCurTime / (float)nTotalTime;
            const float fInvProgress = fProgress > 0.0F ? fProgress : 0.0F;
            fCurSpeed += (float)nTargetSpeed * 0.1F * fInvProgress * fCurSpeed;
            if (nCurTime >= nTotalTime)
                m_ucTargetSwimSpeed = 0;

            m_fCurSpeed = fCurSpeed;
        }

        const auto vecForward = GetForwardX87_6E4(m_pObject);
        const CVector vecStep{ vecForward.x * fTimeStep * fCurSpeed, vecForward.y * fTimeStep * fCurSpeed, vecForward.z * fTimeStep * fCurSpeed };
        m_pObject->SetPosn(CVector{ vecStep.x + vecPos.x, vecStep.y + vecPos.y, vecStep.z + vecPos.z });
    }

    if (IsJellyfish())
    {
        const auto vecJellyPos = m_pObject->GetPosition();
        float fWaterLevel;
        if (CWaterLevel::GetWaterLevel(vecJellyPos.x, vecJellyPos.y, vecJellyPos.z, fWaterLevel, true, nullptr))
            m_pObject->SetPosn(CVector(vecJellyPos.x, vecJellyPos.y, fWaterLevel - 0.2F));
    }

    m_pObject->UpdateRwMatrix();
    m_pObject->UpdateRwFrame();
    CWorld::Remove(m_pObject); //BUG? Is this remove needed here?
    CWorld::Add(m_pObject);

    // Como el original: suma z*z + y*y + x*x y comparacion estricta con 3600
    const auto& vecCamPos = TheCamera.GetPosition();
    const auto& vecObjPos = m_pObject->GetPosition();
    const CVector d{ vecObjPos.x - vecCamPos.x, vecObjPos.y - vecCamPos.y, vecObjPos.z - vecCamPos.z };
    if (d.z * d.z + d.y * d.y + d.x * d.x > WaterCreatureManager_c::ms_fMaxWaterCreaturesDrawDistanceSquared)
        g_waterCreatureMan.TryToExitGroup(this);
}
