<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InvalidateNonVisibleEvents: banco preparado

2026-10-08. No compilado ni ejecutado. Identidad `CInterestingEvents::InvalidateNonVisibleEvents`, `0x006029C0`.

384 pares con fixtures locales, mÃ¡scara de slots ocupados, cÃ¡maras 0..2, entidades con placement/matriz, LOS visible/oculto y selecciÃ³n activa/inactiva. ComparaciÃ³n byte a byte de estructura y entidad, mÃ¡s argumentos y resultado de LOS y limpieza de referencias.

Las dos rutas usan los mismos colaboradores aislados mediante parches de las entradas EXE y DLL de LOS/CleanUpOldReference. Se comprueba restauraciÃ³n de los cuatro parches, estructura completa de TheCamera, ambas entradas del hook y flags/candados de todos los hooks. NingÃºn estado de restauraciÃ³n se fija a verdadero por conveniencia.

Disparidad esperada antes de corregir: candidato conserva `event.entity`, EXE lo pone a cero. Conservar el replay fallido antes de cualquier modificaciÃ³n. REA nuevo, compilaciÃ³n con seis procesos, manifiesto de fuentes/instrumentos y ejecuciÃ³n los coordina root. No interpretar este banco aislado como prueba integral de CWorld/Entity/CCamera.

## Approved menu replay, 2026-10-08

InvalidateNonVisibleEvents passed 384 pairs after correction, with original/candidate entries and collaborators symmetrically isolated. [Frozen enriched summary](../../RequestedMenuBatch/runs/visibility/20261008-112224-200531/enriched-summary.json). LOS and reference traces certify the declared fixture contract, not real-world integration. dependency_review remains null. Earlier failed replays are preserved.
