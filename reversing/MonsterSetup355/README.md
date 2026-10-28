<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# MonsterSetup355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CMonsterTruck::SetupSuspensionLines`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-monster-setup355/20261008-024222-120602/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-024222-120602, passed for the tested snapshot. Latest historical passing report: 20261008-024222-120602.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

Banco SetupSuspensionLines (VMT 0x8717D8 slot 48, nativo 0x6C7FB0): 1.024 pares. Pendiente de ejecución. Fixture aislado con discos existentes, ausentes o líneas anteriores; asignación/liberación y posiciones de ruedas sustituidas simétricamente. Compara objeto, discos, límites, esfera y trazas; restaura entrada de modelo 400 y hooks. ASM íntegro: ../VehicleAudio355Evidence/Monster_SetupFull.txt.

Cuarentena temporal de arranque: el hook Constructor (0x6C8D60), previamente reconstruido, estÃ¡ .reversed=false porque su llamada calificada ejecuta SetupSuspensionLines incluso con el hook Setup desactivado. Restaurar Constructor=true antes del estado final una vez Setup aprobado. Esta cuarentena no es validaciÃ³n nueva ni altera el inventario de entradas pendientes.

Runtime approved: 20261008-024222-120602. All prepared paired cases passed; production and bank remain frozen for coordinator final verification. Monster constructor hook restoration is controlled by root after Setup approval.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [Monster_SetupFull.txt](../VehicleAudio355Evidence/Monster_SetupFull.txt)

REA pseudocode recovery for CMonsterTruck::SetupSuspensionLines failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
