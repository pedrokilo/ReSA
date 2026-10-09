<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CameraPath355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCamera::LoadPathSplines`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-path355/20261008-070847-021754/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CameraPath355

CCamera::LoadPathSplines — 0x5B24D0, 1024 pares EXE/DLL pendientes de ejecución. Combo REA+ASM: ../World355Evidence/CCamera_LoadPathSplines.txt.

Fixtures de cero a cinco bloques, cero a tres puntos, cuatro/diez componentes, números fraccionarios positivos/negativos, comentarios, líneas vacías, delimitadores coma/espacio, líneas ignoradas y fin tras quinto encabezado. Parser real sscanf/strtok/atof; mocks simétricos de carga de línea, limpieza y asignación. El allocator se sustituye solo durante la llamada, y sus trazas usan arrays fijos sin asignaciones; se restaura antes de emitir JSON. Todo el buffer de cámara y los cuatro buffers se compara por FNV-1a64, con tamaños y pointers normalizados exactos. Sin archivos reales ni render.

python reversing/CameraPath355/build_probe.py
python reversing/CameraPath355/run_local.py

## Evidencia y cierre del lote 355

Último informe real: `20261008-041930-768411`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-path355/20261008-041930-768411/summary.json>).

Última aprobación histórica: `20261008-041930-768411`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-path355/20261008-041930-768411/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCamera_LoadPathSplines.txt](../World355Evidence/CCamera_LoadPathSplines.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
