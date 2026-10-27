<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Arrastrar fuera despacio y levantarse (`CTaskComplexCarSlowBeDraggedOutAndStandUp`)

Banco preparado el 05/10/2026 (sin compilar ni ejecutar todavía): **4.608 pares original/candidato dentro de GTA** previstos para las 8 funciones que faltaban por validar de la clase.

| Función | Casos |
|---|---|
| Constructor (0x648620) | 256 |
| Destructor (0x648690) | 256 |
| `Clone` (vtable 0x86EF80, hueco 1) | 256 |
| `MakeAbortable` (hueco 6) | 512 |
| `CreateNextSubTask` (hueco 8) | 1024 |
| `CreateFirstSubTask` (hueco 9) | 1024 |
| `ControlSubTask` (hueco 10) | 256 |
| `CreateSubTask` (0x648710) | 1024 |

**Reconstrucción desde el EXE 1.0 US (sin `FIX_BUGS`):**
- `ControlSubTask` (0x648B80): `mov eax,[ecx+8]; ret 4` (devuelve `m_pSubTask`).
- `MakeAbortable` (0x6486F0): solo con prioridad 2 (inmediata) delega en `m_pSubTask->MakeAbortable` (hueco 6, sin guarda de nulo); si no, `false`.
- `CreateSubTask` (0x648710): `LEAVE_CAR` (0x2C0) mira el grupo de animación (`handling+0xDE` + 0x58) de `m_Vehicle`: 0x67 (autobús largo) o 0x70 (tanque) crean `CTaskComplexLeaveCar(veh del peatón, 8 o 10, 0, false, true)`, y 0x68 con `dword10 == 8` también; `GET_UP_AND_STAND_STILL` (0xCE) crea `CTaskComplexGetUpAndStandStill`; `SET_PED_OUT` (0x330) crea `CTaskSimpleCarSetPedOut(veh del peatón, dword10, true)`; `CAR_SLOW_BE_DRAGGED_OUT` (0x337) crea `CTaskComplexCarSlowBeDraggedOut(m_Vehicle, dword10, false)`; el resto (incluido `TASK_FINISHED`) devuelve nulo.
- `CreateNextSubTask` (0x6488F0): tras `SET_PED_OUT` o `LEAVE_CAR` termina; tras `GET_UP_AND_STAND_STILL` llama a `IsPlayer` (resultado descartado) y termina; tras `CAR_SLOW_BE_DRAGGED_OUT` crea `GetUpAndStandStill` si el peatón está vivo y su salud no es `<= 0` (NaN cuenta como mayor, `fcomp` + `test ah,41h`), si no añade un `CEventDeath(false)` al grupo de eventos de su inteligencia y termina.
- `CreateFirstSubTask` (0x648A10): si el peatón es `FindPlayerPed(0)` avisa al audio de su vehículo (`PlayerAboutToExitVehicleAsDriver`), si no y es el conductor llama a `SetRadioStation`; con grupo 0x67/0x70 (o 0x68 con `dword10 == 8`) devuelve `CreateSubTask(LEAVE_CAR)`; si no reproduce un sonido de golpe según el grupo (0x45 o 0x46 según `dword10` 10 o 8 para los grupos 0, 13, 14, 18, 19, 22, 23 y 28; 0x47 grupo 1; 0x49 grupo 17) y devuelve `CreateSubTask(CAR_SLOW_BE_DRAGGED_OUT)`.
- Se reutiliza el vehículo del peatón sin guarda de nulo, como el EXE (los fixtures de `CreateFirstSubTask` siempre lo tienen).

**Método:**
- 3 peatones con inteligencia propia (salud 0, negativa, NaN, minúscula y positiva; estados vivo, muriendo y muerto; tipos de peatón variados), 2 vehículos con vtable de trampas, handling propio (byte de grupo de animación variado) y conductor, 3 tareas falsas (`GetTaskType`, `MakeAbortable` y destructor trazados) y un evento falso.
- Simulados con traza: RNG, `FindPlayerPed`, `CPed::SetRadioStation`, `PlayerAboutToExitVehicleAsDriver`, `AddAudioEvent` y `CEventGroup::Add` (el evento de muerte de la pila se serializa por campos).
- Las subtareas creadas salen del pool real (constructores del EXE en el original y del DLL en el candidato) y se serializan enteras. Pool de tareas restaurado en cada ejecución (slots libres con 0xCD); constructores sobre buffer con 0xCD comparados enteros; reloj guardado y restaurado.

Repetir: `python reversing/DragOut60/build_probe.py` y `python reversing/run.py --case dragout60`.
