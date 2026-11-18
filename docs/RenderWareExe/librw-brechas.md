<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11770 con evidencia histórica y 671 pendientes de validación en 24 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cruce con librw: qué de la API de RenderWare que usa ReSA no está en librw

[Plan de RenderWare por lotes](plan-lotes.md) · [Mapa de la región](README.md) · [Lista completa (CSV)](librw-brechas.csv)

Generado por `contrib/librw_brechas.py` el 15/11/2026. Cruza la API de RenderWare y D3D que el código del proyecto **llama** (identificadores `Rw*`, `Rp*`, `Rt*`, `_rw*` fuera de `source/game_sa/RenderWare/rw`, con su número de usos) con el código de `librw` (copia de pruebas en `D:\Original\librw-test\librw`).

**Cómo leerlo.** El cruce es **por nombre**: `RwFrameCreate` se busca como `Frame::create`. `librw` es una reimplementación con otra API, así que un «no» puede ser una diferencia de nombre y no una falta real. Por eso hay dos niveles: la lista automática (CSV) y la revisión de abajo, que separa lo que parece brecha real de lo que parece nombre distinto. La revisión es un juicio a partir de los nombres y del código de `librw`; **no está probada** contra el juego.

## Resultado automático

436 funciones de la API usadas por el código del proyecto.

| Módulo | Sí | Parcial | No | Acceso a campo | Código propio |
| --- | ---: | ---: | ---: | ---: | ---: |
| RwCore | 65 | 29 | 35 | 27 | 6 |
| RpWorld | 34 | 10 | 26 | 19 | 51 |
| D3D9 | 0 | 8 | 29 | 3 | 0 |
| Im2D/Im3D | 0 | 8 | 14 | 2 | 4 |
| RpSkin | 0 | 1 | 6 | 3 | 1 |
| RpHAnim | 0 | 0 | 6 | 0 | 2 |
| RtQuat | 1 | 3 | 4 | 0 | 0 |
| RtCharset | 5 | 0 | 1 | 0 | 0 |
| otros | 0 | 3 | 3 | 0 | 0 |
| RpMatFX | 0 | 0 | 3 | 2 | 0 |
| RpMorph | 0 | 0 | 1 | 4 | 0 |
| RtAnim | 1 | 0 | 4 | 0 | 0 |
| RtDict | 0 | 1 | 2 | 0 | 1 |
| Rt (otros) | 0 | 0 | 1 | 0 | 2 |
| RpAnisot | 0 | 0 | 1 | 0 | 1 |
| RpUVAnim | 0 | 0 | 0 | 0 | 1 |
| RtBMP | 0 | 0 | 1 | 0 | 0 |
| RtPNG | 0 | 0 | 1 | 0 | 0 |
| **Total** | 106 | 63 | 138 | 60 | 69 |

«Acceso a campo» son accesores triviales (`RwTextureGetRaster`, `RpGeometryGetMaterial`...) que en `librw` son campos de estructura; «código propio» son funciones del proyecto que llevan nombre `Rp*` pero no son del SDK (`RpAnimBlend*`, `RpNan*`, ayudantes `*Cpp`).

## Revisión de las «no»

### Brechas que parecen reales (el SDK de SA hace algo que `librw` no hace)

| Qué | Funciones del proyecto que lo usan | Por qué parece brecha |
| --- | --- | --- |
| Plugin 2dfx de SA | `RpGeometryGet2dFxCount`, `RpGeometryGet2dFxAtIndex` | `librw` no tiene ningún plugin 2dfx (sin rastro en su código). |
| Lectura en streaming de SA | `RpClumpGtaStreamRead1/2`, `RpClumpGtaCancelStream`, `RwTexDictionaryGtaStreamRead*` | Es la lectura asíncrona propia de SA; `librw` no la tiene. |
| Plugins de SA restantes: Breakable, nombres de nodo, pipeline set, normales extra, COL embebido, material especular y de entorno, `RpAnisot` | `RpAnisotPluginAttach`, registros de plugin | Ninguno existe en `librw`; el MatFX de `librw` no es el de SA. |
| Conversión de geometría a lista/tira de triángulos | `RpAtomicConvertGeometryToTL/TS`, `RpClumpConvertGeometryToTL/TS` | Sin equivalente en `librw`; hay que comprobar si es una función de SA o del SDK. |
| Puente D3D9 de bajo nivel | `RwD3D9SetTransform`, `_rwD3D9SetDepthStencilSurface`, `RwD3D9GetCurrentD3DDevice`, `RwD3D9GetCaps`, `RwD3D9ChangeMultiSamplingLevels`, `_rwD3D9SetStreams`, `RwD3D9SetLight`, `RwD3D9EnableLight`, `_rwD3D9CameraClear`, etc. (29) | El juego llama al dispositivo D3D9 a través de RenderWare (79 usos de `SetTextureStageState`, 28 de `SetRenderState`); `librw` usa su propio backend y no expone estas funciones: hace falta un puente. |
| Escritura de imágenes | `RtPNGImageWrite`, `RtBMPImageRead` | `librw` lee PNG/BMP pero la escritura está sin probar. |
| BSP / mundos | `_rpMaterialListDeinitialize`, `_rpMaterialListFindMaterialIndex`, y toda la maquinaria de `RpWorld`/sectores | El README de `librw` dice que BSP no está soportado. |

### Parecen diferencias de nombre o de forma (hay que confirmar a mano)

| Qué | Funciones | Nota |
| --- | --- | --- |
| Estados de render | `RwRenderStateSet` (846 usos), `RwRenderStateGet` | `librw` tiene `engine->setRenderState`; no se encontró por nombre. Es lo más usado de toda la API. |
| Vértices Im2D e Im3D | `RwIm2DVertexSet*`, `RwIm3DVertexSet*` (14) | `librw` tiene vértices Im2D e Im3D con campos; los setters del SDK son macros. |
| Recorrer objetos | `RpClumpForAllAtomics` (59 usos), `RpGeometryForAllMaterials`, `RwFrameForAllObjects`, `RwTexDictionaryForAllTextures` | `librw` usa listas y bucles propios. |
| Skin y HAnim | `RpSkinGetVertexBoneWeights/Indices`, `RpSkinAtomicGet/SetHAnimHierarchy`, `RpHAnimHierarchyGetMatrixArray` (43 usos), `RpHAnimHierarchyUpdateMatrices` | `librw` tiene Skin y HAnim con otros nombres; el rendimiento y los datos coinciden (medido: 0 diferencias en skin y HAnim). |
| MatFX | `RpMatFXMaterialSetEnvMapCoefficient`, `RpMatFXMaterialSetEnvMapFrame`, `RpMatFXAtomicQueryEffects` | Existe el módulo MatFX; nombres distintos. Ojo: nunca se comparó el MatFX contra el EXE. |
| Cuaterniones | `RtQuatConvertFromMatrix`, `RtQuatConvertToMatrix`, `RtQuatSetupSlerpCache` | `librw` tiene `Quat` con otros nombres. |
| Animación | `RtAnimAnimationCreate/Destroy`, `RtAnimRegisterInterpolationScheme` | `librw` tiene `anim.cpp`. |
| Texturas | `RwTexDictionaryFindNamedTexture`, `RwTextureSetFindCallBack`, `RwTextureSetAddressingU/V` | `librw` tiene `TexDictionary` y `Texture::setFilterMode`, `addressU/V`. |

## Lo que no entra en este cruce

- Las funciones **internas** del SDK del EXE (la región 0x750000–0x81FFFF, 1.828 pendientes) que el juego no llama directamente. Eso se mide mejor comparando comportamiento, no nombres.
- Lo que `librw` hace distinto sin que haya una función equivalente: formato de pipelines, color de pintura de los coches, filtrado de partes por nombre de frame, reflejos (ver `docs/RutaExeIndependiente.md`).

## Para seguir

1. Confirmar a mano las filas de «parecen diferencias de nombre»: comprobar en `librw` que cada una tiene su equivalente.
2. Para las brechas reales, decidir entre añadirlas a `librw` o reconstruirlas en el proyecto. Las del juego (2dfx, streaming, plugins de SA, puente D3D9) son las que bloquean el ejecutable propio.
