<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainNearest355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::FindNearestTrain`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-nearest355/20261008-011930-068753/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-011930-068753, passed for the tested snapshot. Latest historical passing report: 20261008-011930-068753.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

FindNearestTrain 0x6F7090: 1.024 pares preparados, pendiente ejecución. Pool aislado 0-8 slots libres/usados, tipos 0-6, filtros front carriage, posición placeable/matriz, empates y NaN. Distancia planar en orden original y recorrido inverso; retorna identidad normalizada y compara bytes conservados. Restaura pool. Evidencia combo ../VehicleAudio355Evidence/Train_6F7090.txt.

## Resultado de ejecución

El coordinador confirmó aprobación contra el EXE el 2026-10-08: 1,024 pares, sin diferencias. Identificador y enlace del informe final pendientes de incorporación por el coordinador. Esta aprobación no amplía las ramas ni dependencias descritas arriba.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.FindNearestTrain_6F7090.c](../_handoff/ghidra/CTrain.FindNearestTrain_6F7090.c)
- [Train_6F7090.txt](../VehicleAudio355Evidence/Train_6F7090.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
