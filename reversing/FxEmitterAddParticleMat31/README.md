<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco FxEmitterAddParticleMat31: FxEmitter_c::AddParticle(const RwMatrix&, ...) (EXE 0x4A4050)

Estado: **APROBADO** (19/10/2026): 4096 pares, cero diferencias, estado, partidas y configuración restaurados. Corrida `20261019-095245-962195`, archivada en `reversing/CppTwelveLevel6Final/runs/FxEmitterAddParticleMat31-20261019-095245-962195/`; sonda compilada desde el worktree wf-1 (HEAD fdd6cba63 + `source/game_sa/Fx/FxEmitter.cpp`, idéntico al del árbol principal). Registrado en `validation-registry.json` (tipo V, dirección 0x00000000, original_address 0x4A4050) y `approvals.json`. Alcance: fixtures del banco; colaboradores sustituidos; EAX/ECX/EDX sólo registrados.

| Función | Entrada | Convención | Pares |
| --- | --- | --- | --- |
| FxEmitter_c::AddParticle (sobrecarga con matriz) | 0x4A4050 (slot +0x10 de la vtable 0x85A7A4) | thiscall, 7 argumentos de pila, `ret 1Ch`; `void` | 4096 |

Hook RH **Virtual**: `RH_ScopedVMTOverloadedInstall(AddParticle, "matrix", 0x4A4050, ...)`, nombre `AddParticle-matrix`; fila `FxEmitter_c,AddParticle-matrix,0x004A4050,1,0,1,V` de docs/hooks.csv (la añade el coordinador); en el registro, tipo V con dirección 0x00000000 y original_address 0x4A4050.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh FxEmitterAddParticleMat31
# o a mano, siempre en serie:
python -B reversing/FxEmitterAddParticleMat31/build_probe.py
python -B reversing/FxEmitterAddParticleMat31/run_local.py
python -B reversing/FxEmitterAddParticleMat31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro no existe o no ha terminado
python -I reversing/FxEmitterAddParticleMat31/fixture_oracle.py                                             # autoprueba del oráculo ("ok")
python -B reversing/FxEmitterAddParticleMat31/register_hook.py <carpetaEvidencia>                           # sólo tras aprobar (rellenar SCOPE_NOTE antes)
```
Arranque: `fxemitteraddparticlemat31-runtime.start`; salida `fxemitteraddparticlemat31-runtime.jsonl`. `build_probe.py`/`run_local.py` fijan `runtime_runner.GAME = E:/JUEGOS/GTA San Andreas - CVehicle Test`.

## Fuente (FASE 1)

`source.patch` (git diff -U1 contra HEAD fdd6cba63, SÓLO `source/game_sa/Fx/FxEmitter.cpp`; `git apply --check` correcto sobre `git show HEAD:...`): activa el hook Virtual `"matrix"` y sustituye el `return plugin::CallMethod<0x4A4050,...>` y el borrador sin compilar por la reconstrucción. **No toca X87Math**: reutiliza `FxAddParticleRotZByteCpp` (ya en HEAD, lote 30). Depende de `FxEmitter_c::CreateParticle` (ya en C++ en HEAD); en el banco se sustituye por un stub en el EXE y en la entrada de la DLL.

### Relación con 0x4A3EA0 (sobrecarga de vector)
El EXE **duplica el cuerpo**, no salta ni llama a la de vector. Diferencias exactas (diff de desensamblados): (1) solo cambia cómo se obtiene la matriz de mundo C (3.ª del pool): el EXE escribe la identidad (right.x = up.y = at.z = 1, resto a 0, `flags |= 0x20003`; stores muertos) y a continuación hace `rep movs` de las 16 palabras de `mat` sobre C (flags y rellenos incluidos), así que **C = mat** sin más; (2) reparto de registros distinto. El resto (ProcessEmissionInfo(0,0,length,byte useConst), orden Create A,B,C, RwMatrixUpdate(C), B = C·padre o copia de 16 palabras, D = GetRWMatrix, A = D·B, Destroy(D), CreateParticle, `rotZ >= 0` con `fld/fcomp 0.0/test ah,1`, `_ftol2(rotZ*0.5f)` en `[particula+0x36]`, Destroy C, B, A) es idéntico. No interviene rand() ni sscanf ni RwV3dTransformPoints (la posición va dentro de `mat`; solo RwMatrixUpdate/Multiply, envolturas RenderWare sustituidas en el banco).

### Llamadores (callers-residue.txt)
Solo se invoca por la vtable; el único llamador con 7 argumentos es 0x4AA5F7 (FxSystem_c::AddParticle matriz): EAX/ECX/EDX se reescriben antes de leerse. La función es `void`: **EAX/ECX/EDX solo se registran**.

## Revisión propia contra el EXE (FASE 2)
Desensamblado de 0x4A4050..0x4A41DD contrastado línea a línea con el C++ (qué matriz es cuál: A = 1.ª Create = `[esp+14]`, B = 2.ª = `[esp+10]`, C = 3.ª = ebx; orden de los argumentos de RwMatrixMultiply, de CreateParticle y de los Destroy). Además, el mismo método del banco de vector: `offline/` ejecuta los **bytes reales del EXE (400 bytes) reubicados** frente al texto de producción de FxEmitter.cpp (`extract.py`) con X87Math.cpp compilado con las opciones de producción y los mismos stubs: **0 diferencias en las 4096 fixtures y en 1.000.000 de casos aleatorios con /O1, 300.000 con /Od**; `cmp_o.py` (fixtures C++ == espejo Python; oráculo == lado EXE): 0 diferencias; `livetest.exe` + `run_local_check.py` (mismo Core.hpp que la sonda): 4096 pares aceptados. Mutantes detectados: `flags |= 0x20003` tras la copia (3584 diferencias), `rotZ >= 0` -> `> 0` (66).
Discrepancias en el borrador de HEAD (nunca compilado): `*mat0 = mat` tras `RwMatrixSetIdentity` y `mat1 = mat0` (puntero) en lugar de `rep movs`; destrucción en orden mat0, mat1, mat2 (el EXE: C, B, A); `(uint8)(rotZ * 2.0f)` en vez de byte bajo de `_ftol2(rotZ * 0.5f)`; el hook estaba comentado. Todas corregidas. Límite: no se compiló el proyecto completo.

## Qué se compara
Idéntico al banco FxEmitterAddParticle30 (arena con guardas de 16 bytes en bits, traza de 7 colaboradores sustituidos en EXE y DLL, ABI: ESP/`ret 1Ch`, EBX/ESI/EDI/EBP, DF, x87 normalizado; ruta original no salta a la DLL / candidato sí; oráculo Python independiente desde el desensamblado), con el argumento 1 = matriz de entrada de 16 palabras en el arena.
**Arena = 0x370** (Fixtures.hpp y fixture_oracle.py, verificados por `cmp_o.py`): emisor 0x000, prim 0x040, sistema 0x0C0, blueprint 0x140, padre 0x180, 4 matrices 0x1C0, **matriz de entrada 0x2C0..0x300**, vel 0x300, mult 0x310, partícula 0x330..0x370.
Fixtures (4096, 8 familias × 512): la matriz de entrada lleva 12 floats por `Val` (natural/especial/extremo) y flags + 3 rellenos como basura de 32 bits; el resto igual que el banco de vector (rotZ sensible al byte bajo, particleMode, padre, useConst, createLocal, basura en las matrices del pool). Cobertura calibrada = idéntica a la de vector (particle 3054, no_particle 1042, parent 2768, local 2039, useconst 2114, rot_written 1909, rot_skipped 1145, rot_nonzero 670, rot_nan 186, rot_neg 1418).

## Suposiciones y riesgos
- sNaN no cubierto en las fixtures (NaN silenciosos); la función solo copia la matriz y el hash FNV del stub la registra en bits. El arnés aleatorio con palabras crudas no halló diferencias.
- `createLocal`/`m_UseConstTime` canónicos (0/1).
- Hook Virtual: la ruta exige que 0x4A4050 salte a la DLL con el hook encendido; si el nombre cambiara el banco se detiene en `registered_hook = false`. El hook de vector (`AddParticle-vector`) puede estar encendido a la vez: no interviene.
- Colaboradores sustituidos: probar un consumidor no valida sus cuerpos.
- Con el compilador en Release, si inlineara algún colaborador los parches de entrada no lo interceptarían (la traza fallaría con aviso claro).

## Evidencia sin juego (`offline/`)
```
cd reversing/FxEmitterAddParticleMat31/offline
python -I gen_exe.py && python -I extract.py ../../../source/game_sa/Fx/FxEmitter.cpp      # exebytes.inc, add_body.inc (generados, no versionados)
sh ./buildh.sh cphaddmat31.cpp ; ./cphaddmat31.exe 4096 6 ; ./cphaddmat31.exe 1000000 6 r   # OPT=/Od para Debug
python -I cmp_o.py
EXTRA_INC="<nlohmann/include>" sh ./buildh.sh livetest.cpp && ./livetest.exe live_test.jsonl 4096 && python -I run_local_check.py live_test.jsonl
```
Requieren capstone, el EXE canónico y MSVC 14.51 (rutas en buildh.sh). Los binarios generados se borraron del banco.

## Archivos
`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py`, `source.patch`, `callers-residue.txt`, `offline/`, `README.md`.
