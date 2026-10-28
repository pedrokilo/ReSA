<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco CameraShake30: CCamera::ProcessShake(float) (0x516560)

**Estado: PREPARADO (lote 30; no compilado ni ejecutado en el juego).** Plantilla: `reversing/ModifySpeed27` (colaborador sustituido por un falso en la dirección del EXE y en el cuerpo de la DLL), `reversing/QuatGetAxis29` (oráculo x87, autoprueba, compuertas) y `reversing/CameraSoundDist30` (hermano de este lote).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh CameraShake30         # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/CameraShake30/build_probe.py
python -B reversing/CameraShake30/run_local.py
python -B reversing/CameraShake30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/CameraShake30/fixture_oracle.py          # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -B reversing/CameraShake30/fixture_selfcheck.py       # compila fixture_dump.cpp (Fixtures.hpp tal cual) con cl x64, vuelca las 4096 filas, compara las constantes de arena con las del oráculo y comprueba la cobertura (ya ejecutada: ok)
python -B reversing/CameraShake30/synthetic_record.py        # fabrica un registro con la forma de Core.hpp usando el oráculo en ambos lados y comprueba run_local.analyze (ya ejecutada: ok)
python -B reversing/CameraShake30/offline_check.py           # SIN juego: compila X87Math.cpp (cl de 32 bits, /O2 /Oi /arch:IA32 /fp:fast) + offline_harness.cpp y compara CamShakeOrientCpp con el oráculo en las 4096 fixtures (ya ejecutada: 0 diferencias)
python -B reversing/CameraShake30/register_hook.py <carpetaEvidencia>   # sólo tras aprobar (revisar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedOverloadedInstall(ProcessShake, "float", 0x516560, CVector*(CCamera::*)(float))` (hook «ProcessShake-float», clase `CCamera`); aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `camerashake30-runtime.start`; salida `camerashake30-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CCamera,ProcessShake-float,0x00516560,1,0,1,S` (la añade el coordinador al registrar; la línea comentada `ProcessJiggle` de `Camera.cpp` era una entrada mal nombrada de la misma dirección y se sustituye).

## Fuente (FASE 1)

`source.patch` (`git diff HEAD`, sólo esta función; verificado con `git apply --check` sobre `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Camera.cpp`: `CCamera::ProcessShake(float)` en C++ (antes `plugin::CallMethodAndReturn<0x516560>`), `#include "HandShaker.h"` y el hook. Llama a `CHandShaker::Process` en C++ (colaborador ya reconstruido) y a `CamShakeOrientCpp`.
- `source/game_sa/X87Math.h` / `X87Math.cpp` (bloque propio al final, prefijo `CamShake`): `CamShakeOrientCpp(front, up, matrix, angleZ, intensity)` sobre la máquina `FlcX87` con `FlcMatInvMul3x3` (0x59C810), `FlcVecNormalise` (0x59C910) y `FlcVecCross` (0x59C730) ya existentes. El bloque está pensado para **concatenarse** con los de los demás lotes (todos añaden al final del archivo).

Qué hace el EXE (desensamblado 0x516560..0x516AD0, thiscall, ECX = this, `ret 4`; **el argumento se ignora**, se lee `m_fShakeIntensity` en +0xC90):

```
ebx   = &camaras[activa] = this + 0x174 + this[0x59] * 0x238
si [0xB70048] == 0: 65 escrituras de parámetros en la tabla de CHandShaker (0xB6ECA0, 6 x 0x94) y [0xB70048] = 1
ent   = 0xB6ECA0 + m_nShakeType(+0xCA0) * 0x94 ; CHandShaker::Process(ent, m_fShakeIntensity) (0x50D930)
angulo = float(ent.m_ang.z(+8) * m_fShakeIntensity)
front  = float(front * ent.m_resultMat(+0x3C))  (0x59C810) ; Normalise (0x59C910)
up     = { float(fsin(angulo)), 0, float(fcos(angulo)) }
side   = front x up ; Normalise ; up = side x front                (0x59C730)
si front.x == 0 y front.y == 0 (fcomp): ambos = 0x38D1B717
right  = front x up ; Normalise ; up = right x front
```
Devuelve (EAX) un puntero a un temporal de su pila; la DLL devuelve `nullptr`.

## Revisión propia contra el EXE (FASE 2)

Borrador previo (árbol principal) leído y contrastado instrucción a instrucción; segundo desensamblado completo tras escribir. Discrepancias halladas y corregidas:

1. **El borrador llamaba al EXE** para `CHandShaker::Process` (`plugin::CallMethod<0x50D930>`) y para 0x59C810 (`plugin::CallAndReturn<0x59C810>`): incompatible con la política (todo en C++). Ahora `Process` se llama en C++ y 0x59C810 es `FlcMatInvMul3x3`.
2. **Aritmética del borrador** (`ang.z * intensidad` en float, `CVector::Normalise()`/`CrossProduct()` de la DLL): el EXE usa el x87 a 24 bits con exponente extendido y la sumatoria `(x*x + y*y) + z*z` sin salir de registro. Con vectores enormes (|v| ≥ 2^64: la suma de cuadrados desborda float pero no el x87) o diminutos (cuadrados bajo el mínimo de float) el C++ en float da otro resultado (inf/0). Sustituido por `CamShakeOrientCpp` (secuencia FlcX87 idéntica a 0x59C810/0x59C910/0x59C730; `fsin`/`fcos` del hardware).
3. **Hook ausente**: el borrador no lo registraba; añadido `RH_ScopedOverloadedInstall(ProcessShake, "float", 0x516560, ...)` (el hueco comentado `ProcessJiggle` de la misma dirección se elimina).
4. **Tabla de inicialización**: las 65 escrituras del borrador se comprobaron **mecánicamente** contra el desensamblado (0x516586..0x516967; un simulador de `mov [esp+x]`/registro/`mov [abs]` y comparación de diccionarios): idénticas (valores y direcciones; el orden no es observable salvo la bandera, que ya se escribe en el punto correcto).
5. **Índice de la entrada**: el EXE hace `imul 94h` de 32 bits sin comprobar el rango; ahora `0xB6ECA0 + uint32(m_nShakeType) * 0x94`.
6. Comprobado y conforme: orden de argumentos de 0x59C730 (`out, a, b`; `a` = segundo), de 0x59C810 (`out, vector, matriz`), `fcomp ...; test ah,44h; jp` ⇒ sólo la igualdad ordenada (NaN no cuenta), `front.x` y `front.y` se comprueban **después** de actualizar `up` y antes de `right`, `up.y = 0` por almacenamiento entero, `ang.z` leído tras `Process`, desplazamientos de `CCam` (front = +0x190, up = +0x1B4: coinciden con `VALIDATE_SIZE(CCam, 0x238)` contando campos).
7. No verificado: los desplazamientos de `CCamera` +0xC90 (`m_fShakeIntensity`), +0xCA0 (`m_nShakeType`) y +0x174 (`m_aCams`) provienen del desensamblado (el borrador y el orden de campos de `Camera.h` concuerdan, pero no se recontaron byte a byte); el C++ usa los campos con nombre, así que un desajuste saldría en el banco; `m_nShakeType` fuera de 0..5 no se prueba (el falso escribiría fuera de la tabla).

### Llamadores y registros de salida

`residue_scan.py 0x516560` → dos llamadores directos: `0x51A72B` (`CCamera::ProcessShake()`, 0x51A6F0: tras la llamada `pop ecx; ret`, EAX no se lee) y `0x52BA68` (`CCamera::Process`: `jmp` a `mov al,[esi+58h]`, AL se reescribe). **Ningún llamador lee EAX/ECX/EDX** → sólo registrados (`kCompareVolatileRegisters=false`). EBX/ESI/EDI/EBP los conserva el EXE y se exige a la DLL.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Objeto `CCamera` completo** (0xD78 bytes, relleno aleatorio): sólo cambian `front` (+0x190) y `up` (+0x1B4) de la cámara activa (se comparan contra el oráculo; el resto, `rest_ok`, contra la referencia), guardas (0x40 a cada lado).
- **Tabla de CHandShaker del juego** (0xB6ECA0, 0x378 bytes) completa y bandera 0xB70048: el banco las escribe antes de cada lado (relleno aleatorio; bandera 0 en ≈ 18 % de los casos para ejercitar las 65 escrituras) y las restaura al final del caso; el oráculo calcula la tabla esperada (relleno + 65 escrituras si la bandera es 0 + lo que escribe el falso).
- **Colaborador `CHandShaker::Process` (0x50D930)**: falso en la dirección del EXE y en el cuerpo de la DLL (`Resolve(bit_cast<uintptr_t>(&CHandShaker::Process))`). Registra el índice de la entrada y los bits del argumento (debe ser `m_fShakeIntensity`), y escribe `m_ang.z` y los 9 floats de `m_resultMat` que la función lee (right, forward, up; las palabras de relleno 3, 7 y 11 no se tocan). `traps_zero` exige exactamente 1 llamada.
- **Rutinas CVector del EXE** (0x59C910 Normalise, 0x59C730 CrossProduct) **no se sustituyen**; el lado EXE debe ejecutar su código real, por lo que el banco **apaga durante toda la corrida los hooks RH `CVector/Normalise` y `CVector/CrossProduct-out`** (`HookOff`; `begin.exe_helper_hooks` lo registra y `run_local.py` lo exige; se restauran al final y se comprueba). La DLL no las llama (usa FlcX87).
- ESP (`ret 4`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado, MXCSR sin banderas, hook/ruta, entradas de código vigiladas (0x516560, 0x50D930, cuerpo de la DLL del colaborador y de la función), tabla y bandera restauradas.
- Oráculo `fixture_oracle.py`: desde el desensamblado con `Fraction`; x87 de 24 bits con exponente extendido, `fstp dword` al rango float32, reglas de NaN/inf/cero con signo; `fsin`/`fcos`: NaN → mismo NaN, ±inf → indefinido, ±0 → ±0 y 1, |A| ≥ 2^63 → el operando queda (C2), 2^20 ≤ |A| < 2^63 → **desconocido** (reducción de 66 bits del hardware; sólo se exige EXE == DLL), resto → `math.sin/cos` redondeado a float. Autoprueba incluida (casos a mano + 3000 casos contra una cadena float32 por operación).

## Fixtures (`Fixtures.hpp`; volcado `fixture_dump.cpp` + `fixture_selfcheck.py`): 4096 casos = 16 familias × 256 (familia = n % 16)

0 típico (tabla ya rellena); 1 típico con bandera 0 (el EXE rellena la tabla); 2 intensidad con 0/−0/subnormal/enorme/NaN/inf/negativa; 3 `m_ang.z` con especiales; 4 matriz con especiales; 5 `front` con especiales; 6 `front = (±0, ±0, z)` con la identidad (anulación a 0x38D1B717) y variantes con sólo x o y distinto de 0; 7 ángulo mediano (hasta ≈ 2^19); 8 ángulo en el rango impreciso del `fsin` (2^20..2^62), ≥ 2^63 e infinito; 9 vector girado nulo (filas de la matriz o `front` a cero); 10 `front` paralelo a `up` (ángulo ±0): lado nulo; 11 casi paralelo (ángulo y componentes diminutos); 12 exponente extendido por encima (componentes 2^62..2^120: la suma de cuadrados desborda float); 13 exponente extendido por debajo (2^−90..2^−50); 14 bits crudos (nunca sNaN); 15 empates y cargas útiles de NaN. Entradas de la tabla: `m_nShakeType` 0..5 repartido uniformemente; cámara activa 0/1/2. Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`.

Cobertura medida (volcado compilado + oráculo): bandera 0 → 749 casos; ángulo cero 437, mediano 244, impreciso 152, ≥ 2^63 209, infinito 168, NaN 193; norm1_zero 203, norm2_zero 200, anulación 317, ext_exp_hi 335, ext_exp_lo 127, nan_out 934, desconocidos 194; mínimos en `run_local.py` al 60 %.

## Verificaciones hechas sin juego

- `fixture_oracle.py` (autoprueba), `fixture_selfcheck.py` (constantes de arena iguales en C++ y oráculo; cobertura), `synthetic_record.py` (forma del registro ↔ `run_local.analyze`, acepta lo correcto y rechaza lo corrupto).
- **`offline_check.py`**: `X87Math.cpp` real compilado con cl de 32 bits y las opciones del proyecto + `offline_harness.cpp` (copia fiel del flujo de `ProcessShake` con la lista `put(...)` extraída de `Camera.cpp`, tabla propia y el mismo falso) ⇒ **4096 filas iguales al oráculo** (incluidos los comodines). Esto valida `CamShakeOrientCpp` frente al desensamblado; **no** valida el cuerpo compilado de `Camera.cpp` ni la integración con el EXE (eso es el banco).

## Suposiciones y riesgos del banco

- No se compiló el proyecto ni se ejecutó GTA: el C++ de `Core.hpp`/`LiveComparison.hpp` sigue la plantilla pero **no ha pasado por el compilador** (sí `Fixtures.hpp`, `fixture_dump.cpp` y el arnés). Si falla al compilar, revisar primero `FakeHandShakerProcess` (`__fastcall` con ECX, EDX ficticio y un argumento en la pila, `ret 4` como el thiscall) y `HookOff`.
- Se asume que la DLL llama a `CHandShaker::Process` por llamada directa al cuerpo resuelto con `Resolve` y que los nombres de hook `CVector/Normalise` y `CVector/CrossProduct-out` son los registrados (`Vector.cpp`); si faltan, `begin.exe_helper_hooks` mostrará `found=false` y el lado EXE usará el EXE de todos modos.
- `fsin`/`fcos` entre 2^20 y 2^63 quedan como comodín (sólo EXE == DLL); el resto del rango se compara con `math.sin/cos` (error de empate de redondeo ≈ 2^−29 por valor, despreciable con 4096 casos; una discrepancia aislada de 1 ulp en `up` con ángulo normal se investigaría mirando la fila `input`).
- sNaN no cubierto; los empates de NaN con signos opuestos sólo exigen EXE == DLL.
- La función se llama con la palabra de control de 24 bits fijada por el banco (`_controlfp(_PC_24|_RC_NEAR)`), que es el estado del juego.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_dump.cpp`, `fixture_selfcheck.py`, `synthetic_record.py`, `offline_harness.cpp`, `offline_check.py`, `register_hook.py`, `source.patch`, `README.md`.
