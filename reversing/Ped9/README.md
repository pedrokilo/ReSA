<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# IK del peatón, efectos de armas, cadena IK, lanzar y recoger bici (lista de 9)

Validadas el 02/10/2026 con **14.720 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPedIK` | Constructor, `GetWorldMatrix`, `MoveLimb` (sin normalizar y normalizado), `PitchForSlope`, `PointGunAtPosition`, `PointGunInDirection`, `RotateTorso`, `RotateTorsoForArm` |
| `CWeaponEffects` | `Init`, `Shutdown`, `IsLockedOn`, `MarkTarget`, `ClearCrossHair`, `ClearCrossHairs`, `ClearCrossHairImmediately`, `ClearCrossHairsImmediately`, `Render` |
| `CTaskSimpleIKChain` | Constructor, Destructor, `BlendOut`, `Clone`, `CreateIKChain`, `GetIKChain`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleThrowProjectile` | Constructor, Destructor, `Clone`, `ControlThrow`, `FinishAnimThrowProjectileCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `StartAnim` |
| `CTaskSimplePickUpBike` | Constructor, Destructor, `Clone`, `FinishAnimPickUpBikeCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `StartAnim` |

**Diferencias reales corregidas:**
- `CPedIK`:
  - los grados usan la constante del EXE 57,29578f (0x859878), no `180/PI`;
  - `RotateTorso` elige el modo de la primera rotación con el flag (`PRECONCAT`/`REPLACE`) y marca `bUpdateMatricesRequired`, no `bDontAcceptIKLookAts`;
  - `RotateTorsoForArm` usa el rumbo de `fpatan` sin redondear, recorta a 45° en vez de saltar al otro caso y, sin `bRotateWithNeck`, gira el torso el ángulo entero;
  - `PointGunInDirection` repone la copia sin uso en una matriz de RW, vuelve a pedir la jerarquía, toma la versión sin normalizar también con NaN y calcula `fpatan`/`fsin`/`fcos` a precisión completa;
  - `MoveLimb` compara primero con el máximo (sin `std::clamp`) y el pitch solo devuelve "sin alcanzar" si se mueve;
  - `PitchForSlope` se reescribió siguiendo el EXE: "muriendo" incluye `bFallenDown`, límites con `(1 - 0,9) * mult + 0,9 * límite`, comprueba la tarea activa, pantorrilla y pie usan el asin del muslo con `POSTCONCAT`, el byte 0xC0AFC0 elige la fórmula del muslo y el desplazamiento usa `fsin`/`fcos` a precisión completa.
- `CWeaponEffects`:
  - `Shutdown` destruye `lockonFire`, `lockon` y `target256` en ese orden;
  - `IsLockedOn` trunca el float (`_ftol2`) en vez de convertirlo a bool;
  - `ClearCrossHairsImmediately` pone hora - 100 y desactiva cada mira (llamaba a `ClearCrossHair`);
  - `Render`: el modo inmediato compara el byte con 1, los bordes de pantalla y NaN cuentan como dentro, el tamaño se recorta con el min/max del EXE, el centro se suma (c + b) + a, `fsin`/`fcos` a precisión completa y el triángulo cooperativo sigue el orden de operaciones del EXE (incluida la constante double 0,1).
- `CTaskSimpleIKChain`:
  - el constructor guarda el hueso de desplazamiento como dword (antes no lo guardaba) y no escribe `m_Blend`;
  - `Clone` y `CreateIKChain` pasan el hueso como dword;
  - `ProcessPed` caduca solo con duración (`m_Duration != -1`, estaba al revés) y la mezcla usa el paso truncado a entero con `fild`/`fidiv` y tope solo por encima de 1.
- `CTaskSimpleThrowProjectile`:
  - `StartAnim` usa la habilidad estándar y lanza normal si el objetivo está lejos; la animación "U" solo de cerca con el botón soltado o con la carga satchel (la condición estaba invertida);
  - `ProcessPed` usa la habilidad estándar, el tope del contador con signo y `fild * 0,05f`.
- `CTaskSimplePickUpBike`:
  - `ProcessPed` termina sin vehículo y solo marca la bici con las cuatro animaciones de alinear a partir de 0,4667f;
  - `StartAnim` elige la animación "HI" cuando `right.z` es NaN.

**Método:**
- Los constructores se llaman en la dirección del EXE sobre un buffer con 0xCD y se compara el tipo en vez del vptr; los métodos virtuales se llaman por la vtable del EXE.
- `CPedIK` corre sobre un peatón falso con clump, 33 frames de animación con su cuaternión, una jerarquía de 32 nodos con matrices propias (a veces falta un hueso: índice -1) y frames de RW encadenados.
- `CWeaponEffects` corre sobre las miras, texturas y estáticos reales del EXE, guardados y restaurados en cada ejecución; `Render` usa peatones falsos en `CWorld::Players` y cambia temporalmente la cámara (pantalla ancha, matriz, modo y puntería).
- Se simulan y trazan `RtQuatRotate`, `GetAnimHierarchyFromSkinClump`, `RwMatrixCreate`/`Destroy`, `UpdateRwFrame`, `GetWeaponSkill`, `IsPlayer`, las animaciones, la cadena IK, `CWeapon::Fire`, el alineado con el vehículo, los grupos de animación del vehículo, el TXD, las texturas, los sprites, `RwRenderStateSet` (puntero del motor), el radio del arma, el modo cooperativo, el objetivo más cercano y la mira auxiliar del jugador.
- Se restauran el reloj, `ms_fTimeStep`, `bRotateWithNeck`, el byte 0xC0AFC0, las tareas nuevas del pool (m_LastFreeSlot y huecos libres con 0xCD) y las referencias.
- `IsLockedOn` se compara como verdadero/falso: el EXE devuelve el byte bajo del entero y la DLL un bool.

Repetir: `python reversing/Ped9/build_probe.py` y `python reversing/run.py --case ped9`. Evidencia: [validation-summary.json](validation-summary.json).
