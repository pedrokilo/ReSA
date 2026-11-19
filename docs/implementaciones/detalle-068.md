<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1341"></a>

## Globales y helpers: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PipelineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:50](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| * PipelineCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:59](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| * PipelineStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:67](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gPipelinePluginOffset: source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:25 (0x008D6080) |
| PipelineGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp:74](../../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1342"></a>

## Globales y helpers: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| *& RpAnimBlendClumpGetData(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:31](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ClumpOffset: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:28 (0x00B5F878) |
| FrameInitCBskin(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:148](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendClumpInitSkinned(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:160](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FrameCountCB(RwFrame* f, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:191](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L191) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetFrameDataFrameCB(RwFrame* f, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:198](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L198) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameInitCBnonskin(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:207](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendClumpInitNonSkinned(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:219](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & RpAnimBlendClumpGetAssociations(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:261](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L261) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameFindByBoneTagCB(AnimBlendFrameData* fd, void* data) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:379](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L379) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendFrameData: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:376 (0x00B5F87C) |
| * RpAnimBlendClumpFindAssociationIf_N(RpClump* clump, Fn&& Pred, size_t n) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:453](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L453) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetAssociation(RpClump* clump, bool, CAnimBlendHierarchy* h) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:469](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L469) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetAssociation-Hier 0x004D68E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetAssociation(RpClump* clump, const char* name) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:476](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L476) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetAssociation-AnimName 0x004D6870 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetAssociation(RpClump* clump, uint32 animId) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:484](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L484) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetAssociation-AnimId 0x004D68B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetFirstAssociation(RpClump* clump) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:491](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetFirstAssociation- 0x004D15E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetFirstAssociation(RpClump* clump, uint32 flags) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:500](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L500) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendClumpGetFirstAssociation-Flags 0x004D6A70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendClumpCountAssociationsIf(RpClump* clump, Fn&& Pred) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:606](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L606) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeAdd(float a, float b) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:701](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L701) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeSub(float a, float b) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:702](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L702) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeMul(float a, float b) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:703](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L703) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculateTotalBlendOfPartial(AnimBlendUpdateData* c, AnimBlendFrameData* fd, bool checkDontAddToBlend) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:705](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L705) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackNoVelocity(AnimBlendFrameData* fd, AnimBlendUpdateData* c) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:738](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L738) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackExeVelocity(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:839](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L839) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:696 (0x00B4EA0C) |
| FrameUpdateCallBackWithVelocityExtractionCompressedSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1021](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1021) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1040](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1040) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1059](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1059) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1065](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1065) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1088](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1088) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FrameUpdateCallBackNonSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1094](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1094) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RpAnimBlendNodeUpdateKeyFrames(AnimBlendUpdateData* c, AnimBlendFrameData* frames, int32 nFrames) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1126](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendNodeUpdateKeyFramesExe 0x004D1570 | 0 / 0 | Posible dato fijo gpAnimBlendClump: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:696 (0x00B4EA0C) |
| * RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1279](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendGetNextAssociation-Any 0x004D6AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association, uint32 flags) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1287](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1287) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::RpAnimBlendGetNextAssociation-Flags 0x004D6AD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConvertPedNode2BoneTagExe(int32 pedNode) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1300](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1300) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: RpAnimBlend::ConvertPedNode2BoneTag 0x004D58A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __cdecl ConvertBoneTag2BoneNameExe(int32 boneTag) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp:1305](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp#L1305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: RpAnimBlend::ConvertBoneTag2BoneName 0x004D56F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1343"></a>

## Globales y helpers: source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RpAnimBlendClumpForEachAssociation(RpClump* clump, Fn&& IterFn) | [source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h:51](../../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.h#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1344"></a>

## Globales y helpers: source/game_sa/PointLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FogEmitSprite(const CPointLight& light, const float& alpha, const RwV3d& screen, float w, float h, int32 tab, uint32 timeValue, bool pointLight) | [source/game_sa/PointLights.cpp:156](../../source/game_sa/PointLights.cpp#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogDirectionalLight(const CPointLight& light, const float& intensity) | [source/game_sa/PointLights.cpp:164](../../source/game_sa/PointLights.cpp#L164) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogPointLight(const CPointLight& light, const float& intensity, const float& size) | [source/game_sa/PointLights.cpp:196](../../source/game_sa/PointLights.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1345"></a>

## Globales y helpers: source/game_sa/Pools.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ConstructPoolBody(uint8* pool, int32 capacity, uint32 elementSize) | [source/game_sa/Pools.cpp:234](../../source/game_sa/Pools.cpp#L234) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ConstructPool(int32 capacity, uint32 elementSize) | [source/game_sa/Pools.cpp:247](../../source/game_sa/Pools.cpp#L247) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolDestructorBodyExe(void* poolPtr, void* /*edx*/) | [source/game_sa/Pools.cpp:271](../../source/game_sa/Pools.cpp#L271) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPools::PoolDestructorBody 0x004049C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PoolScalarDeletingDestructorExe(void* self, void* /*edx*/, uint32 deleteFlags) | [source/game_sa/Pools.cpp:391](../../source/game_sa/Pools.cpp#L391) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DestroyPool(void* poolPtr) | [source/game_sa/Pools.cpp:427](../../source/game_sa/Pools.cpp#L427) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetPedPool() | [source/game_sa/Pools.cpp:767](../../source/game_sa/Pools.cpp#L767) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedPool: source/game_sa/Pools.cpp:83 (0x00B74490) |
| * GetVehiclePool() | [source/game_sa/Pools.cpp:772](../../source/game_sa/Pools.cpp#L772) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pVehiclePool: source/game_sa/Pools.cpp:84 (0x00B74494) |
| * GetBuildingPool() | [source/game_sa/Pools.cpp:777](../../source/game_sa/Pools.cpp#L777) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pBuildingPool: source/game_sa/Pools.cpp:85 (0x00B74498) |
| * GetObjectPool() | [source/game_sa/Pools.cpp:782](../../source/game_sa/Pools.cpp#L782) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pObjectPool: source/game_sa/Pools.cpp:86 (0x00B7449C) |
| * GetDummyPool() | [source/game_sa/Pools.cpp:787](../../source/game_sa/Pools.cpp#L787) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pDummyPool: source/game_sa/Pools.cpp:87 (0x00B744A0) |
| * GetColModelPool() | [source/game_sa/Pools.cpp:792](../../source/game_sa/Pools.cpp#L792) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pColModelPool: source/game_sa/Pools.cpp:88 (0x00B744A4) |
| * GetTaskPool() | [source/game_sa/Pools.cpp:797](../../source/game_sa/Pools.cpp#L797) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pTaskPool: source/game_sa/Pools.cpp:89 (0x00B744A8) |
| * GetPedIntelligencePool() | [source/game_sa/Pools.cpp:802](../../source/game_sa/Pools.cpp#L802) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedIntelligencePool: source/game_sa/Pools.cpp:90 (0x00B744C0) |
| * GetPtrNodeSingleLinkPool() | [source/game_sa/Pools.cpp:807](../../source/game_sa/Pools.cpp#L807) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPtrNodeSingleLinkPool: source/game_sa/Pools.cpp:91 (0x00B74484) |
| * GetPtrNodeDoubleLinkPool() | [source/game_sa/Pools.cpp:812](../../source/game_sa/Pools.cpp#L812) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPtrNodeDoubleLinkPool: source/game_sa/Pools.cpp:92 (0x00B74488) |
| * GetEntryInfoNodePool() | [source/game_sa/Pools.cpp:817](../../source/game_sa/Pools.cpp#L817) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pEntryInfoNodePool: source/game_sa/Pools.cpp:93 (0x00B7448C) |
| * GetPointRoutePool() | [source/game_sa/Pools.cpp:822](../../source/game_sa/Pools.cpp#L822) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPointRoutePool: source/game_sa/Pools.cpp:94 (0x00B744B0) |
| * GetPatrolRoutePool() | [source/game_sa/Pools.cpp:827](../../source/game_sa/Pools.cpp#L827) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPatrolRoutePool: source/game_sa/Pools.cpp:95 (0x00B744B4) |
| * GetEventPool() | [source/game_sa/Pools.cpp:832](../../source/game_sa/Pools.cpp#L832) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pEventPool: source/game_sa/Pools.cpp:96 (0x00B744AC) |
| * GetNodeRoutePool() | [source/game_sa/Pools.cpp:837](../../source/game_sa/Pools.cpp#L837) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pNodeRoutePool: source/game_sa/Pools.cpp:97 (0x00B744B8) |
| * GetTaskAllocatorPool() | [source/game_sa/Pools.cpp:842](../../source/game_sa/Pools.cpp#L842) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pTaskAllocatorPool: source/game_sa/Pools.cpp:98 (0x00B744BC) |
| * GetPedAttractorPool() | [source/game_sa/Pools.cpp:847](../../source/game_sa/Pools.cpp#L847) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pPedAttractorPool: source/game_sa/Pools.cpp:99 (0x00B744C4) |

<a id="grupo-1346"></a>

## Globales y helpers: source/game_sa/Pools/PoolRawDelete.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PoolFreeSlotRaw(void* pool, const void* obj, int32 elementSize) | [source/game_sa/Pools/PoolRawDelete.h:6](../../source/game_sa/Pools/PoolRawDelete.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolFreeSlotRawShift(void* pool, const void* obj, int32 shift) | [source/game_sa/Pools/PoolRawDelete.h:17](../../source/game_sa/Pools/PoolRawDelete.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1347"></a>

## Globales y helpers: source/game_sa/Population.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LoadGroupFile(const char* fileName, auto& outModelsInGroup, auto& outNumOfModelsPerGroup) | [source/game_sa/Population.cpp:171](../../source/game_sa/Population.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPossibleDriverModelsOfCar(eModelID carModelIndex, auto&& ProcessModel) | [source/game_sa/Population.cpp:610](../../source/game_sa/Population.cpp#L610) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PopulationTask(size_t size, Args... args) | [source/game_sa/Population.cpp:1911](../../source/game_sa/Population.cpp#L1911) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationModelRadius(eModelID model) | [source/game_sa/Population.cpp:1915](../../source/game_sa/Population.cpp#L1915) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationModelLoaded(eModelID model) | [source/game_sa/Population.cpp:1918](../../source/game_sa/Population.cpp#L1918) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PopulationCouple(eModelID maleModel, eModelID femaleModel, CVector position) | [source/game_sa/Population.cpp:1921](../../source/game_sa/Population.cpp#L1921) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028); Posible dato fijo ms_offsets: source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h:28 (0x00C196E8) |
| PopulationDefaultTask(CPed* ped,ePedType type) | [source/game_sa/Population.cpp:1959](../../source/game_sa/Population.cpp#L1959) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0048E610: CTaskComplexWanderCriminal::Constructor; ruta efectiva sin resolver; Colaborador por dirección 0x00681B60: CTaskManager::SetTaskSecondary; ruta efectiva sin resolver; Conversión de dirección fija: 0x00A90CF0; Posible dato fijo MI_TELLY: source/game_sa/ModelIndices.cpp:69 (0x008CD5A8) |
| PopulationSunbathe(CPed* ped,const CVector& position) | [source/game_sa/Population.cpp:1977](../../source/game_sa/Population.cpp#L1977) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0043E0C0: CPlaceable::SetHeading; ruta efectiva sin resolver; Colaborador por dirección 0x00632140: CTaskComplexSunbathe::CanSunbathe; ruta efectiva sin resolver; Conversión de dirección fija: 0x00969159; Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |

<a id="grupo-1348"></a>

## Globales y helpers: source/game_sa/PostEffects.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeLog2TruncViaDouble(int32 value) | [source/game_sa/PostEffects.cpp:39](../../source/game_sa/PostEffects.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLog2TruncKeepExtended(int32 value) | [source/game_sa/PostEffects.cpp:44](../../source/game_sa/PostEffects.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePow2Above(int32 exponentPlusOne) | [source/game_sa/PostEffects.cpp:50](../../source/game_sa/PostEffects.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x008220F0 |
| HazeScaleX(float k) | [source/game_sa/PostEffects.cpp:57](../../source/game_sa/PostEffects.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| HazeScaleY(float k) | [source/game_sa/PostEffects.cpp:58](../../source/game_sa/PostEffects.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| RippleWave(uint32 timeMs, float speed, float freq, float y, float xoffset) | [source/game_sa/PostEffects.cpp:61](../../source/game_sa/PostEffects.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RippleFinalU(uint32 timeMs, float speed, float freq, float y, float xoffset, int32 width, float* outU0, float* outU1) | [source/game_sa/PostEffects.cpp:67](../../source/game_sa/PostEffects.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogSinCoord(float angle, float quarter, float radius, float half, float third) | [source/game_sa/PostEffects.cpp:72](../../source/game_sa/PostEffects.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FogCosCoord(float angle, float quarter, float radius, float half, float third) | [source/game_sa/PostEffects.cpp:75](../../source/game_sa/PostEffects.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeClampScaleChannel(uint8 channel, float mult) | [source/game_sa/PostEffects.cpp:80](../../source/game_sa/PostEffects.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1349"></a>

## Globales y helpers: source/game_sa/ProjectileInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProjectileCreatorHeading(CEntity* creator) | [source/game_sa/ProjectileInfo.cpp:97](../../source/game_sa/ProjectileInfo.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CProjectileInfo::AddProjectile 0x00737C80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProjectileThrownVelocity(float heading, float speed, float height) | [source/game_sa/ProjectileInfo.cpp:104](../../source/game_sa/ProjectileInfo.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ProjectileField(CEntity* entity, size_t offset) | [source/game_sa/ProjectileInfo.cpp:247](../../source/game_sa/ProjectileInfo.cpp#L247) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProjectileDamping(float timestep) | [source/game_sa/ProjectileInfo.cpp:250](../../source/game_sa/ProjectileInfo.cpp#L250) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveBackwardProjectileSteering(CVector& steering,const CVector& direction) | [source/game_sa/ProjectileInfo.cpp:257](../../source/game_sa/ProjectileInfo.cpp#L257) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddProjectileSteering(CVector& velocity, const CVector& steering, float acceleration, float timestep, bool fast) | [source/game_sa/ProjectileInfo.cpp:260](../../source/game_sa/ProjectileInfo.cpp#L260) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LimitProjectileSpeed(CProjectile* projectile) | [source/game_sa/ProjectileInfo.cpp:264](../../source/game_sa/ProjectileInfo.cpp#L264) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1350"></a>

## Globales y helpers: source/game_sa/ProtectionStub.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProtectionStubEmpty(uint32 /*unused*/) | [source/game_sa/ProtectionStub.cpp:26](../../source/game_sa/ProtectionStub.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CProtectionStub::Empty 0x004032A4 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1351"></a>

## Globales y helpers: source/game_sa/Queue.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * InitialiseQueue(Queue* q, int32 size) | [source/game_sa/Queue.h:26](../../source/game_sa/Queue.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FinalizeQueue(Queue* q) | [source/game_sa/Queue.h:33](../../source/game_sa/Queue.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddToQueue(Queue* q, int32 i) | [source/game_sa/Queue.h:38](../../source/game_sa/Queue.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetFirstInQueue(Queue* q) | [source/game_sa/Queue.h:43](../../source/game_sa/Queue.h#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveFirstInQueue(Queue* q) | [source/game_sa/Queue.h:47](../../source/game_sa/Queue.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1352"></a>

## Globales y helpers: source/game_sa/RGBA.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| lerp&lt;CRGBA&gt;(const CRGBA& from, const CRGBA& to, float t) | [source/game_sa/RGBA.h:231](../../source/game_sa/RGBA.h#L231) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1353"></a>

## Globales y helpers: source/game_sa/Radar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RadarExeFloat(uintptr_t address) | [source/game_sa/Radar.cpp:33](../../source/game_sa/Radar.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RadarStretchBaseX() | [source/game_sa/Radar.cpp:38](../../source/game_sa/Radar.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| RadarStretchBaseY() | [source/game_sa/Radar.cpp:39](../../source/game_sa/Radar.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| RadarFtol(float v) | [source/game_sa/Radar.cpp:41](../../source/game_sa/Radar.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformRadarPointToScreenSpaceImpl(CVector2D* out, const CVector2D* in) | [source/game_sa/Radar.cpp:444](../../source/game_sa/Radar.cpp#L444) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| * __cdecl RadarTransformScreenExe(CVector2D* out, const CVector2D* in) | [source/game_sa/Radar.cpp:469](../../source/game_sa/Radar.cpp#L469) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CRadar::TransformRadarPointToScreenSpace 0x00583480 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Limit(float& x, float& y) | [source/game_sa/Radar.cpp:911](../../source/game_sa/Radar.cpp#L911) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| RadarRawRect(float left, float plus4, float right, float plusC) | [source/game_sa/Radar.cpp:923](../../source/game_sa/Radar.cpp#L923) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RadarTraceStaticColour(const tRadarTrace& trace) | [source/game_sa/Radar.cpp:1784](../../source/game_sa/Radar.cpp#L1784) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo HudColour: source/game_sa/HudColours.cpp:23 (0x00BAB22C) |
| RadarGangPulse(float angle, int32 alphaInt) | [source/game_sa/Radar.cpp:1793](../../source/game_sa/Radar.cpp#L1793) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAirStripLocation(eAirstripLocation location) | [source/game_sa/Radar.cpp:2425](../../source/game_sa/Radar.cpp#L2425) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1354"></a>

## Globales y helpers: source/game_sa/Ragdoll/IKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| *& RawRefSlot(notsa::EntityRef&lt;T&gt;& ref) | [source/game_sa/Ragdoll/IKChain.cpp:56](../../source/game_sa/Ragdoll/IKChain.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1355"></a>

## Globales y helpers: source/game_sa/Ragdoll/IKChainManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetPedIKManagerTask(CPed* ped, bool createIfNotExists = false) | [source/game_sa/Ragdoll/IKChainManager.cpp:190](../../source/game_sa/Ragdoll/IKChainManager.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetPedIKLookAtTask(CPed* ped) | [source/game_sa/Ragdoll/IKChainManager.cpp:206](../../source/game_sa/Ragdoll/IKChainManager.cpp#L206) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1356"></a>

## Globales y helpers: source/game_sa/Remote.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & RemoteField(void* base, size_t offset) | [source/game_sa/Remote.cpp:35](../../source/game_sa/Remote.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1357"></a>

## Globales y helpers: source/game_sa/RenderWare/D3D9RasterDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComCall(void* self, Args... args) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:66](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EngineCall(Args... args) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:73](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RasterExt(RwRaster* raster, int32 offset) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:79](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ExtField(uint8* ext, size_t offset) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:83](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReleaseCom(void* object) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:86](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFormatInReleaseRange(int32 format) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:90](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveRasterFromList(RwRaster* raster) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:95](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RasterListFreeList: source/game_sa/RenderWare/D3D9RasterDriver.cpp:44 (0x00B4EA00); Posible dato fijo RasterListHead: source/game_sa/RenderWare/D3D9RasterDriver.cpp:43 (0x00B4E9FC) |
| PushFreePaletteIndex(uint32 index) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:118](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FreePaletteCapacity: source/game_sa/RenderWare/D3D9RasterDriver.cpp:47 (0x00B4E9EC); Posible dato fijo FreePaletteCount: source/game_sa/RenderWare/D3D9RasterDriver.cpp:46 (0x00B4E9E8); Posible dato fijo FreePaletteIndices: source/game_sa/RenderWare/D3D9RasterDriver.cpp:49 (0x00B4E9F4) |
| CheckDeviceFormat(uint32 adapterFormat, uint32 usage, uint32 resourceType, uint32 checkFormat) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:302](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L302) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AdapterIndex: source/game_sa/RenderWare/D3D9RasterDriver.cpp:52 (0x00C97C24); Posible dato fijo DeviceType: source/game_sa/RenderWare/D3D9RasterDriver.cpp:53 (0x008E2428); Posible dato fijo Direct3DObject: source/game_sa/RenderWare/D3D9RasterDriver.cpp:51 (0x00C97C20) |
| CheckDepthStencilMatch(uint32 adapterFormat, uint32 renderTargetFormat, uint32 depthFormat) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:306](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L306) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AdapterIndex: source/game_sa/RenderWare/D3D9RasterDriver.cpp:52 (0x00C97C24); Posible dato fijo DeviceType: source/game_sa/RenderWare/D3D9RasterDriver.cpp:53 (0x008E2428); Posible dato fijo Direct3DObject: source/game_sa/RenderWare/D3D9RasterDriver.cpp:51 (0x00C97C20) |
| CheckBumpFormat(uint32 usage, uint32 resourceType, uint32 format) | [source/game_sa/RenderWare/D3D9RasterDriver.cpp:357](../../source/game_sa/RenderWare/D3D9RasterDriver.cpp#L357) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AdapterFormat: source/game_sa/RenderWare/D3D9RasterDriver.cpp:54 (0x00C9BEF0); Posible dato fijo DeviceCapsFlags: source/game_sa/RenderWare/D3D9RasterDriver.cpp:56 (0x00C9BF0C) |

<a id="grupo-1358"></a>

## Globales y helpers: source/game_sa/RenderWare/D3D9RasterFormat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PixelFormatTable() | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:65](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B4E7E0 |
| * RwFormatTable() | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:68](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x0085C670 |
| * ExtOf(RwRaster* raster) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:72](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Device() | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:76](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AdapterDisplayDepth() | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:81](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ZBufferDepth() | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:82](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BackBufferFormat() | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:83](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TextureCapsLowByte() | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:84](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CheckTextureFormat(int32 fmt) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:87](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L87) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004CBDE0: RwD3D9RasterDriver::CheckTextureFormat; ruta efectiva sin resolver |
| CheckRenderTargetFormat(int32 fmt) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:88](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L88) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004CBE20: RwD3D9RasterDriver::CheckRenderTargetTextureFormat; ruta efectiva sin resolver |
| CheckDepthTextureFormat(int32 fmt) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:89](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L89) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004CBE80: RwD3D9RasterDriver::CheckDepthStencilTextureFormat; ruta efectiva sin resolver |
| CheckCubeRenderTargetFormat(int32 fmt) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:90](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L90) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004CBF20: RwD3D9RasterDriver::CheckRenderTargetCubeFormat; ruta efectiva sin resolver |
| CheckAutoGenMipFormat(int32 fmt) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:91](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L91) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004CBF80: RwD3D9RasterDriver::CheckTextureFormatUsage400; ruta efectiva sin resolver |
| CheckDepthSurfaceFormat(int32 fmt) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:92](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L92) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004CBD80: RwD3D9RasterDriver::CheckDepthStencilSurfaceFormat; ruta efectiva sin resolver |
| CubeOrTextureFormatSupported(int32 isCube, int32 fmt, uint32 rasterType) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:96](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindFormatNormalOrTexture(uint32 fmt) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:119](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindFormatCameraTexture(uint32 fmt) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:219](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CreateCameraTextureObject(RwRaster* raster, RasterExt* ext, int32 format, uint32 levels, uint32 renderTarget) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:283](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DxtPack(uint32 r, uint32 g, uint32 b, uint32 a) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:688](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L688) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DxtDecodePalette(const uint8* colorBlock, uint32 transparentColor) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:693](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L693) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DxtWriteColors(uint32* dst, int32 stridePixels, const uint8* colorBlock, const DxtBlockPalette& palette) | [source/game_sa/RenderWare/D3D9RasterFormat.cpp:722](../../source/game_sa/RenderWare/D3D9RasterFormat.cpp#L722) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1359"></a>

## Globales y helpers: source/game_sa/RenderWare/D3DIndexDataBuffer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetDesc(void* indexBuffer, IndexBufferDesc* desc) | [source/game_sa/RenderWare/D3DIndexDataBuffer.cpp:32](../../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1360"></a>

## Globales y helpers: source/game_sa/RenderWare/D3DResourceSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * D3DDevice() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:75](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C97C28 |
| ExeFree(void* memory) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:79](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L79) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008214BD: fuera del catálogo consultado; ruta efectiva sin resolver |
| * TextureBufferAt(int32 index) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:83](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * IndexDataBufferAt(int32 index) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:86](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreResource(void* array, int32 index, void* resource) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:89](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GrowableArrayResize(void* self, void* /*edx*/, uint32 newCapacity) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:125](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x0082119A |
