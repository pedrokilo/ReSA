<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión de los 21 destinos inciertos (PLAN-001)

Resolución del 17 de octubre de 2026 de las 21 filas que la cohorte de 499 tenía con `category=unknown`. Cada una se clasificó con al menos dos evidencias independientes (forma y convención en el EXE, grafo de llamadores y llamados, zona del binario, nombres de referencia), se reconstruyó en C++ puro con hook RH y se validó dentro del juego con los bancos `reversing/Dependencies21A` a `reversing/Dependencies21G`: **47 104 pares, cero diferencias, ningún resultado nulo, estado, partidas y configuración restaurados**. Resultado: **20 funciones del juego y 1 biblioteca (CRT)**; ya no queda ninguna incierta.

Las clasificaciones se redactaron durante la tanda de reconstrucción (informes de trabajo `ga` a `gg`) y aquí se resumen. `targets.csv` y `baseline.json` no cambian. Clasificar y validar son cosas distintas: la validación vale sólo para los dominios de fixtures de cada banco y no certifica independencia transitiva del EXE (`dependency_review=null`).

| Banco | Funciones | Pares | Corrida final (`reversing/CppTwelveLevel6Final/runs/`) |
| --- | --- | ---: | --- |
| Dependencies21A | `CRT::CrtPrintf` | 4096 | `Dependencies21A-20261017-014358-631804` |
| Dependencies21B | 5 `CCamera::ProcessObbeCinemaCamera*` | 10240 | `Dependencies21B-20261017-022839-927967` |
| Dependencies21C | 3 `CTaskSimpleFight::*` | 4608 | `Dependencies21C-20261017-024536-158294` |
| Dependencies21D | 4 `CTaskSimpleGangDriveBy::*` | 11264 | `Dependencies21D-20261017-023047-676022` |
| Dependencies21E | 3 ayudantes (población, zona, cámara de tarea) | 6144 | `Dependencies21E-20261017-014822-184376` |
| Dependencies21F | 2 `D3DResourceSystem::TidyUp*` | 4096 | `Dependencies21F-20261017-014930-329804` |
| Dependencies21G | `GetFirstTexture`, `RpGeometryReplaceOldMaterialWithNewMaterial`, `CQuaternion::Slerp` | 6656 | `Dependencies21G-20261017-023157-980087` |
| **Total** | **21** | **47104** | |

Los pares de cada función están en el `validation-summary.json` del banco. Total de pares de los bancos 21A a 21G: 4096 + 10240 + 4608 + 11264 + 6144 + 4096 + 6656 = 47104.

## Grupo A · CRT

### 0x00821982 · `printf(const char*, ...)` de la CRT estática de MSVC · categoría `library`
- Evidencia 1 (forma): el desensamblado completo (0x821982 a 0x8219E7) es la plantilla de `printf` de VC8/VC9: prólogo SEH (`push 0x887DC0` / `call 0x8286E0`), `_lock_file2(1, stream)`, `_stbuf` (0x827DEC), `_output` (0x827F4B) con los varargs en `ebp+0xC`, `_ftbuf` (0x827E74), `finally` con `_unlock_file2` y epílogo SEH; cdecl, devuelve en EAX el número de caracteres.
- Evidencia 2 (char frente a wchar): `_output` lee el formato con `mov bl, byte ptr [eax]` y tablas de bytes; el identificador `FID_conflict:_wprintf` de Ghidra es una coincidencia de huella y se descarta. Los 29 llamadores pasan literales ANSI.
- Evidencia 3 (flujo y zona): 0x8E2C50 es `_iob[1]` (stdout); 0x82xxxx es la CRT estática contigua a `sscanf` (0x8220AD) y `rand` (0x821B1E).
- Decisión: envoltorio C++ (`__try/__finally`, `va_list`) que llama por dirección a lock/`_stbuf`/`_output`/`_ftbuf`/unlock; el motor de formato sigue siendo el del EXE. Se cuenta como **biblioteca con seguimiento separado**, no como función del juego aprobada.
- Banco `Dependencies21A`, 4096 pares.

## Grupo B · cámaras cinemáticas Obbe (`game`)

Evidencias comunes: cada una es `thiscall` sin argumentos de pila, guarda EBX/ESI/EDI y usa campos documentados de `CCamera` (`m_nActiveCam` +0x59, `m_bCinemaCamera` +0xCF0, `m_nModeObbeCamIsInForCar` +0xC34, modo de la cámara activa +0x180); cada una tiene exactamente un llamador, dentro de `CCamera::CamControl` (0x527FA0), que despacha por tipo de vehículo; todas llaman a `CCamera::TryToStartNewCamMode` (0x51E560) y muestran el mensaje de ayuda `CINCAM`. Banco `Dependencies21B`, 2048 pares cada una.

| Dirección | Identidad | Evidencia propia |
| --- | --- | --- |
| 0x005267C0 | `CCamera::ProcessObbeCinemaCameraCar` | Llamada única 0x52A72D en la rama general del despacho; tabla de 11 secuencias en 0x8CC828, índice 0x8CCEEC, módulo 11. |
| 0x00526950 | `CCamera::ProcessObbeCinemaCameraTrain` | Llamada única 0x52A71F con subtipo de vehículo 6 (tren); tabla de 6 en 0x8CC858, índice 0x8CCEF0. |
| 0x00526AE0 | `CCamera::ProcessObbeCinemaCameraHeli` | Llamada única 0x52A6F9 tras `GetVehicleAppearance == 3`; tabla de 7 en 0x8CC874, comprobación de cámara bajo el agua (0x50B830). |
| 0x00526C80 | `CCamera::ProcessObbeCinemaCameraPlane` | Llamada única 0x52A6DF con subtipo 4 (avión); tabla de 6 en 0x8CC894 y marca 0xB6EC34 antes del mensaje. |
| 0x00526E20 | `CCamera::ProcessObbeCinemaCameraBoat` | Llamada única 0x52A70C con subtipo 5 (barco); tabla de 3 en 0x8CC8B0. |

Zona: bloque contiguo 0x5267C0 a 0x526FB0, antes de `CCam::Process` (0x526FC0), y `Camera.h` declara las seis variantes con esos nombres.

## Grupo C · pelea (`game`)

Las tres son métodos de `CTaskSimpleFight` (`this` en ECX) dentro del bloque contiguo 0x61C470 a 0x629920 de la clase; ninguna llama a CRT ni a RenderWare salvo `RpHAnimIDGetIndex` en `FightHitCar`. Banco `Dependencies21C`, 1536 pares cada una.

| Dirección | Identidad | Evidencias |
| --- | --- | --- |
| 0x0061D5F0 | `CTaskSimpleFight::FightSetUpCol(float)` | `ret 4`; único colaborador `CColSphere::Set` (0x40FD10); escribe el `CColModel` estático 0xC17824 que sólo leen `FightStrike` y `CTaskGangHassleVehicle::ControlSubTask`. |
| 0x0061D0B0 | `CTaskSimpleFight::FightHitCar(...)` | `ret 0x18`, marco SEH propio; `CCrime::ReportCrime(CRIME_HIT_CAR)`, eventos `CEventVehicleDamageWeapon`, `Fx_c::AddSparks`; llamadores 0x624577 y 0x66412C. |
| 0x006240B0 | `CTaskSimpleFight::FightStrike(CPed*, CVector&)` | `ret 8`; indexa `m_aComboData` (0xC170D0); llamadores en `CTaskSimpleFight::ProcessPed` y `CTaskSimpleUseGun::ProcessPed`; llama a `FightHitPed/Obj/Car`. |

Hallazgo: `TaskGangHassleVehicle.cpp` escribía `[0x20]`, `[0x21]` y `[0x23]` sobre la tarea; en el EXE son `[0x24]` (combo), `[0x25]` (movimiento) y `[0x27]` (último comando).

## Grupo D · `CTaskSimpleGangDriveBy` (`game`)

Las cuatro son de la clase (vtable 0x86D944, tamaño 0x44, tipo 0x3FE) y ocupan el hueco entre funciones ya enganchadas de la misma clase (0x6218B0 `GetTaskType`, 0x6218C0 destructor, 0x6236D0 `Clone`, 0x62D290 `MakeAbortable`, 0x62D3B0 `ProcessPed`). Banco `Dependencies21D`.

| Dirección | Identidad | Pares | Evidencias |
| --- | --- | ---: | --- |
| 0x006217D0 | `CTaskSimpleGangDriveBy::Constructor` (antes sin nombre, `FUN_006217d0`) | 1024 | `ret 0x18`, prólogo SEH, llama al constructor de `CTaskSimple` (0x61A390) y escribe la vtable 0x86D944; 13 llamadores con la forma `new CTaskSimpleGangDriveBy(...)`. |
| 0x00621960 | `CTaskSimpleGangDriveBy::PlayerTarget` | 2048 | `ret 4`; lee `TheCamera`, `fpatan` y tabla de 4 casos para la animación; único llamador 0x62D59B, rama del jugador. |
| 0x00627600 | `CTaskSimpleGangDriveBy::AITarget` | 4096 | `ret 4`; distancia, rumbo y estilo (9 estilos) con rango del arma; único llamador 0x62D5A2, rama de NPC. |
| 0x00628350 | `CTaskSimpleGangDriveBy::AimGun` | 4096 | Apunta el arma por IK (`CPedIK::PointGunInDirection`), no dispara (el disparo es 0x627CC0); único llamador 0x62D813. |

`GangWars.cpp` pasó de la llamada por dirección a `new CTaskSimpleGangDriveBy(...)`.

## Grupo E · ayudantes de población, zona y cámara de tarea (`game`)

Banco `Dependencies21E`.

| Dirección | Identidad | Pares | Evidencias |
| --- | --- | ---: | --- |
| 0x00610DB0 | `CPopulation::RandomIntInLimitRange` | 1536 | Cuerpo de `CGeneral::GetRandomNumberInRange` con límites 1 y 5 de 0x86C6C4 y 0x86C6C8 (`rand` 0x821B1E, `fimul`, `_ftol2`); sólo lo llama `CPopulation::AddToPopulation` (0x614AC6); zona entre `CPopCycle::Update` y `CPopulation::Initialise`. |
| 0x0072D6D0 | `CZoneDef::GetCenter` | 1536 | Entre `CCullZones::Init` (0x72D6B0) y `CZoneDef::IsPointWithin` (0x72D850); lee la disposición `int16[8]` de la zona; único llamador 0x529312 en `CCamera::CamControl`. |
| 0x0063A380 | `CTaskComplexEnterCar::GetCameraStickModifier` | 3072 | Entre `GetTargetPos` y `GetCameraAvoidVehicle` de la misma clase; llamador 0x523E9B en `CCam::Process_FollowPed_SA` tras `GetTaskType() == 701`; usa `m_Car` y `m_CamMovementChoice`. |

## Grupo F · caché D3D (`game`, confianza media-alta)

La procedencia histórica (código de Rockstar o RenderWare modificado) no es observable en el binario. Se decidió `game` porque la capa `D3DResourceSystem` es una unidad única propia del juego (0x730510 a 0x730D30), sus llamadores son todos del juego y la capa depende de RenderWare, no al revés. Banco `Dependencies21F`, 2048 pares cada una.

| Dirección | Identidad | Evidencias |
| --- | --- | --- |
| 0x007305E0 | `D3DResourceSystem::TidyUpD3DTextures(uint32)` | Llamadores `CGame::TidyUpMemory` (0x53C518), `CGame::DrasticTidyUpMemory` (0x53C812) y `CMenuManager::Process` (0x57B46C); cdecl con `imul 0x1C` (cabecera `D3DTextureBuffer`), único colaborador `IUnknown::Release`. |
| 0x00730740 | `D3DResourceSystem::TidyUpD3DIndexBuffers(uint32)` | Mismos tres llamadores (0x53C522, 0x53C822, 0x57B465); bucle de 16 vueltas sobre `D3DIndexDataBuffer` (0x18) con contadores globales 0xC87FD4 y 0xC87FD8 inicializados por `Init` con el `operator new` del juego. |

## Grupo G · RenderWare del juego y núcleo (`game`)

Banco `Dependencies21G`.

| Dirección | Identidad | Pares | Evidencias |
| --- | --- | ---: | --- |
| 0x00734940 | `RwHelper::GetFirstTexture` | 1024 | Bloque contiguo 0x734810 a 0x734E90 de `RwHelper` con vecinos ya validados (`GetFirstAtomic` 0x734820, `GetFirstObject` 0x7348C0, `GetFirstChild` 0x734900); mismo patrón de 37 bytes; 7 llamadores, todos del juego. |
| 0x00734DE0 | `RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial` | 1536 | Entre `SetFilterModeOnClumpsTextures` (0x734DC0) y `RwTexDictionaryFindHashNamedTexture` (0x734E50); recorre las mallas sin `firstMeshOffset` (código de cliente, no accesor de la biblioteca); único llamador `CCustomCarPlateMgr::MaterialUpgradeSetCarplateTextureCB`. |
| 0x0059C300 | `CQuaternion::Slerp` (variante `halftheta`) | 4096 | `thiscall` con cinco argumentos y `ret 0x14`, no es `RtQuatSetupSlerpCache`; comparte bloque con `Set-matrix` (0x59C3E0) y `Set-axis-angle` (0x59C600); llamadores sólo de animación (`CAnimBlendNode::Update` y `UpdateCompressed`). |

## Estado de cada residuo

Ninguna quedó como incierta. Límites que se conservan: confianza media-alta para el grupo F; colaboradores sustituidos por falsos en los bancos de B, C, D y G (el banco comprueba el contrato de la función, no los colaboradores); `dependency_review=null` en las 21.
