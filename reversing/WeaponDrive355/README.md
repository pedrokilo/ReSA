<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WeaponDrive355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CWeapon::DoDriveByAutoAiming`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-drive355/20261008-042058-950566/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# WeaponDrive355

DoDriveByAutoAiming,1024pares. Reconstruida desde combo REA/asm. Se sustituye búsqueda de entidades, distancia a línea y ángulo de autoaim mediante stubs pareados. Selección y reajuste del vector ejecutan código original/candidato. Fixtures no modifican mundo real: entidades vacías, owner nulo, peds muertos/vivos/en vehículo, vehículos, objetivos delante/detrás y tipos helicóptero/avión/terrestre. Compara vector y estado completo, trazas y switches. Pendiente de compilar y validar por coordinador; posibles diferencias de redondeo x87 se resolverán desde resultados, sin declararlo aprobado antes.

Snapshot de objetos completos mediante FNV-1a de 64 bits; outputs, punteros y trazas explícitos permanecen exactos. Las aprobaciones anteriores requieren repetición con este banco actualizado.

Primera ejecución: 62/1024 diferencias en trazas. ASM 0x73D839 compara el campo físico +0xFC (`m_pAttachedTo`), no `m_pVehicle`. Corregido y ampliado fixture para variar ambos campos de manera independiente; requiere repetición.

## Evidencia y cierre del lote 355

Último informe real: `20261008-042058-950566`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-drive355/20261008-042058-950566/summary.json>).

Última aprobación histórica: `20261008-042058-950566`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-drive355/20261008-042058-950566/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CWeapon_DoDriveByAutoAiming.txt](../World355Evidence/CWeapon_DoDriveByAutoAiming.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
