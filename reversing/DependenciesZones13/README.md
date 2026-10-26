<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesZones13 — banco completo aprobado

Estado: **APROBADO y registrado** en [20261010-221516-870556](runs/20261010-221516-870556/approval-audit.json): **1 función /2048 pares, cero diferencias y cero registros original/candidate NULL**, restauración final de estado/rutas/configuraciones y partidas verificada. Fuente aplicada y compilada por la raíz después de revisar el EXE completo; los cuatro instrumentos aprobados permanecen inmutables.

Evidencia: [comparaciones gzip](runs/20261010-221516-870556/comparisons.jsonl.gz), [resultado](runs/20261010-221516-870556/summary.json), [auditoría independiente](audit-runtime.md). Log SHA256 descomprimido `e0ec4d10cccbebb03f5864d7553d5430618bea7f67312ea00386886abe85aae2`; ASI ejecutado `2cc37b2fd22993871e88609f947ae728b1a3732ee9ce1e15cbea5d7160b8c7c6`.

## Fuente y ABI

Las 46 instrucciones hasta RET0x572899 están transcritas en source-proposal.patch, exclusivamente TheZones.cpp. Main ya registra CTheZones y la firma existe. Cuatro puentes PUSH dirección/RET llaman los colaboradores reales 0x56E010/0x572300/0x572130/0x821B40; no simulan sus resultados. La función es cdecl void: el gate inicializa EAX/ECX/EDX iguales y captura inmediatamente ESP, EBX/ESI/EDI/EBP y FS:[0], sin atribuir un retorno semántico a registros volátiles.

Siempre invoca 0x572800 con RH producción OFF/ON, exige registro y restaura modo/lock/bytes iniciales al terminar. La restauración RH se llama incondicionalmente, incluso si memoria o colaboradores fallan.

## 2048 perfiles, todas las celdas y salidas

16 coordenadas X × 16 coordenadas Y × off-map2 × visited2 × exterior/interior2 = 2048 perfiles únicos. Diez centros de celda y seis extremos/bordes internos cubren las **100 celdas** del grid. Todos los valores son finitos dentro[-3000,3000), incluyendo-3000 y2999; se excluyen+3000/NaN/infinito y otros valores que producirían acceso fuera del grid del EXE. Z=0. No se presenta un dominio inseguro como probado.

El actor es el jugador real seleccionado en el array nativo, a pie. Se comprueban ID/puntero/flag nativo antes del banco y se mantiene su matriz real si existe: no se coloca una matriz ficticia ni se sustituye el helper. Sólo cambian temporalmente sus 12 bytes de posición. Antes/después de cada invocación se comparan todos los bytes del jugador y de CMatrixLink frente al snapshot del fixture; después se devuelve posición original y se verifica snapshot original completo. Se restaura antes de cualquier PumpMessages/escritura de logs. No se guardan partidas.

El grid parte de un patrón completo de100 bytes booleanos, target=visited y count=popcount coherente. off-map usa TheScripts::bPlayerIsOffTheMap0xA444A4 y area usa CGame::currArea0xB72914 (0/1). CurrentLevel se debe actualizar SIEMPRE, incluso cuando una condición evita desbloquear. Sólo off-map=false, area0 y visited=false pone la celda a1 e incrementa contador en1.

## Observación completa y oracle independiente

Se comparan **grid100 bytes completo, counter DWORD y level DWORD**; no basta un contador final para aprobar. La posición observada se obtiene también invocando la ruta real actual FindPlayerCoors con su ABI de resultado por puntero y jugador-1, y se comprueba contra los bytes de la posición del actor. Actor, matriz, catálogo real de39 zonas y contador de zonas permanecen intactos.

El nivel esperado se calcula independientemente recorriendo las zonas reales1..count-1 y comprobando límites int16 inclusivos XYZ; si ninguna contiene el punto, se usa el nivel de zona0. No se llama a GetLevelFromPosition para fabricar expected. El runner repite este oracle sobre los1248 bytes de mapa capturados en begin y verifica todos los pares. La celda esperada se deriva de un índice conocido por coordenada, no de duplicar _ftol; verifica todas100 celdas,8 condiciones de salida y2048 casos únicos. También recalcula patrón/contador completo desde case.

Guardas de16 bytes antes de counter/grid y después de grid, más4 bytes a ambos lados del level DWORD, detectan escrituras vecinas. Se restituyen grid/counter/level/off-map/area/posición y se exige estado completo original, sin reescribir jugador entero o catálogo para ocultar efectos. No hay normalización de payload ni de diferencias.

## Colaboradores reales y restitución

Cinco entradas se observan con cuerpo resuelto, módulo, bytes y flags RH antes/después/final: FindPlayerCoors, GetLevelFromPosition, PointLiesWithinZone, GetCurrentZoneLockedOrUnlocked y _ftol. Los tres primeros pueden estar reconstruidos/activos en ambos lados; no se reclama independencia transitiva. Se incluyen World.cpp/World_Helper.cpp y demás fuentes reales en el manifiesto.

GetCurrentZoneLockedOrUnlocked nativo completo69 bytes y _ftol completo117 bytes se cotejan contra el EXE original hasta RET, mediante NativeProof.hpp y gates dinámicos antes/después/final. FindPlayerCoors completo124 bytes se coteja sólo cuando su ruta inicial es nativa; si RH activo, begin declara player_coords_native_code_checked=false y mantiene/comprueba su ruta real actual, además de observar su lectura efectiva del actor. No se apaga un colaborador o se sustituye con un stub para facilitar el resultado. Estado x87 completo108 bytes y MXCSR se preserva/restaura por lado y al final.

build_probe valida que existan las20 rutas del manifiesto; cuatro instrumentos archivables (dos HPP y dos PY) quedan estables para la raíz. run_local rechaza registro ausente, observaciones nulas, diferencias, gates falsos, grid truncado, oracle incorrecto, celdas/ramas incompletas y restauración final ausente. [Revisión previa](review.md) documenta dirección/instrucciones. Sin cambios en runtime_runner.py, documentos congelados ni juego personal.

Si cualquier gate inicial de registro, colaboradores, cuerpos nativos o restitución falla después de begin, se restauran los hooks incondicionalmente y se emite error con cero pares y estado de restitución real. Nunca se ejecuta el loop sobre un fixture no admitido por preflight.
