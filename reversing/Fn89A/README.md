<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 872 grupos / 9609 funciones reconstruidas; 9609 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn89A: `CCamera::IsItTimeForNewCamera` (lote 89A)

Estado: **APROBADO dentro del juego** (03/11/2026, worktree `wf-2` sobre la rama `wip-4-reconstrucciones`, commit c1fd077eb, con una correccion de `Camera.cpp` que halló este banco). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/restauracion en ambos lados, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).
Evidencia final: ver `validation-summary.json` (carpeta archivada en `reversing/CppTwelveLevel6Final/runs/`). Una funcion de codigo de juego reconstruida en C++ puro y enganchada (tipo S).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `CCamera::IsItTimeForNewCamera` (en el EXE `IsItTimeForNewcam`, sin simbolo) | 0x51D770 | `CCamera` / `IsItTimeForNewCamera` / S | `Camera.cpp` |

Mide 0xD75 B (0x51D770..0x51E4E4) mas la tabla de saltos de 30 entradas en 0x51E4E8, thiscall con dos argumentos (`camSequence`, `startTime`), `ret 8`, devuelve AL. Decide si ya toca cambiar la camara cinematografica de la secuencia dada. Llamadores: `CCamera::ProcessObbeCinemaCamera*` (ya reconstruidos; su banco `Dependencies21B` la tenia como falso solo en la direccion del EXE).

## Hallazgo del banco
La primera corrida (`resultados-fn89a/20261103-223956-755034`) dio 3141 pares con diferencias de 16384, **solo** en los casos 1, 2, 3, 5, 8, 15, 16, 17, 18 y 19: la reconstruccion devolvia `true` cuando el transcurrido (float guardado) era **menor o igual** a 15000, y el EXE (`fld [esp+14h]; fcomp [8CCDF0h]; test ah,41h; je 51E4D8`) devuelve `true` cuando es **mayor**. En esos 16384 pares el EXE coincidio con el oraculo (0 diferencias): el oraculo no depende del C++. Corregido en `Camera.cpp` (`pastFifteenSeconds()`), la corrida siguiente fue limpia. Los treinta casos del despacho, las ramas de tiempo (`now > start + N` sin signo, con envolvente), el palo derecho, el aviso/paso (0x8CCDF4 / 0x8CC471) y el residuo `0xB6F059` quedaron comprobados.

## Revision contra el EXE (0x51D770..0x51E4E8)
- `camSequence < 0` (con signo): true. Transcurrido: `fild [0xB7CB84]` (+2^32 con `fadd` si es negativo) y `fisub startTime`, en la x87 a 24 bits; `fst` lo guarda como float y el registro se compara con 20000 (true si es mayor).
- Palo derecho (`CPad::GetPad(0)`, word +4 del estado nuevo): `|palo| <= 32` arma el aviso (0x8CCDF4 = 1); `|palo| > 96` con el aviso armado fija el paso (0x8CC471 = 1 si palo > 0, si no 0xFF), lo desarma y devuelve true. `camSequence > 29` (sin signo): false.
- Casos 0 y 8 (vehiculo/barco/rhino y linea de vision hasta la camara activa), 1/2/3/5/15..19 (linea de vision del jugador al origen fijo +0x86C, vector jugador - origen fijo con 0x40FE60, modulo con 0x4082C0, producto escalar con 0x56E090 en dos ordenes distintos), 4/6/29 (solo tiempo), 7 (`GetIsOnScreen` sin comprobar nulo), 9..14 (false), 20..23 y 26..28 (`Process_DW_*` con 1, resultado negado), 24 y 25 (0xB6F059 = 0, true).

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores con hook RH, apagado durante el banco): `CWorld::GetIsLineOfSightClear` 0x56A490, `FindPlayerVehicle` 0x56E0D0, `FindPlayerCoors` 0x56E010, `FindPlayerSpeed` 0x56E090, `CEntity::GetIsOnScreen` 0x534540 y los siete `CCam::Process_DW_*` (0x51A740, 0x51B120, 0x51B850, 0x51C250, 0x51C760, 0x51CC30, 0x51D100). `RwCameraSetNearClipPlane` 0x7EE1D0 no tiene hook: la DLL lo llama por su direccion y el falso va solo ahi. Registran sus argumentos EN ORDEN (vectores con su contenido, banderas, `this` con etiqueta de arena).
Corren de verdad en ambos lados: el transcurrido de 24 bits, el palo derecho, `CVector::Magnitude`/`operator-` (codigo real del EXE frente a los ayudantes `Veh83C*` de la DLL), los productos escalares y el despacho. Globales reales (se guardan y restauran): reloj 0xB7CB84, aviso 0x8CCDF4, paso 0x8CC471, 0xB6F059 y el eje X del palo derecho del pad 0 (0xB7345C). La camara (con cuatro camaras), el objetivo, su matriz, el vehiculo del jugador y la velocidad son arenas con guardas.
Se observa: el valor devuelto (AL), los globales, los eventos de los falsos con todos sus argumentos y los bytes cambiados de todas las arenas (`+0x138`, `+0x31` y las mutaciones). Mutaciones de una escritura dentro de un falso: origen fijo.x y velocidad.y (en la linea de vision), coordenadas siguientes del jugador (en la 1a `FindPlayerCoors`) y tipo del vehiculo (en la 1a `FindPlayerVehicle`).
16 familias de 1024 casos (por `(n / 32) % 16`) y 32 secuencias (`n % 32`: 0..29, negativa y mayor de 29): transcurridos pequeños, alrededor de 1000/2000/3000/5000 (con envolventes de 32 bits), de 15000 y de 20000, aleatorios, negativos, enormes; palo en los umbrales 32 y 96; geometria con la distancia del jugador alrededor de cada umbral (1, 2, 3, 4.5, 29, 36, 38, 40, 44, 48, 50, 57); barcos y rhino; linea de vision falsa; valores no finitos; mutaciones; ABI con registros sembrados.

## Fuera de los fixtures
NaN con cargas distintas de 0xFFC00000, NaN senalizantes, bytes altos de los booleanos de los falsos, camara activa mayor de 3 o un objetivo de camara nulo (el original lo desreferencia), y el reloj o el palo reales del juego fuera de los valores del banco.

## Bancos repetidos
`Dependencies21B` citaba 0x51D770 (`FakeIsItTime`, falso solo en la direccion del EXE): adaptado (el mismo falso va tambien en el cuerpo de la DLL, `Resolve(&CCamera::IsItTimeForNewCamera)`) y repetido; ver su README.
