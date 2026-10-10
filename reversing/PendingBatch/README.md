<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7809 con evidencia histórica, 0 con vigencia acreditada y 637 pendientes en 9 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote de 78 funciones pendientes

Alcance solicitado: las 78 entradas pendientes de 27 clases enumeradas en [scope.json](scope.json). Los métodos ya reconstruidos de esas clases no se cuentan como nuevas validaciones de este lote.

La reconstrucción contrasta el EXE 1.0 US con pseudocódigo REA y ensamblador completo. Cuando REA no reconoce una función, el banco declara esa limitación y utiliza el ensamblador. Cada prueba alterna el hook original/candidato y compara retornos, estado y efectos sobre fixtures controlados. Las dependencias aisladas y ramas excluidas se describen en el README de cada banco.

Hay 30 bancos: ProstituteSolicit, KillPedOnFootArmed y LineUpPedWithCar se dividen en dos bancos cada una. El lote exige cobertura de las 78 identidades exactas, sin duplicados. [register.py](register.py) comprueba los últimos resultados y los binarios archivados antes de incorporar evidencia al registro y casos a la suite. Rechaza bancos fallidos o recompilados después de su resultado.

Los resultados finales se conservan en [summary.json](summary.json); cada banco tiene su `validation-summary.json`, manifiesto de compilación y enlace al informe completo. La ausencia del resumen final significa que todavía no se ha registrado el lote completo.

La validación certifica el alcance de los fixtures, no independencia del EXE ni una campaña integral de estabilidad. Los runners preservan ASI/PDB, configuración y partidas y cierran GTA al terminar. Las sondas se compilan en serie con CL=/MP6; no deben ejecutarse dos bancos ni dos compilaciones simultáneamente.

El lote registrado aporta **78 aprobaciones y 81.664 pares exactos**, en 27 clases. El catálogo resultante contiene 8.040 entradas reconstruidas, 6.329 con evidencia vinculada y 407 pendientes. La repetición final y los bancos anteriores se registran por separado; repetir un banco no suma funciones nuevas.

## Cierre de la entrega

Los 30 bancos se repitieron sobre las fuentes finales y aprobaron los 81.664 pares. La [regresión anterior](regression-summary.json) aprobó otros 9.984 pares en cinco bancos: CTaskDead, IdleAttractorPending, FightingControlPending, PlayerFighterPending y RoadBlockPending. Las 78 evidencias coinciden con las fuentes archivadas.

Compilación normal sin sondas completada con CL=/MP6; [log](normal-build.log). El auditor estricto no encontró diferencias fuente/catálogo/runtime; las 19 pruebas de contrib pasaron. GTA quedó cerrado, con partidas y configuración restauradas.
