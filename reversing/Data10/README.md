<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Datos de armas, plantillas de efectos y estar en grupo (lista de 10)

Validadas el 02/10/2026 con **8.896 pares original/candidato dentro de GTA**, todos exactos: 30 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CWeaponInfo` | `FindWeaponFireType`, `FindWeaponType`, `GetCrouchReloadAnimationID`, `GetSkillStatIndex`, `GetTargetHeadRange`, `GetWeaponInfo`, `GetWeaponReloadTime`, `Initialise`, `LoadWeaponData`, `Shutdown` |
| `FxSystemBP_c` | Constructor, Destructor, `operator new`, `Load`, `Unload`, `Update`, `Render`, `FreePrtFromSystem`, `GetBoundingSphere`, `SetBoundingSphere` |
| `CTaskComplexBeInGroup` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask`, `MonitorMainGroupTask`, `MonitorSecondaryGroupTask` |

**Diferencias reales corregidas:**
- `CWeaponInfo`:
  - `m_nAimOffsetIndex` es `int16` (el EXE lo lee con `movsx`);
  - `GetWeaponReloadTime` devuelve la primera de `RLoadA`, `CrouchRLoadA`, `RLoadB` que supera 400 tras sumar 100 (no el máximo), leídas sin signo, y si no `CrouchRLoadB + 100` o 400;
  - `GetWeaponInfo` trunca el índice a `int16` y con una habilidad desconocida devuelve la entrada 47 (sin asserts);
  - `GetSkillStatIndex` devuelve -1 para armas sin niveles; `FindWeaponType` devuelve 0 sin assert;
  - `GetTargetHeadRange`: `fild` con signo y producto redondeado una sola vez en el x87.
- `CWeaponInfo::LoadWeaponData`:
  - tiempos con `fild * 1/30` (no `/ 30`) y fin de bucle `floor((fin - inicio) * 50 + 0,1) * 0,02 - 0,006 + inicio`;
  - grupo de animación buscado en línea con `strcmp` (distingue mayúsculas) y solo asignado si existe; en `%` sin comprobar (si no aparece se usa el número de grupos);
  - se saltan comentarios `#` y líneas vacías, y `ENDWEAPONDATA` solo cuenta como primera palabra de una línea no reconocida;
  - sin `VERIFY` ni asserts; solo los cuatro campos opcionales empiezan a cero.
- `FxSystemBP_c`:
  - `Update`, `Render`, `FreePrtFromSystem` y `Load` tratan `m_nNumPrims` con signo (`movsx`);
  - `Load` reserva los emisores en la arena FX con 0x40 bytes (antes en el heap) y escribe los indicadores de plano y calima en `FxInfoManager_c` (+0x3C/+0x3D; antes +0x44/+0x45, fuera del bloque del EXE);
  - `Load` no comprueba nombres ni número de campos, lee longitud y bucle directamente en los miembros y devuelve `false`.
- `CTaskComplexBeInGroup`:
  - `MakeAbortable` anula la tarea principal y la secundaria aunque `IsTaskPtr` falle (solo el informe depende de ella);
  - `MonitorMainGroupTask` devuelve la subtarea leída al principio.

**Método:**
- `weapon.dat` y `effects.fxp` se generan por caso y llegan por `CFileMgr::OpenFile/ReadLine/CloseFile` simulados; los decimales son múltiplos de 1/8 para que las dos CRT conviertan igual.
- `aWeaponInfo`, los desplazamientos de apuntado, `CFileLoader::ms_line` y 20 punteros de model info (sustituidos por model infos falsos) se guardan y se restauran en cada ejecución.
- `FxMemoryPool_c::GetMem` reparte una arena propia; `FxEmitterBP_c::Load`, `LoadTextures` (ranuras de la vtable del EXE y de la DLL) e `IsFxInfoPresent` se trazan; las primitivas de `Update`/`Render`/`FreePrtFromSystem` tienen vtable falsa.
- Las tareas de grupo usan peatones e inteligencias falsas, `CPedGroupIntelligence` y `CTask::IsTaskPtr` simulados y tareas falsas trazadas; el pool de tareas y las referencias se restauran.
- Fuera de los fixtures:
  - habilidades fuera de 0..3 en armas con niveles (el EXE usa un índice sin inicializar);
  - líneas incompletas o tokens que desbordan los buffers del EXE;
  - en `Load`, líneas que no son `FX_PRIM_EMITTER_DATA:` (el EXE deja la primitiva sin inicializar) y fin de archivo antes de tiempo;
  - `GetTaskSecondarySlot` = -1 con tarea secundaria (la DLL aborta en `SetTaskSecondary(-1)`);
  - en `GetSkillStatIndex` solo se comparan los 16 bits bajos (`eStats` es `uint16`).
- Se mantiene el `FIX_BUGS` de `MonitorSecondaryGroupTask` (ranura -1 sin leer); el fixture deja nula la primaria 4, que es lo que lee el EXE.

Repetir: `python reversing/Data10/build_probe.py` y `python reversing/run.py --case data10`. Evidencia: [validation-summary.json](validation-summary.json).
