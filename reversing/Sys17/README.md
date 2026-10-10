<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de ficheros y cadenas IK (lista de 17)

Validadas el 03/10/2026 con **13.248 pares original/candidato dentro de GTA**, todos exactos: 34 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CFileMgr` | `Initialise`, `ChangeDir`, `SetDir`, `SetDirMyDocuments`, `InitUserDirectories`, `LoadFile`, `OpenFile`, `OpenFileForWriting`, `OpenFileForAppending`, `Read`, `Write`, `Seek`, `ReadLine`, `CloseFile`, `GetTotalSize`, `Tell`, `GetErrorReadWrite` |
| `IKChainManager_c` | `Init`, `Exit`, `Reset`, `Update`, `AddIKChain`, `RemoveIKChain`, `CanAccept`, `CanAcceptLookAt`, `LookAt`, `PointArm`, `IsLooking`, `GetLookAtEntity`, `GetLookAtOffset`, `AbortLookAt`, `IsArmPointing`, `AbortPointArm`, `IsFacingTarget` |

**Diferencias reales corregidas:**
- `CFileMgr`:
  - `Initialise`, `ChangeDir` y `SetDir` no limpian los buferes de carpeta: `strcpy`/`strcat` como el EXE (una ruta relativa se añade a la carpeta actual);
  - la barra final se decide por el último carácter de la ruta pasada y se añade con `strcat` (barra y terminador);
  - `strcpy` en vez de `strcpy_s`/`strcat_s` (en depuración rellenaban con 0xFE);
  - `LoadFile` ignora el tamaño, lee en bloques de 0x4000 y termina el búfer con un 0;
  - `OpenFile` usa `fopen` compartido (no `fopen_s`, que abre en exclusiva);
  - `GetErrorReadWrite` devuelve `_flag & _IOERR` (0x20) en EAX completo, no un `bool`.
- `IKChainManager_c`:
  - `CanAccept` comprueba `m_pRwObject`, mira el modo francotirador antes del alcance y acepta con NaN (`!(r < 999)`, `!(r² < d²)`);
  - `LookAt` lee `byte_C1542C` solo tras aceptar y vuelve a leer la tarea gestora después de `SetTaskSecondary`;
  - las tareas de los huecos IK se usan sin comprobar su tipo (sin llamadas a `GetTaskType`), también en `AbortPointArm`.
- `CTaskSimpleIKChain` (usada por las anteriores): `ProcessPed` caduca solo con duración, trunca el paso en ms y divide con `fild/fidiv`; el hueso de desplazamiento se guarda y pasa como dword.

**Método:**
- `CFileMgr`, carpetas: `_getcwd`/`_chdir` del EXE y `_getcwd`/`_wgetcwd`/`_chdir`/`_wchdir` de la DLL simulados con traza; nunca se cambia la carpeta real.
- `InitUserDirectories`: registro (`Shell Folders\Personal`) en el EXE y `SHGetFolderPathW` en la DLL devuelven la misma ruta; `CreateFile`/`CreateDirectory`/`CloseHandle` simulados parcheando la IAT del EXE y del ASI solo durante la llamada (otros hilos pasan a la función real).
- E/S sobre ficheros temporales en `artifacts/fmtmp`: el original recibe un `FILE*` del CRT del EXE y el candidato uno del CRT de la DLL; se comparan retornos, buferes, posición, EOF/error y el contenido final en disco.
- `IKChainManager_c`: gestor falso con 32 cadenas en listas coherentes, peatón con inteligencia y tareas IK falsas, entidad objetivo y arena para `CTask::operator new`; se simulan `IsAlive`, `GetIsOnScreen`, `UpdateRpHAnim`, `ReturnBoneNode`, `IKChain_c::Init/Update/Exit/IsFacingTarget`, `RpAnimBlendClumpGetAssociation`, `BlendOut`, `UpdateLookAtInfo`, `UpdatePointArmInfo`, `AddIKChainTask` y `operator delete[]`.
- Se guardan y restauran `ms_dirName`, `ms_rootDirName`, las carpetas de usuario, el código de búsqueda, el modo de cámara, `byte_C1542C` y las referencias.
- Fuera de los fixtures:
  - la caché NOTSA de `InitUserDirectories` (se prueba con el primer byte a 0) y rutas largas (las comprobaciones de longitud NOTSA);
  - rutas de carpeta de más de 128 bytes (el EXE desborda);
  - modos de `fopen` inválidos, `fgets` con tamaño < 2, `CloseFile(nullptr)` y leer de flujos de solo escritura (aserciones del CRT de depuración o diferencias entre CRT);
  - posición en modo texto o anexar;
  - código de búsqueda en 0xFFFF (`CWorld::ClearScanCodes` es protegida);
  - `AbortLookAt`/`AbortPointArm` sin tarea gestora (el EXE se cae).

Repetir: `python reversing/Sys17/build_probe.py` y `python reversing/run.py --case sys17`. Evidencia: [validation-summary.json](validation-summary.json).
