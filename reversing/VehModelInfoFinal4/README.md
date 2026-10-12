<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CVehicleModelInfo (banco final 4)

Banco par a par dentro del juego completo (original con los hooks de la clase desactivados frente a candidata con hooks activos) para las
**75 funciones** de `CVehicleModelInfo` (`docs/hooks.csv`, `validated=0`): 16.384 pares previstos. Sin ejecutar todavia: pendiente de compilar y correr.

Comando: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh VehModelInfoFinal4 3`

## Correcciones de fuente (VehicleModelInfo.cpp/.h), con evidencia del EXE
- `GetNumDoors` 4C73C0 `movsx eax, byte [ecx+4Ch]`: devuelve el byte con signo.
- `SetAnimFile` 4C767E: `call 8229B6` es `_stricmp` ("NULL" tambien se ignora); `strcpy` en vez de `strcpy_s` (4C76C0).
- `FindTextureCB` 4C7560: copia sin limite (`strcpy`).
- `SetAtomicRendererCB_Boat` 4C79B1: el recorrido de materiales va ANTES de comparar el nombre.
- `SetVehicleComponentFlags` 4C7C1B, `PreprocessHierarchy` 4C8E6F, `GetMaximumNumberOfPassengers...` 4C89BF: `movzx byte [..+4Ah]` (solo el byte bajo del id de handling).
- `GetWheelPosn` 4C7D5A: `rep movsd` de 0x40 bytes (`sizeof(RwMatrix)`, antes `sizeof(CMatrix)`).
- `CountCompsInRule` 4C7F9B `sar ecx,4` (antes `/ 16`).
- `ChooseComponent` global/miembro/segundo 4C7FC2..4C8100, 4C80B7: azar de `0x407180` = `GetRandomIntInRange`, no la plantilla entera (desfasada en uno); `m_nNumExtras` con signo (movsx 4C80E2, 4C81C1).
- `ChooseVehicleColour` 4C8570: indice con `movzx` del byte (sin comprobar el array); `AddRemap` 4C86F4 y `CreateInstance` 4C96E2/4C9750: sin comprobacion de limite.
- `SetCustomCarPlateText` 4C898F: `strncpy(.., 8)` (antes `strcpy_s` sin truncar, desviacion NOTSA).
- `GetMaximumNumberOfPassengers...` 4C8A31: `m_nNumDoors` sin signo en esa ruta.
- `GetMatFXEffectMaterialCB` 4C8816/4C8823: dos llamadas a `RpMatFXMaterialGetEffects`.
- `SetEnvironmentMapCB` 4C8840: compara el dword completo con 0xFFFF y llama directo a `RpMatFXMaterialSetEffects(material, 0)`.
- `SetEnvMapCoeffCB` 4C88C7..4C88DC: x87 `fild`, `fadd 2^32` si es negativo, `fmul 0.001f` (antes `/ 1000.0f`), con `__asm`.
- `SetEnvMapCoeff` 4C9819: truncacion a 64 bits (`_ftol2`).
- `ReduceMaterialsInVehicle` 4C8C02/4C8C54/4C8C59: llamadas a `CTimer` descartadas; numExtras con signo.
- `PreprocessHierarchy` 4C919A: callback por defecto `0x7491C0` (no el envoltorio de la DLL); `m_apExtras` indexado con `movsx`.
- `AssignRemapTxd` 4C937B/4C93BD: `isdigit` sobre el byte sin signo y `strncpy`.
- `LoadEnvironmentMaps` 4C8781..4C8793: `FindTxdSlot` antes de `PushCurrentTxd`. `ShutdownEnvironmentMaps` 4C87D9: pone a 0 el byte global 0xB4E6BC (antes una estatica local).
- `LoadVehicleColours` 5B68E0: sin limite de 10 caracteres, comparacion sin signo; sin el `FIX_BUGS` de `"77.93,96"`; `red/green/blue` persisten entre lineas como los locales de pila del EXE.
- `LoadVehicleUpgrades` 5B65BE: abre el fichero antes de poner a 0 los contadores; `iWheelSet` persistente y sin `VERIFY`.
- `SetupCommonData` 5B8F93: sin `assert(vehicleTxd)`.
- `CVehicleStructure`: constructor sin `m_aUpgrades()` y `UpgradePosnDesc::m_qRotation` en union anonima (el EXE, 4C8D60/4C73F0, no escribe esos bytes); destructor con cuenta con signo (4C7410).
- `VehicleModelInfo.h`: se quita `static` de las 6 funciones globales para poder llamarlas desde el banco.

## Metodo
- Mundo falso estatico (`Support.hpp`/`Ctx.hpp`): model infos con vtable copiada del EXE, `CVehicleStructure` en un pool falso (0xB4E680), TXD falso (0xC8800C), clumps (byte de tipo 2), arboles de frames, atomics, geometrias, mallas, materiales y texturas; offsets de plugin leidos del EXE.
- Stubs trazados en la direccion del EXE y en el simbolo de la DLL (`EntryPatch`, `Body()` calculado antes de parchear): primitivas RW, `CFileMgr`/`CFileLoader`, `CTxdStore`, `CModelInfo::GetModelInfo`, azar (0x821B1E + `CGeneral::GetRandomNumber`), `operator new/delete`, `CTimer`, scratchpad, `CClumpModelInfo` base y subfunciones de la clase en `CreateInstance`/`SetClump`. Los hilos ajenos no se trazan.
- Los parches se crean tras fijar el estado de los hooks de cada ejecucion. x87 a 24 bits. Todos los globales tocados se guardan y restauran en cada ejecucion y al final.
- Comparan: retorno, traza, hash del mundo falso, de la arena y de los globales guardados, mas campos concretos por funcion.

## Cosas enmascaradas / fuera de dominio
- `SetCarCustomPlate`: el EXE pasa a `SetupClump` un dword con basura en los 3 bytes altos; se traza solo el byte bajo.
- Pool de `SetupCommonData`: el byte 0x11 del pool (`m_DealWithNoMemory`) y el relleno 0xFD de la DLL no se comparan (solo cabecera y estados).
- Almacenamiento del pool falso de `CVehicleStructure` a cero antes de `SetClump`.
- Fuera del fixture: `CountCompsInRule` con negativos (bucle infinito en el EXE), nibbles 6..14 en las reglas (desbordan la lista local), `GetMaximumNumberOfPassengers...(-1)` (lee `ms_modelInfoPtrs[-1]`), `LoadVehicleColours` con lineas de color sin `#` (el EXE rastrea la pila), mas de 6 extras.

## Aprobación (10/10/2026)

Corrida `20261010-005102-024078`: passed=true, 16384 pares, 75 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_vehmodelinfofinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco CVehicleModelInfo queda sin pendientes. Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.
