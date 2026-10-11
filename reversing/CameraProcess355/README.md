<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CameraProcess355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCamera::Process`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-process355/20261008-071113-018494/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CameraProcess355

`CCamera::Process` (0x52B730), reconstruida en C++ a partir de REA y ensamblador completo del EXE canónico (`World355Evidence/CCamera_Process.txt`). Banco preparado de 16384 pares; historial de ejecución detallado en la sección de evidencia.

Fixtures aislados cubren inicialización de objetivos/cámaras, revisión de control, movimiento/tracking/FOV, fades/widescreen, cambio angular, estado/fin de transición, fases de objetivo y cámara incluidas fracciones cero e igualdad, interpolación del up/FOV, modos topdown/37 y generales, mínimo horizontal, geometría, escalera/garaje, efecto de embriaguez, shake/blur, barco inclinado, LOD, RW, promedio de velocidad y agua.

Los colaboradores se sustituyen simétricamente: no se procesa una escena ni se renderiza, carga agua o dispara eventos reales. `StoreValuesDuringInterPol` devuelve un FOV modificado deliberadamente para detectar que Process vuelve a leer el estado después de la llamada. Los callbacks conservan los argumentos y float por bits, y se compara FNV1a64 de todos los bytes de cámara, matrices, entidades y RW más snapshots explícitos de vectores y escalares para diagnosticar diferencias.

Se mantienen helpers x87 nombrados para easing, conversiones ftol, shake y acumulación de distancia. El cuerpo completo conserva C++ mantenible. Los globales de escena, FOV/LOD, reloj, efecto de embriaguez, flags de cámara, distancias RW y estado de agua se restauran. Hook desactivado hasta aprobación runtime.

## Evidencia y cierre del lote 355

Último informe real: `20261008-045823-488973`; no aprobado, pares no informados en el cierre. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-camera-process355/20261008-045823-488973/summary.json>).

No hay aprobación histórica en los informes disponibles.

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCamera_Process.txt](../World355Evidence/CCamera_Process.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
