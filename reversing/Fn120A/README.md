# Fn120A: cinco metodos virtuales diminutos (lote 120A)

Estado: **APROBADO dentro del juego** (07/11/2026, worktree `wv-1` sobre main 5bf6a9cd00 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn120a/20261107-224324-151029` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn120A-20261107-224324-151029`, `comparisons.jsonl.gz` de 6 MB). Banco par a par dentro del juego, 5 x 16384 = 81920 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, x87 igual y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).

Plantilla del banco: `Fn118A` (misma familia que `Fn94A`/`Fn100A`: `Gate` de ABI, `HookRoute`, parches de entrada); aqui se quitan los colaboradores, los falsos y los trampolines porque ninguna de las cinco funciones llama a nada.

| Funcion | Direccion | Bytes | Vtable (ranura) | Fuente | Listado de pendientes decia |
|---|---|---|---|---|---|
| `CAEPedlessSpeechAudioEntity::AddScriptSayEvent` | 0x4E6080 | 3 | 0x85F330 (1) | `Audio/Entities/AEPedlessSpeechAudioEntity.h/.cpp` | `CAEPedlessSpeechAudioEntity` (acierto) |
| `CTaskComplexGangJoinRespond::GetTaskType` | 0x661710 | 6 | 0x86FB5C (4) | `Tasks/TaskTypes/TaskComplexGangJoinRespond.h/.cpp` | `CTaskComplexGangJoinRespond` (acierto) |
| `CTaskComplexKillPedOnFoot::GetTaskType` | 0x620EE0 | 6 | 0x86D894 (4) | `Tasks/TaskTypes/TaskComplexKillPedOnFoot.h/.cpp` | `CTaskComplexKillPedOnFoot` (acierto, confianza baja) |
| `CTaskComplexPartner::GetTaskType` | 0x681F10 | 6 | 0x870664 (4) | `Tasks/TaskTypes/TaskComplexPartner.h/.cpp` | `CTaskComplexPartner` (acierto, confianza baja) |
| `CTaskComplexPartnerDeal::GetTaskType` | 0x681DF0 | 6 | 0x870754 (4) | `Tasks/TaskTypes/TaskComplexPartnerDeal.h/.cpp` | `CTaskManager` (**mal atribuida por cercania**) |

Las cinco tenian clase con archivo propio y vtable inequivoca: se recorrio el PE del EXE y cada una de las cinco direcciones aparece **una sola vez** en una vtable (las del listado eran correctas salvo 0x681DF0, que esta en la vtable de `CTaskComplexPartnerDeal` en 0x870754, ranura 4, y no tiene nada que ver con `CTaskManager`). Ninguna tenia hook ni estaba en `docs/hooks.csv`. Ninguna se dejo fuera.

## Revision contra el EXE
- **AddScriptSayEvent (0x4E6080, thiscall)**: `C2 14 00` = `ret 14h`: no hace nada y limpia los cinco argumentos de pila (`audioEvent`, `scriptID`, `overrideSilence`, `isForceAudible`, `isFrontEnd`). Es la ranura 1 de `CAEPedlessSpeechAudioEntity` (override vacio de `CAEPedSpeechAudioEntity::AddScriptSayEvent`, 0x4E4F70).
- **GetTaskType (0x661710 / 0x620EE0 / 0x681F10 / 0x681DF0, thiscall)**: `mov eax, K; ret` con K = 0x4C3 = 1219 (`TASK_COMPLEX_GANG_JOIN_RESPOND`), 0x3E8 = 1000 (`TASK_COMPLEX_KILL_PED_ON_FOOT`), 0x100 = 256 (`TASK_COMPLEX_PARTNER`) y 0x4B2 = 1202 (`TASK_COMPLEX_PARTNER_DEAL`); las constantes coinciden con `eTaskType` y con el `Type` de cada clase. Ninguna lee `this` ni memoria.
- Los llamadores del EXE solo leen EAX en las `GetTaskType`; en `AddScriptSayEvent` ningun registro.

## Cambios de produccion
Una linea por clase en su `InjectHooks()` (los cuerpos ya estaban en C++ puro, en linea en la cabecera, como `GetPedType` o `GetAllocatedVoice` de la misma clase de audio): `RH_ScopedVMTInstall(AddScriptSayEvent, 0x4E6080)` y `RH_ScopedVMTInstall(GetTaskType, 0x661710 / 0x620EE0 / 0x681F10 / 0x681DF0)`. Es un hook virtual (tipo V): redirige la ranura de la vtable del EXE (los objetos que construye el EXE llaman por ella) y enlaza la entrada de la funcion (llamadas directas). No cambia `source/CMakeLists.txt` (ni x87 ni contrato de registros: devuelven una constante o nada) ni `X87Math`.
Filas en `docs/hooks.csv` (convencion de los hooks virtuales: direccion `0x00000000`, tipo `V`; la direccion real consta aqui y en `evidence.original_address` del registro): `CAEPedlessSpeechAudioEntity,AddScriptSayEvent`, `CTaskComplexGangJoinRespond,GetTaskType`, `CTaskComplexKillPedOnFoot,GetTaskType`, `CTaskComplexPartner,GetTaskType`, `CTaskComplexPartnerDeal,GetTaskType`. El encargo pedia el formato `...,S` con la direccion; como el hook es virtual se uso la convencion de las filas V existentes (1692).

## Banco
Sin colaboradores, sin falsos y sin memoria real del juego: cada caso llama a la funcion con un objeto sintetico (arena propia con guardas de 0x40, mas una arena centinela) y compara EAX (solo en las `GetTaskType`; `AddScriptSayEvent` es `void`), los bytes cambiados de ambas arenas (deben ser ninguno), los eventos (ninguno), las compuertas de ABI (ESP restaurado, EBX/ESI/EDI/EBP, DF, SEH) y el estado x87. EAX/ECX/EDX de salida solo se registran (`kCompareVolatileRegisters = false`): ningun llamador del EXE los lee salvo EAX de `GetTaskType`, que si se compara.
**Dos rutas por caso** (mitad y mitad, bit 4 del numero de caso): por la **entrada del EXE** (original = sus bytes con el hook apagado; candidato = el enlace de la entrada hacia la DLL) y por la **ranura de la vtable del EXE** (original = la ranura contiene la direccion del EXE; candidato = contiene una funcion del ASI): asi el banco invoca la direccion del EXE con un objeto sintetico pero tambien prueba lo que usan los objetos construidos por el EXE hasta que se enganche su constructor. `route_ok` exige en ambos lados la forma correcta de la entrada y de la ranura, y `watched_entries` vigila los 16 bytes de cada entrada y cada ranura (deben quedar intactos al final).
16 familias de 1024 casos por funcion: tipo de `this` (arena alineado, arena desalineado, nulo o crudo con 0xCDCDCDCD/0xFFFFFFFF/0x80000000/... y aleatorios impares, arena alineado con la vtable del EXE en +0) x relleno de la arena (aleatorio, ceros, unos, 0xCD); desplazamientos 0..0x3C; en `AddScriptSayEvent` ademas eventos y banderas (0, 1, 2, 0x80, 0xFF, 0x100, 0xFFFFFFFF, con basura en los bytes altos) con tandas de las tres banderas a 1 y a 0.

## Fuera de los fixtures
Objetos reales construidos por el EXE (ninguna de las cinco lee el objeto) y que la clase real sea la que dice la vtable (se confirmo por la vtable y el desensamblado, no por un banco con objetos reales).

## Bancos repetidos
`grep` de las cinco direcciones (con y sin ceros, en `.hpp/.cpp/.h/.py` de `reversing/`): **ningun banco antiguo cita 0x4E6080, 0x661710, 0x620EE0, 0x681F10 ni 0x681DF0**, asi que por la regla (repetir solo los que citan la direccion) no se repite ni se adapta ninguno por direccion.

## Prueba negativa
(se completa tras las dos rondas; ver el commit siguiente)

## Tiempo
Una corrida de los 81920 pares: unos 3 min (mas la compilacion incremental de la sonda).
