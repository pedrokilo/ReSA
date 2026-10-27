<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Helpers67W: CWaterLevel::RenderSeaBedSegment 0x6E6870 (lote 67W)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-2`, HEAD 1ca520153 + `source.patch`). 16384 pares, cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados, `fp_difference` vacio, salida de cada lado igual a la del oraculo independiente y cobertura sobre los minimos. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helpers67w\20261026-070223-371509`. Fila de `docs/hooks.csv` para el coordinador: `CWaterLevel,RenderSeaBedSegment,0x006E6870,1,0,1,S`. (La corrida esperó un rato el candado `D:\Original\gta_lock` y luego se ejecutó sola.)

## Qué hace (desensamblado 0x6E6870..0x6E6A0B)
cdecl `(int32 x, int32 y, float minX, float maxX, float minY, float maxY)`, 6 argumentos que retira el llamador. Anade al bufer temporal de agua el cuadrilatero del fondo marino lejano del bloque: cuatro vertices (36 bytes, z = -70, color 0xFF505050, normales intactas), seis indices (n, n+1, n+2, n+3, n+1, n+2) y avanza los contadores (0xC4B950 +4, 0xC4B954 +6). px(a) = (x + a)*500 - 3000, py(c) = (c + y)*500 - 3000, UV = fraccion*8, x87 de 24 bits. Asimetria del EXE: el X de minX entra como entero exacto (fild) y el de maxX como el float guardado en la pila. Unicos llamadores: 0x6EF822 y 0x6EF861 (dentro de RenderWater), que no leen EAX/ECX/EDX. Nombre: `RenderSeaBedSegment` (el de la lista de funciones pendientes de `WaterLevel.h`; la hermana 0x6E6A10 es `RenderDetailedSeaBedSegment`, fuera de alcance).

## Producción
`WaterLevel.cpp/.h` (hook `RH_ScopedGlobalInstall(RenderSeaBedSegment, 0x6E6870)`; `RenderWater` ya no llama por dirección a 0x6E6870) y `X87Math.cpp/.h` (`Wl67SeaBedSegmentCpp`: pasos intermedios por `volatile double` porque `/fp:fast` distribuia `(x+a)*500`; objeto verificado instruccion a instruccion).

## Banco
16384 pares, una familia de 16 x 1024 casos; sin colaboradores; region [0xC4B950, 0xC5F958) guardada/rellena/restaurada y 0x40 bytes vigilados a cada lado. `fixture_oracle.py` (modelo independiente x87) y `fixture_selfcheck.py` (volcado compilado + minimos de cobertura) pasan sin juego.

```
bash reversing/_handoff/bench_here.sh Helpers67W
python -I reversing/Helpers67W/fixture_oracle.py
python -I reversing/Helpers67W/fixture_selfcheck.py
```
