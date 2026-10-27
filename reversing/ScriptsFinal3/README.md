<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptsFinal3: banco por pares de CTheScripts (69 funciones)

Banco nuevo, estilo `HudCamFinal2`/`PadMouse1`. Sin compilar ni ejecutar todavia. Comando: `bash reversing/_handoff/bench.sh ScriptsFinal3 3`.
Filtro opcional: `scriptsfinal3-only.txt` en la carpeta del juego con `CTheScripts::<nombre del hook>`.

## Que se compara
Original = hook desactivado (codigo del EXE); candidato = hook activo, con el mismo fixture determinista por caso (16384 pares, 64 a 1024 por funcion; ver la tabla de
`LiveComparison.hpp`). Cada ejecucion devuelve el retorno, el hash de toda la zona de estado de CTheScripts (`0xA43C78..0xA95780`, incluida la tabla estatica de `CColPoint`
de ClearSpaceForMissionEntity en `0xA951F8`), fragmentos hexadecimales de los arrays que cambian y la traza de colaboradores. La zona se restaura tras cada ejecucion.

## Aislamiento
- Pools de peds, edificios, objetos y dummies, pool de TXD y motor RW son falsos (el puntero global se intercambia y se restaura). Peds, vehiculos, objetos, tareas, eventos,
  grupos, matrices y model info son bloques falsos con tablas virtuales falsas (destructor, `GetTaskType`, `SetAnimFile/ConvertAnimFileIndex/GetAnimFileIndex`).
- Colaboradores con efectos reales, doblados con EntryPatch (direccion del EXE + simbolo de la DLL + cuerpo), respetando `ret N`: referencias de entidades, mundo, cerebros de
  script, CFileMgr, flujo de guardado (0x5D1270/0x5D1300, solo en memoria), inicializadores de otras clases, TXD, PlaceMarker (12 argumentos en el EXE, 9 con CRGBA en la DLL),
  marcadores, sombras, dibujo 2D, CRunningScript::Process y similares, tareas/grupos/vehiculos de CleanUpThisPed/RemoveThisPed/ClearSpaceForMissionEntity.
- Load graba antes el flujo con el Save del EXE sobre un estado coherente y lo consume despues; no se abre ningun fichero de guardado.
- Init usa un main.scm sintetico (cadena de chunks valida) servido por la lectura de fichero simulada y un estado previo totalmente aleatorio (comprueba lo que NO se escribe).
- Hilos ajenos: los dobles solo trazan en el hilo del banco (`owner`/`Mine()`).

## Enmascarados y notas
- `DrawWindow`: el quinto argumento (`unused`) es un byte en el EXE y `bool` en la DLL; solo se anota si es distinto de cero (el callee no lo usa).
- El relleno de las estructuras se compara tal cual: la fuente ya no escribe los bytes que el EXE no escribe (tScriptText +0x25..0x27, tScriptEffectSystem +1, etc.).
- Evento `layout`: comprueba offsets de la DLL frente a los del EXE (vehiculo, objeto, checkpoint, colision, RwGlobals +0x20/+0x28, ranuras virtuales, strides de pools).

## Fuera del banco
- Cola de CleanUpThisPed (EXE 0x486522..0x48663F: crea CTaskComplexSequence/LeaveAnyCar/Wander y un CEventScriptCommand con `call 61A5A0`): en la DLL los constructores de tareas son
  en linea y el destructor de CEventScriptCommand (borra la tarea por vtable real) no tiene direccion tomable para doblarlo; con memoria falsa romperia. El fixture solo llega a
  ramas donde la tarea buscada ya existe. La correccion del orden de reserva queda sin validar por el banco.
- RemoveFromVehicleModelsBlockedByScript, DrawDebugCube/AngledCube y RenderTheScriptDebugLines no estan en la lista (sin direccion en el EXE).
- Rama de paquete de misiones de Init (`CheckMissionPackValidMenu`, 0x468E30): requiere menu y ficheros del usuario.

## Aprobación (09/10/2026)

Corrida `20261009-203734-459814`: passed=true, 16384 pares, 69 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_scriptsfinal3.py` (evidencia en `runs/`, `validation-summary.json`). Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.
