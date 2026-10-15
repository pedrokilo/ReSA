/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CFire
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

#include "Fire.h"
#include "FireManager.h"
#include "CreepingFire.h"
#include "X87Math.h"

void CFire::InjectHooks() {
    RH_ScopedClass(CFire);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x539D90);
    RH_ScopedInstall(Initialise, 0x538B30);
    RH_ScopedInstall(CreateFxSysForStrength, 0x539360);
    RH_ScopedInstall(Extinguish, 0x5393F0);
    RH_ScopedInstall(ProcessFire, 0x53A570);
}

// 0x539D90
CFire::CFire() {
    Initialise();
}

CFire* CFire::Constructor() {
    this->CFire::CFire();
    return this;
}

// 0x538B30
void CFire::Initialise() {
    // Originally m_nFlags = (m_nFlags & 0xF4) | 0x14; - Clear 1st, 2nd, 4th and set 3rd, 5th bits (1-based numbering)
    m_IsActive = false;
    m_IsCreatedByScript = false;
    m_MakesNoise = true;
    m_IsBeingExtinguished = false;
    m_IsFirstGeneration = true;
    m_Position = CVector{};
    m_ScriptReferenceIndex = 1;
    m_TimeToBurn = 0;
    m_EntityOnFire = nullptr;
    m_EntityStartedFire = nullptr;
    m_Strength = 1.0f;
    m_FxSystem = nullptr;
    m_NumGenerationsAllowed = 100;
    m_RemovalDist = 60;
}

// Inlined in 0x5394C0 CFireManager::ExtinguishPointWithWater
void CFire::ExtinguishWithWater(float fWaterStrength) {
    // Como el original: fuerza - (agua * 0.02 * paso), con el orden de productos del EXE.
    const float fOriginalStrength = m_Strength;
    CVector particlePos = m_Position;
    m_Strength = m_Strength - fWaterStrength * 0.02f * CTimer::GetTimeStep();

    // Como el original: rand() & 0xFF; x e y centrados (-128) por 0.01, z sin centrar por 0.005.
    particlePos.x = (float)((int32)(CGeneral::GetRandomNumber() & 0xFF) - 128) * 0.01f + particlePos.x;
    particlePos.y = (float)((int32)(CGeneral::GetRandomNumber() & 0xFF) - 128) * 0.01f + particlePos.y;
    particlePos.z = (float)(int32)(CGeneral::GetRandomNumber() & 0xFF) * 0.005f + particlePos.z;

    FxPrtMult_c prtMult{ 1.0f, 1.0f, 1.0f, 0.6f, 0.75f, 0.0f, 0.4f };
    const CVector vel0{ 0.0f, 0.0f, 0.8f };
    const CVector vel1{ 0.0f, 0.0f, 1.4f };
    g_fx.m_SmokeII3expand->AddParticle(particlePos, vel0, 0.0f, prtMult, -1.0f, 1.2f, 0.6f, false);
    g_fx.m_SmokeII3expand->AddParticle(particlePos, vel1, 0.0f, prtMult, -1.0f, 1.2f, 0.6f, false);

    m_IsBeingExtinguished = true;
    if (m_Strength < 0.0f) { // Como el original: un NaN no apaga
        Extinguish();
    } else if ((int32)(int64)m_Strength != (int32)(int64)fOriginalStrength) { // _ftol2: 64 bits truncados, se comparan los 32 bajos
        CreateFxSysForStrength(m_Position, nullptr);
    }
}

// Como el original (0x539F00): (rand * 1/32767 * 0.29999995 + 1) * tiempo + ahora, todo en x87 (enteros sin signo con fild + 2^32)
// y un solo _ftol2 sobre el resultado (ver FireBurnTimeCpp en X87Math).
static uint32 FireStartBurnTime(uint32 burnTime) {
    const int32 rnd = (int32)CGeneral::GetRandomNumber();
    const uint32 nowMs = CTimer::GetTimeInMS();
    const float total = FireBurnTimeCpp(rnd, std::bit_cast<float>(0x38000100u) /* 0x858C7C */, std::bit_cast<float>(0x3E999998u) /* 0x863A40 */,
                                        1.0f /* 0x858624 */, burnTime, nowMs);
    return (uint32)(int64)total;
}

// see 0x539F00 CFireManager::StartFire
void CFire::Start(CEntity* creator, CVector pos, uint32 nTimeToBurn, uint8 nGens) {
    // Como el original: posicion, flags (sin script; activo, ruido, primera generacion) y luego el rand.
    m_Position = pos;
    m_IsCreatedByScript = false;
    m_IsActive = true;
    m_MakesNoise = true;
    m_IsFirstGeneration = true;

    m_TimeToBurn = FireStartBurnTime(nTimeToBurn);

    CEntity::SafeCleanUpRef(m_EntityOnFire);
    m_EntityOnFire = nullptr;
    m_IsBeingExtinguished = false;

    CEntity::SafeCleanUpRef(m_EntityStartedFire);
    m_EntityStartedFire = creator;
    m_NumGenerationsAllowed = nGens;
    m_Strength = 1.0f;
    CEntity::SafeRegisterRef(m_EntityStartedFire);

    CreateFxSysForStrength(m_Position, nullptr);
}

// see 0x53A050 CFireManager::StartFire
void CFire::Start(CEntity* creator, CEntity* target, uint32 nTimeToBurn, uint8 nGens) {
    switch (target->GetType()) {
    case ENTITY_TYPE_PED: {
        auto targetPed = target->AsPed();
        targetPed->m_pFire = this;
        (void)FindPlayerPed(); // Como el original: se llama y se descarta
        if (creator) { // Como el original: solo se denuncia con autor
            CCrime::ReportCrime(
                targetPed->m_nPedType == PED_TYPE_COP ? eCrimeType::CRIME_SET_COP_PED_ON_FIRE : eCrimeType::CRIME_SET_PED_ON_FIRE,
                targetPed,
                creator->AsPed()
            );
        }
        break;
    }
    case ENTITY_TYPE_VEHICLE: {
        auto targetVehicle = target->AsVehicle();
        targetVehicle->m_pFire = this;
        if (creator) {
            CCrime::ReportCrime(eCrimeType::CRIME_SET_CAR_ON_FIRE, targetVehicle, creator->AsPed());
        }
        break;
    }
    case ENTITY_TYPE_OBJECT: {
        target->AsObject()->m_pFire = this;
        break;
    }
    }

    m_Position = target->GetPosition();
    m_IsCreatedByScript = false;
    m_IsBeingExtinguished = false;
    m_IsActive = true;
    m_IsFirstGeneration = true;
    m_Strength = 1.0f;

    // Como el original: aleatorio entero de 0x407180 ([0, 1000) con 1/32768)
    if (target->GetIsTypePed() && target->AsPed()->IsPlayer())
        m_TimeToBurn = CTimer::GetTimeInMS() + 2333;
    else if (target->GetIsTypeVehicle())
        m_TimeToBurn = (uint32)CGeneral::GetRandomIntInRange(0, 1000) + CTimer::GetTimeInMS() + 3000;
    else
        m_TimeToBurn = (uint32)CGeneral::GetRandomIntInRange(0, 1000) + CTimer::GetTimeInMS() + nTimeToBurn;

    CEntity::SafeCleanUpRef(m_EntityOnFire);
    m_EntityOnFire = target;
    m_EntityOnFire->RegisterReference(&m_EntityOnFire);

    CEntity::SafeCleanUpRef(m_EntityStartedFire);
    m_EntityStartedFire = creator;
    CEntity::SafeRegisterRef(m_EntityStartedFire);

    m_MakesNoise = true;
    m_NumGenerationsAllowed = nGens;

    CreateFxSysForStrength(m_Position, nullptr);
}

// see 0x53A270 CFireManager::StartScriptFire
void CFire::Start(CVector pos, float fStrength, CEntity* target, uint8 nGens) {
    m_IsActive = true;
    m_IsCreatedByScript = true;
    m_MakesNoise = true;
    m_IsBeingExtinguished = false;
    m_IsFirstGeneration = true;
    m_Position = pos;
    m_NumGenerationsAllowed = nGens;

    CEntity::SafeCleanUpRef(m_EntityOnFire);
    m_EntityOnFire = target;
    CEntity::SafeRegisterRef(m_EntityOnFire);

    CEntity::SafeCleanUpRef(m_EntityStartedFire);
    m_EntityStartedFire = nullptr;
    m_Strength = fStrength;

    if (!target) {
        CreateFxSysForStrength(pos, nullptr);
        return;
    }
    // Como el original: solo peatones y vehiculos reciben m_pFire y efecto; otros tipos se quedan sin efecto.
    switch (target->GetType()) {
    case ENTITY_TYPE_PED: {
        target->AsPed()->m_pFire = this;
        const CVector targetPos = target->GetPosition();
        CreateFxSysForStrength(targetPos, nullptr);
        break;
    }
    case ENTITY_TYPE_VEHICLE: {
        target->AsVehicle()->m_pFire = this;
        const CVector targetPos = target->GetPosition();
        CreateFxSysForStrength(targetPos, nullptr);
        break;
    }
    }
}

void CFire::SetEntityOnFire(CEntity* target) {
    CEntity::SafeCleanUpRef(m_EntityOnFire); /* Assume old target's m_pFire is not pointing to `*this` */

    m_EntityOnFire = target; /* assign, even if its null, to clear it */
    CEntity::SafeRegisterRef(m_EntityOnFire); /* Assume caller set target->m_pFire */
}

void CFire::SetEntityStartedFire(CEntity* creator) {
    CEntity::SafeCleanUpRef(m_EntityStartedFire);

    m_EntityStartedFire = creator; /* assign, even if its null, to clear it */
    CEntity::SafeRegisterRef(m_EntityStartedFire);
}

void CFire::DestroyFx() {
    if (m_FxSystem) {
        m_FxSystem->Kill();
        m_FxSystem = nullptr;
    }
}

auto CFire::GetFireParticleNameForStrength() const {
    if (m_Strength > 1.0f)
        return (m_Strength > 2.0f) ? "fire_large" : "fire_med";
    else
        return "fire";
};

// 0x539360
void CFire::CreateFxSysForStrength(const CVector& point, RwMatrix* matrix) {
    DestroyFx();
    // Como el original: comparaciones x87 (un NaN elige "fire_large").
    const char* name = !(m_Strength <= 1.0f) ? (!(m_Strength <= 2.0f) ? "fire_large" : "fire_med") : "fire";
    m_FxSystem = g_fxMan.CreateFxSystem(name, point, matrix, true);
    if (m_FxSystem)
        m_FxSystem->Play();
}

// 0x5393F0
void CFire::Extinguish() {
    if (!m_IsActive)
        return;

    m_TimeToBurn = 0;

    // Originally m_nFlags = (m_nFlags & 0xF6) | 0x10; - Clear 1st and 4th, set 5th
    m_IsActive = false;
    m_IsBeingExtinguished = false;
    m_IsFirstGeneration = true;

    DestroyFx();

    if (m_EntityOnFire) {
        switch (m_EntityOnFire->GetType()) {
        case ENTITY_TYPE_PED: {
            m_EntityOnFire->AsPed()->m_pFire = nullptr;
            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            m_EntityOnFire->AsVehicle()->m_pFire = nullptr;
            break;
        }
        }
        CEntity::ClearReference(m_EntityOnFire);
    }
}

// 0x59C890 con su orden de sumas (CMatrix * CVector)
static CVector FireMatrixTimesPoint(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// Como el original: distancia con la suma z + y + x (fld st0 = z; ...; fsqrt).
static float FireDistanceZYX(const CVector& a, const CVector& b) {
    const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
    return std::sqrt(dz * dz + dy * dy + dx * dx);
}

// 0x53A570
void CFire::ProcessFire() {
    {
        // Como el original: guarda la nueva fuerza y la revierte si cambia su parte entera.
        const float oldStrength = m_Strength;
        float newStrength = CTimer::GetTimeStep() * 0.002f + oldStrength;
        if (3.0f < newStrength) {
            newStrength = 3.0f;
        }
        m_Strength = newStrength;
        if ((int32)(int64)newStrength != (int32)(int64)oldStrength) {
            m_Strength = oldStrength;
        }
    }

    if (m_EntityOnFire) {
        m_Position = m_EntityOnFire->GetPosition();

        switch (m_EntityOnFire->GetType()) {
        case ENTITY_TYPE_PED: {
            auto targetPed = m_EntityOnFire->AsPed();

            if (targetPed->m_pFire != this) {
                Extinguish();
                return;
            }

            switch (targetPed->m_nPedState) {
            case PEDSTATE_DIE:
            case PEDSTATE_DEAD: {
                m_Position.z -= 1.0f; /* probably because ped is laying on the ground */
                break;
            }
            }

            if (targetPed->bInVehicle && targetPed->m_pVehicle) {
                auto vehicle = targetPed->m_pVehicle;
                if (vehicle->m_nModelIndex != MODEL_FIRETRUK && vehicle->IsAutomobile()) {
                    vehicle->m_fHealth = 75.0f;
                }
            } else if (!targetPed->IsPlayer() && !targetPed->IsAlive()) {
                targetPed->physicalFlags.bRenderScorched = true;
            }

            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            auto targetVehicle = m_EntityOnFire->AsVehicle();

            if (targetVehicle->m_pFire != this) {
                Extinguish();
                return;
            }

            if (!m_IsCreatedByScript) {
                targetVehicle->InflictDamage(m_EntityStartedFire, eWeaponType::WEAPON_FLAMETHROWER, CTimer::GetTimeStep() * 1.2f, CVector{ 0.0f, 0.0f, 0.0f });
            }

            auto onFire = m_EntityOnFire->AsVehicle();
            if (onFire->IsAutomobile()) {
                // Como el original: la matriz se aplica al dummy ya desplazado 0.15 en z (0x59C890).
                CVector dummy = onFire->GetVehicleModelInfo()->GetModelDummyPosition(eVehicleDummy::DUMMY_LIGHT_FRONT_MAIN);
                dummy.z = dummy.z + 0.15f;
                m_Position = FireMatrixTimesPoint(onFire->GetMatrix(), dummy);
            }
            break;
        }
        }

        if (m_FxSystem) {
            const float step = CTimer::GetTimeStep() + CTimer::GetTimeStep();
            const CVector& moveSpeed = m_EntityOnFire->AsPhysical()->m_vecMoveSpeed;
            m_FxSystem->SetOffsetPos(CVector{ step * moveSpeed.x + m_Position.x, step * moveSpeed.y + m_Position.y, step * moveSpeed.z + m_Position.z });
        }
    }

    if (!m_EntityOnFire || !m_EntityOnFire->GetIsTypeVehicle()) {
        // Check if we can set player's ped on fire
        if (!FindPlayerVehicle()
         && !FindPlayerPed()->m_pFire /* not already on fire */
         && !FindPlayerPed()->physicalFlags.bFireProof
         && !FindPlayerPed()->m_pAttachedTo
         ) {
            const CVector& playerPos = FindPlayerPed()->GetPosition();
            const CVector d{ playerPos.x - m_Position.x, playerPos.y - m_Position.y, playerPos.z - m_Position.z };
            if (d.x * d.x + d.z * d.z + d.y * d.y < 1.2f) { /* Note: Squared distance; como el original: x + z + y */
                FindPlayerPed()->DoStuffToGoOnFire();
                gFireManager.StartFire(FindPlayerPed(), m_EntityStartedFire, 0.8f, true, 7000, 100);
            }
        }
    }

    if ((CGeneral::GetRandomNumber() & 0x1F) == 0) {
        // Como el original: recorre el pool hacia atras y salta coches ya ardiendo y camiones de bomberos.
        auto* const pool = GetVehiclePool();
        for (auto idx = (int32)pool->GetSize() - 1; idx >= 0; --idx) {
            auto* const veh = pool->GetAt(idx);
            if (!veh || veh->m_pFire || veh->m_nModelIndex == MODEL_FIRETRUK) {
                continue;
            }
            if (!(FireDistanceZYX(veh->GetPosition(), m_Position) < 2.0f)) {
                continue;
            }
            if (veh->IsSubBMX()) {
                FindPlayerPed()->DoStuffToGoOnFire();
                gFireManager.StartFire(FindPlayerPed(), m_EntityStartedFire, 0.8f, true, 7000, 100);
                veh->BurstTyre((uint8)(CAR_PIECE_FIRST_WHEEL + +veh->FindTyreNearestPoint(CVector2D{ m_Position.x, m_Position.y })), false);
            } else {
                gFireManager.StartFire(veh, m_EntityStartedFire, 0.8f, true, 7000, 0);
            }
        }
    }

    if ((CGeneral::GetRandomNumber() & 3) == 0) {
        auto* const pool = GetObjectPool();
        for (auto idx = (int32)pool->GetSize() - 1; idx >= 0; --idx) {
            auto* const obj = pool->GetAt(idx);
            if (!obj || !(FireDistanceZYX(obj->GetPosition(), m_Position) < 3.0f)) {
                continue;
            }
            obj->ObjectFireDamage(CTimer::GetTimeStep() * 8.0f, m_EntityStartedFire);
        }
    }

    if ((int8)m_NumGenerationsAllowed > 0 && (CGeneral::GetRandomNumber() & 0x7F) == 0) {
        if (gFireManager.GetNumOfFires() < 25) {
            // Como el original: rand() * (1/32767) en linea; x usa el segundo aleatorio, se normaliza y z sube 2.
            const float recip = std::bit_cast<float>(0x38000100u);
            const float randY = (float)CGeneral::GetRandomNumber() * recip;
            const float randX = (float)CGeneral::GetRandomNumber() * recip;
            CVector dir{ randX + randX - 1.0f, randY + randY - 1.0f, 0.0f };
            dir.Normalise();
            const float dist = (float)CGeneral::GetRandomNumber() * recip + 2.0f;
            const CVector pos{ dir.x * dist + m_Position.x, dir.y * dist + m_Position.y, dir.z * dist + m_Position.z + 2.0f };
            CCreepingFire::TryToStartFireAtCoors(pos, (uint8)(m_NumGenerationsAllowed - 1), false, IsScript(), 10.0f);
        }
    }

    if (m_Strength <= 2.0f && m_NumGenerationsAllowed && (CGeneral::GetRandomNumber() & 0xF) == 0) {
        // Como el original: indice = (rand & 0xFFFF) / 32768 * 60, truncado.
        const auto rnd = (int32)(CGeneral::GetRandomNumber() & 0xFFFF);
        const auto idx = (int32)(int64)((float)rnd * std::bit_cast<float>(0x38000000u) * 60.0f);
        CFire& nearby = gFireManager.m_aFires.data()[idx]; // Como el original: sin comprobar si esta activo
        if (&nearby != this && nearby.m_IsActive && !nearby.m_IsCreatedByScript && nearby.m_Strength <= 1.0f) {
            if ((m_Position - nearby.m_Position).Magnitude() < 3.5f) {
                // Como el original: se desplaza este fuego (no el cercano) hacia el cercano.
                const CVector nearbyPart{ 0.3f * nearby.m_Position.x, 0.3f * nearby.m_Position.y, 0.3f * nearby.m_Position.z };
                const CVector thisPart{ 0.7f * m_Position.x, 0.7f * m_Position.y, 0.7f * m_Position.z };
                m_Strength += 1.0f;
                m_Position = CVector{ thisPart.x + nearbyPart.x, thisPart.y + nearbyPart.y, thisPart.z + nearbyPart.z };
                m_TimeToBurn = std::max(m_TimeToBurn, CTimer::GetTimeInMS() + 7000);
                CreateFxSysForStrength(m_Position, nullptr);
                // Como el original: se queda con el menor numero de generaciones (comparacion con signo).
                if (!((int8)m_NumGenerationsAllowed < (int8)nearby.m_NumGenerationsAllowed)) {
                    m_NumGenerationsAllowed = nearby.m_NumGenerationsAllowed;
                }
                nearby.Extinguish();
            }
        }
    }

    if (m_FxSystem) {
        // Como el original: parte fraccionaria s - (int)s y tiempo restante (m_TimeToBurn - ahora) / 3500.
        const float strength = m_Strength;
        const int32 whole = (int32)(int64)strength;
        const uint32 remaining = m_TimeToBurn - CTimer::GetTimeInMS();
        // fisub: strength - (int)strength con el entero exacto y un solo redondeo; fild + 2^32 si el tiempo restante tiene el bit alto, por 1/3500.
        const float fract = IntFractionCpp(strength, whole);
        const float timeLeft = UnsignedTimesFloatCpp(remaining, std::bit_cast<float>(0x3995CBECu) /* 0x863A48 */);
        m_FxSystem->SetConstTime(true, fract < timeLeft ? fract : timeLeft);
    }

    bool keepBurning;
    if (CTimer::GetTimeInMS() < m_TimeToBurn) {
        // Como el original: distancia 2D a la camara comparada con la distancia de borrado.
        const CVector& camPos = TheCamera.GetPosition();
        const float dx = camPos.x - m_Position.x, dy = camPos.y - m_Position.y;
        keepBurning = (float)m_RemovalDist > std::sqrt(dy * dy + dx * dx) || m_IsCreatedByScript;
    } else {
        keepBurning = m_IsCreatedByScript;
    }
    if (keepBurning) {
        const float fColorRG = (float)(int16)(CGeneral::GetRandomNumber() & 0x7F) * 0.001953125f;
        CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, m_Position, CVector{ 0.0f, 0.0f, 0.0f }, 8.0f, fColorRG, fColorRG, 0.0f, 0, false, nullptr);
    } else {
        if (m_Strength <= 1.0f) {
            Extinguish();
        } else {
            m_Strength -= 1.0f;
            m_TimeToBurn = CTimer::GetTimeInMS() + 7000;
            CreateFxSysForStrength(m_Position, nullptr);
        }
    }
}

bool CFire::HasTimeToBurn() const {
    return CTimer::GetTimeInMS() < m_TimeToBurn;
}

bool CFire::IsNotInRemovalDistance() const {
    return m_RemovalDist > (TheCamera.GetPosition() - m_Position).Magnitude();
}
