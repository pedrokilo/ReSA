<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainQueries355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::FindClosestTrackNode`, `CTrain::IsNextStationAllowed`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-queries355/20261008-011753-566671/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-011753-566671, passed for the tested snapshot. Latest historical passing report: 20261008-011753-566671.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

FindClosestTrackNode 0x6F6BD0 e IsNextStationAllowed 0x6F7260: 1.024 pares cada una, pendiente ejecución. Nodos aislados en cuatro vías y búsqueda con distancias finitas menores que 99.999,9: se excluye dominio original de retorno indeterminado sin mejora. Closest compara suma original z²+y²+x². Allowed incluye 1-8 vagones, dirección, distancia, nivel y stats negativos/positivos/NaN con rechazo NaN confirmado ASM. Restaura nodos y estaciones. Dependencias zone/stat simétricas; retornos y trazas exactos.

## Resultado de ejecución

El coordinador confirmó aprobación contra el EXE el 2026-10-08: 2,048 pares, sin diferencias. Identificador y enlace del informe final pendientes de incorporación por el coordinador. Esta aprobación no amplía las ramas ni dependencias descritas arriba.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.FindClosestTrackNode_6F6BD0.c](../_handoff/ghidra/CTrain.FindClosestTrackNode_6F6BD0.c)
- [CTrain.IsNextStationAllowed_6F7260.c](../_handoff/ghidra/CTrain.IsNextStationAllowed_6F7260.c)
- [Train_6F6BD0.txt](../VehicleAudio355Evidence/Train_6F6BD0.txt)
- [Train_6F7260.txt](../VehicleAudio355Evidence/Train_6F7260.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
