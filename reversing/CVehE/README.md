<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CVehicle, grupo E: armas de avion, barco, cabrestante, cuchillas y colision (26 funciones)

Banco por pares dentro del motor (original = EXE con los hooks del grupo desactivados; candidato = DLL), con comparacion bit a bit
del estado tocado y de la traza de las dependencias simuladas. Aun sin ejecutar: lo compila y lo itera quien lo lance.

| Funcion | Direccion | Casos |
|---|---|---:|
| `ApplyBoatWaterResistance` | 0x6D2740 | 512 |
| `ProcessBoatControl` | 0x6DBCE0 | 1024 |
| `BladeColSectorList<CPtrListSingleLink<CEntity*>>` | 0x6DAF00 | 512 |
| `DoBladeCollision` | 0x6E2E50 | 512 |
| `DoFixedMachineGuns` | 0x73F400 | 256 |
| `FirePlaneGuns` | 0x6D4D30 | 512 |
| `FireUnguidedMissile` | 0x6D5110 | 512 |
| `GetFiringRateMultiplier` / `SetFiringRateMultiplier` | 0x6D4090 / 0x6D4010 | 128 / 256 |
| `GetPlaneGunsAutoAimAngle` | 0x6D3E00 | 128 |
| `GetPlaneGunsPosition` | 0x6D4290 | 256 |
| `GetPlaneGunsRateOfFire` | 0x6D40E0 | 256 |
| `GetPlaneNumGuns` | 0x6D3F30 | 128 |
| `GetPlaneOrdnancePosition` | 0x6D46E0 | 256 |
| `GetPlaneOrdnanceRateOfFire` | 0x6D4590 | 256 |
| `SelectPlaneWeapon` | 0x6D4900 | 256 |
| `InitWinch` / `UpdateWinch` / `RemoveWinch` | 0x6D3B60 / 0x6D3B80 / 0x6D3C70 | 128 / 256 / 128 |
| `PickUpEntityWithWinch` / `QueryPickedUpEntityWithWinch` / `ReleasePickedUpEntityWithWinch` | 0x6D3CD0 / 0x6D3CF0 / 0x6D3CB0 | 64 / 64 / 64 |
| `GetRopeHeightForHeli` / `SetRopeHeightForHeli` | 0x6D3D10 / 0x6D3D30 | 64 / 64 |
| `SpecialEntityCalcCollisionSteps` (vtable 0x871E80, ranura 16) | 0x6D0E90 | 512 |
| `SpecialEntityPreCollisionStuff` (vtable 0x871E80, ranura 15) | 0x6D6640 | 1024 |

Total: 8.128 pares.

**Fixtures**
- Vehiculo FALSO (bufer de 0xA40 bytes con 0xCD y solo los campos que el EXE lee: matriz propia, handling, tipo/subtipo, banderas, conductor,
  remolque...), peatones falsos (conductor, entidades de la lista), objeto, matrices, modelos de colision (caja en +0 / +0xC, esfera en +0x18)
  e informacion de modelo falsa (se sustituyen las entradas usadas de `ms_modelInfoPtrs`; el dummy del arma sale de +0x5C -> +0x9C).
  Nunca vehiculos, peatones ni pools reales. `m_pReferences = nullptr`; las referencias que crean los eventos se limpian tras cada ejecucion.
- Cuerdas: se guarda y restaura todo el array `CRopes::aRopes`; se rellena con bytes aleatorios y la cuerda del vehiculo (id = this + 0x1D, o
  this + 0xA320 en `RemoveWinch`) existe o no. `FindRope` corre de verdad. Las funciones que el EXE llama sin comprobar el resultado de
  `FindRope` solo se prueban con la cuerda presente.
- Lista de aspas: nodos reales de `CPtrNodeSingleLinkPool` (la DLL en Debug exige nodos validos del pool); el pool (cabecera y estados de
  ranura) se guarda y restaura al empezar cada ejecucion.
- Se comprueba en tiempo de ejecucion que la disposicion de campos de la DLL coincide con los desplazamientos del EXE (incluidos los bits de
  `vehicleFlags`, `physicalFlags`, banderas del peaton y del objeto); si no, evento `error`.

**Dependencias simuladas y trazadas (EXE y DLL)**: ApplyMoveForce/ApplyTurnForce/ApplySoftCollision/SetDamagedPieceRecord, GetColModel,
AnimatePedColModelSkinned, CCollision::ProcessColModels (rellena `m_aTempColPts`), CPed::GetLocalDirection, Fx_c::AddSparks/AddBlood,
CEventDamage::AffectsPed, ComputeDamageResponse, CEventGroup::Add (se anotan los campos del evento), CLocalisation::Blood, GetIsOnScreen,
CCamera::GetGameCamPosition, CAudioEngine::ReportCollision/ReportWeaponEvent, CAEVehicleAudioEntity::AddAudioEvent, rand (0x821B1E y
CGeneral::GetRandomNumber), CWorld::ClearScanCodes, cBuoyancy::ProcessBuoyancyBoat, CWaterLevel::GetWaterLevel, CBoat::AddWakePoint,
CPlayerPed::GetPadFromPlayer/HandlePlayerBreath, CPad::GetHandBrake/GetCarGunFired/StartShake, CWeapon::FireInstantHit/FireProjectile,
CVehicle::DoPlaneGunFireFX/FireFixedMachineGuns/ApplyBoatWaterResistance (solo dentro de ProcessBoatControl), CEntity::DoesNotCollideWithFlyers,
CRopes::RegisterRope y CRope::ReleasePickedUpObject/PickUpObject/Remove. En `DoBladeCollision` se simulan el EXE (0x6DAF00) y las cuatro
instanciaciones de la DLL (`CPtrListSingleLink<CBuilding*>` y `CPtrListDoubleLink<CVehicle*|CPed*|CObject*>`) anotando lista (sector o sector
de repeticion), modelo de colision (hash de la zona estatica 0xC1CD38), matriz, tipo de rotor y dano.
Corren de verdad: el constructor y destructor de `CEventDamage`, el constructor de `CWeapon`, `CMatrix` (Invert, operator=), `fsin`/`fcos`/`pow`
del x87/CRT y `FindRope`.

Se guardan y restauran: paso y reloj del juego, codigo de escaneo, `planeRotorDmgTimeMS`, puntos de colision temporales, la camara activa
(`m_nDirectionWasLooking`), el valor de inmersion de `cBuoyancy`, las entradas de model info usadas, el array de cuerdas, el pool de nodos y la
zona estatica de la malla de aspas.

**Correcciones de la DLL (comparadas con el desensamblado)**
- `ApplyBoatWaterResistance`: `* 0.001` (no `/ 1000`) y el orden `((prof * fuerza) * prof) * masa`; producto escalar con la suma z, y, x;
  `pow` del CRT del EXE (`0x822130`); sin `sq()`; el eje z con `(1 - z) * 0.5 + z` solo si la velocidad z es <= 0 o NaN.
- `ProcessBoatControl`: `bIsDrowning` solo se limpia si no se cumple la condicion de ahogo; `fsin`/`fcos` reales; comparaciones con NaN como
  el EXE; constantes exactas (0x871FBC, 0x871FB8, 0.008) y productos en lugar de divisiones; `* paso` que faltaba en la fuerza de giro del
  Skimmer; cadena de productos de la fuerza de empuje componente a componente; fuerzas de traccion y de giro con el orden del EXE;
  `fWaterLevel` conserva `fMoveSpeed` si `GetWaterLevel` no escribe; `fMoveSpeed > 0.05` con el double; ola con `_ftol2` + `int16` y
  `* 0.001`; comparaciones de segundo giro y de limite; llamadas a `GetColModel` que el EXE hace aunque no use el resultado.
- `BladeColSectorList`: el calculador de respuesta se construye antes que el evento; la direccion al peaton es 2D normalizada con
  `CVector2D::Normalise`; se quita `bIsStanding` antes de empujar al peaton; la sangre sale de la posicion del PEATON con `* 0.01`, `* 0.35`,
  `+ 0.6`; el evento se pasa por puntero (no una copia); la fuerza de giro del Hunter es el producto vectorial (sin velocidad ni normalizar)
  por `turnMass * -0.0005`; el dano de la pieza se multiplica por `damageMult`; `100 * masa * (1/3000)`; el sonido solo se prueba para entidades
  que NO son peatones y con rango inclusivo; `GetRandomIntInRange` (0x407180); productos escalares y producto vectorial con el orden del EXE;
  `cpOnRotor` conserva la copia del ultimo punto aunque no sea valido; todos los tipos de rotor.
- `DoBladeCollision`: la posicion se transforma con la MATRIZ RECIBIDA (no la del vehiculo); indice de sector `floor(x * 0.02 + 60)` con el
  rango recortado a 0..120 (como el EXE, sin `IterateSectorsOverlappedByRect`); solo los tipos de rotor -3..3 (salvo 0) fijan grosor; se
  anula tambien `m_pSpheres`.
- `FirePlaneGuns`: frecuencias de temblor del Rustler (224) y del Hydra/Tornado (128), no 225/123; primero `FireInstantHit` y despues el efecto;
  la segunda ametralladora recalcula la posicion con la x negada (el DLL no la recalculaba); mando solo para tipos de peaton 0 y 1.
- `FireUnguidedMissile`: el segundo misil sale del lado opuesto (x negada); `max(0, dot)` y `(adelante * v) * paso`; dot con la suma z, y, x.
- `GetPlaneGunsPosition`: el desplazamiento del Rustler se suma tambien a la posicion del dummy del modelo; el Tornado lee la global.
- `GetPlaneGunsRateOfFire` / `GetPlaneOrdnanceRateOfFire`: conversion con `_ftol2` (palabra baja de un int64).
- `DoFixedMachineGuns`: pide siempre el mando 0 y despues el 1 (jugador 2).
- `UpdateWinch`: `segCount` entero con signo (`_ftol2`) y matriz * punto con el orden del EXE; `RemoveWinch`: busca la cuerda con
  `this + 0xA320` como el EXE (la funcion no se usa). `CRopes::GetRope` sin comprobar el rango (`aRopes.data()[i]`).
- `SpecialEntityPreCollisionStuff`: el EXE prueba el bit 24 de las banderas fisicas (`bDontCollideWithFlyers`), no `bSubmergedInWater`; la
  farola caida termina la funcion; `Invert` y la transformacion con el orden del EXE.
- `SpecialEntityCalcCollisionSteps`: `(|v|^2 * paso) * paso`, limites 0.16 / 0.32 exactos del EXE (0x871E34 / 0x871E30), `|dot| * paso / dimension`,
  maximo con `>` estricto y el techo con el CRT sobre el double.
- Sin cambios (MATCH): `InitWinch`, `Pick/Query/ReleasePickedUp...`, `Get/SetRopeHeightForHeli`, `GetPlaneGunsAutoAimAngle`, `GetPlaneNumGuns`,
  `Get/SetFiringRateMultiplier`, `GetPlaneOrdnancePosition`, `SelectPlaneWeapon`.

Repetir: `python reversing/CVehE/build_probe.py` y `python reversing/run.py --case cvehe`.
