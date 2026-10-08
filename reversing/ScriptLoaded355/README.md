<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptLoaded355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAEScriptAudioEntity::PlayLoadedMissionAudio`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-loaded355/20261008-042037-359035/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-042037-359035, passed for the tested snapshot. Latest historical passing report: 20261008-042037-359035.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

PlayLoadedMissionAudio 0x4EC270: 1.024 pares preparados, pendiente ejecución. Ranuras válidas/inválidas; bank/sound inválidos y loading 0/1; evento 65535, volumen -128 y ordinary, entidad con/sin matriz, sentinela/cero/posición. Initialise, SetFlags, Request y destructor semánticos simétricos; flags iniciales 14 y protección 0x100/0x200/0x800. Compara enlaces completos y traza argumentos/posición/volumen/flags, sin observar padding indeterminado del CAESound temporal original. Restaura pointertabla de volumen y hooks. Evidencia ../VehicleAudio355Evidence/Script_4EC270.txt.

## Evidencia y cierre del lote 355

Último informe real: `20261008-042037-359035`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-loaded355/20261008-042037-359035/summary.json>).

Última aprobación histórica: `20261008-042037-359035`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-loaded355/20261008-042037-359035/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [Script_4EC270.txt](../VehicleAudio355Evidence/Script_4EC270.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAEScriptAudioEntity.PlayLoadedMissionAudio_4EC270.c](../_handoff/ghidra/CAEScriptAudioEntity.PlayLoadedMissionAudio_4EC270.c)
- [Script_4EC270.txt](../VehicleAudio355Evidence/Script_4EC270.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
