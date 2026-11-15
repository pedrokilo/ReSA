#include "StdInc.h"

#include "PlayerInfoSaveStructure.h"

void CPlayerInfoSaveStructure::InjectHooks() {
    RH_ScopedClass(CPlayerInfoSaveStructure);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Construct, 0x5D24B0); // lote 196A (sin validar)
    RH_ScopedInstall(Extract, 0x5D2560);   // lote 196A (sin validar)
}

// 0x5D24B0 (lote 196A, sin validar; el listado decia CVehicleSaveStructure: es la estructura de guardado de CPlayerInfo, la que llena CPlayerInfo::Save en 0x5D3AD0; thiscall, un argumento de pila, `ret 4`, 172 bytes).
// Copia campo a campo (el relleno de la estructura no se toca): dinero (+0xB8) a +0x0, densidad de coches (+0x130, WORD) a +0x4, estado del jugador (+0xDC, BYTE) a +0x6, densidad de carretera (+0x134) a +0x8,
// dinero mostrado (+0xBC) a +0xC, horas sin comer (+0x144, solo el BYTE bajo del WORD) a +0x10, coleccionables recogidos (+0xC0) a +0x14, total (+0xC4) a +0x18, nueve bytes de banderas (+0x14C..+0x154) a +0x1C..+0x24
// y el ultimo mensaje de arresto (+0x156, WORD) a +0x26. Sin x87 ni colaboradores.
void CPlayerInfoSaveStructure::Construct(CPlayerInfo* info) {
    static_assert(offsetof(CPlayerInfo, m_nMoney) == 0xB8 && offsetof(CPlayerInfo, m_nDisplayMoney) == 0xBC);
    static_assert(offsetof(CPlayerInfo, m_nCollectablesPickedUp) == 0xC0 && offsetof(CPlayerInfo, m_nTotalNumCollectables) == 0xC4);
    static_assert(offsetof(CPlayerInfo, m_nPlayerState) == 0xDC && offsetof(CPlayerInfo, m_nCarDensityForCurrentZone) == 0x130);
    static_assert(offsetof(CPlayerInfo, m_fRoadDensityAroundPlayer) == 0x134 && offsetof(CPlayerInfo, m_nNumHoursDidntEat) == 0x144);
    static_assert(offsetof(CPlayerInfo, m_bDoesNotGetTired) == 0x14C && offsetof(CPlayerInfo, m_nBustedAudioStatus) == 0x154);
    static_assert(offsetof(CPlayerInfo, m_nLastBustMessageNumber) == 0x156);

    m_nMoney                    = info->m_nMoney;
    m_nCarDensityForCurrentZone = info->m_nCarDensityForCurrentZone;
    m_nPlayerState              = info->m_nPlayerState;
    m_fRoadDensityAroundPlayer  = info->m_fRoadDensityAroundPlayer;
    m_nDisplayMoney             = info->m_nDisplayMoney;
    m_nNumHoursDidntEat         = static_cast<uint8>(info->m_nNumHoursDidntEat);
    m_nCollectablesPickedUp     = info->m_nCollectablesPickedUp;
    m_nTotalNumCollectables     = info->m_nTotalNumCollectables;
    m_bDoesNotGetTired          = info->m_bDoesNotGetTired;
    m_bFastReload               = info->m_bFastReload;
    m_bFireProof                = info->m_bFireProof;
    m_nMaxHealth                = info->m_nMaxHealth;
    m_nMaxArmour                = info->m_nMaxArmour;
    m_bGetOutOfJailFree         = info->m_bGetOutOfJailFree;
    m_bFreeHealthCare           = info->m_bFreeHealthCare;
    m_bCanDoDriveBy             = info->m_bCanDoDriveBy;
    m_nBustedAudioStatus        = info->m_nBustedAudioStatus;
    m_nLastBustMessageNumber    = info->m_nLastBustMessageNumber;
}

// 0x5D2560 (lote 196A, sin validar; thiscall, un argumento de pila, `ret 4`, 186 bytes): inversa de Construct. El EXE escribe dos veces el WORD de densidad de coches (+0x130; el mismo valor) y las horas sin comer
// (+0x144) como WORD con el BYTE de la estructura extendido a cero (`movzx dx, byte [ecx+10h]; mov [eax+144h], dx`); los dos bytes altos de ese campo del jugador quedan a cero.
void CPlayerInfoSaveStructure::Extract(CPlayerInfo* info) {
    info->m_nMoney                    = m_nMoney;
    info->m_nCarDensityForCurrentZone = m_nCarDensityForCurrentZone;
    info->m_nPlayerState              = m_nPlayerState;
    info->m_fRoadDensityAroundPlayer  = m_fRoadDensityAroundPlayer;
    info->m_nDisplayMoney             = m_nDisplayMoney;
    info->m_nNumHoursDidntEat         = m_nNumHoursDidntEat;
    info->m_nCollectablesPickedUp     = m_nCollectablesPickedUp;
    info->m_nTotalNumCollectables     = m_nTotalNumCollectables;
    info->m_bDoesNotGetTired          = m_bDoesNotGetTired;
    info->m_bFastReload               = m_bFastReload;
    info->m_bFireProof                = m_bFireProof;
    info->m_nMaxHealth                = m_nMaxHealth;
    info->m_nMaxArmour                = m_nMaxArmour;
    info->m_bGetOutOfJailFree         = m_bGetOutOfJailFree;
    info->m_bFreeHealthCare           = m_bFreeHealthCare;
    info->m_bCanDoDriveBy             = m_bCanDoDriveBy;
    info->m_nBustedAudioStatus        = m_nBustedAudioStatus;
    info->m_nLastBustMessageNumber    = m_nLastBustMessageNumber;
}
