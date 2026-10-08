<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Reparación prioritaria del proveedor REA

El usuario pidió corregir primero la admisión de GTA San Andreas x86 el 8 de octubre de 2026. Se modificó el proveedor local configurado en Codex, sin cambiar la configuración ni el EXE.

La nueva extensión admite el EXE canónico SHA-256 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b y conserva GTA IV x86 y aplicaciones x64. Los checks restantes de formato, rol, código nativo, hash y snapshots siguen activos.

Prueba real aprobada con una conexión MCP nueva: importación Ghidra 12.1.4, bytes exactos, pseudocódigo y ensamblador de CDecision::From, análisis de WinMain, cierre limpio y EXE intacto. Esta evidencia verifica el proveedor; **no es validación de las funciones C++ del lote de 1.710** y no modifica hooks.csv ni el registro.

Informe: `C:/Users/pedro/Tools/ReIV-analysis/REA-SA-X86.md`.
Resultado: `C:/Users/pedro/Tools/ReIV-analysis/evidence-sa-x86/verification-summary.json`.
Fuentes/evidencia: `C:/Users/pedro/Tools/ReIV-analysis/evidence-sa-x86/2026-10-08T16-19-17-621Z/`.
Respaldo: `C:/Users/pedro/Tools/ReIV-analysis/backups/before-sa-x86-20261008/`.

La conexión del chat iniciada antes de la reparación mantiene el módulo antiguo en memoria; se retesteó y conservó ese rechazo. Debe reconectarse REA o reiniciarse Codex para usar la corrección desde esa conexión. El ejecutor local verify-sa.mjs funciona con una conexión nueva sin cambiar config.toml. No se aplicó el setup genérico de upstream.

La consulta de GetTaskType (0x6218B0) produjo invalid_request después de la importación y lectura de bytes correctas. Ese intento se conserva; no se afirma reconocimiento automático de todos los entrypoints. La disponibilidad de pseudocódigo está comprobada con From y WinMain.

El inventario de validación anterior permanece pendiente: 77 clases, 1.710 identidades iniciales sin evidencia vinculada. No hubo comparaciones GTA nuevas, cambios de comportamiento, commit ni push. La compilación Debug normal sin sonda terminó correctamente antes de esta reparación; GTA quedó cerrado.
