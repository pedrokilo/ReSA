#pragma once

#include <span>
#include "Enums/eCheats.h"

class CControllerState;

/*
 * PadCheats (NOTSA, desviacion propia; docs/DesviacionesConocidas.md 15.P, docs/CodigoVivoExe/trucos-mando.md): los trucos clasicos de PS2 introducidos con los botones del mando.
 * Detecta por flanco de boton la secuencia de PS2 de cada truco y llama a la MISMA activacion que la entrada de teclado (CCheat::ActivateCheatByIndex).
 * Las secuencias son las de la version PS2 del juego, tomadas de la documentacion publica de GInputSA 1.11 de Silent (autorizacion solicitada por el usuario del proyecto).
 *
 * Opcion propia: variable de entorno GTASA_PAD_CHEATS=0|1 o linea `PadCheats=0|1` en gta-reversed.ini; POR DEFECTO ACTIVADA. Apagada, el codigo es el del EXE (no se lee el mando).
 */
namespace PadCheats {

// Botones del mando de PS2 (el estado ya procesado por CPad: CControllerState del mando fisico, sin el teclado ni el raton).
enum class Btn : uint8 { L1, L2, R1, R2, Up, Down, Left, Right, Cross, Circle, Square, Triangle, L3, R3, COUNT };

struct Entry {
    const char* name;  // descripcion de la lista de PS2
    eCheats     cheat; // truco que activa; TOTAL_CHEATS = ninguno (entrada duplicada dudosa)
    uint8       mode;  // 0 = se registra; 1 = alias de otra entrada (misma secuencia y truco); 2 = duplicada dudosa (no se registra)
    uint8       len;
    Btn         seq[12];
};

// Las 87 entradas de la lista de PS2, en el orden de la lista.
std::span<const Entry> Entries();

constexpr uint32 kIdleResetMs = 3000; // sin pulsar durante este tiempo el buffer se reinicia
constexpr int16  kPressOn     = 128;  // un valor analogico (L2/R2...) cuenta como pulsado a partir de aqui...
constexpr int16  kPressOff    = 64;   // ...y como soltado por debajo de aqui (histeresis)

// Detector de la secuencia (sin dependencias del juego salvo CCheat al activar; lo usa el banco con estados inyectados).
class Detector {
public:
    // Anade una pulsacion; devuelve el indice (en Entries()) de la entrada reconocida o -1. Al reconocer una, el buffer se vacia (como el teclado).
    int Feed(Btn button, uint32 nowMs);
    // Lee un estado de mando: genera las pulsaciones por flanco (en orden fijo de botones) y devuelve la ultima entrada reconocida o -1.
    int Poll(const CControllerState& state, uint32 nowMs);
    void Reset(bool alsoButtons = true);
    uint32 BufferedCount() const { return m_count; }

private:
    Btn    m_buf[12]{};
    uint32 m_count   = 0;
    uint32 m_lastMs  = 0;
    uint16 m_prevMask = 0; // botones pulsados en la lectura anterior
    bool   m_havePrev = false;
};

bool Enabled();
// Llamado por CPad::Update con el estado del mando de ESE fotograma (antes de mezclarlo con teclado y raton).
void CaptureJoyState(int32 pad, const CControllerState& state);
// Llamado desde CCheat::DoCheats una vez por fotograma de juego (no en pausa ni menu).
void Process();

// --- Para el banco (PadCheatsSelfTest) ---
void TestForceEnabled(int forced);                                 // -1 = usa la opcion; 0/1 = fuerza
void TestInjectState(int32 pad, const CControllerState* state);    // estado inyectado en lugar del capturado (nullptr = desactiva)
uint32 TestActivations();                                          // veces que el mando ha activado un truco desde el arranque

} // namespace PadCheats
