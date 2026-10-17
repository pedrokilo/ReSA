<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEventDamage (lista de 20)

Validadas el 03/10/2026 con **6.976 pares original/candidato dentro de GTA**, todos exactos: 20 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventDamage` | Constructor (copia, 0x4B33B0), Constructor (0x4AD830), `From`, `GetEventType`, `GetEventPriority`, `GetLifeTime`, `AffectsPed`, `AffectsPedGroup`, `IsCriminalEvent`, `ReportCriminalEvent`, `GetSourceEntity`, `TakesPriorityOver`, `GetLocalSoundLevel`, `DoInformVehicleOccupants`, `CanBeInterruptedBySameEvent`, `CloneEditable`, `ProcessDamage`, `ComputeBodyPartToRemove`, `ComputeDeathAnim`, `ComputeDamageAnim` |

La vtable 0x85AD78 tiene 17 huecos (0x4B5CF0 … 0x4B5D40) y coincide con `RH_ScopedVirtualClass(CEventDamage, 0x85AD78, 17)` y los huecos de cada `RH_ScopedVMTInstall`.

**Diferencias reales corregidas:**
- `AffectsPed`:
  - `GetSourceEntity` solo se llama en la rama de invulnerable (antes, siempre al principio);
  - el arma se compara con signo con `WEAPON_GRENADE`;
  - `GetActiveTask` del origen se llama dos veces en la comprobación de sigilo;
  - producto escalar como (y·fy + z·fz) + x·fx.
- `AffectsPedGroup`: `GetActiveTask` del origen se llama dos veces.
- `ReportCriminalEvent`: el arma se compara con signo (`<= 9`, `<= 40`).
- `TakesPriorityOver`: primero la prioridad y la entidad del otro evento, después las propias.
- `ComputeBodyPartToRemove`: tirada con `CGeneral::GetRandomIntInRange` (0x407180).
- `ComputeDeathAnim`:
  - atropello de frente: brazo derecho → `KD_RIGHT` y el resto → `KO_SKID_BACK` (estaba invertido según la pieza);
  - con dirección mayor que 3 la animación de derribo no cambia (no `KO_SPIN_L`);
  - la fuerza usa la dirección 2D normalizada (0x44E480), no la 3D;
  - `GetBonePosition` (0x5E4280) sobre un vector a cero; `GetActiveTask` y `GetSimplestActiveTask` se llaman como en el EXE.
- `ComputeDamageAnim`:
  - `GetBonePosition` antes de `GetSimplestActiveTask`;
  - daño del arma (int16), arma y estado de movimiento comparados con signo; con salud NaN no cae;
  - `m_nHitAnimDelayTimer` = `_ftol2(float(tiempo) + 2500/1500)` (antes suma entera);
  - tiradas: brazo izquierdo `0xAB - _ftol2(rand · 1/32768 · -3)`, resto `GetRandomIntInRange(0, 3/4)` + base;
  - `HIT_WALK` solo si armadura + salud > 20 (con NaN no);
  - la fuerza usa la dirección 2D normalizada; `GetActiveTask` dos veces al hacer abortable la tarea.

**Método:**
- Original con los hooks de la clase desactivados frente a candidato con fixtures idénticos; constructores en la dirección del EXE sobre un buffer con 0xCD, comparados enteros (tipo en vez de vptr); virtuales por la entrada de la vtable del EXE.
- Falsos: peatones (matriz, inteligencia y datos de jugador propios), vehículo, objeto, tareas (vtable con `GetSubTask`, `GetTaskType` y `MakeAbortable` trazado), eventos de referencia e historial, `CPedGroup`, `CWeaponInfo`, `CWanted` y clump.
- Simulados y trazados:
  - `FindPlayerPed`, `FindPlayerWanted`, `CWanted::RegisterCrime`, `CPedType::PoliceDontCareAboutCrimesAgainstPedType`, `CPedGroups::AreInSameGroup`;
  - `CPedIntelligence::CanSeeEntityWithLights`, `CPhysical::CanPhysicalBeDamaged`/`ApplyMoveForce`, `CWeaponInfo::GetWeaponInfo`, `CStats::GetFatAndMuscleModifier`;
  - `CPed::GetBonePosition`/`RemoveBodyPart`, `RpAnimBlendClumpGetFirstAssociation` y el RNG (`rand` del EXE y `CGeneral::GetRandomNumber`).
- Se guardan y restauran en cada ejecución y al final: el pool de eventos (cabecera con `m_LastFreeSlot`, estados y huecos libres con 0xCD), el reloj, `ms_fThresholdDotProduct`, el truco `MEGA_PUNCH` y `m_bFireProof` del jugador en foco.

Repetir: `python reversing/Event20/build_probe.py` y `python reversing/run.py --case event20`. Evidencia: [validation-summary.json](validation-summary.json).
