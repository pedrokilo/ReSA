<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 871 grupos / 9586 funciones reconstruidas; 9586 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn85B: cuatro métodos de `CGarage` (lote 85B)

Estado: **APROBADO dentro del juego** (02/11/2026, árbol principal sobre HEAD 73d2d7dcc + los cambios del lote). 65536 pares (16384 por función), cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauración/x87 en ambos lados y salida de CADA lado igual a la del oráculo independiente (`fixture_oracle.py`, integrado en `run_local.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn85b/20261102-132751-076819` (archivada en `reversing/CppTwelveLevel6Final/runs/`). Cuatro funciones de código de juego reconstruidas en C++ puro y enganchadas (hook S):

| Función | Dirección | Hook | Fuente |
|---|---|---|---|
| `CGarage::RestoreCarsForThisImpoundingGarage` (antes el envoltorio `GaRestoreImpounding`) | 0x4485C0 | `CGarage` / `RestoreCarsForThisImpoundingGarage` / S | `Garage.cpp` |
| `CGarage::CountCarsWithCenterPointWithinGarage` (antes `GaCountCarsCenterWithin`) | 0x4495F0 | `CGarage` / `CountCarsWithCenterPointWithinGarage` / S | `Garage.cpp` |
| `CGarage::SlideDoorOpen` (antes `GaSlideDoorOpen`) | 0x44A660 | `CGarage` / `SlideDoorOpen` / S | `Garage.cpp` |
| `CGarage::SlideDoorClosed` (antes `GaSlideDoorClosed`) | 0x44A750 | `CGarage` / `SlideDoorClosed` / S | `Garage.cpp` |

Además se sustituye la envoltura `FightNormaliseExe` (0x59C910, `CVector::Normalise`, que ya tiene hook C++ en `Vector.cpp`) por la llamada directa `direction.Normalise()` en `TaskSimpleFight.cpp`. No necesita banco propio: el caller es `CTaskSimpleFight::FightHitPed`, cubierto por `FightHitPed75` (ver «Bancos repetidos»).

## Revisión contra el EXE
- **RestoreCarsForThisImpoundingGarage** (0x4485C0, 137 B, `ret 4`; ECX sin uso): igual que el hide-out pero con solo los **3** primeros coches guardados. Con modelo ≠ 0: `CStoredCar::RestoreCar` (0x447E40); si devuelve vehículo, bit 5 del byte +0x42D, `CWorld::Add` (0x563220) y, según el dword +0x594 (tipo de vehículo), `CAutomobile::PlaceOnRoadProperly` (0x6AF420) si es 0 o `CBike::PlaceOnRoadProperly` (0x6BEEB0) si es 9; el hueco queda vacío. Devuelve verdadero si los 3 huecos quedan vacíos.
- **CountCarsWithCenterPointWithinGarage** (0x4495F0, 147 B, `ret 4`): recorre el pool de vehículos (`ds:[0xB74494]`, tamaño leído una vez) desde `size` hasta 1 (índice `i-1`, incluido el 0), salta las ranuras libres (bit 7 del byte de ranura) y el vehículo ignorado, y cuenta los cuya posición (matriz +0x30, o placement +4 si no hay matriz) cae dentro del garaje (`IsPointInsideGarage`, 0x448740, con el vector por valor).
- **SlideDoorOpen** (0x44A660, 226 B): la posición de la puerta (+0x38) avanza `ms_fTimeStep * paso` (0.011 por defecto, 0.0011 en los hangares 44 y 45) con los x87 a 24 bits y se guarda como float. Con posición ≥ 1.0 (comparación ordenada) queda en 1.0, se avisa a la primera puerta que devuelve `FindDoorsWithGarage` (0x449FF0) con el evento 0xA0 y devuelve verdadero; si no (también con NaN), evento 0x9F y falso.
- **SlideDoorClosed** (0x44A750, 217 B): la posición retrocede `ms_fTimeStep * paso` (0.013 por defecto, 0.0013 en los hangares); con posición ≤ 0.0 (ordenado) queda en 0, evento 0xA2 y verdadero; si no (también NaN), evento 0xA1 y falso. El aviso es `CAEDoorAudioEntity::AddAudioEvent` (0x4DC860) sobre el audio del garaje (+0x50): posición de la puerta (matriz +0x30 o placement +4), volumen 0 y velocidad 1.

## Banco
Falsos idénticos en la dirección del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco): `RestoreCar` 0x447E40, `CWorld::Add` 0x563220, `PlaceOnRoadProperly` de coche y de moto (0x6AF420, 0x6BEEB0), `IsPointInsideGarage` 0x448740 (la DLL de C++ llama al método miembro, no al cuerpo del hook del EXE: se parchea también), `FindDoorsWithGarage` 0x449FF0 y `AddAudioEvent` 0x4DC860. Registran todos sus argumentos EN ORDEN y devuelven lo que dice el caso. El pool de vehículos se sustituye por uno propio (estructura +0 objetos, +4 mapa, +8 tamaño, +0xC primera libre) de hasta 8 ranuras de 0xA18 bytes con matrices propias; `CTimer::ms_fTimeStep` se fija por caso.
`SlideDoorOpen/Closed` corren de verdad en ambos lados (los ayudantes x87 `Veh83D*` de la DLL frente a los x87 del EXE). 16384 casos por función, 16 familias de 1024 casos; el oráculo (`fixture_oracle.py`) reproduce la aritmética x87 a 24 bits con fracciones exactas, el `fstp dword` (desnormales, desbordamiento a infinito) y los eventos con todos sus argumentos.

## Fuera de los fixtures
Tamaño de pool mayor que 8, NaN con cargas distintas (los fixtures usan un único patrón), bytes altos de los booleanos y valores de tipo de vehículo (+0x594) no listados en `kVTypes`.

## Hallazgos del banco (ninguno es un fallo de producción)
- El falso de `IsPointInsideGarage` (0x448740) tuvo que ser un stub `naked` que conserva EDX: el original lo deja intacto y el bucle del EXE de 0x44961C lo usa de contador a través de la llamada; un `__fastcall` de C++ lo destruía y el EXE cortaba el bucle tras la primera vuelta (10790/16384 diferencias en la primera corrida).
- El oráculo de `RestoreCarsForThisImpoundingGarage` anotaba en el evento de `CWorld::Add` los modelos iniciales y no los vivos (los coches ya restaurados quedan a cero).
- La DLL de C++ llama al método miembro `IsPointInsideGarage(CVector)` y no al cuerpo del hook del EXE (`GarageIsPointInsideRegs`): se parchean ambos con el mismo falso.

## Prueba negativa
Producción mutada en ocho puntos de `Garage.cpp` (cuarto coche también restaurado; tipo de moto 9→8; sin excluir el vehículo ignorado; el bucle del pool salta el índice 0; paso de apertura 0.011→0.012; `>=` por `>` al abrir; evento 0xA2→0xA3; `<=` por `<` al cerrar). Fallan **31331/65536** pares, sin caída: `RestoreCars...` 12249, `CountCars...` 10298, `SlideDoorOpen` 4705, `SlideDoorClosed` 4079. Evidencia: `D:/Original/_e_resultados_movidos/fn85b_neg/20261102-132332-556921` (movida desde el clon de pruebas). Restaurada desde la copia buena y comprobada con `cmp` y con el hash (`Garage.cpp` `2baa8707730c2153`) antes de la corrida final.

## Bancos repetidos
Citan alguna de estas funciones o el código modificado: `GarageUpdate63` (falsea las funciones de garaje como colaboradoras de `CGarage::Update`; sin adaptación): **APROBADO**, 16384 pares, `resultados-garageupdate63/20261102-133218-926760`. `FightHitPed75` (cubre `CTaskSimpleFight::FightHitPed`, que llama a `CVector::Normalise`: ahora directamente y ya no por la envoltura `FightNormaliseExe`; se adaptó parcheando también el cuerpo de la DLL de `CVector::Normalise` con el mismo falso, `env.dllOnly`): **APROBADO**, 21808 pares, `resultados-fighthitped75/20261102-134316-555003`. La evidencia de ambos está refrescada en el registro. Ningún otro banco cita estas direcciones ni estos símbolos.
