# Banco PartnerDealFirst31: CTaskComplexPartnerDeal::CreateFirstSubTask (0x6823B0)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantilla: `reversing/PartnerChatSeq30` (ABI, hook Virtual, falso con parche de entrada) y contrato `reversing/_handoff/BRIEF.md`.

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

## Qué hace la función

El EXE (0x6823B0) es **un único `jmp 0x681F20`**, es decir, `CTaskComplexPartner::CreateFirstSubTask`: reenvía `this` (ECX) y el argumento de pila (`ped`) sin tocar nada y devuelve su EAX. El C++ es `return CTaskComplexPartner::CreateFirstSubTask(ped);`. **Importante:** la base (0x681F20) sigue siendo una envoltura al EXE en HEAD (`plugin::CallMethodAndReturn<..., 0x681F20, ...>`), así que el C++ de esta función depende de una función no reconstruida; es lo que hace el original (un salto), pero la política «sin llamadas al EXE» sólo se cumple del todo cuando se reconstruya 0x681F20 (hay borrador sin validar en el árbol principal).

## Fase 2: revisión contra el EXE

`dis.sh 0x6823B0 0x6823C0`: `jmp 00681F20`, nada más. La vtable de `CTaskComplexPartnerDeal` (0x870754) tiene en la ranura 9 (+0x24) el valor 0x6823B0 (verificado leyendo el EXE); la de `CTaskComplexPartnerChat` (0x8707C4) tiene la base 0x681F20 en esa ranura, lo que confirma que Deal sólo es el reenvío. El borrador del lote 27 del árbol principal (`CTaskComplexPartner::CreateFirstSubTask(ped)`) es correcto; sin discrepancias.

**Residuo de registros / llamadores.** Llamadas virtuales `call [reg+24h]` de la maquinaria de `CTaskComplex`. Un escaneo con capstone de todo `.text` (191 sitios `call [reg+24h]`, programa `scan2.py` del lote) no encuentra, en el código de tareas (0x5xxxxx–0x6xxxxx), ningún sitio que lea ECX/EDX antes de escribirlos tras la llamada (los pocos marcados son falsos positivos de código no relacionado: escrituras de `dl`, llamadas con ECX recién cargado). Por tanto **ningún llamador lee EAX distinto del retorno ni ECX/EDX**; ECX/EDX sólo se registran. (Al ser un salto, el EXE deja en ECX/EDX lo que deje 0x681F20; el C++ puede dejar otros valores. Si algún día se encontrara un lector, poner `kCompareVolatileRegisters=true`.)

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskComplexPartnerDeal","CreateFirstSubTask",0x6823B0)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). **Sustituido** por un falso idéntico en ambos lados: 0x681F20 (parche de entrada; el cuerpo de la DLL de `CTaskComplexPartner::CreateFirstSubTask` no tiene hook y llama a esa dirección, así que basta el parche en el EXE). El falso registra `(this, ped)` y devuelve un valor del caso. El banco **exige** que 0x681F20 NO salte a la DLL (`collaborators_dll`) y vigila sus 16 bytes. Observado: EAX (etiqueta `T+0` si es `this`), llamadas `[(this, ped)]` y su número (exactamente 1), `this` intacto (arena completa de 0x100 con guardas), ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta, estado restaurado.

## Fixtures: 4096 = 16 familias × 256

ped nulo / bordes (0, 1, 2, 3, 4, 0x7FFFFFFF, 0x80000000, 0x80000001, 0xFFFFFFFF, 0xFFFFFFFE, 0xCDCDCDCD, …) / arbitrario / el propio `this`; retorno de valor nulo, bordes, arbitrario, el propio `this` o el propio `ped`; EAX/EDX/pila sembrados con ocho semillas de borde y aleatorias. Mínimos calibrados (≈ 50 %) con `fixture_mirror.py`.

## Suposiciones y riesgos

- El C++ de la DLL reenvía por la envoltura `plugin::CallMethodAndReturn` de la base: si esa envoltura desapareciera (reconstrucción de 0x681F20 con hook), el banco avisa (`collaborators_dll` falla, 0x681F20 salta a la DLL) y habría que sustituir el cuerpo de la DLL también.
- La función no usa x87; los fixtures no ejercen coma flotante.
- Hash de fuentes probadas: `build_probe.py` lista `TaskComplexPartnerDeal.cpp/.h`, `TaskComplexPartner.cpp/.h`, `Task.cpp`, los hooks `Simple`/`Virtual` y los archivos del banco.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
