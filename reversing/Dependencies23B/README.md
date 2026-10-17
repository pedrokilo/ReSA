<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
- Residuo conocido: asin(+-1) y acos(-1) con PE limpio dejan en T-1 una constante de 64 bits que el plantado (53 bits) no puede reproducir; las fixtures no la alcanzan en las 16896 + 330000 pruebas, pero el caso teorico existe.
- pow/asin/acos/_ftol2: `FlcCrtPow` llama al nucleo SSE2 del propio EXE (0x829B99); dependen de los globales del CRT del EXE 0xC9C400 (=1) y 0xC9ABFC (=0), que el banco fija y restaura en cada par.
- Colaboradores sustituidos (sin credito) en ambos lados con el mismo falso: ApplyTurnForce 0x542A50, ApplyMoveForce 0x5429F0, GetSpeed 0x542CE0, HeightAboveCeiling 0x6D2600 (conserva ECX/EDX), ocho lectores de CPad,
  GetColModel 0x535300, IsPlayer 0x5DF8F0, GetPadFromPlayer 0x609560. Se parchea la entrada del EXE y el cuerpo de la DLL (Resolve) ANTES de crear parches, y se restauran en orden inverso.
- Cobertura por ruta medida con la traza de colaboradores (`run_local.py`: MIN y ABSENT por ruta) y, en el arnes sin juego, por instruccion: ala 924/927, rotor 1187/1189 (el resto son alternativas muertas con matriz nula).

## Evidencia sin juego (offline/)
`offline/main.cpp` + `mock.h` + `bld*.bat` enlazan `flc_vehicle.inc` (el C++ del parche) con el anexo X87Math y lo comparan con la imagen real del EXE; `trans.py`, `x87gen*.py`, `annot.py` son las herramientas con las que se
tradujeron los bloques B y C. Resultado al entregar: 330000 casos (30000 por ruta) y 20000 por ruta con el anexo anadido a X87Math, 0 diferencias (FNSAVE, MXCSR, memoria, traza, errno, ESP).

## Ejecucion
```
python -B reversing/Dependencies23B/build_probe.py
python -B reversing/Dependencies23B/run_local.py
python -B reversing/Dependencies23B/run_local.py --analyze <comparisons.jsonl>
```
Antes: aplicar `g23b.patch`, anexar `g23b-x87.h.txt`/`g23b-x87.cpp.txt` al final de `X87Math.h`/`X87Math.cpp` y compilar el proyecto.
