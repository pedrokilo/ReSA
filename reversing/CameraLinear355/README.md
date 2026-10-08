<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CameraLinear355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCamera::VectorMoveLinear`, `CCamera::VectorTrackLinear`, `CCamera::ProcessVectorTrackLinear-0`, `CCamera::ProcessVectorTrackLinear-1`, `CCamera::ProcessVectorMoveLinear-0`, `CCamera::ProcessVectorMoveLinear-1`, `CCamera::ProcessFOVLerp-0`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-linear355/20261008-070732-093629/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CameraLinear355

Siete entradas de interpolación de CCamera reconstruidas desde combo REA y asm: VectorMoveLinear, VectorTrackLinear, ambas sobrecargas ProcessVectorMoveLinear/ProcessVectorTrackLinear y ProcessFOVLerp(float). 7168 pares con interpolación directa y seno x87, tiempos antes/igual/después y unsigned altos, persistencia, duraciones negativas/cero/positivas y ratios externos a [0,1]. Se comparan bytes del bloque de interpolación y vectores de entrada. Reloj restaurado al terminar. Sin cámara/render reales.

Pendiente de compilación y ejecución por el coordinador. No cubre el resto de CCamera ni declara independencia del EXE.

Aprobado 7168 pares, 0 diferencias, informe 20261007-210125-857627.

## Evidencia y cierre del lote 355

Último informe real: `20261007-210125-857627`; aprobado, 7168 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-linear355/20261007-210125-857627/summary.json>).

Última aprobación histórica: `20261007-210125-857627`; aprobado, 7168 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-linear355/20261007-210125-857627/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCamera_VectorMoveLinear.txt](../World355Evidence/CCamera_VectorMoveLinear.txt), [CCamera_VectorTrackLinear.txt](../World355Evidence/CCamera_VectorTrackLinear.txt), [CCamera_ProcessVectorTrackLinear-0.txt](../World355Evidence/CCamera_ProcessVectorTrackLinear-0.txt), [CCamera_ProcessVectorTrackLinear-1.txt](../World355Evidence/CCamera_ProcessVectorTrackLinear-1.txt), [CCamera_ProcessVectorMoveLinear-0.txt](../World355Evidence/CCamera_ProcessVectorMoveLinear-0.txt), [CCamera_ProcessVectorMoveLinear-1.txt](../World355Evidence/CCamera_ProcessVectorMoveLinear-1.txt), [CCamera_ProcessFOVLerp-0.txt](../World355Evidence/CCamera_ProcessFOVLerp-0.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
