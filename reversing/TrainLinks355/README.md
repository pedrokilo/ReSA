<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7047 con evidencia histórica, 0 con vigencia acreditada y 1399 pendientes en 38 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainLinks355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::FindCaboose`, `CTrain::FindEngine`, `CTrain::FindCarriage`, `CTrain::FindNextStationPositionInDirection`, `CTrain::FindMaximumSpeedToStopAtStations`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-links355/20261008-011706-019489/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-011706-019489, passed for the tested snapshot. Latest historical passing report: 20261008-011706-019489.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

# CTrain: enlaces y distancias a estaciones

Cinco funciones preparadas para comparaciÃ³n diferencial contra el EXE 1.0 US: FindCaboose, FindEngine, FindCarriage, FindNextStationPositionInDirection y FindMaximumSpeedToStopAtStations. Cada funciÃ³n tiene 1.024 pares, 5.120 en total; ejecuciÃ³n y aprobaciÃ³n pendientes.

Combos con pseudocÃ³digo y ASM en ../VehicleAudio355Evidence/Train_*.txt. FindEngine no tiene pseudocÃ³digo disponible inicialmente; la rutina completa ocupa 0x6F5E90â€“0x6F5EAC y se transcribe desde ASM, pendiente de recuperaciÃ³n REA.

Las cadenas de uno a ocho vagones son aisladas y se recorren desde distintas posiciones, incluyendo Ã­ndices de vagÃ³n fuera de la cadena. La tabla de estaciones y longitud de vÃ­a se sustituye temporalmente y se restaura. Se prueban ambas direcciones, el lÃ­mite de cien unidades, vuelta al inicio/final, NaN silencioso en la bÃºsqueda de estaciÃ³n y distintas vÃ­as en el cÃ¡lculo de velocidad. Se comparan retornos, salidas, todos los bytes de vagones y globals. No se usan trenes ni estaciones reales. Este banco no valida circulaciÃ³n, generaciÃ³n, carga de archivos ni pasajeros.

## Resultado de ejecución

El coordinador confirmó aprobación contra el EXE el 2026-10-08: 5,120 pares, sin diferencias. Identificador y enlace del informe final pendientes de incorporación por el coordinador. Esta aprobación no amplía las ramas ni dependencias descritas arriba.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.FindCaboose_6F5E70.c](../_handoff/ghidra/CTrain.FindCaboose_6F5E70.c)
- [CTrain.FindCarriage_6F5EB0.c](../_handoff/ghidra/CTrain.FindCarriage_6F5EB0.c)
- [CTrain.FindMaximumSpeedToStopAtStations_6F5BA0.c](../_handoff/ghidra/CTrain.FindMaximumSpeedToStopAtStations_6F5BA0.c)
- [CTrain.FindNextStationPositionInDirection_6F5F00.c](../_handoff/ghidra/CTrain.FindNextStationPositionInDirection_6F5F00.c)
- [Train_6F5BA0.txt](../VehicleAudio355Evidence/Train_6F5BA0.txt)
- [Train_6F5E70.txt](../VehicleAudio355Evidence/Train_6F5E70.txt)
- [Train_6F5E90.txt](../VehicleAudio355Evidence/Train_6F5E90.txt)
- [Train_6F5EB0.txt](../VehicleAudio355Evidence/Train_6F5EB0.txt)
- [Train_6F5F00.txt](../VehicleAudio355Evidence/Train_6F5F00.txt)

REA pseudocode recovery for CTrain::FindEngine failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
