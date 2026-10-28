<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
