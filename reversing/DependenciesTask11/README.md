<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesTask11 — dos entradas aprobadas y registradas

Estado: **aprobado y registrado**, corrida `20261010-213032-855203`: dos funciones, **4096 pares (2048 por función), cero diferencias y cero registros pasantes con objetos nulos**. Restauración final, rutas RH de producción OFF/ON, partidas intactas y configuraciones restauradas verificadas. La propuesta se aplicó por la raíz; los scripts no la aplican.

| Entrada del EXE | Identidad | Pares admitidos |
| --- | --- | --- |
| 0x4684F0 | CDecisionMakerTypes::GetInstance | 2048 |
| 0x632140 | función C++ global CanSunbathe, owner RH CTaskComplexSunbathe | 2048 |

CanSunbathe es una función global real: el owner RH identifica su inyector y no inventa una clase C++ ni un grupo nuevo del catálogo. Population.cpp contiene el caller original 0x632140; TaskComplexSunbathe.cpp ya contiene su registro global comentado. El registro nuevo conserva el scope de CTaskComplexSunbathe, dentro de un bloque local.

## Fuente y ABI

`source-proposal.patch` cambia solamente DecisionMakerTypes.cpp y TaskComplexSunbathe.cpp. GetInstance transcribe las 29 instrucciones completas de 0x4684F0..0x46855C, incluidos ambos RET, el handler SEH original 0x837D9B, la lectura/escritura del singleton 0xC0B030 y las llamadas reales a operator new 0x82119A y al constructor completo 0x4650F0. Los puentes PUSH dirección/RET conservan los argumentos y la convención. La ruta normal compara ESP, EBX/ESI/EDI/EBP y FS:[0]. No certifica desenrollado por excepción, ni provoca agotamiento de memoria. operator new original reintenta/lanza en fallo; la rama defensiva de retorno NULL se conserva pero no se fuerza con mocks.

CanSunbathe transcribe las 27 instrucciones completas de 0x632140..0x63218C, con ambos RET. Lee hora 0xB70153 y clima int16 0xC8131C; admite horas [10,18), climas directos {1,3,14,5,10,18} y el helper completo 0x62F2D0 para {0,2,13,6,11,17}. Devuelve EAX entero 0/1 y conserva ECX/EDX. El banco inicializa EAX/ECX/EDX igual en ambos lados y captura los registros inmediatamente en ensamblador, además de no volátiles, ESP y SEH.

## Fixture real y observación completa

Las llamadas usan siempre la misma entrada EXE con RH de producción OFF/ON y registro obligatorio. No hay stubs de allocators, constructor ni clima. GetInstance requiere que el juego ya tenga singleton no NULL: preserva una copia de sus 61888 bytes y verifica que no se modifica en ningún lado. La rama existente devuelve exactamente ese objeto; la rama NULL invoca operator new y constructor completos del EXE, observa el objeto nuevo, lo libera una sola vez mediante delete real 0x8214BD y restaura el singleton y los 80 bytes de globals antes de cualquier bombeo de mensajes.

El constructor 0x4650F0 tiene 415 bytes hasta RET incluido, verificados completos en ejecución antes/después/final. Inicializa 25 matrices de 41 decisiones (1025 decisiones de 60 bytes: seis tareas -1, 24 probabilidades BYTE cero y 12 booleanos cero), más m_NoOfDecisionMakers=0: se comprueban directamente los **61504 bytes definidos**, sin reducir la prueba a un hash ni copiar outputs originales como expected. Los 384 bytes de m_EventIndices no los escribe el constructor original y se excluyen de igualdad entre reservas nuevas. No se presenta ese padding/estado indefinido como inicializado. Se comprueban los 80 bytes de flags e índices globales contra el patrón del EXE (20 flags cero, 20 índices uint16=1, 20 flags cero).

Los 2048 casos GetInstance alternan singleton real/NULL, cuatro semillas EAX y bytes globales de entrada variados; son pruebas repetidas del contrato completo y no 2048 ramas distintas. CanSunbathe cruza 16 horas, 32 climas (incluidos negativos, límites int16 y valores fuera del enum) y cuatro palabras altas EAX: 2048 perfiles distintos. Solo se instalan fixtures al ejecutar cada llamada y todo se restaura antes de escribir registros o procesar mensajes.

Diez colaboradores conservan sus rutas reales actuales (entrada, cuerpo resuelto, módulo, bytes y flags RH) antes/después/final. SetDefault puede estar reconstruido/activo en ambos lados; esto se declara explícitamente y no certifica independencia transitiva del EXE. El helper meteorológico de 49 bytes y el constructor entero se cotejan contra NativeProof.hpp. Se guarda/restaura el estado x87 completo de 108 bytes y MXCSR. Los contadores de asignación/liberación prueban vidas lógicas del objeto nuevo: no certifican restauración de metadatos internos del heap.

## Ejecución y rechazo

La raíz aplicó/revisó la propuesta y ejecutó serialmente el build y runner del banco. build_probe.py valida que cada ruta del manifiesto exista. run_local.py exige exactamente 4096 pares únicos, dos objetos no nulos por registro, igualdad estricta, todos los gates ciertos, 2048 perfiles de clima, ambas ramas singleton y restauración final. No modifica runtime_runner.py, documentos congelados ni el juego personal. La sesión de prueba no guarda partidas.

## Evidencia admitida

[Comparaciones completas](runs/20261010-213032-855203/comparisons.jsonl.gz), [admisión](runs/20261010-213032-855203/approval-audit.json), [resumen](runs/20261010-213032-855203/summary.json) y [auditoría independiente](audit-runtime.md).

Log descomprimido SHA-256: `9bc789a856a699e8f8c17c13ea9abb46851850e05844e573127de809b042f2b5`.
ASI ejecutada SHA-256: `bbaa8052ba3bc66e05dd33fc1b06e0e871b7bcb8102f2a438f13417618e4f0ba`.
Los cuatro instrumentos de prueba conservan sus huellas admitidas. Los límites de SEH normal, m_EventIndices indefinido y colaboradores compartidos siguen vigentes tras aprobar.
