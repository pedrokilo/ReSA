<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarPassenger355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCarEnterExit::MakeUndraggedPassengerPedsLeaveCar`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car-passenger355/20261008-073040-551140/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CarPassenger355

MakeUndraggedPassengerPedsLeaveCar (0x64F540), reconstruida desde combo REA y asm. Banco de 1024 pares con capacidad cero a ocho, huecos, pasajero arrastrado y flag StayInCarOnJack. Compara estado completo y orden de cálculo de puerta, referencias, publicación y limpieza del evento PedEnteredMyVehicle. Constructor/destructor del evento ejecutan su código; se sustituyen referencias y publicación a grupos ficticios. No se producen eventos reales. Bytes de padding de evento en pila no son salidas y no se comparan.

Pendiente de compilación/ejecución por coordinador. Las otras cuatro funciones CCarEnterExit quedan fuera de este banco.

Aprobado 1024 pares, 0 diferencias, informe 20261007-205809-268548.
Snapshot actualizado FNV-1a64 de todos los bytes de objetos; repetir este banco para vigencia nueva.

## Evidencia y cierre del lote 355

Último informe real: `20261007-205809-268548`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car-passenger355/20261007-205809-268548/summary.json>).

Última aprobación histórica: `20261007-205809-268548`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car-passenger355/20261007-205809-268548/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CCarEnterExit_MakeUndraggedPassengerPedsLeaveCar.txt](../World355Evidence/CCarEnterExit_MakeUndraggedPassengerPedsLeaveCar.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
