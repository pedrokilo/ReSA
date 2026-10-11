<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FootWave4: CTaskSimplePlayerOnFoot (las 5 funciones pendientes)

Banco simple (estilo Small2/PadMouse1/TaskWave2A: corre dentro del juego completo con partida cargada via FastLoader y se engancha en `CGame::Process` con `probe_build.py`). **Escrito sin compilar ni ejecutar** (regla del encargo): al compilar pueden aparecer errores de nombres/tipos y avisos `error` de diseno (ver "Supuestos a revisar").

Uso: `bash reversing/_handoff/bench.sh FootWave4 3`. Filtro opcional: `footwave4-only.txt` en la carpeta del juego con `CTaskSimplePlayerOnFoot::<Funcion>`.

Deja COMPLETA la clase `CTaskSimplePlayerOnFoot` (hooks.csv): las otras 5 funciones (Constructor, Destructor, Clone, MakeAbortable y ProcessPed, mas PlayerControlFighter ya validada) estan cubiertas por TaskWave2A / el registro previo.

## Funciones y casos: 5120 pares

| Clase | Funcion | EXE | Casos |
|---|---|---|---|
| CTaskSimplePlayerOnFoot | PlayIdleAnimations | 0x6872C0 (`ret 4`, thiscall) | 1024 |
| | PlayerControlZeldaWeapon | 0x687C20 (estatica, `ret 4` en el EXE, cdecl en la DLL) | 1024 |
| | PlayerControlDucked | 0x687F30 (estatica, igual) | 1024 |
| | PlayerControlZelda | 0x6883D0 (`ret 8`, devuelve int) | 1024 |
| | ProcessPlayerWeapon | 0x6859A0 (`ret 4`) | 1024 |

Original = hook de la funcion desactivado (codigo del EXE); candidato = hook activo (fuente de la DLL). Para las dos estaticas el original entra por la direccion del EXE con convencion stdcall y el candidato llama directamente a la funcion de la DLL (direccion tomada del hook, `GetHookOurAddress`) con cdecl.

## Correcciones de fuente (direccion + instruccion del EXE)

`source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp` (el `.h` no cambia). Los puntos de NaN siguen la leccion 25 (`fcomp + fnstsw + test ah,..`).

PlayIdleAnimations 0x6872C0
- 0x6872CE `call 609560`: el pad se pide ANTES del modo cooperativo (0x6872D5..0x6872E5, lectura inline de `CWorld::Players[0/1].m_pPed` = 0xB7CD98 / 0xB7CF28); antes se pedia despues.
- 0x687300 `test byte [ebp+478h],2`: la bandera es `bUsingMobilePhone` (byte 13, bit 1), no `bCrouchWhenShooting` (bit 25 de 0x46C).
- 0x687323 `cmp dword [ebp+534h],1 / jg`: comparacion CON SIGNO del estado de movimiento (en la DLL `eMoveState` es `uint32`).
- 0x687393 `cmp ebx,2710h / jle` y 0x687424 `sub edx,ecx / cmp edx,4E20h / jle`: el retardo y la resta con gLastTouchTimeDelta se comparan con signo (antes `uint32`).
- 0x68740D `test byte [ebp+46Ch],0Ch / jne`: sale si esta activa CUALQUIERA de `bIsLooking` / `bIsRestoringLook` (antes solo si lo estaban las dos).
- 0x6873BD..0x687406: la busqueda de animaciones del bloque va ANTES de las banderas y del retardo, recorre con `RpAnimBlendClumpGetFirstAssociation` (0x4D15E0) / `RpAnimBlendGetNextAssociation` (0x4D6AB0) y el indice se prueba `first <= idx < first + num` (`jl` + `jl`, final exclusivo); `CAnimManager::IsAnimInBlock` de la DLL usaba `<=` (final inclusivo). El bucle de 0x6874E3..0x68750C (animaciones 0x200 al estar tocando) usa las mismas dos funciones.
- 0x687430 `push 4 / push 0 / call 407180`: aleatorio de `CGeneral::GetRandomIntInRange(0, 4)` (`rand * (1/32768) * 4`), no la plantilla entera `GetRandomNumberInRange<int>` (reparto distinto con 1/32767).

PlayerControlZeldaWeapon 0x687C20
- 0x687D73 `fsin`, 0x687D99 `fcos` y 0x687EA9 `fpatan` (precision completa del x87): -sin/cos con `FighterDirection` y atan2 con `fpatan` (`Atan2X87`); la DLL usaba `std::sin/cos/atan2`. Resto MATCH (orden de llamadas al pad, 1/128, formulas de matriz `cos*r.y + nsin*r.x + r.z*0`, `fcom 1.0 / test ah,41h / jne`).

PlayerControlDucked 0x687F30
- 0x687F5F `call 53FD30` / 0x687F75 `call 53FC90` y 0x687FE2 `mov al,[ebp+18h]` / 0x687FED `[ebp+19h]`: primero se leen arriba/abajo y luego izquierda/derecha del pad y SOLO DESPUES se mira `m_bIsFinished / m_bIsAborting` (antes salia antes de tocar el pad e invertia el orden de las lecturas).
- 0x6880C8..0x6880E9 y 0x68821D..0x68823A: la direccion que recibe `IsPlayerAllowedToGoInThisDirection` es (-sin, cos, 0) (`fchs` + `[edx]`, `fcos` + `[edx+4]`); la DLL pasaba (0, -sin, 0) y usaba `moveDirection = (0, -sin, cos)` en el reparto por matriz de 0x688104..0x68814E.
- 0x6882C9 / 0x688343 `fcomp 0.5 / test ah,41h / jne`: sale si NO `ratio > 0.5` (con NaN tambien); antes `ratio <= 0.5`.
- 0x68829B / 0x6882A8: dos llamadas a `GetTaskUseGun` antes de leer `m_WeaponInfo` (solo cuenta de llamadas).
- Trigonometria con `fsin/fcos/fpatan` (0x6880B3, 0x6880D8, 0x68820B, 0x688229, 0x688188) como arriba.

PlayerControlZelda 0x6883D0
- 0x688491 `fcomp 0.0 / test ah,41h / jne 688569`: `!(ratio > 0)` (con NaN pone m_fMoveBlendRatio a 0 y no actualiza); antes `ratio <= 0`.
- 0x68849C..0x6884C4: el angulo (GetRadianAngleBetweenPoints - orientacion, LimitRadianAngle) solo se calcula dentro de la rama de actualizacion (antes siempre).
- 0x68852D `fcomp st(1) / test ah,41h / jne 68853E`, 0x68854B `fcompp / test ah,41h / jne 68855E`: `(ratio-cur) > max` -> cur += max; si no, `!(-max > ratio-cur)` -> cur = ratio (con NaN tambien); si no cur -= max. La DLL con NaN caia en `cur = max + cur`.
- 0x6885A7..0x6885B0 `mov eax,[eax+40h] / test al,4 / test al,8`: la entidad sobre la que se esta (`m_standingOnEntity`, 0x568) impide mirar el sprint solo si sus banderas FISICAS (+0x40) tienen el bit 2 activo y el bit 3 apagado; la DLL probaba `m_bIsStatic`/`m_bHasContacted` de CEntity (+0x1C).
- 0x688687..0x6886C5 y 0x6886CB: `DuckJustDown` (0x540720) y `CanPedDuck` (0x692610) se consultan SIEMPRE; con `bAvoidJumpingAndDucking` solo se omite `SetTaskDuckSecondary(0)` y todo el bloque de salto (el `test [esp+40h]` esta despues). La DLL volvia antes de consultar el pad.
- 0x6884D1..0x688508: (-sin, cos, 0) con `fsin/fcos` del x87 (`FighterDirection`).

ProcessPlayerWeapon 0x6859A0
- 0x6862F4..0x68632D (INSTANT_HIT / AREA_EFFECT): con el arma en estado 2 el comando es AIM solo si `GetTarget` (0x540670) o hay objetivo o `m_bFreeAiming` (bit 3 de +0x34); si no, `je 6860AD` salta TODO el control del arma (incluida la puesta a cero de `m_fLookPitch`); la DLL devolvia siempre AIM.
- 0x686AF9..0x686B0D (`mov ecx,[edi+47Ch]` con edi = objetivo): `IsInSeeingRange` (0x600C60) y `GetActivePrimaryTask` (0x4B85B0) se piden a la inteligencia del OBJETIVO, no a la del jugador.
- 0x686BE4..0x686C08 (`mov edi,[edi+47Ch] / lea ecx,[edi+68h] / call 4AB420`): `CEventGunAimedAt` se anade al grupo de eventos del OBJETIVO (la DLL usaba el del jugador).
- 0x6871F0 `fcomp 0.0 / test ah,41h / jne 687253`: aborta la mirada si NO `dot > 0` (con NaN tambien); antes `dot <= 0`.
- 0x686F90 `fcomp -0.5 / test ah,5 / jp 686F9B`: `fightMovement.y < -0.5` -> 16, en otro caso (incluye NaN) 15; antes `>= -0.5`.
- 0x686DB5 `fsin / fadd [esp+1Ch]` (seno sin redondear antes de sumar `z`) y 0x6870A9 `fpatan`: `SinPlusX87` y `Atan2X87`.
- 0x68674C `call 59C890` (CMatrix MultiplyMatrixWithVector): suma `up, forward, right` (x) y `up, right, forward` (y, z) mas la posicion; la DLL usaba `CMatrix::TransformPoint` (suma en otro orden): `MultiplyMatrixWithVectorX87`.
- Indexado de armas con `movsx edx, byte [esi+718h]` (`imul 1Ch`): se usa `m_aWeapons.data()[int8(slot)]` (sin comprobar el rango).
- Solo cuenta de llamadas (0x685B11/0x685B20 dos `GetTaskUseGun`, 0x685D7D/0x685D8E dos `GetTaskDuck`, 0x6867D4 primero `GetCrouchReloadAnimationID` y la bandera 0x8D2FE8 y luego `GetTaskDuck`).
- MATCH: todo lo demas (precision 95/100, arma de primera persona, mensajes de camara, melee + muerte sigilosa, USE/detonador/gafas, lanzables, lanzacohetes, camara de fotos, recarga, bloque de apuntado y la mirada IK).

## Colaboradores doblados (parcheados con `EntryPatch` en la direccion del EXE y en la funcion de la DLL, solo en el hilo del banco)

El EXE y la DLL usan el mismo comportamiento y la misma traza. La direccion de la DLL sale del propio hook (`Simple::GetHookOurAddress` / `Virtual::GetHookOurAddress`, tambien para virtuales como `CEventDamage::AffectsPed` 0x4B35A0, cuya llamada directa solo se intercepta asi); para las funciones sin hook se usa `Addr(&Clase::Miembro)` (FindWeaponLockOnTarget 0x60DC50, FindNextWeaponLockOnTarget 0x60E530, Compute3rdPersonMouseTarget 0x60B650, ControlButtonSprint 0x60A610, SetRealMoveAnim 0x60A9C0, CTaskSimpleFight::ControlFight 0x61C5E0, CPedGroups::GetPedsGroup 0x5F7E80 / AreInSameGroup 0x5F7F40, CGeneral::GetRandomNumber). Si algun hook no se encuentra o su direccion no coincide, el banco escribe un evento `error` con la lista completa (`missing`) antes de parchear nada.

- Aleatorios: rand 0x821B1E y `CGeneral::GetRandomNumber` (cola de 8 valores con al menos dos tramos distintos de [0,4) para que el bucle de PlayIdleAnimations termine).
- CPad (todas las consultas, con `this` = pad falso): SetTouched 0x53F200, GetTouchedTimeDelta 0x53F210, GetPedWalkUpDown 0x53FD30 / LeftRight 0x53FC90 y las variantes con ped 0x540E20 / 0x540DC0, DuckJustDown 0x540720, GetSprint 0x5407A0, JumpJustDown 0x540770, ExitVehicleJustDown 0x540120, GetTarget 0x540670, GetEnterTargeting 0x5406B0, ShiftTargetLeft/RightJustDown 0x540850 / 0x540880, WeaponJustDown 0x540250, MeleeAttackJustDown 0x540390, GetMeleeAttack 0x540340, GetWeapon 0x540180, AimWeaponLeftRight 0x541040 / UpDown 0x5410C0.
- CCamera: Using1stPersonWeaponMode 0x50BFF0, ClearPlayerWeaponMode 0x50AB10, SetNewPlayerWeaponMode 0x50BFB0, UpdateAimingCoors 0x50CB10; CWeaponEffects::ClearCrossHair 0x742C60.
- CPed: Say 0x5EFFE0, GetWeaponSkill 0x5E6580, SetPedState 0x5E4500. CPlayerPed: GetPadFromPlayer 0x609560, ClearWeaponTarget 0x609C80, Clear3rdPersonMouseTarget 0x609ED0, PlayerHasJustAttackedSomeone 0x60D5A0 (salto a 0x60CC50), DoesTargetHaveToBeBroken 0x60C0C0, PedCanBeTargettedVehicleWise 0x609D90, FindWeaponLockOnTarget, FindNextWeaponLockOnTarget, Compute3rdPersonMouseTarget, ControlButtonSprint (devuelve el float en ST0), SetRealMoveAnim.
- Animacion: CAnimManager::BlendAnimation 0x4D4610 (devuelve asociaciones falsas), RpAnimBlendClumpGetAssociation 0x4D68B0, RpAnimBlendClumpGetFirstAssociation 0x4D15E0, RpAnimBlendGetNextAssociation 0x4D6AB0 (lista falsa), CAnimBlendAssocGroup::GetAnimation 0x4CE090, CStreaming::RequestModel 0x4087E0 / SetModelIsDeletable 0x409C10, CStats::GetStatValue 0x558E40, SurfaceInfos_c::CantSprintOn 0x55E870.
- Logica: CGameLogic::IsPlayerUse2PlayerControls 0x442020 / IsPlayerAllowedToGoInThisDirection 0x441E10, CLocalisation::KickingWhenDown 0x56D270.
- Inteligencia y tareas: SetTaskDuckSecondary 0x601230, ClearTaskDuckSecondary 0x601390, TestForStealthKill 0x601E00, IsInSeeingRange 0x600C60, GetActivePrimaryTask 0x4B85B0, CTaskManager::SetTask 0x681AF0 / SetTaskSecondary 0x681B60, CTaskSimpleUseGun (RequirePistolWhip 0x61E200, ControlGun 0x61E040, PlayerPassiveControlGun 0x61E0A0, AbortIK 0x61DFA0, ControlGunMove 0x61E0C0), CTaskSimpleFight::ControlFight, CTaskSimpleThrowProjectile::ControlThrow 0x61F810, CTaskSimpleDuck (CanPedDuck 0x692610, IsTaskInUseByOtherTasks 0x61C3D0, ControlDuckMove 0x6923F0).
- Grupos, eventos, crimenes y audio: CPedGroups::GetPedsGroup / AreInSameGroup, CPedGroupIntelligence::AddEvent 0x5F7470, CEventGroup::Add 0x4AB420, CEventDamage::AffectsPed 0x4B35A0, CPedDamageResponseCalculator::ComputeDamageResponse 0x4B5AC0, CCrime::ReportCrime 0x532010, CAEPedWeaponAudioEntity::AddAudioEvent 0x4E69F0, CWeapon::Fire 0x742300 / IsTypeMelee 0x73B1C0.
- IK: IKChainManager_c::IsLooking 0x6181A0, GetLookAtEntity 0x6181D0, AbortLookAt 0x618280, LookAt 0x618970.

## Lo que se ejecuta de verdad (y por que)

- `CWeaponInfo::GetWeaponInfo` (0x743C60) y `GetCrouchReloadAnimationID` (0x685700), `CGeneral::GetRadianAngleBetweenPoints` (0x53CBE0) / `LimitRadianAngle` (0x53CB50), `CPlaceable::GetForward` (0x41CCB0), `CVector` (0x40FE60, 0x411A00), `CGameLogic::IsCoopGameGoingOn` (0x441390; en el EXE PlayIdleAnimations lo lee inline de 0xB7CD98 / 0xB7CF28, asi que no se puede parchear: se fija `CWorld::Players[1].m_pPed`), `CPedIntelligence::GetTaskUseGun/GetTaskFighting/GetTaskDuck/GetTaskThrow/GetTaskHold` y `CTaskManager::GetTaskSecondary/GetActiveTask/GetSimplestActiveTask`: son puras (solo leen) y estan validadas. Las tareas falsas que recorren son "silenciosas" (el numero de lecturas de GetTaskType no es comparable entre EXE y DLL); solo `GetActivePrimaryTask` devuelve una tarea falsa con vtable trazada.
- Constructores y destructores: de las tareas nuevas (`CTaskSimpleUseGun` 0x61DE60, `CTaskSimpleFight` 0x61C470, `CTaskSimpleStealthKill` 0x6225F0, `CTaskSimpleThrowProjectile` 0x61F660, `CTaskComplexJump` 0x67A030, `CTaskComplexUseGoggles` 0x634EF0; las nuevas se detectan en el pool de tareas, se resumen por tipo + campos que fijan los argumentos del constructor y se borran) y de los objetos de pila `CPedDamageResponseCalculator` (0x4AD3F0 / 0x4AD420), `CEventDamage` (0x4AD830 / 0x4AD960), `CEventGunAimedAt` (0x4B0700 / 0x4B07B0) y `CEventGroupEvent` (0x4ADFD0 / 0x4AE070). No se pueden doblar con seguridad: el compilador los llama directamente sobre objetos de pila (un stub dejaria el objeto sin inicializar y el destructor correria sobre basura) y solo registran/limpian referencias. `CEvent::operator new/delete` (pool de eventos, 0x4B5620) tambien corre de verdad.

## Fixtures

Jugador (buffer `CPlayerPed` completo con desplazamientos crudos del EXE), objetivo ped, otro ped (objetivo del raton), vehiculo (objetivo, suelo y "attached"), inteligencias (tareas secundarias/primarias silenciosas), `CPlayerPedData`, pad (`CPad` completo), grupo falso (inteligencia de grupo en +0x30), clump falso de tipo rpCLUMP, asociaciones de animacion y estaticas falsas, bloques/tabla de grupos de animacion falsos (se intercambia `CAnimManager::ms_aAnimAssocGroups`, 0xB4EA34) y cuatro bloques reales de `ms_aAnimBlocks` (0xB5D4A0) que se salvan y restauran. Globales reales tocados y restaurados tras CADA ejecucion: `CWorld::Players[1].m_pPed`, gLastTouchTimeDelta (0xC19664), gLastRandomNumberForIdleAnimationID (0x8D2FEC), gbUnknown_8D2FE8, gDuckAnimBlendData (0x8D2FF0), `TheCamera` (modo y frente de las 3 camaras, camara activa 0xB6F081, orientacion 0xB6F178, pantalla ancha 0xB6F065, raton 3a persona 0xB6EC2E, salto desde 1a persona 0xB6F048). No se toca ni `CTimer` ni las demas tablas.

Fronteras exactas: retardo 10000/20000 ms y su resta (incluidos valores >= 0x80000000), estado de movimiento 0x80000000, indice de animacion en los bordes del bloque (first-1, first, first+num-1, first+num), +-50 / +-100 del pad, 0.5 y 1.9 del ratio de movimiento, +-0.5 de `m_vecFightMovement.y` (y NaN), +-max de `CTimer::GetTimeStep()*0.07` en PlayerControlZelda (y NaN), tiempo de la camara de fotos (==, +1, +2), banderas fisicas del suelo, escenarios dirigidos de muerte sigilosa y de camara de fotos. Todo con NaN silencioso (nunca senalizante).

## Que se compara

Retorno, memoria del objeto probado (0x1C) y su relleno (ninguna funcion debe escribir fuera), tareas nuevas del pool (tipo + campos fijados por el constructor), memoria de las tareas falsas, huella de todos los buffers (referencias enmascaradas), listas de referencias antes y despues del borrado, traza completa (orden y numero de llamadas con argumentos de todos los colaboradores doblados, incluidas las lecturas de aleatorios), huella de los globales tocados, palabra de control x87 y pila x87 vacia. El x87 corre a 24 bits.

## Supuestos a revisar al compilar

- Nombres usados: `CCam::m_nMode` (uint16, offset 0xC), `m_vecFront`, `CAnimBlock::{IsLoaded,FirstAnimIdx,NumAnims}`, `CAnimBlendAssociation::{m_BlendHier,m_Flags,m_BlendAmount,m_BlendDelta}`, `CPad::{NewState.m_bPedWalk,DisablePlayerControls}`, `TheCamera.{m_aCams,m_bWideScreenOn,m_fOrientation,m_nActiveCam,m_bJustJumpedOutOf1stPersonBecauseOfTarget}`; el evento `layout` comprueba todos los desplazamientos y constantes de enums (si falla, el banco se detiene y lista que no coincide).
- Los nombres de hook (`FindHook(clase, funcion)`) son los de `docs/hooks.csv` (p. ej. `GetStatValue--OG`, `GetPedWalkUpDown-ped`, `AddEvent-`, `Add-`); si alguno no coincide, el evento `error` lista todos los que faltan.
- `Addr(&CTaskSimpleFight::ControlFight)`, `Addr(&CPlayerPed::...)`, `Addr(&CPedGroups::...)` requieren miembros publicos sin sobrecargas.
- Los hooks de CPad/CCamera/etc. se parchean aunque esten desactivados (el cuerpo de la DLL empieza por un salto al EXE); `Body()` no entra nunca en el EXE.
- `AllowDirStub` recibe `CVector` por valor como tres floats consecutivos en la pila.
- Los dos tramos aleatorios y las entradas de `ms_aAnimBlocks` 1..3 suponen al menos 8 bloques de animacion y 90 definiciones de grupo (se comprueba en `layout`).
- Las estaticas `PlayerControlZeldaWeapon` / `PlayerControlDucked` son cdecl en la DLL y `ret 4` en el EXE: el hook solo es seguro porque su unico llamador del EXE es `ProcessPed` (0x6888EA / 0x688886), que a su vez esta enganchado (no se ha cambiado la convencion para no romper TaskWave2A).

## Aprobación (09/10/2026)

Corrida `20261009-123357-157735`: passed=true, 5120 pares, 5 funciones con 0 fallos (1024 cada una), evento complete (state_restored), evento layout correcto, saves y configuraciones intactos, `executed_probe_sha256` a9334da8... Registrada con `register_footwave4.py` (evidencia en `runs/`, `validation-summary.json`). Con esto CTaskSimplePlayerOnFoot queda COMPLETA. Límites: ~75 colaboradores doblados con traza y sin crédito; ctor/dtor de objetos de pila y ctors de tareas nuevas reales; las dos estáticas (ZeldaWeapon, Ducked) son `ret 4` en el EXE y cdecl en la DLL (no cambiado). Las notas anteriores de "no compilado ni ejecutado" quedan superadas.
