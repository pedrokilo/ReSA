<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Colas de eventos de grupo

Validadas el 02/10/2026 con **6.464 pares original/candidato dentro de GTA**, todos exactos: `CInformFriendsEventQueue` (Init, Flush, Add, Process), `CInformGroupEventQueue` (Init, Flush, Add, Process) y `CInformGroupEvent` (Constructor, Destructor, Process). 11/11 con `validated=1`.

**Diferencias reales corregidas por el banco:**
- `CInformFriendsEventQueue::Add`: el retardo es `300 + trunc(rand/32768·500)` (0x4AC235); la plantilla entera de `GetRandomNumberInRange` podía dar 1 ms menos. Ahora usa `CGeneral::GetRandomIntInRange`.
- `CInformGroupEvent`: el constructor original (0x4AC350) no inicializa `m_PedGroup`.

**Método:** eventos falsos con vtable propia (registran borrados y clones), peatones y colas como fixtures, y las dependencias `CEventGroup::Add`, `CPedGroupIntelligence::AddEvent`, `ComputeResponseTaskType`, `GetPedsGroup` y el RNG controladas. Se comparan el retorno, las colas globales, las listas de referencias, los efectos sobre los eventos y la traza. Las colas reales se guardan y se restauran byte a byte.

**Límites:** no se ejecuta la IA real de los grupos ni la respuesta a eventos; no acredita independencia del EXE.

Repetir: `python reversing/InformQueues/build_probe.py` y `python reversing/run.py --case inform`. Evidencia: [validation-summary.json](validation-summary.json).
