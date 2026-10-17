<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1401"></a>

## Globales y helpers: source/game_sa/StencilShadows.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StencilMulPoint(const CMatrix& m, const CVector& v) | [source/game_sa/StencilShadows.cpp:47](../../source/game_sa/StencilShadows.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilTransformFromObjectSpace(const CEntity* entity, const CVector& offset) | [source/game_sa/StencilShadows.cpp:57](../../source/game_sa/StencilShadows.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0054ECE0: Common::TransformPoint; ruta efectiva sin resolver |
| StencilSignedDistSq(const CVector& cam, CEntity* entity) | [source/game_sa/StencilShadows.cpp:68](../../source/game_sa/StencilShadows.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * StencilFindForOwner(const CEntity* owner) | [source/game_sa/StencilShadows.cpp:82](../../source/game_sa/StencilShadows.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo pFirstActiveStencilShadowObject: source/game_sa/StencilShadows.h:10 (0x00C6A16C) |
| StencilObjectInvalid(CEntity* owner) | [source/game_sa/StencilShadows.cpp:92](../../source/game_sa/StencilShadows.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilToggleEdge(RxVertexIndex* edges, uint16& count, uint16 i0, uint16 i1) | [source/game_sa/StencilShadows.cpp:101](../../source/game_sa/StencilShadows.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilTransformPoints(CVector* out, int32 count, const CMatrix* matrix, const CVector* in) | [source/game_sa/StencilShadows.cpp:121](../../source/game_sa/StencilShadows.cpp#L121) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0054EEF0: Common::TransformPoints; ruta efectiva sin resolver |
| BuildShadowVolume(CStencilShadowObject* obj, bool forVehicle) | [source/game_sa/StencilShadows.cpp:128](../../source/game_sa/StencilShadows.cpp#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo s_ShadowTrianglePoints: source/game_sa/StencilShadows.cpp:28 (0x00C6A174); Posible dato fijo s_ShadowTrianglePointsUnk: source/game_sa/StencilShadows.cpp:27 (0x00C6A170); Posible dato fijo s_SunPosNrm: source/game_sa/StencilShadows.cpp:30 (0x008D5244); Posible dato fijo s_TransformedShadowTrianglePoints: source/game_sa/StencilShadows.cpp:29 (0x00C6A178) |

<a id="grupo-1402"></a>

## Globales y helpers: source/game_sa/Streaming.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeSector(float v) | [source/game_sa/Streaming.cpp:39](../../source/game_sa/Streaming.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLodSector(float v) | [source/game_sa/Streaming.cpp:41](../../source/game_sa/Streaming.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MemoryUsedBelow(size_t goal) | [source/game_sa/Streaming.cpp:43](../../source/game_sa/Streaming.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_memoryUsedBytes: source/game_sa/Streaming.h:245 (0x008E4CB4) |

<a id="grupo-1403"></a>

## Globales y helpers: source/game_sa/Streaming.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelDFF(int32 model) | [source/game_sa/Streaming.h:81](../../source/game_sa/Streaming.h#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelTXD(int32 model) | [source/game_sa/Streaming.h:82](../../source/game_sa/Streaming.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelCOL(int32 model) | [source/game_sa/Streaming.h:83](../../source/game_sa/Streaming.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelIPL(int32 model) | [source/game_sa/Streaming.h:84](../../source/game_sa/Streaming.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelDAT(int32 model) | [source/game_sa/Streaming.h:85](../../source/game_sa/Streaming.h#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelIFP(int32 model) | [source/game_sa/Streaming.h:86](../../source/game_sa/Streaming.h#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelRRR(int32 model) | [source/game_sa/Streaming.h:87](../../source/game_sa/Streaming.h#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelSCM(int32 model) | [source/game_sa/Streaming.h:88](../../source/game_sa/Streaming.h#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetModelType(int32 model) | [source/game_sa/Streaming.h:90](../../source/game_sa/Streaming.h#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DFFToModelId(int32 relativeId) | [source/game_sa/Streaming.h:122](../../source/game_sa/Streaming.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TXDToModelId(int32 relativeId) | [source/game_sa/Streaming.h:123](../../source/game_sa/Streaming.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| COLToModelId(int32 relativeId) | [source/game_sa/Streaming.h:124](../../source/game_sa/Streaming.h#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IPLToModelId(int32 relativeId) | [source/game_sa/Streaming.h:125](../../source/game_sa/Streaming.h#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DATToModelId(size_t relativeId) | [source/game_sa/Streaming.h:126](../../source/game_sa/Streaming.h#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IFPToModelId(int32 relativeId) | [source/game_sa/Streaming.h:127](../../source/game_sa/Streaming.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RRRToModelId(int32 relativeId) | [source/game_sa/Streaming.h:128](../../source/game_sa/Streaming.h#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCMToModelId(int32 relativeId) | [source/game_sa/Streaming.h:129](../../source/game_sa/Streaming.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToDFF(int32 absId) | [source/game_sa/Streaming.h:132](../../source/game_sa/Streaming.h#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToTXD(int32 absId) | [source/game_sa/Streaming.h:133](../../source/game_sa/Streaming.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToCOL(int32 absId) | [source/game_sa/Streaming.h:134](../../source/game_sa/Streaming.h#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToIPL(int32 absId) | [source/game_sa/Streaming.h:135](../../source/game_sa/Streaming.h#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToDAT(int32 absId) | [source/game_sa/Streaming.h:136](../../source/game_sa/Streaming.h#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToIFP(int32 absId) | [source/game_sa/Streaming.h:137](../../source/game_sa/Streaming.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToRRR(int32 absId) | [source/game_sa/Streaming.h:138](../../source/game_sa/Streaming.h#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToSCM(int32 absId) | [source/game_sa/Streaming.h:139](../../source/game_sa/Streaming.h#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1404"></a>

## Globales y helpers: source/game_sa/StuckCarCheck.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetVehicleNoBoundsCheck(int32 handle) | [source/game_sa/StuckCarCheck.cpp:25](../../source/game_sa/StuckCarCheck.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1405"></a>

## Globales y helpers: source/game_sa/StuntJumpManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RawPool() | [source/game_sa/StuntJumpManager.cpp:52](../../source/game_sa/StuntJumpManager.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo mp_poolStuntJumps: source/game_sa/StuntJumpManager.h:28 (0x00A9A888) |
| * StuntJumpPoolNew(StuntJumpPoolRaw* pool) | [source/game_sa/StuntJumpManager.cpp:57](../../source/game_sa/StuntJumpManager.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpTimeStepInMS() | [source/game_sa/StuntJumpManager.cpp:79](../../source/game_sa/StuntJumpManager.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| SetJumpState(eJumpState state) | [source/game_sa/StuntJumpManager.cpp:84](../../source/game_sa/StuntJumpManager.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_jumpState: source/game_sa/StuntJumpManager.h:33 (0x00A9A898) |
| ResetAllJumps() | [source/game_sa/StuntJumpManager.cpp:351](../../source/game_sa/StuntJumpManager.cpp#L351) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpTestCode() | [source/game_sa/StuntJumpManager.cpp:359](../../source/game_sa/StuntJumpManager.cpp#L359) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1406"></a>

## Globales y helpers: source/game_sa/TagManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsTagInArea(const CRect& area, const tTagDesc& tag) | [source/game_sa/TagManager.cpp:121](../../source/game_sa/TagManager.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1407"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocator | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h:10](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1408"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorChat | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h:33](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h:41](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1409"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorFollowAnyMeans | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h:31](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h#L31) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h:39](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1410"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorFollowLimited | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h:28](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h:36](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1411"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorRandom | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h:30](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h#L30) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h:57](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1412"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h:31](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h#L31) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h:39](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1413"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorStandStill | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h:28](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h:36](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1414"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocator.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskAllocator | [source/game_sa/Tasks/Allocators/TaskAllocator.h:22](../../source/game_sa/Tasks/Allocators/TaskAllocator.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ProcessGroup(CPedGroupIntelligence* intel) | [source/game_sa/Tasks/Allocators/TaskAllocator.h:39](../../source/game_sa/Tasks/Allocators/TaskAllocator.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Abort() | [source/game_sa/Tasks/Allocators/TaskAllocator.h:41](../../source/game_sa/Tasks/Allocators/TaskAllocator.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| classof(const From* f) | [source/game_sa/Tasks/Allocators/TaskAllocator.h:45](../../source/game_sa/Tasks/Allocators/TaskAllocator.h#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1415"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorAttack.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskAllocatorAttack | [source/game_sa/Tasks/Allocators/TaskAllocatorAttack.h:6](../../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() override | [source/game_sa/Tasks/Allocators/TaskAllocatorAttack.h:15](../../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1416"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskAllocatorKillOnFoot | [source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.h:5](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() override | [source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.h:15](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1417"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestThreatDistance(const CVector& a, const CVector& b) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp:92](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1418"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskAllocatorKillThreatsBasic | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.h:5](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() override | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.h:14](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1419"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskAllocatorKillThreatsBasicRandomGroup | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h:5](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() override | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h:14](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1420"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskAllocatorKillThreatsDriveby | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h:7](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() override | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h:16](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
