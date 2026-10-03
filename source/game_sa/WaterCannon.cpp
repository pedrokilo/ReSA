/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWaterCannon
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "WaterCannon.h"
#include "FireManager.h"
#include "Events/EventHitByWaterCannon.h"

void CWaterCannon::InjectHooks() {
    RH_ScopedClass(CWaterCannon);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x728B40);
    RH_ScopedInstall(Update_OncePerFrame, 0x72A280);
    RH_ScopedInstall(Update_NewInput, 0x728C20);
    RH_ScopedInstall(PushPeds, 0x7295E0);
    RH_ScopedInstall(Render, 0x728DA0);
}

// 0x728B40
void CWaterCannon::Init() {
    m_nId = 0;
    m_nSectionsCount = 0;
    m_nCreationTime = CTimer::GetTimeInMS();
    std::ranges::fill(m_abUsed, false);
    m_auRenderIndices = { 0, 1, 2, 1, 3, 2, 4, 5, 6, 5, 7, 6, 8, 9, 10, 9, 11, 10 };
    m_Audio.Initialise(this);
}

// 0x72A280
void CWaterCannon::Update_OncePerFrame(int16 index) {
    const auto LIFETIME = 150;

    if (CTimer::GetTimeInMS() > m_nCreationTime + LIFETIME) {
        m_nSectionsCount = (int16)((m_nSectionsCount + 1) % SECTIONS_COUNT);
        m_abUsed[m_nSectionsCount] = false;
    }

    for (auto i = 0; i < SECTIONS_COUNT; i++) {
        if (m_abUsed[i]) {
            // Como el original: paso * 0.004 (constante del EXE) y suma paso * velocidad + punto.
            CVector& speed = m_sectionMoveSpeed[i];
            speed.z = speed.z - CTimer::GetTimeStep() * std::bit_cast<float>(0x3B83126Fu);

            CVector& point = m_sectionPoint[i];
            point.x = CTimer::GetTimeStep() * speed.x + point.x;
            point.y = CTimer::GetTimeStep() * speed.y + point.y;
            point.z = CTimer::GetTimeStep() * speed.z + point.z;
        }
    }
    for (auto i = 0; i < SECTIONS_COUNT; i++) {
        if (m_abUsed[i]) {
            gFireManager.ExtinguishPointWithWater(m_sectionPoint[i], 2.0f, 0.5f);
        }
    }

    if ((uint8)(CTimer::GetFrameCounter() + index) % 4 == 0) { // Notice cast to byte
        PushPeds();
    }

    // free if unused
    for (auto& used : m_abUsed) {
        if (used) {
            return;
        }
    }
    m_nId = 0;
}

// 0x728C20
void CWaterCannon::Update_NewInput(const CVector* start, const CVector* end) {
    m_sectionPoint[m_nSectionsCount]     = *start;
    m_sectionMoveSpeed[m_nSectionsCount] = *end;
    m_abUsed[m_nSectionsCount]           = true;
}

// 0x7295E0
void CWaterCannon::PushPeds() {
    // Como el original: caja inicial de +-10000 y comparaciones x87 (el empate o NaN toma el punto).
    CVector min{ 10000.0f, 10000.0f, 10000.0f }, max{ -10000.0f, -10000.0f, -10000.0f };
    for (auto i = 0; i < SECTIONS_COUNT; i++) {
        if (!IsSectionActive(i))
            continue;

        const auto& secPos = GetSectionPosn(i);
        if (!(min.x < secPos.x)) min.x = secPos.x;
        if (!(max.x > secPos.x)) max.x = secPos.x;
        if (!(min.y < secPos.y)) min.y = secPos.y;
        if (!(max.y > secPos.y)) max.y = secPos.y;
        if (!(min.z < secPos.z)) min.z = secPos.z;
        if (!(max.z > secPos.z)) max.z = secPos.z;
    }

    // Como el original: pool recorrido hacia atras; solo peatones con colision y estrictamente dentro de la caja.
    auto* const pool = GetPedPool();
    for (auto pedIdx = (int32)pool->GetSize() - 1; pedIdx >= 0; --pedIdx) {
        auto* const pedPtr = pool->GetAt(pedIdx);
        if (!pedPtr || !pedPtr->m_bUsesCollision) {
            continue;
        }
        auto& ped = *pedPtr;
        const CVector& pos = ped.GetPosition();
        if (!(pos.x > min.x) || !(pos.x < max.x) || !(pos.y > min.y) || !(pos.y < max.y) || !(pos.z > min.z) || !(pos.z < max.z)) {
            continue;
        }

        for (auto i = 0; i < SECTIONS_COUNT; i++) {
            if (!IsSectionActive(i)) {
                continue;
            }
            const CVector& secPosn = GetSectionPosn(i);
            const CVector& pedPosn = ped.GetPosition();
            const float dx = secPosn.x - pedPosn.x, dy = secPosn.y - pedPosn.y, dz = secPosn.z - pedPosn.z;
            if (!(dz * dz + dy * dy + dx * dx < 5.0f)) { // Como el original: z + y + x
                continue;
            }

            const CVector& secMoveSpeed = GetSectionMoveSpeed(i);
            {
                CEventHitByWaterCannon event(secPosn, secMoveSpeed);
                ped.GetEventGroup().Add(&event, false);

                ped.bWasStanding = false;
                ped.bIsStanding = false;

                ped.ApplyMoveForce({ 0.0f, 0.0f, CTimer::GetTimeStep() });

                // Como el original: (0.1 * velocidad - mov) * 0.1 sumado a la velocidad propia del peaton.
                const float applyX = (0.1f * secMoveSpeed.x - ped.m_vecMoveSpeed.x) * 0.1f;
                if (applyX * secMoveSpeed.x > 0.0f) {
                    ped.m_vecMoveSpeed.x = applyX + ped.m_vecMoveSpeed.x;
                }
                const float applyY = (0.1f * secMoveSpeed.y - ped.m_vecMoveSpeed.y) * 0.1f;
                if (applyY * secMoveSpeed.y > 0.0f) {
                    ped.m_vecMoveSpeed.y = applyY + ped.m_vecMoveSpeed.y;
                }

                const CVector particlePos = ped.GetPosition();
                const CVector particleVelocity1{ ped.m_vecMoveSpeed.x * 0.3f, ped.m_vecMoveSpeed.y * 0.3f, ped.m_vecMoveSpeed.z * 0.3f };
                const CVector particleVelocity2{ ped.m_vecMoveSpeed.x * -0.3f, ped.m_vecMoveSpeed.y * -0.3f, ped.m_vecMoveSpeed.z * -0.3f + 0.5f };
                FxPrtMult_c prtInfo{ 1.0f, 1.0f, 1.0f, 0.6f, 0.75f, 0.0f, 0.2f };
                g_fx.m_SmokeII3expand->AddParticle(particlePos, particleVelocity1, 0.0f, prtInfo);
                g_fx.m_SmokeII3expand->AddParticle(particlePos, particleVelocity2, 0.0f, prtInfo);
            }
            break;
        }
    }
}

// 0x728DA0
void CWaterCannon::Render() {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,        RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));

    (void)CGeneral::GetRandomNumber(); // Como el original: rand() cuyo resultado no se usa

    // Como el original: indices int16 con resto con signo; "a" es la seccion mas nueva y "b" la anterior.
    int16 idxA = (int16)(m_nSectionsCount % SECTIONS_COUNT);
    int16 idxB = (int16)(idxA - 1);
    if (idxB < 0) {
        idxB += SECTIONS_COUNT;
    }

    bool hasCalculatedMatrix = false, hasSectionHit = false;
    CVector right, fwd, up;
    const float recip = std::bit_cast<float>(0x38000100u);

    m_Audio.ClearSplashInfo();

    static auto& vertices = StaticRef<std::array<RxObjSpace3DVertex, 12>>(0xC80550);
    for (int16 i = 0; i < SECTIONS_COUNT - 1 && !hasSectionHit; i++) { // Como el original: 31 iteraciones
        if (IsSectionActive(idxA) && IsSectionActive(idxB)) {
            const CVector& posA = GetSectionPosn(idxA);
            const CVector& posB = GetSectionPosn(idxB);

            const CVector dir{ posB.x - posA.x, posB.y - posA.y, posB.z - posA.z };
            if (dir.x * dir.x + dir.y * dir.y + dir.z * dir.z < 25.0f) {
                if (!hasCalculatedMatrix) {
                    hasCalculatedMatrix = true;

                    // Como el original: cruz con el forward de m_mCameraMatrix (0xB6F9AC) y escala 0.05 / |cruz|.
                    up = CrossProduct(dir, StaticRef<CVector>(0xB6F9AC));
                    const float scale = 0.05f / std::sqrt(up.x * up.x + up.z * up.z + up.y * up.y);
                    up = CVector{ up.x * scale, up.y * scale, up.z * scale };

                    right.x = (float)CGeneral::GetRandomNumber() * recip;
                    right.y = (float)CGeneral::GetRandomNumber() * recip;
                    right.z = (float)CGeneral::GetRandomNumber() * recip;
                    right.Normalise();
                    right = CVector{ right.x * 0.05f, right.y * 0.05f, right.z * 0.05f };

                    fwd.x = (float)CGeneral::GetRandomNumber() * recip;
                    fwd.y = (float)CGeneral::GetRandomNumber() * recip;
                    fwd.z = (float)CGeneral::GetRandomNumber() * recip;
                    fwd.Normalise();
                    fwd = CVector{ fwd.x * 0.05f, fwd.y * 0.05f, fwd.z * 0.05f };
                }

                const float size = (float)(i * i) * 0.03125f + 3.0f;
                const auto  alpha = (RwUInt8)(int32)(64.0f - (float)i * 0.03125f * 64.0f);
                RwRGBA color{ 200, 200, 255, alpha };
                for (auto& vertice : vertices) {
                    RxObjSpace3DVertexSetPreLitColor(&vertice, &color);
                }

                const auto SetPair = [&](size_t v, const CVector& axis) {
                    const CVector s{ axis.x * size, axis.y * size, axis.z * size };
                    const CVector p[4] = {
                        CVector{ posA.x - s.x, posA.y - s.y, posA.z - s.z },
                        CVector{ s.x + posA.x, s.y + posA.y, s.z + posA.z },
                        CVector{ posB.x - s.x, posB.y - s.y, posB.z - s.z },
                        CVector{ s.x + posB.x, s.y + posB.y, s.z + posB.z },
                    };
                    for (size_t k = 0; k < 4; k++) {
                        RxObjSpace3DVertexSetPos(&vertices[v + k], &p[k]);
                    }
                };
                SetPair(0, up);
                SetPair(4, right);
                SetPair(8, fwd);

                CColPoint colPoint;
                CEntity* hitEntity{};
                if (CWorld::ProcessLineOfSight(posB, posA, colPoint, hitEntity, true, true, false, false, false, false, false, false)) {
                    FxPrtMult_c prtInfo{ 1.0f, 1.0f, 1.0f, 0.15f, 0.75f, 1.0f, 0.2f };
                    // Como el original: normal * 3 por componente aleatoria 0.2 + rand * (1/32767) * 1.6 (0x3FCCCCCC).
                    CVector velocity0{ colPoint.m_vecNormal.x * 3.0f, colPoint.m_vecNormal.y * 3.0f, colPoint.m_vecNormal.z * 3.0f };
                    const float range = std::bit_cast<float>(0x3FCCCCCCu);
                    velocity0.x = ((float)CGeneral::GetRandomNumber() * recip * range + 0.2f) * velocity0.x;
                    velocity0.y = ((float)CGeneral::GetRandomNumber() * recip * range + 0.2f) * velocity0.y;
                    velocity0.z = ((float)CGeneral::GetRandomNumber() * recip * range + 0.2f) * velocity0.z;
                    const CVector velocity1{ velocity0.x * 0.6f, velocity0.y * 0.6f, velocity0.z * 0.6f };

                    for (auto n = 0; n < 2; n++) {
                        // Como el original: n / (uint32)(paso * 0.02 * 1000) con fild sin signo.
                        const uint32 stepMs = (uint32)(int64)(CTimer::GetTimeStep() * 0.02f * 1000.0f);
                        const float unk = (float)n / (float)stepMs;

                        g_fx.m_WaterSplash->AddParticle(colPoint.m_vecPoint, velocity0, unk, prtInfo);
                        g_fx.m_SmokeII3expand->AddParticle(colPoint.m_vecPoint, velocity1, unk, prtInfo);
                    }

                    m_Audio.SetSplashInfo(colPoint.m_vecPoint, std::sqrt(velocity0.z * velocity0.z + velocity0.y * velocity0.y + velocity0.x * velocity0.x));
                    hasSectionHit = true; // Como el original: se renderiza esta seccion y se sale en la siguiente vuelta
                }

                if (RwIm3DTransform(vertices.data(), vertices.size(), nullptr, rwIM3D_VERTEXRGBA)) {
                    RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, m_auRenderIndices.data(), m_auRenderIndices.size());
                    RwIm3DEnd();
                }
            }
        }
        idxA = idxB;
        idxB = (int16)(idxB - 1);
        if (idxB < 0) {
            idxB += SECTIONS_COUNT;
        }
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
}

bool CWaterCannon::IsSectionActive(size_t idx) const {
    return m_abUsed[idx];
}

CVector& CWaterCannon::GetSectionPosn(size_t idx) {
    return m_sectionPoint[idx];
}

CVector& CWaterCannon::GetSectionMoveSpeed(size_t idx) {
    return m_sectionMoveSpeed[idx];
}
