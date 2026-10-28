<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco FxEmitterAddParticle30: FxEmitter_c::AddParticle(const CVector&, ...) (EXE 0x4A3EA0)

Estado: **PREPARADO** (fuente C++ escrita y contrastada sin juego; el banco NO se ha compilado como sonda ni ejecutado dentro de GTA: lo hace el coordinador en serie).

| Función | Entrada | Convención | Pares |
| --- | --- | --- | --- |
| FxEmitter_c::AddParticle (sobrecarga con vector de posición) | 0x4A3EA0 (slot +0x14 de la vtable 0x85A7A4) | thiscall, 7 argumentos de pila, `ret 1Ch`; `void` | 4096 |

La sobrecarga con matriz es **0x4A4050** (slot +0x10) y NO se toca (sigue en el EXE). El comentario de HEAD `RH_ScopedOverloadedInstall(AddParticle_Reversed, "matrix", 0x4A3EA0, ...)` estaba al revés: 0x4A3EA0 construye una matriz identidad con la posición, o sea es la de vector.

Hook RH **Virtual** (`RH_ScopedVMTOverloadedInstall(AddParticle, "vector", 0x4A3EA0, ...)`, nombre `AddParticle-vector`; fila `FxEmitter_c,AddParticle-vector,0x004A3EA0,1,0,1,V` de docs/hooks.csv, que añade el coordinador). Un hook virtual cambia la entrada del EXE en la vtable y parchea además 0x4A3EA0 para las llamadas directas.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh FxEmitterAddParticle30        # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/FxEmitterAddParticle30/build_probe.py
python -B reversing/FxEmitterAddParticle30/run_local.py
python -B reversing/FxEmitterAddParticle30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl    # solo análisis (None si el registro no existe o no ha terminado)
python -I reversing/FxEmitterAddParticle30/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -B reversing/FxEmitterAddParticle30/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar SCOPE_NOTE antes)
```
Archivo de arranque en el clon: `fxemitteraddparticle30-runtime.start`; salida `fxemitteraddparticle30-runtime.jsonl`. `build_probe.py` y `run_local.py` fijan `runtime_runner.GAME = E:/JUEGOS/GTA San Andreas - CVehicle Test`. Compila `Virtual.h/.cpp` (el cuerpo de la DLL se lee del propio hook Virtual, no de un puntero a miembro virtual).

## Fuente (FASE 1)

`source.patch` (git diff unificado de 1 línea de contexto contra HEAD, SÓLO esta función; `git apply --check` correcto sobre `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Fx/FxEmitter.cpp`: `FxEmitter_c::AddParticle(const CVector&, ...)` pasa del borrador sin compilar (con un `return plugin::CallMethod<0x4A3EA0, ...>` delante) a la reconstrucción completa, y se activa el hook Virtual (el comentario de HEAD con el nombre «matrix» se sustituye).
- `source/game_sa/X87Math.cpp` / `.h`: bloque «Lote 30: ... prefijo FxAddParticle» **al final** del archivo (para que el coordinador concatene): `FxAddParticleRotZByteCpp` (byte bajo de `_ftol2(rotZ * 0.5f)`, `noinline`, `float_control(precise, on)`).
- **Dependencia con `FxEmitterCreateParticle30`**: `AddParticle` llama a `FxEmitter_c::CreateParticle` (no virtual). Con sólo este parche esa llamada iría a la versión de HEAD (borrador sin hook); en producción hay que aplicar los dos parches (se componen sin conflictos salvo los dos bloques al final de X87Math.cpp/.h, que concatena el coordinador). En el banco `CreateParticle` se sustituye por un stub en la dirección del EXE y en la entrada de la DLL, así que el banco no depende del estado de ese hook.

Colaboradores y decisión de cada uno: `FxInfoManager_c::ProcessEmissionInfo` (0x4A4960), `FxManager_c::FxRwMatrixCreate` (0x4A9440) y `FxRwMatrixDestroy` (0x4A9460), `FxPrimBP_c::GetRWMatrix` (0x4A9DC0) ya están en C++ y se llaman en C++; `RwMatrixUpdate` (0x7F18A0) y `RwMatrixMultiply` (0x7F18B0) son envolturas de RenderWare que llaman al EXE por dirección; `CreateParticle` (0x4A2580) se llama en C++ (miembro, hook activado en el otro banco); `_ftol2` es el del CRT de la DLL (el banco compara su resultado con el del EXE). Siete se sustituyen por stubs idénticos en ambos lados (ver más abajo).

## Qué hace la función (0x4A3EA0..0x4A4041)

```
ProcessEmissionInfo(this = &primBP->m_FxInfoManager, 0.0f, 0.0f, systemBP->m_fLength, (byte) m_UseConstTime, &emision)   ; emision (0x68) local de pila
A = FxRwMatrixCreate() ; B = FxRwMatrixCreate() ; C = FxRwMatrixCreate()
C: right.x = up.y = at.z = 1.0f ; flags |= 0x20003 ; columnas restantes a 0 ; pos = *pos   (los rellenos +0x1C, +0x2C, +0x3C intactos) ; RwMatrixUpdate(C)
padre (m_System->m_ParentMatrix) ? RwMatrixMultiply(B, C, padre) : B = C (rep movs, 16 palabras)
D = FxRwMatrixCreate() ; m_PrimBP->GetRWMatrix(D) ; RwMatrixMultiply(A, D, B) ; FxRwMatrixDestroy(D)
p = CreateParticle(emision, A, &vel, timeSince, &mult, brightness, createLocal)
si p != 0 y !(rotZ < 0 o NaN) [fld rotZ; fcomp 0.0; test ah, 1; jne]: p->m_RotZ (byte +0x36) = low8(_ftol2(rotZ * 0.5f))   (constante 0x858B8C)
FxRwMatrixDestroy(C) ; FxRwMatrixDestroy(B) ; FxRwMatrixDestroy(A)
```

## Revisión propia contra el EXE (FASE 2)

Se escribió desde el desensamblado (`dis.sh 0x4A3EA0 0x4A4050`; orden de las pilas de llamada trazado a mano) y se contrastó de tres formas:

1. **Línea a línea con el desensamblado** (qué matriz es cuál: A resultado, B mundo, C identidad, D la del prim; orden de `Destroy`: C, B, A; `rep movs` de 16 palabras; orden de los argumentos de `RwMatrixMultiply` (destino, D, B) y de `CreateParticle`; constante 0.5f de `rotZ`).
2. **Ejecución de los bytes reales del EXE** (`offline/`, sin juego): `gen_exe.py` extrae de `gta-sa.exe` los 418 bytes de AddParticle y `_ftol2` con sus reubicaciones; `cphadd30.exe` los ejecuta frente al **mismo texto de FxEmitter.cpp** (`extract.py`) con `X87Math.cpp` compilado con las opciones de producción y los mismos colaboradores sustituidos de `Fixtures.hpp`, y compara memoria completa, traza, ABI y x87 normalizado. Resultado: **0 diferencias en las 4096 fixtures, en 1.000.000 de casos aleatorios** (palabras crudas, incluidos NaN señalizantes en pos/vel/rotZ) con el cuerpo en `/O1` y en 300.000 con `/Od` (como en el proyecto). Mutantes deliberados (`rotZ >= 0` -> `rotZ > 0`: 66 diferencias; flags `0x20003` -> `0x20001`: 2036) quedan detectados.
3. **Oráculo en Python independiente** (`fixture_oracle.py`, desde el desensamblado): contrastado con el lado EXE del arnés, 4096/4096 sin diferencias; las fixtures de `Fixtures.hpp` compiladas con `cl` y las del espejo en Python son idénticas campo a campo (`offline/cmp_o.py`).

Además, `offline/livetest.exe` ejecuta **el mismo `Core.hpp` que se compila en la sonda** (original = bytes del EXE reubicados, candidato = C++) y escribe un `.jsonl` con el formato del banco; `offline/run_local_check.py` pasa `run_local.py::analyze` sobre él: 4096 pares aceptados y cobertura según lo calibrado. La sintaxis de `FxEmitter.cpp`, `Core.hpp` y `LiveComparison.hpp` se comprobó con `cl /Zs` y las opciones del proyecto (sin errores).

Discrepancias halladas y corregidas en el **borrador de HEAD** (nunca compilado, y precedido de un `return plugin::CallMethod<0x4A3EA0, ...>` que lo dejaba inerte):
- `mat0 = mat;` asignaba punteros: el EXE copia las 16 palabras de C a B con `rep movs` (de modo que B y C son matrices distintas y C se destruye después con su contenido intacto).
- `m_RotZ = (uint8)(rotZ / 2.0f)`: la división entre 2 es idéntica a `* 0.5f`, pero el `(uint8)` de un float mayor que 255 o menor que 0 no es el byte bajo del entero de 64 bits que deja `_ftol2` (para `rotZ >= 512` el EXE da el byte bajo del truncado, no una saturación); se sustituye por `FxAddParticleRotZByteCpp`. (La sobrecarga con matriz, que no es esta función, usa además `* 2.0f` en el borrador, al revés que el EXE.)
- El orden de creación/destrucción de las matrices, las llamadas `RwMatrixMultiply` y la identidad con `RwMatrixSetIdentity` del borrador SÍ coincidían con el EXE (la macro de RenderWare hace `flags |= identidad | ortonormal` igual que el EXE); se conservó la lógica con escrituras explícitas campo a campo para que no dependa de la macro (que en `RWDEBUG` es una función).
- El hook estaba comentado con el nombre `"matrix"`, que es el de la otra sobrecarga.
- Entre la versión final y los bytes del EXE no se halló ninguna otra discrepancia en el arnés.

Límites de la revisión: **no se compiló el proyecto completo** (sólo `cl /Zs` de la fuente y del banco, y la compilación independiente de `X87Math.cpp` con las opciones de producción).

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Arena** (0x340 bytes con 16 de guarda por lado: emisor, prim, sistema, blueprint, matriz padre, cuatro matrices del pool con basura de 32 bits en flags y rellenos, `pos`, `vel`, `FxPrtMult_c` y la partícula) completa, en bits; guardas intactas. Cualquier escritura fuera de lo esperado (rellenos de la matriz C, matriz padre, `vel`, partícula) aparece como diferencia.
- **Traza de colaboradores** (siete stubs, EntryPatch en la dirección del EXE y en la entrada de la DLL con los mismos cuerpos): `ProcessEmissionInfo` (this, bits de los tres floats, byte de `m_UseConstTime`), `FxRwMatrixCreate` (qué matriz del pool), `FxRwMatrixDestroy`, `GetRWMatrix`, `RwMatrixUpdate` y `RwMatrixMultiply` (desplazamientos y hashes FNV de las matrices; el resultado es `hash + i`), `CreateParticle` (this, hash de la emisión, matriz de trabajo, `vel`, bits de timeSince/brightness, hash de `FxPrtMult_c`, `createLocal`); el stub devuelve `nullptr` o la partícula del arena según `particleMode`.
- **ABI**: ESP (`ret 1Ch`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW completa, TOP, etiquetas, ningún ST válido al salir), MXCSR sin banderas (`& 0xFFC0`). **EAX/ECX/EDX sólo se registran** (`callers-residue.txt`: la función es `void` y ningún llamador del EXE los lee).
- **Rutas**: original -> `0x4A3EA0` NO salta a la DLL; candidato -> sí (hook Virtual encendido), con el cuerpo en el ASI. Se vigilan 16 bytes de la entrada, del cuerpo y de los 14 puntos de colaboradores (`globals_restored`).
- **Oráculo** (`fixture_oracle.py`): arena completa y traza iguales a las de cada lado; **fixture == oráculo** (la fila `input` es exactamente `make(caso)` del espejo Python).

## Fixtures (`Fixtures.hpp`; espejo en `fixture_oracle.py`): 4096 casos, 8 familias × 512 (familia = n % 8)

Valores de `pos`, `vel`, `timeSince`, `brightness`, `FxPrtMult_c`, longitud, emisión, matriz padre y `GetRWMatrix` por `Val(caso, clave)`: natural (exponentes 118..131), especial (40 valores: ±0, desnormales, ±inf, NaN silenciosos con cargas distintas...) o extremo (exponentes 1..24 / 231..254); familias 4 (especiales), 5 (extremos), 6 (mezcla) y el resto benignas. `rotZ` sale de una tabla de 36 valores sensibles al byte bajo del entero (127, 128, 255, 256, 2^24, 2^31, 2^32, 2^63, ±inf, NaN, negativos, −0.0, desnormales...) en el 55 % de los casos. Además: `m_UseConstTime` (0/1), `createLocal` (0/1), padre presente (2/3), `particleMode` (3/4 con partícula) y basura de 32 bits en el contenido previo de las cuatro matrices del pool.

Cobertura calibrada con el modelo sobre las 4096 fixtures (reales entre paréntesis; mínimos en `run_local.py` con ~40 % de margen): con partícula (3054), sin partícula (1042), con padre (2768), local (2039), `m_UseConstTime` (2114), `m_RotZ` escrito (1909), omitido por `rotZ < 0` o NaN (1145), escrito con byte no nulo (670), `rotZ` NaN (186), `rotZ` negativo (1418).

## Suposiciones y riesgos del banco

- **sNaN no cubierto en `rotZ`/emisión/etc.**: las fixtures sólo generan NaN silenciosos (en el arnés aleatorio con sNaN crudos tampoco hubo diferencias porque la función no opera con ellos más que `rotZ`, que carga con `fld` y queda en cero tras `_ftol2`).
- **`createLocal` y `m_UseConstTime` canónicos**: el banco pasa 0/1; el EXE pasa a `ProcessEmissionInfo` el byte de `m_UseConstTime` sin normalizar (un valor distinto de 0/1 no está probado).
- **`CreateParticle` y los colaboradores** se sustituyen en ambos lados: probar un consumidor no valida a sus colaboradores (`CreateParticle` tiene su propio banco, FxEmitterCreateParticle30).
- **Hook Virtual**: el banco construye la ruta con `HookRoute("FxEmitter_c", "AddParticle-vector", 0x4A3EA0, <cuerpo de la DLL>)` y lee el cuerpo de `Virtual::GetHookOurAddress()`; la ruta del candidato exige que 0x4A3EA0 salte a la DLL (lo hace el parche simple interno del hook Virtual). Si el nombre del hook cambiara, el banco se detiene en `registered_hook = false` sin comparar nada.
- Si el compilador del proyecto en modo Release inlineara algún colaborador, los parches de entrada no lo interceptarían y la traza del oráculo fallaría (aviso claro, no falso positivo).
- Con sólo este parche (sin el de CreateParticle) la llamada interna iría al borrador de HEAD: ver «Fuente».

## Evidencia sin juego (`offline/`)

```
cd reversing/FxEmitterAddParticle30/offline
python -I gen_exe.py && python -I extract.py ../../../source/game_sa/Fx/FxEmitter.cpp      # exebytes.inc, add_body.inc (generados, no versionados)
./buildh.sh cphadd30.cpp                    # OPT=/Od ./buildh.sh cphadd30.cpp para el modo Debug
./cphadd30.exe 4096 6                       # fixtures: EXE reubicado frente al C++      (modo r: aleatorias)
python -I cmp_o.py                          # fixtures C++ == espejo Python y oráculo == lado EXE
EXTRA_INC="<ruta de nlohmann/include>" ./buildh.sh livetest.cpp && ./livetest.exe live_test.jsonl 4096 && python -I run_local_check.py live_test.jsonl
```
Requieren capstone, el EXE canónico `D:\Grand Theft Auto San Andreas\gta-sa.exe` y MSVC 14.51 (rutas en `buildh.sh`).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py`, `source.patch`, `callers-residue.txt`, `offline/` (`gen_exe.py`, `extract.py`, `buildh.sh`, `cphadd30.cpp`, `cmp_o.py`, `livetest.cpp`, `run_local_check.py`), `README.md`.
