<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8547 funciones reconstruidas; 8547 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9226 entradas binarias sin nombre catalogado; 822 destinos originales explícitos fuera del catálogo, 398 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 102 aprobadas dentro del juego; 133 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainHit355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::TrainHitStuff<CPtrListSingleLink<CPhysical*>>`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-hit355/20261008-024817-633875/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-024817-633875, passed for the tested snapshot. Latest historical passing report: 20261008-024817-633875.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

TrainHitStuff<CPtrListSingleLink<CPhysical*>>0x6F5CF0:1024 pares pendientes. List0..8/exclusionself, type/static/waiting, signedmodel-1 versus table65535, matchingmodels0/500. Fakevtable4SetIsStatic muta enlaceNext para comprobar que el recorrido lo captura antes, Moving paired con sideeffect. Fullphysicalobjects/nodeNext/globalmodelindices contiguous12 restaurados. Sin lista/pool real. Nativa aplicada hookfalse.

Runtime repair: EXE movsx sign-extends the 16-bit model index before comparing to unsigned 16-bit configured model IDs. The source now explicitly casts int16; storedFFFF therefore does not match configured65535. Waiting for repeat.

Runtime repeat approved: 20261008-024817-633875, 1,024 pairs. Final coordinator hash verification pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.TrainHitStuff_CPtrListSingleLink_CPhysical____6F5CF0.c](../_handoff/ghidra/CTrain.TrainHitStuff_CPtrListSingleLink_CPhysical____6F5CF0.c)
- [Train_6F5CF0.txt](../VehicleAudio355Evidence/Train_6F5CF0.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
