<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainCtor355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::Constructor`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-ctor355/20261008-014327-194356/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-014327-194356, passed for the tested snapshot. Latest historical passing report: 20261008-014327-194356.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

Constructor0x6F6030:1024 pares preparados. BaseCtor DLL resuelto en runtime con helper probado Boat23; confirma target coincide desde wrapperCVehicle y actualCTrainbody antes patch. BaseCtor/SetModelIndex/FillFrames/Colour/Audio/RNG paired aislados; fakeModelslots400/570,4handling blocks snapshot contiguo,clock. Fullobjectmenosvptr y traces intermedios puertas/audio. HandlingID lowbyte, dragNaN originalcopy, doors exactbits, passengers RNG order, trainflags preserved masks. Constructor nativo aplicado, hookfalse. Si resolver no encuentra target falla seguro sin llamar constructorreal.

Coordinator runtime approval: 1,024 comparisons, no differences, report 20261008-014327-194356.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.Constructor_6F6030.c](../_handoff/ghidra/CTrain.Constructor_6F6030.c)
- [Train_6F6030.txt](../VehicleAudio355Evidence/Train_6F6030.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
