<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CameraNewMode355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCamera::TryToStartNewCamMode`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-new-mode355/20261008-070817-079516/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CameraNewMode355

TryToStartNewCamMode (`0x51E560`), 16384 pares EXE/DLL pendientes. Switch completo reconstruido C++ desde combo `../World355Evidence/CCamera_TryToStartNewCamMode.txt`, incluido ASM final recuperado por dis.sh después del límite de2000 líneas.

512 fixtures por cada secuencia -1..30. Cubre vehículo/boat/tank, coors/velocity, ground/roof/water, posiciones fijas y nearclip, distancias/dot, pool4vehículos/wanted/cop/rechazo, siete ciney modos positivos, modos24/25 y default. Stubs simétricos de colaboradores; no control real de cámara/pools/player/engine. Helper x87 conserva ATan/cos/sin del ajuste de dirección. FNV1a64 de todos los bytes de cámara, seis entidades, cinco matrices y RW, salida y trace exacta. Globals pool/RW/active/floor restaurados. Pares0..16383.

## Evidencia y cierre del lote 355

Último informe real: `20261008-045912-807470`; aprobado, 16384 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-new-mode355/20261008-045912-807470/summary.json>).

Última aprobación histórica: `20261008-045912-807470`; aprobado, 16384 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-new-mode355/20261008-045912-807470/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCamera_TryToStartNewCamMode.txt](../World355Evidence/CCamera_TryToStartNewCamMode.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
