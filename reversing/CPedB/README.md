<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPed, grupo B: ropa, dibujo y muerte (25 funciones)

Banco de validación por pares (EXE gta_sa 1.0 US frente a DLL) de: `AddGogglesModel`, `RemoveGogglesModel`, `PutOnGoggles`, `TakeOffGoggles`, `IsWearingGoggles`, `Dress`, `Undress`, `CreateDeadPedMoney`, `CreateDeadPedPickupCoors` (float&), `CreateDeadPedWeaponPickups`, `DeadPedMakesTyresBloody`, `RemoveBodyPart`, `SpawnFlyingComponent`, `PedCanPickUpPickUp`, `SetPedAtomicVisibilityCB`, `SetupLighting`, `RemoveLighting`, `Render`, `PreRender`, `PreRenderAfterTest`, `DeleteRwObject`, `SetModelIndex`, `SetCharCreatedBy`, `UseGroundColModel` y `ProcessBuoyancy`.

Estado: **escrito y revisado contra el desensamblado; pendiente de compilar y ejecutar** (lo hace el responsable de la integración). Prefijo de caso `cpedb`, espacio de nombres `CPedBLive`. Repetir: `python reversing/CPedB/build_probe.py` y `python reversing/run.py --case cpedb`.

## Diferencias reales corregidas en `Ped.cpp`

- `PedCanPickUpPickUp`: el EXE usa `CPedIntelligence::FindTaskByType` (0x600EE0, solo las tareas primarias) dos veces, no `FindActiveTaskByType`.
- `CreateDeadPedMoney`: el dinero se compara sin signo (`cmp cx,0Ah / jb`: con 10 ya se crea) y se pasa con `movzx`.
- `CreateDeadPedWeaponPickups`: la munición es `min` con signo contra la mitad (`shr 1`) de la tabla de la calle (la DLL multiplicaba por 2) y, al terminar el bucle de 13 armas, llama a `ClearWeapons`.
- `AddGogglesModel`: sin `assert`; si ya había gafas llama a `RemoveGogglesModel`; crea la instancia por la vtable y hace `AddRef` después.
- `PutOnGoggles`/`TakeOffGoggles`: el slot activo se lee como `int8` sin comprobar rango (`movsx`).
- `Dress`: pone `m_nModelIndex` a -1 antes de `SetModelIndex` (con el índice con signo) y escribe el estado `IDLE` directo (no llama a `SetPedState`). `Undress`: lee el modelo (movsx) antes de `DeleteRwObject`.
- `RemoveBodyPart`: el índice del hueso no se comprueba (`.data()[]`).
- `DeadPedMakesTyresBloody`: reescrita. El EXE usa sectores normales (0..0x77, `x*0,02f+60`, `floor`, `_ftol2`) y la tabla de repeat sectors, no los sectores LOD (30x30) con `x/50`; añade además la instanciación explícita `MakeTyresMuddySectorList<CPtrListDoubleLink<CVehicle*>>` que necesita el banco.
- `SetModelIndex`: sin `assert`; primero el dinero según grupo (`rand()%50+20` o `rand()%25`) y después la tirada del 3 % (`(rand&0xFFFF)*(1/32768)*100` truncado, `<3` pone 400); antes la tirada iba primero y con otra fórmula.
- `Render`: el rango de cámara sale solo si `distSq > rango²` (con igualdad o NaN dibuja; la DLL usaba `<`); la comprobación de nadar usa `bSubmergedInWater` (bit 8 de CPhysical), no `bInVehicle`; las gafas usan `GetAnimHierarchyFromSkinClump` + `RpHAnimIDGetIndex` + `RpHAnimHierarchyGetMatrixArray` (no `GetBoneMatrix`); `GetTaskJetPack` se llama dos veces.
- `PreRenderAfterTest` (reescrita siguiendo el EXE): `GetTaskSwim/JetPack/InAir` se llaman dos veces; giro del minigun con signo y 57,29578f (0x859878); la tarea de entrar al coche es `TASK_COMPLEX_ENTER_CAR_AS_DRIVER` (0x2BD), no `ENTER_ANY_CAR_AS_DRIVER`; sombra en tiempo real con calidad alta solo para el jugador tipo 0; distancia al cuadrado `(dy²+dx²) <= 225` (la DLL comparaba la distancia con 225); sombra de bajo coste sin comprobar `m_pShadowData` cuando no hay tiempo real; `m_pVehicle` nulo con `bInVehicle` apaga la sombra; solo se mira la primera clave de ropa (`vest`/`torso`); `windMod` empieza en 1,0; coche descapotable solo comprueba `m_pVehicle`; velocidad del vehículo = componente en su `forward` ((z+y)+x), no el módulo; los flags son `bRemoveHead` (bit 15), no `bIsTalking`; `fsin` a precisión completa para el bamboleo (`asm`); terremoto con `* 0,0025f` (0x86C340) y fórmula `lo + t*(hi-lo)` del EXE (no el lerp de la DLL); la sangre pide otra vez la jerarquía; lluvia con el col model de impactos (`m_pHitColModel`), `z + radio*0,75`, `x/y ±0,08` y partícula de mojado con `rand*(1/32767)*0,6 - 0,3`, `* 0,01f` en la alfa y `int8` en la humedad.
- `ProcessBuoyancy` (reescrita): empuje `(mult*masa)*0,008f` (0x863984, no `/125`) y mismas constantes en las comparaciones; consume un `rand` (llamadas "por alguna razón" del EXE); `GetTaskSwim` se hace con `GetSimplestActiveTask` otra vez; `GetTaskClimb` inline (dos `GetSimplestActiveTask` + `GetTaskType`); flags en el orden del EXE; arrastre con `_CIpow` del CRT del EXE (0x822130) y producto x87 a float.

Sin cambios necesarios (MATCH): `RemoveGogglesModel`, `IsWearingGoggles`, `CreateDeadPedPickupCoors`, `SpawnFlyingComponent` (`ret 8` vacío), `SetPedAtomicVisibilityCB`, `SetupLighting`, `RemoveLighting`, `PreRender`, `DeleteRwObject` (`jmp 0x534030`), `SetCharCreatedBy`, `UseGroundColModel`.

Desviaciones NOTSA que se dejan (y el banco mantiene fuera de su camino con las trampas desactivadas): `RenderBigHead`/`RenderThinBody` en `Render` (solo con trucos activos; el banco aborta si alguno está activo) y el `NOTSA_LOG_DEBUG` de `RemoveBodyPart` (el `printf` del EXE se simula sin traza).

## Método

- El peatón es un búfer de `sizeof(CPed)` con patrón 0xCD y vptr a una vtable falsa de 64 thunks con traza (los métodos virtuales del peatón se llaman por la vtable del EXE 0x86C358; `Dress`/`Undress` comprueban así los slots 5 y 8). Vehículos, inteligencia, tareas (vtable con traza), clumps (byte de tipo 2), frames, atomic, jerarquía y 64 matrices de huesos, model infos con vtable de trazas, col model de impactos con esferas, datos de jugador, ropa y estadísticas son búferes propios. Los model infos falsos se colocan temporalmente en `ms_modelInfoPtrs` (19990/19991, el de infrarrojos/visión nocturna y el modelo del peatón).
- Se simulan y trazan (dirección del EXE + símbolo y cuerpo de la DLL, incluidos `CEntity::Render/DeleteRwObject/SetModelIndex` por `DllBody`): rand (0x821B1E y `CGeneral::GetRandomNumber`), RW y modelos, armas, streaming, mundo, pickups, localización, luces, visión, tareas, jerarquía de huesos, matrices, sombras, viento, lluvia, Fx, agua, audio, grupo de eventos, fuerzas, buoyancy y respiración. `RwRenderStateSet/Get` son macros del motor: se cambian los dos punteros de `RwEngineInstance` (+0x20/+0x24) y se restauran.
- Globales instantaneadas y restauradas en cada ejecución: reloj, contador de fotogramas, `bSkipProcessThisFrame`, ciclo horario (sombras), cámara, mirrors, clima, `currArea`, punteros de Fx, flags de visión, código de escaneo, jugador 0 del mundo, estados del motor, buoyancy global y `ms_modelInfoPtrs`.
- Comparación: instantánea (hash con nombres estables de punteros) de peatón, inteligencia y objetos tocados + traza ordenada + retorno; ante una diferencia se repite con detalle completo.

## Tabla

| Función | Dirección / slot | Pares |
|---|---|---:|
| AddGogglesModel | 0x5E3A90 | 192 |
| RemoveGogglesModel | 0x5DF170 | 192 |
| PutOnGoggles | 0x5E3AE0 | 256 |
| TakeOffGoggles | 0x5E6010 | 256 |
| IsWearingGoggles | 0x479D10 | 64 |
| Dress | 0x5E0130 | 192 |
| Undress | 0x5E00F0 | 192 |
| CreateDeadPedMoney | 0x4590F0 | 512 |
| CreateDeadPedPickupCoors | 0x459180 | 128 |
| CreateDeadPedWeaponPickups | 0x4591D0 | 512 |
| DeadPedMakesTyresBloody | 0x6B4200 | 512 |
| RemoveBodyPart | 0x5F0140 | 128 |
| SpawnFlyingComponent | 0x5F0190 | 64 |
| PedCanPickUpPickUp | 0x455560 | 128 |
| SetPedAtomicVisibilityCB | 0x5F0060 | 128 |
| SetupLighting | vtable 0x86C358 slot 19 | 64 |
| RemoveLighting | slot 20 | 64 |
| Render | slot 18 | 768 |
| PreRender | slot 17 | 256 |
| PreRenderAfterTest | 0x5E65A0 | 1024 |
| DeleteRwObject | slot 8 | 64 |
| SetModelIndex | slot 5 | 384 |
| SetCharCreatedBy | 0x5E47E0 | 256 |
| UseGroundColModel | 0x5DEFE0 | 128 |
| ProcessBuoyancy | 0x5E1FA0 | 768 |

Total 7.232 pares. Evidencia: `validation-summary.json` (se genera al aprobar).
