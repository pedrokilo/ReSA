/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CLoadMonitor
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

auto& g_LoadMonitor = StaticRef<CLoadMonitor>(0xB72978);

void CLoadMonitor::InjectHooks() {
    RH_ScopedClass(CLoadMonitor);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x53CFA0);
    RH_ScopedInstall(Destructor, 0x856430);

    RH_ScopedInstall(BeginFrame, 0x53D030);
    RH_ScopedInstall(EndFrame, 0x53D0B0);
    RH_ScopedInstall(StartTimer, 0x53D050);
    RH_ScopedInstall(EndTimer, 0x53D070);
    RH_ScopedInstall(Render, 0x53D480);
}

// 0x53CFA0
CLoadMonitor::CLoadMonitor() {
    m_bInFrame            = 0;
    m_bUseLoadMonitor     = 1;
    m_eProcLevel          = eProcessingLevel::OK;
    m_LastTime            = CTimer::GetTimeInMS();
    m_NumFramesThisSec    = 0;
    m_FPS                 = 0;
    m_bEnableAmbientCrime = true;

    rng::fill(m_iMaxCycles, 0);

    rng::fill(m_fAveragedCyclesThisSecond, 0.f);
    std::memset(&m_iCyclesHistory, 0, sizeof(m_iCyclesHistory)); // 0x53CFED

    m_fPeakLevels[+eLoadType::PED_AI]                 = 15.0f;
    m_fPeakLevels[+eLoadType::COLLISION]              = 20.0f;
    m_fPeakLevels[+eLoadType::NUM_STREAMING_REQUESTS] = 10.0f;
    m_fPeakLevels[+eLoadType::MOVE_SPEED]             = 50.0f;

    m_DisplayType = eLoadMonitorDisplay::NONE;
    m_VarConsoleDisplayType = eLoadMonitorDisplay::NONE;
}

// 0x53D030
void CLoadMonitor::BeginFrame() {
    rng::fill(m_iCyclesThisFrame, 0);
    m_bInFrame = 1;
}

// 0x53D0B0
void CLoadMonitor::EndFrame() {
    if (auto* ped = FindPlayerPed()) {
        const auto& movevec = ped->IsInVehicle()
            ? ped->GetVehicleIfInOne()->GetMoveSpeed()
            : ped->GetMoveSpeed();

        // 0x53D0FD: magnitude of the scaled vector, summed as (z*z + y*y) + x*x
        const float x = movevec.x * 50.f, y = movevec.y * 50.f, z = movevec.z * 50.f;
        m_iCyclesThisFrame[+eLoadType::MOVE_SPEED] = (uint32)(int64)std::sqrt((z * z + y * y) + x * x);
    }

    // 0x53D143 - 0x53D1D3
    for (auto t = 0u; t < +eLoadType::NUM_LOAD_TYPES; t++) {
        m_iMaxCycles[t] = std::max(m_iMaxCycles[t], m_iCyclesThisFrame[t]);
        // The game runs the x87 at 24-bit precision: `fild` loads the integer exactly and only the sum is rounded.
        m_fAveragedCyclesThisSecond[t] = (float)((double)m_fAveragedCyclesThisSecond[t] + (double)m_iCyclesThisFrame[t]);
    }

    auto mstime = CTimer::GetTimeInMS();
    auto delta  = mstime - m_LastTime;

    if (delta > 2'000) {
        m_LastTime = mstime;
        m_NumFramesThisSec = 0;
        return;
    }

    if (delta >= 1'000) {
        m_FPS = m_NumFramesThisSec;
        m_LastTime = mstime;

        if (m_NumFramesThisSec == 0) {
            m_NumFramesThisSec = 1;
        }

        const float recip = 1.f / (float)m_NumFramesThisSec;
        for (auto& avg : m_fAveragedCyclesThisSecond) {
            avg *= recip;
        }

        m_NumFramesThisSec = 0;

        for (auto&& [ch, acts] : rngv::zip(m_iCyclesHistory, m_fAveragedCyclesThisSecond)) {
            rng::shift_right(ch, 1);
            ch[0] = (uint32)acts;
            acts = 0.0f;
        }

        m_eProcLevel = eProcessingLevel::OK;

        const auto needsCyclesConversion = [](uint32 t) {
            return t != +eLoadType::NUM_STREAMING_REQUESTS
                && t != +eLoadType::MOVE_SPEED;
        };

        for (auto t = 0u; t < +eLoadType::NUM_LOAD_TYPES; t++) {
            // 0x53D2C0: x87 at 24-bit precision; the integers are loaded exactly (`fild`) and every operation is
            // rounded to float, so each step is computed exactly in double and then rounded.
            float smoothed{};
            if (m_iMaxCycles[t] != 0) {
                float sum = (float)((double)m_iCyclesHistory[t][0] + (double)m_iCyclesHistory[t][1]);
                for (auto a = 2u; a < 8u; a++) {
                    sum = (float)((double)sum + (double)m_iCyclesHistory[t][a]);
                }
                const float maxCycles = (float)m_iMaxCycles[t];
                smoothed = std::min(sum * 0.125f, maxCycles);
            }
            m_fSmoothedValues[t] = smoothed;

            const float val        = needsCyclesConversion(t) ? (float)((double)smoothed / (double)CTimer::GetCyclesPerMillisecond()) : smoothed;
            const float normalized = val / m_fPeakLevels[t];

            if (normalized > (2.f / 3.f) && m_eProcLevel < eProcessingLevel::HIGH) {
                m_eProcLevel = eProcessingLevel::HIGH;
            } else if (normalized > (1.f / 3.f) && m_eProcLevel < eProcessingLevel::MED) {
                m_eProcLevel = eProcessingLevel::MED;
            }

            m_fNormalizedPeakRangeValues[t] = normalized;
            m_fSmoothedValues[t] = (float)((double)smoothed / (double)m_iMaxCycles[t]);
        }
    } else {
        m_NumFramesThisSec++;
    }

    if (IsForcingProcLevel()) {
        m_eProcLevel = m_eProcLevelToForce;
    }
}

// 0x53D050
void CLoadMonitor::StartTimer(eLoadType timerIndex) {
    m_iStartTimes[+timerIndex] = CTimer::GetCurrentTimeInCycles();
}

// 0x53D070
void CLoadMonitor::EndTimer(eLoadType timerIndex) {
    m_iCyclesThisFrame[+timerIndex] += CTimer::GetCurrentTimeInCycles() - m_iStartTimes[+timerIndex];
}

// debug function?
// See LoadMonitorDebugModule
// 0x53D480
void CLoadMonitor::Render() {
    // NOP
}
