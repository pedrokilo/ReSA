/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CExplosion
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Explosion.h"
#include "CreepingFire.h"
#include "FireManager.h"
#include "InterestingEvents.h"
#include "Shadows.h"

void CExplosion::InjectHooks() {
    RH_ScopedClass(CExplosion);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ClearAllExplosions, 0x736840);
    RH_ScopedInstall(Shutdown, 0x7368F0);
    RH_ScopedInstall(GetExplosionActiveCounter, 0x736900);
    RH_ScopedInstall(ResetExplosionActiveCounter, 0x736910);
    RH_ScopedInstall(DoesExplosionMakeSound, 0x736920);
    RH_ScopedInstall(GetExplosionType, 0x736930);
    RH_ScopedInstall(GetExplosionPosition, 0x736940);
    RH_ScopedInstall(TestForExplosionInArea, 0x736950);
    RH_ScopedInstall(RemoveAllExplosionsInArea, 0x7369E0);
    RH_ScopedInstall(Initialise, 0x736A40);
    RH_ScopedInstall(AddExplosion, 0x736A50);
    RH_ScopedInstall(Update, 0x737620);
}

// 0x736A40
void CExplosion::Initialise() {
    ClearAllExplosions();
}

// 0x7368F0
void CExplosion::Shutdown() {
    // NOP
}

// Como el original: conversión de uint32 a float con un solo redondeo (fild + 2^32 en el EXE)
static float U32ToFloat(uint32 value) {
    return (float)(double)value;
}

// 0x736840
void CExplosion::ClearAllExplosions() {
    for (auto& exp : aExplosions) {
        exp.m_vecPosition = CVector{0.0f, 0.0f, 0.0f};
        exp.m_nType = EXPLOSION_GRENADE;
        exp.m_fRadius = 1.0f;
        exp.m_fVisibleDistance = 0.0f;
        exp.m_fPropagationRate = 0.0f;
        exp.m_fGroundZ = 0.0f;
        exp.m_pCreator = nullptr;
        exp.m_pVictim = nullptr;
        exp.m_nExpireTime = 0.0f;
        exp.m_nIteration = 0;
        exp.m_nCreatedTime = 0.0f;
        exp.m_bMakeSound = true;
        exp.m_nFuelTimer = 0;

        // Como el original: el bucle del combustible usa el mismo índice para la explosión y para la entrada,
        // así que solo limpia la entrada i de las explosiones 0, 1 y 2 (y no toca ni el contador activo ni el daño)
        for (auto i = 0; i < NUM_FUEL; i++) {
            aExplosions.data()[i].m_vecFuelDirection[i] = CVector{0.0f, 0.0f, 0.0f};
            aExplosions.data()[i].m_fFuelOffsetDistance[i] = 0.0f;
            aExplosions.data()[i].m_fFuelSpeed[i] = 0.0f;
        }
    }
}

// 0x736900
uint8 CExplosion::GetExplosionActiveCounter(uint8 id) {
    return aExplosions.data()[id].m_nActiveCounter;
}

// 0x736910
void CExplosion::ResetExplosionActiveCounter(uint8 id) {
    aExplosions.data()[id].m_nActiveCounter = 0;
}

// 0x736920
uint8 CExplosion::DoesExplosionMakeSound(uint8 id) {
    return aExplosions.data()[id].m_bMakeSound;
}

// 0x736930
int32 CExplosion::GetExplosionType(uint8 id) {
    return aExplosions.data()[id].m_nType;
}

// 0x736940
const CVector& CExplosion::GetExplosionPosition(uint8 id) {
    return aExplosions.data()[id].m_vecPosition;
}

// 0x736950
bool CExplosion::TestForExplosionInArea(eExplosionType type, float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    for (auto& exp : aExplosions) {
        if (!exp.m_nIteration)
            continue;

        if (exp.m_nType != type && type != eExplosionType::EXPLOSION_UNDEFINED)
            continue;

        // Como el original: min <= p <= max en x, y y z (NaN nunca está dentro)
        const auto& p = exp.m_vecPosition;
        if (!(p.x >= minX) || !(p.x <= maxX) || !(p.y >= minY) || !(p.y <= maxY) || !(p.z >= minZ) || !(p.z <= maxZ))
            continue;

        return true;
    }
    return false;
}

// 0x7369E0
void CExplosion::RemoveAllExplosionsInArea(CVector pos, float radius) {
    for (auto& exp : aExplosions) {
        if (!exp.m_nIteration)
            continue;

        // Como el original: (dx² + dz²) + dy² frente a r² (NaN no borra)
        const float dz = pos.z - exp.m_vecPosition.z;
        const float dy = pos.y - exp.m_vecPosition.y;
        const float dx = pos.x - exp.m_vecPosition.x;
        if (radius * radius > dx * dx + dz * dz + dy * dy) {
            exp.m_nIteration = 0;
        }
    }
}

// NOTSA
CExplosion* CExplosion::GetFree() {
    for (auto& exp : aExplosions) {
        if (!exp.m_nIteration)
            return &exp;
    }
    return nullptr;
}

// NOTSA
void CExplosion::SetCreator(CEntity* newCreator) noexcept {
    // Como el original: la referencia se registra sobre el miembro, no sobre el parámetro
    CEntity::SafeCleanUpRef(m_pCreator);
    m_pCreator = newCreator;
    CEntity::SafeRegisterRef(m_pCreator);
}

// NOTSA
void CExplosion::SetVictim(CEntity* newVictim) noexcept {
    CEntity::SafeCleanUpRef(m_pVictim);
    m_pVictim = newVictim;
    CEntity::SafeRegisterRef(m_pVictim);
}

bool DoesNeedToVehProcessBombTimer(eExplosionType type) {
    switch (type) {
    case eExplosionType::EXPLOSION_ROCKET:
    case eExplosionType::EXPLOSION_QUICK_CAR:
    case eExplosionType::EXPLOSION_MINE:
    case eExplosionType::EXPLOSION_OBJECT:
    case eExplosionType::EXPLOSION_TANK_FIRE:
        return true;
    }
    return false;
};

// Como el original: rand() * (1/32767) con la aritmética del EXE
static float ExplosionRand01() {
    return (float)CGeneral::GetRandomNumber() * std::bit_cast<float>(0x38000100u);
}

// 0x736A50
void CExplosion::AddExplosion(CEntity* victim, CEntity* creator, eExplosionType type, CVector pos, uint32 lifetime, uint8 usesSound, float cameraShake, uint8 bInvisible) {
    if (FindPlayerPed() == creator) {
        auto& info = FindPlayerInfo();
        info.m_nHavocCaused += 5;
        info.m_fCurrentChaseValue += 7.0f;
    }

    auto exp = GetFree();
    if (!exp)
        return;

    exp->m_nType = type;
    exp->m_vecPosition = pos;
    exp->m_fRadius = 1.0f;
    exp->m_fVisibleDistance = 0.0f;
    exp->m_fGroundZ = 0.0f;
    exp->m_fDamagePercentage = 1.0f;

    exp->SetCreator(creator);
    exp->SetVictim(victim);

    // Como el original: iteración y contador activo a 1; el sonido va en 0x2A
    exp->m_nIteration = 1;
    exp->m_nActiveCounter = 1;
    exp->m_bMakeSound = usesSound;
    exp->m_nFuelTimer = 0;

    for (auto i = 0; i < NUM_FUEL; i++) {
        float& fOffsetDistance = exp->m_fFuelOffsetDistance[i];
        float& fFuelSpeed = exp->m_fFuelSpeed[i];
        CVector& vecFuelDir = exp->m_vecFuelDirection[i];

        if (i && CGeneral::GetRandomNumber() >= RAND_MAX / 2) {
            fOffsetDistance = 0.0f;
        } else {
            vecFuelDir.x = ExplosionRand01() * 2.0f - 1.0f;
            vecFuelDir.y = ExplosionRand01() * 2.0f - 1.0f;
            vecFuelDir.z = ExplosionRand01() * 0.8f + 0.2f;
            fOffsetDistance = ExplosionRand01() * 1.5f + 0.5f;
            fFuelSpeed = ExplosionRand01() * 10.0f + 20.0f;
        }
    }

    exp->m_nParticlesExpireTime = lifetime ? CTimer::GetTimeInMS() + lifetime : 0;

    const auto PlaySoundIfEnabled = [&](float volume = .0f) {
        if (exp->m_bMakeSound) {
            m_ExplosionAudioEntity.AddAudioEvent(eAudioEvents::AE_EXPLOSION, exp->m_vecPosition, volume);
        }
    };

    const auto PlayFx = [&](FxSystem_c* fx, float volume) {
        if (fx) {
            PlaySoundIfEnabled(volume);
            fx->PlayAndKill();
        }
    };

    // Como el original: con víctima el efecto va relativo a su matriz (pos - posición de la víctima);
    // sin víctima, en `pos` (el argumento, que el cóctel molotov puede haber bajado al suelo)
    const auto CreateAndPlayFxWithSound = [&](const char* name, float volume = .0f) {
        if (victim) {
            if (victim->GetRwObject()) {
                if (RwMatrix* matrix = victim->GetModellingMatrix()) {
                    CVector expToVictimDir = pos - victim->GetPosition();
                    PlayFx(g_fxMan.CreateFxSystem(name, expToVictimDir, matrix, false), volume);
                }
            }
        } else {
            PlayFx(g_fxMan.CreateFxSystem(name, pos, nullptr, false), volume);
        }
    };

    // Como el original: solo los tipos 0..12 aplican la invisibilidad
    if (bInvisible && (uint32)type <= (uint32)EXPLOSION_RC_VEHICLE) {
        exp->m_fRadius = 0.0f;
        exp->m_fVisibleDistance = 0.0f;
    }

    bool bNoFire = false;
    switch (type) {
    case eExplosionType::EXPLOSION_GRENADE: {
        if (!bInvisible) {
            exp->m_fRadius = 9.0f;
            exp->m_fVisibleDistance = 300.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;
        // Como el original: también con víctima (relativo a su matriz)
        CreateAndPlayFxWithSound("explosion_small");
        break;
    }
    case eExplosionType::EXPLOSION_MOLOTOV: {
        if (!bInvisible) {
            exp->m_fRadius = 6.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 3000);
        exp->m_fPropagationRate = 0.5f; // Como el original

        bool bHit = false;
        const float fGroundPos = CWorld::FindGroundZFor3DCoord({pos.x, pos.y, pos.z + 3.0f}, &bHit, nullptr);
        if (bHit)
            pos.z = fGroundPos;

        float fWaterLevel{};
        if (CWaterLevel::GetWaterLevelNoWaves(pos, &fWaterLevel, nullptr, nullptr)) {
            /* must be done like this because of call order */
            if (pos.z < fWaterLevel) {
                bNoFire = true;
                break;
            }
        }

        CreateAndPlayFxWithSound("explosion_molotov", -36.f);

        break;
    }
    case eExplosionType::EXPLOSION_ROCKET:
    case eExplosionType::EXPLOSION_WEAK_ROCKET: {
        if (!bInvisible) {
            exp->m_fRadius = 10.0f;
            exp->m_fVisibleDistance = 300.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;
        if (type == eExplosionType::EXPLOSION_WEAK_ROCKET) {
            exp->m_fVisibleDistance = 200.0f;
            exp->m_fDamagePercentage = 0.2f;
        }

        CreateAndPlayFxWithSound("explosion_small");
        break;
    }
    case eExplosionType::EXPLOSION_CAR:
    case eExplosionType::EXPLOSION_QUICK_CAR: {
        if (!bInvisible) {
            exp->m_fRadius = 9.0f;
            exp->m_fVisibleDistance = 300.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 4250);
        exp->m_fPropagationRate = 0.5f;
        exp->m_nCreatedTime = (float)(int32)CTimer::GetTimeInMS(); // Como el original: fild con signo

        if (exp->m_pVictim) {
            CCrime::ReportCrime(eCrimeType::CRIME_EXPLOSION, exp->m_pVictim, nullptr); /* won't do anything as second ped is nullptr */
        }
        // Como el original: sin víctima no hay efecto; con víctima sale en el dummy de +0x54 de la estructura del
        // vehículo (relativo a su matriz)
        if (const auto v = exp->m_pVictim) {
            const auto* mi = reinterpret_cast<const uint8*>(CModelInfo::GetModelInfo((int16)v->m_nModelIndex));
            CVector fxPos = *reinterpret_cast<const CVector*>(*reinterpret_cast<const uint8* const*>(mi + 0x5C) + 0x54);
            if (victim->GetRwObject()) {
                if (RwMatrix* matrix = victim->GetModellingMatrix()) {
                    PlayFx(g_fxMan.CreateFxSystem("explosion_medium", fxPos, matrix, false), 0.0f);
                }
            }
        }
        break;
    }
    case eExplosionType::EXPLOSION_BOAT:
    case eExplosionType::EXPLOSION_AIRCRAFT: {
        if (!bInvisible) {
            exp->m_fRadius = 25.0f;
            exp->m_fVisibleDistance = 600.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 3000);
        exp->m_nCreatedTime = (float)(int32)CTimer::GetTimeInMS(); // Como el original: fild con signo
        exp->m_fPropagationRate = 0.5f; // Como el original

        CreateAndPlayFxWithSound("explosion_large");
        break;
    }
    case eExplosionType::EXPLOSION_MINE: {
        if (!bInvisible) {
            exp->m_fRadius = 10.0f;
            exp->m_fVisibleDistance = 150.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 750);
        // Como el original: busca el suelo 4 m por encima y descarta el resultado
        CWorld::FindGroundZFor3DCoord({pos.x, pos.y, pos.z + 4.0f}, nullptr, nullptr);
        exp->m_fPropagationRate = 0.5f;

        PlaySoundIfEnabled();
        /* No fx for this */
        break;
    }
    case eExplosionType::EXPLOSION_OBJECT: {
        if (!bInvisible) {
            exp->m_fRadius = 10.0f;
            exp->m_fVisibleDistance = 150.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;
        PlaySoundIfEnabled();
        break;
    }
    case eExplosionType::EXPLOSION_TANK_FIRE: {
        if (!bInvisible) {
            exp->m_fRadius = 10.0f;
            exp->m_fVisibleDistance = 150.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;

        CreateAndPlayFxWithSound("explosion_large");
        break;
    }
    case eExplosionType::EXPLOSION_SMALL: {
        if (!bInvisible) {
            exp->m_fRadius = 3.0f;
            exp->m_fVisibleDistance = 90.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;

        CreateAndPlayFxWithSound("explosion_small");
        break;
    }
    case eExplosionType::EXPLOSION_RC_VEHICLE: {
        if (!bInvisible) {
            exp->m_fRadius = 3.0f;
            exp->m_fVisibleDistance = 90.0f;
        }
        exp->m_nExpireTime = U32ToFloat(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;

        CreateAndPlayFxWithSound("explosion_tiny");
        break;
    }
    }

    if (victim)
        g_InterestingEvents.Add(CInterestingEvents::EType::EVENT_ATTRACTOR, victim);

    // Como el original: el molotov que cae al agua desactiva la explosión y termina aquí
    if (bNoFire) {
        exp->m_nIteration = 0;
        return;
    }

    switch (type) {
    case eExplosionType::EXPLOSION_MOLOTOV:
    case eExplosionType::EXPLOSION_ROCKET:
    case eExplosionType::EXPLOSION_WEAK_ROCKET:
    case eExplosionType::EXPLOSION_OBJECT: {
        // Como el original: & 3 (no % 4) sobre rand() - 2 o rand() + 1
        const int32 numFires = (type == eExplosionType::EXPLOSION_MOLOTOV) ? (((int32)CGeneral::GetRandomNumber() - 2) & 3) : (((int32)CGeneral::GetRandomNumber() + 1) & 3);

        if (numFires) {
            for (auto i = 0; i < numFires; i++) {
                CVector firePos = exp->m_vecPosition;
                // Como el original: el primer fuego sale en el centro; los demás en ((r * 8) + x) - 4
                if (i) {
                    firePos.x = ExplosionRand01() * 8.0f + firePos.x - 4.0f;
                    firePos.y = ExplosionRand01() * 8.0f + firePos.y - 4.0f;
                }
                bool bHitGround{};
                firePos.z = CWorld::FindGroundZFor3DCoord({firePos.x, firePos.y, firePos.z + 3.0f}, &bHitGround, nullptr); // 0x73735C
                if (bHitGround && std::fabs(firePos.z - exp->m_vecPosition.z) < 10.0f) {
                    // Como el original: duración ((r * 0.40000004) * 7000) + 5600 truncada; el tercer argumento es i != 0
                    const auto burnTime = (uint32)(int64)(ExplosionRand01() * std::bit_cast<float>(0x3ECCCCCEu) * 7000.0f + 5600.0f);
                    gFireManager.StartFire(firePos, 0.8f, (uint8)(i != 0), exp->m_pCreator, burnTime, 3, 1);
                }
            }
            // Como el original: solo cuenta el jugador 1 (m_nPedType == 0)
            if (creator && creator->GetIsTypePed() && creator->AsPed()->m_nPedType == PED_TYPE_PLAYER1) {
                CStats::IncrementStat(eStats::STAT_FIRES_STARTED, 1.0f);
            }
        }
        break;
    }
    }

    // Como el original: la sombra va 5 m por encima
    CVector shadowPos{ pos.x, pos.y, pos.z + 5.0f };
    CShadows::AddPermanentShadow(eShadowType::SHADOW_DEFAULT, gpShadowHeliTex, &shadowPos, 8.0f, 0.0f, 0.0f, -8.0f, 200, 0, 0, 0, 10.0f, 30000, 1.0f);

    if (exp->m_fVisibleDistance != 0.0f && !exp->m_nParticlesExpireTime) {
        CWorld::TriggerExplosion(pos, exp->m_fRadius, exp->m_fVisibleDistance, victim, creator, DoesNeedToVehProcessBombTimer(type), exp->m_fDamagePercentage);
    }

    if (type == eExplosionType::EXPLOSION_MOLOTOV) {
        TheCamera.CamShake(cameraShake == -1.0f ? 0.2f : cameraShake, pos);
    } else {
        if (cameraShake == -1.0f)
            cameraShake = 0.6f;
        TheCamera.CamShake(cameraShake, pos);

        CPad::GetPad(0)->StartShake_Distance(300, 128, pos);
        if (CGameLogic::IsCoopGameGoingOn())
            CPad::GetPad(1)->StartShake_Distance(300, 128, pos);
    }
}

// 0x737620
void CExplosion::Update() {
    for (auto& exp : aExplosions) {
        if (!exp.m_nIteration)
            continue;

        if (exp.m_nParticlesExpireTime) {
            if (CTimer::GetTimeInMS() > exp.m_nParticlesExpireTime) {
                exp.m_nParticlesExpireTime = 0;
                if (exp.m_fVisibleDistance != 0.0f) {
                    CWorld::TriggerExplosion(
                        exp.m_vecPosition,
                        exp.m_fRadius,
                        exp.m_fVisibleDistance,
                        exp.m_pVictim,
                        exp.m_pCreator,
                        DoesNeedToVehProcessBombTimer(exp.m_nType),
                        exp.m_fDamagePercentage
                    );
                }
            }
        } else {
            exp.m_fRadius += CTimer::GetTimeStep() * exp.m_fPropagationRate;
            const CVector pos = exp.m_vecPosition;
            // Como el original: (expiración - tiempo) con el tiempo exacto, redondeado a float y truncado con _ftol2
            const auto remaining = (int32)(int64)(float)((double)exp.m_nExpireTime - (double)CTimer::GetTimeInMS());
            switch (exp.m_nType) {
            case eExplosionType::EXPLOSION_GRENADE:
            case eExplosionType::EXPLOSION_ROCKET:
            case eExplosionType::EXPLOSION_WEAK_ROCKET:
            case eExplosionType::EXPLOSION_AIRCRAFT:
            case eExplosionType::EXPLOSION_MINE:
            case eExplosionType::EXPLOSION_OBJECT: {
                if (CTimer::GetFrameCounter() % 2) {
                    // Como el original: con sombras extra
                    CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, exp.m_vecPosition, {}, 20.0f, 1.0f, 1.0f, 0.5f, 0, true, nullptr);
                }
                if (exp.m_nType == eExplosionType::EXPLOSION_AIRCRAFT && CGeneral::GetRandomIntInRange(0, 100) < 5) {
                    if (exp.m_pVictim) {
                        CExplosion::AddExplosion(exp.m_pVictim, exp.m_pCreator, eExplosionType::EXPLOSION_ROCKET, exp.m_pVictim->GetPosition(), 0, true, -1.0f, false);
                    }
                }
                break;
            }
            case eExplosionType::EXPLOSION_MOLOTOV: {
                CWorld::SetPedsOnFire(pos.x, pos.y, pos.z, 6.0f, exp.m_pCreator);
                CWorld::SetWorldOnFire(pos, 6.0f, exp.m_pCreator);
                CWorld::SetCarsOnFire(pos, 0.1f, exp.m_pCreator);

                if (exp.m_nIteration < 10 && exp.m_nIteration == 1) {
                    CEntity* hitEntity;
                    CColPoint colPoint{};
                    // Como el original: la línea vertical empieza 5 m por encima
                    const CVector origin{ exp.m_vecPosition.x, exp.m_vecPosition.y, exp.m_vecPosition.z + 5.0f };
                    const bool bGroundHit = CWorld::ProcessVerticalLine(origin, -1000.0f, colPoint, hitEntity, true, false, false, false, true, false, nullptr);
                    exp.m_fGroundZ = bGroundHit ? colPoint.m_vecPoint.z : exp.m_vecPosition.z;
                }
                break;
            }
            case eExplosionType::EXPLOSION_CAR:
            case eExplosionType::EXPLOSION_QUICK_CAR:
            case eExplosionType::EXPLOSION_BOAT: {
                if (exp.m_pVictim && (CGeneral::GetRandomNumber() & 0x1F) == 0) {
                    // Como el original: la primera tirada va a y y la segunda a x; el fuego sale junto a la víctima,
                    // 2 m por encima
                    CVector rndOffset;
                    rndOffset.y = CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);
                    rndOffset.x = CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);
                    rndOffset.z = 0.0f;
                    rndOffset.Normalise();
                    rndOffset *= CGeneral::GetRandomNumberInRange(1.0f, 2.0f);
                    rndOffset += exp.m_pVictim->GetPosition();
                    CCreepingFire::TryToStartFireAtCoors({ rndOffset.x, rndOffset.y, rndOffset.z + 2.0f }, 0, true, false, 10.0f);
                }
                if (CTimer::GetFrameCounter() % 2) {
                    CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, pos, {}, 15.0f, 1.0f, 0.7f, 0.5f, 0, true, nullptr);
                }
                break;
            }
            }

            if (remaining > 0)
                exp.m_nIteration++;
            else
                exp.m_nIteration = 0;

            // Como el original: resta (paso * 0.02 * -1000) truncado
            exp.m_nFuelTimer -= (int32)(int64)(CTimer::GetTimeStep() * 0.02f * -1000.0f);

            switch (exp.m_nType) {
            case eExplosionType::EXPLOSION_CAR:
            case eExplosionType::EXPLOSION_QUICK_CAR:
            case eExplosionType::EXPLOSION_BOAT:
            case eExplosionType::EXPLOSION_AIRCRAFT: {
                // Como el original: comparación sin signo
                if ((uint32)exp.m_nFuelTimer > 200)
                    break;
                // Como el original: tiempo * 0.001 (no / 1000)
                const float fFuelTimerProgress = U32ToFloat((uint32)exp.m_nFuelTimer) * 0.001f;
                for (auto i = 0; i < NUM_FUEL; i++) {
                    const float& fOffsetDistance = exp.m_fFuelOffsetDistance[i];
                    if (fOffsetDistance > 0.0f) {
                        const float dist = fFuelTimerProgress * exp.m_fFuelSpeed[i] + fOffsetDistance;
                        CVector fxPos = exp.m_vecPosition + exp.m_vecFuelDirection[i] * dist;
                        if (auto fx = g_fxMan.CreateFxSystem("explosion_fuel_car", fxPos, nullptr, false)) {
                            fx->PlayAndKill();
                        }
                    }
                }
                break;
            }
            }
        }
    }
}
