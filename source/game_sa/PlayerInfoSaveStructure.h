#pragma once

#include "PlayerInfo.h"

// Estructura de guardado de CPlayerInfo (0x28 bytes). Construct (0x5D24B0) la llena desde el jugador y Extract (0x5D2560) la vuelca de vuelta; las llaman CPlayerInfo::Save (0x5D3AC0) y CPlayerInfo::Load (0x5D3B00).
class CPlayerInfoSaveStructure {
    int32        m_nMoney;
    uint16       m_nCarDensityForCurrentZone;
    ePlayerState m_nPlayerState;
    float        m_fRoadDensityAroundPlayer;
    int32        m_nDisplayMoney;
    uint8        m_nNumHoursDidntEat;
    uint32       m_nCollectablesPickedUp;
    uint32       m_nTotalNumCollectables;
    bool         m_bDoesNotGetTired;
    bool         m_bFastReload;
    bool         m_bFireProof;
    uint8        m_nMaxHealth;
    uint8        m_nMaxArmour;
    bool         m_bGetOutOfJailFree;
    bool         m_bFreeHealthCare;
    bool         m_bCanDoDriveBy;
    uint8        m_nBustedAudioStatus;
    uint16       m_nLastBustMessageNumber;

public:
    static void InjectHooks();

    void Construct(CPlayerInfo* info); // 0x5D24B0
    void Extract(CPlayerInfo* info);   // 0x5D2560
};
VALIDATE_SIZE(CPlayerInfoSaveStructure, 0x28);
