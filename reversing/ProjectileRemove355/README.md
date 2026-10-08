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
