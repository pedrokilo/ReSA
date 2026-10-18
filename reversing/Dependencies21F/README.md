<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencies21F — D3DResourceSystem::TidyUpD3DTextures y TidyUpD3DIndexBuffers

Grupo F de los 21 destinos inciertos de la cohorte de 499 (PLAN-001): `D3DResourceSystem::TidyUpD3DTextures` (0x7305E0) y `D3DResourceSystem::TidyUpD3DIndexBuffers` (0x730740). Ambas eran envoltorios (`plugin::Call<...>`) y pasan a ser C++ puro (sin `__asm` ni `naked`) con hook RH (`RH_ScopedInstall`); el banco las valida par a par dentro del juego: EXE con el hook apagado frente a la DLL con el hook activo. El desensamblado completo del EXE (verificado contra `gta-sa.exe`) esta en `TidyUpD3DTextures.dis.txt` y `TidyUpD3DIndexBuffers.dis.txt`.

## Estructuras (de 0x730830 Init, 0x730510/0x7306A0 y los dos cuerpos)

- `NumTextureBuffers` (0xC87C60, int32; Init lo pone a 1, CreateTexture lo sube hasta 17) y `TextureBuffers[17]` (0xC87C68, `D3DTextureBuffer` de 0x1C bytes). `FreeTextureBufferIndex` (0xC87FE0).
- `IndexDataBuffers[16]` (0xC87E48, `D3DIndexDataBuffer` de 0x18 bytes), el buffer generico `IndexDataBuffer` (0xC87FC8) y `FreeIndexBufferIndex` (0xC87FE4).
- En cada buffer `m_nSize` es el tope de la pila de punteros (`m_apTextures[m_nSize-1]` es el siguiente) y `m_nNumTexturesInBuffer`/`m_nNumDatasInBuffer` es un contador total. Ambos se decrementan **antes** de mirar el puntero, de modo que un puntero nulo consume la entrada igualmente.
- Un recurso se libera con `call dword ptr [edx+8]` (`IUnknown::Release`, `push this`).

## Algoritmo

- `TidyUpD3DTextures(count)`: si `count == 0` sale. Recorre en circulo los `NumTextureBuffers` buffers empezando en `FreeTextureBufferIndex` (sin comprobar el rango); si la pila del buffer actual no esta vacia saca la ultima textura (contadores--), la libera si no es nula y, tras el Release, **vuelve a leer** `NumTextureBuffers` y `FreeTextureBufferIndex` de memoria, pone a cero el numero de pasos vacios y descuenta una cuenta. El indice avanza `(idx+1) % NumTextureBuffers` (idiv con signo) y se guarda en cada paso. Termina al llegar a `count` liberaciones o tras `NumTextureBuffers` pasos seguidos sin liberar nada.
- `TidyUpD3DIndexBuffers(count)`: primero vacia la pila del buffer generico (0xC87FC8), igual que arriba pero sin indice; una pila vacia o un puntero nulo la termina. Despues recorre los 16 buffers desde `FreeIndexBufferIndex` con el mismo esquema (`(idx+1) % 16`, tope de 16 pasos vacios, indice recargado tras cada Release). Con el indice 16 se lee el buffer generico como ranura 17 (sin comprobacion de rango en el original).
- Estado que dejan: contadores/tope de cada buffer tocado, `FreeTextureBufferIndex` / `FreeIndexBufferIndex`, y los objetos liberados. No tocan nada mas. Registros: salen `ebx`, `esi`, `edi` y `ebp` conservados, pila limpia (`ret` cdecl); EAX/ECX/EDX son residuo de la ultima instruccion (despues de un Release, el que deja la implementacion COM).

## Fuente

`source/game_sa/RenderWare/D3DResourceSystem.cpp/.h` (ver `gf.patch`): se definen los globales (los que el `.h` ya declaraba mas `NumTextureBuffers`, `TextureBuffers[17]`, `IndexDataBuffers[16]`, `FreeIndexBufferIndex`), `TidyUpD3DTextures` y `TidyUpD3DIndexBuffers` y sus dos hooks. El archivo ya se compila entero con `/O2 /Oi /Oy /arch:IA32 /fp:fast` y sin PCH/unity/RTC por `source/CMakeLists.txt` (para que `SetUseD3DResourceBuffering` mida 10 bytes como el EXE): las dos funciones nuevas no usan float, asi que esas opciones no cambian su semantica, y no hace falta ninguna entrada nueva en CMake. Release se llama por la vtable (`(*(ReleaseFn**)p)[2](p)`, stdcall) para no depender de `d3d9.h`.

## Residuo de registros (EAX/ECX/EDX)

No se reproduce. Los unicos llamadores del EXE son `CGame::TidyUpMemory` (0x53C518/0x53C522), `CGame::DrasticTidyUpMemory` (0x53C812) y `CMenuManager::Process` (0x57B465/0x57B46C); los tres estan reconstruidos y con hook (docs/hooks.csv) y ninguno lee EAX/ECX/EDX tras la llamada (ver `gf-classification.md`). Ademas, tras un Release el residuo de ECX/EDX/EAX es el de la implementacion COM de D3D9 (no determinista entre drivers), por lo que no es una propiedad reproducible. El banco **no compara** EAX/ECX/EDX: los registra en cada fila (`registers_not_compared`) como informacion, y compara ESP, EBX/ESI/EDI/EBP, FNSAVE completo (TOP, FCW, FSW, FTW, registros ST) y MXCSR.

## Banco

- 2 x 2048 pares (4096). Combinaciones (cuenta x llenado x mutacion) = 512, por 4 variantes aleatorias deterministas. Cuentas: 0, 1, 2, 3, 4, 5, 7, 17, 33, 64, 100, 200, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFE, 0xFFFFFFFF. `NumTextureBuffers` 1, 2, 3, 5, 9, 16, 17 y aleatorio; indices libres dentro y fuera de `NumTextureBuffers` (hasta 16) y el buffer generico como ranura 16 en indices; pilas de 0 a 60 entradas; punteros nulos (1/8), repetidos (el mismo objeto en varias ranuras) y relleno de veneno (0xDEAD0000) tras el tope; contadores totales `m_nNum...` extremos (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFE).
- Mutaciones dentro de Release (el objeto falso cambia `NumTextureBuffers`, `FreeTextureBufferIndex` y/o `FreeIndexBufferIndex` en el Release n.o 1..7): comprueban que ambas versiones recargan los globales tras cada llamada.
- Ningun recurso real se libera: las cabeceras de las caches (0xC87C60..0xC87FE8) apuntan a listas de punteros del banco y a 2048 objetos COM falsos (vtable de tres ranuras; QueryInterface/AddRef llamados = fallo). Se guardan y restauran la region de globales y las listas reales a las que apuntaba (`real_lists_protected`).
- Cada lado registra: region de globales completa (hex), traza de Release (objeto y hash FNV de la region en el momento de la llamada, en orden), numero de Release, huella FNV-64 de toda la memoria observable (region + 35 listas + pool de objetos con sus contadores) y se compara ademas **byte a byte** en C++ (`memory_bytes_equal`).
- `fixture_oracle.py` repite el desensamblado con enteros de Python a partir de la fixture registrada y exige que EXE y DLL coincidan con el, no solo entre si (traza de objetos, contadores y tope de cada buffer, indices libres y `NumTextureBuffers`), y que se recorran todas las rutas (cuenta 0, pasos agotados, cuenta alcanzada, entrada nula, buffer vacio, vuelta del indice, recarga tras Release, generico vacio / nulo / con liberaciones, ranura 16, ...).
- Compuertas por lado: `stack_restored`, `nonvolatile_preserved`, `route_ok` (EXE sin salto en OFF; salto hacia la DLL en ON y destino en el modulo del ASI), `com_calls_ok`, `globals_restored`, `floating_state_restored`; el hook RH se restaura al final.
- Colaboradores: la unica llamada saliente es el Release indirecto, que cae en los objetos falsos del banco; no hay llamadas directas a funciones del EXE, asi que no hacen falta `EntryPatch` de colaboradores. `helper_routes` queda vacio.

## Comprobacion fuera del juego (offline/)

`offline/harness.cpp` carga en memoria ejecutable los bytes reales del EXE (0x7305E0..0x730656 y 0x730740..0x7307EC) y los compara con el C++ del parche sobre globales falsos mapeados en 0xC87C60, con objetos COM falsos y 2 x 200000 casos aleatorios (memoria completa y traza con hash de los globales en cada Release): 0 diferencias. Una mutacion deliberada (no recargar el indice tras Release) se detecta en 12408 casos. `prepare.py` extrae los bytes y el cuerpo; `build.bat <repo>` compila con las opciones de CMake. Si Windows tiene reservado 0xC80000 el arnes puede no arrancar (se reintenta).

## Ejecucion

Con el proyecto compilado con el parche aplicado:

```
python -B reversing/Dependencies21F/build_probe.py
python -B reversing/Dependencies21F/run_local.py
python -B reversing/Dependencies21F/run_local.py --analyze <comparisons.jsonl>
```

o `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies21F`. Tras la corrida: `python contrib/sync_docs.py --update-source-status` (hooks.csv: reversed=1, locked=0, validated=0 hasta registrar la evidencia).
