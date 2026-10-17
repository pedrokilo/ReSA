<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ProjectileUpdate355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CProjectileInfo::Update`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-update355/20261008-052937-363970/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ProjectileUpdate355

CProjectileInfo::Update (`0x738B20`), 8192 pares EXE/DLL pendientes. C++ completo desde pseudo+ASM `../World355Evidence/CProjectileInfo_Update.txt`.

Cubre playbackgate/slot32, FXkill/creator validity, tipo16/17/18/19/20/21/39/58/other, friction/fire, particles positivos, expiración/detonador, aceleración/speedcap, flare rollback, near-creator/LOS, heatseek victim y decoy positivos, player audio, Hunter damping/steering, target prediction, attached charge y lastpos. Fixture32 proyectos/matrices +creator/victim/fakeFX; stubs simétricos sin audio/fuego/partículas/explosiones/allocaciones reales. Remove sólo desactiva slot; pow conserva ABI x87. FNV1a64 de todos los bytes de proyectos/matrices/creator/victim/infos más salida/trace exactas; arrays/globals/clock/fx/ignore restaurados. Pares0..8191.

## Evidencia y cierre del lote 355

Último informe real: `20261008-050255-372332`; no aprobado, pares no informados en el cierre. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-update355/20261008-050255-372332/summary.json>).

No hay aprobación histórica en los informes disponibles.

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CProjectileInfo_Update.txt](../World355Evidence/CProjectileInfo_Update.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
