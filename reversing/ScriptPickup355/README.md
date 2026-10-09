<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptPickup355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CRunningScript::ScriptTaskPickUpObject`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-pickup355/20261008-024111-145481/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ScriptPickup355

ScriptTaskPickUpObject — 0x46AF50, 2048 pares EXE/DLL, pendientes de ejecución. Evidencia: ../World355Evidence/CRunningScript_ScriptTaskPickUpObject.txt (combo, pseudocódigo REA y ASM completo).

Fixtures: actor explícito y secuencia (-1), comandos 70A y 605, NULL en cada etiqueta y diferencia de mayúsculas, referencias de objeto válidas/inválidas, huesos truncados a byte, flags 10/18, offsets positivos/negativos, asignación nula en primera/segunda tarea. Constructores reales previamente reconstruidos; mocks simétricos de referencias, animaciones, incorporación a secuencia, tarea secundaria y registro. Se restaura parámetros, secuencia y pools. Se comparan todos los bytes de script/tareas con FNV-1a64 y trazas exactas. Sin eventos, archivos ni render real.

python reversing/ScriptPickup355/build_probe.py
python reversing/ScriptPickup355/run_local.py

## Evidencia y cierre del lote 355

Último informe real: `20261008-024111-145481`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-pickup355/20261008-024111-145481/summary.json>).

Última aprobación histórica: `20261008-024111-145481`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-pickup355/20261008-024111-145481/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CRunningScript_ScriptTaskPickUpObject.txt](../World355Evidence/CRunningScript_ScriptTaskPickUpObject.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
