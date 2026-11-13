<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 898 grupos / 10446 funciones reconstruidas; 10446 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn709CF0: CShadows::ShadowRenderTriangleCB (0x709CF0) contra el EXE, sin hook y sin `__asm` en `source/game_sa`

Caso CASO-CONGELADOS, parte B. Estado: ****APROBADO dentro del juego** (10/11/2026, worktree `wv-6` sobre main 8f02c8dfdb + los cambios del caso)**.

## Resumen

| Dato | Valor |
|---|---|
| Funcion del EXE | `CShadows::ShadowRenderTriangleCB`, 0x709CF0, 837 bytes, `__usercall`: EAX = normal (`CVector*`), EBX = triangulo (`CVector[3]`), EDI = parametros (`_ProjectionParam*`), `ret`, EAX = EBX al volver |
| Version C++ | `ShadowRenderTriangleCB(CVector*, CVector*, _ProjectionParam*)` en `source/game_sa/Shadows.cpp` (cdecl, devuelve el triangulo); sin hook |
| Pares | 16384 EXE/DLL, cero diferencias |
| Evidencia | `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn709cf0/20261110-052051-840016` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn709CF0-20261110-052051-840016`) |
| SHA256 sonda / EXE | `56147454a0d54658b1253698d2ae8fee7791dbe532a57da0aee64fbe9c71c4dd` / `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b` |
| Como repetirlo | `bash reversing/_handoff/bench_here.sh Fn709CF0` |

## Respuesta a la pregunta del caso: si se puede validar sin hook y sin `__asm` en `source/game_sa`

**Si, y ya se habia hecho antes sin que el resumen de Fn136A lo viera.** `reversing/CppOnlyMigration/PROCESO.md` prohibe `__asm` y `naked` solo en `source/game_sa` (y su unica excepcion es `WindowedMode.cpp`, fuera de ese directorio); los bancos de `reversing/` son codigo de prueba y usan ensamblador en linea de forma generalizada para invocar el EXE con registros de entrada: `Core.hpp` de Fn136A siembra EAX/ECX/EDX y empuja los argumentos en un `__asm`, y `reversing/ShadowTriangle355/LiveComparison.hpp` (07/10/2026, historico) ya llamaba a 0x709CF0 con `mov eax,normal / mov ebx,triangle / mov edi,param / call` y comparaba contra el simbolo de la DLL. El comentario de Fn136A ("ningun banco cita 0x709CF0") era un falso negativo del grep: ese banco escribe la direccion como `0709CF0h`. Ese banco antiguo (2048 pares recorridos, 3072 casos, comparacion por hashes) dependia de un hook `naked` que la migracion a C++ retiro (`CppTwelveLevel6Study/README.md`: "Hook retirado"), y su evidencia es historica; este banco la sustituye por una de 16384 pares con oraculo independiente.

El banco, por tanto:
- **Original** = la funcion del EXE en 0x709CF0 por `GateUser` (`Core.hpp`, `__asm` del banco: EAX/EBX/EDI de entrada, ESI sembrado y comprobado, pila, EBP, SEH, EFLAGS, no volatiles). Se comprueba que 0x709CF0 es codigo original (sin salto hacia la DLL).
- **Candidato** = `ShadowRenderTriangleCB` de la DLL, por su direccion de simbolo, por `Gate` (cdecl, tres argumentos de pila).

No hay hook RH ni se necesita: nadie la llama por direccion (su unico llamador del EXE, 0x70A437 dentro de `CastShadowEntityXYZ`, esta reemplazado) y MSVC no permite a C++ leer EBX/EDI de entrada. **Estado que debe llevar en `docs/JuegoExe`**: sigue en el grupo A (sin fila `reversed=1` con esa direccion en `hooks.csv`, que es lo que cuenta `contrib/exe_coverage.py`), pero con la nota "reconstruida en C++ y validada contra el EXE por Fn709CF0 sin hook"; no pasa a "Reemplazada por C++" salvo que el coordinador anada una fila de catalogo con la direccion. No se ejecuta `sync_docs` en este caso.

## Revision contra el EXE (desensamblado de 0x709CF0..0x70A034)

Ver el encabezado de `fixture_oracle.py`: 1) `RwV3dTransformPoints` (0x7EDD90) del triangulo al mundo con la matriz `param + 0x58`; 2) producto `((at.y*n.y) + (at.z*n.z)) + (at.x*n.x)` en la x87 a 24 bits y descarte si es `> 0` (ordenado: con NaN sigue); 3) `RwV3dTransformPoints` del mundo al proyectado con `param + 0x0C`; 4) cinco grupos de "algun vertice cumple" (z no negativa, x no negativa, x no mayor que 1, y no negativa, y no mayor que 1; comparaciones ordenadas, un NaN cumple la condicion); 5) `RenderBuffer::StartStoring(3, 3, &indices, &vertices)` (0x708290); 6) por vertice: x, y, z + 0.06f (`fadd`, guardado como float), u, v y color (alfa = byte bajo de `_ftol2(sombra * (1.0f - z*z))` si hay fade y `1.0f - z*z >= 0`; o el byte de sombra); indices 0, 1, 2; 7) `StopStoring` (0x7077A0).

## Hallazgo: defecto real de produccion corregido (`Shadows.cpp`)

El banco encontro 89 casos sin solapes de buffers (de las 16384) en los que la DLL difiere del EXE en un solo byte: con un **NaN senalizador** en la x del primer vertice del mundo, el EXE lo copia con `fld`/`fstp` (0x709E87-0x709E8F), que lo silencia (bit 22 a uno), mientras que el resto de coordenadas (la x de los vertices 1 y 2, la y, u y v) son copias de 32 bits (`mov`) y conservan los bits. El C++ copiaba todas con `movss`, que no silencia. Es un caso inalcanzable en juego (`RwV3dTransformPoints` no produce NaN senalizadores), pero se corrige con el minimo cambio (comparado con el desensamblado): solo `vertices[0].objVertex.x` pasa por una comprobacion de bits que activa el bit de silencio si es un NaN senalizador. `fixture_oracle.py` lo modela asi (y la x de los vertices 1 y 2 sin silenciar, lo que coincide con el EXE en todos los casos).

## Banco

Cuatro arenas propias (A, B y C con las entradas; D con los buffers de indices y vertices; ventana de 4 KiB con guardas de 0x40) con relleno aleatorio, ceros, unos o 0xCD. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: `RwV3dTransformPoints` (0x7EDD90: la DLL lo llama por la direccion del EXE; escribe las dos tablas de 9 floats del caso, de modo que todas las ramas de la funcion se alcanzan sin depender de la transformada real, que ya validan otros bancos), `StartStoring` (devuelve los buffers de la arena D) y `StopStoring` (solo registra). Se observan: bytes cambiados de las cuatro arenas, eventos de los falsos con todos sus argumentos (punteros por etiqueta de arena; los locales de pila por "en la pila"), valor devuelto (etiqueta), estado x87 (debe quedar vacio), pila, no volatiles, DF, SEH y ruta. EAX/ECX/EDX tras la llamada quedan en `original_residue`/`candidate_residue` (informativos: el unico llamador del EXE los ignora).

16 familias de 1024 casos (`Fixtures.hpp`): colocacion de los tres objetos de entrada (tres arenas, una arena sin solapes, solapes de solo lectura entre normal, triangulo y parametros, y al borde de la ventana con lectura de guardas) x relleno de las arenas. Clases de producto escalar (normal = -at, normal = at, ceros, valores especiales, casi cancelacion, enteros pequenos), diez escenarios de proyeccion (todo dentro, cada grupo fallando, un vertice por grupo, fronteras 0/-0/1/NaN, aleatorios), clases de fade (0, 1, 2, 0x100, 0x80000000, 0xFFFFFFFF, aleatorio), de byte de sombra (con basura en los 24 bits altos) y valores del mundo con especiales (NaN silenciosos y senalizadores, infinitos, desnormales, cerca de FLT_MAX).

Oraculo independiente (`fixture_oracle.py` + `x87model.py`, copia del modelo de 24 bits de Fn135A): reproduce desde la entrada efectiva (los valores leidos de la memoria de las arenas) los eventos, los bytes cambiados de la arena D (incluido el relleno inicial), el valor devuelto y la x87 vacia; `run_local.py` exige que CADA lado de CADA par coincida con el. Sin juego: `python -I fixture_selfcheck.py` compila `fixture_dump.cpp` (mismo `Fixtures.hpp`) y mide la cobertura de ramas.

Fuera de los fixtures: buffers de vertices e indices solapados entre si o con las entradas (el EXE escribe los tres vertices y despues los indices; el C++ intercala), punteros nulos o crudos, y la transformada real.

## Prueba negativa

Dos rondas (`mutate.py`, una mutacion por ronda sobre las fuentes de produccion; sonda recompilada y `FN709CF0_CASES=2048`): las dos fallan (`negative-runs/`, corridas `20261110-045431-588668` y `20261110-050407-568930`; los pares con diferencias son tambien los que no coinciden con el oraculo). Fuentes restauradas con SHA256 identico a las copias buenas (`artifacts/good/`, `mutate.py restore`) antes de recompilar y repetir la corrida final con la sonda reconstruida.
- Ronda 1 (`Shadows.cpp`): coordenada `v` del vertice tomada de `projected.x` en lugar de `projected.y` (797 de 2048 pares con diferencias).
- Ronda 2 (`X87Math.cpp`, `ShadowsTriangleFacesAwayCpp`): `facing >= 0.0` en lugar de `facing > 0.0`, es decir, un producto escalar exactamente cero descarta el triangulo en lugar de seguir (317 de 2048).

## Bancos repetidos

`reversing/ShadowTriangle355` (historico, necesita el hook retirado) no es repetible; queda sustituido por este. Ningun otro banco cita la funcion. Cambios de produccion de este banco: `Shadows.cpp` (la x del primer vertice); ningun otro banco prueba `ShadowRenderTriangleCB`.
