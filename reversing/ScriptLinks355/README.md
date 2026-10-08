<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptLinks355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAEScriptAudioEntity::AttachMissionAudioToPhysical`, `CAEScriptAudioEntity::ClearMissionAudio`, `CAEScriptAudioEntity::GetMissionAudioLoadingStatus`, `CAEScriptAudioEntity::GetMissionAudioPosition`, `CAEScriptAudioEntity::IsMissionAudioSampleFinished`, `CAEScriptAudioEntity::SetMissionAudioPosition`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-links355/20261007-205240-078573/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261007-205240-078573, passed for the tested snapshot. Latest historical passing report: 20261007-205240-078573.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

# Enlaces de audio de misión

Banco preparado para seis entradas pendientes de CAEScriptAudioEntity: AttachMissionAudioToPhysical (0x4EC100), ClearMissionAudio (0x4EC040), GetMissionAudioLoadingStatus (0x4EBF60), GetMissionAudioPosition (0x4EC4D0), IsMissionAudioSampleFinished (0x4EBFE0), SetMissionAudioPosition (0x4EC0C0). Cada entrada tiene 1.024 pares: 6.144 en total. Validado: 6.144/6.144 pares iguales, informe `resultados-script-links355/20261007-205240-078573`, ejecutado por el coordinador con el EXE canónico.

Reconstrucción comparada mediante combo (pseudocódigo REA y ASM íntegro) contra gta-sa.exe 1.0 US SHA-256 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. Evidencias en ../VehicleAudio355Evidence/Script_*.txt.

Objetos y entidades aislados, estados de enlace vacío/activo, posición sentinela/cero/ordinaria, entidad con/sin matriz, bancos y sonidos negativos/positivos y ranuras válidas/inválidas cuando la función original las comprueba. Las tres funciones sin comprobación original usan ranuras válidas para evitar desbordamientos. Se comparan todos los bytes del objeto, entidad y matriz, retornos normalizados a identidad y trazas de llamadas a hardware/gestor. Las dependencias de hardware y cancelación se sustituyen simétricamente en EXE y DLL. No se reproduce ni modifica audio real. Todos los hooks se restauran.

Las cinco restantes de la clase se validarán en otro banco; este resultado no las incluye.

## Evidencia y cierre del lote 355

Último informe real: `20261007-205240-078573`; aprobado, 6144 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-links355/20261007-205240-078573/summary.json>).

Última aprobación histórica: `20261007-205240-078573`; aprobado, 6144 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-links355/20261007-205240-078573/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [Script_4EC100.txt](../VehicleAudio355Evidence/Script_4EC100.txt), [Script_4EC040.txt](../VehicleAudio355Evidence/Script_4EC040.txt), [Script_4EBF60.txt](../VehicleAudio355Evidence/Script_4EBF60.txt), [Script_4EC4D0.txt](../VehicleAudio355Evidence/Script_4EC4D0.txt), [Script_4EBFE0.txt](../VehicleAudio355Evidence/Script_4EBFE0.txt), [Script_4EC0C0.txt](../VehicleAudio355Evidence/Script_4EC0C0.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAEScriptAudioEntity.AttachMissionAudioToPhysical_4EC100.c](../_handoff/ghidra/CAEScriptAudioEntity.AttachMissionAudioToPhysical_4EC100.c)
- [CAEScriptAudioEntity.ClearMissionAudio_4EC040.c](../_handoff/ghidra/CAEScriptAudioEntity.ClearMissionAudio_4EC040.c)
- [CAEScriptAudioEntity.GetMissionAudioLoadingStatus_4EBF60.c](../_handoff/ghidra/CAEScriptAudioEntity.GetMissionAudioLoadingStatus_4EBF60.c)
- [CAEScriptAudioEntity.GetMissionAudioPosition_4EC4D0.c](../_handoff/ghidra/CAEScriptAudioEntity.GetMissionAudioPosition_4EC4D0.c)
- [CAEScriptAudioEntity.IsMissionAudioSampleFinished_4EBFE0.c](../_handoff/ghidra/CAEScriptAudioEntity.IsMissionAudioSampleFinished_4EBFE0.c)
- [CAEScriptAudioEntity.SetMissionAudioPosition_4EC0C0.c](../_handoff/ghidra/CAEScriptAudioEntity.SetMissionAudioPosition_4EC0C0.c)
- [Script_4EBF60.txt](../VehicleAudio355Evidence/Script_4EBF60.txt)
- [Script_4EBFE0.txt](../VehicleAudio355Evidence/Script_4EBFE0.txt)
- [Script_4EC040.txt](../VehicleAudio355Evidence/Script_4EC040.txt)
- [Script_4EC0C0.txt](../VehicleAudio355Evidence/Script_4EC0C0.txt)
- [Script_4EC100.txt](../VehicleAudio355Evidence/Script_4EC100.txt)
- [Script_4EC4D0.txt](../VehicleAudio355Evidence/Script_4EC4D0.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
