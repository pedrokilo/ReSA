#include "StdInc.h"

#include "OnscreenTimerEntry.h"
#include "TheScripts.h"
#include "X87Math.h"

void COnscreenTimerEntry::Init() {
    m_nVarId = 0;

    memset(&m_szDescriptionTextKey, 0, sizeof(m_szDescriptionTextKey)); // original initialization

    m_bEnabled = false;
    m_nTimerDirection = eTimerDirection::DECREASE;
    m_nClockBeepCountdownSecs = 12;
}

// 0x44CB10 (lote 185A, sin validar; el listado decia COnscreenCounterEntry; thiscall, `ret`, 167 bytes): avanza el reloj de la pantalla. Sin variable (+0 == 0) no hace nada. Paso en milisegundos = _ftol(paso * 0.02 * 1000)
// con la x87 (TimerStepMsCpp, 0x858B38 y 0x858C4C). Lee el valor de la variable de script (ScriptSpace + id, sin el assert de GetPointerToScriptVariable) y calcula los segundos del valor ANTERIOR (division con signo
// entre 1000). Direccion 0 (INCREASE): suma el paso y termina. Cualquier otro byte (DECREASE): resta el paso; si el resultado es negativo pone el valor, la variable (+0), el primer byte del texto (+4) y el habilitado
// (+0x38) a cero; si no, y solo si los segundos anteriores (sin signo) son MENORES que la cuenta atras (+0x3C) y no hay pantalla ancha (TheCamera.m_bWideScreenOn), emite el evento 0x21 (AE_FRONTEND_TIMER_COUNT, 0.0f, 1.0f).
void COnscreenTimerEntry::Process() {
    if (m_nVarId == 0)
        return;

    const uint32 varId  = m_nVarId;
    const int32  stepMs = TimerStepMsCpp(CTimer::GetTimeStep());
    int32&       value  = *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[varId]);
    const int32  before = value;
    const int32  beforeSeconds = before / 1000;

    if (m_nTimerDirection == eTimerDirection::INCREASE) {
        value = static_cast<int32>(static_cast<uint32>(before) + static_cast<uint32>(stepMs));
        return;
    }

    value = static_cast<int32>(static_cast<uint32>(before) - static_cast<uint32>(stepMs));
    int32& after = *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[m_nVarId]);
    if (after < 0) {
        after = 0;
        m_nVarId = 0;
        m_szDescriptionTextKey[0] = '\0';
        m_bEnabled = false;
        return;
    }

    if (static_cast<uint32>(beforeSeconds) < m_nClockBeepCountdownSecs && !TheCamera.m_bWideScreenOn) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_TIMER_COUNT, 0.0f, 1.0f);
    }
}

// 0x44CA40
void COnscreenTimerEntry::ProcessForDisplayClock() {
    auto time = *CTheScripts::GetPointerToScriptVariable(m_nVarId);
    // Como el original: sprintf sin limite (sprintf_s rellena con 0xFE en depuracion)
    sprintf(
        m_szDisplayedText,
        "%02d:%02d",
        time / 1000 / 60 % 100,
        time / 1000 % 60
    );
}
