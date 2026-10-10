<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainSkip355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::SkipToNextAllowedStation`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-skip355/20261008-011832-847068/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-011832-847068, passed for the tested snapshot. Latest historical passing report: 20261008-011832-847068.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

SkipToNextAllowedStation 0x6F72F0:1024 pares preparados. Cadena1-8 previa, clockwise, iteraciones rechazo0-3, estación0-5, matrix/placeable, raildistance. NextStation/Zones/Stats/Streaming/PassTime simétricos; LoadScene/LoadAll alteran posición antes de distancia/tiempo para exigir orden. No globals/pool real modificados. NaNStats excluido porque EXE repite indefinidamente; distancia finite y conversión _ftol2 low32. Cuerpo Train_SkipNative.txt aplicado, hook reversed=false hasta aprobación.

## Resultado de ejecución

El coordinador confirmó aprobación contra el EXE el 2026-10-08: 1,024 pares, sin diferencias. Identificador y enlace del informe final pendientes de incorporación por el coordinador. Esta aprobación no amplía las ramas ni dependencias descritas arriba.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.SkipToNextAllowedStation_6F72F0.c](../_handoff/ghidra/CTrain.SkipToNextAllowedStation_6F72F0.c)
- [Train_6F72F0.txt](../VehicleAudio355Evidence/Train_6F72F0.txt)
- [Train_SkipNative.txt](../VehicleAudio355Evidence/Train_SkipNative.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
