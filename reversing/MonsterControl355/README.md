<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# MonsterControl355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CMonsterTruck::ExtendSuspension`, `CMonsterTruck::ResetSuspension`, `CMonsterTruck::ProcessControl`, `CMonsterTruck::ProcessControlCollisionCheck`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-monster-control355/20261008-024202-737146/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-024202-737146, passed for the tested snapshot. Latest historical passing report: 20261008-024202-737146.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

# CMonsterTruck: control y extensión

Banco diferencial preparado para cuatro entradas del alcance Pending355: `ExtendSuspension` (0x6C7D80), `ResetSuspension` (VMT 67, 0x6C7D40), `ProcessControl` (VMT 10, 0x6C8250), `ProcessControlCollisionCheck` (VMT 23, 0x6C8330). Son 1.024 pares por entrada, 4.096 en total. Resultado pendiente de ejecución por el coordinador.

La reconstrucción usa el EXE 1.0 US SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b` y los combos guardados en `../VehicleAudio355Evidence`. Se verificaron los índices contra la tabla original de 71 entradas en 0x8717D8. Reset copia la longitud de línea a la posición de rueda y escribe cuatro unos en 0x988; no escribe unos en la posición. Control normaliza las compresiones y extiende únicamente cuando la entidad no está aplazada y tiene velocidad.

Los objetos son búferes aislados. Las llamadas a la clase base se sustituyen simétricamente en EXE y DLL por trazas; no se valida aquí la física completa de CAutomobile. Cada par compara todos los bytes del objeto salvo la tabla virtual, además del orden y argumentos de llamadas. Los casos recorren ambos estados de aplazamiento, movimiento y reposo, compresiones inferiores/iguales/superiores a uno, límites y ajuste negativo. Se restauran el paso de tiempo, la tasa de extensión, el indicador global y todos los hooks.

`SetupSuspensionLines` y `ProcessSuspension` tendrán un banco separado y no están incluidos en este resultado.

Runtime approved: 20261008-024202-737146. All prepared paired cases passed; production and bank remain frozen for coordinator final verification. Monster constructor hook restoration is controlled by root after Setup approval.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CMonsterTruck.ExtendSuspension_6C7D80.c](../_handoff/ghidra/CMonsterTruck.ExtendSuspension_6C7D80.c)

- [Monster_Reset.txt](../VehicleAudio355Evidence/Monster_Reset.txt)

- [Monster_Control.txt](../VehicleAudio355Evidence/Monster_Control.txt)

- [Monster_Check.txt](../VehicleAudio355Evidence/Monster_Check.txt)

REA pseudocode recovery for CMonsterTruck::ResetSuspension failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

REA pseudocode recovery for CMonsterTruck::ProcessControl failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

REA pseudocode recovery for CMonsterTruck::ProcessControlCollisionCheck failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
