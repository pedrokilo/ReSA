<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Efectos, objetos procedurales y tareas de interior (lista de 8)

Validadas el 02/10/2026 con **15.552 pares original/candidato dentro de GTA**, todos exactos: 40 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `Fx_c` | `TriggerWaterSplash`, `TriggerBulletSplash`, `TriggerFootSplash`, `RenderBegin`, `RenderAddTri_`, `RenderEnd`, `RotateVecIntoVec`, `RotateVecAboutVec` |
| `ProcObjectMan_c` | `Init`, `Update`, `Exit`, `LoadDataFile`, `GetEntityFromPool`, `ReturnEntityToPool`, `ProcessTriangleAdded`, `ProcessTriangleRemoved` |
| `CTaskInteriorBeInHouse` | Constructor, Destructor, `GetInfoForPedToUse`, `Clone`, `GetTaskType`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask` |
| `CTaskInteriorBeInOffice` | Constructor, Destructor, `GetInfoForPedToUse`, `Clone`, `GetTaskType`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask` |
| `CTaskInteriorUseInfo` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask` |

**Diferencias reales corregidas:**
- `Fx_c`:
  - `Trigger*Splash`: distancia (cámara - pos) sumada x² + z² + y² y descarte solo si es > 625 (con NaN se crea el efecto);
  - `RenderAddTri_`:
    - el color se compone con los enteros completos ((((a << 8 | r) << 8 | g) << 8) | b), sin recortar a bytes;
    - umbrales con signo y vaciado en línea sin comprobar que haya triángulos;
  - `RotateVecIntoVec` usa la referencia del EXE (0,4243; 0,5657; 0,7071), no (3, 4, 5) normalizado;
  - `RotateVecAboutVec` sigue el orden de productos y sumas del EXE (p. ej. (x·x)·t + c, (z·x)·t).
- `ProcObjectMan_c`:
  - `Init` devuelve `true`;
  - `ProcessTriangleAdded` devuelve solo el byte bajo de la suma (los llamadores prueban `al`);
  - `Exit`, `ProcessTriangleAdded` y `ProcessTriangleRemoved` recorren con el contador con signo (antes `std::span` con `size_t`);
  - `LoadDataFile` usa `sscanf` sin comprobar campos, buffers sin inicializar y pasa el byte de `align` sin normalizar.
- `CTaskInteriorBeInHouse`:
  - `GetInfoForPedToUse`:
    - borra `m_IntInfo` antes de buscar;
    - la duración solo se escribe en cada rama, y la de UNK_5 (con su aleatorio) aunque no se encuentre la info;
    - aleatorios en línea (`rand()&0xFFFF` × 1/32768 × 100, `rand() < 0x3FFF`);
  - `CreateNextSubTask` marca el bit 7 de 0x474 del peatón y crea la `UseInfo` no instantánea (antes llamaba a `CreateFirstSubTask`).
- `CTaskInteriorBeInOffice`:
  - `GetInfoForPedToUse` es `__thiscall` de un solo argumento (`ret 4`; la DLL tenía dos) y calcula la duración dentro del bucle con los aleatorios del EXE;
  - `CreateNextSubTask` marca el bit 7 de 0x474 y crea la `UseInfo` no instantánea (la DLL pasaba siempre `true`).
- `CTaskInteriorUseInfo`: el destructor comprueba `m_IntInfo` nulo como el EXE (0x675A93).

**Método:**
- Objetos falsos para todo; nunca se crea un sistema de efectos ni un objeto real.
- `Fx_c`:
  - la cámara recibe una matriz/posición propias durante la llamada;
  - se simulan `CreateFxSystem`/`PlayAndKill`, `FxRwMatrixCreate`/`Destroy`, `fpRenderStateGet`/`Set` y `RwIm3DTransform`/`RenderPrimitive`/`End`;
  - el buffer temporal de vértices (0xC4D958) y los campos de `g_fx` se guardan y restauran.
- `ProcObjectMan_c`:
  - el gestor es un buffer propio con listas y entidades falsas (vtable trazada);
  - la lista global 0xBBC8B8 se cambia por una vacía y se restaura;
  - se simulan `procobj.dat` (OpenFile/LoadLine/CloseFile), `ProcSurfaceInfo_c::Init`/`Exit`/`AddObjects` y `CWorld::Remove`.
- Tareas:
  - peatón, inteligencia, interiores, infos y subtareas falsos;
  - se simulan `FindInteriorInfo`, `GetPed`, `AreAnimsLoaded`, `Say` y el RNG;
  - las tareas creadas se buscan en el pool y se borran.
- Se guarda y se restaura todo el estado global tocado.

**Desviaciones mantenidas:**
- Ninguna en estas clases. Fuera de los fixtures: `m_bRghtHandSide` (+0x10) de las `CTaskInteriorLieInBed` creadas por `UseInfo` se enmascara (FIX_BUGS lo escribe; el EXE no).

Repetir: `python reversing/Fx8/build_probe.py` y `python reversing/run.py --case fx8`. Evidencia: [validation-summary.json](validation-summary.json).
