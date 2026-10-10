<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeRoad355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::PlaceOnRoadProperly`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-road355/20261008-065641-875641/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-044710-678195, passed for the tested snapshot. Latest historical passing report: 20261008-044710-678195.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

PlaceOnRoadProperly 0x6BEEB0: 1024 pares pendientes EXE. Combo pseudocodigo y ASM Bike_6BEEB0/Bike_RoadFull. ColModel dos llamadas, rayos delante/atras con hit/miss independientes, mutacion de Z entre llamadas, lighting y flags C0000000, base x87 fpatan/fcos/fsin exacta, midpoint Z redondeado antes multiplicar .5, altura virtual con cambio a placement. Hash objeto completo y matriz, trazas parametros y orden. Valores finitos, matriz inicialmente valida, span positivo; no pretende validar NaN ni span cero. Altura fixture binaria exacta. Hooks nuevos false hasta runtime.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CBike.PlaceOnRoadProperly_6BEEB0.c](../_handoff/ghidra/CBike.PlaceOnRoadProperly_6BEEB0.c)
- [Bike_6BEEB0.txt](../VehicleAudio355Evidence/Bike_6BEEB0.txt)
- [Bike_RoadFull.txt](../VehicleAudio355Evidence/Bike_RoadFull.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
