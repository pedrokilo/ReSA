<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlaneInputs355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPlane::ProcessControlInputs`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-plane-inputs355/20261008-025200-936978/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-025200-936978, passed for the tested snapshot. Latest historical passing report: 20261008-025200-936978.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

ProcessControlInputs 0x6CADD0 VMT0x871948 slot24: 1024 pairs prepared, pending runtime. Full plane object and mutable pad dependency traces. Mouse/keyboard arbitration, captured timestep before axis query, per-axis blending/decay/clamp, skid controls, steering/braking, landing-gear toggles and completion, Hydra signed low16 nozzle increments with repeated pad reads, disable-controls speed cap. Pad queries mutate timestep/mouse/time in selected fixtures. Contiguous vehicle/mouse/timer/active globals saved/restored; finite and NaN old axes, models539/520/464. Named x87 helpers preserve forward-dot, interpolation, pre-rounding gear completion and nozzle low16 conversion. External CRT pow helper0x822130 remains dependency. Scoped hook false until runtime approval. ASM Plane_InputsFull endsRET6CB7B4.

Runtime approved1,024 pairs report20261008-025200-936978. Final coordinator current-source hash verification pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CPlane.ProcessControlInputs_6CADD0.c](../_handoff/ghidra/CPlane.ProcessControlInputs_6CADD0.c)
- [Plane_InputsFull.txt](../VehicleAudio355Evidence/Plane_InputsFull.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
