# SilentPatch: gráficos, audio, tiempo y streaming (rama silentpatch-graficos, 24/11/2026)

Fuente: SilentPatch para SA (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent); `CHANGELOG-SA.md`, `Config\SilentPatchSA.ini`, `SilentPatchSA\*.cpp`). Atribución y licencia: `docs/THIRD_PARTY_NOTICES.md`. Cada sitio lleva `// SilentPatch (MIT, Silent): ...`.

## Cómo apagarlo

- Todo SilentPatch: `GTASA_SILENTPATCH=0` (entorno) o `SilentPatch=0` en `gta-reversed.ini`. Por defecto encendido (una constante en `source/extensions/SilentPatchConfig.cpp`). Apagado, `SilentPatchOption()` devuelve false y el comportamiento es el del EXE.
- Un solo arreglo: `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0` o `SilentPatch_<Nombre>=0` en el ini. Las opciones se leen una vez por proceso (hay que reiniciar para cambiarlas).
- Los valores por defecto son los de SilentPatchSA.ini; los arreglos sin opción allí van activados (nombre propio en la tabla).

## Implementados (clase A)

| Viñeta del changelog | Función de SilentPatch | Función nuestra | Opción | Estado |
| --- | --- | --- | --- | --- |
| «Coronas now properly rotate as the camera gets closer» | `CoronaRotationFix::RenderOneXLUSprite_Rotate_Aspect_SilentPatch` | `CCoronas::Render` (Coronas.cpp) | `CoronaRotation` (1) | implementado, compilado; no ejercitado en juego |
| Sol más grande (ini `SunSizeHack=1`) | `SunSizeHack::DoSunAndMoon_SunSizeHack` | `CCoronas::DoSunAndMoon` | `SunSizeHack` (1) | implementado, compilado; no ejercitado |
| Sombras estáticas/guardadas sin prueba alfa (ligado a «Weapons and a jetpack now cast proper shadows» / arreglos de sombras; `StaticShadowAlphaFix`) | `StaticShadowAlphaFix::Render*Shadows_StateFix` | `CShadows::RenderStaticShadows`, `RenderStoredShadows` | `StaticShadowAlphaFix` (1) | implementado, compilado; no ejercitado |
| «Script sprites now have bilinear filtering applied» | `DrawScriptSpritesAndRectangles` | `CTheScripts::DrawScriptSpritesAndRectangles` | `ScriptSpritesBilinear` (1) | implementado, compilado; no ejercitado |
| «The 14ms frame delay has been removed» | parche 0x53E923/0x53E99F | `Idle` (app_game.cpp) | `NoFrameDelay` (1) | implementado, compilado; no medido (FPS) |
| «If the illumination value is absent from the timecycle file... defaults to 1.0» y «red-tinted countryside evening rain» | `TimecycDatMissingDataFix::sscanf_TimecycLine` | `CTimeCycle::Initialise` | `TimecycMissingDataFix` (1) | implementado, compilado; no ejercitado |
| «The streamed entity list has been expanded a bit» | parche 0x5B8E55/0x5B8EB0 (12000 -> 15000 bytes) | `CStreaming::Init` (`ms_rwObjectInstances.Init(1250)`) | `BiggerStreamedEntityList` (1) | implementado; no ejercitado |

Estado de las pruebas: ver el final de este documento (las comprobaciones realmente hechas).

## Clasificadas y NO implementadas todavía

Clase A pendientes (viable, no hecho por falta de tiempo en esta pasada; ninguna tocada): «Flare effects now scale to resolution correctly» (`CoronaFlaresScaling`, necesita el módulo UIScales), «Coronas no longer have a Z test forced» y sun lens flare con AMD/Intel (`LensFlareRenderFix`, cambia el orden de FlushSpriteBuffer en `CCoronas::Render`), escalado de créditos / cruceta del mapa / calor / ondas bajo el agua / mira de misil / relleno de cajas de texto (UIScales, un bloque grande común), «Light shadows from fire», «Weapons and a jetpack cast proper shadows», «Shadows and lights cast correctly on rotated map objects», «Script draws can no longer cause the radio station name text to wrap», «Mission title and Passed texts same duration regardless of resolution», estrellas fugaces blancas, texto «Small Steam texts» (ini `SmallSteamTexts=-1`: desactivado por defecto), sprites de luz de coche, «Timers tick more accurately» (CTimer::Update con contador >1000 FPS; falta revisar `Timer.cpp`), «Dancing minigame timings», «Very long loading times loop loading screens», «Moonphases», «Wet road reflections», luces extra en vehículos (6 luces).

Clase B (ya hecho/diferente en el proyecto): sin evidencia comprobada en esta pasada; ninguna marcada B sin verificar.

Clase C (no aplica al standalone: parche del EXE, compatibilidad o UI de Windows): DirectPlay, ruta de archivos de usuario por API, juegos III/VC/SA simultáneos, choque al minimizar junto a espejo, bloqueo del cursor, `FILE_FLAG_NO_BUFFERING` en el IMG (el CdStream propio no lo usa), IMG > 4 GB (CdStream propio), diálogo de monitores y `FriendlyMonitorNames`, resolución por defecto del escritorio / refresco del escritorio / 16:9 seleccionable / MSAA 2x-16x (D3D9 propio en rwcore.cpp: pendiente decidir, D si dependen del hardware), Alt+F4, aspecto Steam/RGL, textos de las versiones 1.01/3.0/Steam, fugas de memoria de 1.01.

Clase D (necesitan bibliotecas o hardware que no tenemos): decodificador FLAC (`FLACDecoderSA`), extensiones WAV de 8/16/24 bits y ID3 (`WaveDecoderSA`), MP3 sin QuickTime (el standalone ya usa Media Foundation), EAX/NVIDIA splashes (el standalone no los tiene; el ASI sí: `SkipIntroSplashes=1`).

Clase E (ambigua): «Fixed a streaming-related deadlock» / «CdStream data racing». Ver la sección siguiente.

## CdStream y la rama carga-guardar-2

La viñeta «streaming-related deadlock ... entering interiors» (SilentPatch: sincronización de `CdStreamSync`/hilo de lectura) es del CdStream del EXE. El standalone tiene un CdStream propio; no se ha comparado con el cuelgue de ~120 s ni con la sección crítica de `carga-guardar-2` (sin integrar, no demostrada como causa). No se ha implementado nada: queda en E hasta leer `CdStreamSA` de SilentPatch contra nuestro CdStream.

## Pruebas realizadas

- Compilación de los archivos tocados con `compilar_over.cmd` (resultado en la sección siguiente tras la ejecución).
- Interruptor apagado: por construcción, cada sitio hace `SilentPatchOption(...)` -> false y ejecuta el código anterior sin cambios (el código antiguo se conserva literal en las ramas `false`).
