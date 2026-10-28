<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerDealFirst31: CTaskComplexPartnerDeal::CreateFirstSubTask (0x6823B0)

**Estado: APROBADO (revisión 2, 19/10/2026)** — la base 0x681F20 pasa a C++ con hook; el banco adaptado compiló y pasó en el juego (4096 pares, corrida 20261019-171326-340195; evidencia registrada en `validation-summary-reval-20261019.json`). La aprobación de la revisión 1 (corrida 20261019-161255-872921) valía sólo para el caso «base = envoltura al EXE».

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerDealFirst31
# o a mano, siempre en serie:
python -B reversing/PartnerDealFirst31/build_probe.py
python -B reversing/PartnerDealFirst31/run_local.py
python -B reversing/PartnerDealFirst31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PartnerDealFirst31/fixture_oracle.py                                              # «ok»
python -I reversing/PartnerDealFirst31/fixture_mirror.py                                              # compila con cl: tamaños de arena C++ == oráculo + cobertura
python -B reversing/PartnerDealFirst31/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedVMTInstall(CreateFirstSubTask, 0x6823B0)` (`source.patch`; `source-all.patch` aplica además las otras dos funciones de la clase). Archivo de arranque en el clon: `partnerdealfirst31-runtime.start`; salida `partnerdealfirst31-runtime.jsonl`. 4096 pares. Fila de `docs/hooks.csv`: `CTaskComplexPartnerDeal,CreateFirstSubTask,0x00000000,1,0,1,V` (original_address 0x006823B0 en el registro).

Ya comprobado sin el juego: `fixture_oracle.py` → `ok`; `fixture_mirror.py` → `ok` (4096 casos, arenas iguales, 0 faltas de cobertura); `source.patch` pasa `git apply --check` contra el blob de HEAD; `--analyze` de un registro inexistente devuelve `None`. **No** se compiló `Core.hpp`/`LiveComparison.hpp` con el proyecto.

## Adaptación (base 0x681F20 reconstruida con hook Virtual)

**Asume** (API prevista, otro subagente la entrega en `TaskComplexPartner.cpp/.h`): `CTaskComplexPartner::CreateFirstSubTask(CPed*)` en C++ con `RH_ScopedVMTInstall(CreateFirstSubTask, 0x681F20)` (hook «V», categoría `CTaskComplexPartner`). Antes la compuerta exigía que 0x681F20 NO saltara a la DLL; con el hook ese salto existe y el candidato habría ejecutado el cuerpo real. Ahora, siguiendo `CTaskSimpleJetPack`/`PickWeap1`:
- `Run` resuelve el cuerpo real de la DLL de la base con su hook **ENCENDIDO** (`Body(GetHookOurAddress())`, siguiendo thunks ILT) y guarda 16 bytes de referencia; luego lo apaga y guarda los 16 bytes de 0x681F20 (EXE puro). Todo ello **antes** de crear ningún parche.
- Por lado, dentro de `Execute`: ORIGINAL = hook de la base apagado + falso parcheado en 0x681F20; CANDIDATO = hook de la base encendido + el MISMO `FakeBase` parcheado en el cuerpo de la DLL. El parche se crea DESPUÉS de fijar el estado del hook (State reescribe 0x681F20 y la entrada de la DLL) y se destruye antes del siguiente cambio.
- `collaborators_dll` por lado: original = hook apagado, 0x681F20 no salta a la DLL, bytes = referencia EXE; candidato = hook encendido, 0x681F20 salta exactamente a `GetHookOurAddress()` (módulo ASI), cuerpo en el ASI, no es un salto al EXE y bytes = referencia. `globals_restored` = el parche de ese lado restauró los 16 bytes. Al final el hook de la base vuelve a su estado/bloqueo inicial y se verifica la vigilancia de 0x681F20 y de la entrada de la DLL (`watched_entries`, `partner_hook` en `begin`).
- `run_local.py` exige `partner_hook.registered`, cuerpo en la DLL y `stubbed[0].dll == body`. Si el hook de la base no existe (HEAD actual) el banco no arranca (`registered_hook=false`, evento `error`).
- Sin cambios: fixtures, oráculo, espejo y `build_probe.py` (ya lista `TaskComplexPartner.cpp/.h`). `register_hook.py` no se tocó: antes de registrar, actualizar su `SCOPE_NOTE` (la base ya no es una envoltura al EXE; hook de la base sustituido por falso en ambos lados). Ver «Suposiciones y riesgos» más abajo.
- Nada de esto se compiló ni se ejecutó.

## Qué hace la función

El EXE (0x6823B0) es **un único `jmp 0x681F20`**, es decir, `CTaskComplexPartner::CreateFirstSubTask`: reenvía `this` (ECX) y el argumento de pila (`ped`) sin tocar nada y devuelve su EAX. El C++ es `return CTaskComplexPartner::CreateFirstSubTask(ped);`. **Importante:** la base (0x681F20) sigue siendo una envoltura al EXE en HEAD (`plugin::CallMethodAndReturn<..., 0x681F20, ...>`), así que el C++ de esta función depende de una función no reconstruida; es lo que hace el original (un salto), pero la política «sin llamadas al EXE» sólo se cumple del todo cuando se reconstruya 0x681F20 (hay borrador sin validar en el árbol principal).

## Fase 2: revisión contra el EXE

`dis.sh 0x6823B0 0x6823C0`: `jmp 00681F20`, nada más. La vtable de `CTaskComplexPartnerDeal` (0x870754) tiene en la ranura 9 (+0x24) el valor 0x6823B0 (verificado leyendo el EXE); la de `CTaskComplexPartnerChat` (0x8707C4) tiene la base 0x681F20 en esa ranura, lo que confirma que Deal sólo es el reenvío. El borrador del lote 27 del árbol principal (`CTaskComplexPartner::CreateFirstSubTask(ped)`) es correcto; sin discrepancias.

**Residuo de registros / llamadores.** Llamadas virtuales `call [reg+24h]` de la maquinaria de `CTaskComplex`. Un escaneo con capstone de todo `.text` (191 sitios `call [reg+24h]`, programa `scan2.py` del lote) no encuentra, en el código de tareas (0x5xxxxx–0x6xxxxx), ningún sitio que lea ECX/EDX antes de escribirlos tras la llamada (los pocos marcados son falsos positivos de código no relacionado: escrituras de `dl`, llamadas con ECX recién cargado). Por tanto **ningún llamador lee EAX distinto del retorno ni ECX/EDX**; ECX/EDX sólo se registran. (Al ser un salto, el EXE deja en ECX/EDX lo que deje 0x681F20; el C++ puede dejar otros valores. Si algún día se encontrara un lector, poner `kCompareVolatileRegisters=true`.)

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskComplexPartnerDeal","CreateFirstSubTask",0x6823B0)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). **Sustituido** por un falso idéntico en ambos lados: 0x681F20 en el original y el cuerpo de la DLL de la base en el candidato (ver «Adaptación»). El falso registra `(this, ped)` y devuelve un valor del caso. El banco comprueba por lado el estado de 0x681F20 (`collaborators_dll`) y vigila sus 16 bytes y la entrada de la DLL de la base. Observado: EAX (etiqueta `T+0` si es `this`), llamadas `[(this, ped)]` y su número (exactamente 1), `this` intacto (arena completa de 0x100 con guardas), ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta, estado restaurado.

## Fixtures: 4096 = 16 familias × 256

ped nulo / bordes (0, 1, 2, 3, 4, 0x7FFFFFFF, 0x80000000, 0x80000001, 0xFFFFFFFF, 0xFFFFFFFE, 0xCDCDCDCD, …) / arbitrario / el propio `this`; retorno de valor nulo, bordes, arbitrario, el propio `this` o el propio `ped`; EAX/EDX/pila sembrados con ocho semillas de borde y aleatorias. Mínimos calibrados (≈ 50 %) con `fixture_mirror.py`.

## Suposiciones y riesgos

- Si el compilador inlinara `CTaskComplexPartner::CreateFirstSubTask` en el cuerpo de Deal, el falso no se ejecutaría (`ncalls`=0) y el banco fallaría (no pasaría en silencio).
- Si el hook de la base está bloqueado por otro motivo, `State()` no cambia y `collaborators_dll` falla. Se usa `LockState(false)` y se restaura.
- El thunk ILT: si `GetHookOurAddress()` es un thunk a la función real, `Body()` lo sigue; si con el hook encendido el cuerpo resultara ser un salto al EXE, la compuerta falla.
- Aserciones de fuentes: la base debe tener la firma `CTask* (CPed*)` virtual y el nombre exacto `CreateFirstSubTask` en `RH_ScopedVMTInstall`; si la categoría del hook no fuera `CTaskComplexPartner`, cambiar `FindHook` en `LiveComparison.hpp`.
- La función no usa x87; los fixtures no ejercen coma flotante.
- Hash de fuentes probadas: `build_probe.py` lista `TaskComplexPartnerDeal.cpp/.h`, `TaskComplexPartner.cpp/.h`, `Task.cpp`, los hooks `Simple`/`Virtual` y los archivos del banco.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
