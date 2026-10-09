<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptAnim355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CRunningScript::PlayAnimScriptCommand`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-anim355/20261008-022305-841354/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ScriptAnim355

PlayAnimScriptCommand: 2048 pares. Combo completo; opcodes605/812/88A/A1A/default, flags anim, duración−2..2, interrupt/offset/sequence, allocator-null en ambos pasos y GivePedScriptedTask. Constructores reales de RunNamedAnim/AffectSecondaryBehaviour colaboradores previamente reconstruidos; GetAnimBlock pareado nullptr sin I/O. Stubs Collect/Label/TaskNew/Give pareados. Globals restauradas, objetos completos FNV64. Hook false pendiente raíz.

## Evidencia y cierre del lote 355

Último informe real: `20261008-022305-841354`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-anim355/20261008-022305-841354/summary.json>).

Última aprobación histórica: `20261008-022305-841354`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-anim355/20261008-022305-841354/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CRunningScript_PlayAnimScriptCommand.txt](../World355Evidence/CRunningScript_PlayAnimScriptCommand.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
