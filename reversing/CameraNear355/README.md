<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CameraNear355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCamera::SetNearClipBasedOnPedCollision`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-near355/20261008-070759-551855/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CameraNear355

CCamera::SetNearClipBasedOnPedCollision — 50CB90, 2048 pares pendientes. Combo intentado: REA devolvió invalid_request para esta función pequeña; la evidencia conserva el error y ASM completo, delimitado a 50CB90..50CBDF. Reconstrucción basada en ese ASM, sin pseudocódigo REA disponible.

Raíz, división y clamp conservan la pila x87 original. Fixtures: entrada/base/denominador 0, -0, positivos, negativos, NaN e infinito; salida near-clip bit a bit por stub simétrico, sin render. Bytes completos de cámara/variables mediante FNV-1a64. Puntero de variables y denominador restaurados.

python reversing/CameraNear355/build_probe.py
python reversing/CameraNear355/run_local.py

Aprobado por coordinador: 2048 pares, informe `20261007-221526-026282`. Cambios posteriores de fuente compartida requieren comprobar hash final y repetir si corresponde.

## Evidencia y cierre del lote 355

Último informe real: `20261007-221526-026282`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-near355/20261007-221526-026282/summary.json>).

Última aprobación histórica: `20261007-221526-026282`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-near355/20261007-221526-026282/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCamera_SetNearClipBasedOnPedCollision.txt](../World355Evidence/CCamera_SetNearClipBasedOnPedCollision.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
