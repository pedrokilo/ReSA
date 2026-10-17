<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de la corrida aprobada

Corrida `20261010-202154-353545`. Se descomprimió y leyó íntegramente [comparisons.jsonl.gz](runs/20261010-202154-353545/comparisons.jsonl.gz), sin ejecutar el analizador del banco ni reutilizar su conclusión como sustituto de la revisión por registros. No se modificaron instrumentos, fuentes, evidencia archivada ni registros de admisión.

| Función | Casos únicos | Pares | Diferencias | original/candidate nulos |
| --- | ---: | ---: | ---: | ---: |
| CDecisionMakerTypes::MakeDecision-Ped | 2.048 | 2.048 | 0 | 0 |
| CDecisionMakerTypes::MakeDecision-Group | 2.048 | 2.048 | 0 | 0 |
| CPedPool::GetRef | 2.048 | 2.048 | 0 | 0 |
| CEventScriptCommand::Destructor | 2.048 | 2.048 | 0 | 0 |
| Total | 8.192 | 8.192 | 0 | 0 |

Cada función contiene exactamente casos0..2047; los pares globales son exactamente0..8191, sin duplicados ni huecos. Los8.192 original/candidate son objetos completos iguales, con equal y accepted verdaderos. Se verificaron individualmente ocho gates en ambos lados de cada par: route_ok, helper_route_ok, stack_restored, nonvolatile_preserved, seh_restored, contract_ok, globals_restored y fpu_restored. Son65.536 controles verdaderos por lado,131.072 en total. Los deltas EBX/ESI/EDI/EBP son siempre[0,0,0,0]. No existen eventos de acceso inválido. Los cuatro function_summary tienen2.048 casos y0 fallos, y hay un único complete con8.192 pares y state_restored=true.

begin confirma los cuatro RH registrados, ausencia de colaboradores stub y que no se probó exception unwind. Sus12 rutas contienen entradas y cuerpos resueltos con16 bytes. Las rutas6040D0/606060/821B1E/827B3D resuelven a sus mismas entradas EXE; se conserva el cuerpo original. Se recalculó el destino relativo del E8 de rand y es exactamente827B3D. Las rutas EXE61A5A0/61A5B0 y las direcciones DLL directas de operator new/delete resuelven, respectivamente, a los mismos cuerpos. Cada pareja exige sus gates antes/después y al finalizar, conforme al HPP cuyo hash se verificó.

GetRef: se recalculó de forma independiente el resultado esperado para los2.048 casos: slot=case%7, bitmap=(case+slot*13)&255, retorno=(slot<<8)+bitmap. Los siete bytes de bitmap se cotejaron contra su fórmula completa, incluyendo todos los bits. Coinciden todos los resultados; el contrato del banco exige además cabecera y almacenamiento intactos.

Destructor:1.024 casos con StandStill real y1.024 con tarea nula, exactamente según la paridad de caso. Todos terminan con vptr de evento85AB38. En los casos con tarea, el bitmap queda[81,80,80,80] hexadecimal, preservando el ID del slot0 y marcándolo libre; los nulos conservan[80,80,80,80]. Se observó la misma ruta virtual auténtica en los1.024 casos con tarea: entrada703BB532→cuerpo705EA150 en esta ejecución. No se normalizaron ni sustituyeron sus bytes. Los registros preservan objetos, almacenamiento completo y cabecera del pool; contract_ok confirma la comprobación adicional de cursor, padding y campos intactos fuera del vptr de la tarea destruida.

Se verificaron los hashes, no sólo su presencia en el JSON:

| Evidencia | SHA-256 comprobado |
| --- | --- |
| Registro descomprimido | `d8409dc9d049fa7591508e99a80cca7765441fe7c9cf16ba098207ab83c7c602` |
| Archivo gzip | `ac70aab2961002b122becc884b2e1fe041c6fc7a0f61f28d3962c2d3fbfbe1c3` |
| ASI conservado en artifacts | `bc145be762e287f67ddbea34b2c667bb9114dd55d39f23e5728a5c61d430230a` |
| EXE canónico original | `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b` |
| LiveComparison.hpp | `77824787a9784fa42883c782b633c233eea3d44c5941d4350a2f20e3973cb47c` |
| build_probe.py | `a873e8e9504179fd5542409473d88137b64c440032df2b04600e071aa7026a19` |
| run_local.py | `7e5ddb05dc1fac1738d44092d908ef9f0a9215a25fd5dbbebc5db09a7bfa9bcd` |

Los33 tested_sources de [approval-audit.json](runs/20261010-202154-353545/approval-audit.json) coinciden íntegramente con el manifiesto de construcción; los33 archivos actuales también conservaban esos hashes al realizar esta revisión. Hash del ASI coincide con manifiesto, resumen y archivo físico. Hash del registro coincide con resumen y aprobación. Los tres instrumentos aprobados permanecen intactos.

El [summary.json](runs/20261010-202154-353545/summary.json) registra passed=true, state_restored=true, saves_unchanged=true y configs_restored=true, además de hashes previos de partidas/configuraciones. La auditoría independiente del gzip confirma las restituciones de memoria/rutas/FPU por llamada; la verificación de archivos de partidas y configuración procede de la ejecución completa del runner archivada. No se afirma una segunda ejecución del juego durante esta auditoría.

Alcance: dispatchers completos, referencia de puntero válido del pool y destructor con tarea nula/StandStill real. Colaboradores compartidos reales, sin certificar por transitividad sus otras funciones o clases. SEH sólo por camino normal, con handler83A018 original; no se certifica exception unwind. No se guardó la sesión ni se insertaron los objetos de prueba en el mundo.
