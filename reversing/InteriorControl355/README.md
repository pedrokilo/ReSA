<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
