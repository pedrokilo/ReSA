<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco DecisionPedGroup27: CDecisionMakerTypes::MakeDecision Ped (0x606E70) y Group (0x606F80) con CDecision::MakeDecision (0x6040D0) en C++

**Estado: APROBADO** (18/10/2026, corrida `20261018-144321-185827`: 5120 pares (2560 Ped + 2560 Group), 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-decisionpedgroup27\20261018-144321-185827`; archivado en `reversing/CppTwelveLevel6Final/runs/DecisionPedGroup27-20261018-144321-185827/`. Sonda compilada desde el worktree D:\Original\gta-wt-dm2 (HEAD 7a759750f) con fuentes idénticas byte a byte a las del árbol principal. Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`; sustituye como evidencia vigente a `DependenciesDecisionPed7`, que se conserva como `previous_evidence`/`previous`).

Deriva de `reversing/DependenciesDecisionPed7` (banco histórico del 10/10/2026: Ped y Group, 2048 pares cada una, con 0x6040D0 del EXE como colaborador y el puente «Decision7One») y usa como plantilla de estructura `reversing/DecisionMakeDecision27` (llamada con semilla de `srand` del EXE 0x821B11 compartida y comparación del estado de rand). Motivo: desde 7a759750f `CDecision::MakeDecision` (0x6040D0) está en C++ (`Decision.cpp`, hook activo, colaborador 0x6007A0 en línea y `rand` 0x821B1E del EXE por dirección) y los dos sitios de `DecisionMakerTypes.cpp` ya lo llaman en C++: la evidencia histórica de Ped/Group ya no corresponde a la fuente actual.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh DecisionPedGroup27              # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie, sólo en el clon E:\JUEGOS\GTA San Andreas - CVehicle Test:
python -B reversing/DecisionPedGroup27/build_probe.py
python -B reversing/DecisionPedGroup27/run_local.py
python -B reversing/DecisionPedGroup27/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis
python -I reversing/DecisionPedGroup27/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime ok)
```
Requiere el ASI del árbol actual con `RH_ScopedOverloadedInstall(MakeDecision, "Ped"/"Group", ...)` (categoría `CDecisionMakerTypes`) **y** `RH_ScopedInstall(MakeDecision, 0x6040D0)` (categoría `CDecision`). 5120 pares (2 x 2560). Archivo de arranque en el clon: `decisionpedgroup27-runtime.start`; salida `decisionpedgroup27-runtime.jsonl`. Tras una corrida buena falta un `register_hook.py` (no se escribe aquí: depende de la corrida) siguiendo `DecisionMakeDecision27/register_hook.py`; las dos funciones son hooks ya catalogados.

## Rutas

| Lado | 0x606E70 | 0x606F80 | 0x6040D0 | 0x606060 | 0x6007A0 | rand 0x821B1E |
| --- | --- | --- | --- | --- | --- | --- |
| Original | EXE (hook apagado) | EXE (hook apagado) | EXE (hook apagado) | EXE | EXE | EXE |
| Candidato | DLL (C++) sólo la que se prueba; la otra apagada | idem | DLL (C++, 0x6007A0 en línea, rand del EXE por dirección) | EXE | en línea en la DLL; la entrada 0x6007A0 NO está enganchada | EXE |

`HookRoute` se construye para los tres hooks (Ped y Group con el cuerpo explícito de `MakeDecision`; 0x6040D0 con `RegisteredTarget`) y `Invoke` los anida: `decision.Invoke(cand, [&]{ entrada.Invoke(cand, llamada) })`. `route_ok` (por lado y por caso) exige: original = ninguno de los tres salta a la DLL; candidato = la entrada probada y 0x6040D0 saltan al ASI y la otra entrada no; en ambos 0x606060 y 0x6007A0 no están enganchados. Antes de empezar (con los tres hooks apagados) se exigen 16 bytes canónicos del EXE (extraídos de `gta-sa.exe` canónico) en 0x6040D0, 0x606060, 0x6007A0, 0x821B1E, 0x821B11, 0x827B3D, 0x606E70 y 0x606F80; se vigilan además 0x606060, 0x6007A0, 0x821B1E, 0x821B11, 0x827B3D y los 11 bytes del cuerpo de cada una de las tres entradas tras el parche de 5 bytes. La ruta -1 de Ped (decision maker «por defecto») pasa por 0x606060 del EXE, que a su vez llama a 0x6040D0: en el candidato esa llamada entra a la DLL por el hook de 0x6040D0 (es la composición que se valida y hace que 0x606060 sea el mismo colaborador en ambos lados). Ningún colaborador se sustituye (`collaborators_stubbed=false`).

## Qué se compara (por lado, en bits; original == candidato == oráculo)

- Decisión seleccionada (60 bytes), maker/ped/intel/group completos sin cambios (`objects_unchanged`), canarios de las salidas int16 (en Ped, `outputs[0]` y `outputs[3]`; en Group, las cuatro), `task_slot`/`face_slot` de Ped (int16 con signo), EAX de retorno de Group (`return_eax`, `movsx` de la ranura de pila del primer argumento).
- **Estado del RNG del EXE tras la llamada** (`rng_state`) y número de `rand` consumidos (`rand_calls`: pasos del LCG desde la semilla; 0 si no hay opciones o `seek` se encuentra, 1 si hay tirada, −1 = estado inesperado). Ambos lados arrancan con la MISMA semilla fijada con `srand` del EXE (0x821B11) justo antes de la llamada; el estado se restaura tras cada lado y al final.
- ESP (`ret 2Ch` / `ret 20h`), EBX/ESI/EDI sembrados y conservados, EBP, cabeza de la cadena SEH (`fs:[0]`), DF = 0, x87 normalizado como en `DecisionMakeDecision27` (FCW completa, TOP, etiquetas y ST válidos; el EXE ejecuta x87 en 0x6040D0 y la DLL puede usar SSE), MXCSR sin banderas, restauración del x87 completo, del RNG y de las rutas.
- EAX/ECX/EDX: se **siembran** como en el banco original (EAX = 0xA5123456, EDX = 0xED123411 ^ caso, ECX = this) y se **registran** (`original_residue`, `candidate_residue`, con EFLAGS) pero no se comparan, salvo el EAX de Group (retorno). `begin` registra `compare_volatile_registers=false` y `compare_group_return_eax=true`; `run_local.py` lo exige.

## Análisis de llamadores (justifica el contrato de registros)

Herramienta: `python -I reversing/Dependencies25A/residue_scan.py 0x606E70` y `... 0x606F80` (escáner con capstone sobre `gta-sa.exe` canónico, E8/E9 en `.text` 0x401000..0x857000) más desensamblado a mano de los llamadores y de lo que consumen tras el `call` (`dis.sh` de `_handoff` usa `dumpbin //range` y falla aquí con «número no válido»; se usó capstone con las mismas direcciones). Además se buscó en la imagen del EXE el dword 0x606E70/0x606F80 como puntero (vtables/tablas): sin coincidencias, no hay llamadores indirectos.

| Destino | Llamador | Tras el `call` | Lee EAX/ECX/EDX? |
| --- | --- | --- | --- |
| 0x606E70 Ped | 0x4B5720 (`CEventEditableResponse::ComputeResponseTaskType(CPed*)`, 0x4B56C0, `ret 8`, `void`) | `pop edi/ebx/esi/ebp; ret 8` | No |
| 0x606E70 Ped | 0x4B5783 (función 0x4B5730, `ret 8`) | `movsx eax,word ptr [esp+0Ch]` (escribe EAX); `xor ecx,ecx` (escribe ECX); `sete cl; mov al,cl` | No: EAX y ECX se sobrescriben; EDX no se usa |
| 0x606F80 Group | 0x4B5841 (`ComputeResponseTaskType(CPedGroup*)`, 0x4B57A0, `ret 4`, `void`) | `mov word ptr [edi+0Eh],ax` | **EAX sí: es el valor de retorno** (tarea `movsx`). ECX se reescribe (`mov ecx,ebp`). EDX llega sin escribir a `call 0x5F69A0` (sólo lee ECX), `call 0x5DF8F0` (sólo lee ECX) y `call 0x4ABAC0` (`ComputeEventSourceType`: carga ECX de la pila y llama a la virtual `GetSourceEntity`, thiscall, EDX no es argumento): ninguno lo lee |
| 0x606F80 Group | 0x4B58C3 (misma función) | `mov word ptr [edi+0Eh],ax; pop esi/ebp/edi; ret 4` | **EAX sí (retorno)**; ECX/EDX no |

Conclusión (por función):
- **Ped:** nadie lee EAX, ECX ni EDX (función `void`) → sólo registrarlos, como en `DecisionMakeDecision27`.
- **Group:** EAX es el valor de retorno y sí lo leen los dos llamadores → **sigue exigido** (en `observed.return_eax`; el candidato lo produce por contrato, no por residuo). ECX y EDX: nadie los lee → sólo registrarlos.
- Los residuos distintos que dejan EXE y DLL (el EXE conserva los del colaborador 0x6040D0 o 0x606060; la DLL ya no) no son observables por ningún llamador. Se avisa de un único punto que sólo un revisor puede cerrar: en 0x4B5841 el EDX de salida de 0x606F80 sobrevive hasta la llamada virtual `GetSourceEntity` dentro de 0x4ABAC0; ese getter es thiscall sin argumentos y no se espera que lea EDX, pero no se ha desensamblado cada override. Si algún override lo leyera habría que dejar EDX de Group exigido y reproducirlo en C++ con el truco `ResidueRegsCpp`.

## Fixtures (2560 por función; `Fixtures.hpp`)

- **Casos 0..2047 (familia 0): mismo reparto que el banco original** (fórmulas por `scenario` s = índice): selector `dmIds[(s/8)%6]` = {−2,−1,0,1,7,19} y el otro selector el siguiente; `eventId=(s/8)%96`, `eventIndex=(s/4)%41`, `sourceType=s%4`, `inVehicle=(s/4)%2`; `avoid1=1001`, `avoid2=1002` si `s&64`, `avoid3=1003` si `s&128`, `seek=1004` si `s&256`; Ped: tipo ped (+0x484) = 2 si `s&16` (si no 1), `useGroup = s&32` (usa B8 en vez de B4); Group: id −2 → −1, byte +0x2D0 = 1 si `s&16`. Las seis opciones: tarea `1000+n` (o −1 si `s&512`), probabilidades `1+(n+r+s)%99` (o 0 si `s&1024`), bools `(s+n)&1` y `((s/2)+n)&1`. Maker completo relleno de basura (semilla `s*2654435761+0x12893211`), índices de evento y decisión escritos encima; estado inicial de rand pseudoaleatorio.
- **Casos 2048..2559 (familias 1..8, 64 cada una): rand.** Enrutado con las mismas fórmulas, decisión construida a mano con opciones válidas (`tarea 2000+7*ranura`) y filtradas por una razón (probabilidad 0, tarea −1, tarea = avoid1, bool del tipo de vehículo a 0), permutadas entre ranuras, y la tirada colocada eligiendo el estado inicial del RNG (`SeedForRoll`, inversión del LCG, `static_assert` en el encabezado): 1 umbral ±2 (2..6 opciones); 2 tirada 32767 con totales «malos» 41/47/55/61 (`roll=1.0f` > acumulado 0.99999994 → `taskSlot=200`); 3 tirada 32767 con otros totales (incl. 1530 = seis a 255); 4 tirada 0; 5 **seek hallado** (no consume rand); 6 seek no hallado (inexistente, filtrado por avoid1 o por probabilidad 0) con tirada aleatoria; 7 tirada aleatoria con 1..6 opciones; 8 seis opciones 1/255 con umbral ±2.
- Registros: EAX/EDX/ECX y 64 dwords de pila sembrados por caso (como el banco original y `DecisionMakeDecision27`).

`fixture_oracle.py` (opcional, incluido) modela el despachador desde el desensamblado (selector, desplazamiento de la decisión, `movsx` de Group) y delega el cálculo de 0x6040D0/0x6007A0/rand en el oráculo aprobado de `DecisionMakeDecision27` (x87 a 24 bits con `Fraction`); verifica además el desplazamiento seleccionado que registra el banco contra su propia fórmula. `run_local.py` exige: gates por lado, `fp_difference` vacío, `observed` de cada lado == oráculo, rand 0 con sin-opciones/seek hallado y 1 con tirada, cobertura mínima (familias 1..8 con 64 casos, los seis selectores de Ped, los cinco ids de Group, misión/aleatorio, `rand1`, seek hallado/no hallado, `roll_max`, `roll_zero`, totales 41/47/55/61 con 32767, `miss_total_roll1`, umbral, selección en cada posición 0..5, `old_double_style_differs`, total 1530, sourceType 0..3, vehículo/no vehículo, avoid1/2/3) y la restauración final.

## Suposiciones

- `CDecisionMakerTypes::MakeDecision` Ped/Group son miembros públicos no virtuales y `FunctionToVoidPtr(static_cast<...>(&...))` compila como en `DependenciesDecisionPed7`; los nombres RH son «MakeDecision-Ped» y «MakeDecision-Group» (categoría `CDecisionMakerTypes`) y `CDecision::MakeDecision` (categoría `CDecision`).
- `sizeof(CDecision)==0x3C`, offsets del maker (índices de evento en 0xC034..0xC1B4; decisiones propias 4+(id*41+idx)*60; jugador 0xD4EC; aleatorio ped 0xC1B4, misión ped 0xCB50; aleatorio grupo 0xDE88, misión grupo 0xE824) y offsets de ped/intel/group (+0x47C, +0x484, +0xB4, +0xB8, +0x2C8, +0x2D0) son los del banco original; no se comprueban contra los tipos del juego.
- `rand` 0x821B1E se invoca vía `srand` 0x821B11 y el estado se lee en `[0x827B3D()+0x14]` en el hilo del juego (misma técnica que el banco original y que `DecisionMakeDecision27`).
- La ruta de Ped con id −1 consume exactamente un `rand` (0x606060 llama a 0x6040D0 una vez); el oráculo y `rand_calls` lo exigen.
- El `DecisionMakerTypes.cpp` del árbol contiene además el cambio de `AddEventResponse` (0x6044C0 → 0x600600) que este banco no ejercita ni valida; los hashes de `build-manifest.json` incluyen ese archivo tal como esté al compilar.

## Riesgos

- **Nada de C++ se ha compilado.** Posibles errores de compilación en `LiveComparison.hpp` (ensamblador del `Gate`, listas de inicialización de `nlohmann::json`, asignación de `std::array` desde lista de llaves, ambigüedad `u8/u32` con los typedefs del juego — `DecisionMakeDecision27` usa el mismo patrón) o de `Fixtures.hpp`.
- La cobertura mínima de `run_local.py` se calibró con un portado a Python del generador (en el scratchpad de la sesión, no se entrega) y no con la salida del C++ real; una divergencia entre ambos podría hacer fallar un mínimo por poco (bajar el umbral o ajustar la familia).
- Frente al banco original se retiran los casos de GetRef y del destructor de `CEventScriptCommand` (otras dos funciones del mismo banco, fuera de esta tarea) y la prueba de desenrollado SEH; la semilla de rand y el relleno del maker usan otras funciones pseudoaleatorias, aunque con el mismo reparto estructural.
- Los dos hooks de `DecisionMakerTypes` se validan junto con el hook de 0x6040D0 encendido; un fallo puede atribuirse a cualquiera de los dos cuerpos. Si ocurre, repetir con la entrada de 0x6040D0 apagada (ruta del banco histórico) separa ambos.
- `HookRoute::Restore` devuelve los hooks a su estado inicial (encendidos en producción): los bytes finales de las tres entradas son los del hook, y el control final se hace con las rutas y los bytes vigilados, no con los canónicos.
- El EDX de Group descrito en «Análisis de llamadores».

## Archivos

`Fixtures.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `README.md`. No hay `register_hook.py` ni `validation-summary.json` (se generan al aprobar la corrida).
