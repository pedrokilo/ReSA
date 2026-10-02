/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CHandShaker
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

#include "HandShaker.h"

void CHandShaker::InjectHooks() {
    RH_ScopedClass(CHandShaker);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SetDefaults, 0x517330);
    RH_ScopedInstall(Reset, 0x50D860);
    RH_ScopedInstall(Process, 0x50D930);
}

// 0x517330
void CHandShaker::SetDefaults() {
    m_lim.Set(0.02f, 0.02f, 0.01f);
    m_motion.Set(0.0002f, 0.0002f, 0.0001f);
    m_slow.Set(1.3f, 1.3f, 1.4f);

    m_scaleReactionMin = 0.3f;
    m_scaleReactionMax = 1.f;
    m_twitchFreq       = 20;
    m_twitchVel        = 0.001f;

    Reset();
}

// 0x50D860
void CHandShaker::Reset() {
    m_ang = CVector{0.f, 0.f, 0.f};
    // Como el original: tres rand() directos; el primero va a Z y el ultimo a X (min = 0, sin suma final)
    const float rz = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
    const float ry = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
    const float rx = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
    m_vel.x = rx * m_motion.x;
    m_vel.y = ry * m_motion.y;
    m_vel.z = rz * m_motion.z;
}

// 0x50D930
void CHandShaker::Process(float degree) {
    // Como el original: escala = |ang / lim| * (max - min) + min, para los tres ejes antes de nada
    const float range = m_scaleReactionMax - m_scaleReactionMin;
    float scale[3];
    for (auto i = 0; i < 3; i++) {
        scale[i] = std::abs(m_ang[i] / m_lim[i]) * range + m_scaleReactionMin;
    }
    // Como el original: solo si ambos son estrictamente positivos o estrictamente negativos (los ceros no cuentan)
    for (auto i = 0; i < 3; i++) {
        if ((m_ang[i] > 0.f && m_vel[i] > 0.f) || (m_ang[i] < 0.f && m_vel[i] < 0.f)) {
            scale[i] *= m_slow[i];
        }
    }

    //> 0x50DA61 - Como el original: el primer rand() es para Z y el ultimo para X
    const float rz = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
    const float ry = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
    const float rx = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
    float motion[3]{ rx * m_motion.x * scale[0], ry * m_motion.y * scale[1], rz * m_motion.z * scale[2] };
    for (auto i = 0; i < 3; i++) {
        if (m_ang[i] > 0.f) {
            motion[i] *= -1.f;
        }
    }
    //> 0x50DB20
    m_vel.x += motion[0];
    m_vel.y += motion[1];
    m_vel.z += motion[2];

    // Como el original: timestep * 0.02 * 1000 truncado (64 bits) e interpretado sin signo, por 0.03
    // (fild es exacto y el x87 a 24 bits solo redondea en cada operacion: se emula con double -> float)
    const auto stepMs = (int32)(int64)(CTimer::ms_fTimeStepNonClipped * 0.02f * 1000.f);
    const float k003 = std::bit_cast<float>(0x3CF5C290u); // 0x85A6EC: el float siguiente a 0.03, no 0.03f
    const float twitchT = stepMs >= 0
        ? (float)((double)stepMs * (double)k003)
        : (float)((double)(float)((double)stepMs + 4294967296.0) * (double)k003);
    const auto range2 = (int32)(int64)(float)((double)m_twitchFreq * (double)twitchT);
    const float rnd01 = (float)CGeneral::GetRandomNumber() * (1.f / 32768.f);
    const auto rnd = (int32)(int64)(float)((double)rnd01 * (double)(range2 - 1));
    if (rnd + 1 == 2) { //> 0x50DB9D
        const float lo = -m_twitchVel, hi = m_twitchVel;
        const float r1 = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
        const float r2 = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
        const float r3 = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL;
        m_vel.x += r3 * (hi - lo) + lo;
        m_vel.y += (hi - lo) * r2 + lo;
        m_vel.z += (hi - lo) * r1 + lo;
    }
    m_ang += twitchT * m_vel; //> 0x50DC74

    //> 0x50DCAF - Como el original: primero el limite superior, luego el inferior
    for (auto i = 0; i < 3; i++) {
        const float a = m_ang[i];
        if (a > m_lim[i]) {
            m_ang[i] = m_lim[i];
        } else if (a < -m_lim[i]) {
            m_ang[i] = -m_lim[i];
        }
    }

    m_resultMat.SetRotate(m_ang.x * degree, m_ang.y * degree, m_ang.z * degree); //> 0x50DD40
}
