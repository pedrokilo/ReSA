<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7047 con evidencia histórica, 0 con vigencia acreditada y 1399 pendientes en 38 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlaneCreation355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPlane::FindPlaneCreationCoors`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-plane-creation355/20261008-045318-070599/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-045318-070599, passed for the tested snapshot. Latest historical passing report: 20261008-045318-070599.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

FindPlaneCreationCoors0x6CD090:1024 pares preparados. Small/big, retries1..50, RNG/player paired con secuencia mutable, direction globals, scan0/65534/65535, sphere/LOS/buildings semantictraces, aliases outputsvector y orientation/height. x87 fsin/fcos namedhelper retiene precision antesfmul/fadd. FakeLOS muta origin despuesendcalculado; sphereSet altera direction para nextiteration; Clear observa scanantesreset. CameraForward12 y scan2 restaurados. Box local EXE sin inicializar no observado porque callee41A820 no lo usa; bankcompara sphereparamsemanticos. Cuerpo draft Plane_CreationNative.txt aplicado, hookfalse pendiente runtime.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CPlane.FindPlaneCreationCoors_6CD090.c](../_handoff/ghidra/CPlane.FindPlaneCreationCoors_6CD090.c)
- [Plane_6CD090.txt](../VehicleAudio355Evidence/Plane_6CD090.txt)
- [Plane_CreationNative.txt](../VehicleAudio355Evidence/Plane_CreationNative.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
