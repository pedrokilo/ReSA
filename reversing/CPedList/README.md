<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPedList - 7/7

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Cuatro funciones reconstruidas: BuildListFromGroup_NotInCar_NoLeader, BuildListOfPedsOfPedType, RemovePedsAttackingPedType y RemovePedsThatDontListenToPlayer. 4.096 pares originales/candidatos dentro de GTA: 1.024 por funcion, igualdad exacta de toda la lista y trazas de consultas. Entradas sin mutacion; pool global, jugador, hooks, partidas y ASI restaurados.

Se conserva el recorrido inverso del pool, limite de 30, exclusion del lider y compactacion estable. RemovePedsAttackingPedType, pese al nombre, conserva solo quienes atacan al tipo indicado. Se captura el numero original antes de eliminar para no saltar miembros.

PROBAR.cmd --case pedlist o --all. El pool y grupo son fixtures aislados; consultas de inteligencia controladas, no se ejecuta toda la IA. Esta evidencia no garantiza estabilidad general ni prueba combinada del ASI actual. Detalles y hashes en validation-summary.json. Compilacion normal conjunta instalada solo en la copia de pruebas; respaldo y hashes de partidas verificados.
