/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CLinkedUpgradeList
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CVehicleModelInfo
 * Reconstrucción: COMPLETA — 75/75 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 75/75 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"
#include "X87Math.h"

#include "VehicleModelInfo.h"
#include "CustomCarPlateMgr.h"
#include "LoadingScreen.h"
#include "CarFXRenderer.h"

auto& vehicleTxd = StaticRef<RwTexDictionary*>(0xB4E688);
auto& carFrame = StaticRef<RwFrame*>(0xB4E6B8);
auto& gLightSurfProps = StaticRef<RwSurfaceProperties>(0x8A645C);
auto& gRestoreEntries = StaticRef<tRestoreEntry[NUM_RESTORE_ENTRIES]>(0xB4DBE8);
auto& gpWhiteTexture = StaticRef<RwTexture*>(0xB4E3EC);
auto& fEnvMapDefaultCoeff = StaticRef<float>(0x8A7780);
auto& fRearDoubleWheelOffsetFactor = StaticRef<float>(0x8A7784);

/**
 * Registra las operaciones de configuración, extras, materiales y upgrades de vehículos.
 */
void CVehicleModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CVehicleModelInfo, 0x85C5C8, 17);
    RH_ScopedCategory("Models");

    { // ClinkedUpgradeList
        RH_ScopedClass(CLinkedUpgradeList);

        RH_ScopedInstall(AddUpgradeLink, 0x4C74B0);
        RH_ScopedInstall(FindOtherUpgrade, 0x4C74D0);
    }

    RH_ScopedVMTInstall(GetModelType, 0x4C7650);
    RH_ScopedVMTInstall(Init, 0x4C7630);
    RH_ScopedVMTInstall(DeleteRwObject, 0x4C9890);
    RH_ScopedVMTInstall(CreateInstance, 0x4C9680);
    RH_ScopedVMTInstall(SetAnimFile, 0x4C7670);
    RH_ScopedVMTInstall(ConvertAnimFileIndex, 0x4C76D0);
    RH_ScopedVMTInstall(GetAnimFileIndex, 0x4C7660);
    RH_ScopedVMTInstall(SetClump, 0x4C95C0);
    RH_ScopedInstall(SetAtomicRenderCallbacks, 0x4C7B10);
    RH_ScopedInstall(SetVehicleComponentFlags, 0x4C7C10);
    RH_ScopedInstall(GetWheelPosn, 0x4C7D20);
    RH_ScopedInstall(GetOriginalCompPosition, 0x4C7DD0);
    RH_ScopedInstall(ChooseComponent, 0x4C8040);
    RH_ScopedInstall(ChooseSecondComponent, 0x4C8120);
    RH_ScopedInstall(IsUpgradeAvailable, 0x4C8200);
    RH_ScopedInstall(SetVehicleColour, 0x4C84B0);
    RH_ScopedInstall(ChooseVehicleColour, 0x4C8500);
    RH_ScopedInstall(GetNumRemaps, 0x4C86B0);
    RH_ScopedInstall(AddRemap, 0x4C86D0);
    RH_ScopedInstall(SetRenderPipelines, 0x4C8900);
    RH_ScopedInstall(GetCustomCarPlateText, 0x4C8970);
    RH_ScopedInstall(SetCustomCarPlateText, 0x4C8980);
    RH_ScopedInstall(ReduceMaterialsInVehicle, 0x4C8BD0);
    RH_ScopedInstall(SetCarCustomPlate, 0x4C9450);
    RH_ScopedInstall(DisableEnvMap, 0x4C97E0);
    RH_ScopedInstall(SetEnvMapCoeff, 0x4C9800);
    RH_ScopedInstall(GetNumDoors, 0x4C73C0);
    RH_ScopedInstall(PreprocessHierarchy, 0x4C8E60);
    RH_ScopedInstall(SetupLightFlags, 0x4C8C90);
    RH_ScopedInstall(ShutdownLightTexture, 0x4C7470);
    RH_ScopedInstall(GetMaximumNumberOfPassengersFromNumberOfDoors, 0x4C89B0);
    RH_ScopedInstall(UseCommonVehicleTexDicationary, 0x4C75A0);
    RH_ScopedInstall(StopUsingCommonVehicleTexDicationary, 0x4C75C0);
    RH_ScopedInstall(FindTextureCB, 0x4C7510);
    RH_ScopedInstall(MoveObjectsCB, 0x4C7700);
    RH_ScopedInstall(ResetEditableMaterials, 0x4C8460);
    RH_ScopedInstall(SetEditableMaterials, 0x4C8430);
    RH_ScopedOverloadedInstall(SetEditableMaterialsCB, "RpMaterial", 0x4C8220, RpMaterial*(*)(RpMaterial*, void*));
    RH_ScopedOverloadedInstall(SetEditableMaterialsCB, "RpAtomic", 0x4C83E0, RpAtomic * (*)(RpAtomic*, void*));
    RH_ScopedInstall(StoreAtomicUsedMaterialsCB, 0x4C8B60);
    RH_ScopedInstall(HideDamagedAtomicCB, 0x4C7720);
    RH_ScopedInstall(HideAllComponentsAtomicCB, 0x4C7790);
    RH_ScopedInstall(HasAlphaMaterialCB, 0x4C77C0);
    RH_ScopedInstall(SetAtomicRendererCB, 0x4C77E0);
    RH_ScopedInstall(SetAtomicRendererCB_RealHeli, 0x4C7870);
    RH_ScopedInstall(SetAtomicRendererCB_Plane, 0x4C7930);
    RH_ScopedInstall(SetAtomicRendererCB_Boat, 0x4C79A0);
    RH_ScopedInstall(SetAtomicRendererCB_Heli, 0x4C7A30);
    RH_ScopedInstall(SetAtomicRendererCB_Train, 0x4C7AA0);
    RH_ScopedInstall(SetAtomicFlagCB, 0x4C7B90);
    RH_ScopedInstall(ClearAtomicFlagCB, 0x4C7BB0);
    RH_ScopedInstall(AddWheelUpgrade, 0x4C8700);
    RH_ScopedInstall(GetNumWheelUpgrades, 0x4C8740);
    RH_ScopedInstall(GetWheelUpgrade, 0x4C8750);
    RH_ScopedInstall(DeleteVehicleColourTextures, 0x4C8770);
    RH_ScopedInstall(SetDirtTextures, 0x5D5DB0);
    RH_ScopedInstall(ShutdownEnvironmentMaps, 0x4C87D0);
    RH_ScopedInstall(GetMatFXEffectMaterialCB, 0x4C8810);
    RH_ScopedInstall(SetEnvironmentMapCB, 0x4C8840);
    RH_ScopedInstall(SetEnvMapCoeffCB, 0x4C88B0);
    RH_ScopedInstall(SetRenderPipelinesCB, 0x4C88F0); // Antes 0x4C88F4: era el ret; la función es mov eax,[esp+4]; ret
    RH_ScopedInstall(CollapseFramesCB, 0x4C8E30);
    RH_ScopedInstall(SetEnvironmentMapAtomicCB, 0x4C9410);
    RH_ScopedInstall(SetEnvMapCoeffAtomicCB, 0x4C9430);
    RH_ScopedInstall(AssignRemapTxd, 0x4C9360);

// Setup
    RH_ScopedInstall(SetupCommonData, 0x5B8F00);
    RH_ScopedInstall(LoadVehicleColours, 0x5B6890);
    RH_ScopedInstall(LoadVehicleUpgrades, 0x5B65A0);
    RH_ScopedInstall(LoadEnvironmentMaps, 0x4C8780);

// Other
    RH_ScopedGlobalInstall(IsValidCompRule, 0x4C7E10);
    RH_ScopedNamedGlobalInstall(::ChooseComponent, "ChooseComponent-Global", 0x4C7FB0);  // There's a global `ChooseComponent` and `CVehicleModelInfo::ChooseComponent`.. Why?
    RH_ScopedGlobalInstall(CountCompsInRule, 0x4C7F80);
    RH_ScopedGlobalInstall(GetListOfComponentsNotUsedByRules, 0x4C7E50);
    RH_ScopedGlobalInstall(RemoveWindowAlphaCB, 0x4C83B0);
    RH_ScopedGlobalInstall(GetOkAndDamagedAtomicCB, 0x4C7BD0);
}

/**
 * Inicializa colecciones opcionales, remaps y upgrades antes de cargar el clump real.
 */
CVehicleModelInfo::CVehicleModelInfo() : CClumpModelInfo()
{
    m_pVehicleStruct = nullptr;
    m_nNumColorVariations = 0;
    m_nFlags = 0;
    m_nAnimBlockIndex = -1;
    std::ranges::fill(m_anUpgrades, -1);
    std::ranges::fill(m_anRemapTxds, -1);
}

// 0x4C7650
ModelInfoType CVehicleModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_VEHICLE;
}

// 0x4C7630
void CVehicleModelInfo::Init()
{
    CClumpModelInfo::Init();
    m_nVehicleType     = VEHICLE_TYPE_IGNORE;
    m_nWheelModelIndex = -1;
    m_fBikeSteerAngle  = 999.99F;
}

// 0x4C9890
// Libera la estructura derivada de componentes y delega el resto del clump a la clase base.
void CVehicleModelInfo::DeleteRwObject()
{
    delete m_pVehicleStruct;
    m_pVehicleStruct = nullptr;
    CClumpModelInfo::DeleteRwObject();
}

// 0x4C9680
/**
 * Clona el vehículo base y materializa hasta dos extras elegidos por las reglas del modelo.
 *
 * Los atomics extra se clonan con frames propios y se anexan al clump de la instancia;
 * ms_compsUsed registra cuáles fueron seleccionados para el siguiente procesamiento.
 */
RwObject* CVehicleModelInfo::CreateInstance()
{
    auto clump = reinterpret_cast<RpClump*>(CClumpModelInfo::CreateInstance());
    if (m_pVehicleStruct->m_nNumExtras) {
        RwFrame* frame;
        if (!IsBike() && !IsBMX() && m_nVehicleType >= VEHICLE_TYPE_BOAT) {
            frame = RpClumpGetFrame(clump);
        }
        else {
            frame = CClumpModelInfo::GetFrameFromId(clump, 1);
            if (!frame)
                frame = RpClumpGetFrame(clump);
        }

        auto iFirstComp = ChooseComponent();
        // Como el original (0x4C96E2, 0x4C9750): [struct + idx * 4 + 2F4h] sin comprobar el límite del array
        if (iFirstComp != -1 && m_pVehicleStruct->m_apExtras.data()[iFirstComp]) {
            auto& extra = m_pVehicleStruct->m_apExtras.data()[iFirstComp];
            auto atomicClone = RpAtomicClone(extra);
            auto newFrame = RwFrameCreate();
            RwFrameTransform(newFrame, RwFrameGetMatrix(RpAtomicGetFrame(extra)), RwOpCombineType::rwCOMBINEREPLACE);
            RpAtomicSetFrame(atomicClone, newFrame);
            RpClumpAddAtomic(clump, atomicClone);
            RwFrameAddChild(frame, newFrame);
        }
        ms_compsUsed[0] = iFirstComp;

        auto iSecondComp = ChooseSecondComponent();
        if (iSecondComp != -1 && m_pVehicleStruct->m_apExtras.data()[iSecondComp]) {
            auto& extra = m_pVehicleStruct->m_apExtras.data()[iSecondComp];
            auto atomicClone = RpAtomicClone(extra);
            auto newFrame = RwFrameCreate();
            RwFrameTransform(newFrame, RwFrameGetMatrix(RpAtomicGetFrame(extra)), RwOpCombineType::rwCOMBINEREPLACE);
            RpAtomicSetFrame(atomicClone, newFrame);
            RpClumpAddAtomic(clump, atomicClone);
            RwFrameAddChild(frame, newFrame);
        }
        ms_compsUsed[1] = iSecondComp;
    }
    else {
        ms_compsUsed[0] = -1;
        ms_compsUsed[1] = -1;
    }

    RpClumpForAllAtomics(clump, SetEnvironmentMapAtomicCB, nullptr);
    return reinterpret_cast<RwObject*>(clump);
}

// 0x4C7670
// Conserva temporalmente el nombre del AnimBlock hasta resolverlo a índice.
void CVehicleModelInfo::SetAnimFile(const char* filename)
{
    // Como el original (0x4C767E: call 0x8229B6): 0x8229B6 es _stricmp, así que "NULL" también se ignora
    if (!_stricmp(filename, "null")) {
        m_nAnimBlockIndex = -1;
        return;
    }

    const auto size = strlen(filename) + 1;
    auto name = new char[size];
    m_animBlockFileName = name;
    strcpy(name, filename); // Como el original (0x4C76C0): copia byte a byte; strcpy_s rellenaría con 0xFE en Debug
}

// 0x4C76D0
void CVehicleModelInfo::ConvertAnimFileIndex()
{
    if (m_nAnimBlockIndex == -1)
        return;

    auto iIndex = CAnimManager::GetAnimationBlockIndex(m_animBlockFileName);
    delete[] m_animBlockFileName;
    m_nAnimBlockIndex = iIndex;
}

// 0x4C7660
int32 CVehicleModelInfo::GetAnimFileIndex()
{
    return m_nAnimBlockIndex;
}

// 0x4C95C0
/**
 * Instala el clump base, crea CVehicleStructure y preprocesa componentes/render callbacks.
 *
 * A partir de aquí la estructura auxiliar conserva dummies, extras y posiciones de
 * upgrades extraídas de la jerarquía del modelo.
 */
void CVehicleModelInfo::SetClump(RpClump* clump)
{
    m_pVehicleStruct = new CVehicleStructure();
    CClumpModelInfo::SetClump(clump);
    SetAtomicRenderCallbacks();
    CClumpModelInfo::SetFrameIds(ms_vehicleDescs[m_nVehicleType]);
    SetRenderPipelines();
    PreprocessHierarchy();
    ReduceMaterialsInVehicle();
    m_nCurrentPrimaryColor = 255;
    m_nCurrentSecondaryColor = 255;
    m_nCurrentTertiaryColor = 255;
    m_nCurrentQuaternaryColor = 255;
    SetCarCustomPlate();
}

// 0x4C7B10
void CVehicleModelInfo::SetAtomicRenderCallbacks()
{
    switch (m_nVehicleType) {
    case VEHICLE_TYPE_TRAIN:
        RpClumpForAllAtomics(GetRpClump(), SetAtomicRendererCB_Train, nullptr);
        break;
    case VEHICLE_TYPE_PLANE:
    case VEHICLE_TYPE_FPLANE:
        RpClumpForAllAtomics(GetRpClump(), SetAtomicRendererCB_Plane, nullptr);
        break;
    case VEHICLE_TYPE_BOAT:
        RpClumpForAllAtomics(GetRpClump(), SetAtomicRendererCB_Boat, GetRpClump());
        break;
    case VEHICLE_TYPE_HELI:
        RpClumpForAllAtomics(GetRpClump(), SetAtomicRendererCB_RealHeli, GetRpClump());
        break;
    default:
        RpClumpForAllAtomics(GetRpClump(), SetAtomicRendererCB, GetRpClump());
        break;
    }
}

// 0x4C7C10
void CVehicleModelInfo::SetVehicleComponentFlags(RwFrame* component, uint32 flags)
{
    tVehicleComponentFlagsUnion flagsUnion;
    flagsUnion.m_nFlags = flags;

    // Como el original (0x4C7C1B): movzx edi, byte ptr [ecx+4Ah]; imul 0E0h; + 0C2B9DCh (solo el byte bajo del id, sin comprobar rango)
    auto handling = &gHandlingDataMgr.m_aVehicleHandling.data()[static_cast<uint8>(m_nHandlingId)];
    if (flagsUnion.bCull)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_FLAT);

    if (flagsUnion.bRenderAlways)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_DONT_CULL);

    if (flagsUnion.bDisableReflections)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_UNIQUE_MATERIALS);

    if (flagsUnion.bIsFront)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_FRONT);
    else if (flagsUnion.bIsRear && (handling->m_bIsVan || !(flagsUnion.bIsLeft || flagsUnion.bIsRight)))
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_REAR);
    else if (flagsUnion.bIsLeft)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_LEFT);
    else if (flagsUnion.bIsRight)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_RIGHT);

    if (flagsUnion.bSwinging && (handling->m_bIsHatchback || flagsUnion.bIsFrontDoor || flagsUnion.bIsRearDoor))
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_TOP);

    if (flagsUnion.bIsRearDoor)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_REAR_DOOR);
    else if (flagsUnion.bIsFrontDoor)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_FRONT_DOOR);

    if (flagsUnion.bHasAlpha)
        RwFrameForAllObjects(component, SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_ALPHA);
}

void CVehicleModelInfo::GetWheelPosn(int32 wheel, CVector& outVec, bool local) const
{
    auto frame = CClumpModelInfo::GetFrameFromId(GetRpClump(), ms_wheelFrameIDs[wheel]);

    if (m_nVehicleType != VEHICLE_TYPE_PLANE || local)
        outVec = *RwMatrixGetPos(RwFrameGetMatrix(frame));
    else {
        auto matrix = RwMatrixCreate();
        // Como el original (0x4C7D5A): rep movsd con ecx = 10h (0x40 bytes = sizeof(RwMatrix)); sizeof(CMatrix) copiaba 8 bytes de más
        memcpy(matrix, RwFrameGetMatrix(frame), sizeof(RwMatrix));
        auto parent = RwFrameGetParent(frame);
        while (parent) {
            RwMatrixTransform(matrix, RwFrameGetMatrix(parent), RwOpCombineType::rwCOMBINEPOSTCONCAT);
            parent = RwFrameGetParent(parent);
        }

        outVec = *RwMatrixGetPos(matrix);
        RwMatrixDestroy(matrix);
    }
}

bool CVehicleModelInfo::GetOriginalCompPosition(CVector& outVec, int32 component)
{
    auto frame = CClumpModelInfo::GetFrameFromId(GetRpClump(), component);
    if (!frame)
        return false;

    outVec = *RwMatrixGetPos(RwFrameGetMatrix(frame));
    return true;
}

// 0x4C8040
int32 CVehicleModelInfo::ChooseComponent()
{
    auto result = ms_compsToUse[0];
    if (result != -2) {
        ms_compsToUse[0] = -2;
        return result;
    }

    if (m_extraComps.nExtraARule && IsValidCompRule(m_extraComps.nExtraARule)) {
        return ::ChooseComponent(m_extraComps.nExtraARule, m_extraComps.nExtraAComp);
    }

    // Como el original (0x4C80B7..0x4C80D5): azar inlinado rand() * (1/32768) * 3.0 truncado = GetRandomIntInRange(0, 3);
    // la plantilla entera de CGeneral interpola con 1/32767 sobre [min, max - 1] y puede salir desfasada en uno.
    if (CGeneral::GetRandomIntInRange(0, 3) < 2) {
        int32 anVariations[6];
        // movsx edx, byte ptr [ecx+30Ch] (0x4C80E2): m_nNumExtras con signo
        const auto numComps = GetListOfComponentsNotUsedByRules(m_extraComps.m_nComps, static_cast<int8>(m_pVehicleStruct->m_nNumExtras), anVariations);
        if (numComps) {
            // Como el original (0x4C8100): call 0x407180 (0, numComps)
            return anVariations[CGeneral::GetRandomIntInRange(0, numComps)];
        }
    }

    return -1;
}

// 0x4C8120
int32 CVehicleModelInfo::ChooseSecondComponent()
{
    if (ms_compsToUse[1] != -2) {
        return std::exchange(ms_compsToUse[1], -2);
    }

    if (m_extraComps.nExtraBRule && IsValidCompRule(m_extraComps.nExtraBRule)) {
        return ::ChooseComponent(m_extraComps.nExtraBRule, m_extraComps.nExtraBComp);
    } else if (m_extraComps.nExtraARule && IsValidCompRule(m_extraComps.nExtraARule) && CGeneral::GetRandomIntInRange(0, 3) < 2) {
        int32 anVariations[6];
        // movsx edx, byte ptr [ecx+30Ch] (0x4C81C1): m_nNumExtras con signo
        const auto numComps = GetListOfComponentsNotUsedByRules(m_extraComps.m_nComps, static_cast<int8>(m_pVehicleStruct->m_nNumExtras), anVariations);
        if (numComps)
            return anVariations[CGeneral::GetRandomIntInRange(0, numComps)];
    }

    return -1;
}

// 0x4C8200
bool CVehicleModelInfo::IsUpgradeAvailable(eVehicleUpgradePosn upgrade)
{
    return m_pVehicleStruct->m_aUpgrades[upgrade].m_nParentComponentId >= 0;
}

void CVehicleModelInfo::SetVehicleColour(uint8 prim, uint8 sec, uint8 tert, uint8 quat)
{
    ms_currentCol[0] = prim;
    ms_currentCol[1] = sec;
    ms_currentCol[2] = tert;
    ms_currentCol[3] = quat;

    m_nCurrentPrimaryColor = prim;
    m_nCurrentSecondaryColor = sec;
    m_nCurrentTertiaryColor = tert;
    m_nCurrentQuaternaryColor = quat;
}

// 0x4C8500
void CVehicleModelInfo::ChooseVehicleColour(uint8& prim, uint8& sec, uint8& tert, uint8& quat, int32 variationShift)
{
    if (!m_nNumColorVariations || CCheat::IsActive(CHEAT_BLACK_TRAFFIC)) {
        prim = 0;
        sec = 0;
        tert = 0;
        quat = 0;
        return;
    }

    if (CCheat::IsActive(CHEAT_PINK_TRAFFIC)) {
        prim = 126;
        sec = 126;
        tert = 126;
        quat = 126;
        return;
    }

    // Como el original (0x4C8554..0x4C8570): idiv con signo; el resto se guarda como byte y se indexa con movzx
    // (un resto negativo da un índice de 0..255 sin comprobar el límite del array)
    auto iVariation = (variationShift + m_nLastColorVariation) % m_nNumColorVariations;
    m_nLastColorVariation = iVariation;
    prim = m_anPrimaryColors.data()[m_nLastColorVariation];
    sec = m_anSecondaryColors.data()[m_nLastColorVariation];
    tert = m_anTertiaryColors.data()[m_nLastColorVariation];
    quat = m_anQuaternaryColors.data()[m_nLastColorVariation];

    if (m_nNumColorVariations <= 1)
        return;

    auto vehicle = FindPlayerVehicle();
    if (!vehicle)
        return;

    if (CModelInfo::GetModelInfo(vehicle->m_nModelIndex) == this
        && vehicle->m_nPrimaryColor == prim
        && vehicle->m_nSecondaryColor == sec
        && vehicle->m_nTertiaryColor == tert
        && vehicle->m_nQuaternaryColor == quat) {

        iVariation = (variationShift + m_nLastColorVariation) % m_nNumColorVariations;
        m_nLastColorVariation = iVariation;
        prim = m_anPrimaryColors.data()[m_nLastColorVariation];
        sec = m_anSecondaryColors.data()[m_nLastColorVariation];
        tert = m_anTertiaryColors.data()[m_nLastColorVariation];
        quat = m_anQuaternaryColors.data()[m_nLastColorVariation];
    }
}

// 0x4C86B0
int32 CVehicleModelInfo::GetNumRemaps()
{
    auto iCount = 0;
    for (int32 i = 0; i < 4; ++i) {
        if (m_anRemapTxds[i] == -1)
            break;

        ++iCount;
    }

    return iCount;
}

// 0x4C86D0
void CVehicleModelInfo::AddRemap(int32 txd)
{
    auto numRemaps = GetNumRemaps();
    // Como el original (0x4C86F4): mov word ptr [ecx+eax*2+2FAh], dx sin comprobar el límite (con 4 remaps escribe en el
    // relleno que sigue a m_anRemapTxds, en 0x302)
    m_anRemapTxds.data()[numRemaps] = static_cast<int16>(txd);
}

void CVehicleModelInfo::SetRenderPipelines()
{
    CCarFXRenderer::CustomCarPipeClumpSetup(GetRpClump());
    if (carFrame)
        return;

    carFrame = RwFrameCreate();
    const CVector axis{1.0F, 0.0f, 0.0f};
    RwMatrixRotate(RwFrameGetMatrix(carFrame), &axis, 60.0F, RwOpCombineType::rwCOMBINEREPLACE);
    RwFrameUpdateObjects(carFrame);
    RwFrameGetLTM(carFrame);
}

char* CVehicleModelInfo::GetCustomCarPlateText()
{
    if (!m_szPlateText[0])
        return nullptr;

    return m_szPlateText;
}

// 0x4C8980
void CVehicleModelInfo::SetCustomCarPlateText(char* text) {
    if (text) {
        // Como el original (0x4C898F: call 0x821F40 con 8): strncpy trunca a 8 caracteres y no escribe el terminador
        // (m_szPlateText[8] no se toca); strcpy_s no truncaba y fallaba con textos largos
        strncpy(m_szPlateText, text, 8);
    } else {
        m_szPlateText[0] = '\0';
    }
}

// 0x4C8BD0
void CVehicleModelInfo::ReduceMaterialsInVehicle()
{
    CMemoryMgr::LockScratchPad();
    CMemoryMgr::InitScratchPad();

    RpMaterialList matList;
    matList.space = 20;
    matList.numMaterials = 0;
    // El EXE reserva con 0x72F420 (malloc); el new[] de la DLL reenvía a 0x821195 -> 0x82119A, que también acaba en malloc
    matList.materials = new RpMaterial*[matList.space];

    // Como el original (0x4C8C02, 0x4C8C54, 0x4C8C59): medición de tiempos descartada, pero el EXE hace las tres llamadas
    (void)CTimer::GetCurrentTimeInCycles();
    RpClumpForAllAtomics(GetRpClump(), StoreAtomicUsedMaterialsCB, &matList);
    // Como el original: la cuenta es un byte con signo (test cl, cl; jle / movsx ecx, byte ptr [eax+30Ch]) y se relee cada vuelta
    for (int32 i = 0; i < static_cast<int8>(m_pVehicleStruct->m_nNumExtras); ++i)
        StoreAtomicUsedMaterialsCB(m_pVehicleStruct->m_apExtras.data()[i], &matList);

    (void)CTimer::GetCurrentTimeInCycles();
    (void)CTimer::GetCyclesPerMillisecond();
    _rpMaterialListDeinitialize(&matList);
    CMemoryMgr::ReleaseScratchPad();
    CVisibilityPlugins::ClearClumpForAllAtomicsFlag(GetRpClump(), eAtomicComponentFlag::ATOMIC_UNIQUE_MATERIALS);
}

void CVehicleModelInfo::SetCarCustomPlate()
{
    m_pPlateMaterial = nullptr;
    SetCustomCarPlateText(nullptr);
    
    m_nPlateType = CARPLATE_DEFAULT;
    char plateBuffer[8 + 1] = "DEFAULT";

    CCustomCarPlateMgr::GeneratePlateText(plateBuffer, sizeof(plateBuffer) - 1);
    if (auto* material = CCustomCarPlateMgr::SetupClump(GetRpClump(), plateBuffer, m_nPlateType)) {
        m_pPlateMaterial = material;
    }
}

void CVehicleModelInfo::DisableEnvMap()
{
    if (!GetRwObject())
        return;

    RpClumpForAllAtomics(GetRpClump(), SetEnvironmentMapAtomicCB, (void*)0xFFFF);
}

// 0x4C9800
void CVehicleModelInfo::SetEnvMapCoeff(float coeff)
{
    // Como el original (0x4C9800..0x4C981B): fmul 1000.0f, floor (CRT) y _ftol2 (0x821B40): truncación a 64 bits y se usa el dword bajo
    auto iUsedCoeff = static_cast<int32>(static_cast<int64>(floor(coeff * 1000.0F)));
    if (!GetRwObject())
        return;

    RpClumpForAllAtomics(GetRpClump(), SetEnvMapCoeffAtomicCB, (void*)iUsedCoeff);
}

// 0x4C73C0
int32 CVehicleModelInfo::GetNumDoors()
{
    // Como el original: movsx eax, byte ptr [ecx+4Ch] (extiende el signo del byte)
    return static_cast<int8>(m_nNumDoors);
}

// 0x4C8E60
void CVehicleModelInfo::PreprocessHierarchy()
{
    m_nNumDoors = 0;
    RpAtomic* mainWheelAtomic = nullptr;
    RpAtomic* pTrainBogieAtomic = nullptr;
    // Como el original (0x4C8E6F): movzx eax, byte ptr [edi+4Ah]; imul 0E0h; + 0C2B9DCh (solo el byte bajo del id, sin comprobar rango)
    auto& handling = gHandlingDataMgr.m_aVehicleHandling.data()[static_cast<uint8>(m_nHandlingId)];
    RwObjectNameIdAssocation* nameIdAssoc = ms_vehicleDescs[m_nVehicleType];
    while (nameIdAssoc->m_pName) {
        auto flags = nameIdAssoc->AsFlagsUnion();

        if (flags.bIsDummy || flags.bIsExtra || flags.bIsUpgrade) {
            auto searchStruct = tCompSearchStructByName(nameIdAssoc->m_pName, nullptr);
            RwFrameForAllChildren(RpClumpGetFrame(GetRpClump()), CClumpModelInfo::FindFrameFromNameWithoutIdCB, &searchStruct);
            if (searchStruct.m_pFrame) {
                if (flags.bIsDummy) {
                    auto& vecDummyPos = GetModelDummyPosition(static_cast<eVehicleDummy>(nameIdAssoc->m_dwHierarchyId));
                    vecDummyPos = *RwMatrixGetPos(RwFrameGetMatrix(searchStruct.m_pFrame));
                    auto parent = RwFrameGetParent(searchStruct.m_pFrame);
                    if (parent) {
                        while (RwFrameGetParent(parent)) {
                            RwV3dTransformPoints(&vecDummyPos, &vecDummyPos, 1, RwFrameGetMatrix(parent));
                            parent = RwFrameGetParent(parent);
                        }
                    }
                    RwFrameDestroy(searchStruct.m_pFrame);
                }
                else if (flags.bIsUpgrade) {
                    auto parent = RwFrameGetParent(searchStruct.m_pFrame);
                    auto parentHierarchyId = CVisibilityPlugins::GetFrameHierarchyId(parent);
                    auto& upgrade = m_pVehicleStruct->m_aUpgrades[nameIdAssoc->m_dwHierarchyId];
                    upgrade.m_vPosition = *RwMatrixGetPos(RwFrameGetMatrix(searchStruct.m_pFrame));
                    upgrade.m_qRotation.Set(*RwFrameGetMatrix(searchStruct.m_pFrame));
                    upgrade.m_nParentComponentId = parentHierarchyId;
                }
                else {
                    auto atomic = reinterpret_cast<RpAtomic*>(GetFirstObject(searchStruct.m_pFrame));
                    RpClumpRemoveAtomic(GetRpClump(), atomic);
                    RwFrameRemoveChild(searchStruct.m_pFrame);
                    SetVehicleComponentFlags(searchStruct.m_pFrame, nameIdAssoc->m_dwFlags);
                    // Como el original (0x4C902E): movsx ecx, byte ptr [eax+30Ch]; sin comprobar el límite del array
                    m_pVehicleStruct->m_apExtras.data()[static_cast<int8>(m_pVehicleStruct->m_nNumExtras)] = atomic;
                    ++m_pVehicleStruct->m_nNumExtras;
                }
            }
        }

        if (flags.bIsMainWheel || flags.bIsTrainFrontBogie) {
            auto searchStruct = tCompSearchStructById(nameIdAssoc->m_dwHierarchyId, nullptr);
            RwFrameForAllChildren(RpClumpGetFrame(GetRpClump()), CClumpModelInfo::FindFrameFromIdCB, &searchStruct);
            if (searchStruct.m_pFrame) {
                auto frame = searchStruct.m_pFrame;
                auto bNoChild = false;
                while (!GetFirstObject(frame)) {
                    frame = GetFirstChild(frame);
                    if (!frame) {
                        bNoChild = true;
                        break;
                    }
                }

                if (!bNoChild) {
                    if (flags.bIsMainWheel)
                        mainWheelAtomic = reinterpret_cast<RpAtomic*>(GetFirstObject(frame));
                    else
                        pTrainBogieAtomic = reinterpret_cast<RpAtomic*>(GetFirstObject(frame));
                }
            }
        }

        nameIdAssoc++;
    }

    nameIdAssoc = ms_vehicleDescs[m_nVehicleType];
    while (nameIdAssoc->m_pName) {
        auto flags = nameIdAssoc->AsFlagsUnion();

        if (flags.bIsDummy || flags.bIsExtra || flags.bIsUpgrade) {
            nameIdAssoc++;
            continue;
        }

        auto searchStruct = tCompSearchStructById(nameIdAssoc->m_dwHierarchyId, nullptr);
        RwFrameForAllChildren(RpClumpGetFrame(GetRpClump()), CClumpModelInfo::FindFrameFromIdCB, &searchStruct);
        if (!searchStruct.m_pFrame) {
            nameIdAssoc++;
            continue;
        }

        auto frame = searchStruct.m_pFrame;
        if (flags.bIsDoor)
            ++m_nNumDoors;

        if (flags.bIsDamageable) {
            RpAtomic* aOkDamAtomics[2]{ nullptr, nullptr };
            RwFrameForAllChildren(frame, CollapseFramesCB, frame);
            RwFrameUpdateObjects(frame);
            RwFrameForAllObjects(frame, GetOkAndDamagedAtomicCB, aOkDamAtomics);
            if (aOkDamAtomics[0] && aOkDamAtomics[1]) {
                // Como el original (0x4C918B..0x4C919A): dam->renderCallBack = ok->renderCallBack y, si es nulo, AtomicDefaultRenderCallBack
                // del EXE (0x7491C0); la macro de la DLL guardaría la dirección del envoltorio de la DLL.
                aOkDamAtomics[1]->renderCallBack = aOkDamAtomics[0]->renderCallBack;
                if (!aOkDamAtomics[1]->renderCallBack) {
                    aOkDamAtomics[1]->renderCallBack = reinterpret_cast<RpAtomicCallBackRender>(0x7491C0);
                }
                m_pVehicleStruct->m_nMaskComponentsDamagable |= (1 << nameIdAssoc->m_dwHierarchyId);
            }
        }

        SetVehicleComponentFlags(frame, nameIdAssoc->m_dwFlags);

        if (flags.bIsWheel || flags.bIsMainWheel) {
            if (mainWheelAtomic) {
                if (flags.bIsMainWheel) {
                    RwFrameForAllChildren(frame, CollapseFramesCB, frame);
                    RwFrameUpdateObjects(frame);
                    CVisibilityPlugins::SetAtomicRenderCallback(mainWheelAtomic, CVisibilityPlugins::RenderWheelAtomicCB);
                }
                else {
                    auto pClone = RpAtomicClone(mainWheelAtomic);
                    RpAtomicSetFrame(pClone, frame);
                    RpClumpAddAtomic(GetRpClump(), pClone);
                    // Como el original (0x4C920F..0x4C9215: push 7323C0h; call 0x7328A0): la rueda clonada también recibe RenderWheelAtomicCB
                    CVisibilityPlugins::SetAtomicRenderCallback(pClone, CVisibilityPlugins::RenderWheelAtomicCB);
                    if (nameIdAssoc->m_dwHierarchyId != CAR_WHEEL_RF
                        && nameIdAssoc->m_dwHierarchyId != CAR_WHEEL_LF
                        && handling.m_bDoubleRwheels) {

                        auto clone2 = RpAtomicClone(mainWheelAtomic);
                        auto newFrame = RwFrameCreate();
                        RpAtomicSetFrame(clone2, newFrame);
                        RwFrameAddChild(frame, newFrame);
                        auto fOffset = fRearDoubleWheelOffsetFactor * -0.25F;
                        auto matrix = RwFrameGetMatrix(newFrame);
                        *RwMatrixGetRight(matrix) = { 1.0F, 0.0F, 0.0F };
                        *RwMatrixGetUp(matrix)    = { 0.0F, 1.0F, 0.0F };
                        *RwMatrixGetAt(matrix)    = { 0.0F, 0.0F, 1.0F };
                        *RwMatrixGetPos(matrix)   = { fOffset, 0.0F, 0.0F };
                        matrix->flags |= (rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL);
                        RpClumpAddAtomic(GetRpClump(), clone2);
                        CVisibilityPlugins::SetAtomicRenderCallback(clone2, CVisibilityPlugins::RenderWheelAtomicCB);
                    }
                }
            }
        }
        else if (flags.bIsTrainRearBogie && pTrainBogieAtomic) {
            auto pClone = RpAtomicClone(pTrainBogieAtomic);
            RpAtomicSetFrame(pClone, frame);
            RpClumpAddAtomic(GetRpClump(), pClone);
            CVisibilityPlugins::SetAtomicRenderCallback(pClone, CVisibilityPlugins::RenderWheelAtomicCB);
        }

        nameIdAssoc++;
    }
}

void CVehicleModelInfo::SetupLightFlags(CVehicle* vehicle)
{
    ms_lightsOn[0] = vehicle->m_renderLights.m_bLeftFront;
    ms_lightsOn[1] = vehicle->m_renderLights.m_bRightFront;
    ms_lightsOn[2] = vehicle->m_renderLights.m_bLeftRear;
    ms_lightsOn[3] = vehicle->m_renderLights.m_bRightRear;
}

// 0x4C89B0
int32 CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors(int32 modelId)
{
    auto mi = CModelInfo::GetModelInfo(modelId)->AsVehicleModelInfoPtr();
    // Como el original (0x4C89BF): movzx eax, byte ptr [edx+4Ah]; imul 0E0h; + 0C2B9DCh (solo el byte bajo del id, sin comprobar rango)
    const auto handling = &gHandlingDataMgr.m_aVehicleHandling.data()[static_cast<uint8>(mi->m_nHandlingId)];
    if (mi->IsBoat() || mi->IsTrailer())
        return 0;

    int32 iDoorsNum;
    switch (modelId) {
    case -1:
        iDoorsNum = 3;
        break;

    case MODEL_FIRETRUK:
    case MODEL_JOURNEY:
        iDoorsNum = 2;
        break;

    case MODEL_HUNTER:
        iDoorsNum = 1;
        break;

    default:
        // Como el original (0x4C8A31: mov cl, byte ptr [edx+4Ch]; ...; movzx eax, cl): el byte se usa SIN signo aquí
        // (GetNumDoors, 0x4C73C0, sí lo extiende con movsx)
        iDoorsNum = mi->m_nNumDoors;
        if (iDoorsNum)
            break;

        if (mi->IsBike() || mi->IsBMX() || mi->IsQuad() || handling->m_bTandemSeats) {
            return mi->m_pVehicleStruct->IsDummyActive(eVehicleDummy::DUMMY_SEAT_REAR) ? 1 : 0;
        }
        else {
            if (modelId == MODEL_RCBANDIT || modelId == MODEL_RCTIGER)
                return 0;
            return 1;
        }
    }

    if (handling->m_bTandemSeats)
        return std::max(0, (iDoorsNum - 2) / 2);

    if (modelId == MODEL_COACH || modelId == MODEL_BUS)
        return 8;

    return iDoorsNum - 1;
}

RwObject* CVehicleModelInfo::MoveObjectsCB(RwObject* object, void* data)
{
    RpAtomicSetFrame(reinterpret_cast<RpAtomic*>(object), reinterpret_cast<RwFrame*>(data));
    return object;
}

RpMaterial* CVehicleModelInfo::SetEditableMaterialsCB(RpMaterial* material, void* data)
{
    auto ppEntries = reinterpret_cast<tRestoreEntry**>(data);

    CRGBA color = *RpMaterialGetColor(material);
    color.a = 0;

    if (ms_pRemapTexture
        && RpMaterialGetTexture(material)
        && RwTextureGetName(RpMaterialGetTexture(material))[0] == '#') {

        (*ppEntries)->m_pAddress = &material->texture;
        (*ppEntries)->m_pValue = material->texture;
        (*ppEntries)++;

        material->texture = ms_pRemapTexture;
    }

    if (RpMaterialGetTexture(material) == ms_pLightsTexture) {
        int32 iLightIndex = -1;
        if (color == CRGBA(255, 175, 0, 0))
            iLightIndex = 0;
        else if (color == CRGBA(0, 255, 200, 0))
            iLightIndex = 1;
        else if (color == CRGBA(185, 255, 0, 0))
            iLightIndex = 2;
        else if (color == CRGBA(255, 60, 0, 0))
            iLightIndex = 3;

        (*ppEntries)->m_pAddress = RpMaterialGetColor(material);
        (*ppEntries)->m_pValue = *reinterpret_cast<void**>(RpMaterialGetColor(material));
        (*ppEntries)++;

        RpMaterialGetColor(material)->red = 255;
        RpMaterialGetColor(material)->green = 255;
        RpMaterialGetColor(material)->blue = 255;

        if (iLightIndex != -1 && ms_lightsOn[iLightIndex]) {
            (*ppEntries)->m_pAddress = &material->texture;
            (*ppEntries)->m_pValue = material->texture;
            (*ppEntries)++;

            (*ppEntries)->m_pAddress = RpMaterialGetSurfaceProperties(material);
            (*ppEntries)->m_pValue = *(void**)RpMaterialGetSurfaceProperties(material);
            (*ppEntries)++;

            material->texture = ms_pLightsOnTexture;
            RpMaterialSetSurfaceProperties(material, &gLightSurfProps);
        }
    }
    else {
        int32 iColorIndex;
        if (color == CRGBA(60, 255, 0, 0))
            iColorIndex = ms_currentCol[0];
        else if (color == CRGBA(255, 0, 175, 0))
            iColorIndex = ms_currentCol[1];
        else if (color == CRGBA(0, 255, 255, 0))
            iColorIndex = ms_currentCol[2];
        else if (color == CRGBA(255, 0, 255, 0))
            iColorIndex = ms_currentCol[3];
        else
            return material;

        (*ppEntries)->m_pAddress = RpMaterialGetColor(material);
        (*ppEntries)->m_pValue = *reinterpret_cast<void**>(RpMaterialGetColor(material));
        (*ppEntries)++;

        auto& color = ms_vehicleColourTable[iColorIndex]; // todo: shadow var
        RpMaterialGetColor(material)->red = color.r;
        RpMaterialGetColor(material)->green = color.g;
        RpMaterialGetColor(material)->blue = color.b;
    }

    return material;
}

RpAtomic* CVehicleModelInfo::SetEditableMaterialsCB(RpAtomic* atomic, void* data)
{
    if (rwObjectTestFlags(atomic, RpAtomicFlag::rpATOMICRENDER) == 0)
        return atomic;

    if (CVisibilityPlugins::GetAtomicId(atomic) & eAtomicComponentFlag::ATOMIC_DONT_RENDER_ALPHA)
        RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), RemoveWindowAlphaCB, data);

    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), SetEditableMaterialsCB, data);
    return atomic;
}

void CVehicleModelInfo::ShutdownLightTexture()
{
    if (ms_pLightsTexture) {
        RwTextureDestroy(ms_pLightsTexture);
        ms_pLightsTexture = nullptr;
    }

    if (ms_pLightsOnTexture) {
        RwTextureDestroy(ms_pLightsOnTexture);
        ms_pLightsOnTexture = nullptr;
    }
}

RwTexture* CVehicleModelInfo::FindTextureCB(const char* name)
{
    auto dictTex = RwTexDictionaryFindNamedTexture(vehicleTxd, name);
    if (dictTex)
        return dictTex;

    auto current = RwTexDictionaryGetCurrent();
    auto named = RwTexDictionaryFindNamedTexture(current, name);
    if (strncmp(name, "remap", 5))
        return named;

    // Remap below this point
    if (named) {
        named->name[0] = '#';
        return named;
    }

    // Como el original (0x4C7560): copia byte a byte hasta el NUL sin comprobar el tamaño del búfer local
    char buffer[32];
    strcpy(buffer, name);
    buffer[0] = '#';
    // Como el original (0x4C7574): vuelve a llamar a RwTexDictionaryGetCurrent (0x7F3A90) para esta segunda búsqueda
    return RwTexDictionaryFindNamedTexture(RwTexDictionaryGetCurrent(), buffer);
}

void CVehicleModelInfo::UseCommonVehicleTexDicationary()
{
    CVehicleModelInfo::SavedTextureFindCallback = RwTextureGetFindCallBack();
    RwTextureSetFindCallBack(FindTextureCB);
}

void CVehicleModelInfo::StopUsingCommonVehicleTexDicationary()
{
    RwTextureSetFindCallBack(SavedTextureFindCallback);
    CVehicleModelInfo::SavedTextureFindCallback = nullptr;
}

void CVehicleModelInfo::SetEditableMaterials(RpClump* clump)
{
    auto pEntry = gRestoreEntries;
    RpClumpForAllAtomics(clump, SetEditableMaterialsCB, &pEntry);
    pEntry->m_pAddress = nullptr;
}

void CVehicleModelInfo::ResetEditableMaterials(RpClump* clump)
{
    auto pEntry = gRestoreEntries;
    while (pEntry->m_pAddress) {
        *(uint32*)pEntry->m_pAddress = (uint32)pEntry->m_pValue;
        pEntry++;
    }
}

RpAtomic* CVehicleModelInfo::HideDamagedAtomicCB(RpAtomic* atomic, void* data)
{
    auto frame = RpAtomicGetFrame(atomic);
    auto nodeName = GetFrameNodeName(frame);
    if (strstr(nodeName, "_dam")) {
        RpAtomicSetFlags(atomic, 0);
        CVisibilityPlugins::SetAtomicFlag(atomic, eAtomicComponentFlag::ATOMIC_DAMAGED);
        return atomic;
    }

    if (strstr(nodeName, "_ok"))
        CVisibilityPlugins::SetAtomicFlag(atomic, eAtomicComponentFlag::ATOMIC_OK);

    return atomic;
}

RpAtomic* CVehicleModelInfo::HideAllComponentsAtomicCB(RpAtomic* atomic, void* data)
{
    auto uiData = reinterpret_cast<uint32>(data);
    auto checkedFlags = uiData & CVisibilityPlugins::GetAtomicId(atomic);
    if (checkedFlags)
        RpAtomicSetFlags(atomic, 0);
    else
        RpAtomicSetFlags(atomic, RpAtomicFlag::rpATOMICRENDER);

    return atomic;
}

RpMaterial* CVehicleModelInfo::HasAlphaMaterialCB(RpMaterial* material, void* data)
{
    if (RpMaterialGetColor(material)->alpha != 255) {
        *reinterpret_cast<bool*>(data) = true;
        return nullptr;
    }

    return material;
}

RpAtomic* CVehicleModelInfo::SetAtomicRendererCB(RpAtomic* atomic, void* data)
{
    auto nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
    if (strstr(nodeName, "_vlo")) {
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleReallyLowDetailCB);
    }
    else {
        auto geometry = RpAtomicGetGeometry(atomic);
        bool bHasAlpha = false;
        RpGeometryForAllMaterials(geometry, HasAlphaMaterialCB, &bHasAlpha);
        if (bHasAlpha || !strncmp(nodeName, "windscreen", 10))
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailAlphaCB);
        else
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB);
    }

    HideDamagedAtomicCB(atomic, nullptr);
    return atomic;
}

RpAtomic* CVehicleModelInfo::SetAtomicRendererCB_RealHeli(RpAtomic* atomic, void* data)
{
    auto nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
    if (!strcmp(nodeName, "moving_rotor"))
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderHeliRotorAlphaCB);
    else if (!strcmp(nodeName, "moving_rotor2"))
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderHeliTailRotorAlphaCB);
    else if (strstr(nodeName, "_vlo")) {
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleReallyLowDetailCB);
    }
    else {
        auto geometry = RpAtomicGetGeometry(atomic);
        bool bHasAlpha = false;
        RpGeometryForAllMaterials(geometry, HasAlphaMaterialCB, &bHasAlpha);
        if (bHasAlpha || !strncmp(nodeName, "windscreen", 10))
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailAlphaCB);
        else
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB);
    }

    HideDamagedAtomicCB(atomic, nullptr);
    return atomic;
}

RpAtomic* CVehicleModelInfo::SetAtomicRendererCB_Plane(RpAtomic* atomic, void* data)
{
    auto nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
    if (strstr(nodeName, "_vlo")) {
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle);
    }
    else {
        auto geometry = RpAtomicGetGeometry(atomic);
        bool bHasAlpha = false;
        RpGeometryForAllMaterials(geometry, HasAlphaMaterialCB, &bHasAlpha);
        if (bHasAlpha)
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle);
        else
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle);
    }

    HideDamagedAtomicCB(atomic, nullptr);
    return atomic;
}

RpAtomic* CVehicleModelInfo::SetAtomicRendererCB_Boat(RpAtomic* atomic, void* data)
{
    auto nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
    // Como el original (0x4C79B1..0x4C79C6): el recorrido de materiales (HasAlphaMaterialCB) se hace ANTES de comparar el nombre
    bool bHasAlpha = false;
    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), HasAlphaMaterialCB, &bHasAlpha);
    if (!strcmp(nodeName, "boat_hi"))
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB_Boat);
    else if (strstr(nodeName, "_vlo")) {
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleLoDetailCB_Boat);
    }
    else {
        if (bHasAlpha)
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat);
        else
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB_Boat);
    }

    HideDamagedAtomicCB(atomic, nullptr);
    return atomic;
}

RpAtomic* CVehicleModelInfo::SetAtomicRendererCB_Heli(RpAtomic* atomic, void* data)
{
    auto nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
    if (!strncmp(nodeName, "toprotor", 8))
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderHeliRotorAlphaCB);
    else if (!strncmp(nodeName, "rearrotor", 9))
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderHeliTailRotorAlphaCB);
    else
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, nullptr);

    return atomic;
}

RpAtomic* CVehicleModelInfo::SetAtomicRendererCB_Train(RpAtomic* atomic, void* data)
{
    auto nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
    if (strstr(nodeName, "_vlo")) {
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle);
    }
    else {
        auto geometry = RpAtomicGetGeometry(atomic);
        bool bHasAlpha = false;
        RpGeometryForAllMaterials(geometry, HasAlphaMaterialCB, &bHasAlpha);
        if (bHasAlpha)
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderTrainHiDetailAlphaCB);
        else
            CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderTrainHiDetailCB);
    }

    CVehicleModelInfo::HideDamagedAtomicCB(atomic, nullptr);
    return atomic;
}

RwObject* CVehicleModelInfo::SetAtomicFlagCB(RwObject* object, void* data)
{
    CVisibilityPlugins::SetAtomicFlag((RpAtomic*)object, (uint16)(uintptr)data);
    return object;
}

RwObject* CVehicleModelInfo::ClearAtomicFlagCB(RwObject* object, void* data)
{
    CVisibilityPlugins::ClearAtomicFlag((RpAtomic*)object, (uint16)(uintptr)data);
    return object;
}

// 0x4C8700
void CVehicleModelInfo::AddWheelUpgrade(int32 wheelSetNumber, int32 modelId)
{
    auto iUpgradesNum = ms_numWheelUpgrades[wheelSetNumber];
    ms_upgradeWheels[wheelSetNumber][iUpgradesNum] = modelId;
    ms_numWheelUpgrades[wheelSetNumber] = iUpgradesNum + 1;
}

int32 CVehicleModelInfo::GetWheelUpgrade(int32 wheelSetNumber, int32 wheelUpgradeNumber)
{
    return ms_upgradeWheels[wheelSetNumber][wheelUpgradeNumber];
}

int32 CVehicleModelInfo::GetNumWheelUpgrades(int32 wheelSetNumber)
{
    return ms_numWheelUpgrades[wheelSetNumber];
}

// 0x4C8770
void CVehicleModelInfo::DeleteVehicleColourTextures() {
    // NOP
}

void CVehicleModelInfo::SetDirtTextures(CVehicleModelInfo* info, int32 dirtLevel)
{
    for (auto i = 0; i < CCarFXRenderer::NUM_DIRT_TEXTURES; ++i)
        if (info->m_apDirtMaterials[i])
            RpMaterialSetTexture(info->m_apDirtMaterials[i], CCarFXRenderer::ms_aDirtTextures[dirtLevel]);
}

// 0x4C87D0
void CVehicleModelInfo::ShutdownEnvironmentMaps()
{
    // Como el original (0x4C87D9): mov byte ptr ds:[0B4E6BCh], 0 (bandera global del EXE; antes era una estática local de la DLL)
    StaticRef<bool>(0xB4E6BC) = false;
    RwTextureDestroy(gpWhiteTexture);
    gpWhiteTexture = nullptr;

    if (carFrame)
        RwFrameDestroy(carFrame);
    carFrame = nullptr;
}

// 0x4C8810
RpMaterial* CVehicleModelInfo::GetMatFXEffectMaterialCB(RpMaterial* material, void* data)
{
    // Como el original (0x4C8816 y 0x4C8823): llama dos veces a RpMatFXMaterialGetEffects
    if (!RpMatFXMaterialGetEffects(material))
        return material;

    *reinterpret_cast<RpMatFXMaterialFlags*>(data) = RpMatFXMaterialGetEffects(material);
    return nullptr;
}

// 0x4C8840
RpMaterial* CVehicleModelInfo::SetEnvironmentMapCB(RpMaterial* material, void* data)
{
    // Como el original (0x4C8840: cmp dword ptr [esp+8], 0FFFFh): se compara el dword completo, no solo 16 bits;
    // la rama llama directamente a RpMatFXMaterialSetEffects(material, 0) (0x4C884F..0x4C8852: push 0, push esi, call 0x811C80)
    if (std::bit_cast<uintptr_t>(data) == 0xFFFF) {
        RpMatFXMaterialSetEffects(material, rpMATFXEFFECTNULL);
        return material;
    }

    if (RpMatFXMaterialGetEffects(material) != RpMatFXMaterialFlags::rpMATFXEFFECTENVMAP)
        return material;

    RpMatFXMaterialSetEnvMapFrame(material, carFrame);
    if (!RpMaterialGetTexture(material))
        RpMaterialSetTexture(material, gpWhiteTexture);

    RpMatFXMaterialSetEnvMapCoefficient(material, fEnvMapDefaultCoeff);
    return material;
}

// 0x4C88B0
RpMaterial* CVehicleModelInfo::SetEnvMapCoeffCB(RpMaterial* material, void* data)
{
    if (RpMatFXMaterialGetEffects(material) != RpMatFXMaterialFlags::rpMATFXEFFECTENVMAP)
        return material;

    // Como el original (0x4C88C7..0x4C88DC), en x87: fild dword [data]; si es negativo suma 2^32 (0x858C54);
    // fmul 0.001f (0x858CDC, NO una división entre 1000); fstp dword. El producto se redondea una sola vez a float.
    const float fCoeff = UnsignedMillisToSecondsCpp(reinterpret_cast<uint32>(data)); // X87Math.cpp
    RpMatFXMaterialSetEnvMapCoefficient(material, fCoeff);
    return material;
}

RpAtomic* CVehicleModelInfo::SetRenderPipelinesCB(RpAtomic* atomic, void* data)
{
    return atomic;
}

RwFrame* CVehicleModelInfo::CollapseFramesCB(RwFrame* frame, void* data)
{
    RwFrameForAllChildren(frame, CollapseFramesCB, data);
    RwFrameForAllObjects(frame, MoveObjectsCB, data);
    RwFrameDestroy(frame);
    return frame;
}

RpAtomic* CVehicleModelInfo::SetEnvironmentMapAtomicCB(RpAtomic* atomic, void* data)
{
    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), SetEnvironmentMapCB, data);
    return atomic;
}

RpAtomic* CVehicleModelInfo::SetEnvMapCoeffAtomicCB(RpAtomic* atomic, void* data)
{
    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), SetEnvMapCoeffCB, data);
    return atomic;
}

// 0x4C9360
void CVehicleModelInfo::AssignRemapTxd(const char* name, int16 txdSlot)
{
    // Como el original (0x4C937B, 0x4C938D, 0x4C93A2): movzx del byte antes de llamar a isdigit (carácter sin signo)
    auto iLen = strlen(name);
    if (!isdigit(static_cast<uint8>(name[iLen - 1])))
        return;

    auto iLastIndex = iLen - 2;
    while (isdigit(static_cast<uint8>(name[iLastIndex])))
        --iLastIndex;

    // Como el original (0x4C93BD: call 0x821F40): strncpy sin comprobar el tamaño del búfer local, luego el terminador
    char buffer[24];
    strncpy(buffer, name, iLastIndex + 1);
    buffer[iLastIndex + 1] = '\0';

    auto mi = CModelInfo::GetModelInfo(buffer, 400, 630);
    if (mi && mi->GetModelType() == ModelInfoType::MODEL_INFO_VEHICLE)
        mi->AsVehicleModelInfoPtr()->AddRemap(txdSlot);
}

RpAtomic* CVehicleModelInfo::StoreAtomicUsedMaterialsCB(RpAtomic* atomic, void* data)
{
    auto matList = reinterpret_cast<RpMaterialList*>(data);
    auto geometry = RpAtomicGetGeometry(atomic);
    auto pMeshHeader = geometry->mesh;
    if (CVisibilityPlugins::GetAtomicId(atomic) & eAtomicComponentFlag::ATOMIC_UNIQUE_MATERIALS)
        return atomic;

    for (int32 i = 0; i < pMeshHeader->numMeshes; ++i) {
        auto pMesh = RpGeometryGetMesh(geometry, i);
        // auto mat = matList->materials; // Unused code i guess
        // for (int32 iMat = 0; iMat < matList->numMaterials; ++iMat)
        //     mat++;

        _rpMaterialListAppendMaterial(matList, pMesh->material);
    }
    return atomic;
}

// 0x5B8F00
void CVehicleModelInfo::SetupCommonData() {
    ZoneScoped;

    LoadVehicleColours();
    CLoadingScreen::NewChunkLoaded();
    LoadVehicleUpgrades();
    CLoadingScreen::NewChunkLoaded();
    LoadEnvironmentMaps();
    CLoadingScreen::NewChunkLoaded();

    auto slot = CTxdStore::FindOrAddTxdSlot("vehicle");
    CTxdStore::LoadTxd(slot, "MODELS\\GENERIC\\VEHICLE.TXD");
    CTxdStore::AddRef(slot);
    if (slot != -1)
        vehicleTxd = CTxdStore::ms_pTxdPool->GetAt(slot)->m_pRwDictionary;
    // Sin assert(vehicleTxd): el EXE (0x5B8F93) pasa el diccionario tal cual a RwTexDictionaryFindNamedTexture
    ms_pLightsTexture = RwTexDictionaryFindNamedTexture(vehicleTxd, "vehiclelights128");
    ms_pLightsOnTexture = RwTexDictionaryFindNamedTexture(vehicleTxd, "vehiclelightson128");
    CLoadingScreen::NewChunkLoaded();
    CVehicleStructure::m_pInfoPool = new CPool<CVehicleStructure>(50, "VehicleStruct");
    CCarFXRenderer::InitialiseDirtTexture();
}

// 0x5B6890
void CVehicleModelInfo::LoadVehicleColours()
{
    char buffer[1024];
    auto file = CFileMgr::OpenFile("DATA\\CARCOLS.DAT", "r");
    auto curColor = ms_vehicleColourTable;

    eCarColLineType iLastMode = eCarColLineType::IGNORED;
    eCarColLineType iCurMode = eCarColLineType::IGNORED;
    // Como el original: red/green/blue son locales de pila que persisten entre líneas (sscanf no escribe los que no lee, p. ej. con
    // el error de carcols.dat "77.93,96": el EXE deja los valores de la línea anterior). Se inicializan una vez fuera del bucle.
    uint32 red{}, green{}, blue{};
    while (CFileMgr::ReadLine(file, buffer, 1024)) {
        // Como el original (0x5B68E0..0x5B690B): sin límite de 10 caracteres, comparación SIN signo (cmp al, 20h; ja) y parada en NUL o '\n'
        auto iStartInd = 0;
        while (static_cast<uint8>(buffer[iStartInd]) <= ' ' && buffer[iStartInd] && buffer[iStartInd] != '\n')
            ++iStartInd;

        auto pLineStart = &buffer[iStartInd];
        auto iLength = 0;
        while (pLineStart[iLength] && pLineStart[iLength] != '\n') {
            if (pLineStart[iLength] == ',' || pLineStart[iLength] == '\r')
                pLineStart[iLength] = ' ';

            ++iLength;
        }
        pLineStart[iLength] = '\0';

        if (!pLineStart[0] || pLineStart[0] == '#')
            continue;

        if (iLastMode == eCarColLineType::IGNORED) {
            if (!strncmp(pLineStart, "col", 3)) {
                iLastMode = eCarColLineType::GLOBAL_RGB;
                iCurMode = eCarColLineType::GLOBAL_RGB;
            }
            else if (!strncmp(pLineStart, "car4", 4)) {
                iLastMode = eCarColLineType::CAR_4COL;
                iCurMode = eCarColLineType::CAR_4COL;
            }
            else if (!strncmp(pLineStart, "car", 3)) {
                iLastMode = eCarColLineType::CAR_2COL;
                iCurMode = eCarColLineType::CAR_2COL;
            }
            continue;
        }

        if (!strncmp(pLineStart, "end", 3)) {
            iLastMode = eCarColLineType::IGNORED;
            iCurMode = eCarColLineType::IGNORED;
            continue;
        }

        if (iLastMode == eCarColLineType::GLOBAL_RGB) {
            // Como el original (0x5B6A3E): solo sscanf "%d %d %d"; sin el arreglo de la línea "77.93,96" de carcols.dat (el EXE no lo tiene)
            RET_IGNORED(sscanf_s(buffer, "%d %d %d", &red, &green, &blue));
            curColor->Set(red, green, blue, 255);
            auto pLineEnd = pLineStart;
            while (*pLineEnd != '#') // Seems redundant(?)
                pLineEnd++;

            curColor++;
            continue;
        }

        uint32 colorBuffer[8][4];
        if (iLastMode == eCarColLineType::CAR_2COL)
        {
            char modelName[64];
            auto iNumRead = sscanf_s(buffer, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                SCANF_S_STR(modelName),
                &colorBuffer[0][0],
                &colorBuffer[0][1],
                &colorBuffer[1][0],
                &colorBuffer[1][1],
                &colorBuffer[2][0],
                &colorBuffer[2][1],
                &colorBuffer[3][0],
                &colorBuffer[3][1],
                &colorBuffer[4][0],
                &colorBuffer[4][1],
                &colorBuffer[5][0],
                &colorBuffer[5][1],
                &colorBuffer[6][0],
                &colorBuffer[6][1],
                &colorBuffer[7][0],
                &colorBuffer[7][1]);

            auto mi = CModelInfo::GetModelInfo(modelName, nullptr)->AsVehicleModelInfoPtr();
            auto iNumVariations = (iNumRead - 1) / 2;
            mi->m_nNumColorVariations = iNumVariations;

            for (int32 i = 0; i < iNumVariations; ++i) {
                mi->m_anPrimaryColors[i]    = colorBuffer[i][0];
                mi->m_anSecondaryColors[i]  = colorBuffer[i][1];
                mi->m_anTertiaryColors[i]   = 0;
                mi->m_anQuaternaryColors[i] = 0;
            }

            iLastMode = iCurMode;
            continue;
        }

        if (iLastMode == eCarColLineType::CAR_4COL) {
            char modelName[64];
            auto iNumRead = sscanf_s(buffer, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                SCANF_S_STR(modelName),
                &colorBuffer[0][0],
                &colorBuffer[0][1],
                &colorBuffer[0][2],
                &colorBuffer[0][3],
                &colorBuffer[1][0],
                &colorBuffer[1][1],
                &colorBuffer[1][2],
                &colorBuffer[1][3],
                &colorBuffer[2][0],
                &colorBuffer[2][1],
                &colorBuffer[2][2],
                &colorBuffer[2][3],
                &colorBuffer[3][0],
                &colorBuffer[3][1],
                &colorBuffer[3][2],
                &colorBuffer[3][3],
                &colorBuffer[4][0],
                &colorBuffer[4][1],
                &colorBuffer[4][2],
                &colorBuffer[4][3],
                &colorBuffer[5][0],
                &colorBuffer[5][1],
                &colorBuffer[5][2],
                &colorBuffer[5][3],
                &colorBuffer[6][0],
                &colorBuffer[6][1],
                &colorBuffer[6][2],
                &colorBuffer[6][3],
                &colorBuffer[7][0],
                &colorBuffer[7][1],
                &colorBuffer[7][2],
                &colorBuffer[7][3]
            );

            auto mi = CModelInfo::GetModelInfo(modelName, nullptr)->AsVehicleModelInfoPtr();
            auto iNumVariations = (iNumRead - 1) / 4;
            mi->m_nNumColorVariations = iNumVariations;

            for (int32 i = 0; i < iNumVariations; ++i) {
                mi->m_anPrimaryColors[i]    = colorBuffer[i][0];
                mi->m_anSecondaryColors[i]  = colorBuffer[i][1];
                mi->m_anTertiaryColors[i]   = colorBuffer[i][2];
                mi->m_anQuaternaryColors[i] = colorBuffer[i][3];
            }

            iLastMode = iCurMode;
            continue;
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x5B65A0
void CVehicleModelInfo::LoadVehicleUpgrades()
{
    // Como el original (0x5B65BE..0x5B65D1): primero se abre el fichero y después se ponen a 0 los contadores de ruedas
    auto file = CFileMgr::OpenFile("DATA\\CARMODS.DAT", "r");
    for (auto& wheelUpgrade : ms_numWheelUpgrades)
        wheelUpgrade = 0;
    char* line;
    eCarModsLineType iLineType = eCarModsLineType::IGNORED;
    // Como el original: el número de juego de ruedas es un local de pila que persiste entre líneas (si sscanf no lo lee, conserva el anterior)
    int32 iWheelSet = 0;
    while ((line = CFileLoader::LoadLine(file))) {
        if (!line[0] || line[0] == '#')
            continue;

        if (iLineType == eCarModsLineType::IGNORED) {
            if (!strncmp(line, "link", 4))
                iLineType = eCarModsLineType::LINK;
            else if (!strncmp(line, "mods", 4))
                iLineType = eCarModsLineType::MODS;
            else if (!strncmp(line, "wheel", 5))
                iLineType = eCarModsLineType::WHEEL;

            continue;
        }

        if (!strncmp(line, "end", 3)) {
            iLineType = eCarModsLineType::IGNORED;
            continue;
        }

        char* pLastToken{};
        switch (iLineType) {
        case eCarModsLineType::LINK: {
            int32 iModelId1 = -1, iModelId2 = -1;
            char* pLastToken{};
            auto pToken = strtok_s(line, " \t,", &pLastToken);
            if (pToken) {
                auto pModel1 = static_cast<CAtomicModelInfo*>(CModelInfo::GetModelInfo(pToken, &iModelId1));
                pModel1->SetupVehicleUpgradeFlags(pToken);

                auto pNextToken = strtok_s(nullptr, " \t,", &pLastToken);
                auto pModel2 = static_cast<CAtomicModelInfo*>(CModelInfo::GetModelInfo(pNextToken, &iModelId2));
                pModel2->SetupVehicleUpgradeFlags(pNextToken);

                ms_linkedUpgrades.AddUpgradeLink(iModelId1, iModelId2);
            }
            break;
        }

        case eCarModsLineType::MODS: {
            auto pToken = strtok_s(line, " \t,", &pLastToken);
            if (!pToken)
                break;

            int32 iModelId = -1;
            auto mi = CModelInfo::GetModelInfo(pToken, &iModelId)->AsVehicleModelInfoPtr();
            auto nextToken = strtok_s(nullptr, " \t,", &pLastToken);
            size_t upgradeIdx = 0;
            while (nextToken) {
                auto ami = static_cast<CAtomicModelInfo*>(CModelInfo::GetModelInfo(nextToken, &iModelId));
                ami->SetupVehicleUpgradeFlags(nextToken);
                mi->m_anUpgrades.data()[upgradeIdx++] = iModelId; // sin comprobar el límite, como el original (0x5B6784)
                nextToken = strtok_s(nullptr, " \t,", &pLastToken);
            }

            auto hydraulicsAMI = static_cast<CAtomicModelInfo*>(CModelInfo::GetModelInfo("hydralics", &iModelId));
            hydraulicsAMI->SetupVehicleUpgradeFlags("hydralics");
            mi->m_anUpgrades.data()[upgradeIdx++] = iModelId; // sin comprobar el límite, como el original (0x5B6784)

            auto stereoAMI = static_cast<CAtomicModelInfo*>(CModelInfo::GetModelInfo("stereo", &iModelId));
            stereoAMI->SetupVehicleUpgradeFlags("stereo");
            mi->m_anUpgrades.data()[upgradeIdx] = iModelId;
            break;
        }

        case eCarModsLineType::WHEEL: {
            int32 iModelId = -1;
            RET_IGNORED(sscanf_s(line, "%d", &iWheelSet)); // el EXE (0x5B66B3) no comprueba el resultado
            RET_IGNORED(strtok_s(line, " \t,", &pLastToken));
            char* token;
            while ((token = strtok_s(nullptr, " \t,", &pLastToken))) {
                auto wheelMI = static_cast<CAtomicModelInfo*>(CModelInfo::GetModelInfo(token, &iModelId));
                wheelMI->SetupVehicleUpgradeFlags(token);
                AddWheelUpgrade(iWheelSet, iModelId);
            }
            break;
        }
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x4C8780
void CVehicleModelInfo::LoadEnvironmentMaps()
{
    // Como el original (0x4C8781..0x4C8793): FindTxdSlot("particle") se evalúa ANTES de PushCurrentTxd
    const auto particleSlot = CTxdStore::FindTxdSlot("particle");
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(particleSlot);
    if (!gpWhiteTexture) {
        gpWhiteTexture = RwTextureRead("white", nullptr);
        RwTextureGetName(gpWhiteTexture)[0] = '@';
        RwTextureSetFilterMode(gpWhiteTexture, RwTextureFilterMode::rwFILTERLINEAR);
    }
    CTxdStore::PopCurrentTxd();
}

void CVehicleModelInfo::SetHandlingId(const char* handlingName) {
    m_nHandlingId = gHandlingDataMgr.GetHandlingId(handlingName);
}

tHandlingData& CVehicleModelInfo::GetHandlingData() const {
    return gHandlingDataMgr.m_aVehicleHandling[m_nHandlingId];
}

tFlyingHandlingData& CVehicleModelInfo::GetFlyingHandlingData() const {
    return *gHandlingDataMgr.GetFlyingPointer(static_cast<uint8>(m_nHandlingId));
}

void CVehicleModelInfo::CLinkedUpgradeList::AddUpgradeLink(int16 upgrade1, int16 upgrade2)
{
    m_anUpgrade1[m_nLinksCount] = upgrade1;
    m_anUpgrade2[m_nLinksCount] = upgrade2;
    ++m_nLinksCount;
}

int16 CVehicleModelInfo::CLinkedUpgradeList::FindOtherUpgrade(int16 upgrade)
{
    if (!m_nLinksCount)
        return -1;

    for (int32 i = m_nLinksCount - 1; i >= 0; --i) {
        if (m_anUpgrade1[i] == upgrade)
            return m_anUpgrade2[i];

        if (m_anUpgrade2[i] == upgrade)
            return m_anUpgrade1[i];
    }

    return -1;
}

// 0x4C8D60
CVehicleModelInfo::CVehicleStructure::CVehicleStructure()
{
    // Como el original (0x4C8D60): el constructor de cada UpgradePosnDesc (0x4C73F0) no escribe nada; solo se ponen a 0 los
    // dummies, el padre de cada upgrade a -1, los extras, su contador y la máscara (sin inicializar m_aUpgrades por valor)
    for (auto& vecPos : m_avDummyPos)
        vecPos.Set(0.0F, 0.0F, 0.0F);

    for (auto& upgrade : m_aUpgrades)
        upgrade.m_nParentComponentId = -1;

    m_apExtras.fill(nullptr);
    m_nNumExtras = 0;
    m_nMaskComponentsDamagable = 0;
}

CVehicleModelInfo::CVehicleStructure::~CVehicleStructure()
{
    // Como el original (0x4C7410..0x4C744B): la cuenta es un byte con signo (test al, al; jle / movsx eax, byte ptr [ebp+30Ch])
    for (int32 i = 0; i < static_cast<int8>(m_nNumExtras); ++i) {
        auto atomic = m_apExtras.data()[i];
        auto frame = RpAtomicGetFrame(atomic);
        RpAtomicDestroy(atomic);
        RwFrameDestroy(frame);
    }
}

void* CVehicleModelInfo::CVehicleStructure::operator new(unsigned size)
{
    return m_pInfoPool->New();
}

void CVehicleModelInfo::CVehicleStructure::operator delete(void* data)
{
    m_pInfoPool->Delete(reinterpret_cast<CVehicleStructure*>(data));
}

bool IsValidCompRule(int32 nRule)
{
    return nRule != eComponentsRules::ONLY_WHEN_RAINING
        || CWeather::OldWeatherType == WEATHER_RAINY_COUNTRYSIDE
        || CWeather::NewWeatherType == WEATHER_RAINY_COUNTRYSIDE
        || CWeather::OldWeatherType == WEATHER_RAINY_SF
        || CWeather::NewWeatherType == WEATHER_RAINY_SF;
}

// 0x4C7FB0
int32 ChooseComponent(int32 rule, int32 comps)
{
    // Como el original: las tres ramas llaman a 0x407180 (min + (int)(rand * 1/32768 * (max - min))) = GetRandomIntInRange;
    // la plantilla entera de CGeneral interpola con 1/32767 sobre [min, max - 1] y puede salir desfasada en uno.
    if (rule == eComponentsRules::ALLOW_ALWAYS || rule == eComponentsRules::ONLY_WHEN_RAINING) {
        auto iNumComps = CountCompsInRule(comps);
        auto rand = CGeneral::GetRandomIntInRange(0, iNumComps);
        return (comps >> (4 * rand)) & 0xF;
    }
    else if (rule == eComponentsRules::MAYBE_HIDE) {
        auto iNumComps = CountCompsInRule(comps);
        auto rand = CGeneral::GetRandomIntInRange(-1, iNumComps);
        if (rand == -1)
            return -1;

        return (comps >> (4 * rand)) & 0xF;
    }
    else if (rule == eComponentsRules::FULL_RANDOM) {
        return CGeneral::GetRandomIntInRange(0, 5);
    }
    else
        return -1;
}

// 0x4C7F80
int32 CountCompsInRule(int32 comps)
{
    int32 result = 0;
    while (comps) {
        if ((comps & 0xF) != 0xF)
            ++result;

        comps >>= 4; // Como el original (0x4C7F9B): sar ecx, 4 (desplazamiento aritmético; "/ 16" redondea hacia cero y difiere con negativos)
    }

    return result;
}

int32 GetListOfComponentsNotUsedByRules(uint32 compRules, int32 numExtras, int32* outList)
{
    int32 iCompsList[]{ 0, 1, 2, 3, 4, 5 };
    tVehicleCompsUnion comps{};
    comps.m_nComps = compRules;

    if (comps.nExtraARule && IsValidCompRule(comps.nExtraARule)) {
        if (comps.nExtraARule == eComponentsRules::FULL_RANDOM)
            return 0;

        if (comps.nExtraA_comp1 != 0xF)
            iCompsList[comps.nExtraA_comp1] = 0xF;

        if (comps.nExtraA_comp2 != 0xF)
            iCompsList[comps.nExtraA_comp2] = 0xF;

        if (comps.nExtraA_comp3 != 0xF)
            iCompsList[comps.nExtraA_comp3] = 0xF;
    }

    if (comps.nExtraBRule && IsValidCompRule(comps.nExtraBRule)) {
        if (comps.nExtraB_comp1 != 0xF)
            iCompsList[comps.nExtraB_comp1] = 0xF;

        if (comps.nExtraB_comp2 != 0xF)
            iCompsList[comps.nExtraB_comp2] = 0xF;

        if (comps.nExtraB_comp3 != 0xF)
            iCompsList[comps.nExtraB_comp3] = 0xF;
    }

    auto iNumComps = 0;
    for (int32 i = 0; i < numExtras; ++i) {
        if (iCompsList[i] == 0xF)
            continue;

        outList[iNumComps] = i;
        ++iNumComps;
    }

    return iNumComps;
}

// 0x4C83B0
RpMaterial* RemoveWindowAlphaCB(RpMaterial* material, void* data)
{
    auto* color = RpMaterialGetColor(material);
    if (color->alpha == 255)
        return material;

    auto ppEntries = reinterpret_cast<tRestoreEntry**>(data);
    (*ppEntries)->m_pAddress = color;
    (*ppEntries)->m_pValue = *reinterpret_cast<void**>(color);
    (*ppEntries)++;

    color->red = 0;
    color->green = 0;
    color->blue = 0;
    color->alpha = 0;

    return material;
}

// 0x4C7BD0
RwObject* GetOkAndDamagedAtomicCB(RwObject* object, void* data)
{
    auto out = reinterpret_cast<RwObject**>(data);
    if ((CVisibilityPlugins::GetAtomicId(reinterpret_cast<RpAtomic*>(object)) & ATOMIC_OK) != 0) {
        out[0] = object;
    }
    else if ((CVisibilityPlugins::GetAtomicId(reinterpret_cast<RpAtomic*>(object)) & ATOMIC_DAMAGED) != 0) {
        out[1] = object;
    }

    return object;
}
