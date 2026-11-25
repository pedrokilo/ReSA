<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ejecución: nodo Rx `nodeD3D9SubmitNoLight.csl` (`0x80E270`)

Rama `nolight-node` (desde `1ec835719c`, 22/11/2026). Fase 8, cuarta ronda: se retira el shim de `0x80E270` del EXE independiente (`StandaloneExe.cpp`, tabla de `InstalarShims`) y se reconstruye el nodeBody en C++. Código: `source/game_sa/RenderWare/rw/rpworld_nolight.cpp` (función `_rxD3D9SubmitNoLightNodeBody`, gancho `RH_ScopedGlobalInstall` en `RpWorldInjectHooksNoLight`, llamado desde `InjectHooksMain.cpp`). El shim de Skin (`0x7C7B90`) sigue y no se toca (otro agente).

## 1. Qué es

Definición `0x8E2978` (`_rwD3D9RxNodeDefinitionGetSubmitNoLight`, `0x80E9A0`, ya en C++). Es el último nodo de las tuberías de render de Im3D de D3D9: lo llama `RxPipelineExecute` por la tabla de `nodeMethods` (por eso no figuraba en ningún CSV: hueco de Ghidra alcanzable solo por puntero). Ni `node` ni `params` se usan: trabaja con el estado de Im3D que `ImmInstance` (`0x80E970`, `_rxD3D9ImmInstanceNodeBody`) dejó en `0xC9AB30` (`rwIm3DPool`, módulo Im3D + `0x38`). Tamaño real: `0x80E270..0x80E943` (1.700 bytes) más dos tablas de saltos de 5 entradas en `0x80E944` y `0x80E958`. Sin x87: todo es entero, así que no hay nada que cuidar de los 24 bits.

## 2. Qué hace (de ensamblador a C++)

Campos del pool (`stash`): `flags` (+0xC del pool), `ltm` (+0x10), `primType` (+0x30), `indices` (+0x34), `numIndices` (+0x38); `numElements` (word, +0) y `elements` (+4, vértices de 0x24 bytes).

1. Estados: `_rwD3D9RenderStateVertexAlphaEnable(ALLOPAQUE ? 0 : 1)` (`0x7FE0A0`), `RwD3D9SetTransformWorld(ltm)` (`0x7FA520`), render states `0x89 = 0`, `0x8F = 0`, `0x88 = NOCLIP ? 0 : 1`. Etapa 0: sin textura de etapa (`0x7FDE50`) COLOROP/ALPHAOP = SELECTARG2 sobre el difuso; con textura MODULATE (TEXTURE x DIFFUSE) en color y alfa.
2. Declaración de vértices `0xC9AB48` (UV, 24 bytes) o `0xC9AB4C` (16 bytes) con la caché `0x8E2444`/`0x8E2440`; quita vertex shader (`0x8E2448`) y pixel shader (`0x8E244C`) si había.
3. Tipo D3D desde la tabla `0x88513C` (RwPrimitiveType -> D3DPRIMITIVETYPE). Sin vértices: devuelve TRUE sin dibujar.
4. Con índices (`stash.indices != NULL` y `numIndices > 0`): número de primitivas por tipo (tabla de saltos `0x80E944`: lista de líneas n/2, tira de líneas n-1, lista de triángulos n/3, tira y abanico n-2).
   * Pequeña (`numIndices <= numVertices` y `<= 8`): bloqueo del búfer dinámico (`RwD3D9DynamicVertexBufferLock`, `0x7F5B10`), copia solo los vértices indexados (posición, color en +0x18 y UV en +0x1C del vértice de Im3D), `SetStreamSource`, vaciado de estados (`0x7FC200`) y `DrawPrimitive`.
   * Grande: copia todos los vértices, vuelca los índices al búfer de índices dinámico `0xC9AB40` (posición `0xC9AB3C`; si `pos + n > 0xFFFF` vuelve a 0 con `DISCARD | NOSYSLOCK` (`0x2800`), si no `NOOVERWRITE | NOSYSLOCK` (`0x1800`)), `SetIndices` con caché `0x8E2450` y `DrawIndexedPrimitive(tipo, baseIndex, 0, numVértices, posición, primitivas)`; después `0xC9AB3C += n`. Si no hay búfer de índices o `n > 0xFFFF`, va al camino de reserva.
5. Sin índices: copia todos los vértices y `DrawPrimitive`.
6. Camino de reserva (cualquier fallo de bloqueo o de dibujo): declaración `0xC9AB44` (36 bytes), flujo 0 vacío, con índices además `SetIndices(NULL)`, y `DrawIndexedPrimitiveUP` (índices de 16 bits, vértices originales, paso `0x24`) o `DrawPrimitiveUP`.
7. Siempre devuelve TRUE.

## 3. Diferencias deliberadas con el EXE

* Lista de puntos (`primType = 6`): la tabla `0x88513C` solo cubre 1..5 y el EXE lee basura; el número de primitivas en el caso `default` era una variable local sin inicializar. Aquí vale el número de elementos. Las tuberías de Im3D no generan ese caso con estos tipos en las pruebas.
* El resto se conserva tal cual, incluidos detalles como que en el camino de reserva sin índices no se deja el búfer de índices en NULL.

## 4. Verificación (humo, sin bancos por función)

* Compilación del ASI en `wv-nolight` (`_build_NOLIGHT.sh`, lock `compile_lock`): `EXIT=0`. `python contrib/audit_hook_catalog.py --strict`: OK (como `_rxD3D9ImmInstanceNodeBody`, el gancho no genera fila nueva en `docs/hooks.csv`: esa tabla la regenera el juego al arrancar y esta rama no la toca).
* Humo con el ASI construido (`reversing/FnBIS1`, `AGUA_ASI` = ASI de `wv-nolight`, argumentos `--unhook=Zzz/Nada`, todos los ganchos activos): `prueba_ropero.py` BIEN (puerta y marcador), `prueba_agua.py` BIEN (brillo 46,9), `prueba_marcadores.py` BIEN (12 marcadores, 12 cambian en 1 s). Ojo: el ASI casi no ejercita este nodo (Im3D sin luz solo lo usan dibujos auxiliares), así que esas pruebas sirven de no regresión.
* Standalone (`gta_sa_standalone`, enlazado con los `.obj` de `wv-nolight`):
  * `nl1` (sin `--shims`): la primera ruptura ya no es `0x80E270` sino `0x7C7B90` (Skin), a los 8,9 s y 129 fotogramas.
  * `nl2` (`--shims`, que ahora solo cubre Skin): 80 s, 1.810 fotogramas, `codigo-original-distinto=0`, 0 excepciones; `Draw*` a 472.378, idéntico en orden de magnitud al registro `h16` con los dos shims.
  * `nl3`: igual con un contador temporal en el nodo (compilado solo a `over\`, retirado después): 20.200 llamadas en 60 s sin fallos (listas de triángulos de 44 vértices y 126 índices con UV, por el camino grande; mallas de 1.452 y 2.046 índices sin UV; listas de líneas) con `codigo-original-distinto=0`. Con el shim esas ~20.000 llamadas no dibujaban nada. Datos: `reversing/FnFase8/ejecuciones/nl3-20261122-095823/contador_nolight.txt`.
  * Capturas: `reversing/FnFase8/ejecuciones/nl{1,2,3}-captura.png` (copias en `D:\Original\sim_out\nolight_*.png`). No hay una captura gemela con y sin nodo de la misma escena (la cámara y la escena cambian entre ejecuciones por otras ramas), así que "se ve más geometría" se apoya en el contador y no en una comparación píxel a píxel.

## 5. Riesgos

* Camino grande y de reserva comprobados solo por ejecución sin errores (sin comparación de imágenes ni bancos): el contador (una muestra cada 200 llamadas) mostró el camino grande indexado; el camino sin índices, el pequeño (`<= 8` índices) y el de reserva no están confirmados en estas ejecuciones (las muestras de listas de líneas y de 44 vértices pasan por el grande o el sin índices según el caso).
* `RwD3D9DynamicVertexBufferLock` y `SetStreamSource` son C++ sin validar; un fallo ahí aparecería como geometría rota en este nodo.
* El cielo blanco del standalone es el defecto de timecyc ya documentado, no de este nodo.

## 6. Cómo reproducir

```
bash /d/Original/_build_NOLIGHT.sh                          # ASI en wv-nolight (log /d/Original/_build_NOLIGHT.log)
# standalone: cl.rsp lleva rutas de wv-fase8c; hay que cambiarlas por las del worktree (sed) antes de enlazar
cmd /c "reversing\FnFase8\enlazar.cmd D:\Original\wv-nolight D:\Original\wv-nolight\reversing\FnFase8\salida D:\Original\wv-nolight\build\source\gta_reversed.dir\Debug"
pwsh -File reversing/FnFase8/correr_f8b.ps1 -Et nl2 -Lim 80 -Cap 70 -Extra "--shims"
```
