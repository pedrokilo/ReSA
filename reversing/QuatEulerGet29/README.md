<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco QuatEulerGet29: CQuaternion::Get(float* x, float* y, float* z) (0x59C160)

**Estado: APROBADO y registrado (19/10/2026, corrida 20261019-005207-470374, 4096 pares, EXE == DLL bit a bit; informe en resultados-quateulerget29\20261019-005207-470374).** Antes: preparado. Plantilla: `reversing/QuatGetAxis29` (misma clase, mismas fuentes `Quaternion.cpp` / `X87Math`) y `reversing/QuatGet28` (colaborador 0x59C080).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh QuatEulerGet29          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/QuatEulerGet29/build_probe.py
python -B reversing/QuatEulerGet29/run_local.py
python -B reversing/QuatEulerGet29/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/QuatEulerGet29/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -I reversing/QuatEulerGet29/fixture_mirror.py                                              # cobertura de las 4096 fixtures según el oráculo (sin juego; ya ejecutada)
python -B reversing/QuatEulerGet29/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedOverloadedInstall(Get, "euler", 0x59C160, void (CQuaternion::*)(float*, float*, float*))` (hook «Get-euler», clase `CQuaternion`) y con el hook «Get-matrix» (0x59C080) ya existente. Aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `quateulerget29-runtime.start`; salida `quateulerget29-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CQuaternion,Get-euler,0x0059C160,1,0,1,S` (la añade el coordinador al registrar) y hay que retirar la línea de `docs/implementaciones/envolturas-pendientes.csv` de `CQuaternion,Get,0x0059C160` si existe (no la he tocado).

## Fuente (FASE 1)

`source.patch` (`git diff HEAD` de los tres archivos, sólo esta función; verificado con `git apply --check` sobre `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Core/Quaternion.cpp`: `CQuaternion::Get(float*, float*, float*)` deja de llamar al EXE por dirección: `RwMatrix matrix; Get(&matrix); QuatEulerGetCpp(reinterpret_cast<const float*>(&matrix), x, y, z);` y se activa el hook `Get, "euler", 0x59C160`.
- `source/game_sa/X87Math.cpp` / `X87Math.h` (bloque propio AL FINAL de cada archivo, prefijo `QuatEuler`): `QuatEulerGetCpp(const float* m, float* px, float* py, float* pz)` y el ayudante privado `QuatEulerWrapStoreCpp`, ambos sobre la máquina `FlcX87` ya existente. No se duplica nada.

Qué hace el EXE (0x59C160..0x59C223, `ret 0Ch`, thiscall; `sub esp,40h`; la matriz local m ocupa [esp+0..0x3F]; argumentos en [esp+44h] = 1er puntero x, [esp+48h] = 2o y, [esp+4Ch] = 3er z):

```
call 0x59C080 (this, &m)                         m[0..2] = right, m[4..6] = up, m[8..10] = at   (sólo esos nueve floats; el resto de m no se lee)
r1 = fpatan(m1, m5)  -> *z = float(r1) ; fcom 0.0 ; test ah,5 ; jp ; si r1 < 0: fadd [0x858CBC = 2*pi float] ; fstp *z        (z se escribe primero)
s = fsin(*z) ; c = fcos(*z)                      se relee el float YA guardado en *z, resultados de 64 bits sin redondear a float
S = (s*m1) + (c*m5)   (cada fmul/faddp a 24 bits)
r2 = fpatan(-m9, S)  -> *x = float(r2) ; si r2 < 0: + 2*pi ; fstp *x
D = (m2*c) - (m6*s) ; E = (m0*c) - (m4*s) ; r3 = fpatan(-D, E)  -> *y = float(r3) ; si r3 < 0: + 2*pi ; fstp *y
```
`fchs` se aplica a m9 y a D (invierte el signo también de un NaN); la comparación es `fcom` contra 0.0 con `test ah,5 / jp`: sólo suma 2*pi cuando r < 0 (NaN, +-0 y positivos no). La pila x87 queda vacía; los registros muertos y banderas los deja `FlcX87::Commit()`. Orden de escritura en memoria: z (dos veces si r1 < 0), x, y: los punteros pueden solaparse y la lectura de *z ocurre tras escribirse z.

## Revisión propia contra el EXE (FASE 2)

Segundo desensamblado completo (`dis.sh 0x59C160 0x59C226`, pila x87 trazada instrucción a instrucción en los tres bloques, incluidos `fld st(2)/st(3)/st(4)` de los productos, `faddp`/`fsubp st(1),st` = `Arith(op,1,1,0,true)`, `fchs`, y la cola `fpatan; fstp st(2); fstp st(0)` = `FstpSt(2); Pop()`) contrastado con `QuatEulerGetCpp`:

- Origen de cada operando (m[1], m[5], m[9], m[2], m[6], m[0], m[4] = [esp+4, 14h, 24h, 8, 18h, 0, 10h]), orden de operandos de los productos (sin·a primero, cos·b después; a·cos antes que b·sin), destinos z → x → y (3er, 1er, 2o argumento) y el reuso de sin/cos hasta el final: **coinciden**.
- Discrepancias halladas y corregidas durante la revisión: **ninguna** en el orden ni en las operaciones. Al escribirlo se comprobó que `jp` salta cuando (AH & 5) tiene paridad par (0 ó 5: >, = y no ordenados), de modo que sólo suma 2*pi en «menor que»; el ayudante replica `(Ah() & 5) == 0 || == 5` en vez de simplificarlo a `< 0`.
- Constantes verificadas en el EXE: 0x858B50 = 0.0f, 0x858CBC = 0x40C90FDB (2*pi float).
- **Llamadores del EXE: ninguno.** `residue_scan.py 0x59C160` no encuentra ningún `call`/`jmp`, y una búsqueda de la dirección como dword en todo el EXE tampoco; en la fuente de la DLL nadie llama a `Get(float*,float*,float*)`. Por tanto ningún llamador lee EAX/ECX/EDX; sólo se registran (`kCompareVolatileRegisters=false`).
- Colaborador 0x59C080 (ya C++, validado en QuatGet28): en el lado original el hook «Get-matrix» también se apaga (el EXE ejecuta su propia 0x59C080) y el candidato llama a la C++ directamente; no hace falta falso. 0x59C080 no depende de EAX/ECX/EDX de entrada tras el `call` (el EXE recarga ECX desde la pila).

Límites de la revisión: no se compiló, así que no se ha leído el objeto de `X87Math.cpp` ni de `Quaternion.cpp` (que `FlcX87` no emita FP real salvo en sin/cos/atan2; que `RwMatrix matrix` sin inicializar no active `/RTCu`: sólo se pasa su dirección). La precisión de `fsin/fcos/fpatan` en `FlcX87` es la de `double` (el EXE usa 64 bits): una discrepancia real sólo es posible si un resultado trascendente cae a ~2^-29 de un empate de redondeo a 24 bits o de float; el banco lo delataría con EXE != DLL.

## Qué se compara (por lado y en bits; original == candidato == oráculo salvo tolerancia explicada)

- **Arena completa** (0xC0 bytes: guarda 0x40, ventana 0x40 con el cuaternion y los tres floats de salida en los desplazamientos del caso, guarda 0x40; relleno previo aleatorio, 0x00, 0xFF o NaN silenciosos). Cualquier escritura fuera de los cuatro bloques aparece como diferencia.
- Oráculo (`fixture_oracle.py`, desde el desensamblado; reusa sin copiar el modelo de QuatGet28 para la matriz): x87 de 24 bits con exponente extendido para las operaciones aritméticas, sin/cos/atan2 en doubles; tolerancia de 2 ulp en los floats de salida y comodín `??` en las salidas inestables (variantes con perturbaciones relativas de 2^-50 en los resultados trascendentes) o con empate de NaN con signos opuestos. Simula la memoria en orden (z, x, y) para solapamientos. **La comparación EXE == DLL es estricta bit a bit en los 4096 pares.** Autoprueba (ya ejecutada: ok): identidad, giros de +-90 grados, 800 cuaterniones aleatorios contra una cadena float32 (tolerancia 4 ulp), NaN y solapamientos.
- ABI: ESP (`ret 0Ch`: `popBytes=0`), EBX/ESI/EDI/EBP, DF=0, x87 normalizado (FCW 24 bits, TOP, etiquetas, ST válidos; ninguno al salir), MXCSR sin banderas, ruta (original sin salto a la DLL; candidato en el ASI; hook de 0x59C080 apagado en ambos), entradas de código vigiladas (0x59C160, 0x59C080 y el cuerpo), estado x87 final.
- **EAX/ECX/EDX de salida: sólo se registran** (ningún llamador).
- No hay `errno` (la función no llama al CRT).

## Fixtures (`Fixtures.hpp`; espejo `fixture_mirror.py`): 4096 casos = 16 familias x 256 (familia = n % 16)

0 típicos (componentes en [1/8, 1)); 1 componentes exactas (+-0, +-1, +-0,5, +-0,7071); 2 cerca del bloqueo de cardán (x, w = +-0,7071, y y z diminutos o cero); 3 diminutos y subnormales; 4 enormes (productos a inf/NaN); 5 NaN (al menos uno); 6 infinitos (al menos uno); 7 mezcla en las cuatro componentes; 8 ceros con signo y diminutos; 9 rotaciones pequeñas de ambos signos (rama +2*pi); 10 salidas sobre el cuaternion y entre sí; 11 salidas superpuestas entre sí; 12 desplazamientos arbitrarios (múltiplos de 4); 13 bits crudos (nunca sNaN); 14 empates y cargas útiles de NaN; 15 cuaterniones unitarios exactos. Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`. Todo en enteros (el FCW del juego al generar no importa).

Cobertura calibrada con el espejo: alias 625, all_known 3193, finite_in 3101, finite_out 3093, inf_in 393, nan_in 650, nan_out 1003, negzero_out 804, pi_out 359, subnormal_out 59, unknown 903 (familias 4-7, 12-14), wrapped_out 2249, zero_out 1011, 256 exactos por familia; mínimos en `run_local.py` con ~40-50 % de margen.

## Suposiciones y riesgos del banco

- Ni el espejo ni el oráculo se han ejecutado contra una sonda real; `Fixtures.hpp` (C++) y el espejo deben producir las mismas filas `input`. Si `run_local.py` rechaza por cobertura, comparar primero las filas `input` con el espejo.
- Tolerancia del oráculo (2 ulp + comodines): sólo afecta a la comparación contra el oráculo; EXE == DLL sigue siendo exacto. Una salida en bloqueo de cardán (familia 2) cuyo signo dependa de la última cifra del seno puede salir como `??` (se exige EXE == DLL).
- sNaN no cubierto; empates de NaN con signos opuestos: comodín.
- Se usan desplazamientos múltiplos de 4 (el oráculo compara por palabras): los solapamientos parciales de bytes no están cubiertos.
- El estado sticky de las banderas x87 al entrar es el mismo para ambos lados (se restaura antes de cada uno); el estado x87 final se compara normalizado.
- La rama del hook de 0x59C080 apagado depende de que `HookRoute("CQuaternion","Get-matrix",0x59C080,...)` encuentre el hook (ya registrado en HEAD); si no existe, `ready` queda falso y el banco no corre.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
