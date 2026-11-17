<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 996 grupos / 12438 funciones reconstruidas; 11156 con evidencia histórica y 1282 pendientes de validación en 142 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1001"></a>

## FxInfoEmPos_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmPos_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmPos_c::FxInfoEmPos_c() | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmPos_c::Constructor 0x004A5010 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmPos_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:22](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L22) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoEmPos_c::Destructor 0x004A6D30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoEmPos_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:27](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmPos_c::ScalarDeletingDestructor 0x004A6D10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmPos_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:36](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmPos_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:41](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/Blueprints/FxInfoEmPos.h:23](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmPos_c::Constructor 0x004A5010 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1002"></a>

## FxInfoEmRate_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmRate_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRate_c::FxInfoEmRate_c() | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmRate_c::Constructor 0x004A6A00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoEmRate_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:22](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmRate_c::Constructor 0x004A6A00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRate_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:28](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoEmRate_c::Destructor 0x004A6A90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoEmRate_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:33](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmRate_c::ScalarDeletingDestructor 0x004A6A70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRate_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:42](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRate_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:47](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1003"></a>

## FxInfoEmRotation_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmRotation_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:5](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRotation_c::FxInfoEmRotation_c() | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:15](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmRotation_c::Constructor 0x004A51B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRotation_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:21](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRotation_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:26](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRotation_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:39](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmRotation_c::DestructorBody 0x004A6E10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoEmRotation_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:47](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmRotation_c::ScalarDeletingDestructor 0x004A6DF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.h:22](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmRotation_c::Constructor 0x004A51B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1004"></a>

## FxInfoEmSize_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmSize_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSize_c::FxInfoEmSize_c() | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmSize_c::Constructor 0x004A4C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSize_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:22](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L22) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoEmSize_c::Destructor 0x004A6B00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoEmSize_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:27](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmSize_c::ScalarDeletingDestructor 0x004A6AE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSize_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:36](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSize_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:41](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/Blueprints/FxInfoEmSize.h:23](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmSize_c::Constructor 0x004A4C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1005"></a>

## FxInfoEmSpeed_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmSpeed_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSpeed_c::FxInfoEmSpeed_c() | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmSpeed_c::Constructor 0x004A4D00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSpeed_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:23](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L23) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoEmSpeed_c::Destructor 0x004A6B70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoEmSpeed_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:28](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmSpeed_c::ScalarDeletingDestructor 0x004A6B50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSpeed_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:37](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSpeed_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:47](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.h:23](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmSpeed_c::Constructor 0x004A4D00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1006"></a>

## FxInfoEmWeather_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmWeather_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmWeather_c::FxInfoEmWeather_c() | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmWeather_c::Constructor 0x004A50E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmWeather_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:22](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmWeather_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:27](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmWeather_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:42](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmWeather_c::DestructorBody 0x004A6DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoEmWeather_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:50](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmWeather_c::ScalarDeletingDestructor 0x004A6D80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.h:24](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoEmWeather_c::Constructor 0x004A50E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1007"></a>

## FxInfoFlat_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoFlat_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFlat_c::FxInfoFlat_c() | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFlat_c::Constructor 0x004A6320 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoFlat_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:23](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFlat_c::Constructor 0x004A6320 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFlat_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:29](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoFlat_c::Destructor 0x004A7810 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoFlat_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:34](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFlat_c::ScalarDeletingDestructor 0x004A77F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFlat_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:43](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFlat_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:48](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1008"></a>

## FxInfoFloat_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoFloat_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFloat_c::FxInfoFloat_c() | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFloat_c::Constructor 0x004A5DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoFloat_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:24](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFloat_c::Constructor 0x004A5DA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFloat_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:30](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFloat_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:35](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFloat_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:43](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFloat_c::DestructorBody 0x004A7500 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoFloat_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:51](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFloat_c::ScalarDeletingDestructor 0x004A74E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1009"></a>

## FxInfoForce_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoForce_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:5](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoForce_c::FxInfoForce_c() | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:15](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoForce_c::Constructor 0x004A5450 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoForce_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:21](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoForce_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:26](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoForce_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:42](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoForce_c::DestructorBody 0x004A6EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoForce_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:50](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoForce_c::ScalarDeletingDestructor 0x004A6ED0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/Blueprints/FxInfoForce.h:22](../../source/game_sa/Fx/Blueprints/FxInfoForce.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoForce_c::Constructor 0x004A5450 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1010"></a>

## FxInfoFriction_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoFriction_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:5](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFriction_c::FxInfoFriction_c() | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:15](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFriction_c::Constructor 0x004A5550 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFriction_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:21](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFriction_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:26](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFriction_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:42](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFriction_c::DestructorBody 0x004A6F60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoFriction_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:50](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFriction_c::ScalarDeletingDestructor 0x004A6F40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/Blueprints/FxInfoFriction.h:22](../../source/game_sa/Fx/Blueprints/FxInfoFriction.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoFriction_c::Constructor 0x004A5550 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1011"></a>

## FxInfoGroundCollide_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoGroundCollide_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoGroundCollide_c::FxInfoGroundCollide_c() | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoGroundCollide_c::Constructor 0x004A5980 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoGroundCollide_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:22](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoGroundCollide_c::Constructor 0x004A5980 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoGroundCollide_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:28](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoGroundCollide_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:33](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoGroundCollide_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:71](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoGroundCollide_c::DestructorBody 0x004A70B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoGroundCollide_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:79](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoGroundCollide_c::ScalarDeletingDestructor 0x004A7090 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1012"></a>

## FxInfoHeatHaze_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoHeatHaze_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoHeatHaze_c::FxInfoHeatHaze_c() | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoHeatHaze_c::Constructor 0x004A61A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoHeatHaze_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:23](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoHeatHaze_c::Constructor 0x004A61A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoHeatHaze_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:29](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoHeatHaze_c::Destructor 0x004A7730 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoHeatHaze_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:34](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoHeatHaze_c::ScalarDeletingDestructor 0x004A7710 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoHeatHaze_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:43](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoHeatHaze_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:48](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1013"></a>

## FxInfoJitter_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoJitter_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoJitter_c::FxInfoJitter_c() | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoJitter_c::Constructor 0x004A5B20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoJitter_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:22](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoJitter_c::Constructor 0x004A5B20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoJitter_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:28](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoJitter_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:33](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoJitter_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:53](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoJitter_c::DestructorBody 0x004A7420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoJitter_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:61](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoJitter_c::ScalarDeletingDestructor 0x004A7400 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1014"></a>

## FxInfoManager_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoManager_c::InjectHooks() | [source/game_sa/Fx/FxInfoManager.cpp:75](../../source/game_sa/Fx/FxInfoManager.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoManager_c::AddFxInfo(int32 info) | [source/game_sa/Fx/FxInfoManager.cpp:103](../../source/game_sa/Fx/FxInfoManager.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxInfoManager_c::AddFxInfo 0x004A7B00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoManager_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/FxInfoManager.cpp:203](../../source/game_sa/Fx/FxInfoManager.cpp#L203) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInfoManager_c::Load 0x005C0B70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Conversión de dirección fija: 0x008220AD; Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInfoManager_c::ProcessEmissionInfo(float currentTime, float mult, float totalTime, bool useConst, EmissionInfo_t* emission) | [source/game_sa/Fx/FxInfoManager.cpp:271](../../source/game_sa/Fx/FxInfoManager.cpp#L271) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxInfoManager_c::ProcessEmissionInfo 0x004A4960 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoManager_c::ProcessMovementInfo(float currentTime, float mult, float totalTime, float length, bool useConst, MovementInfo_t* movementInfo) | [source/game_sa/Fx/FxInfoManager.cpp:285](../../source/game_sa/Fx/FxInfoManager.cpp#L285) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxInfoManager_c::ProcessMovementInfo 0x004A4A10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoManager_c::ProcessRenderInfo(float currentTime, float mult, float totalTime, float length, bool useConst, RenderInfo_t* renderInfo) | [source/game_sa/Fx/FxInfoManager.cpp:299](../../source/game_sa/Fx/FxInfoManager.cpp#L299) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxInfoManager_c::ProcessRenderInfo 0x004A4A80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetInfos() | [source/game_sa/Fx/FxInfoManager.h:50](../../source/game_sa/Fx/FxInfoManager.h#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetInfos() const | [source/game_sa/Fx/FxInfoManager.h:51](../../source/game_sa/Fx/FxInfoManager.h#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1015"></a>

## FxInfoNoise_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoNoise_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:5](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoNoise_c::FxInfoNoise_c() | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:15](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoNoise_c::Constructor 0x004A5270 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoNoise_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:20](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoNoise_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:25](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoNoise_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:52](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoNoise_c::DestructorBody 0x004A6E80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoNoise_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:60](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoNoise_c::ScalarDeletingDestructor 0x004A6E60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/Blueprints/FxInfoNoise.h:22](../../source/game_sa/Fx/Blueprints/FxInfoNoise.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoNoise_c::Constructor 0x004A5270 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1016"></a>

## FxInfoRotSpeed_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoRotSpeed_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoRotSpeed_c::FxInfoRotSpeed_c() | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoRotSpeed_c::Constructor 0x004A5CB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoRotSpeed_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:23](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoRotSpeed_c::Constructor 0x004A5CB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoRotSpeed_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:29](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoRotSpeed_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:34](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoRotSpeed_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:53](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoRotSpeed_c::DestructorBody 0x004A7490 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoRotSpeed_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:61](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoRotSpeed_c::ScalarDeletingDestructor 0x004A7470 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1017"></a>

## FxInfoSelfLit_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSelfLit_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSelfLit_c::FxInfoSelfLit_c() | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSelfLit_c::Constructor 0x004A6760 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoSelfLit_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:24](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSelfLit_c::Constructor 0x004A6760 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSelfLit_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:30](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoSelfLit_c::Destructor 0x004A79D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoSelfLit_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:35](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSelfLit_c::ScalarDeletingDestructor 0x004A79B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSelfLit_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:44](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSelfLit_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:49](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1018"></a>

## FxInfoSize_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSize_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSize_c::FxInfoSize_c() | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSize_c::Constructor 0x004A5FA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoSize_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:23](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSize_c::Constructor 0x004A5FA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSize_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:29](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSize_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:37](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSize_c::DestructorBody() | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:62](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSize_c::DestructorBody 0x004A7650 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoSize_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:70](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSize_c::ScalarDeletingDestructor 0x004A7630 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1019"></a>

## FxInfoSmoke_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSmoke_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSmoke_c::FxInfoSmoke_c() | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSmoke_c::Constructor 0x004A68E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoSmoke_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:22](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSmoke_c::Constructor 0x004A68E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSmoke_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:28](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoSmoke_c::Destructor 0x004A7AB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoSmoke_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:33](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSmoke_c::ScalarDeletingDestructor 0x004A7A90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSmoke_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:42](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSmoke_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:47](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1020"></a>

## FxInfoSpriteRect_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSpriteRect_c::InjectHooks() | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSpriteRect_c::FxInfoSpriteRect_c() | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSpriteRect_c::Constructor 0x004A60B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoSpriteRect_c::Constructor() | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:22](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSpriteRect_c::Constructor 0x004A60B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSpriteRect_c::Destructor() | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:28](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: FxInfoSpriteRect_c::Destructor 0x004A76C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSpriteRect_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:33](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSpriteRect_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:38](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoSpriteRect_c::ScalarDeletingDestructor(uint8 flags) | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:58](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: FxInfoSpriteRect_c::ScalarDeletingDestructor 0x004A76A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
