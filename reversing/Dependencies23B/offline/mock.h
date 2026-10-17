// Tipos falsos con los desplazamientos reales del EXE, solo para compilar CVehicle::FlyingControl fuera del proyecto (arnes sin juego).
#pragma once
#define NOMINMAX
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
#ifdef FLC_ANNEX
#include "X87Math.h"
#else
#include "../soft/flc_soft.h"
#include "../soft/flc_macros.h"
#endif

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
};
struct CMatrixLink : CMatrix { uint32 tail[2]; };
static_assert(offsetof(CMatrix, m_forward) == 0x10 && offsetof(CMatrix, m_up) == 0x20 && offsetof(CMatrix, m_pos) == 0x30 && sizeof(CMatrixLink) == 0x48);

enum eFlightModel : int32 {
    FLIGHT_MODEL_UNK = 0, FLIGHT_MODEL_BARON = 1, FLIGHT_MODEL_RC = 2, FLIGHT_MODEL_PLANE = 3, FLIGHT_MODEL_UNK2 = 4, FLIGHT_MODEL_BOAT = 5, FLIGHT_MODEL_HELI = 6,
};
enum eEntityStatus : uint8 { STATUS_PLAYER = 0, STATUS_PHYSICS = 4 };

struct tFlyingHandlingData {
    int32 m_nVehicleId;
    float m_fThrust, m_fThrustFallOff, m_fYaw, m_fYawStab, m_fSideSlip, m_fRoll, m_fRollStab, m_fPitch, m_fPitchStab, m_fFormLift, m_fAttackLift, m_fGearUpR, m_fGearDownL, m_fWindMult, m_fMoveRes;
    CVector m_vecTurnRes;
    CVector m_vecSpeedRes;
};
static_assert(sizeof(tFlyingHandlingData) == 0x58 && offsetof(tFlyingHandlingData, m_vecTurnRes) == 0x40 && offsetof(tFlyingHandlingData, m_vecSpeedRes) == 0x4C && offsetof(tFlyingHandlingData, m_fWindMult) == 0x38);

struct CPad;
struct CPlayerPed { CPad* GetPadFromPlayer() const; };
struct CPed { bool IsPlayer() const; CPlayerPed* AsPlayer() { return reinterpret_cast<CPlayerPed*>(this); } };

struct CTimerMock { float& ms_fTimeStep = *reinterpret_cast<float*>(0xB7CB5C); };
struct CTimer { static inline float& ms_fTimeStep = *reinterpret_cast<float*>(0xB7CB5C); };
struct CWeather { static inline CVector& WindDir = *reinterpret_cast<CVector*>(0xC813E0); };

struct CVehicle {
    uint8 _a0[4];               CVector m_vPosn;                       // 0x04
    uint8 _p0[0x14 - 0x10];     CMatrixLink* m_matrix;                 // 0x14
    uint8 _p1[0x22 - 0x18];     uint16 m_nModelIndex;                  // 0x22
    uint8 _p2[0x36 - 0x24];     uint8 m_nTypeStatus;                   // 0x36
    uint8 _p3[0x44 - 0x37];     CVector m_vecMoveSpeed;                // 0x44
    CVector m_vecTurnSpeed;                                            // 0x50
    uint8 _p4[0x8C - 0x5C];     float m_fMass;                         // 0x8C
    float m_fTurnMass;                                                 // 0x90
    uint8 _p5[0xA4 - 0x94];     CVector m_vecCentreOfMass;             // 0xA4
    uint8 _p6[0x388 - 0xB0];    tFlyingHandlingData* m_pFlyingHandlingData;   // 0x388
    uint8 _p7[0x460 - 0x38C];   CPed* m_pDriver;                       // 0x460
    uint8 _p8[0x4A4 - 0x464];   uint8 m_nCreatedBy;                    // 0x4A4
    uint8 _p9[0xA20 - 0x4A5];

    void* GetColModel();
    CVector& GetPosition() { return m_matrix ? m_matrix->m_pos : m_vPosn; }
    eEntityStatus GetStatus() const { return eEntityStatus(m_nTypeStatus >> 3); }
    void FlyingControl(eFlightModel flightModel, float leftRightSkid, float steeringUpDown, float steeringLeftRight, float accelerationBreakStatus);
    void ApplyTurnForce(CVector force, CVector point);
    void ApplyMoveForce(CVector force);
    CVector GetSpeed(CVector point);
    float HeightAboveCeiling(float z, eFlightModel model);
};
static_assert(offsetof(CVehicle, m_matrix) == 0x14 && offsetof(CVehicle, m_nTypeStatus) == 0x36 && offsetof(CVehicle, m_vecTurnSpeed) == 0x50 && offsetof(CVehicle, m_fTurnMass) == 0x90 &&
              offsetof(CVehicle, m_vecCentreOfMass) == 0xA4 && offsetof(CVehicle, m_pFlyingHandlingData) == 0x388 && offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_nCreatedBy) == 0x4A4);
