<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RadioSelection355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAERadioTrackManager::ChooseIdentIndex`, `CAERadioTrackManager::ChooseAdvertIndex`, `CAERadioTrackManager::ChooseMusicTrackIndex`, `CAERadioTrackManager::ChooseDJBanterIndexFromList`, `CAERadioTrackManager::ChooseTalkRadioShow`, `CAERadioTrackManager::IsVehicleRadioActive`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-radio-selection355/20261007-215201-680039/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261007-215201-680039, passed for the tested snapshot. Latest historical passing report: 20261007-215201-680039.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

# CAERadioTrackManager: selección

Seis entradas con 1.024 pares cada una: ChooseIdentIndex, ChooseAdvertIndex, ChooseMusicTrackIndex, ChooseDJBanterIndexFromList, ChooseTalkRadioShow e IsVehicleRadioActive. Total 6.144 pares preparados, pendientes de compilación y ejecución por el coordinador.

Evidencia combo REA y ASM del EXE 1.0 US en ../VehicleAudio355Evidence/Radio_*.txt, SHA-256 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. Se conserva el límite específico de cada historial, los descartes y las repeticiones de estadísticas que hace el original. ChooseTalkRadioShow conserva los saltos del pseudocódigo revisados contra ASM, incluidos el último grupo opcional y el retorno byte del índice de programa.

Se sustituyen simétricamente RNG, consulta de estadísticas y opciones de vehículo en EXE/DLL; se comparan sus trazas ordenadas. La tabla de rangos DJ es un array contiguo de pares de enteros, pese al tipo histórico int32** de la declaración. Historiales reemplazados temporalmente y restaurados; los demás objetos son aislados. Se prueban rangos ausentes, historial repetido, búsqueda cíclica, estadísticas variadas y los tipos de radio permitido/prohibido/nulo. No se reproduce audio ni se cambian estaciones reales. Se restauran todos los hooks. No cubre las ocho funciones restantes del gestor.

Aprobado runtime:6144/6144 pares, informe resultados-radio-selection355/20261007-215201-680039. Flags cuarentena permanecenfalse hasta registro final del root; requiere vigencia hash final.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAERadioTrackManager.ChooseAdvertIndex_4E9570.c](../_handoff/ghidra/CAERadioTrackManager.ChooseAdvertIndex_4E9570.c)
- [CAERadioTrackManager.ChooseDJBanterIndexFromList_4E95E0.c](../_handoff/ghidra/CAERadioTrackManager.ChooseDJBanterIndexFromList_4E95E0.c)
- [CAERadioTrackManager.ChooseIdentIndex_4E94C0.c](../_handoff/ghidra/CAERadioTrackManager.ChooseIdentIndex_4E94C0.c)
- [CAERadioTrackManager.ChooseMusicTrackIndex_4EA270.c](../_handoff/ghidra/CAERadioTrackManager.ChooseMusicTrackIndex_4EA270.c)
- [CAERadioTrackManager.ChooseTalkRadioShow_4E8E40.c](../_handoff/ghidra/CAERadioTrackManager.ChooseTalkRadioShow_4E8E40.c)
- [CAERadioTrackManager.IsVehicleRadioActive_4E9800.c](../_handoff/ghidra/CAERadioTrackManager.IsVehicleRadioActive_4E9800.c)
- [Radio_4E8E40.txt](../VehicleAudio355Evidence/Radio_4E8E40.txt)
- [Radio_4E94C0.txt](../VehicleAudio355Evidence/Radio_4E94C0.txt)
- [Radio_4E9570.txt](../VehicleAudio355Evidence/Radio_4E9570.txt)
- [Radio_4E95E0.txt](../VehicleAudio355Evidence/Radio_4E95E0.txt)
- [Radio_4E9800.txt](../VehicleAudio355Evidence/Radio_4E9800.txt)
- [Radio_4EA270.txt](../VehicleAudio355Evidence/Radio_4EA270.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
