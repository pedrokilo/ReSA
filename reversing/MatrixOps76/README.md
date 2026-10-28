<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# MatrixOps76: CMatrix::Attach (0x0059BD10) y CMatrix::operator= (0x0059BBC0)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-1`, HEAD a7266bf64 + `source.patch`). 36864 pares (18432 por funcion: 16 familias de 1024 + 2 familias extra de 1024), cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-matrixops76\20261027-214215-184061`.

`source.patch` (Matrix.cpp, Matrix.h, InjectHooksMain.cpp, Camera.cpp; el mismo de CamSphere76).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| Attach | 0x0059BD10 | CMatrix,Attach | S |
| operator= | 0x0059BBC0 | CMatrix,operator= | S |

## Estado previo y cambios de produccion
- Ambas ya estaban en C++ puro (sin `plugin::Call`, `__asm` ni `naked`), pero **no se instalaban**: el `RH_ScopedInstall` estaba en `CMatrix::InjectHooks`, desactivado en `InjectHooksMain.cpp` por rendimiento (el resto de CMatrix: 40+ funciones, muchas de ellas en rutas calientes, sin banco). Activarlo entero habria enganchado funciones sin validar; se hizo como con `operator*(CMatrix, CVector)`: una funcion nueva `CMatrix::InjectHooksAttachAssign()` instala SOLO Attach y operator=, y las dos filas se quitan del `InjectHooks` completo (para que no se instalen dos veces si alguien lo activa). Coste: un salto por llamada a 0x59BBC0 (89 llamadores en el EXE) y 0x59BD10.
- **Defecto de PRODUCCION corregido** (`Matrix.cpp`, Attach): el EXE LEE la bandera de propiedad como dword (`mov ecx,[esi+44h]; test ecx,ecx`) y la ESCRIBE como dword con el byte del argumento (`movzx ecx,byte ptr [esp+0Ch]; mov [esi+44h],ecx`), de modo que los tres bytes de relleno de `+0x45..0x47` cuentan al decidir si destruir la RwMatrix previa y quedan a cero tras la llamada. La DLL usaba `Detach()` y un `bool`: ignoraba el relleno al decidir y lo dejaba intacto. Ahora lee/escribe el dword (`std::bit_cast<uint8>` del argumento, sin normalizar a 0/1) y llama directamente a `RwMatrixDestroy`. La prueba negativa de Attach es justo el codigo anterior: **8814/18432 diferencias**.
- operator=: sin defectos. El EXE devuelve `this` en EAX y la DLL es `void`; ninguno de los 89 llamadores del EXE (`call 0059BBC0`) lee EAX tras la llamada (la instruccion siguiente lo carga o no lo usa), por eso la firma `void` se mantiene (cambiarla rompe los `static_cast<void (CMatrix::*)(const CMatrix&)>` de bancos antiguos). EAX de salida se registra, no se compara.

## Revision contra el EXE
- Attach (0x59BD10..0x59BD80): si `[this+0x40] != 0` y el DWORD `[this+0x44] != 0` -> `RwMatrixDestroy` (0x7F2A20, cdecl, 1 argumento); `[this+0x40] = matriz`; `[this+0x44] = movzx(byte del argumento)`; copia los dwords +0x00, +0x04, +0x08, +0x10, +0x14, +0x18, +0x20, +0x24, +0x28, +0x30, +0x34, +0x38 de la RwMatrix (no copia banderas +0x0C ni relleno +0x1C/+0x2C/+0x3C). Sin aritmetica: movimientos de enteros, asi que los NaN (de señal incluidos) deben copiarse bit a bit.
- operator= (0x59BBC0..0x59BBE6): lee `[this+0x40]` ANTES de la copia, `rep movsd` de 16 dwords (0x40 bytes: matriz completa con banderas y relleno, sin `+0x40/+0x44`) y, si habia RwMatrix, llama a 0x59AD70 (UpdateRwMatrix: los mismos 12 dwords de `this` a la RwMatrix y `RwMatrixUpdate` 0x7F18A0, cdecl).

## Banco
Fixtures: 18432 casos por funcion, 18 familias. Attach: sin matriz previa, previa propia/ajena/suelta/igual a la nueva/la propia CMatrix, dword de propiedad con todos los patrones (byte bajo cero con basura arriba, basura sola, 0..255), argumento bool con todos los valores del byte y basura en los otros tres, RwMatrix nueva con especiales (NaN de señal y silenciosos con cargas utiles, infinitos, desnormales, ±0) en los 12 dwords copiados y en los 4 no copiados, alias de la RwMatrix con la propia CMatrix, contenidos neutros. operator=: con y sin RwMatrix asociada, origen con especiales, autoasignacion con/sin RwMatrix, RwMatrix asociada = el propio origen / la propia `this` / otra, `+0x40/+0x44` del origen especiales (no se copian), destino con contenido previo especial.
Arenas propias (guarda de 0x40 bytes a cada lado de cada objeto, comprobada): CMatrix `this` y `other` (0x48), RwMatrix nueva y antigua (0x40). Se comparan eventos de los falsos EN ORDEN con sus argumentos (y, en RwMatrixUpdate, los 16 dwords de la RwMatrix en el momento de la llamada), bytes cambiados de las cuatro arenas, el puntero `+0x40` (como etiqueta) y el dword `+0x44`, compuertas de ABI (pila, no volatiles, DF, SEH), ruta (entrada del EXE; salto a la DLL en el lado candidato), estado x87/MXCSR y restauracion de entradas. Los bytes crudos de `+0x40..0x43` son direcciones de ejecucion y se comparan por etiqueta.
Colaboradores sustituidos por falsos identicos en la direccion del EXE (son codigo de RenderWare sin hook: la DLL los llama por su direccion):
- RwMatrixDestroy 0x7F2A20 (Attach): sin banco propio (RenderWare, fuera del catalogo).
- RwMatrixUpdate 0x7F18A0 (operator=): sin banco propio (RenderWare, fuera del catalogo).
Corren de verdad en ambos lados (sin hook, entradas vigiladas 0x59AD70, 0x59AD20, 0x59BB60, 0x59BBB0, 0x59ACF0): la copia de 12 dwords de la RwMatrix (inline en el EXE; `CMatrix::Update/UpdateMatrix` de la DLL), `CopyOnlyMatrix` y `UpdateRW/UpdateRwMatrix` (en el EXE 0x59AD70): su correccion queda comprobada por estas pruebas solo en el contexto de Attach y operator= (el banco no las valida como funciones propias).
Cobertura de ramas medida y calibrada al 50 % en `coverage.json`.
Aritmetica: ninguna (solo movimientos de dwords), por lo que no hay microtest `__asm` de reglas de NaN; la copia bit a bit de NaN de señal, desnormales e infinitos queda cubierta por los pares (2741 pares de Attach y 5858 de operator= con RwMatrix actualizada copian un sNaN).

Pruebas negativas (produccion mutada y restaurada despues; `git diff` de los cuatro archivos igual al de `source.patch`): Attach con el codigo anterior (`Detach()` + `bool`) y operator= sin `UpdateRW()`: **Attach 8814/18432 y operator= 13469/18432 diferencias de resultado**, sin caida: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-matrixops76\20261027-213544-403768`.

Hallazgos del BANCO (no de produccion): una `static_assert(offsetof(CMatrix, m_right))` no compila (miembros privados); se quito. Un primer intento del banco de la camara fallo por el offset de la arena en el propio banco (ver CamSphere76).

## Bancos antiguos tocados (la DLL ahora ejecuta el cuerpo de Attach y operator= como colaboradores CON hook)
HeliPre75, HeliFly75 (Attach), CarPolice75 y CarConvoy75 (operator=; CarPolice75 tambien IsSphereVisible): las tablas `STUBBED` de `run_local.py` y `Pair{...,true}` de `LiveComparison.hpp` pasan a hooked=true y se repitieron; ver sus README.

No validado: RwMatrix o CMatrix de origen nulos (el EXE se cae), solapamiento parcial (no total) de origen y destino en operator= (la copia `rep movsd` y `memcpy` difieren), EAX/ECX/EDX de salida, `RwMatrixUpdate`/`RwMatrixDestroy` reales, y la correccion de CopyOnlyMatrix/Update/UpdateRW/UpdateRwMatrix como funciones propias.

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh MatrixOps76`; `python -B run_local.py --analyze <carpeta>/comparisons.jsonl`; `python -B fixture_oracle.py <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>` (solo el coordinador).
