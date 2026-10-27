<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco ScriptsBrainsStart31: CScriptsForBrains::StartNewStreamedScriptBrain (0x46B270)

**Estado: PREPARADO (19/10/2026).** Fuente reconstruida y revisada contra el EXE; el banco NO se ha compilado ni ejecutado (lo hace el coordinador, en serie). Sin registro, sin tocar `docs/` ni `hooks.csv`. Plantilla: `reversing/DecisionMakeDecision27` (estructura, compuertas de ABI/x87) con colaboradores sustituidos como en `reversing/CTaskSimpleJetPack`.

## Comandos para el coordinador

```
python -I reversing/ScriptsBrainsStart31/fixture_oracle.py        # autoprueba: compila Fixtures.hpp con cl, compara constantes y calcula la cobertura (ya ejecutada: ok)
python -B reversing/ScriptsBrainsStart31/build_probe.py           # compila la sonda (solo el coordinador)
python -B reversing/ScriptsBrainsStart31/run_local.py             # corre en el clon E:\JUEGOS\GTA San Andreas - CVehicle Test
python -B reversing/ScriptsBrainsStart31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # solo analisis (None si el registro no existe o no ha terminado)
python -B reversing/ScriptsBrainsStart31/register_hook.py <carpetaEvidencia>   # solo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedInstall(StartNewStreamedScriptBrain, 0x46B270)` (nuevo; en HEAD estaba comentado con `{.reversed = false}`) y los hooks de los colaboradores `CStreamedScripts::StartNewStreamedScript` (0x470890) y `CTheScripts::RemoveFromWaitingForScriptBrainArray` (0x46ABC0), ya en HEAD. Aplicar antes `source.patch` si el arbol no lo contiene (sin cambios en el `.h`). `source-3funciones.patch` (en esta carpeta) es el parche combinado de los TRES bancos hermanos (`ScriptsBrainsCheckNew31`, `ScriptsBrainsRequest31`, este); los tres `source.patch` individuales tambien se aplican en cualquier orden. Arranque en el clon: `scriptsbrainsstart31-runtime.start`; salida `scriptsbrainsstart31-runtime.jsonl`.

## Fuente (FASE 1)

`source.patch` (git diff unificado contra HEAD, SOLO esta funcion; verificado con `git apply --check` sobre `git show HEAD:source/game_sa/ScriptsForBrains.cpp` en un directorio temporal) toca `source/game_sa/ScriptsForBrains.cpp`: sustituye `plugin::CallMethodAndReturn<...0x46B270...>` por el cuerpo C++ y activa el hook. Sin `__asm` ni llamadas al EXE por esta funcion.

Qué hace el EXE (0x46B270..0x46B367, thiscall, ECX = this, pila: index (byte), entity, bHasAScriptBrain (byte), `ret 0Ch`):

```
ESI = this + movzx byte [arg1]*20 ; push movsx word [ESI] ; ECX = 0xA47B60 ; call 0x470890 (CStreamedScripts::StartNewStreamedScript) -> EBX = CRunningScript*
byte [EBX+0xC9] = byte [ESI+2] (m_ExternalType) ; EAX = movsx byte [ESI+2] ; cmp EAX,5 ; ja fin           (sin signo: negativos y > 5 -> solo el byte 0xC9)
jmp [EAX*4 + 0x46B36C] = {0:46B2AB, 1:46B32A, 2:46B364 (fin), 3:46B2AB, 4:46B32A, 5:46B2AB}               (tabla leida del EXE)
ped (0,3,5):   idx = (ent - [[0xB74490]]) / 0x7C4 (idiv con signo) ; [EBX+0x3C] = idx<<8 + byte [[pool+4]+idx] ; [ent+0x474] |= 0x800000 ;
               si tipo==5: [EBX+0x40] = movzx byte [arg3] ; flags = [ent+0x474] & 0xFEFFFFFF -> [ent+0x474] ; push movzx word [ent+0x794] ; push ent ;
               call 0x46ABC0 (RemoveFromWaitingForScriptBrainArray, cdecl) ; word [ent+0x794] = 0xFFFF
objeto (1,4):  idx = (ent - [[0xB7449C]]) / 0x19C ; [EBX+0x3C] = idx<<8 + byte [[pool+4]+idx] ; [ent+0x140] |= 0x300000
```
Tipo 2 y los demas: solo `m_ExternalType`; la entidad no se toca ni se lee.

## Revisión propia contra el EXE (FASE 2)

Tras escribir, se volvió a desensamblar y se contrastó linea a linea:

| EXE | fuente |
|---|---|
| `movzx eax,byte [esp+4]` ; `lea esi,[ecx+eax*20]` | `m_aScriptForBrains[index]` (`uint8 index`; los 24 bits altos de la pila son basura y se prueban) |
| `push movsx word [esi]` ; `call 0x470890` (ECX = 0xA47B60) | `CTheScripts::StreamedScripts.StartNewStreamedScript(brain.m_StreamedScriptIndex)` (indice int16 -> int32 con signo) |
| `byte [ebx+0xC9] = [esi+2]` | `script->m_ExternalType = brain.m_TypeOfBrain` (`m_ExternalType` esta en 0xC9; `m_LocalVars` en 0x3C: comprobado en ejecucion con `offsetof`) |
| `movsx eax,byte [esi+2] ; cmp eax,5 ; ja` + tabla | `switch (brain.m_TypeOfBrain)` con casos 0/3/5 (ped), 1/4 (objeto) y `default: break` (incluye 2 y negativos) |
| ped: `[ebx+0x3C] = ref` | `script->m_LocalVars[0].iParam = GetPedPool()->GetRef(ped)` (`(idx << 8) \| byte de estado`) |
| ped: `or [edi+0x474],0x800000` | `ped->bHasAScriptBrain = true` |
| `cmp byte [esi+2],5 ; [ebx+0x40] = movzx byte [esp+18h]` | `if (type == 5) script->m_LocalVars[1].iParam = int32(uint8(bHasAScriptBrain))` |
| `and ebx,0xFEFFFFFF` ; `movzx edx,word [edi+0x794]` ; `call 0x46ABC0(edi, edx)` | `toLoad = ped->m_StreamedScriptBrainToLoad; ped->bWaitingForScriptBrainToLoad = false; RemoveFromWaitingForScriptBrainArray(ped, toLoad)` |
| `word [edi+0x794] = 0xFFFF` | `ped->m_StreamedScriptBrainToLoad = -1` |
| objeto: `[ebx+0x3C] = ref(pool 0xB7449C)` ; `or [esi+0x140],0x300000` | `GetObjectPool()->GetRef(object)` ; `object->m_nObjectFlags \|= 0x300000` |

El borrador del lote 27 (`git diff HEAD` del arbol principal) coincidia con el desensamblado en todo lo anterior (incluido el orden: indice del script, tipo externo, variable local 0, bandera, variable local 1, limpieza, remove, 0xFFFF). Sin discrepancias de comportamiento.

## Registros EAX/ECX/EDX de salida: ningun llamador los lee

Llamadores en el EXE (`residue_scan.py 0x46B270`): `0x46B3BE` (`StartAttractorScriptBrainWithThisName`, ya en C++ en la DLL), `0x46CE55` (`StartOrRequestNewStreamedScriptBrain`, ya en C++) y `0x46CFCD` (`StartOrRequestNewStreamedScriptBrainWithThisName` 0x46CED0, aun en el EXE: tras la llamada `jmp 0x46D029` salta a `add edi,8 ; dec ebp ; jne 0x46CF50`, y la cabeza del bucle escribe `esi`/`eax`/`ecx` antes de leerlos). Nadie lee EAX/ECX/EDX de salida: el banco los **registra** pero no los exige.

## Diseño del banco

- Original = entrada 0x46B270 del EXE con el hook apagado; candidato = hook encendido (`HookRoute`/`RegisteredTarget`).
- **Colaboradores sustituidos**, con el MISMO falso en la direccion del EXE y en el cuerpo C++ de la DLL (hook encendido; `Body()` resuelto antes de parchear; parches quitados ANTES de restaurar los hooks): `CStreamedScripts::StartNewStreamedScript` (registra this —etiquetado contra 0xA47B60— e indice con signo, y devuelve la arena de script del caso) y `CTheScripts::RemoveFromWaitingForScriptBrainArray` (registra entidad —etiquetada— y modelo de 16 bits; el EXE lo empuja con extension a cero y la DLL con signo: solo cuenta el valor de 16 bits). Las dos escriben en una misma traza ordenada.
- **Pools FALSOS**: durante cada lado, los punteros globales 0xB74490 (peds) y 0xB7449C (objetos) apuntan a dos `FakePool` (0x14 bytes, misma disposicion que `CPool`: almacenamiento, estados de slot, capacidad 4) con 4 slots de 0x7C4 y 0x19C bytes en arenas con guardas; se restauran justo despues de cada llamada. Los estados de slot son bytes crudos aleatorios (bit 7 incluido), de modo que `GetRef` (`ref = indice << 8 | estado`) se prueba con todos los valores; entidad en los slots 0..3.
- 4096 fixtures (`kind = n % 16`): tipos 0, 3, 5 (con bHasAScriptBrain 0 y 1), 1, 4, 2, -1, 6, 7, 127, -128, 100 y aleatorios; flags de ped extremos (0xFFFFFFFF, 0, solo 0x800000, solo 0x1000000), flags de objeto, `m_StreamedScriptBrainToLoad` 0/0xFFFF/0x8000, indices de script con signo y fuera de rango (el colaborador esta sustituido), punteros de entidad basura (0, 0xDEADBEEF, ...) con tipos que no usan la entidad, y entidad valida de ambos pools con esos mismos tipos (no se debe tocar). `bHasAScriptBrain` siempre 0/1 en el byte bajo (es un `bool`), con basura en los 24 bits altos; el indice igual (basura en los 24 altos).
- Se comparan los bytes cambiados (`script_diff`, `ped_diff`, `obj_diff`, incluidas las guardas) con el oraculo; la tabla de cerebros por hash; y que los pools falsos y sus estados no cambien (`pools_ok`).
- Constantes de arena (`this` 0x5F8, script 0x40+0xE0+0x40 = 0x160, pool de peds 0x40+0x1F10+0x40 = 0x1F90, pool de objetos 0x40+0x670+0x40 = 0x6F0) iguales en `Fixtures.hpp` y `fixture_oracle.py`, comprobadas por la autoprueba (compila `Fixtures.hpp` con `cl` y compara constantes y todas las filas).
- Compuertas por lado: ESP, EBX/ESI/EDI/EBP, DF=0, x87 normalizado, MXCSR, `route_ok` (con los cuatro parches puestos), globales restaurados (punteros de pool incluidos). `run_local.py` con oraculo independiente, cobertura minima calibrada (85 % de lo observado) y `analyze` que devuelve `None` si el `.jsonl` no existe. Probado sin juego con un registro sintetico perfecto (aceptado) y 8 corrupciones (rechazadas).

## Suposiciones y riesgos

- No se ha compilado: `Core.hpp` se comprobo sintacticamente (`cl /Zs`, x86, C++20) con un stub de `HookRoute`; `LiveComparison.hpp` usa tipos del juego y sus comprobaciones de layout se evaluan en ejecucion (`layout_ok`: `GetPedPool()/GetObjectPool()` apuntan a los pools falsos, `sizeof(CPedPool)==0x14`, `offsetof(CRunningScript, m_LocalVars)==0x3C`, `m_ExternalType==0xC9`, `sizeof(CRunningScript)==0xE0`, `CPed::m_StreamedScriptBrainToLoad==0x794`, `CObject::m_nObjectFlags==0x140`).
- Se supone que `CPool<CPed,...>::GetRef` de la DLL divide por 0x7C4 y el de objetos por 0x19C (tamaño con que `Pools.cpp` construye los pools); si `sizeof(S)` fuera otro, la referencia de la DLL diferiria y el banco lo delataria (seria una diferencia real de `GetRef`, no de la funcion).
- Se asume que la DLL llama a los dos colaboradores por sus cuerpos (Debug, sin inlining).
- `bHasAScriptBrain` con byte bajo no canonico (2, 0x80, 0xFF) no se prueba: es un `bool` en C++ y el EXE lo ve como byte; ningun llamador lo pasa asi.
- Los punteros de entidad no validos solo se usan con tipos que no los desreferencian (2, 6.., negativos); con tipos 0/1/3/4/5 el EXE y la DLL (assert de `IsPtrFromPool`) exigen un puntero de su pool.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py` (plantilla), `source.patch`, `source-3funciones.patch`, `README.md`.
