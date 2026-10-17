// Tipos falsos con los desplazamientos reales del EXE, solo para compilar CVehicle::DoVehicleLights / DoHeadLightEffect fuera del proyecto.
#pragma once
#include <windows.h>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t;
using int8 = std::int8_t; using int16 = std::int16_t; using int32 = std::int32_t; using uint64 = std::uint64_t; using int64 = std::int64_t;
#include "X87Math.h"

struct CVector {
    float x{}, y{}, z{};
    CVector() = default;
    CVector(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};
struct CMatrix {
    CVector m_right; uint32 f0;
    CVector m_forward; uint32 f1;
    CVector m_up; uint32 f2;
    CVector m_pos; uint32 f3;
    CVector& GetForward() { return m_forward; }
    CVector& GetPosition() { return m_pos; }
};
struct CMatrixLink : CMatrix { uint32 tail[2]; };
static_assert(offsetof(CMatrix, m_forward) == 0x10 && offsetof(CMatrix, m_pos) == 0x30 && sizeof(CMatrixLink) == 0x48);

struct CSimpleTransform { CVector m_vPosn; float m_fHeading; };
struct CPlaceable {
    void* vptr_;
    CSimpleTransform m_placement;
    CMatrixLink* m_matrix;
    CVector& GetPosition() { return m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn; }
};
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CPlaceable, m_placement) + offsetof(CSimpleTransform, m_fHeading) == 0x10);

enum eVehicleDummy { DUMMY_LIGHT_FRONT_MAIN = 0, DUMMY_LIGHT_REAR_MAIN = 1 };
enum eVehicleType : int32 { VEHICLE_TYPE_AUTOMOBILE = 0, VEHICLE_TYPE_TRAIN = 6, VEHICLE_TYPE_BIKE = 9 };
enum eModelID { MODEL_STALLION = 439, MODEL_TRAM = 449, MODEL_SABRE = 475, MODEL_ZR350 = 477 };
enum eVehicleHandlingFlags : uint32 { VEHICLE_HANDLING_HALOGEN_LIGHTS = 0x400000 };
enum eEntityStatus : uint8 { STATUS_PLAYER = 0, STATUS_ABANDONED = 4, STATUS_WRECKED = 5 };
enum eCamMode : uint16 { MODE_1STPERSON = 16 };
enum eCoronaType { CORONATYPE_SHINYSTAR = 0, CORONATYPE_HEADLIGHT = 1, CORONATYPE_HEADLIGHTLINE = 4 };
enum eCoronaFlareType : uint8 { FLARETYPE_NONE = 0 };
enum eCoronaLOSCheck : uint8 { LOSCHECK_OFF = 0 };
enum eCoronaReflType : uint8 { CORREFL_NONE = 0, CORREFL_SIMPLE = 1 };
enum eCoronaTrail : uint8 { TRAIL_OFF = 0 };
enum eLights : uint8 { LIGHT_FRONT_LEFT = 0, LIGHT_FRONT_RIGHT = 1, LIGHT_REAR_RIGHT = 2, LIGHT_REAR_LEFT = 3, MAX_LIGHTS };
enum eLightsState : uint8 { VEHICLE_LIGHT_OK = 0, VEHICLE_LIGHT_SMASHED = 1 };
enum ePointLightType : uint8 { PLTYPE_POINTLIGHT = 0, PLTYPE_DIRECTIONAL, PLTYPE_ANTILIGHT };
enum eVehicleLightsFlags : uint32 { VEHICLE_LIGHTS_TWIN = 1, VEHICLE_LIGHTS_IGNORE_DAMAGE = 4, VEHICLE_LIGHTS_DISABLE_FRONT = 16, VEHICLE_LIGHTS_DISABLE_REAR = 32 };
enum eVehicleOverrideLightsState { NO_CAR_LIGHT_OVERRIDE = 0, FORCE_CAR_LIGHTS_OFF = 1, FORCE_CAR_LIGHTS_ON = 2 };

struct CVehicleStructure { std::array<CVector, 15> m_avDummyPos; };
struct CVehicleModelInfo {
    uint8 _p[0x5C]; CVehicleStructure* m_pVehicleStruct;
    auto&& GetModelDummyPosition(this auto&& self, eVehicleDummy dummy) { return self.m_pVehicleStruct->m_avDummyPos[dummy]; }
};
static_assert(offsetof(CVehicleModelInfo, m_pVehicleStruct) == 0x5C);
struct tHandlingData { uint8 _p[0xD0]; eVehicleHandlingFlags m_nHandlingFlags; uint8 _q[0xE0 - 0xD4]; };
static_assert(offsetof(tHandlingData, m_nHandlingFlags) == 0xD0);
struct CDamageManager { uint8 raw[0x20]; eLightsState GetLightStatus(eLights light) const; };
struct CPed;
struct CAutomobile;
struct CEntity : CPlaceable {};
struct CVehicle : CEntity {
    uint8 _p0[0x22 - sizeof(CPlaceable)];
    int16 m_nModelIndex;
    uint8 _p1[0x36 - 0x24];
    struct { uint8 m_nType : 3; uint8 m_nStatus : 5; } m_info;
    uint8 _p1b[0x44 - 0x37];
    CVector m_vecMoveSpeed;
    uint8 _p2[0x384 - 0x50];
    tHandlingData* m_pHandlingData;
    uint8 _p3[0x428 - 0x388];
    union { uint32 m_nVehicleLowerFlags; struct { uint32 bIsLawEnforcer : 1, bIsAmbulanceOnDuty : 1, bIsFireTruckOnDuty : 1, bIsLocked : 1, bEngineOn : 1, bIsHandbrakeOn : 1, bLightsOn : 1, bFreebies : 1; } vehicleFlags; };
    uint8 _p4[0x45C - 0x42C];
    uint16 m_nAlarmState;
    uint8 _p4b[0x460 - 0x45E];
    CPed* m_pDriver;
    uint8 _p5[0x4A0 - 0x464];
    float m_BrakePedal;
    uint8 _p6[0x4A8 - 0x4A4];
    uint8 m_nBombOnBoard : 3;
    uint8 m_nOverrideLights : 2;
    uint8 _p7[0x584 - 0x4A9];
    union { uint8 m_nRenderLightsFlags; struct { uint8 m_bRightFront : 1; uint8 m_bLeftFront : 1; uint8 m_bRightRear : 1; uint8 m_bLeftRear : 1; } m_renderLights; };
    uint8 _p8[0x590 - 0x585];
    eVehicleType m_nVehicleType;
    eVehicleType m_nVehicleSubType;
    uint8 _p9[0x5A0 - 0x598];
    static inline bool& ms_forceVehicleLightsOff = *reinterpret_cast<bool*>(0xC1CC18);
    CVehicleModelInfo* GetVehicleModelInfo() const { return *reinterpret_cast<CVehicleModelInfo**>(0xA9B0C8 + 4 * static_cast<int>(m_nModelIndex)); }
    CAutomobile* AsAutomobile() { return reinterpret_cast<CAutomobile*>(this); }
    eEntityStatus GetStatus() const { return static_cast<eEntityStatus>(m_info.m_nStatus); }
    bool IsSubTrain() const { return m_nVehicleSubType == VEHICLE_TYPE_TRAIN; }
    bool IsAutomobile() const { return m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE; }
    bool CanUpdateHornCounter() { return m_nAlarmState == 0 || m_nAlarmState == 0xFFFF || m_info.m_nStatus == STATUS_WRECKED; }
    bool GetVehicleLightsStatus();
    bool DoHeadLightEffect(int32 lightIndex, CMatrix& vehicleMatrix, uint8 rightSide, uint8 disabled);
    bool DoTailLightEffect(int32 lightIndex, CMatrix& matrix, uint8 rightSide, uint8 disabled, uint32 unused, uint8 lightsOn);
    void DoHeadLightReflection(CMatrix& matrix, uint32 flags, bool left, bool right);
    void DoVehicleLights(CMatrix& matrix, eVehicleLightsFlags flags);
};
struct CAutomobile : CVehicle {
    CDamageManager m_damageManager;
    uint8 _pa[0x958 - 0x5A0 - sizeof(CDamageManager)];
    float m_fPropRotate;
    uint8 _pb[0x988 - 0x95C];
};
static_assert(offsetof(CVehicle, m_nModelIndex) == 0x22 && offsetof(CVehicle, m_pHandlingData) == 0x384 && offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_BrakePedal) == 0x4A0
              && offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(CAutomobile, m_damageManager) == 0x5A0 && offsetof(CVehicle, m_nVehicleLowerFlags) == 0x428
              && offsetof(CVehicle, m_vecMoveSpeed) == 0x44 && offsetof(CVehicle, m_nAlarmState) == 0x45C && offsetof(CVehicle, m_nRenderLightsFlags) == 0x584 && offsetof(CVehicle, m_nVehicleType) == 0x590
              && offsetof(CAutomobile, m_fPropRotate) == 0x958 && sizeof(CAutomobile) == 0x988 && offsetof(CVehicle, m_info) == 0x36);

struct CTimer {
    static inline float& ms_fTimeStep = *reinterpret_cast<float*>(0xB7CB5C);
    static uint32 GetTimeInMS() { return *reinterpret_cast<uint32*>(0xB7CB84); }
};
struct CCam { uint8 _p0[0xC]; eCamMode m_nMode; uint8 _p1[0x238 - 0xE]; };
struct CCamera : CPlaceable {
    uint8 _p0[0x59 - sizeof(CPlaceable)]; uint8 m_nActiveCam; uint8 _p1[0xF0 - 0x5A];
    float m_fLODDistMultiplier;
    uint8 _p2[0x174 - 0xF4];
    std::array<CCam, 3> m_aCams;
    uint8 tail[0xD78 - 0x174 - 3 * 0x238];
    static CCam& GetActiveCamera();
};
static_assert(offsetof(CCamera, m_aCams) == 0x174 && offsetof(CCamera, m_nActiveCam) == 0x59 && offsetof(CCamera, m_fLODDistMultiplier) == 0xF0 && sizeof(CCamera) == 0xD78);
inline CCamera& TheCamera = *reinterpret_cast<CCamera*>(0xB6F028);
CVehicle* FindPlayerVehicle(int32 playerId = -1, bool bIncludeRemote = false);
struct CCoronas {
    static void RegisterCorona(uint32 id, void* attachTo, uint8 r, uint8 g, uint8 b, uint8 intensity, const CVector& pos, float size, float range,
                               eCoronaType coronaType, eCoronaFlareType flareType, eCoronaReflType reflType, eCoronaLOSCheck checkLOS, eCoronaTrail usesTrails,
                               float normalAngle, bool neonFade, float pullTowardsCam, bool fullBrightAtStart, float fadeSpeed, bool onlyFromBelow, bool whiteCore);
};
struct CPointLights {
    static void AddLight(uint8 lightType, CVector point, CVector direction, float radius, float red, float green, float blue, uint8 fogType = 0, bool generateExtraShadows = false, CEntity* entityAffected = nullptr);
};
