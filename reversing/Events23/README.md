<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Eventos, escáner y referencias (lista de 23)

Validadas el 02/10/2026 con **11.328 pares original/candidato dentro de GTA**, todos exactos: 32 funciones de 8 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventAcquaintancePed` | Constructor, `AffectsPed`, `AffectsPedGroup`, `TakesPriorityOver` |
| `CEventAttractor` | Constructor, `AffectsPed`, `CloneEditable`, `IsEffectActive` |
| `CEventBuildingCollision` | Constructor, `AffectsPed`, `CanTreatBuildingAsObject`, `IsHeadOnCollision` |
| `CEventDanger` | Constructor, `AffectsPed`, `AffectsPedGroup`, `GetSourceEntity` |
| `CEventStuckInAir` | Constructor, `AffectsPed`, `GetEventPriority`, `TakesPriorityOver` |
| `CEntityScanner` | Constructor, Destructor, `Clear`, `ScanForEntitiesInRange` |
| `CScriptedBrainTaskStore` | `SetTask`, `GetTask`, `Clear` (peatón y tarea) |
| `CReferences` | `Init`, `ListSize`, `PruneAllReferencesInWorld`, `RemoveReferencesToPlayer` |

**Diferencias reales corregidas:**
- `CEventAcquaintancePed`:
  - `AffectsPedGroup` devuelve cierto cuando el conocido no es miembro del grupo del jugador (faltaba la negación);
  - `TakesPriorityOver` es cierto cuando el otro evento no tiene conocido.
- `CEventAttractor::AffectsPed`:
  - la comprobación de refugio sin lluvia se hace siempre;
  - tiradas con `CGeneral::GetRandomIntInRange` (0x407180);
  - el orden de sumas del punto de mirada.
- `CEventBuildingCollision::AffectsPed`:
  - NaN pasa las pruebas de normal y producto escalar (`test ah,41h`);
  - el assert se reemplaza por el filtro real del EXE (evento 60, mismo edificio, producto > -0,5);
  - se quita una comprobación extra `dot <= -0,5`.
- `CEventDanger::AffectsPed` y `AffectsPedGroup`: `!(r² < d²)`, así que un radio o una posición NaN pasan.
- `CEventStuckInAir::GetEventPriority`: devuelve 75 solo cuando hay **más** de un evento en cola (la condición estaba invertida); la comparación es con signo. Arregla también `TakesPriorityOver`, que la llama por la vtable.
- `CReferences`:
  - `Init` recorre las 3000 entradas;
  - `RemoveReferencesToPlayer` repite las llamadas a `FindPlayerVehicle`/`FindPlayerPed` como el EXE;
  - `PruneAllReferencesInWorld` recorre cada pool del último slot al primero (cambia el orden de la lista libre).

**Método:**
- `PruneAllReferencesInWorld` corre sobre pools falsos de peatones, vehículos y objetos (tamaños del EXE).
- El escáner usa listas de sectores falsas sobre 24 entidades falsas.
- El almacén de tareas de los scripts se llena con peatones y tareas falsas.
- Se guardan y restauran `aRefs` y `pEmptyList`, el pool de eventos, los grupos de peatones, el almacén de tareas, los sectores de repetición, el código de búsqueda y los punteros de pools.
- Fuera de los fixtures:
  - el `assert` NOTSA de `CPedGroups::GetGroup`;
  - `m_ppEntity` nulo (la DLL lo comprueba en `ResolveReferences`);
  - efecto nulo en `IsEffectActive` (el EXE se cae);
  - rangos NaN o enormes en el escáner.

Repetir: `python reversing/Events23/build_probe.py` y `python reversing/run.py --case events23`. Evidencia: [validation-summary.json](validation-summary.json).
