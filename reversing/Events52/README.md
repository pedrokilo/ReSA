<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Eventos (lista de 52)

Validadas el 02/10/2026 con **13.888 pares original/candidato dentro de GTA**, todos exactos: 30 funciones de 10 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEvent` | Constructor, `CalcSoundLevelIncrement`, `GetSoundLevel` |
| `CEventAreaCodes` | Constructor, `AffectsPed`, `TakesPriorityOver` |
| `CEventGlobalGroup` | `AddEventsToGroup`, `AddEventsToPed`, `GetSoundLevel` |
| `CEventPedCollisionWithPed` | Constructor, `AffectsPed`, `TakesPriorityOver` |
| `CEventPotentialWalkIntoPed` | Constructor, `AffectsPed`, `TakesPriorityOver` |
| `CEventSoundQuiet` | Constructor, `AffectsPed`, `CloneEditable` |
| `CEventVehicleCollision` | Constructor, `AffectsPed`, `Clone` |
| `CEventVehicleDied` | Constructor, `AffectsPed`, `Clone` |
| `CEventVehicleHitAndRun` | Constructor, `Clone`, `ReportCriminalEvent` |
| `CEventVehicleOnFire` | Constructor, `AffectsPed`, `CloneEditable` |

**Diferencias reales corregidas:**
- `CEvent::CalcSoundLevelIncrement`: `pow` en double, sumas y división a 24 bits, y `log10` con `fyl2x`, como el EXE.
- `CEvent::GetSoundLevel`:
  - la distancia tiene un *mínimo* de 1 (la DLL la limitaba a un máximo);
  - suma (z²+y²)+x²;
  - `log10(d²)` con `fyl2x` ×10;
  - distancia ×0,01.
- `CEventGlobalGroup`: bucles con el contador con signo releído en cada vuelta, como el EXE.
- `CEventPedCollisionWithPed::AffectsPed`:
  - con estado de movimiento menor que caminar, la víctima debe estar en un grupo y ser su líder;
  - la comparación es con signo (`eMoveState` es `uint32`);
  - el contador de la víctima se mira antes de `IsThreatenedBy`;
  - se lee el puntero crudo del peatón a evitar.
- `CEventPotentialWalkIntoPed`:
  - el constructor registra la referencia sin comprobar nulo;
  - `AffectsPed` busca la tarea de matar entre las primarias (`FindTaskByType`).
- `CEventSoundQuiet::AffectsPed`:
  - el evento actual del historial se consulta por su propia vtable. Como la clase es `final`, la DLL resolvía `GetEventType`/`GetSourceEntity` en tiempo de compilación: el tipo daba siempre 62 y cualquier evento de otra clase pasaba por uno de sonido;
  - diferencia de tiempo con signo;
  - NaN cuenta como lejos.
- `CEventVehicleCollision::AffectsPed`: `SetNewVehicle` recibe los bits 2-5 con signo del byte +0xD.
- `CEventVehicleDied::AffectsPed`: con el indicador de salida del vehículo ya puesto, comprueba directamente el vehículo del peatón.
- `CEventVehicleOnFire::AffectsPed`: suma (x²+z²)+y² y NaN cuenta como cerca.

**Método:**
- Fixtures reutilizados de [Events70](../Events70/README.md): peatones, matrices, vehículos, inteligencias, historial y grupo falsos; pool de eventos restaurado.
- `CEventGlobalGroup` sobre un objeto falso con eventos falsos.
- Se simulan y se trazan `IsThreatenedBy`, `GetPedsGroup`, `ComputeEntityHitSide`, `ComputeEntityBoundingBoxPlanes`, `SetNewVehicle`, `FindPlayerWanted`, `RegisterCrime`, `CEventGroup::Add` y `AddEvent`.
- Solo NaN silenciosos.

Repetir: `python reversing/Events52/build_probe.py` y `python reversing/run.py --case events52`. Evidencia: [validation-summary.json](validation-summary.json).
