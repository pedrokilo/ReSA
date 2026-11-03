<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 02/11/2026 (Ciudad de México):** 820 grupos / 9015 funciones reconstruidas; 9015 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Car84K: CCarCtrl::GenerateEmergencyServicesCar 0x42F9C0 (lote 84K)

Estado: **APROBADO dentro del juego** (02/11/2026, arbol principal, HEAD 7d4f18e34 + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados y salida de CADA lado igual a la del oraculo independiente (`fixture_oracle.py`, integrado en `run_local.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car84k/20261102-033430-952656` (el manifiesto de construccion corresponde al `CarCtrl.cpp` probado, hash `21fa2606f6edbe8f...`; la primera corrida, `...032752-094121`, ya dio los 18432 pares sin diferencias). Plantilla: `reversing/Car84F`.

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `static void CCarCtrl::GenerateEmergencyServicesCar()` (antes `plugin::Call<0x42F9C0>`, sin hook) | 0x42F9C0 | `CCarCtrl` / `GenerateEmergencyServicesCar` / S (`RH_ScopedInstall`) | `CarCtrl.cpp` (sin ayudantes x87: solo enteros y copias crudas) |

Fila de `docs/hooks.csv`: `CCarCtrl,GenerateEmergencyServicesCar,0x0042F9C0,1,0,1,S`.

## Revision contra el EXE (0x42F9C0..0x42FBB2, 0x1F3 bytes, cdecl sin argumentos)
1. Vuelve si el byte de 0x8A5B28 es 0, si `CGangWars::GangWarFightingGoingOn()` (0x443AC0), si el nivel de busqueda (`FindPlayerPed(-1)->+0x480->+0->+0x2C`, con signo) es mayor que 3, si `CGame::currArea` (0xB72914) no es 0 o si el nivel de zona (0xBA6718) es 0.
2. Suma de 32 bits de seis contadores (ambulancias, bomberos, aparcados, de mision, policias, aleatorios) comparada CON SIGNO con `MaxNumberOfCarsInUse`; si la suma es mayor, vuelve.
3. Ambulancia (solo si `NumAmbulancesOnDuty == 0`): con al menos dos accidentes libres (comparacion con signo) toma `FindPlayerCoors(-1)` y el accidente libre mas cercano; si lo hay, `StreamAmbulanceAndMedic(true)` es verdadero y `ahora > ultimo + 30000` (sin signo, la suma desborda a 32 bits), crea la ambulancia en la posicion del herido (matriz +0x14 en +0x30, o la posicion compacta +4; tres palabras crudas) con el modelo de la tabla 0x8A5AB4 del nivel y, si `GenerateOneEmergencyServicesCar` (0x42B7D0) devuelve algo, guarda el tiempo; con menos de dos, `StreamAmbulanceAndMedic(false)`.
4. Bomberos (solo si `NumFireTrucksOnDuty == 0`, releido): con menos de tres fuegos (el retorno de `GetNumOfNonScriptFires` se lee en AX) `StreamFireEngineAndFireman(false)` y vuelve; si no, `FindNearestFire(jugador, true, true)`, `StreamFireEngineAndFireman(true)`, `ahora > ultimo + 0x88B8` y crea el camion en la posicion del fuego (+4) con el modelo de la tabla 0x8A5AD4.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (hook RH apagado): 0x443AC0, 0x56E210, 0x56CF20, 0x56CEE0, 0x56E010, 0x56D050, 0x40A2A0, 0x40A400, 0x42B7D0, 0x538F10 y 0x538F40; registran sus argumentos EN ORDEN y devuelven lo que dice el caso. Corren de verdad las lecturas de las tablas de modelos (0x407D30 / 0x407DC0 en el EXE) y de los contadores, el tiempo y el nivel de zona del juego (regiones restauradas tras cada lado).
Se comparan los eventos, los ultimos tiempos, los contadores y que ninguna arena cambie; cada lado debe coincidir con `fixture_oracle.py`. 18432 casos, 18 familias (16384 + 2x1024).

## Prueba negativa
Produccion mutada en siete puntos de alcance amplio y restaurada (`CarCtrl.cpp` sha256 bueno `21fa2606f6edbe8f...`; comprobado con `cmp` y con el hash antes de la corrida final): nivel de busqueda `> 3` -> `> 2`, `+ 30000` -> `+ 29999` (tiempo de ambulancia), accidentes libres `>= 2` -> `>= 3`, tablas de modelos intercambiadas (`0x8A5AB4` -> `0x8A5AD4`), fuegos `< 3` -> `< 4`, suma de contadores `>` -> `>=` y posicion del herido sin matriz `+4` -> `+8`. Fallan **10824/18432 pares**, sin caida (`resultados-car84k/20261102-033125-356205`).

## Bancos repetidos (citan 0x42F9C0)
- `CarCtrl22` (falsea `GenerateEmergencyServicesCar` en el simbolo de la DLL y en la direccion del EXE): **APROBADO**, 10304 pares, `resultados-carctrl22/20261102-033806-725632`; evidencia refrescada (22 filas). No necesito adaptacion.
Ningun otro banco de `reversing/` cita la direccion ni el simbolo.

## Fuera de los fixtures
Nivel de zona (0xBA6718) fuera de 0..7, jugador sin datos de jugador o sin busqueda, herido con ped invalido, el byte de `bAllowEmergencyServicesToBeCreated` distinto de 0 y 1, bytes altos de los booleanos que devuelven los colaboradores y que el compilador cambie el orden de las llamadas a los colaboradores (el banco comprueba el orden de todos los eventos).
