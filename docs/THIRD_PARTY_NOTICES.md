# Avisos de terceros

## SilentPatch

Algunas correcciones opcionales del ejecutable independiente y de la ASI se basan en el comportamiento descrito y el código de **SilentPatch**
(https://github.com/CookiePLMonster/SilentPatch). Solo se tomó lo necesario de cada corrección; cada archivo afectado lleva un comentario de atribución.
Todas son desviaciones deliberadas del EXE original, se apagan con `GTASA_SILENTPATCH=0` (o `SilentPatch=0` en `gta-reversed.ini`) y están catalogadas en
[DesviacionesConocidas.md](DesviacionesConocidas.md), sección 15.

Correcciones incluidas hasta ahora:

- «Streaming memory bug» (`GTARtAnimInterpolatorSetCurrentAnim` de `SilentPatchSA/SilentPatchSA.cpp`): `source/game_sa/RenderWare/rw/SilentPatchFixes.*` y `rtanim.cpp`.
- Vehículos, peds, jugador, fuego y pools (rama `silentpatch-entidades`; detalle en [CodigoVivoExe/silentpatch-entidades.md](CodigoVivoExe/silentpatch-entidades.md)): munición de armas cuerpo a cuerpo de los trucos, precisión de la IA, Securicar, impound, muzzle flash de la última bala, BMX en llamas, coche robado por el lado del pasajero, explosión doble, nitro marcha atrás, postura de las armas de los policías, aleatoriedad de script de 16 bits, luces del tráfico, extra6, Beagle de Fort Carson, SEAROADER, motoristas policía, aviones, lista de entidades en streaming, reinicio de variables en New Game e invencibilidad real. Cada sitio lleva el comentario `// SilentPatch (MIT, Silent): <arreglo>`; los cambios están en `source/game_sa/` (Automobile, Vehicle, Weapon, Cheat, Ped, tareas de pistola y de entrar al coche, Garages, CarCtrl, CarAI, Fire, Game, Streaming, FileLoader, RunningScript, MathCommands, Plane, PedDamageResponseCalculator, VehicleModelInfo, TheScripts).

Texto de la licencia de SilentPatch:

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
