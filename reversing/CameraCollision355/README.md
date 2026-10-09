<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CameraCollision355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCamera::CameraColDetAndReact`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-collision355/20261008-070514-223142/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CameraCollision355

CCamera::CameraColDetAndReact — 520190, 4096 pares pendientes; combo REA+ASM en ../World355Evidence/CCamera_CameraColDetAndReact.txt. Firma corregida a bool (AL original). La resolución usa TheCamera global como el EXE.

Fixtures: 14 contextos de cámara, entidad ignorada nula/coche/bici/objeto, caché de modelo acertada/fallida, collision data nula y conteo firmado -1..3, matriz existente/asignada, radio y límites, fracciones negativas/cero/intermedias/>1/NaN, colisión true/false, historial inicializado/no inicializado y progresión temporal. Cone-cast y near-clip simulados simétricamente; objetos y buffers globales se comparan por FNV-1a64; salida, radio, fracción y trazas son exactos. Historial/cache, variables de cámara, punteros, modelo y timestep restaurados. Sin render real.

python reversing/CameraCollision355/build_probe.py
python reversing/CameraCollision355/run_local.py

Aprobado por coordinador: 4096 pares, informe `20261007-221447-225791`. Cambios posteriores de fuente compartida requieren comprobar hash final y repetir si corresponde.

## Evidencia y cierre del lote 355

Último informe real: `20261007-221447-225791`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-collision355/20261007-221447-225791/summary.json>).

Última aprobación histórica: `20261007-221447-225791`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-collision355/20261007-221447-225791/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCamera_CameraColDetAndReact.txt](../World355Evidence/CCamera_CameraColDetAndReact.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
