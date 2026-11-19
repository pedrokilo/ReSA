<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn236A: 4 funciones de jdapimin de la libjpeg 6b de descompresion (lote 236A)

Banco par a par dentro del juego (plantilla `Fn234A`, misma libjpeg). Worktree `wv-6` sobre `1fb2441788`; las fuentes del lote (`LibJpegDecompressApi.cpp/.h`) estan tal cual en esa base.

| # | Funcion | Entrada | ABI |
|---|---|---|---|
| 0 | `jpeg_CreateDecompress` | 0x5CFD20 | cdecl(cinfo, version, structsize) |
| 1 | `jpeg_consume_input` (con `default_decompress_parms` 0x5CFE20 sin hook) | 0x5CFFB0 | cdecl(cinfo) -> int |
| 2 | `jpeg_finish_decompress` | 0x5D0110 | cdecl(cinfo) -> AL |
| 3 | `jpeg_read_header` | 0x5D01D0 | cdecl(cinfo, require_image BYTE) -> int |

- 16384 pares por funcion (65536 en total), `Fixtures.hpp` genera los casos y `fixture_oracle.py` es el oraculo independiente (escrito desde el desensamblado, con las tablas de salto 0x5D0048 y de indices 0x5D005C leidas del EXE).
- Escena: `jpeg_decompress_struct` en la arena A (con err, src, inputctl y master propios) y tabla de componentes en la B; las arenas C y D deben quedar intactas.
- Falsos identicos en EXE y DLL: error_exit (vuelve o sale por longjmp segun el caso), emit_message, consume_input (guion de retornos, escribe los datos de color tras la llamada, activa eoi_reached), reset_input_controller, init_source, term_source y finish_output_pass. Falsos en la entrada del EXE (hook de la DLL apagado): `jinit_memory_mgr`, `jinit_marker_reader`, `jinit_input_controller` y `jpeg_abort`. `jpeg_consume_input` es codigo real del EXE en el lado original de `jpeg_read_header`.
- Compuertas de ABI/SEH/DF/restauracion en ambos lados, x87 vacia, cobertura por familia y por rama (`KEYS` del oraculo, minimo 50 casos).
- Prueba negativa: `mutate.py` (2 rondas, una mutacion por funcion y ronda, condicionadas a una estructura desalineada que solo produce el banco). Cadena completa: `chain236.sh`.
- Autoprueba sin juego: `python -I fixture_selfcheck.py`. Registro: `register_hook.py <carpeta-de-evidencia>`.
