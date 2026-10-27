<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# notes_Cl (CClothesBuilder, parte Cl de GfxWave2)

## Correcciones de fuente (source/game_sa/ClothesBuilder.cpp)
- GetAtomicWithName 0x5A47E0 (callback): al coincidir devuelve NULL (`0x5A4801 jne` no tomado, eax=0) y detiene RpClumpForAllAtomics: gana la PRIMERA coincidencia; la fuente seguia y se quedaba con la ultima.
- AddWeightToBoneVertex 0x5A4840: ranura libre -> `weights[i] += add` (fadd 0x5A489C) y `weights[i+1]=0`; si las 8 ranuras estan ocupadas y no hay coincidencia el EXE hace `weights[8] += add` (0x5A487F-0x5A4888) en vez de NOTSA_UNREACHABLE.
- StoreBoneArray 0x5A48B0: bucle hacia delante sin limite (`row[i]=nodeID`, 0x5A4900-0x5A4916) y UN solo -1 de cierre si i<64 (0x5A4918/0x5A4923); la fuente rellenaba toda la fila con -1.
- BuildBoneIndexConversionTable 0x5A56E0: `cmp al,0FFh` (0x5A5712) compara solo el byte bajo: RpHAnimIDGetIndex = -1 -> 0.
- CopyTexture 0x5A5730: formato = `(cFormat & 0x6F) << 8 | 4` (0x5A5742 and eax,0FFFFFF6Fh; shl 8; or 4), no `& 0x0F00`.
- PlaceTextureOnTopOfTexture 0x5A57B0: lock del ORIGEN primero y luego destino (0x5A57C4/0x5A57D0), numero de pixeles = ancho*alto del ORIGEN (0x5A57D5), unlock origen y luego destino (0x5A5804/0x5A580A).
- BlendTextures x3 (0x5A5820/0x5A59C0/0x5A5BC0): reescritas (el bucle anterior con `i--` anidado no equivalia): orden de locks (src, [src2], dst, [tat]) y de unlocks, cuenta de pixeles del primer origen, 3 canales (alfa intacto), suma `src2*r3 + src1*r2 + dst*r1` (2 texturas: `src*r2 + dst*r1`), conversion _ftol2 y byte bajo sin saturar (`cmp al,0FFh ; ja` nunca salta), orden de escritura canal 0,2,1; tatuaje: `fild((tat-v)*alfa) * 0x859A3C(0x3B808081) + v`; CTimer::Suspend/Resume (0x5619D0 / 0x561A00 tail jmp).
- CreateSkinnedClump 0x5A69D0: (a) `[0BC1288h] = odscr` (0x5A6A85); (b) banderas con odscr: AA5=AA6=0; si fat/muscle difieren (fcompp, NaN = distinto) AA4=AA5=1 si no AA4=0; si las 10 claves de modelo difieren AA5=1; si las 18 claves de textura difieren AA6=1 (0x5A6B84-0x5A6C2E) — la fuente tenia las banderas cruzadas y AA6 siempre a true; (c) ratios 0x5A42B0: `max(fat-200,0)*0x865354(0.00125f)`, `max(stat,0)*0x858CDC(0.001f)` (multiplica, no divide), recorte a 1.0 solo si > 1, normal = 1-(f+m), reescalado si f+m > 1; (d) `tmpGeo->flags |= 2` tras RpGeometryDestroy (0x5A6D5A).
- MATCH (sin cambios): LoadCdDirectory 0x5A4190, RequestGeometry 0x5A41C0 (bHasComplexHierarchy = bit 1 de +0x13), RequestTexture 0x5A4220, DestroySkinArrays 0x5A56C0 (dos operator delete 0x8214BD), GetTextureFromTxdAndLoadNextTxd 0x5A5F70.

## Banco (ClothesPart.hpp): 14 funciones
Casos: AddWeight 512, Blend2 256, Blend3 256, BlendTat 256, BuildBoneIndexConversionTable 256, CopyTexture 128, DestroySkinArrays 64, GetAtomicWithName 512, GetTextureFromTxd 512, LoadCdDirectory 64, Place 256, RequestGeometry 256, RequestTexture 256, StoreBoneArray 512.
Se compara el valor devuelto, las trazas de colaboradores (con argumentos crudos) y todos los bytes de fixtures (texturas, rasters, pixeles, arena de RwRasterCreate/RwTextureCreate, gBoneIndices, modelinfo falso, tablas). Solo dimensiones de raster coherentes (w*h<=36, 32 bits): los asserts de la DLL (AssertTextureLayouts) y la diferencia ORIGEN/DESTINO de la cuenta de pixeles no se ejercitan.

## Fuera
- CreateSkinnedClump 0x5A69D0 (regla 17): llama ConstructGeometryArray (carga real de PLAYER.IMG con CStreaming::LoadAllRequestedModels, 0x5A55A0), RwTexDictionary real, y crea clump/atomo/frame/hierarchy RW reales; no se puede doblar con seguridad. Solo comparado con el desensamblado.

## Supuestos al compilar
- Los hooks de GetAtomicWithName / GetTextureFromTxd... estan en la categoria "CClothesBuilder" (RH_ScopedGlobalInstall); si FindHook no los encuentra, ajustar el nombre de categoria.
- Simbolos de la DLL usados: CDirectory::{Init,ReadDirFile,FindItem const}, CStreaming::{RequestFile,LoadRequestedModels,LoadAllRequestedModels,RemoveModel}, CTimer::{Suspend,Resume}, GetFrameNodeName, RpSkinAtomicGetHAnimHierarchy, RpHAnimIDGetIndex, RwRasterCreate/Lock/Unlock, RwTextureCreate, GetFirstTexture.
- GetFirstTexture, RwRaster* y RwTextureCreate se parchean tambien en el EXE (0x734940, 0x7FB230, 0x7FB2D0, 0x7FAEC0, 0x7F37C0): el coordinador debe instalar Stubs() solo durante las funciones de esta parte.
- DestroySkinArrays: EntryPatch local de 0x8214BD; si la DLL llama a un operator delete con tamano y no reenvia a 0x8214BD, no habra traza.
- GfxCommon.hpp: en la copia leida `Push` aparece truncado (falta el cuerpo `auto& r = recs[nrec++]; ...` y la llave de cierre) y no hay `Recs()`; ClothesPart.hpp los usa (como en GeoFinal1).
