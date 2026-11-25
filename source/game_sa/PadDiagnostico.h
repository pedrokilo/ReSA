#pragma once

// Vigilancia del mando (NOTSA). Solo se activa con la variable de entorno GTASA_PAD_DEBUG=1: escribe logs/pad.log cada 0,5 s y en cada cambio
// (estado crudo de XInput, el DIJOYSTATE2 construido, CPad::Pads[0].NewState, menu/camara/cinematica y configuracion). NO altera el comportamiento del juego.
namespace PadDiag {
struct RawXInput {
    uint32 index{};
    uint32 rc{};          // resultado de XInputGetState (0 = conectado, 1167 = desconectado); 0xFFFFFFFF = la capa no se consulto (sin DLL o reintento en espera)
    uint32 packet{};
    uint16 buttons{};
    uint8  lt{}, rt{};
    int16  lx{}, ly{}, rx{}, ry{};
    bool   usingXInput{}; // el juego uso esta lectura (ProcessPad sustituyo DirectInput)
};

[[nodiscard]] bool Enabled();
void NoteRead(const RawXInput& raw, const DIJOYSTATE2* joyState);
void NoteEvent(const char* text); // acontecimientos (conmutacion de metodo de control, mapa de botones restaurado, Start->pausa...)
void EndFrame();                  // al final de CPad::UpdatePads (tras Update): escribe si toca
}
