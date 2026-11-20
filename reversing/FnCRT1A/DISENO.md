# CRT1A: diseño del montón compartido (19/11/2026)

Pregunta (IND1A, sección 8): al sustituir el CRT del EXE por el propio, qué montón comparten el código propio y las estructuras que el original sigue liberando, sin mezclar `new` del EXE con `delete` propio.

## 1. Lo que hace hoy el CRT del EXE (desensamblado, `gta_sa.exe` SHA256 `72ae59e4…ecc9b`)

| Pieza | Dirección | Comportamiento |
|---|---|---|
| `_heap_init` | 0x82C471 | `_crtheap (0xC9C2F4) = HeapCreate(flags, 0x1000, 0)` (flags = `HEAP_NO_SERIALIZE` solo si el argumento es 0); `__active_heap (0xC9C2F8) = __heap_select()` |
| `__heap_select` | 0x82C457 | 1 (montón del sistema) si `_osplatform == 2` y `_winmajor >= 5`; si no, 3 (montón de bloques pequeños V6). **En Windows 2000 o posterior vale siempre 1.** |
| `_heap_alloc` | 0x8241B0 | con `__active_heap == 1`: `HeapAlloc(_crtheap, 0, size ? size : 1)`; sin cabecera propia, sin redondeo a 16 (el redondeo y las llamadas a `__sbh_*` solo se hacen con 3) |
| `malloc` / `_nh_malloc` | 0x824257 / 0x82422B | `size > 0xFFFFFFE0` falla sin llamar al sistema; en fallo, si `_newmode (0xC9ADCC) != 0` llama a `_callnewh` (0x826A57, manejador en 0xC9AC68) y reintenta |
| `free` | 0x82413F | puntero nulo no hace nada; si no, `HeapFree(_crtheap, 0, p)` |
| `realloc` / `calloc` | 0x824269 / 0x824416 | `HeapReAlloc(_crtheap, 0, …)` / `HeapAlloc(_crtheap, HEAP_ZERO_MEMORY, n*m)` (producto de 32 bits sin comprobar desbordamiento) |
| `operator new` / `new[]` | 0x82119A / 0x821195 (salta) | bucle `malloc`; si falla y `_callnewh` devuelve 0, llama a 0x8212ED (lanza `std::bad_alloc`) |
| `operator delete` / `delete[]` | 0x8214BD / 0x8213AE (saltan a 0x82413F) | = `free` |

El proyecto ya sigue una regla: lo que el original puede liberar se reserva con el CRT del EXE (`CMemoryMgr::Malloc/Free/Realloc/Calloc/MallocAlign` llaman por dirección a 0x824257, 0x82413F, 0x824269, 0x824416; las clases con `operator new` propio, como `CColModel`, `CPedAttractor` o `LinkList`, pasan por `CMemoryMgr`). En el modo EXE independiente (IND1A) el `operator new` global del proyecto llama a 0x82119A. En el modo ASI, en cambio, el `new`/`delete` global de la DLL usa el montón del CRT de la propia DLL: un objeto creado así y liberado por el EXE (o al revés) revienta.

## 2. Decisión

**Un solo montón, el `_crtheap` del EXE, y las funciones propias trabajan SOBRE ese mismo manejador.** Razones:

1. En Windows 2000+ el CRT del EXE es un envoltorio fino de `HeapAlloc/HeapReAlloc/HeapFree` sobre un manejador, sin cabecera ni metadatos propios. Un bloque pedido por la versión original se puede liberar, redimensionar o medir (`HeapSize`) con la propia, y al revés, **sin ninguna conversión**: mezclar `new` original con `delete` propio es seguro mientras las dos usen ese manejador y no añadan cabecera ni redondeo.
2. Por eso las sustituciones se pueden enganchar **de una en una** (orden libre, sin ola de "todos a la vez"): no hay estado intermedio incoherente. Los bloques reservados antes de instalar los ganchos (inicializadores estáticos del CRT original) se liberan igual.
3. No hay que parar el juego ni emitir un nuevo montón: el riesgo conocido (dos montones distintos) no existe porque no se crea ninguno.

Implementación (`source/game_sa/Crt/CrtHeap.{h,cpp}`): `ExeCrt::Malloc/NhMalloc/Free/Realloc/Calloc/OperatorNew` leen `_crtheap`, `_newmode` y el manejador de new de las direcciones del EXE (0xC9C2F4, 0xC9ADCC, 0xC9AC68) y exigen `__active_heap == 1` (`CrtHeapIsSystemHeap()`; con 3 no se instalan ganchos y todo sigue en el CRT original: el montón de bloques pequeños no se reproduce).

## 3. Reglas de uso (las que evitan el reventón)

- **Prohibido** liberar con el `delete`/`free` de la DLL (CRT del compilador) un bloque que venga del EXE o de `CMemoryMgr`, y a la inversa. Hay que seguir usando `CMemoryMgr`/`ExeCrt::*` para todo lo que cruce la frontera.
- Para que el código propio nuevo no tenga que acordarse: **sustituir el `operator new/delete` global de la DLL por `ExeCrt::OperatorNew`/`ExeCrt::Free`** (en el modo ASI; en el modo EXE ya apunta a 0x82119A). Con eso cualquier `new` del proyecto cae en el montón del juego. No se hizo en este lote (cambia el comportamiento de todo el proyecto y exige la prueba dentro del juego: ver `EJECUTAR.md`).
- Orden de sustitución propuesto cuando haya prueba dentro del juego: (a) ganchos de `free`/`delete` (inofensivos si todos los productores usan el mismo manejador), (b) `malloc`/`calloc`/`realloc`/`new`, (c) `_heap_init` propio que cree el montón y publique el manejador en 0xC9C2F4 (y `__active_heap = 1`), (d) cuando no quede código original que lea esas dos variables (0x8241B0, 0x82413F, 0x824269, 0x824416, 0x82C4xx y las rutinas `__sbh_*` que nunca se ejecutan), pasarlas a variables propias y retirar los ganchos.
- Sin SBH: `_heap_select` devuelve 1 en cualquier Windows actual; si alguna vez devolviera 3 el gancho no se instala (comprobación explícita).

## 4. Riesgos y qué no cubre

- **Serialización:** `_heap_init(0)` crea el montón con `HEAP_NO_SERIALIZE`; si el juego lo llamara así habría que respetarlo. Las llamadas de `HeapAlloc/HeapFree` de las versiones propias van al mismo manejador con `flags = 0`, igual que el EXE, así que el comportamiento es el mismo en ambos casos. No se verificó en ejecución cuál de los dos casos usa el juego (hace falta el juego).
- **Excepciones:** el `operator new` propio, sin memoria y sin manejador, llama a 0x8212ED (el lanzamiento original); la ruta no se ejercita en el banco (haría falta el juego o un SEH real).
- **Fuera del alcance de este lote:** `HeapSetInformation`, el montón de baja fragmentación (LFH) y demás opciones del sistema no se tocan (el EXE no las pide; el sistema decide).
- **Lo que prueba el banco** (ver `README.md`): que, ante cualquier secuencia de `malloc/calloc/realloc/free/new/delete`, el resultado observable (nulo o no, tamaño real del bloque en el montón, contenido, número de llamadas al manejador de new) coincide con el del EXE sobre un montón equivalente. **Lo que no prueba:** el comportamiento con el resto del juego (hilos, fragmentación, rendimiento): para eso están los pasos de `EJECUTAR.md`.
