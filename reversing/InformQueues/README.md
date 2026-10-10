<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Colas de eventos de grupo

Validadas el 02/10/2026 con **6.464 pares original/candidato dentro de GTA**, todos exactos: `CInformFriendsEventQueue` (Init, Flush, Add, Process), `CInformGroupEventQueue` (Init, Flush, Add, Process) y `CInformGroupEvent` (Constructor, Destructor, Process). 11/11 con `validated=1`.

**Diferencias reales corregidas por el banco:**
- `CInformFriendsEventQueue::Add`: el retardo es `300 + trunc(rand/32768·500)` (0x4AC235); la plantilla entera de `GetRandomNumberInRange` podía dar 1 ms menos. Ahora usa `CGeneral::GetRandomIntInRange`.
- `CInformGroupEvent`: el constructor original (0x4AC350) no inicializa `m_PedGroup`.

**Método:** eventos falsos con vtable propia (registran borrados y clones), peatones y colas como fixtures, y las dependencias `CEventGroup::Add`, `CPedGroupIntelligence::AddEvent`, `ComputeResponseTaskType`, `GetPedsGroup` y el RNG controladas. Se comparan el retorno, las colas globales, las listas de referencias, los efectos sobre los eventos y la traza. Las colas reales se guardan y se restauran byte a byte.

**Límites:** no se ejecuta la IA real de los grupos ni la respuesta a eventos; no acredita independencia del EXE.

Repetir: `python reversing/InformQueues/build_probe.py` y `python reversing/run.py --case inform`. Evidencia: [validation-summary.json](validation-summary.json).
