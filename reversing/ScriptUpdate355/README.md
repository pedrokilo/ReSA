<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptUpdate355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAEScriptAudioEntity::UpdateParameters`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-update355/20261008-023126-882798/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-023126-882798, passed for the tested snapshot. Latest historical passing report: 20261008-023126-882798.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

UpdateParameters 0x4EC970 VMT862E58 slot0: 1024 pares preparados pendientes ejecución. Links cero/uno coincidente, fin -1, entidades matrix/placeable, eventos104/1086/1113/1118/1042/1167/1177/1147/other, reloj wrap y límites300/500/4500/4800, volumen/velocidad NaN, field8C bits0/1/2/NaN. El switch se ejecuta por CADA link no coincidente: llamadas repetidas preservadas. Stubs Stop con efecto en velocidad, SetPosition, Resident/BankSound, RNG simétricos; tabla volumen y reloj restaurados. Evidencia combo Script_4EC970.txt, cuerpo draft Script_UpdateNative.txt. Cuerpo nativo aplicado; hook reversed=false hasta aprobación.

## Evidencia y cierre del lote 355

Último informe real: `20261008-023126-882798`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-update355/20261008-023126-882798/summary.json>).

Última aprobación histórica: `20261008-023126-882798`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-update355/20261008-023126-882798/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia del objetivo `CAEScriptAudioEntity::UpdateParameters`: [Script_4EC970.txt](../VehicleAudio355Evidence/Script_4EC970.txt). La ranura0 de VMT `0x862E58` apunta a `0x4EC970`, confirmado leyendo el EXE canónico. El archivo conserva ensamblador del objetivo hasta los retornos `0x4ECCD1` y `0x4ECCE4`; su tramo desde `0x4ECCF0` pertenece a la función siguiente. El pseudocódigo no está disponible en el índice de combo. El proveedor REA actual rechazó x86 de este EXE; esa limitación no se presenta como pseudocódigo recuperado.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [Script_4EC970.txt](../VehicleAudio355Evidence/Script_4EC970.txt)
- [Script_UpdateNative.txt](../VehicleAudio355Evidence/Script_UpdateNative.txt)

REA pseudocode recovery for CAEScriptAudioEntity::UpdateParameters failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
