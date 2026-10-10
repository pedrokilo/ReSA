<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# VehWave2: banco por pares de CTrain, CPlane y CBike (funciones pendientes)

Banco simple (flujo viejo, copiado de `WorldFinal1`/`PadMouse1`) para 51 funciones. Compilado y ejecutado: aprobado el 09/10/2026 (ver "Aprobacion" al final). Uso: `bash reversing/_handoff/bench.sh VehWave2 3`. Filtro opcional: `vehwave2-only.txt` en la carpeta del juego con `Clase::Funcion`. Corpus: 12864 pares.

| Clase | Funcion | EXE / hueco VMT | Pares |
|---|---|---|---|
| CTrain | Shutdown | 0x6F58D0 | 128 |
| CTrain | DisableRandomTrains | 0x6F5DB0 | 64 |
| CTrain | FindNumCarriagesPulled | 0x6F5CD0 | 128 |
| CTrain | FindSideStationIsOn | 0x6F5EF0 | 64 |
| CTrain | IsInTunnel | 0x6F6320 | 256 |
| CTrain | OpenTrainDoor / AddPassenger / RemovePassenger | 0x6F5D80 / 0x6F5D90 / 0x6F5DA0 | 64 c/u |
| CTrain | ProcessTrainAnnouncements / PlayAnnouncement / UpdateTrains | 0x6F5910 / 0x6F5920 / 0x6F5900 | 64 c/u |
| CTrain | ReleaseOneMissionTrain / RemoveOneMissionTrain | 0x6F5DF0 / 0x6F5DC0 | 256 c/u |
| CTrain | SetTrainSpeed / SetTrainCruiseSpeed | 0x6F5E20 / 0x6F5E50 | 512 c/u |
| CTrain | SetNewTrainPosition | 0x6F7140 | 256 |
| CTrain | RemoveMissionTrains / ReleaseMissionTrains | 0x6F6A20 / 0x6F6B60 | 256 c/u |
| CTrain | MarkSurroundingEntitiesForCollisionWithTrain | 0x6F6640 | 1024 |
| CPlane | Constructor | 0x6C8E20 | 512 |
| CPlane | InitPlaneGenerationAndRemoval | 0x6CAD90 | 64 |
| CPlane | IsAlreadyFlying / SetGearUp / SetGearDown | 0x6CAB90 / 0x6CAC20 / 0x6CAC70 | 128 / 256 / 256 |
| CPlane | CountPlanesAndHelis | 0x6CCA50 | 256 |
| CPlane | AreWeInNoPlaneZone / AreWeInNoBigPlaneZone | 0x6CCAA0 / 0x6CCBB0 | 1024 / 512 |
| CPlane | Render (18), OpenDoor (27), SetUpWheelColModel (43), BurstTyre (44), Fix (50), SetupDamageAfterLoad (51) | vtable 0x871948 | 128, 256, 64, 64, 256, 64 |
| CBike | dmgDrawCarCollidingParticles / KnockOffRider / ReduceHornCounter / GetAllWheelsOffGround / DebugCode / PlayHornIfNecessary | 0x6B5A00 / 0x6B5F40 / 0x6B5F90 / 0x6B6790 / 0x6B67A0 / 0x6B7130 | 64 / 64 / 128 / 128 / 64 / 128 |
| CBike | CalculateLeanMatrix | 0x6B7150 | 1024 |
| CBike | ProcessBuoyancy (sin conductor ni pasajeros) | 0x6B5FB0 | 1024 |
| CBike | SetModelIndex (5), Teleport (14), Render (18), ProcessControlCollisionCheck (23), GetComponentWorldPosition (25), ProcessOpenDoor (28), RemoveRefsToVehicle (40), ProcessDrivingAnims (46), Fix (50), SetupDamageAfterLoad (51) | vtable 0x871360 | 128, 256, 256, 512, 128, 64, 256, 256, 128, 64 |

Huecos verificados volcando las vtables del EXE (`rd.py` en 0x871948, 0x871360 y 0x872370): CPlane 18/27/43/44/50/51 = 0x6CAB70/0x6CACB0/0x6C9140/0x6C9150/0x6CABB0/0x6CAC10; CBike 5/14/18/23/25/28/40/46/50/51 = 0x6B8970/0x6BCFC0/0x6BDE20/0x6B6620/0x6B5990/0x6B58D0/0x6B67B0/0x6BF400/0x6B7050/0x6B7070; `IsComponentPresent` = hueco 26 (0x6B59E0), `PlayCarHorn` = hueco 54 (0x6B7080), CTrain::ProcessControl = hueco 10. Los tres `RH_ScopedVirtualClass` (CPlane 71, CBike 67, CTrain 66) son correctos.

## Que se compara (original = hook desactivado, candidato = hook activo)

Vehiculos falsos de 0xA18 bytes (ranuras `vehs[8]`, vtable falsa, guardas de 32 bytes); se compara el hash completo de cada vehiculo, la guarda, el retorno, el control x87 y la traza de llamadas salientes con argumentos. Las virtuales se llaman por el hueco de la vtable del EXE (con el hook activo apunta a la DLL); las llamadas virtuales internas de la DLL caen en la vtable falsa, cuyo hueco 0 es el destructor (traza), 26 = `IsComponentPresent`, 51 = efecto real de `CPlane::SetupDamageAfterLoad` y 54 = `PlayCarHorn` (traza). El pool de vehiculos real no se toca: el puntero 0xB74494 se sustituye por un pool falso de 8 ranuras (layout CPool: almacen +0, estados +4, capacidad +8) y se restaura.

Colaboradores sustituidos y trazados (parche en la direccion del EXE y en el cuerpo de la DLL, solo durante la ejecucion): `CWorld::Add/Remove` (0x563220/0x563280), `FindPlayerVehicle` (0x56E0D0), `CWorld::ProcessVerticalLine` (0x5674E0), `CTrain::FindPositionOnTrackFromCoors` (0x6F6CC0), `operator delete`/`delete[]` (0x8214BD/0x8213AE, solo en Shutdown), `CWorld::ClearScanCodes` (0x563470), `TrainHitStuff` (0x6F5CF0 y las tres instancias `CPtrListDoubleLink` de la DLL), `CVehicle::Render` (0x6D0E60), `CAutomobile::OpenDoor` (0x6A6AE0), `CVehicle::SetModelIndex` (0x6D6A40), `CClumpModelInfo::FillFrameArray` (0x4C5440), `CBike::ProcessRiderAnims` (0x6B7280), `GetFirstObject` (0x7348C0), `CAutomobile::CAutomobile` (0x6B0A90), `CVehicleModelInfo::GetWheelPosn` (0x4C7D20), `CEntity::GetColModel` (0x535300), `CPlaceable::SetOrientation` (0x439A80), `RwFrameGetLTM` (0x7F0990), `CPhysical::ApplyMoveSpeed/ApplyTurnSpeed/SkipPhysics/CheckCollision/ApplyMoveForce/ApplyTurnForce` (0x542DD0/0x542E20/0x5433B0/0x54D920/0x5429F0/0x542A50), `cBuoyancy::ProcessBuoyancy` (0x6C3EF0), `CBike::CalculateLeanMatrix` y `DoHeadLightBeam` (0x6B7150/0x6E0E20, solo en Render) y los punteros RW de `RwRenderStateSet/Get` (RwGlobals+0x20/+0x24). Los stubs solo trazan en el hilo del banco (`owner`/`Mine()`); `FindPlayerVehicle` devuelve nulo a los hilos ajenos.

Estado global guardado y restaurado (`state_restored`): 0xB74494, 0xC38010, 0xC38024..0xC38034, 0xB7CD78, 0xC1CADC/0xC1CAE0, 0x8D33BC, 0xB7CB84, 0xB7CB5C, TheCamera (0xB6F028, 0x20 bytes), las entradas de model info de los modelos usados y los punteros de RwGlobals. Se comprueba ademas la disposicion de campos de la DLL frente a los desplazamientos del EXE (evento `layout`).

## Correcciones de fuente hechas (EXE frente a DLL)

- `CTrain::Shutdown` (0x6F58D0): el EXE llama `delete[]` (0x8213AE) solo si el puntero no es nulo y pone a cero cada entrada de 0xC38024; la fuente usaba `delete` escalar sobre una copia (no anulaba la tabla).
- `CTrain::SetTrainSpeed` (0x6F5E20): `fmul [0x858B38]` (0.02f), no `speed / 50.0f`.
- `CTrain::SetTrainCruiseSpeed` (0x6F5E50): `_ftol2` y byte bajo (`mov [ecx+3D0h],al`); la fuente hacia `(uint8)speed` que dispara el assert de `SetCruiseSpeed` con valores >255.
- `CTrain::IsInTunnel` (0x6F6320): `CColPoint`/`CEntity*` locales sin inicializar (la fuente los ponia a cero).
- `CTrain::RemoveMissionTrains` (0x6F6A20) y `ReleaseMissionTrains` (0x6F6B60): el EXE recorre el pool de la ultima ranura a la primera (`dec edi`, `sub ebp,0A18h`); la fuente iteraba de la primera a la ultima.
- `MarkSurroundingEntitiesForCollisionWithTrain` (0x6F6640): sector = `floor(v * 0.02f + 60.0f)` (`fmul [858B38]`, `fadd [858B34]`), no `v / 50 + 60`.
- `CPlane::CPlane` (0x6C9053/0x6C9010/0x6C8F5F/0x6C8FAC): angulos de puerta con las constantes del EXE `0x3FF1463B` (HYDRA/RUSTLER/CROPDUST/STUNT), `0xC016CBE4` (SHAMAL) y `0xBFA0D97C` (NEVADA); la fuente los calculaba con `PI=3.14159f` (`3*PI/5`, `-(3*PI/4)`, `-TWO_PI/5`) y difieren en el ultimo bit.
- `CPlane::AreWeInNoPlaneZone` (0x6CCAA0): la distancia se suma `(dz*dz + dy*dy) + dx*dx` (`fmul` en 0x6CCAEA..0x6CCAF6), no `x*x + y*y + z*z`.
- `CBike::CalculateLeanMatrix` (0x6B7150): componente = `(minZ * eje)` redondeada a float y luego `* (1 - fcos(angulo))` (`fcos` x87 y `fsubr` en 0x6B7215..0x6B7229), sumada a la posicion en orden x, y, z; la fuente hacia `(eje * (1-cos)) * minZ`.
- `CBike::ProcessBuoyancy` (0x6B5FB0): fuerza Z = `force.z / ((t * masa) * 0.008)` (0x6B6082..0x6B6088), no `masa/125`; `((masaUsada / constante) * fuerza) * 1.05` (0x6B60A9..0x6B60B3); multiplicador `1 - fuerza * 0.05` (0x6B60D9), no `/20`; `pow` del EXE (0x822130) con el resultado en ST0 sin redondear a float (0x6B6113..0x6B6140).
- `CBike::DamageKnockOffRider` (0x6B5A10): productos escalares como `(y*vy + z*vz) + x*vx` (0x6B5B27..0x6B5D15); peso frontal `(7*v)*v + 0.6` (0x6B5B83..0x6B5B8D); comparaciones `!(force > 10)` y `!(force > umbral)` (NaN falla en el EXE, `fcomp` + `test ah,41h`).
- `CBike::ProcessDrivingAnims` (0x6BF400): el EXE sale si `bOffscreen` (flag 0x20000 de +0x1C) y el estado NO es `STATUS_PLAYER` (`test [ecx+36h],0F8h` / `jne`); la fuente tenia la condicion invertida (`== STATUS_PLAYER`).

## Fuera del banco y por que

- `CTrain::ProcessControl` (VMT hueco 10): fisica completa del tren (seguimiento de vias, colisiones, sonido); no se puede doblar con seguridad (regla 17).
- `CBike::Constructor` (0x6BF430): reserva y modifica datos de colision reales (`0x72F420` con `operator new`), usa `fptan`, el almacen de handlings y el modelo real; hace falta un harness de datos de modelo que no cabe en este encargo. Revisado solo parcialmente (no se compara el cuerpo completo).
- `CBike::Destructor` (0x6B57A0): encadena destructores de miembros (`CAEVehicleAudioEntity::Terminate` 0x4FB8C0, `CRideAnimData` 0x6D0B30, `CMatrix` 0x59ACD0, `CVehicle::~CVehicle` 0x6E2B40) con pila SEH; no se ha dado un doble seguro para el destructor de la base.
- `CBike::DamageKnockOffRider` (0x6B5A10): corregido pero NO benchado: necesita peds falsos (`m_pIntelligence` +0x47C, gestor de tareas, grupo de eventos), `CEventKnockOffBike` (ctor 0x4AFCF0) y `CEventGroup::Add` (0x4AB420); el hueco entre la copia de pila del EXE y el temporal de la DLL exige comparar eventos en profundidad.
- `CBike::ProcessBuoyancy` ramas con conductor/pasajeros: crean `CEventDamage`/`CEventKnockOffBike` y llaman a `CPedDamageResponseCalculator`; el banco fija `m_pDriver = null` y `m_nMaxPassengers = 0`.
- `CBike::GetComponentWorldPosition` con nodo ausente: el EXE llama a `printf` (0x821982) y la DLL a `NOTSA_LOG_DEBUG` (spdlog); solo se prueban nodos presentes.
- `CPlane::OpenDoor` con el flag global 0xC1CAFC activo: codigo muerto en el EXE (nadie escribe 0xC1CAFC); la fuente lo deja como `if (false)`.

## Dominio y supuestos

- Rellenos: las comparaciones de `CMatrix` ignoran el relleno entre vectores (`MatHash`); el resto se compara byte a byte. Floats de los fixtures siempre finitos o NaN silenciosos (nunca señalizantes).
- `AreWeIn*Zone` incluyen NaN en la camara y valores en los bordes exactos de las zonas.
- `SetTrainCruiseSpeed` se prueba hasta |1e6| (fuera de rango de int64 el resultado es UB en ambos lados).
- `MarkSurrounding...`: posiciones en [-3300, 3300] (mas los casos limite), radio hasta 6000; el contador de escaneo se prueba en 0, 1.., 0xFFFE y 0xFFFF.
- Supuestos a comprobar al compilar: simbolos `Body(...)` accesibles (`CEntity::GetColModel`, `CPhysical::ApplyMoveForce(CVector)`, `CBike::ProcessRiderAnims`, `GetFirstObject`, `RwFrameGetLTM`, las tres instancias `TrainHitStuff<CPtrListDoubleLink<...>>`), el nombre de hook `CVehicle::Render`/`SetModelIndex` y `CAutomobile::OpenDoor`, y que el stride del pool de vehiculos de la DLL es 0xA18.

## Ruido del banco y correcciones tras la primera ejecucion (09/10/2026)

- `CBike::CalculateLeanMatrix` (516/1024): solo difería el hash completo del vehiculo; `lean`, `flag`, `lean_pos` y `entity_matrix` eran iguales. Causa: el EXE multiplica con `0x59BE30` (temporal de pila que no escribe el relleno +0xC/+0x1C/+0x2C/+0x3C) y lo copia con `rep movs` (0x59BF8F) a la matriz de inclinacion, de modo que el relleno es basura de pila. Ruido del banco: `CaseLean` enmascara ese relleno tras la llamada.
- `CBike::GetAllWheelsOffGround` (51/128): diferencia REAL de la fuente (el EXE lee el byte +0x805, la fuente leia +0x804). Corregida en Bike.cpp.
- `CBike::ProcessBuoyancy` (5/1024): diferencia REAL con NaN (fuerza Z = 0/0 o inf*0 con masa 0 o constante 0): el EXE usa `fcomp`+`test ah,41h`+`jne`, que con NaN conserva el valor; la fuente usaba `0.5 <= x ? x : 0.5` y `0.01 <= t ? t : 0.01`. Corregidas (y `z < -0.1` en vez de `std::max`).

## Aprobacion (09/10/2026)

Corrida aprobada `20261009-033721-461118`: 12864 pares, 0 diferencias, 51 funciones, state_restored, layout ok, saves y configuraciones intactos; sonda 67a86a6e... Evidencia en `runs/20261009-033721-461118/` (jsonl en .gz con hash verificado), resumen en `validation-summary.json`, registro con `register_vehwave2.py` (51 funciones) y documentacion con `refresh_documentation_cut267.py`. Segun hooks.csv queda COMPLETA CPlane; CTrain conserva 1 pendiente (ProcessControl) y CBike 4 (Constructor, Destructor, DamageKnockOffRider y ProcessEntityCollision), FUERA del banco y SIN credito. Limites: ProcessBuoyancy solo sin conductor ni pasajeros, relleno de la matriz de CalculateLeanMatrix enmascarado (basura de pila del EXE), GetComponentWorldPosition solo con nodos presentes, colaboradores trazados sin credito. La corrida fallida anterior (`previous.jsonl`) no se conserva.
