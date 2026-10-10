<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WorldWave2: banco por pares de CRoadBlocks, CPools y CIdleCam (funciones pendientes)

Banco simple (flujo viejo, copiado de `WorldFinal1`/`PadMouse1`/`Small2`). **Sin compilar ni ejecutar todavia** (encargo: no compilar, no ejecutar GTA).

Uso: `bash reversing/_handoff/bench.sh WorldWave2 3`. Filtro opcional: `worldwave2-only.txt` en la carpeta del juego con `Clase::Funcion`.
`build_probe.py` y `run_local.py` redirigen `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test` (el junction D: ya no existe).

## Corpus: 6.784 pares (maximo)

| Funcion | EXE | Pares |
|---|---|---|
| `CRoadBlocks::RegisterScriptRoadBlock` | 0x460DF0 | 512 |
| `CRoadBlocks::ClearScriptRoadBlocks` | 0x460EC0 | 128 |
| `CRoadBlocks::GetRoadBlockNodeInfo` | 0x460EE0 | 768 |
| `CRoadBlocks::ClearSpaceForRoadBlockObject` | 0x461020 | 1024 |
| `CRoadBlocks::GenerateRoadBlocks` | 0x4629E0 | 1024 |
| `CPools::MakeSureSlotInObjectPoolIsEmpty` | 0x550080 | 512 |
| `CPools::CheckPoolsEmpty` | 0x551950 | 128 |
| `CIdleCam::Init` | 0x50E6D0 | 64 |
| `CIdleCam::Reset` | 0x50A160 | 128 |
| `CIdleCam::ProcessIdleCamTicker` | 0x50A200 | 256 |
| `CIdleCam::GetLookAtPositionOnTarget` | 0x50EAE0 | 512 |
| `CIdleCam::IdleCamGeneralProcess` | 0x50E690 | 256 |
| `CIdleCam::SetTarget` | 0x50A280 | 384 |
| `CIdleCam::SetTargetPlayer` | 0x50EB50 | 128 |
| `CIdleCam::IsTargetValid` | 0x517770 | 512 |

## Qué se compara (original = hook desactivado, candidato = hook activo)

Cada caso reconstruye el mismo fixture desde la misma semilla en los dos legs y compara: resultado, estado de los objetos falsos (por hash o bytes), control x87 y la traza de todas las llamadas salientes con argumentos. Los registros que pasan llevan `original` y `candidate`.

- **CRoadBlocks**: se guarda/restaura el bloque global 0xA43438..0xA43C78 (InOrOut, NumRoadBlocks, GenerateDynamicRoadBlocks, RoadBlockNodes, aScriptRoadBlocks). Las areas de rutas 61 (sin cargar), 62 y 63 de `ThePaths` se sustituyen por nodos (0x1C), enlaces de navegacion y `CCarPathLink` (0xE) falsos; punteros y `m_anNumAddresses` se restauran. Entidades falsas de 0xA40 bytes con vtable falsa (hueco 0 = destructor con borrado, trazado). Dobles: `FindObjectsIntersectingCube`, `CWorld::Remove`, `CanBeDeleted` de vehiculo/ped/objeto, `rand` (0x821B1E) y `CGeneral::GetRandomNumber` (misma secuencia), `FindPlayerWanted/Vehicle/Coors`, `CreateRoadBlockBetween2Points` (solo traza; ya estaba validada), `CWorld::ProcessLineOfSight`. `GetRoadBlockNodeInfo` llama a los dos hooks reales con centinelas 0xDEADBEEF para comprobar que en `return false` no se escribe nada.
- **CPools**: el puntero global del pool de objetos (0xB7449C) se cambia por un pool falso de 4 ranuras de 0x19C bytes y se restaura. Dobles: `CWorld::Add/Remove`, `CProjectileInfo::RemoveIfThisIsAProjectile`, `CObject::operator new` y constructor (el de la DLL por su simbolo exportado), `printf` del EXE (0x821982; la DLL no lo llama, asi que `CheckPoolsEmpty` solo se compara en estado: no debe cambiar nada).
- **CIdleCam**: objeto falso de 0x9C bytes con guardas y camara falsa de 0x300 bytes. Se guardan/restauran CPad 0, `CTimer` (0xB7CB4C/5C/84), el ticker de `gIdleCam` (0xB6FE34), el byte de InterestingEvents (0xC0B184), la camara activa de TheCamera (0xB6F081 y modos 0xB6F1A8) y `CWorld::pIgnoreEntity`. Dobles: `RegisterReference`, `CleanUpOldReference`, `FindPlayerPed`, `GetIsLineOfSightClear`.

Se comprueba la disposicion de campos de la DLL frente a los desplazamientos del EXE (evento `layout`).

## Fuera del banco (regla 17) y por que

- `CRoadBlocks::Init` (0x461100): abre `data\paths\roadblox.dat` (0x538900 CFileMgr::OpenFile + 0x538950 Read): fichero real del juego.
- `CRoadBlocks::GenerateRoadBlockPedsForCar` (0x461170): reserva peds reales del pool (0x5E4720, 0x7A4/0x79C bytes), tareas (0x61A5A0), `CWorld::Add` y eventos (0x4AB420); no se puede doblar con seguridad.
- `CPools::Initialise/ShutDown` (0x550F10/0x5519F0): crean/destruyen los 17 pools reales (~70000 nodos, 13000 edificios, ...); `CPools::Save/Load/LoadPedPool/LoadObjectPool/LoadVehiclePool` (0x5D0880/0x5D0890/0x5D2D70/0x5D4A40/0x5D2A20): leen el buffer de guardado y construyen CPlayerPed/CObject/vehiculos reales (`LoadVehiclePool` llama a `CStreaming::RequestModel` 0x4087E0 + `LoadAllRequestedModels` 0x40EA10, E/S de disco real).
- `CIdleCam::FinaliseIdleCamera/ProcessSlerp/ProcessTargetSelection/Run/Process`: usan `gHandShaker`, `g_InterestingEvents` y `CCam::GetVectorsReadyForRW` reales; no revisadas ni benchadas por falta de tiempo.
- `CCam` (14 funciones) y las funciones restantes de `CIdleCam`: no revisadas por falta de tiempo.

## Resultado de la primera ejecucion y correcciones (09/10/2026)

- `CRoadBlocks::GenerateRoadBlocks` (317/1024): diferencia REAL de la fuente. `ThePaths.IsAreaLoaded(mrbNode)` recibia un `CNodeAddress` que se convertia a bool (area 0/1), asi que la DLL nunca entraba en el bucle dinamico (candidato sin tiradas de rand). Corregido a `mrbNode.m_wAreaId` (0x462AC4: `movzx eax,di`). Ademas se corrigio el cierre del bucle interior: si no hay pareja se crea la barricada del propio nodo con `mrbDir` (0x4631F7..0x4632AE; antes era codigo muerto `i == NumRoadBlocks` dentro del bucle) y la comparacion de |dot| usa `!(fabs > 0.7)` (NaN, como `fcomp` + `test ah,41h`).
- `CIdleCam::SetTarget` (37/384): ruido del banco. Solo fallaba con `m_Target == nullptr` (rama `m_vecSource + m_vecFront` de la camara) y siempre que un componente era un entero pequeno (float denormal) sumado a 0.0: el SSE de la DLL lo trata como 0 (DAZ/FTZ) y el x87 del EXE no. Los vectores de la camara se normalizan en `FillIdle` (sin denormales). La fuente coincide con 0x50A280.

## Aprobacion (09/10/2026)

Corrida aprobada `20261009-023948-282056`: 6336 pares, 0 diferencias, state_restored, saves y configuraciones intactos; sonda 1552ba83... Evidencia en `runs/20261009-023948-282056/` (jsonl en .gz con hash verificado), resumen en `validation-summary.json`, registro con `register_worldwave2.py` (15 funciones) y documentacion con `refresh_documentation_cut195.py`. No se acreditan: CRoadBlocks::Init y GenerateRoadBlockPedsForCar, CPools::Initialise/ShutDown/Save/Load/LoadPedPool/LoadObjectPool/LoadVehiclePool, CIdleCam::FinaliseIdleCamera/ProcessSlerp/ProcessTargetSelection/Run/Process y CCam. Ninguna clase queda completa (CRoadBlocks 2 pendientes, CPools 7, CIdleCam 5).
