<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TaskFinal2: funciones restantes de CTaskSimpleFightingControl y CTaskComplexProstituteSolicit

Banco simple (estilo Small2/PadMouse1, juego completo con partida cargada). Sin compilar ni ejecutar todavia.

Uso: `bash reversing/_handoff/bench.sh TaskFinal2 3`. Filtro opcional: `taskfinal2-only.txt` en la carpeta del juego con `Clase::Funcion`.

## Funciones (7) y casos

| Funcion | EXE | Casos |
|---|---|---|
| CTaskSimpleFightingControl::CalcMoveCommand | 0x624B50 | 1024 |
| CTaskSimpleFightingControl::Clone | vtable 0x86D700 hueco 1 (0x622EB0) | 128 |
| CTaskSimpleFightingControl::MakeAbortable | vtable 0x86D700 hueco 6 (0x61DD00) | 1024 |
| CTaskComplexProstituteSolicit::Clone | vtable 0x86FB88 hueco 1 (0x6622F0) | 128 |
| CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute | 0x661D30 | 256 |
| CTaskComplexProstituteSolicit::IsTaskValid | 0x661BB0 | 1024 |
| CTaskComplexProstituteSolicit::MakeAbortable | vtable 0x86FB88 hueco 6 (0x661B80) | 256 |

Total 3840 pares. Cada par registra `original` y `candidate` (retorno, traza y huella del estado completo).

## Correcciones de fuente que este banco debe confirmar (CTaskSimpleFightingControl)

- `CalcMoveCommand`: el rumbo se resta de la rotacion ANTES de envolver (`LimitRadianAngle(rumbo) - rot` era distinto); envoltura de una sola vuelta (> pi: -2pi, < -pi: +2pi); comparacion estricta `> 0x8D2E60` (0.2617993950843811, no `DegreesToRadians(15)`); `rand` del EXE (0x821B1E) se consume SIEMPRE antes de comparar la distancia en la rama de peaton; el umbral 0.8 compara la magnitud, no el rango; en vehiculo la suma es (z + y) + x y la comparacion es estricta (`> 0.7`); el avance de tiempo usa `_ftol2` de 64 bits como el EXE.
- `MakeAbortable`: prioridad distinta de 0/1/2 y URGENT sin evento caen al final (antes `NOTSA_UNREACHABLE`); `GetEventType` se llama de nuevo en cada comparacion (2,3,4 y luego 1,5,6); la normal de impacto esta en +0x18 en los tres eventos; producto escalar `(y*ny + z*nz) + x*nx`; umbral 0.707 (0x863C1C), no `SQRT_2/2`.

## Intercepcion

Mismo stub en la direccion del EXE y en el simbolo de la DLL (entrada incremental y cuerpo) con `EntryPatch`, y solo actuan en el hilo del banco (`owner`/`Mine()`; en otros hilos devuelven un valor neutro/real): `rand` 0x821B1E, `FindPlayerPed` 0x56E210, `ComputeEntityBoundingBoxPlanesUncachedAll` 0x5F3660, `ComputeEntityHitSide` 0x5F3AC0, `IsPlayerOnAMission` 0x464D50, `IsCoopGameGoingOn` 0x441390.
Se ejecutan de verdad: `FindTaskByType`, `GetSimplestActiveTask`, `GetTaskFighting`, `GetVehicleAppearance`, `IsUpsideDown`, `IsOnItsSide`, `Normalise`, el pool de tareas (las tareas creadas por Clone se detectan en el pool y se borran) y las referencias.

## Fixtures

Peatones, entidad objetivo, vehiculos, inteligencia, datos de jugador, tareas falsas (vtable trazada) y evento falso (vtable trazada con prioridad y tipo) en buffers propios con desplazamientos crudos del EXE. `IsTaskValid` genera los casos por "etapa que falla" (17 condiciones en orden del EXE) mas un segundo fallo aleatorio, con fronteras exactas (distancia al cuadrado 100, up.z = -0.9, right.z = +-0.8). `CalcMoveCommand` cubre los cinco valores de retorno, fronteras de angulo (eje +y: la resta es exacta), envolturas, `rand` con bajos bits controlados, pasos de tiempo extremos (5e8: desbordamiento de 32 bits, negativo, 0) y vehiculos con planos fijados (modo sensible al orden de la suma). `MakeAbortable` cubre prioridades fuera de rango, evento nulo, tipos 0..6 y otros, prioridades de evento 21/22/23, normales en la frontera 0.707.

## Que se compara

Retorno, memoria completa del objeto probado, tareas nuevas del pool (tipo + palabras sin vptr), memoria de las tareas falsas, huella de todos los buffers (referencias enmascaradas), listas de referencias antes y despues del borrado, traza completa (orden y numero de llamadas virtuales y de rand), palabra de control x87 y pila x87 vacia (`FpuTag`). El x87 corre a 24 bits. Se comprueba `m_LastFreeSlot` y el relleno 0xCD de los huecos libres del pool al inicio de cada ejecucion y se restauran al final, igual que las entradas parcheadas, los huecos de vtable y el paso de tiempo.

## Supuestos a revisar al compilar

- El destructor de Prostitute se ejecuta con `FindPlayerPed` devolviendo nulo al borrar las tareas creadas (sale pronto): el borrado solo comprueba que el pool queda limpio.
- Prostitute::Clone con cliente nulo no se prueba (el constructor del EXE y de la DLL desreferencian el cliente).
- GetTaskPool/IsFreeSlotAtIndex/GetAt y los desplazamientos 0x80 por hueco se copian de TasksBatch4.
