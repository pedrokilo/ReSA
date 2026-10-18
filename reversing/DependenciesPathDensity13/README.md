<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesPathDensity13 — CalcRoadDensity

Fuente aplicada tras revisión completa del EXE. El banco está en compilación y ejecución en serie; todavía no tiene aprobación ni registro en el catálogo.

## Identidad y reconstrucción

`CPathFind::CalcRoadDensity(float,float)`, entrada `0x44EFC0`, cuerpo completo hasta `0x44F186: ret 8`. `CalcRoadDensity.dis.txt` conserva las 130 instrucciones originales; el parche conserva siete etiquetas de salto y convierte las demás direcciones en comentarios para evitar C4102 con /WX. CPathFind::InjectHooks ya está invocado: se activa únicamente el registro existente de esta función. No hay llamadas a colaboradores originales ni stubs.

El cuerpo recorre 64 áreas mediante el arreglo de punteros de `this+0x804` y el contador con signo `this+0x10C4`. En `0x44F004` empieza la evaluación de cada nodo: posición comprimida de dos int16 multiplicada por `0x858C48` (0.125), comparación estricta contra `0x858A14` (80). Los enlaces usan el nibble inferior del byte +0x18, el índice base +0x10, nodeLinks +0xA44 y naviLinks +0xDA4. Las longitudes salen de fsqrt; los dos contadores de carriles del byte +0xB se acumulan por separado. El resultado termina con `0x44F179: fmul [0x858FC0]` (float32 0.0004) y retorno ST0.

`Caller.dis.txt` muestra el consumidor real `0x57024D: call 0x44EFC0`, seguido de `0x570252: fstp dword [esi+0x134]`. Hay dos argumentos físicos de cuatro bytes, cleanup del callee y this en ECX. El banco compara EAX/ECX/EDX crudos, ESP, los cuatro registros no volátiles, las palabras físicas de argumentos, el retorno float32 y los diez bytes del retorno x87; no normaliza esos resultados.

## Dominio del banco

2,048 pares: 64 perfiles por 32 valores. Es un grafo sintético de memoria con layout completo CPathFind 0x3C80, CPathNode 0x1C y CCarPathLink 0xE; no se afirma que los nodos hayan sido cargados desde un archivo de caminos real. No consume objetos virtuales, pools ni colaboradores. Siete bloques guardados con canarios abarcan el objeto, tres nodos fuente, dieciséis destinos, marcador de área, quince enlaces de navegación, 45 nodeLinks y 45 naviLinks. Se serializan y comparan todos sus bytes, incluido padding y guardas. El estado real de ThePaths se observa íntegro y no se modifica.

El corpus cubre las 64 áreas de origen (32 casos cada una), destinos ordinarios y áreas 64–71, fuente ausente, contador cero/negativo, enlaces 0/1/2/3/15, destino ausente, navegación ausente, destino de la misma área, longitudes cero/positivas y todos los valores de carriles 0–7. Incluye int16 extremos y wrap, límites x/y por debajo/igual/encima de 80, signos de los límites, NaN en x e infinito en y. Son perfiles combinados, no un producto cartesiano. No promete observar cada instrucción con tracing.

El oráculo independiente usa los inputs serializados, orden de acumulación y redondeo PC24 por operación; además reconstruye los siete bloques completos mediante sus direcciones declaradas. La comprobación offline de los 2,048 inputs satisface todos los dominios requeridos: 456 resultados positivos, 1,592 cero, 352 casos con múltiples contribuciones; x igual 240, x unordered 104, y igual 288. Estos recuentos prueban el generador y el oráculo, no una ejecución del EXE.

## Rutas y restauración

Ambos lados llaman la misma entrada EXE: RH OFF exige ruta original y RH ON exige E9 al cuerpo DLL del mismo módulo ASI. Se conserva y restaura modo Enabled/Locked y bytes de entrada/cuerpo/ILT mediante HookRoute, comprobando los bytes iniciales al terminar. Cada lado restaura los siete bloques y FNSAVE/FRSTOR de 108 bytes más MXCSR; se captura PC24/nearest para reproducir el dominio numérico. Ningún registro pasante permite original/candidate nulos.

Los snapshots completos de ThePaths real se guardan en eventos `actual_path_initial` y `actual_path_final`, exigidos únicos e idénticos por el analizador. `complete` es compacto, para que el límite de detección de 4,096 bytes del runner no impida reconocer la terminación. Protocolo de inicio idéntico al banco Stats11 reparado: marker `dependenciespathdensity13-runtime.start`, log `dependenciespathdensity13-runtime.jsonl`, espera de jugador/cutscene y captura de fallos con prefijo string.

## Comprobaciones previas

AST de ambos scripts correcto; todos los once paths del manifiesto existen; `git apply --check` acepta el parche LF con contexto actual. La revisión offline verifica tamaño de todos los snapshots, 2,048 fixtures y retorno extendido finito. La prueba final original/RH OFF contra reconstrucción/RH ON dentro del juego completo pasó con restauración final y evidencia archivada.


## Evidencia final

Corrida aprobada [20261010-221026-884852](runs/20261010-221026-884852/summary.json): una función, 2.048 pares, cero diferencias, cero lados nulos, rutas/ABI/FP correctos y ThePaths real íntegro. Partidas sin cambios y configuración restaurada según el runner. [Comparaciones completas](runs/20261010-221026-884852/comparisons.jsonl.gz), [admisión y hashes](runs/20261010-221026-884852/approval-audit.json), [auditoría independiente](audit-runtime.md).

SHA256 del JSONL descomprimido: `940d436c34f116aa13293227867476a7989b7a4503e7331e347b4954e2816d23`. ASI ejecutada: `8e7a9e6bb1b94b8090fe0e5bb0d63d7cfeb2ea3b547bc98364d22584987c567c`. El análisis independiente confirmó los 2.048 casos/par únicos, todos los bytes de siete fixtures, ambos retornos float32/80bits, argumentos físicos, gates y dominios exigidos. Los 15.488 bytes reales de ThePaths inicial/final son idénticos. Los tres instrumentos finales están congelados.
