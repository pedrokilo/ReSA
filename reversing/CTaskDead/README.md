<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CTaskSimpleDead

Validadas el 07/10/2026 con **1.792 pares original/candidato dentro de GTA**, todos exactos: 6 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleDead` | Constructor (0x630590), Destructor (0x6305F0), `Clone` (0x636100), `GetTaskType` (0x6305D0), `MakeAbortable` (0x6305E0), `ProcessPed` (0x630600, slot 7 de la vtable 0x86DEA4 de 9 entradas) |

**Diferencias reales corregidas:**
- `ProcessPed` reconstruida desde el ensamblador (antes reenviaba al EXE con `.reversed = false`):
  - primera vez: animación de muerte en coche (puerta 10/11 -> 0xBB, resto -> 0xBC, mezcla 4.0), `SetPedState(55)`, salud a 0, retirada del modelo del arma activa (`GetWeaponInfo(tipo, 1)`, ranura con signo), `RemoveWeaponAnims(0, -1000.0f)` y recogidas/dinero si no es jugador, evento `CEventDeadPed` al grupo global y `ReportAccident`;
  - `FindPlayerPed(0)` sin comprobar nulo y `FindPlayerPed(1)` llamado dos veces, como el EXE;
  - pendiente del suelo: productos punto en el orden z, y, x, recorte a [-1, 1] (NaN pasa) y `asin` del CRT (0x821E70);
  - sangre: radio 0 / 0.75 / rampa `(t - 2000) * 0.00015`, 16 entidades del escáner con distancia (z² + y²) + x², `bDoBloodyFootprints` y 200; charco con `StoreStaticShadow` (id = this + 0x11) o `AddPermanentShadow` y el bit de charco solo en esta rama o cuando el agua cubre al peatón.
- `Clone` pasaba `m_bFirstTime` donde el constructor espera ahogado: el EXE (0x636100) pasa el bit 1 (`m_bHasDrowned`).
- Constructor: el EXE solo escribe los bits 0..2 del byte (+0xC); el tiempo y el ahogado ya no se inicializan por valor.
- Constructor y destructor catalogados con hook (0x630590 / 0x6305F0), junto con `Clone`, `GetTaskType` y `MakeAbortable` por la vtable.

**Método:**
- Original con los hooks de la clase desactivados frente a candidato con fixtures idénticos en búferes propios: seis peatones falsos de tamaño real (`m_pReferences = nullptr`), escáner de peds, jugadores falsos, clump y asociación de animación falsos.
- Simulados y trazados en el EXE y en la DLL: `FindPlayerPed`, `ComputeTargetDoorToExit`, `BlendAnimation`, `SetPedState`, `IsPlayer`, `GetWeaponInfo`, `RemoveWeaponModel`, `RemoveWeaponAnims`, `CreateDeadPedWeaponPickups`, `CreateDeadPedMoney`, `DeadPedMakesTyresBloody`, `GetEventGlobalGroup`, `CEventGroup::Add`, `CAccidentManager`, `CLocalisation::Blood`, `GetWaterLevelNoWaves`, `StoreStaticShadow` y `AddPermanentShadow`.
- Casos de `ProcessPed`: primera vez / no, ahogado / no, charco ya creado, tiempos de muerte (< 2 s, 2 s exactos, cruce de 2 s en el fotograma, rampa, 5 s, 7 s, > 7 s, reloj envuelto), agua por encima, igual, por debajo y NaN, objetivos bloqueados de ambos jugadores, jugador 1 ausente.
- Reloj (actual y previo) y `gpBloodPoolTex` guardados y restaurados; pool de tareas con `m_LastFreeSlot` restaurado y huecos a 0xCD.
- Constructor y destructor sobre búferes prellenados con 0xCD comparados palabra a palabra (sin vptr); `Clone` compara la tarea nueva del pool; `ProcessPed` compara además el valor devuelto, todos los peatones, matrices, escáner, asociación, referencias y la traza.

Repetir: `python reversing/CTaskDead/build_probe.py` y `python reversing/run.py --case ctaskdead`. Evidencia: [validation-summary.json](validation-summary.json).

## Revisión de continuidad (07/10/2026)

Compilación con `/MP6` y ejecución nueva: 1.792/1.792 pares exactos; estado, INI y partidas restaurados. Se corrigió un byte de codificación inválido en un comentario que impedía compilar.

`combo.sh` no dispone de pseudocódigo Ghidra para 0x630600 ni 0x636100 en su índice actual. La revisión está respaldada por ensamblador del EXE y comparación dinámica, sin atribuir pseudocódigo ausente. La vtable contiene nueve entradas: 0x637930, 0x636100, 0x43E300, 0x43E310, 0x6305D0, 0x421180, 0x6305E0, 0x630600 y 0x43E320; después comienzan constantes.

El constructor de copia es un helper NOTSA, sin función independiente identificada en el EXE. Las seis entradas catalogadas tienen evidencia vinculada; la revisión de dependencias y la regresión completa de juego siguen pendientes.

<!-- BEGIN PENDING BATCH REGRESSION -->

Regresión del 07/10/2026: **APROBADA**. 1.792 pares exactos. [Resumen actualizado](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-ctaskdead\20261007-201226-271151/summary.json>).
<!-- END PENDING BATCH REGRESSION -->
