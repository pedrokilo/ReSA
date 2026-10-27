<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies25A (grupo g25a de BRIEF_21): nueve entradas del EXE de nivel «1 Muy fácil» (cohorte 2 de dependencias originales, categoría juego)

PREPARADO, sin compilar en el árbol real ni ejecutar dentro del juego: la sonda debe construirla y correrla el coordinador (en serie, clon de pruebas). No constituye validación hasta entonces.
Evidencia previa sin juego (sin tocar el directorio `build\`): `offline/` mapea los bytes REALES del EXE en un proceso hijo, compila el MISMO nucleo del banco (`Core.hpp`: fixtures, colaboradores falsos, compuertas de ABI/x87) y el texto de las
nueve reconstrucciones extraído sin cambios del árbol de fuentes parcheado: 9 x 2048 = 18432 pares, 0 diferencias, y `run_local.py --analyze` valida esa corrida contra el oráculo independiente (`fixture_oracle.py`, modelado desde el desensamblado).
Con 23 errores sembrados a propósito en las reconstrucciones (vtable, bit del estado, contador cacheado, zancada, índice acotado, máscara de 8 bits, bit equivocado, parada del recorrido, flag, no anular…) el mismo arnés da diferencias en TODOS (`seed_bugs.py`): el banco tiene dientes.

| Función | Entrada | Convención | Colaboradores | Pares |
| --- | --- | --- | --- | --- |
| CTask::Destructor (cuerpo no virtual de ~CTask) | 0x61A3A0 | thiscall, sin pila, `ret` plano; conserva EAX/ECX/EDX | ninguno | 2048 |
| CVehiclePool::GetAt (`CPool<CVehicle,CHeli>::GetAt`) | 0x41CC10 | thiscall, 1 arg de pila (`ret 4`), EAX | ninguno | 2048 |
| CAutoPilot::RemoveOnePathNode | 0x41B950 | thiscall, sin pila, `ret` plano | ninguno | 2048 |
| CPedGroups::RemoveAllFollowersFromGroup | 0x5FB8A0 | cdecl, 1 arg, termina en `jmp 0x5FB7D0` | CPedGroup::RemoveAllFollowers (EXE 0x5FB7D0 + cuerpo DLL) | 2048 |
| CDecisionMakerTypes::FlushDecisionMakerEventResponse | 0x604490 | thiscall, 2 args (`ret 8`) | CDecision::SetDefault (EXE 0x600530 + cuerpo DLL) | 2048 |
| CGarages::CountCarsInHideoutGarage | 0x44A210 | cdecl, 1 arg (se lee como byte), EAX = cuenta | FindSafeHouseIndexForGarageType (EXE 0x4489F0 + cuerpo DLL) | 2048 |
| FindReplacementUpgradeCB | 0x6D3490 | cdecl, 2 args, EAX | GetAtomicId 0x732370 y GetModelInfoIndex 0x732250 (EXE + cuerpos DLL) | 2048 |
| GetAnimHierarchyFromFrameCallback (helper de GetAnimHierarchyFromFrame) | 0x734A70 | cdecl, 2 args, EAX | RpHAnimFrameGetHierarchy 0x7C5160 y RwFrameForAllChildren 0x7F0DC0 (EXE + cuerpo/envoltorio de la DLL) | 2048 |
| CAEStreamingDecoder scalar deleting destructor | 0x4F2860 | thiscall, 1 arg de pila (`ret 4`), EAX = this | operator delete 0x8214BD (la DLL lo llama por la misma dirección); ~CAEDataStream (0x4DC490) real | 2048 |

Hooks: `RH_ScopedInstall(Destructor, 0x61A3A0)` (CTask), `GetAt, 0x41CC10` (CVehiclePool), `RemoveOnePathNode, 0x41B950` (CAutoPilot, clase nueva con `InjectHooks`), `RemoveAllFollowersFromGroup, 0x5FB8A0`, `FlushDecisionMakerEventResponse, 0x604490`,
`CountCarsInHideoutGarage, 0x44A210`, `RH_ScopedGlobalInstall(FindReplacementUpgradeCB, 0x6D3490)` (categoría CVehicle) y `ScalarDeletingDestructor, 0x4F2860`. Original = EXE con el hook apagado; candidato = DLL con el hook encendido (`HookRoute::RegisteredTarget`).
**La 0x734A70 NO tiene hook** (único llamador del EXE: `GetAnimHierarchyFromFrame` 0x734AB0, ya enganchada; engancharla rompería el aislamiento del banco `reversing/CppIntegerNext/Hierarchy`, que exige el callback nativo): su ruta es «directa» —
original = la entrada nativa 0x734A70 (sin salto a la DLL), candidato = la función de la DLL `GetAnimHierarchyFromFrameCallback` llamada por su dirección (`route_ok` exige módulo ASI y, en el original, módulo del EXE sin hook).
No tiene fila de catálogo (`audit_hook_catalog.py` marcaría una fila sin registro); su dependencia desaparece del cohorte porque `source/` ya no referencia la dirección.

## Qué hace cada función (desensamblado completo en `original-disassembly.txt`)
0. `mov dword ptr [ecx], 0x86D48C ; ret`: deja la vtable de CTask. EAX/ECX/EDX salen INTACTOS y el banco los compara enteros (el destructor escalar 0x61A5E0 hace `mov eax,ecx ; call 0x61A3A0` y usa después EAX = this). La DLL lo compila con `#pragma optimize("gty", on)` + `runtime_checks off` (CMake: `/Zi` y `BasicRuntimeChecks=Default` para Task.cpp) a exactamente esas dos instrucciones.
1. `GetAt(idx)`: 0 si el bit 7 del byte `[estado + idx]` está puesto, si no `[storage] + idx * 0xA18` (aritmética de 32 bits). Sin comprobar límites (la DLL tiene un `assert` de Debug: el banco usa `idx < 256 = capacidad`).
2. `RemoveOnePathNode`: `dec word [ecx+0x88]`; si el contador (con signo) queda > 0, copia `nodo[i+1]` sobre `nodo[i]` con un índice de 16 bits y relee el contador en cada vuelta; con 9 o más nodos la copia pisa el propio contador (+0x88) con la palabra de +0x8C. La reconstrucción lee el contador por un puntero `volatile` y accede por dirección.
3. `RemoveAllFollowersFromGroup(id)`: si el byte `ms_activeGroups[id]` (sin límites, `0xC098E0 + id`) es distinto de 0, salta a `CPedGroup::RemoveAllFollowers` con ECX = `0xC09920 + id * 0x2D4`.
4. `Flush…(a, b)`: `ECX = this + 4 + (m_EventIndices[b] + a * 41) * 0x3C` (32 bits que dan la vuelta) y llama a `CDecision::SetDefault`.
5. `CountCarsInHideoutGarage(type)`: `movzx` del byte, índice = `FindSafeHouseIndexForGarageType` (0x4489F0), cuenta las 4 palabras `m_wModelIndex` ≠ 0 de la tabla `0x96ABEA + idx * 0x100` (paso `0x40`). OJO: el EXE pone EDX = 0 ANTES de llamar a 0x4489F0 y lo usa como contador DESPUÉS: el original solo escribe EAX, así que el colaborador falso es un envoltorio con ensamblador (permitido en bancos) que preserva ECX/EDX. Consecuencia para producción: un `FindSafeHouseIndexForGarageType` de la DLL (hook de 0x4489F0) clobbera EDX; con la 0x44A210 enganchada ya no importa (la DLL la llama en C++), pero con ese hook apagado y el de 0x44A210 apagado el original del EXE seguiría funcionando solo si 0x4489F0 conserva EDX.
6. `FindReplacementUpgradeCB(object, data)`: si `object->type == 1` y `!(GetAtomicId(object) & 0x800)` y `GetModelInfoIndex(object) != -1`: `data[1] = object` y devuelve nullptr; en cualquier otro caso devuelve `object`. Las dos llamadas se hacen en ese orden y solo si procede.
7. `GetAnimHierarchyFromFrameCallback(frame, data)`: `h = RpHAnimFrameGetHierarchy(frame)`; si hay → `*data = h`, nullptr; si no → `RwFrameForAllChildren(frame, <ella misma>, data)` y devuelve `frame`.
8. `ScalarDeletingDestructor(flags)`: `[this] = 0x85F418`; si `[this+4]`: `~CAEDataStream` (0x4DC490), `operator delete(stream)`, `[this+4] = 0`; si `flags & 1` (byte): `operator delete(this)`; EAX = this.

## Qué se compara (en bits, original == candidato == oráculo)
- Por lado: valor de retorno / memoria tocada / traza de colaboradores (según la función), ESP restaurado (cdecl: el llamador retira 4 bytes por argumento; thiscall con `ret N`: lo retira la función), EBX/ESI/EDI/EBP conservados (se siembran), ruta (`route_ok`),
  estado x87 completo (FNSAVE de 108 bytes salvo FIP, con los ocho registros ST sembrados con un residuo conocido; `kCompareDeadX87Registers = true`, NO se relaja) y MXCSR, trampas (recorridos de marcos de más de 32 niveles) y restauración de la memoria global.
- EAX/ECX/EDX de salida: se COMPARAN enteros en `CTask::Destructor` (EAX/EDX de entrada y ECX = el objeto; 259 llamadores del EXE los necesitan); en las demás solo se registran (`original_residue`, `candidate_residue`): ningún llamador del EXE los lee (`callers-residue.txt`, `residue_scan.py`).
- ABI sembrada: 64 dwords bajo el ESP (`SlotValue = semilla ^ k*0x9E3779B1`), EAX/ECX/EDX con semillas por caso.

## Colaboradores y falsos (sin tocar nada real)
- Los falsos se parchean con `EntryPatch` en la dirección del EXE y, cuando la DLL los llama DIRECTAMENTE (reconstruidos: `CPedGroup::RemoveAllFollowers`, `CDecision::SetDefault`, `FindSafeHouseIndexForGarageType`, `GetAtomicId`, `GetModelInfoIndex`), también en el cuerpo de la DLL
  (`Resolve` salta los `E9` de enlace incremental; los cuerpos se calculan ANTES de crear parches). `RpHAnimFrameGetHierarchy` y `RwFrameForAllChildren` hoy son envoltorios de la DLL que llaman a la dirección del EXE (y el grupo g25b ya reconstruye el primero con cuerpo propio y hook en 0x7C5160): se parchean SIEMPRE la dirección del EXE y el cuerpo de la DLL,
  así el banco vale antes y después de g25b y no exige que esos colaboradores estén sin enganchar. `operator delete` (0x8214BD) es un envoltorio de la DLL que llama a la dirección del EXE (MemoryMgr.cpp): un solo parche.
- `0x5FB7D0`/`0x600530`: registran el ECX con que se llaman (el de `SetDefault` relativo al `CDecisionMakerTypes` del caso). `0x4489F0`: devuelve el índice del caso, registra el argumento de 32 bits y conserva ECX/EDX. `0x732370`/`0x732250`: devuelven el id/índice del caso y registran si el átomo es el del caso.
- `0x7C5160`/`0x7F0DC0`: árbol de hasta 8 marcos falsos (`FakeHierarchy` / `FakeChildren`, que llama al callback con cada hijo hasta que devuelve nulo, como el cuerpo del EXE); la traza normaliza el callback (EXE o DLL = 1) y el puntero de datos.
- `0x8214BD`: no libera nada; registra el puntero normalizado (decodificador = 1, flujo = 2, nombre = 3). El resto del destructor del flujo (`~CAEDataStream`, con `m_bIsOpen = false`) es real y se enmascara su primer dword (vtable del flujo: la de la DLL cuando 0x4DC490 está enganchada).

## Estado guardado y restaurado
`ms_activeGroups` y su entorno (región `0xC098D0..0xC09920`, 0x50 bytes) y la tabla de refugios `0x96ABD8` (0x1400 bytes) se guardan al empezar, se rellenan por caso y se restauran tras cada lado y al final (`global_state_final`: región, hash de la tabla, x87).
Vigilados 16 bytes de las 23 entradas implicadas (las nueve del EXE, los colaboradores y sus cuerpos en la DLL; deben quedar intactos) y los hooks (`HookRoute::Restore`). El resto de los casos usa memoria propia del banco (arenas con guardas).

## Fixtures (Fixtures.hpp; fixture_oracle.py repite Mix/Rand/FillWords)
2048 casos por función, con semilla de pila por caso (las 8 primeras: 0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000, 0x9E3779B1, 0x55555555) y EAX/ECX/EDX por caso. Estrés: valores notables de vtable, de almacenamiento (0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF…), contadores de 16 bits con signo
(0, 1, 2…, 13, 0x7FFF, 0x8000, 0xFFFF, hasta 32767 vueltas), identificadores de grupo fuera de rango (-16..0x1000), índices de decisión y de evento extremos (aritmética de 32 bits que da la vuelta), palabras de modelo con byte bajo cero (0x100, 0x8000, 0xFF00…),
ids de átomo con/sin el bit 0x800, índices de modelo -1/0x7FFFFFFF/INT_MIN, árboles de marcos de 1..8 nodos con jerarquía en la raíz/en un hijo/en ninguno, y banderas de 32 bits con el bit 0 en todas las combinaciones con flujo presente/ausente y nombre nulo/puntero a bloque/puntero arbitrario.

## Compuertas de run_local.py
Igualdad fila a fila (`probe_case.analyzer`), hooks registrados y cuerpos en el ASI, función directa declarada, colaboradores sustituidos, ABI/ruta/x87/trampas en ambos lados, `fp_difference` vacío, salida de CADA lado == oráculo, cobertura mínima por función (calculada desde las entradas y el oráculo,
no desde la DLL) y restauración final. `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.

## Ejecución
```
python -B reversing/Dependencies25A/build_probe.py
python -B reversing/Dependencies25A/run_local.py
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies25A
```
Sin juego (compila contra los bytes del EXE; no toca `build\`): `reversing\Dependencies25A\offline\bld.bat` (extrae los `.inc` de `source/`), después `offline\obj\bank25a.exe 2048 salida.jsonl` y `python -B run_local.py --analyze salida.jsonl`.
Antes: aplicar `g25a.patch`, añadir las filas de `g25a-hooks.csv.txt` a docs/hooks.csv (`python contrib/sync_docs.py --update-source-status`) y compilar.

## Efectos sobre otros bancos (volver a correrlos o adaptarlos con el árbol nuevo)
Búsqueda en `reversing/` de las nueve direcciones (excluidas las copias de fuentes de `runs/`):
- `CameraNewMode355` y `PlaneGeneration355` parchean 0x41CC10 como colaborador falso (`EntryPatch po(0x41CC10, Pool)` / `pg(0x41CC10, PoolGet)`): la DLL ya no la llama por esa dirección sino a `CVehiclePool::GetAt`, así que además hay que parchear el cuerpo de la DLL
  (`Resolve(bit_cast<uintptr_t>(&CVehiclePool::GetAt))`), igual que hace este banco con sus colaboradores reconstruidos.
- `DependenciesTask12` (`HELPERS` incluye 0x61A3A0) y cualquier banco de tareas que parchee 0x61A3A0 como ayudante nativo: ahora es un hook (`CTask::Destructor`); el EXE sigue entrando por 0x61A3A0 pero hacia la DLL salvo que el banco apague el hook.
- `CVehC` (lista `cbs`, entrada `FindReplCB` 0x6D3490 con `Addr(&FindReplacementUpgradeCB)`): la función de la DLL deja de ser un envoltorio del EXE; revisar qué compara.
- `CppIntegerNext/Hierarchy` y `DependenciesWorld12` (GetAnimHierarchyFromFrame 0x734AB0): exigen la 0x734A70 nativa y sin salto; siguen siendo válidos porque la 0x734A70 NO se engancha.
- Funciones ya validadas cuyo código cambia: `CCamera::TryToStartNewCamMode`, `CPlane::DoPlaneGenerationAndRemoval`, `CTaskSimpleFight` (destructor y manejador de excepciones `FightUnwind`) y `RwHelper::GetAnimHierarchyFromFrame`.
- Hallazgo colateral (fuera de este lote): el hook `CGarages::FindSafeHouseIndexForGarageType` (0x4489F0, ya existente) escribe ECX y EDX en Debug, y el original solo EAX; `0x44A3C0` (en el EXE) lee ECX tras llamarla (`xor ecx,ecx ; call 0x4489F0 ; cmp word [eax+0x12],cx`).
