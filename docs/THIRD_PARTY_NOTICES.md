# Avisos de terceros

## SilentPatch

Algunas correcciones opcionales del ejecutable independiente y de la ASI se basan en el comportamiento descrito y el código de **SilentPatch**
(https://github.com/CookiePLMonster/SilentPatch). Solo se tomó lo necesario de cada corrección; cada archivo afectado lleva un comentario de atribución.
Todas son desviaciones deliberadas del EXE original, se apagan con `GTASA_SILENTPATCH=0` (o `SilentPatch=0` en `gta-reversed.ini`) y están catalogadas en
[DesviacionesConocidas.md](DesviacionesConocidas.md), sección 15. Esquema de nombres de interruptores y opciones: comentario de cabecera de `source/extensions/SilentPatchConfig.h` (único lugar donde se define).

Correcciones incluidas (rama de integración `silentpatch-integracion`):

- «Streaming memory bug» (`GTARtAnimInterpolatorSetCurrentAnim` de `SilentPatchSA/SilentPatchSA.cpp`): `source/game_sa/RenderWare/rw/SilentPatchFixes.*` y `rtanim.cpp`.
- Vehículos, peds, jugador, fuego y pools (rama `silentpatch-entidades`; detalle en [CodigoVivoExe/silentpatch-entidades.md](CodigoVivoExe/silentpatch-entidades.md)): munición de armas cuerpo a cuerpo de los trucos, precisión de la IA, Securicar, impound, muzzle flash de la última bala, BMX en llamas, coche robado por el lado del pasajero, explosión doble, nitro marcha atrás, postura de las armas de los policías, aleatoriedad de script de 16 bits, luces del tráfico, extra6, Beagle de Fort Carson, SEAROADER, motoristas policía, aviones, lista de entidades en streaming, reinicio de variables en New Game e invencibilidad real. Cada sitio lleva el comentario `// SilentPatch (MIT, Silent): <arreglo>`; los cambios están en `source/game_sa/` (Automobile, Vehicle, Weapon, Cheat, Ped, tareas de pistola y de entrar al coche, Garages, CarCtrl, CarAI, Fire, Game, Streaming, FileLoader, RunningScript, MathCommands, Plane, PedDamageResponseCalculator, VehicleModelInfo, TheScripts).
- Rama de gráficos, audio, tiempo y streaming (`docs/CodigoVivoExe/silentpatch-graficos.md`): rotación de coronas (`CoronaRotationFix`), `SunSizeHack`, `StaticShadowAlphaFix`, filtrado bilineal de los sprites de script (`DrawScriptSpritesAndRectangles`), «NoFrameDelay», `TimecycDatMissingDataFix` (`source/game_sa/Coronas.cpp`, `Shadows.cpp`, `Scripts/TheScripts.cpp`, `TimeCycle.cpp`, `Streaming.cpp`, `source/app/app_game.cpp`) y la lista de entidades en streaming ampliada.
- Scripts, misiones, entrada, HUD, trucos, guardado y textos (rama `silentpatch-scm`): ver «Entradas por área» más abajo.

Licencia de SilentPatch (archivo `LICENSE` del repositorio), texto completo:

```
The MIT License (MIT)

Copyright (c) 2024 Adrian Zdanowicz (Silent)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### Entradas por área

- **Scripts, misiones, entrada, HUD, trucos, guardado y textos (rama `silentpatch-scm`)**: `source/game_sa/Scripts/SilentPatchScripts.*` (parches de bytecode SCM de SilentPatchSA.cpp, namespace `ScriptFixes`, y ScriptSA.cpp), `source/game_sa/SilentPatchNewGame.*` (namespace `VariableResets`), y los hunks marcados con `// SilentPatch (MIT, Silent)` en TheScripts, RunningScript*, MathCommands, MissionCommands, Cheat, Camera/Cam/X87Math, Hud, Stats, MenuManager*, TheZones, FileLoader, Game, Localisation, AERadioTrackManager, Automobile y QuadBike. Detalle: [CodigoVivoExe/silentpatch-scm.md](CodigoVivoExe/silentpatch-scm.md).

## GInputSA 1.11 (lista de secuencias de botones de PS2)

Lista de secuencias de botones de PS2 tomada de la documentación de GInputSA 1.11 de Silent (autorización solicitada por el usuario del proyecto); las secuencias son las de la versión PS2 del juego.
Solo se usan las 87 secuencias de la lista publicada en el paquete (`CheatList for PS.html`); no se tomó nada de `GInputSA.asi` (no se desensambló). Uso: `source/game_sa/PadCheatsTable.inc.h`
(generado por `contrib/pad_cheats_tabla.py`) y `source/game_sa/PadCheats.*` (detector propio); desviación propia 15.P de [DesviacionesConocidas.md](DesviacionesConocidas.md), mapeo en
[CodigoVivoExe/trucos-mando.md](CodigoVivoExe/trucos-mando.md).

## GInputSA 1.11 (Silent)

Iconos del mando en las ayudas y textos (`source/game_sa/PadButtonIcons.*`, [CodigoVivoExe/iconos-mando.md](CodigoVivoExe/iconos-mando.md), [DesviacionesConocidas.md](DesviacionesConocidas.md) sección 15.B): idea y juegos de iconos de GInputSA 1.11 de Silent («Pad buttons in In-Game helps, replacing PC key names…»); las texturas no se redistribuyen, se cargan de la instalación del usuario (`models\ps3btns.txd`, `models60btns.txd`). No se copia código de GInputSA: la implementación es propia y usa los símbolos de botón que el propio juego ya dibuja (`CFont::ParseToken`, `ButtonSprite`). De su análisis estático (con autorización del usuario) se toman datos, no código: la tabla de 124 sustituciones de textos del GXT (clave, patrón, sustituto), las etiquetas de icono y los nombres de textura del TXD.
