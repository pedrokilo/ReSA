<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlayerAnim80D: CPlayerPed::ProcessAnimGroups 0x6098F0 (lote 80D)

Estado: **APROBADO dentro del juego** (28/10/2026, worktree `D:\Original\wz-4`, HEAD e4f077aea + `source.patch`). 20480 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados y CADA lado igual al oraculo independiente (`fixture_oracle.py`).
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-playeranim80d\20261028-120549-837428` (la primera corrida con el analizador corregido; la anterior 20261028-114622-804251 tambien pasaba el analisis a mano).

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `void CPlayerPed::ProcessAnimGroups()` (antes `plugin::CallMethod<0x6098F0>`, sin hook) | 0x6098F0 | `CPlayerPed` / `ProcessAnimGroups` / S (`RH_ScopedInstall`) | `Entity/Ped/PlayerPed.cpp` |

Fila de `docs/hooks.csv` (la anade el coordinador): `CPlayerPed,ProcessAnimGroups,0x006098F0,1,0,1,S`.

## Tamano real
La funcion mide 909 bytes (0x6098F0..0x609C7C; `ret` en 0x609C33 y un bloque de cola hasta 0x609C7C), no los ~987 del encargo. Unico llamador del EXE: 0x60F2E7 (`CPlayerPed::ProcessControl`), que no lee EAX/ECX/EDX de salida (preserva EBX/EBP/ESI).

## Que hace (thiscall sin argumentos, void; sin aritmetica float salvo dos comparaciones)
1. Rumbo `[datos+0x50]` (datos = `[this+0x480]`, releidos): se mira la camara si NO (rumbo > -0.8726646 [0x86D1D4]) o NO (rumbo < +0.8726646 [0x86D1D0]); NaN siempre la mira. Camara activa = byte 0xB6F081 (0x238 bytes por camara) -> `CCam::Using3rdPersonMouseCam` (0x50A850) y, si AL != 0, `CanStrafeOrMouseControl` (0x5DF090). Si ambas: edi = grupo actual `[+0x4D4]` (instantanea); 0x36 -> rama del modelo; otro -> `edi + GetDefaultPlayerMotionGroup() - 0x36`.
2. Si no: edi = 0; con `[+0x4F4]` (objeto del arma): `GetClumpModelInfo` (0x732AC0), `vcall +0x10` (GetModelType, solo AL == 4) -> edi = `[modelo+0x24]` (tipo de arma; si no es un modelo de arma queda lo que hubiera: 0 si no habia modelo). 0x23/0x24 -> 0x39. `GetTaskJetPack` (0x601110, puntero) -> grupo 0x46. Listas de armas: 5,6,7 -> 0x3F; 9,0x25,0x26 -> 0x42; 0x1F,0x1E,0x1B,0x19,0x22,0x21 -> 0x3C. Si no, balaclava (0x5A7950 sobre `[datos+4]`) -> 0x45, si no edi = 0x36.
3. Rama del modelo (0x609A1F): con `[+0x598] == 1` (dword) el grupo de `ms_modelInfoPtrs[(int16)[+0x22]]+0x24` si no es 0x36; si no, rama por defecto.
4. Si `[+0x4D4]` (releido) difiere, se guarda y se llama a `ReApplyMoveAnims` (0x609650).
5. Bloques de animacion de pelea: tipo del arma de la ranura con signo `[+0x718]` (+0x5A0 + ranura*0x1C, sin comprobar el rango), ANTES de `GetWeaponSkill`; `GetWeaponInfo`. **Arma que no es cuerpo a cuerpo, o bit 0x100 de `[+0x46C]` (en vehiculo): salta a 0x609BBF, es decir libera AMBOS bloques** (+0x48 y +0x4C si valen != 0), sin pedir nada. Si no: combo base (`[info+0x6E]`, byte con signo) y estilo de pelea (`[+0x72D]`, byte sin signo) piden su grupo (tabla 0xC170D0, 0x88 bytes por fila, fila = valor-4; 4 = estandar: solo marca liberar si habia referencia): si el grupo difiere del referenciado, marca liberar el anterior, toma el bloque de `ms_aAnimAssocGroups[grupo]` (nulo -> `GetAnimBlockName` + `GetAnimationBlock`), indice = (bloque - 0xB5D4A0) >> 5; con `IsLoaded == 1` (byte) y referencia vacia: `AddAnimBlockRef` y se guarda el grupo; si no esta cargado: `RequestModel(indice + 0x63E7, 8)`. Al final se sueltan (`RemoveAnimBlockRef`) los bloques marcados y se ponen a 0. Los datos del jugador y los grupos se releen tras cada llamada.

## Cambios de produccion (`source.patch`)
`PlayerPed.cpp`: metodo en C++ puro con `RH_ScopedInstall(ProcessAnimGroups, 0x6098F0)` e include de `TaskSimpleFight.h` (tabla de combos). `CAnimManager::ms_aAnimBlocks/ms_aAnimAssocGroups` son privados: se leen por sus direcciones (0xB5D4A0 / 0xB4EA34, este ultimo releido en cada uso). Sin cambios en X87Math (no hay aritmetica x87). Las comparaciones del rumbo se escriben como el EXE (NaN -> mira la camara).
Defecto evitado en la lectura: la primera version de este lote solo liberaba el 2o bloque en la rama de arma no cuerpo a cuerpo; el EXE salta al cuerpo de liberacion del 1o (lo detecto el propio banco: 10673 diferencias).

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (registran argumentos EN ORDEN y mutan en 15 sitios): Using3rdPersonMouseCam, CanStrafeOrMouseControl, GetClumpModelInfo, GetTaskJetPack, GetIsWearingBalaclava, GetDefaultPlayerMotionGroup, ReApplyMoveAnims, GetWeaponSkill(), GetWeaponInfo, GetAnimBlockName, GetAnimationBlock(const char*), AddAnimBlockRef, RequestModel (todos con hook) y `CAnimManager::RemoveAnimBlockRef` 0x4D3FD0 (funcion de la DLL sin hook: falso en el EXE y en su cuerpo, `hooked=false`, sin anadir un hook nuevo). GetModelType se ejerce por una tabla virtual falsa (ranura +0x10; las demas ranuras registran un evento 90+ranura para detectar una ranura equivocada).
Bloques reales de `ms_aAnimBlocks` 0..7 (solo se escribe/restaura el byte IsLoaded), arreglo de grupos 0xB4EA34 apuntado a una arena (24 grupos), tabla de combos 0xC170D0, `ms_modelInfoPtrs[0..7]` y camara activa se fijan por caso y se restauran (regiones). Constantes 0x86D1D0/0x86D1D4 vigiladas.
20480 pares: 16 familias de 1024 (generica, camara+strafe con grupo 0x36 / otro, sin strafe, rumbo interior, lanzacohetes, jetpack, listas de armas, balaclava, modelo del 2o jugador, arma no cuerpo a cuerpo / en vehiculo, combo/estilo estandar, combo nuevo, estilo nuevo, ambos con IsLoaded variado, mutaciones) + rumbo (NaN, inf, +-0, desnormales, limites exactos y vecinos) + tipo de modelo (AL + basura) x tipo de arma 0..0x2F + barrido 15 sitios x 31 tipos de mutacion + ranuras con signo (-20..20, byte). Mutaciones: cambio del puntero a los datos (+0x480), de +0x48/+0x4C, del bloque de un grupo y del grupo de animacion del jugador.
Las comparaciones de punto flotante pasan por el hardware real en el EXE (fld/fcomp) y en la DLL (SSE); el oraculo las evalua en Python con floats exactos.

## Pruebas
- Negativa 1 (produccion mutada y restaurada): `<` cambiado por `<=` en el limite superior del rumbo: **413/20480 diferencias de resultado** (corrida 20261028-122554-041094), sin caida.
- Negativa 2: el puntero a los datos del jugador cacheado antes de `AddAnimBlockRef` (el EXE lo relee): **13/20480** (corrida 20261028-123553-947711).
- Hallazgos de la primera corrida: 10673 diferencias por (a) el defecto de lectura descrito y (b) el banco devolvia basura de 32 bits en `GetWeaponSkill` (el EXE la empuja entera, la DLL la trunca a eWeaponSkill): el real devuelve un valor pequeno; el fixture se limito a 0..3.

## Quien valida a cada colaborador y que NO prueba el banco
Cada colaborador tiene su banco (docs/hooks.csv, reversing/validation-registry.json). Sin banco propio: `CAnimManager::RemoveAnimBlockRef` (sin hook, sustituida). No cubre: datos del jugador, ropa o inteligencia nulos (el EXE se cae); grupos fuera de 0..23, bloques fuera de 0..7, bloque de `GetAnimationBlock` nulo, filas de la tabla de combos fuera de 4..15, ranuras fuera de -20..20, `GetWeaponSkill` con basura en los bits altos, indice de modelo fuera de 0..7; EAX/ECX/EDX de salida (nadie los lee); RequestModel/AddAnimBlockRef reales.

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh PlayerAnim80D`; `python -B reversing/PlayerAnim80D/run_local.py --analyze <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>` (solo el coordinador).
