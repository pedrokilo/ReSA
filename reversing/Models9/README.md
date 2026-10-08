<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Modelos atómicos y de peatón, colisión, carteles y textos (lista de 9)

Validadas el 02/10/2026 con **14.400 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAtomicModelInfo` | `AsAtomicModelInfoPtr`, `GetModelType`, `Init`, `DeleteRwObject`, `GetRwModelType`, `SetAtomic` (VMT), `GetAtomicFromDistance`, `SetupVehicleUpgradeFlags`, `SetAtomicModelInfoFlags` |
| `CColModel` | `operator new`, `operator delete`, `operator=`, `MakeMultipleAlloc`, `AllocateData` (vacío y con parámetros), `RemoveCollisionVolumes`, `CalculateTrianglePlanes`, `RemoveTrianglePlanes` |
| `CCustomRoadsignMgr` | `Initialise`, `Shutdown`, `RoadsignGenerateTextRaster`, `CreateRoadsignTexture`, `SetupRoadsignAtomic`, `SetAtomicAlpha`, `CreateRoadsignAtomicA`, `CreateRoadsignAtomic`, `RenderRoadsignAtomic` |
| `CPedModelInfo` | `GetModelType`, `DeleteRwObject`, `SetClump` (VMT), `AddXtraAtomics`, `SetFaceTexture`, `CreateHitColModelSkinned`, `AnimatePedColModelSkinned`, `AnimatePedColModelSkinnedWorld`, `IncrementVoice` |
| `CText` | Constructor, Destructor, `Unload`, `Get`, `GetNameOfLoadedMissionText`, `ReadChunkHeader`, `Load`, `LoadMissionText`, `LoadMissionPackText` |

**Diferencias reales corregidas:**
- `CAtomicModelInfo`:
  - `GetAtomicFromDistance`: `test ah,41h`, una distancia o un producto NaN devuelve nulo;
  - `SetupVehicleUpgradeFlags` limpia el byte alto de los flags antes de buscar y el prefijo es `"wg_r_"` (faltaba el guion bajo);
  - `SetAtomicModelInfoFlags` aplica etiqueta, estatua y por último el tipo desconocido (gana el desconocido);
  - `SetAtomic`/`DeleteRwObject` llaman dos veces a `GetAnimFileIndex` por la vtable.
- `CColModel`:
  - `operator delete` no rellena el slot con 0xDD (`CColModelPool::Delete` usa la política del EXE);
  - `MakeMultipleAlloc` libera el bloque único con `CMemoryMgr::Free`;
  - `operator=` sin datos en destino no hace nada (quitado el `__debugbreak`);
  - el destructor (inlined en `CPedModelInfo::DeleteRwObject`) no anula `m_pColData`.
- `CCustomRoadsignMgr`:
  - `RoadsignGenerateTextRaster` toma el carácter en `(fila·64 + col)·32` bytes (sin usar el stride real) y avanza en bytes;
  - `CreateRoadsignAtomicA` suma los valores base (·0,0) como el EXE: ceros con signo y NaN iguales;
  - `CreateRoadsignAtomic` usa un `" "` de 16 bytes terminado en cero (antes un único carácter sin terminar);
  - `RenderRoadsignAtomic`: distancia `(z²+y²)+x²` y alfa `(250000−d²)·(1/248400)·254` truncado;
  - `Initialise`/`Shutdown`/`CreateRoadsignTexture`: orden de llamadas y `strncpy`.
- `CPedModelInfo::SetClump` pasa el clump del parámetro a `GetAnimHierarchyFromClump`.
- `CText`:
  - `LoadMissionPackText` espera las dos tablas (`||`), actualiza las claves de misión, comprueba las lecturas y no cierra el fichero;
  - `LoadMissionText` empieza el desplazamiento en 8, vuelve si la misión no existe y copia el nombre con `strcpy`;
  - `Load` y `LoadMissionText` saltan bloques leyendo byte a byte; `Load` consulta `TheText` y convierte con `GxtCharToAscii` (0x69F7E0);
  - `ReadChunkHeader` lee byte a byte (y respeta `unknown`);
  - constructor sin `Unload`, 8 bytes del nombre y 25 de `GxtErrorString`; `Unload` limpia los 8 bytes; `GetNameOfLoadedMissionText` usa `strcpy`.

**Método:**
- Model infos falsos con la vtable del EXE (en `CAtomicModelInfo`, copia con `GetAnimFileIndex` trazado).
- Objetos RW falsos; las primitivas RW de creación, destrucción y registro se simulan. Las matemáticas RW corren de verdad sobre matrices propias (se cambian `CGame::m_pWorkingMatrix1/2`).
- Pool de `CColModel` falso (puntero 0xB744A4); `operator new/delete` del EXE y `CMemoryMgr` van a arenas fijas; trazas sin reservas.
- `CText` trabaja sobre `TheText` intercambiada con ficheros GXT falsos en memoria; se guardan y restauran `TheText`, `GxtErrorString`, el buffer ASCII, el idioma y `bMissionPackGame`.
- Fuera de los fixtures:
  - `GxtErrorString[1..24]` (el EXE copia pila sin inicializar en `Get`);
  - `CheckFileEncoding` (NOTSA): siempre codificación de 8 bits;
  - idiomas fuera de 0..4 (`GetGxtName` NOTSA);
  - `Load` con paquete de misión pendiente (bucle de espera del menú);
  - ficheros GXT truncados en `Load`/`LoadMissionText` (el EXE se queda en bucle).

Repetir: `python reversing/Models9/build_probe.py` y `python reversing/run.py --case models9`. Evidencia: [validation-summary.json](validation-summary.json).
