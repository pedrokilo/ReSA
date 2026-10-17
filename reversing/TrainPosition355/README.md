<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainPosition355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::FindPositionOnTrackFromCoors`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-position355/20261008-045146-101524/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-045146-101524, passed for the tested snapshot. Latest historical passing report: 20261008-045146-101524.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

FindPositionOnTrackFromCoors 0x6F6CC0: 1024 isolated EXE/DLL pairs prepared, runtime pending. Zero through eight nodes, all four tracks, matrix/placeable positions, projection bounds, near/far track, carriage length, direction, speed NaN, and changing lighting between dependency calls. Complete object/node hashes and exact lighting trace; contiguous track globals, model slot, daylight restored. Evidence ../VehicleAudio355Evidence/Train_6F6CC0.txt. x87 projection denominator retained; remaining arithmetic requires runtime comparison before approval.

Runtime repair: GetLightingFromCollision0x6F5F80 returns its packed lighting
byte in AL and uses ret0. The original aggregate-return bank callback introduced
a hidden output pointer/stack cleanup, causing AV at the next loop iteration.
The target now calls the verified uint8 __thiscall dependency explicitly, and
the bank callback returns uint8 with the same ABI; repeat approval is pending.

Second runtime 20261008-024836-946601 had17/1024 state differences. Named x87 helpers now preserve lighting extended intermediates/store boundaries and rail interpolation extended subtraction/wrap. Diagnostics add full object DWORDs plus distance/speed/brightness/position; original hash equality gate remains strict. Repeat pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.FindPositionOnTrackFromCoors_6F6CC0.c](../_handoff/ghidra/CTrain.FindPositionOnTrackFromCoors_6F6CC0.c)
- [Train_6F6CC0.txt](../VehicleAudio355Evidence/Train_6F6CC0.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
