<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CVehicle grupo A: consultas, banderas y estado (31 funciones)

Banco de validacion por pares dentro del motor (EXE gta_sa 1.0 US frente a la DLL) sobre un **vehiculo falso** (buffer de 0xA40 bytes,
mayor que `sizeof(CHeli)` = 0xA18, con patron aleatorio; los punteros que el EXE desreferencia apuntan a objetos falsos: matriz,
clump, handling, peatones, model infos y su `CVehicleStructure`). Prefijo de caso `cveha`, espacio de nombres `CVehALive`.
Pendiente de compilar y ejecutar (el agente no compila ni ejecuta el juego).

Repetir: `python reversing/CVehA/build_probe.py` y `python reversing/run.py --case cveha` (hay que registrar el caso `cveha` en `reversing/cases.json`).
Filtro opcional: fichero `cveha-only.txt` junto al EXE con `CVehicle::<nombre>`.

## Funciones y casos

| Funcion | Direccion | Casos |
|---|---|---:|
| `AddPassenger-Auto-Seat` | 0x6D13A0 | 768 |
| `AddPassenger-Fixed-Seat` | 0x6D14D0 | 1024 |
| `CanBeDeleted` | 0x6D1180 | 768 |
| `CanBeDriven` | 0x6D5400 | 512 |
| `CanDoorsBeDamaged` | 0x6D1E60 | 128 |
| `CanPedEnterCar` | 0x6D1E80 | 768 |
| `CanPedJumpOutCar (virtual, ranura 58)` | vtable 0x871E80 | 1024 |
| `CanPedLeanOut` | 0x6D5CF0 | 512 |
| `CanPedOpenLocks` | 0x6D1E20 | 256 |
| `CanPedStepOutCar (virtual, ranura 57)` | vtable 0x871E80 | 768 |
| `CanVehicleBeDamaged` | 0x6D1280 | 1024 |
| `CarHasRoof` | 0x6D25D0 | 256 |
| `ChangeLawEnforcerState` | 0x6D2330 | 256 |
| `ClearGettingInFlags` | 0x6D3040 | 128 |
| `ClearGettingOutFlags` | 0x6D3060 | 128 |
| `SetGettingInFlags` | 0x6D3000 | 128 |
| `SetGettingOutFlags` | 0x6D3020 | 128 |
| `SetWindowOpenFlag` | 0x6D3080 | 256 |
| `ClearWindowOpenFlag` | 0x6D30B0 | 256 |
| `IsDriver-ModelID` | 0x6D1C60 | 256 |
| `IsDriver-Ped` | 0x6D1C40 | 256 |
| `IsPassenger-ModelID` | 0x6D1C00 | 256 |
| `IsPassenger-Ped` | 0x6D1BD0 | 256 |
| `IsLawEnforcementVehicle` | 0x6D2370 | 512 |
| `IsOnItsSide` | 0x6D1DD0 | 256 |
| `IsUpsideDown` | 0x6D1D90 | 256 |
| `IsVehicleNormal` | 0x6D22F0 | 512 |
| `IsVehiclePointerValid` | 0x6E38F0 | 1024 |
| `UsesSiren` | 0x6D8470 | 512 |
| `SetVehicleCreatedBy` | 0x6D5D70 | 256 |
| `MakeDirty` | 0x6D2BF0 | 1024 |

Total: 14464 pares.

## Diferencias corregidas en la DLL (Vehicle.cpp)

- `CanPedEnterCar`: estaba invertida. El EXE devuelve falso con el vehiculo de lado (`|up.z| <= 0.1` y no moto) y en el resto de casos
  (moto, `up.z > 0.1` o `< -0.1`, NaN) exige que ni la velocidad ni el giro superen 0x863244 = 0.04000000283f (siguiente float a 0.04f; "no mayor que").
- `CanPedStepOutCar`: con `bIgnoreSpeedUpright` el EXE devuelve verdadero cuando el giro NO supera 0.0004 (la DLL devolvia lo contrario);
  comparaciones "no mayor que" (los NaN cuentan como dentro del limite).
- `CanPedJumpOutCar`: la condicion de vuelo se evalua como el EXE (avion/helicoptero, o automovil con `up.z < 0.3` y ultima colision
  hace menos de 1000 ms); las comparaciones son las del x87 (NaN); el amortiguado compara `ts*ts*6.4000007e-05f < 0.01f*|v|^2` (no `|v|^2/100 > ...`);
  el multiplicador final no toca un NaN (`0 > m ? 0 : m`).
- `CanVehicleBeDamaged`: `FindPlayerPed`/`FindPlayerVehicle` se piden solo cuando hacen falta (y `FindPlayerPed` hasta tres veces, como el EXE);
  la salud se compara como `!(health < 250)` (un NaN no impide el dano).
- `AddPassenger` (asiento fijo): aplica la fuerza de giro del ocupante siempre (antes de comprobar el asiento), cosa que la DLL no hacia;
  ambos `AddPassenger` recorren hasta `m_nMaxPassengers` (puede pasar de 8) y usan `.data()[]`.
- `ApplyTurnForceToOccupantOnEntry` (NOTSA): las constantes se multiplican en float (`masa*-0.02f`, `*-0.1f`, `masa*-0.2f`) en vez de dividir (`/-50`, `/-10`, `/-5`).
- `CanBeDriven`: la dummy (0 o 4) la decide el tipo del model info (`mi+0x3C == 5`), no el tipo del vehiculo.
- `IsDriver/IsPassenger (ModelID)`: el EXE lee el modelo del peaton con `movsx` (int16); la DLL comparaba sin signo.
- `MakeDirty`: comparaciones "no mayor que" (NaN), `ts*0.01f`/`ts*0.02f` en vez de `ts/50`, y sin `std::clamp` (su precondicion `lo <= hi` fallaba si la suciedad pasaba de 15).
- `IsVehiclePointerValid`: indice por division con signo de `ptr - almacenamiento` entre 0xA18 sin comprobar que el puntero sea del pool
  (`IsObjectValid` lo hacia; igual que `CPed::IsPointerValid`); luego lee el vehiculo.

## Metodo

Original (hooks del lote desactivados) y candidato (hooks activos) se ejecutan con fixtures identicos regenerados por caso desde la misma
semilla. Se comparan: retorno, los 0x5D0 primeros bytes del vehiculo (palabra a palabra), hash del resto, de los peatones, matrices,
handling, clump, frames, punto de colision, model infos, pool falso y los globales tocados, y la traza de las llamadas externas. En
cada caso se comprueba que el hook cambia el salto del EXE (o la ranura de la vtable) en el candidato y no en el original.
Dobles trazados (direccion del EXE + simbolo/cuerpo de la DLL): `CPhysical::ApplyTurnForce`, `CPhysical::CanPhysicalBeDamaged`,
`CEntity::RegisterReference`, `FindPlayerPed`, `FindPlayerVehicle`, `CCarCtrl::UpdateCarCount`, `CClumpModelInfo::GetFrameFromId` y
`RwFrameForAllObjects` (callback normalizado a `SetAtomicFlagCB`/`ClearAtomicFlagCB`). `CanPedStepOutCar` se simula por la ranura 57 de la
vtable falsa del vehiculo (la llama `CanPedJumpOutCar`); las demas ranuras son trampas. Se guardan y restauran el tiempo y el paso de
`CTimer`, `CWeather::Rain`, `NumLawEnforcerCars`, el puntero del pool de vehiculos (solo se cambia durante `IsVehiclePointerValid`) y
tres entradas de `ms_modelInfoPtrs` (19990..19992, model infos falsos). El evento `layout` comprueba los desplazamientos de la DLL
frente al EXE (y los bits de banderas, el clump, el handling, la dummy del model info y el pool falso por comportamiento).

## No cubierto

- Indices de modelo >= 0x8000 en `IsVehicleNormal`/`CanBeDriven` (el EXE usa `movsx`, la DLL indexa sin signo): leen fuera de la tabla.
- `CanPedOpenLocks` con peaton nulo (el EXE llama a `IsPlayer` siempre salvo para los cierres 2, 4, 5 y 7; la DLL solo con el cierre 3): sin efecto observable con peatones validos.
- `SetVehicleCreatedBy` con argumentos > 255 (el EXE compara el dword con el byte; la DLL recibe un `uint8`).
- `ChangeLawEnforcerState` con argumentos distintos de 0/1.
- Preservacion de EDX: `IsDriver(Ped/ModelID)`, `CanDoorsBeDamaged`, `CanPedLeanOut`, `IsLawEnforcementVehicle`, `UsesSiren` y `ChangeLawEnforcerState` no tocan EDX en el EXE; en los 80 bytes siguientes a cada llamada del EXE no hay lecturas de EDX (los sucesores de saltos no se siguieron).
