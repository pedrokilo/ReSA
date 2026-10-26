<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco 65B: cinco ayudantes pequenos del EXE (camara, heli chase, swat, GoPickUpEntity)

Estado: **APROBADO en el juego** (27 136 pares, cero diferencias; ver "Resultado" al final). Sin registrar (lo hace el coordinador con register_hook.py).
Worktree: `D:\Original\wr-2` (HEAD 633dceef0).

## Funciones

| Funcion (nombre del hook) | Direccion | Bytes | Tipo | Fuente |
|---|---|---|---|---|
| `CCamera::IsActiveCamUnderWater` (helper sin simbolo; la usa CameraObbeCinemaCycle y TryToStartNewCamMode) | 0x50B830 | 77 | S | `source/game_sa/Camera.cpp` |
| `CCam::ResetDWHeliChaseParams` (helper sin simbolo; `RH_ScopedNamedGlobalInstall`, funcion estatica `__fastcall`) | 0x50E180 | 184 | S | `source/game_sa/Cam.cpp` |
| `CCam::RandomiseDWHeliChaseParams` (helper sin simbolo; idem) | 0x50E090 | 192 | S | `source/game_sa/Cam.cpp` |
| `CHeli::FindSwatPositionRelativeToHeli` (envoltorio privado `FindSwatPositionRelativeToHeliExeAbi`, `RH_ScopedNamedGlobalInstall`) | 0x6C4760 | 141 | S | `source/game_sa/Entity/Vehicle/Heli.cpp` |
| `CTaskComplexGoPickUpEntity::Constructor` (helper sin simbolo = constructor de la tarea) | 0x6919C0 | 126 | S | `source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp` |
| `CTaskComplexGoPickUpEntity::Destructor` (cuerpo no deleting, 0x691A50; se reconstruye porque ahora los objetos tienen vtable de la DLL) | 0x691A50 | 133 | S | idem |

## Que hace cada una (deducido del desensamblado)

* **0x50B830**: `esi = 0xB6F338 + byte[0xB6F081] * 0x238` = `TheCamera.m_aCams[m_nActiveCam].m_vecSource` (CCam+0x19C). Llama a `CWaterLevel::GetWaterLevel(x, y, z, &level, true, nullptr)`
  (0x6EB690); devuelve `found && !(level < z)` (el `test ah,5 / jp` da verdadero con C0 = C2 = 0, es decir `level >= z`, y tambien con NaN: C0 = C2 = 1).
  Nombre: "la camara activa esta bajo el agua". Sin comprobar el indice del byte (como el EXE).
* **0x50E180**: inicializa con valores por defecto el juego de parametros (0x9C bytes) de la camara cinematografica DW del helicoptero (50, 30, 55, 50, 0.05, 70, 22, 1, 0, 10 en 0x18..0x3C; enteros 30, 8, 60, 60, 30;
  100, 110, 10, 5, 12, 48, 0.75, 4000; bytes 0x40/0x5C/0x80/0x81/0x88 = 0). Termina con EAX = 0, ECX = this, EDX = 0x3C. **El thunk 0x5173F0 (`call 0x50E180; mov eax,ecx; ret`) lee ECX**,
  por eso se reproduce con `ResidueRegsCpp` (EAX/ECX/EDX exactos). La usaba `CCam::Process_DW_HeliChaseCam` por direccion.
* **0x50E090**: aleatoriza cinco parametros: `p[0x18] *= rand()*1/32767*1.4 + 0.1`; `p[0x1C], p[0x20], p[0x24] *= rand()*1/32767*0.5 + 0.5`; `p[0x28] *= rand()*1/32767*1.5 + 0.5`, con `rand` del CRT del EXE (0x821B1E)
  y fmul/fadd x87 a 24 bits. **Defecto previo en el borrador de Cam.cpp**: la llamaba como `cdecl` sin `this`, pero el EXE es `thiscall` (esi = ecx): el llamador 0x51A87A pone `mov ecx,esi`. Corregido.
  El ultimo producto (por el campo) pasa por `FloatTimesFloatCpp` (X87Math.cpp) para conservar los denormales del x87 frente al FTZ del juego; el resto de pasos es identico en SSE (24 bits, rand() <= 32767 exacto).
* **0x6C4760**: devuelve en el puntero oculto el desplazamiento de la cuerda del SWAT: indices 0..3 (comparacion sin signo `cmp eax,3; ja`) -> (-1.2,-1,-0.5), (1.2,-1,-0.5), (-1.2,1,-0.5), (1.2,1,-0.5)
  (la tabla de saltos 0x6C47F0 esta en orden inverso); cualquier otro -> (0,0,0). EAX = puntero oculto, ECX = this (0..3) o 0, **EDX intacto** (el llamador 0x6C7844 lo conserva a traves de la llamada y lo empuja despues),
  por eso el hook es un envoltorio `__fastcall` que termina en `ResidueRegsCpp`.
* **0x6919C0**: constructor de `CTaskComplexGoPickUpEntity` (this, entidad, grupo de animacion): base `CTaskComplex` (0x61A3B0), `m_pEntity` (+0xC), `m_vecPosition` y `m_vecPickupPosition` = (-999, 0, 0),
  `m_nAnimGroupId` (+0x2C), `RegisterReference` (0x571B70) si hay entidad, `m_nTimePassed...` = 0, `m_bAnimBlockReferenced` = 0; los bytes de relleno 0x31..0x33 no se escriben. Devuelve this.
  Llamadores del EXE: 0x46C9C7, 0x570595 (comprueban nulo de `CTask::operator new` antes) y `Clone` 0x692C80; ninguno lee ECX/EDX tras la llamada, solo EAX = this.
* **0x691A50**: destructor (cuerpo): vtable de la clase, `CleanUpOldReference` (0x571A00) de `m_pEntity`, si `m_bAnimBlockReferenced`: `RemoveAnimBlockRef` (0x4D3FD0) del indice del bloque del grupo y flag a 0,
  y `~CTaskComplex` (0x61A3D0: borra la subtarea con su destructor virtual, deja `m_pSubTask = 0` y la vtable base). Su unico llamador del EXE es el destructor deleting 0x6935F0.

Al reconstruir el constructor, los objetos creados desde el EXE (3 llamadores) y desde C++ (`PlayerInfo`, `ProcessPlayerVehicleInteraction`) pasan a tener la vtable de la DLL, por lo que la clase deja de ser abstracta:
destructor, `Clone` (0x692C80, con la comprobacion de nulo de `CTask::operator new`) y `GetTaskType` (0x136) estan en C++; `CreateNextSubTask` (0x691AE0), `CreateFirstSubTask` (0x693610) y `ControlSubTask` (0x691D50)
siguen en el EXE por reenvio (`plugin::CallMethodAndReturn`, como `CTaskComplexLeaveCar`). `MakeAbortable` (0x4211B0) y `SetSubTask` (0x61A430) son los de `CTaskComplex`. Se activa `CTaskComplexGoPickUpEntity::InjectHooks()` en `InjectHooksMain.cpp`.
`PlayerInfoProcess.cpp.inc` construye la tarea con `CTask::operator new` + `::new` y la misma comprobacion de nulo del EXE.

## Fuente (source.patch)

`source.patch` (git diff contra HEAD 633dceef0, verificado con `git apply --check` sobre `git show HEAD:<archivo>` en un directorio temporal) contiene SOLO estos cambios:
`Camera.cpp/.h` (funcion + hook + 5 llamadas por direccion sustituidas), `Cam.cpp` (struct `DWHeliChaseParams`, dos funciones, dos hooks, dos llamadas), `Heli.cpp` (switch, envoltorio, hook),
`TaskComplexGoPickUpEntity.h/.cpp` (clase), `PlayerInfoProcess.cpp.inc` (construccion en C++), `InjectHooksMain.cpp` (activar InjectHooks de la clase).

## Revision propia contra el EXE (fase 2)

Desensamblado releido tras escribir (dis.sh de cada direccion) contra el objeto compilado del proyecto (`dumpbin /disasm` de Heli.obj para el envoltorio: guarda `mov [ebp-8],edx`, comparacion sin signo, ECX/EDX/EAX
cargados justo antes de `call ResidueRegsCpp`, `ret 8`). Discrepancias halladas y corregidas:
1. `Cam.cpp`: 0x50E090 llamada como `cdecl` sin `this` (el EXE es thiscall con ECX = parametros).
2. `Heli.cpp`: el borrador del `switch` era correcto (la tabla de saltos esta invertida: indice 0 -> 0x6C47C1 = (-1.2,-1,-0.5)), pero comparaba el indice con signo; el EXE usa `ja` (sin signo).
3. `Heli.cpp`: la funcion llamaba al EXE y dejaba el `switch` como codigo muerto; ahora es C++ puro.

## Banco

`LiveComparison.hpp` (namespace `Helpers65BLive`), sonda `helpers65b`. Cada funcion se ejecuta en la direccion del EXE con el hook apagado (original) y encendido (candidato) por una puerta de ABI (`Gate`):
64 dwords de pila sembrados, EBX/ESI/EDI centinela, EAX/ECX/EDX sembrados, exige ESP, no volatiles, DF = 0, cadena SEH intacta, **FCW = 0x7F (precision de 24 bits), pila x87 vacia y MXCSR = 0x9F80 (FTZ) tras la llamada**
(el banco fija FCW y MXCSR antes de llamar y restaura el estado previo). Compara arena completa (guardas de 0x20 bytes, relleno pseudoaleatorio por caso), resultado, registros reproducidos a proposito y traza de colaboradores.

| Funcion | Casos | Fixtures | Colaboradores sustituidos (EXE y cuerpo de la DLL) |
|---|---|---|---|
| IsActiveCamUnderWater | 4608 | camara activa 0..2, fuentes de las tres camaras (NaN, inf, +-0, subnormales, enormes), nivel del agua = z, +-1 ulp, NaN, +-inf, subnormal, aleatorio; encontrado/no | `CWaterLevel::GetWaterLevel` (0x6EB690) |
| ResetDWHeliChaseParams | 4096 | 0x9C bytes pseudoaleatorios; EAX = 0, ECX = this, EDX = 0x3C exigidos | ninguno |
| RandomiseDWHeliChaseParams | 6144 | campos 0x18..0x28 con NaN (silenciosos y senalizadores), inf, subnormales, +-0, enormes; rand() en 0, 1, 16383, 16384, 32766, 32767 y aleatorio | rand del EXE (0x821B1E) |
| FindSwatPositionRelativeToHeli | 4096 | indices 0..3, negativos, 4+, INT_MIN/MAX, aleatorios; `this` 0 o aleatorio; EAX = puntero oculto, ECX, EDX exigidos | ninguno |
| CTaskComplexGoPickUpEntity::Constructor | 4096 | entidad nula / dentro de un bufer, grupos 81, 0..63, extremos | `CEntity::RegisterReference` (0x571B70) |
| CTaskComplexGoPickUpEntity::Destructor | 4096 | subtarea nula o falsa (destructor virtual trazado), entidad nula o no, flag 0/1/otro, 64 grupos falsos (bloque real, nulo o desalineado) | `CEntity::CleanUpOldReference` (0x571A00), `CAnimManager::RemoveAnimBlockRef` (0x4D3FD0) |

Total 27 136 pares. Estado global tocado: byte de camara activa (0xB6F081), fuentes de las tres camaras (0xB6F338 + 0x238k, 12 bytes), puntero y contador de la tabla de grupos de animacion (0xB4EA34 / 0xB4EA28):
se guardan, se restauran por caso y se comprueban al final junto con las entradas del EXE de todos los colaboradores y funciones (`state_restored` = regiones restauradas + entradas intactas tras quitar los parches).

### Registros que leen los llamadores del EXE

* 0x50B830: solo AL (`test al,al`, `neg al / sbb al,al / inc al`) en los 7 llamadores; ECX/EDX/resto de EAX solo se registran.
* 0x50E180: **ECX** (thunk 0x5173F0); EAX/EDX se reproducen igualmente (se exigen). 0x50E090: ningun registro (el llamador 0x51A87A hace `mov ecx,...` despues); solo se registran.
* 0x6C4760: **EAX** (puntero oculto, 0x6C6ACB / 0x6C7849 / 0x6C789E) y **EDX** (0x6C7844 lo empuja tras la llamada); ECX se reproduce por completitud.
* 0x6919C0: **EAX** = this (los tres llamadores); ECX/EDX solo se registran. 0x691A50: ninguno (el llamador 0x6935F0 usa ESI).

## Suposiciones y limites

* No hay oraculo independiente: la referencia es el EXE. La aritmetica de Randomise cubre `rand()` <= 32767 (rango real del CRT); el `fild` de un entero mayor de 24 bits podria diferir de `cvtsi2ss` (doble redondeo) pero el CRT no lo produce.
* El constructor no valida el pool ni `CTask::operator new` (lo comprueban los llamadores). El destructor se valida con subtarea falsa (destructor virtual trazado), no con una tarea real.
* Los reenvios de `CreateNextSubTask / CreateFirstSubTask / ControlSubTask` llaman al EXE sin hook y no se validan aqui (son colaboradores no reconstruidos).
* `GetAnimationBlockIndex` (C++) indexa `GetAssocGroups()[grupo]`, igual que el EXE para grupos validos; un grupo fuera de rango no se prueba (el EXE no comprueba).

## Uso

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Helpers65B
python reversing/Helpers65B/run_local.py --analyze <comparisons.jsonl>   # solo analiza
python reversing/Helpers65B/register_hook.py <carpeta-de-evidencia>      # SOLO tras APROBADO (lo ejecuta el coordinador)
```
Filtro opcional: fichero `helpers65b-only.txt` en el clon con `Clase::Funcion`.

## Resultado

APROBADO el 21/10/2026: 27 136 pares (4608 + 4096 + 6144 + 4096 + 4096 + 4096), cero diferencias, `state_restored` (regiones y entradas del EXE intactas), `layout_ok`.
Entorno medido en el juego: FCW = 0x7F, MXCSR = 0x9FB7 (0x9F80 con banderas pegajosas).
Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers65b/20261021-030217-130118`.
Defectos del BANCO hallados por las dos primeras corridas (no de la fuente): FCW fijado a 0x27F (53 bits) en vez de 0x7F (24 bits), que cambiaba el redondeo de la cadena x87 de Randomise; ya corregido.
Defecto REAL hallado: `m_vecPosition`/`m_vecPickupPosition` se inicializan con -999.0f (0xC479C000), no -1000.0f como dedujo la primera lectura del borrador.
Cobertura de trazas: Constructor 2885 casos con RegisterReference y 1211 sin entidad; Destructor 132/1167/1941/856 casos con 0/1/2/3 colaboradores trazados.
