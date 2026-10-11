<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8547 funciones reconstruidas; 8547 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9226 entradas binarias sin nombre catalogado; 822 destinos originales explícitos fuera del catálogo, 398 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 102 aprobadas dentro del juego; 133 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorControl355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `InteriorGroup_c::Update`, `InteriorGroup_c::SetupPeds`, `InteriorGroup_c::UpdatePeds`, `InteriorGroup_c::RemovePed`, `InteriorGroup_c::SetupOfficePeds`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-control355/20261007-210431-873664/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# InteriorControl355

Cinco entradas (Update, SetupPeds, UpdatePeds, RemovePed, SetupOfficePeds), 2560 pares. Reconstruidas desde combo REA/asm. Compara bytes y orden de llamadas con entrada/salida habilitada, estado de paths y actualización, tipos de grupo válidos y desconocidos, ped pendiente válido/inválido/no encontrado. Los colaboradores se sustituyen simétricamente y se registra su llamada; la función bajo prueba permanece real. Streaming y población no tocan entidades reales. Se restaura el flag global de activación. RemovePed carece de retorno definido en EXE; se corrige void.

Pendiente de compilación y ejecución por coordinador. Los cuerpos de los colaboradores pendientes no se validan por este banco.

Snapshot de objetos completos mediante FNV-1a de 64 bits; outputs, punteros y trazas explícitos permanecen exactos. Las aprobaciones anteriores requieren repetición con este banco actualizado.

Aprobado 2560 pares, 0 diferencias, informe resultados-interior-control355/20261007-210431-873664/summary.json. Repetición final necesaria por cambios posteriores en InteriorGroup_c.

## Evidencia y cierre del lote 355

Último informe real: `20261007-210431-873664`; aprobado, 2560 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-control355/20261007-210431-873664/summary.json>).

Última aprobación histórica: `20261007-210431-873664`; aprobado, 2560 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-control355/20261007-210431-873664/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [InteriorGroup_c_Update.txt](../World355Evidence/InteriorGroup_c_Update.txt), [InteriorGroup_c_SetupPeds.txt](../World355Evidence/InteriorGroup_c_SetupPeds.txt), [InteriorGroup_c_UpdatePeds.txt](../World355Evidence/InteriorGroup_c_UpdatePeds.txt), [InteriorGroup_c_RemovePed.txt](../World355Evidence/InteriorGroup_c_RemovePed.txt), [InteriorGroup_c_SetupOfficePeds.txt](../World355Evidence/InteriorGroup_c_SetupOfficePeds.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
