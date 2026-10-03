/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlayerPedData
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedClothesDesc.h"

void CPlayerPedData::InjectHooks() {
    RH_ScopedClass(CPlayerPedData);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x56F810);
    RH_ScopedInstall(Destructor, 0x571840);
    RH_ScopedInstall(AllocateData, 0x571880);
    RH_ScopedInstall(DeAllocateData, 0x56F890);
    RH_ScopedInstall(SetInitialState, 0x56F210);
}

// 0x56F810
CPlayerPedData::CPlayerPedData() {
    m_pWanted = nullptr;
    m_pPedClothesDesc = nullptr;
    m_nMeleeWeaponAnimReferenced = 0;
    m_nMeleeWeaponAnimReferencedExtra = 0;

    SetInitialState();

    m_pSpecialAtomic = nullptr;

    m_nFireHSMissilePressedTime = 0;
    m_LastHSMissileTarget = nullptr;

    m_nLastHSMissileLOSTime = 0;
    m_bLastHSMissileLOS = true;
}

CPlayerPedData *CPlayerPedData::Constructor() {
    this->CPlayerPedData::CPlayerPedData();
    return this;
}

// 0x571840
CPlayerPedData::~CPlayerPedData() {
    DeAllocateData();
}

CPlayerPedData *CPlayerPedData::Destructor() {
    this->CPlayerPedData::~CPlayerPedData();
    return this;
}

// 0x571880
void CPlayerPedData::AllocateData() {
    if (!m_pWanted) {
        // Como el original: constructor de CWanted inlinado; solo escribe la vtable de CAEPoliceScannerAudioEntity (+0x21C)
        // y el puntero de entidad física de su sonido temporal (+0x22C). `new CWanted()` pondría a cero los 0x29C bytes.
        auto* const wanted = static_cast<uint8*>(::operator new(sizeof(CWanted)));
        if (wanted) {
            *reinterpret_cast<uint32*>(wanted + 0x22C) = 0;
            *reinterpret_cast<uint32*>(wanted + 0x21C) = 0x85F368;
        }
        m_pWanted = reinterpret_cast<CWanted*>(wanted);
    }
    m_pWanted->Initialise();

    if (!m_pPedClothesDesc) {
      m_pPedClothesDesc = new CPedClothesDesc;
    }
    m_pPedClothesDesc->Initialise();
}

//  0x56F890
void CPlayerPedData::DeAllocateData() {
    // Como el original: operator delete solo se llama con punteros no nulos (la expresión delete de MSVC lo llamaría con null)
    if (m_pWanted) {
        delete m_pWanted;
    }
    m_pWanted = nullptr;

    if (m_pPedClothesDesc) {
        delete m_pPedClothesDesc;
    }
    m_pPedClothesDesc = nullptr;
}

// 0x56F210
void CPlayerPedData::SetInitialState() {
    m_pArrestingCop = nullptr;
    m_vecFightMovement = CVector2D(0.0f, 0.0f);
    m_vecTargetBoneOffset = CVector(0.0f, 0.0f, 0.0f);
    m_fMoveBlendRatio = 0.0f;
    m_nChosenWeapon = 0;
    m_nStandStillTimer = 0;
    m_nCarDangerCounter = 0;
    m_pDangerCar = nullptr;
    m_nHitAnimDelayTimer = 0;
    m_fAttackButtonCounter = 0;
    m_nAdrenalineEndTime = 0;
    m_nDrunkenness = 0;
    m_nFadeDrunkenness = 0;
    m_nDrugLevel = 0;
    m_nScriptLimitToGangSize = 99;
    m_fFPSMoveHeading = 0;
    m_fLookPitch = 0;
    m_fSkateBoardSpeed = 0;
    m_fGunSpinSpeed = 0.0f;
    m_fGunSpinAngle = 0;
    m_nLastTimeFiring = 0;
    m_nTargetBone = 0;
    m_nBusFaresCollected = 0;
    m_bPlayerSprintDisabled = false;
    m_bDontAllowWeaponChange = false;
    m_bForceInteriorLighting = false;
    m_nPadDownPressedInMilliseconds = 0;
    m_nPadUpPressedInMilliseconds = 0;
    m_bPlayersGangActive = true;
    m_nWetness = 0;
    m_fTimeCanRun = CStats::GetFatAndMuscleModifier(STAT_MOD_TIME_CAN_RUN);
    m_fMoveSpeed = 0.0f;
    m_fBreath = CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG);

    m_nFireHSMissilePressedTime = 0;
    m_LastHSMissileTarget = nullptr;

    m_pCurrentProstitutePed = nullptr;
    m_pLastProstituteShagged = nullptr;

    // Como el original: (flags & 0xFFFFF7B0) | 0x1010 y después & ~0x80: limpia bStoppedMoving, bAdrenaline,
    // bHaveTargetSelected, bFreeAiming, bJustBeenSnacking, bRequireHandleBreath y bInVehicleDontAllowWeaponChange,
    // y activa bCanBeDamaged y bRenderWeapon
    m_nPlayerFlags = (m_nPlayerFlags & 0xFFFFF7B0u) | 0x1010u;
    m_nPlayerFlags &= ~0x80u;

    m_nLastHSMissileLOSTime = 0;
    m_bLastHSMissileLOS = true;

    m_nModelIndexOfLastBuildingShot = -1;
}
