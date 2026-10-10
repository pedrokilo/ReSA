<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RenderWater audit

Root delegated the arithmetic/helper audit and WaterRender355 fixture correction after report `20261008-005539-916566` (877 differences). Root separately corrected RW states to FOGENABLE(14,1) and ZWRITE(8,0).

`WaterRenderShifts` follows the exact x87 sequence from `0x6EF8E2` through `0x6EFAF9`, including resident flow products, prescribed float stores, wrapped shifts, first/second trig phases, constants read from the executable, and both ordered random calls. It replaces intermediate `double` spills that could change the original rounding.

The EXE creates four outside-world CRenPar values with floats at bytes0–11 and two flow bytes12–13, leaving bytes14–15 undefined (`0x6EFEF4`, `0x6EFEF8`, and corresponding later constructions). The bank now compares all five semantic fields as exact float bits/integer bytes. It continues to compare complete global ranges, render buffers and indices. It no longer hashes undefined argument padding whose stack contents depend on call layout.

The negative visibility case emits a `visible=false` trace before returning. Coverage now identifies positive rendering from the fixture's visibility value instead of treating that trace as proof of rendering; both visible and invisible cases remain required. No equality tolerances were introduced.

Source and bank were frozen after these changes. Root exclusively compiles and runs GTA; runtime approval must come from the revised report.
