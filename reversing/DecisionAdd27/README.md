<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco DecisionAdd27: CDecision::Add (0x600600) y CDecisionMakerTypes::AddEventResponse (0x6044C0)

**Estado: APROBADO** (18/10/2026, corrida `20261018-144229-450592`: 10752 pares (6144 Add + 4608 AddEventResponse), 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-decisionadd27\20261018-144229-450592`; archivado en `reversing/CppTwelveLevel6Final/runs/DecisionAdd27-20261018-144229-450592/`. Sonda compilada desde el worktree D:\Original\gta-wt-dm2 (HEAD 7a759750f) con fuentes idénticas byte a byte a las del árbol principal. Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, filas nuevas de `docs/hooks.csv`).

Plantilla: `reversing/DecisionMakeDecision27` (Core/Fixtures/LiveComparison/build_probe/run_local/fixture_oracle/register_hook) y contrato de `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.). Varias funciones en un banco, en rutas separadas: `DependenciesDecisionPed7`.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh DecisionAdd27          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/DecisionAdd27/build_probe.py
python -B reversing/DecisionAdd27/run_local.py
python -B reversing/DecisionAdd27/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis
python -I reversing/DecisionAdd27/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; ya pasa: imprime «ok»)
python -B reversing/DecisionAdd27/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(Add, 0x600600)` (categoría/clase `CDecision`) y `RH_ScopedInstall(AddEventResponse, 0x6044C0)` (clase `CDecisionMakerTypes`); `HookRoute` los busca por esos nombres. Archivo de arranque en el clon: `decisionadd27-runtime.start`; salida `decisionadd27-runtime.jsonl`. 10 752 pares (6144 + 4608).

## Qué hacen las funciones (desensamblado 0x600600..0x6006AB y 0x6044C0..0x6044F0)

**CDecision::Add**, `this` en ECX (→ EDI), pila tras los tres `push` de registros: `taskId`=+10h, `chances`=+14h (→ EBX), `flags`=+18h; `ret 0Ch`.
1. Recorre las ranuras i=0..5. Una ranura está **libre** si: `dword[this+4i] == -1`, los cuatro `byte[this+18h+4i+j]` valen 0 (`cmp byte,0 ; ja`: comparación SIN signo, así que 0x80..0xFF la ocupan) y los dos `byte[this+30h+2i+k]` valen 0 (`jne`: cualquier byte ≠ 0 la ocupa).
2. En la **primera** libre: guarda `taskId`; para j=0..3 `fld dword [chances+4j]` + `call 0x821B40` (_ftol2) y guarda sólo AL (byte bajo del truncado a int64); `bool[2i]=(dword[flags]!=0)`, `bool[2i+1]=(dword[flags+4]!=0)` (`setne`, dword completo). Sin libre: no escribe nada. Nunca escribe en `chances`/`flags`.
3. `_ftol2` 0x821B40 es la versión x87 del CRT: `fistp qword` (redondeo según FCW), corrige a truncado hacia cero con `x - q`; NaN, ±inf y |x| ≥ 2^63 dan el indefinido 0x8000000000000000 (byte 0) con la excepción de operación inválida enmascarada. El banco fija FCW a 24 bits, al más cercano y con TODAS las excepciones enmascaradas (como el juego).

**CDecisionMakerTypes::AddEventResponse**, `this` en ECX; args: `decisionMakerIndex`=+4, `eventType`=+8, `taskId`=+0Ch, `chances`=+10h, `flags`=+14h; `ret 14h`. Calcula `W = this + 4 + (u32[this + 0xC034 + eventType*4] + decisionMakerIndex*41) * 0x3C` con **aritmética de 32 bits que da la vuelta** (`imul edx,edx,29h ; add ; imul eax,eax,3Ch ; lea ecx,[eax+ecx+4]`, sin comprobar ningún límite), reempuja `taskId, chances, flags` y llama a `0x600600` con `ECX=W`; devuelve con `ret 14h`. Verificado: 4 + 20·0x99C = 0xC034 (`static_assert` en `LiveComparison.hpp`).

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Add**: objeto `CDecision` de 0x3C bytes (arena con guardas de 0x40 antes/después), arena de argumentos (`chances` 16 B + `flags` 8 B + guardas, comparada ENTERA: no se modifican), ESP (`ret 0Ch`/`ret 14h`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW completa, TOP, tags y ST válidos: no debe quedar ninguno; se ignoran FSW banderas/CC, FIP/FDP y registros vacíos; MXCSR sin banderas).
- **AddEventResponse**: arena sintética `CDecisionMakerTypes` de 0x18000 bytes con `this` en +0x3000 y relleno pseudoaleatorio en TODA la arena (incluidas las `CDecision` vecinas y el resto de `m_EventIndices`). Se coloca la `CDecision` destino (pre-imagen) en W y el índice leído en P = `this + 0xC034 + eventType*4`. Se compara: los 0x3C bytes de la ventana W tras la llamada, que **nada fuera de W cambie** (`outside_ok`, `first_outside`), `chances`/`flags` sin modificar y las mismas compuertas de ABI que Add. Así se ve «qué `CDecision` recibe la llamada» y su contenido final.
- **EAX/ECX/EDX de salida: sólo se registran** (`original_residue`, `candidate_residue`), `kCompareVolatileRegisters=false`. `python -I reversing/Dependencies25A/residue_scan.py` da: 0x600600 → llamadores 0x602B3A y 0x6044EA; 0x6044C0 → 0x46D15A y 0x46E552; ninguno lista lecturas de EAX/ECX/EDX tras el `call`. 0x602B3A está dentro de otra función (0x602B20, `ret 10h`, que llama a Add como última instrucción): `residue_scan.py 0x602B20` no devuelve llamadores, pero el revisor debe confirmar a mano si alguien la usa y lee los registros. En el EXE el residuo de Add es EAX = puntero `flags`, ECX = `flags[1]`, EDX = parte alta del último truncado con el byte bajo = `flags[1]!=0` (dependiente del fixture); la DLL no puede reproducirlo.
- Rutas (`route_ok`): original → el hook de la ruta NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI; **en ambos lados** `0x821B40` (_ftol2) no está enganchado, y en la ruta AddEventResponse tampoco `0x600600`. El hook de Add queda **apagado** en la ruta de AddEventResponse: el original llama al Add del EXE y el candidato al Add en C++ de la DLL (directo, sin pasar por el EXE). Se vigilan 16 bytes de 0x600600, 0x6044C0 y 0x821B40 (deben seguir siendo el código del EXE al volver cada llamada). `collaborators_stubbed=false`.

## Fixtures (`Fixtures.hpp`)

### Add: 6144 casos, 16 familias (`kind = n % 16`, 384 casos cada una)
0 ninguna libre; 1..6 exactamente k libres en posiciones aleatorias; 7 las seis libres; 8 una libre en la posición `(n/16) % 6` y las demás «cercanas a libre» (UN solo motivo de ocupación: sólo la tarea ≠ −1, o sólo una probabilidad ≠ 0 en una posición j, o sólo un bool ≠ 0); 9 barrido de las ~85 chances especiales (cada una aparece varias veces en cada componente); 10 barrido de `flags` (0, 1, −1, 2, 0x100, 0x80, 0x10000, 0x01000000, 0x80000000, 0x7FFFFFFF, 0xFFFFFF00, 0x00FFFF00, …); 11 tareas extremas (−1, INT_MIN, INT_MAX, 200, 0xFFFF, 0x10000, 0x100C8, …); 12 ranuras anteriores ocupadas por probabilidades 0x80..0xFF (comparación sin signo) y libres desde `(n/16)%6`; 13 bytes de bool no canónicos (2, 0x80, 0xFF) en las ocupadas; 14 sólo la ranura 5 libre; 15 aleatorio (0..6 libres).
- Estilos de ocupación: sólo tarea, una probabilidad (0x7F/0x80/0xFF/…), un bool, tarea + basura, varias probabilidades, ambos bools, todo lleno. Las ranuras libres exigen tarea −1 y ceros exactos.
- Chances (8 modos por caso): enteros típicos, fracciones k/16, tabla especial (NaN positivo/negativo/con carga —nunca señalizadores—, ±inf, ±0, denormales, 0.5, 0.99999994, 255.9, 255.99998, 256, 256.5, 65536+x, 2^24, 2^31, 2^32, 2^63±1 ulp, 2^64, 1e19, 1e30, ±FLT_MAX, −2^63, negativos hasta −16777216…), una especial entre típicas, las cuatro iguales, bytes 0/1/127/128/129/254/255, frontera 255.x/256.x/65536+x y negativos aleatorios. Se compara el byte resultante tras `_ftol2` en cada una de las 4 posiciones.
- Flags: 0 / ≠ 0 en cada posición, con valores de byte bajo cero (0x100, 0x01000000, 0x80000000, 0xFFFFFF00) que delatan una comparación de byte en vez de dword.
- Registros: semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = objeto.

### AddEventResponse: 4608 casos = 8 modos de dirección (`n % 8`) × 16 familias de contenido de la `CDecision` destino (`(n/8) % 16`, las mismas de Add), 36 repeticiones de cada pareja
Modos: 0 normal (dm 0..19, índice 0..40, eventType 0..95); 1 `decisionMakerIndex` fuera de rango (−4..−1, 20..22); 2 índice leído fuera de 0..40 (41..500, −1..−500); 3 `decisionMakerIndex + k·2^30` (k ∈ {−2,−1,1,2,3}); 4 índice leído + k·2^30; 5 `eventType + k·2^30` (el `eventType*4` da la vuelta y vuelve a la misma P); 6 ventana W arbitraria (múltiplo de 4, fuera de la rejilla de 0x3C) resolviendo un índice enorme para que `(idx + dm·41)·0x3C ≡ W − this − 4 (mod 2^32)`; 7 P en cualquier lugar de la arena (eventType negativo o grande, a veces + k·2^30).
- **Restricción a la arena**: todo valor que da vuelta en 32 bits se acepta SÓLO si W y P caen dentro de la arena (`WindowOK`/`TableOK`: W ∈ [0x40, 0x18000−0x3C−0x40], P no solapa W). Como 0x3C = 4·15 y 41 es impar, sumar k·2^30 a dm, índice o eventType no cambia W ni P; así los valores «dan la vuelta» y aun así apuntan a la arena. Si el generador no halla un par válido en 64 intentos usa un respaldo fijo (dm=1, idx=2, ev=3); `generator_ok` queda registrado y el oráculo recalcula W y P independientemente y falla si no coinciden con el `input`.
- Una implementación con aritmética de 64 bits o con comprobación de límites escribiría fuera de la ventana (o en otra) y lo delataría `outside_ok`/la ventana; el riesgo de que una fuente incorrecta escriba fuera de la arena existe como en cualquier banco de este tipo (la sonda captura accesos inválidos).

`fixture_oracle.py` modela ambas funciones desde el desensamblado (con el algoritmo literal de `_ftol2`, contrastado en la autoprueba con `math.trunc` en 200 000 floats aleatorios y casos de frontera) y comprueba cada lado; `run_local.py` exige cobertura mínima por función (ranuras libres 0..6, ranura rellenada 0..5, cada motivo de ocupación —tarea, cada probabilidad, 0x80+, cada bool, bool no canónico—, near-free, clases de chances NaN/inf/enorme/negativa/≥256/frontera 255/65536+x/denormal/−0, byte resultante 0x80/0xFF/0, las cuatro combinaciones de flags, tareas extremas y, en AddEventResponse, los ocho modos y las clases de dm/índice/eventType). **Los mínimos son conservadores pero no se comprobaron contra el generador (no se compiló): si alguno falla por poco, bajar el umbral o ajustar la mezcla de la familia.**

## Fuente actual: coincidencias y discrepancias vistas (otro agente revisa la fuente)

- `Decision.cpp::Add` y `DecisionMakerTypes.cpp::AddEventResponse` reproducen, en lectura, el desensamblado: mismos tres filtros de ranura (tarea == −1, probabilidades `> 0` sobre `uint8`, bools `!= 0`), primera libre, byte bajo de `static_cast<int64>(float)`, bools `flags[k] != 0` como dword y aritmética `uint32` para W/P con `eventType` como índice `int32`. **No he visto ninguna discrepancia de lógica.**
- Dependencias a vigilar con el banco: (1) `static_cast<int64>(float)` es comportamiento no definido para NaN/inf/|x| ≥ 2^63; el banco lo comprueba (esperado: byte 0, igual que el indefinido de `fistp`). (2) El EXE interpone la lectura de `chances[j]` y la escritura de cada byte; la DLL hace lo mismo en el mismo orden, pero si `chances` apuntara dentro del propio objeto el orden importaría (el banco NO prueba aliasing: no ocurre en el juego). (3) `AddEventResponse` está compilada con las opciones por archivo de `DecisionMakerTypes.cpp` (`/Zi`, RTC por defecto) y `#pragma runtime_checks`; el banco comprueba ESP/no volátiles/DF reales.
- `docs/hooks.csv` no contiene filas `CDecision,Add,0x00600600` ni `CDecisionMakerTypes,AddEventResponse,0x006044C0` (sí `From`, `Set`, `SetDefault`, `MakeDecision`, `FlushDecisionMakerEventResponse`…): al registrar habrá que añadirlas (no lo hace `register_hook.py`, que no toca `docs/`).
- El banco `DecisionMakeDecision27` ya señala que el `DecisionMakerTypes.cpp` del árbol principal tenía un cambio de `AddEventResponse` no validado: este banco es el que lo valida; los hashes de `artifacts/build-manifest.json` de ESTE banco incluyen `DecisionMakerTypes.cpp` y `Decision.cpp` del árbol donde se compile (comprobar que no sea un worktree distinto del que se registra).

## Suposiciones a vigilar al compilar

- `Decision.h`, `DecisionMakerTypes.h` y `CutsceneMgr.h` se incluyen desde `LiveComparison.hpp`; los hooks son `Simple` y `HookRoute::RegisteredTarget` toma el cuerpo con `GetHookOurAddress()`.
- Nombres de hook: clase `CDecision`, nombre `Add`; clase `CDecisionMakerTypes`, nombre `AddEventResponse`.
- `sizeof(CDecision)==0x3C` y `4 + 20·sizeof(CDecisionMaker) == 0xC034` (`static_assert`).
- Se usan `std::bit_cast` constexpr con float literales (`B(...)`) y `std::ranges::all_of` (C++20).
- `_controlfp(_PC_24|_RC_NEAR|_MCW_EM, …)` por lado; el x87 de la sonda se restaura con `frstor` tras cada lado.
- `HookRoute::Invoke` enciende/apaga sólo el hook de la ruta; en la ruta de AddEventResponse el hook de Add queda apagado (comprobado por `route_ok`).
- No hay arnés `offline/`; sólo el oráculo se autoprueba.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py`, `README.md`.
