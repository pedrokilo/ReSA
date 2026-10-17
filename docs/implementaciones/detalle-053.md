<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1041"></a>

## Globales y helpers: source/game_sa/Camera.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ActivePlayerPed() | [source/game_sa/Camera.cpp:63](../../source/game_sa/Camera.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| ElapsedSeconds(int32 elapsedMs) | [source/game_sa/Camera.cpp:69](../../source/game_sa/Camera.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CutSceneElapsed(float percent, uint32 finishTime) | [source/game_sa/Camera.cpp:74](../../source/game_sa/Camera.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreTimeAndTimePlus(float& start, float& end, float plus) | [source/game_sa/Camera.cpp:80](../../source/game_sa/Camera.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraLinearComponent(float from, float to, float ratio, bool ease) | [source/game_sa/Camera.cpp:1642](../../source/game_sa/Camera.cpp#L1642) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraLinearTime() | [source/game_sa/Camera.cpp:1645](../../source/game_sa/Camera.cpp#L1645) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraObbeCinemaCycle(CCamera& camera, const ObbeCinemaKind& kind) | [source/game_sa/Camera.cpp:1796](../../source/game_sa/Camera.cpp#L1796) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheText: source/game_sa/Text/Text.h:88 (0x00C1B340) |
| CameraProcessPhase(uint32 elapsed,uint32 duration) | [source/game_sa/Camera.cpp:1997](../../source/game_sa/Camera.cpp#L1997) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessFractionEase(double phase,float moving,float catchup,bool initial) | [source/game_sa/Camera.cpp:2000](../../source/game_sa/Camera.cpp#L2000) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessAdvance(CVector& result,const CVector& base,const CVector& speed,double coefficient,int mode) | [source/game_sa/Camera.cpp:2003](../../source/game_sa/Camera.cpp#L2003) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessAdvanceToward(CVector& result,const CVector& base,const CVector& destination,double coefficient,int mode) | [source/game_sa/Camera.cpp:2006](../../source/game_sa/Camera.cpp#L2006) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessFovAdvance(float base,float operand,float coefficient,bool difference) | [source/game_sa/Camera.cpp:2009](../../source/game_sa/Camera.cpp#L2009) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessShakeForce(float force, uint32 elapsed) | [source/game_sa/Camera.cpp:2012](../../source/game_sa/Camera.cpp#L2012) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessAccumulateDistance(const CVector& current,const CVector& previous,float sum) | [source/game_sa/Camera.cpp:2015](../../source/game_sa/Camera.cpp#L2015) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessAverageDistance(double sum,int32 frames) | [source/game_sa/Camera.cpp:2018](../../source/game_sa/Camera.cpp#L2018) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessTruncateProduct(float value, float multiplier) | [source/game_sa/Camera.cpp:2023](../../source/game_sa/Camera.cpp#L2023) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessOrthonormalise(CVector& front, CVector& up, eCamMode mode) | [source/game_sa/Camera.cpp:2026](../../source/game_sa/Camera.cpp#L2026) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraProcessMinimumDistance(CVector& source, const CVector& target) | [source/game_sa/Camera.cpp:2040](../../source/game_sa/Camera.cpp#L2040) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraFrustumDot355(const CVector& normal,const CVector& position) | [source/game_sa/Camera.cpp:2354](../../source/game_sa/Camera.cpp#L2354) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCamera::CalculateFrustumPlanes 0x00514D60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CineyModeRotatedDirection(CVector velocity, float rotation) | [source/game_sa/Camera.cpp:2491](../../source/game_sa/Camera.cpp#L2491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCamera::TryToStartNewCamMode 0x0051E560 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlMode(const CCam& cam) | [source/game_sa/Camera.cpp:2709](../../source/game_sa/Camera.cpp#L2709) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlModeIn(uint16 mode, std::initializer_list&lt;uint16&gt; modes) | [source/game_sa/Camera.cpp:2712](../../source/game_sa/Camera.cpp#L2712) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlSetTarget(CCam& cam, CEntity* entity) | [source/game_sa/Camera.cpp:2720](../../source/game_sa/Camera.cpp#L2720) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlCopyFixedState(CCamera& camera, bool lookingAtVector) | [source/game_sa/Camera.cpp:2727](../../source/game_sa/Camera.cpp#L2727) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlFinish(CCamera& camera, uint16 initialMode, bool cinematicAllowed) | [source/game_sa/Camera.cpp:2739](../../source/game_sa/Camera.cpp#L2739) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo gPlayerPedVisible: source/game_sa/Camera.cpp:47 (0x008CC380) |
| CameraControlCommitJump(CCamera& camera, uint16 mode, bool lookingAtVector) | [source/game_sa/Camera.cpp:2787](../../source/game_sa/Camera.cpp#L2787) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlBegin(CCamera& camera) | [source/game_sa/Camera.cpp:2798](../../source/game_sa/Camera.cpp#L2798) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCamera::CamControl 0x00527FA0 | 0 / 0 | Conversión de dirección fija: 0x00B7CD98 |
| CameraControlApplyPlayerOverrides(CCamera& camera) | [source/game_sa/Camera.cpp:2833](../../source/game_sa/Camera.cpp#L2833) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B7CD98 |
| CameraControlApplyRestore(CCamera& camera) | [source/game_sa/Camera.cpp:2906](../../source/game_sa/Camera.cpp#L2906) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlApplyCinematic(CCamera& camera, bool& jump) | [source/game_sa/Camera.cpp:2929](../../source/game_sa/Camera.cpp#L2929) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x005267C0: CCamera::ProcessObbeCinemaCameraCar; ruta efectiva sin resolver; Colaborador por dirección 0x00526950: CCamera::ProcessObbeCinemaCameraTrain; ruta efectiva sin resolver; Colaborador por dirección 0x00526AE0: CCamera::ProcessObbeCinemaCameraHeli; ruta efectiva sin resolver; Colaborador por dirección 0x00526C80: CCamera::ProcessObbeCinemaCameraPlane; ruta efectiva sin resolver; Colaborador por dirección 0x00526E20: CCamera::ProcessObbeCinemaCameraBoat; ruta efectiva sin resolver; Posible dato fijo gbModelViewer: source/game_sa/Camera.cpp:41 (0x00BA6728) |
| CameraControlStepZoom(float& value, float target) | [source/game_sa/Camera.cpp:2996](../../source/game_sa/Camera.cpp#L2996) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlVehicleZoom(CCamera& camera) | [source/game_sa/Camera.cpp:3007](../../source/game_sa/Camera.cpp#L3007) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x00509AE0: CCam::WellBufferMe; ruta efectiva sin resolver |
| CameraControlPedZoom(CCamera& camera) | [source/game_sa/Camera.cpp:3048](../../source/game_sa/Camera.cpp#L3048) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x00509AE0: CCam::WellBufferMe; ruta efectiva sin resolver |
| CameraControlScriptTransition(CCamera& camera) | [source/game_sa/Camera.cpp:3076](../../source/game_sa/Camera.cpp#L3076) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlPlayerTransition(CCamera& camera, bool jump) | [source/game_sa/Camera.cpp:3121](../../source/game_sa/Camera.cpp#L3121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlPolarSource(float angle, float radius, const CVector& position, CVector& source) | [source/game_sa/Camera.cpp:3190](../../source/game_sa/Camera.cpp#L3190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlWeaponMode(CCamera& camera, bool stairs) | [source/game_sa/Camera.cpp:3197](../../source/game_sa/Camera.cpp#L3197) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B7CD98 |
| CameraControlCycleUp(CPad* pad) | [source/game_sa/Camera.cpp:3278](../../source/game_sa/Camera.cpp#L3278) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlVehicleCycle(CCamera& camera) | [source/game_sa/Camera.cpp:3281](../../source/game_sa/Camera.cpp#L3281) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlPedCycle(CCamera& camera) | [source/game_sa/Camera.cpp:3308](../../source/game_sa/Camera.cpp#L3308) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlWrapHeading(float heading, float reference) | [source/game_sa/Camera.cpp:3364](../../source/game_sa/Camera.cpp#L3364) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlTargetHeadingMinusHalfPi(CPed* ped) | [source/game_sa/Camera.cpp:3374](../../source/game_sa/Camera.cpp#L3374) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlChooseJump(CCamera& camera, bool stairs, bool wasArrested, bool& jump) | [source/game_sa/Camera.cpp:3385](../../source/game_sa/Camera.cpp#L3385) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlGarageDoors(CGarage* garage, CObject*& first, CObject*& second) | [source/game_sa/Camera.cpp:3487](../../source/game_sa/Camera.cpp#L3487) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00449FF0: fuera del catálogo consultado; ruta efectiva sin resolver |
| CameraControlZoneExtent(const CAttributeZone* zone) | [source/game_sa/Camera.cpp:3490](../../source/game_sa/Camera.cpp#L3490) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlZoneCameraPosition(CAttributeZone* zone) | [source/game_sa/Camera.cpp:3497](../../source/game_sa/Camera.cpp#L3497) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlZoneSide(CCamera& camera, CAttributeZone* zone, bool vehicle, CVector& source, CVector& center) | [source/game_sa/Camera.cpp:3503](../../source/game_sa/Camera.cpp#L3503) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlGarageDoorSource(CCamera& camera, CObject* first, CObject* second, const CVector& center, const CVector& target, bool vehicle) | [source/game_sa/Camera.cpp:3523](../../source/game_sa/Camera.cpp#L3523) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlLeaveGarage(CCamera& camera) | [source/game_sa/Camera.cpp:3558](../../source/game_sa/Camera.cpp#L3558) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlVehicleGarage(CCamera& camera, bool& stairs) | [source/game_sa/Camera.cpp:3565](../../source/game_sa/Camera.cpp#L3565) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraControlPedGarage(CCamera& camera, bool& stairs) | [source/game_sa/Camera.cpp:3640](../../source/game_sa/Camera.cpp#L3640) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1042"></a>

## Globales y helpers: source/game_sa/CarAI.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float f) | [source/game_sa/CarAI.cpp:81](../../source/game_sa/CarAI.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Dist2D(const CVector& a, const CVector& b) | [source/game_sa/CarAI.cpp:86](../../source/game_sa/CarAI.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Mag3(const CVector& v) | [source/game_sa/CarAI.cpp:93](../../source/game_sa/CarAI.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Dot3(const CVector& a, const CVector& b) | [source/game_sa/CarAI.cpp:98](../../source/game_sa/CarAI.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeForward(const CPlaceable& e) | [source/game_sa/CarAI.cpp:103](../../source/game_sa/CarAI.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeRight(const CPlaceable& e) | [source/game_sa/CarAI.cpp:112](../../source/game_sa/CarAI.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeadOnDot(const CPlaceable& e, const CVector& d) | [source/game_sa/CarAI.cpp:122](../../source/game_sa/CarAI.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFarFromDestination(const CAutoPilot& ap, const CVector& coors, float threshold) | [source/game_sa/CarAI.cpp:131](../../source/game_sa/CarAI.cpp#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PrepareGoToCoors(CVehicle* veh, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) | [source/game_sa/CarAI.cpp:137](../../source/game_sa/CarAI.cpp#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTempActionFor(CAutoPilot& ap, eAutoPilotTempAction action, uint32 durMs) | [source/game_sa/CarAI.cpp:150](../../source/game_sa/CarAI.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayerWantedLevel() | [source/game_sa/CarAI.cpp:155](../../source/game_sa/CarAI.cpp#L155) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1043"></a>

## Globales y helpers: source/game_sa/CarEnterExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputePassengerIndexFromCarDoor_KeepEdx(const CVehicle* vehicle, int32 doorId) | [source/game_sa/CarEnterExit.cpp:39](../../source/game_sa/CarEnterExit.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1044"></a>

## Globales y helpers: source/game_sa/CdStreamInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectCdStreamHooks() | [source/game_sa/CdStreamInfo.cpp:37](../../source/game_sa/CdStreamInfo.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CdStreamHandleToFileID(CdStreamHandle h) | [source/game_sa/CdStreamInfo.cpp:313](../../source/game_sa/CdStreamInfo.cpp#L313) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1045"></a>

## Globales y helpers: source/game_sa/CdStreamInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CdStreamGetLastPosn() | [source/game_sa/CdStreamInfo.h:94](../../source/game_sa/CdStreamInfo.h#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gLastCdStreamPosn: source/game_sa/CdStreamInfo.cpp:33 (0x008E4898) |

<a id="grupo-1046"></a>

## Globales y helpers: source/game_sa/Clothes.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetClothesModelFromName(const char* name) | [source/game_sa/Clothes.cpp:68](../../source/game_sa/Clothes.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1047"></a>

## Globales y helpers: source/game_sa/ClothesBuilder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClothesBlendPair(float a, float b, float ra, float rb) | [source/game_sa/ClothesBuilder.cpp:248](../../source/game_sa/ClothesBuilder.cpp#L248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClothesBlendTriple(float a, float b, float c, float ra, float rb, float rc) | [source/game_sa/ClothesBuilder.cpp:254](../../source/game_sa/ClothesBuilder.cpp#L254) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClothesNormalizeWeights(float* weights) | [source/game_sa/ClothesBuilder.cpp:262](../../source/game_sa/ClothesBuilder.cpp#L262) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * BlendGeometry(RpClump* clump, std::pair&lt;const char*, float&gt; (&&frameNamesRatios)\[N\]) | [source/game_sa/ClothesBuilder.cpp:274](../../source/game_sa/ClothesBuilder.cpp#L274) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AssertTextureLayouts(std::initializer_list&lt;RwTexture*&gt; textures) | [source/game_sa/ClothesBuilder.cpp:459](../../source/game_sa/ClothesBuilder.cpp#L459) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClothesBlendByte(float v) | [source/game_sa/ClothesBuilder.cpp:528](../../source/game_sa/ClothesBuilder.cpp#L528) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1048"></a>

## Globales y helpers: source/game_sa/Clouds.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CloudsFtol(float value) | [source/game_sa/Clouds.cpp:51](../../source/game_sa/Clouds.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CloudsCameraRight() | [source/game_sa/Clouds.cpp:56](../../source/game_sa/Clouds.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |

<a id="grupo-1049"></a>

## Globales y helpers: source/game_sa/Collision/ColSphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CColSphere& sp, const CMatrix& transform) -&gt; CColSphere | [source/game_sa/Collision/ColSphere.cpp:129](../../source/game_sa/Collision/ColSphere.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1050"></a>

## Globales y helpers: source/game_sa/Collision/ColStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityAreaCode(const CEntity* entity) | [source/game_sa/Collision/ColStore.cpp:34](../../source/game_sa/Collision/ColStore.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1051"></a>

## Globales y helpers: source/game_sa/Collision/Collision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestPtPointTriangle( CVector a, CVector b, CVector c, CVector p ) | [source/game_sa/Collision/Collision.cpp:830](../../source/game_sa/Collision/Collision.cpp#L830) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessLineSphere_Internal( const CColLine& line, const CColSphere& sphere, CVector* ip, float* depth ) | [source/game_sa/Collision/Collision.cpp:890](../../source/game_sa/Collision/Collision.cpp#L890) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOTSA_FORCEINLINE ProcessLineTriangle_Internal( const CColLine& line, const CStoredCollPoly& poly, const CColTrianglePlane& plane, float* inOutMaxTouchDist, CVector* outIP, CVector* outPlNorm ) | [source/game_sa/Collision/Collision.cpp:1455](../../source/game_sa/Collision/Collision.cpp#L1455) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNearestDistanceOfPedSphereToCameraNearClip(CPed* ped) | [source/game_sa/Collision/Collision.cpp:3137](../../source/game_sa/Collision/Collision.cpp#L3137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |
| IsPointInPoly2D(CVector2D pt, CVector2D a, CVector2D b, CVector2D c) | [source/game_sa/Collision/Collision.cpp:3298](../../source/game_sa/Collision/Collision.cpp#L3298) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SphereVsEntityBody(CColSphere* /*sphere: ignorada, como en el EXE*/, CEntity* entity) | [source/game_sa/Collision/Collision.cpp:4119](../../source/game_sa/Collision/Collision.cpp#L4119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |

<a id="grupo-1052"></a>

## Globales y helpers: source/game_sa/Collision/ColourSet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/Collision/ColourSet.cpp:110](../../source/game_sa/Collision/ColourSet.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1053"></a>

## Globales y helpers: source/game_sa/Collision/Sphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CSphere& sp, const CMatrix& transform) -&gt; CSphere | [source/game_sa/Collision/Sphere.cpp:54](../../source/game_sa/Collision/Sphere.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1054"></a>

## Globales y helpers: source/game_sa/CompressedVector.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressVector(const CVector& v) | [source/game_sa/CompressedVector.h:12](../../source/game_sa/CompressedVector.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UncompressVector(const CompressedVector& v) | [source/game_sa/CompressedVector.h:16](../../source/game_sa/CompressedVector.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CompressLargeVector(const CVector& v) | [source/game_sa/CompressedVector.h:21](../../source/game_sa/CompressedVector.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UncompressLargeVector(const CompressedLargeVector& v) | [source/game_sa/CompressedVector.h:25](../../source/game_sa/CompressedVector.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CompressUnitVector(const CVector& v) | [source/game_sa/CompressedVector.h:30](../../source/game_sa/CompressedVector.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UncompressUnitVector(const CompressedUnitVector& v) | [source/game_sa/CompressedVector.h:34](../../source/game_sa/CompressedVector.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CompressFxVector(const CVector& v) | [source/game_sa/CompressedVector.h:39](../../source/game_sa/CompressedVector.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UncompressFxVector(const CompressedFxVector& v) | [source/game_sa/CompressedVector.h:43](../../source/game_sa/CompressedVector.h#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1055"></a>

## Globales y helpers: source/game_sa/ControllerConfigManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MouseButtonByte(const CMouseControllerState& state, KeyCode key) | [source/game_sa/ControllerConfigManager.cpp:950](../../source/game_sa/ControllerConfigManager.cpp#L950) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KeyCharToGxtChar(uint8 ch) | [source/game_sa/ControllerConfigManager.cpp:1218](../../source/game_sa/ControllerConfigManager.cpp#L1218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x008D5AD8 |

<a id="grupo-1056"></a>

## Globales y helpers: source/game_sa/Conversations/ConversationForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ConversationTooFar(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:144](../../source/game_sa/Conversations/ConversationForPed.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationFacingAway(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:152](../../source/game_sa/Conversations/ConversationForPed.cpp#L152) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationMovingTooFast(const CVector& speed) | [source/game_sa/Conversations/ConversationForPed.cpp:157](../../source/game_sa/Conversations/ConversationForPed.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1057"></a>

## Globales y helpers: source/game_sa/Core/Matrix.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EulerAxesFromFlags(uint32 flags) | [source/game_sa/Core/Matrix.cpp:323](../../source/game_sa/Core/Matrix.cpp#L323) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:375](../../source/game_sa/Core/Matrix.cpp#L375) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CVector& b) | [source/game_sa/Core/Matrix.cpp:399](../../source/game_sa/Core/Matrix.cpp#L399) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:403](../../source/game_sa/Core/Matrix.cpp#L403) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Invert(CMatrix& in, CMatrix& out) | [source/game_sa/Core/Matrix.cpp:413](../../source/game_sa/Core/Matrix.cpp#L413) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Invert(const CMatrix& in) | [source/game_sa/Core/Matrix.cpp:419](../../source/game_sa/Core/Matrix.cpp#L419) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(CMatrix from, CMatrix to, float t) | [source/game_sa/Core/Matrix.cpp:424](../../source/game_sa/Core/Matrix.cpp#L424) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1058"></a>

## Globales y helpers: source/game_sa/Core/Quaternion.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DotProduct(const CQuaternion& q1, const CQuaternion& q2) | [source/game_sa/Core/Quaternion.h:122](../../source/game_sa/Core/Quaternion.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:129](../../source/game_sa/Core/Quaternion.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:133](../../source/game_sa/Core/Quaternion.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:137](../../source/game_sa/Core/Quaternion.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:141](../../source/game_sa/Core/Quaternion.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:145](../../source/game_sa/Core/Quaternion.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1059"></a>

## Globales y helpers: source/game_sa/Core/Vector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * CrossProduct(CVector* out, CVector* a, CVector* b) | [source/game_sa/Core/Vector.cpp:183](../../source/game_sa/Core/Vector.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProduct(const CVector& a, const CVector& b) | [source/game_sa/Core/Vector.cpp:189](../../source/game_sa/Core/Vector.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(CVector* v1, CVector* v2) | [source/game_sa/Core/Vector.cpp:194](../../source/game_sa/Core/Vector.cpp#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:199](../../source/game_sa/Core/Vector.cpp#L199) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct2D(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:204](../../source/game_sa/Core/Vector.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1060"></a>

## Globales y helpers: source/game_sa/Core/Vector.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| operator-(const CVector& vecOne, const CVector& vecTwo) | [source/game_sa/Core/Vector.h:226](../../source/game_sa/Core/Vector.h#L226) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CVector& vecOne, const CVector& vecTwo) | [source/game_sa/Core/Vector.h:230](../../source/game_sa/Core/Vector.h#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CVector& vecOne, const CVector& vecTwo) | [source/game_sa/Core/Vector.h:234](../../source/game_sa/Core/Vector.h#L234) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator!=(const CVector& vecOne, const CVector& vecTwo) | [source/game_sa/Core/Vector.h:238](../../source/game_sa/Core/Vector.h#L238) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator!=(const CVector& vec, float notEqualTo) | [source/game_sa/Core/Vector.h:242](../../source/game_sa/Core/Vector.h#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const CVector& vec, float equalTo) | [source/game_sa/Core/Vector.h:246](../../source/game_sa/Core/Vector.h#L246) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const CVector& vecLeft, const CVector& vecRight) | [source/game_sa/Core/Vector.h:250](../../source/game_sa/Core/Vector.h#L250) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CVector& vec, float dividend) | [source/game_sa/Core/Vector.h:255](../../source/game_sa/Core/Vector.h#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float multiplier, const CVector& vec) | [source/game_sa/Core/Vector.h:259](../../source/game_sa/Core/Vector.h#L259) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CVector& vec) | [source/game_sa/Core/Vector.h:263](../../source/game_sa/Core/Vector.h#L263) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DistanceBetweenPoints(const CVector& pointOne, const CVector& pointTwo) | [source/game_sa/Core/Vector.h:267](../../source/game_sa/Core/Vector.h#L267) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DistanceBetweenPointsSquared(const CVector& pointOne, const CVector& pointTwo) | [source/game_sa/Core/Vector.h:271](../../source/game_sa/Core/Vector.h#L271) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(const CVector& vecOne, const CVector& vecTwo, float fProgress) | [source/game_sa/Core/Vector.h:275](../../source/game_sa/Core/Vector.h#L275) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clamp(CVector val, CVector min, CVector max) | [source/game_sa/Core/Vector.h:280](../../source/game_sa/Core/Vector.h#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Pow(const CVector& vec, float fPow) | [source/game_sa/Core/Vector.h:287](../../source/game_sa/Core/Vector.h#L287) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Normalized(CVector v) | [source/game_sa/Core/Vector.h:296](../../source/game_sa/Core/Vector.h#L296) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProjectVector(const CVector& what, const CVector& onto) | [source/game_sa/Core/Vector.h:297](../../source/game_sa/Core/Vector.h#L297) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Multiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Core/Vector.h:301](../../source/game_sa/Core/Vector.h#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Multiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Core/Vector.h:302](../../source/game_sa/Core/Vector.h#L302) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyMatrixWithVector(const CMatrix& mat, const CVector& vec) | [source/game_sa/Core/Vector.h:303](../../source/game_sa/Core/Vector.h#L303) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
