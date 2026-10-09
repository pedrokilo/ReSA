<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RemainingLeafPair V2:19152 pares

Extensión separada del corpus9936 preservado. Mantiene CHEAT_NO_ONE_CAN_STOP_US77 como bit independiente y añade CHEAT_INFINITE_HEALTH61. El modelo lee direcciones nativas61/67 y replica el remainder no redondeado de caída. [Diagnóstico y límites](NATIVE_DIAG.md).

Builder/Preflight/runner mantienen la captura y restauración de entradas52B, flags, vtables originales/DLL y timer. El analizador exige720+18432 fixtures/pairs únicos y rechaza diferencias, falta de modelo o restauración. Gzip determinístico preserva logs grandes sin pérdida. El patch externo fue aplicado por autorización del coordinador; la fuente continúa sin aprobación hasta el replay19152. Los manifests de compilación sólo se crearán al compilar realmente.
