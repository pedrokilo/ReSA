<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 08/11/2026 (Ciudad de México):** 866 grupos / 9366 funciones reconstruidas; 9366 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-921"></a>

## CZoneInfo

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetSumOfGangDensity() const | [source/game_sa/ZoneInfo.h:53](../../source/game_sa/ZoneInfo.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-922"></a>

## C_PcSave

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| C_PcSave::InjectHooks() | [source/game_sa/C_PcSave.cpp:31](../../source/game_sa/C_PcSave.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| C_PcSave::SetSaveDirectory(const char* path) | [source/game_sa/C_PcSave.cpp:44](../../source/game_sa/C_PcSave.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: C_PcSave::SetSaveDirectory 0x00619040 | 0 / 0 | Posible dato fijo DefaultPCSaveFileName: source/game_sa/C_PcSave.h:27 (0x00C16F18) |
| C_PcSave::GenerateGameFilename(int32 slot, char* out) | [source/game_sa/C_PcSave.cpp:49](../../source/game_sa/C_PcSave.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: C_PcSave::GenerateGameFilename 0x006190A0 | 0 / 0 | Posible dato fijo DefaultPCSaveFileName: source/game_sa/C_PcSave.h:27 (0x00C16F18) |
| * GxtCharToSafeAscii(const GxtChar* src) | [source/game_sa/C_PcSave.cpp:57](../../source/game_sa/C_PcSave.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: C_PcSave::GxtCharToSafeAscii 0x0069F8C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| C_PcSave::PopulateSlotInfo() | [source/game_sa/C_PcSave.cpp:71](../../source/game_sa/C_PcSave.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: C_PcSave::PopulateSlotInfo 0x00619140 | 0 / 0 | Posible dato fijo DefaultPCSaveFileName: source/game_sa/C_PcSave.h:27 (0x00C16F18); Posible dato fijo TheText: source/game_sa/Text/Text.h:88 (0x00C1B340); Posible dato fijo ms_SlotFileName: source/game_sa/GenericGameStorage.h:73 (0x00C16368); Posible dato fijo ms_SlotSaveDate: source/game_sa/GenericGameStorage.h:72 (0x00C16138); Posible dato fijo ms_Slots: source/game_sa/GenericGameStorage.h:74 (0x00C16EBC); Posible dato fijo s_PcSaveHelper: source/game_sa/C_PcSave.h:42 (0x00C17034) |
| C_PcSave::SaveSlot(int32 slot) | [source/game_sa/C_PcSave.cpp:123](../../source/game_sa/C_PcSave.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: C_PcSave::SaveSlot 0x00619060 | 0 / 0 | Posible dato fijo s_PcSaveHelper: source/game_sa/C_PcSave.h:42 (0x00C17034) |
| C_PcSave::DeleteSlot(int32 slot) | [source/game_sa/C_PcSave.cpp:133](../../source/game_sa/C_PcSave.cpp#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: C_PcSave::DeleteSlot 0x006190D0 | 0 / 0 | Posible dato fijo DefaultPCSaveFileName: source/game_sa/C_PcSave.h:27 (0x00C16F18); Posible dato fijo s_PcSaveHelper: source/game_sa/C_PcSave.h:42 (0x00C17034) |

<a id="grupo-923"></a>

## Car75Word

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Car75Word(uint32_t bits) | [source/game_sa/X87Math.cpp:16469](../../source/game_sa/X87Math.cpp#L16469) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & f() const | [source/game_sa/X87Math.cpp:16470](../../source/game_sa/X87Math.cpp#L16470) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-924"></a>

## CdStream

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CdStreamOpen(const char* lpFileName) | [source/game_sa/CdStreamInfo.cpp:53](../../source/game_sa/CdStreamInfo.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamOpen 0x004067B0 | 0 / 0 | Posible dato fijo gCdImageNames: source/game_sa/CdStreamInfo.cpp:22 (0x008E4098); Posible dato fijo gStreamFileCreateFlags: source/game_sa/CdStreamInfo.cpp:23 (0x008E3FE0); Posible dato fijo gStreamFileHandles: source/game_sa/CdStreamInfo.cpp:21 (0x008E4010) |
| CdStreamSync(int32 streamId) | [source/game_sa/CdStreamInfo.cpp:93](../../source/game_sa/CdStreamInfo.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamSync 0x00406460 | 0 / 0 | Posible dato fijo gCdStreams: source/game_sa/CdStreamInfo.cpp:24 (0x008E3FFC); Posible dato fijo gOverlappedIO: source/game_sa/CdStreamInfo.cpp:28 (0x008E3FE8); Posible dato fijo gStreamingInitialized: source/game_sa/CdStreamInfo.cpp:27 (0x008E3FE4) |
| CdStreamGetStatus(int32 streamId) | [source/game_sa/CdStreamInfo.cpp:111](../../source/game_sa/CdStreamInfo.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamGetStatus 0x004063E0 | 0 / 0 | Posible dato fijo gCdStreams: source/game_sa/CdStreamInfo.cpp:24 (0x008E3FFC); Posible dato fijo gOverlappedIO: source/game_sa/CdStreamInfo.cpp:28 (0x008E3FE8); Posible dato fijo gStreamingInitialized: source/game_sa/CdStreamInfo.cpp:27 (0x008E3FE4) |
| CdStreamRead(int32 streamId, void* lpBuffer, CdStreamPos pos, int32 sectorCount) | [source/game_sa/CdStreamInfo.cpp:139](../../source/game_sa/CdStreamInfo.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamRead 0x00406A20 | 0 / 0 | Posible dato fijo gCdStreams: source/game_sa/CdStreamInfo.cpp:24 (0x008E3FFC); Posible dato fijo gLastCdStreamPosn: source/game_sa/CdStreamInfo.cpp:33 (0x008E4898); Posible dato fijo gOverlappedIO: source/game_sa/CdStreamInfo.cpp:28 (0x008E3FE8); Posible dato fijo gStreamFileHandles: source/game_sa/CdStreamInfo.cpp:21 (0x008E4010); Posible dato fijo gStreamQueue: source/game_sa/CdStreamInfo.cpp:29 (0x008E3FEC); Posible dato fijo gStreamSemaphore: source/game_sa/CdStreamInfo.cpp:30 (0x008E4004); Posible dato fijo gStreamingInitialized: source/game_sa/CdStreamInfo.cpp:27 (0x008E3FE4) |
| CdStreamThread(LPVOID lpParam) | [source/game_sa/CdStreamInfo.cpp:172](../../source/game_sa/CdStreamInfo.cpp#L172) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamThread 0x00406560 | 0 / 0 | Posible dato fijo gCdStreams: source/game_sa/CdStreamInfo.cpp:24 (0x008E3FFC); Posible dato fijo gOverlappedIO: source/game_sa/CdStreamInfo.cpp:28 (0x008E3FE8); Posible dato fijo gStreamQueue: source/game_sa/CdStreamInfo.cpp:29 (0x008E3FEC); Posible dato fijo gStreamSemaphore: source/game_sa/CdStreamInfo.cpp:30 (0x008E4004) |
| CdStreamInitThread() | [source/game_sa/CdStreamInfo.cpp:222](../../source/game_sa/CdStreamInfo.cpp#L222) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamInitThread 0x004068F0 | 0 / 0 | Posible dato fijo gCdStreams: source/game_sa/CdStreamInfo.cpp:24 (0x008E3FFC); Posible dato fijo gStreamCount: source/game_sa/CdStreamInfo.cpp:25 (0x008E4090); Posible dato fijo gStreamQueue: source/game_sa/CdStreamInfo.cpp:29 (0x008E3FEC); Posible dato fijo gStreamSemaphore: source/game_sa/CdStreamInfo.cpp:30 (0x008E4004); Posible dato fijo gStreamingThread: source/game_sa/CdStreamInfo.cpp:31 (0x008E4008); Posible dato fijo gStreamingThreadId: source/game_sa/CdStreamInfo.cpp:32 (0x008E4000) |
| CdStreamInit(int32 streamCount) | [source/game_sa/CdStreamInfo.cpp:248](../../source/game_sa/CdStreamInfo.cpp#L248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamInit 0x00406B70 | 0 / 0 | Posible dato fijo gCdImageNames: source/game_sa/CdStreamInfo.cpp:22 (0x008E4098); Posible dato fijo gCdStreams: source/game_sa/CdStreamInfo.cpp:24 (0x008E3FFC); Posible dato fijo gOpenStreamCount: source/game_sa/CdStreamInfo.cpp:26 (0x008E4094); Posible dato fijo gOverlappedIO: source/game_sa/CdStreamInfo.cpp:28 (0x008E3FE8); Posible dato fijo gStreamCount: source/game_sa/CdStreamInfo.cpp:25 (0x008E4090); Posible dato fijo gStreamFileCreateFlags: source/game_sa/CdStreamInfo.cpp:23 (0x008E3FE0); Posible dato fijo gStreamFileHandles: source/game_sa/CdStreamInfo.cpp:21 (0x008E4010); Posible dato fijo gStreamingInitialized: source/game_sa/CdStreamInfo.cpp:27 (0x008E3FE4) |
| CdStreamRemoveImages() | [source/game_sa/CdStreamInfo.cpp:285](../../source/game_sa/CdStreamInfo.cpp#L285) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamRemoveImages 0x00406690 | 0 / 0 | Posible dato fijo gCdImageNames: source/game_sa/CdStreamInfo.cpp:22 (0x008E4098); Posible dato fijo gOpenStreamCount: source/game_sa/CdStreamInfo.cpp:26 (0x008E4094); Posible dato fijo gStreamCount: source/game_sa/CdStreamInfo.cpp:25 (0x008E4090); Posible dato fijo gStreamFileHandles: source/game_sa/CdStreamInfo.cpp:21 (0x008E4010) |
| CdStreamShutdown() | [source/game_sa/CdStreamInfo.cpp:301](../../source/game_sa/CdStreamInfo.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CdStream::CdStreamShutdown 0x00406370 | 0 / 0 | Posible dato fijo gCdStreams: source/game_sa/CdStreamInfo.cpp:24 (0x008E3FFC); Posible dato fijo gStreamCount: source/game_sa/CdStreamInfo.cpp:25 (0x008E4090); Posible dato fijo gStreamQueue: source/game_sa/CdStreamInfo.cpp:29 (0x008E3FEC); Posible dato fijo gStreamSemaphore: source/game_sa/CdStreamInfo.cpp:30 (0x008E4004); Posible dato fijo gStreamingInitialized: source/game_sa/CdStreamInfo.cpp:27 (0x008E3FE4); Posible dato fijo gStreamingThread: source/game_sa/CdStreamInfo.cpp:31 (0x008E4008) |

<a id="grupo-925"></a>

## CdStreamPos

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ToInt() const | [source/game_sa/CdStreamInfo.h:22](../../source/game_sa/CdStreamInfo.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-926"></a>

## ColDef

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ColDef::operator new(unsigned size) | [source/game_sa/Collision/ColStore.cpp:39](../../source/game_sa/Collision/ColStore.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pColPool: source/game_sa/Collision/ColStore.cpp:24 (0x00965560) |
| ColDef::operator delete(void* data) | [source/game_sa/Collision/ColStore.cpp:43](../../source/game_sa/Collision/ColStore.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_pColPool: source/game_sa/Collision/ColStore.cpp:24 (0x00965560) |

<a id="grupo-927"></a>

## ColHelpers::FileHeader

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetTotalSize() const | [source/game_sa/Collision/ColHelpers.h:56](../../source/game_sa/Collision/ColHelpers.h#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDataSize() const | [source/game_sa/Collision/ColHelpers.h:61](../../source/game_sa/Collision/ColHelpers.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetVersion() const | [source/game_sa/Collision/ColHelpers.h:66](../../source/game_sa/Collision/ColHelpers.h#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-928"></a>

## ColHelpers::FileHeader::FileInfo

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetVersion() const | [source/game_sa/Collision/ColHelpers.h:34](../../source/game_sa/Collision/ColHelpers.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValid() const | [source/game_sa/Collision/ColHelpers.h:46](../../source/game_sa/Collision/ColHelpers.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-929"></a>

## ColHelpers::V2::Header

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsEmpty() const | [source/game_sa/Collision/ColHelpers.h:205](../../source/game_sa/Collision/ColHelpers.h#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasFaceGroups() const | [source/game_sa/Collision/ColHelpers.h:206](../../source/game_sa/Collision/ColHelpers.h#L206) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-930"></a>

## ColHelpers::V3::Header

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetNoOfShdwVerts(CCollisionData* cd) const | [source/game_sa/Collision/ColHelpers.h:224](../../source/game_sa/Collision/ColHelpers.h#L224) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasShadowMesh() const | [source/game_sa/Collision/ColHelpers.h:236](../../source/game_sa/Collision/ColHelpers.h#L236) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-931"></a>

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

<a id="grupo-932"></a>

## ColWorld59c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MulPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/Collision.cpp:2215](../../source/game_sa/Collision/Collision.cpp#L2215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MulVector(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/Collision.cpp:2224](../../source/game_sa/Collision/Collision.cpp#L2224) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityBoundSphere(CEntity* entity) | [source/game_sa/Collision/Collision.cpp:3767](../../source/game_sa/Collision/Collision.cpp#L3767) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-933"></a>

## Common

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsciiToGxtChar(const char* src, GxtChar* dst) | [source/game_sa/GxtChar.cpp:6](../../source/game_sa/GxtChar.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::AsciiToGxtChar 0x00718600 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharToAscii(const GxtChar* src, uint8 start) | [source/game_sa/GxtChar.cpp:15](../../source/game_sa/GxtChar.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Common::GxtCharToAscii 0x0069F7E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharStrcat(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:85](../../source/game_sa/GxtChar.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Common::GxtCharStrcat 0x00718630 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GxtCharStrlen(const GxtChar* str) | [source/game_sa/GxtChar.cpp:100](../../source/game_sa/GxtChar.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Common::GxtCharStrlen 0x00718690 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TextCopy(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:119](../../source/game_sa/GxtChar.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Common::TextCopy 0x0069F910 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharStrcpy(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:129](../../source/game_sa/GxtChar.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Common::GxtCharStrcpy 0x00718660 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrtPowThunkCpp(float base, float exponent) | [source/game_sa/X87Math.cpp:18594](../../source/game_sa/X87Math.cpp#L18594) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores): Common::CrtPowThunk 0x00470000 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RoundToDecimalsCpp(float value, int digits) | [source/game_sa/X87Math.cpp:18614](../../source/game_sa/X87Math.cpp#L18614) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores): Common::RoundToDecimals 0x0050A0A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoint(RwV3d& point, const CSimpleTransform& placement, const RwV3d& vecPos) | [source/game_sa/common.cpp:82](../../source/game_sa/common.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Common::TransformPoint 0x0054ECE0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoints(RwV3d* pointOut, int count, const CMatrix& transformMatrix, RwV3d* pointIn) | [source/game_sa/common.cpp:110](../../source/game_sa/common.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::TransformPoints 0x0054EEF0 | 0 / 0 | Posible dato fijo m_pWorkingMatrix1: source/game_sa/Game.h:22 (0x00B72920) |
| WriteRaster(RwRaster* raster, const char* filename) | [source/game_sa/common.cpp:174](../../source/game_sa/common.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::WriteRaster 0x005A4150 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcScreenCoors(const CVector& in, CVector& out) | [source/game_sa/common.cpp:220](../../source/game_sa/common.cpp#L220) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::CalcScreenCoors-VV 0x0071DAB0 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| LittleTest() | [source/game_sa/common.cpp:234](../../source/game_sa/common.cpp#L234) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::LittleTest 0x00541330 | 0 / 0 | Posible dato fijo g_nNumIm3dDrawCalls: source/game_sa/common.h:70 (0x00B73708) |
| DoesInfiniteLineTouchScreen( //float oX, float oY, float dX, float dY CVector2D origin, CVector2D dir ) | [source/game_sa/common.cpp:260](../../source/game_sa/common.cpp#L260) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::DoesInfiniteLineTouchScreen 0x0071DB80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInsideLine( CVector2D origin, CVector2D dir, CVector2D pt, float radius ) | [source/game_sa/common.cpp:280](../../source/game_sa/common.cpp#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::IsPointInsideLine 0x0071E050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| approxEqual(float f1, float f2, float epsilon) | [source/game_sa/common.h:216](../../source/game_sa/common.h#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::approxEqual 0x004EEA80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-934"></a>

## CommonSize

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CommonSize::Init(int32 blockSize) | [source/game_sa/Memory/CommonSize.cpp:5](../../source/game_sa/Memory/CommonSize.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CommonSize::Malloc() | [source/game_sa/Memory/CommonSize.cpp:19](../../source/game_sa/Memory/CommonSize.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommonSize::Free(HeapFreeBlockDesc* desc) | [source/game_sa/Memory/CommonSize.cpp:30](../../source/game_sa/Memory/CommonSize.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommonSize::_DumpInfo() | [source/game_sa/Memory/CommonSize.cpp:36](../../source/game_sa/Memory/CommonSize.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-935"></a>

## CompressedBox

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressedBox::DrawWireFrame(CRGBA color, const CMatrix& transform) const | [source/game_sa/CompressedBox.cpp:5](../../source/game_sa/CompressedBox.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-936"></a>

## CompressedVector

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| UncompressVectorExe(CVector* out, const int16* source) | [source/game_sa/CompressedVector.cpp:26](../../source/game_sa/CompressedVector.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CompressedVector::UncompressVectorExe 0x00411590 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-937"></a>

## CustomEnvMapPipeMaterialData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FromBuffer(const EnvMapPipeMaterialDataBuffer& b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:33](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToBuffer() const | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:42](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-938"></a>

## CustomSpecMapPipeMaterialData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FromBuffer(const SpecMapPipeMaterialDataBuffer& b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:70](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToBuffer() const | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:77](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-939"></a>

## D3DIndexDataBuffer

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| D3DIndexDataBuffer::InjectHooks() | [source/game_sa/RenderWare/D3DIndexDataBuffer.cpp:37](../../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * D3DIndexDataBuffer::Pop(uint32 indexCount) | [source/game_sa/RenderWare/D3DIndexDataBuffer.cpp:50](../../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: D3DIndexDataBuffer::Pop-Size 0x00730270 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| D3DIndexDataBuffer::Resize(uint32 newCapacity) | [source/game_sa/RenderWare/D3DIndexDataBuffer.cpp:85](../../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: D3DIndexDataBuffer::Resize 0x00730330 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| D3DIndexDataBuffer::GetTotalDataSize() | [source/game_sa/RenderWare/D3DIndexDataBuffer.cpp:107](../../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: D3DIndexDataBuffer::GetTotalDataSize 0x007303B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-940"></a>

## D3DResourceCom

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ** VTable(void* object) | [source/game_sa/RenderWare/D3DResourceSystem.h:19](../../source/game_sa/RenderWare/D3DResourceSystem.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Release(void* resource) | [source/game_sa/RenderWare/D3DResourceSystem.h:23](../../source/game_sa/RenderWare/D3DResourceSystem.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
