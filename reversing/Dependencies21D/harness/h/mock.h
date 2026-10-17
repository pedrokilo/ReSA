// Tipos falsos con los desplazamientos reales del EXE, solo para compilar el codigo de TaskSimpleGangDriveBy.cpp fuera del proyecto.
#pragma once
#include <windows.h>
#include <array>
#include <bit>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t;
using int8 = std::int8_t; using int16 = std::int16_t; using int32 = std::int32_t;
#include "X87Math.h"

struct CVector {
    float x{}, y{}, z{};
    CVector() = default;
    CVector(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};
struct CVector2D { float x, y; };
struct CMatrix {
    CVector m_right; uint32 f0;
    CVector m_forward; uint32 f1;
    CVector m_up; uint32 f2;
    CVector m_pos; uint32 f3;
    CVector& GetForward() { return m_forward; }
    CVector& GetPosition() { return m_pos; }
};
struct CMatrixLink : CMatrix { uint32 tail[2]; };
static_assert(offsetof(CMatrix, m_forward) == 0x10 && offsetof(CMatrix, m_pos) == 0x30);

struct CSimpleTransform {
    CVector m_vPosn; float m_fHeading;
    void UpdateMatrix(CMatrix*);
};
struct CPlaceable {
    void* vptr_;
    CSimpleTransform m_placement;
    CMatrixLink* m_matrix;
    void AllocateMatrix();
    CMatrix& GetMatrix() {
        if (!m_matrix) { AllocateMatrix(); m_placement.UpdateMatrix(m_matrix); }
        return *m_matrix;
    }
    CVector& GetPosition() { return m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn; }
};
static_assert(offsetof(CPlaceable, m_matrix) == 0x14);

enum eEntityType : uint8 { ENTITY_TYPE_NOTHING = 0, ENTITY_TYPE_BUILDING, ENTITY_TYPE_VEHICLE, ENTITY_TYPE_PED, ENTITY_TYPE_OBJECT, ENTITY_TYPE_DUMMY, ENTITY_TYPE_NOTINPOOLS };
struct CPed; struct CVehicle;
struct CEntity : CPlaceable {
    uint8 _pad[0x36 - sizeof(CPlaceable)];
    uint8 m_infoByte;
    uint8 _pad2[0x40 - 0x37];
    eEntityType GetType() const { return eEntityType(m_infoByte & 7); }
    CPed* AsPed() { return reinterpret_cast<CPed*>(this); }
    CVehicle* AsVehicle() { return reinterpret_cast<CVehicle*>(this); }
    static void SafeRegisterRef(CEntity*& ref);
};
static_assert(offsetof(CEntity, m_infoByte) == 0x36);

enum eBoneTag : int16 { BONE_SPINE1 = 3, BONE_R_UPPER_ARM = 22 };
struct CPedIK {
    uint8 pad[0x1C];
    uint32 m_nFlags;
    bool PointGunInDirection(float zAngle, float distance, bool flag, float normalize);
};
struct CPlayerPedData { uint8 pad[0x34]; union { uint32 flags; struct { uint32 b0 : 1, b1 : 1, b2 : 1, m_bFreeAiming : 1; }; }; uint8 tail[0xAC - 0x38]; };
struct CPlayerInfo { uint8 pad[0x153]; bool m_bCanDoDriveBy; uint8 tail[0x190 - 0x154]; };
static_assert(sizeof(CPlayerInfo) == 0x190 && sizeof(CPlayerPedData) == 0xAC);
struct CPlayerPed;
struct CVehicle : CEntity {
    uint8 _p[0x4C0 - sizeof(CEntity)];
    float m_fHealth;
    uint8 _p2[0x590 - 0x4C4];
    int m_nVehicleType;
    uint8 tail[0xA20 - 0x594];
};
static_assert(offsetof(CVehicle, m_fHealth) == 0x4C0 && offsetof(CVehicle, m_nVehicleType) == 0x590);
struct CPed : CEntity {
    uint8 _p[0x480 - sizeof(CEntity)];
    CPlayerPedData* m_pPlayerData;
    uint8 _p2[0x50C - 0x484];
    CPedIK m_pedIK;
    uint8 _p3[0x540 - 0x50C - sizeof(CPedIK)];
    float m_fHealth;
    uint8 _p4[0x58C - 0x544];
    CVehicle* m_pVehicle;
    uint8 tail[0x7A0 - 0x590];
    bool IsPlayer() const;
    CPlayerPedData* GetPlayerData() const { return m_pPlayerData; }
    CPlayerPed* AsPlayer() { return reinterpret_cast<CPlayerPed*>(this); }
    void GetBonePosition(CVector* outVec, eBoneTag bone, bool updateSkinBones);
};
static_assert(offsetof(CPed, m_pPlayerData) == 0x480 && offsetof(CPed, m_pedIK) == 0x50C && offsetof(CPed, m_fHealth) == 0x540 && offsetof(CPed, m_pVehicle) == 0x58C);
struct CPlayerPed : CPed { CPlayerInfo* GetPlayerInfoForThisPlayerPed(); };

enum eCamMode : uint16 { MODE_TWOPLAYER_IN_CAR_AND_SHOOTING = 49, MODE_AIMWEAPON_FROMCAR = 55 };
struct CCam {
    uint8 _p0[0xC]; eCamMode m_nMode; uint8 _p1[0x190 - 0xE];
    CVector m_vecFront; CVector m_vecSource; uint8 _p2[0x238 - 0x1A8];
    CEntity* Get_TwoPlayer_AimVector(CVector&);
};
static_assert(sizeof(CCam) == 0x238 && offsetof(CCam, m_vecFront) == 0x190 && offsetof(CCam, m_vecSource) == 0x19C);
struct CCamera {
    uint8 _p0[0x59]; uint8 m_nActiveCam; uint8 _p1[0x174 - 0x5A];
    std::array<CCam, 3> m_aCams;
    uint8 tail[0xD7C - 0x174 - 3 * 0x238];
    void SetNewPlayerWeaponMode(eCamMode mode, int16 maxZoom, int16 minZoom);
    void Find3rdPersonCamTargetVector(float range, CVector vecGunMuzzle, CVector& outSource, CVector& outTarget);
};
static_assert(offsetof(CCamera, m_aCams) == 0x174 && offsetof(CCamera, m_nActiveCam) == 0x59);
inline CCamera& TheCamera = *reinterpret_cast<CCamera*>(0xB6F028);
struct CGameLogic { static bool IsCoopGameGoingOn(); };
namespace CGeneral {
float GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2);
float GetRadianAngleBetweenPoints(CVector2D a, CVector2D b);
}
struct CWeaponInfo { uint8 pad[8]; float m_fWeaponRange; };
struct CAnimBlendAssociation { uint8 pad[0x2C]; int16 m_AnimId; };
static_assert(offsetof(CAnimBlendAssociation, m_AnimId) == 0x2C);

namespace plugin {
template <typename Ret, unsigned int address, typename... Args> Ret CallAndReturn(Args... args) { return reinterpret_cast<Ret(__cdecl*)(Args...)>(address)(args...); }
template <typename Ret, unsigned int address, typename C, typename... Args>
Ret CallMethodAndReturn(C _this, Args... args) { return reinterpret_cast<Ret(__thiscall*)(C, Args...)>(address)(_this, args...); }
}
enum { ANIM_ID_NO_ANIMATION_SET = 191, ANIM_GROUP_DEFAULT = 0 };
enum class eDrivebyStyle : int8 { FIXED_LHS, FIXED_RHS, START_FROM_LHS, START_FROM_RHS, AI_SIDE, FIXED_FWD, FIXED_BAK, AI_FWD_BAK, AI_ALL_DIRN };

struct CTaskSimpleGangDriveBy {
    void* vptr_{}; void* m_pParentTask{};
    bool m_bIsFinished, m_bAnimsReferenced, m_bSeatRHS, m_bInRangeToShoot, m_bInWeaponRange, m_bReachedAbortRange, m_bFromScriptCommand;
    char m_nNextCommand, m_nLastCommand, m_nBurstShots;
    eDrivebyStyle m_nDrivebyStyle;
    int8 m_nFrequencyPercentage;
    char m_nFakeShootDirn;
    int16 m_nAttackTimer;
    uint32 m_nLOSCheckTime;
    bool m_nLOSBlocked;
    float m_fAbortRange;
    int32 m_nRequiredAnimID;
    int32 m_nRequiredAnimGroup;
    CAnimBlendAssociation* m_pAnimAssoc;
    CWeaponInfo* m_pWeaponInfo;
    CEntity* m_pTargetEntity;
    CVector m_vecCoords;
    CTaskSimpleGangDriveBy(CEntity* target, const CVector* targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS);
    CTaskSimpleGangDriveBy* Constructor(CEntity* target, const CVector* targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS);
    void PlayerTarget(CPed* ped);
    void AITarget(CPed* ped);
    void AimGun(CPed* ped);
};
static_assert(sizeof(CTaskSimpleGangDriveBy) == 0x44 && offsetof(CTaskSimpleGangDriveBy, m_vecCoords) == 0x38 && offsetof(CTaskSimpleGangDriveBy, m_pTargetEntity) == 0x34);
