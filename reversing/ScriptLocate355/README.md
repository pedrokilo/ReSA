# ScriptLocate355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CRunningScript::LocateCarCommand`, `CRunningScript::LocateObjectCommand`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-locate355/20261008-045605-499190/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ScriptLocate355

LocateCarCommand y LocateObjectCommand: 8192 pares. Combo completo; pool fixture refs válidas, 2D/3D, todos bordes inclusivos/exclusivos, stopped car y timestep, matriz/nulo, highlight on/off con IP/marker exactos. CollectParameters, UpdateCompareFlag y HighlightImportantArea simulados pareados. Globals pools/ScriptParams/clocks restaurados; FNV64 todos bytes. Pendiente raíz.

## Evidencia y cierre del lote 355

Último informe real: `20261008-045605-499190`; aprobado, 8192 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-locate355/20261008-045605-499190/summary.json>).

Última aprobación histórica: `20261008-045605-499190`; aprobado, 8192 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-locate355/20261008-045605-499190/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CRunningScript_LocateCarCommand.txt](../World355Evidence/CRunningScript_LocateCarCommand.txt), [CRunningScript_LocateObjectCommand.txt](../World355Evidence/CRunningScript_LocateObjectCommand.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
