<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Reubicación de evidencias a F: (2026-10-08)

Las cinco carpetas de artefactos se movieron a `F:\ReSA-evidence-archive-20261008`.
Los registros históricos conservan sus rutas y hashes originales; este documento sólo
registra la nueva ubicación y no modifica evidencias ni documentos congelados.

Junctions `reversing/<banco>/artifacts` (reparados; antes apuntaban a
`C:\Users\pedro\Tools\ReSA-evidence-archive-20261008`, que ya no existe):

| Banco | Destino actual | Archivos verificados (SHA-256 vs `*-file-hashes.json`) |
|---|---|---|
| RequestedPreflightAdditionalLifetimes | `F:\ReSA-evidence-archive-20261008\RequestedPreflightAdditionalLifetimes-artifacts` | 34/34 |
| RequestedMenuBatch | `...\RequestedMenuBatch-artifacts` | 12/12 |
| RequestedPreflightTaskCtorDtor | `...\RequestedPreflightTaskCtorDtor-artifacts` | 21/21 |
| RequestedGeometryPlanesV2 | `...\RequestedGeometryPlanesV2-artifacts` | 6/6 |
| RequestedAnimProjectilePair | `...\RequestedAnimProjectilePair-artifacts` | 72/72 |

Los manifiestos `*-file-hashes.json` están en la raíz de `F:\ReSA-evidence-archive-20261008`.
