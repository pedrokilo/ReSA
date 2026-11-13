<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10369 funciones reconstruidas; 10369 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn91A: cuatro funciones de `CWaterLevel` (lote 91A)

Estado: **APROBADO dentro del juego** (04/11/2026, worktree `wm-1` sobre main 6e80ef26d + los cambios del lote). 65536 pares (16384 por funcion), cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/restauracion en ambos lados, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).
Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn91a/20261104-022913-834139`. Cuatro funciones de codigo de juego reconstruidas en C++ puro, enganchadas (tipo S, `RH_ScopedGlobalInstall`) y sin llamadas por direccion desde `WaterLevel.cpp`.

## Funciones (todas cdecl, `ret` del llamador)
- `0x6E5750` (182 B) `CWaterLevel::AddToQuadsAndTrianglesList(blockX, blockY, index, type)`: anade un poligono al bloque 12 x 12 (`m_BlockPolyInfo`) creando o ampliando su combinacion en `m_PolyCombos`. (funciones.csv la atribuye a `WaterCreatureManager_c` por vecindad; es de `CWaterLevel`.)
- `0x6EA8A0` (125 B) `CWaterLevel::GetGroundLevel(pos, outZ, outColData, range)`: `CWorld::ProcessVerticalLine` (0x5674E0) desde (x, y, z + range) hasta -range (edificios y dummies); devuelve la z y, si se pide, superficie y pieza del lado B del `CColPoint`.
- `0x6E91D0` (1903 B) `RenderHighDetailWaterRectangle_OneLayer(...)` y `0x6E8780` (1856 B) `RenderHighDetailWaterTriangle_OneLayer(...)`: una capa de la rejilla de alto detalle (olas, atenuacion por distancia a la camara, colores por vertice, indices). Las capas 0 y 1 son las del juego; la capa 2 del triangulo (el EXE multiplica por una ranura de pila sin inicializar, nadie la llama) es una transcripcion con 0.0f y queda fuera del banco.
La aritmetica x87 de 24 bits esta en `X87Math.cpp` (`Wl91*`); el resto (enteros con envoltura, idiv, indices, bucles) en `WaterLevel.cpp`.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los tres colaboradores tienen hook RH, apagado durante el banco): `FlushWaterRenderBuffer` 0x6E7680 (no renderiza; deja los contadores en los valores del caso), `CalculateWavesOnlyForCoordinate` 0x6E6EF0 (escribe altura, multiplicador de color, brillo y normal segun el caso; en algunos casos modifica los contadores tras la llamada) y `CWorld::ProcessVerticalLine` 0x5674E0. Registran sus argumentos en orden. Corre de verdad en ambos lados toda la aritmetica de las funciones.
Memoria real del juego restaurada tras cada lado: bufer temporal de agua, contador y tablas de color, combinaciones y bloques, colores y desplazamientos, normal de las olas y byte de depuracion, distancia de detalle, alfas, escala de la capa 2 y la posicion / matriz de la camara. Se observa: AL devuelto, eventos de los falsos (CRC y primeros eventos), CRC de los vertices (36 bytes), indices y tablas de color de las zonas posibles, contadores, bytes cambiados y que el resto de las regiones no cambia.
16 familias de 1024 casos por funcion (layers, tamanos de rejilla, signos negativos, coordenadas enormes, flotantes especiales, desplazamientos de textura enormes, distancias alrededor de 0.75 y 1.0 de la distancia de detalle, alfas de 32 bits, mutaciones de contadores, ABI con registros sembrados).

## Fuera de los fixtures
`halfX` o `halfY` iguales a cero (#DE), capa 2 del triangulo, NaN con cargas distintas de 0xFFC00000 y NaN senalizantes, rejillas de mas de 441 vertices, bloques fuera de 12 x 12 y contadores fuera de los buferes.

## Prueba negativa
Produccion mutada en seis puntos (escala de textura 0.08f -> 0.07f en el rectangulo, `count + 3` -> `count + 2`, pieza = superficie en `GetGroundLevel`, indice `tail - 3` -> `tail - 4` en el triangulo, alfa de la capa 1 -> el de la 0, umbral 0.75f -> 0.7f de la atenuacion): el banco FALLA (`resultados-fn91a/20261104-034947-336648`) con diferencias en las cuatro funciones: AddToQuadsAndTrianglesList 7734/16384, GetGroundLevel 6892/16384, RenderHighDetailWaterRectangle_OneLayer 14235/16384 y RenderHighDetailWaterTriangle_OneLayer 12563/16384. Fuentes restauradas (sha256 comprobado) y banco repetido (aprobado, ejecucion final).

## Bancos repetidos
Ver el informe del lote.
