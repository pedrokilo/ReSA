<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco FxInterp61: FxInterpInfo{Float,U255,32,255}_c::GetVal(float* outValues, float delta) (0x4A8470, 0x4A8800, 0x4A89C0, 0x4A8B80)

**Estado: APROBADO en el juego** (corrida `20261019-220613-349774`: 16384 pares, 0 diferencias). Worktree `D:\Original\wd-5` (HEAD d349867fb). Plantilla de estructura: `reversing/QuatGet28`; contrato: `reversing/_handoff/BRIEF.md`, `BRIEF28.md` y la adenda `BRIEF60.md`.

## Comandos

```
bash reversing/_handoff/bench_here.sh FxInterp61          # compila la sonda + corre en el juego (candado global) + resume
python -B reversing/FxInterp61/build_probe.py             # solo la sonda
python -B reversing/FxInterp61/run_local.py               # solo la corrida
python -B reversing/FxInterp61/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # solo analisis (None si el registro aun no existe o no termino)
python -I reversing/FxInterp61/fixture_oracle.py          # autoprueba del oraculo (sin juego; imprime «ok»)
python -I reversing/FxInterp61/host_check.py [dirTemporal]  # cuerpo C++ REAL de las 4 GetVal + oraculo, SIN el juego (cl x86, /O2 /Oi /arch:IA32 /fp:fast, x87 a 24 bits): 0 diferencias
python -B reversing/FxInterp61/register_hook.py <carpetaEvidencia>   # solo lo ejecuta el coordinador (SCOPE_NOTE ya relleno)
```
Archivo de arranque en el clon: `fxinterp61-runtime.start`; salida `fxinterp61-runtime.jsonl`. Requiere en el ASI: `RH_ScopedOverloadedInstall(GetVal, "values", 0x4A8470, ...)` (Float, nombre `GetVal-values`) y `RH_ScopedInstall(GetVal, 0x4A8800 / 0x4A89C0 / 0x4A8B80)` (categoría `Fx`, clases `FxInterpInfoFloat_c`, `FxInterpInfoU255_c`, `FxInterpInfo32_c`, `FxInterpInfo255_c`), registrados en `InjectHooksMain.cpp`.

## No son virtuales

El encargo las daba como virtuales (fila `0x00000000`, tipo `V`). **No lo son**: la tabla virtual de cada clase (0x85A970, 0x85A978, 0x85A980, 0x85A988; dos entradas: destructor escalar y `Load` 0x5C16F0/0x5C18F0/0x5C1B10/0x5C1D30) no contiene `GetVal`; ninguna palabra del EXE contiene 0x4A8470/0x4A8800/0x4A89C0/0x4A8B80 y las llamadas son `call rel32` directas desde 0x4A4B80..0x4A71D5 (8 + 6 + 13 + 1 = 28 llamadores, en 0x4A4B80..0x4A71D5, la zona de los `FxInfo*_c::GetValue`). Por eso el hook es `Simple` (tipo `S`) con la dirección real, y la fila de `docs/hooks.csv` que debe añadir el coordinador es `FxInterpInfoFloat_c,GetVal-values,0x004A8470,1,0,1,S` (y `GetVal` para las otras tres).

## Qué hace la función (desensamblado 0x4A8470..0x4A85B7, 0x4A8800..0x4A8988, 0x4A89C0..0x4A8B48, 0x4A8B80..0x4A8D08; `ret 8`)

`void GetVal(float* outValues, float delta)`, thiscall (ECX = this; vptr +0, m_bLooped +4, m_nNumKeys +5 (int8), m_nCount +6 (int16), m_pTimes +8, m_Keys +0xC). Marco de 4 pushes (ecx, ebx, esi, edi), sin x87 residual, sin tocar FCW.
1. `n = (int8)m_nNumKeys`. Si `n == 1`: `outValues[i] = clave[i][0]` para `i < (int8)m_nCount`. **m_nCount se usa sólo como el byte bajo con signo** (`mov cl,[esi+6]; test cl,cl; jle` y `movsx edx,byte ptr [esi+6]`): un `m_nCount` de 0x0103 vale 3 y uno de 0x01FF no escribe nada. Float copia en bits (`mov`), las demás cargan `fild word` · escala y guardan con `fstp`.
2. Si el byte `m_bLooped` ≠ 0 (cualquier valor, no sólo 1): `total = (uint16)times[n-1] * 1/256` (0x859AA0 = 0.00390625); `q = delta / total` (x87); `entero = _ftol2(q)` (0x821B40: trunca; NaN, ±inf y |q| ≥ 2^63 dan 0; si no, **los 32 bits bajos** del truncado a 64 bits, p. ej. 3e9 → −1294967296); `delta = float(delta − entero · total)` (el producto con `fild`/`fmul` sin redondear el entero a float).
3. Búsqueda: primer `i` en `[1, n)` con `delta < times[i] · 1/256` (`fcomp` + `test ah,5; jnp`: comparación ordenada, NaN no corta; `delta == tiempo` no corta). Si no hay (o `n ≤ 1`, incluidos 0 y negativos) copia la clave `n−1` (índice −1 con n = 0).
4. Si hay: `A = times[i−1]/256`, `B = times[i]/256`, `t = float((delta − A) / (B − A))` (guardado como float en la pila), y `outValues[j] = float((clave[j][i] − clave[j][i−1]) · t + clave[j][i−1])`.
5. Float: claves `float` (la interpolación usa `fld clave[i]; fsub clave[i-1]; fmul t; fadd clave[i-1]` con operandos de **memoria**). U255: `uint16 · 1/256`; 32: `int16 · 0.001f` (0x858CDC = 0.0010000000474974513); 255: `int16 · 1/128` (0x858B88 = 0.0078125). En las enteras `a = fild(prev)·escala`, `b = fild(cur)·escala`, `(b − a)·t + a`.
Todo se evalúa con la palabra de control a 24 bits de mantisa y **exponente extendido**: (clave − anterior) puede superar FLT_MAX y volver a entrar en rango tras `· t` (el banco lo cubre: 128 casos Float con `ext_exponent`).

## Fuente C++ (`source.patch`, aplicable con `git apply` sobre HEAD; verificado con `git apply --check` contra `git show HEAD:<archivo>`)

- `FxInterpInfo.h`: plantilla protegida `FxInterpInfo_c::InterpolateKeys<TKey>(keys, outValues, delta, scale)` (cuerpo común de las cuatro; el EXE tiene cuatro copias idénticas salvo el tipo de clave y la escala) y `TruncateLow32` (el truncado de `_ftol2` en C++ puro).
- Las cuatro clases: `GetVal` llama a `InterpolateKeys(m_Keys, outValues, delta, escala)`; `static void InjectHooks()` (declaración en el `.h`) e inclusión en `InjectHooksMain.cpp` (lambda `Fx`). El borrador del lote 27 de `FxInterpInfoFloat.cpp` (un `return plugin::CallMethod` seguido de un parser muerto) se sustituyó: tenía `delta − (delta/total)·total` (debe ser `delta − trunc(delta/total)·total`, con `_ftol2`), no releía m_nCount como byte con signo y mezclaba tipos de índice.
- `source/CMakeLists.txt`: las cuatro `.cpp` de clase se compilan con `/O2 /Oi /arch:IA32 /fp:fast`, sin PCH ni unity (x87 como el EXE). Efecto lateral: `Load`/ctor de esas clases también salen con esas opciones (no tienen banco propio).
- Aritmética: los valores en registro del EXE (tiempos, `total`, cociente, claves enteras, `t`) son `double` en C++ (con la palabra de control a 24 bits cada operación se redondea a 24 bits y se conserva el exponente extendido; `double` tiene rango de sobra para cualquier cadena de floats); sólo se guardan como `float` (`volatile float`) el tiempo plegado y `t`, y cada salida. Las copias de claves float son `memcpy` de 4 bytes. **Float interpola con una expresión `float` pura** (`(k[i] - k[i-1]) * t + k[i-1]`) para que el compilador emita los mismos operandos de memoria que el EXE (ver «Defecto real hallado»).

## Revisión propia contra el EXE (fase 2)

Se desensambló de nuevo el objeto compilado (`dumpbin /disasm` de `FxInterpInfoFloat.obj` y `FxInterpInfo32.obj`) y se contrastó línea a línea: pila de 4 registros guardados (ebx, esi, edi; ECX/EAX/EDX libres, ESP `ret 10h` con el marco de ebp de la DLL), orden `fld total; fdivr; ftol; fild; fmul; fsubr` del plegado, bucle de búsqueda `fild; fmul; fld delta; fcomp; test ah,5; jnp`, `t = (delta−A)/(B−A)` con la misma pila x87 (`fsub; fxch; fsub; fdivrp`), y el bucle de interpolación (`fld cur; fsub [prev]; fmul [t]; fadd [prev]; fstp` en Float; `fild`·escala en las enteras). **Discrepancias halladas y corregidas:** (1) borrador: `(delta/total)·total` en lugar de truncar; (2) borrador: m_nCount como 16 bits; (3) primera versión con claves en `double`: con claves NaN señalizadoras el x87 elige el NaN de forma distinta (ver abajo).

## Defecto real hallado y corregido por el banco

Primera corrida (20261019-215548): 14 diferencias, todas en Float y todas con **NaN señalizadores en la tabla de claves**. Causa: con un operando de memoria que es sNaN y el otro operando en registro un QNaN, el x87 elige el QNaN (tabla 4-7 del manual de Intel: «SNaN and QNaN → QNaN source operand»); con ambos en registro (primera versión: `double` + `fld` de las dos claves) el sNaN ya está silenciado y gana el de mayor significando. Se reescribió la rama Float con una expresión `float` pura (mismas instrucciones que el EXE, operandos de memoria) y el oráculo modela el operando de memoria señalizador. Es un rincón que los datos reales no producen (las claves vienen de un fichero de texto) pero es una diferencia observable y quedó cerrada.

## Qué se compara (por lado, en bits; original == candidato == oráculo)

- **Zona de salida** (8 floats `[0x640, 0x660)`; sólo las `(int8)m_nCount` primeras ranuras cambian) y que **todo el resto de la arena** (guarda previa, objeto con sus punteros, relleno, tiempos, filas de claves, relleno con lecturas fuera de tabla, tabla de punteros, guardas finales) quede idéntico (`guard_ok`).
- ABI: ESP (`ret 8`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW completa, TOP, tags y ST válidos: no debe quedar ninguno; se ignoran banderas FSW, FIP/FDP y registros vacíos; MXCSR sin banderas), FCW fijada a 24 bits como en el juego.
- **EAX/ECX/EDX de salida: sólo se registran** (`original_residue`, `candidate_residue`; `kCompareVolatileRegisters=false`). `python -I reversing/Dependencies25A/residue_scan.py 0x4A8470 / 0x4A8800 / 0x4A89C0 / 0x4A8B80` sobre los 28 llamadores no devuelve ninguna lectura real: el único marcado (0x4A4B80, `ecx`) es un `push ecx` que reserva un hueco que se sobrescribe con `fstp [esp]` antes de usarse; los demás avisos son EDX/ECX «sin escribir» que entran como argumentos de otra llamada (0x4A85C0), que no los lee. **Ningún llamador del EXE lee EAX/ECX/EDX tras estas funciones.**
- Rutas (`route_ok`): original → el hook de cada entrada NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI. Se vigilan 16 bytes de cada entrada del EXE, de cada cuerpo de la DLL y de `_ftol2` (0x821B40, lo llama el EXE; la DLL no lo usa).

## Colaboradores

Ninguno sustituido. El EXE llama a `_ftol2` (CRT, 0x821B40) en las cuatro; la DLL no (reproduce el truncado con enteros). Los llamadores directos (`FxInfoEmSpeed_c::GetValue`, `FxInfoEmSize_c`, `FxInfoColour_c`…) no se ejercitan aquí.

## Fixtures (`Fixtures.hpp`): 4 funciones × 4096 casos = 16384 pares; familia = n % 16 (256 casos por familia y función)

Arena única de 0x700 bytes (`kArena`), construida igual en el juego y en el volcado (`BuildArena`): `0x000` guarda | `0x040` objeto | `0x090` tiempos (16 × uint16) | `0x100` 8 filas de claves (0x40 cada una: 0x10 de relleno + 0x30 de claves) | `0x600` guarda | `0x640` salida (8 floats) | `0x6C0` tabla de punteros a filas (fuera del alcance de las lecturas con índices grandes, para que ningún puntero se lea como dato) — con relleno aleatorio / 0x00 / 0xC3 en el resto. Los punteros se registran como desplazamientos; la arena de cada caso va completa en `input.arena`.
Familias: 0 una clave; 1 sin bucle dentro de rango; 2 sin bucle sobre tiempos / antes del primero / pasado el último; 3 con bucle (incluye bytes `m_bLooped` ≠ 0/1); 4 con bucle y cocientes extremos (1e9..FLT_MAX, 2^31..2^63) y total 0 / diminuto; 5 delta NaN (silencioso/señalizador), ±inf, ±0, subnormales; 6 tiempos desordenados / iguales / 0xFFFF; 7 `m_nNumKeys` 0, −1..−8 y 13..127 (lee fuera de tabla, dentro de la arena); 8 `m_nCount` con bits altos distintos de cero y byte bajo con signo; 9 claves extremas (±FLT_MAX, enteros de 16 bits extremos: (clave − anterior) desborda float); 10 claves inf/NaN (Float, incl. sNaN) o enteros extremos; 11 delta a ±3 ulp de un tiempo de clave; 12 datos típicos de efectos; 13-15 mezclas.
**Reglas de seguridad:** `m_nCount` efectivo ≤ 8 (≤ 127 de índices de clave leídos dentro de la arena), nunca negativo grande en `numKeys` (≥ −8).

## Oráculo (`fixture_oracle.py`)

Modelado desde el desensamblado (no del C++): Fractions exactas con redondeo a 24 bits (mitad al par, exponente extendido) en cada operación x87, `fstp dword` con subnormales graduales y desbordes, NaN según la tabla 4-7 (operando de memoria sNaN vs registro QNaN; mayor significando; empates de signo opuesto = comodín `?`: ahí sólo se exige EXE == DLL), `_ftol2` como truncado a 64 bits y 32 bits bajos. Autoprueba (`python -I fixture_oracle.py`): 4000 pares de floats contra `struct.pack('<f', a op b)` para `+ − × ÷` (redondeo a 24 bits), redondeo mitad-al-par y subnormales, y 20 casos manuales (copias en bits incl. sNaN, interpolación, bucle, extrapolación, NaN, cociente enorme, total 0, exponente extendido, 0·inf, escalas U255/32/255, `m_nCount` de 16 bits, `numKeys` 0, regla de NaN con operando de memoria).

## Comprobaciones sin el juego (hechas)

`host_check.py` + `host_check.cpp` + `fixture_dump.cpp` (+ `host_stub/FxTools.h`): compila el CUERPO REAL de las cuatro `GetVal` (extraído de los `.cpp` de producción) y la plantilla real de `FxInterpInfo.h` con cl x86 `/O2 /Oi /arch:IA32 /fp:fast`, con la palabra de control a 24 bits, sobre las 16384 fixtures volcadas de `Fixtures.hpp` (autoprueba cruzada: el volcado compilado por cl es el mismo que genera el juego, las coberturas coinciden exactamente): **0 diferencias frente al oráculo**. Mordida comprobada con mutaciones de la plantilla: `<` → `<=` en la búsqueda (140 casos distintos), rango de `_ftol2` 63 → 31 (58), `m_bLooped == 1` en vez de ≠ 0 (221), `m_nCount` de 16 bits (el ejecutable se cae por escrituras fuera de la arena).

## Suposiciones y riesgos del banco

- Los NaN señalizadores de las claves sólo entran en la rama Float (`snan_copy`/aritmética); en las enteras no hay NaN de entrada. Empates de NaN con signos opuestos: sólo EXE == DLL (6 casos en Float).
- El EXE deja la FCW como la encuentra: se fija a 24 bits (como el juego) antes de cada llamada, en ambos lados.
- Las funciones guardan `t`/`delta` en la ranura del primer argumento ya consumido (como el EXE: `fstp [esp+18h]`); ese hueco de pila es del llamador y se descarta con `ret 8`. No se compara.
- Efecto lateral de las opciones de compilación por archivo: `Load`, constructores y `Allocate` de las cuatro clases se compilan ahora con x87/`/O2` (no están en el catálogo con banco propio; el cambio no altera su semántica salvo en el redondeo de la conversión float→entero de `Load` si algún valor se sale de rango, que no es el caso de los datos del juego).

## Resultado

**APROBADO** (19/10/2026, corrida `20261019-220613-349774`): 16384 pares (4 funciones × 4096), 0 diferencias EXE/DLL, salida de ambos lados igual a la del oráculo en bits, cobertura sobre los mínimos de `run_local.py`, estado, entradas vigiladas y x87 restaurados. Informe: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-fxinterp6161019-220613-349774`. Corridas previas (sólo nota): `20261019-215548-734922` falló con 14 diferencias en Float por NaN señalizadores en las claves (defecto real de la primera versión de la fuente, corregido); `20261019-215958-064546` pasó todas las comparaciones pero la cobertura mínima `single` estaba mal calibrada en `run_local.py` (3500 en vez de 1200; sin cambio de fuente).
