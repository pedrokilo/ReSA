# Banco CameraSoundDist30: CCamera::UpdateSoundDistances (0x515BD0)

**Estado: PREPARADO (lote 30; no compilado ni ejecutado en el juego).** Plantilla: `reversing/ModifySpeed27` (colaborador sustituido por un falso en la dirección del EXE y en el cuerpo de la DLL) y `reversing/QuatGetAxis29` (oráculo x87, autoprueba, compuertas).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh CameraSoundDist30     # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/CameraSoundDist30/build_probe.py
python -B reversing/CameraSoundDist30/run_local.py
python -B reversing/CameraSoundDist30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/CameraSoundDist30/fixture_oracle.py          # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -B reversing/CameraSoundDist30/fixture_selfcheck.py       # compila fixture_dump.cpp (Fixtures.hpp tal cual) con cl x64, vuelca las 4096 filas, compara las constantes de arena con las del oráculo y comprueba la cobertura (ya ejecutada: ok)
python -B reversing/CameraSoundDist30/synthetic_record.py        # fabrica un registro con la forma de Core.hpp usando el oráculo en ambos lados y comprueba run_local.analyze (ya ejecutada: ok)
python -B reversing/CameraSoundDist30/offline_check.py          # SIN juego: compila X87Math.cpp (cl de 32 bits, /O2 /Oi /arch:IA32 /fp:fast) + offline_harness.cpp y compara los ayudantes CamSound* con el oráculo en las 4096 fixtures (ya ejecutada: 0 diferencias)
python -B reversing/CameraSoundDist30/register_hook.py <carpetaEvidencia>   # sólo tras aprobar (revisar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedInstall(UpdateSoundDistances, 0x515BD0)` (hook «UpdateSoundDistances», clase `CCamera`); aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `camerasounddist30-runtime.start`; salida `camerasounddist30-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CCamera,UpdateSoundDistances,0x00515BD0,1,0,1,S` y hay que retirar la envoltura de `docs/implementaciones/envolturas-pendientes.csv` si figura (la añade/retira el coordinador al registrar).

## Fuente (FASE 1)

`source.patch` (`git diff HEAD`, sólo esta función; verificado con `git apply --check` sobre `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Camera.cpp`: `CCamera::UpdateSoundDistances()` en C++ (antes `plugin::CallMethod<0x515BD0>`), campos por desplazamiento (`CameraRawField`, helper anónimo) y `RH_ScopedInstall(UpdateSoundDistances, 0x515BD0)`.
- `source/game_sa/X87Math.h` / `X87Math.cpp` (bloque propio al final, prefijo `CamSound`): `CamSoundPointCpp`, `CamSoundHeightCpp`, `CamSoundBlendCpp`. El `X87Math` del patch está pensado para **concatenarse** con los bloques de los demás lotes (todos añaden al final del archivo).

Qué hace el EXE (desensamblado 0x515BD0..0x515D7E, thiscall, ECX = this, sin argumentos):

```
modo   = word [this + this[0x59]*0x238 + 0x180]        (CCam::m_nMode de la cámara activa; sólo se comparan 16 bits)
si modo ∈ {10,07,27,28,34,2A,2B,29,2D,2E,22,08,33}h y ([[this+0x958]+0x36] & 7) == 3 (ped): escala = 0,5f (0x858B8C) si no 5,0f (0x858C80)
off    = forward(this+0x984..0x98C = m_mCameraMatrix.forward) * escala   (fld/fmul a 24 bits; z: fstp float)
punto  = off + (m_matrix ? m_matrix+0x30 : this+4)                         (x, y, z: fadd a 24 bits + fstp float)
fase   = [0xB7CB4C] (CTimer::m_FrameCounter) % 12
si fase == 0: [this+0x160] = [this+0x15C]; hit = CWorld::ProcessVerticalLine(&punto, float(punto.z + 20,0f), &colPoint, &entidad, true,false,false,false,true,false, nullptr) (0x5674E0)
              [this+0x15C] = hit ? float(colPoint.z - punto.z) : 20,0f
t      = fild(fase + 1) * 0x3E2AAAAB (0x85F0A0, 24 bits)
[this+0x158] = float(((1,0 - t) * [0x160]) + (t * [0x15C]))                (cada operación a 24 bits; un solo fstp)
```
La función no escribe nada más (ni la entidad ni la matriz ni el contador). Constantes verificadas en el EXE: 0x858B8C = 0,5, 0x858C80 = 5,0, 0x858BA4 = 20,0, 0x858624 = 1,0, 0x85F0A0 = 0x3E2AAAAB.

## Revisión propia contra el EXE (FASE 2)

Borrador previo (árbol principal, `git diff HEAD -- source/game_sa/Camera.cpp`) leído y contrastado instrucción a instrucción; segundo desensamblado completo tras escribir. Discrepancias halladas y corregidas:

1. **Aritmética en `float` del borrador** (producto `forward * escala`, suma con la posición, `punto.z + 20`, `colPoint.z - punto.z` y la mezcla `((1 - t) * viejo) + t * nuevo`): el EXE lo hace en el x87 a 24 bits con **exponente extendido**; en `float` un producto que desborda (p. ej. `5 * 2^126`) o un intermedio de la mezcla con t > 1 pierden información que el EXE conserva. Sustituido por `CamSoundPointCpp`/`CamSoundHeightCpp`/`CamSoundBlendCpp` (X87Math.cpp, `/arch:IA32`), con la secuencia exacta (z del desplazamiento por `fstp` float antes de sumar; `t` y `1 - t` en registro; un solo `fstp` al final).
2. **Copia 0x15C → 0x160**: el EXE la hace por `mov` de 32 bits (conserva un sNaN); ahora se copia como `uint32` (y el `20,0f` como `0x41A00000`).
3. **Hook ausente**: el borrador no registraba el hook; añadido `RH_ScopedInstall(UpdateSoundDistances, 0x515BD0)` (junto a `CalculateDerivedValues`).
4. Comprobado y conforme: lista de 13 modos (`cmp ax` de 16 bits: la palabra alta del modo no cuenta), lectura de `[this+0x958]` **sólo** dentro de la lista, `GetIsTypePed()` = `[ent+0x36] & 7 == 3`, posición = `GetPosition()` (matriz+0x30 o `this+4`), `% 12` sin signo, `phase+1` como `movsx`+`inc`, orden de argumentos de `0x5674E0` (origen, z+20, colPoint, entidad, edificios = 1, vehículos/peds/objetos = 0, dummies = 1, seeThrough = 0, polígono = 0), orden de las escrituras (`0x160` antes de la llamada), constantes del EXE.
5. No se hallaron más discrepancias. **No verificado**: los desplazamientos 0x158/0x15C/0x160/0x958/0x984 se acceden en bruto (el encabezado no los nombra todos); 0x984 es `m_mCameraMatrix.m_forward.x` según una cuenta a mano de `Camera.h` (CMatrix en 0x974; no se recontó con el compilador), el banco los ejercita con el objeto completo.

### Llamadores y registros de salida

`residue_scan.py 0x515BD0` → un único llamador directo en `.text`, `0x52C9B6` (en `CCamera::Process`): tras la llamada hay `mov al,[0xB5F851]` (AL escrito) y el siguiente `call 0x821B40` (`_ftol2`, sólo ST0); **ningún llamador lee EAX/ECX/EDX** → sólo registrados (`kCompareVolatileRegisters=false`). ESI (this del llamador) lo conserva el EXE y la DLL (compuerta de no volátiles).

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Objeto `CCamera` completo** (0xD78 bytes, relleno aleatorio por caso): sólo cambian +0x158, +0x15C y +0x160 (se comparan los 12 bytes contra el oráculo; el resto, `rest_ok`, contra la referencia), entidad objetivo falsa (0x40 bytes), `CMatrix` falsa (0x48), guardas (0x40 entre bloques) y contador de fotogramas global.
- **Colaborador `CWorld::ProcessVerticalLine` (0x5674E0)**: falso en la dirección del EXE y en el cuerpo de la DLL (`Resolve(&CWorld::ProcessVerticalLine)`; con el hook del colaborador encendido o apagado el parche del EXE sustituye la entrada y el de la DLL el cuerpo). Registra origen (3), distancia (bits), buildings, vehicles, peds, objects, dummies, seeThrough (sólo el byte bajo) y puntero de polígono (0); escribe colPoint (x, y fijos; z del caso) y devuelve AL = hit con 24 bits altos de EAX aleatorios. `traps_zero` exige ≤ 1 llamada; el oráculo exige 0 (fase ≠ 0) o 1 (fase = 0) con esa traza.
- ESP (`ret`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW 24 bits, TOP, etiquetas, ST válidos; ninguno al salir), MXCSR sin banderas, hook/ruta, entradas de código vigiladas (0x515BD0, 0x5674E0, cuerpo de la DLL del colaborador y de la función), contador `0xB7CB4C` restaurado.
- Oráculo `fixture_oracle.py`: desde el desensamblado con `Fraction`; x87 de 24 bits con exponente extendido, `fstp dword` al rango float32, reglas de NaN/inf/cero con signo; los valores desconocidos (NaN con empate de signos opuestos) se propagan como comodín y se exige EXE == DLL. Autoprueba incluida (casos a mano + 3000 casos contra una cadena float32 por operación).

## Fixtures (`Fixtures.hpp`; volcado `fixture_dump.cpp` + `fixture_selfcheck.py`): 4096 casos = 16 familias × 256 (familia = n % 16)

0 típico, rayo con choque; 1 típico, rayo sin choque (20,0); 2 sólo mezcla (fases 1..11); 3 modo de la lista + ped (escala 0,5, bits altos del byte de tipo libres); 4 modo de la lista + no ped; 5 palabra de modo con basura en los 16 bits altos / vecinos de la lista / un bit alto distinto; 6 `forward` con NaN/inf/ceros/subnormales/enormes; 7 posición (matriz y propia) con mezcla; 8 exponente extendido (`5 × forward` > FLT_MAX con suma finita; z aparte porque el EXE la guarda como float); 9 subnormales y diminutos; 10 valores previos 0x158/0x15C/0x160 con mezcla; 11 mezcla con t > 1 y exponente extendido; 12 z del choque con mezcla; 13 contador de fotogramas extremo (0, 0xFFFFFFFF, ≥ 2^31, múltiplos de 12...); 14 cámara activa 0/1/2 con modos de la lista en las cámaras no activas; 15 bits crudos (nunca sNaN). Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`. `m_matrix` presente o nulo al 50 % (con la otra posición como señuelo).

Cobertura medida (volcado compilado + oráculo): ran 1049, hit 666, nohit 383, ped_scale 932, ped_nonlist 1720, mode_hi_bits 239, ext_exp 153, blend_ext 43, delta_nan 49, nan_out 123, inf_out 111, subnormal_out 78, frame_hi 145, 12 fases entre 221 y 1049, unknown 1; mínimos en `run_local.py` al 60 %.

## Verificaciones hechas sin juego

- `fixture_oracle.py` (autoprueba), `fixture_selfcheck.py` (constantes de arena iguales en C++ y oráculo; cobertura), `synthetic_record.py` (forma del registro ↔ `run_local.analyze`, acepta lo correcto y rechaza lo corrupto).
- **`offline_check.py`**: `X87Math.cpp` real compilado con cl de 32 bits y las opciones del proyecto + `offline_harness.cpp` (copia fiel del flujo de `UpdateSoundDistances` de `Camera.cpp` sobre la misma arena y con el mismo falso) ⇒ **4096 filas iguales al oráculo**. Esto valida `CamSoundPointCpp`/`CamSoundHeightCpp`/`CamSoundBlendCpp` frente al desensamblado; **no** valida el cuerpo compilado de `Camera.cpp` ni la integración con el EXE (eso es el banco).

## Suposiciones y riesgos del banco

- No se compiló ni se ejecutó: `Core.hpp` sigue la plantilla `ModifySpeed27` pero **el C++ del banco no se ha pasado por el compilador** (sí `Fixtures.hpp` y `fixture_dump.cpp`, con cl x64). Si falla al compilar, revisar primero `FakeProcessVerticalLine` (cdecl, 11 dwords) y `EntryPatch`.
- Se asume que `CCamera::UpdateSoundDistances` llama a `CWorld::ProcessVerticalLine` por una llamada directa (no en línea) al cuerpo resuelto por `Resolve`.
- La entidad objetivo es siempre válida y sólo el EXE/DLL la leen con modo de la lista; no se prueba un puntero nulo (un acceso inválido aborta toda la corrida). `m_nActiveCam` > 2 no se cubre (el EXE indexa sin comprobar).
- Los tamaños de arena (0x40 + 0xD78 + 0x40 + 0x40 + 0x40 + 0x48 + 0x40 = 0xF00) están en `Fixtures.hpp` (con `static_assert`) y en `LAYOUT` de `fixture_oracle.py`; `fixture_selfcheck.py` los compara contra el volcado compilado y `run_local.py` contra la fila `begin.layout`.
- sNaN no cubierto; los empates de NaN con signos opuestos sólo exigen EXE == DLL.
- La función se llama con la palabra de control de 24 bits fijada por el banco (`_controlfp(_PC_24|_RC_NEAR)`), que es el estado del juego; los ayudantes `CamSound*` dependen de ello (como el resto de `X87Math`).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_dump.cpp`, `fixture_selfcheck.py`, `synthetic_record.py`, `offline_harness.cpp`, `offline_check.py`, `register_hook.py`, `source.patch`, `README.md`.
