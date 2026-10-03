/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: cTransmission
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "cTransmission.h"
#include "CarCtrl.h"

void cTransmission::InjectHooks()
{
    RH_ScopedClass(cTransmission);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(DisplayGearRatios, 0x6D0590);
    RH_ScopedInstall(InitGearRatios, 0x6D0460);
    RH_ScopedInstall(CalculateGearForSimpleCar, 0x6D0530);
    RH_ScopedInstall(CalculateDriveAcceleration, 0x6D05E0);
}

// Usage:
//     auto vehicle = FindPlayerVehicle();
//     if (vehicle) {
//         vehicle->m_pHandlingData->GetTransmission().DisplayGearRatios();
//     }
//
// 0x6D0590
void cTransmission::DisplayGearRatios()
{
    // Como el original: el factor es 1 / (0.277778 / 50) (global 0xC1CB54, iniciada en 0x853CE0) y se
    // imprimen siempre las 6 marchas con el printf del CRT del juego (0x821982).
    const float factor = 1.0f / StaticRef<float>(0xC1CB54);
    for (int32 i = 0; i < 6; i++) {
        const tTransmissionGear& gear = m_aGears.data()[i];
        reinterpret_cast<int(__cdecl*)(const char*, ...)>(0x821982)(
            "%d, max v = %3.2f, up at = %3.2f, down at = %3.2f\n",
            i,
            double(factor * gear.MaxVelocity),
            double(factor * gear.ChangeUpVelocity),
            double(factor * gear.ChangeDownVelocity)
        );
    }
}

// 0x6D0460
void cTransmission::InitGearRatios()
{
    m_aGears.fill({});
    // Como el original: se multiplica por el inverso 1 / numMarchas en vez de dividir.
    const float invNumGears = 1.0f / static_cast<float>(m_nNumberOfGears);
    const float averageHalfGearVelocity = 0.5f * m_MaxVelocity * invNumGears;
    const float maxGearVelocity = m_MaxVelocity - averageHalfGearVelocity;
    for (uint8 i = 1; i <= m_nNumberOfGears; i++)
    {
        static auto& gear = StaticRef<tTransmissionGear*>(0xC1CB34); // nullptr
        static auto& previousGear = StaticRef<tTransmissionGear*>(0xC1CB30); // nullptr
        gear = &m_aGears.data()[i];
        previousGear = &m_aGears.data()[i - 1];
        gear->MaxVelocity = static_cast<float>(i) * maxGearVelocity * invNumGears + averageHalfGearVelocity;
        const float velocityDifference = gear->MaxVelocity - previousGear->MaxVelocity;
        if (i >= m_nNumberOfGears)
        {
            gear->ChangeUpVelocity = m_MaxVelocity;
        }
        else
        {
            tTransmissionGear& nextGear = m_aGears.data()[i + 1];
            nextGear.ChangeDownVelocity = 0.42f * velocityDifference + previousGear->MaxVelocity;
            gear->ChangeUpVelocity = 0.6667f * velocityDifference + previousGear->MaxVelocity;
        }
    }
    m_aGears[0].MaxVelocity = m_MaxReverseVelocity;
    m_aGears[0].ChangeUpVelocity = -0.01f;
    m_aGears[0].ChangeDownVelocity = m_MaxReverseVelocity;
    m_aGears[1].ChangeDownVelocity = -0.01f;
}

// 0x6D0530
void cTransmission::CalculateGearForSimpleCar(float speed, uint8& currentGear)
{
    m_Velocity = speed;
    const tTransmissionGear& gear = m_aGears.data()[currentGear];
    if (speed > gear.ChangeUpVelocity)
    {
        // Como el original: min(marcha + 1, numMarchas), aunque la marcha actual ya supere el máximo.
        const int32 nextGear = currentGear + 1;
        currentGear = static_cast<uint8>(m_nNumberOfGears < nextGear ? m_nNumberOfGears : nextGear);
    }
    else if (speed < gear.ChangeDownVelocity)
    {
        if (currentGear > 0)
            currentGear--;
    }
}

// 0x6D05E0
// Como el original: las comparaciones siguen las del EXE (con NaN cada condición cae del mismo lado).
float cTransmission::CalculateDriveAcceleration(const float& gasPedal, uint8& currentGear, float& gearChangeCount, float& velocity, float* a6, float* a7, uint8 allWheelsOnGround, uint8 handlingCheat)
{
    static auto& cheatMultiplier = StaticRef<float>(0xC1CB3C); // 0.0f
    static auto& driveAcceleration = StaticRef<float>(0xC1CB38); // 0.0f
    static auto& currentVelocity = StaticRef<float>(0xC1CB40); // 0.0f
    currentVelocity = velocity;
    if (currentVelocity < m_MaxReverseVelocity)
        return 0.0f;

    for (;;)
    {
        if (currentVelocity > m_MaxVelocity)
            return 0.0f;

        m_Velocity = currentVelocity;
        const tTransmissionGear& gear = m_aGears.data()[currentGear];
        bool accelerate = false;
        if (currentVelocity > gear.ChangeUpVelocity)
        {
            if (currentGear == 0 && !(gasPedal > 0.0f))
                accelerate = true;
            else
                currentGear++;
        }
        else if (!(currentVelocity < gear.ChangeDownVelocity)
              || currentGear == 0
              || currentGear == 1 && !(gasPedal < 0.0f))
        {
            accelerate = true;
        }
        else
        {
            currentGear--;
        }

        if (accelerate)
        {
            float speedMultiplier  = 0.0f;
            float nitrosMultiplier = 0.0f;
            if (m_nNumberOfGears == 1)
            {
                speedMultiplier  = 1.0f;
                nitrosMultiplier = 1.0f;
            }
            else if (currentGear >= 1)
            {
                float gearNumber = 1.0f - (static_cast<float>(currentGear) - 1.0f) / (static_cast<float>(m_nNumberOfGears) - 1.0f);
                gearNumber *= gearNumber;
                if (m_handlingFlags & VEHICLE_HANDLING_1G_BOOST)
                    speedMultiplier = gearNumber * 5.0f;
                else if (m_handlingFlags & VEHICLE_HANDLING_2G_BOOST)
                    speedMultiplier = gearNumber * 4.0f;
                else
                    speedMultiplier = gearNumber * 3.0f;
                speedMultiplier += 1.0f;
                nitrosMultiplier = 1.0f;
            }
            else
            {   // reverse gear
                speedMultiplier = 4.5f;
                nitrosMultiplier = 1.0f;
            }
            cheatMultiplier = 1.0f;
            if (handlingCheat == CHEAT_HANDLING_PERFECT)
                cheatMultiplier = TRANSMISSION_AI_CHEAT_MULT;
            else if (handlingCheat == CHEAT_HANDLING_NITROS)
                nitrosMultiplier = TRANSMISSION_NITROS_MULT;
            driveAcceleration = speedMultiplier * (cheatMultiplier * m_EngineAcceleration) * nitrosMultiplier * 0.4f * gasPedal * CTimer::GetTimeStep();
            if (a6 && a7)
            {
                if (allWheelsOnGround)
                {
                    float currentDownVelocityDiff = 0.0f;
                    float upDownVelocityDiff      = 0.0f;
                    if (currentGear == 0 || currentGear == 1)
                    {
                        // Como el original: el tercio es (1 - 0.6667), calculado en tiempo de ejecución.
                        const float maxVelocityChange = m_MaxVelocity / static_cast<float>(m_nNumberOfGears) * (1.0f - 0.6667f);
                        if (currentGear == 0)
                        {
                            // reverse gear
                            currentDownVelocityDiff = maxVelocityChange - currentVelocity;
                            upDownVelocityDiff      = maxVelocityChange - m_aGears[0].ChangeDownVelocity;
                        }
                        else
                        {
                            currentDownVelocityDiff = currentVelocity + maxVelocityChange;
                            upDownVelocityDiff      = maxVelocityChange + m_aGears[1].ChangeUpVelocity;
                        }
                    }
                    else
                    {
                        currentDownVelocityDiff = currentVelocity - gear.ChangeDownVelocity;
                        upDownVelocityDiff      = gear.ChangeUpVelocity - gear.ChangeDownVelocity;
                    }
                    const float velocityDiffRatio = currentDownVelocityDiff / upDownVelocityDiff;
                    float inertiaMultiplier = velocityDiffRatio - *a6;
                    if (handlingCheat == CHEAT_HANDLING_PERFECT)
                    {
                        inertiaMultiplier *= TRANSMISSION_AI_CHEAT_INERTIA_MULT;
                    }
                    else if (handlingCheat == CHEAT_HANDLING_NITROS)
                    {
                        inertiaMultiplier *= TRANSMISSION_NITROS_INERTIA_MULT;
                    }
                    float acceleration = 1.0f - inertiaMultiplier * m_EngineInertia;
                    acceleration       = std::clamp(acceleration, 0.1f, 1.0f);
                    // Como el original: se lee *a7 antes de escribir y se guarda *a7 antes que *a6.
                    const float smoothed = (1.0f - TRANSMISSION_SMOOTHER_FRAC) * acceleration + TRANSMISSION_SMOOTHER_FRAC * *a7;
                    driveAcceleration *= smoothed;
                    *a7 = smoothed;
                    *a6 = velocityDiffRatio;
                }
                else
                {
                    const float freeAcceleration = fabs(gasPedal) / m_EngineInertia * CTimer::GetTimeStep() * TRANSMISSION_FREE_ACCELERATION + *a6;
                    *a6 = std::min(freeAcceleration, 1.0f);
                    *a7 = 0.1f;
                }
            }
            const tTransmissionGear& finalGear = m_aGears.data()[currentGear];
            float changeInVelocity = 0.0f;
            if (finalGear.MaxVelocity < 0.0f && currentVelocity < cheatMultiplier * finalGear.MaxVelocity)
            {
                changeInVelocity = cheatMultiplier * finalGear.MaxVelocity - currentVelocity;
            }
            else
            {
                if (!(finalGear.MaxVelocity > 0.0f))
                    return driveAcceleration;
                const float gearCheatMaxVelocity = cheatMultiplier * finalGear.MaxVelocity;
                if (!(currentVelocity > gearCheatMaxVelocity))
                    return driveAcceleration;
                changeInVelocity = currentVelocity - gearCheatMaxVelocity;
            }
            driveAcceleration *= (1.0f - std::min(changeInVelocity / 0.05f, 1.0f));
            return driveAcceleration;
        }
        a6 = nullptr;
        a7 = nullptr;
        allWheelsOnGround = false;
        handlingCheat = CHEAT_HANDLING_NONE;
        if (currentVelocity < m_MaxReverseVelocity)
            return 0.0f;
    }
}
