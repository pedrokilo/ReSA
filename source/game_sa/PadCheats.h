#pragma once

#include <span>
#include "Enums/eCheats.h"

class CControllerState;

/*
 * PadCheats (NOTSA, desviacion propia; docs/DesviacionesConocidas.md 15.P, docs/CodigoVivoExe/trucos-mando.md): los trucos clasicos de PS2 introducidos con los botones del mando.
 * Lista de secuencias de botones de PS2 y tabla de hashes tomadas de la documentacion y de GInputSA 1.11 de Silent (autorizacion del usuario del proyecto); las secuencias son las de la
 * version PS2 del juego. El comportamiento (letra por boton, historial de 12, hash como el teclado, un flanco por fotograma, solo el mando 1, sin reinicio por tiempo) reproduce el del asi.
 *
 * Opcion propia: variable de entorno GTASA_PAD_CHEATS=0|1 o linea `PadCheats=0|1` en gta-reversed.ini; POR DEFECTO ACTIVADA. Apagada, el codigo es el del EXE (no se lee el mando).
 */
namespace PadCheats {

// Botones del mando de PS2 (CControllerState ya procesado por CPad). El orden es el de PRIORIDAD de GInputSA cuando varios suben en el mismo fotograma: solo se anota el primero.
enum class Btn : uint8 { Triangle, Circle, Cross, Square, Up, Down, Left, Right, L1, L2, R1, R2, COUNT };
constexpr char kLetters[] = "TCXSUDLR1234"; // letra de cada boton en la cadena que se hashea (como en GInputSA)

struct Entry {
    const char* name;  // descripcion de la lista de PS2
    eCheats     cheat; // truco que activa (id de eCheats)
    uint8       mode;  // 0 = normal; 1 = alias (misma secuencia y mismo hash que otra entrada)
    uint8       len;
    Btn         seq[12];
};

// Las 87 entradas documentadas de PS2, en el orden de la lista (para el banco y los documentos).
std::span<const Entry> Entries();
// Tabla de 90 hashes de PS2 (indice = id de eCheats; los ids 90 y 91 no tienen).
std::span<const uint32> Hashes();

// Detector: historial de las ultimas 12 pulsaciones (la mas reciente primero), coincidencia por hash de los sufijos de 12 a 6 letras, como el teclado.
class Detector {
public:
    // Anade una pulsacion; devuelve el id de eCheats reconocido o -1. Al reconocer, el historial se vacia.
    int Feed(Btn button);
    void Reset() { m_len = 0; m_hist[0] = 0; }
    uint32 BufferedCount() const { return m_len; }

private:
    char   m_hist[14]{};
    uint32 m_len = 0;
};

// Mascara de botones de un estado (un valor distinto de cero cuenta como pulsado: L2/R2 analogicos incluidos) y primer flanco por prioridad.
uint16 MaskOf(const CControllerState& state);
int    FirstEdge(uint16 mask, uint16 prevMask); // indice de Btn o -1

bool Enabled();
// Llamado por CPad::Update con el estado del mando de ESE fotograma (PCTempJoyState, antes de mezclarlo con teclado y raton). Solo el mando 1 (pad 0) cuenta, como en GInputSA.
void CaptureJoyState(int32 pad, const CControllerState& state);
// Llamado desde CCheat::DoCheats una vez por fotograma de juego (no en pausa ni menu).
void Process();

// --- Para el banco (PadCheatsSelfTest) ---
void   TestForceEnabled(int forced);                   // -1 = usa la opcion; 0/1 = fuerza
void   TestInjectState(const CControllerState& state); // equivale a CaptureJoyState(0, state)
uint32 TestActivations();                              // veces que el mando ha activado un truco desde el arranque

} // namespace PadCheats
