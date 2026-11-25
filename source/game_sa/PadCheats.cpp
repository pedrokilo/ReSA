// NOTSA (desviacion propia, docs/DesviacionesConocidas.md 15.P): trucos de PS2 con los botones del mando; ver PadCheats.h.
// Lista de secuencias de PS2 y tabla de hashes: GInputSA 1.11 de Silent (autorizacion del usuario del proyecto; docs/THIRD_PARTY_NOTICES.md).
// El comportamiento sale del analisis estatico de GInputSA.asi (docs/CodigoVivoExe/trucos-mando.md); el codigo es propio.
#include "StdInc.h"

#include "PadCheats.h"
#include "Cheat.h"
#include "CutsceneMgr.h"
#include "ControllerState.h"
#include "extensions/SilentPatchConfig.h"

namespace PadCheats {
namespace {

#define PADCHEATS_HASHES
#include "PadCheatsTable.inc.h"
#undef PADCHEATS_HASHES

constexpr Entry kEntries[] = {
#include "PadCheatsTable.inc.h"
};
static_assert(std::size(kEntries) == 87, "la lista de PS2 tiene 87 entradas");

int      g_forced      = -1;
uint32   g_activations = 0;
uint16   g_mask        = 0; // botones pulsados en el ultimo fotograma capturado
uint16   g_rising      = 0; // flancos de subida acumulados desde el ultimo Process
Detector g_detector;

bool IsDown(const CControllerState& s, Btn b) {
    switch (b) {
    case Btn::Triangle: return s.ButtonTriangle != 0;
    case Btn::Circle:   return s.ButtonCircle != 0;
    case Btn::Cross:    return s.ButtonCross != 0;
    case Btn::Square:   return s.ButtonSquare != 0;
    case Btn::Up:       return s.DPadUp != 0;
    case Btn::Down:     return s.DPadDown != 0;
    case Btn::Left:     return s.DPadLeft != 0;
    case Btn::Right:    return s.DPadRight != 0;
    case Btn::L1:       return s.LeftShoulder1 != 0;
    case Btn::L2:       return s.LeftShoulder2 != 0;
    case Btn::R1:       return s.RightShoulder1 != 0;
    case Btn::R2:       return s.RightShoulder2 != 0;
    default:            return false;
    }
}

} // namespace

std::span<const Entry>  Entries() { return kEntries; }
std::span<const uint32> Hashes() { return kHashes; }

int Detector::Feed(Btn button) {
    // Historial: la pulsacion mas reciente en la posicion 0 (como la cadena del teclado de PC); maximo 12
    const uint32 n = std::min<uint32>(m_len, 11);
    for (uint32 i = n; i > 0; i--) {
        m_hist[i] = m_hist[i - 1];
    }
    m_hist[0]     = kLetters[(int)button];
    m_len         = n + 1;
    m_hist[m_len] = 0;
    if (m_len < 6) {
        return -1;
    }
    // Como AddToCheatString: se prueba la cadena completa y se va acortando por el final (la pulsacion mas antigua) hasta 6 letras
    char buf[14];
    std::memcpy(buf, m_hist, m_len + 1);
    for (uint32 len = m_len; len >= 6; len--) {
        buf[len] = 0;
        const uint32 hash = CKeyGen::GetKey(buf);
        for (size_t id = 0; id < std::size(kHashes); id++) {
            if (kHashes[id] == hash) {
                Reset();
                return (int)id;
            }
        }
    }
    return -1;
}

uint16 MaskOf(const CControllerState& state) {
    uint16 m = 0;
    for (int b = 0; b < (int)Btn::COUNT; b++) {
        if (IsDown(state, (Btn)b)) {
            m |= (uint16)(1u << b);
        }
    }
    return m;
}

int FirstEdge(uint16 mask, uint16 prevMask) {
    const uint16 rising = (uint16)(mask & ~prevMask);
    for (int b = 0; b < (int)Btn::COUNT; b++) {
        if ((rising >> b) & 1) {
            return b;
        }
    }
    return -1;
}

bool Enabled() {
    if (g_forced >= 0) {
        return g_forced != 0;
    }
    static const bool v = OwnDeviationOption("GTASA_PAD_CHEATS", "PadCheats", true);
    return v;
}

void CaptureJoyState(int32 pad, const CControllerState& state) {
    if (pad != 0 || !Enabled()) { // solo el mando 1
        return;
    }
    // Variante propia: los flancos se acumulan hasta que Process los consume (si CPad::Update corriera dos veces entre dos DoCheats, GInputSA perderia la pulsacion)
    const uint16 now = MaskOf(state);
    g_rising |= (uint16)(now & ~g_mask);
    g_mask = now;
}

void Process() {
    if (!Enabled()) {
        return;
    }
    // Como la entrada de teclado (AddToCheatString): nada durante una cinematica
    if (CCutsceneMgr::ms_running) {
        g_rising = 0;
        return;
    }
    // Un solo flanco por fotograma (el primero por prioridad); los demas se pierden, como en GInputSA
    const int b = FirstEdge(g_rising, 0);
    g_rising    = 0;
    if (b < 0) {
        return;
    }
    if (const int id = g_detector.Feed((Btn)b); id >= 0) {
        g_activations++;
        CCheat::ActivateCheatByIndex(id);
    }
}

void   TestForceEnabled(int forced) { g_forced = forced; }
void   TestInjectState(const CControllerState& state) { CaptureJoyState(0, state); }
uint32 TestActivations() { return g_activations; }

} // namespace PadCheats
