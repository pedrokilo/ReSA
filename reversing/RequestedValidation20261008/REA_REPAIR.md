<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
