# CPedList - 7/7

Cuatro funciones reconstruidas: BuildListFromGroup_NotInCar_NoLeader, BuildListOfPedsOfPedType, RemovePedsAttackingPedType y RemovePedsThatDontListenToPlayer. 4.096 pares originales/candidatos dentro de GTA: 1.024 por funcion, igualdad exacta de toda la lista y trazas de consultas. Entradas sin mutacion; pool global, jugador, hooks, partidas y ASI restaurados.

Se conserva el recorrido inverso del pool, limite de 30, exclusion del lider y compactacion estable. RemovePedsAttackingPedType, pese al nombre, conserva solo quienes atacan al tipo indicado. Se captura el numero original antes de eliminar para no saltar miembros.

PROBAR.cmd --case pedlist o --all. El pool y grupo son fixtures aislados; consultas de inteligencia controladas, no se ejecuta toda la IA. Esta evidencia no garantiza estabilidad general ni prueba combinada del ASI actual. Detalles y hashes en validation-summary.json. Compilacion normal conjunta instalada solo en la copia de pruebas; respaldo y hashes de partidas verificados.
