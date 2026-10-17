<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de los adaptadores aprobados

Corrida `20261010-202525-458974`. Se descomprimió y leyó íntegramente [comparisons.jsonl.gz](runs/20261010-202525-458974/comparisons.jsonl.gz), sin ejecutar el analizador del banco. Se reconstruyeron directamente los campos, enlaces, listas y efectos desde los registros crudos. No se modificaron fuentes, instrumentos ni evidencia aprobada.

| Función | Casos únicos | Pares | Diferencias | original/candidate nulos |
| --- | ---: | ---: | ---: | ---: |
| CalculateTrianglePlanes-colModel | 1.024 | 1.024 | 0 | 0 |
| RemoveTrianglePlanes-colModel | 1.024 | 1.024 | 0 | 0 |
| Total | 2.048 | 2.048 | 0 | 0 |

Cada función tiene exactamente casos0..1023; los pares globales son exactamente0..2047. Los2.048 pares tienen original/candidate como objetos no nulos iguales, equal=true, accepted=true y state_restored=true. Los diez gates de rutas, ABI, protección, efectos, caché y recursos son verdaderos en ambos lados:40.960 verificaciones. Existe un begin válido, dos resúmenes sin fallos y un complete único con2.048 pares y restauración verdadera. No hay eventos de acceso inválido.

Existen exactamente4.096 eventos raw_allocations, uno por cada lado de cada par, incluso cuando la lista de asignaciones está vacía. Sus7.424 asignaciones observadas en total tienen IDs consecutivos y únicos por lado, direcciones reales no nulas, tamaños10/20/30/40/50, buffers completos y fases de asignación/retirada válidas. Ninguna permanece pendiente de liberar: todas se retiraron durante ejecución o limpieza.

Para cada asignación se recalcularon triangles=size/10−1 y link=(triangles*10+3)&~3. Se reconstruyeron los nueve bytes definidos de todos los planos y los cuatro bytes completos del enlace desde final_raw. Se verificó que el puntero del enlace pertenece a uno de los cuatro enlaces privados y se recalculó su índice. El resultado reconstruido coincide íntegramente con final_allocations, incluidas las asignaciones liberadas por Remove, expulsión LRU y limpieza; después se cotejaron original/candidate. Se comprobaron185.216 bytes definidos y61.824 bytes de padding/trailer. Cada byte indeterminado conserva su propio valor initial_raw; no se le exige coincidir con otro bloque ni se borra para hacer pasar la comparación.

También se cotejó allocations contra IDs/tamaños/fases crudos, su estado live anterior a limpieza y execution_frees contra las retiradas en fase Execute. Los planes todavía vivos observados antes de limpieza coinciden con sus campos definidos finales. Calculate sólo asigna durante ejecución en perfiles2/3/6; la expulsión en6 y las retiradas Remove3..7 liberan exactamente el recurso esperado. La comparación conserva los campos de recursos ya liberados: no desaparecen de la igualdad al liberar el bloque.

La caché se verificó independientemente desde cache_raw y links_raw. Se obtuvo la base del array de enlaces del campo links real, y se recorrieron used/free por sus punteros, comprobando ambos sentidos, límites, ausencia de ciclos y que los cuatro enlaces aparecen exactamente una vez. Los punteros data de la lista usada corresponden a los cinco datos privados. Los órdenes reconstruidos coinciden con las listas registradas y con los órdenes esperados de los ocho perfiles de cada función. No se usó cache_consistent como sustituto de esa reconstrucción.

Las siete entradas compartidas reales son416330/416400/40F590/40F6A0/40F6C0/40F6E0/411660. begin exige RH registrados y colaboradores/allocator sin stubs; cada lado tiene helper_route_ok=true. El HPP y HelperHookRoute.hpp cuyos hashes se comprobaron exigen esos siete hooks ON/locked, que entrada y símbolo resuelvan al mismo cuerpo del ASI y que los bytes permanezcan intactos antes/después. Los registros de este banco publican esos gates y entradas; esta auditoría no afirma haber vuelto a leer memoria del proceso cerrado ni disponer de direcciones de cuerpos que no se archivaron en JSON.

La ABI publicada acredita ESP y EBX/ESI/EDI/EBP preservados para los adaptadores cdecl. Las instrucciones de sus llamadores documentadas en dis-callers.txt no consumen un retorno semántico ni requieren registros volátiles o flags preservados. allocator_route_ok y allocation_balance_restored son verdaderos en todos los lados, comprobando reenvío al asignador original completo, bytes restaurados y retirada balanceada de recursos propios.

begin contiene allocator_argument_count=1. No existe campo hint en ninguna observación de la corrida aprobada:72F420 sólo lee[ESP+4] y reenvía size. La corrida anterior que interpretaba[ESP+8] como hint no se reclasificó retrospectivamente como aprobada; el banco corregido fue recompilado y ejecutado completo.

| Evidencia | SHA-256 comprobado |
| --- | --- |
| Registro descomprimido | `91ea740f08b68b530c82d86e3bbf3aec6922a7eb3ba9bbe126742f2f4413162d` |
| Archivo gzip | `dfd43b0a56b87205be94973e5ab8f1b37bbd6852c51e755ec3e6c2d454016b59` |
| ASI conservado en artifacts | `63cd3f02f7874168a14ceefd1f74d0882e8bc47a0772b1e4ce8eb4d26fefda99` |
| EXE canónico | `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b` |
| AllocationTrace.hpp | `f57e4cd98980cb1784b49e1c93686c3cf514ac8f61a943d57b9587ecdbdb9ed5` |
| HelperHookRoute.hpp | `db858ad185fe0a6436da2e4bce090431dff811f3427605d856233f6be67ec0e1` |
| LiveComparison.hpp | `3c7adec50500ee2dc67760c8b2495c98a31cd12b9083b17eeb5789175204588d` |
| build_probe.py | `e802273c0a09b40bd4aa9708f8742ce494e54d03003e498c61375c60f6e18654` |
| run_local.py | `855eaed9023c1467254c21349f241193ffaed866d25194864afe090f0031d978` |

Se verificaron físicamente ASI, EXE e instrumentos; sus hashes coinciden con [approval-audit.json](runs/20261010-202525-458974/approval-audit.json), manifiesto y [summary.json](runs/20261010-202525-458974/summary.json). Las22 fuentes del manifiesto coinciden con la aprobación y con sus archivos actuales al revisar. El resumen archivado acredita partidas intactas y configuraciones restauradas; esta revisión no ejecutó nuevamente el juego ni el runner.

Alcance de restitución: caché real, estructuras, mallas, enlaces, bytes/rutas de hooks y control x87 observados, además de recursos propios balanceados. No se afirma restauración de metadatos internos del CRT/heap ni del estado x87 completo de108 bytes, que este banco no captura. Los colaboradores reconstruidos permanecen compartidos; la aprobación de los adaptadores no equivale a una nueva aprobación independiente de cada colaborador, ni cubre fallos de asignación, administrador alternativo o concurrencia.
