<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ProjectileRemove355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CProjectileInfo::RemoveProjectile`, `CProjectileInfo::RemoveAllProjectiles`, `CProjectileInfo::RemoveIfThisIsAProjectile`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-remove355/20261007-221933-528941/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ProjectileRemove355

RemoveProjectile, RemoveAllProjectiles, RemoveIfThisIsAProjectile.1536pares. Combo REA/asm. Entradas activas/inactivas, FX presente/ausente, objetos encontrados/no encontrados, tipos de armas válidos/desconocidos, creadores nulos/ped/vehículo y cohetes player/no player. Explosiones, FX, audio, radar, world y destructor virtual con trazas pareadas. Pool de objetos ficticio conserva stride0x19C y generación de handles original. Globals/pool restaurados. Sin explosiones ni entidades reales. Pendiente de compilar y ejecutar por coordinador.

Snapshot de objetos completos mediante FNV-1a de 64 bits; outputs, punteros y trazas explícitos permanecen exactos. Las aprobaciones anteriores requieren repetición con este banco actualizado.

Aprobado por coordinador: 1536 pares, informe `20261007-221933-528941`. Comprobar vigencia del hash final y repetir si cambió la fuente compartida.

## Evidencia y cierre del lote 355

Último informe real: `20261007-221933-528941`; aprobado, 1536 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-remove355/20261007-221933-528941/summary.json>).

Última aprobación histórica: `20261007-221933-528941`; aprobado, 1536 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-projectile-remove355/20261007-221933-528941/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CProjectileInfo_RemoveProjectile.txt](../World355Evidence/CProjectileInfo_RemoveProjectile.txt), [CProjectileInfo_RemoveAllProjectiles.txt](../World355Evidence/CProjectileInfo_RemoveAllProjectiles.txt), [CProjectileInfo_RemoveIfThisIsAProjectile.txt](../World355Evidence/CProjectileInfo_RemoveIfThisIsAProjectile.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
