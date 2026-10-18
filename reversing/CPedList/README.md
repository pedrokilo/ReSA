<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPedList - 7/7

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Cuatro funciones reconstruidas: BuildListFromGroup_NotInCar_NoLeader, BuildListOfPedsOfPedType, RemovePedsAttackingPedType y RemovePedsThatDontListenToPlayer. 4.096 pares originales/candidatos dentro de GTA: 1.024 por funcion, igualdad exacta de toda la lista y trazas de consultas. Entradas sin mutacion; pool global, jugador, hooks, partidas y ASI restaurados.

Se conserva el recorrido inverso del pool, limite de 30, exclusion del lider y compactacion estable. RemovePedsAttackingPedType, pese al nombre, conserva solo quienes atacan al tipo indicado. Se captura el numero original antes de eliminar para no saltar miembros.

PROBAR.cmd --case pedlist o --all. El pool y grupo son fixtures aislados; consultas de inteligencia controladas, no se ejecuta toda la IA. Esta evidencia no garantiza estabilidad general ni prueba combinada del ASI actual. Detalles y hashes en validation-summary.json. Compilacion normal conjunta instalada solo en la copia de pruebas; respaldo y hashes de partidas verificados.
