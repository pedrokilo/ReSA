<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerShoveSeq28: CTaskComplexPartnerShove::GetPartnerSequence (0x683120)

**Estado: APROBADO** (18/10/2026, corrida `20261018-175528-909759`: 6144 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-partnershoveseq28\20261018-175528-909759`; archivado en `reversing/CppTwelveLevel6Final/runs/PartnerShoveSeq28-20261018-175528-909759/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CTaskComplexPartnerShove,GetPartnerSequence,0x00000000,1,0,1,V` en `docs/hooks.csv`). Hook virtual: el registro lo guarda como tipo V con `original_address` 0x00683120.

Plantilla: `reversing/DecisionAdd27` y `reversing/CTasks7b` (manejo del pool de tareas) y contrato de `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerShoveSeq28          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/PartnerShoveSeq28/build_probe.py
python -B reversing/PartnerShoveSeq28/run_local.py
python -B reversing/PartnerShoveSeq28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/PartnerShoveSeq28/fixture_oracle.py                                             # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/PartnerShoveSeq28/fixture_mirror.py                                             # espejo del generador (compila fixture_mirror.cpp con cl, sin el juego): 0 casos inválidos + cobertura frente a los mínimos
python -B reversing/PartnerShoveSeq28/register_hook.py <carpetaEvidencia>                           # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedVMTInstall(GetPartnerSequence, 0x683120)` (categoría/clase `CTaskComplexPartnerShove`, hook `Virtual`; `source.patch` lo añade a HEAD). Archivo de arranque en el clon: `partnershoveseq28-runtime.start`; salida `partnershoveseq28-runtime.jsonl`. 6144 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskComplexPartnerShove,GetPartnerSequence,0x00000000,1,0,1,V` (no la añade `register_hook.py`, que no toca `docs/`).

**Requisitos del árbol al compilar** (el banco falla con mensaje claro si no se cumplen): los seis colaboradores (`CTask::operator new` 0x61A5A0, constructores 0x632BD0 / 0x66B890 / 0x61A8B0, `AddTask` 0x632D10, `RegisterReference` 0x571B70) deben seguir enganchados a la DLL (`collaborators_dll`); `CReferences::pEmptyList` no nulo con ≥ 8 referencias libres; pool de tareas con ≥ 256 ranuras (el juego: 500, `Pools.cpp:188`).

## Qué hace la función (desensamblado 0x683120..0x68322F, `ret` normal, marco SEH propio)

`CTaskComplexSequence* GetPartnerSequence()`; `this` en ECX (→ EDI). Lee `m_partner` (+0x38) y el **byte** `m_leadSpeaker` (+0x58). Orden observable:
1. `CTask::operator new(0x40)` (0x61A5A0) → si no nulo, `CTaskComplexSequence` (0x632BD0); si nulo, ESI = 0.
2. `operator new(0x28)` → si no nulo, `CTaskComplexTurnToFaceEntityOrCoord(m_partner, [0x86FC7C] = 0.5f, 0x3A83126F = 0.001f)` (0x66B890; registra una referencia en el partner si no es nulo); `AddTask(seq, turn)` (0x632D10) **aunque la tarea sea nula**.
3. `test [edi+58h]`: ≠ 0 → `operator new(0x20)` + `CTaskSimpleRunAnim(grupo 0, anim 0xA4, blend 4.0f = 0x40800000, hold false)` (0x61A8B0); = 0 → lo mismo con anim 0x8E. `AddTask(seq, run)`.
4. Devuelve la secuencia (EAX = ESI); restaura SEH.

Cambios respecto a la envoltura anterior (`plugin::CallMethodAndReturn<..., 0x683120>`): ninguna llamada al EXE; el `RH_ScopedVMTInstall` es nuevo.

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskComplexPartnerShove.cpp`: includes de `TaskComplexSequence.h`, `TaskComplexTurnToFaceEntityOrCoord.h` y `TaskSimpleRunAnim.h`; `RH_ScopedVMTInstall(GetPartnerSequence, 0x683120)`; cuerpo
```cpp
auto* seq = new CTaskComplexSequence; // sin inicialización por valor, como el original
seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f, 0.001f));
seq->AddTask(new CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, m_leadSpeaker ? ANIM_ID_SHOVE_PARTIAL : ANIM_ID_HANDSUP, 4.0f, false));
return seq;
```
Verificado con `git apply --check` y `git apply` sobre `git show HEAD:<archivo>` en un directorio temporal (el resultado coincide con el árbol de trabajo salvo fin de línea: el árbol tiene CRLF y el blob de HEAD, LF). El `.h` no cambia (ya declara `virtual GetPartnerSequence()`).

## Fase 2: revisión contra el EXE (línea a línea)

| EXE | C++ | Resultado |
|---|---|---|
| 0x68313D..0x683157: `new(0x40)`, ctor 0x632BD0 | `new CTaskComplexSequence` | igual (ctor sin value-init: bytes +0x39..+0x3B no se escriben; el comentario del borrador ya lo decía) |
| 0x683165..0x683195: `new(0x28)`, ctor 0x66B890(partner = [edi+38h], [0x86FC7C], 0x3A83126F) | `new Turn(m_partner, 0.5f, 0.001f)` | `[0x86FC7C]` = 0.5f leído de `.rdata` del EXE canónico (también 0x86FC80 = 0.2f, usado por `CTaskComplexPartner`); 0x3A83126F = 0.001f. Orden de argumentos (partner, mezcla, rumbo máx.) confirmado con la pila de 0x66B890 (`[esi+20h]` ← 0.5, `[esi+24h]` ← 0.001) |
| 0x68319E: `AddTask(seq, turn)` con `turn` posiblemente nulo | `seq->AddTask(new ...)` | igual: el `AddTask` C++ acepta nulo y lo deja en su sitio; con un nulo, la siguiente tarea ocupa la misma posición (modelado en el oráculo) |
| 0x6831AA: `al = [edi+58h]; test al,al` | `m_leadSpeaker ? ... : ...` | igual: el byte se prueba entero (bytes no canónicos 2, 0x80, 0xFF cuentan como verdadero; el banco los prueba) |
| 0x6831CB..0x6831DB: `61A8B0(0, 0xA4, 4.0f, 0)`; 0x6831FA..0x68320A: `(0, 0x8E, 4.0f, 0)` | `CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, ANIM_ID_SHOVE_PARTIAL=164=0xA4 / ANIM_ID_HANDSUP=142=0x8E, 4.0f, false)` | igual (orden de `push`: hold, blend, anim, grupo → el grupo es el primer argumento) |
| 0x683213: `AddTask(seq, run)`; `mov eax,esi` | `AddTask(...); return seq;` | igual |
| orden de las reservas y de las lecturas | — | `[edi+38h]` se lee después de la 1.ª reserva y `[edi+58h]` después del 1.er `AddTask`: ninguna escritura intermedia puede cambiarlos (la función no escribe en `this`; el banco comprueba `this` entero) |
| marco SEH (`fs:[0]`), EBX/ESI/EDI | — | efecto neto nulo; el banco exige `fs:[0]` igual tras la llamada, EBX/ESI/EDI/EBP conservados, ESP, DF |

**Discrepancias halladas: ninguna.** El borrador (que nunca se compiló) era correcto; sólo se añadió un comentario con los dos ids de animación. Riesgo residual de compilación: `new` con `operator new` de clase que devuelve nulo (pool lleno): en MSVC sin `/Zc:throwingNew` la expresión `new` comprueba nulo y omite el constructor, como el EXE (`test eax,eax; je`); el banco lo ejercita (ver familias 12–15). Un pool sin NINGUNA ranura libre no se prueba: el EXE llama a `AddTask` con `this = 0` y se detiene.

**Residuo de registros.** Único llamador del EXE con la tabla virtual: `CTaskComplexPartner::CreateNextSubTask` (0x683AD0), `call [eax+34h]` en 0x683B46 → `jmp 0x6840AC` (`mov ecx,[esp+30h]`, pops, `mov fs:[0],ecx`, `ret 4`). **Nadie lee ECX/EDX tras la llamada** (ECX se sobrescribe; EDX no se usa); sólo se usa EAX (el retorno), que el banco compara siempre como etiqueta (`observed.ret`). Los únicos otros usos de `GetPartnerSequence` en el código de la DLL son las llamadas virtuales de `CTaskComplexPartner.cpp:119`. ECX/EDX de salida sólo se registran (`original_residue`, `candidate_residue`).

## Qué se compara (por lado; original == candidato == oráculo)

Cada caso monta, en memoria propia: un `this` falso de 0x74 bytes con guardas (relleno pseudoaleatorio; sólo se fijan `m_partner` y `m_leadSpeaker`), un partner `CEntity` falso (tipo, flag temporal/procedural e IPL a voluntad, 0..2 referencias previas encadenadas, a veces con la referencia ya registrada) y un **pool de tareas sintético** (bytes de estado con ids de borde, `m_LastFreeSlot`, y qué ranuras están libres; el cuerpo de las ranuras reales se restaura siempre). Se registran en `observed`:
- **ret** (EAX como etiqueta: la 1.ª ranura libre), `last_after` (cursor del pool), **cambios de estado** (índice y byte nuevo: id+1 con el bit 7 limpio, ids 0x7F→0), cada **tarea nueva** completa (0x80 bytes con punteros como etiquetas `W<k>+off`/`E+0`/`N<j>`/`R<i>`; la vtable se sustituye por `GetTaskType`: 0xF4, 0x398, 0x190), que **nada del almacenamiento del pool fuera de las ranuras reservadas cambie** (`pool_outside_ok`), `this` entero intacto, el partner sólo cambia `m_pReferences` (`partner_other_ok`), nodos de referencia previos intactos, la **cadena de referencias** del partner, `pEmptyList` y los **registros de `aRefs` que cambian**.
- ABI: ESP (`ret` normal: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, `fs:[0]` igual (SEH), x87 normalizado (FCW completa, TOP, etiquetas y ST válidos; la función no usa x87 pero los constructores podrían), mundo restaurado (estados, cursor, almacenamiento, `aRefs`, `pEmptyList`, entradas de colaboradores vigiladas).
- Rutas (`route_ok`): original → la entrada 0x683120 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI (el hook `Virtual` parchea también la entrada). `collaborators_dll`: en ambos lados los seis colaboradores saltan a la DLL.

## Colaboradores y alcance honesto

**Ninguno sustituido** (`collaborators_stubbed=false`): todos están ya en C++ y enganchados, así que **el lado «original» también ejecuta esos colaboradores de la DLL** (sólo el cuerpo de `GetPartnerSequence` es el del EXE). Consecuencia: el banco prueba la secuencia, el orden y los argumentos de las llamadas, los fallos de reserva, la lógica de pool y las referencias, **no** los cuerpos de los constructores (validados en sus propios bancos). El oráculo, en cambio, modela las tareas byte a byte desde el desensamblado de los constructores (0x632BD0, 0x66B890, 0x61A8B0, 0x61A390, 0x61A3B0): si falla **sólo** el campo `tasks` en una palabra de relleno o de un campo de un constructor (p. ej. `m_nTaskType` de 2 bytes: el EXE escribe una palabra en +0x1C, no un dword), es un defecto de ese constructor reconstruido y no de esta función; `run_local.py` lo informa con el campo distinto. Si se prefiere no exigir ese nivel, bastaría sustituir `tasks[].words` por una máscara en el oráculo.
`CPool::SetDealWithNoMemory(true)` se activa durante el banco (y se restaura): sin él, la reserva fallida del pool en la DLL ejecuta `__debugbreak`.

## Fixtures (`Fixtures.hpp`): 6144 casos = 16 familias × 384

`n % 16`: 0/1 sin partner (lead 0/1); 2 peatón sin referencias; 3 peatón con 1 referencia previa (1/3 con la referencia ya registrada → no se añade otra); 4 peatón con 2 (igual); 5 vehículo; 6 objeto; 7 edificio sin IPL, ni temporal ni procedural (NO registra); 8 edificio con IPL; 9 edificio temporal; 10 edificio procedural; 11 tipos 0/5/6/7 con flags temporal/procedural/IPL aleatorios; 12 una sola ranura libre; 13 dos; 14 tres o cuatro; 15 vuelta del cursor y libres sólo antes del cursor. En 12–15 el partner es nulo la mitad de las veces. `m_leadSpeaker`: 0 (3/12), 1 (3/12), lista de bordes {2,3,0x10,0x7F,0x80,0x81,0xFE,0xFF} o byte aleatorio. Pool (por caso): natural (10 % libres), todo libre, k libres al azar, k libres justo tras el cursor, cursor al final con libres al principio, libres sólo en/antes del cursor, 50 % libres; cursor en 0, 1, cap−2, cap−1 o aleatorio; ids de estado 0, 1, 0x7E, 0x7F en una de cada 8 ranuras. Semillas de pila, EAX y EDX por caso.

`fixture_mirror.py` (compila sólo `Fixtures.hpp`, sin el juego) confirma 6144 filas aceptadas por el oráculo con pool de 500 ranuras. Mínimos de `run_local.py` calibrados con esas cuentas (≈ 50 % de lo medido): `alloc3` 5254, `alloc2` 448, `alloc1` 442, `registered` 3734, `dup_ref` 253, `skip_building` 412, `search_wraps` 2528, `id_wrap` 653, etc. Si el pool del juego no fuera de 500 ranuras las cuentas varían poco.

## Suposiciones y riesgos del banco

- `CReferences::pEmptyList` no nulo al empezar (si no, `RegisterReference` recorrería los pools de peds/vehículos/objetos para podar: `begin.registered_hook=false`).
- El banco reescribe temporalmente los bytes de estado del pool de tareas y el almacenamiento (restaurados al final de cada lado y comprobados): asume un solo hilo de juego mientras corre (como los demás bancos, todo en un fotograma).
- Offsets del banco verificados con `static_assert` (`sizeof` de ranura 0x80, `CTaskComplexPartnerShove` 0x74, `CReference` 8) y en el oráculo (`m_pReferences` en +0x24); las tablas del pool (`m_Storage`, `m_SlotState`, `m_Capacity`, `m_LastFreeSlot` en +0, +4, +8, +0xC) las usa igual `reversing/CTasks7b`.
- Hash de fuentes probadas: `build_probe.py` lista `TaskComplexPartnerShove.cpp/.h`, `TaskComplexPartner.h`, `TaskComplexSequence`, `TaskComplexTurnToFaceEntityOrCoord`, `TaskSimpleRunAnim`, `Task.cpp`, `Pool.h`, los hooks `Simple`/`Virtual` y los tres archivos del banco; `TaskComplexPartner.cpp` tiene cambios sin commit ajenos y NO está en la lista (el banco no ejecuta su código: la función se llama directamente por 0x683120).
- No hay arnés `offline/`; sólo el oráculo y el espejo del generador se autoprueban.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.py`, `fixture_mirror.cpp`, `register_hook.py`, `source.patch`, `README.md`.
