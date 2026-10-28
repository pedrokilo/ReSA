<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies23B (grupo g23b de BRIEF_21): CVehicle::FlyingControl

Evidencia previa sin juego: arnes de `offline/` (bytes reales del EXE mapeados en un proceso hijo contra el C++ de la DLL); no sustituye a la corrida dentro del juego, que NO se ha ejecutado.

| Funcion | Entrada | Convencion | Pares |
| --- | --- | --- | --- |
| CVehicle::FlyingControl | 0x6D85F0 | __thiscall, 5 argumentos de pila (eFlightModel, leftRightSkid, steeringUpDown, steeringLeftRight, accelerationBreakStatus), `ret 0x14`, void | 11 rutas x 1536 = 16896 |

## Rutas (ruta del banco = modelo de vuelo; ruta 10 = modelo 40)
| Ruta | flightModel | Bloque del EXE | Reconstruccion |
| --- | --- | --- | --- |
| 0 | 0 | A 0x6D8715 (dirigible/"UNK") | FlcRouteAirship, a mano |
| 1, 3, 4, 5 | 1, 3, 4, 5 | B 0x6D8B1D (ala: Baron, avion, UNK2, bote) | FlcRouteWing, transliteracion mecanica |
| 2, 6, 8 | 2, 6, 8 | C 0x6D986E (rotor: RC, helicoptero, 8) | FlcRouteRotor, transliteracion mecanica |
| 7, 9, 10 | 7, 9 y 40 (>8) | solo prologo y cola comun 0x6DA900 | FlcTail |

Cada ruta ejecuta ademas el prologo (salida si no hay `m_pFlyingHandlingData` o `timeStep <= 0`, pad del conductor jugador, viento, velocidad con deriva, Multiply3x3 del centro de masas) y la cola comun
(amortiguacion con pow, correcciones de velocidad y giro, normalizaciones).

## Que se compara (en bits)
FNSAVE completo tras la llamada (FCW, FSW con banderas acumuladas y C0..C3, FTW/TOP, FCS/FOP/FDP/FDS y los diez bytes de los ocho registros, VACIOS incluidos), MXCSR (banderas), errno (via `_errno` del EXE 0x82B181),
memoria de la arena (vehiculo, datos de vuelo, matriz, conductor, pad, colmodelo: 0x4000 bytes, hash), traza de colaboradores en bits, ESP y EBX/ESI/EDI/EBP. FIP a cero en ambos lados y comprobacion del modulo.
EAX/ECX/EDX al volver NO se exigen: ningun llamador del EXE los lee (ver `g23b-audit.txt`).

## Limites declarados
- `kCompareDeadX87Registers = true` (no se relaja): la DLL usa la maquina `FlcX87` (aritmetica de software) y `Commit()` deja el hardware en el estado final del EXE, plantando los registros muertos con
  X87PlantStmt1..5 y FlcPlantStmt6..8 (mismo metodo que X87Plant*/Ftol2RegsCore de 21D, 21E, 22A, 22B).
- Residuo de 64 bits, resuelto en T-1: asin(+-1) y acos(-1) con PE limpio dejan en T-1 una constante de 64 bits (+-pi/2 y pi, `fld tbyte` de 0x8E313A, sin el redondeo a 53 bits de 0x828EA7) que el plantado de 53 bits no reproducia. `Commit` la construye ahora exacta, sin ensamblador: la mantisa se parte en dos doubles exactos (mitad alta y baja de `sig >> 1`) mas `+-0,5` y se suman a 64 bits (PC = 64) con productos por potencias de 2 (`FlcPlanExt`, `FlcExtValue`); la funcion devuelve el valor en ST0 y el llamador lo descarta (`fstp st(0)` de un resultado ignorado) o lo almacena como float (`FlcStoreExt`, C1 del redondeo), de modo que no se levanta ninguna bandera ni se tocan C0, C2 ni C3. Prueba: `offline/ext/ext_test.cpp` contra los bytes del EXE (`_CIasin` y `_CIacos`, 40000 casos cada uno, 12417 asin y 6162 acos(-1) con PE limpio: antes 12417 y 6162 diferencias, ahora 0) y contra una referencia de hardware (40000 casos con T-1 de mantisa de 64 bits arbitraria y profundidades 1..8: antes 20153, ahora 0).
- Limite que queda: un valor de mas de 53 bits en T-2 o mas profundo sigue plantandose con 53 bits (el compilador evalua primero el subarbol del valor y no se puede fijar el orden desde C++). Es inalcanzable en FlyingControl: los cinco puntos de llamada de asin/acos (0x6D96CD, 0x6D9BC4, 0x6D9BD9, 0x6D9BEE, 0x6DA47D) tienen la pila x87 vacia salvo el argumento (el resultado queda en T-1), y la cola comun 0x6DA900 empieza siempre con `fld` en T-1, asi que ninguna constante de 64 bits sobrevive al final de la funcion; el arnes lo comprueba (0 registros muertos no representables en 53 bits al llegar a `Commit` en todas las corridas).
- pow/asin/acos/_ftol2: `FlcCrtPow` llama al nucleo SSE2 del propio EXE (0x829B99); dependen de los globales del CRT del EXE 0xC9C400 (=1) y 0xC9ABFC (=0), que el banco fija y restaura en cada par.
- Colaboradores sustituidos (sin credito) en ambos lados con el mismo falso: ApplyTurnForce 0x542A50, ApplyMoveForce 0x5429F0, GetSpeed 0x542CE0, HeightAboveCeiling 0x6D2600 (conserva ECX/EDX), ocho lectores de CPad,
  GetColModel 0x535300, IsPlayer 0x5DF8F0, GetPadFromPlayer 0x609560. Se parchea la entrada del EXE y el cuerpo de la DLL (Resolve) ANTES de crear parches, y se restauran en orden inverso.
- Cobertura por ruta medida con la traza de colaboradores (`run_local.py`: MIN y ABSENT por ruta) y, en el arnes sin juego, por instruccion: ala 924/927, rotor 1187/1189 (el resto son alternativas muertas con matriz nula).

## Evidencia sin juego (offline/)
`offline/main.cpp` + `mock.h` + `bld*.bat` enlazan `flc_vehicle.inc` (el C++ del parche) con el anexo X87Math y lo comparan con la imagen real del EXE; `trans.py`, `x87gen*.py`, `annot.py` son las herramientas con las que se
tradujeron los bloques B y C. Resultado al entregar: 330000 casos (30000 por ruta) y 20000 por ruta con el anexo anadido a X87Math, 0 diferencias (FNSAVE, MXCSR, memoria, traza, errno, ESP).
Los ultimos 96 casos de las rutas 1 a 6 y 8 son de +-1 exacto con PE limpio (`MakeExactCase` en Fixtures.hpp: matriz de permutacion con signos, valores potencia de 2, paso de tiempo 1, viento 0, FSW inicial sin PE; en una cuarta parte, vecinos 0x3F7FFFFF / 0x3F800001): alcanzan asin(+-1) y acos(-1) con PE limpio en los cinco puntos de llamada (por cada 96 casos: ala 65 y 66 en las rutas 1 y 3, 9 en las 4 y 5; rotor 36 en la ruta 2 y 34 en la 6, de ellos acos(-1) 5 y 7; la ruta 8 siempre llega con PE ya levantado en 0x6D995F) y el resto de las 1440 fixtures no cambia. `offline/bld3.bat` compila el `source/game_sa/X87Math.cpp` real contra los bytes del EXE: 11 x 1536 casos y 11 x 30000, 0 diferencias en el FNSAVE completo.

## Ejecucion
```
python -B reversing/Dependencies23B/build_probe.py
python -B reversing/Dependencies23B/run_local.py
python -B reversing/Dependencies23B/run_local.py --analyze <comparisons.jsonl>
```

Sin juego (compila el X87Math.cpp real y mapea los bytes del EXE; no toca el directorio build):
```
reversing/Dependencies23B/offline/bld3.bat  &&  reversing/Dependencies23B/offline/obj3/fch.exe 1536
reversing/Dependencies23B/offline/ext/bld.bat  &&  reversing/Dependencies23B/offline/ext/obj/ext_test.exe 40000
```
Antes: aplicar `g23b.patch`, anexar `g23b-x87.h.txt`/`g23b-x87.cpp.txt` al final de `X87Math.h`/`X87Math.cpp` y compilar el proyecto.
