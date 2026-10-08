<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# GangJoin355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CTaskComplexGangJoinRespond::Clone`, `CTaskComplexGangJoinRespond::MakeAbortable`, `CTaskComplexGangJoinRespond::CreateNextSubTask`, `CTaskComplexGangJoinRespond::CreateFirstSubTask`, `CTaskComplexGangJoinRespond::ControlSubTask`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-gang-join355/20261008-073907-648304/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskComplexGangJoinRespond: cinco pendientes

Banco diferencial contra GTA SA 1.0 US: Clone256, MakeAbortable256, Next512, First256 y Control512, 1.792 pares previstos. Aún pendiente de compilación y ejecución.

Fuentes reconstruidas desde los combos en ../Tasks355Evidence. Clone conserva los bytes sin escribir de m_attempts; First lo inicializa. Next conserva el límite firmado de diez intentos y las animaciones0x132/0x133. Control orienta al jugador y carga, referencia o libera el bloque gangs.

Se aíslan asignación de tareas, referencias, jugador, carga de animaciones y solicitudes de streaming mediante trazas de argumentos. Se comparan tareas, asignaciones y ped completos salvo el puntero virtual; no se ejecuta la reproducción real de animaciones ni streaming. Estado global del bloque restaurado entre funciones. Los constructores auxiliares son dependencias, no acreditaciones adicionales.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261007-203203-483629`, 1792 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-gang-join355/20261007-203203-483629/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261007-203203-483629`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-gang-join355/20261007-203203-483629/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
