<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RenderWater audit

Root delegated the arithmetic/helper audit and WaterRender355 fixture correction after report `20261008-005539-916566` (877 differences). Root separately corrected RW states to FOGENABLE(14,1) and ZWRITE(8,0).

`WaterRenderShifts` follows the exact x87 sequence from `0x6EF8E2` through `0x6EFAF9`, including resident flow products, prescribed float stores, wrapped shifts, first/second trig phases, constants read from the executable, and both ordered random calls. It replaces intermediate `double` spills that could change the original rounding.

The EXE creates four outside-world CRenPar values with floats at bytes0–11 and two flow bytes12–13, leaving bytes14–15 undefined (`0x6EFEF4`, `0x6EFEF8`, and corresponding later constructions). The bank now compares all five semantic fields as exact float bits/integer bytes. It continues to compare complete global ranges, render buffers and indices. It no longer hashes undefined argument padding whose stack contents depend on call layout.

The negative visibility case emits a `visible=false` trace before returning. Coverage now identifies positive rendering from the fixture's visibility value instead of treating that trace as proof of rendering; both visible and invisible cases remain required. No equality tolerances were introduced.

Source and bank were frozen after these changes. Root exclusively compiles and runs GTA; runtime approval must come from the revised report.
