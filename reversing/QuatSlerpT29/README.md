<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco QuatSlerpT29: CQuaternion::Slerp(const CQuaternion& from, const CQuaternion& to, float t) (0x59C630)

**Estado: APROBADO y registrado (19/10/2026, corrida 20261019-005314-077172, 4096 pares, EXE == DLL bit a bit; informe en resultados-quatslerpt29\20261019-005314-077172).** Antes: preparado. Plantilla: `reversing/QuatGetAxis29` (misma clase y mismas fuentes `Quaternion.cpp` / `X87Math`). Worktree de trabajo: `D:\Original\wq-q4`.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh QuatSlerpT29          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/QuatSlerpT29/build_probe.py
python -B reversing/QuatSlerpT29/run_local.py
python -B reversing/QuatSlerpT29/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/QuatSlerpT29/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -I reversing/QuatSlerpT29/fixture_mirror.py                                              # cobertura de las 4096 fixtures según el oráculo (sin juego)
python -B reversing/QuatSlerpT29/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedOverloadedInstall(Slerp, "t", 0x59C630, void (CQuaternion::*)(const CQuaternion&, const CQuaternion&, float))` (hook «Slerp-t», clase `CQuaternion`). Aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `quatslerpt29-runtime.start`; salida `quatslerpt29-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CQuaternion,Slerp-t,0x0059C630,1,0,1,S` (la añade el coordinador al registrar; no existía fila de envoltura pendiente para 0x59C630 en `docs/implementaciones/*.csv`). `build_probe.py` vigila también `source/game_sa/Animation/AnimBlendNode.cpp` (hook de CalcTheta, que el banco apaga).

## Fuente (FASE 1)

`source.patch` (`git diff HEAD` de los tres archivos, verificado con `git apply --check` sobre `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Core/Quaternion.cpp`: `CQuaternion::Slerp(from, to, t)` pasa a `QuatSlerpTCpp(&x, &from.x, &to.x, t)` y se activa el hook `RH_ScopedOverloadedInstall(Slerp, "t", 0x59C630, ...)`.
- `source/game_sa/X87Math.h` / `X87Math.cpp` (bloque propio al final, prefijo `QuatSlerpT`): `QuatSlerpTCpp(float* out, const float* from, const float* to, float t)` y el ayudante privado `QuatSlerpTCalcThetaCpp`. No se duplica nada: reutiliza `FlcX87`, `FlcCrtAcos`, `ExtendedPiSinCpp` (privada del mismo archivo) y `QuaternionSlerpCpp` (0x59C300, ya validada).

Qué hace el EXE (desensamblado 0x59C630..0x59C66C, thiscall, `ret 0Ch`; ECX = this = destino; [esp+4] = from, [esp+8] = to, [esp+0Ch] = t):

```
push ebx/esi/edi ; ebx = from ; edi = to ; esi = this
eax = &(hueco de pila del argumento `to`) ; ecx = &(hueco del argumento `from`)       <- punteros de salida a los propios huecos de argumentos
call 0x4D00E0 (cdecl)  CAnimBlendNode::CalcTheta(from, to, &theta [hueco from], &invSin [hueco to])
ecx = this ; push t, invSin, theta, to, from ; call 0x59C300 (CQuaternion::Slerp 5 argumentos)
pop edi/esi/ebx ; ret 0Ch
```
CalcTheta (0x4D00E0): `dot = ((from.w*to.w + from.z*to.z) + from.y*to.y) + from.x*to.x` (24 bits); `fst [tmp]`; `fcomp 1.0` y, sólo si dot > 1, `tmp = 1.0f`; `acos` del CRT del EXE (_CIacos 0x822380); `*theta = float(A)`; si A == 0 (`fcom 0.0`, `test ah,44h / jnp`) `*invSin = 0` y se descarta A; si no, `fsin ; fdivr 1.0 ; fstp [invSin]`. Es lo que replica `QuatSlerpTCalcThetaCpp` con la máquina `FlcX87` (la misma que `QuaternionGetAxisAngleCpp`); incluye el caso dot == -1 (acos = fldpi de 64 bits, el seno se calcula con el x87 real a 64 bits como en `QuaternionGetAxisAngleCpp`). La segunda llamada es `QuaternionSlerpCpp(this, from, to, theta, invSin, t)`, ya validada (Dependencies21G).

Nota sobre `AnimCalcThetaCpp` (la versión de CalcTheta que ya existe en C++ para `CAnimBlendNode::CalcTheta`): no se reutiliza a propósito. Está escrita con `double`/`atan2`/`sin` y, por lo que se ve en su código, no modela `errno = 33` ni el NaN indefinido del dominio (dot < -1 o NaN) ni el `fldpi` de 64 bits (no se comprobó contra el banco de CalcTheta); para que el banco exija igualdad con el EXE también fuera del dominio se usa la ruta `FlcX87` + `FlcCrtAcos`, como `QuaternionGetAxisAngleCpp`.

Efectos secundarios y contrato: no hay escrituras de memoria salvo el destino (las salidas de CalcTheta van a huecos de pila, locales en C++); el destino puede coincidir con `from` o con `to` (CalcTheta lee antes de que `QuaternionSlerpCpp` escriba nada; la helper conserva el orden lectura/escritura componente a componente del EXE). Cambia `errno` (33) en NaN/dominio igual que el EXE. Deja la pila x87 vacía. **Llamadores del EXE: ninguno** (ver abajo), así que EAX/ECX/EDX de salida no los lee nadie.

## Revisión propia contra el EXE (FASE 2)

Segundo desensamblado (`dis.sh 0x59C630 0x59C670`, `0x4D00E0 0x4D0160`, `0x59C300 0x59C3E0`) contrastado instrucción a instrucción:

- Argumentos de CalcTheta: los dos últimos son `&hueco(from)` (theta) y `&hueco(to)` (invSin): `eax = lea [esp+14h]` (tras 3 push, = hueco del 2.º argumento original) se empuja primero (4.º argumento), y `ecx = lea [esp+14h]` (tras 4 push = hueco del 1.º) se empuja después (3.º). Al volver se lee `ecx = [esp+20h]` (theta) y `eax = [esp+24h]` (invSin); `edx = [esp+28h]` = t; se empuja t, invSin, theta, to, from → `Slerp5(from, to, halftheta = theta, sintheta_inv = invSin, t)`. **Coincide.**
- CalcTheta: orden del producto escalar (w, z, y, x), `fst` + `fcomp 1.0` + `test ah,41h / jne` (recorte sólo con dot > 1, con la comparación sobre el valor sin redondear a float), recarga `fld [tmp]`, `fld st(0) ; fstp [theta]`, `fcom 0.0 ; test ah,44h ; jnp` (cero: `fstp st(0)` y `[invSin] = 0`; NaN y el resto: `fsin ; fdivr 1.0 ; fstp [invSin]`). **Coincide.**
- Descubierto en la revisión: la primera versión del ayudante no trataba el caso acos = fldpi de 64 bits (dot == -1) antes de `fsin`; `FlcX87::Fsin` pasaría el argumento a `double`. Corregido copiando el tratamiento ya usado en `QuaternionGetAxisAngleCpp` (`ExtendedPiSinCpp`).
- Segunda llamada: `Slerp5` ya validada (Dependencies21G, 4096 pares, y el arnés de 12 000 000 casos de su comentario). El orden de los argumentos coincide con su declaración `Slerp(from, to, halfTheta, sinThetaInv, t)`.
- **Llamadores del EXE: ninguno.** Búsqueda de `call/jmp` relativos a 0x59C630 en todas las secciones del EXE (cero resultados), búsqueda de la dirección como dword en todo el EXE (cero) y `residue_scan.py 0x59C630` (cero). En la fuente nadie llama a `CQuaternion::Slerp(from, to, t)` (sólo `RtQuatSlerp`, de RenderWare). Por tanto ningún llamador lee EAX/ECX/EDX; sólo se registran (`kCompareVolatileRegisters=false`).
- No verificado (límite): lo que hace `0x83365F -> 0x82E53A` (excepción matemática de CRT) respecto de `errno` en la rama «inexacto»; se asume como en `QuatGetAxis29`.
- Límites: no se compiló; el coordinador debe comprobar que `ExtendedPiSinCpp` emite `fld [hi]; fadd [lo]; fsin` sin `fstp/fld` intermedios (ya vigilado por el caso 2w == -1 de QuatGetAxis29) y que `QuaternionSlerpCpp` conserva el orden de lecturas cuando `destino` solapa parcialmente a from/to (familia 13; hasta ahora sólo estaban verificados los solapes exactos out == from y out == to).

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Arena completa** (0x140 bytes: guarda 0x40, ventana 0x60 con from, to y destino en los desplazamientos del caso, guarda 0x40; relleno previo aleatorio, 0x00, 0xFF o NaN silenciosos) y **errno del EXE** (centinela 0x5A5A5A5A antes de cada lado).
- **Colaboradores**: `CAnimBlendNode::CalcTheta` (0x4D00E0) y `CQuaternion::Slerp` de cinco argumentos (0x59C300) tienen hook RH en la DLL; el banco crea una `HookRoute` para cada una (el constructor las apaga) y las mantiene apagadas toda la corrida: el lado original ejecuta los bytes puros del EXE (la compuerta `route_ok` comprueba en cada lado que ninguna de las dos salta a la DLL) y el candidato no las llama. No se sustituyen por falsos (`collaborators_stubbed = false`); sus entradas de 16 bytes (y 0x59C630 y el cuerpo del hook) se vigilan y se verifican intactas al final.
- Oráculo (`fixture_oracle.py`, desde el desensamblado): x87 de 24 bits con exponente extendido, `fstp dword` al rango float32, NaN/inf/ceros con signo; etapa 1 (CalcTheta con el `_CIacos` modelado como en `QuatGetAxis29`) y etapa 2 (0x59C300: copia si theta == 0, rama theta > pi/2 con `pi - theta` guardado como float y `b` negado con `fchs` que también invierte el signo de un NaN, senos de precisión alta calculados en punto fijo de 400 bits). Simula la memoria en orden, de modo que los solapes son exactos; los resultados desconocidos se propagan como comodín `??`. Autoprueba (ya ejecutada: ok): casos a mano (copia con dot = 1 y con recorte, theta = pi/2, t = 0 y 1, dominio con errno, dot = -1) y 3000 pares aleatorios contra una cadena float32 por operación.
- ABI: ESP (`ret 0Ch`: `popBytes=0`), EBX/ESI/EDI/EBP, DF=0, x87 normalizado (FCW 24 bits, TOP, etiquetas, ST válidos; ninguno al salir), MXCSR sin banderas, hook/ruta, entradas de código vigiladas, estado x87 final.
- **EAX/ECX/EDX de salida: sólo se registran** (ningún llamador).

## Fixtures (`Fixtures.hpp`; espejo `fixture_mirror.py`): 4096 casos = 16 familias x 256 (familia = n % 16)

0 típicos (|dot| < 1, t en [0,004; 1)); 1 casi paralelos (dot = 1 ± pocos ulp, incluye recorte y theta == 0); 2 from == to; 3 opuestos con ruido (dot = -1 ± pocos ulp: dominio, pi de 64 bits o theta cerca de pi); 4 ortogonales (theta = pi/2, frontera de la rama theta > pi/2); 5 sin normalizar (dot > 1 o < -1); 6 mezcla de NaN/inf/ceros/subnormales/enormes en from y to; 7 ceros, subnormales y ±1; 8 valores especiales de t (0, -0, 1, 0,5, negativos, > 1, 1 ± ulp, subnormal, NaN, 1e5…) con to negado la mitad de las veces; 9 dot = 1 - hasta 64 ulp por componente; 10 destino == from; 11 destino == to; 12 from == to (mismo objeto) con destino igual o aparte; 13 desplazamientos arbitrarios (solapes parciales); 14 bits crudos (nunca sNaN); 15 empates y cargas útiles de NaN. Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = destino. Los cuaterniones se generan sólo con bits (tabla de unitarios exactos, perturbaciones ±k ulp, componentes aleatorias): no hay aritmética de coma flotante en el generador.

Cobertura calibrada con el espejo (`fixture_mirror.py`): alias_inputs 329, alias_out 781, all_known 3941, clamp 875, copy 1161, errno_domain 723, finite_in 3634, finite_out 3191, flip 693, indefinite_out 433, inf_in 101, inf_out 15, kind_domain 413, kind_minus1 66, kind_nan 310, kind_normal 2077, kind_plus1 1161, nan_in 396, nan_out 735, negzero_out 467, out_eq_from 388, out_eq_to 391, subnormal_out 155, t_nan 163, t_outside 282, t_unit 3651, theta_half_pi 402, theta_small 354, unknown 155, zero_out 1035; mínimos en `run_local.py` con ~40-60 % de margen (más 256 exactos por familia).

## Suposiciones y riesgos del banco

- Ni el espejo ni el oráculo se han ejecutado contra una sonda real; `Fixtures.hpp` (C++) y el espejo deben producir las mismas filas `input` (el espejo fija `matherr: 0`; el banco registra el valor real de [0xC9ABFC] y el oráculo lo usa). Si `run_local.py` rechaza por cobertura, comparar primero las filas `input`.
- **errno**: el oráculo exige 33 sólo en NaN/dominio (con [0xC9ABFC] == 0); en el resto no exige nada (EXE == DLL).
- **Tolerancia matemática del oráculo**: igual que QuatGetAxis29 (acos del CRT del EXE y fsin no se modelan al ulp): 2 ulp en los floats de salida del oráculo; EXE == DLL estricto en bits. Una diferencia de 3 o más ulp contra el oráculo con EXE == DLL indicaría un fallo del oráculo (p. ej. cancelación en `a*from + b*to`), no de la fuente.
- **Desconocidos (EXE == DLL)**: dot == -1 (el seno de pi de 64 bits depende del hardware: invSin desconocido y por tanto todo el destino), empates de NaN con signos opuestos, `fsin` de argumento infinito o de módulo >= 2^20 (t = 1e5 con theta ~pi queda por debajo del límite).
- **sNaN no cubierto.**
- Si alguno de los dos hooks colaboradores no estuviera registrado en la DLL (`Registered() == false`) el banco no arranca (`ready = false`).
- Si `QuaternionSlerpCpp` no conserva el orden de lectura en solapes parciales (familia 13), el banco lo detecta como diferencia EXE != DLL y no como fallo del oráculo (el oráculo simula el orden del EXE).
- La función se llama con CW de 24 bits fijada por el banco (`_controlfp(_PC_24|_RC_NEAR)`); `FlcX87` redondea por software a 24 bits, como el EXE con la CW del juego.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
