<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco RadarBlip28: CRadar::DrawEntityBlip (0x587000)

**Estado: APROBADO** (18/10/2026, corrida `20261018-180858-453761`: 12288 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-radarblip28\20261018-180858-453761`; archivado en `reversing/CppTwelveLevel6Final/runs/RadarBlip28-20261018-180858-453761/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CRadar,DrawEntityBlip,0x00587000,1,0,1,S` en `docs/hooks.csv`). Corrida previa `20261018-180039-989711` (sonda 4cf50bdb...): sólo nota, no evidencia.

Plantilla: `reversing/DecisionMakeDecision27` y `reversing/FileLoaderLoad2dEffect27` (Core/Fixtures/LiveComparison/build_probe/run_local/fixture_oracle/register_hook) y contrato de `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` valen sólo para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.). La infraestructura de falsos con traza viene de `reversing/RadarFinal3` (que doblaba `DrawEntityBlip`: aquí es la función bajo prueba).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh RadarBlip28                       # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/RadarBlip28/build_probe.py
python -B reversing/RadarBlip28/run_local.py
python -B reversing/RadarBlip28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no está completo)
python -I reversing/RadarBlip28/fixture_oracle.py                                              # autoprueba del oráculo (sin juego)
reversing/RadarBlip28/offline/build.bat                                                       # ensayo sin juego (compila 3 unidades fuera de build\, no toca el proyecto)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(DrawEntityBlip, 0x587000)` (categoría `CRadar`). 12288 pares. Archivo de arranque en el clon: `radarblip28-runtime.start`; salida `radarblip28-runtime.jsonl`. Tras una corrida aprobada: `python -B reversing/RadarBlip28/register_hook.py <carpeta de resultados>` (plantilla; no toca `docs/` ni `hooks.csv`: ese paso es del coordinador).

## Fuente (hunks)

`source.patch` (aplicable con `git apply` sobre HEAD; verificado con `git apply --check` contra `git show HEAD:<archivo>`):

- `source/game_sa/Radar.cpp`: `CRadar::DrawEntityBlip` (antes `plugin::Call<0x587000>`) reconstruida en C++ (~290 líneas), cinco estáticas `StaticRef` en las direcciones del EXE (`0xBAA370..0xBAA380`: destello de la pista) y `RH_ScopedInstall(DrawEntityBlip, 0x587000)` activado.
- `source/game_sa/Radar.h`: el segundo parámetro se llama `isSprite` (era `arg1`).
- `source/game_sa/X87Math.cpp/.h`: `RadarAirstripRotateCpp` (fcos/fsin sin redondear del destello de la pista; el módulo x87 compartido, compilado con `/O2 /Oi /arch:IA32 /fp:fast`).

Sin `__asm`/`naked`/`plugin::Call*`. La función sale como `void`; los registros `EAX/ECX/EDX` no se reproducen porque nadie los lee (ver «Llamadores»).

## Qué hace la función (desensamblado 0x587000..0x587BE8)

Pila: `blipIndex`=+50h, `isSprite`=+54h (**sólo el byte bajo**), `ret` plano (cdecl, 2 args). `trace = 0xBA86F0 + 40·blipIndex`; tipo = `(byte[+0x26] >> 2) & 15`; tabla de saltos en 0x587BEC por `tipo-1`.

1. **Entidad / posición** (0x58703C..0x58737B): 1 vehículo (pool 0xB74494, `0x4048E0`), 2 peatón (pool 0xB74490, `0x404910`; bit 8 de `+0x46C` → usa `m_pVehicle` `+0x58C`, y si es nulo sale), 3 objeto (pool 0xB7449C, `0x465040`); entidad nula → sale. Posición = `matriz ? matriz+0x30 : +4`. Si `(byte[+0x36] & 7) == 3` y `[+0x78C] != 0` → `0x43EA00(this=[+0x78C], &pos)`. 6 foco de script (`0x4839A0(handle, 2) < 0` sale; pos = `foco+0x14`, radio = `foco+0x20`), 7 pickup (`0x4552A0(handle) < 0` sale; pos = `GetPosn`), 8 pista; el resto (0, 4, 5, >8) sale.
2. **Pista** (tipo 8, `bl = 1`): 4 llamadas a `FindPlayerVehicle(-1, 0)`; `d = sqrt((ay-p4.y)·(ay-p3.y) + (ax-p1.x)·(ax-p2.x))` guardado como float; si `!(d < 500)` → `0xBAA370 = 0` y **`pos` queda sin inicializar** (basura de pila). Si no: estáticas con inicialización perezosa (guarda `0xBAA380`: bit 0 → `0xBAA37C` paso, bit 1 → `0xBAA378` fin; ambas `(int16)ftol(-longitud)`); `pos = pos del blip`; `0x583530` (resultado descartado); giro `x = cos(a)·paso + x`, `y = y − sin(a)·paso` con `a = rumbo·0x8595EC` (fcos/fsin sin redondear); `0x583530` del punto girado; `dd = sqrt(ry²+rx²)`; el paso sólo se salta cuando `dd` es menor que 0.9 (o NaN: `fcom` pone C0 con NaN) y han pasado como mucho 3 ms (resta sin signo con `ahora = [0xB7CB7C]`); si no: `paso = (int16)(paso+100)`, si `(float)paso > longitud` → `(int16)ftol(-longitud)`, `fin == paso` → `0xBAA370 = 0`, `dd < 0.9` → `fin = paso; 0xBAA370 = 1`, `0xBAA374 = ahora`; después `0x5832F0` (descartada).
3. **Común** (0x58713D): `0x584770(color, brillo, amistoso)`; pantalla 1 o 3 y `DbgFlag` → `0x584480(pos, color, radio)` y `radio = radio − 0.1; < 1.0 → 5.0`; entrada/salida del blip → `0x43EA00`; pantalla ≠ 2 y 3 → sale; `0x583530`, `0x5832F0` (distancia real), `0x583480`; corto alcance y distancia > 1.0 y sin mapa → sale.
4. **Zoom del script** (0x587264): `RadarZoomValue ≠ 0` y `FindPlayerVehicle(-1,0)` nulo y tipo ≠ 3/7: `isSprite` sale; sin «remain» y `!(sqrt(ry²+rx²) ≤ 1.0)` sale; rumbo = `entidad ? 0x441DB0 : 0`; mapa y sin `ShowMissionBlips` sale; `0x56E400(&centro, 0)`; `z−2 > cz` → altura 0 (sólo con `RadarShowBlipOnAllLevels`), `z+4 < cz` → altura 1, si no la flecha `0x584850(sprite 2, x, y, rumbo+π | rumbo−(orientación+π), w, h, color)` (cámara activa en modo 1 = rumbo+π). `h = ftol(alto·0x859524·8)`, `w = ftol(ancho·0x859520·8)`; color = `(c>>24, c>>16, c>>8, 255)`.
5. **Pase de iconos** (`isSprite`): sprite 0 sale; `0x583AF0(blip)`; `0x585FF0(sprite, x, y, 255)`. **Pase de formas**: sprite ≠ 0 sale; mapa sin `ShowMissionBlips` sale; foco: `dd ≤ 0.9` y sin mapa → dos `0x727D60` (radio `((ancho·0x859520)·94/rango·radioFoco)·0.6` y ese radio menos `ancho·0x859520`; colores (0,0,0,150) y (220,220,220,200); 15 segmentos) y sale; pista: sin mapa, `dd < 0.9` y `d < 500` → `0x584850` (sprite 56, ángulo 0, ×8); `pos = pos del blip`, otra vez `0x583530/0x5832F0/0x583480`; con mapa `0x584850` (sprite 57, `rumbo` −90 salvo la pista 1, ×16) + `0x5859F0(0, 57)`; sin mapa y `0xBAA370 == 0` → `0x584850` (sprite 57, `(−orientación) − (dir−90)·rad`); resto: `0x56E400`, altura por `z−2 > cz` / `z+4 < cz`, `0x584070(x, y, tamaño, c>>24, c>>16, c>>8, c, altura)` y, con mapa, `0x5859F0(1, blipIndex)`.

Detalles que la fuente reproduce a propósito (y el banco comprueba): `[BAA370..BAA380]` son estáticas locales del EXE (`StaticRef`, no estáticas de la DLL); el primer `0x583530` y los `0x5832F0/0x583480` de la pista son llamadas cuyo resultado se descarta pero se hacen; `FindPlayerVehicle` se llama 4 veces (los términos salen de las llamadas 4,3,1,2) y 1 más con el zoom; las constantes son las del EXE (`RadarExeFloat`: 0.9 = 0x3F666666, 500, 94, 0.6, 16, 90, π = 0x40490FDB…); los argumentos de color se pasan sin enmascarar (`c>>24`, `c>>16`, `c>>8`, `c`).

## Revisión propia contra el EXE (fase 2)

Se desensambló dos veces (`dis.sh 0x587000 0x587C10`, más 0x585FF0, 0x584070, 0x583AF0, 0x4549A0, 0x4552A0, 0x4048E0/0x404910/0x465040, 0x821B40) y se contrastó línea a línea; además el **ensayo sin juego** ejecuta los bytes del EXE.

- No existía borrador en C++ (la función era `plugin::Call<0x587000>`): no hay discrepancias que corregir respecto a una versión previa; el árbol principal no tenía cambios ajenos en Radar.cpp/Radar.h/X87Math.cpp/.h.
- Hallazgos de la lectura (todos reproducidos en la fuente): (1) la altura que se pasa a `0x584070` es un dword con basura en los 24 bits altos (escritura de un byte sobre el slot de la entidad); el destino sólo lee el byte bajo (`movzx eax, byte [esp+74h]`): el banco enmascara a 8 bits; (2) el sprite de `0x585FF0` se empuja como `movsx ax, byte` con los bits altos de EAX de `HasThisBlipBeenRevealed` y el destino lee el word bajo sin signo: el banco lo registra como `(int16)(int8)`; (3) `isSprite` llega con los 24 bits altos de basura (sólo `[esp+54h]` byte); (4) `fcom` con NaN pone C0 (la rama «menor» del destello); `fcomp` + `test ah,5/41h` + `jp/jnp` se tradujeron una a una con la tabla NaN; (5) la función **no inicializa `pos`** para el aeropuerto lejano (ver «Riesgos»); (6) con zoom el color de la flecha se arma con `(c>>24, c>>16, c>>8, 255)` (no con `GetRadarTraceColour` enmascarado).
- Discrepancia única hallada por el ensayo, del **banco** y no de la fuente: la DLL pasa `eRadarSprite` (8 bits) a `DrawRadarSprite` extendido con ceros, el EXE con signo (461 casos de 12288 con sprite negativo); `SSprite` canoniza a `(int16)(int8)` en ambos lados.
- Resultado del ensayo (con la corrección anterior): ver «Ensayo sin juego».

## Llamadores y registros de salida

Un único llamador en el EXE: `0x58837C` (`CRadar::DrawBlips`: `push ebx; push esi; call 0x587000; add esp,8`, luego `add edi,28h; inc esi; cmp edi,0BAA26Ch; jl`): no lee EAX/ECX/EDX tras la llamada (ni en el bucle que sigue, 0x588384..0x5883A2). Búsqueda de referencias: sólo hay un `call rel32` a 0x587000 y ningún `dword 0x00587000` en el EXE (sin tablas de punteros). `kCompareVolatileRegisters=false`: se registran en `original_residue/candidate_residue`. Preserva EBX/ESI/EDI/EBP (push/pop) y la pila x87 queda vacía.

## Colaboradores: qué se sustituye y por qué

Se sustituyen por **falsos con traza, idénticos en ambos lados** (`EntryPatch` en la dirección del EXE y en el símbolo y el cuerpo de la DLL, sólo durante la llamada; los cuerpos se calculan antes de parchear):

| EXE | Colaborador | Por qué |
|---|---|---|
| 0x56E0D0, 0x56E400 | `FindPlayerVehicle`, `FindPlayerCentreOfWorldForMap` | dependen del jugador real; el falso devuelve vehículos/centro de la fixture (el vehículo por número de llamada) |
| 0x583530, 0x5832F0, 0x583480 | transformaciones y límite del radar | son otras funciones ya validadas; sus salidas las fija la fixture para forzar cada rama (0.9, 1.0, NaN…) y sus **argumentos se comparan en bits** (la aritmética propia de la función queda en esos argumentos) |
| 0x584770, 0x584480 | `GetRadarTraceColour`, `ShowRadarMarker` | idem |
| 0x584070, 0x584850, 0x727D60, 0x585FF0, 0x5859F0, 0x583AF0 | dibujo y leyenda | efectos de dibujo; se registran todos los argumentos |
| 0x43EA00, 0x441DB0 | `GetPositionRelativeToOutsideWorld`, `GetHeading` | el falso desplaza la posición con constantes y devuelve el rumbo de la fixture |
| 0x4552A0, 0x4839A0 | `GetActualPickupIndex`, `GetActualScriptThingIndex` | índice fijado por la fixture (puede ser negativo) |

Reales (sin sustituir): los pools (se intercambian los punteros 0xB74490/94/9C por tablas falsas de 8 ranuras con la capacidad y el mapa de estados del EXE), `CPickup::GetPosn` (lee el int16 de un pickup real, restaurado), `ScriptSearchLightArray[0..3]` (entradas reales, restauradas), el constructor de `CRGBA` (0x7170C0), `_ftol2` y la matemática x87/SSE de la función (incluido `RadarAirstripRotateCpp`). **Un banco con colaboradores sustituidos no prueba a esos colaboradores**: cada uno tiene (o necesita) su propio banco; `RadarFinal3` cubre `CRadar::*` con sus llamantes.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- La **traza completa** de llamadas a colaboradores (nombre, orden y argumentos en bits), la fila `trace` del blip (40 bytes, incluidos los de relleno: la función sólo cambia el radio), las estáticas `0xBAA370..0xBAA383` (20 bytes con relleno) y que **el resto de la memoria vigilada no cambie** (array de blips completo salvo la fila, 15 regiones globales (17 con el array de blips y las estáticas), todas las entidades/matrices/pools falsos: hash).
- ESP (cdecl: `popBytes = 8`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW completa de 24 bits, TOP, tags y ST válidos: no debe quedar ninguno; se ignoran FSW banderas/CC, FIP/FDP y registros vacíos; MXCSR sin banderas). `begin` registra `compare_dead_x87_registers=false` y `run_local.py` lo exige.
- `route_ok`: en el original 0x587000 no salta a la DLL; en el candidato sí (módulo ASI); y **cada uno de los 17 sitios sustituidos salta al ASI** (EXE y símbolo de la DLL) mientras dura la llamada. Las entradas parcheadas (hasta 52 direcciones de 5 bytes: EXE, símbolo y cuerpo de cada sitio, más 0x587000) y las regiones globales se comprueban restauradas tras cada lado y al final.
- `fixture_oracle.py` modela la función desde el desensamblado (no desde la fuente C++) con el x87 a 24 bits: `+ − × ÷ √` en float (exactos), `fild·fmul` y el giro con coseno/seno en racionales de ~200 bits con un redondeo por operación; propaga NaN y `sqrt` negativo = −NaN (0xFFC00000).

## Fixtures (12288 casos; `Fixtures.hpp`)

Mezcla de radices (no aleatoria) para que ninguna combinación quede sin cubrir: `d0` camino ×16 (CAR ×2, CAR con handle fallido/fuera de rango, peatón a pie ×2, peatón en vehículo ×2 —incluido el vehículo extra fuera del pool—, peatón con vehículo nulo, OBJECT, PICKUP, SPOT, pista cerca ×2, pista lejos/NaN, tipos inválidos 0/4/5/9..15, y «trucos»: tipo 3 en vehículos/peatones con/sin `m_pEnex`) × `isSprite` (con basura en los 24 bits altos) × zoom × pantalla del blip (0..3) × mapa abierto × corto alcance × remain/ShowMissionBlips × variantes (sprite negativo, cámara activa/otras, pila x87); los casos 8192.. repiten las combinaciones con otros floats. Cada caso fija: traza del blip (relleno incluido, radios 0.9..5, tamaños con signo, `m_pEntryExit`), banderas globales, pantalla (ancho/alto incluidos 0, 1 y 3840), orientación y rango, hora y estáticas (guarda con bits altos, pasos cerca de la longitud y del desbordamiento de `int16`, `ahora − último` en 0..5 y con envoltura), cámaras (la activa y las otras), pools y entidades (matriz/sin matriz, byte de tipo con basura en los bits altos, `m_pVehicle`, `m_pEnex`), vehículo del jugador por llamada (cuatro vehículos distintos para comprobar qué llamada alimenta cada término, posiciones a 0..5000 m, exactamente 500 m, NaN), salidas de los falsos (radar en 0.9 y 1.0 ± 1 ulp, NaN en una coordenada, distancias, pantalla), altura del jugador en los bordes exactos (`z−2`, `z+4`), foco y pickup (índice negativo incluido). Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX constante.

`run_local.py` exige la igualdad con el oráculo en ambos lados y cobertura mínima (calibrada con el generador compilado en el anfitrión y el oráculo, ≈ 40–55 % de lo observado; ver `MINIMUMS`).

## Ensayo sin juego (`offline/`)

`offline/build.bat` compila (fuera de `build\`, sólo lee `build\source\gta_reversed.vcxproj`): `prepare.py` extrae el texto de `DrawEntityBlip` de `Radar.cpp` byte a byte, `X87Math.cpp` con `/O2 /Oi /arch:IA32 /fp:fast` y `main.cpp`, que mapea el EXE real en 0x400000 de un proceso hijo suspendido y ejecuta el `Run()` real de `Core.hpp` (mismas fixtures, falsos y compuertas) con un `HookRoute` que parchea 0x587000 hacia el cuerpo de la DLL. Genera `radarblip28-runtime.jsonl`, que se analiza con `run_local.py --analyze`. **No sustituye a la corrida en el juego** (no ejercita el hook RH, el resto del ASI, los pools ni las estructuras reales del juego en marcha).

**Resultado del ensayo (18/10/2026, tras corregir `SSprite`):** `run_local.py --analyze` sobre el registro del ensayo: **12288 pares, 0 diferencias** (EXE == DLL == oráculo en los 12288), todas las compuertas de ABI/ruta/restauración en ambos lados, `fp_difference` vacío, cobertura mínima cumplida. La primera ejecución dio 461 diferencias, todas del banco (argumento `eRadarSprite` de `DrawRadarSprite`: la DLL lo extiende con ceros y el EXE con signo; sólo el byte bajo es contrato) y ninguna de la fuente. `LiveComparison.hpp` (con el `HookRoute` y `ProbeRuntime.hpp` reales), `Core.hpp`, `Fixtures.hpp`, la función de `Radar.cpp` y `RadarAirstripRotateCpp` compilan con las cabeceras y opciones reales del proyecto (unidades aisladas; el proyecto no se ha compilado ni enlazado).

**Sensibilidad del banco (mutaciones de la fuente, 4096 casos cada una, ensayo sin juego):** M1 `dd >= 0.9` -> `>` en el destello: 21 diferencias; M2 sin `HasThisBlipBeenRevealed`: 636; M3 radio del segundo círculo con la altura en vez del ancho: 21; M4 `z + 3` en vez de `z + 4`: 15; M5 `p4` en lugar de `p3` en el término Y: 37; M6 «remain» ignorado: 24; M7 signo de la orientación en la pista: 60; M8 alfa 254 en `DrawRadarSprite`: 382; M9 `fcom` con NaN (`!(d < 0.9)` en lugar de `d >= 0.9`): 26 (0 antes de permitir NaN en los puntos de radar de las fixtures); M10 radio del marcador de depuración tras restarle 0.1: 414. Las diez se detectan.


## Suposiciones y riesgos

- **Aeropuerto lejano** (`d ≥ 500` o NaN): el EXE deja `pos` sin inicializar; la DLL lo deja a cero. Sólo afecta a argumentos de colaboradores de depuración y a la primera `0x583530` (resultado sustituido por la fixture, tras lo cual el EXE recalcula la posición desde el blip), así que nada visible depende de ello; el banco enmascara a cero (en los dos lados y en el oráculo) los argumentos derivados de `pos` en ese caso. Si algún día se desea reproducir la basura, habría que fijar los 3 dwords de la pila en `[ESP_entrada − 0x10 .. −8]` y no se puede hacer en C++.
- Handles de pool con índice fuera de la capacidad: el EXE no comprueba límites (lee el mapa de estados más allá del final); `CPool::GetAtRef` de la DLL sí (devuelve nulo). El banco usa índices 8..47 con el mapa de estados relleno de 0x80 (nunca coincide) y evita `handle & 0xFF == 0x80` allí. Con un handle real nunca ocurre.
- El peatón con `bInVehicle` y `m_pVehicle` nulo sale; un vehículo/peatón cuyo byte de tipo (bits 0..2) valga 3 usa `m_pEnex` en `+0x78C`: el banco fija esa memoria. En objetos (0x19C bytes) no se escribe `+0x78C`: el generador evita el tipo 3 allí.
- La hora `now − último` se compara sin signo (`> 3`): el banco incluye valores con envoltura.
- Los falsos devuelven `HasThisBlipBeenRevealed` como `bool` (AL) y las transformaciones por puntero oculto (`CVector2D* f(out, in)`), igual que los símbolos de la DLL; `GetHeading` es `__fastcall(this)` y devuelve el float en ST0.
- Si `RH_ScopedInstall(DrawEntityBlip, 0x587000)` convive con otro hook sobre 0x587000, el banco lo vería como `route_ok=false`.
- Cobertura por trazado de instrucciones: no hay; es por entradas serializadas + oráculo independiente (`coverage_method` del resumen).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py`, `source.patch`, `README.md`, `offline/{build.bat,main.cpp,prepare.py,mkrsp.py}`.
