<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CCamera (banco final 4)

Banco par a par dentro del juego completo (original con los hooks de la clase desactivados frente a candidata con hooks activos) para las
**91 funciones** pendientes de `CCamera` (`docs/hooks.csv`, `class=CCamera`, `validated=0`): **19648 pares previstos**. Sin ejecutar todavia:
pendiente de compilar y correr (el encargo prohibia compilar y ejecutar GTA).

Comando: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh CameraFinal4 3`

Filtro opcional: `camerafinal4-only.txt` en la carpeta del juego con `CCamera::Funcion`.

## Correcciones de fuente (`source/game_sa/Camera.cpp` / `Camera.h`), con evidencia del EXE
- `Init` (0x5BC520): la fuente anterior era otra reconstruccion. El EXE hace `rep stos` de 0x358 dwords desde +0x18 (5BC535), llama a
  `InitialiseScriptableComponents` con ecx=0xB6F028 (5BC539), `CCam::Init` x3 (5BC55E/563/56E) y luego escribe las constantes (p. ej. `OneZExtra`
  = 0x3D75C28F, NO 0.06f). `FindPlayerVehicle` se llama dos veces con vehiculo (5BC729/74F) y el jugador se lee en linea. Se reescribe entera.
- `Constructor` (0x51A450): el EXE solo escribe CPlaceable (54F1E0), vtable 0x8630E8, `CCam::Init` x3, punteros de rutas a 0, +0x40/+0x44 de las seis
  matrices, `CamTweak` a (-1,1,1,0) (50E470), `m_nShakeType`, `m_matrix`, factores de agachado e `InitialiseScriptableComponents`. Los inicializadores de
  miembro de C++ escribian muchos bytes mas (matrices a identidad, tweaks a cero...). Se transcribe a mano.
- `StartTransition` (0x515200): angulo del ped con `GetATanOfXY - 0x858FE4` (no el `HALF_PI` de common.h); los modos de arma usan `fpatan(x,y)` directo de la matriz
  (5153F6..515417), no `GetATanOfXY`; `FOLLOWPED` desde `CAM_ON_A_STRING` suma SIEMPRE 235 grados (0x8631DC; el EXE normaliza un 0.0 en 5154CF..515523);
  `CAM_ON_A_STRING` guarda el angulo sin sumar (5155C8/5155DB); `PED_DEAD_BABY` usa 0x3E860A92 (5155F8); `PI` = 0x858CB8; se elimina el codigo muerto de
  `m_bItsOkToLookJustAtThePlayer`; con vehiculo se llama 3 veces a `FindPlayerVehicle`; `[0xB70044]` en vez de 0.0 en AIMWEAPON (5157B4).
- `UpdateTargetEntity` (0x50C360): reescrita desde el asm (orden de `CleanUpOldReference`/`FindPlayerPed`/`FindPlayerVehicle`/`RegisterReference`; `ENTER_CAR` y
  `CARJACK/OPEN_DOOR` toman `m_pVehicle` del jugador y no `FindPlayerEntity`; deteccion de drive-by por tipo 0x3FE con dos `GetSimplestActiveTask`; velocidad
  `(x2+y2)+z2 > 0x8CCC80`).
- `TakeControl` (0x50C7C0): con `mode==NONE` el tipo se lee de la entidad objetivo ACTUAL (50C840), no de la nueva; `FindPlayerPed(-1)` final (50C8A0).
- `TakeControlAttachToEntity` (0x50C910): antes llamaba al propio EXE (`plugin::CallMethod`). Reconstruida (`attached` por defecto = entidad del jugador; comparacion
  `lookAt != offset` de 0x509760; desplazamiento por defecto (0,0,2); modo 0x2F).
- `Restore` / `RestoreWithJumpCut` (0x50B930 / 0x50BAB0): jugador en linea y `FindPlayerVehicle` dos veces; a `RestoreWithJumpCut` le faltaba `m_bCameraJustRestored=true` y los dos
  permisos de primera persona a false (50BAED/50BAF0/50BAF3), y lee los jugadores 0/1 de la tabla (B7CD98/B7CF28), no con `FindPlayerPed`.
- `ProcessFade` (0x50B5D0): con duracion 0 el EXE pone alfa=0 (FADE_OUT) o 255 (FADE_IN); el tiempo es `paso * 0.02f` (0x858B38), no `paso / 50`; el valor se trunca con
  `_ftol2`. `ProcessMusicFade` (0x50B6D0): mismo `paso * 0.02f` y orientacion de las comparaciones del EXE.
- `Fade` (0x50AC20): `min(max(d*0.3, 0.3), d)` con las comparaciones del EXE (`fcom`/`fcomp`).
- `CamShake` / `CamShakeNoPos` (0x50A9F0 / 0x50A970): el tiempo transcurrido es `fild * 0.001f` (0x858CDC) con un solo redondeo; la distancia es raiz de (dz2 + raiz(dx2+dy2)2);
  `1 - d * 0.01` con los double 0x85A308/0x85A310; tope 2.0 antes que 0.0; la camara activa sale de `this`.
- `AddShakeSimple` / `LerpFOV` (0x50D240 / 0x50D280): el fin se suma al reloj SIN pasar antes por float (`fild`, `fst`, `fadd`); `ProcessFOVLerp-1` (0x516500) compara y resta con el reloj sin redondear.
- `GetScreenRect` (0x50AB50): `(h/2 * %) * 0.01f - (h * 1/448) * 22` (0x859524) en lugar de `/100.f` y `SCREEN_SCALE_Y`. `DrawBordersForWideScreen` (0x514860): el CRect del EXE es
  (izq, arriba, der, abajo); el constructor de la DLL recibe (izq, ABAJO, der, ARRIBA) y colocaba mal los campos (y su `assert` de rectangulo invertido podia saltar).
- `GetCutSceneFinishTime` / `SetPercentAlongCutScene` (0x50AD90 / 0x50C070): el codigo anterior COPIABA la otra camara sobre la activa (`cam = m_aCams[..]`); ahora es un puntero. El porcentaje es
  `(pct * 0.01f) * tiempo_final` (fild, +2^32 si es negativo).
- `SetParametersForScriptInterpolation` (0x50C030): `* 0.01f` en lugar de `/ 100.0f`.
- `Find3rdPersonQuickAimPitch` (0x50AD40) y `Find3rdPersonCamTargetVector` (0x514970): x87 con `fptan`/`fpatan` y el orden del EXE (`tan * (vy2 * (1/aspecto))`, proyeccion `(dx*ux + dz*uz) + dy*uy`).
- `CalculateMirroredMatrix` (0x50B380): producto escalar sumado en el orden z, y, x y factor 2 aplicado al resultado.
- `CalculateGroundHeight` (0x514B80): bandera de inicializacion 0xB70040 bit 0; el modelo de colision sale de `CModelInfo` (movsx del indice); el caso por defecto devuelve `bbBottom` en lugar de abortar.
- `ConeCastCollisionResolve` (0x51A5D0): destino = `(pos - lookAt) * dist + lookAt` (no la `lerp` generica) y esferas con material 0, pieza 0 e iluminacion 0xFF.
- `ResetDuckingSystem` (0x50CEF0): compara el cuadrado de la velocidad con 0x8630EC (no el modulo con 1e-6). `CameraVehicleModeSpecialCases` (0x50CDE0): compara el cuadrado con 0x8CCC88^2 y el valor lento es 0x3C23D70B.
- `CameraGenericModeSpecialCases` (0x50CD30): guarda la entidad SOSTENIDA (`task+8`), no el ped. `IsExtraEntityToIgnore` (0x50CE80): recorre solo `m_nExtraEntitiesCount` entradas.
- `SetColVarsPed` (0x50CC50): otros tipos de ped no tocan nada. `SetZoomValueFollowPedScript` (0x50C160): solo 0/1/2 cambian el valor. `SetZoomValueCamStringScript` (0x50C1B0): el tipo se comprueba con
  `(byte[+0x36] & 7)==2` (vehiculo), no con `GetStatus`; sin `VERIFY`; modos distintos de 0/1/2 no cambian el valor.
- `SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString` (0x50BDA0): rumbo con `GetATanOfXY` (no `GetHeading`); `..InFront_ForAPed` (0x50BE30): `TheCamera.m_pTargetEntity`, no `this`.
- `SetMotionBlur` (0x50BF40): argumentos de 32 bits (los miembros son uint32). `ApplyVehicleCameraTweaks` (0x50A480): se saltan las entradas con `ModelID` negativo y se escribe `m_nCurrentTweakModelIndex`.
  `InitialiseCameraForDebugMode` (0x50AF90): el EXE es un `ret` (se quita el codigo de depuracion). `DeleteCutSceneCamDataMemory` (0x5B24A0): `operator delete` sin tamano.
- Sin cambios (MATCH con el asm): AllowShootingWith2PlayersInCar, CameraPedAimModeSpecialCases, CameraPedModeSpecialCases, ClearPlayerWeaponMode, ConsiderPedAsDucking, DealWithMirrorBeforeConstructRenderList,
  DontProcessObbeCinemaCamera, Enable1rstPersonCamCntrlsScript/WeaponsCamera, FindCamFOV, FinishCutscene, GetArrPosForVehicleType (tabla de salto 50AF64), GetFading, GetFadingDirection, GetGameCamPosition,
  GetLookDirection, GetLookingForwardFirstPerson, GetLookingLRBFirstPerson, GetPositionAlongSpline, GetRoughDistanceToGround, GetScreenFadeStatus, Get_Just_Switched_Status, InitCameraVehicleTweaks,
  InitialiseScriptableComponents, ProcessObbeCinemaCameraPed, ProcessScriptedCommands, ProcessWideScreenOn, RenderMotionBlur (el EXE llama a un `ret`, 0x71D700), RestoreCameraAfterMirror, SetCamCollisionVarDataSet,
  SetCamCutSceneOffSet, SetCamPositionForFixedMode, SetCameraDirectlyBehind/InFront (sin ForAPed), SetCameraUpForMirror, SetColVarsAimWeapon/Vehicle, SetFadeColour, SetMotionBlurAlpha, SetNearClipScript,
  SetNewPlayerWeaponMode, SetRwCamera (el EXE llama ademas a 0x71D710, que solo devuelve 1), SetWideScreenOn/Off, Start/StopCooperativeCamMode, StartTransitionWhenNotFinishedInter, StoreValuesDuringInterPol,
  TakeControlNoEntity, TakeControlWithSpline, UpdateAimingCoors, Using1stPersonWeaponMode.

## Metodo
- Camara real (`TheCamera`, 0xD78 bytes) guardada y restaurada en cada ejecucion; mundo falso estatico (`Support.hpp`/`Ctx.hpp`): peds, vehiculos, matrices, tareas con vtable propia, datos de handling, RwCamera,
  edificio con modelo de colision en una ranura sin uso de `ModelInfo` (19995). `CWorld::Players` y `PlayerInFocus` se guardan y restauran.
- Dobles trazados con `EntryPatch` en la direccion del EXE, el simbolo de la DLL y su cuerpo (`Body()` calculado antes de parchear; los `EntryPatch` viven en almacenamiento estatico y el conjunto se cierra antes de
  construir el json): `RegisterReference`/`CleanUpOldReference`, `FindPlayerPed`/`FindPlayerVehicle` (con la tabla de jugadores falsa), `GetATanOfXY`, `GetTaskHold`/`GetTaskDuck`, `IsCoopGameGoingOn`, `CanPedOpenLocks`,
  audio, `ProcessVerticalLine`, cono de colision, `CSprite2d::DrawRect` y `RwRenderStateSet` (parche temporal del puntero en `RwEngineInstance+0x20`), operaciones de `CMatrix` (operator=, Invert, SetScale, Attach),
  `CPlaceable::SetMatrix`, `operator delete`, `CCam::Init`, `Get_TwoPlayer_AimVector` y los colaboradores de la propia clase en las pruebas que los necesitan.
  `GetSimplestActiveTask` NO se dobla: el administrador de tareas es real (CTaskManager en `intel+4` con una tarea falsa cuya vtable devuelve sin subtarea y el tipo del fixture).
- Los hilos ajenos al del banco no se trazan. x87 a 24 bits durante la llamada. Cada global tocado se guarda y restaura en cada ejecucion y al final.
- Se compara: retorno, traza (nombre, orden y argumentos de cada llamada saliente), hash de la camara completa, del mundo falso y de los globales guardados, mas salidas por referencia. Los fallos guardan
  `diff_values`, `diff_blocks` (camara) y `diff_world` (mundo falso) para separar ruido de diferencia real.

## Corpus: 19648 pares

| Funcion | Direccion | Pares |
|---|---|---:|
| CCamera::AddShakeSimple | 0x50D240 | 256 |
| CCamera::AllowShootingWith2PlayersInCar | 0x50C280 | 64 |
| CCamera::ApplyVehicleCameraTweaks | 0x50A480 | 256 |
| CCamera::CalculateGroundHeight | 0x514B80 | 512 |
| CCamera::CalculateMirroredMatrix | 0x50B380 | 512 |
| CCamera::CamShake | 0x50A9F0 | 512 |
| CCamera::CamShakeNoPos | 0x50A970 | 256 |
| CCamera::CameraGenericModeSpecialCases | 0x50CD30 | 128 |
| CCamera::CameraPedAimModeSpecialCases | 0x50CDA0 | 128 |
| CCamera::CameraPedModeSpecialCases | 0x50CD80 | 64 |
| CCamera::CameraVehicleModeSpecialCases | 0x50CDE0 | 256 |
| CCamera::ClearPlayerWeaponMode | 0x50AB10 | 64 |
| CCamera::ConeCastCollisionResolve | 0x51A5D0 | 512 |
| CCamera::ConsiderPedAsDucking | 0x50CEB0 | 128 |
| CCamera::Constructor | 0x51A450 | 64 |
| CCamera::DealWithMirrorBeforeConstructRenderList | 0x50B510 | 256 |
| CCamera::DeleteCutSceneCamDataMemory | 0x5B24A0 | 128 |
| CCamera::DontProcessObbeCinemaCamera | 0x50AB40 | 64 |
| CCamera::DrawBordersForWideScreen | 0x514860 | 256 |
| CCamera::Enable1rstPersonCamCntrlsScript | 0x50AC00 | 64 |
| CCamera::Enable1rstPersonWeaponsCamera | 0x50AC10 | 64 |
| CCamera::Fade | 0x50AC20 | 512 |
| CCamera::Find3rdPersonCamTargetVector | 0x514970 | 512 |
| CCamera::Find3rdPersonQuickAimPitch | 0x50AD40 | 256 |
| CCamera::FindCamFOV | 0x50AD20 | 64 |
| CCamera::FinishCutscene | 0x514950 | 256 |
| CCamera::GetArrPosForVehicleType | 0x50AF00 | 128 |
| CCamera::GetCutSceneFinishTime | 0x50AD90 | 128 |
| CCamera::GetFading | 0x50ADE0 | 64 |
| CCamera::GetFadingDirection | 0x50ADF0 | 64 |
| CCamera::GetGameCamPosition | 0x50AE50 | 64 |
| CCamera::GetLookDirection | 0x50AE90 | 128 |
| CCamera::GetLookingForwardFirstPerson | 0x50AED0 | 128 |
| CCamera::GetLookingLRBFirstPerson | 0x50AE60 | 128 |
| CCamera::GetPositionAlongSpline | 0x50AF80 | 64 |
| CCamera::GetRoughDistanceToGround | 0x516B00 | 128 |
| CCamera::GetScreenFadeStatus | 0x50AE20 | 128 |
| CCamera::GetScreenRect | 0x50AB50 | 256 |
| CCamera::Get_Just_Switched_Status | 0x50AE10 | 64 |
| CCamera::Init | 0x5BC520 | 128 |
| CCamera::InitCameraVehicleTweaks | 0x50A3B0 | 128 |
| CCamera::InitialiseCameraForDebugMode | 0x50AF90 | 64 |
| CCamera::InitialiseScriptableComponents | 0x50D2D0 | 64 |
| CCamera::IsExtraEntityToIgnore | 0x50CE80 | 256 |
| CCamera::LerpFOV | 0x50D280 | 256 |
| CCamera::ProcessFOVLerp-1 | 0x516500 | 512 |
| CCamera::ProcessFade | 0x50B5D0 | 512 |
| CCamera::ProcessMusicFade | 0x50B6D0 | 512 |
| CCamera::ProcessObbeCinemaCameraPed | 0x50B880 | 64 |
| CCamera::ProcessScriptedCommands | 0x516AE0 | 256 |
| CCamera::ProcessWideScreenOn | 0x50B890 | 128 |
| CCamera::RenderMotionBlur | 0x50B8F0 | 64 |
| CCamera::ResetDuckingSystem | 0x50CEF0 | 256 |
| CCamera::Restore | 0x50B930 | 512 |
| CCamera::RestoreCameraAfterMirror | 0x51A5A0 | 128 |
| CCamera::RestoreWithJumpCut | 0x50BAB0 | 512 |
| CCamera::SetCamCollisionVarDataSet | 0x50CB60 | 128 |
| CCamera::SetCamCutSceneOffSet | 0x50BD20 | 64 |
| CCamera::SetCamPositionForFixedMode | 0x50BEC0 | 64 |
| CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString | 0x50BD40 | 128 |
| CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString | 0x50BDA0 | 256 |
| CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString | 0x50BD70 | 128 |
| CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString | 0x50BE30 | 256 |
| CCamera::SetCameraUpForMirror | 0x51A560 | 128 |
| CCamera::SetColVarsAimWeapon | 0x50CBF0 | 128 |
| CCamera::SetColVarsPed | 0x50CC50 | 256 |
| CCamera::SetColVarsVehicle | 0x50CCA0 | 256 |
| CCamera::SetFadeColour | 0x50BF00 | 256 |
| CCamera::SetMotionBlur | 0x50BF40 | 128 |
| CCamera::SetMotionBlurAlpha | 0x50BF80 | 64 |
| CCamera::SetNearClipScript | 0x50BF90 | 64 |
| CCamera::SetNewPlayerWeaponMode | 0x50BFB0 | 64 |
| CCamera::SetParametersForScriptInterpolation | 0x50C030 | 128 |
| CCamera::SetPercentAlongCutScene | 0x50C070 | 256 |
| CCamera::SetRwCamera | 0x50C100 | 128 |
| CCamera::SetWideScreenOff | 0x50C150 | 64 |
| CCamera::SetWideScreenOn | 0x50C140 | 64 |
| CCamera::SetZoomValueCamStringScript | 0x50C1B0 | 256 |
| CCamera::SetZoomValueFollowPedScript | 0x50C160 | 128 |
| CCamera::StartCooperativeCamMode | 0x50C260 | 64 |
| CCamera::StartTransition | 0x515200 | 1024 |
| CCamera::StartTransitionWhenNotFinishedInter | 0x515BC0 | 256 |
| CCamera::StopCooperativeCamMode | 0x50C270 | 64 |
| CCamera::StoreValuesDuringInterPol | 0x50C290 | 256 |
| CCamera::TakeControl | 0x50C7C0 | 512 |
| CCamera::TakeControlAttachToEntity | 0x50C910 | 512 |
| CCamera::TakeControlNoEntity | 0x50C8B0 | 256 |
| CCamera::TakeControlWithSpline | 0x50CAE0 | 128 |
| CCamera::UpdateAimingCoors | 0x50CB10 | 64 |
| CCamera::UpdateTargetEntity | 0x50C360 | 1024 |
| CCamera::Using1stPersonWeaponMode | 0x50BFF0 | 128 |

## Fuera del fixture / supuestos a revisar al compilar
- Ninguna funcion de la lista queda fuera: las 91 tienen casos.
- Dominio de los fixtures: booleanos 0/1, floats moderados (sin NaN/Inf/denormales), `bInVehicle` implica `m_pVehicle`, matrices de ped/vehiculo siempre presentes (el EXE las lee sin comprobar), `m_nExtraEntitiesCount` 0..2
  (0..1 en las funciones que anaden entradas), entidades objetivo no nulas donde el EXE las desreferencia.
- `GetATanOfXY` se dobla con una funcion determinista. Las llamadas desde `__asm` usan el puntero a la funcion de la DLL, que el doble tambien parchea.
- `ProcessScriptedCommands` dobla sus tres colaboradores (`ProcessVectorMoveLinear-1`, `ProcessVectorTrackLinear-1`, `ProcessFOVLerp-1`): solo se valida orden y receptor. `GetRoughDistanceToGround` dobla `CalculateGroundHeight`.
- Se asume `FrontEndMenuManager.m_bStartGameLoading` = byte 0xBA67A5 (en `Init` se usa la direccion directa), `bInVehicle` = bit 0x100 y `bIsDucking` = bit 0x4000000 de +0x46C, `CTask::GetTaskType` = ranura 4 de la vtable
  y `GetSubTask` = ranura 2 (por el orden de `Task.h`); el evento `layout` comprueba los desplazamientos y los valores de las enumeraciones que usan los fixtures.
- Las llamadas a `CMatrix::operator=`, `Invert`, `SetScale` y `Attach` se doblan en todas las pruebas que las usan: se valida CCamera y no la implementacion de CMatrix.
- `CCamera::Init`/`Constructor` usan `CCam::Init` doblado: no se valida el contenido de las tres `CCam`.
- Avisos esperables del compilador: constantes `kExe*` sin usar y variables `out`/`ret` sin usar en los casos que no devuelven nada.

## Aprobación (10/10/2026)

Corrida `20261010-130000-406558`: passed=true, 19648 pares, 91 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_camerafinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco CCamera queda sin pendientes. Límites declarados en el alcance de cada función y en las secciones anteriores de este README.
