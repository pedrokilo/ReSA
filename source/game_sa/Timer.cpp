/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTimer
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "oswrapper.h"
#include "X87Math.h"


void CTimer::InjectHooks()
{
    RH_ScopedClass(CTimer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5617E0);
    RH_ScopedInstall(Shutdown, 0x5618C0);
    RH_ScopedInstall(Suspend, 0x5619D0);
    RH_ScopedInstall(Resume, 0x561A00);
    RH_ScopedInstall(Stop, 0x561AA0);
    RH_ScopedInstall(StartUserPause, 0x561AF0);
    RH_ScopedInstall(EndUserPause, 0x561B00);
    RH_ScopedInstall(GetCyclesPerMillisecond, 0x561A40);
    RH_ScopedInstall(GetCyclesPerFrame, 0x561A50);
    RH_ScopedInstall(GetCurrentTimeInCycles, 0x561A80);
    RH_ScopedInstall(GetIsSlowMotionActive, 0x561AD0);
    RH_ScopedInstall(UpdateVariables, 0x5618D0);
    RH_ScopedInstall(Update, 0x561B10);
}

// 64-bit RsTimer wrapper
// 0x5617C0
uint64 GetMillisecondTime() {
    return plugin::CallAndReturn<uint64, 0x5617C0>();
    // return RsTimer();
}

// Como el original: el EXE comprueba el BOOL de QueryPerformanceFrequency (no la frecuencia) y solo usa la parte baja.
bool QueryTimerFrequency(uint32& frequencyLow) {
    LARGE_INTEGER frequency;
    if (!::QueryPerformanceFrequency(&frequency)) {
        return false;
    }
    frequencyLow = frequency.LowPart;
    return true;
}

// 0x5617E0
void CTimer::Initialise()
{
    m_UserPause = false;
    m_CodePause = false;
    bSlowMotionActive = false;
    bSkipProcessThisFrame = false;

    m_snTimeInMilliseconds = 0;
    m_snTimeInMillisecondsPauseMode = 1;
    m_snTimeInMillisecondsNonClipped = 1;
    m_snPreviousTimeInMilliseconds = 0;
    m_snPPreviousTimeInMilliseconds = 0;
    m_snPPPreviousTimeInMilliseconds = 0;
    m_snPPPPreviousTimeInMilliseconds = 0;
    m_snPreviousTimeInMillisecondsNonClipped = 0;

    m_snRenderTimerPauseCount = 0;

    m_FrameCounter = 0;
    m_sbEnableTimeDebug = false;
    game_FPS = 0.0f;

    ms_fTimeScale = 1.0f;
    ms_fSlowMotionScale = -1.0f; // unused
    ms_fTimeStep = 1.0f;
    ms_fOldTimeStep = 1.0f;

    TimerFunction_t timerFunc;
    uint32 frequency;
    if (QueryTimerFrequency(frequency)) { // Como el original: BOOL de QueryPerformanceFrequency, parte baja / 1000
        timerFunc = GetOSWPerformanceTime;
        m_snTimerDivider = frequency / 1000;
    } else {
        timerFunc = GetMillisecondTime;
        m_snTimerDivider = 1;
    }
    ms_fnTimerFunction = timerFunc;
    m_snRenderStartTime = timerFunc();
}

// 0x5618C0
void CTimer::Shutdown() {
    m_sbEnableTimeDebug = false;
}

// 0x5619D0
void CTimer::Suspend()
{
    if (m_sbEnableTimeDebug)
    {
        if (++m_snRenderTimerPauseCount <= 1)
            m_snRenderPauseTime = ms_fnTimerFunction();
    }
}

// 0x561A00
void CTimer::Resume()
{
    if (m_sbEnableTimeDebug)
    {
        if (!--m_snRenderTimerPauseCount) {
            m_snRenderStartTime = ms_fnTimerFunction() - m_snRenderPauseTime + m_snRenderStartTime;
        }
    }
}

// 0x561AA0
void CTimer::Stop()
{
    m_snPPPPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
    m_snPPPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
    m_snPPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
    m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
    m_sbEnableTimeDebug = false;
    m_snPreviousTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped;
}

// 0x561AF0
void CTimer::StartUserPause()
{
    m_UserPause = true;
}

// 0x561B00
void CTimer::EndUserPause()
{
    m_UserPause = false;
}

// 0x561A40
uint32 CTimer::GetCyclesPerMillisecond()
{
    return m_snTimerDivider;
}

// cycles per ms * 20
// 0x561A50
uint32 CTimer::GetCyclesPerFrame()
{
    return static_cast<uint32>(MfDecoderCursorToPlayTimeCpp(m_snTimerDivider, 20.0f)); // Como el original: fild (+2^32) + fmul 20 + _ftol2
}

// 0x561A80
uint32 CTimer::GetCurrentTimeInCycles()
{
    // TODO: Make it use 64-bit timestamps.
    return (uint32)(GetOSWPerformanceTime() - m_snRenderStartTime);
}

// 0x561AD0
bool CTimer::GetIsSlowMotionActive()
{
    return ms_fTimeScale < 1.0f;
}

// 0x5618D0
void CTimer::UpdateVariables(float timeElapsed)
{
    /* Izzotop: from IDA directly to here (tested)
    float step = timeStep / float(m_snTimerDivider);
    m_snTimeInMillisecondsNonClipped += (uint32)(step);
    ms_fTimeStepNonClipped = step * 0.05f; // step / 20.0f;

    if (step > 300.f) {
        step = 300.f;
    }
    m_snTimeInMilliseconds += (uint32)(step);

    if (ms_fTimeStepNonClipped < 0.01f &&
        !GetIsPaused() &&
        !CSpecialFX::bSnapShotActive
    ) {
        ms_fTimeStepNonClipped = 0.01f;
    }

    ms_fOldTimeStep = ms_fTimeStep;

    if (ms_fTimeStepNonClipped > 3.0f) {
        ms_fTimeStep = 3.0f;
    } else if (ms_fTimeStepNonClipped > 0.00001f) {
        ms_fTimeStep = ms_fTimeStepNonClipped;
    } else {
        ms_fTimeStep = 0.00001f;
    }
    */

    // Como el original (transcrito del EXE): divisor y contadores cargados con fild, suma en float y _ftol2;
    // el paso no recortado es q * 0,05 (no q / 20) y los ms se truncan antes de limitar a 300 (sin signo).
    // Como el original (0x5618D0..0x56191A): q queda en la pila del x87 (exponente extendido) para la suma, el
    // producto por 0,05 (un denormal se guarda como denormal, no se anula como con SSE) y el truncado de los ms.
    uint32 elapsedMs, newNonClipped;
    float stepNonClipped;
    TimerUpdateStepCpp(timeElapsed, m_snTimerDivider, m_snTimeInMillisecondsNonClipped, &newNonClipped, &stepNonClipped, &elapsedMs);
    m_snTimeInMillisecondsNonClipped = newNonClipped;
    ms_fTimeStepNonClipped = stepNonClipped;
    if (elapsedMs > 300) {
        elapsedMs = 300;
    }
    m_snTimeInMilliseconds += elapsedMs;

    if (ms_fTimeStepNonClipped < 0.01f && !m_UserPause && !m_CodePause && !CSpecialFX::bSnapShotActive) {
        ms_fTimeStepNonClipped = 0.01f;
    }

    ms_fOldTimeStep = ms_fTimeStep;
    // Como el original: un NaN acaba en 3.0 (solo se copia si es < 3) y el mínimo es 0.00001
    const float clipped = ms_fTimeStepNonClipped < 3.0f ? ms_fTimeStepNonClipped : 3.0f;
    ms_fTimeStep = clipped > 0.00001f ? clipped : 0.00001f;
}

// 0x561B10
void CTimer::Update() {
    ZoneScoped;

    if (!ms_fnTimerFunction)
        return;

    m_sbEnableTimeDebug = true;
    game_FPS = TimerDivByU32Cpp(1000.0f, m_snTimeInMillisecondsNonClipped - m_snPreviousTimeInMillisecondsNonClipped);

    // Update history
    m_snPPPPreviousTimeInMilliseconds = m_snPPPreviousTimeInMilliseconds;
    m_snPPPreviousTimeInMilliseconds = m_snPPreviousTimeInMilliseconds;
    m_snPPreviousTimeInMilliseconds = m_snPreviousTimeInMilliseconds;
    m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;

    m_snPreviousTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped;

    // Como el original: diferencia de 64 bits convertida a float sin signo con un solo redondeo
    const uint64 nRenderTimeNow = ms_fnTimerFunction();
    auto fTimeDelta = TimerU64ToFloatCpp(nRenderTimeNow - m_snRenderStartTime);
    if (!GetIsPaused())
        fTimeDelta = ms_fTimeScale * fTimeDelta;
    m_snRenderStartTime = nRenderTimeNow;

    m_snTimeInMillisecondsPauseMode = TimerPauseModeCpp(m_snTimeInMillisecondsPauseMode, fTimeDelta, m_snTimerDivider);
    if (GetIsPaused())
        fTimeDelta = 0.0f;

    UpdateVariables(fTimeDelta);
    m_FrameCounter++;
}
