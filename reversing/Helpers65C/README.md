<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Helpers65C: cinco funciones pequeñas del EXE (lote 65C)

**Estado: APROBADO en el juego** (corrida `20261021-024609-331783`: 20480 pares = 5 funciones × 4096, 0 diferencias, estado, partidas y configuración restaurados). Evidencia en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helpers65c\20261021-024609-331783`. Worktree `D:\Original\wr-3` (HEAD 633dceef0 + `source.patch`). Plantillas: `reversing/ObjectCtor28` (constructor, vptr propio, falsos en EXE y DLL), `reversing/CameraShake30` (falsos `__fastcall`, globales del juego guardados/restaurados) y `reversing/FxInterp61`; contrato: `reversing/_handoff/BRIEF.md`, `BRIEF28.md`, `BRIEF60.md`.

| Función | Dirección | Hook RH (clase / nombre) | Tipo | Fuente |
|---|---|---|---|---|
| `Fx_c::TriggerWaterHydrant(const CVector&)` | 0x4A0D70 | `Fx_c` / `TriggerWaterHydrant` | S | `source/game_sa/Fx/Fx.cpp` |
| `Fx_c::TriggerTankFire(const CVector&, const CVector&)` | 0x4A0FA0 | `Fx_c` / `TriggerTankFire` | S | `source/game_sa/Fx/Fx.cpp` |
| `Fx_c::AddTyreBurst(const CVector&, const CVector&)` | 0x49F300 | `Fx_c` / `AddTyreBurst` | S | `source/game_sa/Fx/Fx.cpp` |
| `CProjectile::CProjectile(int32)` (helper sin símbolo 0x5A4030) | 0x5A4030 | `CProjectile` / `Constructor` | S | `source/game_sa/Entity/Object/Projectile.{h,cpp}`, `ProjectileInfo.cpp`, `InjectHooksMain.cpp` |
| `CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB(CAnimBlendAssociation*, void*)` (helper sin símbolo 0x63C840) | 0x63C840 | `CTaskSimpleCarDrive` / `FinishAnimCloseDoorRollingCB` | S | `source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.{h,cpp}` |

Filas de `docs/hooks.csv` que debe añadir el coordinador (ninguna es virtual): `Fx_c,AddTyreBurst,0x0049F300,1,0,1,S`, `Fx_c,TriggerTankFire,0x004A0FA0,1,0,1,S`, `Fx_c,TriggerWaterHydrant,0x004A0D70,1,0,1,S`, `CProjectile,Constructor,0x005A4030,1,0,1,S`, `CTaskSimpleCarDrive,FinishAnimCloseDoorRollingCB,0x0063C840,1,0,1,S`.

## Comandos

```
bash reversing/_handoff/bench_here.sh Helpers65C          # compila la sonda + corre en el juego (candado global) + resume
python -B reversing/Helpers65C/build_probe.py             # solo la sonda
python -B reversing/Helpers65C/run_local.py               # solo la corrida
python -B reversing/Helpers65C/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # solo análisis (None si el registro aún no existe o no terminó)
python -I reversing/Helpers65C/fixture_oracle.py          # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/Helpers65C/fixture_selfcheck.py       # compila fixture_dump.cpp (Fixtures.hpp tal cual) con cl, vuelca las 20480 filas, compara las constantes de arena con las del oráculo y la cobertura con run_local.MINIMUMS (imprime «ok»)
python -B reversing/Helpers65C/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers65c/20261021-024609-331783"   # solo el coordinador (SCOPE_NOTE ya relleno)
```
Archivo de arranque en el clon: `helpers65c-runtime.start`; salida `helpers65c-runtime.jsonl`.

## Qué hace cada función (desensamblado) y fuente C++

**0x4A0D70 `TriggerWaterHydrant`** (108 B, `ret 4`). `cámara = TheCamera.m_matrix ? m_matrix+0x30 : &TheCamera.m_placement.m_vPosn`; `d = cámara − pos`; `s = (dx² + dz²) + dy²` (x87 a 24 bits); si `s > 625.0` (0x85A6E8; con NaN no retorna) termina; si no, `CreateFxSystem("water_hydrant" @0x85A70C, pos, nullptr, false)` (0x4A9BE0) y, si el resultado no es nulo, `PlayAndKill` (0x4AA3D0). Es idéntica en forma a `TriggerWaterSplash/BulletSplash/FootSplash` ya reconstruidas: C++ = `CreateFxWithinCameraRange("water_hydrant", pos, 625.0f)`. Para no duplicar la prueba de rango se extrajo el helper `IsFxWithinCameraRange(pos, range)` (sin cambio de comportamiento para las otras tres).

**0x4A0FA0 `TriggerTankFire`** (195 B, `ret 8`, `ebx = this`). Mismo filtro de distancia; `M = g_fxMan.FxRwMatrixCreate()` (0x4A9440); `this->CreateMatFromVec(M, &pos, &dir)` (0x49E950); `CreateFxSystem("tank_fire" @0x85A734, &(0,0,0), M, false)`; si no es nulo: `CopyParentMatrix` (0x4AA890) y `PlayAndKill`; `FxRwMatrixDestroy(M)` (0x4A9460).

**0x49F300 `AddTyreBurst`** (199 B, `ret 8`). Mismo filtro; **no usa `this`**: lee el `Fx_c` global (`[0xA9AE24]` = `g_fx.m_SmokeII3expand`, recargado en cada vuelta); `mult = FxPrtMult_c(1, 1, 1, 0x3ECCCCCD, 0x3DF5C28F, 0, 0x3DCCCCCD)` (ctor 0x4AB290); 4 vueltas `i = 0..3`: `AddParticle(posn, velocity, i·0x3D4CCCCD, mult, −1, 1.2f, 0.6f, false)` (0x4AA440).

**0x5A4030 `CProjectile::CProjectile(int32 modelId)`** (167 B, `ret 4`, EAX = this; helper que usaba `CProjectileInfo::AddProjectile`). `CObject::CObject()` (0x5A1D10); `[+0x1C] |= 0x80000000` (`m_bTunnelTransition`); vptr 0x867030; `+0x8C = +0x90 = 1.0`, `+0x98 = 0x3F7FFF58`, `+0x9C = 0.75`, `+0xA0 = 0x3A51B718`; `CObject::SetIsStatic(false)` (0x5A0760, llamada directa); `[+0x40] |= 0x800000` (`bExplosionProof`); `CEntity::SetModelIndex(modelId)` (0x532AE0, directa); `byte [+0x13C] = 5` (`OBJECT_TYPE_DECORATION`). C++: el constructor ya existía en `Projectile.cpp` sin hook; se añadió la envoltura `Constructor` (patrón de `CObject::Constructor`), `CProjectile::InjectHooks` (registrada en `InjectHooksMain.cpp`) y `CProjectileInfo::AddProjectile` pasó de `((…)0x5A4030)(memory, model)` a `::new(memory) CProjectile(model)` (el EXE tampoco comprueba nulo del constructor, solo de la memoria).

**0x63C840 `CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB`** (108 B, cdecl `ret`, `(CAnimBlendAssociation*, void* data = tarea)`; helper usado por `StartRollingDoor`). `byte [task+0x5C] &= 0xDF` (`m_b20 = false`); `v = task->m_pVehicle`; `task->m_pAnimCloseDoorRolling = nullptr`; si `v`: `v->ClearGettingOutFlags(1)` (0x6D3060); **relee** `v = task->m_pVehicle`; si `v->m_pDriver`: `grupo = CVehicleAnimGroupData::GetVehicleAnimGroup(v->m_pHandlingData->m_nAnimGroup).GetGroup(ANIM_ID_CAR_ROLLDOOR = 0x182)` (0x6E3B00); **relee** `v = task->m_pVehicle`; `v->ProcessOpenDoor(v->m_pDriver /*releído*/, 10, grupo, 0x182, 1.0f)` por la ranura 28 (0x70) de la vtable. Nombre elegido: el del callback que `StartRollingDoor` (0x642700) registra con `SetFinishCallback`; `StartRollingDoor` ahora pasa `FinishAnimCloseDoorRollingCB` en lugar de la dirección 0x63C840.

## Revisión propia contra el EXE (fase 2) y defecto real hallado

Se desensambló cada función completa y el borrador de `wf-5` (Fx) y se contrastó línea a línea; después se compararon por el banco. Resultado:

1. **Defecto real (constructor): la constante de flotación.** El C++ previo de `CProjectile` fijaba `m_fBuoyancyConstant = 1.0F / 1250.0F`, que el compilador evalúa como `0x3A51B717`; el EXE guarda `0x3A51B718` (`mov dword ptr [esi+0A0h], 3A51B718h`). Corregido con `std::bit_cast<float>(0x3A51B718u)` (el resto: 0x3F7FFF58 = `0.99999F`, 0.75, 1.0, 1.0 coinciden). El constructor no tenía hook ni banco; era otro defecto latente del fuente.
2. Las tres rutinas Fx: orden de operandos de la distancia `(dx² + dz²) + dy²` (el valor de `fcomp` solo decide `> 625`, y las diferencias entre float SSE y x87 de 24 bits con exponente extendido —desborde, subnormales— solo ocurren en magnitudes que ya superan 625 o en términos que no mueven la suma; el banco las cubre con familias de desborde, infinitos, NaN, subnormales y diferencias a ±ulps del 625), orden de las llamadas, `AddTyreBurst` independiente de `this`, `ret 8` y `ret 4`. Sin discrepancias: se dejó la compilación por defecto del proyecto (no hizo falta x87 por archivo, a diferencia del borrador de wf-5).
3. Callback: los **tres relecturas de `[task+8]`** (tras `ClearGettingOutFlags`, tras `GetGroup` y el conductor releído del último vehículo) se reproducen y el banco las ejercita con falsos que re-apuntan la tarea a otro vehículo o cambian el conductor entre llamadas (`veh_changed`: 367 casos); es cdecl (el llamador retira 8 bytes: `popBytes = 8` en el `Gate`); el segundo argumento (`void*`) es la tarea, el primero (asociación) se ignora.

Al instalar el hook, `StartRollingDoor` y `AddProjectile` ya no llaman por dirección a 0x63C840 / 0x5A4030.

### Llamadores del EXE y registros de salida

`residue_scan.py`: 0x4A0D70 (5 llamadores: 0x548F6D, 0x548F80, 0x54A860, 0x5678F2, 0x567902), 0x4A0FA0 (0x6AF0F5), 0x49F300 (0x736445, 0x73BD62) y 0x63C840 (sin `call` directo: solo se registra como puntero de callback en 0x642700) **no leen EAX/ECX/EDX** (los avisos `unwritten=edx` son EDX/ECX que entran como argumento de otra llamada, que no los lee). Los tres llamadores de 0x5A4030 (0x7383E5, 0x7384D0, 0x73853C, todos dentro de `CProjectileInfo::AddProjectile` del EXE) **leen EAX (= this)**: se exige aparte (`observed.eax_is_this` en ambos lados); no leen ECX/EDX. Por eso `kCompareVolatileRegisters = false` y los residuos solo se registran (`original_residue`, `candidate_residue`).

## Qué se compara (por lado, en bits; original == candidato == oráculo)

- **Traza plana de todos los colaboradores** (evento + argumentos leídos en el momento de la llamada; punteros de la arena como etiqueta `1 + desplazamiento`, `ECX` de los colaboradores de `g_fxMan` = 0xA9AE80). Cualquier llamada de más, de menos, en otro orden, con otro argumento (incluidas las constantes float en bits, la cadena del nombre por su FNV-1a, el contenido de los `CVector*` y de `FxPrtMult_c`) cambia el resultado.
- Fx: matriz M de la arena (patrón del falso de `CreateMatFromVec` solo si la función llega a llamarlo), resto de la arena y guardas intactos, y que la función no altere los globales (`TheCamera.m_placement.m_vPosn` 0xB6F02C, `TheCamera.m_matrix` 0xB6F03C, `g_fx.m_SmokeII3expand` 0xA9AE24: el banco los escribe antes de cada lado y los restaura al final del caso).
- Constructor: objeto de 0x17C bytes completo (vptr propio de cada lado sustituido por `kVtLabel` solo si es el esperado: EXE 0x867030, DLL su vtable, capturada una vez construyendo un `CProjectile` con los falsos), guardas, resto de la arena y EAX = this.
- Callback: tarea de 0x60 bytes (con `m_pVehicle` como etiqueta), los tres vehículos (vptr, handling, conductor), resto de la arena y guardas, y la traza.
- ABI: ESP (`ret 4/8` → `popBytes = 0`; callback cdecl → 8), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, cadena SEH (`fs:[0]`), x87 normalizado (FCW completa, TOP, etiquetas, ST válidos: ninguno), MXCSR sin banderas; FCW fijada a 24 bits como en el juego; `route_ok` (original: el hook de la función NO salta a la DLL; candidato: sí, con el cuerpo en el módulo ASI); entradas vigiladas del EXE y de la DLL de las 5 funciones y de los 12 colaboradores; `globals_restored` se evalúa tras quitar los parches (lección 3 de BRIEF60).

### Colaboradores sustituidos (12; falso en la dirección del EXE y en el cuerpo de la DLL, resuelto antes de escribir ningún parche)

`FxManager_c::CreateFxSystem(name, CVector)` 0x4A9BE0 (hook `CreateFxSystem-4`), `FxSystem_c::PlayAndKill` 0x4AA3D0, `FxManager_c::FxRwMatrixCreate` 0x4A9440, `FxRwMatrixDestroy` 0x4A9460, `Fx_c::CreateMatFromVec` 0x49E950 (escribe 16 palabras de patrón en la matriz), `FxSystem_c::CopyParentMatrix` 0x4AA890, `FxSystem_c::AddParticle(v3d)` 0x4AA440 (hook `AddParticle-v3d`); `CObject::CObject()` 0x5A1D10 (sin hook: primera llamada del constructor de la DLL; ruido + vptr marcador), `CObject::SetIsStatic` 0x5A0760 (invierte el bit 2 de +0x1C y borra/invierte bits de +0x40: delata el orden respecto a `bExplosionProof`), `CEntity::SetModelIndex` 0x532AE0; `CVehicle::ClearGettingOutFlags` 0x6D3060, `CVehicleAnimGroup::GetGroup` 0x6E3B00 (devuelven/mutan según el caso) y la ranura 28 de la vtable del vehículo (64 falsos distintos en una vtable de arena que registran su índice: la DLL debe usar la misma ranura 0x70). Los demás colaboradores no se ejecutan.

## Fixtures (`Fixtures.hpp`): 5 funciones × 4096 casos; familia = n % 16 (256 casos por familia y función)

- **Fx (funciones 0..2):** cámara por `m_matrix` nulo / no nulo (la posición no usada lleva ruido), `this` = Fx_c falso / g_fx / basura, `CreateFxSystem` nulo / no nulo, argumentos con 0, ±0, subnormales, NaN (incl. sNaN), ±inf (también inf − inf = NaN), desbordes de los cuadrados, diferencias diminutas, coordenadas grandes con diferencia pequeña, suma exacta de 625 con ternas pitagóricas (25,0,0), (0,7,24), (0,15,20), (12,15,16), y ±1–3 ulps alrededor del límite. Cobertura medida: por función ≈ 2000 saltados, ≈ 2100 aceptados, ≈ 550 NaN, ≈ 300 ∞, ≈ 770 a menos de 2 del 625, ≈ 205 exactos 625.
- **Constructor:** 8 clases de `modelId` (pequeños, reales 0..19999, extremos, dword aleatorio, negativos, 16 bits), 6 modos de relleno previo (ruido, 0, 0xFF, 0xCD, 0xA5), 4 desplazamientos del objeto en la arena (0x40..0x4C).
- **Callback:** vehículo nulo (512), sin conductor (1426), con conductor (2158), mutaciones por los falsos de `ClearGettingOutFlags` (805) y `GetGroup` (559 + 558 de conductor); byte de banderas 0xFF/0x20/0/aleatorio; grupo de animación 0/0xFF/aleatorio; valor de retorno de `GetGroup` de 0 a 2^32.

## Oráculo (`fixture_oracle.py`)

Modelado desde el desensamblado (no del C++ de la DLL): aritmética con `Fraction` y redondeo a 24 bits (mitad al par, exponente extendido) en cada operación x87, reglas de NaN/inf; construcción de las trazas, la matriz M, el objeto del constructor (con el ruido del falso) y la simulación del callback con los tres vehículos. Autoprueba (`python -I fixture_oracle.py`): redondeo contra `struct.pack('<f', …)` en 4000 pares y ejemplos manuales de cada función (umbral 625, 25 + 1 ulp, NaN, inf, desborde en exponente extendido, trazas completas, constructor, callback con y sin conductor y con vehículo nulo).

## Comprobaciones sin el juego (hechas)

`fixture_selfcheck.py`: las constantes de arena del C++ compilado con `cl` coinciden con las del oráculo (`LAYOUT`) y la cobertura de las 20480 fixtures supera `run_local.MINIMUMS` (calibrados con margen del 35–45 %).

## Suposiciones y riesgos

- `Fx.cpp` conserva las opciones por defecto del proyecto (SSE en Debug): solo cambian magnitudes que no pueden afectar a la comparación `> 625`. Si en el futuro se recompila con otras opciones, repetir el banco.
- `CProjectile` construido por la DLL lleva **el vptr de la vtable de la DLL**, no 0x867030. En el EXE ambas vtables (CObject 0x866F60 y CProjectile 0x867030) son idénticas salvo la entrada 0 (destructor escalar: 0x5A40F0 → 0x5A40E0 + `0x5A1F20` frente a 0x5A2BD0); en la DLL el destructor escalar de `CProjectile` pasa por el implícito → `~CObject` + `CObject::operator delete`. El banco solo valida el constructor; el destructor y el resto de virtuales de proyectiles creados por `AddProjectile` no se ejercitan aquí (misma situación que el `CObject` construido por `Population`).
- `CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB` no fue validada con una animación real; el banco valida el cuerpo con vehículos/handling/vtable falsos en arena.
- Mutaciones de prueba (constante de flotación, orden CopyParentMatrix/PlayAndKill, alfa del `FxPrtMult_c`, condición del filtro con NaN, no releer el vehículo) se prepararon para comprobar que el banco las detecta, pero la corrida no se completó: el disco D: se llenó (LNK1180/LNK1116, error 112) y las fuentes se restauraron byte a byte (hashes del manifiesto iguales). El banco compara las trazas y los objetos completos con un oráculo independiente del C++, y el defecto real de la constante de flotación se halló por desensamblado antes de la corrida.
