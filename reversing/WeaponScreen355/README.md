<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WeaponScreen355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CWeapon::FindNearestTargetEntityWithScreenCoors`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-screen355/20261008-021251-487799/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# WeaponScreen355

CWeapon::FindNearestTargetEntityWithScreenCoors — 73E240, 4096 pares pendientes. Evidencia combo REA+ASM: ../World355Evidence/CWeapon_FindNearestTargetEntityWithScreenCoors.txt.

Corrige orden descendente de pools (empates), distancia proyectada con raíz y constante exacta 1/15, comparación NaN y criterio del estado DEAD específico. Fixtures con pools de cuatro personajes/vehículos, slots libres, estados DEAD/DIE/vivo, personaje en vehículo, objetivos elegibles, vehículo del jugador, proyección fallida, tamaños de pantalla 0/1/640/1920, rangos 0/negativos/positivos/NaN, outputs opcionales y cercanía equivalente. Mocks simétricos de elegibilidad, vehículo propio y proyección. Buffers íntegros comparados mediante FNV-1a64, retorno/outputs/traza exactos; pools y resolución restaurados.

python reversing/WeaponScreen355/build_probe.py
python reversing/WeaponScreen355/run_local.py

## Evidencia y cierre del lote 355

Último informe real: `20261008-021251-487799`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-screen355/20261008-021251-487799/summary.json>).

Última aprobación histórica: `20261008-021251-487799`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weapon-screen355/20261008-021251-487799/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CWeapon_FindNearestTargetEntityWithScreenCoors.txt](../World355Evidence/CWeapon_FindNearestTargetEntityWithScreenCoors.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
