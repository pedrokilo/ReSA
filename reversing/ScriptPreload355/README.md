<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptPreload355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAEScriptAudioEntity::PreloadMissionAudio`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-preload355/20261008-020607-715362/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-020607-715362, passed for the tested snapshot. Latest historical passing report: 20261008-020607-715362.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

PreloadMissionAudio 0x4EC190: 1.024 pares preparados, pendiente de ejecución. Ranuras válidas e inválidas, sonidos activos/inactivos, mapper exitoso/fallido y bancos/sonidos de distintos signos. Mapper y carga de hardware simétricos registran evento, ranura y argumentos; compara bytes de todos los enlaces y exige ramas positivas. EXE canónico y evidencia ../VehicleAudio355Evidence/Script_4EC190.txt.

## Evidencia y cierre del lote 355

Último informe real: `20261008-020607-715362`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-preload355/20261008-020607-715362/summary.json>).

Última aprobación histórica: `20261008-020607-715362`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-preload355/20261008-020607-715362/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [Script_4EC190.txt](../VehicleAudio355Evidence/Script_4EC190.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAEScriptAudioEntity.PreloadMissionAudio_4EC190.c](../_handoff/ghidra/CAEScriptAudioEntity.PreloadMissionAudio_4EC190.c)
- [Script_4EC190.txt](../VehicleAudio355Evidence/Script_4EC190.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
