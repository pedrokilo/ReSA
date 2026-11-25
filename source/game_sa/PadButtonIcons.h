#pragma once

#include "ControllerConfigManager.h"
#include "Sprite2d.h"

/*
 * Iconos del mando en las ayudas y textos (NOTSA, no existe en el EXE). Idea y juegos de iconos de GInputSA 1.11 de Silent: cuando se juega con mando, los
 * textos `~k~~ACCION~` muestran el boton asignado en el mando (circulo, cruz, L1...) como el sprite en linea que ya dibuja CFont (simbolos ~O~, ~X~, ~K~...,
 * ButtonSprite[]) en lugar del nombre de tecla. Las texturas NO se redistribuyen: se cargan en tiempo de ejecucion de `models\ps3btns.txd` (PlayStation) o
 * `models\x360btns.txd` (Xbox) de la carpeta del juego; sin ellas el comportamiento es el del EXE (y una linea en el log). Ver docs/CodigoVivoExe/iconos-mando.md.
 *
 * Opciones propias (OwnDeviationOption): GTASA_PAD_ICONS=0|1 / `PadIcons=` (por defecto 1; 0 = comportamiento del EXE) y GTASA_PAD_BUTTONS=auto|ps|xbox /
 * `PadButtons=` (por defecto ps: el mapa de botones por defecto del proyecto es el de PS2; auto = xbox si el mando es de Microsoft/Xbox, si no ps).
 */
namespace PadButtonIcons {
// Se llama una vez por fotograma desde CPad::ProcessPad (PAD1) con el estado del mando: sirve para saber si el ultimo dispositivo usado fue el mando o el teclado/raton.
void NoteInput(const DIJOYSTATE2& joy, bool usingXInput);

// true si el juego debe mostrar iconos de mando ahora (opcion activada, mando presente y ultimo dispositivo = mando).
[[nodiscard]] bool PadModeActive();

// Si la accion tiene un boton de mando con icono disponible, escribe en dst la etiqueta de simbolo (p. ej. "~X~") y devuelve true; si no, devuelve false y dst no se toca
// (el llamador usa el nombre en texto del EXE).
bool WriteToken(eControllerAction action, GxtChar* dst, uint16 maxLen);

// Ancho en pantalla que ocupa el simbolo de la etiqueta que empieza en `tag` (texto tras el '~' de apertura) cuando se dibuja como icono; 0 si no es un icono cargado por este
// modulo. CFont::GetStringWidth lo suma (el EXE no cuenta el ancho de los simbolos, que con iconos desbordaria el ajuste de linea).
[[nodiscard]] float SymbolAdvance(const char* tag);

// Etiquetas de icono propias de GInputSA (`~[~`, `~]x~`, `~ud~`, `~<>~`...): devuelve el numero de simbolo (> 14) si `tag` (texto tras el '~' de apertura) es una y su textura esta
// cargada, 0 si no. CFont::ParseToken lo consulta antes de su tabla; CFont::PrintChar pide el sprite con SpriteFor.
[[nodiscard]] int ExtendedSymbol(const char* tag);
[[nodiscard]] CSprite2d& SpriteFor(int symbol);

// Sustituciones de textos del GXT de GInputSA (clave -> expresion regular -> texto con iconos); lo llama CText::Get con el texto original y devuelve ese mismo puntero si no hay
// sustitucion (opcion apagada, dispositivo teclado, sin TXD, clave ausente o texto de otro idioma que no contiene la secuencia).
[[nodiscard]] const GxtChar* ApplyGxtFix(const char* key, const GxtChar* original);

// Libera los sprites y los TXD propios (CFont::Shutdown, antes de que borre los suyos).
void Shutdown();
} // namespace PadButtonIcons
