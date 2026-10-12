#pragma once

#include "Vector.h"
#include "Weapon.h"
#include "Ped.h"

class CPedSaveStructure {
public:
    CVector       m_pos;
    float         m_health;
    float         m_armor;
    CWeapon       m_weapons[13];
    ePedCreatedBy m_createdBy;
    int8          m_activeWeaponSlot;
    int8          m_areaCode;
    int32         m_nExitIndex;
    uint8         m_nFightingStyle;
    uint8         m_nAllowedAttackMoves;

    static void InjectHooks();
    void Extract(CPed* ped);
    // 0x5D43D0: devuelve EDX:EAX como el EXE (residuo de sus registros; ECX queda a 0 tras el rep movsd)
    uint64 Construct(CPed* ped);
};
VALIDATE_SIZE(CPedSaveStructure, 0x18C);
