<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7809 con evidencia histórica, 0 con vigencia acreditada y 637 pendientes en 9 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CRoadBlocks: CreateRoadBlockBetween2Points

Reconstrucción de `0x4619C0..0x4629D5`, con combo Ghidra 12.1.4 vía REA y ensamblador exacto. La evidencia completa está en `evidence/combo-4619C0.txt`.

El banco contiene 1024 pares original/candidato, selecciona el hook por categoría y comprueba el cambio tras activarlo. Simula en EXE y DLL jugador, wanted, selección de modelos, RNG, espacio, suelo, constructores, asignación desde pools, daño, física, alpha, mundo y fuego. Usa 16 entidades y matrices falsas; no crea entidades reales ni utiliza los pools reales. La vtable falsa cubre destructor, static, altura, puertas y explosión. Compara trazas, bytes completos de entidades, ejes y posición de matrices y estado del fuego. Los cuatro padding dword de la matriz temporal están excluidos porque el EXE no los inicializa.

Casos: bloqueo policial/gang, modelos no disponibles, copbike, geometría nula/vertical/oblicua, lados del jugador, cero a cinco vehículos y cero a ocho barreras, suelo/espacio aceptado y rechazado, entidad temporal incendiada, coche dañado/explotado, rechazo por inclinación y apertura de puertas. Se restauran tabla de modelos, streaming, tiempo, contador de temporales, índices de barrera/coche y estados de hooks; los parches se restauran por RAII.

Los registros passing incluyen FNV-1a de 64 bits (offset basis 14695981039346656037, prime 1099511628211) sobre el UTF-8 de `json.dump()` completo: bytes de entidades, matrices, fuegos, trazas, contador y switch. El banco compara además los json completos antes de aceptar. El gate final exige contadores positivos de constructor de coche/objeto, fuego, puertas, destructor, rechazo de espacio, rechazo gang, salida gang con modelo -1 y salida copbike.

La resolución por export exacto/PDB de los dos constructores DLL es un requisito comprobado antes de ejecutar. Si no resuelven los nombres decorados, el banco registra un error con `state_restored=true`; el responsable de compilación debe ajustar el nombre al PDB. Los modelos y constructor de entidades son simulaciones, no validan sus implementaciones ni integración real con física/render.

**Validado el 07/10/2026: 1.024/1.024 pares exactos**, nueve ramas exigidas con cobertura positiva y estado, configuración y partidas restaurados. [Evidencia](validation-summary.json). Compilación con `/MP6`.

<!-- BEGIN PENDING BATCH REGRESSION -->

Regresión del 07/10/2026: **APROBADA**. 1.024 pares exactos. [Resumen actualizado](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-roadblock-pending\20261007-201658-292020/summary.json>).
<!-- END PENDING BATCH REGRESSION -->
