# Avisos de terceros

## SilentPatch for GTA San Andreas (MIT)

Parte de los arreglos del juego portados al código reconstruido de este proyecto (comentarios `// SilentPatch (MIT, Silent): <arreglo>` en cada sitio) están adaptados del código fuente de SilentPatch, <https://github.com/CookiePLMonster/SilentPatch>. Sólo se copia o adapta el mínimo necesario; los parches de bytes sobre direcciones del EXE no se usan: cada arreglo cambia nuestra función reconstruida. Todos son opcionales (interruptor general `GTASA_SILENTPATCH=0|1` o `SilentPatch=0|1` en la sección `[SilentPatch]` de `gta-reversed.ini`; cada arreglo tiene además su opción propia con el valor por defecto de `Config\SilentPatchSA.ini`).

Licencia de SilentPatch (archivo `LICENSE` del repositorio):

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
