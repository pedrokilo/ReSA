<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco QuatEulerSet29: CQuaternion::Set(float x, float y, float z) (0x59C530)

**Estado: APROBADO y registrado (19/10/2026, corrida 20261019-004919-715372, 4096 pares, EXE == DLL bit a bit; informe en resultados-quateulerset29\20261019-004919-715372).** Antes: preparado. Plantilla: `reversing/QuatGetAxis29` (misma clase y mismas fuentes `Quaternion.cpp` / `X87Math`). Fuente escrita en el worktree `D:\Original\wq-q3` (HEAD 98286d43c).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh QuatEulerSet29          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/QuatEulerSet29/build_probe.py
python -B reversing/QuatEulerSet29/run_local.py
python -B reversing/QuatEulerSet29/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/QuatEulerSet29/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -I reversing/QuatEulerSet29/fixture_mirror.py                                              # cobertura de las 4096 fixtures según el oráculo (sin juego)
python -B reversing/QuatEulerSet29/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedOverloadedInstall(Set, "euler", 0x59C530, void (CQuaternion::*)(float, float, float))` (hook «Set-euler», clase `CQuaternion`). Aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `quateulerset29-runtime.start`; salida `quateulerset29-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CQuaternion,Set-euler,0x0059C530,1,0,1,S` (la añade el coordinador al registrar) y hay que retirar de `docs/implementaciones/envolturas-pendientes.csv` la línea de `CQuaternion,Set,0x0059C530` si existe.

## Fuente (FASE 1)

`source.patch` (git diff unificado contra HEAD, sólo esta función; verificado con `git apply --check` contra `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Core/Quaternion.cpp`: `CQuaternion::Set(float,float,float)` pasa a `QuaternionFromEulerCpp(&this->x, x, y, z)` y se activa el hook `RH_ScopedOverloadedInstall(Set, "euler", 0x59C530, ...)`.
- `source/game_sa/X87Math.h` / `X87Math.cpp`: `QuaternionFromEulerCpp(float* q, float ax, float ay, float az)` y los ayudantes privados `QuatEulerSin` / `QuatEulerCos`, en un bloque propio al final de ambos archivos (prefijo `QuatEuler`). El archivo se compila con `/O2 /Oi /arch:IA32 /fp:fast` (x87, `sin`/`cos` en línea).

Qué hace el EXE (0x59C530..0x59C5EF, `ret 0Ch`, thiscall, ECX = this; argumentos x, y, z en [esp+4], [esp+8], [esp+0Ch]; `0x858B8C` = 0,5f):

```
hx = x*0.5 ; hy = y*0.5 ; hz = z*0.5 (24 bits)       cx = cos(hx), cy = cos(hy), cz = cos(hz) (fcos sin redondear)
fstp float: cz ; sx = sin(hx), sy = sin(hy), sz = sin(hz) (fsin sin redondear y guardados como float)
cycx = f(cy*cx)  sysx = f(sy*sx)  sxcy = f(sx*cy)     (cy, cx sin redondear; f = 24 bits y fstp float)   cxsy = r24(cx*sy) (se queda en registro)
w = f( r24(sysx*sz) + r24(cycx*cz) )   -> [ecx+0Ch]   (escrito primero)
x = f( r24(cycx*sz) - r24(sysx*cz) )   -> [ecx]
y = f( r24(sz*cxsy) + r24(sxcy*cz) )   -> [ecx+4]
z = f( r24(cxsy*cz) - r24(sxcy*sz) )   -> [ecx+8]
```
Nótese la convención del EXE (la componente `x` del cuaternión sale del ángulo `z`, la `y` del `x`...): se reproduce tal cual. Los argumentos son por valor: no hay solapamiento posible con `this`. Los valores que el EXE guarda como float son `volatile float` (un redondeo a float por `fstp`); el resto va en `double` con un redondeo a 24 bits por la palabra de control. `|h| >= 2^63` deja a fsin/fcos el operando intacto (C2 = 1); el ayudante lo hace explícito para no depender de que el compilador no llame a la CRT.

## Revisión propia contra el EXE (FASE 2)

Segundo desensamblado de `0x59C530..0x59C5F0` contrastado línea a línea con el C++ (se rastreó la pila x87 completa: ST0..ST5 tras los tres `fcos`, cada `fxch` y cada `fstp` a [esp+10h/18h/0/14h/4/8]):

- Orden de las operaciones, qué pasa por memoria float (cz, sx, sy, sz, cycx, sysx, sxcy) y qué queda en registro sin redondear (cx, cy, cxsy y los ocho productos de la suma final): **coinciden**. Orden de escritura (w, x, y, z): **coincide**.
- Discrepancias halladas: ninguna en el orden. Una corrección durante el estudio: el borrador mental inicial suponía `x = sen(ax/2)...`; el desensamblado dice que la componente `y` lleva `sen(ax/2)` y la `x` lleva `sen(az/2)`; el ayudante y el oráculo siguen el desensamblado.
- **Llamadores del EXE**: ninguno. `residue_scan.py` sobre 0x59C530 no devuelve ningún `call`/`jmp` directo y una búsqueda de la dirección como dword y como `E8/E9 rel32` en todas las secciones del EXE tampoco. Ningún llamador lee EAX/ECX/EDX: sólo se registran (`kCompareVolatileRegisters=false`). En la fuente nadie llama a `CQuaternion::Set(float,float,float)`.
- **Riesgo conocido, no comprobado (no se compiló):** las 6 cantidades `cx`, `cy` (sin redondear) y `hx/hy/hz` deben vivir en la pila x87 de la DLL sin derramarse a `double` de 64 bits; si el compilador derramara `cx` o `cy`, `cycx`, `sxcy` y `cxsy` diferirían en ~2^-53 relativos y habría diferencias muy raras (un empate de redondeo a 24 bits por cada 2^29 productos) — el banco las delataría como EXE != DLL. Si ocurre, volver a escribir el cuerpo con `FlcX87` o reordenar para acortar la vida de `cx`/`cy`. El coordinador debe leer el objeto de `X87Math.cpp` y comprobar `fcos`/`fsin` en línea (sin llamadas a la CRT) en `QuaternionFromEulerCpp`.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Arena completa** (0xC0 bytes: guarda 0x40, ventana 0x40 con el cuaternión en `qoff`, guarda 0x40; relleno previo aleatorio, 0x00, 0xFF o NaN silenciosos).
- Oráculo (`fixture_oracle.py`, desde el desensamblado): x87 de 24 bits con exponente extendido (`r24`), `fstp dword` al rango float32, NaN/inf/ceros con signo; fsin/fcos con `math.sin`/`math.cos` (doble precisión) y tolerancia de 2 ulp en los floats de salida; `|h| >= 2^63` deja el operando; `2^26 <= |h| < 2^63` es DESCONOCIDO (`??`, la reducción de 66 bits del hardware no se modela; sólo EXE == DLL); empates de NaN con signos opuestos, comodín. Autoprueba (ya ejecutada: ok): casos a mano (ángulos 0, x = pi, z = pi/2, NaN, inf, ángulo 2^64, cuaternión desalineado) y 3000 ternas aleatorias contra una cadena float32 por operación.
- ABI: ESP (`ret 0Ch`: `popBytes=0`), EBX/ESI/EDI/EBP, DF=0, x87 normalizado (FCW 24 bits, TOP, etiquetas, ST válidos; ninguno al salir), MXCSR sin banderas, hook/ruta, entradas de código vigiladas, estado x87 final.
- **EAX/ECX/EDX de salida: sólo se registran** (ningún llamador).

## Fixtures (`Fixtures.hpp`; espejo `fixture_mirror.py`): 4096 casos = 16 familias x 256 (familia = n % 16)

0 típicos (|a| hasta ~8); 1 pequeños (2^-40..2^-6); 2 subnormales y ±0; 3 notables (0, ±1, ±pi/4, ±pi/2, ±pi, ±2pi, ±4pi, subnormales, máximo float); 4 grandes (2^3..2^24); 5 enormes (2^25..2^127; cubre |a/2| >= 2^63); 6 NaN en algunos ángulos; 7 infinitos en algunos; 8 mezcla; 9 casi todos nulos; 10 bits crudos (nunca sNaN); 11 NaN con cargas útiles y signos distintos; 12 cuaternión desalineado (0..0x30 bytes); 13 desplazamiento alineado + mezcla; 14 frontera de fsin/fcos (2^64 ±1 ulp) y de la reducción fiable (2^26); 15 exponentes -10..30. Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`.

Cobertura calibrada con el espejo: all_known 3455, finite_in 3167, finite_out 2460, huge_in 672, mid_in 617, nan_in 594, nan_out 960, inf_in 362, inf_out 109, indefinite_out 426, negzero_out 229, subnormal_out 271, zero_in 659, zero_out 477, unaligned 192, unknown 641; mínimos en `run_local.py` con ~40-50 % de margen (más 256 exactos por familia).

## Suposiciones y riesgos del banco

- Ni el espejo ni el oráculo se han ejecutado contra una sonda real; `Fixtures.hpp` (C++) y el espejo deben producir las mismas filas `input` (el espejo se copió a mano del generador; no hay compilador disponible para cotejarlos). Si `run_local.py` rechaza por cobertura, comparar primero las filas `input`.
- La salida de la función sólo depende de los tres floats (no lee la ventana): los fixtures de ventana sólo vigilan que no se escriba fuera de los 16 bytes del cuaternión (guardas y bytes vecinos a `qoff`).
- Los ángulos con 2^27 <= |a| < 2^64 (`mid_in`, ~15 %) tienen oráculo desconocido: sólo EXE == DLL. Si el EXE y la DLL difieren ahí, la causa es fsin/fcos de la DLL (por ejemplo una llamada a la CRT en lugar de la instrucción).
- **sNaN no cubierto**; empates de NaN con signos opuestos (familias 6, 8, 10, 11): comodín `??`, se exige EXE == DLL.
- La función se llama con CW de 24 bits fijada por el banco (`_controlfp(_PC_24|_RC_NEAR)`).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
