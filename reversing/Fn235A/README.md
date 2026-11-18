# Fn235A: 30 funciones de la libjpeg 6b embebida en el EXE (lote 235A: jdpostct, jdsample, jerror, jmemmgr, jmemnobs, jutils y jcapimin)

Estado: **APROBADO dentro del juego** (18/11/2026, worktree `wv-5` desde 0c8b256677, HEAD de integracion del coordinador con los lotes validados hasta el 232A; plantillas `Fn232A`, `Fn233A`, `Fn219A` y `Fn173A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn235a/20261118-101643-792821` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn235A-20261118-101643-792821`, `comparisons.jsonl.xz`). 30 x 16384 = **491520 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, escrito desde el desensamblado: `desensamblado/`). SHA256 de la sonda ejecutada `7e58ad6f86d6d105d3e9fe6cb479e92cfca024e28ba7fba53a7133f73d6953fb`, del registro `c1f30ad53980333d743b317eec177cff8c1a944f2d8fb671fbf079491a2cb88c` y del EXE `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El listado atribuia las direcciones a `FxEmitterBP_c`; son de la libjpeg 6b del EXE (clases `LibJpegDecompressUpsample`, `LibJpegError`, `LibJpegMemory` y `LibJpegCompressApi`, en `source/game_sa/Plugins/JPegCompressPlugin/`).

## Defecto que encontro el banco (corregido en la fuente)

`jinit_upsampler` (0x5CD350, `LibJpegDecompressUpsample.cpp`): el EXE lee `max_v_samp_factor`, `max_h_samp_factor`, `output_width` y `mem` ANTES de llamar a `jround_up` (0x5CF8A0) y empuja `max_v` en la pila para la llamada a `alloc_sarray` (`mov edx,[esi+114h]` ... `push edx` ... `call 5CF8A0` ... `call [edi+8]`). La version C++ releia `max_v_samp_factor` DESPUES de `jround_up`, de modo que un efecto lateral de `jround_up` sobre ese campo cambiaba el cuarto argumento de `alloc_sarray` (1/512 pares en el humo; el lado EXE coincidia con el oraculo y la DLL no: EXE `alloc_sarray(ci, 1, 99, 2)` contra DLL `alloc_sarray(ci, 1, 99, 4)`). Ahora `max_v` se lee antes de `jround_up` (variable `maxVForAlloc`). Con la correccion el banco queda en cero diferencias. Las 29 funciones restantes no tuvieron diferencias; ninguna se excluyo. Las 30 filas de `docs/hooks.csv` pasan a `validated = 1`.

| Funcion | Entrada | Archivo |
|---|---|---|
| `jinit_post_controller` | 0x5CCDF0 | LibJpegDecompressUpsample.cpp |
| `sep_upsample` | 0x5CCE80 | LibJpegDecompressUpsample.cpp |
| `jinit_upsampler` | 0x5CD350 | LibJpegDecompressUpsample.cpp (corregida) |
| `error_exit` | 0x5CD6B0 | LibJpegError.cpp |
| `output_message` | 0x5CD6D0 | LibJpegError.cpp |
| `jpeg_std_error` | 0x5CD810 | LibJpegError.cpp |
| `alloc_small` | 0x5CEBC0 | LibJpegMemory.cpp |
| `alloc_large` | 0x5CED10 | LibJpegMemory.cpp |
| `alloc_sarray` | 0x5CEDD0 | LibJpegMemory.cpp |
| `alloc_barray` | 0x5CEE80 | LibJpegMemory.cpp |
| `request_virt_sarray` | 0x5CEF30 | LibJpegMemory.cpp |
| `request_virt_barray` | 0x5CEFA0 | LibJpegMemory.cpp |
| `realize_virt_arrays` | 0x5CF010 | LibJpegMemory.cpp |
| `free_pool` | 0x5CF580 | LibJpegMemory.cpp |
| `jinit_memory_mgr` | 0x5CF6D0 | LibJpegMemory.cpp |
| `jpeg_get_small` | 0x5CF800 | LibJpegMemory.cpp |
| `jpeg_free_small` | 0x5CF810 | LibJpegMemory.cpp |
| `jpeg_get_large` | 0x5CF820 | LibJpegMemory.cpp |
| `jpeg_free_large` | 0x5CF830 | LibJpegMemory.cpp |
| `jpeg_mem_available` | 0x5CF840 | LibJpegMemory.cpp |
| `jpeg_open_backing_store` | 0x5CF850 | LibJpegMemory.cpp |
| `jpeg_mem_init` | 0x5CF870 | LibJpegMemory.cpp |
| `jpeg_mem_term` | 0x5CF880 | LibJpegMemory.cpp |
| `jdiv_round_up` | 0x5CF890 | LibJpegMemory.cpp |
| `jround_up` | 0x5CF8A0 | LibJpegMemory.cpp |
| `jcopy_sample_rows` | 0x5CF8C0 | LibJpegMemory.cpp |
| `jzero_far` | 0x5CF940 | LibJpegMemory.cpp |
| `jpeg_CreateCompress` | 0x5CF960 | LibJpegCompressApi.cpp |
| `jpeg_suppress_tables` | 0x5CFA50 | LibJpegCompressApi.cpp |
| `jpeg_finish_compress` | 0x5CFAC0 | LibJpegCompressApi.cpp |

Saltadas en el lote (no estan enganchadas ni se probaron): `do_sarray_io` (0x5CF1B0) y `do_barray_io` (0x5CF250), con el cinfo del arreglo virtual en ESI, y `jpeg_destroy_compress` (0x5CFA30), un salto puro a `jpeg_destroy`. Alcance y exclusiones de los fixtures: `scope_note.txt`.

## Como se prueba cada funcion

Cada funcion se prueba aislada, con casos declarativos (cabecera + escrituras + retornos programados) sobre cuatro arenas con guardas de 0x40; el oraculo Python ejecuta la funcion como un programa sobre su modelo de memoria, en el orden de lecturas y escrituras del EXE. Los colaboradores del propio lote que llama cada funcion por direccion (alloc_small, alloc_large, alloc_sarray, alloc_barray, jpeg_get_small, jpeg_get_large, jpeg_free_small, jpeg_free_large, jpeg_mem_available, jpeg_open_backing_store, jpeg_mem_init, jpeg_mem_term, jinit_memory_mgr, jround_up), los de otros lotes (jpeg_destroy, jpeg_abort) y `CMemoryMgr::Malloc/Free` (0x72F420/0x72F430) son falsos identicos en la direccion del EXE y en el cuerpo de la DLL (el hook queda apagado); `exit`, `fprintf`, `getenv` y `sscanf` del CRT del EXE solo en la direccion del EXE. `exit` y los `error_exit` programados no vuelven (salida no local de Gate), para cubrir los caminos donde el EXE seguiria sobre memoria invalida (bloque nulo de `jinit_memory_mgr`, `jpeg_get_large` nulo, `rowsperchunk <= 0` con filas, pool invalido de `free_pool`). Dos efectos laterales programables por caso sobre la memoria (relecturas de campos entre llamadas a falsos) y retornos programados con BYTE de entrada con basura en los bytes altos (`need_full_buffer`, `pre_zero`, `suppress`). Los falsos por puntero (error_exit, output_message, format_message, alloc_small/alloc_sarray de la tabla `mem`, metodos de sobremuestreo, color_convert, close_backing_store, prepare_for_pass, finish_pass, progress_monitor, compress_data, write_file_trailer, term_destination) son identicos en ambos lados por construccion. La compilacion de la sonda es Debug (sin inlining): los falsos sobre el cuerpo de la DLL de los colaboradores del mismo archivo dependen de ello.

Cobertura de ramas medida sobre las entradas y el modelo (minimo 50 casos por rama, `COVER_FLOOR`; la rama mas rara medida: `alloc_barray` 59, `realize_virt_arrays` 81) y 16 familias por funcion (posicion y relleno de las arenas: alineado, desalineado, al borde y cerca del inicio, con relleno aleatorio, ceros, unos y 0xCD). `kCompareVolatileRegisters = false`: EAX/ECX/EDX tras la llamada solo se registran (331115 pares con residuo distinto, informativo: ningun llamador del EXE lee esos registros salvo EAX, cuyo valor devuelto se compara en las funciones que lo devuelven).

## Prueba negativa (2 rondas, 30 mutaciones por ronda, 2048 casos por funcion)

Las 30 funciones fallan en ambas rondas (lado EXE siempre dentro del oraculo): ronda 1 con 24167 pares con diferencias de 61440 y ronda 2 con 22021 de 61440. Resumenes en `negative-runs/`. Las mutaciones se condicionan a un cinfo (o puntero) desalineado, o a un argumento negativo, que el juego real no produce. Una primera tanda de rondas murio a mitad de la lista por mutaciones mal escogidas (una escribia por un puntero crudo en `jpeg_mem_term`, otra dividia por cero con `samplesperrow` = 0xFFFFFFFF y otra desreferenciaba un `err` basura en `jpeg_open_backing_store`): se cambiaron por mutaciones que no desreferencian (en `jpeg_mem_term`, que no tiene salida observable, la mutacion deja un valor en la pila x87) y las dos rondas se repitieron enteras con 30 funciones. La compilacion final se hizo con las fuentes buenas (SHA256 igual a las copias).

## Reproducir
`CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/Fn235A/chain235.sh` (una sola sesion del candado del juego: corrida principal + 2 rondas negativas; las rondas negativas finales se hicieron con `chain235neg.sh`, `ROUNDS="1 2"`). Humo: `bash reversing/Fn235A/smoke235.sh 512`. Autoprueba sin juego: `python -I reversing/Fn235A/fixture_selfcheck.py`. Diagnostico: `python -I reversing/Fn235A/diag.py <comparisons.jsonl>`.

## Bancos anteriores que citan estas direcciones
Solo como colaboradores o referencias (no se repiten): Fn230A (0x5CF8A0 jround_up y 0x5CFA50 jpeg_suppress_tables), Fn231A y Fn233A (0x5CF890 jdiv_round_up) y Fx43 (0x5CD810 jpeg_std_error, 0x5CF960 jpeg_CreateCompress y 0x5CFAC0 jpeg_finish_compress). `reversing/PendienteValidar/234A.md` cita 0x5CCDF0 y 0x5CD350 (jinit_post_controller y jinit_upsampler como funciones llamadas por el lote 234A).
