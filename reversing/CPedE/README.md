<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPed, grupo E: posicion, enganches, voz y percepcion (25 funciones)

Banco por pares dentro del motor (original = EXE con los hooks del grupo desactivados; candidato = DLL), con comparacion bit a bit
de todo el estado tocado y de la traza de las dependencias simuladas. Aun sin ejecutar: lo compila y lo itera quien lo lance.

| Funcion | Direccion | Casos |
|---|---|---:|
| `AttachPedToBike` | 0x5E7E60 | 512 |
| `AttachPedToEntity` | 0x5E7CB0 | 1024 |
| `DettachPedFromEntity` | 0x5E7EC0 | 768 |
| `PositionAnyPedOutOfCollision` | 0x5E13C0 | 256 |
| `PositionAttachedPed` | 0x5DFDF0 | 1024 |
| `UpdatePosition` | 0x5E1B10 | 1024 |
| `Teleport` (virtual, vtable 0x86C358 ranura 14) | 0x5E4110 | 256 |
| `SetPedPositionInCar` | 0x5DF910 | 1024 |
| `GetBonePosition-Original` | 0x5E4280 | 512 |
| `GetTransformedBonePosition` | 0x5E01C0 | 512 |
| `CanSeeEntity` | 0x5E0730 | 1024 |
| `OurPedCanSeeThisEntity` | 0x5E1660 | 512 |
| `DoesLOSBulletHitPed` | 0x5F01A0 | 256 |
| `IsPedHeadAbovePos` | 0x5F02C0 | 256 |
| `ReleaseCoverPoint` | 0x5E0270 | 64 |
| `Say` / `SayScript` | 0x5EFFE0 / 0x5EFFB0 | 256 / 128 |
| `DisablePedSpeech` / `...ForScriptSpeech` | 0x5EFF60 / 0x5EFF80 | 64 / 64 |
| `EnablePedSpeech` / `...ForScriptSpeech` | 0x5EFF70 / 0x5EFF90 | 64 / 64 |
| `GetPedTalking`, `CanPedHoldConversation` | 0x5EFF50, 0x5EFFA0 | 64, 64 |
| `PedIsInvolvedInConversation`, `PedIsReadyForConversation` | 0x43AB90, 0x43ABA0 | 64, 512 |

Total: 10.368 pares.

**Fixtures**
- Peatones falsos (buffer de `sizeof(CPed)` con 0xCD y solo los campos que el EXE lee), vehiculo falso en un buffer de 0xA00 bytes
  (conductor/pasajeros, handling, tipo y subtipo, nodos de carroceria, matriz de inclinacion y salto BMX), objeto falso, inteligencia,
  datos de jugador y `CWanted` falsos, entidad de voz falsa con vtable propia en +0x294. Nunca el jugador ni peatones reales.
- Se intercambian temporalmente las entradas 532 (Combine) y 19990 de `CModelInfo::ms_modelInfoPtrs` por informacion de vehiculo falsa
  (tipo en +0x3C, estructura de dummies en +0x5C). Se restauran antes de cada ejecucion y al final.
- Las matrices de las entidades siempre existen (GetMatrix/SetMatrix reservarian matrices del pool real); la posicion compacta (sin
  matriz) se usa solo donde las funciones no reservan matriz (Teleport, PositionAnyPedOutOfCollision, UpdatePosition, CanSeeEntity,
  OurPedCanSeeThisEntity, IsPedHeadAbovePos).
- Se comprueba en tiempo de ejecucion que la disposicion de campos de la DLL coincide con los desplazamientos del EXE (si no, evento `error`).

**Relleno de matriz que el EXE deja con basura de pila**: las palabras +0x0C, +0x1C, +0x2C y +0x3C (pads/flags del RwMatrix) de las matrices de los peatones, el vehiculo y el objeto se normalizan a 0 antes de comparar (hash y volcado), en original y candidato; el resto de la matriz se compara bit a bit.

**Dependencias simuladas y trazadas (EXE y DLL)**: CWorld::Add/Remove/TestSphereAgainstWorld/ProcessLineOfSight, CPedIntelligence::
FlushImmediately/GetTaskSwim/GetTaskJetPack/GetTaskFighting/FindTaskByType, CTaskManager::FindActiveTaskByType, CEventGroup::GetEventOfType,
CPhysical::GetSpeed/ApplyMoveForce, CEntity::GetColModel/UpdateRpHAnim, GetAnimHierarchyFromSkinClump, RpHAnimIDGetIndex,
RpHAnimHierarchyGetMatrixArray, RwV3dTransformPoints, RwFrameGetLTM, CPed::GiveWeapon/SetCurrentWeapon(ranura), CPlayerPed::
MakeChangesForNewWeapon/ClearWeaponTarget, CCamera::SetNewPlayerWeaponMode, CAnimManager::BlendAnimation, CBike::CalculateLeanMatrix,
SurfaceInfos_c::IsSteepSlope, CCoverPoint::ReleaseCoverPointForPed, FindPlayerPed y los metodos de CAEPedSpeechAudioEntity (incluida la
llamada virtual AddScriptSayEvent: el EXE por la vtable falsa, la DLL por el cuerpo real leido de la vtable con el hook activo).
`PositionAttachedPed`, `SetMatrix`, `LimitRadianAngle`, las funciones de CMatrix y `CWeaponInfo::GetWeaponInfo` corren de verdad.

**Correcciones de la DLL (comparadas con el desensamblado)**
- `PositionAnyPedOutOfCollision`: reescrita (la DLL buscaba en una rejilla de pasos de -3,5 acumulados, invertia la prueba de colision y
  nunca encontraba punto). Rejilla 15x15 de paso 0,5 alrededor del peaton, punto mas cercano (distancia^2 truncada a entero, umbral 999),
  z + altura maxima de la ULTIMA entidad probada con vehiculos.
- `SetPedPositionInCar`: el grupo de animacion 13 se comparaba mal (`GetAnimGroupId()` devuelve 88 + grupo: es 0x65); el camion Combine
  hacia `SetTranslateOnly(0, 0, z)` en vez de sumar solo `chassisPosZ` a la z; constantes exactas del EXE (pi/2, 0,6), Multiply3x3 en el
  orden de sumas del EXE y `fpatan - pi/2` sin redondear el arcotangente; sin `assert(IsInVehicle())`.
- `PositionAttachedPed`: `fpatan` real (no `std::atan2`), Multiply3x3 en el orden del EXE y constantes pi, pi/2 y 2pi exactas.
- `DettachPedFromEntity`: la fuerza al morir lleva z + 4,0 (0x858B90); la rama normal pone `m_bUsesCollision` (+0x1C bit 0), no `bIsStanding`.
- `AttachPedToEntity`: faltaba `CPlayerPed::MakeChangesForNewWeapon` (0x60B460) tras guardar el arma elegida del jugador.
- `CanSeeEntity`: usaba `GetRadianAngleBetweenPoints` y 2pi=6,28318; el EXE usa `GetAngleBetweenPoints * 0,01745329238474369`, 2pi exacto,
  normaliza tambien `m_fCurrentRotation` y compara con `|angulo - rotacion|`.
- `OurPedCanSeeThisEntity`: el rayo parte de la posicion del peaton con z + 1; el limite usa `sqrt(...) < 40` (no el cuadrado) y no se
  inicializan el `CColPoint` ni la entidad.
- `UpdatePosition`: en la rama del barco la posicion relativa es (x, y, z - 1) - suelo (la DLL restaba 1 despues); la normalizacion de la
  pendiente sigue la rutina del EXE.
- `GetBonePosition-Original` / `GetTransformedBonePosition` / `DoesLOSBulletHitPed` / `IsPedHeadAbovePos`: jerarquia de
  `GetAnimHierarchyFromSkinClump` (no `GetAnimHierarchyFromClump`), matriz = array + indice sin comprobaciones, `RwV3dTransformPoints` en el
  sitio, y la matriz por la posicion estandar de hueso en el orden de sumas de 0x59C890; `DoesLOSBulletHitPed` devuelve 0 con NaN.

Repetir: `python reversing/CPedE/build_probe.py` y `python reversing/run.py --case cpede`.
