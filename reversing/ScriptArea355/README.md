<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptArea355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CRunningScript::CarInAreaCheckCommand`, `CRunningScript::ObjectInAreaCheckCommand`, `CRunningScript::CharInAreaCheckCommand`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-area355/20261008-045656-936113/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ScriptArea355

Car/Char/ObjectInAreaCheckCommand: 12288 pares. Combo completo; 2D/3D, rangos invertidos por eje y límites inclusivos, onfoot/invehicle/any y stopped, ped en vehículo nullptr/nonnull, highlight con altura media. Collect/Compare/Highlight/PedStopped/VehicleStopped pareados; pools, params y clocks restaurados; FNV64 todos bytes. Hooks aún reversed=false hasta aprobación. Pendiente raíz.

## Evidencia y cierre del lote 355

Último informe real: `20261008-045656-936113`; aprobado, 12288 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-area355/20261008-045656-936113/summary.json>).

Última aprobación histórica: `20261008-045656-936113`; aprobado, 12288 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-area355/20261008-045656-936113/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CRunningScript_CarInAreaCheckCommand.txt](../World355Evidence/CRunningScript_CarInAreaCheckCommand.txt), [CRunningScript_ObjectInAreaCheckCommand.txt](../World355Evidence/CRunningScript_ObjectInAreaCheckCommand.txt), [CRunningScript_CharInAreaCheckCommand.txt](../World355Evidence/CRunningScript_CharInAreaCheckCommand.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
