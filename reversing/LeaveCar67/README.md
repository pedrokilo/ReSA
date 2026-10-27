<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# LeaveCar67: CTaskSimpleGangDriveBy::SetupAnim (0x627B20), helper de animacion de la tarea del tiroteo desde coche (lote 67)

Estado: **NO VALIDADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-3`, HEAD 1ca520153 + `source.patch`). El C++ compila (proyecto real y sonda `gta_reversed-leavecar67-probe.asi`)
y el oraculo y las fixtures pasan su autoprueba sin juego, pero la corrida par a par **no se ha ejecutado**: `bench_here.sh` quedo esperando el candado global
`D:\Original\gta_lock`, que es un resto de una ejecucion interrumpida el 22/10/2026 01:58 (sin ningun `gta_sa.exe` en marcha); en el clon de pruebas queda ademas un
`reversed-tests.lock` de 6 bytes nulos del mismo instante, que `runtime_runner.run_case` (`lock.open('x')`) no deja sobrescribir. Al no poder quitar esos restos, no hay evidencia, ni
carpeta de resultados, ni pares. Tras quitarlos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh LeaveCar67`.

| Funcion | Direccion | Hook RH (clase / nombre) | Tipo | Fuente |
|---|---|---|---|---|
| `bool CTaskSimpleGangDriveBy::SetupAnim(CPed*)` (sin simbolo confirmado; nombre elegido por lo que hace) | 0x627B20 | `CTaskSimpleGangDriveBy` / `SetupAnim` | S | `TaskSimpleGangDriveBy.cpp/.h` |

Fila de `docs/hooks.csv` que debe anadir el coordinador (no es virtual): `CTaskSimpleGangDriveBy,SetupAnim,0x00627B20,1,0,1,S`.

## Que hace (desensamblado, 414 bytes, thiscall `ret 4`, AL = resultado)

Los dos llamadores del EXE (0x62D2EA en `MakeAbortable` 0x62D290 y 0x62D5C4 en `ProcessPed` 0x62D3B0) hacen `test al,al`; no leen EAX alto, ECX ni EDX.

1. Bloque de animaciones del grupo requerido: `ms_aAnimAssocGroups[+0x28]` (0xB4EA34, grupos de 0x14 bytes) -> primer dword (`m_AnimBlock`); si es nulo, `GetAnimationBlock(GetAnimBlockName(grupo))` (0x4D3940 / 0x4D3A30).
   Indice = `(bloque - 0xB5D4A0) >> 5` aritmetico. Si el byte `IsLoaded` (+0x10) **no es exactamente 1**: `CStreaming::RequestModel(indice + 0x63E7, 8)` y devuelve false.
2. Si `m_bAnimsReferenced` (+9) es 0: `AddAnimBlockRef(indice)` (0x4D3FB0) y lo pone a 1.
3. Con `m_nRequiredAnimID` (+0x24) == 0xBF y sin asociacion (+0x2C): devuelve false.
4. Decide si reemplaza la animacion: sin asociacion, si `(int16)m_nAttackTimer` (+0x16) <= 0 y `m_bInWeaponRange` (+0xC) != 0; con asociacion, si `movsx(m_AnimId)` (+0x2C de la asociacion)
   != el id requerido (32 bits), la orden siguiente (+0xF) es 1, 2 o 3 y `(char)m_nLastCommand` (+0x10) < 4.
5. Reemplazo: si habia asociacion, `SetDeleteCallback(0x4D1390, 0)` y `flags &= 0xFE` (AND de byte); `BlendAnimation(clump = ped+0x18, grupo, id, 4.0f)` (0x4D4610); `SetDeleteCallback(0x621BE0, this)`;
   si la orden siguiente es 3 y `(char)m_nBurstShots` <= 1: `m_nBurstShots = GetRandomNumberInRange(2, (int16)m_pWeaponInfo->m_nAmmoClip)` (0x407180);
   con asociacion previa: `SetCurrentTime([0x8D2E7C])`, `flags &= 0xFE`, ultima = 1, siguiente = 0 y, si `!ped->IsPlayer()` (0x5DF8F0), `m_nAttackTimer = 100`; sin ella: ultima = siguiente, siguiente = 0.
6. Resultado: asociacion actual no nula y `m_BlendAmount > 0.9f` (0x858C20) y `m_BlendDelta >= 0.0f` (0x858B50), comparaciones ordenadas de la x87 (`fcomp` + `test ah,41h` / `test ah,1`): NaN y un subnormal negativo dan false.

Cambios de produccion (`source.patch`): `TaskSimpleGangDriveBy.cpp` (`SetupAnim`, `RH_ScopedInstall(SetupAnim, 0x627B20)`, `MakeAbortable` y `ProcessPed` la llaman en C++ en lugar de `CallMethodAndReturn<bool, 0x627B20>`),
`TaskSimpleGangDriveBy.h` (declaracion). Sin `__asm`/`naked`; las comparaciones usan los ayudantes x87 que el archivo ya tenia (`X87Gt`, `X87Ge`). Los callbacks 0x4D1390 y 0x621BE0 se pasan por direccion
(el segundo es un callback sin reconstruir; no se ejecuta en el banco). Quedan fuera de esta tarea: `FireGun` 0x627CC0 y el callback 0x621BE0.

## Banco

`Original` = bytes del EXE con el hook RH de `SetupAnim` apagado; `candidato` = DLL con SOLO ese hook encendido. 16384 pares (16 familias de 1024). Colaboradores (9) sustituidos por falsos identicos en la direccion del EXE y en el cuerpo
de la DLL, que registran sus argumentos en orden: `GetAnimBlockName` 0x4D3A30, `GetAnimationBlock(const char*)` 0x4D3940, `RequestModel` 0x4087E0, `AddAnimBlockRef` 0x4D3FB0, `BlendAnimation(id)` 0x4D4610,
`SetDeleteCallback` 0x4CEBC0, `SetCurrentTime` 0x4CEA80, `CPed::IsPlayer` 0x5DF8F0 (todos con hook) y `rand()` del EXE 0x821B1E (la DLL lo llama por la direccion del EXE; `GetRandomNumberInRange(int)` 0x407180
queda como codigo real en el lado del EXE y llega a ese falso). `ms_aAnimAssocGroups` (0xB4EA34) se apunta durante la llamada a un arreglo falso de 256 grupos (se restaura tras cada lado).
Los bloques con indice negativo son direcciones de `.rdata` alineadas a 0x20 con el byte +0x10 igual a 1 / 0 (comprueba el desplazamiento aritmetico).

Fixtures (`Fixtures.hpp`): bloque no cargado (por el grupo, por nombre, bloque del EXE), byte `IsLoaded` no canonico (0, 2, 3, 0x80, 0xFF...), referencia presente/ausente/no canonica, id 0xBF con y sin asociacion,
reemplazo sin asociacion (temporizador 0, negativo, cualquier valor con signo; byte de rango no canonico), misma animacion (incluida la extension de signo de 16 bits contra 32), distinta con ordenes 1..3 o fuera,
ultima orden con signo, rafaga (cargador con signo y bits altos, byte de rafaga con signo, `rand` extremos), `IsPlayer` con basura en los bits altos, umbrales 0.9f/0 (+-1 ulp), NaN, infinitos, subnormales y -0 en peso y delta.
`fixture_oracle.py` modela la funcion desde el desensamblado (autoprueba: `python -I fixture_oracle.py`); `fixture_selfcheck.py` compila `fixture_dump.cpp` (Fixtures.hpp tal cual), comprueba las constantes de arena,
pasa los 16384 casos por el oraculo y escribe `coverage.json` (50 % de lo previsto). `run_local.py` exige: igualdad EXE/DLL, compuertas de ABI/ruta/DF/SEH/restauracion en ambos lados, `fp_difference` vacio,
salida del EXE = oraculo (eventos en orden, AL y cambios de las arenas), cobertura por familia y de ramas, y restauracion final.

## Comandos

```
pwsh -NoProfile -File reversing\CVehicle\build-project.ps1                       # CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1
bash reversing/_handoff/bench_here.sh LeaveCar67                                 # compila la sonda y corre en el juego (candado global)
python -B reversing/LeaveCar67/run_local.py --analyze <carpeta>/comparisons.jsonl
python -I reversing/LeaveCar67/fixture_oracle.py && python -I reversing/LeaveCar67/fixture_selfcheck.py
python -B reversing/LeaveCar67/register_hook.py <carpeta-de-evidencia>           # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `leavecar67-runtime.start`; salida `leavecar67-runtime.jsonl`.
