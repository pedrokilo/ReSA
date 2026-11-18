/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlayerPedData
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/6 entradas.
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
    RH_ScopedInstall(operator=, 0x45C4B0); // lote 214A (sin validar)
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

// 0x56F810
CPlayerPedData *CPlayerPedData::Constructor() {
    this->CPlayerPedData::CPlayerPedData();
    return this;
}

// 0x571840
CPlayerPedData::~CPlayerPedData() {
    DeAllocateData();
}

// 0x571840
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

void PlayerPedDataAssignExe(CPlayerPedData* self, const CPlayerPedData* rhsPtr, uint32 esi);

// 0x45C4B0 (lote 214A, sin validar; el listado decia CReplay): operator= de CPlayerPedData del EXE (thiscall, un puntero de origen en la pila, `ret 4`, 537 bytes; devuelve this). Su unico llamador es
// CPlayerInfo::operator= (0x45DEF0). Copia SOLO los campos, en orden creciente, con el ancho del EXE (dwords, bytes y words); no copia el relleno (+0x22/+0x23, +0x87, +0x8F).
// Las banderas de +0x34 las mezcla con ESI sin inicializar (en esta funcion ESI es el del llamador, que es el CPlayerInfo, this - 4): bit 9 con el valor de destino, bits 10-12 con el de origen y
// el resto con ESI. El bit 31 de +0xA0 acaba con el valor de origen (el EXE lo mezcla en dos pasos y el resultado es el dword de origen).
CPlayerPedData& CPlayerPedData::operator=(const CPlayerPedData& rhs) {
    PlayerPedDataAssignExe(this, &rhs, static_cast<uint32>(reinterpret_cast<uintptr_t>(this) - 4u));
    return *this;
}

// Cuerpo de 0x45C4B0 con el ESI del llamador explicito (el EXE lo lee sin tocarlo): CPlayerInfo::operator= (0x45DEF0) pasa su this, que es el que tiene ESI en el EXE. Sin insertar en linea.
__declspec(noinline) void PlayerPedDataAssignExe(CPlayerPedData* self, const CPlayerPedData* rhsPtr, uint32 esi) {
    auto* const dst = reinterpret_cast<uint8*>(self);
    const auto* const src = reinterpret_cast<const uint8*>(rhsPtr);
    const auto copy = [&](size_t off, size_t size) { memcpy(dst + off, src + off, size); };

    copy(0x00, 0x22); // 0x00..0x1F dwords + bytes 0x20/0x21
    copy(0x24, 0x10); // 0x24..0x33 dwords
    {
        // Banderas +0x34 (0x45C508..0x45C547): cuatro pasos de lectura-modificacion-escritura; los tres ultimos RELEEN el origen del EXE (si origen == destino ven lo ya escrito).
        uint32 oldFlags;
        memcpy(&oldFlags, dst + 0x34, 4);
        uint32 v = ((oldFlags ^ esi) & 0x200u) ^ esi;
        memcpy(dst + 0x34, &v, 4);
        for (const uint32 mask : {0x400u, 0x800u, 0x1000u}) {
            uint32 s;
            memcpy(&s, src + 0x34, 4);
            v = ((s ^ v) & mask) ^ v;
            memcpy(dst + 0x34, &v, 4);
        }
    }
    copy(0x38, 0x08); // 0x38, 0x3C
    copy(0x40, 0x04); // cuatro bytes
    copy(0x44, 0x30); // 0x44..0x73 dwords
    copy(0x74, 0x0C); // tres dwords
    copy(0x80, 0x07); // dword 0x80 + bytes 0x84..0x86
    copy(0x88, 0x07); // words 0x88/0x8A + bytes 0x8C..0x8E
    copy(0x90, 0x10); // 0x90..0x9F dwords
    copy(0xA0, 0x04); // dword completo (ver arriba)
    copy(0xA4, 0x08); // 0xA4, 0xA8
}
