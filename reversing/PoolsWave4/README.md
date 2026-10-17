<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PoolsWave4: banco por pares de las 7 funciones pendientes de CPools

Banco simple (flujo viejo, copiado de `WorldWave2`/`StorageFinal1`/`TaskFinal1`). **Sin compilar ni ejecutar todavia** (encargo: no compilar, no ejecutar GTA, no commit).

Uso: `bash reversing/_handoff/bench.sh PoolsWave4 3`. Filtro opcional: `poolswave4-only.txt` en la carpeta del juego con `CPools::Funcion`.
`build_probe.py` y `run_local.py` redirigen `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test` (el junction D: ya no existe).
Prefijo `poolswave4`, namespace `PoolsWave4Live`.

## Corpus: 2.320 pares

| Funcion | EXE | Pares |
|---|---|---|
| `CPools::Initialise` | 0x550F10 | 144 |
| `CPools::ShutDown` | 0x5519F0 | 256 |
| `CPools::Save` | 0x5D0880 | 64 |
| `CPools::Load` | 0x5D0890 | 64 |
| `CPools::LoadObjectPool` | 0x5D4A40 | 512 |
| `CPools::LoadPedPool` | 0x5D2D70 | 512 |
| `CPools::LoadVehiclePool` | 0x5D2A20 | 768 |

Original = hook desactivado (codigo del EXE); candidato = hook activo. Cada caso reconstruye el mismo fixture desde la misma semilla en los dos legs y compara resultado, estado de los pools falsos (hash de almacenamiento y de estados, cabecera de 0x14 bytes), globales tocados, control x87 y la traza completa de llamadas salientes con argumentos. Los registros que pasan llevan `original` y `candidate`.

## Que se dobla (y por que es seguro)

* **Initialise / ShutDown** — el unico estado real tocado son los 17 punteros globales de pool (0xB74484..0xB744C4), guardados y restaurados en cada ejecucion. **Toda la memoria se dobla**: solo durante la llamada se parchean `operator new` (0x82119A), `new[]` (0x821195), `delete` (0x8214BD) y `delete[]` (0x8213AE) con dobles que registran en una matriz fija (sin asignaciones; no se escribe en el json durante la ventana) y sirven memoria de un arena estatico de 5 MB. Los hilos ajenos no se desvian: `new` va a un trampolin con los 5 primeros bytes originales de 0x82119A (`56 8B 74 24 08`, comprobados antes de usarlos) y salto a 0x82119F; `delete` llama a 0x82413F (destino de `jmp` de 0x8214BD); `printf` ajeno devuelve 0.
  * `Initialise`: se ejecutan los **constructores de CPool reales del EXE** (0x550180, 0x550250, ... 0x550E40) sobre el arena y se compara orden y tamano de cada reserva (`new` 0x14 / `new[]` n*tamano / `new[]` n), los 17 punteros globales (como desplazamiento en el arena), las 17 cabeceras (incluidos los bytes +0x11..+0x13 que el EXE no escribe) y todo el arena usado + 256 bytes de guarda, con 4 patrones de relleno (0xCD, 0x00, 0xFD, 0x5A) y 18 variantes (sin fallo, o `operator new` escalar nulo en el pool k: el EXE no construye el pool y deja el global a 0, 0x550F3E `je`).
  * `ShutDown`: los 17 pools son cabeceras falsas en el arena (global nulo, tamano <= 0, propietario 0/1/2/0xFF, punteros arbitrarios que nunca se desreferencian). Se compara la secuencia `printf`(hash del formato)/`delete[]`/`delete`, las cabeceras resultantes y que los globales NO se anulan.
* **Save / Load** — `SavePedPool` 0x5D4B40, `SaveObjectPool` 0x5D4940, `LoadPedPool` 0x5D2D70 y `LoadObjectPool` 0x5D4A40 se doblan (EXE + cuerpo de la DLL) con resultado programado. Como red de seguridad, los pools globales de peds/objetos se cambian por pools falsos vacios y `SaveDataToWorkBuffer`/`LoadDataFromWorkBuffer` (0x5D1270/0x5D1300) tambien se doblan: si el compilador hubiese integrado una subfuncion en la DLL, la traza difiere y no se toca el bufer real. Se comprueba que 0xC16EE8..0xC16EFD no cambia.
* **LoadObjectPool / LoadPedPool / LoadVehiclePool** — el "fichero de guardado" es un flujo en memoria (`LoadDataFromWorkBuffer` doblado: traza tamano, posicion y hash de los bytes entregados). Los pools globales (0xB7449C objetos, 0xB74490 peds, 0xB74494 vehiculos) se sustituyen por pools falsos de 16 ranuras con la misma disposicion que el CPool del EXE (+0 almacenamiento, +4 estados, +8 tamano, +0xC ultima libre, +0x10 propietario) y almacenamiento aleatorio; las ranuras 12..15 siempre estan libres para que `CreateAtRef` (0x59F610) termine. **Se ejecutan de verdad**: `CObject::operator new(size,ref)` (0x5A1EF0), `CPed::operator new(size,ref)` (0x5E4730), `CVehicle::operator new(size,ref)` (0x6E2D60), `CreateAtRef` y sus equivalentes de la DLL. Doblados: `CObject::CObject` (0x5A1D70 y simbolo exportado), `CPlayerPed::CPlayerPed` (0x60D5B0), los 10 constructores de vehiculo (0x6C8D60 CMonsterTruck, 0x6CE370 CQuadBike, 0x6C4190 CHeli, 0x6C8E20 CPlane, 0x6F2940 CBoat, 0x6F6030 CTrain, 0x6BF430 CBike, 0x6BF820 CBmx, 0x6D03A0 CTrailer, 0x6B0A90 CAutomobile, cada uno con su simbolo `??0C...@@QAE@HW4eVehicleCreatedBy@@...` de la DLL), `CObjectSaveStructure::Extract` 0x5D2220, `CPopulation::ConvertToDummyObject` 0x614670 (libera la ranura como el original), `CWorld::Add` 0x563220, `CStreaming::RequestModel` 0x4087E0 y `LoadAllRequestedModels` 0x40EA10 (la E/S de streaming), `CCarCtrl::UpdateCarCount` 0x424000. Los objetos creados reciben una vtable falsa de 128 ranuras que registran su indice: se comprueba que EXE y DLL llaman a la misma ranura virtual de `Load` (25 en peds, 0x5D4640; 65 en vehiculos, 0x5D2900; ambas leidas de las vtables del EXE). `CVehicleModelInfo` se falsea para 16 ids reservados (19980..19995 de `ms_modelInfoPtrs`, 0xA9B0C8, comprobados vacios) con los 17 valores de tipo (-1, 0..11, 12, 255, INT_MAX, INT_MIN), incluyendo los tipos 7 y 8 y los fuera de rango que van a la rama por defecto (`ja` sin signo, 0x5D2A9D). Se guardan y restauran `Players[0].m_pPed` (0xB7CD98), `NumRandomCars` (0x969094) y los ids de modelo.
* Evento `layout`: pasos de pool (`sizeof(CCopPed)` 0x7C4, `CHeli` 0xA18, `CCutsceneObject` 0x19C), `CObjectSaveStructure` 0x30, `CVehicleModelInfo::m_nVehicleType` +0x3C, `CBike::bikeFlags` +0x614 con mascara 0x10, `CPed::m_nWeaponAccuracy` +0x71A, `&CWorld::Players` 0xB7CD98.

Dominio de los fixtures (leccion 12/21): el primer registro de LoadPedPool es siempre `PED_TYPE_PLAYER1` (si no, el EXE desreferencia un puntero nulo en 0x5D2E26); las referencias usan ranuras 0..11; en peds y vehiculos las ranuras creadas son libres y distintas (assert de `NewAt` en la DLL que el EXE no tiene); como mucho 8 registros. En LoadObjectPool se repiten ranuras ocupadas (rama de `ConvertToDummyObject`) y duplicadas.

## Correcciones de la fuente (`source/game_sa/Pools.cpp`; solo ese archivo)

| Funcion | Diferencia hallada | Evidencia del EXE |
|---|---|---|
| `Initialise` | El `CPool(capacidad, nombre)` de la DLL **rellena todo el almacenamiento con 0xFD** (`DoFill(NOMANSLAND_FILL)`); el EXE no rellena nada | 0x550180..0x5501DA: solo `mov [esi],eax` / `[esi+4]`, `[esi+10h]=1`, `[esi+8]`, `[esi+0Ch]=-1` y el bucle `or dl,80h / and dl,80h` sobre los estados; sin `memset` |
| `Initialise` | Pool de **rutas de patrulla**: `CPatrolRoutePool` es `CPool<void*>` (4 bytes por elemento, 128 bytes en total); el EXE reserva 0x1A4 bytes por elemento (13.440 bytes). El codigo del EXE que usa el pool (0xB744B4) desbordaba el monton | 0x550B08 `imul eax,eax,1A4h` (y 0x550F10..: `push 20h`, `call 550B00`) |
| `Initialise` | La DLL ponia a `false` el byte +0x11 (`m_DealWithNoMemory`); el EXE no lo escribe | 0x5501A7 solo `mov byte ptr [esi+10h],1` |
| `Initialise` | Rama de fallo de `operator new` | 0x550F3E `je 00550F53` -> `xor eax,eax` y global a 0 (idem en los 17 pools) |
| `ShutDown` | `delete pool` ejecutaba `~CPool`/`Flush`: **rellena con 0xFD el almacenamiento**, pone la ultima libre a -1, borra propietario y `m_DealWithNoMemory`, y libera tambien con tamano 0 | 0x551A0D..0x551A40: `cmp ecx,ebx / jle` (tamano con signo > 0), `cmp byte [eax+10h],bl / je` (solo libera si propietario != 0), delete[] almacenamiento, delete[] estados, `[esi]=[esi+4]=[edi]=[esi+0Ch]=0` (la ultima libre queda a 0), `push esi / call 008214BD`; sin relleno |
| `ShutDown` | Los mensajes salian por `NOTSA_LOG_DEBUG` (spdlog reserva memoria); el EXE llama a `printf` del CRT con "Shutdown pool started\n" / "Shutdown pool done\n" | 0x5519F3 `push 863D78h / call 00821982`; 0x551E53 `push 863D64h / call 00821982` |
| `Save`, `Load` | MATCH (sin cambios) | 0x5D0880: `call 5D4B40 / test al,al / jne / ret / jmp 5D4940`; 0x5D0890: `call 5D2D70 ... jmp 5D4A40` |
| `LoadObjectPool` | MATCH (sin cambios; ya corregida por el agente previo) | 0x5D4A40..0x5D4B3A: ref, modelo, `ConvertToDummyObject` si el estado tiene bit 7 a 0, `operator new(0x17C, ref)` (0x5A1EF0), ctor 0x5A1D70(modelo, 0), tamano (4 bytes), 0x30 bytes, `Extract` 0x5D2220, `CWorld::Add` 0x563220 |
| `LoadPedPool` | MATCH (sin cambios) | 0x5D2D93 `xor esi,esi` fuera del bucle; 3 lecturas; `test eax,eax / jne 5D2E26`; 0x5E4730(0x7A4, ref); ctor 0x60D5B0(0,0); `[eax+71Ah]=64h`; `[B7CD98]=eax`; `call [edx+64h]`; `CWorld::Add` |
| `LoadVehiclePool` | MATCH (sin cambios) | 0x5D2A47..0x5D2D2D: lee ref y modelo, `4087E0(modelo, 8)`, `40EA10(0)`, tabla de saltos 0x5D2D40 indexada por tipo-1 (`ja` sin signo -> automovil `6B0A90(modelo,1,1)`), bicis `or byte [eax+614h],10h`, `call [edx+104h]`, `dec [969094h]`, `424000(veh, 0)`, `563220` |

Observaciones no corregidas (fuera del alcance de `Pools.cpp`/`Pools.h`):
* `source/game_sa/Pools/PatrolRoutePool.h` sigue declarando `CPool<void*>`: sus metodos (`New`, `GetAt`...) usarian un paso de 4 bytes. Hoy nadie los usa (los hooks estan comentados); `Initialise` ya reserva los 0x1A4 bytes del EXE. Conviene declararlo con un tipo de 0x1A4 bytes cuando se enganchen sus funciones.
* El nombre que reciben los constructores de CPool (`"PtrNode Single"`, ...) no se guarda en el EXE (0x550180 solo usa el primer argumento); la DLL tampoco lo guardaba. Los bytes +0x11..+0x13 de cada pool quedan sin inicializar (como el EXE); `CPool::CanDealWithNoMemory()` (NOTSA, solo depuracion) puede leer basura en estos 17 pools, y solo afecta a la rama de fallo de la `New()` generica, que los pools con `New` propio no usan.

## Fuera del banco

* Nada de las 7 funciones queda fuera. Limitaciones del doble:
  * `Initialise`/`ShutDown` operan sobre los 17 globales reales durante unos microsegundos por ejecucion; el codigo de otros hilos que consulte un pool en esa ventana vera un pool valido del arena (nunca memoria sin mapear). Se restauran siempre.
  * El caso "fallo de `operator new[]` del almacenamiento" no se prueba: el EXE no comprueba el resultado (0x550195..0x5501A0) y escribiria en `NULL+i`.
  * Con el compilador en Release el `if (!pool)` de `ConstructPool` podria eliminarse (la DLL asume que `operator new` no devuelve nulo); el banco compila en Debug.
  * `LoadPedPool` escribe un ped falso en `Players[0].m_pPed` mientras dura la llamada (el EXE hace lo mismo en 0x5D2E21); se restaura al terminar cada ejecucion.

## Supuestos a revisar al compilar

* Simbolos exportados `??0C<Clase>@@QAE@HW4eVehicleCreatedBy@@@Z` (y `...@@_N@Z` para `CAutomobile`) y `??0CPlayerPed@@QAE@H_N@Z`, `??0CObject@@QAE@H_N@Z`: comprobados con `dumpbin /exports` sobre `bin/Debug/gta_reversed.asi`. Si faltara alguno, el banco emite un evento `error` con la lista antes de parchear nada.
* `Body()` de `LoadDataFromWorkBuffer(void*,int32)`/`SaveDataToWorkBuffer(void*,int32)` usa `static_cast` al puntero de funcion (sobrecargas con plantilla).
* El cuerpo de la DLL de `SavePedPool` etc. se busca con los hooks activos (se calculan todos antes de crear ningun `EntryPatch`, leccion 11).

Resultado de la primera ejecucion: **pendiente** (no se ha compilado ni ejecutado).

## Aprobación (09/10/2026)

Corrida `20261009-123502-353564`: passed=true, 2320 pares, 7 funciones con 0 fallos (Initialise 144, ShutDown 256, Save 64, Load 64, LoadObjectPool 512, LoadPedPool 512, LoadVehiclePool 768), evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_poolswave4.py` (evidencia en `runs/`, `validation-summary.json`). Con esto CPools queda COMPLETA. Límites: new/delete/printf doblados solo durante la llamada con arena estática; fallo de new[] no cubierto; Initialise/ShutDown tocan los 17 globales reales durante microsegundos con restauración; `Pools/PatrolRoutePool.h` sigue declarando `CPool<void*>` (no editado).
