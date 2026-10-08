<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPed grupo A: armas e inventario del peatón (31 funciones)

Banco de validación por pares dentro del motor (EXE gta_sa 1.0 US frente a la DLL) sobre un **peatón falso**
(buffer del tamaño de `CPed` con patrón aleatorio; los punteros que el EXE desreferencia apuntan a objetos falsos).
Prefijo de caso `cpeda`, espacio de nombres `CPedALive`. Pendiente de compilar y ejecutar (el agente no compila ni ejecuta el juego).

Repetir: `python reversing/CPedA/build_probe.py` y `python reversing/run.py --case cpeda` (hay que registrar el caso `cpeda` en `reversing/cases.json`).

## Funciones y casos

| Función | Dirección | Casos |
|---|---|---:|
| `GetWeaponSlot` | 0x5DF200 | 256 |
| `GrantAmmo` | 0x5DF220 | 512 |
| `SetAmmo` | 0x5DF290 | 512 |
| `DoWeHaveWeaponAvailable` | 0x5DF300 | 256 |
| `DoGunFlash` | 0x5DF340 | 256 |
| `SetGunFlashAlpha` | 0x5DF400 | 512 |
| `ResetGunFlashAlpha` | 0x5DF4E0 | 128 |
| `GetHoldingTask` | 0x5E0290 | 256 |
| `GetEntityThatThisPedIsHolding` | 0x5E02E0 | 256 |
| `DropEntityThatThisPedIsHolding` | 0x5E0360 | 512 |
| `CanThrowEntityThatThisPedIsHolding` | 0x5E0400 | 256 |
| `GiveObjectToPedToHold` (solo ramas sin crear objeto) | 0x5E4390 | 256 |
| `RemoveWeaponModel` | 0x5E3990 | 512 |
| `AddWeaponModel` | 0x5E5ED0 | 768 |
| `GetWeaponSkill-Current` | 0x5E6580 | 256 |
| `GetWeaponSkill-WeaponType` | 0x5E3B60 | 768 |
| `SetWeaponSkill` | 0x5E3C10 | 128 |
| `GiveWeapon-` | 0x5E6080 | 1024 |
| `SetCurrentWeapon-Slot` | 0x5E61F0 | 512 |
| `SetCurrentWeapon-WepType` | 0x5E6280 | 512 |
| `ClearWeapon` | 0x5E62B0 | 512 |
| `ClearWeapons` | 0x5E6320 | 256 |
| `RemoveWeaponWhenEnteringVehicle` | 0x5E6370 | 768 |
| `ReplaceWeaponWhenExitingVehicle` | 0x5E6490 | 512 |
| `ReplaceWeaponForScriptedCutscene` | 0x5E6530 | 256 |
| `RemoveWeaponForScriptedCutscene` | 0x5E6550 | 256 |
| `RequestDelayedWeapon` | 0x5E8910 | 512 |
| `GiveDelayedWeapon` | 0x5E89B0 | 768 |
| `GiveWeaponAtStartOfFight` | 0x5E8AB0 | 768 |
| `GiveWeaponWhenJoiningGang` | 0x5E8BE0 | 512 |
| `RemoveWeaponAnims` | 0x5F0250 | 512 |

Total: 14.080 pares.

## Diferencias corregidas en la DLL (Ped.cpp / Ped.h)

- `GrantAmmo`: el recorte a 99999 y el "queda munición" se comparan con signo (`jl`/`jle`), no sin signo.
- `SetAmmo`: el cargador baja al total (`min` con signo; la DLL hacía `max`), recorte y test de munición con signo.
- `GiveWeapon` (mismo arma): recorte con signo (`jl`); el modelo que se añade sale de la información del arma de la ranura activa.
- `GetWeaponSkill(tipo)` (jugador): con PRO compara `req <= valor`; el segundo corte es con el nivel STD (no POOR) y con `<`; el valor de la estadística se pide una vez por comparación (después de leer el nivel).
- `RemoveWeaponModel`: el EXE siempre quita la animación de piel del clump del arma (la DLL lo hacía solo con `SA_SKINNED_PEDS`, que no está definido).
- `RemoveWeaponWhenEnteringVehicle`: sin Uzi/Tec-9/MP5 utilizable y sin jetpack el EXE también quita el modelo del arma (falta en la DLL); sin assert sobre `isJetpack`; munición con signo.
- `GiveObjectToPedToHold`: busca primero la tarea HOLD_ENTITY y llama siempre a `GetEntityThatThisPedIsHolding` (suelta lo que sujete aunque no haya tarea HOLD_ENTITY); devuelve el `CObject*` creado (EAX del EXE) o `nullptr` (el tipo de retorno pasó de `void` a `CObject*`).
- `AddWeaponModel` (molotov): jerarquía de la piel (`GetAnimHierarchyFromSkinClump` 0x734A40) + `RpHAnimIDGetIndex` + array de matrices, sin las comprobaciones de `GetBoneMatrix`.
- `DropEntityThatThisPedIsHolding`: la entidad sujeta se lee antes de `DropEntity` (que puede borrar la referencia) y solo el tipo 2 de objeto (`OBJECT_MISSION`) se conserva (`IsMissionObject()` también aceptaba el 6).

## Método

- Original (los 31 hooks apagados) frente a candidato (encendidos), con fixtures idénticos por semilla. Se compara el retorno, el peatón completo (palabra a palabra, con nombres para los punteros a objetos falsos), los objetos falsos y la traza de todo lo externo.
- Se simulan y trazan en ambos lados (parche de entrada en la DLL y en el EXE): `CWeapon::Initialise/Shutdown/Reload` (imitan los campos sin tocar model infos reales), `CPed::RemoveGogglesModel`, `CTaskManager::FindActiveTaskByType`, `CPedIntelligence::GetTaskHold`, `CTaskSimpleHoldEntity::DropEntity/CanThrowEntity`, `CWorld::Remove`, `CPlayerPed::GetPlayerInfoForThisPlayerPed`, `CStreaming::RequestModel`, `CStats::GetStatValue`, `CTxdStore::AddRef/RemoveRef`, `CClumpModelInfo::GetFrameFromName`, `FxManager_c::CreateFxSystem/DestroyFxSystem`, `FxSystem_c::SetLocalParticles/CopyParentMatrix/Play`, `GetAnimHierarchyFromSkinClump`, `RpHAnimIDGetIndex`, `RpHAnimHierarchyGetMatrixArray`, `RpClumpDestroy`, `GetFirstAtomic`, `RpSkinGeometryGetSkin`, `RpClumpForAllAtomics`, `GetFirstObject`, `CVehicle::SetComponentAtomicAlpha`, `RwMatrixRotate`, `RpAnimBlendClumpGetAssociation`, `CAnimManager::BlendAnimation`, el `rand` del EXE (0x821B1E) y `CGeneral::GetRandomNumber`.
- `IsPlayer` está inlineada en el EXE: se rellena el `m_nPedType` real (jugador, policía, pandilleros...). `CBaseModelInfo::AddRef/RemoveRef`, `CVisibilityPlugins::GetClumpModelInfo`, `CWeaponInfo::GetWeaponInfo`, `CStreaming::IsModelLoaded` y `CCheat` se ejecutan de verdad sobre datos controlados.
- Estado global guardado y restaurado en cada ejecución y al final: `CWeaponInfo::aWeaponInfo[0..46]` (ranura y modelos, a veces ranura -1 solo en las funciones que la comprueban), las entradas 344..349 de `CModelInfo::ms_modelInfoPtrs` (cambiadas por model infos falsos con vtable trazada), el estado de carga de streaming de los modelos usados (344..349 y 1000..1002) y los dos trucos `NO_ONE_CAN_STOP_US`/`ROCKET_MAYHEM`.
- Al empezar se comprueba que la disposición de la DLL coincide con la del EXE (campos de `CPed`, `CWeapon`, `CPlayerPedData`, `CPlayerInfo` +0x153, `m_TaskMgr`, hold entity, tipos de tarea 0x133..0x136, `MODEL_MOLOTOV`, bytes de truco, tipo de entidad en 0x36, plugin de frame 0x8D6090...); si algo no coincide el banco termina con un evento `error` que lo describe.
- La vtable falsa del model info trampea todas las ranuras salvo la 11 (`CreateInstance()` sin argumentos, la que usa el EXE): si la DLL llamara a otra ranura (orden de sobrecargas virtuales) la traza lo mostraría. Lo mismo con la vtable falsa de la entidad sujeta (destructor en la 0, `DeleteRwObject` en la 8).

## No cubierto

- `GiveObjectToPedToHold`: la rama que crea el `CObject` (pool real de objetos, constructor real de `CObject`, `CWorld::Add` y la tarea) no se prueba; solo las ramas que terminan antes (hay tarea HOLD_ENTITY y no se pide reemplazar o no hay entidad). La rama de creación se validó solo por desensamblado (0x5E4400-0x5E44EE).
- Peatones reales, el jugador y el audio no se tocan; `CWeapon::Initialise/Shutdown/Reload` son simulaciones (sus propias lógicas pertenecen a otro grupo).
