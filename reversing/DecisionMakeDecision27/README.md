<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco DecisionMakeDecision27: CDecision::MakeDecision (0x6040D0)

**Estado: APROBADO** (18/10/2026, corrida `20261018-140521-969916`: 6144 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-decisionmakedecision27\20261018-140521-969916`; archivado en `reversing/CppTwelveLevel6Final/runs/DecisionMakeDecision27-20261018-140521-969916/`. Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila `CDecision,MakeDecision,0x006040D0` de `docs/hooks.csv`).

**Desajuste de fuentes probadas.** La sonda se compiló en el worktree limpio `D:\Original\gta-wt-decision` (HEAD 0688182ff + `Decision.cpp`/`Decision.h` + `DecisionMakerTypes.cpp` SIN el cambio de `AddEventResponse` + banco). El `DecisionMakerTypes.cpp` del árbol principal contiene además un cambio AJENO de `AddEventResponse` (0x6044C0 -> llamada a 0x600600) que NO está validado ni forma parte de este banco. Los hashes de `artifacts/build-manifest.json` registrados corresponden a la versión del worktree; `DecisionMakerTypes.cpp` no figura entre ellos y no debe darse por probado el del árbol principal. Los dos sitios de llamada de `CDecisionMakerTypes::MakeDecision` (Ped/Group) no están cubiertos por este banco.

Plantilla: `reversing/ModifySpeed27` (Core/Fixtures/LiveComparison/build_probe/run_local/fixture_oracle) y contrato de `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.). Dominio: `DependenciesDecisionPed7` / `DependenciesDecisionRelationship6` (usaban 0x6040D0 como colaborador sin probarlo).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh DecisionMakeDecision27          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/DecisionMakeDecision27/build_probe.py
python -B reversing/DecisionMakeDecision27/run_local.py
python -B reversing/DecisionMakeDecision27/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis
python -I reversing/DecisionMakeDecision27/fixture_oracle.py                                              # autoprueba del oráculo (sin juego)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(MakeDecision, 0x6040D0)` (categoría `CDecision`). 6144 pares. Archivo de arranque en el clon: `decisionmakedecision27-runtime.start`; salida `decisionmakedecision27-runtime.jsonl`.

## Qué hace la función (desensamblado 0x6040D0..0x60421A y 0x6007A0..0x60083A)

Pila tras el prólogo: `sourceType`=+74h, `inVehicle`=+78h (**sólo el byte bajo**: `mov al,[esp+78h]`), `avoid1/2/3`=+7Ch/+80h/+84h, `seek`=+88h, `&taskSlot`=+8Ch, `&facialSlot`=+90h; `ret 20h`.

1. `tasks[6]=200`, `probs[6]=0`; `ebp=this+0x30+(inVehicle!=0)`; `edi=this+0x18+sourceType` (**sin comprobar límites**).
2. Para i=0..5: `p=byte[edi+4i]` (0 → salta); `t=dword[this+4i]` (`-1`, `avoid1`, `avoid2`, `avoid3` → salta); `byte[ebp+2i]==0` → salta; si pasa, `tasks[n]=t; probs[n]=p; n++`.
3. `word[taskSlot]=200`. `n<=0` (con signo) → `word[facialSlot]=-1` y fin: **sin x87 ni rand**.
4. x87 a 24 bits: `st0=0.0f`; `st0=r24(st0+probs[i])` (fiadd) y `cumul[i]=float(st0)` para las SEIS entradas (las ≥n repiten el total); `inv=r24(1.0f/total)`; `cumul[i]=float(r24(inv*cumul[i]))`.
5. Llama a **0x6007A0** (`ecx=&tasks[6]` con el contador 6 en +30h y `cumul` en +18h; args `seek, &taskSlot, &dummy`, `ret 0Ch`): `word[taskSlot]=200`; `seek!=-1` → busca en las 6 `tasks` (incluido el relleno 200); si la encuentra pone `low16(tasks[i])`. Si `word[taskSlot]!=200` termina **sin rand**. Si no: `r=rand()` (0x821B1E), `roll=float(r24(r*0x38000100))` y toma el primer `i` con `roll<=cumul[i]` (`fcomp`+`test ah,41h`+`jnp`) → `word[taskSlot]=low16(tasks[i])`; ninguno → 200.
6. `word[facialSlot]=-1` y `ret 20h`. Escribe además un dword (-1/índice) en el slot de pila del arg2 (no observable).

Sólo se guardan dos salidas observables: `taskSlot` y `facialSlot`, más el estado del RNG del EXE. Pila x87 vacía al volver. EBX/ESI/EDI/EBP se guardan/restauran.

`rand` del EXE (0x821B1E): `estado=estado*0x343FD+0x269EC3` (tiddata+0x14, `0x827B3D`), devuelve `(estado>>16)&0x7FFF`; `srand` del EXE = 0x821B11. Con `r=32767`, `roll = 1.0f` (producto 32767·0x38000100 = 1-2^-30 → 1.0 a 24 bits).

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- Objeto `CDecision` completo (0x3C bytes, no debe cambiar) y guardas antes/después en su arena; arena de salidas de 32 bytes con `taskSlot`/`facialSlot` (int16) y guardas alrededor (sólo se pueden escribir 2+2 bytes); valores iniciales aleatorios de ambos slots (para ver si se escriben).
- **Estado del RNG del EXE tras la llamada** (`rng_state`) y número de `rand` consumidos (`rand_calls`: 0 si `count<=0` o si `seek` se encuentra entre las tareas válidas; 1 si hay tirada; −1 = estado inesperado). Ambos lados arrancan con la MISMA semilla fijada por `srand` del EXE (0x821B11) justo antes de la llamada; el estado se restaura después de cada lado y al final.
- ESP (`ret 20h`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, x87 normalizado (FCW completa de 24 bits, TOP, tags y ST válidos: no debe quedar ninguno; se ignoran FSW banderas/CC, FIP/FDP y registros vacíos; MXCSR sin banderas), `begin` registra `compare_dead_x87_registers=false` y run_local lo exige.
- **EAX/ECX/EDX de salida: sólo se registran** (`original_residue`, `candidate_residue`), `kCompareVolatileRegisters=false`. `python -I reversing/Dependencies25A/residue_scan.py 0x6040D0` da 6 llamadores (0x606093, 0x606EE5, 0x606F69, 0x606FDF, 0x606FF4, 0x607036) y ninguno lista lecturas de EAX/ECX/EDX tras el `call`; el revisor debe confirmarlo a mano (la función es `void`).
- Rutas: `route_ok` exige en el original que 0x6040D0 no salte a la DLL y en el candidato que sí (módulo ASI) y que **0x6007A0 no esté enganchado** (el EXE lo ejecuta; la DLL lo lleva en C++). Se vigilan 16 bytes de 0x6007A0, 0x821B1E, 0x821B11 y 0x827B3D. **No se sustituye ningún colaborador** (`collaborators_stubbed=false`): se prueban 0x6007A0 y rand de verdad.

## Fixtures (6144 casos; `Fixtures.hpp`)

16 familias (`kind = n % 16`, 384 casos cada una): 0 vacío; 1..6 exactamente k opciones válidas; 7 totales «malos» con tirada 32767 (41, 47, 55, 61, 82, 83, 94, … donde `r24(r24(1/T)*T) < 1`, por lo que `roll=1.0f` no acierta y `taskSlot=200`) y otros totales; 8 umbrales intermedios (tirada ≈ acumulado/total·32767 ± 2); 9 seis opciones a 255 (total 1530); 10 mezcla 0/1/255; 11 las seis variantes de `seek` (ausente, hallado, no hallado, tarea filtrada/avoid, =200, 0x100C8); 12 tareas iguales a avoid1/2/3; 13 bools por vehículo y byte bajo de `inVehicle` (0x100, 0xFFFFFF00 → falso; 2, 0x80, 0xFFFFFF01 → verdadero) y bytes de bool no canónicos; 14 `sourceType` fuera de rango (−24..23: las lecturas de `probs` quedan dentro del objeto, leyendo tareas o bools como probabilidades); 15 tareas extremas (INT_MIN, 0x7FFFFFFF, 0xFFFF, 200, 199, 0x100C8 …).

- Opciones válidas/filtradas: cada fila filtrada lo está por UNA razón (prob 0, tarea −1, tarea = avoid1/2/3, bool del tipo de vehículo a 0); las columnas no usadas llevan valores aleatorios 0/1/255/aleatorio. Tareas repetidas entre filas válidas (el seek halla la primera).
- RNG: la tirada deseada `r` (0, 1, 32766, 32767, umbral ±2, aleatoria) se coloca eligiendo la semilla (`SeedForRoll`, inversión del LCG; `static_assert` en el encabezado). Los casos que no consumen `rand` también llevan semilla y el estado debe quedar intacto.
- Registros: semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`.

`fixture_oracle.py` modela la función desde el desensamblado con `Fraction` (x87 a 24 bits: un redondeo por operación, `1/total` redondeado y luego el producto) y comprueba cada lado; `run_local.py` exige cobertura mínima (cuentas 0..6, `rand` 0/1, seek hallado/no hallado, avoid1/2/3, bool por vehículo, sourceType 0..3 y fuera de rango, `roll_max`, totales 41/47/55/61 con 32767, selección en cada posición, sin acierto, `old_double_style_differs`, total 1530, etc.). **Los mínimos no se comprobaron contra el generador (no se compiló): si alguno falla por poco, bajar el umbral o ajustar la mezcla de la familia.**

## Fuente actual (revisada por el coordinador)

`Decision.cpp` ya usa float para la normalización (`inverse = 1.0f/total`, `cumul[i] = inverse*cumul[i]`: dos redondeos a float = x87 24 bits) y llama al rand del EXE por dirección (`0x821B1E`). Con la versión anterior (`1/total` en double y `rand()` de la DLL) el oráculo prevé diferencias: 293 488 de 1 171 215 pares (total, acumulado) difieren; p. ej. total 41: EXE `cumul=0.99999994`, con tirada 32767 → `roll=1.0f` sin acierto → `taskSlot=200`; la `rand()` de la DLL deja sin avanzar el estado del EXE (`rng_state` y `rand_calls` lo delatan). El contador `old_double_style_differs` mide cuántas fixtures distinguen el estilo anterior.

## Suposiciones a vigilar al compilar

- `Decision.h` (con `notsa::mdarray`) y `CutsceneMgr.h` se incluyen desde `LiveComparison.hpp`; el hook es `Simple` en `CDecision::MakeDecision` (`RegisteredTarget`).
- `rand` del EXE se invoca con `srand` 0x821B11 (cdecl, 1 arg) y el estado se lee en `[0x827B3D()+0x14]` (misma técnica que `DependenciesDecisionPed7`); el hilo de la sonda es el del juego.
- `sizeof(CDecision)==0x3C`; el banco trabaja con ese número a mano. Los bools no canónicos (2, 0x80, 0xFF) no existen en el juego pero ambos lados sólo comprueban `!= 0` leyendo bytes crudos.
- El hook de la DLL recibe `inVehicle` como `bool` (byte): si el compilador lo leyera como dword los casos 0x100/0xFFFFFF00 lo delatarían.
- `_controlfp(_PC_24|_RC_NEAR, …)` por lado deja el x87 a 24 bits (estado del juego); el x87 de la sonda se restaura con `frstor` tras cada lado.
- No hay arnés `offline/`; sólo el oráculo se autoprueba.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `README.md`.
