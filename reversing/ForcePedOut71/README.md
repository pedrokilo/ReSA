<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ForcePedOut71: constructor de CTaskSimpleCarForcePedOut 0x647710 (no virtual, lote 71)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-2`, HEAD 1b2235ec3 + `source.patch`). 16384 pares, cero diferencias, todas las compuertas de ABI/ruta/SEH/restauracion en ambos lados.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test
esultados-forcepedout7161026-181819-894577`.

Fila de `docs/hooks.csv` (la anade el coordinador): `CTaskSimpleCarForcePedOut,Constructor,0x00647710,1,0,1,S`.

## Que hace (0x647710..0x64876B, thiscall (vehiculo, puerta), `ret 8`, marco SEH propio, vtable 0x86EE94, tarea de 0x10 bytes)
Llama a CTaskSimple::CTaskSimple (0x61A390: `[+4]=0`, `[+0]=0x86D4A8`), escribe `[+0]=0x86EE94`, `[+8]=vehiculo`, `[+0xC]=puerta`, y si el vehiculo no es nulo llama a `CEntity::RegisterReference` (0x571B70, ECX = vehiculo, arg = `&this->m_pVehicle`). Devuelve `this` en EAX. No escribe relleno (no hay: 0x10 bytes exactos).
Llamadores en el EXE (2): `CTaskComplexLeaveCar::CreateSubTask` (0x6418B8) y `CTaskSimpleCarForcePedOut::Clone` (0x649F1D, retorna EAX); ninguno lee ECX/EDX tras la llamada (los recargan), asi que solo se compara EAX.

## Cambios de produccion (`source.patch`)
- `TaskSimpleCarForcePedOut.h/.cpp`: campos `m_pVehicle` (+8), `m_nDoor` (+0xC), `VALIDATE_SIZE(...,0x10)`, constructor en C++ puro + `RH_ScopedInstall(Constructor, 0x647710)` con envoltura `Constructor`. Para que la clase sea instanciable en la DLL (virtuales puros) se anaden SIN hook: destructor (0x647790, limpia la referencia al vehiculo: imprescindible, el del EXE lo hace), `Clone` (0x649EE0), `MakeAbortable` (0x647780, `false`) y `ProcessPed` (0x6477F0), que sigue en el EXE (reenvio `plugin::CallMethodAndReturn`; pendiente de reconstruir). `GetTaskType` ya existia.
- `InjectHooksMain.cpp`: se descomenta `CTaskSimpleCarForcePedOut::InjectHooks()` (+ include). Solo registra el constructor (la vtable no se instala).
- `TaskComplexLeaveCar.cpp`: la rama `TASK_SIMPLE_CAR_FORCE_PED_OUT` de `CreateSubTask` usa `NewLeaveCarSubTask<CTaskSimpleCarForcePedOut>` (mas el include).

## Banco
`Original` = bytes del EXE con el hook S apagado; `candidato` = DLL con el hook encendido (`route_ok` en cada lado). Objeto en arena con guardas de 0x40 bytes, relleno previo semilla/0xCD/ceros/0xFF; vehiculo nulo/arena (con desplazamientos)/crudo; puertas validas, extremas, aleatorias y con bit alto.
Se compara: cambios de bytes en la arena del objeto (relleno y guardas incluidos) y del vehiculo; EAX (= this); secuencia de eventos de `RegisterReference` (falso identico en la direccion del EXE y en el cuerpo de la DLL; ausente si el vehiculo es nulo); vtable por origen de cada lado (EXE 0x86EE94, DLL en el ASI) y por tipo (GetTaskType = 0x32F); SEH (cabeza de la cadena restaurada), ESP, no volatiles, DF, x87, globales. ECX/EDX solo se registran.
**CTaskSimple::CTaskSimple 0x61A390**: la DLL no tiene cuerpo propio (CTask/CTaskSimple se integran en linea), asi que no hay "cuerpo de la DLL" donde poner el falso. En el EXE se sustituye por un falso con el mismo efecto que anota el evento; se exige que sea una sola llamada, la primera, con `this` = objeto (`base_ctor_ok`), y en la DLL se exige el mismo efecto (padre a 0). El marco SEH interno del EXE no es observable en la DLL: se comprueba que `fs:[0]` queda restaurado en ambos.
Cobertura: 16 familias x 1024; `coverage.json` = 50 % de la primera corrida.

No cubierto aqui: destructor, Clone, MakeAbortable y ProcessPed (ver el banco `ForcePedOutVt71`); que `RegisterReference` en si sea correcta (otros bancos).

## Comandos
```
bash reversing/_handoff/bench_here.sh ForcePedOut71
python -B reversing/ForcePedOut71/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/ForcePedOut71/register_hook.py <carpeta-de-evidencia>      # SOLO el coordinador
```

## 2026-10-26 (tarde): reejecutado con las virtuales enganchadas
Tras reconstruir y enganchar destructor, Clone, MakeAbortable y ProcessPed (banco `ForcePedOutVt71`; ya no hay reenvio al EXE ni virtuales sin banco) se repitio este banco con todo activo: **APROBADO**, 16384 pares, cero diferencias, `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-forcepedout7161026-184540-280109`. El `source.patch` incluye ahora los cambios de las virtuales.
