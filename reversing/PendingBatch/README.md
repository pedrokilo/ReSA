<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
