<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesWorldWaterPath6 — aprobada y registrada

Cuatro destinos de la cohorte original aprobados y registrados mediante el run **20261010-202741-158472**: **24576 pares, cero diferencias y cero registros nulos**, estado restaurado, partidas sin cambios y configuraciones restauradas. Instrumentación ampliada ejecutada con todos los contratos de cobertura verdaderos. No volver a ejecutar prepare.py ni reaplicar la propuesta histórica.

| Dirección | Alias descriptivo reconstruido | Pares previstos |
|---|---|---:|
| 5B2090 | CCam::CameraPathInterpolateVector (global bajo categoría CCam) | 4096 |
| 5B2330 | CCam::CameraPathInterpolateScalar (global bajo categoría CCam) | 4096 |
| 6E5BB0 | CWaterLevel::GetWaterLevelNoWavesForQuad | 8192 |
| 6E5E90 | CWaterLevel::GetWaterLevelNoWavesForTriangle | 8192 |

Total por ejecución: **24576 pares**. Estos nombres explican el contrato; no se afirma que sean nombres recuperados del EXE.

La propuesta reemplaza las cuatro llamadas delegadas de Process_FlyBy y las dos de GetWaterLevelNoWaves. Las hojas de agua no contienen CALL: atlas real más x87. Las hojas de caminos sólo llaman al CRT 821B40 con argumento ST0; el banco verifica que la biblioteca pertenece al módulo EXE. Cada función tiene hook RH independiente; original apagado y candidata encendida sobre la misma entrada. No se necesita activar familias ni añadir entradas a InjectHooksMain porque CCam/CWaterLevel ya se inyectan.

Evidencia en disassembly.txt con dirección e instrucción completas: vector RET5B2321, escalar RET5B2476 y RET5B249A, quad RET6E5E85, triangle RET6E619D/6E61A7. Todos los epílogos son explícitos. La transcripción conserva ds: y emite únicamente etiquetas que reciben saltos.

Los caminos usan el contrato confirmado por CCamera::LoadPathSplines5B24D0: un float de cantidad y nodos de 4 floats para roll/FOV o 10 para source/target, con tiempo, extremo y dos controles Bézier. Cantidades2..16, tiempos crecientes, índices válidos, ocho combinaciones de controles vectoriales, controles escalares iguales/distintos, antes/durante/después del intervalo, límites adyacentes y tiempo posterior al último nodo; traslación global cero/no cero. No son arrays con formato supuesto.

El agua lee exclusivamente las tablas y vértices **cargados por el juego completo**. Sus cantidades y strides se verifican; no se inyecta un atlas fabricado ni se cambian sus vértices. El banco toma una copia privada de un polígono real y varía solamente el bit documentado de profundidad limitada. El atlas original entero queda bajo comprobación de sólo lectura. Si faltan polígonos no degenerados o alguna orientación triangular real, termina con error; nunca aprueba listas vacías.

Fixtures de agua: interior y exterior, bordes exactos/adyacentes, ambas mitades del quad, ambas orientaciones triangulares, altura hasta ±2000 respecto de un vértice real, profundidad limitada sí/no y las tres combinaciones válidas de salidas opcionales. En el EXE big!=NULL exige small!=NULL. Cuando big==NULL el quad termina antes de comprobar profundidad; el triángulo todavía hace la comprobación. Se conservan estos comportamientos y los outputs escritos incluso al devolver false.

Compara bool AL, outputs por bits, índice, ESP y EBX/ESI/EDI/EBP, guardas, entradas y atlas, ruta RH y CRT, pila/control x87. Guarda/restaura atlas, cantidades, traslación, FNSAVE/FRSTOR y MXCSR antes de cada Write y al final. Original/candidate contienen objetos completos incluso en rechazo. Resúmenes exigen outputs escritos, ambas rutas geométricas aceptadas y devoluciones true/false; las ramas cortas no bastan para aprobar.

Archivos Python y manifest leen explícitamente UTF-8; backups, cachés y artifacts fuera del alcance. No toca runtime_runner.py ni documentos congelados.

La ampliación usa intervalos internos de 16, 32, 33 y 750 ms y un último tramo de 750 ms. Los índices iniciales se calculan como Process_FlyBy; el preflight comprueba todas las lecturas contra la longitud declarada, incluido el último tiempo. Hay canario inmediato después de esa longitud y comprobación de sólo lectura del almacenamiento entero. Un fixture inválido no ejecuta la función y no puede pasar. La simulación con operaciones float32 produce, por helper, 520 casos inferiores a 32, 216 iguales y 3360 superiores, con 736 avances y cero rangos ilegales. El resumen exige las tres clases y avances observados en el EXE.

Agua incorpora igualdad y vecinos representables de -6 y +20, sobre polígonos reales cargados cuyos vértices tienen altura cero. No fabrica vértices. Exige observar aceptación exacta, rechazo inferior con profundidad limitada, rechazo superior y la diferencia quad/triángulo con big=NULL. La revisión está en [audit-static.md](audit-static.md). La ejecución aprobada confirma esta cobertura ampliada; véase [audit-runtime.md](audit-runtime.md).

Huellas de evidencia: log `ff7a5756fef912bff337370db1b4585e3d997981b28e7c43439383984f325373`; ASI `e88cc70c414f1e2278023180f041c62bd303c491babb5e77da513195e7ee6936`. El alcance corresponde a los fixtures del banco y no demuestra integración universal ni independencia transitiva del EXE.
