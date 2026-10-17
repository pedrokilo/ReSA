<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarRoom355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCarEnterExit::IsRoomForPedToLeaveCar`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car-room355/20261008-073118-190832/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CarRoom355

IsRoomForPedToLeaveCar, 4096 pares aislados. combo REA/ASM, seat/tandem/bike/boat/train y puertas válidas/ inválidas, offsets opcionales, clear/sphere/vertical outcomes. Colaboradores stubs simétricos, geometría real. Tabla global restaurada, snapshots FNV64 y trazas exactas. Pendiente de validación coordinada.

## Evidencia y cierre del lote 355

Último informe real: `20261008-022225-380092`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car-room355/20261008-022225-380092/summary.json>).

Última aprobación histórica: `20261008-022225-380092`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car-room355/20261008-022225-380092/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCarEnterExit_IsRoomForPedToLeaveCar.txt](../World355Evidence/CCarEnterExit_IsRoomForPedToLeaveCar.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
