<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnS13A16A: lotes S13A, S14A (RpMatFX) y S15A, S16A (RwCore) del plan de sustitucion (21/11/2026)

Plan: [docs/ExeIndependiente/plan-sustitucion.md](../../docs/ExeIndependiente/plan-sustitucion.md); raices en [raices.csv](../../docs/ExeIndependiente/raices.csv). Worktree `wv-19`, rama `s13a16a`, sobre f16f4d4b82. Sin push.

**Compilacion: OK** (ASI Debug completo `EXIT=0` bajo candado, `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1`, `build-project.ps1` en wv-19). Tras el primer intento hubo dos arreglos de enlace/compilacion (camera->object.object.parent; `_rwD3D9SetClippingForObject` sacada del espacio anonimo de rpworld.cpp para usarla desde rpmatfx.cpp).

Estado: **91 funciones de los 4 lotes + 3 extras reconstruidas en C++ y enganchadas (94 filas nuevas en `docs/hooks.csv`, validated=0)**. Los llamadores propios (RpMatFXPluginAttach, _rpMatFXPipelinesCreate, D3D9RasterPluginAttachExe, las
envolturas de rwcore.cpp) usan ya las funciones propias. Sin bancos ni juego (decision del 20/11). Extras: los callbacks 0x804990, 0x804B60 y 0x804B70 (por textura, escritura de diccionarios), que las funciones de S16A pasaban por direccion del EXE.

Fuentes: `source/game_sa/RenderWare/rw/rpmatfx.cpp` (S13A y S14A; hooks en `RpMatFXInjectHooks`) y `rwcore.cpp` (S15A y S16A; hooks en `RwCoreInjectHooksS15A16A`, llamada nueva en `InjectHooksMain.cpp`).

## Tabla

| Lote | Funcion | Direccion | Fuente | Estado | Verificacion |
|---|---|---|---|---|---|
| S13A | `RpMatFXMaterialDataSetFreeListCreateParams` | 0x810700 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXEngineDestructor` | 0x810BB0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXEngineConstructor` | 0x810BF0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXMaterialConstructor` | 0x810C40 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXMaterialStreamGetSize` | 0x811970 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXAtomicConstructor` | 0x811A00 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXAtomicDestructor` | 0x811A20 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXAtomicCopy` | 0x811A40 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXAtomicStreamWrite` | 0x811A60 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXAtomicStreamGetSize` | 0x811AE0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXWorldSectorConstructor` | 0x811B00 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXWorldSectorDestructor` | 0x811B20 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXWorldSectorCopy` | 0x811B40 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXWorldSectorStreamWrite` | 0x811B60 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `_rpMatFXWorldSectorStreamGetSize` | 0x811BE0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `RpMatFXAtomicEnableEffects` | 0x811C00 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `RpMatFXWorldSectorEnableEffects` | 0x811C40 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `RpMatFXWorldSectorQueryEffects` | 0x811C70 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `RpMatFXMaterialSetupEnvMap` | 0x811ED0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `RpMatFXMaterialSetupDualTexture` | 0x812040 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `RpMatFXMaterialSetBumpMapFrame` | 0x812320 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `RpMatFXMaterialGetBumpMapBumpedTexture` | 0x8123E0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S13A | `RpMatFXMaterialGetBumpMapTexture` | 0x812430 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `_rpMatFXMaterialDestructor` | 0x810C60 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `_rpMatFXMaterialCopy` | 0x810D20 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `_rpMatFXMaterialStreamWrite` | 0x8111F0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `_rpMatFXMaterialStreamRead` | 0x811410 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialSetupBumpMap` | 0x811E00 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialSetBumpMapCoefficient` | 0x812380 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetBumpMapFrame` | 0x812480 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetBumpMapCoefficient` | 0x8124D0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialSetEnvMapTexture` | 0x812530 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialSetEnvMapFrameBufferAlpha` | 0x812610 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetEnvMapTexture` | 0x8126F0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetEnvMapFrame` | 0x812740 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetEnvMapFrameBufferAlpha` | 0x812790 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetEnvMapCoefficient` | 0x8127E0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialSetDualTexture` | 0x812830 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetDualTexture` | 0x812930 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetDualBlendModes` | 0x812980 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXMaterialGetUVTransformMatrices` | 0x812A50 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `_rpMatFXD3D9AllInOneRender` | 0x815C20 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `_rpMatFXAtomicInstanceCallBack` | 0x8160A0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `_rpMatFXWorldSectorInstanceCallBack` | 0x816120 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S14A | `RpMatFXGetD3D9Pipeline` | 0x8162F0 | `rpmatfx.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwD3D9RasterExtConstructor` | 0x4C9A60 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwCameraSetFreeListCreateParams` | 0x7EE0F0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwCameraGetPluginOffset` | 0x7EE480 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwCameraValidatePlugins` | 0x7EE4A0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwCameraClone` | 0x7EF3B0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwIm3DGetTransformPipeline` | 0x7EF9D0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwIm3DGetRenderPipeline` | 0x7EF9E0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `_rwIm3DGetPool` | 0x7EFDD0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwFrameSetFreeListCreateParams` | 0x7EFED0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwFrameCloneHierarchy` | 0x7F0250 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `_rwFrameInit` | 0x7F0450 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `_rwFrameDeInit` | 0x7F06F0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwFrameGetRoot` | 0x7F09B0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwFrameCount` | 0x7F0E00 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwFrameScale` | 0x7F0ED0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwFrameGetPluginOffset` | 0x7F1290 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwFrameValidatePlugins` | 0x7F12B0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwTextureGetReadCallBack` | 0x7F3520 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `_rwTextureSetAutoMipMapState` | 0x7F3590 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `_rwTextureGetAutoMipMapState` | 0x7F35C0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwTextureGetPluginOffset` | 0x7F3BE0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwTextureValidatePlugins` | 0x7F3C00 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S15A | `RwTexDictionaryGetPluginOffset` | 0x7F3C40 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTexDictionaryValidatePlugins` | 0x7F3C60 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTextureSetMipmapGenerationCallBack` | 0x7F3C70 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTextureGetMipmapGenerationCallBack` | 0x7F3C90 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTextureSetMipmapNameCallBack` | 0x7F3CA0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTextureGetMipmapNameCallBack` | 0x7F3CC0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTexDictionarySetFreeListCreateParams` | 0x7F3E80 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwD3D9VertexBufferManagerChangeDefaultSize` | 0x7F57F0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwRasterGetPluginOffset` | 0x7FAE40 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwRasterGetOffset` | 0x7FAEA0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwRasterSetFreeListCreateParams` | 0x7FB350 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwD3D9GetSamplerState` | 0x7FC400 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTextureSetStreamAlwaysCallBack` | 0x804580 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTexDictionaryRegisterPluginStream` | 0x8048E0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTexDictionarySetStreamAlwaysCallBack` | 0x804910 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTexDictionaryStreamGetSize` | 0x804930 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RwTexDictionaryStreamWrite` | 0x8049F0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `_rwTextureChunkInfoRead` | 0x804E60 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RxPipelineSetFreeListCreateParams` | 0x804FC0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RxPacketCreate` | 0x805300 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RxClusterSetStride` | 0x805330 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RxClusterSetExternalData` | 0x805340 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RxClusterSetData` | 0x8053A0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| S16A | `RxClusterDestroyData` | 0x8054C0 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| extra S16A | `TexDictionarySizeTextureCallBack` | 0x804990 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| extra S16A | `TexDictionaryCountTextureCallBack` | 0x804B60 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |
| extra S16A | `TexDictionaryWriteTextureCallBack` | 0x804B70 | `rwcore.cpp` | reconstruida y enganchada; los llamadores propios usan la funcion propia | no ejecutada (sin juego) |

## Notas de las funciones delicadas
- Coeficientes de relieve: se guardan NEGADOS (fld+fchs+fstp); un NaN senalizante sale silencioso (`X87Reload*`); las copias de palabras usan memcpy.
- `_rpMatFXMaterialCopy` (0x810D20): la copia de la ranura de relieve copia +4 y +8 (no el marco) y suma refCount a ambas texturas; entorno/dual/UV como el EXE. Un revisor independiente detecto un desplazamiento mal leido (ebp = ranura + 4) y se corrigio.
- `_rpMatFXMaterialStreamRead` (0x811410): 1.0f / ancho con `Veh83DDiv` + `Veh83DStore` (x87); los errores de textura (RwErrorGet/Set con 0x80000000 y 0x16 tolerados) se replican; fallo de la 2a textura de relieve destruye la 1a.
- `_rpMatFXD3D9AllInOneRender` (0x815C20): estado plano con iluminacion apagada y sin bandera 8; SetIndices, flujos, declaracion; por malla el despacho por banderas (1..6).
- `RxClusterDestroyData` devuelve NULL (como el EXE), `RpMatFXMaterialGetUVTransformMatrices` pasa a `RwMatrix**` (firma del SDK).

## Lo que NO se hizo
- No se ejecuto ningun banco ni el juego. No se emulo con Unicorn (el oraculo seria mi propia lectura del desensamblado); la comprobacion fue una segunda lectura independiente por un revisor (solo desensamblado contra C++).
- Los callbacks 0x4C9A80 (destructor del plugin de raster, lote S18A) siguen por direccion del EXE.
- `docs/ReversedClasses.md` y los bloques de estado de las fuentes no se regeneraron.
