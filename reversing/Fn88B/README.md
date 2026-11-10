<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 883 grupos / 9905 funciones reconstruidas; 9905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn88B: `CWaterLevel::RenderSeaBedSegmentNear` (lote 88B)

Estado: **APROBADO dentro del juego** (04/11/2026, worktree `wd-2` sobre main 07238e172 + los cambios del lote). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/restauracion en ambos lados, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn88b/20261104-000908-327565` (archivada en `reversing/CppTwelveLevel6Final/runs/`). Una funcion de codigo de juego reconstruida en C++ puro y enganchada (tipo S, 0x6E6A10; antes llamada por direccion desde `RenderWater`).

## Funcion
cdecl (6 argumentos, `ret` del llamador): `(int x, int y, float minX, float maxX, float minY, float maxY)`. Version subdividida de `RenderSeaBedSegment` (0x6E6870) para el fondo marino cercano: reparte el rectangulo en max(1, trunc(ancho*4)) x max(1, trunc(alto*4)) celdas y anade al bufer temporal de agua cuatro vertices (z = -70, color 0xFF505050) y seis indices por celda; contadores 0xC4B950/0xC4B954 al final. La aritmetica x87 de 24 bits esta en `X87Math.cpp` (`Wl88AxisCells`, `Wl88Column`, `Wl88Row`). No llama a ninguna funcion del juego (solo `_ftol2` del CRT).

## Banco
Region del bufer temporal de agua [0xC4B950, 0xC5F958) guardada, rellenada por caso y restaurada; se compara FNV-1a de vertices e indices escritos, los dos primeros vertices, contadores y el resto de la region. Ver `SCOPE_NOTE` de `register_hook.py`.

## Fuera de los fixtures
Anchos que dan mas de 11 celdas por eje (la funcion no comprueba el desbordamiento del bufer) y NaN senalizantes.

## Prueba negativa
Produccion mutada en seis puntos (z -70 -> -69, color 0xFF505051, orden de los dos ultimos indices, orden de las Y de la celda, `ancho*4` -> `ancho*3`, `-3000` -> `-3001` en la fila): el banco FALLA con 16384/16384 pares con diferencias (`resultados-fn88b/20261104-000741-744171`); fuente restaurada y banco repetido (aprobado, ejecucion final).

## Bancos repetidos
`WaterRender355` (cita 0x6E6A10; ahora usa el helper real de la DLL frente al EXE en `SeaNear`): aprobado, 1024 pares (`resultados-water-render355/20261104-001241-790019`).
