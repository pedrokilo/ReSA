<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# GfxWave2: banco por pares de CClothesBuilder, CShadows y CWaterLevel

Banco simple (flujo `Small2`/`PadMouse1`/`GeoFinal1`, dentro del juego completo, enganchado en `CGame::Process`). Estado de ejecucion: ver la seccion de aprobacion parcial al final (la frase 'sin compilar ni ejecutar' era del estado previo a la corrida).
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
- Banco: GraphicsHighQuality se parchea sobre el simbolo libre ::GraphicsHighQuality (Shadows.cpp llama a ese, no al miembro de CStencilShadows); ScanThroughBlocks normaliza el callback (BlockHit exe/DLL); AddWaterLevelQuad kind 9 usa dos niveles de Y (el EXE lee pila sin inicializar con lista A de 1 o 3 elementos; ademas la sustitucion del vertice 1 por el vertice 0 (dedup) solo se aplica si la Y minima sigue en exactamente 2 vertices: antes rompia el 2+2 en ~25% de los casos y causaba las 254/1024 diferencias, ruido del banco y no del fuente WaterLevel.cpp, que coincide con 0x6E7EF0).
- Fuente: RenderIndicatorShadow compara frontX con -sideY (0x70CCB3 fld [esp+2Ch]); RenderExtraPlayerShadows llama FindPlayerCoors/FindPlayerVehicle/GetColModel x5 por luz como el EXE; SetUpWaterFog llama FindPlayerCoors antes de GetWaterLevelNoWaves; RenderWaterRectangle pasa (minX,minY,maxX,maxY) a SetUpWaterFog; lerp(CRenPar) = (1-t)*a + t*b como 0x6E5280 (std::lerp daba otros redondeos); SplitWaterTriangleAlongXLine: rama X1==X3 corregida (el EXE 0x6ECF5D..0x6ED2DB interpola desde P3: B=(1-t)*P3+t*P1 en (X1,YS), H=(1-t)*P3+t*P2 en (XS,YS); la fuente usaba lerp(P1,P3)/lerp(P2,P3) invertidos); rama X2==X3: lerps P12,P13,P23 verificados en el EXE (0x6ED2E6..), rect P12,P2,P13,P23 coherente con la geometria (orden de structs en 0x6ED5AC no cerrado del todo). Pendiente de re-ejecutar el banco.

## Aprobacion parcial por funcion (corrida 20261009-095105-342457)
- Sonda ASI 9843870c...8099c (manifiesto en `artifacts/build-manifest.json`), EXE 72ae59e4...; 27616 pares, `passed=false` global porque fallan tres funciones; saves y configuraciones intactos; estado restaurado. El banco escribe `complete` solo si todo pasa; con diferencias cierra con un evento `error` (pairs=27616, state_restored=true, sin `reason`): la corrida termino con normalidad.
- Registradas 52 funciones con todos sus pares iguales (25056 pares): CClothesBuilder 14, CShadows 19, CWaterLevel 19. Re-auditoria en streaming y evidencia en `runs/20261009-095105-342457/` (jsonl en `.gz`, hash original verificado). Resumen: `validation-summary.json`; script: `register_gfxwave2.py`.
- RECHAZADAS (sin registrar, sin credito): CWaterLevel::AddWaterLevelQuad (254/1024), RenderFlatWaterRectangle (472/1024), SplitWaterTriangleAlongXLine (258/512).
- Fuera del banco y sin credito: CreateSkinnedClump, RenderStoredShadows, CastPlayerShadowSectorList, WaterLevelInitialise, RenderBoatWakes, SplitWaterTriangleAlongYLine. `Fx_c::GetFxQuality` es un stub fijo sin traza.
- Ninguna de las tres clases queda completa segun docs/hooks.csv.
- Aviso: `source/game_sa/WaterLevel.cpp` se modifico en el arbol de trabajo DESPUES de la corrida (09:57); el registro conserva el hash probado, asi que `source_match` es falso para las entradas de CWaterLevel hasta volver a validar. Las demas fuentes coinciden con el manifiesto.
- Las notas de `notes_Wl.md` sobre dudas de SplitWaterTriangleAlongXLine (rama X2==X3) siguen vigentes: es una de las funciones rechazadas.
