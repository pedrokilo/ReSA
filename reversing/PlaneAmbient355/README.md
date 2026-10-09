<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7047 con evidencia histórica, 0 con vigencia acreditada y 1399 pendientes en 38 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlaneAmbient355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPlane::SwitchAmbientPlanes`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-plane-ambient355/20261008-045209-886331/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-045209-886331, passed for the tested snapshot. Latest historical passing report: 20261008-045209-886331.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

SwitchAmbientPlanes 0x6CCC50: 1.024 pares preparados, pendientes de ejecución. Pool aislado de 0-8 slots con subtipos 0-5, createdBy 0-2, slots libres/usados y activación previa/nueva. Remove y destructor virtual registran traza simétrica, orden inverso e indicador delete=1. Restaura pool y GenPlane_Active. Evidencia combo ../VehicleAudio355Evidence/Plane_6CCC50.txt.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CPlane.SwitchAmbientPlanes_6CCC50.c](../_handoff/ghidra/CPlane.SwitchAmbientPlanes_6CCC50.c)
- [Plane_6CCC50.txt](../VehicleAudio355Evidence/Plane_6CCC50.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
