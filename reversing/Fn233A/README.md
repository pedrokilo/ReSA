<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11736 con evidencia histórica y 705 pendientes de validación en 29 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn233A: 12 funciones de la libjpeg 6b de descompresion embebida en el EXE (lote 233A)

Estado: **APROBADO dentro del juego** (18/11/2026, worktree `wv-6` desde f8f2c804aa, HEAD de integracion del coordinador con 212A y 213A; plantillas `Fn231A`/`Fn173A`/`Fn171A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn233a/20261118-011202-734145` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn233A-20261118-011202-734145`, `comparisons.jsonl.gz`). 12 x 16384 = **196608 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `c65f539541f304f9dbea7b8f5b2ca40d072b2b46a7f952bb3843eafd4e34d469`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El listado atribuia las direcciones a `FxEmitterBP_c`; son de la libjpeg 6b del EXE (clase `LibJpegDecompress`, `source/game_sa/Plugins/JPegCompressPlugin/LibJpegDecompress.cpp`; jdhuff.c, jdinput.c y jdmainct.c). **El banco no encontro ninguna diferencia: no se cambio la fuente ni se excluyo ninguna funcion** (las 12 filas de `docs/hooks.csv` pasan a `validated = 1`).

| Funcion | Entrada |
|---|---|
| `jpeg_make_d_derived_tbl` | 0x5C9790 |
| `jpeg_fill_bit_buffer` | 0x5C9A60 |
| `jpeg_huff_decode` | 0x5C9B80 |
| `jinit_huff_decoder` | 0x5CA260 |
| `start_input_pass` | 0x5CA6E0 |
| `consume_markers` | 0x5CA720 |
| `reset_input_controller` | 0x5CA7E0 |
| `jinit_input_controller` | 0x5CA840 |
| `make_funny_pointers` | 0x5CA940 |
| `set_wraparound_pointers` | 0x5CAA90 |
| `set_bottom_pointers` | 0x5CAB70 |
| `jinit_d_main_controller` | 0x5CAE30 |

Los ayudantes sin hook (`initial_setup`, `per_scan_setup`, `latch_quant_tables`, `alloc_funny_pointers`) se ejercitan por sus llamadores. Todos los colaboradores se llaman por punteros de la propia `jpeg_decompress_struct` (falsos identicos en ambos lados). Alcance y exclusiones de los fixtures: `scope_note.txt`.

## Prueba negativa (2 rondas, 12 mutaciones por ronda, 2048 casos por funcion)
Las 12 funciones fallan en ambas rondas (lado EXE siempre dentro del oraculo): ronda 1 con 7768 pares con diferencias de 24576, ronda 2 con 6582 de 24576. Resumenes en `negative-runs/`.

## Reproducir
`CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/Fn233A/chain233.sh` (una sola sesion del candado del juego: corrida principal + 2 rondas negativas). Humo: `bash reversing/Fn233A/smoke233.sh 64`.

## Bancos anteriores que citan estas direcciones
Ninguno (busqueda con git grep en `reversing/`).
