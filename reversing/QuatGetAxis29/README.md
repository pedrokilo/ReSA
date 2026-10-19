# Banco QuatGetAxis29: CQuaternion::Get(RwV3d* axis, float* angle) (0x59C230)

**Estado: APROBADO y registrado (18/10/2026, corrida 20261018-222018-607411, 4096 pares, EXE == DLL bit a bit; oráculo independiente a 2 ulp; la corrida 20261018-221739-268232 se rechazó sólo por el oráculo y no es evidencia).** Antes: preparado. Plantilla: `reversing/QuatGet28` (misma clase y mismas fuentes `Quaternion.cpp` / `X87Math`).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh QuatGetAxis29          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/QuatGetAxis29/build_probe.py
python -B reversing/QuatGetAxis29/run_local.py
python -B reversing/QuatGetAxis29/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/QuatGetAxis29/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -I reversing/QuatGetAxis29/fixture_mirror.py                                              # cobertura de las 4096 fixtures según el oráculo (sin juego)
python -B reversing/QuatGetAxis29/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedOverloadedInstall(Get, "axis-angle", 0x59C230, void (CQuaternion::*)(RwV3d*, float*))` (hook «Get-axis-angle», clase `CQuaternion`). Aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `quatgetaxis29-runtime.start`; salida `quatgetaxis29-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CQuaternion,Get-axis-angle,0x0059C230,1,0,1,S` (la añade el coordinador al registrar) y hay que retirar la línea 411 de `docs/implementaciones/envolturas-pendientes.csv` (`CQuaternion,Get,0x0059C230`).

## Fuente (FASE 1)

`source.patch` (git diff unificado contra HEAD, sólo esta función; verificado con `git apply --check` sobre `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Core/Quaternion.cpp`: `CQuaternion::Get(RwV3d*, float*)` pasa a `QuaternionGetAxisAngleCpp(&x, &axis->x, angle)` y se activa el hook `RH_ScopedOverloadedInstall(Get, "axis-angle", 0x59C230, ...)`.
- `source/game_sa/X87Math.h` / `X87Math.cpp`: `QuaternionGetAxisAngleCpp(const float* q, float* axis, float* angle)` (junto a `RpNanQuatToMatrixCpp`) y el ayudante privado `ExtendedPiSinCpp`. No se duplica nada: usa la máquina `FlcX87` y `FlcCrtAcos` ya existentes (el `_CIacos` del EXE, con IE/errno del dominio).

Qué hace el EXE (desensamblado 0x59C230..0x59C266, `ret 8`, thiscall, ECX = this, [esp+4] = axis, [esp+8] = angle):

```
fld w ; fadd st,st                     X = 2w            (acos(2w), no 2*acos(w): así está en el EXE)
call _CIacos (0x822380)                A = acos(X)       (CRT del EXE: |X| > 1 o inf -> indefinido y errno = 33; NaN -> mismo NaN y errno = 33; X = +1 -> fldz; X = -1 -> fldpi de 64 bits)
fld st ; fstp [angle]                  *angle = float(A)                    <- PRIMERA escritura
fsin ; fdivr [0x858624 = 1.0f]         I = 1 / sin(A)    (24 bits; A y sin(A) sin pasar por float)
fld st ; fmul [q.x] ; fstp [axis.x]    ; fld st ; fmul [q.y] ; fstp [axis.y] ; fmul [q.z] ; fstp [axis.z]
```
Las lecturas de q.x/q.y/q.z ocurren después de las escrituras anteriores (angle, axis.x...), así que la superposición de punteros es observable: el ayudante conserva ese orden (cada `Fld/Fmul/Fstp` de `FlcX87` lee/escribe en el momento). No hay `__asm`, ni llamadas al EXE por la función: el `errno` lo fija `FlcMathTail` con `_errno()` del EXE (0x82B181) igual que el resto de usuarios de `FlcCrtAcos`.

**Salvedad resuelta (fldpi):** `FlcX87::Fsin` pasa su argumento a `double`; con X = -1 el EXE le da a `fsin` el pi de 64 bits (si no hay PE acumulado antes). El ayudante lo detecta (`sig == 0xC90FDAA22168C235`, `exp == 1`) y calcula el seno con el x87 real a precisión 64: `sin(piHi + piLo)` con `piHi = 0x1.921fb54442d18p+1` y `piLo = 0x1.1a8p-53` (= pi_64 - piHi exacto, comprobado con `Fraction`), todo en ST0 sin guardar (hay que confirmarlo en el objeto, ver límites). Si hay PE acumulado, `FlcMathTail` ya redondea pi a 53 bits y se usa el camino normal, igual que el EXE.

## Revisión propia contra el EXE (FASE 2)

Segundo desensamblado (`dis.sh 0x59C230 0x59C270`, `0x822380..0x82244A`, `0x828EA7`, `0x828F05..0x828F9B`, `0x83365F`) contrastado instrucción a instrucción con `QuaternionGetAxisAngleCpp`:

- Orden de operaciones, qué pasa por memoria float (sólo `*angle` y los tres ejes; A, sin(A) e I quedan en registro), destinos (`angle` se lee de `[esp+0xC]`, `axis` de `[esp+8]` tras el `push esi`): **coinciden**. `fadd st,st` = `Arith('+',0,0,0,false)`; `fdivr dword` = `Fdivr` (m / ST0); la pila x87 queda vacía al salir (`Commit`).
- Clasificación del CRT (0x8223A2..0x82241F): el `je` de 0x82239D viene del `cmp eax,7FF00000h` de 0x828F78 (exponente todo unos -> NaN/inf); `3FF00000h` exacto con mantisa 0 -> `fldz`/`fldpi`; mayor -> indefinido (0x8E3130); menor -> `fpatan(sqrt((1+x)(1-x)), x)` tras `fxch`: **coincide** con `FlcCrtAcos`. Cola 0x8223C8/0x82242C y 0x828F9B: con CW 0x7F (PE enmascarado) y PE acumulado -> `0x828EA7` (fstp qword + fld: 53 bits); dominio/NaN -> mismo camino con errno: **coincide** con `FlcMathTail`.
- Discrepancias: ninguna en el orden ni en las operaciones. Un punto no modelado por `FlcX87` y corregido en el ayudante: `fsin` de pi de 64 bits (arriba). No verificado: qué hace `0x83365F -> 0x82E53A` con la excepción «inexacta» (tipo 8) respecto de errno; `FlcMathTail` asume que no lo toca.
- **Llamadores del EXE**: ninguno. `residue_scan.py` sobre 0x59C230 no encuentra ningún `call`/`jmp` directo en `.text` (sí encuentra los de 0x59C080, comprobado como control) y una búsqueda de la dirección como dword en todo el EXE tampoco (0x59C160 igual). Por tanto ningún llamador lee EAX/ECX/EDX; sólo se registran (`kCompareVolatileRegisters=false`). En la fuente nadie llama a `CQuaternion::Get(RwV3d*, float*)`: el hook sólo se ejercita con el banco.
- Hermana `Get(float*,float*,float*)` (0x59C160, `ret 0Ch`): **sigue en el EXE** (envoltura en `Quaternion.cpp:49`), **no tiene entrada** ni en `envolturas-pendientes.csv` ni en `hooks.csv`, tampoco tiene llamadores ni referencias de datos en el EXE (código muerto), y llama a 0x59C080 (hoy C++ con hook; relee el resultado de su propia pila y recarga ECX desde `[esp+4Ch]`, no depende de EAX/ECX/EDX de 0x59C080). Fuera de alcance: lleva 3 `fpatan`, `fsin`/`fcos` y 3 comprobaciones `fcom`/`fadd [0x858CBCh]`; es factible con `FlcX87` en otro banco. La clase no queda «sin envolturas»: siguen `Multiply`, `Set(x,y,z)`, `Slerp(t)`, `Conjugate` y 0x59C160.

Límites: no se compiló, así que el objeto de `X87Math.cpp` no se ha leído; el coordinador debe comprobar que `ExtendedPiSinCpp` emite `fld [hi]; fadd [lo]; fsin` sin `fstp/fld` intermedios (si se materializara a double perdería los 64 bits y el caso 2w = -1 diferiría; el banco lo detecta con la familia 8, 256 casos).

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Arena completa** (0xC0 bytes: guarda 0x40, ventana 0x40 con cuaternion + eje + ángulo en los desplazamientos del caso, guarda 0x40; relleno previo aleatorio, 0x00, 0xFF o NaN silenciosos) y **errno del EXE** (centinela 0x5A5A5A5A antes de cada lado).
- Oráculo (`fixture_oracle.py`, desde el desensamblado): x87 de 24 bits con exponente extendido (`r24`), `fstp dword` al rango float32, NaN/inf/ceros con signo como el oráculo de QuatGet28; `_CIacos` modelado (NaN, dominio, ±1, camino normal con `s = sqrt(r24(r24(1+X)*r24(1-X)))` a 24 bits e `I = r24(sqrt(s^2+X^2)/s)`, ángulo `float(atan2(s,X))`). Simula la memoria en orden, de modo que la superposición cuaternion/eje/ángulo es exacta; los resultados que dependen de un valor desconocido se propagan como comodín `??`. Autoprueba (ya ejecutada: ok): casos a mano (w = 0, 0,25, 0,75, ±0,5, NaN, inf, -0, 2w mayor que el máximo float, aliasing) y 3000 cuaterniones aleatorios contra una cadena float32 por operación (`math.atan2`/`math.sin`).
- ABI: ESP (`ret 8`: `popBytes=0`), EBX/ESI/EDI/EBP, DF=0, x87 normalizado (FCW 24 bits, TOP, etiquetas, ST válidos; ninguno al salir), MXCSR sin banderas, hook/ruta, entradas de código vigiladas, estado x87 final.
- **EAX/ECX/EDX de salida: sólo se registran** (ningún llamador).

## Fixtures (`Fixtures.hpp`; espejo `fixture_mirror.py`): 4096 casos = 16 familias x 256 (familia = n % 16)

0 típicos (|2w| < 0,5); 1 |w| en [0,25, 0,5); 2 w en el borde del dominio (±0,5 exacto, un ulp dentro/fuera); 3 w cero/subnormal/diminuto con xyz especiales (acos = pi/2); 4 dominio inválido (|2w| >= 1, hasta 2^127); 5 w NaN o inf; 6 w normal y xyz con mezcla (NaN, inf, ±0, subnormales, enormes); 7 2w = +1 (acos = 0, I = +inf); 8 2w = -1 (fldpi de 64 bits; eje desconocido en el oráculo, sólo EXE == DLL); 9 |w| = 0,5 - k ulp (k <= 64); 10 eje superpuesto al cuaternion; 11 ángulo superpuesto al cuaternion; 12 ángulo superpuesto al eje; 13 desplazamientos arbitrarios; 14 bits crudos (nunca sNaN); 15 empates y cargas útiles de NaN. Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`.

Cobertura calibrada con el espejo (`fixture_mirror.py`): alias 914, all_known 3773, errno_domain 1028, finite_in 3100, finite_out 2548, indefinite_out 921, inf_in 409, inf_out 405, kind_domain 764, kind_minus1 290, kind_nan 264, kind_normal 2497, kind_plus1 281, nan_in 664, nan_out 1330, negzero_out 67, subnormal_out 140, unknown 323, zero_out 140; mínimos en `run_local.py` con ~40-50 % de margen (más 256 exactos por familia).

## Suposiciones y riesgos del banco

- Ni el espejo ni el oráculo se han ejecutado contra una sonda real; `Fixtures.hpp` (C++) y el espejo deben producir las mismas filas `input` (el espejo fija `matherr: 0`; el banco registra el valor real de [0xC9ABFC] y el oráculo lo usa). Si `run_local.py` rechaza por cobertura, comparar primero las filas `input`.
- **errno**: el oráculo exige 33 sólo en NaN/dominio (con [0xC9ABFC] == 0); en el resto no exige nada (EXE == DLL) por lo no verificado de arriba. Si en la corrida EXE y DLL difieren en errno, la causa está en `FlcMathTail`, no en el banco.
- **Tolerancia matemática del oráculo**: A y sin(A) se toman exactos (fpatan/fsin de 64 bits) y el CRT redondea A a 53 bits; la DLL pasa por `FlcX87` (fpatan/fsin sobre `double`). Una discrepancia real sólo es posible si 1/sin(A) cae a ~2^-29 de un empate de redondeo a 24 bits (probabilidad despreciable con 4096 casos), y entonces EXE != DLL lo delataría.
- **sNaN no cubierto**; empates de NaN con signos opuestos (familia 15 y 6/14): comodín `??`, se exige EXE == DLL.
- El resultado de 2w = -1 (familia 8, 256 casos) depende del hardware (`fsin` de pi de 64 bits) y de PE acumulado; el banco sólo exige EXE == DLL. El estado sticky de las banderas x87 al entrar es el mismo para ambos lados (se restaura antes de cada uno).
- La función se llama con CW de 24 bits fijada por el banco (`_controlfp(_PC_24|_RC_NEAR)`); `FlcX87` redondea por software a 24 bits, como el EXE con la CW del juego.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
