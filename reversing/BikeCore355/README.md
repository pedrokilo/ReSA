<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeCore355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::ResetSuspension`, `CBike::SetRemoveAnimFlags`, `CBike::DoSoftGroundResistance`, `CBike::GetCorrectedWorldDoorPosition`, `CBike::PlayCarHorn`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-core355/20261008-065242-706340/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-052129-104573, passed for the tested snapshot. Latest historical passing report: 20261008-052129-104573.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

# CBike: suspensión, animaciones, suelo, posición y bocina

Cinco entradas preparadas, 1.024 pares cada una (5.120): ResetSuspension, SetRemoveAnimFlags, DoSoftGroundResistance, GetCorrectedWorldDoorPosition y PlayCarHorn. Aprobación pendiente de ejecución.

Combos guardados en ../VehicleAudio355Evidence/Bike_*.txt contra gta-sa.exe 1.0 US SHA-256 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. La bocina usa el índice tras modificar AL, como el ensamblador, y la posición corregida conserva el ancho peculiar del original (max.z menos max.x) y su orden de sumas. Reset escribe ruedas, velocidad angular, ratios y contadores exactos. SetRemoveAnimFlags comprueba entidad ped y aplica el bit de eliminación a las asociaciones filtradas con 0x1000.

Moto, ped, matriz, clump, colisión y asociaciones son objetos aislados. Se sustituyen consultas de asociaciones, adhesión, fuerza, colisión, RNG y voz simétricamente en EXE/DLL. Se comparan memoria, salidas, flags y trazas ordenadas. Variaciones de contacto, arena/otra superficie, acelerador, velocidad, caja de colisión, alarma, conductor y pausas de bocina. Las ramas activas deben producir trazas. Se restaura el paso de tiempo y todos los hooks; no se aplican fuerzas ni voz reales. No cubre control/IA/render completo de moto ni las otras trece entradas pendientes.

Inicialmente SetRemoveAnimFlags carecía de pseudocódigo REA; la rutina completa se conservó por ASM en 0x6B5F50–0x6B5F8A. Se documentará cualquier recuperación posterior por separado.

Runtime20261007-215243-526410: SoftGround/RemoveAnim/GetDoor aprobaron; Reset/Horn fallaron. Corregidos Reset offset750 (pitch angles, no angular velocity758) y Horn timer519 (m_nCarHornTimer, no pattern518). Fixture timer0/2/80/FF, pattern separado. Pendiente repeticion final.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CBike.DoSoftGroundResistance_6B6D40.c](../_handoff/ghidra/CBike.DoSoftGroundResistance_6B6D40.c)
- [CBike.GetCorrectedWorldDoorPosition_6BF230.c](../_handoff/ghidra/CBike.GetCorrectedWorldDoorPosition_6BF230.c)
- [CBike.ResetSuspension_6B6740.c](../_handoff/ghidra/CBike.ResetSuspension_6B6740.c)
- [Bike_6B5F50.txt](../VehicleAudio355Evidence/Bike_6B5F50.txt)
- [Bike_6B6740.txt](../VehicleAudio355Evidence/Bike_6B6740.txt)
- [Bike_6B6D40.txt](../VehicleAudio355Evidence/Bike_6B6D40.txt)
- [Bike_6BF230.txt](../VehicleAudio355Evidence/Bike_6BF230.txt)

- [Bike_6B7080.txt](../VehicleAudio355Evidence/Bike_6B7080.txt)

REA pseudocode recovery for CBike::SetRemoveAnimFlags failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

REA pseudocode recovery for CBike::PlayCarHorn failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
