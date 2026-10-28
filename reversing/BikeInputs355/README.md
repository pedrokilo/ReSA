<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeInputs355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::ProcessControlInputs`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-inputs355/20261008-065339-000335/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-044630-961682, passed for the tested snapshot. Latest historical passing report: 20261008-044630-961682.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

ProcessControlInputs 0x6BE310 VMT871360 slot24:1024 pares pendientes. ASM integro Bike_InputsFull. Mouse/keyboard arbitration, cached control mode, mouse-look decay, clamp finite/NaN, pad axes/pedals, forward/reverse/stationary, BMX exclusion, broken engine timer pattern, disabled controls speed cap. Paired GetPad and eight methods/player calls traced; GetPad mutates step/mouse/clock in selected cases to check order. Globals saved/restored as contiguous vehicle16,mouse8,timer64,active3 plus camera/steer/game. x87 blending, multiply-add, angle and normalization; decay uses original CRT pow internal 822130 dependency with x87 input ABI (outside scoped function). Fixtures timestep0/1/2 or mutated binary steps; no certification of CRT itself. Hookfalse until EXE validation.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CBike.ProcessControlInputs_6BE310.c](../_handoff/ghidra/CBike.ProcessControlInputs_6BE310.c)
- [Bike_InputsFull.txt](../VehicleAudio355Evidence/Bike_InputsFull.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
