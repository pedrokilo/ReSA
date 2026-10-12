<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de StatsUi8

Corrida `20261010-203949-345184`. Se leyó íntegramente el gzip archivado mediante una comprobación independiente, sin ejecutar el analizador del banco, compilar o ejecutar el juego.

## Registro y rutas

8192 comparaciones con identificadores únicos exactamente 0..8191. Las cuatro funciones contienen 2048 casos únicos exactamente 0..2047. Cero resultados original/candidate nulos, cero desigualdades de objetos completos y cero equal/accepted falsos. Ambos lados cumplen fixture_contract, floating_state_restored, globals_restored, helper_route_ok, nonvolatile_preserved, return_nonnull, route_ok y stack_restored.

Un comienzo y un cierre únicos acreditan hooks registrados, restauración flotante y rutas correctas. Los cuatro resúmenes indican 2048 casos y cero fallos. Los nueve colaboradores tienen route_ok=true: 0x53FB70 y 0x821B40 permanecen originales sin RH; los siete restantes están registrados, reconstruidos y habilitados. La lectura de HelperRoutes confirma comprobación de flags/código y resolución al módulo esperado, conservando ese modo compartido. Las rutas originales de colaboradores no equivalen a independencia transitiva del EXE.

## Cobertura comprobada

El registro de rating contiene efectivamente vuelo 0/36000000/39600000 ms dentro de 375000..499999, dinero 9999999/10000000/10000001 para rating ≥1000000 y arrestos 0/1001 dentro de 1150..1299. Sus retornos no nulos, textos cargados y contrato de rating pasan en todos los pares.

Los 2048 identificadores completos de cada función de salud recorren, conforme al fixture leído, los diez contadores incluidos límites signed/unsigned, nueve umbrales incluidos NaN/infinito, seis valores de salud y tres incrementos. No constituyen todo el producto cartesiano. Se comparan los bloques crudos completos y el argumento final de AddToHealth: se observaron 1251 estados de salida distintos en Respawn y 1012 en AddToHealth.

Los 2048 casos de mensajes recorren los siete contadores 0/1/2/5/127/128/129 y las ocho condiciones iniciales mediante los selectores documentados; preparan flags displayed, tipos y umbrales por entrada, incluidos unordered. Los 2048 estados de salida son distintos. La clave real inicial es SBOFAT0. El registro no serializa por separado todos los inputs de salud/mensajes: esa cobertura se deriva de los identificadores completos y del fixture cuyo hash coincide con la compilación, no de contadores de ramas dinámicos.

## Integridad y restauración

SHA-256 calculado del JSONL descomprimido: `36580e799fa2346a4a9a8186e6f85a0266eb50fa4cd1f1f453a0df1aa4e792c8`. SHA-256 del ASI disponible en artifacts: `acfc66e52cb4a0e0d1d18b0fad5f7a5a71da34bed33e37d5d6178900c6f55ca1`. Coinciden con aprobación, resumen y manifest. Las 36 fuentes del manifest coinciden con aprobación y archivos actuales al revisar; los cuatro instrumentos coinciden también.

Se observan/restauran estadísticas B78200..B79517, ayuda BAA460..BAA92F, gString/gString2, PlayerInfo completo y los flags/búferes enumerados por el banco. Cada lado restaura y comprueba esos bloques, x87 de 108 bytes y MXCSR. El cierre incorpora la restauración de las cuatro rutas y checks de colaboradores. Esto cubre los snapshots declarados, no todos los globales o el estado interno completo del juego.

El resumen declara partidas sin cambios y configuraciones restauradas; contiene huellas iniciales de tres partidas y dos configuraciones, pero no huellas posteriores individuales. Estas propiedades se apoyan en la comprobación del runner, sin volver a medir archivos vivos durante otra corrida. Tampoco se recalculó aquí el EXE vivo.

La reparación de las etiquetas sin referencias es posterior a la revisión estática inicial y anterior a esta compilación aprobada. No se encontró discrepancia en el registro completo ni un pase nulo; la aprobación permanece limitada a estas cuatro entradas y sus colaboradores compartidos.
