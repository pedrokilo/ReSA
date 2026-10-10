<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WeaponOccupant355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CWeapon::CheckForShootingVehicleOccupant`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-occupant355/20261008-055634-486611/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# WeaponOccupant355

CheckForShootingVehicleOccupant (`0x73F480`), 4096 pares EXE/DLL pendientes de ejecución. Firma void corregida desde ensamblador completo `../World355Evidence/CWeapon_CheckForShootingVehicleOccupant.txt`.

Ocupante conductor y ocho pasajeros, flag de elegibilidad, jerarquía/hueso, profundidad compartida, selección final del último impacto válido, ausencia de impacto y copia selectiva de CColPoint sin rellenos. Coche/no coche, dirección vertical, cristal, tres triángulos, progresión doble de parabrisas. Colaboradores de animación/colisión/cristal/daño simétricos; no se genera cristal real. Digest FNV1a64 de vehículo, peds, punto y trace de orden/argumentos; tabla de modelo restaurada. Pares 0..4095.

## Evidencia y cierre del lote 355

Último informe real: `20261008-050124-840035`; no aprobado, pares no informados en el cierre. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-occupant355/20261008-050124-840035/summary.json>).

No hay aprobación histórica en los informes disponibles.

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CWeapon_CheckForShootingVehicleOccupant.txt](../World355Evidence/CWeapon_CheckForShootingVehicleOccupant.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
