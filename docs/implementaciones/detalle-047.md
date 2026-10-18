<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-921"></a>

## ColHelpers::V2::Header

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsEmpty() const | [source/game_sa/Collision/ColHelpers.h:205](../../source/game_sa/Collision/ColHelpers.h#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasFaceGroups() const | [source/game_sa/Collision/ColHelpers.h:206](../../source/game_sa/Collision/ColHelpers.h#L206) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-922"></a>

## ColHelpers::V3::Header

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetNoOfShdwVerts(CCollisionData* cd) const | [source/game_sa/Collision/ColHelpers.h:224](../../source/game_sa/Collision/ColHelpers.h#L224) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasShadowMesh() const | [source/game_sa/Collision/ColHelpers.h:236](../../source/game_sa/Collision/ColHelpers.h#L236) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-923"></a>

## ColProc59b

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MultiplyMatrixWithVector(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/Collision.cpp:1359](../../source/game_sa/Collision/Collision.cpp#L1359) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Multiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/Collision.cpp:1368](../../source/game_sa/Collision/Collision.cpp#L1368) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyColPoint(CColPoint& dst, const CColPoint& src) | [source/game_sa/Collision/Collision.cpp:1377](../../source/game_sa/Collision/Collision.cpp#L1377) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PointInTriangle2D(uint8 orientation, const CVector& v0, const CVector& v1, const CVector& v2, const CVector& ip) | [source/game_sa/Collision/Collision.cpp:1711](../../source/game_sa/Collision/Collision.cpp#L1711) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsOutsideRange(float s, float v0, float v1, float v2) | [source/game_sa/Collision/Collision.cpp:1742](../../source/game_sa/Collision/Collision.cpp#L1742) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreTriangleHit(CColPoint& cp, const CVector& point, const CVector& normal, const CColTriangle& tri, CStoredCollPoly* collPoly, const CVector& v0, const CVector& v1, const CVector& v2) | [source/game_sa/Collision/Collision.cpp:1747](../../source/game_sa/Collision/Collision.cpp#L1747) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopySurfaceA(CColPoint& cp, const CColSurface& surf) | [source/game_sa/Collision/Collision.cpp:1767](../../source/game_sa/Collision/Collision.cpp#L1767) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopySurfaceB(CColPoint& cp, const CColSurface& surf) | [source/game_sa/Collision/Collision.cpp:1772](../../source/game_sa/Collision/Collision.cpp#L1772) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-924"></a>

## ColWorld59c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MulPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/Collision.cpp:2215](../../source/game_sa/Collision/Collision.cpp#L2215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MulVector(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/Collision.cpp:2224](../../source/game_sa/Collision/Collision.cpp#L2224) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityBoundSphere(CEntity* entity) | [source/game_sa/Collision/Collision.cpp:3767](../../source/game_sa/Collision/Collision.cpp#L3767) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-925"></a>

## Common

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsciiToGxtChar(const char* src, GxtChar* dst) | [source/game_sa/GxtChar.cpp:6](../../source/game_sa/GxtChar.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::AsciiToGxtChar 0x00718600 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoint(RwV3d& point, const CSimpleTransform& placement, const RwV3d& vecPos) | [source/game_sa/common.cpp:59](../../source/game_sa/common.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Common::TransformPoint 0x0054ECE0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoints(RwV3d* pointOut, int count, const CMatrix& transformMatrix, RwV3d* pointIn) | [source/game_sa/common.cpp:87](../../source/game_sa/common.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::TransformPoints 0x0054EEF0 | 0 / 0 | Posible dato fijo m_pWorkingMatrix1: source/game_sa/Game.h:22 (0x00B72920) |
| WriteRaster(RwRaster* raster, const char* filename) | [source/game_sa/common.cpp:151](../../source/game_sa/common.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::WriteRaster 0x005A4150 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcScreenCoors(const CVector& in, CVector& out) | [source/game_sa/common.cpp:197](../../source/game_sa/common.cpp#L197) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::CalcScreenCoors-VV 0x0071DAB0 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |
| LittleTest() | [source/game_sa/common.cpp:211](../../source/game_sa/common.cpp#L211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::LittleTest 0x00541330 | 0 / 0 | Posible dato fijo g_nNumIm3dDrawCalls: source/game_sa/common.h:70 (0x00B73708) |
| DoesInfiniteLineTouchScreen( //float oX, float oY, float dX, float dY CVector2D origin, CVector2D dir ) | [source/game_sa/common.cpp:237](../../source/game_sa/common.cpp#L237) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::DoesInfiniteLineTouchScreen 0x0071DB80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInsideLine( CVector2D origin, CVector2D dir, CVector2D pt, float radius ) | [source/game_sa/common.cpp:257](../../source/game_sa/common.cpp#L257) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::IsPointInsideLine 0x0071E050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-926"></a>

## CommonSize

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CommonSize::Init(int32 blockSize) | [source/game_sa/Memory/CommonSize.cpp:5](../../source/game_sa/Memory/CommonSize.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CommonSize::Malloc() | [source/game_sa/Memory/CommonSize.cpp:19](../../source/game_sa/Memory/CommonSize.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommonSize::Free(HeapFreeBlockDesc* desc) | [source/game_sa/Memory/CommonSize.cpp:30](../../source/game_sa/Memory/CommonSize.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommonSize::_DumpInfo() | [source/game_sa/Memory/CommonSize.cpp:36](../../source/game_sa/Memory/CommonSize.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-927"></a>

## CompressedBox

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressedBox::DrawWireFrame(CRGBA color, const CMatrix& transform) const | [source/game_sa/CompressedBox.cpp:5](../../source/game_sa/CompressedBox.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-928"></a>

## CustomEnvMapPipeMaterialData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FromBuffer(const EnvMapPipeMaterialDataBuffer& b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:33](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToBuffer() const | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:42](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-929"></a>

## CustomSpecMapPipeMaterialData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FromBuffer(const SpecMapPipeMaterialDataBuffer& b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:70](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToBuffer() const | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:77](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-930"></a>

## D3DResourceSystem

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| D3DResourceSystem::InjectHooks() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:28](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| D3DResourceSystem::CancelBuffering() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:37](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L37) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730900: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::GetTotalIndexDataSize() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:42](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L42) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x007307F0: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::GetTotalPixelsSize() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:47](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L47) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730660: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::Init() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:52](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L52) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730830: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::SetUseD3DResourceBuffering(bool enabled) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:58](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: D3DResourceSystem::SetUseD3DResourceBuffering 0x00730AC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Conversión de dirección fija: 0x008D6084 |
| D3DResourceSystem::Shutdown() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:63](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L63) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730A00: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::TidyUpD3DIndexBuffers(uint32 count) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:88](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: D3DResourceSystem::TidyUpD3DIndexBuffers 0x00730740 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| D3DResourceSystem::TidyUpD3DTextures(uint32 count) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:143](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: D3DResourceSystem::TidyUpD3DTextures 0x007305E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| D3DResourceSystem::CreateIndexBuffer(uint32 numIndices, uint32 format, void** ppIndexBuffer) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:180](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L180) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x007306A0: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::CreateTexture(int32 width, int32 height, uint32 format, void** ppTexture) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:185](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L185) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730510: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::DestroyIndexBuffer(void* pIndexBuffer) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:190](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L190) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730D30: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::DestroyTexture(void* texture) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:195](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L195) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730B70: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-931"></a>

## EmissionInfo_t

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Process(float mult) | [source/game_sa/Fx/EmissionInfo.h:31](../../source/game_sa/Fx/EmissionInfo.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-932"></a>

## FlcX87

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FlcX87::FlcX87() | [source/game_sa/X87Math.cpp:11887](../../source/game_sa/X87Math.cpp#L11887) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Push(const FlcExt& v) | [source/game_sa/X87Math.cpp:11891](../../source/game_sa/X87Math.cpp#L11891) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Write(int i, const FlcExt& v) | [source/game_sa/X87Math.cpp:11898](../../source/game_sa/X87Math.cpp#L11898) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::MemOperand(const float& m) | [source/game_sa/X87Math.cpp:11916](../../source/game_sa/X87Math.cpp#L11916) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::MemOperandD(const double& m) | [source/game_sa/X87Math.cpp:11917](../../source/game_sa/X87Math.cpp#L11917) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fld(const float& m) | [source/game_sa/X87Math.cpp:11919](../../source/game_sa/X87Math.cpp#L11919) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FldRet(const float& m) | [source/game_sa/X87Math.cpp:11920](../../source/game_sa/X87Math.cpp#L11920) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FldD(const double& m) | [source/game_sa/X87Math.cpp:11921](../../source/game_sa/X87Math.cpp#L11921) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FldI(int value) | [source/game_sa/X87Math.cpp:11922](../../source/game_sa/X87Math.cpp#L11922) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FldSt(int i) | [source/game_sa/X87Math.cpp:11923](../../source/game_sa/X87Math.cpp#L11923) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FldZero() | [source/game_sa/X87Math.cpp:11924](../../source/game_sa/X87Math.cpp#L11924) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FldExt(const FlcExt& v) | [source/game_sa/X87Math.cpp:11925](../../source/game_sa/X87Math.cpp#L11925) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fst(float& m) | [source/game_sa/X87Math.cpp:11926](../../source/game_sa/X87Math.cpp#L11926) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fstp(float& m) | [source/game_sa/X87Math.cpp:11933](../../source/game_sa/X87Math.cpp#L11933) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FstpSt(int i) | [source/game_sa/X87Math.cpp:11941](../../source/game_sa/X87Math.cpp#L11941) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FstSt(int i) | [source/game_sa/X87Math.cpp:11947](../../source/game_sa/X87Math.cpp#L11947) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Pop() | [source/game_sa/X87Math.cpp:11948](../../source/game_sa/X87Math.cpp#L11948) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fxch(int i) | [source/game_sa/X87Math.cpp:11949](../../source/game_sa/X87Math.cpp#L11949) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fchs() | [source/game_sa/X87Math.cpp:11950](../../source/game_sa/X87Math.cpp#L11950) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fabs() | [source/game_sa/X87Math.cpp:11951](../../source/game_sa/X87Math.cpp#L11951) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::ArithReg(char op, FlcExt a, FlcExt b, int dest, bool pop, bool) | [source/game_sa/X87Math.cpp:11959](../../source/game_sa/X87Math.cpp#L11959) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Arith(char op, int dest, int srcA, int srcB, bool pop) | [source/game_sa/X87Math.cpp:11978](../../source/game_sa/X87Math.cpp#L11978) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fadd(const float& m) | [source/game_sa/X87Math.cpp:11980](../../source/game_sa/X87Math.cpp#L11980) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fsub(const float& m) | [source/game_sa/X87Math.cpp:11981](../../source/game_sa/X87Math.cpp#L11981) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fsubr(const float& m) | [source/game_sa/X87Math.cpp:11982](../../source/game_sa/X87Math.cpp#L11982) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fmul(const float& m) | [source/game_sa/X87Math.cpp:11983](../../source/game_sa/X87Math.cpp#L11983) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fdiv(const float& m) | [source/game_sa/X87Math.cpp:11984](../../source/game_sa/X87Math.cpp#L11984) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fdivr(const float& m) | [source/game_sa/X87Math.cpp:11985](../../source/game_sa/X87Math.cpp#L11985) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FaddD(const double& m) | [source/game_sa/X87Math.cpp:11986](../../source/game_sa/X87Math.cpp#L11986) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FsubD(const double& m) | [source/game_sa/X87Math.cpp:11987](../../source/game_sa/X87Math.cpp#L11987) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FsubrD(const double& m) | [source/game_sa/X87Math.cpp:11988](../../source/game_sa/X87Math.cpp#L11988) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FmulD(const double& m) | [source/game_sa/X87Math.cpp:11989](../../source/game_sa/X87Math.cpp#L11989) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FdivD(const double& m) | [source/game_sa/X87Math.cpp:11990](../../source/game_sa/X87Math.cpp#L11990) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FdivrD(const double& m) | [source/game_sa/X87Math.cpp:11991](../../source/game_sa/X87Math.cpp#L11991) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FimulI(int v) | [source/game_sa/X87Math.cpp:11992](../../source/game_sa/X87Math.cpp#L11992) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FidivI(int v) | [source/game_sa/X87Math.cpp:11993](../../source/game_sa/X87Math.cpp#L11993) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FiaddI(int v) | [source/game_sa/X87Math.cpp:11994](../../source/game_sa/X87Math.cpp#L11994) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FcomMem(const float& m, bool pop) | [source/game_sa/X87Math.cpp:11996](../../source/game_sa/X87Math.cpp#L11996) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FcomSt(int i, bool pop) | [source/game_sa/X87Math.cpp:12006](../../source/game_sa/X87Math.cpp#L12006) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fcompp() | [source/game_sa/X87Math.cpp:12013](../../source/game_sa/X87Math.cpp#L12013) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fsqrt() | [source/game_sa/X87Math.cpp:12020](../../source/game_sa/X87Math.cpp#L12020) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Ah() const | [source/game_sa/X87Math.cpp:12027](../../source/game_sa/X87Math.cpp#L12027) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::StFloat(int i) | [source/game_sa/X87Math.cpp:12028](../../source/game_sa/X87Math.cpp#L12028) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FldOne() | [source/game_sa/X87Math.cpp:12044](../../source/game_sa/X87Math.cpp#L12044) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FldI64(long long v) | [source/game_sa/X87Math.cpp:12045](../../source/game_sa/X87Math.cpp#L12045) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FstD(double& m) | [source/game_sa/X87Math.cpp:12046](../../source/game_sa/X87Math.cpp#L12046) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::FstpD(double& m) | [source/game_sa/X87Math.cpp:12053](../../source/game_sa/X87Math.cpp#L12053) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fistp64(long long& m) | [source/game_sa/X87Math.cpp:12054](../../source/game_sa/X87Math.cpp#L12054) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fsin() | [source/game_sa/X87Math.cpp:12088](../../source/game_sa/X87Math.cpp#L12088) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fcos() | [source/game_sa/X87Math.cpp:12100](../../source/game_sa/X87Math.cpp#L12100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Fpatan() | [source/game_sa/X87Math.cpp:12112](../../source/game_sa/X87Math.cpp#L12112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcX87::Commit() | [source/game_sa/X87Math.cpp:12467](../../source/game_sa/X87Math.cpp#L12467) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & St(int i) const | [source/game_sa/X87Math.h:1438](../../source/game_sa/X87Math.h#L1438) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Top() | [source/game_sa/X87Math.h:1445](../../source/game_sa/X87Math.h#L1445) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & S(int i) | [source/game_sa/X87Math.h:1446](../../source/game_sa/X87Math.h#L1446) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-933"></a>

## FurnitureGroup_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FurnitureGroup_c::InjectHooks() | [source/game_sa/Interior/FurnitureGroup_c.cpp:26](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::Init() | [source/game_sa/Interior/FurnitureGroup_c.cpp:39](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::Init 0x005910A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::Exit() | [source/game_sa/Interior/FurnitureGroup_c.cpp:45](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::Exit 0x005910B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::AddSubGroup(int32 subGroupId, int32 minWidth, int32 minDepth, int32 maxWidth, int32 maxDepth, uint8 canPlaceInFrontOfWindow, bool isTall, bool canSteal) | [source/game_sa/Interior/FurnitureGroup_c.cpp:57](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::AddSubGroup 0x005910E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureGroup_c::GetFurniture(int32 subGroupId, int16 furnitureId, uint8 wealth) | [source/game_sa/Interior/FurnitureGroup_c.cpp:77](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::GetFurniture 0x00591130 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::GetRandomId(int32 subGroupId, uint8 rating) | [source/game_sa/Interior/FurnitureGroup_c.cpp:85](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::GetRandomId 0x00591170 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::AddFurniture(int32 subGroupId, uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) | [source/game_sa/Interior/FurnitureGroup_c.cpp:93](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::AddFurniture 0x005C0230 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureGroup_c::GetSubGroup(int32 subGroupId) | [source/game_sa/Interior/FurnitureGroup_c.cpp:105](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-934"></a>

## FurnitureManager_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FurnitureManager_c::InjectHooks() | [source/game_sa/Interior/FurnitureManager_c.cpp:26](../../source/game_sa/Interior/FurnitureManager_c.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureManager_c::Init() | [source/game_sa/Interior/FurnitureManager_c.cpp:44](../../source/game_sa/Interior/FurnitureManager_c.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::Init 0x005C04A0 | 0 / 0 | Posible dato fijo g_currFurnitureId: source/game_sa/Interior/FurnitureManager_c.h:20 (0x00BAB378); Posible dato fijo g_currSubGroupId: source/game_sa/Interior/FurnitureManager_c.h:19 (0x00BAB37C) |
| FurnitureManager_c::Exit() | [source/game_sa/Interior/FurnitureManager_c.cpp:61](../../source/game_sa/Interior/FurnitureManager_c.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::Exit 0x005911A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureManager_c::NewSubGroup() | [source/game_sa/Interior/FurnitureManager_c.cpp:68](../../source/game_sa/Interior/FurnitureManager_c.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_currSubGroupId: source/game_sa/Interior/FurnitureManager_c.h:19 (0x00BAB37C); Posible dato fijo g_subGroupStore: source/game_sa/Interior/FurnitureManager_c.h:21 (0x00BAD3F8) |
| FurnitureManager_c::LoadFurniture() | [source/game_sa/Interior/FurnitureManager_c.cpp:82](../../source/game_sa/Interior/FurnitureManager_c.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::LoadFurniture 0x005C0280 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureManager_c::GetGroupId(const char* name) | [source/game_sa/Interior/FurnitureManager_c.cpp:132](../../source/game_sa/Interior/FurnitureManager_c.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::GetGroupId 0x005BFB40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureManager_c::GetSubGroupId(const char* name) | [source/game_sa/Interior/FurnitureManager_c.cpp:145](../../source/game_sa/Interior/FurnitureManager_c.cpp#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::GetSubGroupId 0x005BFBF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureManager_c::GetFurniture(int32 groupId, int32 subGroupId, int16 id, uint8 wealth) | [source/game_sa/Interior/FurnitureManager_c.cpp:214](../../source/game_sa/Interior/FurnitureManager_c.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::GetFurniture 0x005911E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureManager_c::GetRandomId(int32 groupId, int32 subGroupId, uint8 wealth) | [source/game_sa/Interior/FurnitureManager_c.cpp:220](../../source/game_sa/Interior/FurnitureManager_c.cpp#L220) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::GetRandomId 0x00591220 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-935"></a>

## FurnitureSubGroup_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FurnitureSubGroup_c::InjectHooks() | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:26](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:61](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::AddFurniture 0x005C00C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureSubGroup_c::GetFurniture(int16 id, uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:92](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::GetFurniture 0x00590EE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::GetRandomId(uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:103](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::GetRandomId 0x00590FD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::Exit() | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:108](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-936"></a>

## Fx

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrawLight(CVector pos, CVector color) | [source/game_sa/Scripts/Commands/ObjectCommands.cpp:138](../../source/game_sa/Scripts/Commands/ObjectCommands.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddBigGunFlash(CVector origin, CVector target) | [source/game_sa/Scripts/Commands/ObjectCommands.cpp:143](../../source/game_sa/Scripts/Commands/ObjectCommands.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |

<a id="grupo-937"></a>

## FxBox_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxBox_c() | [source/game_sa/Fx/FxBox.h:14](../../source/game_sa/Fx/FxBox.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Reset() | [source/game_sa/Fx/FxBox.h:18](../../source/game_sa/Fx/FxBox.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator=(const RwMatrix& right) | [source/game_sa/Fx/FxBox.h:25](../../source/game_sa/Fx/FxBox.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-938"></a>

## FxBufferedMatrix

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyFromRwMatrix(const RwMatrix& mat) | [source/game_sa/Fx/FxPrimBP.h:33](../../source/game_sa/Fx/FxPrimBP.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyFromVectors(const CVector& r, const CVector& u, const CVector& a, const CVector& p) | [source/game_sa/Fx/FxPrimBP.h:40](../../source/game_sa/Fx/FxPrimBP.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyToRwMatrix(RwMatrix& out) | [source/game_sa/Fx/FxPrimBP.h:44](../../source/game_sa/Fx/FxPrimBP.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-939"></a>

## FxEmitterBP_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitterBP_c::InjectHooks() | [source/game_sa/Fx/FxEmitterBP.cpp:36](../../source/game_sa/Fx/FxEmitterBP.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::FxEmitterBP_c() | [source/game_sa/Fx/FxEmitterBP.cpp:52](../../source/game_sa/Fx/FxEmitterBP.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Constructor 0x004A18D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::RenderHeatHaze(RwCamera* camera, uint32 txdHashKey, float brightness) | [source/game_sa/Fx/FxEmitterBP.cpp:196](../../source/game_sa/Fx/FxEmitterBP.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::RenderHeatHaze 0x004A1940 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::UpdateParticle(float deltaTime, FxEmitterPrt_c* emitter) | [source/game_sa/Fx/FxEmitterBP.cpp:202](../../source/game_sa/Fx/FxEmitterBP.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::UpdateParticle 0x004A21D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxEmitterBP_c::CreateInstance() | [source/game_sa/Fx/FxEmitterBP.cpp:248](../../source/game_sa/Fx/FxEmitterBP.cpp#L248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::CreateInstance 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::Update(float deltaTime) | [source/game_sa/Fx/FxEmitterBP.cpp:256](../../source/game_sa/Fx/FxEmitterBP.cpp#L256) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Update 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitterBP_c::Load(FILESTREAM file, int32 version, FxName32_t* textureNames) | [source/game_sa/Fx/FxEmitterBP.cpp:277](../../source/game_sa/Fx/FxEmitterBP.cpp#L277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Load 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::LoadTextures(FxName32_t* textureNames, int32 version) | [source/game_sa/Fx/FxEmitterBP.cpp:295](../../source/game_sa/Fx/FxEmitterBP.cpp#L295) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::LoadTextures 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::Render(RwCamera* camera, uint32 txdHashKey, float brightness, bool doHeatHaze) | [source/game_sa/Fx/FxEmitterBP.cpp:327](../../source/game_sa/Fx/FxEmitterBP.cpp#L327) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Render 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitterBP_c::FreePrtFromPrim(FxSystem_c* system) | [source/game_sa/Fx/FxEmitterBP.cpp:344](../../source/game_sa/Fx/FxEmitterBP.cpp#L344) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::FreePrtFromPrim 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitterBP_c::IsFxInfoPresent(eFxInfoType type) const | [source/game_sa/Fx/FxEmitterBP.cpp:357](../../source/game_sa/Fx/FxEmitterBP.cpp#L357) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/FxEmitterBP.h:26](../../source/game_sa/Fx/FxEmitterBP.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Constructor 0x004A18D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-940"></a>

## FxEmitterPrt_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * FxEmitterPrt_c::operator new\[\](size_t size) | [source/game_sa/Fx/FxEmitterPrt.cpp:26](../../source/game_sa/Fx/FxEmitterPrt.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterPrt_c::operator new\[\] 0x004A18B0 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitterPrt_c::InjectHooks() | [source/game_sa/Fx/FxEmitterPrt.cpp:33](../../source/game_sa/Fx/FxEmitterPrt.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
