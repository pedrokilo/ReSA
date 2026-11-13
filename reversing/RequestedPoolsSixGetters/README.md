<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 901 grupos / 10540 funciones reconstruidas; 10540 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedPoolsSixGetters

## Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Estado: **NO repetible; sustituido por `RequestedPoolsSixGettersV2`** (V1 fue rechazado en octubre: ver `RequestedPoolsSixGettersV2/Diagnostics/v1-failure-audit.json`). Con `VTableRanges.hpp` reparado la corrida `20261110-130045-445428` vuelve a dar FALLO (`error` vacio, sin comparaciones validas), como en octubre. Los seis getters los cubre V2 (evidencia de octubre; su repeticion queda pendiente, ver su README).
