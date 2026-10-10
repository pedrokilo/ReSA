<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptLocateChar355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CRunningScript::LocateCharCommand`, `CRunningScript::LocateCharCarCommand`, `CRunningScript::LocateCharCharCommand`, `CRunningScript::LocateCharObjectCommand`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-locate-char355/20261008-052639-595259/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ScriptLocateChar355

Cuatro funciones, 10240 pares: LocateCharCommand y LocateCharCar/Char/ObjectCommand. Combo completo; opcodefamilies2D/3D, stopped, onfoot/invehicle/any, ped en vehículo null/nonnull, límites x/y/z y highlight con/sin. Stubs Collect/Compare/Highlight/IsPedStopped pareados. Pools/params/clocks restaurados; FNV64 objetos completos. Pendiente raíz.

## Evidencia y cierre del lote 355

Último informe real: `20261008-045633-041988`; no aprobado, pares no informados en el cierre. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-locate-char355/20261008-045633-041988/summary.json>).

No hay aprobación histórica en los informes disponibles.

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CRunningScript_LocateCharCommand.txt](../World355Evidence/CRunningScript_LocateCharCommand.txt), [CRunningScript_LocateCharCarCommand.txt](../World355Evidence/CRunningScript_LocateCharCarCommand.txt), [CRunningScript_LocateCharCharCommand.txt](../World355Evidence/CRunningScript_LocateCharCharCommand.txt), [CRunningScript_LocateCharObjectCommand.txt](../World355Evidence/CRunningScript_LocateCharObjectCommand.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
