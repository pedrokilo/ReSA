# FnS17A20A: lotes S17A, S18A, S19A y S20A del plan de sustitucion (RenderWare: RwCore y RwPlCore), 21/11/2026

Plan: [docs/ExeIndependiente/plan-sustitucion.md](../../docs/ExeIndependiente/plan-sustitucion.md), raices [raices.csv](../../docs/ExeIndependiente/raices.csv), lotes [lotes-sustitucion.csv](../../docs/ExeIndependiente/lotes-sustitucion.csv).
Ejemplo previo de principio a fin: [FnS7A1A](../FnS7A1A/README.md).

Estado: **reconstruido y enganchado: 90 de 91 raices del grupo (falta 0x81F360, ver "Pendiente")**; mas 2 funciones internas que esas raices llamaban por direccion y que ningun lote cubria.
Sin bancos en juego (decision del 20/11/2026): solo reconstruir, enganchar, compilar y comprobar que no rompe. Las filas nuevas de `docs/hooks.csv` quedan con `validated=0`.
Worktree `wv-20`, rama `s17a20a`, sobre `origin/main`. Sin push.

## Que se hizo
Cada raiz era una envoltura (`((tipo(__cdecl*)(...))0xADDR)(...)`) o una direccion fija registrada como callback. Ahora el cuerpo esta escrito en C++ desde el desensamblado (capstone, `exedis.py`), lleva `RH_ScopedGlobalInstall`
en su direccion del EXE y el propio codigo registra las funciones propias en lugar de la direccion:
- `RwCameraCreate` guarda `_rwCameraSyncCallBack`, `_rwCameraBeginUpdateCallBack` y `_rwCameraEndUpdateCallBack` (0x7EE5A0, 0x7EF370, 0x7EF340).
- `D3D9RasterPluginAttachExe` registra `_rwD3D9RasterExtDestructor` (0x4C9A80).
- Las busquedas de imagenes (`RwImageRead*`, `_rwImageFindFormatExtension`) pasan `_rwImageOpenCallback` y `_rwImageExtensionCallback` (0x8032D0, 0x803340).
- `RwEngineInit` (rwplcore.cpp) registra por direccion propia los 7 modulos de motor: flujos, matrices, camaras, Im3D, marcos, texturas (solo el constructor; el destructor 0x7F3D00 no era de este lote) y rasters.
- El codigo de las raices vive junto a las demas del archivo: S17A/S18A/S19A y la parte de modulos de camaras, Im3D, marcos, texturas y rasters en `source/game_sa/RenderWare/rw/rwcore.cpp` (`RwCoreInjectHooksS17A..S20A`);
  el resto de S20A en `rwplcore.cpp` (`RwPlCoreInjectHooksS20A`). Las llamadas a `RwCoreInjectHooksS17A..S20A` y `RwPlCoreInjectHooksS20A` estan en `source/InjectHooksMain.cpp`.
- `strip_wrappers.py` quita las envolturas viejas (deja un comentario); `exedis.py` es el desensamblador usado.

## Tabla (lote / funcion / direccion / archivo / estado)
Compilacion: **OK** (`build-project.ps1` bajo candado, EXIT=0, 21/11/2026 00:40; la primera pasada dio un unico error de declaracion de `_rwDeviceSystemRequest`, corregido). Verificacion contra el EXE: solo `RwBBoxAddPoint` y `RwBBoxContainsPoint` (ver abajo); el resto no se comprobo.

| Lote | Funcion | Direccion | Fuente | Estado |
|---|---|---|---|---|
| S17A | `RxClusterUnlock` | 0x8055C0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineNodeGetPipelineCluster` | 0x805680 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineClusterGetCreationAttributes` | 0x8056B0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineClusterSetCreationAttributes` | 0x8056C0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxClusterGetAttributes` | 0x8056E0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxClusterSetAttributes` | 0x8056F0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineNodeFindOutputByIndex` | 0x805D70 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineNodeFindInput` | 0x805DA0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineNodeRequestCluster` | 0x805DB0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineNodeReplaceCluster` | 0x805E20 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineNodeGetInitData` | 0x805F30 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineClone` | 0x806AC0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxPipelineFindNodeByIndex` | 0x806BC0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxLockedPipeReplaceNode` | 0x806F20 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxLockedPipeDeleteNode` | 0x807040 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxLockedPipeSetEntryPoint` | 0x807070 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxLockedPipeGetEntryPoint` | 0x8070D0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxLockedPipeAddPath` | 0x807100 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RxLockedPipeDeletePath` | 0x807170 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RwFrameListSetAutoUpdate` | 0x807570 | rwcore.cpp | reconstruida y enganchada |
| S17A | `RwFrameSetStreamAlwaysCallBack` | 0x8075B0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `_rwFrameListFindFrame` | 0x8076E0 | rwcore.cpp | reconstruida y enganchada |
| S17A | `_rwFrameListStreamGetSize` | 0x807750 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwD3D9RasterExtDestructor` | 0x4C9A80 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwCameraSyncCallBack` | 0x7EE5A0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwCameraEndUpdateCallBack` | 0x7EF340 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwCameraBeginUpdateCallBack` | 0x7EF370 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwTexDictionaryForAllTexDictionaries` | 0x7F3770 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwTextureGenerateMipmapName` | 0x7F3AA0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwD3D9SetVertexShaderConstant` | 0x7FACA0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwD3D9SetPixelShaderConstant` | 0x7FAD00 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwRasterRenderScaled` | 0x7FAE80 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwRasterClear` | 0x7FAEE0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwRasterClearRect` | 0x7FAF90 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwRasterRender` | 0x7FAFB0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwRasterSubRaster` | 0x7FB1D0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwD3D9SetClipPlane` | 0x7FC4A0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwImageSetPath` | 0x802EA0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwImageOpenCallback` | 0x8032D0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwImageExtensionCallback` | 0x803340 | rwcore.cpp | reconstruida y enganchada |
| S18A | `_rwFrameListStreamWrite` | 0x8077A0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwCameraRegisterPluginStream` | 0x808C90 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwCameraSetStreamAlwaysCallBack` | 0x808CC0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwCameraStreamGetSize` | 0x808CE0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwCameraChunkInfoRead` | 0x808EF0 | rwcore.cpp | reconstruida y enganchada |
| S18A | `RwBBoxInitialize` | 0x809020 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RwIm3DRenderTriangle` | 0x7EF810 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RwIm3DRenderLine` | 0x7EF900 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RwRasterRead` | 0x8043F0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RwRasterReadMaskedRaster` | 0x8044E0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `_rwTextureDestroyCallBack` | 0x804E40 | rwcore.cpp | interna (no estaba en el lote; la llamaba una raiz) |
| S19A | `RwTexDictionaryStreamRead` | 0x804C30 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxClusterInitializeData` | 0x805400 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxClusterResizeData` | 0x805470 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxClusterLockWrite` | 0x8054F0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxPipelineNodeSendConfigMsg` | 0x8055D0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxPipelineNodeForAllConnectedOutputs` | 0x8055F0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxPipelineNodeFindOutputByName` | 0x805D10 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxPipelineNodeCreateInitData` | 0x805EA0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxPipelineInsertDebugNode` | 0x8071B0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `_rwFrameListInitialize` | 0x8075D0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RwCameraStreamWrite` | 0x808D00 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RwBBoxAddPoint` | 0x809060 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RwBBoxContainsPoint` | 0x8090E0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `RxHeapAlloc` | 0x809AA0 | rwcore.cpp | reconstruida y enganchada |
| S19A | `_rxHeapAllocFromEntry` | 0x809C70 | rwcore.cpp | interna (no estaba en el lote; la llamaba una raiz) |
| S19A | `RxHeapRealloc` | 0x809D10 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwCameraModuleClose` | 0x7EE0B0 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwCameraModuleOpen` | 0x7EE110 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwIm3DModuleClose` | 0x7EFDE0 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwIm3DModuleOpen` | 0x7EFE20 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwFrameModuleOpen` | 0x7EFEF0 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwFrameModuleClose` | 0x7EFF70 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwTextureModuleOpen` | 0x7F3EA0 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwRasterModuleClose` | 0x7FB310 | rwcore.cpp | reconstruida y enganchada |
| S20A | `_rwRasterModuleOpen` | 0x7FB370 | rwcore.cpp | reconstruida y enganchada |
| S20A | `RwStreamSetFreeListCreateParams` | 0x7EC760 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `_rwStreamModuleOpen` | 0x7EC780 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `_rwStreamModuleClose` | 0x7EC7E0 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwStreamWriteInt16` | 0x7ED480 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwStreamReadInt16` | 0x7ED4A0 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `_rwMatrixModuleClose` | 0x7F1660 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwMatrixSetFreeListCreateParams` | 0x7F16A0 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `_rwMatrixModuleOpen` | 0x7F16C0 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwEngineGetMatrixTolerances` | 0x7F1780 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwEngineSetMatrixTolerances` | 0x7F17B0 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwEngineGetVersion` | 0x7F2BA0 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwEngineGetTextureMemorySize` | 0x7F2D80 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwEngineGetMaxTextureSize` | 0x7F2DB0 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwEngineSetFocus` | 0x7F2DE0 | rwplcore.cpp | reconstruida y enganchada |
| S20A | `RwEngineGetMetrics` | 0x7F2E10 | rwplcore.cpp | reconstruida y enganchada || S19A | `RwD3D9RasterStreamReadDDS` | 0x81F360 | rwcore.cpp | **PENDIENTE**: sigue siendo la envoltura del EXE |

## Notas de reconstruccion (defectos del EXE que se reproducen)
- `RwRasterRead` (0x8043F0): si `stdFunc[IMAGEFINDRASTERFORMAT]` falla, el EXE usa el puntero del nombre de archivo como ancho, alto y banderas y deja la profundidad sin inicializar (aqui 0).
- `RxLockedPipeReplaceNode` (0x806F20): rellena con 0xFFFFFFFF las salidas del nodo VIEJO (no las del nuevo) desde `min(numOutputs)` hasta `numOutputs` del nuevo; puede escribir pasado el final del vector del viejo. Reproducido.
- `RxPipelineInsertDebugNode` (0x8071B0): el resultado de `RxLockedPipeAddFragment` no se comprueba (NULL bloquea el proceso, como en el EXE), y si `RxPipelineLock` falla no se libera la lista reservada.
- `RxHeapRealloc` (0x809D10): `allowCopy` no se usa; al encoger no se fusiona el sobrante con el bloque libre siguiente. `RxHeapAlloc` (0x809AA0) formatea con rwsprintf "Heap resized from %d to %d bytes" en un bufer local y
  descarta el resultado: no se reproduce (sin efecto visible).
- Comparaciones de float (`RwBBoxAddPoint` 0x809060, `RwBBoxContainsPoint` 0x8090E0): reproducen los saltos `fcomp + fnstsw + and ax,0x4100 / test ah,5 + jne/jp` (con NaN no asignan; `Contains` da TRUE con NaN). Las copias van por palabras de 32 bits.
- `RwCameraStreamWrite` (0x808D00): near/far/fog pasan por la x87 (fld/fstp): un NaN senalizante sale silencioso (`S19X87FloatPassThrough`); ventana y desplazamiento se copian como palabras.
- `_rwCameraSyncCallBack` (0x7EE5A0): operaciones float en el orden exacto de la x87 con el control a 24 bits (equivale a float); no se emula el rango de exponente extendido de la x87 para intermedios fuera de rango de float.
- `RwEngineGetMatrixTolerances/Set...` copian palabras (sin x87).

## Pendiente / lo que no se pudo
- **0x81F360 `RwD3D9RasterStreamReadDDS` (5801 bytes)**: lector de DDS con 73 llamadas, varias a internas sin reconstruir (0x4CBDE0, 0x4CBEE0, 0x81E720, 0x81EA70, 0x81EDC0). No se abordo por tamano. Con el queda S19A al 19/20.
- Verificacion contra el EXE (Unicorn o `FnLIB1A/bench.cpp`): solo hecha para `RwBBoxAddPoint` y `RwBBoxContainsPoint` con `emu_bbox.py` (1331 casos: ceros, signos, infinitos, NaN silencioso y senalizante, denormales; 0 diferencias entre el EXE emulado y el modelo que sigue el C++; compara el modelo, no el codigo compilado). Sin comprobar: `_rwCameraSyncCallBack` (x87), el monton de Rx (`RxHeapAlloc`, `RxHeapRealloc`, `_rxHeapAllocFromEntry`) y el resto.
- Funciones internas del EXE que siguen llamandose por direccion desde lo nuevo (fuera de estos lotes): 0x7F53D0, 0x7F40F0 y 0x7F5140 (callbacks por defecto del modulo de texturas, guardados por `_rwTextureModuleOpen`),
  0x7F12F0 (multiplicacion por defecto del modulo de matrices) y 0x7F3D00 (destructor del modulo de texturas).
