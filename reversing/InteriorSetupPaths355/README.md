<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorSetupPaths355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `InteriorGroup_c::SetupPaths`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-setup-paths355/20261008-020256-734750/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# InteriorSetupPaths355

SetupPaths (`0x595590`), 4096 pares EXE/DLL; todavía pendiente de ejecución.

Reconstrucción completa desde pseudocódigo REA y ensamblador en `../World355Evidence/InteriorGroup_c_SetupPaths.txt`. Firma EXE corregida a void. Incluye grafo de puntos de paso, cuatro puertas, índices negativos, primer portal, nodos exteriores, distancia de conexión <=9, conexiones transversales y emparejamiento greedy de salidas sin exterior con distancia cuadrada <3. No se sintetiza conexión 1-3.

Fixtures de dos interiores independientes, 0/3/4/5/6 puntos, habilitación de puertas, grupo doméstico/oficina, vecinos externos y salidas pendientes. Colaboradores PathFind simétricos; ninguna ruta real se modifica. Snapshot FNV1a64 de todos los bytes del grupo y ambos interiores; trace exacta de argumentos y orden. Tabla global de nodos restaurada. Pares contiguos 0..4095. CompleteNewInterior y RemoveInteriorLinks son colaboradores externos sin implementación local y se invocan por sus direcciones originales.

## Evidencia y cierre del lote 355

Último informe real: `20261008-020256-734750`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-setup-paths355/20261008-020256-734750/summary.json>).

Última aprobación histórica: `20261008-020256-734750`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-setup-paths355/20261008-020256-734750/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [InteriorGroup_c_SetupPaths.txt](../World355Evidence/InteriorGroup_c_SetupPaths.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
