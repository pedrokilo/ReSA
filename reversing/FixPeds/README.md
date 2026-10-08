<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# Arreglos de peatones y grupos fuera de listas

Validadas el 03/10/2026 con **9.664 pares original/candidato dentro de GTA**, todos exactos: 21 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPedAttractor` | Constructor, Destructor, `SetTaskForPed`, `RegisterPed`, `DeRegisterPed`, `IsRegisteredWithPed`, `IsAtHeadOfQueue`, `GetTaskForPed`, `GetQueueSlot`, `GetHeadOfQueue`, `GetTailOfQueue`, `HasEmptySlot`, `ComputeDeltaPos`, `ComputeDeltaHeading`, `ComputeAttractTime`, `ComputeAttractPos`, `ComputeAttractHeading`, `BroadcastDeparture`, `BroadcastArrival` |
| `CPedGroupIntelligence` | `FlushTasks` |
| `CPlayerInfo` | `CancelPlayerEnteringCars` |

**Diferencias reales corregidas:**
- `CPedAttractor`:
  - las cuatro virtuales (`IsAtHeadOfQueue`, `ComputeAttractPos`, `ComputeAttractHeading`, `BroadcastDeparture`, huecos 2-5 de 0x86C538) se enganchaban con `RH_ScopedInstall`: el símbolo de la DLL es un thunk de llamada virtual que vuelve a la entrada enganchada (bucle infinito). Ahora van por la vtable;
  - `Destructor` llamaba al destructor por la vtable (`std::destroy_at`), que vuelve a 0x5EC410: mismo bucle. Ahora es una llamada no virtual;
  - el constructor no inicializa por valor los `SArray` (el original no escribe el allocator) ni el nombre de script, y copia el nombre con `strcpy`;
  - `RegisterPed` mete al peatón en `m_AttractPeds` (no en la cola), con el hueco = tamaño de la cola y la comparación con signo;
  - `DeRegisterPed` busca y borra en `m_AttractPeds` (buscaba en la cola y borraba en la otra lista);
  - `BroadcastDeparture` busca al peatón en la cola (buscaba en `m_AttractPeds` y borraba en la cola con ese iterador) y manda a los de detrás al hueco i - 1 tras esperar 2000 · i ms (usaba el hueco i);
  - `BroadcastArrival` manda a los que se acercan con `PEDMOVE_WALK`, no con el modo del atractor;
  - `SetTaskForPed` apunta la tarea nueva en `ms_tasks` (no lo hacía) y solo busca/borra la vieja si no es nula;
  - `GetQueueSlot` devuelve -1 si el peatón no está; `HasEmptySlot` compara con signo;
  - `IsAtHeadOfQueue` lee el primer hueco aunque la cola esté vacía (la comprobación de puntero nulo queda bajo `FIX_BUGS`);
  - `ComputeAttractPos` calcula `(dirección · id) · separación` y `ComputeAttractHeading` ya no sobrescribe `m_UseDir` con la dirección de la cola.
- `CPedGroupIntelligence::FlushTasks` es `__thiscall` con dos argumentos (`ret 8`); como función estática `__cdecl` descuadraba la pila de los llamadores del EXE.
- `CPlayerInfo::CancelPlayerEnteringCars` es un `__thiscall` vacío que saca su argumento (`ret 4`); era estático `__cdecl`.

**Método:**
- El atractor vive en un buffer propio con la vtable derivada del EXE de cada tipo (0-3 y 5-9; el de refugio sobrescribe las virtuales). Las virtuales se recorren por esas vtables, así que pasan por la entrada enganchada como en la partida.
- Los `SArray` (`m_AttractPeds`, `m_ArrivedPeds`, `m_PedTaskPairs` y `ms_tasks` en 0xC0985C) se reservan con el `operator new` del EXE. Se comparan por contenido, tamaño, capacidad y si han cambiado de bloque. `ms_tasks` se sustituye y se restaura en cada ejecución.
- En los pares se enmascara el relleno tras `UsedTask`, porque el EXE copia ahí un par local sin inicializar.
- Se usan peatones ficticios (solo se comparan punteros), tareas falsas con el destructor trazado y una entidad falsa con matriz para las referencias del constructor y del destructor.
- Las tareas nuevas se buscan en el pool, con `m_LastFreeSlot` restaurado y los huecos libres rellenos con 0xCD. Se enmascara el relleno sin escribir de la secuencia, de la tarea de quedarse quieto y de la de atractor emparejado.
- Se simula y se traza el RNG (`rand` del EXE y `CGeneral::GetRandomNumber`).
- En `FlushTasks` y `CancelPlayerEnteringCars` se mide el desplazamiento de la pila tras la llamada.
- En el constructor y el destructor la vtable del EXE y la de la DLL cuentan como la misma.
- `IsAtHeadOfQueue` siempre tiene bloque en la cola, para no entrar en el camino de `FIX_BUGS`.
- El fichero `fixpeds-only.txt` en la carpeta del juego limita el banco a una función.

Repetir: `python reversing/FixPeds/build_probe.py` y `python reversing/run.py --case fixpeds`. Evidencia: [validation-summary.json](validation-summary.json).
