<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco ScriptsBrainsCheckNew31: CScriptsForBrains::CheckIfNewEntityNeedsScript (0x46FF20)

**Estado: PREPARADO (19/10/2026).** Fuente reconstruida y revisada contra el EXE; el banco NO se ha compilado ni ejecutado (lo hace el coordinador, en serie). Sin registro, sin tocar `docs/` ni `hooks.csv`. Plantilla: `reversing/DecisionMakeDecision27` (misma estructura, mismo RNG del EXE, mismas compuertas de ABI/x87).

## Comandos para el coordinador

```
python -I reversing/ScriptsBrainsCheckNew31/fixture_oracle.py        # autoprueba: compila Fixtures.hpp con cl, compara constantes y calcula la cobertura (ya ejecutada: ok)
python -B reversing/ScriptsBrainsCheckNew31/build_probe.py           # compila la sonda (solo el coordinador)
python -B reversing/ScriptsBrainsCheckNew31/run_local.py             # corre en el clon E:\JUEGOS\GTA San Andreas - CVehicle Test
python -B reversing/ScriptsBrainsCheckNew31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # solo analisis (None si el registro no existe o no ha terminado)
python -B reversing/ScriptsBrainsCheckNew31/register_hook.py <carpetaEvidencia>   # solo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con los hooks `CScriptsForBrains::CheckIfNewEntityNeedsScript` (0x46FF20, el nuevo) y `StartOrRequestNewStreamedScriptBrain` (0x46CD80, ya en HEAD). Aplicar antes `source.patch` si el arbol no lo contiene (sin cambios en el `.h`). Arranque en el clon: `scriptsbrainschecknew31-runtime.start`; salida `scriptsbrainschecknew31-runtime.jsonl`.

## Fuente (FASE 1)

`source.patch` (git diff unificado contra HEAD, SOLO esta funcion; verificado con `git apply --check` sobre `git show HEAD:source/game_sa/ScriptsForBrains.cpp` en un directorio temporal, y encadenable con los parches de los otros dos bancos hermanos en cualquier orden) toca `source/game_sa/ScriptsForBrains.cpp`: sustituye `plugin::CallMethod<0x46FF20,...>` por el cuerpo C++ y añade `RH_ScopedInstall(CheckIfNewEntityNeedsScript, 0x46FF20)`. Sin `__asm`, sin llamadas al EXE para esta funcion; el unico acceso por direccion es el `rand` del CRT del EXE (0x821B1E), como en `CDecision::MakeDecision`.

Qué hace el EXE (0x46FF20..0x46FFE6, thiscall, ECX = this = tabla de 70 cerebros de 0x14 bytes, pila: entity, attachType, unused, `ret 0Ch`):

```
EBX = attachType (solo BL cuenta) ; EBP = entity ; EDI = this ; CL = indice = 0 ; bandera "ya arranco" = 0
test bl,bl ; jne objeto
  ped:     EAX = [EBP+0x474] ; test EAX,0x800000 -> ret ; test EAX,0x1000000 -> ret        (bHasAScriptBrain / bWaitingForScriptBrainToLoad)
  objeto:  test [EBP+0x140],0x300000 -> ret                                                 (m_nObjectFlags 0x100000 | 0x200000)
bucle (indice 0..69, cmp cl,46h ; jb):
  si la bandera esta puesta -> fin
  cmp byte [brain+2],bl      (m_TypeOfBrain, BYTE) -> distinto: siguiente
  cmp word [brain+0xC],[EBP+0x22]   (modelo, 16 bits) -> distinto: siguiente
  call rand (0x821B1E) ; and eax,0xFFFF ; fild ; fmul [0x858B14]=2^-15 ; fmul [0x858628]=100.0 ; call 0x821B40 (_ftol2, trunca)
  cmp eax, movzx word [brain+0xE]   (m_PercentageChance, SIN signo) ; jge siguiente
  push 1 ; push attachType(dword completo) ; push entity ; push indice ; ECX = this ; bandera = 1 ; call 0x46CD80 (StartOrRequestNewStreamedScriptBrain)
```
Un `rand` por coincidencia (tipo y modelo iguales) hasta el primer acierto; tras arrancar un script no se examina ningun cerebro mas. El producto `rand * 2^-15 * 100` es exacto en float (r < 2^15, r*100 < 2^22), asi que la precision x87 (24/64 bits) no cambia el entero resultante; roll en [0, 99].

## Revisión propia contra el EXE (FASE 2)

Tras escribir, se volvió a desensamblar (0x46FF20..0x46FFE6) y se contrastó linea a linea con el codigo:

| EXE | fuente |
|---|---|
| `test bl,bl ; jne` (46FF32) | `attachType == 0` ped; resto, objeto (cualquier valor != 0, tambien negativos) |
| `[ebp+0x474] & 0x800000 / 0x1000000` -> ret | `ped->bHasAScriptBrain \|\| ped->bWaitingForScriptBrainToLoad` |
| `[ebp+0x140] & 0x300000` -> ret | `static_cast<CObject*>(entity)->m_nObjectFlags & 0x300000` |
| `cl` indice, `cmp cl,46h ; jb` | `for (uint8 i = 0; i < 70; i++)` |
| `cmp byte [esi+2],bl` | `brain.m_TypeOfBrain != attachType` (ambos int8) |
| `cmp cx,[ebp+0x22]` (16 bits) | `uint16(brain.m_PedModelOrPedGeneratorIndex) != uint16(entity->m_nModelIndex)` |
| `call 0x821B1E` | `reinterpret_cast<int(__cdecl*)()>(0x821B1E)()` |
| `and eax,0xFFFF ; fild ; fmul 2^-15 ; fmul 100.0 ; _ftol2` | `int32(float(r & 0xFFFF) * 2^-15f * 100.0f)` (`bit_cast<float>(0x38000000)`; las constantes 0x858B14 = 3.0517578125e-05 y 0x858628 = 100.0 se leyeron del EXE) |
| `movzx esi, word [esi+0xE]` ; `cmp eax,esi ; jge` | `roll < int32(brain.m_PercentageChance)` (uint16 ampliado sin signo) |
| `push 1,ebx,ebp,idx ; call 0x46CD80` ; bandera = 1 | `StartOrRequestNewStreamedScriptBrain(i, entity, attachType, true); return;` |

**Discrepancia hallada y corregida en el borrador del lote 27** (`git diff HEAD` del arbol principal): el borrador llamaba a `rand()` (el del CRT de la DLL); el EXE usa el suyo (0x821B1E) con su propia semilla (`srand` 0x821B11). Corregido a la direccion del EXE. El resto del borrador (orden de comparaciones, `uint16` del porcentaje, retorno tras el primer acierto) coincidia con el desensamblado. Los 24 bits altos de `attachType` que el EXE reenvia en `push ebx` no los lee nadie (el colaborador toma `int8`).

## Registros EAX/ECX/EDX de salida: ningun llamador los lee

Llamadores en el EXE (`residue_scan.py 0x46FF20`): `0x59EAF7` (nada despues), `0x61590C` (`push 0/eax ; call 0x732B00`: lee solo la pila), `0x6F3D76` (`push 64h ; push 0 ; call 0x407180`: GetRandomNumberInRange, solo pila). Ninguno lee EAX, ECX ni EDX tras la llamada, asi que el banco los **registra** (`original_residue`/`candidate_residue`) pero no los exige (`kCompareVolatileRegisters=false`).

## Diseño del banco

- Original = entrada 0x46FF20 del EXE con el hook apagado; candidato = hook encendido (`HookRoute`/`RegisteredTarget`, igual que `DecisionMakeDecision27`).
- 4096 fixtures (`Fixtures.hpp`, `kind = n % 16`, 256 por clase): ped con 0x800000 o 0x1000000 (retorno previo), objeto con 0x100000/0x200000/ambos, sin coincidencias, una o varias coincidencias (2-4) donde acierta la j-esima o ninguna (recorre los 70), coincidencias en los indices 0 y 69, modelos limite/casi-iguales (bit a bit), porcentajes >= 0x8000 (sin signo), tipos distintos con modelo y 100 % tentadores, aleatorio libre, y flags del otro tipo con la polaridad contraria. attachType con basura en los 24 bits altos. Las tiradas se colocan sobre la semilla real calculando el LCG: `pct == roll` (falla en el limite) y `pct == roll + 1` (acierta en el limite).
- **Colaborador sustituido**: `StartOrRequestNewStreamedScriptBrain` (0x46CD80) por el MISMO falso `__fastcall` en la direccion del EXE y en el cuerpo C++ de la DLL (hook encendido, resuelto con `HookRoute::Body()` antes de parchear, y parches quitados ANTES de restaurar los hooks). Traza: this (etiquetado contra la arena), indice, entidad (etiquetada), attachType (byte bajo), bAdd (byte bajo).
- **Reales** (no sustituidos): `rand` del EXE 0x821B1E y `_ftol2` 0x821B40; el RNG es real en ambos lados con la misma semilla (`srand` 0x821B11, estado en `tiddata+0x14`); se compara el estado tras la llamada y el numero de `rand` consumidos (0 con retorno previo o sin coincidencias; 1 por coincidencia examinada). Entradas vigiladas (`Watch`) de 0x821B1E/0x821B11/0x827B3D/0x821B40.
- Memoria: arena de `this` (0x40 guarda + 0x578 + 0x40 guarda = 0x5F8) y de entidad (0x40 + 0x7C4 + 0x40 = 0x844); hash FNV de toda la arena (con guardas) antes y despues. Las mismas constantes en `Fixtures.hpp` (C++) y `fixture_oracle.py` (comprobadas por la autoprueba: compila `Fixtures.hpp` con `cl` y compara las constantes y TODAS las filas).
- Compuertas por lado: ESP, EBX/ESI/EDI/EBP, DF=0, x87 (FCW 24 bits normalizado: FCW, TOP, tags y ST validos), MXCSR sin banderas, ruta (`route_ok`: entrada del EXE intacta (original) o salto al ASI (candidato), y los dos falsos puestos), globales restaurados (semilla de rand, entradas vigiladas, x87).
- `run_local.py`: oraculo independiente (`fixture_oracle.py`, modelado del desensamblado), cobertura minima calibrada con el volcado (`MINIMUMS`, 85 % de lo observado), `analyze` devuelve `None` si el `.jsonl` aun no existe. Probado sin juego con un registro sintetico "perfecto" (aceptado) y 8 corrupciones (todas rechazadas).

## Suposiciones y riesgos

- No se ha compilado: Core.hpp se comprobo sintacticamente (`cl /Zs`, x86, C++20) con un stub de `HookRoute`; `LiveComparison.hpp` (usa tipos del juego) y los desplazamientos de los simbolos de la DLL se comprueban en ejecucion (`layout_ok`: `sizeof(tScriptForBrains)==20`, offsets 2/0xC/0xE, `CEntity::m_nModelIndex==0x22`, `CObject::m_nObjectFlags==0x140`).
- Los bits de flags de ped (0x800000/0x1000000 de `+0x474`) se toman de los bitfields de `CPed`; la fuente ya los usa `StartOrRequestNewStreamedScriptBrain` (validada).
- Se asume que la DLL llama a `StartOrRequestNewStreamedScriptBrain` por su cuerpo (Debug, sin inlining). Si estuviera en linea, la traza saldria vacia y el banco lo delataria (el oraculo exige la llamada).
- El numero de `rand` consumidos se deduce del estado final del LCG (hasta 80 pasos); si el estado no se encuentra, `rand_calls = -1` y el oraculo lo rechaza.
- Un cerebro con `percentageChance` como `int16` firmado (error tipico) se delataria con los porcentajes >= 0x8000 de los tipos 12/14.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py` (plantilla), `source.patch`, `README.md`. El parche combinado de los tres bancos hermanos esta en `reversing/ScriptsBrainsStart31/source-3funciones.patch`.
