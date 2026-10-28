<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ProjectileCore355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CProjectileInfo::Initialise`, `CProjectileInfo::GetProjectileInfo`, `CProjectileInfo::RemoveFXSystem`, `CProjectileInfo::Shutdown`, `CProjectileInfo::RemoveNotAdd`, `CProjectileInfo::RemoveDetonatorProjectiles`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-core355/20261007-204038-114408/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ProjectileCore355

Seis funciones reconstruidas con el pseudocódigo de REA y ensamblador del EXE canónico: Initialise, GetProjectileInfo, RemoveFXSystem, Shutdown, RemoveNotAdd y RemoveDetonatorProjectiles. Evidencia en `../World355Evidence/CProjectileInfo_*.txt`.

Banco diferencial de 3072 pares (512 por función). Revisa bytes completos de las 32 entradas y objetos, retorno y llamadas de explosiones y FX. Incluye FX presente/ausente, destrucción inmediata/diferida, tipos admitidos/rechazados, entradas activas/inactivas y detonadores. Los efectos se sustituyen simétricamente en EXE y DLL; no se crean explosiones reales. Se restauran las dos tablas globales y el estado de hooks.

Aprobado: **3072 pares, cero diferencias**, ejecución `resultados-projectile-core355/20261007-204038-114408/summary.json` (compilación /MP6 y runner del coordinador). No constituye validación de AddProjectile, Update ni las otras eliminaciones, ni independencia del EXE.

Snapshot de objetos completos mediante FNV-1a de 64 bits; outputs, punteros y trazas explícitos permanecen exactos. Las aprobaciones anteriores requieren repetición con este banco actualizado.

## Evidencia y cierre del lote 355

Último informe real: `20261007-204038-114408`; aprobado, 3072 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-core355/20261007-204038-114408/summary.json>).

Última aprobación histórica: `20261007-204038-114408`; aprobado, 3072 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-core355/20261007-204038-114408/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CProjectileInfo_Initialise.txt](../World355Evidence/CProjectileInfo_Initialise.txt), [CProjectileInfo_GetProjectileInfo.txt](../World355Evidence/CProjectileInfo_GetProjectileInfo.txt), [CProjectileInfo_RemoveFXSystem.txt](../World355Evidence/CProjectileInfo_RemoveFXSystem.txt), [CProjectileInfo_Shutdown.txt](../World355Evidence/CProjectileInfo_Shutdown.txt), [CProjectileInfo_RemoveNotAdd.txt](../World355Evidence/CProjectileInfo_RemoveNotAdd.txt), [CProjectileInfo_RemoveDetonatorProjectiles.txt](../World355Evidence/CProjectileInfo_RemoveDetonatorProjectiles.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
