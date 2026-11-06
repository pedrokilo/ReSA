<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 06/11/2026 (Ciudad de México):** 845 grupos / 9225 funciones reconstruidas; 9225 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Car84L — CCarCtrl::GenerateOneRandomCar (0x430050)

Banco par a par del lote 84L: `CCarCtrl::GenerateOneRandomCar` (EXE `0x430050..0x431F3A`, 0x1EEA bytes, hook S: `RH_ScopedInstall`). cdecl sin argumentos, `ret` simple, void. 18432 pares (familias 0..15 de 1024 casos y las familias 16 y 17 de 1024 casos cada una).

## Qué es la función de la DLL

**Es una transcripción mecánica del desensamblado, no una reconstrucción legible.** `reversing/Car84L/tools/lift.py` y `lift_emit.py` leen el desensamblado limpio (`tools/ins.txt`, salida de `dumpbin`) y las tres tablas de salto del EXE y generan `source/game_sa/CarCtrlGenerateOneRandomCar.inl`, incluido por `CarCtrl.cpp`:

- los registros enteros son variables locales; el marco de pila es un array `F` (los punteros a locales que reciben los colaboradores apuntan a `F`); ESP y la profundidad de la pila x87 se siguen estáticamente (el elevador comprueba que cada instrucción tiene un solo valor posible de ESP y de profundidad x87 y que `ret` sale con ambos en cero);
- la pila x87 son variables `x0..x7` y cada operación pasa por `Veh83D*` (mantisa de 24 bits, exponente ampliado); los saltos son `goto` y los saltos indirectos, `switch` sobre las tablas del EXE (la tabla de bytes y las constantes de `.rdata` se incrustan como literales);
- los colaboradores se llaman por su dirección del EXE: los que tienen hook RH ejecutan el C++ de la DLL y los que no, los bytes del EXE. La tabla de llamadas (convención, argumentos de pila, bytes que limpia el llamado y retorno) está en `lift.py` (`CALLS`); un error en ella rompería la coherencia de ESP que el elevador comprueba.

Regenerar: `python reversing/Car84L/tools/lift_emit.py <salida>` (y copiar tras la cabecera de 5 líneas de `source/game_sa/CarCtrlGenerateOneRandomCar.inl`).

**No hay oráculo independiente** (a diferencia de los bancos anteriores): la función de la DLL sale del mismo desensamblado que el EXE y un modelo Python propio habría duplicado ~1800 instrucciones. Su única comprobación es la igualdad exacta con los bytes del EXE sobre las fixtures del banco, más la cobertura por sitio de llamada. `run_local.py` no tiene `fixture_oracle.py`.

## Método del banco

Original = bytes del EXE con el hook apagado; candidato = la DLL con SOLO el hook de esta función encendido. Los colaboradores falseados llevan el mismo falso en los dos lados (la DLL los llama por la dirección del EXE: basta parchear la entrada del EXE, `dll = 0`), con su hook RH apagado durante el banco. Cada falso registra, EN ORDEN, su etiqueta (`kind`, ver `LiveComparison.hpp`), sus argumentos y la huella (FNV) del vehículo en ese instante, y devuelve lo que dice el caso. Tras el evento número `mutAt` el falso muta un campo del vehículo para comprobar que la función relee la memoria donde el EXE la relee.

- **Falsos (40):** jugador (`FindPlayerCentreOfWorld`, `FindPlayerCoors`, `FindPlayerWanted`, `FindPlayerVehicle`, `FindPlayerPed`, `GetWantedLevel`), recuentos y estado (`FewerCars`, `FindCarMultiplierMotorway`, `GangWarFightingGoingOn`, `LaRiotsActiveHere`), elección de modelo (`ChooseModel`, `ChoosePoliceCarModel`, `PickLeastUsedModel`, `RequestModel`, `IsBoatModel`), `GenerateCarCreationCoors2`, `FindObjectsKindaColliding`, `ProcessVerticalLine`, `GetWaterLevel`, `GetZoneInfo`, `GetNewVehicleDependingOnCarModel`, `CWorld::Add`, `SetPosn`, `SetClumpAlpha`, `AddVehicleUpgrade`, `GetIsOnScreen`, `PossiblyStreamCarOutAfterCreation`, `CreatePoliceChase`, `CreateConvoy`, `SetUpDriverAndPassengersForVehicle`, `AddPoliceCarOccupants`, `ChangeLawEnforcerState`, `0x6A2530`, `PickNextNodeRandomly`, `ChooseVehicleColour`, `GetVehicleAppearance`, `FindPolice*` (cuatro) y las dos entradas virtuales del vehículo (destructor y `GetHeightAboveRoad`, en una tabla virtual del banco).
- **Corren de verdad (bytes del EXE en los dos lados, hook apagado):** `srand`/`rand`/`_ftol2`, `GetRandomNumberInRange` (int y float), aritmética de vectores (`0x40FE30`, `0x40FE60`, `0x40FEC0`, `0x4119D0`), `0x4082C0`, `0x59C910`, `0x41CCB0`, `0x420800`, `0x420920/0x420950/0x420980`, posición de nodos y tramos (`0x420A10`, `0x420A60`), `0x421120`, `0x424130`, `0x43C710`, `0x43C900` y `0x44DB00`. Como los dos lados ejecutan los mismos bytes, esta prueba no valida el C++ de la DLL de esos colaboradores (cada uno tiene su banco).
- **Mundo:** `ThePaths` (cuatro tablas por área, tres áreas cargadas con ocho tramos, ocho nodos y cuarenta enlaces de nodo y de navegación), las informaciones de modelo (`0xA9B0C8[modelo]`, cuatro variantes con su modelo de colisión), el primer byte de la información de streaming (`0x8E4CD0 + 20 * modelo`) para los 28 modelos del banco y 31 globales del juego (contadores, reloj, cámara...). Todo se guarda al empezar y se restaura tras cada lado.
- **Observado en cada lado:** escrituras en las arenas del vehículo (cambios byte a byte respecto a su imagen inicial), la matriz, el conductor y las cuatro informaciones de modelo; hash de todas las regiones (globales incluidas); traza de eventos con su hash FNV; ABI (pila, no volátiles, DF, SEH), estado x87 y restauración.
- **Cobertura:** cada falso anota la dirección de retorno del lado original (sitio de llamada del EXE). Hay 63 sitios (llamadas a falsos y llamadas virtuales); `run_local.py` exige 20 casos en cada uno salvo los tres de `coverage.json` (alcanzados por menos casos porque dependen de sorteos encadenados: `0x430870`, `0x431DD1` y `0x431DE7`).

## Prueba negativa

`tools/mutate.py apply` aplica cinco mutaciones al `.inl`: sentido del sorteo de densidad (`0x4304C8`), el `0x50` del sorteo con disturbios (`0x4308DE`), el tramo anterior del piloto automático (`0x4306BA`), el tipo de peatón del contador de la ley (`0x431C46`) y un factor de la caja de colisión (`0x4310F2`). El banco falla en 14288 de 18432 pares, sin caída ni accesos inválidos. El `.inl` bueno se restauró desde una copia y se comprobó con `cmp` antes de la corrida final.

## Fuera de los fixtures

Nodos con cuenta de enlaces 0 (el EXE divide por cero en `0x430AE2`); nodos cuyos enlaces son todos iguales (el EXE no sale del bucle de `0x430AD0`); áreas o tramos sin cargar o con id fuera de las tablas del banco; modelos fuera de los 28 del banco; vehículo sin matriz; NaN con cargas distintas (las fixtures usan un único patrón); lecturas de pila sin inicializar (el marco `F` de la DLL empieza en cero); y bytes altos de los booleanos que devuelven los falsos (el elevador los entrega con ceros).

## Hallazgos

- El primer fallo del banco fue del elevador: las llamadas virtuales (`call [eax+0D4h]`, `call [edx]`) dereferenciaban dos veces la tabla virtual. Corregido en `lift_emit.py`.
- Los falsos registran los punteros a locales etiquetados (`Tag`): sin etiquetar, la dirección de pila del EXE y la de la DLL difieren.
- `GenerateOneRandomCar` no comprueba que el nodo elegido tenga enlaces ni que sus enlaces difieran: el banco los garantiza.

## Herramientas (`tools/`)

`lift.py` / `lift_emit.py` (elevador), `mutate.py` (prueba negativa), `firstdiff.py` (primer caso con diferencias de un registro), `sites.py` (cobertura por sitio), `dump_cases.cpp` + `dump.bat` (vuelca las fixtures como JSON), `chk.bat` y `prelude_test.cpp` (compilación de comprobación de la función elevada fuera del juego), `ins.txt` (desensamblado limpio).

## Actualizacion del lote 104A (05/11/2026)
`CAutomobile::SetRandomDamage` (0x6A2530), que este banco sustituye por un falso, tiene hook RH desde el lote 104A (`reversing/Fn104A`). `MakeCollab` detecta el hook: ruta apagada y el mismo falso en la entrada del EXE y en el cuerpo del hook de la DLL. Repetido sin cambios en el banco y aprobado con el codigo final (`runs/Car84L-20261105-230729-397024`).
