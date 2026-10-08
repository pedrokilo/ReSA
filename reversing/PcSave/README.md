<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# C_PcSave

Validadas el 02/10/2026 con **832 pares original/candidato dentro de GTA**, todos exactos: las 5 funciones de `C_PcSave` con `validated=1` (`SetSaveDirectory`, `GenerateGameFilename`, `PopulateSlotInfo`, `SaveSlot`, `DeleteSlot`).

**Diferencias reales corregidas:**
- `SaveSlot`: el original llama a `InitUserDirectories` (0x744FB0), no a `CFileMgr::SetDirMyDocuments`, y pasa el slot como `int8`.
- `DeleteSlot`: arma la ruta con `"%s%i.b"`, borra con `DeleteFileA` y devuelve `true` solo si después el archivo ya no se abre (no usa `std::filesystem::remove`).
- `PopulateSlotInfo`: un slot corrupto sigue como ocupado (sin el estado NOTSA `SLOT_CORRUPTED`) y el aviso `FEC_SLC` lleva el número de slot en base 1. La comparación con `TopLineEmptyFile` es `strncmp` de 26 caracteres. El mes pasa por la conversión GXT→ASCII de 0x69F8C0 (búfer 0xC1BED0), y con un mes fuera de rango se reutiliza el texto anterior.

**Método:** partidas falsas en una carpeta temporal del juego (`pcsave_bench_tmp`). `GenericSave`, `MakeValidSaveName`, `DoGameSpecificStuffBeforeSave`, `InitUserDirectories` y `CheckDataNotCorrupt` se simulan y se trazan. Los archivos truncados conservan siempre la fecha, porque leer más allá del final deja pila sin inicializar.

Repetir: `python reversing/PcSave/build_probe.py` y `python reversing/run.py --case pcsavebatch`. Evidencia: [validation-summary.json](validation-summary.json).
