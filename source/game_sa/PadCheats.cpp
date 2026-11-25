// NOTSA (desviacion propia, docs/DesviacionesConocidas.md 15.P): trucos de PS2 con los botones del mando; ver PadCheats.h.
// Lista de secuencias de botones de PS2 tomada de la documentacion de GInputSA 1.11 de Silent (autorizacion solicitada por el usuario del proyecto);
// las secuencias son las de la version PS2 del juego (docs/THIRD_PARTY_NOTICES.md). Solo se toman las secuencias de la documentacion publica.
#include "StdInc.h"

#include "PadCheats.h"
#include "Cheat.h"
#include "CutsceneMgr.h"
#include "ControllerState.h"
#include "extensions/SilentPatchConfig.h"

namespace PadCheats {
namespace {

constexpr Entry kEntries[] = {
#include "PadCheatsTable.inc.h"
};
static_assert(std::size(kEntries) == 87, "la lista de PS2 tiene 87 entradas");

int g_forced = -1;
uint32 g_activations = 0;
CControllerState g_captured[2]{};
CControllerState g_injected[2]{};
bool g_hasInjected[2]{};
Detector g_detector[2];
uint32 g_lastFrame = 0;

int16 StateValue(const CControllerState& s, Btn b) {
    switch (b) {
    case Btn::L1:       return s.LeftShoulder1;
    case Btn::L2:       return s.LeftShoulder2;
    case Btn::R1:       return s.RightShoulder1;
    case Btn::R2:       return s.RightShoulder2;
    case Btn::Up:       return s.DPadUp;
    case Btn::Down:     return s.DPadDown;
    case Btn::Left:     return s.DPadLeft;
    case Btn::Right:    return s.DPadRight;
    case Btn::Cross:    return s.ButtonCross;
    case Btn::Circle:   return s.ButtonCircle;
    case Btn::Square:   return s.ButtonSquare;
    case Btn::Triangle: return s.ButtonTriangle;
    case Btn::L3:       return s.ShockButtonL;
    case Btn::R3:       return s.ShockButtonR;
    default:            return 0;
    }
}

} // namespace

std::span<const Entry> Entries() { return kEntries; }

int Detector::Feed(Btn button, uint32 nowMs) {
    // Reinicio por inactividad (el reloj puede ser el del juego, que se detiene en pausa)
    if (m_count > 0 && nowMs - m_lastMs > kIdleResetMs) {
        m_count = 0;
    }
    m_lastMs = nowMs;
    if (m_count == std::size(m_buf)) {
        std::copy(m_buf + 1, m_buf + std::size(m_buf), m_buf);
        m_count--;
    }
    m_buf[m_count++] = button;

    // Coincidencia del sufijo del buffer con cada secuencia registrada. Ninguna secuencia de la lista es sufijo de otra distinta (comprobado por el banco).
    for (size_t i = 0; i < std::size(kEntries); i++) {
        const auto& e = kEntries[i];
        if (e.mode != 0 || e.len > m_count) {
            continue;
        }
        bool ok = true;
        for (uint32 k = 0; k < e.len && ok; k++) {
            ok = m_buf[m_count - e.len + k] == e.seq[k];
        }
        if (ok) {
            m_count = 0; // como el teclado: la cadena se vacia al reconocer un truco
            return (int)i;
        }
    }
    return -1;
}

void Detector::Reset(bool alsoButtons) {
    m_count = 0;
    if (alsoButtons) {
        m_havePrev = false;
        m_prevMask = 0;
    }
}

int Detector::Poll(const CControllerState& state, uint32 nowMs) {
    uint16 mask = 0;
    for (int b = 0; b < (int)Btn::COUNT; b++) {
        const int16 v     = StateValue(state, (Btn)b);
        const bool  was   = (m_prevMask >> b) & 1;
        const bool  down  = was ? (v >= kPressOff) : (v >= kPressOn);
        mask |= (uint16)(down ? 1u << b : 0u);
    }
    int result = -1;
    if (m_havePrev) {
        // Flancos de subida; si hay varios en el mismo fotograma se anaden en el orden fijo de Btn
        for (int b = 0; b < (int)Btn::COUNT; b++) {
            if (((mask >> b) & 1) && !((m_prevMask >> b) & 1)) {
                if (const int r = Feed((Btn)b, nowMs); r >= 0) {
                    result = r;
                }
            }
        }
    }
    m_prevMask = mask;
    m_havePrev = true;
    return result;
}

bool Enabled() {
    if (g_forced >= 0) {
        return g_forced != 0;
    }
    static const bool v = OwnDeviationOption("GTASA_PAD_CHEATS", "PadCheats", true);
    return v;
}

void CaptureJoyState(int32 pad, const CControllerState& state) {
    if (pad >= 0 && pad < 2) {
        g_captured[pad] = state;
    }
}

void Process() {
    if (!Enabled()) {
        return;
    }
    const uint32 frame = CTimer::GetFrameCounter();
    // Como la entrada de teclado (AddToCheatString): nada durante una cinematica
    const bool blocked = CCutsceneMgr::ms_running;
    // Si se salto algun fotograma de juego (pausa, menu, carga), el buffer se reinicia
    const bool gap = g_lastFrame != 0 && frame - g_lastFrame > 2;
    g_lastFrame = frame;
    for (int pad = 0; pad < 2; pad++) {
        auto& det = g_detector[pad];
        if (blocked) {
            det.Reset(true); // en cinematica no se anota nada; al volver, lo que estuviera pulsado no cuenta como pulsacion nueva
            continue;
        }
        if (gap) {
            det.Reset(false); // tras una pausa/menu/carga se vacia el buffer, pero se conserva el estado anterior de los botones (una pulsacion nueva sigue contando)
        }
        const auto& st = g_hasInjected[pad] ? g_injected[pad] : g_captured[pad];
        const int r = det.Poll(st, CTimer::GetTimeInMS());
        if (r >= 0) {
            const auto cheat = kEntries[r].cheat;
            if (cheat < TOTAL_CHEATS) {
                g_activations++;
                CCheat::ActivateCheatByIndex(cheat);
            }
        }
    }
}

void TestForceEnabled(int forced) { g_forced = forced; }
void TestInjectState(int32 pad, const CControllerState* state) {
    if (pad < 0 || pad > 1) {
        return;
    }
    g_hasInjected[pad] = state != nullptr;
    if (state) {
        g_injected[pad] = *state;
    }
}
uint32 TestActivations() { return g_activations; }

} // namespace PadCheats
