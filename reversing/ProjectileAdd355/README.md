<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ProjectileAdd355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CProjectileInfo::AddProjectile`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-add355/20261008-022104-441344/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ProjectileAdd355

AddProjectile (`0x737C80`), 16384 pares EXE/DLL pendientes. C++ completo de los ocho tipos válidos16/17/18/19/20/21/39/58 desde combo `../World355Evidence/CProjectileInfo_AddProjectile.txt`.

Fixtures positivas de creación/gravedad/heading/throw force/rocket vehicle-player-dir/null/flare, slot32/full/allocation failure, collisionData nuevo/vacío/existente, reference order/matrix/world, teargasFX/play/null, victim/creator damage link/radar/audio. Constructor externo de CProjectile5A4030 conservado como colaborador; stub simétrico con storage aislado0x19C. Model table/object pool/collision allocation/FX/Streaming/radar/audio todos falsos o stub, globals/arrays/camera/time restaurados.

FNV1a64 de todos los bytes de objeto/creator/victim/infos/slot pointers/matrices/model collision/data/sphere y trace/salida exacta. Stubs SetMatrix sólo leen/copían XYZ: flags/padding de matriz local que el EXE deja indefinidos se excluyen. Fuera del dominio: tipo de arma inválido, cuyo camino EXE usa locales sin inicializar; candidato devuelve false. Pares0..16383.

## Evidencia y cierre del lote 355

Último informe real: `20261008-022104-441344`; aprobado, 16384 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-add355/20261008-022104-441344/summary.json>).

Última aprobación histórica: `20261008-022104-441344`; aprobado, 16384 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-add355/20261008-022104-441344/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CProjectileInfo_AddProjectile.txt](../World355Evidence/CProjectileInfo_AddProjectile.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
