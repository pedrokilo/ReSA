<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerChatSeq30: CTaskComplexPartnerChat::GetPartnerSequence (0x684380)

**Estado: PREPARADO** (sin compilar ni ejecutar: lo corre el coordinador en serie). Plantilla: `reversing/PartnerShoveSeq28` (pool sintético, referencias, hook Virtual), `reversing/PedSaveExtract28` (falso con parche de entrada en el EXE y en la DLL) y el contrato `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerChatSeq30          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/PartnerChatSeq30/build_probe.py
python -B reversing/PartnerChatSeq30/run_local.py
python -B reversing/PartnerChatSeq30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/PartnerChatSeq30/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/PartnerChatSeq30/fixture_mirror.py                                              # compila fixture_mirror.cpp con cl (sin el juego): 0 casos inválidos + tamaños de arena C++ == oráculo + cobertura frente a los mínimos
python -B reversing/PartnerChatSeq30/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedVMTInstall(GetPartnerSequence, 0x684380)` (categoría/clase `CTaskComplexPartnerChat`, hook `Virtual`; `source.patch` lo añade a HEAD; `source-both.patch` aplica además `MakeAbortable`, ver banco hermano `PartnerChatAbort30`). Archivo de arranque en el clon: `partnerchatseq30-runtime.start`; salida `partnerchatseq30-runtime.jsonl`. 4608 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskComplexPartnerChat,GetPartnerSequence,0x00000000,1,0,1,V`.

**Requisitos del árbol al compilar** (el banco falla con mensaje claro si no se cumplen): los colaboradores reales (`CTask::operator new` 0x61A5A0, constructores 0x632BD0 / 0x66B890 / 0x682CB0 / 0x62F310 / 0x61ABE0, `AddTask` 0x632D10, `RegisterReference` 0x571B70) deben seguir enganchados a la DLL (`collaborators_dll`); `CReferences::pEmptyList` no nulo con ≥ 8 referencias libres; pool de tareas con ≥ 256 ranuras (el juego: 500).

Ya comprobado sin el juego (worktree `D:\Original\wc-4`): `fixture_oracle.py` imprime `ok`; `fixture_mirror.py` (cl, pool de 500) imprime `ok` con 4608 casos aceptados por el oráculo, tamaños de arena iguales en `Fixtures.hpp` y el oráculo y 0 faltas de cobertura; `source.patch` pasa `git apply --check` contra el blob de HEAD. **No** se ha compilado `Core.hpp`/`LiveComparison.hpp` con el proyecto (riesgo de errores de compilación del banco: el coordinador lo verá en `probe-build.log`).

## Qué hace la función (desensamblado 0x684380..0x6846C3, `ret` normal, marco SEH propio)

`this` en ECX (→ ESI). Lee `m_partner` (+0x38), `m_point` x/y/z (+0x40/+0x44/+0x48), el **byte** `m_leadSpeaker` (+0x58), el `int8` `m_updateDirectionCount` (n, +0x5B) y el **byte** `m_conversationEnabled` (+0x74). Orden observable:
1. `new(0x40)` (0x61A5A0) + `CTaskComplexSequence` (0x632BD0); `new(0x28)` + `CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f ([0x86FC7C]), 0.001f)` (0x66B890); `AddTask` (0x632D10) aunque el Turn sea nulo.
2. **Duraciones** (x87 con FCW de 24 bits): `fild(n<<4)`, `fadd z`, `fadd y`, `fadd x` (cada suma redondeada a 24 bits); `_ftol` (0x821B40: trunca hacia 0, NaN/inf/|x|≥2^63 → bit de signo, parte baja 0) `& 0x7F` = s; r = s·(1/128) (0x858B88), `1−r`; `t1 = (int)(r·3000 + 3500)` y `t2 = (int)((1−r)·3000 + 3500)` (0x859A94, 0x8707FC; todo exacto en 24 bits, con `1−r` pasando por un `fstp float` intermedio).
3. `m_leadSpeaker == 0` (byte entero): `ecx = [m_partner+0x47C]` (`m_pIntelligence`, **sin comprobar nulo**) → `CPedIntelligence::FindTaskByType(0x4B4 = TASK_COMPLEX_PARTNER_CHAT)` (0x600EE0) → `m_conversationEnabled = byte [eax+0x74]` (sin comprobar nulo; copia el byte entero).
4. `m_conversationEnabled != 0` (byte): contextos A = palabra **sin signo** en `0xB613F8 − 4n` y B = en `0xB613FA − 4n` (= `s_Conversation[6−2n]` y `[7−2n]`; con n = 4 se leen **antes del array**: 0xB613E8 = `s_ConversationLength` y 0xB613EA); `len = (int16)[0xB613E8]`; si `3 − n == len` → `m_updateDirectionCount = 0`; lead ≠ 0: `CTaskComplexChat(1, partner, n', A)` y `(0, partner, n', A)`; lead = 0: `(0, partner, n', B)` y `(1, partner, n', B)` (0x682CB0; n' = `movsx` del byte releído; el contexto se guarda como **palabra**), cada uno con su `AddTask`.
5. Si no: lead ≠ 0 → `CTaskSimpleChat(t1)` (0x5F65A0, 0x34 bytes) y `CTaskSimpleStandStill(t2, false, false, 8.0f)` (0x62F310); lead = 0 → `StandStill(t1…)` y `SimpleChat(t2)`.
6. Devuelve la secuencia (EAX = EDI).

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskComplexPartnerChat.cpp`: includes (`TaskComplexChat.h`, `TaskComplexSequence.h`, `TaskComplexTurnToFaceEntityOrCoord.h`, `TaskSimpleChat.h`, `TaskSimpleStandStill.h`), `RH_ScopedVMTInstall(GetPartnerSequence, 0x684380)` y el cuerpo, rodeado de `#pragma float_control(push) / (precise, on) / (pop)` (el proyecto compila con `/fp:fast`, que podría reordenar las sumas de `float`; con `/arch:IA32` las sumas son x87 con el FCW de 24 bits del juego, igual que el EXE). `source-both.patch` = este y `MakeAbortable` juntos (los dos parches individuales tocan líneas contiguas de `InjectHooks()` y los includes: aplícalos uno sobre otro con `git apply --3way` o usa el combinado). El `.h` no cambia (ya declara `virtual GetPartnerSequence()`).

## Fase 2: revisión contra el EXE (línea a línea) y discrepancias del borrador

Tras escribir se volvió a desensamblar 0x684380..0x6846C3 y se contrastó. **Discrepancias halladas en el borrador sin commit (nunca compilado) y corregidas:**
1. **Semilla en doble precisión.** El borrador sumaba en `double` (`(double)(n<<4) + z + y + x`). El EXE suma en x87 con redondeo a 24 bits tras cada `fadd` (p. ej. n=2, z=16777215.0: el EXE da 16777248 → semilla 32; en doble daría 16777247 → 31). Ahora son sumas de `float` en el orden del EXE `((n<<4)+z)+y)+x` bajo `float_control(precise, on)`. (La conversión `(int32)` de un `float` fuera de rango da 0x80000000 en MSVC y el `_ftol` del EXE da la parte baja de un entero de 64 bits; con `& 0x7F` ambos dan 0 porque un `float` ≥ 2^31 es múltiplo de 256: el banco prueba NaN, ±inf, 2^31, 2^63, FLT_MAX y combinaciones.)
2. **Contextos con signo y lectura fuera del array.** El borrador leía `int16` por `reinterpret_cast<const int16*>(s_Conversation.data())[6 − 2n]` (indexado fuera de los límites, con extensión de signo). El EXE lee **palabras sin signo** en `0xB613F8 − 4n` y `0xB613FA − 4n`; el contexto se guarda en el hijo como palabra, así que sólo importan los 16 bits bajos (el banco prueba 0x7FFF/0x8000/0xFFFF). Ahora la dirección se calcula como entero (`convAt + (6−2n)·2`), sin UB de indexado, y con n = 4 se lee la longitud (0xB613E8) y 0xB613EA como en el EXE.
3. Sin más diferencias: orden de reservas y `AddTask`, argumentos de todos los constructores (`push` en orden inverso verificados: Chat `(esJefe, partner, etapa, contexto)`, StandStill `(tiempo, looped=0, useAnimIdle=0, 8.0f=0x41000000)`, SimpleChat `(duración)`), qué duración va en cada orden (t1 al primero), la prueba de los bytes enteros (`m_leadSpeaker`, `m_conversationEnabled`), la copia del byte desde la otra tarea **antes** de decidir la rama, el reinicio de n **antes** de construir los hijos, y que la etapa se relee tras el reinicio.

| EXE | C++ | Resultado |
|---|---|---|
| 0x684395..0x6843FF | `new CTaskComplexSequence`; `AddTask(new Turn(m_partner, 0.5f, 0.001f))` | igual (constantes 0x86FC7C = 0.5f, 0x3A83126F = 0.001f leídas del EXE) |
| 0x68440B..0x684468 | semilla / `t1` / `t2` | corregido (1); constantes 0x858B88 = 0.0078125, 0x858624 = 1.0, 0x859A94 = 3000.0, 0x8707FC = 3500.0 leídas del `.rdata` del EXE |
| 0x68446F..0x68448C | `if (!m_leadSpeaker) m_conversationEnabled = other->m_conversationEnabled` | igual: `FindTaskByType(0x4B4)` con `this = [partner+0x47C]` |
| 0x68448F..0x6844CB | rama de conversación, contextos, reinicio de n | corregido (2) |
| 0x6844D2..0x684557 / 0x684557..0x6845CB | lead / no lead: dos `CTaskComplexChat` | igual (`ebx` = contexto A y `ebp` = B se pasan con `push` de 32 bits, pero el constructor guarda sólo la palabra baja; los 16 bits altos de `ebx`/`ebp` son los de t2/t1 (< 65536) = 0) |
| 0x6845D6..0x684695 | rama sin conversación | igual |
| 0x6846A7..0x6846C3 | último `AddTask`; `eax = edi` | igual |

**Residuo de registros / llamadores.** Único llamador del EXE: `CTaskComplexPartner::CreateNextSubTask` 0x683AD0, `call [eax+34h]` en 0x683B46 (ranura 13 de las vtables de las tareas de pareja; la de `CTaskComplexPartnerChat` en 0x8707C4 contiene 0x684380, verificado en el EXE) → `jmp 0x6840AC` (`mov ecx,[esp+30h]` …): **nadie lee ECX/EDX tras la llamada**; sólo EAX (el retorno), que el banco compara como etiqueta. ECX/EDX de salida sólo se registran. `residue_scan.py 0x684380` no devuelve llamadas directas (sólo virtual). **El revisor debe confirmarlo a mano** (es el mismo sitio de llamada que el de `PartnerShoveSeq28`, donde ya se comprobó).

## Qué se compara (por lado; original == candidato == oráculo)

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskComplexPartnerChat","GetPartnerSequence",0x684380)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). Cada caso monta, en memoria propia: un `this` falso de 0x78 bytes (con guardas; se fijan partner, `m_point`, lead, n y el byte de conversación), un **partner falso** de 0x480 bytes (tipo/flags/IPL a voluntad, 0..2 referencias previas encadenadas, `m_pIntelligence` en +0x47C hacia una `CPedIntelligence` falsa), la **tarea «otra»** falsa (byte +0x74 = conversación de la otra), un **pool de tareas sintético** (estados con ids de borde, `m_LastFreeSlot`, qué ranuras están libres; el cuerpo de las ranuras reales se restaura siempre) y, si hay conversación, las tres palabras de `s_Conversation` que se leen (longitud, contexto A y B; región 0xB613E0..0xB61404 guardada y restaurada). Observado: `ret` (etiqueta de la 1.ª ranura), `last_after`, cambios de estado del pool, **cada tarea nueva completa** (0x80 bytes con punteros como etiquetas `W<k>+off`/`E+0`/`N<j>`/`R<i>`; vtable sustituida por `GetTaskType`), nada fuera de las ranuras reservadas, `this_changes` (sólo +0x5B y +0x74), partner (sólo `m_pReferences`), `CPedIntelligence` y tarea «otra» intactas, región de `s_Conversation` intacta, llamadas a `FindTaskByType` (this + tipo), cadena de referencias, `pEmptyList` y registros de `aRefs`. ABI: ESP, EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, `fs:[0]`, x87 normalizado, ruta (`route_ok`) y `collaborators_dll`. Todo restaurado y comprobado (`globals_restored`).

**Colaboradores.** *Reales* (ya en C++ y enganchados; se ejecutan en ambos lados): operator new 0x61A5A0, constructores 0x632BD0 / 0x66B890 / 0x682CB0 / 0x62F310 / 0x61ABE0, `AddTask` 0x632D10, `RegisterReference` 0x571B70. `0x5F65A0` (constructor de `CTaskSimpleChat`) **no está enganchado**: en el lado original es código del EXE (llama a 0x61ABE0, de la DLL) y en el candidato es el constructor C++; el banco compara los bytes de la tarea resultante, así que una diferencia ahí delata un defecto de ese constructor. *Sustituido* (`collaborators_stubbed=true`): `CPedIntelligence::FindTaskByType` — parche de entrada en 0x600EE0 **y** en el cuerpo de la DLL (resuelto antes de parchear), mismo falso en ambos lados: registra `(this, tipo)` y devuelve la tarea «otra» (no se valida su cuerpo; ver `reversing/PedIntel52`).

Consecuencia: el banco prueba la secuencia, el orden y los argumentos de las llamadas, los fallos de reserva, la lógica de pool y las referencias, el cálculo de la semilla y los contextos; **no** valida los cuerpos de los constructores (los bytes de las tareas los modela el oráculo desde su desensamblado: si falla **sólo** `tasks` en un campo de un constructor —p. ej. un contexto de 4 bytes en `CTaskComplexChat` en vez de la palabra, o campos del temporizador de `CTaskSimpleRunTimedAnim`— es un defecto de ese constructor reconstruido y no de esta función; `run_local.py` informa el campo distinto).

## Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

`n % 16`: 0 lead sin conversación sin partner; 1 lead con conversación sin partner; 2 lead sin conversación, peatón; 3 lead con conversación; 4 NO lead con conversación de la otra; 5 NO lead sin conversación; 6 peatón con 1–2 referencias previas (a veces ya registrada: la del Turn o la de la primera `CTaskComplexChat`); 7 vehículo/objeto/tipos 0,5,6,7; 8 edificios (sin IPL no registra; con IPL/temporal/procedural sí); 9 conversación con reinicio frecuente; 10 contextos de borde; 11 semilla con flotantes extremos; 12–15 pool casi agotado (1, 2, 3, 4 libres o vuelta del cursor). El partner es nulo sólo si `m_leadSpeaker ≠ 0` (con 0 el EXE lee `[0x47C]`). n con conversación ∈ {−1,…,5} (las palabras tocadas están en la región restaurada); sin conversación, n cubre −128..127. `m_point`: múltiplos de 1/8 en ±5000 más una lista de 40 bordes (±0, ±inf, NaN silenciosos y de señalización, FLT_MAX, 2^24 ± 1, 2^31, 2^63, denormales…). Bytes de lead/conversación: 0, 1, bordes (2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF) y aleatorios. Mínimos de `run_local.py` calibrados con `fixture_mirror.py` (≈ 50 % de lo medido).

## Suposiciones y riesgos

- No se prueba el agotamiento total del pool (el EXE llamaría a `AddTask` con `this = 0` y se detiene) ni partner nulo con lead = 0 (el EXE lee `[0x47C]`) ni la tarea «otra» nula (el EXE lee `[eax+0x74]`).
- Se sobrescribe temporalmente la región 0xB613E0..0xB61404 (incluye `s_bAPlayerSpeaking`, `s_bAllSpeechDisabled`, `s_bPlayerConversationHappening` de `CAEPedSpeechAudioEntity`); se restaura bit a bit tras cada lado y antes de volcar la fila (todo el banco corre en un fotograma, sin ejecutar audio). Si el hilo de audio leyera esas variables justo entonces, podría dar un glitch, no una diferencia del banco.
- `CPool::SetDealWithNoMemory(true)` se activa durante el banco (y se restaura): sin él, la reserva fallida del pool en la DLL ejecuta `__debugbreak`.
- Si la DLL **inlinease** `CPedIntelligence::FindTaskByType` (otra unidad de traducción hoy), el falso no se invocaría y saldría diferencia en `finds`.
- `GetTaskType` de `CTaskSimpleChat` se invoca sobre la vtable del EXE (lado original, 0x86C78C) o de la DLL: si difirieran, el campo `type` lo mostraría.
- `offsetof` verificados con `static_assert` en `LiveComparison.hpp` (`sizeof` 0x78, +0x38, +0x40, +0x58, +0x5B, +0x74, `CPed::m_pIntelligence` +0x47C) y las arenas del C++ frente al oráculo en `fixture_mirror.py`.
- Hash de fuentes probadas: `build_probe.py` lista `TaskComplexPartnerChat.cpp/.h`, `TaskComplexPartner.h`, `TaskComplexSequence`, `TaskComplexTurnToFaceEntityOrCoord`, `TaskComplexChat`, `TaskSimpleChat`, `TaskSimpleRunTimedAnim`, `TaskSimpleStandStill`, `Task.cpp`, `Pool.h`, `PedIntelligence`, los hooks `Simple`/`Virtual` y los tres archivos del banco.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-both.patch`, `README.md`.
