<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Boat355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CTaskComplexKillPedFromBoat::Clone`, `CTaskComplexKillPedFromBoat::GetTaskType`, `CTaskComplexKillPedFromBoat::CreateNextSubTask`, `CTaskComplexKillPedFromBoat::CreateFirstSubTask`, `CTaskComplexKillPedFromBoat::ControlSubTask`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-boat355/20261008-065747-118703/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskComplexKillPedFromBoat: cinco pendientes

Banco pendiente de ejecuciÃ³n: Clone256, GetTaskType256, Next256, First256, Control1024: 2.048 pares aprobados, sin diferencias, ejecución20261007-203302-929311. Informe: resultados-boat355/20261007-203302-929311/summary.json. Evidencia ASM en ../Tasks355Evidence/Boat_*.txt.

Compara tareas, ped, asignaciones y trazas originales/candidatas. Control cubre ped policía (tipo6), jugador, estados de vehÃ­culo/agua, bÃºsqueda del nivel de bÃºsqueda, aborto y transiciÃ³n entre StandStill y KillPedOnFoot. Se aÃ­slan jugador, wanted, referencias, audio y asignaciÃ³n; no reproduce audio real ni combate. First activa bStayInSamePlace. Constructor auxiliar de KillPedOnFoot es dependencia, no validaciÃ³n adicional.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261007-203302-929311`, 2048 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-boat355/20261007-203302-929311/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261007-203302-929311`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-boat355/20261007-203302-929311/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
