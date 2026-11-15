# Lote R1A: reconstrucción sin validar

Rama `reconstruccion-sin-validar`. Doce funciones pequeñas de RenderWare 3.7 enlazadas en el EXE (no son del juego): diez del módulo RtDict (diccionarios de plantillas), más `RwD3D9GetCaps` y `_rwD3D9DeviceGetRestoreCallback` del controlador D3D9 de RenderWare. **No hay banco**: no se ha ejecutado el juego ni se ha tocado `approvals.json` ni `validation-registry`. En `docs/hooks.csv` entran con `validated=0`. Cada cuerpo se revisó contra el desensamblado instrucción por instrucción.

Corrección al encargo: `0x7FAD20` no es de RpAnisot sino `RwD3D9GetCaps` (`mov eax,0C9BF00h; ret`; el propio `rpanisot.cpp` ya lo decía). Ninguna de las doce estaba enganchada en la rama (`git grep` de las direcciones y filas de `docs/hooks.csv`); las diez de RtDict y las dos de D3D9 eran envolturas por dirección fija (`rtdict.cpp`, `rwcore.cpp`) y se sustituyeron por el cuerpo en C++.

## Funciones

| Dirección | Nombre | Bytes | Tipo | Archivo | Riesgos que debe mirar el banco |
| --- | --- | ---: | :---: | --- | --- |
| `0x007CED40` | `RtDictSchemaInit` | 44 | S | `source/game_sa/RenderWare/rw/rtdict.cpp` | `_rwSListCreate(4, dictChunkType \| 0x40000)` (0x809160). Si falla devuelve NULL sin tocar `current`; si no, `current = NULL`. Comprobar `dictionaries` y `current` con arena sucia. |
| `0x007CEEF0` | `RtDictSchemaSetCurrentDict` | 12 | S | `rtdict.cpp` | `schema->current = dict` (+0x14); devuelve el esquema. Trivial. |
| `0x007CEF60` | `_rtDictDestruct` | 75 | S | `rtdict.cpp` | Recorre las entradas (end 0x809540 antes que begin 0x809530) llamando `schema->destroyCB` (+0x1C, cdecl, un arg) y destruye la lista de entradas (0x809440). Devuelve TRUE. No libera el `RtDict` ni toca el esquema. Comparar la secuencia de llamadas al callback. |
| `0x007CEFE0` | `RtDictFindNamedEntry` | 114 | S | `rtdict.cpp` | `getNameCB` (+0x20) se lee antes de recorrer; `rwstrcmp` (RwEngineInstance+0x114) se relee en cada vuelta (el EXE lo hace con `mov eax,[0C97B24h]` dentro del bucle) y se llama como `rwstrcmp(getNameCB(entry), name)`. Devuelve la primera coincidencia o NULL. Lista vacía devuelve NULL. |
| `0x007CF060` | `RtDictForAllEntries` | 85 | S | `rtdict.cpp` | `callBack(entry, data)` por entrada; sale al primer resultado NULL; devuelve SIEMPRE el diccionario recibido (las dos salidas del EXE cargan `[esp+14h]`). Orden de los argumentos del callback: entrada, data. |
| `0x007CF0C0` | `RtDictRemoveEntry` | 106 | S | `rtdict.cpp` | **Posible fallo del propio RW reproducido a propósito**: tras `destroyCB(*it)` hace `*it = _rwSListGetEntry(entries, _rwSListGetNumEntries(entries))` SIN restar 1 y SIN desreferenciar (0x7CF0FE..0x7CF114; 0x809510 devuelve `base + entrySize*n`), o sea guarda la dirección de la ranura una pasada del final, y luego `_rwSListDestroyEndEntries(entries, 1)` (0x809400). Lo contrario de `RtDictDestroy` (0x7CF16F..0x7CF18D), que resta 1 y desreferencia. Si el banco ve que el EXE de verdad hace esto, la copia es fiel; si alguna prueba lo contradice, revisar este punto primero. Devuelve el diccionario, o NULL si la entrada no está o la lista está vacía. |
| `0x007CF130` | `RtDictDestroy` | 182 | S | `rtdict.cpp` | Si `dict == schema->current` lo pone a NULL; lo quita de la lista del esquema (begin 0x809530, end 0x809540; hueco = contenido de la última, `_rwSListGetEntry(n-1)` desreferenciado; `DestroyEndEntries(1)`); `destroyCB` por entrada propia (end antes que begin; `dict->schema` releído en cada vuelta); `_rwSListDestroy(entries)`; `RwFree(dict)` = `[RwEngineInstance+0x138]`. Devuelve TRUE. Colaboradores: listas 0x809xxx y la función de memoria del motor (cdecl). |
| `0x007CF240` | `RtDictSchemaStreamReadDict` | 579 | S | `rtdict.cpp` | Cabecera con `RwStreamFindChunk(stream, 1, &length, &version)` (0x7ED2D0); `version < compatibilityVersion` (sin signo) delega en `streamReadCompatibilityCB` (+0x2C) y devuelve lo que éste devuelva. `numEntries` se lee con `RwStreamRead(stream, &numEntries, length)` (0x7EC9D0) pidiendo `length` bytes (en el EXE el destino es la ranura del argumento de pila: un `length > 4` desborda igual; en C++ desborda una variable local de 4 B, vigilar con flujos de prueba de longitud 4). `RwMalloc(8, dictChunkType\|0x30000)` (+0x134); si falla: `RwError{pluginID=0x1BF, errorCode=_rwerror(0x80000013, 8)}` + `RwErrorSet` (0x8088D0 y 0x808820, se llaman por función) y NULL. Lista de entradas (`_rwSListCreate(4, hint)`); fallo: `RwFree` y NULL sin error. Alta en la lista del esquema (`_rwSListGetNewEntry`); fallo: destruye lista, `RwFree`, NULL. Por entrada: `FindChunk(entryChunkType)`, `streamReadCB` (+0x28), `GetNewEntry(dict->entries, dict->schema->dictChunkType\|0x30000)`, `addRefCB` (+0x18) y `destroyCB` (+0x1C) (suelta la referencia nominal). Los fallos intermedios llaman a `RtDictDestroy` (el EXE los repite en línea con la misma secuencia 0x7CF3CA..0x7CF46F y devuelve 0; el resultado y las llamadas son idénticos; no se pasa por el hook del EXE sino por el C++). Banco: flujo de memoria con 0, 1 y N entradas, entrada corrupta, versión antigua, y fallo de asignación. SEH: ninguno en el EXE. |
| `0x007CF490` | `RtDictStreamWrite` | 283 | S | `rtdict.cpp` | Tamaño = `0x10 + sum(0xC + streamGetSizeCB(entry))` (+0x24); si `size == 0` tras un recorrido no vacío (envoltura de 32 bits) devuelve NULL. Dos cabeceras con `_rwStreamWriteVersionedChunkHeader` (0x7ED270): (`dictChunkType`, size, `0x36003`, `0xFFFF`) y (STRUCT=1, 4, `0x36003`, `0xFFFF`); número de entradas con `RwStreamWrite(.., 4)` (0x7ECB30) y `streamWriteCB` (+0x30, `(entry, stream)`); cualquier fallo devuelve NULL. Devuelve el diccionario. Comparar los bytes escritos en un flujo de memoria. |
| `0x007CF5B0` | `RtDictSchemaForAllDictionaries` | 75 | S | `rtdict.cpp` | `callback(dict, data)` por diccionario de `schema->dictionaries` (+0x10); sale al primer NULL; devuelve TRUE siempre (`mov eax,1`), no el esquema. |
| `0x007FAD20` | `RwD3D9GetCaps` (el encargo decía RpAnisot) | 6 | S | `source/game_sa/RenderWare/rw/rwcore.cpp` | `mov eax,0C9BF00h; ret`: devuelve la dirección fija del D3DCAPS9 de RenderWare. Lo usa `RpAnisotGetMaxSupportedMaxAnisotropy` (0x748F20) y ya está enganchada en la rama; ahora llama a la reconstruida. |
| `0x007FAE30` | `_rwD3D9DeviceGetRestoreCallback` (rwD3D9DeviceRestoreCallBack) | 6 | S | `rwcore.cpp` | `mov eax,[0C980B0h]; ret`: devuelve el puntero guardado por `_rwD3D9DeviceSetRestoreCallback` (0x7FAE20, sin hook). `WindowedMode.cpp` tiene el mismo dato como `D3D9RestoreDeviceCallback`. |

Las dos últimas se registran con `RH_ScopedNamespaceName("RwD3D9")` en la función nueva `RwD3D9InjectHooksR1A` (`rwcore.cpp`, declarada en `RenderWare.h`, llamada desde `InjectHooksMain.cpp` junto a `RtDictInjectHooks`); las diez de RtDict van en `RtDictInjectHooks`. Clases en `docs/hooks.csv`: `RtDict` y `RwD3D9`.

## Notas de revisión (sin banco)

- x87: ninguna de las doce toca coma flotante. ABI: todas cdecl con `ret` simple (resultado en EAX). SEH: ninguna lo usa.
- Las listas (`_rwSListCreate`, `GetNewEntry`, `GetBegin`, `GetEnd`, `GetNumEntries`, `GetEntry`, `DestroyEndEntries`, `Destroy`) están ya reconstruidas en `rwplcore.cpp`; se llaman por su nombre C++ (si estuvieran enganchadas, el banco debe tenerlo en cuenta).
- Orden de colaboradores: se conserva el del EXE (end antes que begin salvo en las búsquedas de `RtDictDestroy`, que van begin y luego end). Ninguna de las listas escribe en `GetBegin/GetEnd`, así que el orden solo se vería si se enganchasen.
- Las funciones vecinas de RtDict que siguen siendo envolturas por dirección (`RtDictSchemaCreateDict` 0x7CED90, `AddDict` 0x7CEE50, `RemoveDict` 0x7CEE80, `RtDictAddEntry` 0x7CEFB0, `RtDictStreamGetSize` 0x7CF1F0, `_rtDictSchemaInitDict` 0x7CEF00) no eran de este lote.

## Saltadas

Ninguna.

## Bancos viejos que citan las direcciones

Búsqueda con `git grep` de cada dirección sobre `reversing/` (sin `runs/`, ni `.csv/.txt/.json/.md`). No se repiten.

| Dirección | Bancos que la citan |
| --- | --- |
| `0x7CEEF0` | StreamingFinal4 |
| `0x7CF130` | StreamingFinal4 |
| `0x7CF240` | StreamingFinal4 |
| `0x7FAD20` | Dependencies25B, EnvMap37, Misc70, RequestedFxEmitterTwo |
| `0x7FAE30` | Game22, RequestedFxEmitterTwo |
| resto (0x7CED40, 0x7CEF60, 0x7CEFE0, 0x7CF060, 0x7CF0C0, 0x7CF490, 0x7CF5B0) | ninguno |
