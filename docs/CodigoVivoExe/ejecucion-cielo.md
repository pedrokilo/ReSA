<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cielo blanco y 184 avisos «Línea de timecyc incorrecta» en el EXE independiente (22/11/2026)

Rama `sky-fix` (base `1ec835719c`). Objetivo CMake `gta_sa_standalone`, partida nueva con `--shims` (ver `fase8-experimento.md` sec. 8).

## Conclusión

La hipótesis del `sscanf` / CRT propio **no se confirma**. El timecyc se lee bien y el cielo calculado es el correcto; el blanco es de dibujo:

1. **Los 184 avisos son ruido, no un fallo de lectura.** El formato de `CTimeCycle::Initialise` (52 conversiones, idéntico al del EXE en `0x5BBAC0`) termina en `%d %d %f` (multiplicador direccional), pero el `timecyc.dat` de la 1.0 solo trae **51 columnas** por línea. `sscanf` devuelve 51 en las 183 líneas buenas y 20 en la línea 320 del archivo (la de R*, que trae un solo `255` en lugar de los tres RGB). El código avisaba con `n < 52`, así que avisaba en las 184.
2. **Banco** (`D:\Original\_sky_bench\b.cpp`, `sscanf` de MSVC con el mismo formato sobre `data\timecyc.dat` real): 183 líneas con `n=51`, la línea 135 de datos con `n=20`. El CRT propio da lo mismo: tras el arreglo el log del EXE independiente trae un único aviso, con `n=20` y el texto exacto de esa línea.
3. **Las tablas están bien**: volcado de `m_nSkyTopRed/...[hora][clima]` frente al archivo (clima 1, horas 3 y 4: cielo `63,205,255` / `30,117,210`, etc. coinciden). Con la partida nueva el cielo calculado (`m_CurrentColours`) es `90,175,241` arriba y `193,145,100` abajo (clima 14, 6:30), azul de día, y `LightsMult = 1`.
4. **Tramo de la cinemática inicial** (primeros ~23 s): `CCutsceneMgr::LoadCutsceneData_preload` llama a `StartExtraColour(2)` (línea `EXTRACOL` del `.cut`; `ret` resuelto con el `.map`); `CalcColoursForPoint` mezcla entonces la fila de color extra (cielo `255,255,255` / `5,5,5`, ambiente 0). Es el comportamiento del código (tablas del juego); se apaga con `StopExtraColour` al acabar la cinemática (`m_bExtraColourOn=0`) y el cielo vuelve a `90,175,241`.
5. **Por qué sigue blanco en juego**: el cielo se dibuja con `RwIm3DTransform` + `RwIm3DRenderPrimitive` (`CClouds::RenderSkyPolys` / `SetUpOneSkyPoly`), es decir, por la canalización Im3D por defecto: `ImmInstance` + **`nodeD3D9SubmitNoLight.csl` (`0x80E270`)**, que el experimento tiene **sustituido por un shim que devuelve 1 y no dibuja**. Además la limpieza de cada fotograma es solo `D3DCLEAR_ZBUFFER|STENCIL` (medido en el vtable del dispositivo: `Clear flags=0x6`, sin `TARGET`), así que lo que el cielo no pinta queda con el contenido anterior del backbuffer, aquí blanco. Todo lo que pasa por Im3D sin luz (cielo, nubes, sol, coronas de Im3D) falta por el mismo motivo.

Por tanto **no hay arreglo de CRT que haga el cielo azul**: se arreglará con la reconstrucción de `0x80E270` (agente de SubmitNoLight). No se pudo comprobar con capturas que el cielo deje de ser blanco: sigue blanco con el shim (`D:\Original\sim_out\cielo-sk6-timecyc-ok-cielo-blanco.png`, `cielo-skf-final.png`).

## Cambio en C++ (`source/game_sa/TimeCycle.cpp`)

* El aviso solo salta con `n < 51` y ahora dice cuántos campos leyó. Resultado: 1 aviso (la línea 320 real de R*) en lugar de 184. No cambia ningún valor leído.

## Riesgos / observaciones

* `dirMult` (columna 52) nunca se lee: es una variable local sin inicializar, igual que en el EXE (allí es la basura de pila). En el EXE independiente vale 0 en las 184 filas (`m_nDirectionalMult = 0`, así `m_fIllumination = 0` y la luz direccional de `app_light.cpp` sale a 0). No se pudo medir el valor del EXE original (solo estático); si el ASI normal ve otro valor, la iluminación direccional del EXE independiente difiere. Pendiente: fijar el valor con una prueba dinámica del original antes de inicializarla.
* Para depurar se midieron `m_ExtraColour*`, `InTunnelness`, `UnderWaterness`: tunel y agua a 0; el color extra solo en la cinemática.

## Cómo reproducir

```
bash /d/Original/_cc_SKY.sh source/game_sa/TimeCycle.cpp     # recompila a over\ y reenlaza (scripts de wv-sky: usan los .obj de wv-int)
bash /d/Original/_run_sky.sh <etiqueta> 40 32 3 "--shims"     # partida nueva, captura a los 32 s
```
