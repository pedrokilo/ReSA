<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco FxEmitterCreateParticle30: FxEmitter_c::CreateParticle (EXE 0x4A2580)

Estado: **PREPARADO** (fuente C++ escrita y contrastada sin juego; el banco NO se ha compilado como sonda ni ejecutado dentro de GTA: lo hace el coordinador en serie).

| Función | Entrada | Convención | Pares |
| --- | --- | --- | --- |
| FxEmitter_c::CreateParticle | 0x4A2580 | thiscall, 7 argumentos de pila, `ret 1Ch`; devuelve la partícula en EAX (o 0) | 4096 |

Hook RH `FxEmitter_c / CreateParticle` (`RH_ScopedInstall(CreateParticle, 0x4A2580)`, tipo `S`, fila `FxEmitter_c,CreateParticle,0x004A2580,1,0,1,S` de docs/hooks.csv, que añade el coordinador). Hermana ya validada: `FxEmitter_c::CreateParticles` 0x4A41E0 (banco Dependencies22C).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh FxEmitterCreateParticle30      # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/FxEmitterCreateParticle30/build_probe.py
python -B reversing/FxEmitterCreateParticle30/run_local.py
python -B reversing/FxEmitterCreateParticle30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # solo análisis (None si el registro no existe o no ha terminado)
python -I reversing/FxEmitterCreateParticle30/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -B reversing/FxEmitterCreateParticle30/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar SCOPE_NOTE antes)
```
Archivo de arranque en el clon: `fxemittercreateparticle30-runtime.start`; salida `fxemittercreateparticle30-runtime.jsonl`. `build_probe.py` y `run_local.py` fijan `runtime_runner.GAME = E:/JUEGOS/GTA San Andreas - CVehicle Test`.

## Fuente (FASE 1)

`source.patch` (git diff unificado de 1 línea de contexto contra HEAD, SÓLO esta función; `git apply --check` correcto sobre `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Fx/FxEmitter.cpp`: `FxEmitter_c::CreateParticle` pasa de un borrador sin compilar a la reconstrucción completa, con el ayudante local `FxEmitterExeRemainder()` (`rand() % 10000` con el `rand` del CRT del EXE, **0x821B1E**, nunca el de la DLL); se activa `RH_ScopedInstall(CreateParticle, 0x4A2580)`; y `FxEmitter_c::CreateParticles` (0x4A41E0) llama ahora a `CreateParticle` en C++ en lugar de a `plugin::CallMethodAndReturn<..., 0x4A2580>`. **Consecuencia:** el banco `Dependencies22C` (CreateParticles) sustituye `CreateParticle` sólo en la dirección del EXE; hay que añadirle el parche de la entrada de la DLL (`Entry(&FxEmitter_c::CreateParticle)`, como hace `FxEmitterAddParticle30`) o repetir ese banco con la fuente nueva.
- `source/game_sa/X87Math.cpp` / `.h`: bloque «Lote 30: ... prefijo FxPrt» **al final** del archivo (para que el coordinador concatene): `FxPrtLifeCpp`, `FxPrtColorByteCpp`, `FxPrtBrightnessByteCpp`, `FxPrtRandByteCpp`, `FxPrtRotationCpp`, `FxPrtBoxPositionCpp`, `FxPrtSpherePositionCpp`, `FxPrtDirectionCpp`, `FxPrtSpeedCpp`, `FxPrtVelAddCpp` (todos `noinline`, compilados con `/O2 /Oi /arch:IA32 /fp:fast`, sumas y productos dentro de `float_control(precise, on)`, la raíz fuera de esa región).

Colaboradores y decisión de cada uno: `FxManager_c::GetParticle` (0x4A93C0), `FreeUpParticle` (0x4A9400), `FxPrimBP_c::GetRWMatrix` (0x4A9DC0), `FxEmitterBP_c::UpdateParticle` (0x4A21D0) y `RotateVecIntoVec` (0x4A1660) ya están en C++ y se llaman en C++; `RwV3dNormalize` (0x7ED9B0) y `RwV3dTransformVectors` (0x7EDDF0) son envolturas de RenderWare que llaman al EXE por dirección; `TList_c::AddItem` (0x4A8DF0) es la plantilla en línea de la DLL (el EXE ejecuta su código; la memoria resultante se compara); `_ftol2` es el del CRT de la DLL (el banco compara su resultado byte a byte con el `_ftol2` del EXE). En el banco, siete de ellos se sustituyen por stubs idénticos en ambos lados (ver más abajo).

## Qué hace la función (0x4A2580..0x4A2B19)

Orden exacto de las tiradas `rand() % 10000` (`cdq; idiv 2710h`) y de las llamadas:

```
GetParticle(0); sin hueco: si [system+0x62] & 0x20 -> FreeUpParticle + GetParticle(0) (si sigue sin hueco: devuelve 0)
r1: [p+0xC] = 0 ; [p+8] = ((r1 * 0.0002f - 1.0f) * lifeBias + life) * mult.life ; [p+0x28] = m_System
[p+0x2C..0x31] = low8(_ftol2(x * 255.0f)), x = mult.red, green, blue, alpha, size, rot ; [p+0x37] = createLocal
r2, r3, r4: [p+0x32], [p+0x33], [p+0x34] = low8(_ftol2((r * 0.0001f) * 255.0f)) ; [p+0x35] = low8(_ftol2(brightness * 100.0f))
r5: [p+0x36] = 0xFF ; [p+0x38] = (r5 * 0.0001f) * (rotMax - rotMin) + rotMin
si [p+0x37] != 0: GetRWMatrix(wld)
|radio| >= 0.001 (double 0x85A7C8) NO: caja (r6, r7, r8)   SÍ: esfera (r6, r7, r8 y, si !(radio < 0), r9)
  x' = float(x) + pos.x ; y' = float(y) + pos.y ; z' = z + pos.z (z sin pasar por float)
  [p+0x10..0x18] = ((z' * at + y' * up) + x' * right) + wld.pos        (cada eje redondeado a float al guardar)
con velOverride: [p+0x1C..0x24] = copia   si no: rA, rB (ángulos), tabla de senos 0xBB3E00, dir.x > 10 ? pos de la partícula : dirección normalizada (0x7ED9B0),
  RwV3dTransformVectors(0x7EDDF0), RotateVecIntoVec(0x4A1660), rC: k = ((rC * 0.0002f - 1.0f) * speedBias) + speed ; vel = float(rot * k)
vel[i] = float(velAdd[i] + vel[i]) ; UpdateParticle(timeSince, p) (0x4A21D0) ; m_PrimBP->m_Particles.AddItem(p) (0x4A8DF0) ; devuelve p
```
Constantes del EXE leídas del fichero: 0.0001f (0x858FC4), 0.0002f (0x85A7D0), 1.0f (0x858624), 255.0f (0x859AAC), 100.0f (0x858628), 6.28318f (0x85A7C0 = 0x40C90FD0), 0.0174533f (0x85A7BC), 40.7437f (0x85A778), 64.0f (0x859A44), 10.0f (0x85862C), 0.0 (0x858B50), qword 1.0 (0x85A310), qword (double)0.001f (0x85A7C8).

## Revisión propia contra el EXE (FASE 2)

Se escribió desde el desensamblado (`dis.sh 0x4A2580 0x4A2B1A`; tablas de pila trazadas a mano: locales en `[esp+10..+0x44]`, argumentos en `[esp+0x48..]`) y se contrastó de tres formas:

1. **Línea a línea con el desensamblado** (pila x87 incluida: qué valores se guardan como `float` con `fstp` y cuáles se quedan en registro con mantisa de 24 bits y exponente extendido: z de la posición, escala de la esfera, `maxRad`, `prodA/prodB`; qué constantes son `float` y cuáles `double`).
2. **Ejecución de los bytes reales del EXE** (`offline/`, sin juego): `gen_exe.py` extrae de `gta-sa.exe` los 1434 bytes de CreateParticle, `_ftol2` y `AddItem` con sus reubicaciones; `cph30.exe` los ejecuta frente al **mismo texto de FxEmitter.cpp** (`extract.py`) con `X87Math.cpp` compilado con las opciones de producción, con los mismos colaboradores sustituidos de `Fixtures.hpp`, y compara memoria completa, traza, EAX, ABI (ESP, EBX/ESI/EDI/EBP), x87 normalizado y cuántos `rand` se consumieron. Resultado: **0 diferencias en las 4096 fixtures y en 2.000.000 de casos aleatorios** (palabras de 32 bits crudas, NaN silenciosos) con el cuerpo compilado en `/O1`; 500.000 en `/Od` (como en el proyecto) y 300.000 en `/O2`. Un mutante deliberado (la velocidad usa `m_fSpeedBias` en lugar de `m_Speed` y se añade un término espurio) da 261 diferencias en 512 casos: el arnés discrimina.
3. **Oráculo en Python independiente** (`fixture_oracle.py`, desde el desensamblado): contrastado con el lado EXE del arnés, 4096/4096 sin diferencias (sólo 3 casos con comodín por empate de NaN); las fixtures de `Fixtures.hpp` compiladas con `cl` y las del espejo en Python son idénticas campo a campo (`offline/cmp_o.py`).

Además, `offline/livetest.exe` ejecuta **el mismo `Core.hpp` que se compila en la sonda** (con el original = bytes del EXE reubicados y el candidato = el C++) y escribe un `.jsonl` con el formato del banco; `offline/run_local_check.py` pasa `run_local.py::analyze` sobre él: 4096 pares aceptados, cobertura según lo calibrado, y rechazo comprobado de un registro corrompido en ambos lados. La sintaxis de `FxEmitter.cpp`, `Core.hpp` y `LiveComparison.hpp` se comprobó con `cl /Zs` y las opciones del proyecto (sin errores).

Discrepancias halladas y corregidas:

- **Borrador de HEAD** (`FxEmitter.cpp`, nunca compilado): `m_fCurrentLife`/`m_fTotalLife` y la vida usaban `GetRandomNumber` de la DLL y `/5000 - 1` en lugar de `rand%10000 * 0.0002f - 1.0f`; rotación, posición y velocidad con `GetRandomNumberInRange` (otro rand), `Magnitude()`, `lerp`, `GetSinFast` (que no es la lectura de tabla con `_ftol2`), `DegreesToRadians` (constante distinta), `vec.Normalise()` en lugar de `RwV3dNormalize` del EXE; `approxEqual(radio, 0, 0.001)` en lugar de `fabs(radio) < (double)0.001f` con `fcomp`; `emissionInfo.m_Direction.x <= 10.0f` (con NaN el EXE normaliza la dirección, no usa la posición); sin la rama «sin hueco» (`m_MustCreateParticles` + `FreeUpParticle` + segundo `GetParticle`); sin comprobar el segundo `GetParticle`; la partícula se creaba sin comprobar `nullptr`.
- Entre la versión final y los bytes del EXE no se halló ninguna otra discrepancia en el arnés (aparte de los sNaN descritos en los riesgos).

Límites de la revisión: **no se compiló el proyecto completo** (sólo `cl /Zs` de la fuente y del banco, y la compilación independiente de `X87Math.cpp` con las opciones de producción); el objeto real de `X87Math.cpp` se desensambló una vez (`dumpbin /disasm`) y se vio que el compilador conserva el orden de operaciones (almacena/recarga `float` donde se pidió, `double` en registros con spill exacto) y llama a `__ftol2` del CRT para los índices.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Arena** (0x2F0 bytes con 16 de guarda por lado: emisor, prim con lista, sistema, `EmissionInfo_t`, `FxPrtMult_c`, matriz de trabajo, velocidad impuesta, partículas A y B, elemento previo de la lista) completa, en bits; guardas intactas.
- **Traza de colaboradores** (siete stubs, EntryPatch en la dirección del EXE y en la entrada de la DLL con los mismos cuerpos): `GetParticle` (0x4A93C0), `FreeUpParticle` (0x4A9400), `GetRWMatrix` (0x4A9DC0), `RwV3dNormalize` (0x7ED9B0), `RwV3dTransformVectors` (0x7EDDF0), `RotateVecIntoVec` (0x4A1660), `UpdateParticle` (0x4A21D0): tipo, desplazamientos en el arena (`0xFFFFFFFE` si es un local de pila, `0xFFFFFFFF` si es nulo), hashes FNV de las estructuras y bits de los floats. El pool lo guionan `poolMode`/`mustCreate`: partícula A, null (con y sin `FreeUpParticle`), o B tras `FreeUpParticle`.
- **Partícula devuelta** (EAX como desplazamiento) y **cuántas tiradas de rand consumió** cada lado (la siguiente tirada, tras fijar la semilla del hilo del EXE: `0x827B3D + 0x14`, que se restaura al final).
- **ABI**: ESP (`ret 1Ch`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW completa 24 bits/redondeo al par/máscaras, TOP, etiquetas, ningún ST válido al salir), MXCSR sin banderas (`& 0xFFC0`). `EAX/ECX/EDX`: EAX se compara como `ret`; ECX/EDX sólo se registran (`callers-residue.txt`: ningún llamador del EXE los lee).
- **Rutas**: original -> el hook de 0x4A2580 NO salta a la DLL; candidato -> sí, con el cuerpo en el ASI. Se vigilan 16 bytes de la entrada, del cuerpo y de los 14 puntos de colaboradores (`globals_restored`).
- **Oráculo** (`fixture_oracle.py`, desde el desensamblado, x87 a 24 bits con `Fraction`): arena completa, traza, partícula devuelta y `rand_next` iguales a los de cada lado. Comodín (`None`) sólo donde un empate de NaN con signos opuestos es indeterminado: ahí se exige EXE == DLL.
- **Fixture == oráculo**: la fila `input` registrada debe ser exactamente `make(caso)` del espejo Python.

## Fixtures (`Fixtures.hpp`; espejo en `fixture_oracle.py`): 4096 casos, 8 familias × 512 (familia = n % 8)

Cada campo float (26 palabras de `EmissionInfo_t`, 7 de `FxPrtMult_c`, 16 de la matriz, 12 de `GetRWMatrix`, velocidad impuesta, `m_VelAdd`, `timeSince`, `brightness`) sale de `Val(caso, clave, clase)`: valor natural de su clase (exponentes acotados, colores/escalas/vidas casi siempre positivos), o uno de 40 especiales (±0, desnormales, ±inf, NaN silenciosos con cargas distintas, 0.001f, 255, 127.5...), o un exponente extremo (1..24 / 231..254); familias 4 (muchos especiales), 5 (muchos extremos), 6 (mezcla), 7 (algo de ambos) y 0-3 benignas. Además: radio en 8 clases (0, ±0.0005, 0.001f exacto, 0.001f − 1 ulp, positivo, negativo, especial, −0.001f), `dir.x` en 6 clases (normal, > 10, == 10, NaN, ±inf, grande), `hasVel` (1/4), `createLocal` (1/3), `mustCreate` (1/2), `poolMode` (70 % A, 15 % null+B, 15 % null), lista vacía o con un elemento, relleno del arena (4 patrones) y semilla del rand (4 de cada 16 casos fuerzan que la primera tirada sea 0 o 9999).

Cobertura calibrada con el modelo sobre las 4096 fixtures (reales entre paréntesis; mínimos en `run_local.py` con ~40 % de margen): partícula (3151), caja (1533), esfera con radio > 0 (906) y < 0 (712), dirección normalizada (1317) y desde la posición (1021), velocidad impuesta (813), local (1380), lista no vacía (2044), `FreeUpParticle` (298), sin partícula (945), primera tirada 0 (198) y 9999 (196), salida finita (1872), con infinitos (770), con NaN (682); casos con comodín: 3.

## Suposiciones y riesgos del banco

- **sNaN no cubierto**: las fixtures sólo generan NaN silenciosos. Con senalizantes (bit 22 a 0) la carga útil del resultado depende de si el NaN entra en una instrucción x87 por memoria o por registro; en 200.000 casos aleatorios con palabras crudas hubo 21 diferencias, todas las inspeccionadas de carga útil de un NaN (p. ej. el EXE devuelve el QNaN del otro operando y la DLL el sNaN silenciado). El juego no produce sNaN.
- **Empates de NaN** (mismo significando, signo opuesto): el oráculo deja comodín y sólo se exige EXE == DLL; si el compilador conmutara operandos de una suma/producto podría salir una diferencia real en esos 3 casos.
- **`createLocal` canónico**: el banco pasa 0 o 1. La DLL guarda el byte recibido tal cual (`[p+0x37]`), como el EXE; un llamador que pasara otro valor no está probado.
- **`_ftol2` del CRT de la DLL** frente al del EXE: iguales en las 4096 + millones del arnés (incl. NaN, ±inf, > 2^63, negativos); en el juego el CRT es el de la DLL con el mismo resultado de 64 bits truncado.
- La tabla de senos (0xBB3E00) se registra en el evento `begin` (`sin_table`) y el oráculo la usa; el banco no la modifica.
- Los colaboradores se sustituyen porque `FreeUpParticle` usa el `rand` de la DLL y `UpdateParticle` depende de blueprints; **probar un consumidor no valida a sus colaboradores**.
- `Dependencies22C` (CreateParticles) debe revalidarse tras este cambio (ver «Fuente»).
- Si el compilador del proyecto en modo Release inlineara `GetParticle` o `UpdateParticle` en `CreateParticle`, los parches de entrada no los interceptarían y la traza esperada del oráculo fallaría (aviso claro, no falso positivo).

## Evidencia sin juego (`offline/`)

```
cd reversing/FxEmitterCreateParticle30/offline
python -I gen_exe.py && python -I extract.py ../../../source/game_sa/Fx/FxEmitter.cpp      # exebytes.inc, cp_rand.inc, cp_body.inc, add_body.inc (generados, no versionados)
./buildh.sh cph30.cpp                       # OPT=/Od ./buildh.sh cph30.cpp para el modo Debug
./cph30.exe 4096 6                          # fixtures: EXE reubicado frente al C++      (modo r: aleatorias;  CPH_SNAN=1 las deja con senalizantes)
python -I cmp_o.py                          # fixtures C++ == espejo Python y oráculo == lado EXE
EXTRA_INC="<ruta de nlohmann/include>" ./buildh.sh livetest.cpp && ./livetest.exe live_test.jsonl 4096 && python -I run_local_check.py live_test.jsonl
```
Requieren capstone (`python -I -c "import capstone"`), el EXE canónico `D:\Grand Theft Auto San Andreas\gta-sa.exe` y MSVC 14.51 (rutas en `buildh.sh`).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py`, `source.patch`, `callers-residue.txt`, `offline/` (`gen_exe.py`, `extract.py`, `buildh.sh`, `cph30.cpp`, `cmp_o.py`, `livetest.cpp`, `run_local_check.py`), `README.md`.
