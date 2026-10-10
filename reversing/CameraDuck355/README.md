<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CameraDuck355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCamera::HandleCameraMotionForDucking`, `CCamera::HandleCameraMotionForDuckingDuringAim`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-duck355/20261008-070617-299554/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CameraDuck355

HandleCameraMotionForDucking y HandleCameraMotionForDuckingDuringAim,2048pares. Combo REA/asm. TaskDuck presente/ausente, abortando/activo, duckflag, velocidades bajo/alrededor/sobreumbral exacto 0x358637BE, ajuste bloqueado/permitido, source/target nulos y timestep variable. Las dos búsquedas de tarea se preservan y trazan mediante stubs pareados. Globals timestep/constantes de ajuste restaurados. No se modifican cámaras reales. Pendiente de compilar/ejecutar por coordinador.

Aprobado por coordinador: 2048 pares, informe `20261007-222014-783330`. Comprobar vigencia del hash final y repetir si cambió la fuente compartida.

## Evidencia y cierre del lote 355

Último informe real: `20261007-222014-783330`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-duck355/20261007-222014-783330/summary.json>).

Última aprobación histórica: `20261007-222014-783330`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-duck355/20261007-222014-783330/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCamera_HandleCameraMotionForDucking.txt](../World355Evidence/CCamera_HandleCameraMotionForDucking.txt), [CCamera_HandleCameraMotionForDuckingDuringAim.txt](../World355Evidence/CCamera_HandleCameraMotionForDuckingDuringAim.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
