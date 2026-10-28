<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# C_PcSave

Validadas el 02/10/2026 con **832 pares original/candidato dentro de GTA**, todos exactos: las 5 funciones de `C_PcSave` con `validated=1` (`SetSaveDirectory`, `GenerateGameFilename`, `PopulateSlotInfo`, `SaveSlot`, `DeleteSlot`).

**Diferencias reales corregidas:**
- `SaveSlot`: el original llama a `InitUserDirectories` (0x744FB0), no a `CFileMgr::SetDirMyDocuments`, y pasa el slot como `int8`.
- `DeleteSlot`: arma la ruta con `"%s%i.b"`, borra con `DeleteFileA` y devuelve `true` solo si después el archivo ya no se abre (no usa `std::filesystem::remove`).
- `PopulateSlotInfo`: un slot corrupto sigue como ocupado (sin el estado NOTSA `SLOT_CORRUPTED`) y el aviso `FEC_SLC` lleva el número de slot en base 1. La comparación con `TopLineEmptyFile` es `strncmp` de 26 caracteres. El mes pasa por la conversión GXT→ASCII de 0x69F8C0 (búfer 0xC1BED0), y con un mes fuera de rango se reutiliza el texto anterior.

**Método:** partidas falsas en una carpeta temporal del juego (`pcsave_bench_tmp`). `GenericSave`, `MakeValidSaveName`, `DoGameSpecificStuffBeforeSave`, `InitUserDirectories` y `CheckDataNotCorrupt` se simulan y se trazan. Los archivos truncados conservan siempre la fecha, porque leer más allá del final deja pila sin inicializar.

Repetir: `python reversing/PcSave/build_probe.py` y `python reversing/run.py --case pcsavebatch`. Evidencia: [validation-summary.json](validation-summary.json).
