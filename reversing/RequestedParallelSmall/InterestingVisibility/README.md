<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InvalidateNonVisibleEvents: banco preparado

2026-10-08. No compilado ni ejecutado. Identidad `CInterestingEvents::InvalidateNonVisibleEvents`, `0x006029C0`.

384 pares con fixtures locales, mÃ¡scara de slots ocupados, cÃ¡maras 0..2, entidades con placement/matriz, LOS visible/oculto y selecciÃ³n activa/inactiva. ComparaciÃ³n byte a byte de estructura y entidad, mÃ¡s argumentos y resultado de LOS y limpieza de referencias.

Las dos rutas usan los mismos colaboradores aislados mediante parches de las entradas EXE y DLL de LOS/CleanUpOldReference. Se comprueba restauraciÃ³n de los cuatro parches, estructura completa de TheCamera, ambas entradas del hook y flags/candados de todos los hooks. NingÃºn estado de restauraciÃ³n se fija a verdadero por conveniencia.

Disparidad esperada antes de corregir: candidato conserva `event.entity`, EXE lo pone a cero. Conservar el replay fallido antes de cualquier modificaciÃ³n. REA nuevo, compilaciÃ³n con seis procesos, manifiesto de fuentes/instrumentos y ejecuciÃ³n los coordina root. No interpretar este banco aislado como prueba integral de CWorld/Entity/CCamera.

## Approved menu replay, 2026-10-08

InvalidateNonVisibleEvents passed 384 pairs after correction, with original/candidate entries and collaborators symmetrically isolated. [Frozen enriched summary](../../RequestedMenuBatch/runs/visibility/20261008-112224-200531/enriched-summary.json). LOS and reference traces certify the declared fixture contract, not real-world integration. dependency_review remains null. Earlier failed replays are preserved.
