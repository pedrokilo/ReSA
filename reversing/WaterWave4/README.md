<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WaterWave4: banco por pares de las 4 ultimas funciones pendientes de CWaterLevel

Banco simple (flujo `Small2`/`PadMouse1`/`GfxWave2`, dentro del juego completo, enganchado en `CGame::Process`). **Sin compilar ni ejecutar** (encargo: no compilar, no ejecutar GTA, no commit).
Uso: `bash reversing/_handoff/bench.sh WaterWave4 3`. Filtro opcional: `waterwave4-only.txt` en la carpeta del juego con `CWaterLevel::funcion`.
`build_probe.py` y `run_local.py` redirigen `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test` en memoria (sin tocar `runtime_runner.py`).
Con estas 4 (AddWaterLevelQuad ya estaba en `GfxWave2`, aqui se repite con el fixture corregido) quedan cubiertas las 30 funciones de `CWaterLevel` de `docs/hooks.csv`: 21 de `GfxWave2` + AddWaterLevelQuad + RenderBoatWakes + SplitWaterTriangleAlongYLine + WaterLevelInitialise + las 5 bloqueadas `S` (AddWaveToResult, FindNearestWaterAndItsFlow, GetWaterLevelNoWaves, RenderWater, RenderWaterFog), que no son de este encargo.

## Corpus: 2816 pares

| Funcion | EXE | Pares | Que se ejecuta |
|---|---|---|---|
| `CWaterLevel::AddWaterLevelQuad` | 0x6E7EF0 | 1024 | EXE vs fuente sobre `m_aVertices`/`WaterQuads` reales (AddWaterLevelVertex real, hook apagado) |
| `CWaterLevel::RenderBoatWakes` | 0x6ED9A0 | 512 | 0-4 barcos falsos con 0-32 puntos de estela; estelas trazadas, buffer temporal comparado |
| `CWaterLevel::SplitWaterTriangleAlongYLine` | 0x6EE5A0 | 1024 | las 4 ramas (~256 cada una); RenderWaterTriangle/Rectangle trazados |
| `CWaterLevel::WaterLevelInitialise` | 0x6EAE80 | 256 | `water.dat` en memoria (4-40 lineas), sscanf real, Add* trazados |

Original = hook desactivado (corre el codigo del EXE); candidato = hook activo (corre la fuente). Los dos legs reconstruyen el mismo fixture desde la misma semilla y se comparan: valor/estado (hashes), control x87 (24 bits), `switches` (`JumpsToDll` tras `State()`) y la traza completa de colaboradores con argumentos crudos. Los registros que pasan llevan `original` y `candidate`.
Los parches (`EntryPatch` en la direccion del EXE, en el simbolo de la DLL y en su cuerpo; todos los `Body()` se calculan ANTES de crear ningun parche) viven solo durante cada ejecucion y solo actuan en el hilo del banco (`Mine()`).
Nada dibuja, nada abre ficheros reales.

## Correcciones de fuente (`source/game_sa/WaterLevel.cpp`, evidencia asm)

### SplitWaterTriangleAlongYLine 0x6EE5A0: FIXED (reescrita rama por rama)
La fuente anterior (`SplitWaterTriangleAlongXLine` con una X calculada) no era la logica del EXE. Transcripcion simbolica de las 4 ramas (simulador de pila sobre el desensamblado; Ghidra `CWaterLevel.SplitWaterTriangleAlongYLine_6EE5A0.c` coincide):
- Comun (0x6EE5B3..0x6EE5D3): `dS = sY-Y1` (`sub ecx,esi`), `dH = Y3-Y1` (`sub ebx,edi`), `t = fild dS / fidiv dH` guardado como float (`fstp [esp+4Ch]`).
- 0x6EE5C1 `cmp esi,edi` / 0x6EE5D7 `jge 0x6EED72`: `Y1 < Y3` -> A/B; si no C/D. 0x6EE5DD y 0x6EED72 `cmp ebp,eax / jne`: `X1 == X3` -> A/C (0x6EE5E5, 0x6EED7A); si no B/D (0x6EEA48, 0x6EF1D7).
- XS con `imul`+`cdq`+`idiv` (truncacion): A/C `XS=(X1-X2)*dS/dH+X2` (0x6EE5EB..0x6EE603), B/D `XS=(X2-X1)*dS/dH+X1`.
- A: `Tri(XS,Y1,L21; X2,Y2,P2; XS,sY,L23)` (0x6EE73D), `Rect(X1,XS,Y1,sY; P1,L21,L13,L23)` (0x6EE91F), `Tri(X1,sY,L13; XS,sY,L23; X3,Y3,P3)` (0x6EF63C).
- B: `Tri(X1,Y1,P1; XS,Y1,L12; XS,sY,L13)` (0x6EEB94), `Rect(XS,X2,Y1,sY; L12,P2,L13,L23)` (0x6EF502), `Tri(XS,sY,L13; X2,sY,L23; X3,Y3,P3)`.
- C: como A con `Rect(X1,XS,sY,Y1; L13,L23,P1,L21)` (0x6EF0AE).
- D: como B con `Rect(XS,X2,sY,Y1; L13,L13,L12,P2)` (0x6EF502, bug del EXE: las dos primeras esquinas reciben `lerp(P1,P3,t)`; 0x6EF3E1 y 0x6EF445 llaman a `0x6E5280` con los mismos argumentos). Se conserva.
(`Lab` = `lerp(Pa,Pb,t)` = `(1-t)*a + t*b` de 0x6E5280, que deja flow a 0.) `DontRenderYSplitTri` (NOTSA, depuracion, `false` por defecto) se mantiene; el EXE no lo tiene.

### RenderBoatWakes 0x6ED9A0: FIXED (antes delegaba en `CBoat::RenderAllWakePointBoats`)
Transcripcion exacta de la pasada inlineada (x87 a 24 bits == float; constantes leidas del EXE; sin FMA):
- 0x6ED9AE..0x6ED9C1: `RwRenderStateSet(rwRENDERSTATETEXTURERASTER=1, [0xC228B8])` por `call [ecx+20h]` (la version de `CBoat` usaba `RwTextureGetRaster(texWaterwake)`), luego `CBoat::FillBoatList` (0x6F2710).
- Lista 0xC27994, `movsx bx`, maximo 4 (`cmp bx,4`), se para en el primer nulo (0x6ED9EA).
- 0x6ED9F2..0x6EDA2B: direccion adelante = `[m+0x10],[m+0x14]` (sin comprobar `m`); ancho base = `colmodel.max.x * 0.65f` (0x858F50), `*0.4f` (0x858EE8) si modelo 0x1CC.
- 0x6EDA3A..0x6EDA74: ancho del punto 0 `((150-c0)*((s0*0.04f+0.5f)/150))*W+W`; alfa del punto 0 `s0*0.01f*0.0f` (0x6EDA78..0x6EDA88).
- 0x6EDA93..0x6EDAE1: distancia 2D `sqrt(dy*dy+dx*dx)` barco(matriz+0x30 o entidad+4)-camara(matriz+0x30 o 0xB6F02C); `50 < d` -> alfa0 `*= (80-d)*0.033333335f`.
- Bucle 0x6EDB10..0x6EDD2B (`cmp ecx,eax / jl`, i con signo de 16 bits): `d=prev-cur`; `sumsq > 9` -> normaliza con `1/sqrt` y `len > 13` anula el dibujo (`xor bl,bl`); ancho propio; esquinas A,B con la direccion y el ancho anteriores y C,D con los actuales; `alfa=(1-i/N)*160`, `i<3` -> `*i*0.33333334f` (la fuente de `CBoat` usaba `(i+1)/3` con FIX_BUGS, `i/3` sin el), `*(s*0.01f+0.15f)`, `50<d` -> `*(80-d)*0.033333335f`; llamada a 0x6EA260 con punteros `(A,B,C,D,&-0.03f,&ancho,&alfaPrev,&alfa,&-0.03f)` (0xBCF5C28F).
- 0x6EDD46..0x6EDD96: vaciado inline `LittleTest`, `RwIm3DTransform(.., n, 0, 1)`, `RwIm3DRenderIndexedPrimitive(3, ..)`, `RwIm3DEnd`; contadores a 0.
`CBoat::RenderAllWakePointBoats`/`RenderWakePoints` (Boat.cpp) ya no las usa nadie desde `CWaterLevel` (no se tocan: fuera del alcance).

### WaterLevelInitialise 0x6EAE80: FIXED (LoadDataFile)
- 0x6EAF47 `push 859C6Ch ("rb")`: el modo es `"rb"` y la ruta `"DATA\\water.dat"` (0x872148) / `"DATA\\water1.dat"` (0x872158) (la fuente abria `"r"` y `"DATA//water.dat"`).
- 0x6EB069 `mov [esp+0ACh],1` (antes del primer sscanf de cada linea): `flags` vale **1** por omision (la fuente usaba 0 -> poligonos invisibles). (La nota de GfxWave2 "flags sin inicializar" era erronea: se inicializa en 0x6EB069.)
- Estructura: 0x6EAF80..0x6EAF9C se ignoran lineas que empiezan por `;`, `\0`, `*` o `p`; sscanf 29 (0x8720F0, 0x6EB074) == 0x1D o sscanf 28 (0x872098, 0x6EB144) == 0x1C -> cuadrilatero (0x6EB39F..0x6EB5CE); si no, sscanf 22 (0x872050) **sin comprobar el resultado** -> triangulo (0x6EB1D8..0x6EB392). La fuente usaba `std::stringstream` y deducia el tipo por el recuento de vertices.
- Conversion: X, Y por `_ftol2` (0x821B40), flow = byte bajo de `_ftol2(f * 64.0f)` (0x859A44).
- **Bug del EXE en cuadrilateros**: el `flowY` del primer vertice es el del segundo: 0x6EB47A `call _ftol2` (operando `[esp+4Ch]` = v2.flowY) / 0x6EB491 `mov bl,al` / 0x6EB497 `mov [esp+0C1h],bl` y 0x6EB4F1 `mov [esp+0F5h],bl` (el byte alto del `CRenPar` del vertice 1 recibe BL; nunca se carga v1.flowY). En triangulos cada vertice usa el suyo. Reproducido.
- El sscanf es el del CRT del EXE (0x8220AD), llamado por direccion (mismo redondeo de `%f` que el juego).
- MATCH sin cambios: puesta a cero inicial (`rep stosd` 0x48 dwords en 0xC21B70 y los 4 contadores), `CloseFile` 0x5389D0, `FillQuadsAndTrianglesList` 0x6E7B30, y todo `LoadTextures` (0x7316A0 Push, 0x731850 FindTxdSlot("particle" 0x85C648), 0x7319C0 SetCurrentTxd, `RwTextureRead` 0x7F3AC0 con mascara 0 para waterclear256/seabd32/waterwake (0x872040/0x872038/0x87202C) solo si el puntero es nulo, rasters en 0xC228A8/B0/B8, y cola `jmp 0x7316B0` Pop).

### AddWaterLevelQuad 0x6E7EF0: MATCH (sin cambios de fuente)
Reverificado instruccion a instruccion: salida temprana X1==X2==X3==X4 (0x6E7EFB..0x6E7F10) o Y1==Y2==Y3==Y4 (0x6E7F1E..0x6E7F33); 4 llamadas a 0x6E5A40 en orden 1..4; Y minima con `cmp ax,bx / jl` de 16 bits con signo (0x6E8030..0x6E8097); lista A (Y minima) en `[esp+1Ch+4k]` y B en `[esp+24h+4k]` (se solapan, como `lst[0..3]` de la fuente); `cmp dx,word / jle` por X con indices `movsx` (0x6E8120/0x6E814A); escritura de 4 words y de 3 bits de banderas (`and byte,0FEh`; test dl,1 -> bit1 = !(flags&1); test dl,2 -> bit2).
**Clasificacion de los 2 fallos restantes de GfxWave2 (1022/1024): ruido del banco, no de la fuente.** Reproducidos reimplementando el generador (`Seed`/`Rnd`/`RgI`/`RandPar` + el orden de evaluacion de `RgI(..)*(Rnd&1?1:-1)` de MSVC) en un script: los casos **548** (y = -3039,-1500(dedup),-3388,-3388) y **997** (y = -3263,1050(dedup),-3091,-3263). En ambos el fixture contaba la Y minima con la Y de ENTRADA (2 vertices -> aceptado), pero `AddWaterLevelVertex` (0x6E5A40, `fcomp 0x859A94`, `test ah,1`) fija toda Y <= -3000 en -3000: las Y guardadas son -3000 x3 (+1), la lista A queda con 3 elementos y B con 1, y el EXE lee `[esp+28h]` (B[1]) **sin inicializar** (la DLL tiene `lst[8]{}` = 0). Con la Y guardada el recuento es 3 (no 2 ni 4). Este banco cuenta con la Y guardada (`StoredY`) y solo deja 2 o 4; si no, deshace la deduplicacion o fuerza 2+2.

## Dobles por funcion
- **AddWaterLevelQuad**: sin dobles; `AddWaterLevelVertex` real con su hook apagado (`HooksOff`) en ambos legs. Se compara `NumWaterVertices/Quads`, hash de `WaterQuads` (301 x 10 bytes, relleno 0xAA) y de los vertices (campos reales + resto en bruto). Tipos de quad: rectangulo (con deduplicacion contra el vertice 0, coordenadas fuera de +-3000), X iguales, Y iguales, dos niveles de Y arbitrarios; flags 0-7.
- **SplitWaterTriangleAlongYLine**: dobles `RenderWaterTriangle` 0x6EE240 (tag 3013) y `RenderWaterRectangle` 0x6EC5D0 (tag 3005) con X, Y crudos y hash de los `CRenPar` (z, big, small, flowX, flowY). Triangulos con Y3 por encima/debajo, vertice 3 bajo X1 o X2, Y2 distinto de Y1 (25%), `sY` en [Y1,Y3], extremos, y fuera de rango (t<0, t>1).
- **RenderBoatWakes**: barcos falsos de 0x800 bytes (matriz en +0x14, modelo +0x22, N +0x644, coordenadas +0x648, contadores +0x748, velocidades +0x7C8), matrices falsas (adelante +0x10, pos +0x30), ColModel falso (max.x en +0xC), matriz de camara falsa o posicion 0xB6F02C. Doblados: `CBoat::FillBoatList` 0x6F2710 (escribe la lista 0xC27994: 0-4 barcos, a veces con hueco), `CEntity::GetColModel` 0x535300, `RenderWakeSegment` 0x6EA260 (tags 3042/3043: 4 esquinas y 5 floats), `RwRenderStateSet` (puntero `[0xC97B24]+0x20`, tag 3045), `LittleTest` 0x541330 y `RwIm3DTransform/RenderIndexedPrimitive/End` 0x7EF450/550/520 (devuelve nulo cada 5 casos). Distancias exactas 50 (30,40), 80, 79, 49.9...; pasos de 0, 1, 2.9, 3 (sumsq=9), 3.1, 12.9, 13, 13.1...; N de 0 a 32; velocidades 0/255; contadores 0/150; Skimmer 0x1CC; un NaN en la camara (comparaciones con NaN) y un NaN en el colmodel (~1/40). Se compara el estado del buffer temporal (contadores, hash de vertices e indices) y el hash de cada barco.
- **WaterLevelInitialise**: `CFileMgr::OpenFile` 0x538900, `CFileLoader::LoadLine(FILESTREAM)` 0x536F80 y `CFileMgr::CloseFile` 0x5389D0 se doblan con un fichero en memoria (lineas saneadas: sin espacios iniciales, comas ni controles; `FILE*` falso en la imagen de la DLL, < 2 GB porque el EXE hace `jl` sobre el valor); otros hilos usan `fopen/fclose`. Se ejecutan de verdad `sscanf` (0x8220AD) y todo el codigo propio. Doblados con traza: `AddWaterLevelQuad` 0x6E7EF0 (tags 4020/4021: X, Y, hash de cada `CRenPar` con su flujo y `flags`), `AddWaterLevelTriangle` 0x6E7D40 (4022/4023), `FillQuadsAndTrianglesList` 0x6E7B30, `CTxdStore::PushCurrentTxd/PopCurrentTxd/SetCurrentTxd/FindTxdSlot(const char*)` 0x7316A0/0x7316B0/0x7319C0/0x731850 y `RwTextureRead` 0x7F3AC0. Fixture: configuracion 0 o 1, texturas ya cargadas o nulas (se traza `RwTextureRead`), 2-40 lineas mezclando comentarios (`;`, `*`, `p...`, vacias), cuadrilateros con y sin flags (0..7, 255, 256), triangulos con y sin flags, separadores espacio/tabulador/varios, x e y con fraccion (truncacion hacia cero de negativos) y flujos en [-2.5, 2.5] (el byte `*64` se envuelve). Se compara el nombre/modo recibidos (hash), contadores, `m_BlockPolyInfo` (poblado con 0xAB: debe quedar a cero), configuracion, texturas y rasters.

## Dominio del fixture (lecciones 12 y 21: el EXE lee pila sin inicializar fuera de el)
- AddWaterLevelQuad: Y minima guardada en exactamente 2 o 4 vertices (con 1 o 3 el EXE lee `[esp+20h]`/`[esp+28h]` sin inicializar; la fuente da 0).
- RenderBoatWakes: la matriz del barco siempre existe (el EXE la desreferencia en 0x6ED9F5 sin comprobar); N <= 32.
- WaterLevelInitialise: configuracion 0 o 1 (otro valor deja la ruta sin inicializar); solo lineas bien formadas (el EXE, si el sscanf de 22 falla, usa los locales de la linea anterior).
- SplitWaterTriangleAlongYLine: `Y3 != Y1` (idiv y fidiv por cero); coordenadas pequenas para que `(X1-X2)*dS` no desborde.

## Fuera del banco
Nada de las cuatro queda fuera. Lo unico no ejecutado de verdad es la E/S de ficheros (sustituida por memoria con el contenido de `water.dat` generado) y el dibujo; `water.dat` real no se lee. No se puede comprobar con este banco la rama de configuracion distinta de 0/1 (pila sin inicializar) ni los `RwTextureRead` que devuelven nulo (el EXE y la DLL se caen igual).

## Supuestos a revisar al compilar / ejecutar
1. `CBoat::apFrameWakeGeneratingBoats`, `m_WakeCoords` etc. son publicos y `CEntity::GetColModel` es no virtual (`DA(&CEntity::GetColModel)` toma su direccion); `CBoat::FillBoatList` es publico. El evento `layout` comprueba los desplazamientos (0x14, 4, 0x22, 0x644, 0x648, 0x748, 0x7C8, matriz +0x10/+0x30, bbox +0xC, 0xC27994, 0xB6F03C/0xB6F02C, 0xC228B8) y que el `FILE*` falso este por debajo de 2 GB.
2. `static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)` instancia la plantilla `LoadLine(auto)`; si el enlazador no la encuentra, usar la direccion del cuerpo exportado.
3. `RwRenderStateSet` de la DLL va por `[RwEngineInstance + 0x20]` (como se comprobo en GfxWave2/ShadowsPart). Si no, el trazado 3045 faltara en el candidato.
4. El orden de evaluacion de `RgI(..) * (Rnd(s) & 1 ? 1 : -1)` no esta definido en C++; los fixtures dependen de el pero ambos legs construyen exactamente el mismo estado (misma semilla, mismo binario).
5. `sscanf` del EXE (0x8220AD) bajo x87 a 24 bits: los dos legs lo ejecutan igual; el redondeo de `%f` es el del juego (que tambien corre con 24 bits).
6. `RenderBoatWakes` (fuente) usa `std::sqrt(float)` (sqrtss) y suma/producto en float: equivalente a x87 a 24 bits salvo NaN con dos operandos NaN distintos (el fixture solo tiene un origen de NaN por caso).
7. Esta fuente cambia tres comportamientos reales: flags por omision 1 en `water.dat`, bug de `flowY` del vertice 1 de los cuadrilateros y la rama D de `SplitWaterTriangleAlongYLine`; cualquier regresion visual esperada en el juego es la del EXE original.

## Aprobación (09/10/2026)

Corrida `20261009-123543-732613`: passed=true, 2816 pares, 4 funciones con 0 fallos (AddWaterLevelQuad 1024, RenderBoatWakes 512, SplitWaterTriangleAlongYLine 1024, WaterLevelInitialise 256), evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_waterwave4.py` (evidencia en `runs/`, `validation-summary.json`). Con esto CWaterLevel queda COMPLETA. `CWaterLevel::AddWaterLevelQuad` figuraba RECHAZADA en GfxWave2 (el fixture contaba la Y mínima con la Y de entrada en vez de la clamped a -3000); ahora se registra desde este banco. Límites: water.dat en memoria con sscanf real; barcos/ColModel/matrices falsos; dominio excluido por pila sin inicializar (Y mínima 1 o 3, config distinta de 0/1, líneas mal formadas, barco sin matriz).
