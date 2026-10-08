<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RadioDJ355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAERadioTrackManager::ChooseDJBanterIndex`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-radio-dj355/20261008-020712-018463/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-020712-018463, passed for the tested snapshot. Latest historical passing report: 20261008-020712-018463.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

ChooseDJBanterIndex 0x4EA2D0: 1.024 pares preparados; pendiente aplicar fuente y ejecutar. DJ especial 0/1/2/otros, índices 0-21, historiales repetidos, horas 0-23, forecast, probabilidad, persecución y disturbios. Incluye secuencia exacta de llamadas forecast aun para rama original imposible [8,6). Stubs simétricos de dependencias; compara return/historia y trazas. Restaura globales. Evidencia combo ../VehicleAudio355Evidence/Radio_4EA2D0.txt.


Runtime approved: 1,024 paired cases, report 20261008-020712-018463. Source and bank remain frozen for final coordinator verification.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAERadioTrackManager.ChooseDJBanterIndex_4EA2D0.c](../_handoff/ghidra/CAERadioTrackManager.ChooseDJBanterIndex_4EA2D0.c)
- [Radio_4EA2D0.txt](../VehicleAudio355Evidence/Radio_4EA2D0.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
