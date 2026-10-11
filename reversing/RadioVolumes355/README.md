<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8547 funciones reconstruidas; 8547 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9226 entradas binarias sin nombre catalogado; 822 destinos originales explícitos fuera del catálogo, 398 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 102 aprobadas dentro del juego; 133 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RadioVolumes355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAERadioTrackManager::UpdateRadioVolumes`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-radio-volumes355/20261008-020732-679230/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-020732-679230, passed for the tested snapshot. Latest historical passing report: 20261008-020732-679230.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

UpdateRadioVolumes 0x4EA010: 1024 pares pendientes. Cutscene/pausa/camera/speech globals aislados; hardware master getters secuencia mutable y trazada para exigir orden y repetición; eventos/posición/distancia15 exacta/NaN; fade m_f80/84 negativo/cero/NaN; bass-1..3; channel16 truncation. Getters/engine/relative/channel stubs simétricos, vector Magnitude real pura. ASM log10 fldlg2/fyl2x en draft Radio_VolumesNative.txt; Cuerpo nativo aplicado; hook reversed=false hasta aprobación. Cuatro globalsbyte restaurados.

Runtime approved: 1,024 paired cases, report 20261008-020732-679230. Source and bank remain frozen for final coordinator verification.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAERadioTrackManager.UpdateRadioVolumes_4EA010.c](../_handoff/ghidra/CAERadioTrackManager.UpdateRadioVolumes_4EA010.c)
- [Radio_4EA010.txt](../VehicleAudio355Evidence/Radio_4EA010.txt)
- [Radio_VolumesNative.txt](../VehicleAudio355Evidence/Radio_VolumesNative.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
