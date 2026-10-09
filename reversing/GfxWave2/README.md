# GfxWave2: banco por pares de CClothesBuilder, CShadows y CWaterLevel

Banco simple (flujo `Small2`/`PadMouse1`/`GeoFinal1`, dentro del juego completo, enganchado en `CGame::Process`). Sin compilar ni ejecutar todavia.
Uso: `bash reversing/_handoff/bench.sh GfxWave2 3`. Filtro opcional: `gfxwave2-only.txt` en la carpeta del juego con `Clase::funcion`.
`build_probe.py` y `run_local.py` redirigen `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test`.

## Estructura
`LiveComparison.hpp` (tabla + bucle principal) incluye `GfxCommon.hpp` (utilidades) y tres partes (`ClothesPart.hpp`, `ShadowsPart.hpp`, `WaterPart.hpp`), cada una con sus `fns`, `Run`, `Stubs()` y `Globals()`. Los parches (EntryPatch en la direccion del EXE y en el simbolo de la DLL, con todos los `Body()` calculados antes) de una parte solo estan activos mientras corren las funciones de esa parte.

## Corpus (original = hook desactivado, candidato = hook activo)
55 funciones, 27616 pares: Clothes 14 funciones / 4096, Shadows 19 / 6304, Water 22 / 17216. La tabla exacta esta al principio de `LiveComparison.hpp`.

## Fuera del banco (regla 17)
Ver `notes_Cl.md`, `notes_Sh.md`, `notes_Wl.md`: CreateSkinnedClump (streaming real de PLAYER.IMG + objetos RW reales), RenderStoredShadows y CastPlayerShadowSectorList (sectores y nodos del pool reales), WaterLevelInitialise (fichero + sscanf), RenderBoatWakes (pasada inlineada sobre 0xC27994), SplitWaterTriangleAlongYLine (la fuente aun no iguala las 4 ramas del EXE).

## Ruido controlado
Hilos ajenos ignorados (`Mine()`), dibujo sustituido por trazas, x87 a 24 bits, solo se comparan campos reales (relleno enmascarado), sin NaN de senalizacion.

## Revision tras la ejecucion 20261009-033856
- Banco: GraphicsHighQuality se parchea sobre el simbolo libre ::GraphicsHighQuality (Shadows.cpp llama a ese, no al miembro de CStencilShadows); ScanThroughBlocks normaliza el callback (BlockHit exe/DLL); AddWaterLevelQuad kind 9 usa dos niveles de Y (el EXE lee pila sin inicializar con lista A de 1 o 3 elementos).
- Fuente: RenderIndicatorShadow compara frontX con -sideY (0x70CCB3 fld [esp+2Ch]); RenderExtraPlayerShadows llama FindPlayerCoors/FindPlayerVehicle/GetColModel x5 por luz como el EXE; SetUpWaterFog llama FindPlayerCoors antes de GetWaterLevelNoWaves; RenderWaterRectangle pasa (minX,minY,maxX,maxY) a SetUpWaterFog; lerp(CRenPar) = (1-t)*a + t*b como 0x6E5280 (std::lerp daba otros redondeos); SplitWaterTriangleAlongXLine rama X2==X3 pasa P12,P2,P13,P23 (por confirmar).
