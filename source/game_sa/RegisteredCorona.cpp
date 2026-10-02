/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRegisteredCorona
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "RegisteredCorona.h"

void CRegisteredCorona::InjectHooks() {
    RH_ScopedClass(CRegisteredCorona);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Update, 0x6FABF0);
}

// 0x6FABF0
void CRegisteredCorona::Update() {
    if (!m_bRegisteredThisFrame) {
        m_Color.a = 0;
    }

    if (m_bAttached && !m_pAttachedTo) {
        m_dwId = 0;
        --CCoronas::NumCoronas;
        return;
    }

    const auto fadeStep = CTimer::GetTimeStep() * m_fFadeSpeed;
    const auto& camPos = TheCamera.GetPosition();
    if (!m_bCheckObstacles || ((!CCoronas::SunBlockedByClouds || m_dwId != 2)
        && CWorld::GetIsLineOfSightClear(m_vPosn, camPos, true, false, false, false, false, false, false))) {
        if (m_bOffScreen || (m_bOnlyFromBelow && camPos.z > m_vPosn.z)) {
            m_FadedIntensity = (uint8)(std::max(0.f, (float)(m_FadedIntensity)-fadeStep));
        } else {
            m_FadedIntensity = (uint8)(notsa::step_to((float)(m_FadedIntensity), (float)(m_Color.a), fadeStep));
            if (CCoronas::bChangeBrightnessImmediately) {
                m_FadedIntensity = m_Color.a;
            }

            if (m_dwId == 2) {
                CCoronas::LightsMult = std::max(0.6f, CCoronas::LightsMult - CTimer::GetTimeStep() * 0.06f);
            }
        }
    } else {
        m_FadedIntensity = (uint8)(std::max(0.f, (float)(m_FadedIntensity)-fadeStep));
    }

    if (!m_FadedIntensity && !m_bJustCreated) {
        m_dwId = 0;
        --CCoronas::NumCoronas;
    }

    m_bJustCreated = false;
    m_bRegisteredThisFrame = false;
}

//! Calculate the position to use for rendering
auto CRegisteredCorona::GetPosition() const -> CVector {
    if (!m_pAttachedTo) {
        return m_vPosn;
    }
    if (m_pAttachedTo->GetIsTypeVehicle() && m_pAttachedTo->AsVehicle()->IsSubBike()) {
        return m_pAttachedTo->AsBike()->m_mLeanMatrix.TransformPoint(m_vPosn);
    }
    return m_pAttachedTo->GetMatrix().TransformPoint(m_vPosn);
}

auto CRegisteredCorona::CalculateIntensity(float scrZ, float farClip) const -> float {
    return std::clamp(invLerp(farClip, farClip / 2.f, scrZ), 0.f, 1.f);
}
